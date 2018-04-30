//***************************************************************************
//  NARS2000 -- Primitive Function -- NotEqual
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
#include "headers.h"


#ifndef PROTO
PRIMSPEC PrimSpecNotEqual = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecNotEqualStorageTypeMon, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualAPA_EM, -- No monadic defn, can't happen w/NotEqual

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonNotEqualBisB, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualBisI, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualBisF, -- No monadic defn, can't happen w/NotEqual

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonNotEqualIisI, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualIisF, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualFisI, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualFisF, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualRisR, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualVisR, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualVisV, -- No monadic defn, can't happen w/NotEqual

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonNotEqualHC2IisHC2I, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC2IisHC2F, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC2FisHC2I, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC2FisHC2F, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC2RisHC2R, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC2VisHC2R, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC2VisHC2V, -- No monadic defn, can't happen w/NotEqual

    NULL,   // &PrimFnMonNotEqualHC4IisHC4I, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC4IisHC4F, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC4FisHC4I, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC4FisHC4F, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC4RisHC4R, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC4VisHC4R, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC4VisHC4V, -- No monadic defn, can't happen w/NotEqual

    NULL,   // &PrimFnMonNotEqualHC8IisHC8I, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC8IisHC8F, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC8FisHC8I, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC8FisHC8F, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC8RisHC8R, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC8VisHC8R, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualHC8VisHC8V, -- No monadic defn, can't happen w/NotEqual

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonNotEqualFisHC2I, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualFisHC2F, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualVisHC2R, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualVisHC2V, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualFisHC4I, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualFisHC4F, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualVisHC4R, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualVisHC4V, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualFisHC8I, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualFisHC8F, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualVisHC8R, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualVisHC8V, -- Can't happen w/NotEqual

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecNotEqualStorageTypeDyd,
    NULL,   // &PrimFnDydNotEqualAPA_EM, -- Result Boolean, can't happen w/NotEqual

    // Dyadic Boolean result functions
    &PrimFnDydNotEqualBisBvB,
    &PrimFnDydNotEqualBisIvI,
    &PrimFnDydNotEqualBisFvF,
    &PrimFnDydNotEqualBisCvC,
    &PrimFnDydNotEqualBisHvH,
    &PrimFnDydNotEqualBisRvR,
    &PrimFnDydNotEqualBisVvV,

    &PrimFnDydNotEqualBisHC2IvHC2I,
    &PrimFnDydNotEqualBisHC2FvHC2F,
    &PrimFnDydNotEqualBisHC2RvHC2R,
    &PrimFnDydNotEqualBisHC2VvHC2V,

    &PrimFnDydNotEqualBisHC4IvHC4I,
    &PrimFnDydNotEqualBisHC4FvHC4F,
    &PrimFnDydNotEqualBisHC4RvHC4R,
    &PrimFnDydNotEqualBisHC4VvHC4V,

    &PrimFnDydNotEqualBisHC8IvHC8I,
    &PrimFnDydNotEqualBisHC8FvHC8F,
    &PrimFnDydNotEqualBisHC8RvHC8R,
    &PrimFnDydNotEqualBisHC8VvHC8V,

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydNotEqualIisIvI, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualIisFvF, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualFisIvI, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualFisFvF, -- Result Boolean, cCan't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualRisRvR, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualVisRvR, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualVisVvV, -- Result Boolean, can't happen w/NotEqual

    NULL,   // &PrimFnDydNotEqualHC2IisHC2IvHC2I, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC2IisHC2FvHC2F, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC2FisHC2IvHC2I, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC2FisHC2FvHC2F, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC2RisHC2RvHC2R, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC2VisHC2RvHC2R, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC2VisHC2VvHC2V, -- Result Boolean, can't happen w/NotEqual

    NULL,   // &PrimFnDydNotEqualHC4IisHC4IvHC4I, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC4IisHC4FvHC4F, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC4FisHC4IvHC4I, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC4FisHC4FvHC4F, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC4RisHC4RvHC4R, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC4VisHC4RvHC4R, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC4VisHC4VvHC4V, -- Result Boolean, can't happen w/NotEqual

    NULL,   // &PrimFnDydNotEqualHC8IisHC8IvHC8I, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC8IisHC8FvHC8F, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC8FisHC8IvHC8I, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC8FisHC8FvHC8F, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC8RisHC8RvHC8R, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC8VisHC8RvHC8R, -- Result Boolean, can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualHC8VisHC8VvHC8V, -- Result Boolean, can't happen w/NotEqual

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonNotEqualB64isB64, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualB32isB32, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualB16isB16, -- No monadic defn, can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualB08isB08, -- No monadic defn, can't happen w/NotEqual

    // Dyadic Boolean chunk functions
    &PrimFnDydNotEqualB64isB64vB64,
    &PrimFnDydNotEqualB32isB32vB32,
    &PrimFnDydNotEqualB16isB16vB16,
    &PrimFnDydNotEqualB08isB08vB08,

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecNotEqual};
#endif


