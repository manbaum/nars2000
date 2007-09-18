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
//  $SysFnDR_EM
//
//  System function:  []DR -- Data Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDR_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDR_EM
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
        return SysFnMonDR_EM (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydDR_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnDR_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonDR_EM
//
//  Monadic []DR -- Display the Data Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonDR_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonDR_EM
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    HGLOBAL      hGlbData;
    LPVOID       lpMem;
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
#define DR_BOOL           100   //    1 bit  per value
#define DR_CHAR          1601   //   16 bits ...
#define DR_INT           6402   //   64 ...
#define DR_FLOAT         6403   //   64 ...
#define DR_APA           6404   //   64 ...
#define DR_HETERO        3205   //   32 ...
#define DR_NESTED        3206   //   32 ...
#define DR_LIST          3207   //   32 ...
#define DR_RATIONAL         8   //   ?? ...
#define DR_COMPLEX      12809   //  128 ...
#define DR_QUATERNIONS  25610   //  256 ...
#define DR_OCTONIONS    51211   //  512 ...

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, it's an HGLOBAL
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                break;      // Continue with HGLOBAL case
            } // End IF

            // Handle the immediate case

            // stData is an immediate
            Assert (lptkRhtArg->tkData.tkSym->stFlags.Imm);

            // Split cases based upon the token's immediate type
            switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    lpYYRes->tkToken.tkData.tkInteger = DR_BOOL;

                    return lpYYRes;

                case IMMTYPE_INT:
                    lpYYRes->tkToken.tkData.tkInteger = DR_INT;

                    return lpYYRes;

                case IMMTYPE_FLOAT:
                    lpYYRes->tkToken.tkData.tkInteger = DR_FLOAT;

                    return lpYYRes;

                case IMMTYPE_CHAR:
                    lpYYRes->tkToken.tkData.tkInteger = DR_CHAR;

                    return lpYYRes;

                defstop
                    return NULL;
            } // End SWITCH

            DbgStop ();         // We should never get here

        case TKT_VARIMMED:
            // Split cases based upon the token's immediate type
            switch (lptkRhtArg->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    lpYYRes->tkToken.tkData.tkInteger = DR_BOOL;

                    return lpYYRes;

                case IMMTYPE_INT:
                    lpYYRes->tkToken.tkData.tkInteger = DR_INT;

                    return lpYYRes;

                case IMMTYPE_FLOAT:
                    lpYYRes->tkToken.tkData.tkInteger = DR_FLOAT;

                    return lpYYRes;

                case IMMTYPE_CHAR:
                    lpYYRes->tkToken.tkData.tkInteger = DR_CHAR;

                    return lpYYRes;

                defstop
                    return NULL;
            } // End SWITCH

            DbgStop ();         // We should never get here

        case TKT_LISTPAR:
            lpYYRes->tkToken.tkData.tkInteger = DR_LIST;

            return lpYYRes;

        case TKT_VARARRAY:
            hGlbData = lptkRhtArg->tkData.tkGlbData;

            break;      // Continue with HGLOBAL case

        defstop
            break;
    } // End SWITCH

    // HGLOBAL case
    // tk/stData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (hGlbData));

    // Clear the ptr type bits
    hGlbData = ClrPtrTypeDirGlb (hGlbData);

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    // Split cases based upon the array storage type
    switch (lpHeader->ArrType)
