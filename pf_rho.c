//***************************************************************************
//  NARS2000 -- Primitive Function -- Rho
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
//  PrimFnRho_EM
//
//  Primitive function for monadic and dyadic rho (shape and reshape)
//***************************************************************************

LPYYSTYPE PrimFnRho_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_RHO);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonRho_EM             (lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydRho_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnRho_EM


//***************************************************************************
//  PrimFnMonRho_EM
//
//  Primitive function for monadic rho (shape)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonRho_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonRho_EM
    (LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
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

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkRhtArg->tkData.lpSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.lpSym->stData.stGlbData));

                return PrimFnMonRhoGlb_EM (ClrPtrTypeDirGlb (lptkRhtArg->tkData.lpSym->stData.stGlbData),
                                           lptkFunc);
            } // End IF

            // Handle the immediate case

            // Fall through to TKT_VARIMMED case to return {zilde}

        case TKT_VARIMMED:          // Return {zilde}
            return PrimFnMonRhoCon_EM (lptkFunc);

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

            return PrimFnMonRhoGlb_EM (ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkGlbData),
                                       lptkFunc);
        case TKT_LIST:
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkFunc);
            return NULL;

        defstop
            return NULL;
    } // End SWITCH
} // End PrimFnMonRho_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonRhoCon_EM
//
//  Monadic rho on a symbol table constant
//***************************************************************************

LPYYSTYPE PrimFnMonRhoCon_EM
    (LPTOKEN lptkFunc)

{
    UINT YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = NewYYResIndex ();

    // As this is a simple scalar, the result is {zilde}

    // Fill in the result token
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
////YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;    // Already zero from ZeroMemory
////YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;    // Already zero from ZeroMemory
    YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbZilde);
    YYRes[YYLclIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return &YYRes[YYLclIndex];
} // End PrimFnMonRhoCon_EM


//***************************************************************************
//  PrimFnMonRhoGlb_EM
//
//  Monadic rho on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonRhoGlb_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonRhoGlb_EM
    (HGLOBAL hGlbRht,
     LPTOKEN lptkFunc)

