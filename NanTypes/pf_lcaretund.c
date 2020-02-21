//***************************************************************************
//  NARS2000 -- Primitive Function -- LeftCaretUnderbar
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2020 Sudley Place Software

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
PRIMSPEC PrimSpecLeftCaretUnderbar = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecLeftCaretUnderbarStorageTypeMon, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarAPA_EM, -- Can't happen w/LeftCaretUnderbar

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonLeftCaretUnderbarBisB, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarBisI, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarBisF, -- Can't happen w/LeftCaretUnderbar

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonLeftCaretUnderbarIisI, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarIisF, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarFisI, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarFisF, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarRisR, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarVisR, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarVisV, -- Can't happen w/LeftCaretUnderbar

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonLeftCaretUnderbarHC2IisHC2I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC2IisHC2F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC2FisHC2I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC2FisHC2F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC2RisHC2R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC2VisHC2R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC2VisHC2V, -- Can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnMonLeftCaretUnderbarHC4IisHC4I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC4IisHC4F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC4FisHC4I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC4FisHC4F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC4RisHC4R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC4VisHC4R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC4VisHC4V, -- Can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnMonLeftCaretUnderbarHC8IisHC8I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC8IisHC8F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC8FisHC8I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC8FisHC8F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC8RisHC8R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC8VisHC8R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC8VisHC8V, -- Can't happen w/LeftCaretUnderbar

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonLeftCaretUnderbarFisHC2I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarFisHC2F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarVisHC2R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarVisHC2V, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarFisHC4I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarFisHC4F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarVisHC4R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarVisHC4V, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarFisHC8I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarFisHC8F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarVisHC8R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarVisHC8V, -- Can't happen w/LeftCaretUnderbar

    // Monadic BAxF result BAxF arg functions (indexable)
    NULL,   // &PrimFnMonLeftCaretUnderbarBA1FisBA1F,
    NULL,   // &PrimFnMonLeftCaretUnderbarBA2FisBA2F,
    NULL,   // &PrimFnMonLeftCaretUnderbarBA4FisBA4F,
    NULL,   // &PrimFnMonLeftCaretUnderbarBA8FisBA8F,
    NULL,   // &PrimFnMonLeftCaretUnderbarARBisBA1F,
    NULL,   // &PrimFnMonLeftCaretUnderbarARBisBA2F,
    NULL,   // &PrimFnMonLeftCaretUnderbarARBisBA4F,
    NULL,   // &PrimFnMonLeftCaretUnderbarARBisBA8F,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecLeftCaretUnderbarStorageTypeDyd,
    NULL,   // &PrimFnDydLeftCaretUnderbarAPA_EM, -- Can't happen w/LeftCaretUnderbar

    // Dyadic Boolean result functions
    &PrimFnDydLeftCaretUnderbarBisBvB,
    &PrimFnDydLeftCaretUnderbarBisIvI,
    &PrimFnDydLeftCaretUnderbarBisFvF,
    NULL,   // &PrimFnDydLeftCaretUnderbarBisCvC, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHvH, -- Can't happen w/LeftCaretUnderbar
    &PrimFnDydLeftCaretUnderbarBisRvR,
    &PrimFnDydLeftCaretUnderbarBisVvV,

    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC2IvHC2I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC2FvHC2F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC2RvHC2R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC2VvHC2V, -- Can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC4IvHC4I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC4FvHC4F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC4RvHC4R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC4VvHC4V, -- Can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC8IvHC8I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC8FvHC8F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC8RvHC8R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC8VvHC8V, -- Can't happen w/LeftCaretUnderbar

    // Dyadic Boolean result functions with ARBs (indexable)
    &PrimFnDydLeftCaretUnderbarBisBA1FvBA1F,
    NULL,   // &PrimFnDydLeftCaretUnderbarBisBA2FvBA2F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisBA4FvBA4F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisBA8FvBA8F, -- Can't happen w/LeftCaretUnderbar

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydLeftCaretUnderbarIisIvI, -- Result Boolean, can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarIisFvF, -- Result Boolean, can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarFisIvI, -- Result Boolean, can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarFisFvF, -- Result Boolean, can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarRisRvR, -- Result Boolean, can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarVisRvR, -- Result Boolean, can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarVisVvV, -- Result Boolean, can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnDydLeftCaretUnderbarHC2IisHC2IvHC2I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC2IisHC2FvHC2F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC2FisHC2IvHC2I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC2FisHC2FvHC2F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC2RisHC2RvHC2R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC2VisHC2RvHC2R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC2VisHC2VvHC2V, -- Can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnDydLeftCaretUnderbarHC4IisHC4IvHC4I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC4IisHC4FvHC4F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC4FisHC4IvHC4I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC4FisHC4FvHC4F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC4RisHC4RvHC4R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC4VisHC4RvHC4R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC4VisHC4VvHC4V, -- Can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnDydLeftCaretUnderbarHC8IisHC8IvHC8I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC8IisHC8FvHC8F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC8FisHC8IvHC8I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC8FisHC8FvHC8F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC8RisHC8RvHC8R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC8VisHC8RvHC8R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC8VisHC8VvHC8V, -- Can't happen w/LeftCaretUnderbar

    // Dyadic BAxF result functions (indexable)
    NULL,   // &PrimFnDydLeftCaretUnderbarBA1FisBA1FvBA1F,
    NULL,   // &PrimFnDydLeftCaretUnderbarBA2FisBA2FvBA2F,
    NULL,   // &PrimFnDydLeftCaretUnderbarBA4FisBA4FvBA4F,
    NULL,   // &PrimFnDydLeftCaretUnderbarBA8FisBA8FvBA8F,

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonLeftCaretUnderbarB64isB64, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarB32isB32, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarB16isB16, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarB08isB08, -- Can't happen w/LeftCaretUnderbar

    // Dyadic Boolean chunk functions
    &PrimFnDydLeftCaretUnderbarB64isB64vB64,
    &PrimFnDydLeftCaretUnderbarB32isB32vB32,
    &PrimFnDydLeftCaretUnderbarB16isB16vB16,
    &PrimFnDydLeftCaretUnderbarB08isB08vB08,

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecLeftCaretUnderbar};
#endif


