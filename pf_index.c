//***************************************************************************
//  NARS2000 -- Primitive Function -- Bracket Indexing
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
//  $ArrayIndexRef_EM_YY
//
//  Index reference of an array, e.g., A[L]  or  A[L1;...;Ln]
//  The form A[L] can use either (or both) Reach and Scatter index reference,
//    whereas the form A[L1;...;Ln] can use Rectangular index reference only.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexRef_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ArrayIndexRef_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    APLSTYPE     aplTypeLft,        // Left arg storage type
                 aplTypeRes,        // Result   ...
                 aplTypeSub;        // Right arg item ...
    APLNELM      aplNELMLft,        // Left arg NELM
                 aplNELMRht,        // Right ...
                 aplNELMSub;        // Right arg item ...
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankSub;        // Right arg item ...
    HGLOBAL      hGlbLft = NULL,    // Left arg global memory handle
                 hGlbRht = NULL,    // Right ...
                 hGlbSub = NULL,    // Right arg item ...
                 hGlbRes = NULL,    // Result         ...
                 hGlbItm;           // Subitem global memory handle
    LPVOID       lpMemLft = NULL,   // Ptr to left arg global memory
                 lpMemRht = NULL,   // Ptr to right ...
                 lpMemSub = NULL,   // Ptr to right item ...
                 lpMemRes = NULL;   // Ptr to result     ...
    APLUINT      ByteRes,           // # bytes in the result
                 uSub;              // Loop counter
    BOOL         bRet = TRUE,       // TRUE iff result is valid
                 bQuadIO;           // []IO
    LPPL_YYSTYPE lpYYRes = NULL,    // Ptr to the result
                 lpYYItm;           // Ptr to temporary result
    LPTOKEN      lptkFunc = lptkRhtArg; // Ptr to function token
    APLLONGEST   aplLongestLft,     // Left arg immediate value
                 aplLongestSub,     // Right arg item ...
                 aplLongestItm;     // Right arg subitem ...
    IMM_TYPES    immTypeItm;        // Right arg subitem immediate type
    UINT         uBitMask,          // Bit mask for when looping through Booleans
                 uBitIndex;         // Bit index ...

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, NULL,        &aplNELMRht, NULL,        NULL);

    // Map APA left arg to INT result
    if (aplTypeLft EQ ARRAY_APA)
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeLft;

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
                    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // The right arg is always a list of rank 1

    //***************************************************************
    // If the right arg list is empty, ...
    //***************************************************************
    if (aplNELMRht EQ 0)
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // If the left arg is immediate, ...
        if (hGlbLft EQ NULL)
        {
            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType   = TranslateArrayTypeToImmType (aplTypeLft);
////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkLongest  = aplLongestLft;
            lpYYRes->tkToken.tkCharIndex       = lptkRhtArg->tkCharIndex;
        } else
        {
            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDirGlb (hGlbLft);
            lpYYRes->tkToken.tkCharIndex       = lptkRhtArg->tkCharIndex;
        } // End IF/ELSE
    } else
    //***************************************************************
    // If the right arg list is a singleton, ...
    //***************************************************************
    if (aplNELMRht EQ 1)
    {
        APLSTYPE aplTypeItm;            // Right arg subitem storage type
        APLNELM  aplNELMItm;            // Right arg subitem NELM
        APLRANK  aplRankItm;            // Right arg subitem rank

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, 1);

        // Split cases based upon the right arg item ptr type
        switch (GetPtrTypeInd (lpMemRht))
        {
            case PTRTYPE_STCONST:
                // Set the vars for an STE
                hGlbSub = NULL;
                aplTypeSub = TranslateImmTypeToArrayType ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType);
                aplNELMSub = 1;
                aplRankSub = 0;
                aplLongestSub = (*(LPAPLHETERO) lpMemRht)->stData.stLongest;

                break;

            case PTRTYPE_HGLOBAL:
                // Set the vars for an HGLOBAL
                hGlbSub = ClrPtrTypeIndGlb (lpMemRht);
                AttrsOfGlb (hGlbSub, &aplTypeSub, &aplNELMSub, &aplRankSub, NULL);
                lpMemSub = MyGlobalLock (hGlbSub);

                break;

            defstop
                break;
        } // End SWITCH

        // Handle obvious DOMAIN ERRORs
        if (aplTypeSub EQ ARRAY_HETERO
         || (aplTypeSub EQ ARRAY_CHAR
          && aplNELMSub NE 0))
        {
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        //***************************************************************
        // Split off cases
        //***************************************************************

        //***************************************************************
        // If the right arg item is immediate, do rectangular indexing.
        // The left arg must be a vector.
        //***************************************************************
        if (hGlbSub EQ NULL)
        {
            lpYYRes = ArrayIndexRefRhtImm_EM_YY (hGlbLft,           // Left arg global memory handle
                                                 aplTypeLft,        // Left arg storage type
                                                 aplNELMLft,        // Left arg NELM
                                                 aplRankLft,        // Left arg rank
                                                 aplTypeSub,        // Right arg storage type
                                                 aplLongestSub,     // Right arg immediate value
                                                 aplTypeRes,        // Result storage type
                                                 lptkFunc);         // Ptr to function token
            goto NORMAL_EXIT;
        } // End IF

        //***************************************************************
        // From here on the right arg is a global.
        //***************************************************************

        //***************************************************************
        // If the right arg is simple, do rectangular indexing.
        // The left arg must be a vector.
        //***************************************************************
        if (IsSimple (aplTypeSub))
        {
            lpYYRes = ArrayIndexRefRhtSimpGlb_EM_YY (hGlbLft,       // Left arg global memory handle
                                                     aplTypeLft,    // Left arg storage type
                                                     aplNELMLft,    // Left arg NELM
                                                     aplRankLft,    // Left arg rank
                                                     hGlbSub,       // Right arg global memory handle
                                                     aplTypeSub,    // Right arg storage type
                                                     aplNELMSub,    // Right arg NELM
                                                     aplRankSub,    // Right arg rank
                                                     aplTypeRes,    // Result storage type
                                                     lptkFunc);     // Ptr to function token
            goto NORMAL_EXIT;
        } // End IF


        //***************************************************************
        // If the left arg is scalar, do reach indexing (special case).
        // The right arg must be a nested array of simple empty numeric
        //   or char vectors.
        //***************************************************************
        if (aplRankLft EQ 0)
        {
            lpYYRes = ArrayIndexRefLftScalar_EM_YY (hGlbLft,        // Left arg global memory handle
                                                    lpMemLft,       // Ptr to left arg global memory
                                                    aplTypeLft,     // Left arg storage type
                                                    aplLongestLft,  // Left arg immediate value
                                                    hGlbRht,        // Right arg global memory handle
                                                    lpMemSub,       // Ptr to right arg global memory
                                                    aplTypeSub,     // Right arg storage type
                                                    aplNELMSub,     // Right arg NELM
                                                    aplRankSub,     // Right arg rank
                                                    lptkFunc);      // Ptr to function token
            goto NORMAL_EXIT;
        } // End IF

        //***************************************************************
        // From here on the left arg is a global.
        //***************************************************************

        //***************************************************************
        // If the right arg is a nested array of equal length simple
        //   numeric vectors, do scatter indexing, in which case
        //   the left arg rank must be the same as the length of the
        //   right arg sub items.
        // If the right arg is a nested array of nested vectors,
        //   do reach indexing, in which case
        //   the left arg rank must be the same as the length of the
        //   first element of each of the right arg sub items.
        //***************************************************************

        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRes, aplNELMSub, aplRankSub);

        // Allocate space for the result
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
        lpHeader->NELM       = aplNELMSub;
        lpHeader->Rank       = aplRankSub;
