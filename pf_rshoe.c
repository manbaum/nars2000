//***************************************************************************
//  NARS2000 -- Primitive Function -- RightShoe
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
//  $PrimFnRightShoe_EM_YY
//
//  Primitive function for monadic and dyadic RightShoe ("disclose" and "pick")
//***************************************************************************

LPPL_YYSTYPE PrimFnRightShoe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_RIGHTSHOE);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonRightShoe_EM_YY             (lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydRightShoe_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnRightShoe_EM


//***************************************************************************
//  $PrimProtoFnRightShoe_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic RightShoe
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnRightShoe_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnRightShoe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnRightShoe_EM_YY,   // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnRightShoe_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonRightShoe_EM_YY
//
//  Primitive function for monadic RightShoe ("disclose")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonRightShoe_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonRightShoe_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
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

                return PrimFnMonRightShoeGlb_EM_YY
                       (ClrPtrTypeDirAsGlb (lptkRhtArg->tkData.tkSym->stData.stGlbData),  // HGLOBAL
                        lptkAxis,                                                       // Ptr to axis token (may be NULL)
                        lptkFunc);                                                      // Ptr to function token
            } // End IF

            // Handle the immediate case
            return PrimFnMonRightShoeCon_EM_YY
                   (lptkRhtArg->tkData.tkSym->stFlags.ImmType,  // Immediate type
                    lptkRhtArg->tkData.tkSym->stData.stLongest, // Immediate value
                    lptkAxis,                                   // Ptr to axis token (may be NULL)
                    lptkFunc);                                  // Ptr to function token
        case TKT_VARIMMED:
            return PrimFnMonRightShoeCon_EM_YY
                   (lptkRhtArg->tkFlags.ImmType,                // Immediate type
                    lptkRhtArg->tkData.tkLongest,               // Immediate value
                    lptkAxis,                                   // Ptr to axis token (may be NULL)
                    lptkFunc);                                  // Ptr to function token
        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

            return PrimFnMonRightShoeGlb_EM_YY
                   (ClrPtrTypeDirAsGlb (lptkRhtArg->tkData.tkGlbData),  // HGLOBAL
                    lptkAxis,                                           // Ptr to axis token (may be NULL)
                    lptkFunc);                                          // Ptr to function token
        defstop
            return NULL;
    } // End SWITCH
} // End PrimFnMonRightShoe_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonRightShoeCon_EM_YY
//
//  Monadic RightShoe ("disclose") on an immediate value.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonRightShoeCon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonRightShoeCon_EM_YY
    (IMM_TYPES  immType,            // The immediate type (see IMM_TYPES)
     APLLONGEST aplLongest,         // The immediate value
     LPTOKEN    lptkAxis,           // Ptr to axis token
     LPTOKEN    lptkFunc)           // Ptr to function token

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Disclose with axis on a simple scalar requires
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
    lpYYRes->tkToken.tkFlags.ImmType   = immType;
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkLongest  = aplLongest;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End PrimFnMonRightShoeCon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonRightShoeGlb_EM_YY
//
//  Monadic RightShoe ("disclose") on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonRightShoeGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonRightShoeGlb_EM_YY
    (HGLOBAL hGlbRht,               // Right arg global memory handle
     LPTOKEN lptkAxis,              // Ptr to axis token (may be NULL)
     LPTOKEN lptkFunc)              // Ptr to function token

