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
//  $ArrayIndex_EM_YY
//
//  Index of an array, e.g., L[R]  or  L[R1;...;Rn]
//  The form L[R] can use either (or both) Reach and Scatter indexing,
//    whereas the form L[R1;...;Rn] can use Rectangular indexing only.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndex_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ArrayIndex_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    APLSTYPE     aplTypeLft,        // The storage type of the left arg
                 aplTypeRht,        // ...                     right ...
                 aplTypeRch,        // ...                     reach ...
                 aplTypeInd,        // ...                     index ...
                 aplTypeRes;        // ...                     result
    APLNELM      aplNELMLft,        // The # elements in left arg
                 aplNELMRht,        // ...               right ...
                 aplNELMRch,        // ...               reach ...
                 aplNELMInd,        // ...               index ...
                 aplNELMRes;        // ...               result
    APLRANK      aplRankLft,        // The rank of the left arg
                 aplRankRht,        // ...             right ...
                 aplRankRch,        // ...             reach ...
                 aplRankInd,        // ...             index ...
                 aplRankRes,        // ...             result
                 aplRankData;       // ...             data  ...
    HGLOBAL      hGlbLft = NULL,    // Global memory handle to the left arg
                 hGlbRht = NULL,    // ...                         right ...
                 hGlbRes = NULL,    // ...                         result
                 hGlbRch = NULL,    // ...                         reach ...
                 hGlbData;          // ...                         data  ...
    LPVOID       lpMemLft = NULL,   // Ptr to contents of the left arg
                 lpMemRht = NULL,   // ...                    right ...
                 lpMemRch = NULL,   // ...                    reach ...
                 lpMemRes = NULL,   // ...                    result
                 lpMemData;         // ...                    data  ...
    LPAPLDIM     lpMemDimLft;       // Ptr to the dimensions of the left arg
    LPAPLNESTED  lpMemInd;          // Ptr to contents of the index arg
    BOOL         bRet = TRUE;       // TRUE iff result is valid
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result
    APLNELM      uRht,              // Right arg loop counter
                 uDim,              // Dimension ...
                 uRes;              // Result    ...
    APLUINT      ByteRes;           // # bytes in the result

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Map APA left arg to INT result
    if (aplTypeLft EQ ARRAY_APA)
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeLft;

    return PrimFnNonceError_EM (lptkLftArg);

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Skip over the header and dimensions to the data
    lpMemInd = VarArrayBaseToData (lpMemRht, aplRankRht);

    //***************************************************************
    // Calculate the NELM and rank of the result
    //***************************************************************
    if (aplNELMRht EQ 1             // Single array index matches all ranks
     && aplTypeLft EQ ARRAY_NESTED) // and it's Reach Indexing
    {
        // Split cases based upon the ptr type of the right arg
        switch (GetPtrTypeInd (lpMemInd))
        {
            case PTRTYPE_STCONST:
                hGlbRch    = NULL;
                aplNELMRes = 1;
                aplRankRes = 0;

                break;

            case PTRTYPE_HGLOBAL:
                // Get the global memory handle of the single index array
                hGlbRch = ClrPtrTypeIndGlb (lpMemInd);

                // Get the attributes (Type, NELM, and Rank) of the next index array
                AttrsOfGlb (hGlbRch, &aplTypeRch, &aplNELMRch, &aplRankRch, NULL);

                // Save the NELM and rank
                aplNELMRes = aplNELMRch;
                aplRankRes = aplRankRch;

                break;

            defstop
                break;
        } // End SWITCH
    } else
    {
        DbgBrk ();

        // Check for RANK ERROR
        if (aplNELMRht NE aplRankLft)
        {
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkLftArg);
            goto ERROR_EXIT;
        } // End IF

        DbgBrk ();

        // Loop through the elements of the right arg
        aplRankRes = 0; aplNELMRes = 1;
        for (uRht = 0; uRht < aplNELMRht; uRht++, lpMemInd++)
        {
            // Split cases based upon the ptr type of the element
            switch (GetPtrTypeInd (lpMemInd))
            {
                case PTRTYPE_STCONST:       // Skip over scalars
                    break;

                case PTRTYPE_HGLOBAL:
                    // Get the attributes (Type, NELM, and Rank) of the next index array
                    AttrsOfGlb (ClrPtrTypeIndGlb (lpMemInd), &aplTypeInd, &aplNELMInd, &aplRankInd, NULL);

                    // Check for DOMAIN ERROR
                    if (!IsSimpleNum (aplTypeInd))
                    {
                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkLftArg);
                        goto ERROR_EXIT;
                    } // End IF

                    // Accumulate into NELM and rank
                    aplNELMRes *= aplNELMInd;
                    aplRankRes += aplRankInd;

                    break;

                defstop
                    break;
            } // End SWITCH
        } // End FOR
    } // End IF/ELSE

    //***************************************************************
    // Calculate space needed for the result
    //***************************************************************
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    //***************************************************************
    // Allocate space for the result
    // N.B.:  Conversion from APLUINT to UINT.
    //***************************************************************
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkLftArg);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;           // Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    //***************************************************************
    // Copy the appropriate dimensions from the right arg
    //   to the result's dimension
    //***************************************************************
    if (aplNELMRht EQ 1             // Single array index matches all ranks
     && hGlbRch NE NULL
     && aplTypeLft EQ ARRAY_NESTED) // and it's Reach Indexing
    {
        DbgBrk ();

        // Lock the memory to get a ptr to it
        lpMemRch = MyGlobalLock (hGlbRch);

        // Skip over the header to the dimensions
        lpMemRch = VarArrayBaseToDim (lpMemRch);

        // Loop through the dimensions of the right arg
        for (uDim = 0; uDim < aplRankRch; uDim++)
            *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRch)++;

        // lpMemRch now points to the reach arg's data
        // lpMemRes now points to the result's data
    } else
    {
        // Skip over the header and dimensions to the data
        lpMemInd = VarArrayBaseToData (lpMemRht, aplRankRht);

        // Loop through the elements of the right arg
        for (uRht = 0; uRht < aplNELMRht; uRht++, lpMemInd++)
        {
            // Split cases based upon the ptr type of the element
            switch (GetPtrTypeInd (lpMemInd))
            {
                case PTRTYPE_STCONST:       // Skip over scalars
                    break;

                case PTRTYPE_HGLOBAL:
                    // Get the HGLOBAL of the next index array
                    hGlbData = ClrPtrTypeIndGlb (lpMemInd);

                    // Lock the memory to get a ptr to it
                    lpMemData = MyGlobalLock (hGlbData);

                    // Get the rank of the element
                    aplRankData = ((LPVARARRAY_HEADER) lpMemData)->Rank;

                    // Skip over the header to the dimensions
                    lpMemData = VarArrayBaseToDim (lpMemData);

                    // Loop through the dimensions of the element
                    for (uDim = 0; uDim < aplRankData; uDim++)
                        *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemData)++;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbData); lpMemData = NULL;

                    break;

                defstop
                    break;
            } // End SWITCH
        } // End FOR

        // lpMemRes now points to the result's data
    } // End IF/ELSE

    //***************************************************************
    // Fill nested result with PTR_REUSED
    //   in case we fail part way through
    //***************************************************************
    if (aplTypeRes EQ ARRAY_NESTED)
    for (uRes = 0; uRes < aplNELMRes; uRes++)
        ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;

    // Skip over the header to the dimensions
    lpMemDimLft = VarArrayBaseToDim (lpMemLft);

    // Skip over the header and dimensions to the data
    lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

    //***************************************************************
    // Copy the data to the result
    //***************************************************************
    if (aplNELMRht EQ 1             // Single array index matches all ranks
     && hGlbRch NE NULL
     && aplTypeLft EQ ARRAY_NESTED) // and it's Reach Indexing
    {
        DbgBrk ();

        // Loop through the reach arg
        for (uRht = 0;
             bRet && uRht < aplNELMRch;
             uRht++, ((LPAPLNESTED) lpMemRes)++, ((LPAPLNESTED) lpMemRch)++)
            // Process each index vector
            bRet =
            ReachIndex_EM (lpMemRes,                            // Put the result here
                         *(LPAPLNESTED) lpMemRch,               // The current reach arg
                          aplTypeRch,                           // The storage type of the reach arg
                          lpMemLft,                             // Get the results from here
                          aplTypeLft,                           // The storage type of the left arg
                          aplRankLft,                           // The rank of the left arg
                          lpMemDimLft,                          // Ptr to the dimensions of the left arg
                          lptkLftArg);                          // Ptr to "function" token
        if (!bRet)
            goto ERROR_EXIT;
    } else
    {
        DbgBrk ();

        // Rectangular indexing (Scatter indexing if aplNELMRht EQ 1)

        // Split cases based upon the left arg storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:







                break;

            case ARRAY_INT:







                break;

            case ARRAY_FLOAT:







                break;

            case ARRAY_CHAR:







                break;

            case ARRAY_APA:







                break;

            case ARRAY_HETERO:







                break;

            case ARRAY_NESTED:







                break;

            defstop
                break;
        } // End SWITCH
    } // End IF/ELSE

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
    lpYYRes->tkToken.tkCharIndex       = lptkLftArg->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    lpYYRes->tkToken = *TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

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
    if (hGlbRch && lpMemRch)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRch); lpMemRch = NULL;
    } // End IF

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

    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    if (bRet)
        return lpYYRes;
    else
        return NULL;
} // End ArrayIndex_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ReachIndex_EM
//
//  Given an index vector, trundle through its elements extracting
//    the appropriate element from the left arg as this element of the result.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ReachIndex_EM"
#else
#define APPEND_NAME
#endif

