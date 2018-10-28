//***************************************************************************
//  NARS2000 -- System Function -- Quad DC
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
//  $SysFnDC_EM_YY
//
//  System function:  []DC -- Data Conversion
//***************************************************************************

LPPL_YYSTYPE SysFnDC_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonDC_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydDC_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnDC_EM_YY


//***************************************************************************
//  $SysFnMonDC_EM_YY
//
//  Monadic []DC -- Data Conversion
//
//  For 1-dimension numeric arrays (BOOL, INT, FLOAT, APA, RAT, VFP, or ARB),
//    if the # cols is 1, 2, 4, or 8, convert the arg to the corresponding
//    HyperComplex number.
//  For 2-, 4-, or 8-dimension HC arrays, split out the individual
//    coefficients as a new (column) dimension.
//  For all other arrays, signal an error.
//***************************************************************************

LPPL_YYSTYPE SysFnMonDC_EM_YY
    (LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)
{
    APLSTYPE aplTypeRht;                // Right arg storage type
    APLNELM  aplNELMRht;                // ...       NELM
    APLRANK  aplRankRht;                // ...       rank

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL :
        case ARRAY_INT  :
        case ARRAY_FLOAT:
        case ARRAY_APA  :
        case ARRAY_RAT  :
        case ARRAY_VFP  :
        case ARRAY_ARB  :
            return SysFnMonDC_ToHC_EM_YY (lptkRhtArg,       // Ptr to right arg token
                                          aplTypeRht,       // Right arg storage type
                                          aplNELMRht,       // ...       NELM
                                          aplRankRht,       // ...       rank
                                          lptkFunc,         // Ptr to function token
                                          lptkAxis);        // Ptr to axis token (may be NULL)
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

        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
            return SysFnMonDC_ToSimp_EM_YY (lptkRhtArg,     // Ptr to right arg token
                                            aplTypeRht,     // Right arg storage type
                                            aplNELMRht,     // ...       NELM
                                            aplRankRht,     // ...       rank
                                            lptkFunc,       // Ptr to function token
                                            lptkAxis);      // Ptr to axis token (may be NULL)
        case ARRAY_CHAR  :
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


//***************************************************************************
//  $SysFnMonDC_ToHC_EM_YY
//
//  The right arg is simple numeric (BOOL, INT, FLOAT, APA, RAT, VFP, or ARB), and
//    the # cols is either 1, 2, 4, or 8, so convert it to the corresponding
//    HyperComplex number.
//***************************************************************************

LPPL_YYSTYPE SysFnMonDC_ToHC_EM_YY
    (LPTOKEN  lptkRhtArg,           // Ptr to right arg token
     APLSTYPE aplTypeRht,           // Right arg storage type
     APLNELM  aplNELMRht,           // ...       NELM
     APLRANK  aplRankRht,           // ...       rank
     LPTOKEN  lptkFunc,             // Ptr to function token
     LPTOKEN  lptkAxis)             // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeRes;           // Result storage type
    APLNELM           aplNELMRes;           // Result NELM
    APLRANK           aplRankRes;           // Result rank.
    APLUINT           ByteRes,              // # bytes in the result
                      aplAxis,              // The (one and only) axis value
                      uDim,                 // Loop counter
                      uLo,                  // ...
                      uDimLo,               // ...
                      uAx,                  // ...
                      uDimAx,               // ...
                      uHi,                  // ...
                      uDimHi,               // ...
                      uRes,                 // ...
                      uRht;                 // ...
    UINT              uHCDimIndex,          // HC Dimension index (0, 1, 2, 3) for (1, 2, 4, 8)
                      uBitMask;             // Bit mask when looping through Booleans
    LPAPLDIM          lpMemDimRes,          // Ptr to result    dimensions
                      lpMemDimRht;          // ...    right arg ...
    APLLONGEST        aplLongestRht;        // Ptr to right arg immediate data
    LPVARARRAY_HEADER lpMemHdrRes = NULL,   // Ptr to result header
                      lpMemHdrRht = NULL;   // ...    right arg ...
    HGLOBAL           hGlbRht,              // Right arg global memory handle
                      hGlbRes = NULL;       // Result    ...
    LPVOID            lpMemRht,             // Ptr to left arg global memory
                      lpMemRes;             // Ptr to result    ...
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    int               iSizeofRht;           // # bytes in each right arg item

    // Get the size of an item in the right arg
    iSizeofRht = TranslateArrayTypeToSizeof (aplTypeRht);

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // []DC allows a single integer axis value only
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRht,      // All values less than this
                           TRUE,            // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // Return TRUE iff fractional values present
                          &aplAxis,         // Return last axis value
                           NULL,            // Return # elements in axis vector
                           NULL))           // Return HGLOBAL with APLINT axis values
            goto ERROR_EXIT;
    } else
    {
        // No axis specified:  use last axis
        aplAxis = aplRankRht - 1;
    } // End IF/ELSE

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

    // If the right arg is a scalar, ...
    if (IsScalar (aplRankRht))
    {
        // Calculate the product of the axes below and above the axis dimension
        uDimLo = uDimAx = uDimHi = 1;

        // Point to the data
        lpMemRht = &aplLongestRht;
    } else
    {
        // Loop through the axes below the axis dimension
        for (uDim = 0, uDimLo = 1; uDim < aplAxis; uDim++)
            // Calculate the product of the dimensions below the axis dimension
            uDimLo *= lpMemDimRht[uDim];

        // Get the length of the axis dimension
        uDimAx = lpMemDimRht[uDim++];

        // Loop through the axes above the axis dimension
        for (uDimHi = 1; uDim < aplRankRht; uDim++)
            // Calculate the product of the dimensions above the axis dimension
            uDimHi *= lpMemDimRht[uDim];

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    } // End IF/ELSE

    // If <uDimAx> is out of range, ...
    if (uDimAx >= countof (iHCDimLog2))
        goto RIGHT_LENGTH_EXIT;

    // Get the HC Dimension index of the result (0, 1, 2, 3) for (1, 2, 4, 8)
    uHCDimIndex = iHCDimLog2[uDimAx];

    // Check for error
    if (uHCDimIndex EQ -1)
        goto RIGHT_LENGTH_EXIT;

    Assert (uDimAx EQ 1
         || uDimAx EQ 2
         || uDimAx EQ 4
         || uDimAx EQ 8);

    //***************************************************************
    // The Hypercomplex cases in the SWITCH stmt below may occur when this
    //    routine is called from Monadic LeftCaret, not from []DC.
    //***************************************************************

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT :
        case ARRAY_APA :
        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
            // Get the result storage type
            aplTypeRes = uHCxI[uHCDimIndex];

            break;

        case ARRAY_FLOAT:
        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
            // Get the result storage type
            aplTypeRes = uHCxF[uHCDimIndex];

            break;

        case ARRAY_RAT:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            // Get the result storage type
            aplTypeRes = uHCxR[uHCDimIndex];

            break;

        case ARRAY_VFP:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            // Get the result storage type
            aplTypeRes = uHCxV[uHCDimIndex];

            break;

        case ARRAY_ARB:
        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
            // Get the result storage type
            aplTypeRes = uBAxF[uHCDimIndex];

            break;

        defstop
            break;
    } // End SWITCH

    // Calculate the result NELM
    aplNELMRes = aplNELMRht / uDimAx;

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

    // Skip over the header to the dimensions
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // If the result is not a scalar, ...
    if (!IsScalar (aplRankRes))
    {
        // Copy the dimension to the result
        CopyMemory ( lpMemDimRes         ,  lpMemDimRht             , (APLU3264) (              aplAxis      * sizeof (lpMemDimRht[0])));
        CopyMemory (&lpMemDimRes[aplAxis], &lpMemDimRht[aplAxis + 1], (APLU3264) ((aplRankRht - aplAxis - 1) * sizeof (lpMemDimRht[0])));
    } // End IF

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    //***************************************************************
    // The Hypercomplex cases in the SWITCH stmt below may occur when this
    //    routine is called from Monadic LeftCaret, not from []DC.
    //***************************************************************

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            // Loop through the right arg copying the data to the result
            for (uRes = uLo = 0; uLo < uDimLo; uLo++)
            for (       uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Calculate the indices into the right arg
                    uRht = uDim + uAx * uDimHi;
                    uBitMask  = BIT0 << (UINT) (uRht % NBIB);

                    // Save the INT in the result
                    ((LPAPLINT) lpMemRes)[uRes++] =
                      ((uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? TRUE : FALSE);
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
            // Loop through the right arg copying the data to the result
            for (uRes = uLo = 0; uLo < uDimLo; uLo++)
            for (       uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Calculate the indices into the right arg
                    uRht = uDim + uAx * uDimHi;

                    // If it has an non-zero Imagainary component, ...
                    if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht * iSizeofRht)))
                        goto RIGHT_DOMAIN_EXIT;

                    // Save the INT in the result
                    ((LPAPLINT  ) lpMemRes)[uRes++] =
                      GetNextInteger (lpMemRht, aplTypeRht, uRht);
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_FLOAT:
        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
            // Loop through the right arg copying the data to the result
            for (uRes = uLo = 0; uLo < uDimLo; uLo++)
            for (       uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Calculate the indices into the right arg
                    uRht = uDim + uAx * uDimHi;

                    // If it has an non-zero Imagainary component, ...
                    if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht * iSizeofRht)))
                        goto RIGHT_DOMAIN_EXIT;

                    // Save the FLT in the result
                    ((LPAPLFLOAT) lpMemRes)[uRes++] =
                    ((LPAPLFLOAT) lpMemRht)[uRht];
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_RAT:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            // Loop through the right arg copying the data to the result
            for (uRes = uLo = 0; uLo < uDimLo; uLo++)
            for (       uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Calculate the indices into the right arg
                    uRht = uDim + uAx * uDimHi;

                    // If it has an non-zero Imagainary component, ...
                    if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht * iSizeofRht)))
                        goto RIGHT_DOMAIN_EXIT;

                    // Save the RAT in the result
                    mpq_init_set (&((LPAPLRAT  ) lpMemRes)[uRes++],
                                  &((LPAPLRAT  ) lpMemRht)[uRht]);
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_VFP:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            // Loop through the right arg copying the data to the result
            for (uRes = uLo = 0; uLo < uDimLo; uLo++)
            for (       uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Calculate the indices into the right arg
                    uRht = uDim + uAx * uDimHi;

                    // If it has an non-zero Imagainary component, ...
                    if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht * iSizeofRht)))
                        goto RIGHT_DOMAIN_EXIT;

                    // Save the VFP in the result
                    mpfr_init_set (&((LPAPLVFP  ) lpMemRes)[uRes++],
                                   &((LPAPLVFP  ) lpMemRht)[uRht],
                                    MPFR_RNDN);
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_ARB:
        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
            // Loop through the right arg copying the data to the result
            for (uRes = uLo = 0; uLo < uDimLo; uLo++)
            for (       uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Calculate the indices into the right arg
                    uRht = uDim + uAx * uDimHi;

                    // If it has an non-zero Imagainary component, ...
                    if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht * iSizeofRht)))
                        goto RIGHT_DOMAIN_EXIT;

                    // Save the ARB in the result
                    arb_init_set (&((LPAPLARB  ) lpMemRes)[uRes++],
                                  &((LPAPLARB  ) lpMemRht)[uRht]);
                } // End FOR
            } // End FOR/FOR

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
////TypeDemote (&lpYYRes->tkToken, FALSE);

    goto NORMAL_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
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


