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
            lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDirAsGlb (hGlbNam);
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
                hGlbSub = ClrPtrTypeIndAsGlb (lpMemLst);
                AttrsOfGlb (hGlbSub, &aplTypeSub, &aplNELMSub, &aplRankSub, NULL);
                lpMemSub = MyGlobalLock (hGlbSub);

                break;

            defstop
                break;
        } // End SWITCH

        // Handle obvious DOMAIN ERRORs
        if (aplTypeSub EQ ARRAY_HETERO
         || (IsSimpleChar (aplTypeSub)
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
            lpYYRes = ArrayIndexRefLstImm_EM_YY (hGlbNam,           // Name arg global memory handle
                                                 aplTypeNam,        // Name arg storage type
                                                 aplNELMNam,        // Name arg NELM
                                                 aplRankNam,        // Name arg rank
                                                 aplTypeSub,        // List arg storage type
                                                 aplLongestSub,     // List arg immediate value
                                                 aplTypeRes,        // Result storage type
                                                 lptkFunc);         // Ptr to function token
        else
        //***************************************************************
        // From here on the siongleton list arg is a global.
        //***************************************************************

        //***************************************************************
        // If the singleton list arg item is simple, do rectangular indexing.
        // The name arg must be a vector.
        //***************************************************************
        if (IsSimple (aplTypeSub))
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
        else
        //***************************************************************
        // If the name arg is scalar, do reach indexing (special case).
        // The list arg item must be a nested array of simple empty numeric
        //   or char vectors.
        //***************************************************************
        if (aplRankNam EQ 0)
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
        else
        {
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
////////////lpHeader->Perm       = 0;               // Already zero from GHND
////////////lpHeader->SysVar     = 0;               // Already zero from GHND
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

            // If the result is a HETERO/NESTED result, fill all items with PTR_REUSED
            //   in case we fail later on
            if (!IsSimple (aplTypeRes))
            {
                // Handle protoype case
                *((LPAPLNESTED) lpMemRes) = PTR_REUSED;

                // Fill remaining result items with PTR_REUSED
                for (uSub = 1; uSub < aplNELMSub; uSub++)
                    ((LPAPLNESTED) lpMemRes)[uSub] = PTR_REUSED;
            } // End IF

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
                    if (IsImmChr (immTypeItm))
                    {
                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        goto ERROR_EXIT;
                    } // End IF

                    // If the immediate is a float, attempt to convert it
                    if (IsImmFlt (immTypeItm))
                    {
                        // Attempt to convert the float to an integer using System CT
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
                            uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) aplLongestItm);

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
                    hGlbItm = ClrPtrTypeDirAsGlb (((LPAPLNESTED) lpMemSub)[uSub]);

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
////////////////////////tkLstArg.tkFlags.ImmType   = 0;     // Already zero from = {0}
////////////////////////tkLstArg.tkFlags.NoDisplay = 0;     // Already zero from = {0}
                        tkLstArg.tkData.tkGlbData  = MakePtrTypeGlb (hGlbItm);
                        tkLstArg.tkCharIndex       = lptkFunc->tkCharIndex;

                        // Note the args get switched between indexing and squad
                        lpYYItm =
                          PrimFnDydSquadGlb_EM_YY (&tkLstArg,           // Ptr to left arg token
                                                    hGlbNam,            // Right arg global memory handle
                                                    NULL,               // Ptr to axis token (may be NULL)
                                                    lptkFunc,           // Ptr to function token
                                                    NULL);              // Ptr to set arg token
                        // If valid, disclose the item as squad returns an enclosed item
                        if (lpYYItm)
                        {
                            LPPL_YYSTYPE lpYYItm2;

                            lpYYItm2 = PrimFnMonRightShoe_EM_YY (lptkFunc,
                                                                &lpYYItm->tkToken,
                                                                 NULL);
                            if (lpYYItm2)
                            {
                                FreeResult (&lpYYItm->tkToken); YYFree (lpYYItm); lpYYItm = NULL;
                                lpYYItm = lpYYItm2;
                            } // End IF
                        } // End IF
                    } else
                        // Note the args get switched between indexing and pick
                        lpYYItm =
                          PrimFnDydRightShoeGlbGlb_EM_YY (hGlbItm,      // Left arg global memory handle
                                                          hGlbNam,      // Right arg global memory handle
                                                          lptkFunc,     // Ptr to function token
                                                          FALSE,        // TRUE iff array assignment
                                                          ARRAY_ERROR,  // Set arg storage type
                                                          NULL,         // Set arg global memory handle/LPSYMENTRY (NULL if immediate)
                                                          0);           // Set arg immediate value
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
////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

            // Unlock the result global memory in case TypeDemote actually demotes
            if (hGlbRes && lpMemRes)
            {
                // We no longer need this ptr
                MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
            } // End IF
        } // End IF/ELSE/...

        // See if it fits into a lower (but not necessarily smaller) datatype
        TypeDemote (&lpYYRes->tkToken);
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
        // Attempt to convert the float to an integer using System CT
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
        // The name arg is a global

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
        *((LPAPLNESTED) lpMemRes) = CopySymGlbDirAsGlb (hGlbSub);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
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


    // If the result is a HETERO/NESTED result, fill all items with PTR_REUSED
    //   in case we fail later on
    if (!IsSimple (aplTypeRes))
    {
        // Handle protoype case
        if (aplTypeRes EQ ARRAY_NESTED)
            *((LPAPLNESTED) lpMemRes) = PTR_REUSED;
        // Fill result items with PTR_REUSED
        for (uLst = 0; uLst < aplNELMLst; uLst++)
            ((LPAPLNESTED) lpMemRes)[uLst] = PTR_REUSED;
    } // End IF

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
        hGlbProto =
          MakeMonPrototype_EM (ClrPtrTypeIndAsGlb (lpMemNam),   // Proto arg handle
                               lptkFunc,                        // Ptr to function token
                               MP_CHARS);                       // CHARs allowed
        // We no longer this ptr
        MyGlobalUnlock (hGlbNam); lpMemNam = NULL;

        if (!hGlbProto)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Save the value in the result
        *((LPAPLNESTED) lpMemRes) = MakePtrTypeGlb (hGlbProto);
    } else
    // Loop through the list arg elements
    for (uLst = 0; uLst < aplNELMLst; uLst++)
    {
        APLLONGEST aplLongestNam,           // Name arg immediate value
                   aplLongestLst;           // List ...

        // Get the next index from the list arg
        GetNextValueGlb (hGlbLst,           // List arg global memory handle
                         uLst,              // Index into list arg
                         NULL,              // Ptr to list global memory handle (may be NULL)
                        &aplLongestLst,     // Ptr to list immediate value (may be NULL)
                         NULL);             // Ptr to list immediate type (may be NULL)
        // Convert float to int if necessary
        if (aplTypeLst EQ ARRAY_FLOAT)
        {
            // Attempt to convert the float to an integer using System CT
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

        // Check for INDEX ERROR
        if (aplLongestLst >= aplNELMNam)
        {
            ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Get the indexed item from the name arg
        GetNextItemGlb (hGlbNam,            // Item global memory handle
                        aplLongestLst,      // Index into item
                       &hGlbSub,            // Ptr to result global memory handle (may be NULL)
                       &aplLongestNam);     // Ptr to result immediate value (may be NULL)
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
                *((LPAPLINT)    lpMemRes)++ = (APLINT) aplLongestNam;

                break;

            case ARRAY_FLOAT:
                // Save the value in the result
                *((LPAPLFLOAT)  lpMemRes)++ = *(LPAPLFLOAT) &aplLongestNam;

                break;

            case ARRAY_CHAR:
                // Save the value in the result
                *((LPAPLCHAR)   lpMemRes)++ = (APLCHAR) aplLongestNam;

                break;

            case ARRAY_HETERO:
            case ARRAY_NESTED:
                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (hGlbSub);

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
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

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
//  Array index reference for scalar name arg
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
    HGLOBAL      hGlbRes = NULL;    // Result global memory handle
    LPVOID       lpMemRes = NULL;   // Ptr to result global memory
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
            {
                HGLOBAL hGlbProto;          // Prototype global memory handle

                hGlbProto =
                  MakeMonPrototype_EM (ClrPtrTypeIndAsGlb (lpMemNam),   // Proto arg handle
                                       lptkFunc,                        // Ptr to function token
                                       MP_CHARS);                       // CHARs allowed
                if (!hGlbProto)
                    goto ERROR_EXIT;
                *((LPAPLNESTED) lpMemRes) = MakePtrTypeGlb (hGlbProto);
            } else
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
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

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
            AttrsOfGlb (ClrPtrTypeDirAsGlb (((LPAPLNESTED) lpMemNdx)[uNdx]), &aplTypeSub, &aplNELMSub, &aplRankSub, NULL);

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
    (LPTOKEN     lptkLstArg,            // Ptr to list arg token
     HGLOBAL     hGlbNam,               // Name arg gobal memory handle
     APLRANK     aplRankNam,            // Name arg rank
     LPAPLNESTED lpMemLst,              // Ptr to list arg global memory (points to list header)
     APLNELM     aplNELMLst,            // List arg NELM
     LPTOKEN     lptkFunc)              // Ptr to function token

{
    APLUINT      uLst,                  // Loop counter
                 uCount,                // Count of non-elided indices
                 ByteRes;               // # bytes in the result
    HGLOBAL      hGlbLstNew = NULL,     // New list arg global memory handle
                 hGlbAxis = NULL;       // Axis operator global memory handle
    LPAPLNESTED  lpMemLstNew = NULL;    // Ptr to new list arg global memory
    LPAPLUINT    lpMemAxis = NULL;      // Ptr to axis operator global memory
    BOOL         bQuadIO;               // []IO
    LPPL_YYSTYPE lpYYRes = NULL;        // Ptr to the result
    TOKEN        tkLstArg = {0},        // New list arg token
                 tkAxis = {0};          // Axis token

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
        lpYYRes =
          PrimFnDydSquadGlb_EM_YY (lptkLstArg,  // Ptr to Left arg token
                                   hGlbNam,     // Right arg global memory handle
                                   NULL,        // Ptr to axis token (may be NULL)
                                   lptkFunc,    // Ptr to function token
                                   NULL);       // Ptr to set arg token
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
            *lpMemLstNew = MakePtrTypeGlb (hGlbZilde);
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
        tkLstArg.tkData.tkGlbData  = MakePtrTypeGlb (hGlbLstNew);
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
        tkAxis.tkData.tkGlbData  = MakePtrTypeGlb (hGlbAxis);
        tkAxis.tkCharIndex       = lptkFunc->tkCharIndex;

        // Note the args get switched between indexing and squad
        lpYYRes =
          PrimFnDydSquadGlb_EM_YY (&tkLstArg,       // Ptr to left arg token
                                    hGlbNam,        // Right arg global memory handle
                                   &tkAxis,         // Ptr to axis token (may be NULL)
                                    lptkFunc,       // Ptr to function token
                                    NULL);          // Ptr to set arg token
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
                lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
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
//  Array index assignment, e.g., A[L]{is}R
//                            or  A[L1;...;Ln]{is}R
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
    HGLOBAL hGlbNam = NULL,         // Name arg global memory handle
            hGlbRes = NULL;         // Result   ...
    APLNELM aplNELMLst;             // List NELM
    APLRANK aplRankNam;             // Name arg rank
    BOOL    bRet = TRUE;            // TRUE iff the result is valid

#define lptkFunc        lptkNamArg

    // Get the attributes (Type, NELM, and Rank) of the name & list args
    AttrsOfToken (lptkNamArg, NULL,  NULL,       &aplRankNam, NULL);
    AttrsOfToken (lptkLstArg, NULL, &aplNELMLst,  NULL,       NULL);

    // If the name arg is a system var, validate the args
    if (IsNameTypeVar (lptkNamArg->tkData.tkSym->stFlags.ObjType)
     && lptkNamArg->tkData.tkSym->stFlags.ObjName EQ OBJNAME_SYS)
    {
        // If the target is a user-defined function/operator system label, signal a SYNTAX ERROR
        if (lptkNamArg->tkData.tkSym->stFlags.DfnSysLabel)
        {
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Validate the value
        if (!(*aSysVarValid[lptkNamArg->tkData.tkSym->stFlags.SysVarValid]) (lptkNamArg, lptkLstArg, lptkRhtArg))
            goto ERROR_EXIT;
    } // End IF

    // Split cases based upon whether or not the name
    //   var is scalar (this also covers immediates)
    if (aplRankNam EQ 0)
    {
        //***************************************************************
        // The name arg value is a scalar
        //***************************************************************

        if (!ArrayIndexSetNamScalar_EM (lptkNamArg,         // Ptr to name arg token
                                        lptkLstArg,         // Ptr to list ...
                                        lptkRhtArg,         // Ptr to right ...
                                        lptkFunc))          // Ptr to function ...
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } else
    {
        //***************************************************************
        // The name arg value is a non-scalar global handle
        //***************************************************************

        //***************************************************************
        // If the list arg is empty, ...
        //***************************************************************
        if (aplNELMLst EQ 0)                // A[]{is}R
        {
            hGlbRes = ArrayIndexSetNoLst_EM (lptkNamArg,    // Ptr to name arg token
                                             lptkRhtArg,    // Ptr to right ...
                                             lptkFunc);     // Ptr to function ...
            if (!hGlbRes)
                goto ERROR_EXIT;
        } else
        {
            // Trundle through the index list and the right arg
            //   validating the indices and replacing the
            //   corresponding item in the name arg

            //***************************************************************
            // If the list arg is a singleton, ...
            //***************************************************************
            if (aplNELMLst EQ 1)
            {
                bRet = ArrayIndexSetSingLst_EM (lptkNamArg,     // Ptr to name arg token
                                                lptkLstArg,     // Ptr to list ...
                                                lptkRhtArg,     // Ptr to right ...
                                               &hGlbRes,        // Ptr to result global memory handle
                                                lptkFunc);      // Ptr to function token
                if (!bRet)
                    goto ERROR_EXIT;
                if (!hGlbRes)
                    goto NORMAL_EXIT;
            } else
            //***************************************************************
            // The list arg list has multiple elements:  do rectangular indexing.
            // The name arg must be a matching rank array.
            //***************************************************************
            {
                // Check for RANK ERROR between the name and list args
                if (aplRankNam NE aplNELMLst)
                {
                    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                               lptkFunc);
                    goto ERROR_EXIT;
                } // End IF

                bRet = ArrayIndexSetRect_EM (lptkNamArg,        // Ptr to name arg token
                                             lptkLstArg,        // Ptr to list ...
                                             lptkRhtArg,        // Ptr to right ...
                                            &hGlbRes,           // Ptr to result global memory handle
                                             lptkFunc);         // Ptr to function ...
                if (!bRet)
                    goto ERROR_EXIT;
                if (!hGlbRes)
                    goto NORMAL_EXIT;
            } // End IF/ELSE/...
        } // End IF/ELSE/...

        // Get name arg global ptr
        GetGlbPtrs_LOCK (lptkNamArg, &hGlbNam, NULL);

        // Free the original global memory handle
        FreeResultGlobalVar (hGlbNam); hGlbNam = NULL;

        // Save the new global memory handle in the STE
        lptkNamArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbRes);

        // See if it fits into a lower (but not necessarily smaller) datatype
        TypeDemote (lptkNamArg);
    } // End IF/ELSE

    goto NORMAL_EXIT;

ERROR_EXIT:
    bRet = FALSE;

    if (hGlbRes)
    {
        // We no longer need this storage
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    return bRet;
} // End ArrayIndexSet_EM
#undef  lptkFunc
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexSetNamScalar_EM
//
//  Array index assignment where the name arg is a scalar
//    (possibly an immediate)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexSetNamScalar_EM"
#else
#define APPEND_NAME
#endif

BOOL ArrayIndexSetNamScalar_EM
    (LPTOKEN    lptkNamArg,         // Ptr to name arg token
     LPTOKEN    lptkLstArg,         // Ptr to list ...
     LPTOKEN    lptkRhtArg,         // Ptr to right ...
     LPTOKEN    lptkFunc)           // Ptr to function token

{
    HGLOBAL    hGlbNam = NULL,      // Name arg global memory handle
               hGlbLst = NULL,      // List arg global memory handle
               hGlbRht = NULL,      // Right arg global memory handle
               hGlbSubLst = NULL,   // List arg item global memory handle
               hGlbSubRht = NULL,   // Right arg item ...
               hGlbRes = NULL;      // Result        ...
    LPVOID     lpMemNam = NULL,     // Ptr to name arg global memory
               lpMemLst = NULL,     // Ptr to list arg gobal memory
               lpMemSubLst = NULL,  // Ptr to list arg item global memory
               lpMemRht = NULL,     // Ptr to right arg global memory
               lpMemRes = NULL;     // Ptr to result        ...
    BOOL       bRet = TRUE;         // TRUE iff the result is valid
    APLSTYPE   aplTypeSubLst,       // List arg subitem storage type
               aplTypeRht;          // Right arg storage type
    APLNELM    aplNELMLst,          // List arg NELM
               aplNELMSubLst,       // List arg subitem NELM
               aplNELMRht;          // Right arg NELM
    APLRANK    aplRankNam,          // Name arg rank
               aplRankSubLst,       // List arg subitem rank
               aplRankRht;          // Right arg rank
    APLLONGEST aplLongestRht;       // Right arg immediate value
    APLUINT    ByteRes,             // # bytes in the result
               uRht;                // Loop counter
    IMM_TYPES  immTypeRht;          // Right arg item immediate type

    //***************************************************************
    // The name arg value is a scalar
    //***************************************************************

    // Get the attributes (Type, NELM, and Rank) of the name, list, and right args
    AttrsOfToken (lptkNamArg,  NULL,        NULL,       &aplRankNam, NULL);
    AttrsOfToken (lptkLstArg,  NULL,       &aplNELMLst,  NULL,       NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get name, list, and right arg global ptrs
                    GetGlbPtrs_LOCK (lptkLstArg, &hGlbLst, &lpMemLst);
                    GetGlbPtrs_LOCK (lptkNamArg, &hGlbNam, &lpMemNam);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the list is non-empty, ...
    if (aplNELMLst)
    {
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
                hGlbSubLst = ClrPtrTypeIndAsGlb (lpMemLst);

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
    } else
    {
        // Check for RANK ERROR between the name and right args
        if (aplNELMRht NE 1
         && aplRankNam NE aplRankRht)
        {
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        if (hGlbRht)
        {
            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
        } // End IF
    } // End IF/ELSE

    // Get the last item of the right arg
    if (hGlbRht)
        GetNextValueMem (lpMemRht,
                         aplTypeRht,
                         aplNELMRht - 1,
                        &hGlbSubRht,
                        &aplLongestRht,
                        &immTypeRht);
    else
        immTypeRht = TranslateArrayTypeToImmType (aplTypeRht);

    // Assign the last item of the right arg to the name arg

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
////////lpHeader->Perm       = 0;               // Already zero from GHND
////////lpHeader->SysVar     = 0;               // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = 1;
        lpHeader->Rank       = 0;
#undef  lpHeader

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, 0);

        // Fill in the data
        *((LPAPLNESTED) lpMemRes) = CopySymGlbDir (MakePtrTypeGlb (hGlbSubRht));

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

        // If the name arg is not immediate, free it first
        if (!lptkNamArg->tkData.tkSym->stFlags.Imm)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbNam); lpMemNam = NULL;

            FreeResultGlobalVar (ClrPtrTypeDirAsGlb (hGlbNam));
        } // End IF

        // Mark as no longer immediate
        lptkNamArg->tkData.tkSym->stFlags.Imm = FALSE;
        lptkNamArg->tkData.tkSym->stFlags.ImmType = 0;

        // Save the new global memory handle in the STE
        lptkNamArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbRes);
    } else
    {
        // If the name arg is not immediate, free it first
        if (!lptkNamArg->tkData.tkSym->stFlags.Imm)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbNam); lpMemNam = NULL;

            FreeResultGlobalVar (ClrPtrTypeDirAsGlb (hGlbNam));
        } // End IF

        // Save the new immediate type & value
        lptkNamArg->tkData.tkSym->stFlags.Imm      = TRUE;
        lptkNamArg->tkData.tkSym->stFlags.ImmType  = immTypeRht;
        lptkNamArg->tkData.tkSym->stData.stLongest = aplLongestRht;
    } // End IF/ELSE

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
} // End ArrayIndexSetNamScalar_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexSetNoLst_EM
//
//  Array assignment with an empty list arg
//  The name arg scalar case has already been taken care of
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexSetNoLst_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL ArrayIndexSetNoLst_EM
    (LPTOKEN    lptkNamArg,         // Ptr to name arg token
     LPTOKEN    lptkRhtArg,         // Ptr to right ...
     LPTOKEN    lptkFunc)           // Ptr to function token

