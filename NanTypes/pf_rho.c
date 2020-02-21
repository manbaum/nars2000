//***************************************************************************
//  NARS2000 -- Primitive Function -- Rho
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2020 Sudley Place Software

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
//  $PrimFnRho_EM_YY
//
//  Primitive function for monadic and dyadic Rho ("shape" and "reshape")
//***************************************************************************

LPPL_YYSTYPE PrimFnRho_EM_YY
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
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonRho_EM_YY             (lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydRho_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnRho_EM_YY


//***************************************************************************
//  $PrimProtoFnRho_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Rho
//***************************************************************************

LPPL_YYSTYPE PrimProtoFnRho_EM_YY
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


//***************************************************************************
//  $PrimIdentFnRho_EM_YY
//
//  Generate an identity element for the primitive function dyadic Rho
//***************************************************************************

LPPL_YYSTYPE PrimIdentFnRho_EM_YY
    (LPTOKEN lptkRhtOrig,           // Ptr to original right arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig NE NULL);
    Assert (lptkFunc    NE NULL);
    Assert (lptkRhtArg  NE NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // The (left) identity function for dyadic Rho
    //   (L {rho} R) ("reshape") is
    //   {rho} R.
    return
      PrimFnMonRho_EM_YY (lptkFunc,     // Ptr to function token
                          lptkRhtArg,   // Ptr to right arg token
                          NULL);        // Ptr to axis token (may be NULL)
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimIdentFnRho_EM_YY


//***************************************************************************
//  $PrimFnMonRho_EM_YY
//
//  Primitive function for monadic Rho ("shape")
//***************************************************************************

LPPL_YYSTYPE PrimFnMonRho_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

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
                Assert (IsGlbTypeVarDir_PTB (lptkRhtArg->tkData.tkSym->stData.stGlbData));

                return PrimFnMonRhoGlb_EM_YY
                       (lptkRhtArg->tkData.tkSym->stData.stGlbData, // HGLOBAL
                        lptkFunc);                                  // Ptr to function token
            } // End IF

            // Handle the immediate case

            // Fall through to TKT_VARIMMED case to return {zilde}

        case TKT_VARIMMED:          // Return {zilde}
            return PrimFnMonRhoCon_EM_YY (lptkFunc);

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (lptkRhtArg->tkData.tkGlbData));

            return PrimFnMonRhoGlb_EM_YY
                   (lptkRhtArg->tkData.tkGlbData,                   // HGLOBAL
                    lptkFunc);                                      // Ptr to function token
        defstop
            return NULL;
    } // End SWITCH

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnMonRho_EM_YY


//***************************************************************************
//  $PrimFnMonRhoCon_EM_YY
//
//  Monadic Rho on a symbol table constant
//***************************************************************************

LPPL_YYSTYPE PrimFnMonRhoCon_EM_YY
    (LPTOKEN lptkFunc)                  // Ptr to function token

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // As this is a simple scalar, the result is {zilde}

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbZilde);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End PrimFnMonRhoCon_EM_YY


//***************************************************************************
//  $PrimFnMonRhoGlb_EM_YY
//
//  Monadic Rho on a global memory object
//***************************************************************************

LPPL_YYSTYPE PrimFnMonRhoGlb_EM_YY
    (HGLOBAL hGlbRht,                   // Right arg handle
     LPTOKEN lptkFunc)                  // Ptr to function token