{
    LPVOID  lpMemRht, lpMemRes;
    APLRANK aplRankRht;      // The rank of the array
    HGLOBAL hGlbRes;
    APLUINT ByteRes;
    BOOL    bRet = TRUE;
    UINT    uRes;
    UINT    YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = NewYYResIndex ();

    // Lock the global memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeaderRht     ((LPVARARRAY_HEADER) lpMemRht)

    // Get the rank
    aplRankRht = lpHeaderRht->Rank;

#undef  lpHeaderRht

    // Calculate the size of the result
    ByteRes = CalcArraySize (ARRAY_INT, aplRankRht, 1);
////ByteRes = sizeof (VARARRAY_HEADER)
////        + sizeof (APLDIM) * 1           // The result is a vector
////        + sizeof (APLINT) * aplRankRht; // ...with this many integers

    // Allocate space for one dimension and <aplRankRht> integers
    // N.B.:  Conversion from aplRankRht to UINT
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF

    // Fill in the result token
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
////YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;    // Already zero from ZeroMemory
////YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;    // Already zero from ZeroMemory
    YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    YYRes[YYLclIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // Lock the global memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes     ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeaderRes->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeaderRes->ArrType    = ARRAY_INT;
////lpHeaderRes->Perm       = 0;                // Already zero from GHND
////lpHeaderRes->SysVar     = 0;                // Already zero from GHND
    lpHeaderRes->RefCnt     = 1;
    lpHeaderRes->NELM       = aplRankRht;
    lpHeaderRes->Rank       = 1;

#undef  lpHeaderRes

    // Save the dimension
    *VarArrayBaseToDim (lpMemRes) = aplRankRht;

    // Skip over the header and dimension to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // Skip over the header to the right arg's dimensions
    lpMemRht = VarArrayBaseToDim (lpMemRht);

#define lpDimRht        ((LPAPLDIM) lpMemRht)
#define lpDataRes       ((LPAPLINT) lpMemRes)

    // Copy the dimensions to the result
    for (uRes = 0; uRes < aplRankRht; uRes++)
        *lpDataRes++ = *lpDimRht++;

#undef  lpDimRht
#undef  lpDataRes

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
ERROR_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

    if (bRet)
        return &YYRes[YYLclIndex];
    else
        return NULL;
} // End PrimFnMonRhoGlb_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydRho_EM
//
//  Primitive function for dydadic rho (reshape)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydRho_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDydRho_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    APLNELM  aplNELMRes,    // # elements in the result
             aplNELMRht;    // ...               right arg
    APLRANK  aplRankRes;    // The rank of the result
    HGLOBAL  hGlbRes,       // Handle of result's global memory
             hGlbProto;     // ...                prototype
    LPVOID   lpMemRes,      // Ptr to result's global memory
             lpDataRes;     // ...    result's data
    LPAPLDIM lpDimRes;      // Ptr to result's dimensions
    BOOL     bRet = TRUE,
             bPrototype = FALSE; // TRUE iff we're to generate a prototype

    APLSTYPE aplTypeRes,    // Storage type of the result
             cImmTypeRht;   // Immediate type of right arg first element
    APLINT   aplIntTmp;
    APLUINT  ByteRes;       // # bytes needed in the result
    UINT     uRes;
    UINT     YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = NewYYResIndex ();

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

    //***************************************************************
    // Validate the left argument as a simple numeric scalar or vector
    //   all of whose elements can be coerced to non-negative integers,
    //   and accumulate the product of the dimensions so we know the NELM
    //   of the result.

    // All paths through this switch statement should set
    //   aplRankRes and aplNELMRes.
    //***************************************************************

    // Split cases based upon the left arg's token type
    switch (lptkLftArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkLftArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, stData is an HGLOBAL
            if (!lptkLftArg->tkData.lpSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkLftArg->tkData.lpSym->stData.stGlbData));

                bRet = PrimFnDydRhoLftGlbValid_EM (ClrPtrTypeDirGlb (lptkLftArg->tkData.lpSym->stData.stGlbData),
                                                   &aplRankRes,
                                                   &aplNELMRes,
                                                   lptkFunc);
                break;
            } // End IF

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkLftArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // stData is an immediate
            Assert (lptkLftArg->tkData.lpSym->stFlags.Imm);

            // The left arg's NELM is the rank of the result
            aplRankRes = 1;     // The result is a vector

            // Check for LEFT DOMAIN ERROR
            // Split cases based upon the left arg's token type
            switch (lptkLftArg->tkData.lpSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:      // All Booleans are OK
                    aplNELMRes = (APLNELM) (lptkLftArg->tkData.lpSym->stData.stInteger);

                    break;

                case IMMTYPE_INT:       // Non-negative integers are OK
                    // Ensure the immediate value isn't too large and isn't negative
                    if (lptkLftArg->tkData.lpSym->stData.stInteger > MAX_APLNELM
                     || lptkLftArg->tkData.lpSym->stData.stInteger < 0)
                    {
                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        return NULL;
                    } // End IF

                    aplNELMRes = (APLNELM) (lptkLftArg->tkData.lpSym->stData.stInteger);

                    break;

                case IMMTYPE_FLOAT:     // Ensure it's close enough to a non-negative integer
                    // Convert the value to an integer using System CT
                    aplIntTmp = FloatToAplint_SCT (lptkLftArg->tkData.lpSym->stData.stFloat, &bRet);

                    if (bRet
                     && aplIntTmp < MAX_APLNELM
                     && !SIGN_APLNELM (aplIntTmp))
                    {
                        aplNELMRes = (APLNELM) aplIntTmp;

                        break;
                    } // End IF

                    // Fall through to IMMTYPE_CHAR to handle DOMAIN ERROR

                case IMMTYPE_CHAR:
                    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                               lptkFunc);
                    return NULL;

                defstop
                    return NULL;
            } // End SWITCH

            break;              // Exit to common reshape code

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkLftArg->tkData.tkGlbData));

            bRet = PrimFnDydRhoLftGlbValid_EM (ClrPtrTypeDirGlb (lptkLftArg->tkData.tkGlbData),
                                               &aplRankRes,
                                               &aplNELMRes,
                                               lptkFunc);
            break;

        case TKT_VARIMMED:
            // The left arg's NELM is the rank of the result
            aplRankRes = 1;     // The result is a vector

            // Split cases based upon the left arg's token type
            switch (lptkLftArg->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:      // All Booleans are OK
                    aplNELMRes = (APLNELM) (lptkLftArg->tkData.tkInteger);

                    break;

                case IMMTYPE_INT:       // Non-negative integers are OK
                    // Ensure the immediate value isn't too large and isn't negative
                    if (lptkLftArg->tkData.tkInteger > MAX_APLNELM
                     || lptkLftArg->tkData.tkInteger < 0)
                    {
                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        return NULL;
                    } // End IF

                    aplNELMRes = (APLNELM) (lptkLftArg->tkData.tkInteger);

                    break;

                case IMMTYPE_FLOAT:     // Ensure it's close enough to a non-negative integer
                    // Convert the value to an integer using System CT
                    aplIntTmp = FloatToAplint_SCT (lptkLftArg->tkData.tkFloat, &bRet);

                    if (bRet
                     && aplIntTmp < MAX_APLNELM
                     && !SIGN_APLNELM (aplIntTmp))
                    {
                        aplNELMRes = (APLNELM) aplIntTmp;

                        break;
                    } // End IF

                    // Fall through to IMMTYPE_CHAR to handle DOMAIN ERROR

                case IMMTYPE_CHAR:
                    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                               lptkFunc);
                    return NULL;

                defstop
                    return NULL;
            } // End SWITCH

            break;

        case TKT_LIST:
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkFunc);
            return NULL;

        defstop
            return NULL;
    } // End SWITCH

    // If there's been an error, exit now
    if (!bRet)
        return NULL;

    //***************************************************************
    // Get the storage type of the result and NELM of the right arg
    //***************************************************************

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, stData is an HGLOBAL
            if (!lptkRhtArg->tkData.lpSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.lpSym->stData.stGlbData));

                // Determine the right arg's storage type and NELM
                PrimFnDydRhoRhtGlbType (ClrPtrTypeDirGlb (lptkRhtArg->tkData.lpSym->stData.stGlbData),
                                        &aplTypeRes,
                                        &cImmTypeRht,
                                        &aplNELMRht);
                break;
            } // End IF

            // Handle the immediate case

            // The tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // stData is immediate
            Assert (lptkRhtArg->tkData.lpSym->stFlags.Imm);

            // Only one element in the right arg
            aplNELMRht = 1;

            // Set common var
            cImmTypeRht = lptkRhtArg->tkData.lpSym->stFlags.ImmType;
            aplTypeRes = TranslateImmTypeToArrayType (cImmTypeRht);

            break;

        case TKT_VARIMMED:
            // Only one element in the right arg
            aplNELMRht = 1;

            // Set common var
            cImmTypeRht = lptkRhtArg->tkFlags.ImmType;
            aplTypeRes = TranslateImmTypeToArrayType (cImmTypeRht);

            break;

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

            // Determine the right arg's storage type and NELM
            PrimFnDydRhoRhtGlbType (ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkGlbData),
                                   &aplTypeRes,
                                   &cImmTypeRht,
                                   &aplNELMRht);
            break;

        case TKT_LIST:
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkFunc);
            return NULL;

        defstop
            return NULL;
    } // End SWITCH

    // Check for non-zero result NELM and zero right arg NELM
    if (aplNELMRes NE 0
     && aplNELMRht EQ 0)
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    //***************************************************************
    // If there are no elements in the result,
    //   and the result is a nested array,
    //   make room for a prototype.  Also,
    //   demote the storage type (actually
    //   the prototype) to Boolean if numeric.
    //***************************************************************
    if (aplNELMRes EQ 0)
    {
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_APA:
                aplTypeRes = ARRAY_BOOL;
            case ARRAY_CHAR:
                ByteRes = 0;

                break;

            case ARRAY_HETERO:
                // If the empty result is HETERO, convert the result
                //   type to either char or Boolean.
                aplTypeRes = TranslateImmTypeToArrayType (cImmTypeRht);
                if (aplTypeRes NE ARRAY_CHAR)
                    aplTypeRes = ARRAY_BOOL;

                ByteRes = 0;

                break;

            case ARRAY_NESTED:
                // Because the right arg must be ARRAY_NESTED in order
                //   to get here, various token types and subclasses
                //   can't occur.

                // Split cases based upon the right arg's token type
                switch (lptkRhtArg->tkFlags.TknType)
                {
                    case TKT_VARNAMED:
                        // tkData is an LPSYMENTRY
                        Assert (GetPtrTypeDir (lptkRhtArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

                        // stData is NOT an immediate
                        Assert (!lptkRhtArg->tkData.lpSym->stFlags.Imm);

                        // Get the HGLOBAL
                        hGlbProto = lptkRhtArg->tkData.lpSym->stData.stGlbData;

                        break;

                    case TKT_VARARRAY:
                        // Get the HGLOBAL
                        hGlbProto = lptkRhtArg->tkData.tkGlbData;

                        break;

                    defstop
                        break;
                } // End SWITCH

                // st/tkData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbProto));

                // Check to see if the first element is simple.
                // If so, fill in aplTypeRes; if not, fill in hGlbProto
                //   with the HGLOBAL of the first element.
                if (IsFirstSimpleGlb (&hGlbProto, &aplTypeRes))
                    ByteRes = 0;
                else
                {
                    ByteRes = 1 * sizeof (APLNESTED);

                    // Mark as needing to generate a prototype
                    bPrototype = TRUE;
                } // End IF/ELSE

                break;

            defstop
                return NULL;
        } // End SWITCH

        // Add in the header and <aplRankRes> dimensions
        ByteRes += sizeof (VARARRAY_HEADER)
                 + sizeof (APLDIM) * aplRankRes;
    } else
    {
        //***************************************************************
        // Now that we know the NELM, rank, and storage type of the result, we can
        //   calculate the amount of storage needed for the result
        //***************************************************************

        // Handle APAs specially
        if (aplTypeRes EQ ARRAY_APA)
        {
            // If the right arg is reused, we cannot
            //   store the result as an APA, so use integers
            if (aplNELMRes > aplNELMRht)
                aplTypeRes = ARRAY_INT;
            ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

////////////// If the right arg isn't reused, we can
//////////////   store the result as an APA
////////////if (aplNELMRes <= aplNELMRht)
////////////    ByteRes = sizeof (APLAPA) * 1;
////////////else
////////////{
////////////    ByteRes = sizeof (APLINT) * aplNELMRes;
////////////    aplTypeRes = ARRAY_INT;
////////////} // End IF/ELSE
////////////
////////////// Add in the header and <aplRankRes> dimensions
////////////ByteRes += sizeof (VARARRAY_HEADER)
////////////         + sizeof (APLDIM) * aplRankRes;
        } else
            ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);
////////switch (aplTypeRes)
////////{
////////    case ARRAY_BOOL:            // One bit per value
////////        ByteRes = sizeof (APLBOOL)   * RoundUpBits8 (aplNELMRes);
////////
////////        break;
////////
////////    case ARRAY_INT:
////////        ByteRes = sizeof (APLINT)    * aplNELMRes;
////////
////////        break;
////////
////////    case ARRAY_CHAR:
////////        ByteRes = sizeof (APLCHAR)   * aplNELMRes;
////////
////////        break;
////////
////////    case ARRAY_FLOAT:
////////        ByteRes = sizeof (APLFLOAT)  * aplNELMRes;
////////
////////        break;
////////
////////    case ARRAY_HETERO:
////////        ByteRes = sizeof (APLHETERO) * aplNELMRes;
////////
////////        break;
////////
////////    case ARRAY_NESTED:
////////        ByteRes = sizeof (APLNESTED) * aplNELMRes;
////////
////////        break;
////////
////////    case ARRAY_APA:
////////        // If the right arg isn't reused, we can
////////        //   store the result as an APA
////////        if (aplNELMRes <= aplNELMRht)
////////            ByteRes = sizeof (APLAPA) * 1;
////////        else
////////        {
////////            ByteRes = sizeof (APLINT) * aplNELMRes;
////////            aplTypeRes = ARRAY_INT;
////////        } // End IF/ELSE
////////
////////        break;
////////
////////    defstop
////////        return NULL;
////////} // End SWITCH
    } // End IF/ELSE

