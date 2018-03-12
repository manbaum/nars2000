//***************************************************************************
//  NARS2000 -- Primitive Function -- RightShoe
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2018 Sudley Place Software

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
//  $PrimFnRightShoe_EM_YY
//
//  Primitive function for monadic and dyadic RightShoe ("disclose" and "pick")
//***************************************************************************

LPPL_YYSTYPE PrimFnRightShoe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_RIGHTSHOE);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonRightShoe_EM_YY             (lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydRightShoe_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnRightShoe_EM


//***************************************************************************
//  $PrimProtoFnRightShoe_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic RightShoe
//***************************************************************************

LPPL_YYSTYPE PrimProtoFnRightShoe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnRightShoe_EM_YY,   // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnRightShoe_EM_YY


//***************************************************************************
//  $PrimIdentFnRightShoe_EM_YY
//
//  Generate an identity element for the primitive function dyadic RightShoe
//***************************************************************************

LPPL_YYSTYPE PrimIdentFnRightShoe_EM_YY
    (LPTOKEN lptkRhtOrig,           // Ptr to original right arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to result

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

    // The (left) identity element for dyadic RightShoe
    //   (L {rightshoe} R) ("pick") is
    //   {zilde}.

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbZilde);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimIdentFnRightShoe_EM_YY


//***************************************************************************
//  $PrimFnMonRightShoe_EM_YY
//
//  Primitive function for monadic RightShoe ("disclose")
//***************************************************************************

LPPL_YYSTYPE PrimFnMonRightShoe_EM_YY
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
                Assert (IsGlbTypeVarDir_PTB (lptkRhtArg->tkData.tkSym->stData.stGlbData));

                return PrimFnMonRightShoeGlb_EM_YY
                       (lptkRhtArg->tkData.tkSym->stData.stGlbData, // HGLOBAL
                        lptkAxis,                                   // Ptr to axis token (may be NULL)
                        lptkFunc);                                  // Ptr to function token
            } // End IF

            // Handle the immediate case
            return PrimFnMonRightShoeCon_EM_YY
                   (lptkRhtArg->tkData.tkSym->stFlags.ImmType,  // Immediate type
                    lptkRhtArg->tkData.tkSym->stData.stLongest, // Immediate value
                    lptkAxis,                                   // Ptr to axis token (may be NULL)
                    lptkFunc);                                  // Ptr to function token
        case TKT_VARIMMED:
            return PrimFnMonRightShoeCon_EM_YY
                   (lptkRhtArg->tkFlags.ImmType,                // Immediate type
                    lptkRhtArg->tkData.tkLongest,               // Immediate value
                    lptkAxis,                                   // Ptr to axis token (may be NULL)
                    lptkFunc);                                  // Ptr to function token
        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (lptkRhtArg->tkData.tkGlbData));

            return PrimFnMonRightShoeGlb_EM_YY
                   (lptkRhtArg->tkData.tkGlbData,               // HGLOBAL
                    lptkAxis,                                   // Ptr to axis token (may be NULL)
                    lptkFunc);                                  // Ptr to function token
        defstop
            return NULL;
    } // End SWITCH
} // End PrimFnMonRightShoe_EM_YY


//***************************************************************************
//  $PrimFnMonRightShoeCon_EM_YY
//
//  Monadic RightShoe ("disclose") on an immediate value.
//***************************************************************************

LPPL_YYSTYPE PrimFnMonRightShoeCon_EM_YY
    (IMM_TYPES  immType,            // The immediate type (see IMM_TYPES)
     APLLONGEST aplLongest,         // The immediate value
     LPTOKEN    lptkAxis,           // Ptr to axis token
     LPTOKEN    lptkFunc)           // Ptr to function token

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Disclose with axis on a simple scalar requires
        //   that the axis be an empty vector
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           0,               // All values less than this
                           FALSE,           // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // TRUE iff fractional values allowed
                           NULL,            // Return last axis value
                           NULL,            // Return # elements in axis vector
                           NULL))           // Return HGLOBAL with APLINT axis values
            return NULL;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = immType;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkLongest  = aplLongest;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End PrimFnMonRightShoeCon_EM_YY


//***************************************************************************
//  $PrimFnMonRightShoeGlb_EM_YY
//
//  Monadic RightShoe ("disclose") on a global memory object
//***************************************************************************

LPPL_YYSTYPE PrimFnMonRightShoeGlb_EM_YY
    (HGLOBAL hGlbRht,                       // Right arg global memory handle
     LPTOKEN lptkAxis,                      // Ptr to axis token (may be NULL)
     LPTOKEN lptkFunc)                      // Ptr to function token