{
    HGLOBAL      hGlbRes = NULL,    // Result global memory handle
                 hGlbAxis = NULL,   // Axis global memory handle
                 hGlbSub,           // Right arg item global memory handle
                 hGlbProto,         // Right arg item prototype as global memory handle
                 hGlbDimCom = NULL, // Right arg common item dimension global memory handle
                 hGlbLft;           // Left arg global memory handle
    LPVOID       lpMemRes = NULL,   // Ptr to result global memory
                 lpMemRht = NULL,   // Ptr to right arg global memory
                 lpMemSub;          // Ptr to right arg item global memory
    LPAPLDIM     lpMemDimRht = NULL,// Ptr to right arg dimensions
                 lpMemDimCom = NULL,// Ptr to right arg common item dimensions
                 lpMemDimRes;       // Ptr to result dimensions
    LPAPLUINT    lpMemAxis = NULL,  // Ptr to axis values, fleshed out
                 lpMemLft;          // Ptr to left arg global memory
    APLUINT      ByteRes;           // # bytes in the result
    APLNELM      aplNELMAxis,       // Axis NELM
                 aplNELMRes,        // Result NELM
                 aplNELMRht,        // Right arg NELM
                 aplNELMNstRht,     // Right arg NELM if empty nested
                 aplNELMCom,        // Right arg common item NELM
                 aplNELMComRest,    // Right arg common item NELM except for last dimension
                 aplNELMComLast,    // Right arg common item NELM last dimension
                 aplNELMSub;        // Right arg item NELM
    APLRANK      aplRankRht,        // Right arg rank
                 aplRankRes,        // Result rank
                 aplRankCom,        // Right arg common item rank
                 aplRankSub;        // Right arg item rank
    BOOL         bRet = TRUE;       // TRUE iff result is valid
    UINT         uBitMask;          // Bit mask for looping through Booleans
    APLNELM      uRht,              // Loop counter
                 uCom,              // ...
                 uSub;              // ...
    APLSTYPE     aplTypeRht,        // Right arg storage type
                 aplTypeSub;        // Right arg item storage type
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result
    LPSYMENTRY   lpSymProto,        // Right arg item prototype as Symbol Table Entry
                 lpSym0,            // LPSYMENTRY for constant zero
                 lpSym1;            // ...                     one
    APLUINT      uSubRest,          // Loop counter
                 uSubLast;          // ...
    APLNELM      aplNELMSubRest,    // Right arg item NELM except for last coordinate
                 aplNELMSubLast;    // Right arg item NELM last coordinate
    APLINT       apaOffSub,         // Right arg item APA offset
                 apaMulSub;         // Right arg item APA multiplier

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg global
    AttrsOfGlb (hGlbRht, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Take into account nested prototypes
    if (IsNested (aplTypeRht))
        aplNELMNstRht = max (aplNELMRht, 1);
    else
        aplNELMNstRht = aplNELMRht;

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Initialize the common item rank
    aplRankCom = 0;

    // Trundle through the right arg ensuring that, except for scalars,
    //   all of the items have the same rank
    if (!IsSimple (aplTypeRht))
    for (uRht = 0; uRht < aplNELMNstRht; uRht++)
    {
        // Split cases based upon the ptr type of the item
        switch (GetPtrTypeDir (((LPAPLNESTED) lpMemRht)[uRht]))
        {
            case PTRTYPE_STCONST:   // Ignore (simple) scalar items
                break;

            case PTRTYPE_HGLOBAL:
                // Get the attributes (Type, NELM, and Rank)
                //   of the item
                AttrsOfGlb (ClrPtrTypeDirAsGlb (((LPAPLNESTED) lpMemRht)[uRht]), NULL, NULL, &aplRankSub, NULL);

                // If the item is not a scalar, ...
                if (aplRankSub > 0)
                {
                    // If the result rank hasn't been specified as yet, ...
                    if (aplRankCom EQ 0)
                        aplRankCom = aplRankSub;
                    else
                    // If the result rank and the item rank are unequal, ...
                    if (aplRankCom NE aplRankSub)
                    {
                        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                                   lptkFunc);
                        goto ERROR_EXIT;
                    } // End IF
                } // End IF

                break;

            defstop
                break;
        } // End SWITCH
    } // End IF/FOR

    // The result rank is the sum of the common item rank and the right arg rank
    aplRankRes = aplRankCom + aplRankRht;

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
            goto ERROR_EXIT;
    } else
        // No axis means disclose all dimensions
        aplNELMAxis = aplRankCom;

    // If the common item rank isn't the same as the # elements in the axis, ..
    if (aplRankCom NE aplNELMAxis)
    {
        ErrorMessageIndirectToken (ERRMSG_AXIS_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    //***************************************************************
    // hGlbAxis is not-NULL iff there is an axis
    // aplNELMAxis contains the # elements in the axis.
    //***************************************************************

    // If the right arg is simple, the result is the same as the right arg
    if (IsSimple (aplTypeRht))
    {
        // Copy the right arg
        hGlbRes = CopySymGlbDir (MakePtrTypeGlb (hGlbRht));

        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // From here on, the right arg is ARRAY_NESTED.
    //***************************************************************

    // Calculate space needed for the dimensions
    ByteRes = sizeof (APLUINT) * aplRankCom;

    // Allocate space for the max shape of the items
    // N.B.:  Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbDimCom = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbDimCom)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemDimCom = MyGlobalLock (hGlbDimCom);

    // Trundle through the right arg calculating the shape
    //   of the common item
    for (aplNELMRes = 0, uRht = 0; uRht < aplNELMNstRht; uRht++)
    {
        // Split cases based upon the ptr type of the item
        switch (GetPtrTypeDir (((LPAPLNESTED) lpMemRht)[uRht]))
        {
            case PTRTYPE_STCONST:   // Ignore (simple) scalar items
                break;

            case PTRTYPE_HGLOBAL:
                // Get the attributes (Type, NELM, and Rank)
                //   of the item
                AttrsOfGlb (ClrPtrTypeDirAsGlb (((LPAPLNESTED) lpMemRht)[uRht]), NULL, &aplNELMSub, &aplRankSub, NULL);

                // If the item is not a scalar, ...
                if (aplRankSub > 0)
                {
                    LPAPLDIM lpMemSub;

                    // Lock the memory to get a ptr to it
                    lpMemSub = MyGlobalLock (ClrPtrTypeDirAsGlb (((LPAPLNESTED) lpMemRht)[uRht]));

                    // Skip over the header to the dimensions
                    lpMemSub = VarArrayBaseToDim (lpMemSub);

                    // Loop through the dimensions of the item
                    //   calculating the maximum of the corresponding dimensions
                    for (uSub = 0; uSub < aplRankSub; uSub++)
                        lpMemDimCom[uSub] = max (lpMemDimCom[uSub], lpMemSub[uSub]);
                    // We no longer need this ptr
                    MyGlobalUnlock (ClrPtrTypeDirAsGlb (((LPAPLNESTED) lpMemRht)[uRht])); lpMemSub = NULL;
                } // End IF

                break;

            defstop
                break;
        } // End SWITCH
    } // End IF/FOR

    // Trundle through the common item's shape and calculate its NELM
    if (aplRankCom > 0)
    {
        // Calculate the product of  the dimensions except for the last
        for (aplNELMComRest = 1, uCom = 0; uCom < (aplRankCom - 1); uCom++)
            aplNELMComRest *= lpMemDimCom[uCom];

        // Get the last dimension
        aplNELMComLast = lpMemDimCom[uCom];

        // Calculate the NELM of the common item
        aplNELMCom = aplNELMComRest * aplNELMComLast;
    } else
        aplNELMCom = aplNELMComLast = aplNELMComRest = 1;

    // Calculate the result NELM
    aplNELMRes = aplNELMCom * aplNELMRht;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_NESTED, aplNELMRes, aplRankRes);

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
    lpHeader->ArrType    = ARRAY_NESTED;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = 0;               // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Point to the result's dimension
    lpMemDimRes = VarArrayBaseToDim (lpMemRes);

    // Fill in the result's dimension

    // First, copy the dimensions from the right arg
    for (uRht = 0; uRht < aplRankRht; uRht++)
        lpMemDimRes[uRht] = lpMemDimRht[uRht];

    // Next, copy the dimensions from the common item
    for (uCom = 0; uCom < aplRankCom; uCom++)
        lpMemDimRes[uCom + aplRankRht] = lpMemDimCom[uCom];

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    // Handle prototypes
    if (aplNELMRes EQ 0)
    {
        // Get the handle of the right arg item (prototype)
        hGlbSub = ClrPtrTypeIndAsGlb ((LPAPLNESTED *) lpMemRht);

        // Get the attributes (Type, NELM, and Rank)
        //   of the right arg global
        AttrsOfGlb (hGlbSub, &aplTypeSub, NULL, &aplRankSub, NULL);

        // Split cases based upon the right arg's prototype's storage type
        switch (aplTypeSub)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_APA:
                *((LPAPLNESTED) lpMemRes) = GetSteZero ();

                break;

            case ARRAY_CHAR:
                *((LPAPLNESTED) lpMemRes) = GetSteBlank ();

                break;

            case ARRAY_HETERO:
            case ARRAY_NESTED:
                // Lock the memory to get a ptr to it
                lpMemSub = MyGlobalLock (hGlbSub);

                // Skip over the header and dimensions to the data
                lpMemSub = VarArrayBaseToData (lpMemSub, aplRankSub);

                // Save first item as prototype
                *((LPAPLNESTED) lpMemRes) = CopySymGlbInd (lpMemSub);

                // We no longer need this ptr
                MyGlobalUnlock (hGlbSub); lpMemSub = NULL;

                break;

            defstop
                break;
        } // End SWITCH

        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // Copy the data to the result
    //
    // At this point, the right arg is nested and non-empty
    //***************************************************************

    // Fill the entire result with PTR_REUSED in case we fail during processing
    for (uCom = 0; uCom < aplNELMRes; uCom++)
        ((LPAPLNESTED) lpMemRes)[uCom] = PTR_REUSED;

    // Loop through the elements of the right arg
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Split cases based upon the ptr type of the item
        switch (GetPtrTypeDir (((LPAPLNESTED) lpMemRht)[uRht]))
        {
            case PTRTYPE_STCONST:
                // Get the item as a prototype
                if (IsImmChr (((LPAPLHETERO) lpMemRht)[uRht]->stFlags.ImmType))
                    lpSymProto = GetSteBlank ();
                else
                    lpSymProto = GetSteZero ();

                // Loop through the rest of common item's elements
                //   saving the item's prototype
                for (uSubRest = 0; uSubRest < aplNELMComRest; uSubRest++)
                for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                    ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                       CopySymGlbDir (lpSymProto);

                // Overtake this scalar
                ((LPAPLNESTED) lpMemRes)[uRht * aplNELMCom] = ((LPAPLNESTED) lpMemRht)[uRht];

                break;

            case PTRTYPE_HGLOBAL:
                // Get the item's global handle
                hGlbSub = ClrPtrTypeDirAsGlb (((LPAPLNESTED) lpMemRht)[uRht]);

                // Lock the memory to get a ptr to it
                lpMemSub = MyGlobalLock (hGlbSub);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemSub)
                // Get the Array Type, NELM, and Rank
                aplTypeSub = lpHeader->ArrType;
                aplRankSub = lpHeader->Rank;
#undef  lpHeader

                // Skip over the header to the dimensions
                lpMemSub = VarArrayBaseToDim (lpMemSub);

                // Trundle through the right arg item's shape, ...
                if (aplRankSub > 0)
                {
                    // Calculate the product of the dimensions except for the last
                    for (aplNELMSubRest = 1, uSub = 0; uSub < (aplRankSub - 1); uSub++)
                        aplNELMSubRest *= *((LPAPLDIM) lpMemSub)++;

                    // Get the last dimension
                    aplNELMSubLast = *((LPAPLDIM) lpMemSub)++;
                } else
                    aplNELMSubLast = aplNELMSubRest = 1;

                // lpMemSub now points to the right arg item's data

                // Split cases based upon the right arg item's storage type
                switch (aplTypeSub)
                {
                    case ARRAY_BOOL:
                        // Get the prototype
                        lpSymProto = GetSteZero ();

                        // Get the two possible values
                        lpSym0 = GetSteZero ();
                        lpSym1 = GetSteOne ();

                        uBitMask = 0x01;

                        // Loop through the right arg item's elements
                        //   copying them to the result
                        for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                        for (uSubLast = 0; uSubLast < aplNELMSubLast; uSubLast++)
                        {
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              (uBitMask & *(LPAPLBOOL) lpMemSub) ? lpSym1 : lpSym0;

                            // Shift over the bit mask
                            uBitMask <<= 1;

                            // Check for end-of-byte
                            if (uBitMask EQ END_OF_BYTE)
                            {
                                uBitMask = 0x01;            // Start over
                                ((LPAPLBOOL) lpMemSub)++;   // Skip to next byte
                            } // End IF
                        } // End FOR/FOR

                        // Loop through the missing elements in the result (right arg item's cols)
                        //   copying the prototype
                        for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                        for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              CopySymGlbDir (lpSymProto);

                        // Loop through the missing elements in the result (right arg item's rows)
                        //   copying the prototype
                        for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                        for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              CopySymGlbDir (lpSymProto);
                        break;

                    case ARRAY_INT:
                        // Get the prototype
                        lpSymProto = GetSteZero ();

                        // Loop through the right arg item's elements
                        //   copying them to the result
                        for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                        for (uSubLast = 0; uSubLast < aplNELMSubLast; uSubLast++)
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              SymTabAppendInteger_EM (*((LPAPLINT) lpMemSub)++);

                        // Loop through the missing elements in the result (right arg item's cols)
                        //   copying the prototype
                        for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                        for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              CopySymGlbDir (lpSymProto);

                        // Loop through the missing elements in the result (right arg item's rows)
                        //   copying the prototype
                        for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                        for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              CopySymGlbDir (lpSymProto);
                        break;

                    case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemSub)
                        // Get the APA parameters
                        apaOffSub = lpAPA->Off;
                        apaMulSub = lpAPA->Mul;
#undef  lpAPA
                        // Get the prototype
                        lpSymProto = GetSteZero ();

                        // Loop through the right arg item's elements
                        //   copying them to the result
                        for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                        for (uSubLast = 0; uSubLast < aplNELMSubLast; uSubLast++)
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              SymTabAppendInteger_EM (apaOffSub + apaMulSub * (uSubLast + (uSubRest * aplNELMSubLast)));

                        // Loop through the missing elements in the result (right arg item's cols)
                        //   copying the prototype
                        for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                        for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              CopySymGlbDir (lpSymProto);

                        // Loop through the missing elements in the result (right arg item's rows)
                        //   copying the prototype
                        for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                        for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              CopySymGlbDir (lpSymProto);
                        break;

                    case ARRAY_FLOAT:
                        // Get the prototype
                        lpSymProto = GetSteZero ();

                        // Loop through the right arg item's elements
                        //   copying them to the result
                        for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                        for (uSubLast = 0; uSubLast < aplNELMSubLast; uSubLast++)
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              SymTabAppendFloat_EM (*((LPAPLFLOAT) lpMemSub)++);

                        // Loop through the missing elements in the result (right arg item's cols)
                        //   copying the prototype
                        for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                        for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              CopySymGlbDir (lpSymProto);

                        // Loop through the missing elements in the result (right arg item's rows)
                        //   copying the prototype
                        for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                        for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              CopySymGlbDir (lpSymProto);
                        break;

                    case ARRAY_CHAR:
                        // Get the prototype
                        lpSymProto = GetSteBlank ();

                        // Loop through the right arg item's elements
                        //   copying them to the result
                        for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                        for (uSubLast = 0; uSubLast < aplNELMSubLast; uSubLast++)
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              SymTabAppendChar_EM (*((LPAPLCHAR) lpMemSub)++);

                        // Loop through the missing elements in the result (right arg item's cols)
                        //   copying the prototype
                        for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                        for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              CopySymGlbDir (lpSymProto);

                        // Loop through the missing elements in the result (right arg item's rows)
                        //   copying the prototype
                        for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                        for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              CopySymGlbDir (lpSymProto);
                        break;

                    case ARRAY_HETERO:
                    case ARRAY_NESTED:
                        // Get the right arg item's first element global memory handle
                        hGlbProto = ClrPtrTypeIndAsGlb ((LPAPLNESTED) lpMemSub);

                        // Calculate its prototype
                        hGlbProto =
                          MakeMonPrototype_EM (hGlbProto,   // Proto arg handle
                                               lptkFunc,    // Ptr to function token
                                               MP_CHARS);   // CHARs allowed
                        if (!hGlbProto)
                        {
                            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                                       lptkFunc);
                            goto ERROR_EXIT;
                        } // End IF

                        // Set the ptr bits
                        hGlbProto = MakePtrTypeGlb (hGlbProto);

                        // Loop through the right arg item's elements
                        //   copying them to the result
                        for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                        for (uSubLast = 0; uSubLast < aplNELMSubLast; uSubLast++)
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              CopySymGlbDir (*((LPAPLNESTED) lpMemSub)++);

                        // Loop through the missing elements in the result (right arg item's cols)
                        //   copying the prototype
                        for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                        for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              CopySymGlbDir (hGlbProto);

                        // Loop through the missing elements in the result (right arg item's rows)
                        //   copying the prototype
                        for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                        for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                              CopySymGlbDir (hGlbProto);
                        // We no longer need this storage
                        FreeResultGlobalVar (ClrPtrTypeDirAsGlb (hGlbProto)); hGlbProto = NULL;

                        break;

                    defstop
                        break;
                } // End SWITCH

                // We no longer need this ptr
                MyGlobalUnlock (hGlbSub); lpMemSub = NULL;

                break;

            defstop
                break;
        } // End FOR/SWITCH
    } // End FOR
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
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // If there's an axis, transpose the result
    if (hGlbAxis)
    {
        LPPL_YYSTYPE lpYYRes2,      // Ptr to secondary result
                     lpYYRes3;      // Ptr to tertiary result
        APLBOOL      bQuadIO;       // []IO

        // Calculate space needed for left arg
        ByteRes = CalcArraySize (ARRAY_INT, aplRankRes, 1);

        // Allocate space for the max shape of the items
        // N.B.:  Conversion from APLUINT to UINT.
        Assert (ByteRes EQ (UINT) ByteRes);
        hGlbLft = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (!hGlbLft)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemLft = MyGlobalLock (hGlbLft);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemLft)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_INT;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = 0;               // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplRankRes;
        lpHeader->Rank       = 1;