//***************************************************************************
//  $PrimFnLeftCaretUnderbar_EM_YY
//
//  Primitive function for monadic and dyadic LeftCaretUnderbar (ERROR and "less than or equal")
//***************************************************************************

LPPL_YYSTYPE PrimFnLeftCaretUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharNotMore (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonLeftCaretUnderbar_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY)   (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnLeftCaretUnderbar_EM_YY


//***************************************************************************
//  $PrimFnMonLeftCaretUnderbar_EM_YY
//
//  Primitive function for monadic LeftCaretUnderbar ("Contract")
//
//  For length 2 coordinate numeric arrays (BOOL, INT, FLOAT, APA, RAT, VFP, or ARB),
//    convert the length 2 coordinate to the corresponding Ball Arithmetic number.
//  For all other arrays, signal an error.
//***************************************************************************

LPPL_YYSTYPE PrimFnMonLeftCaretUnderbar_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common outer routine
    return
      LeftCaretUnd_EP2MR_Outer_EM_YY (lptkFunc,             // Ptr to function token
                                      lptkRhtArg,           // Ptr to right arg token
                                      lptkAxis,             // Ptr to axis token (may be NULL)
                                      LeftCaretUnd_Inner);  // Ptr to common inner processing routine
} // End PrimFnMonLeftCaretUnderbar_EM_YY


//***************************************************************************
//  $LeftCaret_EP2MR_Outer_EM_YY
//
//  Outer routine common to <PrimFnMonLeftCaretUnderbar_EM_YY> and
//    <PrimFnDoubleTildeEP2MR_EM_YY>
//***************************************************************************

LPPL_YYSTYPE LeftCaretUnd_EP2MR_Outer_EM_YY
    (LPTOKEN lptkFunc,                                  // Ptr to function token
     LPTOKEN lptkRhtArg,                                // Ptr to right arg token
     LPTOKEN lptkAxis,                                  // Ptr to axis token (may be NULL)
     LEFTCARETUND_EP2MR_INNER LeftCaretUnd_EP2MR_Inner) // Ptr to common inner routine

