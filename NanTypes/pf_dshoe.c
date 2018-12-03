//***************************************************************************
//  NARS2000 -- Primitive Function -- DownShoe
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


//***************************************************************************
//  $PrimFnDownShoe_EM_YY
//
//  Primitive function for monadic and dyadic DownShoe ("unique" and "union")
//***************************************************************************

LPPL_YYSTYPE PrimFnDownShoe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DOWNSHOE);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonDownShoe_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydDownShoe_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnDownShoe_EM_YY


//***************************************************************************
//  $PrimProtoFnDownShoe_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic DownShoe
//***************************************************************************

LPPL_YYSTYPE PrimProtoFnDownShoe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnDownShoe_EM_YY,   // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnDownShoe_EM


//***************************************************************************
//  $PrimIdentFnDownShoe_EM_YY
//
//  Generate an identity element for the primitive function dyadic DownShoe
//***************************************************************************

LPPL_YYSTYPE PrimIdentFnDownShoe_EM_YY
    (LPTOKEN lptkRhtOrig,           // Ptr to original right arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLRANK      aplRankRht;        // Right arg rank
    LPPL_YYSTYPE lpYYRes;           // Ptr to result

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig NE NULL);
    Assert (lptkFunc    NE NULL);
    Assert (lptkRhtArg  NE NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // The (left/right) identity function for dyadic DownShoe
    //   (L {downshoe} R) ("union") is
    //   {zilde}.

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, NULL, NULL, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto DOMAIN_EXIT;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbZilde);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimIdentFnDownShoe_EM_YY


//***************************************************************************
//  $PrimFnMonDownShoe_EM_YY
//
//  Primitive function for monadic DownShoe ("unique")
//***************************************************************************

