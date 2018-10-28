//***************************************************************************
//  NARS2000 -- Type Demote/Promote Functions
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2018 Sudley Place Software

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
//  mean in fewer bytes as 32-bit ptrs are four bytes while integers are eight.
//***************************************************************************

void TypeDemote
    (LPTOKEN lptkRhtArg,                    // Ptr to token to demote
     UBOOL   bDimDemote)                    // TRUE iff dimension demotion allowed

{
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbSub,              // Temp      ...
                      hGlbRes = NULL;       // Result    ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL,   // ...    result    ...
                      lpMemHdrSub = NULL;   // ...    subitem   ...
    LPVOID            lpMemRht,             // Ptr to right arg global memory
                      lpMemRes,             // Ptr to result    ...
                      lpMemSub;             // Ptr to temp      ...
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMNstRht;        // Right arg NELM in case empty nested
    APLRANK           aplRankRht,           // Right arg rank
                      aplRankSub;           // Temp      ...
    APLUINT           uRht,                 // Right arg loop counter
                      ByteRes;              // # bytes in the result
    APLINT            aplInteger;           // Temporary integer
    APLFLOAT          aplFloat;             // ...       float
    APLSTYPE          aplTypeRes,           // Result storage type
                      aplTypeRht,           // Right arg ...
                      aplTypeSub;           // Right arg item ...
    LPSYMENTRY        lpSymEntry;           // Ptr to Hetero item
    SIZE_T            dwSize;               // GlobalSize of right arg
    int               iHCDimRht,            // Right arg HC dimension (1, 2, 4, 8)
                      iHCMax,               // HC max dimension
                      iSizeofRht;           // # bytes in each right arg item
    int               i;                    // Loop counter

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
            // if it's integer-like and Boolean valued, ...
            if (IsImmInt (lptkRhtArg->tkFlags.ImmType)
             && IsBooleanValue (*GetPtrTknLongest (lptkRhtArg)))
                lptkRhtArg->tkFlags.ImmType = IMMTYPE_BOOL;
            else
            // If it's an INT masquerading as a FLT, ...
            if (IsImmFlt (lptkRhtArg->tkFlags.ImmType)
            // Check for PoM infinity and numbers whose
            //   absolute value is >= 2*53
             && !(IsFltInfinity (lptkRhtArg->tkData.tkFloat)
               || fabs (lptkRhtArg->tkData.tkFloat) >= Float2Pow53))
            {
                APLFLOAT aplFlt = lptkRhtArg->tkData.tkFloat,
                         aplFlr = floor (aplFlt);

                // If this FLT is an INT,
                //   and if -0 is allowed, the value is not -0, ...
                if (aplFlt EQ aplFlr
                 && !(gAllowNeg0 && IsFltN0 (aplFlt)))
                {
                    // Convert it
                    lptkRhtArg->tkFlags.ImmType  = IMMTYPE_INT;
                    lptkRhtArg->tkData.tkInteger = (APLINT) aplFlr;;
                } // End IF
            } // End IF/ELSE/...

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
    lpMemHdrRht = MyGlobalLockVar (hGlbRht);

    // Get the Type, NELM, and Rank
    aplTypeRht = lpMemHdrRht->ArrType;
    aplNELMRht = lpMemHdrRht->NELM;
    aplRankRht = lpMemHdrRht->Rank;

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // Calculate the # bytes in this item
    iSizeofRht = TranslateArrayTypeToSizeof (aplTypeRht);

    // Calculate the HC dimension (1, 2, 4, 8)
    iHCDimRht  = TranslateArrayTypeToHCDim  (aplTypeRht);

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
                // Split cases based upon the item's ptr type
                switch (GetPtrTypeInd (lpMemRht))
                {
                    case PTRTYPE_STCONST:
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

                            case IMMTYPE_RAT:
                                // Get the global memory handle
                                hGlbSub = ClrPtrTypeInd (lpMemRht);

                                // Lock the memory to get a handle on it
                                lpMemHdrSub = MyGlobalLockVar (hGlbSub);

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Copy the data
                                lptkRhtArg->tkFlags.TknType  = TKT_VARARRAY;
                                lptkRhtArg->tkFlags.ImmType  = IMMTYPE_RAT;
                                lptkRhtArg->tkData.tkGlbData =
                                  MakeGlbEntry_EM (ARRAY_RAT,               // Entry type
                                                   (LPAPLRAT) lpMemSub,     // Ptr to the value
                                                   TRUE,                    // TRUE iff we should initialize the target first
                                                   lptkRhtArg);             // Ptr to function token
                                // We no longer need this ptr
                                MyGlobalUnlock (hGlbSub); lpMemHdrSub = NULL;

                                // Check for error
                                if (lptkRhtArg->tkData.tkGlbData EQ NULL)
                                    goto WSFULL_EXIT;
                                goto UNLOCK_EXIT;

                            case IMMTYPE_VFP:
                                // Get the global memory handle
                                hGlbSub = ClrPtrTypeInd (lpMemRht);

                                // Lock the memory to get a handle on it
                                lpMemHdrSub = MyGlobalLockVar (hGlbSub);

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Copy the data
                                lptkRhtArg->tkFlags.TknType  = TKT_VARARRAY;
                                lptkRhtArg->tkFlags.ImmType  = IMMTYPE_VFP;
                                lptkRhtArg->tkData.tkGlbData =
                                  MakeGlbEntry_EM (ARRAY_VFP,               // Entry type
                                                   (LPAPLVFP) lpMemSub,     // Ptr to the value
                                                   TRUE,                    // TRUE iff we should initialize the target first
                                                   lptkRhtArg);             // Ptr to function token
                                // We no longer need this ptr
                                MyGlobalUnlock (hGlbSub); lpMemHdrSub = NULL;

                                // Check for error
                                if (lptkRhtArg->tkData.tkGlbData EQ NULL)
                                    goto WSFULL_EXIT;
                                goto UNLOCK_EXIT;

                            case IMMTYPE_ARB:
                                // Get the global memory handle
                                hGlbSub = ClrPtrTypeInd (lpMemRht);

                                // Lock the memory to get a handle on it
                                lpMemHdrSub = MyGlobalLockVar (hGlbSub);

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Copy the data
                                lptkRhtArg->tkFlags.TknType  = TKT_VARARRAY;
                                lptkRhtArg->tkFlags.ImmType  = IMMTYPE_ARB;
                                lptkRhtArg->tkData.tkGlbData =
                                  MakeGlbEntry_EM (ARRAY_ARB,               // Entry type
                                                   (LPAPLARB) lpMemSub,     // Ptr to the value
                                                   TRUE,                    // TRUE iff we should initialize the target first
                                                   lptkRhtArg);             // Ptr to function token
                                // We no longer need this ptr
                                MyGlobalUnlock (hGlbSub); lpMemHdrSub = NULL;

                                // Check for error
                                if (lptkRhtArg->tkData.tkGlbData EQ NULL)
                                    goto WSFULL_EXIT;
                                goto UNLOCK_EXIT;

                            defstop
                                break;
                        } // End SWITCH

                        break;

                    case PTRTYPE_HGLOBAL:
                        // Get the global memory handle
                        hGlbSub = ClrPtrTypeInd (lpMemRht);

                        // Lock the memory to get a handle on it
                        lpMemHdrSub = MyGlobalLockVar (hGlbSub);

                        Assert (IsScalar (lpMemHdrSub->Rank));

                        // Get the array storage type
                        aplTypeSub = lpMemHdrSub->ArrType;

                        // Skip over the header and dimensions to the data
                        lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                        // Split cases based upon the array storage type
                        switch (aplTypeSub)
                        {
                            case ARRAY_RAT:
                                // Copy the data
                                lptkRhtArg->tkFlags.TknType  = TKT_VARARRAY;
                                lptkRhtArg->tkFlags.ImmType  = IMMTYPE_RAT;
                                lptkRhtArg->tkData.tkGlbData =
                                  MakeGlbEntry_EM (ARRAY_RAT,               // Entry type
                                                   (LPAPLRAT) lpMemSub,     // Ptr to the value
                                                   TRUE,                    // TRUE iff we should initialize the target first
                                                   lptkRhtArg);             // Ptr to function token
                                break;

                            case ARRAY_VFP:
                                // Copy the data
                                lptkRhtArg->tkFlags.TknType  = TKT_VARARRAY;
                                lptkRhtArg->tkFlags.ImmType  = IMMTYPE_VFP;
                                lptkRhtArg->tkData.tkGlbData =
                                  MakeGlbEntry_EM (ARRAY_VFP,               // Entry type
                                                   (LPAPLVFP) lpMemSub,     // Ptr to the value
                                                   TRUE,                    // TRUE iff we should initialize the target first
                                                   lptkRhtArg);             // Ptr to function token
                                break;

                            case ARRAY_ARB:
                                // Copy the data
                                lptkRhtArg->tkFlags.TknType  = TKT_VARARRAY;
                                lptkRhtArg->tkFlags.ImmType  = IMMTYPE_ARB;
                                lptkRhtArg->tkData.tkGlbData =
                                  MakeGlbEntry_EM (ARRAY_ARB,               // Entry type
                                                   (LPAPLARB) lpMemSub,     // Ptr to the value
                                                   TRUE,                    // TRUE iff we should initialize the target first
                                                   lptkRhtArg);             // Ptr to function token
                                break;

                            defstop
                                break;
                        } // End SWITCH

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbSub); lpMemHdrSub = NULL;

                        // Check for error
                        if (lptkRhtArg->tkData.tkGlbData EQ NULL)
                            goto WSFULL_EXIT;
                        goto UNLOCK_EXIT;

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
UNLOCK_EXIT:
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

        // We no longer need this storage
        FreeResultGlobalVar (hGlbRht); hGlbRht = NULL;

        goto IMMED_EXIT;
    } // End IF

    // Initialize the max value
    iHCMax = 0;

    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_HC2I:        // Attempt to catch trailing zero imaginary parts
        case ARRAY_HC4I:        // ...
        case ARRAY_HC8I:        // ...
            if (!bDimDemote)
                goto IMMED_EXIT;
            // Loop through the elements
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Loop through the dimension backwards looking for non-zero values
                for (i = iHCDimRht - 1; i >= 0; i--)
                if ((((LPAPLHC8I) &((LPBYTE) lpMemRht)[uRht * iSizeofRht])->parts[i]) NE 0)
                    break;

                // Save the largest value
                iHCMax = max (i, iHCMax);
            } // End FOR

            // We made it through all elements
            // iHCDimRht    2     4     8
            // i == 0      INT   INT   INT
            // i == 1      HC2I  HC2I  HC2I
            // i == 2            HC4I  HC4I
            // i == 3            HC4I  HC4I
            // i == 4                  HC8I
            // i == 5                  HC8I
            // i == 6                  HC8I
            // i == 7                  HC8I

            // Calculate the result storage type
            aplTypeRes = uHCxI[aHCDimToIndex[iHCMax + 1]];  // "+ 1" to skip over aHCDimToIndex[0] == ENUM_HCFI_ERR

            break;

        case ARRAY_HC2F:        // Attempt to catch trailing zero imaginary parts
        case ARRAY_HC4F:        // ...
        case ARRAY_HC8F:        // ...
            if (!bDimDemote)
                goto IMMED_EXIT;
            // Loop through the elements
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Loop through the dimension backwards looking for non-zero values
                for (i = iHCDimRht - 1; i >= 0; i--)
                if ((((LPAPLHC8F) &((LPBYTE) lpMemRht)[uRht * iSizeofRht])->parts[i]) NE 0.0)
                    break;

                // Save the largest value
                iHCMax = max (i, iHCMax);
            } // End FOR

            // We made it through all elements
            // iHCDimRht    2     4     8
            // i == 0      FLT   FLT   FLT
            // i == 1      HC2F  HC2F  HC2F
            // i == 2            HC4F  HC4F
            // i == 3            HC4F  HC4F
            // i == 4                  HC8F
            // i == 5                  HC8F
            // i == 6                  HC8F
            // i == 7                  HC8F

            // Calculate the result storage type
            aplTypeRes = uHCxF[aHCDimToIndex[iHCMax + 1]];  // "+ 1" to skip over aHCDimToIndex[0] == ENUM_HCFI_ERR

            break;

        case ARRAY_HC2R:        // Attempt to catch trailing zero imaginary parts
        case ARRAY_HC4R:        // ...
        case ARRAY_HC8R:        // ...
            if (!bDimDemote)
                goto IMMED_EXIT;
            // Loop through the elements
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Loop through the dimension backwards looking for non-zero values
                for (i = iHCDimRht - 1; i >= 0; i--)
                if (!IsMpq0 (&((LPAPLHC8R) &((LPBYTE) lpMemRht)[uRht * iSizeofRht])->parts[i]))
                    break;

                // Save the largest value
                iHCMax = max (i, iHCMax);
            } // End FOR

            // We made it through all elements
            // iHCDimRht    2     4     8
            // i == 0      RAT   RAT   RAT
            // i == 1      HC2R  HC2R  HC2R
            // i == 2            HC4R  HC4R
            // i == 3            HC4R  HC4R
            // i == 4                  HC8R
            // i == 5                  HC8R
            // i == 6                  HC8R
            // i == 7                  HC8R

            // Calculate the result storage type
            aplTypeRes = uHCxR[aHCDimToIndex[iHCMax + 1]];  // "+ 1" to skip over aHCDimToIndex[0] == ENUM_HCFI_ERR

            break;

        case ARRAY_HC2V:        // Attempt to catch trailing zero imaginary parts
        case ARRAY_HC4V:        // ...
        case ARRAY_HC8V:        // ...
            if (!bDimDemote)
                goto IMMED_EXIT;
            // Loop through the elements
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Loop through the dimension backwards looking for non-zero values
                for (i = iHCDimRht - 1; i >= 0; i--)
                if (!IsMpf0 (&((LPAPLHC8V) &((LPBYTE) lpMemRht)[uRht * iSizeofRht])->parts[i]))
                    break;

                // Save the largest value
                iHCMax = max (i, iHCMax);
            } // End FOR

            // We made it through all elements
            // iHCDimRht    2     4     8
            // i == 0      VFP   VFP   VFP
            // i == 1      HC2V  HC2V  HC2V
            // i == 2            HC4V  HC4V
            // i == 3            HC4V  HC4V
            // i == 4                  HC8V
            // i == 5                  HC8V
            // i == 6                  HC8V
            // i == 7                  HC8V

            // Calculate the result storage type
            aplTypeRes = uHCxV[aHCDimToIndex[iHCMax + 1]];  // "+ 1" to skip over aHCDimToIndex[0] == ENUM_HCFI_ERR

            break;

        case ARRAY_BA2F:        // Attempt to catch trailing zero imaginary parts
        case ARRAY_BA4F:        // ...
        case ARRAY_BA8F:        // ...
            if (!bDimDemote)
                goto IMMED_EXIT;
            // Loop through the elements
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Loop through the dimension backwards looking for non-zero values
                for (i = iHCDimRht - 1; i >= 0; i--)
                if (!IsArb0 (&((LPAPLBA8F) &((LPBYTE) lpMemRht)[uRht * iSizeofRht])->parts[i]))
                    break;

                // Save the largest value
                iHCMax = max (i, iHCMax);
            } // End FOR

            // We made it through all elements
            // iHCDimRht    2     4     8
            // i == 0      ARB   ARB   ARB
            // i == 1      BA2F  BA2F  BA2F
            // i == 2            BA4F  BA4F
            // i == 3            BA4F  BA4F
            // i == 4                  BA8F
            // i == 5                  BA8F
            // i == 6                  BA8F
            // i == 7                  BA8F

            // Calculate the result storage type
            aplTypeRes = uBAxF[aHCDimToIndex[iHCMax + 1]];  // "+ 1" to skip over aHCDimToIndex[0] == ENUM_HCFI_ERR

            break;

        case ARRAY_BOOL:        // No place to go
        case ARRAY_APA:         // ...
        case ARRAY_CHAR:        // ...
        case ARRAY_RAT:         // ...
        case ARRAY_VFP:         // ...
        case ARRAY_ARB:         // ...
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

                // If it's -0, ...
                if (gAllowNeg0
                 && IsFltN0 (aplFloat))
                    aplTypeSub = ARRAY_FLOAT;
                else
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

                            // If it's -0, ...
                            if (gAllowNeg0
                             && IsFltN0 (aplFloat))
                                aplTypeSub = ARRAY_FLOAT;
                            else
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
                    // Get the global memory handle
                    hGlbSub = ClrPtrTypeInd (lpMemRht);

                    // Get the global's Type
                    AttrsOfGlb (hGlbSub, &aplTypeSub, NULL, &aplRankSub, NULL);

                    // If it's not a scalar, ...
                    if (!IsScalar (aplRankSub))
                        // It's nested, so there's no demotion
                        goto NORMAL_EXIT;

                    // Split cases based upon the item storage type
                    switch (aplTypeSub)
                    {
                        case ARRAY_BOOL:
                        case ARRAY_NESTED:

                        case ARRAY_INT:
                        case ARRAY_HC2I:
                        case ARRAY_HC4I:
                        case ARRAY_HC8I:

                        case ARRAY_FLOAT:
                        case ARRAY_HC2F:
                        case ARRAY_HC4F:
                        case ARRAY_HC8F:

                        case ARRAY_RAT:
                        case ARRAY_HC2R:
                        case ARRAY_HC4R:
                        case ARRAY_HC8R:

                        case ARRAY_VFP:
                        case ARRAY_HC2V:
                        case ARRAY_HC4V:
                        case ARRAY_HC8V:

                        case ARRAY_ARB:
                        case ARRAY_BA2F:
                        case ARRAY_BA4F:
                        case ARRAY_BA8F:
                            // Check storage type
                            aplTypeRes = aTypePromote[aplTypeRes][aplTypeSub];

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

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

    // Skip over header and dimensions to the data
    //   as the above SWITCH stmt may have modified lpMemRht
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // Check for demotion from Nested to Hetero
    if (IsSimpleHet (aplTypeRes)
     && IsNested (aplTypeRht))
    {
        // If the reference count of this array is one, just
        //   change the array type from ARRAY_NESTED to ARRAY_HETERO.
        if (lpMemHdrRht->RefCnt EQ 1)
            lpMemHdrRht->ArrType = ARRAY_HETERO;
        else
        {
            // Copy this array and change the type from ARRAY_NESTED to ARRAY_HETERO
            dwSize = MyGlobalSize (hGlbRht);
            hGlbRes = DbgGlobalAlloc (GHND, dwSize);
            if (hGlbRes NE NULL)
            {
                // Lock the memory to get a ptr to it
                lpMemHdrRes = MyGlobalLock000 (hGlbRes);

                // Copy source to destin
                CopyMemory (lpMemHdrRes, lpMemHdrRht, dwSize);

#ifdef DEBUG_REFCNT
                dprintfWL0 (L"##RefCnt=1 in " APPEND_NAME L": %p(res=1) (%S#%d)", hGlbRes, FNLN);
#endif
#define lpHeader    lpMemHdrRes
                // Set the reference count and array type
                lpHeader->RefCnt  = 1;
                lpHeader->ArrType = ARRAY_HETERO;
#undef  lpHeader
                // We no longer need these ptrs
                MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
                MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

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
                    lpaplLongestRht = GetPtrTknLongest (lptkRhtArg);

                    break;

                defstop
                    break;
            } // End SWITCH

            // Skip over header and dimensions to the data
            lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

            // Demote the data in the right arg, copying it to the result
            DemoteData (aplTypeRes,
                        lpaplLongestRht,
                        aplTypeRht,
                        aplNELMRht,
                        lpMemRht,
                        bDimDemote);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

            // Free the old array
            FreeResultGlobalVar (hGlbRht); hGlbRht = NULL;

            // See if the result can be demoted any more
            //  say from FLT to INT
            TypeDemote (lptkRhtArg, bDimDemote);

            goto IMMED_EXIT;
        } // End IF

        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Allocate space for a new array
        hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;
        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
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
        lpMemRht = VarArrayBaseToDim (lpMemHdrRht);
        lpMemRes = VarArrayBaseToDim (lpMemHdrRes);

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
                    lpMemRht,
                    bDimDemote);
        // We no longer need these ptrs
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

        // Free the old array
        FreeResultGlobalVar (hGlbRht); hGlbRht = NULL;
    } else
    // Check for demotion from Nested or Hetero to RAT/VFP/HCxy/BAxF
    // Check for demotion of HC Dimension from right arg to result
    if ((IsPtrArray (aplTypeRht)
      && IsGlbNum (aplTypeRes))
     || (!IsPtrArray (aplTypeRht)
      && bDimDemote
      && (TranslateArrayTypeToHCDim (aplTypeRht)
        > TranslateArrayTypeToHCDim (aplTypeRes)))
       )
    {
        // If the result is simple NUMERIC, ...
        if (IsSimpleNum (aplTypeRes))
        {
            // The result must be one-dimensional
            Assert (TranslateArrayTypeToHCDim (aplTypeRes) EQ 1);

            // Fill in the result token
            lptkRhtArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkRhtArg->tkFlags.ImmType  = TranslateArrayTypeToImmType (aplTypeRes);

            // Skip over the header to the dimensions
            lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

            // Demote the data in the right arg, copying it to the result
            DemoteData (aplTypeRes,
                       GetPtrTknLongest (lptkRhtArg),
                        aplTypeRht,
                        aplNELMRht,
                        lpMemRht,
                        bDimDemote);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

            // See if the result can be demoted any more
            //  say from FLT to INT
            TypeDemote (lptkRhtArg, bDimDemote);
        } else
        {
            // Calculate space needed for the result
            ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

            // Check for overflow
            if (ByteRes NE (APLU3264) ByteRes)
                goto WSFULL_EXIT;

            // Allocate space for a new array
            hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
            if (hGlbRes EQ NULL)
                goto WSFULL_EXIT;
            // Lock the memory to get a ptr to it
            lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
            // Fill in the header
            lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = aplTypeRes;
////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELMRht;
            lpHeader->Rank       = aplRankRht;
#undef  lpHeader

            // Skip over the header to the dimensions
            lpMemRht = VarArrayBaseToDim (lpMemHdrRht);
            lpMemRes = VarArrayBaseToDim (lpMemHdrRes);

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
                        lpMemRht,
                        bDimDemote);
            // We no longer need these ptrs
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
            MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
        } // End IF/ELSE

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

        case TKT_VARIMMED:      // Handled above
            break;

        defstop
            break;
    } // End SWITCH
IMMED_EXIT:
    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF
} // End TypeDemote


//***************************************************************************
//  $TypeDemoteGlb
//
//  Attempt to type demote the data in a global memory handle
//***************************************************************************

HGLOBAL TypeDemoteGlb
    (HGLOBAL hGlbRht,               // Right arg global memory handle
     UBOOL   bDimDemote)            // TRUE iff dimension demotion allowed

{
    TOKEN      tkRhtArg = {0};      // Right arg token
    LPSYMENTRY lpSymEntry;          // Result if immediate

    tkRhtArg.tkFlags.TknType   = TKT_VARARRAY;
////tkRhtArg.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////tkRhtArg.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
    tkRhtArg.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRht);
////tkRhtArg.tkCharIndex       =                // Unused

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&tkRhtArg, bDimDemote);

    // Split cases based upon the token type
    switch (tkRhtArg.tkFlags.TknType)
    {
        case TKT_VARIMMED:
            lpSymEntry =
              MakeSymEntry_EM (tkRhtArg.tkFlags.ImmType,
                               GetPtrTknLongest (&tkRhtArg),
                               NULL);
            if (lpSymEntry EQ NULL)
                return hGlbRht;
            else
                return lpSymEntry;

        case TKT_VARARRAY:
            return tkRhtArg.tkData.tkGlbData;

        defstop
            return NULL;
    } // End SWITCH
} // End TypeDemoteGlb


//***************************************************************************
//  $DemoteData
//
//  Demote the data in the right arg, copying it to the result
//***************************************************************************

UBOOL DemoteData
    (APLSTYPE aplTypeRes,                   // Result storage type
     LPVOID   lpMemRes,                     // Ptr to result global memory
     APLSTYPE aplTypeRht,                   // Right arg storage type
     APLNELM  aplNELMRht,                   // Right arg NELM
     LPVOID   lpMemRht,                     // Ptr to right arg global memory
     UBOOL    bDimDemote)                   // TRUE iff dimension demotion allowed

{
    APLUINT           uRht;                 // Loop counter
    UINT              uBitIndex;            // Bit index for looping through Booleans
    LPSYMENTRY        lpSymGlbSub;          // Ptr to temp SYMENTRY/HGLOBAL
    LPVOID            lpMemSub;             // Ptr to temp global memory
    LPVARARRAY_HEADER lpMemHdrSub = NULL;   // Ptr to temp memory header
    ALLTYPES          atSub = {0};          // Item as ALLTYPES
    UBOOL             bRet;                 // TRUE iff the result is valid
    int               iHCDimRht,            // Right arg HC dimension (1, 2, 4, 8)
                      iHCDimRes,            // Result    ...
                      iSizeofRht,           // Sizeof a right arg item
                      iSizeofRes,           // ...      result    ...
                      i;                    // Loop counter
    TC_ACTION         tcActionRht;          // Ptr to TypeActConvert action routine for right arg

    // Calculate the size of the dimension
    iHCDimRht = TranslateArrayTypeToHCDim (aplTypeRht);
    iHCDimRes = TranslateArrayTypeToHCDim (aplTypeRes);

    // Calculate the sizeof
    iSizeofRht = TranslateArrayTypeToSizeof (aplTypeRht);
    iSizeofRes = TranslateArrayTypeToSizeof (aplTypeRes);

    // Split cases based upon the result's storage type
    // Note that the result is always of lower type than
    //   the right arg.
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:            // Res = BOOL, Rht = INT/FLOAT/HCxy/HETERO/NESTED
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
                        // Get the SymGlb ptr
                        lpSymGlbSub = *((LPAPLHETERO) lpMemRht)++;

                        // Split cases based upon the ptr type bits
                        switch (GetPtrTypeDir (lpSymGlbSub))
                        {
                            case PTRTYPE_STCONST:
                                // Split cases based upon the immediate type
                                switch (lpSymGlbSub->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:
                                        Assert (IsBooleanValue (lpSymGlbSub->stData.stBoolean));

                                        *((LPAPLBOOL) lpMemRes) |= (lpSymGlbSub->stData.stBoolean) << uBitIndex;

                                        break;

                                    case IMMTYPE_INT:
                                        Assert (IsBooleanValue (lpSymGlbSub->stData.stInteger));

                                        *((LPAPLBOOL) lpMemRes) |= (lpSymGlbSub->stData.stInteger) << uBitIndex;

                                        break;

                                    case IMMTYPE_FLOAT:
                                        Assert (IsBooleanValue (lpSymGlbSub->stData.stFloat));

                                        *((LPAPLBOOL) lpMemRes) |= ((APLINT) (lpSymGlbSub->stData.stFloat)) << uBitIndex;

                                        break;

                                    case IMMTYPE_CHAR:
                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:
                                // Lock the memory to get a ptr to it
                                lpMemHdrSub = MyGlobalLockVar (lpSymGlbSub);

                                Assert (IsScalar (lpMemHdrSub->Rank));

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Split cases based upon the storage type
                                switch (lpMemHdrSub->ArrType)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Rht = BOOL
                                    case ARRAY_INT:     // Res = BOOL, Rht = INT
                                    case ARRAY_APA:     // Res = BOOL, Rht = APA
                                    case ARRAY_FLOAT:   // Res = BOOL, Rht = FLT
                                        *((LPAPLBOOL) lpMemRes) |= GetNextInteger (lpMemSub, lpMemHdrSub->ArrType, 0) << uBitIndex;

                                        break;

                                    case ARRAY_RAT:     // Res = BOOL, Rht = RAT
                                    case ARRAY_VFP:     // Res = BOOL, Rht = VFP
                                                        // We don't demote RAT/VFP to BOOL
                                    defstop
                                        break;
                                } // End SWITCH

                                // We no longer need this ptr
                                MyGlobalUnlock (lpSymGlbSub); lpMemHdrSub = NULL;

                                break;

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

                case ARRAY_RAT:     // Res = BOOL, Rht = RAT
                case ARRAY_VFP:     // Res = BOOL, Rht = VFP
                case ARRAY_HC2I:    // Res = BOOL, Rht = HC2I
                case ARRAY_HC2F:    // Res = BOOL, Rht = HC2F
                case ARRAY_HC2R:    // Res = BOOL, Rht = HC2R
                case ARRAY_HC2V:    // Res = BOOL, Rht = HC2V
                case ARRAY_HC4I:    // Res = BOOL, Rht = HC4I
                case ARRAY_HC4F:    // Res = BOOL, Rht = HC4F
                case ARRAY_HC4R:    // Res = BOOL, Rht = HC4R
                case ARRAY_HC4V:    // Res = BOOL, Rht = HC4V
                case ARRAY_HC8I:    // Res = BOOL, Rht = HC8I
                case ARRAY_HC8F:    // Res = BOOL, Rht = HC8F
                case ARRAY_HC8R:    // Res = BOOL, Rht = HC8R
                case ARRAY_HC8V:    // Res = BOOL, Rht = HC8V
                    // Point to TCA routine to extract an indexed
                    //   value from the right arg
                    tcActionRht = aTypeActConvert[aplTypeRht][aplTypeRes];

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Convert to lower data type
                        (*tcActionRht) (lpMemRht, uRht, &atSub, &bRet);

                        // Ensure it's Boolean-valued
                        bRet &= IsBooleanValue (atSub.aplInteger);

                        if (!bRet)
                            goto ERROR_EXIT;

                        // Save in the result
                        *((LPAPLBOOL) lpMemRes) |= atSub.aplBoolean << uBitIndex;

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

        case ARRAY_INT:         // Res = INT, Rht = FLOAT/HCxy/HETERO/NESTED
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
                    {
                        // Get the SymGlb ptr
                        lpSymGlbSub = *((LPAPLHETERO) lpMemRht)++;

                        // Split cases based upon the ptr type bits
                        switch (GetPtrTypeDir (lpSymGlbSub))
                        {
                            case PTRTYPE_STCONST:
                                // Split cases based upon the immediate type
                                switch (lpSymGlbSub->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:  // Res = INT , Rht = BOOL
                                        *((LPAPLINT) lpMemRes)++ = lpSymGlbSub->stData.stBoolean;

                                        break;

                                    case IMMTYPE_INT:   // Res = INT , Rht = INT
                                        *((LPAPLINT) lpMemRes)++ = lpSymGlbSub->stData.stInteger;

                                        break;

                                    case IMMTYPE_FLOAT: // Res = INT , Rht = FLT
                                        *((LPAPLINT) lpMemRes)++ = (APLINT) lpSymGlbSub->stData.stFloat;

                                        break;

                                    case IMMTYPE_CHAR:  // Res = INT , Rht = CHAR
                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:
                                // Lock the memory to get a ptr to it
                                lpMemHdrSub = MyGlobalLockVar (lpSymGlbSub);

                                Assert (IsScalar (lpMemHdrSub->Rank));

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Split cases based upon the storage type
                                switch (lpMemHdrSub->ArrType)
                                {
                                    case ARRAY_BOOL:    // Res = INT , Rht = BOOL
                                    case ARRAY_INT:     // Res = INT , Rht = INT
                                    case ARRAY_APA:     // Res = INT , Rht = APA
                                    case ARRAY_FLOAT:   // Res = INT , Rht = FLT
                                        *((LPAPLINT) lpMemRes)++ = GetNextInteger (lpMemSub, lpMemHdrSub->ArrType, 0);

                                        break;

                                    case ARRAY_RAT:     // Res = INT , Rht = RAT
                                    case ARRAY_VFP:     // Res = INT , Rht = VFP
                                                        // We don't demote RAT/VFP to INT
                                    defstop
                                        break;
                                } // End SWITCH

                                // We no longer need this ptr
                                MyGlobalUnlock (lpSymGlbSub); lpMemHdrSub = NULL;

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                case ARRAY_RAT:     // Res = INT, Rht = RAT
                case ARRAY_VFP:     // Res = INT, Rht = VFP
                case ARRAY_HC2I:    // Res = INT, Rht = HC2I
                case ARRAY_HC2F:    // Res = INT, Rht = HC2F
                case ARRAY_HC2R:    // Res = INT, Rht = HC2R
                case ARRAY_HC2V:    // Res = INT, Rht = HC2V
                case ARRAY_HC4I:    // Res = INT, Rht = HC4I
                case ARRAY_HC4F:    // Res = INT, Rht = HC4F
                case ARRAY_HC4R:    // Res = INT, Rht = HC4R
                case ARRAY_HC4V:    // Res = INT, Rht = HC4V
                case ARRAY_HC8I:    // Res = INT, Rht = HC8I
                case ARRAY_HC8F:    // Res = INT, Rht = HC8F
                case ARRAY_HC8R:    // Res = INT, Rht = HC8R
                case ARRAY_HC8V:    // Res = INT, Rht = HC8V
                case ARRAY_ARB:     // Res = INT, Rht = ARB
                case ARRAY_BA2F:    // Res = INT, Rht = BA2F
                case ARRAY_BA4F:    // Res = INT, Rht = BA4F
                case ARRAY_BA8F:    // Res = INT, Rht = BA8F
                    // Point to TCA routine to extract an indexed
                    //   value from the right arg
                    tcActionRht = aTypeActConvert[aplTypeRht][aplTypeRes];

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Convert to lower data type
                        (*tcActionRht) (lpMemRht, uRht, &atSub, &bRet);

                        if (!bRet)
                            goto ERROR_EXIT;

                        // Save in the result
                        *((LPAPLINT) lpMemRes)++ = atSub.aplInteger;
                    } // End FOR

////////////////case ARRAY_HC2I:        // Reduce the dimension
////////////////case ARRAY_HC4I:        // ...
////////////////case ARRAY_HC8I:        // ...
                    if (bDimDemote && FALSE)
                    {
                        DbgBrk ();

                        // Loop through the elements
                        for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPBYTE) lpMemRht) += (iSizeofRht - iSizeofRes))
                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRes; i++)
                            // Copy to the result
                            *((LPAPLINT) lpMemRes)++ = *((LPAPLINT) lpMemRht)++;
                    } // End IF

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_FLOAT:       // Res = FLOAT, Rht = FLOAT/HCxy/HETERO/NESTED
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_HETERO:  // Res = FLOAT, Rht = HETERO
                case ARRAY_NESTED:  // Res = FLOAT, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Get the SymGlb ptr
                        lpSymGlbSub = *((LPAPLHETERO) lpMemRht)++;

                        // Split cases based upon the ptr type bits
                        switch (GetPtrTypeDir (lpSymGlbSub))
                        {
                            case PTRTYPE_STCONST:
                                // Split cases based upon the immediate type
                                switch (lpSymGlbSub->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:
                                        *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) lpSymGlbSub->stData.stBoolean;

                                        break;

                                    case IMMTYPE_INT:
                                        *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) lpSymGlbSub->stData.stInteger;

                                        break;

                                    case IMMTYPE_FLOAT:
                                        *((LPAPLFLOAT) lpMemRes)++ = lpSymGlbSub->stData.stFloat;

                                        break;

                                    case IMMTYPE_CHAR:
                                    defstop
                                        break;
                                } // End FOR/SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:
                                // Lock the memory to get a ptr to it
                                lpMemHdrSub = MyGlobalLockVar (lpSymGlbSub);

                                Assert (IsScalar (lpMemHdrSub->Rank));

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Split cases based upon the storage type
                                switch (lpMemHdrSub->ArrType)
                                {
                                    case ARRAY_BOOL:    // Res = FLT , Rht = BOOL
                                    case ARRAY_INT:     // Res = FLT , Rht = INT
                                    case ARRAY_APA:     // Res = FLT , Rht = APA
                                    case ARRAY_FLOAT:   // Res = FLT , Rht = FLT
                                        *((LPAPLFLOAT) lpMemRes)++ = GetNextFloat (lpMemSub, lpMemHdrSub->ArrType, 0);

                                        break;

                                    case ARRAY_RAT:     // Res = FLT , Rht = RAT
                                    case ARRAY_VFP:     // Res = FLT , Rht = VFP
                                                        // We don't demote RAT/VFP to FLT
                                    defstop
                                        break;
                                } // End SWITCH

                                // We no longer need this ptr
                                MyGlobalUnlock (lpSymGlbSub); lpMemHdrSub = NULL;

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                case ARRAY_RAT:     // Res = FLT, Rht = RAT
                case ARRAY_VFP:     // Res = FLT, Rht = VFP
                case ARRAY_HC2I:    // Res = FLT, Rht = HC2I
                case ARRAY_HC2F:    // Res = FLT, Rht = HC2F
                case ARRAY_HC2R:    // Res = FLT, Rht = HC2R
                case ARRAY_HC2V:    // Res = FLT, Rht = HC2V
                case ARRAY_HC4I:    // Res = FLT, Rht = HC4I
                case ARRAY_HC4F:    // Res = FLT, Rht = HC4F
                case ARRAY_HC4R:    // Res = FLT, Rht = HC4R
                case ARRAY_HC4V:    // Res = FLT, Rht = HC4V
                case ARRAY_HC8I:    // Res = FLT, Rht = HC8I
                case ARRAY_HC8F:    // Res = FLT, Rht = HC8F
                case ARRAY_HC8R:    // Res = FLT, Rht = HC8R
                case ARRAY_HC8V:    // Res = FLT, Rht = HC8V
                case ARRAY_BA1F:    // Res = FLT, Rht = BA1F
                case ARRAY_BA2F:    // Res = FLT, Rht = BA2F
                case ARRAY_BA4F:    // Res = FLT, Rht = BA4F
                case ARRAY_BA8F:    // Res = FLT, Rht = BA8F
                    // Point to TCA routine to extract an indexed
                    //   value from the right arg
                    tcActionRht = aTypeActConvert[aplTypeRht][aplTypeRes];

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Convert to lower data type
                        (*tcActionRht) (lpMemRht, uRht, &atSub, &bRet);

                        if (!bRet)
                            goto ERROR_EXIT;

                        // Save in the result
                        *((LPAPLFLOAT) lpMemRes)++ = atSub.aplFloat;
                    } // End FOR

////////////////case ARRAY_HC2F:        // Reduce the dimension
////////////////case ARRAY_HC4F:        // ...
////////////////case ARRAY_HC8F:        // ...
                    if (bDimDemote && FALSE)
                    {
                        DbgBrk ();

                        // Loop through the elements
                        for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPBYTE) lpMemRht) += (iSizeofRht - iSizeofRes))
                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRes; i++)
                            // Copy to the result
                            *((LPAPLFLOAT) lpMemRes)++ = *((LPAPLFLOAT) lpMemRht)++;
                    } // End IF

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

        case ARRAY_RAT:         // Res = RAT, Rht = HCxy/HETERO/NESTED
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_HETERO:  // Res = RAT, Rht = HETERO
                case ARRAY_NESTED:  // Res = RAT, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Get the SymGlb ptr
                        lpSymGlbSub = *((LPAPLNESTED) lpMemRht)++;

                        // Split cases based upon the ptr type bits
                        switch (GetPtrTypeDir (lpSymGlbSub))
                        {
                            case PTRTYPE_STCONST:
                                // Split cases based upon the immediate storage type
                                switch (lpSymGlbSub->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:
                                        // Convert the data to RAT
                                        mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, BIT0 & lpSymGlbSub->stData.stInteger, 1);

                                        break;

                                    case IMMTYPE_INT:
                                        // Convert the data to RAT
                                        mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, lpSymGlbSub->stData.stInteger, 1);

                                        break;

                                    case IMMTYPE_FLOAT:
                                        // Convert the data to RAT
                                        mpq_init_set_d  (((LPAPLRAT) lpMemRes)++, lpSymGlbSub->stData.stFloat);

                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:
                                // Lock the memory to get a ptr to it
                                lpMemHdrSub = MyGlobalLockVar (lpSymGlbSub);

                                Assert (IsScalar (lpMemHdrSub->Rank));

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Copy the data
                                mpq_init_set (((LPAPLRAT) lpMemRes)++, (LPAPLRAT) lpMemSub);

                                // We no longer need this ptr
                                MyGlobalUnlock (lpSymGlbSub); lpMemHdrSub = NULL;

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

////////////////case ARRAY_RAT:     // Res = RAT, Rht = RAT
                case ARRAY_VFP:     // Res = RAT, Rht = VFP
                case ARRAY_HC2I:    // Res = RAT, Rht = HC2I
                case ARRAY_HC2F:    // Res = RAT, Rht = HC2F
                case ARRAY_HC2R:    // Res = RAT, Rht = HC2R
                case ARRAY_HC2V:    // Res = RAT, Rht = HC2V
                case ARRAY_HC4I:    // Res = RAT, Rht = HC4I
                case ARRAY_HC4F:    // Res = RAT, Rht = HC4F
                case ARRAY_HC4R:    // Res = RAT, Rht = HC4R
                case ARRAY_HC4V:    // Res = RAT, Rht = HC4V
                case ARRAY_HC8I:    // Res = RAT, Rht = HC8I
                case ARRAY_HC8F:    // Res = RAT, Rht = HC8F
                case ARRAY_HC8R:    // Res = RAT, Rht = HC8R
                case ARRAY_HC8V:    // Res = RAT, Rht = HC8V
                case ARRAY_BA1F:    // Res = RAT, Rht = BA1F
                case ARRAY_BA2F:    // Res = RAT, Rht = BA2F
                case ARRAY_BA4F:    // Res = RAT, Rht = BA4F
                case ARRAY_BA8F:    // Res = RAT, Rht = BA8F
                    // Point to TCA routine to extract an indexed
                    //   value from the right arg
                    tcActionRht = aTypeActConvert[aplTypeRht][aplTypeRes];

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Convert to lower data type
                        (*tcActionRht) (lpMemRht, uRht, &atSub, &bRet);

                        if (!bRet)
                            goto ERROR_EXIT;

                        // Save in the result
                        *((LPAPLRAT) lpMemRes)++ = atSub.aplRat;
                    } // End FOR

////////////////case ARRAY_HC2R:        // Reduce the dimension
////////////////case ARRAY_HC4R:        // ...
////////////////case ARRAY_HC8R:        // ...
                    if (bDimDemote && FALSE)
                    {
                        DbgBrk ();

                        // Loop through the elements
                        for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPBYTE) lpMemRht) += (iSizeofRht - iSizeofRes))
                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRes; i++)
                            // Copy to the result
                            mpq_init_set (((LPAPLRAT) lpMemRes)++, ((LPAPLRAT) lpMemRht)++);
                    } // End IF

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_VFP:         // Res = VFP, Rht = HCxy/HETERO/NESTED
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_HETERO:  // Res = VFP, Rht = HETERO
                case ARRAY_NESTED:  // Res = VFP, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Get the SymGlb ptr
                        lpSymGlbSub = *((LPAPLNESTED) lpMemRht)++;

                        // Split cases based upon the ptr type bits
                        switch (GetPtrTypeDir (lpSymGlbSub))
                        {
                            case PTRTYPE_STCONST:
                                // Split cases based upon the immediate storage type
                                switch (lpSymGlbSub->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:
                                        // Convert the data to VFP
                                        mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++, BIT0 & lpSymGlbSub->stData.stInteger, MPFR_RNDN);

                                        break;

                                    case IMMTYPE_INT:
                                        // Convert the data to VFP
                                        mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++, lpSymGlbSub->stData.stInteger, MPFR_RNDN);

                                        break;

                                    case IMMTYPE_FLOAT:
                                        // Convert the data to VFP
                                        mpfr_init_set_d  (((LPAPLVFP) lpMemRes)++, lpSymGlbSub->stData.stFloat, MPFR_RNDN);

                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:
                                // Lock the memory to get a ptr to it
                                lpMemHdrSub = MyGlobalLockVar (lpSymGlbSub);

                                Assert (IsScalar (lpMemHdrSub->Rank));

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Convert to VFP and copy the data
                                (*aTypeActConvert[lpMemHdrSub->ArrType][aplTypeRes]) (lpMemSub, 0, (LPALLTYPES) ((LPAPLVFP) lpMemRes)++, &bRet);

                                if (!bRet)
                                    goto ERROR_EXIT;

                                // We no longer need this ptr
                                MyGlobalUnlock (lpSymGlbSub); lpMemHdrSub = NULL;

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

////////////////case ARRAY_RAT:     // Res = VFP, Rht = RAT
////////////////case ARRAY_VFP:     // Res = VFP, Rht = VFP
                case ARRAY_HC2I:    // Res = VFP, Rht = HC2I
                case ARRAY_HC2F:    // Res = VFP, Rht = HC2F
                case ARRAY_HC2R:    // Res = VFP, Rht = HC2R
                case ARRAY_HC2V:    // Res = VFP, Rht = HC2V
                case ARRAY_HC4I:    // Res = VFP, Rht = HC4I
                case ARRAY_HC4F:    // Res = VFP, Rht = HC4F
                case ARRAY_HC4R:    // Res = VFP, Rht = HC4R
                case ARRAY_HC4V:    // Res = VFP, Rht = HC4V
                case ARRAY_HC8I:    // Res = VFP, Rht = HC8I
                case ARRAY_HC8F:    // Res = VFP, Rht = HC8F
                case ARRAY_HC8R:    // Res = VFP, Rht = HC8R
                case ARRAY_HC8V:    // Res = VFP, Rht = HC8V
                case ARRAY_BA1F:    // Res = VFP, Rht = BA1F
                case ARRAY_BA2F:    // Res = VFP, Rht = BA2F
                case ARRAY_BA4F:    // Res = VFP, Rht = BA4F
                case ARRAY_BA8F:    // Res = VFP, Rht = BA8F
                    // Point to TCA routine to extract an indexed
                    //   value from the right arg
                    tcActionRht = aTypeActConvert[aplTypeRht][aplTypeRes];

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Convert to lower data type
                        (*tcActionRht) (lpMemRht, uRht, &atSub, &bRet);

                        if (!bRet)
                            goto ERROR_EXIT;

                        // Save in the result
                        *((LPAPLVFP) lpMemRes)++ = atSub.aplVfp;
                    } // End FOR

////////////////case ARRAY_HC2V:        // Reduce the dimension
////////////////case ARRAY_HC4V:        // ...
////////////////case ARRAY_HC8V:        // ...
                    if (bDimDemote && FALSE)
                    {
                        DbgBrk ();

                        // Loop through the elements
                        for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPBYTE) lpMemRht) += (iSizeofRht - iSizeofRes))
                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRes; i++)
                            // Copy to the result
                            mpfr_init_set (((LPAPLVFP) lpMemRes)++, ((LPAPLVFP) lpMemRht)++, MPFR_RNDN);
                    } // End IF

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_HC2I:        // Res = HC2I, Rht = HC4I/HC8I
        case ARRAY_HC4I:        // Res = HC4I, Rht =      HC8I
        case ARRAY_HC8I:        // Res = HC8I, Rht =      HC8I
        case ARRAY_HC2F:        // Res = HC2F, Rht = HC4F/HC8F
        case ARRAY_HC4F:        // Res = HC4F, Rht =      HC8F
        case ARRAY_HC8F:        // Res = HC8F, Rht =      HC8F
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_HETERO:  // Res = INT , Rht = HETERO
                case ARRAY_NESTED:  // Res = INT , Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Get the SymGlb ptr
                        lpSymGlbSub = *((LPAPLHETERO) lpMemRht)++;

                        // Split cases based upon the ptr type bits
                        switch (GetPtrTypeDir (lpSymGlbSub))
                        {
                            case PTRTYPE_STCONST:
                                // Split cases based upon the result storage type
                                switch (aplTypeRes)
                                {
                                    case ARRAY_HC2I:
                                    case ARRAY_HC4I:
                                    case ARRAY_HC8I:
                                        // Split cases based upon the immediate type
                                        switch (lpSymGlbSub->stFlags.ImmType)
                                        {
                                            case IMMTYPE_BOOL:  // Res = HCxI, Rht = BOOL
                                                // Set the real part
                                                *((LPAPLINT) lpMemRes)++ = lpSymGlbSub->stData.stBoolean;

                                                // Skip over the imaginary parts
                                                ((LPBYTE) lpMemRes) += iSizeofRes - sizeof (APLINT);

                                                break;

                                            case IMMTYPE_INT:   // Res = HCxI, Rht = INT
                                                // Set the real part
                                                *((LPAPLINT) lpMemRes)++ = lpSymGlbSub->stData.stInteger;

                                                // Skip over the imaginary parts
                                                ((LPBYTE) lpMemRes) += iSizeofRes - sizeof (APLINT);

                                                break;

                                            case IMMTYPE_FLOAT: // Res = HCxI, Rht = FLT
                                                // Set the real part
                                                *((LPAPLINT) lpMemRes)++ = (APLINT) lpSymGlbSub->stData.stFloat;

                                                // Skip over the imaginary parts
                                                ((LPBYTE) lpMemRes) += iSizeofRes - sizeof (APLINT);

                                                break;

                                            case IMMTYPE_CHAR:  // Res = HCxI, Rht = CHAR
                                            defstop
                                                break;
                                        } // End SWITCH

                                        break;

                                    case ARRAY_HC2F:
                                    case ARRAY_HC4F:
                                    case ARRAY_HC8F:
                                        // Split cases based upon the immediate type
                                        switch (lpSymGlbSub->stFlags.ImmType)
                                        {
                                            case IMMTYPE_BOOL:  // Res = HCxF, Rht = BOOL
                                                // Set the real part
                                                *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) lpSymGlbSub->stData.stBoolean;

                                                // Skip over the imaginary parts
                                                ((LPBYTE) lpMemRes) += iSizeofRes - sizeof (APLFLOAT);

                                                break;

                                            case IMMTYPE_INT:   // Res = HCxF, Rht = INT
                                                // Set the real part
                                                *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) lpSymGlbSub->stData.stInteger;

                                                // Skip over the imaginary parts
                                                ((LPBYTE) lpMemRes) += iSizeofRes - sizeof (APLFLOAT);

                                                break;

                                            case IMMTYPE_FLOAT: // Res = HCxF, Rht = FLT
                                                // Set the real part
                                                *((LPAPLFLOAT) lpMemRes)++ = lpSymGlbSub->stData.stFloat;

                                                // Skip over the imaginary parts
                                                ((LPBYTE) lpMemRes) += iSizeofRes - sizeof (APLFLOAT);

                                                break;

                                            case IMMTYPE_CHAR:  // Res = HC2F, Rht = CHAR
                                            defstop
                                                break;
                                        } // End SWITCH

                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:
                                // Lock the memory to get a ptr to it
                                lpMemHdrSub = MyGlobalLockVar (lpSymGlbSub);

                                Assert (IsScalar (lpMemHdrSub->Rank));

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Zero the memory in case we might free it
                                ZeroMemory (&atSub, sizeof (atSub));

                                // Convert the item to the result type
                                (*aTypeActPromote[lpMemHdrSub->ArrType][aplTypeRes]) (lpMemSub, 0, &atSub);

                                // Save in the result
                                CopyMemory (lpMemRes, &atSub, iSizeofRes);

                                // Skip to the next item in the result
                                ((LPBYTE) lpMemRes) += iSizeofRes;

                                // We no longer need this ptr
                                MyGlobalUnlock (lpSymGlbSub); lpMemHdrSub = NULL;

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

////////////////case ARRAY_HC2I:
                case ARRAY_HC4I:
                case ARRAY_HC8I:
                    if (bDimDemote)
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPBYTE) lpMemRht) += (iSizeofRht - iSizeofRes))
                    {
                        // Reduce the dimension

                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRes; i++)
                            // Save in the result
                            *((LPAPLINT) lpMemRes)++ = *((LPAPLINT) lpMemRht)++;
                    } // End FOR

                    break;

////////////////case ARRAY_HC2F:
                case ARRAY_HC4F:
                case ARRAY_HC8F:
                    if (bDimDemote)
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPBYTE) lpMemRht) += (iSizeofRht - iSizeofRes))
                    {
                        // Reduce the dimension

                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRes; i++)
                            // Save in the result
                            *((LPAPLFLOAT) lpMemRes)++ = *((LPAPLFLOAT) lpMemRht)++;
                    } // End FOR

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_HC2R:        // Res = HC2R, Rht = HETERO/NESTED
        case ARRAY_HC4R:        // Res = HC4R, Rht = HETERO/NESTED
        case ARRAY_HC8R:        // Res = HC8R, Rht = HETERO/NESTED
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_HETERO:  // Res = HCxR, Rht = HETERO
                case ARRAY_NESTED:  // Res = HCxR, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Get the SymGlb ptr
                        lpSymGlbSub = *((LPAPLNESTED) lpMemRht)++;

                        // Split cases based upon the ptr type bits
                        switch (GetPtrTypeDir (lpSymGlbSub))
                        {
                            case PTRTYPE_STCONST:
                                // Split cases based upon the immediate storage type
                                switch (lpSymGlbSub->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:
                                        // Set the real part
                                        mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, BIT0 & lpSymGlbSub->stData.stInteger, 1);

                                        // Loop though the imaginary parts
                                        for (i = 1; i < iHCDimRes; i++)
                                            // Initialize to 0/1
                                            mpq_init (((LPAPLRAT) lpMemRes)++);
                                        break;

                                    case IMMTYPE_INT:
                                        // Set the real part
                                        mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, lpSymGlbSub->stData.stInteger, 1);

                                        // Loop though the imaginary parts
                                        for (i = 1; i < iHCDimRes; i++)
                                            // Initialize to 0/1
                                            mpq_init (((LPAPLRAT) lpMemRes)++);
                                        break;

                                    case IMMTYPE_FLOAT:
                                        // Set the real part
                                        mpq_init_set_d  (((LPAPLRAT) lpMemRes)++, lpSymGlbSub->stData.stFloat);

                                        // Loop though the imaginary parts
                                        for (i = 1; i < iHCDimRes; i++)
                                            // Initialize to 0/1
                                            mpq_init (((LPAPLRAT) lpMemRes)++);
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:
                                // Lock the memory to get a ptr to it
                                lpMemHdrSub = MyGlobalLockVar (lpSymGlbSub);

                                Assert (IsScalar (lpMemHdrSub->Rank));

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Zero the memory in case we might free it
                                ZeroMemory (&atSub, sizeof (atSub));

                                // Convert the item to the result type
                                (*aTypeActPromote[lpMemHdrSub->ArrType][aplTypeRes]) (lpMemSub, 0, &atSub);

                                // Save in the result
                                CopyMemory (lpMemRes, &atSub, iSizeofRes);

                                // Skip to the next item in the result
                                ((LPBYTE) lpMemRes) += iSizeofRes;

                                // We no longer need this ptr
                                MyGlobalUnlock (lpSymGlbSub); lpMemHdrSub = NULL;

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                case ARRAY_HC2R:        // Reduce the dimension
                case ARRAY_HC4R:        // ...
                case ARRAY_HC8R:        // ...
                    if (bDimDemote)
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPBYTE) lpMemRht) += (iSizeofRht - iSizeofRes))
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRes; i++)
                        // Copy to the result
                        mpq_init_set (((LPAPLRAT) lpMemRes)++, ((LPAPLRAT) lpMemRht)++);
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_HC2V:        // Res = HC2V, Rht = HETERO/NESTED
        case ARRAY_HC4V:        // Res = HC4V, Rht = HETERO/NESTED
        case ARRAY_HC8V:        // Res = HC8V, Rht = HETERO/NESTED
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_HETERO:  // Res = HCxV, Rht = HETERO
                case ARRAY_NESTED:  // Res = HCxV, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Get the SymGlb ptr
                        lpSymGlbSub = *((LPAPLNESTED) lpMemRht)++;

                        // Split cases based upon the ptr type bits
                        switch (GetPtrTypeDir (lpSymGlbSub))
                        {
                            case PTRTYPE_STCONST:
                                // Split cases based upon the immediate storage type
                                switch (lpSymGlbSub->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:
                                        // Set the real part
                                        mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++, BIT0 & lpSymGlbSub->stData.stInteger, MPFR_RNDN);

                                        // Loop though the imaginary parts
                                        for (i = 1; i < iHCDimRes; i++)
                                            // Initialize to 0
                                            mpfr_init0 (((LPAPLVFP) lpMemRes)++);
                                        break;

                                    case IMMTYPE_INT:
                                        // Set the real part
                                        mpfr_init_set_sx (((LPAPLVFP) lpMemRes)++, lpSymGlbSub->stData.stInteger, MPFR_RNDN);

                                        // Loop though the imaginary parts
                                        for (i = 1; i < iHCDimRes; i++)
                                            // Initialize to 0
                                            mpfr_init0 (((LPAPLVFP) lpMemRes)++);
                                        break;

                                    case IMMTYPE_FLOAT:
                                        // Set the real part
                                        mpfr_init_set_d  (((LPAPLVFP) lpMemRes)++, lpSymGlbSub->stData.stFloat, MPFR_RNDN);

                                        // Loop though the imaginary parts
                                        for (i = 1; i < iHCDimRes; i++)
                                            // Initialize to 0
                                            mpfr_init0 (((LPAPLVFP) lpMemRes)++);
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:
                                // Lock the memory to get a ptr to it
                                lpMemHdrSub = MyGlobalLockVar (lpSymGlbSub);

                                Assert (IsScalar (lpMemHdrSub->Rank));

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Zero the memory in case we might free it
                                ZeroMemory (&atSub, sizeof (atSub));

                                // Convert the item to the result type
                                (*aTypeActPromote[lpMemHdrSub->ArrType][aplTypeRes]) (lpMemSub, 0, &atSub);

                                // Save in the result
                                CopyMemory (lpMemRes, &atSub, iSizeofRes);

                                // Skip to the next item in the result
                                ((LPBYTE) lpMemRes) += iSizeofRes;

                                // We no longer need this ptr
                                MyGlobalUnlock (lpSymGlbSub); lpMemHdrSub = NULL;

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                case ARRAY_HC2V:        // Reduce the dimension
                case ARRAY_HC4V:        // ...
                case ARRAY_HC8V:        // ...
                    if (bDimDemote)
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPBYTE) lpMemRht) += (iSizeofRht - iSizeofRes))
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRes; i++)
                        // Copy to the result
                        mpfr_init_set (((LPAPLVFP) lpMemRes)++, ((LPAPLVFP) lpMemRht)++, MPFR_RNDN);
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_ARB:         // Res = ARB, Rht = HCxy/HETERO/NESTED
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_HETERO:  // Res = ARB, Rht = HETERO
                case ARRAY_NESTED:  // Res = ARB, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Get the SymGlb ptr
                        lpSymGlbSub = *((LPAPLNESTED) lpMemRht)++;

                        // Split cases based upon the ptr type bits
                        switch (GetPtrTypeDir (lpSymGlbSub))
                        {
                            case PTRTYPE_STCONST:
                                // Split cases based upon the immediate storage type
                                switch (lpSymGlbSub->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:
                                        // Convert the data to ARB
                                        arb_init_set_sx (((LPAPLARB) lpMemRes)++, BIT0 & lpSymGlbSub->stData.stInteger);

                                        break;

                                    case IMMTYPE_INT:
                                        // Convert the data to ARB
                                        arb_init_set_sx (((LPAPLARB) lpMemRes)++, lpSymGlbSub->stData.stInteger);

                                        break;

                                    case IMMTYPE_FLOAT:
                                        // Convert the data to ARB
                                        arb_init_set_d  (((LPAPLARB) lpMemRes)++, lpSymGlbSub->stData.stFloat);

                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:
                                // Lock the memory to get a ptr to it
                                lpMemHdrSub = MyGlobalLockVar (lpSymGlbSub);

                                Assert (IsScalar (lpMemHdrSub->Rank));

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Convert to ARB and copy the data
                                (*aTypeActConvert[lpMemHdrSub->ArrType][aplTypeRes]) (lpMemSub, 0, (LPALLTYPES) ((LPAPLARB) lpMemRes)++, &bRet);

                                if (!bRet)
                                    goto ERROR_EXIT;

                                // We no longer need this ptr
                                MyGlobalUnlock (lpSymGlbSub); lpMemHdrSub = NULL;

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

////////////////case ARRAY_RAT:     // Res = ARB, Rht = RAT
////////////////case ARRAY_VFP:     // Res = ARB, Rht = VFP
////////////////case ARRAY_ARB:     // Res = ARB, Rht = ARB
                case ARRAY_HC2I:    // Res = ARB, Rht = HC2I
                case ARRAY_HC2F:    // Res = ARB, Rht = HC2F
                case ARRAY_HC2R:    // Res = ARB, Rht = HC2R
                case ARRAY_HC2V:    // Res = ARB, Rht = HC2V
                case ARRAY_HC4I:    // Res = ARB, Rht = HC4I
                case ARRAY_HC4F:    // Res = ARB, Rht = HC4F
                case ARRAY_HC4R:    // Res = ARB, Rht = HC4R
                case ARRAY_HC4V:    // Res = ARB, Rht = HC4V
                case ARRAY_HC8I:    // Res = ARB, Rht = HC8I
                case ARRAY_HC8F:    // Res = ARB, Rht = HC8F
                case ARRAY_HC8R:    // Res = ARB, Rht = HC8R
                case ARRAY_HC8V:    // Res = ARB, Rht = HC8V
                case ARRAY_BA1F:    // Res = ARB, Rht = BA1F
                case ARRAY_BA2F:    // Res = ARB, Rht = BA2F
                case ARRAY_BA4F:    // Res = ARB, Rht = BA4F
                case ARRAY_BA8F:    // Res = ARB, Rht = BA8F
                    // Point to TCA routine to extract an indexed
                    //   value from the right arg
                    tcActionRht = aTypeActConvert[aplTypeRht][aplTypeRes];

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Convert to lower data type
                        (*tcActionRht) (lpMemRht, uRht, &atSub, &bRet);

                        if (!bRet)
                            goto ERROR_EXIT;

                        // Save in the result
                        *((LPAPLARB) lpMemRes)++ = atSub.aplArb;
                    } // End FOR

////////////////case ARRAY_HC2V:        // Reduce the dimension
////////////////case ARRAY_HC4V:        // ...
////////////////case ARRAY_HC8V:        // ...
                    if (bDimDemote && FALSE)
                    {
                        DbgBrk ();

                        // Loop through the elements
                        for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPBYTE) lpMemRht) += (iSizeofRht - iSizeofRes))
                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRes; i++)
                            // Copy to the result
                            arb_init_set (((LPAPLARB) lpMemRes)++, ((LPAPLARB) lpMemRht)++);
                    } // End IF

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_BA2F:        // Res = BA2F, Rht = HETERO/NESTED
        case ARRAY_BA4F:        // Res = BA4F, Rht = HETERO/NESTED
        case ARRAY_BA8F:        // Res = BA8F, Rht = HETERO/NESTED
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_HETERO:  // Res = BAxF, Rht = HETERO
                case ARRAY_NESTED:  // Res = BAxF, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Get the SymGlb ptr
                        lpSymGlbSub = *((LPAPLNESTED) lpMemRht)++;

                        // Split cases based upon the ptr type bits
                        switch (GetPtrTypeDir (lpSymGlbSub))
                        {
                            case PTRTYPE_STCONST:
                                // Split cases based upon the immediate storage type
                                switch (lpSymGlbSub->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:
                                        // Set the real part
                                        arb_init_set_sx (((LPAPLARB) lpMemRes)++, BIT0 & lpSymGlbSub->stData.stInteger);

                                        // Loop though the imaginary parts
                                        for (i = 1; i < iHCDimRes; i++)
                                            // Initialize to 0/0
                                            Myarb_init (((LPAPLARB) lpMemRes)++);
                                        break;

                                    case IMMTYPE_INT:
                                        // Set the real part
                                        arb_init_set_sx (((LPAPLARB) lpMemRes)++, lpSymGlbSub->stData.stInteger);

                                        // Loop though the imaginary parts
                                        for (i = 1; i < iHCDimRes; i++)
                                            // Initialize to 0/0
                                            Myarb_init (((LPAPLARB) lpMemRes)++);
                                        break;

                                    case IMMTYPE_FLOAT:
                                        // Set the real part
                                        arb_init_set_d  (((LPAPLARB) lpMemRes)++, lpSymGlbSub->stData.stFloat);

                                        // Loop though the imaginary parts
                                        for (i = 1; i < iHCDimRes; i++)
                                            // Initialize to 0/0
                                            Myarb_init (((LPAPLARB) lpMemRes)++);
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:
                                // Lock the memory to get a ptr to it
                                lpMemHdrSub = MyGlobalLockVar (lpSymGlbSub);

                                Assert (IsScalar (lpMemHdrSub->Rank));

                                // Skip over the header and dimensions to the data
                                lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                                // Zero the memory in case we might free it
                                ZeroMemory (&atSub, sizeof (atSub));

                                // Convert the item to the result type
                                (*aTypeActPromote[lpMemHdrSub->ArrType][aplTypeRes]) (lpMemSub, 0, &atSub);

                                // Save in the result
                                CopyMemory (lpMemRes, &atSub, iSizeofRes);

                                // Skip to the next item in the result
                                ((LPBYTE) lpMemRes) += iSizeofRes;

                                // We no longer need this ptr
                                MyGlobalUnlock (lpSymGlbSub); lpMemHdrSub = NULL;

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                case ARRAY_BA2F:        // Reduce the dimension
                case ARRAY_BA4F:        // ...
                case ARRAY_BA8F:        // ...
                    if (bDimDemote)
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPBYTE) lpMemRht) += (iSizeofRht - iSizeofRes))
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRes; i++)
                        // Copy to the result
                        arb_init_set (((LPAPLARB) lpMemRes)++, ((LPAPLARB) lpMemRht)++);
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH

    bRet = TRUE;
ERROR_EXIT:
    return bRet;
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

UBOOL TypePromoteGlb_EM
    (HGLOBAL *lphGlbArg,            // Ptr to global memory handle
     APLSTYPE aplTypeRes,           // The result storage type
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    HGLOBAL           hGlbArg,              // Arg    ...
                      hGlbRes = NULL;       // Result global memory handle
    UBOOL             bRet = FALSE;         // TRUE iff the result is valid
    LPVARARRAY_HEADER lpMemHdrArg = NULL,   // Ptr to arg header
                      lpMemHdrRes = NULL;   // ...    result ...
    LPVOID            lpMemArg,             // Ptr to global memory
                      lpMemRes;             // Ptr to result global memory
    APLSTYPE          aplTypeArg;           // Arg storage type of HGLOBAL
    APLNELM           aplNELMArg;           // Arg NELM         ...
    APLRANK           aplRankArg;           // Arg Rank         ...
    APLUINT           ByteRes;              // # bytes in the result
////LPSYMENTRY        lpSymTmp;             // Ptr to temporary LPSYMENTRY
    ALLTYPES          atRes = {0};          // Result as ALLTYPES
    int               i,                    // Loop counter
                      iHCDimRes;            // Result HC Dimension (1, 2, 4, 8)

    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDimRes = TranslateArrayTypeToHCDim (aplTypeRes);

    // Copy the HGLOBAL
    hGlbArg = *lphGlbArg;

    // Lock the memory to get a ptr to it
    lpMemHdrArg = MyGlobalLockVar (hGlbArg);

#define lpHeader    lpMemHdrArg
    // Get the Array Type and NELM
    aplTypeArg = lpHeader->ArrType;
    aplNELMArg = lpHeader->NELM;
    aplRankArg = lpHeader->Rank;
#undef  lpHeader

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMArg, aplRankArg);

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        // WS FULL, so no promotion
        goto NORMAL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
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
    lpMemArg = VarArrayBaseToDim (lpMemHdrArg);
    lpMemRes = VarArrayBaseToDim (lpMemHdrRes);

    // Copy the arg dimensions to the result
    CopyMemory (lpMemRes, lpMemArg, (APLU3264) BytesIn (ARRAY_INT, aplRankArg));

    // Skip over the dimensions to the data
    lpMemArg = VarArrayDataFmBase (lpMemHdrArg);
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        APLUINT   uRes;                 // Loop counter
        APLINT    apaOff,               // APA offset
                  apaMul;               // APA multiplier
        UINT      uBitMask;             // Bit mask for looping through Booleans
////    APLHETERO lpSym0,               // LPSYMENTRY for 0
////              lpSym1;               // ...            1

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

        case ARRAY_FLOAT:               // B/I/A/F -> F
            Assert (IsSimpleNum (aplTypeArg));

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

                case ARRAY_FLOAT:
                    // Loop through the arg converting values to the result
                    for (uRes = 0; uRes < aplNELMArg; uRes++)
                        *((LPAPLFLOAT) lpMemRes)++ =
                          *((LPAPLFLOAT) lpMemArg)++;
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_HETERO:              // B/I/F/C/A -> H
////        Assert (IsSimpleNH (aplTypeArg));
        case ARRAY_NESTED:              // B/I/F/C/A/H -> N
////        Assert (IsSimple   (aplTypeArg));

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
                    case ARRAY_HC2I:
                    case ARRAY_HC4I:
                    case ARRAY_HC8I:
                    case ARRAY_HC2F:
                    case ARRAY_HC4F:
                    case ARRAY_HC8F:
                        // Save a zero STE as the prototype
                        *((LPAPLHETERO) lpMemRes)++ = GetSteZero ();

                        break;

                    case ARRAY_CHAR:
                        // Save a zero STE as the prototype
                        *((LPAPLHETERO) lpMemRes)++ = GetSteBlank ();

                        break;

                    case ARRAY_RAT:
                    case ARRAY_HC2R:
                    case ARRAY_HC4R:
                    case ARRAY_HC8R:
                    case ARRAY_VFP:
                    case ARRAY_HC2V:
                    case ARRAY_HC4V:
                    case ARRAY_HC8V:
                        // Save a zero STE as the prototype
                        *((LPAPLHETERO) lpMemRes)++ =
                          MakeGlbEntry_EM (aplTypeArg,          // Array Type to use (see ARRAY_TYPES)
                 (IsAnyRat (aplTypeArg) ? (LPVOID) &mpqZero
                                        : (LPVOID) &mpfZero),   // Prt to value to use
                                           TRUE,                // TRUE iff we should initialize the target first
                                           lptkFunc);           // Ptr to token to use in case of error
                        break;

                    defstop
                        break;
                } // End SWITCH
            } else
            {
                // Loop through the arg converting values to the result
                for (uRes = 0; uRes < aplNELMArg; uRes++)
                {
                    // Zero the memory in case we might free it
                    ZeroMemory (&atRes, sizeof (atRes));

                    // Promote the arg to the result type
                    (*aTypeActPromote[aplTypeArg][aplTypeRes]) (lpMemArg, uRes, &atRes);

                    if (atRes.aplNested EQ NULL)
                        goto ERROR_EXIT;

                    // Save in the result
                    *((LPAPLNESTED) lpMemRes)++ = atRes.aplNested;
                } // End FOR
            } // End IF/ELSE

///             // Split cases based upon the arg storage type
///             switch (aplTypeArg)
///             {
///                 case ARRAY_BOOL:
///                     uBitMask = BIT0;
///                     lpSym0 = GetSteZero ();
///                     lpSym1 = GetSteOne ();
///
///                     // Loop through the arg converting values to the result
///                     for (uRes = 0; uRes < aplNELMArg; uRes++)
///                     {
///                         *((LPAPLHETERO) lpMemRes)++ =
///                           (uBitMask & *(LPAPLBOOL) lpMemArg) ? lpSym1 : lpSym0;
///
///                         // Shift over the bit mask
///                         uBitMask <<= 1;
///
///                         // Check for end-of-byte
///                         if (uBitMask EQ END_OF_BYTE)
///                         {
///                             uBitMask = BIT0;            // Start over
///                             ((LPAPLBOOL) lpMemArg)++;   // Skip to next byte
///                         } // End IF
///                     } // End FOR
///
///                     break;
///
///                 case ARRAY_INT:
///                     // Loop through the arg converting values to the result
///                     for (uRes = 0; uRes < aplNELMArg; uRes++)
///                     {
///                         *((LPAPLHETERO) lpMemRes)++ =
///                         lpSymTmp =
///                           SymTabAppendInteger_EM (*((LPAPLINT) lpMemArg)++, TRUE);
///                         if (lpSymTmp EQ NULL)
///                             goto ERROR_EXIT;
///                     } // End FOR
///
///                     break;
///
///                 case ARRAY_FLOAT:
///                     // Loop through the arg converting values to the result
///                     for (uRes = 0; uRes < aplNELMArg; uRes++)
///                     {
///                         *((LPAPLHETERO) lpMemRes)++ =
///                         lpSymTmp =
///                           SymTabAppendFloat_EM (*((LPAPLFLOAT) lpMemArg)++);
///                         if (lpSymTmp EQ NULL)
///                             goto ERROR_EXIT;
///                     } // End FOR
///
///                     break;
///
///                 case ARRAY_CHAR:
///                     // Loop through the arg converting values to the result
///                     for (uRes = 0; uRes < aplNELMArg; uRes++)
///                     {
///                         *((LPAPLHETERO) lpMemRes)++ =
///                         lpSymTmp =
///                           SymTabAppendChar_EM (*((LPAPLCHAR) lpMemArg)++, TRUE);
///                         if (lpSymTmp EQ NULL)
///                             goto ERROR_EXIT;
///                     } // End FOR
///
///                     break;
///
///                 case ARRAY_APA:
/// #define lpAPA       ((LPAPLAPA) lpMemArg)
///                     // Get the APA parameters
///                     apaOff = lpAPA->Off;
///                     apaMul = lpAPA->Mul;
/// #undef  lpAPA
///                     // Loop through the arg converting values to the result
///                     for (uRes = 0; uRes < aplNELMArg; uRes++)
///                     {
///                         *((LPAPLHETERO) lpMemRes)++ =
///                         lpSymTmp =
///                           SymTabAppendInteger_EM (apaOff + apaMul * uRes, TRUE);
///                         if (lpSymTmp EQ NULL)
///                             goto ERROR_EXIT;
///                     } // End FOR
///
///                     break;
///
///                 case ARRAY_HETERO:
///                     // Loop through the arg converting values to the result
///                     for (uRes = 0; uRes < aplNELMArg; uRes++)
///                         *((LPAPLHETERO) lpMemRes)++ =
///                           *((LPAPLHETERO) lpMemArg)++;
///                     break;
///
///                 defstop
///                     break;
///             } // End SWITCH

            break;

        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
            // Loop through the arg converting values to the result
            for (uRes = 0; uRes < aplNELMArg; uRes++)
            {
////////////////// Zero the memory in case we might free it
////////////////ZeroMemory (&atRes, sizeof (atRes));
////////////////
                // Promote to the result type
                (*aTypeActPromote[aplTypeArg][aplTypeRes]) (lpMemArg, uRes, &atRes);

                // Loop through all of the parts
                for (i = 0; i < iHCDimRes; i++)
                    *((LPAPLINT  ) lpMemRes)++ = atRes.aplHC8I.parts[i];
            } // End IF

            break;

        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
            // Loop through the arg converting values to the result
            for (uRes = 0; uRes < aplNELMArg; uRes++)
            {
////////////////// Zero the memory in case we might free it
////////////////ZeroMemory (&atRes, sizeof (atRes));
////////////////
                // Promote to the result type
                (*aTypeActPromote[aplTypeArg][aplTypeRes]) (lpMemArg, uRes, &atRes);

                // Loop through all of the parts
                for (i = 0; i < iHCDimRes; i++)
                    *((LPAPLFLOAT) lpMemRes)++ = atRes.aplHC8F.parts[i];
            } // End IF

            break;

        case ARRAY_RAT:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            // Loop through the arg converting values to the result
            for (uRes = 0; uRes < aplNELMArg; uRes++)
            {
                // Zero the memory in case we might free it
                ZeroMemory (&atRes, sizeof (atRes));

                // Promote to the result type
                (*aTypeActPromote[aplTypeArg][aplTypeRes]) (lpMemArg, uRes, &atRes);

                // Loop through all of the parts
                for (i = 0; i < iHCDimRes; i++)
                    *((LPAPLRAT  ) lpMemRes)++ = atRes.aplHC8R.parts[i];
            } // End IF

            break;

        case ARRAY_VFP:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            // Loop through the arg converting values to the result
            for (uRes = 0; uRes < aplNELMArg; uRes++)
            {
                // Zero the memory in case we might free it
                ZeroMemory (&atRes, sizeof (atRes));

                // Promote to the result type
                (*aTypeActPromote[aplTypeArg][aplTypeRes]) (lpMemArg, uRes, &atRes);

                // Loop through all of the parts
                for (i = 0; i < iHCDimRes; i++)
                    *((LPAPLVFP  ) lpMemRes)++ = atRes.aplHC8V.parts[i];
            } // End IF

            break;

        case ARRAY_ARB:
        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
            // Loop through the arg converting values to the result
            for (uRes = 0; uRes < aplNELMArg; uRes++)
            {
                // Zero the memory in case we might free it
                ZeroMemory (&atRes, sizeof (atRes));

                // Promote to the result type
                (*aTypeActPromote[aplTypeArg][aplTypeRes]) (lpMemArg, uRes, &atRes);

                // Loop through all of the parts
                for (i = 0; i < iHCDimRes; i++)
                    *((LPAPLARB  ) lpMemRes)++ = atRes.aplBA8F.parts[i];
            } // End IF

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need thess ptrs
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;

    // Free the old HGLOBAL
    FreeResultGlobalVar (hGlbArg); hGlbArg = NULL;

    // Save the new HGLOBAL
    *lphGlbArg = hGlbRes;
NORMAL_EXIT:
    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    if (hGlbArg NE NULL && lpMemHdrArg NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;
    } // End IF

    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    return bRet;
} // End TypePromoteGlb_EM


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
//      TPF Routines
//***************************************************************************

//***************************************************************************
//  $TPF_IDENT
//***************************************************************************

void TPF_IDENT
    (LPVOID  lpArg,
     APLNELM aplIndex)
{
    // No global storage to free
} // End TPF_IDENT


//***************************************************************************
//  $TPF_HETERO
//  $TPF_NESTED
//***************************************************************************

void TPF_HETERO
    (LPALLTYPES lpatArg,
     APLNELM    aplIndex)
{
    Assert (aplIndex EQ 0);

    // If it's valid, ...
    if (lpatArg->aplHetero NE NULL)
    // Split cases based upon the ptr type bits
    switch (GetPtrTypeDir (lpatArg->aplHetero))
    {
        case PTRTYPE_STCONST:
            break;

        case PTRTYPE_HGLOBAL:
            // Free the global var
            FreeResultGlobalVar (lpatArg->aplHetero);

            // Zap the save area
            lpatArg->aplHetero = NULL;

            break;

        defstop
            break;
    } // End IF/SWITCH
} // End TPF_HETERO


//***************************************************************************
//  $TPF_RAT
//***************************************************************************

void TPF_RAT
    (LPAPLRAT lpArg,
     APLNELM  aplIndex)

{
    Myq_clear (&lpArg[aplIndex]);
} // End TPF_RAT


//***************************************************************************
//  $TPF_HC2R
//***************************************************************************

void TPF_HC2R
    (LPAPLHC2R lpArg,
     APLNELM   aplIndex)

{
    Myhc2r_clear (&lpArg[aplIndex]);
} // End TPF_HC2R


//***************************************************************************
//  $TPF_HC4R
//***************************************************************************

void TPF_HC4R
    (LPAPLHC4R lpArg,
     APLNELM   aplIndex)

{
    Myhc4r_clear (&lpArg[aplIndex]);
} // End TPF_HC4R


//***************************************************************************
//  $TPF_HC8R
//***************************************************************************

void TPF_HC8R
    (LPAPLHC8R lpArg,
     APLNELM   aplIndex)

{
    Myhc8r_clear (&lpArg[aplIndex]);
} // End TPF_HC8R


//***************************************************************************
//  $TPF_VFP
//***************************************************************************

void TPF_VFP
    (LPAPLVFP lpArg,
     APLNELM   aplIndex)

{
    Myf_clear (&lpArg[aplIndex]);
} // End TPF_VFP


//***************************************************************************
//  $TPF_HC2V
//***************************************************************************

void TPF_HC2V
    (LPAPLHC2V lpArg,
     APLNELM   aplIndex)

{
    Myhc2v_clear (&lpArg[aplIndex]);
} // End TPF_HC2V


//***************************************************************************
//  $TPF_HC4V
//***************************************************************************

void TPF_HC4V
    (LPAPLHC4V lpArg,
     APLNELM   aplIndex)

{
    Myhc4v_clear (&lpArg[aplIndex]);
} // End TPF_HC4V


//***************************************************************************
//  $TPF_HC8V
//***************************************************************************

void TPF_HC8V
    (LPAPLHC8V lpArg,
     APLNELM   aplIndex)

{
    Myhc8v_clear (&lpArg[aplIndex]);
} // End TPF_HC8V


//***************************************************************************
//  $TPF_BA1F
//***************************************************************************

void TPF_BA1F
    (LPAPLBA1F lpArg,
     APLNELM   aplIndex)

{
    Myarb_clear (&lpArg[aplIndex]);
} // End TPF_BA1F


//***************************************************************************
//  $TPF_BA2F
//***************************************************************************

void TPF_BA2F
    (LPAPLBA2F lpArg,
     APLNELM   aplIndex)

{
    arb2f_clear (&lpArg[aplIndex]);
} // End TPF_BA2F


//***************************************************************************
//  $TPF_BA4F
//***************************************************************************

void TPF_BA4F
    (LPAPLBA4F lpArg,
     APLNELM   aplIndex)

{
    arb4f_clear (&lpArg[aplIndex]);
} // End TPF_BA4F


//***************************************************************************
//  $TPF_BA8F
//***************************************************************************

void TPF_BA8F
    (LPAPLBA8F lpArg,
     APLNELM   aplIndex)

{
    arb8f_clear (&lpArg[aplIndex]);
} // End TPF_BA8F


//***************************************************************************
//      TPT Routines
//***************************************************************************

//***************************************************************************
//  $TPT_ERROR
//***************************************************************************

void TPT_ERROR
    (LPTOKEN lptkArg)

{
    DbgStop ();
} // TPT_ERROR


//***************************************************************************
//  $TPT_IDENT
//***************************************************************************

void TPT_IDENT
    (LPTOKEN lptkArg)

{
} // TPT_IDENT


//***************************************************************************
//  $TPT_BOOL_INT
//***************************************************************************

void TPT_BOOL_INT
    (LPTOKEN lptkArg)

{
    lptkArg->tkData.tkInteger = BIT0 & lptkArg->tkData.tkBoolean;
    lptkArg->tkFlags.ImmType  = IMMTYPE_INT;
} // TPT_BOOL_INT


//***************************************************************************
//  $TPT_BOOL_FLT
//***************************************************************************

void TPT_BOOL_FLT
    (LPTOKEN lptkArg)

{
    lptkArg->tkData.tkFloat  = (APLFLOAT) lptkArg->tkData.tkBoolean;
    lptkArg->tkFlags.ImmType = IMMTYPE_FLOAT;
} // TPT_BOOL_FLT


//***************************************************************************
//  $TPT_BOOL_RAT
//***************************************************************************

void TPT_BOOL_RAT
    (LPTOKEN lptkArg)

{
    APLRAT aplRat = {0};

    // Convert the BOOL to a RAT
    mpq_init_set_sx (&aplRat, BIT0 & lptkArg->tkData.tkBoolean, 1);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_RAT,           // Array Type to use (see ARRAY_TYPES)
                      &aplRat,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_RAT;
} // TPT_BOOL_RAT


//***************************************************************************
//  $TPT_BOOL_VFP
//***************************************************************************

void TPT_BOOL_VFP
    (LPTOKEN lptkArg)

{
    APLVFP aplVfp = {0};

    // Convert the BOOL to a VFP
    mpfr_init_set_sx (&aplVfp, BIT0 & lptkArg->tkData.tkBoolean, MPFR_RNDN);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_VFP,           // Array Type to use (see ARRAY_TYPES)
                      &aplVfp,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_VFP;
} // TPT_BOOL_VFP


//***************************************************************************
//  $TPT_BOOL_HC2I
//***************************************************************************

void TPT_BOOL_HC2I
    (LPTOKEN lptkArg)

{
    APLHC2I aplHC2I = {0};

    // Convert the BOOL to a HC2I
    aplHC2I.parts[0] = BIT0 & lptkArg->tkData.tkBoolean;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC2I,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC2I,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC2I;
} // TPT_BOOL_HC2I


//***************************************************************************
//  $TPT_BOOL_HC2F
//***************************************************************************

void TPT_BOOL_HC2F
    (LPTOKEN lptkArg)

{
    APLHC2F aplHC2F = {0};

    // Convert the BOOL to a HC2F
    aplHC2F.parts[0] = (APLFLOAT) (BIT0 & lptkArg->tkData.tkBoolean);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC2F,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC2F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC2F;
} // TPT_BOOL_HC2F


//***************************************************************************
//  $TPT_BOOL_HC2R
//***************************************************************************

void TPT_BOOL_HC2R
    (LPTOKEN lptkArg)

{
    APLHC2R aplHC2R = {0};

    // Initialize to 0/1
    mphc2r_init (&aplHC2R);

    // Convert the BOOL to a HC2R
    mpq_set_sx (&aplHC2R.parts[0], BIT0 & lptkArg->tkData.tkBoolean, 1);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC2R,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC2R,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC2R;
} // TPT_BOOL_HC2R


//***************************************************************************
//  $TPT_BOOL_HC2V
//***************************************************************************

void TPT_BOOL_HC2V
    (LPTOKEN lptkArg)

{
    APLHC2V aplHC2V = {0};

    // Initialize to 0
    mphc2v_init0 (&aplHC2V);

    // Convert the BOOL to a HC2V
    mpfr_set_sx (&aplHC2V.parts[0], BIT0 & lptkArg->tkData.tkBoolean, MPFR_RNDN);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC2V,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC2V,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC2V;
} // TPT_BOOL_HC2V


//***************************************************************************
//  $TPT_BOOL_HC4I
//***************************************************************************

void TPT_BOOL_HC4I
    (LPTOKEN lptkArg)

{
    APLHC4I aplHC4I = {0};

    // Convert the BOOL to a HC4I
    aplHC4I.parts[0] = BIT0 & lptkArg->tkData.tkBoolean;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC4I,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC4I,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC4I;
} // TPT_BOOL_HC4I


//***************************************************************************
//  $TPT_BOOL_HC4F
//***************************************************************************

void TPT_BOOL_HC4F
    (LPTOKEN lptkArg)

{
    APLHC4F aplHC4F = {0};

    // Convert the BOOL to a HC4F
    aplHC4F.parts[0] = (APLFLOAT) (BIT0 & lptkArg->tkData.tkBoolean);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC4F,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC4F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC4F;
} // TPT_BOOL_HC4F


//***************************************************************************
//  $TPT_BOOL_HC4R
//***************************************************************************

void TPT_BOOL_HC4R
    (LPTOKEN lptkArg)

{
    APLHC4R aplHC4R = {0};

    // Initialize to 0/1
    mphc4r_init (&aplHC4R);

    // Convert the BOOL to a HC4R
    mpq_set_sx (&aplHC4R.parts[0], BIT0 & lptkArg->tkData.tkBoolean, 1);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC4R,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC4R,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC4R;
} // TPT_BOOL_HC4R


//***************************************************************************
//  $TPT_BOOL_HC4V
//***************************************************************************

void TPT_BOOL_HC4V
    (LPTOKEN lptkArg)

{
    APLHC4V aplHC4V = {0};

    // Initialize to 0
    mphc4v_init0 (&aplHC4V);

    // Convert the BOOL to a HC4V
    mpfr_set_sx (&aplHC4V.parts[0], BIT0 & lptkArg->tkData.tkBoolean, MPFR_RNDN);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC4V,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC4V,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC4V;
} // TPT_BOOL_HC4V


//***************************************************************************
//  $TPT_BOOL_HC8I
//***************************************************************************

void TPT_BOOL_HC8I
    (LPTOKEN lptkArg)

{
    APLHC8I aplHC8I = {0};

    // Convert the BOOL to a HC8I
    aplHC8I.parts[0] = BIT0 & lptkArg->tkData.tkBoolean;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC8I,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC8I,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC8I;
} // TPT_BOOL_HC8I


//***************************************************************************
//  $TPT_BOOL_HC8F
//***************************************************************************

void TPT_BOOL_HC8F
    (LPTOKEN lptkArg)

{
    APLHC8F aplHC8F = {0};

    // Convert the BOOL to a HC8F
    aplHC8F.parts[0] = (APLFLOAT) (BIT0 & lptkArg->tkData.tkBoolean);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC8F,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC8F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC8F;
} // TPT_BOOL_HC8F


//***************************************************************************
//  $TPT_BOOL_HC8R
//***************************************************************************

void TPT_BOOL_HC8R
    (LPTOKEN lptkArg)

{
    APLHC8R aplHC8R = {0};

    // Initialize to 0/1
    mphc8r_init (&aplHC8R);

    // Convert the BOOL to a HC8R
    mpq_set_sx (&aplHC8R.parts[0], BIT0 & lptkArg->tkData.tkBoolean, 1);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC8R,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC8R,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC8R;
} // TPT_BOOL_HC8R


//***************************************************************************
//  $TPT_BOOL_HC8V
//***************************************************************************

void TPT_BOOL_HC8V
    (LPTOKEN lptkArg)

{
    APLHC8V aplHC8V = {0};

    // Initialize to 0
    mphc8v_init0 (&aplHC8V);

    // Convert the BOOL to a HC8V
    mpfr_set_sx (&aplHC8V.parts[0], BIT0 & lptkArg->tkData.tkBoolean, MPFR_RNDN);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC8V,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC8V,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC8V;
} // TPT_BOOL_HC8V


//***************************************************************************
//  $TPT_BOOL_BA1F
//***************************************************************************

void TPT_BOOL_BA1F
    (LPTOKEN lptkArg)

{
    APLBA1F aplBA1F = {0};

    // Initialize to 0
    Myarb_init (&aplBA1F);

    // Convert the BOOL to a BA1F
    arb_set_si (&aplBA1F, BIT0 & lptkArg->tkData.tkBoolean);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA1F,          // Array Type to use (see ARRAY_TYPES)
                      &aplBA1F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_BA1F;
} // TPT_BOOL_BA1F


//***************************************************************************
//  $TPT_BOOL_BA2F
//***************************************************************************

void TPT_BOOL_BA2F
    (LPTOKEN lptkArg)

{
    APLBA2F aplBA2F = {0};

    // Initialize to 0
    arb2f_init (&aplBA2F);

    // Convert the BOOL to a BA2F
    arb_set_si (&aplBA2F.parts[0], BIT0 & lptkArg->tkData.tkBoolean);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA2F,          // Array Type to use (see ARRAY_TYPES)
                      &aplBA2F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_BA2F;
} // TPT_BOOL_BA2F


//***************************************************************************
//  $TPT_BOOL_BA4F
//***************************************************************************

void TPT_BOOL_BA4F
    (LPTOKEN lptkArg)

{
    APLBA4F aplBA4F = {0};

    // Initialize to 0
    arb4f_init (&aplBA4F);

    // Convert the BOOL to a BA4F
    arb_set_si (&aplBA4F.parts[0], BIT0 & lptkArg->tkData.tkBoolean);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA4F,          // Array Type to use (see ARRAY_TYPES)
                      &aplBA4F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_BA4F;
} // TPT_BOOL_BA4F


//***************************************************************************
//  $TPT_BOOL_BA8F
//***************************************************************************

void TPT_BOOL_BA8F
    (LPTOKEN lptkArg)

{
    APLBA8F aplBA8F = {0};

    // Initialize to 0
    arb8f_init (&aplBA8F);

    // Convert the BOOL to a BA8F
    arb_set_si (&aplBA8F.parts[0], BIT0 & lptkArg->tkData.tkBoolean);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA8F,          // Array Type to use (see ARRAY_TYPES)
                      &aplBA8F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_BA8F;
} // TPT_BOOL_BA8F


//***************************************************************************
//  $TPT_INT_FLT
//***************************************************************************

void TPT_INT_FLT
    (LPTOKEN lptkArg)

{
    lptkArg->tkData.tkFloat  = (APLFLOAT) lptkArg->tkData.tkInteger;
    lptkArg->tkFlags.ImmType = IMMTYPE_FLOAT;
} // TPT_INT_FLT


//***************************************************************************
//  $TPT_INT_RAT
//***************************************************************************

void TPT_INT_RAT
    (LPTOKEN lptkArg)

{
    APLRAT aplRat = {0};

    // Convert the INT to a RAT
    mpq_init_set_sx (&aplRat, lptkArg->tkData.tkInteger, 1);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_RAT,           // Array Type to use (see ARRAY_TYPES)
                      &aplRat,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_RAT;
} // TPT_INT_RAT


//***************************************************************************
//  $TPT_INT_VFP
//***************************************************************************

void TPT_INT_VFP
    (LPTOKEN lptkArg)

{
    APLVFP aplVfp = {0};

    // Convert the INT to a VFP
    mpfr_init_set_sx (&aplVfp, lptkArg->tkData.tkInteger, MPFR_RNDN);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_VFP,           // Array Type to use (see ARRAY_TYPES)
                      &aplVfp,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_VFP;
} // TPT_INT_VFP


//***************************************************************************
//  $TPT_INT_HC2I
//***************************************************************************

void TPT_INT_HC2I
    (LPTOKEN lptkArg)

{
    APLHC2I aplHC2I = {0};

    // Convert the INT to a HC2I
    aplHC2I.parts[0] = lptkArg->tkData.tkInteger;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC2I,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC2I,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC2I;
} // TPT_INT_HC2I


//***************************************************************************
//  $TPT_INT_HC2F
//***************************************************************************

void TPT_INT_HC2F
    (LPTOKEN lptkArg)

{
    APLHC2F aplHC2F = {0};

    // Convert the INT to a HC2F
    aplHC2F.parts[0] = (APLFLOAT) lptkArg->tkData.tkInteger;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC2F,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC2F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC2F;
} // TPT_INT_HC2F


//***************************************************************************
//  $TPT_INT_HC2R
//***************************************************************************

void TPT_INT_HC2R
    (LPTOKEN lptkArg)

{
    APLHC2R aplHC2R = {0};

    // Initialize to 0/1
    mphc2r_init (&aplHC2R);

    // Convert the INT to a HC2R
    mpq_set_sx (&aplHC2R.parts[0], lptkArg->tkData.tkInteger, 1);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC2R,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC2R,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC2R;
} // TPT_INT_HC2R


//***************************************************************************
//  $TPT_INT_HC2V
//***************************************************************************

void TPT_INT_HC2V
    (LPTOKEN lptkArg)

{
    APLHC2V aplHC2V = {0};

    // Initialize to 0
    mphc2v_init0 (&aplHC2V);

    // Convert the INT to a HC2V
    mpfr_set_sx (&aplHC2V.parts[0], lptkArg->tkData.tkInteger, MPFR_RNDN);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC2V,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC2V,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC2V;
} // TPT_INT_HC2V


//***************************************************************************
//  $TPT_INT_HC4I
//***************************************************************************

void TPT_INT_HC4I
    (LPTOKEN lptkArg)

{
    APLHC4I aplHC4I = {0};

    // Convert the INT to a HC4I
    aplHC4I.parts[0] = lptkArg->tkData.tkInteger;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC4I,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC4I,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC4I;
} // TPT_INT_HC4I


//***************************************************************************
//  $TPT_INT_HC4F
//***************************************************************************

void TPT_INT_HC4F
    (LPTOKEN lptkArg)

{
    APLHC4F aplHC4F = {0};

    // Convert the INT to a HC4F
    aplHC4F.parts[0] = (APLFLOAT) lptkArg->tkData.tkInteger;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC4F,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC4F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC4F;
} // TPT_INT_HC4F


//***************************************************************************
//  $TPT_INT_HC4R
//***************************************************************************

void TPT_INT_HC4R
    (LPTOKEN lptkArg)

{
    APLHC4R aplHC4R = {0};

    // Initialize to 0/1
    mphc4r_init (&aplHC4R);

    // Convert the INT to a HC4R
    mpq_set_sx (&aplHC4R.parts[0], lptkArg->tkData.tkInteger, 1);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC4R,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC4R,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC4R;
} // TPT_INT_HC4R


//***************************************************************************
//  $TPT_INT_HC4V
//***************************************************************************

void TPT_INT_HC4V
    (LPTOKEN lptkArg)

{
    APLHC4V aplHC4V = {0};

    // Initialize to 0
    mphc4v_init0 (&aplHC4V);

    // Convert the INT to a HC4V
    mpfr_set_sx (&aplHC4V.parts[0], lptkArg->tkData.tkInteger, MPFR_RNDN);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC4V,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC4V,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC4V;
} // TPT_INT_HC4V


//***************************************************************************
//  $TPT_INT_HC8I
//***************************************************************************

void TPT_INT_HC8I
    (LPTOKEN lptkArg)

{
    APLHC8I aplHC8I = {0};

    // Convert the INT to a HC8I
    aplHC8I.parts[0] = lptkArg->tkData.tkInteger;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC8I,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC8I,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC8I;
} // TPT_INT_HC8I


//***************************************************************************
//  $TPT_INT_HC8F
//***************************************************************************

void TPT_INT_HC8F
    (LPTOKEN lptkArg)

{
    APLHC8F aplHC8F = {0};

    // Convert the INT to a HC8F
    aplHC8F.parts[0] = (APLFLOAT) lptkArg->tkData.tkInteger;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC8F,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC8F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC8F;
} // TPT_INT_HC8F


//***************************************************************************
//  $TPT_INT_HC8R
//***************************************************************************

void TPT_INT_HC8R
    (LPTOKEN lptkArg)

{
    APLHC8R aplHC8R = {0};

    // Initialize to 0/1
    mphc8r_init (&aplHC8R);

    // Convert the INT to a HC8R
    mpq_set_sx (&aplHC8R.parts[0], lptkArg->tkData.tkInteger, 1);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC8R,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC8R,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC8R;
} // TPT_INT_HC8R


//***************************************************************************
//  $TPT_INT_HC8V
//***************************************************************************

void TPT_INT_HC8V
    (LPTOKEN lptkArg)

{
    APLHC8V aplHC8V = {0};

    // Initialize to 0
    mphc8v_init0 (&aplHC8V);

    // Convert the INT to a HC8V
    mpfr_set_sx (&aplHC8V.parts[0], lptkArg->tkData.tkInteger, MPFR_RNDN);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC8V,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC8V,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC8V;
} // TPT_INT_HC8V


//***************************************************************************
//  $TPT_INT_BA1F
//***************************************************************************

void TPT_INT_BA1F
    (LPTOKEN lptkArg)

{
    APLBA1F aplBA1F = {0};

    // Initialize to 0
    Myarb_init (&aplBA1F);

    // Convert the INT to a BA1F
    arb_set_sx   (&aplBA1F, lptkArg->tkData.tkInteger);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA1F,          // Array Type to use (see ARRAY_TYPES)
                      &aplBA1F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_BA1F;
} // TPT_INT_BA1F


//***************************************************************************
//  $TPT_INT_BA2F
//***************************************************************************

void TPT_INT_BA2F
    (LPTOKEN lptkArg)

{
    APLBA2F aplBA2F = {0};

    // Initialize to 0/0
    arb2f_init (&aplBA2F);

    // Convert the INT to a BA2F
    arb_set_sx (&aplBA2F.parts[0], lptkArg->tkData.tkInteger);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA2F,          // Array Type to use (see ARRAY_TYPES)
                      &aplBA2F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_BA2F;
} // TPT_INT_BA2F


//***************************************************************************
//  $TPT_INT_BA4F
//***************************************************************************

void TPT_INT_BA4F
    (LPTOKEN lptkArg)

{
    APLBA4F aplBA4F = {0};

    // Initialize to 0
    arb4f_init (&aplBA4F);

    // Convert the INT to a BA4F
    arb_set_sx (&aplBA4F.parts[0], lptkArg->tkData.tkInteger);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA4F,          // Array Type to use (see ARRAY_TYPES)
                      &aplBA4F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_BA4F;
} // TPT_INT_BA4F


//***************************************************************************
//  $TPT_INT_BA8F
//***************************************************************************

void TPT_INT_BA8F
    (LPTOKEN lptkArg)

{
    APLBA8F aplBA8F = {0};

    // Initialize to 0
    arb8f_init (&aplBA8F);

    // Convert the INT to a BA8F
    arb_set_sx (&aplBA8F.parts[0], lptkArg->tkData.tkInteger);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA8F,          // Array Type to use (see ARRAY_TYPES)
                      &aplBA8F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_BA8F;
} // TPT_INT_BA8F


//***************************************************************************
//  $TPT_INT_NEST
//***************************************************************************

void TPT_INT_NEST
    (LPTOKEN lptkArg)

{
    APLSTYPE          aplTypeArg;
    APLLONGEST        aplLongestArg;
    APLINT            aplInteger;
    HGLOBAL           hGlbArg;
    LPVARARRAY_HEADER lpMemHdrArg = NULL;
    LPVOID            lpMemArg;
    LPSYMENTRY        lpSymArg;

    // Get the attributes (Type, NELM, and Rank)
    //   of the arg
    AttrsOfToken (lptkArg, &aplTypeArg, NULL, NULL, NULL);

    // Get arg's global ptrs
    aplLongestArg = GetGlbPtrs_LOCK (lptkArg, &hGlbArg, &lpMemHdrArg);

    // If the item is immediate, ...
    if (hGlbArg EQ NULL)
        // Point to the data
        lpMemArg = &aplLongestArg;
    else
        // Point to the data
        lpMemArg = VarArrayDataFmBase (lpMemHdrArg);

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpMemArg, aplTypeArg, 0);

    // If the item is not immediate, ...
    if (hGlbArg NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;

        // We no longer need this storage
        FreeResultGlobalVar (hGlbArg); hGlbArg = NULL;
    } // End IF

    // Allocate space for a Nested array
    hGlbArg =
      AllocateGlobalArray (ARRAY_NESTED, 1, 0, NULL);
    if (hGlbArg EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);

    // Lock the memory to get a ptr to it
    lpMemHdrArg = MyGlobalLockVar (hGlbArg);

    // Skip over the header and dimentions to the data
    lpMemArg = VarArrayDataFmBase (lpMemHdrArg);

    lpSymArg                    =
    ((LPAPLNESTED) lpMemArg)[0] =
      MakeSymEntry_EM (TranslateArrayTypeToImmType (aplTypeArg),
       (LPAPLLONGEST) &aplInteger,
                       NULL);
    // We no longer need this ptr
    MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;

    if (lpSymArg EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);

    // Set the token values
    lptkArg->tkFlags.TknType  = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType  = IMMTYPE_ERROR;
    lptkArg->tkData.tkGlbData = MakePtrTypeGlb (hGlbArg);
} // TPT_INT_NEST


//***************************************************************************
//  $TPT_FLT_NEST
//***************************************************************************

void TPT_FLT_NEST
    (LPTOKEN lptkArg)

{
    APLSTYPE          aplTypeArg;
    APLLONGEST        aplLongestArg;
    APLFLOAT          aplFloat;
    HGLOBAL           hGlbArg;
    LPVARARRAY_HEADER lpMemHdrArg = NULL;
    LPVOID            lpMemArg;
    LPSYMENTRY        lpSymArg;

    // Get the attributes (Type, NELM, and Rank)
    //   of the arg
    AttrsOfToken (lptkArg, &aplTypeArg, NULL, NULL, NULL);

    // Get arg's global ptrs
    aplLongestArg = GetGlbPtrs_LOCK (lptkArg, &hGlbArg, &lpMemHdrArg);

    // If the item is immediate, ...
    if (hGlbArg EQ NULL)
        // Point to the data
        lpMemArg = &aplLongestArg;
    else
        // Point to the data
        lpMemArg = VarArrayDataFmBase (lpMemHdrArg);

    // Copy the input value in case it overlaps with the output
    aplFloat = GetNextFloat (lpMemArg, aplTypeArg, 0);

    // If the item is not immediate, ...
    if (hGlbArg NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;

        // We no longer need this storage
        FreeResultGlobalVar (hGlbArg); hGlbArg = NULL;
    } // End IF

    // Allocate space for a Nested array
    hGlbArg =
      AllocateGlobalArray (ARRAY_NESTED, 1, 0, NULL);
    if (hGlbArg EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);

    // Lock the memory to get a ptr to it
    lpMemHdrArg = MyGlobalLockVar (hGlbArg);

    // Skip over the header and dimentions to the data
    lpMemArg = VarArrayDataFmBase (lpMemHdrArg);

    lpSymArg                    =
    ((LPAPLNESTED) lpMemArg)[0] =
      MakeSymEntry_EM (TranslateArrayTypeToImmType (aplTypeArg),
       (LPAPLLONGEST) &aplFloat,
                       NULL);
    // We no longer need this ptr
    MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;

    if (lpSymArg EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);

    // Set the token values
    lptkArg->tkFlags.TknType  = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType  = IMMTYPE_ERROR;
    lptkArg->tkData.tkGlbData = MakePtrTypeGlb (hGlbArg);
} // TPT_FLT_NEST


//***************************************************************************
//  $TPT_RAT_NEST
//***************************************************************************

void TPT_RAT_NEST
    (LPTOKEN lptkArg)

{
    APLSTYPE          aplTypeArg;
    APLLONGEST        aplLongestArg;
    APLRAT            aplRat = {0};
    HGLOBAL           hGlbArg;
    LPVARARRAY_HEADER lpMemHdrArg = NULL;
    LPVOID            lpMemArg;
    LPSYMENTRY        lpSymArg;

    // Get the attributes (Type, NELM, and Rank)
    //   of the arg
    AttrsOfToken (lptkArg, &aplTypeArg, NULL, NULL, NULL);

    // Get arg's global ptrs
    aplLongestArg = GetGlbPtrs_LOCK (lptkArg, &hGlbArg, &lpMemHdrArg);

    // If the item is immediate, ...
    if (hGlbArg EQ NULL)
        // Point to the data
        lpMemArg = &aplLongestArg;
    else
        // Point to the data
        lpMemArg = VarArrayDataFmBase (lpMemHdrArg);

    // Copy the input value in case it overlaps with the output
    aplRat = GetNextRat_INIT (lpMemArg, aplTypeArg, 0);

    // If the item is not immediate, ...
    if (hGlbArg NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;

        // We no longer need this storage
        FreeResultGlobalVar (hGlbArg); hGlbArg = NULL;
    } // End IF

    // Allocate space for a Nested array
    hGlbArg =
      AllocateGlobalArray (ARRAY_NESTED, 1, 0, NULL);
    if (hGlbArg EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);

    // Lock the memory to get a ptr to it
    lpMemHdrArg = MyGlobalLockVar (hGlbArg);

    // Skip over the header and dimentions to the data
    lpMemArg = VarArrayDataFmBase (lpMemHdrArg);

    lpSymArg                    =
    ((LPAPLNESTED) lpMemArg)[0] =
      MakeGlbEntry_EM (aplTypeArg,
                      &aplRat,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // We no longer need this ptr
    MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;

    if (lpSymArg EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);

    // Set the token values
    lptkArg->tkFlags.TknType  = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType  = IMMTYPE_ERROR;
    lptkArg->tkData.tkGlbData = MakePtrTypeGlb (hGlbArg);
} // TPT_RAT_NEST


//***************************************************************************
//  $TPT_VFP_NEST
//***************************************************************************

void TPT_VFP_NEST
    (LPTOKEN lptkArg)

{
    APLSTYPE          aplTypeArg;
    APLLONGEST        aplLongestArg;
    APLVFP            aplVfp = {0};
    HGLOBAL           hGlbArg;
    LPVARARRAY_HEADER lpMemHdrArg = NULL;
    LPVOID            lpMemArg;
    LPSYMENTRY        lpSymArg;

    // Get the attributes (Type, NELM, and Rank)
    //   of the arg
    AttrsOfToken (lptkArg, &aplTypeArg, NULL, NULL, NULL);

    // Get arg's global ptrs
    aplLongestArg = GetGlbPtrs_LOCK (lptkArg, &hGlbArg, &lpMemHdrArg);

    // If the item is immediate, ...
    if (hGlbArg EQ NULL)
        // Point to the data
        lpMemArg = &aplLongestArg;
    else
        // Point to the data
        lpMemArg = VarArrayDataFmBase (lpMemHdrArg);

    // Copy the input value in case it overlaps with the output
    aplVfp = GetNextVfp_INIT (lpMemArg, aplTypeArg, 0);

    // If the item is not immediate, ...
    if (hGlbArg NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;

        // We no longer need this storage
        FreeResultGlobalVar (hGlbArg); hGlbArg = NULL;
    } // End IF

    // Allocate space for a Nested array
    hGlbArg =
      AllocateGlobalArray (ARRAY_NESTED, 1, 0, NULL);
    if (hGlbArg EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);

    // Lock the memory to get a ptr to it
    lpMemHdrArg = MyGlobalLockVar (hGlbArg);

    // Skip over the header and dimentions to the data
    lpMemArg = VarArrayDataFmBase (lpMemHdrArg);

    lpSymArg                    =
    ((LPAPLNESTED) lpMemArg)[0] =
      MakeGlbEntry_EM (aplTypeArg,
                      &aplVfp,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // We no longer need this ptr
    MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;

    if (lpSymArg EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);

    // Set the token values
    lptkArg->tkFlags.TknType  = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType  = IMMTYPE_ERROR;
    lptkArg->tkData.tkGlbData = MakePtrTypeGlb (hGlbArg);
} // TPT_VFP_NEST


//***************************************************************************
//  $TPT_BA1F_NEST
//***************************************************************************

void TPT_BA1F_NEST
    (LPTOKEN lptkArg)

{
    TPT_BAxF_NEST (lptkArg, 1);
} // End TPT_BA1F_NEST


//***************************************************************************
//  $TPT_BA2F_NEST
//***************************************************************************

void TPT_BA2F_NEST
    (LPTOKEN lptkArg)

{
    TPT_BAxF_NEST (lptkArg, 2);
} // End TPT_BA2F_NEST


//***************************************************************************
//  $TPT_BA4F_NEST
//***************************************************************************

void TPT_BA4F_NEST
    (LPTOKEN lptkArg)

{
    TPT_BAxF_NEST (lptkArg, 4);
} // End TPT_BA4F_NEST


//***************************************************************************
//  $TPT_BA8F_NEST
//***************************************************************************

void TPT_BA8F_NEST
    (LPTOKEN lptkArg)

{
    TPT_BAxF_NEST (lptkArg, 8);
} // End TPT_BA8F_NEST


//***************************************************************************
//  $TPT_BAxF_NEST
//***************************************************************************

void TPT_BAxF_NEST
    (LPTOKEN lptkArg,       // Ptr to token to promote
     int     iHCDim)        // HC dimension (1, 2, 4, 8)

{
    APLSTYPE          aplTypeArg;
    APLLONGEST        aplLongestArg;
    APLBA8F           aplArb = {0};
    HGLOBAL           hGlbArg;
    LPVARARRAY_HEADER lpMemHdrArg = NULL;
    LPVOID            lpMemArg;
    LPSYMENTRY        lpSymArg;

    // Get the attributes (Type, NELM, and Rank)
    //   of the arg
    AttrsOfToken (lptkArg, &aplTypeArg, NULL, NULL, NULL);

    // Get arg's global ptrs
    aplLongestArg = GetGlbPtrs_LOCK (lptkArg, &hGlbArg, &lpMemHdrArg);

    // If the item is immediate, ...
    if (hGlbArg EQ NULL)
        // Point to the data
        lpMemArg = &aplLongestArg;
    else
        // Point to the data
        lpMemArg = VarArrayDataFmBase (lpMemHdrArg);

    // Copy the input value in case it overlaps with the output
    aplArb = GetNextArb_INIT (lpMemArg, aplTypeArg, 0, iHCDim);

    // If the item is not immediate, ...
    if (hGlbArg NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;

        // We no longer need this storage
        FreeResultGlobalVar (hGlbArg); hGlbArg = NULL;
    } // End IF

    // Allocate space for a Nested array
    hGlbArg =
      AllocateGlobalArray (ARRAY_NESTED, 1, 0, NULL);
    if (hGlbArg EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);

    // Lock the memory to get a ptr to it
    lpMemHdrArg = MyGlobalLockVar (hGlbArg);

    // Skip over the header and dimentions to the data
    lpMemArg = VarArrayDataFmBase (lpMemHdrArg);

    lpSymArg                    =
    ((LPAPLNESTED) lpMemArg)[0] =
      MakeGlbEntry_EM (aplTypeArg,
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // We no longer need this ptr
    MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;

    if (lpSymArg EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);

    // Set the token values
    lptkArg->tkFlags.TknType  = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType  = IMMTYPE_ERROR;
    lptkArg->tkData.tkGlbData = MakePtrTypeGlb (hGlbArg);
} // TPT_BAxF_NEST


//***************************************************************************
//  $TPT_FLT_VFP
//***************************************************************************

void TPT_FLT_VFP
    (LPTOKEN lptkArg)

{
    APLVFP aplVfp = {0};

    // Convert the FLT to a VFP
    mpfr_init_set_d  (&aplVfp, lptkArg->tkData.tkFloat, MPFR_RNDN);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_VFP,           // Array Type to use (see ARRAY_TYPES)
                      &aplVfp,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_VFP;
} // TPT_FLT_VFP


//***************************************************************************
//  $TPT_FLT_HC2F
//***************************************************************************

void TPT_FLT_HC2F
    (LPTOKEN lptkArg)

{
    APLHC2F aplHC2F = {0};

    // Convert the FLT to a HC2F
    aplHC2F.parts[0] = lptkArg->tkData.tkFloat;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC2F,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC2F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC2F;
} // TPT_FLT_HC2F


//***************************************************************************
//  $TPT_FLT_HC2V
//***************************************************************************

void TPT_FLT_HC2V
    (LPTOKEN lptkArg)

{
    APLHC2V aplHC2V = {0};

    // Initialize to 0
    mphc2v_init0 (&aplHC2V);

    // Convert the FLT to a HC2V
    mpfr_set_d  (&aplHC2V.parts[0], lptkArg->tkData.tkFloat, MPFR_RNDN);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC2V,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC2V,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC2V;
} // TPT_FLT_HC2V


//***************************************************************************
//  $TPT_FLT_HC4F
//***************************************************************************

void TPT_FLT_HC4F
    (LPTOKEN lptkArg)

{
    APLHC4F aplHC4F = {0};

    // Convert the FLT to a HC4F
    aplHC4F.parts[0] = lptkArg->tkData.tkFloat;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC4F,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC4F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC4F;
} // TPT_FLT_HC4F


//***************************************************************************
//  $TPT_FLT_HC4V
//***************************************************************************

void TPT_FLT_HC4V
    (LPTOKEN lptkArg)

{
    APLHC4V aplHC4V = {0};

    // Initialize to 0
    mphc4v_init0 (&aplHC4V);

    // Convert the FLT to a HC4V
    mpfr_set_d  (&aplHC4V.parts[0], lptkArg->tkData.tkFloat, MPFR_RNDN);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC4V,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC4V,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC4V;
} // TPT_FLT_HC4V


//***************************************************************************
//  $TPT_FLT_HC8F
//***************************************************************************

void TPT_FLT_HC8F
    (LPTOKEN lptkArg)

{
    APLHC8F aplHC8F = {0};

    // Convert the FLT to a HC8F
    aplHC8F.parts[0] = lptkArg->tkData.tkFloat;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC8F,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC8F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array (not immediate)
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC8F;
} // TPT_FLT_HC8F


//***************************************************************************
//  $TPT_FLT_HC8V
//***************************************************************************

void TPT_FLT_HC8V
    (LPTOKEN lptkArg)

{
    APLHC8V aplHC8V = {0};

    // Initialize to 0
    mphc8v_init0 (&aplHC8V);

    // Convert the FLT to a HC8V
    mpfr_set_d  (&aplHC8V.parts[0], lptkArg->tkData.tkFloat, MPFR_RNDN);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC8V,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC8V,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC8V;
} // TPT_FLT_HC8V


//***************************************************************************
//  $TPT_FLT_BA1F
//***************************************************************************

void TPT_FLT_BA1F
    (LPTOKEN lptkArg)

{
    APLBA1F aplBA1F = {0};

    // Initialize to 0/0
    Myarb_init (&aplBA1F);

    // Convert the FLT to a BA1F
    arb_set_d  (&aplBA1F, lptkArg->tkData.tkFloat);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA1F,          // Array Type to use (see ARRAY_TYPES)
                      &aplBA1F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_BA1F;
} // TPT_FLT_BA1F


//***************************************************************************
//  $TPT_FLT_BA2F
//***************************************************************************

void TPT_FLT_BA2F
    (LPTOKEN lptkArg)

{
    APLBA2F aplBA2F = {0};

    // Initialize to 0/0
    arb2f_init (&aplBA2F);

    // Convert the FLT to a BA2F
    arb_set_d  (&aplBA2F.parts[0], lptkArg->tkData.tkFloat);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA2F,          // Array Type to use (see ARRAY_TYPES)
                      &aplBA2F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_BA2F;
} // TPT_FLT_BA2F


//***************************************************************************
//  $TPT_FLT_BA4F
//***************************************************************************

void TPT_FLT_BA4F
    (LPTOKEN lptkArg)

{
    APLBA4F aplBA4F = {0};

    // Initialize to 0/0
    arb4f_init (&aplBA4F);

    // Convert the FLT to a BA4F
    arb_set_d  (&aplBA4F.parts[0], lptkArg->tkData.tkFloat);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA4F,          // Array Type to use (see ARRAY_TYPES)
                      &aplBA4F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_BA4F;
} // TPT_FLT_BA4F


//***************************************************************************
//  $TPT_FLT_BA8F
//***************************************************************************

void TPT_FLT_BA8F
    (LPTOKEN lptkArg)

{
    APLBA8F aplBA8F = {0};

    // Initialize to 0/0
    arb8f_init (&aplBA8F);

    // Convert the FLT to a BA8F
    arb_set_d  (&aplBA8F.parts[0], lptkArg->tkData.tkFloat);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA8F,          // Array Type to use (see ARRAY_TYPES)
                      &aplBA8F,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_BA8F;
} // TPT_FLT_BA8F


//***************************************************************************
//  $TPT_APA_INT
//***************************************************************************

void TPT_APA_INT
    (LPTOKEN lptkArg)

{
    lptkArg->tkData.tkInteger = GetNextInteger (&lptkArg->tkData.tkAPA, ARRAY_APA, 0);
    lptkArg->tkFlags.ImmType  = IMMTYPE_INT;
} // TPT_APA_INT


//***************************************************************************
//  $TPT_APA_FLT
//***************************************************************************

void TPT_APA_FLT
    (LPTOKEN lptkArg)

{
    lptkArg->tkData.tkFloat  = GetNextFloat (&lptkArg->tkData.tkAPA, ARRAY_APA, 0);
    lptkArg->tkFlags.ImmType = IMMTYPE_FLOAT;
} // TPT_APA_FLT


//***************************************************************************
//  $TPT_APA_RAT
//***************************************************************************

void TPT_APA_RAT
    (LPTOKEN lptkArg)

{
    APLRAT aplRat = {0};

    // Convert the APA to a RAT
    mpq_init_set_sx (&aplRat, GetNextInteger (&lptkArg->tkData.tkAPA, ARRAY_APA, 0), 1);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_RAT,           // Array Type to use (see ARRAY_TYPES)
                      &aplRat,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_RAT;
} // TPT_APA_RAT


//***************************************************************************
//  $TPT_APA_VFP
//***************************************************************************

void TPT_APA_VFP
    (LPTOKEN lptkArg)

{
    APLVFP aplVfp = {0};

    // Convert the APA to a VFP
    mpfr_init_set_sx (&aplVfp, GetNextInteger (&lptkArg->tkData.tkAPA, ARRAY_APA, 0), MPFR_RNDN);

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_VFP,           // Array Type to use (see ARRAY_TYPES)
                      &aplVfp,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_VFP;
} // TPT_APA_VFP


//***************************************************************************
//  $TPT_APA_BA1F
//***************************************************************************

void TPT_APA_BA1F
    (LPTOKEN lptkArg)

{
    APLARB aplArb = {0};

    // Convert the APA to a ARB
    arb_init_set_sx (&aplArb, GetNextInteger (&lptkArg->tkData.tkAPA, ARRAY_APA, 0));

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_ARB,           // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_ARB;
} // TPT_APA_BA1F


//***************************************************************************
//  $TPT_APA_BA2F
//***************************************************************************

void TPT_APA_BA2F
    (LPTOKEN lptkArg)

{
    APLBA2F aplArb = {0};

    // Initialize to 0/0
    arb2f_init (&aplArb);

    // Convert the APA to a VFP
    arb_set_sx (&aplArb.parts[0], GetNextInteger (&lptkArg->tkData.tkAPA, ARRAY_APA, 0));

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA2F,          // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_BA2F;
} // TPT_APA_BA2F


//***************************************************************************
//  $TPT_APA_BA4F
//***************************************************************************

void TPT_APA_BA4F
    (LPTOKEN lptkArg)

{
    APLBA4F aplArb = {0};

    // Initialize to 0/0
    arb4f_init (&aplArb);

    // Convert the APA to a VFP
    arb_set_sx (&aplArb.parts[0], GetNextInteger (&lptkArg->tkData.tkAPA, ARRAY_APA, 0));

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA4F,          // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_BA4F;
} // TPT_APA_BA4F


//***************************************************************************
//  $TPT_APA_BA8F
//***************************************************************************

void TPT_APA_BA8F
    (LPTOKEN lptkArg)

{
    APLBA8F aplArb = {0};

    // Initialize to 0/0
    arb8f_init (&aplArb);

    // Convert the APA to a VFP
    arb_set_sx (&aplArb.parts[0], GetNextInteger (&lptkArg->tkData.tkAPA, ARRAY_APA, 0));

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA8F,          // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_BA8F;
} // TPT_APA_BA8F


//***************************************************************************
//  $TPT_RAT_VFP
//***************************************************************************

void TPT_RAT_VFP
    (LPTOKEN lptkArg)

{
    HGLOBAL           hGlbRat;
    LPVARARRAY_HEADER lpMemHdrRat = NULL;   // Ptr to var header
    LPAPLRAT          lpMemRat;
    APLVFP            aplVfp;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Get the global memory handle
    hGlbRat = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrRat = MyGlobalLockVar (hGlbRat);

    Assert (IsScalar (lpMemHdrRat->Rank));

    // Skip over the header and dimensions to the data
    lpMemRat = VarArrayDataFmBase (lpMemHdrRat);

    // Convert the RAT to a VFP
    mpfr_init_set_q (&aplVfp, lpMemRat, MPFR_RNDN);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRat); lpMemHdrRat = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbRat); hGlbRat = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_VFP,           // Array Type to use (see ARRAY_TYPES)
                      &aplVfp,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_VFP;
} // TPT_RAT_VFP


//***************************************************************************
//  $TPT_RAT_HC2R
//***************************************************************************

void TPT_RAT_HC2R
    (LPTOKEN lptkArg)

{
    APLHC2R           aplHC2R = {0};
    HGLOBAL           hGlbRat;
    LPVARARRAY_HEADER lpMemHdrRat = NULL;   // Ptr to var header
    LPAPLRAT          lpMemRat;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Initialize to 0/1
    mphc2r_init (&aplHC2R);

    // Get the global memory handle
    hGlbRat = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrRat = MyGlobalLockVar (hGlbRat);

    Assert (IsScalar (lpMemHdrRat->Rank));

    // Skip over the header and dimensions to the data
    lpMemRat = VarArrayDataFmBase (lpMemHdrRat);

    // Convert the RAT to a HC2R
    mpq_set (&aplHC2R.parts[0], lpMemRat);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRat); lpMemHdrRat = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbRat); hGlbRat = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC2R,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC2R,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC2R;
} // TPT_RAT_HC2R


//***************************************************************************
//  $TPT_RAT_HC2V
//***************************************************************************

void TPT_RAT_HC2V
    (LPTOKEN lptkArg)

{
    APLHC2V           aplHC2V = {0};
    HGLOBAL           hGlbRat;
    LPVARARRAY_HEADER lpMemHdrRat = NULL;   // Ptr to var header
    LPAPLRAT          lpMemRat;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Initialize to 0
    mphc2v_init0 (&aplHC2V);

    // Get the global memory handle
    hGlbRat = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrRat = MyGlobalLockVar (hGlbRat);

    Assert (IsScalar (lpMemHdrRat->Rank));

    // Skip over the header and dimensions to the data
    lpMemRat = VarArrayDataFmBase (lpMemHdrRat);

    // Convert the RAT to a HC2V
    mpfr_set_q (&aplHC2V.parts[0], lpMemRat, MPFR_RNDN);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRat); lpMemHdrRat = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbRat); hGlbRat = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC2V,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC2V,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_HC2V;
} // TPT_RAT_HC2V


//***************************************************************************
//  $TPT_RAT_HC4R
//***************************************************************************

void TPT_RAT_HC4R
    (LPTOKEN lptkArg)

{
    APLHC4R           aplHC4R = {0};
    HGLOBAL           hGlbRat;
    LPVARARRAY_HEADER lpMemHdrRat = NULL;   // Ptr to var header
    LPAPLRAT          lpMemRat;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Initialize to 0/1
    mphc4r_init (&aplHC4R);

    // Get the global memory handle
    hGlbRat = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrRat = MyGlobalLockVar (hGlbRat);

    Assert (IsScalar (lpMemHdrRat->Rank));

    // Skip over the header and dimensions to the data
    lpMemRat = VarArrayDataFmBase (lpMemHdrRat);

    // Convert the RAT to a HC4R
    mpq_set (&aplHC4R.parts[0], lpMemRat);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRat); lpMemHdrRat = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbRat); hGlbRat = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC4R,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC4R,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC4R;
} // TPT_RAT_HC4R


//***************************************************************************
//  $TPT_RAT_HC4V
//***************************************************************************

void TPT_RAT_HC4V
    (LPTOKEN lptkArg)

{
    APLHC4V           aplHC4V = {0};
    HGLOBAL           hGlbRat;
    LPVARARRAY_HEADER lpMemHdrRat = NULL;   // Ptr to var header
    LPAPLRAT          lpMemRat;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Initialize to 0
    mphc4v_init0 (&aplHC4V);

    // Get the global memory handle
    hGlbRat = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrRat = MyGlobalLockVar (hGlbRat);

    Assert (IsScalar (lpMemHdrRat->Rank));

    // Skip over the header and dimensions to the data
    lpMemRat = VarArrayDataFmBase (lpMemHdrRat);

    // Convert the RAT to a HC4V
    mpfr_set_q (&aplHC4V.parts[0], lpMemRat, MPFR_RNDN);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRat); lpMemHdrRat = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbRat); hGlbRat = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC4V,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC4V,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_HC4V;
} // TPT_RAT_HC4V


//***************************************************************************
//  $TPT_RAT_HC8R
//***************************************************************************

void TPT_RAT_HC8R
    (LPTOKEN lptkArg)

{
    APLHC8R           aplHC8R = {0};
    HGLOBAL           hGlbRat;
    LPVARARRAY_HEADER lpMemHdrRat = NULL;   // Ptr to var header
    LPAPLRAT          lpMemRat;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Initialize to 0/1
    mphc8r_init (&aplHC8R);

    // Get the global memory handle
    hGlbRat = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrRat = MyGlobalLockVar (hGlbRat);

    Assert (IsScalar (lpMemHdrRat->Rank));

    // Skip over the header and dimensions to the data
    lpMemRat = VarArrayDataFmBase (lpMemHdrRat);

    // Convert the RAT to a HC8R
    mpq_set (&aplHC8R.parts[0], lpMemRat);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRat); lpMemHdrRat = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbRat); hGlbRat = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC8R,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC8R,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    // Mark as an array
    lptkArg->tkFlags.TknType = TKT_VARARRAY;
    lptkArg->tkFlags.ImmType = IMMTYPE_HC8R;
} // TPT_RAT_HC8R


//***************************************************************************
//  $TPT_RAT_HC8V
//***************************************************************************

void TPT_RAT_HC8V
    (LPTOKEN lptkArg)

{
    APLHC8V           aplHC8V = {0};
    HGLOBAL           hGlbRat;
    LPVARARRAY_HEADER lpMemHdrRat = NULL;   // Ptr to var header
    LPAPLRAT          lpMemRat;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Initialize to 0
    mphc8v_init0 (&aplHC8V);

    // Get the global memory handle
    hGlbRat = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrRat = MyGlobalLockVar (hGlbRat);

    Assert (IsScalar (lpMemHdrRat->Rank));

    // Skip over the header and dimensions to the data
    lpMemRat = VarArrayDataFmBase (lpMemHdrRat);

    // Convert the RAT to a HC8V
    mpfr_set_q (&aplHC8V.parts[0], lpMemRat, MPFR_RNDN);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRat); lpMemHdrRat = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbRat); hGlbRat = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC8V,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC8V,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_HC8V;
} // TPT_RAT_HC8V


//***************************************************************************
//  $TPT_RAT_BA1F
//***************************************************************************

void TPT_RAT_BA1F
    (LPTOKEN lptkArg)

{
    HGLOBAL           hGlbRat;
    LPVARARRAY_HEADER lpMemHdrRat = NULL;   // Ptr to var header
    LPAPLRAT          lpMemRat;
    APLARB            aplArb;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Get the global memory handle
    hGlbRat = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrRat = MyGlobalLockVar (hGlbRat);

    Assert (IsScalar (lpMemHdrRat->Rank));

    // Skip over the header and dimensions to the data
    lpMemRat = VarArrayDataFmBase (lpMemHdrRat);

    // Initialize to 0/0
    Myarb_init  (&aplArb);

    // Convert the RAT to an ARB
    arb_set_q    (&aplArb, lpMemRat);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRat); lpMemHdrRat = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbRat); hGlbRat = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_ARB,           // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_ARB;
} // TPT_RAT_BA1F


//***************************************************************************
//  $TPT_RAT_BA2F
//***************************************************************************

void TPT_RAT_BA2F
    (LPTOKEN lptkArg)

{
    HGLOBAL           hGlbRat;
    LPVARARRAY_HEADER lpMemHdrRat = NULL;   // Ptr to var header
    LPAPLRAT          lpMemRat;
    APLBA2F           aplArb;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Get the global memory handle
    hGlbRat = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrRat = MyGlobalLockVar (hGlbRat);

    Assert (IsScalar (lpMemHdrRat->Rank));

    // Skip over the header and dimensions to the data
    lpMemRat = VarArrayDataFmBase (lpMemHdrRat);

    // Initialize to 0/0
    arb2f_init (&aplArb);

    // Convert the RAT to an ARB
    arb_set_q    (&aplArb.parts[0], lpMemRat);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRat); lpMemHdrRat = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbRat); hGlbRat = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA2F,          // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_BA2F;
} // TPT_RAT_BA2F


//***************************************************************************
//  $TPT_RAT_BA4F
//***************************************************************************

void TPT_RAT_BA4F
    (LPTOKEN lptkArg)

{
    HGLOBAL           hGlbRat;
    LPVARARRAY_HEADER lpMemHdrRat = NULL;   // Ptr to var header
    LPAPLRAT          lpMemRat;
    APLBA4F           aplArb;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Get the global memory handle
    hGlbRat = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrRat = MyGlobalLockVar (hGlbRat);

    Assert (IsScalar (lpMemHdrRat->Rank));

    // Skip over the header and dimensions to the data
    lpMemRat = VarArrayDataFmBase (lpMemHdrRat);

    // Initialize to 0/0
    arb4f_init (&aplArb);

    // Convert the RAT to an ARB
    arb_set_q    (&aplArb.parts[0], lpMemRat);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRat); lpMemHdrRat = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbRat); hGlbRat = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA4F,          // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_BA4F;
} // TPT_RAT_BA4F


//***************************************************************************
//  $TPT_RAT_BA8F
//***************************************************************************

void TPT_RAT_BA8F
    (LPTOKEN lptkArg)

{
    HGLOBAL           hGlbRat;
    LPVARARRAY_HEADER lpMemHdrRat = NULL;   // Ptr to var header
    LPAPLRAT          lpMemRat;
    APLBA8F           aplArb;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Get the global memory handle
    hGlbRat = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrRat = MyGlobalLockVar (hGlbRat);

    Assert (IsScalar (lpMemHdrRat->Rank));

    // Skip over the header and dimensions to the data
    lpMemRat = VarArrayDataFmBase (lpMemHdrRat);

    // Initialize to 0/0
    arb8f_init (&aplArb);

    // Convert the RAT to an ARB
    arb_set_q    (&aplArb.parts[0], lpMemRat);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRat); lpMemHdrRat = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbRat); hGlbRat = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA8F,          // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_BA8F;
} // TPT_RAT_BA8F


//***************************************************************************
//  $TPT_VFP_HC2V
//***************************************************************************

void TPT_VFP_HC2V
    (LPTOKEN lptkArg)

{
    APLHC2V           aplHC2V = {0};
    HGLOBAL           hGlbVfp;
    LPVARARRAY_HEADER lpMemHdrVfp = NULL;   // Ptr to var header
    LPAPLVFP          lpMemVfp;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Initialize to 0
    mphc2v_init0 (&aplHC2V);

    // Get the global memory handle
    hGlbVfp = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrVfp = MyGlobalLockVar (hGlbVfp);

    Assert (IsScalar (lpMemHdrVfp->Rank));

    // Skip over the header and dimensions to the data
    lpMemVfp = VarArrayDataFmBase (lpMemHdrVfp);

    // Convert the VFP to a HC2V
    mpfr_set (&aplHC2V.parts[0], lpMemVfp, MPFR_RNDN);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbVfp); lpMemHdrVfp = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbVfp); hGlbVfp = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC2V,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC2V,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_HC2V;
} // TPT_VFP_HC2V


//***************************************************************************
//  $TPT_VFP_HC4V
//***************************************************************************

void TPT_VFP_HC4V
    (LPTOKEN lptkArg)

{
    APLHC4V           aplHC4V = {0};
    HGLOBAL           hGlbVfp;
    LPVARARRAY_HEADER lpMemHdrVfp = NULL;   // Ptr to var header
    LPAPLVFP          lpMemVfp;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Initialize to 0
    mphc4v_init0 (&aplHC4V);

    // Get the global memory handle
    hGlbVfp = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrVfp = MyGlobalLockVar (hGlbVfp);

    Assert (IsScalar (lpMemHdrVfp->Rank));

    // Skip over the header and dimensions to the data
    lpMemVfp = VarArrayDataFmBase (lpMemHdrVfp);

    // Convert the VFP to a HC4V
    mpfr_set (&aplHC4V.parts[0], lpMemVfp, MPFR_RNDN);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbVfp); lpMemHdrVfp = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbVfp); hGlbVfp = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC4V,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC4V,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_HC4V;
} // TPT_VFP_HC4V


//***************************************************************************
//  $TPT_VFP_HC8V
//***************************************************************************

void TPT_VFP_HC8V
    (LPTOKEN lptkArg)

{
    APLHC8V           aplHC8V = {0};
    HGLOBAL           hGlbVfp;
    LPVARARRAY_HEADER lpMemHdrVfp = NULL;   // Ptr to var header
    LPAPLVFP          lpMemVfp;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Initialize to 0
    mphc8v_init0 (&aplHC8V);

    // Get the global memory handle
    hGlbVfp = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrVfp = MyGlobalLockVar (hGlbVfp);

    Assert (IsScalar (lpMemHdrVfp->Rank));

    // Skip over the header and dimensions to the data
    lpMemVfp = VarArrayDataFmBase (lpMemHdrVfp);

    // Convert the VFP to a HC8V
    mpfr_set (&aplHC8V.parts[0], lpMemVfp, MPFR_RNDN);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbVfp); lpMemHdrVfp = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbVfp); hGlbVfp = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_HC8V,          // Array Type to use (see ARRAY_TYPES)
                      &aplHC8V,             // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_HC8V;
} // TPT_VFP_HC8V


//***************************************************************************
//  $TPT_VFP_BA1F
//***************************************************************************

void TPT_VFP_BA1F
    (LPTOKEN lptkArg)

{
    HGLOBAL           hGlbVfp;
    LPVARARRAY_HEADER lpMemHdrVfp = NULL;   // Ptr to var header
    LPAPLVFP          lpMemVfp;
    APLARB            aplArb;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Get the global memory handle
    hGlbVfp = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrVfp = MyGlobalLockVar (hGlbVfp);

    Assert (IsScalar (lpMemHdrVfp->Rank));

    // Skip over the header and dimensions to the data
    lpMemVfp = VarArrayDataFmBase (lpMemHdrVfp);

    // Initialize to 0/0
    Myarb_init  (&aplArb);

    // Convert the VFP to an ARB
    arb_set_mpfr (&aplArb, lpMemVfp);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbVfp); lpMemHdrVfp = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbVfp); hGlbVfp = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_ARB,           // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_ARB;
} // TPT_VFP_BA1F


//***************************************************************************
//  $TPT_VFP_BA2F
//***************************************************************************

void TPT_VFP_BA2F
    (LPTOKEN lptkArg)

{
    HGLOBAL           hGlbVfp;
    LPVARARRAY_HEADER lpMemHdrVfp = NULL;   // Ptr to var header
    LPAPLVFP          lpMemVfp;
    APLBA2F           aplArb;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Get the global memory handle
    hGlbVfp = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrVfp = MyGlobalLockVar (hGlbVfp);

    Assert (IsScalar (lpMemHdrVfp->Rank));

    // Skip over the header and dimensions to the data
    lpMemVfp = VarArrayDataFmBase (lpMemHdrVfp);

    // Initialize to 0/0
    arb2f_init  (&aplArb);

    // Convert the VFP to an ARB
    arb_set_mpfr (&aplArb.parts[0], lpMemVfp);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbVfp); lpMemHdrVfp = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbVfp); hGlbVfp = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA2F,          // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_BA2F;
} // TPT_VFP_BA2F


//***************************************************************************
//  $TPT_VFP_BA4F
//***************************************************************************

void TPT_VFP_BA4F
    (LPTOKEN lptkArg)

{
    HGLOBAL           hGlbVfp;
    LPVARARRAY_HEADER lpMemHdrVfp = NULL;   // Ptr to var header
    LPAPLVFP          lpMemVfp;
    APLBA4F           aplArb;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Get the global memory handle
    hGlbVfp = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrVfp = MyGlobalLockVar (hGlbVfp);

    Assert (IsScalar (lpMemHdrVfp->Rank));

    // Skip over the header and dimensions to the data
    lpMemVfp = VarArrayDataFmBase (lpMemHdrVfp);

    // Initialize to 0/0
    arb4f_init  (&aplArb);

    // Convert the VFP to an ARB
    arb_set_mpfr (&aplArb.parts[0], lpMemVfp);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbVfp); lpMemHdrVfp = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbVfp); hGlbVfp = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA4F,          // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_BA4F;
} // TPT_VFP_BA4F


//***************************************************************************
//  $TPT_VFP_BA8F
//***************************************************************************

void TPT_VFP_BA8F
    (LPTOKEN lptkArg)

{
    HGLOBAL           hGlbVfp;
    LPVARARRAY_HEADER lpMemHdrVfp = NULL;   // Ptr to var header
    LPAPLVFP          lpMemVfp;
    APLBA8F           aplArb;

    Assert (lptkArg->tkFlags.TknType EQ TKT_VARARRAY);
    Assert (GetPtrTypeDir (lptkArg->tkData.tkGlbData) EQ PTRTYPE_HGLOBAL);

    // Get the global memory handle
    hGlbVfp = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrVfp = MyGlobalLockVar (hGlbVfp);

    Assert (IsScalar (lpMemHdrVfp->Rank));

    // Skip over the header and dimensions to the data
    lpMemVfp = VarArrayDataFmBase (lpMemHdrVfp);

    // Initialize to 0/0
    arb8f_init  (&aplArb);

    // Convert the VFP to an ARB
    arb_set_mpfr (&aplArb.parts[0], lpMemVfp);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbVfp); lpMemHdrVfp = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbVfp); hGlbVfp = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA8F,          // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_BA8F;
} // TPT_VFP_BA8F


//***************************************************************************
//  $TPT_BA1F_BA2F
//***************************************************************************

void TPT_BA1F_BA2F
    (LPTOKEN lptkArg)

{
    APLBA2F           aplArb;
    HGLOBAL           hGlbArb;
    LPVARARRAY_HEADER lpMemHdrArb = NULL;   // Ptr to var header
    LPAPLARB          lpMemArb;

    // Initialize to 0/0
    arb2f_init (&aplArb);

    // Get the global memory handle
    hGlbArb = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrArb = MyGlobalLockVar (hGlbArb);

    Assert (IsScalar (lpMemHdrArb->Rank));

    // Skip over the header and dimensions to the data
    lpMemArb = VarArrayDataFmBase (lpMemHdrArb);

    // Copy the previous value
    arb_set (&aplArb.parts[0], lpMemArb);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbArb); lpMemHdrArb = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbArb); hGlbArb = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA2F,          // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_BA2F;
} // TPT_BA1F_BA2F


//***************************************************************************
//  $TPT_BA1F_BA4F
//***************************************************************************

void TPT_BA1F_BA4F
    (LPTOKEN lptkArg)

{
    APLBA4F           aplArb;
    HGLOBAL           hGlbArb;
    LPVARARRAY_HEADER lpMemHdrArb = NULL;   // Ptr to var header
    LPAPLARB          lpMemArb;

    // Initialize to 0/0
    arb4f_init (&aplArb);

    // Get the global memory handle
    hGlbArb = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrArb = MyGlobalLockVar (hGlbArb);

    Assert (IsScalar (lpMemHdrArb->Rank));

    // Skip over the header and dimensions to the data
    lpMemArb = VarArrayDataFmBase (lpMemHdrArb);

    // Copy the previous value
    arb_set (&aplArb.parts[0], lpMemArb);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbArb); lpMemHdrArb = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbArb); hGlbArb = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA4F,          // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_BA4F;
} // TPT_BA1F_BA4F


//***************************************************************************
//  $TPT_BA1F_BA8F
//***************************************************************************

void TPT_BA1F_BA8F
    (LPTOKEN lptkArg)

{
    APLBA8F           aplArb;
    HGLOBAL           hGlbArb;
    LPVARARRAY_HEADER lpMemHdrArb = NULL;   // Ptr to var header
    LPAPLARB          lpMemArb;

    // Initialize to 0/0
    arb8f_init (&aplArb);

    // Get the global memory handle
    hGlbArb = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrArb = MyGlobalLockVar (hGlbArb);

    Assert (IsScalar (lpMemHdrArb->Rank));

    // Skip over the header and dimensions to the data
    lpMemArb = VarArrayDataFmBase (lpMemHdrArb);

    // Copy the previous value
    arb_set (&aplArb.parts[0], lpMemArb);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbArb); lpMemHdrArb = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbArb); hGlbArb = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA8F,          // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_BA8F;
} // TPT_BA1F_BA8F


//***************************************************************************
//  $TPT_BA2F_BA4F
//***************************************************************************

void TPT_BA2F_BA4F
    (LPTOKEN lptkArg)

{
    APLBA4F           aplArb;
    HGLOBAL           hGlbArb;
    LPVARARRAY_HEADER lpMemHdrArb = NULL;   // Ptr to var header
    LPAPLBA2F         lpMemArb;

    // Initialize to 0/0
    arb4f_init (&aplArb);

    // Get the global memory handle
    hGlbArb = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrArb = MyGlobalLockVar (hGlbArb);

    Assert (IsScalar (lpMemHdrArb->Rank));

    // Skip over the header and dimensions to the data
    lpMemArb = VarArrayDataFmBase (lpMemHdrArb);

    // Copy the previous value
    arb2f_set (&aplArb.partsLo, lpMemArb);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbArb); lpMemHdrArb = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbArb); hGlbArb = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA4F,          // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_BA4F;
} // TPT_BA2F_BA4F


//***************************************************************************
//  $TPT_BA2F_BA8F
//***************************************************************************

void TPT_BA2F_BA8F
    (LPTOKEN lptkArg)

{
    APLBA8F           aplArb;
    HGLOBAL           hGlbArb;
    LPVARARRAY_HEADER lpMemHdrArb = NULL;   // Ptr to var header
    LPAPLBA2F         lpMemArb;

    // Initialize to 0/0
    arb8f_init (&aplArb);

    // Get the global memory handle
    hGlbArb = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrArb = MyGlobalLockVar (hGlbArb);

    Assert (IsScalar (lpMemHdrArb->Rank));

    // Skip over the header and dimensions to the data
    lpMemArb = VarArrayDataFmBase (lpMemHdrArb);

    // Copy the previous value
    arb2f_set (&aplArb.partsLo.partsLo, lpMemArb);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbArb); lpMemHdrArb = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbArb); hGlbArb = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA8F,          // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_BA8F;
} // TPT_BA2F_BA8F


//***************************************************************************
//  $TPT_BA4F_BA8F
//***************************************************************************

void TPT_BA4F_BA8F
    (LPTOKEN lptkArg)

{
    APLBA8F           aplArb;
    HGLOBAL           hGlbArb;
    LPVARARRAY_HEADER lpMemHdrArb = NULL;   // Ptr to var header
    LPAPLBA4F         lpMemArb;

    // Initialize to 0/0
    arb8f_init (&aplArb);

    // Get the global memory handle
    hGlbArb = lptkArg->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrArb = MyGlobalLockVar (hGlbArb);

    Assert (IsScalar (lpMemHdrArb->Rank));

    // Skip over the header and dimensions to the data
    lpMemArb = VarArrayDataFmBase (lpMemHdrArb);

    // Copy the previous value
    arb4f_set (&aplArb.partsLo, lpMemArb);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbArb); lpMemHdrArb = NULL;

    // We no longer need this storage
    FreeResultGlobalVar (hGlbArb); hGlbArb = NULL;

    // Make a global numeric entry of it
    lptkArg->tkData.tkGlbData =
      MakeGlbEntry_EM (ARRAY_BA8F,          // Array Type to use (see ARRAY_TYPES)
                      &aplArb,              // Ptr to value to use
                       FALSE,               // TRUE iff we should initialize the target first
                       lptkArg);            // Ptr to token to use in case of error
    // Check for errors
    if (lptkArg->tkData.tkGlbData EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
    lptkArg->tkFlags.ImmType = IMMTYPE_BA8F;
} // TPT_BA4F_BA8F


//***************************************************************************
//      TPA Routines
//***************************************************************************

//***************************************************************************
//  $TPA_ERROR
//***************************************************************************

void TPA_ERROR
    (LPVOID      lpaplAny,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    DbgStop ();
} // TPA_ERROR


//***************************************************************************
//  $TPA_BOOL_BOOL
//***************************************************************************

void TPA_BOOL_BOOL
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplInteger = GetNextBoolean (lpaplBoolean, uInt);
} // TPA_BOOL_BOOL


//***************************************************************************
//  $TPA_BOOL_INT
//***************************************************************************

void TPA_BOOL_INT
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplInteger = GetNextBoolean (lpaplBoolean, uInt);
} // TPA_BOOL_INT


//***************************************************************************
//  $TPA_BOOL_FLT
//***************************************************************************

void TPA_BOOL_FLT
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplFloat = (APLFLOAT) GetNextBoolean (lpaplBoolean, uInt);
} // TPA_BOOL_FLT


//***************************************************************************
//  $TPA_BOOL_HETE
//***************************************************************************

void TPA_BOOL_HETE
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextBoolean (lpaplBoolean, uInt);

    lpAllTypes->aplHetero =
      MakeSymEntry_EM (IMMTYPE_BOOL,
       (LPAPLLONGEST) &aplInteger,
                       NULL);
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_BOOL_HETE


//***************************************************************************
//  $TPA_BOOL_NEST
//***************************************************************************

void TPA_BOOL_NEST
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextBoolean (lpaplBoolean, uInt);

    lpAllTypes->aplNested =
      MakeSymEntry_EM (IMMTYPE_BOOL,
       (LPAPLLONGEST) &aplInteger,
                       NULL);
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_BOOL_NEST


//***************************************************************************
//  $TPA_BOOL_RAT
//***************************************************************************

void TPA_BOOL_RAT
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextBoolean (lpaplBoolean, uInt);

    // Initialize the result to 0/1
    Myq_init (&lpAllTypes->aplRat);

    mpq_set_sx (&lpAllTypes->aplRat, aplInteger, 1);
} // TPA_BOOL_RAT


//***************************************************************************
//  $TPA_BOOL_VFP
//***************************************************************************

void TPA_BOOL_VFP
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextBoolean (lpaplBoolean, uInt);

    // Initialize the result to NaN
    Myf_init (&lpAllTypes->aplVfp);

    mpfr_set_sx (&lpAllTypes->aplVfp, aplInteger, MPFR_RNDN);
} // TPA_BOOL_VFP


//***************************************************************************
//  $TPA_INT_INT
//***************************************************************************

void TPA_INT_INT
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplInteger = lpaplInteger[uInt];
} // TPA_INT_INT


//***************************************************************************
//  $TPA_INT_FLT
//***************************************************************************

void TPA_INT_FLT
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // No need to copy the input as it is the same size as the output

    lpAllTypes->aplFloat = (APLFLOAT) lpaplInteger[uInt];
} // TPA_INT_FLT


//***************************************************************************
//  $TPA_INT_HETE
//***************************************************************************

void TPA_INT_HETE
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = lpaplInteger[uInt];

    lpAllTypes->aplHetero =
      MakeSymEntry_EM (IMMTYPE_INT,
       (LPAPLLONGEST) &aplInteger,
                       NULL);
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_INT_HETE


//***************************************************************************
//  $TPA_INT_NEST
//***************************************************************************

void TPA_INT_NEST
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = lpaplInteger[uInt];

    lpAllTypes->aplNested =
      MakeSymEntry_EM (IMMTYPE_INT,
       (LPAPLLONGEST) &aplInteger,
                       NULL);
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_INT_NEST


//***************************************************************************
//  $TPA_INT_RAT
//***************************************************************************

void TPA_INT_RAT
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = lpaplInteger[uInt];

    // Initialize the result to 0/1
    Myq_init (&lpAllTypes->aplRat);

    mpq_set_sx (&lpAllTypes->aplRat, aplInteger, 1);
} // TPA_INT_RAT


//***************************************************************************
//  $TPA_INT_VFP
//***************************************************************************

void TPA_INT_VFP
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = lpaplInteger[uInt];

    // Initialize the result to NaN
    Myf_init (&lpAllTypes->aplVfp);

    mpfr_set_sx (&lpAllTypes->aplVfp, aplInteger, MPFR_RNDN);
} // TPA_INT_VFP


//***************************************************************************
//  $TPA_FLT_FLT
//***************************************************************************

void TPA_FLT_FLT
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // No need to copy the input as it is the same size as the output

    lpAllTypes->aplFloat = lpaplFloat[uInt];
} // TPA_FLT_FLT


//***************************************************************************
//  $TPA_FLT_HETE
//***************************************************************************

void TPA_FLT_HETE
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLFLOAT aplFloat;

    // Copy the input value in case it overlaps with the output
    aplFloat = lpaplFloat[uInt];

    lpAllTypes->aplHetero =
      MakeSymEntry_EM (IMMTYPE_FLOAT,
       (LPAPLLONGEST) &aplFloat,
                       NULL);
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_FLT_HETE


//***************************************************************************
//  $TPA_FLT_NEST
//***************************************************************************

void TPA_FLT_NEST
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLFLOAT aplFloat;

    // Copy the input value in case it overlaps with the output
    aplFloat = lpaplFloat[uInt];

    lpAllTypes->aplNested =
      MakeSymEntry_EM (IMMTYPE_FLOAT,
       (LPAPLLONGEST) &aplFloat,
                       NULL);
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_FLT_NEST


//***************************************************************************
//  $TPA_FLT_VFP
//***************************************************************************

void TPA_FLT_VFP
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLFLOAT aplFloat;

    // Copy the input value in case it overlaps with the output
    aplFloat = lpaplFloat[uInt];

    // Initialize the result to NaN
    Myf_init (&lpAllTypes->aplVfp);

    mpfr_set_d  (&lpAllTypes->aplVfp, aplFloat, MPFR_RNDN);
} // TPA_FLT_VFP


//***************************************************************************
//  $TPA_CHAR_CHAR
//***************************************************************************

void TPA_CHAR_CHAR
    (LPAPLCHAR   lpaplChar,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // No need to copy the input as it is the same size as the output

    lpAllTypes->aplChar = lpaplChar[uInt];
} // TPA_CHAR_CHAR


//***************************************************************************
//  $TPA_CHAR_HETE
//***************************************************************************

void TPA_CHAR_HETE
    (LPAPLCHAR   lpaplChar,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLCHAR aplChar;

    // Copy the input value in case it overlaps with the output
    aplChar = lpaplChar[uInt];

    lpAllTypes->aplHetero =
      MakeSymEntry_EM (IMMTYPE_CHAR,
       (LPAPLLONGEST) &aplChar,
                       NULL);
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_CHAR_HETE


//***************************************************************************
//  $TPA_CHAR_NEST
//***************************************************************************

void TPA_CHAR_NEST
    (LPAPLCHAR   lpaplChar,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLCHAR aplChar;

    // Copy the input value in case it overlaps with the output
    aplChar = lpaplChar[uInt];

    lpAllTypes->aplNested =
      MakeSymEntry_EM (IMMTYPE_CHAR,
       (LPAPLLONGEST) &aplChar,
                       NULL);
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_CHAR_NEST


//***************************************************************************
//  $TPA_HETE_NEST
//***************************************************************************

void TPA_HETE_NEST
    (LPAPLHETERO lpaplHetero,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // Copy the HETERO to NESTED
    lpAllTypes->aplNested = CopySymGlbDir_PTB (((LPAPLHETERO) ClrPtrTypeDir (lpaplHetero))[uInt]);
} // TPA_HETE_NEST


//***************************************************************************
//  $TPA_APA_INT
//***************************************************************************

void TPA_APA_INT
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // No need to copy the input as it is larger than the output

    lpAllTypes->aplInteger = GetNextInteger (lpaplAPA, ARRAY_APA, uInt);
} // TPA_APA_INT


//***************************************************************************
//  $TPA_APA_FLT
//***************************************************************************

void TPA_APA_FLT
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // No need to copy the input as it is larger than the output

    lpAllTypes->aplFloat = GetNextFloat (lpaplAPA, ARRAY_APA, uInt);
} // TPA_APA_FLT


//***************************************************************************
//  $TPA_APA_HETE
//***************************************************************************

void TPA_APA_HETE
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplAPA, ARRAY_APA, uInt);

    lpAllTypes->aplHetero =
      MakeSymEntry_EM (IMMTYPE_INT,
       (LPAPLLONGEST) &aplInteger,
                       NULL);
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_APA_HETE


//***************************************************************************
//  $TPA_APA_NEST
//***************************************************************************

void TPA_APA_NEST
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplAPA, ARRAY_APA, uInt);

    lpAllTypes->aplNested =
      MakeSymEntry_EM (IMMTYPE_INT,
       (LPAPLLONGEST) &aplInteger,
                       NULL);
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_APA_NEST


//***************************************************************************
//  $TPA_APA_RAT
//***************************************************************************

void TPA_APA_RAT
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplAPA, ARRAY_APA, uInt);

    // Initialize the result to 0/1
    Myq_init (&lpAllTypes->aplRat);

    mpq_set_sx (&lpAllTypes->aplRat, aplInteger, 1);
} // TPA_APA_RAT


//***************************************************************************
//  $TPA_APA_VFP
//***************************************************************************

void TPA_APA_VFP
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplAPA, ARRAY_APA, uInt);

    // Initialize the result to NaN
    Myf_init (&lpAllTypes->aplVfp);

    mpfr_set_sx (&lpAllTypes->aplVfp, aplInteger, MPFR_RNDN);
} // TPA_APA_VFP


//***************************************************************************
//  $TPA_RAT_VFP
//***************************************************************************

void TPA_RAT_VFP
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLRAT aplRat;

    // Copy the input value in case it overlaps with the output
    aplRat = lpaplRat[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplRat)
        DbgStop ();
#endif

    // Initialize the result to NaN
    Myf_init (&lpAllTypes->aplVfp);

    mpfr_set_q  (&lpAllTypes->aplVfp, &aplRat, MPFR_RNDN);
} // TPA_RAT_VFP


//***************************************************************************
//  $TPA_HETE_HETE
//***************************************************************************

void TPA_HETE_HETE
    (LPAPLHETERO lpaplHetero,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // Copy the HETERO to HETERO
    lpAllTypes->aplNested = CopySymGlbDir_PTB (((LPAPLHETERO) ClrPtrTypeDir (lpaplHetero))[uInt]);
} // TPA_HETE_HETE


//***************************************************************************
//  $TPA_NEST_NEST
//***************************************************************************

void TPA_NEST_NEST
    (LPAPLNESTED lpaplNested,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // Copy the NESTED to NESTED
    lpAllTypes->aplNested = CopySymGlbDir_PTB (((LPAPLNESTED) ClrPtrTypeDir (lpaplNested))[uInt]);
} // TPA_NEST_NEST


//***************************************************************************
//  $TPA_RAT_HETE
//***************************************************************************

void TPA_RAT_HETE
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_RAT,       // Entry type
                      &lpaplRat[uInt],  // Ptr to the value
                       TRUE,            // TRUE iff we should initialize the target first
                       NULL);           // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_RAT_HETE


//***************************************************************************
//  $TPA_RAT_NEST
//***************************************************************************

void TPA_RAT_NEST
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_RAT,       // Entry type
                      &lpaplRat[uInt],  // Ptr to the value
                       TRUE,            // TRUE iff we should initialize the target first
                       NULL);           // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_RAT_NEST


//***************************************************************************
//  $TPA_RAT_RAT
//***************************************************************************

void TPA_RAT_RAT
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLRAT aplRat;

    // Copy the input value in case it overlaps with the output
    aplRat = lpaplRat[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplRat)
        DbgStop ();
#endif

    // Initialize the result to 0/1
    Myq_init (&lpAllTypes->aplRat);

    mpq_set (&lpAllTypes->aplRat, &aplRat);
} // TPA_RAT_RAT


//***************************************************************************
//  $TPA_VFP_HETE
//***************************************************************************

void TPA_VFP_HETE
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_VFP,       // Entry type
                      &lpaplVfp[uInt],  // Ptr to the value
                       TRUE,            // TRUE iff we should initialize the target first
                       NULL);           // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_VFP_HETE


//***************************************************************************
//  $TPA_VFP_NEST
//***************************************************************************

void TPA_VFP_NEST
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_VFP,       // Entry type
                      &lpaplVfp[uInt],  // Ptr to the value
                       TRUE,            // TRUE iff we should initialize the target first
                       NULL);           // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_VFP_NEST


//***************************************************************************
//  $TPA_VFP_VFP
//***************************************************************************

void TPA_VFP_VFP
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLVFP aplVfp;

    // Copy the input value in case it overlaps with the output
    aplVfp = lpaplVfp[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplVfp)
        DbgStop ();
#endif

    // Initialize the result to NaN
    Myf_init (&lpAllTypes->aplVfp);

    mpfr_copy (&lpAllTypes->aplVfp, &aplVfp);
} // TPA_VFP_VFP


//***************************************************************************
//  $TPA_BOOL_HC2R
//***************************************************************************

void TPA_BOOL_HC2R
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextBoolean (lpaplBoolean, uInt);

    // Initialize the result to 0/1
    mphc2r_init (&lpAllTypes->aplHC2R);

    mpq_set_sx (&lpAllTypes->aplRat, aplInteger, 1);
} // TPA_BOOL_HC2R


//***************************************************************************
//  $TPA_BOOL_HC2V
//***************************************************************************

void TPA_BOOL_HC2V
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextBoolean (lpaplBoolean, uInt);

    // Initialize the result to 0
    mphc2v_init0 (&lpAllTypes->aplHC2V);

    mpfr_set_sx (&lpAllTypes->aplVfp, aplInteger, MPFR_RNDN);
} // TPA_BOOL_HC2V


//***************************************************************************
//  $TPA_BOOL_HC4R
//***************************************************************************

void TPA_BOOL_HC4R
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextBoolean (lpaplBoolean, uInt);

    // Initialize the result to 0/1
    mphc4r_init (&lpAllTypes->aplHC4R);

    mpq_set_sx (&lpAllTypes->aplRat, aplInteger, 1);
} // TPA_BOOL_HC4R


//***************************************************************************
//  $TPA_BOOL_HC4V
//***************************************************************************

void TPA_BOOL_HC4V
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextBoolean (lpaplBoolean, uInt);

    // Initialize the result to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    mpfr_set_sx (&lpAllTypes->aplVfp, aplInteger, MPFR_RNDN);
} // TPA_BOOL_HC4V


//***************************************************************************
//  $TPA_BOOL_HC8R
//***************************************************************************

void TPA_BOOL_HC8R
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextBoolean (lpaplBoolean, uInt);

    // Initialize the result to 0/1
    mphc8r_init (&lpAllTypes->aplHC8R);

    mpq_set_sx (&lpAllTypes->aplRat, aplInteger, 1);
} // TPA_BOOL_HC8R


//***************************************************************************
//  $TPA_BOOL_HC8V
//***************************************************************************

void TPA_BOOL_HC8V
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextBoolean (lpaplBoolean, uInt);

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    mpfr_set_sx (&lpAllTypes->aplVfp, aplInteger, MPFR_RNDN);
} // TPA_BOOL_HC8V


//***************************************************************************
//  $TPA_INT_HC2R
//***************************************************************************

void TPA_INT_HC2R
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = lpaplInteger[uInt];

    // Initialize the result to 0/1
    mphc2r_init (&lpAllTypes->aplHC2R);

    mpq_set_sx (&lpAllTypes->aplRat, aplInteger, 1);
} // TPA_INT_HC2R


//***************************************************************************
//  $TPA_INT_HC2V
//***************************************************************************

void TPA_INT_HC2V
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = lpaplInteger[uInt];

    // Initialize the result to 0
    mphc2v_init0 (&lpAllTypes->aplHC2V);

    mpfr_set_sx (&lpAllTypes->aplVfp, aplInteger, MPFR_RNDN);
} // TPA_INT_HC2V


//***************************************************************************
//  $TPA_INT_HC4R
//***************************************************************************

void TPA_INT_HC4R
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = lpaplInteger[uInt];

    // Initialize the result to 0/1
    mphc4r_init (&lpAllTypes->aplHC4R);

    mpq_set_sx (&lpAllTypes->aplRat, aplInteger, 1);
} // TPA_INT_HC4R


//***************************************************************************
//  $TPA_INT_HC4V
//***************************************************************************

void TPA_INT_HC4V
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = lpaplInteger[uInt];

    // Initialize the result to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    mpfr_set_sx (&lpAllTypes->aplVfp, aplInteger, MPFR_RNDN);
} // TPA_INT_HC4V


//***************************************************************************
//  $TPA_INT_HC8R
//***************************************************************************

void TPA_INT_HC8R
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = lpaplInteger[uInt];

    // Initialize the result to 0/1
    mphc8r_init (&lpAllTypes->aplHC8R);

    mpq_set_sx (&lpAllTypes->aplRat, aplInteger, 1);
} // TPA_INT_HC8R


//***************************************************************************
//  $TPA_INT_HC8V
//***************************************************************************

void TPA_INT_HC8V
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = lpaplInteger[uInt];

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    mpfr_set_sx (&lpAllTypes->aplVfp, aplInteger, MPFR_RNDN);
} // TPA_INT_HC8V


//***************************************************************************
//  $TPA_FLT_HC2V
//***************************************************************************

void TPA_FLT_HC2V
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLFLOAT aplFloat;

    // Copy the input value in case it overlaps with the output
    aplFloat = lpaplFloat[uInt];

    // Initialize the result to 0
    mphc2v_init0 (&lpAllTypes->aplHC2V);

    mpfr_set_d  (&lpAllTypes->aplVfp, aplFloat, MPFR_RNDN);
} // TPA_FLT_HC2V


//***************************************************************************
//  $TPA_FLT_HC4V
//***************************************************************************

void TPA_FLT_HC4V
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLFLOAT aplFloat;

    // Copy the input value in case it overlaps with the output
    aplFloat = lpaplFloat[uInt];

    // Initialize the result to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    mpfr_set_d  (&lpAllTypes->aplVfp, aplFloat, MPFR_RNDN);
} // TPA_FLT_HC4V


//***************************************************************************
//  $TPA_FLT_HC8V
//***************************************************************************

void TPA_FLT_HC8V
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLFLOAT aplFloat;

    // Copy the input value in case it overlaps with the output
    aplFloat = lpaplFloat[uInt];

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    mpfr_set_d  (&lpAllTypes->aplVfp, aplFloat, MPFR_RNDN);
} // TPA_FLT_HC8V


//***************************************************************************
//  $TPA_APA_HC2R
//***************************************************************************

void TPA_APA_HC2R
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplAPA, ARRAY_APA, uInt);

    // Initialize the result to 0/1
    mphc2r_init (&lpAllTypes->aplHC2R);

    mpq_set_sx (&lpAllTypes->aplHC2R.parts[0], aplInteger, 1);
} // TPA_APA_HC2R


//***************************************************************************
//  $TPA_APA_HC2V
//***************************************************************************

void TPA_APA_HC2V
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplAPA, ARRAY_APA, uInt);

    // Initialize the result to 0
    mphc2v_init0 (&lpAllTypes->aplHC2V);

    mpfr_set_sx (&lpAllTypes->aplVfp, aplInteger, MPFR_RNDN);
} // TPA_APA_HC2V


//***************************************************************************
//  $TPA_APA_HC4R
//***************************************************************************

void TPA_APA_HC4R
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplAPA, ARRAY_APA, uInt);

    // Initialize the result to 0/1
    mphc4r_init (&lpAllTypes->aplHC4R);

    mpq_set_sx (&lpAllTypes->aplRat, aplInteger, 1);
} // TPA_APA_HC4R


//***************************************************************************
//  $TPA_APA_HC4V
//***************************************************************************

void TPA_APA_HC4V
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplAPA, ARRAY_APA, uInt);

    // Initialize the result to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    mpfr_set_sx (&lpAllTypes->aplVfp, aplInteger, MPFR_RNDN);
} // TPA_APA_HC4V


//***************************************************************************
//  $TPA_APA_HC8R
//***************************************************************************

void TPA_APA_HC8R
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplAPA, ARRAY_APA, uInt);

    // Initialize the result to 0/1
    mphc8r_init (&lpAllTypes->aplHC8R);

    mpq_set_sx (&lpAllTypes->aplRat, aplInteger, 1);
} // TPA_APA_HC8R


//***************************************************************************
//  $TPA_APA_HC8V
//***************************************************************************

void TPA_APA_HC8V
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLINT aplInteger;

    // Copy the input value in case it overlaps with the output
    aplInteger = GetNextInteger (lpaplAPA, ARRAY_APA, uInt);

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    mpfr_set_sx (&lpAllTypes->aplVfp, aplInteger, MPFR_RNDN);
} // TPA_APA_HC8V


//***************************************************************************
//  $TPA_RAT_HC2R
//***************************************************************************

void TPA_RAT_HC2R
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLRAT aplRat;

    // Copy the input value in case it overlaps with the output
    aplRat = lpaplRat[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplRat)
        DbgStop ();
#endif

    // Initialize the result to 0/1
    mphc2r_init (&lpAllTypes->aplHC2R);

    // Set the common parts to RAT & HC2R
    mpq_set (&lpAllTypes->aplRat, &aplRat);
} // TPA_RAT_HC2R


//***************************************************************************
//  $TPA_RAT_HC2V
//***************************************************************************

void TPA_RAT_HC2V
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLRAT aplRat;

    // Copy the input value in case it overlaps with the output
    aplRat = lpaplRat[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplRat)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc2v_init0 (&lpAllTypes->aplHC2V);

    // Set the common parts to RAT & HC2V
    mpfr_set_q  (&lpAllTypes->aplVfp, &aplRat, MPFR_RNDN);
} // TPA_RAT_HC2V


//***************************************************************************
//  $TPA_RAT_HC4R
//***************************************************************************

void TPA_RAT_HC4R
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLRAT aplRat;

    // Copy the input value in case it overlaps with the output
    aplRat = lpaplRat[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplRat)
        DbgStop ();
#endif

    // Initialize the result to 0/1
    mphc4r_init (&lpAllTypes->aplHC4R);

    // Set the common parts to RAT & HC4R
    mpq_set (&lpAllTypes->aplRat, &aplRat);
} // TPA_RAT_HC4R


//***************************************************************************
//  $TPA_RAT_HC4V
//***************************************************************************

void TPA_RAT_HC4V
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLRAT aplRat;

    // Copy the input value in case it overlaps with the output
    aplRat = lpaplRat[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplRat)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    // Set the common parts to RAT & HC4V
    mpfr_set_q  (&lpAllTypes->aplVfp, &aplRat, MPFR_RNDN);
} // TPA_RAT_HC4V


//***************************************************************************
//  $TPA_RAT_HC8R
//***************************************************************************

void TPA_RAT_HC8R
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLRAT aplRat;

    // Copy the input value in case it overlaps with the output
    aplRat = lpaplRat[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplRat)
        DbgStop ();
#endif

    // Initialize the result to 0/1
    mphc8r_init (&lpAllTypes->aplHC8R);

    // Set the common parts to RAT & HC8R
    mpq_set (&lpAllTypes->aplRat, &aplRat);
} // TPA_RAT_HC8R


//***************************************************************************
//  $TPA_RAT_HC8V
//***************************************************************************

void TPA_RAT_HC8V
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLRAT aplRat;

    // Copy the input value in case it overlaps with the output
    aplRat = lpaplRat[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplRat)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Set the common parts to RAT & HC8V
    mpfr_set_q  (&lpAllTypes->aplVfp, &aplRat, MPFR_RNDN);
} // TPA_RAT_HC8V


//***************************************************************************
//  $TPA_VFP_HC2V
//***************************************************************************

void TPA_VFP_HC2V
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLVFP aplVfp;

    // Copy the input value in case it overlaps with the output
    aplVfp = lpaplVfp[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplVfp)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc2v_init0 (&lpAllTypes->aplHC2V);

    // Set the common parts to VFP & HC2V
    mpfr_copy (&lpAllTypes->aplVfp, &aplVfp);
} // TPA_VFP_HC2V


//***************************************************************************
//  $TPA_VFP_HC4V
//***************************************************************************

void TPA_VFP_HC4V
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLVFP aplVfp;

    // Copy the input value in case it overlaps with the output
    aplVfp = lpaplVfp[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplVfp)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    // Set the common parts to VFP & HC4V
    mpfr_copy (&lpAllTypes->aplVfp, &aplVfp);
} // TPA_VFP_HC4V


//***************************************************************************
//  $TPA_VFP_HC8V
//***************************************************************************

void TPA_VFP_HC8V
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    APLVFP aplVfp;

    // Copy the input value in case it overlaps with the output
    aplVfp = lpaplVfp[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplVfp)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Set the common parts to VFP & HC8V
    mpfr_copy (&lpAllTypes->aplVfp, &aplVfp);
} // TPA_VFP_HC8V


//***************************************************************************
//  $TPA_HC2I_HETE
//***************************************************************************

void TPA_HC2I_HETE
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_HC2I,          // Entry type
                      &lpaplHC2I[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC2I_HETE


//***************************************************************************
//  $TPA_HC2I_NEST
//***************************************************************************

void TPA_HC2I_NEST
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_HC2I,          // Entry type
                      &lpaplHC2I[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC2I_NEST


//***************************************************************************
//  $TPA_HC2I_HC2I
//***************************************************************************

void TPA_HC2I_HC2I
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // No need to copy the input as it is the same size as the output

    lpAllTypes->aplHC2I = lpaplHC2I[uInt];
} // TPA_HC2I_HC2I


//***************************************************************************
//  $TPA_HC2I_HC2F
//***************************************************************************

void TPA_HC2I_HC2F
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int i;

    // No need to copy the input as it is the same size as the output

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        lpAllTypes->aplHC2F.parts[i] = (APLFLOAT) lpaplHC2I[uInt].parts[i];
} // TPA_HC2I_HC2F


//***************************************************************************
//  $TPA_HC2I_HC2R
//***************************************************************************

void TPA_HC2I_HC2R
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2I aplHC2I;

    // Copy the input value in case it overlaps with the output
    aplHC2I = lpaplHC2I[uInt];

    // Initialize the result to 0/1
    mphc2r_init (&lpAllTypes->aplHC2R);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        mpq_set_sx (&lpAllTypes->aplHC2R.parts[i], aplHC2I.parts[i], 1);
} // TPA_HC2I_HC2R


//***************************************************************************
//  $TPA_HC2I_HC2V
//***************************************************************************

void TPA_HC2I_HC2V
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2I aplHC2I;

    // Copy the input value in case it overlaps with the output
    aplHC2I = lpaplHC2I[uInt];

    // Initialize the result to 0
    mphc2v_init0 (&lpAllTypes->aplHC2V);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        mpfr_set_sx (&lpAllTypes->aplHC2V.parts[i], aplHC2I.parts[i], MPFR_RNDN);
} // TPA_HC2I_HC2V


//***************************************************************************
//  $TPA_HC2I_BA2F
//***************************************************************************

void TPA_HC2I_BA2F
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2I aplHC2I;

    // Copy the input value in case it overlaps with the output
    aplHC2I = lpaplHC2I[uInt];

    // Initialize the result to 0/0
    arb2f_init (&lpAllTypes->aplBA2F);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        arb_set_sx (&lpAllTypes->aplBA2F.parts[i], aplHC2I.parts[i]);
} // TPA_HC2I_BA2F


//***************************************************************************
//  $TPA_HC2I_BA4F
//***************************************************************************

void TPA_HC2I_BA4F
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2I aplHC2I;

    // Copy the input value in case it overlaps with the output
    aplHC2I = lpaplHC2I[uInt];

    // Initialize the result to 0/0
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        arb_set_sx (&lpAllTypes->aplBA4F.parts[i], aplHC2I.parts[i]);
} // TPA_HC2I_BA4F


//***************************************************************************
//  $TPA_HC2I_BA8F
//***************************************************************************

void TPA_HC2I_BA8F
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2I aplHC2I;

    // Copy the input value in case it overlaps with the output
    aplHC2I = lpaplHC2I[uInt];

    // Initialize the result to 0/0
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        arb_set_sx (&lpAllTypes->aplBA8F.parts[i], aplHC2I.parts[i]);
} // TPA_HC2I_BA8F


//***************************************************************************
//  $TPA_HC2F_BA2F
//***************************************************************************

void TPA_HC2F_BA2F
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2F aplHC2F;

    // Copy the input value in case it overlaps with the output
    aplHC2F = lpaplHC2F[uInt];

    // Initialize the result to 0/0
    arb2f_init (&lpAllTypes->aplBA2F);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        arb_set_d (&lpAllTypes->aplBA2F.parts[i], aplHC2F.parts[i]);
} // TPA_HC2F_BA2F


//***************************************************************************
//  $TPA_HC2F_BA4F
//***************************************************************************

void TPA_HC2F_BA4F
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2F aplHC2F;

    // Copy the input value in case it overlaps with the output
    aplHC2F = lpaplHC2F[uInt];

    // Initialize the result to 0/0
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        arb_set_d (&lpAllTypes->aplBA4F.parts[i], aplHC2F.parts[i]);
} // TPA_HC2F_BA4F


//***************************************************************************
//  $TPA_HC2F_BA8F
//***************************************************************************

void TPA_HC2F_BA8F
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2F aplHC2F;

    // Copy the input value in case it overlaps with the output
    aplHC2F = lpaplHC2F[uInt];

    // Initialize the result to 0/0
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        arb_set_d (&lpAllTypes->aplBA8F.parts[i], aplHC2F.parts[i]);
} // TPA_HC2F_BA8F


//***************************************************************************
//  $TPA_HC2R_BA2F
//***************************************************************************

void TPA_HC2R_BA2F
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2R aplHC2R;

    // Copy the input value in case it overlaps with the output
    aplHC2R = lpaplHC2R[uInt];

    // Initialize the result to 0/0
    arb2f_init (&lpAllTypes->aplBA2F);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        arb_set_q (&lpAllTypes->aplBA2F.parts[i], &aplHC2R.parts[i]);
} // TPA_HC2R_BA2F


//***************************************************************************
//  $TPA_HC2R_BA4F
//***************************************************************************

void TPA_HC2R_BA4F
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2R aplHC2R;

    // Copy the input value in case it overlaps with the output
    aplHC2R = lpaplHC2R[uInt];

    // Initialize the result to 0/0
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        arb_set_q (&lpAllTypes->aplBA4F.parts[i], &aplHC2R.parts[i]);
} // TPA_HC2R_BA4F


//***************************************************************************
//  $TPA_HC2R_BA8F
//***************************************************************************

void TPA_HC2R_BA8F
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2R aplHC2R;

    // Copy the input value in case it overlaps with the output
    aplHC2R = lpaplHC2R[uInt];

    // Initialize the result to 0/0
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        arb_set_q (&lpAllTypes->aplBA8F.parts[i], &aplHC2R.parts[i]);
} // TPA_HC2R_BA8F


//***************************************************************************
//  $TPA_HC2V_BA2F
//***************************************************************************

void TPA_HC2V_BA2F
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2V aplHC2V;

    // Copy the input value in case it overlaps with the output
    aplHC2V = lpaplHC2V[uInt];

    // Initialize the result to 0/0
    arb2f_init (&lpAllTypes->aplBA2F);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        arb_set_mpfr (&lpAllTypes->aplBA2F.parts[i], &aplHC2V.parts[i]);
} // TPA_HC2V_BA2F


//***************************************************************************
//  $TPA_HC2V_BA4F
//***************************************************************************

void TPA_HC2V_BA4F
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2V aplHC2V;

    // Copy the input value in case it overlaps with the output
    aplHC2V = lpaplHC2V[uInt];

    // Initialize the result to 0/0
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        arb_set_mpfr (&lpAllTypes->aplBA4F.parts[i], &aplHC2V.parts[i]);
} // TPA_HC2V_BA4F


//***************************************************************************
//  $TPA_HC2V_BA8F
//***************************************************************************

void TPA_HC2V_BA8F
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2V aplHC2V;

    // Copy the input value in case it overlaps with the output
    aplHC2V = lpaplHC2V[uInt];

    // Initialize the result to 0/0
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        arb_set_mpfr (&lpAllTypes->aplBA8F.parts[i], &aplHC2V.parts[i]);
} // TPA_HC2V_BA8F


//***************************************************************************
//  $TPA_HC2I_HC4R
//***************************************************************************

void TPA_HC2I_HC4R
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2I aplHC2I;

    // Copy the input value in case it overlaps with the output
    aplHC2I = lpaplHC2I[uInt];

    // Initialize the result to 0/1
    mphc4r_init (&lpAllTypes->aplHC4R);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the right arg to the result
        mpq_set_sx (&lpAllTypes->aplHC4R.parts[i], aplHC2I.parts[i], 1);
} // TPA_HC2I_HC4R


//***************************************************************************
//  $TPA_HC2I_HC4V
//***************************************************************************

void TPA_HC2I_HC4V
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2I aplHC2I;

    // Copy the input value in case it overlaps with the output
    aplHC2I = lpaplHC2I[uInt];

    // Initialize the result to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the right arg to the result
        mpfr_set_sx (&lpAllTypes->aplHC4V.parts[i], aplHC2I.parts[i], MPFR_RNDN);
} // TPA_HC2I_HC4V


//***************************************************************************
//  $TPA_HC2I_HC8R
//***************************************************************************

void TPA_HC2I_HC8R
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2I aplHC2I;

    // Copy the input value in case it overlaps with the output
    aplHC2I = lpaplHC2I[uInt];

    // Initialize the result to 0/1
    mphc8r_init (&lpAllTypes->aplHC8R);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the right arg to the result
        mpq_set_sx (&lpAllTypes->aplHC8R.parts[i], aplHC2I.parts[i], 1);
} // TPA_HC2I_HC8R


//***************************************************************************
//  $TPA_HC2I_HC8V
//***************************************************************************

void TPA_HC2I_HC8V
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2I aplHC2I;

    // Copy the input value in case it overlaps with the output
    aplHC2I = lpaplHC2I[uInt];

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the right arg to the result
        mpfr_set_sx (&lpAllTypes->aplHC8V.parts[i], aplHC2I.parts[i], MPFR_RNDN);
} // TPA_HC2I_HC8V


//***************************************************************************
//  $TPA_HC2F_HC2F
//***************************************************************************

void TPA_HC2F_HC2F
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // No need to copy the input as it is the same size as the output

    lpAllTypes->aplHC2F = lpaplHC2F[uInt];
} // TPA_HC2F_HC2F


//***************************************************************************
//  $TPA_HC2F_HETE
//***************************************************************************

void TPA_HC2F_HETE
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_HC2F,          // Entry type
                      &lpaplHC2F[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC2F_HETE


//***************************************************************************
//  $TPA_HC2F_NEST
//***************************************************************************

void TPA_HC2F_NEST
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_HC2F,          // Entry type
                      &lpaplHC2F[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC2F_NEST


//***************************************************************************
//  $TPA_HC2F_HC2V
//***************************************************************************

void TPA_HC2F_HC2V
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2F aplHC2F;

    // Copy the input value in case it overlaps with the output
    aplHC2F = lpaplHC2F[uInt];

    // Initialize the result to 0
    mphc2v_init0 (&lpAllTypes->aplHC2V);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        mpfr_set_d  (&lpAllTypes->aplHC2V.parts[i], aplHC2F.parts[i], MPFR_RNDN);
} // TPA_HC2F_HC2V


//***************************************************************************
//  $TPA_HC2F_HC4V
//***************************************************************************

void TPA_HC2F_HC4V
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2F aplHC2F;

    // Copy the input value in case it overlaps with the output
    aplHC2F = lpaplHC2F[uInt];

    // Initialize the result to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the right arg to the result
        mpfr_set_d  (&lpAllTypes->aplHC4V.parts[i], aplHC2F.parts[i], MPFR_RNDN);
} // TPA_HC2F_HC4V


//***************************************************************************
//  $TPA_HC2F_HC8V
//***************************************************************************

void TPA_HC2F_HC8V
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2F aplHC2F;

    // Copy the input value in case it overlaps with the output
    aplHC2F = lpaplHC2F[uInt];

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the right arg to the result
        mpfr_set_d  (&lpAllTypes->aplHC8V.parts[i], aplHC2F.parts[i], MPFR_RNDN);
} // TPA_HC2F_HC8V


//***************************************************************************
//  $TPA_HC4I_HETE
//***************************************************************************

void TPA_HC4I_HETE
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_HC4I,          // Entry type
                      &lpaplHC4I[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC4I_HETE


//***************************************************************************
//  $TPA_HC4I_NEST
//***************************************************************************

void TPA_HC4I_NEST
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_HC4I,          // Entry type
                      &lpaplHC4I[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC4I_NEST


//***************************************************************************
//  $TPA_HC4I_HC4I
//***************************************************************************

void TPA_HC4I_HC4I
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // No need to copy the input as it is the same size as the output

    lpAllTypes->aplHC4I = lpaplHC4I[uInt];
} // TPA_HC4I_HC4I


//***************************************************************************
//  $TPA_HC4I_HC4F
//***************************************************************************

void TPA_HC4I_HC4F
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int i;

    // No need to copy the input as it is the same size as the output

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Copy the right arg to the result
        lpAllTypes->aplHC4F.parts[i] = (APLFLOAT) lpaplHC4I[uInt].parts[i];
} // TPA_HC4I_HC4F


//***************************************************************************
//  $TPA_HC4I_HC4R
//***************************************************************************

void TPA_HC4I_HC4R
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4I aplHC4I;

    // Copy the input value in case it overlaps with the output
    aplHC4I = lpaplHC4I[uInt];

    // Initialize the result to 0/1
    mphc4r_init (&lpAllTypes->aplHC4R);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Copy the right arg to the result
        mpq_set_sx (&lpAllTypes->aplHC4R.parts[i], aplHC4I.parts[i], 1);
} // TPA_HC4I_HC4R


//***************************************************************************
//  $TPA_HC4I_HC4V
//***************************************************************************

void TPA_HC4I_HC4V
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4I aplHC4I;

    // Copy the input value in case it overlaps with the output
    aplHC4I = lpaplHC4I[uInt];

    // Initialize the result to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Copy the right arg to the result
        mpfr_set_sx (&lpAllTypes->aplHC4V.parts[i], aplHC4I.parts[i], MPFR_RNDN);
} // TPA_HC4I_HC4V


//***************************************************************************
//  $TPA_HC4I_BA4F
//***************************************************************************

void TPA_HC4I_BA4F
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4I aplHC4I;

    // Copy the input value in case it overlaps with the output
    aplHC4I = lpaplHC4I[uInt];

    // Initialize the result to 0/0
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Copy the right arg to the result
        arb_set_sx (&lpAllTypes->aplBA4F.parts[i], aplHC4I.parts[i]);
} // TPA_HC4I_BA4F


//***************************************************************************
//  $TPA_HC4I_BA8F
//***************************************************************************

void TPA_HC4I_BA8F
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4I aplHC4I;

    // Copy the input value in case it overlaps with the output
    aplHC4I = lpaplHC4I[uInt];

    // Initialize the result to 0/0
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Copy the right arg to the result
        arb_set_sx (&lpAllTypes->aplBA8F.parts[i], aplHC4I.parts[i]);
} // TPA_HC4I_BA8F


//***************************************************************************
//  $TPA_HC4F_BA4F
//***************************************************************************

void TPA_HC4F_BA4F
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4F aplHC4F;

    // Copy the input value in case it overlaps with the output
    aplHC4F = lpaplHC4F[uInt];

    // Initialize the result to 0/0
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Copy the right arg to the result
        arb_set_d (&lpAllTypes->aplBA4F.parts[i], aplHC4F.parts[i]);
} // TPA_HC4F_BA4F


//***************************************************************************
//  $TPA_HC4F_BA8F
//***************************************************************************

void TPA_HC4F_BA8F
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4F aplHC4F;

    // Copy the input value in case it overlaps with the output
    aplHC4F = lpaplHC4F[uInt];

    // Initialize the result to 0/0
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Copy the right arg to the result
        arb_set_d (&lpAllTypes->aplBA8F.parts[i], aplHC4F.parts[i]);
} // TPA_HC4F_BA8F


//***************************************************************************
//  $TPA_HC4R_BA4F
//***************************************************************************

void TPA_HC4R_BA4F
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4R aplHC4R;

    // Copy the input value in case it overlaps with the output
    aplHC4R = lpaplHC4R[uInt];

    // Initialize the result to 0/0
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Copy the right arg to the result
        arb_set_q (&lpAllTypes->aplBA4F.parts[i], &aplHC4R.parts[i]);
} // TPA_HC4R_BA4F


//***************************************************************************
//  $TPA_HC4R_BA8F
//***************************************************************************

void TPA_HC4R_BA8F
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4R aplHC4R;

    // Copy the input value in case it overlaps with the output
    aplHC4R = lpaplHC4R[uInt];

    // Initialize the result to 0/0
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Copy the right arg to the result
        arb_set_q (&lpAllTypes->aplBA8F.parts[i], &aplHC4R.parts[i]);
} // TPA_HC4R_BA8F


//***************************************************************************
//  $TPA_HC4V_BA4F
//***************************************************************************

void TPA_HC4V_BA4F
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4V aplHC4V;

    // Copy the input value in case it overlaps with the output
    aplHC4V = lpaplHC4V[uInt];

    // Initialize the result to 0/0
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Copy the right arg to the result
        arb_set_mpfr (&lpAllTypes->aplBA4F.parts[i], &aplHC4V.parts[i]);
} // TPA_HC4V_BA4F


//***************************************************************************
//  $TPA_HC4V_BA8F
//***************************************************************************

void TPA_HC4V_BA8F
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4V aplHC4V;

    // Copy the input value in case it overlaps with the output
    aplHC4V = lpaplHC4V[uInt];

    // Initialize the result to 0/0
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Copy the right arg to the result
        arb_set_mpfr (&lpAllTypes->aplBA8F.parts[i], &aplHC4V.parts[i]);
} // TPA_HC4V_BA8F


//***************************************************************************
//  $TPA_HC4I_HC8R
//***************************************************************************

void TPA_HC4I_HC8R
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4I aplHC4I;

    // Copy the input value in case it overlaps with the output
    aplHC4I = lpaplHC4I[uInt];

    // Initialize the result to 0/1
    mphc8r_init (&lpAllTypes->aplHC8R);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the right arg to the result
        mpq_set_sx (&lpAllTypes->aplHC8R.parts[i], aplHC4I.parts[i], 1);
} // TPA_HC4I_HC8R


//***************************************************************************
//  $TPA_HC4I_HC8V
//***************************************************************************

void TPA_HC4I_HC8V
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4I aplHC4I;

    // Copy the input value in case it overlaps with the output
    aplHC4I = lpaplHC4I[uInt];

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the right arg to the result
        mpfr_set_sx (&lpAllTypes->aplHC8V.parts[i], aplHC4I.parts[i], MPFR_RNDN);
} // TPA_HC4I_HC8V


//***************************************************************************
//  $TPA_HC4F_HETE
//***************************************************************************

void TPA_HC4F_HETE
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_HC4F,          // Entry type
                      &lpaplHC4F[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC4F_HETE


//***************************************************************************
//  $TPA_HC4F_NEST
//***************************************************************************

void TPA_HC4F_NEST
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_HC4F,          // Entry type
                      &lpaplHC4F[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC4F_NEST


//***************************************************************************
//  $TPA_HC4F_HC4V
//***************************************************************************

void TPA_HC4F_HC4V
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4F aplHC4F;

    // Copy the input value in case it overlaps with the output
    aplHC4F = lpaplHC4F[uInt];

    // Initialize the result to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Copy the right arg to the result
        mpfr_set_d  (&lpAllTypes->aplHC4V.parts[i], aplHC4F.parts[i], MPFR_RNDN);
} // TPA_HC4F_HC4V


//***************************************************************************
//  $TPA_HC4F_HC8V
//***************************************************************************

void TPA_HC4F_HC8V
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4F aplHC4F;

    // Copy the input value in case it overlaps with the output
    aplHC4F = lpaplHC4F[uInt];

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the right arg to the result
        mpfr_set_d  (&lpAllTypes->aplHC8V.parts[i], aplHC4F.parts[i], MPFR_RNDN);
} // TPA_HC4F_HC8V


//***************************************************************************
//  $TPA_HC4F_HC4F
//***************************************************************************

void TPA_HC4F_HC4F
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHC4F = lpaplHC4F[uInt];
} // TPA_HC4F_HC4F


//***************************************************************************
//  $TPA_HC8I_HETE
//***************************************************************************

void TPA_HC8I_HETE
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_HC8I,          // Entry type
                      &lpaplHC8I[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC8I_HETE


//***************************************************************************
//  $TPA_HC8I_NEST
//***************************************************************************

void TPA_HC8I_NEST
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_HC8I,          // Entry type
                      &lpaplHC8I[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC8I_NEST


//***************************************************************************
//  $TPA_HC8I_HC8I
//***************************************************************************

void TPA_HC8I_HC8I
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // No need to copy the input as it is the same size as the output

    lpAllTypes->aplHC8I = lpaplHC8I[uInt];
} // TPA_HC8I_HC8I


//***************************************************************************
//  $TPA_HC8I_HC8F
//***************************************************************************

void TPA_HC8I_HC8F
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int i;

    // No need to copy the input as it is the same size as the output

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Copy the right arg to the result
        lpAllTypes->aplHC8F.parts[i] = (APLFLOAT) lpaplHC8I[uInt].parts[i];
} // TPA_HC8I_HC8F


//***************************************************************************
//  $TPA_HC8I_HC8R
//***************************************************************************

void TPA_HC8I_HC8R
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC8I aplHC8I;

    // Copy the input value in case it overlaps with the output
    aplHC8I = lpaplHC8I[uInt];

    // Initialize the result to 0/1
    mphc8r_init (&lpAllTypes->aplHC8R);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Copy the right arg to the result
        mpq_set_sx (&lpAllTypes->aplHC8R.parts[i], aplHC8I.parts[i], 1);
} // TPA_HC8I_HC8R


//***************************************************************************
//  $TPA_HC8I_HC8V
//***************************************************************************

void TPA_HC8I_HC8V
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC8I aplHC8I;

    // Copy the input value in case it overlaps with the output
    aplHC8I = lpaplHC8I[uInt];

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Copy the right arg to the result
        mpfr_set_sx (&lpAllTypes->aplHC8V.parts[i], aplHC8I.parts[i], MPFR_RNDN);
} // TPA_HC8I_HC8V


//***************************************************************************
//  $TPA_HC8F_HETE
//***************************************************************************

void TPA_HC8F_HETE
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_HC8F,          // Entry type
                      &lpaplHC8F[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC8F_HETE


//***************************************************************************
//  $TPA_HC8F_NEST
//***************************************************************************

void TPA_HC8F_NEST
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_HC8F,          // Entry type
                      &lpaplHC8F[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC8F_NEST


//***************************************************************************
//  $TPA_HC8F_HC8F
//***************************************************************************

void TPA_HC8F_HC8F
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    // No need to copy the input as it is the same size as the output

    lpAllTypes->aplHC8F = lpaplHC8F[uInt];
} // TPA_HC8F_HC8F


//***************************************************************************
//  $TPA_HC8F_HC8V
//***************************************************************************

void TPA_HC8F_HC8V
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC8F aplHC8F;

    // Copy the input value in case it overlaps with the output
    aplHC8F = lpaplHC8F[uInt];

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Copy the right arg to the result
        mpfr_set_d  (&lpAllTypes->aplHC8V.parts[i], aplHC8F.parts[i], MPFR_RNDN);
} // TPA_HC8F_HC8V


//***************************************************************************
//  $TPA_HC8I_BA8F
//***************************************************************************

void TPA_HC8I_BA8F
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC8I aplHC8I;

    // Copy the input value in case it overlaps with the output
    aplHC8I = lpaplHC8I[uInt];

    // Initialize the result to 0/0
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Copy the right arg to the result
        arb_set_sx (&lpAllTypes->aplBA8F.parts[i], aplHC8I.parts[i]);
} // TPA_HC8I_BA8F


//***************************************************************************
//  $TPA_HC8F_BA8F
//***************************************************************************

void TPA_HC8F_BA8F
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC8F aplHC8F;

    // Copy the input value in case it overlaps with the output
    aplHC8F = lpaplHC8F[uInt];

    // Initialize the result to 0/0
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Copy the right arg to the result
        arb_set_d (&lpAllTypes->aplBA8F.parts[i], aplHC8F.parts[i]);
} // TPA_HC8F_BA8F


//***************************************************************************
//  $TPA_HC8R_BA8F
//***************************************************************************

void TPA_HC8R_BA8F
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC8R aplHC8R;

    // Copy the input value in case it overlaps with the output
    aplHC8R = lpaplHC8R[uInt];

    // Initialize the result to 0/0
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Copy the right arg to the result
        arb_set_q (&lpAllTypes->aplBA8F.parts[i], &aplHC8R.parts[i]);
} // TPA_HC8R_BA8F


//***************************************************************************
//  $TPA_HC8V_BA8F
//***************************************************************************

void TPA_HC8V_BA8F
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC8V aplHC8V;

    // Copy the input value in case it overlaps with the output
    aplHC8V = lpaplHC8V[uInt];

    // Initialize the result to 0/0
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Copy the right arg to the result
        arb_set_mpfr (&lpAllTypes->aplBA8F.parts[i], &aplHC8V.parts[i]);
} // TPA_HC8V_BA8F


//***************************************************************************
//  $TPA_HC2R_HETE
//***************************************************************************

void TPA_HC2R_HETE
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_HC2R,          // Entry type
                      &lpaplHC2R[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC2R_HETE


//***************************************************************************
//  $TPA_HC2R_NEST
//***************************************************************************

void TPA_HC2R_NEST
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_HC2R,          // Entry type
                      &lpaplHC2R[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC2R_NEST


//***************************************************************************
//  $TPA_HC2R_HC2R
//***************************************************************************

void TPA_HC2R_HC2R
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2R aplHC2R;

    // Copy the input value in case it overlaps with the output
    aplHC2R = lpaplHC2R[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC2R)
        DbgStop ();
#endif

    // Initialize the result to 0/1
    mphc2r_init (&lpAllTypes->aplHC2R);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        mpq_set (&lpAllTypes->aplHC2R.parts[i], &aplHC2R.parts[i]);
} // TPA_HC2R_HC2R


//***************************************************************************
//  $TPA_HC2R_HC2V
//***************************************************************************

void TPA_HC2R_HC2V
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2R aplHC2R;

    // Copy the input value in case it overlaps with the output
    aplHC2R = lpaplHC2R[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC2R)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc2v_init0 (&lpAllTypes->aplHC2V);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        mpfr_set_q  (&lpAllTypes->aplHC2V.parts[i], &aplHC2R.parts[i], MPFR_RNDN);
} // TPA_HC2R_HC2V


//***************************************************************************
//  $TPA_HC2R_HC4R
//***************************************************************************

void TPA_HC2R_HC4R
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2R aplHC2R;

    // Copy the input value in case it overlaps with the output
    aplHC2R = lpaplHC2R[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC2R)
        DbgStop ();
#endif

    // Initialize the result to 0/1
    mphc4r_init (&lpAllTypes->aplHC4R);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the right arg to the result
        mpq_set (&lpAllTypes->aplHC4R.parts[i], &aplHC2R.parts[i]);
} // TPA_HC2R_HC4R


//***************************************************************************
//  $TPA_HC2R_HC4V
//***************************************************************************

void TPA_HC2R_HC4V
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2R aplHC2R;

    // Copy the input value in case it overlaps with the output
    aplHC2R = lpaplHC2R[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC2R)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the right arg to the result
        mpfr_set_q  (&lpAllTypes->aplHC4V.parts[i], &aplHC2R.parts[i], MPFR_RNDN);
} // TPA_HC2R_HC4V


//***************************************************************************
//  $TPA_HC2R_HC8R
//***************************************************************************

void TPA_HC2R_HC8R
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2R aplHC2R;

    // Copy the input value in case it overlaps with the output
    aplHC2R = lpaplHC2R[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC2R)
        DbgStop ();
#endif

    // Initialize the result to 0/1
    mphc8r_init (&lpAllTypes->aplHC8R);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the right arg to the result
        mpq_set (&lpAllTypes->aplHC8R.parts[i], &aplHC2R.parts[i]);
} // TPA_HC2R_HC8R


//***************************************************************************
//  $TPA_HC2R_HC8V
//***************************************************************************

void TPA_HC2R_HC8V
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2R aplHC2R;

    // Copy the input value in case it overlaps with the output
    aplHC2R = lpaplHC2R[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC2R)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the right arg to the result
        mpfr_set_q  (&lpAllTypes->aplHC8V.parts[i], &aplHC2R.parts[i], MPFR_RNDN);
} // TPA_HC2R_HC8V


//***************************************************************************
//  $TPA_HC4R_HETE
//***************************************************************************

void TPA_HC4R_HETE
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_HC4R,          // Entry type
                      &lpaplHC4R[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC4R_HETE


//***************************************************************************
//  $TPA_HC4R_NEST
//***************************************************************************

void TPA_HC4R_NEST
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_HC4R,          // Entry type
                      &lpaplHC4R[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC4R_NEST


//***************************************************************************
//  $TPA_HC4R_HC4R
//***************************************************************************

void TPA_HC4R_HC4R
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4R aplHC4R;

    // Copy the input value in case it overlaps with the output
    aplHC4R = lpaplHC4R[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC4R)
        DbgStop ();
#endif

    // Initialize the result to 0/1
    mphc4r_init (&lpAllTypes->aplHC4R);

    // Loop through the common parts
    for (i = 0; i < 4; i++)
        // Copy the right arg to the result
        mpq_set (&lpAllTypes->aplHC4R.parts[i], &aplHC4R.parts[i]);
} // TPA_HC4R_HC4R


//***************************************************************************
//  $TPA_HC4R_HC4V
//***************************************************************************

void TPA_HC4R_HC4V
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4R aplHC4R;

    // Copy the input value in case it overlaps with the output
    aplHC4R = lpaplHC4R[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC4R)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    // Loop through the common parts
    for (i = 0; i < 4; i++)
        // Convert HC4R to HC4V
        mpfr_set_q (&lpAllTypes->aplHC4V.parts[i], &aplHC4R.parts[i], MPFR_RNDN);
} // TPA_HC4R_HC4V


//***************************************************************************
//  $TPA_HC4R_HC8R
//***************************************************************************

void TPA_HC4R_HC8R
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4R aplHC4R;

    // Copy the input value in case it overlaps with the output
    aplHC4R = lpaplHC4R[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC4R)
        DbgStop ();
#endif

    // Initialize the result to 0/1
    mphc8r_init (&lpAllTypes->aplHC8R);

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Convert HC4R to HC8R
        mpq_set (&lpAllTypes->aplHC8R.parts[i], &aplHC4R.parts[i]);
} // TPA_HC4R_HC8R


//***************************************************************************
//  $TPA_HC4R_HC8V
//***************************************************************************

void TPA_HC4R_HC8V
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4R aplHC4R;

    // Copy the input value in case it overlaps with the output
    aplHC4R = lpaplHC4R[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC4R)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Convert HC4R to HC8V
        mpfr_set_q  (&lpAllTypes->aplHC8V.parts[i], &aplHC4R.parts[i], MPFR_RNDN);
} // TPA_HC4R_HC8V


//***************************************************************************
//  $TPA_HC8R_HETE
//***************************************************************************

void TPA_HC8R_HETE
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_HC8R,          // Entry type
                      &lpaplHC8R[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC8R_HETE


//***************************************************************************
//  $TPA_HC8R_NEST
//***************************************************************************

void TPA_HC8R_NEST
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_HC8R,          // Entry type
                      &lpaplHC8R[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC8R_NEST


//***************************************************************************
//  $TPA_HC8R_HC8R
//***************************************************************************

void TPA_HC8R_HC8R
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC8R aplHC8R;

    // Copy the input value in case it overlaps with the output
    aplHC8R = lpaplHC8R[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC8R)
        DbgStop ();
#endif

    // Initialize the result to 0/1
    mphc8r_init (&lpAllTypes->aplHC8R);

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Copy the right arg to the result
        mpq_set (&lpAllTypes->aplHC8R.parts[i], &aplHC8R.parts[i]);
} // TPA_HC8R_HC8R


//***************************************************************************
//  $TPA_HC8R_HC8V
//***************************************************************************

void TPA_HC8R_HC8V
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC8R aplHC8R;

    // Copy the input value in case it overlaps with the output
    aplHC8R = lpaplHC8R[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC8R)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Copy the right arg to the result
        mpfr_set_q  (&lpAllTypes->aplHC8V.parts[i], &aplHC8R.parts[i], MPFR_RNDN);
} // TPA_HC8R_HC8V


//***************************************************************************
//  $TPA_HC2V_HETE
//***************************************************************************

void TPA_HC2V_HETE
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_HC2V,          // Entry type
                      &lpaplHC2V[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC2V_HETE


//***************************************************************************
//  $TPA_HC2V_NEST
//***************************************************************************

void TPA_HC2V_NEST
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_HC2V,          // Entry type
                      &lpaplHC2V[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC2V_NEST


//***************************************************************************
//  $TPA_HC2V_HC2V
//***************************************************************************

void TPA_HC2V_HC2V
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2V aplHC2V;

    // Copy the input value in case it overlaps with the output
    aplHC2V = lpaplHC2V[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC2V)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc2v_init0 (&lpAllTypes->aplHC2V);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        // Copy the right arg to the result
        mpfr_copy (&lpAllTypes->aplHC2V.parts[i], &aplHC2V.parts[i]);
} // TPA_HC2V_HC2V


//***************************************************************************
//  $TPA_HC2V_HC4V
//***************************************************************************

void TPA_HC2V_HC4V
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2V aplHC2V;

    // Copy the input value in case it overlaps with the output
    aplHC2V = lpaplHC2V[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC2V)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the right arg to the result
        mpfr_copy (&lpAllTypes->aplHC4V.parts[i], &aplHC2V.parts[i]);
} // TPA_HC2V_HC4V


//***************************************************************************
//  $TPA_HC2V_HC8V
//***************************************************************************

void TPA_HC2V_HC8V
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC2V aplHC2V;

    // Copy the input value in case it overlaps with the output
    aplHC2V = lpaplHC2V[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC2V)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the right arg to the result
        mpfr_copy (&lpAllTypes->aplHC8V.parts[i], &aplHC2V.parts[i]);
} // TPA_HC2V_HC8V


//***************************************************************************
//  $TPA_HC4V_HETE
//***************************************************************************

void TPA_HC4V_HETE
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_HC4V,          // Entry type
                      &lpaplHC4V[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC4V_HETE


//***************************************************************************
//  $TPA_HC4V_NEST
//***************************************************************************

void TPA_HC4V_NEST
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_HC4V,          // Entry type
                      &lpaplHC4V[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC4V_NEST


//***************************************************************************
//  $TPA_HC4V_HC4V
//***************************************************************************

void TPA_HC4V_HC4V
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4V aplHC4V;

    // Copy the input value in case it overlaps with the output
    aplHC4V = lpaplHC4V[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC4V)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    // Loop through the common parts
    for (i = 0; i < 4; i++)
        // Copy the right arg to the result
        mpfr_copy (&lpAllTypes->aplHC4V.parts[i], &aplHC4V.parts[i]);
} // TPA_HC4V_HC4V


//***************************************************************************
//  $TPA_HC4V_HC8V
//***************************************************************************

void TPA_HC4V_HC8V
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC4V aplHC4V;

    // Copy the input value in case it overlaps with the output
    aplHC4V = lpaplHC4V[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC4V)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the right arg to the result
        mpfr_copy (&lpAllTypes->aplHC8V.parts[i], &aplHC4V.parts[i]);
} // TPA_HC4V_HC8V


//***************************************************************************
//  $TPA_HC8V_HETE
//***************************************************************************

void TPA_HC8V_HETE
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_HC8V,          // Entry type
                      &lpaplHC8V[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC8V_HETE


//***************************************************************************
//  $TPA_HC8V_NEST
//***************************************************************************

void TPA_HC8V_NEST
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_HC8V,          // Entry type
                      &lpaplHC8V[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_HC8V_NEST


//***************************************************************************
//  $TPA_HC8V_HC8V
//***************************************************************************

void TPA_HC8V_HC8V
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int     i;
    APLHC8V aplHC8V;

    // Copy the input value in case it overlaps with the output
    aplHC8V = lpaplHC8V[uInt];

#ifdef DEBUG
    // If the input and output overlap, ...
    if (((LPBYTE) lpAllTypes) EQ (LPBYTE) lpaplHC8V)
        DbgStop ();
#endif

    // Initialize the result to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Copy the right arg to the result
        mpfr_copy (&lpAllTypes->aplHC8V.parts[i], &aplHC8V.parts[i]);
} // TPA_HC8V_HC8V


//***************************************************************************
//  $TPA_BOOL_BA1F
//***************************************************************************

void TPA_BOOL_BA1F
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    Myarb_init     (&lpAllTypes->aplArb);
    arb_set_sx   (&lpAllTypes->aplArb, BIT0 & GetNextBoolean (lpaplBoolean, uInt));
} // TPA_BOOL_BA1F


//***************************************************************************
//  $TPA_INT_BA1F
//***************************************************************************

void TPA_INT_BA1F
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    Myarb_init     (&lpAllTypes->aplArb);
    arb_set_sx   (&lpAllTypes->aplArb, GetNextInteger (lpaplInteger, ARRAY_INT, uInt));
} // TPA_INT_BA1F


//***************************************************************************
//  $TPA_FLT_BA1F
//***************************************************************************

void TPA_FLT_BA1F
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    Myarb_init     (&lpAllTypes->aplArb);
    arb_set_d    (&lpAllTypes->aplArb, lpaplFloat[uInt]);
} // TPA_FLT_BA1F


//***************************************************************************
//  $TPA_APA_BA1F
//***************************************************************************

void TPA_APA_BA1F
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    Myarb_init     (&lpAllTypes->aplArb);
    arb_set_sx   (&lpAllTypes->aplArb, GetNextInteger (lpaplAPA, ARRAY_APA, uInt));
} // TPA_APA_BA1F


//***************************************************************************
//  $TPA_RAT_BA1F
//***************************************************************************

void TPA_RAT_BA1F
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    Myarb_init     (&lpAllTypes->aplArb);
    arb_set_q    (&lpAllTypes->aplArb, &lpaplRat[uInt]);
} // TPA_RAT_BA1F


//***************************************************************************
//  $TPA_VFP_BA1F
//***************************************************************************

void TPA_VFP_BA1F
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    Myarb_init     (&lpAllTypes->aplArb);
    arb_set_mpfr (&lpAllTypes->aplArb, &lpaplVfp[uInt]);
} // TPA_VFP_BA1F


//***************************************************************************
//  $TPA_BA1F_BA1F
//***************************************************************************

void TPA_BA1F_BA1F
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    Myarb_init     (&lpAllTypes->aplArb);
    arb_set      (&lpAllTypes->aplArb, &lpaplBA1F[uInt]);
} // TPA_BA1F_BA1F


//***************************************************************************
//  $TPA_BA1F_HETE
//***************************************************************************

void TPA_BA1F_HETE
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_BA1F,          // Entry type
                      &lpaplBA1F[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_BA1F_HETE


//***************************************************************************
//  $TPA_BA1F_BA2F
//***************************************************************************

void TPA_BA1F_BA2F
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb2f_init   (&lpAllTypes->aplBA2F);
    arb_set      (&lpAllTypes->aplBA2F.parts[0], &lpaplBA1F[uInt]);
} // TPA_BA1F_BA2F


//***************************************************************************
//  $TPA_BA1F_BA4F
//***************************************************************************

void TPA_BA1F_BA4F
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb4f_init   (&lpAllTypes->aplBA4F);
    arb_set      (&lpAllTypes->aplBA4F.parts[0], &lpaplBA1F[uInt]);
} // TPA_BA1F_BA4F


//***************************************************************************
//  $TPA_BA1F_BA8F
//***************************************************************************

void TPA_BA1F_BA8F
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb8f_init   (&lpAllTypes->aplBA8F);
    arb_set      (&lpAllTypes->aplBA8F.parts[0], &lpaplBA1F[uInt]);
} // TPA_BA1F_BA8F


//***************************************************************************
//  $TPA_BA1F_NEST
//***************************************************************************

void TPA_BA1F_NEST
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_BA1F,          // Entry type
                      &lpaplBA1F[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_BA1F_NEST


//***************************************************************************
//  $TPA_BOOL_BA2F
//***************************************************************************

void TPA_BOOL_BA2F
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb2f_init   (&lpAllTypes->aplBA2F);
    arb_set_sx   (&lpAllTypes->aplBA2F.parts[0], BIT0 & GetNextBoolean (lpaplBoolean, uInt));
} // TPA_BOOL_BA2F


//***************************************************************************
//  $TPA_INT_BA2F
//***************************************************************************

void TPA_INT_BA2F
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb2f_init   (&lpAllTypes->aplBA2F);
    arb_set_sx   (&lpAllTypes->aplBA2F.parts[0], GetNextInteger (lpaplInteger, ARRAY_INT, uInt));
} // TPA_INT_BA2F


//***************************************************************************
//  $TPA_FLT_BA2F
//***************************************************************************

void TPA_FLT_BA2F
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb2f_init   (&lpAllTypes->aplBA2F);
    arb_set_d    (&lpAllTypes->aplBA2F.parts[0], lpaplFloat[uInt]);
} // TPA_FLT_BA2F


//***************************************************************************
//  $TPA_APA_BA2F
//***************************************************************************

void TPA_APA_BA2F
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb2f_init   (&lpAllTypes->aplBA2F);
    arb_set_sx   (&lpAllTypes->aplBA2F.parts[0], GetNextInteger (lpaplAPA, ARRAY_APA, uInt));
} // TPA_APA_BA2F


//***************************************************************************
//  $TPA_RAT_BA2F
//***************************************************************************

void TPA_RAT_BA2F
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb2f_init   (&lpAllTypes->aplBA2F);
    arb_set_q    (&lpAllTypes->aplBA2F.parts[0], &lpaplRat[uInt]);
} // TPA_RAT_BA2F


//***************************************************************************
//  $TPA_VFP_BA2F
//***************************************************************************

void TPA_VFP_BA2F
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb2f_init   (&lpAllTypes->aplBA2F);
    arb_set_mpfr (&lpAllTypes->aplBA2F.parts[0], &lpaplVfp[uInt]);
} // TPA_VFP_BA2F


//***************************************************************************
//  $TPA_BA2F_BA2F
//***************************************************************************

void TPA_BA2F_BA2F
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb2f_init   (&lpAllTypes->aplBA2F);
    arb_set      (&lpAllTypes->aplBA2F.parts[0], &lpaplBA2F[uInt].parts[0]);
    arb_set      (&lpAllTypes->aplBA2F.parts[1], &lpaplBA2F[uInt].parts[1]);
} // TPA_BA2F_BA2F


//***************************************************************************
//  $TPA_BA2F_BA4F
//***************************************************************************

void TPA_BA2F_BA4F
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb4f_init   (&lpAllTypes->aplBA4F);
    arb_set      (&lpAllTypes->aplBA4F.parts[0], &lpaplBA2F[uInt].parts[0]);
    arb_set      (&lpAllTypes->aplBA4F.parts[1], &lpaplBA2F[uInt].parts[1]);
} // TPA_BA2F_BA4F


//***************************************************************************
//  $TPA_BA2F_BA8F
//***************************************************************************

void TPA_BA2F_BA8F
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb8f_init   (&lpAllTypes->aplBA8F);
    arb_set      (&lpAllTypes->aplBA8F.parts[0], &lpaplBA2F[uInt].parts[0]);
    arb_set      (&lpAllTypes->aplBA8F.parts[1], &lpaplBA2F[uInt].parts[1]);
} // TPA_BA2F_BA8F


//***************************************************************************
//  $TPA_BA4F_BA8F
//***************************************************************************

void TPA_BA4F_BA8F
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int i;

    // Initialize the result
    arb8f_init   (&lpAllTypes->aplBA8F);

    // Loop through the common parts
    for (i = 0; i < 4; i++)
        arb_set (&lpAllTypes->aplBA8F.parts[i], &lpaplBA4F[uInt].parts[i]);
} // TPA_BA4F_BA8F


//***************************************************************************
//  $TPA_BA2F_HETE
//***************************************************************************

void TPA_BA2F_HETE
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_BA2F,          // Entry type
                      &lpaplBA2F[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplHetero EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_BA2F_HETE


//***************************************************************************
//  $TPA_BA2F_NEST
//***************************************************************************

void TPA_BA2F_NEST
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_BA2F,          // Entry type
                      &lpaplBA2F[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_BA2F_NEST


//***************************************************************************
//  $TPA_BA4F_NEST
//***************************************************************************

void TPA_BA4F_NEST
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_BA4F,          // Entry type
                      &lpaplBA4F[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_BA4F_NEST


//***************************************************************************
//  $TPA_BA8F_NEST
//***************************************************************************

void TPA_BA8F_NEST
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_BA8F,          // Entry type
                      &lpaplBA8F[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_BA8F_NEST


//***************************************************************************
//  $TPA_BOOL_BA4F
//***************************************************************************

void TPA_BOOL_BA4F
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb4f_init   (&lpAllTypes->aplBA4F);
    arb_set_sx   (&lpAllTypes->aplBA4F.parts[0], BIT0 & GetNextBoolean (lpaplBoolean, uInt));
} // TPA_BOOL_BA4F


//***************************************************************************
//  $TPA_INT_BA4F
//***************************************************************************

void TPA_INT_BA4F
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb4f_init   (&lpAllTypes->aplBA4F);
    arb_set_sx   (&lpAllTypes->aplBA4F.parts[0], GetNextInteger (lpaplInteger, ARRAY_INT, uInt));
} // TPA_INT_BA4F


//***************************************************************************
//  $TPA_FLT_BA4F
//***************************************************************************

void TPA_FLT_BA4F
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb4f_init   (&lpAllTypes->aplBA4F);
    arb_set_d    (&lpAllTypes->aplBA4F.parts[0], lpaplFloat[uInt]);
} // TPA_FLT_BA4F


//***************************************************************************
//  $TPA_APA_BA4F
//***************************************************************************

void TPA_APA_BA4F
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb4f_init   (&lpAllTypes->aplBA4F);
    arb_set_sx   (&lpAllTypes->aplBA4F.parts[0], GetNextInteger (lpaplAPA, ARRAY_APA, uInt));
} // TPA_APA_BA4F


//***************************************************************************
//  $TPA_RAT_BA4F
//***************************************************************************

void TPA_RAT_BA4F
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb4f_init   (&lpAllTypes->aplBA4F);
    arb_set_q    (&lpAllTypes->aplBA4F.parts[0], &lpaplRat[uInt]);
} // TPA_RAT_BA4F


//***************************************************************************
//  $TPA_VFP_BA4F
//***************************************************************************

void TPA_VFP_BA4F
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb4f_init   (&lpAllTypes->aplBA4F);
    arb_set_mpfr (&lpAllTypes->aplBA4F.parts[0], &lpaplVfp[uInt]);
} // TPA_VFP_BA4F


//***************************************************************************
//  $TPA_BA4F_BA4F
//***************************************************************************

void TPA_BA4F_BA4F
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int i;

    // Initialize the result
    arb4f_init   (&lpAllTypes->aplBA4F);

    // Loop through the common parts
    for (i = 0; i < 4; i++)
        arb_set (&lpAllTypes->aplBA4F.parts[i], &lpaplBA4F[uInt].parts[i]);
} // TPA_BA4F_BA4F


//***************************************************************************
//  $TPA_BA4F_HETE
//***************************************************************************

void TPA_BA4F_HETE
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplHetero =
      MakeGlbEntry_EM (ARRAY_BA4F,          // Entry type
                      &lpaplBA4F[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_BA4F_HETE


//***************************************************************************
//  $TPA_BOOL_BA8F
//***************************************************************************

void TPA_BOOL_BA8F
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb8f_init   (&lpAllTypes->aplBA8F);
    arb_set_sx   (&lpAllTypes->aplBA8F.parts[0], BIT0 & GetNextBoolean (lpaplBoolean, uInt));
} // TPA_BOOL_BA8F


//***************************************************************************
//  $TPA_INT_BA8F
//***************************************************************************

void TPA_INT_BA8F
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb8f_init   (&lpAllTypes->aplBA8F);
    arb_set_sx   (&lpAllTypes->aplBA8F.parts[0], GetNextInteger (lpaplInteger, ARRAY_INT, uInt));
} // TPA_INT_BA8F


//***************************************************************************
//  $TPA_FLT_BA8F
//***************************************************************************

void TPA_FLT_BA8F
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb8f_init   (&lpAllTypes->aplBA8F);
    arb_set_d    (&lpAllTypes->aplBA8F.parts[0], lpaplFloat[uInt]);
} // TPA_FLT_BA8F


//***************************************************************************
//  $TPA_APA_BA8F
//***************************************************************************

void TPA_APA_BA8F
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb8f_init   (&lpAllTypes->aplBA8F);
    arb_set_sx   (&lpAllTypes->aplBA8F.parts[0], GetNextInteger (lpaplAPA, ARRAY_APA, uInt));
} // TPA_APA_BA8F


//***************************************************************************
//  $TPA_RAT_BA8F
//***************************************************************************

void TPA_RAT_BA8F
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb8f_init   (&lpAllTypes->aplBA8F);
    arb_set_q    (&lpAllTypes->aplBA8F.parts[0], &lpaplRat[uInt]);
} // TPA_RAT_BA8F


//***************************************************************************
//  $TPA_VFP_BA8F
//***************************************************************************

void TPA_VFP_BA8F
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    arb8f_init   (&lpAllTypes->aplBA8F);
    arb_set_mpfr (&lpAllTypes->aplBA8F.parts[0], &lpaplVfp[uInt]);
} // TPA_VFP_BA8F


//***************************************************************************
//  $TPA_BA8F_BA8F
//***************************************************************************

void TPA_BA8F_BA8F
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    int i;

    // Initialize the result
    arb8f_init   (&lpAllTypes->aplBA8F);

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        arb_set (&lpAllTypes->aplBA8F.parts[i], &lpaplBA8F[uInt].parts[i]);
} // TPA_BA8F_BA8F


//***************************************************************************
//  $TPA_BA8F_HETE
//***************************************************************************

void TPA_BA8F_HETE
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes)

{
    lpAllTypes->aplNested =
      MakeGlbEntry_EM (ARRAY_BA8F,          // Entry type
                      &lpaplBA8F[uInt],     // Ptr to the value
                       TRUE,                // TRUE iff we should initialize the target first
                       NULL);               // Ptr to function token
    if (lpAllTypes->aplNested EQ NULL)
        RaiseException (EXCEPTION_WS_FULL, 0, 0, NULL);
} // TPA_BA8F_HETE


//***************************************************************************
//      TCA Routines
//***************************************************************************

//***************************************************************************
//  $TCA_ERROR
//***************************************************************************

void TCA_ERROR
    (LPVOID      lpaplAny,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as in error
        lpbRet[0] = FALSE;
} // TCA_ERROR


//***************************************************************************
//  $TCA_BOOL_BOOL
//***************************************************************************

void TCA_BOOL_BOOL
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    lpAllTypes->aplBoolean = BIT0 & (APLBOOL) GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_BOOL


//***************************************************************************
//  $TCA_BOOL_INT
//***************************************************************************

void TCA_BOOL_INT
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    lpAllTypes->aplInteger = GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_INT


//***************************************************************************
//  $TCA_BOOL_HC2I
//***************************************************************************

void TCA_BOOL_HC2I
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC2I.parts[0] = GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt);

    // Loop through the remainder of the imaginary HC2I parts
    for (; i < 2; i++)          // No bRet
        // Zero the imaginary part
        lpAllTypes->aplHC2I.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_HC2I


//***************************************************************************
//  $TCA_BOOL_HC4I
//***************************************************************************

void TCA_BOOL_HC4I
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC4I.parts[0] = GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt);

    // Loop through the remainder of the imaginary HC4I parts
    for (; i < 4; i++)          // No bRet
        // Zero the imaginary part
        lpAllTypes->aplHC4I.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_HC4I


//***************************************************************************
//  $TCA_BOOL_HC8I
//***************************************************************************

void TCA_BOOL_HC8I
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC8I.parts[0] = GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt);

    // Loop through the remainder of the imaginary HC8I parts
    for (; i < 8; i++)          // No bRet
        // Zero the imaginary part
        lpAllTypes->aplHC8I.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_HC8I


//***************************************************************************
//  $TCA_BOOL_FLT
//***************************************************************************

void TCA_BOOL_FLT
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Convert the BOOL to a FLT
    lpAllTypes->aplFloat = GetNextFloat (lpaplBoolean, ARRAY_BOOL, uInt);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_FLT


//***************************************************************************
//  $TCA_BOOL_HC2F
//***************************************************************************

void TCA_BOOL_HC2F
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC2F.parts[0] = GetNextFloat (lpaplBoolean, ARRAY_BOOL, uInt);

    // Loop through the remainder of the imaginary HC2F parts
    for (; i < 2; i++)          // No bRet
        // Zero the imaginary part
        lpAllTypes->aplHC2F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_HC2F


//***************************************************************************
//  $TCA_BOOL_HC4F
//***************************************************************************

void TCA_BOOL_HC4F
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC4F.parts[0] = GetNextFloat (lpaplBoolean, ARRAY_BOOL, uInt);

    // Loop through the remainder of the imaginary HC4F parts
    for (; i < 4; i++)          // No bRet
        // Zero the imaginary part
        lpAllTypes->aplHC4F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_HC4F


//***************************************************************************
//  $TCA_BOOL_HC8F
//***************************************************************************

void TCA_BOOL_HC8F
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC8F.parts[0] = GetNextFloat (lpaplBoolean, ARRAY_BOOL, uInt);

    // Loop through the remainder of the imaginary HC8F parts
    for (; i < 8; i++)          // No bRet
        // Zero the imaginary part
        lpAllTypes->aplHC8F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_HC8F


//***************************************************************************
//  $TCA_BOOL_RAT
//***************************************************************************

void TCA_BOOL_RAT
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Convert the BOOL to a RAT
    mpq_init_set_sx (&lpAllTypes->aplRat, GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt), 1);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_RAT


//***************************************************************************
//  $TCA_BOOL_HC2R
//***************************************************************************

void TCA_BOOL_HC2R
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpq_init_set_sx (&lpAllTypes->aplHC2R.parts[0], GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt), 1);

    // Loop through the remainder of the imaginary HC2R parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0/1
        mpq_init (&lpAllTypes->aplHC2R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_HC2R


//***************************************************************************
//  $TCA_BOOL_HC4R
//***************************************************************************

void TCA_BOOL_HC4R
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpq_init_set_sx (&lpAllTypes->aplHC4R.parts[0], GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt), 1);

    // Loop through the remainder of the imaginary HC4R parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0/1
        mpq_init (&lpAllTypes->aplHC4R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_HC4R


//***************************************************************************
//  $TCA_BOOL_HC8R
//***************************************************************************

void TCA_BOOL_HC8R
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpq_init_set_sx (&lpAllTypes->aplHC8R.parts[0], GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt), 1);

    // Loop through the remainder of the imaginary HC8R parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0/1
        mpq_init (&lpAllTypes->aplHC8R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_HC8R


//***************************************************************************
//  $TCA_BOOL_VFP
//***************************************************************************

void TCA_BOOL_VFP
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part
    mpfr_init_set_sx (&lpAllTypes->aplVfp, GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt), MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_VFP


//***************************************************************************
//  $TCA_BOOL_HC2V
//***************************************************************************

void TCA_BOOL_HC2V
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpfr_init_set_sx (&lpAllTypes->aplHC2V.parts[0], GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt), MPFR_RNDN);

    // Loop through the remainder of the imaginary HC2V parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0
        mpfr_init0 (&lpAllTypes->aplHC2V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_HC2V


//***************************************************************************
//  $TCA_BOOL_HC4V
//***************************************************************************

void TCA_BOOL_HC4V
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpfr_init_set_sx (&lpAllTypes->aplHC4V.parts[0], GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt), MPFR_RNDN);

    // Loop through the remainder of the imaginary HC4V parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0
        mpfr_init0 (&lpAllTypes->aplHC4V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_HC4V


//***************************************************************************
//  $TCA_BOOL_HC8V
//***************************************************************************

void TCA_BOOL_HC8V
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpfr_init_set_sx (&lpAllTypes->aplHC8V.parts[0], GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt), MPFR_RNDN);

    // Loop through the remainder of the imaginary HC8V parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0
        mpfr_init0 (&lpAllTypes->aplHC8V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_HC8V


//***************************************************************************
//  $TCA_INT_BOOL
//***************************************************************************

void TCA_INT_BOOL
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    lpAllTypes->aplInteger = BIT0 & lpaplInteger[uInt];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = IsBooleanValue (lpaplInteger[uInt]);
} // TCA_INT_BOOL


//***************************************************************************
//  $TCA_INT_INT
//***************************************************************************

void TCA_INT_INT
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    lpAllTypes->aplInteger = lpaplInteger[uInt];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_INT


//***************************************************************************
//  $TCA_INT_HC2I
//***************************************************************************

void TCA_INT_HC2I
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC2I.parts[0] = lpaplInteger[uInt];

    // Loop through the remainder of the imaginary HC2I parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC2I.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_HC2I


//***************************************************************************
//  $TCA_INT_HC4I
//***************************************************************************

void TCA_INT_HC4I
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC4I.parts[0] = lpaplInteger[uInt];

    // Loop through the remainder of the imaginary HC4I parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC4I.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_HC4I


//***************************************************************************
//  $TCA_INT_HC8I
//***************************************************************************

void TCA_INT_HC8I
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC8I.parts[0] = lpaplInteger[uInt];

    // Loop through the remainder of the imaginary HC8I parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC8I.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_HC8I


//***************************************************************************
//  $TCA_INT_FLT
//***************************************************************************

void TCA_INT_FLT
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    lpAllTypes->aplFloat = (APLFLOAT) lpaplInteger[uInt];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_FLT


//***************************************************************************
//  $TCA_INT_HC2F
//***************************************************************************

void TCA_INT_HC2F
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC2F.parts[0] = (APLFLOAT) lpaplInteger[uInt];

    // Loop through the remainder of the imaginary HC2F parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC2F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_HC2F


//***************************************************************************
//  $TCA_INT_HC4F
//***************************************************************************

void TCA_INT_HC4F
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC4F.parts[0] = (APLFLOAT) lpaplInteger[uInt];

    // Loop through the remainder of the imaginary HC4F parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC4F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_HC4F


//***************************************************************************
//  $TCA_INT_HC8F
//***************************************************************************

void TCA_INT_HC8F
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC8F.parts[0] = (APLFLOAT) lpaplInteger[uInt];

    // Loop through the remainder of the imaginary HC8F parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC8F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_HC8F


//***************************************************************************
//  $TCA_INT_RAT
//***************************************************************************

void TCA_INT_RAT
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Convert the INT to a RAT
    mpq_init_set_sx (&lpAllTypes->aplRat, lpaplInteger[uInt], 1);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_RAT


//***************************************************************************
//  $TCA_INT_HC2R
//***************************************************************************

void TCA_INT_HC2R
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpq_init_set_sx (&lpAllTypes->aplHC2R.parts[0], lpaplInteger[uInt], 1);

    // Loop through the remainder of the imaginary HC2R parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0/1
        mpq_init (&lpAllTypes->aplHC2R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_HC2R


//***************************************************************************
//  $TCA_INT_HC4R
//***************************************************************************

void TCA_INT_HC4R
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpq_init_set_sx (&lpAllTypes->aplHC4R.parts[0], lpaplInteger[uInt], 1);

    // Loop through the remainder of the imaginary HC4R parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0/1
        mpq_init (&lpAllTypes->aplHC4R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_HC4R


//***************************************************************************
//  $TCA_INT_HC8R
//***************************************************************************

void TCA_INT_HC8R
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpq_init_set_sx (&lpAllTypes->aplHC8R.parts[0], lpaplInteger[uInt], 1);

    // Loop through the remainder of the imaginary HC8R parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0/1
        mpq_init (&lpAllTypes->aplHC8R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_HC8R


//***************************************************************************
//  $TCA_INT_VFP
//***************************************************************************

void TCA_INT_VFP
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    mpfr_init_set_sx (&lpAllTypes->aplVfp, lpaplInteger[uInt], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_VFP


//***************************************************************************
//  $TCA_INT_HC2V
//***************************************************************************

void TCA_INT_HC2V
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpfr_init_set_sx (&lpAllTypes->aplHC2V.parts[0], lpaplInteger[uInt], MPFR_RNDN);

    // Loop through the remainder of the imaginary HC2V parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0
        mpfr_init0 (&lpAllTypes->aplHC2V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_HC2V


//***************************************************************************
//  $TCA_INT_HC4V
//***************************************************************************

void TCA_INT_HC4V
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpfr_init_set_sx (&lpAllTypes->aplHC4V.parts[0], lpaplInteger[uInt], MPFR_RNDN);

    // Loop through the remainder of the imaginary HC4V parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0
        mpfr_init0 (&lpAllTypes->aplHC4V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_HC4V


//***************************************************************************
//  $TCA_INT_HC8V
//***************************************************************************

void TCA_INT_HC8V
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpfr_init_set_sx (&lpAllTypes->aplHC8V.parts[0], lpaplInteger[uInt], MPFR_RNDN);

    // Loop through the remainder of the imaginary HC8V parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0
        mpfr_init0 (&lpAllTypes->aplHC8V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_HC8V


//***************************************************************************
//  $TCA_FLT_BOOL
//***************************************************************************

void TCA_FLT_BOOL
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the FLT to an integer using []CT as called for by lpAllTypes->enumCT
    lpAllTypes->aplInteger = ConvertFltToInt (lpaplFloat, uInt, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
        // Mark as successful or not
        lpbRet[0] &= IsBooleanValue (lpAllTypes->aplInteger);
} // TCA_FLT_BOOL


//***************************************************************************
//  $TCA_FLT_INT
//***************************************************************************

void TCA_FLT_INT
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the FLT to an integer using []CT as called for by lpAllTypes->enumCT
    lpAllTypes->aplInteger = ConvertFltToInt (lpaplFloat, uInt, lpAllTypes, lpbRet);

////// If the flag ptr is valid, ...
////if (lpbRet NE NULL)
////    // Mark as successful
////    lpbRet[0] = TRUE;       // Already set above
} // TCA_FLT_INT


//***************************************************************************
//  $TCA_FLT_HC2I
//***************************************************************************

void TCA_FLT_HC2I
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Attempt to convert the FLT to an integer using []CT as called for by lpAllTypes->enumCT
    lpAllTypes->aplInteger = ConvertFltToInt (lpaplFloat, uInt, lpAllTypes, lpbRet);

    // Loop through the remainder of the imaginary HC2I parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC2I.parts[i] = 0;

////// If the flag ptr is valid, ...
////if (lpbRet NE NULL)
////    // Mark as successful
////    lpbRet[0] = TRUE;       // Already set above
} // TCA_FLT_HC2I


//***************************************************************************
//  $TCA_FLT_HC4I
//***************************************************************************

void TCA_FLT_HC4I
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Attempt to convert the FLT to an integer using []CT as called for by lpAllTypes->enumCT
    lpAllTypes->aplInteger = ConvertFltToInt (lpaplFloat, uInt, lpAllTypes, lpbRet);

    // Loop through the remainder of the imaginary HC4I parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC4I.parts[i] = 0;

////// If the flag ptr is valid, ...
////if (lpbRet NE NULL)
////    // Mark as successful
////    lpbRet[0] = TRUE;       // Already set above
} // TCA_FLT_HC4I


//***************************************************************************
//  $TCA_FLT_HC8I
//***************************************************************************

void TCA_FLT_HC8I
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Attempt to convert the FLT to an integer using []CT as called for by lpAllTypes->enumCT
    lpAllTypes->aplInteger = ConvertFltToInt (lpaplFloat, uInt, lpAllTypes, lpbRet);

    // Loop through the remainder of the imaginary HC8I parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC8I.parts[i] = 0;

////// If the flag ptr is valid, ...
////if (lpbRet NE NULL)
////    // Mark as successful
////    lpbRet[0] = TRUE;       // Already set above
} // TCA_FLT_HC8I


//***************************************************************************
//  $TCA_FLT_FLT
//***************************************************************************

void TCA_FLT_FLT
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the FLT to the result
    lpAllTypes->aplFloat = lpaplFloat[uInt];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_FLT_FLT


//***************************************************************************
//  $TCA_FLT_HC2F
//***************************************************************************

void TCA_FLT_HC2F
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Copy the FLT to the result
    lpAllTypes->aplHC2F.parts[0] = lpaplFloat[uInt];

    // Loop through the remainder of the imaginary HC2F parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC2F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_FLT_HC2F


//***************************************************************************
//  $TCA_FLT_HC4F
//***************************************************************************

void TCA_FLT_HC4F
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Copy the FLT to the result
    lpAllTypes->aplHC4F.parts[0] = lpaplFloat[uInt];

    // Loop through the remainder of the imaginary HC4F parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC4F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_FLT_HC4F


//***************************************************************************
//  $TCA_FLT_HC8F
//***************************************************************************

void TCA_FLT_HC8F
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Copy the FLT to the result
    lpAllTypes->aplHC8F.parts[0] = lpaplFloat[uInt];

    // Loop through the remainder of the imaginary HC8F parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC8F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_FLT_HC8F


//***************************************************************************
//  $TCA_FLT_RAT
//***************************************************************************

void TCA_FLT_RAT
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Convert the FLT to a RAT
    mpq_init_set_d (&lpAllTypes->aplRat, lpaplFloat[uInt]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_FLT_RAT


//***************************************************************************
//  $TCA_FLT_HC2R
//***************************************************************************

void TCA_FLT_HC2R
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpq_init_set_d (&lpAllTypes->aplHC2R.parts[0], lpaplFloat[uInt]);

    // Loop through the remainder of the imaginary HC2R parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0/1
        mpq_init (&lpAllTypes->aplHC2R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_FLT_HC2R


//***************************************************************************
//  $TCA_FLT_HC4R
//***************************************************************************

void TCA_FLT_HC4R
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpq_init_set_d (&lpAllTypes->aplHC4R.parts[0], lpaplFloat[uInt]);

    // Loop through the remainder of the imaginary HC4R parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0/1
        mpq_init (&lpAllTypes->aplHC4R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_FLT_HC4R


//***************************************************************************
//  $TCA_FLT_HC8R
//***************************************************************************

void TCA_FLT_HC8R
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpq_init_set_d (&lpAllTypes->aplHC8R.parts[0], lpaplFloat[uInt]);

    // Loop through the remainder of the imaginary HC8R parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0/1
        mpq_init (&lpAllTypes->aplHC8R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_FLT_HC8R


//***************************************************************************
//  $TCA_FLT_VFP
//***************************************************************************

void TCA_FLT_VFP
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Convert the FLT to a VFP
    mpfr_init_set_d (&lpAllTypes->aplVfp, lpaplFloat[uInt], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_FLT_VFP


//***************************************************************************
//  $TCA_FLT_HC2V
//***************************************************************************

void TCA_FLT_HC2V
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real aprt
    mpfr_init_set_d (&lpAllTypes->aplHC2V.parts[0], lpaplFloat[uInt], MPFR_RNDN);

    // Loop through the remainder of the imaginary HC2V parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0
        mpfr_init0 (&lpAllTypes->aplHC2V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_FLT_HC2V


//***************************************************************************
//  $TCA_FLT_HC4V
//***************************************************************************

void TCA_FLT_HC4V
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real aprt
    mpfr_init_set_d (&lpAllTypes->aplHC4V.parts[0], lpaplFloat[uInt], MPFR_RNDN);

    // Loop through the remainder of the imaginary HC4V parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0
        mpfr_init0 (&lpAllTypes->aplHC4V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_FLT_HC4V


//***************************************************************************
//  $TCA_FLT_HC8V
//***************************************************************************

void TCA_FLT_HC8V
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real aprt
    mpfr_init_set_d (&lpAllTypes->aplHC8V.parts[0], lpaplFloat[uInt], MPFR_RNDN);

    // Loop through the remainder of the imaginary HC8V parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0
        mpfr_init0 (&lpAllTypes->aplHC8V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_FLT_HC8V


//***************************************************************************
//  $TCA_CHAR_CHAR
//***************************************************************************

void TCA_CHAR_CHAR
    (LPAPLCHAR   lpaplChar,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the CHAR
    lpAllTypes->aplChar = lpaplChar[uInt];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_CHAR_CHAR


//***************************************************************************
//  $TCA_HETE_HETE
//***************************************************************************

void TCA_HETE_HETE
    (LPAPLHETERO lpaplHetero,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the HETERO to HETERO
    lpAllTypes->aplHetero = CopySymGlbDir_PTB (((LPAPLHETERO) ClrPtrTypeDir (lpaplHetero))[uInt]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HETE_HETE


//***************************************************************************
//  $TCA_NEST_BOOL
//***************************************************************************

void TCA_NEST_BOOL
    (LPAPLNESTED lpaplNested,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
#define lpSymEntry  ((LPSYMENTRY *) lpaplNested)[uInt]
#define arrType     TranslateImmTypeToArrayType (lpSymEntry->stFlags.ImmType)

    // Split cases based upon the ptr type bits
    switch (GetPtrTypeDir (lpaplNested[uInt]))
    {
        case PTRTYPE_STCONST:
            // If it's numeric, ...
            lpAllTypes->aplInteger =
              ConvertToInteger_SCT (arrType,                        // Arg storage type
                                   &lpSymEntry->stData.stLongest,   // Ptr to global memory
                                    0,                              // Index into ...
                                    lpbRet);                        // Ptr to TRUE iff the result is valid
            // If the flag ptr is valid, ...
            if (lpbRet NE NULL)
                // Ensure the value is Boolean
                lpbRet[0] = IsBooleanValue (lpAllTypes->aplInteger);

            break;

        case PTRTYPE_HGLOBAL:
            // If the flag ptr is valid, ...
            if (lpbRet NE NULL)
                // Mark as in error
                lpbRet[0] = FALSE;
            break;

        defstop
            break;
    } // End SWITCH
#undef  arrType
#undef  lpSymEntry
} // TCA_NEST_BOOL


//***************************************************************************
//  $TCA_NEST_INT
//***************************************************************************

void TCA_NEST_INT
    (LPAPLNESTED lpaplNested,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
#define lpSymEntry  ((LPSYMENTRY *) lpaplNested)[uInt]
#define arrType     TranslateImmTypeToArrayType (lpSymEntry->stFlags.ImmType)

    // Split cases based upon the ptr type bits
    switch (GetPtrTypeDir (lpSymEntry))
    {
        case PTRTYPE_STCONST:
            // If it's numeric, ...
            lpAllTypes->aplInteger =
              ConvertToInteger_SCT (arrType,                        // Arg storage type
                                   &lpSymEntry->stData.stLongest,   // Ptr to global memory
                                    0,                              // Index into ...
                                    lpbRet);                        // Ptr to TRUE iff the result is valid
            break;

        case PTRTYPE_HGLOBAL:
            // If the flag ptr is valid, ...
            if (lpbRet NE NULL)
                // Mark as in error
                lpbRet[0] = FALSE;
            break;

        defstop
            break;
    } // End SWITCH
#undef  arrType
#undef  lpSymEntry
} // TCA_NEST_INT


//***************************************************************************
//  $TCA_NEST_FLT
//***************************************************************************

void TCA_NEST_FLT
    (LPAPLNESTED lpaplNested,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
#define lpSymEntry  ((LPSYMENTRY *) lpaplNested)[uInt]
#define arrType     TranslateImmTypeToArrayType (lpSymEntry->stFlags.ImmType)

    // Split cases based upon the ptr type bits
    switch (GetPtrTypeDir (lpaplNested[uInt]))
    {
        case PTRTYPE_STCONST:
            // If it's numeric, ...
            lpAllTypes->aplFloat =
              ConvertToFloat (arrType,                      // Arg storage type
                             &lpSymEntry->stData.stLongest, // Ptr to global memory
                              lpbRet);                      // Ptr to TRUE iff the result is valid
            break;

        case PTRTYPE_HGLOBAL:
            // If the flag ptr is valid, ...
            if (lpbRet NE NULL)
                // Mark as in error
                lpbRet[0] = FALSE;
            break;

        defstop
            break;
    } // End SWITCH
#undef  arrType
#undef  lpSymEntry
} // TCA_NEST_FLT


//***************************************************************************
//  $TCA_NEST_CHAR
//***************************************************************************

void TCA_NEST_CHAR
    (LPAPLNESTED lpaplNested,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    UBOOL bRet;

    // Split cases based upon the ptr type bits
    switch (GetPtrTypeDir (lpaplNested[uInt]))
    {
        case PTRTYPE_STCONST:
            // Determine if it's a character
            bRet = IsImmChr (((LPSYMENTRY) lpaplNested)[uInt].stFlags.ImmType);

            // If it's character, ...
            if (bRet)
                lpAllTypes->aplChar = ((LPSYMENTRY) lpaplNested)[uInt].stData.stChar;
            // If the flag ptr is valid, ...
            if (lpbRet NE NULL)
                // Mark as done
                lpbRet[0] = bRet;
            break;

        case PTRTYPE_HGLOBAL:
            // If the flag ptr is valid, ...
            if (lpbRet NE NULL)
                // Mark as in error
                lpbRet[0] = FALSE;
            break;

        defstop
            break;
    } // End SWITCH
} // TCA_NEST_CHAR


//***************************************************************************
//  $TCA_NEST_NEST
//***************************************************************************

void TCA_NEST_NEST
    (LPAPLNESTED lpaplNested,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the NESTED to NESTED
    lpAllTypes->aplNested = CopySymGlbDir_PTB (((LPAPLNESTED) ClrPtrTypeDir (lpaplNested))[uInt]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_NEST_NEST


//***************************************************************************
//  $TCA_APA_BOOL
//***************************************************************************

void TCA_APA_BOOL
    (LPAPLAPA    lpaplApa,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    UBOOL bRet[2];

    // Save in the result
    lpAllTypes->aplInteger = iadd64 (lpaplApa->Off, imul64 (lpaplApa->Mul, uInt, &bRet[0], EXCEPTION_RESULT_FLOAT), &bRet[1], EXCEPTION_RESULT_FLOAT);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1]
                 && IsBooleanValue (lpAllTypes->aplInteger);
} // TCA_APA_BOOL


//***************************************************************************
//  $TCA_APA_INT
//***************************************************************************

void TCA_APA_INT
    (LPAPLAPA    lpaplApa,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    UBOOL bRet[2];

    // Save in the result
    lpAllTypes->aplInteger = iadd64 (lpaplApa->Off, imul64 (lpaplApa->Mul, uInt, &bRet[0], EXCEPTION_RESULT_FLOAT), &bRet[1], EXCEPTION_RESULT_FLOAT);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_APA_INT


//***************************************************************************
//  $TCA_APA_HC2I
//***************************************************************************

void TCA_APA_HC2I
    (LPAPLAPA    lpaplApa,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i = 1;
    UBOOL bRet[2];

    // Save in the result
    lpAllTypes->aplHC2I.parts[0] = iadd64 (lpaplApa->Off, imul64 (lpaplApa->Mul, uInt, &bRet[0], EXCEPTION_RESULT_FLOAT), &bRet[1], EXCEPTION_RESULT_FLOAT);

    // Loop through the remainder of the imaginary HC2I parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC2I.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_APA_HC2I


//***************************************************************************
//  $TCA_APA_HC4I
//***************************************************************************

void TCA_APA_HC4I
    (LPAPLAPA    lpaplApa,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i = 1;
    UBOOL bRet[2];

    // Save in the result
    lpAllTypes->aplHC4I.parts[0] = iadd64 (lpaplApa->Off, imul64 (lpaplApa->Mul, uInt, &bRet[0], EXCEPTION_RESULT_FLOAT), &bRet[1], EXCEPTION_RESULT_FLOAT);

    // Loop through the remainder of the imaginary HC4I parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC4I.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_APA_HC4I


//***************************************************************************
//  $TCA_APA_HC8I
//***************************************************************************

void TCA_APA_HC8I
    (LPAPLAPA    lpaplApa,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i = 1;
    UBOOL bRet[2];

    // Save in the result
    lpAllTypes->aplHC8I.parts[0] = iadd64 (lpaplApa->Off, imul64 (lpaplApa->Mul, uInt, &bRet[0], EXCEPTION_RESULT_FLOAT), &bRet[1], EXCEPTION_RESULT_FLOAT);

    // Loop through the remainder of the imaginary HC8I parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC8I.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_APA_HC8I


//***************************************************************************
//  $TCA_APA_FLT
//***************************************************************************

void TCA_APA_FLT
    (LPAPLAPA    lpaplApa,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    UBOOL bRet[2];

    // Save in the result
    lpAllTypes->aplFloat = (APLFLOAT) iadd64 (lpaplApa->Off, imul64 (lpaplApa->Mul, uInt, &bRet[0], EXCEPTION_RESULT_FLOAT), &bRet[1], EXCEPTION_RESULT_FLOAT);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_APA_FLT


//***************************************************************************
//  $TCA_APA_HC2F
//***************************************************************************

void TCA_APA_HC2F
    (LPAPLAPA    lpaplApa,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i = 1;
    UBOOL bRet[2];

    // Save in the result
    lpAllTypes->aplHC2F.parts[0] = (APLFLOAT) iadd64 (lpaplApa->Off, imul64 (lpaplApa->Mul, uInt, &bRet[0], EXCEPTION_RESULT_FLOAT), &bRet[1], EXCEPTION_RESULT_FLOAT);

    // Loop through the remainder of the imaginary HC2F parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC2F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_APA_HC2F


//***************************************************************************
//  $TCA_APA_HC4F
//***************************************************************************

void TCA_APA_HC4F
    (LPAPLAPA    lpaplApa,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i = 1;
    UBOOL bRet[2];

    // Save in the result
    lpAllTypes->aplHC4F.parts[0] = (APLFLOAT) iadd64 (lpaplApa->Off, imul64 (lpaplApa->Mul, uInt, &bRet[0], EXCEPTION_RESULT_FLOAT), &bRet[1], EXCEPTION_RESULT_FLOAT);

    // Loop through the remainder of the imaginary HC4F parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC4F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_APA_HC4F


//***************************************************************************
//  $TCA_APA_HC8F
//***************************************************************************

void TCA_APA_HC8F
    (LPAPLAPA    lpaplApa,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i = 1;
    UBOOL bRet[2];

    // Save in the result
    lpAllTypes->aplHC8F.parts[0] = (APLFLOAT) iadd64 (lpaplApa->Off, imul64 (lpaplApa->Mul, uInt, &bRet[0], EXCEPTION_RESULT_FLOAT), &bRet[1], EXCEPTION_RESULT_FLOAT);

    // Loop through the remainder of the imaginary HC8F parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC8F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_APA_HC8F


//***************************************************************************
//  $TCA_APA_RAT
//***************************************************************************

void TCA_APA_RAT
    (LPAPLAPA    lpaplApa,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    UBOOL bRet[2];

    // Convert the APA to a RAT
    mpq_init_set_sx (&lpAllTypes->aplRat, iadd64 (lpaplApa->Off, imul64 (lpaplApa->Mul, uInt, &bRet[0], EXCEPTION_RESULT_FLOAT), &bRet[1], EXCEPTION_RESULT_FLOAT), 1);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_APA_RAT


//***************************************************************************
//  $TCA_APA_HC2R
//***************************************************************************

void TCA_APA_HC2R
    (LPAPLAPA    lpaplApa,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i = 1;
    UBOOL bRet[2];

    // Set the real part
    mpq_init_set_sx (&lpAllTypes->aplHC2R.parts[0], iadd64 (lpaplApa->Off, imul64 (lpaplApa->Mul, uInt, &bRet[0], EXCEPTION_RESULT_FLOAT), &bRet[1], EXCEPTION_RESULT_FLOAT), 1);

    // Loop through the remainder of the imaginary HC2R parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary part to 0/1
        mpq_init (&lpAllTypes->aplHC2R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_APA_HC2R


//***************************************************************************
//  $TCA_APA_HC4R
//***************************************************************************

void TCA_APA_HC4R
    (LPAPLAPA    lpaplApa,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i = 1;
    UBOOL bRet[2];

    // Set the real part
    mpq_init_set_sx (&lpAllTypes->aplHC4R.parts[0], iadd64 (lpaplApa->Off, imul64 (lpaplApa->Mul, uInt, &bRet[0], EXCEPTION_RESULT_FLOAT), &bRet[1], EXCEPTION_RESULT_FLOAT), 1);

    // Loop through the remainder of the imaginary HC4R parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary part to 0/1
        mpq_init (&lpAllTypes->aplHC4R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_APA_HC4R


//***************************************************************************
//  $TCA_APA_HC8R
//***************************************************************************

void TCA_APA_HC8R
    (LPAPLAPA    lpaplApa,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i = 1;
    UBOOL bRet[2];

    // Set the real part
    mpq_init_set_sx (&lpAllTypes->aplHC8R.parts[0], iadd64 (lpaplApa->Off, imul64 (lpaplApa->Mul, uInt, &bRet[0], EXCEPTION_RESULT_FLOAT), &bRet[1], EXCEPTION_RESULT_FLOAT), 1);

    // Loop through the remainder of the imaginary HC8R parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary part to 0/1
        mpq_init (&lpAllTypes->aplHC8R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_APA_HC8R


//***************************************************************************
//  $TCA_APA_VFP
//***************************************************************************

void TCA_APA_VFP
    (LPAPLAPA    lpaplApa,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    UBOOL bRet[2];

    mpfr_init_set_sx (&lpAllTypes->aplVfp, iadd64 (lpaplApa->Off, imul64 (lpaplApa->Mul, uInt, &bRet[0], EXCEPTION_RESULT_FLOAT), &bRet[1], EXCEPTION_RESULT_FLOAT), 1);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_APA_VFP


//***************************************************************************
//  $TCA_APA_HC2V
//***************************************************************************

void TCA_APA_HC2V
    (LPAPLAPA    lpaplApa,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i = 1;
    UBOOL bRet[2];

    // Set the real part
    mpfr_init_set_sx (&lpAllTypes->aplHC2V.parts[0], iadd64 (lpaplApa->Off, imul64 (lpaplApa->Mul, uInt, &bRet[0], EXCEPTION_RESULT_FLOAT), &bRet[1], EXCEPTION_RESULT_FLOAT), 1);

    // Loop through the remainder of the imaginary HC2V parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary part to 0
        mpfr_init0 (&lpAllTypes->aplHC2V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_APA_HC2V


//***************************************************************************
//  $TCA_APA_HC4V
//***************************************************************************

void TCA_APA_HC4V
    (LPAPLAPA    lpaplApa,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i = 1;
    UBOOL bRet[2];

    // Set the real part
    mpfr_init_set_sx (&lpAllTypes->aplHC4V.parts[0], iadd64 (lpaplApa->Off, imul64 (lpaplApa->Mul, uInt, &bRet[0], EXCEPTION_RESULT_FLOAT), &bRet[1], EXCEPTION_RESULT_FLOAT), 1);

    // Loop through the remainder of the imaginary HC4V parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary part to 0
        mpfr_init0 (&lpAllTypes->aplHC4V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_APA_HC4V


//***************************************************************************
//  $TCA_APA_HC8V
//***************************************************************************

void TCA_APA_HC8V
    (LPAPLAPA    lpaplApa,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i = 1;
    UBOOL bRet[2];

    // Set the real part
    mpfr_init_set_sx (&lpAllTypes->aplHC8V.parts[0], iadd64 (lpaplApa->Off, imul64 (lpaplApa->Mul, uInt, &bRet[0], EXCEPTION_RESULT_FLOAT), &bRet[1], EXCEPTION_RESULT_FLOAT), 1);

    // Loop through the remainder of the imaginary HC8V parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary part to 0
        mpfr_init0 (&lpAllTypes->aplHC8V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_APA_HC8V


//***************************************************************************
//  $TCA_RAT_BOOL
//***************************************************************************

void TCA_RAT_BOOL
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    lpAllTypes->aplInteger = ConvertRatToInt (lpaplRat, uInt, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
        // Mark as successful or not
        lpbRet[0] &= IsBooleanValue (lpAllTypes->aplInteger);
} // TCA_RAT_BOOL


//***************************************************************************
//  $TCA_RAT_INT
//***************************************************************************

void TCA_RAT_INT
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    lpAllTypes->aplInteger = ConvertRatToInt (lpaplRat, uInt, lpAllTypes, lpbRet);

////// If the flag ptr is valid, ...
////if (lpbRet NE NULL)
////    // Mark as successful
////    lpbRet[0] = TRUE;       // Already set above
} // TCA_RAT_INT


//***************************************************************************
//  $TCA_RAT_HC2I
//***************************************************************************

void TCA_RAT_HC2I
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC2I.parts[0] = ConvertRatToInt (lpaplRat, uInt, lpAllTypes, lpbRet);

    // Loop through the remainder of the imaginary HC2I parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC2I.parts[i] = 0;

////// If the flag ptr is valid, ...
////if (lpbRet NE NULL)
////    // Mark as successful
////    lpbRet[0] = TRUE;       // Already set above
} // TCA_RAT_HC2I


//***************************************************************************
//  $TCA_RAT_HC4I
//***************************************************************************

void TCA_RAT_HC4I
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC4I.parts[0] = ConvertRatToInt (lpaplRat, uInt, lpAllTypes, lpbRet);

    // Loop through the remainder of the imaginary HC4I parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC4I.parts[i] = 0;

////// If the flag ptr is valid, ...
////if (lpbRet NE NULL)
////    // Mark as successful
////    lpbRet[0] = TRUE;       // Already set above
} // TCA_RAT_HC4I


//***************************************************************************
//  $TCA_RAT_HC8I
//***************************************************************************

void TCA_RAT_HC8I
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC8I.parts[0] = ConvertRatToInt (lpaplRat, uInt, lpAllTypes, lpbRet);

    // Loop through the remainder of the imaginary HC8I parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC8I.parts[i] = 0;

////// If the flag ptr is valid, ...
////if (lpbRet NE NULL)
////    // Mark as successful
////    lpbRet[0] = TRUE;       // Already set above
} // TCA_RAT_HC8I


//***************************************************************************
//  $TCA_RAT_FLT
//***************************************************************************

void TCA_RAT_FLT
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the RAT as FLT
    lpAllTypes->aplFloat = mpq_get_d (&lpaplRat[uInt]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_RAT_FLT


//***************************************************************************
//  $TCA_RAT_HC2F
//***************************************************************************

void TCA_RAT_HC2F
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC2F.parts[0] = mpq_get_d (&lpaplRat[uInt]);

    // Loop through the remainder of the imaginary HC2F parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC2F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_RAT_HC2F


//***************************************************************************
//  $TCA_RAT_HC4F
//***************************************************************************

void TCA_RAT_HC4F
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC4F.parts[0] = mpq_get_d (&lpaplRat[uInt]);

    // Loop through the remainder of the imaginary HC4F parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC4F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_RAT_HC4F


//***************************************************************************
//  $TCA_RAT_HC8F
//***************************************************************************

void TCA_RAT_HC8F
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC8F.parts[0] = mpq_get_d (&lpaplRat[uInt]);

    // Loop through the remainder of the imaginary HC8F parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC8F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_RAT_HC8F


//***************************************************************************
//  $TCA_RAT_RAT
//***************************************************************************

void TCA_RAT_RAT
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the RAT to a RAT
    mpq_init_set (&lpAllTypes->aplRat, &lpaplRat[uInt]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_RAT_RAT


//***************************************************************************
//  $TCA_RAT_HC2R
//***************************************************************************

void TCA_RAT_HC2R
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpq_init_set (&lpAllTypes->aplHC2R.parts[0], &lpaplRat[uInt]);

    // Loop through the remainder of the imaginary HC2R parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0/1
        mpq_init (&lpAllTypes->aplHC2R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_RAT_HC2R


//***************************************************************************
//  $TCA_RAT_HC4R
//***************************************************************************

void TCA_RAT_HC4R
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpq_init_set (&lpAllTypes->aplHC4R.parts[0], &lpaplRat[uInt]);

    // Loop through the remainder of the imaginary HC4R parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0/1
        mpq_init (&lpAllTypes->aplHC4R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_RAT_HC4R


//***************************************************************************
//  $TCA_RAT_HC8R
//***************************************************************************

void TCA_RAT_HC8R
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpq_init_set (&lpAllTypes->aplHC8R.parts[0], &lpaplRat[uInt]);

    // Loop through the remainder of the imaginary HC8R parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0/1
        mpq_init (&lpAllTypes->aplHC8R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_RAT_HC8R


//***************************************************************************
//  $TCA_RAT_VFP
//***************************************************************************

void TCA_RAT_VFP
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the RAT as VFP
    mpfr_init_set_q  (&lpAllTypes->aplVfp, &lpaplRat[uInt], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_RAT_VFP


//***************************************************************************
//  $TCA_RAT_HC2V
//***************************************************************************

void TCA_RAT_HC2V
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpfr_init_set_q  (&lpAllTypes->aplHC2V.parts[0], &lpaplRat[uInt], MPFR_RNDN);

    // Loop through the remainder of the imaginary HC2V parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0
        mpfr_init0 (&lpAllTypes->aplHC2V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_RAT_HC2V


//***************************************************************************
//  $TCA_RAT_HC4V
//***************************************************************************

void TCA_RAT_HC4V
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpfr_init_set_q  (&lpAllTypes->aplHC4V.parts[0], &lpaplRat[uInt], MPFR_RNDN);

    // Loop through the remainder of the imaginary HC4V parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0
        mpfr_init0 (&lpAllTypes->aplHC4V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_RAT_HC4V


//***************************************************************************
//  $TCA_RAT_HC8V
//***************************************************************************

void TCA_RAT_HC8V
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpfr_init_set_q  (&lpAllTypes->aplHC8V.parts[0], &lpaplRat[uInt], MPFR_RNDN);

    // Loop through the remainder of the imaginary HC8V parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0
        mpfr_init0 (&lpAllTypes->aplHC8V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_RAT_HC8V


//***************************************************************************
//  $TCA_VFP_BOOL
//***************************************************************************

void TCA_VFP_BOOL
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the VFP to an integer using []CT as called for by lpAllTypes->enumCT
    lpAllTypes->aplInteger = ConvertVfpToInt (&lpaplVfp[uInt], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
        // Mark as successful or not
        lpbRet[0] &= IsBooleanValue (lpAllTypes->aplInteger);
} // TCA_VFP_BOOL


//***************************************************************************
//  $TCA_VFP_INT
//***************************************************************************

void TCA_VFP_INT
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the VFP to an integer using []CT as called for by lpAllTypes->enumCT
    lpAllTypes->aplInteger = ConvertVfpToInt (&lpaplVfp[uInt], 0, lpAllTypes, lpbRet);

////// If the flag ptr is valid, ...
////if (lpbRet NE NULL)
////    // Mark as successful
////    lpbRet[0] = TRUE;       // Already set above
} // TCA_VFP_INT


//***************************************************************************
//  $TCA_VFP_HC2I
//***************************************************************************

void TCA_VFP_HC2I
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Attempt to convert the VFP to an integer using []CT as called for by lpAllTypes->enumCT
    lpAllTypes->aplInteger = ConvertVfpToInt (&lpaplVfp[uInt], 0, lpAllTypes, lpbRet);

    // Loop through the remainder of the imaginary HC2I parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC2I.parts[i] = 0;

////// If the flag ptr is valid, ...
////if (lpbRet NE NULL)
////    // Mark as successful
////    lpbRet[0] = TRUE;       // Already set above
} // TCA_VFP_HC2I


//***************************************************************************
//  $TCA_VFP_HC4I
//***************************************************************************

void TCA_VFP_HC4I
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Attempt to convert the VFP to an integer using []CT as called for by lpAllTypes->enumCT
    lpAllTypes->aplInteger = ConvertVfpToInt (&lpaplVfp[uInt], 0, lpAllTypes, lpbRet);

    // Loop through the remainder of the imaginary HC4I parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC4I.parts[i] = 0;

////// If the flag ptr is valid, ...
////if (lpbRet NE NULL)
////    // Mark as successful
////    lpbRet[0] = TRUE;       // Already set above
} // TCA_VFP_HC4I


//***************************************************************************
//  $TCA_VFP_HC8I
//***************************************************************************

void TCA_VFP_HC8I
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Attempt to convert the VFP to an integer using []CT as called for by lpAllTypes->enumCT
    lpAllTypes->aplInteger = ConvertVfpToInt (&lpaplVfp[uInt], 0, lpAllTypes, lpbRet);

    // Loop through the remainder of the imaginary HC8I parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC8I.parts[i] = 0;

////// If the flag ptr is valid, ...
////if (lpbRet NE NULL)
////    // Mark as successful
////    lpbRet[0] = TRUE;       // Already set above
} // TCA_VFP_HC8I


//***************************************************************************
//  $TCA_VFP_FLT
//***************************************************************************

void TCA_VFP_FLT
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the VFP as FLT
    lpAllTypes->aplFloat = mpfr_get_d (&lpaplVfp[uInt], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_VFP_FLT


//***************************************************************************
//  $TCA_VFP_HC2F
//***************************************************************************

void TCA_VFP_HC2F
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC2F.parts[0] = mpfr_get_d (&lpaplVfp[uInt], MPFR_RNDN);

    // Loop through the remainder of the imaginary HC2F parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC2F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_VFP_HC2F


//***************************************************************************
//  $TCA_VFP_HC4F
//***************************************************************************

void TCA_VFP_HC4F
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC4F.parts[0] = mpfr_get_d (&lpaplVfp[uInt], MPFR_RNDN);

    // Loop through the remainder of the imaginary HC4F parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC4F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_VFP_HC4F


//***************************************************************************
//  $TCA_VFP_HC8F
//***************************************************************************

void TCA_VFP_HC8F
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    lpAllTypes->aplHC8F.parts[0] = mpfr_get_d (&lpaplVfp[uInt], MPFR_RNDN);

    // Loop through the remainder of the imaginary HC8F parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC8F.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_VFP_HC8F


//***************************************************************************
//  $TCA_VFP_RAT
//***************************************************************************

void TCA_VFP_RAT
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the VFP to a RAT
    mpq_init_set_fr (&lpAllTypes->aplRat, &lpaplVfp[uInt]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_VFP_RAT


//***************************************************************************
//  $TCA_VFP_HC2R
//***************************************************************************

void TCA_VFP_HC2R
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpq_init_set_fr (&lpAllTypes->aplHC2R.parts[0], &lpaplVfp[uInt]);

    // Loop through the remainder of the imaginary HC2R parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0/1
        mpq_init (&lpAllTypes->aplHC2R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_VFP_HC2R


//***************************************************************************
//  $TCA_VFP_HC4R
//***************************************************************************

void TCA_VFP_HC4R
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpq_init_set_fr (&lpAllTypes->aplHC4R.parts[0], &lpaplVfp[uInt]);

    // Loop through the remainder of the imaginary HC4R parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0/1
        mpq_init (&lpAllTypes->aplHC4R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_VFP_HC4R


//***************************************************************************
//  $TCA_VFP_HC8R
//***************************************************************************

void TCA_VFP_HC8R
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpq_init_set_fr (&lpAllTypes->aplHC8R.parts[0], &lpaplVfp[uInt]);

    // Loop through the remainder of the imaginary HC8R parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0/1
        mpq_init (&lpAllTypes->aplHC8R.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_VFP_HC8R


//***************************************************************************
//  $TCA_VFP_VFP
//***************************************************************************

void TCA_VFP_VFP
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the VFP as VFP
    mpfr_init_set (&lpAllTypes->aplVfp, &lpaplVfp[uInt], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_VFP_VFP


//***************************************************************************
//  $TCA_VFP_HC2V
//***************************************************************************

void TCA_VFP_HC2V
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpfr_init_set (&lpAllTypes->aplHC2V.parts[0], &lpaplVfp[uInt], MPFR_RNDN);

    // Loop through the remainder of the imaginary HC2V parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0
        mpfr_init0 (&lpAllTypes->aplHC2V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_VFP_HC2V


//***************************************************************************
//  $TCA_VFP_HC4V
//***************************************************************************

void TCA_VFP_HC4V
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpfr_init_set (&lpAllTypes->aplHC4V.parts[0], &lpaplVfp[uInt], MPFR_RNDN);

    // Loop through the remainder of the imaginary HC4V parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0
        mpfr_init0 (&lpAllTypes->aplHC4V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_VFP_HC4V


//***************************************************************************
//  $TCA_VFP_HC8V
//***************************************************************************

void TCA_VFP_HC8V
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    mpfr_init_set (&lpAllTypes->aplHC8V.parts[0], &lpaplVfp[uInt], MPFR_RNDN);

    // Loop through the remainder of the imaginary HC8V parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0
        mpfr_init0 (&lpAllTypes->aplHC8V.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_VFP_HC8V


//***************************************************************************
//  $TCA_HC2I_BOOL
//***************************************************************************

void TCA_HC2I_BOOL
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to BOOL
    lpAllTypes->aplInteger = lpaplHC2I[uInt].parts[0];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful or not
        lpbRet[0] = IsBooleanValue (lpAllTypes->aplInteger);

        // Loop through the remainder of the imaginary HC2I parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2I part is 0
            lpbRet[0] &= lpaplHC2I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC2I_BOOL


//***************************************************************************
//  $TCA_HC2I_INT
//***************************************************************************

void TCA_HC2I_INT
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to INT
    lpAllTypes->aplInteger = lpaplHC2I[uInt].parts[0];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2I parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2I part is 0
            lpbRet[0] &= lpaplHC2I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC2I_INT


//***************************************************************************
//  $TCA_HC2I_HC2I
//***************************************************************************

void TCA_HC2I_HC2I
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the HC2I as HC2I
    lpAllTypes->aplHC2I = lpaplHC2I[uInt];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2I_HC2I


//***************************************************************************
//  $TCA_HC2I_HC4I
//***************************************************************************

void TCA_HC2I_HC4I
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Zero the HC4I memory
    ZeroMemory (&lpAllTypes->aplHC4I, sizeof (lpAllTypes->aplHC4I));

    // Copy the HC2I as HC4I
    lpAllTypes->aplHC4I.partsLo[0] = lpaplHC2I[uInt];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2I_HC4I


//***************************************************************************
//  $TCA_HC2I_HC8I
//***************************************************************************

void TCA_HC2I_HC8I
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Zero the HC8I memory
    ZeroMemory (&lpAllTypes->aplHC8I, sizeof (lpAllTypes->aplHC8I));

    // Copy the HC2I as HC8I
    lpAllTypes->aplHC8I.partsLo[0].partsLo[0] = lpaplHC2I[uInt];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2I_HC8I


//***************************************************************************
//  $TCA_HC2I_FLT
//***************************************************************************

void TCA_HC2I_FLT
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to FLT
    lpAllTypes->aplFloat = (APLFLOAT) lpaplHC2I[uInt].parts[0];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2I parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2I part is 0
            lpbRet[0] &= lpaplHC2I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC2I_FLT


//***************************************************************************
//  $TCA_HC2I_HC2F
//***************************************************************************

void TCA_HC2I_HC2F
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

////// Zero the HC2F memory
////ZeroMemory (&lpAllTypes->aplHC2F, sizeof (lpAllTypes->aplHC2F));

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2I as HC2F
        lpAllTypes->aplHC2F.parts[i] = (APLFLOAT) lpaplHC2I[uInt].parts[i];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2I_HC2F


//***************************************************************************
//  $TCA_HC2I_HC4F
//***************************************************************************

void TCA_HC2I_HC4F
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Zero the HC4F memory
    ZeroMemory (&lpAllTypes->aplHC4F, sizeof (lpAllTypes->aplHC4F));

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2I as HC4F
        lpAllTypes->aplHC4F.parts[i] = (APLFLOAT) lpaplHC2I[uInt].parts[i];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2I_HC4F


//***************************************************************************
//  $TCA_HC2I_HC8F
//***************************************************************************

void TCA_HC2I_HC8F
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Zero the HC8F memory
    ZeroMemory (&lpAllTypes->aplHC8F, sizeof (lpAllTypes->aplHC8F));

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2I as HC8F
        lpAllTypes->aplHC8F.parts[i] = (APLFLOAT) lpaplHC2I[uInt].parts[i];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2I_HC8F


//***************************************************************************
//  $TCA_HC2I_RAT
//***************************************************************************

void TCA_HC2I_RAT
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Convert the real part of HC2I to a RAT
    mpq_init_set_sx (&lpAllTypes->aplRat, lpaplHC2I[uInt].parts[0], 1);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2I parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2I part is 0
            lpbRet[0] &= lpaplHC2I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC2I_RAT


//***************************************************************************
//  $TCA_HC2I_HC2R
//***************************************************************************

void TCA_HC2I_HC2R
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Set the parts to HC2R
        mpq_init_set_sx (&lpAllTypes->aplHC2R.parts[i], lpaplHC2I[uInt].parts[i], 1);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2I_HC2R


//***************************************************************************
//  $TCA_HC2I_HC4R
//***************************************************************************

void TCA_HC2I_HC4R
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0/1
    mphc4r_init (&lpAllTypes->aplHC4R);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2I parts to HC4R
        mpq_set_sx (&lpAllTypes->aplHC4R.parts[i], lpaplHC2I[uInt].parts[i], 1);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2I_HC4R


//***************************************************************************
//  $TCA_HC2I_HC8R
//***************************************************************************

void TCA_HC2I_HC8R
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0/1
    mphc8r_init (&lpAllTypes->aplHC8R);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2I parts to HC8R
        mpq_set_sx (&lpAllTypes->aplHC8R.parts[i], lpaplHC2I[uInt].parts[i], 1);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2I_HC8R


//***************************************************************************
//  $TCA_HC2I_VFP
//***************************************************************************

void TCA_HC2I_VFP
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to VFP
    mpfr_init_set_sx (&lpAllTypes->aplVfp, lpaplHC2I[uInt].parts[0], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2I parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2I part is 0
            lpbRet[0] &= lpaplHC2I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC2I_VFP


//***************************************************************************
//  $TCA_HC2I_HC2V
//***************************************************************************

void TCA_HC2I_HC2V
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2I parts to HC2V
        mpfr_init_set_sx (&lpAllTypes->aplHC2V.parts[i], lpaplHC2I[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2I_HC2V


//***************************************************************************
//  $TCA_HC2I_HC4V
//***************************************************************************

void TCA_HC2I_HC4V
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2I parts to HC4V
        mpfr_set_sx (&lpAllTypes->aplHC4V.parts[i], lpaplHC2I[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2I_HC4V


//***************************************************************************
//  $TCA_HC2I_HC8V
//***************************************************************************

void TCA_HC2I_HC8V
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2I parts to HC8V
        mpfr_set_sx (&lpAllTypes->aplHC8V.parts[i], lpaplHC2I[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2I_HC8V


//***************************************************************************
//  $TCA_HC2F_BOOL
//***************************************************************************

void TCA_HC2F_BOOL
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the HC2F real part to an integer using System CT
    lpAllTypes->aplInteger = ConvertFltToInt (&lpaplHC2F[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Mark as successful or not
        lpbRet[0] &= IsBooleanValue (lpAllTypes->aplInteger);

        // Loop through the remainder of the imaginary HC2F parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2F part is a near 0
            lpbRet[0] &= (ConvertFltToInt (&lpaplHC2F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0);
    } // End IF
} // TCA_HC2F_BOOL


//***************************************************************************
//  $TCA_HC2F_INT
//***************************************************************************

void TCA_HC2F_INT
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the HC2F real part to an integer using System CT
    lpAllTypes->aplInteger = ConvertFltToInt (&lpaplHC2F[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Loop through the remainder of the imaginary HC2F parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2F part is a near 0
            lpbRet[0] &= (ConvertFltToInt (&lpaplHC2F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0);
    } // End IF
} // TCA_HC2F_INT


//***************************************************************************
//  $TCA_HC2F_HC2I
//***************************************************************************

void TCA_HC2F_HC2I
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert the HC2F to HC2I using []CT as called for by lpAllTypes->enumCT
        lpAllTypes->aplHC2I.parts[i] = ConvertFltToInt (&lpaplHC2F[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_HC2F_HC2I


//***************************************************************************
//  $TCA_HC2F_HC4I
//***************************************************************************

void TCA_HC2F_HC4I
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Zero the HC4I memory
    ZeroMemory (&lpAllTypes->aplHC4I, sizeof (lpAllTypes->aplHC4I));

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert the HC2F to HC4I using []CT as called for by lpAllTypes->enumCT
        lpAllTypes->aplHC4I.parts[i] = ConvertFltToInt (&lpaplHC2F[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_HC2F_HC4I


//***************************************************************************
//  $TCA_HC2F_HC8I
//***************************************************************************

void TCA_HC2F_HC8I
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Zero the HC8I memory
    ZeroMemory (&lpAllTypes->aplHC8I, sizeof (lpAllTypes->aplHC8I));

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert the HC2F to HC8I using []CT as called for by lpAllTypes->enumCT
        lpAllTypes->aplHC8I.parts[i] = ConvertFltToInt (&lpaplHC2F[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_HC2F_HC8I


//***************************************************************************
//  $TCA_HC2F_FLT
//***************************************************************************

void TCA_HC2F_FLT
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to FLT
    lpAllTypes->aplFloat = lpaplHC2F[uInt].parts[0];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2F parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2F part is a near 0
            lpbRet[0] &= (ConvertFltToInt (&lpaplHC2F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0);
    } // End IF
} // TCA_HC2F_FLT


//***************************************************************************
//  $TCA_HC2F_HC2F
//***************************************************************************

void TCA_HC2F_HC2F
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the HC2F as HC2F
    lpAllTypes->aplHC2F = lpaplHC2F[uInt];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2F_HC2F


//***************************************************************************
//  $TCA_HC2F_HC4F
//***************************************************************************

void TCA_HC2F_HC4F
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Zero the HC4F memory
    ZeroMemory (&lpAllTypes->aplHC4F, sizeof (lpAllTypes->aplHC4F));

    // Copy the HC2F as HC4F
    lpAllTypes->aplHC4F.partsLo[0] = lpaplHC2F[uInt];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2F_HC4F


//***************************************************************************
//  $TCA_HC2F_HC8F
//***************************************************************************

void TCA_HC2F_HC8F
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Zero the HC8F memory
    ZeroMemory (&lpAllTypes->aplHC8F, sizeof (lpAllTypes->aplHC8F));

    // Copy the HC2F as HC8F
    lpAllTypes->aplHC8F.partsLo[0].partsLo[0] = lpaplHC2F[uInt];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2F_HC8F


//***************************************************************************
//  $TCA_HC2F_RAT
//***************************************************************************

void TCA_HC2F_RAT
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Convert the real part of HC2F to a RAT
    mpq_init_set_d (&lpAllTypes->aplRat, lpaplHC2F[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2F parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC2F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC2F_RAT


//***************************************************************************
//  $TCA_HC2F_HC2R
//***************************************************************************

void TCA_HC2F_HC2R
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy HC2F to HC2R
        mpq_init_set_d (&lpAllTypes->aplHC2R.parts[i], lpaplHC2F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2F_HC2R


//***************************************************************************
//  $TCA_HC2F_HC4R
//***************************************************************************

void TCA_HC2F_HC4R
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0/1
    mphc4r_init (&lpAllTypes->aplHC4R);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy HC2F to HC4R
        mpq_set_d (&lpAllTypes->aplHC4R.parts[i], lpaplHC2F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2F_HC4R


//***************************************************************************
//  $TCA_HC2F_HC8R
//***************************************************************************

void TCA_HC2F_HC8R
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0/1
    mphc8r_init (&lpAllTypes->aplHC8R);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy HC2F to HC8R
        mpq_set_d (&lpAllTypes->aplHC8R.parts[i], lpaplHC2F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2F_HC8R


//***************************************************************************
//  $TCA_HC2F_VFP
//***************************************************************************

void TCA_HC2F_VFP
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to VFP
    mpfr_init_set_d (&lpAllTypes->aplVfp, lpaplHC2F[uInt].parts[0], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2F parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC2F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC2F_VFP


//***************************************************************************
//  $TCA_HC2F_HC2V
//***************************************************************************

void TCA_HC2F_HC2V
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2F parts to HC2V
        mpfr_init_set_d (&lpAllTypes->aplHC2V.parts[i], lpaplHC2F[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2F_HC2V


//***************************************************************************
//  $TCA_HC2F_HC4V
//***************************************************************************

void TCA_HC2F_HC4V
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2F parts to HC4V
        mpfr_set_d (&lpAllTypes->aplHC4V.parts[i], lpaplHC2F[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2F_HC4V


//***************************************************************************
//  $TCA_HC2F_HC8V
//***************************************************************************

void TCA_HC2F_HC8V
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2F parts to HC8V
        mpfr_set_d (&lpAllTypes->aplHC8V.parts[i], lpaplHC2F[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2F_HC8V


//***************************************************************************
//  $TCA_HC2R_BOOL
//***************************************************************************

void TCA_HC2R_BOOL
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to BOOL
    lpAllTypes->aplInteger = ConvertRatToInt (&lpaplHC2R[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Mark as successful or not
        lpbRet[0] &= IsBooleanValue (lpAllTypes->aplInteger);

        // Loop through the remainder of the imaginary HC2R parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC2R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC2R_BOOL


//***************************************************************************
//  $TCA_HC2R_INT
//***************************************************************************

void TCA_HC2R_INT
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the real part of HC2R to INT
    lpAllTypes->aplInteger = ConvertRatToInt (&lpaplHC2R[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Loop through the remainder of the imaginary HC2R parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC2R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC2R_INT


//***************************************************************************
//  $TCA_HC2R_HC2I
//***************************************************************************

void TCA_HC2R_HC2I
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert each part of HC2R to INT
        lpAllTypes->aplHC2I.parts[i] = ConvertRatToInt (&lpaplHC2R[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_HC2R_HC2I


//***************************************************************************
//  $TCA_HC2R_HC4I
//***************************************************************************

void TCA_HC2R_HC4I
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Zero the HC4I memory
    ZeroMemory (&lpAllTypes->aplHC4I, sizeof (lpAllTypes->aplHC4I));

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert each part of HC2R to INT
        lpAllTypes->aplHC4I.parts[i] = ConvertRatToInt (&lpaplHC2R[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_HC2R_HC4I


//***************************************************************************
//  $TCA_HC2R_HC8I
//***************************************************************************

void TCA_HC2R_HC8I
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Zero the HC8I memory
    ZeroMemory (&lpAllTypes->aplHC8I, sizeof (lpAllTypes->aplHC8I));

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert each part of HC2R to INT
        lpAllTypes->aplHC8I.parts[i] = ConvertRatToInt (&lpaplHC2R[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_HC2R_HC8I


//***************************************************************************
//  $TCA_HC2R_FLT
//***************************************************************************

void TCA_HC2R_FLT
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the real part of HC2R as FLT
    lpAllTypes->aplFloat = mpq_get_d (&lpaplHC2R[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2R parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC2R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC2R_FLT


//***************************************************************************
//  $TCA_HC2R_HC2F
//***************************************************************************

void TCA_HC2R_HC2F
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the parts of HC2R as HC2F
        lpAllTypes->aplHC2F.parts[i] = mpq_get_d (&lpaplHC2R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2R_HC2F


//***************************************************************************
//  $TCA_HC2R_HC4F
//***************************************************************************

void TCA_HC2R_HC4F
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Zero the HC4F memory
    ZeroMemory (&lpAllTypes->aplHC4F, sizeof (lpAllTypes->aplHC4F));

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the parts of HC2R as HC4F
        lpAllTypes->aplHC4F.parts[i] = mpq_get_d (&lpaplHC2R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2R_HC4F


//***************************************************************************
//  $TCA_HC2R_HC8F
//***************************************************************************

void TCA_HC2R_HC8F
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Zero the HC8F memory
    ZeroMemory (&lpAllTypes->aplHC8F, sizeof (lpAllTypes->aplHC8F));

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the parts of HC2R as HC8F
        lpAllTypes->aplHC8F.parts[i] = mpq_get_d (&lpaplHC2R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2R_HC8F


//***************************************************************************
//  $TCA_HC2R_RAT
//***************************************************************************

void TCA_HC2R_RAT
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the real part of HC2R to a RAT
    mpq_init_set (&lpAllTypes->aplRat, &lpaplHC2R[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2R parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC2R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC2R_RAT


//***************************************************************************
//  $TCA_HC2R_HC2R
//***************************************************************************

void TCA_HC2R_HC2R
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2R as HC2R
        mpq_init_set (&lpAllTypes->aplHC2R.parts[i], &lpaplHC2R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2R_HC2R


//***************************************************************************
//  $TCA_HC2R_HC4R
//***************************************************************************

void TCA_HC2R_HC4R
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0
    mphc4r_init (&lpAllTypes->aplHC4R);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2R as HC4R
        mpq_set (&lpAllTypes->aplHC4R.parts[i], &lpaplHC2R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2R_HC4R


//***************************************************************************
//  $TCA_HC2R_HC8R
//***************************************************************************

void TCA_HC2R_HC8R
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0
    mphc8r_init (&lpAllTypes->aplHC8R);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2R as HC8R
        mpq_set (&lpAllTypes->aplHC8R.parts[i], &lpaplHC2R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2R_HC8R


//***************************************************************************
//  $TCA_HC2R_VFP
//***************************************************************************

void TCA_HC2R_VFP
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the real part of HC2R as VFP
    mpfr_init_set_q  (&lpAllTypes->aplVfp, &lpaplHC2R[uInt].parts[0], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2R parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC2R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC2R_VFP


//***************************************************************************
//  $TCA_HC2R_HC2V
//***************************************************************************

void TCA_HC2R_HC2V
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the parts of HC2R as HC2V
        mpfr_init_set_q  (&lpAllTypes->aplHC2V.parts[i], &lpaplHC2R[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2R_HC2V


//***************************************************************************
//  $TCA_HC2R_HC4V
//***************************************************************************

void TCA_HC2R_HC4V
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the parts of HC2R as HC4V
        mpfr_set_q  (&lpAllTypes->aplHC4V.parts[i], &lpaplHC2R[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2R_HC4V


//***************************************************************************
//  $TCA_HC2R_HC8V
//***************************************************************************

void TCA_HC2R_HC8V
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the parts of HC2R as HC8V
        mpfr_set_q  (&lpAllTypes->aplHC8V.parts[i], &lpaplHC2R[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2R_HC8V


//***************************************************************************
//  $TCA_HC2V_BOOL
//***************************************************************************

void TCA_HC2V_BOOL
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the real part of HC2V to an integer using []CT as called for by lpAllTypes->enumCT
    lpAllTypes->aplInteger = ConvertVfpToInt (&lpaplHC2V[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Mark as successful or not
        lpbRet[0] &= IsBooleanValue (lpAllTypes->aplInteger);

        // Loop through the remainder of the imaginary HC2V parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC2V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC2V_BOOL


//***************************************************************************
//  $TCA_HC2V_INT
//***************************************************************************

void TCA_HC2V_INT
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the real part of HC2V to an integer using []CT as called for by lpAllTypes->enumCT
    lpAllTypes->aplInteger = ConvertVfpToInt (&lpaplHC2V[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Loop through the remainder of the imaginary HC2V parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC2V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC2V_INT


//***************************************************************************
//  $TCA_HC2V_HC2I
//***************************************************************************

void TCA_HC2V_HC2I
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert each part of HC2V to INT
        lpAllTypes->aplHC2I.parts[i] = ConvertVfpToInt (&lpaplHC2V[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_HC2V_HC2I


//***************************************************************************
//  $TCA_HC2V_HC4I
//***************************************************************************

void TCA_HC2V_HC4I
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Zero the HC4I memory
    ZeroMemory (&lpAllTypes->aplHC4I, sizeof (lpAllTypes->aplHC4I));

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert each part of HC2V to INT
        lpAllTypes->aplHC4I.parts[i] = ConvertVfpToInt (&lpaplHC2V[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_HC2V_HC4I


//***************************************************************************
//  $TCA_HC2V_HC8I
//***************************************************************************

void TCA_HC2V_HC8I
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Zero the HC8I memory
    ZeroMemory (&lpAllTypes->aplHC8I, sizeof (lpAllTypes->aplHC8I));

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert each part of HC2V to INT
        lpAllTypes->aplHC8I.parts[i] = ConvertVfpToInt (&lpaplHC2V[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_HC2V_HC8I


//***************************************************************************
//  $TCA_HC2V_FLT
//***************************************************************************

void TCA_HC2V_FLT
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the real part of HC2V as FLT
    lpAllTypes->aplFloat = mpfr_get_d (&lpaplHC2V[uInt].parts[0], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2V parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC2V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC2V_FLT


//***************************************************************************
//  $TCA_HC2V_HC2F
//***************************************************************************

void TCA_HC2V_HC2F
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the parts of HC2V as HC2F
        lpAllTypes->aplHC2F.parts[i] = mpfr_get_d (&lpaplHC2V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2V_HC2F


//***************************************************************************
//  $TCA_HC2V_HC4F
//***************************************************************************

void TCA_HC2V_HC4F
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Zero the HC4F memory
    ZeroMemory (&lpAllTypes->aplHC4F, sizeof (lpAllTypes->aplHC4F));

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the parts of HC2V as HC4F
        lpAllTypes->aplHC4F.parts[i] = mpfr_get_d (&lpaplHC2V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2V_HC4F


//***************************************************************************
//  $TCA_HC2V_HC8F
//***************************************************************************

void TCA_HC2V_HC8F
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Zero the HC8F memory
    ZeroMemory (&lpAllTypes->aplHC8F, sizeof (lpAllTypes->aplHC8F));

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the parts of HC2V as HC8F
        lpAllTypes->aplHC8F.parts[i] = mpfr_get_d (&lpaplHC2V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2V_HC8F


//***************************************************************************
//  $TCA_HC2V_RAT
//***************************************************************************

void TCA_HC2V_RAT
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the real part of HC2V to a RAT
    mpq_init_set_fr (&lpAllTypes->aplRat, &lpaplHC2V[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2V parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC2V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC2V_RAT


//***************************************************************************
//  $TCA_HC2V_HC2R
//***************************************************************************

void TCA_HC2V_HC2R
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2V as HC4R
        mpq_init_set_fr (&lpAllTypes->aplHC4R.parts[i], &lpaplHC2V[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2V_HC2R


//***************************************************************************
//  $TCA_HC2V_HC4R
//***************************************************************************

void TCA_HC2V_HC4R
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0/1
    mphc4r_init (&lpAllTypes->aplHC4R);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2V as HC4R
        mpq_set_fr (&lpAllTypes->aplHC4R.parts[i], &lpaplHC2V[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2V_HC4R


//***************************************************************************
//  $TCA_HC2V_HC8R
//***************************************************************************

void TCA_HC2V_HC8R
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0/1
    mphc8r_init (&lpAllTypes->aplHC8R);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2V as HC8R
        mpq_set_fr (&lpAllTypes->aplHC8R.parts[i], &lpaplHC2V[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2V_HC8R


//***************************************************************************
//  $TCA_HC2V_VFP
//***************************************************************************

void TCA_HC2V_VFP
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the real part of HC2V as VFP
    mpfr_init_set  (&lpAllTypes->aplVfp, &lpaplHC2V[uInt].parts[0], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2V parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC2V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC2V_VFP


//***************************************************************************
//  $TCA_HC2V_HC2V
//***************************************************************************

void TCA_HC2V_HC2V
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2V part to HC2V
        mpfr_init_set (&lpAllTypes->aplHC2V.parts[i], &lpaplHC2V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2V_HC2V


//***************************************************************************
//  $TCA_HC2V_HC4V
//***************************************************************************

void TCA_HC2V_HC4V
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0
    mphc4v_init0 (&lpAllTypes->aplHC4V);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2V part to HC4V
        mpfr_set (&lpAllTypes->aplHC4V.parts[i], &lpaplHC2V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2V_HC4V


//***************************************************************************
//  $TCA_HC2V_HC8V
//***************************************************************************

void TCA_HC2V_HC8V
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC2V part to HC8V
        mpfr_set (&lpAllTypes->aplHC8V.parts[0], &lpaplHC2V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2V_HC8V


//***************************************************************************
//  $TCA_HC4I_BOOL
//***************************************************************************

void TCA_HC4I_BOOL
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to BOOL
    lpAllTypes->aplInteger = lpaplHC4I[uInt].parts[0];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful or not
        lpbRet[0] = IsBooleanValue (lpAllTypes->aplInteger);

        // Loop through the remainder of the imaginary HC4I parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4I part is 0
            lpbRet[0] &= lpaplHC4I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC4I_BOOL


//***************************************************************************
//  $TCA_HC4I_INT
//***************************************************************************

void TCA_HC4I_INT
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to INT
    lpAllTypes->aplInteger = lpaplHC4I[uInt].parts[0];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4I parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4I part is 0
            lpbRet[0] &= lpaplHC4I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC4I_INT


//***************************************************************************
//  $TCA_HC4I_HC2I
//***************************************************************************

void TCA_HC4I_HC2I
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2;i++)      // No bRet
        // Copy the HC4I as HC2I
        lpAllTypes->aplHC2I.parts[i] = lpaplHC4I[uInt].parts[i];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4I parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4I part is 0
            lpbRet[0] &= lpaplHC4I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC4I_HC2I


//***************************************************************************
//  $TCA_HC4I_HC4I
//***************************************************************************

void TCA_HC4I_HC4I
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the HC4I as HC4I
    lpAllTypes->aplHC4I = lpaplHC4I[uInt];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4I_HC4I


//***************************************************************************
//  $TCA_HC4I_HC8I
//***************************************************************************

void TCA_HC4I_HC8I
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Zero the HC8I memory
    ZeroMemory (&lpAllTypes->aplHC8I, sizeof (lpAllTypes->aplHC8I));

    // Copy the HC4I as HC8I
    lpAllTypes->aplHC8I.partsLo[0] = lpaplHC4I[uInt];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4I_HC8I


//***************************************************************************
//  $TCA_HC4I_FLT
//***************************************************************************

void TCA_HC4I_FLT
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to FLT
    lpAllTypes->aplFloat = (APLFLOAT) lpaplHC4I[uInt].parts[0];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4I parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4I part is 0
            lpbRet[0] &= lpaplHC4I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC4I_FLT


//***************************************************************************
//  $TCA_HC4I_HC2F
//***************************************************************************

void TCA_HC4I_HC2F
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC4I as HC2F
        lpAllTypes->aplHC2F.parts[i] = (APLFLOAT) lpaplHC4I[uInt].parts[i];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the HC4I parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4I part is zero
            lpbRet[0] &= lpaplHC4I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC4I_HC2F


//***************************************************************************
//  $TCA_HC4I_HC4F
//***************************************************************************

void TCA_HC4I_HC4F
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC4I as HC4F
        lpAllTypes->aplHC4F.parts[i] = (APLFLOAT) lpaplHC4I[uInt].parts[i];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4I_HC4F


//***************************************************************************
//  $TCA_HC4I_HC8F
//***************************************************************************

void TCA_HC4I_HC8F
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Zero the HC8F memory
    ZeroMemory (&lpAllTypes->aplHC8F, sizeof (lpAllTypes->aplHC8F));

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC4I as HC8F
        lpAllTypes->aplHC8F.parts[i] = (APLFLOAT) lpaplHC4I[uInt].parts[i];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4I_HC8F


//***************************************************************************
//  $TCA_HC4I_RAT
//***************************************************************************

void TCA_HC4I_RAT
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Convert the real part of HC4I to a RAT
    mpq_init_set_sx (&lpAllTypes->aplRat, lpaplHC4I[uInt].parts[0], 1);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4I parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4I part is 0
            lpbRet[0] &= lpaplHC4I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC4I_RAT


//***************************************************************************
//  $TCA_HC4I_HC2R
//***************************************************************************

void TCA_HC4I_HC2R
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Set the parts to HC2R
        mpq_init_set_sx (&lpAllTypes->aplHC2R.parts[i], lpaplHC4I[uInt].parts[i], 1);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4I parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4I part is 0
            lpbRet[0] &= lpaplHC4I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC4I_HC2R


//***************************************************************************
//  $TCA_HC4I_HC4R
//***************************************************************************

void TCA_HC4I_HC4R
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC4I parts to HC4R
        mpq_init_set_sx (&lpAllTypes->aplHC4R.parts[i], lpaplHC4I[uInt].parts[i], 1);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4I_HC4R


//***************************************************************************
//  $TCA_HC4I_HC8R
//***************************************************************************

void TCA_HC4I_HC8R
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0/1
    mphc8r_init (&lpAllTypes->aplHC8R);

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC4I parts to HC8R
        mpq_set_sx (&lpAllTypes->aplHC8R.parts[i], lpaplHC4I[uInt].parts[i], 1);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4I_HC8R


//***************************************************************************
//  $TCA_HC4I_VFP
//***************************************************************************

void TCA_HC4I_VFP
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to VFP
    mpfr_init_set_sx (&lpAllTypes->aplVfp, lpaplHC4I[uInt].parts[0], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4I parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4I part is 0
            lpbRet[0] &= lpaplHC4I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC4I_VFP


//***************************************************************************
//  $TCA_HC4I_HC2V
//***************************************************************************

void TCA_HC4I_HC2V
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC4I parts to HC2V
        mpfr_init_set_sx (&lpAllTypes->aplHC2V.parts[i], lpaplHC4I[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4I parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4I part is 0
            lpbRet[0] &= lpaplHC4I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC4I_HC2V


//***************************************************************************
//  $TCA_HC4I_HC4V
//***************************************************************************

void TCA_HC4I_HC4V
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC4I parts to HC4V
        mpfr_init_set_sx (&lpAllTypes->aplHC4V.parts[i], lpaplHC4I[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4I_HC4V


//***************************************************************************
//  $TCA_HC4I_HC8V
//***************************************************************************

void TCA_HC4I_HC8V
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC4I parts to HC8V
        mpfr_set_sx (&lpAllTypes->aplHC8V.parts[i], lpaplHC4I[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4I_HC8V


//***************************************************************************
//  $TCA_HC4F_BOOL
//***************************************************************************

void TCA_HC4F_BOOL
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the HC4F real part to an integer using System CT
    lpAllTypes->aplInteger = ConvertFltToInt (&lpaplHC4F[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Mark as successful or not
        lpbRet[0] &= IsBooleanValue (lpAllTypes->aplInteger);

        // Loop through the remainder of the imaginary HC4F parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4F part is a near 0
            lpbRet[0] &= (ConvertFltToInt (&lpaplHC4F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0);
    } // End IF
} // TCA_HC4F_BOOL


//***************************************************************************
//  $TCA_HC4F_INT
//***************************************************************************

void TCA_HC4F_INT
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the HC4F real part to an integer using System CT
    lpAllTypes->aplInteger = ConvertFltToInt (&lpaplHC4F[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Loop through the remainder of the imaginary HC4F parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4F part is a near 0
            lpbRet[0] &= (ConvertFltToInt (&lpaplHC4F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0);
    } // End IF
} // TCA_HC4F_INT


//***************************************************************************
//  $TCA_HC4F_HC2I
//***************************************************************************

void TCA_HC4F_HC2I
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert the HC4F to HC2I
        lpAllTypes->aplHC2I.parts[i] = ConvertFltToInt (&lpaplHC4F[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
        // Loop through the remainder of the imaginary HC4F parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC4F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4F_HC2I


//***************************************************************************
//  $TCA_HC4F_HC4I
//***************************************************************************

void TCA_HC4F_HC4I
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[4];

////// Zero the HC4I memory
////ZeroMemory (&lpAllTypes->aplHC4I, sizeof (lpAllTypes->aplHC4I));

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Attempt to convert the HC4F to HC4I using System CT
        lpAllTypes->aplHC4I.parts[i] = ConvertFltToInt (&lpaplHC4F[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1]
                 && bRet[2]
                 && bRet[3];
} // TCA_HC4F_HC4I


//***************************************************************************
//  $TCA_HC4F_HC8I
//***************************************************************************

void TCA_HC4F_HC8I
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[4];

    // Zero the HC8I memory
    ZeroMemory (&lpAllTypes->aplHC8I, sizeof (lpAllTypes->aplHC8I));

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Attempt to convert the HC4F to HC8I using System CT
        lpAllTypes->aplHC8I.parts[i] = ConvertFltToInt (&lpaplHC4F[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1]
                 && bRet[2]
                 && bRet[3];
} // TCA_HC4F_HC8I


//***************************************************************************
//  $TCA_HC4F_FLT
//***************************************************************************

void TCA_HC4F_FLT
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to FLT
    lpAllTypes->aplFloat = lpaplHC4F[uInt].parts[0];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4F parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC4F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4F_FLT


//***************************************************************************
//  $TCA_HC4F_HC2F
//***************************************************************************

void TCA_HC4F_HC2F
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the HC4F as HC2F
    lpAllTypes->aplHC2F = lpaplHC4F[uInt].partsLo[0];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4F parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC4F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4F_HC2F


//***************************************************************************
//  $TCA_HC4F_HC4F
//***************************************************************************

void TCA_HC4F_HC4F
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the HC4F as HC4F
    lpAllTypes->aplHC4F = lpaplHC4F[uInt];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4F_HC4F


//***************************************************************************
//  $TCA_HC4F_HC8F
//***************************************************************************

void TCA_HC4F_HC8F
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Zero the memory
    ZeroMemory (&lpAllTypes->aplHC8F, sizeof (lpAllTypes->aplHC8F));

    // Copy the HC4F as HC8F
    lpAllTypes->aplHC8F.partsLo[0] = lpaplHC4F[uInt];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4F_HC8F


//***************************************************************************
//  $TCA_HC4F_RAT
//***************************************************************************

void TCA_HC4F_RAT
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Convert the real part of HC4F to a RAT
    mpq_init_set_d (&lpAllTypes->aplRat, lpaplHC4F[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4F parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC4F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4F_RAT


//***************************************************************************
//  $TCA_HC4F_HC2R
//***************************************************************************

void TCA_HC4F_HC2R
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy HC4F to HC2R
        mpq_init_set_d (&lpAllTypes->aplHC2R.parts[i], lpaplHC4F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4F parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC4F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4F_HC2R


//***************************************************************************
//  $TCA_HC4F_HC4R
//***************************************************************************

void TCA_HC4F_HC4R
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy HC4F to HC4R
        mpq_init_set_d (&lpAllTypes->aplHC4R.parts[i], lpaplHC4F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4F_HC4R


//***************************************************************************
//  $TCA_HC4F_HC8R
//***************************************************************************

void TCA_HC4F_HC8R
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0/1
    mphc8r_init (&lpAllTypes->aplHC8R);

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy HC4F to HC8R
        mpq_set_d (&lpAllTypes->aplHC8R.parts[i], lpaplHC4F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4F_HC8R


//***************************************************************************
//  $TCA_HC4F_VFP
//***************************************************************************

void TCA_HC4F_VFP
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to VFP
    mpfr_init_set_d (&lpAllTypes->aplVfp, lpaplHC4F[uInt].parts[0], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4F parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC4F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4F_VFP


//***************************************************************************
//  $TCA_HC4F_HC2V
//***************************************************************************

void TCA_HC4F_HC2V
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC4F parts to HC2V
        mpfr_init_set_d (&lpAllTypes->aplHC2V.parts[i], lpaplHC4F[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4F parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC4F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4F_HC2V


//***************************************************************************
//  $TCA_HC4F_HC4V
//***************************************************************************

void TCA_HC4F_HC4V
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC4F parts to HC4V
        mpfr_init_set_d (&lpAllTypes->aplHC4V.parts[i], lpaplHC4F[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4F_HC4V


//***************************************************************************
//  $TCA_HC4F_HC8V
//***************************************************************************

void TCA_HC4F_HC8V
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC4F parts to HC8V
        mpfr_set_d (&lpAllTypes->aplHC8V.parts[i], lpaplHC4F[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4F_HC8V


//***************************************************************************
//  $TCA_HC4R_BOOL
//***************************************************************************

void TCA_HC4R_BOOL
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to BOOL
    lpAllTypes->aplInteger = ConvertRatToInt (&lpaplHC4R[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Mark as successful or not
        lpbRet[0] &= IsBooleanValue (lpAllTypes->aplInteger);

        // Loop through the remainder of the imaginary HC4R parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC4R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4R_BOOL


//***************************************************************************
//  $TCA_HC4R_INT
//***************************************************************************

void TCA_HC4R_INT
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the real part of HC4R to INT
    lpAllTypes->aplInteger = ConvertRatToInt (&lpaplHC4R[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Loop through the remainder of the imaginary HC4R parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC4R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4R_INT


//***************************************************************************
//  $TCA_HC4R_HC2I
//***************************************************************************

void TCA_HC4R_HC2I
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert each part of HC4R to INT
        lpAllTypes->aplHC2I.parts[i] = ConvertRatToInt (&lpaplHC4R[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
        // Loop through the remainder of the imaginary HC4R parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC4R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4R_HC2I


//***************************************************************************
//  $TCA_HC4R_HC4I
//***************************************************************************

void TCA_HC4R_HC4I
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[4];

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Attempt to convert each part of HC4R to INT
        lpAllTypes->aplHC4I.parts[i] = ConvertRatToInt (&lpaplHC4R[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1]
                 && bRet[2]
                 && bRet[3];
} // TCA_HC4R_HC4I


//***************************************************************************
//  $TCA_HC4R_HC8I
//***************************************************************************

void TCA_HC4R_HC8I
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[4];

    // Zero the HC8I memory
    ZeroMemory (&lpAllTypes->aplHC8I, sizeof (lpAllTypes->aplHC8I));

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Attempt to convert each part of HC4R to INT
        lpAllTypes->aplHC8I.parts[i] = ConvertRatToInt (&lpaplHC4R[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1]
                 && bRet[2]
                 && bRet[3];
} // TCA_HC4R_HC8I


//***************************************************************************
//  $TCA_HC4R_FLT
//***************************************************************************

void TCA_HC4R_FLT
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the real part of HC4R as FLT
    lpAllTypes->aplFloat = mpq_get_d (&lpaplHC4R[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4R parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC4R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4R_FLT


//***************************************************************************
//  $TCA_HC4R_HC2F
//***************************************************************************

void TCA_HC4R_HC2F
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the parts of HC4R as HC2F
        lpAllTypes->aplHC2F.parts[i] = mpq_get_d (&lpaplHC4R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4R parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC4R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4R_HC2F


//***************************************************************************
//  $TCA_HC4R_HC4F
//***************************************************************************

void TCA_HC4R_HC4F
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the parts of HC4R as HC4F
        lpAllTypes->aplHC4F.parts[i] = mpq_get_d (&lpaplHC4R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4R_HC4F


//***************************************************************************
//  $TCA_HC4R_HC8F
//***************************************************************************

void TCA_HC4R_HC8F
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Zero the HC8F memory
    ZeroMemory (&lpAllTypes->aplHC8F, sizeof (lpAllTypes->aplHC8F));

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the parts of HC4R as HC8F
        lpAllTypes->aplHC8F.parts[i] = mpq_get_d (&lpaplHC4R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4R_HC8F


//***************************************************************************
//  $TCA_HC4R_RAT
//***************************************************************************

void TCA_HC4R_RAT
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Convert the real part of HC4R to a RAT
    mpq_init_set (&lpAllTypes->aplRat, &lpaplHC4R[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4R parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC4R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4R_RAT


//***************************************************************************
//  $TCA_HC4R_HC2R
//***************************************************************************

void TCA_HC4R_HC2R
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC4R as HC2R
        mpq_init_set (&lpAllTypes->aplHC2R.parts[i], &lpaplHC4R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4R parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC4R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4R_HC2R


//***************************************************************************
//  $TCA_HC4R_HC4R
//***************************************************************************

void TCA_HC4R_HC4R
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC4R as HC4R
        mpq_init_set (&lpAllTypes->aplHC4R.parts[i], &lpaplHC4R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4R_HC4R


//***************************************************************************
//  $TCA_HC4R_HC8R
//***************************************************************************

void TCA_HC4R_HC8R
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0/1
    mphc8r_init (&lpAllTypes->aplHC8R);

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC4R as HC8R
        mpq_set (&lpAllTypes->aplHC8R.parts[i], &lpaplHC4R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2R_HC8R


//***************************************************************************
//  $TCA_HC4R_VFP
//***************************************************************************

void TCA_HC4R_VFP
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the real part of HC4R as VFP
    mpfr_init_set_q  (&lpAllTypes->aplVfp, &lpaplHC4R[uInt].parts[0], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4R parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC4R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4R_VFP


//***************************************************************************
//  $TCA_HC4R_HC2V
//***************************************************************************

void TCA_HC4R_HC2V
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the parts of HC4R as HC2V
        mpfr_init_set_q  (&lpAllTypes->aplHC2V.parts[i], &lpaplHC4R[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4R parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC4R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4R_HC2V


//***************************************************************************
//  $TCA_HC4R_HC4V
//***************************************************************************

void TCA_HC4R_HC4V
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the parts of HC4R as HC4V
        mpfr_init_set_q  (&lpAllTypes->aplHC4V.parts[i], &lpaplHC4R[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4R_HC4V


//***************************************************************************
//  $TCA_HC4R_HC8V
//***************************************************************************

void TCA_HC4R_HC8V
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the parts of HC4R as HC8V
        mpfr_set_q  (&lpAllTypes->aplHC8V.parts[i], &lpaplHC4R[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4R_HC8V


//***************************************************************************
//  $TCA_HC4V_BOOL
//***************************************************************************

void TCA_HC4V_BOOL
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part of HC4V to BOOL
    lpAllTypes->aplInteger = ConvertVfpToInt (&lpaplHC4V[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Mark as successful or not
        lpbRet[0] &= IsBooleanValue (lpAllTypes->aplInteger);

        // Loop through the remainder of the imaginary HC4V parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC4V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4V_BOOL


//***************************************************************************
//  $TCA_HC4V_INT
//***************************************************************************

void TCA_HC4V_INT
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the real part of HC4V to INT
    lpAllTypes->aplInteger = ConvertVfpToInt (&lpaplHC4V[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Loop through the remainder of the imaginary HC4V parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC4V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4V_INT


//***************************************************************************
//  $TCA_HC4V_HC2I
//***************************************************************************

void TCA_HC4V_HC2I
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert each part of HC4V to INT
        lpAllTypes->aplHC2I.parts[i] = ConvertVfpToInt (&lpaplHC4V[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
        // Loop through the remainder of the imaginary HC4V parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC4V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4V_HC2I


//***************************************************************************
//  $TCA_HC4V_HC4I
//***************************************************************************

void TCA_HC4V_HC4I
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[4];

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Attempt to convert each part of HC4V to INT
        lpAllTypes->aplHC4I.parts[i] = ConvertVfpToInt (&lpaplHC4V[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1]
                 && bRet[2]
                 && bRet[3];
} // TCA_HC4V_HC4I


//***************************************************************************
//  $TCA_HC4V_HC8I
//***************************************************************************

void TCA_HC4V_HC8I
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[4];

    // Zero the HC8I memory
    ZeroMemory (&lpAllTypes->aplHC8I, sizeof (lpAllTypes->aplHC8I));

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Attempt to convert each part of HC4V to INT
        lpAllTypes->aplHC8I.parts[i] = ConvertVfpToInt (&lpaplHC4V[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1]
                 && bRet[2]
                 && bRet[3];
} // TCA_HC4V_HC8I


//***************************************************************************
//  $TCA_HC4V_FLT
//***************************************************************************

void TCA_HC4V_FLT
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the real part of HC4V as FLT
    lpAllTypes->aplFloat = mpfr_get_d (&lpaplHC4V[uInt].parts[0], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4V parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC4V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4V_FLT


//***************************************************************************
//  $TCA_HC4V_HC2F
//***************************************************************************

void TCA_HC4V_HC2F
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the parts of HC4V as HC2F
        lpAllTypes->aplHC2F.parts[i] = mpfr_get_d (&lpaplHC4V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4V parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC4V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4V_HC2F


//***************************************************************************
//  $TCA_HC4V_HC4F
//***************************************************************************

void TCA_HC4V_HC4F
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the parts of HC4V as HC4F
        lpAllTypes->aplHC4F.parts[i] = mpfr_get_d (&lpaplHC4V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4V_HC4F


//***************************************************************************
//  $TCA_HC4V_HC8F
//***************************************************************************

void TCA_HC4V_HC8F
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Zero the HC8F memory
    ZeroMemory (&lpAllTypes->aplHC8F, sizeof (lpAllTypes->aplHC8F));

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the parts of HC4V as HC8F
        lpAllTypes->aplHC8F.parts[i] = mpfr_get_d (&lpaplHC4V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4V_HC8F


//***************************************************************************
//  $TCA_HC4V_RAT
//***************************************************************************

void TCA_HC4V_RAT
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the real part of HC4V to a RAT
    mpq_init_set_fr (&lpAllTypes->aplRat, &lpaplHC4V[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4V parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC4V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4V_RAT


//***************************************************************************
//  $TCA_HC4V_HC2R
//***************************************************************************

void TCA_HC4V_HC2R
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC4V as HC4R
        mpq_init_set_fr (&lpAllTypes->aplHC4R.parts[i], &lpaplHC4V[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4V parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC4V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4V_HC2R


//***************************************************************************
//  $TCA_HC4V_HC4R
//***************************************************************************

void TCA_HC4V_HC4R
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC4V as HC4R
        mpq_init_set_fr (&lpAllTypes->aplHC4R.parts[i], &lpaplHC4V[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4V_HC4R


//***************************************************************************
//  $TCA_HC4V_HC8R
//***************************************************************************

void TCA_HC4V_HC8R
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0/1
    mphc8r_init (&lpAllTypes->aplHC8R);

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC4V as HC8R
        mpq_set_fr (&lpAllTypes->aplHC8R.parts[i], &lpaplHC4V[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4V_HC8R


//***************************************************************************
//  $TCA_HC4V_VFP
//***************************************************************************

void TCA_HC4V_VFP
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the real part of HC4V as VFP
    mpfr_init_set  (&lpAllTypes->aplVfp, &lpaplHC4V[uInt].parts[0], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4V parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC4V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4V_VFP


//***************************************************************************
//  $TCA_HC4V_HC2V
//***************************************************************************

void TCA_HC4V_HC2V
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC4V part to HC2V
        mpfr_init_set (&lpAllTypes->aplHC2V.parts[i], &lpaplHC4V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4V parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC4V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4V_HC2V


//***************************************************************************
//  $TCA_HC4V_HC4V
//***************************************************************************

void TCA_HC4V_HC4V
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC4V part to HC4V
        mpfr_init_set (&lpAllTypes->aplHC4V.parts[i], &lpaplHC4V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4V_HC4V


//***************************************************************************
//  $TCA_HC4V_HC8V
//***************************************************************************

void TCA_HC4V_HC8V
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize to 0
    mphc8v_init0 (&lpAllTypes->aplHC8V);

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC4V part to HC8V
        mpfr_set (&lpAllTypes->aplHC8V.parts[0], &lpaplHC4V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4V_HC8V


//***************************************************************************
//  $TCA_HC8I_BOOL
//***************************************************************************

void TCA_HC8I_BOOL
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to BOOL
    lpAllTypes->aplInteger = lpaplHC8I[uInt].parts[0];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful or not
        lpbRet[0] = IsBooleanValue (lpAllTypes->aplInteger);

        // Loop through the remainder of the imaginary HC8I parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8I part is 0
            lpbRet[0] &= lpaplHC8I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC8I_BOOL


//***************************************************************************
//  $TCA_HC8I_INT
//***************************************************************************

void TCA_HC8I_INT
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to INT
    lpAllTypes->aplInteger = lpaplHC8I[uInt].parts[0];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8I parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8I part is 0
            lpbRet[0] &= lpaplHC8I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC8I_INT


//***************************************************************************
//  $TCA_HC8I_HC2I
//***************************************************************************

void TCA_HC8I_HC2I
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2;i++)      // No bRet
        // Copy the HC8I as HC2I
        lpAllTypes->aplHC2I.parts[i] = lpaplHC8I[uInt].parts[i];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8I parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8I part is 0
            lpbRet[0] &= lpaplHC8I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC8I_HC2I


//***************************************************************************
//  $TCA_HC8I_HC4I
//***************************************************************************

void TCA_HC8I_HC4I
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC8I as HC4I
        lpAllTypes->aplHC4I.parts[i] = lpaplHC8I[uInt].parts[i];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8I parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8I part is 0
            lpbRet[0] &= lpaplHC8I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC4I_HC4I


//***************************************************************************
//  $TCA_HC8I_HC8I
//***************************************************************************

void TCA_HC8I_HC8I
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the HC8I as HC8I
    lpAllTypes->aplHC8I = lpaplHC8I[uInt];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC8I_HC8I


//***************************************************************************
//  $TCA_HC8I_FLT
//***************************************************************************

void TCA_HC8I_FLT
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to FLT
    lpAllTypes->aplFloat = (APLFLOAT) lpaplHC8I[uInt].parts[0];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8I parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8I part is 0
            lpbRet[0] &= lpaplHC8I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC8I_FLT


//***************************************************************************
//  $TCA_HC8I_HC2F
//***************************************************************************

void TCA_HC8I_HC2F
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC8I as HC2F
        lpAllTypes->aplHC2F.parts[i] = (APLFLOAT) lpaplHC8I[uInt].parts[i];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the HC8I parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8I part is zero
            lpbRet[0] &= lpaplHC8I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC8I_HC2F


//***************************************************************************
//  $TCA_HC8I_HC4F
//***************************************************************************

void TCA_HC8I_HC4F
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC8I as HC4F
        lpAllTypes->aplHC4F.parts[i] = (APLFLOAT) lpaplHC8I[uInt].parts[i];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC8I_HC4F


//***************************************************************************
//  $TCA_HC8I_HC8F
//***************************************************************************

void TCA_HC8I_HC8F
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Zero the HC8F memory
    ZeroMemory (&lpAllTypes->aplHC8F, sizeof (lpAllTypes->aplHC8F));

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Copy the HC8I as HC8F
        lpAllTypes->aplHC8F.parts[i] = (APLFLOAT) lpaplHC8I[uInt].parts[i];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC8I_HC8F


//***************************************************************************
//  $TCA_HC8I_RAT
//***************************************************************************

void TCA_HC8I_RAT
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Convert the real part of HC8I to a RAT
    mpq_init_set_sx (&lpAllTypes->aplRat, lpaplHC8I[uInt].parts[0], 1);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8I parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8I part is 0
            lpbRet[0] &= lpaplHC8I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC8I_RAT


//***************************************************************************
//  $TCA_HC8I_HC2R
//***************************************************************************

void TCA_HC8I_HC2R
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Set the parts to HC2R
        mpq_init_set_sx (&lpAllTypes->aplHC2R.parts[i], lpaplHC8I[uInt].parts[i], 1);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8I parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8I part is 0
            lpbRet[0] &= lpaplHC8I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC8I_HC2R


//***************************************************************************
//  $TCA_HC8I_HC4R
//***************************************************************************

void TCA_HC8I_HC4R
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC8I parts to HC4R
        mpq_init_set_sx (&lpAllTypes->aplHC4R.parts[i], lpaplHC8I[uInt].parts[i], 1);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8I parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8I part is 0
            lpbRet[0] &= lpaplHC8I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC8I_HC4R


//***************************************************************************
//  $TCA_HC8I_HC8R
//***************************************************************************

void TCA_HC8I_HC8R
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Copy the HC8I parts to HC8R
        mpq_init_set_sx (&lpAllTypes->aplHC8R.parts[i], lpaplHC8I[uInt].parts[i], 1);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC8I_HC8R


//***************************************************************************
//  $TCA_HC8I_VFP
//***************************************************************************

void TCA_HC8I_VFP
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part to VFP
    mpfr_init_set_sx (&lpAllTypes->aplVfp, lpaplHC8I[uInt].parts[0], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8I parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8I part is 0
            lpbRet[0] &= lpaplHC8I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC8I_VFP


//***************************************************************************
//  $TCA_HC8I_HC2V
//***************************************************************************

void TCA_HC8I_HC2V
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC8I parts to HC2V
        mpfr_init_set_sx (&lpAllTypes->aplHC2V.parts[i], lpaplHC8I[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8I parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8I part is 0
            lpbRet[0] &= lpaplHC8I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC8I_HC2V


//***************************************************************************
//  $TCA_HC8I_HC4V
//***************************************************************************

void TCA_HC8I_HC4V
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC8I parts to HC4V
        mpfr_init_set_sx (&lpAllTypes->aplHC4V.parts[i], lpaplHC8I[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8I parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8I part is 0
            lpbRet[0] &= lpaplHC8I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC8I_HC4V


//***************************************************************************
//  $TCA_HC8I_HC8V
//***************************************************************************

void TCA_HC8I_HC8V
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Copy the HC8I parts to HC8V
        mpfr_init_set_sx (&lpAllTypes->aplHC8V.parts[i], lpaplHC8I[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC8I_HC8V


//***************************************************************************
//  $TCA_HC8F_BOOL
//***************************************************************************

void TCA_HC8F_BOOL
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the FLT to an integer using System CT
    lpAllTypes->aplInteger = ConvertFltToInt (&lpaplHC8F[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Mark as successful or not
        lpbRet[0] &= IsBooleanValue (lpAllTypes->aplInteger);

        // Loop through the remainder of the imaginary HC8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8F_BOOL


//***************************************************************************
//  $TCA_HC8F_INT
//***************************************************************************

void TCA_HC8F_INT
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the HC8F real part to an integer using System CT
    lpAllTypes->aplInteger = ConvertFltToInt (&lpaplHC8F[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Loop through the remainder of the imaginary HC8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8F part is a near 0
            lpbRet[0] &= (ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0);
    } // End IF
} // TCA_HC8F_INT


//***************************************************************************
//  $TCA_HC8F_HC2I
//***************************************************************************

void TCA_HC8F_HC2I
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert the HC8F to HC2I using System CT
        lpAllTypes->aplHC2I.parts[i] = ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
        // Loop through the remainder of the imaginary HC8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8F_HC2I


//***************************************************************************
//  $TCA_HC8F_HC4I
//***************************************************************************

void TCA_HC8F_HC4I
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[4];

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Attempt to convert the HC8F to HC4I using System CT
        lpAllTypes->aplHC4I.parts[i] = ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1]
                 && bRet[2]
                 && bRet[3];
        // Loop through the remainder of the imaginary HC8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8F_HC4I


//***************************************************************************
//  $TCA_HC8F_HC8I
//***************************************************************************

void TCA_HC8F_HC8I
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[8];

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Attempt to convert the HC8F to HC4I using System CT
        lpAllTypes->aplHC8I.parts[i] = ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1]
                 && bRet[2]
                 && bRet[3]
                 && bRet[4]
                 && bRet[5]
                 && bRet[6]
                 && bRet[7];
} // TCA_HC8F_HC8I


//***************************************************************************
//  $TCA_HC8F_FLT
//***************************************************************************

void TCA_HC8F_FLT
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to FLT
    lpAllTypes->aplFloat = lpaplHC8F[uInt].parts[0];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8F_FLT


//***************************************************************************
//  $TCA_HC8F_HC2F
//***************************************************************************

void TCA_HC8F_HC2F
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        // Copy the HC8F as HC2F
        lpAllTypes->aplHC2F.parts[i] = lpaplHC8F[uInt].parts[i];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8F_HC2F


//***************************************************************************
//  $TCA_HC8F_HC4F
//***************************************************************************

void TCA_HC8F_HC4F
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)
        // Copy the HC8F as HC4F
        lpAllTypes->aplHC4F.parts[i] = lpaplHC8F[uInt].parts[i];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8F_HC4F


//***************************************************************************
//  $TCA_HC8F_HC8F
//***************************************************************************

void TCA_HC8F_HC8F
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the HC8F as HC8F
    lpAllTypes->aplHC8F = lpaplHC8F[uInt];

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC8F_HC8F


//***************************************************************************
//  $TCA_HC8F_RAT
//***************************************************************************

void TCA_HC8F_RAT
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Convert the real part of HC8F to a RAT
    mpq_init_set_d (&lpAllTypes->aplRat, lpaplHC8F[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8F_RAT


//***************************************************************************
//  $TCA_HC8F_HC2R
//***************************************************************************

void TCA_HC8F_HC2R
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy HC8F to HC2R
        mpq_init_set_d (&lpAllTypes->aplHC2R.parts[i], lpaplHC8F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8F_HC2R


//***************************************************************************
//  $TCA_HC8F_HC4R
//***************************************************************************

void TCA_HC8F_HC4R
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy HC8F to HC4R
        mpq_init_set_d (&lpAllTypes->aplHC4R.parts[i], lpaplHC8F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HX8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8F_HC4R


//***************************************************************************
//  $TCA_HC8F_HC8R
//***************************************************************************

void TCA_HC8F_HC8R
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Copy HC8F to HC8R
        mpq_init_set_d (&lpAllTypes->aplHC8R.parts[i], lpaplHC8F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC8F_HC8R


//***************************************************************************
//  $TCA_HC8F_VFP
//***************************************************************************

void TCA_HC8F_VFP
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to VFP
    mpfr_init_set_d (&lpAllTypes->aplVfp, lpaplHC8F[uInt].parts[0], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8F_VFP


//***************************************************************************
//  $TCA_HC8F_HC2V
//***************************************************************************

void TCA_HC8F_HC2V
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC8F parts to HC2V
        mpfr_init_set_d (&lpAllTypes->aplHC2V.parts[i], lpaplHC8F[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8F_HC2V


//***************************************************************************
//  $TCA_HC8F_HC4V
//***************************************************************************

void TCA_HC8F_HC4V
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC8F parts to HC4V
        mpfr_init_set_d (&lpAllTypes->aplHC4V.parts[i], lpaplHC8F[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8F_HC4V


//***************************************************************************
//  $TCA_HC8F_HC8V
//***************************************************************************

void TCA_HC8F_HC8V
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Copy the HC8F parts to HC8V
        mpfr_init_set_d (&lpAllTypes->aplHC8V.parts[i], lpaplHC8F[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC8F_HC8V


//***************************************************************************
//  $TCA_HC8R_BOOL
//***************************************************************************

void TCA_HC8R_BOOL
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to BOOL
    lpAllTypes->aplInteger = ConvertRatToInt (&lpaplHC8R[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Mark as successful or not
        lpbRet[0] &= IsBooleanValue (lpAllTypes->aplInteger);

        // Loop through the remainder of the imaginary HC8R parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC8R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8R_BOOL


//***************************************************************************
//  $TCA_HC8R_INT
//***************************************************************************

void TCA_HC8R_INT
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the real part of HC8R to INT
    lpAllTypes->aplInteger = ConvertRatToInt (&lpaplHC8R[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Loop through the remainder of the imaginary HC8R parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC8R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8R_INT


//***************************************************************************
//  $TCA_HC8R_HC2I
//***************************************************************************

void TCA_HC8R_HC2I
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        // Attempt to convert each part of HC8R to INT
        lpAllTypes->aplHC2I.parts[i] = ConvertRatToInt (&lpaplHC8R[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
        // Loop through the remainder of the imaginary HC8R parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC8R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8R_HC2I


//***************************************************************************
//  $TCA_HC8R_HC4I
//***************************************************************************

void TCA_HC8R_HC4I
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[4];

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Attempt to convert each part of HC8R to INT
        lpAllTypes->aplHC4I.parts[i] = ConvertRatToInt (&lpaplHC8R[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1]
                 && bRet[2]
                 && bRet[3];
        // Loop through the remainder of the imaginary HC8R parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC8R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8R_HC4I


//***************************************************************************
//  $TCA_HC8R_HC8I
//***************************************************************************

void TCA_HC8R_HC8I
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[8];

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Attempt to convert each part of HC8R to INT
        lpAllTypes->aplHC8I.parts[i] = ConvertRatToInt (&lpaplHC8R[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1]
                 && bRet[2]
                 && bRet[3]
                 && bRet[4]
                 && bRet[5]
                 && bRet[6]
                 && bRet[7];
} // TCA_HC8R_HC8I


//***************************************************************************
//  $TCA_HC8R_FLT
//***************************************************************************

void TCA_HC8R_FLT
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the real part of HC8R as FLT
    lpAllTypes->aplFloat = mpq_get_d (&lpaplHC8R[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8R parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC8R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8R_FLT


//***************************************************************************
//  $TCA_HC8R_HC2F
//***************************************************************************

void TCA_HC8R_HC2F
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the parts of HC8R as HC2F
        lpAllTypes->aplHC2F.parts[i] = mpq_get_d (&lpaplHC8R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8R parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC8R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8R_HC2F


//***************************************************************************
//  $TCA_HC8R_HC4F
//***************************************************************************

void TCA_HC8R_HC4F
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the parts of HC8R as HC4F
        lpAllTypes->aplHC4F.parts[i] = mpq_get_d (&lpaplHC8R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8R parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC8R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8R_HC4F


//***************************************************************************
//  $TCA_HC8R_HC8F
//***************************************************************************

void TCA_HC8R_HC8F
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Copy the parts of HC8R as HC8F
        lpAllTypes->aplHC8F.parts[i] = mpq_get_d (&lpaplHC8R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC8R_HC8F


//***************************************************************************
//  $TCA_HC8R_RAT
//***************************************************************************

void TCA_HC8R_RAT
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Convert the real part of HC8R to a RAT
    mpq_init_set (&lpAllTypes->aplRat, &lpaplHC8R[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8R parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC8R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8R_RAT


//***************************************************************************
//  $TCA_HC8R_HC2R
//***************************************************************************

void TCA_HC8R_HC2R
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC8R as HC2R
        mpq_init_set (&lpAllTypes->aplHC2R.parts[i], &lpaplHC8R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8R parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC8R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8R_HC2R


//***************************************************************************
//  $TCA_HC8R_HC4R
//***************************************************************************

void TCA_HC8R_HC4R
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC8R as HC4R
        mpq_init_set (&lpAllTypes->aplHC4R.parts[i], &lpaplHC8R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8R parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC8R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4R_HC4R


//***************************************************************************
//  $TCA_HC8R_HC8R
//***************************************************************************

void TCA_HC8R_HC8R
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Copy the HC8R as HC8R
        mpq_init_set (&lpAllTypes->aplHC8R.parts[i], &lpaplHC8R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2R_HC8R


//***************************************************************************
//  $TCA_HC8R_VFP
//***************************************************************************

void TCA_HC8R_VFP
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the real part of HC8R as VFP
    mpfr_init_set_q  (&lpAllTypes->aplVfp, &lpaplHC8R[uInt].parts[0], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8R parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC8R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8R_VFP


//***************************************************************************
//  $TCA_HC8R_HC2V
//***************************************************************************

void TCA_HC8R_HC2V
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the parts of HC8R as HC2V
        mpfr_init_set_q  (&lpAllTypes->aplHC2V.parts[i], &lpaplHC8R[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8R parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC8R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8R_HC2V


//***************************************************************************
//  $TCA_HC8R_HC4V
//***************************************************************************

void TCA_HC8R_HC4V
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the parts of HC8R as HC4V
        mpfr_init_set_q  (&lpAllTypes->aplHC4V.parts[i], &lpaplHC8R[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8R parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8R part is a near 0
            lpbRet[0] &= ConvertRatToInt (&lpaplHC8R[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8R_HC4V


//***************************************************************************
//  $TCA_HC8R_HC8V
//***************************************************************************

void TCA_HC8R_HC8V
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Copy the parts of HC8R as HC8V
        mpfr_init_set_q  (&lpAllTypes->aplHC8V.parts[i], &lpaplHC8R[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC8R_HC8V


//***************************************************************************
//  $TCA_HC8V_BOOL
//***************************************************************************

void TCA_HC8V_BOOL
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part of HC8V to BOOL
    lpAllTypes->aplInteger = ConvertVfpToInt (&lpaplHC8V[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Mark as successful or not
        lpbRet[0] &= IsBooleanValue (lpAllTypes->aplInteger);

        // Loop through the remainder of the imaginary HC8V parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC8V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8V_BOOL


//***************************************************************************
//  $TCA_HC8V_INT
//***************************************************************************

void TCA_HC8V_INT
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the real part of HC8V to INT
    lpAllTypes->aplInteger = ConvertVfpToInt (&lpaplHC8V[uInt].parts[0], 0, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Loop through the remainder of the imaginary HC8V parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC8V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8V_INT


//***************************************************************************
//  $TCA_HC8V_HC2I
//***************************************************************************

void TCA_HC8V_HC2I
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert each part of HC8V to INT
        lpAllTypes->aplHC2I.parts[i] = ConvertVfpToInt (&lpaplHC8V[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
        // Loop through the remainder of the imaginary HC8V parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC8V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8V_HC2I


//***************************************************************************
//  $TCA_HC8V_HC4I
//***************************************************************************

void TCA_HC8V_HC4I
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[4];

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Attempt to convert each part of HC8V to INT
        lpAllTypes->aplHC4I.parts[i] = ConvertVfpToInt (&lpaplHC8V[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful or not
        lpbRet[0] &= bRet[0]
                  && bRet[1]
                  && bRet[2]
                  && bRet[3];
        // Loop through the remainder of the imaginary HC8V parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC8V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8V_HC4I


//***************************************************************************
//  $TCA_HC8V_HC8I
//***************************************************************************

void TCA_HC8V_HC8I
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[8];

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Attempt to convert each part of HC8V to INT
        lpAllTypes->aplHC8I.parts[i] = ConvertVfpToInt (&lpaplHC8V[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1]
                 && bRet[2]
                 && bRet[3]
                 && bRet[4]
                 && bRet[5]
                 && bRet[6]
                 && bRet[7];
} // TCA_HC8V_HC8I


//***************************************************************************
//  $TCA_HC8V_FLT
//***************************************************************************

void TCA_HC8V_FLT
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the real part of HC8V as FLT
    lpAllTypes->aplFloat = mpfr_get_d (&lpaplHC8V[uInt].parts[0], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8V parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC8V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8V_FLT


//***************************************************************************
//  $TCA_HC8V_HC2F
//***************************************************************************

void TCA_HC8V_HC2F
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the parts of HC8V as HC2F
        lpAllTypes->aplHC2F.parts[i] = mpfr_get_d (&lpaplHC8V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8V parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC8V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8V_HC2F


//***************************************************************************
//  $TCA_HC8V_HC4F
//***************************************************************************

void TCA_HC8V_HC4F
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the parts of HC8V as HC4F
        lpAllTypes->aplHC4F.parts[i] = mpfr_get_d (&lpaplHC8V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8V parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC8V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8V_HC4F


//***************************************************************************
//  $TCA_HC8V_HC8F
//***************************************************************************

void TCA_HC8V_HC8F
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Copy the parts of HC8V as HC8F
        lpAllTypes->aplHC8F.parts[i] = mpfr_get_d (&lpaplHC8V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC8V_HC8F


//***************************************************************************
//  $TCA_HC8V_RAT
//***************************************************************************

void TCA_HC8V_RAT
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Convert the real part of HC8V to a RAT
    mpq_init_set_fr (&lpAllTypes->aplRat, &lpaplHC8V[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8V parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC8V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8V_RAT


//***************************************************************************
//  $TCA_HC8V_HC2R
//***************************************************************************

void TCA_HC8V_HC2R
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC8V as HC4R
        mpq_init_set_fr (&lpAllTypes->aplHC4R.parts[i], &lpaplHC8V[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8V parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC8V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8V_HC2R


//***************************************************************************
//  $TCA_HC8V_HC4R
//***************************************************************************

void TCA_HC8V_HC4R
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC8V as HC4R
        mpq_init_set_fr (&lpAllTypes->aplHC4R.parts[i], &lpaplHC8V[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8V parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC8V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8V_HC4R


//***************************************************************************
//  $TCA_HC8V_HC8R
//***************************************************************************

void TCA_HC8V_HC8R
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Copy the HC8V as HC8R
        mpq_init_set_fr (&lpAllTypes->aplHC8R.parts[i], &lpaplHC8V[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC8V_HC8R


//***************************************************************************
//  $TCA_HC8V_VFP
//***************************************************************************

void TCA_HC8V_VFP
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Copy the real part of HC8V as VFP
    mpfr_init_set  (&lpAllTypes->aplVfp, &lpaplHC8V[uInt].parts[0], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8V parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC8V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8V_VFP


//***************************************************************************
//  $TCA_HC8V_HC2V
//***************************************************************************

void TCA_HC8V_HC2V
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Copy the HC8V part to HC2V
        mpfr_init_set (&lpAllTypes->aplHC2V.parts[i], &lpaplHC8V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8V parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC8V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8V_HC2V


//***************************************************************************
//  $TCA_HC8V_HC4V
//***************************************************************************

void TCA_HC8V_HC4V
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Copy the HC4V part to HC4V
        mpfr_init_set (&lpAllTypes->aplHC4V.parts[i], &lpaplHC8V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8V parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8V part is a near 0
            lpbRet[0] &= ConvertVfpToInt (&lpaplHC8V[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8V_HC4V


//***************************************************************************
//  $TCA_HC8V_HC8V
//***************************************************************************

void TCA_HC8V_HC8V
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Copy the HC8V part to HC8V
        mpfr_init_set (&lpAllTypes->aplHC8V.parts[0], &lpaplHC8V[uInt].parts[i], MPFR_RNDN);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC8V_HC8V


//***************************************************************************
//  $TCA_BOOL_BA1F
//***************************************************************************

void TCA_BOOL_BA1F
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part
    arb_init_set_sx (&lpAllTypes->aplArb, GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt));

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_BA1F


//***************************************************************************
//  $TCA_BOOL_BA2F
//***************************************************************************

void TCA_BOOL_BA2F
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    arb_init_set_sx (&lpAllTypes->aplBA2F.parts[0], GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt));

    // Loop through the remainder of the imaginary BA2F parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA2F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_BA2F


//***************************************************************************
//  $TCA_BOOL_BA4F
//***************************************************************************

void TCA_BOOL_BA4F
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    arb_init_set_sx (&lpAllTypes->aplBA4F.parts[0], GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt));

    // Loop through the remainder of the imaginary BA4F parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA4F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_BA4F


//***************************************************************************
//  $TCA_BOOL_BA8F
//***************************************************************************

void TCA_BOOL_BA8F
    (LPAPLBOOL   lpaplBoolean,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    arb_init_set_sx (&lpAllTypes->aplBA8F.parts[0], GetNextInteger (lpaplBoolean, ARRAY_BOOL, uInt));

    // Loop through the remainder of the imaginary BA8F parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA8F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BOOL_BA8F


//***************************************************************************
//  $TCA_INT_BA1F
//***************************************************************************

void TCA_INT_BA1F
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    arb_init_set_sx (&lpAllTypes->aplArb, lpaplInteger[uInt]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_BA1F


//***************************************************************************
//  $TCA_FLT_BA1F
//***************************************************************************

void TCA_FLT_BA1F
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Convert the FLT to an ARB
    Myarb_init  (&lpAllTypes->aplArb);
    arb_set_d (&lpAllTypes->aplArb, lpaplFloat[uInt]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_FLT_BA1F


//***************************************************************************
//  $TCA_RAT_BA1F
//***************************************************************************

void TCA_RAT_BA1F
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Initialize to 0/0
    Myarb_init  (&lpAllTypes->aplArb);

    // Copy the RAT as ARB
    arb_set_q     (&lpAllTypes->aplArb, &lpaplRat[uInt]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_RAT_BA1F


//***************************************************************************
//  $TCA_VFP_BA1F
//***************************************************************************

void TCA_VFP_BA1F
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Initialize to 0/0
    Myarb_init  (&lpAllTypes->aplArb);

    // Copy the VFP as ARB
    arb_set_mpfr (&lpAllTypes->aplArb, &lpaplVfp[uInt]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_VFP_BA1F


//***************************************************************************
//  $TCA_APA_BA1F
//***************************************************************************

void TCA_APA_BA1F
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    arb_init_set_sx (&lpAllTypes->aplArb, GetNextInteger (lpaplAPA, ARRAY_APA, uInt));

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_APA_BA1F


//***************************************************************************
//  $TCA_INT_BA2F
//***************************************************************************

void TCA_INT_BA2F
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    arb_init_set_sx (&lpAllTypes->aplBA2F.parts[0], lpaplInteger[uInt]);

    // Loop through the remainder of the imaginary BA2F parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA2F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_BA2F


//***************************************************************************
//  $TCA_FLT_BA2F
//***************************************************************************

void TCA_FLT_BA2F
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real aprt
    Myarb_init  (&lpAllTypes->aplBA2F.parts[0]);
    arb_set_d (&lpAllTypes->aplBA2F.parts[0], lpaplFloat[uInt]);

    // Loop through the remainder of the imaginary BA2F parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA2F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_FLT_BA2F


//***************************************************************************
//  $TCA_RAT_BA2F
//***************************************************************************

void TCA_RAT_BA2F
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    arb_init_set_q  (&lpAllTypes->aplBA2F.parts[0], &lpaplRat[uInt]);

    // Loop through the remainder of the imaginary BA2F parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA2F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_RAT_BA2F


//***************************************************************************
//  $TCA_VFP_BA2F
//***************************************************************************

void TCA_VFP_BA2F
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    arb_init_set_mpfr (&lpAllTypes->aplBA2F.parts[0], &lpaplVfp[uInt]);

    // Loop through the remainder of the imaginary BA2F parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA2F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_VFP_BA2F


//***************************************************************************
//  $TCA_APA_BA2F
//***************************************************************************

void TCA_APA_BA2F
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    arb_init_set_sx (&lpAllTypes->aplBA2F.parts[0], GetNextInteger (lpaplAPA, ARRAY_APA, uInt));

    // Loop through the remainder of the imaginary BA2F parts
    for (; i < 2; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA2F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_APA_BA2F


//***************************************************************************
//  $TCA_INT_BA4F
//***************************************************************************

void TCA_INT_BA4F
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    arb_init_set_sx (&lpAllTypes->aplBA4F.parts[0], lpaplInteger[uInt]);

    // Loop through the remainder of the imaginary BA4F parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA4F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_BA4F


//***************************************************************************
//  $TCA_FLT_BA4F
//***************************************************************************

void TCA_FLT_BA4F
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real aprt
    Myarb_init  (&lpAllTypes->aplBA4F.parts[0]);
    arb_set_d (&lpAllTypes->aplBA4F.parts[0], lpaplFloat[uInt]);

    // Loop through the remainder of the imaginary HC4V parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA4F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_FLT_BA4F


//***************************************************************************
//  $TCA_RAT_BA4F
//***************************************************************************

void TCA_RAT_BA4F
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    arb_init_set_q  (&lpAllTypes->aplBA4F.parts[0], &lpaplRat[uInt]);

    // Loop through the remainder of the imaginary BA4F parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA4F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_RAT_BA4F


//***************************************************************************
//  $TCA_VFP_BA4F
//***************************************************************************

void TCA_VFP_BA4F
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    arb_init_set_mpfr (&lpAllTypes->aplBA4F.parts[0], &lpaplVfp[uInt]);

    // Loop through the remainder of the imaginary BA4F parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA4F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_VFP_BA4F


//***************************************************************************
//  $TCA_APA_BA4F
//***************************************************************************

void TCA_APA_BA4F
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    arb_init_set_sx (&lpAllTypes->aplBA4F.parts[0], GetNextInteger (lpaplAPA, ARRAY_APA, uInt));

    // Loop through the remainder of the imaginary BA4F parts
    for (; i < 4; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA4F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_APA_BA4F


//***************************************************************************
//  $TCA_INT_BA8F
//***************************************************************************

void TCA_INT_BA8F
    (LPAPLINT    lpaplInteger,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    arb_init_set_sx (&lpAllTypes->aplBA8F.parts[0], lpaplInteger[uInt]);

    // Loop through the remainder of the imaginary BA8F parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA8F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_INT_BA8F


//***************************************************************************
//  $TCA_FLT_BA8F
//***************************************************************************

void TCA_FLT_BA8F
    (LPAPLFLOAT  lpaplFloat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real aprt
    Myarb_init  (&lpAllTypes->aplBA8F.parts[0]);
    arb_set_d (&lpAllTypes->aplBA8F.parts[0], lpaplFloat[uInt]);

    // Loop through the remainder of the imaginary HC8V parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA8F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_FLT_BA8F


//***************************************************************************
//  $TCA_RAT_BA8F
//***************************************************************************

void TCA_RAT_BA8F
    (LPAPLRAT    lpaplRat,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    arb_init_set_q  (&lpAllTypes->aplBA8F.parts[0], &lpaplRat[uInt]);

    // Loop through the remainder of the imaginary BA8F parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA8F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_RAT_BA8F


//***************************************************************************
//  $TCA_VFP_BA8F
//***************************************************************************

void TCA_VFP_BA8F
    (LPAPLVFP    lpaplVfp,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    arb_init_set_mpfr (&lpAllTypes->aplBA8F.parts[0], &lpaplVfp[uInt]);

    // Loop through the remainder of the imaginary BA8F parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA8F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_VFP_BA8F


//***************************************************************************
//  $TCA_APA_BA8F
//***************************************************************************

void TCA_APA_BA8F
    (LPAPLAPA    lpaplAPA,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part
    arb_init_set_sx (&lpAllTypes->aplBA8F.parts[0], GetNextInteger (lpaplAPA, ARRAY_APA, uInt));

    // Loop through the remainder of the imaginary BA8F parts
    for (; i < 8; i++)          // No bRet
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA8F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_APA_BA8F


//***************************************************************************
//  $TCA_HC2I_BA1F
//***************************************************************************

void TCA_HC2I_BA1F
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part to BA1F
    arb_init_set_sx (&lpAllTypes->aplArb, lpaplHC2I[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2I parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2I part is 0
            lpbRet[0] &= lpaplHC2I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC2I_BA1F


//***************************************************************************
//  $TCA_HC4I_BA1F
//***************************************************************************

void TCA_HC4I_BA1F
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part to BA1F
    arb_init_set_sx (&lpAllTypes->aplArb, lpaplHC4I[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4I parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4I part is 0
            lpbRet[0] &= lpaplHC4I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC4I_BA1F


//***************************************************************************
//  $TCA_HC8I_BA1F
//***************************************************************************

void TCA_HC8I_BA1F
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part to BA1F
    arb_init_set_sx (&lpAllTypes->aplArb, lpaplHC8I[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8I parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8I part is 0
            lpbRet[0] &= lpaplHC8I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC8I_BA1F


//***************************************************************************
//  $TCA_HC2I_BA2F
//***************************************************************************

void TCA_HC2I_BA2F
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb2f_init (&lpAllTypes->aplBA2F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        // Convert to BA2F
        arb_set_sx (&lpAllTypes->aplBA2F.parts[i], lpaplHC2I[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2I_BA2F


//***************************************************************************
//  $TCA_HC4I_BA2F
//***************************************************************************

void TCA_HC4I_BA2F
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb2f_init (&lpAllTypes->aplBA2F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        // Convert to BA2F
        arb_set_sx (&lpAllTypes->aplBA2F.parts[i], lpaplHC4I[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4I parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4I part is 0
            lpbRet[0] &= lpaplHC4I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC4I_BA2F


//***************************************************************************
//  $TCA_HC8I_BA2F
//***************************************************************************

void TCA_HC8I_BA2F
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Set the real part to BA2F
    arb2f_init (&lpAllTypes->aplBA2F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        // Convert to BA2F
        arb_set_sx (&lpAllTypes->aplBA2F.parts[i], lpaplHC8I[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8I parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8I part is 0
            lpbRet[0] &= lpaplHC8I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC8I_BA2F


//***************************************************************************
//  $TCA_HC8I_BA4F
//***************************************************************************

void TCA_HC8I_BA4F
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through the common parts
    for (i = 0; i < 4; i++)
        // Convert to BA4F
        arb_set_sx (&lpAllTypes->aplBA4F.parts[i], lpaplHC8I[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8I parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8I part is 0
            lpbRet[0] &= lpaplHC8I[uInt].parts[i] EQ 0;
    } // End IF
} // TCA_HC8I_BA4F


//***************************************************************************
//  $TCA_HC2F_BA1F
//***************************************************************************

void TCA_HC2F_BA1F
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Set the real part to ARB
    arb_init_set_d (&lpAllTypes->aplBA1F, lpaplHC2F[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2F parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC2F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC2F_BA1F


//***************************************************************************
//  $TCA_HC4F_BA1F
//***************************************************************************

void TCA_HC4F_BA1F
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to ARB
    arb_init_set_d (&lpAllTypes->aplArb, lpaplHC4F[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4F parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC4F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4F_BA1F


//***************************************************************************
//  $TCA_HC8F_BA1F
//***************************************************************************

void TCA_HC8F_BA1F
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the real part to ARB
    arb_init_set_d (&lpAllTypes->aplArb, lpaplHC8F[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8F_BA1F


//***************************************************************************
//  $TCA_HC2R_BA1F
//***************************************************************************

void TCA_HC2R_BA1F
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Initialize to 0/0
    Myarb_init  (&lpAllTypes->aplArb);

    // Copy the RAT as ARB
    arb_set_q     (&lpAllTypes->aplArb, &lpaplHC2R[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2R parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2R part is 0
            lpbRet[0] &= IsZeroHCxR (&lpaplHC2R[uInt].parts[i], 2);
    } // End IF
} // TCA_HC2R_BA1F


//***************************************************************************
//  $TCA_HC4R_BA1F
//***************************************************************************

void TCA_HC4R_BA1F
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Initialize to 0/0
    Myarb_init  (&lpAllTypes->aplArb);
    arb_set_q (&lpAllTypes->aplArb, &lpaplHC4R[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4R parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4R part is 0
            lpbRet[0] &= IsZeroHCxR (&lpaplHC4R[uInt].parts[i], 4);
    } // End IF
} // TCA_HC4R_BA1F


//***************************************************************************
//  $TCA_HC8R_BA1F
//***************************************************************************

void TCA_HC8R_BA1F
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Convert the RAT to ARB
    lpAllTypes->aplArb = ConvertRatToArb (&lpaplHC8R->parts[0], uInt);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8R parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8R part is 0
            lpbRet[0] &= IsZeroHCxR (&lpaplHC8R[uInt].parts[i], 8);
    } // End IF
} // TCA_HC8R_BA1F


//***************************************************************************
//  $TCA_HC2R_BA2F
//***************************************************************************

void TCA_HC2R_BA2F
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Convert the RAT to ARB
    lpAllTypes->aplBA2F.parts[0] = ConvertRatToArb (&lpaplHC2R[uInt].parts[0], 0);
    lpAllTypes->aplBA2F.parts[1] = ConvertRatToArb (&lpaplHC2R[uInt].parts[1], 0);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2R_BA2F


//***************************************************************************
//  $TCA_HC4R_BA2F
//***************************************************************************

void TCA_HC4R_BA2F
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        // Convert the RAT to ARB
        lpAllTypes->aplBA2F.parts[i] = ConvertRatToArb (&lpaplHC4R[uInt].parts[i], 0);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4R parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4R part is 0
            lpbRet[0] &= IsZeroHCxR (&lpaplHC4R[uInt].parts[i], 4);
    } // End IF
} // TCA_HC4R_BA2F


//***************************************************************************
//  $TCA_HC8R_BA2F
//***************************************************************************

void TCA_HC8R_BA2F
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb2f_init (&lpAllTypes->aplBA2F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        // Convert the RAT to ARB
        lpAllTypes->aplBA2F.parts[i] = ConvertRatToArb (&lpaplHC8R[uInt].parts[i], 0);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8R parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8R part is 0
            lpbRet[0] &= IsZeroHCxR (&lpaplHC8R[uInt].parts[i], 8);
    } // End IF
} // TCA_HC8R_BA2F


//***************************************************************************
//  $TCA_HC2R_BA4F
//***************************************************************************

void TCA_HC2R_BA4F
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        // Convert the RAT to ARB
        lpAllTypes->aplBA4F.parts[i] = ConvertRatToArb (&lpaplHC2R[uInt].parts[i], 0);

    // Loop through the remainder of the imaginary BA4F parts
    for (; lpbRet[0] && i < 4; i++)
        // Set the imaginary parts to 0/0
        Myarb_init (&lpAllTypes->aplBA4F.parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2R_BA4F


//***************************************************************************
//  $TCA_HC4R_BA4F
//***************************************************************************

void TCA_HC4R_BA4F
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb4f_init   (&lpAllTypes->aplBA4F);

    // Loop through the common parts
    for (i = 0; i < 4; i++)
        // Convert the RAT to ARB
        lpAllTypes->aplBA4F.parts[i] = ConvertRatToArb (&lpaplHC4R[uInt].parts[i], 0);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4R_BA4F


//***************************************************************************
//  $TCA_HC8R_BA4F
//***************************************************************************

void TCA_HC8R_BA4F
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb4f_init   (&lpAllTypes->aplBA4F);

    // Loop through the common parts
    for (i = 0; i < 4; i++)
        // Convert the RAT to ARB
        lpAllTypes->aplBA2F.parts[i] = ConvertRatToArb (&lpaplHC8R[uInt].parts[i], 0);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8R parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8R part is 0
            lpbRet[0] &= IsZeroHCxR (&lpaplHC8R[uInt].parts[i], 8);
    } // End IF
} // TCA_HC8R_BA4F


//***************************************************************************
//  $TCA_HC2V_BA1F
//***************************************************************************

void TCA_HC2V_BA1F
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    Myarb_init     (&lpAllTypes->aplArb);
    arb_set_mpfr (&lpAllTypes->aplArb, &lpaplHC2V[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2I parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2I part is 0
            lpbRet[0] &= IsZeroHCxV (&lpaplHC2V[uInt].parts[i], 2);
    } // End IF
} // TCA_HC2V_BA1F


//***************************************************************************
//  $TCA_HC4V_BA1F
//***************************************************************************

void TCA_HC4V_BA1F
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    Myarb_init     (&lpAllTypes->aplArb);
    arb_set_mpfr (&lpAllTypes->aplArb, &lpaplHC4V[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4I parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4I part is 0
            lpbRet[0] &= IsZeroHCxV (&lpaplHC4V[uInt].parts[i], 4);
    } // End IF
} // TCA_HC4V_BA1F


//***************************************************************************
//  $TCA_HC8V_BA1F
//***************************************************************************

void TCA_HC8V_BA1F
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    Myarb_init     (&lpAllTypes->aplArb);
    arb_set_mpfr (&lpAllTypes->aplArb, &lpaplHC8V[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8I parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8I part is 0
            lpbRet[0] &= IsZeroHCxV (&lpaplHC8V[uInt].parts[i], 8);
    } // End IF
} // TCA_HC8V_BA1F


//***************************************************************************
//  $TCA_HC2V_BA2F
//***************************************************************************

void TCA_HC2V_BA2F
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    arb2f_init   (&lpAllTypes->aplBA2F);
    arb_set_mpfr (&lpAllTypes->aplBA2F.parts[0], &lpaplHC2V[uInt].parts[0]);
    arb_set_mpfr (&lpAllTypes->aplBA2F.parts[1], &lpaplHC2V[uInt].parts[1]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2V_BA2F


//***************************************************************************
//  $TCA_HC4V_BA2F
//***************************************************************************

void TCA_HC4V_BA2F
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb2f_init   (&lpAllTypes->aplBA2F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        arb_set_mpfr (&lpAllTypes->aplBA2F.parts[i], &lpaplHC4V[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4V parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4V part is 0
            lpbRet[0] &= IsZeroHCxV (&lpaplHC4V[uInt].parts[i], 4);
    } // End IF
} // TCA_HC4V_BA2F


//***************************************************************************
//  $TCA_HC8V_BA2F
//***************************************************************************

void TCA_HC8V_BA2F
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb2f_init   (&lpAllTypes->aplBA2F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        arb_set_mpfr (&lpAllTypes->aplBA2F.parts[i], &lpaplHC8V[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC2I parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC2I part is 0
            lpbRet[0] &= IsZeroHCxV (&lpaplHC8V[uInt].parts[i], 8);
    } // End IF
} // TCA_HC8V_BA2F


//***************************************************************************
//  $TCA_HC2V_BA4F
//***************************************************************************

void TCA_HC2V_BA4F
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb4f_init   (&lpAllTypes->aplBA4F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        arb_set_mpfr (&lpAllTypes->aplBA4F.parts[i], &lpaplHC2V[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2V_BA4F


//***************************************************************************
//  $TCA_HC4V_BA4F
//***************************************************************************

void TCA_HC4V_BA4F
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb4f_init   (&lpAllTypes->aplBA4F);

    // Loop through the common parts
    for (i = 0; i < 4; i++)
        arb_set_mpfr (&lpAllTypes->aplBA4F.parts[i], &lpaplHC4V[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4V_BA4F


//***************************************************************************
//  $TCA_HC8V_BA4F
//***************************************************************************

void TCA_HC8V_BA4F
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb4f_init   (&lpAllTypes->aplBA4F);

    // Loop through the common parts
    for (i = 0; i < 4; i++)
        arb_set_mpfr (&lpAllTypes->aplBA4F.parts[i], &lpaplHC8V[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8V parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8V part is 0
            lpbRet[0] &= IsZeroHCxV (&lpaplHC8V[uInt].parts[i], 8);
    } // End IF
} // TCA_HC8V_BA4F


//***************************************************************************
//  $TCA_HC2F_BA2F
//***************************************************************************

void TCA_HC2F_BA2F
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Set the low part of ARB
    arb_init_set_d (&lpAllTypes->aplBA2F.parts[0], lpaplHC2F[uInt].parts[0]);
    arb_init_set_d (&lpAllTypes->aplBA2F.parts[1], lpaplHC2F[uInt].parts[1]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2F_BA2F


//***************************************************************************
//  $TCA_HC4F_BA2F
//***************************************************************************

void TCA_HC4F_BA2F
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb2f_init (&lpAllTypes->aplBA2F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        // Set the real part to ARB
        arb_set_d  (&lpAllTypes->aplBA2F.parts[i], lpaplHC4F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC4F parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC4F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC4F_BA2F


//***************************************************************************
//  $TCA_HC8F_BA2F
//***************************************************************************

void TCA_HC8F_BA2F
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb2f_init (&lpAllTypes->aplBA2F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        // Set the real part to ARB
        arb_set_d  (&lpAllTypes->aplBA2F.parts[i], lpaplHC8F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8F_BA2F


//***************************************************************************
//  $TCA_HC8F_BA4F
//***************************************************************************

void TCA_HC8F_BA4F
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through the common parts
    for (i = 0; i < 4; i++)
        arb_set_d  (&lpAllTypes->aplBA4F.parts[i], lpaplHC8F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary HC8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8F part is a near 0
            lpbRet[0] &= ConvertFltToInt (&lpaplHC8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_HC8F_BA4F


//***************************************************************************
//  $TCA_HC2I_BA4F
//***************************************************************************

void TCA_HC2I_BA4F
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        arb_set_sx (&lpAllTypes->aplBA4F.parts[i], lpaplHC2I[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2I_BA4F


//***************************************************************************
//  $TCA_HC2F_BA4F
//***************************************************************************

void TCA_HC2F_BA4F
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        arb_set_d  (&lpAllTypes->aplBA4F.parts[i], lpaplHC2F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2F_BA4F


//***************************************************************************
//  $TCA_HC2I_BA8F
//***************************************************************************

void TCA_HC2I_BA8F
    (LPAPLHC2I   lpaplHC2I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        arb_set_sx (&lpAllTypes->aplBA4F.parts[i], lpaplHC2I[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2I_BA8F


//***************************************************************************
//  $TCA_HC2F_BA8F
//***************************************************************************

void TCA_HC2F_BA8F
    (LPAPLHC2F   lpaplHC2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        arb_set_d  (&lpAllTypes->aplBA4F.parts[i], lpaplHC2F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2F_BA8F


//***************************************************************************
//  $TCA_HC2R_BA8F
//***************************************************************************

void TCA_HC2R_BA8F
    (LPAPLHC2R   lpaplHC2R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        arb_set_q  (&lpAllTypes->aplBA4F.parts[i], &lpaplHC2R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2R_BA8F


//***************************************************************************
//  $TCA_HC2V_BA8F
//***************************************************************************

void TCA_HC2V_BA8F
    (LPAPLHC2V   lpaplHC2V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        arb_set_mpfr (&lpAllTypes->aplBA4F.parts[i], &lpaplHC2V[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC2V_BA8F


//***************************************************************************
//  $TCA_HC4I_BA4F
//***************************************************************************

void TCA_HC4I_BA4F
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through all the parts
    for (i = 0; i < 4; i++)
        arb_set_sx   (&lpAllTypes->aplBA4F.parts[i], lpaplHC4I[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4I_BA4F


//***************************************************************************
//  $TCA_HC4F_BA4F
//***************************************************************************

void TCA_HC4F_BA4F
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through all the parts
    for (i = 0; i < 4; i++)
        arb_set_d    (&lpAllTypes->aplBA4F.parts[i], lpaplHC4F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4F_BA4F


//***************************************************************************
//  $TCA_HC4I_BA8F
//***************************************************************************

void TCA_HC4I_BA8F
    (LPAPLHC4I   lpaplHC4I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all the parts
    for (i = 0; i < 4; i++)
        arb_set_sx   (&lpAllTypes->aplBA8F.parts[i], lpaplHC4I[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4I_BA8F


//***************************************************************************
//  $TCA_HC4F_BA8F
//***************************************************************************

void TCA_HC4F_BA8F
    (LPAPLHC4F   lpaplHC4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all the parts
    for (i = 0; i < 4; i++)
        arb_set_d    (&lpAllTypes->aplBA8F.parts[i], lpaplHC4F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4F_BA8F


//***************************************************************************
//  $TCA_HC4R_BA8F
//***************************************************************************

void TCA_HC4R_BA8F
    (LPAPLHC4R   lpaplHC4R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all the parts
    for (i = 0; i < 4; i++)
        arb_set_q    (&lpAllTypes->aplBA8F.parts[i], &lpaplHC4R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4R_BA8F


//***************************************************************************
//  $TCA_HC4V_BA8F
//***************************************************************************

void TCA_HC4V_BA8F
    (LPAPLHC4V   lpaplHC4V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all the parts
    for (i = 0; i < 4; i++)
        arb_set_mpfr (&lpAllTypes->aplBA8F.parts[i], &lpaplHC4V[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC4V_BA8F


//***************************************************************************
//  $TCA_HC8I_BA8F
//***************************************************************************

void TCA_HC8I_BA8F
    (LPAPLHC8I   lpaplHC8I,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all the parts
    for (i = 0; i < 8; i++)
        arb_set_sx   (&lpAllTypes->aplBA8F.parts[i], lpaplHC8I[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC8I_BA8F


//***************************************************************************
//  $TCA_HC8F_BA8F
//***************************************************************************

void TCA_HC8F_BA8F
    (LPAPLHC8F   lpaplHC8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all the parts
    for (i = 0; i < 8; i++)
        arb_set_d    (&lpAllTypes->aplBA8F.parts[i], lpaplHC8F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC8F_BA8F


//***************************************************************************
//  $TCA_HC8R_BA8F
//***************************************************************************

void TCA_HC8R_BA8F
    (LPAPLHC8R   lpaplHC8R,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all the parts
    for (i = 0; i < 8; i++)
        arb_set_q    (&lpAllTypes->aplBA8F.parts[i], &lpaplHC8R[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC8R_BA8F


//***************************************************************************
//  $TCA_HC8V_BA8F
//***************************************************************************

void TCA_HC8V_BA8F
    (LPAPLHC8V   lpaplHC8V,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through all the parts
    for (i = 0; i < 8; i++)
        arb_set_mpfr (&lpAllTypes->aplBA8F.parts[i], &lpaplHC8V[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_HC8V_BA8F


//***************************************************************************
//  $TCA_BA1F_BOOL
//***************************************************************************

void TCA_BA1F_BOOL
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the parts to INT
    lpAllTypes->aplInteger = ConvertArbToInt (lpaplBA1F, uInt, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
        // Mark as successful or not
        lpbRet[0] = IsBooleanValue (lpAllTypes->aplInteger);
} // TCA_BA1F_BOOL


//***************************************************************************
//  $TCA_BA1F_INT
//***************************************************************************

void TCA_BA1F_INT
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the parts to INT
    lpAllTypes->aplInteger = ConvertArbToInt (lpaplBA1F, uInt, lpAllTypes, lpbRet);

////// If the flag ptr is valid, ...
////if (lpbRet NE NULL)
////    // Mark as successful
////    lpbRet[0] = TRUE;       // Already set above
} // TCA_BA1F_INT


//***************************************************************************
//  $TCA_BA1F_FLT
//***************************************************************************

void TCA_BA1F_FLT
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA1F[uInt]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Attempt to convert the parts to FLT
        lpAllTypes->aplFloat = arb_get_d (&lpaplBA1F[uInt]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA1F_FLT


//***************************************************************************
//  $TCA_BA1F_RAT
//***************************************************************************

void TCA_BA1F_RAT
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA1F[uInt]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Attempt to convert the parts to RAT
        lpAllTypes->aplRat = arb_get_q (&lpaplBA1F[uInt]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA1F_RAT


//***************************************************************************
//  $TCA_BA1F_VFP
//***************************************************************************

void TCA_BA1F_VFP
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA1F[uInt]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Attempt to convert the parts to RAT
        lpAllTypes->aplVfp = arb_get_mpfr (&lpaplBA1F[uInt]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA1F_VFP


//***************************************************************************
//  $TCA_BA1F_HC2I
//***************************************************************************

void TCA_BA1F_HC2I
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Attempt to convert the parts to INT
    lpAllTypes->aplHC2I.parts[0] = ConvertArbToInt (&lpaplBA1F[uInt], 0, lpAllTypes, lpbRet);

    // Loop through the remainder of the imaginary HC2I parts
    for (; i < 2; i++)          // No bRet
        // Zero the imaginary part
        lpAllTypes->aplHC2I.parts[i] = 0;

////// If the flag ptr is valid, ...
////if (lpbRet NE NULL)
////    // Mark as successful
////    lpbRet[0] = TRUE;       // Already set above
} // TCA_BA1F_HC2I


//***************************************************************************
//  $TCA_BA1F_HC4I
//***************************************************************************

void TCA_BA1F_HC4I
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Attempt to convert the parts to INT
    lpAllTypes->aplHC4I.parts[0] = ConvertArbToInt (&lpaplBA1F[uInt], 0, lpAllTypes, lpbRet);

    // Loop through the remainder of the imaginary HC4I parts
    for (; i < 4; i++)          // No bRet
        // Zero the imaginary part
        lpAllTypes->aplHC4I.parts[i] = 0;

////// If the flag ptr is valid, ...
////if (lpbRet NE NULL)
////    // Mark as successful
////    lpbRet[0] = TRUE;       // Already set above
} // TCA_BA1F_HC4I


//***************************************************************************
//  $TCA_BA1F_HC8I
//***************************************************************************

void TCA_BA1F_HC8I
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // Attempt to convert the parts to INT
    lpAllTypes->aplHC8I.parts[0] = ConvertArbToInt (&lpaplBA1F[uInt], 0, lpAllTypes, lpbRet);

    // Loop through the remainder of the imaginary HC8I parts
    for (; i < 8; i++)          // No bRet
        // Zero the imaginary part
        lpAllTypes->aplHC8I.parts[i] = 0;

////// If the flag ptr is valid, ...
////if (lpbRet NE NULL)
////    // Mark as successful
////    lpbRet[0] = TRUE;       // Already set above
} // TCA_BA1F_HC8I


//***************************************************************************
//  $TCA_BA1F_HC2F
//***************************************************************************

void TCA_BA1F_HC2F
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA1F[uInt]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Attempt to convert the parts to FLT
        lpAllTypes->aplHC2F.parts[0] = arb_get_d (&lpaplBA1F[uInt]);

        // Loop through the remainder of the imaginary HC2F parts
        for (; i < 2; i++)          // No bRet
            // Zero the imaginary part
            lpAllTypes->aplHC2F.parts[i] = 0;

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA1F_HC2F


//***************************************************************************
//  $TCA_BA1F_HC4F
//***************************************************************************

void TCA_BA1F_HC4F
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA1F[uInt]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Attempt to convert the parts to FLT
        lpAllTypes->aplHC4F.parts[0] = arb_get_d (&lpaplBA1F[uInt]);

        // Loop through the remainder of the imaginary HC4F parts
        for (; i < 4; i++)          // No bRet
            // Zero the imaginary part
            lpAllTypes->aplHC4F.parts[i] = 0;

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA1F_HC4F


//***************************************************************************
//  $TCA_BA1F_HC8F
//***************************************************************************

void TCA_BA1F_HC8F
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA1F[uInt]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Attempt to convert the parts to FLT
        lpAllTypes->aplHC8F.parts[0] = arb_get_d (&lpaplBA1F[uInt]);

        // Loop through the remainder of the imaginary HC8F parts
        for (; i < 8; i++)          // No bRet
            // Zero the imaginary part
            lpAllTypes->aplHC8F.parts[i] = 0;

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA1F_HC8F


//***************************************************************************
//  $TCA_BA1F_HC2R
//***************************************************************************

void TCA_BA1F_HC2R
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA1F[uInt]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Attempt to convert the parts to RAT
        lpAllTypes->aplHC2R.parts[0] = arb_get_q (&lpaplBA1F[uInt]);

        // Loop through the remainder of the imaginary HC2R parts
        for (; i < 2; i++)          // No bRet
            // Zero the imaginary part
            mpq_init (&lpAllTypes->aplHC2R.parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA1F_HC2R


//***************************************************************************
//  $TCA_BA1F_HC4R
//***************************************************************************

void TCA_BA1F_HC4R
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA1F[uInt]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Attempt to convert the parts to FLT
        lpAllTypes->aplHC4R.parts[0] = arb_get_q (&lpaplBA1F[uInt]);

        // Loop through the remainder of the imaginary HC4R parts
        for (; i < 4; i++)          // No bRet
            // Zero the imaginary part
            mpq_init (&lpAllTypes->aplHC4R.parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA1F_HC4R


//***************************************************************************
//  $TCA_BA1F_HC8R
//***************************************************************************

void TCA_BA1F_HC8R
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA1F[uInt]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Attempt to convert the parts to FLT
        lpAllTypes->aplHC8R.parts[0] = arb_get_q (&lpaplBA1F[uInt]);

        // Loop through the remainder of the imaginary HC8R parts
        for (; i < 8; i++)          // No bRet
            // Zero the imaginary part
            mpq_init (&lpAllTypes->aplHC8R.parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA1F_HC8R


//***************************************************************************
//  $TCA_BA1F_HC2V
//***************************************************************************

void TCA_BA1F_HC2V
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA1F[uInt]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Attempt to convert the parts to VFP
        lpAllTypes->aplHC2V.parts[0] = arb_get_mpfr (&lpaplBA1F[uInt]);

        // Loop through the remainder of the imaginary HC2V parts
        for (; i < 2; i++)          // No bRet
            // Zero the imaginary part
            mpfr_init0 (&lpAllTypes->aplHC2V.parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA1F_HC2V


//***************************************************************************
//  $TCA_BA1F_HC4V
//***************************************************************************

void TCA_BA1F_HC4V
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA1F[uInt]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Attempt to convert the parts to VFP
        lpAllTypes->aplHC4V.parts[0] = arb_get_mpfr (&lpaplBA1F[uInt]);

        // Loop through the remainder of the imaginary HC4V parts
        for (; i < 4; i++)          // No bRet
            // Zero the imaginary part
            mpfr_init0 (&lpAllTypes->aplHC4V.parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA1F_HC4V


//***************************************************************************
//  $TCA_BA1F_HC8V
//***************************************************************************

void TCA_BA1F_HC8V
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i = 1;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA1F[uInt]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Attempt to convert the parts to VFP
        lpAllTypes->aplHC8V.parts[0] = arb_get_mpfr (&lpaplBA1F[uInt]);

        // Loop through the remainder of the imaginary HC8V parts
        for (; i < 8; i++)          // No bRet
            // Zero the imaginary part
            mpfr_init0 (&lpAllTypes->aplHC8V.parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA1F_HC8V


//***************************************************************************
//  $TCA_BA2F_BOOL
//***************************************************************************

void TCA_BA2F_BOOL
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the real part to an integer
    lpAllTypes->aplInteger = ConvertArbToInt (&lpaplBA2F->parts[0], uInt, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Mark as successful or not
        lpbRet[0] &= IsBooleanValue (lpAllTypes->aplInteger);

        // Loop through the remainder of the imaginary BA2F parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary BA2F part is a near 0
            lpbRet[0] &= ConvertArbToInt (&lpaplBA2F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_BA2F_BOOL


//***************************************************************************
//  $TCA_BA4F_BOOL
//***************************************************************************

void TCA_BA4F_BOOL
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the real part to an integer
    lpAllTypes->aplInteger = ConvertArbToInt (&lpaplBA4F->parts[0], uInt, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Mark as successful or not
        lpbRet[0] &= IsBooleanValue (lpAllTypes->aplInteger);

        // Loop through the remainder of the imaginary BA4F parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary BA4F part is a near 0
            lpbRet[0] &= ConvertArbToInt (&lpaplBA4F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_BA4F_BOOL


//***************************************************************************
//  $TCA_BA8F_BOOL
//***************************************************************************

void TCA_BA8F_BOOL
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the real part to an integer
    lpAllTypes->aplInteger = ConvertArbToInt (&lpaplBA8F->parts[0], uInt, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Mark as successful or not
        lpbRet[0] &= IsBooleanValue (lpAllTypes->aplInteger);

        // Loop through the remainder of the imaginary BA8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary BA8F part is a near 0
            lpbRet[0] &= ConvertArbToInt (&lpaplBA8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_BA8F_BOOL


//***************************************************************************
//  $TCA_BA2F_INT
//***************************************************************************

void TCA_BA2F_INT
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the real part to an integer
    lpAllTypes->aplInteger = ConvertArbToInt (&lpaplBA2F->parts[0], uInt, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Loop through the remainder of the imaginary BA2F parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary HC2V part is a near 0
            lpbRet[0] &= ConvertArbToInt (&lpaplBA2F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_BA2F_INT


//***************************************************************************
//  $TCA_BA4F_INT
//***************************************************************************

void TCA_BA4F_INT
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the real part to an integer
    lpAllTypes->aplInteger = ConvertArbToInt (&lpaplBA4F->parts[0], uInt, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Loop through the remainder of the imaginary BA4F parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary HC4V part is a near 0
            lpbRet[0] &= ConvertArbToInt (&lpaplBA4F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_BA4F_INT


//***************************************************************************
//  $TCA_BA8F_INT
//***************************************************************************

void TCA_BA8F_INT
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the real part to an integer
    lpAllTypes->aplInteger = ConvertArbToInt (&lpaplBA8F->parts[0], uInt, lpAllTypes, lpbRet);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL
     && lpbRet[0])
    {
        int i = 1;

        // Loop through the remainder of the imaginary BA8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary HC8V part is a near 0
            lpbRet[0] &= ConvertArbToInt (&lpaplBA8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_BA8F_INT


//***************************************************************************
//  $TCA_BA2F_FLT
//***************************************************************************

void TCA_BA2F_FLT
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA2F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA2F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Attempt to convert the real part to a FLT
        lpAllTypes->aplFloat = arb_get_d (&lpaplBA2F->parts[0]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL
         && lpbRet[0])
        {
            int i = 1;

            // Loop through the remainder of the imaginary BA2F parts
            for (; lpbRet[0] && i < 2; i++)
                // Ensure the imaginary HC2V part is a near 0
                lpbRet[0] &= ConvertArbToInt (&lpaplBA2F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
        } // End IF
    } // End IF/ELSE
} // TCA_BA2F_FLT


//***************************************************************************
//  $TCA_BA4F_FLT
//***************************************************************************

void TCA_BA4F_FLT
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA4F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Attempt to convert the real part to FLT
        lpAllTypes->aplFloat = arb_get_d (&lpaplBA4F->parts[0]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
        {
            int i = 1;

            // Mark as successful
            lpbRet[0] = TRUE;

            // Loop through the remainder of the imaginary BA4F parts
            for (; lpbRet[0] && i < 4; i++)
                // Ensure the imaginary HC4V part is a near 0
                lpbRet[0] &= ConvertArbToInt (&lpaplBA4F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
        } // End IF
    } // End IF/ELSE
} // TCA_BA4F_FLT


//***************************************************************************
//  $TCA_BA8F_FLT
//***************************************************************************

void TCA_BA8F_FLT
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA8F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[4]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[5]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[6]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[7]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Attempt to convert the real part to FLT
        lpAllTypes->aplFloat = arb_get_d (&lpaplBA8F->parts[0]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
        {
            int i = 1;

            // Mark as successful
            lpbRet[0] = TRUE;

            // Loop through the remainder of the imaginary BA8F parts
            for (; lpbRet[0] && i < 8; i++)
                // Ensure the imaginary HC8V part is a near 0
                lpbRet[0] &= ConvertArbToInt (&lpaplBA8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
        } // End IF
    } // End IF/ELSE
} // TCA_BA8F_FLT


//***************************************************************************
//  $TCA_BA2F_RAT
//***************************************************************************

void TCA_BA2F_RAT
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA2F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA2F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Convert the real part to a RAT
        lpAllTypes->aplRat = arb_get_q (&lpaplBA2F[uInt].parts[0]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
        {
            int i = 1;

            // Mark as successful
            lpbRet[0] = TRUE;

            // Loop through the remainder of the imaginary BA2F parts
            for (; lpbRet[0] && i < 2; i++)
                // Ensure the imaginary BA2F part is a near 0
                lpbRet[0] &= ConvertArbToInt (&lpaplBA2F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
        } // End IF
    } // End IF/ELSE
} // TCA_BA2F_RAT


//***************************************************************************
//  $TCA_BA4F_RAT
//***************************************************************************

void TCA_BA4F_RAT
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA4F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Convert the real part to a RAT
        lpAllTypes->aplRat = arb_get_q (&lpaplBA4F[uInt].parts[0]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
        {
            int i = 1;

            // Mark as successful
            lpbRet[0] = TRUE;

            // Loop through the remainder of the imaginary BA4F parts
            for (; lpbRet[0] && i < 4; i++)
                // Ensure the imaginary BA4F part is a near 0
                lpbRet[0] &= ConvertArbToInt (&lpaplBA4F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
        } // End IF
    } // End IF/ELSE
} // TCA_BA4F_RAT


//***************************************************************************
//  $TCA_BA8F_RAT
//***************************************************************************

void TCA_BA8F_RAT
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA8F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[4]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[5]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[6]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[7]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Convert the real part to a RAT
        lpAllTypes->aplRat = arb_get_q (&lpaplBA8F[uInt].parts[0]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
        {
            int i = 1;

            // Mark as successful
            lpbRet[0] = TRUE;

            // Loop through the remainder of the imaginary BA8F parts
            for (; lpbRet[0] && i < 8; i++)
                // Ensure the imaginary BA8F part is a near 0
                lpbRet[0] &= ConvertArbToInt (&lpaplBA8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
        } // End IF
    } // End IF/ELSE
} // TCA_BA8F_RAT


//***************************************************************************
//  $TCA_BA2F_VFP
//***************************************************************************

void TCA_BA2F_VFP
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA2F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA2F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Convert the real part to a VFP
        lpAllTypes->aplVfp = arb_get_mpfr (&lpaplBA2F[uInt].parts[0]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
        {
            int i = 1;

            // Mark as successful
            lpbRet[0] = TRUE;

            // Loop through the remainder of the imaginary BA2F parts
            for (; lpbRet[0] && i < 2; i++)
                // Ensure the imaginary BA2F part is a near 0
                lpbRet[0] &= ConvertArbToInt (&lpaplBA2F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
        } // End IF
    } // End IF/ELSE
} // TCA_BA2F_VFP


//***************************************************************************
//  $TCA_BA4F_VFP
//***************************************************************************

void TCA_BA4F_VFP
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA4F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Convert the real part to a VFP
        lpAllTypes->aplVfp = arb_get_mpfr (&lpaplBA4F[uInt].parts[0]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
        {
            int i = 1;

            // Mark as successful
            lpbRet[0] = TRUE;

            // Loop through the remainder of the imaginary BA4F parts
            for (; lpbRet[0] && i < 4; i++)
                // Ensure the imaginary BA4F part is a near 0
                lpbRet[0] &= ConvertArbToInt (&lpaplBA4F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
        } // End IF
    } // End IF/ELSE
} // TCA_BA4F_VFP


//***************************************************************************
//  $TCA_BA8F_VFP
//***************************************************************************

void TCA_BA8F_VFP
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA8F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[4]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[5]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[6]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[7]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Convert the real part to a VFP
        lpAllTypes->aplVfp = arb_get_mpfr (&lpaplBA8F[uInt].parts[0]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
        {
            int i = 1;

            // Mark as successful
            lpbRet[0] = TRUE;

            // Loop through the remainder of the imaginary BA8F parts
            for (; lpbRet[0] && i < 8; i++)
                // Ensure the imaginary BA8F part is a near 0
                lpbRet[0] &= ConvertArbToInt (&lpaplBA8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
        } // End IF
    } // End IF/ELSE
} // TCA_BA8F_VFP


//***************************************************************************
//  $TCA_BA2F_HC2I
//***************************************************************************

void TCA_BA2F_HC2I
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    UBOOL bRet[2];

    // Attempt to convert the parts to an integer
    lpAllTypes->aplHC2I.parts[0] = ConvertArbToInt (&lpaplBA2F[uInt].parts[0], 0, lpAllTypes, &bRet[0]);
    lpAllTypes->aplHC2I.parts[1] = ConvertArbToInt (&lpaplBA2F[uInt].parts[1], 0, lpAllTypes, &bRet[1]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_BA2F_HC2I


//***************************************************************************
//  $TCA_BA4F_HC2I
//***************************************************************************

void TCA_BA4F_HC2I
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        // Attempt to convert the parts to an integer
        lpAllTypes->aplHC2I.parts[i] = ConvertArbToInt (&lpaplBA4F[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
        // Loop through the remainder of the imaginary BA4F parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary BA4F part is a near 0
            lpbRet[0] &= ConvertArbToInt (&lpaplBA4F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_BA4F_HC2I


//***************************************************************************
//  $TCA_BA8F_HC2I
//***************************************************************************

void TCA_BA8F_HC2I
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        // Attempt to convert the parts to an integer
        lpAllTypes->aplHC8I.parts[i] = ConvertArbToInt (&lpaplBA8F[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1];
        // Loop through the remainder of the imaginary BA8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary BA8F part is a near 0
            lpbRet[0] &= ConvertArbToInt (&lpaplBA8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_BA8F_HC2I


//***************************************************************************
//  $TCA_BA8F_HC4I
//***************************************************************************

void TCA_BA8F_HC4I
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[4];

    // Loop through the common parts
    for (i = 0; i < 4; i++)
        // Attempt to convert the parts to an integer
        lpAllTypes->aplHC4I.parts[i] = ConvertArbToInt (&lpaplBA8F[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1]
                 && bRet[2]
                 && bRet[3];
        // Loop through the remainder of the imaginary BA8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary BA8F part is a near 0
            lpbRet[0] &= ConvertArbToInt (&lpaplBA8F[uInt].parts[i], 0, lpAllTypes, &lpbRet[0]) EQ 0;
    } // End IF
} // TCA_BA8F_HC4I


//***************************************************************************
//  $TCA_BA2F_HC4I
//***************************************************************************

void TCA_BA2F_HC4I
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        // Attempt to convert the parts to an integer
        lpAllTypes->aplHC4I.parts[i] = ConvertArbToInt (&lpaplBA2F[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // Loop through the remainder of the imaginary HC4I parts
    for (; i < 4; i++)     // No bRet
        // Set the imaginary parts to 0
        lpAllTypes->aplHC4I.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_BA2F_HC4I


//***************************************************************************
//  $TCA_BA2F_HC8I
//***************************************************************************

void TCA_BA2F_HC8I
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[2];

    // Loop through the common parts
    for (i = 0; i < 2; i++)
        // Attempt to convert the parts to an integer
        lpAllTypes->aplHC8I.parts[i] = ConvertArbToInt (&lpaplBA2F[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // Loop through the remainder of the imaginary HC8I parts
    for (; lpbRet[0] && i < 8; i++)
        // Set the imaginary parts to 0
        lpAllTypes->aplHC8I.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = bRet[0]
                 && bRet[1];
} // TCA_BA2F_HC8I


//***************************************************************************
//  $TCA_BA4F_HC4I
//***************************************************************************

void TCA_BA4F_HC4I
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[4];

    // Loop through the common parts
    for (i = 0; i < 4; i++)
        // Attempt to convert the parts to an integer
        lpAllTypes->aplHC4I.parts[i] = ConvertArbToInt (&lpaplBA4F[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1]
                 && bRet[2]
                 && bRet[3];
} // TCA_BA4F_HC4I


//***************************************************************************
//  $TCA_BA4F_HC8I
//***************************************************************************

void TCA_BA4F_HC8I
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[4];

    // Loop through the common parts
    for (i = 0; i < 4; i++)
        // Attempt to convert the parts to an integer
        lpAllTypes->aplHC8I.parts[i] = ConvertArbToInt (&lpaplBA4F[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // Loop through the remainder of the imaginary HC8I parts
    for (; lpbRet[0] && i < 8; i++)
        // Set the imaginary parts to 0
        lpAllTypes->aplHC8I.parts[i] = 0;

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1]
                 && bRet[2]
                 && bRet[3];
} // TCA_BA4F_HC8I


//***************************************************************************
//  $TCA_BA8F_HC8I
//***************************************************************************

void TCA_BA8F_HC8I
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int   i;
    UBOOL bRet[8];

    // Loop through the common parts
    for (i = 0; i < 8; i++)
        // Attempt to convert the parts to an integer
        lpAllTypes->aplHC8I.parts[i] = ConvertArbToInt (&lpaplBA8F[uInt].parts[i], 0, lpAllTypes, &bRet[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful or not
        lpbRet[0] = bRet[0]
                 && bRet[1]
                 && bRet[2]
                 && bRet[3]
                 && bRet[4]
                 && bRet[5]
                 && bRet[6]
                 && bRet[7];
} // TCA_BA8F_HC8I


//***************************************************************************
//  $TCA_BA2F_HC2F
//***************************************************************************

void TCA_BA2F_HC2F
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA2F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA2F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Attempt to convert the parts to an integer
        lpAllTypes->aplHC2F.parts[0] = arb_get_d (&lpaplBA2F[uInt].parts[0]);
        lpAllTypes->aplHC2F.parts[1] = arb_get_d (&lpaplBA2F[uInt].parts[1]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA2F_HC2F


//***************************************************************************
//  $TCA_BA4F_HC2F
//***************************************************************************

void TCA_BA4F_HC2F
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA4F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 2; i++)
            // Attempt to convert the parts to an integer
            lpAllTypes->aplHC4F.parts[i] = arb_get_d (&lpaplBA4F[uInt].parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
        {
            // Mark as successful
            lpbRet[0] = TRUE;

            // Loop through the remainder of the imaginary BA4F parts
            for (; lpbRet[0] && i < 4; i++)
                // Ensure the imaginary BA4F part is a near 0
                lpbRet[0] &= arb_get_d (&lpaplBA4F[uInt].parts[i]) EQ 0;
        } // End IF
    } // End IF/ELSE
} // TCA_BA4F_HC2F


//***************************************************************************
//  $TCA_BA8F_HC2F
//***************************************************************************

void TCA_BA8F_HC2F
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA8F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[4]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[5]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[6]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[7]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 2; i++)
            // Attempt to convert the parts to an integer
            lpAllTypes->aplHC8F.parts[i] = arb_get_d (&lpaplBA8F[uInt].parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
        {
            // Mark as successful
            lpbRet[0] = TRUE;

            // Loop through the remainder of the imaginary BA2F parts
            for (; lpbRet[0] && i < 8; i++)
                // Ensure the imaginary BA8F part is a near 0
                lpbRet[0] &= arb_get_d (&lpaplBA8F[uInt].parts[i]) EQ 0;
        } // End IF
    } // End IF/ELSE
} // TCA_BA8F_HC2F


//***************************************************************************
//  $TCA_BA8F_HC4F
//***************************************************************************

void TCA_BA8F_HC4F
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA8F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[4]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[5]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[6]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[7]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 4; i++)
            // Attempt to convert the parts to an integer
            lpAllTypes->aplHC4F.parts[i] = arb_get_d (&lpaplBA8F[uInt].parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
        {
            // Mark as successful
            lpbRet[0] = TRUE;

            // Loop through the remainder of the imaginary BA8F parts
            for (; lpbRet[0] && i < 8; i++)
                // Ensure the imaginary BA8F part is a near 0
                lpbRet[0] &= arb_get_d (&lpaplBA8F[uInt].parts[i]) EQ 0;
        } // End IF
    } // End IF/ELSE
} // TCA_BA8F_HC4F


//***************************************************************************
//  $TCA_BA2F_HC4F
//***************************************************************************

void TCA_BA2F_HC4F
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA2F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA2F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 2; i++)
            // Attempt to convert the parts to an integer
            lpAllTypes->aplHC4F.parts[i] = arb_get_d (&lpaplBA2F[uInt].parts[i]);

        // Loop through the remainder of the imaginary HC4F parts
        for (; lpbRet[0] && i < 4; i++)
            // Set the imaginary parts to 0
            lpAllTypes->aplHC4F.parts[i] = 0;

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA2F_HC4F


//***************************************************************************
//  $TCA_BA2F_HC8F
//***************************************************************************

void TCA_BA2F_HC8F
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA2F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA2F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 2; i++)
            // Attempt to convert the parts to an integer
            lpAllTypes->aplHC8F.parts[i] = arb_get_d (&lpaplBA2F[uInt].parts[i]);

        // Loop through the remainder of the imaginary HC8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Set the imaginary parts to 0
            lpAllTypes->aplHC8F.parts[i] = 0;

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA2F_HC8F


//***************************************************************************
//  $TCA_BA4F_HC4F
//***************************************************************************

void TCA_BA4F_HC4F
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA4F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 4; i++)
            // Attempt to convert the parts to an integer
            lpAllTypes->aplHC4F.parts[i] = arb_get_d (&lpaplBA4F[uInt].parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA4F_HC4F


//***************************************************************************
//  $TCA_BA4F_HC8F
//***************************************************************************

void TCA_BA4F_HC8F
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA4F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 4; i++)
            // Attempt to convert the parts to an integer
            lpAllTypes->aplHC8F.parts[i] = arb_get_d (&lpaplBA4F[uInt].parts[i]);

        // Loop through the remainder of the imaginary HC8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Set the imaginary parts to 0
            lpAllTypes->aplHC8F.parts[i] = 0;

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA4F_HC8F


//***************************************************************************
//  $TCA_BA8F_HC8F
//***************************************************************************

void TCA_BA8F_HC8F
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA8F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[4]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[5]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[6]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[7]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 8; i++)
            // Attempt to convert the parts to an integer
            lpAllTypes->aplHC8F.parts[i] = arb_get_d (&lpaplBA8F[uInt].parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA8F_HC8F


//***************************************************************************
//  $TCA_BA2F_HC2R
//***************************************************************************

void TCA_BA2F_HC2R
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA2F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA2F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 2; i++)
            // Attempt to convert the parts to RAT
            lpAllTypes->aplHC2R.parts[i] = arb_get_q (&lpaplBA2F[uInt].parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA2F_HC2R


//***************************************************************************
//  $TCA_BA2F_HC4R
//***************************************************************************

void TCA_BA2F_HC4R
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA2F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA2F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 2; i++)
            // Attempt to convert the parts to RAT
            lpAllTypes->aplHC4R.parts[i] = arb_get_q (&lpaplBA2F[uInt].parts[i]);

        // Loop through the remainder of the imaginary HC4R parts
        for (; lpbRet[0] && i < 4; i++)
            // Set the imaginary parts to 0/1
            mpq_init (&lpAllTypes->aplHC4R.parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA2F_HC4R


//***************************************************************************
//  $TCA_BA2F_HC8R
//***************************************************************************

void TCA_BA2F_HC8R
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA2F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA2F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 2; i++)
            // Attempt to convert the parts to RAT
            lpAllTypes->aplHC8R.parts[i] = arb_get_q (&lpaplBA2F[uInt].parts[i]);

        // Loop through the remainder of the imaginary HC8R parts
        for (; lpbRet[0] && i < 8; i++)
            // Set the imaginary parts to 0/1
            mpq_init (&lpAllTypes->aplHC8R.parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA2F_HC8R


//***************************************************************************
//  $TCA_BA2F_HC2V
//***************************************************************************

void TCA_BA2F_HC2V
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA2F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA2F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Attempt to convert the parts to VFP
        lpAllTypes->aplHC2V.parts[0] = arb_get_mpfr (&lpaplBA2F[uInt].parts[0]);
        lpAllTypes->aplHC2V.parts[1] = arb_get_mpfr (&lpaplBA2F[uInt].parts[1]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA2F_HC2V


//***************************************************************************
//  $TCA_BA2F_HC4V
//***************************************************************************

void TCA_BA2F_HC4V
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA2F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA2F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 2; i++)
            // Attempt to convert the parts to VFP
            lpAllTypes->aplHC4V.parts[i] = arb_get_mpfr (&lpaplBA2F[uInt].parts[i]);

        // Loop through the remainder of the imaginary HC4V parts
        for (; lpbRet[0] && i < 4; i++)
            // Set the imaginary parts to 0
            mpfr_init0 (&lpAllTypes->aplHC4V.parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA2F_HC4V


//***************************************************************************
//  $TCA_BA2F_HC8V
//***************************************************************************

void TCA_BA2F_HC8V
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA2F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA2F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 2; i++)
            // Attempt to convert the parts to RAT
            lpAllTypes->aplHC8V.parts[i] = arb_get_mpfr (&lpaplBA2F[uInt].parts[i]);

        // Loop through the remainder of the imaginary HC8V parts
        for (; lpbRet[0] && i < 8; i++)
            // Set the imaginary parts to 0
            mpfr_init0 (&lpAllTypes->aplHC8V.parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA2F_HC8V


//***************************************************************************
//  $TCA_BA4F_HC2R
//***************************************************************************

void TCA_BA4F_HC2R
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA4F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 2; i++)
            // Attempt to convert the parts to RAT
            lpAllTypes->aplHC2R.parts[i] = arb_get_q (&lpaplBA4F[uInt].parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
        {
            // Mark as successful
            lpbRet[0] = TRUE;

            // Loop through the remainder of the imaginary BA4F parts
            for (; lpbRet[0] && i < 4; i++)
                // Ensure the imaginary BA4F part is 0
                lpbRet[0] &= IsZeroBAxF (&lpaplBA4F[uInt].parts[i], 1);
        } // End IF
    } // End IF/ELSE
} // TCA_BA4F_HC2R


//***************************************************************************
//  $TCA_BA4F_HC4R
//***************************************************************************

void TCA_BA4F_HC4R
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA4F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 4; i++)
            // Attempt to convert the parts to RAT
            lpAllTypes->aplHC4R.parts[i] = arb_get_q (&lpaplBA4F[uInt].parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA4F_HC4R


//***************************************************************************
//  $TCA_BA4F_HC8R
//***************************************************************************

void TCA_BA4F_HC8R
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA4F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 4; i++)
            // Attempt to convert the parts to RAT
            lpAllTypes->aplHC8R.parts[i] = arb_get_q (&lpaplBA4F[uInt].parts[i]);

        // Loop through the remainder of the imaginary BA8F parts
        for (; i < 8; i++)          // No bRet
            // Set the imaginary parts to 0/1
            mpq_init (&lpAllTypes->aplHC8R.parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA4F_HC8R


//***************************************************************************
//  $TCA_BA4F_HC2V
//***************************************************************************

void TCA_BA4F_HC2V
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA4F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 2; i++)     // No bRet
            // Attempt to convert the parts to RAT
            lpAllTypes->aplHC2V.parts[i] = arb_get_mpfr (&lpaplBA4F[uInt].parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
        {
            // Mark as successful
            lpbRet[0] = TRUE;

            // Loop through the remainder of the imaginary BA4F parts
            for (; lpbRet[0] && i < 4; i++)
                // Ensure the imaginary BA4F part is 0
                lpbRet[0] &= IsZeroBAxF (&lpaplBA4F[uInt].parts[i], 1);
        } // End IF
    } // End IF/ELSE
} // TCA_BA4F_HC2V


//***************************************************************************
//  $TCA_BA4F_HC4V
//***************************************************************************

void TCA_BA4F_HC4V
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA4F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 4; i++)     // No bRet
            // Attempt to convert the parts to VFP
            lpAllTypes->aplHC4V.parts[i] = arb_get_mpfr (&lpaplBA4F[uInt].parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA4F_HC4V


//***************************************************************************
//  $TCA_BA4F_HC8V
//***************************************************************************

void TCA_BA4F_HC8V
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA4F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA4F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 4; i++)     // No bRet
            // Attempt to convert the parts to VFP
            lpAllTypes->aplHC8V.parts[i] = arb_get_mpfr (&lpaplBA4F[uInt].parts[i]);

        // Loop through the remainder of the imaginary HC8V parts
        for (; lpbRet[0] && i < 8; i++)
            // Set the imaginary parts to 0
            mpfr_init0 (&lpAllTypes->aplHC8V.parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA4F_HC8V


//***************************************************************************
//  $TCA_BA8F_HC2R
//***************************************************************************

void TCA_BA8F_HC2R
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA8F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[4]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[5]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[6]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[7]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 2; i++)     // No bRet
            // Attempt to convert the parts to RAT
            lpAllTypes->aplHC2R.parts[i] = arb_get_q (&lpaplBA8F[uInt].parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
        {
            // Mark as successful
            lpbRet[0] = TRUE;

            // Loop through the remainder of the imaginary BA8F parts
            for (; lpbRet[0] && i < 8; i++)
                // Ensure the imaginary BA8F part is 0
                lpbRet[0] &= IsZeroBAxF (&lpaplBA8F[uInt].parts[i], 1);
        } // End IF
    } // End IF/ELSE
} // TCA_BA8F_HC2R


//***************************************************************************
//  $TCA_BA8F_HC4R
//***************************************************************************

void TCA_BA8F_HC4R
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA8F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[4]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[5]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[6]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[7]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 4; i++)     // No bRet
            // Attempt to convert the parts to RAT
            lpAllTypes->aplHC4R.parts[i] = arb_get_q (&lpaplBA8F[uInt].parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
        {
            // Mark as successful
            lpbRet[0] = TRUE;

            // Loop through the remainder of the imaginary BA8F parts
            for (; lpbRet[0] && i < 8; i++)
                // Ensure the imaginary BA8F part is 0
                lpbRet[0] &= IsZeroBAxF (&lpaplBA8F[uInt].parts[i], 1);
        } // End IF
    } // End IF/ELSE
} // TCA_BA8F_HC4R


//***************************************************************************
//  $TCA_BA8F_HC8R
//***************************************************************************

void TCA_BA8F_HC8R
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA8F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[4]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[5]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[6]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[7]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 8; i++)     // No bRet
            // Attempt to convert the parts to RAT
            lpAllTypes->aplHC8R.parts[i] = arb_get_q (&lpaplBA8F[uInt].parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA8F_HC8R


//***************************************************************************
//  $TCA_BA8F_HC2V
//***************************************************************************

void TCA_BA8F_HC2V
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA8F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[4]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[5]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[6]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[7]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 2; i++)     // No bRet
            // Attempt to convert the parts to RAT
            lpAllTypes->aplHC2V.parts[i] = arb_get_mpfr (&lpaplBA8F[uInt].parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
        {
            // Mark as successful
            lpbRet[0] = TRUE;

            // Loop through the remainder of the imaginary BA8F parts
            for (; lpbRet[0] && i < 8; i++)
                // Ensure the imaginary BA8F part is 0
                lpbRet[0] &= IsZeroBAxF (&lpaplBA8F[uInt].parts[i], 1);
        } // End IF
    } // End IF/ELSE
} // TCA_BA8F_HC2V


//***************************************************************************
//  $TCA_BA8F_HC4V
//***************************************************************************

void TCA_BA8F_HC4V
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA8F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[4]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[5]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[6]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[7]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 4; i++)     // No bRet
            // Attempt to convert the parts to RAT
            lpAllTypes->aplHC4V.parts[i] = arb_get_mpfr (&lpaplBA8F[uInt].parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
        {
            // Mark as successful
            lpbRet[0] = TRUE;

            // Loop through the remainder of the imaginary BA8F parts
            for (; lpbRet[0] && i < 8; i++)
                // Ensure the imaginary BA8F part is 0
                lpbRet[0] &= IsZeroBAxF (&lpaplBA8F[uInt].parts[i], 1);
        } // End IF
    } // End IF/ELSE
} // TCA_BA8F_HC4V


//***************************************************************************
//  $TCA_BA8F_HC8V
//***************************************************************************

void TCA_BA8F_HC8V
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // If the Radius is non-zero, ...
    if (mag_equal (arb_radref (&lpaplBA8F[uInt].parts[0]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[1]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[2]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[3]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[4]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[5]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[6]), arb_radref (&arbZero)) EQ 0
     || mag_equal (arb_radref (&lpaplBA8F[uInt].parts[7]), arb_radref (&arbZero)) EQ 0)
    {
        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as failed
            *lpbRet = FALSE;
    } else
    {
        // Loop through the common parts
        for (i = 0; i < 8; i++)     // No bRet
            // Attempt to convert the parts to VFP
            lpAllTypes->aplHC8V.parts[i] = arb_get_mpfr (&lpaplBA8F[uInt].parts[i]);

        // If the flag ptr is valid, ...
        if (lpbRet NE NULL)
            // Mark as successful
            lpbRet[0] = TRUE;
    } // End IF/ELSE
} // TCA_BA8F_HC8V


//***************************************************************************
//  $TCA_BA1F_BA1F
//***************************************************************************

void TCA_BA1F_BA1F
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the parts to ARB
    arb_set (&lpAllTypes->aplArb, &lpaplBA1F[uInt]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BA1F_BA1F


//***************************************************************************
//  $TCA_BA1F_BA2F
//***************************************************************************

void TCA_BA1F_BA2F
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Initialize the result
    arb2f_init (&lpAllTypes->aplBA2F);

    // Attempt to convert the parts to ARB
    arb_set (&lpAllTypes->aplBA2F.parts[0], &lpaplBA1F[uInt]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BA1F_BA2F


//***************************************************************************
//  $TCA_BA1F_BA4F
//***************************************************************************

void TCA_BA1F_BA4F
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Initialize the result
    arb4f_init (&lpAllTypes->aplBA4F);

    // Attempt to convert the parts to ARB
    arb_set (&lpAllTypes->aplBA4F.parts[0], &lpaplBA1F[uInt]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BA1F_BA4F


//***************************************************************************
//  $TCA_BA1F_BA8F
//***************************************************************************

void TCA_BA1F_BA8F
    (LPAPLBA1F   lpaplBA1F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Initialize the result
    arb8f_init (&lpAllTypes->aplBA8F);

    // Attempt to convert the parts to ARB
    arb_set (&lpAllTypes->aplBA8F.parts[0], &lpaplBA1F[uInt]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BA1F_BA8F


//***************************************************************************
//  $TCA_BA2F_BA1F
//***************************************************************************

void TCA_BA2F_BA1F
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Attempt to convert the parts to ARB
    arb_set (&lpAllTypes->aplArb, &lpaplBA2F[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary BA2F parts
        for (; lpbRet[0] && i < 2; i++)
            // Ensure the imaginary BA2F part is 0
            lpbRet[0] &= IsZeroBAxF (&lpaplBA2F[uInt].parts[i], 1);
    } // End IF
} // TCA_BA2F_BA1F


//***************************************************************************
//  $TCA_BA2F_BA2F
//***************************************************************************

void TCA_BA2F_BA2F
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb2f_init (&lpAllTypes->aplBA2F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert the parts to ARB
        arb_set (&lpAllTypes->aplBA2F.parts[i], &lpaplBA2F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BA2F_BA2F


//***************************************************************************
//  $TCA_BA2F_BA4F
//***************************************************************************

void TCA_BA2F_BA4F
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert the parts to ARB
        arb_set (&lpAllTypes->aplBA4F.parts[i], &lpaplBA2F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BA2F_BA4F


//***************************************************************************
//  $TCA_BA2F_BA8F
//***************************************************************************

void TCA_BA2F_BA8F
    (LPAPLBA2F   lpaplBA2F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert the parts to ARB
        arb_set (&lpAllTypes->aplBA8F.parts[i], &lpaplBA2F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BA2F_BA8F


//***************************************************************************
//  $TCA_BA4F_BA1F
//***************************************************************************

void TCA_BA4F_BA1F
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Initialize the result
    Myarb_init (&lpAllTypes->aplArb);

    // Attempt to convert the parts to ARB
    arb_set (&lpAllTypes->aplArb, &lpaplBA4F[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary BA4F parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary BA4F part is 0
            lpbRet[0] &= IsZeroBAxF (&lpaplBA4F[uInt].parts[i], 1);
    } // End IF
} // TCA_BA4F_BA1F


//***************************************************************************
//  $TCA_BA4F_BA2F
//***************************************************************************

void TCA_BA4F_BA2F
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb2f_init (&lpAllTypes->aplBA2F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert the parts to ARB
        arb_set (&lpAllTypes->aplBA2F.parts[i], &lpaplBA4F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary BA4F parts
        for (; lpbRet[0] && i < 4; i++)
            // Ensure the imaginary BA4F part is 0
            lpbRet[0] &= IsZeroBAxF (&lpaplBA4F[uInt].parts[i], 1);
    } // End IF
} // TCA_BA4F_BA2F


//***************************************************************************
//  $TCA_BA4F_BA4F
//***************************************************************************

void TCA_BA4F_BA4F
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Attempt to convert the parts to ARB
        arb_set (&lpAllTypes->aplBA4F.parts[i], &lpaplBA4F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BA4F_BA4F


//***************************************************************************
//  $TCA_BA4F_BA8F
//***************************************************************************

void TCA_BA4F_BA8F
    (LPAPLBA4F   lpaplBA4F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Attempt to convert the parts to ARB
        arb_set (&lpAllTypes->aplBA8F.parts[i], &lpaplBA4F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BA4F_BA8F


//***************************************************************************
//  $TCA_BA8F_BA1F
//***************************************************************************

void TCA_BA8F_BA1F
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    // Initialize the result
    Myarb_init (&lpAllTypes->aplArb);

    // Attempt to convert the parts to ARB
    arb_set (&lpAllTypes->aplArb, &lpaplBA8F[uInt].parts[0]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        int i = 1;

        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary BA8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary BA8F part is 0
            lpbRet[0] &= IsZeroBAxF (&lpaplBA8F[uInt].parts[i], 1);
    } // End IF
} // TCA_BA8F_BA1F


//***************************************************************************
//  $TCA_BA8F_BA2F
//***************************************************************************

void TCA_BA8F_BA2F
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb2f_init (&lpAllTypes->aplBA2F);

    // Loop through the common parts
    for (i = 0; i < 2; i++)     // No bRet
        // Attempt to convert the parts to ARB
        arb_set (&lpAllTypes->aplBA2F.parts[i], &lpaplBA8F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary BA8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary BA8F part is 0
            lpbRet[0] &= IsZeroBAxF (&lpaplBA8F[uInt].parts[i], 1);
    } // End IF
} // TCA_BA8F_BA2F


//***************************************************************************
//  $TCA_BA8F_BA4F
//***************************************************************************

void TCA_BA8F_BA4F
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb4f_init (&lpAllTypes->aplBA4F);

    // Loop through the common parts
    for (i = 0; i < 4; i++)     // No bRet
        // Attempt to convert the parts to ARB
        arb_set (&lpAllTypes->aplBA4F.parts[i], &lpaplBA8F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
    {
        // Mark as successful
        lpbRet[0] = TRUE;

        // Loop through the remainder of the imaginary BA8F parts
        for (; lpbRet[0] && i < 8; i++)
            // Ensure the imaginary BA8F part is 0
            lpbRet[0] &= IsZeroBAxF (&lpaplBA8F[uInt].parts[i], 1);
    } // End IF
} // TCA_BA8F_BA4F


//***************************************************************************
//  $TCA_BA8F_BA8F
//***************************************************************************

void TCA_BA8F_BA8F
    (LPAPLBA8F   lpaplBA8F,
     APLINT      uInt,
     LPALLTYPES  lpAllTypes,
     LPUBOOL     lpbRet)

{
    int i;

    // Initialize the result
    arb8f_init (&lpAllTypes->aplBA8F);

    // Loop through the common parts
    for (i = 0; i < 8; i++)     // No bRet
        // Attempt to convert the parts to ARB
        arb_set (&lpAllTypes->aplBA8F.parts[i], &lpaplBA8F[uInt].parts[i]);

    // If the flag ptr is valid, ...
    if (lpbRet NE NULL)
        // Mark as successful
        lpbRet[0] = TRUE;
} // TCA_BA8F_BA8F


//***************************************************************************
//  $AllocateDemote
//
//  Allocate a new array and demote the data from the old to the new
//***************************************************************************

HGLOBAL AllocateDemote
    (APLSTYPE   aplTypeRes,                 // Result storage type
     HGLOBAL    hGlbArg,                    // Arg global memory handle (may be NULL iff lpatArg is not NULL)
     LPALLTYPES lpatArg,                    // Ptr to ALLTYPES arg values (may be NULL iff hGlbArg is not NULL)
     APLSTYPE   aplTypeArg,                 // Arg storage type
     APLNELM    aplNELMArg,                 // ... NELM
     APLRANK    aplRankArg,                 // ... rank
     LPUBOOL    lpbRet)                     // Ptr to TRUE iff the result is not demotable

{
    HGLOBAL           hGlbRes = NULL;       // Result global memory handle
    LPVARARRAY_HEADER lpMemHdrRes = NULL,   // Ptr to arg header
                      lpMemHdrArg = NULL;   // Ptr to arg header

    // Assume no error
    lpbRet[0] = TRUE;

    // If the arg is HC, ...
    if (IsHCAny (aplTypeArg))
    {
        // Lock the memory to get a ptr to it
        lpMemHdrArg = MyGlobalLockVar (hGlbArg);

        // Allocate space for the new left arg
        hGlbRes = AllocateGlobalArray (aplTypeRes, aplNELMArg, aplRankArg, VarArrayBaseToDim (lpMemHdrArg));

        // Check for error
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Demote the data in the left arg, copying it to the new left
        if (!DemoteData (aplTypeRes,                            // Result storage type
                         VarArrayDataFmBase (lpMemHdrRes),      // Ptr to result global memory
                         aplTypeArg,                            // Arg storage type
                         aplNELMArg,                            // Arg NELM
                         VarArrayDataFmBase (lpMemHdrArg),      // tPr to arg global memory
                         FALSE))                                // TRUE iff dimension demotion allowed
        {
            // Mark as not demotable
            lpbRet[0] = FALSE;

            goto DOMAIN_EXIT;
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
     } else
        // Return the arg handle, copied once
        hGlbRes = CopySymGlbDirAsGlb (hGlbArg);

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
WSFULL_EXIT:
NORMAL_EXIT:
    return hGlbRes;
} // End AllocateDemote


//***************************************************************************
//  $SimpleDemote
//
//  Attempt to demote to a simple integer or float type
//***************************************************************************

UBOOL SimpleDemote
    (HGLOBAL  hGlbArg,              // Arg global memory handle
     HGLOBAL *lphGlbArg,            // Ptr to outgoing global memory handle
     LPUBOOL  lpbRet)               // Ptr to TRUE iff the result is not demotable (may be NULL)

{
    APLSTYPE aplTypeArg;            // Arg storage type
    APLNELM  aplNELMArg;            // ... NELM
    APLRANK  aplRankArg;            // ... rank

    // Assume we fail
    *lphGlbArg = NULL;
    if (lpbRet NE NULL)
        lpbRet[0] = FALSE;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfGlb (hGlbArg, &aplTypeArg, &aplNELMArg, &aplRankArg, NULL);

    // Check for DOMAIN ERROR or type demotion
    if (!(IsRealBIA (aplTypeArg)
       || IsRealFlt (aplTypeArg)))
    {
        APLSTYPE aplTypeArg2;           // Base storage type
        UBOOL    bRet;                  // TRUE iff the arg is demotable

        // Calculate the right arg base storage type
        aplTypeArg2 = aToSimple[aplTypeArg];

        // If the demoted type is not a simple integer or float, ...
        if (!(IsRealBIA (aplTypeArg2)
           || IsRealFlt (aplTypeArg2)))
            return FALSE;

        // Convert tiny FLTs to zero
        ConvertTinyFlt2Zero (hGlbArg);

        // Allocate new and Demote the arg
        *lphGlbArg =
          AllocateDemote (aplTypeArg2,      // Base storage type
                          hGlbArg,          // Global memory handle (may be NULL
                          NULL,             // Ptr to ALLTYPES values (may be NULL)
                          aplTypeArg,       // Arg storage type
                          aplNELMArg,       // ... NELM
                          aplRankArg,       // ... rank
                         &bRet);            // Ptr to TRUE iff the result is not a DOMAIN ERROR
        // Set result if the caller asked
        if (lpbRet NE NULL)
            lpbRet[0] = bRet;

        // Check for error
        if (!bRet)
        {
            Assert (*lphGlbArg EQ NULL);

            return FALSE;
        } // End IF

        return (*lphGlbArg NE NULL);
    } else
        return TRUE;
} // End SimpleDemote


//***************************************************************************
//  $ConvertTinyFlt2Zero
//
//  Convert tiny FLTs to zero
//***************************************************************************

void ConvertTinyFlt2Zero
    (HGLOBAL hGlbArg)

{
    APLSTYPE          aplTypeArg,           // Arg storage type
                      aplTypeBase;          // Base ...
    APLNELM           aplNELMArg;           // ... NELM
    APLNELM           iCnt,                 // Loop counter
                      i,                    // ...
                      iHCDimArg;            // HC Dimension (1, 2, 4, 8)
    LPVARARRAY_HEADER lpMemHdrArg = NULL;   // Ptr to arg header
    LPAPLFLOAT        lpMemArg;             // Ptr to global memory data

    Assert (hGlbArg NE NULL);

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfGlb (hGlbArg, &aplTypeArg, &aplNELMArg, NULL, NULL);

    // Calculate the base result type (IFRV)
    aplTypeBase = aToSimple[aplTypeArg];

    // If it's not a FLT, ...
    if (!IsRealFlt (aplTypeBase))
        return;

    // Calculate the arg HC dimension (1, 2, 4, 8)
    iHCDimArg = TranslateArrayTypeToHCDim (aplTypeArg);

    // Lock the memory to get a ptr to it
    lpMemHdrArg = MyGlobalLockVar (hGlbArg);

    // Point to the data
    lpMemArg = VarArrayDataFmBase (lpMemHdrArg);

    // Loop through the elements
    for (iCnt = 0; iCnt < aplNELMArg; iCnt++)
    // Loop through all of the parts
    for (i = 0; i < iHCDimArg; i++)
    // If the part is tiny, ...
    if (fabs (lpMemArg[i + iCnt * iHCDimArg]) < SYS_CT)
              // Set to zero
              lpMemArg[i + iCnt * iHCDimArg] = 0.0;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;
} // End ConvertTinyFlt2Zero


//***************************************************************************
//  $DemoteMemToHCxF
//
//  Demotet the memory pointed to into a lower dimensional HCxF array
//
//  Note that the result is a raw global without an APL array header.
//***************************************************************************

HGLOBAL DemoteMemToHCxF
    (APLSTYPE   aplTypeRht,     // Right arg storage type
     APLNELM    aplNELMRht,     // ...       NELM
     LPAPLFLOAT lpMemRht,       // Ptr to right arg global memory data
     APLSTYPE   aplTypeRes)     // Result storage type
{
    int      iSizeofRes;        // The size of an element in the result
    HGLOBAL  hGlbRes;           // The result global memory handle
    LPVOID   lpMemRes;          // Ptr to result global memory data
    ALLTYPES atRes = {0};       // Result arg as ALLTYPES
    APLNELM  uRow;              // Loop counter
    UBOOL    bRet;              // TRUE iff the result is valid

    // Translate the result array type to sizeof
    iSizeofRes = TranslateArrayTypeToSizeof (aplTypeRes);

    // Allocate the result array to hold the higher-dimensiona FLTS when converted to lower-dimensional FLTs
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) (aplNELMRht * iSizeofRes));
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock000 (hGlbRes);

    // Loop through the rows and cols
    for (uRow = 0; uRow < aplNELMRht; uRow++)
    {
        // Convert from HCxI/HCxF to HCxF
        (*aTypeActConvert[aplTypeRht][aplTypeRes]) (lpMemRht, uRow, &atRes, &bRet);

        // If it failed, ...
        if (!bRet)
            goto DOMAIN_EXIT;

        // Copy over to the result
        CopyMemory (&((LPBYTE) lpMemRes)[uRow * iSizeofRes], &atRes, (APLU3264) iSizeofRes);

////////// Zero the memory in case we might free it
////////ZeroMemory (&atRes, sizeof (atRes));
    } // End FOR

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    return hGlbRes;

DOMAIN_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // We no longer need this storage
    MyGlobalFree (hGlbRes); hGlbRes = NULL;

    // Fall through to common error exit

WSFULL_EXIT:
    return NULL;
} // End DemoteMemToHCxF


//***************************************************************************
//  End of File: typemote.c
//***************************************************************************