{
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPVOID            lpMemRht,             // Ptr to right arg memory
                      lpMemRes;             // ...    result ...
    APLRANK           aplRankRht;           // The rank of the array
    HGLOBAL           hGlbRes;              // Result global memory handle
    APLUINT           ByteRes;              // # bytes in the result
    UINT              uRes;                 // Loop counter
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLockVar (hGlbRht);

#define lpHeader        lpMemHdrRht
    // Get the rank
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // If the right arg rank is zero, the result is {zilde}
    if (IsScalar (aplRankRht))
        hGlbRes = hGlbZilde;
    else
    {
        // Calculate space needed for the result
        ByteRes = CalcArraySize (ARRAY_INT, aplRankRht, 1);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Allocate space for one dimension and <aplRankRht> integers
        hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        lpMemHdrRes
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_INT;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplRankRht;
        lpHeader->Rank       = 1;
#undef  lpHeader

        // Save the dimension
        *VarArrayBaseToDim (lpMemHdrRes) = aplRankRht;

        // Skip over the header and dimension to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Skip over the header to the right arg's dimensions
        lpMemRht = VarArrayBaseToDim (lpMemHdrRht);

#define lpDimRht        ((LPAPLDIM) lpMemRht)
#define lpDataRes       ((LPAPLINT) lpMemRes)

        // Copy the dimensions to the result
        for (uRes = 0; uRes < aplRankRht; uRes++)
            *lpDataRes++ = *lpDimRht++;

#undef  lpDimRht
#undef  lpDataRes

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

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

    return lpYYRes;
} // End PrimFnMonRhoGlb_EM_YY


//***************************************************************************
//  $PrimFnDydRho_EM_YY
//
//  Primitive function for dydadic Rho ("reshape")
//***************************************************************************

LPPL_YYSTYPE PrimFnDydRho_EM_YY
    (LPTOKEN lptkLftArg,                    // Ptr to left arg token
     LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis)                      // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeRes;           // Result    ...
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMRes;           // Result ...
    APLRANK           aplRankRes;           // Result rank
    HGLOBAL           hGlbRes = NULL,       // Handle of result's global memory
                      hSymGlbProto;         // ...                prototype
    LPVOID            lpMemRes;             // Ptr to result's global memory
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result global memory header
    UBOOL             bRet = TRUE,          // TRUE iff result is valid
                      bReshapeSing = FALSE, // TRUE if reshaping an integer singleton
                      bPrototype = FALSE;   // TRUE iff we're to generate a prototype
    APLUINT           ByteRes;              // # bytes in the result
    LPPL_YYSTYPE      lpYYRes;              // Ptr to the result

    //***************************************************************
    // Validate the left arg token
    //***************************************************************
    if (!PrimFnDydRhoLftValid_EM (lptkLftArg,       // Ptr to left arg token
                                 &aplRankRes,       // Ptr to result rank
                                 &aplNELMRes,       // Ptr to result NELM
                                  lptkFunc))        // Ptr to function token
        goto ERROR_EXIT;

    //***************************************************************
    // Get the attributes (Type, NELM, and Rank) of the right arg
    //***************************************************************
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, NULL, NULL);

    // Normally, the result storage type is the same as that of the right arg
    aplTypeRes = aplTypeRht;

    //***************************************************************
    // If there are no elements in the result,
    //   and the result is a nested array,
    //   make room for a prototype.  Also,
    //   demote the storage type (actually
    //   the prototype) to Boolean if BOOL/INT/FLOAT/APA.
    //***************************************************************
    if (IsEmpty (aplNELMRes))
    {
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_APA:
            case ARRAY_HC2I:
            case ARRAY_HC2F:
            case ARRAY_HC4I:
            case ARRAY_HC4F:
            case ARRAY_HC8I:
            case ARRAY_HC8F:
                aplTypeRes = ARRAY_BOOL;

                break;

            case ARRAY_RAT:
            case ARRAY_VFP:
            case ARRAY_HC2R:
            case ARRAY_HC2V:
            case ARRAY_HC4R:
            case ARRAY_HC4V:
            case ARRAY_HC8R:
            case ARRAY_HC8V:
            case ARRAY_BA1F:
            case ARRAY_BA2F:
            case ARRAY_BA4F:
            case ARRAY_BA8F:
            case ARRAY_CHAR:
////////////////aplTypeRes = aplTypeRes;

                break;

            case ARRAY_HETERO:
                // If the empty result is HETERO, convert the result
                //   type to either char or Boolean.

                // Get the storage type of the first value
                GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                                    NULL,           // Ptr to integer result
                                    NULL,           // Ptr to float ...
                                    NULL,           // Ptr to WCHAR ...
                                    NULL,           // Ptr to longest ...
                                    NULL,           // Ptr to lpSym/Glb ...
                                    NULL,           // Ptr to ...immediate type ...
                                   &aplTypeRes);    // Ptr to array type ...
                // If it's not char, it's Boolean
                if (!IsSimpleChar (aplTypeRes))
                    aplTypeRes = ARRAY_BOOL;
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
                        hSymGlbProto = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                        break;

                    case TKT_VARARRAY:
                        // Get the HGLOBAL
                        hSymGlbProto = lptkRhtArg->tkData.tkGlbData;

                        break;

                    defstop
                        break;
                } // End SWITCH

                // st/tkData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hSymGlbProto));

                // Check to see if the first element is simple.
                // If so, fill in aplTypeRes; if not, fill in hSymGlbProto
                //   with the HGLOBAL of the first element.
                if (!IsFirstSimpleGlb (&hSymGlbProto, &aplTypeRes))
                    // Mark as needing to generate a prototype
                    bPrototype = TRUE;

                break;

            defstop
                goto ERROR_EXIT;
        } // End SWITCH
    } // End IF

    //***************************************************************
    // Handle APAs specially
    //***************************************************************
    if (IsRealBIA (aplTypeRht)    // If right arg is simple integer (BOOL, INT, APA),
     && IsSingleton (aplNELMRht)    //   and a singleton, ...
     && IsMultiNELM (aplNELMRes))   //   and the result has multiple elements
    {
        aplTypeRes = ARRAY_APA;     // Result is an APA
        bReshapeSing = TRUE;        // Mark as special handling
    } else
    if (IsSimpleAPA (aplTypeRes))
    {
        // If the right arg is reused, we cannot
        //   store the result as an APA, so use integers
        if (aplNELMRes > aplNELMRht)
            aplTypeRes = ARRAY_INT;
    } // End IF

    //***************************************************************
    // Now that we know the NELM, rank, and storage type of the result, we can
    //   calculate the amount of storage needed for the result
    //***************************************************************

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
    // Now we can allocate the storage for the result.
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

    //***************************************************************
    // Now run through the left arg again, copying its data
    //   to the result as its dimensions.
    // Because the left argument's values have already been
    //   validated above, we can skip error checks.
    //***************************************************************
    PrimFnDydRhoCopyDim (lpMemHdrRes, lptkLftArg);

    //***************************************************************
    // Now run through the right arg copying its data
    //   to the result as its data
    //***************************************************************

    // Check for reshaping an integer singleton
    if (bReshapeSing)
    {
        // Get a ptr to the result's data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

#define lpAPA       ((LPAPLAPA) lpMemRes)
        // Get the first (and only) value from the right arg
        GetFirstValueToken (lptkRhtArg, // Ptr to right arg token
                           &lpAPA->Off, // Ptr to integer result
                            NULL,       // Ptr to float ...
                            NULL,       // Ptr to WCHAR ...
                            NULL,       // Ptr to longest ...
                            NULL,       // Ptr to lpSym/Glb ...
                            NULL,       // Ptr to ...immediate type ...
                            NULL);      // Ptr to array type ...
        lpAPA->Mul = 0;

        // Save the All2s state
        lpMemHdrRes->All2s = (lpAPA->Off EQ 2);
#undef  lpAPA
    } else
    //***************************************************************
    // If the result is empty, but we need to generate
    //   a prototype, do so now
    //***************************************************************
    if (bPrototype)
    {
        // Make the prototype
        hSymGlbProto =
          MakeMonPrototype_EM_PTB (hSymGlbProto,    // Proto arg handle
                                   ARRAY_NESTED,    // Array storage type
                                   lptkFunc,        // Ptr to function token
                                   FALSE);          // Allow CHARs
        if (!hSymGlbProto)
            bRet = FALSE;
        else
        {
            // Get a ptr to the result's data
            lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

            // Save the handle
            *((LPAPLNESTED) lpMemRes) = hSymGlbProto;
        } // End IF
    } else
    if (!IsEmpty (aplNELMRes))
        bRet =
          PrimFnDydRhoRhtCopyData (lptkRhtArg,      // Ptr to right arg token
                                   aplTypeRes,      // Result storage type
                                   aplNELMRes,      // Result NELM
                                   aplRankRes,      // Result rank
                                   lpMemHdrRes,     // Ptr to result memory
                                   lptkFunc);       // Ptr to function token
    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

    // Check the result
    if (bRet)
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        // See if it fits into a lower (but not necessarily smaller) datatype
        TypeDemote (&lpYYRes->tkToken, FALSE);

        return lpYYRes;
    } // End IF

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

    return NULL;
} // End PrimFnDydRho_EM_YY


