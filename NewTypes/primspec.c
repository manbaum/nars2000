//***************************************************************************
//  NARS2000 -- Primitive Function Special
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2017 Sudley Place Software

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

#define EXTERN
#define DEFINE_VALUES
#include "primspec.mac"
#undef  DEFINE_VALUES
#undef  EXTERN


//***************************************************************************
//  $PrimFnSyntaxError_EM
//
//  Primitive function SYNTAX ERROR
//***************************************************************************

LPPL_YYSTYPE PrimFnSyntaxError_EM
    (LPTOKEN lptkFunc
#ifdef DEBUG
, LPWCHAR APPEND_NAME
#endif
    )

{
#ifdef DEBUG
    static WCHAR wszTemp[1024];

    // Copy the error message to the temp
    MyStrcpyW (wszTemp, sizeof (wszTemp), ERRMSG_SYNTAX_ERROR);

    // Append the suffix to the temp
    MyStrcatW (wszTemp, sizeof (wszTemp), APPEND_NAME);

    ErrorMessageIndirectToken (wszTemp, lptkFunc);
#else
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR,
                               lptkFunc);
#endif
    return NULL;
} // End PrimFnSyntaxError_EM


//***************************************************************************
//  $PrimFnValenceError_EM
//
//  Primitive function VALENCE ERROR
//***************************************************************************

LPPL_YYSTYPE PrimFnValenceError_EM
    (LPTOKEN lptkFunc
#ifdef DEBUG
, LPWCHAR APPEND_NAME
#endif
    )

{
#ifdef DEBUG
    static WCHAR wszTemp[1024];

    // Copy the error message to the temp
    MyStrcpyW (wszTemp, sizeof (wszTemp), ERRMSG_VALENCE_ERROR);

    // Append the suffix to the temp
    MyStrcatW (wszTemp, sizeof (wszTemp), APPEND_NAME);

    ErrorMessageIndirectToken (wszTemp, lptkFunc);
#else
    ErrorMessageIndirectToken (ERRMSG_VALENCE_ERROR,
                               lptkFunc);
#endif
    return NULL;
} // End PrimFnValenceError_EM


//***************************************************************************
//  $PrimFnNonceError_EM
//
//  Primitive function NONCE ERROR
//***************************************************************************

LPPL_YYSTYPE PrimFnNonceError_EM
    (LPTOKEN lptkFunc
#ifdef DEBUG
, LPWCHAR APPEND_NAME
#endif
    )

{
#ifdef DEBUG
    static WCHAR wszTemp[1024];

    // Copy the error message to the temp
    MyStrcpyW (wszTemp, sizeof (wszTemp), ERRMSG_NONCE_ERROR);

    // Append the suffix to the temp
    MyStrcatW (wszTemp, sizeof (wszTemp), APPEND_NAME);

    ErrorMessageIndirectToken (wszTemp, lptkFunc);
#else
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR,
                               lptkFunc);
#endif
    return NULL;
} // End PrimFnNonceError_EM


//***************************************************************************
//  $PrimFnMonNonceError_RE
//
//  Primitive function NONCE ERROR
//***************************************************************************

void PrimFnMonNonceError_RE
    (LPVOID     lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    NONCE_RE                        // PrimFnMonNonceError_RE
} // End PrimFnMonNonceError_RE


//***************************************************************************
//  $PrimFnDydNonceError_RE
//
//  Primitive function NONCE ERROR
//***************************************************************************

void PrimFnDydNonceError_RE
    (LPVOID     lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    NONCE_RE                        // PrimFnDydNonceError_RE
} // End PrimFnDydNonceError_RE


//***************************************************************************
//  $PrimFnDomainError_EM
//
//  Primitive function DOMAIN ERROR
//***************************************************************************

LPPL_YYSTYPE PrimFnDomainError_EM
    (LPTOKEN lptkFunc
#ifdef DEBUG
, LPWCHAR APPEND_NAME
#endif
    )

{
#ifdef DEBUG
    static WCHAR wszTemp[1024];

    // Copy the error message to the temp
    MyStrcpyW (wszTemp, sizeof (wszTemp), ERRMSG_DOMAIN_ERROR);

    // Append the suffix to the temp
    MyStrcatW (wszTemp, sizeof (wszTemp), APPEND_NAME);

    ErrorMessageIndirectToken (wszTemp, lptkFunc);
#else
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR,
                               lptkFunc);
#endif
    return NULL;
} // End PrimFnDomainError_EM


//***************************************************************************
//  $PrimFnValueError_EM
//
//  Primitive function VALUE ERROR
//***************************************************************************

LPPL_YYSTYPE PrimFnValueError_EM
    (LPTOKEN lptkFunc
#ifdef DEBUG
, LPWCHAR APPEND_NAME
#endif
    )

{
#ifdef DEBUG
    static WCHAR wszTemp[1024];

    // Copy the error message to the temp
    MyStrcpyW (wszTemp, sizeof (wszTemp), ERRMSG_VALUE_ERROR);

    // Append the suffix to the temp
    MyStrcatW (wszTemp, sizeof (wszTemp), APPEND_NAME);

    // If there's a name attached to this error, ...
    if ((IsTknNamed   (lptkFunc)
      || IsTknAFO     (lptkFunc))
     && GetPtrTypeDir (lptkFunc->tkData.tkVoid) EQ PTRTYPE_STCONST
     && lptkFunc->tkData.tkSym->stHshEntry)
    {
        // Leading separator
        MyStrcatW (wszTemp, sizeof (wszTemp), L" (");

        // Append the name
        MyStrcatW (wszTemp, sizeof (wszTemp), lptkFunc->tkData.tkSym->stHshEntry->lpwCharName);

        // Trailing separator
        MyStrcatW (wszTemp, sizeof (wszTemp), L")");
    } // End IF

    ErrorMessageIndirectToken (wszTemp, lptkFunc);
#else
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR,
                               lptkFunc);
#endif
    return NULL;
} // End PrimFnValueError_EM


//***************************************************************************
//  $PrimFnMonValenceError_EM
//
//  Primitive scalar monadic function VALENCE ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonValenceError_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonValenceError_EM
    (LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis,           // Ptr to axis token (may be NULL)
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimFnMonValenceError_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnMixed_EM_YY
//
//  Generate a prototype result for the monadic & dyadic primitive mixed functions
//***************************************************************************

LPPL_YYSTYPE PrimProtoFnMixed_EM_YY
    (LPPRIMFNS  lpPrimFn,           // Ptr to primitive function routine
     LPTOKEN    lptkLftArg,         // Ptr to left arg token
     LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis)           // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result
    HGLOBAL      hGlbTmp,           // Temporary global memory handle
                 hSymGlbProto;      // Prototype ...

    // Call the original function
    lpYYRes = (*lpPrimFn) (lptkLftArg,      // Ptr to left arg token
                           lptkFunc,        // Ptr to function token
                           lptkRhtArg,      // Ptr to right arg token
                           lptkAxis);       // Ptr to axis token (may be NULL)
    if (lpYYRes NE NULL)
    // As this is a prototype function, convert
    //   the result to a prototype
    // Split cases based upon the result token type
    switch (lpYYRes->tkToken.tkFlags.TknType)
    {
        case TKT_VARIMMED:
            if (IsImmChr (lpYYRes->tkToken.tkFlags.ImmType))
                lpYYRes->tkToken.tkData.tkChar = L' ';
            else
            {
                lpYYRes->tkToken.tkFlags.ImmType  = IMMTYPE_BOOL;
                lpYYRes->tkToken.tkData.tkBoolean = FALSE;
            } // End IF/ELSE

            break;

        case TKT_VARARRAY:
            // Get the global memory handle
            hGlbTmp = lpYYRes->tkToken.tkData.tkGlbData;

            // Make the prototype
            hSymGlbProto =
              MakeMonPrototype_EM_PTB (hGlbTmp,         // Proto arg handle
                                       ARRAY_NESTED,    // Array storage tyoe
                                       lptkFunc,        // Ptr to function token
                                       MP_CHARS);       // CHARs allowed
            if (hSymGlbProto EQ NULL)
            {
                YYFree (lpYYRes); lpYYRes = NULL;
            } else
            {
                // Save back into the result
                lpYYRes->tkToken.tkData.tkGlbData = hSymGlbProto;

                // We no longer need this storage
                FreeResultGlobalVar (hGlbTmp); hGlbTmp = NULL;
            } // End IF/ELSE

            break;

        defstop
            break;
    } // End IF/SWITCH

    return lpYYRes;
} // End PrimProtoFnMixed_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnScalar_EM_YY
//
//  Generate a prototype result for the monadic & dyadic primitive scalar functions
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnScalar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnScalar_EM_YY
    (LPTOKEN    lptkLftArg,         // Ptr to left arg token
     LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis)           // Ptr to axis token (may be NULL)

{
    HGLOBAL      hGlbLft,           // Left arg global memory handle
                 hGlbRht,           // Right ...
                 hSymGlbRes;        // Result   ...
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result

    // Get right arg's global memory handle
    hGlbRht = GetGlbHandle (lptkRhtArg);

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
    {
        //***************************************************************
        // Scalar monadic primitive functions are not sensitive
        //   to the axis operator, so signal a syntax error if present
        //***************************************************************
        if (lptkAxis NE NULL)
            goto AXIS_SYNTAX_EXIT;

        //***************************************************************
        // Called monadically
        //***************************************************************

        // Make the prototype
        hSymGlbRes =
          MakeMonPrototype_EM_PTB (hGlbRht,         // Proto arg handle
                                   ARRAY_NESTED,    // Array storage tyoe
                                   lptkFunc,        // Ptr to function token
                                   MP_NUMONLY);     // Numerics only
    } else
    {
        //***************************************************************
        // Called dyadically
        //***************************************************************

        // Get left arg's global memory handle
        hGlbLft = GetGlbHandle (lptkLftArg);

        // Check for both args immediate
        if (hGlbLft EQ NULL
         && hGlbRht EQ NULL)
        {
            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_BOOL;
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkInteger  = 0;
            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

            goto NORMAL_EXIT;
        } // End IF

        // If the left arg is global, ...
        if (hGlbLft NE NULL)
            // Set the ptr type bits
            hGlbLft = MakePtrTypeGlb (hGlbLft);
        // If the right arg is global, ...
        if (hGlbRht NE NULL)
            // Set the ptr type bits
            hGlbRht = MakePtrTypeGlb (hGlbRht);

        // Handle as dyadic prototype
        hSymGlbRes =
          MakeDydPrototype_EM_PTB (hGlbLft,                     // Left arg global memory handle (may be NULL if immediate)
                                   GetImmedType (lptkLftArg),   // Left arg immediate type
                                   lptkFunc,                    // Ptr to function token
                                   hGlbRht,                     // Right arg global memory handle
                                   GetImmedType (lptkRhtArg),   // Left arg immediate type
                                   lptkAxis);                   // Ptr to axis value token (may be NULL)
    } // End IF

    if (hSymGlbRes EQ NULL)
        goto ERROR_EXIT;

    Assert (GetPtrTypeDir (hSymGlbRes) EQ PTRTYPE_HGLOBAL);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = hSymGlbRes;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return lpYYRes;
} // End PrimProtoFnScalar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimIdentFnScalar_EM_YY
//
//  Generate an identity element for a primitive scalar dyadic function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimIdentFnScalar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimIdentFnScalar_EM_YY
    (LPTOKEN lptkRhtOrig,               // Ptr to original right arg token
     LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to identity element result
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeRes;           // Result    ...
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMRes,           // Result    ...
                      aplNELMAxis;          // Axis      ...
    APLRANK           aplRankRht,           // Right arg rank
                      aplRankRes;           // Result    ...
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL,       // Result    ...
                      hGlbAxis = NULL;      // Axis      ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPVOID            lpMemRht,             // Ptr to right arg global memory
                      lpMemRes;             // Ptr to result    ...
    LPAPLDIM          lpMemDimRht,          // Ptr to right arg dimensions
                      lpMemDimRes;          // Ptr to result    ...
    APLLONGEST        aplLongestRht;        // Right arg immediate value
    APLUINT           ByteRes;              // # bytes in the result
    LPAPLUINT         lpMemAxisHead = NULL, // Ptr to axis values, fleshed out by CheckAxis_EM
                      lpMemAxisTail = NULL; // Ptr to grade up of AxisHead
    LPPRIMSPEC        lpPrimSpec;           // Ptr to function PRIMSPEC
    LPPRIMFLAGS       lpPrimFlags;          // Ptr to function PrimFlags entry
    LPPRIMIDENT       lpPrimIdent;          // Ptr to function PrimIdent entry
    APLUINT           uRht;                 // Loop counter

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig NE NULL);
    Assert (lptkFunc    NE NULL);
    Assert (lptkRhtArg  NE NULL);

    // Get the corresponding lpPrimSpec
    lpPrimSpec = PrimSpecTab[SymTrans (lptkFunc)];

    // Get a ptr to the Primitive Function Flags
    lpPrimFlags = GetPrimFlagsPtr (lptkFunc);

    // If there's an identity element, ...
    if (lpPrimFlags NE NULL && lpPrimFlags->IdentElem)
        lpPrimIdent = &PrimIdent[lpPrimFlags->Index];
    else
        goto DOMAIN_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Check the axis values, fill in # elements in axis
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRht,      // All values less than this
                           FALSE,           // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // TRUE iff fractional values allowed
                           NULL,            // Return last axis value
                          &aplNELMAxis,     // Return # elements in axis vector
                          &hGlbAxis))       // Return HGLOBAL with APLINT axis values
            return NULL;

        // Lock the memory to get a ptr to it
        lpMemAxisHead = MyGlobalLockInt (hGlbAxis);

        // Get pointer to the axis tail (where the [X] values are)
        lpMemAxisTail = &lpMemAxisHead[aplRankRht - aplNELMAxis];
    } else
        // No axis is the same as all axes
        aplNELMAxis = aplRankRht;

    // The result rank is the NELM axis
    aplRankRes = aplNELMAxis;

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMRht,
                                               &aplTypeRht,
                                                lptkFunc,
                                                aplNELMRht,
                                               &aplTypeRht);
    if (IsNonceType (aplTypeRes))
        goto NONCE_EXIT;

    if (IsErrorType (aplTypeRes))
        goto DOMAIN_EXIT;

    Assert (IsSimpleGlbNum (aplTypeRes)
         || IsNested (aplTypeRes));

    // If the result is simple numeric, ...
    if (IsSimpleNum (aplTypeRes))
    {
        // If the identity element is simple Boolean, ...
        if (lpPrimIdent->IsBool)
            // Make it Boolean
            aplTypeRes = ARRAY_BOOL;
        else
            // Make it float
            aplTypeRes = ARRAY_FLOAT;
    } // End IF

    // Get right arg global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the right arg is a simple scalar, ...
    if (hGlbRht EQ NULL)
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = lpPrimIdent->IsBool ? IMMTYPE_BOOL
                                                                 : IMMTYPE_FLOAT;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        if (lpPrimIdent->IsBool)
            lpYYRes->tkToken.tkData.tkBoolean = lpPrimIdent->bIdentElem;
        else
            lpYYRes->tkToken.tkData.tkFloat   = lpPrimIdent->fIdentElem;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        goto NORMAL_EXIT;
    } // End IF

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

    //***************************************************************
    //  Code from here on down to the call to <PrimFnScalarCommon_EM>
    //    can be incorporated into that routine as an optimization.
    //***************************************************************

    //***************************************************************
    //  Calculate result NELM
    //***************************************************************
    if (lpMemAxisTail NE NULL)
        for (aplNELMRes = 1, uRht = 0; uRht < aplRankRes; uRht++)
            aplNELMRes *= lpMemDimRht[lpMemAxisTail[uRht]];
    else
        aplNELMRes = aplNELMRht;

    //***************************************************************
    // Calculate space needed for the result
    //***************************************************************
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    //***************************************************************
    // Check for overflow
    //***************************************************************
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Now we can allocate the storage for the result
    //***************************************************************
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        lpMemHdrRes
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    //***************************************************************
    // Fill in the result's dimension
    //***************************************************************
    if (lpMemAxisTail NE NULL)
        for (uRht = 0; uRht < aplRankRes; uRht++)
            *lpMemDimRes++ = lpMemDimRht[lpMemAxisTail[uRht]];
    else
        CopyMemory (lpMemDimRes, lpMemDimRht, (APLU3264) aplRankRes * sizeof (APLDIM));

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // If the right arg (and result) is simple or Global (numeric), ...
    if (IsSimpleGlbNum (aplTypeRht))
    {
        // Fill in the result data
        if (lpPrimIdent->IsBool)
        {
            // Split cases based upon the storage type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:
                    // If the identity element is 1, ...
                    if (lpPrimIdent->bIdentElem)
                        FillMemory (lpMemRes, (APLU3264) RoundUpBitsToBytes (aplNELMRes), 0xFF);
                    break;

                case ARRAY_RAT:
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        mpq_init_set_sx (((LPAPLRAT) lpMemRes)++, lpPrimIdent->bIdentElem, 1);
                    break;

                case ARRAY_VFP:
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        mpfr_init_set_si (((LPAPLVFP) lpMemRes)++, lpPrimIdent->bIdentElem, MPFR_RNDN);
                    break;

                case ARRAY_HC2R:
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        mphc2r_init_set_sx (((LPAPLHC2R) lpMemRes)++, lpPrimIdent->bIdentElem, 1);
                    break;

                case ARRAY_HC2V:
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        mphc2v_init_set_sx (((LPAPLHC2V) lpMemRes)++, lpPrimIdent->bIdentElem);
                    break;

                case ARRAY_HC4R:
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        mphc4r_init_set_sx (((LPAPLHC4R) lpMemRes)++, lpPrimIdent->bIdentElem, 1);
                    break;

                case ARRAY_HC4V:
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        mphc4v_init_set_si (((LPAPLHC4V) lpMemRes)++, lpPrimIdent->bIdentElem);
                    break;

                case ARRAY_HC8R:
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        mphc8r_init_set_sx (((LPAPLHC8R) lpMemRes)++, lpPrimIdent->bIdentElem, 1);
                    break;

                case ARRAY_HC8V:
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        mphc8v_init_set_si (((LPAPLHC8V) lpMemRes)++, lpPrimIdent->bIdentElem);
                    break;

                defstop
                    break;
            } // End SWITCH
        } else
        {
            // Split cases based upon the storage type
            switch (aplTypeRes)
            {
                case ARRAY_FLOAT:
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        *((LPAPLFLOAT) lpMemRes)++ = lpPrimIdent->fIdentElem;
                    break;

                case ARRAY_RAT:
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        mpq_init_set_d  (((LPAPLRAT) lpMemRes)++, lpPrimIdent->fIdentElem);
                    break;

                case ARRAY_VFP:
                    for (uRht = 0; uRht < aplNELMRes; uRht++)
                        mpfr_init_set_d (((LPAPLVFP) lpMemRes)++, lpPrimIdent->fIdentElem, MPFR_RNDN);
                    break;

                defstop
                    break;
            } // End SWITCH
        } // End IF/ELSE
    } else
    {
        // The result is nested
        Assert (IsNested (aplTypeRes));

        // Handle nested prototypes
        aplNELMRht = max (aplNELMRht, 1);

        // Recurse into the array
        if (!PrimIdentFnScalarCommon_EM (lpMemRht,          // Ptr to right arg global memory data
                                         lpMemRes,          // ...    result ...
                                         aplNELMRht,        // Right arg NELM
                                         lpPrimIdent,       // Ptr to function PrimIdent entry
                                         lpPrimSpec,        // Ptr to function PRIMSPEC
                                         lptkAxis,          // Ptr to axis token
                                         lptkFunc))         // Ptr to function token
            goto ERROR_EXIT;
    } // End IF/ELSE

    // Unlock the result global memory in case TypeDemote actually demotes
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken, lpPrimSpec->bDydDimDemote);

    goto NORMAL_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
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

    if (hGlbAxis NE NULL && lpMemAxisHead NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbAxis); lpMemAxisHead = lpMemAxisTail = NULL;
    } // End IF

    return lpYYRes;
} // End PrimIdentFnScalar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimIdentFnScalarCommon_EM
//
//  Common (recursive) routine to PrimIdentScalar_EM_YY
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimIdentFnScalarCommon_EM_YY"
#else
#define APPEND_NAME
#endif

UBOOL PrimIdentFnScalarCommon_EM
    (LPAPLHETERO lpMemRht,                  // Ptr to right arg global memory data
     LPAPLHETERO lpMemRes,                  // ...    result ...
     APLNELM     aplNELMRht,                // Right arg NELM
     LPPRIMIDENT lpPrimIdent,               // Ptr to function PrimIdent entry
     LPPRIMSPEC  lpPrimSpec,                // Ptr to function PRIMSPEC
     LPTOKEN     lptkAxis,                  // Ptr to axis token
     LPTOKEN     lptkFunc)                  // Ptr to function token

