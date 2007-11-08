//***************************************************************************
//  NARS2000 -- Type Demote/Promote Functions
//***************************************************************************

#define STRICT
#include <windows.h>
#include <float.h>
#include <math.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $TypeDemote
//
//  Attempt to demote the type of a global memory handle
//
//  Various primitives (e.g. Match) rely upon type demotion to
//  return a correct result.
//
//  Consequently, it must not be possible to store an all simple
//  homogeneous array in nested array format.  In other words,
//  structural functions such as
//
//    partitioned enclose
//    bracket indexed assignment
//    etc.
//
//  The following functions have been changed to use TypeDemote:
//    primitive scalar dyadic functions, simple vs. simple
//    enclose with axis
//    reshape
//    collapsing dyadic transpose
//    enlist
//    compression/replicate
//    each
//    first
//    outer product
//    reduction w/ or w/o axis
//    N-wise reduction
//    monadic & dyadic each
//    squad
//    scan
//    disclose
//    user-defined function/operator with multiple result names
//    pick
//    take
//    drop
//    bracket indexing
//    without
//
//  must call this function to check their result to see if it
//  can be stored more simply.  Note that more simply does not
//  mean in fewer bytes as ptrs are four bytes while integers are eight.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- TypeDemote"
#else
#define APPEND_NAME
#endif

LPTOKEN TypeDemote
    (LPTOKEN lptkRhtArg)