//***************************************************************************
//  $PrimFnDydRhoRhtCopyData
//
//  Copy data to the result
//***************************************************************************

UBOOL PrimFnDydRhoRhtCopyData
    (LPTOKEN  lptkRhtArg,           // Ptr to right arg token
     APLSTYPE aplTypeRes,           // Result storage type
     APLNELM  aplNELMRes,           // Result NELM
     APLRANK  aplRankRes,           // Result rank
     LPVOID   lpMemHdrRes,          // Ptr to result header
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    UBOOL         bRet = TRUE;      // TRUE iff result is valid
    UINT          uRes;             // Loop counter
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;     // Ptr to Ctrl-Break flag
    LPVOID        lpMemRes;         // Ptr to result memory

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

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
                Assert (IsGlbTypeVarDir_PTB (lptkRhtArg->tkData.tkSym->stData.stGlbData));

                bRet = PrimFnDydRhoRhtGlbCopyData_EM (lptkRhtArg->tkData.tkSym->stData.stGlbData,
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

                    Assert (IsSimpleBool (aplTypeRes));

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
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *((LPAPLBOOL) lpMemRes)++ = aplBool;
                    } // End FOR

                    // # valid bits in last byte
                    uBits = MASKLOG2NBIB & (UINT) aplNELMRes;
                    if (uBits)
                        *((LPAPLBOOL) lpMemRes) = aplBool & ((BIT0 << uBits) - 1);
                    break;
                } // End IMMTYPE_BOOL

                case IMMTYPE_INT:
                {
                    APLINT aplInt;

                    Assert (aplTypeRes EQ ARRAY_INT);

                    // Copy the single value to avoid recalling it everytime
                    aplInt = lptkRhtArg->tkData.tkSym->stData.stInteger;

                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *((LPAPLINT) lpMemRes)++ = aplInt;
                    } // End FOR

                    break;
                } // End IMMTYPE_INT

                case IMMTYPE_FLOAT:
                {
                    APLFLOAT aplFloat;

                    Assert (IsRealFlt (aplTypeRes));

                    // Copy the single value to avoid recalling it everytime
                    aplFloat = lptkRhtArg->tkData.tkSym->stData.stFloat;

                    for (uRes = 0; bRet && uRes < aplNELMRes; uRes++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *((LPAPLFLOAT) lpMemRes)++ = aplFloat;
                    } // End FOR

                    break;
                } // End IMMTYPE_FLOAT

                case IMMTYPE_CHAR:
                {
                    APLCHAR aplChar;

                    Assert (IsSimpleChar (aplTypeRes));

                    // Copy the single value to avoid recalling it everytime
                    aplChar = lptkRhtArg->tkData.tkSym->stData.stChar;

                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *((LPAPLCHAR) lpMemRes)++ = aplChar;
                    } // End FOR

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

                    Assert (IsSimpleBool (aplTypeRes));

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
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *((LPAPLBOOL) lpMemRes)++ = aplBool;
                    } // End FOR

                    // # valid bits in last byte
                    uBits = MASKLOG2NBIB & (UINT) aplNELMRes;
                    if (uBits)
                        *((LPAPLBOOL) lpMemRes) = aplBool & ((BIT0 << uBits) - 1);
                    break;
                } // End IMMTYPE_BOOL

                case IMMTYPE_INT:
                {
                    APLINT aplInt;

                    Assert (aplTypeRes EQ ARRAY_INT);

                    // Copy the single value to avoid recalling it everytime
                    aplInt = lptkRhtArg->tkData.tkInteger;

                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *((LPAPLINT) lpMemRes)++ = aplInt;
                    } // End FOR

                    break;
                } // End IMMTYPE_INT

                case IMMTYPE_FLOAT:
                {
                    APLFLOAT aplFloat;

                    Assert (IsRealFlt (aplTypeRes));

                    // Copy the single value to avoid recalling it everytime
                    aplFloat = lptkRhtArg->tkData.tkFloat;

                    for (uRes = 0; bRet && uRes < aplNELMRes; uRes++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *((LPAPLFLOAT) lpMemRes)++ = aplFloat;
                    } // End FOR

                    break;
                } // End IMMTYPE_FLOAT

                case IMMTYPE_CHAR:
                {
                    APLCHAR aplChar;

                    Assert (IsSimpleChar (aplTypeRes));

                    // Copy the single value to avoid recalling it everytime
                    aplChar = lptkRhtArg->tkData.tkChar;

                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *((LPAPLCHAR) lpMemRes)++ = aplChar;
                    } // End FOR

                    break;
                } // End IMMTYPE_CHAR

                defstop
                    break;
            } // End SWITCH

            break;

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (lptkRhtArg->tkData.tkGlbData));

            bRet = PrimFnDydRhoRhtGlbCopyData_EM (lptkRhtArg->tkData.tkGlbData,
                                                  aplTypeRes,
                                                  aplNELMRes,
                                                  lpMemRes,
                                                  lptkFunc);
            break;

        defstop
            break;
    } // End SWITCH

    return bRet;
ERROR_EXIT:
    return FALSE;
} // End PrimFnDydRhoRhtCopyData


//***************************************************************************
//  $PrimFnDydRhoCopyDim
//
//  Copy dimensions to the result of dyadic Rho
//***************************************************************************