#undef  lpHeader
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

        case ARRAY_HETERO:
            lpYYRes->tkToken.tkData.tkInteger = DR_HETERO;

            break;

        case ARRAY_NESTED:
            lpYYRes->tkToken.tkData.tkInteger = DR_NESTED;

            break;

        case ARRAY_APA:
            lpYYRes->tkToken.tkData.tkInteger = DR_APA;

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;

    return lpYYRes;
} // End SysFnMonDR_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydDR_EM
//
//  Dyadic []DR -- Convert To A Data Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydDR_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydDR_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeLft,
                 aplTypeRht;
    APLNELM      aplNELMLft,
                 aplNELMRht;
    APLRANK      aplRankLft,
                 aplRankRht;
    HGLOBAL      hGlbRes = NULL;
    APLINT       aplIntegerLft;
    APLFLOAT     aplFloatLft;
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

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
            return SysFnDydDR_SHOW_EM (aplTypeRht, lptkFunc);

        case DR_BOOL:
        case DR_CHAR:
        case DR_INT:
        case DR_FLOAT:
            ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                       lptkFunc);
            return NULL;

            DbgBrk ();      // ***FINISHME*** -- SysFnMonDR_EM





            break;

        case DR_APA:
        case DR_HETERO:
        case DR_NESTED:
        case DR_LIST:
        case DR_RATIONAL:
        case DR_COMPLEX:
        case DR_QUATERNIONS:
        case DR_OCTONIONS:
            // ***DEBUG***
            hGlbRes = DR_FloatToChar_EM (lptkRhtArg, lptkFunc);

            break;

        default:
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                       lptkFunc);
            return NULL;
    } // End SWITCH

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnDydDR_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydDR_SHOW_EM
//
//  Return a character representation of the storage type
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydDR_SHOW_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydDR_SHOW_EM
    (APLSTYPE aplTypeRht,
     LPTOKEN  lptkFunc)

{
    LPAPLCHAR    lpw;           // Ptr to WCHAR text
    APLUINT      ByteRes;       // # bytes in the result
    HGLOBAL      hGlbRes;       // Result global memory handle
    LPVOID       lpMemRes;      // Ptr to result global memory
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result
    APLNELM      aplNELMRes;    // Result NELM

    // Split cases based upon the rigth arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            lpw = L"Boolean:  1 bit per element";

            break;

        case ARRAY_INT:
            lpw = L"Integer:  64 bits per element";

            break;

        case ARRAY_FLOAT:
            lpw = L"Floating Point:  64 bits per element";

            break;

        case ARRAY_CHAR:
            lpw = L"Character:  16 bits per element";

            break;

        case ARRAY_APA:
            lpw = L"Arithmetic Progression Array:  128 bits total";

            break;

        case ARRAY_LIST:
            lpw = L"List:  32 bits per element";

            break;

        case ARRAY_NESTED:
            lpw = L"Nested Array:  32 bits per element";

            break;

        case ARRAY_HETERO:
            lpw = L"Heterogeneous Array:  32 bits per element";

            break;

        defstop
            break;
    } // End SWITCH

    // Get the result NELM
    aplNELMRes = lstrlenW (lpw);

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
    CopyMemory (lpMemRes, lpw, (UINT) aplNELMRes * sizeof (APLCHAR));

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
} // End SysFnDydDR_SHOW_EM
#undef  APPEND_NAME


//***************************************************************************
//  $DR_FloatToChar_EM
//
//  QuadDR subroutine to convert FP to WCHAR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- DR_FloatToChar_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL DR_FloatToChar_EM
    (LPTOKEN lptkRhtArg,
     LPTOKEN lptkFunc)

{
    APLSTYPE aplTypeRht;        // Right arg storage type
    APLNELM  aplNELMRht;        // Right arg NELM
    APLRANK  aplRankRht;        // Right arg rank
    APLUINT  ByteRes,           // # bytes needed for the result
             uRes;              // Result loop counter
    HGLOBAL  hGlbRht,           // Right arg global memory handle
             hGlbRes;           // Result    ...
    LPVOID   lpMemRht = NULL,   // Ptr to right arg global memory
             lpMemRes = NULL;   // Ptr to result    ...
    APLFLOAT aplFloatRht;       // Temporary float

    // Get the attributes (Type, NELM, and Rank)
    //   of the right args
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

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
} // End DR_FloatToChar_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_dr.c
//***************************************************************************