{
    HGLOBAL           hGlbRht = NULL,   // Right arg global memory handle
                      hGlbRes = NULL;   // Result    ...
    LPVOID            lpMemRht = NULL,  // Ptr to right arg global memory
                      lpMemRes = NULL;  // Ptr to result    ...
    APLNELM           aplNELMRht,       // Right arg NELM
                      aplNELMNest;      // Right arg NELM in case empty nested
    APLRANK           aplRankRht;       // Right arg rank
    APLUINT           uRht,             // Right arg loop counter
                      ByteRes;          // # bytes in the result
    APLINT            aplInteger;       // Temporary integer
    APLFLOAT          aplFloat;         // ...       float
    APLSTYPE          aplTypeRes,
                      aplTypeRht,
                      aplTypeSub;
    LPSYMENTRY        lpSymEntry;
    LPVARARRAY_HEADER lpMemRhtHdr;
    DWORD             dwSize;

    // Note that neither ARRAY_APA not ARRAY_LIST can occur
    //   as an argument to this function.
    static APLSTYPE aplTypeArr[ARRAY_LENGTH][ARRAY_LENGTH] =
    //      BOOL          INT           FLOAT         CHAR        HETERO        NESTED        INIT
    {{ARRAY_BOOL  , ARRAY_INT   , ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_BOOL  },  // BOOL
     {ARRAY_INT   , ARRAY_INT   , ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_INT   },  // INT
     {ARRAY_FLOAT , ARRAY_FLOAT , ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_FLOAT },  // FLOAT
     {ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_CHAR  , ARRAY_HETERO, ARRAY_NESTED, ARRAY_CHAR  },  // CHAR
     {ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_HETERO},  // HETERO
     {ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED},  // NESTED
     {ARRAY_BOOL  , ARRAY_INT   , ARRAY_FLOAT , ARRAY_CHAR  , ARRAY_HETERO, ARRAY_NESTED, ARRAY_LIST  },  // INIT
    };

    // Split cases based upon the arg token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARIMMED:
            return lptkRhtArg;

        case TKT_VARARRAY:
            hGlbRht = ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkGlbData);

            break;

        defstop
            break;
    } // End SWITCH

    // Copy to the result in case we don't demote
    hGlbRes = hGlbRht;

    // Lock the memory to get a ptr to it
    lpMemRht = lpMemRhtHdr = MyGlobalLock (hGlbRht);

    // Get the Type, NELM, and Rank
    aplTypeRht = lpMemRhtHdr->ArrType;
    aplNELMRht = lpMemRhtHdr->NELM;
    aplRankRht = lpMemRhtHdr->Rank;

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRhtHdr, aplRankRht);

    // If the right arg is a simple scalar,
    //   convert it to an immediate
    if (IsSimple (aplTypeRht)
     && aplRankRht EQ 0)
    {
        // Fill in the result token
        lptkRhtArg->tkFlags.TknType  = TKT_VARIMMED;
        lptkRhtArg->tkFlags.ImmType  = TranslateArrayTypeToImmType (aplTypeRht);
        lptkRhtArg->tkData.tkLongest = *(LPAPLLONGEST) lpMemRht;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = lpMemRhtHdr = NULL;

        // We no longer need this storage
        FreeResultGlobalVar (hGlbRht); hGlbRht = NULL;

        goto IMMED_EXIT;
    } // End IF

    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:        // No place to go
        case ARRAY_APA:         // No place to go
        case ARRAY_CHAR:        // No place to go
            goto IMMED_EXIT;

        case ARRAY_INT:         // Demote to Boolean
            // Start with lowest type
            aplTypeRes = ARRAY_BOOL;

            // Loop through the elements
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                aplInteger = *((LPAPLINT) lpMemRht)++;

                // Check for Boolean value
                if (!IsBooleanValue (aplInteger))
                {
                    aplTypeRes = ARRAY_INT;

                    break;
                } // End IF
            } // End FOR

            break;

        case ARRAY_FLOAT:       // Demote to Boolean/Integer
            // Start with lowest type
            aplTypeRes = ARRAY_BOOL;

            // Loop through the elements
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                aplFloat = *((LPAPLFLOAT) lpMemRht)++;

                // Check for Boolean value
                if (IsBooleanValue (aplFloat))
                    aplTypeSub = ARRAY_BOOL;
                else
                // Check for Integer value
                if (aplFloat EQ floor (aplFloat)
                 && fabs (aplFloat) < Float2Pow53)
                    aplTypeSub = ARRAY_INT;
                else
                    aplTypeSub = ARRAY_FLOAT;

                // Check storage type
                aplTypeRes = aplTypeArr[aplTypeRes][aplTypeSub];

                // If we're back to the same type, quit
                if (aplTypeRes EQ aplTypeRht)
                    break;
            } // End FOR

            break;

        case ARRAY_HETERO:      // Demote to simple homogeneous
        case ARRAY_NESTED:      // Demote to simple homogeneous/heterogeneous
            // Initialize the type (as ARRAY_LIST never occurs
            //   in this context, we co-opt it).
            aplTypeRes = ARRAY_LIST;

            // Take into account nested prototypes
            if (aplTypeRht EQ ARRAY_NESTED)
                aplNELMNest = max (aplNELMRht, 1);
            else
                aplNELMNest = aplNELMRht;

            // Loop through the elements
            for (uRht = 0; uRht < aplNELMNest; uRht++, ((LPAPLHETERO) lpMemRht)++)
            {
                // Split cases based upon the ptr type of the element
                switch (GetPtrTypeInd (lpMemRht))
                {
                    case PTRTYPE_STCONST:
                        lpSymEntry = *(LPAPLHETERO) lpMemRht;

                        // stData is an immediate
                        Assert (lpSymEntry->stFlags.Imm);

                        // Get the immediate type (translated to an ARRAY_TYPE)
                        aplTypeSub = TranslateImmTypeToArrayType (lpSymEntry->stFlags.ImmType);

                        // Check for various possible type demotions
                        switch (aplTypeSub)
                        {
                            case ARRAY_FLOAT:   // Check for demotion from FLOAT to INT/BOOL
                                aplFloat = lpSymEntry->stData.stFloat;

                                if (IsBooleanValue (aplFloat))
                                    aplTypeSub = ARRAY_BOOL;
                                else
                                if (aplFloat EQ floor (aplFloat)
                                 && fabs (aplFloat) < Float2Pow53)
                                    aplTypeSub = ARRAY_INT;
                                break;

                            case ARRAY_INT:     // Check for demotion from INT to BOOL
                                aplInteger = lpSymEntry->stData.stInteger;

                                if (IsBooleanValue (aplInteger))
                                    aplTypeSub = ARRAY_BOOL;
                                break;

                            case ARRAY_BOOL:    // No type demotion
                            case ARRAY_CHAR:    // No type demotion
                            default:
                                break;
                        } // End SWITCH

                        // Check storage type
                        aplTypeRes = aplTypeArr[aplTypeRes][aplTypeSub];

                        // Check for no demotion
                        if (aplTypeRes EQ ARRAY_HETERO
                         && aplTypeRht EQ ARRAY_HETERO)
                            goto NORMAL_EXIT;
                        break;

                    case PTRTYPE_HGLOBAL:
                        // It's nested, so there's no demotion
                        goto NORMAL_EXIT;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    // If the result type is the same as the right arg type,
    //   there's no demotion
    if (aplTypeRes EQ aplTypeRht)
        goto NORMAL_EXIT;

    // Check for demotion from Nested to Hetero
    if (aplTypeRes EQ ARRAY_HETERO
     && aplTypeRht EQ ARRAY_NESTED)
    {
        // If the reference count of this array is one, just
        //   change the array type from ARRAY_NESTED to ARRAY_HETERO.
        if (lpMemRhtHdr->RefCnt EQ 1)
            lpMemRhtHdr->ArrType = ARRAY_HETERO;
        else
        {
            // Copy this array and change the type from ARRAY_NESTED to ARRAY_HETERO
            dwSize = MyGlobalSize (hGlbRht);
            hGlbRes = DbgGlobalAlloc (GHND, dwSize);
            if (hGlbRes)
            {
                // Lock the memory to get a ptr to it
                lpMemRes = MyGlobalLock (hGlbRes);

                // Copy source to destin
                CopyMemory (lpMemRes, lpMemRht, dwSize);

#ifdef DEBUG_REFCNT
                dprintfW (L"##RefCnt=1 in " APPEND_NAME L": %08X (%S#%d)", lpMemRes, FNLN);
#endif
#define lpHeaderRes ((LPVARARRAY_HEADER) lpMemRes)
                // Set the reference count and array type
                lpHeaderRes->RefCnt  = 1;
                lpHeaderRes->ArrType = ARRAY_HETERO;
#undef  lpHeaderRes
                // We no longer need these ptrs
                MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
                MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

                // Free the old array
                FreeResultGlobalVar (hGlbRht); hGlbRht = NULL;
            } else
                // WS FULL, so no demotion
                hGlbRes = hGlbRht;
        } // End IF/ELSE
    } else
    // Check for demotion to simple homogeneous
    if (uTypeMap[aplTypeRes] < min (uTypeMap[ARRAY_HETERO], uTypeMap[aplTypeRht]))
    {
        // Handle simple scalars as immediates
        if (aplRankRht EQ 0
         && IsSimple (aplTypeRes))
        {
            lptkRhtArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkRhtArg->tkFlags.ImmType  = TranslateArrayTypeToImmType (aplTypeRes);
            lptkRhtArg->tkData.tkLongest = 0;

            // Skip over header and dimensions to the data
            lpMemRht = VarArrayBaseToData (lpMemRhtHdr, aplRankRht);

            // Demote the data in the right arg, copying it to the result
            DemoteData (aplTypeRes,
                       &lptkRhtArg->tkData.tkLongest,
                        aplTypeRht,
                        aplNELMRht,
                        lpMemRht);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

            // Free the old array
            FreeResultGlobalVar (hGlbRht); hGlbRht = NULL;

            goto IMMED_EXIT;
        } // End IF

        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

        // Allocate space for a new array
        Assert (ByteRes EQ (UINT) ByteRes);
        hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (hGlbRes)
        {
            // Lock the memory to get a ptr to it
            lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
            // Fill in the header
            lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = aplTypeRes;
////////////lpHeader->Perm       = 0;               // Already zero from GHND
////////////lpHeader->SysVar     = 0;               // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELMRht;
            lpHeader->Rank       = aplRankRht;
#undef  lpHeader

            // Skip over the header to the dimensions
            lpMemRht = VarArrayBaseToDim (lpMemRhtHdr);
            lpMemRes = VarArrayBaseToDim (lpMemRes);

            // Copy the dimensions to the result
            for (uRht = 0; uRht < aplRankRht; uRht++)
                *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;

            // Now, lpMemRes and lpMemRht both point to their
            //   respective data

            // Demote the data in the right arg, copying it to the result
            DemoteData (aplTypeRes,
                        lpMemRes,
                        aplTypeRht,
                        aplNELMRht,
                        lpMemRht);
            // We no longer need these ptrs
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
            MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

            // Free the old array
            FreeResultGlobalVar (hGlbRht); hGlbRht = NULL;
        } else
            // WS FULL, so no demotion
            hGlbRes = hGlbRht;
    } // End IF/ELSE
NORMAL_EXIT:
    // Save the result in the right arg token
    lptkRhtArg->tkData.tkGlbData = MakeGlbTypeGlb (hGlbRes);
IMMED_EXIT:
    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lptkRhtArg;
} // End TypeDemote
#undef  APPEND_NAME


//***************************************************************************
//  $DemoteData
//
//  Demote the data in the right arg, copying it to the result
//***************************************************************************

void DemoteData
    (APLSTYPE aplTypeRes,           // Result storage type
     LPVOID   lpMemRes,             // Ptr to result global memory
     APLSTYPE aplTypeRht,           // Right arg storage type
     APLNELM  aplNELMRht,           // Right arg NELM
     LPVOID   lpMemRht)             // Ptr to right arg global memory

{
    APLUINT uRht;
    UINT    uBitIndex;

    // Split cases based upon the result's storage type
    // Note that the result is always of lower type than
    //   the right arg.
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:            // Res = BOOL, Rht = INT/FLOAT/HETERO/NESTED
            uBitIndex = 0;

            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_INT:     // Res = BOOL, Rht = INT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        *((LPAPLBOOL) lpMemRes) |= (*((LPAPLINT) lpMemRht)++) << uBitIndex;

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_FLOAT:   // Res = BOOL, Rht = FLOAT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        *((LPAPLBOOL) lpMemRes) |= ((APLBOOL) *((LPAPLFLOAT) lpMemRht)++) << uBitIndex;

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_HETERO:  // Res = BOOL, Rht = HETERO
                case ARRAY_NESTED:  // Res = BOOL, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Split cases based upon the immediate type
                        switch ((*((LPAPLHETERO) lpMemRht))->stFlags.ImmType)
                        {
                            case IMMTYPE_BOOL:
                                *((LPAPLBOOL) lpMemRes) |= ((*((LPAPLHETERO) lpMemRht)++)->stData.stBoolean) << uBitIndex;

                                break;

                            case IMMTYPE_INT:
                                *((LPAPLBOOL) lpMemRes) |= ((*((LPAPLHETERO) lpMemRht)++)->stData.stInteger) << uBitIndex;

                                break;

                            case IMMTYPE_FLOAT:
                                *((LPAPLBOOL) lpMemRes) |= ((APLINT) (*((LPAPLHETERO) lpMemRht)++)->stData.stFloat) << uBitIndex;

                                break;

                            case IMMTYPE_CHAR:
                            defstop
                                break;
                        } // End SWITCH

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_INT:         // Res = INT, Rht = FLOAT/HETERO/NESTED
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_FLOAT:   // Res = INT , Rht = FLOAT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                        *((LPAPLINT) lpMemRes)++ = (APLINT) *((LPAPLFLOAT) lpMemRht)++;
                    break;

                case ARRAY_HETERO:  // Res = INT , Rht = HETERO
                case ARRAY_NESTED:  // Res = INT , Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    // Split cases based upon the immediate type
                    switch ((*((LPAPLHETERO) lpMemRht))->stFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:
                            *((LPAPLINT) lpMemRes)++ = (*((LPAPLHETERO) lpMemRht)++)->stData.stBoolean;

                            break;

                        case IMMTYPE_INT:
                            *((LPAPLINT) lpMemRes)++ = (*((LPAPLHETERO) lpMemRht)++)->stData.stInteger;

                            break;

                        case IMMTYPE_FLOAT:
                            *((LPAPLINT) lpMemRes)++ = (APLINT) (*((LPAPLHETERO) lpMemRht)++)->stData.stFloat;

                            break;

                        case IMMTYPE_CHAR:
                        defstop
                            break;
                    } // End FOR/SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_FLOAT:       // Res = FLOAT, Rht = HETERO/NESTED
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_HETERO:  // Res = FLOAT, Rht = HETERO
                case ARRAY_NESTED:  // Res = FLOAT, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    // Split cases based upon the immediate type
                    switch ((*((LPAPLHETERO) lpMemRht))->stFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:
                            *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) (*((LPAPLHETERO) lpMemRht)++)->stData.stBoolean;

                            break;

                        case IMMTYPE_INT:
                            *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) (*((LPAPLHETERO) lpMemRht)++)->stData.stInteger;

                            break;

                        case IMMTYPE_FLOAT:
                            *((LPAPLFLOAT) lpMemRes)++ = (*((LPAPLHETERO) lpMemRht)++)->stData.stFloat;

                            break;

                        case IMMTYPE_CHAR:
                        defstop
                            break;
                    } // End FOR/SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_CHAR:        // Res = CHAR, Rht = HETERO/NESTED
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_HETERO:  // Res = CHAR, Rht = HETERO
                case ARRAY_NESTED:  // Res = CHAR, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                        *((LPAPLCHAR) lpMemRes)++ = (*((LPAPLHETERO) lpMemRht)++)->stData.stChar;
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH
} // End DemoteData


//***************************************************************************
//  End of File: typemote.c
//***************************************************************************