void PrimFnDydRhoCopyDim
    (LPVARARRAY_HEADER lpMemHdrRes, // Ptr to result global memory header
     LPTOKEN           lptkLftArg)  // Ptr to left arg token

{
    LPAPLDIM  lpDimRes;     // Ptr to result's dimensions

    // Get ptr to the dimensions in the result
    lpDimRes = VarArrayBaseToDim (lpMemHdrRes);

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
                Assert (IsGlbTypeVarDir_PTB (lptkLftArg->tkData.tkSym->stData.stGlbData));

                // Copy the left argument's values to the result's dimensions
                PrimFnDydRhoLftGlbCopyDim (lptkLftArg->tkData.tkSym->stData.stGlbData,
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
                    *lpDimRes = (APLDIM) FloatToAplint_SCT (lptkLftArg->tkData.tkSym->stData.stFloat, NULL);

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
                    *lpDimRes = (APLDIM) FloatToAplint_SCT (lptkLftArg->tkData.tkFloat, NULL);

                    break;

                case IMMTYPE_CHAR:      // Handled above as DOMAIN ERROR
                defstop
                    break;
            } // End SWITCH

            break;

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (lptkLftArg->tkData.tkGlbData));

            // Copy the left argument's values to the result's dimensions
            PrimFnDydRhoLftGlbCopyDim (lptkLftArg->tkData.tkGlbData,
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

UBOOL PrimFnDydRhoLftValid_EM
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token
     LPAPLRANK lpaplRankRes,        // Ptr to result rank
     LPAPLNELM lpaplNELMRes,        // Ptr to result NELM
     LPTOKEN   lptkFunc)            // Ptr to function token

{
    UBOOL  bRet = TRUE;
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
                Assert (IsGlbTypeVarDir_PTB (lptkLftArg->tkData.tkSym->stData.stGlbData));

                bRet = PrimFnDydRhoLftGlbValid_EM (lptkLftArg->tkData.tkSym->stData.stGlbData,
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
                        goto DOMAIN_EXIT;

                    *lpaplNELMRes = (APLNELM) (lptkLftArg->tkData.tkSym->stData.stInteger);

                    break;

                case IMMTYPE_FLOAT:     // Ensure it's close enough to a non-negative integer
                    // Attempt to convert the float to an integer using System []CT
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
                    goto DOMAIN_EXIT;

                defstop
                    return FALSE;
            } // End SWITCH

            break;              // Exit to common reshape code

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (lptkLftArg->tkData.tkGlbData));

            bRet = PrimFnDydRhoLftGlbValid_EM (lptkLftArg->tkData.tkGlbData,
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
                        goto DOMAIN_EXIT;

                    *lpaplNELMRes = (APLNELM) (lptkLftArg->tkData.tkInteger);

                    break;

                case IMMTYPE_FLOAT:     // Ensure it's close enough to a non-negative integer
                    // Attempt to convert the float to an integer using System []CT
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
                    goto DOMAIN_EXIT;

                defstop
                    return FALSE;
            } // End SWITCH

            break;

        defstop
            return FALSE;
    } // End SWITCH

    return bRet;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    return FALSE;
} // End PrimFnDydRhoLftValid_EM


//***************************************************************************
//  $INTToAplint
//
//  INT conversion routine
//***************************************************************************

APLINT INTToAplint
    (LPAPLINT   lpDataLft,  // Ptr to left arg data
     APLUINT    u,          // Index into left arg
     LPUBOOL    lpbRet)     // Ptr to TRUE iff it succeeded

{
    // Get the next dimension
    return lpDataLft[u];
} // INTToAplint


//***************************************************************************
//  $FLTToAplint
//
//  FLT conversion routine
//***************************************************************************

APLINT FLTToAplint
    (LPAPLFLOAT lpDataLft,      // Ptr to left arg data
     APLUINT    u,              // Index into left arg
     LPUBOOL    lpbRet)         // Ptr to TRUE iff it succeeded

{
    // Attempt to convert the FLT to an integer using System []CT
    return FloatToAplint_SCT (lpDataLft[u], lpbRet);
} // FLTToAplint


//***************************************************************************
//  $RhoLftGlbValidCom
//
//  Common routine used in PrimFnDydRhoLftGlbValid_EM
//***************************************************************************

UBOOL RhoLftGlbValidCom
    (LPVOID        lpDataLft,               // Ptr to left arg data
     APLNELM       aplNELMLft,              // Left arg NELM
     LPAPLNELM     lpaplNELMRes,            // Ptr to result NELM
     HCxyTOAPLINT  HCxyToAplint)            // Conversion to integer routine

{
    APLUINT u;
    APLINT  aplIntTmp;
    UBOOL   bRet = TRUE;

    // Loop through the dimensions
    for (u = 0; bRet && u < aplNELMLft; u++)
    {
        // Get the next dimension
        aplIntTmp = (*HCxyToAplint) (lpDataLft, u, &bRet);

        // Ensure the value fits into a dimension
        if (!bRet
         || 0 > aplIntTmp
         ||     aplIntTmp > MAX_APLDIM)
            goto DOMAIN_EXIT;

        // Multiply the two numbers as APLINTs so we can check for overflow
        aplIntTmp = imul64 (*lpaplNELMRes, aplIntTmp, &bRet, EXCEPTION_RESULT_FLOAT);

        // Check for overflow
        bRet = bRet && (aplIntTmp <= MAX_APLNELM);

        // If overflow, check for zeros
        if (!bRet)
        {
            // Loop through the rest of the dimensions
            for (u++; u < aplNELMLft; u++)
            {
                // Get the next dimension
                aplIntTmp = (*HCxyToAplint) (lpDataLft, u, &bRet);

                if (IsZeroDim (aplIntTmp))
                {
                    // The result NELM is zero
                    *lpaplNELMRes = 0;

                    goto NORMAL_EXIT;
                } // End IF
            } // End FOR

            goto DOMAIN_EXIT;
        } // End IF

        // Save back
        *lpaplNELMRes = (APLNELM) aplIntTmp;
    } // End FOR
NORMAL_EXIT:
    return TRUE;
DOMAIN_EXIT:
    return FALSE;
} // End RhoLftGlbValidCom


