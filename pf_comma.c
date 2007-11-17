//***************************************************************************
//  NARS2000 -- Primitive Function -- Comma
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
//  $PrimFnComma_EM
//
//  Primitive function for monadic and dyadic Comma ("ravel/table" and "catenate/laminate")
//***************************************************************************

LPPL_YYSTYPE PrimFnComma_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_COMMA
         || lptkFunc->tkData.tkChar EQ UTF16_COMMABAR);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonComma_EM_YY             (lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydComma_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnComma_EM_YY


//***************************************************************************
//  $PrimProtoFnComma_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Comma
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnComma_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnComma_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnComma_EM_YY,  // Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnComma_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonComma_EM_YY
//
//  Primitive function for monadic Comma ("ravel/table")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonComma_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonComma_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    HGLOBAL hGlbRht;            // Right arg global memory handle

    //***************************************************************
    // Comma-bar is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************

    if (lptkFunc->tkData.tkChar EQ UTF16_COMMABAR
     && lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // Get the global memory handle
                hGlbRht = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbRht));

                break;          // Join common global code
            } // End IF

            // Handle the immediate case
            return PrimFnMonCommaImm_EM_YY
                   (lptkRhtArg->tkData.tkSym->stFlags.ImmType,      // Immediate type
                    lptkRhtArg->tkData.tkSym->stData.stLongest,     // Immediate value
                    lptkAxis,                                       // Ptr to axis token (may be NULL)
                    lptkFunc);                                      // Ptr to function token
        case TKT_VARIMMED:
            return PrimFnMonCommaImm_EM_YY
                   (lptkRhtArg->tkFlags.ImmType,                    // Immediate type
                    lptkRhtArg->tkData.tkLongest,                   // Immediate value
                    lptkAxis,                                       // Ptr to axis token (may be NULL)
                    lptkFunc);                                      // Ptr to function token
        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

            // Get the global memory handle
            hGlbRht = lptkRhtArg->tkData.tkGlbData;

            break;          // Join common global code

        defstop
            return NULL;
    } // End SWITCH

    return PrimFnMonCommaGlb_EM_YY (ClrPtrTypeDirAsGlb (hGlbRht),   // HGLOBAL
                                    lptkAxis,                       // Ptr to axis token (may be NULL)
                                    lptkFunc);                      // Ptr to function token
} // End PrimFnMonComma_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonCommaImm_EM_YY
//
//  Monadic Comma ("ravel/table") on an immediate value.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonCommaImm_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonCommaImm_EM_YY
    (IMM_TYPES     immType,         // Right arg Immediate type (see IMM_TYPES)
     APLLONGEST    aplLongest,      // Ptr to right arg value
     LPTOKEN       lptkAxis,        // Ptr to axis token (may be NULL)
     LPTOKEN       lptkFunc)        // Ptr to function token

