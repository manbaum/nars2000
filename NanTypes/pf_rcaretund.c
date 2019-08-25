//***************************************************************************
//  NARS2000 -- Primitive Function -- RightCaretUnderbar
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


#ifndef PROTO
PRIMSPEC PrimSpecRightCaretUnderbar = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecRightCaretUnderbarStorageTypeMon, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarAPA_EM, -- Can't happen w/RightCaretUnderbar

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonRightCaretUnderbarBisB, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarBisI, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarBisF, -- Can't happen w/RightCaretUnderbar

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonRightCaretUnderbarIisI, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarIisF, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarFisI, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarFisF, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarRisR, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarVisR, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarVisV, -- Can't happen w/RightCaretUnderbar

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonRightCaretUnderbarHC2IisHC2I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC2IisHC2F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC2FisHC2I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC2FisHC2F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC2RisHC2R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC2VisHC2R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC2VisHC2V, -- Can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnMonRightCaretUnderbarHC4IisHC4I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC4IisHC4F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC4FisHC4I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC4FisHC4F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC4RisHC4R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC4VisHC4R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC4VisHC4V, -- Can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnMonRightCaretUnderbarHC8IisHC8I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC8IisHC8F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC8FisHC8I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC8FisHC8F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC8RisHC8R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC8VisHC8R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC8VisHC8V, -- Can't happen w/RightCaretUnderbar

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonRightCaretUnderbarFisHC2I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarFisHC2F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarVisHC2R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarVisHC2V, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarFisHC4I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarFisHC4F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarVisHC4R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarVisHC4V, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarFisHC8I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarFisHC8F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarVisHC8R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarVisHC8V, -- Can't happen w/RightCaretUnderbar

    // Monadic BAxF result BAxF arg functions (indexable)
    NULL,   // &PrimFnMonRightCaretUnderbarBA1FisBA1F,
    NULL,   // &PrimFnMonRightCaretUnderbarBA2FisBA2F,
    NULL,   // &PrimFnMonRightCaretUnderbarBA4FisBA4F,
    NULL,   // &PrimFnMonRightCaretUnderbarBA8FisBA8F,
    NULL,   // &PrimFnMonRightCaretUnderbarARBisBA1F,
    NULL,   // &PrimFnMonRightCaretUnderbarARBisBA2F,
    NULL,   // &PrimFnMonRightCaretUnderbarARBisBA4F,
    NULL,   // &PrimFnMonRightCaretUnderbarARBisBA8F,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecRightCaretUnderbarStorageTypeDyd,
    NULL,   // &PrimFnDydRightCaretUnderbarAPA_EM, -- Can't happen w/RightCaretUnderbar

    // Dyadic Boolean result functions
    &PrimFnDydRightCaretUnderbarBisBvB,
    &PrimFnDydRightCaretUnderbarBisIvI,
    &PrimFnDydRightCaretUnderbarBisFvF,
    NULL,   // &PrimFnDydRightCaretUnderbarBisCvC, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHvH, -- Can't happen w/RightCaretUnderbar
    &PrimFnDydRightCaretUnderbarBisRvR,
    &PrimFnDydRightCaretUnderbarBisVvV,

    NULL,   // &PrimFnDydRightCaretUnderbarBisHC2IvHC2I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC2FvHC2F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC2RvHC2R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC2VvHC2V, -- Can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnDydRightCaretUnderbarBisHC4IvHC4I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC4FvHC4F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC4RvHC4R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC4VvHC4V, -- Can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnDydRightCaretUnderbarBisHC8IvHC8I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC8FvHC8F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC8RvHC8R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC8VvHC8V, -- Can't happen w/RightCaretUnderbar

    // Dyadic Boolean result functions with ARBs (indexable)
    &PrimFnDydRightCaretUnderbarBisBA1FvBA1F,
    NULL,   // &PrimFnDydRightCaretUnderbarBisBA2FvBA2F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisBA4FvBA4F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisBA8FvBA8F, -- Can't happen w/RightCaretUnderbar

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydRightCaretUnderbarIisIvI, -- Result Boolean, can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarIisFvF, -- Result Boolean, can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarFisIvI, -- Result Boolean, can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarFisFvF, -- Result Boolean, can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarRisRvR, -- Result Boolean, can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarVisRvR, -- Result Boolean, can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarVisVvV, -- Result Boolean, can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnDydRightCaretUnderbarHC2IisHC2IvHC2I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC2IisHC2FvHC2F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC2FisHC2IvHC2I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC2FisHC2FvHC2F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC2RisHC2RvHC2R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC2VisHC2RvHC2R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC2VisHC2VvHC2V, -- Can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnDydRightCaretUnderbarHC4IisHC4IvHC4I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC4IisHC4FvHC4F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC4FisHC4IvHC4I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC4FisHC4FvHC4F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC4RisHC4RvHC4R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC4VisHC4RvHC4R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC4VisHC4VvHC4V, -- Can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnDydRightCaretUnderbarHC8IisHC8IvHC8I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC8IisHC8FvHC8F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC8FisHC8IvHC8I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC8FisHC8FvHC8F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC8RisHC8RvHC8R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC8VisHC8RvHC8R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC8VisHC8VvHC8V, -- Can't happen w/RightCaretUnderbar

    // Dyadic BAxF result functions (indexable)
    NULL,   // &PrimFnDydRightCaretUnderbarBA1FisBA1FvBA1F,
    NULL,   // &PrimFnDydRightCaretUnderbarBA2FisBA2FvBA2F,
    NULL,   // &PrimFnDydRightCaretUnderbarBA4FisBA4FvBA4F,
    NULL,   // &PrimFnDydRightCaretUnderbarBA8FisBA8FvBA8F,

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonRightCaretUnderbarB64isB64, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarB32isB32, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarB16isB16, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarB08isB08, -- Can't happen w/RightCaretUnderbar

    // Dyadic Boolean chunk functions
    &PrimFnDydRightCaretUnderbarB64isB64vB64,
    &PrimFnDydRightCaretUnderbarB32isB32vB32,
    &PrimFnDydRightCaretUnderbarB16isB16vB16,
    &PrimFnDydRightCaretUnderbarB08isB08vB08,

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecRightCaretUnderbar};
#endif


