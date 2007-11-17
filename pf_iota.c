//***************************************************************************
//  NARS2000 -- Primitive Function -- Iota
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"
#include "execmfn.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $PrimFnIota_EM_YY
//
//  Primitive function for monadic and dyadic iota ("index generator" and "index of")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnIota_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnIota_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_IOTA);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

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
        return PrimFnMonIota_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydIota_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnIota_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnIota_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Iota
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnIota_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnIota_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnIota_EM_YY,   // Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnIota_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonIota_EM_YY
//
//  Primitive function for monadic iota ("index generator")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonIota_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonIota_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLNELM      aplNELMRes;    // Result NELM
    HGLOBAL      hGlbRes;       // Result global memory handle
    UINT         ByteRes;       // # bytes in the result
    LPVOID       lpMemRes;      // Ptr to result global memory
    BOOL         bRet = TRUE;   // TRUE iff result is valid
    APLINT       aplIntTmp;     // Temporary integer
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result
    APLBOOL      bQuadIO;       // []IO

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    //***************************************************************
    // Check the right argument for RANK, LENGTH, and DOMAIN ERRORs
    //   and fill in aplNELMRes.
    //***************************************************************

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkSym->stData.stGlbData));

                if (!PrimFnMonIotaGlb_EM (ClrPtrTypeDirAsGlb (lptkRhtArg->tkData.tkSym->stData.stGlbData),
                                         &aplNELMRes,
                                          lptkFunc))
                    return NULL;
                break;
            } // End IF

            // Handle the immediate case

            // stData is an immediate
            Assert (lptkRhtArg->tkData.tkSym->stFlags.Imm);

            if (!PrimFnMonIotaCon_EM (lptkRhtArg->tkData.tkSym,
                                     &aplNELMRes,
                                      lptkFunc))
                return NULL;
            break;              // Continue with common code

        case TKT_VARIMMED:
            // Split cases based upon the token's immediate type
            switch (lptkRhtArg->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:          // All Booleans are OK
                    aplNELMRes = lptkRhtArg->tkData.tkBoolean;

                    break;                  // Continue with common code

                case IMMTYPE_INT:           // Ensure non-negative integer
                    // Ensure the immediate value isn't too large and isn't negative
                    if (lptkRhtArg->tkData.tkInteger > MAX_APLNELM
                     || lptkRhtArg->tkData.tkInteger < 0)
                    {
                        // Mark as a DOMAIN ERROR
                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        return NULL;
                    } // End IF

                    aplNELMRes = lptkRhtArg->tkData.tkInteger;

                    break;                  // Continue with common code

                case IMMTYPE_FLOAT:         // Ensure it's close enough to a non-negative integer
                    // Convert the value to an integer using System CT
                    aplIntTmp = FloatToAplint_SCT (lptkRhtArg->tkData.tkFloat, &bRet);

                    if (bRet
                     && aplIntTmp < MAX_APLNELM
                     && !SIGN_APLNELM (aplIntTmp))
                    {
                        aplNELMRes = (APLNELM) aplIntTmp;

                        break;              // Continue with common code
                    } // End IF

                    // Fall through to IMMTYPE_CHAR to handle DOMAIN ERROR

                case IMMTYPE_CHAR:
                    // Mark as a DOMAIN ERROR
                    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                               lptkFunc);
                    return NULL;

                defstop
                    return NULL;
            } // End SWITCH

            break;              // Continue with common code

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

            if (!PrimFnMonIotaGlb_EM (ClrPtrTypeDirAsGlb (lptkRhtArg->tkData.tkGlbData),
                                     &aplNELMRes,
                                      lptkFunc))
                return NULL;
            break;              // Continue with common code

        defstop
            return NULL;
    } // End SWITCH

    // Calculate space needed for the result
    ByteRes = (UINT) CalcArraySize (ARRAY_APA, aplNELMRes, 1);

    // Allocate space for an APA
    hGlbRes = DbgGlobalAlloc (GHND, ByteRes);
    if (!hGlbRes)
    {
        // Mark as a WS FULL
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes     ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeaderRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeaderRes->ArrType    = ARRAY_APA;
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

    // Save the APA values
#define lpAPA       ((LPAPLAPA) lpMemRes)
    lpAPA->Off = bQuadIO;
    lpAPA->Mul = 1;
#undef  lpAPA

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeAsGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End PrimFnMonIota_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonIotaCon_EM
//
//  Monadic iota on a symbol table constant
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonIotaCon_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnMonIotaCon_EM
    (LPSYMENTRY lpSym,              // Ptr to the symbol table constant
     LPAPLNELM  lpaplNELMRes,       // Ptr to result NELM
     LPTOKEN    lptkFunc)           // Ptr to function token

{
    APLINT aplIntTmp;
    BOOL   bRet = TRUE;

    // stData is an immediate
    Assert (lpSym->stFlags.Imm);

    // Split cases based upon the symbol's immediate type
    switch (lpSym->stFlags.ImmType)
    {
        case IMMTYPE_BOOL:
            // Return the Boolean value
            *lpaplNELMRes = lpSym->stData.stBoolean;

            return TRUE;

        case IMMTYPE_INT:
            // Ensure the value is non-negative
            if (lpSym->stData.stInteger >= 0)
            {
                // Return the Integer value
                *lpaplNELMRes = lpSym->stData.stInteger;

                return TRUE;
            } // End IF

            break;      // Continue with DOMAIN ERROR handling

        case IMMTYPE_FLOAT:
            // Convert the value to an integer using System CT
            aplIntTmp = FloatToAplint_SCT (lpSym->stData.stFloat, &bRet);

            if (bRet
             && aplIntTmp < MAX_APLNELM
             && !SIGN_APLNELM (aplIntTmp))
            {
                // Return the Integer value
                *lpaplNELMRes = aplIntTmp;

                return TRUE;
            } // End IF

            break;      // Continue with DOMAIN ERROR handling

        case IMMTYPE_CHAR:
            break;      // Continue with DOMAIN ERROR handling

        defstop
            break;
    } // End SWITCH

    // Mark as a DOMAIN ERROR
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    return FALSE;
} // End PrimFnMonIotaCon_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonIotaGlobal_EM
//
//  Monadic iota on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonIotaGlb_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnMonIotaGlb_EM
    (HGLOBAL   hGlbRht,             // Handle to right arg
     LPAPLNELM lpaplNELMRes,        // Ptr to result NELM
     LPTOKEN   lptkFunc)            // Ptr to function token

{
    LPVOID   lpMem;
    APLSTYPE aplType;           // The array storage type (see ARRAY_TYPES)
    APLNELM  aplNELM;           // # elements in the array
    APLRANK  aplRank;           // The rank of the array
    BOOL     bRet = TRUE;

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
    // It's an array
    Assert (lpHeader->Sig.nature EQ VARARRAY_HEADER_SIGNATURE);

    // Get the Array Type, NELM, and Rank
    aplType = lpHeader->ArrType;
    aplNELM = lpHeader->NELM;
    aplRank = lpHeader->Rank;
#undef  lpHeader

    // Skip past the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, aplRank);

    // Only singletons allowed so far
    if (aplNELM NE 1)
    {
        // If its rank is too high, it's a RANK ERROR,
        //   otherwise it's a LENGTH ERROR
        if (aplRank > 1)
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkFunc);
        else
            ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                       lptkFunc);
        bRet = FALSE;
    } else
    // Traverse the array checking for DOMAIN ERROR
    // Split cases based upon the array storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            // Return the Boolean value
            *lpaplNELMRes = *(LPAPLBOOL) lpMem;

            break;

        case ARRAY_INT:
            // Ensure the value is non-negative
            if ((*(LPAPLINT) lpMem) < 0)
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                bRet = FALSE;
            } else
                // Return the Integer value
                *lpaplNELMRes = *(LPAPLINT) lpMem;

            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMem)
            // Ensure the value is non-negative
            if (lpAPA->Off < 0)
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                bRet = FALSE;
            } else
                // Return the Integer value
                *lpaplNELMRes = lpAPA->Off;
