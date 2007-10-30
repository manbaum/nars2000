//***************************************************************************
//  NARS2000 -- System Function -- Quad DR
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $SysFnDR_EM_YY
//
//  System function:  []DR -- Data Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDR_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDR_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Note that we don't test for TKT_LIST/ARRAY_LIST
    //   as this function processes that element.

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis);
        return NULL;
    } // End IF

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonDR_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydDR_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnDR_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonDR_EM_YY
//
//  Monadic []DR -- Display the Data Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonDR_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonDR_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeRht;        // Right arg storage type
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkData.tkInteger  =   (filled in below)
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

#define DR_SHOW             0   // Return a character vector representation
#define DR_DISPFLOAT        1   // Return a character representation of a float
#define DR_BOOL           100   //  1 bit  per value
#define DR_CHAR          1601   // 16 bits ...
#define DR_INT           6402   // 64 ...
#define DR_FLOAT         6403   // 64 ...
#define DR_APA           6404   // 64 ... offset & multiplier
#define DR_COMPLEX       6405   // 64 ... real & imaginary
#define DR_QUATERNIONS   6406   // 64 ... real & ...
#define DR_OCTONIONS     6407   // 64 ... real & ...
#define DR_HETERO32      3208   // 32 ... per item
#define DR_HETERO64      6409   // 64 ... ...
#define DR_NESTED32      3210   // 32 ... ...
#define DR_NESTED64      6411   // 64 ... ...
#define DR_LIST32        3212   // 32 ... ...
#define DR_LIST64        6413   // 64 ... ...
#define DR_RATIONAL        14   // ??

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, NULL, NULL, NULL);

    // Set the datatype
    // Split cases baed upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            lpYYRes->tkToken.tkData.tkInteger = DR_BOOL;

            break;

        case ARRAY_INT:
            lpYYRes->tkToken.tkData.tkInteger = DR_INT;

            break;

        case ARRAY_FLOAT:
            lpYYRes->tkToken.tkData.tkInteger = DR_FLOAT;

            break;

        case ARRAY_CHAR:
            lpYYRes->tkToken.tkData.tkInteger = DR_CHAR;

            break;

        case ARRAY_APA:
            lpYYRes->tkToken.tkData.tkInteger = DR_APA;

            break;

        case ARRAY_HETERO:
            lpYYRes->tkToken.tkData.tkInteger = DR_HETERO32;

            break;

        case ARRAY_NESTED:
            lpYYRes->tkToken.tkData.tkInteger = DR_NESTED32;

            break;

        case ARRAY_LIST:
            lpYYRes->tkToken.tkData.tkInteger = DR_LIST32;

            break;

        defstop
            break;
    } // End SWITCH

    return lpYYRes;
} // End SysFnMonDR_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydDR_EM_YY
//
//  Dyadic []DR -- Convert To A Data Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydDR_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydDR_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeLft;        // Left arg storage type
    APLNELM      aplNELMLft;        // Left arg NELM
    APLRANK      aplRankLft;        // Left arg rank
    HGLOBAL      hGlbRes = NULL;    // Result global memory handle
    APLINT       aplIntegerLft;     // Left arg as integer
    APLFLOAT     aplFloatLft;       // Left arg as float
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Get the attributes (Type, NELM, and Rank)
    //   of the left arg
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);

    // Check for LEFT RANK ERROR
    if (aplRankLft > 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Check for LEFT LENGTH ERROR
    if (aplNELMLft NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Check for LEFT DOMAIN ERROR
    if (!IsSimpleNum (aplTypeLft))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Get the first (and only) value
    FirstValue (lptkLftArg,         // Ptr to left arg token
               &aplIntegerLft,      // Ptr to integer result
               &aplFloatLft,        // Ptr to float ...
                NULL,               // Ptr to WCHAR ...
                NULL,               // Ptr to longest ...
                NULL,               // Ptr to lpSym/Glb ...
                NULL,               // Ptr to ...immediate type ...
                NULL);              // Ptr to array type ...

    // If it's a float, ...
    if (aplTypeLft EQ ARRAY_FLOAT)
    {
        BOOL bRet;

        // Attempt to convert the float to an integer
        aplIntegerLft = FloatToAplint_SCT (aplFloatLft, &bRet);
        if (!bRet)
        {
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                       lptkFunc);
            return NULL;
        } // End IF
    } // End IF

    // Ensure the left arg is valid
    switch (aplIntegerLft)
    {
        case DR_SHOW:
            // Return a character representation
            return SysFnDydDR_SHOW_EM_YY (lptkRhtArg, lptkFunc);

        case DR_DISPFLOAT:
            hGlbRes = SysFnDydDR_FloatToChar_EM (lptkRhtArg, lptkFunc);

            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

            return lpYYRes;

        case DR_BOOL:
            return SysFnDydDR_Convert_EM_YY (ARRAY_BOOL,  lptkRhtArg, lptkFunc);

        case DR_CHAR:
            return SysFnDydDR_Convert_EM_YY (ARRAY_CHAR,  lptkRhtArg, lptkFunc);

        case DR_INT:
            return SysFnDydDR_Convert_EM_YY (ARRAY_INT,   lptkRhtArg, lptkFunc);

        case DR_FLOAT:
            return SysFnDydDR_Convert_EM_YY (ARRAY_FLOAT, lptkRhtArg, lptkFunc);

        case DR_APA:
        case DR_HETERO32:
        case DR_HETERO64:
        case DR_NESTED32:
        case DR_NESTED64:
        case DR_LIST32:
        case DR_LIST64:
        case DR_RATIONAL:
        default:
            return PrimFnDomainError_EM (lptkFunc);

        case DR_COMPLEX:
        case DR_QUATERNIONS:
        case DR_OCTONIONS:
            return PrimFnNonceError_EM (lptkFunc);
    } // End SWITCH
} // End SysFnDydDR_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydDR_Convert_EM_YY
//
//  Return the <aplTypeRes> representation of the right arg
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydDR_Convert_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydDR_Convert_EM_YY
    (APLSTYPE aplTypeRes,           // Result storage type
     LPTOKEN  lptkRhtArg,           // Ptr to right arg token
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    HGLOBAL      hGlbRht = NULL,    // Right arg global memory handle
                 hGlbRes = NULL;    // Result    ...
    LPVOID       lpMemRht = NULL,   // Ptr to right arg global memory
                 lpMemRes = NULL;   // Ptr to result    ...
    APLSTYPE     aplTypeRht;        // Right arg storage type
    APLNELM      aplNELMRht,        // Right arg NELM
                 aplNELMRes;        // Result    ...
    APLRANK      aplRankRht,        // Right arg rank
                 aplRankRes;        // Result rank
    APLUINT      ByteRes,           // # bytes in the result
                 aplColsRht,        // Right arg # cols
                 aplColsRes;        // Result    # cols
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    APLLONGEST   aplLongestRht;     // Right arg as immediate

    // The result is an array of the same shape as R with
    //   the last coordinate multiplied by the # bits per
    //   right arg element divided by the bits per result element.

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, &aplColsRht);

    // Check for RIGHT DOMAIN ERROR
    if (!IsSimpleNH (aplTypeRht))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Handle empty right args & result
    if (aplNELMRht EQ 0)
        aplTypeRes = ARRAY_BOOL;

    // Calculate the result cols
    aplColsRes = SysFnDydDR_GetCols_EM (aplTypeRes, aplTypeRht, aplColsRht, lptkFunc);
    if (aplColsRes EQ NEG1U)
        goto ERROR_EXIT;

    // Check on right arg APA
    if (aplTypeRht EQ ARRAY_APA)
    {
        // Get the # bits per element for the result
        aplNELMRes = (2 * 64) / SysFnDydDR_BPE (aplTypeRes);

        // Set the # result cols
        aplColsRes = aplNELMRes;
    } else
    if (aplColsRht NE 0)
        aplNELMRes = aplColsRes * (aplNELMRht / aplColsRht);
    else
        aplNELMRes = 0;

    // Calculate the result rank
    if (aplTypeRht EQ ARRAY_APA)
        aplRankRes = 1;
    else
        aplRankRes = max (aplRankRht, 1);

    // Calculate space in the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Allocate space for the result
    // N.B.:  Conversion from APLUINT to UINT
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes     ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeaderRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeaderRes->ArrType    = aplTypeRes;
////lpHeaderRes->Perm       = 0;        // Already zero from GHND
////lpHeaderRes->SysVar     = 0;        // Already zero from GHND
    lpHeaderRes->RefCnt     = 1;
    lpHeaderRes->NELM       = aplNELMRes;
    lpHeaderRes->Rank       = aplRankRes;
