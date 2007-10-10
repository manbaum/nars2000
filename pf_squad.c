//***************************************************************************
//  NARS2000 -- Primitive Function -- Squad
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


//***************************************************************************
//  $PrimFnSquad_EM_YY
//
//  Primitive function for monadic and dyadic Squad (ERROR and "rectangular indexing")
//***************************************************************************

LPPL_YYSTYPE PrimFnSquad_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_SQUAD);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonSquad_EM_YY             (lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydSquad_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnSquad_EM_YY


//***************************************************************************
//  $PrimProtoFnSquad_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Squad
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnSquad_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnSquad_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnSquad_EM_YY,  // Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnSquad_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonSquad_EM_YY
//
//  Primitive function for monadic Squad (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonSquad_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonSquad_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnSyntaxError_EM (lptkFunc);
} // End PrimFnMonSquad_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSquad_EM_YY
//
//  Primitive function for dyadic Squad ("rectangular indexing")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSquad_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydSquad_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLLONGEST   aplLongestRht;     // The value of the right arg
    APLSTYPE     aplTypeLft;        // The storage type of the left arg
    APLNELM      aplNELMLft;        // The # elements in the left arg
    APLRANK      aplRankLft;        // The rank of the left arg
    IMM_TYPES    immTypeRht;        // The immediate type of the right arg (see IMM_TYPES)
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

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

                return PrimFnDydSquadGlb_EM_YY
                       (lptkLftArg,                                                     // Ptr to left arg token
                        ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkSym->stData.stGlbData),  // HGLOBAL
                        lptkAxis,                                                       // Ptr to axis token (may be NULL)
                        lptkFunc);                                                      // Ptr to function token
            } // End IF

            // Handle the immediate case

            // Get the immediate value & type
            aplLongestRht = lptkRhtArg->tkData.tkSym->stData.stLongest;
            immTypeRht    = lptkRhtArg->tkData.tkSym->stFlags.ImmType;

            break;

        case TKT_VARIMMED:
            // Get the immediate value & type
            aplLongestRht = lptkRhtArg->tkData.tkLongest;
            immTypeRht    = lptkRhtArg->tkFlags.ImmType;

            break;


        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

            return PrimFnDydSquadGlb_EM_YY
                   (lptkLftArg,                                         // Ptr to left arg token
                    ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkGlbData),    // HGLOBAL
                    lptkAxis,                                           // Ptr to axis token (may be NULL)
                    lptkFunc);                                          // Ptr to function token
        defstop
            return NULL;
    } // End SWITCH

    // Common immediate case, value in <aplLongestRht>

    // The only allowed left arg is an empty vector

    // Get the attributes (Type, NELM, and Rank) of the left arg
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);

    // Check for RANK ERROR
    if (aplRankLft NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Check for LENGTH ERROR
    if (aplNELMLft NE 0)
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Check for DOMAIN ERROR
    if (!IsSimpleNH (aplTypeLft))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = immTypeRht;
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkLongest  = aplLongestRht;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End PrimFnDydSquad_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSquadGlb_EM_YY
//
//  Dyadic Squad ("rectangular indexing") on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSquadGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydSquadGlb_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     HGLOBAL hGlbRht,               // Right arg handle
     LPTOKEN lptkAxis,              // Ptr to axis token (may be NULL)
     LPTOKEN lptkFunc)              // Ptr to function token

