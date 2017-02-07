//***************************************************************************
//  NARS2000 -- Primitive Function -- CircleStile
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
#include "headers.h"


//***************************************************************************
//  $PrimFnCircleStile_EM_YY
//
//  Primitive function for monadic and dyadic CircleStile ("reverse" and "rotate")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnCircleStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnCircleStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_CIRCLESTILE
         || lptkFunc->tkData.tkChar EQ UTF16_CIRCLEBAR);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonCircleStile_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydCircleStile_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnCircleStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnCircleStile_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic CircleStile
//***************************************************************************

LPPL_YYSTYPE PrimProtoFnCircleStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnCircleStile_EM_YY,// Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnCircleStile_EM_YY


//***************************************************************************
//  $PrimIdentFnCircleStile_EM_YY
//
//  Generate an identity element for the primitive function dyadic CircleStile
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimIdentFnCircleStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimIdentFnCircleStile_EM_YY
    (LPTOKEN lptkRhtOrig,           // Ptr to original right arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to result
    APLNELM      aplNELMRht;        // Right arg NELM
    APLRANK      aplRankRht;        // Right arg rank
    APLUINT      aplAxis;           // The (one and only) axis value
    HGLOBAL      hGlbRht = NULL;    // Right arg global memory handle
    LPVOID       lpMemRht = NULL;   // Ptr to right arg global memory
    LPAPLDIM     lpMemDimRht;       // Ptr to right arg dimensions

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig NE NULL);
    Assert (lptkFunc    NE NULL);
    Assert (lptkRhtArg  NE NULL);

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, NULL, &aplNELMRht, &aplRankRht, NULL);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Rotate allows a single integer axis value only
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
        // No axis specified:
        //   if CircleStile, use last dimension
        if (lptkFunc->tkData.tkChar EQ UTF16_CIRCLESTILE)
            aplAxis = aplRankRht - 1;
        else
        {
            Assert (lptkFunc->tkData.tkChar EQ UTF16_CIRCLEBAR);

            // Otherwise, it's CircleBar on the first dimension
            aplAxis = 0;
        } // End IF/ELSE
    } // End IF/ELSE

    // The (left) identity element for dyadic CircleStile
    //   (L {circlestile} R) ("rotate") is
    //   (({neg}1{drop}{rho}R){rho}0.

    // The (left) identity element for dyadic CircleBar
    //   (L {circlebar} R) ("rotate first") is
    //   ((1{drop}{rho}R){rho}0.

    // The (left) identity element for dyadic CircleStile
    //   (L {circlestile}[X] R) ("rotate w/axis") is
    //   ((X{ne}{iota}{rho}{rho}R)/{rho}R){rho}0.

    // If the right arg is a scalar, ...
    if (IsScalar (aplRankRht))
    {
        // The result is a scalar zero

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken                   = tkZero;
////////lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;  // Already set by tkZero
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_BOOL;  // Already set by tkZero
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already set by tkZero
////////lpYYRes->tkToken.tkData.tkBoolean  = FALSE;         // Already set by tkZero
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } else
    {
        APLNELM  aplNELMRes;            // Result NELM
        APLUINT  ByteRes;               // # bytes in the result
        HGLOBAL  hGlbRes = NULL;        // Result global memory handle
        LPVOID   lpMemRes = NULL;       // Ptr to result global memory
        LPAPLDIM lpMemDimRes;           // Ptr to result dimensions
        APLUINT  uRht;                  // Loop counter

        // The result is a rank (aplRankRht - 1) Boolean array of zeros

        // Get the right arg global memory handle
        hGlbRht = lptkRhtArg->tkData.tkGlbData;

        // Lock the memory to get a ptr to it
        lpMemRht = MyGlobalLockVar (hGlbRht);

        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

        // The aplNELMRes is aplNELMRht / lpMemDimRht[aplAxis]
        for (uRht = 0, aplNELMRes = 1; uRht < aplRankRht; uRht++)
        if (uRht NE aplAxis)
            aplNELMRes *= lpMemDimRht[uRht];

        // Calculate space needed for the result
        ByteRes = CalcArraySize (ARRAY_BOOL, aplNELMRes, aplRankRht - 1);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Allocate storage for the result
        hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
        // Fill in the header values
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_BOOL;
////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMRes;
        lpHeader->Rank       = aplRankRht - 1;
#undef  lpHeader

        // Skip over the header to the dimensions
        lpMemDimRes = VarArrayBaseToDim (lpMemRes);

        // Copy the dimensions from the right arg
        //   omitting the aplAxis dimension
        for (uRht = 0; uRht < aplRankRht; uRht++)
        if (uRht NE aplAxis)
            *lpMemDimRes++ = lpMemDimRht[uRht];

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_BOOL;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already set by tkZero
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbRht)
    {
        if (lpMemRht)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
        } // End IF
    } // End IF

    return lpYYRes;
} // End PrimIdentFnCircleStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonCircleStile_EM_YY
//
//  Primitive function for monadic CircleStile ("reverse")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonCircleStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonCircleStile_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeRht,       // Right arg storage type
                      aplTypeRes;       // Result    ...
    APLNELM           aplNELMRht;       // Right arg NELM
    APLRANK           aplRankRht;       // Right arg rank
    HGLOBAL           hGlbRht = NULL,   // Right arg global memory handle
                      hGlbRes = NULL;   // Result    ...
    LPVOID            lpMemRht = NULL,  // Ptr to right arg global memory
                      lpMemRes = NULL;  // Ptr to result    ...
    LPAPLDIM          lpMemDimRht;      // Ptr to right arg dimensions
    LPVARARRAY_HEADER lpMemHdrRht,      // Ptr to right arg header
                      lpMemHdrRes;      // Ptr to result ...
    APLUINT           aplAxis,          // The (one and only) axis value
                      uDim,             // Loop counter
                      uLo,              // ...
                      uDimLo,           // ...
                      uAx,              // ...
                      uDimAx,           // ...
                      uHi,              // ...
                      uDimHi,           // ...
                      uRes,             // ...
                      uRht;             // ...
    APLINT            apaOffRht,        // Right arg APA offset
                      apaMulRht;        // Right arg APA multiplier
    LPPL_YYSTYPE      lpYYRes = NULL;   // Ptr to the result
    APLUINT           ByteRes;          // # bytes in the result
    UINT              uBitMask,         // Bit mask when looping through Booleans
                      uBitIndex;        // Bit index ...
    LPPLLOCALVARS     lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;     // Ptr to Ctrl-Break flag
    int               iHCDimRht,        // HC Dimension (1, 2, 4, 8)
                      i;                // Loop counter

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Reverse allows a single integer axis value only
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
            return NULL;
    } else
    {
        // No axis specified:
        //   if CircleStile, use last dimension
        if (lptkFunc->tkData.tkChar EQ UTF16_CIRCLESTILE)
            aplAxis = aplRankRht - 1;
        else
        {
            Assert (lptkFunc->tkData.tkChar EQ UTF16_CIRCLEBAR);

            // Otherwise, it's CircleBar on the first dimension
            aplAxis = 0;
        } // End IF/ELSE
    } // End IF/ELSE

    // Check for singleton
    if (IsSingleton (aplNELMRht))
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Copy the right arg token
        lpYYRes->tkToken = *CopyToken_EM (lptkRhtArg, FALSE);

        goto IMMED_EXIT;
    } // End IF

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Skip over the header to the dimensions
    lpMemRht = lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

    // The type of the result is the same as the
    //   type of the right arg except APA -> INT
    if (IsSimpleAPA (aplTypeRht))
    {
        // The reverse of an APV is an APV
        if (IsVector (aplRankRht))
            aplTypeRes = aplTypeRht;
        else
            aplTypeRes = ARRAY_INT;
    } else
        aplTypeRes = aplTypeRht;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate storage for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->PV0        = lpMemHdrRht->PV0;
    lpHeader->PV1        = lpMemHdrRht->PV1;
    lpHeader->bSelSpec   = lpMemHdrRht->bSelSpec;
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemHdrRes);

    // Fill in the dimensions
    for (uDim = 0; uDim < aplRankRht; uDim++)
        *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;

    // Copy the data to the result

    // Handle prototypes
    if (IsEmpty (aplNELMRht))
    {
        if (IsNested (aplTypeRht))
            *((LPAPLNESTED) lpMemRes) = CopySymGlbInd_PTB (lpMemRht);
        goto NORMAL_EXIT;
    } // End IF

    if (IsScalar (aplRankRht))
        uDimLo = uDimAx = uDimHi = 1;
    else
    {
        // Calculate the product of the dimensions below the axis dimension
        for (uDim = 0, uDimLo = 1; uDim < aplAxis; uDim++)
            uDimLo *= lpMemDimRht[uDim];

        // Get the length of the axis dimension
        uDimAx = lpMemDimRht[uDim++];

        // Calculate the product of the dimensions below the axis dimension
        for (uDimHi = 1; uDim < aplRankRht; uDim++)
            uDimHi *= lpMemDimRht[uDim];
    } // End IF/ELSE

    // Calculate the HC Dimension
    iHCDimRht = TranslateArrayTypeToHCDim (aplTypeRht);

    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    uRes = uDim +                 uAx  * uDimHi;
                    uRht = uDim + ((uDimAx - 1) - uAx) * uDimHi;
                    uBitMask  = BIT0 << (UINT) (uRht % NBIB);
                    uBitIndex = (UINT) (uRes % NBIB);
                    ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |=
                    ((uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? TRUE : FALSE) << uBitIndex;
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_INT:
        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Loop through the parts
                    for (i = 0; i < iHCDimRht; i++)
                        ((LPAPLINT  ) lpMemRes)[i + iHCDimRht * (uDim +                 uAx  * uDimHi)] =
                        ((LPAPLINT  ) lpMemRht)[i + iHCDimRht * (uDim + ((uDimAx - 1) - uAx) * uDimHi)];
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_FLOAT:
        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Loop through the parts
                    for (i = 0; i < iHCDimRht; i++)
                        ((LPAPLFLOAT) lpMemRes)[i + iHCDimRht * (uDim +                 uAx  * uDimHi)] =
                        ((LPAPLFLOAT) lpMemRht)[i + iHCDimRht * (uDim + ((uDimAx - 1) - uAx) * uDimHi)];
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_CHAR:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    ((LPAPLCHAR) lpMemRes)[uDim +                 uAx  * uDimHi] =
                    ((LPAPLCHAR) lpMemRht)[uDim + ((uDimAx - 1) - uAx) * uDimHi];
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOffRht = lpAPA->Off;
            apaMulRht = lpAPA->Mul;
#undef  lpAPA
            // If the result is an APV, ...
            if (IsSimpleAPA (aplTypeRes))
            {
                Assert (uDimAx EQ aplNELMRht);

                // Calculate the new APA parameters
                ((LPAPLAPA) lpMemRes)->Off = apaOffRht + apaMulRht * (aplNELMRht - 1);
                ((LPAPLAPA) lpMemRes)->Mul = -apaMulRht;
            } else
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    ((LPAPLINT) lpMemRes)[uDim +                 uAx  * uDimHi] =
                       apaOffRht + apaMulRht * (uDim + ((uDimAx - 1) - uAx) * uDimHi);
                } // End FOR
            } // End IF/ELSE/FOR/FOR

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    ((LPAPLNESTED) lpMemRes)[uDim +                 uAx  * uDimHi] = CopySymGlbDir_PTB
                   (((LPAPLNESTED) lpMemRht)[uDim + ((uDimAx - 1) - uAx) * uDimHi]);
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_RAT:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Loop through the parts
                    for (i = 0; i < iHCDimRht; i++)
                        mpq_init_set (&((LPAPLRAT) lpMemRes)[i + iHCDimRht * (uDim +                 uAx  * uDimHi)],
                                      &((LPAPLRAT) lpMemRht)[i + iHCDimRht * (uDim + ((uDimAx - 1) - uAx) * uDimHi)]);
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_VFP:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Loop through the parts
                    for (i = 0; i < iHCDimRht; i++)
                        mpfr_init_copy (&((LPAPLVFP) lpMemRes)[i + iHCDimRht * (uDim +                 uAx  * uDimHi)],
                                        &((LPAPLVFP) lpMemRht)[i + iHCDimRht * (uDim + ((uDimAx - 1) - uAx) * uDimHi)]);
                } // End FOR
            } // End FOR/FOR

            break;

        defstop
            break;
    } // End SWITCH