#undef  lpHeader

        // Fill in the dimension
        *VarArrayBaseToDim (lpMemLft) = aplRankRes;

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, 1);

        // Lock the memory to get a ptr to it
        lpMemAxis = MyGlobalLock (hGlbAxis);

        // Copy the values from lpMemAxis
        CopyMemory (lpMemLft, lpMemAxis, (UINT) aplRankRes * sizeof (APLUINT));

        // Get the current value of []IO
        bQuadIO = GetQuadIO ();

        // Because lpMemAxis is in origin-0, and the left arg to
        //   dyadic transpose is []IO-sensitive, we must
        //   loop through the left arg and add in []IO
        for (uCom = 0; uCom < aplRankRes; uCom++)
            *lpMemLft++ += bQuadIO;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;

        // Transpose the values

        // Allocate a new YYRes
        lpYYRes2 = YYAlloc ();

        // Fill in the left arg token
        lpYYRes2->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes2->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////lpYYRes2->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes2->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbLft);
        lpYYRes2->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        // Transpose the values
        lpYYRes3 = PrimFnDydCircleSlope_EM_YY (&lpYYRes2->tkToken,  // Ptr to left arg token
                                                lptkFunc,           // Ptr to function token
                                               &lpYYRes->tkToken,   // Ptr to right arg token
                                                NULL);              // Ptr to axis token (may be NULL)
        // We no longer need this storage
        FreeResult (&lpYYRes->tkToken);  YYFree (lpYYRes);  lpYYRes  = NULL;
        FreeResult (&lpYYRes2->tkToken); YYFree (lpYYRes2); lpYYRes2 = NULL; // N.B.:  This frees hGlbLft, too.

        // Copy to result var
        lpYYRes = lpYYRes3; lpYYRes3 = NULL;
    } // End IF

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto TAIL_EXIT;

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

    bRet = FALSE;
