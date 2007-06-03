//***************************************************************************
//  NARS2000 -- Primitive Operator -- JotDot
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


//***************************************************************************
//  $PrimOpJotDot_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from JotDot ("ERROR" and "outer product")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpJotDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpJotDot_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    Assert (lpYYFcnStr->tkToken.tkData.tkChar EQ INDEX_JOTDOT);

    //***************************************************************
    // The derived functions from this operator are not sensitive to
    //   the axis operator, so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis);
        return NULL;
    } // End IF

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonJotDot_EM_YY (lpYYFcnStr,   // Ptr to operator function strand
                                      lptkRhtArg,   // Ptr to right arg token
                                      lptkAxis);    // Ptr to axis token (may be NULL)
    else
        return PrimOpDydJotDot_EM_YY (lptkLftArg,   // Ptr to left arg token
                                      lpYYFcnStr,   // Ptr to operator function strand
                                      lptkRhtArg,   // Ptr to right arg token
                                      lptkAxis);    // Ptr to axis token (may be NULL)
} // End PrimOpJotDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOpJotDot_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator JotDot ("ERROR" and "outer product")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoOpJotDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimProtoOpJotDot_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    LPPRIMFNS lpPrimProtoRht;       // Ptr to right operand prototype function
    LPYYSTYPE lpYYFcnStrRht;        // Ptr to right operand function strand

    //***************************************************************
    // The derived functions from this operator are not sensitive to
    //   the axis operator, so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis);
        return NULL;
    } // End IF

    // Set ptr to right operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = &lpYYFcnStr[1 + (lptkAxis NE NULL)];

    // Get a ptr to the prototype function for the first symbol (a function or operator)
    lpPrimProtoRht = PrimProtoFnsTab[SymTrans (&lpYYFcnStrRht->tkToken)];
    if (!lpPrimProtoRht)
    {
        ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                  &lpYYFcnStrRht->tkToken);
        return NULL;
    } // End IF

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return PrimFnSyntaxError_EM (&lpYYFcnStr->tkToken);
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return PrimOpDydJotDotCommon_EM_YY (lptkLftArg,         // Ptr to left arg token
                                            lpYYFcnStr,         // Ptr to operator function strand
                                            lptkRhtArg,         // Ptr to right arg token
                                            lptkAxis,           // Ptr to axis token (may be NULL)
                                            lpPrimProtoRht);    // Ptr to right operand prototype function
} // End PrimProtoOpJotDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpMonJotDot_EM_YY
//
//  Primitive operator for monadic derived function from JotDot ("ERROR")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonJotDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpMonJotDot_EM_YY
    (LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    return PrimFnSyntaxError_EM (&lpYYFcnStr->tkToken);
} // End PrimOpMonJotDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydJotDot_EM_YY
//
//  Primitive operator for dyadic derived function from JotDot ("outer product")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydJotDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpDydJotDot_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    return PrimOpDydJotDotCommon_EM_YY (lptkLftArg,     // Ptr to left arg token
                                        lpYYFcnStr,     // Ptr to operator function strand
                                        lptkRhtArg,     // Ptr to right arg token
                                        lptkAxis,       // Ptr to axis token (may be NULL)
                                        NULL);          // Ptr to right operand prototype function
} // End PrimOpDydJotDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydJotDotCommon_EM_YY
//
//  Primitive operator for dyadic derived function from JotDot ("outer product")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydJotDotCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpDydJotDotCommon_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis,            // Ptr to axis token (may be NULL)
     LPPRIMFNS lpPrimProtoRht)      // Ptr to right operand prototype function

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
    APLUINT   ByteRes,              // # bytes needed for the result
              uLft,                 // Left arg loop counter
              uRht,                 // Right ...
              uRes;                 // Result   ...
    HGLOBAL   hGlbLft = NULL,       // Left arg global memory handle
              hGlbRht = NULL,       // Right ...
              hGlbRes = NULL;       // Result   ...
    LPVOID    lpMemLft = NULL,      // Ptr to left arg global memory
              lpMemRht = NULL,      // Ptr to right ...
              lpMemRes = NULL;      // Ptr to result   ...
    BOOL      bRet = TRUE;          // TRUE iff result is valid
    TOKEN     tkLftArg = {0},       // Left arg token
              tkRhtArg = {0};       // Right ...
    UCHAR     immType;              // Immediate type
    APLINT    apaOffLft,            // Left arg APA offset
              apaMulLft,            // ...          multiplier
              apaOffRht,            // Right arg APA offset
              apaMulRht;            // ...           multiplier
    LPYYSTYPE lpYYFcnStrRht,        // Ptr to right operand function strand
              lpYYRes = NULL;       // Ptr to the result

    // Set ptr to right operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = &lpYYFcnStr[1 + (lptkAxis NE NULL)];

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // The result NELM is the product of the left & right NELMs
    aplNELMRes = imul64 (aplNELMLft, aplNELMRht, &bRet);
    if (!bRet)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                  &lpYYFcnStr->tkToken);
        return NULL;
    } // End IF

    // Handle prototypes separately
    if (aplNELMRes EQ 0
     || lpPrimProtoRht EQ NULL)
    {
        // Get a ptr to the prototype function for the first symbol (a function or operator)
        lpPrimProtoRht = PrimProtoFnsTab[SymTrans (&lpYYFcnStrRht->tkToken)];
        if (!lpPrimProtoRht)
        {
            ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                      &lpYYFcnStrRht->tkToken);
            goto ERROR_EXIT;
        } // End IF
    } // End IF

    // The rank of the result is the sum of the left & right ranks
    aplRankRes = aplRankLft + aplRankRht;

    // The result storage type is assumed to be NESTED,
    //   but we'll call TypeDemote at the end just in case.
    aplTypeRes = ARRAY_NESTED;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Allocate space for the result.
    // N.B. Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                  &lpYYFcnStr->tkToken);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;   // Already zero from GHND