#undef  lpHeaderRes

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg is a global, ...
    if (hGlbRht)
    {
        // Copy the right arg dimensions to the result unless APA
        if (aplTypeRht NE ARRAY_APA)
            CopyMemory (VarArrayBaseToDim (lpMemRes),
                        VarArrayBaseToDim (lpMemRht),
                        (UINT) (aplRankRes - 1) * sizeof (APLDIM));
        // Save the last dimension
        (VarArrayBaseToDim (lpMemRes))[aplRankRes - 1] = aplColsRes;

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

        // Copy the right arg to the result
        CopyMemory (lpMemRes, lpMemRht, (UINT) BytesIn (aplTypeRht, aplNELMRht));
    } else
    {
        // Save the last dimension
        *VarArrayBaseToDim (lpMemRes) = aplColsRes;

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

        // Copy the right arg to the result
        *((LPAPLLONGEST) lpMemRes) = aplLongestRht;
    } // End IF/ELSE

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
ERROR_EXIT:
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
} // End SysFnDydDR_Convert_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydDR_BPE
//
//  Return the # bits per element for a given storage type
//***************************************************************************

UINT SysFnDydDR_BPE
    (APLSTYPE aplTypeRht)           // Right arg storage type

{
    static UINT uBPE[] = {BPE_VEC};

    return uBPE[aplTypeRht];
} // End SysFnDydDR_BPE


