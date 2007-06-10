//***************************************************************************
//  NARS2000 -- Primitive Function -- Rho
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
//  $PrimFnRho_EM_YY
//
//  Primitive function for monadic and dyadic Rho ("shape" and "reshape")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnRho_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnRho_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_RHO);

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
        return PrimFnMonRho_EM_YY             (lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydRho_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnRho_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnRho_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Rho
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnRho_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimProtoFnRho_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnRho_EM_YY,    // Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnRho_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonRho_EM_YY
//
//  Primitive function for monadic Rho ("shape")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonRho_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonRho_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkSym->stData.stGlbData));

                return PrimFnMonRhoGlb_EM_YY
                       (ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkSym->stData.stGlbData),  // HGLOBAL
                        lptkFunc);                                                      // Ptr to function token
            } // End IF

            // Handle the immediate case

            // Fall through to TKT_VARIMMED case to return {zilde}

        case TKT_VARIMMED:          // Return {zilde}
            return PrimFnMonRhoCon_EM_YY (lptkFunc);

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

            return PrimFnMonRhoGlb_EM_YY
                   (ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkGlbData),    // HGLOBAL
                    lptkFunc);                                          // Ptr to function token
        case TKT_LISTPAR:
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkFunc);
            return NULL;

        defstop
            return NULL;
    } // End SWITCH
} // End PrimFnMonRho_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonRhoCon_EM_YY
//
//  Monadic Rho on a symbol table constant
//***************************************************************************

LPYYSTYPE PrimFnMonRhoCon_EM_YY
    (LPTOKEN lptkFunc)

{
    LPYYSTYPE lpYYRes;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // As this is a simple scalar, the result is {zilde}

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbZilde);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End PrimFnMonRhoCon_EM_YY


//***************************************************************************
//  $PrimFnMonRhoGlb_EM_YY
//
//  Monadic Rho on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonRhoGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonRhoGlb_EM_YY
    (HGLOBAL hGlbRht,               // Right arg handle
     LPTOKEN lptkFunc)              // Ptr to function token

{
    LPVOID    lpMemRht,         // Ptr to right arg memory
              lpMemRes;         // ...    result ...
    APLRANK aplRankRht;      // The rank of the array
    HGLOBAL   hGlbRes;          // Result global memory handle
    APLUINT   ByteRes;          // # bytes needed for the result
    BOOL      bRet = TRUE;      // TRUE iff the result is valid
    UINT      uRes;             // Loop counter
    LPYYSTYPE lpYYRes = NULL;   // Ptr to result

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeaderRht     ((LPVARARRAY_HEADER) lpMemRht)

    // Get the rank
    aplRankRht = lpHeaderRht->Rank;

#undef  lpHeaderRht

    // If the right arg rank is zero, the result is {zilde}
    if (aplRankRht EQ 0)
        hGlbRes = hGlbZilde;
    else
    {
        // Calculate space needed for the result
        ByteRes = CalcArraySize (ARRAY_INT, aplRankRht, 1);

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

        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes     ((LPVARARRAY_HEADER) lpMemRes)

        // Fill in the header
        lpHeaderRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeaderRes->ArrType    = ARRAY_INT;
////////lpHeaderRes->Perm       = 0;                // Already zero from GHND
////////lpHeaderRes->SysVar     = 0;                // Already zero from GHND
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
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
ERROR_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

    return lpYYRes;
} // End PrimFnMonRhoGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydRho_EM_YY
//
//  Primitive function for dydadic Rho ("reshape")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydRho_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDydRho_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE aplTypeRht,    // Right arg storage type
             aplTypeRes;    // Result    ...
    APLNELM  aplNELMRht,    // Right arg NELM
             aplNELMRes;    // Result ...
    APLRANK  aplRankRes;    // Result rank
    HGLOBAL  hGlbRes,       // Handle of result's global memory
             hGlbProto;     // ...                prototype
    LPVOID   lpMemRes;      // Ptr to result's global memory
    BOOL     bRet = TRUE,   // TRUE iff the result is valid
             bReshapeSing = FALSE, // TRUE if reshaping an integer singleton
             bPrototype = FALSE; // TRUE iff we're to generate a prototype
    APLUINT  ByteRes;       // # bytes needed for the result
    LPYYSTYPE lpYYRes;      // Ptr to the result

    //***************************************************************
    // Validate the left arg token
    //***************************************************************
    if (!PrimFnDydRhoLftValid_EM (lptkLftArg,       // Ptr to left arg token
                                 &aplRankRes,       // Ptr to result rank
                                 &aplNELMRes,       // Ptr to result NELM
                                  lptkFunc))        // Ptr to function token
        return NULL;

    //***************************************************************
    // Get the attributes (Type, NELM, and Rank) of the right arg
    //***************************************************************
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, NULL);

    // Normally, the result storage type is the same as that of the right arg
    aplTypeRes = aplTypeRht;

    //***************************************************************
    // Check for non-zero result NELM and zero right arg NELM
    //***************************************************************
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
////////////////ByteRes = 0;

                break;

            case ARRAY_HETERO:
                // If the empty result is HETERO, convert the result
                //   type to either char or Boolean.

                // Get the storage type of the first value
                FirstValue (lptkRhtArg,     // Ptr to right arg token
                            NULL,           // Ptr to integer result
                            NULL,           // Ptr to float ...
                            NULL,           // Ptr to WCHAR ...
                            NULL,           // Ptr to longest ...
                            NULL,           // Ptr to lpSym/Glb ...
                            NULL,           // Ptr to ...immediate type ...
                           &aplTypeRes);    // Ptr to array type ...
                // If it's not char, it's Boolean
                if (aplTypeRes NE ARRAY_CHAR)
                    aplTypeRes = ARRAY_BOOL;