{
    APLSTYPE     aplTypeRes;        // Result storage type
    APLRANK      aplRankRes;        // Result rank
    HGLOBAL      hGlbRes;           // Result global memory handle
    LPVOID       lpMemRes;          // Ptr to result global memory
    BOOL         bFract = FALSE,    // TRUE iff axis has fractional values
                 bTableRes;         // TRUE iff function is UTF16_COMMABAR
    APLUINT      ByteRes;           // # bytes in the result
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Check for axis present
    while (lptkAxis NE NULL)
    {
        // Ravel with axis on a simple scalar requires
        //   that the axis be an empty vector
        if (CheckAxis_EM (lptkAxis,         // The axis token
                          0,                // All values less than this
                          FALSE,            // TRUE iff scalar or one-element vector only
                          FALSE,            // TRUE iff want sorted axes
                          TRUE,             // TRUE iff axes must be contiguous
                          FALSE,            // TRUE iff duplicate axes are allowed
                          NULL,             // Return TRUE iff fractional values present
                          NULL,             // Return last axis value
                          NULL,             // Return # elements in axis vector
                          NULL))            // Return HGLOBAL with APLINT axis values
            break;
        //   or a singleton fractional value
        if (CheckAxis_EM (lptkAxis,         // The axis token
                          1,                // All values less than this
                          TRUE,             // TRUE iff scalar or one-element vector only
                          FALSE,            // TRUE iff want sorted axes
                          TRUE,             // TRUE iff axes must be contiguous
                          FALSE,            // TRUE iff duplicate axes are allowed
                         &bFract,           // Return TRUE iff fractional values present
                          NULL,             // Return last axis value
                          NULL,             // Return # elements in axis vector
                          NULL)             // Return HGLOBAL with APLINT axis values
         && bFract)
            break;
        // Otherwise, it's an AXIS ERROR
        return NULL;
    } // End WHILE

    //***************************************************************
    // If the function is Comma-bar (table), the result is
    //   1 1 {rho} R
    //***************************************************************
    bTableRes = (lptkFunc->tkData.tkChar EQ UTF16_COMMABAR);
    aplRankRes = 1 + bTableRes;

    // Set the result storage type
    aplTypeRes = TranslateImmTypeToArrayType (immType);

    //***************************************************************
    // Calculate space needed for the result
    //***************************************************************
    ByteRes = CalcArraySize (aplTypeRes, 1, aplRankRes);

    //***************************************************************
    // Now we can allocate the storage for the result
    // N.B.:  Conversion from APLUINT to UINT.
    //***************************************************************
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;               // Already zero from GHND
////lpHeader->SysVar     = 0;               // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = 1;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Fill in the result's dimension
    *VarArrayBaseToDim (lpMemRes) = 1;
    if (bTableRes)
        *(1 + VarArrayBaseToDim (lpMemRes)) = 1;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    // Copy the immediate value to the result
    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:
            *((LPAPLBOOL)  lpMemRes) = (APLBOOL)  aplLongest;

            break;

        case ARRAY_INT:
            *((LPAPLINT)   lpMemRes) = (APLINT)   aplLongest;

            break;

        case ARRAY_FLOAT:
            *((LPAPLFLOAT) lpMemRes) = *(LPAPLFLOAT) &aplLongest;

            break;

        case ARRAY_CHAR:
            *((LPAPLCHAR)  lpMemRes) = (APLCHAR)  aplLongest;

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeAsGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End PrimFnMonCommaImm_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonCommaGlb_EM_YY
//
//  Monadic Comma ("ravel/table") on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonCommaGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonCommaGlb_EM_YY
    (HGLOBAL       hGlbRht,         // Handle to right arg
     LPTOKEN       lptkAxis,        // Ptr to axis token (may be NULL)
     LPTOKEN       lptkFunc)        // Ptr to function token

{
    HGLOBAL      hGlbRes = NULL,    // Result global memory handle
                 hGlbAxis = NULL,   // Axis ...
                 hGlbOdo = NULL,    // Odometer ...
                 hGlbWVec = NULL;   // Weighting vector ...
    LPVOID       lpMemRht = NULL,   // Ptr to right arg global memory
                 lpMemRes = NULL;   // Ptr to result    ...
    LPAPLDIM     lpMemDimRht = NULL;// Ptr to right arg dimensions
    LPAPLUINT    lpMemAxis = NULL,  // Ptr to axis global memory
                 lpMemGrUp = NULL,  // Ptr to grade up ...
                 lpMemOdo = NULL,   // Ptr to odometer ...
                 lpMemWVec = NULL;  // Ptr to weighting vector ...
    APLUINT      ByteRes;           // # bytes in the result
    APLNELM      aplNELMRht,        // Right arg NELM
                 aplNELMAxis;       // Axis      ...
    APLRANK      aplRankRht,        // Right arg rank
                 aplRankRes;        // Result    ...
    APLNELM      uRht,              // Right arg loop counter
                 uRes,              // Result    ...
                 uOdo;              // Odometer  ...
    APLDIM       aplDimNew;         //
    APLSTYPE     aplTypeRht,        // Right arg storage type
                 aplTypeRes;        // Result    ...
    APLNELMSIGN  iRht;              // Right arg loop counter
    APLINT       apaOffRht,         // Right arg APA offset
                 apaMulRht;         // ...           multiplier
    APLUINT      aplFirstAxis,      // First axis value (if contiguous, then lowest)
                 aplLastAxis;       // Last ...                              highest
    BOOL         bFract = FALSE,    // TRUE iff axis has fractional values
                 bTableRes,         // TRUE iff function is UTF16_COMMABAR
                 bRet = TRUE,       // TRUE iff result is valid
                 bReorder = FALSE;  // TRUE iff result values are reordered
                                    //   from those in the right arg
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Get the rank of the right arg
    aplRankRht = RankOfGlb (hGlbRht);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        while (TRUE)
        {
            // Ravel/table with axis requires
            //   that the axis be a scalar or vector subset of
            //   {iota}aplRankRht
            if (CheckAxis_EM (lptkAxis,         // The axis token
                              aplRankRht,       // All values less than this
                              FALSE,            // TRUE iff scalar or one-element vector only
                              FALSE,            // TRUE iff want sorted axes
                              TRUE,             // TRUE iff axes must be contiguous
                              FALSE,            // TRUE iff duplicate axes are allowed
                              NULL,             // Return TRUE iff fractional values present
                             &aplLastAxis,      // Return last axis value
                             &aplNELMAxis,      // Return # elements in axis vector
                             &hGlbAxis))        // Return HGLOBAL with APLINT axis values
                break;
            //   or a singleton fractional value
            if (CheckAxis_EM (lptkAxis,         // The axis token
                              aplRankRht + 1,   // All values less than this
                              TRUE,             // TRUE iff scalar or one-element vector only
                              FALSE,            // TRUE iff want sorted axes
                              TRUE,             // TRUE iff axes must be contiguous
                              FALSE,            // TRUE iff duplicate axes are allowed
                             &bFract,           // Return TRUE iff fractional values present
                             &aplLastAxis,      // Return last axis value
                             &aplNELMAxis,      // Return # elements in axis vector
                             &hGlbAxis)         // Return HGLOBAL with APLINT axis values
             && bFract)
                break;
            // Otherwise, it's an AXIS ERROR
            return NULL;
        } // End WHILE
    } else
    {
        // No axis means ravel/table all dimensions
        aplNELMAxis = aplRankRht;
        aplLastAxis = aplRankRht - 1;
    } // End IF/ELSE

    //***************************************************************
    // If the function is Comma-bar (table), the result is a
    //   matrix equivalent to
    //     ,[1{drop}{iota}{rho}{rho}R] R
    //***************************************************************
    bTableRes = (lptkFunc->tkData.tkChar EQ UTF16_COMMABAR);
    if (bTableRes)
    {
        aplLastAxis = aplRankRht;
        aplNELMAxis = aplRankRht - 1;
    } else
    // Empty axis means insert new last unit coordinate
    if (aplNELMAxis EQ 0)
        aplLastAxis = aplRankRht;

    // Set the value of the first axis
    // Note that for contiguous axes, <aplLastAxis> is
    //   the highest axis value, and <aplFirstAxis> is
    //   the lowest axis value, inclusive.
    // For example, for ,[1 3 2] (in origin-0)
    //   aplFirstAxis is 1, and
    //   aplLastAxis  is 3
    aplFirstAxis = 1 + aplLastAxis - aplNELMAxis;

    //***************************************************************
    // hGlbAxis is not-NULL iff there is an axis
    // aplNELMAxis contains the # elements in the axis.
    //***************************************************************

    // Calculate the rank of the result
    if (bFract)
        aplRankRes = aplRankRht + 1;
    else
        aplRankRes = aplRankRht + 1 - aplNELMAxis;

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)
    // Get the Array Type and NELM
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Check to see if there is reordering
    if (hGlbAxis)
    {
        // Lock the memory to get a ptr to it
        lpMemAxis = MyGlobalLock (hGlbAxis);

        // Point to the grade-up of the first
        //   <aplRankRht> values in lpMemAxis
        lpMemGrUp = &lpMemAxis[aplRankRht];

        // Get the first axis element + 1
        uRht = lpMemAxis[aplRankRht - aplNELMAxis] + 1;

        for (uRes = aplRankRht + 1 - aplNELMAxis; (!bReorder) && uRes < aplRankRht; uRes++, uRht++)
            bReorder = (uRht NE lpMemAxis[uRes]);
    } // End IF

    //***************************************************************
    // The NELM of the result is the same as that of the right arg
    //   as we're only re-arranging the right arg.
    //***************************************************************

    //***************************************************************
    // The three cases to consider are as follows:
    //  1.  X is fractional
    //      Insert a unit coordinate to the left of aplLastAxis.
    //  2.  X is empty
    //      Append a unit coordinate as the new last axis.
    //  3.  X is a scalar or vector of integers
    //      Transpose R according to X, and collapse the
    //      X coordinates to a single coordinate.
    //***************************************************************

    // If we're reordering and the right arg is ARRAY_APA,
    //   calculate the array size based upon APLINTs
    if (bReorder && aplTypeRht EQ ARRAY_APA)
    {
        // Set the array storage type for the result
        aplTypeRes = ARRAY_INT;

        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRes);
    } else
    {
        // Set the array storage type for the result
        aplTypeRes = aplTypeRht;

        // The # bytes in the result is the same as the right argument
        //   minus its dimensions plus the result's dimensions
        ByteRes = MyGlobalSize (hGlbRht)
                + sizeof (APLDIM) * (aplRankRes - aplRankRht);
    } // End IF/ELSE

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

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes     ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeaderRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeaderRes->ArrType    = aplTypeRes;
////lpHeaderRes->Perm       = 0;            // Already zero from GHND
////lpHeaderRes->SysVar     = 0;            // Already zero from GHND
    lpHeaderRes->RefCnt     = 1;
    lpHeaderRes->NELM       = aplNELMRht;
    lpHeaderRes->Rank       = aplRankRes;