#undef  lpHeader

        // Skip over the header to the dimensions
        lpMemRes = VarArrayBaseToDim (lpMemRes);
        lpMemSub = VarArrayBaseToDim (lpMemSub);

        // Fill in the result dimensions
        CopyMemory (lpMemRes, lpMemSub, (UINT) aplRankSub * sizeof (APLDIM));

        // Skip over the dimensions to the data
        lpMemRes = VarArrayDimToData (lpMemRes, aplRankSub);
        lpMemSub = VarArrayDimToData (lpMemSub, aplRankSub);

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

        // Validate each element of the right arg as either reach (pick)
        //   or scatter (squad) index reference

        // Get the current value of []IO
        bQuadIO = GetQuadIO ();

        // Initialize bit index when looping through Booleans
        uBitIndex = 0;

        // Loop through the elements of the right arg
        for (uSub = 0; uSub < aplNELMSub; uSub++)
        // Split cases based upon the right arg item ptr type
        switch (GetPtrTypeDir (((LPAPLNESTED) lpMemSub)[uSub]))
        {
            case PTRTYPE_STCONST:       // Immediates are NELM 1, Rank 0
                // Check for LENGTH ERROR
                if (aplRankLft NE 1)    // Check left rank vs. right item NELM
                {
                    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                               lptkFunc);
                    goto ERROR_EXIT;
                } // End IF

                // Get the value & type
                aplLongestItm = (((LPAPLHETERO) lpMemSub)[uSub])->stData.stLongest;
                immTypeItm    = (((LPAPLHETERO) lpMemSub)[uSub])->stFlags.ImmType;

                // Check for DOMAIN ERROR
                if (immTypeItm EQ IMMTYPE_CHAR)
                {
                    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                               lptkFunc);
                    goto ERROR_EXIT;
                } // End IF

                // If the immediate is a float, attempt to convert it
                if (immTypeItm EQ IMMTYPE_FLOAT)
                {
                    aplLongestItm = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestItm, &bRet);
                    if (!bRet)
                    {
                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        goto ERROR_EXIT;
                    } // End IF
                } // End IF

                // Convert to origin-0
                aplLongestItm -= bQuadIO;

                // Check for INDEX ERROR
                if (aplLongestItm >= aplNELMLft)
                {
                    ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                                               lptkFunc);
                    goto ERROR_EXIT;
                } // End IF

                // Extract the <aplLongestItm> from the (vector) left arg
                // Split cases based upon the result storage type
                switch (aplTypeRes)
                {
                    case ARRAY_BOOL:
                        uBitMask = 1 << (MASKLOG2NBIB & (UINT) aplLongestItm);

                        *((LPAPLBOOL)   lpMemRes) |= ((uBitMask & ((LPAPLBOOL) lpMemLft)[aplLongestItm >> LOG2NBIB]) ? 1 : 0) << uBitIndex;

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF

                        break;

                    case ARRAY_INT:
                        // Save the value in the result
                        *((LPAPLINT)    lpMemRes)++ = ((LPAPLINT)    lpMemLft)[aplLongestItm];

                        break;

                    case ARRAY_FLOAT:
                        // Save the value in the result
                        *((LPAPLFLOAT)  lpMemRes)++ = ((LPAPLFLOAT)  lpMemLft)[aplLongestItm];

                        break;

                    case ARRAY_CHAR:
                        // Save the value in the result
                        *((LPAPLCHAR)   lpMemRes)++ = ((LPAPLCHAR)   lpMemLft)[aplLongestItm];

                        break;

                    case ARRAY_HETERO:
                        // Save the value in the result
                        *((LPAPLHETERO) lpMemRes)++ = ((LPAPLHETERO) lpMemLft)[aplLongestItm];

                        break;

                    case ARRAY_NESTED:
                        // Save the value in the result
                        *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (((LPAPLNESTED) lpMemLft)[aplLongestItm]);

                        break;

                    defstop
                        break;
                } // End SWITCH

                break;

            case PTRTYPE_HGLOBAL:
                // Get the item global memory handle
                hGlbItm = ClrPtrTypeDirGlb (((LPAPLNESTED) lpMemSub)[uSub]);

                // Get the attributes (Type, NELM, and Rank) of the right arg item
                AttrsOfGlb (hGlbItm, &aplTypeItm, &aplNELMItm, &aplRankItm, NULL);

                // Check for RANK ERROR
                if (aplRankItm NE 1)
                {
                    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                               lptkFunc);
                    goto ERROR_EXIT;
                } // End IF

                // If it's simple numeric and the left arg rank
                //   matches the right arg subitem NELM,
                //   use Scatter Index Reference; otherwise,
                //   use Reach Index Reference
                if (IsSimple (aplTypeItm)
                 && aplRankLft EQ aplNELMItm)
                {
                    TOKEN tkRhtArg = {0};

                    // Fill in the right arg token
                    tkRhtArg.tkFlags.TknType   = TKT_VARARRAY;
////////////////////tkRhtArg.tkFlags.ImmType   = 0;     // Already zero from = {0}
////////////////////tkRhtArg.tkFlags.NoDisplay = 0;     // Already zero from = {0}
                    tkRhtArg.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbItm);
                    tkRhtArg.tkCharIndex       = lptkFunc->tkCharIndex;

                    // Note the args get switched between indexing and squad
                    lpYYItm = PrimFnDydSquadGlb_EM_YY (&tkRhtArg,           // Ptr to left arg token
                                                        hGlbLft,            // Right arg global memory handle
                                                        NULL,               // Ptr to axis token (may be NULL)
                                                        lptkFunc);          // Ptr to function token
                } else
                    // Note the args get switched between indexing and pick
                    lpYYItm = PrimFnDydRightShoeGlbGlb_EM_YY (hGlbItm,      // Left  arg global memory handle
                                                              hGlbLft,      // Right arg global memory handle
                                                              lptkFunc);    // Ptr to function token
                if (lpYYItm)
                {
                    // Split cases based upon the result storage type
                    switch (aplTypeRes)
                    {
                        case ARRAY_BOOL:
                            *((LPAPLBOOL)  lpMemRes) |= lpYYItm->tkToken.tkData.tkBoolean << uBitIndex;

                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;              // Start over
                                ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                            } // End IF

                            break;

                        case ARRAY_INT:
                            // Save the value in the result
                            *((LPAPLINT)   lpMemRes)++ = lpYYItm->tkToken.tkData.tkInteger;

                            break;

                        case ARRAY_FLOAT:
                            // Save the value in the result
                            *((LPAPLFLOAT) lpMemRes)++ = lpYYItm->tkToken.tkData.tkFloat;

                            break;

                        case ARRAY_CHAR:
                            // Save the value in the result
                            *((LPAPLCHAR)  lpMemRes)++ = lpYYItm->tkToken.tkData.tkChar;

                            break;

                        case ARRAY_HETERO:
                        case ARRAY_NESTED:
                            // Copy the squad/pick value to the result
                            if (lpYYItm->tkToken.tkFlags.TknType EQ TKT_VARIMMED)
                                *((LPAPLNESTED) lpMemRes)++ = MakeSymEntry_EM (lpYYItm->tkToken.tkFlags.ImmType,
                                                                              &lpYYItm->tkToken.tkData.tkLongest,
                                                                               lptkFunc);
                            else
                                *((LPAPLNESTED) lpMemRes)++ = lpYYItm->tkToken.tkData.tkGlbData;
                            break;

                        case ARRAY_APA:
                        defstop
                            break;
                    } // End SWITCH

                    YYFree (lpYYItm); lpYYItm = NULL;
                } else
                    goto ERROR_EXIT;
                break;

            defstop
                break;
        } // End FOR/SWITCH

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // ALready zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        // Unlock the result global memory in case TypeDemote actually demotes
        if (hGlbRes && lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // See if it fits into a lower (but not necessarily smaller) datatype
        lpYYRes->tkToken = *TypeDemote (&lpYYRes->tkToken);
    } else
    //***************************************************************
    // The right arg list has multiple elements:  do rectangular indexing.
    // The left arg must be a matching rank array.
    //***************************************************************
        lpYYRes = ArrayIndexRefRect_EM_YY (lptkRhtArg,      // Ptr to right arg token
                                           hGlbLft,         // Left arg global memory handle
                                           aplRankLft,      // Left arg rank
                                           lpMemRht,        // Ptr to right arg global memory (points to list header)
                                           aplNELMRht,      // Right arg NELM
                                           lptkFunc);       // Ptr to function token
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
    if (hGlbSub && lpMemSub)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbSub); lpMemSub = NULL;
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

    return lpYYRes;
} // End ArrayIndexRef_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexRefRhtImm_EM_YY
//
//  Array index reference for immediate right arg
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexRefRhtImm_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ArrayIndexRefRhtImm_EM_YY
    (HGLOBAL    hGlbLft,            // Left arg global memory handle
     APLSTYPE   aplTypeLft,         // Left arg storage type
     APLNELM    aplNELMLft,         // Left arg NELM
     APLRANK    aplRankLft,         // Left arg rank
     APLSTYPE   aplTypeRht,         // Right arg storage type
     APLLONGEST aplLongestRht,      // Right arg immediate value
     APLSTYPE   aplTypeRes,         // Result storage type
     LPTOKEN    lptkFunc)           // Ptr to function token

