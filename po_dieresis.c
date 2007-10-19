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
#include "dfnhdr.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $PrimOpDieresis_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    monadic operator Dieresis ("each" and "each")
//***************************************************************************

LPPL_YYSTYPE PrimOpDieresis_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_DIERESIS);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (&lpYYFcnStrOpr->tkToken);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonDieresis_EM_YY (lpYYFcnStrOpr,  // Ptr to operator function strand
                                        lptkRhtArg);    // Ptr to right arg token
    else
        return PrimOpDydDieresis_EM_YY (lptkLftArg,     // Ptr to left arg token
                                        lpYYFcnStrOpr,  // Ptr to operator function strand
                                        lptkRhtArg);    // Ptr to right arg token
} // End PrimOpDieresis_EM_YY


//***************************************************************************
//  $PrimProtoOpDieresis_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator Dieresis ("each" and "each")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpDieresis_EM_YY
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
        return PrimOpMonDieresisCommon_EM_YY (lpYYFcnStrOpr,        // Ptr to operator function strand
                                              lptkRhtArg,           // Ptr to right arg token
                                              TRUE);                // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return PrimOpDydDieresisCommon_EM_YY (lptkLftArg,           // Ptr to left arg token
                                              lpYYFcnStrOpr,        // Ptr to operator function strand
                                              lptkRhtArg,           // Ptr to right arg token
                                              TRUE);                // TRUE iff prototyping
} // End PrimProtoOpDieresis_EM_YY


//***************************************************************************
//  $PrimOpMonDieresis_EM_YY
//
//  Primitive operator for monadic derived function from Dieresis ("each")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonDieresis_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpMonDieresisCommon_EM_YY (lpYYFcnStrOpr,        // Ptr to operator function strand
                                          lptkRhtArg,           // Ptr to right arg token
                                          FALSE);               // TRUE iff prototyping
} // End PrimOpDieresis_EM_YY


//***************************************************************************
//  $PrimOpMonDieresisCommon_EM_YY
//
//  Primitive operator for monadic derived function from Dieresis ("each")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonDieresisCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpMonDieresisCommon_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     BOOL         bPrototyping)         // TRUE iff protoyping

