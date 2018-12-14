//***************************************************************************
//  NARS2000 -- Primitive Function -- Epsilon
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


// Define the following var in order to grade the secondary
//   argument so as to catch duplicate values.
//   It's probably not worth the effort, so this option is
//     not recommended.
////#define GRADE2ND


//***************************************************************************
//  $PrimFnEpsilon_EM_YY
//
//  Primitive function for monadic and dyadic Epsilon ("enlist" and "member of")
//***************************************************************************

LPPL_YYSTYPE PrimFnEpsilon_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharEpsilon (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonEpsilon_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydEpsilon_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnEpsilon_EM_YY


//***************************************************************************
//  $PrimProtoFnEpsilon_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Epsilon
//***************************************************************************

LPPL_YYSTYPE PrimProtoFnEpsilon_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnEpsilon_EM_YY,// Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnEpsilon_EM_YY


//***************************************************************************
//  $PrimFnMonEpsilon_EM_YY
//
//  Primitive function for monadic Epsilon ("enlist")
//***************************************************************************

LPPL_YYSTYPE PrimFnMonEpsilon_EM_YY
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

    //***************************************************************
    // Loop through the right arg counting all of the simple scalars
    //   and saving their storage type.  The result will be either
    //   simple homogeneous or simple heterogeneous.
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
                Assert (IsGlbTypeVarDir_PTB (lptkRhtArg->tkData.tkSym->stData.stGlbData));

                return PrimFnMonEpsilonGlb_EM_YY
                       (lptkRhtArg->tkData.tkSym->stData.stGlbData, // HGLOBAL
                        lptkFunc);                                  // Ptr to function token
            } // End IF

            // Handle the immediate case
            return PrimFnMonEpsilonImm_EM_YY
                   (TranslateImmTypeToArrayType (lptkRhtArg->tkData.tkSym->stFlags.ImmType),    // Immediate type
                    lptkRhtArg->tkData.tkSym->stData.stLongest,                                 // Immediate value
                    lptkFunc);                                                                  // Ptr to function token
        case TKT_VARIMMED:
            return PrimFnMonEpsilonImm_EM_YY
                   (TranslateImmTypeToArrayType (lptkRhtArg->tkFlags.ImmType),  // Immediate type
                    lptkRhtArg->tkData.tkLongest,                               // Immediate value
                    lptkFunc);                                                  // Ptr to function token
        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (lptkRhtArg->tkData.tkGlbData));

            return PrimFnMonEpsilonGlb_EM_YY
                   (lptkRhtArg->tkData.tkGlbData,                   // HGLOBAL
                    lptkFunc);                                      // Ptr to function token
        defstop
            return NULL;
    } // End SWITCH
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnMonEpsilon_EM_YY


//***************************************************************************
//  $PrimFnMonEpsilonImm_EM_YY
//
//  Monadic Epsilon on an immediate value
//***************************************************************************

LPPL_YYSTYPE PrimFnMonEpsilonImm_EM_YY
    (ARRAY_TYPES aplTypeRes,    // Right arg storage type
     APLLONGEST  aplLongest,    // Right arg immediate value
     LPTOKEN     lptkFunc)      // Ptr to function token

{
    APLUINT      ByteRes;       // # bytes in the result
    HGLOBAL      hGlbRes;       // Result global memory handle
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPVOID       lpMemRes;      // Ptr to result global memory
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result

    // Calculate space needed for the result (a one-element vector)
    ByteRes = CalcArraySize (aplTypeRes, 1, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = 1;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemHdrRes) = 1;

    // Skip over the header and dimension to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:
            // Save the Boolean value
            *((LPAPLBOOL)  lpMemRes) = (APLBOOL)  aplLongest;

            break;

        case ARRAY_INT:
            // Save the Integer value
            *((LPAPLINT)   lpMemRes) = (APLINT)   aplLongest;

            break;

        case ARRAY_FLOAT:
            // Save the Float value
            *((LPAPLFLOAT) lpMemRes) = *(LPAPLFLOAT) &aplLongest;

            break;

        case ARRAY_CHAR:
            // Save the Char value
            *((LPAPLCHAR)  lpMemRes) = (APLCHAR)  aplLongest;

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnMonEpsilonImm_EM_YY


//***************************************************************************
//  $PrimFnMonEpsilonGlb_EM_YY
//
//  Monadic Epsilon on a global memory object:
//     Count the # scalars and the common storage type
//     Allocate storage
//     Copy the data to the result
//***************************************************************************

LPPL_YYSTYPE PrimFnMonEpsilonGlb_EM_YY
    (HGLOBAL hGlbRht,                       // Right arg global memory handle
     LPTOKEN lptkFunc)                      // Ptr to function token

{
    APLSTYPE          aplTypeRes,           // Result storage type
                      aplTypePro;           // Prototype ...
    APLNELM           aplNELMRes;           // # elements in the result
    HGLOBAL           hGlbRes = NULL;       // Result global memory handle
    LPVOID            lpMemRes;             // Ptr to result global memory
    APLUINT           ByteRes;              // # bytes in the result
    UINT              uBitMask = 0x01,      // Bit mask for marching through Booleans
                      uBitIndex = 0;        // Bit index ...
    UBOOL             bRet = TRUE;          // TRUE iff result is valid
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...

    // Traverse the array counting the # simple scalars
    //   and keeping track of the common storage type --
    //   it'll be simple homogeneous or heterogeneous.
    aplTypeRes = ARRAY_INIT;    // Initialize storage type
    aplNELMRes = 0;             // ...        count
    PrimFnMonEpsilonGlbCount (hGlbRht,      // Right arg global memory handle
                             &aplTypeRes,   // Ptr to result storage type
                             &aplNELMRes,   // Ptr to result NELM
                             &aplTypePro);  // Ptr to prototype storage type
    // Handle empty result
    if (IsEmpty (aplNELMRes))
        aplTypeRes = aplTypePro;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLockVar (hGlbRht);

#define lpHeader        lpMemHdrRes
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->bSelSpec   = lpMemHdrRht->bSelSpec;
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

    // Save the dimension in the result
    *VarArrayBaseToDim (lpMemHdrRes) = aplNELMRes;

    // Skip over the header and dimension to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Copy the data from the right arg to the result
    if (!PrimFnMonEpsilonGlbCopy_EM (aplTypeRes,
                                    &lpMemRes,
                                    &uBitIndex,
                                     hGlbRht,
                                     lptkFunc))
        goto ERROR_EXIT;
    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken, FALSE);

    return lpYYRes;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
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
} // End PrimFnMonEpsilonGlb_EM_YY


//***************************************************************************
//  $PrimFnMonEpsilonGlbCount
//
//  Count the # simple scalars in the global memory object
//***************************************************************************

void PrimFnMonEpsilonGlbCount
    (HGLOBAL    hGlbRht,            // Right arg global memory handle
     LPAPLSTYPE lpaplTypeRes,       // Ptr to result storage type
     LPAPLNELM  lpaplNELMRes,       // Ptr to result NELM
     LPAPLSTYPE lpaplTypePro)                       // Ptr to prototype storage type

{
    LPVARARRAY_HEADER lpMemHdrRht = NULL;   // Ptr to right arg header
    LPVOID            lpMemRht;             // Ptr to right arg global memory
    APLSTYPE          aplTypeRht;           // Right arg storage type
    APLNELM           aplNELMRht;           // ...       NELM
    APLRANK           aplRankRht;           // ...       rank
    APLUINT           uRht;                 // Loop counter

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLockVar (hGlbRht);

#define lpHeader    lpMemHdrRht
    // Get the Array Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_CHAR:
        case ARRAY_APA:
        case ARRAY_HETERO:
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
        case ARRAY_BA1F:
        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
            // Empty args don't contribute to the result,
            //   so we don't want to risk changing the storage
            //   type to HETERO by running through aTypePromote.
            if (!IsEmpty (aplNELMRht))
            {
                *lpaplTypeRes = aTypePromote[*lpaplTypeRes][aplTypeRht];
                (*lpaplNELMRes) += aplNELMRht;
            } else
            // If it's an empty APA, ...
            if (IsSimpleAPA (aplTypeRht))
                // Call it an integer so we don't fail later on
                *lpaplTypePro = ARRAY_INT;
            else
                *lpaplTypePro = aplTypeRht;
            break;

        case ARRAY_NESTED:
            // Initialize the prototype storage type
            //   in case the nested array is empty
            *lpaplTypePro = ARRAY_BOOL;

            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

            // Loop through the elements
            for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
            // Split cases based upon the ptr type
            switch (GetPtrTypeInd (lpMemRht))
            {
                case PTRTYPE_STCONST:
                    // Set the prototype and result storage type
                    *lpaplTypePro = TranslateImmTypeToArrayType ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType);
                    *lpaplTypeRes = aTypePromote[*lpaplTypeRes][*lpaplTypePro];

                    // Count in another item
                    (*lpaplNELMRes)++;

                    break;

                case PTRTYPE_HGLOBAL:
                    // It's a valid HGLOBAL variable array
                    Assert (IsGlbTypeVarInd_PTB (lpMemRht));

                    PrimFnMonEpsilonGlbCount (ClrPtrTypeInd (lpMemRht), // Right arg global memory handle
                                              lpaplTypeRes,             // Ptr to result storage type
                                              lpaplNELMRes,             // Ptr to result NELM
                                              lpaplTypePro);            // Ptr to prototype storage type
                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
} // End PrimFnMonEpsilonGlbCount


//***************************************************************************
//  $PrimFnMonEpsilonGlbCopy_EM
//
//  Copy the data from the right arg to the result
//***************************************************************************

