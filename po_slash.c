//***************************************************************************
//  NARS2000 -- Primitive Operator -- Slash
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

extern LPFASTBOOLFNS FastBoolFns;   // ***FIXME*** -- move into externs.h


//***************************************************************************
//  $PrimOpSlash_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from Slash
//    ("reduction" and "N-wise reduction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpSlash_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpSlash_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    Assert (lpYYFcnStr->tkToken.tkData.tkChar EQ INDEX_OPSLASH
         || lpYYFcnStr->tkToken.tkData.tkChar EQ INDEX_OPSLASHBAR);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonSlash_EM_YY (lpYYFcnStr,    // Ptr to operator function strand
                                     lptkRhtArg,    // Ptr to right arg
                                     lptkAxis);     // Ptr to axis token (may be NULL)
    else
        return PrimOpDydSlash_EM_YY (lptkLftArg,    // Ptr to left arg token
                                     lpYYFcnStr,    // Ptr to operator function strand
                                     lptkRhtArg,    // Ptr to right arg token
                                     lptkAxis);     // Ptr to axis token (may be NULL)
} // End PrimOpSlash_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOpSlash_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator Slash ("reduction" and "N-wise reduction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoOpSlash_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimProtoOpSlash_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token
     LPYYSTYPE lpYYFcnStrOpr,       // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    LPYYSTYPE lpYYFcnStrLft;        // Ptr to left operand function strand
    LPPRIMFNS lpPrimProtoLft;       // Ptr to left operand prototype function

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];

    // Get a ptr to the prototype function for the first symbol (a function or operator)
    lpPrimProtoLft = PrimProtoFnsTab[SymTrans (&lpYYFcnStrLft->tkToken)];
    if (!lpPrimProtoLft)
    {
        ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                  &lpYYFcnStrLft->tkToken);
        return NULL;
    } // End IF

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************

        return PrimOpMonSlashCommon_EM_YY (lpYYFcnStrOpr,   // Ptr to operator function strand
                                           lptkRhtArg,      // Ptr to right arg token
                                           lptkAxis,        // Ptr to axis token (may be NULL)
                                           lpPrimProtoLft); // Ptr to left operand prototype function
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************

        return PrimOpDydSlashCommon_EM_YY (lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
                                           lpYYFcnStrOpr,   // Ptr to operator function strand
                                           lptkRhtArg,      // Ptr to right arg token
                                           lptkAxis,        // Ptr to axis token (may be NULL)
                                           lpPrimProtoLft); // Ptr to left operand prototype function
} // End PrimProtoOpSlash_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpMonSlash_EM_YY
//
//  Primitive operator for monadic derived function from Slash ("reduction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonSlash_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpMonSlash_EM_YY
    (LPYYSTYPE lpYYFcnStrOpr,       // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    return PrimOpMonSlashCommon_EM_YY (lpYYFcnStrOpr,       // Ptr to operator function strand
                                       lptkRhtArg,          // Ptr to right arg token
                                       lptkAxis,            // Ptr to axis token (may be NULL)
                                       NULL);               // Ptr to left operand prototype function
} // End PrimOpMonSlash_EM_YY


//***************************************************************************
//  $PrimOpMonSlashCommon_EM_YY
//
//  Primitive operator for monadic derived function from Slash ("reduction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonSlash_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpMonSlashCommon_EM_YY
    (LPYYSTYPE lpYYFcnStrOpr,       // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis,            // Ptr to axis token (may be NULL)
     LPPRIMFNS lpPrimProtoLft)      // Ptr to left operand prototype function