#undef  lpHeaderRes

    // Calculate the length of the raveled dimension
    if (bFract)
        aplDimNew = 1;
    else
    for (uRes = aplFirstAxis, aplDimNew = 1; uRes <= aplLastAxis; uRes++)
        aplDimNew *= lpMemDimRht[uRes];

    // Point to the result's dimension
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    //***************************************************************
    // Copy the appropriate dimensions from the right arg
    //   to the result's dimension
    //***************************************************************
    if (bTableRes)
    {
        // Save the dimensions
        ((LPAPLDIM) lpMemRes)[0] = (aplRankRht EQ 0) ? 1 : lpMemDimRht[0];
        ((LPAPLDIM) lpMemRes)[1] = aplDimNew;
    } else
    // If we're collapsing dimensions, ...
    if (aplRankRes < aplRankRht)
    {
        for (uRes = uRht = 0; uRht < aplRankRht; uRht++)
        {
            // If it's not an axis dimension, ...
            if (uRht < aplFirstAxis
             || uRht > aplLastAxis)
                ((LPAPLDIM) lpMemRes)[uRes++] = lpMemDimRht[uRht];
            else
            // If, it's the last (highest) axis dimension, ...
            if (uRht EQ aplLastAxis)
                ((LPAPLDIM) lpMemRes)[uRes++] = aplDimNew;
        } // End FOR

        // Otherwise, it's the last (highest) axis dimension, ...
        if (uRht EQ aplLastAxis)
            ((LPAPLDIM) lpMemRes)[uRes++] = aplDimNew;
    } else
    // If we're inserting a unit coordinate, ...
    if (aplRankRes > aplRankRht)
    {
        // Insert the unit coordinate
        ((LPAPLDIM) lpMemRes)[aplLastAxis] = 1;

        for (uRes = uRht = 0; uRht < aplRankRht; uRes++)
        {
            // If it's not the last axis value, ...
            if (uRes NE aplLastAxis)
                ((LPAPLDIM) lpMemRes)[uRes] = lpMemDimRht[uRht++];
        } // End FOR
    } else
    {
        for (uRes = uRht = 0; uRht < aplRankRht; uRes++)
            ((LPAPLDIM) lpMemRes)[uRes] = lpMemDimRht[uRht++];
    } // End IF/ELSE

    // Point to the result's data
    lpMemRes = &((LPAPLDIM) lpMemRes)[aplRankRes];

    // As a shortcut, if there's no reordering we can copy the
    //   data from the right arg to the result
    if (!bReorder)
    {
        APLNELM aplNELM;

        // We're about to copy the entries from the right arg
        //   into the result.  If the right arg is ARRAY_NESTED,
        //   we need to increment each HGLOBAL's reference count.
        if (aplTypeRht EQ ARRAY_NESTED)
        {
            // In case the right arg is empty, include its prototype
            aplNELM = max (aplNELMRht, 1);

            // Loop through the right arg
            for (uRht = 0; uRht < aplNELM; uRht++)
                IncrRefCntDir (((LPAPLNESTED) lpMemRht)[uRht]);
        } // End IF

        // Account for the header and dimensions
        ByteRes -= sizeof (VARARRAY_HEADER)
                 + sizeof (APLDIM) * aplRankRes;
        CopyMemory (lpMemRes, lpMemRht, (UINT) ByteRes);
    } else
    // Reorder the right arg into the result
    {
        // If the right arg is an APA, ...
        if (aplTypeRht EQ ARRAY_APA)
        {
#define lpAPA       ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOffRht = lpAPA->Off;
            apaMulRht = lpAPA->Mul;
#undef  lpAPA
        } // End IF

        //***************************************************************
        // Allocate space for the weighting vector which is
        //   {times}{backscan}1{drop}({rho}R),1
        // N.B.  Conversion from APLUINT to UINT.
        //***************************************************************
        ByteRes = aplRankRht * sizeof (APLUINT);
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
        // N.B.  Conversion from APLUINT to UINT.
        //***************************************************************
        ByteRes = aplRankRht * sizeof (APLUINT);
        Assert (ByteRes EQ (UINT) ByteRes);
        hGlbOdo = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (!hGlbOdo)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkFunc);
            bRet = FALSE;

            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemOdo = MyGlobalLock (hGlbOdo);

        // Split cases based upon the storage type of the right arg/result
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    UINT uBitMask;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);

                    uBitMask = 1 << (MASKLOG2NBIB & (UINT) uRht);

                    // Copy element # uRht from the right arg to lpMemRes[uRes]
                    ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |=
                    ((uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? 1 : 0) << (MASKLOG2NBIB & (UINT) uRes);
                } // End FOR

                break;

            case ARRAY_INT:
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemRes[uRes]
                    ((LPAPLINT) lpMemRes)[uRes] = ((LPAPLINT) lpMemRht)[uRht];
                } // End FOR

                break;

            case ARRAY_FLOAT:
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemRes[uRes]
                    ((LPAPLFLOAT) lpMemRes)[uRes] = ((LPAPLFLOAT) lpMemRht)[uRht];
                } // End FOR

                break;

            case ARRAY_CHAR:
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemRes[uRes]
                    ((LPAPLCHAR) lpMemRes)[uRes] = ((LPAPLCHAR) lpMemRht)[uRht];
                } // End FOR

                break;

            case ARRAY_HETERO:
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemRes[uRes]
                    // Note that APLHETERO elements are LPSYMENTRYs, so there's no
                    //   reference count to increment, or other special handling.
                    ((LPAPLHETERO) lpMemRes)[uRes] = ((LPAPLHETERO) lpMemRht)[uRht];
                } // End FOR

                break;

            case ARRAY_NESTED:
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemRes[uRes]
                    // Note that APLNESTED elements are a mixture of LPSYMENTRYs
                    //   and HGLOBALs, so we need to run the HGLOBALs through
                    //   CopySymGlbDir so as to increment the reference count.
                    ((LPAPLNESTED) lpMemRes)[uRes] = CopySymGlbDir (((LPAPLNESTED) lpMemRht)[uRht]);
                } // End FOR

                break;

            case ARRAY_APA:
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemRes[uRes]
                    ((LPAPLINT) lpMemRes)[uRes] = apaOffRht + apaMulRht * uRht;
                } // End FOR

                break;

            defstop
                break;
        } // End SWITCH
    } // End IF/ELSE

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeAsGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
ERROR_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht);  lpMemRht  = NULL;
    } // End IF

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

    if (bRet)
        return lpYYRes;
    else
        return NULL;
} // End PrimFnMonCommaGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydComma_EM_YY
//
//  Primitive function for dyadic Comma ("catenate/laminate")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydComma_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydComma_EM_YY
    (LPTOKEN       lptkLftArg,      // Ptr to left arg token
     LPTOKEN       lptkFunc,        // Ptr to function token
     LPTOKEN       lptkRhtArg,      // Ptr to right arg token
     LPTOKEN       lptkAxis)        // Ptr to axis token (may be NULL)