UBOOL PrimFnMonEpsilonGlbCopy_EM
    (APLSTYPE aplTypeRes,           // Result type
     LPVOID  *lplpMemRes,           // Ptr to ptr to result memory
     LPUINT   lpuBitIndex,          // Ptr to uBitIndex
     HGLOBAL  hGlbRht,              // Handle to right arg
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    APLSTYPE      aplTypeRht,       // Right arg storage type
                  aplTypeSub;       // Item      ...
    APLNELM       aplNELMRht;       // Right arg NELM
    APLRANK       aplRankRht;       // Right arg rank
    LPVARARRAY_HEADER lpMemHdrRht = NULL;   // Ptr to right arg header
    LPVOID        lpMemRht,         // Ptr to right arg global memory
                  lpMemRes;         // ...    result    ...
    UINT          uBitMask = BIT0;  // Bit mask for marching through Booleans
    APLUINT       uRht;             // Right arg loop counter
    APLUINT       ByteRes;          // # bytes in the result
    APLINT        apaOffRht,        // Right arg APA offset
                  apaMulRht;        // ...           multiplier
    APLLONGEST    aplVal;           // Temporary value
    UBOOL         bRet = FALSE;     // TRUE iff the result is valid
    LPSYMENTRY    lpSymTmp;         // Ptr to temporary LPSYMENTRY
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;     // Ptr to Ctrl-Break flag
    ALLTYPES      atRht = {0};      // Right arg item as ALLTYPES
    int           iSizeofRes,       // Sizeof an item
                  iHCDimRes;        // HC Dimension (1, 2, 4, 8)

    // Calculate the sizeof an item
    iSizeofRes = TranslateArrayTypeToSizeof (aplTypeRes);

    // Get the HC Dimension (1, 2, 4, 8)
    iHCDimRes = TranslateArrayTypeToHCDim (aplTypeRes);

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Lock the memory to get a ptr to it
    lpMemHdrRht = MyGlobalLockVar (hGlbRht);

#define lpHeader    lpMemHdrRht
    // Get the Array Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // Skip past the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // If the right arg is an APA, ...
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // If the item is non-empty, ...
    if (!IsEmpty (aplNELMRht))
    // Split cases based upon the result's storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:                            // Res = BOOL  , Rht = BOOL/NESTED
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:                    // Res = BOOL  , Rht = BOOL
                    // The result's and right arg's storage type are the same,
                    //   so just copy the data if it's aligned
                    if (*lpuBitIndex EQ 0)
                    {
                        // Calculate space needd for the result
                        ByteRes = sizeof (APLBOOL) * RoundUpBitsToBytes (aplNELMRht);

                        // Check for overflow
                        if (ByteRes NE (APLU3264) ByteRes)
                            goto WSFULL_EXIT;

                        CopyMemory (*lplpMemRes, lpMemRht, (APLU3264) ByteRes);

                        // Unless the right arg is a multiple of NBIB bits,
                        //   the following += is one too large
                        ((LPAPLBOOL) *lplpMemRes) += RoundUpBitsToBytes (aplNELMRht);

                        // Calculate the next bit index
                        *lpuBitIndex = (UINT) (aplNELMRht % NBIB);

                        // Account for non-NBIB # bits in right arg
                        if (*lpuBitIndex NE 0)
                            ((LPAPLBOOL) *lplpMemRes)--;
                    } else
                    {
                        // Loop through the elements
                        for (uRht = 0; uRht < aplNELMRht; uRht++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (lpbCtrlBreak))
                                goto ERROR_EXIT;

                            *(*(LPAPLBOOL *) lplpMemRes) |= ((uBitMask & *((LPAPLBOOL) lpMemRht)) ? TRUE : FALSE) << *lpuBitIndex;

                            // Check for end-of-byte
                            if (++*lpuBitIndex EQ NBIB)
                            {
                                *lpuBitIndex = 0;               // Start over
                                (*(LPAPLBOOL *) lplpMemRes)++;  // Skip to next byte
                            } // End IF

                            // Shift over the bit mask
                            uBitMask <<= 1;

                            // Check for end-of-byte
                            if (uBitMask EQ END_OF_BYTE)
                            {
                                uBitMask = BIT0;                // Start over
                                ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } // End IF

                    break;

                case ARRAY_NESTED:                  // Res = BOOL  , Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Split cases based upon the ptr type
                        switch (GetPtrTypeInd (lpMemRht))
                        {
                            case PTRTYPE_STCONST:       // Res = BOOL  , Rht = NESTED:BOOL/INT/FLOAT/CHAR
                                // Split cases based upon the right arg's immediate type
                                switch ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:  // Res = BOOL  , Rht = NESTED:BOOL
                                        *(*(LPAPLBOOL *) lplpMemRes) |= ((*(LPAPLHETERO) lpMemRht)->stData.stBoolean) << *lpuBitIndex;

                                        // Check for end-of-byte
                                        if (++*lpuBitIndex EQ NBIB)
                                        {
                                            *lpuBitIndex = 0;               // Start over
                                            (*(LPAPLBOOL *) lplpMemRes)++;  // Skip to next byte
                                        } // End IF

                                        break;

                                    case IMMTYPE_INT:   // Res = BOOL  , Rht = NESTED:INT    (Can't happen w/Boolean result)
                                    case IMMTYPE_FLOAT: // Res = BOOL  , Rht = NESTED:FLOAT  (...)
                                    case IMMTYPE_CHAR:  // Res = BOOL  , Rht = NESTED:CHAR   (...)
                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:       // Res = BOOL  , Rht = NESTED:NESTED
                                // It's a valid HGLOBAL variable array
                                Assert (IsGlbTypeVarInd_PTB (lpMemRht));

                                // Copy the data to the result
                                if (!PrimFnMonEpsilonGlbCopy_EM (aplTypeRes,
                                                                 lplpMemRes,
                                                                 lpuBitIndex,
                                                                 ClrPtrTypeInd (lpMemRht),
                                                                 lptkFunc))
                                    goto ERROR_EXIT;
                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                case ARRAY_INT:                     // Res = BOOL  , Rht = INT    (Can't happen w/Boolean result)
                case ARRAY_FLOAT:                   // Res = BOOL  , Rht = FLOAT  (...)
                case ARRAY_CHAR:                    // Res = BOOL  , Rht = CHAR   (...)
                case ARRAY_APA:                     // Res = BOOL  , Rht = APA    (...)
                case ARRAY_HETERO:                  // Res = BOOL  , Rht = HETERO (...)
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_CHAR:                            // Res = CHAR  , Rht = CHAR/NESTED
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_CHAR:                    // Res = CHAR  , Rht = CHAR
                    // The result's and right arg's storage type are the same,
                    //   so just copy the data
                    // Calculate space needed for the result
                    ByteRes = sizeof (APLCHAR) * aplNELMRht;

                    // Check for overflow
                    if (ByteRes NE (APLU3264) ByteRes)
                        goto WSFULL_EXIT;

                    CopyMemory (*lplpMemRes, lpMemRht, (APLU3264) ByteRes);
                    ((LPAPLCHAR) *lplpMemRes) += aplNELMRht;

                    break;

                case ARRAY_NESTED:                  // Res = CHAR  , Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Split cases based upon the ptr type
                        switch (GetPtrTypeInd (lpMemRht))
                        {
                            case PTRTYPE_STCONST:       // Res = CHAR  , Rht = NESTED:CHAR
                                // Split cases based upon the right arg's immediate type
                                switch ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType)
                                {
                                    case IMMTYPE_CHAR:  // Res = CHAR  , Rht = NESTED:CHAR
                                        *(*(LPAPLCHAR *) lplpMemRes)++ = (*(LPAPLHETERO) lpMemRht)->stData.stChar;

                                        break;

                                    case IMMTYPE_BOOL:  // Res = CHAR  , Rht = NESTED:BOOL    (Can't happen w/Char result)
                                    case IMMTYPE_INT:   // Res = CHAR  , Rht = NESTED:INT     (...)
                                    case IMMTYPE_FLOAT: // Res = CHAR  , Rht = NESTED:FLOAT   (...)
                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:       // Res = CHAR  , Rht = NESTED:NESTED
                                // It's a valid HGLOBAL variable array
                                Assert (IsGlbTypeVarInd_PTB (lpMemRht));

                                // Copy the data to the result
                                if (!PrimFnMonEpsilonGlbCopy_EM (aplTypeRes,
                                                                 lplpMemRes,
                                                                 lpuBitIndex,
                                                                 ClrPtrTypeInd (lpMemRht),
                                                                 lptkFunc))
                                    goto ERROR_EXIT;
                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                case ARRAY_BOOL:                    // Res = CHAR  , Rht = BOOL   (Can't happen w/Char result)
                case ARRAY_INT:                     // Res = CHAR  , Rht = INT    (...)
                case ARRAY_APA:                     // Res = CHAR  , Rht = APA    (...)
                case ARRAY_HETERO:                  // Res = CHAR  , Rht = HETERO (...)
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_HETERO:                          // Res = HETERO, Rht = BOOL/INT/FLOAT/APA/CHAR/HETERO/NESTED/RAT/VFP
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:                    // Res = HETERO, Rht = BOOL
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        aplVal = (uBitMask & *((LPAPLBOOL) lpMemRht)) ? TRUE : FALSE;
                        *(*(LPAPLHETERO *) lplpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_BOOL,            // Immediate type
                                          &aplVal,                  // Ptr to immediate value
                                           lptkFunc);               // Ptr to function token
                        if (lpSymTmp EQ NULL)
                            goto ERROR_EXIT;
                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;         // Start over
                            ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:                     // Res = HETERO, Rht = INT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        aplVal = *((LPAPLINT) lpMemRht)++;
                        *(*(LPAPLHETERO *) lplpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_INT,             // Immediate type
                                          &aplVal,                  // Ptr to immediate value
                                           lptkFunc);               // Ptr to function token
                        if (lpSymTmp EQ NULL)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_FLOAT:                   // Res = HETERO, Rht = FLOAT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        aplVal = *(LPAPLLONGEST) ((LPAPLFLOAT) lpMemRht)++;
                        *(*(LPAPLHETERO *) lplpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_FLOAT,           // Immediate type
                                          &aplVal,                  // Ptr to immediate value
                                           lptkFunc);               // Ptr to function token
                        if (lpSymTmp EQ NULL)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_APA:                     // Res = HETERO, Rht = APA
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        aplVal = apaOffRht + apaMulRht * uRht;
                        *(*(LPAPLHETERO *) lplpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_INT,             // Immediate type
                                          &aplVal,                  // Ptr to immediate value
                                           lptkFunc);               // Ptr to function token
                        if (lpSymTmp EQ NULL)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_CHAR:                    // Res = HETERO, Rht = CHAR
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        aplVal = *((LPAPLCHAR) lpMemRht)++;
                        *(*(LPAPLHETERO *) lplpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_CHAR,            // Immediate type
                                          &aplVal,                  // Ptr to immediate value
                                           lptkFunc);               // Ptr to function token
                        if (lpSymTmp EQ NULL)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_HETERO:                  // Res = HETERO, Rht = HETERO
                    // The result's and right arg's storage type are the same,
                    //   so just copy the data
                    // Calculate space needed for the result
                    ByteRes = sizeof (APLHETERO) * aplNELMRht;

                    // Check for overflow
                    if (ByteRes NE (APLU3264) ByteRes)
                        goto WSFULL_EXIT;

                    CopyMemory (*lplpMemRes, lpMemRht, (APLU3264) ByteRes);
                    ((LPAPLHETERO) *lplpMemRes) += aplNELMRht;

                    break;

                case ARRAY_NESTED:                  // Res = HETERO, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Split cases based upon the ptr type
                        switch (GetPtrTypeInd (lpMemRht))
                        {
                            case PTRTYPE_STCONST:       // Res = HETERO, Rht = NESTED:BOOL/INT/FLOAT/CHAR
                                // Split cases based upon the right arg's immediate type
                                switch ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:  // Res = HETERO, Rht = NESTED:BOOL
                                        aplVal = (*(LPAPLHETERO) lpMemRht)->stData.stBoolean;
                                        *(*(LPAPLNESTED *) lplpMemRes)++ =
                                        lpSymTmp =
                                          MakeSymEntry_EM (IMMTYPE_BOOL,    // Immediate type
                                                          &aplVal,          // Ptr to immediate value
                                                           lptkFunc);       // Ptr to function token
                                        if (lpSymTmp EQ NULL)
                                            goto ERROR_EXIT;
                                        break;

                                    case IMMTYPE_INT:   // Res = HETERO, Rht = NESTED:INT
                                        aplVal = (*(LPAPLHETERO) lpMemRht)->stData.stInteger;
                                        *(*(LPAPLNESTED *) lplpMemRes)++ =
                                        lpSymTmp =
                                          MakeSymEntry_EM (IMMTYPE_INT,     // Immediate type
                                                          &aplVal,          // Ptr to immediate value
                                                           lptkFunc);       // Ptr to function token
                                        if (lpSymTmp EQ NULL)
                                            goto ERROR_EXIT;
                                        break;

                                    case IMMTYPE_FLOAT: // Res = HETERO, Rht = NESTED:FLOAT
                                        aplVal = *(LPAPLLONGEST) &(*(LPAPLHETERO) lpMemRht)->stData.stFloat;
                                        *(*(LPAPLNESTED *) lplpMemRes)++ =
                                        lpSymTmp =
                                          MakeSymEntry_EM (IMMTYPE_FLOAT,   // Immediate type
                                                          &aplVal,          // Ptr to immediate value
                                                           lptkFunc);       // Ptr to function token
                                        if (lpSymTmp EQ NULL)
                                            goto ERROR_EXIT;
                                        break;


                                    case IMMTYPE_CHAR:  // Res = HETERO, Rht = NESTED:CHAR
                                        aplVal = (*(LPAPLHETERO) lpMemRht)->stData.stChar;
                                        *(*(LPAPLNESTED *) lplpMemRes)++ =
                                        lpSymTmp =
                                          MakeSymEntry_EM (IMMTYPE_CHAR,    // Immediate type
                                                          &aplVal,          // Ptr to immediate value
                                                          lptkFunc);        // Ptr to function token
                                        if (lpSymTmp EQ NULL)
                                            goto ERROR_EXIT;
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case PTRTYPE_HGLOBAL:       // Res = HETERO, Rht = NESTED:NESTED
                                // It's a valid HGLOBAL variable array
                                Assert (IsGlbTypeVarInd_PTB (lpMemRht));

                                // Copy the data to the result
                                if (!PrimFnMonEpsilonGlbCopy_EM (aplTypeRes,
                                                                 lplpMemRes,
                                                                 lpuBitIndex,
                                                                 ClrPtrTypeInd (lpMemRht),
                                                                 lptkFunc))
                                    goto ERROR_EXIT;
                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    break;

                case ARRAY_RAT:                     // Res = HETERO, Rht = RAT
                case ARRAY_VFP:                     // Res = HETERO, Rht = VFP
                case ARRAY_ARB:                     // Res = HETERO, Rht = ARB
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLRAT) lpMemRht)++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        *(*(LPAPLHETERO *) lplpMemRes)++ =
                        lpSymTmp =
                          MakeGlbEntry_EM (aplTypeRht,              // Entry type
                                           lpMemRht,                // Ptr to the value
                                           TRUE,                    // TRUE iff we should initialize the target first
                                           lptkFunc);               // Ptr to function token
                        if (lpSymTmp EQ NULL)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_INT:                             // Res = INT   , Rht = BOOL/INT/APA/NESTED
        case ARRAY_HC2I:                            // Res = HC2I
        case ARRAY_HC4I:                            // Res = HC4I
        case ARRAY_HC8I:                            // Res = HC8I
        case ARRAY_FLOAT:                           // Res = FLOAT , Rht = BOOL/INT/FLOAT/APA/NESTED
        case ARRAY_HC2F:                            // Res = HC2F
        case ARRAY_HC4F:                            // Res = HC4F
        case ARRAY_HC8F:                            // Res = HC8F
        case ARRAY_RAT:                             // Res = RAT   , Rht = BOOL/INT/APA/NESTED/RAT
        case ARRAY_HC2R:                            // Res = HC2R
        case ARRAY_HC4R:                            // Res = HC4R
        case ARRAY_HC8R:                            // Res = HC8R
        case ARRAY_VFP:                             // Res = VFP   , Rht = BOOL/INT/FLOAT/APA/NESTED/RAT/VFP
        case ARRAY_HC2V:                            // Res = HC2V
        case ARRAY_HC4V:                            // Res = HC4V
        case ARRAY_HC8V:                            // Res = HC8V
        case ARRAY_BA1F:                            // Res = BA1F
        case ARRAY_BA2F:                            // Res = BA2F
        case ARRAY_BA4F:                            // Res = BA4F
        case ARRAY_BA8F:                            // Res = BA8F
            // Get a ptr to the result's global memory
            lpMemRes = *lplpMemRes;

            // Loop through the elements
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Split cases based upon the right arg storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:                    // Res = HCxX  , Rht = BOOL
                    case ARRAY_INT:                     // Res = HCxX  , Rht = INT
                    case ARRAY_APA:                     // Res = HCxX  , Rht = APA
                    case ARRAY_HC2I:                    // Res = HCxX  , Rht = HC2I
                    case ARRAY_HC4I:                    // Res = HCxX  , Rht = HC4I
                    case ARRAY_HC8I:                    // Res = HCxX  , Rht = HC8I
                    case ARRAY_FLOAT:                   // Res = HCxX  , Rht = FLOAT
                    case ARRAY_HC2F:                    // Res = HCxX  , Rht = HC2F
                    case ARRAY_HC4F:                    // Res = HCxX  , Rht = HC4F
                    case ARRAY_HC8F:                    // Res = HCxX  , Rht = HC8F
                    case ARRAY_RAT:                     // res = HCxX  , Rht = RAT
                    case ARRAY_HC2R:                    // Res = HCxX  , Rht = HC2R
                    case ARRAY_HC4R:                    // Res = HCxX  , Rht = HC4R
                    case ARRAY_HC8R:                    // Res = HCxX  , Rht = HC8R
                    case ARRAY_VFP:                     // res = HCxX  , Rht = VFP
                    case ARRAY_HC2V:                    // Res = HCxX  , Rht = HC2V
                    case ARRAY_HC4V:                    // Res = HCxX  , Rht = HC4V
                    case ARRAY_HC8V:                    // Res = HCxX  , Rht = HC8V
                    case ARRAY_BA1F:                    // Res = HCxX  , Rht = BA1F
                    case ARRAY_BA2F:                    // Res = HCxX  , Rht = BA2F
                    case ARRAY_BA4F:                    // Res = HCxX  , Rht = BA4F
                    case ARRAY_BA8F:                    // Res = HCxX  , Rht = BA8F
                        // Convert the right arg item to the result type
                        (*aTypeActPromote[aplTypeRht][aplTypeRes]) (lpMemRht, uRht, &atRht);

                        // Save in the result
                        CopyMemory (lpMemRes, &atRht.aplLongest, iSizeofRes);

                        // Skip over the item
                        ((LPBYTE) lpMemRes) += iSizeofRes;

                        // Zero the memory in case we use it again
                        ZeroMemory (&atRht, sizeof (atRht));

                        break;

                    case ARRAY_NESTED:                  // Res = HCxX  , Rht = NESTED
                        // Loop through the elements
                        for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Split cases based upon the ptr type
                            switch (GetPtrTypeInd (lpMemRht))
                            {
                                case PTRTYPE_STCONST:       // Res = VFP   , Rht = NESTED:BOOL/INT/FLOAT
                                    // Get the storage type of the item
                                    aplTypeSub = TranslateImmTypeToArrayType ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType);

                                    // Convert the right arg item to the result type
                                    (*aTypeActPromote[aplTypeSub][aplTypeRes]) (&(*(LPAPLHETERO) lpMemRht)->stData.stLongest,
                                                                                 0,
                                                                                &atRht);
                                    // Save in the result
                                    CopyMemory (lpMemRes, &atRht.aplLongest, iSizeofRes);

                                    // Skip over the item
                                    ((LPBYTE) lpMemRes) += iSizeofRes;

                                    // Zero the memory in case we use it again
                                    ZeroMemory (&atRht, sizeof (atRht));

                                    break;

                                case PTRTYPE_HGLOBAL:       // Res = VFP   , Rht = NESTED:NESTED
                                    // It's a valid HGLOBAL variable array
                                    Assert (IsGlbTypeVarInd_PTB (lpMemRht));

                                    // Copy the data to the result
                                    if (!PrimFnMonEpsilonGlbCopy_EM (aplTypeRes,
                                                                    &lpMemRes,
                                                                     lpuBitIndex,
                                                                     ClrPtrTypeInd (lpMemRht),
                                                                     lptkFunc))
                                        goto ERROR_EXIT;
                                    break;

                                defstop
                                    break;
                            } // End SWITCH
                        } // End FOR

                        break;

                    case ARRAY_CHAR:                    // Res = HCxX  , Rht = CHAR   (Can't happen w/HCxX result)
                    case ARRAY_HETERO:                  // Res = HCxX  , Rht = HETERO (...)
                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            // Save back for caller
            *lplpMemRes = lpMemRes;

            break;

        case ARRAY_APA:                             // Res = APA    (Can't happen w/Enlist)
        case ARRAY_NESTED:                          // Res = NESTED (...)
        defstop
            break;
    } // End SWITCH

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

    return bRet;
} // End PrimFnMonEpsilonGlbCopy_EM


