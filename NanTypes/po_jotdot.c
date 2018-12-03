//***************************************************************************
//  NARS2000 -- Primitive Operator -- JotDot
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
//  $PrimOpJotDot_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    monadic operator JotDot (ERROR and "outer product")
//***************************************************************************

LPPL_YYSTYPE PrimOpJotDot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_JOTDOT);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonJotDot_EM_YY (lpYYFcnStrOpr,    // Ptr to operator function strand
                                      lptkRhtArg);      // Ptr to right arg token
    else
        return PrimOpDydJotDot_EM_YY (lptkLftArg,       // Ptr to left arg token
                                      lpYYFcnStrOpr,    // Ptr to operator function strand
                                      lptkRhtArg);      // Ptr to right arg token
} // End PrimOpJotDot_EM_YY


//***************************************************************************
//  $PrimProtoOpJotDot_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator JotDot (ERROR and "outer product")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpJotDot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis)             // Ptr to axis token always NULL)

{
    Assert (lptkAxis EQ NULL);

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return PrimOpMonJotDotCommon_EM_YY (lpYYFcnStrOpr,      // Ptr to operator function strand
                                            lptkRhtArg,         // Ptr to right arg token
                                            TRUE);              // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return PrimOpDydJotDotCommon_EM_YY (lptkLftArg,         // Ptr to left arg token
                                            lpYYFcnStrOpr,      // Ptr to operator function strand
                                            lptkRhtArg,         // Ptr to right arg token
                                            TRUE);              // TRUE iff prototyping
} // End PrimProtoOpJotDot_EM_YY


//***************************************************************************
//  $PrimIdentOpJotDot_EM_YY
//
//  Generate an identity element for the dyadic derived function from the
//    primitive operator JotDot
//***************************************************************************

LPPL_YYSTYPE PrimIdentOpJotDot_EM_YY
    (LPTOKEN      lptkRhtOrig,      // Ptr to original right arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,    // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     LPTOKEN      lptkAxisOpr)      // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYFcnStrRht;     // Ptr to right operand function strand
    HGLOBAL      hGlbMFO;               // Magic function/operator global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig   NE NULL);
    Assert (lpYYFcnStrOpr NE NULL);
    Assert (lptkRhtArg    NE NULL);

    //***************************************************************
    // This operator is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // The (left/right -- depends upon f) identity function for dyadic JotDot
    //   (L {jot}.f R) ("outer product") is
    //   f/0{rho} R.

    // Set ptr to right operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = GetMonLftOper (lpYYFcnStrOpr, lptkAxisOpr); Assert (lpYYFcnStrRht NE NULL);

    // Ensure the right operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     || IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_OPERAND_SYNTAX_EXIT;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the magic function/operator global memory handles
    hGlbMFO = lpMemPTD->hGlbMFO[MFOE_IdnJotDot];

    return
      ExecuteMagicOperator_EM_YY (NULL,                     // Ptr to left arg token
                                 &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                  lpYYFcnStrRht,            // Ptr to left operand function strand
                                  lpYYFcnStrOpr,            // Ptr to function strand
                                  NULL,                     // Ptr to right operand function strand (may be NULL)
                                  lptkRhtArg,               // Ptr to right arg token
                                  lptkAxisOpr,              // Ptr to axis token
                                  hGlbMFO,                  // Magic function/operator global memory handle
                                  NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                  LINENUM_ID);              // Starting line # type (see LINE_NUMS)
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    return NULL;

RIGHT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    return NULL;
} // End PrimIdentOpJotDot_EM_YY


//***************************************************************************
//  Magic function/operator for identity function from the
//    outer product operator
//   f/0{rho} R.
//***************************************************************************

#include "mf_jotdot.h"


//***************************************************************************
//  $PrimOpMonJotDot_EM_YY
//
//  Primitive operator for monadic derived function from JotDot (ERROR)
//***************************************************************************

LPPL_YYSTYPE PrimOpMonJotDot_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpMonJotDotCommon_EM_YY (lpYYFcnStrOpr,      // Ptr to operator function strand
                                        lptkRhtArg,         // Ptr to right arg token
                                        FALSE);             // TRUE iff prototyping
} // End PrimOpMonJotDot_EM_YY


//***************************************************************************
//  $PrimOpMonJotDotCommon_EM_YY
//
//  Primitive operator for monadic derived function from JotDot (ERROR)
//***************************************************************************

LPPL_YYSTYPE PrimOpMonJotDotCommon_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff prototyping