{
    APLUINT      aplAxis,           // The (one and only) axis value
                 uLft,
                 uRht,
                 uBeg,
                 uEnd,
                 uEndLft,
                 uEndRht,
                 ByteRes;
    APLRANK      aplRankLft,        // The rank of the left arg
                 aplRankRht,        // ...             right ...
                 aplRankRes,        // ...             result
                 aplRankTmp;
    APLSTYPE     aplTypeLft,
                 aplTypeRht,
                 aplTypeRes;
    APLNELM      aplNELMLft,
                 aplNELMRht,
                 aplNELMRes;
    HGLOBAL      hGlbLft = NULL,
                 hGlbRht = NULL,
                 hGlbRes = NULL;
    LPVOID       lpMemLft = NULL,
                 lpMemRht = NULL,
                 lpMemRes = NULL,
                 lpSymGlbLft,
                 lpSymGlbRht;
    LPAPLDIM     lpMemDimLft,
                 lpMemDimRht,
                 lpMemDimRes,
                 lpMemDimDir;
    APLDIM       aplDimTmp,
                 aplDimBeg,
                 aplDimLftEnd,
                 aplDimRhtEnd,
                 aplDim1 = 1;
    BOOL         bRet = TRUE,
                 bFract = FALSE;
    UINT         uBitMaskLft,
                 uBitMaskRht,
                 uBitIndexRes;
    APLINT       aplIntegerLft,
                 aplIntegerRht,
                 apaOffLft,
                 apaOffRht,
                 apaMulLft,
                 apaMulRht;
    APLFLOAT     aplFloatLft,
                 aplFloatRht;
    APLCHAR      aplCharLft,
                 aplCharRht;
    APLLONGEST   aplVal;
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Both arguments are within bounds
    Assert (aplTypeLft < ARRAY_LENGTH);
    Assert (aplTypeRht < ARRAY_LENGTH);

    // Get the respective first values
    if (aplRankLft EQ 0                         // Scalar
     && aplNELMLft NE 0                         // and non-empty
     && aplTypeLft NE ARRAY_NESTED)             // and non-nested
        GetFirstValueToken (lptkLftArg,         // Ptr to left arg token
                           &aplIntegerLft,      // Ptr to integer result
                           &aplFloatLft,        // Ptr to float ...
                           &aplCharLft,         // Ptr to WCHAR ...
                            NULL,               // Ptr to longest ...
                            NULL,               // Ptr to lpSym/Glb ...
                            NULL,               // Ptr to ...immediate type ...
                            NULL);              // Ptr to array type ...
    else                                        // otherwise,
    if ((aplRankLft EQ 0 || aplNELMLft EQ 0)    // Scalar or empty
     && aplTypeLft EQ ARRAY_NESTED)             // and nested
        GetFirstValueToken (lptkLftArg,         // Ptr to left arg token
                            NULL,               // Ptr to integer result
                            NULL,               // Ptr to float ...
                            NULL,               // Ptr to WCHAR ...
                            NULL,               // Ptr to longest ...
                           &lpSymGlbLft,        // Ptr to lpSym/Glb ...
                            NULL,               // Ptr to ...immediate type ...
                            NULL);              // Ptr to array type ...
    if (aplRankRht EQ 0                         // Scalar
     && aplNELMRht NE 0                         // and non-empty
     && aplTypeRht NE ARRAY_NESTED)             // and non-nested
        GetFirstValueToken (lptkRhtArg,         // Ptr to right arg token
                           &aplIntegerRht,      // Ptr to integer result
                           &aplFloatRht,        // Ptr to float ...
                           &aplCharRht,         // Ptr to WCHAR ...
                            NULL,               // Ptr to longest ...
                            NULL,               // Ptr to lpSym/Glb ...
                            NULL,               // Ptr to ...immediate type ...
                            NULL);              // Ptr to array type ...
    else                                        // otherwise,
    if ((aplRankRht EQ 0 || aplNELMRht EQ 0)    // Scalar or empty
     && aplTypeRht EQ ARRAY_NESTED)             // and nested
        GetFirstValueToken (lptkRhtArg,         // Ptr to right arg token
                            NULL,               // Ptr to integer result
                            NULL,               // Ptr to float ...
                            NULL,               // Ptr to WCHAR ...
                            NULL,               // Ptr to longest ...
                           &lpSymGlbRht,        // Ptr to lpSym/Glb ...
                            NULL,               // Ptr to ...immediate type ...
                            NULL);              // Ptr to array type ...
    // The rank of the result is the larger of the two args
    aplRankRes = max (aplRankLft, aplRankRht);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        while (TRUE)
        {
            // Catentate allows integer axis values only
            if (CheckAxis_EM (lptkAxis,         // The axis token
                              aplRankRes,       // All values less than this
                              TRUE,             // TRUE iff scalar or one-element vector only
                              FALSE,            // TRUE iff want sorted axes
                              FALSE,            // TRUE iff axes must be contiguous
                              FALSE,            // TRUE iff duplicate axes are allowed
                              NULL,             // Return TRUE iff fractional values present
                             &aplAxis,          // Return last axis value
                              NULL,             // Return # elements in axis vector
                              NULL))            // Return HGLOBAL with APLINT axis values
                break;
            // Laminate allows fractional values
            if (CheckAxis_EM (lptkAxis,         // The axis token
                              aplRankRes + 1,   // All values less than this
                              TRUE,             // TRUE iff scalar or one-element vector only
                              FALSE,            // TRUE iff want sorted axes
                              FALSE,            // TRUE iff axes must be contiguous
                              FALSE,            // TRUE iff duplicate axes are allowed
                             &bFract,           // Return TRUE iff fractional values present
                             &aplAxis,          // Return last axis value
                              NULL,             // Return # elements in axis vector
                              NULL)             // Return HGLOBAL with APLINT axis values
             && bFract)
                break;
            // Otherwise, it's an AXIS ERROR
            return NULL;
        } // End WHILE
    } else
    {
        // No axis specified:
        //   if comma, use last dimension
        if (lptkFunc->tkData.tkChar EQ UTF16_COMMA)
            aplAxis = max (0, (APLRANKSIGN) aplRankRes - 1);
        else
            // Otherwise, it's CommaBar on the first dimension
            aplAxis = 0;
    } // End IF/ELSE

    // Left and right scalar args are laminated
    if (aplRankLft EQ 0
     && aplRankRht EQ 0)
    {
        bFract = TRUE;
        aplAxis = 0;
    } // End IF

    // If it's laminate, add one dimension
    aplRankRes += bFract;

    //***************************************************************
    // Check for RANK & LENGTH ERRORs.
    // If either argument is a scalar, no error.
    // Catenate:
    //      The arguments may differ in rank by 0 or 1 only.
    //      If the arguments differ in rank by 1, then the shape of the
    //        arg of larger rank without the axis dimension must be the
    //        same as that of shape of the arg of the smaller rank.
    // Laminate:
    //      The arguments must be of the same rank and shape.
    //***************************************************************

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If neither arg is a scalar, ...
    if (aplRankLft NE 0
     && aplRankRht NE 0)
    {
        // Skip over the headers to the dimensions
        lpMemDimLft = VarArrayBaseToDim (lpMemLft);
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

        // If the ranks differ, ...
        if (aplRankLft NE aplRankRht)
        {
            // If laminate or the ranks differ by more than 1, ...
            if (bFract || abs64 ((APLRANKSIGN) (aplRankLft - aplRankRht)) NE 1)
            {
                ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                           lptkFunc);
                bRet = FALSE;

                goto ERROR_EXIT;
            } // End IF

            // The shapes must be equal except for aplAxis in the larger rank arg
            for (uRht = uLft = 0; uRht < aplRankRht && uLft < aplRankLft; uLft++, uRht++)
            {
                // If the right arg is the larger rank, ...
                if (aplRankLft < aplRankRht)
                    // If this is the axis dimension, skip it
                    uRht += (uRht EQ aplAxis);
                else
                    // If this is the axis dimension, skip it
                    uLft += (uLft EQ aplAxis);

                // Compare the dimensions
                if (lpMemDimLft[uLft] NE lpMemDimRht[uRht])
                {
                    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                               lptkFunc);
                    bRet = FALSE;

                    goto ERROR_EXIT;
                } // End IF
            } // End FOR
        } else
        // The ranks are the same
        {
            // For catenate, the shapes must be equal except for aplAxis.
            // For laminate, the shapes must be equal.
            for (uRht = 0; uRht < aplRankRht; uRht++)
            if ((bFract || uRht NE aplAxis)             // Laminate or not aplAxis
             && lpMemDimLft[uRht] NE lpMemDimRht[uRht]) // Compare the dimensions
            {
                ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                           lptkFunc);
                bRet = FALSE;

                goto ERROR_EXIT;
            } // End FOR/IF
        } // End IF/ELSE
    } // End IF

    // If left arg is scalar, set dimension to 1
    if (aplRankLft EQ 0)
        lpMemDimLft = &aplDim1;
    else
        // Skip over the header to the dimensions
        lpMemDimLft = VarArrayBaseToDim (lpMemLft);

    // If right arg is scalar, set dimension to 1
    if (aplRankRht EQ 0)
        lpMemDimRht = &aplDim1;
    else
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Get a ptr to the dimensions of the larger (or equal) rank arg
    if (aplRankLft < aplRankRht)
    {
        lpMemDimDir = lpMemDimRht;
        aplRankTmp  = aplRankRht;
    } else
    {
        lpMemDimDir = lpMemDimLft;
        aplRankTmp  = aplRankLft;
    } // End IF/ELSE

    // Calculate the product of the non-axis dimensions
    for (uRht = 0, aplDimTmp = 1; uRht < aplRankTmp; uRht++)
    if (bFract || uRht NE aplAxis)
        aplDimTmp *= lpMemDimDir[uRht];

    // Calculate the NELM of the result
    if (bFract)
        aplNELMRes = aplDimTmp * 2;
    else
    if (aplRankLft NE aplRankRht)
        aplNELMRes = aplDimTmp * (lpMemDimDir[aplAxis] + 1);
    else
        aplNELMRes = aplDimTmp * (lpMemDimLft[aplAxis] + lpMemDimRht[aplAxis]);

    // Determine the storage type of the result based upon
    //   the storage types of the left and right args
    //   as well as the NELMs
    if (aplNELMLft EQ 0)
        aplTypeRes = aplTypeRht;
    else
    if (aplNELMRht EQ 0)
        aplTypeRes = aplTypeLft;
    else
        aplTypeRes = aTypePromote[aplTypeLft][aplTypeRht];

    // If the result is empty, use the prototype of the right arg
    if (aplNELMRes EQ 0)
    {
        // Split cases based upon the right arg's storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
                aplTypeRes = ARRAY_BOOL;

                break;

            case ARRAY_CHAR:
                aplTypeRes = ARRAY_CHAR;

                break;

            case ARRAY_NESTED:
                break;

            case ARRAY_APA:         // Can't happen
            case ARRAY_HETERO:      // Can't happen
            defstop
                break;
        } // End SWITCH
    } // End IF

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    //***************************************************************
    // Now we can allocate the storage for the result
    // N.B.:  Conversion from APLUINT to UINT
    //***************************************************************
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);

    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes     ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeaderRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeaderRes->ArrType    = aplTypeRes;