{
    APLRANK      aplRankRes = 0;    // Result rank
    HGLOBAL      hGlbSub,           // Left arg item global memory handle
                 hGlbRes;           // Result global memory handle
    LPVOID       lpMemRes;          // Ptr to result global memory
    IMM_TYPES    immTypeLft;        // Left arg immediate type
    BOOL         bRet,              // TRUE iff result is valid
                 bQuadIO;           // []IO
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    APLUINT      ByteRes;           // # bytes in the result
    APLLONGEST   aplLongestLft;     // Right arg immediate value

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Check for RANK ERROR
    if (aplRankLft NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Convert float to int if necessary
    if (aplTypeRht EQ ARRAY_FLOAT)
    {
        aplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht, &bRet);
        if (!bRet)
        {
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF
    } // End IF

    // Convet the index to origin-0
    aplLongestRht -= bQuadIO;

    // Check for within range
    if (aplLongestRht >= aplNELMLft)
    {
        ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // If the left arg is simple, the result is immediate
    if (IsSimple (aplTypeLft))
    {
        // Get the indexed value
        GetNextValueGlb (hGlbLft,           // Item global memory handle
                         aplLongestRht,     // Index into item
                         NULL,              // Ptr to result global memory handle (may be NULL)
                        &aplLongestLft,     // Ptr to result immediate value (may be NULL)
                        &immTypeLft);       // Ptr to result immediate type (may be NULL)
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = immTypeLft;
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkLongest  = aplLongestLft;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        goto NORMAL_EXIT;
    } else
    {
        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRes, 1, aplRankRes);

        // Allocate space for the result
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
        lpHeader->NELM       = 1;
        lpHeader->Rank       = aplRankRes;
#undef  lpHeader

        // No need to fill in the result's dimension as it's a scalar

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

        // Get the indexed value
        GetNextValueGlb (hGlbLft,           // Item global memory handle
                         aplLongestRht,     // Index into item
                        &hGlbSub,           // Ptr to result global memory handle (may be NULL)
                         NULL,              // Ptr to result immediate value (may be NULL)
                         NULL);             // Ptr to result immediate type (may be NULL)
        // Copy the global memory handle to the result
        *((LPAPLNESTED) lpMemRes) = CopySymGlbDirGlb (hGlbSub);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE

    goto NORMAL_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return lpYYRes;
} // End ArrayIndexRefRhtImm_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexRefRhtSimpGlb_EM_YY
//
//  Array index reference for simple global right arg
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexRefRhtSimpGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ArrayIndexRefRhtSimpGlb_EM_YY
    (HGLOBAL    hGlbLft,            // Left arg global memory handle
     APLSTYPE   aplTypeLft,         // Left arg storage type
     APLNELM    aplNELMLft,         // Left arg NELM
     APLRANK    aplRankLft,         // Left arg rank
     HGLOBAL    hGlbRht,            // Right arg global memory handle
     APLSTYPE   aplTypeRht,         // Right arg storage type
     APLNELM    aplNELMRht,         // Right arg NELM
     APLRANK    aplRankRht,         // Right arg rank
     APLSTYPE   aplTypeRes,         // Result storage type
     LPTOKEN    lptkFunc)           // Ptr to function token

{
    HGLOBAL      hGlbSub,           // Left arg item global memory handle
                 hGlbRes = NULL;    // Result global memory handle
    LPVOID       lpMemRht = NULL,   // Ptr to right arg global memory
                 lpMemRes = NULL;   // Ptr to result    ...
    BOOL         bRet,              // TRUE iff result is valid
                 bQuadIO;           // []IO
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    APLUINT      ByteRes,           // # bytes in the result
                 uRht;              // Loop counter
    UINT         uBitIndex;         // Bit index when looping through Booleans

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Check for RANK ERROR
    if (aplRankLft NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;               // Already zero from GHND
////lpHeader->SysVar     = 0;               // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);
    lpMemRht = VarArrayBaseToDim (lpMemRht);

    // Copy the dimensions to the result
    CopyMemory (lpMemRes, lpMemRht, (UINT) aplRankRht * sizeof (APLDIM));

    // Skip over the dimensions to the data
    lpMemRes = VarArrayDimToData (lpMemRes, aplRankRht);
////lpMemRht = VarArrayDimToData (lpMemRes, aplRankRht);

    // Handle prototypes
    if (aplNELMRht EQ 0
     && aplTypeRes EQ ARRAY_NESTED)
    {
        LPAPLNESTED lpMemLft;           // Ptr to left arg global memory
        HGLOBAL     hGlbProto;          // Prototype global memory handle

        // Lock the memory to get a ptr to it
        lpMemLft = MyGlobalLock (hGlbLft);

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

        // Make a prototype for the result
        hGlbProto = MakeMonPrototype_EM (ClrPtrTypeIndGlb (lpMemLft),   // Proto arg handle
                                         lptkFunc,                      // Ptr to function token
                                         MP_CHARS);                     // CHARs allowed
        // Save the value in the result
        *((LPAPLNESTED) lpMemRes) = MakeGlbTypeGlb (hGlbProto);

        // We no longer this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } else
    // Loop through the right arg elements
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        APLLONGEST aplLongestLft,           // Left arg immediate value
                   aplLongestRht;           // Right ...
        IMM_TYPES  immTypeLft;              // Left arg immediate type

        // Get the next index from the right arg
        GetNextValueGlb (hGlbRht,           // Right arg global memory handle
                         uRht,              // Index into right arg
                         NULL,              // Ptr to result global memory handle (may be NULL)
                        &aplLongestRht,     // Ptr to result immediate value (may be NULL)
                         NULL);             // Ptr to result immediate type (may be NULL)
        // Convert float to int if necessary
        if (aplTypeRht EQ ARRAY_FLOAT)
        {
            aplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht, &bRet);
            if (!bRet)
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF
        } // End IF

        // Convet the index to origin-0
        aplLongestRht -= bQuadIO;

        // Check for within range
        if (aplLongestRht >= aplNELMLft)
        {
            ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Get the indexed value from the left arg
        GetNextValueGlb (hGlbLft,           // Item global memory handle
                         aplLongestRht,     // Index into item
                        &hGlbSub,           // Ptr to result global memory handle (may be NULL)
                        &aplLongestLft,     // Ptr to result immediate value (may be NULL)
                        &immTypeLft);       // Ptr to result immediate type (may be NULL)
        // Split cases based upon the result storage type
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:
                *((LPAPLBOOL)  lpMemRes) |= aplLongestLft << uBitIndex;

                // Check for end-of-byte
                if (++uBitIndex EQ NBIB)
                {
                    uBitIndex = 0;              // Start over
                    ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                } // End IF

                break;

            case ARRAY_INT:
                // Save the value in the result
                *((LPAPLINT)   lpMemRes)++ = (APLINT) aplLongestLft;

                break;

            case ARRAY_FLOAT:
                // Save the value in the result
                *((LPAPLFLOAT) lpMemRes)++ = *(LPAPLFLOAT) &aplLongestLft;

                break;

            case ARRAY_CHAR:
                // Save the value in the result
                *((LPAPLCHAR)  lpMemRes)++ = (APLCHAR) aplLongestLft;

                break;

            case ARRAY_HETERO:
                // Save the value in the result
                *((LPAPLHETERO) lpMemRes)++ = MakeSymEntry_EM (immTypeLft,
                                                              &aplLongestLft,
                                                               lptkFunc);
                break;

            case ARRAY_NESTED:
                *((LPAPLNESTED) lpMemRes) = CopySymGlbDirGlb (hGlbSub);

                break;

            case ARRAY_APA:
            defstop
                break;
        } // End SWITCH
    } // End FOR

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

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
            MyGlobalUnlock (hGlbRes); lpMemRes= NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht= NULL;
    } // End IF

    return lpYYRes;
} // End ArrayIndexRefRhtSimpGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexRefLftScalar_EM_YY
//
//  Array index reference for scalar left arg
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexRefLftScalar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ArrayIndexRefLftScalar_EM_YY
    (
     HGLOBAL    hGlbLft,            // Left arg global memory handle
     LPVOID     lpMemLft,           // Ptr to left arg global memory (points to header)
     APLSTYPE   aplTypeLft,         // Left arg storage type
     APLLONGEST aplLongestLft,      // Left arg immediate value
     HGLOBAL    hGlbRht,            // Right arg global memory handle
     LPVOID     lpMemRht,           // Ptr to right arg global memory (points to header)
     APLSTYPE   aplTypeRht,         // Right arg storage type
     APLNELM    aplNELMRht,         // Right arg NELM
     APLRANK    aplRankRht,         // Right arg rank
     LPTOKEN    lptkFunc)           // Ptr to function token

{
    APLUINT      uRht;              // Loop counter
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    APLSTYPE     aplTypeSub;        // Right arg item storage type
    APLNELM      aplNELMSub,        // Right arg item NELM
                 aplNELMRes;        // Result         ...
    LPAPLDIM     lpMemDimRht;       // Ptr to right arg dimensions
    APLRANK      aplRankSub;        // Right arg rank
    APLUINT      ByteRes;           // # bytes in the result
    HGLOBAL      hGlbRes;           // Result global memory handle
    LPVOID       lpMemRes;          // Ptr to result global memory
    APLUINT      uRes;              // Loop counter

    // Check for RANK ERROR
    if (IsSimple (aplTypeRht))
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Copy as result NELM
    aplNELMRes = aplNELMRht;

    // Handle prototypes
    aplNELMRht = max (aplNELMRht, 1);

    // Skip over the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Loop through the right arg looking for simple empty vectors
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    // Split cases based upon the right arg item ptr type
    switch (GetPtrTypeDir (((LPAPLNESTED) lpMemRht)[uRht]))
    {
        case PTRTYPE_STCONST:       // Immediates are NELM 1, Rank 0
            // It's not a vector, so signal a RANK ERROR
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;

        case PTRTYPE_HGLOBAL:
            // Get the attributes (Type, NELM, and Rank) of the right arg item
            AttrsOfGlb (ClrPtrTypeDirGlb (((LPAPLNESTED) lpMemRht)[uRht]), &aplTypeSub, &aplNELMSub, &aplRankSub, NULL);

            // Check for RANK ERROR
            if (aplRankSub NE 1)
            {
                ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Check for LENGTH ERROR
            if (aplNELMSub NE 0)
            {
                ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Check for DOMAIN ERROR
            if (!IsSimple (aplTypeSub))
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            break;

        defstop
            break;
    } // End FOR

    // The right arg has been validated, return ({rho}R){rho}L

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeLft, aplNELMRes, aplRankRht);

    // Allocate space for the result
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
    lpHeader->ArrType    = aplTypeLft;
////lpHeader->Perm       = 0;               // Already zero from GHND
////lpHeader->SysVar     = 0;               // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Copy the right arg dimensions to the result
    CopyMemory (lpMemRes, lpMemDimRht, (UINT) aplRankRht * sizeof (APLDIM));

    // Skip over the dimensions to the data
    lpMemRes = VarArrayDimToData (lpMemRes, aplRankRht);

    // Repeat the (scalar) left arg in the result

    // Split cases based upon the left arg storage type
    switch (aplTypeLft)
    {
        case ARRAY_BOOL:
            if ((APLBOOL) aplLongestLft)
                FillMemory (lpMemRes, (UINT) RoundUpBits8 (aplNELMRht), 0xFF);
            break;

        case ARRAY_INT:
            for (uRes = 0; uRes < aplNELMRht; uRes++)
                *((LPAPLINT) lpMemRes)++ = (APLINT) aplLongestLft;
            break;

        case ARRAY_FLOAT:
            for (uRes = 0; uRes < aplNELMRht; uRes++)
                *((LPAPLFLOAT) lpMemRes)++ = *(LPAPLFLOAT) &aplLongestLft;
            break;

        case ARRAY_CHAR:
            for (uRes = 0; uRes < aplNELMRht; uRes++)
                *((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestLft;
            break;

        case ARRAY_NESTED:
            // Skip over the header and dimensions to the data
            lpMemLft = VarArrayBaseToData (lpMemLft, 0);

            // If the right arg is empty, copy the left arg prototype to the result
            if (aplNELMRht EQ 0)
                *((LPAPLNESTED) lpMemRes) =
                  MakeMonPrototype_EM (ClrPtrTypeIndGlb (lpMemLft), // Proto arg handle
                                       lptkFunc,                    // Ptr to function token
                                       MP_CHARS);                   // CHARs allowed
            else
            for (uRes = 0; uRes < aplNELMRht; uRes++)
                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbInd (lpMemLft);
            break;

        case ARRAY_HETERO:
        case ARRAY_APA:
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
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    lpYYRes->tkToken = *TypeDemote (&lpYYRes->tkToken);
ERROR_EXIT:
    return lpYYRes;
} // End ArrayIndexRefLftScalar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexRefRect_EM_YY
//
//  Array index reference for multiple list elements (rectangular indexing)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexRefRect_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ArrayIndexRefRect_EM_YY
    (LPTOKEN     lptkRhtArg,        // Ptr to right arg token
     HGLOBAL     hGlbLft,           // Left arg gobal memory handle
     APLRANK     aplRankLft,        // Left arg rank
     LPAPLNESTED lpMemRht,          // Ptr to right arg global memory (points to list header)
     APLNELM     aplNELMRht,        // Right arg NELM
     LPTOKEN     lptkFunc)          // Ptr to function token

{
    APLUINT      uRht,              // Loop counter
                 uCount,            // Count of non-elided indices
                 ByteRes;           // # bytes in the result
    HGLOBAL      hGlbRhtNew = NULL, // New right arg global memory handle
                 hGlbRhtOld = NULL, // Old ...
                 hGlbAxis = NULL;   // Axis operator global memory handle
    LPAPLNESTED  lpMemRhtNew = NULL;  // Ptr to new right arg global memory
    LPAPLUINT    lpMemAxis = NULL;  // Ptr to axis operator global memory
    BOOL         bQuadIO;           // []IO
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    TOKEN        tkRhtArg = {0},    // New right arg token
                 tkAxis = {0};      // Axis token

    // Skip over the header and dimension
    lpMemRht = VarArrayBaseToData (lpMemRht, 1);

    // Loop through the elements of the right arg
    //   counting the # non-placeholders (non-elided) indices
    for (uCount = uRht = 0; uRht < aplNELMRht; uRht++)
        uCount += !PtrReusedDir (lpMemRht[uRht]);

    // Check for RANK ERROR
    if (aplRankLft NE aplNELMRht
     && aplNELMRht NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // If there are no elided indices, use squad without axis operator
    if (uCount EQ aplNELMRht)
        // Note the args get switched between indexing and squad
        lpYYRes = PrimFnDydSquadGlb_EM_YY (lptkRhtArg,     // Ptr to left arg token
                                           hGlbLft,        // Right arg global memory handle
                                           NULL,           // Ptr to axis token (may be NULL)
                                           lptkFunc);      // Ptr to function token
    else
    // There are elided indices, so we must create a new
    //   right arg (w/o the elided indices) and a new
    //   axis operator value
    {
        // Calculate bytes needed for the new right arg
        ByteRes = CalcArraySize (ARRAY_NESTED, uCount, 1);

        // Allocate space for the new right arg
        hGlbRhtNew = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (!hGlbRhtNew)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemRhtNew = MyGlobalLock (hGlbRhtNew);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRhtNew)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_NESTED;
////////lpHeader->Perm       = 0;               // Already zero from GHND
////////lpHeader->SysVar     = 0;               // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = uCount;
        lpHeader->Rank       = 1;
#undef  lpHeader

        // Fill in the dimension
        *VarArrayBaseToDim (lpMemRhtNew) = uCount;

        // Skip over the header and dimension to the data
        lpMemRhtNew = VarArrayBaseToData (lpMemRhtNew, 1);

        // Handle prototypes
        if (uCount EQ 0)
            // Use {zilde} as the prototype
            *lpMemRhtNew = MakeGlbTypeGlb (hGlbZilde);
        else
        // Copy the non-elided indices to the new right arg
        for (uRht = 0; uRht < aplNELMRht; uRht++)
        if (!PtrReusedDir (lpMemRht[uRht]))
            *lpMemRhtNew++ = CopySymGlbDir (lpMemRht[uRht]);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRhtNew); lpMemRhtNew = NULL;

        // Fill in the new right arg token
        tkRhtArg.tkFlags.TknType   = TKT_VARARRAY;
////////tkRhtArg.tkFlags.ImmType   = 0;     // Already zero from = {0}
////////tkRhtArg.tkFlags.NoDisplay = 0;     // Already zero from = {0}
        tkRhtArg.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRhtNew);
        tkRhtArg.tkCharIndex       = lptkFunc->tkCharIndex;

        // Calculate space needed for the axis operator
        ByteRes = CalcArraySize (ARRAY_INT, uCount, 1);

        // Allocate space for the axis operator
        hGlbAxis = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (!hGlbAxis)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemAxis = MyGlobalLock (hGlbAxis);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemAxis)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_INT;
////////lpHeader->Perm       = 0;               // Already zero from GHND
////////lpHeader->SysVar     = 0;               // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = uCount;
        lpHeader->Rank       = 1;
#undef  lpHeader

        // Fill in the dimension
        *VarArrayBaseToDim (lpMemAxis) = uCount;

        // Skip over the header and dimension to the data
        lpMemAxis = VarArrayBaseToData (lpMemAxis, 1);

        // Get the current value of []IO
        bQuadIO = GetQuadIO ();

        // Save the value of the non-elided indices to the axis operator
        for (uCount = uRht = 0; uRht < aplNELMRht; uRht++, uCount++)
        if (!PtrReusedDir (lpMemRht[uRht]))
            *lpMemAxis++ = bQuadIO + uCount;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbAxis); lpMemAxis = NULL;

        // Fill in the axis token
        tkAxis.tkFlags.TknType   = TKT_VARARRAY;
////////tkAxis.tkFlags.ImmType   = 0;     // Already zero from = {0}
////////tkAxis.tkFlags.NoDisplay = 0;     // Already zero from = {0}
        tkAxis.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbAxis);
        tkAxis.tkCharIndex       = lptkFunc->tkCharIndex;

        // Note the args get switched between indexing and squad
        lpYYRes = PrimFnDydSquadGlb_EM_YY (&tkRhtArg,   // Ptr to left arg token
                                            hGlbLft,    // Right arg global memory handle
                                           &tkAxis,     // Ptr to axis token (may be NULL)
                                            lptkFunc);  // Ptr to function token
    } // End IF/ELSE
ERROR_EXIT:
    if (hGlbAxis)
    {
        if (lpMemAxis)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbAxis); lpMemAxis = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbAxis); hGlbAxis = NULL;
    } // End IF

    if (hGlbRhtNew)
    {
        if (lpMemRhtNew)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRhtNew); lpMemRhtNew = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbRhtNew); hGlbRhtNew = NULL;
    } // End IF

    CheckMemStat ();

    return lpYYRes;
} // End ArrayIndexRefRect_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ListIndexRef_EM_YY
//
//  Index reference of a list, e.g., 1.  A[L]        [M]
//                               or  2.  A[L]        [M1;...;Mn]
//                               or  3.  A[L1;...;Ln][M]
//                               or  4.  A[L1;...;Ln][M1;...;Mn]
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ListIndexRef_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ListIndexRef_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    APLSTYPE     aplTypeLft;        // Left arg storage type
    APLNELM      aplNELMLft;        // Left arg NELM
    APLRANK      aplRankLft;        // Left arg rank
    HGLOBAL      hGlbLft = NULL;    // Left arg global memory handle
    LPVOID       lpMemLft = NULL;   // Ptr to left arg global memory
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
#define lptkFunc    lptkRhtArg

    // Get the attributes (Type, NELM, and Rank) of the left arg
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);

    // Get left arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);

    //***************************************************************
    // If the left arg list is empty or a singleton (cases 1 & 2), ...
    //***************************************************************
    if (aplNELMLft <= 1)
    {
        TOKEN tkLftArg = {0};       // Left arg token

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, 1);

        // Split cases based upon the left arg item ptr type
        switch (GetPtrTypeInd (lpMemLft))
        {
            case PTRTYPE_STCONST:
                // Fill in the new left arg token
                tkLftArg.tkFlags.TknType   = TKT_VARIMMED;
                tkLftArg.tkFlags.ImmType   = (*(LPAPLHETERO) lpMemLft)->stFlags.ImmType;
////////////////tkLftArg.tkFlags.NoDisplay = 0;     // Already zero from = {0}
                tkLftArg.tkData.tkLongest  = (*(LPAPLHETERO) lpMemLft)->stData.stLongest;
                tkLftArg.tkCharIndex       = lptkFunc->tkCharIndex;

                break;

            case PTRTYPE_HGLOBAL:
                // Fill in the new left arg token
                tkLftArg.tkFlags.TknType   = TKT_VARARRAY;
////////////////tkLftArg.tkFlags.ImmType   = 0;     // Already zero from = {0}
////////////////tkLftArg.tkFlags.NoDisplay = 0;     // Already zero from = {0}
                tkLftArg.tkData.tkGlbData  = *(LPAPLNESTED) lpMemLft;
                tkLftArg.tkCharIndex       = lptkFunc->tkCharIndex;

                break;

            defstop
                break;
        } // End SWITCH

        // Calculate the resulting index
        lpYYRes = ArrayIndexRef_EM_YY (&tkLftArg,       // Ptr to left arg token
                                        lptkRhtArg);    // Ptr to right arg token
        // If it succeeded, ...
        if (lpYYRes)
        {
            HGLOBAL hGlbRes;
            LPVOID  lpMemRes;
            APLUINT ByteRes;

            // Calculate space needed for the result
            ByteRes = CalcArraySize (ARRAY_LIST, 1, 1);

            // Allocate space for the result
            hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
            if (!hGlbRes)
            {
                ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                           lptkFunc);
            } else
            {
                // Lock the memory to get a ptr to it
                lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
                // Fill in the header
                lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                lpHeader->ArrType    = ARRAY_LIST;
////////////////lpHeader->Perm       = 0;               // Already zero from GHND
////////////////lpHeader->SysVar     = 0;               // Already zero from GHND
                lpHeader->RefCnt     = 1;
                lpHeader->NELM       = 1;
                lpHeader->Rank       = 1;
#undef  lpHeader
                // Fill in the dimension
                *VarArrayBaseToDim (lpMemRes) = 1;

                // Skip over the header and dimension to the data
                lpMemRes = VarArrayBaseToData (lpMemRes, 1);

                // If it's immediate, ...
                if (lpYYRes->tkToken.tkFlags.TknType EQ TKT_VARIMMED)
                    // Fill in the data
                    *((LPAPLNESTED) lpMemRes) = MakeSymEntry_EM (lpYYRes->tkToken.tkFlags.ImmType,
                                                                &lpYYRes->tkToken.tkData.tkLongest,
                                                                 lptkFunc);
                else
                    // Fill in the data
                    *((LPAPLNESTED) lpMemRes) = lpYYRes->tkToken.tkData.tkGlbData;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbRes); lpMemRes = NULL;  // ***DEBUG***

                YYFree (lpYYRes); lpYYRes = NULL;

                // Allocate a new YYRes
                lpYYRes = YYAlloc ();

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_LISTBR;
////////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
                lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
            } // End IF/ELSE
        } // End IF
    } else
    {
        PrimFnNonceError_EM (lptkFunc);

        DbgBrk ();      // ***FINISHME*** -- A[L1;...;Ln][M] or A[L1;...;Ln][M1;...;Mn]











    } // End IF/ELSE

    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    return lpYYRes;