NORMAL_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto IMMED_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
IMMED_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnMonCircleStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydCircleStile_EM_YY
//
//  Primitive function for dyadic CircleStile ("rotate")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydCircleStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydCircleStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft,       // Left arg storage type
                      aplTypeRht,       // Right ...
                      aplTypeRes;       // Result   ...
    APLNELM           aplNELMLft,       // Left arg NELM
                      aplNELMRht;       // Right ...
    APLRANK           aplRankLft,       // Left arg rank
                      aplRankRht;       // Right ...
    HGLOBAL           hGlbLft = NULL,   // Left arg global memory handle
                      hGlbRht = NULL,   // Right ...
                      hGlbRes = NULL,   // Result   ...
                      hGlbRot = NULL;   // Normalized left arg ...
    LPAPLDIM          lpMemDimLft,      // Ptr to left arg dimensions
                      lpMemDimRht;      // Ptr to right ...
    LPVOID            lpMemLft = NULL,  // Ptr to left arg global memory
                      lpMemRht = NULL,  // Ptr to right ...
                      lpMemRes = NULL;  // Ptr to result   ...
    LPVARARRAY_HEADER lpMemHdrLft,      // Ptr to left arg header
                      lpMemHdrRht,      // ...    right ...
                      lpMemHdrRes;      // ...    result    ...
    LPAPLINT          lpMemRot = NULL;  // Ptr to normalized left arg ...
    UBOOL             bRet = TRUE;      // TRUE iff result is valid
    APLUINT           aplAxis,          // The (one and only) axis value
                      ByteRes,          // # bytes in the result
                      uLo,              // Loop counter
                      uDimLo,           // ...
                      uAx,              // ...
                      uDimAx,           // ...
                      uHi,              // ...
                      uDimHi,           // ...
                      uDim,             // ...
                      uRes,             // ...
                      uRht;             // ...
    APLINT            aplIntegerLft,    // Temporary left arg integer
                      apaOffRht,        // Right arg APA offset
                      apaMulRht,        // Right arg APA multiplier
                      aplRot;           //
    APLLONGEST        aplLongestLft;    // Left arg immediate value
    LPPL_YYSTYPE      lpYYRes = NULL;   // Ptr to the result
    UINT              uBitMask,         // Bit mask when looping through Booleans
                      uBitIndex;        // Bit index ...
    LPPLLOCALVARS     lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;     // Ptr to Ctrl-Break flag
    int               iHCDimRht,        // HC Dimension (1, 2, 4, 8)
                      i;                // Loop counter

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Rotate allows a single integer axis value only
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
            return NULL;
    } else
    {
        // No axis specified:
        //   if CircleStile, use last dimension
        if (lptkFunc->tkData.tkChar EQ UTF16_CIRCLESTILE)
            aplAxis = aplRankRht - 1;
        else
        {
            Assert (lptkFunc->tkData.tkChar EQ UTF16_CIRCLEBAR);

            // Otherwise, it's CircleBar on the first dimension
            aplAxis = 0;
        } // End IF/ELSE
    } // End IF/ELSE

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
                    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the left arg is immediate, ...
    if (hGlbLft EQ NULL)
        // Point to the immediate data
        lpMemLft = &aplLongestLft;
    else
        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);

    // Scalar or one-element vector left arg matches everything
    if (!(IsScalar (aplRankLft) || IsVectorSing (aplNELMLft, aplRankLft)))
    {
        // Check for RANK ERROR
        if ((aplRankRht - aplRankLft) NE 1)
            goto RANK_EXIT;

        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);
        lpMemDimLft = VarArrayBaseToDim (lpMemHdrLft);

        // Check for LENGTH ERROR
        for (uDim = 0; uDim < aplRankLft; uDim++)
        if (lpMemDimLft[uDim] NE lpMemDimRht[uDim + (uDim >= aplAxis)])
            goto LENGTH_EXIT;
    } // End IF

    // Check for DOMAIN ERROR
    if (!IsNumeric (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // If the left arg is singleton, get its value
    if (IsSingleton (aplNELMLft))
    {
        // Attempt to convert it to an INT
        aplIntegerLft = ConvertToInteger_SCT (aplTypeLft, lpMemLft, 0, &bRet);
        if (!bRet)
            goto LEFT_DOMAIN_EXIT;
    } else
    // Split cases based upon the left arg's storage type
    {
        LPAPLINT lpMemRotIni;

        // Copy left arg to temp storage (all APLINTs), and
        //   check the left arg for valid values

        // Calculate space needed for the result
        ByteRes = aplNELMLft * sizeof (APLINT);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Allocate temp storage for the normalized left arg
        hGlbRot = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (hGlbRot EQ NULL)
            goto WSFULL_EXIT;

        // If the left arg is non-empty, ...
        if (ByteRes)
            // Lock the memory to get a ptr to it
            lpMemRot = lpMemRotIni = MyGlobalLock000 (hGlbRot);
        else
            // The left arg is empty and W doesn't handle
            //   locking empty storage, so we just NULL the ptrs
            lpMemRot = lpMemRotIni = NULL;

        // Loop through the left arg NELM
        for (uDim = 0; uDim < aplNELMLft; uDim++)
        {
            // Attempt to convert the left arg items to an INT
            *lpMemRot++ = ConvertToInteger_SCT (aplTypeLft, lpMemLft, uDim, &bRet);

            if (!bRet)
                goto LEFT_DOMAIN_EXIT;
        } // End FOR

        // Restore lpMemRot to the start of the block
        lpMemRot = lpMemRotIni;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    // Map APA right arg to INT result
    if (IsSimpleAPA (aplTypeRht))
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeRht;

    // Strip out the simple scalar right argument case
    if (IsScalar (aplRankRht) && IsSimpleNH (aplTypeRes))
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Split cases based upon the right arg's token type
        switch (lptkRhtArg->tkFlags.TknType)
        {
            case TKT_VARNAMED:
                // tkData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

                // If it's not immediate, we must look inside the array
                if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
                {
                    // stData is a valid HGLOBAL variable array
                    Assert (IsGlbTypeVarDir_PTB (lptkRhtArg->tkData.tkSym->stData.stGlbData));
#ifdef DEBUG
                    // If we ever get here, we must have missed a type demotion
                    DbgStop ();             // #ifdef DEBUG
#endif
                } // End IF

                // Handle the immediate case

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkFlags.ImmType   = lptkRhtArg->tkData.tkSym->stFlags.ImmType;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkLongest  = lptkRhtArg->tkData.tkSym->stData.stLongest;
                lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

                break;

            case TKT_VARIMMED:
                // Fill in the result token
                lpYYRes->tkToken = *lptkRhtArg;

                break;

            defstop
                break;
        } // End SWITCH

        goto NORMAL_EXIT;
    } // End IF

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->PV0        = lpMemHdrRht->PV0;
    lpHeader->PV1        = lpMemHdrRht->PV1;
    lpHeader->bSelSpec   = lpMemHdrRht->bSelSpec;
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemHdrRes);
    lpMemDimRht = lpMemRht = VarArrayBaseToDim (lpMemHdrRht);

    if (IsScalar (aplRankRht))
        uDimLo = uDimAx = uDimHi = 1;
    else
    {
        // Calculate the product of the right arg's dimensions below the axis dimension
        for (uDim = 0, uDimLo = 1; uDim < aplAxis; uDim++)
            uDimLo *= lpMemDimRht[uDim];

        // Get the length of the axis dimension
        uDimAx = lpMemDimRht[uDim++];

        // Calculate the product of the right arg's dimensions below the axis dimension
        for (uDimHi = 1; uDim < aplRankRht; uDim++)
            uDimHi *= lpMemDimRht[uDim];
    } // End IF/ELSE

    // Copy the dimensions from the right arg to the result
    for (uDim = 0; uDim < aplRankRht; uDim++)
        *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;;

    // lpMemRes now points to the result's data
    // lpMemRht now points to the right arg's data

    // Calculate the HC Dimension
    iHCDimRht = TranslateArrayTypeToHCDim (aplTypeRht);

    // Handle empty nested array results (prototypes)
    if (IsEmpty (aplNELMRht) && IsNested (aplTypeRes))
        *((LPAPLNESTED) lpMemRes) = CopySymGlbInd_PTB (lpMemRht);
    else
    // Copy the data to the result
    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (IsSingleton (aplNELMLft))
                    aplRot = aplIntegerLft;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    uRes = uDim +                  uAx           * uDimHi;
                    uRht = uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi;
                    uBitMask  = BIT0 << (UINT) (uRht % NBIB);
                    uBitIndex = (UINT) (uRes % NBIB);
                    ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |=
                    ((uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? TRUE : FALSE) << uBitIndex;
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_INT:
        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (IsSingleton (aplNELMLft))
                    aplRot = aplIntegerLft;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRht; i++)
                        ((LPAPLINT  ) lpMemRes)[i + iHCDimRht * (uDim +                  uAx           * uDimHi)] =
                        ((LPAPLINT  ) lpMemRht)[i + iHCDimRht * (uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi)];
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_FLOAT:
        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (IsSingleton (aplNELMLft))
                    aplRot = aplIntegerLft;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRht; i++)
                        ((LPAPLFLOAT) lpMemRes)[i + iHCDimRht * (uDim +                  uAx           * uDimHi)] =
                        ((LPAPLFLOAT) lpMemRht)[i + iHCDimRht * (uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi)];
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_CHAR:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (IsSingleton (aplNELMLft))
                    aplRot = aplIntegerLft;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    ((LPAPLCHAR) lpMemRes)[uDim +                  uAx           * uDimHi] =
                    ((LPAPLCHAR) lpMemRht)[uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi];
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOffRht = lpAPA->Off;
            apaMulRht = lpAPA->Mul;
