//***************************************************************************
//  NARS2000 -- Primitive Function -- Comma
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2015 Sudley Place Software

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
//  $PrimFnComma_EM_YY
//
//  Primitive function for monadic and dyadic Comma ("ravel/table" and "catenate/laminate")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnComma_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnComma_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_COMMA
         || lptkFunc->tkData.tkChar EQ UTF16_COMMABAR);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonComma_EM_YY             (lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydComma_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnComma_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnComma_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Comma
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnComma_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnComma_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnComma_EM_YY,  // Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnComma_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimIdentFnComma_EM_YY
//
//  Generate an identity element for the primitive function dyadic Comma
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimIdentFnComma_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimIdentFnComma_EM_YY
    (LPTOKEN lptkRhtOrig,           // Ptr to original right arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    TOKEN        tkLft = tkZero,    // Token for temporary left arg
                 tkFcn = {0},       // Ptr to function token
                 tkAxis = {0};      // Ptr to axis token
    LPPL_YYSTYPE lpYYIdent = NULL;  // Ptr to identity element result
    APLRANK      aplRankRht;        // Right arg rank
    APLBOOL      bQuadIO;           // []IO

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig NE NULL);
    Assert (lptkFunc    NE NULL);
    Assert (lptkRhtArg  NE NULL);

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, NULL, NULL, &aplRankRht, NULL);

    // Check for axis present
    if (lptkAxis)
    {
        // Catentate allows integer axis values only
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRht,      // All values less than this
                           TRUE,            // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // Return TRUE iff fractional values present
                           NULL,            // Return last axis value
                           NULL,            // Return # elements in axis vector
                           NULL))           // Return HGLOBAL with APLINT axis values
            goto ERROR_EXIT;
    } // End IF

    // Disallow scalars as they do not have a catenate identity function
    if (IsScalar (aplRankRht))
        goto DOMAIN_EXIT;

    // The (left/right) identity function for dyadic comma
    //   (L,[X] R) ("catenate") is
    //   0/[X] R.

    // Setup the left arg token
////tkLft.tkFlags.TknType   = TKT_VARIMMED;     // Already set from tkZero
////tkLft.tkFlags.ImmType   = IMMTYPE_BOOL;     // Already set from tkZero
////tkLft.tkFlags.NoDisplay = FALSE;            // Already set from tkZero
////tkLft.tkData.tkLongest  = 0;                // Already set ftom tkZero
    tkLft.tkCharIndex       = lptkFunc->tkCharIndex;

    // Setup the function token
    tkFcn.tkFlags.TknType   = TKT_FCNIMMED;
    tkFcn.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////tkFcn.tkFlags.NoDisplay = FALSE;            // Already zero from = {0}
    tkFcn.tkData.tkChar     = (lptkFunc->tkData.tkChar EQ UTF16_COMMABAR) ? UTF16_SLASHBAR
                                                                          : UTF16_SLASH;
    tkFcn.tkCharIndex       = lptkFunc->tkCharIndex;

    // Compute 0/[aplAxis] R
    lpYYIdent =
      PrimFnDydSlash_EM_YY (&tkLft,             // Ptr to left arg token
                            &tkFcn,             // Ptr to function token
                             lptkRhtArg,        // Ptr to right arg token
                             lptkAxis);         // Ptr to left operand axis token (may be NULL)
    return lpYYIdent;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    return NULL;
} // End PrimIdentFnComma_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonComma_EM_YY
//
//  Primitive function for monadic Comma ("ravel/table")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonComma_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonComma_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE aplTypeRht;            // Right arg storage type
    APLRANK  aplRankRht;            // Right arg rank
    HGLOBAL  hGlbRht;               // Right arg global memory handle

    //***************************************************************
    // Comma-bar is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************

    if (lptkFunc->tkData.tkChar EQ UTF16_COMMABAR
     && lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, NULL, &aplRankRht, NULL);

    // Split cases based upon the right arg's token type
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

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbRht));
            } else
                hGlbRht = NULL;

            // If it's a scalar, ...
            if (IsScalar (aplRankRht))
                // Handle the scalar case
                return PrimFnMonCommaScalar_EM_YY
                       (aplTypeRht,                                     // Array storage type
                        lptkRhtArg->tkData.tkSym->stData.stLongest,     // Immediate value if simple scalar
                        hGlbRht,                                        // Right arg global memory handle (or NULL if simple scalar)
                        lptkAxis,                                       // Ptr to axis token (may be NULL)
                        lptkFunc);                                      // Ptr to function token
            break;          // Join common global code

        case TKT_VARIMMED:
            // Handle the scalar case
            return PrimFnMonCommaScalar_EM_YY
                   (aplTypeRht,                                     // Array storage type
                    lptkRhtArg->tkData.tkLongest,                   // Immediate value if simple scalar
                    NULL,                                           // Right arg global memory handle (or NULL if simple scalar)
                    lptkAxis,                                       // Ptr to axis token (may be NULL)
                    lptkFunc);                                      // Ptr to function token
        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (lptkRhtArg->tkData.tkGlbData));

            // Get the global memory handle
            hGlbRht = lptkRhtArg->tkData.tkGlbData;

            // If it's a scalar, ...
            if (IsScalar (aplRankRht))
                // Handle the scalar case
                return PrimFnMonCommaScalar_EM_YY
                       (aplTypeRht,                                 // Array storage type
                        0,                                          // Immediate value if simple scalar
                        hGlbRht,                                    // Right arg global memory handle (or NULL if simple scalar)
                        lptkAxis,                                   // Ptr to axis token (may be NULL)
                        lptkFunc);                                  // Ptr to function token
            break;          // Join common global code

        defstop
            return NULL;
    } // End SWITCH

    return PrimFnMonCommaGlb_EM_YY (hGlbRht,                    // HGLOBAL
                                    lptkAxis,                   // Ptr to axis token (may be NULL)
                                    lptkFunc);                  // Ptr to function token
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnMonComma_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonCommaScalar_EM_YY
//
//  Monadic Comma ("ravel/table") on a scalar value.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonCommaScalar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonCommaScalar_EM_YY
    (ARRAY_TYPES   aplTypeRht,      // Right arg storage type (see ARRAY_TYPES)
     APLLONGEST    aplLongest,      // Right arg immediate value if simple scalar
     HGLOBAL       hGlbRht,         // Right arg global memory handle (or NULL if simple scalar)
     LPTOKEN       lptkAxis,        // Ptr to axis token (may be NULL)
     LPTOKEN       lptkFunc)        // Ptr to function token