#undef  lpAPA
            break;

        case ARRAY_FLOAT:
            // Convert the value to an integer using System CT
            *lpaplNELMRes = FloatToAplint_SCT (*(LPAPLFLOAT) lpMem, &bRet);

            if (bRet
             && *lpaplNELMRes < MAX_APLNELM
             && !SIGN_APLNELM (*lpaplNELMRes))
                break;

            // Fall through to ARRAY_HETERO, etc. to handle DOMAIN ERROR

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                       lptkFunc);
            bRet = FALSE;

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMem = NULL;

    return bRet;
} // End PrimFnMonIotaGlb_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydIota_EM_YY
//
//  Primitive function for dyadic iota ("index of")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydIota_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydIota_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeLft,        // Left arg storage type
                 aplTypeRht;        // Right ...
    APLNELM      aplNELMLft,        // Left arg NELM
                 aplNELMRht;        // Right ...
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankRht;        // Right ...
    HGLOBAL      hGlbLft = NULL,    // Left arg global memory handle
                 hGlbRht = NULL,    // Right ...
                 hGlbRes = NULL;    // Result   ...
    LPVOID       lpMemLft,          // Ptr to left arg global memory
                 lpMemRht;          // Ptr to right ...
    LPAPLUINT    lpMemRes = NULL;   // Ptr to result   ...
    BOOL         bQuadIO;           // []IO
    APLUINT      NotFound;          // Not found value
    APLUINT      ByteRes;           // # bytes in the result
    APLLONGEST   aplLongestLft,     // Left arg immediate value
                 aplLongestRht;     // Right ...
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis);
        goto ERROR_EXIT;
    } // End IF

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for LEFT RANK ERROR
    if (aplRankLft EQ 0)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } else
    // Check for extended dyadic iota
    if (aplRankLft > 1)
    {
        HGLOBAL      hGlbPTD;           // PerTabData global memory handle
        LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
        LPPL_YYSTYPE lpYYRes;           // Prt to result

        // Get the PerTabData global memory handle
        hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        //  Extend to aplRankLft > 1 args by returning
        //    an array of index vectors where the length
        //    of each vector is aplRankLft.
        //  Use an internal magic function.
        lpYYRes =
          ExecuteMagicFunction_EM_YY (lptkLftArg,               // Ptr to left arg token
                                      lptkFunc,                 // Ptr to function token
                                      lptkRhtArg,               // Ptr to right arg token
                                      lptkAxis,                 // Ptr to axis token
                                      lpMemPTD->hGlbMF_DydIota);// Magic function global memory handle
        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

        return lpYYRes;
    } // End IF

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    //***************************************************************
    // Calculate space needed for the result
    //***************************************************************
    ByteRes = CalcArraySize (ARRAY_INT, aplNELMRht, aplRankRht);

    //***************************************************************
    // Now we can allocate the storage for the result
    // N.B.:  Conversion from APLUINT to UINT.
    //***************************************************************
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->Perm       = 0;               // Already zero from GHND
////lpHeader->SysVar     = 0;               // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = (LPAPLUINT) VarArrayBaseToDim (lpMemRes);

    // Fill in the result's dimension
    if (lpMemRht)
    {
        // Skip over the header to the dimensions
        lpMemRht = VarArrayBaseToDim (lpMemRht);

        // Copy the left arg dimensions to the result
        CopyMemory (lpMemRes, lpMemRht, (UINT) aplRankRht * sizeof (APLDIM));

        // Skip over the dimensions to the data
        lpMemRes = VarArrayDimToData (lpMemRes, aplRankRht);
        lpMemRht = VarArrayDimToData (lpMemRht, aplRankRht);
    } else
        // Point to the right arg immediate value
        lpMemRht = &aplLongestRht;

    if (lpMemLft)
        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
    else
        // Point to the left arg immediate value
        lpMemLft = &aplLongestLft;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Calculate the NotFound value
    NotFound = bQuadIO + aplNELMLft;

    // If the result is non-empty, and
    //    the right arg is non-empty, and
    //    the left and right args are not different types (numeric vs. char), ...
    if (!(aplNELMLft EQ 0
       || aplNELMRht EQ 0
       || (IsSimpleNum (aplTypeLft) && aplTypeRht EQ ARRAY_CHAR)
       || (IsSimpleNum (aplTypeRht) && aplTypeLft EQ ARRAY_CHAR)))
    {
        // Split cases based upon the left & right arg storage types
        if (aplTypeLft EQ ARRAY_BOOL && aplTypeRht EQ ARRAY_BOOL)
            // Handle APLBOOL vs. APLBOOL
            PrimFnDydIotaBvB (lpMemRes,             // Ptr to result global memory data
                              aplNELMLft,           // Left arg NELM
                              lpMemLft,             // Ptr to left arg global memory data
                              aplNELMRht,           // Right arg NELM
                              lpMemRht,             // Ptr to right arg global memory data
                              bQuadIO,              // []IO
                              NotFound);            // Not found value
        else
        if (IsSimpleInt (aplTypeLft) && IsSimpleInt (aplTypeRht))
        {
            // Handle APLBOOL/APLINT/APLAPA vs. APLBOOL/APLINT/APLAPA
            if (!PrimFnDydIotaIvI_EM (lpMemRes,     // Ptr to result global memory data
                                      aplTypeLft,   // Left arg storage type
                                      aplNELMLft,   // Left arg NELM
                                      lpMemLft,     // Ptr to left arg global memory data
                                      aplTypeRht,   // Right arg storage type
                                      aplNELMRht,   // Right arg NELM
                                      lpMemRht,     // Ptr to right arg global memory data
                                      bQuadIO,      // []IO
                                      NotFound,     // Not found value
                                      lptkFunc))    // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (aplTypeLft EQ ARRAY_CHAR && aplTypeRht EQ ARRAY_CHAR)
        {
            // Handle APLCHAR vs. APLCHAR
            if (!PrimFnDydIotaCvC_EM (lpMemRes,     // Ptr to result global memory data
                                      aplNELMLft,   // Left arg NELM
                                      lpMemLft,     // Ptr to left arg global memory data
                                      aplNELMRht,   // Right arg NELM
                                      lpMemRht,     // Ptr to right arg global memory data
                                      bQuadIO,      // []IO
                                      NotFound,     // Not found value
                                      lptkFunc))    // Ptr to function token
                goto ERROR_EXIT;
        } else
            // Handle all other combinations
            PrimFnDydIotaOther (lpMemRes,           // Ptr to result global memory data
                                aplTypeLft,         // Left arg storage type
                                aplNELMLft,         // Left arg NELM
                                lpMemLft,           // Ptr to left arg global memory data
                                aplTypeRht,         // Right arg storage type
                                aplNELMRht,         // Right arg NELM
                                lpMemRht,           // Ptr to right arg global memory data
                                bQuadIO,            // []IO
                                NotFound,           // Not found value
                                lptkFunc);          // Ptr to function token
    } else
    {
        APLUINT uRht;           // Loop counter

        // Fill the result with NotFound
        for (uRht = 0; uRht < aplNELMRht; uRht++)
            *lpMemRes++ = NotFound;
    } // End IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeAsGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

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

    return lpYYRes;
} // End PrimFnDydIota_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydIotaBvB
//
//  Dyadic iota between Boolean args
//***************************************************************************