//***************************************************************************
//  $PrimFnDydEpsilon_EM_YY
//
//  Primitive function for dyadic Epsilon ("member of")
//***************************************************************************

LPPL_YYSTYPE PrimFnDydEpsilon_EM_YY
    (LPTOKEN lptkLftArg,                    // Ptr to left arg token
     LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis)                      // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht,           // Right ...
                      aplTypeCom;           // Common var storage type
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht;           // Right ...
    APLRANK           aplRankLft,           // Left arg rank
                      aplRankRht;           // Right ...
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL,       // Right ...
                      hGlbRes = NULL;       // Result   ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg header
                      lpMemHdrRht = NULL,   // ...    right ...
                      lpMemHdrRes = NULL;   // ...    result   ...
    LPAPLDIM          lpMemDimLft,          // Ptr to left  arg dimensions
                      lpMemDimRes;          // ...    result    ...
    LPVOID            lpMemLft,             // ...    left arg global memory
                      lpMemRht;             // ...    right ...
    LPAPLBOOL         lpMemRes;             // ...    result   ...
    APLUINT           ByteRes;              // # bytes in the result
    APLLONGEST        aplLongestLft,        // Left arg immediate value
                      aplLongestRht;        // Right ...
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    //***************************************************************
    // Calculate space needed for the result
    //***************************************************************
    ByteRes = CalcArraySize (ARRAY_BOOL, aplNELMLft, aplRankLft);

    //***************************************************************
    // Check for overflow
    //***************************************************************
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
    lpHeader->ArrType    = ARRAY_BOOL;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMLft;
    lpHeader->Rank       = aplRankLft;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Fill in the result's dimension
    if (lpMemHdrLft NE NULL)
    {
        // Skip over the header to the dimensions
        lpMemDimLft = VarArrayBaseToDim (lpMemHdrLft);

        // Copy the left arg dimensions to the result
        CopyMemory (lpMemDimRes, lpMemDimLft, (APLU3264) aplRankLft * sizeof (APLDIM));

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    } else
        // Point to the left arg immediate value
        lpMemLft = &aplLongestLft;

    if (lpMemHdrRht NE NULL)
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        // Point to the right arg immediate value
        lpMemRht = &aplLongestRht;

    // Calculate the common var storage type
    aplTypeCom = aTypePromote[aplTypeLft][aplTypeRht];

    // If the left arg is non-empty, and
    //    the right arg is non-empty, and
    //    the left and right args are not different types (numeric vs. char), ...
    if (!(IsEmpty (aplNELMLft)
       || IsEmpty (aplNELMRht)
       || (IsNumeric (aplTypeLft) && IsSimpleChar (aplTypeRht))
       || (IsNumeric (aplTypeRht) && IsSimpleChar (aplTypeLft))))
    {
        // Split cases based upon the left & right arg storage types
        if (IsSimpleBool (aplTypeLft) && IsSimpleBool (aplTypeRht))
        {
            // Handle APLBOOL vs. APLBOOL
            if (!PrimFnDydEpsilonBvB (lpMemRes,         // Ptr to result global memory data
                                      aplNELMLft,       // Left arg NELM
                                      lpMemLft,         // Ptr to left arg global memory data
                                      aplNELMRht,       // Right arg NELM
                                      lpMemRht,         // Ptr to right arg global memory data
                                      lpbCtrlBreak))    // Ptr to Ctrl-Break flag
                goto ERROR_EXIT;
        } else
        if (IsNumeric (aplTypeLft) && IsSimpleBool (aplTypeRht))
        {
            // Handle APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP/HCxy vs. APLBOOL
            if (!PrimFnDydEpsilonNvB (lpMemRes,         // Ptr to result global memory data
                                      aplTypeLft,       // Left arg storage type
                                      aplNELMLft,       // Left arg NELM
                                      lpMemLft,         // Ptr to left arg global memory data
                                      aplNELMRht,       // Right arg NELM
                                      lpMemRht,         // Ptr to right arg global memory data
                                      lpbCtrlBreak))    // Ptr to Ctrl-Break flag
                goto ERROR_EXIT;
        } else
        if (IsNumeric (aplTypeLft) && IsPermVector (lpMemHdrRht))
        {
            // Handle APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP/HCxy vs. PV
            if (!PrimFnDydEpsilonNvP_EM (lpMemRes,      // Ptr to result global memory data
                                         aplTypeLft,    // Left arg storage type
                                         aplNELMLft,    // Left arg NELM
                                         lpMemLft,      // Ptr to left arg global memory data
                                         lpMemHdrRht,   // Ptr to right arg header
                                         aplTypeRht,    // Right arg storage type
                                         aplNELMRht,    // Right arg NELM
                                         lpbCtrlBreak,  // Ptr to Ctrl-Break flag
                                         lptkFunc))     // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsNumeric (aplTypeLft) && IsSimpleAPA (aplTypeRht))
        {
            // Handle APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP/HCxy vs. APLAPA
            if (!PrimFnDydEpsilonNvA_EM (lpMemRes,      // Ptr to result global memory data
                                         aplTypeLft,    // Left arg storage type
                                         aplNELMLft,    // Left arg NELM
                                         lpMemLft,      // Ptr to left arg global memory data
                                         aplNELMRht,    // Right arg NELM
                                         lpMemRht,      // Ptr to right arg global memory data
                                         lpbCtrlBreak,  // Ptr to Ctrl-Break flag
                                         lptkFunc))     // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsNumeric (aplTypeLft) && IsNumeric (aplTypeRht))
        {
            // Handle Numeric vs. Numeric not handled above
            if (!PrimFnDydEpsilonNvN_EM (lpMemRes,      // Ptr to result global memory data
                                         lptkLftArg,    // Ptr to left arg token
                                         aplTypeLft,    // Left arg storage type
                                         aplNELMLft,    // Left arg NELM
                                         aplRankLft,    // Left arg rank
                                         lpMemLft,      // Ptr to left arg global memory data
                                         lptkRhtArg,    // Ptr to right arg token
                                         aplTypeRht,    // Right arg storage type
                                         aplNELMRht,    // Right arg NELM
                                         aplRankRht,    // Right arg rank
                                         lpMemRht,      // Ptr to right arg global memory data
                                         lpbCtrlBreak,  // Ptr to Ctrl-Break flag
                                         lptkFunc))     // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsSimpleChar (aplTypeLft) && IsSimpleChar (aplTypeRht))
        {
            // Handle APLCHAR vs. APLCHAR
            if (!PrimFnDydEpsilonCvC_EM (lpMemRes,      // Ptr to result global memory data
                                         aplNELMLft,    // Left arg NELM
                                         lpMemLft,      // Ptr to left arg global memory data
                                         aplNELMRht,    // Right arg NELM
                                         lpMemRht,      // Ptr to right arg global memory data
                                         lpbCtrlBreak,  // Ptr to Ctrl-Break flag
                                         lptkFunc))     // Ptr to function token
                goto ERROR_EXIT;
        } else
        {
            // Handle all other combinations
            if (!PrimFnDydEpsilonOther_EM (lpMemRes,        // Ptr to result global memory data
                                           aplTypeLft,      // Left arg storage type
                                           aplNELMLft,      // Left arg NELM
                                           lpMemLft,        // Ptr to left arg global memory data
                                           aplTypeRht,      // Right arg storage type
                                           aplNELMRht,      // Right arg NELM
                                           lpMemRht,        // Ptr to right arg global memory data
                                           lpbCtrlBreak,    // Ptr to Ctrl-Break flag
                                           lptkFunc))       // Ptr to function token
                goto ERROR_EXIT;
        } // End IF/ELSE/...
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken, FALSE);

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkAxis);
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

    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnDydEpsilon_EM_YY


//***************************************************************************
//  $PrimFnDydEpsilonBvB
//
//  Dyadic epsilon of APLBOOL vs. APLBOOL
//***************************************************************************

UBOOL PrimFnDydEpsilonBvB
    (LPAPLBOOL lpMemRes,            // Ptr to result global memory data
     APLNELM   aplNELMLft,          // Left arg (and result) NELM
     LPAPLBOOL lpMemLft,            // Ptr to left arg global memory data
     APLNELM   aplNELMRht,          // Right arg NELM
     LPAPLBOOL lpMemRht,            // Ptr to right arg global memory data
     LPUBOOL   lpbCtrlBreak)        // Ptr to Ctrl-Break flag

{
    UBOOL   Found[2];               // TRUE iff there is at least one [0,1] in the right arg
    APLUINT BytesInLftBits,         // # bytes in the left arg data
            BytesInRhtBits,         // ...            right ...
            uBit,                   // The bit we found or are searching for
            uValid,                 // # valid bits in the arg
            uLft,                   // Loop counter
            uRht;                   // Loop counter

    // The result is either all 0s              (if right arg is empty -- already ruled out), or
    //                      same as Lft         (if right arg is all 1s), or
    //                      complement of Lft   (if right arg is all 0s), or
    //                      all 1s              (if right arg has at least one 0 and one 1)

    // Calculate the # bytes in the left arg (result) data
    BytesInLftBits = RoundUpBitsToBytes (aplNELMLft);

    // Calculate the # bytes in the right arg data
    BytesInRhtBits = RoundUpBitsToBytes (aplNELMRht);

    // Get the first bit from the right arg
    uBit = BIT0 & *(LPAPLBOOL) lpMemRht;

    // Save that information
    Found[uBit] = TRUE;

    // Complement the value to get the bit we're searching for
    uBit = !uBit;

    // Search the right arg for uBit
    for (Found[uBit] = FALSE, uRht = 0; uRht < (BytesInRhtBits - 1); uRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        if (FastBoolTrans[lpMemRht[uRht]][fbtFirst[uBit]] < NBIB)
        {
            Found[uBit] = TRUE;

            break;
        } // End IF
    } // End FOR

    // If we didn't find uBit, check the last byte (may be short)
    if (!Found[uBit])
    {
        // Calculate the # valid bits in the last byte in the right arg
        uValid = (MASKLOG2NBIB & (aplNELMRht - 1)) + 1;

        // Handle the last byte specially
        Found[uBit] = (FastBoolTrans[lpMemRht[uRht] & ((BIT0 << uValid) - 1)][fbtFirst[uBit]] < uValid);
    } // End IF

    // If we found both a 0 and a 1, the result is all 1s
    if (Found[0] && Found[1])
        FillBitMemory (lpMemRes, aplNELMLft);
    else
    // If we found only 0s, the result is the complement of the left arg
    if (Found[0])
    {
        // Copy all but the last byte
        for (uLft = 0; uLft < (BytesInLftBits - 1); uLft++)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (lpbCtrlBreak))
                goto ERROR_EXIT;

            *lpMemRes++ = ~*lpMemLft++;
        } // End FOR

        // Calculate the # valid bits in the last byte in the left arg
        uValid = (MASKLOG2NBIB & (aplNELMLft - 1)) + 1;

        // Handle the last byte specially
        *lpMemRes |= ((BIT0 << uValid) - 1) & ~*lpMemLft;
    } else
    // If we found only 1s, the result is the same as the left arg
    if (Found[1])
        CopyMemory (lpMemRes, lpMemLft, (APLU3264) BytesInLftBits);

    return TRUE;
ERROR_EXIT:
    return FALSE;
} // End PrimFnDydEpsilonBvB