{
    APLUINT           uRht;                 // Loop counter
    HGLOBAL           hGlbItm = NULL,       // Item global memory handle
                      hGlbRes2 = NULL,      // Result2 ...
                      hGlbAxis = NULL;      // Axis ...
    APLSTYPE          aplTypeItm,           // Item storage type
                      aplTypeRes2;          // Result2 ...
    APLNELM           aplNELMItm,           // Item NELM
                      aplNELMRes2,          // Result2 ...
                      aplNELMAxis;          // Axis ...
    APLRANK           aplRankItm,           // Item rank
                      aplRankRes2;          // Result2 rank
    LPVARARRAY_HEADER lpMemHdrItm = NULL,   // Ptr to item header
                      lpMemHdrRes2 = NULL;  // ...    result2 ...
    LPVOID            lpMemItm,             // Ptr to item global memory
                      lpMemRes2;            // ...    result2 ...
    UBOOL             bRet = FALSE;         // TRUE iff the result is valid
    APLUINT           ByteRes2;             // # bytes in the result2
    LPAPLDIM          lpMemDimItm,          // Ptr to item dimensions
                      lpMemDimRes2;         // Ptr to result2    ...
    LPAPLUINT         lpMemAxisHead = NULL, // Ptr to axis values, fleshed out by CheckAxis_EM
                      lpMemAxisTail = NULL; // Ptr to grade up of AxisHead

    // Loop through the right arg
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Split cases based upon the ptr type bits
        switch (GetPtrTypeDir (lpMemRht[uRht]))
        {
            case PTRTYPE_STCONST:
                // Save the identity element
                *lpMemRes++ =
                  MakeSymEntry_EM (lpMemRht[uRht]->stFlags.ImmType,     // Immediate type
                                  &lpMemRht[uRht]->stData.stLongest,    // Ptr to immediate value
                                   lptkFunc);                           // Ptr to function token
                break;

            case PTRTYPE_HGLOBAL:
                // Get the item's global memory handle
                hGlbItm = lpMemRht[uRht];

                // Lock the memory to get a ptr to it
                lpMemHdrItm = MyGlobalLockVar (hGlbItm);

                // Get the item's attributes
#define lpHeader        lpMemHdrItm
                aplTypeItm = lpHeader->ArrType;
                aplNELMItm = lpHeader->NELM;
                aplRankItm = lpHeader->Rank;
#undef  lpHeader
                // Check for axis present
                if (lptkAxis NE NULL)
                {
                    // Check the axis values, fill in # elements in axis
                    if (!CheckAxis_EM (lptkAxis,        // The axis token
                                       aplRankItm,      // All values less than this
                                       FALSE,           // TRUE iff scalar or one-element vector only
                                       FALSE,           // TRUE iff want sorted axes
                                       FALSE,           // TRUE iff axes must be contiguous
                                       FALSE,           // TRUE iff duplicate axes are allowed
                                       NULL,            // TRUE iff fractional values allowed
                                       NULL,            // Return last axis value
                                      &aplNELMAxis,     // Return # elements in axis vector
                                      &hGlbAxis))       // Return HGLOBAL with APLINT axis values
                        goto ERROR_EXIT;

                    // Lock the memory to get a ptr to it
                    lpMemAxisHead = MyGlobalLockInt (hGlbAxis);

                    // Get pointer to the axis tail (where the [X] values are)
                    lpMemAxisTail = &lpMemAxisHead[aplRankItm - aplNELMAxis];
                } else
                    // No axis is the same as all axes
                    aplNELMAxis = aplRankItm;

                // The result2 rank is the NELM axis
                aplRankRes2 = aplNELMAxis;

                // Calculate the storage type of the result2
                aplTypeRes2 = (*lpPrimSpec->StorageTypeDyd) (aplNELMItm,
                                                            &aplTypeItm,
                                                             lptkFunc,
                                                             aplNELMItm,
                                                            &aplTypeItm);
                if (IsNonceType (aplTypeRes2))
                    goto NONCE_EXIT;

                if (IsErrorType (aplTypeRes2))
                    goto DOMAIN_EXIT;

                Assert (IsSimpleNum (aplTypeRes2)
                     || IsNested (aplTypeRes2));

                // If the result2 is simple numeric, ...
                if (IsSimpleNum (aplTypeRes2))
                {
                    // If the identity element is simple Boolean, ...
                    if (lpPrimIdent->IsBool)
                        // Make it Boolean
                        aplTypeRes2 = ARRAY_BOOL;
                    else
                        // Make it float
                        aplTypeRes2 = ARRAY_FLOAT;
                } // End IF

                // Skip over the header to the dimensions
                lpMemDimItm = VarArrayBaseToDim (lpMemHdrItm);

                //***************************************************************
                //  Calculate result2 NELM
                //***************************************************************
                if (lpMemAxisTail NE NULL)
                    for (aplNELMRes2 = 1, uRht = 0; uRht < aplRankRes2; uRht++)
                        aplNELMRes2 *= lpMemDimItm[lpMemAxisTail[uRht]];
                else
                    aplNELMRes2 = aplNELMItm;

                //***************************************************************
                // Calculate space needed for the result2
                //***************************************************************
                ByteRes2 = CalcArraySize (aplTypeRes2, aplNELMRes2, aplRankRes2);

                //***************************************************************
                // Check for overflow
                //***************************************************************
                if (ByteRes2 NE (APLU3264) ByteRes2)
                    goto WSFULL_EXIT;

                //***************************************************************
                // Now we can allocate the storage for the result
                //***************************************************************
                hGlbRes2 = DbgGlobalAlloc (GHND, (APLU3264) ByteRes2);
                if (hGlbRes2 EQ NULL)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemHdrRes2 = MyGlobalLock000 (hGlbRes2);

#define lpHeader        lpMemHdrRes2
                // Fill in the header
                lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                lpHeader->ArrType    = aplTypeRes2;
////////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
                lpHeader->RefCnt     = 1;
                lpHeader->NELM       = aplNELMRes2;
                lpHeader->Rank       = aplRankRes2;
#undef  lpHeader

                // Skip over the header to the dimensions
                lpMemDimRes2 = VarArrayBaseToDim (lpMemHdrRes2);

                //***************************************************************
                // Fill in the result's dimension
                //***************************************************************
                if (lpMemAxisTail NE NULL)
                    for (uRht = 0; uRht < aplRankRes2; uRht++)
                        *lpMemDimRes2++ = lpMemDimItm[lpMemAxisTail[uRht]];
                else
                    CopyMemory (lpMemDimRes2, lpMemDimItm, (APLU3264) aplRankRes2 * sizeof (APLDIM));

                // Skip over the header and dimensions to the data
                lpMemItm  = VarArrayDataFmBase (lpMemHdrItm );
                lpMemRes2 = VarArrayDataFmBase (lpMemHdrRes2);

                // If the item (and result2) is simple (numeric), ...
                if (IsSimpleNum (aplTypeItm))
                {
                    // Fill in the result data
                    if (lpPrimIdent->IsBool)
                    {
                        Assert (IsSimpleBool (aplTypeRes2));

                        if (lpPrimIdent->bIdentElem)
                            FillMemory (lpMemRes2, (APLU3264) RoundUpBitsToBytes (aplNELMRes2), 0xFF);
                    } else
                    {
                        Assert (IsSimpleFlt (aplTypeRes2));

                        for (uRht = 0; uRht < aplNELMRes2; uRht++)
                            *((LPAPLFLOAT) lpMemRes2)++ = lpPrimIdent->fIdentElem;
                    } // End IF/ELSE
                } else
                {
                    // The result is nested
                    Assert (IsNested (aplTypeRes2));

                    // Handle nested prototypes
                    aplNELMItm = max (aplNELMItm, 1);

                    // Recurse into the array
                    if (!PrimIdentFnScalarCommon_EM (lpMemItm,          // Ptr to item global memory data
                                                     lpMemRes2,         // ...    result2 ...
                                                     aplNELMItm,        // Item NELM
                                                     lpPrimIdent,       // Ptr to function PrimIdent entry
                                                     lpPrimSpec,        // Ptr to function PRIMSPEC
                                                     lptkAxis,          // Ptr to axis token
                                                     lptkFunc))         // Ptr to function token
                        goto ERROR_EXIT;
                } // End IF/ELSE

                // We no longer need these ptrs
                MyGlobalUnlock (hGlbItm);  lpMemHdrItm  = NULL;
                MyGlobalUnlock (hGlbRes2); lpMemHdrRes2 = NULL;

                // Save in the result
                *lpMemRes++ = MakePtrTypeGlb (hGlbRes2);

                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes2 NE NULL)
    {
        if (lpMemHdrRes2 NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes2); lpMemHdrRes2 = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes2); hGlbRes2 = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes2 NE NULL && lpMemHdrRes2 NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes2); lpMemHdrRes2 = NULL;
    } // End IF

    if (hGlbItm NE NULL && lpMemHdrItm NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbItm); lpMemHdrItm = NULL;
    } // End IF

    if (hGlbAxis NE NULL && lpMemAxisTail NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbAxis); lpMemAxisHead = lpMemAxisTail = NULL;
    } // End IF

    return bRet;
} // End PrimIdentFnScalarCommon_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMon_EM_YY
//
//  Primitive scalar monadic function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMon_EM_YY
    (LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis,           // Ptr to axis token (may be NULL)
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    HGLOBAL      hGlbRes,           // Result global memory handle
                 hOldGlbRes;        // Old ...
    APLSTYPE     aplTypeRes,        // Result storage type
                 aplTypeRht;        // Right arg storage type
    APLRANK      aplRankRht;        // Right arg rank
    APLNELM      aplNELMRht;        // Right arg NELM
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result
    PRIMSPEC     LclPrimSpec;       // Writable copy of PRIMSPEC
    APLVFP       mpfArg = {0},      // VFP arg
                 mpfRes = {0};      // VFP result
    ATISAT      *lpPrimFn;          // Ptr to PSDF function
    ALLTYPES     atRht = {0};       // Right arg as ALLTYPES
    UBOOL        bGlbEntry = FALSE; // TRUE iff we created a new global entry due to an exception
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Check for axis present
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Save a writable copy of PRIMSPEC
    LclPrimSpec = *lpPrimSpec;

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Handle prototypes separately
    if (IsEmpty (aplNELMRht))
        return PrimProtoFnScalar_EM_YY (NULL,       // Ptr to left arg token
                                        lptkFunc,   // Ptr to function token
                                        lptkRhtArg, // Ptr to right arg token
                                        lptkAxis);  // Ptr to axis token (may be NULL)
    // Get the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeMon) (aplNELMRht,
                                               &aplTypeRht,
                                                lptkFunc);
    // Check for DOMAIN ERROR
    if (IsErrorType (aplTypeRes))
        goto DOMAIN_EXIT;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARIMMED:
RESTART_EXCEPTION_VARIMMED:
            // In order to make roll atomic, save the current []RL into LclPrimSpec
            SavePrimSpecRL (&LclPrimSpec);

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType   = TranslateArrayTypeToImmType (aplTypeRes);
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkData            =        // Filled in below
            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

            __try
            {
                // Split cases based upon the result storage type
                switch (aplTypeRes)
                {
                    case ARRAY_BOOL:            // Res = BOOL
                        // Split cases based upon the right arg's storage type
                        switch (lptkRhtArg->tkFlags.ImmType)
                        {
                            case IMMTYPE_BOOL:  // Res = BOOL, Rht = BOOL
                                lpYYRes->tkToken.tkData.tkBoolean =
                                  (*lpPrimSpec->BisB) (lptkRhtArg->tkData.tkBoolean & BIT0,
                                                      &LclPrimSpec);
                                break;

                            case IMMTYPE_INT:   // Res = BOOL, Rht = INT
                                lpYYRes->tkToken.tkData.tkBoolean =
                                  (*lpPrimSpec->BisI) (lptkRhtArg->tkData.tkInteger,
                                                      &LclPrimSpec);
                                break;

                            case IMMTYPE_FLOAT: // Res = BOOL, Rht = FLOAT
                                lpYYRes->tkToken.tkData.tkBoolean =
                                  (*lpPrimSpec->BisF) (lptkRhtArg->tkData.tkFloat,
                                                      &LclPrimSpec);
                                break;

                            defstop
                                return NULL;
                        } // End SWITCH

                        break;

                    case ARRAY_INT:             // Res = INT
                    case ARRAY_FLOAT:           // Res = FLOAT
                        // Check for NaNs

                        // Is the arg a NaN?
                        if (IsArgNaN (aplTypeRht, &lptkRhtArg->tkData.tkLongest, 0))
                        {
                            if (!gbAllowNaN)
                                goto DOMAIN_EXIT;

////////////////////////////DbgBrk ();      // #1:  Monadic PSF {NaN}

                            // Save in the result
                            lpYYRes->tkToken.tkData.tkLongest = lptkRhtArg->tkData.tkLongest;
                            lpYYRes->tkToken.tkFlags.ImmType  = IMMTYPE_FLOAT;
                        } else
                        {
                            // Get the target function
                            lpPrimFn = TranslateTypesToMonPSFIndex (lpPrimSpec, aplTypeRes, aplTypeRht);

                            // Copy the right arg to common var
                            (*aTypeActPromote[aplTypeRht][aplTypeRht]) (&lptkRhtArg->tkData.tkLongest, 0, &atRht);

                            __try
                            {
                                __try
                                {
                                    // Call the function
                                    (*lpPrimFn) (&lpYYRes->tkToken.tkData.tkLongest, 0, &atRht, &LclPrimSpec);
                                } __finally
                                {
                                    // Free the old atRht (if any)
                                    (*aTypeFree[aplTypeRht]) (&atRht, 0);
                                } // End __try/__finally
                            } __except (EXCEPTION_CONTINUE_SEARCH) {}
                        } // End IF/ELSE

                        break;

                    defstop
                        return NULL;
                } // End SWITCH
            } __except (CheckException (GetExceptionInformation (), L"PrimFnMon_EM_YY #2"))
            {
                EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

                dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L" #2: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                // Split cases based upon the ExceptionCode
                switch (ExceptionCode)
                {
                    case EXCEPTION_DOMAIN_ERROR:
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        YYFree (lpYYRes); lpYYRes = NULL;

                        goto DOMAIN_EXIT;

                    case EXCEPTION_WS_FULL:
                        goto WSFULL_EXIT;

                    case EXCEPTION_NONCE_ERROR:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        YYFree (lpYYRes); lpYYRes = NULL;

                        goto NONCE_EXIT;

                    case EXCEPTION_RESULT_FLOAT:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        // Because the right arg is immediate, it can't be global numeric
                        Assert (!IsGlbNum (aplTypeRes));

                        if (IsSimpleNum (aplTypeRes)
                         && !IsSimpleFlt (aplTypeRes))
                        {
                            // It's now a FLOAT result
                            aplTypeRes = ARRAY_FLOAT;

                            dprintfWL0 (L"!!Restarting Exception in " APPEND_NAME L" #2: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            goto RESTART_EXCEPTION_VARIMMED;
                        } // End IF

                        // Display message for unhandled exception
                        DisplayException ();

                        break;

                    case EXCEPTION_RESULT_VFP:
                    case EXCEPTION_RESULT_HC2F:
                    case EXCEPTION_RESULT_HC4F:
                    case EXCEPTION_RESULT_HC8F:
                    case EXCEPTION_RESULT_HC2V:
                    case EXCEPTION_RESULT_HC4V:
                    case EXCEPTION_RESULT_HC8V:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        // Because the right arg is immediate, it can't be global numeric
                        Assert (!IsGlbNum (aplTypeRes));

                        if (IsSimpleNum (aplTypeRes))
                        {
                            TOKEN tkTmp;

                            // It's now a HC[248][FV] result
                            aplTypeRes = TranslateExceptionCodeToArrayType (ExceptionCode);

                            dprintfWL0 (L"!!Restarting Exception in " APPEND_NAME L" #2A: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            // Copy the immediate token to a temp
                            tkTmp = *lptkRhtArg;

                            // Promote the immediate value
                            (*aTypeTknPromote[aplTypeRht][aplTypeRes]) (&tkTmp);

                            // Note that we created a global entry due to an exception
                            bGlbEntry = TRUE;

                            // Get the global memory handle
                            hGlbRes = tkTmp.tkData.tkGlbData;

                            goto RESTART_EXCEPTION_VARARRAY;
                        } // End IF

                        // Display message for unhandled exception
                        DisplayException ();

                        break;

                    default:
                        // Display message for unhandled exception
                        DisplayException ();

                        break;
                } // End SWITCH
            } // End __try/__except

            // Restore the value of []RL from LclPrimSpec
            RestPrimSpecRL (&LclPrimSpec);

            return lpYYRes;

        case TKT_VARARRAY:
            // Get the global memory handle
            hGlbRes = lptkRhtArg->tkData.tkGlbData;

            // In order to make roll atomic, save the current []RL into LclPrimSpec
            SavePrimSpecRL (&LclPrimSpec);
RESTART_EXCEPTION_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbRes));

            // Save the old global memory handle
            hOldGlbRes = hGlbRes;

            // Handle via subroutine
            hGlbRes = PrimFnMonGlb_EM (hGlbRes,
                                       lptkFunc,
                                      &LclPrimSpec);
            // If we created a new global entry due to an exception, ...
            if (bGlbEntry)
            {
                // Free it as no one else will
                MyGlobalFree (hOldGlbRes); hOldGlbRes = NULL;

                // Clear the flag
                bGlbEntry = FALSE;
            } // End IF

            if (hGlbRes EQ NULL)
            {
                YYFree (lpYYRes); lpYYRes = NULL;

                return NULL;
            } // End IF

            // Restore the value of []RL from LclPrimSpec
            RestPrimSpecRL (&LclPrimSpec);

            // Split cases based upon the ptr type bits
            switch (GetPtrTypeDir (hGlbRes))
            {
                case PTRTYPE_STCONST:
                    // Fill in the result token
                    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                    lpYYRes->tkToken.tkFlags.ImmType   = ((LPSYMENTRY) hGlbRes)->stFlags.ImmType;
////////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
                    lpYYRes->tkToken.tkData.tkLongest  = ((LPSYMENTRY) hGlbRes)->stData.stLongest;
                    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

                    break;

                case PTRTYPE_HGLOBAL:
                    // Fill in the result token
                    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
                    lpYYRes->tkToken.tkData.tkGlbData  = hGlbRes;
                    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

                    break;

                defstop
                    break;
            } // End SWITCH

            return lpYYRes;

        defstop
            break;
    } // End SWITCH

    DbgStop ();         // We should never get here

    YYFree (lpYYRes); lpYYRes = NULL;

    return NULL;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnMon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonGlb_EM
//
//  Primitive scalar monadic function on a global memory object
//  Returning an HGLOBAL with the ptr type bits significant
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonGlb_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL PrimFnMonGlb_EM
    (HGLOBAL    hGlbRht,                    // Right arg handle
     LPTOKEN    lptkFunc,                   // Ptr to function token
     LPPRIMSPEC lpPrimSpec)                 // Ptr to local PRIMSPEC

{
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPVOID            lpMemRht,             // Ptr to right arg global memory
                      lpMemRes;             // Ptr to result    ...
    HGLOBAL           hGlbRes = NULL,       // Result global memory handle
                      hGlbSub;              // Subarray ...
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeCom,           // Common storage type between Res & Rht arg
                      aplTypeRes;           // Result    ...
    APLNELM           aplNELMRht,           // # elements in the array
                      aplNELMTmp,           // Temporary NELM
                      aplNELMRem;           // Remaining NELM
    APLRANK           aplRankRht;           // The rank of the array
    APLINT            uRes,                 // Result loop counter
                      apaOffRht,            // Right arg APA offset
                      apaMulRht;            // ...           multiplier
    APLUINT           ByteRes;              // # bytes in the result
    UBOOL             bRet = TRUE,          // TRUE iff result is valid
                      bTypeDemote = FALSE;  // TRUE iff the result is eligible for type demotion
    UINT              uBitIndex;            // Bit index when marching through Booleans
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLVFP            mpfRes = {0};         // VFP result
    ATISAT           *lpPrimFn;             // Ptr to PSDF function
    ALLTYPES          atRht = {0};          // Right arg as ALLTYPES
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
    int               iSizeofRes;           // Sizeof an item in the result

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLockVar (hGlbRht);

#define lpHeader    lpMemHdrRht
    // Get the Array Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;

    // Get the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeMon) (aplNELMRht,
                                               &aplTypeRht,
                                                lptkFunc);
    // Calculate the sizeof
    iSizeofRes = TranslateArrayTypeToSizeof (aplTypeRes);

    // In case StorageTypeMon changed the value of aplTypeRht,
    //   save it back into the array
    lpHeader->ArrType = aplTypeRht;
#undef  lpHeader

    // Check for DOMAIN ERROR
    if (IsErrorType (aplTypeRes))
        goto DOMAIN_EXIT;

    // In case the result is APA
    if (IsSimpleAPA (aplTypeRes))
    {
        __try
        {
            if (!(*lpPrimSpec->ApaResultMon_EM) (NULL,
                                                 lptkFunc,
                                                 hGlbRht,
                                                &hGlbRes,
                                                 aplRankRht,
                                                 lpPrimSpec))
                goto ERROR_EXIT;
            else
                goto NORMAL_EXIT;
        } __except (CheckException (GetExceptionInformation (), L"PrimFnMonGlb_EM"))
        {
            EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

            dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L" #3: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

            // Split cases based upon the ExceptionCode
            switch (ExceptionCode)
            {
                case EXCEPTION_DOMAIN_ERROR:
                case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                case EXCEPTION_INT_DIVIDE_BY_ZERO:
                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                    goto DOMAIN_EXIT;

                case EXCEPTION_WS_FULL:
                    goto WSFULL_EXIT;

                case EXCEPTION_NONCE_ERROR:
                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                    goto NONCE_EXIT;

                case EXCEPTION_RESULT_FLOAT:
                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                    // Because the right arg is immediate, it can't be global numeric
                    Assert (!IsGlbNum (aplTypeRes));

                    if (IsSimpleNum (aplTypeRes)
                     && !IsSimpleFlt (aplTypeRes))
                    {
                        // It's now a FLOAT result
                        aplTypeRes = ARRAY_FLOAT;

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

                        // Lock the memory to get a ptr to it
                        lpMemHdrRht = MyGlobalLockVar (hGlbRht);

                        if (hGlbRes NE NULL)
                        {
                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                            // We no longer need this storage
                            FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
                        } // End IF

                        dprintfWL0 (L"!!Restarting Exception in " APPEND_NAME L" #3: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                        goto RESTART_EXCEPTION;
                    } // End IF

                    // Display message for unhandled exception
                    DisplayException ();

                    break;

////////////////case EXCEPTION_RESULT_RAT:
////////////////case EXCEPTION_RESULT_HC2R:
////////////////case EXCEPTION_RESULT_HC4R:
////////////////case EXCEPTION_RESULT_HC8R:

                case EXCEPTION_RESULT_HC2F:
                case EXCEPTION_RESULT_HC4F:
                case EXCEPTION_RESULT_HC8F:

                case EXCEPTION_RESULT_VFP:
                case EXCEPTION_RESULT_HC2V:
                case EXCEPTION_RESULT_HC4V:
                case EXCEPTION_RESULT_HC8V:
                    DbgBrk ();                  // This can never happen as no APA result is GlbNum

                    break;

                default:
                    // Display message for unhandled exception
                    DisplayException ();

                    break;
            } // End SWITCH
        } // End __try/__except
    } // End IF
RESTART_EXCEPTION:
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
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemHdrRes);
    lpMemRht = VarArrayBaseToDim (lpMemHdrRht);

    //***************************************************************
    // Copy the dimensions from the right arg
    //   to the result's dimension
    //***************************************************************
    for (uRes = 0; uRes < (APLRANKSIGN) aplRankRht; uRes++)
        *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;

    // lpMemRes now points to the result's data
    // lpMemRht now points to the right arg's data

    // If the right arg is an APA, ...
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // In case the right arg is Boolean
    uBitIndex = 0;

    __try
    {
        // Split cases based upon the storage type of the result
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:            // Res = BOOL
                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Res = BOOL, Rht = BOOL
                        // Initialize # remaining NELM
                        aplNELMRem = aplNELMRht;

                        // Check for optimized chunking
                        if (lpPrimSpec->B64isB64 NE NULL)
                        {
                            // Calculate the # 64-bit chunks
                            aplNELMTmp  = aplNELMRem / 64;
                            aplNELMRem -= aplNELMTmp * 64;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB64) lpMemRes)++ =
                                  (*lpPrimSpec->B64isB64) (*((LPAPLB64) lpMemRht)++, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining 32-bit chunks
                            aplNELMTmp  = aplNELMRem / 32;
                            aplNELMRem -= aplNELMTmp * 32;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB32) lpMemRes)++ =
                                  (*lpPrimSpec->B32isB32) (*((LPAPLB32) lpMemRht)++, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining 16-bit chunks
                            aplNELMTmp  = aplNELMRem / 16;
                            aplNELMRem -= aplNELMTmp * 16;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB16) lpMemRes)++ =
                                  (*lpPrimSpec->B16isB16) (*((LPAPLB16) lpMemRht)++, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining  8-bit chunks
                            aplNELMTmp  = aplNELMRem /  8;
                            aplNELMRem -= aplNELMTmp *  8;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB08) lpMemRes)++ =
                                  (*lpPrimSpec->B08isB08) (*((LPAPLB08) lpMemRht)++, lpPrimSpec);
                            } // End FOR
                        } // End IF

                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRem; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLBOOL) lpMemRes) |=
                              (*lpPrimSpec->BisB) ((APLBOOL) (BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex)),
                                                   lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;              // Start over
                                ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                                ((LPAPLBOOL) lpMemRes)++;   // ...
                            } // End IF
                        } // End FOR

                        break;

                    case ARRAY_INT:     // Res = BOOL, Rht = INT
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLBOOL) lpMemRes) |=
                              (*lpPrimSpec->BisI) (*((LPAPLINT) lpMemRht)++,
                                                   lpPrimSpec) << uBitIndex;

                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;              // Start over
                                ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                            } // End IF
                        } // End FOR

                        break;

                    case ARRAY_APA:     // Res = BOOL, Rht = APA
                        // Initialize # remaining NELM
                        aplNELMRem = aplNELMRht;

                        // Check for Boolean APA and optimized chunking