//***************************************************************************
//  $SysFnMonDC_ToSimp_EM_YY
//
//  The right arg is Real or HyperComplex (HCxI, HCxF, HCxR, or HCxV) so convert it
//    to the corresponding simple numeric (INT, FLOAT, RAT, VFP, or ARB) array
//    with a new column dimension of 2, 4, or 8 as per the Dimension of
//    the right arg.
//***************************************************************************

LPPL_YYSTYPE SysFnMonDC_ToSimp_EM_YY
    (LPTOKEN  lptkRhtArg,           // Ptr to right arg token
     APLSTYPE aplTypeRht,           // Right arg storage type
     APLNELM  aplNELMRht,           // ...       NELM
     APLRANK  aplRankRht,           // ...       rank
     LPTOKEN  lptkFunc,             // Ptr to function token
     LPTOKEN  lptkAxis)             // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeRes;           // Result storage type
    APLNELM           aplNELMRes;           // Result NELM
    APLRANK           aplRankRes;           // Result rank.
    APLUINT           ByteRes,              // # bytes in the result
                      aplAxis,              // The (one and only) axis value
                      uDim,                 // Loop counter
                      uLo,                  // ...
                      uDimLo,               // ...
                      uAx,                  // ...
                      uDimAx,               // ...
                      uHi,                  // ...
                      uDimHi,               // ...
                      uRes,                 // ...
                      uRht;                 // ...
    LPVARARRAY_HEADER lpMemHdrRes = NULL,   // Ptr to result header
                      lpMemHdrRht = NULL;   // ...    right arg ...
    HGLOBAL           hGlbRht,              // Right arg global memory handle
                      hGlbRes = NULL;       // Result    ...
    LPVOID            lpMemRht,             // Ptr to left arg global memory
                      lpMemRes;             // Ptr to result    ...
    LPAPLDIM          lpMemDimRes,          // Ptr to result    dimensions
                      lpMemDimRht;          // ...    right arg ...
    APLLONGEST        aplLongestRht;        // Data if right arg is immediate
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the right arg global memory handle
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Calculate the result axis length (= right arg HC Dimension)
    uDimAx = TranslateArrayTypeToHCDim (aplTypeRht);

    Assert (uDimAx EQ 1
         || uDimAx EQ 2
         || uDimAx EQ 4
         || uDimAx EQ 8);

    // Calculate the result storage type
    if (IsSimpleAPA (aplTypeRht))
        aplTypeRes = aplTypeRht;
    else
        aplTypeRes = aToSimple[aplTypeRht];

    // Calculate the result NELM
    aplNELMRes = aplNELMRht * uDimAx;

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

    // Skip over the header to the dimensions
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Check the axis values, fill in # elements in axis
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRes,      // All values less than this
                           TRUE,            // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // TRUE iff fractional values allowed
                          &aplAxis,         // Return last axis value
                           NULL,            // Return # elements in axis vector
                           NULL))           // Return HGLOBAL with APLINT axis values
            goto ERROR_EXIT;
    } else
    {
        // No axis specified:  use last axis
        aplAxis = aplRankRes - 1;
    } // End IF/ELSE

    // If the result is not a scalar, ...
    if (!IsScalar (aplRankRht))
    {
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

        // Copy the dimension to the result
        CopyMemory ( lpMemDimRes             ,  lpMemDimRht         , (APLU3264) ((             aplAxis)    * sizeof (lpMemDimRht[0])));
        CopyMemory (&lpMemDimRes[aplAxis + 1], &lpMemDimRht[aplAxis], (APLU3264) ((aplRankRht - aplAxis)   * sizeof (lpMemDimRht[0])));
    } // End IF

    // Fill in the axis dimension
    lpMemDimRes[aplAxis] = uDimAx;

    // If the right arg is not an immediate, ...
    if (hGlbRht NE NULL)
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        // Point to the data
        lpMemRht = &aplLongestRht;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // If the right arg is a scalar, ...
    if (IsScalar (aplRankRht))
        // Calculate the product of the axes below and above the axis dimension
        uDimLo = uDimHi = 1;
    else
    {
        // Loop through the axes below the axis dimension
        for (uDim = 0, uDimLo = 1; uDim < aplAxis; uDim++)
            // Calculate the product of the dimensions below the axis dimension
            uDimLo *= lpMemDimRht[uDim];

        // Loop through the axes above the axis dimension
        for (uDimHi = 1; uDim < aplRankRht; uDim++)
            // Calculate the product of the dimensions above the axis dimension
            uDimHi *= lpMemDimRht[uDim];
    } // End IF/ELSE

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_INT:
            // Loop through the right arg copying the data to the result
            for (uRht = uLo = 0; uLo < uDimLo; uLo++)
            for (       uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Calculate the indices into the result
                    uRes = uDim + uAx * uDimHi;

                    // Save the INT in the result
                    ((LPAPLINT  ) lpMemRes)[uRes] =
                    ((LPAPLINT  ) lpMemRht)[uRht++];
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_FLOAT:
            // Loop through the right arg copying the data to the result
            for (uRht = uLo = 0; uLo < uDimLo; uLo++)
            for (       uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Calculate the indices into the result
                    uRes = uDim + uAx * uDimHi;

                    // Save the FLT in the result
                    ((LPAPLFLOAT) lpMemRes)[uRes] =
                    ((LPAPLFLOAT) lpMemRht)[uRht++];
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_RAT:
            // Loop through the right arg copying the data to the result
            for (uRht = uLo = 0; uLo < uDimLo; uLo++)
            for (       uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Calculate the indices into the result
                    uRes = uDim + uAx * uDimHi;

                    // Save the RAT in the result
                    mpq_init_set (&((LPAPLRAT  ) lpMemRes)[uRes],
                                  &((LPAPLRAT  ) lpMemRht)[uRht++]);
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_VFP:
            // Loop through the right arg copying the data to the result
            for (uRht = uLo = 0; uLo < uDimLo; uLo++)
            for (       uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Calculate the indices into the result
                    uRes = uDim + uAx * uDimHi;

                    // Save the VFP in the result
                    mpfr_init_set (&((LPAPLVFP  ) lpMemRes)[uRes],
                                   &((LPAPLVFP  ) lpMemRht)[uRht++],
                                    MPFR_RNDN);
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_ARB:
            // Loop through the right arg copying the data to the result
            for (uRht = uLo = 0; uLo < uDimLo; uLo++)
            for (       uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Calculate the indices into the result
                    uRes = uDim + uAx * uDimHi;

                    // Save the ARB in the result
                    arb_init_set (&((LPAPLARB  ) lpMemRes)[uRes],
                                  &((LPAPLARB  ) lpMemRht)[uRht++]);
                } // End FOR
            } // End FOR/FOR

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


//***************************************************************************
//  $SysFnDydDC_EM_YY
//
//  Dyadic []DC -- Data Conversion
//
//  The left arg is one of 'I', 'F', 'R', or 'V' indicating the storage type
//    of the result with the same HC Dimension as the right arg.
//  The right arg is converted to the above storage type.
//***************************************************************************

LPPL_YYSTYPE SysFnDydDC_EM_YY
    (LPTOKEN lptkLftArg,                    // Ptr to left arg token
     LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis)                      // Ptr to axis token (may be NULL)

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
    UBOOL             bRet;                 // TRUE iff the result is valid

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

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

        case 'b':
        case 'B':
            // Get the result storage type
            aplTypeRes = uBAxF[uHCDimIndex];

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
        (*aTypeActConvert[aplTypeSimpRht][aplTypeSimpRes]) (lpMemRht, uRht, &atRht, &bRet);

        // If it's invalid, ...
        if (!bRet)
            goto RIGHT_DOMAIN_EXIT;

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

            case ARRAY_ARB:
                // Save as ARB
                *((LPAPLARB) lpMemRes)++ = atRht.aplArb;

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
////TypeDemote (&lpYYRes->tkToken, FALSE);

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

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
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


//***************************************************************************
//  End of File: qf_dc.c
//***************************************************************************