//***************************************************************************
//  $PrimFnDydEpsilonNvB
//
//  Dyadic epsilon of APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP/HCxy vs. APLBOOL
//***************************************************************************

UBOOL PrimFnDydEpsilonNvB
    (LPAPLBOOL lpMemRes,            // Ptr to result global memory data
     APLSTYPE  aplTypeLft,          // Left arg storage type
     APLNELM   aplNELMLft,          // Left arg (and result) NELM
     LPAPLBOOL lpMemLft,            // Ptr to left arg global memory data
     APLNELM   aplNELMRht,          // Right arg NELM
     LPAPLBOOL lpMemRht,            // Ptr to right arg global memory data
     LPUBOOL   lpbCtrlBreak)        // Ptr to Ctrl-Break flag

{
    UBOOL     Found[2];             // TRUE iff there is at least one [0,1] in the right arg
    APLUINT   BytesInRhtBits,       // # bytes in the right arg data
              uBit,                 // The bit we found or are searching for
              uValid,               // # valid bits in the arg
              uLft,                 // Loop counter
              uRht;                 // Loop counter
    UINT      uBitIndex;            // Bit index when marching through Booleans
    APLFLOAT  fQuadCT;              // []CT
    UBOOL     bRet;                 // TRUE iff the result is valid
    ALLTYPES  atLft = {0};          // Left arg items asALLTYPEs
    TC_ACTION tcAction;             // Ptr to TCA routine

    // Point to TCA routine to extract an indexed
    //   value from the right arg
    tcAction = aTypeActConvert[aplTypeLft][ARRAY_BOOL];

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Calculate the # bytes in the right arg data
    BytesInRhtBits = RoundUpBitsToBytes (aplNELMRht);

    // Get the first bit from the right arg
    uBit = BIT0 & *(LPAPLBOOL) lpMemRht;

    // Save that information
    Found[uBit] = TRUE;

    // Complement the value to get the bit we're searching for
    uBit = !uBit;

    // Search the right arg for uBit
    for (Found[uBit] = FALSE, uRht = 0; uRht < (BytesInRhtBits - 1); uRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        if (FastBoolTrans[lpMemRht[uRht]][fbtFirst[uBit]] < NBIB)
        {
            Found[uBit] = TRUE;

            break;
        } // End IF
    } // End FOR

    // If we didn't find uBit, check the last byte (may be short)
    if (!Found[uBit])
    {
        // Calculate the # valid bits in the last byte in the right arg
        uValid = (MASKLOG2NBIB & (aplNELMRht - 1)) + 1;

        // Handle the last byte specially
        Found[uBit] = (FastBoolTrans[lpMemRht[uRht] & ((BIT0 << uValid) - 1)][fbtFirst[uBit]] < uValid);
    } // End IF

    // Initialize the Boolean bit index
    uBitIndex = 0;

    // Use []CT when converting the left arg
    atLft. enumCT = ENUMCT_USER;
    atLft.fQuadCT = fQuadCT;

    // Loop through the left arg,
    //   saving in the result Found[0] for each 0 in the left arg, and
    //                        Found[1] for each 1 in the left arg, and
    //                        0        for each non-Boolean ...
    for (uLft = 0; uLft < aplNELMLft; uLft++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // Attempt to convert the current item from the left arg to a BOOL
        (*tcAction) (lpMemLft, uLft, &atLft, &bRet);
        if (bRet)
        {
            // Split cases based upon the left arg value
            if (IsBooleanValue (atLft.aplInteger))
                // Save the appropriate value in the result
                *lpMemRes |= Found[atLft.aplInteger] << uBitIndex;
////////////else
////////////    // Save the appropriate value in the result
////////////    *lpMemRes |= 0;
        } // End IF

        // Free the old atLft (if any)
        (*aTypeFree[aplTypeLft]) (&atLft, 0);

        // Zero the memory in case we might free it
        ZeroMemory (&atLft, sizeof (atLft));

        // Check for end-of-byte
        if (++uBitIndex EQ NBIB)
        {
            uBitIndex = 0;              // Start over
            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
        } // End IF
    } // End FOR

    return TRUE;
ERROR_EXIT:
    return FALSE;
} // End PrimFnDydEpsilonNvB