////////////////ByteRes = 0;

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
                        Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

                        // stData is NOT an immediate
                        Assert (!lptkRhtArg->tkData.tkSym->stFlags.Imm);

                        // Get the HGLOBAL
                        hGlbProto = lptkRhtArg->tkData.tkSym->stData.stGlbData;

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
                {
////////////////////ByteRes = 0;
                } else
                {
////////////////////ByteRes = 1 * sizeof (APLNESTED);

                    // Mark as needing to generate a prototype
                    bPrototype = TRUE;
                } // End IF/ELSE

                break;

            defstop
                return NULL;
        } // End SWITCH

////////// Add in the header and <aplRankRes> dimensions
////////ByteRes += sizeof (VARARRAY_HEADER)
////////         + sizeof (APLDIM) * aplRankRes;
    } // End IF

    //***************************************************************
    // Now that we know the NELM, rank, and storage type of the result, we can
    //   calculate the amount of storage needed for the result
    //***************************************************************

    //***************************************************************
    // Handle APAs specially
    //***************************************************************
    if (IsSimpleInt (aplTypeRht)    // If right arg is simple integer (BOOL, INT, APA),
     && aplNELMRht EQ 1             //   and a singleton, ...
     && aplNELMRes NE 0)            //   and not an empty result
    {
        aplTypeRes = ARRAY_APA;     // Result is an APA
        bReshapeSing = TRUE;        // Mark as special handling
    } else
    if (aplTypeRes EQ ARRAY_APA)
    {
        // If the right arg is reused, we cannot
        //   store the result as an APA, so use integers
        if (aplNELMRes > aplNELMRht)
            aplTypeRes = ARRAY_INT;
    } // End IF

    //***************************************************************
    // Calculate space needed for the result
    //***************************************************************
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

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

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes     ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeaderRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeaderRes->ArrType    = aplTypeRes;
////lpHeaderRes->Perm       = 0;        // ALready zero from GHND
////lpHeaderRes->SysVar     = 0;        // Already zero from GHND
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
    PrimFnDydRhoCopyDim (lpMemRes, lptkLftArg);

    //***************************************************************
    // Now run through the right arg copying its data
    //   to the result as its data
    //***************************************************************

    // Check for reshaping an integer singleton
    if (bReshapeSing)
    {
        // Get a ptr to the result's data
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

#define lpAPA       ((LPAPLAPA) lpMemRes)
        // Get the first value from the right arg
        FirstValue (lptkRhtArg,     // Ptr to right arg token
                   &lpAPA->Off,     // Ptr to integer result
                    NULL,           // Ptr to float ...
                    NULL,           // Ptr to WCHAR ...
                    NULL,           // Ptr to longest ...
                    NULL,           // Ptr to lpSym/Glb ...
                    NULL,           // Ptr to ...immediate type ...
                    NULL);          // Ptr to array type ...
        lpAPA->Mul = 0;
        lpAPA->Len = aplNELMRes;
#undef  lpAPA
    } else
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
        {
            // Get a ptr to the result's data
            lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

            // Save the handle
            *((LPAPLNESTED) lpMemRes) = MakeGlbTypeGlb (hGlbProto);
        } // End IF
    } else
    if (aplNELMRes)
        bRet =
        PrimFnDydRhoRhtCopyData (lptkRhtArg,        // Ptr to right arg token
                                 aplTypeRes,        // Result storage type
                                 aplNELMRes,        // Result NELM
                                 aplRankRes,        // Result rank
                                 lpMemRes,          // Ptr to result memory
                                 lptkFunc);         // Ptr to function token
    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Check the result
    if (bRet)
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        // See if it fits into a lower (but not necessarily smaller) datatype
        lpYYRes->tkToken = *TypeDemote (&lpYYRes->tkToken);

        return lpYYRes;
    } else
    // If we failed, release the allocated storage
    {
        // We no longer need this storage
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;

        return NULL;
    } // End IF
} // End PrimFnDydRho_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydRhoRhtCopyData
//
//  Copy data to the result
//***************************************************************************