//***************************************************************************
//  $PrimFnRightCaretUnderbar_EM_YY
//
//  Primitive function for monadic and dyadic RightCaretUnderbar (ERROR and "not less")
//***************************************************************************

LPPL_YYSTYPE PrimFnRightCaretUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharNotLess (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonRightCaretUnderbar_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY)    (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnRightCaretUnderbar_EM_YY


//***************************************************************************
//  $PrimFnMonRightCaretUnderbar_EM_YY
//
//  Primitive function for monadic RightCaretUnderbar ("Distract")
//
//  For 1-, 2-, 4-, or 8-dimension Real or HC arrays, split out the individual
//    Midpoint and Radius as a new length 2 dimension.
//  For all other arrays, signal an error.
//***************************************************************************

LPPL_YYSTYPE PrimFnMonRightCaretUnderbar_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    Assert (lptkFunc->tkData.tkChar EQ UTF16_RIGHTCARETUNDERBAR
         || lptkFunc->tkData.tkChar EQ INDEX_DOUBLETILDE_GE);

    // Call common outer routine
    return
      RightCaretUnd_MR2EP_Outer_EM_YY (lptkFunc,                // Ptr to function token
                                       lptkRhtArg,              // Ptr to right arg token
                                       lptkAxis,                // Ptr to axis token (may be NULL)
                                       RightCaretUnd_Inner);    // Ptr to common inner processing routine
} // End PrimFnMonRightCaretUnderbar_EM_YY


//***************************************************************************
//  $RightCaret_MR2EP_Outer_EM_YY
//
//  Outer routine common to <PrimFnMonRightCaretUnderbar_EM_YY> and
//    <PrimFnDoubleTildeMR2EP_EM_YY>
//***************************************************************************

LPPL_YYSTYPE RightCaretUnd_MR2EP_Outer_EM_YY
    (LPTOKEN lptkFunc,                                      // Ptr to function token
     LPTOKEN lptkRhtArg,                                    // Ptr to right arg token
     LPTOKEN lptkAxis,                                      // Ptr to axis token (may be NULL)
     RIGHTCARETUND_MR2EP_INNER RightCaretUnd_MR2EP_Inner)   // Ptr to common inner routine