//***************************************************************************
//  $PrimFnDydEpsilonNvA_EM
//
//  Dyadic epsilon of APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP/HCxy vs. APLAPA
//***************************************************************************

UBOOL PrimFnDydEpsilonNvA_EM
    (LPAPLBOOL lpMemRes,                // Ptr to result global memory data
     APLSTYPE  aplTypeLft,              // Left arg storage type
     APLNELM   aplNELMLft,              // Left arg NELM
     LPVOID    lpMemLft,                // Ptr to left arg global memory data
     APLNELM   aplNELMRht,              // Right arg NELM
     LPAPLAPA  lpMemRht,                // Ptr to right arg global memory data
     LPUBOOL   lpbCtrlBreak,            // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    APLINT       iLft,                  // Loop counter
                 apaOffRht,             // Right arg APA offset
                 apaMulRht,             // ...           multiplier
                 apaMinRht,             // ...           minimum value
                 apaMaxRht,             // ...           maximum ...
                 aplIntegerLft;         // Left arg integer
    APLFLOAT     fQuadCT;               // []CT
    UBOOL        bRet = FALSE;          // TRUE iff the result is valid

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Get the right arg APA parameters
    apaOffRht = lpMemRht->Off;
    apaMulRht = lpMemRht->Mul;

    if (apaMulRht >= 0)
    {
        apaMinRht = apaOffRht;
        apaMaxRht = apaOffRht + (aplNELMRht - 1) * apaMulRht;
    } else
    {
        apaMinRht = apaOffRht + (aplNELMRht - 1) * apaMulRht;
        apaMaxRht = apaOffRht;
    } // End IF/ELSE

    // Loop through the left arg values and look'em up
    //   in the right arg
    for (iLft = 0; iLft < (APLINT) aplNELMLft; iLft++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // Split cases based upon the left arg storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                // Get the next integer from the left arg
                aplIntegerLft =
                  GetNextInteger (lpMemLft,     // Ptr to global memory
                                  aplTypeLft,   // Storage type
                                  iLft);        // Index
                // Mark as an integer
                bRet = TRUE;

                break;

            case ARRAY_FLOAT:
                // Attempt to convert the float to an integer using []CT
                aplIntegerLft =
                  FloatToAplint_CT (*((LPAPLFLOAT) lpMemLft)++,
                                    fQuadCT,
                                   &bRet);
                break;

            case ARRAY_RAT:
                // Attempt to convert the RAT to an APLINT using []CT
                aplIntegerLft = mpq_get_ctsx (((LPAPLRAT) lpMemLft)++, fQuadCT, &bRet);

                break;

            case ARRAY_VFP:
                // Attempt to convert the VFP to an APLINT using []CT
                aplIntegerLft = mpfr_get_ctsx (((LPAPLVFP) lpMemLft)++, fQuadCT, &bRet);

                break;

            case ARRAY_ARB:
                // Attempt to convert the ARB to an APLINT using []CT
                aplIntegerLft = arb_get_ctsx  (((LPAPLARB) lpMemLft)++, fQuadCT, &bRet);

                break;

            defstop
                break;
        } // End SWITCH

        // Determine if the left arg value is in the right arg
        if (bRet                                            //     L is an integer
         && apaMinRht <= aplIntegerLft                      // AND L is at or above the minimum
         &&              aplIntegerLft <= apaMaxRht         // AND L is at or below the maximum
         && (0 EQ apaMulRht                                 // AND (R is a constant
          || 0 EQ (aplIntegerLft - apaOffRht) % apaMulRht)) //   OR L is in the APA)
            // Save in the result
            lpMemRes[iLft >> LOG2NBIB] |=
              1 << (MASKLOG2NBIB & (UINT) iLft);
////////else
////////    // Save in the result
////////    lpMemRes[iLft >> LOG2NBIB] |=
////////      0 << (MASKLOG2NBIB & (UINT) iLft);
    } // End FOR

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    return bRet;
} // End PrimFnDydEpsilonNvA_EM


//***************************************************************************
//  $PrimFnDydEpsilonNvP_EM
//
//  Dyadic epsilon of APLBOOL/APLINT/APLAPA/APLFLOAT/APLRAT/APLVFP/HCxy vs. PV
//***************************************************************************

UBOOL PrimFnDydEpsilonNvP_EM
    (LPAPLBOOL         lpMemRes,        // Ptr to result global memory data
     APLSTYPE          aplTypeLft,      // Left arg storage type
     APLNELM           aplNELMLft,      // Left arg NELM
     LPVOID            lpMemLft,        // Ptr to left arg global memory data
     LPVARARRAY_HEADER lpMemHdrRht,     // Ptr to right arg header
     APLSTYPE          aplTypeRht,      // Right arg storage type
     APLNELM           aplNELMRht,      // Right arg NELM
     LPUBOOL           lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPTOKEN           lptkFunc)        // Ptr to function token