//***************************************************************************
//  $PrimFnDydRhoLftGlbValid_EM
//
//  Dyadic Rho left argument validation on a global memory object
//***************************************************************************

UBOOL PrimFnDydRhoLftGlbValid_EM
    (HGLOBAL   hGlbLft,                 // Left arg handle
     LPAPLRANK lpaplRankRes,            // Ptr to result rank
     LPAPLNELM lpaplNELMRes,            // Ptr to result NELM
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    LPVARARRAY_HEADER lpMemHdrLft;      // Ptr to left arg header
    LPVOID            lpMemLft;         // Ptr to left argument global memory
    APLSTYPE          aplTypeLft;
    APLNELM           aplNELMLft;
    APLRANK           aplRankLft;
    UBOOL             bRet = TRUE;
    APLNELM           uNELM;
    UINT              u,
                      uBits;
    APLINT            aplIntTmp;
    jmp_buf           oldHeapFull;      // Save area for previous jmp_buf

    // Save the previous heapFull
    *oldHeapFull = *heapFull;

    // Set the heapFull for a longjmp
    if (setjmp (heapFull) NE 0)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrLft = MyGlobalLockVar (hGlbLft);

#define lpHeader        lpMemHdrLft
    aplTypeLft = lpHeader->ArrType;
    aplNELMLft = lpHeader->NELM;
    aplRankLft = lpHeader->Rank;
#undef  lpHeader

    // The left arg's NELM is the rank of the result
    *lpaplRankRes = aplNELMLft;

    // Initialize with identity element for multiply
    *lpaplNELMRes = 1;

    // Check for LEFT RANK ERROR
    if (IsRank2P (aplRankLft))
        goto RANK_EXIT;
    else
    {
        // Point to the left arg's data
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);

        // Check for LEFT DOMAIN ERROR and fill in *lpaplNELMRes
        // Split cases based upon the left arg's storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
                uNELM = aplNELMLft >> LOG2NBIB;

                // Loop through the dimensions
                for (u = 0; *lpaplNELMRes && u < uNELM; u++) // char at a time, except for the last byte
                    *lpaplNELMRes = ((LPAPLBOOL) lpMemLft)[u] EQ 0xFF;

                // Check the last byte

                // # valid bits in last byte
                uBits = MASKLOG2NBIB & (UINT) aplNELMLft;
                if (uBits)                                  // Last byte not full
                    *lpaplNELMRes &= ((LPAPLBOOL) lpMemLft)[u] EQ ((BIT0 << uBits) - 1);

                break;

            case ARRAY_INT:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       INTToAplint))      // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_FLOAT:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       FLTToAplint))      // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_APA:
            {
                APLINT  apaOff,
                        apaMul,
                        apaVal;
                APLUINT apaLen;

#define lpAPA       ((LPAPLAPA) lpMemLft)
                // Get the APA parameters
                apaOff = lpAPA->Off;
                apaMul = lpAPA->Mul;
                apaLen = aplNELMLft;
#undef  lpAPA
                // Ensure no element in the left arg goes negative
                //   by checking the first and last elements
                bRet = ((apaOff >= 0) && ((apaOff + apaMul * (apaLen - 1)) >= 0));

                // Loop through the dimensions
                for (apaLen = 0; bRet && apaLen < aplNELMLft; apaLen++)
                {
                    // Check for out-of-bounds
                    apaVal = apaOff + apaMul * apaLen;
                    if (0 > apaVal
                     ||     apaVal > MAX_APLDIM)
                        bRet = FALSE;
                    else
                    {
                        // Multiply the two numbers as APLINTs so we can check for overflow
                        aplIntTmp = imul64 (((APLINT) *lpaplNELMRes), apaVal, &bRet, EXCEPTION_RESULT_FLOAT);

                        // Check for overflow
                        if (!bRet
                          || 0 > aplIntTmp
                          ||     aplIntTmp > MAX_APLDIM)
                            goto DOMAIN_EXIT;

                        // Save back
                        *lpaplNELMRes = (APLNELM) aplIntTmp;
                    } // End IF/ELSE
                } // End FOR

                break;
            } // End ARRAY_APA

            case ARRAY_CHAR:
                // OK if empty
                if (IsEmpty (aplNELMLft))
                    break;

            case ARRAY_NESTED:          // We could check for promotion to simple numeric
            case ARRAY_HETERO:          // ...
                bRet = FALSE;

                break;

            case ARRAY_RAT:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       RATToAplint))      // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_VFP:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       VFPToAplint))      // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_HC2I:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       HC2IToAplint_SCT))     // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_HC4I:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       HC4IToAplint_SCT))     // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_HC8I:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       HC8IToAplint_SCT))     // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_HC2F:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       HC2FToAplint_SCT))     // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_HC4F:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       HC4FToAplint_SCT))     // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_HC8F:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       HC8FToAplint_SCT))     // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_HC2R:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       HC2RToAplint_SCT))     // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_HC4R:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       HC4RToAplint_SCT))     // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_HC8R:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       HC8RToAplint_SCT))     // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_HC2V:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       HC2VToAplint_SCT))     // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_HC4V:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       HC4VToAplint_SCT))     // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_HC8V:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       HC8VToAplint_SCT))     // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_ARB:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       ARBToAplint))      // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_BA2F:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       BA2FToAplint_SCT))     // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_BA4F:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       BA4FToAplint_SCT))     // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            case ARRAY_BA8F:
                // Handle this loop/test in a common routine
                if (RhoLftGlbValidCom (lpMemLft,                // Ptr to left arg data
                                       aplNELMLft,              // Left arg NELM
                                       lpaplNELMRes,            // Ptr to result NELM
                                       BA8FToAplint_SCT))     // Routine to convert to integer
                    goto NORMAL_EXIT;
                else
                    goto DOMAIN_EXIT;

                break;

            defstop
                break;
        } // End SWITCH

        // If error, it's a DOMAIN ERROR
        if (!bRet)
            goto DOMAIN_EXIT;
    } // End IF/ELSE

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
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
    // Mark as unsuccessful
    bRet = FALSE;