////// Add in the header and <aplRankRes> dimensions
////ByteRes += sizeof (VARARRAY_HEADER)
////         + sizeof (APLDIM) * aplRankRes;

    //***************************************************************
    // Now we can allocate the storage for the result.
    // N.B.:  Conversion from APLUINT to UINT.
    //***************************************************************
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the global memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes     ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeaderRes->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeaderRes->ArrType    = aplTypeRes;
////lpHeaderRes->Perm       = 0;
////lpHeaderRes->SysVar     = 0;
    lpHeaderRes->RefCnt     = 1;
    lpHeaderRes->NELM       = aplNELMRes;
    lpHeaderRes->Rank       = aplRankRes;

#undef  lpHeaderRes

    //***************************************************************
    // Now run through the left arg again, copying its data
    //   to the result as its dimensions.
    // Because the left argument's values have already been
    //   validated above, we can skip error checks.
    //***************************************************************

    // Get ptr to the dimensions in the result
    lpDimRes = VarArrayBaseToDim (lpMemRes);

    // Split cases based upon the left arg's token type
    switch (lptkLftArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkLftArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, stData is an HGLOBAL
            if (!lptkLftArg->tkData.lpSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkLftArg->tkData.lpSym->stData.stGlbData));

                // Copy the left argument's values to the result's dimensions
                PrimFnDydRhoLftGlbCopyDim (ClrPtrTypeDirGlb (lptkLftArg->tkData.lpSym->stData.stGlbData),
                                           lpDimRes);
                break;
            } // End IF

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkLftArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // stData is immediate
            Assert (lptkLftArg->tkData.lpSym->stFlags.Imm);

            // Split cases based upon the left arg's immediate type
            switch (lptkLftArg->tkData.lpSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    *lpDimRes = (APLDIM) (lptkLftArg->tkData.lpSym->stData.stBoolean);

                    break;

                case IMMTYPE_INT:
                    *lpDimRes = (APLDIM) (lptkLftArg->tkData.lpSym->stData.stInteger);

                    break;

                case IMMTYPE_FLOAT:
                    *lpDimRes = (APLDIM) (lptkLftArg->tkData.lpSym->stData.stFloat);

                    break;

                case IMMTYPE_CHAR:      // Handled above as DOMAIN ERROR
                defstop
                    break;
            } // End SWITCH

            break;

        case TKT_VARIMMED:
            // Split cases based upon the left arg's immediate type
            switch (lptkLftArg->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    *lpDimRes = (APLDIM) (lptkLftArg->tkData.tkBoolean);

                    break;

                case IMMTYPE_INT:
                    *lpDimRes = (APLDIM) (lptkLftArg->tkData.tkInteger);

                    break;

                case IMMTYPE_FLOAT:
                    *lpDimRes = (APLDIM) (lptkLftArg->tkData.tkFloat);

                    break;

                case IMMTYPE_CHAR:      // Handled above as DOMAIN ERROR
                defstop
                    break;
            } // End SWITCH

            break;

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkLftArg->tkData.tkGlbData));

            // Copy the left argument's values to the result's dimensions
            PrimFnDydRhoLftGlbCopyDim (ClrPtrTypeDirGlb (lptkLftArg->tkData.tkGlbData),
                                       lpDimRes);
            break;

        case TKT_LIST:
        defstop
            break;
    } // End SWITCH