BOOL ReachIndex_EM
    (LPAPLNESTED lpMemRes,      // Put the result here
     LPVOID      lpMemNdx,      // The current index vector
     APLSTYPE    aplTypeNdx,    // The storage type of the index vector
     LPVOID      lpMemLft,      // Get the results from here
     APLSTYPE    aplTypeLft,    // The storage type of the left arg
     APLRANK     aplRankLft,    // The rank of the left arg
     LPAPLDIM    lpMemDimLft,   // Ptr to the dimensions of the left arg
     LPTOKEN     lptkFunc)      // Pt to function token

{
    BOOL       bRet = TRUE;
    APLSTYPE   aplTypeSub;
    APLNELM    aplNELMSub;
    APLRANK    aplRankSub;

    DbgBrk ();

    // Split cases based upon the ptr type of the index vector
    switch (GetPtrTypeInd (lpMemNdx))
    {
        case PTRTYPE_STCONST:
            bRet =
            ReachSymTabConst_EM (lpMemRes,          // Put the result here
                                 lpMemNdx,          // Ptr to the symbol table const (a scalar)
                                 lpMemLft,          // Get the results from here
                                 aplTypeLft,        // The storage type of the left arg
                                 aplRankLft,        // The rank of the left arg
                                 lpMemDimLft,       // Ptr to the dimensions of the left arg
                                 lptkFunc);         // Ptr to the function token
            break;

#undef  lpSymRch

        case PTRTYPE_HGLOBAL:
            // The global should point to a scalar or a vector of scalars or vectors
            //   which serves as the reach index into the left arg

            // Get the attributes (Type, NELM, and Rank) of the index vector
            AttrsOfGlb (ClrPtrTypeIndGlb (lpMemNdx), &aplTypeSub, &aplNELMSub, &aplRankSub, NULL);

            // Check for RANK ERROR
            if (aplRankSub > 1)
            {
                ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                                           lptkFunc);
                bRet = FALSE;
            } else
            {
                DbgBrk ();

                // Recurse through the elements of the index vector
                //   and the left arg at the same time using successive
                //   elements of the index vector as indices into the left arg.
                bRet =
                ReachDown_EM (lpMemRes,                 // Put the result here
                              *(LPAPLNESTED) lpMemNdx,  // Ptr to the reach vector w/aplNELMSub elements
                              aplTypeSub,               // The storage type of the reach vector
                              aplNELMSub,               // The NELM of the reach vector
                              lpMemLft,                 // Get the results from here
                              aplTypeLft,               // The storage type of the left arg
                              aplRankLft,               // The rank of the left arg
                              lpMemDimLft,              // Ptr to the dimensions of the left arg
                              lptkFunc);                // Ptr to function token
            } // End IF/ELSE

            break;

        defstop
            break;
    } // End SWITCH

    return bRet;
} // End ReachIndex_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ReachSymTabConst_EM
//
//  Index the left arg with a symbol table constant
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ReachSymTabConst_EM"
#else
#define APPEND_NAME
#endif