{
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeRes;           // Result    ...
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMRes;           // Result    ...
    APLRANK           aplRankRht,           // Right arg rank
                      aplRankRes;           // Result    ...
    APLUINT           ByteRes,              // # bytes in the result
                      aplAxis,              // The (one and only) axis value
                      uDimAx,               // Length of the axis dimension
                      uDim,                 // Loop counter
                      uLo,                  // ...
                      uDimLo,               // ...
                      uHi,                  // ...
                      uDimHi,               // ...
                      uRes;                 // ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    HGLOBAL           hGlbRht,              // Right arg global memory handle
                      hGlbRes = NULL;       // Result    ...
    LPVOID            lpMemRht;             // Ptr to left arg global memory
    LPAPLARB          lpMemRes;             // Ptr to result    ...
    LPAPLDIM          lpMemDimRht,          // Ptr to right arg dimensions
                      lpMemDimRes;          // ...    result    ...
    APLLONGEST        aplLongestRht;        // Ptr to right arg immediate data
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    APLARB            aplArbTmp = {0};      // Temp
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Ensure the right arg is not a ptr arrray
    if (IsPtrArray (aplTypeRht))
        goto DOMAIN_EXIT;

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Contract allows a single integer axis value only
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

    // If the right arg is not immediate, ...
    if (hGlbRht NE NULL)
    {
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    } else
        // Point to the data
        lpMemRht = &aplLongestRht;

    // If the right arg is a scalar, ...
    if (IsScalar (aplRankRht))
        // Calculate the product of the axes below and above the axis dimension
        uDimLo = uDimAx = uDimHi = 1;
    else
    {
        // Loop through the axes below the axis dimension
        for (uDim = 0, uDimLo = 1; uDim < aplAxis; uDim++)
            // Calculate the product of the dimensions below the axis dimension
            uDimLo *= lpMemDimRht[uDim];

        // Get the length of the axis dimension
        uDimAx = lpMemDimRht[uDim++];

        // Ensure it's length 1 or 2
        if (uDimAx NE 1
         && uDimAx NE 2)
            goto LENGTH_EXIT;

        // Loop through the axes above the axis dimension
        for (uDimHi = 1; uDim < aplRankRht; uDim++)
            // Calculate the product of the dimensions above the axis dimension
            uDimHi *= lpMemDimRht[uDim];
    } // End IF/ELSE

    // Set the result storage type
    aplTypeRes = ARRAY_ARB;

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

    __try
    {
        // Loop through the right arg copying the data to the result
        for (uRes = uLo = 0; uLo < uDimLo; uLo++)
        for (       uHi = 0; uHi < uDimHi; uHi++)
        {
            uDim = uLo * uDimHi * uDimAx + uHi;

            // Check for Ctrl-Break
            if (CheckCtrlBreak (lpbCtrlBreak))
                goto ERROR_EXIT;

            // Call common routine
            if (!(*LeftCaretUnd_EP2MR_Inner) (aplTypeRht,
                                              uDim + 0 * uDimHi,
                                              uDim + 1 * uDimHi,
                                              uDimAx,
                                              lpMemRht,
                                             &lpMemRes[uRes]))
                goto DOMAIN_EXIT;

            // Skip to the next item in the result
            uRes++;
        } // End FOR/FOR
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
////////exCode = MyGetExceptionCode ();  // The exception code
////////
////////dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%s#%d)", MyGetExceptionStr (exCode), WFNLN);
////////
////////// Check the exception code in a helper function
////////CheckExCodeHelper (&exCode);
        goto DOMAIN_EXIT;
    } // End __try/__except

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

DOMAIN_EXIT:
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
} // End LeftCaretUndEP2MR_Outer_EM_YY


//***************************************************************************
//  $LeftCaretUnd_Inner
//
//  Inner routine for <LeftCaretUnd_EP2MR_Outer_EM_YY>
//***************************************************************************

UBOOL LeftCaretUnd_Inner
    (APLSTYPE aplTypeRht,       // Right arg storage type
     APLUINT  uRht0,            // uAX EQ 0 index into the right arg
     APLUINT  uRht1,            // ...    1 ...
     APLUINT  uDimAx,           // Length of the axis dimension
     LPVOID   lpMemRht,         // Ptr to right arg global memory data
     struct tagARB_STRUCT *lpMemRes)         // Ptr to result global memory data