#undef  lpDimRes

    // Now run through the right arg copying its data
    //   to the result as its data

    // Get a ptr to the result's data
    lpDataRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    //***************************************************************
    // If the result is empty, but we need to generate
    //   a prototype, do so now
    //***************************************************************
    if (bPrototype)
    {
        // Make the prototype
        hGlbProto = MakePrototype_EM (ClrPtrTypeDirGlb (hGlbProto),
                                      lptkFunc,
                                      FALSE);   // Allow CHARs
        if (!hGlbProto)
            bRet = FALSE;
        else
            // Save the handle
            *((LPAPLNESTED) lpDataRes) = MakeGlbTypeGlb (hGlbProto);
    } else
    if (aplNELMRes)
    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, stData is an HGLOBAL
            if (!lptkRhtArg->tkData.lpSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.lpSym->stData.stGlbData));

                bRet = PrimFnDydRhoRhtGlbCopyData_EM (ClrPtrTypeDirGlb (lptkRhtArg->tkData.lpSym->stData.stGlbData),
                                                      aplTypeRes,
                                                      aplNELMRes,
                                                      lpDataRes,
                                                      lptkFunc);
                break;
            } // End IF

            // Handle the immediate case

            // The tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // stData is immediate
            Assert (lptkRhtArg->tkData.lpSym->stFlags.Imm);

            // Split cases based upon the right arg's immediate type
            switch (lptkRhtArg->tkData.lpSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                {
                    APLBOOL aplBool;
                    APLNELM uNELM;
                    UINT    uBits;

                    Assert (aplTypeRes EQ ARRAY_BOOL);

                    // If the single value is 1, we're saving all 1s (e.g., 0xFF),
                    //   otherwise, we're saving all 0s (e.g. 0x00).
                    if (lptkRhtArg->tkData.lpSym->stData.stBoolean)
                        aplBool = 0xFF;
                    else
                        aplBool = 0x00;

                    // Smear all 1s or 0s into the result,
                    //   except possibly for the last byte
                    uNELM = aplNELMRes >> LOG2NBIB;
                    for (uRes = 0; uRes < uNELM; uRes++)
                        *((LPAPLBOOL) lpDataRes)++ = aplBool;

                    // # valid bits in last byte
                    uBits = (UINT) (aplNELMRes & MASKLOG2NBIB);
                    if (uBits)
                        *((LPAPLBOOL) lpDataRes) = aplBool & ((1 << uBits) - 1);
                    break;
                } // End IMMTYPE_BOOL

                case IMMTYPE_INT:
                {
                    APLINT aplInt;

                    Assert (aplTypeRes EQ ARRAY_INT);

                    // Copy the single value to avoid recalling it everytime
                    aplInt = lptkRhtArg->tkData.lpSym->stData.stInteger;

                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                        *((LPAPLINT) lpDataRes)++ = aplInt;
                    break;
                } // End IMMTYPE_INT

                case IMMTYPE_FLOAT:
                {
                    APLFLOAT aplFloat;

                    Assert (aplTypeRes EQ ARRAY_FLOAT);

                    // Copy the single value to avoid recalling it everytime
                    aplFloat = lptkRhtArg->tkData.lpSym->stData.stFloat;

                    for (uRes = 0; bRet && uRes < aplNELMRes; uRes++)
                        *((LPAPLFLOAT) lpDataRes)++ = aplFloat;
                    break;
                } // End IMMTYPE_FLOAT

                case IMMTYPE_CHAR:
                {
                    APLCHAR aplChar;

                    Assert (aplTypeRes EQ ARRAY_CHAR);

                    // Copy the single value to avoid recalling it everytime
                    aplChar = lptkRhtArg->tkData.lpSym->stData.stChar;

                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                        *((LPAPLCHAR) lpDataRes)++ = aplChar;
                    break;
                } // End IMMTYPE_CHAR

                defstop
                    break;
            } // End SWITCH

            break;

        case TKT_VARIMMED:
            // Split cases based upon the right arg's immediate type
            switch (lptkRhtArg->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                {
                    APLBOOL aplBool;
                    APLNELM uNELM;
                    UINT    uBits;

                    Assert (aplTypeRes EQ ARRAY_BOOL);

                    // If the single value is 1, we're saving all 1s (e.g., 0xFF),
                    //   otherwise, we're saving all 0s (e.g. 0x00).
                    if (lptkRhtArg->tkData.tkBoolean)
                        aplBool = 0xFF;
                    else
                        aplBool = 0x00;

                    // Smear all 1s or 0s into the result,
                    //   except possibly for the last byte
                    uNELM = aplNELMRes >> LOG2NBIB;
                    for (uRes = 0; uRes < uNELM; uRes++)
                        *((LPAPLBOOL) lpDataRes)++ = aplBool;

                    // # valid bits in last byte
                    uBits = (UINT) (aplNELMRes & MASKLOG2NBIB);
                    if (uBits)
                        *((LPAPLBOOL) lpDataRes) = aplBool & ((1 << uBits) - 1);
                    break;
                } // End IMMTYPE_BOOL

                case IMMTYPE_INT:
                {
                    APLINT aplInt;

                    Assert (aplTypeRes EQ ARRAY_INT);

                    // Copy the single value to avoid recalling it everytime
                    aplInt = lptkRhtArg->tkData.tkInteger;

                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                        *((LPAPLINT) lpDataRes)++ = aplInt;
                    break;
                } // End IMMTYPE_INT

                case IMMTYPE_FLOAT:
                {
                    APLFLOAT aplFloat;

                    Assert (aplTypeRes EQ ARRAY_FLOAT);

                    // Copy the single value to avoid recalling it everytime
                    aplFloat = lptkRhtArg->tkData.tkFloat;

                    for (uRes = 0; bRet && uRes < aplNELMRes; uRes++)
                        *((LPAPLFLOAT) lpDataRes)++ = aplFloat;
                    break;
                } // End IMMTYPE_FLOAT

                case IMMTYPE_CHAR:
                {
                    APLCHAR aplChar;

                    Assert (aplTypeRes EQ ARRAY_CHAR);

                    // Copy the single value to avoid recalling it everytime
                    aplChar = lptkRhtArg->tkData.tkChar;

                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                        *((LPAPLCHAR) lpDataRes)++ = aplChar;
                    break;
                } // End IMMTYPE_CHAR

                defstop
                    break;
            } // End SWITCH

            break;

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

            bRet = PrimFnDydRhoRhtGlbCopyData_EM (ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkGlbData),
                                                  aplTypeRes,
                                                  aplNELMRes,
                                                  lpDataRes,
                                                  lptkFunc);
            break;

        case TKT_LIST:          // Handled above as SYNTAX ERROR
        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Check the result
    if (bRet)
    {
        // Fill in the result token
        YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;    // Already zero from ZeroMemory
////////YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;    // Already zero from ZeroMemory
        YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (TypeDemote (hGlbRes));
        YYRes[YYLclIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        return &YYRes[YYLclIndex];
    } else
        return NULL;
} // End PrimFnDydRho_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydRhoLftGlbValid_EM
//
//  Dyadic rho left argument validation on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydRhoLftGlbValid_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydRhoLftGlbValid_EM
    (HGLOBAL   hGlbLft,
     LPAPLRANK lpaplRankRes,
     LPAPLNELM lpaplNELMRes,
     LPTOKEN   lptkFunc)

