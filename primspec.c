//***************************************************************************
//  NARS2000 -- Primitive Function Special
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

#define PRIMPROTOFNSCALAR


//***************************************************************************
//  PrimFnSyntaxError_EM
//
//  Primitive function SYNTAX ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnSyntaxError_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnSyntaxError_EM
    (LPTOKEN lptkFunc)

{
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnSyntaxError_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnNonceError_EM
//
//  Primitive function NONCE ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnNonceError_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnNonceError_EM
    (LPTOKEN lptkFunc)

{
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnNonceError_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnValueError
//
//  Primitive function VALUE ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnValueError"
#else
#define APPEND_NAME
#endif

void PrimFnValueError
    (LPTOKEN lptkFunc)

{
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                               lptkFunc);
} // End PrimFnValueError
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonSyntaxError_EM
//
//  Primitive scalar monadic function SYNTAX ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonSyntaxError_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonSyntaxError_EM
    (LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis,           // Ptr to axis token (may be NULL)
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnMonSyntaxError_EM
#undef  APPEND_NAME


//// //***************************************************************************
//// //  PrimFnDydSyntaxError_EM
//// //
//// //  Primitive scalar dyadic function SYNTAX ERROR
//// //***************************************************************************
////
//// #ifdef DEBUG
//// #define APPEND_NAME     L" -- PrimFnDydSyntaxError_EM"
//// #else
//// #define APPEND_NAME
//// #endif
////
//// LPYYSTYPE PrimFnDydSyntaxError_EM
////     (LPTOKEN    lptkLftArg,         // Ptr to left arg token
////      LPTOKEN    lptkFunc,           // Ptr to function token
////      LPTOKEN    lptkRhtArg,         // Ptr to right arg token
////      LPTOKEN    lptkAxis,           // Ptr to axis token (may be NULL)
////      LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC
////
//// {
////     ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
////                                lptkFunc);
////     return NULL;
//// } // End PrimFnDydSyntaxError_EM
//// #undef  APPEND_NAME


//***************************************************************************
//  PrimProtoFnMixed_EM
//
//  Generate a prototype result for the monadic & dyadic primitive mixed functions
//***************************************************************************

LPYYSTYPE PrimProtoFnMixed_EM
    (LPPRIMFNS  lpPrimFn,           // Ptr to primitive function routine
     LPTOKEN    lptkLftArg,         // Ptr to left arg token
     LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis)           // Ptr to axis token (may be NULL)

{
    LPYYSTYPE lpYYRes;
    HGLOBAL   hGlbRes,
              hGlbResProto;

    // Call the original function
    lpYYRes = (*lpPrimFn) (lptkLftArg,      // Ptr to left arg token
                           lptkFunc,        // Ptr to function token
                           lptkRhtArg,      // Ptr to right arg token
                           lptkAxis);       // Ptr to axis token (may be NULL)
    if (lpYYRes)
    // As this is a prototype function, convert
    //   the result to a prototype
    // Split cases based upon the result token type
    switch (lpYYRes->tkToken.tkFlags.TknType)
    {
        case TKT_VARIMMED:
            if (lpYYRes->tkToken.tkFlags.ImmType EQ IMMTYPE_CHAR)
                lpYYRes->tkToken.tkData.tkChar = L' ';
            else
            {
                lpYYRes->tkToken.tkFlags.ImmType = IMMTYPE_BOOL;
                lpYYRes->tkToken.tkData.tkBoolean = 0;
            } // End IF/ELSE

            break;

        case TKT_VARARRAY:
            hGlbRes = ClrPtrTypeDirGlb (lpYYRes->tkToken.tkData.tkGlbData);

            // Make the prototype
            hGlbResProto = MakePrototype_EM (hGlbRes,   // Proto arg handle
                                             lptkFunc,  // Ptr to function token
                                             MP_CHARS); // CHARs allowed
            // Save back into the result
            lpYYRes->tkToken.tkData.tkGlbData =
              MakeGlbTypeGlb (hGlbResProto);

            // We no longer need this storage
            FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;

            break;

        defstop
            break;
    } // End IF/SWITCH

    return lpYYRes;
} // End PrimProtoFnMixed_EM
#undef  APPEND_NAME


#ifdef PRIMPROTOFNSCALAR
//***************************************************************************
//  PrimProtoFnScalar_EM
//
//  Generate a prototype result for the monadic & dyadic primitive scalar functions
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnScalar_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimProtoFnScalar_EM
    (LPTOKEN    lptkLftArg,         // Ptr to left arg token
     LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis)           // Ptr to axis token (may be NULL)

{
    HGLOBAL    hGlbLft = NULL,
               hGlbRht = NULL,
               hGlbRes = NULL,
               hGlbAxis = NULL;
    LPVOID     lpMemLft = NULL,
               lpMemRht = NULL;
    APLSTYPE   aplTypeLft,
               aplTypeRht,
               aplTypeRes;
    APLRANK    aplRankLft,
               aplRankRht,
               aplRankRes;
    APLNELM    aplNELMAxis,
               aplNELMLft,
               aplNELMRht;
    LPAPLUINT  lpMemAxisHead = NULL,
               lpMemAxisTail = NULL;
    LPYYSTYPE  lpYYRes = NULL;
    LPPRIMSPEC lpPrimSpec;
    BOOL       bBoolFn;

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
    {
        //***************************************************************
        // Called monadically
        //***************************************************************

        // Make the prototype
        hGlbRes = MakePrototype_EM (hGlbRht,    // Proto arg handle
                                    lptkFunc,   // Ptr to function token
                                    MP_NUMONLY);// Numeric only
    } else
    {
        //***************************************************************
        // Called dyadically
        //***************************************************************

        // Get the attributes (Type, NELM, and Rank)
        //   of the left & right args
        AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
        AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

        // Get left arg's global ptrs
        GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);

        // The rank of the result is the larger of the two args
        aplRankRes = max (aplRankLft, aplRankRht);

        // Check for axis present
        if (lptkAxis NE NULL)
        {
            // Check the axis values, fill in # elements in axis
            if (!CheckAxis_EM (lptkAxis,        // The axis token
                               aplRankRes,      // All values less than this
                               FALSE,           // TRUE iff scalar or one-element vector only
                               TRUE,            // TRUE iff want sorted axes
                               FALSE,           // TRUE iff axes must be contiguous
                               FALSE,           // TRUE iff duplicate axes are allowed
                               NULL,            // TRUE iff fractional values allowed
                               NULL,            // Return last axis value
                              &aplNELMAxis,     // Return # elements in axis vector
                              &hGlbAxis))       // Return HGLOBAL with APLINT axis values
                goto ERROR_EXIT;
            // Lock the memory to get a ptr to it
            lpMemAxisHead = MyGlobalLock (hGlbAxis);

            // Get pointer to the axis tail (where the [X] values are)
            lpMemAxisTail = &lpMemAxisHead[aplRankRes - aplNELMAxis];
        } else
            // No axis is the same as all axes
            aplNELMAxis = aplRankRes;

        // Get the corresponding lpPrimSpec
        lpPrimSpec = PrimSpecTab[SymTrans (lptkFunc)];

        // Calculate the storage type of the result
        aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                                   &aplTypeLft,
                                                    lptkFunc,
                                                    aplNELMRht,
                                                   &aplTypeRht);
        if (aplTypeRes EQ ARRAY_ERROR)
        {
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Get left arg's global handle (if any)
        hGlbLft = GetGlbHandle (lptkLftArg);

        // Check for RANK and LENGTH ERRORs
        if (!CheckRankLengthError_EM (aplRankRes,
                                      aplRankLft,
                                      aplNELMLft,
                                      lpMemLft,
                                      aplRankRht,
                                      aplNELMRht,
                                      lpMemRht,
                                      aplNELMAxis,
                                      lpMemAxisTail,
                                      lptkFunc))
            goto ERROR_EXIT;

        // ***FIXME***
        // Instead of returning one of the args as the result,
        //   we should create a result by trundling through
        //   the args using scalar extension as well as checking
        //   for RANK and LENGTH ERRORs between the args.
        // For example, adding empty vectors whose prototypes are
        //   0 (0 0) and (0 0) 0 should produce an empty vector
        //   whose prototype is (0 0) (0 0) by scalar extension.








        // By convention, use the prototype of the right arg
        //   if it is empty, the left arg if not
        hGlbRes = (aplNELMRht EQ 0) ? hGlbRht : hGlbLft;

        // Get the function type
        bBoolFn = lptkFunc->tkData.tkChar EQ UTF16_EQUAL
               || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL;

        // Make the prototype
        hGlbRes = MakePrototype_EM (hGlbRes,    // Proto arg handle
                                    lptkFunc,   // Ptr to function token
                                    bBoolFn ? MP_NUMCONV : MP_NUMONLY);
    } // End IF/ELSE

    if (!hGlbRes)
        goto ERROR_EXIT;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
ERROR_EXIT:
    if (lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    if (lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
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

    return lpYYRes;
} // End PrimProtoFnScalar_EM
#undef  APPEND_NAME
#endif


//***************************************************************************
//  PrimFnMon_EM
//
//  Primitive scalar monadic function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMon_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMon_EM
    (LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis,           // Ptr to axis token (may be NULL)
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    HGLOBAL  hGlbRes;
    APLSTYPE aplTypeRes,
             aplTypeRht;
    APLRANK  aplRankRht;
    APLNELM  aplNELMRht;
    LPYYSTYPE lpYYRes;

    DBGENTER;

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

#ifdef PRIMPROTOFNSCALAR
    // Handle prototypes separately
    if (aplNELMRht EQ 0)
        return PrimProtoFnScalar_EM (NULL,
                                     lptkFunc,
                                     lptkRhtArg,
                                     lptkAxis);
#endif
    // Get the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeMon) (aplNELMRht,
                                               &aplTypeRht,
                                                lptkFunc);
    // Check for DOMAIN ERROR
    if (aplTypeRes EQ ARRAY_ERROR)
    {
        // Mark as a DOMAIN ERROR
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                hGlbRes = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbRes));

                // Handle via subroutine
                hGlbRes = PrimFnMonGlb_EM (ClrPtrTypeDirGlb (hGlbRes),
                                           lptkFunc,
                                           lpPrimSpec);
                if (!hGlbRes)
                {
                    YYFree (lpYYRes); lpYYRes = NULL; return NULL;
                } // End IF

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
                lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

                DBGLEAVE;

                return lpYYRes;
            } // End IF

            // Handle the immediate case

            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType   = aplTypeRes;
////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkData            =        // Filled in below
            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
RESTART_EXCEPTION_VARNAMED:
            __try
            {
            // Split cases based upon the storage type of the result
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:            // Res = BOOL
                    // Split cases based upon the right arg's storage type
                    switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:  // Res = BOOL, Rht = BOOL
                            lpYYRes->tkToken.tkData.tkBoolean  =
                              (*lpPrimSpec->BisB) (lptkRhtArg->tkData.tkSym->stData.stBoolean,
                                                   lpPrimSpec);
                            break;

                        case IMMTYPE_INT:   // Res = BOOL, Rht = INT
                            lpYYRes->tkToken.tkData.tkBoolean  =
                              (*lpPrimSpec->BisI) (lptkRhtArg->tkData.tkSym->stData.stInteger,
                                                   lpPrimSpec);
                            break;

                        case IMMTYPE_FLOAT: // Res = BOOL, Rht = FLOAT
                            lpYYRes->tkToken.tkData.tkBoolean  =
                              (*lpPrimSpec->BisF) (lptkRhtArg->tkData.tkSym->stData.stFloat,
                                                   lpPrimSpec);
                            break;

                        defstop
                            return NULL;
                    } // End SWITCH

                    break;

                case ARRAY_INT:             // Res = INT
                    // Split cases based upon the right arg's storage type
                    switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:  // Res = INT, Rht = BOOL
                            lpYYRes->tkToken.tkData.tkInteger  =
                              (*lpPrimSpec->IisI) (lptkRhtArg->tkData.tkSym->stData.stBoolean,
                                                   lpPrimSpec);
                            break;

                        case IMMTYPE_INT:   // Res = INT, Rht = INT
                            lpYYRes->tkToken.tkData.tkInteger  =
                              (*lpPrimSpec->IisI) (lptkRhtArg->tkData.tkSym->stData.stInteger,
                                                   lpPrimSpec);
                            break;

                        case IMMTYPE_FLOAT: // Res = INT, Rht = FLOAT
                            lpYYRes->tkToken.tkData.tkInteger  =
                              (*lpPrimSpec->IisF) (lptkRhtArg->tkData.tkSym->stData.stFloat,
                                                   lpPrimSpec);
                            break;

                        defstop
                            return NULL;
                    } // End SWITCH

                    break;

                case ARRAY_FLOAT:           // Res = FLOAT
                    // Split cases based upon the right arg's storage type
                    switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:  // Res = FLOAT, Rht = BOOL
                            lpYYRes->tkToken.tkData.tkFloat  =
                              (*lpPrimSpec->FisI) (lptkRhtArg->tkData.tkSym->stData.stBoolean,
                                                   lpPrimSpec);
                            break;

                        case IMMTYPE_INT:   // Res = FLOAT, Rht = INT
                            lpYYRes->tkToken.tkData.tkFloat  =
                              (*lpPrimSpec->FisI) (lptkRhtArg->tkData.tkSym->stData.stInteger,
                                                   lpPrimSpec);
                            break;

                        case IMMTYPE_FLOAT: // Res = FLOAT, Rht = FLOAT
                            lpYYRes->tkToken.tkData.tkFloat  =
                              (*lpPrimSpec->FisF) (lptkRhtArg->tkData.tkSym->stData.stFloat,
                                                   lpPrimSpec);
                            break;

                        defstop
                            return NULL;
                    } // End SWITCH

                    break;

                defstop
                    return NULL;
            } // SWITCH
            } __except (CheckException (GetExceptionInformation (), lpPrimSpec))
            {
#ifdef DEBUG
                dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
#endif
                // Split cases based upon the ExecCode
                switch (*lpPrimSpec->lpExecCode)
                {
                    case EXEC_DOMAIN_ERROR:
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:
                        *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        YYFree (lpYYRes); lpYYRes = NULL; return NULL;

                    case EXEC_RESULT_FLOAT:
                        *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                        if (IsSimpleNum (aplTypeRes)
                         && aplTypeRes NE ARRAY_FLOAT)
                        {
                            aplTypeRes = ARRAY_FLOAT;
#ifdef DEBUG
                            dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
#endif
                            goto RESTART_EXCEPTION_VARNAMED;
                        } // End IF

                        // Fall through to never-never-land

                    defstop
                        break;
                } // End SWITCH
            } // End __try/__except

            DBGLEAVE;

            return lpYYRes;

        case TKT_VARIMMED:
