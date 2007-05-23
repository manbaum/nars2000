//***************************************************************************
//  NARS2000 -- Primitive Operator -- Dieresis
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  PrimOpDieresis_EM
//
//  Primitive operator for monadic and dyadic derived functions from Dieresis ("each" and "each")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDieresis_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpDieresis_EM
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    Assert (lpYYFcnStr->tkToken.tkData.tkChar EQ UTF16_DIERESIS);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonDieresis_EM (lpYYFcnStr,    // Ptr to operator function strand
                                     lptkRhtArg,    // Ptr to right arg token
                                     lptkAxis);     // Ptr to axis token (may be NULL)
    else
        return PrimOpDydDieresis_EM (lptkLftArg,    // Ptr to left arg token
                                     lpYYFcnStr,    // Ptr to operator function strand
                                     lptkRhtArg,    // Ptr to right arg token
                                     lptkAxis);     // Ptr to axis token (may be NULL)
} // End PrimOpDieresis_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimProtoOpDieresis_EM
//
//  Generate a prototype for the derived functions from
//    monadic operator Dieresis ("each" and "each")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoOpDieresis_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimProtoOpDieresis_EM
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    APLSTYPE  aplTypeLft,
              aplTypeRht,
              aplTypeRes;
    APLRANK   aplRankLft,
              aplRankRht,
              aplRankRes;
    APLNELM   aplNELMLft,
              aplNELMRht,
              aplNELMRes = 0,
              aplNELMAxis;
    HGLOBAL   hGlbLft = NULL,
              hGlbRht = NULL,
              hGlbRes = NULL,
              hGlbAxis = NULL,
              hGlbWVec = NULL,
              hGlbOdo = NULL,
              hGlbTmpRht;
    LPAPLUINT lpMemAxisHead = NULL,
              lpMemAxisTail = NULL,
              lpMemWVec = NULL,
              lpMemOdo = NULL;
    LPAPLINT  lpMemDimRes;
    LPVOID    lpMemLft = NULL,
              lpMemRht = NULL,
              lpMemRes = NULL;
    APLUINT   ByteRes,
              ByteAlloc,
              uLft,
              uRht,
              uRes,
              uArg;
    LPYYSTYPE lpYYRes = NULL,
              lpYYFcnStrLft;
    LPPRIMFNS lpPrimProtoLft;
    TOKEN     tkLftArg = {0},
              tkRhtArg = {0};
    UCHAR     immType;
    BOOL      bRet = TRUE;
    APLINT    apaOffLft,
              apaMulLft,
              apaOffRht,
              apaMulRht,
              iDim;

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStr[1 + (lptkAxis NE NULL)];

    // Get a ptr to the prototype function for the first symbol (a function or operator)
    lpPrimProtoLft = PrimProtoFnsTab[SymTrans (&lpYYFcnStrLft->tkToken)];
    if (!lpPrimProtoLft)
    {
        ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                  &lpYYFcnStrLft->tkToken);
        goto ERROR_EXIT;
    } // End IF

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // The result storage type is assumed to be NESTED,
    //   but we'll call TypeDemote at the end just in case.
    aplTypeRes = ARRAY_NESTED;

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
    {
        //***************************************************************
        // Called monadically
        //***************************************************************

        // Calculate the space needed for the result
        ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

        // Allocate space for the result.
        // N.B. Conversion from APLUINT to UINT.
        Assert (ByteRes EQ (UINT) ByteRes);
        hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (!hGlbRes)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                      &lpYYFcnStrLft->tkToken);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

        // Fill in the header
        lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = aplTypeRes;
////////lpHeader->Perm       = 0;   // Already zero from GHND
////////lpHeader->SysVar     = 0;   // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMRht;
        lpHeader->Rank       = aplRankRht;