TAIL_EXIT:
    if (hGlbDimCom)
    {
        if (lpMemDimCom)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbDimCom); lpMemDimCom = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbDimCom); hGlbDimCom = NULL;
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

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht);  lpMemRht  = NULL;
    } // End IF

    if (bRet)
        return lpYYRes;
    else
        return NULL;
} // End PrimFnMonRightShoeGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydRightShoe_EM_YY
//
//  Primitive function for dyadic RightShoe ("pick")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydRightShoe_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydRightShoe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
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
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkSym->stData.stGlbData));

                return PrimFnDydRightShoeGlb_EM_YY (lptkLftArg,
                                                    ClrPtrTypeDirAsGlb (lptkRhtArg->tkData.tkSym->stData.stGlbData),
                                                    lptkFunc);
            } // End IF

            // Handle the immediate case

            // If the left arg is valid, return the immediate in the right arg
            return PrimFnDydRightShoeImm_EM_YY (lptkRhtArg->tkData.tkSym->stFlags.ImmType,
                                                lptkRhtArg->tkData.tkSym->stData.stLongest,
                                                lptkLftArg,
                                                lptkFunc);
        case TKT_VARIMMED:
            // If the left arg is valid, return the immediate in the right arg
            return PrimFnDydRightShoeImm_EM_YY (lptkRhtArg->tkFlags.ImmType,
                                                lptkRhtArg->tkData.tkLongest,
                                                lptkLftArg,
                                                lptkFunc);
        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

            return PrimFnDydRightShoeGlb_EM_YY (lptkLftArg,
                                                ClrPtrTypeDirAsGlb (lptkRhtArg->tkData.tkGlbData),
                                                lptkFunc);
        defstop
            return NULL;
    } // End SWITCH
} // End PrimFnDydRightShoe_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydRightShoeImm_EM_YY
//
//  Dyadic RightShoe ("pick") on an immediate right arg
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydRightShoeImm_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydRightShoeImm_EM_YY
    (IMM_TYPES  immTypeRht,         // The immediate type (see IMM_TYPES)
     APLLONGEST aplLongestRht,      // The immediate value
     LPTOKEN    lptkLftArg,         // Ptr to left arg token
     LPTOKEN    lptkFunc)           // Ptr to function token

