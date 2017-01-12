//***************************************************************************
//  NARS2000 -- System Function -- Quad DC
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2016 Sudley Place Software

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
//  $SysFnDC_EM_YY
//
//  System function:  []DC -- Data Conversion
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDC_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDC_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonDC_EM_YY (            lptkFunc, lptkRhtArg);
    else
        return SysFnDydDC_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnDC_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonDC_EM_YY
//
//  Monadic []DC -- Data Conversion
//
//  For 1-measurable numeric arrays (BOOL, INT, FLOAT, APA, RAT, or VFP),
//    if the # cols is 1, 2, 4, or 8, convert the arg to the corresponding
//    HyperComplex number.
//  For 2-, 4-, or 8-measurable HC arrays, split out the individual
//    coefficients as a new (column) dimension.
//  For all other arrays, signal an error.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonDC_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonDC_EM_YY
    (LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg)                // Ptr to right arg token

{
    APLSTYPE aplTypeRht;                // Right arg storage type
    APLNELM  aplNELMRht;                // ...       NELM
    APLRANK  aplRankRht;                // ...       rank
    APLDIM   aplColsRht;                // ...       # cols

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, &aplColsRht);

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL :
        case ARRAY_INT  :
        case ARRAY_FLOAT:
        case ARRAY_APA  :
        case ARRAY_RAT  :
        case ARRAY_VFP  :
            return SysFnMonDC_ToHC_EM_YY (lptkRhtArg,       // Ptr to right arg token
                                          aplTypeRht,       // Right arg storage type
                                          aplNELMRht,       // ...       NELM
                                          aplRankRht,       // ...       rank
                                          aplColsRht,       //           # cols
                                          lptkFunc);        // Ptr to function token
        case ARRAY_HC2I:
        case ARRAY_HC2F:
        case ARRAY_HC2R:
        case ARRAY_HC2V:
        case ARRAY_HC4I:
        case ARRAY_HC4F:
        case ARRAY_HC4R:
        case ARRAY_HC4V:
        case ARRAY_HC8I:
        case ARRAY_HC8F:
        case ARRAY_HC8R:
        case ARRAY_HC8V:
            return SysFnMonDC_ToSimp_EM_YY (lptkRhtArg,     // Ptr to right arg token
                                            aplTypeRht,     // Right arg storage type
                                            aplNELMRht,     // ...       NELM
                                            aplRankRht,     // ...       rank
                                            aplColsRht,     //           # cols
                                            lptkFunc);      // Ptr to function token
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            goto RIGHT_DOMAIN_EXIT;

        defstop
            goto ERROR_EXIT;
    } // End SWITCH
RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