{
    LPVOID   lpMemLft,      // Ptr to left argument global memory
             lpDataLft;
    BOOL     bRet = TRUE;
    APLNELM  uNELM;
    UINT     u, uBits;
    APLINT   aplIntTmp;

    // Lock the global memory to get a ptr to it
    lpMemLft = MyGlobalLock (hGlbLft);

#define lpHeaderLft     ((LPVARARRAY_HEADER) lpMemLft)

    // The left arg's NELM is the rank of the result
    *lpaplRankRes = lpHeaderLft->NELM;

    // Check for LEFT RANK ERROR
    if (lpHeaderLft->Rank > 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        bRet = FALSE;
    } else
    {
        // Point to the left arg's data
        lpDataLft = VarArrayBaseToData (lpMemLft, lpHeaderLft->Rank);

        // Check for LEFT DOMAIN ERROR and fill in *lpaplNELMRes
        // Split cases based upon the left arg's array type
        switch (lpHeaderLft->ArrType)
        {
            case ARRAY_BOOL:
                // The result's NELM is either zero or one
                *lpaplNELMRes = 1;

                uNELM = *lpaplRankRes >> LOG2NBIB;
                for (u = 0; *lpaplNELMRes && u < uNELM; u++) // char at a time, except for the last byte
                    *lpaplNELMRes = ((LPAPLBOOL) lpDataLft)[u] EQ 0xFF;

                // Check the last byte

                // # valid bits in last byte
                uBits = (UINT) (*lpaplRankRes & MASKLOG2NBIB);
                if (uBits)                                  // Last byte not full
                    *lpaplNELMRes &= ((LPAPLBOOL) lpDataLft)[u] EQ ((1 << uBits) - 1);

                break;

            case ARRAY_INT:
                *lpaplNELMRes = 1;      // Initialize with identity element for multiply
                for (u = 0; bRet && u < *lpaplRankRes; u++)
                if (((LPAPLINT) lpDataLft)[u] > MAX_APLNELM)
                    bRet = FALSE;
                else
                {
                    // Ensure the value fits into a dimension
                    bRet = ((LPAPLINT) lpDataLft)[u] <= MAX_APLDIM;

                    // Multiply the two numbers as APLINTs so we can check for overflow
                    aplIntTmp = ((APLINT) *lpaplNELMRes) * (APLNELM) ((LPAPLINT) lpDataLft)[u];

                    // Check for overflow
                    bRet = bRet && (aplIntTmp <= MAX_APLNELM);

                    // Save back
                    *lpaplNELMRes = (APLNELM) aplIntTmp;
                } // End FOR/IF/ELSE

                break;

            case ARRAY_FLOAT:
                *lpaplNELMRes = 1;      // Initialize with identity element for multiply
                for (u = 0; bRet && u < *lpaplRankRes; u++)
                if (((LPAPLFLOAT) lpDataLft)[u] > MAX_APLNELM)
                    bRet = FALSE;
                else
                {
                    // Ensure the value fits into a dimension
                    bRet = ((LPAPLFLOAT) lpDataLft)[u] <= MAX_APLDIM;

                    // Multiply the two numbers as APLINTs so we can check for overflow
                    aplIntTmp = ((APLINT) *lpaplNELMRes) * (APLNELM) ((LPAPLFLOAT) lpDataLft)[u];

                    // Check for overflow
                    bRet = bRet && (aplIntTmp <= MAX_APLNELM);

                    // Save back
                    *lpaplNELMRes = (APLNELM) aplIntTmp;
                } // End FOR/IF/ELSE

                break;

            case ARRAY_APA:
            {
                APLINT  apaOff,
                        apaMul,
                        apaVal;
                APLUINT apaLen;

                // Save the APA data
                apaOff = ((LPAPLAPA) lpDataLft)->Off;
                apaMul = ((LPAPLAPA) lpDataLft)->Mul;
                apaLen = ((LPAPLAPA) lpDataLft)->Len;

                // Ensure no element in the left arg goes negative
                //   by checking the first and last elements
                bRet = ((apaOff >= 0) && ((apaOff + apaMul * (apaLen - 1)) >= 0));

                *lpaplNELMRes = 1;      // Initialize with identity element for multiply
                for (apaLen = 0; bRet && apaLen < *lpaplRankRes; apaLen++)
                {
                    apaVal = apaOff + apaMul * apaLen;
                    if (apaVal > MAX_APLNELM)
                        bRet = FALSE;
                    else
                    {
                        // Multiply the two numbers as APLINTs so we can check for overflow
                        aplIntTmp = ((APLINT) *lpaplNELMRes) * apaVal;

                        // Check for overflow
                        bRet = bRet && (aplIntTmp <= MAX_APLNELM);

                        // Save back
                        *lpaplNELMRes = (APLNELM) aplIntTmp;
                    } // End IF/ELSE
                } // End FOR

                break;
            } // End ARRAY_APA

            case ARRAY_CHAR:
            case ARRAY_NESTED:          // We could check for promotion to simple numeric
            case ARRAY_HETERO:          // ...
            case ARRAY_LIST:
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                bRet = FALSE;

                break;

            defstop
                break;
        } // End SWITCH
    } // End IF/ELSE
#undef  lpHeaderLft

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLft); lpMemLft = NULL;

    return bRet;
} // End PrimFnDydRhoLftGlbValid_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydRhoLftGlbCopyDim
//
//  Dyadic rho left argument copying from the left global memory object
//    to the result's dimensions
//***************************************************************************