BOOL ReachSymTabConst_EM
    (LPVOID   lpMemRes,         // Put the result here
     LPVOID   lpMemRch,         // Ptr to the reach symbol table constant
     LPVOID   lpMemLft,         // Get the results from here
     APLSTYPE aplTypeLft,       // The storage type of the left arg
     APLRANK  aplRankLft,       // The rank of the left arg
     LPAPLDIM lpMemDimLft,      // Ptr to the dimensions of the left arg
     LPTOKEN  lptkFunc)         // Ptr to the function token

{
    APLINT  aplInteger;         // Temporary integer
    BOOL    bRet = TRUE;        // TRUE iff result is valid
    APLBOOL bQuadIO;            // []IO

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Check for RANK ERROR
    if (aplRankLft NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        return FALSE;
    } // End IF

#define lpSymRch        (*(LPAPLHETERO) lpMemRch)

    // Split cases based upon the immediate type of the reach arg
    switch (lpSymRch->stFlags.ImmType)
    {
        case IMMTYPE_BOOL:
            aplInteger = lpSymRch->stData.stBoolean;

            break;

        case IMMTYPE_INT:
            aplInteger = lpSymRch->stData.stInteger;

            break;

        case IMMTYPE_FLOAT:
            aplInteger = FloatToAplint_SCT (lpSymRch->stData.stFloat, &bRet);
            if (bRet)
                break;

            // Otherwise, fall through to common DOMAIN ERROR

        case IMMTYPE_CHAR:
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                       lptkFunc);
            return FALSE;

        defstop
            break;
    } // End SWITCH

    // Convert to origin-0
    aplInteger -= bQuadIO;

    // Check for INDEX ERROR
    if (aplInteger < 0
     || aplInteger >= (APLINT) lpMemDimLft[0])
    {
        ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                                   lptkFunc);
        return FALSE;
    } // End IF

    // Use aplInteger to index lpMemLft
    bRet =
    ValueAsSymentry (lpMemRes,      // Put the result here
                     lpMemLft,      // Get the result from here
                     aplTypeLft,    // The storage type of the left arg
                     aplInteger,    // The index to use
                     lptkFunc);     // The function token
    return bRet;
} // End ReachSymTabConst_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ValueAsSymentry
//
//  Extract a particular value from an array as a LPSYMENTRY
//***************************************************************************