//***************************************************************************
//  $PrimFnNotEqual_EM_YY
//
//  Primitive function for monadic and dyadic NotEqual (ERROR and "NotEqual")
//***************************************************************************

LPPL_YYSTYPE PrimFnNotEqual_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonNotEqual_EM_YY                    (lptkFunc, lptkRhtArg, lptkAxis);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnNotEqual_EM_YY


//***************************************************************************
//  $PrimFnMonNotEqual_EM_YY
//
//  Primitive function for monadic Equal ("norm")
//***************************************************************************

LPPL_YYSTYPE PrimFnMonNotEqual_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeRes;           // Result    ...
    APLNELM           aplNELMRht;           // ...       NELM
    APLRANK           aplRankRht;           // ...       rank
    APLUINT           ByteRes,              // # bytes in the result
                      uCnt;                 // Loop counter
    ALLTYPES          atRes = {0},          // Result as ALLTYPES
                      atItm = {0},          // Item   ...
                      atMul = {0};          // Multiplier ...
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL;       // Result    ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPVOID            lpMemRht,             // Ptr to right arg global memory
                      lpMemRes;             // ...    result    ...
    LPAPLDIM          lpMemDimRht,          // Ptr to right arg dimensions
                      lpMemDimRes;          // ...    result    ...
    APLLONGEST        aplLongestRht;        // Right arg as immediate
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to result
    int               iHCDimRht,            // Right arg dimension (1, 2, 4, 8)
                      i;                    // Loop counter

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get the global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the right arg is 1-dimensional, ...
    if (TranslateArrayTypeToHCDim (aplTypeRht) EQ 1)
    {
        // If the right arg is immediate, ...
        if (hGlbRht EQ NULL)
            goto YYALLOC_IMM_EXIT;

        // Pass through the right arg
        hGlbRes = CopySymGlbDirAsGlb (hGlbRht);

        goto YYALLOC_GLB_EXIT;
    } // End IF

    // Set the result type to simple IFRV (INT, FLT, RAT, VFP)
    aplTypeRes = aToSimple[aplTypeRht];

    // Get the HC dimension (1, 2, 4, 8)
    iHCDimRht = TranslateArrayTypeToHCDim (aplTypeRht);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result.
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