#undef  lpHeader

        // Skip over the header to the dimensions
        lpMemRes = VarArrayBaseToDim (lpMemRes);
        lpMemRht = VarArrayBaseToDim (lpMemRht);

        //***************************************************************
        // Copy the dimensions from the right arg
        //   to the result's dimension
        //***************************************************************
        for (uRes = 0; uRes < aplRankRht; uRes++)
            *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;

        // lpMemRes now points to its data
        // lpMemRht now points to its data

        // Fill in with PTR_REUSED in case we fail
        *((LPAPLNESTED) lpMemRes) = PTR_REUSED;
        for (uRes = 1; uRes < aplNELMRht; uRes++)
            ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;

        // Check for axis operator
        if (lpYYFcnStrLft->FcnCount > 1
         && (lpYYFcnStrLft[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
          || lpYYFcnStrLft[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
            lptkAxis = &lpYYFcnStrLft[1].tkToken;
        else
            lptkAxis = NULL;

        // Take into account the nested prototype
        if (aplTypeRht EQ ARRAY_NESTED)
            aplNELMRht = max (aplNELMRht, 1);

        // Loop through the result
        for (uRes = 0; bRet && uRes < aplNELMRht; uRes++)
        {
            // Swap the right arg prototype with the right arg
            hGlbTmpRht = lptkRhtArg->tkData.tkGlbData;
            lptkRhtArg->tkData.tkGlbData = *(LPAPLNESTED) lpMemRht;

            // Execute the derived function from the operator
            bRet = ExecFuncOnToken (&lpMemRes,          // Ptr to output storage
                                     lptkLftArg,        // Ptr to left arg token
                                     lpYYFcnStrLft,     // Ptr to function strand
                                     lptkRhtArg,        // Ptr to right arg token
                                     lptkAxis,          // Ptr to axis token (may be NULL)
                                     lpPrimProtoLft);   // Ptr to primtive prototype function *may be NULL)
            // Restore the previous right arg
            lptkRhtArg->tkData.tkGlbData = hGlbTmpRht;
        } // End FOR

        // Check the result
        if (bRet)
        {
            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
            lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;
        } // End IF

        if (lpYYRes)
            goto NORMAL_EXIT;
        else
            goto ERROR_EXIT;
    } else
    {
        //***************************************************************
        // Called dyadically
        //***************************************************************

        // Get the attributes (Type, NELM, and Rank) of the left arg
        AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);

        // The rank of the result is the larger of the two args
        aplRankRes = max (aplRankLft, aplRankRht);

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
                return NULL;
            // Lock the memory to get a ptr to it
            lpMemAxisHead = MyGlobalLock (hGlbAxis);

            // Get pointer to the axis tail (where the [X] values are)
            lpMemAxisTail = &lpMemAxisHead[aplRankRes - aplNELMAxis];
        } else
            // No axis is the same as all axes
            aplNELMAxis = aplRankRes;

        // Get left arg's global ptrs
        GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);

        // Check for RANK and LENGTH ERRORs
        if (!CheckRankLengthError_EM (aplRankRes,
                                      aplRankLft,
                                      aplNELMLft,
                                      lpMemLft,         // Ptr to left arg memory sign.ature
                                      aplRankRht,
                                      aplNELMRht,
                                      lpMemRht,         // Ptr to right arg memory sign.ature
                                      aplNELMAxis,
                                      lpMemAxisTail,
                                     &lpYYFcnStrLft->tkToken))
            goto ERROR_EXIT;

        // Allocate space for result
        if (!PrimScalarFnDydAllocate_EM (&lpYYFcnStrLft->tkToken,
                                         &hGlbRes,
                                          lpMemLft,     // Ptr to left arg memory sign.ature
                                          lpMemRht,     // ...    right ...
                                         &lpMemRes,
                                          aplRankLft,
                                          aplRankRht,
                                         &aplRankRes,
                                          aplTypeRes,
                                          aplNELMLft,
                                          aplNELMRht,
                                          aplNELMRes))
            goto ERROR_EXIT;

        // Fill in the arg tokens
        tkLftArg.tkCharIndex =
        tkRhtArg.tkCharIndex = lpYYFcnStrLft->tkToken.tkCharIndex;

        // If the left arg is immediate, fill in the token
        if (lpMemLft EQ NULL)
        {
            tkLftArg.tkFlags.TknType = TKT_VARIMMED;
            FirstValue (lptkLftArg,                 // Ptr to right arg token
                        NULL,                       // Ptr to integer result
                        NULL,                       // Ptr to float ...
                        NULL,                       // Ptr to WCHAR ...
                       &tkLftArg.tkData.tkLongest,  // Ptr to longest ...
                        NULL,                       // Ptr to lpSym/Glb ...
                       &immType,                    // Ptr to ...immediate type ...
                        NULL);                      // Ptr to array type ...
            // Because this is a prototype, convert the immediate value to a zero or blank
            if (IsSimpleNum (aplTypeLft))
            {
                immType = IMMTYPE_BOOL;
                tkLftArg.tkData.tkBoolean = 0;
            } else
                tkLftArg.tkData.tkChar    = L' ';

            tkLftArg.tkFlags.ImmType = immType;
        } else
        // Otherwise, skip over the header and dimensions to the data
            lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

        // If the right arg is immediate, fill in the token
        if (lpMemRht EQ NULL)
        {
            tkRhtArg.tkFlags.TknType = TKT_VARIMMED;
            FirstValue (lptkRhtArg,                 // Ptr to right arg token
                        NULL,                       // Ptr to integer result
                        NULL,                       // Ptr to float ...
                        NULL,                       // Ptr to WCHAR ...
                       &tkRhtArg.tkData.tkLongest,  // Ptr to longest ...
                        NULL,                       // Ptr to lpSym/Glb ...
                       &immType,                    // Ptr to ...immediate type ...
                        NULL);                      // Ptr to array type ...
            // Because this is a prototype, convert the immediate value to a zero or blank
            if (IsSimpleNum (aplTypeRht))
            {
                immType = IMMTYPE_BOOL;
                tkRhtArg.tkData.tkBoolean = 0;
            } else
                tkRhtArg.tkData.tkChar    = L' ';

            tkRhtArg.tkFlags.ImmType = immType;
        } else
        // Otherwise, skip over the header and dimensions to the data
            lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

        // If the left arg is APA, fill in the offset and multiplier
        if (aplTypeLft EQ ARRAY_APA)
        {
#define lpAPA       ((LPAPLAPA) lpMemLft)
            apaOffLft = lpAPA->Off;
            apaMulLft = lpAPA->Mul;
#undef  lpAPA
        } // End IF

        // If the right arg is APA, fill in the offset and multiplier
        if (aplTypeRht EQ ARRAY_APA)
        {
#define lpAPA       ((LPAPLAPA) lpMemRht)
            apaOffRht = lpAPA->Off;
            apaMulRht = lpAPA->Mul;
#undef  lpAPA
        } // End IF

        // Skip over the header to the dimensions
        lpMemDimRes = VarArrayBaseToDim (lpMemRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

        // Handle axis if present
        if (aplNELMAxis NE aplRankRes)
        {
            //***************************************************************
            // Allocate space for the weighting vector which is
            //   {times}{backscan}1{drop}({rho}Z),1
            // N.B.  Conversion from APLUINT to UINT.
            //***************************************************************
            ByteAlloc = aplRankRes * sizeof (APLINT);
            Assert (ByteAlloc EQ (UINT) ByteAlloc);
            hGlbWVec = DbgGlobalAlloc (GHND, (UINT) ByteAlloc);
            if (!hGlbWVec)
            {
                ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                          &lpYYFcnStrLft->tkToken);
                goto ERROR_EXIT;
            } // End IF

            // Lock the memory to get a ptr to it
            lpMemWVec = MyGlobalLock (hGlbWVec);

            // Loop through the dimensions of the result in reverse
            //   order {backscan} and compute the cumulative product
            //   (weighting vector).
            // Note we use a signed index variable because we're
            //   walking backwards and the test against zero must be
            //   made as a signed variable.
            for (uRes = 1, iDim = aplRankRes - 1; iDim >= 0; iDim--)
            {
                lpMemWVec[iDim] = uRes;
                uRes *= lpMemDimRes[iDim];
            } // End FOR

            //***************************************************************
            // Allocate space for the odometer array, one value per dimension
            //   in the right arg, with values initially all zero (thanks to GHND).
            // N.B.  Conversion from APLUINT to UINT.
            //***************************************************************
            ByteAlloc = aplRankRes * sizeof (APLINT);
            Assert (ByteAlloc EQ (UINT) ByteAlloc);
            hGlbOdo = DbgGlobalAlloc (GHND, (UINT) ByteAlloc);
            if (!hGlbOdo)
            {
                ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                          &lpYYFcnStrLft->tkToken);
                goto ERROR_EXIT;
            } // End IF

            // Lock the global memory to get a ptr to it
            lpMemOdo = MyGlobalLock (hGlbOdo);
        } // End IF

        // If there's an axis, ...
        if (lptkAxis
         && aplNELMAxis NE aplRankRes)
        {
            // Loop through the odometer values accumulating
            //   the weighted sum
            for (uArg = 0, uRht = aplRankRes - aplNELMAxis; uRht < aplRankRes; uRht++)
                uArg += lpMemOdo[lpMemAxisHead[uRht]] * lpMemWVec[uRht];

            // Increment the odometer in lpMemOdo subject to
            //   the values in lpMemDimRes
            IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

            // Use the just computed index for the argument
            //   with the smaller rank
            if (aplRankLft < aplRankRht)
            {
                uLft = uArg;
                uRht = uRes;
            } else
            {
                uRht = uArg;
                uLft = uRes;
            } // End IF/ELSE
        } else
        {
            uLft = aplNELMLft ? (uRes % aplNELMLft) : 0;
            uRht = aplNELMRht ? (uRes % aplNELMRht) : 0;
        } // End IF/ELSE

        // If the left arg is not immediate, get the next value
        if (lpMemLft)
            // Get the next value from the left arg
            GetValueInToken (uLft, lpMemLft, aplTypeLft, apaOffLft, apaMulLft, &tkLftArg, TRUE);

        // If the right arg is not immediate, get the next value
        if (lpMemRht)
            // Get the next value from the right arg
            GetValueInToken (uRht, lpMemRht, aplTypeRht, apaOffRht, apaMulRht, &tkRhtArg, TRUE);

        // Execute the function on the arg token
        // Note that we cast the function strand to LPTOKEN
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        lpYYRes = (*lpPrimProtoLft) (&tkLftArg,         // Ptr to left arg token
                            (LPTOKEN) lpYYFcnStrLft,    // Ptr to function strand
                                     &tkRhtArg,         // Ptr to right arg token
                                      lptkAxis);        // Ptr to axis token (may be NULL)
        // If we copied an APLNESTED object in GetValueInToken, free it now
        if (lpMemLft
         && aplTypeLft EQ ARRAY_NESTED
         && GetPtrTypeDir (tkLftArg.tkData.tkGlbData) EQ PTRTYPE_HGLOBAL)
            FreeResult (&tkLftArg);

        if (lpMemRht
         && aplTypeRht EQ ARRAY_NESTED
         && GetPtrTypeDir (tkRhtArg.tkData.tkGlbData) EQ PTRTYPE_HGLOBAL)
            FreeResult (&tkRhtArg);

        // If it succeeded, ...
        if (lpYYRes)
        {
            // Split cases based upon the result token type
            switch (lpYYRes->tkToken.tkFlags.TknType)
            {
                case TKT_VARIMMED:
                    // Convert the immediate value into a symbol table constant,
                    //   and save into the result
                    if (lpYYRes->tkToken.tkFlags.ImmType EQ IMMTYPE_CHAR)
                        *((LPAPLNESTED) lpMemRes) = SymTabAppendChar_EM    (L' ');
                    else
                        *((LPAPLNESTED) lpMemRes) = SymTabAppendInteger_EM (0);
                    break;

                case TKT_VARARRAY:
                    // Copy (increment the reference count of) the global object,
                    //   and save into the result
                    *((LPAPLNESTED) lpMemRes) = CopySymGlbDir (lpYYRes->tkToken.tkData.tkGlbData);

                    break;

                defstop
                    break;
            } // End SWITCH

            // Free the result of the function execution
            FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;
        } else
            goto ERROR_EXIT;
    } // End IF/ELSE

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (TypeDemote (hGlbRes));

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

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbAxis)
    {
        if (lpMemAxisHead)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbAxis); lpMemAxisHead = lpMemAxisTail = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbAxis); hGlbAxis = NULL;
    } // End IF

    if (hGlbOdo)
    {
        if (lpMemOdo)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbOdo); lpMemOdo = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbOdo); hGlbOdo = NULL;
    } // End IF

    if (hGlbWVec)
    {
        if (lpMemWVec)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbWVec); lpMemWVec = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbWVec); hGlbWVec = NULL;
    } // End IF

    return lpYYRes;
} // End PrimProtoOpDieresis_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimOpMonDieresis_EM
//
//  Primitive operator for monadic derived function from Dieresis ("each")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonDieresis_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpMonDieresis_EM
    (LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    APLUINT   ByteRes;
    APLSTYPE  aplTypeRht,
              aplTypeRes;
    APLNELM   aplNELMRht;
    APLRANK   aplRankRht;
    HGLOBAL   hGlbRht,
              hGlbRes;
    LPVOID    lpMemRht,
              lpMemRes;
    APLUINT   uRht;
    LPYYSTYPE lpYYRes,
              lpYYRes2;
    APLINT    apaOff,
              apaMul;
    TOKEN     tkRhtArg = {0};
    UINT      uBitMask;
    BOOL      bRet = TRUE;
    LPYYSTYPE lpYYFcnStrLft;

    //***************************************************************
    // The derived function from this operator is not sensitive to
    //   the axis operator, so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis);
        return NULL;
    } // End IF

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStr[1 + (lptkAxis NE NULL)];

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    //***************************************************************
    //  Handle prototypes separately
    //***************************************************************
    if (aplNELMRht EQ 0)
        return PrimProtoOpDieresis_EM (NULL,        // No left arg token
                                       lpYYFcnStr,  // Ptr to operator function strand
                                       lptkRhtArg,  // Ptr to right arg token
                                       lptkAxis);   // Ptr to axis token (may be NULL)
    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Handle immediate right arg (lpMemRht is NULL)
    if (lpMemRht EQ NULL)
    {
        lpYYRes2 = ExecFuncStr_EM (NULL,            // No left arg token
                                   lpYYFcnStrLft,   // Ptr to left operand function strand
                                   lptkRhtArg);     // Ptr to right arg token
        if (lpYYRes2)
        {
            // Enclose the item
            lpYYRes = PrimFnMonLeftShoe_EM (&lpYYFcnStrLft->tkToken,    // Ptr to function token
                                            &lpYYRes2->tkToken,         // Ptr to right arg token
                                             NULL);                     // Ptr to LPPRIMSPEC
            FreeResult (&lpYYRes2->tkToken); YYFree (lpYYRes2); lpYYRes2 = NULL;
        } else
            lpYYRes = lpYYRes2;

        return lpYYRes;
    } // End IF

    // The result storage type is assumed to be NESTED,
    //   but we'll call TypeDemote at the end just in case.
    aplTypeRes = ARRAY_NESTED;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

    // Allocate space for the result
    // N.B. Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                  &lpYYFcnStrLft->tkToken);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header values
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;       // Already zero from GHND
////lpHeader->SysVar     = 0;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;