#define lpAPA       ((LPAPLAPA) lpMemRht)
                        if (IsBooleanAPA (lpAPA)
                         && lpPrimSpec->B64isB64)
                        {
                            APLB64 aplB64APA;

                            if (lpAPA->Off NE 0)
                                aplB64APA = 0xFFFFFFFFFFFFFFFF;
                            else
                                aplB64APA = 0x0000000000000000;
#undef  lpAPA
                            // Calculate the # 64-bit chunks
                            aplNELMTmp  = aplNELMRem / 64;
                            aplNELMRem -= aplNELMTmp * 64;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB64) lpMemRes)++ =
                                  (*lpPrimSpec->B64isB64) ((APLB64) aplB64APA, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining 32-bit chunks
                            aplNELMTmp  = aplNELMRem / 32;
                            aplNELMRem -= aplNELMTmp * 32;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB32) lpMemRes)++ =
                                  (*lpPrimSpec->B32isB32) ((APLB32) aplB64APA, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining 16-bit chunks
                            aplNELMTmp  = aplNELMRem / 16;
                            aplNELMRem -= aplNELMTmp * 16;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB16) lpMemRes)++ =
                                  (*lpPrimSpec->B16isB16) ((APLB16) aplB64APA, lpPrimSpec);
                            } // End FOR

                            // Calculate the # remaining  8-bit chunks
                            aplNELMTmp  = aplNELMRem /  8;
                            aplNELMRem -= aplNELMTmp *  8;

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMTmp; uRes++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Save in the result
                                *((LPAPLB08) lpMemRes)++ =
                                  (*lpPrimSpec->B08isB08) ((APLB08) aplB64APA, lpPrimSpec);
                            } // End FOR
                        } // End IF

                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRem; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLBOOL) lpMemRes) |=
                              (*lpPrimSpec->BisI) (apaOffRht + apaMulRht * uRes,
                                                   lpPrimSpec) << uBitIndex;

                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;              // Start over
                                ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                            } // End IF
                        } // End FOR

                        break;

                    case ARRAY_FLOAT:   // Res = BOOL, Rht = FLOAT
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Save in the result
                            *((LPAPLBOOL) lpMemRes) |=
                              (*lpPrimSpec->BisF) (*((LPAPLFLOAT) lpMemRht)++,
                                                   lpPrimSpec) << uBitIndex;

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
                } // End FOR/SWITCH

                break;

            case ARRAY_NESTED:          // Res = NESTED
                // Loop through the right arg/result
                for (uRes = 0; bRet && uRes < (APLNELMSIGN) aplNELMRht; uRes++, ((APLNESTED *) lpMemRht)++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    switch (GetPtrTypeInd (lpMemRht))
                    {
                        case PTRTYPE_STCONST:
                        {
                            APLSTYPE   aplTypeRes2,
                                       aplTypeRht2;
                            LPSYMENTRY lpSymSrc,
                                       lpSymDst;

                            // Get the type of the SYMENTRY
                            aplTypeRht2 = TranslateImmTypeToArrayType (((LPSYMENTRY) ClrPtrTypeInd (lpMemRht))->stFlags.ImmType);

                            // Get the storage type of the result
                            aplTypeRes2 = (*lpPrimSpec->StorageTypeMon) (1,
                                                                        &aplTypeRht2,
                                                                         lptkFunc);
                            // Check for DOMAIN ERROR
                            if (IsErrorType (aplTypeRes2))
                                goto DOMAIN_EXIT;

                            // Copy the SYMENTRY as the same type as the result
                            lpSymSrc = ClrPtrTypeInd (lpMemRht);
                            lpSymDst = CopyImmSymEntry_EM (lpSymSrc,
                                                           TranslateArrayTypeToImmType (aplTypeRes2),
                                                           lptkFunc);
                            if (lpSymDst NE NULL)
                            {
                                // Split cases based upon the result storage type
                                switch (aplTypeRes2)
                                {
                                    case ARRAY_BOOL:            // Res = BOOL
                                        // Split cases based upon the right arg's storage type
                                        switch (lpSymSrc->stFlags.ImmType)
                                        {
                                            case IMMTYPE_BOOL:  // Res = BOOL, Rht = BOOL
                                                lpSymDst->stData.stBoolean =
                                                  (*lpPrimSpec->BisB) (lpSymSrc->stData.stBoolean & BIT0,
                                                                       lpPrimSpec);
                                                break;

                                            case IMMTYPE_INT:   // Res = BOOL, Rht = INT
                                                lpSymDst->stData.stBoolean =
                                                  (*lpPrimSpec->BisI) (lpSymSrc->stData.stInteger,
                                                                       lpPrimSpec);
                                                break;

                                            case IMMTYPE_FLOAT: // Res = BOOL, Rht = FLOAT
                                                lpSymDst->stData.stBoolean =
                                                  (*lpPrimSpec->BisF) (lpSymSrc->stData.stFloat,
                                                                       lpPrimSpec);
                                                break;

                                            defstop
                                                break;
                                        } // End SWITCH

                                        break;

                                    case ARRAY_INT:             // Res = INT    {floor} 1.5 (2 3)
                                    case ARRAY_FLOAT:           // Res = FLT
                                        // Check for NaNs

                                        // Is the arg a NaN?
                                        if (IsArgNaN (aplTypeRht2, &lpSymDst->stData.stLongest, 0))
                                        {
                                            if (!gbAllowNaN)
                                                goto DOMAIN_EXIT;
////////////////////////////////////////////DbgBrk ();          // #2: {floor} {NaN} (2 3)

                                            // Save in the result
////////////////////////////////////////////lpSymDst->stData.stLongest = lpSymDst->stData.stLongest;
                                            lpSymDst->stFlags.ImmType = IMMTYPE_FLOAT;
                                        } else
                                        {
                                            // Get the target function
                                            lpPrimFn = TranslateTypesToMonPSFIndex (lpPrimSpec, aplTypeRes2, aplTypeRht2);

                                            // Copy the right arg to common var
                                            (*aTypeActPromote[aplTypeRht2][aplTypeRht2]) (&lpSymDst->stData.stLongest, 0, &atRht);

                                            __try
                                            {
                                                __try
                                                {
                                                    // Call the function
                                                    (*lpPrimFn) (&lpSymDst->stData.stLongest, 0, &atRht, lpPrimSpec);
                                                } __finally
                                                {
                                                    // Free the old atRht (if any)
                                                    (*aTypeFree[aplTypeRht2]) (&atRht, 0);
                                                } // End __try/__finally
                                            } __except (EXCEPTION_CONTINUE_SEARCH) {}
                                        } // End IF/ELSE

                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Save in the result
                                *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeSym (lpSymDst);
                            } else
                                bRet = FALSE;
                            break;
                        } // End PTRTYPE_STCONST

                        case PTRTYPE_HGLOBAL:
                            // Handle via subroutine
                            hGlbSub = PrimFnMonGlb_EM (ClrPtrTypeInd (lpMemRht),
                                                       lptkFunc,
                                                       lpPrimSpec);
                            if (hGlbSub NE NULL)
                                // Save in the result
                                *((LPAPLNESTED) lpMemRes)++ = hGlbSub;
                            else
                                bRet = FALSE;
                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End FOR

                break;

            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_RAT:
            case ARRAY_VFP:
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
                // If the argument's dimension and the result's differ,
                //   promote the arg to the type of the result
                // This can happen when (say) a VFP blows up to HC2V (e.g., log -2.5v)
                if (TranslateArrayTypeToHCDim (aplTypeRht)
                  < TranslateArrayTypeToHCDim (aplTypeRes))
                    aplTypeCom = aplTypeRes;
                else
                    aplTypeCom = aplTypeRht;

                // Get the target function
                lpPrimFn = TranslateTypesToMonPSFIndex (lpPrimSpec, aplTypeRes, aplTypeCom);

                // Loop through the right arg/result
                for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Check for NaNs

                    // Is the arg a NaN?
                    if (IsArgNaN (aplTypeRht, lpMemRht, uRes))
                    {
                        // HC Dimension index (0, 1, 2, 3)
                        int iHCDimIndex = aArrayTypeToHCDimIndex[aplTypeRes];

                        if (!gbAllowNaN)
                            goto DOMAIN_EXIT;
////////////////////////DbgBrk ();      // #3:  Monadic PSF ,{NaN}
                                        //      Monadic PSF {enclose},{NaN}

                        // Do we need to promote the result type from HCxI to HCxF as in {floor},{NaN}  ?
                        if (IsSimpleInt (aToSimple[aplTypeRes]))
                            RaiseException (EXCEPTION_RESULT_FLOAT + 2 * iHCDimIndex, 0, 0, NULL);

                        // Copy the right arg to the result
                        (*aTypeActPromote[aplTypeRht][aplTypeRes]) (lpMemRht, uRes, (LPALLTYPES) ByteAddr (lpMemRes, uRes * iSizeofRes));
                    } else
                    {
                        // Copy the right arg to ALLTYPES
                        (*aTypeActPromote[aplTypeRht][aplTypeCom]) (lpMemRht, uRes, &atRht);

                        __try
                        {
                            __try
                            {
                                // Save in the result
                                (*lpPrimFn) (lpMemRes, uRes, &atRht, lpPrimSpec);
                            } __finally
                            {
                                // Free the old atRht (if any)
                                (*aTypeFree[aplTypeCom]) (&atRht, 0);
                            } // End __try/__finally
                        } __except (CheckException (GetExceptionInformation (), L"PrimFnMonGlb_EM"))
                        {
                            EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

                            dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L" #4A: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                            // Pass it on
                            RaiseException (ExceptionCode, 0, 0, NULL);
                        } // End __try/__except
                    } // End IF/ELSE
                } // End FOR

                // Mark as eligible for type demotion
                bTypeDemote = TRUE;

                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckException (GetExceptionInformation (), L"PrimFnMonGlb_EM"))
    {
        EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L" #4: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

        // Split cases based upon the ExceptionCode
        switch (ExceptionCode)
        {
            case EXCEPTION_DOMAIN_ERROR:
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                goto DOMAIN_EXIT;

            case EXCEPTION_WS_FULL:
                goto WSFULL_EXIT;

            case EXCEPTION_NONCE_ERROR:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                goto NONCE_EXIT;

            case EXCEPTION_RESULT_HC1F:
            case EXCEPTION_RESULT_HC2F:
            case EXCEPTION_RESULT_HC4F:
            case EXCEPTION_RESULT_HC8F:
            case EXCEPTION_RESULT_HC1V:
            case EXCEPTION_RESULT_HC2V:
            case EXCEPTION_RESULT_HC4V:
            case EXCEPTION_RESULT_HC8V:
                MySetExceptionCode (EXCEPTION_SUCCESS); /* Reset */

                if (IsNumeric (aplTypeRes))
                {
                    /* It's now a HCxF or HCxV result */
                    aplTypeRes = TranslateExceptionCodeToArrayType (ExceptionCode);

                    /* We no longer need this ptr */
                    MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

                    /* Lock the memory to get a ptr to it */
                    lpMemHdrRht = MyGlobalLockVar (hGlbRht);

                    /* We no longer need this ptr */
                    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                    /* We no longer need this storage */
                    FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

                    dprintfWL0 (L"!!Restarting Exception in " APPEND_NAME L" #4: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    goto RESTART_EXCEPTION;
                } /* End IF */

                /* Display message for unhandled exception */
                DisplayException ();

                break;

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except

    if (bRet)
        goto NORMAL_EXIT;
    else
        goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    bRet = FALSE;

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
    // If the result global memory handle is valid, ...
    if (hGlbRes NE NULL)
        // Make it into a ptr type
        hGlbRes = MakePtrTypeGlb (hGlbRes);

    // We no longer need this storage
    Myf_clear (&mpfRes);

    if (lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // If the result is eligible for type demotion, ...
    if (bTypeDemote
     && hGlbRes NE NULL)
        hGlbRes = TypeDemoteGlb (hGlbRes, lpPrimSpec->bMonDimDemote);

    return hGlbRes;
} // End PrimFnMonGlb_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDyd_EM_YY
//
//  Primitive scalar function for dyadic fns
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDyd_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDyd_EM_YY
    (LPTOKEN    lptkLftArg,         // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis,           // Ptr to axis token (may be NULL)
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLRANK            aplRankLft,              // Left arg rank
                       aplRankRht,              // Right ...
                       aplRankRes;              // Result   ...
    APLNELM            aplNELMLft,              // Left arg NELM
                       aplNELMRht,              // Right ...
                       aplNELMRes,              // Result   ...
                       aplNELMAxis;             // Axis     ...
    APLLONGEST         aplLongestLft,           // Left arg longest value
                       aplLongestRht;           // Right ...
    HGLOBAL            hGlbLft = NULL,          // Left arg global memory handle
                       hGlbRht = NULL,          // Right ...
                       hGlbRes = NULL,          // Result   ...
                       hGlbAxis = NULL;         // Axis     ...
    APLSTYPE           aplTypeLft,              // Left arg storage type
                       aplTypeRht,              // Right ...
                       aplTypeRes,              // Result   ...
                       aplTypeLft2,             // Left arg base storage type
                       aplTypeRht2;             // Right ...
    LPAPLUINT          lpMemAxisHead = NULL,    // Ptr to axis values, fleshed out by CheckAxis_EM
                       lpMemAxisTail = NULL;    // Ptr to grade up of AxisHead
    LPVARARRAY_HEADER  lpMemHdrLft = NULL,      // Ptr to left arg header
                       lpMemHdrRht = NULL;      // ...    right    ...
    APLINT             aplInteger;              // Temporary integer value
    UBOOL              bRet = TRUE,             // TRUE iff result is valid
                       bRealOnly = FALSE,       // TRUE iff the args must be demoted to real
                       bLftIdent,               // TRUE iff the function has a left identity element and the Axis tail is valid
                       bRhtIdent;               // ...                         right ...
    LPPRIMFN_DYD_SNvSN lpPrimFn;                // Ptr to dyadic scalar Simp/Nest vs. Simp/Nest function
    LPPL_YYSTYPE       lpYYRes = NULL;          // Ptr to the result
    LPPRIMFLAGS        lpPrimFlags;             // Ptr to function PrimFlags entry
    PRIMSPEC           LclPrimSpec;             // Writable copy of PRIMSPEC
    HGLOBAL            hGlbLft2 = NULL,         // Temp handle for type demotion
                       hGlbRht2 = NULL;         // ...

    // Save a writable copy of PRIMSPEC
    LclPrimSpec = *lpPrimSpec;

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Handle prototypes separately
    if (IsEmpty (aplNELMLft)
     || IsEmpty (aplNELMRht))
    {
        lpYYRes = PrimProtoFnScalar_EM_YY (lptkLftArg,  // Ptr to left arg token
                                           lptkFunc,    // Ptr to function token
                                           lptkRhtArg,  // Ptr to right arg token
                                           lptkAxis);   // Ptr to axis token (may be NULL)
        bRet = (lpYYRes NE NULL);

        goto NORMAL_EXIT;
    } // End IF

    // The rank of the result is the larger of the two args
    aplRankRes = max (aplRankLft, aplRankRht);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Check the axis values, fill in # elements in axis
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRes,      // All values less than this
                           FALSE,           // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // TRUE iff fractional values allowed
                           NULL,            // Return last axis value
                          &aplNELMAxis,     // Return # elements in axis vector
                          &hGlbAxis))       // Return HGLOBAL with APLINT axis values
            return NULL;

        // Lock the memory to get a ptr to it
        lpMemAxisHead = MyGlobalLockInt (hGlbAxis);

        // Get pointer to the axis tail (where the [X] values are)
        lpMemAxisTail = &lpMemAxisHead[aplRankRes - aplNELMAxis];
    } else
        // No axis is the same as all axes
        aplNELMAxis = aplRankRes;

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                               &aplTypeLft,
                                                lptkFunc,
                                                aplNELMRht,
                                               &aplTypeRht);
    if (IsRealType  (aplTypeRes))
    {
        // Calculate the left arg base storage type
        aplTypeLft2 = aToSimple[aplTypeLft];

        // Calculate the left arg base storage type
        aplTypeRht2 = aToSimple[aplTypeRht];

        // Set the result storage type to the base type
        aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                                   &aplTypeLft2,
                                                    lptkFunc,
                                                    aplNELMRht,
                                                   &aplTypeRht2);
        // Mark as requiring type demotion of one or both args
        bRealOnly = TRUE;
    } else
    if (IsNonceType (aplTypeRes))
        goto NONCE_EXIT;
    else
    if (IsErrorType (aplTypeRes))
        goto DOMAIN_EXIT;

    Assert (IsNumeric (aplTypeRes)
         || IsNested  (aplTypeRes));

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Get a ptr to the Primitive Function Flags
    lpPrimFlags = GetPrimFlagsPtr (lptkFunc);

    // Set the identity element bits
    bLftIdent = lpPrimFlags->bLftIdent
             && (lpMemAxisTail NE NULL);
    bRhtIdent = lpPrimFlags->bRhtIdent
             && (lpMemAxisTail NE NULL);

    // Check for RANK and LENGTH ERRORs
    if (!CheckRankLengthError_EM (aplRankRes,
                                  aplRankLft,
                                  aplNELMLft,
                                  lpMemHdrLft,
                                  aplRankRht,
                                  aplNELMRht,
                                  lpMemHdrRht,
                                  aplNELMAxis,
                                  lpMemAxisTail,
                                  bLftIdent,
                                  bRhtIdent,
                                  lptkFunc))
        goto ERROR_EXIT;

    // The NELM of the result is the larger of the two args
    //   unless one is empty
    if (IsEmpty (aplNELMLft) || IsEmpty (aplNELMRht))
        aplNELMRes = 0;
    else
        aplNELMRes = max (aplNELMLft, aplNELMRht);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Handle APA result separately
    if (IsSimpleAPA (aplTypeRes))
    {
        if (IsSimpleAPA (aplTypeLft))
            aplInteger = aplLongestRht;
        else
            aplInteger = aplLongestLft;
        __try
        {
            if (!(*lpPrimSpec->ApaResultDyd_EM) (lpYYRes,
                                                 lptkFunc,
                                                 hGlbLft,
                                                 hGlbRht,
                                                &hGlbRes,
                                                 aplRankLft,
                                                 aplRankRht,
                                                 aplNELMLft,
                                                 aplNELMRht,
                                                 aplInteger,
                                                &LclPrimSpec))
                goto ERROR_EXIT;
            else
                goto NORMAL_EXIT;
        } __except (CheckException (GetExceptionInformation (), L"PrimFnDyd_EM_YY #1"))
        {
            EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

            dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L" #11: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

            // Split cases based upon the ExceptionCode
            switch (ExceptionCode)
            {
                case EXCEPTION_RESULT_FLOAT:
                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                    // It's now a FLOAT result
                    aplTypeRes = ARRAY_FLOAT;

                    if (hGlbRes NE NULL)
                    {
                        // We need to start over with the result
                        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
                    } // End IF

                    dprintfWL0 (L"!!Restarting Exception in " APPEND_NAME L" #11: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    break;

                defstop
                    RaiseException (ExceptionCode, 0, 0, NULL);

                    break;
            } // End SWITCH
        } // End __try/__except
    } // End IF

    // If one or both args must be demoted, ...
    if (bRealOnly)
    {
        // If the left arg is HC, ...
        if (IsHCAny (aplTypeLft))
        {
            // Allocate new and Demote the left arg
            hGlbLft2 = AllocateDemote (aplTypeLft2,         // Base storage type
                                       hGlbLft,             // Left arg global memory handle (may be NULL)
                                       NULL,                // Ptr to ALLTYPES values (may be NULL)
                                       aplTypeLft,          // ... storage type
                                       aplNELMLft,          // ... NELM
                                       aplRankLft,          // ... rank
                                      &bRet);               // TRUE iff the result is not a WS FULL
            // We no longer need this ptr
            MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;

            // Check for error
            if (!bRet)
                goto LEFT_DOMAIN_EXIT;
            if (hGlbLft2 EQ NULL)
                goto WSFULL_EXIT;
            // Save as the new global handle and storage type
            hGlbLft    = hGlbLft2;
            aplTypeLft = aplTypeLft2;

            // Lock the memory to get a ptr to it
            lpMemHdrLft = MyGlobalLockVar (hGlbLft);
        } // End IF

        // If the right arg is HC, ...
        if (IsHCAny (aplTypeRht))
        {
            // Allocate new and Demote the right arg
            hGlbRht2 = AllocateDemote (aplTypeRht2,         // Base storage type
                                       hGlbRht,             // Right arg global memory handle (may be NULL
                                       NULL,                // Ptr to ALLTYPES values (may be NULL)
                                       aplTypeRht,          // ... storage type
                                       aplNELMRht,          // ... NELM
                                       aplRankRht,          // ... rank
                                      &bRet);               // TRUE iff the result is not a WS FULL
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

            // Check for error
            if (!bRet)
                goto RIGHT_DOMAIN_EXIT;
            if (hGlbRht2 EQ NULL)
                goto WSFULL_EXIT;
            // Save as the new global handle and storage type
            hGlbRht    = hGlbRht2;
            aplTypeRht = aplTypeRht2;

            // Lock the memory to get a ptr to it
            lpMemHdrRht = MyGlobalLockVar (hGlbRht);
        } // End IF
    } // End IF

    // Allocate space for result
    if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                    &hGlbRes,
                                     lpMemHdrLft,
                                     lpMemHdrRht,
                                     aplRankLft,
                                     aplRankRht,
                                    &aplRankRes,
                                     aplTypeRes,
                                     bLftIdent,
                                     bRhtIdent,
                                     aplNELMLft,
                                     aplNELMRht,
                                     aplNELMRes))
        goto ERROR_EXIT;

    // Four cases:
    //   Result     Left       Right
    //   ----------------------------
    //   Simple     Simple     Simple
    //   Nested     Simple     Nested
    //   Nested     Nested     Simple
    //   Nested     Nested     Nested

    // Split cases based upon the combined left vs. right types
    if ( IsNested (aplTypeLft)
     &&  IsNested (aplTypeRht))
        // Left arg is NESTED/HETERO, right arg is NESTED
        lpPrimFn = &PrimFnDydNestNest_EM;
    else
    if (!IsNested (aplTypeLft)
     &&  IsNested (aplTypeRht))
        // Left arg is SIMPLE, right arg is NESTED
        lpPrimFn = &PrimFnDydSimpNest_EM;
    else
    if ( IsNested (aplTypeLft)
     && !IsNested (aplTypeRht))
        // Left arg is NESTED, right arg is SIMPLE
        lpPrimFn = &PrimFnDydNestSimp_EM;
    else
    if (!IsNested (aplTypeLft)
     && !IsNested (aplTypeRht))
        // Left arg is SIMPLE, right arg is SIMPLE
        lpPrimFn = &PrimFnDydSimpSimp_EM;
    else
    {
        DbgStop ();     // We should never get here

        return NULL;
    } // Endf IF/ELSE/...

    // Call the appropriate function
    if (!(*lpPrimFn) (lpYYRes,
                      lptkLftArg,
                      lptkFunc,
                      lptkRhtArg,
                      hGlbLft,
                      hGlbRht,
                     &hGlbRes,
                      lpMemHdrLft,      // Ptr to left arg header
                      lpMemHdrRht,      // ...    right ...
                      lpMemAxisHead,    // Ptr to axis values, fleshed out by CheckAxis_EM
                      lpMemAxisTail,    // Ptr to grade up of AxisHead
                      aplRankLft,
                      aplRankRht,
                      aplRankRes,
                      aplTypeLft,
                      aplTypeRht,
                      aplTypeRes,
                      aplNELMLft,
                      aplNELMRht,
                      aplNELMRes,
                      aplNELMAxis,
                      bLftIdent,
                      bRhtIdent,
                     &LclPrimSpec))
        goto ERROR_EXIT;
    else
        goto NORMAL_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
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
    bRet = FALSE;

    if (hGlbRes NE NULL)
    {
        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    // If we allocated to demote data
    if (hGlbLft2 NE NULL)
    {
        // We no longer need this resource
        FreeResultGlobalIncompleteVar (hGlbLft2); hGlbLft2 = NULL;
    } // End IF

    // If we allocated to demote data
    if (hGlbRht2 NE NULL)
    {
        // We no longer need this resource
        FreeResultGlobalIncompleteVar (hGlbRht2); hGlbRht2 = NULL;
    } // End IF

    // If the result is valid, ...
    if (lpYYRes NE NULL)
        // Fill in the character index
        lpYYRes->tkToken.tkCharIndex = lptkFunc->tkCharIndex;

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

    if (hGlbAxis NE NULL && lpMemAxisHead NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbAxis); lpMemAxisHead = lpMemAxisTail = NULL;
    } // End IF

    // If we failed and there's a result, free it
    if (!bRet && lpYYRes NE NULL)
    {
        YYFree (lpYYRes); lpYYRes = NULL;
    } // End IF/ELSE

    return lpYYRes;
} // End PrimFnDyd_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSimpNest_EM
//
//  Dyadic primitive scalar function, left simple or global numeric, right nested/hetero
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSimpNest_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydSimpNest_EM
    (LPPL_YYSTYPE      lpYYRes,         // Ptr to the result

     LPTOKEN           lptkLftArg,      // Ptr to left arg token
     LPTOKEN           lptkFunc,        // ...    function ...
     LPTOKEN           lptkRhtArg,      // ...    right arg ...

     HGLOBAL           hGlbLft,         // Left arg handle
     HGLOBAL           hGlbRht,         // Right ...
     HGLOBAL          *lphGlbRes,       // Ptr to result handle

     LPVARARRAY_HEADER lpMemHdrLft,     // Ptr to left arg header
     LPVARARRAY_HEADER lpMemHdrRht,     // ...    right    ...

     LPAPLUINT         lpMemAxisHead,   // Ptr to axis values, fleshed out by CheckAxis_EM
     LPAPLUINT         lpMemAxisTail,   // Ptr to grade up of AxisHead
                                        //
     APLRANK           aplRankLft,      // Left arg rank
     APLRANK           aplRankRht,      // Right ...
     APLRANK           aplRankRes,      // Result ...
                                        //
     APLSTYPE          aplTypeLft,      // Left arg type
     APLSTYPE          aplTypeRht,      // Right ...
     APLSTYPE          aplTypeUnused,   // Result ... (always ARRAY_NESTED)

     APLNELM           aplNELMLft,      // Left arg NELM
     APLNELM           aplNELMRht,      // Right ...
     APLNELM           aplNELMRes,      // Result ...
     APLNELM           aplNELMAxis,     // Axis ...

     UBOOL             bLftIdent,       // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL             bRhtIdent,       // ...                         right ...

     LPPRIMSPEC        lpPrimSpec)      // Ptr to local PRIMSPEC