{
    APLSTYPE    aplTypeRht,         // Right arg storage type
                aplTypeRes;         // Result    ...
    APLNELM     aplNELMRht,         // Right arg NELM
                aplNELMRes;         // Result    ...
    APLRANK     aplRankRht,         // Right arg rank
                aplRankRes;         // Result    ...
    APLUINT     aplAxis,            // The (one and only) axis value
                uLo,                // uDimLo loop counter
                uHi,                // uDimHi ...
                uDim,
                uDimLo,             // Product of dimensions below axis
                uDimHi,             // ...                   above ...
                uDimRht,            // Starting index in right arg of current vector
                uDimRes,            // ...               result    ...
                uDimAxRht,          // Right arg axis dimension
                uRht,               // Right arg loop counter
                uRes,               // Result loop counter
                ByteRes;            // # bytes in the result
    APLINT      iDim,               // Integer dimension loop counter
                apaOffRht,          // Right arg APA offset
                apaMulRht;          // ...           multiplier
    HGLOBAL     hGlbRht = NULL,     // Right arg global memory handle
                hGlbRes = NULL;     // Result    ...
    LPVOID      lpMemRht = NULL,    // Ptr to right arg global memory
                lpMemRes = NULL;    // Ptr to result    ...
    LPAPLDIM    lpMemDimRht;        // Ptr to right arg dimensions
    APLFLOAT    aplFloatIdent;      // Identity element
    BOOL        bRet = TRUE,        // TRUE iff result is valid
                bFastBool = FALSE;  // TRUE iff this is a Fast Boolean operation
////LPPRIMSPEC  lpPrimSpec;         // Ptr to local PRIMSPEC
    LPPRIMFLAGS lpPrimFlags;        // Ptr to corresponding PrimFlags entry
    LPYYSTYPE   lpYYRes = NULL,     // Ptr to the result
                lpYYFcnStrLft;      // Ptr to operator function strand
    TOKEN       tkLftArg = {0},     // Left arg token
                tkRhtArg = {0};     // Right ...

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Reduction allows a single integer axis value only
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
        // if Slash, use last dimension
        if (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLASH)
            aplAxis = aplRankRht - 1;
        else
            // Otherwise, it's SlashBar on the first dimension
            aplAxis = 0;
    } // End IF/ELSE

    // Get right arg's global ptr
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg is a scalar, return it
    if (aplRankRht EQ 0)
    {
        lpYYRes = PrimOpMonSlashScalar_EM_YY (lptkRhtArg,       // Ptr to right arg token
                                              hGlbRht,          // Right arg global memory handle
                                              lpMemRht,         // Ptr to right arg global memory
                                              lpYYFcnStrOpr,    // Ptr to operator function strand
                                              lpPrimProtoLft);  // Ptr to lefft operand prototype function
        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // From here on, the right arg is a vector or higher rank array
    //***************************************************************

    // The rank of the result is one less than that of the right arg
    aplRankRes = aplRankRht - 1;

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    //***************************************************************
    // Calculate product of dimensions before, at, and after the axis dimension
    //***************************************************************

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

    // Calculate the result NELM
    aplNELMRes = imul64 (uDimLo, uDimHi, &bRet);
    if (!bRet)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                  &lpYYFcnStrOpr->tkToken);
        goto ERROR_EXIT;
    } // End IF

    // Handle prototypes specially
    if (aplNELMRes EQ 0
     && lpPrimProtoLft EQ NULL)
    {
        // Get a ptr to the prototype function for the first symbol (a function or operator)
        lpPrimProtoLft = PrimProtoFnsTab[SymTrans (&lpYYFcnStrLft->tkToken)];
        if (!lpPrimProtoLft)
        {
            ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                      &lpYYFcnStrLft->tkToken);
            return NULL;
        } // End IF
    } // End IF

    // Calculate a ptr to the Primitive Function Flags
    lpPrimFlags = &PrimFlags[SymTrans (&lpYYFcnStrLft->tkToken)];

    // If the axis dimension is zero, get the identity
    //   element for the left operand or signal a DOMAIN ERROR
    if (uDimAxRht EQ 0)
    {
        // If it's an immediate primitive function,
        //   and a
        if (!lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
         || PrimFlags[SymTrans (&lpYYFcnStrLft->tkToken)].Index EQ PF_IND_UNK)
        {
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                      &lpYYFcnStrOpr->tkToken);
            goto ERROR_EXIT;
        } // End IF

        // Get the identity element
        aplFloatIdent = PrimIdent[PrimFlags[SymTrans (&lpYYFcnStrLft->tkToken)].Index];

        // If the identity element is Boolean, the result is too
        if (aplFloatIdent EQ 0.0
         || aplFloatIdent EQ 1.0
         || lpPrimProtoLft NE NULL)
            aplTypeRes = ARRAY_BOOL;
        else
            aplTypeRes = ARRAY_FLOAT;
    } else
    // If the product of the dimensions above
    //   the axis dimension is one, and
    //   this is a primitive function, and
    //   the right arg is Boolean, and
    //   we're not doing prototypes, then
    //   check for the possibility of doing a
    //   Fast Boolean Reduction
    if (uDimHi EQ 1
     && lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
     && aplTypeRht EQ ARRAY_BOOL
     && lpPrimProtoLft EQ NULL
     && (lpPrimFlags->FastBool || lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_PLUS))
    {
        // Mark as a Fast Boolean operation
        bFastBool = TRUE;

        // If this is "plus", then the storage type is Integer
        if (lpPrimFlags->Index EQ PF_IND_PLUS)
            aplTypeRes = ARRAY_INT;
        else
        // Otherwise, it's Boolean
            aplTypeRes = ARRAY_BOOL;
    } else
////// If the operand is a primitive scalar dyadic function,
//////   calculate the storage type of the result,
//////   otherwise, assume it's ARRAY_NESTED
////if (lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
//// && PrimFlags[SymTrans (&lpYYFcnStrLft->tkToken)].DydScalar)
////{
////    // Get the corresponding lpPrimSpec
////    lpPrimSpec = PrimSpecTab[SymTrans (&lpYYFcnStrLft->tkToken)];
////
////    // Calculate the storage type of the result
////    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (1,
////                                               &aplTypeRht,
////                                               &lpYYFcnStrLft->tkToken,
////                                                1,
////                                               &aplTypeRht);
////    if (aplTypeRes EQ ARRAY_ERROR)
////    {
////        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
////                                  &lpYYFcnStrOpr->tkToken);
////        goto ERROR_EXIT;
////    } // End IF
////} else
        aplTypeRes = ARRAY_NESTED;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Allocate space for the result
    // N.B. Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                  &lpYYFcnStrOpr->tkToken);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header values
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;       // Already zero from GHND
////lpHeader->SysVar     = 0;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;