#undef  lpHeader

    // Copy the dimensions from the right arg to the result
    if (lpMemRht)
        CopyMemory (VarArrayBaseToDim (lpMemRes),
                    VarArrayBaseToDim (lpMemRht),
                    (UINT) aplRankRht * sizeof (APLDIM));

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRht);
    if (lpMemRht)
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Fill in the result with PTR_REUSED in case
    //   we fail at some intermediate point
    for (uRht = 0; uRht < aplNELMRht; uRht++)
        ((LPAPLNESTED) lpMemRes)[uRht] = PTR_REUSED;

    // Fill in the right arg token
    tkRhtArg.tkFlags.TknType   = TKT_VARIMMED;
////tkRhtArg.tkFlags.ImmType   =       // To be filled in below
////tkRhtArg.tkFlags.NoDisplay = 0;    // Already zero from = {0}
////tkRhtArg.tkData.tkLongest  =       // To be filled in below
    tkRhtArg.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;

    // Translate ARRAY_APA args to ARRAY_INT
    if (aplTypeRht EQ ARRAY_APA)
        tkRhtArg.tkFlags.ImmType = ARRAY_INT;
    else
        tkRhtArg.tkFlags.ImmType = aplTypeRht;

    // Split cases based upon the storage type of the right arg
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            // Initialize the bit mask
            uBitMask = 0x01;

            // Loop through the right arg
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Copy the value to the arg token
                tkRhtArg.tkData.tkBoolean = (uBitMask & *(LPAPLBOOL) lpMemRht) ? 1 : 0;

                // Shift over the bit mask
                uBitMask <<= 1;

                // Check for end-of-byte
                if (uBitMask EQ END_OF_BYTE)
                {
                    uBitMask = 0x01;            // Start over
                    ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                } // End IF

                // Execute the function on the arg token
                if (!ExecFuncOnToken (&lpMemRes,        // Ptr to output storage
                                       NULL,            // Ptr to left arg token
                                       lpYYFcnStrLft,   // Ptr to function strand
                                      &tkRhtArg,        // Ptr to right arg token
                                       NULL,            // Ptr to axis token
                                       NULL))           // Ptr to primtive prototype function
                    goto ERROR_EXIT;
            } // End FOR

            break;

        case ARRAY_INT:
            // Loop through the right arg
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Copy the value to the arg token
                tkRhtArg.tkData.tkInteger = *((LPAPLINT) lpMemRht)++;

                // Execute the function on the arg token
                if (!ExecFuncOnToken (&lpMemRes,        // Ptr to output storage
                                       NULL,            // Ptr to left arg token
                                       lpYYFcnStrLft,   // Ptr to function strand
                                      &tkRhtArg,        // Ptr to right arg token
                                       NULL,            // Ptr to axis token
                                       NULL))           // Ptr to primtive prototype function
                    goto ERROR_EXIT;
            } // End FOR

            break;

        case ARRAY_FLOAT:
            // Loop through the right arg
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Copy the value to the arg token
                tkRhtArg.tkData.tkFloat = *((LPAPLFLOAT) lpMemRht)++;

                // Execute the function on the arg token
                if (!ExecFuncOnToken (&lpMemRes,        // Ptr to output storage
                                       NULL,            // Ptr to left arg token
                                       lpYYFcnStrLft,   // Ptr to function strand
                                      &tkRhtArg,        // Ptr to right arg token
                                       NULL,            // Ptr to axis token
                                       NULL))           // Ptr to primtive prototype function
                    goto ERROR_EXIT;
            } // End FOR

            break;

        case ARRAY_CHAR:
            // Loop through the right arg
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Copy the value to the arg token
                tkRhtArg.tkData.tkChar = *((LPAPLCHAR) lpMemRht)++;

                // Execute the function on the arg token
                if (!ExecFuncOnToken (&lpMemRes,        // Ptr to output storage
                                       NULL,            // Ptr to left arg token
                                       lpYYFcnStrLft,   // Ptr to function strand
                                      &tkRhtArg,        // Ptr to right arg token
                                       NULL,            // Ptr to axis token
                                       NULL))           // Ptr to primtive prototype function
                    goto ERROR_EXIT;
            } // End FOR

            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemRht)
            apaOff = lpAPA->Off;
            apaMul = lpAPA->Mul;