//***************************************************************************
//  $BytesIn
//
//  Return the # bytes in an array of a given type and NELM
//***************************************************************************

APLUINT BytesIn
    (APLSTYPE aplTypeRht,               // Right arg storage type
     APLNELM  aplNELMRht)               // Right arg NELM

{
    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:                // 1 bit per element
            return RoundUpBits8 (aplNELMRht);

        case ARRAY_INT:                 // 8 bytes per element
        case ARRAY_FLOAT:
            return aplNELMRht * sizeof (APLINT);

        case ARRAY_CHAR:                // 2 byte per element
            return aplNELMRht * sizeof (APLCHAR);

        case ARRAY_APA:                 // 2 8-byte elements
            return sizeof (APLAPA);

        case ARRAY_HETERO:              // 4 bytes per element
            return aplNELMRht * sizeof (APLHETERO);

        case ARRAY_NESTED:              // 4 bytes per element
            return aplNELMRht * sizeof (APLNESTED);

        defstop
            return 0;
    } // End SWITCH
} // End BytesIn


//***************************************************************************
//  $SysFnDydDR_GetCols_EM
//
//  Return the # result cols for a given right arg storage type & cols
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydDR_GetCols_EM"
#else
#define APPEND_NAME
#endif

APLNELM SysFnDydDR_GetCols_EM
    (APLSTYPE aplTypeRes,               // Result storage type
     APLSTYPE aplTypeRht,               // Right arg storage type
     APLUINT  aplColsRht,               // Right arg # cols
     LPTOKEN  lptkFunc)                 // Ptr to function token

{
    UINT uBPERht,                       // # bits per element for the right arg
         uBPERes;                       // # bits per element for the result

    // Get the # bits per element for the right arg & result
    uBPERht = SysFnDydDR_BPE (aplTypeRht);
    uBPERes = SysFnDydDR_BPE (aplTypeRes);

    if (uBPERht NE 0 && uBPERes NE 0)
    {
        // Calculate # bits in the right arg cols
        aplColsRht *= uBPERht;

        // Ensure the right arg BPE is a multiple of the result BPE
        if (0 NE (aplColsRht % uBPERes))
        {
            ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                       lptkFunc);
            return NEG1U;
        } // End IF

        // Return # result cols
        return (aplColsRht / uBPERes);
    } else
        return 0;
} // End SysFnDydDR_GetCols_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydDR_SHOW_EM_YY
//
//  Return a character representation of the storage type
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydDR_SHOW_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydDR_SHOW_EM_YY
    (LPTOKEN  lptkRhtArg,
     LPTOKEN  lptkFunc)