BOOL PrimFnDydRhoRhtCopyData
    (LPTOKEN  lptkRhtArg,           // Ptr to right arg token
     APLSTYPE aplTypeRes,           // Result storage type
     APLNELM  aplNELMRes,           // Result NELM
     APLRANK  aplRankRes,           // Result rank
     LPVOID   lpMemRes,             // Ptr to result memory
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    BOOL   bRet = TRUE;     // TRUE iff result is valid
    UINT   uRes;            // Loop counter

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, stData is an HGLOBAL
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkSym->stData.stGlbData));

                bRet = PrimFnDydRhoRhtGlbCopyData_EM (ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkSym->stData.stGlbData),
                                                      aplTypeRes,
                                                      aplNELMRes,
                                                      lpMemRes,
                                                      lptkFunc);
                break;
            } // End IF

            // Handle the immediate case

            // The tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // stData is immediate
            Assert (lptkRhtArg->tkData.tkSym->stFlags.Imm);

            // Split cases based upon the right arg's immediate type
            switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                {
                    APLBOOL aplBool;
                    APLNELM uNELM;
                    UINT    uBits;

                    Assert (aplTypeRes EQ ARRAY_BOOL);

                    // If the single value is 1, we're saving all 1s (e.g., 0xFF),
                    //   otherwise, we're saving all 0s (e.g. 0x00).
                    if (lptkRhtArg->tkData.tkSym->stData.stBoolean)
                        aplBool = 0xFF;
                    else
                        aplBool = 0x00;

                    // Smear all 1s or 0s into the result,
                    //   except possibly for the last byte
                    uNELM = aplNELMRes >> LOG2NBIB;
                    for (uRes = 0; uRes < uNELM; uRes++)
                        *((LPAPLBOOL) lpMemRes)++ = aplBool;

                    // # valid bits in last byte
                    uBits = (UINT) (aplNELMRes & MASKLOG2NBIB);
                    if (uBits)
                        *((LPAPLBOOL) lpMemRes) = aplBool & ((1 << uBits) - 1);
                    break;
                } // End IMMTYPE_BOOL

                case IMMTYPE_INT:
                {
                    APLINT aplInt;

                    Assert (aplTypeRes EQ ARRAY_INT);

                    // Copy the single value to avoid recalling it everytime
                    aplInt = lptkRhtArg->tkData.tkSym->stData.stInteger;

                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                        *((LPAPLINT) lpMemRes)++ = aplInt;
                    break;
                } // End IMMTYPE_INT

                case IMMTYPE_FLOAT:
                {
                    APLFLOAT aplFloat;

                    Assert (aplTypeRes EQ ARRAY_FLOAT);

                    // Copy the single value to avoid recalling it everytime
                    aplFloat = lptkRhtArg->tkData.tkSym->stData.stFloat;

                    for (uRes = 0; bRet && uRes < aplNELMRes; uRes++)
                        *((LPAPLFLOAT) lpMemRes)++ = aplFloat;
                    break;
                } // End IMMTYPE_FLOAT

                case IMMTYPE_CHAR:
                {
                    APLCHAR aplChar;

                    Assert (aplTypeRes EQ ARRAY_CHAR);

                    // Copy the single value to avoid recalling it everytime
                    aplChar = lptkRhtArg->tkData.tkSym->stData.stChar;

                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                        *((LPAPLCHAR) lpMemRes)++ = aplChar;
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
                        *((LPAPLBOOL) lpMemRes)++ = aplBool;

                    // # valid bits in last byte
                    uBits = (UINT) (aplNELMRes & MASKLOG2NBIB);
                    if (uBits)
                        *((LPAPLBOOL) lpMemRes) = aplBool & ((1 << uBits) - 1);
                    break;
                } // End IMMTYPE_BOOL

                case IMMTYPE_INT:
                {
                    APLINT aplInt;

                    Assert (aplTypeRes EQ ARRAY_INT);

                    // Copy the single value to avoid recalling it everytime
                    aplInt = lptkRhtArg->tkData.tkInteger;

                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                        *((LPAPLINT) lpMemRes)++ = aplInt;
                    break;
                } // End IMMTYPE_INT

                case IMMTYPE_FLOAT:
                {
                    APLFLOAT aplFloat;

                    Assert (aplTypeRes EQ ARRAY_FLOAT);

                    // Copy the single value to avoid recalling it everytime
                    aplFloat = lptkRhtArg->tkData.tkFloat;

                    for (uRes = 0; bRet && uRes < aplNELMRes; uRes++)
                        *((LPAPLFLOAT) lpMemRes)++ = aplFloat;
                    break;
                } // End IMMTYPE_FLOAT

                case IMMTYPE_CHAR:
                {
                    APLCHAR aplChar;

                    Assert (aplTypeRes EQ ARRAY_CHAR);

                    // Copy the single value to avoid recalling it everytime
                    aplChar = lptkRhtArg->tkData.tkChar;

                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                        *((LPAPLCHAR) lpMemRes)++ = aplChar;
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
                                                  lpMemRes,
                                                  lptkFunc);
            break;

        defstop
            break;
    } // End SWITCH

    return bRet;
} // End PrimFnDydRhoRhtCopyData