#undef  lptkFunc
} // End ListIndexRef_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexSet_EM
//
//  Index assignment of an array, e.g., A[L]{is}R
//                                  or  A[L1;...;Ln]{is}R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexSet_EM"
#else
#define APPEND_NAME
#endif

BOOL ArrayIndexSet_EM
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkLstArg,            // Ptr to list arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    PrimFnNonceError_EM (lptkNamArg);

    return FALSE;

    DbgBrk ();          // ***FINISHME*** -- A[L]{is}R












    return FALSE;
} // End ArrayIndexSet_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexFcnSet_EM
//
//  Modified index assignment of an array, e.g., A[L]fcn{is}R
//                                           or  A[L1;...;Ln]fcn{is}R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexFcnSet_EM"
#else
#define APPEND_NAME
#endif

BOOL ArrayIndexFcnSet_EM
    (LPTOKEN      lptkNamArg,           // Ptr to name arg token
     LPTOKEN      lptkLstArg,           // Ptr to list arg token
     LPPL_YYSTYPE lpYYFcnStr,           // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    LPPL_YYSTYPE lpYYRes1,              // Ptr to temporary resuilt
                 lpYYRes2;              // ...
    BOOL         bRet;                  // TRUE iff result is valid

    // Compute A[L]
    lpYYRes1 = ArrayIndexRef_EM_YY (lptkNamArg,         // Ptr to left arg token
                                   lptkLstArg);         // Ptr to right arg token
    if (!lpYYRes1)
        return FALSE;
    // Compute A[L] fcn R
    lpYYRes2 = ExecFuncStr_EM_YY (&lpYYRes1->tkToken,   // Ptr to left arg token (may be NULL if monadic)
                                   lpYYFcnStr,          // Ptr to function strand
                                   lptkRhtArg,          // Ptr to right arg token
                                   NULL);               // Ptr to axis token (may be NULL)
    FreeResult (&lpYYRes1->tkToken); YYFree (lpYYRes1); lpYYRes1 = NULL;
    if (!lpYYRes2)
        return FALSE;

    // Assign the resulting values into A[L]
    bRet = ArrayIndexSet_EM (lptkNamArg,
                             lptkLstArg,
                            &lpYYRes2->tkToken);
    FreeResult (&lpYYRes2->tkToken); YYFree (lpYYRes2); lpYYRes2 = NULL;

    return bRet;
} // End ArrayIndexFcnSet_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_index.c
//***************************************************************************