{
    APLNELM      aplNELMRht;        // Right arg NELM
    APLRANK      aplRankNam,        // Name arg rank
                 aplRankRht;        // Right ...
    HGLOBAL      hGlbNam = NULL,    // Name arg global memory handle
                 hGlbRht = NULL,    // Right arg ...
                 hGlbRes = NULL;    // Result    ...
    LPVOID       lpMemNam = NULL,   // Ptr to name arg global memory
                 lpMemRht = NULL,   // Ptr to right arg global memory
                 lpMemRes = NULL;   // Ptr to result   ...
    APLUINT      uRht;              // Loop counter
    LPPL_YYSTYPE lpYYRes1,          // Ptr to primary result
                 lpYYRes2;          // Ptr to secondary result

    // Get the attributes (Type, NELM, and Rank) of the name and right args
    AttrsOfToken (lptkNamArg,  NULL,  NULL,       &aplRankNam, NULL);
    AttrsOfToken (lptkRhtArg,  NULL, &aplNELMRht, &aplRankRht, NULL);

    // Get name and right arg global ptrs
    GetGlbPtrs_LOCK (lptkNamArg, &hGlbNam, &lpMemNam);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Singleton right args match all lists
    if (aplNELMRht NE 1)
    {
        // Check for RANK ERROR between the list and right args
        if (aplRankNam NE aplRankRht)
        {
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Skip over the header to the dimensions
        lpMemNam = VarArrayBaseToDim (lpMemNam);
        lpMemRht = VarArrayBaseToDim (lpMemRht);

        // Check for LENGTH ERROR between the list and right args
        for (uRht = 0; uRht < aplRankRht; uRht++)
        if (*((LPAPLDIM) lpMemNam)++ NE *((LPAPLDIM) lpMemRht)++)
        {
            ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End FOR/IF
    } // End IF

    // Replace all elements of the name arg with those of the right arg

    // Find the shape of the name arg
    lpYYRes1 = PrimFnMonRho_EM_YY (lptkFunc,                // Ptr to function token
                                   lptkNamArg,              // Ptr to right arg token
                                   NULL);                   // Ptr to axis token
    if (!lpYYRes1)
        goto ERROR_EXIT;

    // Reshape the right arg to the shape of the name arg
    lpYYRes2 = PrimFnDydRho_EM_YY (&lpYYRes1->tkToken,      // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    NULL);                  // Ptr to axis token
    // Free the first result
    FreeResult (&lpYYRes1->tkToken); YYFree (lpYYRes1); lpYYRes1 = NULL;

    if (!lpYYRes2)
        goto ERROR_EXIT;

    // Get the global memory handle
    hGlbRes = ClrPtrTypeDirAsGlb (lpYYRes2->tkToken.tkData.tkGlbData);

    // Free the second result
    YYFree (lpYYRes2); lpYYRes2 = NULL;
ERROR_EXIT:
    if (hGlbNam && lpMemNam)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbNam); lpMemNam = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return hGlbRes;
} // End ArrayIndexSetNoLst_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexSetSingLst_EM
//
//  Array assignment with a singleton list arg
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexSetSingLst_EM"
#else
#define APPEND_NAME
#endif

BOOL ArrayIndexSetSingLst_EM
    (LPTOKEN    lptkNamArg,         // Ptr to name arg token
     LPTOKEN    lptkLstArg,         // Ptr to list ...
     LPTOKEN    lptkRhtArg,         // Ptr to right ...
     HGLOBAL   *lphGlbRes,          // Ptr to result global memory handle
     LPTOKEN    lptkFunc)           // Ptr to function token

{
    HGLOBAL    hGlbNam = NULL,      // Name arg global memory handle
               hGlbLst = NULL,      // List ...
               hGlbSubLst = NULL,   // List arg item global memory handle
               hGlbRht = NULL,      // Right arg ...
               hGlbSubRht = NULL;   // Right arg item...
    LPVOID     lpMemNam = NULL,     // Ptr to name arg global memory
               lpMemLst = NULL,     // Ptr to list arg global memory
               lpMemSubLst = NULL,  // Ptr to list arg item global memory
               lpMemRht = NULL,     // Ptr to right arg global memory
               lpMemRes = NULL;     // Ptr to result   ...
    LPAPLDIM   lpMemDimRes;         // Ptr to result dimensions
    APLSTYPE   aplTypeNam,          // Name arg storage type
               aplTypeSubLst,       // List arg subitem ...
               aplTypeRht,          // Right ...
               aplTypeRes;          // Result    ...
    APLNELM    aplNELMNam,          // Name arg NELM
               aplNELMLst,          // List ...
               aplNELMSubLst,       // List arg subitem ...
               aplNELMRht;          // Right ...
    APLRANK    aplRankNam,          // Name arg rank
               aplRankSubLst,       // List arg subitem ...
               aplRankRht;          // Right ...
    APLLONGEST aplLongestSubLst,    // List arg subitem immediate value
               aplLongestRht;       // Right arg item   ...
    IMM_TYPES  immTypeRht;          // Right ...
    APLUINT    uRht,                // Loop counter
               uRes;                // Loop counter
    UINT       uBitMask,            // Bit mask for when looping through Booleans
               bRet = TRUE;         // TRUE iff result is valid
    BOOL       bQuadIO;             // []IO

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get the attributes (Type, NELM, and Rank) of the name, list, and right args
    AttrsOfToken (lptkNamArg, &aplTypeNam, &aplNELMNam, &aplRankNam, NULL);
    AttrsOfToken (lptkLstArg,  NULL,       &aplNELMLst,  NULL,       NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get name, list, and right arg global ptrs
                    GetGlbPtrs_LOCK (lptkNamArg, &hGlbNam, &lpMemNam);
                    GetGlbPtrs_LOCK (lptkLstArg, &hGlbLst, &lpMemLst);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Skip over the header and dimensions to the data
    lpMemLst = VarArrayBaseToData (lpMemLst, 1);

    // Assume no result
    *lphGlbRes = NULL;

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
            hGlbSubLst = ClrPtrTypeIndAsGlb (lpMemLst);
            AttrsOfGlb (hGlbSubLst, &aplTypeSubLst, &aplNELMSubLst, &aplRankSubLst, NULL);
            lpMemSubLst = MyGlobalLock (hGlbSubLst);

            break;

        defstop
            break;
    } // End SWITCH

    // Check for RANK ERROR between the list and right args
    if (aplNELMRht NE 1
     && aplRankSubLst NE aplRankRht
     && aplNELMSubLst NE aplNELMRht)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Skip over the header to the dimensions
    lpMemSubLst = VarArrayBaseToDim (lpMemSubLst);

    // Check for LENGTH ERROR between the list and right args
    if (aplNELMRht NE 1)
    {
        APLRANK aplRankMax;

        // Use the larger rank
        aplRankMax = max (aplRankSubLst, aplRankRht);

        // Skip over the header to the dimensions
        lpMemRht = VarArrayBaseToDim (lpMemRht);

        for (uRht = 0; uRht < aplRankMax; uRht++)
        {
            APLDIM aplDimSubLst,        // Temporary dimension value
                   aplDimRht;           // ...

            // Get the respective dimensions
            if (uRht < aplRankSubLst)
                aplDimSubLst = *((LPAPLDIM) lpMemSubLst)++;
            else
                aplDimSubLst = 1;
            if (uRht < aplRankRht)
                aplDimRht    = *((LPAPLDIM) lpMemRht)++;
            else
                aplDimRht    = 1;

            // Skip over equal or unit dimensions
            if (aplDimSubLst      EQ aplDimRht
             || aplDimSubLst EQ 1 && aplDimRht NE 1
             || aplDimSubLst NE 1 && aplDimRht EQ 1)
                continue;

            // If the ranks differ, it's a RANK ERROR
            //   otherwise, it's a LENGTH ERROR
            ErrorMessageIndirectToken ((aplRankSubLst NE aplRankRht) ? ERRMSG_RANK_ERROR   APPEND_NAME
                                                                     : ERRMSG_LENGTH_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End FOR
    } else
    {
        // Skip over the dimensions to the data
        lpMemSubLst = VarArrayDimToData (lpMemSubLst, aplRankSubLst);

        if (hGlbRht)
            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
    } // End IF/ELSE

    //***************************************************************
    // From here on, lpMemSubLst points to the data, and if the
    //   right arg is not immediate, lpMemRht points to its data
    //***************************************************************

    // Handle obvious DOMAIN ERRORs
    if (aplTypeSubLst EQ ARRAY_HETERO
     || (IsSimpleChar (aplTypeSubLst)
      && aplNELMSubLst NE 0))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Calculate the storage type of the result
    aplTypeRes = aTypePromote[aplTypeNam][aplTypeRht];

    //***************************************************************
    // Split off cases
    //***************************************************************

    //***************************************************************
    // If the list arg item is immediate, do rectangular indexing.
    // The name arg must be a vector.
    //***************************************************************
    if (hGlbSubLst EQ NULL)
    {
        // Convert to origin-0
        aplLongestSubLst -= bQuadIO;

        // Skip over the header to the dimensions
        lpMemNam = VarArrayBaseToDim (lpMemNam);

        // Check for INDEX ERROR (skipping over the single dimension)
        if (aplLongestSubLst >= *((LPAPLDIM) lpMemNam)++)
        {
            ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // If the types are different, we need to type promote the name arg
        if (aplTypeRes NE aplTypeNam)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbNam); lpMemNam = NULL;

            if (!TypePromote_EM (lptkNamArg, aplTypeRes, lptkFunc))
                goto ERROR_EXIT;

            // Get name arg's global ptrs
            GetGlbPtrs_LOCK (lptkNamArg, &hGlbNam, &lpMemNam);

            // Skip over the header and dimensions to the data
            lpMemNam = VarArrayBaseToData (lpMemNam, aplRankNam);
        } // End IF

        // Get the one (and only) item from the right arg
        GetFirstItemToken (lptkRhtArg,      // Ptr to the token
                          &aplLongestRht,   // Ptr to the longest (may be NULL)
                          &immTypeRht,      // ...        immediate type (see IMM_TYPES) (may be NULL)
                          &hGlbSubRht);     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
        // Split cases based upon the common storage type
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:
                uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) aplLongestSubLst);
                if (aplLongestRht)
                    ((LPAPLBOOL)   lpMemNam)[aplLongestSubLst >> LOG2NBIB] |= uBitMask;
                else
                    ((LPAPLBOOL)   lpMemNam)[aplLongestSubLst >> LOG2NBIB] &= ~uBitMask;
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
                // Free the old entry
                // Split cases based upon the ptr type
                switch (GetPtrTypeDir (((LPAPLHETERO) lpMemNam)[aplLongestSubLst]))
                {
                    case PTRTYPE_STCONST:
                        break;

                    case PTRTYPE_HGLOBAL:
                        FreeResultGlobalVar (ClrPtrTypeDirAsGlb (((LPAPLNESTED) lpMemNam)[aplLongestSubLst]));
                        ((LPAPLNESTED) lpMemNam)[aplLongestSubLst] = NULL;

                        break;

                    defstop
                        break;
                } // End SWITCH

                if (hGlbSubRht)
                    ((LPAPLHETERO) lpMemNam)[aplLongestSubLst] = CopySymGlbDir (hGlbSubRht);
                else
                    ((LPAPLHETERO) lpMemNam)[aplLongestSubLst] = MakeSymEntry_EM (immTypeRht, &aplLongestRht, lptkFunc);
                break;

            defstop
                break;
        } // End SWITCH

        // We no longer need this ptr
        MyGlobalUnlock (hGlbNam); lpMemNam = NULL;

        // See if it fits into a lower (but not necessarily smaller) datatype
        TypeDemote (lptkNamArg);

        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // From here on the singleton list arg is a global.
    //***************************************************************

    // We no longer need this ptr
    MyGlobalUnlock (hGlbNam); lpMemNam = NULL;

    // Because this operation is supposed to be atomic,
    //   we need to copy the entire array first, change
    //   the copied array (blowing it up if necessary),
    //   delete the old array, and change the STE to
    //   point to the new array.
    *lphGlbRes = CopyArray_EM (hGlbNam, lptkNamArg);
    if (!*lphGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // If the types are different, we need to type promote the name arg
    if (aplTypeRes NE aplTypeNam)
    {
        if (!TypePromoteGlb_EM (lphGlbRes, aplTypeRes, lptkFunc))
            goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (*lphGlbRes);

    // Skip over the header to the dimensions
    lpMemDimRes = VarArrayBaseToDim (lpMemRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankNam);

    //***************************************************************
    // If the singleton list arg item is simple, do rectangular indexing.
    // The name arg must be a vector.
    //***************************************************************
    if (IsSimple (aplTypeSubLst))
    {
        if (aplNELMRht EQ 1)
            // Get the one (and only) item from the right arg
            GetFirstItemToken (lptkRhtArg,      // Ptr to the token
                              &aplLongestRht,   // Ptr to the longest (may be NULL)
                              &immTypeRht,      // ...        immediate type (see IMM_TYPES) (may be NULL)
                              &hGlbSubRht);     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
        // Loop through the list indices and the right arg
        //   changing the result as appropriate
        for (uRes = 0; uRes < aplNELMSubLst; uRes++)
        {
            // Get the next item from the right arg
            if (aplNELMRht NE 1)
                GetNextItemMem (lpMemRht,
                                aplTypeRht,
                                uRes,
                               &hGlbSubRht,
                               &aplLongestRht);
            // Index assignment into the top level of a vector
            bRet = ArrayIndexSetVector_EM (aplNELMNam,      // Name arg NELM
                                           lpMemSubLst,     // Ptr to list arg subitem
                                           aplTypeSubLst,   // List arg subitem storage type
                                           aplTypeRes,      // Result storage type
                                           lpMemRes,        // Ptr to result global memory
                                           uRes,            // Index into the list arg subitem
                                           hGlbSubRht,      // Right arg item global memory handle
                                           aplLongestRht,   // Right arg immediate value
                                           bQuadIO,         // []IO
                                           lptkFunc);       // Ptr to function token
            if (!bRet)
                goto ERROR_EXIT;
        } // End FOR
    } else
    {
        APLSTYPE aplTypeSet;

        //***************************************************************
        // From here on the singleton list arg is nested
        //***************************************************************

        //***************************************************************
        // If the list arg item is a nested array of equal length simple
        //   numeric vectors, do scatter indexing, in which case
        //   the name arg rank must be the same as the length of the
        //   list arg item sub items.
        //
        // If the list item arg is a nested array of nested vectors,
        //   do reach indexing, in which case
        //   the name arg rank must be the same as the length of the
        //   first element of each of the list arg sub items.
        //***************************************************************

        // If the right arg is an APA, set the set type to INT
        if (aplTypeRht EQ ARRAY_APA)
            aplTypeSet = ARRAY_INT;
        else
            aplTypeSet = aplTypeRht;

        if (aplNELMRht EQ 1)
            // Get the one (and only) item from the right arg
            GetFirstItemToken (lptkRhtArg,      // Ptr to the token
                              &aplLongestRht,   // Ptr to the longest (may be NULL)
                              &immTypeRht,      // ...        immediate type (see IMM_TYPES) (may be NULL)
                              &hGlbSubRht);     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
        // Loop through the list indices and the right arg
        //   changing the result as appropriate
        for (uRes = 0; uRes < aplNELMSubLst; uRes++)
        {
            // Get the next value from the right arg
            if (aplNELMRht NE 1)
                GetNextItemMem (lpMemRht,
                                aplTypeRht,
                                uRes,
                               &hGlbSubRht,
                               &aplLongestRht);
            // Split cases based upon the list arg subitem ptr type
            switch (GetPtrTypeDir (((LPAPLHETERO) lpMemSubLst)[uRes]))
            {
                LPPL_YYSTYPE lpYYItm;
                HGLOBAL      hGlbSubLst2;
                APLSTYPE     aplTypeSubLst2;

                case PTRTYPE_STCONST:
                    // Check for RANK ERROR
                    if (aplRankNam NE 1)
                    {
                        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                                   lptkFunc);
                        goto ERROR_EXIT;
                    } // End IF

                    // Index assignment into a the top level of a vector
                    bRet = ArrayIndexSetVector_EM (aplNELMNam,      // Name arg NELM
                                                   lpMemSubLst,     // Ptr to list arg subitem
                                                   aplTypeSubLst,   // List arg subitem storage type
                                                   aplTypeRes,      // Result storage type
                                                   lpMemRes,        // Ptr to result global memory
                                                   uRes,            // Index into the list arg subitem
                                                   hGlbSubRht,      // Right arg item global memory handle
                                                   aplLongestRht,   // Right arg immediate value
                                                   bQuadIO,         // []IO
                                                   lptkFunc);       // Ptr to function token
                    if (!bRet)
                        goto ERROR_EXIT;
                    break;

                case PTRTYPE_HGLOBAL:
                    // Get the global memory handle
                    hGlbSubLst2 = ((LPAPLHETERO) lpMemSubLst)[uRes];
                    hGlbSubLst2 = ClrPtrTypeDirAsGlb (hGlbSubLst2);

                    AttrsOfGlb (hGlbSubLst2, &aplTypeSubLst2, NULL, NULL, NULL);

                    // Because of the way RightShoe works, if this global is simple,
                    //   we need to enclose it first
                    if (IsSimple (aplTypeSubLst2))
                    {
                        lpYYItm =
                          PrimFnMonLeftShoeGlb_EM_YY (hGlbSubLst2,          // Right arg global memory handle
                                                      NULL,                 // Ptr to axis token (may be NULL)
                                                      lptkFunc);            // Ptr to function token
                        // Save just the global memory handle (which we need to free later)
                        hGlbSubLst2 = lpYYItm->tkToken.tkData.tkGlbData;
                        hGlbSubLst2 = ClrPtrTypeDirAsGlb (hGlbSubLst2);

                        YYFree (lpYYItm); lpYYItm = NULL;
                    } // End IF

                    // Set the corresponding element of the result
                    // Note the args get switched between indexing and pick
                    lpYYItm =
                      PrimFnDydRightShoeGlbGlb_EM_YY (hGlbSubLst2,      // Left arg global memory handle
                                                     *lphGlbRes,        // Right arg global memory handle
                                                      lptkFunc,         // Ptr to function token
                                                      TRUE,             // TRUE iff array assignment
                                                      aplTypeSet,       // Set arg storage type
                                                      hGlbSubRht,       // Set arg global memory handle/LPSYMENTRY (NULL if immediate)
                                                      aplLongestRht);   // Set arg immediate value
                    // If we allocated it above, free it now
                    if (IsSimple (aplTypeSubLst2))
                    {
                        // We no longer need this storage
                        FreeResultGlobalVar (hGlbSubLst2); hGlbSubLst2 = NULL;
                    } // End IF

                    if (!lpYYItm)
                        goto ERROR_EXIT;
                    break;

                defstop
                    break;
            } // End SWITCH
        } // End FOR
    } // End IF/ELSE/...

    goto NORMAL_EXIT;

ERROR_EXIT:
    bRet = FALSE;

    if (*lphGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (*lphGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;
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
} // End ArrayIndexSetSingLst_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexSetVector_EM
//
//  Array assignment into a vector
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexSetVector_EM"
#else
#define APPEND_NAME
#endif

BOOL ArrayIndexSetVector_EM
    (APLNELM    aplNELMNam,             // Name arg NELM
     LPVOID     lpMemSubLst,            // Ptr to list arg subitem
     APLSTYPE   aplTypeSubLst,          // List arg subitem storage type
     APLSTYPE   aplTypeRes,             // Result storage type
     LPVOID     lpMemRes,               // Ptr to result global memory
     APLUINT    uRes,                   // Index into the list arg subitem
     HGLOBAL    hGlbSubRht,             // Right arg item global memory handle
     APLLONGEST aplLongestRht,          // Right arg immediate value
     BOOL       bQuadIO,                // []IO
     LPTOKEN    lptkFunc)               // Ptr to function token

{
    BOOL       bRet = TRUE;             // TRUE iff the result is valid
    APLLONGEST aplLongestSubLst;        // List arg subitem immediate value
    HGLOBAL    hGlbSubLst;              // Ptr to list arg subitem global memory handle

    // Get next index from global memory
    GetNextItemMem (lpMemSubLst,        // Ptr to item global memory data
                    aplTypeSubLst,      // Item storage type
                    uRes,               // Index into item
                   &hGlbSubLst,         // Ptr to result global memory handle (may be NULL)
                   &aplLongestSubLst);  // Ptr to result immediate value (may be NULL)
    // Check for DOMAIN ERROR
    // Split cases based upon the list arg subitem storage type
    switch (aplTypeSubLst)
    {
        case ARRAY_BOOL:
            aplLongestSubLst &= BIT0;

            break;

        case ARRAY_INT:
            break;

        case ARRAY_FLOAT:
            // Attempt to convert the float to an integer using System CT
            aplLongestSubLst = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestSubLst, &bRet);
            if (bRet)
                break;

            // Fall through to common DOMAIN ERROR code

        case ARRAY_CHAR:
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;

        defstop
            break;
    } // End SWITCH

    // Convert to origin-0
    aplLongestSubLst -= bQuadIO;

    // Check for INDEX ERROR
    if (aplLongestSubLst >= aplNELMNam)
    {
        ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        UINT uBitMask;          // Mask for working with Booleans

        case ARRAY_BOOL:
            // Replace the corresponding item in the result
            uBitMask = (BIT0 << (MASKLOG2NBIB & (UINT) aplLongestSubLst));
            if (aplLongestRht)
                ((LPAPLBOOL)    lpMemRes)[aplLongestSubLst >> LOG2NBIB] |= uBitMask;
            else
                ((LPAPLBOOL)    lpMemRes)[aplLongestSubLst >> LOG2NBIB] &= ~uBitMask;
            break;

        case ARRAY_INT:
            // Replace the corresponding item in the result
            ((LPAPLINT)    lpMemRes)[aplLongestSubLst] = (APLINT) aplLongestRht;

            break;

        case ARRAY_FLOAT:
            // Replace the corresponding item in the result
            ((LPAPLFLOAT)  lpMemRes)[aplLongestSubLst] = *(LPAPLFLOAT) &aplLongestRht;

            break;

        case ARRAY_CHAR:
            // Replace the corresponding item in the result
            ((LPAPLCHAR)   lpMemRes)[aplLongestSubLst] = (APLCHAR) aplLongestRht;

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Free the old value
            // Split cases based upon the ptr type
            switch (GetPtrTypeDir (((LPAPLNESTED) lpMemRes)[aplLongestSubLst]))
            {
                case PTRTYPE_STCONST:
                    break;

                case PTRTYPE_HGLOBAL:
                    FreeResultGlobalVar (ClrPtrTypeDirAsGlb (((LPAPLNESTED) lpMemRes)[aplLongestSubLst]));
                    ((LPAPLNESTED) lpMemRes)[aplLongestSubLst] = NULL;

                    break;

                defstop
                    break;
            } // End SWITCH

            // Replace the corresponding item in the result
            ((LPAPLNESTED) lpMemRes)[aplLongestSubLst] = CopySymGlbDir (hGlbSubRht);

            break;

        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

ERROR_EXIT:
    bRet = FALSE;
NORMAL_EXIT:
    return bRet;
} // End ArrayIndexSetVector_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexSetRect
//
//  Array assignment with a multiple list arg
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ArrayIndexSetRect_EM"
#else
#define APPEND_NAME
#endif

BOOL ArrayIndexSetRect_EM
    (LPTOKEN    lptkNamArg,             // Ptr to name arg token
     LPTOKEN    lptkLstArg,             // Ptr to list ...
     LPTOKEN    lptkRhtArg,             // Ptr to right ...
     HGLOBAL   *lphGlbRes,              // Ptr to result global memory handle
     LPTOKEN    lptkFunc)               // Ptr to function token

{
    APLNELM      aplNELMLst;            // List arg NELM
    APLRANK      aplRankNam;            // Name arg rank
    HGLOBAL      hGlbNam = NULL,        // Name arg gobal memory handle
                 hGlbLst = NULL;        // List ...
    LPAPLNESTED  lpMemLst = NULL;       // Ptr to list ...
    APLUINT      uLst,                  // Loop counter
                 uCount,                // Count of non-elided indices
                 ByteRes;               // # bytes in the result
    HGLOBAL      hGlbLstNew = NULL,     // New list arg global memory handle
                 hGlbAxis = NULL;       // Axis operator global memory handle
    LPAPLNESTED  lpMemLstNew = NULL;    // Ptr to new list arg global memory
    LPAPLUINT    lpMemAxis = NULL;      // Ptr to axis operator global memory
    BOOL         bRet = FALSE,          // TRUE iff result is valid
                 bQuadIO;               // []IO
    LPPL_YYSTYPE lpYYRes = NULL;        // Ptr to the result
    TOKEN        tkLstArg = {0},        // New list arg token
                 tkAxis = {0};          // Axis token

    // Get the attributes (Type, NELM, and Rank) of the name, list, and right args
    AttrsOfToken (lptkNamArg, NULL, NULL,        &aplRankNam, NULL);
    AttrsOfToken (lptkLstArg, NULL, &aplNELMLst, NULL,        NULL);

    // Get name and list arg's global ptrs
    GetGlbPtrs_LOCK (lptkNamArg, &hGlbNam, NULL);
    GetGlbPtrs_LOCK (lptkLstArg, &hGlbLst, &(LPVOID) lpMemLst);

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
        lpYYRes =
          PrimFnDydSquadGlb_EM_YY (lptkLstArg,      // Ptr to Left arg token
                                  &hGlbNam,         // Ptr to right arg global memory handle
                                   NULL,            // Ptr to axis token (may be NULL)
                                   lptkFunc,        // Ptr to function token
                                   lptkRhtArg);     // Ptr to set arg token
        if (lpYYRes)
            lptkNamArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbNam);
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
            *lpMemLstNew = MakePtrTypeGlb (hGlbZilde);
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
        tkLstArg.tkData.tkGlbData  = MakePtrTypeGlb (hGlbLstNew);
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
        tkAxis.tkData.tkGlbData  = MakePtrTypeGlb (hGlbAxis);
        tkAxis.tkCharIndex       = lptkFunc->tkCharIndex;

        // Note the args get switched between indexing and squad
        lpYYRes =
          PrimFnDydSquadGlb_EM_YY (&tkLstArg,       // Ptr to left arg token
                                   &hGlbNam,        // Right arg global memory handle
                                   &tkAxis,         // Ptr to axis token (may be NULL)
                                    lptkFunc,       // Ptr to function token
                                    lptkRhtArg);    // Ptr to set arg token
        if (lpYYRes)
            lptkNamArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbNam);
    } // End IF/ELSE

    // The result is dependent upon the state of lpYYRes
    //   (it's either NULL or PTR_REUSED)
    bRet = (lpYYRes NE NULL);
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

    if (hGlbLst && lpMemLst)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLst); lpMemLst = NULL;
    } // End IF

    return bRet;
} // End ArrayIndexSetRect_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ArrayIndexReplace
//
//  Replace a value in an array
//***************************************************************************

