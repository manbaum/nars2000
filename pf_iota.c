//***************************************************************************
//  NARS2000 -- Primitive Function -- Iota
//***************************************************************************

#pragma pack (1)
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
//  PrimFnIota_EM
//
//  Primitive function for monadic and dyadic iota ("index generator" and "index of")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnIota_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnIota_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_IOTA);

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
        return PrimFnMonIota_EM (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydIota_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnIota_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonIota_EM
//
//  Primitive function for monadic iota ("index generator")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonIota_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonIota_EM
    (LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    APLNELM aplNELMRes;
    HGLOBAL hGlbRes;
    UINT    ByteRes;
    LPVOID  lpMemRes;
    BOOL    bRet = TRUE;
    APLINT  aplIntTmp;
    UINT    YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = NewYYResIndex ();

    //***************************************************************
    // Check the right argument for RANK, LENGTH, and DOMAIN ERRORs
    //   and fill in aplNELMRes.
    //***************************************************************

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lptkRhtArg->tkData.lpSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.lpSym->stData.stGlbData));

                if (!PrimFnMonIotaGlb_EM (ClrPtrTypeDirGlb (lptkRhtArg->tkData.lpSym->stData.stGlbData),
                                          &aplNELMRes,
                                          lptkFunc))
                    return NULL;
                break;
            } // End IF

            // Handle the immediate case

            // stData is an immediate
            Assert (lptkRhtArg->tkData.lpSym->stFlags.Imm);

            if (!PrimFnMonIotaCon_EM (lptkRhtArg->tkData.lpSym,
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

            if (!PrimFnMonIotaGlb_EM (ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkGlbData),
                                      &aplNELMRes,
                                      lptkFunc))
                return NULL;
            break;              // Continue with common code

        case TKT_LISTPAR:
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkFunc);
            return NULL;

        defstop
            return NULL;
    } // End SWITCH

    // Calculate size of the result
    ByteRes = (UINT) CalcArraySize (ARRAY_APA, 0, 1);
////ByteRes = sizeof (VARARRAY_HEADER)
////        + sizeof (APLDIM) * 1      // It's a vector
////        + sizeof (APLAPA);         // APA struct

    // Allocate space for an APA
    hGlbRes = DbgGlobalAlloc (GHND, ByteRes);
    if (!hGlbRes)
    {
        // Mark as a WS FULL
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the global memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes     ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeaderRes->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeaderRes->ArrType    = ARRAY_APA;
////lpHeaderRes->Perm       = 0;
////lpHeaderRes->SysVar     = 0;
    lpHeaderRes->RefCnt     = 1;
    lpHeaderRes->NELM       = aplNELMRes;
    lpHeaderRes->Rank       = 1;

#undef  lpHeaderRes

    // Save the dimension in the result
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Point to the data (APLAPA struct)
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

#define lpAPARes    ((LPAPLAPA) lpMemRes)

    // Save the APA values
    lpAPARes->Off = bQuadIO;
    lpAPARes->Mul = 1;
    lpAPARes->Len = aplNELMRes;

#undef  lpAPARes

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Fill in the result token
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
////YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;    // Already zero from ZeroMemory
////YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;    // Already zero from ZeroMemory
    YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    YYRes[YYLclIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return &YYRes[YYLclIndex];
} // End PrimFnMonIota_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonIotaCon_EM
//
//  Monadic iota on a symbol table constant
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonIotaCon_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnMonIotaCon_EM
    (LPSYMENTRY lpSym,
     LPAPLNELM  lpaplNELMRes,
     LPTOKEN    lptkFunc)

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
//  PrimFnMonIotaGlobal_EM
//
//  Monadic iota on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonIotaGlb_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnMonIotaGlb_EM
    (HGLOBAL   hGlbRht,
     LPAPLNELM lpaplNELMRes,
     LPTOKEN   lptkFunc)

{
    LPVOID   lpMem;
    APLSTYPE aplType;           // The array storage type (see enum ARRAY_TYPES)
    APLNELM  aplNELM;           // # elements in the array
    APLRANK  aplRank;           // The rank of the array
    BOOL     bRet = TRUE;

    // Traverse the array looking for DOMAIN ERRORs
    lpMem = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    // It's an array
    Assert (lpHeader->Sign.ature EQ VARARRAY_HEADER_SIGNATURE);

    // Save the Type, NELM, and Rank
    aplType = lpHeader->ArrType;
    aplNELM = lpHeader->NELM;
    aplRank = lpHeader->Rank;

#undef  lpHeader

    // Skip past the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, aplRank);

    // Only singletons allowed so far
    if (aplNELM > 1)
    {
        // If it's rank is too high, it's a RANK ERROR,
        //   otherwise it's a LENGTH ERROR
        if (aplRank > 1)
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkFunc);
        else
            ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                       lptkFunc);
        bRet = FALSE;
    } else
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
            // Ensure the value is non-negative
            if ((((LPAPLAPA) lpMem)->Off) < 0)
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                bRet = FALSE;
            } else
                // Return the Integer value
                *lpaplNELMRes = ((LPAPLAPA) lpMem)->Off;

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

        case ARRAY_LIST:
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
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
//  PrimFnDydIota_EM
//
//  Primitive function for dyadic iota ("index of")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydIota_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDydIota_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    APLSTYPE aplTypeLft,
             aplTypeRht;
    APLNELM  aplNELMLft,
             aplNELMRht;
    APLRANK  aplRankLft,
             aplRankRht;
    HGLOBAL  hGlbLft,
             hGlbRht;
    LPVOID   lpMemLft,
             lpMemRht;
    BOOL     bRet = TRUE;
    UINT     YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = NewYYResIndex ();

    DbgBrk ();          // ***FINISHME***

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Check for RANK ERROR
    if (aplRankLft NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkLftArg);
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF















ERROR_EXIT:
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

    if (bRet)
        return &YYRes[YYLclIndex];
    else
        return NULL;
} // End PrimFnDydIota_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_iota.c
//***************************************************************************