{
    APLUINT   uLft;                     // Loop counter
    APLINT    aplIntegerRhtMin,         // Right arg minimum value
              aplIntegerRhtMax,         // Right arg maximum ...
              aplIntegerLft;            // Left  ...
    APLFLOAT  aplFloatLft,              // Left arg float
              fQuadCT;                  // []CT
    UINT      uBitMask;                 // Bit mask for looping through Booleans
    UBOOL     bRet = FALSE;             // TRUE iff the result is valid

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Get the minimim and maximum values in the PV
    aplIntegerRhtMin = lpMemHdrRht->PV1;
    aplIntegerRhtMax = aplNELMRht - lpMemHdrRht->PV0;

    // Initialize the bit mask
    uBitMask = BIT0;

    // Loop through the left arg comparing each value
    //   against the minimum and maximum values in the PV,
    //   saving the answer in the result
    for (uLft = 0; uLft < aplNELMLft; uLft++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // Split cases based upon the left arg storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                // Get the next integer
                aplIntegerLft =
                  GetNextInteger (lpMemLft,
                                  aplTypeLft,
                                  uLft);
                // Mark as an integer
                bRet = TRUE;

                break;

            case ARRAY_FLOAT:
                // Get the next float
                aplFloatLft = *((LPAPLFLOAT) lpMemLft)++;

                // Attempt to convert the float to an integer using []CT
                aplIntegerLft =
                  FloatToAplint_CT (aplFloatLft,
                                    fQuadCT,
                                   &bRet);
                break;

            case ARRAY_RAT:
                // Attempt to convert the RAT to an APLINT using []CT
                aplIntegerLft = mpq_get_ctsx (((LPAPLRAT) lpMemLft)++, fQuadCT, &bRet);

                break;

            case ARRAY_VFP:
                // Attempt to convert the VFP to an APLINT using []CT
                aplIntegerLft = mpfr_get_ctsx (((LPAPLVFP) lpMemLft)++, fQuadCT, &bRet);

                break;

            case ARRAY_ARB:
                // Attempt to convert the ARB to an APLINT using []CT
                aplIntegerLft = arb_get_ctsx  (((LPAPLARB) lpMemLft)++, fQuadCT, &bRet);

                break;

            defstop
                break;
        } // End SWITCH

        // If it's an integer, and it's within range, ...
        if (bRet
         && aplIntegerRhtMin <= aplIntegerLft
         &&                     aplIntegerLft <= aplIntegerRhtMax)
            // Save a one in the result
            *lpMemRes |= uBitMask;
////////else
////////    // Save a zero in the result
////////    *lpMemRes |= 0;

        // Shift over the bit mask
        uBitMask <<= 1;

        // Check for end-of-byte
        if (uBitMask EQ END_OF_BYTE)
        {
            uBitMask = BIT0;            // Start over
            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
        } // End IF
    } // End FOR

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    return bRet;
} // End PrimFnDydEpsilonNvP_EM


//***************************************************************************
//  $PrimFnDydEpsilonCvC_EM
//
//  Dyadic epsilon of APLCHAR vs. APLCHAR
//***************************************************************************

UBOOL PrimFnDydEpsilonCvC_EM
    (LPAPLBOOL lpMemRes,            // Ptr to result global memory data
     APLNELM   aplNELMLft,          // Left arg (and result) NELM
     LPAPLCHAR lpMemLft,            // Ptr to left arg global memory data
     APLNELM   aplNELMRht,          // Right arg NELM
     LPAPLCHAR lpMemRht,            // Ptr to right arg global memory data
     LPUBOOL   lpbCtrlBreak,        // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)            // Ptr to function token

{
    LPAPLBOOL lpMemTT;              // Translate table global memory handle
    APLUINT   ByteTT;               // # bytes in the Translate Table
    APLUINT   uLft,                 // Loop counter
              uRht;                 // Loop counter
    APLCHAR   aplChar;              // Temporary char
    UINT      uBitIndex;            // Bit index for marching through Booleans

    // Calculate # bytes in the TT at one bit per 16-bit index (APLCHAR)
    ByteTT = RoundUpBitsToBytes (APLCHAR_SIZE);

    // Check for overflow
    if (ByteTT NE (APLU3264) ByteTT)
        goto WSFULL_EXIT;

    // Allocate space for a ByteTT Translate Table
    // Note that this allocation is GPTR (GMEM_FIXED | GMEM_ZEROINIT)
    //   because we'll use it quickly and then free it.
    lpMemTT = DbgGlobalAlloc (GPTR, (APLU3264) ByteTT);
    if (lpMemTT EQ NULL)
        goto WSFULL_EXIT;

    // Trundle through the right arg marking the TT
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // Get the APLCHAR from the right arg
        aplChar = *lpMemRht++;

        lpMemTT[aplChar >> LOG2NBIB] |= BIT0 << (MASKLOG2NBIB & aplChar);
    } // End FOR

    // Trundle through the left arg looking the chars in the TT
    //   and setting the result bit
    uBitIndex = 0;
    for (uLft = 0; uLft < aplNELMLft; uLft++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // Get the APLCHAR from the left arg
        aplChar = *lpMemLft++;

        // If the char is in the TT, ...
        if (lpMemTT[aplChar >> LOG2NBIB] & (BIT0 << (MASKLOG2NBIB & aplChar)))
            *lpMemRes |= (BIT0 << uBitIndex);
        // Check for end-of-byte
        if (++uBitIndex EQ NBIB)
        {
            uBitIndex = 0;              // Start over
            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
        } // End IF
    } // End FOR

    // We no longer need this storage
    DbgGlobalFree (lpMemTT); lpMemTT = NULL;

    return TRUE;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    return FALSE;
} // End PrimFnDydEpsilonCvC_EM


//***************************************************************************
//  $PrimFnDydEpsilonNvN_EM
//
//  Dyadic epsilon of Numeric vs. Numeric not special case (NvB, NvP, NvA)
//***************************************************************************