BOOL ValueAsSymentry
    (LPVOID   lpMemRes,     // Put the result here
     LPVOID   lpMemLft,     // Get the result from here
     APLSTYPE aplTypeLft,   // The storage type of the left arg
     APLINT   aplInteger,   // The index to use
     LPTOKEN  lptkFunc)     // The function token

{
    APLINT     apaOff,
               apaMul;
    APLLONGEST aplVal;

   // Split cases based upon the storage type of the left arg
   switch (aplTypeLft)
   {
       case ARRAY_BOOL:
           aplVal = ((1 << (aplInteger % NBIB)) & ((LPAPLBOOL) lpMemLft)[aplInteger >> LOG2NBIB]) ? 1 : 0;
           *((LPAPLNESTED) lpMemRes) =
             MakeSymEntry_EM (IMMTYPE_BOOL,
                             &aplVal,
                              lptkFunc);
           break;

       case ARRAY_INT:
           aplVal = ((LPAPLINT) lpMemLft)[aplInteger];
           *((LPAPLNESTED) lpMemRes) =
             MakeSymEntry_EM (IMMTYPE_INT,
                             &aplVal,
                              lptkFunc);
           break;

       case ARRAY_FLOAT:
           aplVal = *(LPAPLLONGEST) &((LPAPLFLOAT) lpMemLft)[aplInteger];
           *((LPAPLNESTED) lpMemRes) =
             MakeSymEntry_EM (IMMTYPE_FLOAT,
                             &aplVal,
                              lptkFunc);
           break;

       case ARRAY_CHAR:
           aplVal = ((LPAPLCHAR) lpMemLft)[aplInteger];
           *((LPAPLNESTED) lpMemRes) =
             MakeSymEntry_EM (IMMTYPE_CHAR,
                             &aplVal,
                              lptkFunc);
           break;

       case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemLft)
            // Get the APA parameters
            apaOff = lpAPA->Off;
            apaMul = lpAPA->Mul;
#undef  lpAPA
            aplVal = apaOff + apaMul * aplInteger;
            *((LPAPLNESTED) lpMemRes) =
              MakeSymEntry_EM (IMMTYPE_INT,
                              &aplVal,
                               lptkFunc);
            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            *((LPAPLNESTED) lpMemRes) = CopySymGlbDir (((LPAPLNESTED) lpMemLft)[aplInteger]);

            break;

        defstop
            break;
    } // End SWITCH

    return TRUE;
} // End ValueAsSymentry