{
    APLSTYPE          aplTypeCom;           // Common storage type between Lft & Rht args
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPVOID            lpMemLft,             // Ptr to left arg global memory
                      lpMemRht,             // ...    right    ...
                      lpMemRes;             // ...    result   ...
    UBOOL             bRet = TRUE;          // TRUE iff result is valid
    APLINT            uRes;                 // Loop counter
    APLINT            apaOffLft,
                      apaMulLft,
                      iDim;
    APLUINT           ByteAlloc;
    HGLOBAL           hGlbWVec = NULL,
                      hGlbOdo = NULL,
                      lpSymGlbLft,
                      hGlbSub;
    LPAPLUINT         lpMemWVec = NULL,
                      lpMemDimLft,
                      lpMemDimRht,
                      lpMemDimRes,
                      lpMemOdo = NULL;
    APLLONGEST        aplLongestLft;        // Immediate value
    ALLTYPES          atLft = {0},          // Left arg as ALLTYPES
                      atRht = {0};          // Right ...
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    Assert (IsNested (aplTypeUnused));

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // If the left arg is immediate, get the one and only value
    if (lpMemHdrLft EQ NULL)
    {
        // Point to the data
        lpSymGlbLft =
        lpMemLft    = GetPtrTknLongest (lptkLftArg);
    } else
    {
        // Skip over the header to the dimensions
        lpMemDimLft = VarArrayBaseToDim (lpMemHdrLft);

        // Skip over the header and dimensions to the data
        lpSymGlbLft =
        lpMemLft    = VarArrayDataFmBase (lpMemHdrLft);
    } // End IF/ELSE

    // In case of immediate, get the value
    aplLongestLft = *(LPAPLLONGEST) lpMemLft;

    // If the left arg is APA, ...
    if (IsSimpleAPA (aplTypeLft))
    {
#define lpAPA       ((LPAPLAPA) lpMemLft)
        // Get the APA parameters
        apaOffLft = lpAPA->Off;
        apaMulLft = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Handle axis if present
    if (aplNELMAxis NE aplRankRes)
    {
        // Calculate space needed for the weighting vector
        ByteAlloc = aplRankRes * sizeof (APLUINT);

        // Check for overflow
        if (ByteAlloc NE (APLU3264) ByteAlloc)
            goto WSFULL_EXIT;

        //***************************************************************
        // Allocate space for the weighting vector which is
        //   {times}{backscan}1{drop}({rho}Z),1
        //***************************************************************
        hGlbWVec = DbgGlobalAlloc (GHND, (APLU3264) ByteAlloc);
        if (hGlbWVec EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemWVec = MyGlobalLock000 (hGlbWVec);

        // Loop through the dimensions of the result in reverse
        //   order {backscan} and compute the cumulative product
        //   (weighting vector).
        // Note we use a signed index variable because we're
        //   walking backwards and the test against zero must be
        //   made as a signed variable.
        for (uRes = 1, iDim = aplRankRes - 1; iDim >= 0; iDim--)
        {
            lpMemWVec[iDim] = uRes;
            uRes *= lpMemDimRes[iDim];
        } // End FOR

        // Calculate space needed for the odometer array
        ByteAlloc = aplRankRes * sizeof (APLUINT);

        // Check for overflow
        if (ByteAlloc NE (APLU3264) ByteAlloc)
            goto WSFULL_EXIT;

        //***************************************************************
        // Allocate space for the odometer array, one value per dimension
        //   in the right arg, with values initially all zero (thanks to GHND).
        //***************************************************************
        hGlbOdo = DbgGlobalAlloc (GHND, (APLU3264) ByteAlloc);
        if (hGlbOdo EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemOdo = MyGlobalLock000 (hGlbOdo);
    } // End IF

    // Loop through the result
    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
    {
        APLINT   uLft, uRht, uArg;
        APLSTYPE aplTypeHetRht;
        UBOOL    bNaNLft,                   // TRUE iff the left arg is a NaN
                 bNaNRht;                   // ...          right ...

        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // If the left arg is not immediate, get the next value
        if (lpMemHdrLft NE NULL)
        {
            if (aplNELMAxis NE aplRankRes)
            {
                // Loop through the odometer values accumulating
                //   the weighted sum
                for (uArg = 0, uRht = aplRankRes - aplNELMAxis; uRht < (APLRANKSIGN) aplRankRes; uRht++)
                    uArg += lpMemOdo[lpMemAxisHead[uRht]] * lpMemWVec[uRht];

                // Increment the odometer in lpMemOdo subject to
                //   the values in lpMemDimRes
                IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

                // Use the just computed index for the argument
                //   with the smaller rank
                if (aplRankLft < aplRankRht)
                {
                    uLft = uArg * !IsScalar (aplRankLft);
                    uRht = uRes;
                } else
                {
                    uRht = uArg * !IsScalar (aplRankRht);
                    uLft = uRes;
                } // End IF/ELSE
            } else
            {
                uLft = uRes % aplNELMLft;
                uRht = uRes % aplNELMRht;
            } // End IF/ELSE
        } else
        {
            uLft = 0;
            uRht = uRes;
        } // End IF/ELSE

        // Get the right arg element
        hGlbSub = ((LPAPLNESTED) lpMemRht)[uRht];

        // Split cases based upon the ptr type of the nested right arg
        switch (GetPtrTypeDir (hGlbSub))
        {
            case PTRTYPE_STCONST:
                aplTypeHetRht = TranslateImmTypeToArrayType (((LPSYMENTRY) hGlbSub)->stFlags.ImmType);

                // Get the common storage type between the left & right args
                aplTypeCom = aTypePromote[aplTypeLft][aplTypeHetRht];

                // Promote the left arg to the common type
                (*aTypeActPromote[aplTypeLft][aplTypeCom]) (lpMemLft, uLft, &atLft);

                // Promote the right arg to the common type
                (*aTypeActPromote[aplTypeHetRht][aplTypeCom]) (&((LPSYMENTRY) hGlbSub)->stData.stLongest, 0, &atRht);

                // Check for NaNs

                // Are the args a NaN?
                bNaNLft = IsArgNaN (aplTypeCom, &atLft, 0);
                bNaNRht = IsArgNaN (aplTypeCom, &atRht, 0);

                // If either arg is a NaN, ...
                if (bNaNLft || bNaNRht)
                {
                    if (gbAllowNaN)         // 1x + {NaN} (3 4)
                    {
                        if (bNaNLft)
                            hGlbSub =
                              MakeGlbEntry_EM (aplTypeCom,              // Entry type
                                              &atLft,                   // Ptr to the value
                                               TRUE,                    // TRUE iff we should initialize the target first
                                               lptkFunc);               // Ptr to function token
                        else
                        {
                            // If the right arg is a global numeric, ...
                            if (IsGlbNum (aplTypeCom))
                                hGlbSub =
                                  MakeGlbEntry_EM (aplTypeCom,              // Entry type
                                                  &atRht,                   // Ptr to the value
                                                   TRUE,                    // TRUE iff we should initialize the target first
                                                   lptkFunc);               // Ptr to function token
                            else
                                hGlbSub =
                                  MakeSymEntry_EM (TranslateArrayTypeToImmType (aplTypeCom),    // Immediate type
                                                  &atRht.aplLongest,                            // Ptr to immediate value
                                                   lptkFunc);                                   // Ptr to function token
                        } // End IF/ELSE

                        // Free the old atLft & atRht (if any)
                        (*aTypeFree[aplTypeCom]) (&atLft, 0);
                        (*aTypeFree[aplTypeCom]) (&atRht, 0);

                        // Zero the memory in case we use it again
                        //   because aplTypeCom changes when looping
                        //   through a HETERO.
                        ZeroMemory (&atLft, sizeof (atLft));
                        ZeroMemory (&atRht, sizeof (atRht));
                    } else
                    {
                        // Free the old atLft & atRht (if any)
                        (*aTypeFree[aplTypeCom]) (&atLft, 0);
                        (*aTypeFree[aplTypeCom]) (&atRht, 0);

                        // Zero the memory in case we use it again
                        //   because aplTypeCom changes when looping
                        //   through a HETERO.
                        ZeroMemory (&atLft, sizeof (atLft));
                        ZeroMemory (&atRht, sizeof (atRht));

                        if (bNaNLft)
                            goto LEFT_DOMAIN_EXIT;
                        else
                            goto RIGHT_DOMAIN_EXIT;
                    } // End IF/ELSE
                } else
                {
                    __try
                    {
                        __try
                        {
                            hGlbSub = PrimFnDydSiScSiSc_EM (lptkFunc,
                                                           *lphGlbRes,
                                                            aplTypeCom,
                                                           &atLft,
                                                            aplTypeCom,
                                                           &atRht,
                                                            lpPrimSpec);
                        } __finally
                        {
                            // Free the old atLft & atRht (if any)
                            (*aTypeFree[aplTypeCom]) (&atLft, 0);
                            (*aTypeFree[aplTypeCom]) (&atRht, 0);

                            // Zero the memory in case we use it again
                            //   because aplTypeCom changes when looping
                            //   through a HETERO.
                            ZeroMemory (&atLft, sizeof (atLft));
                            ZeroMemory (&atRht, sizeof (atRht));
                        } // End __try/__finally
                    } __except (EXCEPTION_CONTINUE_SEARCH) {}
                } // End IF/ELSE

                if (hGlbSub EQ NULL)
                    goto ERROR_EXIT;
                else
                // If the result is not already filled in, ...
                if (((LPAPLNESTED) lpMemRes)[0] EQ NULL)
                    // Save in the result
                    ((LPAPLNESTED) lpMemRes)[0] = hGlbSub;
                // Skip to the next result
                ((LPAPLNESTED) lpMemRes)++;

                break;

            case PTRTYPE_HGLOBAL:
                // Copy the left arg to an ALLTYPES
                (*aTypeActPromote[aplTypeLft][aplTypeLft]) (lpMemLft, uLft, &atLft);

                // 2 4-({enclose}0 1)(0 1 2)
                hGlbSub = PrimFnDydSiScNest_EM (lptkFunc,
                                                aplTypeLft,
                                               &atLft,
                                                hGlbSub,
                                                bLftIdent,
                                                bRhtIdent,
                                                lpPrimSpec);
                // Free the old atLft (if any)
                (*aTypeFree[aplTypeLft]) (&atLft, 0);

                if (hGlbSub EQ NULL)
                    goto ERROR_EXIT;
                else
                    // Save in the result
                    *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeGlb (hGlbSub);
                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

ERROR_EXIT:
    bRet = FALSE;

    if (*lphGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (*lphGlbRes); *lphGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (*lphGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbWVec, lpMemWVec);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbOdo, lpMemOdo);

    return bRet;
} // End PrimFnDydSimpNest_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydNestSimp_EM
//
//  Dyadic primitive scalar function, left nested/hetero, right simple or global numeric
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydNestSimp_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydNestSimp_EM
    (LPPL_YYSTYPE  lpYYRes,             // Ptr to the result

     LPTOKEN           lptkLftArg,      // Ptr to left arg token
     LPTOKEN           lptkFunc,        // ...    function ...
     LPTOKEN           lptkRhtArg,      // ...    right arg ...

     HGLOBAL           hGlbLft,         // Left arg handle
     HGLOBAL           hGlbRht,         // Right ...
     HGLOBAL          *lphGlbRes,       // Ptr to result handle

     LPVARARRAY_HEADER lpMemHdrLft,     // Ptr to left arg header
     LPVARARRAY_HEADER lpMemHdrRht,     // Ptr to right ...

     LPAPLUINT         lpMemAxisHead,   // Ptr to axis values, fleshed out by CheckAxis_EM
     LPAPLUINT         lpMemAxisTail,   // Ptr to grade up of AxisHead

     APLRANK           aplRankLft,      // Left arg rank
     APLRANK           aplRankRht,      // Right ...
     APLRANK           aplRankRes,      // Result ...

     APLSTYPE          aplTypeLft,      // Left arg type
     APLSTYPE          aplTypeRht,      // Right ...
     APLSTYPE          aplTypeUnused,   // Result ... (always ARRAY_NESTED)

     APLNELM           aplNELMLft,      // Left arg NELM
     APLNELM           aplNELMRht,      // Right ...
     APLNELM           aplNELMRes,      // Result ...
     APLNELM           aplNELMAxis,     // Axis ...

     UBOOL             bLftIdent,       // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL             bRhtIdent,       // ...                         right ...

     LPPRIMSPEC        lpPrimSpec)      // Ptr to local PRIMSPEC

{
    APLSTYPE          aplTypeCom;           // Common storage type between Lft & Rht args
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPVOID            lpMemLft,             // Ptr to left arg global memory
                      lpMemRht,             // ...    right    ...
                      lpMemRes;             // ...    result   ...
    UBOOL             bRet = TRUE;          // TRUE iff result is valid
    APLINT            uRes;                 // Loop counter
    APLINT            apaOffRht,
                      apaMulRht,
                      iDim;
    APLUINT           ByteAlloc;
    HGLOBAL           hGlbWVec = NULL,
                      hGlbOdo = NULL,
                      lpSymGlbRht,
                      hGlbSub;
    LPAPLUINT         lpMemWVec = NULL,
                      lpMemDimLft,
                      lpMemDimRht,
                      lpMemDimRes,
                      lpMemOdo = NULL;
    APLLONGEST        aplLongestRht;        // Right arg if immediate
    ALLTYPES          atLft = {0},          // Left arg as ALLTYPES
                      atRht = {0};          // Right ...
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    Assert (IsNested (aplTypeUnused));

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // If the right arg is immediate, get the one and only value
    if (lpMemHdrRht EQ NULL)
    {
        // Point to the data
        lpSymGlbRht =
        lpMemRht    = GetPtrTknLongest (lptkRhtArg);
    } else
    {
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

        // Skip over the header and dimensions to the data
        lpSymGlbRht =
        lpMemRht    = VarArrayDataFmBase (lpMemHdrRht);
    } // End IF/ELSE

    // In case of immediate, get the value
    aplLongestRht = *(LPAPLLONGEST) lpMemRht;

    // If the right arg is APA, ...
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

    // Skip over the header to the dimensions
    lpMemDimLft = VarArrayBaseToDim (lpMemHdrLft);
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // Skip over the header and dimensions to the data
    lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Handle axis if present
    if (aplNELMAxis NE aplRankRes)
    {
        // Calculate space needed for the weighting vector
        ByteAlloc = aplRankRes * sizeof (APLUINT);

        // Check for overflow
        if (ByteAlloc NE (APLU3264) ByteAlloc)
            goto WSFULL_EXIT;

        //***************************************************************
        // Allocate space for the weighting vector which is
        //   {times}{backscan}1{drop}({rho}Z),1
        //***************************************************************
        hGlbWVec = DbgGlobalAlloc (GHND, (APLU3264) ByteAlloc);
        if (hGlbWVec EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemWVec = MyGlobalLock000 (hGlbWVec);

        // Loop through the dimensions of the result in reverse
        //   order {backscan} and compute the cumulative product
        //   (weighting vector).
        // Note we use a signed index variable because we're
        //   walking backwards and the test against zero must be
        //   made as a signed variable.
        for (uRes = 1, iDim = aplRankRes - 1; iDim >= 0; iDim--)
        {
            lpMemWVec[iDim] = uRes;
            uRes *= lpMemDimRes[iDim];
        } // End FOR

        // Calculate space needed for the odometer array
        ByteAlloc = aplRankRes * sizeof (APLUINT);

        // Check for overflow
        if (ByteAlloc NE (APLU3264) ByteAlloc)
            goto WSFULL_EXIT;

        //***************************************************************
        // Allocate space for the odometer array, one value per dimension
        //   in the right arg, with values initially all zero (thanks to GHND).
        //***************************************************************
        hGlbOdo = DbgGlobalAlloc (GHND, (APLU3264) ByteAlloc);
        if (hGlbOdo EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemOdo = MyGlobalLock000 (hGlbOdo);
    } // End IF

    // Loop through the result
    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
    {
        APLINT   uLft, uRht, uArg;
        APLSTYPE aplTypeHetLft;
        UBOOL    bNaNLft,                   // TRUE iff the left arg is a NaN
                 bNaNRht;                   // ...          right ...

        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // If the right arg is not immediate, get the next value
        if (lpMemHdrRht NE NULL)
        {
            if (aplNELMAxis NE aplRankRes)
            {
                // Loop through the odometer values accumulating
                //   the weighted sum
                for (uArg = 0, uLft = aplRankRes - aplNELMAxis; uLft < (APLRANKSIGN) aplRankRes; uLft++)
                    uArg += lpMemOdo[lpMemAxisHead[uLft]] * lpMemWVec[uLft];

                // Increment the odometer in lpMemOdo subject to
                //   the values in lpMemDimRes
                IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

                // Use the just computed index for the argument
                //   with the smaller rank
                if (aplRankLft < aplRankRht)
                {
                    uLft = uArg * !IsScalar (aplRankLft);
                    uRht = uRes;
                } else
                {
                    uRht = uArg * !IsScalar (aplRankRht);
                    uLft = uRes;
                } // End IF/ELSE
            } else
            {
                uLft = uRes % aplNELMLft;
                uRht = uRes % aplNELMRht;
            } // End IF/ELSE
        } else
        {
            uLft = uRes;
            uRht = 0;
        } // End IF/ELSE

        // Get the left arg element
        hGlbSub = ((LPAPLNESTED) lpMemLft)[uLft];

        // Split cases based upon the ptr type of the nested left arg
        switch (GetPtrTypeDir (hGlbSub))
        {
            case PTRTYPE_STCONST:
                aplTypeHetLft = TranslateImmTypeToArrayType (((LPSYMENTRY) hGlbSub)->stFlags.ImmType);

                // Get the common storage type between the left & right args
                aplTypeCom = aTypePromote[aplTypeHetLft][aplTypeRht];

                // Promote the right arg to the common type
                (*aTypeActPromote[aplTypeRht][aplTypeCom]) (lpMemRht, uRht, &atRht);

                // Promote the left arg to the common type
                (*aTypeActPromote[aplTypeHetLft][aplTypeCom]) (&((LPSYMENTRY) hGlbSub)->stData.stLongest, 0, &atLft);

                // Check for NaNs

                // Are the args a NaN?
                bNaNLft = IsArgNaN (aplTypeHetLft, &((LPSYMENTRY) hGlbSub)->stData.stLongest, 0);
                bNaNRht = IsArgNaN (aplTypeRht   ,  lpMemRht                                , uRht);

                // If either arg is a NaN, ...
                if (bNaNLft || bNaNRht)
                {
                    if (gbAllowNaN)         // {NaN} (3 4) + 1x
                    {
                        if (bNaNLft)
                        {
                            // If the left arg is a global numeric, ...
                            if (IsGlbNum (aplTypeCom))
                                hGlbSub =
                                  MakeGlbEntry_EM (aplTypeCom,              // Entry type
                                                  &atLft,                   // Ptr to the value
                                                   TRUE,                    // TRUE iff we should initialize the target first
                                                   lptkFunc);               // Ptr to function token
                            else
                            hGlbSub =
                              MakeSymEntry_EM (((LPSYMENTRY) hGlbSub)->stFlags.ImmType,     // Immediate type
                                              &((LPSYMENTRY) hGlbSub)->stData.stLongest,    // Ptr to immediate value
                                               lptkFunc);                                   // Ptr to function token
                        } else
                            hGlbSub =
                              MakeGlbEntry_EM (aplTypeCom,              // Entry type
                                              &atRht,                   // Ptr to the value
                                               TRUE,                    // TRUE iff we should initialize the target first
                                               lptkFunc);               // Ptr to function token
                        // Free the old atLft & atRht (if any)
                        (*aTypeFree[aplTypeCom]) (&atLft, 0);
                        (*aTypeFree[aplTypeCom]) (&atRht, 0);

                        // Zero the memory in case we use it again
                        //   because aplTypeCom changes when looping
                        //   through a HETERO.
                        ZeroMemory (&atLft, sizeof (atLft));
                        ZeroMemory (&atRht, sizeof (atRht));
                    } else
                    {
                        // Free the old atLft & atRht (if any)
                        (*aTypeFree[aplTypeCom]) (&atLft, 0);
                        (*aTypeFree[aplTypeCom]) (&atRht, 0);

                        // Zero the memory in case we use it again
                        //   because aplTypeCom changes when looping
                        //   through a HETERO.
                        ZeroMemory (&atLft, sizeof (atLft));
                        ZeroMemory (&atRht, sizeof (atRht));

                        if (bNaNLft)
                            goto LEFT_DOMAIN_EXIT;
                        else
                            goto RIGHT_DOMAIN_EXIT;
                    } // End IF/ELSE
                } else
                {
                    __try
                    {
                        __try
                        {
                            hGlbSub = PrimFnDydSiScSiSc_EM (lptkFunc,
                                                           *lphGlbRes,
                                                            aplTypeCom,
                                                           &atLft,
                                                            aplTypeCom,
                                                           &atRht,
                                                            lpPrimSpec);
                        } __finally
                        {
                            // Free the old atLft & atRht (if any)
                            (*aTypeFree[aplTypeCom]) (&atLft, 0);
                            (*aTypeFree[aplTypeCom]) (&atRht, 0);
                        } // End __try/__finally
                    } __except (EXCEPTION_CONTINUE_SEARCH) {}
                } // End IF/ELSE

                if (hGlbSub EQ NULL)
                    goto ERROR_EXIT;
                else
                // If the result is not already filled in, ...
                if (((LPAPLNESTED) lpMemRes)[0] EQ NULL)
                    // Save in the result
                    ((LPAPLNESTED) lpMemRes)[0] = hGlbSub;
                // Skip to the next result
                ((LPAPLNESTED) lpMemRes)++;

                break;

            case PTRTYPE_HGLOBAL:
                // Copy the right arg to an ALLTYPES
                (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, uRht, &atRht);

                __try
                {
                    __try
                    {
                        // ({enclose}0 1)(0 1 2)-2 4
                        hGlbSub = PrimFnDydNestSiSc_EM (lptkFunc,
                                                        aplTypeRht,
                                                       &atRht,
                                                        hGlbSub,
                                                        bLftIdent,
                                                        bRhtIdent,
                                                        lpPrimSpec);
                    } __finally
                    {
                        // Free the old atRht (if any)
                        (*aTypeFree[aplTypeRht]) (&atRht, 0);
                    } // End __try/__finally
                } __except (EXCEPTION_CONTINUE_SEARCH) {}

                if (hGlbSub EQ NULL)
                    goto ERROR_EXIT;
                else
                    // Save in the result
                    *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeGlb (hGlbSub);
                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

ERROR_EXIT:
    bRet = FALSE;

    if (*lphGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (*lphGlbRes); *lphGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (*lphGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbWVec, lpMemWVec);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbOdo, lpMemOdo);

    return bRet;
} // End PrimFnDydNestSimp_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydNestSiSc_EM
//
//  Subroutine to PrimFnDydNestSimp_EM to handle right arg simple scalars or
//    global numeric scalars, left arg nested/hetero/global numeric
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydNestSiSc_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL PrimFnDydNestSiSc_EM
    (LPTOKEN    lptkFunc,           // Ptr to function token
     APLSTYPE   aplTypeRht,         // Right arg type
     LPALLTYPES lpatRht,            // ...       immediate value
     HGLOBAL    hGlbLft,            // Left arg nested value
     UBOOL      bLftIdent,          // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL      bRhtIdent,          // ...                         right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    UBOOL             bRet      = TRUE,
                      bNaNLft,                  // TRUE iff the left arg is a NaN
                      bNaNRht,                  // ...          right ...
                      bRealOnly = FALSE;        // TRUE iff the args must be demoted to real
    HGLOBAL           hGlbRes = NULL,
                      hGlbSub,
                      hGlbLft2 = NULL;          // Temp handle for type demotion
    LPVARARRAY_HEADER lpMemHdrRes = NULL,
                      lpMemHdrLft = NULL;
    LPVOID            lpMemLft,
                      lpMemRes;
    APLSTYPE          aplTypeLft,
                      aplTypeRes,
                      aplTypeCom,
                      aplTypeLft2,              // Left arg base storage type
                      aplTypeRht2;              // Right ...
    APLNELM           aplNELMLft,
                      aplNELMRht = 1,
                      aplNELMRes;
    APLRANK           aplRankLft,
                      aplRankRht = 0,
                      aplRankRes;
    APLINT            uRes,
                      apaOffLft,
                      apaMulLft;
    UINT              uBitIndex = 0;
    ATISATVAT        *lpPrimFn;
    ALLTYPES          atLft = {0},
                      atRht = {0},
                      atRht2 = {0};
    LPALLTYPES        lpatRht2 = NULL;
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
    int               iSizeofRes;           // Sizeof an item in the result

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // The left arg data is a valid HGLOBAL array
    Assert (IsGlbTypeVarDir_PTB (hGlbLft));

    // Lock the memory to get a ptr to it
    lpMemHdrLft = MyGlobalLockVar (hGlbLft);

#define lpHeader    lpMemHdrLft
    // Get the Array Type, NELM, and Rank
    aplTypeLft = lpHeader->ArrType;
    aplNELMLft = lpHeader->NELM;
    aplRankLft = lpHeader->Rank;
#undef  lpHeader

    // The left arg is a ptr array (nested or Hetero)
    //   or a simple global numeric
    Assert (IsPtrArray     (aplTypeLft)
         || IsSimpleGlbNum (aplTypeLft));

    // The NELM of the result is NELM of the non-scalar
    aplNELMRes = aplNELMLft;

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                               &aplTypeLft,
                                                lptkFunc,
                                                aplNELMRht,
                                               &aplTypeRht);
    if (IsRealType  (aplTypeRes))
    {
        // Calculate the left arg base storage type
        aplTypeLft2 = aToSimple[aplTypeLft];

        // Calculate the left arg base storage type
        aplTypeRht2 = aToSimple[aplTypeRht];

        // Set the result storage type to the base type
        aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                                   &aplTypeLft2,
                                                    lptkFunc,
                                                    aplNELMRht,
                                                   &aplTypeRht2);
        // Mark as requiring type demotion of one or both args
        bRealOnly = TRUE;
    } else
    if (IsNonceType (aplTypeRes))
        goto NONCE_EXIT;
    else
    if (IsErrorType (aplTypeRes))
        goto DOMAIN_EXIT;

    // Calculate the sizeof
    iSizeofRes = TranslateArrayTypeToSizeof (aplTypeRes);

    // Special case APA result
    if (IsSimpleAPA (aplTypeRes))
    {
        if (!(*lpPrimSpec->ApaResultDyd_EM) (NULL,
                                             lptkFunc,
                                             hGlbLft,
                                             NULL,
                                            &hGlbRes,
                                             aplRankLft,
                                             aplRankRht,
                                             aplNELMLft,
                                             aplNELMRht,
                                             lpatRht->aplInteger,
                                             lpPrimSpec))
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } // End IF
RESTART_EXCEPTION:
    Assert (IsSimpleGlbNum (aplTypeRes)
         || IsNested (aplTypeRes));

    // If one or both args must be demoted, ...
    if (bRealOnly)
    {
        // If the right arg is HC, ...
        if (IsHCAny (aplTypeRht))
        {
            Assert (IsSingleton (aplNELMRht));

            // Save the new ptr so we can free it at the end
            lpatRht2 = &atRht2;

            // Demote the right arg
            (*aTypeActConvert[aplTypeRht][aplTypeRht2]) (lpatRht2, 0, lpatRht, &bRet);

            if (!bRet)
                goto RIGHT_DOMAIN_EXIT;
            // Save as new storage type
            aplTypeRht = aplTypeRht2;
            lpatRht    = lpatRht2;
        } // End IF

        // If the left arg is HC, ...
        if (IsHCAny (aplTypeLft))
        {
            // Allocate new and Demote the left arg
            hGlbLft2 = AllocateDemote (aplTypeLft2,         // Result storage type
                                       hGlbLft,             // Left arg global memory handle (may be NULL)
                                       NULL,                // Ptr to ALLTYPES values (may be NULL)
                                       aplTypeLft,          // ... storage type
                                       aplNELMLft,          // ... NELM
                                       aplRankLft,          // ... rank
                                      &bRet);               // TRUE iff the result is not demotable
            // We no longer need this ptr
            MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;

            // Check for error
            if (!bRet)
                goto LEFT_DOMAIN_EXIT;
            if (hGlbLft2 EQ NULL)
                goto WSFULL_EXIT;
            // Save as the new global handle and storage type
            hGlbLft    = hGlbLft2;
            aplTypeLft = aplTypeLft2;

            // Lock the memory to get a ptr to it
            lpMemHdrLft = MyGlobalLockVar (hGlbLft);
        } // End IF
    } // End IF

    // Allocate space for result
    if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                    &hGlbRes,
                                     NULL,
                                     lpMemHdrLft,
                                     aplRankRht,
                                     aplRankLft,
                                    &aplRankRes,
                                     aplTypeRes,
                                     bLftIdent,
                                     bRhtIdent,
                                     aplNELMRht,
                                     aplNELMLft,
                                     aplNELMRes))
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Skip over the header and dimensions to the data
    lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // If the left arg is an APA, ...
    if (IsSimpleAPA (aplTypeLft))
    {
#define lpAPA       ((LPAPLAPA) lpMemLft)
        // Get the APA parameters
        apaOffLft = lpAPA->Off;
        apaMulLft = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Loop through the left arg/result
    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMLft; uRes++)
    {
        APLSTYPE aplTypeHetLft;

        // If the left arg is nested, ...
        if (IsNested (aplTypeLft))
        {
            // Get the left arg element
            hGlbSub = ((LPAPLNESTED) lpMemLft)[uRes];

            // Split cases based upon the ptr type of the nested left arg
            switch (GetPtrTypeDir (hGlbSub))
            {
                case PTRTYPE_STCONST:
                    atLft.aplLongest = ((LPSYMENTRY) hGlbSub)->stData.stLongest;
                    aplTypeHetLft    = TranslateImmTypeToArrayType (((LPSYMENTRY) hGlbSub)->stFlags.ImmType);

                    // Check for NaNs

                    // Are the args a NaN?
                    bNaNLft = IsArgNaN (aplTypeHetLft, &atLft.aplLongest, 0);
                    bNaNRht = IsArgNaN (aplTypeRht   ,  lpatRht         , 0);

                    // If either arg is a NaN, ...
                    if (bNaNLft || bNaNRht)
                    {
                        if (gbAllowNaN)
                        {
                            if (bNaNLft)
                                hGlbSub =
                                  MakeSymEntry_EM (((LPSYMENTRY) hGlbSub)->stFlags.ImmType,     // Immediate type
                                                  &atLft.aplLongest,                            // Ptr to immediate value
                                                   lptkFunc);                                   // Ptr to function token
                            else
                                hGlbSub =
                                  MakeGlbEntry_EM (aplTypeCom,              // Entry type
                                                   lpatRht,                 // Ptr to the value
                                                   TRUE,                    // TRUE iff we should initialize the target first
                                                   lptkFunc);               // Ptr to function token
                        } else
                        if (bNaNLft)
                            goto LEFT_DOMAIN_EXIT;
                        else
                            goto RIGHT_DOMAIN_EXIT;
                    } else
                    {
                        hGlbSub = PrimFnDydSiScSiSc_EM (lptkFunc,
                                                        hGlbRes,
                                                        aplTypeHetLft,
                                                       &atLft,
                                                        aplTypeRht,
                                                        lpatRht,
                                                        lpPrimSpec);
                        // No need to free the old atLft as it isn't created (it's copied)
                    } // End IF/ELSE

                    if (hGlbSub EQ NULL)
                        goto ERROR_EXIT;
                    else
                    // If the result is not already filled in, ...
                    if (((LPAPLNESTED) lpMemRes)[0] EQ NULL)
                        // Save in the result
                        ((LPAPLNESTED) lpMemRes)[0] = hGlbSub;
                    // Skip to the next result
                    ((LPAPLNESTED) lpMemRes)++;

                    break;

                case PTRTYPE_HGLOBAL:
                    // ({enclose}0 1)(0 1 2)-2 4
                    hGlbSub = PrimFnDydNestSiSc_EM (lptkFunc,
                                                    aplTypeRht,
                                                    lpatRht,
                                                    hGlbSub,
                                                    bLftIdent,
                                                    bRhtIdent,
                                                    lpPrimSpec);
                    if (hGlbSub EQ NULL)
                        goto ERROR_EXIT;
                    else
                        // Save in the result
                        *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeGlb (hGlbSub);
                    break;

                defstop
                    break;
            } // End SWITCH
        } else
        // The left arg is hetero, simple numeric, or simple global numeric
        {
            APLLONGEST aplLongestLft;
            LPSYMENTRY lpSymGlbSub;
            ALLTYPES   atTmp = {0};

            Assert (IsSimpleHet (aplTypeLft)
                 || IsNumeric   (aplTypeLft));

            // If the left arg is hetero, ...
            if (IsSimpleHet (aplTypeLft))
            {
                // Get a ptr to the left arg element
                lpSymGlbSub = ((LPAPLNESTED) lpMemLft)[uRes];

                aplLongestLft = lpSymGlbSub->stData.stLongest;
                aplTypeHetLft = TranslateImmTypeToArrayType (lpSymGlbSub->stFlags.ImmType);
            } else
            // The left arg is a simple numeric array or simple global numeric array
            {
                // Copy the left arg to an ALLTYPES
                (*aTypeActPromote[aplTypeLft][aplTypeLft]) (lpMemLft, uRes, &atTmp);

                // Get a ptr to the left arg element
                lpSymGlbSub = (LPVOID) &atTmp;
                aplLongestLft = atTmp.aplLongest;

                // Save as left arg element type
                aplTypeHetLft = aplTypeLft;
            } // End IF/ELSE

            // Check for NaNs

            // Are the args a NaN?
            bNaNLft = IsArgNaN (aplTypeHetLft, &aplLongestLft, 0);
            bNaNRht = IsArgNaN (aplTypeRht   ,  lpatRht      , 0);

            // If either arg is a NaN, ...
            if (bNaNLft || bNaNRht)
            {
                if (gbAllowNaN)
                {
////////////////////DbgBrk ();          // #4:  {NaN} ({NaN} 3) + 1x

                    if (bNaNLft)
                        // Copy the left arg to the result
                        (*aTypeActPromote[aplTypeHetLft][aplTypeRes]) (&aplLongestLft, 0, (LPALLTYPES) ByteAddr (lpMemRes, uRes * iSizeofRes));
                    else
                        // Copy the right arg to the result
                        (*aTypeActPromote[aplTypeRht   ][aplTypeRes]) (lpatRht       , 0, (LPALLTYPES) ByteAddr (lpMemRes, uRes * iSizeofRes));
                } else
                if (bNaNLft)
                    goto LEFT_DOMAIN_EXIT;
                else
                    goto RIGHT_DOMAIN_EXIT;
            } else
            {
                // Get the common storage type between the left & right args
                aplTypeCom = aTypePromote[aplTypeHetLft][aplTypeRht];

                // Promote the right arg to the common type
                (*aTypeActPromote[aplTypeRht][aplTypeCom]) (lpatRht, 0, &atRht);

                // If the left arg is hetero, ...
                if (IsSimpleHet (aplTypeLft))
                    // Promote the left arg to the common type
                    (*aTypeActPromote[aplTypeHetLft][aplTypeCom]) (&aplLongestLft, 0, &atLft);
                else
                    // Promote the left arg to the common type
                    (*aTypeActPromote[aplTypeHetLft][aplTypeCom]) (lpSymGlbSub   , 0, &atLft);

                // Get the target function
                lpPrimFn = TranslateTypesToDydPSFIndex (lpPrimSpec, aplTypeRes, aplTypeCom);

                __try
                {
                    __try
                    {
                        // Call the function
                        (*lpPrimFn) (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
                    } __finally
                    {
                        // Free the old atLft & atRht (if any)
                        (*aTypeFree[aplTypeCom]) (&atLft, 0);
                        (*aTypeFree[aplTypeCom]) (&atRht, 0);
                    } // End __try/__finally
                } __except (CheckException (GetExceptionInformation (), L"PrimFnDydNestSiSc_EM"))
                {
                    EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

                    dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L" #7: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    // Split cases based upon the ExceptionCode
                    switch (ExceptionCode)
                    {
                        case EXCEPTION_DOMAIN_ERROR:
                        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                        case EXCEPTION_INT_DIVIDE_BY_ZERO:
                            MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                            goto DOMAIN_EXIT;

                        case EXCEPTION_WS_FULL:
                            goto WSFULL_EXIT;

                        case EXCEPTION_NONCE_ERROR:
                            MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                            goto NONCE_EXIT;

                        case EXCEPTION_RESULT_FLOAT:
                        case EXCEPTION_RESULT_HC2F:
                        case EXCEPTION_RESULT_HC4F:
                        case EXCEPTION_RESULT_HC8F:
                        case EXCEPTION_RESULT_VFP:
                        case EXCEPTION_RESULT_HC2V:
                        case EXCEPTION_RESULT_HC4V:
                        case EXCEPTION_RESULT_HC8V:
                            MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                            if (IsNumeric (aplTypeRes))
                            {
                                if (hGlbRes NE NULL)
                                {
                                    if (lpMemHdrRes NE NULL)
                                    {
                                        // We need to start over with the result
                                        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
                                    } // End IF

                                    // We no longer need this storage
                                    FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
                                } // End IF

                                // It's now a promoted result
                                aplTypeRes = TranslateExceptionCodeToArrayType (ExceptionCode);

                                dprintfWL0 (L"!!Restarting Exception in " APPEND_NAME L" #7: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                                goto RESTART_EXCEPTION;
                            } // End IF

                            // Display message for unhandled exception
                            DisplayException ();

                            break;

                        default:
                            // Display message for unhandled exception
                            DisplayException ();

                            break;
                    } // End SWITCH
                } // End __try/__except
            } // End IF/ELSE
        } // End IF/ELSE
    } // End FOR

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
RIGHT_DOMAIN_EXIT:
DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    bRet = FALSE;

    // If we allocated to demote data
    if (lpatRht2 NE NULL)
    {
        // We no longer need this resource
        (*aTypeFree[aplTypeRht2]) (lpatRht2, 0);
    } // End IF

    // If we allocated to demote data
    if (hGlbLft2 NE NULL)
    {
        // We no longer need this resource
        FreeResultGlobalIncompleteVar (hGlbLft2); hGlbLft2 = NULL;
    } // End IF

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
    if (lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;

    if (bRet)
        return hGlbRes;
    else
        return NULL;
} // End PrimFnDydNestSiSc_EM
#undef  APPEND_NAME


//***************************************************************************
//  $FillToken_PTB
//
//  Fill in a token from a Sym/Glb whose value is sensitive to Ptr Type Bits.
//***************************************************************************

void FillToken_PTB
    (LPTOKEN lptkArg,           // Ptr to arg token
     LPVOID  lpSymGlb,          // Ptr to either HGLOBAL or LPSYMENTRY
     UINT    tkCharIndex)       // Character index in case of error

{
    // Split cases based upon the ptr type
    switch (GetPtrTypeDir (lpSymGlb))
    {
        case PTRTYPE_STCONST:
            lptkArg->tkFlags.TknType   = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType   = ((LPSYMENTRY) lpSymGlb)->stFlags.ImmType;
////////////lptkArg->tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
            lptkArg->tkData.tkLongest  = ((LPSYMENTRY) lpSymGlb)->stData.stLongest;
            lptkArg->tkCharIndex       = tkCharIndex;

            break;

        case PTRTYPE_HGLOBAL:
            lptkArg->tkFlags.TknType   = TKT_VARARRAY;
////////////lptkArg->tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////////lptkArg->tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
            lptkArg->tkData.tkGlbData  = lpSymGlb;
            lptkArg->tkCharIndex       = tkCharIndex;

            break;

        defstop
            break;
    } // End SWITCH
} // End FillToken_PTB


//***************************************************************************
//  $PrimFnDydNestNest_EM
//
//  Dyadic primitive scalar function, left nested/hetero, right nested/hetero
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydNestNest_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydNestNest_EM
    (LPPL_YYSTYPE      lpYYRes,         // Ptr to the result

     LPTOKEN           lptkLftArg,      // Ptr to left arg token
     LPTOKEN           lptkFunc,        // ...    function ...
     LPTOKEN           lptkRhtArg,      // ...    right arg ...

     HGLOBAL           hGlbLft,         // Left arg handle
     HGLOBAL           hGlbRht,         // Right ...
     HGLOBAL          *lphGlbRes,       // Ptr to result handle

     LPVARARRAY_HEADER lpMemHdrLft,     // Ptr to left arg header
     LPVARARRAY_HEADER lpMemHdrRht,     // ...    right ...

     LPAPLUINT         lpMemAxisHead,   // Ptr to axis values, fleshed out by CheckAxis_EM
     LPAPLUINT         lpMemAxisTail,   // Ptr to grade up of AxisHead

     APLRANK           aplRankLft,      // Left arg rank
     APLRANK           aplRankRht,      // Right ...
     APLRANK           aplRankRes,      // Result ...

     APLSTYPE          aplTypeLft,      // Left arg type
     APLSTYPE          aplTypeRht,      // Right ...
     APLSTYPE          aplTypeRes,      // Result ...

     APLNELM           aplNELMLft,      // Left arg NELM
     APLNELM           aplNELMRht,      // Right ...
     APLNELM           aplNELMRes,      // Result ...
     APLNELM           aplNELMAxis,     // Axis ...

     UBOOL             bLftIdent,       // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL             bRhtIdent,       // ...                         right ...

     LPPRIMSPEC        lpPrimSpec)      // Ptr to local PRIMSPEC

{
    UBOOL             bRet = TRUE;          // TRUE iff result is valid
    APLINT            uRes;                 // Loop counter
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPVOID            lpMemLft,             // Ptr to left arg global memory
                      lpMemRht,             // ...    right ...
                      lpMemRes;             // ...    result   ...
    LPPL_YYSTYPE      lpYYRes2;             // Ptr to the secondary result

    // Check for INNER RANK and LENGTH ERRORs
    if (!CheckRankLengthError_EM (aplRankRes,
                                  aplRankLft,
                                  aplNELMLft,
                                  lpMemHdrLft,
                                  aplRankRht,
                                  aplNELMRht,
                                  lpMemHdrRht,
                                  aplNELMAxis,
                                  lpMemAxisTail,
                                  bLftIdent,
                                  bRhtIdent,
                                  lptkFunc))
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

    // Skip over the headers and dimensions to the data
    lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Loop through the left and right args
    for (uRes = 0; bRet && uRes < (APLNELMSIGN) aplNELMRes; uRes++)
    {
        TOKEN     tkLft = {0},
                  tkRht = {0};

        // Fill in the left arg token
        FillToken_PTB (&tkLft,
                        ((LPAPLNESTED) lpMemLft)[uRes % aplNELMLft],
                        lptkLftArg->tkCharIndex);
        // Fill in the right arg token
        FillToken_PTB (&tkRht,
                        ((LPAPLNESTED) lpMemRht)[uRes % aplNELMRht],
                        lptkRhtArg->tkCharIndex);
        // Call as dyadic function
        lpYYRes2 =
          (*lpPrimSpec->PrimFnDyd_EM_YY) (&tkLft,           // Ptr to left arg token
                                           lptkFunc,        // Ptr to function token
                                          &tkRht,           // Ptr to right arg token
                                           NULL,            // Ptr to axis token
                                           lpPrimSpec);     // Ptr to local PRIMSPEC
        if (lpYYRes2 NE NULL)
        {
            // If the result is Boolean, ...
            if (IsSimpleBool (aplTypeRes))
            {
                ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |=
                  lpYYRes2->tkToken.tkData.tkBoolean
                  << (MASKLOG2NBIB & (UINT) uRes);
            } else
            // If the result is immediate, make it into a SYMENTRY
            // Split cases based upon the result token type
            switch (lpYYRes2->tkToken.tkFlags.TknType)
            {
                case TKT_VARIMMED:
                    // Save in the result
                    ((LPAPLNESTED) lpMemRes)[uRes] =
                      MakeSymEntry_EM (lpYYRes2->tkToken.tkFlags.ImmType,   // Immediate type
                                      &lpYYRes2->tkToken.tkData.tkLongest,  // Ptr to immediate value
                                       lptkFunc);                           // Ptr to function token
                    break;

                case TKT_VARARRAY:
                    // Save in the result
                    ((LPAPLNESTED) lpMemRes)[uRes] =
                      lpYYRes2->tkToken.tkData.tkGlbData;
                    break;

                defstop
                    break;
            } // End IF/ELSE/SWITCH

            // Free the YYRes
            YYFree (lpYYRes2); lpYYRes2 = NULL;
        } else
            bRet = FALSE;
    } // End FOR

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;

    if (bRet)
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF
ERROR_EXIT:
    return bRet;
} // End PrimFnDydNestNest_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSiScNest_EM
//
//  Subroutine to PrimFnDydSimpNest_EM to handle left arg simple or
//    global numeric scalars, right arg nested/hetero/global numeric
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSiScNest_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL PrimFnDydSiScNest_EM
    (LPTOKEN    lptkFunc,           // Ptr to function token
     APLSTYPE   aplTypeLft,         // Left arg type
     LPALLTYPES lpatLft,            // Left arg value
     HGLOBAL    hGlbRht,            // Right arg nested value
     UBOOL      bLftIdent,          // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL      bRhtIdent,          // ...                         right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    UBOOL             bRet      = TRUE,
                      bNaNLft,                  // TRUE iff the left arg is a NaN
                      bNaNRht,                  // ...          right ...
                      bRealOnly = FALSE;        // TRUE iff the args must be demoted to real
    HGLOBAL           hGlbRes = NULL,
                      hGlbSub,
                      hGlbRht2 = NULL;          // Temp handle for type demotion
    LPVARARRAY_HEADER lpMemHdrRes = NULL,
                      lpMemHdrRht = NULL;
    LPVOID            lpMemRht,
                      lpMemRes;
    APLSTYPE          aplTypeRht,
                      aplTypeRes,
                      aplTypeCom,
                      aplTypeLft2,              // Left arg base storage type
                      aplTypeRht2;              // Right ...
    APLNELM           aplNELMLft = 1,
                      aplNELMRht,
                      aplNELMRes;
    APLRANK           aplRankLft = 0,
                      aplRankRht,
                      aplRankRes;
    APLINT            uRes,
                      apaOffRht,
                      apaMulRht;
    UINT              uBitIndex = 0;
    ATISATVAT        *lpPrimFn;
    ALLTYPES          atLft = {0},
                      atRht = {0},
                      atLft2 = {0};
    LPALLTYPES        lpatLft2 = NULL;
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
    int               iSizeofRes;           // Sizeof an item in the result

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // The right arg data is a valid HGLOBAL array
    Assert (IsGlbTypeVarDir_PTB (hGlbRht));

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLockVar (hGlbRht);

#define lpHeader    lpMemHdrRht
    // Get the Array Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // The right arg is a ptr array (nested or Hetero)
    //   or a simple global numeric
    Assert (IsPtrArray     (aplTypeRht)
         || IsSimpleGlbNum (aplTypeRht));

    // The NELM of the result is NELM of the non-scalar
    aplNELMRes = aplNELMRht;

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                               &aplTypeLft,
                                                lptkFunc,
                                                aplNELMRht,
                                               &aplTypeRht);
    if (IsRealType  (aplTypeRes))
    {
        // Calculate the left arg base storage type
        aplTypeLft2 = aToSimple[aplTypeLft];

        // Calculate the left arg base storage type
        aplTypeRht2 = aToSimple[aplTypeRht];

        // Set the result storage type to the base type
        aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                                   &aplTypeLft2,
                                                    lptkFunc,
                                                    aplNELMRht,
                                                   &aplTypeRht2);
        // Mark as requiring type demotion of one or both args
        bRealOnly = TRUE;
    } else
    if (IsNonceType (aplTypeRes))
        goto NONCE_EXIT;
    else
    if (IsErrorType (aplTypeRes))
        goto DOMAIN_EXIT;

    // Calculate the sizeof
    iSizeofRes = TranslateArrayTypeToSizeof (aplTypeRes);

    // Special case APA result
    if (IsSimpleAPA (aplTypeRes))
    {
        if (!(*lpPrimSpec->ApaResultDyd_EM) (NULL,
                                             lptkFunc,
                                             NULL,
                                             hGlbRht,
                                            &hGlbRes,
                                             aplRankLft,
                                             aplRankRht,
                                             aplNELMLft,
                                             aplNELMRht,
                                             lpatLft->aplInteger,
                                             lpPrimSpec))
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } // End IF
RESTART_EXCEPTION:
    Assert (IsSimpleGlbNum (aplTypeRes)
         || IsNested (aplTypeRes));

    // If one or both args must be demoted, ...
    if (bRealOnly)
    {
        // If the left arg is HC, ...
        if (IsHCAny (aplTypeLft))
        {
            Assert (IsSingleton (aplNELMLft));

            // Save the new ptr so we can free it at the end
            lpatLft2 = &atLft2;

            // Demote the left arg
            (*aTypeActConvert[aplTypeLft][aplTypeLft2]) (lpatLft2, 0, lpatLft, &bRet);

            if (!bRet)
                goto LEFT_DOMAIN_EXIT;
            // Save as new storage type
            aplTypeLft = aplTypeLft2;
            lpatLft    = lpatLft2;
        } // End IF

        // If the right arg is HC, ...
        if (IsHCAny (aplTypeRht))
        {
            // Allocate new and Demote the right arg
            hGlbRht2 = AllocateDemote (aplTypeRht2,         // Result storage type
                                       hGlbRht,             // Right arg global memory handle (may be NULL)
                                       NULL,                // Ptr to ALLTYPES values (may be NULL)
                                       aplTypeRht,          // ... storage type
                                       aplNELMRht,          // ... NELM
                                       aplRankRht,          // ... rank
                                      &bRet);               // TRUE iff the result is not demotable
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

            // Check for error
            if (!bRet)
                goto RIGHT_DOMAIN_EXIT;
            if (hGlbRht2 EQ NULL)
                goto WSFULL_EXIT;
            // Save as the new global handle and storage type
            hGlbRht    = hGlbRht2;
            aplTypeRht = aplTypeRht2;

            // Lock the memory to get a ptr to it
            lpMemHdrRht = MyGlobalLockVar (hGlbRht);
        } // End IF
    } // End IF

    // Allocate space for result
    if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                    &hGlbRes,
                                     NULL,
                                     lpMemHdrRht,
                                     aplRankLft,
                                     aplRankRht,
                                    &aplRankRes,
                                     aplTypeRes,
                                     bRhtIdent,
                                     bLftIdent,
                                     aplNELMLft,
                                     aplNELMRht,
                                     aplNELMRes))
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRht    = VarArrayDataFmBase (lpMemHdrRht);
    lpMemRes    = VarArrayDataFmBase (lpMemHdrRes);

    // If the right arg is an APA, ...
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Loop through the right arg/result
    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
    {
        APLSTYPE aplTypeHetRht;

        // If the right arg is nested, ...
        if (IsNested (aplTypeRht))
        {
            // Get the right arg element
            hGlbSub = ((LPAPLNESTED) lpMemRht)[uRes];

            // Split cases based upon the ptr type of the nested right arg
            switch (GetPtrTypeDir (hGlbSub))
            {
                case PTRTYPE_STCONST:
                    atRht.aplLongest = ((LPSYMENTRY) hGlbSub)->stData.stLongest;
                    aplTypeHetRht    = TranslateImmTypeToArrayType (((LPSYMENTRY) hGlbSub)->stFlags.ImmType);

                    // Check for NaNs

                    // Are the args a NaN?
                    bNaNLft = IsArgNaN (aplTypeLft   ,  lpatLft                                 , 0);
                    bNaNRht = IsArgNaN (aplTypeHetRht, &((LPSYMENTRY) hGlbSub)->stData.stLongest, 0);

                    // If either arg is a NaN, ...
                    if (bNaNLft || bNaNRht)
                    {
                        if (gbAllowNaN)
                        {
                            if (bNaNLft)
                                hGlbSub =
                                  MakeGlbEntry_EM (aplTypeLft,              // Entry type
                                                   lpatLft,                 // Ptr to the value
                                                   TRUE,                    // TRUE iff we should initialize the target first
                                                   lptkFunc);               // Ptr to function token
                            else
                                hGlbSub =
                                  MakeSymEntry_EM (((LPSYMENTRY) hGlbSub)->stFlags.ImmType,     // Immediate type
                                                  &((LPSYMENTRY) hGlbSub)->stData.stLongest,    // Ptr to immediate value
                                                   lptkFunc);                                   // Ptr to function token
                        } else
                        if (bNaNLft)
                            goto LEFT_DOMAIN_EXIT;
                        else
                            goto RIGHT_DOMAIN_EXIT;
                    } else
                    {
                        hGlbSub = PrimFnDydSiScSiSc_EM (lptkFunc,
                                                        hGlbRes,
                                                        aplTypeLft,
                                                        lpatLft,
                                                        aplTypeHetRht,
                                                       &atRht,
                                                        lpPrimSpec);
                        // No need to free the old atRht as it isn't created (it's copied)
                    } // End IF/ELSE

                    if (hGlbSub EQ NULL)
                        goto ERROR_EXIT;
                    else
                    // If the result is not already filled in, ...
                    if (((LPAPLNESTED) lpMemRes)[0] EQ NULL)
                        // Save in the result
                        ((LPAPLNESTED) lpMemRes)[0] = hGlbSub;
                    // Skip to the next result
                    ((LPAPLNESTED) lpMemRes)++;

                    break;

                case PTRTYPE_HGLOBAL:
                    // 2 4-({enclose}0 1)(0 1 2)
                    hGlbSub = PrimFnDydSiScNest_EM (lptkFunc,
                                                    aplTypeLft,
                                                    lpatLft,
                                                    hGlbSub,
                                                    bRhtIdent,
                                                    bLftIdent,
                                                    lpPrimSpec);
                    if (hGlbSub EQ NULL)
                        goto ERROR_EXIT;
                    else
                        // Save in the result
                        *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeGlb (hGlbSub);
                    break;

                defstop
                    break;
            } // End SWITCH
        } else
        // The right arg is hetero or simple global numeric
        {
            APLLONGEST aplLongestRht;
            LPSYMENTRY lpSymGlbSub;
            ALLTYPES   atTmp = {0};

            // If the right arg is hetero, ...
            if (IsSimpleHet (aplTypeRht))
            {
                // Get a ptr to the right arg element
                lpSymGlbSub = ((LPAPLNESTED) lpMemRht)[uRes];

                aplLongestRht = lpSymGlbSub->stData.stLongest;
                aplTypeHetRht = TranslateImmTypeToArrayType (lpSymGlbSub->stFlags.ImmType);
            } else
            // The right arg is a simple global numeric array
            {
                // Copy the right arg to an ALLTYPES
                (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, uRes, &atTmp);

                // Get a ptr to the right arg element
                lpSymGlbSub   = (LPVOID) &atTmp;
                aplLongestRht = atTmp.aplLongest;

                // Save as right arg element type
                aplTypeHetRht = aplTypeRht;
            } // End IF/ELSE

            // Check for NaNs

            // Are the args a NaN?
            bNaNLft = IsArgNaN (aplTypeLft   ,  lpatLft      , 0);
            bNaNRht = IsArgNaN (aplTypeHetRht, &aplLongestRht, 0);

            // If either arg is a NaN, ...
            if (bNaNLft || bNaNRht)
            {
                if (gbAllowNaN)
                {
////////////////////DbgBrk ();          // #5:  {NaN}x + 1 (2 3)

                    if (bNaNLft)
                        // Copy the left arg to the result
                        (*aTypeActPromote[aplTypeLft   ][aplTypeRes]) (lpatLft       , 0, (LPALLTYPES) ByteAddr (lpMemRes, uRes * iSizeofRes));
                    else
                        // Copy the right arg to the result
                        (*aTypeActPromote[aplTypeHetRht][aplTypeRes]) (&aplLongestRht, 0, (LPALLTYPES) ByteAddr (lpMemRes, uRes * iSizeofRes));
                } else
                if (bNaNLft)
                    goto LEFT_DOMAIN_EXIT;
                else
                    goto RIGHT_DOMAIN_EXIT;
            } else
            {
                // Get the common storage type between the left & right args
                aplTypeCom = aTypePromote[aplTypeLft][aplTypeHetRht];

                // Promote the left arg to the common type
                (*aTypeActPromote[aplTypeLft][aplTypeCom]) (lpatLft, 0, &atLft);

                // If the right arg is hetero, ...
                if (IsSimpleHet (aplTypeRht))
                    // Promote the right arg to the common type
                    (*aTypeActPromote[aplTypeHetRht][aplTypeCom]) (&aplLongestRht, 0, &atRht);
                else
                    // Promote the right arg to the common type
                    (*aTypeActPromote[aplTypeHetRht][aplTypeCom]) (lpSymGlbSub   , 0, &atRht);

                // Get the target function
                lpPrimFn = TranslateTypesToDydPSFIndex (lpPrimSpec, aplTypeRes, aplTypeCom);

                __try
                {
                    __try
                    {
                        // Call the function
                        (*lpPrimFn) (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
                    } __finally
                    {
                        // Free the old atLft & atRht (if any)
                        (*aTypeFree[aplTypeCom]) (&atLft, 0);
                        (*aTypeFree[aplTypeCom]) (&atRht, 0);
                    } // End __try/__finally
                } __except (CheckException (GetExceptionInformation (), L"PrimFnDydNestSiSc_EM"))
                {
                    EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

                    dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L" #8: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    // Split cases based upon the ExceptionCode
                    switch (ExceptionCode)
                    {
                        case EXCEPTION_DOMAIN_ERROR:
                        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                        case EXCEPTION_INT_DIVIDE_BY_ZERO:
                            MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                            goto DOMAIN_EXIT;

                        case EXCEPTION_WS_FULL:
                            goto WSFULL_EXIT;

                        case EXCEPTION_NONCE_ERROR:
                            MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                            goto NONCE_EXIT;

                        case EXCEPTION_RESULT_FLOAT:
                        case EXCEPTION_RESULT_HC2F:
                        case EXCEPTION_RESULT_HC4F:
                        case EXCEPTION_RESULT_HC8F:
                        case EXCEPTION_RESULT_VFP:
                        case EXCEPTION_RESULT_HC2V:
                        case EXCEPTION_RESULT_HC4V:
                        case EXCEPTION_RESULT_HC8V:
                            MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                            if (IsNumeric (aplTypeRes))
                            {
                                if (hGlbRes NE NULL)
                                {
                                    if (lpMemHdrRes NE NULL)
                                    {
                                        // We need to start over with the result
                                        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
                                    } // End IF

                                    // We no longer need this storage
                                    FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
                                } // End IF

                                // It's now a promoted result
                                aplTypeRes = TranslateExceptionCodeToArrayType (ExceptionCode);

                                dprintfWL0 (L"!!Restarting Exception in " APPEND_NAME L" #8: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                                goto RESTART_EXCEPTION;
                            } // End IF

                            // Display message for unhandled exception
                            DisplayException ();

                            break;

                        default:
                            // Display message for unhandled exception
                            DisplayException ();

                            break;
                    } // End SWITCH
                } // End __try/__except
            } // End IF/ELSE
        } // End IF/ELSE
    } // End FOR

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
RIGHT_DOMAIN_EXIT:
DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    bRet = FALSE;

    // If we allocated to demote data
    if (lpatLft2 NE NULL)
    {
        // We no longer need this resource
        (*aTypeFree[aplTypeLft2]) (lpatLft2, 0);
    } // End IF

    // If we allocated to demote data
    if (hGlbRht2 NE NULL)
    {
        // We no longer need this resource
        FreeResultGlobalIncompleteVar (hGlbRht2); hGlbRht2 = NULL;
    } // End IF

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
    if (lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

    if (bRet)
        return hGlbRes;
    else
        return NULL;
} // End PrimFnDydSiScNest_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSiScSiSc_EM
//
//  Dyadic primitive scalar function, left simple scalar, right simple scalar
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSiScSiSc_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL PrimFnDydSiScSiSc_EM
    (LPTOKEN    lptkFunc,           // Ptr to function token
     HGLOBAL    hGlbRes,            // Result global memory handle
     APLSTYPE   aplTypeLft,         // Left arg storage type
     LPALLTYPES lpatLft,            // ...      as ALLTYPES
     APLSTYPE   aplTypeRht,         // Right arg storage type
     LPALLTYPES lpatRht,            // ...      as ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    TOKEN    tkRes = {0};   // Result token
    APLSTYPE aplTypeRes;    // Result storage type

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (1,
                                               &aplTypeLft,
                                                lptkFunc,
                                                1,
                                               &aplTypeRht);
    if (IsNonceType (aplTypeRes))
        goto NONCE_EXIT;

    if (IsErrorType (aplTypeRes))
        goto DOMAIN_EXIT;

    if (PrimFnDydSiScSiScSub_EM (&tkRes,
                                  NULL,                     // Result as ALLTYPES (may be NULL)
                                  lptkFunc,
                                 &hGlbRes,
                                  aplTypeRes,
                                  aplTypeLft,
                                  lpatLft,
                                  aplTypeRht,
                                  lpatRht,
                                 &aplTypeRes,
                                  lpPrimSpec))
    {
        // If the result is an immediate, ...
        if (IsTknImmed (&tkRes))
            // Convert the immediate type and value in tkRes
            //   into an LPSYMENTRY
            return MakeSymEntry_EM (tkRes.tkFlags.ImmType,      // Immediate type
                                   &tkRes.tkData.tkLongest,     // Ptr to immediate value
                                    lptkFunc);                  // Ptr to function token
        else
            return tkRes.tkData.tkGlbData;
    } else
        return NULL;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnDydSiScSiSc_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSiScSiScSub_EM
//
//  Subroutine to PrimFnDydSiScSiSc_EM
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSiScSiScSub_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydSiScSiScSub_EM
    (LPTOKEN    lptkRes,            // Ptr to result token
     LPALLTYPES lpatRes,            // Result as ALLTYPES (may be NULL)
     LPTOKEN    lptkFunc,           // Ptr to function token
     HGLOBAL   *lphGlbRes,          // Ptr to result global memory handle (may be NULL)
     APLSTYPE   aplTypeRes,         // Result storage type
     APLSTYPE   aplTypeLft,         // Left arg storage type
     LPALLTYPES lpatLft,            // ...      as ALLTYPES
     APLSTYPE   aplTypeRht,         // Right arg storage type
     LPALLTYPES lpatRht,            // ...      as ALLTYPES
     APLSTYPE  *lpaplTypeRes,       // Ptr to result storage type
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    IMM_TYPES         immType;              // Result immediate type
    UBOOL             bRet = TRUE;          // TRUE iff result is valid
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPVOID            lpMemRes;             // Ptr to result global memory
    APLSTYPE          aplTypeCom;           // Common storage type
    HGLOBAL           hGlbTmp = NULL;       // Temporary hGlbRes
    ALLTYPES          atLft = {0},          // Left arg as ALLTYPES
                      atRht = {0};          // Right ...
    ATISATVAT        *lpPrimFn;             // Ptr to primitive dyadic scalar function

RESTART_EXCEPTION_IMMED:
    // In case we restart, ...
    if (lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTmp); lpMemHdrRes = NULL;
    } // End IF

    // If the result is simple, ...
    if (IsSimple (aplTypeRes))
    {
        // Get the immediate type for the token
        immType = TranslateArrayTypeToImmType (aplTypeRes);

        Assert (IsImmNum (immType));

        // Fill in the result token
        lptkRes->tkFlags.TknType   = TKT_VARIMMED;
        lptkRes->tkFlags.ImmType   = immType;
////////lptkRes->tkFlags.NoDisplay = FALSE;     // Already zero from YYAlloc
        lptkRes->tkCharIndex       = lptkFunc->tkCharIndex;
    } else
    {
        // Fill in the result token
        lptkRes->tkFlags.TknType   = TKT_VARARRAY;
////////lptkRes->tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lptkRes->tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lptkRes->tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE

    __try
    {
        // If the result is global numeric,
        if (IsGlbNum (aplTypeRes))
        {
            // Allocate space for the result
            if (!AllocateSpaceGlbNum_LOCK (lphGlbRes,       // Ptr to result global memory handle
                                          &hGlbTmp,         // ...    temp   ...
                                           aplTypeRes,      // Result storage type
                                           1,               // ...    NELM
                                           0,               // ...    rank
                                           lptkRes,         // Ptr to result token
                                          &lpMemHdrRes))    // Ptr to ptr to result header
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemRes = VarArrayDataFmBase (lpMemHdrRes);
        } else
        {
            // Ensure it's initialized to zero as some primitives assume so
            lptkRes->tkData.tkLongest = 0;

            // Point to the data save area
            lpMemRes = GetPtrTknLongest (lptkRes);
        } // End IF/ELSE

        // Get the common storage type between the left & right args
        aplTypeCom = aTypePromote[aplTypeLft][aplTypeRht];

        // If the HC dimension of the common type is less than
        //   the HC dimension of the result, set the common type
        //   to that of the result so we are sure to initialize the
        //   high-order parts in each argument
        if (TranslateArrayTypeToHCDim (aplTypeCom)
          < TranslateArrayTypeToHCDim (aplTypeRes))
            aplTypeCom = aplTypeRes;

        // Convert the left arg to the common datatype
        (*aTypeActPromote[aplTypeLft][aplTypeCom]) (lpatLft, 0, &atLft);

        // Convert the right arg to the common datatype
        (*aTypeActPromote[aplTypeRht][aplTypeCom]) (lpatRht, 0, &atRht);

        // Get the target function
        lpPrimFn = TranslateTypesToDydPSFIndex (lpPrimSpec, aplTypeRes, aplTypeCom);

        __try
        {
            __try
            {
                // Call the function
                // NaNs handled by the caller
                (*lpPrimFn) (lpMemRes, 0, &atLft, &atRht, lpPrimSpec);
            } __finally
            {
                // Free the old atLft & atRht (if any)
                (*aTypeFree[aplTypeCom]) (&atLft, 0);
                (*aTypeFree[aplTypeCom]) (&atRht, 0);

                // If the caller wants <lpatRes> filled in, ...
                if (lpatRes NE NULL)
                    // Copy from global memory to ALLTYPES
                    CopyMemory (lpatRes, lpMemRes, TranslateArrayTypeToSizeof (aplTypeRes));

                // Note that lpMemHdrRes locks hGlbTmp, not hGlbRes
                //   unless *lphGlbRes is valid and not nested ***FIXME***
                if (hGlbTmp NE NULL)
                {
                    if (lpMemHdrRes NE NULL)
                    {
                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbTmp); lpMemHdrRes = NULL;
                    } // End IF

                    // If the caller wants <lpatRes> filled in, ...
                    if (lpatRes NE NULL)
                    {
                        // We no longer need this storage
                        MyGlobalFree (hGlbTmp); hGlbTmp = NULL;
                    } // End IF
                } // End IF
            } // End __try/__finally
        } __except (EXCEPTION_CONTINUE_SEARCH) {}
    } __except (CheckException (GetExceptionInformation (), L"PrimFnDydSiScSiScSub_EM"))
    {
        EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L" #9: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

        // Split cases based upon the ExceptionCode
        switch (ExceptionCode)
        {
            case EXCEPTION_DOMAIN_ERROR:
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                goto DOMAIN_EXIT;

            case EXCEPTION_WS_FULL:
                goto WSFULL_EXIT;

            case EXCEPTION_NONCE_ERROR:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                goto NONCE_EXIT;

            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_RESULT_HC2F:
            case EXCEPTION_RESULT_HC4F:
            case EXCEPTION_RESULT_HC8F:
            case EXCEPTION_RESULT_VFP:
            case EXCEPTION_RESULT_HC2V:
            case EXCEPTION_RESULT_HC4V:
            case EXCEPTION_RESULT_HC8V:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                if (IsNumeric (aplTypeRes))
                {
                    if (lphGlbRes NE NULL && *lphGlbRes NE NULL)
                    {
                        if (lpMemHdrRes NE NULL)
                        {
                            // We need to start over with the result
                            MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                        } // End IF

                        // We no longer need this storage
                        FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;
                    } else
                    if (hGlbTmp NE NULL)
                    {
                        if (lpMemHdrRes NE NULL)
                        {
                            // We need to start over with the result
                            MyGlobalUnlock (hGlbTmp); lpMemHdrRes = NULL;
                        } // End IF

                        // We no longer need this storage
                        FreeResultGlobalVar (hGlbTmp); hGlbTmp = NULL;
                    } // End IF/ELSE

                    // It's now a VFP result
                    aplTypeRes = TranslateExceptionCodeToArrayType (ExceptionCode);

                    dprintfWL0 (L"!!Restarting Exception in " APPEND_NAME L" #9: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                    goto RESTART_EXCEPTION_IMMED;
                } // End IF

                // Display message for unhandled exception
                DisplayException ();

                break;

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbTmp NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbTmp); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbTmp); hGlbTmp = NULL;

        // If the caller passed it in, ...
        if (lphGlbRes NE NULL)
            *lphGlbRes = NULL;
    } // End IF

    bRet = FALSE;
NORMAL_EXIT:
    // Return as (possibly new) storage type
    *lpaplTypeRes = aplTypeRes;

    return bRet;
} // End PrimFnDydSiScSiScSub_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSimpSimp_EM
//
//  Dyadic primitive scalar function,
//    left simple or global numeric, right simple or global numeric
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSimpSimp_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydSimpSimp_EM
    (LPPL_YYSTYPE      lpYYRes,             // Ptr to the result

     LPTOKEN           lptkLftArg,          // Ptr to left arg token
     LPTOKEN           lptkFunc,            // ...    function ...
     LPTOKEN           lptkRhtArg,          // ...    right arg ...

     HGLOBAL           hGlbLft,             // Left arg handle
     HGLOBAL           hGlbRht,             // Right ...
     HGLOBAL          *lphGlbRes,           // Ptr to result handle (may point to NULL)

     LPVARARRAY_HEADER lpMemHdrLft,         // Points to Sig.nature (may be NULL if immediate)
     LPVARARRAY_HEADER lpMemHdrRht,         // ...

     LPAPLUINT         lpMemAxisHead,       // Ptr to axis values, fleshed out by CheckAxis_EM
     LPAPLUINT         lpMemAxisTail,       // Ptr to grade up of AxisHead

     APLRANK           aplRankLft,          // Left arg rank
     APLRANK           aplRankRht,          // Right ...
     APLRANK           aplRankRes,          // Result ...

     APLSTYPE          aplTypeLft,          // Left arg type
     APLSTYPE          aplTypeRht,          // Right ...
     APLSTYPE          aplTypeRes,          // Result ...

     APLNELM           aplNELMLft,          // Left arg NELM
     APLNELM           aplNELMRht,          // Right ...
     APLNELM           aplNELMRes,          // Result ...
     APLNELM           aplNELMAxis,         // Axis ...

     UBOOL             bLftIdent,           // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL             bRhtIdent,           // ...                         right ...

     LPPRIMSPEC        lpPrimSpec)          // Ptr to local PRIMSPEC

{
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header (in case we blow up)
    LPVOID            lpMemRes,             // Ptr to result global memory
                      lpMemLft,             // ...    left arg ...
                      lpMemRht;             // ...    right ...
    UBOOL             bRet = TRUE,          // TRUE iff result is valid
                      bNaNLft,              // TRUE iff the left arg is a NaN
                      bNaNRht,              // ...          right ...
                      bHetLft,              // TRUE iff the left arg is hetero
                      bHetRht;              // ...          right ...
    LPAPLUINT         lpMemWVec = NULL,
                      lpMemOdo = NULL;
    HGLOBAL           hGlbWVec = NULL,
                      hGlbOdo = NULL;
    APLINT            uLft,
                      uRht,
                      uRes;
    APLUINT           uLftInc,              // Left arg increment 0 (singleton) 1 not singleton)
                      uRhtInc,              // Right ...
                      uHetLft,              // Index into left arg in case it's hetero
                      uHetRht;              // ...        right ...
    LPAPLDIM          lpMemDimRes;
    APLINT            iRht;
    UINT              uBitIndex = 0;
    APLUINT           ByteRes;              // # bytes in the result
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLNELM           aplNELMTmp,           // Temporary NELM
                      aplNELMRem;           // Remaining NELM
    HGLOBAL           lpSymGlbLft,          // Left arg Sym/Glb
                      lpSymGlbRht;          // Right ...
    APLSTYPE          aplTypeHetLft,        // Left hetero types
                      aplTypeHetRht,        // Right ...
                      aplTypeCom;           // Common storage type
    UBOOL             bAPALft,              // TRUE iff the left arg is Boolean APA
                      bAPARht;              // ...          right ...
    ALLTYPES          atLft = {0},          // Left arg as ALLTYPES
                      atRht = {0};          // Right ...
    ATISATVAT        *lpPrimFn;             // Ptr to primitive dyadic scalar function
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
    int               iSizeofRes;           // Sizeof an item in the result

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Calculate the sizeof
    iSizeofRes = TranslateArrayTypeToSizeof (aplTypeRes);

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    //***************************************************************
    // Both arguments are simple
    //***************************************************************

    // APA result handled elsewhere
    Assert (!IsSimpleAPA (aplTypeRes));

    // If the result is a global, ...
    if (*lphGlbRes NE NULL)
    {
        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Skip over the header to the dimensions
        lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);
    } else
        // Point to the immediate data
        lpMemRes = GetPtrTknLongest (&lpYYRes->tkToken);

    // If the left arg is immediate, ...
    if (lpMemHdrLft EQ NULL)
    {
        // Point to the immediate data
        lpMemLft = GetPtrTknLongest (lptkLftArg);

        // Is the left arg Boolean APA?
        bAPALft = FALSE;
    } else
    {
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);

        // Is the left arg Boolean APA?
        bAPALft = (IsSimpleAPA (aplTypeLft) && IsBooleanAPA ((LPAPLAPA) lpMemLft));
    } // End IF/ELSE

    // If the right arg is immediate, ...
    if (lpMemHdrRht EQ NULL)
    {
        // Point to the immediate data
        lpMemRht = GetPtrTknLongest (lptkRhtArg);

        // Is the right arg Boolean APA?
        bAPARht = FALSE;
    } else
    {
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

        // Is the right arg Boolean APA?
        bAPARht = (IsSimpleAPA (aplTypeRht) && IsBooleanAPA ((LPAPLAPA) lpMemRht));
    } // End IF/ELSE

    // If we can chunk a Boolean result, ...
    if (IsSimpleBool (aplTypeRes)
      && (IsSimpleBool (aplTypeLft) || bAPALft)
      && (IsSimpleBool (aplTypeRht) || bAPARht)
      && !IsSingleton (aplNELMLft)
      && !IsSingleton (aplNELMRht)
      && lpMemAxisHead EQ NULL
      && lpPrimSpec->B08isB08vB08 NE NULL)
    {
        UBOOL   bIncLft,
                bIncRht;
        APLUINT All1s = 0xFFFFFFFFFFFFFFFF,
                All0s = 0x0000000000000000;

        // Boolean chunking
        // Initialize # remaining NELM
        aplNELMRem = aplNELMRes;

        // Check for optimized chunking
        if (lpPrimSpec->B64isB64vB64 NE NULL)
        {
            // Calculate the # 64-bit chunks
            aplNELMTmp  = aplNELMRem / 64;
            aplNELMRem -= aplNELMTmp * 64;

            // If the left arg is Boolean APA, ...
            if (bAPALft)
            {
                bIncLft = 0;
                lpSymGlbLft = (BIT0 & ((LPAPLAPA) lpMemLft)->Off) ? &All1s : &All0s;
            } else
            {
                bIncLft = 1;
                lpSymGlbLft = lpMemLft;
            } // End IF/ELSE

            // If the right arg is Boolean APA, ...
            if (bAPARht)
            {
                bIncRht = 0;
                lpSymGlbRht = (BIT0 & ((LPAPLAPA) lpMemRht)->Off) ? &All1s : &All0s;
            } else
            {
                bIncRht = 1;
                lpSymGlbRht = lpMemRht;
            } // End IF/ELSE

            // Loop through the left/right args/result
            for (uRes = 0;
                 uRes < (APLNELMSIGN) aplNELMTmp;
                 uRes++,
                   ((LPAPLB64) lpSymGlbLft) += bIncLft,
                   ((LPAPLB64) lpSymGlbRht) += bIncRht)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (*lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Save in the result
                *((LPAPLB64) lpMemRes)++ =
                  (*lpPrimSpec->B64isB64vB64) (*(LPAPLB64) lpSymGlbLft, *(LPAPLB64) lpSymGlbRht, lpPrimSpec);
            } // End FOR

            // Calculate the # remaining 32-bit chunks
            aplNELMTmp  = aplNELMRem / 32;
            aplNELMRem -= aplNELMTmp * 32;

            // Loop through the left/right args/result
            for (uRes = 0;
                 uRes < (APLNELMSIGN) aplNELMTmp;
                 uRes++,
                   ((LPAPLB32) lpSymGlbLft) += bIncLft,
                   ((LPAPLB32) lpSymGlbRht) += bIncRht)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (*lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Save in the result
                *((LPAPLB32) lpMemRes)++ =
                  (*lpPrimSpec->B32isB32vB32) (*(LPAPLB32) lpSymGlbLft, *(LPAPLB32) lpSymGlbRht, lpPrimSpec);
            } // End FOR

            // Calculate the # remaining 16-bit chunks
            aplNELMTmp  = aplNELMRem / 16;
            aplNELMRem -= aplNELMTmp * 16;

            // Loop through the left/right args/result
            for (uRes = 0;
                 uRes < (APLNELMSIGN) aplNELMTmp;
                 uRes++,
                   ((LPAPLB16) lpSymGlbLft) += bIncLft,
                   ((LPAPLB16) lpSymGlbRht) += bIncRht)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (*lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Save in the result
                *((LPAPLB16) lpMemRes)++ =
                  (*lpPrimSpec->B16isB16vB16) (*(LPAPLB16) lpSymGlbLft, *(LPAPLB16) lpSymGlbRht, lpPrimSpec);
            } // End FOR

            // Calculate the # remaining  8-bit chunks
            aplNELMTmp  = aplNELMRem /  8;
            aplNELMRem -= aplNELMTmp *  8;

            // Loop through the left/right args/result
            for (uRes = 0;
                 uRes < (APLNELMSIGN) aplNELMTmp;
                 uRes++,
                   ((LPAPLB08) lpSymGlbLft) += bIncLft,
                   ((LPAPLB08) lpSymGlbRht) += bIncRht)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (*lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Save in the result
                *((LPAPLB08) lpMemRes)++ =
                  (*lpPrimSpec->B08isB08vB08) (*(LPAPLB08) lpSymGlbLft, *(LPAPLB08) lpSymGlbRht, lpPrimSpec);
            } // End FOR
        } // End IF

        // Loop through the left/right args/result
        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRem; uRes++)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            // Copy the left arg to the common datatype
            (*aTypeActPromote[ARRAY_BOOL][ARRAY_BOOL]) (lpSymGlbLft, uRes, &atLft);

            // Copy the right arg to the common datatype
            (*aTypeActPromote[ARRAY_BOOL][ARRAY_BOOL]) (lpSymGlbRht, uRes, &atRht);

            // Save in the result
            (*lpPrimSpec->BisBvB) (lpMemRes,
                                   uRes,
                                  &atLft,
                                  &atRht,
                                   lpPrimSpec);
////////////// Free the old atLft & atRht (if any)
////////////(*aTypeFree[ARRAY_BOOL]) (&atLft, 0);
////////////(*aTypeFree[ARRAY_BOOL]) (&atRht, 0);
        } // End FOR
    } else
    {
        // Initialize left & right data ptrs
        lpSymGlbLft = lpMemLft;
        lpSymGlbRht = lpMemRht;

        // Calculate increments depending upon singleton status
        // In each case, the increment is 0 (singleton) or 1 (not singleton)
        uLftInc = !IsSingleton (aplNELMLft);
        uRhtInc = !IsSingleton (aplNELMRht);

        // If the axis is significant, ...
        if (lpMemAxisHead NE NULL)
        {
            // Calculate space needed for the weighting vector
            ByteRes = aplRankRes * sizeof (APLUINT);

            // Check for overflow
            if (ByteRes NE (APLU3264) ByteRes)
                goto WSFULL_EXIT;

            //***************************************************************
            // Allocate space for the weighting vector which is
            //
            //   {times}{backscan}1{drop}({rho}R)[Cx,Ax],1
            //
            //   where Ax contains the specified axes, and
            //   Cx contains the remaining axes.
            //***************************************************************
            hGlbWVec = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
            if (hGlbWVec EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemWVec = MyGlobalLock000 (hGlbWVec);

            // Loop through the dimensions of the result in reverse
            //   order {backscan} and compute the cumulative product
            //   (weighting vector).
            // Note we use a signed index variable because we're
            //   walking backwards and the test against zero must be
            //   made as a signed variable.
            for (uRes = 1, iRht = aplRankRes - 1; iRht >= 0; iRht--)
            {
                lpMemWVec[iRht] = uRes;
                uRes *= lpMemDimRes[lpMemAxisHead[iRht]];
            } // End FOR

            // Calculate space needed for the odometer array
            ByteRes = aplRankRes * sizeof (APLUINT);

            // Check for overflow
            if (ByteRes NE (APLU3264) ByteRes)
                goto WSFULL_EXIT;

            //***************************************************************
            // Allocate space for the odometer array, one value per dimension
            //   in the right arg, with values initially all zero (thanks to GHND).
            //***************************************************************
            hGlbOdo = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
            if (hGlbOdo EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemOdo = MyGlobalLock000 (hGlbOdo);
        } // End IF

        // Is either arg hetero?
        bHetLft = IsSimpleHet (aplTypeLft);
        bHetRht = IsSimpleHet (aplTypeRht);
RESTART_EXCEPTION:
        __try
        {
            APLSTYPE aplTypeComLft,
                     aplTypeComRht;

            // Loop through the left/right args/result
            for (uRes = uLft = uRht = 0;
                 uRes < (APLNELMSIGN) aplNELMRes;
                 uRes++, uLft += uLftInc, uRht += uRhtInc)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (*lpbCtrlBreak))
                    goto ERROR_EXIT;

                // If the axis is significant, ...
                if (lpMemAxisHead NE NULL)
                    // Calculate the left and right argument indices
                    CalcLftRhtArgIndices (uRes,
                                          aplRankRes,
                                         &uLft,
                                          aplRankLft,
                                         &uRht,
                                          aplRankRht,
                                          bLftIdent,
                                          bRhtIdent,
                                          aplNELMAxis,
                                          lpMemAxisHead,
                                          lpMemOdo,
                                          lpMemWVec,
                                          lpMemDimRes);
                // Get the next values and type
                if (bHetLft)
                {
                    aplTypeHetLft = GetNextHetero2 (lpMemLft, uLft, NULL, NULL, NULL, &lpSymGlbLft);
                    uHetLft = 0;
                } else
                {
                    aplTypeHetLft = aplTypeLft;
                    uHetLft = uLft;
                } // End IF

                // Get the next values and type
                if (bHetRht)
                {
                    aplTypeHetRht = GetNextHetero2 (lpMemRht, uRht, NULL, NULL, NULL, &lpSymGlbRht);
                    uHetRht = 0;
                } else
                {
                    aplTypeHetRht = aplTypeRht;
                    uHetRht = uRht;
                } // End IF

                // Check for NaNs

                // Are the args a NaN?
                bNaNLft = IsArgNaN (aplTypeHetLft, lpMemLft, uLft);
                bNaNRht = IsArgNaN (aplTypeHetRht, lpMemRht, uRht);

                // If either arg is a NaN, ...
                if (bNaNLft || bNaNRht)
                {
                    if (gbAllowNaN)
                    {
                        if (bNaNLft)
                            // Convert the left arg to the common datatype
                            (*aTypeActPromote[aplTypeHetLft][aplTypeRes]) (lpSymGlbLft, uHetLft, (LPALLTYPES) ByteAddr (lpMemRes, uRes * iSizeofRes));
                        else
                            // Convert the right arg to the common datatype
                            (*aTypeActPromote[aplTypeHetRht][aplTypeRes]) (lpSymGlbRht, uHetRht, (LPALLTYPES) ByteAddr (lpMemRes, uRes * iSizeofRes));
                    } else
                    if (bNaNLft)
                        goto LEFT_DOMAIN_EXIT;
                    else
                        goto RIGHT_DOMAIN_EXIT;
                } else
                {
                    // If the argument's dimension and the result's differ,
                    //   promote the arg to the type of the result
                    // This can happen when (say) a VFP blows up to HC2V (e.g., log -2.5v  or  (-4)o-0.5v)
                    if ((TranslateArrayTypeToHCDim (aplTypeLft)
                       < TranslateArrayTypeToHCDim (aplTypeRes))
                     || (TranslateArrayTypeToHCDim (aplTypeRht)
                       < TranslateArrayTypeToHCDim (aplTypeRes)))
                        aplTypeCom = aplTypeRes;
                    else
                        // Get the common storage type between the left & right args
                        aplTypeCom = aTypePromote[aplTypeHetLft][aplTypeHetRht];

                    // Initialize left and right common types
                    aplTypeComLft = aplTypeComRht = aplTypeCom;

                    // Convert the left arg to the common datatype
                    (*aTypeActPromote[aplTypeHetLft][aplTypeComLft]) (lpSymGlbLft, uHetLft, &atLft);

                    // Convert the right arg to the common datatype
                    (*aTypeActPromote[aplTypeHetRht][aplTypeComRht]) (lpSymGlbRht, uHetRht, &atRht);

                    // Get the target function
                    lpPrimFn = TranslateTypesToDydPSFIndex (lpPrimSpec, aplTypeRes, aplTypeCom);

                    __try
                    {
                        __try
                        {
                            // Call the function
                            (*lpPrimFn) (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
                        } __finally
                        {
                            // Free the old atLft & atRht (if any)
                            (*aTypeFree[aplTypeComLft]) (&atLft, 0);
                            (*aTypeFree[aplTypeComRht]) (&atRht, 0);

                            // Zero the memory in case we use it again
                            //   because aplTypeCom changes when looping
                            //   through a HETERO.
                            ZeroMemory (&atLft, sizeof (atLft));
                            ZeroMemory (&atRht, sizeof (atRht));
                        } // End __try/__finally
                    } __except (EXCEPTION_CONTINUE_SEARCH) {}
                } // End IF/ELSE
            } // End FOR
        } __except (CheckException (GetExceptionInformation (), L"PrimFnDydSimpSimp_EM #1"))
        {
            EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code

            dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L" #10: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

            // Split cases based upon the ExceptionCode
            switch (ExceptionCode)
            {
                case EXCEPTION_DOMAIN_ERROR:
                case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                case EXCEPTION_INT_DIVIDE_BY_ZERO:
                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                    goto DOMAIN_EXIT;

                case EXCEPTION_NONCE_ERROR:
                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                    goto NONCE_EXIT;

                case EXCEPTION_RESULT_VFP:
                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                    if (IsNumeric (aplTypeRes)
                     && !IsVfp    (aplTypeRes))
                    {
                        // It's now a VFP result
                        aplTypeRes = ARRAY_VFP;

                        // If the old result is not immediate, ...
                        if (lpMemHdrRes NE NULL)
                        {
                            // We need to start over with the result
                            MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
                            FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;
                        } // End IF

                        if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                         lphGlbRes,
                                                         lpMemHdrLft,
                                                         lpMemHdrRht,
                                                         aplRankLft,
                                                         aplRankRht,
                                                        &aplRankRes,
                                                         aplTypeRes,
                                                         bLftIdent,
                                                         bRhtIdent,
                                                         aplNELMLft,
                                                         aplNELMRht,
                                                         aplNELMRes))
                            goto ERROR_EXIT;

                        // Lock the memory to get a ptr to it
                        lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

                        // If the result is not nested, ...
                        if (!IsNested (lpMemHdrRes->ArrType))
                            // Tell the header about it
                            lpMemHdrRes->ArrType = aplTypeRes;

                        // Skip over the header and dimensions to the data
                        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

                        dprintfWL0 (L"!!Restarting Exception in " APPEND_NAME L" #10: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                        goto RESTART_EXCEPTION;
                    } // End IF

                    // Display message for unhandled exception
                    DisplayException ();

                    break;

                case EXCEPTION_RESULT_FLOAT:
                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                    if (IsNumeric (aplTypeRes))
                    {
                        // If the previous result is Boolean, we need to
                        //   unlock, free, and allocate the result anew
                        if (IsSimpleBool (aplTypeRes))
                        {
                            // It's now a FLOAT result
                            aplTypeRes = ARRAY_FLOAT;

                            // If the old result is not immediate, ...
                            if (lpMemHdrRes NE NULL)
                            {
                                // We need to start over with the result
                                MyGlobalUnlock (*lphGlbRes); lpMemRes = lpMemHdrRes = NULL;
                                FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;
                            } // End IF

                            if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                             lphGlbRes,
                                                             lpMemHdrLft,
                                                             lpMemHdrRht,
                                                             aplRankLft,
                                                             aplRankRht,
                                                            &aplRankRes,
                                                             aplTypeRes,
                                                             bLftIdent,
                                                             bRhtIdent,
                                                             aplNELMLft,
                                                             aplNELMRht,
                                                             aplNELMRes))
                                goto ERROR_EXIT;

                            // Lock the memory to get a ptr to it
                            lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);
                        } else
                        // The previous result must have been INT which is
                        //   the same size as FLOAT, so there's no need to
                        //   change storage.
                            // It's now a FLOAT result
                            aplTypeRes = ARRAY_FLOAT;

                        // If the result is not immediate, ...
                        if (lpMemHdrRes NE NULL)
                        {
                            // If the result is not nested, ...
                            if (!IsNested (lpMemHdrRes->ArrType))
                                // Tell the header about it
                                lpMemHdrRes->ArrType = aplTypeRes;

                            // Skip over the header and dimensions to the data
                            lpMemRes = VarArrayDataFmBase (lpMemHdrRes);
                        } // End IF

                        dprintfWL0 (L"!!Restarting Exception in " APPEND_NAME L" #12: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                        goto RESTART_EXCEPTION;
                    } // End IF

                    // Display message for unhandled exception
                    DisplayException ();

                    break;

                case EXCEPTION_RESULT_HC2F:
                case EXCEPTION_RESULT_HC4F:
                case EXCEPTION_RESULT_HC8F:
                case EXCEPTION_RESULT_HC2V:
                case EXCEPTION_RESULT_HC4V:
                case EXCEPTION_RESULT_HC8V:
                    MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                    if (IsNumeric (aplTypeRes))
                    {
                        // It's now a HCxy result
                        aplTypeRes = TranslateExceptionCodeToArrayType (ExceptionCode);

                        // If the old result is not immediate, ...
                        if (lpMemHdrRes NE NULL)
                        {
                            // We need to start over with the result
                            MyGlobalUnlock (*lphGlbRes); lpMemRes = lpMemHdrRes = NULL;
                            FreeResultGlobalVar (*lphGlbRes); *lphGlbRes = NULL;
                        } // End IF

                        if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                                         lphGlbRes,
                                                         lpMemHdrLft,
                                                         lpMemHdrRht,
                                                         aplRankLft,
                                                         aplRankRht,
                                                        &aplRankRes,
                                                         aplTypeRes,
                                                         bLftIdent,
                                                         bRhtIdent,
                                                         aplNELMLft,
                                                         aplNELMRht,
                                                         aplNELMRes))
                            goto ERROR_EXIT;

                        // Lock the memory to get a ptr to it
                        lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

                        // If the result is not nested, ...
                        if (!IsNested (lpMemHdrRes->ArrType))
                            // Tell the header about it
                            lpMemHdrRes->ArrType = aplTypeRes;

                        // Skip over the header and dimensions to the data
                        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

                        dprintfWL0 (L"!!Restarting Exception in " APPEND_NAME L" #12: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

                        goto RESTART_EXCEPTION;
                    } // End IF

                    // Display message for unhandled exception
                    DisplayException ();

                    break;

////////////////case EXCEPTION_RESULT_RAT:
                default:
                    // Display message for unhandled exception
                    DisplayException ();

                    break;
            } // End SWITCH
        } // End __try/__except
    } // End IF/ELSE

    // If the result is a global, ...
    if (*lphGlbRes NE NULL)
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = TranslateArrayTypeToImmType (aplTypeRes);
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
////////lpYYRes->tkToken.tkData.tkLongest  =                // Already filled in by call to (*lpPrimFn)
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE

    // Unlock the result global memory in case TypeDemote actually demotes
    if (*lphGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken, lpPrimSpec->bDydDimDemote);

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as in error
    bRet = FALSE;

    if (*lphGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (*lphGlbRes); *lphGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
////// We no longer need this storage
////Myq_clear (&aplRatRht);
////Myq_clear (&aplRatLft);
////Myf_clear (&aplVfpRht);
////Myf_clear (&aplVfpLft);
////
    if (*lphGlbRes NE NULL && lpMemRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbWVec, lpMemWVec);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbOdo, lpMemOdo);

    return bRet;
} // End PrimFnDydSimpSimp_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CalcLftRhtArgIndices
//
//  Calculate the left and right argument indices
//***************************************************************************