{
    HGLOBAL           hGlbRes = NULL,       // Result global memory handle
                      hGlbAxis = NULL,      // Axis global memory handle
                      hGlbSub = NULL,       // Right arg item global memory handle
                      hSymGlbProto,         // Right arg item prototype as global memory handle
                      hGlbDimCom = NULL,    // Right arg common item dimension global memory handle
                      hGlbLft;              // Left arg global memory handle
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg header
                      lpMemHdrRht = NULL,   // ...    right ...
                      lpMemHdrRes = NULL,   // ...    result   ...
                      lpMemHdrSub = NULL;   // ...    subitem  ...
    LPVOID            lpMemRes,             // Ptr to result global memory
                      lpMemRht,             // Ptr to right arg global memory
                      lpMemSub;             // Ptr to right arg item global memory
    LPAPLDIM          lpMemDimRht = NULL,   // Ptr to right arg dimensions
                      lpMemDimCom = NULL,   // Ptr to right arg common item dimensions
                      lpMemDimSub,          // ...              item dimensions
                      lpMemDimRes;          // Ptr to result dimensions
    LPAPLUINT         lpMemAxisHead = NULL, // Ptr to axis values, fleshed out by CheckAxis_EM
                      lpMemLft;             // Ptr to left arg global memory
    APLUINT           ByteRes;              // # bytes in the result
    APLNELM           aplNELMAxis,          // Axis NELM
                      aplNELMRes,           // Result NELM
                      aplNELMRht,           // Right arg NELM
                      aplNELMNstRht,        // Right arg NELM if empty nested
                      aplNELMCom,           // Right arg common item NELM
                      aplNELMComRest,       // Right arg common item NELM except for last dimension
                      aplNELMComLast,       // Right arg common item NELM last dimension
                      aplNELMSub;           // Right arg item NELM
    APLRANK           aplRankRht,           // Right arg rank
                      aplRankRes,           // Result rank
                      aplRankCom,           // Right arg common item rank
                      aplRankSub;           // Right arg item rank
    UBOOL             bRet = TRUE,          // TRUE iff result is valid
                      bScalarItem = FALSE;  // TRUE iff the right arg contains a scalar item
    UINT              uBitMask;             // Bit mask for looping through Booleans
    APLNELM           uRht,                 // Loop counter
                      uCom,                 // ...
                      uDim,                 // ...
                      uSub;                 // ...
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeSub,           // Right arg item storage type
                      aplType1SS,           // First simple scalar storage type
                      aplTypeRes;           // Result    ...
    LPPL_YYSTYPE      lpYYRes;              // Ptr to the result
    LPSYMENTRY        lpSymProto,           // Right arg item prototype as Symbol Table Entry
                      lpSym0,               // LPSYMENTRY for constant zero
                      lpSym1,               // ...                     one
                      lpSymB,               // ...                     ' '
                      lpSymTmp;             // Ptr to temporary LPSYMENTRY
    APLUINT           uSubRest,             // Loop counter
                      uSubLast,             // ...
                      aplIndex;             // Index for Booleans
    APLNELM           aplNELMSubRest,       // Right arg item NELM except for last coordinate
                      aplNELMSubLast;       // Right arg item NELM last coordinate
    APLINT            apaOffSub,            // Right arg item APA offset
                      apaMulSub;            // Right arg item APA multiplier
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLINT            aplInteger;           // Immediate value as Boolean/Integer
    APLFLOAT          aplFloat;             // ...                Float
    ALLTYPES          atSub = {0};          // Sub item as ALLTYPES
    int               i;                    // Loop counter

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get STEs for constants
    lpSym0 = GetSteZero  ();
    lpSym1 = GetSteOne   ();
    lpSymB = GetSteBlank ();

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg global
    AttrsOfGlb (hGlbRht, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Take into account nested prototypes
    if (IsNested (aplTypeRht))
        aplNELMNstRht = max (aplNELMRht, 1);
    else
        aplNELMNstRht = aplNELMRht;

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLockVar (hGlbRht);

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // Initialize the common item rank
    aplRankCom = 0;

    // Trundle through the right arg ensuring that, except for scalars,
    //   all of the items have the same rank
    if (!IsSimpleGlbNum (aplTypeRht))
    for (uRht = 0; uRht < aplNELMNstRht; uRht++)
    {
        // Split cases based upon the ptr type of the item
        switch (GetPtrTypeDir (((LPAPLNESTED) lpMemRht)[uRht]))
        {
            case PTRTYPE_STCONST:   // Ignore (simple) scalar items
                break;

            case PTRTYPE_HGLOBAL:
                // Get the attributes (Type, NELM, and Rank)
                //   of the item
                AttrsOfGlb (((LPAPLNESTED) lpMemRht)[uRht], NULL, NULL, &aplRankSub, NULL);

                // Calculate the higher rank
                aplRankCom = max (aplRankCom, aplRankSub);

                break;

            defstop
                break;
        } // End SWITCH
    } // End IF/FOR

    // The result rank is the sum of the common item rank and the right arg rank
    aplRankRes = aplRankCom + aplRankRht;

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Check the axis values, fill in # elements in axis
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRes,      // All values less than this
                           FALSE,           // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // TRUE iff fractional values allowed
                           NULL,            // Return last axis value
                          &aplNELMAxis,     // Return # elements in axis vector
                          &hGlbAxis))       // Return HGLOBAL with APLINT axis values
            goto ERROR_EXIT;
    } else
        // No axis means disclose all dimensions
        aplNELMAxis = aplRankCom;

    // If the common item rank isn't the same as the # elements in the axis, ..
    if (aplRankCom NE aplNELMAxis)
        goto AXIS_EXIT;

    //***************************************************************
    // hGlbAxis is not-NULL iff there is an axis
    // aplNELMAxis contains the # elements in the axis.
    //***************************************************************

    // If the right arg is simple or global numeric,
    //   the result is the same as the right arg
    if (IsSimpleGlbNum (aplTypeRht))
    {
        // Copy the right arg
        hGlbRes = CopySymGlbDirAsGlb (hGlbRht);

        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // From here on, the right arg is ARRAY_NESTED.
    //***************************************************************

    Assert (IsNested (aplTypeRht));

    // Calculate space needed for the dimensions
    ByteRes = sizeof (APLUINT) * aplRankCom;

    // Ensure we don't attempt to allocate zero bytes as
    //   W chokes when we try to lock the memory from
    //   that simple request.
    ByteRes = max (1, ByteRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the max shape of the items
    hGlbDimCom = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbDimCom EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemDimCom = MyGlobalLockInt (hGlbDimCom); // Might be only 1 byte

    // Initialize the result storage type
    //   and type of first simple scalar
    aplTypeRes = ARRAY_INIT;
    aplType1SS = ARRAY_ERROR;

    // Trundle through the right arg calculating the shape and type
    //   of the common item
    for (aplNELMRes = 0, uRht = 0; uRht < aplNELMNstRht; uRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // Split cases based upon the ptr type of the item
        switch (GetPtrTypeDir (((LPAPLNESTED) lpMemRht)[uRht]))
        {
            case PTRTYPE_STCONST:   // Ignore (simple) scalar items
                // Mark as a scalar item
                bScalarItem = TRUE;

                // Calculate its storage type
                aplTypeSub = TranslateImmTypeToArrayType (((LPAPLHETERO) lpMemRht)[uRht]->stFlags.ImmType);

                // Keep track of the type of the first simple scalar
                if (IsErrorType (aplType1SS))
                    aplType1SS = aplTypeSub;
                break;

            case PTRTYPE_HGLOBAL:
                // Get the attributes (Type, NELM, and Rank)
                //   of the item
                AttrsOfGlb (((LPAPLNESTED) lpMemRht)[uRht], &aplTypeSub, &aplNELMSub, &aplRankSub, NULL);

                // If the item is not a scalar, ...
                if (!IsScalar (aplRankSub))
                {
                    // Lock the memory to get a ptr to it
                    lpMemHdrSub = MyGlobalLockVar (((LPAPLNESTED) lpMemRht)[uRht]);

                    // Skip over the header to the dimensions
                    lpMemDimSub = VarArrayBaseToDim (lpMemHdrSub);

                    // Loop through the dimensions of the item
                    //   calculating the maximum of the corresponding dimensions
                    for (uDim = aplRankCom - aplRankSub, uSub = 0; uSub < aplRankSub; uDim++, uSub++)
                        lpMemDimCom[uDim] = max (lpMemDimCom[uDim], ((LPAPLDIM) lpMemDimSub)[uSub]);
                    // Keep track of the type of the first simple scalar
                    if (IsErrorType (aplType1SS)
                     && IsSimpleHet (aplTypeSub))
                    {
                        // Skip over the header and dimensions to the data
                        lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                        // Calculate its storage type
                        aplType1SS = TranslateImmTypeToArrayType (((LPAPLHETERO) lpMemSub)[0]->stFlags.ImmType);
                    } // End IF

                    // We no longer need this ptr
                    MyGlobalUnlock (((LPAPLNESTED) lpMemRht)[uRht]); lpMemHdrSub = NULL;
                } else
                    // Mark as a scalar item
                    bScalarItem = TRUE;
                break;

            defstop
                break;
        } // End SWITCH

        // Include this type in the mix
        aplTypeRes = aTypePromote[aplTypeRes][aplTypeSub];
    } // End FOR

    // If the right arg contains a scalar, ...
    if (bScalarItem)
    // Loop through the dimensions of the item
    for (uSub = 0; uSub < aplRankSub; uSub++)
        // Each new dimension in the result is at least one
        //   as the scalar is extended to be of shape all 1s
        lpMemDimCom[uSub] = max (lpMemDimCom[uSub], 1);

    // Trundle through the common item's shape and calculate its NELM
    if (!IsScalar (aplRankCom))
    {
        // Calculate the product of  the dimensions except for the last
        for (aplNELMComRest = 1, uCom = 0; uCom < (aplRankCom - 1); uCom++)
            aplNELMComRest *= lpMemDimCom[uCom];

        // Get the last dimension
        aplNELMComLast = lpMemDimCom[uCom];

        // Calculate the NELM of the common item
        aplNELMCom = aplNELMComRest * aplNELMComLast;
    } else
        aplNELMCom = aplNELMComLast = aplNELMComRest = 1;

    // Calculate the result NELM
    aplNELMRes = aplNELMCom * aplNELMRht;

    // If the result is empty, convert type Hetero to type Boolean or Character
    //   depending upon the type of the first simple scalar
    if (IsEmpty (aplNELMRes)
     && IsSimpleHet (aplTypeRes))
        aplTypeRes = (IsSimpleChar (aplType1SS)) ? ARRAY_CHAR : ARRAY_BOOL;

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

    // Point to the result's dimension
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // Fill in the result's dimension

    // First, copy the dimensions from the right arg
    for (uRht = 0; uRht < aplRankRht; uRht++)
        lpMemDimRes[uRht] = lpMemDimRht[uRht];

    // Next, copy the dimensions from the common item
    for (uCom = 0; uCom < aplRankCom; uCom++)
        lpMemDimRes[uCom + aplRankRht] = lpMemDimCom[uCom];

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Handle prototypes
    if (IsEmpty (aplNELMRes))
    {
        // If the result is Nested, ...
        if (IsNested (aplTypeRes))
        {
            // Get the handle of the right arg item (prototype)
            hGlbSub = ClrPtrTypeInd ((LPAPLNESTED *) lpMemRht);

            // Get the attributes (Type, NELM, and Rank)
            //   of the right arg global
            AttrsOfGlb (hGlbSub, &aplTypeSub, NULL, &aplRankSub, NULL);

            // Split cases based upon the right arg's prototype's storage type
            switch (aplTypeSub)
            {
                case ARRAY_BOOL:
                case ARRAY_INT:
                case ARRAY_FLOAT:
                case ARRAY_APA:
                    *((LPAPLNESTED) lpMemRes) = lpSym0;

                    break;

                case ARRAY_CHAR:
                    *((LPAPLNESTED) lpMemRes) = lpSymB;

                    break;

                case ARRAY_HETERO:
                case ARRAY_NESTED:
                    // Lock the memory to get a ptr to it
                    lpMemHdrSub = MyGlobalLockVar (hGlbSub);

                    // Skip over the header and dimensions to the data
                    lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                    // Save first item as prototype
                    *((LPAPLNESTED) lpMemRes) = CopySymGlbInd_PTB (lpMemSub);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbSub); lpMemHdrSub = NULL;

                    break;

                case ARRAY_RAT:             // Can't happen
                case ARRAY_VFP:             // ...
                case ARRAY_HC2I:            // ...
                case ARRAY_HC2F:            // ...
                case ARRAY_HC2R:            // ...
                case ARRAY_HC2V:            // ...
                case ARRAY_HC4I:            // ...
                case ARRAY_HC4F:            // ...
                case ARRAY_HC4R:            // ...
                case ARRAY_HC4V:            // ...
                case ARRAY_HC8I:            // ...
                case ARRAY_HC8F:            // ...
                case ARRAY_HC8R:            // ...
                case ARRAY_HC8V:            // ...
                defstop
                    break;
            } // End SWITCH
        } // End IF

        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // Copy the data to the result
    //***************************************************************
    if (IsSimpleNH (aplTypeRes) || IsGlbNum (aplTypeRes))
    {
        //***************************************************************
        // At this point, the right arg is nested and non-empty
        //   and the result is simple or global numeric.
        //***************************************************************

        // Loop through the elements of the right arg
        for (uRht = 0; uRht < aplNELMRht; uRht++)
        {
            // Split cases based upon the ptr type of the item
            switch (GetPtrTypeDir (((LPAPLNESTED) lpMemRht)[uRht]))
            {
                case PTRTYPE_STCONST:   // Ignore (simple) scalar items
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Split cases based upon the storage type of the result
                    switch (aplTypeRes)
                    {
                        case ARRAY_BOOL:
                            // The fill element of zero is already present via GHND

                            // Get the index into the Boolean array
                            aplIndex = uRht * aplNELMCom;

                            // Save the Boolean in the first position
                            ((LPAPLBOOL)  lpMemRes)[aplIndex >> LOG2NBIB] |=
                              (BIT0 & ((LPAPLHETERO) lpMemRht)[uRht]->stData.stBoolean) << (MASKLOG2NBIB & (UINT) aplIndex);

                            break;

                        case ARRAY_INT:
                            // The fill element of zero is already present via GHND

                            // Get the value
                            aplInteger = ((LPAPLHETERO) lpMemRht)[uRht]->stData.stInteger;

                            // If the immediate is Boolean, ...
                            if (((LPAPLHETERO) lpMemRht)[uRht]->stFlags.ImmType EQ IMMTYPE_BOOL)
                                aplInteger &= BIT0;

                            // Save the Integer in the first position
                            ((LPAPLINT)   lpMemRes)[uRht * aplNELMCom] =        aplInteger;

                            break;

                        case ARRAY_FLOAT:
                            // The fill element of zero is already present via GHND

                            // If the immediate is Boolean, ...
                            if (((LPAPLHETERO) lpMemRht)[uRht]->stFlags.ImmType EQ IMMTYPE_BOOL)
                                aplFloat = (APLFLOAT) (BIT0 & ((LPAPLHETERO) lpMemRht)[uRht]->stData.stBoolean);
                            else
                            // If the immediate is Integer, ...
                            if (((LPAPLHETERO) lpMemRht)[uRht]->stFlags.ImmType EQ IMMTYPE_INT)
                                aplFloat = (APLFLOAT) ((LPAPLHETERO) lpMemRht)[uRht]->stData.stInteger;
                            else
                                aplFloat =            ((LPAPLHETERO) lpMemRht)[uRht]->stData.stFloat;

                            // Save the Float in the first position
                            ((LPAPLFLOAT) lpMemRes)[uRht * aplNELMCom] =        aplFloat;

                            break;

                        case ARRAY_CHAR:
                            // Fill the row with blanks
                            // Loop through the rest of common item's elements
                            //   saving the item's prototype
                            for (uSubRest = 0; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                                ((LPAPLCHAR) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] = L' ';

                            // Save the Char in the first position
                            ((LPAPLCHAR)  lpMemRes)[uRht * aplNELMCom] =        ((LPAPLHETERO) lpMemRht)[uRht]->stData.stChar;

                            break;

                        case ARRAY_RAT:
                            // Fill the row with zeros
                            // Loop through the rest of common item's elements
                            //   saving the item's prototype
                            for (uSubRest = 0; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                                // Initialize to 0/1
                                mpq_init (&((LPAPLRAT) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)]);

                            // Save the RAT in the first position
                            // Split cases based upon the immediate storage type
                            switch (((LPAPLHETERO) lpMemRht)[uRht]->stFlags.ImmType)
                            {
                                case IMMTYPE_BOOL:
                                case IMMTYPE_INT:
                                    mpq_set_sx (&((LPAPLRAT)   lpMemRes)[uRht * aplNELMCom], ((LPAPLHETERO) lpMemRht)[uRht]->stData.stInteger, 1);

                                    break;

                                case IMMTYPE_FLOAT:         // Can't happen w/PTR_STCONST
                                case IMMTYPE_CHAR:          // ...
                                case IMMTYPE_RAT:           // ...
                                case IMMTYPE_VFP:           // ...
                                case IMMTYPE_HC2I:          // ...
                                case IMMTYPE_HC2F:          // ...
                                case IMMTYPE_HC2R:          // ...
                                case IMMTYPE_HC2V:          // ...
                                case IMMTYPE_HC4I:          // ...
                                case IMMTYPE_HC4F:          // ...
                                case IMMTYPE_HC4R:          // ...
                                case IMMTYPE_HC4V:          // ...
                                case IMMTYPE_HC8I:          // ...
                                case IMMTYPE_HC8F:          // ...
                                case IMMTYPE_HC8R:          // ...
                                case IMMTYPE_HC8V:          // ...
                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case ARRAY_VFP:
                            // Fill the row with zeros
                            // Loop through the rest of common item's elements
                            //   saving the item's prototype
                            for (uSubRest = 0; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                                mpfr_init0 (&((LPAPLVFP) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)]);

                            // Save the VFP in the first position
                            // Split cases based upon the immediate storage type
                            switch (((LPAPLHETERO) lpMemRht)[uRht]->stFlags.ImmType)
                            {
                                case IMMTYPE_BOOL:
                                case IMMTYPE_INT:
                                    mpfr_set_sx (&((LPAPLVFP)   lpMemRes)[uRht * aplNELMCom], ((LPAPLHETERO) lpMemRht)[uRht]->stData.stInteger, MPFR_RNDN);

                                    break;

                                case IMMTYPE_FLOAT:
                                    mpfr_set_d  (&((LPAPLVFP)   lpMemRes)[uRht * aplNELMCom], ((LPAPLHETERO) lpMemRht)[uRht]->stData.stFloat, MPFR_RNDN);

                                    break;


                                case IMMTYPE_RAT:       // Can't happen w/PTR_STCONST
                                case IMMTYPE_VFP:       // ...
                                case IMMTYPE_HC2I:      // ...
                                case IMMTYPE_HC2F:      // ...
                                case IMMTYPE_HC2R:      // ...
                                case IMMTYPE_HC2V:      // ...
                                case IMMTYPE_HC4I:      // ...
                                case IMMTYPE_HC4F:      // ...
                                case IMMTYPE_HC4R:      // ...
                                case IMMTYPE_HC4V:      // ...
                                case IMMTYPE_HC8I:      // ...
                                case IMMTYPE_HC8F:      // ...
                                case IMMTYPE_HC8R:      // ...
                                case IMMTYPE_HC8V:      // ...
                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case ARRAY_HC2I:                // Can't happen w/PTR_STCONST
                        case ARRAY_HC2F:                // ...
                        case ARRAY_HC2R:                // ...
                        case ARRAY_HC2V:                // ...
                        case ARRAY_HC4I:                // ...
                        case ARRAY_HC4F:                // ...
                        case ARRAY_HC4R:                // ...
                        case ARRAY_HC4V:                // ...
                        case ARRAY_HC8I:                // ...
                        case ARRAY_HC8F:                // ...
                        case ARRAY_HC8R:                // ...
                        case ARRAY_HC8V:                // ...
                        defstop
                            break;
                    } // End SWITCH

                    break;

                case PTRTYPE_HGLOBAL:
                    // Get the item's global handle
                    hGlbSub = ((LPAPLNESTED) lpMemRht)[uRht];

                    // Lock the memory to get a ptr to it
                    lpMemHdrSub = MyGlobalLockVar (hGlbSub);

#define lpHeader    lpMemHdrSub
                    // Get the Array Type, NELM, and Rank
                    aplTypeSub = lpHeader->ArrType;
                    aplRankSub = lpHeader->Rank;
#undef  lpHeader
                    // Skip over the header to the dimensions
                    lpMemSub = VarArrayBaseToDim (lpMemHdrSub);

                    // Trundle through the right arg item's shape, ...
                    if (!IsScalar (aplRankSub))
                    {
                        // Calculate the product of the dimensions except for the last
                        for (aplNELMSubRest = 1, uSub = 0; uSub < (aplRankSub - 1); uSub++)
                            aplNELMSubRest *= *((LPAPLDIM) lpMemSub)++;

                        // Get the last dimension
                        aplNELMSubLast = *((LPAPLDIM) lpMemSub)++;
                    } else
                        aplNELMSubLast = aplNELMSubRest = 1;

                    // lpMemSub now points to the right arg item's data

                    // Split cases based upon the result storage type
                    switch (aplTypeRes)
                    {
                        case ARRAY_BOOL:
                            // The fill element of zero is already present via GHND

                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = uSub = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast =        0; uSubLast < aplNELMSubLast; uSubLast++, uSub++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;
                                // Get the index into the Boolean array
                                aplIndex = (uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast);

                                ((LPAPLBOOL)   lpMemRes)[aplIndex >> LOG2NBIB] |=
                                  (BIT0 & (APLBOOL) GetNextInteger (lpMemSub, aplTypeSub, uSub)) << (MASKLOG2NBIB & (UINT) aplIndex);
                            } // End FOR/FOR

                            // The following code is commented out because the fill element of zero
                            //   is already in <lpMemRes>.

////////////////////////////// Loop through the missing elements in the result (right arg item's cols)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
////////////////////////////for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    ((LPAPLBOOL)   lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
////////////////////////////      FALSE;
////////////////////////////} // End FOR/FOR
////////////////////////////
////////////////////////////// Loop through the missing elements in the result (right arg item's rows)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
////////////////////////////for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    ((LPAPLBOOL)   lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
////////////////////////////      FALSE;
////////////////////////////} // End FOR/FOR

                            break;

                        case ARRAY_INT:
                            // The fill element of zero is already present via GHND

                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = uSub = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast =        0; uSubLast < aplNELMSubLast; uSubLast++, uSub++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLINT)    lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  GetNextInteger (lpMemSub, aplTypeSub, uSub);
                            } // End FOR/FOR

                            // The following code is commented out because the fill element of zero
                            //   is already in <lpMemRes>.

////////////////////////////// Loop through the missing elements in the result (right arg item's cols)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
////////////////////////////for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    ((LPAPLINT)    lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
////////////////////////////      0;
////////////////////////////} // End FOR/FOR
////////////////////////////
////////////////////////////// Loop through the missing elements in the result (right arg item's rows)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
////////////////////////////for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    ((LPAPLINT)    lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
////////////////////////////      0;
////////////////////////////} // End FOR/FOR

                            break;

                        case ARRAY_FLOAT:
                            // The fill element of zero is already present via GHND

                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = uSub = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast =        0; uSubLast < aplNELMSubLast; uSubLast++, uSub++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLFLOAT)  lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  GetNextFloat (lpMemSub, aplTypeSub, uSub);
                            } // End FOR/FOR

                            // The following code is commented out because the fill element of zero
                            //   is already in <lpMemRes>.

////////////////////////////// Loop through the missing elements in the result (right arg item's cols)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
////////////////////////////for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    ((LPAPLFLOAT)  lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
////////////////////////////      0.0;
////////////////////////////} // End FOR/FOR
////////////////////////////
////////////////////////////// Loop through the missing elements in the result (right arg item's rows)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
////////////////////////////for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    ((LPAPLFLOAT)  lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
////////////////////////////      0.0;
////////////////////////////} // End FOR/FOR

                            break;

                        case ARRAY_CHAR:
                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMSubLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLCHAR)   lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  *((LPAPLCHAR) lpMemSub)++;
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's cols)
                            //   copying the prototype
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLCHAR)   lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  L' ';
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's rows)
                            //   copying the prototype
                            for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLCHAR)   lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  L' ';
                            } // End FOR/FOR

                            break;

                        case ARRAY_RAT:
                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = uSub = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast =        0; uSubLast < aplNELMSubLast; uSubLast++, uSub++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert this item to the target storage type
                                (*aTypeActPromote[aplTypeSub][aplTypeRes]) (lpMemSub, uSub, &atSub);

                                // Copy the converted item to the result
                                ((LPAPLRAT) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] = atSub.aplRat;

                                // DO NOT FREE at it is saved not copied to the result