{
    APLSTYPE     aplTypeLft,        // Left arg storage type of the left arg
                 aplTypeRht,        // Right ...
                 aplTypeRes;        // Result   ...
    APLNELM      aplNELMLft,        // Left arg NELM
                 aplNELMRht,        // Right ...
                 aplNELMAxis,       // Axis ...
                 aplNELMRes;        // Result   ...
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankRht,        // Right ...
                 aplRankRes;        // Result   ...
    HGLOBAL      hGlbLft = NULL,    // Left arg global memory handle
                 hGlbAxis = NULL,   // Axis ...
                 hGlbRes = NULL,    // Result   ...
                 hGlbOdo = NULL;    // Odometer ...
    LPVOID       lpMemLft = NULL,   // Ptr to left arg global memory
                 lpMemRht = NULL,   // Ptr to right ...
                 lpMemRes = NULL;   // Ptr to result   ...
    APLUINT      uLft,              // Loop counter
                 ByteRes,           // # bytes in the result
                 uWVal,             // Weighting value
                 aplIntAcc,         // Accumulation value
                 uRes;              // Loop counter
    APLINT       iLft,              // Loop counter
                 iAxisNxt;          // Index of next axis value
    LPAPLUINT    lpMemAxis = NULL,  // Ptr to axis global memory
                 lpMemAxisAct,      // Ptr to actual axis values
                 lpMemAxisEli,      // Ptr to elided ...
                 lpMemOdo = NULL,   // Ptr to odometer global memory
                 lpMemLimLft;       // Ptr to left arg limit vector
    LPAPLDIM     lpMemDimRht,       // Ptr to right arg dimensions
                 lpMemDimRes;       // Ptr to result    ...
    APLLONGEST   aplLongestLft;     // Left arg as immediate value
    UINT         uBitMask;          // Bit mask when looping through Booleans
    BOOL         bRet = TRUE;       // TRUE iff result is valid
    APLBOOL      bQuadIO;           // []IO
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfGlb   (hGlbRht   , &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    //***************************************************************
    // Check for axis present
    //***************************************************************
    if (lptkAxis NE NULL)
    {
        // Check the axis values, fill in # elements in axis
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRht,      // All values less than this
                           FALSE,           // TRUE iff scalar or one-element vector only
                           TRUE,            // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // TRUE iff fractional values allowed
                           NULL,            // Return last axis value
                          &aplNELMAxis,     // Return # elements in axis vector
                          &hGlbAxis))       // Return HGLOBAL with APLINT axis values
            goto ERROR_EXIT;
    } else
        aplNELMAxis = aplRankRht;

    // Check for LENGTH ERROR
    if (aplNELMLft NE aplNELMAxis)
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Check for LEFT DOMAIN ERROR
    if (aplTypeLft EQ ARRAY_CHAR
     || aplTypeLft EQ ARRAY_HETERO)
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Calc result storage type
    if (aplTypeRht EQ ARRAY_APA)
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeRht;

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    lpMemRht = MyGlobalLock (hGlbRht);

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Initialize NELM and rank to their identity elements
    aplNELMRes = 1;
    aplRankRes = 0;

    //***************************************************************
    // Trundle through the left arg accumulating the
    //   sum of ranks and product of NELMs
    // Note that for simple NH arrays, the NELM and Rank above are all we need
    //***************************************************************
    if (!IsSimpleNH (aplTypeLft))
    for (uLft = 0; uLft < aplNELMLft; uLft++)
    {
        HGLOBAL    hGlbSub;
        APLLONGEST aplLongestSub;
        IMM_TYPES  immTypeSub;

        // Get next value from the left arg
        GetNextValue (hGlbLft, uLft, &hGlbSub, &aplLongestSub, &immTypeSub);

        // If the left arg item is a global, ...
        if (hGlbSub)
        {
            APLSTYPE aplTypeSub;
            APLNELM  aplNELMSub;
            APLRANK  aplRankSub;

            // Get the global attrs
            AttrsOfGlb (hGlbSub, &aplTypeSub, &aplNELMSub, &aplRankSub, NULL);

            // Ensure the item is simple numeric
            if (!IsSimpleNum (aplTypeSub))
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Accumulate the NELM & rank
            aplNELMRes *= aplNELMSub;
            aplRankRes += aplRankSub;
        } else
        // The left arg item is immediate (in <aplLongestSub> and of type <immTypeSub>)
        {
            // Ensure the left arg item is simple numeric
            if (!IsImmNum (immTypeSub))
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Accumulate the NELM & rank
            aplNELMRes *= 1;            // No action:  compiler will eliminate
            aplRankRes += 0;            // ...
        } // End IF/ELSE
    } // End FOR

    // Handle elided axes
    if (lptkAxis)
    {
        APLUINT uAxis;

        // Lock the memory to get a ptr to it
        lpMemAxis = MyGlobalLock (hGlbAxis);

        // Loop through the elided axes
        for (uAxis = 0; uAxis < (aplRankRht - aplNELMAxis); uAxis++)
        {
            // Accumulate the NELM & rank
            aplNELMRes *= lpMemDimRht[uAxis];
            aplRankRes += 1;
        } // End FOR
    } // End IF

    //***************************************************************
    // Calculate space needed for the result
    //***************************************************************
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

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
        goto ERROR_EXIT;
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
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemDimRes = VarArrayBaseToDim (lpMemRes);

    //***************************************************************
    // Calculate space needed for odometer and limit vectors
    //***************************************************************
    ByteRes = aplRankRht * sizeof (APLUINT) * 2;

    //***************************************************************
    // Now we can allocate the storage for the odometer & limit vectors
    // N.B.:  Conversion from APLUINT to UINT.
    //***************************************************************
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbOdo = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbOdo)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemOdo = MyGlobalLock (hGlbOdo);

    // Calc ptr to limit vector
    lpMemLimLft = &lpMemOdo[aplRankRht];

    // Calc ptr to first of actual axes
    lpMemAxisAct = &lpMemAxis[aplRankRht - aplNELMAxis];

    // Initialize axis index
    iAxisNxt = uLft = 0;

    //***************************************************************
    // Copy dimensions from the right arg or left arg items
    //    to the result
    //***************************************************************