RESTART_EXCEPTION_VARIMMED:
            __try
            {
            // Split cases based upon the result storage type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:            // Res = BOOL
                    // Split cases based upon the right arg's storage type
                    switch (lptkRhtArg->tkFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:  // Res = BOOL, Rht = BOOL
                            lpYYRes->tkToken.tkData.tkBoolean =
                              (*lpPrimSpec->BisB) (lptkRhtArg->tkData.tkBoolean,
                                                   lpPrimSpec);
                            break;

                        case IMMTYPE_INT:   // Res = BOOL, Rht = INT
                            lpYYRes->tkToken.tkData.tkBoolean =
                              (*lpPrimSpec->BisI) (lptkRhtArg->tkData.tkInteger,
                                                   lpPrimSpec);
                            break;

                        case IMMTYPE_FLOAT: // Res = BOOL, Rht = FLOAT
                            lpYYRes->tkToken.tkData.tkBoolean =
                              (*lpPrimSpec->BisF) (lptkRhtArg->tkData.tkFloat,
                                                   lpPrimSpec);
                            break;

                        defstop
                            return NULL;
                    } // End SWITCH

                    break;

                case ARRAY_INT:             // Res = INT
                    // Split cases based upon the right arg's storage type
                    switch (lptkRhtArg->tkFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:  // Res = INT, Rht = BOOL
                            lpYYRes->tkToken.tkData.tkInteger =
                              (*lpPrimSpec->IisI) (lptkRhtArg->tkData.tkBoolean,
                                                   lpPrimSpec);
                            break;

                        case IMMTYPE_INT:   // Res = INT, Rht = INT
                            lpYYRes->tkToken.tkData.tkInteger =
                              (*lpPrimSpec->IisI) (lptkRhtArg->tkData.tkInteger,
                                                   lpPrimSpec);
                            break;

                        case IMMTYPE_FLOAT: // Res = INT, Rht = FLOAT
                            lpYYRes->tkToken.tkData.tkInteger =
                              (*lpPrimSpec->IisF) (lptkRhtArg->tkData.tkFloat,
                                                   lpPrimSpec);
                            break;

                        defstop
                            return NULL;
                    } // End SWITCH

                    break;

                case ARRAY_FLOAT:           // Res = FLOAT
                    // Split cases based upon the right arg's storage type
                    switch (lptkRhtArg->tkFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:  // Res = FLOAT, Rht = BOOL
                            lpYYRes->tkToken.tkData.tkFloat   =
                              (*lpPrimSpec->FisI) (lptkRhtArg->tkData.tkBoolean,
                                                   lpPrimSpec);
                            break;

                        case IMMTYPE_INT:   // Res = FLOAT, Rht = INT
                            lpYYRes->tkToken.tkData.tkFloat   =
                              (*lpPrimSpec->FisI) (lptkRhtArg->tkData.tkInteger,
                                                   lpPrimSpec);
                            break;

                        case IMMTYPE_FLOAT: // Res = FLOAT, Rht = FLOAT
                            lpYYRes->tkToken.tkData.tkFloat   =
                              (*lpPrimSpec->FisF) (lptkRhtArg->tkData.tkFloat,
                                                   lpPrimSpec);
                            break;

                        defstop
                            return NULL;
                    } // End SWITCH

                    break;

                defstop
                    return NULL;
            } // End SWITCH
            } __except (CheckException (GetExceptionInformation (), lpPrimSpec))
            {
#ifdef DEBUG
                dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
#endif
                // Split cases based upon the ExecCode
                switch (*lpPrimSpec->lpExecCode)
                {
                    case EXEC_DOMAIN_ERROR:
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:
                        *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        YYFree (lpYYRes); lpYYRes = NULL; return NULL;

                    case EXEC_RESULT_FLOAT:
                        *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                        if (IsSimpleNum (aplTypeRes)
                         && aplTypeRes NE ARRAY_FLOAT)
                        {
                            aplTypeRes = ARRAY_FLOAT;
#ifdef DEBUG
                            dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
#endif
                            goto RESTART_EXCEPTION_VARIMMED;
                        } // End IF

                        // Fall through to never-never-land

                    defstop
                        break;
                } // End SWITCH
            } // End __try/__except

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType   = aplTypeRes;
////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkData            = (Filled in above)
            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

            DBGLEAVE;

            return lpYYRes;

        case TKT_VARARRAY:
            hGlbRes = lptkRhtArg->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbRes));

            // Handle via subroutine
            hGlbRes = PrimFnMonGlb_EM (ClrPtrTypeDirGlb (hGlbRes),
                                       lptkFunc,
                                       lpPrimSpec);
            if (!hGlbRes)
            {
                YYFree (lpYYRes); lpYYRes = NULL; return NULL;
            } // End IF

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

            DBGLEAVE;

            return lpYYRes;

        defstop
            break;
    } // End SWITCH

    DbgStop ();         // We should never get here

    YYFree (lpYYRes); lpYYRes = NULL; return NULL;
} // End PrimFnMon_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonGlb_EM
//
//  Primitive scalar monadic function on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonGlb_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL PrimFnMonGlb_EM
    (HGLOBAL    hGlbRht,            // Right arg handle
     LPTOKEN    lptkFunc,           // Ptr to function token
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    LPVOID   lpMemRes = NULL,
             lpMemRht = NULL;
    HGLOBAL  hGlbRes = NULL,
             hGlbSub;
    APLSTYPE aplTypeRht,        // The array storage type (see enum ARRAY_TYPES)
             aplTypeRes;
    APLNELM  aplNELMRht;        // # elements in the array
    APLRANK  aplRankRht;        // The rank of the array
    APLINT   uRes,
             apaOffRht,
             apaMulRht;
    APLUINT  ByteRes;
    BOOL     bRet = TRUE;
    UINT     uBitIndex;

    DBGENTER;

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)

    // Save the Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;

    // Get the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeMon) (aplNELMRht,
                                               &aplTypeRht,
                                                lptkFunc);
    // In case StorageTypeMon changed the value of aplTypeRht,
    //   save it back into the array
    lpHeader->ArrType = aplTypeRht;

#undef  lpHeader

    // Check for DOMAIN ERROR
    if (aplTypeRes EQ ARRAY_ERROR)
    {
        // Mark as a DOMAIN ERROR
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // In case the result is APA
    if (aplTypeRes EQ ARRAY_APA)
    {
        if (!(*lpPrimSpec->ApaResultMon_EM) (NULL,
                                             lptkFunc,
                                             hGlbRht,
                                            &hGlbRes,
                                            &lpMemRes,
                                             aplRankRht,
                                             lpPrimSpec))
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } // End IF

    // Calculate space for the result
RESTART_EXCEPTION:
    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

    // Allocate space for the result.
    // N.B. Conversion from APLUINT to UINT.
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

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;   // Already zero from GHND
////lpHeader->SysVar     = 0;   // Already zero from GHND
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
    for (uRes = 0; uRes < (APLRANKSIGN) aplRankRht; uRes++)
        *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;

    // lpMemRes now points to its data
    // lpMemRht now points to its data

#ifndef PRIMPROTOFNSCALAR
    // Handle prototypes separately
    if (aplNELMRht EQ 0)
    {
        HGLOBAL hGlbProto;

        // In case we fail
        *((LPAPLNESTED) lpMemRes) = PTR_REUSED;

        // Make the prototype
        hGlbProto = MakePrototype_EM (hGlbRht,          // Proto arg handle
                                      lptkFunc,         // Ptr to function token
                                      MP_NUMONLY);      // Numerics only
        if (!hGlbProto)
            goto ERROR_EXIT;

        // Save the handle
        *((LPAPLNESTED) lpMemRes) = MakeGlbTypeGlb (hGlbProto);

        goto NORMAL_EXIT;
    } // End IF
#endif

    // If the right arg is an APA, get its parameters
    if (aplTypeRht EQ ARRAY_APA)
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // In case the right arg is Boolean
    uBitIndex = 0;

    __try
    {
    // Split cases based upon the storage type of the result
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:            // Res = BOOL
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:    // Res = BOOL, Rht = BOOL
                    // ***FIXME*** -- Optimize by chunking

                    // Loop through the right arg/result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                    {
                        *((LPAPLBOOL) lpMemRes) |=
                          (*lpPrimSpec->BisB) ((APLBOOL) (BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex)),
                                               lpPrimSpec) << uBitIndex;

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                            ((LPAPLBOOL) lpMemRes)++;   // ...
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:     // Res = BOOL, Rht = INT
                    // Loop through the right arg/result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                    {
                        *((LPAPLBOOL) lpMemRes) |=
                          (*lpPrimSpec->BisI) (*((LPAPLINT) lpMemRht)++,
                                               lpPrimSpec) << uBitIndex;

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_APA:     // Res = BOOL, Rht = APA
                    // Loop through the right arg/result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                    {
                        *((LPAPLBOOL) lpMemRes) |=
                          (*lpPrimSpec->BisI) (apaOffRht + apaMulRht * uRes,
                                               lpPrimSpec) << uBitIndex;

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_FLOAT:   // Res = BOOL, Rht = FLOAT
                    // Loop through the right arg/result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                    {
                        *((LPAPLBOOL) lpMemRes) |=
                          (*lpPrimSpec->BisF) (*((LPAPLFLOAT) lpMemRht)++,
                                               lpPrimSpec) << uBitIndex;

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        case ARRAY_INT:             // Res = INT
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:    // Res = INT, Rht = BOOL
                    // Loop through the right arg/result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                    {
                        *((LPAPLINT) lpMemRes)++ =
                          (*lpPrimSpec->IisI) (BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex),
                                               lpPrimSpec);

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:     // Res = INT, Rht = INT
                    // Loop through the right arg/result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        *((LPAPLINT) lpMemRes)++ =
                          (*lpPrimSpec->IisI) (*((LPAPLINT) lpMemRht)++,
                                               lpPrimSpec);
                    break;

                case ARRAY_FLOAT:   // Res = INT, Rht = FLOAT
                    // Loop through the right arg/result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        *((LPAPLINT) lpMemRes)++ =
                          (*lpPrimSpec->IisF) (*((LPAPLFLOAT) lpMemRht)++,
                                               lpPrimSpec);
                    break;

                case ARRAY_APA:     // Res = INT, Rht = APA
                    // Loop through the right arg/result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        *((LPAPLINT) lpMemRes)++ =
                          (*lpPrimSpec->IisI) (apaOffRht + apaMulRht * uRes,
                                               lpPrimSpec);
                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        case ARRAY_FLOAT:           // Res = FLOAT
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:    // Res = FLOAT, Rht = BOOL
                    // Loop through the right arg/result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                    {
                        *((LPAPLFLOAT) lpMemRes)++ =
                          (*lpPrimSpec->FisI) (BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex),
                                               lpPrimSpec);
                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:     // Res = FLOAT, Rht = INT
                    // Loop through the right arg/result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        *((LPAPLFLOAT) lpMemRes)++ =
                          (*lpPrimSpec->FisI) (*((LPAPLINT) lpMemRht)++,
                                               lpPrimSpec);
                    break;

                case ARRAY_APA:     // Res = FLOAT, Rht = APA
                    // Loop through the right arg/result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        *((LPAPLFLOAT) lpMemRes)++ =
                          (*lpPrimSpec->FisI) (apaOffRht + apaMulRht * uRes,
                                               lpPrimSpec);
                    break;

                case ARRAY_FLOAT:   // Res = FLOAT, Rht = FLOAT
                    // Loop through the right arg/result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRht; uRes++)
                        *((LPAPLFLOAT) lpMemRes)++ =
                          (*lpPrimSpec->FisF) (*((LPAPLFLOAT) lpMemRht)++,
                                               lpPrimSpec);
                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        case ARRAY_NESTED:          // Res = NESTED
            // Loop through the right arg/result
            for (uRes = 0; bRet && uRes < (APLNELMSIGN) aplNELMRht; uRes++, ((APLNESTED *) lpMemRht)++)
            switch (GetPtrTypeInd (lpMemRht))
            {
                case PTRTYPE_STCONST:
                {
                    APLSTYPE   aplTypeRes2,
                               aplTypeRht2;
                    LPSYMENTRY lpSymSrc,
                               lpSymDst;

                    // Get the type of the SYMENTRY
                    aplTypeRht2 = TranslateImmTypeToArrayType ((ClrPtrTypeIndSym (lpMemRht))->stFlags.ImmType);

                    // Get the storage type of the result
                    aplTypeRes2 = (*lpPrimSpec->StorageTypeMon) (1,
                                                                &aplTypeRht2,
                                                                 lptkFunc);
                    // Check for DOMAIN ERROR
                    if (aplTypeRes2 EQ ARRAY_ERROR)
                    {
                        // Mark as a DOMAIN ERROR
                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        goto ERROR_EXIT;
                    } // End IF

                    // Copy the SYMENTRY as the same type as the result
                    lpSymSrc = ClrPtrTypeIndSym (lpMemRht);
                    lpSymDst = CopyImmSymEntry_EM (lpSymSrc,
                                                   TranslateArrayTypeToImmType (aplTypeRes2),
                                                   lptkFunc);
                    if (lpSymDst)
                    {
                        // Split cases based upon the result storage type
                        switch (aplTypeRes2)
                        {
                            case ARRAY_BOOL:            // Res = BOOL
                                // Split cases based upon the right arg's storage type
                                switch (lpSymSrc->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:  // Res = BOOL, Rht = BOOL
                                        lpSymDst->stData.stBoolean =
                                          (*lpPrimSpec->BisB) (lpSymSrc->stData.stBoolean,
                                                               lpPrimSpec);
                                        break;

                                    case IMMTYPE_INT:   // Res = BOOL, Rht = INT
                                        DbgBrk ();  // ***TESTME*** -- No such primitive

                                        lpSymDst->stData.stBoolean =
                                          (*lpPrimSpec->BisI) (lpSymSrc->stData.stInteger,
                                                               lpPrimSpec);
                                        break;

                                    case IMMTYPE_FLOAT: // Res = BOOL, Rht = FLOAT
                                        lpSymDst->stData.stBoolean =
                                          (*lpPrimSpec->BisF) (lpSymSrc->stData.stFloat,
                                                               lpPrimSpec);
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case ARRAY_INT:
                                // Split cases based upon the right arg's storage type
                                switch (lpSymSrc->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:  // Res = INT, Rht = BOOL
                                        DbgBrk ();  // ***TESTME*** -- No such primitive

                                        lpSymDst->stData.stInteger =
                                          (*lpPrimSpec->IisI) (lpSymSrc->stData.stBoolean,
                                                               lpPrimSpec);
                                        break;

                                    case IMMTYPE_INT:   // Res = INT, Rht = INT
                                        lpSymDst->stData.stInteger =
                                          (*lpPrimSpec->IisI) (lpSymSrc->stData.stInteger,
                                                               lpPrimSpec);
                                        break;

                                    case IMMTYPE_FLOAT: // Res = INT, Rht = FLOAT
                                        lpSymDst->stData.stInteger =
                                          (*lpPrimSpec->IisF) (lpSymSrc->stData.stFloat,
                                                               lpPrimSpec);
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            case ARRAY_FLOAT:
                                // Split cases based upon the right arg's storage type
                                switch (lpSymSrc->stFlags.ImmType)
                                {
                                    case IMMTYPE_BOOL:  // Res = FLOAT, Rht = BOOL
                                        lpSymDst->stData.stFloat =
                                          (*lpPrimSpec->FisI) (lpSymSrc->stData.stBoolean,
                                                               lpPrimSpec);
                                        break;

                                    case IMMTYPE_INT:   // Res = FLOAT, Rht = INT
                                        lpSymDst->stData.stFloat =
                                          (*lpPrimSpec->FisI) (lpSymSrc->stData.stInteger,
                                                               lpPrimSpec);
                                        break;

                                    case IMMTYPE_FLOAT: // Res = FLOAT, Rht = FLOAT
                                        lpSymDst->stData.stFloat =
                                          (*lpPrimSpec->FisF) (lpSymSrc->stData.stFloat,
                                                               lpPrimSpec);
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        // Save in the result
                        *((LPAPLNESTED) lpMemRes)++ = MakeSymTypeSym (lpSymDst);
                    } else
                        bRet = FALSE;
                    break;
                } // End PTRTYPE_STCONST

                case PTRTYPE_HGLOBAL:
                    // Handle via subroutine
                    hGlbSub = PrimFnMonGlb_EM (ClrPtrTypeIndGlb (lpMemRht),
                                               lptkFunc,
                                               lpPrimSpec);
                    if (hGlbSub)
                        *((LPAPLNESTED) lpMemRes)++ = MakeGlbTypeGlb (hGlbSub);
                    else
                        bRet = FALSE;
                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        defstop
            break;
    } // End SWITCH
    } __except (CheckException (GetExceptionInformation (), lpPrimSpec))
    {
#ifdef DEBUG
        dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
#endif
        // Split cases based upon the ExecCode
        switch (*lpPrimSpec->lpExecCode)
        {
            case EXEC_DOMAIN_ERROR:
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                goto ERROR_EXIT;

            case EXEC_RESULT_FLOAT:
                *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                if (IsSimpleNum (aplTypeRes)
                 && aplTypeRes NE ARRAY_FLOAT)
                {
                    aplTypeRes = ARRAY_FLOAT;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

                    // Lock the memory to get a ptr to it
                    lpMemRht = MyGlobalLock (hGlbRht);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

                    // We no longer need this storage
                    FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
#ifdef DEBUG
                    dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
#endif
                    goto RESTART_EXCEPTION;
                } // End IF

                // Fall through to never-never-land

            defstop
                break;
        } // End SWITCH
    } // End __try/__except

    if (bRet)
        goto NORMAL_EXIT;
ERROR_EXIT:
    bRet = FALSE;

    if (lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbRes)
    {
        // We no longer need this storage
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
    } // End IF
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

    DBGLEAVE;

    return hGlbRes;
} // End PrimFnMonGlb_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDyd_EM
//
//  Primitive scalar function for dyadic fns
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDyd_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDyd_EM
    (LPTOKEN    lptkLftArg,         // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis,           // Ptr to axis token (may be NULL)
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLRANK     aplRankLft,
                aplRankRht,
                aplRankRes;
    APLNELM     aplNELMAxis,
                aplNELMLft,
                aplNELMRht,
                aplNELMRes;
    HGLOBAL     hGlbAxis = NULL,
                hGlbRes = NULL,
                hGlbRht = NULL,
                hGlbLft = NULL;
    APLSTYPE    aplTypeLft,
                aplTypeRht,
                aplTypeRes;
    LPAPLUINT   lpMemAxisHead = NULL,
                lpMemAxisTail = NULL;
    LPVOID      lpMemLft = NULL,
                lpMemRht = NULL,
                lpMemRes = NULL;
    APLINT      aplInteger;
    BOOL        bRet = TRUE;
    LPPRIMFN_DYD_SNvSN PrimFn;
    LPYYSTYPE   lpYYRes = NULL;

    DBGENTER;

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
                           TRUE,            // TRUE iff want sorted axes
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

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                               &aplTypeLft,
                                                lptkFunc,
                                                aplNELMRht,
                                               &aplTypeRht);
    if (aplTypeRes EQ ARRAY_ERROR)
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    Assert (IsSimpleNum (aplTypeRes)
         || aplTypeRes EQ ARRAY_NESTED);

#ifdef PRIMPROTOFNSCALAR
    // Handle prototypes separately
    if (aplNELMLft EQ 0
     || aplNELMRht EQ 0)
    {
        lpYYRes = PrimProtoFnScalar_EM (lptkLftArg,
                                        lptkFunc,
                                        lptkRhtArg,
                                        lptkAxis);
        bRet = (lpYYRes NE NULL);

        goto NORMAL_EXIT;
    } // End IF
#endif

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Check for RANK and LENGTH ERRORs
    if (!CheckRankLengthError_EM (aplRankRes,
                                  aplRankLft,
                                  aplNELMLft,
                                  lpMemLft,
                                  aplRankRht,
                                  aplNELMRht,
                                  lpMemRht,
                                  aplNELMAxis,
                                  lpMemAxisTail,
                                  lptkFunc))
        goto ERROR_EXIT;

    // The NELM of the result is the larger of the two args
    //   unless one is empty
    if (aplNELMLft EQ 0 || aplNELMRht EQ 0)
        aplNELMRes = 0;
    else
        aplNELMRes = max (aplNELMLft, aplNELMRht);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Handle APA result separately
    if (aplTypeRes EQ ARRAY_APA)
    {
        if (aplTypeLft EQ ARRAY_APA)
            FirstValue (lptkRhtArg,     // Ptr to right arg token
                       &aplInteger,     // Ptr to integer result
                        NULL,           // Ptr to float ...
                        NULL,           // Ptr to WCHAR ...
                        NULL,           // Ptr to longest ...
                        NULL,           // Ptr to lpSym/Glb ...
                        NULL,           // Ptr to ...immediate type ...
                        NULL);          // Ptr to array type ...
        else
            FirstValue (lptkLftArg,     // Ptr to right arg token
                       &aplInteger,     // Ptr to integer result
                        NULL,           // Ptr to float ...
                        NULL,           // Ptr to WCHAR ...
                        NULL,           // Ptr to longest ...
                        NULL,           // Ptr to lpSym/Glb ...
                        NULL,           // Ptr to ...immediate type ...
                        NULL);          // Ptr to array type ...
        if (!(*lpPrimSpec->ApaResultDyd_EM) (lpYYRes,
                                             lptkFunc,
                                             hGlbLft,
                                             hGlbRht,
                                            &hGlbRes,
                                            &lpMemRes,
                                             aplRankLft,
                                             aplRankRht,
                                             aplNELMLft,
                                             aplNELMRht,
                                             aplInteger,
                                             lpPrimSpec))
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } // End IF

    // Allocate space for result
    if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                    &hGlbRes,
                                     lpMemLft,
                                     lpMemRht,
                                    &lpMemRes,
                                     aplRankLft,
                                     aplRankRht,
                                    &aplRankRes,
                                     aplTypeRes,
                                     aplNELMLft,
                                     aplNELMRht,
                                     aplNELMRes))
        goto ERROR_EXIT;

    // Four cases:
    //   Result     Left       Right
    //   ----------------------------
    //   Simple     Simple     Simple
    //   Nested     Simple     Nested
    //   Nested     Nested     Simple
    //   Nested     Nested     Nested

    // Split cases based upon the combined left vs. right types
    if (aplTypeLft EQ ARRAY_NESTED
     && aplTypeRht EQ ARRAY_NESTED)
        // Left arg is NESTED, right arg is NESTED
        PrimFn = PrimFnDydNestNest_EM;
    else
    if (aplTypeLft NE ARRAY_NESTED
     && aplTypeRht EQ ARRAY_NESTED)
        // Left arg is SIMPLE, right arg is NESTED
        PrimFn = PrimFnDydSimpNest_EM;
    else
    if (aplTypeLft EQ ARRAY_NESTED
     && aplTypeRht NE ARRAY_NESTED)
        // Left arg is NESTED, right arg is SIMPLE
        PrimFn = PrimFnDydNestSimp_EM;
    else
    if (aplTypeLft NE ARRAY_NESTED
     && aplTypeRht NE ARRAY_NESTED)
        // Left arg is SIMPLE, right arg is SIMPLE
        PrimFn = PrimFnDydSimpSimp_EM;
    else
    {
        DbgStop ();     // We should never get here

        return NULL;
    } // Endf IF/ELSE/...

    // Call the appropriate function
    if (!(*PrimFn) (lpYYRes,
                    lptkLftArg,
                    lptkFunc,
                    lptkRhtArg,
                    hGlbLft,
                    hGlbRht,
                   &hGlbRes,
                    lpMemLft,           // Points to sign.ature
                    lpMemRht,           // ...
                    lpMemRes,           // ...
                    lpMemAxisHead,
                    lpMemAxisTail,
                    aplRankLft,
                    aplRankRht,
                    aplRankRes,
                    aplTypeLft,
                    aplTypeRht,
                    aplTypeRes,
                    aplNELMLft,
                    aplNELMRht,
                    aplNELMRes,
                    aplNELMAxis,
                    lpPrimSpec))
        goto ERROR_EXIT;
    else
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

    if (hGlbAxis && lpMemAxisHead)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbAxis); lpMemAxisHead = lpMemAxisTail = NULL;
    } // End IF

    // If we failed and there's a result, free it
    if (!bRet && lpYYRes)
    {
        YYFree (lpYYRes); lpYYRes = NULL;
    } // End IF/ELSE

    DBGLEAVE;

    return lpYYRes;
} // End PrimFnDyd_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydSimpNest_EM
//
//  Dyadic primitive scalar function, left simple, right nested
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSimpNest_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydSimpNest_EM
    (LPYYSTYPE     lpYYRes,         // Ptr to result token

     LPTOKEN       lptkLftArg,      // Ptr to left arg token
     LPTOKEN       lptkFunc,        // ...    function ...
     LPTOKEN       lptkRhtArg,      // ...    right arg ...

     HGLOBAL       hGlbLft,         // Left arg handle
     HGLOBAL       hGlbRht,         // Right ...
     HGLOBAL      *lphGlbRes,       // Ptr to result handle

     LPVOID        lpMemLft,        // Points to sign.ature
     LPVOID        lpMemRht,        // ...
     LPVOID        lpMemRes,        // ...

     LPAPLUINT     lpMemAxisHead,   //
     LPAPLUINT     lpMemAxisTail,   //
                                    //
     APLRANK       aplRankLft,      // Left arg rank
     APLRANK       aplRankRht,      // Right ...
     APLRANK       aplRankRes,      // Result ...
                                    //
     APLSTYPE      aplTypeLft,      // Left arg type
     APLSTYPE      aplTypeRht,      // Right ...
     APLSTYPE      aplTypeRes,      // Result ...

     APLNELM       aplNELMLft,      // Left arg NELM
     APLNELM       aplNELMRht,      // Right ...
     APLNELM       aplNELMRes,      // Result ...
     APLNELM       aplNELMAxis,     // Axis ...
     LPPRIMSPEC    lpPrimSpec)      // Ptr to local PRIMSPEC