{
    APLSTYPE     aplTypeLft;        // Left arg storage type
    APLNELM      aplNELMLft;        // Left arg NELM
    APLRANK      aplRankLft;        // Left arg rank
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    HGLOBAL      hGlbLft = NULL;    // Left arg global memory handle
    LPVOID       lpMemLft = NULL;   // Ptr to left arg global memory

    // Get the attributes (Type, NELM, and Rank)
    //   of the left arg
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);

    // Check for LEFT RANK ERROR
    if (aplRankLft > 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // If the left arg is simple, it must be empty (i.e., {zilde} or '')
    if (IsSimple (aplTypeLft))
    {
        if (aplNELMLft NE 0)
        {
            ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        goto NORMAL_EXIT;
    } // End IF

    // Check for LEFT DOMAIN ERROR
    if (!IsNested (aplTypeLft))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // From here on the left arg is a nested scalar or vector

    // Take into account nested prototypes
    aplNELMLft = max (aplNELMLft, 1);

    // Get left arg global ptr
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);

    // Skip over the herader and dimensions
    lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

    if (!PrimFnDydRightShoeGlbImm_EM (aplNELMLft,           // Left arg NELM
                                      lpMemLft,             // Ptr to left arg global memory
                                      0,                    // Left arg starting index
                                      immTypeRht,           // Right arg singleton type
                                      aplLongestRht,        // Right arg singleton value
                                      lptkFunc))            // Ptr to function token
        goto ERROR_EXIT;
NORMAL_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = immTypeRht;
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkLongest  = aplLongestRht;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
ERROR_EXIT:
    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnDydRightShoeImm_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydRightShoeGlbImm_EM
//
//  Dyadic RightShoe ("pick") on a left arg global memory handle
//    and right arg immediate
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydRightShoeGlbImm_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydRightShoeGlbImm_EM
    (APLNELM    aplNELMLft,     // Left arg NELM
     LPVOID     lpMemLft,       // Ptr to left arg global memory
     APLUINT    uLft,           // Left arg starting index
     IMM_TYPES  immTypeRht,     // Right arg singleton type
     APLLONGEST aplLongestRht,  // Right arg singleton value
     LPTOKEN    lptkFunc)       // Ptr to function token

{
    APLSTYPE aplTypeSub;        // Left arg item storage type
    APLNELM  aplNELMSub;        // Left arg item NELM
    APLRANK  aplRankSub;        // Left arg item rank

    // Loop through the left arg to ensure that it is
    //   a nested vector of zildes
    for (; uLft < aplNELMLft; uLft++)
    {
        // Split cases based upon the ptr type of the element
        switch (GetPtrTypeDir (((LPAPLNESTED) lpMemLft)[uLft]))
        {
            case PTRTYPE_STCONST:
                ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                           lptkFunc);
                return FALSE;

            case PTRTYPE_HGLOBAL:
                // Ensure that this element is a simple empty vector

                // Get the attributes (Type, NELM, and Rank)
                //   of the item
                AttrsOfGlb (ClrPtrTypeDirAsGlb (((LPAPLNESTED) lpMemLft)[uLft]), &aplTypeSub, &aplNELMSub, &aplRankSub, NULL);

                // Check for LEFT RANK ERROR
                if (aplRankSub NE 1)
                {
                    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                               lptkFunc);
                    return FALSE;
                } // End IF

                // Check for LEFT LENGTH ERROR
                if (aplNELMSub NE 0)
                {
                    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                               lptkFunc);
                    return FALSE;
                } // End IF

                // Check for LEFT DOMAIN ERROR
                if (!IsSimple (aplTypeSub))     // Note we allow empty char
                {
                    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                               lptkFunc);
                    return FALSE;
                } // End IF

                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR

    return TRUE;
} // End PrimFnDydRightShoeGlbImm_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydRightShoeGlb_EM_YY
//
//  Dyadic RightShoe ("pick") on a right arg global memory handle
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydRightShoeGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydRightShoeGlb_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     HGLOBAL hGlbRht,               // Handle to right arg
     LPTOKEN lptkFunc)              // Ptr to function token

{
    // Split cases based upon the left arg's token type
    switch (lptkLftArg->tkFlags.TknType)
    {
        HGLOBAL hGlbLft;

        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkLftArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkLftArg->tkData.tkSym->stFlags.Imm)
            {
                // Get the left arg global memory handle
                hGlbLft = lptkLftArg->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbLft));

                return PrimFnDydRightShoeGlbGlb_EM_YY
                       (ClrPtrTypeDirAsGlb (hGlbLft),           // Left arg global memory handle
                        hGlbRht,                                // Right arg global memory handle
                        lptkFunc,                               // Ptr to function token
                        FALSE,                                  // TRUE iff array assignment
                        ARRAY_ERROR,                            // Set arg storage type
                        NULL,                                   // Set arg global memory handle/LPSYMENTRY (NULL if immediate)
                        0);                                     // Set arg immediate value
            } // End IF

            // Handle the immediate case
            return PrimFnDydRightShoeImmGlb_EM_YY
                   (lptkLftArg->tkData.tkSym->stFlags.ImmType,  // Immediate type
                    lptkLftArg->tkData.tkSym->stData.stLongest, // Immediate value
                    hGlbRht,                                    // Right arg global memory handle
                    lptkFunc);                                  // Ptr to function token
        case TKT_VARIMMED:
            return PrimFnDydRightShoeImmGlb_EM_YY
                   (lptkLftArg->tkFlags.ImmType,                // Immediate type
                    lptkLftArg->tkData.tkLongest,               // Immediate value
                    hGlbRht,                                    // Right arg global memory handle
                    lptkFunc);                                  // Ptr to function token
        case TKT_VARARRAY:
            // Get the left arg global memory handle
            hGlbLft = lptkLftArg->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbLft));

            return PrimFnDydRightShoeGlbGlb_EM_YY
                   (ClrPtrTypeDirAsGlb (hGlbLft),               // Left arg global memory handle
                    hGlbRht,                                    // Right arg global memory handle
                    lptkFunc,                                   // Ptr to function token
                    FALSE,                                      // TRUE iff array assignment
                    ARRAY_ERROR,                                // Set arg storage type
                    NULL,                                       // Set arg global memory handle/LPSYMENTRY (NULL if immediate)
                    0);                                         // Set arg immediate value
        defstop
            return NULL;
    } // End SWITCH
} // End PrimFnDydRightShoeGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydRightShoeImmGlb_EM_YY
//
//  Dyadic RightShoe ("pick") on a left arg immediate
//    and right arg global memory handle
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydRightShoeImmGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydRightShoeImmGlb_EM_YY
    (IMM_TYPES  immTypeLft,             // Left arg immediate type (see IMM_TYPES)
     APLLONGEST aplLongestLft,          // Left arg immediate value
     HGLOBAL    hGlbRht,                // Right arg global memory handle
     LPTOKEN    lptkFunc)               // Ptr to function token