{
    UINT     uBitMask0,         // Bit mask when looping through Booleans
             uBitMask1;         // ...
    LPAPLRAT lpaplRat;          // Temp
    LPAPLVFP lpaplVfp;          // ...
    LPAPLARB lpaplArb;          // ...
    int      iSizeofRht;        // # bytes in each right arg item

    // Get the size of an item in the right arg
    iSizeofRht = TranslateArrayTypeToSizeof (aplTypeRht);

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            uBitMask0  = BIT0 << (UINT) (uRht0 % NBIB);
            uBitMask1  = BIT0 << (UINT) (uRht1 % NBIB);

            // Convert the first element into an ARB Midpoint
            arb_init_set_sx   (            lpMemRes,  ((uBitMask0 & ((LPAPLBOOL) lpMemRht)[uRht0 >> LOG2NBIB]) ? 1   : 0  ));

            // If there's a Radius, ...
            if (uDimAx EQ 2)
            {
                if (uBitMask1 & ((LPAPLBOOL) lpMemRht)[uRht1 >> LOG2NBIB])
                    // Convert the second element into an ARB Radius
                    mag_one  (arb_radref (lpMemRes));
                else
                    // Convert the second element into an ARB Radius
                    mag_zero (arb_radref (lpMemRes));
            } // End IF

            break;

        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht0 * iSizeofRht))
             || IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht1 * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_INT:
        case ARRAY_APA:
            // Convert the first element into an ARB Midpoint
            arb_init_set_sx   (            lpMemRes,  GetNextInteger(lpMemRht, aplTypeRht, uRht0));

            // If there's a Radius, ...
            if (uDimAx EQ 2)
            {
                APLVFP aplVfpRad = {0};

                // Get the Radius as VFP
                aplVfpRad = GetNextVfp_INIT (lpMemRht, aplTypeRht, uRht1);

                // Convert the second element into an ARB Radius
                mag_set_mpfr (arb_radref (lpMemRes), &aplVfpRad);

                // We no longer need this storage
                mpfr_clear (&aplVfpRad);
             } // End IF

            break;

        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht0 * iSizeofRht))
             || IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht1 * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_FLOAT:
            // Convert the first element into an ARB Midpoint
            arb_init_set_d    (            lpMemRes,            ((LPAPLFLOAT) lpMemRht)[uRht0]);

            // If there's 1 Radius, ...
            if (uDimAx EQ 2)
                // Convert the second element into an ARB Radius
                mag_set_d2   (arb_radref (lpMemRes),              ((LPAPLFLOAT) lpMemRht)[uRht1]);
            else
                // Set the Radius to 2*-53
                mag_set_ui_2exp_si (arb_radref (lpMemRes), 1, -53);
            break;

        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht0 * iSizeofRht))
             || IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht1 * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_RAT:
            // Get the next RAT
            lpaplRat = GetNextRatPtr_XFREE (lpMemRht, aplTypeRht, uRht0);

            // Convert the first element into an ARB Midpoint
            arb_init_set_q    (            lpMemRes, lpaplRat);

            // If there's a Radius, ...
            if (uDimAx EQ 2)
            {
                APLVFP aplVfpRad = {0};

                // Initialize to 0
                mpfr_init0 (&aplVfpRad);

                // Get the next RAT
                lpaplRat = GetNextRatPtr_XFREE (lpMemRht, aplTypeRht, uRht1);

                // Convert RAT to VFP
                mpfr_set_q (&aplVfpRad, lpaplRat, MPFR_RNDN);

                // Convert the second element into an ARB Radius
                mag_set_mpfr (arb_radref (lpMemRes), &aplVfpRad);

                // We no longer need this storage
                mpfr_clear (&aplVfpRad);
            } // End IF

            break;

        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht0 * iSizeofRht))
             || IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht1 * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_VFP:
            // Get the next VFP
            lpaplVfp = GetNextVfpPtr_XFREE (lpMemRht, aplTypeRht, uRht0);

            // Convert the first element into an ARB Midpoint with its radius
            arb_init_set_mpfr (            lpMemRes, lpaplVfp);

            // If there's a Radius, ...
            if (uDimAx EQ 2)
            {
                // Get the next VFP
                lpaplVfp = GetNextVfpPtr_XFREE (lpMemRht, aplTypeRht, uRht1);

                // Convert the second element into an ARB Radius
                mag_set_mpfr (arb_radref (lpMemRes),lpaplVfp);
            } // End IF

            break;

        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht0 * iSizeofRht))
             || IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht1 * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_ARB:
            // Get the next ARB
            lpaplArb = GetNextArbPtr_XFREE (lpMemRht, aplTypeRht, uRht0);

            // Convert the first element into an ARB Midpoint
            arb_init_set      (            lpMemRes, lpaplArb);

            // If there's a Radius, ...
            if (uDimAx EQ 2)
            {
                APLVFP aplVfpRad = {0};

                // Initialize to 0
                mpfr_init0 (&aplVfpRad);

                // Get the next ARB
                lpaplArb = GetNextArbPtr_XFREE (lpMemRht, aplTypeRht, uRht1);

                // Convert ARB to VFP
                aplVfpRad = arb_get_mpfr (lpaplArb);

                // Convert the second element into an ARB Radius
                mag_set_mpfr (arb_radref (lpMemRes), &aplVfpRad);

                // We no longer need this storage
                mpfr_clear (&aplVfpRad);
            } // End IF

            break;

        defstop
            break;
    } // End SWITCH

    return TRUE;
} // End LeftCaretUnd_Inner