void PrimFnDydIotaBvB
    (LPAPLUINT lpMemRes,            // Ptr to result global memory data
     APLNELM   aplNELMLft,          // Left arg NELM
     LPAPLBOOL lpMemLft,            // Ptr to left arg global memory data
     APLNELM   aplNELMRht,          // Right arg (and result) NELM
     LPAPLBOOL lpMemRht,            // Ptr to right arg global memory data
     BOOL      bQuadIO,             // []IO
     APLUINT   NotFound)            // Not found value

{
    BOOL    Found0,                 // TRUE iff there is at least one 0 in the right arg
            Found1;                 // ...                            1 ...
    APLUINT Index0,                 // Index of the first 0 in the left arg
            Index1,                 // ...                1 ...
            BytesInLftBits,         // # bytes in the left arg data
            BytesInRhtBits,         // ...            right ...
            uLft,                   // Loop counter
            uRht,                   // Loop counter
            uTmp;                   // Temporary
    UINT    uBitMaskRht;            // Bit mask for the right arg

    // The result is either all NotFounds       (if left arg is empty -- already ruled out), or
    //                      same as Lft         (if left arg is all 1s), or
    //                      complement of Lft   (if left arg is all 0s), or
    //                      all 1s              (if left arg has at least one 0 and one 1)

    // Calculate the # bytes in the left arg (result) data
    BytesInLftBits = RoundUpBits8 (aplNELMLft);

    // Calculate the # bytes in the right arg data
    BytesInRhtBits = RoundUpBits8 (aplNELMRht);

    // Search the left arg for a 0
    for (Found0 = FALSE, Index0 = bQuadIO, uLft = 0; uLft < (BytesInLftBits - 1); uLft++)
    {
        uTmp = FastBoolTrans[lpMemLft[uLft]][0];
        Index0 += uTmp;

        if (uTmp < NBIB)
        {
            Found0 = TRUE;

            break;
        } // End IF
    } // End FOR

    // If we didn't find a 0, check the last byte (may be short)
    if (!Found0)
    {
        uTmp = FastBoolTrans[lpMemLft[uLft]][0];
        Index0 += uTmp;
    } // End IF

    // Search the left arg for a 1
    for (Found1 = FALSE, Index1 = bQuadIO, uLft = 0; uLft < (BytesInLftBits - 1); uLft++)
    {
        uTmp = FastBoolTrans[lpMemLft[uLft]][1];
        Index1 += uTmp;

        if (uTmp < NBIB)
        {
            Found1 = TRUE;

            break;
        } // End IF
    } // End FOR

    // If we didn't find a 1, check the last byte (may be short)
    if (!Found1)
    {
        uTmp = FastBoolTrans[lpMemLft[uLft]][1];
        Index1 += uTmp;
    } // End IF

    uBitMaskRht = 0x01;

    // Loop throught the right arg,
    //   saving in the result Index0 for each 0 in the right arg, and
    //                        Index1 for each 1 in the right arg
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        if (*lpMemRht & uBitMaskRht)
            *lpMemRes++ = Index1;
        else
            *lpMemRes++ = Index0;

        // Shift over the right bit mask
        uBitMaskRht <<= 1;

        // Check for end-of-byte
        if (uBitMaskRht EQ END_OF_BYTE)
        {
            uBitMaskRht = 0x01;     // Start over
            lpMemRht++;             // Skip to next byte
        } // End IF
    } // End FOR
} // End PrimFnDydIotaBvB


