//***************************************************************************
//  NARS2000 -- Primitive Function -- Slash
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2019 Sudley Place Software

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
//  $PrimFnSlash_EM_YY
//
//  Primitive function for monadic and dyadic Slash (ERROR and "replicate")
//***************************************************************************

LPPL_YYSTYPE PrimFnSlash_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_SLASH
         || lptkFunc->tkData.tkChar EQ UTF16_SLASHBAR);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonSlash_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydSlash_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnSlash_EM_YY


//***************************************************************************
//  $PrimProtoFnSlash_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Slash
//***************************************************************************

LPPL_YYSTYPE PrimProtoFnSlash_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnSlash_EM_YY,  // Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnSlash_EM_YY


//***************************************************************************
//  $PrimIdentFnSlash_EM_YY
//
//  Generate an identity element for the dyadic derived function from the
//    primitive operator Slash
//***************************************************************************

LPPL_YYSTYPE PrimIdentFnSlash_EM_YY
    (LPTOKEN lptkRhtOrig,           // Ptr to original right arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxisOpr)           // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result
    LPPRIMFLAGS  lpPrimFlagsLft;    // Ptr to left operand PrimFlags entry
    APLRANK      aplRankRht;        // Right arg rank

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig NE NULL);
    Assert (lptkFunc    NE NULL);
    Assert (lptkRhtArg  NE NULL);

    // The left identity function for dyadic function Slash
    //   (L /[X] R) ("replicate") is
    //   1 if 1.  X (if present) is a valid axis to R
    //        2.  R is non-scalar (because L/R always returns a vector or higher rank array)
    //            Note that we are called only from //0{rho}R and we can't see the
    //            original R.

    // Ensure the right operand is a function
    if (lptkFunc  EQ NULL
     || !IsTknFcnOpr (lptkFunc)
     || IsTknFillJot (lptkFunc))
        goto LEFT_OPERAND_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtOrig, NULL, NULL, &aplRankRht, NULL);

    // Ensure the right arg is non-scalar
    if (IsScalar (aplRankRht))
        goto DOMAIN_EXIT;

    // Check for axis present
    if (lptkAxisOpr NE NULL)
    {
        // Reduction allows a single integer axis value only
        if (!CheckAxis_EM (lptkAxisOpr,     // The derived function axis token
                           aplRankRht,      // All values less than this
                           TRUE,            // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // Return TRUE iff fractional values present
                           NULL,            // Return last axis value (may be NULL)
                           NULL,            // Return # elements in axis vector
                           NULL))           // Return HGLOBAL with APLINT axis values
            goto AXIS_SYNTAX_EXIT;
    } // End IF

    // Get a ptr to the Primitive Function Flags
    lpPrimFlagsLft = GetPrimFlagsPtr (lptkFunc);

    // If the flags are not present,
    //   or there's no identity element, ...
    if (lpPrimFlagsLft EQ NULL
     || !lpPrimFlagsLft->bIdentElem)
        return NULL;

    // Return the scalar 1 as the result

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_BOOL;
    lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;
    lpYYRes->tkToken.tkData.tkBoolean  = 1;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtOrig);
    return NULL;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    return NULL;

LEFT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimIdentFnSlash_EM_YY


//***************************************************************************
//  $PrimFnMonSlash_EM_YY
//
//  Primitive function for monadic Slash (ERROR)
//***************************************************************************

LPPL_YYSTYPE PrimFnMonSlash_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimFnMonSlash_EM_YY


//***************************************************************************
//  $PrimFnDydSlash_EM_YY
//
//  Primitive function for dyadic Slash ("replicate")
//***************************************************************************