void ArrayIndexReplace
    (APLSTYPE   aplTypeRht,             // Right arg storage type
     LPVOID     lpMemRht,               // Ptr to right arg global memory
     APLUINT    aplIndex,               // Index into right arg
     APLSTYPE   aplTypeSet,             // Set arg storage type
     APLLONGEST aplLongestSet,          // Set arg immediate value
     HGLOBAL    hGlbSet,                // Set arg global memory handle
     LPTOKEN    lptkFunc)               // Ptr to function token

{
    HGLOBAL hGlbSubRht;
    UINT    uBitMask;

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) aplIndex);

            if (aplLongestSet & BIT0)
                ((LPAPLBOOL) lpMemRht)[aplIndex >> LOG2NBIB] |= uBitMask;
            else
                ((LPAPLBOOL) lpMemRht)[aplIndex >> LOG2NBIB] &= ~uBitMask;
            break;

        case ARRAY_INT:
            ((LPAPLINT) lpMemRht)[aplIndex] = (APLINT) aplLongestSet;

            break;

        case ARRAY_FLOAT:
            ((LPAPLFLOAT) lpMemRht)[aplIndex] = *(LPAPLFLOAT) &aplLongestSet;

            break;

        case ARRAY_CHAR:
            ((LPAPLCHAR) lpMemRht)[aplIndex] = (APLCHAR) aplLongestSet;

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Get the previous value
            hGlbSubRht = ((LPAPLNESTED) lpMemRht)[aplIndex];

            // If the set arg is simple, ...
            if (hGlbSet EQ NULL)
                ((LPAPLNESTED) lpMemRht)[aplIndex] =
                  MakeSymEntry_EM (TranslateArrayTypeToImmType (aplTypeSet),
                                  &aplLongestSet,
                                   lptkFunc);
            else
                ((LPAPLNESTED) lpMemRht)[aplIndex] =
                  CopySymGlbDir (hGlbSet);

            // Free the old value if it's a global
            if (GetPtrTypeDir (hGlbSubRht) EQ PTRTYPE_HGLOBAL)
            {
                FreeResultGlobalVar (ClrPtrTypeDirAsGlb (hGlbSubRht)); hGlbSubRht = NULL;
            } // End IF

            break;

        defstop
            break;
    } // End SWITCH
} // End ArrayIndexReplace


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