{
    APLUINT      ByteRes;           // # bytes in the result
    APLSTYPE     aplTypeRht,        // Right arg storage type
                 aplTypeRes;        // Result    ...
    APLNELM      aplNELMRht;        // Right arg NELM
    APLRANK      aplRankRht;        // Right arg rank
    HGLOBAL      hGlbRht,           // Right arg global memory handle
                 hGlbRes;           // Result    ...
    LPVOID       lpMemRht,          // Ptr to right arg global memory
                 lpMemRes;          // Ptr to result    ...
    APLUINT      uRht,              // Right arg loop counter
                 uRes;              // Result    ...
    LPPL_YYSTYPE lpYYRes = NULL,    // Ptr to the result
                 lpYYRes2;          // Ptr to secondary result
    APLINT       apaOff,            // Right arg APA offset
                 apaMul;            // ...           multiplier
    TOKEN        tkRhtArg = {0};    // Right arg token
    UINT         uBitMask;          // Bit mask for marching through Booleans
    BOOL         bRet = TRUE;       // TRUE iff result is valid
    LPPL_YYSTYPE lpYYFcnStrLft;     // Ptr to left operand function strand
    LPTOKEN      lptkAxis;          // Ptr to axis token (may be NULL)
    LPPRIMFNS    lpPrimProtoLft;    // Ptr to left operand prototype function (may be NULL if not prototyping)

    // Check for axis operator
    lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

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
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // The result storage type is assumed to be NESTED,
    //   but we'll call TypeDemote at the end just in case.
    aplTypeRes = ARRAY_NESTED;

    //***************************************************************
    //  Handle prototypes separately
    //***************************************************************
    if (aplNELMRht EQ 0
     || bPrototyping)
    {
        // Split cases baed upon the token type of the function strand's first item
        switch (lpYYFcnStrLft->tkToken.tkFlags.TknType)
        {
            case TKT_FCNIMMED:
            case TKT_OP1IMMED:
            case TKT_OP2IMMED:
            case TKT_OP3IMMED:
            case TKT_OPJOTDOT:
                // Get a ptr to the prototype function for the first symbol (a function or operator)
                lpPrimProtoLft = PrimProtoFnsTab[SymTrans (&lpYYFcnStrLft->tkToken)];
                if (!lpPrimProtoLft)
                {
                    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                              &lpYYFcnStrLft->tkToken);
                    goto ERROR_EXIT;
                } // End IF

                break;

            case TKT_FCNARRAY:
                // Get a ptr to the prototype function for the user-defined function/operator
                lpPrimProtoLft = ExecDfnGlbProto_EM_YY;

////////////////// Make sure the result is marked as Nested
////////////////aplTypeRes = ARRAY_NESTED;      // Set previously

                break;

            defstop
                break;
        } // End SWITCH
    } else
        lpPrimProtoLft = NULL;

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Handle immediate right arg (lpMemRht is NULL)
    if (lpMemRht EQ NULL)
    {
        if (lpPrimProtoLft)
            lpYYRes2 = (*lpPrimProtoLft) (NULL,             // Ptr to left arg token
                                (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand function strand
                                          lptkRhtArg,       // Ptr to right arg token
                                          lptkAxis);        // Ptr to axis token
        else
            lpYYRes2 = ExecFuncStr_EM_YY (NULL,             // No left arg token
                                          lpYYFcnStrLft,    // Ptr to left operand function strand
                                          lptkRhtArg,       // Ptr to right arg token
                                          lptkAxis);        // Ptr to axis token
        if (lpYYRes2)
        {
            // Enclose the item
            lpYYRes = PrimFnMonLeftShoe_EM_YY (&lpYYFcnStrLft->tkToken,     // Ptr to function token
                                               &lpYYRes2->tkToken,          // Ptr to right arg token
                                                NULL);                      // Ptr to LPPRIMSPEC
            FreeResult (&lpYYRes2->tkToken); YYFree (lpYYRes2); lpYYRes2 = NULL;
        } else
            lpYYRes = NULL;

        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // From here on, the right arg is an HGLOBAL
    //***************************************************************

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
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;       // Already zero from GHND
////lpHeader->SysVar     = 0;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Copy the dimensions from the right arg to the result
    CopyMemory (VarArrayBaseToDim (lpMemRes),
                VarArrayBaseToDim (lpMemRht),
                (UINT) aplRankRht * sizeof (APLDIM));

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRht);
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    if (aplTypeRes EQ ARRAY_NESTED)
    {
        // Fill nested result with PTR_REUSED
        //   in case we fail part way through
        *((LPAPLNESTED) lpMemRes) = PTR_REUSED;
        for (uRes = 1; uRes < aplNELMRht; uRes++)
            ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;
    } // End IF

    // Fill in the right arg token
    tkRhtArg.tkFlags.TknType   = TKT_VARIMMED;
////tkRhtArg.tkFlags.ImmType   =       // To be filled in below
////tkRhtArg.tkFlags.NoDisplay = 0;    // Already zero from = {0}
////tkRhtArg.tkData.tkLongest  =       // To be filled in below
    tkRhtArg.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;

    // Handle prototypes separately
    if (aplNELMRht EQ 0)
    {
        // Split cases based upon the storage type of the right arg
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_APA:
                tkRhtArg.tkFlags.ImmType = ARRAY_BOOL;
                tkRhtArg.tkData.tkBoolean = 0;

                // Execute the function on the arg token
                if (!ExecFuncOnToken_EM (&lpMemRes,             // Ptr to output storage
                                          NULL,                 // Ptr to left arg token
                                          lpYYFcnStrLft,        // Ptr to function strand
                                         &tkRhtArg,             // Ptr to right arg token
                                          NULL,                 // Ptr to axis token
                                          lpPrimProtoLft))      // Ptr to left operand prototype function (may be NULL)
                    goto ERROR_EXIT;
                break;

            case ARRAY_CHAR:
                tkRhtArg.tkFlags.ImmType = ARRAY_CHAR;
                tkRhtArg.tkData.tkBoolean = L' ';

                // Execute the function on the arg token
                if (!ExecFuncOnToken_EM (&lpMemRes,             // Ptr to output storage
                                          NULL,                 // Ptr to left arg token
                                          lpYYFcnStrLft,        // Ptr to function strand
                                         &tkRhtArg,             // Ptr to right arg token
                                          NULL,                 // Ptr to axis token
                                          lpPrimProtoLft))      // Ptr to left operand prototype function (may be NULL)
                    goto ERROR_EXIT;
                break;

            case ARRAY_NESTED:
            case ARRAY_HETERO:
                // Split cases based upon the ptr type
                switch (GetPtrTypeInd (lpMemRht))
                {
                    case PTRTYPE_STCONST:
                        // Set the token type
                        tkRhtArg.tkFlags.TknType = TKT_VARIMMED;

                        // Get the immediate type from the STE
                        tkRhtArg.tkFlags.ImmType = (*(LPAPLHETERO) lpMemRht)->stFlags.ImmType;

                        // Copy the value to the arg token
                        tkRhtArg.tkData.tkLongest = (*(LPAPLHETERO) lpMemRht)->stData.stLongest;

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
                bRet = ExecFuncOnToken_EM (&lpMemRes,           // Ptr to output storage
                                           NULL,                // Ptr to left arg token
                                           lpYYFcnStrLft,       // Ptr to function strand
                                          &tkRhtArg,            // Ptr to right arg token
                                           NULL,                // Ptr to axis token
                                           lpPrimProtoLft);     // Ptr to left operand prototype function (may be NULL)
                // Free the arg token
                FreeResult (&tkRhtArg);

                if (!bRet)
                    goto ERROR_EXIT;
                break;

            defstop
                break;
        } // End SWITCH
    } else
    {
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
                    if (!ExecFuncOnToken_EM (&lpMemRes,             // Ptr to output storage
                                              NULL,                 // Ptr to left arg token
                                              lpYYFcnStrLft,        // Ptr to function strand
                                             &tkRhtArg,             // Ptr to right arg token
                                              NULL,                 // Ptr to axis token
                                              lpPrimProtoLft))      // Ptr to left operand prototype function (may be NULL)
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
                    if (!ExecFuncOnToken_EM (&lpMemRes,             // Ptr to output storage
                                              NULL,                 // Ptr to left arg token
                                              lpYYFcnStrLft,        // Ptr to function strand
                                             &tkRhtArg,             // Ptr to right arg token
                                              NULL,                 // Ptr to axis token
                                              lpPrimProtoLft))      // Ptr to left operand prototype function (may be NULL)
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
                    if (!ExecFuncOnToken_EM (&lpMemRes,             // Ptr to output storage
                                              NULL,                 // Ptr to left arg token
                                              lpYYFcnStrLft,        // Ptr to function strand
                                             &tkRhtArg,             // Ptr to right arg token
                                              NULL,                 // Ptr to axis token
                                              lpPrimProtoLft))      // Ptr to left operand prototype function (may be NULL)
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
                    if (!ExecFuncOnToken_EM (&lpMemRes,             // Ptr to output storage
                                              NULL,                 // Ptr to left arg token
                                              lpYYFcnStrLft,        // Ptr to function strand
                                             &tkRhtArg,             // Ptr to right arg token
                                              NULL,                 // Ptr to axis token
                                              lpPrimProtoLft))      // Ptr to left operand prototype function (may be NULL)
                        goto ERROR_EXIT;
                } // End FOR

                break;

            case ARRAY_APA:
    #define lpAPA       ((LPAPLAPA) lpMemRht)
                // Get the APA parameters
                apaOff = lpAPA->Off;
                apaMul = lpAPA->Mul;
    #undef  lpAPA
            // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Copy the value to the arg token
                    tkRhtArg.tkData.tkInteger = apaOff + apaMul * uRht;

                    // Execute the function on the arg token
                    if (!ExecFuncOnToken_EM (&lpMemRes,             // Ptr to output storage
                                              NULL,                 // Ptr to left arg token
                                              lpYYFcnStrLft,        // Ptr to function strand
                                             &tkRhtArg,             // Ptr to right arg token
                                              NULL,                 // Ptr to axis token
                                              lpPrimProtoLft))      // Ptr to left operand prototype function (may be NULL)
                        goto ERROR_EXIT;
                } // End FOR

                break;

            case ARRAY_NESTED:
                // Take into account nested prototypes
                if (aplTypeRht EQ ARRAY_NESTED)
                    aplNELMRht = max (aplNELMRht, 1);
            case ARRAY_HETERO:
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
                            tkRhtArg.tkFlags.ImmType = (*(LPAPLHETERO) lpMemRht)->stFlags.ImmType;

                            // Copy the value to the arg token
                            tkRhtArg.tkData.tkLongest = (*(LPAPLHETERO) lpMemRht)->stData.stLongest;

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
                    bRet = ExecFuncOnToken_EM (&lpMemRes,           // Ptr to output storage
                                               NULL,                // Ptr to left arg token
                                               lpYYFcnStrLft,       // Ptr to function strand
                                              &tkRhtArg,            // Ptr to right arg token
                                               NULL,                // Ptr to axis token
                                               lpPrimProtoLft);     // Ptr to left operand prototype function (may be NULL)
                    // Free the arg token
                    FreeResult (&tkRhtArg);

                    if (!bRet)
                        goto ERROR_EXIT;
                } // End FOR

                break;

            defstop
                break;
        } // End SWITCH
    } // End IF/ELSE

    // Unlock the result global memory in case TypeDemote actually demotes
    if (hGlbRes && lpMemRes)
    {
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
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    lpYYRes->tkToken = *TypeDemote (&lpYYRes->tkToken);

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

    return lpYYRes;
} // End PrimOpMonDieresisCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecFuncOnToken_EM
//
//  Execute a monadic or dyadic derived function strand
//    on a token or between tokens
//***************************************************************************