{
    APLSTYPE     aplTypeRht;        // Right arg storage type
    APLNELM      aplNELMRht;        // Right arg NELM
    APLRANK      aplRankRht;        // Right arg rank
    IMM_TYPES    immTypeRes;        // Result immediate type (see IMM_TYPES)
    APLLONGEST   aplLongestRes;     // Result immediate value
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    BOOL         bRet;              // TRUE iff FloatToAplint_SCT is valid
    APLBOOL      bQuadIO;           // []IO
    HGLOBAL      hGlbRes = NULL;    // Result global memory handle

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg global
    AttrsOfGlb (hGlbRht, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for LEFT LENGTH ERROR
    if (aplRankRht NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Split cases based upon the left arg immediate type
    switch (immTypeLft)
    {
        case IMMTYPE_FLOAT:
            // Attempt to convert the float to an integer using System CT
            aplLongestLft = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestLft, &bRet);
            if (!bRet)
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                return NULL;
            } // End IF

            // Fall through to common code

        case IMMTYPE_BOOL:
        case IMMTYPE_INT:
            // Get the current value of []IO
            bQuadIO = GetQuadIO ();

            // Convert to origin-0
            aplLongestLft -= bQuadIO;

            // Ensure that the index is within range
            // N.B.:  Because APLLONGEST is unsigned, we don't have to worry about negatives
            if (aplLongestLft >= aplNELMRht)
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                return NULL;
            } // End IF

            // Extract an element from the right arg
            GetNextValueGlb (hGlbRht,               // Right arg global memory handle
                             aplLongestLft,         // Index
                            &hGlbRes,               // Ptr to result as HGLOBAL
                            &aplLongestRes,         // Ptr to result immediate value
                            &immTypeRes);           // Ptr to result immediate type
            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // If the result is an HGLOBAL
            if (hGlbRes)
            {
                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDir (MakePtrTypeGlb (hGlbRes));
                lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
            } else
            {
                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkFlags.ImmType   = immTypeRes;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;         // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkLongest  = aplLongestRes;
                lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
            } // End IF/ELSE

            break;

        case IMMTYPE_CHAR:
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                       lptkFunc);
            return NULL;

        defstop
            break;
    } // End SWITCH

    return lpYYRes;
} // End PrimFnDydRightShoeImmGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydRightShoeGlbGlb_EM_YY
//
//  Dyadic RightShoe ("pick") on a left arg global memory handle
//    and right arg global memory handle
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydRightShoeGlbGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydRightShoeGlbGlb_EM_YY
    (HGLOBAL    hGlbLft,                // Left  arg global memory handle
     HGLOBAL    hGlbRht,                // Right ...
     LPTOKEN    lptkFunc,               // Ptr to function token
     BOOL       bArraySet,              // TRUE iff in array assignment
     APLSTYPE   aplTypeSet,             // Set arg storage type
     HGLOBAL    hGlbSet,                // Set arg global memory handle/LPSYMENTRY (NULL if immediate)
     APLLONGEST aplLongestSet)          // Set arg immediate value