//***************************************************************************
//  $PrimFnDydRhoCopyDim
//
//  Copy dimensions to the result of dyadic Rho
//***************************************************************************

void PrimFnDydRhoCopyDim
    (LPVOID  lpMemRes,      // Ptr to result memory
     LPTOKEN lptkLftArg)    // Ptr to left arg token

{
    LPAPLDIM  lpDimRes;     // Ptr to result's dimensions

    // Get ptr to the dimensions in the result
    lpDimRes = VarArrayBaseToDim (lpMemRes);

    // Split cases based upon the left arg's token type
    switch (lptkLftArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkLftArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, stData is an HGLOBAL
            if (!lptkLftArg->tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkLftArg->tkData.tkSym->stData.stGlbData));

                // Copy the left argument's values to the result's dimensions
                PrimFnDydRhoLftGlbCopyDim (ClrPtrTypeDirGlb (lptkLftArg->tkData.tkSym->stData.stGlbData),
                                           lpDimRes);
                break;
            } // End IF

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkLftArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // stData is immediate
            Assert (lptkLftArg->tkData.tkSym->stFlags.Imm);

            // Split cases based upon the left arg's immediate type
            switch (lptkLftArg->tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    *lpDimRes = (APLDIM) (lptkLftArg->tkData.tkSym->stData.stBoolean);

                    break;

                case IMMTYPE_INT:
                    *lpDimRes = (APLDIM) (lptkLftArg->tkData.tkSym->stData.stInteger);

                    break;

                case IMMTYPE_FLOAT:
                    *lpDimRes = (APLDIM) (lptkLftArg->tkData.tkSym->stData.stFloat);

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

        defstop
            break;
    } // End SWITCH
} // End PrimFnDydRhoCopyDim