#define uAxisNxt    ((APLUINT) iAxisNxt)
    while (uAxisNxt < aplRankRht)
    {
        if (aplNELMAxis EQ aplRankRht           // No or full axis operator
         || (lpMemAxisAct < lpMemLimLft         //   or ptr is within range
          && uAxisNxt EQ *lpMemAxisAct))        //     and next axis value is an actual one
        {
            HGLOBAL hGlbSub;

            // Skip over the actual axis
            lpMemAxisAct++;

            // If the left arg is a global, ...
            if (hGlbLft)
            {
                // Get next value from the left arg
                GetNextValue (hGlbLft, uLft++, &hGlbSub, NULL, NULL);

                // If the left arg item is a global, ...
                if (hGlbSub)
                {
                    APLNELM  aplNELMSub;
                    APLRANK  aplRankSub;
                    LPAPLDIM lpMemSub;

                    // Get the global attrs
                    AttrsOfGlb (hGlbSub, NULL, &aplNELMSub, &aplRankSub, NULL);

                    // Lock the memory to get a ptr to it
                    lpMemSub = MyGlobalLock (hGlbSub);

                    // Fill in the result's dimension
                    CopyMemory (lpMemDimRes, VarArrayBaseToDim (lpMemSub), (UINT) aplRankSub * sizeof (APLDIM));

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbSub); lpMemSub = NULL;

                    // Skip over the copied dimensions
                    lpMemDimRes += aplRankSub;

                    // Fill in the left arg item limit
                    *lpMemLimLft++ = aplNELMSub;
                } else
                // The left arg item is immediate
                    // Fill in the left arg item limit
                    *lpMemLimLft++ = 1;
            } else
            // The left arg is immediate (in <aplLongestLft> of array type <aplTypeLft>
            {
                // No dimensions to copy for scalar left arg

                // Fill in the left arg item limit
                *lpMemLimLft++ = 1;
            } // End IF/ELSE
        } else  // Elided dimension:  use dimension from right arg
        {
            // Fill in the result's dimension
            *lpMemDimRes++ = lpMemDimRht[uAxisNxt];

            // Fill in the left arg item limit
            *lpMemLimLft++ = lpMemDimRht[uAxisNxt];
        } // End IF/ELSE

        // Skip to next dimension index
        uAxisNxt++;
    } // End WHILE