LPPL_YYSTYPE PrimFnMonDownShoe_EM_YY
    (LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeRes;           // Result    ...
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMRes;           // Result    ...
    APLRANK           aplRankRht;           // Right arg rank
    APLLONGEST        aplLongestRht;        // Right arg immediate value
    APLUINT           ByteRes,              // # bytes in the result
                      uRht;                 // Loop counter
    APLBOOL           bQuadIO;              // []IO
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL,       // Result    ...
                      hGlbGup = NULL,       // Grade-up global memory handle
                      hGlbTmp = NULL;       // Temporary ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL,   // Ptr to right arg header
                      lpMemHdrGup = NULL,   // Ptr to Gup header
                      lpMemHdrTmp = NULL;   // Ptr to Tmp header
    LPAPLINT          lpMemTmp;             // Ptr to temporary global memory
    LPVOID            lpMemRht,             // Ptr to right arg global memory
                      lpMemRes;             // Ptr to result    ...
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to result
    TOKEN             tkFunc = {0};         // Internal function token
    int               iHCDimRht,            // HC Dimension (1, 2, 4, 8)
                      i;                    // Loop counter
    UBOOL             bMemGup = FALSE;      // TRUE iff we have already set <lpMemGup>

    // Save the current index origin and set it to zero
    bQuadIO = GetQuadIO ();
    SetQuadIO (0);

    Assert (lptkAxis EQ NULL);

    // Get the right arg (Type, NELM, Rank)
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // Get right arg global ptr
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the right arg is not immediate, ...
    if (hGlbRht NE NULL)
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        // Point to the data
        lpMemRht = &aplLongestRht;

    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDimRht = TranslateArrayTypeToHCDim (aplTypeRht);

    // If the right arg is a scalar or an APA with a zero multiplier, ...
    if (IsScalar (aplRankRht)
     || (IsSimpleAPA (aplTypeRht)
      && ((LPAPLAPA) lpMemRht)->Mul EQ 0))
    {
        // Return a one-element vector

        // If the right arg is an APA, the result is an integer
        if (IsSimpleAPA (aplTypeRht))
            aplTypeRes = ARRAY_INT;
        else
            aplTypeRes = aplTypeRht;

        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRht, 1, 1);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Allocate space for the result.
        hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        lpMemHdrRes
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = aplTypeRes;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
////////lpHeader->PV0        = FALSE;           // Already zero from GHND
////////lpHeader->PV1        = FALSE;           // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = 1;
        lpHeader->Rank       = 1;
#undef  lpHeader

        // Save the dimension in the result
        *VarArrayBaseToDim (lpMemHdrRes) = 1;

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
                *((LPAPLBOOL)  lpMemRes) = (APLBOOL) aplLongestRht;

                break;

            case ARRAY_INT:
            case ARRAY_APA:
                *((LPAPLINT)   lpMemRes) = aplLongestRht;

                break;

            case ARRAY_FLOAT:
                *((LPAPLFLOAT) lpMemRes) = *(LPAPLFLOAT) &aplLongestRht;

                break;

            case ARRAY_CHAR:
                *((LPAPLCHAR)  lpMemRes) = (APLCHAR) aplLongestRht;

                break;

            case ARRAY_RAT:
            case ARRAY_HC2R:
            case ARRAY_HC4R:
            case ARRAY_HC8R:
                // Loop through all of the parts
                for (i = 0; i < iHCDimRht; i++)
                    mpq_init_set (((LPAPLRAT) lpMemRes)++, ((LPAPLRAT) lpMemRht)++);

                break;

            case ARRAY_VFP:
            case ARRAY_HC2V:
            case ARRAY_HC4V:
            case ARRAY_HC8V:
                // Loop through all of the parts
                for (i = 0; i < iHCDimRht; i++)
                    mpfr_init_set (((LPAPLVFP) lpMemRes)++, ((LPAPLVFP) lpMemRht)++, MPFR_RNDN);

                break;

            case ARRAY_ARB:
            case ARRAY_BA2F:
            case ARRAY_BA4F:
            case ARRAY_BA8F:
                // Loop through all of the parts
                for (i = 0; i < iHCDimRht; i++)
                    arb_init_set (((LPAPLARB) lpMemRes)++, ((LPAPLARB) lpMemRht)++);

                break;

            case ARRAY_NESTED:
                *((LPAPLNESTED) lpMemRes) = CopySymGlbInd_PTB (lpMemRht);

                break;

            case ARRAY_HETERO:      // No such thing as a salar hetero array
            defstop
                break;
        } // End SWITCH

        goto YYALLOC_EXIT;
    } else
    // If the right arg is NOT a scalar and NOT an APA with a zero multiplier, ...
    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        {
            APLNELM aplNELMRnd;         // Right arg NELM rounded up to bits in byte
            APLBOOL aplComp,            // Comparison value (0x00 or 0xFF)
                    aplLastByte,        // Last byte
                    aplMask;            // Mask for testing the extra bits in the last byte

            // The first element of the right arg is in aplLongestRht

            // Get the # bytes in the right arg
            aplNELMRnd = RoundUpBitsToBytes (aplNELMRht);

            // Get the comparison value
            // If we're searching for a 1, compare using NE against 0x00;
            // ...                      0, ...                      0xFF
            aplComp = (aplLongestRht EQ 0) ? 0x00 : 0xFF;

            // Search for the complement of the first value
            for (uRht = 0; uRht < aplNELMRnd; uRht++)
            if (aplComp NE (aplLastByte = ((LPAPLBOOL) lpMemRht)[uRht]))
                break;

            // Initialize the result NELM
            aplNELMRes = (aplNELMRht NE 0);

            // Get the # extra bits (0 to 7)
            aplMask = (MASKLOG2NBIB & (UINT) aplNELMRht);

            // Check for the last byte
            if (uRht EQ (aplNELMRnd - 1)
             && aplMask)
            {
                // Get a mask to isolate the active bits in the last byte
                //   0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F
                aplMask = (BIT0 << aplMask) - 1;

                if ((aplComp & aplMask) NE (aplLastByte & aplMask))
                    aplNELMRes++;
            } else
            if (uRht < aplNELMRnd)
                aplNELMRes++;

            // If there's more than one element in the result, ...
            if (aplNELMRes > 1
             && aplLongestRht EQ 0)
                aplLongestRht ^= BIT1;

            // Calculate space needed for the result
            ByteRes = CalcArraySize (aplTypeRht, aplNELMRes, 1);

            // Check for overflow
            if (ByteRes NE (APLU3264) ByteRes)
                goto WSFULL_EXIT;

            // Allocate space for the result.
            hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
            if (hGlbRes EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        lpMemHdrRes
            // Fill in the header
            lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = ARRAY_BOOL;
////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
////////////lpHeader->PV0        = FALSE;           // Already zero from GHND
////////////lpHeader->PV1        = FALSE;           // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELMRes;
            lpHeader->Rank       = 1;
#undef  lpHeader

            // Save the dimension in the result
            *VarArrayBaseToDim (lpMemHdrRes) = aplNELMRes;

            // Skip over the header and dimensions to the data
            lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

            if (!IsEmpty (aplNELMRes))
                *((LPAPLBOOL) lpMemRes) = (APLBOOL) aplLongestRht;
            break;
        } // End ARRAY_BOOL

        case ARRAY_INT:
            // If the right arg is a PermVec, return it
            if (IsPermVector (lpMemHdrRht))
            {
                hGlbRes = CopySymGlbDirAsGlb (hGlbRht);

                break;
            } // End IF

            // Otherwise fall through ...

        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:

        case ARRAY_FLOAT:
        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:

        case ARRAY_CHAR:

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

        case ARRAY_HETERO:
        case ARRAY_NESTED:
        {
            TOKEN    tkRht  = {0};          // Right arg token
            APLFLOAT fQuadCT;               // []CT
            ALLTYPES atLst = {0},           // Last value as ALLTYPES
                     atRht = {0};           // Right ...
            APLCHAR  aplLastChr;            // ...      char  ...
            LPAPLINT lpMemGup;              // Ptr to grade-up global memory

            // Setup the grade-up function token
            tkFunc.tkFlags.TknType   = TKT_FCNIMMED;
            tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////////////tkFunc.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
            tkFunc.tkData.tkChar     = UTF16_DELTASTILE;
            tkFunc.tkCharIndex       = lptkFunc->tkCharIndex;

            // Get the current value of []CT
            fQuadCT = GetQuadCT ();

            //***************************************************************
            // In order to find unique values, we grade-up the right arg and
            //   loop through the grade-up indices looking for different
            //   values in the right arg.
            //***************************************************************

            // Grade-up the right arg
            lpYYRes =
              PrimFnMonGradeCommon_EM_YY (&tkFunc,      // Ptr to function token
                                           lptkRhtArg,  // Ptr to right arg token
                                           NULL,        // Ptr to axis token (may be NULL)
                                           TRUE,        // TRUE iff we're to treat the right arg as ravelled
                                           TRUE);       // TRUE iff we can grade all arrays
            if (lpYYRes EQ NULL)
                goto ERROR_EXIT;
            // Get the grade-up global memory handle
            hGlbGup = lpYYRes->tkToken.tkData.tkGlbData;

            // Free the YYRes
            YYFree (lpYYRes); lpYYRes = NULL;

            // Lock the memory to get a ptr to it
            lpMemHdrGup = MyGlobalLockVar (hGlbGup);

            // Skip over the header and dimensions to the data
            lpMemGup = VarArrayDataFmBase (lpMemHdrGup);

            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_INT:
                    // Initialize the last value with the first one
                    if (!IsEmpty (aplNELMRht))
                        (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, lpMemGup[0], &atLst);

                    // Trundle through the grade-up of the right arg
                    //   counting unique values and saving their indices
                    for (uRht = 1, aplNELMRes = (aplNELMRht > 0);
                         uRht < aplNELMRht;
                         uRht++)
                    {
                        // Copy the next value in Gup order
                        (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, lpMemGup[uRht], &atRht);

                        // Check for a different (and thus unique) value
                        if (!EqualHCxIvHCxI (&atLst, &atRht, iHCDimRht, fQuadCT, WFCN))
                        {
                            // Save as the next unique value's index
                            lpMemGup[aplNELMRes++] = lpMemGup[uRht];

                            // Free old atLst
                            (*aTypeFree[aplTypeRht]) (&atLst, 0);

                            // Save as the next unique value
                            (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, lpMemGup[uRht], &atLst);
                        } // End IF

                        // Free old atRht
                        (*aTypeFree[aplTypeRht]) (&atRht, 0);
                    } // End FOR

                    if (!IsEmpty (aplNELMRht))
                        // Free old atLst
                        (*aTypeFree[aplTypeRht]) (&atLst, 0);

                    break;

                case ARRAY_FLOAT:
                    // Initialize the last value with the first one
                    if (!IsEmpty (aplNELMRht))
                        (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, lpMemGup[0], &atLst);

                    // Trundle through the grade-up of the right arg
                    //   counting unique values and saving their indices
                    for (uRht = 1, aplNELMRes = (aplNELMRht > 0);
                         uRht < aplNELMRht;
                         uRht++)
                    {
                        // Copy the next value in Gup order
                        (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, lpMemGup[uRht], &atRht);

                        // Check for a different (and thus unique) value
                        if (!EqualHCxFvHCxF (&atLst, &atRht, iHCDimRht, fQuadCT, WFCN))
                        {
                            // Save as the next unique value's index
                            lpMemGup[aplNELMRes++] = lpMemGup[uRht];

                            // Free old atLst
                            (*aTypeFree[aplTypeRht]) (&atLst, 0);

                            // Save as the next unique value
                            (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, lpMemGup[uRht], &atLst);
                        } // End IF

                        // Free old atRht
                        (*aTypeFree[aplTypeRht]) (&atRht, 0);
                    } // End FOR

                    if (!IsEmpty (aplNELMRht))
                        // Free old atLst
                        (*aTypeFree[aplTypeRht]) (&atLst, 0);

                    break;

                case ARRAY_CHAR:
                    // Initialize the last value with the first one
                    if (!IsEmpty (aplNELMRht))
                        aplLastChr = ((LPAPLCHAR)  lpMemRht)[*lpMemGup];

                    // Trundle through the grade-up of the right arg
                    //   counting unique values and saving their indices
                    for (uRht = 1, aplNELMRes = (aplNELMRht > 0);
                         uRht < aplNELMRht;
                         uRht++)
                    // Check for a different (and thus unique) value
                    if (aplLastChr NE ((LPAPLCHAR) lpMemRht)[lpMemGup[uRht]])
                    {
                        // Save as the next unique value's index
                        lpMemGup[aplNELMRes++] = lpMemGup[uRht];

                        // Save as the next unique value
                        aplLastChr = ((LPAPLCHAR) lpMemRht)[lpMemGup[uRht]];
                    } // End FOR/IF

                    break;

                case ARRAY_RAT:
                    // Initialize the last value with the first one
                    if (!IsEmpty (aplNELMRht))
                        (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, lpMemGup[0], &atLst);

                    // Trundle through the grade-up of the right arg
                    //   counting unique values and saving their indices
                    for (uRht = 1, aplNELMRes = (aplNELMRht > 0);
                         uRht < aplNELMRht;
                         uRht++)
                    {
                        // Copy the next value in Gup order
                        (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, lpMemGup[uRht], &atRht);

                        // Check for a different (and thus unique) value
                        if (!EqualHCxRvHCxR (&atLst, &atRht, iHCDimRht, fQuadCT, WFCN))
                        {
                            // Save as the next unique value's index
                            lpMemGup[aplNELMRes++] = lpMemGup[uRht];

                            // Free old atLst
                            (*aTypeFree[aplTypeRht]) (&atLst, 0);

                            // Save as the next unique value
                            (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, lpMemGup[uRht], &atLst);
                        } // End IF

                        // Free old atRht
                        (*aTypeFree[aplTypeRht]) (&atRht, 0);
                    } // End FOR

                    if (!IsEmpty (aplNELMRht))
                        // Free old atLst
                        (*aTypeFree[aplTypeRht]) (&atLst, 0);

                    break;

                case ARRAY_VFP:
                    // Initialize the last value with the first one
                    if (!IsEmpty (aplNELMRht))
                        (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, lpMemGup[0], &atLst);

                    // Trundle through the grade-up of the right arg
                    //   counting unique values and saving their indices
                    for (uRht = 1, aplNELMRes = (aplNELMRht > 0);
                         uRht < aplNELMRht;
                         uRht++)
                    {
                        // Copy the next value in Gup order
                        (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, lpMemGup[uRht], &atRht);

                        // Check for a different (and thus unique) value
                        if (!EqualHCxVvHCxV (&atLst, &atRht, iHCDimRht, fQuadCT, WFCN))
                        {
                            // Save as the next unique value's index
                            lpMemGup[aplNELMRes++] = lpMemGup[uRht];

                            // Free old atLst
                            (*aTypeFree[aplTypeRht]) (&atLst, 0);

                            // Save as the next unique value
                            (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, lpMemGup[uRht], &atLst);
                        } // End IF

                        // Free old atRht
                        (*aTypeFree[aplTypeRht]) (&atRht, 0);
                    } // End FOR

                    if (!IsEmpty (aplNELMRht))
                        // Free old atLst
                        (*aTypeFree[aplTypeRht]) (&atLst, 0);

                    break;

                case ARRAY_ARB:
                    // Initialize the last value with the first one
                    if (!IsEmpty (aplNELMRht))
                        (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, lpMemGup[0], &atLst);

                    // Trundle through the grade-up of the right arg
                    //   counting unique values and saving their indices
                    for (uRht = 1, aplNELMRes = (aplNELMRht > 0);
                         uRht < aplNELMRht;
                         uRht++)
                    {
                        // Copy the next value in Gup order
                        (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, lpMemGup[uRht], &atRht);

                        // Check for a different (and thus unique) value
                        if (!EqualBAxFvBAxF (&atLst, &atRht, iHCDimRht, fQuadCT, WFCN))
                        {
                            // Save as the next unique value's index
                            lpMemGup[aplNELMRes++] = lpMemGup[uRht];

                            // Free old atLst
                            (*aTypeFree[aplTypeRht]) (&atLst, 0);

                            // Save as the next unique value
                            (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, lpMemGup[uRht], &atLst);
                        } // End IF

                        // Free old atRht
                        (*aTypeFree[aplTypeRht]) (&atRht, 0);
                    } // End FOR

                    if (!IsEmpty (aplNELMRht))
                        // Free old atLst
                        (*aTypeFree[aplTypeRht]) (&atLst, 0);

                    break;

                case ARRAY_HC2I:
                case ARRAY_HC4I:
                case ARRAY_HC8I:

                case ARRAY_HC2F:
                case ARRAY_HC4F:
                case ARRAY_HC8F:

                case ARRAY_HC2R:
                case ARRAY_HC4R:
                case ARRAY_HC8R:

                case ARRAY_HC2V:
                case ARRAY_HC4V:
                case ARRAY_HC8V:

                case ARRAY_BA2F:
                case ARRAY_BA4F:
                case ARRAY_BA8F:
                {
                    HGLOBAL           hGlbRiR;
                    LPVARARRAY_HEADER lpMemHdrRiR = NULL;
                    LPAPLINT          lpMemRiR;

                    // Setup the {iota} function token
                    tkFunc.tkFlags.TknType   = TKT_FCNIMMED;
                    tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////////////////////tkFunc.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
                    tkFunc.tkData.tkChar     = UTF16_IOTA;
                    tkFunc.tkCharIndex       = lptkFunc->tkCharIndex;

                    // Calculate R{iota}R
                    lpYYRes =
                      PrimFnDydIotaCom_EM_YY (lptkRhtArg,       // Ptr to left arg token
                                             &tkFunc,           // Ptr to function token
                                              lptkRhtArg,       // Ptr to right arg token
                                              lptkAxis,         // Ptr to axis token (may be NULL)
                                              FALSE);           // TRUE iff we're to TypeDemote at the end
                    // Check for error
                    if (lpYYRes EQ NULL)
                        goto ERROR_EXIT;
                    // Get the global handle
                    hGlbRiR = GetGlbHandle (&lpYYRes->tkToken);

                    // We no longer need this resource
                    YYFree (lpYYRes); lpYYRes = NULL;

                    // Lock the memory to get a ptr to it
                    lpMemHdrRiR = MyGlobalLockVar (hGlbRiR);

                    // Skip over the header and dimensions to the data
                    lpMemRiR = VarArrayDataFmBase (lpMemHdrRiR);

                    // Initialize the starting element of <lpMemGup>
                    //   because were skipping over it
                    lpMemGup[0] = 0;

                    // Loop through the above R{iota}R
                    for (uRht = 1, aplNELMRes = (aplNELMRht > 0);
                         uRht < aplNELMRht;
                         uRht++)
                    {
                        // If the RiR value is the same as {iota}{rho}R (a.k.a. uRht), ...
                        if (lpMemRiR[uRht] EQ uRht)
                            // Save as the next unique value's index
                            lpMemGup[aplNELMRes++] = uRht;
                    } // End FOR

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbRiR); lpMemHdrRiR = NULL;

                    // We no longer need this storage
                    MyGlobalFree (hGlbRiR); hGlbRiR = NULL;

                    // Tell the following code we have already set <lpMemGup>
                    bMemGup = TRUE;

                    break;
                } // End RiR code

                defstop
                    break;
            } // End IF/SWITCH

            //***************************************************************
            // In order to preserve the order of the unique elements
            //   we need to grade-up the indices of the unique values so we
            //   can place them into the result in the same order as
            //   they occur in the right arg.
            //***************************************************************

            // Calculate space needed for a temp
            ByteRes = CalcArraySize (ARRAY_INT, aplNELMRes, 1);

            // Check for overflow
            if (ByteRes NE (APLU3264) ByteRes)
                goto WSFULL_EXIT;

            // Allocate space for the temp
            hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
            if (hGlbTmp EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemHdrTmp = MyGlobalLock000 (hGlbTmp);

#define lpHeader        lpMemHdrTmp
            // Fill in the header
            lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = ARRAY_INT;
////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
////////////lpHeader->PV0        = FALSE;           // Already zero from GHND
////////////lpHeader->PV1        = FALSE;           // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELMRes;
            lpHeader->Rank       = 1;
#undef  lpHeader
            // Save the dimension in the result
            *VarArrayBaseToDim (lpMemHdrTmp) = aplNELMRes;

            // Skip over the header and dimensions to the data
            lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

            // Copy the data from lpMemGup to lpMemTmp
            CopyMemory (lpMemTmp, lpMemGup, (APLU3264) aplNELMRes * sizeof (APLINT));

            // We no longer need this ptr
            MyGlobalUnlock (hGlbGup); lpMemHdrGup = NULL;

            // We no longer need this storage
            FreeResultGlobalVar (hGlbGup); hGlbGup = NULL;

            // If we have already set <lpMemGup> (now <lpMemTmp>), ...
            if (bMemGup)
            {
                // Create a temp vector with {iota}{rho}R for use in the fill code as in <lpMemTmp[*lpMemGup]>
                // Calculate space needed for a temp
                ByteRes = CalcArraySize (ARRAY_INT, aplNELMRes, 1);

                // Check for overflow
                if (ByteRes NE (APLU3264) ByteRes)
                    goto WSFULL_EXIT;

                // Allocate space for the temp
                hGlbGup = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                if (hGlbGup EQ NULL)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemHdrGup = MyGlobalLock000 (hGlbGup);

#define lpHeader        lpMemHdrGup
                // Fill in the header
                lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                lpHeader->ArrType    = ARRAY_INT;
////////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
////////////////lpHeader->PV0        = FALSE;           // Already zero from GHND
////////////////lpHeader->PV1        = FALSE;           // Already zero from GHND
                lpHeader->RefCnt     = 1;
                lpHeader->NELM       = aplNELMRes;
                lpHeader->Rank       = 1;
#undef  lpHeader
                // Save the dimension in the result
                *VarArrayBaseToDim (lpMemHdrGup) = aplNELMRes;

                // Skip over the header and dimensions to the data
                lpMemGup = VarArrayDataFmBase (lpMemHdrGup);

                // Loop through the items in the temp
                for (uRht = 0; uRht < aplNELMRes; uRht++)
                    // Fill in the temp with {iota}{rho}R
                    lpMemGup[uRht] = uRht;
            } else
            {
                // Setup the right arg token
                tkRht.tkFlags.TknType   = TKT_VARARRAY;
////////////////tkRht.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////////////tkRht.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
                tkRht.tkData.tkGlbData  = MakePtrTypeGlb (hGlbTmp);
                tkRht.tkCharIndex       = lptkFunc->tkCharIndex;

                // Grade-up the graded-up right arg
                lpYYRes =
                  PrimFnMonGradeCommon_EM_YY (&tkFunc,      // Ptr to function token
                                              &tkRht,       // Ptr to right arg token
                                               NULL,        // Ptr to axis token (may be NULL)
                                               TRUE,        // TRUE iff we're to treat the right arg as ravelled
                                               TRUE);       // TRUE iff we can grade all arrays
                if (lpYYRes EQ NULL)
                    goto ERROR_EXIT;
                // Get the grade-up global memory handle
                hGlbGup = lpYYRes->tkToken.tkData.tkGlbData;

                // Free the YYRes
                YYFree (lpYYRes); lpYYRes = NULL;

                // Lock the memory to get a ptr to it
                lpMemHdrGup = MyGlobalLockVar (hGlbGup);

                // Skip over the header and dimensions to the data
                lpMemGup = VarArrayDataFmBase (lpMemHdrGup);
            } // End IF/ELSE

            // Calculate space needed for the result
            ByteRes = CalcArraySize (aplTypeRht, aplNELMRes, 1);

            // Check for overflow
            if (ByteRes NE (APLU3264) ByteRes)
                goto WSFULL_EXIT;

            // Allocate space for the result.
            hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
            if (hGlbRes EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        lpMemHdrRes
            // Fill in the header
            lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = aplTypeRht;
////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
////////////lpHeader->PV0        = FALSE;           // Already zero from GHND
////////////lpHeader->PV1        = FALSE;           // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELMRes;
            lpHeader->Rank       = 1;
#undef  lpHeader
            // Save the dimension in the result
            *VarArrayBaseToDim (lpMemHdrRes) = aplNELMRes;

            // Skip over the header and dimensions to the data
            lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_INT:
                case ARRAY_HC2I:
                case ARRAY_HC4I:
                case ARRAY_HC8I:
                    // Trundle through the right arg via the grade-up
                    //   copying elements to the result
                    for (uRht = 0; uRht < aplNELMRes; uRht++, lpMemGup++)
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRht; i++)
                        *((LPAPLINT)   lpMemRes)++ = ((LPAPLINT)   lpMemRht)[i + iHCDimRht * lpMemTmp[*lpMemGup]];
                    break;

                case ARRAY_FLOAT:
                case ARRAY_HC2F:
                case ARRAY_HC4F:
                case ARRAY_HC8F:
                    // Trundle through the right arg via the grade-up
                    //   copying elements to the result
                    for (uRht = 0; uRht < aplNELMRes; uRht++, lpMemGup++)
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRht; i++)
                        *((LPAPLFLOAT) lpMemRes)++ = ((LPAPLFLOAT) lpMemRht)[i + iHCDimRht * lpMemTmp[*lpMemGup]];
                    break;

                case ARRAY_CHAR:
                    // Trundle through the right arg via the grade-up
                    //   copying elements to the result
                    for (uRht = 0; uRht < aplNELMRes; uRht++, lpMemGup++)
                        *((LPAPLCHAR)  lpMemRes)++ = ((LPAPLCHAR)  lpMemRht)[lpMemTmp[*lpMemGup]];
                    break;

                case ARRAY_RAT:
                case ARRAY_HC2R:
                case ARRAY_HC4R:
                case ARRAY_HC8R:
                    // Trundle through the right arg via the grade-up
                    //   copying elements to the result
                    for (uRht = 0; uRht < aplNELMRes; uRht++, lpMemGup++)
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRht; i++)
                        mpq_init_set (((LPAPLRAT)   lpMemRes)++, &((LPAPLRAT)   lpMemRht)[i + iHCDimRht * lpMemTmp[*lpMemGup]]);
                    break;

                case ARRAY_VFP:
                case ARRAY_HC2V:
                case ARRAY_HC4V:
                case ARRAY_HC8V:
                    // Trundle through the right arg via the grade-up
                    //   copying elements to the result
                    for (uRht = 0; uRht < aplNELMRes; uRht++, lpMemGup++)
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRht; i++)
                        mpfr_init_copy (((LPAPLVFP) lpMemRes)++, &((LPAPLVFP)   lpMemRht)[i + iHCDimRht * lpMemTmp[*lpMemGup]]);
                    break;

                case ARRAY_ARB:
                case ARRAY_BA2F:
                case ARRAY_BA4F:
                case ARRAY_BA8F:
                    // Trundle through the right arg via the grade-up
                    //   copying elements to the result
                    for (uRht = 0; uRht < aplNELMRes; uRht++, lpMemGup++)
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRht; i++)
                        arb_init_set (((LPAPLARB) lpMemRes)++, &((LPAPLARB)   lpMemRht)[i + iHCDimRht * lpMemTmp[*lpMemGup]]);
                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbTmp); lpMemTmp = NULL;

            break;
        } // End Non-ARRAY_BOOL/ARRAY_APA

        case ARRAY_APA:
            // The APA multiplier is non-zero, so return the right arg
            hGlbRes = CopySymGlbDirAsGlb (hGlbRht);

            break;

        defstop
            break;
    } // End IF/ELSE/SWITCH

    // If there's no result global memory handle, ...
    //   (we executed a magic function/operator)
    if (hGlbRes EQ NULL)
        goto NORMAL_EXIT;