//***************************************************************************
//  $PrimFnDydRhoLftValid_EM
//
//  Validate the left argument as a simple numeric scalar or vector
//    all of whose elements can be coerced to non-negative integers,
//    and accumulate the product of the dimensions so we know the NELM
//    of the result.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydRhoLftValid_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydRhoLftValid_EM
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token
     LPAPLRANK lpaplRankRes,        // Ptr to result rank
     LPAPLNELM lpaplNELMRes,        // Ptr to result NELM
     LPTOKEN   lptkFunc)            // Ptr to function token

{
    BOOL   bRet = TRUE;
    APLINT aplIntTmp;

    // All paths through this switch statement should set
    //   aplRankRes and aplNELMRes.

    // Split cases based upon the left arg's token type
    switch (lptkLftArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkLftArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, stData is an HGLOBAL
            if (!lptkLftArg->tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkLftArg->tkData.tkSym->stData.stGlbData));

                bRet = PrimFnDydRhoLftGlbValid_EM (ClrPtrTypeDirGlb (lptkLftArg->tkData.tkSym->stData.stGlbData),
                                                   lpaplRankRes,
                                                   lpaplNELMRes,
                                                   lptkFunc);
                break;
            } // End IF

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkLftArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // stData is an immediate
            Assert (lptkLftArg->tkData.tkSym->stFlags.Imm);

            // The left arg's NELM is the rank of the result
            *lpaplRankRes = 1;      // The result is a vector

            // Check for LEFT DOMAIN ERROR
            // Split cases based upon the left arg's token type
            switch (lptkLftArg->tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:      // All Booleans are OK
                    *lpaplNELMRes = (APLNELM) (lptkLftArg->tkData.tkSym->stData.stInteger);

                    break;

                case IMMTYPE_INT:       // Non-negative integers are OK
                    // Ensure the immediate value isn't too large and isn't negative
                    if (lptkLftArg->tkData.tkSym->stData.stInteger > MAX_APLNELM
                     || lptkLftArg->tkData.tkSym->stData.stInteger < 0)
                    {
                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        return FALSE;
                    } // End IF

                    *lpaplNELMRes = (APLNELM) (lptkLftArg->tkData.tkSym->stData.stInteger);

                    break;

                case IMMTYPE_FLOAT:     // Ensure it's close enough to a non-negative integer
                    // Convert the value to an integer using System CT
                    aplIntTmp = FloatToAplint_SCT (lptkLftArg->tkData.tkSym->stData.stFloat, &bRet);

                    if (bRet
                     && aplIntTmp < MAX_APLNELM
                     && !SIGN_APLNELM (aplIntTmp))
                    {
                        *lpaplNELMRes = (APLNELM) aplIntTmp;

                        break;
                    } // End IF

                    // Fall through to IMMTYPE_CHAR to handle DOMAIN ERROR

                case IMMTYPE_CHAR:
                    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                               lptkFunc);
                    return FALSE;

                defstop
                    return FALSE;
            } // End SWITCH

            break;              // Exit to common reshape code

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkLftArg->tkData.tkGlbData));

            bRet = PrimFnDydRhoLftGlbValid_EM (ClrPtrTypeDirGlb (lptkLftArg->tkData.tkGlbData),
                                               lpaplRankRes,
                                               lpaplNELMRes,
                                               lptkFunc);
            break;

        case TKT_VARIMMED:
            // The left arg's NELM is the rank of the result
            *lpaplRankRes = 1;      // The result is a vector

            // Split cases based upon the left arg's token type
            switch (lptkLftArg->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:      // All Booleans are OK
                    *lpaplNELMRes = (APLNELM) (lptkLftArg->tkData.tkInteger);

                    break;

                case IMMTYPE_INT:       // Non-negative integers are OK
                    // Ensure the immediate value isn't too large and isn't negative
                    if (lptkLftArg->tkData.tkInteger > MAX_APLNELM
                     || lptkLftArg->tkData.tkInteger < 0)
                    {
                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        return FALSE;
                    } // End IF

                    *lpaplNELMRes = (APLNELM) (lptkLftArg->tkData.tkInteger);

                    break;

                case IMMTYPE_FLOAT:     // Ensure it's close enough to a non-negative integer
                    // Convert the value to an integer using System CT
                    aplIntTmp = FloatToAplint_SCT (lptkLftArg->tkData.tkFloat, &bRet);

                    if (bRet
                     && aplIntTmp < MAX_APLNELM
                     && !SIGN_APLNELM (aplIntTmp))
                    {
                        *lpaplNELMRes = (APLNELM) aplIntTmp;

                        break;
                    } // End IF

                    // Fall through to IMMTYPE_CHAR to handle DOMAIN ERROR

                case IMMTYPE_CHAR:
                    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                               lptkFunc);
                    return FALSE;

                defstop
                    return FALSE;
            } // End SWITCH

            break;

        case TKT_LISTPAR:
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkFunc);
            return FALSE;

        defstop
            return FALSE;
    } // End SWITCH

    return bRet;
} // End PrimFnDydRhoLftValid_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydRhoLftGlbValid_EM
//
//  Dyadic Rho left argument validation on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydRhoLftGlbValid_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydRhoLftGlbValid_EM
    (HGLOBAL   hGlbLft,                 // Left arg handle
     LPAPLRANK lpaplRankRes,            // Ptr to result rank
     LPAPLNELM lpaplNELMRes,            // Ptr to result NELM
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    LPVOID   lpMemLft,      // Ptr to left argument global memory
             lpDataLft;
    APLSTYPE aplTypeLft;
    APLNELM  aplNELMLft;
    APLRANK  aplRankLft;
    BOOL     bRet = TRUE;
    APLNELM  uNELM;
    UINT     u,
             uBits;
    APLINT   aplIntTmp;
    LPWCHAR  lpErrMsg;

    // Lock the memory to get a ptr to it
    lpMemLft = MyGlobalLock (hGlbLft);

#define lpHeaderLft     ((LPVARARRAY_HEADER) lpMemLft)
    aplTypeLft = lpHeaderLft->ArrType;
    aplNELMLft = lpHeaderLft->NELM;
    aplRankLft = lpHeaderLft->Rank;
#undef  lpHeaderLft

    // The left arg's NELM is the rank of the result
    *lpaplRankRes = aplNELMLft;

    // Check for LEFT RANK ERROR
    if (aplRankLft > 1)
    {
        lpErrMsg = ERRMSG_RANK_ERROR APPEND_NAME;
        bRet = FALSE;
    } else
    {
        // Point to the left arg's data
        lpDataLft = VarArrayBaseToData (lpMemLft, aplRankLft);

        // Check for LEFT DOMAIN ERROR and fill in *lpaplNELMRes
        // Split cases based upon the left arg's storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
                // The result's NELM is either zero or one
                *lpaplNELMRes = 1;

                uNELM = aplNELMLft >> LOG2NBIB;
                for (u = 0; *lpaplNELMRes && u < uNELM; u++) // char at a time, except for the last byte
                    *lpaplNELMRes = ((LPAPLBOOL) lpDataLft)[u] EQ 0xFF;

                // Check the last byte

                // # valid bits in last byte
                uBits = (UINT) (aplNELMLft & MASKLOG2NBIB);
                if (uBits)                                  // Last byte not full
                    *lpaplNELMRes &= ((LPAPLBOOL) lpDataLft)[u] EQ ((1 << uBits) - 1);

                break;

            case ARRAY_INT:
                *lpaplNELMRes = 1;      // Initialize with identity element for multiply
                for (u = 0; bRet && u < aplNELMLft; u++)
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
                for (u = 0; bRet && u < aplNELMLft; u++)
                {
                    aplIntTmp = FloatToAplint_SCT (((LPAPLFLOAT) lpDataLft)[u], &bRet);
                    if ((!bRet) || aplIntTmp > MAX_APLNELM)
                        bRet = FALSE;
                    else
                    {
                        // Ensure the value fits into a dimension
                            bRet = aplIntTmp <= MAX_APLDIM;

                        // Multiply the two numbers as APLINTs so we can check for overflow
                            aplIntTmp = imul64 ((APLINT) *lpaplNELMRes, (APLNELM) aplIntTmp, &bRet);

                        // Check for overflow
                        bRet = bRet && (aplIntTmp <= MAX_APLNELM);

                        // Save back
                        *lpaplNELMRes = (APLNELM) aplIntTmp;
                    } // End IF/ELSE
                } // End FOR

                break;

            case ARRAY_APA:
            {
                APLINT  apaOff,
                        apaMul,
                        apaVal;
                APLUINT apaLen;


                // Save the APA data
#define lpAPA       ((LPAPLAPA) lpDataLft)
                apaOff = lpAPA->Off;
                apaMul = lpAPA->Mul;
                apaLen = lpAPA->Len;
#undef  lpAPA
                // Ensure no element in the left arg goes negative
                //   by checking the first and last elements
                bRet = ((apaOff >= 0) && ((apaOff + apaMul * (apaLen - 1)) >= 0));

                *lpaplNELMRes = 1;      // Initialize with identity element for multiply
                for (apaLen = 0; bRet && apaLen < aplNELMLft; apaLen++)
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
                bRet = FALSE;

                break;

            defstop
                break;
        } // End SWITCH

        // If error, it's a DOMAIN ERROR
        if (!bRet)
            lpErrMsg = ERRMSG_DOMAIN_ERROR APPEND_NAME;
    } // End IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLft); lpMemLft = NULL;

    // If error, set the error msg
    if (!bRet)
        ErrorMessageIndirectToken (lpErrMsg,
                                   lptkFunc);
    return bRet;
} // End PrimFnDydRhoLftGlbValid_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydRhoLftGlbCopyDim
//
//  Dyadic Rho left argument copying from the left global memory object
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

    // Lock the memory to get a ptr to it
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
#define lpAPA       ((LPAPLAPA) lpDataLft)
            apaOff = lpAPA->Off;
            apaMul = lpAPA->Mul;
#undef  lpAPA
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
//  $PrimFnDydRhoRhtGlbCopyData_EM
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

    // Lock the memory to get a ptr to it
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

        case ARRAY_NESTED:
            // Take into account the nested prototype
            aplNELMRes = max (aplNELMRes, 1);
        case ARRAY_HETERO:
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

        case ARRAY_APA:
        {
            APLINT  apaOff,
                    apaMul;
            APLUINT apaLen;

            // Save the APA data
#define lpAPA       ((LPAPLAPA) lpMemRhtNext)
            apaOff = lpAPA->Off;
            apaMul = lpAPA->Mul;
            apaLen = lpAPA->Len;
#undef  lpAPA
            // If the right arg isn't reused, we can
            //   store the result as an APA
            if (aplNELMRes <= apaLen)
            {
                // Copy the right arg's Off and MUL, and the
                //   result's NELM
#define lpAPA       ((LPAPLAPA) lpDataRes)
                lpAPA->Off = apaOff;
                lpAPA->Mul = apaMul;
                lpAPA->Len = aplNELMRes;
#undef  lpAPA
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