void CalcLftRhtArgIndices
    (APLINT    uRes,
     APLRANK   aplRankRes,
     LPAPLINT  lpuLft,
     APLRANK   aplRankLft,
     LPAPLINT  lpuRht,
     APLRANK   aplRankRht,
     UBOOL     bLftIdent,           // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL     bRhtIdent,           // ...                         right ...
     APLNELM   aplNELMAxis,
     LPAPLUINT lpMemAxisHead,       // Ptr to axis values, fleshed out by CheckAxis_EM
     LPAPLUINT lpMemOdo,
     LPAPLUINT lpMemWVec,
     LPAPLDIM  lpMemDimRes)

{
    APLINT uArg, uInd;

    // Loop through the odometer values accumulating
    //   the weighted sum
    // Note that the contents of lpMemOdo change each time this function is called
    for (uArg = 0, uInd = aplRankRes - aplNELMAxis; uInd < (APLRANKSIGN) aplRankRes; uInd++)
        uArg += lpMemOdo[lpMemAxisHead[uInd]] * lpMemWVec[uInd];

    // Increment the odometer in lpMemOdo subject to
    //   the values in lpMemDimRes
    IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

    // Use the just computed index for the argument
    //   with the smaller rank
    if (aplRankLft > aplRankRht)
    {
        *lpuRht = uArg * !IsScalar (aplRankRht);
        *lpuLft = uRes;
    } else
    if (aplRankLft < aplRankRht)
    {
        *lpuLft = uArg * !IsScalar (aplRankLft);
        *lpuRht = uRes;
    } else
    {
        // If there's right identity element only, ...
        if (!bLftIdent && bRhtIdent)
        {
            *lpuRht = uArg * !IsScalar (aplRankRht);
            *lpuLft = uRes;
        } else
        {
            *lpuLft = uArg * !IsScalar (aplRankLft);
            *lpuRht = uRes;
        } // End IF/ELSE
    } // End IF/ELSE
} // End CalcLftRhtArgIndices