{
    APLRANK           aplRankRes;           // Result rank
    HGLOBAL           hGlbRes;              // Result global memory handle
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPVOID            lpMemRes,             // Ptr to result global memory
                      lpMemRht;             // Ptr to right arg global memory
    UBOOL             bFract = FALSE,       // TRUE iff axis has fractional values
                      bTableRes;            // TRUE iff function is UTF16_COMMABAR
    APLUINT           ByteRes;              // # bytes in the result
    int               i;                    // Loop counter
    LPPL_YYSTYPE      lpYYRes;              // Ptr to the result

    // Check for axis present
    while (lptkAxis NE NULL)
    {
        // Ravel with axis on a simple scalar requires
        //   that the axis be an empty vector
        if (CheckAxis_EM (lptkAxis,         // The axis token
                          0,                // All values less than this
                          FALSE,            // TRUE iff scalar or one-element vector only
                          FALSE,            // TRUE iff want sorted axes
                          TRUE,             // TRUE iff axes must be contiguous
                          FALSE,            // TRUE iff duplicate axes are allowed
                          NULL,             // Return TRUE iff fractional values present
                          NULL,             // Return last axis value
                          NULL,             // Return # elements in axis vector
                          NULL))            // Return HGLOBAL with APLINT axis values
            break;
        //   or a singleton fractional value
        if (CheckAxis_EM (lptkAxis,         // The axis token
                          1,                // All values less than this
                          TRUE,             // TRUE iff scalar or one-element vector only
                          FALSE,            // TRUE iff want sorted axes
                          TRUE,             // TRUE iff axes must be contiguous
                          FALSE,            // TRUE iff duplicate axes are allowed
                         &bFract,           // Return TRUE iff fractional values present
                          NULL,             // Return last axis value
                          NULL,             // Return # elements in axis vector
                          NULL)             // Return HGLOBAL with APLINT axis values
         && bFract)
            break;
        // Otherwise, it's an AXIS ERROR
        return NULL;
    } // End WHILE

    //***************************************************************
    // If the function is Comma-bar (table), the result is
    //   1 1 {rho} R
    //***************************************************************
    bTableRes = (lptkFunc->tkData.tkChar EQ UTF16_COMMABAR);
    aplRankRes = 1 + bTableRes;

    //***************************************************************
    // Calculate space needed for the result
    //***************************************************************
    ByteRes = CalcArraySize (aplTypeRht, 1, aplRankRes);

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
    lpMemHdrRes = MyGlobalLock (hGlbRes);

#define lpHeader        lpMemHdrRes
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRht;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = 1;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Fill in the result's dimension
    *VarArrayBaseToDim (lpMemHdrRes) = 1;
    if (bTableRes)
        *(1 + VarArrayBaseToDim (lpMemHdrRes)) = 1;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // If there's a global memory handle, ...
    if (hGlbRht NE NULL)
    {
        // Lock the memory to get a handle to it
        lpMemHdrRht = MyGlobalLock (hGlbRht);

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    } // End IF

    // Copy the immediate value to the result
    // Split cases based upon the result storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            *((LPAPLBOOL)  lpMemRes) = (APLBOOL)  aplLongest;

            break;

        case ARRAY_INT:
            *((LPAPLINT)   lpMemRes) = (APLINT)   aplLongest;

            break;

        case ARRAY_FLOAT:
            *((LPAPLFLOAT) lpMemRes) = *(LPAPLFLOAT) &aplLongest;

            break;

        case ARRAY_CHAR:
            *((LPAPLCHAR)  lpMemRes) = (APLCHAR)  aplLongest;

            break;

        case ARRAY_NESTED:
            *((LPAPLNESTED) lpMemRes) = CopySymGlbInd_PTB (lpMemRht);

            break;

        case ARRAY_RAT:
            mpq_init_set  ((LPAPLRAT) lpMemRes, (LPAPLRAT) lpMemRht);

            break;

        case ARRAY_VFP:
            mpfr_init_copy ((LPAPLVFP) lpMemRes, (LPAPLVFP) lpMemRht);

            break;

        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
            // Copy to the result
            CopyMemory (lpMemRes, lpMemRht, TranslateArrayTypeToSizeof (aplTypeRht));

            break;

        case ARRAY_HC2R:
            // Loop through all of the parts
            for (i = 0; i < 2; i++)
                // Copy to the result
                mpq_init_set (&((LPAPLHC2R) lpMemRes)->parts[i], &((LPAPLHC2R) lpMemRht)->parts[i]);
            break;

        case ARRAY_HC4R:
            // Loop through all of the parts
            for (i = 0; i < 4; i++)
                // Copy to the result
                mpq_init_set (&((LPAPLHC4R) lpMemRes)->parts[i], &((LPAPLHC4R) lpMemRht)->parts[i]);
            break;

        case ARRAY_HC8R:
            // Loop through all of the parts
            for (i = 0; i < 8; i++)
                // Copy to the result
                mpq_init_set (&((LPAPLHC8R) lpMemRes)->parts[i], &((LPAPLHC8R) lpMemRht)->parts[i]);
            break;

        case ARRAY_HC2V:
            // Loop through all of the parts
            for (i = 0; i < 2; i++)
                // Copy to the result
                mpfr_init_set (&((LPAPLHC2V) lpMemRes)->parts[i], &((LPAPLHC2V) lpMemRht)->parts[i], MPFR_RNDN);
            break;

        case ARRAY_HC4V:
            // Loop through all of the parts
            for (i = 0; i < 4; i++)
                // Copy to the result
                mpfr_init_set (&((LPAPLHC4V) lpMemRes)->parts[i], &((LPAPLHC4V) lpMemRht)->parts[i], MPFR_RNDN);
            break;

        case ARRAY_HC8V:
            // Loop through all of the parts
            for (i = 0; i < 8; i++)
                // Copy to the result
                mpfr_init_set (&((LPAPLHC8V) lpMemRes)->parts[i], &((LPAPLHC8V) lpMemRht)->parts[i], MPFR_RNDN);
            break;

        defstop
            break;
    } // End SWITCH

    // If there's a global memory handle, ...
    if (hGlbRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnMonCommaScalar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonCommaGlb_EM_YY
//
//  Monadic Comma ("ravel/table") on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonCommaGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonCommaGlb_EM_YY
    (HGLOBAL       hGlbRht,             // Handle to right arg
     LPTOKEN       lptkAxis,            // Ptr to axis token (may be NULL)
     LPTOKEN       lptkFunc)            // Ptr to function token

{
    HGLOBAL           hGlbRes = NULL,       // Result global memory handle
                      hGlbAxis = NULL,      // Axis ...
                      hGlbOdo = NULL,       // Odometer ...
                      hGlbWVec = NULL;      // Weighting vector ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPVOID            lpMemRht,             // Ptr to right arg global memory
                      lpMemRes;             // Ptr to result    ...
    LPAPLDIM          lpMemDimRht = NULL;   // Ptr to right arg dimensions
    LPAPLUINT         lpMemAxisHead = NULL, // Ptr to axis values, fleshed out by CheckAxis_EM
                      lpMemAxisTail = NULL, // Ptr to grade up of AxisHead
                      lpMemOdo = NULL,      // Ptr to odometer ...
                      lpMemWVec = NULL;     // Ptr to weighting vector ...
    APLUINT           ByteRes;              // # bytes in the result
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMAxis;          // Axis      ...
    APLRANK           aplRankRht,           // Right arg rank
                      aplRankRes;           // Result    ...
    APLNELM           uRht,                 // Right arg loop counter
                      uRes,                 // Result    ...
                      uOdo;                 // Odometer  ...
    int               i;                    // Loop counter
    APLDIM            aplDimNew;            //
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeRes;           // Result    ...
    APLNELMSIGN       iRht;                 // Right arg loop counter
    APLINT            apaOffRht,            // Right arg APA offset
                      apaMulRht;            // ...           multiplier
    APLUINT           aplFirstAxis,         // First axis value (if contiguous, then lowest)
                      aplLastAxis;          // Last ...                              highest
    UBOOL             bFract = FALSE,       // TRUE iff axis has fractional values
                      bTableRes,            // TRUE iff function is UTF16_COMMABAR
                      bReorder = FALSE;     // TRUE iff result values are reordered
                                            //   from those in the right arg
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the rank of the right arg
    aplRankRht = RankOfGlb (hGlbRht);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        while (TRUE)
        {
            // Ravel/table with axis requires
            //   that the axis be a scalar or vector subset of
            //   {iota}aplRankRht
            if (CheckAxis_EM (lptkAxis,         // The axis token
                              aplRankRht,       // All values less than this
                              FALSE,            // TRUE iff scalar or one-element vector only
                              FALSE,            // TRUE iff want sorted axes
                              TRUE,             // TRUE iff axes must be contiguous
                              FALSE,            // TRUE iff duplicate axes are allowed
                              NULL,             // Return TRUE iff fractional values present
                             &aplLastAxis,      // Return last axis value
                             &aplNELMAxis,      // Return # elements in axis vector
                             &hGlbAxis))        // Return HGLOBAL with APLINT axis values
                break;
            //   or a singleton fractional value
            if (CheckAxis_EM (lptkAxis,         // The axis token
                              aplRankRht + 1,   // All values less than this
                              TRUE,             // TRUE iff scalar or one-element vector only
                              FALSE,            // TRUE iff want sorted axes
                              TRUE,             // TRUE iff axes must be contiguous
                              FALSE,            // TRUE iff duplicate axes are allowed
                             &bFract,           // Return TRUE iff fractional values present
                             &aplLastAxis,      // Return last axis value
                             &aplNELMAxis,      // Return # elements in axis vector
                             &hGlbAxis)         // Return HGLOBAL with APLINT axis values
             && bFract)
                break;
            // Otherwise, it's an AXIS ERROR
            return NULL;
        } // End WHILE
    } else
    {
        // No axis means ravel all dimensions or table all but the first
        aplNELMAxis = aplRankRht;
        aplLastAxis = aplRankRht - 1;
    } // End IF/ELSE

    //***************************************************************
    // If the function is Comma-bar (table), the result is a
    //   matrix equivalent to
    //     ,[1{drop}{iota}{rho}{rho}R] R
    //***************************************************************
    bTableRes = (lptkFunc->tkData.tkChar EQ UTF16_COMMABAR);
    if (bTableRes)
    {
        // If the right arg is already a matrix, ...
        // Note that the axis operator does not apply to the table function
        //   so there can be no reordering.
        if (IsMatrix (aplRankRht))
        {
            // Copy the right arg
            hGlbRes = CopySymGlbDir_PTB (hGlbRht);

            goto YYALLOC_EXIT;
        } // End IF

        aplLastAxis = aplRankRht - 1;
        aplNELMAxis = aplRankRht - 1;
    } else
    // Empty axis means insert new last unit coordinate
    if (IsEmpty (aplNELMAxis))
        aplLastAxis = aplRankRht;

    // Set the value of the first axis
    // Note that for contiguous axes, <aplLastAxis> is
    //   the highest axis value, and <aplFirstAxis> is
    //   the lowest axis value, inclusive.
    // For example, for ,[1 3 2] (in origin-0)
    //   aplFirstAxis is 1, and
    //   aplLastAxis  is 3
    aplFirstAxis = 1 + aplLastAxis - aplNELMAxis;

    //***************************************************************
    // hGlbAxis is not-NULL iff there is an axis
    // aplNELMAxis contains the # elements in the axis.
    //***************************************************************

    // Calculate the rank of the result
    if (bFract)
        aplRankRes = aplRankRht + 1;
    else
        aplRankRes = aplRankRht + 1 - aplNELMAxis;

    //***************************************************************
    // Optimize ravel of a vector
    //***************************************************************

    if (IsVector (aplRankRht)       // If the right arg is a vector
     && IsVector (aplRankRes))      //   and the result is a vector, ...
    {
        // Increment the right arg reference count
        hGlbRes = CopySymGlbDirAsGlb (hGlbRht);

        goto YYALLOC_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLock (hGlbRht);

#define lpHeader    lpMemHdrRht
    // Get the Array Type and NELM
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // Check to see if there is reordering
    if (hGlbAxis NE NULL)
    {
        // Lock the memory to get a ptr to it
        lpMemAxisHead = MyGlobalLock (hGlbAxis);

        // Point to the grade-up of the first
        //   <aplRankRht> values in lpMemAxis
        lpMemAxisTail = &lpMemAxisHead[aplRankRht];

        // Get the first axis element + 1
        uRht = lpMemAxisHead[aplRankRht - aplNELMAxis] + 1;

        for (uRes = aplRankRht + 1 - aplNELMAxis; (!bReorder) && uRes < aplRankRht; uRes++, uRht++)
            bReorder = (uRht NE lpMemAxisHead[uRes]);
    } // End IF

    //***************************************************************
    // The NELM of the result is the same as that of the right arg
    //   as we're only re-arranging the right arg.
    //***************************************************************

    //***************************************************************
    // The three cases to consider are as follows:
    //  1.  X is fractional
    //      Insert a unit coordinate to the left of aplLastAxis.
    //  2.  X is empty
    //      Append a unit coordinate as the new last axis.
    //  3.  X is a scalar or vector of integers
    //      Transpose R according to X, and collapse the
    //      X coordinates to a single coordinate.
    //***************************************************************

    // If we're reordering and the right arg is ARRAY_APA,
    //   calculate the array size based upon APLINTs
    if (bReorder && IsSimpleAPA (aplTypeRht))
        // Set the array storage type for the result
        aplTypeRes = ARRAY_INT;
    else
        // Set the array storage type for the result
        aplTypeRes = aplTypeRht;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Now we can allocate the storage for the result.
    //***************************************************************
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock (hGlbRes);

#define lpHeader        lpMemHdrRes
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->bSelSpec   = lpMemHdrRht->bSelSpec;
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Calculate the length of the raveled dimension
    if (bFract)
        aplDimNew = 1;
    else
    for (uRes = aplFirstAxis, aplDimNew = 1; uRes <= aplLastAxis; uRes++)
        aplDimNew *= lpMemDimRht[uRes];

    // Point to the result's dimension
    lpMemRes = VarArrayBaseToDim (lpMemHdrRes);

    //***************************************************************
    // Copy the appropriate dimensions from the right arg
    //   to the result's dimension
    //***************************************************************
    if (bTableRes)
    {
        // Save the dimensions
        ((LPAPLDIM) lpMemRes)[0] = IsScalar (aplRankRht) ? 1 : lpMemDimRht[0];
        ((LPAPLDIM) lpMemRes)[1] = aplDimNew;
    } else
    // If we're collapsing dimensions, ...
    if (aplRankRes < aplRankRht)
    {
        for (uRes = uRht = 0; uRht < aplRankRht; uRht++)
        {
            // If it's not an axis dimension, ...
            if (uRht < aplFirstAxis
             || uRht > aplLastAxis)
                ((LPAPLDIM) lpMemRes)[uRes++] = lpMemDimRht[uRht];
            else
            // If, it's the last (highest) axis dimension, ...
            if (uRht EQ aplLastAxis)
                ((LPAPLDIM) lpMemRes)[uRes++] = aplDimNew;
        } // End FOR

        // Otherwise, it's the last (highest) axis dimension, ...
        if (uRht EQ aplLastAxis)
            ((LPAPLDIM) lpMemRes)[uRes++] = aplDimNew;
    } else
    // If we're inserting a unit coordinate, ...
    if (aplRankRes > aplRankRht)
    {
        // Insert the unit coordinate
        ((LPAPLDIM) lpMemRes)[aplLastAxis] = 1;

        for (uRes = uRht = 0; uRht < aplRankRht; uRes++)
        {
            // If it's not the last axis value, ...
            if (uRes NE aplLastAxis)
                ((LPAPLDIM) lpMemRes)[uRes] = lpMemDimRht[uRht++];
        } // End FOR
    } else
    {
        for (uRes = uRht = 0; uRht < aplRankRht; uRes++)
            ((LPAPLDIM) lpMemRes)[uRes] = lpMemDimRht[uRht++];
    } // End IF/ELSE

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // As a shortcut, if there's no reordering we can copy the
    //   data from the right arg to the result
    if (!bReorder)
    {
        APLNELM aplNELM;

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
                // Account for the header and dimensions
                ByteRes -= sizeof (VARARRAY_HEADER)
                         + sizeof (APLDIM) * aplRankRes;
                CopyMemory (lpMemRes, lpMemRht, (APLU3264) ByteRes);

                break;

            case ARRAY_NESTED:
                // We're about to copy the entries from the right arg
                //   into the result.  If the right arg is ARRAY_NESTED,
                //   we need to increment each HGLOBAL's reference count.

                // In case the right arg is empty, include its prototype
                aplNELM = max (aplNELMRht, 1);

                // Loop through the right arg
                for (uRht = 0; uRht < aplNELM; uRht++)
                    // Increment the refcnt
                    DbgIncrRefCntDir_PTB (((LPAPLNESTED) lpMemRht)[uRht]);  // EXAMPLE:  ,NestedMarix

                // Account for the header and dimensions
                ByteRes -= sizeof (VARARRAY_HEADER)
                         + sizeof (APLDIM) * aplRankRes;
                CopyMemory (lpMemRes, lpMemRht, (APLU3264) ByteRes);

                break;

            case ARRAY_HETERO:
                // Make a copy of each RAT/VFP item
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                // Split cases based upon the item's storage type
                switch (GetPtrTypeDir (((LPAPLHETERO) lpMemRht)[uRes]))
                {
                    case PTRTYPE_STCONST:
                        // Copy the ptr
                        ((LPAPLHETERO *) lpMemRes)[uRes] = ((LPAPLHETERO *) lpMemRht)[uRes];

                        break;

                    case PTRTYPE_HGLOBAL:
                        // Increment the reference count
                        DbgIncrRefCntDir_PTB (((LPAPLNESTED) lpMemRht)[uRes]);  // EXAMPLE:  ,1 2{rho}'a' 1x

                        // Copy the ptr
                        ((LPAPLHETERO *) lpMemRes)[uRes] = ((LPAPLHETERO *) lpMemRht)[uRes];

                        break;

                    defstop
                        break;
                } // End FOR/SWITCH

                break;

            case ARRAY_RAT:
                // Make a copy of each RAT item
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                    mpq_init_set (&((LPAPLRAT) lpMemRes)[uRes], &((LPAPLRAT) lpMemRht)[uRes]);
                break;

            case ARRAY_VFP:
                // Make a copy of each RAT item
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                    mpfr_init_copy (&((LPAPLVFP) lpMemRes)[uRes], &((LPAPLVFP) lpMemRht)[uRes]);
                break;

            case ARRAY_HC2R:
                // Make a copy of each RAT item
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                for (i = 0; i < 2; i++)
                    mpq_init_set (&((LPAPLHC2R) lpMemRes)[uRes].parts[i], &((LPAPLHC2R) lpMemRht)[uRes].parts[i]);
                break;

            case ARRAY_HC2V:
                // Make a copy of each RAT item
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                for (i = 0; i < 2; i++)
                    mpfr_init_copy (&((LPAPLHC2V) lpMemRes)[uRes].parts[i], &((LPAPLHC2V) lpMemRht)[uRes].parts[i]);
                break;

            case ARRAY_HC4R:
                // Make a copy of each RAT item
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                for (i = 0; i < 4; i++)
                    mpq_init_set (&((LPAPLHC4R) lpMemRes)[uRes].parts[i], &((LPAPLHC4R) lpMemRht)[uRes].parts[i]);
                break;

            case ARRAY_HC4V:
                // Make a copy of each RAT item
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                for (i = 0; i < 4; i++)
                    mpfr_init_copy (&((LPAPLHC4V) lpMemRes)[uRes].parts[i], &((LPAPLHC4V) lpMemRht)[uRes].parts[i]);
                break;

            case ARRAY_HC8R:
                // Make a copy of each RAT item
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                for (i = 0; i < 8; i++)
                    mpq_init_set (&((LPAPLHC8R) lpMemRes)[uRes].parts[i], &((LPAPLHC8R) lpMemRht)[uRes].parts[i]);
                break;

            case ARRAY_HC8V:
                // Make a copy of each RAT item
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                for (i = 0; i < 8; i++)
                    mpfr_init_copy (&((LPAPLHC8V) lpMemRes)[uRes].parts[i], &((LPAPLHC8V) lpMemRht)[uRes].parts[i]);
                break;

            defstop
                break;
        } // End SWITCH
    } else
    // Reorder the right arg into the result
    {
        // If the right arg is an APA, ...
        if (IsSimpleAPA (aplTypeRht))
        {
#define lpAPA       ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOffRht = lpAPA->Off;
            apaMulRht = lpAPA->Mul;
#undef  lpAPA
        } // End IF

        // Calculate space needed for the result
        ByteRes = aplRankRht * sizeof (APLUINT);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        //***************************************************************
        // Allocate space for the weighting vector which is
        //   {times}{backscan}1{drop}({rho}R),1
        //***************************************************************
        hGlbWVec = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (hGlbWVec EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemWVec = MyGlobalLock (hGlbWVec);

        // Loop through the dimensions of the right arg in reverse
        //   order {backscan} and compute the cumulative product
        //   (weighting vector).
        // Note we use a signed index variable because we're
        //   walking backwards and the test against zero must be
        //   made as a signed variable.
        for (uRes = 1, iRht = aplRankRht - 1; iRht >= 0; iRht--)
        {
            lpMemWVec[iRht] = uRes;
            uRes *= lpMemDimRht[iRht];
        } // End FOR

        // Calculate space needed for the result
        ByteRes = aplRankRht * sizeof (APLUINT);

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
        lpMemOdo = MyGlobalLock (hGlbOdo);

        // Split cases based upon the storage type of the right arg/result
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    UINT uBitMask;

                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemAxisTail[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxisHead]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxisHead, aplRankRht);

                    uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) uRht);

                    // Copy element # uRht from the right arg to lpMemRes[uRes]
                    ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |=
                      ((uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? TRUE : FALSE) << (MASKLOG2NBIB & (UINT) uRes);
                } // End FOR

                break;

            case ARRAY_CHAR:
            case ARRAY_INT:
            case ARRAY_APA:
            case ARRAY_HETERO:
            case ARRAY_NESTED:
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
                for (uRes = 0; uRes < aplNELMRht; uRes++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemAxisTail[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxishead]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxisHead, aplRankRht);

                    // Split cases based upon the right arg storage type
                    switch (aplTypeRht)
                    {
                        case ARRAY_CHAR:
                            // Copy element # uRht from the right arg to lpMemRes[uRes]
                            ((LPAPLCHAR) lpMemRes)[uRes] = ((LPAPLCHAR) lpMemRht)[uRht];

                            break;

                        case ARRAY_INT:
                            // Copy element # uRht from the right arg to lpMemRes[uRes]
                            ((LPAPLINT) lpMemRes)[uRes] = ((LPAPLINT) lpMemRht)[uRht];

                            break;

                        case ARRAY_APA:
                            // Copy element # uRht from the right arg to lpMemRes[uRes]
                            ((LPAPLINT) lpMemRes)[uRes] = apaOffRht + apaMulRht * uRht;

                            break;

                        case ARRAY_HETERO:
                            // Copy element # uRht from the right arg to lpMemRes[uRes]
                            // Note that APLHETERO elements are LPSYMENTRYs, so there's no
                            //   reference count to increment, or other special handling.
                            ((LPAPLHETERO) lpMemRes)[uRes] = ((LPAPLHETERO) lpMemRht)[uRht];

                            break;

                        case ARRAY_NESTED:
                            // Copy element # uRht from the right arg to lpMemRes[uRes]
                            // Note that APLNESTED elements are a mixture of LPSYMENTRYs
                            //   and HGLOBALs, so we need to run the HGLOBALs through
                            //   CopySymGlbDir_PTB so as to increment the reference count.
                            ((LPAPLNESTED) lpMemRes)[uRes] = CopySymGlbDir_PTB (((LPAPLNESTED) lpMemRht)[uRht]);

                            break;

                        case ARRAY_FLOAT:
                            // Copy element # uRht from the right arg to lpMemRes[uRes]
                            ((LPAPLFLOAT) lpMemRes)[uRes] = ((LPAPLFLOAT) lpMemRht)[uRht];

                            break;

                        case ARRAY_HC2I:
                            // Copy element # uRht from the right arg to lpMemRes[uRes]
                            ((LPAPLHC2I) lpMemRes)[uRes] = ((LPAPLHC2I) lpMemRht)[uRht];

                            break;

                        case ARRAY_HC4I:
                            // Copy element # uRht from the right arg to lpMemRes[uRes]
                            ((LPAPLHC4I) lpMemRes)[uRes] = ((LPAPLHC4I) lpMemRht)[uRht];

                            break;

                        case ARRAY_HC8I:
                            // Copy element # uRht from the right arg to lpMemRes[uRes]
                            ((LPAPLHC8I) lpMemRes)[uRes] = ((LPAPLHC8I) lpMemRht)[uRht];

                            break;

                        case ARRAY_HC2F:
                            // Copy element # uRht from the right arg to lpMemRes[uRes]
                            ((LPAPLHC2F) lpMemRes)[uRes] = ((LPAPLHC2F) lpMemRht)[uRht];

                            break;

                        case ARRAY_HC4F:
                            // Copy element # uRht from the right arg to lpMemRes[uRes]
                            ((LPAPLHC4F) lpMemRes)[uRes] = ((LPAPLHC4F) lpMemRht)[uRht];

                            break;

                        case ARRAY_HC8F:
                            // Copy element # uRht from the right arg to lpMemRes[uRes]
                            ((LPAPLHC8F) lpMemRes)[uRes] = ((LPAPLHC8F) lpMemRht)[uRht];

                            break;

                        case ARRAY_RAT:
                            // Copy element # uRht from the right arg to lpMemRes[uRes]
                            mpq_init_set (&((LPAPLRAT) lpMemRes)[uRes], &((LPAPLRAT) lpMemRht)[uRht]);

                            break;

                        case ARRAY_HC2R:
                            // Loop through all of the parts
                            for (i = 0; i < 2; i++)
                                // Copy element # uRht from the right arg to lpMemRes[uRes]
                                mpq_init_set (&((LPAPLHC2R) lpMemRes)[uRes].parts[i], &((LPAPLHC2R) lpMemRht)[uRht].parts[i]);

                            break;

                        case ARRAY_HC4R:
                            // Loop through all of the parts
                            for (i = 0; i < 4; i++)
                                // Copy element # uRht from the right arg to lpMemRes[uRes]
                                mpq_init_set (&((LPAPLHC4R) lpMemRes)[uRes].parts[i], &((LPAPLHC4R) lpMemRht)[uRht].parts[i]);

                            break;

                        case ARRAY_HC8R:
                            // Loop through all of the parts
                            for (i = 0; i < 8; i++)
                                // Copy element # uRht from the right arg to lpMemRes[uRes]
                                mpq_init_set (&((LPAPLHC8R) lpMemRes)[uRes].parts[i], &((LPAPLHC8R) lpMemRht)[uRht].parts[i]);

                            break;

                        case ARRAY_VFP:
                            // Copy element # uRht from the right arg to lpMemRes[uRes]
                            mpfr_init_copy (&((LPAPLVFP) lpMemRes)[uRes], &((LPAPLVFP) lpMemRht)[uRht]);

                            break;

                        case ARRAY_HC2V:
                            // Loop through all of the parts
                            for (i = 0; i < 2; i++)
                                // Copy element # uRht from the right arg to lpMemRes[uRes]
                                mpfr_init_copy (&((LPAPLHC2V) lpMemRes)[uRes].parts[i], &((LPAPLHC2V) lpMemRht)[uRht].parts[i]);

                            break;

                        case ARRAY_HC4V:
                            // Loop through all of the parts
                            for (i = 0; i < 4; i++)
                                // Copy element # uRht from the right arg to lpMemRes[uRes]
                                mpfr_init_copy (&((LPAPLHC4V) lpMemRes)[uRes].parts[i], &((LPAPLHC4V) lpMemRht)[uRht].parts[i]);

                            break;

                        case ARRAY_HC8V:
                            // Loop through all of the parts
                            for (i = 0; i < 8; i++)
                                // Copy element # uRht from the right arg to lpMemRes[uRes]
                                mpfr_init_copy (&((LPAPLHC8V) lpMemRes)[uRes].parts[i], &((LPAPLHC8V) lpMemRht)[uRht].parts[i]);

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End FOR

                break;

            defstop
                break;
        } // End SWITCH
    } // End IF/ELSE
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
        MyGlobalUnlock (hGlbRht);  lpMemHdrRht  = NULL;
    } // End IF

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbWVec, lpMemWVec);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbOdo, lpMemOdo);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbAxis, lpMemAxisHead);

    return lpYYRes;
} // End PrimFnMonCommaGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydComma_EM_YY
//
//  Primitive function for dyadic Comma ("catenate/laminate")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydComma_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydComma_EM_YY
    (LPTOKEN       lptkLftArg,      // Ptr to left arg token
     LPTOKEN       lptkFunc,        // Ptr to function token
     LPTOKEN       lptkRhtArg,      // Ptr to right arg token
     LPTOKEN       lptkAxis)        // Ptr to axis token (may be NULL)