{
    return PrimFnValenceError_EM (&lpYYFcnStrOpr->tkToken APPEND_NAME_ARG);
} // End PrimOpJotDotCommon_EM_YY


//***************************************************************************
//  $PrimOpDydJotDot_EM_YY
//
//  Primitive operator for dyadic derived function from JotDot ("outer product")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydJotDot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStr,           // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpDydJotDotCommon_EM_YY (lptkLftArg,     // Ptr to left arg token
                                        lpYYFcnStr,     // Ptr to operator function strand
                                        lptkRhtArg,     // Ptr to right arg token
                                        FALSE);         // TRUE iff prototoyping
} // End PrimOpDydJotDot_EM_YY


//***************************************************************************
//  $PrimOpDydJotDotCommon_EM_YY
//
//  Primitive operator for dyadic derived function from JotDot ("outer product")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydJotDotCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff prototyping

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht,           // Right ...
                      aplTypeNew,           // New result   ...
                      aplTypeCom = ARRAY_ERROR, // Common ...
                      aplTypeRes,           // Result   ...
                      aplTypeLft2,          // Left arg base storage type
                      aplTypeRht2;          // Right ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht,           // Right ...
                      aplNELMRes;           // Result   ...
    APLLONGEST        aplLongestLft,        // Left arg longest value
                      aplLongestRht;        // Right ...
    APLRANK           aplRankLft,           // Left arg rank
                      aplRankRht,           // Right ...
                      aplRankRes;           // Result   ...
    APLUINT           ByteRes,              // # bytes in the result
                      uLft,                 // Left arg loop counter
                      uRht,                 // Right ...
                      uRes,                 // Result   ...
                      uValErrCnt = 0;       // VALUE ERROR counter
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL,       // Right ...
                      hGlbRes = NULL,       // Result   ...
                      hGlbLft2 = NULL,      // Temp handle for type demotion
                      hGlbRht2 = NULL;      // ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg header
                      lpMemHdrRht = NULL,   // ...    right
                      lpMemHdrRes = NULL,   // ...    result
                      lpMemHdrSub = NULL;   // ...    subitem
    LPVOID            lpMemLft,             // Ptr to left arg global memory
                      lpMemRht,             // Ptr to right ...
                      lpMemRes;             // Ptr to result   ...
    UBOOL             bRet = TRUE,          // TRUE iff result is valid
                      bRealOnly = FALSE,    // TRUE iff the args must be demoted to real
                      bRad0Only = FALSE;    // TRUE iff the args must be Radius zero only
    TOKEN             tkLftArg = {0},       // Left arg token
                      tkRhtArg = {0};       // Right ...
    IMM_TYPES         immType,              // Immediate type
                      immTypeRes;           // Result immediate type
    APLINT            apaOffLft,            // Left arg APA offset
                      apaMulLft,            // ...          multiplier
                      apaOffRht,            // Right arg APA offset
                      apaMulRht;            // ...           multiplier
    LPPL_YYSTYPE      lpYYFcnStrRht,        // Ptr to right operand function strand
                      lpYYRes = NULL;       // Ptr to the result
    LPPRIMFNS         lpPrimProtoRht;       // Ptr to right operand prototype function
    LPTOKEN           lptkAxisOpr,          // Ptr to operator axis token (may be NULL)
                      lptkAxisRht;          // Ptr to right operand axis token (may be NULL)
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    UBOOL             bDimDemote = FALSE;   // TRUE iff dimension demotion allowed
    LPPRIMSPEC        lpPrimSpec;           // Ptr to local PRIMSPEC
    LPPRIMFLAGS       lpPrimFlagsRht;       // Ptr to right operand PrimFlags entry
    ALLTYPES          atLft = {0},          // Left arg as ALLTYPES
                      atRht = {0};          // Right ...

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for operator axis token
    lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive to
    //   the axis operator, so signal a syntax error if present
    //***************************************************************
    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Set ptr to right operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = GetMonLftOper (lpYYFcnStrOpr, lptkAxisOpr); Assert (lpYYFcnStrRht NE NULL);

    // Ensure the right operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     || IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_OPERAND_SYNTAX_EXIT;

    // The result NELM is the product of the left & right NELMs
    aplNELMRes = imul64 (aplNELMLft, aplNELMRht, &bRet, EXCEPTION_RESULT_FLOAT);
    if (!bRet)
        goto WSFULL_EXIT;

    // Check for right operand axis token
    lptkAxisRht = CheckAxisOper (lpYYFcnStrRht);

    // Handle prototypes separately
    if (IsEmpty (aplNELMRes)
     || bPrototyping)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoRht = GetPrototypeFcnPtr (GetLftFcnTkn (lpYYFcnStrRht));
        if (lpPrimProtoRht EQ NULL)
            goto RIGHT_OPERAND_NONCE_EXIT;
    } else
        lpPrimProtoRht = NULL;

    // Get a ptr to the Primitive Function Flags
    lpPrimFlagsRht = GetPrimFlagsPtr (GetLftFcnTkn (lpYYFcnStrRht));

    // The rank of the result is the sum of the left & right ranks
    aplRankRes = aplRankLft + aplRankRht;

    // If the function is scalar dyadic,
    //   and there's no right operand axis,
    //   and both args are simple non-hetero, ...
    if (lpPrimFlagsRht->bDydScalar
     && lptkAxisRht EQ NULL
     && IsSimpleNHGlbNum (aplTypeLft)
     && IsSimpleNHGlbNum (aplTypeRht))
    {
        // Get the corresponding lpPrimSpec
        lpPrimSpec = PrimSpecTab[SymTrans (&lpYYFcnStrRht->tkToken)];

        // Save the dimension demotion flag
        bDimDemote = lpPrimSpec->bDydDimDemote;

        // Calculate the storage type of the result
        aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                                   &aplTypeLft,
                                                   &lpYYFcnStrRht->tkToken,
                                                    aplNELMRht,
                                                   &aplTypeRht);
        if (IsRealType (aplTypeRes))
        {
            // Calculate the left arg base storage type
            aplTypeLft2 = aToSimple[aplTypeLft];

            // Calculate the right arg base storage type
            aplTypeRht2 = aToSimple[aplTypeRht];

            // Set the result storage type to the base type
            aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                                       &aplTypeLft2,
                                                       &lpYYFcnStrRht->tkToken,
                                                        aplNELMRht,
                                                       &aplTypeRht2);
            // Mark as requiring type demotion of one or both args
            bRealOnly = TRUE;
        } else
        if (IsRad0Type (aplTypeRes))
            // Mark as requiring Radius 0 args only
            bRad0Only = TRUE;
        else
        if (IsNonceType (aplTypeRes))
            goto NONCE_EXIT;
        else
        if (IsErrorType (aplTypeRes))
            goto DOMAIN_EXIT;
        // For the moment, APA is treated as INT
        if (IsSimpleAPA (aplTypeRes))
            aplTypeRes = ARRAY_INT;
    } else
        // The result storage type is assumed to be NESTED,
        //   but we'll call TypeDemote at the end just in case.
        aplTypeRes = ARRAY_NESTED;

    // Save as immediate type
    immTypeRes = TranslateArrayTypeToImmType (aplTypeRes);