void PrimFnDydRhoLftGlbCopyDim
    (HGLOBAL   hGlbLft,
     LPAPLDIM  lpaplDim)

{
    LPVOID   lpMemLft,      // Ptr to left argument global memory
             lpDataLft;
    APLNELM  aplNELMLft;
    UINT     uLft, uBitMaskLft;

    // Lock the global memory to get a ptr to it
    lpMemLft = MyGlobalLock (hGlbLft);

#define lpHeaderLft     ((LPVARARRAY_HEADER) lpMemLft)

    // Get the left arg's NELM to use as loop limit
    aplNELMLft = lpHeaderLft->NELM;

    // Point to the left arg's data
    lpDataLft = VarArrayBaseToData (lpMemLft, lpHeaderLft->Rank);

    // Split cases based upon the left arg's array type
    switch (lpHeaderLft->ArrType)
    {
        case ARRAY_BOOL:
            uBitMaskLft = 0x01;
            for (uLft = 0; uLft < aplNELMLft; uLft++)
            {
                *lpaplDim++ = (uBitMaskLft & *(LPAPLBOOL) lpDataLft) ? 1 : 0;

                // Shift over the left arg's bit mask
                uBitMaskLft <<= 1;

                // Check for end-of-byte
                if (uBitMaskLft EQ END_OF_BYTE)
                {
                    uBitMaskLft = 0x01;         // Start over
                    ((LPAPLBOOL) lpDataLft)++;  // Skip to next byte
                } // End IF
            } // End FOR

            break;

        case ARRAY_INT:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) *((LPAPLINT) lpDataLft)++;
            break;

        case ARRAY_FLOAT:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) *((LPAPLFLOAT) lpDataLft)++;
            break;

        case ARRAY_APA:
        {
            APLINT  apaOff,
                    apaMul;
            APLUINT apaLen;

            // Save the APA data
            apaOff = ((LPAPLAPA) lpDataLft)->Off;
            apaMul = ((LPAPLAPA) lpDataLft)->Mul;
////////////apaLen = ((LPAPLAPA) lpDataLft)->Len;

            for (apaLen = 0; apaLen < aplNELMLft; apaLen++)
                *lpaplDim++ = (APLDIM) (apaOff + apaMul * apaLen);
            break;
        } // End ARRAY_APA

        case ARRAY_CHAR:
        case ARRAY_NESTED:          // We could check for promotion to simple numeric
        case ARRAY_HETERO:          // ...
        case ARRAY_LIST:
            break;

        defstop
            break;
    } // End SWITCH