{
    BOOL       bRet = TRUE;
    APLINT     uRes;
    APLINT     aplIntegerLft,
               apaOffLft,
               apaMulLft,
               iDim;
    APLUINT    ByteAlloc;
    APLFLOAT   aplFloatLft;
    HGLOBAL    hGlbWVec = NULL,
               hGlbOdo = NULL,
               hGlbSub;
    LPAPLUINT  lpMemWVec = NULL,
               lpMemDimLft,
               lpMemDimRht,
               lpMemDimRes,
               lpMemOdo = NULL;
    APLCHAR    aplCharLft;

    DBGENTER;

    // If the left arg is immediate, get the one and only value
    if (!lpMemLft)
        FirstValue (lptkLftArg,         // Ptr to right arg token
                   &aplIntegerLft,      // Ptr to integer result
                   &aplFloatLft,        // Ptr to float ...
                   &aplCharLft,         // Ptr to WCHAR ...
                    NULL,               // Ptr to longest ...
                    NULL,               // Ptr to lpSym/Glb ...
                    NULL,               // Ptr to ...immediate type ...
                    NULL);              // Ptr to array type ...
    else
    {
        // Skip over the header to the dimensions
        lpMemDimLft = VarArrayBaseToDim (lpMemLft);

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
    } // End IF/ELSE

    // If the left arg is APA, get its parameters
    if (aplTypeLft EQ ARRAY_APA)
    {
#define lpAPA       ((LPAPLAPA) lpMemLft)
        apaOffLft = lpAPA->Off;
        apaMulLft = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);
    lpMemDimRes = VarArrayBaseToDim (lpMemRes);

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    // Ensure the elements of the result are set to PTR_REUSED
    //   in case we fail along the way
    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
        ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;

#ifndef PRIMPROTOFNSCALAR
    // Handle prototypes separately
    if (aplNELMRes EQ 0)
    {
        HGLOBAL hGlbProto;

        // Make the prototype
        hGlbProto = MakePrototype_EM (hGlbRht,          // Proto arg handle
                                      lptkFunc,         // Ptr to function token
                                      MP_NUMCONV);      // Convert to numerics
        if (hGlbProto)
            // Save the handle
            *((LPAPLNESTED) lpMemRes) = MakeGlbTypeGlb (hGlbProto);
        else
            goto ERROR_EXIT;
    } else
#endif
    {
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
                                           lptkFunc);
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
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Lock the global memory to get a ptr to it
            lpMemOdo = MyGlobalLock (hGlbOdo);
        } // End IF

        // Loop through the result
        for (uRes = 0; bRet && uRes < (APLNELMSIGN) aplNELMRes; uRes++)
        {
            APLINT   uLft, uRht, uArg;
            APLSTYPE aplTypeHet;

            // Copy in case we are hetrogeneous
            aplTypeHet = aplTypeLft;

            // If the left arg is not immediate, get the next value
            if (lpMemLft)
            {
                if (aplNELMAxis NE aplRankRes)
                {
                    // Loop through the odometer values accumulating
                    //   the weighted sum
                    for (uArg = 0, uRht = aplRankRes - aplNELMAxis; uRht < (APLRANKSIGN) aplRankRes; uRht++)
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

                // Split cases based upon the left arg's storage type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                        aplIntegerLft = GetNextInteger (lpMemLft, aplTypeLft, uLft, apaOffLft, apaMulLft);
                        aplFloatLft   = (APLFLOAT) aplIntegerLft;   // In case of type promotion

                        break;

                    case ARRAY_FLOAT:
                        aplFloatLft   = GetNextFloat   (lpMemLft, aplTypeLft, uLft, apaOffLft, apaMulLft);

                        break;

                    case ARRAY_CHAR:
                        aplCharLft    = ((LPAPLCHAR) lpMemLft)[uRes];

                        break;

                    case ARRAY_HETERO:
                        aplTypeHet = GetNextHetero (lpMemLft, uLft, &aplIntegerLft, &aplFloatLft, &aplCharLft);

                        break;

                    defstop
                        break;
                } // End SWITCH
            } else
                uRht = uRes;

            hGlbSub = PrimFnDydSimpNestSub_EM (lptkFunc,
                                               aplTypeHet,
                                               aplIntegerLft,
                                               aplFloatLft,
                                               aplCharLft,
                                               ((LPAPLNESTED) lpMemRht)[uRht],
                                               lpPrimSpec);
            if (!hGlbSub)
                goto ERROR_EXIT;
            else
                *((LPAPLNESTED) lpMemRes)++ = MakeGlbTypeGlb (hGlbSub);
        } // End FOR
    } // End IF/ELSE/...

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (*lphGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

ERROR_EXIT:
    bRet = FALSE;
NORMAL_EXIT:
    if (lpMemWVec)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbWVec); lpMemWVec = NULL;
    } // End IF

    if (lpMemOdo)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbOdo); lpMemOdo = NULL;
    } // End IF

    if (hGlbWVec)
    {
        // We no longer need this storage
        DbgGlobalFree (hGlbWVec); hGlbWVec = NULL;
    } // End IF

    if (hGlbOdo)
    {
        // We no longer need this storage
        DbgGlobalFree (hGlbOdo); hGlbOdo = NULL;
    } // End IF

    DBGLEAVE;

    return bRet;
} // End PrimFnDydSimpNest_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydNestSimp_EM
//
//  Dyadic primitive scalar function, left nested, right simple
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydNestSimp_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydNestSimp_EM
    (LPYYSTYPE     lpYYRes,         // Ptr to result token

     LPTOKEN       lptkLftArg,      // Ptr to left arg token
     LPTOKEN       lptkFunc,        // ...    function ...
     LPTOKEN       lptkRhtArg,      // ...    right arg ...

     HGLOBAL       hGlbLft,         // Left arg handle
     HGLOBAL       hGlbRht,         // Right ...
     HGLOBAL      *lphGlbRes,       // Ptr to result handle

     LPVOID        lpMemLft,        // Points to sign.ature
     LPVOID        lpMemRht,        // ...
     LPVOID        lpMemRes,        // ...

     LPAPLUINT     lpMemAxisHead,   //
     LPAPLUINT     lpMemAxisTail,   //
                                    //
     APLRANK       aplRankLft,      // Left arg rank
     APLRANK       aplRankRht,      // Right ...
     APLRANK       aplRankRes,      // Result ...
                                    //
     APLSTYPE      aplTypeLft,      // Left arg type
     APLSTYPE      aplTypeRht,      // Right ...
     APLSTYPE      aplTypeRes,      // Result ...

     APLNELM       aplNELMLft,      // Left arg NELM
     APLNELM       aplNELMRht,      // Right ...
     APLNELM       aplNELMRes,      // Result ...
     APLNELM       aplNELMAxis,     // Axis ...
     LPPRIMSPEC    lpPrimSpec)      // Ptr to local PRIMSPEC

