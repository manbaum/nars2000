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
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkLstArg)            // Ptr to list arg token

{
    APLSTYPE     aplTypeNam,        // Name arg storage type
                 aplTypeRes,        // Result   ...
                 aplTypeSub;        // List arg item ...
    APLNELM      aplNELMNam,        // Name arg NELM
                 aplNELMLst,        // List ...
                 aplNELMSub;        // List arg item ...
    APLRANK      aplRankNam,        // Name arg rank
                 aplRankSub;        // List arg item ...
    HGLOBAL      hGlbNam = NULL,    // Name arg global memory handle
                 hGlbLst = NULL,    // List ...
                 hGlbSub = NULL,    // List arg item ...
                 hGlbRes = NULL,    // Result         ...
                 hGlbItm;           // List subitem  ...
    LPVOID       lpMemNam = NULL,   // Ptr to name arg global memory
                 lpMemLst = NULL,   // Ptr to list ...
                 lpMemSub = NULL,   // Ptr to list item ...
                 lpMemRes = NULL;   // Ptr to result     ...
    APLUINT      ByteRes,           // # bytes in the result
                 uSub;              // Loop counter
    BOOL         bRet = TRUE,       // TRUE iff result is valid
                 bQuadIO;           // []IO
    LPPL_YYSTYPE lpYYRes = NULL,    // Ptr to the result
                 lpYYItm;           // Ptr to temporary result
    LPTOKEN      lptkFunc = lptkLstArg; // Ptr to function token
    APLLONGEST   aplLongestNam,     // Name arg immediate value
                 aplLongestSub,     // List arg item ...
                 aplLongestItm;     // List arg subitem ...
    IMM_TYPES    immTypeItm;        // List arg subitem immediate type
    UINT         uBitMask,          // Bit mask for when looping through Booleans
                 uBitIndex;         // Bit index ...

    // Get the attributes (Type, NELM, and Rank) of the name & list args
    AttrsOfToken (lptkNamArg, &aplTypeNam, &aplNELMNam, &aplRankNam, NULL);
    AttrsOfToken (lptkLstArg, NULL,        &aplNELMLst, NULL,        NULL);

    // Map APA name arg to INT result
    if (aplTypeNam EQ ARRAY_APA)
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeNam;

    // Get name and list arg's global ptrs
    aplLongestNam = GetGlbPtrs_LOCK (lptkNamArg, &hGlbNam, &lpMemNam);
                    GetGlbPtrs_LOCK (lptkLstArg, &hGlbLst, &lpMemLst);

    // The list arg is always of rank 1

    //***************************************************************
    // If the list arg is empty, ...
    //***************************************************************
    if (aplNELMLst EQ 0)
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // If the name arg is immediate, ...
        if (hGlbNam EQ NULL)
        {
            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType   = TranslateArrayTypeToImmType (aplTypeNam);
////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkLongest  = aplLongestNam;
            lpYYRes->tkToken.tkCharIndex       = lptkLstArg->tkCharIndex;
        } else
        {
            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDirGlb (hGlbNam);
            lpYYRes->tkToken.tkCharIndex       = lptkLstArg->tkCharIndex;
        } // End IF/ELSE
    } else
    //***************************************************************
    // If the list arg is a singleton, ...
    //***************************************************************
    if (aplNELMLst EQ 1)
    {
        APLSTYPE aplTypeItm;            // List arg subitem storage type
        APLNELM  aplNELMItm;            // List arg subitem NELM
        APLRANK  aplRankItm;            // List arg subitem rank

        // Skip over the header and dimensions to the data
        lpMemLst = VarArrayBaseToData (lpMemLst, 1);

        // Split cases based upon the list arg item ptr type
        switch (GetPtrTypeInd (lpMemLst))
        {
            case PTRTYPE_STCONST:
                // Set the vars for an STE
                hGlbSub = NULL;
                aplTypeSub = TranslateImmTypeToArrayType ((*(LPAPLHETERO) lpMemLst)->stFlags.ImmType);
                aplNELMSub = 1;
                aplRankSub = 0;
                aplLongestSub = (*(LPAPLHETERO) lpMemLst)->stData.stLongest;

                break;

            case PTRTYPE_HGLOBAL:
                // Set the vars for an HGLOBAL
                hGlbSub = ClrPtrTypeIndGlb (lpMemLst);
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
        // If the list arg item is immediate, do rectangular indexing.
        // The name arg must be a vector.
        //***************************************************************
        if (hGlbSub EQ NULL)
        {
            lpYYRes = ArrayIndexRefLstImm_EM_YY (hGlbNam,           // Name arg global memory handle
                                                 aplTypeNam,        // Name arg storage type
                                                 aplNELMNam,        // Name arg NELM
                                                 aplRankNam,        // Name arg rank
                                                 aplTypeSub,        // List arg storage type
                                                 aplLongestSub,     // List arg immediate value
                                                 aplTypeRes,        // Result storage type
                                                 lptkFunc);         // Ptr to function token
            goto NORMAL_EXIT;
        } // End IF

        //***************************************************************
        // From here on the list arg is a global.
        //***************************************************************

        //***************************************************************
        // If the list arg item is simple, do rectangular indexing.
        // The name arg must be a vector.
        //***************************************************************
        if (IsSimple (aplTypeSub))
        {
            lpYYRes = ArrayIndexRefLstSimpGlb_EM_YY (hGlbNam,       // Name arg global memory handle
                                                     aplTypeNam,    // Name arg storage type
                                                     aplNELMNam,    // Name arg NELM
                                                     aplRankNam,    // Name arg rank
                                                     hGlbSub,       // List arg global memory handle
                                                     aplTypeSub,    // List arg storage type
                                                     aplNELMSub,    // List arg NELM
                                                     aplRankSub,    // List arg rank
                                                     aplTypeRes,    // Result storage type
                                                     lptkFunc);     // Ptr to function token
            goto NORMAL_EXIT;
        } // End IF

        //***************************************************************
        // If the name arg is scalar, do reach indexing (special case).
        // The list arg item must be a nested array of simple empty numeric
        //   or char vectors.
        //***************************************************************
        if (aplRankNam EQ 0)
        {
            lpYYRes = ArrayIndexRefNamScalar_EM_YY (hGlbNam,        // Name arg global memory handle
                                                    lpMemNam,       // Ptr to name arg global memory
                                                    aplTypeNam,     // Name arg storage type
                                                    aplLongestNam,  // Name arg immediate value
                                                    hGlbLst,        // List arg global memory handle
                                                    lpMemSub,       // Ptr to list arg item global memory
                                                    aplTypeSub,     // List arg item storage type
                                                    aplNELMSub,     // List arg item NELM
                                                    aplRankSub,     // List arg item rank
                                                    lptkFunc);      // Ptr to function token
            goto NORMAL_EXIT;
        } // End IF

        //***************************************************************
        // From here on the name arg is a global.
        //***************************************************************

        //***************************************************************
        // If the list arg item is a nested array of equal length simple
        //   numeric vectors, do scatter indexing, in which case
        //   the name arg rank must be the same as the length of the
        //   list arg item sub items.
        // If the list item arg is a nested array of nested vectors,
        //   do reach indexing, in which case
        //   the name arg rank must be the same as the length of the
        //   first element of each of the list arg sub items.
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
        lpMemNam = VarArrayBaseToData (lpMemNam, aplRankNam);

        // Validate each element of the list arg items as either reach (pick)
        //   or scatter (squad) index reference

        // Get the current value of []IO
        bQuadIO = GetQuadIO ();

        // Initialize bit index when looping through Booleans
        uBitIndex = 0;

        // Loop through the elements of the list arg
        for (uSub = 0; uSub < aplNELMSub; uSub++)
        // Split cases based upon the list arg item ptr type
        switch (GetPtrTypeDir (((LPAPLNESTED) lpMemSub)[uSub]))
        {
            case PTRTYPE_STCONST:       // Immediates are NELM 1, Rank 0
                // Check for LENGTH ERROR
                if (aplRankNam NE 1)    // Check Name rank vs. list item NELM
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
                if (aplLongestItm >= aplNELMNam)
                {
                    ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                                               lptkFunc);
                    goto ERROR_EXIT;
                } // End IF

                // Extract the <aplLongestItm> from the (vector) name arg
                // Split cases based upon the result storage type
                switch (aplTypeRes)
                {
                    case ARRAY_BOOL:
                        uBitMask = 1 << (MASKLOG2NBIB & (UINT) aplLongestItm);

                        *((LPAPLBOOL)   lpMemRes) |= ((uBitMask & ((LPAPLBOOL) lpMemNam)[aplLongestItm >> LOG2NBIB]) ? 1 : 0) << uBitIndex;

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF

                        break;

                    case ARRAY_INT:
                        // Save the value in the result
                        *((LPAPLINT)    lpMemRes)++ = ((LPAPLINT)    lpMemNam)[aplLongestItm];

                        break;

                    case ARRAY_FLOAT:
                        // Save the value in the result
                        *((LPAPLFLOAT)  lpMemRes)++ = ((LPAPLFLOAT)  lpMemNam)[aplLongestItm];

                        break;

                    case ARRAY_CHAR:
                        // Save the value in the result
                        *((LPAPLCHAR)   lpMemRes)++ = ((LPAPLCHAR)   lpMemNam)[aplLongestItm];

                        break;

                    case ARRAY_HETERO:
                        // Save the value in the result
                        *((LPAPLHETERO) lpMemRes)++ = ((LPAPLHETERO) lpMemNam)[aplLongestItm];

                        break;

                    case ARRAY_NESTED:
                        // Save the value in the result
                        *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (((LPAPLNESTED) lpMemNam)[aplLongestItm]);

                        break;

                    defstop
                        break;
                } // End SWITCH

                break;

            case PTRTYPE_HGLOBAL:
                // Get the item global memory handle
                hGlbItm = ClrPtrTypeDirGlb (((LPAPLNESTED) lpMemSub)[uSub]);

                // Get the attributes (Type, NELM, and Rank) of the list arg item
                AttrsOfGlb (hGlbItm, &aplTypeItm, &aplNELMItm, &aplRankItm, NULL);

                // Check for RANK ERROR
                if (aplRankItm NE 1)
                {
                    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                               lptkFunc);
                    goto ERROR_EXIT;
                } // End IF

                // If it's simple numeric and the name arg rank
                //   matches the list arg subitem NELM,
                //   use Scatter Index Reference; otherwise,
                //   use Reach Index Reference
                if (IsSimple (aplTypeItm)
                 && aplRankNam EQ aplNELMItm)
                {
                    TOKEN tkLstArg = {0};

                    // Fill in the list arg token
                    tkLstArg.tkFlags.TknType   = TKT_VARARRAY;
////////////////////tkLstArg.tkFlags.ImmType   = 0;     // Already zero from = {0}
////////////////////tkLstArg.tkFlags.NoDisplay = 0;     // Already zero from = {0}
                    tkLstArg.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbItm);
                    tkLstArg.tkCharIndex       = lptkFunc->tkCharIndex;

                    // Note the args get switched between indexing and squad
                    lpYYItm = PrimFnDydSquadGlb_EM_YY (&tkLstArg,           // Ptr to name arg token
                                                        hGlbNam,            // List arg global memory handle
                                                        NULL,               // Ptr to axis token (may be NULL)
                                                        lptkFunc);          // Ptr to function token
                } else
                    // Note the args get switched between indexing and pick
                    lpYYItm = PrimFnDydRightShoeGlbGlb_EM_YY (hGlbItm,      // Name arg global memory handle
                                                              hGlbNam,      // Right arg global memory handle
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
    // The list arg list has multiple elements:  do rectangular indexing.
    // The name arg must be a matching rank array.
    //***************************************************************
        lpYYRes = ArrayIndexRefRect_EM_YY (lptkLstArg,      // Ptr to list arg token
                                           hGlbNam,         // Name arg global memory handle
                                           aplRankNam,      // Name arg rank
                                           lpMemLst,        // Ptr to list arg global memory (points to list header)
                                           aplNELMLst,      // List arg NELM
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

    if (hGlbLst && lpMemLst)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLst); lpMemLst = NULL;
    } // End IF

    if (hGlbNam && lpMemNam)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbNam); lpMemNam = NULL;
    } // End IF

    return lpYYRes;
} // End ArrayIndexRef_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexRefLstImm_EM_YY
//
//  Array index reference for immediate list arg
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexRefLstImm_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ArrayIndexRefLstImm_EM_YY
    (HGLOBAL    hGlbNam,            // Name arg global memory handle
     APLSTYPE   aplTypeNam,         // Name arg storage type
     APLNELM    aplNELMNam,         // Name arg NELM
     APLRANK    aplRankNam,         // Name arg rank
     APLSTYPE   aplTypeLst,         // List arg storage type
     APLLONGEST aplLongestLst,      // List arg immediate value
     APLSTYPE   aplTypeRes,         // Result storage type
     LPTOKEN    lptkFunc)           // Ptr to function token