#define lpHeader        lpMemHdrRes
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Skip over the header to the dimension
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // If the right arg is immediate, ...
    if (hGlbRht EQ NULL)
        // Point to the data
        lpMemRht = &aplLongestRht;
    else
    {
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim  (lpMemHdrRht);

        // Skip over the header and dimensions to the data
        lpMemRht    = VarArrayDataFmBase (lpMemHdrRht);

        // Copy the dimensions
        CopyMemory (lpMemDimRes, lpMemDimRht, (APLU3264) aplRankRht);
    } // End IF/ELSE

    // Skip over the header and dimensions
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Split cases based upon the rightarg storage type
    switch (aplTypeRht)
    {
////////case ARRAY_BOOL:
////////case ARRAY_INT:
////////case ARRAY_FLOAT:
////////case ARRAY_APA:
////////    break;

        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
            // Loop through the items
            for (uCnt = 0; uCnt < aplNELMRht; uCnt++)
            {
                // Initialize the accumulator
                atRes.aplInteger = 0;

                // Looop through all of the parts
                for (i = 0; i < iHCDimRht; i++)
                {
                    // Get the next item
                    atItm.aplInteger = *((LPAPLINT) lpMemRht)++;

                    // Square it and accumulate
                    atRes.aplInteger += atItm.aplInteger * atItm.aplInteger;
                } // End FOR

                // Save in the result
                *((LPAPLINT) lpMemRes)++ = atRes.aplInteger;
            } // End FOR

            break;

        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
            // Loop through the items
            for (uCnt = 0; uCnt < aplNELMRht; uCnt++)
            {
                // Initialize the accumulator
                atRes.aplFloat = 0;         // Already zero from = {0}

                // Loop through all of the parts
                for (i = 0; i < iHCDimRht; i++)
                {
                    // Get the next item
                    atItm.aplFloat = *((LPAPLFLOAT) lpMemRht)++;

                    // Square it and accumulate
                    atRes.aplFloat += atItm.aplFloat * atItm.aplFloat;
                } // End FOR

                // Save in the result
                *((LPAPLFLOAT) lpMemRes)++ = atRes.aplFloat;
            } // End FOR

            break;

        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            // Initialize to 0/1
            mpq_init (&atRes.aplRat);
            mpq_init (&atItm.aplRat);
            mpq_init (&atMul.aplRat);

            // Loop through the items
            for (uCnt = 0; uCnt < aplNELMRht; uCnt++)
            {
                // Initialize the accumulator
                mpq_set_si (&atRes.aplRat, 0, 1);

                // Looop through all of the parts
                for (i = 0; i < iHCDimRht; i++)
                {
                    // Get the next item
                    mpq_set (&atItm.aplRat, ((LPAPLRAT) lpMemRht)++);

                    // Square it
                    mpq_mul (&atMul.aplRat, &atItm.aplRat, &atItm.aplRat);

                    // Accumulate
                    mpq_add (&atRes.aplRat, &atRes.aplRat, &atMul.aplRat);
                } // End FOR

                // Save in the result
                mpq_init_set (((LPAPLRAT) lpMemRes)++, &atRes.aplRat);
            } // End FOR

            // We no longer need this storage
            Myq_clear (&atMul.aplRat);
            Myq_clear (&atItm.aplRat);
            Myq_clear (&atRes.aplRat);

            break;

        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            // Initialize to 0
            mpfr_init0 (&atRes.aplVfp);
            mpfr_init0 (&atItm.aplVfp);
            mpfr_init0 (&atMul.aplVfp);

            // Loop through the items
            for (uCnt = 0; uCnt < aplNELMRht; uCnt++)
            {
                // Initialize the accumulator
                mpfr_set_si (&atRes.aplVfp, 0, MPFR_RNDN);

                // Looop through all of the parts
                for (i = 0; i < iHCDimRht; i++)
                {
                    // Get the next item
                    mpfr_set (&atItm.aplVfp, ((LPAPLVFP) lpMemRht)++, MPFR_RNDN);

                    // Square it
                    mpfr_mul (&atMul.aplVfp, &atItm.aplVfp, &atItm.aplVfp, MPFR_RNDN);

                    // Accumulate
                    mpfr_add (&atRes.aplVfp, &atRes.aplVfp, &atMul.aplVfp, MPFR_RNDN);
                } // End FOR

                // Save in the result
                mpfr_init_set (((LPAPLVFP) lpMemRes)++, &atRes.aplVfp, MPFR_RNDN);
            } // End FOR

            // We no longer need this storage
            Myf_clear (&atMul.aplVfp);
            Myf_clear (&atItm.aplVfp);
            Myf_clear (&atRes.aplVfp);

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            goto DOMAIN_EXIT;

        defstop
            break;
    } // End SWITCH
YYALLOC_GLB_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   =                // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // Unlock the result global memory in case TypeDemote actually demotes
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken, FALSE);

    goto NORMAL_EXIT;

YYALLOC_IMM_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = TranslateArrayTypeToImmType (aplTypeRht);
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkLongest  = aplLongestRht;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnMonNotEqual_EM_YY


//***************************************************************************
//  $PrimSpecNotEqualStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecNotEqualStorageTypeDyd
    (APLNELM    aplNELMLft,
     LPAPLSTYPE lpaplTypeLft,
     LPTOKEN    lptkFunc,
     APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht)

{
    APLSTYPE aplTypeRes;

    // In case the left arg is an empty char,
    //   change its type to BOOL
    if (IsCharEmpty (*lpaplTypeLft, aplNELMLft))
        *lpaplTypeLft = ARRAY_BOOL;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsCharEmpty (*lpaplTypeRht, aplNELMRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // Calculate the storage type of the result
    aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // All numeric results are Boolean
    if (IsNumeric (aplTypeRes))
        aplTypeRes = ARRAY_BOOL;

    return aplTypeRes;
} // End PrimSpecNotEqualStorageTypeDyd


//***************************************************************************
//  $PrimFnDydNotEqualBisBvB
//
//  Primitive scalar function dyadic NotEqual:  B {is} B fn B
//***************************************************************************

void PrimFnDydNotEqualBisBvB
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplBoolean NE lpatRht->aplBoolean)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydNotEqualBisBvB


//***************************************************************************
//  $PrimFnDydNotEqualB64isB64vB64
//
//  Primitive scalar function dyadic NotEqual:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydNotEqualB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft ^ aplBooleanRht);
} // End PrimFnDydNotEqualB64isB64vB64