RESTART_JOTDOT:
    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If one or both args must be Radius 0 only, ...
    if (bRad0Only)
    {
        int      iHCDimLft,         // HC Dimension (1, 2, 4, 8)
                 iHCDimRht;         // ...
        APLNELM  aplNELMHCLft,      // Loop limit
                 aplNELMHCRht,      // ...
                 uLft,              // Loop counter
                 uRht;              // ...
        LPAPLARB lpMemLft,          // Ptr to left arg global memory data
                 lpMemRht;          // ...    right ...

        // If the left arg is BAxF, ...
        if (IsAnyArb (aplTypeLft))
        {
            // Calculate the HC Dimension (1, 2, 4, 8)
            iHCDimLft = TranslateArrayTypeToHCDim (aplTypeLft),

            // Calculate # parts in the args
            aplNELMHCLft = aplNELMLft * iHCDimLft;

            // If the left arg is an immediate, ...
            if (hGlbLft EQ NULL)
                // Point to the data
                lpMemLft = (LPAPLARB) &aplLongestLft;
            else
                // Skip over the header and dimensions to the data
                lpMemLft = VarArrayDataFmBase (lpMemHdrLft);

            // Loop through the elements of the left arg
            for (uLft = 0; uLft < aplNELMHCLft; uLft++)
            // If the radius is non-zero, ...
            if (!mag_is_zero (arb_radref (&lpMemLft[uLft])))
                goto LEFT_DOMAIN_EXIT;
        } // End IF

        // If the right arg is BAxF, ...
        if (IsAnyArb (aplTypeRht))
        {
            // Calculate the HC Dimension (1, 2, 4, 8)
            iHCDimRht = TranslateArrayTypeToHCDim (aplTypeRht),

            // Calculate # parts in the args
            aplNELMHCRht = aplNELMRht * iHCDimRht;

            // If the right arg is an immediate, ...
            if (hGlbRht EQ NULL)
                // Point to the data
                lpMemRht = (LPAPLARB) &aplLongestRht;
            else
                // Skip over the header and dimensions to the data
                lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

            // Loop through the elements of the right arg
            for (uRht = 0; uRht < aplNELMHCRht; uRht++)
            // If the radius is non-zero, ...
            if (!mag_is_zero (arb_radref (&lpMemRht[uRht])))
                goto RIGHT_DOMAIN_EXIT;
        } // End IF

        // Set the new result storage type
        aplTypeRes = ARRAY_ARB + (aplTypeRes - ARRAY_RAD0RONLY);

        // Turn off the flag
        bRad0Only = FALSE;
    } // End IF

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result.
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // If one or both args must be demoted, ...
    if (bRealOnly)
    {
        // If the left arg is HC, ...
        if (IsHCAny (aplTypeLft))
        {
            // Allocate new and Demote the left arg
            hGlbLft2 = AllocateDemote (aplTypeLft2,         // Base storage type
                                       hGlbLft,             // Left arg global memory handle (may be NULL)
                                       NULL,                // Ptr to ALLTYPES values (may be NULL)
                                       aplTypeLft,          // ... storage type
                                       aplNELMLft,          // ... NELM
                                       aplRankLft,          // ... rank
                                      &bRet);               // TRUE iff the result is not a WS FULL
            // We no longer need this ptr
            MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;

            // Check for error
            if (!bRet)
                goto LEFT_DOMAIN_EXIT;
            if (hGlbLft2 EQ NULL)
                goto WSFULL_EXIT;
            // Save as the new global handle and storage type
            hGlbLft    = hGlbLft2;
            aplTypeLft = aplTypeLft2;

            // Lock the memory to get a ptr to it
            lpMemHdrLft = MyGlobalLockVar (hGlbLft);

            // Note that we need to delete the left arg (hGlbLft2) on exit
        } // End IF

        // If the right arg is HC, ...
        if (IsHCAny (aplTypeRht))
        {
            // Allocate new and Demote the right arg
            hGlbRht2 = AllocateDemote (aplTypeRht2,         // Base storage type
                                       hGlbRht,             // Right arg global memory handle (may be NULL
                                       NULL,                // Ptr to ALLTYPES values (may be NULL)
                                       aplTypeRht,          // ... storage type
                                       aplNELMRht,          // ... NELM
                                       aplRankRht,          // ... rank
                                      &bRet);               // TRUE iff the result is not a WS FULL
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

            // Check for error
            if (!bRet)
                goto RIGHT_DOMAIN_EXIT;
            if (hGlbRht2 EQ NULL)
                goto WSFULL_EXIT;
            // Save as the new global handle and storage type
            hGlbRht    = hGlbRht2;
            aplTypeRht = aplTypeRht2;

            // Lock the memory to get a ptr to it
            lpMemHdrRht = MyGlobalLockVar (hGlbRht);

            // Note that we need to delete the right arg (hGlbRht2) on exit
        } // End IF
    } // End IF

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemHdrRes);

    //***************************************************************
    // Copy the dimensions from the left arg
    //   to the result's dimension
    //***************************************************************

    // If the left arg is not immediate, ...
    if (lpMemHdrLft NE NULL)
    {
        // Skip over the header to the dimensions
        lpMemLft = VarArrayBaseToDim (lpMemHdrLft);

        for (uRes = 0; uRes < aplRankLft; uRes++)
            // Copy the left arg dimensions
            *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemLft)++;
    } else
    {
        tkLftArg.tkFlags.TknType = TKT_VARIMMED;
        GetFirstValueToken (lptkLftArg,                 // Ptr to left arg token
                            NULL,                       // Ptr to integer result
                            NULL,                       // Ptr to float ...
                            NULL,                       // Ptr to WCHAR ...
                           &tkLftArg.tkData.tkLongest,  // Ptr to longest ...
                            NULL,                       // Ptr to lpSym/Glb ...
                           &immType,                    // Ptr to ...immediate type ...
                            NULL);                      // Ptr to array type ...
        tkLftArg.tkFlags.ImmType = immType;
        lpMemLft = GetPtrTknLongest (&tkLftArg);
    } // End IF/ELSE

    //***************************************************************
    // lpMemLft now points to the left arg's data
    //***************************************************************

    //***************************************************************
    // Copy the dimensions from the right arg
    //   to the result's dimension
    //***************************************************************

    // If the right arg is not immediate, ...
    if (lpMemHdrRht NE NULL)
    {
        // Skip over the header to the dimensions
        lpMemRht = VarArrayBaseToDim (lpMemHdrRht);

        for (uRes = 0; uRes < aplRankRht; uRes++)
            // Copy the right arg dimensions
            *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;
    } else
    {
        tkRhtArg.tkFlags.TknType = TKT_VARIMMED;
        GetFirstValueToken (lptkRhtArg,                 // Ptr to right arg token
                            NULL,                       // Ptr to integer result
                            NULL,                       // Ptr to float ...
                            NULL,                       // Ptr to WCHAR ...
                           &tkRhtArg.tkData.tkLongest,  // Ptr to longest ...
                            NULL,                       // Ptr to lpSym/Glb ...
                           &immType,                    // Ptr to ...immediate type ...
                            NULL);                      // Ptr to array type ...
        tkRhtArg.tkFlags.ImmType = immType;
        lpMemRht = GetPtrTknLongest (&tkRhtArg);
    } // End IF/ELSE

    //***************************************************************
    // lpMemRht now points to the right arg's data
    // lpMemRes now points to the result's data
    //***************************************************************

    // Fill in the arg tokens
    tkLftArg.tkCharIndex =
    tkRhtArg.tkCharIndex = lpYYFcnStrOpr->tkToken.tkCharIndex;

    // If the left arg is APA, fill in the offset and multiplier
    if (IsSimpleAPA (aplTypeLft))
    {
#define lpAPA       ((LPAPLAPA) lpMemLft)
        // Get the APA parameters
        apaOffLft = lpAPA->Off;
        apaMulLft = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // If the right arg is APA, fill in the offset and multiplier
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Take into account nested prototypes
    if (IsNested (aplTypeRht))
        aplNELMRht = max (aplNELMRht, 1);
    if (IsNested (aplTypeLft))
        aplNELMLft = max (aplNELMLft, 1);

    // Handle prototypes
    if (IsEmpty (aplNELMRes)
     && IsNested (aplTypeRes))
    {
        if (!PrimOpDydJotDotProto_EM (&lpMemRes,        // Ptr to ptr to result global memory
                                       lptkLftArg,      // Ptr to left arg tokwn
                                       lptkRhtArg,      // Ptr to right arg token
                                       lpYYFcnStrRht,   // Ptr to right operand function strand
                                       lpPrimProtoRht,  // Ptr to right operand prototype function
                                       lpbCtrlBreak))   // Ptr to Ctrl-Break flag
            goto ERROR_EXIT;
    } else
    // If the function is scalar dyadic,
    //   and there's no right operand axis,
    //   and both args are simple non-hetero or global numeric, ...
    if (lpPrimFlagsRht->bDydScalar
     && lptkAxisRht EQ NULL
     && IsSimpleNHGlbNum (aplTypeLft)
     && IsSimpleNHGlbNum (aplTypeRht))
    {
        TOKEN    tkRes = {0};
////    APLINT   aplIntegerLft,
////             aplIntegerRht;
////    APLFLOAT aplFloatLft,
////             aplFloatRht;
////    APLCHAR  aplCharLft,
////             aplCharRht;
        UINT     uBitIndex;             // Bit index for looping through Boolean result
////    HGLOBAL  lpSymGlbLft = NULL,    // Ptr to left arg global memory
////             lpSymGlbRht = NULL;    // ...    right ...
        LPVOID   lpMemSub;              // Ptr to temp global memory
        int      i,                     // Loop counter
                 iHCDimRes;             // HC Dimension (1, 2, 4, 8)

        // Initialize the bit index
        uBitIndex = 0;

        // Get the common storage type between the left & right args
        aplTypeCom = aTypePromote[aplTypeLft][aplTypeRht];

        // Loop through the left arg
        for (uLft = 0; uLft < aplNELMLft; uLft++)
        {
            // Promote the left arg to the common type
            (*aTypeActPromote[aplTypeLft][aplTypeCom]) (lpMemLft, uLft, &atLft);

            // Loop through the right arg
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Promote the right arg to the common type
                (*aTypeActPromote[aplTypeRht][aplTypeCom]) (lpMemRht, uRht, &atRht);

                // Compare the two items
                bRet =
                    PrimFnDydSiScSiScSub_EM (&tkRes,
                                              NULL,                     // Result as ALLTYPES (may be NULL)
                                             &lpYYFcnStrRht->tkToken,
                                              NULL,                     // Result global memory handle (may be NULL)
                                              aplTypeRes,
                                              aplTypeCom,               // NOT <aplTypeLft> as we converted it to <aplTypeCom> above
                                             &atLft,
                                              aplTypeCom,               // NOT <aplTypeRht> as we converted it to <aplTypeCom> above
                                             &atRht,
                                             &aplTypeNew,               // New storage type
                                              lpPrimSpec);
                // Free the old atRht
                (*aTypeFree[aplTypeCom]) (&atRht, 0);

                if (bRet)
                {
                    // Check for type promotion
                    if (aplTypeRes NE aTypePromote[aplTypeRes][aplTypeNew])
                    {
                        // Free the old atLft & atRht
                        (*aTypeFree[aplTypeCom]) (&atLft, 0);
                        (*aTypeFree[aplTypeCom]) (&atRht, 0);

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                        // We no longer need this resource
                        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

                        hGlbRes = GetGlbHandle (&tkRes);
                        if (hGlbRes NE NULL)
                        {
                            // We no longer need this resource
                            FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
                        } // End IF

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

                        // Save as the new immediate & storage types
                        immTypeRes = (IMM_TYPES) tkRes.tkFlags.ImmType;
                        aplTypeRes = aplTypeNew;

                        goto RESTART_JOTDOT;
                    } // End IF

                    // Calculate the HC Dimension (1, 2, 4, 8)
                    iHCDimRes = TranslateArrayTypeToHCDim (aplTypeRes);

                    // Split cases based upon the result storage type
                    switch (aplTypeRes)
                    {
                        case ARRAY_BOOL:
                            // Save in the result
                            *((LPAPLBOOL) lpMemRes) |= (BIT0 &tkRes.tkData.tkBoolean) << uBitIndex;

                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;              // Start over
                                ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                            } // End IF

                            break;

                        case ARRAY_INT:
                            // Save in the result
                            *((LPAPLINT)   lpMemRes)++ = tkRes.tkData.tkInteger;

                            break;

                        case ARRAY_HC2I:
                        case ARRAY_HC4I:
                        case ARRAY_HC8I:
                            // Lock the memory to get a ptr to it
                            lpMemHdrSub = MyGlobalLockVar (tkRes.tkData.tkGlbData);

                            // Skip over the header and dimensions to the data
                            lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                            // Loop through all of the parts
                            for (i = 0; i < iHCDimRes; i++)
                                // Save in the result
                                *((LPAPLINT)   lpMemRes)++ = *((LPAPLINT) lpMemSub)++;

                            // We no longer need this ptr
                            MyGlobalUnlock (tkRes.tkData.tkGlbData); lpMemHdrSub = NULL;

                            // We no longer need this storage
                            // Note that we reused the global numeric, so we don't
                            //   need to use FreeResultGlobalVar here
                            DbgGlobalFree (tkRes.tkData.tkGlbData); tkRes.tkData.tkGlbData = NULL;

                            break;

                        case ARRAY_FLOAT:
                            // Save in the result
                            *((LPAPLFLOAT) lpMemRes)++ = tkRes.tkData.tkFloat;

                            break;

                        case ARRAY_HC2F:
                        case ARRAY_HC4F:
                        case ARRAY_HC8F:
                            // Lock the memory to get a ptr to it
                            lpMemHdrSub = MyGlobalLockVar (tkRes.tkData.tkGlbData);

                            // Skip over the header and dimensions to the data
                            lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                            // Loop through all of the parts
                            for (i = 0; i < iHCDimRes; i++)
                                // Save in the result
                                *((LPAPLFLOAT)   lpMemRes)++ = *((LPAPLFLOAT) lpMemSub)++;

                            // We no longer need this ptr
                            MyGlobalUnlock (tkRes.tkData.tkGlbData); lpMemHdrSub = NULL;

                            // We no longer need this storage
                            // Note that we reused the global numeric, so we don't
                            //   need to use FreeResultGlobalVar here
                            DbgGlobalFree (tkRes.tkData.tkGlbData); tkRes.tkData.tkGlbData = NULL;

                            break;

                        case ARRAY_CHAR:
                            // Can this ever happen???

                            // Save in the result
                            *((LPAPLCHAR)  lpMemRes)++ = tkRes.tkData.tkChar;

                            break;

                        case ARRAY_RAT:
                        case ARRAY_HC2R:
                        case ARRAY_HC4R:
                        case ARRAY_HC8R:
                            // Lock the memory to get a ptr to it
                            lpMemHdrSub = MyGlobalLockVar (tkRes.tkData.tkGlbData);

                            // Skip over the header and dimensions to the data
                            lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                            // Loop through all of the parts
                            for (i = 0; i < iHCDimRes; i++)
                                // Save in the result
                                *((LPAPLRAT)   lpMemRes)++ = *((LPAPLRAT) lpMemSub)++;

                            // We no longer need this ptr
                            MyGlobalUnlock (tkRes.tkData.tkGlbData); lpMemHdrSub = NULL;

                            // We no longer need this storage
                            // Note that we reused the global numeric, so we don't
                            //   need to use FreeResultGlobalVar here
                            DbgGlobalFree (tkRes.tkData.tkGlbData); tkRes.tkData.tkGlbData = NULL;

                            break;

                        case ARRAY_VFP:
                        case ARRAY_HC2V:
                        case ARRAY_HC4V:
                        case ARRAY_HC8V:
                            // Lock the memory to get a ptr to it
                            lpMemHdrSub = MyGlobalLockVar (tkRes.tkData.tkGlbData);

                            // Skip over the header and dimensions to the data
                            lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                            // Loop through all of the parts
                            for (i = 0; i < iHCDimRes; i++)
                                // Save in the result
                                *((LPAPLVFP)   lpMemRes)++ = *((LPAPLVFP) lpMemSub)++;

                            // We no longer need this ptr
                            MyGlobalUnlock (tkRes.tkData.tkGlbData); lpMemHdrSub = NULL;

                            // We no longer need this storage
                            // Note that we reused the global numeric, so we don't
                            //   need to use FreeResultGlobalVar here
                            DbgGlobalFree (tkRes.tkData.tkGlbData); tkRes.tkData.tkGlbData = NULL;

                            break;

                        case ARRAY_ARB:
                        case ARRAY_BA2F:
                        case ARRAY_BA4F:
                        case ARRAY_BA8F:
                            // Lock the memory to get a ptr to it
                            lpMemHdrSub = MyGlobalLockVar (tkRes.tkData.tkGlbData);

                            // Skip over the header and dimensions to the data
                            lpMemSub = VarArrayDataFmBase (lpMemHdrSub);

                            // Loop through all of the parts
                            for (i = 0; i < iHCDimRes; i++)
                                // Save in the result
                                *((LPAPLARB)   lpMemRes)++ = *((LPAPLARB) lpMemSub)++;

                            // We no longer need this ptr
                            MyGlobalUnlock (tkRes.tkData.tkGlbData); lpMemHdrSub = NULL;

                            // We no longer need this storage
                            // Note that we reused the global numeric, so we don't
                            //   need to use FreeResultGlobalVar here
                            DbgGlobalFree (tkRes.tkData.tkGlbData); tkRes.tkData.tkGlbData = NULL;

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } else
                    goto ERROR_EXIT;
            } // End FOR

            // Free the old atLft
            (*aTypeFree[aplTypeCom]) (&atLft, 0);
        } // End FOR
    } else
    // Loop through the left & right args
    for (uLft = 0; uLft < aplNELMLft; uLft++)
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // If the left arg is not immediate, get the next value
        if (lpMemHdrLft NE NULL)
            // Get the next value from the left arg
            GetNextValueMemIntoToken (uLft,         // Index to use
                                      lpMemLft,     // Ptr to global memory object to index
                                      aplTypeLft,   // Storage type of the arg
                                      aplNELMLft,   // NELM         ...
                                      apaOffLft,    // APA offset (if needed)
                                      apaMulLft,    // APA multiplier (if needed)
                                     &tkLftArg);    // Ptr to token in which to place the result
        // If the right arg is not immediate, get the next value
        if (lpMemHdrRht NE NULL)
            // Get the next value from the right arg
            GetNextValueMemIntoToken (uRht,         // Index to use
                                      lpMemRht,     // Ptr to global memory object to index
                                      aplTypeRht,   // Storage type of the arg
                                      aplNELMRht,   // NELM         ...
                                      apaOffRht,    // APA offset (if needed)
                                      apaMulRht,    // APA multiplier (if needed)
                                     &tkRhtArg);    // Ptr to token in which to place the result
        bRet =
          ExecFuncOnToken_EM (&lpMemRes,            // Ptr to ptr to result global memory
                              &tkLftArg,            // Ptr to left arg token
                               lpYYFcnStrRht,       // Ptr to function strand
                              &tkRhtArg,            // Ptr to right arg token
                               lptkAxisRht,         // Ptr to right operand axis token (may be NULL)
                              &uValErrCnt,          // Ptr to VALUE ERROR counter
                               lpPrimProtoRht);     // Ptr to right operand prototype function
        // Free the left & right arg tokens
        if (lpMemHdrLft NE NULL)
            FreeResultTkn (&tkLftArg);
        if (lpMemHdrRht NE NULL)
            FreeResultTkn (&tkRhtArg);
        // If it failed, ...
        if (!bRet)
            goto ERROR_EXIT;
    } // End FOR/FOR

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

    // Check for VALUE ERROR
    if (uValErrCnt)
    {
        // Check for all VALUE ERRORs
        if (uValErrCnt EQ aplNELMRes)
        {
            // We no longer need this storage
            FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

            // Make a PL_YYSTYPE NoValue entry
            lpYYRes = MakeNoValue_YY (&lpYYFcnStrOpr->tkToken);

            goto NORMAL_EXIT;
        } else
            goto VALUE_EXIT;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken, bDimDemote);

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    goto ERROR_EXIT;

