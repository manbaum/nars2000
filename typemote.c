//***************************************************************************
//  NARS2000 -- Type Demote/Promote Functions
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

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
#include <float.h>
#include <math.h>
#include "headers.h"


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
//    bracket indexed assignment
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

void TypeDemote
    (LPTOKEN lptkRhtArg)

{
    HGLOBAL           hGlbRht = NULL,   // Right arg global memory handle
                      hGlbRes = NULL;   // Result    ...
    LPVOID            lpMemRht = NULL,  // Ptr to right arg global memory
                      lpMemRes = NULL;  // Ptr to result    ...
    APLNELM           aplNELMRht,       // Right arg NELM
                      aplNELMNstRht;    // Right arg NELM in case empty nested
    APLRANK           aplRankRht;       // Right arg rank
    APLUINT           uRht,             // Right arg loop counter
                      ByteRes;          // # bytes in the result
    APLINT            aplInteger;       // Temporary integer
    APLFLOAT          aplFloat;         // ...       float
    APLSTYPE          aplTypeRes,       // Result storage type
                      aplTypeRht,       // Right arg ...
                      aplTypeSub;       // Right arg item ...
    LPSYMENTRY        lpSymEntry;       // Ptr to Hetero item
    LPVARARRAY_HEADER lpMemRhtHdr;      //
    SIZE_T            dwSize;           //

    // Split cases based upon the arg token type
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

                break;          // Join common global code
            } // End IF

            // Handle the immediate case
        case TKT_VARIMMED:
            return;

        case TKT_VARARRAY:
            hGlbRht = lptkRhtArg->tkData.tkGlbData;

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
     && IsScalar (aplRankRht))
    {
        APLLONGEST aplLongestRht;
        IMM_TYPES  immTypeRht;

        // Get the immediate value, making sure we don't
        //   overextend beyond the bounds of the right arg
        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
                aplLongestRht = BIT0 & *(LPAPLBOOL) lpMemRht;
                immTypeRht    = IMMTYPE_BOOL;

                break;

            case ARRAY_INT:
                aplLongestRht = *(LPAPLINT)     lpMemRht;
                immTypeRht    = IMMTYPE_INT;

                break;

            case ARRAY_FLOAT:
                aplLongestRht = *(LPAPLLONGEST) lpMemRht;
                immTypeRht    = IMMTYPE_FLOAT;

                break;

            case ARRAY_CHAR:
                aplLongestRht = *(LPAPLCHAR)    lpMemRht;
                immTypeRht    = IMMTYPE_CHAR;

                break;

            case ARRAY_APA:
#define lpAPA           ((LPAPLAPA) lpMemRht)
                aplLongestRht = lpAPA->Off;
                immTypeRht    = IMMTYPE_INT;
#undef  lpAPA
                break;

            case ARRAY_HETERO:
                switch ((*(LPSYMENTRY *) lpMemRht)->stFlags.ImmType)
                {
                    case IMMTYPE_BOOL:
                        aplLongestRht = BIT0 & (*(LPSYMENTRY *) lpMemRht)->stData.stBoolean;
                        immTypeRht    = IMMTYPE_BOOL;

                        break;

                    case IMMTYPE_INT:
                        aplLongestRht = (*(LPSYMENTRY *) lpMemRht)->stData.stInteger;
                        immTypeRht    = IMMTYPE_INT;

                        break;

                    case IMMTYPE_FLOAT:
                        aplLongestRht = *(LPAPLLONGEST) &(*(LPSYMENTRY *) lpMemRht)->stData.stFloat;
                        immTypeRht    = IMMTYPE_FLOAT;

                        break;

                    case IMMTYPE_CHAR:
                        aplLongestRht = (*(LPSYMENTRY *) lpMemRht)->stData.stChar;
                        immTypeRht    = IMMTYPE_CHAR;

                        break;

                    defstop
                        break;
                } // End SWITCH

                break;

            defstop
                break;
        } // End SWITCH

        // Split cases based upon the right arg token type
        switch (lptkRhtArg->tkFlags.TknType)
        {
            case TKT_VARNAMED:
                // Fill in the result token
                lptkRhtArg->tkData.tkSym->stFlags.Imm      = TRUE;
                lptkRhtArg->tkData.tkSym->stFlags.ImmType  = immTypeRht;
                lptkRhtArg->tkData.tkSym->stData.stLongest = aplLongestRht;

                break;

            case TKT_VARARRAY:
                // Fill in the result token
                lptkRhtArg->tkFlags.TknType  = TKT_VARIMMED;
                lptkRhtArg->tkFlags.ImmType  = immTypeRht;
                lptkRhtArg->tkData.tkLongest = aplLongestRht;

                break;

            defstop
                break;
        } // End SWITCH

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
                aplTypeRes = aTypePromote[aplTypeRes][aplTypeSub];

                // If we're back to the same type, quit
                if (aplTypeRes EQ aplTypeRht)
                    break;
            } // End FOR

            break;

        case ARRAY_HETERO:      // Demote to simple homogeneous
        case ARRAY_NESTED:      // Demote to simple homogeneous/heterogeneous
            // Initialize the type
            aplTypeRes = ARRAY_INIT;

            // Take into account nested prototypes
            if (IsNested (aplTypeRht))
                aplNELMNstRht = max (aplNELMRht, 1);
            else
                aplNELMNstRht = aplNELMRht;

            // Loop through the elements
            for (uRht = 0; uRht < aplNELMNstRht; uRht++, ((LPAPLHETERO) lpMemRht)++)
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
                    aplTypeRes = aTypePromote[aplTypeRes][aplTypeSub];

                    // Check for no demotion
                    if (IsSimpleHet (aplTypeRes)
                     && IsSimpleHet (aplTypeRht))
                        goto NORMAL_EXIT;
                    break;

                case PTRTYPE_HGLOBAL:
                        // It's nested, so there's no demotion
                        goto NORMAL_EXIT;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        defstop
            break;
    } // End SWITCH

    // If the result type is the same as the right arg type,
    //   there's no demotion
    if (aplTypeRes EQ aplTypeRht)
        goto NORMAL_EXIT;

    // Check for demotion from Nested to Hetero
    if (IsSimpleHet (aplTypeRes)
     && IsNested (aplTypeRht))
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
                dprintfWL0 (L"##RefCnt=1 in " APPEND_NAME L": %p(res=1) (%S#%d)", hGlbRes, FNLN);