//***************************************************************************
//  $ReachDown_EM
//
//  Reach down recursively through the index vector & left args
//    to put the reached for item into the result.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ReachDown_EM"
#else
#define APPEND_NAME
#endif

BOOL ReachDown_EM
    (LPAPLNESTED lpMemRes,      // Put the result here
     LPVOID      lpMemNdx,      // The current index vector w/aplNELMNdx elements
     APLSTYPE    aplTypeNdx,    // The storage type of the index vector
     APLNELM     aplNELMNdx,    // The NELM of the index vector
     LPVOID      lpMemLft,      // Get the results from here
     APLSTYPE    aplTypeLft,    // The storage type of the left arg
     APLRANK     aplRankLft,    // The rank of the left arg
     LPAPLDIM    lpMemDimLft,   // Ptr to the dimensions of the left arg
     LPTOKEN     lptkFunc)      // Ptr to function token

{
    BOOL     bRet = TRUE;
    LPVOID   lpMemSub;
    HGLOBAL  hGlbLft;
    APLSTYPE aplTypeSub;
    APLNELM  aplNELMSub,
             aplNELMLft;
    APLRANK  aplRankSub;
    APLINT   aplIndex;

    DbgBrk ();

    // Stop when no more elements remain
    if (aplNELMNdx EQ 0)
        *lpMemRes = CopySymGlbInd (lpMemLft);
    else
    {
        // If the index vector is simple, it's a vector of indices to the left arg
        if (IsSimple (aplTypeNdx))
        {
            // If the NELM of the index vector matches the rank of the left arg,
            //   it's a single index into the left arg
            if (aplNELMNdx EQ aplRankLft)
            {
                // Check each element in lpMemSub against lpMemDimLft
                //   for INDEX ERROR as well as DOMAIN ERROR
                //   and, at the same time, weigh each element as
                //   per lpMemDimLft to get the appropriate index
                //   into lpMemLft.
                bRet = WeighIndex_EM (&aplIndex,        // Save the result here
                                       lpMemNdx,        // Ptr to the index vector
                                       aplTypeNdx,      // Storage type of the index vector
                                       aplNELMNdx,      // NELM of the index vector
                                       lpMemDimLft,     // Ptr to the dimension vector of the left arg
                                       lptkFunc);       // Ptr to the function token
                if (bRet)
                    // Use aplInteger to index lpMemLft
                    bRet =
                    ValueAsSymentry (lpMemRes,      // Put the result here
                                     lpMemLft,      // Get the result from here
                                     aplTypeLft,    // The storage type of the left arg
                                     aplIndex,      // The index to use
                                     lptkFunc);     // The function token
                return bRet;
            } else
            // Otherwise, the left arg must be a vector of vectors of ...
            //   and the successive elements of the index vector
            //   reach into it.
            {
                // Check for RANK ERROR
                if (aplRankLft NE 1)
                {
                    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                               lptkFunc);
                    bRet = FALSE;
                } else
                // Check for INDEX ERROR
                if (aplTypeLft NE ARRAY_NESTED
                 || !IsSimpleNum (aplTypeNdx))
                {
                    ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                                               lptkFunc);
                    bRet = FALSE;
                } else
                {






                } // End IF/ELSE
            } // End IF/ELSE
        } else
        {


        } // End IF/ELSE

        // The global should point to a scalar or a vector of scalars or vectors
        //   which serves as the reach index into the left arg



        // Use the first element of lpMemSub to index lpMemLft

        // Split cases based upon the ptr type of the reach element
        switch (GetPtrTypeInd (lpMemNdx))
        {
            case PTRTYPE_STCONST:
                bRet =
                ReachSymTabConst_EM (lpMemRes,          // Put the result here
                                     lpMemNdx,          // Ptr to the reach symbol table const
                                     lpMemLft,          // Get the results from here
                                     aplTypeLft,        // The storage type of the left arg
                                     aplRankLft,        // The rank of the left arg
                                     lpMemDimLft,       // Ptr to the dimensions of the left arg
                                     lptkFunc);         // Ptr to the function token
                break;

            case PTRTYPE_HGLOBAL:
                // Lock the memory to get a ptr to it
                lpMemSub = MyGlobalLock (ClrPtrTypeIndGlb (lpMemNdx));

#define lpHeader        ((LPVARARRAY_HEADER) lpMemSub)
                // Get the Array Type, NELM, and Rank
                aplTypeSub = lpHeader->ArrType;
                aplNELMSub = lpHeader->NELM;
                aplRankSub = lpHeader->Rank;
#undef  lpHeader

                // Check for RANK ERROR
                if (aplRankSub > 1)
                {
                    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                               lptkFunc);
                    bRet = FALSE;
                } else
                // Check for LENGTH ERROR
                if (aplNELMSub NE aplRankLft)
                {
                    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                               lptkFunc);
                    bRet = FALSE;
                } else
                {
                    // Skip over the header and dimensions to the data
                    lpMemSub = VarArrayBaseToData (lpMemSub, aplRankSub);

                    // Check each element in lpMemSub against lpMemDimLft
                    //   for INDEX ERROR as well as DOMAIN ERROR
                    //   and, at the same time, weigh each element as
                    //   per lpMemDimLft to get the appropriate index
                    //   into lpMemLft.
                    bRet = WeighIndex_EM (&aplIndex,        // Save the result here
                                           lpMemSub,        // Ptr to the index vector
                                           aplTypeSub,      // Storage type of the index vector
                                           aplNELMSub,      // NELM of the index vector
                                           lpMemDimLft,     // Ptr to the dimension vector of the left arg
                                           lptkFunc);       // Ptr to the function token
                    if (bRet)
                    {
                        // Check for LENGTH ERROR if the left arg is simple (Depth < 2)
                        //   and the # reach indices are > 1.
                        if (IsSimple (aplTypeLft)
                         && aplNELMNdx > 1)
                        {
                            ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                                       lptkFunc);
                            bRet = FALSE;
                        } else
                        {
                            // Now use aplIndex to extract the next item from lpMemLft
                            //   if it's simple
                            if (IsSimple (aplTypeLft))
                                bRet =
                                ValueAsSymentry (lpMemRes,          // Put the result here
                                                 lpMemLft,          // Get the result from here
                                                 aplTypeLft,        // The storage type of the left arg
                                                 aplIndex,          // The index to use
                                                 lptkFunc);         // The function token
                            else
                            {
                                HGLOBAL hGlbSub;

                                // Get the global memory handle
                                hGlbSub = *(LPAPLNESTED) lpMemSub;
                                hGlbLft = *(LPAPLNESTED) lpMemLft;

                                // Split cases based upon the ptr type
                                switch (GetPtrTypeDir (hGlbSub))
                                {
                                    case PTRTYPE_STCONST:
                                        DbgBrk ();      // ***FIXME*** -- How do we get here??





                                        break;

                                    case PTRTYPE_HGLOBAL:
                                        // Lock the memory to get a ptr to it
                                        lpMemSub = MyGlobalLock (ClrPtrTypeDirGlb (hGlbSub));
                                        lpMemLft = MyGlobalLock (ClrPtrTypeDirGlb (hGlbLft));

                                        // Get the attributes (Type, NELM, and Rank) of the left arg
                                        AttrsOfGlb (hGlbLft, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);

                                        // Skip over the header to the dimensions
                                        lpMemDimLft = VarArrayBaseToDim (lpMemLft);

                                        // Reach down through the left arg using successive
                                        //   elements of the reach arg as indices
                                        bRet =
                                        ReachDown_EM (lpMemRes,             // Put the result here
                                                      lpMemSub,             // Ptr to the reach vector w/aplNELMSub elements
                                                      aplTypeSub,           // The storage type of the reach vector ***FIXME*** -- Correct??
                                                      aplNELMSub - 1,       // The NELM of the reach vector
                                                      lpMemLft,             // Get the results from here
                                                      aplTypeLft,           // The storage type of the left arg
                                                      aplRankLft,           // The rank of the left arg
                                                      lpMemDimLft,          // Ptr to the dimensions of the left arg
                                                      lptkFunc);            // Ptr to function token
                                        // We no longer need this ptr
                                        MyGlobalUnlock (ClrPtrTypeDirGlb (hGlbLft));
                                        MyGlobalUnlock (ClrPtrTypeDirGlb (hGlbSub));

                                        break;

                                    defstop
                                        break;
                                } // End SWITCH
                            } // End IF/ELSE
                        } // End IF/ELSE
                    } // End IF
                } // End IF/ELSE

                // We no longer need this ptr
                MyGlobalUnlock (ClrPtrTypeIndGlb (lpMemNdx)); lpMemSub = NULL;

                break;

            defstop
                break;
        } // End SWITCH
    } // End IF/ELSE

    return bRet;
} // End ReachDown_EM
#undef  APPEND_NAME