RIGHT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_OPERAND_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    return NULL;

VALUE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    return NULL;

ERROR_EXIT:
    // Is it valid?
    if (!IsErrorType (aplTypeCom))
    {
        // Free the old atLft and atRht
        (*aTypeFree[aplTypeCom]) (&atLft, 0);
        (*aTypeFree[aplTypeCom]) (&atRht, 0);
    } // End IF

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
    // If we allocated to demote data, ...
    if (hGlbLft2 NE NULL)
    {
        // We no longer need this resource
        FreeResultGlobalIncompleteVar (hGlbLft2); hGlbLft2 = NULL;
    } // End IF

    // If we allocated to demote data, ...
    if (hGlbRht2 NE NULL)
    {
        // We no longer need this resource
        FreeResultGlobalIncompleteVar (hGlbRht2); hGlbRht2 = NULL;
    } // End IF

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
} // End PrimOpDydJotDotCommon_EM_YY


//***************************************************************************
//  $PrimOpDydJotDotProto_EM
//
//  Handle prototypes in dyadic jotdot
//***************************************************************************

UBOOL PrimOpDydJotDotProto_EM
    (LPVOID      *lplpMemRes,                   // Ptr to ptr to result global memory
     LPTOKEN      lptkLftArg,                   // Ptr to left arg token
     LPTOKEN      lptkRhtArg,                   // Ptr to right arg token
     LPPL_YYSTYPE lpYYFcnStrRht,                // Ptr to function strand
     LPPRIMFNS    lpPrimProtoRht,               // Ptr to right operand prototype function
     LPUBOOL      lpbCtrlBreak)                 // Ptr to Ctrl-Break flag