#undef  lpAPA
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (IsSingleton (aplNELMLft))
                    aplRot = aplIntegerLft;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    ((LPAPLINT) lpMemRes)[uDim +                  uAx           * uDimHi] =
                       apaOffRht + apaMulRht * (uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi);
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (IsSingleton (aplNELMLft))
                    aplRot = aplIntegerLft;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    ((LPAPLNESTED) lpMemRes)[uDim +                  uAx           * uDimHi] = CopySymGlbDir_PTB
                   (((LPAPLNESTED) lpMemRht)[uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi]);
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_RAT:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (IsSingleton (aplNELMLft))
                    aplRot = aplIntegerLft;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRht; i++)
                        mpq_init_set (&((LPAPLRAT) lpMemRes)[i + iHCDimRht * (uDim +                  uAx           * uDimHi)],
                                      &((LPAPLRAT) lpMemRht)[i + iHCDimRht * (uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi)]);
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_VFP:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (IsSingleton (aplNELMLft))
                    aplRot = aplIntegerLft;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRht; i++)
                        mpfr_init_copy (&((LPAPLVFP) lpMemRes)[i + iHCDimRht * (uDim +                  uAx           * uDimHi)],
                                        &((LPAPLVFP) lpMemRht)[i + iHCDimRht * (uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi)]);
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

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as in error
    bRet = FALSE;

    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbRot, lpMemRot);

    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (bRet)
        return lpYYRes;
    else
        return NULL;
} // End PrimFnDydCircleStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_cstile.c
//***************************************************************************