#undef  lpAPA
        // Loop through the right arg
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Copy the value to the arg token
                tkRhtArg.tkData.tkInteger = apaOff + apaMul * uRht;

                // Execute the function on the arg token
                if (!ExecFuncOnToken (&lpMemRes,        // Ptr to output storage
                                       NULL,            // Ptr to left arg token
                                       lpYYFcnStrLft,   // Ptr to function strand
                                      &tkRhtArg,        // Ptr to right arg token
                                       NULL,            // Ptr to axis token
                                       NULL))           // Ptr to primtive prototype function
                    goto ERROR_EXIT;
            } // End FOR

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Loop through the right arg
            for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLHETERO) lpMemRht)++)
            {
                // Split cases based upon the ptr type
                switch (GetPtrTypeInd (lpMemRht))
                {
                    case PTRTYPE_STCONST:
                        // Set the token type
                        tkRhtArg.tkFlags.TknType = TKT_VARIMMED;

                        // Get the immediate type from the STE
                        tkRhtArg.tkFlags.ImmType = (*(LPSYMENTRY *) lpMemRht)->stFlags.ImmType;

                        // Copy the value to the arg token
                        tkRhtArg.tkData.tkLongest = (*(LPSYMENTRY *) lpMemRht)->stData.stLongest;

                        break;

                    case PTRTYPE_HGLOBAL:
                        // Set the token & immediate type
                        tkRhtArg.tkFlags.TknType = TKT_VARARRAY;
                        tkRhtArg.tkFlags.ImmType = 0;

                        // Copy the value to the arg token
                        tkRhtArg.tkData.tkGlbData = CopySymGlbInd ((LPAPLNESTED) lpMemRht);

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Execute the function on the arg token
                bRet = ExecFuncOnToken (&lpMemRes,      // Ptr to output storage
                                        NULL,           // Ptr to left arg token
                                        lpYYFcnStrLft,  // Ptr to function strand
                                       &tkRhtArg,       // Ptr to right arg token
                                        NULL,           // Ptr to axis token
                                        NULL);          // Ptr to primtive prototype function
                // Free the arg token
                FreeResult (&tkRhtArg);

                if (!bRet)
                    goto ERROR_EXIT;
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (TypeDemote (hGlbRes));
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;

    goto NORMAL_EXIT;

ERROR_EXIT:
    bRet = FALSE;
NORMAL_EXIT:
    if (lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (bRet)
        return lpYYRes;
    else
        return NULL;
} // End PrimOpMonDieresis_EM
#undef  APPEND_NAME


//***************************************************************************
//  ExecFuncOnToken
//
//  Execute a monadic or dyadic derived function strand
//    on a token or between tokens
//***************************************************************************

BOOL ExecFuncOnToken
    (LPVOID    *lplpMemRes,         // Ptr to ptr to result memory
     LPTOKEN    lptkLftArg,         // Ptr to left arg token
     LPYYSTYPE  lpYYFcnStr,         // Ptr to function strand
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis,           // Ptr to axis token (may be NULL)
     LPPRIMFNS  lpPrimProto)        // Ptr to primitive prototype function (may be NULL)

{
    LPYYSTYPE lpYYRes;

    // Execute the function on the arg token
    if (lpPrimProto)
        // Note that we cast the function strand to LPTOKEN
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        lpYYRes = (*lpPrimProto) (lptkLftArg,       // Ptr to left arg token
                        (LPTOKEN) lpYYFcnStr,       // Ptr to function strand
                                  lptkRhtArg,       // Ptr to right arg token
                                  lptkAxis);        // Ptr to axis token (may be NULL)
    else
        lpYYRes = ExecFuncStr_EM (lptkLftArg,       // Ptr to left arg token
                                  lpYYFcnStr,       // Ptr to function strand
                                  lptkRhtArg);      // Ptr to right arg token
    // If it succeeded, ...
    if (lpYYRes)
    {
        // Split cases based upon the result token type
        switch (lpYYRes->tkToken.tkFlags.TknType)
        {
            case TKT_VARIMMED:
                // Convert the immediate value into a symbol table constant,
                //   and save into the result
                *((LPAPLNESTED) *lplpMemRes)++ = MakeSymEntry_EM (lpYYRes->tkToken.tkFlags.ImmType,
                                                                 &lpYYRes->tkToken.tkData.tkLongest,
                                                                 &lpYYFcnStr->tkToken);
                break;

            case TKT_VARARRAY:
                // Copy (increment the reference count of) the global object,
                //   and save into the result
                *((LPAPLNESTED) *lplpMemRes)++ = CopySymGlbDir (lpYYRes->tkToken.tkData.tkGlbData);

                break;

            defstop
                break;
        } // End SWITCH

        // Free the result of the function execution
        FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;

        return TRUE;
    } // End IF

    return FALSE;
} // End ExecFuncOnToken


//***************************************************************************
//  GetValueInToken
//
//  Get the next value from a variable into a token
//***************************************************************************

void GetValueInToken
    (APLUINT  uArg,         // Index to use
     LPVOID   lpMemArg,     // Ptr to global memory object to index
     APLSTYPE aplTypeArg,   // Storage type of the arg
     APLINT   apaOff,       // APA offset (if needed)
     APLINT   apaMul,       // APA multiplier (if needed)
     LPTOKEN  lptkArg,      // Ptr to token in which to place the value
     BOOL     bProto)       // TRUE iff return prototype value

{
    // Split cases based upon the arg storage type
    switch (aplTypeArg)
    {
        case ARRAY_BOOL:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_BOOL;
            lptkArg->tkData.tkBoolean = bProto ? 0
                                               : BIT0 & (UCHAR) (((LPAPLBOOL) lpMemArg)[uArg >> LOG2NBIB] >> (uArg & MASKLOG2NBIB));
            break;

        case ARRAY_INT:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_INT;
            lptkArg->tkData.tkInteger = bProto ? 0
                                               : ((LPAPLINT) lpMemArg)[uArg];
            break;

        case ARRAY_FLOAT:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_FLOAT;
            lptkArg->tkData.tkFloat   = bProto ? 0
                                               : ((LPAPLFLOAT) lpMemArg)[uArg];
            break;

        case ARRAY_CHAR:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_CHAR;
            lptkArg->tkData.tkChar    = bProto ? L' '
                                               : ((LPAPLCHAR) lpMemArg)[uArg];
            break;

        case ARRAY_APA:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_INT;
            lptkArg->tkData.tkInteger = bProto ? 0
                                               : (apaOff + apaMul * uArg);
            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkData.tkGlbData = bProto ? CopySymGlbDir (((LPAPLNESTED) lpMemArg)[uArg])
                                               : ((LPAPLNESTED) lpMemArg)[uArg];
            break;

        defstop
            break;
    } // End SWITCH
} // End GetValueInToken


//***************************************************************************
//  PrimOpDydDieresis_EM
//
//  Primitive operator for dyadic derived function from Dieresis ("each")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydDieresis_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpDydDieresis_EM
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    APLSTYPE    aplTypeLft,             // Left arg storage type
                aplTypeRht,             // Right ...
                aplTypeRes;             // Result ...
    APLNELM     aplNELMLft,             // Left arg NELM
                aplNELMRht,             // Right ...
                aplNELMRes,             // Result ...
                aplNELMAxis;            // Axis ...
    APLRANK     aplRankLft,             // Left arg rank
                aplRankRht,             // Right ...
                aplRankRes;             // Result ...
    HGLOBAL     hGlbAxis = NULL,
                hGlbRes = NULL,
                hGlbRht = NULL,
                hGlbLft = NULL,
                hGlbWVec = NULL,
                hGlbOdo = NULL;
    LPAPLUINT   lpMemAxisHead = NULL,
                lpMemAxisTail = NULL,
                lpMemOdo = NULL,
                lpMemWVec = NULL;
    LPVOID      lpMemLft = NULL,
                lpMemRht = NULL,
                lpMemRes = NULL;
    LPAPLINT    lpMemDimRes;
    BOOL        bRet = TRUE;
    APLUINT     uLft,
                uRht,
                uRes,
                uArg,
                ByteAlloc;
    APLINT      apaOffLft,
                apaMulLft,
                apaOffRht,
                apaMulRht,
                iDim;
    UCHAR       immType;
    LPYYSTYPE   lpYYRes = NULL,
                lpYYFcnStrLft;
    TOKEN       tkLftArg = {0},
                tkRhtArg = {0};

    DBGENTER;

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStr[1 + (lptkAxis NE NULL)];

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // The rank of the result is the larger of the two args
    aplRankRes = max (aplRankLft, aplRankRht);

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
            return NULL;
        // Lock the memory to get a ptr to it
        lpMemAxisHead = MyGlobalLock (hGlbAxis);

        // Get pointer to the axis tail (where the [X] values are)
        lpMemAxisTail = &lpMemAxisHead[aplRankRes - aplNELMAxis];
    } else
        // No axis is the same as all axes
        aplNELMAxis = aplRankRes;

    // The result storage type is assumed to be NESTED,
    //   but we'll call TypeDemote at the end just in case.
    aplTypeRes = ARRAY_NESTED;

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Check for RANK and LENGTH ERRORs
    if (!CheckRankLengthError_EM (aplRankRes,
                                  aplRankLft,
                                  aplNELMLft,
                                  lpMemLft,         // Ptr to left arg memory sign.ature
                                  aplRankRht,
                                  aplNELMRht,
                                  lpMemRht,         // Ptr to right arg memory sign.ature
                                  aplNELMAxis,
                                  lpMemAxisTail,
                                 &lpYYFcnStr->tkToken))
        goto ERROR_EXIT;

    // The NELM of the result is the larger of the two args
    //   unless one is empty
    if (aplNELMLft EQ 0 || aplNELMRht EQ 0)
        aplNELMRes = 0;
    else
        aplNELMRes = max (aplNELMLft, aplNELMRht);

    //***************************************************************
    //  Handle prototypes separately
    //***************************************************************
    if (aplNELMRes EQ 0)
    {
        lpYYRes = PrimProtoOpDieresis_EM (lptkLftArg,   // Ptr to left arg token
                                          lpYYFcnStr,   // Ptr to operator function strand
                                          lptkRhtArg,   // Ptr to right arg token
                                          lptkAxis);    // Ptr to axis token (may be NULL)
        if (lpYYRes)
            goto NORMAL_EXIT;
        else
            goto ERROR_EXIT;
    } // End IF

    // Allocate space for result
    if (!PrimScalarFnDydAllocate_EM (&lpYYFcnStrLft->tkToken,
                                     &hGlbRes,
                                      lpMemLft,     // Ptr to left arg memory sign.ature
                                      lpMemRht,     // ...    right ...
                                     &lpMemRes,
                                      aplRankLft,
                                      aplRankRht,
                                     &aplRankRes,
                                      aplTypeRes,
                                      aplNELMLft,
                                      aplNELMRht,
                                      aplNELMRes))
        goto ERROR_EXIT;

    // Fill in the arg tokens
    tkLftArg.tkCharIndex =
    tkRhtArg.tkCharIndex = lpYYFcnStrLft->tkToken.tkCharIndex;

    // If the left arg is immediate, fill in the token
    if (lpMemLft EQ NULL)
    {
        tkLftArg.tkFlags.TknType = TKT_VARIMMED;
        FirstValue (lptkLftArg,                 // Ptr to right arg token
                    NULL,                       // Ptr to integer result
                    NULL,                       // Ptr to float ...
                    NULL,                       // Ptr to WCHAR ...
                   &tkLftArg.tkData.tkLongest,  // Ptr to longest ...
                    NULL,                       // Ptr to lpSym/Glb ...
                   &immType,                    // Ptr to ...immediate type ...
                    NULL);                      // Ptr to array type ...
        tkLftArg.tkFlags.ImmType = immType;
    } else
    // Otherwise, skip over the header and dimensions to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

    // If the right arg is immediate, fill in the token
    if (lpMemRht EQ NULL)
    {
        tkRhtArg.tkFlags.TknType = TKT_VARIMMED;
        FirstValue (lptkRhtArg,                 // Ptr to right arg token
                    NULL,                       // Ptr to integer result
                    NULL,                       // Ptr to float ...
                    NULL,                       // Ptr to WCHAR ...
                   &tkRhtArg.tkData.tkLongest,  // Ptr to longest ...
                    NULL,                       // Ptr to lpSym/Glb ...
                   &immType,                    // Ptr to ...immediate type ...
                    NULL);                      // Ptr to array type ...
        tkRhtArg.tkFlags.ImmType = immType;
    } else
    // Otherwise, skip over the header and dimensions to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // If the left arg is APA, fill in the offset and multiplier
    if (aplTypeLft EQ ARRAY_APA)
    {
#define lpAPA       ((LPAPLAPA) lpMemLft)
        apaOffLft = lpAPA->Off;
        apaMulLft = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // If the right arg is APA, fill in the offset and multiplier
    if (aplTypeRht EQ ARRAY_APA)
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Skip over the header to the dimensions
    lpMemDimRes = VarArrayBaseToDim (lpMemRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    // Handle axis if present
    if (aplNELMAxis NE aplRankRes)
    {
        //***************************************************************
        // Allocate space for the weighting vector which is
        //   {times}{backscan}1{drop}({rho}Z),1
        // N.B.  Conversion from APLUINT to UINT.
        //***************************************************************
        ByteAlloc = aplRankRes * sizeof (APLINT);
        Assert (ByteAlloc EQ (UINT) ByteAlloc);
        hGlbWVec = DbgGlobalAlloc (GHND, (UINT) ByteAlloc);
        if (!hGlbWVec)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                      &lpYYFcnStrLft->tkToken);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemWVec = MyGlobalLock (hGlbWVec);

        // Loop through the dimensions of the result in reverse
        //   order {backscan} and compute the cumulative product
        //   (weighting vector).
        // Note we use a signed index variable because we're
        //   walking backwards and the test against zero must be
        //   made as a signed variable.
        for (uRes = 1, iDim = aplRankRes - 1; iDim >= 0; iDim--)
        {
            lpMemWVec[iDim] = uRes;
            uRes *= lpMemDimRes[iDim];
        } // End FOR

        //***************************************************************
        // Allocate space for the odometer array, one value per dimension
        //   in the right arg, with values initially all zero (thanks to GHND).
        // N.B.  Conversion from APLUINT to UINT.
        //***************************************************************
        ByteAlloc = aplRankRes * sizeof (APLINT);
        Assert (ByteAlloc EQ (UINT) ByteAlloc);
        hGlbOdo = DbgGlobalAlloc (GHND, (UINT) ByteAlloc);
        if (!hGlbOdo)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                      &lpYYFcnStrLft->tkToken);
            goto ERROR_EXIT;
        } // End IF

        // Lock the global memory to get a ptr to it
        lpMemOdo = MyGlobalLock (hGlbOdo);
    } // End IF

    // Loop through the result
    for (uRes = 0; uRes < aplNELMRes; uRes++)
    {
        // If there's an axis, ...
        if (lptkAxis
         && aplNELMAxis NE aplRankRes)
        {
            // Loop through the odometer values accumulating
            //   the weighted sum
            for (uArg = 0, uRht = aplRankRes - aplNELMAxis; uRht < aplRankRes; uRht++)
                uArg += lpMemOdo[lpMemAxisHead[uRht]] * lpMemWVec[uRht];

            // Increment the odometer in lpMemOdo subject to
            //   the values in lpMemDimRes
            IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

            // Use the just computed index for the argument
            //   with the smaller rank
            if (aplRankLft < aplRankRht)
            {
                uLft = uArg;
                uRht = uRes;
            } else
            {
                uRht = uArg;
                uLft = uRes;
            } // End IF/ELSE
        } else
        {
            uLft = uRes % aplNELMLft;
            uRht = uRes % aplNELMRht;
        } // End IF/ELSE

        // If the left arg is not immediate, get the next value
        if (lpMemLft)
            // Get the next value from the left arg
            GetValueInToken (uLft, lpMemLft, aplTypeLft, apaOffLft, apaMulLft, &tkLftArg, FALSE);

        // If the right arg is not immediate, get the next value
        if (lpMemRht)
            // Get the next value from the right arg
            GetValueInToken (uRht, lpMemRht, aplTypeRht, apaOffRht, apaMulRht, &tkRhtArg, FALSE);

        if (!ExecFuncOnToken (&lpMemRes,        // Ptr to output storage
                              &tkLftArg,        // Ptr to left arg token
                               lpYYFcnStrLft,   // Ptr to function strand
                              &tkRhtArg,        // Ptr to right arg token
                               NULL,            // Ptr to axis token
                               NULL))           // Ptr to primtive prototype function
            goto ERROR_EXIT;
    } // End FOR

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (TypeDemote (hGlbRes));
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStr->tkToken.tkCharIndex;

    goto NORMAL_EXIT;

ERROR_EXIT:
    bRet = FALSE;

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
    if (hGlbWVec)
    {
        if (lpMemWVec)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbWVec); lpMemWVec = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbWVec); hGlbWVec = NULL;
    } // End IF

    if (hGlbOdo)
    {
        if (lpMemOdo)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbOdo); lpMemOdo = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbOdo); hGlbOdo = NULL;
    } // End IF

    if (hGlbAxis)
    {
        if (lpMemAxisHead)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbAxis); lpMemAxisHead = lpMemAxisTail = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbAxis); hGlbAxis = NULL;
    } // End IF

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

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

    // If we failed and there's a result, free it
    if (!bRet && lpYYRes)
    {
        YYFree (lpYYRes); lpYYRes = NULL;
    } // End IF/ELSE

    DBGLEAVE;

    return lpYYRes;
} // End PrimOpDydDieresis_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_dieresis.c
//***************************************************************************