////lpHeader->SysVar     = 0;   // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;

#undef  lpHeader

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);
    if (lpMemLft)
        lpMemLft = VarArrayBaseToDim (lpMemLft);
    if (lpMemRht)
        lpMemRht = VarArrayBaseToDim (lpMemRht);

    //***************************************************************
    // Copy the dimensions from the left arg
    //   to the result's dimension
    //***************************************************************
    if (lpMemLft)
    for (uRes = 0; uRes < aplRankLft; uRes++)
        *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemLft)++;

    //***************************************************************
    // Copy the dimensions from the right arg
    //   to the result's dimension
    //***************************************************************
    if (lpMemRht)
    for (uRes = 0; uRes < aplRankRht; uRes++)
        *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;

    // lpMemRes now points to the result's data
    // lpMemLft now points to the left arg's data
    // lpMemRht now points to the right arg's data

    // Fill nested result with PTR_REUSED
    //   in case we fail part way through
    *((LPAPLNESTED) lpMemRes) = PTR_REUSED;
    for (uRes = 1; uRes < aplNELMRes; uRes++)
        ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;

    // Fill in the arg tokens
    tkLftArg.tkCharIndex =
    tkRhtArg.tkCharIndex = lpYYFcnStr->tkToken.tkCharIndex;

    // If the left arg is immediate, fill in the token
    if (lpMemLft EQ NULL)
    {
        tkLftArg.tkFlags.TknType = TKT_VARIMMED;
        FirstValue (lptkLftArg,                 // Ptr to right arg token
                    NULL,                       // Ptr to integer result
                    NULL,                       // Ptr to float ...
                    NULL,                       // Ptr to WCHAR ...
                   &tkLftArg.tkData.tkLongest,  // Ptr to longest ...
                    NULL,                       // Ptr to lpSym/Glb ...
                   &immType,                    // Ptr to ...immediate type ...
                    NULL);                      // Ptr to array type ...
        tkLftArg.tkFlags.ImmType = immType;
    } // End IF

    // If the right arg is immediate, fill in the token
    if (lpMemRht EQ NULL)
    {
        tkRhtArg.tkFlags.TknType = TKT_VARIMMED;
        FirstValue (lptkRhtArg,                 // Ptr to right arg token
                    NULL,                       // Ptr to integer result
                    NULL,                       // Ptr to float ...
                    NULL,                       // Ptr to WCHAR ...
                   &tkRhtArg.tkData.tkLongest,  // Ptr to longest ...
                    NULL,                       // Ptr to lpSym/Glb ...
                   &immType,                    // Ptr to ...immediate type ...
                    NULL);                      // Ptr to array type ...
        tkRhtArg.tkFlags.ImmType = immType;
    } // End IF

    // If the left arg is APA, fill in the offset and multiplier
    if (aplTypeLft EQ ARRAY_APA)
    {
#define lpAPA       ((LPAPLAPA) lpMemLft)
        apaOffLft = lpAPA->Off;
        apaMulLft = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // If the right arg is APA, fill in the offset and multiplier
    if (aplTypeRht EQ ARRAY_APA)
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Take into account the nested prototype
    if (aplTypeRht EQ ARRAY_NESTED)
        aplNELMRht = max (aplNELMRht, 1);
    if (aplTypeLft EQ ARRAY_NESTED)
        aplNELMLft = max (aplNELMLft, 1);

    // Loop through the left & right args
    for (uLft = uRes = 0; uLft < aplNELMLft; uLft++)
    for (uRht = 0;        uRht < aplNELMRht; uRht++, uRes++)
    {
        // If the left arg is not immediate, get the next value
        if (lpMemLft)
            // Get the next value from the left arg
            GetValueInToken (uLft,                  // Index to use
                             lpMemLft,              // Ptr to global memory object to i
                             aplTypeLft,            // Storage type of the arg
                             apaOffLft,             // APA offset (if needed)
                             apaMulLft,             // APA multiplier (if needed)
                            &tkLftArg);             // Ptr to token in which to place t
        // If the right arg is not immediate, get the next value
        if (lpMemRht)
            // Get the next value from the right arg
            GetValueInToken (uRht,                  // Index to use
                             lpMemRht,              // Ptr to global memory object to i
                             aplTypeRht,            // Storage type of the arg
                             apaOffRht,             // APA offset (if needed)
                             apaMulRht,             // APA multiplier (if needed)
                            &tkRhtArg);             // Ptr to token in which to place t
        if (!ExecFuncOnToken_EM (&lpMemRes,         // Ptr to output storage
                                 &tkLftArg,         // Ptr to left arg token
                                  lpYYFcnStrRht,    // Ptr to function strand
                                 &tkRhtArg,         // Ptr to right arg token
                                  NULL,             // Ptr to axis token
                                  NULL))            // Ptr to left operand prototype function
            goto ERROR_EXIT;

        // Free the left & right arg tokens
        if (lpMemLft)
            FreeResult (&tkRhtArg);
        if (lpMemRht)
            FreeResult (&tkLftArg);
    } // End FOR/FOR

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStr->tkToken.tkCharIndex;

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

    return lpYYRes;
} // End PrimOpDydJotDotCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_jotdot.c
//***************************************************************************