//***************************************************************************
//  $PrimSpecLeftCaretUnderbarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecLeftCaretUnderbarStorageTypeDyd
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

    // Is either arg HC?
    if (IsHCAny (*lpaplTypeLft)
     || IsHCAny (*lpaplTypeRht))
        // Tell the caller to demote the data to HC dimension 1
        return ARRAY_REALONLY;

    // Calculate the storage type of the result
    aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // All numeric results are Boolean
    if (IsNumeric (aplTypeRes))
        aplTypeRes = ARRAY_BOOL;

    return aplTypeRes;
} // End PrimSpecLeftCaretUnderbarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisBvB
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} B fn B
//***************************************************************************

void PrimFnDydLeftCaretUnderbarBisBvB
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplBoolean <= lpatRht->aplBoolean)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydLeftCaretUnderbarBisBvB


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarB64isB64vB64
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydLeftCaretUnderbarB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & ~aplBooleanRht);
} // End PrimFnDydLeftCaretUnderbarB64isB64vB64


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarB32isB32vB32
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydLeftCaretUnderbarB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & ~aplBooleanRht);
} // End PrimFnDydLeftCaretUnderbarB32isB32vB32


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarB16isB16vB16
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydLeftCaretUnderbarB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & ~aplBooleanRht);
} // End PrimFnDydLeftCaretUnderbarB16isB16vB16


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarB08isB08vB08
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydLeftCaretUnderbarB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & ~aplBooleanRht);
} // End PrimFnDydLeftCaretUnderbarB08isB08vB08


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisIvI
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} I fn I
//***************************************************************************

void PrimFnDydLeftCaretUnderbarBisIvI
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplInteger <= lpatRht->aplInteger)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydLeftCaretUnderbarBisIvI


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisFvF
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} F fn F
//***************************************************************************

void PrimFnDydLeftCaretUnderbarBisFvF
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_F (lpatLft->aplFloat, lpatRht->aplFloat, GetQuadCT (), <=)
    << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydLeftCaretUnderbarBisFvF


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisRvR
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} R fn R
//***************************************************************************

void PrimFnDydLeftCaretUnderbarBisRvR
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_R (lpatLft->aplRat, lpatRht->aplRat, GetQuadCT (), <=)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydLeftCaretUnderbarBisRvR


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisVvV
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} V fn V
//***************************************************************************

void PrimFnDydLeftCaretUnderbarBisVvV
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_V (lpatLft->aplVfp, lpatRht->aplVfp, GetQuadCT (), <=)
       << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydLeftCaretUnderbarBisVvV


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisBA1FvBA1F
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydLeftCaretUnderbarBisBA1FvBA1F
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    Left ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_B (lpatLft->aplArb, lpatRht->aplArb, GetQuadCT (), >=)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydLeftCaretUnderbarBisBA1FvBA1F


//***************************************************************************
//  End of File: pf_lcaretund.c
//***************************************************************************