#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Copy the dimensions from the right arg to the result
    //   except for the axis dimension
    for (uDim = 0; uDim < aplRankRht; uDim++)
    if (uDim NE aplAxis)
        *((LPAPLDIM) lpMemRes)++ = lpMemDimRht[uDim];

    // lpMemRes now points to its data

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Get APA parameters
    if (aplTypeRht EQ ARRAY_APA)
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // If the axis dimension is zero, fill with the identity element
    if (uDimAxRht EQ 0)
    {
        // The zero case is done (GHND)

        // If we're not doing prototypes, ...
        if (lpPrimProtoLft EQ NULL)
        {
            // Check for identity element 1
            if (aplFloatIdent EQ 1.0)
            {
                APLNELM uNELMRes;

                // Calculate the # bytes in the result, rounding up
                uNELMRes = (aplNELMRes + (NBIB - 1)) >> LOG2NBIB;

                for (uRes = 0; uRes < uNELMRes; uRes++)
                    *((LPAPLBOOL) lpMemRes)++ = 0xFF;
            } else
                for (uRes = 0; uRes < aplNELMRes; uRes++)
                    *((LPAPLFLOAT) lpMemRes)++ = aplFloatIdent;
        } // End IF

        goto YYALLOC_EXIT;
    } // End IF

    if (aplTypeRes EQ ARRAY_NESTED)
    {
        // Fill nested result with PTR_REUSED
        //   in case we fail part way through
        *((LPAPLNESTED) lpMemRes) = PTR_REUSED;
        for (uRes = 1; uRes < aplNELMRes; uRes++)
            ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;
    } // End IF

    // If this is a fast Boolean operation, ...
    if (bFastBool)
    {
        LPFASTBOOLFCN lpFastBool;           // Ptr to Fast Boolean reduction routine

        // Get a ptr to the appropriate Fast Boolean routine
        lpFastBool = FastBoolFns[lpPrimFlags->Index].lpReduction;

        // Call it
        (*lpFastBool) (aplTypeRht,          // Right arg storage type
                       lpMemRht,            // Ptr to right arg memory
                       lpMemRes,            // Ptr to result    memory
                       uDimLo,              // Product of dimensions below axis
                       uDimAxRht,           // Length of right arg axis dimension
                       1,                   // ...       result    ...
                       lpYYFcnStrOpr);      // Ptr to operator function strand
    } else
    {
        // Fill in the right arg token
////////tkRhtArg.tkFlags.TknType   =            // To be filled in below
////////tkRhtArg.tkFlags.ImmType   =            // To be filled in below
////////tkRhtArg.tkFlags.NoDisplay = 0;         // Already zero from {0}
////////tkRhtArg.tkData.tkGlbData  =            // To be filled in below
        tkRhtArg.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

        // Fill in the left arg token
////////tkLftArg.tkFlags.TknType   =            // To be filled in below
////////tkLftArg.tkFlags.ImmType   =            // To be filled in below
////////tkLftArg.tkFlags.NoDisplay = 0;         // Already zero from {0}
////////tkLftArg.tkData.tkGlbData  =            // To be filled in below
        tkLftArg.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

        // Loop through the right arg calling the
        //   function strand between data, storing in the
        //   result
        for (uLo = 0; uLo < uDimLo; uLo++)
        for (uHi = 0; uHi < uDimHi; uHi++)
        {
            // Calculate the starting index in the right arg/result of this vector
            uDimRht = uLo * uDimHi * uDimAxRht + uHi;
            uDimRes = uLo * uDimHi * 1         + uHi;

            // Calculate the index of last element in this vector
            uRht = uDimRht + (uDimAxRht - 1) * uDimHi;

            // Get the last element as the right arg
            GetValueInToken (uRht,              // Index to use
                             lpMemRht,          // Ptr to global memory object to index
                             aplTypeRht,        // Storage type of the arg
                             apaOffRht,         // APA offset (if needed)
                             apaMulRht,         // APA multiplier (if needed)
                            &tkRhtArg);         // Ptr to token in which to place the value
            // Loop backwards through the elements along the specified axis
            for (iDim = uDimAxRht - 2; iDim >= 0; iDim--)
            {
                // Calculate the index of the previous element in this vector
                uRht = uDimRht + iDim * uDimHi;

                // Get the previous element as the left arg
                GetValueInToken (uRht,          // Index to use
                                 lpMemRht,      // Ptr to global memory object to index
                                 aplTypeRht,    // Storage type of the arg
                                 apaOffRht,     // APA offset (if needed)
                                 apaMulRht,     // APA multiplier (if needed)
                                &tkLftArg);     // Ptr to token in which to place the value
                // Execute the left operand between the left & right args
                if (lpPrimProtoLft)
                    // Note that we cast the function strand to LPTOKEN
                    //   to bridge the two types of calls -- one to a primitive
                    //   function which takes a function token, and one to a
                    //   primitive operator which takes a function strand
                    lpYYRes = (*lpPrimProtoLft) (&tkLftArg,     // Ptr to left arg token
                                        (LPTOKEN) lpYYFcnStrLft,// Ptr to left operand function strand
                                                 &tkRhtArg,     // Ptr to right arg token
                                                  lptkAxis);    // Ptr to axis token (may be NULL)
                else
                    lpYYRes = ExecFuncStr_EM_YY (&tkLftArg,     // Ptr to left arg token
                                                  lpYYFcnStrLft,// Ptr to left operand function strand
                                                 &tkRhtArg);    // Ptr to right arg token
                // Free the left & right arg tokens
                FreeResult (&tkLftArg);
                FreeResult (&tkRhtArg);

                // If it succeeded, ...
                if (lpYYRes)
                {
                    // Copy the result to the right arg token
                    tkRhtArg = *CopyToken_EM (&lpYYRes->tkToken, FALSE);

                    // Free the YYRes
                    FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;
                } else
                    goto ERROR_EXIT;
            } // End FOR

            // Split cases based upon the token type of the right arg (result)
            switch (tkRhtArg.tkFlags.TknType)
            {
                case TKT_VARIMMED:
                    // Save in the result as an LPSYMENTRY
                    *((LPAPLNESTED) lpMemRes)++ = MakeSymEntry_EM (tkRhtArg.tkFlags.ImmType,
                                                                  &tkRhtArg.tkData.tkLongest,
                                                                  &lpYYFcnStrOpr->tkToken);
                    break;

                case TKT_VARARRAY:
                    // Save in the result as an HGLOBAL
                    *((LPAPLNESTED) lpMemRes)++ = tkRhtArg.tkData.tkGlbData;

                    break;

                defstop
                    break;
            } // End SWITCH

            // Because we didn't CopyArray_EM on the tkGlbData,
            //   we don't need to free the right arg token
        } // End FOR/FOR
    } // End IF/ELSE
YYALLOC_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    lpYYRes->tkToken = *TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
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

    return lpYYRes;
} // End PrimOpMonSlashCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpMonSlashScalar_EM_YY
//
//  Handle the case where the right arg is a scalar
//***************************************************************************

LPYYSTYPE PrimOpMonSlashScalar_EM_YY
    (LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     HGLOBAL   hGlbRht,             // Right arg global memory handle
     LPVOID    lpMemRht,            // Ptr to right arg global memory
     LPYYSTYPE lpYYFcnStrOpr,       // Ptr to operator function strand
     LPPRIMFNS lpPrimProtoLft)      // Ptr to left operand prototype function (may be NULL)