NORMAL_EXIT:
    // Restore previous heapFull
    *heapFull = *oldHeapFull;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;

    return bRet;
} // End PrimFnDydRhoLftGlbValid_EM


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
    LPVARARRAY_HEADER lpMemHdrLft;  // Ptr to left arg header
    LPVOID            lpMemLft;     // Ptr to left arg memory
    APLNELM           aplNELMLft;   // Left arg NELM
    UINT              uLft,         // Loop counter
                      uBitMaskLft;  // Left arg bit mask
    UBOOL             bRet;         // TRUE iff the result is valid

    // Lock the memory to get a ptr to it
    lpMemHdrLft = MyGlobalLockVar (hGlbLft);

#define lpHeader        lpMemHdrLft

    // Get the left arg's NELM to use as loop limit
    aplNELMLft = lpHeader->NELM;

    // Point to the left arg's data
    lpMemLft = VarArrayDataFmBase (lpMemHdrLft);

    // Split cases based upon the left arg's array type
    switch (lpHeader->ArrType)
#undef  lpHeader
    {
        case ARRAY_BOOL:
            uBitMaskLft = BIT0;
            for (uLft = 0; uLft < aplNELMLft; uLft++)
            {
                *lpaplDim++ = (uBitMaskLft & *(LPAPLBOOL) lpMemLft) ? TRUE : FALSE;

                // Shift over the left arg's bit mask
                uBitMaskLft <<= 1;

                // Check for end-of-byte
                if (uBitMaskLft EQ END_OF_BYTE)
                {
                    uBitMaskLft = BIT0;         // Start over
                    ((LPAPLBOOL) lpMemLft)++;  // Skip to next byte
                } // End IF
            } // End FOR

            break;

        case ARRAY_INT:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) *((LPAPLINT) lpMemLft)++;
            break;

        case ARRAY_FLOAT:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) FloatToAplint_SCT (*((LPAPLFLOAT) lpMemLft)++, NULL);
            break;

        case ARRAY_APA:
        {
            APLINT  apaOff,
                    apaMul;
            APLUINT apaLen;

#define lpAPA       ((LPAPLAPA) lpMemLft)
            // Get the APA parameters
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
            break;

        case ARRAY_RAT:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) mpq_get_sctsx (((LPAPLRAT) lpMemLft)++, &bRet);
            break;

        case ARRAY_VFP:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) mpfr_get_sctsx (((LPAPLVFP) lpMemLft)++, &bRet);
            break;

        case ARRAY_HC2I:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) ((LPAPLHC2I) lpMemLft)++->parts[0];
            break;

        case ARRAY_HC4I:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) ((LPAPLHC4I) lpMemLft)++->parts[0];
            break;

        case ARRAY_HC8I:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) ((LPAPLHC8I) lpMemLft)++->parts[0];
            break;

        case ARRAY_HC2F:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) FloatToAplint_SCT (((LPAPLHC2F) lpMemLft)++->parts[0], NULL);
            break;

        case ARRAY_HC4F:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) FloatToAplint_SCT (((LPAPLHC4F) lpMemLft)++->parts[0], NULL);
            break;

        case ARRAY_HC8F:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) FloatToAplint_SCT (((LPAPLHC8F) lpMemLft)++->parts[0], NULL);
            break;

        case ARRAY_HC2R:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) mpq_get_sctsx (&((LPAPLHC2R) lpMemLft)++->parts[0], &bRet);
            break;

        case ARRAY_HC4R:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) mpq_get_sctsx (&((LPAPLHC4R) lpMemLft)++->parts[0], &bRet);
            break;

        case ARRAY_HC8R:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) mpq_get_sctsx (&((LPAPLHC8R) lpMemLft)++->parts[0], &bRet);
            break;

        case ARRAY_HC2V:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) mpfr_get_sctsx (&((LPAPLHC2V) lpMemLft)++->parts[0], &bRet);
            break;

        case ARRAY_HC4V:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) mpfr_get_sctsx (&((LPAPLHC4V) lpMemLft)++->parts[0], &bRet);
            break;

        case ARRAY_HC8V:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) mpfr_get_sctsx (&((LPAPLHC8V) lpMemLft)++->parts[0], &bRet);
            break;

        case ARRAY_ARB:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) arb_get_sctsx (((LPAPLARB) lpMemLft)++, &bRet);
            break;

        case ARRAY_BA2F:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) arb_get_sctsx (&((LPAPLBA2F) lpMemLft)++->parts[0], &bRet);
            break;

        case ARRAY_BA4F:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) arb_get_sctsx (&((LPAPLBA4F) lpMemLft)++->parts[0], &bRet);
            break;

        case ARRAY_BA8F:
            for (uLft = 0; uLft < aplNELMLft; uLft++)
                *lpaplDim++ = (APLDIM) arb_get_sctsx (&((LPAPLBA8F) lpMemLft)++->parts[0], &bRet);
            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
} // End PrimFnDydRhoLftGlbCopyDim


//***************************************************************************
//  $PrimFnDydRhoRhtGlbCopyData_EM
//
//  Dyadic rho right argument copy data to the result
//***************************************************************************

UBOOL PrimFnDydRhoRhtGlbCopyData_EM
    (HGLOBAL   hGlbRht,
     APLSTYPE  aplTypeRes,
     APLNELM   aplNELMRes,
     LPVOID    lpDataRes,
     LPTOKEN   lptkFunc)