//***************************************************************************
//  $PrimFnDydIotaIvI_EM
//
//  Dyadic iota between simple ints (APLBOOL/APLINT/APLAPA)
//***************************************************************************

BOOL PrimFnDydIotaIvI_EM
    (LPAPLUINT lpMemRes,            // Ptr to result global memory data
     APLSTYPE  aplTypeLft,          // Left arg storage type
     APLNELM   aplNELMLft,          // Left arg NELM
     LPVOID    lpMemLft,            // Ptr to left arg global memory data
     APLSTYPE  aplTypeRht,          // Right arg storage type
     APLNELM   aplNELMRht,          // Right arg NELM
     LPVOID    lpMemRht,            // Ptr to right arg global memory data
     BOOL      bQuadIO,             // []IO
     APLUINT   NotFound,            // Not found value
     LPTOKEN   lptkFunc)            // Ptr to function token

{
    APLUINT   uLft,                 // Loop counter
              uRht;                 // Loop counter
    APLINT    aplIntegerLft,        // Left arg integer
              aplIntegerRht;        // Right arg integer

    // Here's where we might do something clever such as sort the right arg
    //   or perhaps hash it assuming one or both args are large enough
    //   to warrant such extra work.

    // For the moment, we'll do an outer product
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        aplIntegerRht =
          GetNextInteger (lpMemRht,
                          aplTypeRht,
                          uRht);
        // Loop through the left arg
        for (uLft = 0; uLft < aplNELMLft; uLft++)
        {
            aplIntegerLft =
              GetNextInteger (lpMemLft,
                              aplTypeLft,
                              uLft);
            if (aplIntegerLft EQ aplIntegerRht)
            {
                // Set the result value
                *lpMemRes++ = uLft + bQuadIO;

                break;
            } // End IF
        } // End FOR

        // If we didn't find the value, ...
        if (uLft EQ aplNELMLft)
            // Set the result value
            *lpMemRes++ = NotFound;
    } // End FOR

    return TRUE;
} // End PrimFnDydIotaIvI_EM