BOOL ExecFuncOnToken_EM
    (LPVOID      *lplpMemRes,           // Ptr to ptr to result memory
     LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStr,           // Ptr to function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis,             // Ptr to axis token (may be NULL)
     LPPRIMFNS    lpPrimProto)          // Ptr to left operand prototype function (may be NULL)

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

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
        lpYYRes = ExecFuncStr_EM_YY (lptkLftArg,    // Ptr to left arg token
                                     lpYYFcnStr,    // Ptr to function strand
                                     lptkRhtArg,    // Ptr to right arg token
                                     lptkAxis);     // Ptr to axis token (may be NULL)
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
} // End ExecFuncOnToken_EM


//***************************************************************************
//  $PrimOpDydDieresis_EM_YY
//
//  Primitive operator for dyadic derived function from Dieresis ("each")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydDieresis_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpDydDieresisCommon_EM_YY (lptkLftArg,          // Ptr to left arg token
                                          lpYYFcnStrOpr,       // Ptr to operator function strand
                                          lptkRhtArg,          // Ptr to right arg token
                                          FALSE);               // TRUE iff prototyping
} // End PrimOpDydDieresis_EM_YY


//***************************************************************************
//  $PrimOpDydDieresisCommon_EM_YY
//
//  Primitive operator for dyadic derived function from Dieresis ("each")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydDieresisCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDydDieresisCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     BOOL         bPrototyping)         // TRUE iff prototyping