////lpHeaderRes->Perm       = 0;            // Already zero from GHND
////lpHeaderRes->SysVar     = 0;            // Already zero from GHND
    lpHeaderRes->RefCnt     = 1;
    lpHeaderRes->NELM       = aplNELMRes;
    lpHeaderRes->Rank       = aplRankRes;
#undef  lpHeaderRes

    // Skip over the result header to the dimensions
    lpMemDimRes = VarArrayBaseToDim (lpMemRes);

    // Skip over the result header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    // Copy the dimensions of the larger rank arg
    for (uRht = 0; uRht < aplRankTmp; uRht++)
        lpMemDimRes[uRht + (bFract && (aplAxis <= uRht))] = lpMemDimDir[uRht];

    // Add in the axis dimension
    if (bFract)
        lpMemDimRes[aplAxis] = 2;
    else
    if (aplRankLft NE aplRankRht)
        lpMemDimRes[aplAxis]++;
    else
        lpMemDimRes[aplAxis] = lpMemDimLft[aplAxis] + lpMemDimRht[aplAxis];

    // Calculate the product of the left/right arg dimensions
    //   before the axis
    for (uBeg = 0,
           aplDimBeg = 1;
         uBeg < aplAxis;
         uBeg++)
        aplDimBeg *= lpMemDimRes[uBeg];

    // Calculate the product of the left arg dimensions
    //   at and beyond the axis taking into account
    //   laminate, scalar extension, and rank differing by one
    aplDimLftEnd = 1;
    if (bFract || aplRankLft EQ 0)
        for (uEnd = aplAxis + !bFract;
             uEnd < aplRankRht;
             uEnd++)
            aplDimLftEnd *= lpMemDimRht[uEnd];
    else
        for (uEnd = aplAxis;
             uEnd < aplRankLft;
             uEnd++)
            aplDimLftEnd *= lpMemDimLft[uEnd];

    // Calculate the product of the right arg dimensions
    //   at and beyond the axis taking into account
    //   laminate, scalar extension, and rank differing by one
    aplDimRhtEnd = 1;
    if (bFract || aplRankRht EQ 0)
        for (uEnd = aplAxis + !bFract;
             uEnd < aplRankLft;
             uEnd++)
            aplDimRhtEnd *= lpMemDimLft[uEnd];
    else
        for (uEnd = aplAxis;
             uEnd < aplRankRht;
             uEnd++)
            aplDimRhtEnd *= lpMemDimRht[uEnd];

    // Initialize the bit masks and index for Boolean arguments
    uBitMaskLft = uBitMaskRht = 0x01;
    uBitIndexRes = 0;

    // If the left arg is not a scalar,
    //   skip over its header and dimensions to the data
    //   and get the APA values if appropriate
    if (aplRankLft NE 0)
    {
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

        // If the left arg is an APA, ...
        if (aplTypeLft EQ ARRAY_APA)
        {
#define lpAPA       ((LPAPLAPA) lpMemLft)
            // Get the APA parameters
            apaOffLft = lpAPA->Off;
            apaMulLft = lpAPA->Mul;
#undef  lpAPA
            uEndLft   = 0;
        } // End IF
    } // End IF

    // If the right arg is not a scalar,
    //   skip over its header and dimensions to the data
    //   and get the APA values if appropriate
    if (aplRankRht NE 0)
    {
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

        // If the left arg is an APA, ...
        if (aplTypeRht EQ ARRAY_APA)
        {
#define lpAPA       ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOffRht = lpAPA->Off;
            apaMulRht = lpAPA->Mul;
#undef  lpAPA
            uEndRht   = 0;
        } // End IF
    } // End IF

    // Split cases based upon the result's storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:                // Res = BOOL, Lft/Rht = BOOL
            // If the result is Boolean, then both args are Boolean

            // Loop through the leading dimensions
            for (uBeg = 0; uBeg < aplDimBeg; uBeg++)
            {
                // If the left arg is a scalar, ...
                if (aplRankLft EQ 0)
                    // Loop through the left arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                    {
                        *((LPAPLBOOL) lpMemRes) |= (APLBOOL) (aplIntegerLft << uBitIndexRes);

                        // Increment the result bit index
                        uBitIndexRes++;

                        // Check for end-of-byte
                        if (uBitIndexRes EQ NBIB)
                        {
                            uBitIndexRes = 0;           // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF
                    } // End FOR
                else
                    // Loop through the left arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                    {
                        *((LPAPLBOOL) lpMemRes) |= ((uBitMaskLft & *((LPAPLBOOL) lpMemLft)) ? 1 : 0) << uBitIndexRes;

                        // Increment the result bit index
                        uBitIndexRes++;

                        // Check for end-of-byte
                        if (uBitIndexRes EQ NBIB)
                        {
                            uBitIndexRes = 0;           // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF

                        // Shift over the left bit mask
                        uBitMaskLft <<= 1;

                        // Check for end-of-byte
                        if (uBitMaskLft EQ END_OF_BYTE)
                        {
                            uBitMaskLft = 0x01;         // Start over
                            ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                // If the right arg is a scalar, ...
                if (aplRankRht EQ 0)
                    // Loop through the right arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                    {
                        *((LPAPLBOOL) lpMemRes) |= (APLBOOL) (aplIntegerRht << uBitIndexRes);

                        // Increment the result bit index
                        uBitIndexRes++;

                        // Check for end-of-byte
                        if (uBitIndexRes EQ NBIB)
                        {
                            uBitIndexRes = 0;           // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF
                    } // End FOR
                else
                    // Loop through the right arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                    {
                        *((LPAPLBOOL) lpMemRes) |= ((uBitMaskRht & *((LPAPLBOOL) lpMemRht)) ? 1 : 0) << uBitIndexRes;

                        // Increment the result bit index
                        uBitIndexRes++;

                        // Check for end-of-byte
                        if (uBitIndexRes EQ NBIB)
                        {
                            uBitIndexRes = 0;           // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF

                        // Shift over the right bit mask
                        uBitMaskRht <<= 1;

                        // Check for end-of-byte
                        if (uBitMaskRht EQ END_OF_BYTE)
                        {
                            uBitMaskRht = 0x01;         // Start over
                            ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                        } // End IF
                } // End FOR
            } // End FOR

            break;

        case ARRAY_INT:                 // Res = INT, Lft/Rht = BOOL/INT/APA
            // Loop through the leading dimensions
            for (uBeg = 0; uBeg < aplDimBeg; uBeg++)
            {
                // Split cases based upon the left arg's storage type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:    // Res = INT, Lft = BOOL
                        // If the left arg is a scalar, ...
                        if (aplRankLft EQ 0)
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                                *((LPAPLINT) lpMemRes)++ = aplIntegerLft;
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                *((LPAPLINT) lpMemRes)++ = (uBitMaskLft & *((LPAPLBOOL) lpMemLft)) ? 1 : 0;

                                // Shift over the bit mask
                                uBitMaskLft <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskLft EQ END_OF_BYTE)
                                {
                                    uBitMaskLft = 0x01;         // Start over
                                    ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = INT, Lft = INT
                        // If the left arg is a scalar, ...
                        if (aplRankLft EQ 0)
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                                *((LPAPLINT) lpMemRes)++ = aplIntegerLft;
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                                *((LPAPLINT) lpMemRes)++ = *((LPAPLINT) lpMemLft)++;
                        break;

                    case ARRAY_APA:     // Res = INT, Lft = APA
                        // Loop through the right arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            *((LPAPLINT) lpMemRes)++ = apaOffLft + apaMulLft * uEndLft++;
                        break;

                    defstop
                        break;
                } // End SWITCH

                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Res = INT, Rht = BOOL
                        // If the right arg is a scalar, ...
                        if (aplRankRht EQ 0)
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                                *((LPAPLINT) lpMemRes)++ = aplIntegerRht;
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                *((LPAPLINT) lpMemRes)++ = (uBitMaskRht & *((LPAPLBOOL) lpMemRht)) ? 1 : 0;

                                // Shift over the bit mask
                                uBitMaskRht <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskRht EQ END_OF_BYTE)
                                {
                                    uBitMaskRht = 0x01;         // Start over
                                    ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = INT, Rht = INT
                        // If the right arg is a scalar, ...
                        if (aplRankRht EQ 0)
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                                *((LPAPLINT) lpMemRes)++ = aplIntegerRht;
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                                *((LPAPLINT) lpMemRes)++ = *((LPAPLINT) lpMemRht)++;
                        break;

                    case ARRAY_APA:     // Res = INT, Rht = APA
                        // Loop through the right arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            *((LPAPLINT) lpMemRes)++ = apaOffRht + apaMulRht * uEndRht++;
                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        case ARRAY_FLOAT:               // Res = FLOAT, Lft/Rht = BOOL/INT/APA/FLOAT
            // Loop through the leading dimensions
            for (uBeg = 0; uBeg < aplDimBeg; uBeg++)
            {
                // Split cases based upon the left arg's storage type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:    // Res = FLOAT, Lft = BOOL
                        // If the left arg is a scalar, ...
                        if (aplRankLft EQ 0)
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                                *((LPAPLFLOAT) lpMemRes)++ = aplFloatLft;
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                *((LPAPLFLOAT) lpMemRes)++ = (uBitMaskLft & *((LPAPLBOOL) lpMemLft)) ? 1 : 0;

                                // Shift over the bit mask
                                uBitMaskLft <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskLft EQ END_OF_BYTE)
                                {
                                    uBitMaskLft = 0x01;         // Start over
                                    ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = FLOAT, Lft = INT
                        // If the left arg is a scalar, ...
                        if (aplRankLft EQ 0)
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                                *((LPAPLFLOAT) lpMemRes)++ = aplFloatLft;
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                                *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) *((LPAPLINT) lpMemLft)++;
                        break;

                    case ARRAY_FLOAT:   // Res = FLOAT, Lft = FLOAT
                        // If the left arg is a scalar, ...
                        if (aplRankLft EQ 0)
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                                *((LPAPLFLOAT) lpMemRes)++ = aplFloatLft;
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                                *((LPAPLFLOAT) lpMemRes)++ = *((LPAPLFLOAT) lpMemLft)++;
                        break;

                    case ARRAY_APA:     // Res = FLOAT, Lft = APA
                        // Loop through the left arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) (APLINT) (apaOffLft + apaMulLft * uEndLft++);
                        break;

                    defstop
                        break;
                } // End SWITCH

                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Res = FLOAT, Rht = BOOL
                        // If the right arg is a scalar, ...
                        if (aplRankRht EQ 0)
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                                *((LPAPLFLOAT) lpMemRes)++ = aplFloatRht;
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                *((LPAPLFLOAT) lpMemRes)++ = (uBitMaskRht & *((LPAPLBOOL) lpMemRht)) ? 1 : 0;

                                // Shift over the bit mask
                                uBitMaskRht <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskRht EQ END_OF_BYTE)
                                {
                                    uBitMaskRht = 0x01;         // Start over
                                    ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = FLOAT, Rht = INT
                        // If the right arg is a scalar, ...
                        if (aplRankRht EQ 0)
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                                *((LPAPLFLOAT) lpMemRes)++ = aplFloatRht;
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                                *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) *((LPAPLINT) lpMemRht)++;
                        break;

                    case ARRAY_FLOAT:   // Res = FLOAT, Rht = FLOAT
                        // If the right arg is a scalar, ...
                        if (aplRankRht EQ 0)
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                                *((LPAPLFLOAT) lpMemRes)++ = aplFloatRht;
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                                *((LPAPLFLOAT) lpMemRes)++ = *((LPAPLFLOAT) lpMemRht)++;
                        break;

                    case ARRAY_APA:     // Res = FLOAT, Rht = APA
                        // Loop through the right arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) (APLINT) (apaOffRht + apaMulRht * uEndRht++);
                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        case ARRAY_CHAR:                // Res = CHAR, Lft/Rht = CHAR
            // Loop through the leading dimensions
            for (uBeg = 0; uBeg < aplDimBeg; uBeg++)
            {
                // If the left arg is a scalar, ...
                if (aplRankLft EQ 0)
                    // Loop through the left arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                        *((LPAPLCHAR) lpMemRes)++ = aplCharLft;
                else
                    // Loop through the left arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                        *((LPAPLCHAR) lpMemRes)++ = *((LPAPLCHAR) lpMemLft)++;

                // If the right arg is a scalar, ...
                if (aplRankRht EQ 0)
                    // Loop through the right arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                        *((LPAPLCHAR) lpMemRes)++ = aplCharRht;
                else
                    // Loop through the right arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                        *((LPAPLCHAR) lpMemRes)++ = *((LPAPLCHAR) lpMemRht)++;
            } // End FOR

            break;

        case ARRAY_HETERO:              // Res = HETERO, Lft/Rht = HETERO/Num/CHAR
        case ARRAY_NESTED:              // Res = NESTED, Lft/Rht = NESTED/HETERO/Num/CHAR
            // If the left arg is a simple scalar, ...
            if (aplRankLft EQ 0
             && IsSimpleNH (aplTypeLft))
            {
                // Split cases based upon the left arg storage type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                        aplVal = aplIntegerLft;

                        break;

                    case ARRAY_FLOAT:
                        aplVal = *(LPAPLLONGEST) &aplFloatLft;

                        break;

                    case ARRAY_CHAR:
                        aplVal = aplCharLft;

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Make an LPSYMENTRY out of it
                lpSymGlbLft = MakeSymEntry_EM (TranslateArrayTypeToImmType (aplTypeLft), &aplVal, lptkFunc);
            } // End IF

            // If the right arg is a simple scalar, ...
            if (aplRankRht EQ 0
             && IsSimpleNH (aplTypeRht))
            {
                // Split cases based upon the right arg storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                        aplVal = aplIntegerRht;

                        break;

                    case ARRAY_FLOAT:
                        aplVal = *(LPAPLLONGEST) &aplFloatRht;

                        break;

                    case ARRAY_CHAR:
                        aplVal = aplCharRht;

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Make an LPSYMENTRY out of it
                lpSymGlbRht = MakeSymEntry_EM (TranslateArrayTypeToImmType (aplTypeRht), &aplVal, lptkFunc);
            } // End IF

            // If the result is empty, use the prototype of the right arg
            if (aplNELMRes EQ 0)
                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (lpSymGlbRht);
            else
            // Loop through the leading dimensions
            for (uBeg = 0; uBeg < aplDimBeg; uBeg++)
            {
                // Split cases based upon the left arg's storage type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:    // Res = NESTED, Lft = BOOL
                        // If the left arg is a scalar, ...
                        if (aplRankLft EQ 0)
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (lpSymGlbLft);
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                aplVal = (uBitMaskLft & *((LPAPLBOOL) lpMemLft)) ? 1 : 0;
                                *((LPAPLNESTED) lpMemRes)++ = MakeSymEntry_EM (IMMTYPE_BOOL, &aplVal, lptkFunc);

                                // Shift over the bit mask
                                uBitMaskLft <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskLft EQ END_OF_BYTE)
                                {
                                    uBitMaskLft = 0x01;         // Start over
                                    ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = NESTED, Lft = INT
                        // If the left arg is a scalar, ...
                        if (aplRankLft EQ 0)
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (lpSymGlbLft);
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                aplVal = *((LPAPLINT) lpMemLft)++;
                                *((LPAPLNESTED) lpMemRes)++ = MakeSymEntry_EM (IMMTYPE_INT, &aplVal, lptkFunc);
                            } // End FOR
                        break;

                    case ARRAY_FLOAT:   // Res = NESTED, Lft = FLOAT
                        // If the left arg is a scalar, ...
                        if (aplRankLft EQ 0)
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (lpSymGlbLft);
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                aplVal = *((LPAPLINT) lpMemLft)++;
                                *((LPAPLNESTED) lpMemRes)++ = MakeSymEntry_EM (IMMTYPE_FLOAT, &aplVal, lptkFunc);
                            } // End FOR
                        break;

                    case ARRAY_CHAR:    // Res = NESTED, Lft = CHAR
                        // If the left arg is a scalar, ...
                        if (aplRankLft EQ 0)
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (lpSymGlbLft);
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                aplVal = *((LPAPLCHAR) lpMemLft)++;
                                *((LPAPLNESTED) lpMemRes)++ = MakeSymEntry_EM (IMMTYPE_CHAR, &aplVal, lptkFunc);
                            } // End FOR
                        break;

                    case ARRAY_APA:     // Res = NESTED, Lft = APA
                        // Loop through the left arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                        {
                            aplVal = apaOffLft + apaMulLft * uEndLft++;
                            *((LPAPLNESTED) lpMemRes)++ = MakeSymEntry_EM (IMMTYPE_INT, &aplVal, lptkFunc);
                        } // End FOR

                        break;

                    case ARRAY_HETERO:  // Res = NESTED, Lft = HETERO
                    case ARRAY_NESTED:  // Res = NESTED, Lft = NESTED
                        // If the left arg is a scalar, ...
                        if (aplRankLft EQ 0)
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (lpSymGlbLft);
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (*((LPAPLNESTED) lpMemLft)++);
                        break;

                    defstop
                        break;
                } // End SWITCH

                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Res = NESTED, Rht = BOOL
                        // If the right arg is a scalar, ...
                        if (aplRankRht EQ 0)
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (lpSymGlbRht);
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                aplVal = (uBitMaskRht & *((LPAPLBOOL) lpMemRht)) ? 1 : 0;
                                *((LPAPLNESTED) lpMemRes)++ = MakeSymEntry_EM (IMMTYPE_BOOL, &aplVal, lptkFunc);

                                // Shift over the bit mask
                                uBitMaskRht <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskRht EQ END_OF_BYTE)
                                {
                                    uBitMaskRht = 0x01;         // Start over
                                    ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = NESTED, Rht = INT
                        // If the right arg is a scalar, ...
                        if (aplRankRht EQ 0)
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (lpSymGlbRht);
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                aplVal = *((LPAPLINT) lpMemRht)++;
                                *((LPAPLNESTED) lpMemRes)++ = MakeSymEntry_EM (IMMTYPE_INT, &aplVal, lptkFunc);
                            } // End FOR
                        break;

                    case ARRAY_FLOAT:   // Res = NESTED, Rht = FLOAT
                        // If the right arg is a scalar, ...
                        if (aplRankRht EQ 0)
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (lpSymGlbRht);
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                aplVal = *((LPAPLINT) lpMemRht)++;
                                *((LPAPLNESTED) lpMemRes)++ = MakeSymEntry_EM (IMMTYPE_FLOAT, &aplVal, lptkFunc);
                            } // End FOR
                        break;

                    case ARRAY_CHAR:    // Res = NESTED, Rht = CHAR
                        // If the right arg is a scalar, ...
                        if (aplRankRht EQ 0)
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (lpSymGlbRht);
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                aplVal = *((LPAPLCHAR) lpMemRht)++;
                                *((LPAPLNESTED) lpMemRes)++ = MakeSymEntry_EM (IMMTYPE_CHAR, &aplVal, lptkFunc);
                            } // End FOR
                        break;

                    case ARRAY_APA:     // Res = NESTED, Rht = APA
                        // Loop through the right arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                        {
                            aplVal = apaOffRht + apaMulRht * uEndRht++;
                            *((LPAPLNESTED) lpMemRes)++ = MakeSymEntry_EM (IMMTYPE_INT, &aplVal, lptkFunc);
                        } // End FOR

                        break;

                    case ARRAY_HETERO:  // Res = NESTED, Rht = HETERO
                    case ARRAY_NESTED:  // Res = NESTED, Rht = NESTED
                        // If the right arg is a scalar, ...
                        if (aplRankRht EQ 0)
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (lpSymGlbRht);
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (*((LPAPLNESTED) lpMemRht)++);
                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        case ARRAY_APA:
        defstop
            break;
    } // End SWITCH

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeAsGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
ERROR_EXIT:
    if (lpMemLft)
    {
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    if (lpMemRht)
    {
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (lpMemRes)
    {
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (bRet)
        return lpYYRes;
    else
        return NULL;
} // End PrimFnDydComma_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_comma.c
//***************************************************************************