ERROR_EXIT:
    return NULL;
} // End SysFnMonDC_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonDC_ToHC_EM_YY
//
//  The right arg is simple numeric (BOOL, INT, FLOAT, APA, RAT, or VFP), and
//    the # cols is either 1, 2, 4, or 8, so convert it to the corresponding
//    HyperComplex number.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonDC_ToHC_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonDC_ToHC_EM_YY
    (LPTOKEN  lptkRhtArg,           // Ptr to right arg token
     APLSTYPE aplTypeRht,           // Right arg storage type
     APLNELM  aplNELMRht,           // ...       NELM
     APLRANK  aplRankRht,           // ...       rank
     APLDIM   aplColsRht,           //           # cols
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    APLSTYPE          aplTypeRes;           // Result storage type
    APLNELM           aplNELMRes;           // Result NELM
    APLRANK           aplRankRes;           // Result rank.
    APLUINT           ByteRes;              // # bytes in the result
    UINT              uRht,                 // Loop counter
                      uHCDimIndex,          // HC Dimension index (0, 1, 2, 3) for (1, 2, 4, 8)
                      uBitIndex;            // Bit index when marching through Booleans
    LPVARARRAY_HEADER lpMemHdrRes = NULL,   // Ptr to result header
                      lpMemHdrRht = NULL;   // ...    right arg ...
    HGLOBAL           hGlbRht,              // Right arg global memory handle
                      hGlbRes = NULL;       // Result    ...
    LPVOID            lpMemRht,             // Ptr to left arg global memory
                      lpMemRes;             // Ptr to result    ...
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    APLINT            apaOffRht,            // Right arg APA offset
                      apaMulRht;            // ...           multiplier

    // Check the # cols
    if (aplColsRht NE 1
     && aplColsRht NE 2
     && aplColsRht NE 4
     && aplColsRht NE 8)
        goto RIGHT_LENGTH_EXIT;

    // Get the HC Dimension index of the result (0, 1, 2, 3) for (1, 2, 4, 8)
    //   where 0 is not used as the result is always HC
    uHCDimIndex = (UINT) log2 ((APLFLOAT) aplColsRht);

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT :
        case ARRAY_APA :
            // Get the result storage type
            aplTypeRes = uHCxI[uHCDimIndex];

            break;

        case ARRAY_FLOAT:
            // Get the result storage type
            aplTypeRes = uHCxF[uHCDimIndex];

            break;

        case ARRAY_RAT:
            // Get the result storage type
            aplTypeRes = uHCxR[uHCDimIndex];

            break;

        case ARRAY_VFP:
            // Get the result storage type
            aplTypeRes = uHCxV[uHCDimIndex];

            break;

        defstop
            break;
    } // End SWITCH

    // Calculate the result NELM
    aplNELMRes = aplNELMRht / aplColsRht;

    // Calculate the result rank
    if (IsScalar (aplRankRht))
        aplRankRes = 0;
    else
        aplRankRes = aplRankRht - 1;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Fill in the header
    lpMemHdrRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpMemHdrRes->ArrType    = aplTypeRes;
////lpMemHdrRes->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpMemHdrRes->SysVar     = FALSE;           // Already zero from GHND
    lpMemHdrRes->RefCnt     = 1;
    lpMemHdrRes->NELM       = aplNELMRes;
    lpMemHdrRes->Rank       = aplRankRes;

    // Get the right arg global memory handle
    hGlbRht = GetGlbHandle (lptkRhtArg);

    // If the arg is not immediate, ...
    if (hGlbRht NE NULL)
    {
        // Lock the memory to get a ptr to it
        lpMemHdrRht = MyGlobalLockVar (hGlbRht);

        // Fill in the dimensions
        if (aplRankRes)
            CopyMemory (VarArrayBaseToDim (lpMemHdrRes),
                        VarArrayBaseToDim (lpMemHdrRht),
                        (APLU3264) (aplRankRes * sizeof (APLDIM)));
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    } else
        // Point to the data
        lpMemRht = GetPtrTknLongest (lptkRhtArg);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            // Initialize the bit index
            uBitIndex = 0;

            // Loop through the right arg NELM
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Save the INT
                *((LPAPLINT) lpMemRes)++ = (APLBOOL) (BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex));

                // Check for end-of-byte
                if (++uBitIndex EQ NBIB)
                {
                    uBitIndex = 0;              // Start over
                    ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                } // End IF
            } // End IF

            break;

        case ARRAY_APA:
            // Get the APA offset & multiplier
            apaOffRht = ((LPAPLAPA) lpMemRht)->Off;
            apaMulRht = ((LPAPLAPA) lpMemRht)->Mul;

            // Loop through the result NELM
            for (uRht = 0; uRht < aplNELMRht; uRht++)
                // Save the INT
                *((LPAPLINT) lpMemRes)++ = apaOffRht + apaMulRht * uRht;
            break;

        case ARRAY_INT:
        case ARRAY_FLOAT:
            // Copy the data to the result
            CopyMemory (lpMemRes, lpMemRht, (APLU3264) (aplNELMRes * TranslateArrayTypeToSizeof (aplTypeRes)));

            break;

        case ARRAY_RAT:
            // Loop through the right arg NELM
            for (uRht = 0; uRht < aplNELMRht; uRht++)
                // Save the RAT
                mpq_init_set (((LPAPLRAT) lpMemRes)++,
                              ((LPAPLRAT) lpMemRht)++);
            break;

        case ARRAY_VFP:
            // Loop through the right arg NELM
            for (uRht = 0; uRht < aplNELMRht; uRht++)
                // Save the VFP
                mpfr_init_set (((LPAPLVFP) lpMemRes)++,
                               ((LPAPLVFP) lpMemRht)++,
                               MPFR_RNDN);
            break;

        defstop
            break;
    } // End SWITCH

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    //***************************************************************************
    //  Note that we can't afford to type demote as it might demote the
    //    dimension of a hypercomplex number with enough trailing zeros
    //    such as []DC 8{take}2
    //***************************************************************************