//***************************************************************************
//  $IsTknBooleanAPA
//
//  Return TRUE iff the token is a Boolean APA
//***************************************************************************

UBOOL IsTknBooleanAPA
    (LPTOKEN lptkArg)                       // Ptr to token arg

{
    UBOOL             bRet = FALSE;         // TRUE iff the result is valid
    APLSTYPE          aplTypeArg;           // Arg storage type
    APLRANK           aplRankArg;           // Arg rank
    HGLOBAL           hGlbArg = NULL;       // Arg global memory handle
    LPVARARRAY_HEADER lpMemHdrArg = NULL;   // Ptr to arg header
    LPAPLAPA          lpMemArg;             // Ptr to arg global memory

    // Get the attributes (Type, NELM, and Rank)
    //   of the arg
    AttrsOfToken (lptkArg, &aplTypeArg, NULL, &aplRankArg, NULL);

    // Check for APA
    if (!IsSimpleAPA (aplTypeArg))
        goto NORMAL_EXIT;

    // Get arg global ptrs
    GetGlbPtrs_LOCK (lptkArg, &hGlbArg, &lpMemHdrArg);

    // Skip over the header to the data
    lpMemArg = VarArrayDataFmBase (lpMemHdrArg);

    // Check for Boolean APA
    bRet = IsBooleanAPA (lpMemArg);
NORMAL_EXIT:
    if (hGlbArg NE NULL && lpMemHdrArg NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;
    } // End IF

    return bRet;
} // End IsTknBooleanAPA