//***************************************************************************
//  $PrimFnDydIotaCvC_EM
//
//  Dyadic iota between char args
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydIotaCvC_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydIotaCvC_EM
    (LPAPLUINT lpMemRes,            // Ptr to result global memory data
     APLNELM   aplNELMLft,          // Left arg (and result) NELM
     LPAPLCHAR lpMemLft,            // Ptr to left arg global memory data
     APLNELM   aplNELMRht,          // Right arg NELM
     LPAPLCHAR lpMemRht,            // Ptr to right arg global memory data
     BOOL      bQuadIO,             // []IO
     APLUINT   NotFound,            // Not found value
     LPTOKEN   lptkFunc)            // Ptr to function token

{
    LPAPLUINT lpMemTT;              // Translate table gloabl memory handle
    UINT      ByteTT;               // # bytes in the Translate Table
    APLUINT   uRht;                 // Loop counter
    APLINT    iLft;                 // ...

    // Calculate # bytes in the TT at one APLUINT per 16-bit index (APLCHAR)
    ByteTT = (1 << (NBIB * sizeof (APLCHAR))) * sizeof (APLUINT);

    // Allocate space for a ByteTT Translate Table
    // Note that this allocation is GMEM_FIXED
    //   because we'll use it quickly and then free it.
    lpMemTT = MyGlobalAlloc (GMEM_FIXED, ByteTT);
    if (!lpMemTT)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return FALSE;
    } // End IF

    // Trundle through the TT setting each value to NotFound
    for (uRht = 0; uRht < (1 << (NBIB * sizeof (APLCHAR))); uRht++)
        lpMemTT[uRht] = NotFound;

    // Trundle through the left arg backwards marking the TT
    for (iLft = aplNELMLft - 1, lpMemLft += (aplNELMLft - 1);
         iLft >= 0;
         iLft--)
        lpMemTT[*lpMemLft--] = iLft + bQuadIO;

    // Trundle through the right arg looking the chars in the TT
    //   and setting the result value
    for (uRht = 0; uRht < aplNELMRht; uRht++)
        // Save the index from the TT in the result
        *lpMemRes++ = lpMemTT[*lpMemRht++];

    // We no longer need this storage
    MyGlobalFree (lpMemTT); lpMemTT = NULL;

    return TRUE;
} // End PrimFnDydIotaCvC_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydIotaOther
//
//  Dyadic iota between all other arg combinations
//***************************************************************************