LPPL_YYSTYPE PrimFnDydSlash_EM_YY
    (LPTOKEN lptkLftArg,                // Ptr to left arg token
     LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht,           // Right ...
                      aplTypeRes;           // Result ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht,           // Right ...
                      aplNELMRes;           // Result ...
    APLRANK           aplRankLft,           // Left arg rank
                      aplRankRht,           // Right ...
                      aplRankRes;           // Result ...
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL,       // Right ...
                      hGlbRes = NULL,       // Result ...
                      hGlbRep = NULL;       // Rep ...
    LPAPLDIM          lpMemDimRht = NULL;   // Ptr to right arg dimensions
    LPVOID            lpMemLft = NULL,      // Ptr to left arg data
                      lpMemRht = NULL,      // ...    right ...
                      lpMemRes = NULL;      // ...    result ...
    LPAPLINT          lpMemRep = NULL;      // ...    rep ...
    UBOOL             bRet,                 // TRUE iff the result is valid
                      bLftNeg = FALSE;      // TRUE iff the left arg contains a negative value
    APLUINT           aplAxis,              // The (one and only) axis value
                      ByteRes,              // # bytes in the result
                      uLo,                  // Loop counter
                      uDimLo,               // ...
                      uAx,                  // ...
                      uDimAxRht,            // ...
                      uDimAxRes,            // ...
                      uHi,                  // ...
                      uDimHi,               // ...
                      uDim,                 // ...
                      uDimRht,              // ...
                      uDimRes,              // ...
                      uRes,                 // ...
                      uRht,                 // ...
                      uAcc;                 // ...
    APLINT            aplIntegerLft,        // Left arg as integer
                      aplIntegerRep,        // Rep as integer
                      apaOff,               // APA offset
                      apaMul,               // ... multiplier
                      iLftLen,              // Length if singleton left arg
                      iRep,                 // Loop counter
                      iLen;                 // ...
    APLLONGEST        aplLongestLft;        // Left arg if immediate
    APLFLOAT          aplFloatRep;          // Rep arg as FLT
    APLCHAR           aplCharRep;           // Rep as CHAR
    APLNESTED         aplNestRep;           // Right arg as NESTED
    LPPL_YYSTYPE      lpYYRes = NULL;       // The result
    UINT              uBitMask,             // Bit mask for working with Boolean
                      uBitIndex;            // Bit index ...
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    LPVARARRAY_HEADER lpMemHdrLft,          // Ptr to left arg header
                      lpMemHdrRht,          // ...    right ...
                      lpMemHdrRes;          // ...    result    ...
    HGLOBAL           lpSymGlbLft;          // Ptr to left arg as global numeric
    int               i;                    // Loop counter
    ALLTYPES          atRep = {0},          // Used for repetitions
                      atRht = {0};          // ...

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // The rank of the result is the same as that of the right arg
    //   with scalar right arg promoted to a vector
    aplRankRes = max (aplRankRht, 1);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Replicate allows a single integer axis value only
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRes,      // All values less than this
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
        //   if Slash   , use last  axis
        //   if SlashBar, use first axis
        if (lptkFunc->tkData.tkChar EQ UTF16_SLASH)
            aplAxis = aplRankRes - 1;
        else
        {
            Assert (lptkFunc->tkData.tkChar EQ UTF16_SLASHBAR);

            // Otherwise, it's SlashBar on the first axis
            aplAxis = 0;
        } // End IF/ELSE
    } // End IF/ELSE

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the right arg is a global, ...
    if (lpMemHdrRht)
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

    // Check for LENGTH ERROR
    // Singleton left or right arg matches everything
    if (!IsSingleton (aplNELMLft)
     && !IsSingleton (aplNELMRht)
     && aplNELMLft NE lpMemDimRht[aplAxis])
        goto LENGTH_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsNumeric (aplTypeLft)
     && !IsCharEmpty (aplTypeLft, aplNELMLft))
        goto LEFT_DOMAIN_EXIT;

    // Calculate product of dimensions before, at, and after the axis dimension
    if (IsSingleton (aplNELMRht))
    {
        uDimLo = uDimHi = 1;
        uDimAxRes = uDimAxRht = aplNELMLft;
    } else
    {
        // Calculate the product of the right arg's dimensions below the axis dimension
        uDimLo = 1;
        for (uDim = 0; uDim < aplAxis; uDim++)
            uDimLo *= lpMemDimRht[uDim];

        // Get the length of the axis dimension
        uDimAxRht = lpMemDimRht[uDim++];

        // Calculate the product of the right arg's dimensions above the axis dimension
        uDimHi = 1;
        for (; uDim < aplRankRht; uDim++)
            uDimHi *= lpMemDimRht[uDim];
    } // End IF/ELSE

    // If the left arg is singleton, get its value
    if (IsSingleton (aplNELMLft))
    {
        // Get the integer or float value
        GetFirstValueToken (lptkLftArg,     // Ptr to left arg token
                            NULL,           // Ptr to integer result
                            NULL,           // Ptr to float ...
                            NULL,           // Ptr to WCHAR ...
                           &aplLongestLft,  // Ptr to longest ...
                           &lpSymGlbLft,    // Ptr to lpSym/Glb ...
                            NULL,           // Ptr to ...immediate type ...
                            NULL);          // Ptr to array type ...
        // If the left arg is immediate, ...
        if (lpSymGlbLft EQ NULL)
            lpSymGlbLft = &aplLongestLft;

        // Attempt to convert the left arg item to an integer using System []CT
        aplIntegerLft = ConvertToInteger_SCT (aplTypeLft, lpSymGlbLft, 0, &bRet);
        if (!bRet)
            goto LEFT_DOMAIN_EXIT;

        // Check the singleton arg
        if (aplIntegerLft < 0)
            // Mark as contains a negative value
            bLftNeg = TRUE;

        // Special case 1/R for non-scalar R
        if (aplIntegerLft EQ 1
         && !IsScalar (aplRankRht))
        {
            // Increment the right arg reference count
            hGlbRes = CopySymGlbDirAsGlb (hGlbRht);

            goto YYALLOC_EXIT;
        } // End IF

        // Calculate the length of the axis dimension in the result
        if (lpMemDimRht && !IsScalar (aplRankRht))
            uDimAxRes  = abs64 (aplIntegerLft) * lpMemDimRht[aplAxis];
        else
            uDimAxRes  = abs64 (aplIntegerLft);
        // Calculate the result NELM
        aplNELMRes = uDimLo * uDimAxRes * uDimHi;
    } else
    // Non-singleton left arg
    {
        // Copy left arg to temp storage (all APLINTs),
        //   check the left arg for valid values, and
        //   accumulate the sum (as uDimAxRes).

        // Calculate space needed for the normalized left arg
        ByteRes = aplNELMLft * sizeof (APLINT);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Allocate temp storage for the normalized left arg
        hGlbRep = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (hGlbRep EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        //   and check for empty (char) case
        if (!IsEmpty (aplNELMLft))
            lpMemRep = MyGlobalLock000 (hGlbRep);

        // Skip over the header to the data
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);

        // Initialize the result axis dimension accumulator
        uDimAxRes = 0;

        // Split cases based upon the left arg's storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
                uBitMask = BIT0;

                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    // Get the next INT from the left arg
                    aplIntegerLft = (uBitMask & *(LPAPLBOOL) lpMemLft) ? TRUE : FALSE;

                    // If we're filling w/prototypes
                    if (aplIntegerLft < 0)
                        // Mark as contains a negative value
                        bLftNeg = TRUE;

                    // Accumulate in the axis length
                    uDimAxRes += abs64 (aplIntegerLft);

                    // Save in the normalized left arg
                    *lpMemRep++ = aplIntegerLft;

                    // Shift over the left bit mask
                    uBitMask <<= 1;

                    // Check for end-of-byte
                    if (uBitMask EQ END_OF_BYTE)
                    {
                        uBitMask = BIT0;            // Start over
                        ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                    } // End IF
                } // End FOR

                break;

            case ARRAY_INT:
                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    // Get the next INT from the left arg
                    aplIntegerLft = *((LPAPLINT) lpMemLft)++;

                    // If we're filling w/prototypes
                    if (aplIntegerLft < 0)
                        // Mark as contains a negative value
                        bLftNeg = TRUE;

                    // Accumulate in the axis length
                    uDimAxRes += abs64 (aplIntegerLft);

                    // Save in the normalized left arg
                    *lpMemRep++ = aplIntegerLft;
                } // End IF

                break;

            case ARRAY_FLOAT:
                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    // Attempt to convert the float to an integer using System []CT
                    aplIntegerLft = FloatToAplint_SCT (*((LPAPLFLOAT) lpMemLft)++, &bRet);
                    if (!bRet)
                        goto LEFT_DOMAIN_EXIT;

                    // If we're filling w/prototypes
                    if (aplIntegerLft < 0)
                        // Mark as contains a negative value
                        bLftNeg = TRUE;

                    // Accumulate in the axis length
                    uDimAxRes += abs64 (aplIntegerLft);

                    // Save in the normalized left arg
                    *lpMemRep++ = aplIntegerLft;
                } // End FOR

                break;

            case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemLft)
                // Get the APA parameters
                apaOff = lpAPA->Off;
                apaMul = lpAPA->Mul;