{
    BOOL       bRet = TRUE;
    APLINT     uRes;
    APLINT     aplIntegerRht,
               apaOffRht,
               apaMulRht,
               ByteAlloc,
               iDim;
    APLFLOAT   aplFloatRht;
    HGLOBAL    hGlbWVec = NULL,
               hGlbOdo = NULL,
               hGlbSub;
    LPAPLUINT  lpMemWVec = NULL,
               lpMemDimLft,
               lpMemDimRht,
               lpMemDimRes,
               lpMemOdo = NULL;
    APLCHAR    aplCharRht;

    DBGENTER;

    // If the right arg is immediate, get the one and only value
    if (!lpMemRht)
        FirstValue (lptkRhtArg,         // Ptr to right arg token
                   &aplIntegerRht,      // Ptr to integer result
                   &aplFloatRht,        // Ptr to float ...
                   &aplCharRht,         // Ptr to WCHAR ...
                    NULL,               // Ptr to longest ...
                    NULL,               // Ptr to lpSym/Glb ...
                    NULL,               // Ptr to ...immediate type ...
                    NULL);              // Ptr to array type ...
    else
    {
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
    } // End IF/ELSE

    // If the right arg is APA, get its parameters
    if (aplTypeRht EQ ARRAY_APA)
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Skip over the header to the dimensions
    lpMemDimLft = VarArrayBaseToDim (lpMemLft);
    lpMemDimRes = VarArrayBaseToDim (lpMemRes);

    // Skip over the header and dimensions to the data
    lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    // Ensure the elements of the result are set to PTR_REUSED
    //   in case we fail along the way
    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
        ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;

#ifndef PRIMPROTOFNSCALAR
    // Handle prototypes separately
    if (aplNELMRes EQ 0)
    {
        // Make the prototype
        hGlbSub = MakePrototype_EM (hGlbLft,        // Proto arg handle
                                    lptkFunc,       // Ptr to function token
                                    MP_NUMCONV);    // Convert to numerics
        if (hGlbSub)
            // Save the handle
            *((LPAPLNESTED) lpMemRes) = MakeGlbTypeGlb (hGlbSub);
        else
            goto ERROR_EXIT;
    } else
#endif
    {
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
                                           lptkFunc);
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
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Lock the global memory to get a ptr to it
            lpMemOdo = MyGlobalLock (hGlbOdo);
        } // End IF

        // Loop through the result
        for (uRes = 0; bRet && uRes < (APLNELMSIGN) aplNELMRes; uRes++)
        {
            APLINT   uLft, uRht, uArg;
            APLSTYPE aplTypeHet;

            // Copy in case we are hetrogeneous
            aplTypeHet = aplTypeRht;

            // If the right arg is not immediate, get the next value
            if (lpMemRht)
            {
                if (aplNELMAxis NE aplRankRes)
                {
                    // Loop through the odometer values accumulating
                    //   the weighted sum
                    for (uArg = 0, uLft = aplRankRes - aplNELMAxis; uLft < (APLRANKSIGN) aplRankRes; uLft++)
                        uArg += lpMemOdo[lpMemAxisHead[uLft]] * lpMemWVec[uLft];

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

                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                        aplIntegerRht = GetNextInteger (lpMemRht, aplTypeRht, uRht, apaOffRht, apaMulRht);
                        aplFloatRht   = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                        break;

                    case ARRAY_FLOAT:
                        aplFloatRht   = GetNextFloat   (lpMemRht, aplTypeRht, uRht, apaOffRht, apaMulRht);

                        break;

                    case ARRAY_CHAR:
                        aplCharRht    = ((LPAPLCHAR) lpMemRht)[uRes];

                        break;

                    case ARRAY_HETERO:
                        aplTypeHet = GetNextHetero (lpMemRht, uRht, &aplIntegerRht, &aplFloatRht, &aplCharRht);

                        break;

                    defstop
                        break;
                } // End SWITCH
            } else
                uLft = uRes;

            hGlbSub = PrimFnDydNestSimpSub_EM (lptkFunc,
                                               aplTypeHet,
                                               aplIntegerRht,
                                               aplFloatRht,
                                               aplCharRht,
                                               ((LPAPLNESTED) lpMemLft)[uLft],
                                               lpPrimSpec);
            if (!hGlbSub)
                goto ERROR_EXIT;
            else
                *((LPAPLNESTED) lpMemRes)++ = MakeGlbTypeGlb (hGlbSub);
        } // End FOR
    } // End IF/ELSE/...

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (*lphGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

ERROR_EXIT:
    bRet = FALSE;
NORMAL_EXIT:
    if (lpMemWVec)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbWVec); lpMemWVec = NULL;
    } // End IF

    if (lpMemOdo)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbOdo); lpMemOdo = NULL;
    } // End IF

    if (hGlbWVec)
    {
        // We no longer need this storage
        DbgGlobalFree (hGlbWVec); hGlbWVec = NULL;
    } // End IF

    if (hGlbOdo)
    {
        // We no longer need this storage
        DbgGlobalFree (hGlbOdo); hGlbOdo = NULL;
    } // End IF

    DBGLEAVE;

    return bRet;
} // End PrimFnDydNestSimp_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydNestSimpSub_EM
//
//  Subroutine to PrimFnDydNestSimp_EM
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydNestSimpSub_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL PrimFnDydNestSimpSub_EM
    (LPTOKEN    lptkFunc,           // Ptr to function token
     APLSTYPE   aplTypeRht,         // Right arg type
     APLINT     aplIntegerRht,      // ...       integer value
     APLFLOAT   aplFloatRht,        // ...       float   ...
     APLCHAR    aplCharRht,         // ...       char    ...
     APLNESTED  aplNestedLft,       // Left arg nested value
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    BOOL     bRet = TRUE;
    HGLOBAL  hGlbLft = NULL,
             hGlbRes = NULL,
             hGlbSub;
    LPVOID   lpMemLft,
             lpMemRes = NULL;
    APLSTYPE aplTypeLft,
             aplTypeRes;
    APLNELM  aplNELMLft,
             aplNELMRht = 1,
             aplNELMRes;
    APLRANK  aplRankLft,
             aplRankRht = 0,
             aplRankRes;
    APLINT   uLft,
             apaOffLft,
             apaMulLft;
    UINT     uBitIndex = 0;

    DBGENTER;

    // The left arg data is a valid HGLOBAL array
    Assert (IsGlbTypeVarDir (aplNestedLft));

    // Clear the identifying bits
    hGlbLft = ClrPtrTypeDirGlb (aplNestedLft);

    // Lock the memory to get a ptr to it
    lpMemLft = MyGlobalLock (hGlbLft);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemLft)

    aplTypeLft = lpHeader->ArrType;
    aplNELMLft = lpHeader->NELM;
    aplRankLft = lpHeader->Rank;