////////////////////////////////// Free the old atSub
////////////////////////////////(*aTypeFree[aplTypeRes]) (&atSub, 0);

                                // Zero the memory in case we use it again
                                ZeroMemory (&atSub, sizeof (atSub));
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's cols)
                            //   copying the prototype
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Initialize to 0/1
                                mpq_init     (&((LPAPLRAT)    lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)]);
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's rows)
                            //   copying the prototype
                            for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Initialize to 0/1
                                mpq_init     (&((LPAPLRAT)    lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)]);
                            } // End FOR/FOR

                            break;

                        case ARRAY_VFP:
                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = uSub = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast =        0; uSubLast < aplNELMSubLast; uSubLast++, uSub++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert this item to the target storage type
                                (*aTypeActPromote[aplTypeSub][aplTypeRes]) (lpMemSub, uSub, &atSub);

                                // Copy the converted item to the result
                                ((LPAPLVFP) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] = atSub.aplVfp;

                                // DO NOT FREE at it is saved not copied to the result
////////////////////////////////// Free the old atSub
////////////////////////////////(*aTypeFree[aplTypeRes]) (&atSub, 0);

                                // Zero the memory in case we use it again
                                ZeroMemory (&atSub, sizeof (atSub));
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's cols)
                            //   copying the prototype
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                mpfr_init0   (&((LPAPLVFP)    lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)]);
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's rows)
                            //   copying the prototype
                            for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                mpfr_init0   (&((LPAPLVFP)    lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)]);
                            } // End FOR/FOR

                            break;

                        case ARRAY_HC2I:
                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = uSub = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast =        0; uSubLast < aplNELMSubLast; uSubLast++, uSub++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert this item to the target storage type
                                (*aTypeActPromote[aplTypeSub][aplTypeRes]) (lpMemSub, uSub, &atSub);

                                // Copy the converted item to the result
                                ((LPAPLHC2I) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] = atSub.aplHC2I;

                                // DO NOT FREE at it is saved not copied to the result
////////////////////////////////// Free the old atSub
////////////////////////////////(*aTypeFree[aplTypeRes]) (&atSub, 0);

                                // Zero the memory in case we use it again
                                ZeroMemory (&atSub, sizeof (atSub));
                            } // End FOR/FOR

                            // The following code is commented out because the fill element of zero
                            //   is already in <lpMemRes>.

////////////////////////////// Loop through the missing elements in the result (right arg item's cols)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
////////////////////////////for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    // Loop through all of the parts
////////////////////////////    for (i = 0; i < 2; i++)
////////////////////////////        ((LPAPLHC2I) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i] = 0;
////////////////////////////} // End FOR/FOR
////////////////////////////
////////////////////////////// Loop through the missing elements in the result (right arg item's rows)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
////////////////////////////for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    // Loop through all of the parts
////////////////////////////    for (i = 0; i < 2; i++)
////////////////////////////        ((LPAPLHC2I) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i] = 0;
////////////////////////////} // End FOR/FOR

                            break;

                        case ARRAY_HC4I:
                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = uSub = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast =        0; uSubLast < aplNELMSubLast; uSubLast++, uSub++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert this item to the target storage type
                                (*aTypeActPromote[aplTypeSub][aplTypeRes]) (lpMemSub, uSub, &atSub);

                                // Copy the converted item to the result
                                ((LPAPLHC4I) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] = atSub.aplHC4I;

                                // DO NOT FREE at it is saved not copied to the result
////////////////////////////////// Free the old atSub
////////////////////////////////(*aTypeFree[aplTypeRes]) (&atSub, 0);

                                // Zero the memory in case we use it again
                                ZeroMemory (&atSub, sizeof (atSub));
                            } // End FOR/FOR

                            // The following code is commented out because the fill element of zero
                            //   is already in <lpMemRes>.

////////////////////////////// Loop through the missing elements in the result (right arg item's cols)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
////////////////////////////for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    // Loop through all of the parts
////////////////////////////    for (i = 0; i < 4; i++)
////////////////////////////        ((LPAPLHC4I) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i] = 0;
////////////////////////////} // End FOR/FOR
////////////////////////////
////////////////////////////// Loop through the missing elements in the result (right arg item's rows)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
////////////////////////////for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    // Loop through all of the parts
////////////////////////////    for (i = 0; i < 4; i++)
////////////////////////////        ((LPAPLHC4I) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i] = 0;
////////////////////////////} // End FOR/FOR

                            break;

                        case ARRAY_HC8I:
                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = uSub = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast =        0; uSubLast < aplNELMSubLast; uSubLast++, uSub++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert this item to the target storage type
                                (*aTypeActPromote[aplTypeSub][aplTypeRes]) (lpMemSub, uSub, &atSub);

                                // Copy the converted item to the result
                                ((LPAPLHC8I) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] = atSub.aplHC8I;

                                // DO NOT FREE at it is saved not copied to the result
////////////////////////////////// Free the old atSub
////////////////////////////////(*aTypeFree[aplTypeRes]) (&atSub, 0);

                                // Zero the memory in case we use it again
                                ZeroMemory (&atSub, sizeof (atSub));
                            } // End FOR/FOR

                            // The following code is commented out because the fill element of zero
                            //   is already in <lpMemRes>.

////////////////////////////// Loop through the missing elements in the result (right arg item's cols)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
////////////////////////////for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    // Loop through all of the parts
////////////////////////////    for (i = 0; i < 8; i++)
////////////////////////////        ((LPAPLHC8I) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i] = 0;
////////////////////////////} // End FOR/FOR
////////////////////////////
////////////////////////////// Loop through the missing elements in the result (right arg item's rows)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
////////////////////////////for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    // Loop through all of the parts
////////////////////////////    for (i = 0; i < 8; i++)
////////////////////////////        ((LPAPLHC8I) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i] = 0;
////////////////////////////} // End FOR/FOR

                            break;

                        case ARRAY_HC2F:
                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = uSub = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast =        0; uSubLast < aplNELMSubLast; uSubLast++, uSub++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert this item to the target storage type
                                (*aTypeActPromote[aplTypeSub][aplTypeRes]) (lpMemSub, uSub, &atSub);

                                // Copy the converted item to the result
                                ((LPAPLHC2F) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] = atSub.aplHC2F;

                                // DO NOT FREE at it is saved not copied to the result
////////////////////////////////// Free the old atSub
////////////////////////////////(*aTypeFree[aplTypeRes]) (&atSub, 0);

                                // Zero the memory in case we use it again
                                ZeroMemory (&atSub, sizeof (atSub));
                            } // End FOR/FOR

                            // The following code is commented out because the fill element of zero
                            //   is already in <lpMemRes>.

////////////////////////////// Loop through the missing elements in the result (right arg item's cols)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
////////////////////////////for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    // Loop through all of the parts
////////////////////////////    for (i = 0; i < 2; i++)
////////////////////////////        ((LPAPLHC2F) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i] = 0;
////////////////////////////} // End FOR/FOR
////////////////////////////
////////////////////////////// Loop through the missing elements in the result (right arg item's rows)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
////////////////////////////for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    // Loop through all of the parts
////////////////////////////    for (i = 0; i < 2; i++)
////////////////////////////        ((LPAPLHC2F) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i] = 0;
////////////////////////////} // End FOR/FOR

                            break;

                        case ARRAY_HC4F:
                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = uSub = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast =        0; uSubLast < aplNELMSubLast; uSubLast++, uSub++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert this item to the target storage type
                                (*aTypeActPromote[aplTypeSub][aplTypeRes]) (lpMemSub, uSub, &atSub);

                                // Copy the converted item to the result
                                ((LPAPLHC4F) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] = atSub.aplHC4F;

                                // DO NOT FREE at it is saved not copied to the result
////////////////////////////////// Free the old atSub
////////////////////////////////(*aTypeFree[aplTypeRes]) (&atSub, 0);

                                // Zero the memory in case we use it again
                                ZeroMemory (&atSub, sizeof (atSub));
                            } // End FOR/FOR

                            // The following code is commented out because the fill element of zero
                            //   is already in <lpMemRes>.