#endif
#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
                // Set the reference count and array type
                lpHeader->RefCnt  = 1;
                lpHeader->ArrType = ARRAY_HETERO;
#undef  lpHeader
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
        if (IsScalar (aplRankRht)
         && IsSimple (aplTypeRes))
        {
            LPAPLLONGEST lpaplLongestRht;

            // Split cases based upon the right arg token type
            switch (lptkRhtArg->tkFlags.TknType)
            {
                case TKT_VARNAMED:
                    // Fill in the result token
                    lptkRhtArg->tkData.tkSym->stFlags.Imm      = TRUE;
                    lptkRhtArg->tkData.tkSym->stFlags.ImmType  = TranslateArrayTypeToImmType (aplTypeRes);
                    lptkRhtArg->tkData.tkSym->stData.stLongest = 0;
                    lpaplLongestRht = &lptkRhtArg->tkData.tkSym->stData.stLongest;

                    break;

                case TKT_VARARRAY:
                    // Fill in the result token
                    lptkRhtArg->tkFlags.TknType  = TKT_VARIMMED;
                    lptkRhtArg->tkFlags.ImmType  = TranslateArrayTypeToImmType (aplTypeRes);
                    lptkRhtArg->tkData.tkLongest = 0;
                    lpaplLongestRht = &lptkRhtArg->tkData.tkLongest;

                    break;

                defstop
                    break;
            } // End SWITCH

            // Skip over header and dimensions to the data
            lpMemRht = VarArrayBaseToData (lpMemRhtHdr, aplRankRht);

            // Demote the data in the right arg, copying it to the result
            DemoteData (aplTypeRes,
                        lpaplLongestRht,
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

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Allocate space for a new array
        hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (!hGlbRes)
            goto WSFULL_EXIT;
        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = aplTypeRes;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
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
    } // End IF/ELSE

    goto NORMAL_EXIT;

WSFULL_EXIT:
    // WS FULL, so no demotion
    hGlbRes = hGlbRht;
NORMAL_EXIT:
    // Split cases based upon the right arg token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // Save the result in the right arg token
            lptkRhtArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbRes);

            break;

        case TKT_VARARRAY:
            // Save the result in the right arg token
            lptkRhtArg->tkData.tkGlbData = MakePtrTypeGlb (hGlbRes);

            break;

        defstop
            break;
    } // End SWITCH