YYALLOC_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes NE NULL)
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
    if (hGlbTmp NE NULL)
    {
        if (lpMemHdrTmp NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbTmp); lpMemHdrTmp = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbTmp); hGlbTmp = NULL;
    } // End IF

    if (hGlbGup NE NULL)
    {
        if (lpMemHdrGup NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbGup); lpMemHdrGup = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbGup); hGlbGup = NULL;
    } // End IF

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

    // Restore the original []IO
    SetQuadIO (bQuadIO);

    return lpYYRes;
} // End PrimFnMonDownShoe_EM_YY


//***************************************************************************
//  Magic function/operator for monadic DownShoe
//
//  Monadic DownShoe -- Unique
//
//  On scalar or vector right args, return the unique values
//***************************************************************************

//***************************************************************************
//  Magic function for dyadic DownShoe
//
//  Dyadic Down Shoe -- Union
//
//  Return the elements in L or in R.
//***************************************************************************

#include "mf_dshoe.h"


//***************************************************************************
//  $PrimFnDydDownShoe_EM_YY
//
//  Primitive function for dyadic DownShoe (Union)
//***************************************************************************

LPPL_YYSTYPE PrimFnDydDownShoe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankRht;        // Right ...
    HGLOBAL      hGlbMFO;           // Magic function/operator global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, NULL, NULL, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, NULL, NULL, &aplRankRht, NULL);

    // Check for LEFT/RIGHT RANK ERRORs
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the magic function/operator global memory handle
    hGlbMFO = lpMemPTD->hGlbMFO[MFOE_DydDnShoe];

    //  Return the elements in L or in R.
    //  Use an internal magic function.
    lpYYRes =
      ExecuteMagicFunction_EM_YY (lptkLftArg,   // Ptr to left arg token
                                  lptkFunc,     // Ptr to function token
                                  NULL,         // Ptr to function strand
                                  lptkRhtArg,   // Ptr to right arg token
                                  lptkAxis,     // Ptr to axis token
                                  hGlbMFO,      // Magic function/operator global memory handle
                                  NULL,         // Ptr to HSHTAB struc (may be NULL)
                                  LINENUM_ONE); // Starting line # type (see LINE_NUMS)
    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    return NULL;

ERROR_EXIT:
NORMAL_EXIT:
    return lpYYRes;
} // End PrimFnDydDownShoe_EM_YY


//***************************************************************************
//  End of File: pf_dshoe.c
//***************************************************************************
