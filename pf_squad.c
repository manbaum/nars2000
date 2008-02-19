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
    return PrimFnValenceError_EM (lptkFunc);
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

                // Clear the ptr type bits
                hGlbRht = ClrPtrTypeDirAsGlb (hGlbRht);

                return PrimFnDydSquadGlb_EM_YY
                       (lptkLftArg,         // Ptr to left arg token
                       &hGlbRht,            // Ptr to right arg global memory handle
                        lptkAxis,           // Ptr to axis token (may be NULL)
                        lptkFunc,           // Ptr to function token
                        NULL);              // Ptr to set arg token
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
            // Get the right arg global memory handle
            hGlbRht = lptkRhtArg->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbRht));

            // Clear the ptr type bits
            hGlbRht = ClrPtrTypeDirAsGlb (hGlbRht);

            return PrimFnDydSquadGlb_EM_YY
                   (lptkLftArg,         // Ptr to left arg token
                   &hGlbRht,            // Right arg global memory handle
                    lptkAxis,           // Ptr to axis token (may be NULL)
                    lptkFunc,           // Ptr to function token
                    NULL);              // Ptr to set arg token
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
//  Dyadic Squad ("rectangular indexing") on a right arg global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSquadGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydSquadGlb_EM_YY
    (LPTOKEN    lptkLftArg,         // Ptr to left arg token
     HGLOBAL   *lphGlbRht,          // Ptr to right arg global memory handle
     LPTOKEN    lptkAxis,           // Ptr to axis token (may be NULL)
     LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkSetArg)         // Ptr to set arg token (may be NULL if not assignment)