IMMED_EXIT:
    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF
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
    APLUINT uRht;                   // Loop counter
    UINT    uBitIndex;              // Bit index for looping through Booleans

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
                        Assert (IsBooleanValue (*(LPAPLINT) lpMemRht));

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
                        Assert (IsBooleanValue (*(LPAPLFLOAT) lpMemRht));

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
                                Assert (IsBooleanValue ((*(LPAPLHETERO) lpMemRht)->stData.stBoolean));

                                *((LPAPLBOOL) lpMemRes) |= ((*((LPAPLHETERO) lpMemRht)++)->stData.stBoolean) << uBitIndex;

                                break;

                            case IMMTYPE_INT:
                                Assert (IsBooleanValue ((*(LPAPLHETERO) lpMemRht)->stData.stInteger));

                                *((LPAPLBOOL) lpMemRes) |= ((*((LPAPLHETERO) lpMemRht)++)->stData.stInteger) << uBitIndex;

                                break;

                            case IMMTYPE_FLOAT:
                                Assert (IsBooleanValue ((*(LPAPLHETERO) lpMemRht)->stData.stFloat));

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
//  $TypePromote_EM
//
//  Promote the type of the token to a given storage type
//***************************************************************************

UBOOL TypePromote_EM
    (LPTOKEN  lpToken,              // Ptr to the token
     APLSTYPE aplTypeRes,           // The result storage type
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    HGLOBAL *lphGlbArg;             // Ptr to where the HGLOBAL came from

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                // Point to the HGLOBAL
                lphGlbArg = &lpToken->tkData.tkSym->stData.stGlbData;

                break;      // Continue below with global case
            } // End IF

            // Split cases based upon the result storage type
            switch (aplTypeRes)
            {
                case ARRAY_INT:         // B -> I
                    lpToken->tkData.tkInteger = lpToken->tkData.tkSym->stData.stBoolean;

                    break;

                case ARRAY_FLOAT:       // B/I -> F
                    lpToken->tkData.tkSym->stData.stFloat = (APLFLOAT) lpToken->tkData.tkSym->stData.stInteger;

                    break;

                defstop
                    break;
            } // End SWITCH

            // Change the immediate type in the token
            lpToken->tkData.tkSym->stFlags.ImmType = TranslateArrayTypeToImmType (aplTypeRes);

            return TRUE;

        case TKT_VARIMMED:
            // Split cases based upon the result storage type
            switch (aplTypeRes)
            {
                case ARRAY_INT:         // B -> I
                    lpToken->tkData.tkInteger = lpToken->tkData.tkBoolean;

                    break;

                case ARRAY_FLOAT:       // B/I -> F
                    lpToken->tkData.tkFloat = (APLFLOAT) lpToken->tkData.tkInteger;

                    break;

                defstop
                    break;
            } // End SWITCH

            // Change the immediate type in the token
            lpToken->tkFlags.ImmType = TranslateArrayTypeToImmType (aplTypeRes);

            return TRUE;

        case TKT_VARARRAY:
            // Point to the HGLOBAL
            lphGlbArg = &lpToken->tkData.tkGlbData;

            break;      // Continue below with global case

        defstop
            break;
    } // End SWITCH

    // tk/stData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir_PTB (*lphGlbArg));

    // Handle the HGLOBAL case
    return TypePromoteGlb_EM (lphGlbArg, aplTypeRes, lptkFunc);
} // End TypePromote_EM


//***************************************************************************
//  $TypePromoteGlb_EM
//
//  Promote the type of the global memory handle to a given storage type
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- TypePromoteGlb_EM"
#else
#define APPEND_NAME
#endif