#undef  lpAPA
                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    // Get the next INT from the left arg
                    aplIntegerLft = apaOff + apaMul * uDim;

                    // If we're filling w/prototypes
                    if (aplIntegerLft < 0)
                        // Mark as contains a negative value
                        bLftNeg = TRUE;

                    // Accumulate in the axis length
                    uDimAxRes += abs64 (aplIntegerLft);

                    // Save in the normalized left arg
                    *lpMemRep++ = aplIntegerLft;
                } // End FOR

                break;

            case ARRAY_CHAR:
                break;

            case ARRAY_RAT:
                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    // Get the next INT from the left arg
                    aplIntegerLft = mpq_get_sx (((LPAPLRAT) lpMemLft)++, &bRet);
                    if (!bRet)
                        goto LEFT_DOMAIN_EXIT;

                    // If we're filling w/prototypes
                    if (aplIntegerLft < 0)
                        // Mark as contains a negative value
                        bLftNeg = TRUE;

                    // Accumulate in the axis length
                    uDimAxRes += abs64 (aplIntegerLft);

                    // Save in the normalized left arg
                    *lpMemRep++ = aplIntegerLft;
                } // End IF

                break;

            case ARRAY_VFP:
                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    // Get the next INT from the left arg
                    aplIntegerLft = mpfr_get_sx (((LPAPLVFP) lpMemLft)++, &bRet);
                    if (!bRet)
                        goto LEFT_DOMAIN_EXIT;

                    // If we're filling w/prototypes
                    if (aplIntegerLft < 0)
                        // Mark as contains a negative value
                        bLftNeg = TRUE;

                    // Accumulate in the axis length
                    uDimAxRes += abs64 (aplIntegerLft);

                    // Save in the normalized left arg
                    *lpMemRep++ = aplIntegerLft;
                } // End IF

                break;

            defstop
                break;
        } // End SWITCH

        // Restore lpMemRep to the start of the block
        //   and check for empty (char) case
        if (!IsEmpty (aplNELMLft))
        {
            MyGlobalUnlock (hGlbRep); lpMemRep = NULL;
            lpMemRep = MyGlobalLockInt (hGlbRep);
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;

        // Calculate the result NELM
        aplNELMRes = uDimLo * uDimAxRes * uDimHi;
    } // End IF

    // Map APA right arg to INT result
    if (IsSimpleAPA (aplTypeRht))
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeRht;

    // If the right arg is a singleton,
    //   and the result is non-empty,
    //   and the result is integer-like, ...
    if (IsSingleton (aplNELMRht)
     && !bLftNeg
     && !IsEmpty (aplNELMRes)
     && IsRealBIA (aplTypeRes))
        // Store it in an APA
        aplTypeRes = ARRAY_APA;

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
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->bSelSpec   = lpMemHdrRht && lpMemHdrRht->bSelSpec;
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Copy the dimensions from the right arg to the result
    if (lpMemHdrRht)
        CopyMemory (VarArrayBaseToDim (lpMemHdrRes),
                    VarArrayBaseToDim (lpMemHdrRht),
                    (APLU3264) aplRankRht * sizeof (APLDIM));

    // Fill in the axis dimension
    (VarArrayBaseToDim (lpMemHdrRes))[aplAxis] = uDimAxRes;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);
    if (lpMemHdrRht)
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // Handle empty nested array results (prototypes)
    if (IsEmpty (aplNELMRes))
    {
        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_APA:
                lpMemHdrRes->ArrType = ARRAY_BOOL;

                break;

            case ARRAY_CHAR:
                lpMemHdrRes->ArrType = ARRAY_CHAR;

                break;

            case ARRAY_NESTED:
                *((LPAPLNESTED) lpMemRes) =
                  MakeMonPrototype_EM_PTB (*(LPAPLNESTED) lpMemRht, // Proto arg handle
                                           ARRAY_NESTED,            // Array storage type
                                           lptkFunc,                // Ptr to function token
                                           MP_CHARS);               // CHARs allowed
                break;

            case ARRAY_HETERO:
            {
                LPSYMENTRY lpSym = *(LPAPLHETERO) lpMemRht;

                // Split cases based upon the ptr type bits
                switch (GetPtrTypeDir (lpSym))
                {
                    case PTRTYPE_STCONST:
                        // Set the result storage type to either Boolean or char

                        // Split cases based upon the STE immediate type
                        switch (lpSym->stFlags.ImmType)
                        {
                            case IMMTYPE_BOOL:
                            case IMMTYPE_INT:
                            case IMMTYPE_FLOAT:
                                lpMemHdrRes->ArrType = ARRAY_BOOL;

                                break;

                            case IMMTYPE_CHAR:
                                lpMemHdrRes->ArrType = ARRAY_CHAR;

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        break;

                    case PTRTYPE_HGLOBAL:
                        // The item is a global numeric
                        lpMemHdrRes->ArrType = ARRAY_BOOL;

                        break;

                    defstop
                        break;
                } // End SWITCH

                break;
            } // End ARRAY_HETERO

            case ARRAY_HC2I:
            case ARRAY_HC4I:
            case ARRAY_HC8I:
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
                break;

            defstop
                break;
        } // End SWITCH

        goto PROTO_EXIT;
    } // End IF

    if (IsSingleton (aplNELMLft))
        iLftLen = aplIntegerLft;
    // If the right arg is a singleton, get its value
    if (IsSingleton (aplNELMRht))
        GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                           &aplIntegerRep,  // Ptr to integer result
                           &aplFloatRep,    // Ptr to float ...
                           &aplCharRep,     // Ptr to WCHAR ...
                            NULL,           // Ptr to longest ...
                           &aplNestRep,     // Ptr to lpSym/Glb ...
                            NULL,           // Ptr to ...immediate type ...
                            NULL);          // Ptr to array type ...
    // Copy the data to the result

    // If the result is an APA, ...
    if (IsSimpleAPA (aplTypeRes))
    {
        ((LPAPLAPA) lpMemRes)->Off = aplIntegerRep;
        ((LPAPLAPA) lpMemRes)->Mul = 0;
        lpMemHdrRes->All2s         = (aplIntegerRep EQ 2);
    } else
    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        atRht.aplInteger = 0;
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                    {
                        uRht = uDimRht + uAx * uDimHi;
                        uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) uRht);
                        atRht.aplInteger = (uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? TRUE : FALSE;
                    } else
                        // Use singleton right arg value
                        atRht.aplInteger = aplIntegerRep;

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        uRes = uDimRes + uAcc * uDimHi;
                        uBitIndex = MASKLOG2NBIB & (UINT) uRes;
                        ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |= (atRht.aplInteger << uBitIndex);
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_INT:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        atRht.aplInteger = 0;
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        atRht.aplInteger = ((LPAPLINT) lpMemRht)[uDimRht + uAx * uDimHi];
                    else
                        // Use singleton right arg value
                        atRht.aplInteger = aplIntegerRep;

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        ((LPAPLINT) lpMemRes)[uDimRes + uAcc * uDimHi] = atRht.aplInteger;
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_FLOAT:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        atRht.aplFloat = 0.0;
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        atRht.aplFloat = ((LPAPLFLOAT) lpMemRht)[uDimRht + uAx * uDimHi];
                    else
                        // Use singleton right arg value
                        atRht.aplFloat = aplFloatRep;

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        ((LPAPLFLOAT) lpMemRes)[uDimRes + uAcc * uDimHi] = atRht.aplFloat;
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_CHAR:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        atRht.aplChar = L' ';
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        atRht.aplChar = ((LPAPLCHAR) lpMemRht)[uDimRht + uAx * uDimHi];
                    else
                        // Use singleton right arg value
                        atRht.aplChar = aplCharRep;

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        ((LPAPLCHAR) lpMemRes)[uDimRes + uAcc * uDimHi] = atRht.aplChar;
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOff = lpAPA->Off;
            apaMul = lpAPA->Mul;