{
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeRes;           // Result    ...
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMRes;           // Result    ...
    APLRANK           aplRankRht,           // Right arg rank
                      aplRankRes;           // Result    ...
    APLUINT           ByteRes,              // # bytes in the result
                      aplAxis,              // The (one and only) axis value
                      uDim,                 // Loop counter
                      uLo,                  // ...
                      uDimLo,               // ...
                      uDimAx,               // ...
                      uHi,                  // ...
                      uDimHi,               // ...
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

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Ensure the right arg is not a ptr arrray
    if (IsPtrArray (aplTypeRht))
        goto DOMAIN_EXIT;

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Distract allows a single integer axis value only
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRht + 1,  // All values less than this
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
        aplAxis = aplRankRht;
    } // End IF/ELSE

    // Get the right arg global memory handle
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the right arg is not an immediate, ...
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

    // The new axis is length 2
    uDimAx = 2;

    // Calculate the result storage type
    aplTypeRes = aToSimple[aplTypeRht];     // In case of APA

    // If the result is an ARB, ...
    if (IsRealArb (aplTypeRes))
        // Make it a VFP
        aplTypeRes = ARRAY_VFP;

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

    // If the result is not a scalar, ...
    if (!IsScalar (aplRankRht))
    {
        // Copy the dimension to the result
        CopyMemory ( lpMemDimRes             ,  lpMemDimRht         , (APLU3264) ((             aplAxis)    * sizeof (lpMemDimRht[0])));
        CopyMemory (&lpMemDimRes[aplAxis + 1], &lpMemDimRht[aplAxis], (APLU3264) ((aplRankRht - aplAxis)   * sizeof (lpMemDimRht[0])));
    } // End IF

    // Fill in the axis dimension
    lpMemDimRes[aplAxis] = uDimAx;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    __try
    {
        // Loop through the right arg copying the data to the result
        for (uRht = uLo = 0; uLo < uDimLo; uLo++)
        for (       uHi = 0; uHi < uDimHi; uHi++, uRht++)
        {
            // Calculate the index of the corresponding element in the result
            uDim = uLo * uDimHi * uDimAx + uHi;

            // Check for Ctrl-Break
            if (CheckCtrlBreak (lpbCtrlBreak))
                goto ERROR_EXIT;

            // Call common routine
            (*RightCaretUnd_MR2EP_Inner) (aplTypeRht,           // Right arg storage type
                                          uDim + 0 * uDimHi,    // Midpoint index into the result
                                          uDim + 1 * uDimHi,    // Radius   ...
                                          uRht,                 // Index into the right arg
                                          lpMemRht,             // Ptr to right arg global memory data
                                          lpMemRes);            // Ptr to result global memory data
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

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
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
} // End RightCaretUnd_MR2EP_Outer_EM_YY


//***************************************************************************
//  $RightCaretUnd_Inner
//
//  Inner routine for <RightCaretUnd_MR2EP_Outer_EM_YY>
//***************************************************************************

void RightCaretUnd_Inner
    (APLSTYPE aplTypeRht,       // Right arg storage type
     APLUINT  uRes0,            // uAX EQ 0 index into the result
     APLUINT  uRes1,            // ...    1 ...
     APLUINT  uRht,             // Index into the right arg
     LPVOID   lpMemRht,         // Ptr to right arg global memory data
     LPVOID   lpMemRes)         // Ptr to result global memory data

{
    int iSizeofRht;             // # bytes in each right arg item

    // Get the size of an item in the right arg
    iSizeofRht = TranslateArrayTypeToSizeof (aplTypeRht);

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            // Save the result's Midpoint
            ((LPAPLINT  ) lpMemRes)[uRes0] = GetNextInteger (lpMemRht, aplTypeRht, uRht);

////////////// Save the result's Radius (0)
////////////((LPAPLINT  ) lpMemRes)[uRes] = 0;      // Already zero from GHND

            break;

        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_FLOAT:
        {
            APLFLOAT      aplFltRht;            // Uninitialized temp
            APLFLOATUNION aplFloatUnion;        // Temporary union

            // Initialize
            aplFltRht = GetNextFloat (lpMemRht, aplTypeRht, uRht);

            // Save the FP value in the union so we can pick it apart
            aplFloatUnion.aplFloat = aplFltRht;

            // Save the result's Midpoint
            ((LPAPLFLOAT) lpMemRes)[uRes0] = aplFltRht;

            // If the argument is inexact, ...
             if (_finite (aplFltRht)
              && (fabs (aplFltRht) >= Float2Pow53           // Floats at or above 2*53 are by definition non-integral
               || (aplFloatUnion.aplFloatStr.uMantissa & 0xFF) NE 0))
                // Save the result's Radius
                ((LPAPLFLOAT) lpMemRes)[uRes1] = 1 / Float2Pow53;
////////////else
////////////////// Save the result's Radius (0)
////////////////((LPAPLFLOAT) lpMemRes)[uRes1] = 0.0;    // Already zero from GHND
            break;
        } // End ARRAY_FLOAT

        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_RAT:
            // Save the result's Midpoint
            mpq_init_set (&((LPAPLRAT  ) lpMemRes)[uRes0],
                          GetNextRatPtr_XFREE (lpMemRht, aplTypeRht, uRht));
            // Save the result's Radius (0)
            mpq_init     (&((LPAPLRAT  ) lpMemRes)[uRes1]);

            break;

        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_VFP:
        {
            LPAPLVFP lpaplVfpRht;       // Uninitialized temp

            // Initialize
            lpaplVfpRht = GetNextVfpPtr_XFREE (lpMemRht, aplTypeRht, uRht);

            // Save the VFP in the result
            mpfr_init_set (&((LPAPLVFP  ) lpMemRes)[uRes0],
                            lpaplVfpRht,
                            MPFR_RNDN);
            // Save the result's Radius (0)
            mpfr_init0    (&((LPAPLVFP  ) lpMemRes)[uRes1]);

            // If the argument is inexact, ...
            if (!mpfr_exact_p (lpaplVfpRht))
            {
////////////////APLVFP aplVfpTmp = {0};
////////////////
////////////////// Initialize to 0
////////////////mpfr_init0 (&aplVfpTmp);
////////////////
////////////////// Clear the flags
////////////////mpfr_clear_flags ();
////////////////
////////////////// Add 0 to the value
////////////////mpfr_add_si (&aplVfpTmp, &aplVfpRht, 0, MPFR_RNDN);
////////////////
////////////////// Test the inexact flag
////////////////if (mpfr_inexflag_p () NE 0)
                    // Save the result's Radius based upon the arg's precision
                    mpfr_set_si_2exp (&((LPAPLVFP) lpMemRes)[uRes1],
                                       1,
                                      -mpfr_get_prec (lpaplVfpRht),
                                       MPFR_RNDN);
////////////////// We no longer need this storage
////////////////Myf_clear (&aplVfpTmp);
            } // End IF

            break;
        } // End ARRAY_VFP

        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
            if (IzitImaginary (aplTypeRht, ByteAddr (lpMemRht, uRht * iSizeofRht)))
                // Signal an error
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Fall through to common code

        case ARRAY_ARB:
        {
            LPAPLARB lpaplArbRht;       // Uninitialized temp

            // Get the ARB from the right arg
            lpaplArbRht = GetNextArbPtr_XFREE (lpMemRht, aplTypeRht, uRht);

            // Initialize the results to 0
            mpfr_init0 (&((LPAPLVFP) lpMemRes)[uRes0]);
            mpfr_init0 (&((LPAPLVFP) lpMemRes)[uRes1]);

            // Convert the ARF to MPFR
            arf_get_mpfr2 (&((LPAPLVFP) lpMemRes)[uRes0], arb_midref (lpaplArbRht), MPFR_RNDN);

            // Convert the MAG to MPFR
            mag_get_mpfr  (&((LPAPLVFP) lpMemRes)[uRes1], arb_radref (lpaplArbRht), MPFR_RNDN);

            break;
        } // End ARRAY_ARB

        defstop
            break;
    } // End SWITCH
} // End RightCaretUnd_Inner