////////////////////////////// Loop through the missing elements in the result (right arg item's cols)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
////////////////////////////for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    // Loop through all of the parts
////////////////////////////    for (i = 0; i < 4; i++)
////////////////////////////        ((LPAPLHC4F) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i] = 0;
////////////////////////////} // End FOR/FOR
////////////////////////////
////////////////////////////// Loop through the missing elements in the result (right arg item's rows)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
////////////////////////////for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    // Loop through all of the parts
////////////////////////////    for (i = 0; i < 4; i++)
////////////////////////////        ((LPAPLHC4F) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i] = 0;
////////////////////////////} // End FOR/FOR

                            break;

                        case ARRAY_HC8F:
                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = uSub = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast =        0; uSubLast < aplNELMSubLast; uSubLast++, uSub++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert this item to the target storage type
                                (*aTypeActPromote[aplTypeSub][aplTypeRes]) (lpMemSub, uSub, &atSub);

                                // Copy the converted item to the result
                                ((LPAPLHC8F) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] = atSub.aplHC8F;

                                // DO NOT FREE at it is saved not copied to the result
////////////////////////////////// Free the old atSub
////////////////////////////////(*aTypeFree[aplTypeRes]) (&atSub, 0);

                                // Zero the memory in case we use it again
                                ZeroMemory (&atSub, sizeof (atSub));
                            } // End FOR/FOR

                            // The following code is commented out because the fill element of zero
                            //   is already in <lpMemRes>.

////////////////////////////// Loop through the missing elements in the result (right arg item's cols)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
////////////////////////////for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    // Loop through all of the parts
////////////////////////////    for (i = 0; i < 8; i++)
////////////////////////////        ((LPAPLHC8F) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i] = 0;
////////////////////////////} // End FOR/FOR
////////////////////////////
////////////////////////////// Loop through the missing elements in the result (right arg item's rows)
//////////////////////////////   copying the prototype
////////////////////////////for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
////////////////////////////for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    // Loop through all of the parts
////////////////////////////    for (i = 0; i < 8; i++)
////////////////////////////        ((LPAPLHC8F) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i] = 0;
////////////////////////////} // End FOR/FOR

                            break;

                        case ARRAY_HC2R:
                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = uSub = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast =        0; uSubLast < aplNELMSubLast; uSubLast++, uSub++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert this item to the target storage type
                                (*aTypeActPromote[aplTypeSub][aplTypeRes]) (lpMemSub, uSub, &atSub);

                                // Copy the converted item to the result
                                ((LPAPLHC2R) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] = atSub.aplHC2R;

                                // DO NOT FREE at it is saved not copied to the result
////////////////////////////////// Free the old atSub
////////////////////////////////(*aTypeFree[aplTypeRes]) (&atSub, 0);

                                // Zero the memory in case we use it again
                                ZeroMemory (&atSub, sizeof (atSub));
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's cols)
                            //   copying the prototype
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Loop through all of the parts
                                for (i = 0; i < 2; i++)
                                    // Initialize to 0/1
                                    mpq_init (&((LPAPLHC2R) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i]);
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's rows)
                            //   copying the prototype
                            for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Loop through all of the parts
                                for (i = 0; i < 2; i++)
                                    // Initialize to 0/1
                                    mpq_init (&((LPAPLHC2R) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i]);
                            } // End FOR/FOR

                            break;

                        case ARRAY_HC4R:
                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = uSub = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast =        0; uSubLast < aplNELMSubLast; uSubLast++, uSub++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert this item to the target storage type
                                (*aTypeActPromote[aplTypeSub][aplTypeRes]) (lpMemSub, uSub, &atSub);

                                // Copy the converted item to the result
                                ((LPAPLHC4R) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] = atSub.aplHC4R;

                                // DO NOT FREE at it is saved not copied to the result
////////////////////////////////// Free the old atSub
////////////////////////////////(*aTypeFree[aplTypeRes]) (&atSub, 0);

                                // Zero the memory in case we use it again
                                ZeroMemory (&atSub, sizeof (atSub));
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's cols)
                            //   copying the prototype
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Loop through all of the parts
                                for (i = 0; i < 4; i++)
                                    // Initialize to 0/1
                                    mpq_init (&((LPAPLHC4R) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i]);
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's rows)
                            //   copying the prototype
                            for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Loop through all of the parts
                                for (i = 0; i < 4; i++)
                                    // Initialize to 0/1
                                    mpq_init (&((LPAPLHC4R) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i]);
                            } // End FOR/FOR

                            break;

                        case ARRAY_HC8R:
                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = uSub = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast =        0; uSubLast < aplNELMSubLast; uSubLast++, uSub++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert this item to the target storage type
                                (*aTypeActPromote[aplTypeSub][aplTypeRes]) (lpMemSub, uSub, &atSub);

                                // Copy the converted item to the result
                                ((LPAPLHC8R) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] = atSub.aplHC8R;

                                // DO NOT FREE at it is saved not copied to the result
////////////////////////////////// Free the old atSub
////////////////////////////////(*aTypeFree[aplTypeRes]) (&atSub, 0);

                                // Zero the memory in case we use it again
                                ZeroMemory (&atSub, sizeof (atSub));
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's cols)
                            //   copying the prototype
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Loop through all of the parts
                                for (i = 0; i < 8; i++)
                                    // Initialize to 0/1
                                    mpq_init (&((LPAPLHC8R) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i]);
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's rows)
                            //   copying the prototype
                            for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Loop through all of the parts
                                for (i = 0; i < 8; i++)
                                    // Initialize to 0/1
                                    mpq_init (&((LPAPLHC8R) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i]);
                            } // End FOR/FOR

                            break;

                        case ARRAY_HC2V:
                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = uSub = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast =        0; uSubLast < aplNELMSubLast; uSubLast++, uSub++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert this item to the target storage type
                                (*aTypeActPromote[aplTypeSub][aplTypeRes]) (lpMemSub, uSub, &atSub);

                                // Copy the converted item to the result
                                ((LPAPLHC2V) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] = atSub.aplHC2V;

                                // DO NOT FREE at it is saved not copied to the result
////////////////////////////////// Free the old atSub
////////////////////////////////(*aTypeFree[aplTypeRes]) (&atSub, 0);

                                // Zero the memory in case we use it again
                                ZeroMemory (&atSub, sizeof (atSub));
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's cols)
                            //   copying the prototype
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Loop through all of the parts
                                for (i = 0; i < 2; i++)
                                    mpfr_init0 (&((LPAPLHC2V) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i]);
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's rows)
                            //   copying the prototype
                            for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Loop through all of the parts
                                for (i = 0; i < 2; i++)
                                    mpfr_init0 (&((LPAPLHC2V) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i]);
                            } // End FOR/FOR

                            break;

                        case ARRAY_HC4V:
                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = uSub = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast =        0; uSubLast < aplNELMSubLast; uSubLast++, uSub++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert this item to the target storage type
                                (*aTypeActPromote[aplTypeSub][aplTypeRes]) (lpMemSub, uSub, &atSub);

                                // Copy the converted item to the result
                                ((LPAPLHC4V) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] = atSub.aplHC4V;

                                // DO NOT FREE at it is saved not copied to the result
////////////////////////////////// Free the old atSub
////////////////////////////////(*aTypeFree[aplTypeRes]) (&atSub, 0);

                                // Zero the memory in case we use it again
                                ZeroMemory (&atSub, sizeof (atSub));
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's cols)
                            //   copying the prototype
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Loop through all of the parts
                                for (i = 0; i < 4; i++)
                                    mpfr_init0 (&((LPAPLHC4V) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i]);
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's rows)
                            //   copying the prototype
                            for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Loop through all of the parts
                                for (i = 0; i < 4; i++)
                                    mpfr_init0 (&((LPAPLHC4V) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i]);
                            } // End FOR/FOR

                            break;

                        case ARRAY_HC8V:
                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = uSub = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast =        0; uSubLast < aplNELMSubLast; uSubLast++, uSub++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Convert this item to the target storage type
                                (*aTypeActPromote[aplTypeSub][aplTypeRes]) (lpMemSub, uSub, &atSub);

                                // Copy the converted item to the result
                                ((LPAPLHC8V) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] = atSub.aplHC8V;

                                // DO NOT FREE at it is saved not copied to the result