UBOOL PrimFnDydEpsilonNvN_EM
    (LPAPLBOOL lpMemRes,                        // Ptr to result global memory data
     LPTOKEN   lptkLftArg,                      // Ptr to left arg token
     APLSTYPE  aplTypeLft,                      // Left arg storage type
     APLNELM   aplNELMLft,                      // Left arg NELM
     APLRANK   aplRankLft,                      // Left arg rank
     LPVOID    lpMemLft,                        // Ptr to left arg global memory data
     LPTOKEN   lptkRhtArg,                      // Ptr to right arg token
     APLSTYPE  aplTypeRht,                      // Right arg storage type
     APLNELM   aplNELMRht,                      // Right arg NELM
     APLRANK   aplRankRht,                      // Right arg rank
     LPVOID    lpMemRht,                        // Ptr to right arg global memory data
     LPUBOOL   lpbCtrlBreak,                    // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                        // Ptr to function token

{
    TOKEN             tkFunc = {0};             // Grade-up function token
    APLBOOL           bQuadIO;                  // []IO
    HGLOBAL           hGlbGupRht = NULL;        // Right arg grade-up global memory handle
    LPVARARRAY_HEADER lpMemHdrGupRht = NULL;    // Ptr to right arg grade up header
    LPAPLINT          lpMemGupRht;              // Ptr to right arg grade-up global memory
#ifdef GRADE2ND
    HGLOBAL           hGlbGupLft = NULL;        // Left arg grade-up global memory handle
    LPVARARRAY_HEADER lpMemHdrGupLft = NULL;    // Ptr to left arg grade up header
    LPAPLINT          lpMemGupLft;              // Ptr to left arg grade-up global memory
    UBOOL             bLastVal;                 // Last value saved in the result
    APLINT            aplIntegerLst,            // Last left arg integer
                      iRes;                     // Loop counter
    ALLTYPES          atLst = {0};              // Last value as ALLTYPES
#endif
    APLINT            iLft,                     // Loop counter
                      iRht,                     // Loop counter
                      iMin,                     // Minimum index
                      iMax;                     // Maximum ...
    APLINT            aplIntegerLft = 0,        // Left arg integer
                      aplIntegerZero = 0;       // A zero in case the right arg is a scalar
    LPPL_YYSTYPE      lpYYRes;                  // Ptr to grade-up result
    UBOOL             bRet = FALSE;             // TRUE iff the result is valid
    APLFLOAT          fQuadCT;                  // []CT
    APLSTYPE          aplTypeCom;               // Common storage type
    ALLTYPES          atLft = {0},              // Left arg items as ALLTYPES
                      atRht = {0};              // Right ...
    TP_ACTION         tpActionLft,              // Function ptr to left arg TP_ routine
                      tpActionRht;              // ...             right ...

    // Calculate the common var storage type
    aplTypeCom = aTypePromote[aplTypeLft][aplTypeRht];

    // Get the TPA ptrs
    tpActionLft = aTypeActPromote[aplTypeLft][aplTypeCom];
    tpActionRht = aTypeActPromote[aplTypeRht][aplTypeCom];

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Setup the grade-up function token
    tkFunc.tkFlags.TknType   = TKT_FCNIMMED;
    tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////tkFunc.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
    tkFunc.tkData.tkChar     = UTF16_DELTASTILE;
    tkFunc.tkCharIndex       = lptkFunc->tkCharIndex;
#ifdef GRADE2ND
    // If the left arg is not a scalar, ...
    if (!IsScalar (aplRankLft))
    {
        // Set the current index origin to zero for convenience
        SetQuadIO (0);

        // Grade-up the left arg
        lpYYRes =
          PrimFnMonGradeCommon_EM_YY (&tkFunc,      // Ptr to function token
                                       lptkLftArg,  // Ptr to right arg token
                                       NULL,        // Ptr to axis token (may be NULL)
                                       TRUE,        // TRUE iff we're to treat the right arg as ravelled
                                       TRUE);       // TRUE iff we can grade all arrays
        // Restore the index origin
        SetQuadIO (bQuadIO);

        if (lpYYRes EQ NULL)
            goto ERROR_EXIT;
        // Get the grade-up global memory handle
        hGlbGupLft = lpYYRes->tkToken.tkData.tkGlbData;

        // Free the YYRes
        YYFree (lpYYRes); lpYYRes = NULL;

        // Lock the memory to get a ptr to it
        lpMemHdrGupLft = MyGlobalLockVar (hGlbGupLft);

        // Skip over the header and dimensions to the data
        lpMemGupLft = VarArrayDataFmBase (lpMemHdrGupLft);
    } else
        lpMemGupLft = &aplIntegerZero;
#endif
    // If the right arg is not a scalar, ...
    if (!IsScalar (aplRankRht))
    {
        // Set the current index origin to zero for convenience
        SetQuadIO (0);

        // Grade-up the right arg
        lpYYRes =
          PrimFnMonGradeCommon_EM_YY (&tkFunc,      // Ptr to function token
                                       lptkRhtArg,  // Ptr to right arg token
                                       NULL,        // Ptr to axis token (may be NULL)
                                       TRUE,        // TRUE iff we're to treat the right arg as ravelled
                                       TRUE);       // TRUE iff we can grade all arrays
        // Restore the index origin
        SetQuadIO (bQuadIO);

        if (lpYYRes EQ NULL)
            goto ERROR_EXIT;
        // Get the grade-up global memory handle
        hGlbGupRht = lpYYRes->tkToken.tkData.tkGlbData;

        // Free the YYRes
        YYFree (lpYYRes); lpYYRes = NULL;

        // Lock the memory to get a ptr to it
        lpMemHdrGupRht = MyGlobalLockVar (hGlbGupRht);

        // Skip over the header and dimensions to the data
        lpMemGupRht = VarArrayDataFmBase (lpMemHdrGupRht);
    } else
        lpMemGupRht = &aplIntegerZero;

#ifdef GRADE2ND
    // Get the first value from the left arg as common storage type
     (*tpActionLft) (lpMemLft, 0, &atLft);
#endif
    // Loop through the left arg values and look'em up
    //   in the right arg
    for (iLft = 0; iLft < (APLINT) aplNELMLft; iLft++)
    {
        // Initialize return value in case we fail
        bRet = FALSE;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;
#ifdef GRADE2ND
        // Save the left arg value as the last one
        atLst = atLft;

        // Get the result index
        iRes = lpMemGupLft[iLft];

        // Get the next value from the left arg as common storage type
        (*tpActionLft) (lpMemLft, iRes, &atLft);

        // Check for duplicate value from the left arg
        if (iLft
         && hcXY_cmp (aplTypeCom, &atLft, &atLst, TRUE, fQuadCT) EQ 0)
        {
            // Save in the result
            lpMemRes[iRes >> LOG2NBIB] |=
              bLastVal << (MASKLOG2NBIB & (UINT) iRes);

            continue;
        } // End IF
#else
        // Get the next value from the left arg as common storage type
        (*tpActionLft) (lpMemLft, iLft, &atLft);
#endif
        // Initialize the right arg minimum and maximum indices
        iMin = 0;
        iMax = aplNELMRht - 1;

        // Lookup this value in the right arg (binary search)
        while (!bRet && iMin <= iMax)
        {
            // Set the current index
            iRht = (iMin + iMax) / 2;

            // Get the next value from the right arg as common storage type
            (*tpActionRht) (lpMemRht, lpMemGupRht[iRht], &atRht);

            // Check for a match
            switch (hcXY_cmp (aplTypeCom, &atLft, &atRht, TRUE, fQuadCT))
            {
                case -1:    // Lft < Rht
                    iMax = iRht - 1;

                    break;

                case 1:     // Lft > Rht
                    iMin = iRht + 1;

                    break;

                case 0:     // Lft EQ Rht
                    // We found a match
#ifdef GRADE2ND
                    // Save as last value
                    bLastVal = 1;

                    // Save in the result
                    lpMemRes[iRes >> LOG2NBIB] |=
                      1 << (MASKLOG2NBIB & (UINT) iRes);
#else
                    // Save in the result
                    lpMemRes[iLft >> LOG2NBIB] |=
                      1 << (MASKLOG2NBIB & (UINT) iLft);
#endif
                    // Mark as successful so as to break out of the WHILE loop
                    bRet = TRUE;

                    break;

                defstop
                    break;
            } // End SWITCH

            // Free the old atRht (if any)
            (*aTypeFree[aplTypeCom]) (&atRht, 0);

            // Zero the memory in case we might free it
            ZeroMemory (&atRht, sizeof (atRht));
        } // End WHILE
#ifdef GRADE2ND
        // If we didn't find a match, ...
        if ((!bRet) || iMin > iMax)
        {
            // Save as last value
            bLastVal = 0;

////////////// Save in the result
////////////lpMemRes[iRes >> LOG2NBIB] |=
////////////  0 << (MASKLOG2NBIB & (UINT) iRes);
        } // End IF
#endif
        // Free the old atLft (if any)
        (*aTypeFree[aplTypeCom]) (&atLft, 0);

        // Zero the memory in case we might free it
        ZeroMemory (&atLft, sizeof (atLft));
    } // End FOR

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
#ifdef GRADE2ND
    if (hGlbGupLft NE NULL && lpMemHdrGupLft NE NULL)
    {
        if (lpMemHdrGupLft NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbGupLft); lpMemHdrGupLft = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalVar (hGlbGupLft); hGlbGupLft = NULL;
    } // End IF
#endif
    if (hGlbGupRht NE NULL && lpMemHdrGupRht NE NULL)
    {
        if (lpMemHdrGupRht NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbGupRht); lpMemHdrGupRht = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalVar (hGlbGupRht); hGlbGupRht = NULL;
    } // End IF

    return bRet;
} // End PrimFnDydEpsilonNvN_EM


//***************************************************************************
//  $PrimFnDydEpsilonOther_EM
//
//  Dyadic epsilon between all other arg combinations
//***************************************************************************

UBOOL PrimFnDydEpsilonOther_EM
    (LPAPLBOOL lpMemRes,                    // Ptr to result global memory data
     APLSTYPE  aplTypeLft,                  // Left arg storage type
     APLNELM   aplNELMLft,                  // Left arg NELM
     LPVOID    lpMemLft,                    // Ptr to left arg global memory data
     APLSTYPE  aplTypeRht,                  // Right arg storage type
     APLNELM   aplNELMRht,                  // Right arg NELM
     LPVOID    lpMemRht,                    // Ptr to right arg global memory data
     LPUBOOL   lpbCtrlBreak,                // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)                    // Ptr to function token

{
    HGLOBAL           hGlbSubLft,           // Left arg item global memory handle
                      hGlbSubRht;           // Right ...
    APLLONGEST        aplLongestSubLft,     // Left arg item immediate value
                      aplLongestSubRht;     // Right ...
    IMM_TYPES         immTypeSubLft,        // Left arg item immediate type
                      immTypeSubRht;        // Right ...
    UINT              uBitIndex;            // Bit index for marching through Booleans
    APLFLOAT          fQuadCT;              // []CT
    APLUINT           uLft,                 // Loop counter
                      uRht;                 // ...
    APLUINT           ByteRes;              // # bytes in the result
    LPVARARRAY_HEADER lpMemHdrTmp = NULL;   // Ptr to temp header
    LPVOID            lpMemTmp;             // ...         global memory
    LPPL_YYSTYPE      lpYYTmp;              // Ptr to the temporary result
    UBOOL             bCmp;                 // TRUE iff the comparison is TRUE
    APLSTYPE          aplTypeSubCom;        // Common storage type
    ALLTYPES          atLft = {0},          // Temps
                      atRht = {0};          // ...

    // This leaves:  Left vs.  Right
    //               BIAF vs.     HN
    //               HN   vs. BIAFHN

    uBitIndex = 0;

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Loop through the left arg
    for (uLft = 0; uLft < aplNELMLft; uLft++)
    {
        // Get the next value from the left arg
        GetNextValueMem (lpMemLft,              // Ptr to left arg global memory
                         aplTypeLft,            // Left arg storage type
                         aplNELMLft,            // Left arg NELM
                         uLft,                  // Left arg index
                        &hGlbSubLft,            // Left arg item LPSYMENTRY or HGLOBAL (may be NULL)
                        &aplLongestSubLft,      // Ptr to left arg immediate value
                        &immTypeSubLft);        // Ptr to left arg immediate type
        // Loop through the right arg
        for (uRht = 0; uRht < aplNELMRht; uRht++)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (lpbCtrlBreak))
                goto ERROR_EXIT;

            // Initialize with something harmless
            aplTypeSubCom = ARRAY_ERROR;

            // Get the next value from the right arg
            GetNextValueMem (lpMemRht,              // Ptr to right arg global memory
                             aplTypeRht,            // Right arg storage type
                             aplNELMRht,            // Right arg NELM
                             uRht,                  // Right arg index
                            &hGlbSubRht,            // Right arg item LPSYMENTRY or HGLOBAL (may be NULL)
                            &aplLongestSubRht,      // Ptr to right arg immediate value
                            &immTypeSubRht);        // Ptr to right arg immediate type
            // If both items are simple or global numeric, ...
            if ((IsImmNum (immTypeSubLft) || IsImmGlbNum (immTypeSubLft))
             && (IsImmNum (immTypeSubRht) || IsImmGlbNum (immTypeSubRht)))
            {
                APLSTYPE          aplTypeSubLft = TranslateImmTypeToArrayType (immTypeSubLft),
                                  aplTypeSubRht = TranslateImmTypeToArrayType (immTypeSubRht);
                LPVARARRAY_HEADER lpMemHdrSubLft = NULL,    // Ptr to left item global memory header
                                  lpMemHdrSubRht = NULL;    // ...    right ...
                LPVOID            lpMemSubLft,              // Ptr to left item global memory
                                  lpMemSubRht;              // ...    right ...

                // This leaves BIFRV vs. BIFRV

                // Calculate the common type
                aplTypeSubCom = aTypePromote[aplTypeSubLft][aplTypeSubRht];

                // If the left item is a global, ...
                if (hGlbSubLft NE NULL)
                {
                    // If the HGLOBAL is a ptr, ...
                    if (GetPtrTypeDir (hGlbSubLft) EQ PTRTYPE_HGLOBAL)
                    {
                        // Lock the memory to get a ptr to it
                        lpMemHdrSubLft = MyGlobalLockVar (hGlbSubLft);

                        // Skip over the header and dimensions to the data
                        lpMemSubLft = VarArrayDataFmBase (lpMemHdrSubLft);
                    } else
                        // Point to data
                        lpMemSubLft = hGlbSubLft;
                } else
                    // Point to the data
                    lpMemSubLft = &aplLongestSubLft;

                // If the left item is a global, ...
                if (hGlbSubRht NE NULL)
                {
                    // If the HGLOBAL is a ptr, ...
                    if (GetPtrTypeDir (hGlbSubRht) EQ PTRTYPE_HGLOBAL)
                    {
                        // Lock the memory to get a ptr to it
                        lpMemHdrSubRht = MyGlobalLockVar (hGlbSubRht);

                        // Skip over the header and dimensions to the data
                        lpMemSubRht = VarArrayDataFmBase (lpMemHdrSubRht);
                    } else
                        // Point to data
                        lpMemSubRht = hGlbSubRht;
                } else
                    // Point to the data
                    lpMemSubRht = &aplLongestSubRht;

                // Promote the left & right args to the common type
                (*aTypeActPromote[aplTypeSubLft][aplTypeSubCom]) (lpMemSubLft, 0, &atLft);
                (*aTypeActPromote[aplTypeSubRht][aplTypeSubCom]) (lpMemSubRht, 0, &atRht);

                if (lpMemHdrSubLft NE NULL)
                {
                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbSubLft); lpMemHdrSubLft = NULL;
                } // End IF

                if (lpMemHdrSubRht NE NULL)
                {
                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbSubRht); lpMemHdrSubRht = NULL;
                } // End IF

                // If they compare equally, ...
                if (hcXY_cmp (aplTypeSubCom,
                             &atLft,
                             &atRht,
                              TRUE,
                              fQuadCT) EQ 0)
                    goto SET_RESULT_BIT;

                // Free the old atLft & atRht (if any)
                (*aTypeFree[aplTypeSubCom]) (&atLft, 0);
                (*aTypeFree[aplTypeSubCom]) (&atRht, 0);

                // Zero the memory in case we use it again
                ZeroMemory (&atLft, sizeof (atLft));
                ZeroMemory (&atRht, sizeof (atRht));
            } else
            // If both items are globals, ...
            if ((hGlbSubLft NE NULL) && (hGlbSubRht NE NULL))
            {
                TOKEN tkSubLft = {0},       // Left arg item token
                      tkSubRht = {0};       // Right ...

                Assert (GetPtrTypeDir (hGlbSubLft) EQ PTRTYPE_HGLOBAL);
                Assert (GetPtrTypeDir (hGlbSubRht) EQ PTRTYPE_HGLOBAL);

                // Fill in the left arg item token
                tkSubLft.tkFlags.TknType   = TKT_VARARRAY;
////////////////tkSubLft.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////////////tkSubLft.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
                tkSubLft.tkData.tkGlbData  = hGlbSubLft;
                tkSubLft.tkCharIndex       = lptkFunc->tkCharIndex;

                // Fill in the right arg item token
                tkSubRht.tkFlags.TknType   = TKT_VARARRAY;
////////////////tkSubRht.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////////////tkSubRht.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
                tkSubRht.tkData.tkGlbData  = hGlbSubRht;
                tkSubRht.tkCharIndex       = lptkFunc->tkCharIndex;

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
                    goto SET_RESULT_BIT;
            } else
            // If both items are APLCHARs, ...
            if (IsImmChr (immTypeSubLft) && IsImmChr (immTypeSubRht))
            {
                // Compare the APLCHARs
                if (((APLCHAR) aplLongestSubLft) EQ (APLCHAR) aplLongestSubRht)
                    goto SET_RESULT_BIT;
            } else
            // If one item is char and the other is numeric, ...
            if (IsImmChr (immTypeSubLft) && (IsImmNum (immTypeSubRht) || IsImmGlbNum (immTypeSubRht))
             || IsImmChr (immTypeSubRht) && (IsImmNum (immTypeSubLft) || IsImmGlbNum (immTypeSubLft)))
                continue;
            else
            // If the left item is immediate and the right is global, ...
            if ((hGlbSubLft EQ NULL) && (hGlbSubRht NE NULL))
            {
                TOKEN   tkSubLft = {0},         // Left arg item token
                        tkSubRht = {0};         // Right ...
                HGLOBAL hGlbTmp = NULL;         // Temporary global memory handle

                // Fill in the left arg item token
                tkSubLft.tkFlags.TknType   = TKT_VARIMMED;
                tkSubLft.tkFlags.ImmType   = immTypeSubLft;
////////////////tkSubLft.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
                tkSubLft.tkData.tkLongest  = aplLongestSubLft;
                tkSubLft.tkCharIndex       = lptkFunc->tkCharIndex;

                // Fill in the right arg item token
                tkSubRht.tkFlags.TknType   = TKT_VARARRAY;
////////////////tkSubRht.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////////////tkSubRht.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
////////////////tkSubRht.tkData.tkGlbData  = MakePtrTypeGlb (hGlbSubRht);   // Filled in below
                tkSubRht.tkCharIndex       = lptkFunc->tkCharIndex;

                // If the immediate type is a global numeric, ...
                if (IsImmGlbNum (immTypeSubLft))
                {
                    APLSTYPE aplTypeSubRht;         // Right sub arg storage type

                    // Get the subarray storage type
                    aplTypeSubRht = TranslateImmTypeToArrayType (immTypeSubRht);

                    // Calculate space needed for the result (a scalar)
                    ByteRes = CalcArraySize (aplTypeSubRht, 1, 0);

                    // Check for overflow
                    if (ByteRes NE (APLU3264) ByteRes)
                        goto WSFULL_EXIT;

                    // Allocate space for the result
                    hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                    if (hGlbTmp EQ NULL)
                        goto WSFULL_EXIT;

                    // Lock the memory to get a ptr to it
                    lpMemHdrTmp = MyGlobalLock000 (hGlbTmp);

#define lpHeader    lpMemHdrTmp
                    // Fill in the header values
                    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                    lpHeader->ArrType    = aplTypeSubRht;
////////////////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////////////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
                    lpHeader->RefCnt     = 1;
                    lpHeader->NELM       = 1;
                    lpHeader->Rank       = 0;
#undef  lpHeader
                    // Skip over the header and dimension to the data
                    lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

                    // Copy the data from the right arg item to the temp
                    CopyMemory (lpMemTmp, hGlbSubRht, 1 * TranslateArrayTypeToSizeof (aplTypeSubRht));

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbTmp); lpMemHdrTmp = NULL;

                    // Save in common var
                    hGlbSubRht = hGlbTmp;
                } // End IF

                // Save in temp token
                tkSubRht.tkData.tkGlbData  = MakePtrTypeGlb (hGlbSubRht);

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

                // If the temp handle is valid, ...
                if (hGlbTmp NE NULL)
                {
                    // Free the temp global memory handle
                    // Note that we don't use <FreeResultGlobalVar> (or anything that calls that function)
                    //   because the global numeric we copied into this var has not had its RefCnt incremented.
                    MyGlobalFree (hGlbTmp); hGlbTmp = NULL;
                } // End IF

                if (bCmp)
                    goto SET_RESULT_BIT;
            } else
            // If the left item is global and the right is immediate, ...
            if ((hGlbSubLft NE NULL) && (hGlbSubRht EQ NULL))
            {
                TOKEN   tkSubLft = {0},         // Left arg item token
                        tkSubRht = {0};         // Right ...
                HGLOBAL hGlbTmp = NULL;         // Temporary global memory handle

                // Fill in the right arg item token
                tkSubRht.tkFlags.TknType   = TKT_VARIMMED;
                tkSubRht.tkFlags.ImmType   = immTypeSubRht;
////////////////tkSubRht.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
                tkSubRht.tkData.tkLongest  = aplLongestSubRht;
                tkSubRht.tkCharIndex       = lptkFunc->tkCharIndex;

                // Fill in the left arg item token
                tkSubLft.tkFlags.TknType   = TKT_VARARRAY;
////////////////tkSubLft.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////////////tkSubLft.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
////////////////tkSubLft.tkData.tkGlbData  = MakePtrTypeGlb (hGlbSubLft);   // Filled in below
                tkSubLft.tkCharIndex       = lptkFunc->tkCharIndex;

                // If the immediate type is a global numeric, ...
                if (IsImmGlbNum (immTypeSubRht))
                {
                    APLSTYPE aplTypeSubLft;         // Right sub arg storage type

                    // Get the subarray storage type
                    aplTypeSubLft = TranslateImmTypeToArrayType (immTypeSubLft);

                    // Calculate space needed for the result (a scalar)
                    ByteRes = CalcArraySize (aplTypeSubLft, 1, 0);

                    // Check for overflow
                    if (ByteRes NE (APLU3264) ByteRes)
                        goto WSFULL_EXIT;

                    // Allocate space for the result
                    hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                    if (hGlbTmp EQ NULL)
                        goto WSFULL_EXIT;

                    // Lock the memory to get a ptr to it
                    lpMemHdrTmp = MyGlobalLock000 (hGlbTmp);

#define lpHeader    lpMemHdrTmp
                    // Fill in the header values
                    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                    lpHeader->ArrType    = aplTypeSubLft;
////////////////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////////////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
                    lpHeader->RefCnt     = 1;
                    lpHeader->NELM       = 1;
                    lpHeader->Rank       = 0;
#undef  lpHeader
                    // Skip over the header and dimension to the data
                    lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

                    // Copy the data from the right arg item to the temp
                    CopyMemory (lpMemTmp, hGlbSubLft, 1 * TranslateArrayTypeToSizeof (aplTypeSubLft));

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbTmp); lpMemHdrTmp = NULL;

                    // Save in common var
                    hGlbSubLft = hGlbTmp;
                } // End IF

                // Save in temp token
                tkSubLft.tkData.tkGlbData  = MakePtrTypeGlb (hGlbSubLft);

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

                // If the temp handle is valid, ...
                if (hGlbTmp NE NULL)
                {
                    // Free the temp global memory handle
                    // Note that we don't use <FreeResultGlobalVar> (or anything that calls that function)
                    //   because the global numeric we copied into this var has not had its RefCnt incremented.
                    MyGlobalFree (hGlbTmp); hGlbTmp = NULL;
                } // End IF

                if (bCmp)
                    goto SET_RESULT_BIT;
            } // End IF/ELSE/...

            continue;
SET_RESULT_BIT:
            // Set the result bit
            *lpMemRes |= (BIT0 << uBitIndex);

            // Check for valid
            if (!IsErrorType (aplTypeSubCom))
            {
                // Free the old atLft & atRht (if any)
                (*aTypeFree[aplTypeSubCom]) (&atLft, 0);
                (*aTypeFree[aplTypeSubCom]) (&atRht, 0);

                // Zero the memory in case we use it again
                ZeroMemory (&atLft, sizeof (atLft));
                ZeroMemory (&atRht, sizeof (atRht));

                // Initialize with something harmless
                aplTypeSubCom = ARRAY_ERROR;
            } // End IF

            break;
        } // End FOR

        // Check for end-of-byte
        if (++uBitIndex EQ NBIB)
        {
            uBitIndex = 0;      // Start over
            lpMemRes++;         // Skip to next byte
        } // End IF
    } // End FOR

    return TRUE;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    return FALSE;
} // End PrimFnDydEpsilonOther_EM


//***************************************************************************
//  End of File: pf_epsilon.c
//***************************************************************************