{
    APLSTYPE     aplTypeLft,        // Left arg storage type of the left arg
                 aplTypeRht,        // Right ...
                 aplTypeRes,        // Result   ...
                 aplTypeSet,        // Set  ...
                 aplTypePro;        // Promoted ...
    APLNELM      aplNELMLft,        // Left arg NELM
                 aplNELMRht,        // Right ...
                 aplNELMAxis,       // Axis ...
                 aplNELMRes,        // Result   ...
                 aplNELMSet;        // Set  ...
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankRht,        // Right ...
                 aplRankRes,        // Result   ...
                 aplRankSet;        // Set  ...
    HGLOBAL      hGlbLft = NULL,    // Left arg global memory handle
                 hGlbRht = NULL,    // Right ...
                 hGlbAxis = NULL,   // Axis ...
                 hGlbSet = NULL,    // Set  ...
                 hGlbSubSet,        // Set arg item ...
                 hGlbRes = NULL,    // Result   ...
                 hGlbOdo = NULL;    // Odometer ...
    LPVOID       lpMemLft = NULL,   // Ptr to left arg global memory
                 lpMemRht = NULL,   // Ptr to right ...
                 lpMemRes = NULL,   // Ptr to result   ...
                 lpMemSet = NULL;   // Ptr to set  ...
    APLUINT      uLft,              // Loop counter
                 ByteRes,           // # bytes in the result
                 uWVal,             // Weighting value
                 aplIntAcc,         // Accumulation value
                 uRes,              // Loop counter
                 uSet,              // Loop counter
                 aplIndexSet;       // Index into set arg
    APLINT       iLft,              // Loop counter
                 iAxisNxt;          // Index of next axis value
    LPAPLUINT    lpMemAxis = NULL,  // Ptr to axis global memory
                 lpMemAxisAct,      // Ptr to actual axis values
                 lpMemAxisLst,      // Ptr to (last + 1) of actual axis values
                 lpMemAxisEli,      // Ptr to elided axis values
                 lpMemOdo = NULL,   // Ptr to odometer global memory
                 lpMemLimLft;       // Ptr to left arg limit vector
    LPAPLDIM     lpMemDimRht,       // Ptr to right arg dimensions
                 lpMemDimRes,       // Ptr to result    ...
                 lpMemDimSet;       // Ptr to set   ...
    APLLONGEST   aplLongestLft,     // Left arg as immediate value
                 aplLongestSet;     // Set  ...
    UINT         uBitMask;          // Bit mask when looping through Booleans
    BOOL         bRet = TRUE;       // TRUE iff result is valid
    APLBOOL      bQuadIO;           // []IO
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get the right arg global memory handle
    hGlbRht = *lphGlbRht;

    // Get the attributes (Type, NELM, and Rank) of the left, right, and set args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfGlb   (hGlbRht   , &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);
    if (lptkSetArg)
        AttrsOfToken (lptkSetArg, &aplTypeSet, &aplNELMSet, &aplRankSet, NULL);

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
    if (IsSimpleChar (aplTypeLft)
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

    // Get left arg global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);

    // Lock the memory to get a ptr to it
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
    if (!IsSimpleNH (aplTypeLft))   // Meaning we accept '' as a valid left arg
    {
        HGLOBAL hGlbSub;        // Left arg item global memory handle
        APLSTYPE aplTypeSub;    // Left arg item storage type
        APLNELM  aplNELMSub;    // Left arg item NELM
        APLRANK  aplRankSub;    // Left arg item rank

        // Check for empty left arg
        if (aplNELMLft EQ 0)
        {
            // The prototype for the left arg must be
            //   a nested numeric array

            // Get next value from the left arg
            GetNextValueGlb (hGlbLft, 0, &hGlbSub, NULL, NULL);

            // The item must be a global
            Assert (hGlbSub NE NULL);

            // Get the global attrs
            AttrsOfGlb (hGlbSub, &aplTypeSub, &aplNELMSub, NULL, NULL);

            // Check for DOMAIN ERROR
            if (!IsSimpleNum (aplTypeSub))
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF
        } else
        for (uLft = 0; uLft < aplNELMLft; uLft++)
        {
            APLLONGEST aplLongestSub;       // Left arg item immediate value
            IMM_TYPES  immTypeSub;          // Left arg item immediate type

            // Get next value from the left arg
            GetNextValueGlb (hGlbLft, uLft, &hGlbSub, &aplLongestSub, &immTypeSub);

            // If the left arg item is a global, ...
            if (hGlbSub)
            {
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
    } // End IF

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
            aplNELMRes *= lpMemDimRht[lpMemAxis[uAxis]];
            aplRankRes += 1;
        } // End FOR
    } // End IF

    // If we're not assigning, ...
    if (lptkSetArg EQ NULL)
    {
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
////////lpHeader->Perm       = 0;               // Already zero from GHND
////////lpHeader->SysVar     = 0;               // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMRes;
        lpHeader->Rank       = aplRankRes;
#undef  lpHeader

        // Skip over the header to the dimensions
        lpMemDimRes = VarArrayBaseToDim (lpMemRes);
    } else
    {
        // Check for RANK ERROR between the result arg & set arg ranks
        if (aplNELMSet NE 1
         && aplRankRes NE aplRankSet)
        {
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Get set arg global ptrs
        aplLongestSet = GetGlbPtrs_LOCK (lptkSetArg, &hGlbSet, &lpMemSet);
        if (lpMemSet)
            // Skip over the header to the dimensions
            lpMemDimSet = VarArrayBaseToDim (lpMemSet);
    } // End IF/ELSE

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

    // Calc ptr to (last + 1) of actual dimensions
    lpMemAxisLst = &lpMemAxis[aplRankRht];

    // Initialize axis index
    iAxisNxt = uLft = 0;

    //***************************************************************
    // Copy dimensions from the right arg or left arg items
    //    to the result
    //***************************************************************
#define uAxisNxt    ((APLUINT) iAxisNxt)
    if (lptkSetArg EQ NULL || aplNELMSet NE 1)
    while (uAxisNxt < aplRankRht)
    {
        if (aplNELMAxis EQ aplRankRht       // No or full axis operator
         || (lpMemAxisAct < lpMemAxisLst    //   or ptr is within range
          && uAxisNxt EQ *lpMemAxisAct))    //     and next axis value is an actual one
        {
            HGLOBAL hGlbSub;

            // Skip over the actual axis
            lpMemAxisAct++;

            // If the left arg is a global, ...
            if (hGlbLft)
            {
                // Get next value from the left arg
                GetNextValueGlb (hGlbLft, uLft++, &hGlbSub, NULL, NULL);

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

                    // Skip over the header to the dimensions
                    lpMemSub = VarArrayBaseToDim (lpMemSub);

                    // If we're not assigning, ...
                    if (lptkSetArg EQ NULL)
                        // Fill in the result's dimension
                        CopyMemory (lpMemDimRes, lpMemSub, (UINT) aplRankSub * sizeof (APLDIM));
                    else
                    {
                        // Compare the left arg item & set arg dimensions
                        for (uSet = 0; uSet < aplRankSub; uSet++)
                        if (*lpMemSub++ NE *lpMemDimSet++)
                        {
                            ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                                       lptkFunc);
                            goto ERROR_EXIT;
                        } // End FOR/IF
                    } // End IF/ELSE

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbSub); lpMemSub = NULL;

                    // If we're not assigning, ...
                    if (lptkSetArg EQ NULL)
                    {
                        // Skip over the copied dimensions
                        lpMemDimRes += aplRankSub;
                    } // End IF

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
            // If we're not assigning, ...
            if (lptkSetArg EQ NULL)
                // Fill in the result's dimension
                *lpMemDimRes++ = lpMemDimRht[uAxisNxt];
            else
            {
                // Compare the right arg & set arg dimensions
                if (lpMemDimRht[uAxisNxt] NE *lpMemDimSet++)
                {
                    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                               lptkFunc);
                    goto ERROR_EXIT;
                } // End IF
            } // End IF/ELSE

            // Fill in the left arg item limit
            *lpMemLimLft++ = lpMemDimRht[uAxisNxt];
        } // End IF/ELSE

        // Skip to next dimension index
        uAxisNxt++;
    } // End IF/WHILE
#undef  uAxisNxt

    // If we're not assigning, ...
    if (lptkSetArg EQ NULL)
        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);
    else
    {
        if (lpMemSet)
        {
            // Skip over the header and dimensions to the data
            lpMemSet = VarArrayBaseToData (lpMemSet, aplRankSet);

            if (aplNELMSet EQ 1)
                // Get the first item from the set arg
                GetNextItemMem (lpMemSet,
                                aplTypeSet,
                                0,
                               &hGlbSubSet,
                               &aplLongestSet);
            else
                aplIndexSet = 0;
        } // End IF

        // If the right arg should be promoted, ...
        if (QueryPromote (aplTypeRht, aplTypeSet, &aplTypePro))
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemRht = lpMemDimRht = NULL;

            // Promote the right arg
            if (!TypePromoteGlb_EM (&hGlbRht, aplTypePro, lptkFunc))
                goto ERROR_EXIT;

            // Save the new type
            aplTypeRht = aplTypePro;

            // Save the new global memory handle
            *lphGlbRht = hGlbRht;

            // Lock the memory to get a ptr to it
            lpMemRht = MyGlobalLock (hGlbRht);

            // Skip over the header to the dimensions
            lpMemDimRht = VarArrayBaseToDim (lpMemRht);
        } // End IF
    } // End IF/ELSE

    // If the result is nested or hetero, fill the data with PTR_REUSED
    //   in case we fail along the way
    if (lptkSetArg EQ NULL)
    if (aplTypeRes EQ ARRAY_HETERO
     || aplTypeRes EQ ARRAY_NESTED)
    {
        // Fill in first one in case of nested prototypes
        if (aplTypeRes EQ ARRAY_NESTED)
            ((LPAPLNESTED) lpMemRes)[0] = PTR_REUSED;

        // Loop through the rest of the items
        for (uRes = 0; uRes < aplNELMRes; uRes++)
            ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;
    } // End IF

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

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
                    GetNextValueGlb (hGlbLft, iLft--, &hGlbSub, &aplLongestSub, &immTypeSub);

                    // If the left arg item is a global, ...
                    if (hGlbSub)
                    {
                        // The index value is the <lpMemOdo[iAxisNxt]> value in <hGlbSub>
                        GetNextValueGlb (hGlbSub, lpMemOdo[iAxisNxt], NULL, &aplLongestNxt, &immTypeNxt);

                        // If the index value is float, attempt to convert it to int
                        if (IsImmFlt (immTypeNxt))
                        {
                            // Attempt to convert the float to an integer using System CT
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
                        if (IsImmFlt (immTypeSub))
                        {
                            // Attempt to convert the float to an integer using System CT
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
                    if (IsImmFlt (immTypeSub))
                    {
                        // Attempt to convert the float to an integer using System CT
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

                // Convert to origin-0
                aplLongestNxt -= bQuadIO;
            } else  // Elided dimension:  use dimension from right arg
                // Copy dimension index
                aplLongestNxt = lpMemOdo[iAxisNxt];

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

        // If we're not assigning, ...
        if (lptkSetArg EQ NULL)
        {
            // Extract the <aplIntAcc> value from the right arg
            //   and save into the result
            GetNextValueGlb (hGlbRht, aplIntAcc, &hGlbSub, &aplLongestSub, &immTypeSub);

            // If the right arg item is a global, ...
            if (hGlbSub)
                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDirAsGlb (hGlbSub);
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
        } else
        {
            // Get the next item from the set arg
            if (aplNELMSet NE 1)
                GetNextItemMem (lpMemSet,
                                aplTypeSet,
                                aplIndexSet++,
                               &hGlbSubSet,
                               &aplLongestSet);
            // Replace the <aplIntAcc> element in hGlbRht
            //   with <aplLongestSet> or <hGlbSubSet> depending upon <aplTypeRht>
            ArrayIndexReplace (aplTypeRht,      // Right arg storage type
                               lpMemRht,        // Ptr to right arg global memory
                               aplIntAcc,       // Index into right arg
                               aplTypeSet,      // Set arg storage type
                               aplLongestSet,   // Set arg immediate value
                               hGlbSubSet,      // Set arg global memory handle
                               lptkFunc);       // Ptr to function token
        } // End IF/ELSE

        // Increment the odometer in lpMemOdo subject to
        //   the values in lpMemLimLft
        IncrOdometer (lpMemOdo, lpMemLimLft, NULL, aplRankRht);
    } // End FOR

    // If we're not assigning, ...
    if (lptkSetArg EQ NULL)
    {
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
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        // See if it fits into a lower (but not necessarily smaller) datatype
        TypeDemote (&lpYYRes->tkToken);
    } else
        // Return pseudo-value indicating success
        lpYYRes = PTR_REUSED;

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

    if (hGlbSet && lpMemSet)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbSet); lpMemSet = NULL;
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