{
    LPYYSTYPE lpYYRes;              // Ptr to the result

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // If it's valid, ...
    if (lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

        // Make a copy of the right arg
        if (lpPrimProtoLft)
            hGlbRht = MakePrototype_EM (hGlbRht,                // Proto arg global memory handle
                                       &lpYYFcnStrOpr->tkToken, // Ptr to function token
                                        MP_NUMONLY);            // Numerics only
        else
            hGlbRht = CopySymGlbDir (hGlbRht);

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRht);
////////lpYYRes->tkToken.tkCharIndex       =        // Filled in below
    } else  // It's an immediate
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;

        if (lpPrimProtoLft)
        {
            lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_BOOL;
            lpYYRes->tkToken.tkData.tkLongest  = 0;
        } else
        {
            lpYYRes->tkToken.tkFlags.ImmType   = lptkRhtArg->tkFlags.ImmType;
            lpYYRes->tkToken.tkData.tkLongest  = lptkRhtArg->tkData.tkLongest;
        } // End IF/ELSE

////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkCharIndex       =        // Filled in below
    } // End IF/ELSE

    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    return lpYYRes;
} // End PrimOpMonSlashScalar_EM_YY


//***************************************************************************
//  $SymGlbToToken
//
//  Move a Sym/Glb value to a token
//***************************************************************************

void SymGlbToToken
    (LPTOKEN lptkArg,
     HGLOBAL hSymGlb)

{
    // Split cases based upon the element's ptr type
    switch (GetPtrTypeDir (hSymGlb))
    {
        case PTRTYPE_STCONST:
            lptkArg->tkFlags.TknType   = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType   = ((LPSYMENTRY) hSymGlb)->stFlags.ImmType;
////////////lptkArg->tkFlags.NoDisplay = 0;         // Already zero by the caller
            lptkArg->tkData.tkLongest  = ((LPSYMENTRY) hSymGlb)->stData.stLongest;
////////////lptkArg->tkCharIndex       =            // Filled in by the caller

            break;

        case PTRTYPE_HGLOBAL:
            lptkArg->tkFlags.TknType   = TKT_VARARRAY;
////////////lptkArg->tkFlags.ImmType   = 0;         // Already zero by caller
////////////lptkArg->tkFlags.NoDisplay = 0;         // Already zero by the caller
            lptkArg->tkData.tkGlbData  = hSymGlb;
////////////lptkArg->tkCharIndex       =            // Filled in by the caller

            break;

        defstop
            break;
    } // End SWITCH
} // End SymGlbToToken


//***************************************************************************
//  $PrimOpDydSlash_EM_YY
//
//  Primitive operator for dyadic derived function from Slash ("N-wise reduction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydSlash_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpDydSlash_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE lpYYFcnStrOpr,       // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    return PrimOpDydSlashCommon_EM_YY (lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
                                       lpYYFcnStrOpr,   // Ptr to operator function strand
                                       lptkRhtArg,      // Ptr to right arg token
                                       lptkAxis,        // Ptr to axis token (may be NULL)
                                       NULL);           // Ptr to left operand prototype function
} // End PrimOpDydSlash_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydSlashCommon_EM_YY
//
//  Primitive operator for dyadic derived function from Slash ("N-wise reduction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydSlashCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpDydSlashCommon_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE lpYYFcnStrOpr,       // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis,            // Ptr to axis token (may be NULL)
     LPPRIMFNS lpPrimProtoLft)      // Ptr to left operand prototype function