////////////////////////////////// Free the old atSub
////////////////////////////////(*aTypeFree[aplTypeRes]) (&atSub, 0);

                                // Zero the memory in case we use it again
                                ZeroMemory (&atSub, sizeof (atSub));
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's cols)
                            //   copying the prototype
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Loop through all of the parts
                                for (i = 0; i < 8; i++)
                                    mpfr_init0 (&((LPAPLHC8V) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i]);
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's rows)
                            //   copying the prototype
                            for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Loop through all of the parts
                                for (i = 0; i < 8; i++)
                                    mpfr_init0 (&((LPAPLHC8V) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)].parts[i]);
                            } // End FOR/FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbSub); lpMemHdrSub = NULL;

                    break;

                defstop
                    break;
            } // End SWITCH
        } // End FOR
    } else
    {
        //***************************************************************
        // At this point, the right arg is nested and non-empty
        //   and the result is nested/hetero.
        //***************************************************************

        // Loop through the elements of the right arg
        for (uRht = 0; uRht < aplNELMRht; uRht++)
        {
            // Split cases based upon the ptr type of the item
            switch (GetPtrTypeDir (((LPAPLNESTED) lpMemRht)[uRht]))
            {
                case PTRTYPE_STCONST:
                    // Get the item as a prototype
                    if (IsImmChr (((LPAPLHETERO) lpMemRht)[uRht]->stFlags.ImmType))
                        lpSymProto = lpSymB;
                    else
                        lpSymProto = lpSym0;

                    // Loop through the rest of common item's elements
                    //   saving the item's prototype
                    for (uSubRest = 0; uSubRest < aplNELMComRest; uSubRest++)
                    for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                           CopySymGlbDir_PTB (lpSymProto);
                    } // End FOR/FOR

                    // Overtake this scalar
                    ((LPAPLNESTED) lpMemRes)[uRht * aplNELMCom] = ((LPAPLNESTED) lpMemRht)[uRht];

                    break;

                case PTRTYPE_HGLOBAL:
                    // Get the item's global handle
                    hGlbSub = ((LPAPLNESTED) lpMemRht)[uRht];

                    // Lock the memory to get a ptr to it
                    lpMemHdrSub = MyGlobalLockVar (hGlbSub);

#define lpHeader    lpMemHdrSub
                    // Get the Array Type, NELM, and Rank
                    aplTypeSub = lpHeader->ArrType;
                    aplRankSub = lpHeader->Rank;
#undef  lpHeader
                    // Skip over the header to the dimensions
                    lpMemSub = VarArrayBaseToDim (lpMemHdrSub);

                    // Trundle through the right arg item's shape, ...
                    if (!IsScalar (aplRankSub))
                    {
                        // Calculate the product of the dimensions except for the last
                        for (aplNELMSubRest = 1, uSub = 0; uSub < (aplRankSub - 1); uSub++)
                            aplNELMSubRest *= *((LPAPLDIM) lpMemSub)++;

                        // Get the last dimension
                        aplNELMSubLast = *((LPAPLDIM) lpMemSub)++;
                    } else
                        aplNELMSubLast = aplNELMSubRest = 1;

                    // lpMemSub now points to the right arg item's data

                    // Split cases based upon the right arg item's storage type
                    switch (aplTypeSub)
                    {
                        case ARRAY_BOOL:
                            // Get the prototype
                            lpSymProto = lpSym0;

                            uBitMask = BIT0;

                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMSubLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  (uBitMask & *(LPAPLBOOL) lpMemSub) ? lpSym1 : lpSym0;

                                // Shift over the bit mask
                                uBitMask <<= 1;

                                // Check for end-of-byte
                                if (uBitMask EQ END_OF_BYTE)
                                {
                                    uBitMask = BIT0;            // Start over
                                    ((LPAPLBOOL) lpMemSub)++;   // Skip to next byte
                                } // End IF
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's cols)
                            //   copying the prototype
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  CopySymGlbDir_PTB (lpSymProto);
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's rows)
                            //   copying the prototype
                            for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  CopySymGlbDir_PTB (lpSymProto);
                            break;

                        case ARRAY_INT:
                            // Get the prototype
                            lpSymProto = lpSym0;

                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMSubLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                lpSymTmp =
                                  SymTabAppendInteger_EM (*((LPAPLINT) lpMemSub)++, TRUE);
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's cols)
                            //   copying the prototype
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  CopySymGlbDir_PTB (lpSymProto);
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's rows)
                            //   copying the prototype
                            for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  CopySymGlbDir_PTB (lpSymProto);
                            } // End FOR/FOR

                            break;

                        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemSub)
                            // Get the APA parameters
                            apaOffSub = lpAPA->Off;
                            apaMulSub = lpAPA->Mul;
#undef  lpAPA
                            // Get the prototype
                            lpSymProto = lpSym0;

                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMSubLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                lpSymTmp =
                                  SymTabAppendInteger_EM (apaOffSub + apaMulSub * (uSubLast + (uSubRest * aplNELMSubLast)), TRUE);
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's cols)
                            //   copying the prototype
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  CopySymGlbDir_PTB (lpSymProto);
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's rows)
                            //   copying the prototype
                            for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  CopySymGlbDir_PTB (lpSymProto);
                            } // End FOR/FOR

                            break;

                        case ARRAY_FLOAT:
                            // Get the prototype
                            lpSymProto = lpSym0;

                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMSubLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                lpSymTmp =
                                  SymTabAppendFloat_EM (*((LPAPLFLOAT) lpMemSub)++);
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's cols)
                            //   copying the prototype
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  CopySymGlbDir_PTB (lpSymProto);
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's rows)
                            //   copying the prototype
                            for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  CopySymGlbDir_PTB (lpSymProto);
                            } // End FOR/FOR

                            break;

                        case ARRAY_CHAR:
                            // Get the prototype
                            lpSymProto = lpSymB;

                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMSubLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                lpSymTmp =
                                  SymTabAppendChar_EM (*((LPAPLCHAR) lpMemSub)++, TRUE);
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's cols)
                            //   copying the prototype
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  CopySymGlbDir_PTB (lpSymProto);
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's rows)
                            //   copying the prototype
                            for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  CopySymGlbDir_PTB (lpSymProto);
                            } // End FOR/FOR

                            break;

                        case ARRAY_HETERO:
                        case ARRAY_NESTED:
                            // Split cases based upon the ptr type bits
                            switch (GetPtrTypeInd ((LPAPLNESTED) lpMemSub))
                            {
                                case PTRTYPE_STCONST:
                                    // If the STE is numeric, use lpSym0, otherwise use lpSymB
                                    if (IsImmChr (((LPAPLHETERO) lpMemSub)[0]->stFlags.ImmType))
                                        hSymGlbProto = lpSymB;
                                    else
                                        hSymGlbProto = lpSym0;
                                    break;

                                case PTRTYPE_HGLOBAL:
                                    // Calculate the right arg's first element's prototype
                                    hSymGlbProto =
                                      MakeMonPrototype_EM_PTB (*(LPAPLNESTED) lpMemSub, // Proto arg handle
                                                               ARRAY_NESTED,            // Array storage type
                                                               lptkFunc,                // Ptr to function token
                                                               MP_CHARS);               // CHARs allowed
                                    if (!hSymGlbProto)
                                        goto WSFULL_EXIT;

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMSubLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  CopySymGlbDir_PTB (*((LPAPLNESTED) lpMemSub)++);
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's cols)
                            //   copying the prototype
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  CopySymGlbDir_PTB (hSymGlbProto);
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's rows)
                            //   copying the prototype
                            for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  CopySymGlbDir_PTB (hSymGlbProto);
                            } // End FOR/FOR

                            // If the prototype is a global memory handle, ...
                            if (GetPtrTypeDir (hSymGlbProto) EQ PTRTYPE_HGLOBAL)
                            {
                                // We no longer need this storage
                                FreeResultGlobalVar (hSymGlbProto); hSymGlbProto = NULL;
                            } // End IF

                            break;

                        case ARRAY_HC2I:
                        case ARRAY_HC4I:
                        case ARRAY_HC8I:
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
                            // Clear atSub
                            ZeroMemory (&atSub, sizeof (atSub));

                            // Split cases based upon the right arg item's storage type
                            switch (aplTypeSub)
                            {
                                case ARRAY_HC2I:
                                case ARRAY_HC4I:
                                case ARRAY_HC8I:
                                case ARRAY_HC2F:
                                case ARRAY_HC4F:
                                case ARRAY_HC8F:
                                    // Already initialized to 0
                                    break;

                                case ARRAY_RAT:
                                    // Initialize the temp to 0/1
                                    mpq_init (&atSub.aplRat);

                                    break;

                                case ARRAY_HC2R:
                                    // Initialize the temp to 0/1
                                    mphc2r_init (&atSub.aplHC2R);

                                    break;

                                case ARRAY_HC4R:
                                    // Initialize the temp to 0/1
                                    mphc4r_init (&atSub.aplHC4R);

                                    break;

                                case ARRAY_HC8R:
                                    // Initialize the temp to 0/1
                                    mphc8r_init (&atSub.aplHC8R);

                                    break;

                                case ARRAY_VFP:
                                    // Initialize the temp to 0
                                    mpfr_init0 (&atSub.aplVfp);

                                    break;

                                case ARRAY_HC2V:
                                    // Initialize the temp to 0
                                    mphc2v_init0 (&atSub.aplHC2V);

                                    break;

                                case ARRAY_HC4V:
                                    // Initialize the temp to 0
                                    mphc4v_init0 (&atSub.aplHC4V);

                                    break;

                                case ARRAY_HC8V:
                                    // Initialize the temp to 0
                                    mphc8v_init0 (&atSub.aplHC8V);

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            // Loop through the right arg item's elements
                            //   copying them to the result
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMSubLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  lpSymTmp =
                                    MakeGlbEntry_EM (aplTypeSub,                // Entry type
                                                    lpMemSub,                   // Ptr to the value
                                                    TRUE,                       // TRUE iff we should initialize the target first
                                                    lptkFunc);                  // Ptr to function token
                                // Skip over the item
                                ((LPBYTE) lpMemSub) += TranslateArrayTypeToSizeof (aplTypeSub);

                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's cols)
                            //   copying the prototype
                            for (uSubRest = 0; uSubRest < aplNELMSubRest; uSubRest++)
                            for (uSubLast = aplNELMSubLast; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  lpSymTmp =
                                    MakeGlbEntry_EM (aplTypeSub,                // Entry type
                                                   &atSub,                      // Ptr to the value
                                                    TRUE,                       // TRUE iff we should initialize the target first
                                                    lptkFunc);                  // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR/FOR

                            // Loop through the missing elements in the result (right arg item's rows)
                            //   copying the prototype
                            for (uSubRest = aplNELMSubRest; uSubRest < aplNELMComRest; uSubRest++)
                            for (uSubLast = 0; uSubLast < aplNELMComLast; uSubLast++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLNESTED) lpMemRes)[(uRht * aplNELMCom) + uSubLast + (uSubRest * aplNELMComLast)] =
                                  lpSymTmp =
                                    MakeGlbEntry_EM (aplTypeSub,                // Entry type
                                                   &atSub,                      // Ptr to the value
                                                    TRUE,                       // TRUE iff we should initialize the target first
                                                    lptkFunc);                  // Ptr to function token
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                            } // End FOR/FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbSub); lpMemHdrSub = NULL;

                    break;

                defstop
                    break;
            } // End FOR/SWITCH
        } // End FOR
    } // End IF/ELSE
NORMAL_EXIT:
    // Unlock the result global memory in case TypeDemote actually demotes
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
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

    // If there's an axis, transpose the result
    if (hGlbAxis NE NULL)
    {
        LPPL_YYSTYPE lpYYRes2,      // Ptr to secondary result
                     lpYYRes3;      // Ptr to tertiary result
        APLBOOL      bQuadIO;       // []IO

        // Calculate space needed for left arg
        ByteRes = CalcArraySize (ARRAY_INT, aplRankRes, 1);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Allocate space for the max shape of the items
        hGlbLft = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (hGlbLft EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrLft = MyGlobalLock000 (hGlbLft);

#define lpHeader        lpMemHdrLft
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_INT;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplRankRes;
        lpHeader->Rank       = 1;
#undef  lpHeader

        // Fill in the dimension
        *VarArrayBaseToDim (lpMemHdrLft) = aplRankRes;

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);

        // Lock the memory to get a ptr to it
        lpMemAxisHead = MyGlobalLockInt (hGlbAxis);

        // Copy the values from lpMemAxisHead
        CopyMemory (lpMemLft, lpMemAxisHead, (APLU3264) aplRankRes * sizeof (APLUINT));

        // Get the current value of []IO
        bQuadIO = GetQuadIO ();

        // Because lpMemAxishead is in origin-0, and the left arg to
        //   dyadic transpose is []IO-sensitive, we must
        //   loop through the left arg and add in []IO
        for (uCom = 0; uCom < aplRankRes; uCom++)
            *lpMemLft++ += bQuadIO;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;

        // Transpose the values

        // Allocate a new YYRes
        lpYYRes2 = YYAlloc ();

        // Fill in the left arg token
        lpYYRes2->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes2->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;// Already zero from YYAlloc
////////lpYYRes2->tkToken.tkFlags.NoDisplay = FALSE;        // Already zero from YYAlloc
        lpYYRes2->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbLft);
        lpYYRes2->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        // Transpose the values
        lpYYRes3 = PrimFnDydCircleSlope_EM_YY (&lpYYRes2->tkToken,  // Ptr to left arg token
                                                lptkFunc,           // Ptr to function token
                                               &lpYYRes->tkToken,   // Ptr to right arg token
                                                NULL);              // Ptr to axis token (may be NULL)
        // We no longer need this storage
        FreeResult (lpYYRes);  YYFree (lpYYRes);  lpYYRes  = NULL;
        FreeResult (lpYYRes2); YYFree (lpYYRes2); lpYYRes2 = NULL; // N.B.:  This frees hGlbLft, too.

        // Copy to result var
        lpYYRes = lpYYRes3; lpYYRes3 = NULL;
    } // End IF

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken, FALSE);

    goto TAIL_EXIT;

AXIS_EXIT:
    ErrorMessageIndirectToken (ERRMSG_AXIS_ERROR APPEND_NAME,
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

    bRet = FALSE;
TAIL_EXIT:
    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbDimCom, lpMemDimCom);

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbAxis, lpMemAxisHead);

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht);  lpMemHdrRht  = NULL;
    } // End IF

    if (hGlbSub NE NULL && lpMemHdrSub NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbSub);  lpMemHdrSub  = NULL;
    } // End IF

    if (bRet)
        return lpYYRes;
    else
        return NULL;
} // End PrimFnMonRightShoeGlb_EM_YY


//***************************************************************************
//  $PrimFnDydRightShoe_EM_YY
//
//  Primitive function for dyadic RightShoe ("pick")
//***************************************************************************

LPPL_YYSTYPE PrimFnDydRightShoe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
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

                return PrimFnDydRightShoeGlb_EM_YY (lptkLftArg,
                                                    lptkRhtArg->tkData.tkSym->stData.stGlbData,
                                                    lptkFunc);
            } // End IF

            // Handle the immediate case

            // If the left arg is valid, return the immediate in the right arg
            return PrimFnDydRightShoeImm_EM_YY (lptkRhtArg->tkData.tkSym->stFlags.ImmType,
                                                lptkRhtArg->tkData.tkSym->stData.stLongest,
                                                lptkLftArg,
                                                lptkFunc);
        case TKT_VARIMMED:
            // If the left arg is valid, return the immediate in the right arg
            return PrimFnDydRightShoeImm_EM_YY (lptkRhtArg->tkFlags.ImmType,
                                                lptkRhtArg->tkData.tkLongest,
                                                lptkLftArg,
                                                lptkFunc);
        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (lptkRhtArg->tkData.tkGlbData));

            return PrimFnDydRightShoeGlb_EM_YY (lptkLftArg,
                                                lptkRhtArg->tkData.tkGlbData,
                                                lptkFunc);
        defstop
            return NULL;
    } // End SWITCH

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnDydRightShoe_EM_YY


//***************************************************************************
//  $PrimFnDydRightShoeImm_EM_YY
//
//  Dyadic RightShoe ("pick") on an immediate right arg
//***************************************************************************

LPPL_YYSTYPE PrimFnDydRightShoeImm_EM_YY
    (IMM_TYPES  immTypeRht,         // The immediate type (see IMM_TYPES)
     APLLONGEST aplLongestRht,      // The immediate value
     LPTOKEN    lptkLftArg,         // Ptr to left arg token
     LPTOKEN    lptkFunc)           // Ptr to function token