#undef  uAxisNxt

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    // If the result is nested or hetero, fill the data with PTR_REUSED
    //   in case we fail along the way
    if (aplTypeRes EQ ARRAY_HETERO
     || aplTypeRes EQ ARRAY_NESTED)
    {
        // Fill in first one in case of prototypes
        ((LPAPLNESTED) lpMemRes)[0] = PTR_REUSED;

        // Loop through the rest of the items
        for (uRes = 1; uRes < aplNELMRes; uRes++)
            ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;
    } // End IF

    // Calc ptr to limit vector
    lpMemLimLft = &lpMemOdo[aplRankRht];

    // Calc ptr to last of elided dimensions
    lpMemAxisEli = &lpMemAxis[aplRankRht - aplNELMAxis - 1];

    // Initialize bit mask when looping through Booleans
    uBitMask = 0x01;

    //***************************************************************
    // Calculate each index into the right arg using a weighting value
    //   from the limit vector and copy the corresponding item from
    //   the right arg to the result.
    //***************************************************************
    for (uRes = 0; uRes < aplNELMRes; uRes++)
    {
        HGLOBAL    hGlbSub;
        APLLONGEST aplLongestSub;
        IMM_TYPES  immTypeSub;

        // Calc ptr to last of actual axes
        lpMemAxisAct = &lpMemAxis[aplRankRht - 1];

        // Initialize axis index
        iAxisNxt = aplRankRht - 1;

        // Initialize left arg last index
        iLft = aplNELMLft - 1;

        // Initialize accumulation and weighting values
        aplIntAcc = 0;
        uWVal = 1;

        // Run through the axis values from back to front
        //   accumulating in <aplIntAcc> an index into the right arg
        //   using the weighting value <uWVal>
#define uAxisNxt    ((APLUINT) iAxisNxt)
        while (iAxisNxt >= 0)
        {
            APLLONGEST aplLongestNxt;
            IMM_TYPES  immTypeNxt;

            // Split cases based upon whether or not the dimension is actual or elided
            if (aplNELMAxis EQ aplRankRht       // No or full axis operator
             || (lpMemAxisAct > lpMemAxisEli    //   or ptr is within range
              && uAxisNxt EQ *lpMemAxisAct))    //     and next axis value is an actual one
            {
                // Skip back over the actual axis
                lpMemAxisAct--;

                // If the left arg is a global, ...
                if (hGlbLft)
                {
                    // Get the array of indices from the left arg
                    GetNextValue (hGlbLft, iLft--, &hGlbSub, &aplLongestSub, &immTypeSub);

                    // If the left arg item is a global, ...
                    if (hGlbSub)
                    {
                        // The index value is the <lpMemOdo[iAxisNxt]> value in <hGlbSub>
                        GetNextValue (hGlbSub, lpMemOdo[iAxisNxt], NULL, &aplLongestNxt, &immTypeNxt);

                        // If the index value is float, attempt to convert it to int
                        if (immTypeNxt EQ IMMTYPE_FLOAT)
                        {
                            aplLongestNxt = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestNxt, &bRet);
                            if (!bRet)
                            {
                                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                           lptkFunc);
                                goto ERROR_EXIT;
                            } // End IF
                        } // End IF
                    } else
                    // The left arg item value is immediate
                    //   (in <aplLongestSub> and of immediate type <immTypeSub>)
                    {
                        Assert (lpMemOdo[iAxisNxt] EQ 0);

                        // If the index value is float, attempt to convert it to int
                        if (immTypeSub EQ IMMTYPE_FLOAT)
                        {
                            aplLongestNxt = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestSub, &bRet);
                            if (!bRet)
                            {
                                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                           lptkFunc);
                                goto ERROR_EXIT;
                            } // End IF
                        } else
                            aplLongestNxt = aplLongestSub;
                    } // End IF/ELSE
                } else
                // The left arg item is immediate
                //   (in <aplLongestLft> of array type <aplTypeLft>)
                {
                    Assert (lpMemOdo[iAxisNxt] EQ 0);

                    hGlbSub = NULL;
                    aplLongestSub = aplLongestLft;
                    immTypeSub    = TranslateArrayTypeToImmType (aplTypeLft);

                    // If the index value is float, attempt to convert it to int
                    if (immTypeSub EQ IMMTYPE_FLOAT)
                    {
                        aplLongestNxt = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestSub, &bRet);
                        if (!bRet)
                        {
                            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                       lptkFunc);
                            goto ERROR_EXIT;
                        } // End IF
                    } else
                        aplLongestNxt = aplLongestSub;
                } // End IF/ELSE
            } else  // Elided dimension:  use dimension from right arg
                // Copy dimension index
                aplLongestNxt = lpMemOdo[iAxisNxt];

            // Convert to origin-0
            aplLongestNxt -= bQuadIO;

            // Ensure that the value is within range
            // Note that because <aplLongestNxt> is unsigned,
            //   we don't have to test for negative
            if (aplLongestNxt >= lpMemDimRht[iAxisNxt])
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Accumulate the index
            aplIntAcc += uWVal * aplLongestNxt;

            // Shift over the weighting value
            uWVal *= lpMemDimRht[iAxisNxt];

            // Skip back to next dimension index
            iAxisNxt--;
        } // End WHILE

        // Extract the <aplIntAcc> value from the right arg
        //   and save into the result
        GetNextValue (hGlbRht, aplIntAcc, &hGlbSub, &aplLongestSub, &immTypeSub);

        // If the right arg item is a global, ...
        if (hGlbSub)
            *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDirGlb (hGlbSub);
        else
        // The right arg item is immediate
        //   (in <aplLongestSub> of immediate type <immTypeSub>)
        {
            // Split cases based upon the result storage type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:
                    // Save in result
                    *((LPAPLBOOL) lpMemRes) |= aplLongestSub ? uBitMask : 0;

                    // Shift over the bit mask
                    uBitMask <<= 1;

                    // Check for end-of-byte
                    if (uBitMask EQ END_OF_BYTE)
                    {
                        uBitMask = 0x01;            // Start over
                        ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte

                    } // End IF

                    break;

                case ARRAY_INT:
                    // Save in result
                    *((LPAPLINT) lpMemRes)++ = (APLINT) aplLongestSub;

                    break;

                case ARRAY_FLOAT:
                    // If the right arg item is int, convert it to float
                    if (IsImmInt (immTypeSub))
                        *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) (APLINT) aplLongestSub;
                    else
                        *((LPAPLFLOAT) lpMemRes)++ = *(LPAPLFLOAT) &aplLongestSub;
                    break;

                case ARRAY_CHAR:
                    // Save in result
                    *((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestSub;

                    break;

                case ARRAY_HETERO:
                case ARRAY_NESTED:
                    // Save in result
                    *((LPAPLNESTED) lpMemRes)++ = MakeSymEntry_EM (immTypeSub,
                                                                  &aplLongestSub,
                                                                   lptkFunc);
                    break;

                defstop
                    break;
            } // End SWITCH
        } // End IF/ELSE

        // Increment the odometer in lpMemOdo subject to
        //   the values in lpMemLimLft
        IncrOdometer (lpMemOdo, lpMemLimLft, NULL, aplRankRht);
    } // End FOR

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

    return lpYYRes;
} // End PrimFnDydSquadGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_squad.c
//***************************************************************************