//***************************************************************************
//  $AllocateSpaceGlbNum_LOCK
//
//  Allocate space for a global numeric
//***************************************************************************

UBOOL AllocateSpaceGlbNum_LOCK
    (HGLOBAL           *lphGlbRes,          // Ptr to result global memory handle
     HGLOBAL           *lphGlbTmp,          // ...    temp   ...
     APLSTYPE           aplTypeRes,         // Result storage type
     APLNELM            aplNELMRes,         // ...    NELM
     APLRANK            aplRankRes,         // ...    rank
     LPTOKEN            lptkRes,            // Ptr to result token
     LPVARARRAY_HEADER *lplpMemHdrRes)      // Ptr to ptr to result header

{
    APLUINT           ByteRes;              // # bytes in the result
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    UBOOL             bRet;                 // TRUE iff allocate locally

    // If the result global memory handle is not NULL, ...
    if (lphGlbRes NE NULL && *lphGlbRes NE NULL)
    {
        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

        // Is the global result Nested but the local result is GlbNum
        bRet = IsNested (lpMemHdrRes->ArrType) && IsGlbNum (aplTypeRes);

        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;
    } else
        bRet = FALSE;

    // If the result global memory handle is NULL, ...
    if (bRet || lphGlbRes EQ NULL || *lphGlbRes EQ NULL)
    {
        // Allocate our own global memory and return it in lptkRes

        //***************************************************************
        // Calculate space needed for the result
        //***************************************************************
        ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

        //***************************************************************
        // Check for overflow
        //***************************************************************
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        //***************************************************************
        // Now we can allocate the storage for the result
        //***************************************************************
        *lphGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (lphGlbRes NE NULL)
           *lphGlbRes = *lphGlbTmp;
        if (*lphGlbTmp EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLock000 (*lphGlbTmp);

#define lpHeader        lpMemHdrRes
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = aplTypeRes;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMRes;
        lpHeader->Rank       = aplRankRes;
#undef  lpHeader
    } else
    {
        *lphGlbTmp = *lphGlbRes;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (*lphGlbTmp);

        // In case we promoted the result, ...
        lpMemHdrRes->ArrType = aplTypeRes;
    } // End IF/ELSE

    // Save in the result
    lptkRes->tkData.tkGlbData = MakePtrTypeGlb (*lphGlbTmp);

    // Return the ptr to the result header
    *lplpMemHdrRes = lpMemHdrRes;

    return TRUE;

WSFULL_EXIT:
    return FALSE;
} // End AllocateSpaceGlbNum_LOCK


//***************************************************************************
//  $TranslateResArgToPSFindex
//
//  Translate a result datatype and an argument datatype to
//    a PSF index (in [0, 6]) as per the following table
//
//    0   IisIvI    HCxIisHCxIvHCxI
//    1   IisFvF    HCxIisHCxFvHCxF
//    2   FisIvI    HCxFisHCxIvHCxI
//    3   FisFvF    HCxFisHCxFvHCxF
//    4   RisRvR    HCxRisHCxRvHCxR
//    5   VisRvR    HCxVisHCxRvHCxR
//    6   VisVvV    HCxVisHCxVvHCxV
//***************************************************************************

/// #define TranslateResArgToPSFIndex(aplTypeRes,aplTypeArg)  \
///    ((2 * IsHCFlt (aplTypeRes))                            \
///   +      IsHCFlt (aplTypeArg)                             \
///   +  4 * IsHCRat (aplTypeArg)                             \
///   +  5 * IsHCVfp (aplTypeArg))

UINT TranslateResArgToPSFIndex
    (APLSTYPE aplTypeRes,
     APLSTYPE aplTypeCom)

{
    // Split cases based upon the result datatype
    switch (aplTypeRes)
    {
        case ARRAY_INT:
        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
            Assert (IsHCInt (aplTypeCom)
                 || IsHCFlt (aplTypeCom));
            return 0 + IsHCFlt (aplTypeCom);    // [0, 1]

        case ARRAY_FLOAT:
        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
            Assert (IsHCInt (aplTypeCom)
                 || IsHCFlt (aplTypeCom));
            return 2 + IsHCFlt (aplTypeCom);    // [2, 3]

        case ARRAY_RAT:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            Assert (IsHCRat (aplTypeCom));

            return 4;                           // [4, 4]

        case ARRAY_VFP:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            // If the common type is FLT, ...
            if (IsHCFlt (aplTypeCom))
                return 3;                       // [3]

            Assert (IsHCRat (aplTypeCom)
                 || IsHCVfp (aplTypeCom));
            return 5 + IsHCVfp (aplTypeCom);    // [5, 6]

        case ARRAY_BOOL:
        defstop
            return -1;
    } // End SWITCH
} // End TranslateResArgToPSFIndex


//***************************************************************************
//  $TranslateTypesToMonPSFIndex
//***************************************************************************

LPATISAT TranslateTypesToMonPSFIndex
    (LPPRIMSPEC lpPrimSpec,             // Ptr to local PRIMSPEC
     APLSTYPE   aplTypeRes,             // Result storage type
     APLSTYPE   aplTypeRht)             // Right arg ...

{
    UINT uTmp;                          // Temporary index var

    Assert (!IsSimpleBool (aplTypeRes));

    // If the result is FLT/VFP and the arg is HC, ...
    if ((IsSimpleFlt (aplTypeRes) || IsVfp (aplTypeRes))
     && IsHCAny (aplTypeRht))
    {
        // The index below is into the Table starting at FisHC2I through VisHC8V
        return (&lpPrimSpec->FisHC2I)[aplTypeRht - ARRAY_HC2I];
    } else
    {
        // Get the base value (0, 7, 14, 21)
        uTmp = 7 * aArrayTypeToHCDimIndex[aplTypeRes];

        // Skip to the proper index in "IFIFRRV"
        uTmp += TranslateResArgToPSFIndex (aplTypeRes, aplTypeRht);

        // The index below is into the Table starting at IisI through HC8VisHC8V
        return (&lpPrimSpec->IisI)[uTmp];
    } // End IF/ELSE
} // End TranslateTypesToMonPSFIndex


//***************************************************************************
//  $TranslateTypesToDydPSFIndex
//***************************************************************************

LPATISATVAT TranslateTypesToDydPSFIndex
    (LPPRIMSPEC lpPrimSpec,             // Ptr to local PRIMSPEC
     APLSTYPE   aplTypeRes,             // Result storage type
     APLSTYPE   aplTypeCom)             // Common ...

{
    UINT uTmp;                          // Temporary index var

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        // The index below is into the Table starting at BisBvB through BisHC8VvHC8V
        case ARRAY_BOOL:        // Result is Boolean

            // Split cases based upon the common storage type
            switch (aplTypeCom)
            {
                case ARRAY_BOOL:        // M.B.:  We're assuming that the types in [BOOL, HETERO] are contiguous
                case ARRAY_INT:
                case ARRAY_FLOAT:
                case ARRAY_CHAR:
                case ARRAY_HETERO:
                    // This code handles items BisBvB through BisHvH
                    uTmp =     aplTypeCom - ARRAY_BOOL;

                    break;

                case ARRAY_RAT:         // N.B.:  We're assuming that the types in [RAT, HC8V] are contiguous
                case ARRAY_VFP:
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
                    // This code handles items BisRvR through BisHC8VvHC8V
                    // "5 +" comes from the count of the items skipped over (BisBvB through BisHvH)
                    uTmp = 5 + aplTypeCom - ARRAY_RAT;

                    break;

                defstop
                    break;
            } // End SWITCH

            return (&lpPrimSpec->BisBvB)[uTmp];

        // The indices below are into the Table starting at IisIvI through AT8VisAT8VvAT8V
        case ARRAY_INT:         //   0,  2
        case ARRAY_FLOAT:       //   1,  3
        case ARRAY_RAT:         //   4,  5
        case ARRAY_VFP:         //   6
        case ARRAY_HC2I:        //   7,  9
        case ARRAY_HC2F:        //   8, 10
        case ARRAY_HC2R:        //  11, 12
        case ARRAY_HC2V:        //  13
        case ARRAY_HC4I:        //  14, 16
        case ARRAY_HC4F:        //  15, 17
        case ARRAY_HC4R:        //  18, 19
        case ARRAY_HC4V:        //  20
        case ARRAY_HC8I:        //  21, 23
        case ARRAY_HC8F:        //  22, 23
        case ARRAY_HC8R:        //  25, 26
        case ARRAY_HC8V:        //  27
            // Get the base value (0, 7, 14, 21)
            uTmp = 7 * aArrayTypeToHCDimIndex[aplTypeRes];  // 0, 7, 14, 21

            // Skip to the proper index in "IFIFRRV"
            uTmp += TranslateResArgToPSFIndex (aplTypeRes, aplTypeCom);

            return (&lpPrimSpec->IisIvI)[uTmp];

        defstop
            return NULL;
    } // End SWITCH
} // End TranslateTypesToDydPSFIndex