#undef  lpHeader

    // The NELM of the result is the larger of the two arg's NELMs
    aplNELMRes = max (aplNELMLft, aplNELMRht);

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                               &aplTypeLft,
                                                lptkFunc,
                                                aplNELMRht,
                                               &aplTypeRht);
    if (aplTypeRes EQ ARRAY_ERROR)
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Special case APA result
    if (aplTypeRes EQ ARRAY_APA)
    {
        if (!(*lpPrimSpec->ApaResultDyd_EM) (NULL,
                                             lptkFunc,
                                             hGlbLft,
                                             NULL,
                                            &hGlbRes,
                                            &lpMemRes,
                                             aplRankLft,
                                             aplRankRht,
                                             aplNELMLft,
                                             aplNELMRht,
                                             aplIntegerRht,
                                             lpPrimSpec))
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } // End IF

    Assert (IsSimpleNum (aplTypeRes)
         || aplTypeRes EQ ARRAY_NESTED);

    // Allocate space for result
    if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                    &hGlbRes,
                                     NULL,
                                     lpMemLft,
                                    &lpMemRes,
                                     aplRankRht,
                                     aplRankLft,
                                    &aplRankRes,
                                     aplTypeRes,
                                     aplNELMRht,
                                     aplNELMLft,
                                     aplNELMRes))
        goto ERROR_EXIT;

    // Skip over the header and dimensions to the data
    lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    // In case the left arg is APA
    if (aplTypeLft EQ ARRAY_APA)
    {
#define lpAPA       ((LPAPLAPA) lpMemLft)
        apaOffLft = lpAPA->Off;
        apaMulLft = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // If simple result, ...
    if (IsSimpleNum (aplTypeRes))
    {
        bRet = PrimFnDydMultSing_EM (aplTypeRes,
                                     lpMemRes,
                                     aplNELMRes,
                                     aplTypeLft,
                                     apaOffLft,
                                     apaMulLft,
                                     lpMemLft,
                                     aplTypeRht,
                                     aplIntegerRht,
                                     aplFloatRht,
                                     aplCharRht,
                                     lptkFunc,
                                     lpPrimSpec);

    } else
    // If nested result, ...
    if (aplTypeRes EQ ARRAY_NESTED)
    {
////case ARRAY_NESTED:              // Res = NESTED, Lft = NESTED, Rht = BOOL/INT/FLOAT(S)
#ifndef PRIMPROTOFNSCALAR
        // Handle the prototype case
        aplNELMLft = max (aplNELMLft, 1);
#endif
        // Loop through the left arg/result
        for (uLft = 0; bRet && uLft < (APLNELMSIGN) aplNELMLft; uLft++)
        {
            hGlbSub = PrimFnDydNestSimpSub_EM (lptkFunc,
                                               aplTypeRht,
                                               aplIntegerRht,
                                               aplFloatRht,
                                               aplCharRht,
                                               *((LPAPLNESTED) lpMemLft)++,
                                               lpPrimSpec);
            if (!hGlbSub)
                bRet = FALSE;
            else
                *((LPAPLNESTED) lpMemRes)++ = MakeGlbTypeGlb (hGlbSub);
        } // End FOR
    } else
        DbgStop ();         // We should never get here


    if (bRet)
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
    if (lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLft); lpMemLft = NULL;

    DBGLEAVE;

    if (bRet)
        return hGlbRes;
    else
        return NULL;
} // End PrimFnDydNestSimpSub_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydNestNest_EM
//
//  Dyadic primitive scalar function, left nested, right nested
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydNestNest_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydNestNest_EM
    (LPYYSTYPE  lpYYRes,            // Ptr to result token

     LPTOKEN    lptkLftArg,         // Ptr to left arg token
     LPTOKEN    lptkFunc,           // ...    function ...
     LPTOKEN    lptkRhtArg,         // ...    right arg ...

     HGLOBAL    hGlbLft,            // Left arg handle
     HGLOBAL    hGlbRht,            // Right ...
     HGLOBAL   *lphGlbRes,          // Ptr to result handle

     LPVOID     lpMemLft,           // Points to sign.ature
     LPVOID     lpMemRht,           // ...
     LPVOID     lpMemRes,           // ...

     LPAPLUINT  lpMemAxisHead,      // Ptr to axis values, fleshed out
     LPAPLUINT  lpMemAxisTail,      // Ptr to grade up of AxisHead

     APLRANK    aplRankLft,         // Left arg rank
     APLRANK    aplRankRht,         // Right ...
     APLRANK    aplRankRes,         // Result ...

     APLSTYPE   aplTypeLft,         // Left arg type
     APLSTYPE   aplTypeRht,         // Right ...
     APLSTYPE   aplTypeRes,         // Result ...

     APLNELM    aplNELMLft,         // Left arg NELM
     APLNELM    aplNELMRht,         // Right ...
     APLNELM    aplNELMRes,         // Result ...
     APLNELM    aplNELMAxis,        // Axis ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    BOOL   bRet = TRUE;
    APLINT uRes;

    DBGENTER;

    DbgBrk ();

    // Check for INNER RANK and LENGTH ERRORs
    if (!CheckRankLengthError_EM (aplRankRes,
                                  aplRankLft,
                                  aplNELMLft,
                                  lpMemLft,
                                  aplRankRht,
                                  aplNELMRht,
                                  lpMemRht,
                                  aplNELMAxis,
                                  lpMemAxisTail,
                                  lptkFunc))
        goto ERROR_EXIT;

    // Skip over the headers and dimensions to the data
    lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

#ifndef PRIMPROTOFNSCALAR
    // Handle prototypes separately
    if (aplNELMRes EQ 0)
    {
        DbgBrk ();      // ***FINISHME***




    } else
#endif
    // Loop through the left and right args
    for (uRes = 0; bRet && uRes < (APLNELMSIGN) aplNELMRes; uRes++)
    {
        TOKEN     tkLft = {0},
                  tkRht = {0};
        LPYYSTYPE lpYYRes;

        tkLft.tkFlags.TknType   = TKT_VARARRAY;
////////tkLft.TknType.ImmType   = 0;    // Already zero from YYAlloc
////////tkLft.TknType.NoDisplay = 0;    // Already zero from YYAlloc
        tkLft.tkData.tkGlbData  = ((LPAPLNESTED) lpMemLft)[uRes % aplNELMLft];
        tkLft.tkCharIndex       = lptkLftArg->tkCharIndex;

        tkRht.tkFlags.TknType   = TKT_VARARRAY;
////////tkRht.TknType.ImmType   = 0;    // Already zero from YYAlloc
////////tkRht.TknType.NoDisplay = 0;    // Already zero from YYAlloc
        tkRht.tkData.tkGlbData  = ((LPAPLNESTED) lpMemRht)[uRes % aplNELMRht];
        tkRht.tkCharIndex       = lptkRhtArg->tkCharIndex;

        lpYYRes = (*lpPrimSpec->PrimFnDyd_EM) (&tkLft,
                                                lptkFunc,
                                               &tkRht,
                                                NULL,
                                                lpPrimSpec);
        if (lpYYRes)
        {
            ((LPAPLNESTED) lpMemRes)[uRes] = lpYYRes->tkToken.tkData.tkGlbData;
            YYFree (lpYYRes); lpYYRes = NULL;
        } else
            bRet = FALSE;
    } // End FOR

    if (bRet)
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (*lphGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF
ERROR_EXIT:
    DBGLEAVE;

    return bRet;
} // End PrimFnDydNestNest_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydSingMult_EM
//
//  Primitive scalar dyadic function,
//    left simple singleton, right simple multipleton
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSingMult_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydSingMult_EM
    (APLSTYPE   aplTypeRes,         // Result type
     LPVOID     lpMemRes,           // Ptr to result memory (Points to the data)
     APLNELM    aplNELMRes,         // Result NELM
     APLSTYPE   aplTypeLft,         // Left arg type
     APLINT     aplIntegerLft,      // ...      integer value
     APLFLOAT   aplFloatLft,        // ...      float   ...
     APLCHAR    aplCharLft,         // ...      char    ...
     APLSTYPE   aplTypeRht,         // Right arg type
     APLINT     apaOffRht,          // ...       APA offset
     APLINT     apaMulRht,          // ...       ... multiplier
     LPVOID     lpMemRht,           // Points to the data
     LPTOKEN    lptkFunc,           // Ptr to function token
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLINT   uRes;
    UINT     uBitIndex = 0;
    LPVOID   lpMemResStart,
             lpMemRhtStart;
    APLSTYPE aplTypeHet;
    APLINT   aplIntegerRht;
    APLFLOAT aplFloatRht;
    APLCHAR  aplCharRht;

    // If the singleton is integer,
    //   attempt to demote it to Boolean
    if (aplTypeLft EQ ARRAY_INT
     && IsBooleanValue (aplIntegerLft))
        aplTypeLft = ARRAY_BOOL;

    // Save the starting values in case we need
    //   to restart from an exception
    lpMemResStart = lpMemRes;
    lpMemRhtStart = lpMemRht;
RESTART_EXCEPTION:
    __try
    {
    // Split cases based upon the storage type of the result
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:                    // Res = BOOL
            // Split off character or heterogeneous (CH) arguments
            if (IsSimpleCH (aplTypeLft)
             || IsSimpleCH (aplTypeRht))
            {
                APLBOOL bCvN;

                Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                     || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);

                // Calculate the result of CvN
                bCvN = (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);

                // If both arguments are CHAR,
                //   use BisCvC
                if (aplTypeLft EQ ARRAY_CHAR
                 && aplTypeRht EQ ARRAY_CHAR)
                {
                    // Loop through the result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    {
                        *((LPAPLBOOL)  lpMemRes) |=
                          (*lpPrimSpec->BisCvC) (aplCharLft,
                                                 *((LPAPLCHAR) lpMemRht)++,
                                                 lpPrimSpec) << uBitIndex;
                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;                  // Start over
                            ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                        } // End IF
                    } // End FOR
                } else
                // If the other argument is simple numeric,
                //   use bCvN
                if ((IsSimpleNum (aplTypeLft) && aplTypeRht EQ ARRAY_CHAR)
                 || (IsSimpleNum (aplTypeRht) && aplTypeLft EQ ARRAY_CHAR))
                {
                    // Loop through the result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    {
                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;                  // Start over
                            ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                        } // End IF
                    } // End FOR
                } else
                // If the multipleton argument is HETERO,
                //   look at each LPSYMENTRY
                if (aplTypeRht EQ ARRAY_HETERO)
                {
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = BOOL, Lft = BOOL (S)
                        case ARRAY_INT:     // Res = BOOL, Lft = INT  (S)
                            // Loop through the right arg
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                aplTypeHet = GetNextHetero (lpMemRht, uRes, &aplIntegerRht, &aplFloatRht, &aplCharRht);

                                // Split cases based upon the right arg's item's storage type
                                switch (aplTypeHet)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Lft = BOOL/INT (S), Rht = BOOL (M)
                                    case ARRAY_INT:     // Res = BOOL, Lft = BOOL/INT (S), Rht = INT  (M)
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                                 aplIntegerRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_FLOAT:   // Res = BOOL, Lft = BOOL/INT (S), Rht = FLOAT(M)
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                                 aplFloatRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_CHAR:    // Res = BOOL, Lft = BOOL/INT (S), Rht = CHAR (M)
                                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = BOOL, Lft = FLOAT(S)
                            // Loop through the right arg
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                aplTypeHet = GetNextHetero (lpMemRht, uRes, &aplIntegerRht, &aplFloatRht, &aplCharRht);

                                // Split cases based upon the right arg's item's storage type
                                switch (aplTypeHet)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Lft = FLOAT (S), Rht = BOOL (M)
                                    case ARRAY_INT:     // Res = BOOL, Lft = FLOAT (S), Rht = INT  (M)
                                    case ARRAY_FLOAT:   // Res = BOOL, Lft = FLOAT (S), Rht = FLOAT(M)
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                                 aplFloatRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_CHAR:    // Res = BOOL, Lft = FLOAT (S), Rht = CHAR (M)
                                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_CHAR:    // Res = BOOL, Lft = CHAR (S)
                            // Loop through the right arg
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                aplTypeHet = GetNextHetero (lpMemRht, uRes, &aplIntegerRht, &aplFloatRht, &aplCharRht);

                                // Split cases based upon the right arg's item's storage type
                                switch (aplTypeHet)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Lft = CHAR  (S), Rht = BOOL (M)
                                    case ARRAY_INT:     // Res = BOOL, Lft = CHAR  (S), Rht = INT  (M)
                                    case ARRAY_FLOAT:   // Res = BOOL, Lft = CHAR  (S), Rht = FLOAT(M)
                                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                                        break;

                                    case ARRAY_CHAR:    // Res = BOOL, Lft = CHAR  (S), Rht = CHAR (M)
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisCvC) (aplCharLft,
                                                                 aplCharRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } else
                    DbgStop ();         // We should never get here
            } else
            // Split cases based upon the storage type of the left arg
            switch (aplTypeLft)
            {
                case ARRAY_BOOL:            // Res = BOOL, Lft = BOOL(S)
                    if (aplTypeRht EQ ARRAY_BOOL)
                    {
                        // ***FIXME*** -- Optimize by chunking
                        // Loop through the right arg/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisBvB) ((APLBOOL) aplIntegerLft,
                                                     (APLBOOL) (BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex)),
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                                ((LPAPLBOOL) lpMemRes)++;       // ...
                            } // End IF
                        } // End FOR

                        break;
                    } // End IF

                    // Fall through to common code

                case ARRAY_INT:             // Res = BOOL, Lft = INT(S)
                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = BOOL, Lft = INT(S), Rht = BOOL(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                         BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex),
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                                    ((LPAPLBOOL) lpMemRes)++;       // ...
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = BOOL, Lft = INT(S), Rht = INT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                         *((LPAPLINT) lpMemRht)++,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = BOOL, Lft = INT(S), Rht = APA(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                         apaOffRht + apaMulRht * uRes,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = BOOL, Lft = INT(S), Rht = FLOAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                         *((LPAPLFLOAT) lpMemRht)++,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_FLOAT:           // Res = BOOL, Lft = FLOAT(S)
                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = BOOL, Lft = FLOAT(S), Rht = BOOL(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                         BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex),
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                                    ((LPAPLBOOL) lpMemRes)++;       // ...
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = BOOL, Lft = FLOAT(S), Rht = INT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                         (APLFLOAT) *((LPAPLINT) lpMemRht)++,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = BOOL, Lft = FLOAT(S), Rht = APA(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                         (APLFLOAT) (apaOffRht + apaMulRht * uRes),
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = BOOL, Lft = FLOAT(S), Rht = FLOAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                         *((LPAPLFLOAT) lpMemRht)++,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_INT:                     // Res = INT
            // Split cases based upon the storage type of the left arg
            switch (aplTypeLft)
            {
                case ARRAY_BOOL:            // Res = INT, Lft = BOOL(S)
                case ARRAY_INT:             // Res = INT, Lft = INT(S)
                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = INT, Lft = BOOL/INT(S), Rht = BOOL(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisIvI) (aplIntegerLft,
                                                         BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex),
                                                         lpPrimSpec);
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = INT, Lft = BOOL/INT(S), Rht = INT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisIvI) (aplIntegerLft,
                                                         *((LPAPLINT) lpMemRht)++,
                                                         lpPrimSpec);
                            break;

                        case ARRAY_APA:     // Res = INT, Lft = BOOL/INT(S), Rht = APA(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisIvI) (aplIntegerLft,
                                                         apaOffRht + apaMulRht * uRes,
                                                         lpPrimSpec);
                            break;

                        case ARRAY_FLOAT:   // Res = INT, Lft = BOOL/INT(S), Rht = FLOAT(M)
                            DbgBrk ();      // ***TESTME*** -- No such primitive

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLINT) lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) (aplFloatLft,
                                                         *((LPAPLFLOAT) lpMemRht)++,
                                                         lpPrimSpec);
                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_FLOAT:           // Res = INT, Lft = FLOAT(S)
                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = INT, Lft = FLOAT(S), Rht = BOOL(M)
                            DbgBrk ();      // ***TESTME*** -- No such primitive

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLINT) lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) (aplFloatLft,
                                                         BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex),
                                                         lpPrimSpec);
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = INT, Lft = FLOAT(S), Rht = INT(M)
                            DbgBrk ();      // ***TESTME*** -- No such primitive

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) (aplFloatLft,
                                                         (APLFLOAT) *((LPAPLINT) lpMemRht)++,
                                                         lpPrimSpec);
                            break;

                        case ARRAY_APA:     // Res = INT, Lft = FLOAT(S), Rht = APA(M)
                            DbgBrk ();      // ***TESTME*** -- No such primitive

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) (aplFloatLft,
                                                         (APLFLOAT) (apaOffRht + apaMulRht * uRes),
                                                         lpPrimSpec);
                            break;

                        case ARRAY_FLOAT:   // Res = INT, Lft = FLOAT(S), Rht = FLOAT(M)
                            DbgBrk ();      // ***TESTME*** -- No such primitive

                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) (aplFloatLft,
                                                         *((LPAPLFLOAT) lpMemRht)++,
                                                         lpPrimSpec);
                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_FLOAT:                   // Res = FLOAT
            // Split cases based upon the storage type of the left arg
            switch (aplTypeLft)
            {
                case ARRAY_BOOL:            // Res = FLOAT, Lft = BOOL(S)
                case ARRAY_INT:             // Res = FLOAT, Lft = INT(S)
                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = FLOAT, Lft = BOOL/INT(S), Rht = BOOL(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisIvI) (aplIntegerLft,
                                                         BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex),
                                                         lpPrimSpec);
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = FLOAT, Lft = BOOL/INT(S), Rht = INT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisIvI) (aplIntegerLft,
                                                         *((LPAPLINT) lpMemRht)++,
                                                         lpPrimSpec);
                            break;

                        case ARRAY_APA:     // Res = FLOAT, Lft = BOOL/INT(S), Rht = APA(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisIvI) (aplIntegerLft,
                                                         apaOffRht + apaMulRht * uRes,
                                                         lpPrimSpec);
                            break;

                        case ARRAY_FLOAT:   // Res = FLOAT, Lft = BOOL/INT(S), Rht = FLOAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (aplFloatLft,
                                                         *((LPAPLFLOAT) lpMemRht)++,
                                                         lpPrimSpec);
                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_FLOAT:           // Res = FLOAT, Lft = FLOAT(S)
                    // Split cases based upon the storage type of the right arg
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = FLOAT, Lft = FLOAT(S), Rht = BOOL(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (aplFloatLft,
                                                         BIT0 & ((*(LPAPLBOOL) lpMemRht) >> uBitIndex),
                                                         lpPrimSpec);
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = FLOAT, Lft = FLOAT(S), Rht = INT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (aplFloatLft,
                                                         (APLFLOAT) *((LPAPLINT) lpMemRht)++,
                                                         lpPrimSpec);
                            break;

                        case ARRAY_APA:     // Res = FLOAT, Lft = FLOAT(S), Rht = APA(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (aplFloatLft,
                                                         (APLFLOAT) (apaOffRht + apaMulRht * uRes),
                                                         lpPrimSpec);
                            break;

                        case ARRAY_FLOAT:   // Res = FLOAT, Lft = FLOAT(S), Rht = FLOAT(M)
                            // Loop through the right arg/result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (aplFloatLft,
                                                         *((LPAPLFLOAT) lpMemRht)++,
                                                         lpPrimSpec);
                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH
    } __except (CheckException (GetExceptionInformation (), lpPrimSpec))
    {
#ifdef DEBUG
        dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
#endif
        // Split cases based upon the ExecCode
        switch (*lpPrimSpec->lpExecCode)
        {
            case EXEC_DOMAIN_ERROR:
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                return FALSE;

            case EXEC_RESULT_FLOAT:
                *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                if (IsSimpleNum (aplTypeRes)
                 && aplTypeRes NE ARRAY_FLOAT)
                {
                    aplTypeRes = ARRAY_FLOAT;

                    // Restart the pointers
                    lpMemRes = lpMemResStart;
                    lpMemRht = lpMemRhtStart;
#ifdef DEBUG
                    dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
#endif
                    goto RESTART_EXCEPTION;
                } // End IF

                // Fall through to never-never-land

            defstop
                break;
        } // End SWITCH
    } // End __try/__except

    return TRUE;
} // End PrimFnDydSingMult_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydMultSing_EM
//
//  Primitive scalar dyadic function,
//    left simple multipleton, right simple singleton
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydMultSing_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydMultSing_EM
    (APLSTYPE   aplTypeRes,         // Result type
     LPVOID     lpMemRes,           // Ptr to result memory (Points to the data)
     APLNELM    aplNELMRes,         // Result NELM
     APLSTYPE   aplTypeLft,         // Left arg type
     APLINT     apaOffLft,          // ...      APA offset
     APLINT     apaMulLft,          // ...      ... multiplier
     LPVOID     lpMemLft,           // Points to the data
     APLSTYPE   aplTypeRht,         // Right arg type
     APLINT     aplIntegerRht,      // ...       integer value
     APLFLOAT   aplFloatRht,        // ...       float   ...
     APLCHAR    aplCharRht,         // ...       char    ...
     LPTOKEN    lptkFunc,           // Ptr to function token
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLINT   uRes;
    UINT     uBitIndex = 0;
    LPVOID   lpMemResStart,
             lpMemLftStart;
    APLSTYPE aplTypeHet;
    APLINT   aplIntegerLft;
    APLFLOAT aplFloatLft;
    APLCHAR  aplCharLft;

    // If the singleton is integer,
    //   attempt to demote it to Boolean
    if (aplTypeRht EQ ARRAY_INT
     && IsBooleanValue (aplIntegerRht))
        aplTypeRht = ARRAY_BOOL;

    // Save the starting values in case we need
    //   to restart from an exception
    lpMemResStart = lpMemRes;
    lpMemLftStart = lpMemLft;
RESTART_EXCEPTION:
    __try
    {
    // Split cases based upon the result's storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:                    // Res = BOOL
            // Split off character or heterogeneous (CH) arguments
            if (IsSimpleCH (aplTypeLft)
             || IsSimpleCH (aplTypeRht))
            {
                APLBOOL bCvN;

                Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                     || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);

                // Calculate the result of CvN
                bCvN = (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);

                // If both arguments are CHAR,
                //   use BisCvC
                if (aplTypeLft EQ ARRAY_CHAR
                 && aplTypeRht EQ ARRAY_CHAR)
                {
                    // Loop through the result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    {
                        *((LPAPLBOOL)  lpMemRes) |=
                          (*lpPrimSpec->BisCvC) (*((LPAPLCHAR) lpMemLft)++,
                                                 aplCharRht,
                                                 lpPrimSpec) << uBitIndex;
                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;                  // Start over
                            ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                        } // End IF
                    } // End FOR
                } else
                // If the other argument is simple numeric,
                //   use BisCvN
                if ((IsSimpleNum (aplTypeLft) && aplTypeRht EQ ARRAY_CHAR)
                 || (IsSimpleNum (aplTypeRht) && aplTypeLft EQ ARRAY_CHAR))
                {
                    // Loop through the result
                    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                    {
                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;                  // Start over
                            ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                        } // End IF
                    } // End FOR
                } else
                // If the multipleton argument is HETERO,
                //   look at each LPSYMENTRY
                if (aplTypeLft EQ ARRAY_HETERO)
                {
                    // Split cases based upon the right arg's storage type
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = BOOL,                  Rht = BOOL (S)
                        case ARRAY_INT:     // Res = BOOL,                  Rht = INT  (S)
                            // Loop through the left argument
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                aplTypeHet = GetNextHetero (lpMemLft, uRes, &aplIntegerLft, &aplFloatLft, &aplCharLft);

                                // Split cases based upon the left arg's item's storage type
                                switch (aplTypeHet)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Lft = BOOL (M), Rht = BOOL/INT (S)
                                    case ARRAY_INT:     // Res = BOOL, Lft = INT  (M), Rht = BOOL/INT (S)
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                                 aplIntegerRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_FLOAT:   // Res = BOOL, Lft = FLOAT(M), Rht = BOOL/INT (S)
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                                 aplFloatRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_CHAR:    // Res = BOOL, Lft = CHAR (M), Rht = BOOL/INT (S)
                                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = BOOL,                  Rht = FLOAT(S)
                            // Loop through the left argument
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                aplTypeHet = GetNextHetero (lpMemLft, uRes, &aplIntegerLft, &aplFloatLft, &aplCharLft);

                                // Split cases based upon the left arg's item's storage type
                                switch (aplTypeHet)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Lft = BOOL (M), Rht = FLOAT (S)
                                    case ARRAY_INT:     // Res = BOOL, Lft = INT  (M), Rht = FLOAT (S)
                                    case ARRAY_FLOAT:   // Res = BOOL, Lft = FLOAT(M), Rht = FLOAT (S)
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                                 aplFloatRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    case ARRAY_CHAR:    // Res = BOOL, Lft = CHAR (M), Rht = FLOAT (S)
                                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_CHAR:    // Res = BOOL,                  Rht = CHAR (S)
                            // Loop through the left argument
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                aplTypeHet = GetNextHetero (lpMemLft, uRes, &aplIntegerLft, &aplFloatLft, &aplCharLft);

                                // Split cases based upon the left arg's item's storage type
                                switch (aplTypeHet)
                                {
                                    case ARRAY_BOOL:    // Res = BOOL, Lft = BOOL (M), Rht = CHAR  (S)
                                    case ARRAY_INT:     // Res = BOOL, Lft = INT  (M), Rht = CHAR  (S)
                                    case ARRAY_FLOAT:   // Res = BOOL, Lft = FLOAT(M), Rht = CHAR  (S)
                                        *((LPAPLBOOL)  lpMemRes) |= (bCvN << uBitIndex);

                                        break;

                                    case ARRAY_CHAR:    // Res = BOOL, Lft = CHAR (M), Rht = CHAR  (S)
                                        *((LPAPLBOOL)  lpMemRes) |=
                                          (*lpPrimSpec->BisCvC) (aplCharLft,
                                                                 aplCharRht,
                                                                 lpPrimSpec) << uBitIndex;
                                        break;

                                    defstop
                                        break;
                                } // End SWITCH

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } else
                    DbgStop ();         // We should never get here
            } else
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:            // Res = BOOL,                  Rht = BOOL(S)
                    if (aplTypeLft EQ ARRAY_BOOL)
                    {
                        // ***FIXME*** -- Optimize by chunking

                        // Loop through the result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisBvB) ((APLBOOL) (BIT0 & ((*(LPAPLBOOL) lpMemLft) >> uBitIndex)),
                                                     (APLBOOL) aplIntegerRht,
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemLft)++;       // Skip to next byte
                                ((LPAPLBOOL) lpMemRes)++;       // ...
                            } // End IF
                        } // End FOR

                        break;
                    } // End IF

                    // Fall through to common code

                case ARRAY_INT:             // Res = BOOL,                  Rht = INT(S)
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = BOOL, Lft = BOOL(M),   Rht = INT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisIvI) (BIT0 & ((*(LPAPLBOOL) lpMemLft) >> uBitIndex),
                                                         aplIntegerRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemLft)++;       // Skip to next byte
                                    ((LPAPLBOOL) lpMemRes)++;       // ...
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = BOOL, Lft = INT (M),   Rht = BOOL/INT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisIvI) (*((LPAPLINT) lpMemLft)++,
                                                         aplIntegerRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = BOOL, Lft = APA (M),   Rht = BOOL/INT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisIvI) (apaOffLft + apaMulLft * uRes,
                                                         aplIntegerRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = BOOL, Lft = FLOAT(M),  Rht = BOOL/INT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) (*((LPAPLFLOAT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_FLOAT:           // Res = BOOL,                  Rht = FLOAT(S)
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = BOOL, Lft = BOOL (M),  Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) (BIT0 & ((*(LPAPLBOOL) lpMemLft) >> uBitIndex),
                                                         aplFloatRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemLft)++;       // Skip to next byte
                                    ((LPAPLBOOL) lpMemRes)++;       // ...
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = BOOL, Lft = INT  (M),  Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) ((APLFLOAT) *((LPAPLINT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_APA:     // Res = BOOL, Lft = APA  (M),  Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) ((APLFLOAT) (apaOffLft + apaMulLft * uRes),
                                                         aplFloatRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:   // Res = BOOL, Lft = FLOAT(M),  Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLBOOL)  lpMemRes) |=
                                  (*lpPrimSpec->BisFvF) (*((LPAPLFLOAT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec) << uBitIndex;
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_INT:                     // Res = INT
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:            // Res = INT,                   Rht = BOOL(S)
                case ARRAY_INT:             // Res = INT,                   Rht = INT(S)
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = INT, Lft = BOOL(M),    Rht = BOOL/INT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisIvI) (BIT0 & ((*(LPAPLBOOL) lpMemLft) >> uBitIndex),
                                                         aplIntegerRht,
                                                         lpPrimSpec);
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemLft)++;    // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = INT, Lft = INT (M),    Rht = BOOL/INT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisIvI) (*((LPAPLINT) lpMemLft)++,
                                                         aplIntegerRht,
                                                         lpPrimSpec);
                            break;

                        case ARRAY_APA:     // Res = INT, Lft = APA (M),    Rht = BOOL/INT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisIvI) (apaOffLft + apaMulLft * uRes,
                                                         aplIntegerRht,
                                                         lpPrimSpec);
                            break;

                        case ARRAY_FLOAT:   // Res = INT, Lft = FLOAT(M),   Rht = BOOL/INT(S)
                            DbgBrk ();      // ***TESTME*** -- No such primitive

                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) (*((LPAPLFLOAT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec);
                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_FLOAT:           // Res = INT,                   Rht = FLOAT(S)
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = INT, Lft = BOOL (M),   Rht = FLOAT(S)
                            DbgBrk ();      // ***TESTME*** -- No such primitive

                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) (BIT0 & ((*(LPAPLBOOL) lpMemLft) >> uBitIndex),
                                                         aplFloatRht,
                                                         lpPrimSpec);
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemLft)++;    // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = INT, Lft = INT  (M),   Rht = FLOAT(S)
                            DbgBrk ();      // ***TESTME*** -- No such primitive

                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) ((APLFLOAT) *((LPAPLINT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec);
                            break;

                        case ARRAY_APA:     // Res = INT, Lft = APA  (M),   Rht = FLOAT(S)
                            DbgBrk ();      // ***TESTME*** -- No such primitive

                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) ((APLFLOAT) (apaOffLft + apaMulLft * uRes),
                                                         aplFloatRht,
                                                         lpPrimSpec);
                            break;

                        case ARRAY_FLOAT:   // Res = INT, Lft = FLOAT(M),   Rht = FLOAT(S)
                            DbgBrk ();      // ***TESTME*** -- No such primitive

                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLINT)   lpMemRes)++ =
                                  (*lpPrimSpec->IisFvF) (*((LPAPLFLOAT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec);
                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_FLOAT:                   // Res = FLOAT
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:            // Res = FLOAT,                 Rht = BOOL(S)
                case ARRAY_INT:             // Res = FLOAT,                 Rht = INT (S)
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = FLOAT, Lft = BOOL (M), Rht = BOOL/INT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisIvI) (BIT0 & ((*(LPAPLBOOL) lpMemLft) >> uBitIndex),
                                                         aplIntegerRht,
                                                         lpPrimSpec);
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemLft)++;    // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = FLOAT, Lft = INT  (M), Rht = BOOL/INT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisIvI) (*((LPAPLINT) lpMemLft)++,
                                                         aplIntegerRht,
                                                         lpPrimSpec);
                            break;

                        case ARRAY_APA:     // Res = FLOAT, Lft = APA  (M), Rht = BOOL/INT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisIvI) (apaOffLft + apaMulLft * uRes,
                                                         aplIntegerRht,
                                                         lpPrimSpec);
                            break;

                        case ARRAY_FLOAT:   // Res = FLOAT, Lft = FLOAT(M), Rht = BOOL/INT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (*((LPAPLFLOAT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec);
                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case ARRAY_FLOAT:           // Res = FLOAT,                 Rht = FLOAT(S)
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = FLOAT, Lft = BOOL (M), Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (BIT0 & ((*(LPAPLBOOL) lpMemLft) >> uBitIndex),
                                                         aplFloatRht,
                                                         lpPrimSpec);
                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;                  // Start over
                                    ((LPAPLBOOL) lpMemLft)++;    // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:     // Res = FLOAT, Lft = INT  (M), Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) ((APLFLOAT) *((LPAPLINT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec);
                            break;

                        case ARRAY_APA:     // Res = FLOAT, Lft = APA  (M), Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) ((APLFLOAT) (apaOffLft + apaMulLft * uRes),
                                                         aplFloatRht,
                                                         lpPrimSpec);
                            break;

                        case ARRAY_FLOAT:   // Res = FLOAT, Lft = FLOAT(M), Rht = FLOAT(S)
                            // Loop through the result
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                                *((LPAPLFLOAT) lpMemRes)++ =
                                  (*lpPrimSpec->FisFvF) (*((LPAPLFLOAT) lpMemLft)++,
                                                         aplFloatRht,
                                                         lpPrimSpec);
                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH
    } __except (CheckException (GetExceptionInformation (), lpPrimSpec))
    {
#ifdef DEBUG
        dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
#endif
        // Split cases based upon the ExecCode
        switch (*lpPrimSpec->lpExecCode)
        {
            case EXEC_DOMAIN_ERROR:
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                return FALSE;

            case EXEC_RESULT_FLOAT:
                *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                if (IsSimpleNum (aplTypeRes)
                 && aplTypeRes NE ARRAY_FLOAT)
                {
                    aplTypeRes = ARRAY_FLOAT;

                    // Restart the pointers
                    lpMemRes = lpMemResStart;
                    lpMemLft = lpMemLftStart;
#ifdef DEBUG
                    dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
#endif
                    goto RESTART_EXCEPTION;
                } // End IF

                // Fall through to never-never-land

            defstop
                break;
        } // End SWITCH
    } // End __try/__except

    return TRUE;
} // End PrimFnDydMultSing_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydSimpNestSub_EM
//
//  Subroutine to PrimFnDydSimpNest_EM
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSimpNestSub_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL PrimFnDydSimpNestSub_EM
    (LPTOKEN    lptkFunc,           // Ptr to function token
     APLSTYPE   aplTypeLft,         // Left arg type
     APLINT     aplIntegerLft,      // ...      integer value
     APLFLOAT   aplFloatLft,        // ...      float   ...
     APLCHAR    aplCharLft,         // ...      char    ...
     APLNESTED  aplNestedRht,       // Right arg nested value
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    BOOL     bRet = TRUE;
    HGLOBAL  hGlbRht = NULL,
             hGlbRes = NULL,
             hGlbSub;
    LPVOID   lpMemRht,
             lpMemRes = NULL;
    APLSTYPE aplTypeRht,
             aplTypeRes;
    APLNELM  aplNELMLft = 1,
             aplNELMRht,
             aplNELMRes;
    APLRANK  aplRankLft = 0,
             aplRankRht,
             aplRankRes;
    APLINT   uRht,
             apaOffRht,
             apaMulRht;
    UINT     uBitIndex = 0;

    DBGENTER;

    // The right arg data is a valid HGLOBAL array
    Assert (IsGlbTypeVarDir (aplNestedRht));

    // Clear the identifying bits
    hGlbRht = ClrPtrTypeDirGlb (aplNestedRht);

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)

    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;