//***************************************************************************
//  $WeighIndex_EM
//
//  Weigh an index according to a dimension vector
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- WeighIndex_EM"
#else
#define APPEND_NAME
#endif

BOOL WeighIndex_EM
    (LPAPLINT lpaplIndex,       // Save the result here
     LPVOID   lpMemSub,         // Ptr to the index vector
     APLSTYPE aplTypeSub,       // Storage type of the index vector
     APLNELM  aplNELMSub,       // NELM of the index vector
     LPAPLDIM lpMemDimLft,      // Ptr to the dimension vector of the left arg
     LPTOKEN  lptkFunc)         // Ptr to the function token

{
    BOOL    bRet = TRUE;        // TRUE iff result is valid
    APLUINT aplWeight;          // The weighting accumulator
    APLINT  iRch,               // Loop var (must be signed)
            aplInteger,         // Temporary integer
            apaOff,             // APA offset
            apaMul;             // ... multiplier
    APLBOOL bQuadIO;            // []IO

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Initialize the weight & index accumulator
    aplWeight = 1;
    *lpaplIndex  = 0;

    // Split cases based upon the storage type of the reach index
    switch (aplTypeSub)
    {
        case ARRAY_BOOL:
            // Loop through the reach index, backwards
            for (iRch = aplNELMSub - 1; bRet && iRch >= 0; iRch--)
            {
                // Get the next index & convert to origin-0
                aplInteger = (((1 << (iRch % NBIB)) & ((LPAPLBOOL) lpMemSub)[iRch >> LOG2NBIB]) ? 1 : 0) - bQuadIO;

                // Check for INDEX ERROR
                if (aplInteger < 0
                 || aplInteger >= (APLINT) lpMemDimLft[iRch])
                {
                    ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                                               lptkFunc);
                    bRet = FALSE;
                } else
                {
                    // Add into index & multiply into weight
                    *lpaplIndex  += aplWeight * aplInteger;
                    aplWeight *= lpMemDimLft[iRch];
                } // End IF/LSE
            } // End FOR

            break;

        case ARRAY_INT:
            // Loop through the reach index, backwards
            for (iRch = aplNELMSub - 1; bRet && iRch >= 0; iRch--)
            {
                // Get the next index & convert to origin-0
                aplInteger = ((LPAPLINT) lpMemSub)[iRch] - bQuadIO;

                // Check for INDEX ERROR
                if (aplInteger < 0
                 || aplInteger >= (APLINT) lpMemDimLft[iRch])
                {
                    ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                                               lptkFunc);
                    bRet = FALSE;
                } else
                {
                    // Add into index & multiply into weight
                    *lpaplIndex  += aplWeight * aplInteger;
                    aplWeight *= lpMemDimLft[iRch];
                } // End IF/LSE
            } // End FOR

            break;

        case ARRAY_FLOAT:
            // Loop through the reach index, backwards
            for (iRch = aplNELMSub - 1; bRet && iRch >= 0; iRch--)
            {
                // Get the next index & convert to origin-0
                aplInteger = FloatToAplint_SCT (((LPAPLFLOAT) lpMemSub)[iRch], &bRet) - bQuadIO;

                // Check for INDEX ERROR
                if (!bRet
                 || aplInteger < 0
                 || aplInteger >= (APLINT) lpMemDimLft[iRch])
                {
                    ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                                               lptkFunc);
                    bRet = FALSE;
                } else
                {
                    // Add into index & multiply into weight
                    *lpaplIndex  += aplWeight * aplInteger;
                    aplWeight *= lpMemDimLft[iRch];
                } // End IF/LSE
            } // End FOR

            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemSub)
            // Get the APA parameters
            apaOff = lpAPA->Off;
            apaMul = lpAPA->Mul;
#undef  lpAPA
            // Loop through the reach index, backwards
            for (iRch = aplNELMSub - 1; bRet && iRch >= 0; iRch--)
            {
                // Get the next index & convert to origin-0
                aplInteger = (apaOff + apaMul * iRch) - bQuadIO;

                // Check for INDEX ERROR
                if (aplInteger < 0
                 || aplInteger >= (APLINT) lpMemDimLft[iRch])
                {
                    ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                                               lptkFunc);
                    bRet = FALSE;
                } else
                {
                    // Add into index & multiply into weight
                    *lpaplIndex  += aplWeight * aplInteger;
                    aplWeight *= lpMemDimLft[iRch];
                } // End IF/LSE
            } // End FOR

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                       lptkFunc);
            bRet = FALSE;

            break;

        defstop
            break;
    } // End SWITCH

    return bRet;
} // End WeighIndex_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_squad.c
//***************************************************************************