UBOOL TypePromoteGlb_EM
    (HGLOBAL *lphGlbArg,            // Ptr to global memory handle
     APLSTYPE aplTypeRes,           // The result storage type
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    HGLOBAL    hGlbArg,             // Arg    ...
               hGlbRes = NULL;      // Result global memory handle
    UBOOL      bRet = FALSE;        // TRUE iff the result is valid
    LPVOID     lpMemArg,            // Ptr to global memory
               lpMemRes = NULL;     // Ptr to result global memory
    APLSTYPE   aplTypeArg;          // Arg storage type of HGLOBAL
    APLNELM    aplNELMArg;          // Arg NELM         ...
    APLRANK    aplRankArg;          // Arg Rank         ...
    APLUINT    ByteRes;             // # bytes in the result
    LPSYMENTRY lpSymTmp;            // Ptr to temporary LPSYMENTRY

    // Copy the HGLOBAL
    hGlbArg = *lphGlbArg;

    // Lock the memory to get a ptr to it
    lpMemArg = MyGlobalLock (hGlbArg);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemArg)
    // Get the Array Type and NELM
    aplTypeArg = lpHeader->ArrType;
    aplNELMArg = lpHeader->NELM;
    aplRankArg = lpHeader->Rank;
#undef  lpHeader

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMArg, aplRankArg);

    // Allocate space for the result
    hGlbRes = MyGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        // WS FULL, so no promotion
        goto NORMAL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE     // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMArg;
    lpHeader->Rank       = aplRankArg;
#undef  lpHeader

    // Skip over the dimensions to the data
    lpMemArg = VarArrayBaseToDim (lpMemArg);
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Copy the arg dimensions to the result
    CopyMemory (lpMemRes, lpMemArg, (APLU3264) BytesIn (ARRAY_INT, aplRankArg));

    // Skip over the dimensions to the data
    lpMemArg = VarArrayDimToData (lpMemArg, aplRankArg);
    lpMemRes = VarArrayDimToData (lpMemRes, aplRankArg);

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        APLUINT   uRes;                 // Loop counter
        APLINT    apaOff,               // APA offset
                  apaMul;               // APA multiplier
        UINT      uBitMask;             // Bit mask for looping through Booleans
        APLHETERO lpSym0,               // LPSYMENTRY for 0
                  lpSym1;               // ...            1

        case ARRAY_BOOL:                // A -> B
            Assert (IsSimpleAPA (aplTypeArg));

            // Split cases based upon the arg storage type
            switch (aplTypeArg)
            {
                case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemArg)
                    // Get the APA parameters
                    apaOff = lpAPA->Off;
                    apaMul = lpAPA->Mul;