//***************************************************************************
//  $CheckExCodeHelper
//
//  Check the exception code in a helper function
//***************************************************************************

void CheckExCodeHelper
    (LPEXCEPTION_CODES lpExCode)

{
    // Split cases based upon the ExceptionCode
    switch (*lpExCode)
    {
        case EXCEPTION_SUCCESS:
        case EXCEPTION_DOMAIN_ERROR:

        case EXCEPTION_RESULT_FLOAT:
        case EXCEPTION_RESULT_HC2F:
        case EXCEPTION_RESULT_HC4F:
        case EXCEPTION_RESULT_HC8F:

        case EXCEPTION_RESULT_VFP:
        case EXCEPTION_RESULT_HC2V:
        case EXCEPTION_RESULT_HC4V:
        case EXCEPTION_RESULT_HC8V:
            break;

        default:
            DbgNop ();          // CheckExCodeHelper

            *lpExCode = EXCEPTION_LIMIT_ERROR;

            break;
    } // End SWITCH
} // End CheckExCodeHelper


//***************************************************************************
//  $CheckExCodeMain_RE
//
//  Check the exception code in a main function
//***************************************************************************

void CheckExCodeMain_RE
    (LPEXCEPTION_CODES lpExCode,        // Ptr to exception code to check
     EXCEPTION_CODES   exCode)          // Result exception code

{
    // Split cases based upon the exception code
    switch (*lpExCode)
    {
        case EXCEPTION_SUCCESS:
            break;

        case EXCEPTION_DOMAIN_ERROR:
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            break;

        case EXCEPTION_RESULT_FLOAT:
        case EXCEPTION_RESULT_HC2F:
        case EXCEPTION_RESULT_HC4F:
        case EXCEPTION_RESULT_HC8F:

        case EXCEPTION_RESULT_VFP:
        case EXCEPTION_RESULT_HC2V:
        case EXCEPTION_RESULT_HC4V:
        case EXCEPTION_RESULT_HC8V:
            RaiseException (exCode, 0, 0, NULL);

            break;

        default:
            DbgNop ();              // CheckExCodeMain_RE

            RaiseException (EXCEPTION_LIMIT_ERROR, 0, 0, NULL);

            break;
    } // End SWITCH
} // End CheckExCodeMain_RE


//***************************************************************************
//  End of File: primspec.c
//***************************************************************************