#undef  lpHeader

    // The NELM of the result is the larger of the two arg's NELMs
    aplNELMRes = max (aplNELMLft, aplNELMRht);

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                               &aplTypeLft,
                                                lptkFunc,
                                                aplNELMRht,
                                               &aplTypeRht);
    if (aplTypeRes EQ ARRAY_ERROR)
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Special case APA result
    if (aplTypeRes EQ ARRAY_APA)
    {
        if (!(*lpPrimSpec->ApaResultDyd_EM) (NULL,
                                             lptkFunc,
                                             NULL,
                                             hGlbRht,
                                            &hGlbRes,
                                            &lpMemRes,
                                             aplRankLft,
                                             aplRankRht,
                                             aplNELMLft,
                                             aplNELMRht,
                                             aplIntegerLft,
                                             lpPrimSpec))
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } // End IF

    Assert (IsSimpleNum (aplTypeRes)
         || aplTypeRes EQ ARRAY_NESTED);

    // Allocate space for result
    if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                    &hGlbRes,
                                     NULL,
                                     lpMemRht,
                                    &lpMemRes,
                                     aplRankLft,
                                     aplRankRht,
                                    &aplRankRes,
                                     aplTypeRes,
                                     aplNELMLft,
                                     aplNELMRht,
                                     aplNELMRes))
        goto ERROR_EXIT;

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    // If the right arg is an APA, get its parameters

    if (aplTypeRht EQ ARRAY_APA)
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // If simple result, ...
    if (IsSimpleNum (aplTypeRes))
    {
        bRet = PrimFnDydSingMult_EM (aplTypeRes,
                                     lpMemRes,
                                     aplNELMRes,
                                     aplTypeLft,
                                     aplIntegerLft,
                                     aplFloatLft,
                                     aplCharLft,
                                     aplTypeRht,
                                     apaOffRht,
                                     apaMulRht,
                                     lpMemRht,
                                     lptkFunc,
                                     lpPrimSpec);
    } else
    // If nested result, ...
    if (aplTypeRes EQ ARRAY_NESTED)
    {
////case ARRAY_NESTED:              // Res = NESTED, Lft = BOOL/INT/FLOAT(S), Rht = NESTED
#ifndef PRIMPROTOFNSCALAR
        // Handle the prototype case
        aplNELMRht = max (aplNELMRht, 1);
#endif
        // Loop through the right arg/result
        for (uRht = 0; bRet && uRht < (APLNELMSIGN) aplNELMRht; uRht++)
        {
            hGlbSub = PrimFnDydSimpNestSub_EM (lptkFunc,
                                               aplTypeLft,
                                               aplIntegerLft,
                                               aplFloatLft,
                                               aplCharLft,
                                               *((LPAPLNESTED) lpMemRht)++,
                                               lpPrimSpec);
            if (!hGlbSub)
                bRet = FALSE;
            else
                *((LPAPLNESTED) lpMemRes)++ = MakeGlbTypeGlb (hGlbSub);
        } // End FOR
    } else
        DbgStop ();         // We should never get here

    if (bRet)
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
    if (lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

    DBGLEAVE;

    if (bRet)
        return hGlbRes;
    else
        return NULL;
} // End PrimFnDydSimpNestSub_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydSimpSimp_EM
//
//  Dyadic primitive scalar function, left simple, right simple
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSimpSimp_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydSimpSimp_EM
    (LPYYSTYPE  lpYYRes,            // Ptr to result token

     LPTOKEN    lptkLftArg,         // Ptr to left arg token
     LPTOKEN    lptkFunc,           // ...    function ...
     LPTOKEN    lptkRhtArg,         // ...    right arg ...

     HGLOBAL    hGlbLft,            // Left arg handle
     HGLOBAL    hGlbRht,            // Right ...
     HGLOBAL   *lphGlbRes,          // Ptr to result handle

     LPVOID     lpMemLft,           // Points to sign.ature
     LPVOID     lpMemRht,           // ...
     LPVOID     lpMemRes,           // ...

     LPAPLUINT  lpMemAxisHead,      // Ptr to axis values, fleshed out
     LPAPLUINT  lpMemAxisTail,      // Ptr to grade up of AxisHead

     APLRANK    aplRankLft,         // Left arg rank
     APLRANK    aplRankRht,         // Right ...
     APLRANK    aplRankRes,         // Result ...

     APLSTYPE   aplTypeLft,         // Left arg type
     APLSTYPE   aplTypeRht,         // Right ...
     APLSTYPE   aplTypeRes,         // Result ...

     APLNELM    aplNELMLft,         // Left arg NELM
     APLNELM    aplNELMRht,         // Right ...
     APLNELM    aplNELMRes,         // Result ...
     APLNELM    aplNELMAxis,        // Axis ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    BOOL      bRet = TRUE;
    APLSTYPE  aplTypeArg;
    APLINT    aplIntegerLft,
              aplIntegerRht;
    APLFLOAT  aplFloatLft,
              aplFloatRht;
    LPAPLUINT lpMemWVec = NULL,
              lpMemOdo = NULL;
    HGLOBAL   hGlbWVec = NULL,
              hGlbOdo = NULL;
    APLINT    uLft,
              uRht,
              uRes;
    LPAPLDIM  lpMemDimArg,
              lpMemDimRes;
    APLINT    apaOffLft,
              apaMulLft,
              apaOffRht,
              apaMulRht,
              iRht;
    UINT      uBitIndex = 0;
    LPVOID    lpMemResStart;
    APLUINT   ByteRes;
    APLCHAR   aplCharLft,
              aplCharRht;

    DBGENTER;

    //***************************************************************
    // Both arguments are simple
    //***************************************************************

    if (aplNELMLft EQ 1
     && aplNELMRht EQ 1)
    {
        //***************************************************************
        // Both args are singletons
        //***************************************************************

        // Axis may be anything

        // If the result is a scalar, ...
        if (aplRankRes EQ 0)
        {
            UINT     immType;

            // Get the respective first values
            FirstValue (lptkLftArg,         // Ptr to right arg token
                       &aplIntegerLft,      // Ptr to integer result
                       &aplFloatLft,        // Ptr to float ...
                       &aplCharLft,         // Ptr to WCHAR ...
                        NULL,               // Ptr to longest ...
                        NULL,               // Ptr to lpSym/Glb ...
                        NULL,               // Ptr to ...immediate type ...
                        NULL);              // Ptr to array type ...
            FirstValue (lptkRhtArg,         // Ptr to right arg token
                       &aplIntegerRht,      // Ptr to integer result
                       &aplFloatRht,        // Ptr to float ...
                       &aplCharRht,         // Ptr to WCHAR ...
                        NULL,               // Ptr to longest ...
                        NULL,               // Ptr to lpSym/Glb ...
                        NULL,               // Ptr to ...immediate type ...
                        NULL);              // Ptr to array type ...
RESTART_EXCEPTION_IMMED:
            // Get the immediate type for the token
            immType = TranslateArrayTypeToImmType (aplTypeRes);

            Assert (IMMTYPE_BOOL  EQ immType
                 || IMMTYPE_INT   EQ immType
                 || IMMTYPE_FLOAT EQ immType);

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType   = immType;
////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0; // Already zero from YYAlloc
            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

            __try
            {
            // Split cases based upon the result's storage type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:                    // Res = BOOL
                    // If both arguments are BOOL,
                    //   use BisBvB
                    if (aplTypeLft EQ ARRAY_BOOL
                     && aplTypeRht EQ ARRAY_BOOL)  // Res = BOOL, Lft = BOOL(S), Rht = BOOL(S)
                    {
                        lpYYRes->tkToken.tkData.tkBoolean  =
                          (*lpPrimSpec->BisBvB) ((APLBOOL) aplIntegerLft,
                                                 (APLBOOL) aplIntegerRht,
                                                 lpPrimSpec);
                    } else
                    // If both arguments are integer-like (BOOL, INT, or APA),
                    //   use BisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))  // Res = BOOL, Lft = BOOL/INT/APA(S), Rht = BOOL/INT/APA(S)
                    {
                        lpYYRes->tkToken.tkData.tkBoolean  =
                          (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                 aplIntegerRht,
                                                 lpPrimSpec);
                    } else
                    // If both arguments are CHAR,
                    //   use BisCvC
                    if (aplTypeLft EQ ARRAY_CHAR
                     && aplTypeRht EQ ARRAY_CHAR)  // Res = BOOL, Lft = CHAR(S), Rht = CHAR(S)
                    {
                        lpYYRes->tkToken.tkData.tkBoolean  =
                          (*lpPrimSpec->BisCvC) (aplCharLft,
                                                 aplCharRht,
                                                 lpPrimSpec);
                    } else
                    // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                    //   use BisFvF
                    if ((aplTypeLft EQ ARRAY_FLOAT && IsSimpleNum (aplTypeRht))   // Res = BOOL, Lft = FLOAT(S), Rht = BOOL/INT/APA/FLOAT(S)
                     || (aplTypeRht EQ ARRAY_FLOAT && IsSimpleNum (aplTypeLft)))  // Res = BOOL, Lft = BOOL/INT/APA/FLOAT(S), Rht = FLOAT(S)
                    {
                        lpYYRes->tkToken.tkData.tkBoolean  =
                          (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                 aplFloatRht,
                                                 lpPrimSpec);
                    } else
                        DbgStop ();         // We should never get here
                    break;

                case ARRAY_INT:                     // Res = INT
                    // If both left and right arguments are integer-like (BOOL, INT, or APA),
                    //   use IisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))  // Res = INT, Lft = BOOL/INT/APA(S), Rht = BOOL/INT/APA(S)
                    {
                        lpYYRes->tkToken.tkData.tkInteger  =
                          (*lpPrimSpec->IisIvI) (aplIntegerLft,
                                                 aplIntegerRht,
                                                 lpPrimSpec);
                    } else
                    // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                    //   use IisFvF
                    if ((aplTypeLft EQ ARRAY_FLOAT && IsSimpleNum (aplTypeRht))   // Res = INT, Lft = FLOAT(S), Rht = BOOL/INT/APA/FLOAT(S)
                     || (aplTypeRht EQ ARRAY_FLOAT && IsSimpleNum (aplTypeLft)))  // Res = INT, Lft = BOOL/INT/APA/FLOAT(S), Rht = FLOAT(S)
                    {
                        lpYYRes->tkToken.tkData.tkInteger  =
                          (*lpPrimSpec->IisFvF) (aplFloatLft,
                                                 aplFloatRht,
                                                 lpPrimSpec);
                    } else
                        DbgStop ();         // We should never get here
                    break;

                case ARRAY_FLOAT:                   // Res = FLOAT
                    // If both arguments are simple integer (BOOL, INT, APA),
                    //   use FisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))  // Res = FLOAT, Lft = BOOL/INT/APA(S), Rht = BOOL/INT/APA(S)
                    {
                        lpYYRes->tkToken.tkData.tkFloat    =
                          (*lpPrimSpec->FisIvI) (aplIntegerLft,
                                                 aplIntegerRht,
                                                 lpPrimSpec);
                    } else
                    // If both arguments are simple numeric (BOOL, INT, APA, FLOAT),
                    //   use FisFvF
                    if (IsSimpleNum (aplTypeLft)
                     && IsSimpleNum (aplTypeRht))  // Res = FLOAT, Lft = BOOL/INT/APA/FLOAT(S), Rht = BOOL/INT/APA/FLOAT(S)
                    {
                        lpYYRes->tkToken.tkData.tkFloat    =
                          (*lpPrimSpec->FisFvF) (aplFloatLft,
                                                 aplFloatRht,
                                                 lpPrimSpec);
                    } else
                        DbgStop ();         // We should never get here
                    break;

                defstop
                    break;
            } // End SWITCH
            } __except (CheckException (GetExceptionInformation (), lpPrimSpec))
            {
#ifdef DEBUG
                dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
#endif
                // Split cases based upon the ExecCode
                switch (*lpPrimSpec->lpExecCode)
                {
                    case EXEC_DOMAIN_ERROR:
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:
                        *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                           lptkFunc);
                        bRet = FALSE;

                        goto ERROR_EXIT;

                    case EXEC_RESULT_FLOAT:
                        *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                        if (IsSimpleNum (aplTypeRes)
                         && aplTypeRes NE ARRAY_FLOAT)
                        {
                            aplTypeRes = ARRAY_FLOAT;
#ifdef DEBUG
                            dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
#endif
                            goto RESTART_EXCEPTION_IMMED;
                        } // End IF

                        // Fall through to never-never-land

                    defstop
                        break;
                } // End SWITCH
            } // End __try/__except
        } else
        // It's a singleton array
        {
            // Skip over the header and dimensions to the data
            lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

            // Get the respective values
            FirstValue (lptkLftArg,         // Ptr to right arg token
                       &aplIntegerLft,      // Ptr to integer result
                       &aplFloatLft,        // Ptr to float ...
                       &aplCharLft,         // Ptr to WCHAR ...
                        NULL,               // Ptr to longest ...
                        NULL,               // Ptr to lpSym/Glb ...
                        NULL,               // Ptr to ...immediate type ...
                        NULL);              // Ptr to array type ...
            FirstValue (lptkRhtArg,         // Ptr to right arg token
                       &aplIntegerRht,      // Ptr to integer result
                       &aplFloatRht,        // Ptr to float ...
                       &aplCharRht,         // Ptr to WCHAR ...
                        NULL,               // Ptr to longest ...
                        NULL,               // Ptr to lpSym/Glb ...
                        NULL,               // Ptr to ...immediate type ...
                        NULL);              // Ptr to array type ...
RESTART_EXCEPTION_SINGLETON:
            __try
            {
            // Split cases based upon the result's storage type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:                    // Res = BOOL
                    // If both arguments are BOOL,
                    //   use BisBvB
                    if (aplTypeLft EQ ARRAY_BOOL
                     && aplTypeRht EQ ARRAY_BOOL)  // Res = BOOL, Lft = BOOL(S), Rht = BOOL(S)
                    {
                        *((LPAPLBOOL)  lpMemRes) =
                          (*lpPrimSpec->BisBvB) ((APLBOOL) aplIntegerLft,
                                                 (APLBOOL) aplIntegerRht,
                                                 lpPrimSpec);
                    } else
                    // If both arguments are integer-like (BOOL, INT, or APA),
                    //   use BisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeLft))  // Res = BOOL, Lft = BOOL/INT/APA(S), Rht = BOOL/INT/APA(S)
                    {
                        *((LPAPLBOOL)  lpMemRes) =
                          (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                 aplIntegerRht,
                                                 lpPrimSpec);
                    } else
                    // If both arguments are CHAR,
                    //   use BisCvC
                    if (aplTypeLft EQ ARRAY_CHAR
                     && aplTypeRht EQ ARRAY_CHAR)
                    {
                        *((LPAPLBOOL)  lpMemRes) =
                          (*lpPrimSpec->BisCvC) (aplCharLft,
                                                 aplCharRht,
                                                 lpPrimSpec);
                    } else
                    // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                    //   use BisFvF
                    if ((aplTypeLft EQ ARRAY_FLOAT && IsSimpleNum (aplTypeRht))   // Res = BOOL, Lft = FLOAT(S), Rht = BOOL/INT/APA/FLOAT(S)
                     || (aplTypeRht EQ ARRAY_FLOAT && IsSimpleNum (aplTypeLft)))  // Res = BOOL, Lft = BOOL/INT/APA/FLOAT(S), Rht = FLOAT(S)
                    {
                        *((LPAPLBOOL)  lpMemRes) =
                          (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                 aplFloatRht,
                                                 lpPrimSpec);
                    } else
                        DbgStop ();         // We should never get here
                    break;

                case ARRAY_INT:                     // Res = INT
                    // If both left and right arguments are integer-like (BOOL, INT, or APA),
                    //   use IisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))  // Res = INT, Lft = BOOL/INT/APA(S), Rht = BOOL/INT/APA(S)
                    {
                        *((LPAPLINT)   lpMemRes) =
                          (*lpPrimSpec->IisIvI) (aplIntegerLft,
                                                 aplIntegerRht,
                                                 lpPrimSpec);
                    } else
                    // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                    //   use IisFvF
                    if ((aplTypeLft EQ ARRAY_FLOAT && IsSimpleNum (aplTypeRht))   // Res = INT, Lft = FLOAT(S), Rht = BOOL/INT/APA/FLOAT(S)
                     || (aplTypeRht EQ ARRAY_FLOAT && IsSimpleNum (aplTypeLft)))  // Res = INT, Lft = BOOL/INT/APA/FLOAT(S), Rht = FLOAT(S)
                    {
                        *((LPAPLINT)   lpMemRes) =
                          (*lpPrimSpec->IisFvF) (aplFloatLft,
                                                 aplFloatRht,
                                                 lpPrimSpec);
                    } else
                        DbgStop ();         // We should never get here
                    break;

                case ARRAY_FLOAT:                   // Res = FLOAT
                    // If both arguments are simple integer (BOOL, INT, APA),
                    //   use FisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))  // Res = FLOAT, Lft = BOOL/INT/APA(S), Rht = BOOL/INT/APA(S)
                    {
                        *((LPAPLFLOAT) lpMemRes) =
                          (*lpPrimSpec->FisIvI) (aplIntegerLft,
                                                 aplIntegerRht,
                                                 lpPrimSpec);
                    } else
                    // If both arguments are simple numeric (BOOL, INT, APA, FLOAT),
                    //   use FisFvF
                    if (IsSimpleNum (aplTypeLft)
                     && IsSimpleNum (aplTypeRht))  // Res = FLOAT, Lft = BOOL/INT/APA/FLOAT(S), Rht = BOOL/INT/APA/FLOAT(S)
                    {
                        *((LPAPLFLOAT) lpMemRes) =
                          (*lpPrimSpec->FisFvF) (aplFloatLft,
                                                 aplFloatRht,
                                                 lpPrimSpec);
                    } else
                        DbgStop ();         // We should never get here
                    break;

                defstop
                    break;
            } // End SWITCH
            } __except (CheckException (GetExceptionInformation (), lpPrimSpec))
            {
#ifdef DEBUG
                dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
#endif
                // Split cases based upon the ExecCode
                switch (*lpPrimSpec->lpExecCode)
                {
                    case EXEC_DOMAIN_ERROR:
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:
                        *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                           lptkFunc);
                        bRet = FALSE;

                        goto ERROR_EXIT;

                    case EXEC_RESULT_FLOAT:
                        *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                        if (IsSimpleNum (aplTypeRes)
                         && aplTypeRes NE ARRAY_FLOAT)
                        {
                            aplTypeRes = ARRAY_FLOAT;
#ifdef DEBUG
                            dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
#endif
                            goto RESTART_EXCEPTION_SINGLETON;
                        } // End IF

                        // Fall through to never-never-land

                    defstop
                        break;
                } // End SWITCH
            } // End __try/__except

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = 0; // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0; // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (*lphGlbRes);
        } // End IF/ELSE

        // Finish with common code
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } else
    if (aplNELMLft EQ 1
     || aplNELMRht EQ 1)
    {
        //***************************************************************
        // One of the args is a singleton, the other not
        //***************************************************************

        // Axis may be anything

        // Copy the ptr of the non-singleton argument
        if (aplNELMLft NE 1)
        {
            lpMemDimArg = lpMemLft;
            aplTypeArg  = aplTypeLft;
        } else
        {
            lpMemDimArg = lpMemRht;
            aplTypeArg  = aplTypeRht;
        } // End IF/ELSE

        // Skip over the header and dimensions to the data
        lpMemRes    = VarArrayBaseToData (lpMemRes,    aplRankRes);
        lpMemDimArg = VarArrayBaseToData (lpMemDimArg, aplRankRes);

        // If the non-singleton arg is an APA, get its parameters
        if (aplTypeArg EQ ARRAY_APA)
        {
#define lpAPA       ((LPAPLAPA) lpMemDimArg)
            apaOffLft = apaOffRht = lpAPA->Off;
            apaMulLft = apaMulRht = lpAPA->Mul;
#undef  lpAPA
        } // End IF

        // Get the value of the singleton
        if (aplNELMLft EQ 1)
            FirstValue (lptkLftArg,         // Ptr to right arg token
                       &aplIntegerLft,      // Ptr to integer result
                       &aplFloatLft,        // Ptr to float ...
                       &aplCharLft,         // Ptr to WCHAR ...
                        NULL,               // Ptr to longest ...
                        NULL,               // Ptr to lpSym/Glb ...
                        NULL,               // Ptr to ...immediate type ...
                        NULL);              // Ptr to array type ...
        else
            FirstValue (lptkRhtArg,         // Ptr to right arg token
                       &aplIntegerRht,      // Ptr to integer result
                       &aplFloatRht,        // Ptr to float ...
                       &aplCharRht,         // Ptr to WCHAR ...
                        NULL,               // Ptr to longest ...
                        NULL,               // Ptr to lpSym/Glb ...
                        NULL,               // Ptr to ...immediate type ...
                        NULL);              // Ptr to array type ...
        // Split cases based upon which argument is the simgleton
        if (aplNELMLft NE 1)    // Lft = Multipleton, Rht = Singleton
            bRet = PrimFnDydMultSing_EM (aplTypeRes,
                                         lpMemRes,
                                         aplNELMRes,
                                         aplTypeLft,
                                         apaOffLft,
                                         apaMulLft,
                                         lpMemDimArg,
                                         aplTypeRht,
                                         aplIntegerRht,
                                         aplFloatRht,
                                         aplCharRht,
                                         lptkFunc,
                                         lpPrimSpec);

        else                    // Lft = Singleton, Rht = Multipleton
            bRet = PrimFnDydSingMult_EM (aplTypeRes,
                                         lpMemRes,
                                         aplNELMRes,
                                         aplTypeLft,
                                         aplIntegerLft,
                                         aplFloatLft,
                                         aplCharLft,
                                         aplTypeRht,
                                         apaOffRht,
                                         apaMulRht,
                                         lpMemDimArg,
                                         lptkFunc,
                                         lpPrimSpec);
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (*lphGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } else
    {
        //***************************************************************
        // Neither arg is a singleton, but both are simple
        //***************************************************************

        // Axis is significant (if present)

        // Skip over the header to the dimensions
        lpMemDimRes = VarArrayBaseToDim (lpMemRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

        // Save the starting value in case we need
        //   to restart from an exception
        lpMemResStart = lpMemRes;

        // If the left arg is an APA, get its parameters
        if (aplTypeLft EQ ARRAY_APA)
        {
#define lpAPA       ((LPAPLAPA) lpMemLft)
            apaOffLft = lpAPA->Off;
            apaMulLft = lpAPA->Mul;
#undef  lpAPA
        } // End IF

        // If the right arg is an APA, get its parameters
        if (aplTypeRht EQ ARRAY_APA)
        {
#define lpAPA       ((LPAPLAPA) lpMemRht)
            apaOffRht = lpAPA->Off;
            apaMulRht = lpAPA->Mul;
#undef  lpAPA
        } // End IF

        // If the axis is significant, ...
        if (lpMemAxisHead && aplNELMAxis NE aplRankRes)
        {
            //***************************************************************
            // Allocate space for the weighting vector which is
            //
            //   {times}{backscan}1{drop}({rho}R)[Cx,Ax],1
            //
            //   where Ax contains the specified axes, and
            //   Cx contains the remaining axes.
            // N.B.  Conversion from APLUINT to UINT.
            //***************************************************************
            ByteRes = aplRankRes * sizeof (APLINT);
            Assert (ByteRes EQ (UINT) ByteRes);
            hGlbWVec = DbgGlobalAlloc (GHND, (UINT) ByteRes);
            if (!hGlbWVec)
            {
                ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                           lptkFunc);
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
            for (uRes = 1, iRht = aplRankRes - 1; iRht >= 0; iRht--)
            {
                lpMemWVec[iRht] = uRes;
                uRes *= lpMemDimRes[lpMemAxisHead[iRht]];
            } // End FOR

            //***************************************************************
            // Allocate space for the odometer array, one value per dimension
            //   in the right arg, with values initially all zero (thanks to GHND).
            // N.B.  Conversion from APLUINT to UINT.
            //***************************************************************
            ByteRes = aplRankRes * sizeof (APLINT);
            Assert (ByteRes EQ (UINT) ByteRes);
            hGlbOdo = DbgGlobalAlloc (GHND, (UINT) ByteRes);
            if (!hGlbOdo)
            {
                ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Lock the global memory to get a ptr to it
            lpMemOdo = MyGlobalLock (hGlbOdo);
RESTART_EXCEPTION_AXIS:
            __try
            {
            // Split cases based upon the result's storage type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:                    // Res = BOOL(Axis)
                    // If both arguments are BOOL,
                    //   use BisBvB
                    if (aplTypeLft EQ ARRAY_BOOL
                     && aplTypeRht EQ ARRAY_BOOL)   // Res = BOOL(Axis), Lft = BOOL, Rht = BOOL
                    {
                        // ***FIXME*** -- Optimize by chunking

                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisBvB) ((APLBOOL) GetNextInteger (lpMemLft, aplTypeLft, uLft, apaOffLft, apaMulLft),
                                                     (APLBOOL) GetNextInteger (lpMemRht, aplTypeRht, uRht, apaOffRht, apaMulRht),
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                    // If both arguments are integer-like (BOOL, INT, or APA),
                    //   use BisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeLft))   // Res = BOOL(Axis), Lft = BOOL/INT/APA, Rht = BOOL/INT/APA
                    {
                        DbgBrk ();      // ***TESTME*** -- No such primitive

                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uLft, apaOffLft, apaMulLft),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRht, apaOffRht, apaMulRht),
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                    // If both arguments are CHAR,
                    //   use BisCvC
                    if (aplTypeLft EQ ARRAY_CHAR
                     && aplTypeRht EQ ARRAY_CHAR)   // Res = BOOL(Axis), Lft = CHAR, Rht = CHAR
                    {
                        DbgBrk ();      // ***TESTME*** -- No such primitive

                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisCvC) (((LPAPLCHAR) lpMemLft)[uLft],
                                                     ((LPAPLCHAR) lpMemRht)[uRht],
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                    // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                    //   use BisFvF
                    if ((aplTypeLft EQ ARRAY_FLOAT && IsSimpleNum (aplTypeRht))   // Res = BOOL(Axis), Lft = FLOAT, Rht = BOOL/INT/APA/FLOAT
                     || (aplTypeRht EQ ARRAY_FLOAT && IsSimpleNum (aplTypeLft)))  // Res = BOOL(Axis), Lft = BOOL/INT/APA/FLOAT, Rht = FLOAT
                    {
                        DbgBrk ();      // ***TESTME*** -- No such primitive

                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uLft, apaOffLft, apaMulLft),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRht, apaOffRht, apaMulRht),
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                        DbgStop ();     // We should never get here
                    break;

                case ARRAY_INT:                     // Res = INT(Axis)
                    // If both left and right arguments are integer-like (BOOL, INT, or APA),
                    //   use IisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))   // Res = INT(Axis), Lft = BOOL/INT/APA, Rht = BOOL/INT/APA
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            *((LPAPLINT)   lpMemRes)++ =
                              (*lpPrimSpec->IisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uLft, apaOffLft, apaMulLft),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRht, apaOffRht, apaMulRht),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                    // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                    //   use IisFvF
                    if ((aplTypeLft EQ ARRAY_FLOAT && IsSimpleNum (aplTypeRht))   // Res = INT(Axis), Lft = FLOAT, Rht = BOOL/INT/APA/FLOAT
                     || (aplTypeRht EQ ARRAY_FLOAT && IsSimpleNum (aplTypeLft)))  // Res = INT(Axis), Rht = BOOL/INT/APA/FLOAT, Rht = FLOAT
                    {
                        DbgBrk ();      // ***TESTME*** -- No such primitive

                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            *((LPAPLINT)   lpMemRes)++ =
                              (*lpPrimSpec->IisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uLft, apaOffLft, apaMulLft),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRht, apaOffRht, apaMulRht),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                        DbgStop ();     // We should never get here

                    break;

                case ARRAY_FLOAT:                   // Res = FLOAT(Axis)
                    // If both arguments are simple integer (BOOL, INT, APA),
                    //   use FisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))   // Res = FLOAT(Axis), Lft = BOOL/INT/APA, Rht = BOOL/INT/APA
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            *((LPAPLFLOAT) lpMemRes)++ =
                              (*lpPrimSpec->FisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uLft, apaOffLft, apaMulLft),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRht, apaOffRht, apaMulRht),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                    // If both arguments are simple numeric (BOOL, INT, APA, FLOAT),
                    //   use FisFvF
                    if (IsSimpleNum (aplTypeLft)
                     && IsSimpleNum (aplTypeRht))   // Res = FLOAT(Axis), Lft = BOOL/INT/APA/FLOAT, Rht = BOOL/INT/APA/FLOAT
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Calculate the left and right argument indices
                            CalcLftRhtArgIndices (uRes,
                                                  aplRankRes,
                                                 &uLft,
                                                  aplRankLft,
                                                 &uRht,
                                                  aplRankRht,
                                                  aplNELMAxis,
                                                  lpMemAxisHead,
                                                  lpMemOdo,
                                                  lpMemWVec,
                                                  lpMemDimRes);
                            *((LPAPLFLOAT) lpMemRes)++ =
                              (*lpPrimSpec->FisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uLft, apaOffLft, apaMulLft),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRht, apaOffRht, apaMulRht),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                        DbgStop ();     // We should never get here
                    break;

                defstop
                    break;
            } // End SWITCH
            } __except (CheckException (GetExceptionInformation (), lpPrimSpec))
            {
#ifdef DEBUG
                dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
#endif
                // Split cases based upon the ExecCode
                switch (*lpPrimSpec->lpExecCode)
                {
                    case EXEC_DOMAIN_ERROR:
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:
                        *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        return FALSE;

                    case EXEC_RESULT_FLOAT:
                        *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                        if (IsSimpleNum (aplTypeRes)
                         && aplTypeRes NE ARRAY_FLOAT)
                        {
                            aplTypeRes = ARRAY_FLOAT;

                            // Restart the pointers
                            lpMemRes = lpMemResStart;

                            // Re-initialize lpMemOdo
                            for (uRes = 0 ; uRes < (APLRANKSIGN) aplRankRes; uRes++)
                                lpMemOdo[uRes] = 0;
#ifdef DEBUG
                            dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
#endif
                            goto RESTART_EXCEPTION_AXIS;
                        } // End IF

                        // Fall through to never-never-land

                    defstop
                        break;
                } // End SWITCH
            } // End __try/__except
        } else      // Axis is not significant, neither argument is singleton
        {
RESTART_EXCEPTION_NOAXIS:
            __try
            {
            // Split cases based upon the result's storage type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:                    // Res = BOOL(No Axis)
                    // If both arguments are BOOL,
                    //   use BisBvB
                    if (aplTypeLft EQ ARRAY_BOOL
                     && aplTypeRht EQ ARRAY_BOOL)   // Res = BOOL(No Axis), Lft = BOOL, Rht = BOOL
                    {
                        // ***FIXME*** -- Optimize by chunking

                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisBvB) ((APLBOOL) GetNextInteger (lpMemLft, aplTypeLft, uRes, apaOffLft, apaMulLft),
                                                     (APLBOOL) GetNextInteger (lpMemRht, aplTypeRht, uRes, apaOffRht, apaMulRht),
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                    // If both arguments are integer-like (BOOL, INT, or APA),
                    //   use BisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeLft))   // Res = BOOL(No Axis), Lft = BOOL/INT/APA, Rht = BOOL/INT/APA
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uRes, apaOffLft, apaMulLft),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRes, apaOffRht, apaMulRht),
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                    // If both arguments are CHAR,
                    //   use BisCvC
                    if (aplTypeLft EQ ARRAY_CHAR
                     && aplTypeRht EQ ARRAY_CHAR)   // Res = BOOL(No Axis), Lft = CHAR, Rht = CHAR
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisCvC) (((LPAPLCHAR) lpMemLft)[uRes],
                                                     ((LPAPLCHAR) lpMemRht)[uRes],
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                    // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                    //   use BisFvF
                    if ((aplTypeLft EQ ARRAY_FLOAT && IsSimpleNum (aplTypeRht))   // Res = BOOL(No Axis), Lft = FLOAT, Rht = BOOL/INT/APA/FLOAT
                     || (aplTypeRht EQ ARRAY_FLOAT && IsSimpleNum (aplTypeLft)))  // Res = BOOL(No Axis), Lft = BOOL/INT/APA/FLOAT, Rht = FLOAT
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uRes, apaOffLft, apaMulLft),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRes, apaOffRht, apaMulRht),
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                        DbgStop ();         // We should never get here
                    break;

                case ARRAY_INT:                     // Res = INT(No Axis)
                    // If both left and right arguments are integer-like (BOOL, INT, or APA),
                    //   use IisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))   // Res = INT(No Axis), Lft = BOOL/INT/APA, Rht = BOOL/INT/APA
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            *((LPAPLINT)   lpMemRes)++ =
                              (*lpPrimSpec->IisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uRes, apaOffLft, apaMulLft),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRes, apaOffRht, apaMulRht),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                    // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                    //   use IisFvF
                    if ((aplTypeLft EQ ARRAY_FLOAT && IsSimpleNum (aplTypeRht))   // Res = INT(No Axis), Lft = FLOAT, Rht = BOOL/INT/APA/FLOAT
                     || (aplTypeRht EQ ARRAY_FLOAT && IsSimpleNum (aplTypeLft)))  // Res = INT(No Axis), Lft = BOOL/INT/APA/FLOAT, Rht = FLOAT
                    {
                        DbgBrk ();      // ***TESTME*** -- No such primitive

                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            *((LPAPLINT)   lpMemRes)++ =
                              (*lpPrimSpec->IisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uRes, apaOffLft, apaMulLft),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRes, apaOffRht, apaMulRht),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                        DbgStop ();         // We should never get here
                    break;

                case ARRAY_FLOAT:                   // Res = FLOAT(No Axis)
                    // If both arguments are simple integer (BOOL, INT, APA),
                    //   use FisIvI
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))   // Res = FLOAT(No Axis), Lft = BOOL/INT/APA      , Rht = BOOL/INT/APA
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            *((LPAPLFLOAT) lpMemRes)++ =
                              (*lpPrimSpec->FisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uRes, apaOffLft, apaMulLft),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRes, apaOffRht, apaMulRht),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                    // If both arguments are simple numeric (BOOL, INT, APA, FLOAT),
                    //   use FisFvF
                    if (IsSimpleNum (aplTypeLft)
                     && IsSimpleNum (aplTypeRht))   // Res = FLOAT(No Axis), Lft = BOOL/INT/APA/FLOAT, Rht = BOOL/INT/APA/FLOAT
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            *((LPAPLFLOAT) lpMemRes)++ =
                              (*lpPrimSpec->FisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uRes, apaOffLft, apaMulLft),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRes, apaOffRht, apaMulRht),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                        DbgStop ();         // We should never get here
                    break;

                defstop
                    break;
            } // End SWITCH
            } __except (CheckException (GetExceptionInformation (), lpPrimSpec))
            {
#ifdef DEBUG
                dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
#endif
                // Split cases based upon the ExecCode
                switch (*lpPrimSpec->lpExecCode)
                {
                    case EXEC_DOMAIN_ERROR:
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:
                        *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        return FALSE;

                    case EXEC_RESULT_FLOAT:
                        *lpPrimSpec->lpExecCode = EXEC_SUCCESS; // Reset

                        if (IsSimpleNum (aplTypeRes)
                         && aplTypeRes NE ARRAY_FLOAT)
                        {
                            aplTypeRes = ARRAY_FLOAT;

                            // Restart the pointers
                            lpMemRes = lpMemResStart;
        #ifdef DEBUG
                            dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", *lpPrimSpec->lpExecCode, FNLN);
        #endif
                            goto RESTART_EXCEPTION_NOAXIS;
                        } // End IF

                        // Fall through to never-never-land

                    defstop
                        break;
                } // End SWITCH
            } // End __try/__except
        } // End IF/ELSE

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (*lphGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE/...
ERROR_EXIT:
    if (hGlbOdo && lpMemOdo)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbOdo); lpMemOdo = NULL;
    } // End IF

    if (hGlbWVec && lpMemWVec)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbWVec); lpMemWVec = NULL;
    } // End IF

    if (hGlbWVec)
    {
        // We no longer need this storage
        DbgGlobalFree (hGlbWVec); hGlbWVec = NULL;
    } // End IF

    if (hGlbOdo)
    {
        // We no longer need this storage
        DbgGlobalFree (hGlbOdo); hGlbOdo = NULL;
    } // End IF

    DBGLEAVE;

    return bRet;
} // End PrimFnDydSimpSimp_EM
#undef  APPEND_NAME