{
    APLSTYPE     aplTypeLft,            // Left  arg storage type
                 aplTypeRht,            // Right ...
                 aplTypePro;            // Promoted  ...
    APLNELM      aplNELMLft,            // Left  arg NELM
                 aplNELMNstLft,         // Left  arg NELM if nested
                 aplNELMRht;            // Right arg NELM
    APLRANK      aplRankLft,            // Left  arg rank
                 aplRankRht;            // Right arg rank
    LPPL_YYSTYPE lpYYRes = NULL;        // Ptr to the result
    LPVOID       lpMemLft = NULL,       // Ptr to left arg global memory
                 lpMemRht = NULL;       // Ptr to right ...
    LPAPLDIM     lpMemDimRht;           // Ptr to right arg dimensions
    APLUINT      uLft;                  // Loop counter
    BOOL         bRet = TRUE;           // TRUE iff result is valid
    APLBOOL      bQuadIO;               // []IO

    // Get the attributes (Type, NELM, and Rank)
    //   of the left arg global
    AttrsOfGlb (hGlbLft, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);

    // Check for LEFT RANK ERROR
    if (aplRankLft > 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Lock the memory to get a ptr to it
    lpMemLft = MyGlobalLock (hGlbLft);

    // Skip over the header and dimensions to the data
    lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

    // Take into account nested prototypes
    if (IsNested (aplTypeLft))
        aplNELMNstLft = max (aplNELMLft, 1);
    else
        aplNELMNstLft = aplNELMLft;

    // Loop through the elements of the left arg
    //   reaching into the right arg
    for (uLft = 0; uLft < aplNELMNstLft; uLft++)
    {
        APLLONGEST aplLongestSubLft,        // Left arg item immediate value
                   aplLongestSubRht,        // Right arg item immediate value
                   aplLongestPrvLft;        // Previous left arg item index
        IMM_TYPES  immTypeSubLft,           // Left arg item immediate type
                   immTypeSubRht;           // Right arg item immediate type
        HGLOBAL    hGlbSubLft,              // Left arg item global memory handle
                   hGlbSubRht,              // Right ...
                   hGlbPrvRht;              // Previous right arg global memory handle
        UINT       uBitMask;                // Bit mask for looping through Booleans

        // Get the attributes (Type, NELM, and Rank)
        //   of the right arg global
        AttrsOfGlb (hGlbRht, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

        // Lock the memory to get a ptr to it
        lpMemRht = MyGlobalLock (hGlbRht);

        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

        // Get the index value from the left arg
        GetNextValueGlb (hGlbLft,               // Left arg global memory handle
                         uLft,                  // Index
                        &hGlbSubLft,            // Ptr to result as HGLOBAL (may be NULL if singleton or simple)
                        &aplLongestSubLft,      // Ptr to result as singleton or simple value
                        &immTypeSubLft);        // Ptr to result as singleton or simple type
        // If index value from the left arg is a global value, ...
        if (hGlbSubLft)
        {
            APLSTYPE aplTypeSubLft;             // Left arg item storage type
            APLNELM  aplNELMSubLft;             // Left arg item NELM
            APLRANK  aplRankSubLft;             // Left arg item rank
            LPVOID   lpMemSubLft;               // Ptr to left arg item global memory
            APLINT   apaOffSubLft,              // Left arg item APA offset
                     apaMulSubLft,              // Left arg item APA multiplier
                     iDim;                      // Loop counter
            APLUINT  aplWValSubLft,             // Left arg item weighting value
                     aplTmpSubLft;              // Left arg item temporary value

            // Get the attributes (Type, NELM, and Rank)
            //   of the left arg item global
            AttrsOfGlb (hGlbSubLft, &aplTypeSubLft, &aplNELMSubLft, &aplRankSubLft, NULL);

            // Check for left RANK ERROR
            if (aplRankSubLft > 1)
            {
                ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Check for LEFT LENGTH ERROR
            if (aplNELMSubLft NE aplRankRht)
            {
                ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Check for LEFT DOMAIN ERROR
            if (!IsSimpleNum (aplTypeSubLft))
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Lock the memory to get a ptr to it
            lpMemSubLft = MyGlobalLock (hGlbSubLft);

            // Skip over the header and dimensions to the data
            lpMemSubLft = VarArrayBaseToData (lpMemSubLft, aplRankSubLft);

            // Initialize accumulator to identity element for addition
            aplLongestSubLft = 0;

            // Initialize weighting value to identity element for multiplication
            aplWValSubLft = 1;

            // Split cases based upon the left arg item storage type
            switch (aplTypeSubLft)
            {
                case ARRAY_BOOL:
                    // Loop through the right arg dimensions
                    for (iDim = aplRankRht - 1; iDim >= 0; iDim--)
                    {
                        uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) iDim);

                        // Get the left arg item value in origin-0
                        aplTmpSubLft = ((uBitMask & ((LPAPLBOOL) lpMemSubLft)[iDim >> LOG2NBIB]) ? 1 : 0) - bQuadIO;

                        // Ensure the indices are within range
                        if (lpMemDimRht[iDim] <= aplTmpSubLft)
                        {
                            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                            bRet = FALSE;

                            break;
                        } // End IF

                        // Add into accumulator
                        aplLongestSubLft += aplTmpSubLft * aplWValSubLft;

                        // Shift the weighting value over
                        aplWValSubLft *= lpMemDimRht[iDim];
                    } // End FOR

                    break;

                case ARRAY_INT:
                    // Loop through the right arg dimensions
                    for (iDim = aplRankRht - 1; iDim >= 0; iDim--)
                    {
                        // Get the left arg item value in origin-0
                        aplTmpSubLft = ((LPAPLUINT) lpMemSubLft)[iDim] - bQuadIO;

                        // Ensure the indices are within range
                        if (lpMemDimRht[iDim] <= aplTmpSubLft)
                        {
                            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                            bRet = FALSE;

                            break;
                        } // End IF

                        // Add into accumulator
                        aplLongestSubLft += aplTmpSubLft * aplWValSubLft;

                        // Shift the weighting value over
                        aplWValSubLft *= lpMemDimRht[iDim];
                    } // End FOR

                    break;

                case ARRAY_FLOAT:
                    // Loop through the right arg dimensions
                    for (iDim = aplRankRht - 1; iDim >= 0; iDim--)
                    {
                        // Attempt to convert the float to an integer using System CT
                        aplTmpSubLft = FloatToAplint_SCT (((LPAPLFLOAT) lpMemSubLft)[iDim], &bRet) - bQuadIO;

                        // Ensure the indices are within range
                        if ((!bRet) || lpMemDimRht[iDim] <= aplTmpSubLft)
                        {
                            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                       lptkFunc);
                            bRet = FALSE;

                            break;
                        } // End IF

                        // Add into accumulator
                        aplLongestSubLft += aplTmpSubLft * aplWValSubLft;

                        // Shift the weighting value over
                        aplWValSubLft *= lpMemDimRht[iDim];
                    } // End FOR

                    break;

                case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemSubLft)
                    // Get the APA parameters
                    apaOffSubLft = lpAPA->Off;
                    apaMulSubLft = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the right arg dimensions
                    for (iDim = aplRankRht - 1; iDim >= 0; iDim--)
                    {
                        // Get the left arg item value in origin-0
                        aplTmpSubLft = (apaOffSubLft + apaMulSubLft * iDim) - bQuadIO;

                        // Ensure the indices are within range
                        if (lpMemDimRht[iDim] <= aplTmpSubLft)
                        {
                            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                            bRet = FALSE;

                            break;
                        } // End IF

                        // Add into accumulator
                        aplLongestSubLft += aplTmpSubLft * aplWValSubLft;

                        // Shift the weighting value over
                        aplWValSubLft *= lpMemDimRht[iDim];
                    } // End FOR

                    break;

                case ARRAY_CHAR:
                    if (aplNELMSubLft EQ 0)     // Allow empty chars
                        break;

                    // Fall through to common code

                case ARRAY_HETERO:
                case ARRAY_NESTED:
                    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                               lptkFunc);
                    bRet = FALSE;

                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbSubLft); lpMemSubLft = NULL;

            // Check for error
            if (!bRet)
                goto ERROR_EXIT;
        } else
        {
            // The left arg is an immediate or simple value

            // Ensure that the right arg is a vector
            if (aplRankRht NE 1)
            {
                ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Ensure that the left arg immediate value is numeric
            if (!IsImmNum (immTypeSubLft))
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Ensure that the left arg immediate value can convert to an integer
            if (IsImmFlt (immTypeSubLft))
            {
                // Attempt to convert the float to an integer using System CT
                aplLongestSubLft = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestSubLft, &bRet);
                if (!bRet)
                {
                    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                               lptkFunc);
                    goto ERROR_EXIT;
                } // End IF
            } // End IF

            // Convert the index to origin-0
            aplLongestSubLft -= bQuadIO;

            // Ensure that the index is within range
            // N.B.:  Because APLLONGEST is unsigned, we don't have to worry about negatives
            if (aplLongestSubLft >= aplNELMRht)
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF
        } // End IF/ELSE

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = lpMemDimRht = NULL;

        // Get the indexed value from the right arg
        GetNextValueGlb (hGlbRht,               // Right arg global memory handle
                         aplLongestSubLft,      // Index
                        &hGlbSubRht,            // Ptr to result as HGLOBAL (may be NULL if singleton)
                        &aplLongestSubRht,      // Ptr to result as singleton value
                        &immTypeSubRht);        // Ptr to result as singleton type
        // If the right item arg is a global, ...
        if (hGlbSubRht)
        {
            // If we're assigning a new value
            //   and this is the last element in the
            //   left arg, ...
            if (bArraySet
             && uLft EQ (aplNELMNstLft - 1))
            {
                // Replace the <aplLongestSubLft> element in hGlbRht
                //   with <aplLongestSet> or <hGlbSet> depending upon <aplTypeSet>

                // Get the attributes (Type, NELM, and Rank)
                //   of the right arg global
                AttrsOfGlb (hGlbRht, NULL, NULL, &aplRankRht, NULL);

                // Lock the memory to get a ptr to it
                lpMemRht = MyGlobalLock (hGlbRht);

                // Skip over the header and dimensions to the data
                lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

                // If the set arg is simple non-heterogeneous, ...
                if (IsSimpleNH (aplTypeSet))
                    ((LPAPLNESTED) lpMemRht)[aplLongestSubLft] =
                      MakeSymEntry_EM (TranslateArrayTypeToImmType (aplTypeSet),
                                      &aplLongestSet,
                                       lptkFunc);
                else
                    ((LPAPLNESTED) lpMemRht)[aplLongestSubLft] =
                      CopySymGlbDir (hGlbSet);

                // We no longer need this ptr
                MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

                // Free the old value
                FreeResultGlobalVar (hGlbSubRht); hGlbSubRht = NULL;

                // Return pseudo-value
                lpYYRes = PTR_REUSED;
            } else
            {
                // In case we need to know where this item
                //   came from, save its index & global memory handle
                aplLongestPrvLft = aplLongestSubLft;
                hGlbPrvRht = hGlbRht;

                // Copy as new right arg global memory handle
                hGlbRht = hGlbSubRht;
            } // End IF/ELSE
        } else
        {
            // The right arg item is an immediate or simple value

            // If the left arg is nested, ...
            // Validate that the remaining left arg items are {zilde}s
            if (IsNested (aplTypeLft))
            {
                if (!PrimFnDydRightShoeGlbImm_EM (aplNELMLft,           // Left arg NELM
                                                  lpMemLft,             // Ptr to left arg global memory
                                                  uLft + 1,             // Left arg starting index
                                                  immTypeSubRht,        // Right arg singleton type
                                                  aplLongestSubRht,     // Right arg singleton value
                                                  lptkFunc))            // Ptr to function token
                    goto ERROR_EXIT;
            } else
            if (aplNELMLft NE 1)
            {
                ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF/ELSE/...

            // If we're assigning a new value, ...
            if (bArraySet)
            {
                // If the right arg should be promoted, ...
                if (QueryPromote (aplTypeRht, aplTypeSet, &aplTypePro))
                {
                    LPAPLNESTED lpMemPrvRht;
                    APLRANK     aplRankPrvRht;

                    // Promote the right arg
                    if (!TypePromoteGlb_EM (&hGlbRht, aplTypePro, lptkFunc))
                        goto ERROR_EXIT;

                    // Save the new type
                    aplTypeRht = aplTypePro;

                    // Lock the memory to get a ptr to it
                    lpMemPrvRht = MyGlobalLock (hGlbPrvRht);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemPrvRht)
                    // Get the rank of the previous right arg global
                    aplRankPrvRht = lpHeader->Rank;
#undef  lpHeader
                    // Skip over the header and dimensions to the data
                    lpMemPrvRht = VarArrayBaseToData (lpMemPrvRht, aplRankPrvRht);

                    // Save the new hGlbRht in the array from which we got it
                    //   (i.e., the <aplLongestPrvLft> entry in <hGlbPrvRht>)
                    lpMemPrvRht[aplLongestPrvLft] = MakePtrTypeGlb (hGlbRht);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbPrvRht); lpMemPrvRht = NULL;
                } // End IF

                // Lock the memory to get a ptr to it
                lpMemRht = MyGlobalLock (hGlbRht);

                // Get the array rank
#define lpHeader        ((LPVARARRAY_HEADER) lpMemRht)
                aplRankRht = lpHeader->Rank;
#undef  lpHeader
                // Skip over the header and dimensions to the data
                lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

                // Replace the <aplLongestSubLft> element in hGlbRht
                //   with <aplLongestSet> or <hGlbSet> depending upon <aplTypeRht>
                ArrayIndexReplace (aplTypeRht,      // Right arg storage type
                                   lpMemRht,        // Ptr to right arg global memory
                                   aplLongestSubLft,// Index into right arg
                                   aplTypeSet,      // Set arg storage type
                                   aplLongestSet,   // Set arg immediate value
                                   hGlbSet,         // Set arg global memory handle
                                   lptkFunc);       // Ptr to function token
                // We no longer need this ptr
                MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

                // Return pseudo-value
                lpYYRes = PTR_REUSED;
            } else
            {
                // Allocate a new YYRes
                lpYYRes = YYAlloc ();

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkFlags.ImmType   = immTypeSubRht;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkLongest  = aplLongestSubRht;
                lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
            } // End IF/ELSE

            goto NORMAL_EXIT;
        } // End IF/ELSE
    } // End FOR

    // If we're not assigning a new value, ...
    if (!bArraySet)
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDirAsGlb (hGlbRht);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE

////goto NORMAL_EXIT;

ERROR_EXIT:
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

    return lpYYRes;
} // End PrimFnDydRightShoeGlbGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_rshoe.c
//***************************************************************************