////// Unlock the result global memory in case TypeDemote actually demotes
////if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
////{
////    // We no longer need this ptr
////    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
////} // End IF
////
////// See if it fits into a lower (but not necessarily smaller) datatype
////TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
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
} // End SysFnMonDC_ToHC_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonDC_ToSimp_EM_YY
//
//  The right arg is HyperComplex (HCxI, HCxF, HCxR, or HCxV) so convert it
//    to the corresponding simple numeric (INT, FLOAT, RAT, or VFP) array
//    with a new column dimension of 2, 4, or 8 as per the Dimension of
//    the right arg.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonDC_ToSimp_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonDC_ToSimp_EM_YY
    (LPTOKEN  lptkRhtArg,           // Ptr to right arg token
     APLSTYPE aplTypeRht,           // Right arg storage type
     APLNELM  aplNELMRht,           // ...       NELM
     APLRANK  aplRankRht,           // ...       rank
     APLDIM   aplColsRht,           //           # cols
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    APLSTYPE          aplTypeRes;           // Result storage type
    APLNELM           aplNELMRes,           // Result NELM
                      aplColsRes;           // Result ...
    APLRANK           aplRankRes;           // Result rank.
    APLUINT           ByteRes;              // # bytes in the result
    UINT              uRht;                 // Loop counter
    LPVARARRAY_HEADER lpMemHdrRes = NULL,   // Ptr to result header
                      lpMemHdrRht = NULL;   // ...    right arg ...
    HGLOBAL           hGlbRht,              // Right arg global memory handle
                      hGlbRes = NULL;       // Result    ...
    LPVOID            lpMemRht,             // Ptr to left arg global memory
                      lpMemRes;             // Ptr to result    ...
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result

    // Get the right arg global memory handle
    hGlbRht = GetGlbHandle (lptkRhtArg);

    Assert (hGlbRht NE NULL);

    // Calculate the result # cols (= right arg HC Dimension)
    aplColsRes = TranslateArrayTypeToHCDim (aplTypeRht);

    Assert (aplColsRes EQ 2
         || aplColsRes EQ 4
         || aplColsRes EQ 8);

    // Calculate the result storage type
    aplTypeRes = aToSimple[aplTypeRht];

    // Calculate the result NELM
    aplNELMRes = aplNELMRht * aplColsRes;

    // Calculate the result rank
    aplRankRes = aplRankRht + 1;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Fill in the header
    lpMemHdrRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpMemHdrRes->ArrType    = aplTypeRes;
////lpMemHdrRes->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpMemHdrRes->SysVar     = FALSE;           // Already zero from GHND
    lpMemHdrRes->RefCnt     = 1;
    lpMemHdrRes->NELM       = aplNELMRes;
    lpMemHdrRes->Rank       = aplRankRes;

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLockVar (hGlbRht);

    // Fill in the dimensions
    if (aplRankRes)
        CopyMemory (VarArrayBaseToDim (lpMemHdrRes),
                    VarArrayBaseToDim (lpMemHdrRht),
                    (APLU3264) (aplRankRht * sizeof (APLDIM)));
    // Fill in the last dimension
    (VarArrayBaseToDim (lpMemHdrRes))[aplRankRht] = aplColsRes;

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_INT:
        case ARRAY_FLOAT:
            // Copy the data to the result
            CopyMemory (lpMemRes, lpMemRht, (APLU3264) (aplNELMRht * TranslateArrayTypeToSizeof (aplTypeRht)));

            break;

        case ARRAY_RAT:
            // Loop through the result NELM
            for (uRht = 0; uRht < aplNELMRes; uRht++)
                // Save the RAT
                mpq_init_set (((LPAPLRAT) lpMemRes)++,
                              ((LPAPLRAT) lpMemRht)++);
            break;

        case ARRAY_VFP:
            // Loop through the result NELM
            for (uRht = 0; uRht < aplNELMRes; uRht++)
                // Save the VFP
                mpfr_init_set (((LPAPLVFP) lpMemRes)++,
                               ((LPAPLVFP) lpMemRht)++,
                               MPFR_RNDN);
            break;

        defstop
            break;
    } // End SWITCH

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
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
} // End SysFnMonDC_ToSimp_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydDC_EM_YY
//
//  Dyadic []DC -- Data Conversion
//
//  The left arg is one of 'I', 'F', 'R', or 'V' indicating the storage type
//    of the result with the same HC Dimension as the right arg.
//  The right arg is converted to the above storage type.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydDC_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydDC_EM_YY
    (LPTOKEN lptkLftArg,                    // Ptr to left arg token
     LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg)                    // Ptr to right arg token

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht,           // Right ...
                      aplTypeRes,           // Result   ...
                      aplTypeSimpRht,       // Right arg base simple type
                      aplTypeSimpRes;       // Result    ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht,           // Right ...
                      aplNELMRes,           // Result   ...
                      aplColsRht;           // Right arg # cols
    APLRANK           aplRankLft,           // Left arg rank
                      aplRankRht,           // Right ...
                      aplRankRes;           // Result   ...
    APLUINT           ByteRes;              // # bytes in the result
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    APLLONGEST        aplLongestLft;        // Left arg as immediate
    HGLOBAL           hGlbRht = NULL,       // Left arg global memory handle
                      hGlbRes = NULL;       // Result    ...
    LPVOID            lpMemRht,             // Ptr to right arg global memory
                      lpMemRes;             // ...    result    ...
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    UINT              uHCDimIndex;          // HC index (0, 1, 2, 3) for HC Dimension (1, 2, 4, 8)
    APLUINT           uLen,                 // # coefficients in the right arg
                      uRht;                 // Loop counter
    ALLTYPES          atRht = {0};          // Right arg as ALLTYPES

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL       );
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, &aplColsRht);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for LEFT LENGTH ERROR
    if (!IsSingleton (aplNELMLft))
        goto LEFT_LENGTH_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsSimpleChar (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if (!IsNumeric (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Get the first item in the left arg
    GetFirstItemToken (lptkLftArg, &aplLongestLft, NULL, NULL);

    // Get the HC Dimension index (0, 1, 2, 3) for (1, 2, 4, 8)
    uHCDimIndex = aArrayTypeToHCDimIndex[aplTypeRht];

    // Analyze the left arg
    switch (aplLongestLft)
    {
        case 'i':
        case 'I':
            // Get the result storage type
            aplTypeRes = uHCxI[uHCDimIndex];

            break;

        case 'f':
        case 'F':
            // Get the result storage type
            aplTypeRes = uHCxF[uHCDimIndex];

            break;

        case 'r':
        case 'R':
            // Get the result storage type
            aplTypeRes = uHCxR[uHCDimIndex];

            break;

        case 'v':
        case 'V':
            // Get the result storage type
            aplTypeRes = uHCxV[uHCDimIndex];

            break;

        default:
            goto LEFT_DOMAIN_EXIT;
    } // End SWITCH

    // Check for LEFT DOMAIN ERROR
    if (IsErrorType (aplTypeRes))
        goto LEFT_DOMAIN_EXIT;

    // Calculate the right arg & result base simple type
    aplTypeSimpRht = aToSimple[aplTypeRht];
    aplTypeSimpRes = aToSimple[aplTypeRes];

    // If the right arg is an APA, ...
    if (IsSimpleAPA (aplTypeRht))
        // Use that as the source type when converting the right arg
        aplTypeSimpRht = aplTypeRht;

    // Calculate the result NELM & rank
    aplNELMRes = aplNELMRht;
    aplRankRes = aplRankRht;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Fill in the header
    lpMemHdrRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpMemHdrRes->ArrType    = aplTypeRes;
////lpMemHdrRes->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpMemHdrRes->SysVar     = FALSE;           // Already zero from GHND
    lpMemHdrRes->RefCnt     = 1;
    lpMemHdrRes->NELM       = aplNELMRes;
    lpMemHdrRes->Rank       = aplRankRes;

    // Get the right arg global memory handle
    hGlbRht = GetGlbHandle (lptkRhtArg);

    // If the right arg is not immediate, ...
    if (hGlbRht NE NULL)
    {
        // Lock the memory to get a ptr to it
        lpMemHdrRht = MyGlobalLockVar (hGlbRht);

        // Fill in the dimensions
        if (aplRankRes)
            CopyMemory (VarArrayBaseToDim (lpMemHdrRes),
                        VarArrayBaseToDim (lpMemHdrRht),
                        (APLU3264) (aplRankRht * sizeof (APLDIM)));
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    } else
        // Point to the data
        lpMemRht = GetPtrTknLongest (lptkRhtArg);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Calculate the # coefficients in the right arg
    uLen = aplNELMRht * aHCDimIndexToHCDim[uHCDimIndex];

    // Indicate that conversions are exact
////atRes.enumCT = ENUMCT_NONE;         // Already set from = {0}

    // Loop through the right arg NELM
    for (uRht = 0; uRht < uLen; uRht++)
    {
        // Attempt to convert the next item in the right arg
        (*aTypeActConvert[aplTypeSimpRht][aplTypeSimpRes]) (lpMemRht, uRht, &atRht, NULL);

        // Split cases based upon the result base simple storage type
        switch (aplTypeSimpRes)
        {
            case ARRAY_INT:
                // Save as INT
                *((LPAPLINT) lpMemRes)++ = atRht.aplInteger;

                break;

            case ARRAY_FLOAT:
                // Save as FLOAT
                *((LPAPLFLOAT) lpMemRes)++ = atRht.aplFloat;

                break;

            case ARRAY_RAT:
                // Save as RAT
                *((LPAPLRAT) lpMemRes)++ = atRht.aplRat;

                break;

            case ARRAY_VFP:
                // Save as VFP
                *((LPAPLVFP) lpMemRes)++ = atRht.aplVfp;

                break;

            defstop
                break;
        } // End SWITCH

        // Zero the memory in case we might free it
        ZeroMemory (&atRht, sizeof (atRht));
    } // End FOR

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    //***************************************************************************
    //  Note that we can't afford to type demote as it might demote the
    //    dimension of a hypercomplex number with enough trailing zeros
    //    such as []DC 8{take}2
    //***************************************************************************
////// Unlock the result global memory in case TypeDemote actually demotes
////if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
////{
////    // We no longer need this ptr
////    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
////} // End IF
////
////// See if it fits into a lower (but not necessarily smaller) datatype
////TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

LEFT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes EQ NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbRes); hGlbRes = NULL;
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
} // End SysFnDydDC_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_dc.c
//***************************************************************************