{
    APLSTYPE  aplTypeLft,           // Left arg storage type
              aplTypeRht,           // Right ...
              aplTypeRes;           // Result   ...
    APLNELM   aplNELMLft,           // Left arg NELM
              aplNELMRht,           // Right ...
              aplNELMRes;           // Result   ...
    APLRANK   aplRankLft,           // Left arg rank
              aplRankRht,           // Right ...
              aplRankRes;           // Result   ...
    APLINT    aplIntegerLft,        // Left arg integer
              aplIntegerLftAbs,     // ...              absolute value
              apaOffRht,            // Right arg APA offset
              apaMulRht;            // ...           multiplier
    APLFLOAT  aplFloatLft,          // Left arg float
              aplFloatIdent;        // Identity element
    APLUINT   aplAxis,              // The (one and only) axis value
              uLo,                  // uDimLo loop counter
              uHi,                  // uDimHi ...
              uDim,
              uDimLo,               // Product of dimensions below axis
              uDimHi,               // ...                   above ...
              uDimRht,              // Starting index in right arg of current vector
              uDimRes,              // ...               result    ...
              uDimAxRht,            // Right arg axis dimension length
              uDimAxRes,            // Result    ...
              uRht,                 // Right arg loop counter
              uRes;                 // Result    ...
    HGLOBAL   hGlbRht = NULL,       // Right arg global memory handle
              hGlbRes = NULL;       // Result    ...
    LPVOID    lpMemRht = NULL,      // Ptr to right arg global memory
              lpMemRes = NULL;      // Ptr to result    ...
    LPAPLDIM  lpMemDimRht;          // Ptr to right arg dimensions
    BOOL      bRet = TRUE;          // TRUE iff result is valid
    LPYYSTYPE lpYYRes = NULL,       // Ptr to the result
              lpYYFcnStrLft;        // Ptr to the left operand strand
    TOKEN     tkLftArg = {0},       // Left arg token
              tkRhtArg = {0};       // Right ...

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Handle prototypes specially
    if (aplNELMRht EQ 0
     && lpPrimProtoLft EQ NULL)
    {
        // Get a ptr to the prototype function for the first symbol (a function or operator)
        lpPrimProtoLft = PrimProtoFnsTab[SymTrans (&lpYYFcnStrLft->tkToken)];
        if (!lpPrimProtoLft)
        {
            ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                      &lpYYFcnStrLft->tkToken);
            return NULL;
        } // End IF
    } // End IF

    // Check for LEFT RANK ERROR
    if (aplRankLft > 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                  &lpYYFcnStrOpr->tkToken);
        goto ERROR_EXIT;
    } // End IF

    // Check for LEFT LENGTH ERROR
    if (aplNELMLft NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                  &lpYYFcnStrOpr->tkToken);
        goto ERROR_EXIT;
    } // End IF

    // Check for LEFT DOMAIN ERROR
    if (!IsSimpleNum (aplTypeLft))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                  &lpYYFcnStrOpr->tkToken);
        goto ERROR_EXIT;
    } // End IF

    // Get the one (and only) value from the left arg
    FirstValue (lptkLftArg,             // Ptr to right arg token
               &aplIntegerLft,          // Ptr to integer result
               &aplFloatLft,            // Ptr to float ...
                NULL,                   // Ptr to WCHAR ...
                NULL,                   // Ptr to longest ...
                NULL,                   // Ptr to lpSym/Glb ...
                NULL,                   // Ptr to ...immediate type ...
                NULL);                  // Ptr to array type ...
    // If the left arg is a float,
    //   attempt to convert it to an integer
    if (aplTypeLft EQ ARRAY_FLOAT)
    {
        aplIntegerLft = FloatToAplint_SCT (aplFloatLft, &bRet);
        if (!bRet)
        {
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                      &lpYYFcnStrOpr->tkToken);
            goto ERROR_EXIT;
        } // End IF
    } // End IF

    // Calculate the absolute value of aplIntegerLft
    aplIntegerLftAbs = abs64 (aplIntegerLft);

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
        // if Slash, use last dimension
        if (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLASH)
            aplAxis = max (1, aplRankRht) - 1;
        else
            // Otherwise, it's SlashBar on the first dimension
            aplAxis = 0;
    } // End IF/ELSE

    //***************************************************************
    // Calculate product of dimensions before, at, and after the axis dimension
    //***************************************************************

    // Get right arg's global ptr
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg is not an immediate, ...
    if (lpMemRht)
    {
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

        // Calculate the product of the right arg's dimensions below the axis dimension
        uDimLo = 1;
        for (uDim = 0; uDim < aplAxis; uDim++)
            uDimLo *= lpMemDimRht[uDim];

        // Get the length of the axis dimension
        if (aplRankRht EQ 0)
            uDimAxRht = 1;
        else
            uDimAxRht = lpMemDimRht[uDim];

        // Calculate the product of the right arg's dimensions above the axis dimension
        uDimHi = 1;
        for (uDim++; uDim < aplRankRht; uDim++)
            uDimHi *= lpMemDimRht[uDim];
    } else
        uDimLo = uDimAxRht = uDimHi = 1;

    // Check for more LEFT DOMAIN ERRORs
    if (aplIntegerLftAbs > (APLINT) (1 + uDimAxRht))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                  &lpYYFcnStrOpr->tkToken);
        goto ERROR_EXIT;
    } // End IF

    // Calculate the result axis dimension
    uDimAxRes = 1 + uDimAxRht - aplIntegerLftAbs;

    // The rank of the result is the same as that of the right arg
    //   or 1 whichever is higher
    aplRankRes = max (1, aplRankRht);

    // If the left arg is zero, or
    //    the absolute value of the left arg is (uDimAxRht + 1),
    //    the result is a reshape of LeftOperandIdentityElement,
    //    so we need that value
    if (aplIntegerLft EQ 0
     || aplIntegerLftAbs EQ (APLINT) (uDimAxRht + 1))
    {
        UINT uIdent;

        // Get the index of the identity element (if any)
        uIdent = PrimFlags[SymTrans (&lpYYFcnStrLft->tkToken)].Index;

        // If it's an immediate primitive function without
        //   an identity element, signal a DOMAIN ERROR
        if (!lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
         || uIdent EQ PF_IND_UNK)
        {
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                      &lpYYFcnStrOpr->tkToken);
            goto ERROR_EXIT;
        } // End IF

        // Get the identity element
        aplFloatIdent = PrimIdent[uIdent];

        // If the prototype is Boolean, the result is too
        if (aplFloatIdent EQ 0.0
         || aplFloatIdent EQ 1.0)
            aplTypeRes = ARRAY_BOOL;
        else
            aplTypeRes = ARRAY_FLOAT;
    } else
        // Assume that the result storage type is nested
        //   but we'll call TypeDemote at the end just in case
        aplTypeRes = ARRAY_NESTED;

    // Calculate the result NELM
    aplNELMRes = imul64 (uDimLo, uDimHi, &bRet);
    if (bRet)
        aplNELMRes = imul64 (aplNELMRes, uDimAxRes, &bRet);
    if (!bRet)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                  &lpYYFcnStrOpr->tkToken);
        goto ERROR_EXIT;
    } // End IF

    // Pick off special cases of the left arg

    // If the left arg is zero, the result is
    //   ({rho} Result) {rho} LeftOperandIdentityElement
    //   where ({rho} Result) is ({rho} RightArg)
    if (aplIntegerLft EQ 0)
    {
        // Allocate storage for the result
        if (!PrimOpDydSlashAllocate_EM
             (aplTypeRht,           // Right arg storage type
              max (1, aplRankRht),  // Right arg rank
              aplTypeRes,           // Result storage type
              aplNELMRes,           // Result NELM
              aplRankRes,           // Result rank
              lpMemRht,             // Ptr to right arg global memory
              lpMemDimRht,          // Ptr to right arg dimensions
              aplAxis,              // The (one and only) axis value
              uDimAxRes,            // Result axis value dimension length

             &hGlbRes,              // Ptr to result global memory handle
             &lpMemRes,             // Ptr to ptr to result global memory
             &apaOffRht,            // Ptr to right arg APA offset
             &apaMulRht,            // ...                  multiplier
              lpYYFcnStrOpr))       // Ptr to operator function strand
            goto ERROR_EXIT;

        // Fill in the data values (i.e., replicate the identity element)

        // Check for Boolean result
        if (aplTypeRes EQ ARRAY_BOOL)
        {
            // The zero case is done (already zero from GHND)

            // Check for identity element 1
            if (aplFloatIdent EQ 1.0)
            {
                APLNELM uNELMRes;

                // Calculate the # bytes in the result, rounding up
                uNELMRes = (aplNELMRes + (NBIB - 1)) >> LOG2NBIB;

                for (uRes = 0; uRes < uNELMRes; uRes++)
                    *((LPAPLBOOL) lpMemRes)++ = 0xFF;
            } // End IF
        } else
            for (uRes = 0; uRes < aplNELMRes; uRes++)
                *((LPAPLFLOAT) lpMemRes)++ = aplFloatIdent;
    } else
    // If the absolute value of the left arg is one, the result is
    //   the right arg
    if (aplIntegerLftAbs EQ 1)
        hGlbRes = CopySymGlbDir (MakeGlbTypeGlb (hGlbRht));
    else
    // If the left arg is uDimAxRht, the result is
    //   ({rho} Result) {rho} LeftOperand /[X] RightArg
    //   where ({rho} Result) is ({rho} RightArg) with
    //   ({rho} RightArg})[X] set to one.
    if (aplIntegerLft EQ (APLINT) uDimAxRht)
    {
        // Reduce the right arg
        lpYYRes = PrimOpMonSlashCommon_EM_YY (lpYYFcnStrOpr,    // Ptr to operator function strand
                                              lptkRhtArg,       // Ptr to right arg
                                              lptkAxis,         // Ptr to axis token (may be NULL)
                                              lpPrimProtoLft);  // Ptr to left operand prototype function
        // Insert a dimension into the result
        if (!PrimOpDydSlashInsertDim_EM (lpYYRes,       // Ptr to the result
                                         aplAxis,       // The (one and only) axis value
                                         uDimAxRes,     // Result axis dimension length
                                        &hGlbRes,       // Ptr to the result global memory handle
                                         lpYYFcnStrOpr))// Ptr to operator function strand
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } else
    // If the left arg is -uDimAxRht, the result is
    //   ({rho} Result) {rho} LeftOperand /[X] {reverse}[X] RightArg
    //   where ({rho} Result) is ({rho} RightArg) with
    //   ({rho} RightArg})[X] set to one
    if (aplIntegerLft EQ -(APLINT) uDimAxRht)
    {
        LPYYSTYPE lpYYRes2;

        // Reverse the right arg along the specified axis
        lpYYRes2 = PrimFnMonCircleStile_EM_YY (&lpYYFcnStrOpr->tkToken, // Ptr to function token
                                                lptkRhtArg,             // Ptr to right arg token
                                                lptkAxis);              // Ptr to axis token (may be NULL)
        // If it failed, ...
        if (!lpYYRes2)
            goto ERROR_EXIT;

        // Reduce the reversed right arg along the specified axis
        lpYYRes = PrimOpMonSlash_EM_YY (lpYYFcnStrOpr,      // Ptr to operator function strand
                                       &lpYYRes2->tkToken,  // Ptr to right arg
                                        lptkAxis);          // Ptr to axis token (may be NULL)
        // Free the result of the function execution
        FreeResult (&lpYYRes2->tkToken); YYFree (lpYYRes2); lpYYRes2 = NULL;

        // Insert a dimension into the result
        if (!PrimOpDydSlashInsertDim_EM (lpYYRes,       // Ptr to the result
                                         aplAxis,       // The (one and only) axis value
                                         uDimAxRes,     // Result axis dimension length
                                        &hGlbRes,       // Ptr to the result global memory handle
                                         lpYYFcnStrOpr))// Ptr to operator function strand
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } else
    // If the absolute value of the left arg is (uDimAxRht + 1), the result is
    //   ({rho} result) {rho} LeftOperandIdentityElement
    //   where ({rho} Result) is ({rho} RightArg) with
    //   ({rho} RightArg})[X] set to zero
    if (aplIntegerLftAbs EQ (APLINT) (uDimAxRht + 1))
    {
        // The result is empty and the prototype is the identity element
        //   for the left operand ==> numeric ==> zero
        //   ==> the result storage type is Boolean
        aplTypeRes = ARRAY_BOOL;

        // Allocate storage for the result
        if (!PrimOpDydSlashAllocate_EM
             (aplTypeRht,           // Right arg storage type
              max (1, aplRankRht),  // Right arg rank
              aplTypeRes,           // Result storage type
              aplNELMRes,           // Result NELM
              aplRankRes,           // Result rank
              lpMemRht,             // Ptr to right arg global memory
              lpMemDimRht,          // Ptr to right arg dimensions
              aplAxis,              // The (one and only) axis value
              uDimAxRes,            // Result axis value dimension length

             &hGlbRes,              // Ptr to result global memory handle
             &lpMemRes,             // Ptr to ptr to result global memory
             &apaOffRht,            // Ptr to right arg APA offset
             &apaMulRht,            // ...                  multiplier
              lpYYFcnStrOpr))       // Ptr to operator function strand
            goto ERROR_EXIT;
    } else
    {
        // Otherwise, we're out of special cases

        // Allocate storage for the result
        if (!PrimOpDydSlashAllocate_EM
             (aplTypeRht,           // Right arg storage type
              max (1, aplRankRht),  // Right arg rank
              aplTypeRes,           // Result storage type
              aplNELMRes,           // Result NELM
              aplRankRes,           // Result rank
              lpMemRht,             // Ptr to right arg global memory
              lpMemDimRht,          // Ptr to right arg dimensions
              aplAxis,              // The (one and only) axis value
              uDimAxRes,            // Result axis value dimension length

             &hGlbRes,              // Ptr to result global memory handle
             &lpMemRes,             // Ptr to ptr to result global memory
             &apaOffRht,            // Ptr to right arg APA offset
             &apaMulRht,            // ...                  multiplier
              lpYYFcnStrOpr))       // Ptr to operator function strand
            goto ERROR_EXIT;

        // Fill in the right arg token
////////tkRhtArg.tkFlags.TknType   =            // To be filled in below
////////tkRhtArg.tkFlags.ImmType   =            // To be filled in below
////////tkRhtArg.tkFlags.NoDisplay = 0;         // Already zero from {0}
////////tkRhtArg.tkData.tkGlbData  =            // To be filled in below
        tkRhtArg.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

        // Fill in the left arg token
////////tkLftArg.tkFlags.TknType   =            // To be filled in below
////////tkLftArg.tkFlags.ImmType   =            // To be filled in below
////////tkLftArg.tkFlags.NoDisplay = 0;         // Already zero from {0}
////////tkLftArg.tkData.tkGlbData  =            // To be filled in below
        tkLftArg.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

        // Loop through the right arg calling the
        //   function strand between data, storing in the
        //   result.
        for (uLo = 0; uLo < uDimLo; uLo++)
        for (uHi = 0; uHi < uDimHi; uHi++)
        {
            APLUINT uAx;            // Loop counter for uDimAxRes
            APLINT  iDim;           // Loop counter for aplIntegerLftAbs

            // Calculate the starting index in the right arg/result of this vector
            uDimRht = uLo * uDimHi * uDimAxRht + uHi;
            uDimRes = uLo * uDimHi * uDimAxRes + uHi;

            // Calculate the index in the right arg of the last element in this vector
            uRht = uDimRht + (aplIntegerLftAbs - 1) * uDimHi;

            // There are uDimAxRes elements in the result to be generated now
            for (uAx = 0; uAx < uDimAxRes; uAx++)
            {
                // Reduce the aplIntegerLftAbs values starting at
                //   uRht = uDimRht + uAx * uDimHi;
                // through
                //   uRht = uDimRht + (uAx + aplIntegerLftAbs - 1) * uDimHi;

                // Split cases based upon the sign of the left arg
                if (aplIntegerLft > 0)
                {
                    // Calculate the index of last element in this vector
                    uRht = uDimRht + (uAx + aplIntegerLftAbs - 1) * uDimHi;

                    // Get the last element as the right arg
                    GetValueInToken (uRht,              // Index to use
                                     lpMemRht,          // Ptr to global memory object to index
                                     aplTypeRht,        // Storage type of the arg
                                     apaOffRht,         // APA offset (if needed)
                                     apaMulRht,         // APA multiplier (if needed)
                                    &tkRhtArg);         // Ptr to token in which to place the value
                    // Loop backwards through the elements along the specified axis
                    for (iDim = aplIntegerLftAbs - 2; iDim >= 0; iDim--)
                    {
                        // Calculate the index of the previous element in this vector
                        uRht = uDimRht + (uAx + iDim) * uDimHi;

                        // Get the previous element as the left arg
                        GetValueInToken (uRht,          // Index to use
                                         lpMemRht,      // Ptr to global memory object to index
                                         aplTypeRht,    // Storage type of the arg
                                         apaOffRht,     // APA offset (if needed)
                                         apaMulRht,     // APA multiplier (if needed)
                                        &tkLftArg);     // Ptr to token in which to place the value
                        // Execute the left operand between the left & right args
                        lpYYRes = ExecFuncStr_EM_YY (&tkLftArg,         // Ptr to left arg token
                                                      lpYYFcnStrLft,    // Ptr to function strand
                                                     &tkRhtArg);        // Ptr to right arg token
                        // Free the left & right arg tokens
                        FreeResult (&tkRhtArg);
                        FreeResult (&tkLftArg);

                        // If it succeeded, ...
                        if (lpYYRes)
                        {
                            // Copy the result to the right arg token
                            tkRhtArg = *CopyToken_EM (&lpYYRes->tkToken, FALSE);

                            // Free the YYRes
                            YYFree (lpYYRes); lpYYRes = NULL;
                        } else
                            goto ERROR_EXIT;
                    } // End FOR
                } else
                {
                    // Calculate the index of first element in this vector
                    uRht = uDimRht + uAx * uDimHi;

                    // Get the first element as the right arg
                    GetValueInToken (uRht,              // Index to use
                                     lpMemRht,          // Ptr to global memory object to index
                                     aplTypeRht,        // Storage type of the arg
                                     apaOffRht,         // APA offset (if needed)
                                     apaMulRht,         // APA multiplier (if needed)
                                    &tkRhtArg);         // Ptr to token in which to place the value
                    // Loop forwards through the elements along the specified axis
                    for (iDim = 1; iDim < aplIntegerLftAbs; iDim++)
                    {
                        // Calculate the index of the next element in this vector
                        uRht = uDimRht + (uAx + iDim) * uDimHi;

                        // Get the next element as the left arg
                        GetValueInToken (uRht,          // Index to use
                                         lpMemRht,      // Ptr to global memory object to index
                                         aplTypeRht,    // Storage type of the arg
                                         apaOffRht,     // APA offset (if needed)
                                         apaMulRht,     // APA multiplier (if needed)
                                        &tkLftArg);     // Ptr to token in which to place the value
                        // Execute the left operand between the left & right args
                        lpYYRes = ExecFuncStr_EM_YY (&tkLftArg,         // Ptr to left arg token
                                                      lpYYFcnStrLft,    // Ptr to function strand
                                                     &tkRhtArg);        // Ptr to right arg token
                        // Free the left & right arg tokens
                        FreeResult (&tkRhtArg);
                        FreeResult (&tkLftArg);

                        // If it succeeded, ...
                        if (lpYYRes)
                        {
                            // Copy the result to the right arg token
                            tkRhtArg = *CopyToken_EM (&lpYYRes->tkToken, FALSE);

                            // Free the YYRes
                            YYFree (lpYYRes); lpYYRes = NULL;
                        } else
                            goto ERROR_EXIT;
                    } // End FOR
                } // End IF/ELSE

                // Split cases based upon the token type of the right arg (result)
                switch (tkRhtArg.tkFlags.TknType)
                {
                    case TKT_VARIMMED:
                        // Save in the result as an LPSYMENTRY
                        *((LPAPLNESTED) lpMemRes)++ = MakeSymEntry_EM (tkRhtArg.tkFlags.ImmType,
                                                                      &tkRhtArg.tkData.tkLongest,
                                                                      &lpYYFcnStrOpr->tkToken);
                        break;

                    case TKT_VARARRAY:
                        // Save in the result as an HGLOBAL
                        *((LPAPLNESTED) lpMemRes)++ = tkRhtArg.tkData.tkGlbData;

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR
        } // End FOR/FOR
    } // End IF/ELSE/.../FOR/FOR

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    lpYYRes->tkToken = *TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
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

    return lpYYRes;
} // End PrimOpDydSlashCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydSlashInsertDim_EM
//
//  Insert a new coordinate into a result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydSlashInsertDim_EM_YY"
#else
#define APPEND_NAME
#endif

BOOL PrimOpDydSlashInsertDim_EM
    (LPYYSTYPE lpYYRes,         // Ptr to the result
     APLUINT   aplAxis,         // The (one and only) axis value
     APLUINT   uDimAxRes,       // Result axis dimension length
     HGLOBAL  *lphGlbRes,       // Ptr to result global memory handle
     LPYYSTYPE lpYYFcnStrOpr)   // Ptr to operator function strand

{
    HGLOBAL hGlbTmp;            // Temporary global memory handle
    APLUINT ByteRes;            // # bytes needed for the result
    LPVOID  lpMemRes;           // Ptr to result global memory

    // If it failed, ...
    if (!lpYYRes)
        return FALSE;

    // Get the global memory handle
    hGlbTmp = lpYYRes->tkToken.tkData.tkGlbData;

    Assert (IsGlbTypeVarDir (hGlbTmp));

    // Clear the ptr bits
    hGlbTmp = ClrPtrTypeDirGlb (hGlbTmp);

    // The result is the same as lpYYRes except we need
    //   to insert a unit dimension between aplAxis and
    //   aplAxis + 1.

    // Get the size of the temporary global memory
    ByteRes = MyGlobalSize (hGlbTmp);

    // Resize the global memory to include a new dimension
    *lphGlbRes = MyGlobalReAlloc (hGlbTmp,
                                  (UINT) (ByteRes + sizeof (APLDIM)),
                                  GHND);
    // Check for errors
    if (*lphGlbRes EQ NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                  &lpYYFcnStrOpr->tkToken);
        return FALSE;
    } // End IF

    // If the handle is moveable, GlobalReAlloc returns the same handle
    Assert (hGlbTmp EQ *lphGlbRes);

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (*lphGlbRes);

    // Move the memory upwards to make room for a new dimension
    // Use MoveMemory as the source and destin blocks overlap
    MoveMemory (((LPBYTE) lpMemRes) + sizeof (VARARRAY_HEADER) + sizeof (APLDIM) * (aplAxis + 1),
                ((LPBYTE) lpMemRes) + sizeof (VARARRAY_HEADER) + sizeof (APLDIM) *  aplAxis     ,
                (UINT) (ByteRes    - (sizeof (VARARRAY_HEADER) + sizeof (APLDIM) *  aplAxis     )));
    // Increase the rank by one
    ((LPVARARRAY_HEADER) lpMemRes)->Rank++;

    // Insert the new dimension
    (VarArrayBaseToDim (lpMemRes))[aplAxis] = uDimAxRes;

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;

    return TRUE;
} // End PrimOpDydSlashInsertDim_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydSlashAllocate_EM
//
//  Allocate storage for the result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydSlashAllocate_EM_YY"
#else
#define APPEND_NAME
#endif