//***************************************************************************
//  $PrimSpecRightCaretUnderbarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecRightCaretUnderbarStorageTypeDyd
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
} // End PrimSpecRightCaretUnderbarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisBvB
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} B fn B
//***************************************************************************

void PrimFnDydRightCaretUnderbarBisBvB
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplBoolean >= lpatRht->aplBoolean)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydRightCaretUnderbarBisBvB


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarB64isB64vB64
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydRightCaretUnderbarB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydRightCaretUnderbarB64isB64vB64


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarB32isB32vB32
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydRightCaretUnderbarB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydRightCaretUnderbarB32isB32vB32


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarB16isB16vB16
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydRightCaretUnderbarB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydRightCaretUnderbarB16isB16vB16


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarB08isB08vB08
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydRightCaretUnderbarB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydRightCaretUnderbarB08isB08vB08


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisIvI
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} I fn I
//***************************************************************************

void PrimFnDydRightCaretUnderbarBisIvI
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplInteger >= lpatRht->aplInteger)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydRightCaretUnderbarBisIvI


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisFvF
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} F fn F
//***************************************************************************

void PrimFnDydRightCaretUnderbarBisFvF
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_F (lpatLft->aplFloat, lpatRht->aplFloat, GetQuadCT (), >=)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydRightCaretUnderbarBisFvF


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisRvR
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} R fn R
//***************************************************************************

void PrimFnDydRightCaretUnderbarBisRvR
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_R (lpatLft->aplRat, lpatRht->aplRat, GetQuadCT (), >=)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydRightCaretUnderbarBisRvR


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisVvV
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} V fn V
//***************************************************************************

void PrimFnDydRightCaretUnderbarBisVvV
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_V (lpatLft->aplVfp, lpatRht->aplVfp, GetQuadCT (), >=)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydRightCaretUnderbarBisVvV


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisBA1FvBA1F
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydRightCaretUnderbarBisBA1FvBA1F
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_B (lpatLft->aplArb, lpatRht->aplArb, GetQuadCT (), >=)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydRightCaretUnderbarBisBA1FvBA1F


//***************************************************************************
//  End of File: pf_rcaretund.c
//***************************************************************************