//***************************************************************************
//  $PrimFnDydNotEqualB32isB32vB32
//
//  Primitive scalar function dyadic NotEqual:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydNotEqualB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft ^ aplBooleanRht);
} // End PrimFnDydNotEqualB32isB32vB32


//***************************************************************************
//  $PrimFnDydNotEqualB16isB16vB16
//
//  Primitive scalar function dyadic NotEqual:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydNotEqualB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft ^ aplBooleanRht);
} // End PrimFnDydNotEqualB16isB16vB16


//***************************************************************************
//  $PrimFnDydNotEqualB08isB08vB08
//
//  Primitive scalar function dyadic NotEqual:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydNotEqualB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft ^ aplBooleanRht);
} // End PrimFnDydNotEqualB08isB08vB08


//***************************************************************************
//  $PrimFnDydNotEqualBisIvI
//
//  Primitive scalar function dyadic NotEqual:  B {is} I fn I
//***************************************************************************

void PrimFnDydNotEqualBisIvI
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplInteger NE lpatRht->aplInteger)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydNotEqualBisIvI


//***************************************************************************
//  $PrimFnDydNotEqualBisFvF
//
//  Primitive scalar function dyadic NotEqual:  B {is} F fn F
//***************************************************************************

void PrimFnDydNotEqualBisFvF
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Compare the two floats relative to []CT
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_F (lpatLft->aplFloat, lpatRht->aplFloat, GetQuadCT (), NE)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydNotEqualBisFvF


//***************************************************************************
//  $PrimFnDydNotEqualBisCvC
//
//  Primitive scalar function dyadic NotEqual:  B {is} C fn C
//***************************************************************************

void PrimFnDydNotEqualBisCvC
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplChar NE lpatRht->aplChar)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydNotEqualBisCvC


//***************************************************************************
//  $PrimFnDydNotEqualBisHvH
//
//  Primitive scalar function dyadic NotEqual:  B {is} H fn H
//***************************************************************************

void PrimFnDydNotEqualBisHvH
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    UBOOL bRet;                     // The result

    // If the immediate types are both char, ...
    if (IsImmChr (lpatLft->aplHetero->stFlags.ImmType)
     && IsImmChr (lpatRht->aplHetero->stFlags.ImmType))
        bRet = (lpatLft->aplHetero->stData.stChar NE
                lpatRht->aplHetero->stData.stChar);
    else
        bRet = BIT0;

    // If the result is TRUE, ...
    if (bRet)
        lpMemRes[uRes >> LOG2NBIB] |=
          BIT0
          << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydNotEqualBisHvH


//***************************************************************************
//  $PrimFnDydNotEqualRisRvR
//
//  Primitive scalar function dyadic NotEqual:  B {is} R fn R
//***************************************************************************

void PrimFnDydNotEqualBisRvR
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_R (lpatLft->aplRat, lpatRht->aplRat, GetQuadCT (), NE)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydNotEqualBisRvR


//***************************************************************************
//  $PrimFnDydNotEqualBisVvV
//
//  Primitive scalar function dyadic NotEqual:  B {is} V fn V
//***************************************************************************

void PrimFnDydNotEqualBisVvV
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Compare the two VFPs relative to []CT
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_V (lpatLft->aplVfp, lpatRht->aplVfp, GetQuadCT (), NE)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydNotEqualBisVvV


//***************************************************************************
//  $PrimFnDydNotEqualBisHC2IvHC2I
//
//  Primitive scalar function dyadic NotEqual:  B {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydNotEqualBisHC2IvHC2I
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EqualBisHCxIvHCxI (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       2,               // Dimension (1, 2, 4, 8)
                       FALSE,           // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);           // Ptr to CheckException msg
} // End PrimFnDydNotEqualBisHC2IvHC2I


//***************************************************************************
//  $PrimFnDydNotEqualBisHC4IvHC4I
//
//  Primitive scalar function dyadic NotEqual:  B {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydNotEqualBisHC4IvHC4I
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EqualBisHCxIvHCxI (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       4,               // Dimension (1, 2, 4, 8)
                       FALSE,           // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);           // Ptr to CheckException msg
} // End PrimFnDydNotEqualBisHC4IvHC4I


//***************************************************************************
//  $PrimFnDydNotEqualBisHC8IvHC8I
//
//  Primitive scalar function dyadic NotEqual:  B {is} HC8I fn HC8I
//***************************************************************************