//***************************************************************************
//  CalcLftRhtArgIndices
//
//  Calculate the left and right argument indices
//***************************************************************************

void CalcLftRhtArgIndices
    (APLINT    uRes,
     APLRANK   aplRankRes,
     LPAPLINT  lpuLft,
     APLRANK   aplRankLft,
     LPAPLINT  lpuRht,
     APLRANK   aplRankRht,
     APLNELM   aplNELMAxis,
     LPAPLUINT lpMemAxisHead,
     LPAPLUINT lpMemOdo,
     LPAPLUINT lpMemWVec,
     LPAPLDIM  lpMemDimRes)

{
    APLINT uArg, uInd;

    // Loop through the odometer values accumulating
    //   the weighted sum
    // Note that the contents of lpMemOdo change each time this function is called
    for (uArg = 0, uInd = aplRankRes - aplNELMAxis; uInd < (APLRANKSIGN) aplRankRes; uInd++)
        uArg += lpMemOdo[lpMemAxisHead[uInd]] * lpMemWVec[uInd];

    // Increment the odometer in lpMemOdo subject to
    //   the values in lpMemDimRes
    IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

    // Use the just computed index for the argument
    //   with the smaller rank
    if (aplRankLft < aplRankRht)
    {
        *lpuLft = uArg;
        *lpuRht = uRes;
    } else
    {
        *lpuRht = uArg;
        *lpuLft = uRes;
    } // End IF/ELSE
} // End CalcLftRhtArgIndices


//***************************************************************************
//  End of File: primspec.c
//***************************************************************************