#undef  lpHeaderLft

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
} // End PrimFnDydRhoLftGlbCopyDim


//***************************************************************************
//  PrimFnDydRhoRhtGlbType
//
//  Dyadic rho right argument storage type
//***************************************************************************

void PrimFnDydRhoRhtGlbType
    (HGLOBAL   hGlbRht,
     LPCHAR    lpaplTypeRes,
     LPCHAR    lpcImmTypeRht,
     LPAPLNELM lpaplNELMRht)

{
    LPVOID lpMemRht;

    // Lock the global memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeaderRht     ((LPVARARRAY_HEADER) lpMemRht)

    // Return the right arg's NELM
    *lpaplNELMRht = lpHeaderRht->NELM;

    // Return the right arg's storage type
    *lpaplTypeRes = lpHeaderRht->ArrType;

    // Point to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, lpHeaderRht->Rank);

#undef  lpHeaderRht

    // If the right arg is HETERO, return the
    //   immediate type of its first element
    if (*lpaplTypeRes EQ ARRAY_HETERO)
    {
        LPSYMENTRY lpSym;

        // Data is LPSYMENTRY
        Assert (GetPtrTypeInd (lpMemRht) EQ PTRTYPE_STCONST);

        lpSym = *(LPSYMENTRY *) lpMemRht;

        // stData is an immediate
        Assert (lpSym->stFlags.Imm);

        *lpcImmTypeRht = lpSym->stFlags.ImmType;
    } else
        *lpcImmTypeRht = *lpaplTypeRes;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
} // End PrimFnDydRhoRhtGlbType


//***************************************************************************
//  PrimFnDydRhoRhtGlbCopyData_EM
//
//  Dyadic rho right argument copy data to the result
//***************************************************************************

BOOL PrimFnDydRhoRhtGlbCopyData_EM
    (HGLOBAL   hGlbRht,
     APLSTYPE  aplTypeRes,
     APLNELM   aplNELMRes,
     LPVOID    lpDataRes,
     LPTOKEN   lptkFunc)