{
    APLSTYPE     aplTypeLft,            // Left arg storage type
                 aplTypeRht,            // Right ...
                 aplTypeRes;            // Result ...
    APLNELM      aplNELMLft,            // Left arg NELM
                 aplNELMRht,            // Right ...
                 aplNELMRes,            // Result ...
                 aplNELMAxis;           // Axis ...
    APLRANK      aplRankLft,            // Left arg rank
                 aplRankRht,            // Right ...
                 aplRankRes;            // Result ...
    HGLOBAL      hGlbLft = NULL,        // Left arg global memory handle
                 hGlbRht = NULL,        // Right ...
                 hGlbRes = NULL,        // Result   ...
                 hGlbAxis = NULL,       // Axis     ...
                 hGlbWVec = NULL,       // Weighting vector ...
                 hGlbOdo = NULL;        // Odometer ...
    LPAPLUINT    lpMemAxisHead = NULL,  // Ptr to axis values, fleshed out
                 lpMemAxisTail = NULL,  // Ptr to grade up of AxisHead
                 lpMemOdo = NULL,       // Ptr to odometer global memory
                 lpMemWVec = NULL;      // Ptr to weighting vector ...
    LPVOID       lpMemLft = NULL,       // Ptr to left arg global memory
                 lpMemRht = NULL,       // Ptr to right ...
                 lpMemRes = NULL;       // Ptr to result   ...
    LPAPLDIM     lpMemDimRes;           // Ptr to result dimensions
    BOOL         bRet = TRUE;           // TRUE iff result is valid
    APLUINT      uLft,                  // Left arg loop counter
                 uRht,                  // Right ...
                 uRes,                  // Result   ...
                 uArg,                  // Argument ...
                 ByteAlloc;             // # bytes to allocate
    APLINT       apaOffLft,             // Left arg APA offset
                 apaMulLft,             // ...          multiplier
                 apaOffRht,             // Right arg APA offset
                 apaMulRht,             // ...           multiplier
                 iDim;                  // Dimension loop counter
    UCHAR        immType;               // Immediate type
    LPPL_YYSTYPE lpYYRes = NULL,        // Ptr to the result
                 lpYYFcnStrLft;         // Ptr to left operand function strand
    TOKEN        tkLftArg = {0},        // Left arg token
                 tkRhtArg = {0};        // Right ...
    LPTOKEN      lptkAxis,              // Ptr to axis token (may be NULL)
                 lptkAxis2;             // Ptr to secondary axis token (may be NULL)
    LPPRIMFNS    lpPrimProtoLft;        // Ptr to left operand function strand (may be NULL if not prototyping)

    DBGENTER;

    // Check for axis operator
    lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

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
                                  lpMemLft,         // Ptr to left arg memory Sig.nature
                                  aplRankRht,
                                  aplNELMRht,
                                  lpMemRht,         // Ptr to right arg memory Sig.nature
                                  aplNELMAxis,
                                  lpMemAxisTail,
                                 &lpYYFcnStrOpr->tkToken))
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
    if (aplNELMRes EQ 0
     || bPrototyping)
    {
        // Split cases baed upon the token type of the function strand's first item
        switch (lpYYFcnStrLft->tkToken.tkFlags.TknType)
        {
            case TKT_FCNIMMED:
            case TKT_OP1IMMED:
            case TKT_OP2IMMED:
            case TKT_OP3IMMED:
            case TKT_OPJOTDOT:
                // Get a ptr to the prototype function for the first symbol (a function or operator)
                lpPrimProtoLft = PrimProtoFnsTab[SymTrans (&lpYYFcnStrLft->tkToken)];
                if (!lpPrimProtoLft)
                {
                    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                              &lpYYFcnStrLft->tkToken);
                    goto ERROR_EXIT;
                } // End IF

                break;

            case TKT_FCNARRAY:
                // Get a ptr to the prototype function for the user-defined function/operator
                lpPrimProtoLft = ExecDfnGlbProto_EM_YY;

////////////////// Make sure the result is marked as Nested
////////////////aplTypeRes = ARRAY_NESTED;      // Set previously

                break;

            defstop
                break;
        } // End SWITCH
    } else
        lpPrimProtoLft = NULL;

    // Allocate space for result
    if (!PrimScalarFnDydAllocate_EM (&lpYYFcnStrLft->tkToken,
                                     &hGlbRes,
                                      lpMemLft,     // Ptr to left arg memory Sig.nature
                                      lpMemRht,     // ...    right ...
                                      aplRankLft,
                                      aplRankRht,
                                     &aplRankRes,
                                      aplTypeRes,
                                      aplNELMLft,
                                      aplNELMRht,
                                      aplNELMRes))
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

    // Fill in the arg tokens
    tkLftArg.tkCharIndex =
    tkRhtArg.tkCharIndex = lpYYFcnStrLft->tkToken.tkCharIndex;

    // If the left arg is immediate, fill in the token
    if (lpMemLft EQ NULL)
    {
        tkLftArg.tkFlags.TknType = TKT_VARIMMED;
        FirstValue (lptkLftArg,                 // Ptr to left arg token
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

    // If the left arg is APA, ...
    if (aplTypeLft EQ ARRAY_APA)
    {
#define lpAPA       ((LPAPLAPA) lpMemLft)
        // Get the APA parameters
        apaOffLft = lpAPA->Off;
        apaMulLft = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // If the right arg is APA, ...
    if (aplTypeRht EQ ARRAY_APA)
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
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
        ByteAlloc = aplRankRes * sizeof (APLUINT);
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
        ByteAlloc = aplRankRes * sizeof (APLUINT);
        Assert (ByteAlloc EQ (UINT) ByteAlloc);
        hGlbOdo = DbgGlobalAlloc (GHND, (UINT) ByteAlloc);
        if (!hGlbOdo)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                      &lpYYFcnStrLft->tkToken);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemOdo = MyGlobalLock (hGlbOdo);
    } // End IF

    // Take into account nested prototypes
    if (aplTypeLft EQ ARRAY_NESTED)
        aplNELMLft = max (aplNELMLft, 1);
    if (aplTypeRht EQ ARRAY_NESTED)
        aplNELMRht = max (aplNELMRht, 1);
    if (aplTypeRes EQ ARRAY_NESTED)
        aplNELMRes = max (aplNELMRes, 1);

    // Check for secondary axis operator
    if (lpYYFcnStrLft->FcnCount > 1
     && (lpYYFcnStrLft[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
      || lpYYFcnStrLft[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
        lptkAxis2 = &lpYYFcnStrLft[1].tkToken;
    else
        lptkAxis2 = NULL;

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
            GetValueIntoToken (uLft,            // Index to use
                               lpMemLft,        // Ptr to global memory object to index
                               aplTypeLft,      // Storage type of the arg
                               apaOffLft,       // APA offset (if needed)
                               apaMulLft,       // APA multiplier (if needed)
                              &tkLftArg);       // Ptr to token in which to place the value
        // If the right arg is not immediate, get the next value
        if (lpMemRht)
            // Get the next value from the right arg
            GetValueIntoToken (uRht,            // Index to use
                               lpMemRht,        // Ptr to global memory object to index
                               aplTypeRht,      // Storage type of the arg
                               apaOffRht,       // APA offset (if needed)
                               apaMulRht,       // APA multiplier (if needed)
                              &tkRhtArg);       // Ptr to token in which to place the value
        // Execute the function strand between the left & right arg tokens
        if (!ExecFuncOnToken_EM (&lpMemRes,             // Ptr to output storage
                                 &tkLftArg,             // Ptr to left arg token
                                  lpYYFcnStrLft,        // Ptr to function strand
                                 &tkRhtArg,             // Ptr to right arg token
                                  lptkAxis2,            // Ptr to axis token
                                  lpPrimProtoLft))      // Ptr to left operand prototype function
            goto ERROR_EXIT;

        // Free the left & right arg tokens
        if (lpMemLft)
            FreeResult (&tkLftArg);
        if (lpMemRht)
            FreeResult (&tkRhtArg);
    } // End FOR

    // Unlock the result global memory in case TypeDemote actually demotes
    if (hGlbRes && lpMemRes)
    {
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
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    lpYYRes->tkToken = *TypeDemote (&lpYYRes->tkToken);

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

    // If we failed and there's a result, free it
    if (!bRet && lpYYRes)
    {
        YYFree (lpYYRes); lpYYRes = NULL;
    } // End IF/ELSE

    DBGLEAVE;

    return lpYYRes;
} // End PrimOpDydDieresis_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_dieresis.c
//***************************************************************************