{
    LPVARARRAY_HEADER lpMemHdrRht = NULL;   // Ptr to right arg base header
    LPVOID            lpMemRhtData,         // Ptr to right arg memory
                      lpMemRhtNext;         // ...              next memory
    APLINT            uRes,                 // Loop counter
                      uRht;                 // ...
    UINT              uBitMaskRes,          // Bit mask for the result
                      uBitMaskRht,          // ...              right arg
                      i;                    // Loop counter
    UBOOL             bRet = TRUE;          // TRUE iff the reault is valid
    APLINT            aplNELMRht;           // Right arg NELM
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Lock the memory to get a ptr to it
    lpMemHdrRht  = MyGlobalLockVar (hGlbRht);

#define lpHeader        lpMemHdrRht

    aplNELMRht   = lpHeader->NELM;
    lpMemRhtNext =
    lpMemRhtData = VarArrayDataFmBase (lpMemHdrRht);

    // Check for empty right arg
    if (IsEmpty (aplNELMRht))
    {
        // Split cases based upon the storage type
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_APA:
            case ARRAY_HC2I:
            case ARRAY_HC4I:
            case ARRAY_HC8I:
            case ARRAY_HC2F:
            case ARRAY_HC4F:
            case ARRAY_HC8F:
////////////////// Fill the result with zeros (already done by GND)
////////////////ZeroMemory (lpDataRes, (APLU3264) RoundUpBitsToBytes (aplNELMRes));

                break;

            case ARRAY_CHAR:
                FillMemoryW (lpDataRes, (APLU3264) aplNELMRes, L' ');

                break;

            case ARRAY_NESTED:
                break;

            case ARRAY_RAT:
                // Fill the result with RAT zeros
                for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    mpq_init (((LPAPLRAT) lpDataRes)++);
                break;

            case ARRAY_VFP:
                // Fill the result with VFP zeros
                for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    // Initialize to 0
                    mpfr_init0 (((LPAPLVFP) lpDataRes)++);
                break;

            case ARRAY_HC2R:
                // Fill the result with HC2R zeros
                for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    // Initialize to 0/1
                    mphc2r_init (((LPAPLHC2R) lpDataRes)++);
                break;

            case ARRAY_HC4R:
                // Fill the result with HC4R zeros
                for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    // Initialize to 0/1
                    mphc4r_init (((LPAPLHC4R) lpDataRes)++);
                break;

            case ARRAY_HC8R:
                // Fill the result with HC8R zeros
                for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    // Initialize to 0/1
                    mphc8r_init (((LPAPLHC8R) lpDataRes)++);
                break;

            case ARRAY_HC2V:
                // Fill the result with HC2V zeros
                for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    // Initialize to 0
                    mphc2v_init0 (((LPAPLHC2V) lpDataRes)++);
                break;

            case ARRAY_HC4V:
                // Fill the result with HC4V zeros
                for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    // Initialize to 0
                    mphc4v_init0 (((LPAPLHC4V) lpDataRes)++);
                break;

            case ARRAY_HC8V:
                // Fill the result with HC8V zeros
                for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    // Initialize to 0
                    mphc8v_init0 (((LPAPLHC8V) lpDataRes)++);
                break;

            case ARRAY_ARB:
                // Fill the result with ARB zeros
                for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    // Initialize to 0/0
                    Myarb_init (((LPAPLARB) lpDataRes)++);
                break;

            case ARRAY_BA2F:
                // Fill the result with BA2F zeros
                for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    // Initialize to 0/0
                    arb2f_init (((LPAPLBA2F) lpDataRes)++);
                break;

            case ARRAY_BA4F:
                // Fill the result with BA4F zeros
                for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    // Initialize to 0/0
                    arb4f_init (((LPAPLBA4F) lpDataRes)++);
                break;

            case ARRAY_BA8F:
                // Fill the result with BA8F zeros
                for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    // Initialize to 0/0
                    arb8f_init (((LPAPLBA8F) lpDataRes)++);
                break;

            case ARRAY_HETERO:
            defstop
                break;
        } // End SWITCH
    } // End IF
    else
    // Split cases based upon the right arg's array type
    switch (lpHeader->ArrType)
    {
        case ARRAY_BOOL:
            // This should be optimized to take advantage of alignment
            //   and 32-bits at a time, maybe even the 386 instructions
            //   SHLD/SHRD.

            uBitMaskRes = uBitMaskRht = BIT0;

            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start over again
                //   the right arg's counter and ptr
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                    uBitMaskRht = BIT0;
                } else
                // Check to see if we should start over again
                //   the right arg's bit mask
                if (uBitMaskRht EQ END_OF_BYTE)
                {
                    uBitMaskRht = BIT0;
                    ((LPAPLBOOL) lpMemRhtNext)++;
                } // End IF

                // Check to see if we should start over again
                //   the result's bit mask
                if (uBitMaskRes EQ END_OF_BYTE)
                {
                    uBitMaskRes = BIT0;
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
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
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
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
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
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                *((LPAPLFLOAT) lpDataRes)++ = *((LPAPLFLOAT) lpMemRhtNext)++;
            } // End FOR

            break;

        case ARRAY_NESTED:
            // Take into account nested prototypes
            aplNELMRes = max (aplNELMRes, 1);
        case ARRAY_HETERO:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; bRet && uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                // Split cases based upon the array storage type
                switch (GetPtrTypeInd (lpMemRhtNext))
                {
                    case PTRTYPE_STCONST:
                        // It's an immediate
                        Assert ((*(LPAPLHETERO) lpMemRhtNext)->stFlags.Imm);

                        // Pass on the LPSYMENTRY
                        *((LPAPLHETERO) lpDataRes)++ = *(LPAPLHETERO) lpMemRhtNext;

                        break;

                    case PTRTYPE_HGLOBAL:
                        Assert (IsPtrArray (lpHeader->ArrType));

                        // Data is an valid HGLOBAL array
                        Assert (IsGlbTypeVarInd_PTB (lpMemRhtNext));

                        // Make a copy of the HGLOBAL ptr
                        *((LPAPLNESTED) lpDataRes)++ = CopySymGlbInd_PTB (lpMemRhtNext);

                        break;

                    defstop
                        break;
                } // End SWITCH

                // If the right arg is not empty, ...
                if (!IsEmpty (aplNELMRht))
                    // Skip to next element in right arg
                    ((LPAPLNESTED) lpMemRhtNext)++;
            } // End FOR

            break;

        case ARRAY_APA:
        {
            APLINT  apaOff,
                    apaMul;
            APLUINT apaLen;

#define lpAPA       ((LPAPLAPA) lpMemRhtNext)
            // Get the APA parameters
            apaOff = lpAPA->Off;
            apaMul = lpAPA->Mul;
            apaLen = aplNELMRht;
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
#undef  lpAPA
            } else
            {
                // Loop through the result and right arg copying the data
                for (apaLen = uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, apaLen++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Check to see if we should start the right arg's
                    //   counter over again
                    if (apaLen EQ lpHeader->NELM)
                        apaLen = 0;

                    *((LPAPLINT) lpDataRes)++ = apaOff + apaMul * apaLen;
                } // End FOR
            } // End IF/ELSE

            break;
        } // End ARRAY_APA

        case ARRAY_RAT:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                mpq_init_set (((LPAPLRAT) lpDataRes)++, ((LPAPLRAT) lpMemRhtNext)++);
            } // End FOR

            break;

        case ARRAY_VFP:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                mpfr_init_copy (((LPAPLVFP) lpDataRes)++, ((LPAPLVFP) lpMemRhtNext)++);
            } // End FOR

            break;

        case ARRAY_HC2I:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                *((LPAPLHC2I) lpDataRes)++ = *((LPAPLHC2I) lpMemRhtNext)++;
            } // End FOR

            break;

        case ARRAY_HC2F:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                *((LPAPLHC2F) lpDataRes)++ = *((LPAPLHC2F) lpMemRhtNext)++;
            } // End FOR

            break;

        case ARRAY_HC2R:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                // Loop through all of the parts
                for (i = 0; i < 2; i++)
                    mpq_init_set (&((LPAPLHC2R) lpDataRes)[uRes].parts[i], &((LPAPLHC2R) lpMemRhtNext)[uRht].parts[i]);
            } // End FOR

            break;

        case ARRAY_HC2V:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                // Loop through all of the parts
                for (i = 0; i < 2; i++)
                    mpfr_init_copy (&((LPAPLHC2V) lpDataRes)[uRes].parts[i], &((LPAPLHC2V) lpMemRhtNext)[uRht].parts[i]);
            } // End FOR

            break;

        case ARRAY_HC4I:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                *((LPAPLHC4I) lpDataRes)++ = *((LPAPLHC4I) lpMemRhtNext)++;
            } // End FOR

            break;

        case ARRAY_HC4F:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                *((LPAPLHC4F) lpDataRes)++ = *((LPAPLHC4F) lpMemRhtNext)++;
            } // End FOR

            break;

        case ARRAY_HC4R:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                // Loop through all of the parts
                for (i = 0; i < 4; i++)
                    mpq_init_set (&((LPAPLHC4R) lpDataRes)[uRes].parts[i], &((LPAPLHC4R) lpMemRhtNext)[uRht].parts[i]);
            } // End FOR

            break;

        case ARRAY_HC4V:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                // Loop through all of the parts
                for (i = 0; i < 4; i++)
                    mpfr_init_copy (&((LPAPLHC4V) lpDataRes)[uRes].parts[i], &((LPAPLHC4V) lpMemRhtNext)[uRht].parts[i]);
            } // End FOR

            break;

        case ARRAY_HC8I:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                *((LPAPLHC8I) lpDataRes)++ = *((LPAPLHC8I) lpMemRhtNext)++;
            } // End FOR

            break;

        case ARRAY_HC8F:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                *((LPAPLHC8F) lpDataRes)++ = *((LPAPLHC8F) lpMemRhtNext)++;
            } // End FOR

            break;

        case ARRAY_HC8R:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                // Loop through all of the parts
                for (i = 0; i < 8; i++)
                    mpq_init_set (&((LPAPLHC8R) lpDataRes)[uRes].parts[i], &((LPAPLHC8R) lpMemRhtNext)[uRht].parts[i]);
            } // End FOR

            break;

        case ARRAY_HC8V:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                // Loop through all of the parts
                for (i = 0; i < 8; i++)
                    mpfr_init_copy (&((LPAPLHC8V) lpDataRes)[uRes].parts[i], &((LPAPLHC8V) lpMemRhtNext)[uRht].parts[i]);
            } // End FOR

            break;

        case ARRAY_ARB:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                arb_init_set (((LPAPLARB) lpDataRes)++, ((LPAPLARB) lpMemRhtNext)++);
            } // End FOR

            break;

        case ARRAY_BA2F:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                arb2f_init_set (&((LPAPLBA2F) lpDataRes)[uRes], &((LPAPLBA2F) lpMemRhtNext)[uRht]);
            } // End FOR

            break;

        case ARRAY_BA4F:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                arb4f_init_set (&((LPAPLBA4F) lpDataRes)[uRes], &((LPAPLBA4F) lpMemRhtNext)[uRht]);
            } // End FOR

            break;

        case ARRAY_BA8F:
            // Loop through the result and right arg copying the data
            for (uRes = uRht = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++, uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Check to see if we should start the right arg's
                //   counter and ptr over again
                if (uRht EQ (APLNELMSIGN) lpHeader->NELM)
                {
                    uRht = 0;
                    lpMemRhtNext = lpMemRhtData;
                } // End IF

                arb8f_init_set (&((LPAPLBA8F) lpDataRes)[uRes], &((LPAPLBA8F) lpMemRhtNext)[uRht]);
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH
#undef  lpHeader

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

    return bRet;
ERROR_EXIT:
    return FALSE;
} // End PrimFnDydRhoRhtGlbCopyData_EM


//***************************************************************************
//  End of File: pf_rho.c
//***************************************************************************