void PrimFnDydIotaOther
    (LPAPLUINT lpMemRes,            // Ptr to result global memory data
     APLSTYPE  aplTypeLft,          // Left arg storage type
     APLNELM   aplNELMLft,          // Left arg NELM
     LPVOID    lpMemLft,            // Ptr to left arg global memory data
     APLSTYPE  aplTypeRht,          // Right arg storage type
     APLNELM   aplNELMRht,          // Right arg NELM
     LPVOID    lpMemRht,            // Ptr to right arg global memory data
     BOOL      bQuadIO,             // []IO
     APLUINT   NotFound,            // Not found value
     LPTOKEN   lptkFunc)            // Ptr to function token

{
    HGLOBAL      hGlbSubLft,        // Left arg item global memory handle
                 hGlbSubRht;        // Right ...
    APLLONGEST   aplLongestSubLft,  // Left arg item immediate value
                 aplLongestSubRht;  // Right ...
    IMM_TYPES    immTypeSubLft,     // Left arg item immediate type
                 immTypeSubRht;     // Right ...
    APLFLOAT     fQuadCT;           // []CT
    APLUINT      uLft,              // Loop counter
                 uRht;              // ...
    LPPL_YYSTYPE lpYYTmp;           // Ptr to the temporary result
    BOOL         bCmp;              // TRUE iff the comparison is TRUE

    // This leaves:  Left vs. Right
    //                B   vs.    FHN
    //                I   vs.    FHN
    //                A   vs.    FHN
    //                F   vs. BIAFHN
    //                H   vs. BIAFHN
    //                N   vs. BIAFHN

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Loop through the right arg
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        TOKEN tkSubRht = {0};       // Right arg item token

        // Get the next value from the right arg
        GetNextValueMem (lpMemRht,              // Ptr to right arg global memory
                         aplTypeRht,            // Right arg storage type
                         uRht,                  // Right arg index
                        &hGlbSubRht,            // Right arg item global memory handle
                        &aplLongestSubRht,      // Ptr to right arg immediate value
                        &immTypeSubRht);        // Ptr to right arg immediate type
        // Fill in the right arg item token
        tkSubRht.tkFlags.TknType   = TKT_VARARRAY;
////////tkSubRht.tkFlags.ImmType   = 0;     // Already zero from = {0}
////////tkSubRht.tkFlags.NoDisplay = 0;     // Already zero from = {0}
        tkSubRht.tkData.tkGlbData  = MakeGlbTypeAsGlb (hGlbSubRht);
        tkSubRht.tkCharIndex       = NEG1U;

        // Loop through the left arg
        for (uLft = 0; uLft < aplNELMLft; uLft++)
        {
            // Get the next value from the left arg
            GetNextValueMem (lpMemLft,              // Ptr to left arg global memory
                             aplTypeLft,            // Left arg storage type
                             uLft,                  // Left arg index
                            &hGlbSubLft,            // Left arg item global memory handle
                            &aplLongestSubLft,      // Ptr to left arg immediate value
                            &immTypeSubLft);        // Ptr to left arg immediate type
            // If both items are globals, ...
            if ((hGlbSubLft NE NULL) && (hGlbSubRht NE NULL))
            {
                TOKEN tkSubLft = {0};       // Left arg item token

                // Fill in the left arg item token
                tkSubLft.tkFlags.TknType   = TKT_VARARRAY;
////////////////tkSubLft.tkFlags.ImmType   = 0;     // Already zero from = {0}
////////////////tkSubLft.tkFlags.NoDisplay = 0;     // Already zero from = {0}
                tkSubLft.tkData.tkGlbData  = MakeGlbTypeAsGlb (hGlbSubLft);
                tkSubLft.tkCharIndex       = NEG1U;

                // Use match to determine equality
                lpYYTmp =
                  PrimFnDydEqualUnderbar_EM_YY (&tkSubLft,      // Ptr to left arg token
                                                 lptkFunc,      // Ptr to function token
                                                &tkSubRht,      // Ptr to right arg token
                                                 NULL);         // Ptr to axis token (may be NULL)
                // Save the result of the comparison
                bCmp = lpYYTmp->tkToken.tkData.tkBoolean;

                // Free the temporary result
                YYFree (lpYYTmp); lpYYTmp = NULL;

                if (bCmp)
                    goto SET_RESULT_VALUE;
            } else
            // If both items are simple, ...
            if ((hGlbSubLft EQ NULL) && (hGlbSubRht EQ NULL))
            {
                // If both items are APLCHARs, ...
                if ((immTypeSubLft EQ IMMTYPE_CHAR) && (immTypeSubRht EQ IMMTYPE_CHAR))
                {
                    // Compare the APLCHARs
                    if (((APLCHAR) aplLongestSubLft) EQ (APLCHAR) aplLongestSubRht)
                        goto SET_RESULT_VALUE;
                } else
                // If both items are numeric, ...
                if (IsImmNum (immTypeSubLft) && IsImmNum (immTypeSubRht))
                {
                    // This leaves BIF vs. BIF

                    // Split cases based upon the left arg item immediate type
                    switch (immTypeSubLft)
                    {
                        case IMMTYPE_BOOL:              // Lft = BOOL
                            // Split cases based upon the right arg item immediate type
                            switch (immTypeSubRht)
                            {
                                case IMMTYPE_BOOL:      // Lft = BOOL, Rht = BOOL
                                    if (((APLBOOL) aplLongestSubLft) EQ (APLBOOL) aplLongestSubRht)
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_INT:       // Lft = BOOL, Rht = INT
                                    if (((APLBOOL) aplLongestSubLft) EQ (APLINT)  aplLongestSubRht)
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_FLOAT:     // Lft = BOOL, Rht = FLOAT
                                    if (CompareCT ((APLBOOL) aplLongestSubLft, *(LPAPLFLOAT) &aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_VALUE;
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case IMMTYPE_INT:               // Lft = INT
                            // Split cases based upon the right arg item immediate type
                            switch (immTypeSubRht)
                            {
                                case IMMTYPE_BOOL:      // Lft = INT, Rht = BOOL
                                    if (((APLINT)  aplLongestSubLft) EQ (APLBOOL) aplLongestSubRht)
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_INT:       // Lft = INT, Rht = INT
                                    if (((APLINT)  aplLongestSubLft) EQ (APLINT)  aplLongestSubRht)
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_FLOAT:     // Lft = INT, Rht = FLOAT
                                    if (CompareCT ((APLFLOAT) (APLINT)  aplLongestSubLft, *(LPAPLFLOAT) &aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_VALUE;
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case IMMTYPE_FLOAT:             // Lft = FLOAT
                            // Split cases based upon the right arg item immediate type
                            switch (immTypeSubRht)
                            {
                                case IMMTYPE_BOOL:      // Lft = FLOAT, Rht = BOOL
                                    if (CompareCT (*(LPAPLFLOAT) &aplLongestSubLft, (APLBOOL) aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_INT:       // Lft = FLOAT, Rht = INT
                                    if (CompareCT (*(LPAPLFLOAT) &aplLongestSubLft, (APLFLOAT) (APLINT) aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_FLOAT:     // Lft = FLOAT, Rht = FLOAT
                                    if (CompareCT (*(LPAPLFLOAT) &aplLongestSubLft, *(LPAPLFLOAT) &aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_VALUE;
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End IF/ELSE/...
            } // End IF/ELSE/...
        } // End FOR

        // We didn't find the it, so
        //   set the result value to NotFound
        *lpMemRes ++ = NotFound;

        continue;

SET_RESULT_VALUE:
        // Set the result value
        *lpMemRes++ = uLft + bQuadIO;
    } // End FOR
} // End PrimFnDydIotaOther


//***************************************************************************
//  Magic function for extended dyadic iota
//
//  Extended dyadic iota
//
//  On rank > 1 left args, return an array of vector indices
//    such that A[A iota R] is R, assuming that all of R is in A.
//***************************************************************************

static APLCHAR Header[] =
  $Z $IS $L L" " $F L" " $R L";" $QUAD_IO L";" $O;

static APLCHAR Line1[] =
  $O $IS $QUAD_IO
  $DIAMOND $QUAD_IO $IS L"0";

static APLCHAR Line2[] =
  $Z $IS $ENCLOSE L"[0]" $O L"+(1+" $RHO $L L")" $ENCODE L"(" $NEG L"1" $DROP L",(1+" $RHO $L L")" $TAKE $L L")" $IOTA $R;

static LPAPLCHAR Body[] =
{Line1,
 Line2,
};

MAGIC_FUNCTION MF_DydIota =
{Header,
 Body,
 sizeof (Body) / sizeof (Body[0]),
};


//***************************************************************************
//  End of File: pf_iota.c
//***************************************************************************