BOOL PrimOpDydSlashAllocate_EM
    (APLRANK   aplTypeRht,      // Right arg storage type
     APLRANK   aplRankRht,      // Right arg rank
     APLSTYPE  aplTypeRes,      // Result storage type
     APLNELM   aplNELMRes,      // Result NELM
     APLRANK   aplRankRes,      // Result rank
     LPVOID    lpMemRht,        // Ptr to right arg global memory
     LPAPLDIM  lpMemDimRht,     // Ptr to right arg dimensions
     APLUINT   aplAxis,         // Axis value
     APLUINT   uDimAxRes,       // Result axis value

     HGLOBAL  *lphGlbRes,       // Ptr to result global memory handle
     LPVOID   *lplpMemRes,      // Ptr to ptr to result global memory
     LPAPLINT  lpapaOffRht,     // Ptr to right arg APA offset
     LPAPLINT  lpapaMulRht,     // ...                  multiplier
     LPYYSTYPE lpYYFcnStrOpr)   // Ptr to operator function strand

{
    APLUINT  ByteRes,           // # bytes needed for the result
             uRes,              // Result loop counter
             uDim;              // Dimension loop counter

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Allocate space for the result
    // N.B. Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    *lphGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!*lphGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                  &lpYYFcnStrOpr->tkToken);
        return FALSE;
    } // End IF

    // Lock the memory to get a ptr to it
    *lplpMemRes = MyGlobalLock (*lphGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) *lplpMemRes)

    // Fill in the header values
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;       // Already zero from GHND
////lpHeader->SysVar     = 0;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;

#undef  lpHeader

    // Skip over the header to the dimensions
    *lplpMemRes = VarArrayBaseToDim (*lplpMemRes);

    // Copy the dimensions from the right arg to the result
    //   except for the axis dimension
    for (uDim = 0; uDim < aplRankRht; uDim++)
    if (uDim EQ aplAxis)
        *((LPAPLDIM) *lplpMemRes)++ = uDimAxRes;
    else
        *((LPAPLDIM) *lplpMemRes)++ = lpMemDimRht[uDim];

    // lpMemRes now points to its data

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Get APA parameters
    if (aplTypeRht EQ ARRAY_APA)
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        *lpapaOffRht = lpAPA->Off;
        *lpapaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    if (aplTypeRes EQ ARRAY_NESTED)
    {
        // Fill nested result with PTR_REUSED
        //   in case we fail part way through
        *((LPAPLNESTED) *lplpMemRes) = PTR_REUSED;
        for (uRes = 1; uRes < aplNELMRes; uRes++)
            ((LPAPLNESTED) *lplpMemRes)[uRes] = PTR_REUSED;
    } // End IF

    return TRUE;
} // End PrimOpDydSlashAllocate_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_slash.c
//***************************************************************************