{
    APLSTYPE          aplTypeLft;           // Left arg storage type
    APLNELM           aplNELMLft;           // Left arg NELM
    APLRANK           aplRankLft;           // Left arg rank
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    HGLOBAL           hGlbLft = NULL;       // Left arg global memory handle
    LPVARARRAY_HEADER lpMemHdrLft = NULL;   // Ptr to left arg header
    LPVOID            lpMemLft;             // Ptr to left arg global memory

    // Get the attributes (Type, NELM, and Rank)
    //   of the left arg
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto RANK_EXIT;

    // If the left arg is simple, it must be empty (i.e., {zilde} or '')
    if (IsSimple (aplTypeLft))
    {
        if (!IsEmpty (aplNELMLft))
            goto LENGTH_EXIT;
    } else
    {
        // Check for LEFT DOMAIN ERROR
        if (!IsNested (aplTypeLft)
         || IsEmpty (aplNELMLft))
            goto DOMAIN_EXIT;

        // From here on the left arg is a non-empty nested vector

        // Get left arg global ptr
        GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);

        // Skip over the header and dimensions
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);

        // Ensure the left arg items are zildes
        if (!PrimFnDydRightShoeGlbImm_EM (aplNELMLft,           // Left arg NELM
                                          lpMemLft,             // Ptr to left arg global memory
                                          0,                    // Left arg starting index
                                          lptkFunc))            // Ptr to function token
            goto ERROR_EXIT;
    } // End IF/ELSE

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = immTypeRht;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkLongest  = aplLongestRht;
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

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnDydRightShoeImm_EM_YY


//***************************************************************************
//  $PrimFnDydRightShoeGlbImm_EM
//
//  Dyadic RightShoe ("pick") on a left arg global memory handle
//    and right arg immediate
//***************************************************************************

UBOOL PrimFnDydRightShoeGlbImm_EM
    (APLNELM    aplNELMLft,     // Left arg NELM
     LPVOID     lpMemLft,       // Ptr to left arg global memory
     APLUINT    uLft,           // Left arg starting index
     LPTOKEN    lptkFunc)       // Ptr to function token

{
    HGLOBAL  hGlbLft;           // Left arg global memory handle
    APLSTYPE aplTypeSub;        // Left arg item storage type
    APLNELM  aplNELMSub;        // Left arg item NELM
    APLRANK  aplRankSub;        // Left arg item rank

    // Loop through the left arg to ensure that it is
    //   a nested vector of zildes
    for (; uLft < aplNELMLft; uLft++)
    {
        // Get the global memory handle
        hGlbLft = ((LPAPLNESTED) lpMemLft)[uLft];

        // Split cases based upon the ptr type of the element
        switch (GetPtrTypeDir (hGlbLft))
        {
            case PTRTYPE_STCONST:
                // It's a simple scalar, so signal a RANK ERROR
                goto RANK_EXIT;

            case PTRTYPE_HGLOBAL:
                // Ensure that this element is a simple empty vector

                // Get the attributes (Type, NELM, and Rank)
                //   of the item
                AttrsOfGlb (hGlbLft, &aplTypeSub, &aplNELMSub, &aplRankSub, NULL);

                // Check for LEFT RANK ERROR
                if (!IsVector (aplRankSub))
                    goto RANK_EXIT;

                // Check for LEFT LENGTH ERROR
                if (!IsEmpty (aplNELMSub))
                    goto LENGTH_EXIT;

                // Check for LEFT DOMAIN ERROR
                if (!IsSimple (aplTypeSub))     // Note we allow empty char
                    goto DOMAIN_EXIT;
                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR

    return TRUE;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    return FALSE;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    return FALSE;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    return FALSE;
} // End PrimFnDydRightShoeGlbImm_EM


//***************************************************************************
//  $PrimFnDydRightShoeGlb_EM_YY
//
//  Dyadic RightShoe ("pick") on a right arg global memory handle
//***************************************************************************

LPPL_YYSTYPE PrimFnDydRightShoeGlb_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     HGLOBAL hGlbRht,               // Handle to right arg
     LPTOKEN lptkFunc)              // Ptr to function token

{
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
    HGLOBAL           hGlbLft;              // Left arg global memory handle
    APLSTYPE          aplTypeLft;           // ...      storage type
    APLRANK           aplRankLft;           // ...      rank
    IMM_TYPES         immTypeLft;           // ...      immediate storage type
    APLINT            aplIntegerLft;        // ...      immediate value
    LPVARARRAY_HEADER lpMemHdrLft = NULL;   // Ptr to left arg header
    UBOOL             bRet;                 // TRUE iff the conversion is valid

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Split cases based upon the left arg's token type
    switch (lptkLftArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkLftArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkLftArg->tkData.tkSym->stFlags.Imm)
            {
                // Get the left arg global memory handle
                hGlbLft = lptkLftArg->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbLft));

                // Get the global attrs
                AttrsOfGlb (hGlbLft, &aplTypeLft, NULL, &aplRankLft, NULL);
                immTypeLft = TranslateArrayTypeToImmType (aplTypeLft);

                // If the left arg is a global numeric scalar, ...
                if (IsGlbNum (aplTypeLft) && IsScalar (aplRankLft))
                {
                    // Lock the memory to get a ptr to it
                    lpMemHdrLft = MyGlobalLockVar (hGlbLft);

                    // Attempt to convert the left arg value to an integer
                    aplIntegerLft =
                      ConvertToInteger_SCT (aplTypeLft,
                                            VarArrayDataFmBase (lpMemHdrLft),
                                            0,
                                           &bRet);
                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;

                    // Check for error
                    if (!bRet)
                        goto ERROR_EXIT;
                    // Handle the immediate case
                    lpYYRes =
                      PrimFnDydRightShoeImmGlb_EM_YY
                      (IMMTYPE_INT,                             // Immediate type
                       aplIntegerLft,                           // Immediate value
                       hGlbLft,                                 // Left arg global memory handle (may be NULL)
                       hGlbRht,                                 // Right arg global memory handle
                       lpMemPTD,                                // Ptr to PerTabData global memory
                       lptkFunc);                               // Ptr to function token
                } else
                {
                    lpYYRes =
                      PrimFnDydRightShoeGlbGlb_EM_YY
                      (hGlbLft,                                 // Left arg global memory handle
                       hGlbRht,                                 // Right arg global memory handle
                       lptkFunc,                                // Ptr to function token
                       FALSE,                                   // TRUE iff array assignment
                       ARRAY_ERROR,                             // Set arg storage type
                       NULL,                                    // Set arg global memory handle/LPSYMENTRY (NULL if immediate)
                       0,                                       // Set arg immediate value
                       lpMemPTD);                               // Ptr to PerTabData global memory
                    Assert (!IsPtrSuccess (lpYYRes));
                } // End IF/ELSE
            } else
                // Handle the immediate case
                lpYYRes =
                  PrimFnDydRightShoeImmGlb_EM_YY
                  (lptkLftArg->tkData.tkSym->stFlags.ImmType,   // Immediate type
                   lptkLftArg->tkData.tkSym->stData.stLongest,  // Immediate value
                   NULL,                                        // Left arg global memory handle (may be NULL)
                   hGlbRht,                                     // Right arg global memory handle
                   lpMemPTD,                                    // Ptr to PerTabData global memory
                   lptkFunc);                                   // Ptr to function token
            break;

        case TKT_VARIMMED:
            lpYYRes =
              PrimFnDydRightShoeImmGlb_EM_YY
              (lptkLftArg->tkFlags.ImmType,                 // Immediate type
               lptkLftArg->tkData.tkLongest,                // Immediate value
               NULL,                                        // Left arg global memory handle (may be NULL)
               hGlbRht,                                     // Right arg global memory handle
               lpMemPTD,                                    // Ptr to PerTabData global memory
               lptkFunc);                                   // Ptr to function token
            break;

        case TKT_VARARRAY:
            // Get the left arg global memory handle
            hGlbLft = lptkLftArg->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbLft));

            // Get the global attrs
            AttrsOfGlb (hGlbLft, &aplTypeLft, NULL, &aplRankLft, NULL);
            immTypeLft = TranslateArrayTypeToImmType (aplTypeLft);

            // If the left arg is a global numeric scalar, ...
            if (IsGlbNum (aplTypeLft) && IsScalar (aplRankLft))
            {
                // Lock the memory to get a ptr to it
                lpMemHdrLft = MyGlobalLockVar (hGlbLft);

                // Attempt to convert the left arg value to an integer
                aplIntegerLft =
                  ConvertToInteger_SCT (aplTypeLft,
                                        VarArrayDataFmBase (lpMemHdrLft),
                                        0,
                                       &bRet);
                // We no longer need this ptr
                MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;

                // Check for error
                if (!bRet)
                    goto ERROR_EXIT;
                // Handle the immediate case
                lpYYRes =
                  PrimFnDydRightShoeImmGlb_EM_YY
                  (IMMTYPE_INT,                             // Immediate type
                   aplIntegerLft,                           // Immediate value
                   hGlbLft,                                 // Left arg global memory handle (may be NULL)
                   hGlbRht,                                 // Right arg global memory handle
                   lpMemPTD,                                // Ptr to PerTabData global memory
                   lptkFunc);                               // Ptr to function token
            } else
            {
                lpYYRes =
                  PrimFnDydRightShoeGlbGlb_EM_YY
                  (hGlbLft,                                 // Left arg global memory handle
                   hGlbRht,                                 // Right arg global memory handle
                   lptkFunc,                                // Ptr to function token
                   FALSE,                                   // TRUE iff array assignment
                   ARRAY_ERROR,                             // Set arg storage type
                   NULL,                                    // Set arg global memory handle/LPSYMENTRY (NULL if immediate)
                   0,                                       // Set arg immediate value
                   lpMemPTD);                               // Ptr to PerTabData global memory
                Assert (!IsPtrSuccess (lpYYRes));
            } // End IF/ELSE

            break;

        defstop
            return NULL;
    } // End SWITCH

    if (lpYYRes && !IsPtrSuccess (lpYYRes))
        // See if it fits into a lower (but not necessarily smaller) datatype
        TypeDemote (&lpYYRes->tkToken, FALSE);
ERROR_EXIT:
    return lpYYRes;
} // End PrimFnDydRightShoeGlb_EM_YY


//***************************************************************************
//  $PrimFnDydRightShoeImmGlb_EM_YY
//
//  Dyadic RightShoe ("pick") on a left arg immediate
//    and right arg global memory handle
//***************************************************************************

LPPL_YYSTYPE PrimFnDydRightShoeImmGlb_EM_YY
    (IMM_TYPES    immTypeLft,           // Left arg immediate type (see IMM_TYPES)
     APLLONGEST   aplLongestLft,        // Left arg immediate value
     HGLOBAL      hGlbLft,              // Left arg global memory handle (may be NULL)
     HGLOBAL      hGlbRht,              // Right arg global memory handle
     LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     LPTOKEN      lptkFunc)             // Ptr to function token