{
    APLUINT           aplAxis,              // The (one and only) axis value
                      uLenLft,              // # bytes in sizeof left arg
                      uLenRht,              // ...               right ...
                      uLft,                 // Loop counter
                      uRht,                 // ...
                      uBeg,                 // ...
                      uRes,                 // ...
                      uEnd,                 // ...
                      uEndLft,              // ...
                      uEndRht,              // ...
                      ByteRes;              // # bytes in the result
    APLRANK           aplRankLft,           // The rank of the left arg
                      aplRankRht,           // ...             right ...
                      aplRankRes,           // ...             result
                      aplRankTmp;           // Temporary rank
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht,           // Right ...
                      aplTypeRes;           // Result   ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht,           // Right ...
                      aplNELMRes;           // Result   ...
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL,       // Right ...
                      hGlbRes = NULL,       // Result   ...
                      hGlbTmp;              // Temporary ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg header
                      lpMemHdrRht = NULL,   // ...    right ...
                      lpMemHdrRes = NULL;   // ...    result   ...
    LPVOID            lpMemLft,             // Ptr to left arg global memory
                      lpMemRht,             // ...    right ...
                      lpMemRes,             // ...    result   ...
                      lpSymGlbLft,          // ...    left arg LPSYMENTRY/HGLOBAL
                      lpSymGlbRht;          // ...    right ...
    LPAPLDIM          lpMemDimLft,          // Ptr to left arg dimensions
                      lpMemDimRht,          // ...    right ...
                      lpMemDimRes,          // ...    result   ...
                      lpMemDimDir;          // ...
    APLDIM            aplDimTmp,
                      aplDimBeg,
                      aplDimLftEnd,
                      aplDimRhtEnd,
                      aplDim1 = 1;
    UBOOL             bFract = FALSE;       // TRUE iff the axis value is fractional (laminate)
    UINT              uBitMaskLft,          // Left arg bit mask for trundling through Booleans
                      uBitMaskRht,          // Right ...
                      uBitIndexRes;         // Result   ...
    APLINT            aplIntegerLft,        // Left arg temporary integer value
                      aplIntegerRht,        // Right ...
                      apaOffLft,            // Left arg APA offset
                      apaOffRht,            // Right ...
                      apaMulLft,            // Left arg APA multiplier
                      apaMulRht;            // Right ...
    APLLONGEST        aplLongestLft,        // Ptr to left immediate value
                      aplLongestRht;        // ...    right ...
    APLFLOAT          aplFloatLft,          // Left arg float temporary value
                      aplFloatRht;          // Right ...
    APLCHAR           aplCharLft,           // Left arg temporary character value
                      aplCharRht;           // Right ...
    APLLONGEST        aplVal;               // Temporary value
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPSYMENTRY        lpSymTmp;             // Ptr to temporary LPSYMENTRY
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    ALLTYPES          atLft = {0},          // Left arg item as ALLTYPES
                      atRht = {0};          // Right ...

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Both arguments are within bounds
    Assert (aplTypeLft < ARRAY_LENGTH);
    Assert (aplTypeRht < ARRAY_LENGTH);

    // Get the respective first values
    // Left arg
    if (IsScalar (aplRankLft)                   // Scalar
     && !IsEmpty (aplNELMLft)                   // and non-empty
     && !IsNested (aplTypeLft))                 // and non-nested
        GetFirstValueToken (lptkLftArg,         // Ptr to left arg token
                           &aplIntegerLft,      // Ptr to integer result
                           &aplFloatLft,        // Ptr to float ...
                           &aplCharLft,         // Ptr to WCHAR ...
                            NULL,               // Ptr to longest ...
                           &lpSymGlbLft,        // Ptr to lpSym/Glb ...
                            NULL,               // Ptr to ...immediate type ...
                            NULL);              // Ptr to array type ...
    else                                        // otherwise,
    if ((IsScalar (aplRankLft)                  // Scalar
      || IsEmpty (aplNELMLft))                  //   or empty
     && IsNested (aplTypeLft))                  // and nested
        GetFirstValueToken (lptkLftArg,         // Ptr to left arg token
                            NULL,               // Ptr to integer result
                            NULL,               // Ptr to float ...
                            NULL,               // Ptr to WCHAR ...
                            NULL,               // Ptr to longest ...
                           &lpSymGlbLft,        // Ptr to lpSym/Glb ...
                            NULL,               // Ptr to ...immediate type ...
                            NULL);              // Ptr to array type ...
    // Right arg
    if (IsScalar (aplRankRht)                   // Scalar
     && !IsEmpty (aplNELMRht)                   // and non-empty
     && !IsNested (aplTypeRht))                 // and non-nested
        GetFirstValueToken (lptkRhtArg,         // Ptr to right arg token
                           &aplIntegerRht,      // Ptr to integer result
                           &aplFloatRht,        // Ptr to float ...
                           &aplCharRht,         // Ptr to WCHAR ...
                            NULL,               // Ptr to longest ...
                           &lpSymGlbRht,        // Ptr to lpSym/Glb ...
                            NULL,               // Ptr to ...immediate type ...
                            NULL);              // Ptr to array type ...
    else                                        // otherwise,
    if ((IsScalar (aplRankRht) || IsEmpty (aplNELMRht)) // Scalar or empty
     && IsNested (aplTypeRht))                  // and nested
        GetFirstValueToken (lptkRhtArg,         // Ptr to right arg token
                            NULL,               // Ptr to integer result
                            NULL,               // Ptr to float ...
                            NULL,               // Ptr to WCHAR ...
                            NULL,               // Ptr to longest ...
                           &lpSymGlbRht,        // Ptr to lpSym/Glb ...
                            NULL,               // Ptr to ...immediate type ...
                            NULL);              // Ptr to array type ...
    // The rank of the result is the larger of the two args
    aplRankRes = max (aplRankLft, aplRankRht);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        while (TRUE)
        {
            // Catentate allows integer axis values only
            if (CheckAxis_EM (lptkAxis,         // The axis token
                              aplRankRes,       // All values less than this
                              TRUE,             // TRUE iff scalar or one-element vector only
                              FALSE,            // TRUE iff want sorted axes
                              FALSE,            // TRUE iff axes must be contiguous
                              FALSE,            // TRUE iff duplicate axes are allowed
                              NULL,             // Return TRUE iff fractional values present
                             &aplAxis,          // Return last axis value
                              NULL,             // Return # elements in axis vector
                              NULL))            // Return HGLOBAL with APLINT axis values
                break;
            // Laminate allows fractional values
            if (CheckAxis_EM (lptkAxis,         // The axis token
                              aplRankRes + 1,   // All values less than this
                              TRUE,             // TRUE iff scalar or one-element vector only
                              FALSE,            // TRUE iff want sorted axes
                              FALSE,            // TRUE iff axes must be contiguous
                              FALSE,            // TRUE iff duplicate axes are allowed
                             &bFract,           // Return TRUE iff fractional values present
                             &aplAxis,          // Return last axis value
                              NULL,             // Return # elements in axis vector
                              NULL)             // Return HGLOBAL with APLINT axis values
             && bFract)
                break;

            // Otherwise, it's an AXIS ERROR
            goto ERROR_EXIT;
        } // End WHILE
    } else
    {
        // No axis specified:
        //   if comma, use last dimension
        if (lptkFunc->tkData.tkChar EQ UTF16_COMMA)
            aplAxis = max (0, (APLRANKSIGN) aplRankRes - 1);
        else
        {
            Assert (lptkFunc->tkData.tkChar EQ UTF16_COMMABAR);

            // Otherwise, it's CommaBar on the first dimension
            aplAxis = 0;
        } // End IF/ELSE
    } // End IF/ELSE

    // Left and right scalar args are laminated
    if (IsScalar (aplRankLft)
     && IsScalar (aplRankRht))
    {
        bFract = TRUE;
        aplAxis = 0;
    } // End IF

    // If it's laminate, add one dimension
    aplRankRes += bFract;

    //***************************************************************
    // Check for RANK & LENGTH ERRORs.
    // If either argument is a scalar, no error.
    // Catenate:
    //      The arguments may differ in rank by 0 or 1 only.
    //      If the arguments differ in rank by 1, then the shape of the
    //        arg of larger rank without the axis dimension must be the
    //        same as that of shape of the arg of the smaller rank.
    // Laminate:
    //      The arguments must be of the same rank and shape.
    //***************************************************************

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If neither arg is a scalar, ...
    if (!IsScalar (aplRankLft)
     && !IsScalar (aplRankRht))
    {
        // Skip over the headers to the dimensions
        lpMemDimLft = VarArrayBaseToDim (lpMemHdrLft);
        lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

        // If the ranks differ, ...
        if (aplRankLft NE aplRankRht)
        {
            // If laminate or the ranks differ by more than 1, ...
            if (bFract || abs64 ((APLRANKSIGN) (aplRankLft - aplRankRht)) NE 1)
                goto RANK_EXIT;

            // The shapes must be equal except for aplAxis in the larger rank arg
            for (uRht = uLft = 0; uRht < aplRankRht && uLft < aplRankLft; uLft++, uRht++)
            {
                // If the right arg is the larger rank, ...
                if (aplRankLft < aplRankRht)
                    // If this is the axis dimension, skip it
                    uRht += (uRht EQ aplAxis);
                else
                    // If this is the axis dimension, skip it
                    uLft += (uLft EQ aplAxis);

                // Compare the dimensions
                if (lpMemDimLft[uLft] NE lpMemDimRht[uRht])
                    goto LENGTH_EXIT;
            } // End FOR
        } else
        // The ranks are the same
        {
            // For catenate, the shapes must be equal except for aplAxis.
            // For laminate, the shapes must be equal.
            for (uRht = 0; uRht < aplRankRht; uRht++)
            if ((bFract || uRht NE aplAxis)             // Laminate or not aplAxis
             && lpMemDimLft[uRht] NE lpMemDimRht[uRht]) // Compare the dimensions
                goto LENGTH_EXIT;
        } // End IF/ELSE
    } // End IF

    // If left arg is scalar, set dimension to 1
    if (IsScalar (aplRankLft))
        lpMemDimLft = &aplDim1;
    else
        // Skip over the header to the dimensions
        lpMemDimLft = VarArrayBaseToDim (lpMemHdrLft);

    // If right arg is scalar, set dimension to 1
    if (IsScalar (aplRankRht))
        lpMemDimRht = &aplDim1;
    else
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

    // Get a ptr to the dimensions of the larger (or equal) rank arg
    if (aplRankLft < aplRankRht)
    {
        lpMemDimDir = lpMemDimRht;
        aplRankTmp  = aplRankRht;
    } else
    {
        lpMemDimDir = lpMemDimLft;
        aplRankTmp  = aplRankLft;
    } // End IF/ELSE

    // Calculate the product of the non-axis dimensions
    for (uRht = 0, aplDimTmp = 1; uRht < aplRankTmp; uRht++)
    if (bFract || uRht NE aplAxis)
        aplDimTmp *= lpMemDimDir[uRht];

    // Calculate the NELM of the result
    if (bFract)
        aplNELMRes = aplDimTmp * 2;
    else
    if (aplRankLft NE aplRankRht)
        aplNELMRes = aplDimTmp * (lpMemDimDir[aplAxis] + 1);
    else
        aplNELMRes = aplDimTmp * (lpMemDimLft[aplAxis] + lpMemDimRht[aplAxis]);

    // Determine the storage type of the result based upon
    //   the storage types of the left and right args
    //   as well as the NELMs
    if (IsEmpty (aplNELMLft))
    {
        aplTypeRes = aplTypeLft = aplTypeRht;
        hGlbTmp    = hGlbRht;
    } else
    if (IsEmpty (aplNELMRht))
    {
        aplTypeRes = aplTypeRht = aplTypeLft;
        hGlbTmp    = hGlbLft;
    } else
        aplTypeRes = aTypePromote[aplTypeLft][aplTypeRht];

    // Check for Boolean result
    if ((IsSimpleBool (aplTypeLft)
      && IsTknBooleanAPA (lptkRhtArg))
     || (IsSimpleBool (aplTypeRht)
      && IsTknBooleanAPA (lptkLftArg)))
        aplTypeRes = ARRAY_BOOL;

    // Check for APA result
    if (IsSimpleAPA (aplTypeRes))
    {
        // Copy the right arg
        hGlbRes = CopySymGlbDirAsGlb (hGlbTmp);

        goto YYALLOC_EXIT;
    } // End IF

    // If the result is empty, use the prototype of the right arg
    if (IsEmpty (aplNELMRes))
    {
        // Split cases based upon the right arg's storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
                aplTypeRes = ARRAY_BOOL;

                break;

            case ARRAY_CHAR:
                aplTypeRes = ARRAY_CHAR;

                break;

            case ARRAY_RAT:
            case ARRAY_HC2R:
            case ARRAY_HC4R:
            case ARRAY_HC8R:
            case ARRAY_VFP:
            case ARRAY_HC2V:
            case ARRAY_HC4V:
            case ARRAY_HC8V:
            case ARRAY_NESTED:
                break;

            case ARRAY_APA:         // Can't happen
            case ARRAY_HETERO:      // Can't happen
            defstop
                break;
        } // End SWITCH
    } // End IF

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Now we can allocate the storage for the result
    //***************************************************************
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock (hGlbRes);

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

    // Skip over the result header to the dimensions
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // Skip over the result header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Copy the dimensions of the larger rank arg
    for (uRht = 0; uRht < aplRankTmp; uRht++)
        lpMemDimRes[uRht + (bFract && (aplAxis <= uRht))] = lpMemDimDir[uRht];

    // Add in the axis dimension
    if (bFract)
        lpMemDimRes[aplAxis] = 2;
    else
    if (aplRankLft NE aplRankRht)
        lpMemDimRes[aplAxis]++;
    else
        lpMemDimRes[aplAxis] = lpMemDimLft[aplAxis] + lpMemDimRht[aplAxis];

    // Calculate the product of the left/right arg dimensions
    //   before the axis
    for (uBeg = 0,
           aplDimBeg = 1;
         uBeg < aplAxis;
         uBeg++)
        aplDimBeg *= lpMemDimRes[uBeg];

    // Calculate the product of the left arg dimensions
    //   at and beyond the axis taking into account
    //   laminate, scalar extension, and rank differing by one
    aplDimLftEnd = 1;
    if (IsScalar (aplRankLft))
        for (uEnd = aplAxis + !bFract;
             uEnd < aplRankRht;
             uEnd++)
            aplDimLftEnd *= lpMemDimRht[uEnd];
    else
        for (uEnd = aplAxis;
             uEnd < aplRankLft;
             uEnd++)
            aplDimLftEnd *= lpMemDimLft[uEnd];

    // Calculate the product of the right arg dimensions
    //   at and beyond the axis taking into account
    //   laminate, scalar extension, and rank differing by one
    aplDimRhtEnd = 1;
    if (IsScalar (aplRankRht))
        for (uEnd = aplAxis + !bFract;
             uEnd < aplRankLft;
             uEnd++)
            aplDimRhtEnd *= lpMemDimLft[uEnd];
    else
        for (uEnd = aplAxis;
             uEnd < aplRankRht;
             uEnd++)
            aplDimRhtEnd *= lpMemDimRht[uEnd];

    // Initialize the bit masks and index for Boolean arguments
    uBitMaskLft = uBitMaskRht = BIT0;
    uBitIndexRes = 0;

    // If the left arg is not a scalar or is a global numeric,
    //   skip over its header and dimensions to the data
    //   and get the APA values if appropriate
    if (!IsScalar (aplRankLft)
     || IsGlbNum (aplTypeLft))
    {
        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);

        // If the left arg is an APA, ...
        if (IsSimpleAPA (aplTypeLft))
        {
#define lpAPA       ((LPAPLAPA) lpMemLft)
            // Get the APA parameters
            apaOffLft = lpAPA->Off;
            apaMulLft = lpAPA->Mul;
#undef  lpAPA
            uEndLft   = 0;
        } // End IF
    } else
        // Point to the immediate value
        lpMemLft = &aplLongestLft;

    // If the right arg is not a scalar or is a global numeric,
    //   skip over its header and dimensions to the data
    //   and get the APA values if appropriate
    if (!IsScalar (aplRankRht)
     || IsGlbNum (aplTypeRht))
    {
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

        // If the right arg is an APA, ...
        if (IsSimpleAPA (aplTypeRht))
        {
#define lpAPA       ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOffRht = lpAPA->Off;
            apaMulRht = lpAPA->Mul;
#undef  lpAPA
            uEndRht   = 0;
        } // End IF
    } else
        // Point to the immediate value
        lpMemRht = &aplLongestRht;

    // Split cases based upon the result's storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:                // Res = BOOL, Lft/Rht = BOOL
            // If the result is Boolean, then both args are Boolean-valued
            //   (either or both could be a Boolean-valued APA)

            Assert (!IsSimpleAPA (aplTypeLft) || (aplNELMLft EQ 0) || (apaMulLft EQ 0 && IsBooleanValue (apaOffLft)));
            Assert (!IsSimpleAPA (aplTypeRht) || (aplNELMRht EQ 0) || (apaMulRht EQ 0 && IsBooleanValue (apaOffRht)));

            // Loop through the leading dimensions
            for (uBeg = 0; uBeg < aplDimBeg; uBeg++)
            {
                // If the left arg is a scalar, ...
                if (IsScalar (aplRankLft))
                    // Loop through the left arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *((LPAPLBOOL) lpMemRes) |= (APLBOOL) (aplIntegerLft << uBitIndexRes);

                        // Increment the result bit index
                        uBitIndexRes++;

                        // Check for end-of-byte
                        if (uBitIndexRes EQ NBIB)
                        {
                            uBitIndexRes = 0;           // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF
                    } // End FOR
                else
                    // Loop through the left arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        if (IsSimpleAPA (aplTypeLft))
                            *((LPAPLBOOL) lpMemRes) |= apaOffLft << uBitIndexRes;
                        else
                            *((LPAPLBOOL) lpMemRes) |= ((uBitMaskLft & *((LPAPLBOOL) lpMemLft)) ? TRUE : FALSE) << uBitIndexRes;

                        // Increment the result bit index
                        uBitIndexRes++;

                        // Check for end-of-byte
                        if (uBitIndexRes EQ NBIB)
                        {
                            uBitIndexRes = 0;           // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF

                        // Shift over the left bit mask
                        uBitMaskLft <<= 1;

                        // Check for end-of-byte
                        if (uBitMaskLft EQ END_OF_BYTE)
                        {
                            uBitMaskLft = BIT0;         // Start over
                            ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                // If the right arg is a scalar, ...
                if (IsScalar (aplRankRht))
                    // Loop through the right arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *((LPAPLBOOL) lpMemRes) |= (APLBOOL) (aplIntegerRht << uBitIndexRes);

                        // Increment the result bit index
                        uBitIndexRes++;

                        // Check for end-of-byte
                        if (uBitIndexRes EQ NBIB)
                        {
                            uBitIndexRes = 0;           // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF
                    } // End FOR
                else
                    // Loop through the right arg's trailing dimensions
                    for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        if (IsSimpleAPA (aplTypeRht))
                            *((LPAPLBOOL) lpMemRes) |= apaOffRht << uBitIndexRes;
                        else
                            *((LPAPLBOOL) lpMemRes) |= ((uBitMaskRht & *((LPAPLBOOL) lpMemRht)) ? TRUE : FALSE) << uBitIndexRes;

                        // Increment the result bit index
                        uBitIndexRes++;

                        // Check for end-of-byte
                        if (uBitIndexRes EQ NBIB)
                        {
                            uBitIndexRes = 0;           // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF

                        // Shift over the right bit mask
                        uBitMaskRht <<= 1;

                        // Check for end-of-byte
                        if (uBitMaskRht EQ END_OF_BYTE)
                        {
                            uBitMaskRht = BIT0;         // Start over
                            ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                        } // End IF
                } // End FOR
            } // End FOR

            break;

        case ARRAY_HETERO:              // Res = HETERO, Lft/Rht = HETERO/Num/CHAR
        case ARRAY_NESTED:              // Res = NESTED, Lft/Rht = NESTED/HETERO/Num/CHAR
            // If the left arg is a simple scalar, ...
            if (IsScalar (aplRankLft)
             && IsSimpleNH (aplTypeLft))
            {
                // Split cases based upon the left arg storage type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                        aplVal = aplIntegerLft;

                        break;

                    case ARRAY_FLOAT:
                        aplVal = *(LPAPLLONGEST) &aplFloatLft;

                        break;

                    case ARRAY_CHAR:
                        aplVal = aplCharLft;

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Make an LPSYMENTRY out of it
                lpSymGlbLft =
                  MakeSymEntry_EM (TranslateArrayTypeToImmType (aplTypeLft),    // Immediate type
                                  &aplVal,                                      // Ptr to immediate value
                                   lptkFunc);                                   // Ptr to function token
                if (lpSymGlbLft EQ NULL)
                    goto ERROR_EXIT;
            } // End IF

            // If the right arg is a simple scalar, ...
            if (IsScalar (aplRankRht)
             && IsSimpleNH (aplTypeRht))
            {
                // Split cases based upon the right arg storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                        aplVal = aplIntegerRht;

                        break;

                    case ARRAY_FLOAT:
                        aplVal = *(LPAPLLONGEST) &aplFloatRht;

                        break;

                    case ARRAY_CHAR:
                        aplVal = aplCharRht;

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Make an LPSYMENTRY out of it
                lpSymGlbRht =
                  MakeSymEntry_EM (TranslateArrayTypeToImmType (aplTypeRht),    // Immediate type
                                  &aplVal,                                      // Ptr to immediate value
                                   lptkFunc);                                   // Ptr to function token
                if (lpSymGlbRht EQ NULL)
                    goto ERROR_EXIT;
            } // End IF

            // If the result is empty, use the prototype of the right arg
            if (IsEmpty (aplNELMRes))
                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbRht);
            else
            // Loop through the leading dimensions
            for (uBeg = 0; uBeg < aplDimBeg; uBeg++)
            {
                // Split cases based upon the left arg's storage type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:    // Res = NESTED, Lft = BOOL
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbLft);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplVal = (uBitMaskLft & *((LPAPLBOOL) lpMemLft)) ? TRUE : FALSE;
                                *((LPAPLNESTED) lpMemRes)++ =
                                lpSymTmp =
                                  MakeSymEntry_EM (IMMTYPE_BOOL,    // Immediate type
                                                  &aplVal,          // Ptr to immediate value
                                                   lptkFunc);       // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                                // Shift over the bit mask
                                uBitMaskLft <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskLft EQ END_OF_BYTE)
                                {
                                    uBitMaskLft = BIT0;         // Start over
                                    ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = NESTED, Lft = INT
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbLft);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplVal = *((LPAPLINT) lpMemLft)++;
                                *((LPAPLNESTED) lpMemRes)++ =
                                lpSymTmp =
                                  MakeSymEntry_EM (IMMTYPE_INT,     // Immediate type
                                                  &aplVal,          // Ptr to immediate value
                                                  lptkFunc);        // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR
                        break;

                    case ARRAY_FLOAT:   // Res = NESTED, Lft = FLOAT
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbLft);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplVal = *((LPAPLINT) lpMemLft)++;
                                *((LPAPLNESTED) lpMemRes)++ =
                                lpSymTmp =
                                  MakeSymEntry_EM (IMMTYPE_FLOAT,   // Immediate type
                                                  &aplVal,          // Ptr to immediate value
                                                  lptkFunc);        // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR
                        break;

                    case ARRAY_CHAR:    // Res = NESTED, Lft = CHAR
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbLft);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplVal = *((LPAPLCHAR) lpMemLft)++;
                                *((LPAPLNESTED) lpMemRes)++ =
                                lpSymTmp =
                                  MakeSymEntry_EM (IMMTYPE_CHAR,    // Immediate type
                                                  &aplVal,          // Ptr to immediate value
                                                   lptkFunc);       // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR
                        break;

                    case ARRAY_APA:     // Res = NESTED, Lft = APA
                        // Loop through the left arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            aplVal = apaOffLft + apaMulLft * uEndLft++;
                            *((LPAPLNESTED) lpMemRes)++ =
                            lpSymTmp =
                              MakeSymEntry_EM (IMMTYPE_INT,         // Immediate type
                                              &aplVal,              // Ptr to immediate value
                                               lptkFunc);           // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                        } // End FOR

                        break;

                    case ARRAY_HETERO:  // Res = NESTED, Lft = HETERO
                    case ARRAY_NESTED:  // Res = NESTED, Lft = NESTED
                        // If the left arg is a scalar, ...
                        if (IsScalar (aplRankLft))
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbLft);
                            } // End FOR
                        else
                            // Loop through the left arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (*((LPAPLNESTED) lpMemLft)++);
                            } // End FOR
                        break;

                    case ARRAY_RAT:     // Res = NESTED, Lft = RAT
                    case ARRAY_VFP:     // Res = NESTED, Lft = VFP
                    case ARRAY_HC2I:    // Res = NESTED, Lft = HC2I
                    case ARRAY_HC2F:    // Res = NESTED, Lft = HC2F
                    case ARRAY_HC2R:    // Res = NESTED, Lft = HC2R
                    case ARRAY_HC2V:    // Res = NESTED, Lft = HC2V
                    case ARRAY_HC4I:    // Res = NESTED, Lft = HC4I
                    case ARRAY_HC4F:    // Res = NESTED, Lft = HC4F
                    case ARRAY_HC4R:    // Res = NESTED, Lft = HC4R
                    case ARRAY_HC4V:    // Res = NESTED, Lft = HC4V
                    case ARRAY_HC8I:    // Res = NESTED, Lft = HC8I
                    case ARRAY_HC8F:    // Res = NESTED, Lft = HC8F
                    case ARRAY_HC8R:    // Res = NESTED, Lft = HC8R
                    case ARRAY_HC8V:    // Res = NESTED, Lft = HC8V
                        // If the left arg is not a scalar, ...
                        if (!IsScalar (aplRankLft))
                            // Calculate sizeof
                            uLenLft = TranslateArrayTypeToSizeof (aplTypeLft);
                        else
                            uLenLft = 0;

                        // Loop through the left arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            *((LPAPLNESTED) lpMemRes)++ =
                            lpSymTmp =
                              MakeGlbEntry_EM (aplTypeLft,      // Entry type
                                               lpMemLft,        // Ptr to the value
                                               TRUE,            // TRUE iff we should initialize the target first
                                               lptkFunc);       // Ptr to function token
                            if (lpSymTmp EQ NULL)
                                goto ERROR_EXIT;

                            // Skip over the value
                            ((LPBYTE) lpMemLft) += uLenLft;
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Res = NESTED, Rht = BOOL
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbRht);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplVal = (uBitMaskRht & *((LPAPLBOOL) lpMemRht)) ? TRUE : FALSE;
                                *((LPAPLNESTED) lpMemRes)++ =
                                lpSymTmp =
                                  MakeSymEntry_EM (IMMTYPE_BOOL,    // Immediate type
                                                  &aplVal,          // Ptr to immediate value
                                                   lptkFunc);       // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;

                                // Shift over the bit mask
                                uBitMaskRht <<= 1;

                                // Check for end-of-byte
                                if (uBitMaskRht EQ END_OF_BYTE)
                                {
                                    uBitMaskRht = BIT0;         // Start over
                                    ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                                } // End IF
                            } // End FOR
                        break;

                    case ARRAY_INT:     // Res = NESTED, Rht = INT
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbRht);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplVal = *((LPAPLINT) lpMemRht)++;
                                *((LPAPLNESTED) lpMemRes)++ =
                                lpSymTmp =
                                  MakeSymEntry_EM (IMMTYPE_INT,     // Immediate type
                                                  &aplVal,          // Ptr to immediate value
                                                   lptkFunc);       // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR
                        break;

                    case ARRAY_FLOAT:   // Res = NESTED, Rht = FLOAT
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbRht);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplVal = *((LPAPLINT) lpMemRht)++;
                                *((LPAPLNESTED) lpMemRes)++ =
                                lpSymTmp =
                                  MakeSymEntry_EM (IMMTYPE_FLOAT,   // Immediate type
                                                  &aplVal,          // Ptr to immediate value
                                                   lptkFunc);       // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR
                        break;

                    case ARRAY_CHAR:    // Res = NESTED, Rht = CHAR
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbRht);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                aplVal = *((LPAPLCHAR) lpMemRht)++;
                                *((LPAPLNESTED) lpMemRes)++ =
                                lpSymTmp =
                                  MakeSymEntry_EM (IMMTYPE_CHAR,    // Immediate type
                                                  &aplVal,          // Ptr to immediate value
                                                   lptkFunc);       // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR
                        break;

                    case ARRAY_APA:     // Res = NESTED, Rht = APA
                        // Loop through the right arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            aplVal = apaOffRht + apaMulRht * uEndRht++;
                            *((LPAPLNESTED) lpMemRes)++ =
                            lpSymTmp =
                              MakeSymEntry_EM (IMMTYPE_INT,         // Immediate type
                                              &aplVal,              // Ptr to immediate value
                                               lptkFunc);           // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                        } // End FOR

                        break;

                    case ARRAY_HETERO:  // Res = NESTED, Rht = HETERO
                    case ARRAY_NESTED:  // Res = NESTED, Rht = NESTED
                        // If the right arg is a scalar, ...
                        if (IsScalar (aplRankRht))
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lpSymGlbRht);
                            } // End FOR
                        else
                            // Loop through the right arg's trailing dimensions
                            for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (*lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (*((LPAPLNESTED) lpMemRht)++);
                            } // End FOR
                        break;

                    case ARRAY_RAT:     // Res = NESTED, Rht = RAT
                    case ARRAY_VFP:     // Res = NESTED, Rht = VFP
                    case ARRAY_HC2I:    // Res = NESTED, Rht = HC2I
                    case ARRAY_HC2F:    // Res = NESTED, Rht = HC2F
                    case ARRAY_HC2R:    // Res = NESTED, Rht = HC2R
                    case ARRAY_HC2V:    // Res = NESTED, Rht = HC2V
                    case ARRAY_HC4I:    // Res = NESTED, Rht = HC4I
                    case ARRAY_HC4F:    // Res = NESTED, Rht = HC4F
                    case ARRAY_HC4R:    // Res = NESTED, Rht = HC4R
                    case ARRAY_HC4V:    // Res = NESTED, Rht = HC4V
                    case ARRAY_HC8I:    // Res = NESTED, Rht = HC8I
                    case ARRAY_HC8F:    // Res = NESTED, Rht = HC8F
                    case ARRAY_HC8R:    // Res = NESTED, Rht = HC8R
                    case ARRAY_HC8V:    // Res = NESTED, Rht = HC8V
                        // If the right arg is not a scalar, ...
                        if (!IsScalar (aplRankRht))
                            // Calculate sizeof
                            uLenRht = TranslateArrayTypeToSizeof (aplTypeRht);
                        else
                            uLenRht = 0;

                        // Loop through the right arg's trailing dimensions
                        for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (*lpbCtrlBreak))
                                goto ERROR_EXIT;

                            *((LPAPLNESTED) lpMemRes)++ =
                            lpSymTmp =
                              MakeGlbEntry_EM (aplTypeRht,      // Entry type
                                               lpMemRht,        // Ptr to the value
                                               TRUE,            // TRUE iff we should initialize the target first
                                               lptkFunc);       // Ptr to function token
                            if (lpSymTmp EQ NULL)
                                goto ERROR_EXIT;

                            // Skip over the value
                            ((LPBYTE) lpMemRht) += uLenRht;
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        case ARRAY_INT:                 // Res = INT
        case ARRAY_FLOAT:               // Res = FLOAT
        case ARRAY_RAT:                 // Res = RAT
        case ARRAY_VFP:                 // Res = VFP
        case ARRAY_CHAR:                // Res = CHAR, Lft/Rht = CHAR
        case ARRAY_HC2I:                // Res = HC2I
        case ARRAY_HC2F:                // Res = HC2F
        case ARRAY_HC2R:                // Res = HC2R
        case ARRAY_HC2V:                // Res = HC2V
        case ARRAY_HC4I:                // Res = HC4I
        case ARRAY_HC4F:                // Res = HC4F
        case ARRAY_HC4R:                // Res = HC4R
        case ARRAY_HC4V:                // Res = HC4V
        case ARRAY_HC8I:                // Res = HC8I
        case ARRAY_HC8F:                // Res = HC8F
        case ARRAY_HC8R:                // Res = HC8R
        case ARRAY_HC8V:                // Res = HC8V
            // If the left arg is a scalar, ...
            if (IsScalar (aplRankLft))
                // Convert the (scalar) left arg to the result type
                (*aTypeActPromote[aplTypeLft][aplTypeRes]) (lpMemLft, 0, &atLft);

            // If the right arg is a scalar, ...
            if (IsScalar (aplRankRht))
                // Convert the (scalar) right arg to the result type
                (*aTypeActPromote[aplTypeRht][aplTypeRes]) (lpMemRht, 0, &atRht);

            // Loop through the leading dimensions
            for (uBeg = uRes = uLft = uRht = 0; uBeg < aplDimBeg; uBeg++)
            {
                // Loop through the left arg's trailing dimensions
                for (uEnd = 0; uEnd < aplDimLftEnd; uEnd++, uRes++, uLft++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // If the left arg is not a scalar, ...
                    if (!IsScalar (aplRankLft))
                        // Convert the left arg to the result type
                        (*aTypeActPromote[aplTypeLft][aplTypeRes]) (lpMemLft, uLft, &atLft);

                    // Save in the result
                    InitSet (aplTypeRes, lpMemRes, uRes, &atLft);

                    // If the left arg is not a scalar, ...
                    if (!IsScalar (aplRankLft))
                        // Free the value in atLft
                        (*aTypeFree[aplTypeRes]) (&atLft, 0);
                } // End FOR

                // Loop through the right arg's trailing dimensions
                for (uEnd = 0; uEnd < aplDimRhtEnd; uEnd++, uRes++, uRht++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // If the right arg is not a scalar, ...
                    if (!IsScalar (aplRankRht))
                        // Convert the right arg to the result type
                        (*aTypeActPromote[aplTypeRht][aplTypeRes]) (lpMemRht, uRht, &atRht);

                    // Save in the result
                    InitSet (aplTypeRes, lpMemRes, uRes, &atRht);

                    // If the right arg is not a scalar, ...
                    if (!IsScalar (aplRankRht))
                        // Free the value in atRht
                        (*aTypeFree[aplTypeRes]) (&atRht, 0);
                } // End FOR
            } // End FOR

            // If the left arg is a scalar, ...
            if (IsScalar (aplRankLft))
                // Free the left arg
                (*aTypeFree[aplTypeRes]) (&atLft, 0);

            // If the right arg is a scalar, ...
            if (IsScalar (aplRankRht))
                // Free the right arg
                (*aTypeFree[aplTypeRes]) (&atRht, 0);

            break;

        case ARRAY_APA:
        defstop
            break;
    } // End SWITCH
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

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
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
    if (hGlbLft NE NULL
     && lpMemLft NE NULL)
    {
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    if (hGlbRht NE NULL
     && lpMemRht NE NULL)
    {
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (hGlbRes NE NULL
     && lpMemRes NE NULL)
    {
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnDydComma_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_comma.c
//***************************************************************************