void PrimFnDydNotEqualBisHC8IvHC8I
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EqualBisHCxIvHCxI (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       8,               // Dimension (1, 2, 4, 8)
                       FALSE,           // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);           // Ptr to CheckException msg
} // End PrimFnDydNotEqualBisHC8IvHC8I


//***************************************************************************
//  $PrimFnDydNotEqualBisHC2FvHC2F
//
//  Primitive scalar function dyadic NotEqual:  B {is} HC2F fn HC2F
//***************************************************************************

void PrimFnDydNotEqualBisHC2FvHC2F
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxFvHCxF (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       2,               // Dimension (1, 2, 4, 8)
                       FALSE,           // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);           // Ptr to CheckException msg
} // End PrimFnDydNotEqualBisHC2FvHC2F


//***************************************************************************
//  $PrimFnDydNotEqualBisHC4FvHC4F
//
//  Primitive scalar function dyadic NotEqual:  B {is} HC4F fn HC4F
//***************************************************************************

void PrimFnDydNotEqualBisHC4FvHC4F
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxFvHCxF (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       4,               // Dimension (1, 2, 4, 8)
                       FALSE,           // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);           // Ptr to CheckException msg
} // End PrimFnDydNotEqualBisHC4FvHC4F


//***************************************************************************
//  $PrimFnDydNotEqualBisHC8FvHC8F
//
//  Primitive scalar function dyadic NotEqual:  B {is} HC8F fn HC8F
//***************************************************************************

void PrimFnDydNotEqualBisHC8FvHC8F
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxFvHCxF (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       8,               // Dimension (1, 2, 4, 8)
                       FALSE,           // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);           // Ptr to CheckException msg
} // End PrimFnDydNotEqualBisHC8FvHC8F


//***************************************************************************
//  $PrimFnDydNotEqualBisHC2RvHC2R
//
//  Primitive scalar function dyadic NotEqual:  B {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydNotEqualBisHC2RvHC2R
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxRvHCxR (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       2,               // Dimension (1, 2, 4, 8)
                       FALSE,           // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);           // Ptr to CheckException msg
} // End PrimFnDydNotEqualBisHC2RvHC2R


//***************************************************************************
//  $PrimFnDydNotEqualBisHC4RvHC4R
//
//  Primitive scalar function dyadic NotEqual:  B {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydNotEqualBisHC4RvHC4R
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxRvHCxR (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       4,               // Dimension (1, 2, 4, 8)
                       FALSE,           // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);           // Ptr to CheckException msg
} // End PrimFnDydNotEqualBisHC4RvHC4R


//***************************************************************************
//  $PrimFnDydNotEqualBisHC8RvHC8R
//
//  Primitive scalar function dyadic NotEqual:  B {is} HC8R fn HC8R
//***************************************************************************

void PrimFnDydNotEqualBisHC8RvHC8R
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxRvHCxR (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       8,               // Dimension (1, 2, 4, 8)
                       FALSE,           // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);           // Ptr to CheckException msg
} // End PrimFnDydNotEqualBisHC8RvHC8R


//***************************************************************************
//  $PrimFnDydNotEqualBisHC2VvHC2V
//
//  Primitive scalar function dyadic NotEqual:  B {is} HC2V fn HC2V
//***************************************************************************

void PrimFnDydNotEqualBisHC2VvHC2V
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxVvHCxV (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       2,               // Dimension (1, 2, 4, 8)
                       FALSE,           // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);           // Ptr to CheckException msg
} // End PrimFnDydNotEqualBisHC2VvHC2V


//***************************************************************************
//  $PrimFnDydNotEqualBisHC4VvHC4V
//
//  Primitive scalar function dyadic NotEqual:  B {is} HC4V fn HC4V
//***************************************************************************

void PrimFnDydNotEqualBisHC4VvHC4V
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxVvHCxV (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       4,               // Dimension (1, 2, 4, 8)
                       FALSE,           // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);           // Ptr to CheckException msg
} // End PrimFnDydNotEqualBisHC4VvHC4V


//***************************************************************************
//  $PrimFnDydNotEqualBisHC8VvHC8V
//
//  Primitive scalar function dyadic NotEqual:  B {is} HC8V fn HC8V
//***************************************************************************

void PrimFnDydNotEqualBisHC8VvHC8V
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxVvHCxV (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       8,               // Dimension (1, 2, 4, 8)
                       FALSE,           // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);           // Ptr to CheckException msg
} // End PrimFnDydNotEqualBisHC8VvHC8V


//***************************************************************************
//  End of File: pf_nequal.c
//***************************************************************************