{
    APLSTYPE     aplTypeRht;    // Right arg storage type
    APLUINT      ByteRes;       // # bytes in the result
    HGLOBAL      hGlbRes;       // Result global memory handle
    LPVOID       lpMemRes;      // Ptr to result global memory
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result
    APLNELM      aplNELMRes;    // Result NELM
    APLCHAR      wszTemp[128];  // Temporary APLCHARs

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, NULL, NULL, NULL);

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            wsprintfW (wszTemp,
                      L"Boolean (%d):  1 bit per element",
                       DR_BOOL);
            break;

        case ARRAY_INT:
            wsprintfW (wszTemp,
                      L"Integer (%d):  64 bits per element",
                       DR_INT);
            break;

        case ARRAY_FLOAT:
            wsprintfW (wszTemp,
                      L"Floating Point (%d):  64 bits per element",
                       DR_FLOAT);
            break;

        case ARRAY_CHAR:
            wsprintfW (wszTemp,
                      L"Character (%d):  16 bits per element",
                       DR_CHAR);
            break;

        case ARRAY_APA:
            wsprintfW (wszTemp,
                      L"Arithmetic Progression Array (%d):  64 bit offset + 64 bit multiplier",
                      DR_APA);
            break;

        case ARRAY_LIST:
            wsprintfW (wszTemp,
                      L"List (%d):  32 bits per element",
                      DR_LIST32);
            break;

        case ARRAY_NESTED:
            wsprintfW (wszTemp,
                      L"Nested Array (%d):  32 bits per element",
                      DR_NESTED32);
            break;

        case ARRAY_HETERO:
            wsprintfW (wszTemp,
                      L"Heterogeneous Array (%d):  32 bits per element",
                      DR_HETERO32);
            break;

        defstop
            break;
    } // End SWITCH

    // Get the result NELM
    aplNELMRes = lstrlenW (wszTemp);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, 1);

    // Allocate space for the result
    // N.B.:  Conversion from APLUINT to UINT
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes     ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeaderRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeaderRes->ArrType    = ARRAY_CHAR;
////lpHeaderRes->Perm       = 0;        // Already zero from GHND
////lpHeaderRes->SysVar     = 0;        // Already zero from GHND
    lpHeaderRes->RefCnt     = 1;
    lpHeaderRes->NELM       = aplNELMRes;
    lpHeaderRes->Rank       = 1;
#undef  lpHeaderRes

    // Save the dimension in the result
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Point to the data (APLAPA struct)
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // Copy the text to the result
    CopyMemory (lpMemRes, wszTemp, (UINT) aplNELMRes * sizeof (APLCHAR));

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnDydDR_SHOW_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydDR_FloatToChar_EM
//
//  QuadDR subroutine to convert FP to WCHAR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydDR_FloatToChar_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL SysFnDydDR_FloatToChar_EM
    (LPTOKEN lptkRhtArg,
     LPTOKEN lptkFunc)

{
    APLSTYPE aplTypeRht;        // Right arg storage type
    APLNELM  aplNELMRht;        // Right arg NELM
    APLRANK  aplRankRht;        // Right arg rank
    APLUINT  ByteRes,           // # bytes in the result
             uRes;              // Result loop counter
    HGLOBAL  hGlbRht,           // Right arg global memory handle
             hGlbRes;           // Result    ...
    LPVOID   lpMemRht = NULL,   // Ptr to right arg global memory
             lpMemRes = NULL;   // Ptr to result    ...
    APLFLOAT aplFloatRht;       // Temporary float

    // Get the attributes (Type, NELM, and Rank)
    //   of the right args
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Ensure the right arg is float
    if (aplTypeRht NE ARRAY_FLOAT)
    {
        ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Convert the FP argument to displayable chars

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRht * 16, aplRankRht + 1);

    // Allocate space for the result.
    // N.B. Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->Perm       = 0;   // Already zero from GHND
////lpHeader->SysVar     = 0;   // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht * 16;
    lpHeader->Rank       = aplRankRht + 1;
#undef  lpHeader

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);
    if (lpMemRht)
    {
        lpMemRht = VarArrayBaseToDim (lpMemRht);

        //***************************************************************
        // Copy the dimensions from the right arg
        //   to the result's dimension
        //***************************************************************
        for (uRes = 0; uRes < aplRankRht; uRes++)
            *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;
    } // End IF

    // The last dimension is 16
    *((LPAPLDIM) lpMemRes)++ = 16;

    // lpMemRes now points to the result's data

    // If the right arg is not an immediate, ...
    if (lpMemRht)
    {
        // Loop through the right arg converting it to the result
        for (uRes = 0; uRes < aplNELMRht; uRes++, ((LPAPLCHAR) lpMemRes += 16))
            FloatToAplchar (*((LPAPLFLOAT) lpMemRht)++, (LPAPLCHAR) lpMemRes);
    } else
    // The right arg is an immediate
    {
        // Get the first (and only) value
        FirstValue (lptkRhtArg,         // Ptr to right arg token
                    NULL,               // Ptr to integer result
                   &aplFloatRht,        // Ptr to float ...
                    NULL,               // Ptr to WCHAR ...
                    NULL,               // Ptr to longest ...
                    NULL,               // Ptr to lpSym/Glb ...
                    NULL,               // Ptr to ...immediate type ...
                    NULL);              // Ptr to array type ...
        FloatToAplchar (aplFloatRht, (LPAPLCHAR) lpMemRes);
    } // End IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // We no longer need this ptr
    if (lpMemRht)
    {
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return hGlbRes;
} // End SysFnDydDR_FloatToChar_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_dr.c
//***************************************************************************