#undef  lpAPA
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        atRht.aplInteger = 0;
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        atRht.aplInteger = apaOff + apaMul * (uDimRht + uAx * uDimHi);
                    else
                        // Use singleton right arg value
                        atRht.aplInteger = aplIntegerRep;

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        ((LPAPLINT) lpMemRes)[uDimRes + uAcc * uDimHi] = atRht.aplInteger;
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    UBOOL bNestPro = FALSE;     // TRUE iff the right arg is a nested prototype

                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        atRht.aplNested =
                          MakeMonPrototype_EM_PTB (((LPAPLNESTED) lpMemRht)[uDimRht + uAx * uDimHi], // Proto arg handle
                                                   ARRAY_NESTED,            // Array storage type
                                                   lptkFunc,                // Ptr to function token
                                                   MP_CHARS);               // CHARs allowed
                        iLen = abs64 (iLen);

                        // Mark as a nested prototype
                        bNestPro = TRUE;
                    } else
                    if (!IsSingleton (aplNELMRht))
                        atRht.aplNested = ((LPAPLNESTED) lpMemRht)[uDimRht + uAx * uDimHi];
                    else
                        // Use singleton right arg value
                        atRht.aplNested = aplNestRep;

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        ((LPAPLNESTED) lpMemRes)[uDimRes + uAcc * uDimHi] = CopySymGlbDir_PTB (atRht.aplNested);
                    } // End FOR

                    // If it's a nested prototype, ...
                    if (bNestPro)
                        // Free it
                        FreeResultGlobalVar (atRht.aplNested);
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_RAT:
            // Initialize the temps
            mpq_init (&atRep.aplRat);
            mpq_init (&atRht.aplRat);

            if (IsSingleton (aplNELMRht))
                mpq_set (&atRep.aplRat, (LPAPLRAT) aplNestRep);

            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        mpq_set (&atRht.aplRat, &mpqZero);
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        mpq_set (&atRht.aplRat, &((LPAPLRAT) lpMemRht)[uDimRht + uAx * uDimHi]);
                    else
                        // Use singleton right arg value
                        mpq_set (&atRht.aplRat, &atRep.aplRat);

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        mpq_init_set (&((LPAPLRAT) lpMemRes)[uDimRes + uAcc * uDimHi], &atRht.aplRat);
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_VFP:
            // Initialize the temps
            mpfr_init0 (&atRep.aplVfp);
            mpfr_init0 (&atRht.aplVfp);

            if (IsSingleton (aplNELMRht))
                mpfr_copy (&atRep.aplVfp, (LPAPLVFP) aplNestRep);

            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        mpfr_set (&atRht.aplVfp, &mpfZero, MPFR_RNDN);
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        mpfr_copy (&atRht.aplVfp, &((LPAPLVFP) lpMemRht)[uDimRht + uAx * uDimHi]);
                    else
                        // Use singleton right arg value
                        mpfr_set (&atRht.aplVfp, &atRep.aplVfp, MPFR_RNDN);

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        mpfr_init_copy (&((LPAPLVFP) lpMemRes)[uDimRes + uAcc * uDimHi], &atRht.aplVfp);
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

       case ARRAY_HC2I:
            if (IsSingleton (aplNELMRht))
                atRep.aplHC2I = ((LPAPLHC2I) lpMemRht)[0];

            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        ZeroMemory (&atRht.aplHC2I, sizeof (atRht.aplHC2I));
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        atRht.aplHC2I = ((LPAPLHC2I) lpMemRht)[uDimRht + uAx * uDimHi];
                    else
                        // Use singleton right arg value
                        atRht.aplHC2I = atRep.aplHC2I;

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        ((LPAPLHC2I) lpMemRes)[uDimRes + uAcc * uDimHi] = atRht.aplHC2I;
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HC4I:
            if (IsSingleton (aplNELMRht))
                atRep.aplHC4I = ((LPAPLHC4I) lpMemRht)[0];

            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        ZeroMemory (&atRht.aplHC4I, sizeof (atRht.aplHC4I));
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        atRht.aplHC4I = ((LPAPLHC4I) lpMemRht)[uDimRht + uAx * uDimHi];
                    else
                        // Use singleton right arg value
                        atRht.aplHC4I = atRep.aplHC4I;

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        ((LPAPLHC4I) lpMemRes)[uDimRes + uAcc * uDimHi] = atRht.aplHC4I;
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HC8I:
            if (IsSingleton (aplNELMRht))
                atRep.aplHC8I = ((LPAPLHC8I) lpMemRht)[0];

            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        ZeroMemory (&atRht.aplHC8I, sizeof (atRht.aplHC8I));
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        atRht.aplHC8I = ((LPAPLHC8I) lpMemRht)[uDimRht + uAx * uDimHi];
                    else
                        // Use singleton right arg value
                        atRht.aplHC8I = atRep.aplHC8I;

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        ((LPAPLHC8I) lpMemRes)[uDimRes + uAcc * uDimHi] = atRht.aplHC8I;
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HC2F:
            if (IsSingleton (aplNELMRht))
                atRep.aplHC2F = ((LPAPLHC2F) lpMemRht)[0];

            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        ZeroMemory (&atRht.aplHC2F, sizeof (atRht.aplHC2F));
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        atRht.aplHC2F = ((LPAPLHC2F) lpMemRht)[uDimRht + uAx * uDimHi];
                    else
                        // Use singleton right arg value
                        atRht.aplHC2F = atRep.aplHC2F;

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        ((LPAPLHC2F) lpMemRes)[uDimRes + uAcc * uDimHi] = atRht.aplHC2F;
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HC4F:
            if (IsSingleton (aplNELMRht))
                atRep.aplHC4F = ((LPAPLHC4F) lpMemRht)[0];

            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        ZeroMemory (&atRht.aplHC4F, sizeof (atRht.aplHC4F));
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        atRht.aplHC4F = ((LPAPLHC4F) lpMemRht)[uDimRht + uAx * uDimHi];
                    else
                        // Use singleton right arg value
                        atRht.aplHC4F = atRep.aplHC4F;

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        ((LPAPLHC4F) lpMemRes)[uDimRes + uAcc * uDimHi] = atRht.aplHC4F;
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HC8F:
            if (IsSingleton (aplNELMRht))
                atRep.aplHC8F = ((LPAPLHC8F) lpMemRht)[0];

            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        ZeroMemory (&atRht.aplHC8F, sizeof (atRht.aplHC8F));
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        atRht.aplHC8F = ((LPAPLHC8F) lpMemRht)[uDimRht + uAx * uDimHi];
                    else
                        // Use singleton right arg value
                        atRht.aplHC8F = atRep.aplHC8F;

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        ((LPAPLHC8F) lpMemRes)[uDimRes + uAcc * uDimHi] = atRht.aplHC8F;
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HC2R:
            // Initialize the temps
            mphc2r_init (&atRep.aplHC2R);
            mphc2r_init (&atRht.aplHC2R);

            if (IsSingleton (aplNELMRht))
                // Loop through all of the parts
                for (i = 0; i < 2; i++)
                    mpq_set (&atRep.aplHC2R.parts[i],
                             &((LPAPLHC2R) aplNestRep)->parts[i]);
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        // Loop through all of the parts
                        for (i = 0; i < 2; i++)
                            mpq_set_si (&atRht.aplHC2R.parts[i], 0, 1);
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        // Loop through all of the parts
                        for (i = 0; i < 2; i++)
                            mpq_set (&atRht.aplHC2R.parts[i],
                                     &((LPAPLHC2R) lpMemRht)[uDimRht + uAx * uDimHi].parts[i]);
                    else
                        // Loop through all of the parts
                        for (i = 0; i < 2; i++)
                            // Use singleton right arg value
                            mpq_set (&atRht.aplHC2R.parts[i], &atRep.aplHC2R.parts[i]);

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Loop through all of the parts
                        for (i = 0; i < 2; i++)
                            mpq_init_set (&((LPAPLHC2R) lpMemRes)[uDimRes + uAcc * uDimHi].parts[i],
                                          &atRht.aplHC2R.parts[i]);
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HC4R:
            // Initialize the temps
            mphc4r_init (&atRep.aplHC4R);
            mphc4r_init (&atRht.aplHC4R);

            if (IsSingleton (aplNELMRht))
                // Loop through all of the parts
                for (i = 0; i < 4; i++)
                    mpq_set (&atRep.aplHC4R.parts[i],
                             &((LPAPLHC4R) aplNestRep)->parts[i]);
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        // Loop through all of the parts
                        for (i = 0; i < 4; i++)
                            mpq_set_si (&atRht.aplHC4R.parts[i], 0, 1);
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        // Loop through all of the parts
                        for (i = 0; i < 4; i++)
                            mpq_set (&atRht.aplHC4R.parts[i],
                                     &((LPAPLHC4R) lpMemRht)[uDimRht + uAx * uDimHi].parts[i]);
                    else
                        // Loop through all of the parts
                        for (i = 0; i < 4; i++)
                            // Use singleton right arg value
                            mpq_set (&atRht.aplHC4R.parts[i], &atRep.aplHC4R.parts[i]);

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Loop through all of the parts
                        for (i = 0; i < 4; i++)
                            mpq_init_set (&((LPAPLHC4R) lpMemRes)[uDimRes + uAcc * uDimHi].parts[i],
                                          &atRht.aplHC4R.parts[i]);
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HC8R:
            // Initialize the temp
            mphc8r_init (&atRep.aplHC8R);

            if (IsSingleton (aplNELMRht))
                // Loop through all of the parts
                for (i = 0; i < 8; i++)
                    mpq_set (&atRep.aplHC8R.parts[i],
                             &((LPAPLHC8R) aplNestRep)->parts[i]);
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        // Loop through all of the parts
                        for (i = 0; i < 8; i++)
                            mpq_set_si (&atRht.aplHC8R.parts[i], 0, 1);
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        // Loop through all of the parts
                        for (i = 0; i < 8; i++)
                            mpq_set (&atRht.aplHC8R.parts[i],
                                     &((LPAPLHC8R) lpMemRht)[uDimRht + uAx * uDimHi].parts[i]);
                    else
                        // Loop through all of the parts
                        for (i = 0; i < 8; i++)
                            // Use singleton right arg value
                            mpq_set (&atRht.aplHC8R.parts[i], &atRep.aplHC8R.parts[i]);

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Loop through all of the parts
                        for (i = 0; i < 8; i++)
                            mpq_init_set (&((LPAPLHC8R) lpMemRes)[uDimRes + uAcc * uDimHi].parts[i],
                                          &atRht.aplHC8R.parts[i]);
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HC2V:
            // Initialize the temps
            mphc2v_init0 (&atRep.aplHC2V);
            mphc2v_init0 (&atRht.aplHC2V);

            if (IsSingleton (aplNELMRht))
                // Loop through all of the parts
                for (i = 0; i < 2; i++)
                    mpfr_copy (&atRep.aplHC2V.parts[i],
                               &((LPAPLHC2V) aplNestRep)->parts[i]);

            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        // Loop through all of the parts
                        for (i = 0; i < 2; i++)
                            mpfr_set_zero (&atRht.aplHC2V.parts[i], 1);
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        // Loop through all of the parts
                        for (i = 0; i < 2; i++)
                            mpfr_copy (&atRht.aplHC2V.parts[i],
                                       &((LPAPLHC2V) lpMemRht)[uDimRht + uAx * uDimHi].parts[i]);
                    else
                        // Loop through all of the parts
                        for (i = 0; i < 2; i++)
                            // Use singleton right arg value
                            mpfr_set (&atRht.aplHC2V.parts[i], &atRep.aplHC2V.parts[i], MPFR_RNDN);

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Loop through all of the parts
                        for (i = 0; i < 2; i++)
                            mpfr_init_copy (&((LPAPLHC2V) lpMemRes)[uDimRes + uAcc * uDimHi].parts[i],
                                            &atRht.aplHC2V.parts[i]);
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HC4V:
            // Initialize the temps
            mphc4v_init0 (&atRep.aplHC4V);
            mphc4v_init0 (&atRht.aplHC4V);

            if (IsSingleton (aplNELMRht))
                // Loop through all of the parts
                for (i = 0; i < 4; i++)
                    mpfr_copy (&atRep.aplHC4V.parts[i],
                               &((LPAPLHC4V) aplNestRep)->parts[i]);

            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        // Loop through all of the parts
                        for (i = 0; i < 4; i++)
                            mpfr_set_zero (&atRht.aplHC4V.parts[i], 1);
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        // Loop through all of the parts
                        for (i = 0; i < 4; i++)
                            mpfr_copy (&atRht.aplHC4V.parts[i],
                                       &((LPAPLHC4V) lpMemRht)[uDimRht + uAx * uDimHi].parts[i]);
                    else
                        // Loop through all of the parts
                        for (i = 0; i < 4; i++)
                            // Use singleton right arg value
                            mpfr_set (&atRht.aplHC4V.parts[i], &atRep.aplHC4V.parts[i], MPFR_RNDN);

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Loop through all of the parts
                        for (i = 0; i < 4; i++)
                            mpfr_init_copy (&((LPAPLHC4V) lpMemRes)[uDimRes + uAcc * uDimHi].parts[i],
                                            &atRht.aplHC4V.parts[i]);
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HC8V:
            // Initialize the temps
            mphc8v_init0 (&atRep.aplHC8V);
            mphc8v_init0 (&atRht.aplHC8V);

            if (IsSingleton (aplNELMRht))
                // Loop through all of the parts
                for (i = 0; i < 8; i++)
                    mpfr_copy (&atRep.aplHC8V.parts[i],
                               &((LPAPLHC8V) aplNestRep)->parts[i]);

            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (!IsSingleton (aplNELMLft))
                        iLen = lpMemRep[uAx];
                    else
                        // Use singleton left arg value
                        iLen = iLftLen;

                    // If the length is negative, ...
                    if (iLen < 0)
                    {
                        // Loop through all of the parts
                        for (i = 0; i < 8; i++)
                            mpfr_set_zero (&atRht.aplHC8V.parts[i], 1);
                        iLen = abs64 (iLen);
                    } else
                    if (!IsSingleton (aplNELMRht))
                        // Loop through all of the parts
                        for (i = 0; i < 8; i++)
                            mpfr_copy (&atRht.aplHC8V.parts[i],
                                       &((LPAPLHC8V) lpMemRht)[uDimRht + uAx * uDimHi].parts[i]);
                    else
                        // Loop through all of the parts
                        for (i = 0; i < 8; i++)
                            // Use singleton right arg value
                            mpfr_set (&atRht.aplHC8V.parts[i], &atRep.aplHC8V.parts[i], MPFR_RNDN);

                    // Loop through the repetitions
                    for (iRep = 0; iRep < iLen; iRep++, uAcc++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Loop through all of the parts
                        for (i = 0; i < 8; i++)
                            mpfr_init_copy (&((LPAPLHC8V) lpMemRes)[uDimRes + uAcc * uDimHi].parts[i],
                                            &atRht.aplHC8V.parts[i]);
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        defstop
            break;
    } // End SWITCH