{
    LPPL_YYSTYPE lpYYRes = NULL;                // Ptr to the result
    HGLOBAL      hGlbPro;                       // Prototype result global memory handle
    UBOOL        bRet;                          // TRUE iff the result is valid

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        return FALSE;

    // Execute the right operand between the left & right prototypes
    if (!ExecDydProto_EM (lptkLftArg,           // Ptr to left arg token
           (LPPL_YYSTYPE) lptkLftArg,           // Ptr to left operand function strand (for tkCharIndex only)
                          lptkRhtArg,           // Ptr to right arg token
                          lpYYFcnStrRht,        // Ptr to right operand function strand
                          lpPrimProtoRht,       // Ptr to right operand prototype function
                         &lpYYRes))             // Ptr ptr to to result
        goto ERROR_EXIT;

    // Get the global handle (if any) of the last calc
    hGlbPro = GetGlbHandle (&lpYYRes->tkToken);

    // If the result of the last calc is immediate, ...
    if (hGlbPro EQ NULL)
    {
        // Convert the immediate type and value in lpYYRes->tkToken
        //   into an LPSYMENTRY
        *(*(LPAPLNESTED *) lplpMemRes) =
          MakeSymEntry_EM (lpYYRes->tkToken.tkFlags.ImmType,
                          &lpYYRes->tkToken.tkData.tkLongest,
                           lptkRhtArg);
        if (*lplpMemRes EQ NULL)
            goto ERROR_EXIT;
    } else
        // Save the result into global memory
        *(*(LPAPLNESTED *) lplpMemRes) =
          MakePtrTypeGlb (hGlbPro);

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

ERROR_EXIT:
    if (lpYYRes)
        FreeResult (lpYYRes);

    // Mark as in error
    bRet = FALSE;
NORMAL_EXIT:
    // Free the result item (but not the storage)
    YYFree (lpYYRes); lpYYRes = NULL;

    return bRet;
} // End PrimOpDydJotDotProto_EM


//***************************************************************************
//  End of File: po_jotdot.c
//***************************************************************************