{
    APLRANK      aplRankRes = 0;    // Result rank
    HGLOBAL      hGlbSub,           // Name arg item global memory handle
                 hGlbRes;           // Result global memory handle
    LPVOID       lpMemRes;          // Ptr to result global memory
    IMM_TYPES    immTypeNam;        // Name arg immediate type
    BOOL         bRet,              // TRUE iff result is valid
                 bQuadIO;           // []IO
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    APLUINT      ByteRes;           // # bytes in the result
    APLLONGEST   aplLongestNam;     // Name arg immediate value

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Check for RANK ERROR
    if (aplRankNam NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Convert float to int if necessary
    if (aplTypeLst EQ ARRAY_FLOAT)
    {
        aplLongestLst = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestLst, &bRet);
        if (!bRet)
        {
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF
    } // End IF

    // Convet the index to origin-0
    aplLongestLst -= bQuadIO;

    // Check for within range
    if (aplLongestLst >= aplNELMNam)
    {
        ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // If the name arg is simple, the result is immediate
    if (IsSimple (aplTypeNam))
    {
        // Get the indexed value
        GetNextValueGlb (hGlbNam,           // Item global memory handle
                         aplLongestLst,     // Index into item
                         NULL,              // Ptr to result global memory handle (may be NULL)
                        &aplLongestNam,     // Ptr to result immediate value (may be NULL)
                        &immTypeNam);       // Ptr to result immediate type (may be NULL)
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = immTypeNam;
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkLongest  = aplLongestNam;
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
        GetNextValueGlb (hGlbNam,           // Item global memory handle
                         aplLongestLst,     // Index into item
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
} // End ArrayIndexRefLstImm_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexRefLstSimpGlb_EM_YY
//
//  Array index reference for simple global list arg
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexRefLstSimpGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ArrayIndexRefLstSimpGlb_EM_YY
    (HGLOBAL    hGlbNam,            // Name arg global memory handle
     APLSTYPE   aplTypeNam,         // Name arg storage type
     APLNELM    aplNELMNam,         // Name arg NELM
     APLRANK    aplRankNam,         // Name arg rank
     HGLOBAL    hGlbLst,            // List arg global memory handle
     APLSTYPE   aplTypeLst,         // List arg storage type
     APLNELM    aplNELMLst,         // List arg NELM
     APLRANK    aplRankLst,         // List arg rank
     APLSTYPE   aplTypeRes,         // Result storage type
     LPTOKEN    lptkFunc)           // Ptr to function token

{
    HGLOBAL      hGlbSub,           // Name arg item global memory handle
                 hGlbRes = NULL;    // Result global memory handle
    LPVOID       lpMemLst = NULL,   // Ptr to list arg global memory
                 lpMemRes = NULL;   // Ptr to result    ...
    BOOL         bRet,              // TRUE iff result is valid
                 bQuadIO;           // []IO
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    APLUINT      ByteRes,           // # bytes in the result
                 uLst;              // Loop counter
    UINT         uBitIndex;         // Bit index when looping through Booleans

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Check for RANK ERROR
    if (aplRankNam NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMLst, aplRankLst);

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
    lpMemLst = MyGlobalLock (hGlbLst);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;               // Already zero from GHND
////lpHeader->SysVar     = 0;               // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMLst;
    lpHeader->Rank       = aplRankLst;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);
    lpMemLst = VarArrayBaseToDim (lpMemLst);

    // Copy the dimensions to the result
    CopyMemory (lpMemRes, lpMemLst, (UINT) aplRankLst * sizeof (APLDIM));

    // Skip over the dimensions to the data
    lpMemRes = VarArrayDimToData (lpMemRes, aplRankLst);
////lpMemLst = VarArrayDimToData (lpMemRes, aplRankLst);

    // Handle prototypes
    if (aplNELMLst EQ 0
     && aplTypeRes EQ ARRAY_NESTED)
    {
        LPAPLNESTED lpMemNam;           // Ptr to name arg global memory
        HGLOBAL     hGlbProto;          // Prototype global memory handle

        // Lock the memory to get a ptr to it
        lpMemNam = MyGlobalLock (hGlbNam);

        // Skip over the header and dimensions to the data
        lpMemNam = VarArrayBaseToData (lpMemNam, aplRankNam);

        // Make a prototype for the result
        hGlbProto = MakeMonPrototype_EM (ClrPtrTypeIndGlb (lpMemNam),   // Proto arg handle
                                         lptkFunc,                      // Ptr to function token
                                         MP_CHARS);                     // CHARs allowed
        // Save the value in the result
        *((LPAPLNESTED) lpMemRes) = MakeGlbTypeGlb (hGlbProto);

        // We no longer this ptr
        MyGlobalUnlock (hGlbNam); lpMemNam = NULL;
    } else
    // Loop through the list arg elements
    for (uLst = 0; uLst < aplNELMLst; uLst++)
    {
        APLLONGEST aplLongestNam,           // Name arg immediate value
                   aplLongestLst;           // List ...
        IMM_TYPES  immTypeNam;              // Name arg immediate type

        // Get the next index from the list arg
        GetNextValueGlb (hGlbLst,           // List arg global memory handle
                         uLst,              // Index into list arg
                         NULL,              // Ptr to list global memory handle (may be NULL)
                        &aplLongestLst,     // Ptr to list immediate value (may be NULL)
                         NULL);             // Ptr to list immediate type (may be NULL)
        // Convert float to int if necessary
        if (aplTypeLst EQ ARRAY_FLOAT)
        {
            aplLongestLst = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestLst, &bRet);
            if (!bRet)
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF
        } // End IF

        // Convet the index to origin-0
        aplLongestLst -= bQuadIO;

        // Check for within range
        if (aplLongestLst >= aplNELMNam)
        {
            ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Get the indexed value from the name arg
        GetNextValueGlb (hGlbNam,           // Item global memory handle
                         aplLongestLst,     // Index into item
                        &hGlbSub,           // Ptr to result global memory handle (may be NULL)
                        &aplLongestNam,     // Ptr to result immediate value (may be NULL)
                        &immTypeNam);       // Ptr to result immediate type (may be NULL)
        // Split cases based upon the result storage type
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:
                *((LPAPLBOOL)  lpMemRes) |= aplLongestNam << uBitIndex;

                // Check for end-of-byte
                if (++uBitIndex EQ NBIB)
                {
                    uBitIndex = 0;              // Start over
                    ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                } // End IF

                break;

            case ARRAY_INT:
                // Save the value in the result
                *((LPAPLINT)   lpMemRes)++ = (APLINT) aplLongestNam;

                break;

            case ARRAY_FLOAT:
                // Save the value in the result
                *((LPAPLFLOAT) lpMemRes)++ = *(LPAPLFLOAT) &aplLongestNam;

                break;

            case ARRAY_CHAR:
                // Save the value in the result
                *((LPAPLCHAR)  lpMemRes)++ = (APLCHAR) aplLongestNam;

                break;

            case ARRAY_HETERO:
                // Save the value in the result
                *((LPAPLHETERO) lpMemRes)++ = MakeSymEntry_EM (immTypeNam,
                                                              &aplLongestNam,
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
    if (hGlbLst && lpMemLst)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLst); lpMemLst= NULL;
    } // End IF

    return lpYYRes;
} // End ArrayIndexRefLstSimpGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexRefNamScalar_EM_YY
//
//  Array index reference for scalar named arg
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexRefNamScalar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ArrayIndexRefNamScalar_EM_YY
    (
     HGLOBAL    hGlbNam,            // Name arg global memory handle
     LPVOID     lpMemNam,           // Ptr to name arg global memory (points to header)
     APLSTYPE   aplTypeNam,         // Name arg storage type
     APLLONGEST aplLongestNam,      // Name arg immediate value
     HGLOBAL    hGlbLst,            // List arg global memory handle
     LPVOID     lpMemLst,           // Ptr to list arg global memory (points to header)
     APLSTYPE   aplTypeLst,         // List arg storage type
     APLNELM    aplNELMLst,         // List arg NELM
     APLRANK    aplRankLst,         // List arg rank
     LPTOKEN    lptkFunc)           // Ptr to function token

{
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    APLNELM      aplNELMRes;        // Result NELM
    LPAPLDIM     lpMemDimLst;       // Ptr to list arg dimensions
    APLUINT      ByteRes;           // # bytes in the result
    HGLOBAL      hGlbRes;           // Result global memory handle
    LPVOID       lpMemRes;          // Ptr to result global memory
    APLUINT      uRes;              // Loop counter

    // Check for RANK ERROR
    if (IsSimple (aplTypeLst))
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Copy as result NELM
    aplNELMRes = aplNELMLst;

    // Handle prototypes
    aplNELMLst = max (aplNELMLst, 1);

    // Skip over the dimensions
    lpMemDimLst = VarArrayBaseToDim (lpMemLst);

    // Skip over the header and dimensions to the data
    lpMemLst = VarArrayBaseToData (lpMemLst, aplRankLst);

    // Loop through the list arg looking for simple empty vectors
    if (!ArrayIndexValidZilde_EM (lpMemLst, aplNELMLst, lptkFunc))
        goto ERROR_EXIT;

    // The list arg has been validated, return ({rho}R){rho}L

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeNam, aplNELMRes, aplRankLst);

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
    lpHeader->ArrType    = aplTypeNam;
////lpHeader->Perm       = 0;               // Already zero from GHND
////lpHeader->SysVar     = 0;               // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMLst;
    lpHeader->Rank       = aplRankLst;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Copy the list arg dimensions to the result
    CopyMemory (lpMemRes, lpMemDimLst, (UINT) aplRankLst * sizeof (APLDIM));

    // Skip over the dimensions to the data
    lpMemRes = VarArrayDimToData (lpMemRes, aplRankLst);

    // Repeat the (scalar) name arg in the result

    // Split cases based upon the name arg storage type
    switch (aplTypeNam)
    {
        case ARRAY_BOOL:
            if ((APLBOOL) aplLongestNam)
                FillMemory (lpMemRes, (UINT) RoundUpBits8 (aplNELMLst), 0xFF);
            break;

        case ARRAY_INT:
            for (uRes = 0; uRes < aplNELMLst; uRes++)
                *((LPAPLINT) lpMemRes)++ = (APLINT) aplLongestNam;
            break;

        case ARRAY_FLOAT:
            for (uRes = 0; uRes < aplNELMLst; uRes++)
                *((LPAPLFLOAT) lpMemRes)++ = *(LPAPLFLOAT) &aplLongestNam;
            break;

        case ARRAY_CHAR:
            for (uRes = 0; uRes < aplNELMLst; uRes++)
                *((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestNam;
            break;

        case ARRAY_NESTED:
            // Skip over the header and dimensions to the data
            lpMemNam = VarArrayBaseToData (lpMemNam, 0);

            // If the list arg is empty, copy the name arg prototype to the result
            if (aplNELMLst EQ 0)
                *((LPAPLNESTED) lpMemRes) =
                  MakeMonPrototype_EM (ClrPtrTypeIndGlb (lpMemNam), // Proto arg handle
                                       lptkFunc,                    // Ptr to function token
                                       MP_CHARS);                   // CHARs allowed
            else
            for (uRes = 0; uRes < aplNELMLst; uRes++)
                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbInd (lpMemNam);
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
} // End ArrayIndexRefNamScalar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexValidZilde_EM
//
//  Loop through the index arg looking for simple empty vectors
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexValidZilde_EM"
#else
#define APPEND_NAME
#endif

BOOL ArrayIndexValidZilde_EM
    (LPVOID  lpMemNdx,              // Ptr to index arg global memory
     APLNELM aplNELMNdx,            // Index arg NELM
     LPTOKEN lptkFunc)              // Ptr to function token

{
    APLUINT  uNdx;                  // Loop counter
    APLSTYPE aplTypeSub;            // Index arg item storage type
    APLNELM  aplNELMSub;            // ...            NELM
    APLRANK  aplRankSub;            // ...            rank

    // Loop through the list arg looking for simple empty vectors
    for (uNdx = 0; uNdx < aplNELMNdx; uNdx++)
    // Split cases based upon the list arg item ptr type
    switch (GetPtrTypeDir (((LPAPLNESTED) lpMemNdx)[uNdx]))
    {
        case PTRTYPE_STCONST:       // Immediates are NELM 1, Rank 0
            // It's not a vector, so signal a RANK ERROR
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkFunc);
            return FALSE;

        case PTRTYPE_HGLOBAL:
            // Get the attributes (Type, NELM, and Rank) of the list arg item
            AttrsOfGlb (ClrPtrTypeDirGlb (((LPAPLNESTED) lpMemNdx)[uNdx]), &aplTypeSub, &aplNELMSub, &aplRankSub, NULL);

            // Check for RANK ERROR
            if (aplRankSub NE 1)
            {
                ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                           lptkFunc);
                return FALSE;
            } // End IF

            // Check for LENGTH ERROR
            if (aplNELMSub NE 0)
            {
                ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                           lptkFunc);
                return FALSE;
            } // End IF

            // Check for DOMAIN ERROR
            if (!IsSimple (aplTypeSub))
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                return FALSE;
            } // End IF

            break;

        defstop
            break;
    } // End FOR

    return TRUE;
} // End ArrayIndexValidZilde_EM
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
    (LPTOKEN     lptkLstArg,        // Ptr to list arg token
     HGLOBAL     hGlbNam,           // Name arg gobal memory handle
     APLRANK     aplRankNam,        // Name arg rank
     LPAPLNESTED lpMemLst,          // Ptr to list arg global memory (points to list header)
     APLNELM     aplNELMLst,        // List arg NELM
     LPTOKEN     lptkFunc)          // Ptr to function token

{
    APLUINT      uLst,              // Loop counter
                 uCount,            // Count of non-elided indices
                 ByteRes;           // # bytes in the result
    HGLOBAL      hGlbLstNew = NULL, // New list arg global memory handle
                 hGlbLstOld = NULL, // Old ...
                 hGlbAxis = NULL;   // Axis operator global memory handle
    LPAPLNESTED  lpMemLstNew = NULL;  // Ptr to new list arg global memory
    LPAPLUINT    lpMemAxis = NULL;  // Ptr to axis operator global memory
    BOOL         bQuadIO;           // []IO
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    TOKEN        tkLstArg = {0},    // New list arg token
                 tkAxis = {0};      // Axis token

    // Skip over the header and dimension
    lpMemLst = VarArrayBaseToData (lpMemLst, 1);

    // Loop through the elements of the list arg
    //   counting the # non-placeholders (non-elided) indices
    for (uCount = uLst = 0; uLst < aplNELMLst; uLst++)
        uCount += !PtrReusedDir (lpMemLst[uLst]);

    // Check for RANK ERROR
    if (aplRankNam NE aplNELMLst
     && aplNELMLst NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // If there are no elided indices, use squad without axis operator
    if (uCount EQ aplNELMLst)
        // Note the args get switched between indexing and squad
        lpYYRes = PrimFnDydSquadGlb_EM_YY (lptkLstArg,     // Ptr to Left arg token
                                           hGlbNam,        // Right arg global memory handle
                                           NULL,           // Ptr to axis token (may be NULL)
                                           lptkFunc);      // Ptr to function token
    else
    // There are elided indices, so we must create a new
    //   list arg (w/o the elided indices) and a new
    //   axis operator value
    {
        // Calculate bytes needed for the new list arg
        ByteRes = CalcArraySize (ARRAY_NESTED, uCount, 1);

        // Allocate space for the new list arg
        hGlbLstNew = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (!hGlbLstNew)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemLstNew = MyGlobalLock (hGlbLstNew);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemLstNew)
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
        *VarArrayBaseToDim (lpMemLstNew) = uCount;

        // Skip over the header and dimension to the data
        lpMemLstNew = VarArrayBaseToData (lpMemLstNew, 1);

        // Handle prototypes
        if (uCount EQ 0)
            // Use {zilde} as the prototype
            *lpMemLstNew = MakeGlbTypeGlb (hGlbZilde);
        else
        // Copy the non-elided indices to the new list arg
        for (uLst = 0; uLst < aplNELMLst; uLst++)
        if (!PtrReusedDir (lpMemLst[uLst]))
            *lpMemLstNew++ = CopySymGlbDir (lpMemLst[uLst]);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbLstNew); lpMemLstNew = NULL;

        // Fill in the new list arg token
        tkLstArg.tkFlags.TknType   = TKT_VARARRAY;
////////tkLstArg.tkFlags.ImmType   = 0;     // Already zero from = {0}
////////tkLstArg.tkFlags.NoDisplay = 0;     // Already zero from = {0}
        tkLstArg.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbLstNew);
        tkLstArg.tkCharIndex       = lptkFunc->tkCharIndex;

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
        for (uCount = uLst = 0; uLst < aplNELMLst; uLst++, uCount++)
        if (!PtrReusedDir (lpMemLst[uLst]))
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
        lpYYRes = PrimFnDydSquadGlb_EM_YY (&tkLstArg,   // Ptr to left arg token
                                            hGlbNam,    // Right arg global memory handle
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

    if (hGlbLstNew)
    {
        if (lpMemLstNew)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbLstNew); lpMemLstNew = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbLstNew); hGlbLstNew = NULL;
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
    (LPTOKEN lptkNamArg,            // Ptr to named arg token
     LPTOKEN lptkLstArg,            // Ptr to list arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    HGLOBAL    hGlbNam = NULL,      // Named arg global memory handle
               hGlbLst = NULL,      // List arg  ...
               hGlbSubLst = NULL,   // List arg item ...
               hGlbRht = NULL,      // Right arg ...
               hGlbSubRht = NULL,   // Right arg item...
               hGlbRes = NULL;      // Result    ...
    LPVOID     lpMemNam = NULL,     // Ptr to name arg global memory
               lpMemLst = NULL,     // Ptr to list arg global memory
               lpMemSubLst = NULL,  // Ptr to list arg item global memory
               lpMemRht = NULL,     // Ptr to right arg global memory
               lpMemRes = NULL;     // Ptr to result   ...
    APLSTYPE   aplTypeNam,          // Named arg storage type
               aplTypeSubLst,       // List arg subitem ...
               aplTypeRht,          // Right ...
               aplTypeRes;          // Result    ...
    APLNELM    aplNELMNam,          // Named arg NELM
               aplNELMLst,          // List ...
               aplNELMSubLst,       // List arg subitem ...
               aplNELMRht;          // Right ...
    APLRANK    aplRankNam,          // Named arg rank
               aplRankSubLst,       // List arg subitem ...
               aplRankRht;          // Right ...
    APLUINT    uRht;                // Loop counter
    APLLONGEST aplLongestSubLst,    // List arg subitem immediate value
               aplLongestRht;       // Right arg item immediate value
    IMM_TYPES  immTypeRht;          // Right arg item immediate type
    BOOL       bRet = FALSE;        // TRUE iff the result is valid
    APLUINT    ByteRes;             // # bytes in the result

#define lptkFunc        lptkNamArg

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Get the attributes (Type, NELM, and Rank) of the list arg
    AttrsOfToken (lptkLstArg, NULL, &aplNELMLst, NULL, NULL);

    // Get list arg's global ptrs
    GetGlbPtrs_LOCK (lptkLstArg, &hGlbLst, &lpMemLst);

    // Get the attributes (Type, NELM, and Rank) of the named arg
    AttrsOfToken (lptkNamArg, &aplTypeNam, &aplNELMNam, &aplRankNam, NULL);

    // Get name arg's global ptrs
    GetGlbPtrs_LOCK (lptkNamArg, &hGlbNam, &lpMemNam);

    // Split cases based upon whether or not the named
    //   var is immediate
    if (lptkNamArg->tkData.tkSym->stFlags.Imm)
    {
        APLSTYPE   aplTypeSubLst;
        APLNELM    aplNELMSubLst;
        APLRANK    aplRankSubLst;

        //***************************************************************
        // The named arg value is immediate
        //***************************************************************

        // Check for RANK ERROR
        if (aplNELMLst NE 1
         || hGlbLst EQ NULL)
        {
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Skip over the header and dimensions to the data
        lpMemLst = VarArrayBaseToData (lpMemLst, 1);

        // Split cases based upon the list arg item ptr type
        switch (GetPtrTypeInd (lpMemLst))
        {
            case PTRTYPE_STCONST:
                ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;

            case PTRTYPE_HGLOBAL:
                // Extract the single item from the list
                hGlbSubLst = ClrPtrTypeIndGlb (lpMemLst);

                break;

            defstop
                break;
        } // End SWITCH

        // We no longer need this ptr
        MyGlobalUnlock (hGlbLst); lpMemLst = NULL;

        // Get the attributes (Type, NELM, and Rank) of the list arg item
        AttrsOfGlb (hGlbSubLst, &aplTypeSubLst, &aplNELMSubLst, &aplRankSubLst, NULL);

        // Check for RANK ERROR
        if ((aplNELMSubLst NE 1 || aplNELMRht NE 1)
         && aplRankSubLst NE aplRankRht)
        {
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemSubLst = MyGlobalLock (hGlbSubLst);

        // Check for LENGTH ERROR
        if (aplNELMSubLst NE 1 || aplNELMRht NE 1)
        {
            // Skip over the header to the dimensions
            lpMemRht    = VarArrayBaseToDim (lpMemRht);
            lpMemSubLst = VarArrayBaseToDim (lpMemSubLst);

            for (uRht = 0; uRht < aplRankRht; uRht++)
            if (*((LPAPLDIM) lpMemSubLst)++ NE *((LPAPLDIM) lpMemRht)++)
            {
                ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End FOR/IF
        } else
        {
            if (hGlbRht)
            {
                // Skip over the header and dimensions to the data
                lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
            } // End IF

            // Skip over the header and dimensions to the data
            lpMemSubLst = VarArrayBaseToData (lpMemSubLst, aplRankSubLst);
        } // End IF/ELSE

        // Confirm that the list of indices is an array of zildes
        if (!ArrayIndexValidZilde_EM (lpMemSubLst, aplNELMSubLst, lptkFunc))
            goto ERROR_EXIT;

        // If the index arg is empty, just quit
        if (aplNELMSubLst EQ 0)
            goto NORMAL_EXIT;

        // Assign the last item of the right arg to the named arg
        if (hGlbRht)
            GetNextValueMem (lpMemRht,
                             aplTypeRht,
                             aplNELMRht - 1,
                            &hGlbSubRht,
                            &aplLongestRht,
                            &immTypeRht);
        else
            immTypeRht = TranslateArrayTypeToImmType (aplTypeRht);

        // If the last item is a global, ...
        if (hGlbSubRht)
        {
            // Calculate space needed for the result
            ByteRes = CalcArraySize (ARRAY_NESTED, 1, 0);

            // Wrap the global in a scalar
            hGlbRes = MyGlobalAlloc (GHND, (UINT) ByteRes);
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
////////////lpHeader->Perm       = 0;               // Already zero from GHND
////////////lpHeader->SysVar     = 0;               // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = 1;
            lpHeader->Rank       = 0;
#undef  lpHeader

            // Skip over the header and dimensions to the data
            lpMemRes = VarArrayBaseToData (lpMemRes, 0);

            // Fill in the data
            *((LPAPLNESTED) lpMemRes) = CopySymGlbDir (MakeGlbTypeGlb (hGlbSubRht));

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

            // Mark as no longer immediate
            lptkNamArg->tkData.tkSym->stFlags.Imm = FALSE;
            lptkNamArg->tkData.tkSym->stFlags.ImmType = 0;

            // Save the new global memory handle in the STE
            lptkNamArg->tkData.tkSym->stData.stGlbData = MakeGlbTypeGlb (hGlbRes);
        } else
        {
            // Save the new immediate type & value
            lptkNamArg->tkData.tkSym->stFlags.ImmType  = immTypeRht;
            lptkNamArg->tkData.tkSym->stData.stLongest = aplLongestRht;
        } // End IF/ELSE
    } else
    {
        //***************************************************************
        // The named arg value is global handle
        //***************************************************************

        // Check for RANK ERROR
        if (aplNELMLst > 1
         && aplRankNam NE aplNELMLst)
        {
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Skip over the header to the dimensions
        lpMemNam = VarArrayBaseToDim (lpMemNam);

        //***************************************************************
        // If the list arg is empty, ...
        //***************************************************************
        if (aplNELMLst EQ 0)
        {
            LPPL_YYSTYPE lpYYRes1,          // Ptr to primary result
                         lpYYRes2;          // Ptr to secondary result

            // Check for RANK ERROR between the list and right args
            if (aplNELMRht NE 1
             && aplRankNam NE aplRankRht)
            {
                ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Check for LENGTH ERROR between the list and right args
            if (aplNELMRht NE 1)
            {
                // Skip over the header to the dimensions
                lpMemRht = VarArrayBaseToDim (lpMemRht);

                for (uRht = 0; uRht < aplRankRht; uRht++)
                if (*((LPAPLDIM) lpMemNam)++ NE *((LPAPLDIM) lpMemRht)++)
                {
                    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                               lptkFunc);
                    goto ERROR_EXIT;
                } // End FOR/IF
            } else
            {
                // Skip over the header and dimensions to the data
                lpMemNam = VarArrayBaseToData (lpMemNam, aplRankNam);
                if (hGlbRht)
                    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
            } // End IF/ELSE

            // Replace all elements of the named arg with those of the right arg

            // Find the shape of the named arg
            lpYYRes1 = PrimFnMonRho_EM_YY (lptkFunc,                // Ptr to function token
                                           lptkNamArg,              // Ptr to right arg token
                                           NULL);                   // Ptr to axis token
            // Reshape the right arg to the shape of the named arg
            lpYYRes2 = PrimFnDydRho_EM_YY (&lpYYRes1->tkToken,      // Ptr to left arg token
                                            lptkFunc,               // Ptr to function token
                                            lptkRhtArg,             // Ptr to right arg token
                                            NULL);                  // Ptr to axis token
            // Free the first result
            FreeResult (&lpYYRes1->tkToken); YYFree (lpYYRes1); lpYYRes1 = NULL;

            // Get the global memory handle
            hGlbRes = ClrPtrTypeDirGlb (lpYYRes2->tkToken.tkData.tkGlbData);

            // Free the second result
            YYFree (lpYYRes2); lpYYRes2 = NULL;
        } else
        {
            // Trundle through the index list and the right arg
            //   validating the indices and replacing the
            //   corresponding item in the named arg

            DbgBrk ();          // ***FINISHME*** -- A[L]{is}R

            // Because this operation is supposed to be atomic,
            //   we need to copy the entire array first, change
            //   the copied array (blowing it up if necessary),
            //   delete the old array, and change the STE to
            //   point to the new array.
            hGlbRes = CopyArray_EM (hGlbNam, lptkNamArg);
            if (!hGlbRes)
            {
                ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

////////////// Lock the memory to get a ptr to it (not yet as we might type promote)
////////////lpMemRes = MyGlobalLock (hGlbRes);

            //***************************************************************
            // If the list arg is a singleton, ...
            //***************************************************************
            if (aplNELMLst EQ 1)
            {
                DbgBrk ();          // ***FINISHME*** -- Singleton list arg
////////////////PrimFnNonceError_EM (lptkFunc);

                // Check for RANK ERROR between the list and right args
                if (aplNELMRht NE 1
                 && aplRankNam NE aplRankRht)
                {
                    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                               lptkFunc);
                    goto ERROR_EXIT;
                } // End IF

                // Check for LENGTH ERROR between the list and right args
                if (aplNELMRht NE 1)
                {
                    // Skip over the header to the dimensions
                    lpMemRht = VarArrayBaseToDim (lpMemRht);

                    for (uRht = 0; uRht < aplRankRht; uRht++)
                    if (*((LPAPLDIM) lpMemNam)++ NE *((LPAPLDIM) lpMemRht)++)
                    {
                        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                                   lptkFunc);
                        goto ERROR_EXIT;
                    } // End FOR/IF
                } // End IF

                // Skip over the header and dimensions to the data
                lpMemLst = VarArrayBaseToData (lpMemLst, 1);

                // Split cases based upon the list arg item ptr type
                switch (GetPtrTypeInd (lpMemLst))
                {
                    case PTRTYPE_STCONST:
                        // Set the vars for an STE
                        hGlbSubLst = NULL;
                        aplTypeSubLst = TranslateImmTypeToArrayType ((*(LPAPLHETERO) lpMemLst)->stFlags.ImmType);
                        aplNELMSubLst = 1;
                        aplRankSubLst = 0;
                        aplLongestSubLst = (*(LPAPLHETERO) lpMemLst)->stData.stLongest;

                        break;

                    case PTRTYPE_HGLOBAL:
                        // Set the vars for an HGLOBAL
                        hGlbSubLst = ClrPtrTypeIndGlb (lpMemLst);
                        AttrsOfGlb (hGlbSubLst, &aplTypeSubLst, &aplNELMSubLst, &aplRankSubLst, NULL);
                        lpMemSubLst = MyGlobalLock (hGlbSubLst);

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Handle obvious DOMAIN ERRORs
                if (aplTypeSubLst EQ ARRAY_HETERO
                 || (aplTypeSubLst EQ ARRAY_CHAR
                  && aplNELMSubLst NE 0))
                {
                    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                               lptkFunc);
                    goto ERROR_EXIT;
                } // End IF

                //***************************************************************
                // Split off cases
                //***************************************************************

                //***************************************************************
                // If the list arg item is immediate, do rectangular indexing.
                // The name arg must be a vector.
                //***************************************************************
                if (hGlbSubLst EQ NULL)
                {
                    DbgBrk ();      // ***FINISHME*** -- V[2]{is}R with or w/o type promotion

                    // Check for type promotion

                    // Calculate the storage type of the result
                    aplTypeRes = aTypePromote[aplTypeNam][aplTypeRht];

                    // If the types are the same, there's no need for type promotion
                    if (aplTypeRes EQ aplTypeNam)
                    {
                        // Get the one (and only) item from the right arg
                        FirstItem (lptkRhtArg,      // Ptr to the token
                                  &aplLongestRht,   // Ptr to the longest (may be NULL)
                                  &immTypeRht,      // ...        immediate type:  IMM_TYPES (may be NULL)
                                  &hGlbSubRht);     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
                        // Split cases based upon the common storage type
                        switch (aplTypeRes)
                        {
                            case ARRAY_BOOL:
                                ((LPAPLBOOL)   lpMemNam)[aplLongestSubLst >> LOG2NBIB] |= (BIT0 & (APLBOOL) aplLongestRht) << (MASKLOG2NBIB & aplLongestSubLst);

                                break;

                            case ARRAY_INT:
                                ((LPAPLINT)    lpMemNam)[aplLongestSubLst] = (APLINT) aplLongestRht;

                                break;

                            case ARRAY_FLOAT:
                                ((LPAPLFLOAT)  lpMemNam)[aplLongestSubLst] = *(LPAPLFLOAT) &aplLongestRht;

                                break;

                            case ARRAY_CHAR:
                                ((LPAPLCHAR)   lpMemNam)[aplLongestSubLst] = (APLCHAR) aplLongestRht;

                                break;

                            case ARRAY_HETERO:
                            case ARRAY_NESTED:
                                ((LPAPLHETERO) lpMemNam)[aplLongestSubLst] = CopySymGlbDir (hGlbSubRht);

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } else
                    {
                        DbgBrk ();  // ***FINISHME*** -- V[2]{is}R with type promotion



                    } // End IF/ELSE

                    goto NORMAL_EXIT;
                } // End IF

                //***************************************************************
                // From here on the list arg is a global.
                //***************************************************************

                //***************************************************************
                // If the list arg item is simple, do rectangular indexing.
                // The name arg must be a vector.
                //***************************************************************
                if (IsSimple (aplTypeSubLst))
                {
                    DbgBrk ();





                    goto NORMAL_EXIT;
                } // End IF

                //***************************************************************
                // If the name arg is scalar, do reach indexing (special case).
                // The list arg item must be a nested array of simple empty numeric
                //   or char vectors.
                //***************************************************************
                if (aplRankNam EQ 0)
                {
                    DbgBrk ();






                    goto NORMAL_EXIT;
                } // End IF

                //***************************************************************
                // From here on the name arg is a global.
                //***************************************************************

                //***************************************************************
                // If the list arg item is a nested array of equal length simple
                //   numeric vectors, do scatter indexing, in which case
                //   the name arg rank must be the same as the length of the
                //   list arg item sub items.
                // If the list item arg is a nested array of nested vectors,
                //   do reach indexing, in which case
                //   the name arg rank must be the same as the length of the
                //   first element of each of the list arg sub items.
                //***************************************************************

                DbgBrk ();








            } else
            //***************************************************************
            // The list arg list has multiple elements:  do rectangular indexing.
            // The name arg must be a matching rank array.
            //***************************************************************
            {
                DbgBrk ();
////////////////PrimFnNonceError_EM (lptkFunc);















            } // End IF/ELSE/...

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF/ELSE/...

        // We no longer need this ptr
        MyGlobalUnlock (hGlbNam); lpMemNam = NULL;

        // Free the original global memory handle
        FreeResultGlobalVar (hGlbNam); hGlbNam = NULL;

        // Save the new global memory handle in the STE
        lptkNamArg->tkData.tkSym->stData.stGlbData = MakeGlbTypeGlb (hGlbRes);
    } // End IF/ELSE

    bRet = TRUE;

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
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbSubLst && lpMemSubLst)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbSubLst); lpMemSubLst = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (hGlbLst && lpMemLst)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLst); lpMemLst = NULL;
    } // End IF

    if (hGlbNam && lpMemNam)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbNam); lpMemNam = NULL;
    } // End IF

    return bRet;
} // End ArrayIndexSet_EM
#undef  lptkFunc
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
    lpYYRes1 = ArrayIndexRef_EM_YY (lptkNamArg,         // Ptr to name arg token
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