PROTO_EXIT:
    // Unlock the result global memory in case TypeDemote actually demotes
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF
YYALLOC_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken, FALSE);

    goto NORMAL_EXIT;

LEFT_RANK_EXIT:
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
    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_APA:
        case ARRAY_CHAR:
        case ARRAY_HC2I:
        case ARRAY_HC2F:
        case ARRAY_HC4I:
        case ARRAY_HC4F:
        case ARRAY_HC8I:
        case ARRAY_HC8F:
        case ARRAY_NESTED:
        case ARRAY_HETERO:
            break;

        case ARRAY_RAT:
            // We no longer need this storage
            Myq_clear (&atRep.aplRat);
            Myq_clear (&atRht.aplRat);

            break;

        case ARRAY_HC2R:
            // We no longer need this storage
            Myhc2r_clear (&atRep.aplHC2R);
            Myhc2r_clear (&atRht.aplHC2R);

            break;

        case ARRAY_HC4R:
            // We no longer need this storage
            Myhc4r_clear (&atRep.aplHC4R);
            Myhc4r_clear (&atRht.aplHC4R);

            break;

        case ARRAY_HC8R:
            // We no longer need this storage
            Myhc8r_clear (&atRep.aplHC8R);
            Myhc8r_clear (&atRht.aplHC8R);

            break;

        case ARRAY_VFP:
            // We no longer need this storage
            Myf_clear (&atRep.aplVfp);
            Myf_clear (&atRht.aplVfp);

            break;

        case ARRAY_HC2V:
            // We no longer need this storage
            Myhc2v_clear (&atRep.aplHC2V);
            Myhc2v_clear (&atRht.aplHC2V);

            break;

        case ARRAY_HC4V:
            // We no longer need this storage
            Myhc4v_clear (&atRep.aplHC4V);
            Myhc4v_clear (&atRht.aplHC4V);

            break;

        case ARRAY_HC8V:
            // We no longer need this storage
            Myhc8v_clear (&atRep.aplHC8V);
            Myhc8v_clear (&atRht.aplHC8V);

            break;

        defstop
            break;
    } // End SWITCH

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbRep, lpMemRep);

    if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnDydSlash_EM_YY


//***************************************************************************
//  End of File: pf_slash.c
//***************************************************************************