#undef  lpAPA
                    Assert (apaMul EQ 0 && IsBooleanValue (apaOff));

                    // Do something only for 1s
                    if (apaOff EQ 1)
                        FillBitMemory (lpMemRes, aplNELMArg);
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_INT:                 // B/A -> I
            Assert (IsSimpleBool (aplTypeArg)
                 || IsSimpleAPA (aplTypeArg));
            // Split cases based upon the arg storage type
            switch (aplTypeArg)
            {
                case ARRAY_BOOL:
                    uBitMask = BIT0;

                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                    {
                        if (uBitMask & *(LPAPLBOOL) lpMemArg)
                            *((LPAPLINT) lpMemRes) = TRUE;
                        ((LPAPLINT) lpMemRes)++;

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;            // Start over
                            ((LPAPLBOOL) lpMemArg)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemArg)
                    // Get the APA parameters
                    apaOff = lpAPA->Off;
                    apaMul = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                        *((LPAPLINT) lpMemRes)++ =
                          apaOff + apaMul * uRes;
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_FLOAT:               // B/I/A -> F
            Assert (IsSimpleInt (aplTypeArg));

            // Split cases based upon the arg storage type
            switch (aplTypeArg)
            {
                case ARRAY_BOOL:
                    uBitMask = BIT0;

                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                    {
                        if (uBitMask & *(LPAPLBOOL) lpMemArg)
                            *((LPAPLFLOAT) lpMemRes) = TRUE;
                        ((LPAPLFLOAT) lpMemRes)++;

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;            // Start over
                            ((LPAPLBOOL) lpMemArg)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemArg)
                    // Get the APA parameters
                    apaOff = lpAPA->Off;
                    apaMul = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                        *((LPAPLFLOAT) lpMemRes)++ =
                          (APLFLOAT) (APLINT) (apaOff + apaMul * uRes);
                    break;

                case ARRAY_INT:
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                        *((LPAPLFLOAT) lpMemRes)++ =
                          (APLFLOAT) *((LPAPLINT) lpMemArg)++;
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_HETERO:              // B/I/F/C/A -> H
            Assert (IsSimpleNH (aplTypeArg));
        case ARRAY_NESTED:              // B/I/F/C/A/H -> N
            Assert (IsSimple   (aplTypeArg));

            // Check for empty arg
            if (IsEmpty (aplNELMArg))
            {
                // Split cases based upon the arg storage type
                switch (aplTypeArg)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                    case ARRAY_FLOAT:
                    case ARRAY_APA:
                        // Save a zero STE as the prototype
                        *((LPAPLHETERO) lpMemRes)++ = GetSteZero ();

                        break;

                    case ARRAY_CHAR:
                        // Save a zero STE as the prototype
                        *((LPAPLHETERO) lpMemRes)++ = GetSteBlank ();

                        break;

                    defstop
                        break;
                } // End SWITCH
            } else
            // Split cases based upon the arg storage type
            switch (aplTypeArg)
            {
                case ARRAY_BOOL:
                    uBitMask = BIT0;
                    lpSym0 = GetSteZero ();
                    lpSym1 = GetSteOne ();

                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                    {
                        *((LPAPLHETERO) lpMemRes)++ =
                          (uBitMask & *(LPAPLBOOL) lpMemArg) ? lpSym1 : lpSym0;

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;            // Start over
                            ((LPAPLBOOL) lpMemArg)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                    {
                        *((LPAPLHETERO) lpMemRes)++ =
                        lpSymTmp =
                          SymTabAppendInteger_EM (*((LPAPLINT) lpMemArg)++);
                        if (!lpSymTmp)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_FLOAT:
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                    {
                        *((LPAPLHETERO) lpMemRes)++ =
                        lpSymTmp =
                          SymTabAppendFloat_EM (*((LPAPLFLOAT) lpMemArg)++);
                        if (!lpSymTmp)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_CHAR:
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                    {
                        *((LPAPLHETERO) lpMemRes)++ =
                        lpSymTmp =
                          SymTabAppendChar_EM (*((LPAPLCHAR) lpMemArg)++);
                        if (!lpSymTmp)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemArg)
                    // Get the APA parameters
                    apaOff = lpAPA->Off;
                    apaMul = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                    {
                        *((LPAPLHETERO) lpMemRes)++ =
                        lpSymTmp =
                          SymTabAppendInteger_EM (apaOff + apaMul * uRes);
                        if (!lpSymTmp)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_HETERO:
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                        *((LPAPLHETERO) lpMemRes)++ =
                          *((LPAPLHETERO) lpMemArg)++;
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need thess ptrs
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    MyGlobalUnlock (hGlbArg); lpMemArg = NULL;

    // Free the old HGLOBAL
    FreeResultGlobalVar (hGlbArg); hGlbArg = NULL;

    // Save the new HGLOBAL
    *lphGlbArg = hGlbRes;
NORMAL_EXIT:
    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    if (hGlbArg && lpMemArg)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemArg = NULL;
    } // End IF

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    return bRet;
} // End TypePromoteGlb_EM
#undef  APPEND_NAME


//***************************************************************************
//  $QueryPromote
//
//  Determine if an arg should be promoted
//***************************************************************************

UBOOL QueryPromote
    (APLSTYPE  aplTypeNam,          // Name arg storage type
     APLSTYPE  aplTypeSet,          // Set  ...
     APLSTYPE *lpaplTypeRes)        // Result   ...

{
    // Calculate the result storage type
    *lpaplTypeRes = aTypePromote[aplTypeNam][aplTypeSet];

    // Compare the storage type of the result with the existing storage type
    return (*lpaplTypeRes NE aplTypeNam);
} // End QueryPromote


//***************************************************************************
//  End of File: typemote.c
//***************************************************************************