{
    APLSTYPE          aplTypeRht;           // Right arg storage type
    APLNELM           aplNELMRht;           // Right arg NELM
    APLRANK           aplRankRht;           // Right arg rank
    IMM_TYPES         immTypeRes;           // Result immediate type (see IMM_TYPES)
    APLLONGEST        aplLongestRes;        // Result immediate value
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    UBOOL             bRet;                 // TRUE iff FloatToAplint_SCT is valid
    HGLOBAL           hGlbRes = NULL;       // Result global memory handle
    LPVARARRAY_HEADER lpMemHdrLft = NULL;   // Ptr to left arg header
    LPVOID            lpMemLft;             // Ptr to left arg global memory

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg global
    AttrsOfGlb (hGlbRht, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for LEFT LENGTH ERROR
    if (!IsVector (aplRankRht))
        goto LENGTH_EXIT;

    // Split cases based upon the left arg immediate type
    switch (immTypeLft)
    {
        case IMMTYPE_FLOAT:
            // Attempt to convert the float to an integer using System []CT
            aplLongestLft = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestLft, &bRet);

            break;

        case IMMTYPE_BOOL:
        case IMMTYPE_INT:
            bRet = TRUE;

            break;

        case IMMTYPE_CHAR:
            goto DOMAIN_EXIT;

        case IMMTYPE_RAT:
        case IMMTYPE_VFP:
        case IMMTYPE_HC2I:
        case IMMTYPE_HC2F:
        case IMMTYPE_HC2R:
        case IMMTYPE_HC2V:
        case IMMTYPE_HC4I:
        case IMMTYPE_HC4F:
        case IMMTYPE_HC4R:
        case IMMTYPE_HC4V:
        case IMMTYPE_HC8I:
        case IMMTYPE_HC8F:
        case IMMTYPE_HC8R:
        case IMMTYPE_HC8V:
            // Lock the memory to get a ptr to it
            lpMemHdrLft = MyGlobalLockVar (hGlbLft);

            // If it's not a scalar/vector singleton, ...
            if (IsMultiRank (lpMemHdrLft->Rank)
             || !IsSingleton (lpMemHdrLft->NELM))
                goto LENGTH_EXIT;

            // Skip over the header and dimensions to the data
            lpMemLft = VarArrayDataFmBase (lpMemHdrLft);

            // Attempt to convert to an integer using System []CT
            aplLongestLft = ConvertToInteger_SCT (TranslateImmTypeToArrayType (immTypeLft), lpMemLft, 0, &bRet);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;

            break;

        defstop
            break;
    } // End SWITCH

    if (!bRet)
        goto DOMAIN_EXIT;

    // Convert to origin-0
    aplLongestLft -= GetQuadIO ();

    // Check for negative indices [-aplNELMRht, -1]
    if (SIGN_APLLONGEST (aplLongestLft)
     && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
        aplLongestLft += aplNELMRht;

    // Ensure that the index is within range
    // N.B.:  Because APLLONGEST is unsigned, we don't have to worry about negatives
    if (aplLongestLft >= aplNELMRht)
        goto INDEX_EXIT;

    // Extract an element from the right arg
    GetNextValueGlb (hGlbRht,               // Right arg global memory handle
                     aplLongestLft,         // Index into right arg
                    &hGlbRes,               // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                    &aplLongestRes,         // Ptr to result immediate value (may be NULL)
                    &immTypeRes);           // Ptr to result immediate type (may be NULL)
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // If the result is an HGLOBAL
    if (hGlbRes NE NULL)
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbNumDir_PTB (hGlbRes, aplTypeRht, lptkFunc);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = immTypeRes;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkLongest  = aplLongestRes;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE

    return lpYYRes;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;

INDEX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnDydRightShoeImmGlb_EM_YY


//***************************************************************************
//  $PrimFnDydRightShoeGlbGlb_EM_YY
//
//  Dyadic RightShoe ("pick") on a left arg global memory handle
//    and right arg global memory handle
//***************************************************************************

LPPL_YYSTYPE PrimFnDydRightShoeGlbGlb_EM_YY
    (HGLOBAL      hGlbLft,              // Left  arg global memory handle
     HGLOBAL      hGlbRht,              // Right ...
     LPTOKEN      lptkFunc,             // Ptr to function token
     UBOOL        bArraySet,            // TRUE iff in array assignment
     APLSTYPE     aplTypeSet,           // Set arg storage type
     HGLOBAL      hGlbSet,              // Set arg global memory handle/LPSYMENTRY (NULL if immediate)
     APLLONGEST   aplLongestSet,        // Set arg immediate value
     LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    APLSTYPE          aplTypeLft,       // Left  arg storage type
                      aplTypeRht,       // Right ...
                      aplTypePro;       // Promoted  ...
    APLNELM           aplNELMLft,       // Left  arg NELM
                      aplNELMNstLft,    // Left  arg NELM if nested
                      aplNELMRht;       // Right arg NELM
    APLRANK           aplRankLft,       // Left  arg rank
                      aplRankRht;       // Right arg rank
    LPPL_YYSTYPE      lpYYRes = NULL;   // Ptr to the result
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg header
                      lpMemHdrRht = NULL;   // ...    right ...
    LPVOID            lpMemLft,         // Ptr to left arg global memory
                      lpMemRht;         // Ptr to right ...
    LPAPLDIM          lpMemDimRht;      // Ptr to right arg dimensions
    APLUINT           uLft;             // Loop counter
    UBOOL             bRet = TRUE,      // TRUE iff result is valid
                      bErrIndex = FALSE;// TRUE iff the error exit is for an INDEX ERROR
    APLBOOL           bQuadIO;          // []IO
    LPSYMENTRY        lpSymTmp;         // Ptr to temporary LPSYMENTRY
    LPPLLOCALVARS     lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;     // Ptr to Ctrl-Break flag
    ALLTYPES          atRht = {0};      // Right arg as ALLTYPES
    int               i,                // Loop counter
                      iHCDimRht;        // HC Dimension (1, 2, 4, 8)

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank)
    //   of the left arg global
    AttrsOfGlb (hGlbLft, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto RANK_EXIT;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Lock the memory to get a ptr to it
    lpMemHdrLft = MyGlobalLockVar (hGlbLft);

    // Skip over the header and dimensions to the data
    lpMemLft = VarArrayDataFmBase (lpMemHdrLft);

    // Take into account nested prototypes
    if (IsNested (aplTypeLft))
        aplNELMNstLft = max (aplNELMLft, 1);
    else
        aplNELMNstLft = aplNELMLft;

    // If the left arg is empty, ...
    if (IsEmpty (aplNELMLft))
        // Get the attributes (Type, NELM, and Rank)
        //   of the right arg global
        AttrsOfGlb (hGlbRht, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);
    else
    // Loop through the elements of the left arg
    //   reaching into the right arg
    for (uLft = 0; uLft < aplNELMNstLft; uLft++)
    {
        APLLONGEST aplLongestSubLft,        // Left arg item immediate value
                   aplLongestSubRht,        // Right arg item immediate value
                   aplLongestPrvLft;        // Previous left arg item index
        IMM_TYPES  immTypeSubLft,           // Left arg item immediate type
                   immTypeSubRht;           // Right arg item immediate type
        HGLOBAL    hGlbSubLft,              // Left arg item global memory handle
                   hGlbSubRht,              // Right ...
                   hGlbPrvRht;              // Previous right arg global memory handle
        UINT       uBitMask;                // Bit mask for looping through Booleans

        // Get the attributes (Type, NELM, and Rank)
        //   of the right arg global
        AttrsOfGlb (hGlbRht, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

        // Lock the memory to get a ptr to it
        lpMemHdrRht = MyGlobalLockVar (hGlbRht);

        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

        // Get the index value from the left arg
        GetNextValueGlb (hGlbLft,               // Left arg global memory handle
                         uLft,                  // Index
                        &hGlbSubLft,            // Ptr to result as HGLOBAL (may be NULL if singleton or simple)
                        &aplLongestSubLft,      // Ptr to result as singleton or simple value
                        &immTypeSubLft);        // Ptr to result as singleton or simple type
        // If the left arg item is a global value, ...
        if (hGlbSubLft NE NULL)
        {
            APLSTYPE          aplTypeSubLft;            // Left arg item storage type
            APLNELM           aplNELMSubLft;            // Left arg item NELM
            APLRANK           aplRankSubLft;            // Left arg item rank
            LPVARARRAY_HEADER lpMemHdrSubLft = NULL;    // Ptr to left subitem header
            LPVOID            lpMemSubLft;              // Ptr to left arg item global memory
            APLINT            apaOffSubLft,             // Left arg item APA offset
                              apaMulSubLft,             // Left arg item APA multiplier
                              iDim;                     // Loop counter
            APLUINT           aplWValSubLft,            // Left arg item weighting value
                              aplTmpSubLft;             // Left arg item temporary value

            // Get the attributes (Type, NELM, and Rank)
            //   of the left arg item global
            AttrsOfGlb (hGlbSubLft, &aplTypeSubLft, &aplNELMSubLft, &aplRankSubLft, NULL);

            // Check for LEFT RANK ERROR
            if (IsMultiRank (aplRankSubLft))
                goto RANK_EXIT;

            // Check for LEFT LENGTH ERROR
            if (aplNELMSubLft NE aplRankRht)
                goto LENGTH_EXIT;

            // Check for LEFT DOMAIN ERROR
            if (!IsSimpleGlbNum (aplTypeSubLft))
                goto DOMAIN_EXIT;

            // Lock the memory to get a ptr to it
            lpMemHdrSubLft = MyGlobalLockVar (hGlbSubLft);

            // Skip over the header and dimensions to the data
            lpMemSubLft = VarArrayDataFmBase (lpMemHdrSubLft);

            // Initialize accumulator to identity element for addition
            aplLongestSubLft = 0;

            // Initialize weighting value to identity element for multiplication
            aplWValSubLft = 1;

            // Split cases based upon the left arg item storage type
            switch (aplTypeSubLft)
            {
                case ARRAY_BOOL:
                    // Loop through the right arg dimensions
                    for (iDim = aplRankRht - 1; iDim >= 0; iDim--)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) iDim);

                        // Get the left arg item value in origin-0
                        aplTmpSubLft = ((uBitMask & ((LPAPLBOOL) lpMemSubLft)[iDim >> LOG2NBIB]) ? TRUE : FALSE) - bQuadIO;

                        // Check for negative indices [-lpMemDimRht[iDim], -1]
                        if (SIGN_APLLONGEST (aplTmpSubLft)
                         && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
                            aplTmpSubLft += lpMemDimRht[iDim];

                        // Ensure the indices are within range
                        bErrIndex = (lpMemDimRht[iDim] <= aplTmpSubLft);
                        if (bErrIndex)
                        {
                            bRet  = FALSE;

                            break;
                        } // End IF

                        // Add into accumulator
                        aplLongestSubLft += aplTmpSubLft * aplWValSubLft;

                        // Shift the weighting value over
                        aplWValSubLft *= lpMemDimRht[iDim];
                    } // End FOR

                    break;

                case ARRAY_INT:
                    // Loop through the right arg dimensions
                    for (iDim = aplRankRht - 1; iDim >= 0; iDim--)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Get the left arg item value in origin-0
                        aplTmpSubLft = ((LPAPLUINT) lpMemSubLft)[iDim] - bQuadIO;

                        // Check for negative indices [-lpMemDimRht[iDim], -1]
                        if (SIGN_APLLONGEST (aplTmpSubLft)
                         && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
                            aplTmpSubLft += lpMemDimRht[iDim];

                        // Ensure the indices are within range
                        bErrIndex = (lpMemDimRht[iDim] <= aplTmpSubLft);
                        if (bErrIndex)
                        {
                            bRet = FALSE;

                            break;
                        } // End IF

                        // Add into accumulator
                        aplLongestSubLft += aplTmpSubLft * aplWValSubLft;

                        // Shift the weighting value over
                        aplWValSubLft *= lpMemDimRht[iDim];
                    } // End FOR

                    break;

                case ARRAY_FLOAT:
                    // Loop through the right arg dimensions
                    for (iDim = aplRankRht - 1; iDim >= 0; iDim--)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Attempt to convert the float to an integer using System []CT
                        aplTmpSubLft = FloatToAplint_SCT (((LPAPLFLOAT) lpMemSubLft)[iDim], &bRet) - bQuadIO;

                        // Check for negative indices [-lpMemDimRht[iDim], -1]
                        if (SIGN_APLLONGEST (aplTmpSubLft)
                         && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
                            aplTmpSubLft += lpMemDimRht[iDim];

                        // Ensure the indices are within range
                        bErrIndex = (lpMemDimRht[iDim] <= aplTmpSubLft);
                        if (!bRet || bErrIndex)
                        {
                            bRet = FALSE;

                            break;
                        } // End IF

                        // Add into accumulator
                        aplLongestSubLft += aplTmpSubLft * aplWValSubLft;

                        // Shift the weighting value over
                        aplWValSubLft *= lpMemDimRht[iDim];
                    } // End FOR

                    break;

                case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemSubLft)
                    // Get the APA parameters
                    apaOffSubLft = lpAPA->Off;
                    apaMulSubLft = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the right arg dimensions
                    for (iDim = aplRankRht - 1; iDim >= 0; iDim--)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Get the left arg item value in origin-0
                        aplTmpSubLft = (apaOffSubLft + apaMulSubLft * iDim) - bQuadIO;

                        // Check for negative indices [-lpMemDimRht[iDim], -1]
                        if (SIGN_APLLONGEST (aplTmpSubLft)
                         && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
                            aplTmpSubLft += lpMemDimRht[iDim];

                        // Ensure the indices are within range
                        bErrIndex = (lpMemDimRht[iDim] <= aplTmpSubLft);
                        if (bErrIndex)
                        {
                            bRet = FALSE;

                            break;
                        } // End IF

                        // Add into accumulator
                        aplLongestSubLft += aplTmpSubLft * aplWValSubLft;

                        // Shift the weighting value over
                        aplWValSubLft *= lpMemDimRht[iDim];
                    } // End FOR

                    break;

                case ARRAY_CHAR:
                    if (IsEmpty (aplNELMSubLft))    // Allow empty chars
                        break;

                    // Fall through to common code

                case ARRAY_HETERO:
                case ARRAY_NESTED:
                    bRet = FALSE;

                    break;

                case ARRAY_RAT:
                case ARRAY_VFP:
                case ARRAY_HC2I:
                case ARRAY_HC2F:
                case ARRAY_HC2R:
                case ARRAY_HC2V:
                case ARRAY_HC4I:
                case ARRAY_HC4F:
                case ARRAY_HC4R:
                case ARRAY_HC4V:
                case ARRAY_HC8I:
                case ARRAY_HC8F:
                case ARRAY_HC8R:
                case ARRAY_HC8V:
                    // Loop through the right arg dimensions
                    for (iDim = aplRankRht - 1; iDim >= 0; iDim--)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Attempt to convert the RAT to an integer using System []CT
                        aplTmpSubLft = ConvertToInteger_SCT (aplTypeSubLft, lpMemSubLft, iDim, &bRet) - bQuadIO;

                        // Check for negative indices [-lpMemDimRht[iDim], -1]
                        if (SIGN_APLLONGEST (aplTmpSubLft)
                         && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
                            aplTmpSubLft += lpMemDimRht[iDim];

                        // Ensure the indices are within range
                        bErrIndex = (lpMemDimRht[iDim] <= aplTmpSubLft);
                        if (!bRet || bErrIndex)
                        {
                            bRet = FALSE;

                            break;
                        } // End IF

                        // Add into accumulator
                        aplLongestSubLft += aplTmpSubLft * aplWValSubLft;

                        // Shift the weighting value over
                        aplWValSubLft *= lpMemDimRht[iDim];
                    } // End FOR

                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbSubLft); lpMemHdrSubLft = NULL;

            // Check for error
            if (!bRet)
            {
                if (bErrIndex)
                    goto INDEX_EXIT;
                else
                    goto DOMAIN_EXIT;
            } // End IF
        } else
        {
            // The left arg is an immediate or simple value

            // Ensure that the right arg is a vector
            if (!IsVector (aplRankRht))
                goto LENGTH_EXIT;

            // Ensure that the left arg immediate value is numeric
            if (!IsImmNum (immTypeSubLft))
            {
                // If it's a global numeric, ...
                if (IsImmGlbNum (immTypeSubLft))
                {
                    LPVARARRAY_HEADER lpMemHdrSubLft = NULL;    // Ptr to left subitem header
                    LPVOID            lpMemSubLft;

                    if (GetPtrTypeDir (hGlbSubLft) EQ PTRTYPE_HGLOBAL)
                    {
                        // Lock the memory to get a ptr to it
                        lpMemHdrSubLft = MyGlobalLockVar (hGlbSubLft);

                        lpMemSubLft = VarArrayDataFmBase (lpMemHdrSubLft);
                    } else
                        lpMemSubLft = hGlbSubLft;

                    // Attempt to convert the left arg value to an integer
                    aplLongestSubLft =
                      ConvertToInteger_SCT (aplTypeLft,
                                            lpMemSubLft,
                                            0,
                                           &bRet);
                    if (GetPtrTypeDir (hGlbSubLft) EQ PTRTYPE_HGLOBAL)
                    {
                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbSubLft); lpMemHdrSubLft = NULL;
                    } // End IF

                    // Check for error
                    if (!bRet)
                        goto DOMAIN_EXIT;

                    // Convert the immediate type
                    immTypeSubLft = IMMTYPE_INT;
                } else
                    goto DOMAIN_EXIT;
            } // End IF

            // Ensure that the left arg immediate value can convert to an integer
            if (IsImmFlt (immTypeSubLft))
            {
                // Attempt to convert the float to an integer using System []CT
                aplLongestSubLft = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestSubLft, &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;
            } // End IF

            // Convert the index to origin-0
            aplLongestSubLft -= bQuadIO;

            // Check for negative indices [-aplNELMRht, -1]
            if (SIGN_APLLONGEST (aplLongestSubLft)
             && lpMemPTD->aplCurrentFEATURE[FEATURENDX_NEGINDICES])
                aplLongestSubLft += aplNELMRht;

            // Ensure that the index is within range
            // N.B.:  Because APLLONGEST is unsigned, we don't have to worry about negatives
            if (aplLongestSubLft >= aplNELMRht)
                goto INDEX_EXIT;
        } // End IF/ELSE

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = (LPVOID) lpMemDimRht = NULL;

        // Calculate the HC Dimension (1, 2, 4, 8)
        iHCDimRht = TranslateArrayTypeToHCDim (aplTypeRht);

        // Get the indexed value from the right arg
        GetNextValueGlb (hGlbRht,               // Right arg global memory handle
                         aplLongestSubLft,      // Index
                        &hGlbSubRht,            // Ptr to result as HGLOBAL (may be NULL if singleton)
                        &aplLongestSubRht,      // Ptr to result as singleton value
                        &immTypeSubRht);        // Ptr to result as singleton type
        // If the right arg item is a global value, ...
        if (hGlbSubRht NE NULL)
        {
            // If we're assigning a new value
            //   and this is the last element in the
            //   left arg, ...
            if (bArraySet
             && uLft EQ (aplNELMNstLft - 1))
            {
                LPVOID lpSet;           // Temp Ptr to Set value

                // Replace the <aplLongestSubLft> element in hGlbRht
                //   with <aplLongestSet> or <hGlbSet> depending upon <aplTypeSet>

                // Lock the memory to get a ptr to it
                lpMemHdrRht = MyGlobalLockVar (hGlbRht);

                // Skip over the header and dimensions to the data
                lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

                // If the set arg is immediate, ...
                if (hGlbSet EQ NULL)
                    lpSet = &aplLongestSet;
                else
                    lpSet = hGlbSet;

                // Split cases based upon the right arg storage type
                switch (aplTypeRht)
                {
                    case ARRAY_NESTED:
                    case ARRAY_HETERO:
                        // If the set arg is immediate, ...
                        if (hGlbSet EQ NULL)
                        {
                            ((LPAPLNESTED) lpMemRht)[aplLongestSubLft] =
                            lpSymTmp =
                              MakeSymEntry_EM (TranslateArrayTypeToImmType (aplTypeSet),    // Immediate type
                                              &aplLongestSet,                               // Ptr to immediate value
                                               lptkFunc);                                   // Ptr to function token
                            if (lpSymTmp EQ NULL)
                                goto ERROR_EXIT;
                        } else
                            ((LPAPLNESTED) lpMemRht)[aplLongestSubLft] =
                              CopySymGlbDir_PTB (hGlbSet);

                        // Free the old value
                        FreeResultGlobalVar (hGlbSubRht); hGlbSubRht = NULL;

                        break;

                    case ARRAY_RAT:
                    case ARRAY_HC2R:
                    case ARRAY_HC4R:
                    case ARRAY_HC8R:
                        // Promote the immediate set value to the right arg storage type
                        (*aTypeActPromote[aplTypeSet][aplTypeRht]) (lpSet, 0, &atRht);

                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            // Set the new value freeing the old one
                            mpq_set (&((LPAPLRAT) lpMemRht)[i + iHCDimRht * aplLongestSubLft], &atRht.aplHC8R.parts[i]);

                        // Free the old atSub
                        (*aTypeFree[aplTypeRht]) (&atRht, 0);

                        break;

                    case ARRAY_VFP:
                    case ARRAY_HC2V:
                    case ARRAY_HC8V:
                    case ARRAY_HC4V:
                        // Promote the immediate set value to the right arg storage type
                        (*aTypeActPromote[aplTypeSet][aplTypeRht]) (lpSet, 0, &atRht);

                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            // Set the new value freeing the old one
                            mpfr_copy (&((LPAPLVFP) lpMemRht)[i + iHCDimRht * aplLongestSubLft], &atRht.aplHC8V.parts[i]);

                        // Free the old atSub
                        (*aTypeFree[aplTypeRht]) (&atRht, 0);

                        break;

                    case ARRAY_HC2I:
                    case ARRAY_HC4I:
                    case ARRAY_HC8I:
                        // Promote the immediate set value to the right arg storage type
                        (*aTypeActPromote[aplTypeSet][aplTypeRht]) (lpSet, 0, &atRht);

                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            // Set the new value freeing the old one
                            ((LPAPLINT) lpMemRht)[i + iHCDimRht * aplLongestSubLft] = atRht.aplHC8I.parts[i];

                        // Free the old atSub
                        (*aTypeFree[aplTypeRht]) (&atRht, 0);

                        break;

                    case ARRAY_HC2F:
                    case ARRAY_HC4F:
                    case ARRAY_HC8F:
                        // Promote the immediate set value to the right arg storage type
                        (*aTypeActPromote[aplTypeSet][aplTypeRht]) (lpSet, 0, &atRht);

                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            // Set the new value freeing the old one
                            ((LPAPLFLOAT) lpMemRht)[i + iHCDimRht * aplLongestSubLft] = atRht.aplHC8F.parts[i];

                        // Free the old atSub
                        (*aTypeFree[aplTypeRht]) (&atRht, 0);

                        break;

                    case ARRAY_BOOL:
                    case ARRAY_INT:
                    case ARRAY_FLOAT:
                    case ARRAY_CHAR:
                    case ARRAY_APA:
                    defstop
                        break;
                } // End SWITCH

                // We no longer need this ptr
                MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

                // Return pseudo-value
                lpYYRes = PTR_SUCCESS;
            } else
            {
                // In case we need to know where this item
                //   came from, save its index & global memory handle
                aplLongestPrvLft = aplLongestSubLft;
                hGlbPrvRht = hGlbRht;

                // Copy as new right arg global memory handle
                hGlbRht = hGlbSubRht;
            } // End IF/ELSE
        } else
        {
            // The right arg item is an immediate or simple value

            // If the left arg is nested, ...
            // Validate that the remaining left arg items are {zilde}s
            if (IsNested (aplTypeLft))
            {
                if (!PrimFnDydRightShoeGlbImm_EM (aplNELMLft,           // Left arg NELM
                                                  lpMemLft,             // Ptr to left arg global memory
                                                  uLft + 1,             // Left arg starting index
                                                  lptkFunc))            // Ptr to function token
                    goto ERROR_EXIT;
            } else
            if (uLft NE (aplNELMLft - 1))
                goto LENGTH_EXIT;

            // If we're assigning a new value, ...
            if (bArraySet)
            {
                // If the right arg should be promoted, ...
                if (QueryPromote (aplTypeRht, aplTypeSet, &aplTypePro))
                {
                    LPVARARRAY_HEADER lpMemHdrPrvRht = NULL;
                    LPAPLNESTED       lpMemPrvRht;
                    APLRANK           aplRankPrvRht;

                    // Promote the right arg
                    if (!TypePromoteGlb_EM (&hGlbRht, aplTypePro, lptkFunc))
                        goto ERROR_EXIT;

                    // Save the new type
                    aplTypeRht = aplTypePro;

                    // Lock the memory to get a ptr to it
                    lpMemHdrPrvRht = MyGlobalLockVar (hGlbPrvRht);

#define lpHeader        lpMemHdrPrvRht
                    // Get the rank of the previous right arg global
                    aplRankPrvRht = lpHeader->Rank;
#undef  lpHeader
                    // Skip over the header and dimensions to the data
                    lpMemPrvRht = VarArrayDataFmBase (lpMemHdrPrvRht);

                    // Save the new hGlbRht in the array from which we got it
                    //   (i.e., the <aplLongestPrvLft> entry in <hGlbPrvRht>)
                    lpMemPrvRht[aplLongestPrvLft] = MakePtrTypeGlb (hGlbRht);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbPrvRht); lpMemHdrPrvRht = NULL;
                } // End IF

                // Lock the memory to get a ptr to it
                lpMemHdrRht = MyGlobalLockVar (hGlbRht);

                // Get the array rank
#define lpHeader        lpMemHdrRht
                aplRankRht = lpHeader->Rank;
#undef  lpHeader
                // Skip over the header and dimensions to the data
                lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

                // Replace the <aplLongestSubLft> element in hGlbRht
                //   with <aplLongestSet> or <hGlbSet> depending upon <aplTypeRht>
                if (!ArrayIndexReplace_EM (aplTypeRht,      // Right arg storage type
                                           lpMemRht,        // Ptr to right arg global memory
                                           aplLongestSubLft,// Index into right arg
                                           aplTypeSet,      // Set arg storage type
                                           aplLongestSet,   // Set arg immediate value
                                           hGlbSet,         // Set arg global memory handle
                                           lptkFunc))       // Ptr to function token
                    goto ERROR_EXIT;
                // We no longer need this ptr
                MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

                // Return pseudo-value
                lpYYRes = PTR_SUCCESS;
            } else
            {
                // Allocate a new YYRes
                lpYYRes = YYAlloc ();

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkFlags.ImmType   = immTypeSubRht;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkLongest  = aplLongestSubRht;
                lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
            } // End IF/ELSE

            goto NORMAL_EXIT;
        } // End IF/ELSE
    } // End IF/ELSE/FOR

    // If we're not assigning a new value, ...
    if (!bArraySet)
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbNumDir_PTB (hGlbRht, aplTypeRht, lptkFunc);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

INDEX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnDydRightShoeGlbGlb_EM_YY


//***************************************************************************
//  End of File: pf_rshoe.c
//***************************************************************************