{
    LPVOID  lpMemRhtBase,
            lpMemRhtData,
            lpMemRhtNext;
    APLINT  uRes,
            uRht;
    UINT    uBitMaskRes,
            uBitMaskRht;
    BOOL    bRet = TRUE;
    APLINT  aplNELMRht;

    // Lock the global memory to get a ptr to it
    lpMemRhtBase = MyGlobalLock (hGlbRht);

#define lpHeaderRht     ((LPVARARRAY_HEADER) lpMemRhtBase)

    aplNELMRht = lpHeaderRht->NELM;
    lpMemRhtNext = lpMemRhtData = VarArrayBaseToData (lpMemRhtBase, lpHeaderRht->Rank);

    // Split cases based upon the right arg's array type
    switch (lpHeaderRht->ArrType)
    {
        case ARRAY_BOOL:
            // This should be optimized to take advantage of alignment
            //   and 32-bits at a time, maybe even the 386 instructions
            //   SHLD/SHRD.

            uBitMaskRes = uBitMaskRht = 0x01;

            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check to see if we should start over again
                //   the right arg's counter and ptr
                if (uRht EQ (APLNELMSIGN) lpHeaderRht->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                    uBitMaskRht = 0x01;
                } else
                // Check to see if we should start over again
                //   the right arg's bit mask
                if (uBitMaskRht EQ END_OF_BYTE)
                {
                    uBitMaskRht = 0x01;
                    ((LPAPLBOOL) lpMemRhtNext)++;
                } // End IF

                // Check to see if we should start over again
                //   the result's bit mask
                if (uBitMaskRes EQ END_OF_BYTE)
                {
                    uBitMaskRes = 0x01;
                    ((LPAPLBOOL) lpDataRes)++;
                } // End IF

                if (uBitMaskRht & *(LPAPLBOOL) lpMemRhtNext)
                    *((LPAPLBOOL) lpDataRes) |= uBitMaskRes;

                // Shift over both bit masks
                uBitMaskRes <<= 1;
                uBitMaskRht <<= 1;
            } // End FOR

            break;

        case ARRAY_INT:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeaderRht->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                *((LPAPLINT) lpDataRes)++ = *((LPAPLINT) lpMemRhtNext)++;
            } // End FOR

            break;

        case ARRAY_CHAR:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeaderRht->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                *((LPAPLCHAR) lpDataRes)++ = *((LPAPLCHAR) lpMemRhtNext)++;
            } // End FOR

            break;

        case ARRAY_FLOAT:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeaderRht->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                *((LPAPLFLOAT) lpDataRes)++ = *((LPAPLFLOAT) lpMemRhtNext)++;
            } // End FOR

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
        {
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; bRet && uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeaderRht->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                // Split cases based upon the array storage type
                switch (GetPtrTypeInd (lpMemRhtNext))
                {
                    case PTRTYPE_STCONST:
                        // It's an immediate
                        Assert ((*(LPSYMENTRY *) lpMemRhtNext)->stFlags.Imm);

                        // Pass on the LPSYMENTRY
                        *((LPSYMENTRY *) lpDataRes)++ = *(LPSYMENTRY *) lpMemRhtNext;

                        break;

                    case PTRTYPE_HGLOBAL:
                        Assert (lpHeaderRht->ArrType EQ ARRAY_NESTED);

                        // Data is an valid HGLOBAL array
                        Assert (IsGlbTypeVarInd (lpMemRhtNext));

                        // Make a copy of the HGLOBAL ptr
                        *((LPAPLNESTED) lpDataRes)++ = CopySymGlbInd (lpMemRhtNext);

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Skip to next element in right arg
                ((LPAPLNESTED) lpMemRhtNext)++;
            } // End FOR

            break;
        } // End ARRAY_HETERO/ARRAY_NESTED

        case ARRAY_APA:
        {
            APLINT  apaOff,
                    apaMul;
            APLUINT apaLen;

            // Save the APA data
            apaOff = ((LPAPLAPA) lpMemRhtNext)->Off;
            apaMul = ((LPAPLAPA) lpMemRhtNext)->Mul;
            apaLen = ((LPAPLAPA) lpMemRhtNext)->Len;

            // If the right arg isn't reused, we can
            //   store the result as an APA
            if (aplNELMRes <= apaLen)
            {
                // Copy the right arg's Off and MUL, and the
                //   result's NELM
                ((LPAPLAPA) lpDataRes)->Off = apaOff;
                ((LPAPLAPA) lpDataRes)->Mul = apaMul;
                ((LPAPLAPA) lpDataRes)->Len = aplNELMRes;
            } else
            {
                // Loop through the result and right arg copying the data
                for (apaLen = uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, apaLen++)
                {
                    // Check to see if we should start the right arg's
                    //   counter over again
                    if (apaLen EQ lpHeaderRht->NELM)
                        apaLen = 0;

                    *((LPAPLINT) lpDataRes)++ = apaOff + apaMul * apaLen;
                } // End FOR
            } // End IF/ELSE

            break;
        } // End ARRAY_APA

        case ARRAY_LIST:
        defstop
            break;
    } // End SWITCH

#undef  lpHeaderRht

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemRhtBase = lpMemRhtNext = lpMemRhtData = NULL;

    return bRet;
} // End PrimFnDydRhoRhtGlbCopyData_EM


//***************************************************************************
//  End of File: pf_rho.c
//***************************************************************************
