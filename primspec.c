//***************************************************************************
//  NARS2000 -- Primitive Function Special
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2008 Sudley Place Software

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
//  $PrimFnSyntaxError_EM
//
//  Primitive function SYNTAX ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnSyntaxError_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnSyntaxError_EM
    (LPTOKEN lptkFunc)

{
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnSyntaxError_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnValenceError_EM
//
//  Primitive function VALENCE ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnValenceError_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnValenceError_EM
    (LPTOKEN lptkFunc)

{
    ErrorMessageIndirectToken (ERRMSG_VALENCE_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnValenceError_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnNonceError_EM
//
//  Primitive function NONCE ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnNonceError_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnNonceError_EM
    (LPTOKEN lptkFunc)

{
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnNonceError_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDomainError_EM
//
//  Primitive function DOMAIN ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDomainError_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDomainError_EM
    (LPTOKEN lptkFunc)

{
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnDomainError_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnValueError_EM
//
//  Primitive function VALUE ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnValueError_EM"
#else
#define APPEND_NAME
#endif

void PrimFnValueError_EM
    (LPTOKEN lptkFunc)

{
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                               lptkFunc);
} // End PrimFnValueError_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonValenceError_EM
//
//  Primitive scalar monadic function VALENCE ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonValenceError_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonValenceError_EM
    (LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis,           // Ptr to axis token (may be NULL)
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    return PrimFnValenceError_EM (lptkFunc);
} // End PrimFnMonValenceError_EM
#undef  APPEND_NAME


//// //***************************************************************************
//// //  $PrimFnDydSyntaxError_EM
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
//// LPPL_YYSTYPE PrimFnDydSyntaxError_EM
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
//  $PrimProtoFnMixed_EM_YY
//
//  Generate a prototype result for the monadic & dyadic primitive mixed functions
//***************************************************************************

LPPL_YYSTYPE PrimProtoFnMixed_EM_YY
    (LPPRIMFNS  lpPrimFn,           // Ptr to primitive function routine
     LPTOKEN    lptkLftArg,         // Ptr to left arg token
     LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis)           // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result
    HGLOBAL      hGlbRes,           // Result global memory handle
                 hGlbProto;         // Prototype ...

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
            if (IsImmChr (lpYYRes->tkToken.tkFlags.ImmType))
                lpYYRes->tkToken.tkData.tkChar = L' ';
            else
            {
                lpYYRes->tkToken.tkFlags.ImmType = IMMTYPE_BOOL;
                lpYYRes->tkToken.tkData.tkBoolean = 0;
            } // End IF/ELSE

            break;

        case TKT_VARARRAY:
            hGlbRes = ClrPtrTypeDirAsGlb (lpYYRes->tkToken.tkData.tkGlbData);

            // Make the prototype
            hGlbProto =
              MakeMonPrototype_EM (hGlbRes,     // Proto arg handle
                                   lptkFunc,    // Ptr to function token
                                   MP_CHARS);   // CHARs allowed
            if (!hGlbProto)
            {
                YYFree (lpYYRes); lpYYRes = NULL;
            } else
                // Save back into the result
                lpYYRes->tkToken.tkData.tkGlbData =
                  MakePtrTypeGlb (hGlbProto);
            // We no longer need this storage
            FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;

            break;

        defstop
            break;
    } // End IF/SWITCH

    return lpYYRes;
} // End PrimProtoFnMixed_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnScalar_EM_YY
//
//  Generate a prototype result for the monadic & dyadic primitive scalar functions
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnScalar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnScalar_EM_YY
    (LPTOKEN    lptkLftArg,         // Ptr to left arg token
     LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis)           // Ptr to axis token (may be NULL)

{
    HGLOBAL      hGlbLft,           // Left arg global memory handle
                 hGlbRht,           // Right ...
                 hGlbRes;           // Result   ...
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result

    // Get right arg's global memory handle
    hGlbRht = GetGlbHandle (lptkRhtArg);

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
    {
        //***************************************************************
        // Scalar monadic primitive functions are not sensitive
        //   to the axis operator, so signal a syntax error if present
        //***************************************************************

        if (lptkAxis NE NULL)
        {
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkAxis);
            return NULL;
        } // End IF

        //***************************************************************
        // Called monadically
        //***************************************************************

        // Make the prototype
        hGlbRes = MakeMonPrototype_EM (hGlbRht,     // Proto arg handle
                                       lptkFunc,    // Ptr to function token
                                       MP_NUMONLY); // Numerics only
    } else
    {
        //***************************************************************
        // Called dyadically
        //***************************************************************

        // Get left arg's global memory handle
        hGlbLft = GetGlbHandle (lptkLftArg);

        // Check for both args immediate
        if (hGlbLft EQ NULL
         && hGlbRht EQ NULL)
        {
            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_BOOL;
////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkInteger  = 0;
            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

            goto NORMAL_EXIT;
        } // End IF

        // Handle as dyadic prototype
        hGlbRes = MakeDydPrototype_EM (hGlbLft,                     // Left arg global memory handle (may be NULL if immediate)
                                       lptkLftArg->tkFlags.ImmType, // Left arg immediate type
                                       lptkFunc,                    // Ptr to function token
                                       hGlbRht,                     // Right arg global memory handle
                                       lptkRhtArg->tkFlags.ImmType, // Left arg immediate type
                                       lptkAxis);                   // Ptr to axis value token
    } // End IF

    if (!hGlbRes)
        goto ERROR_EXIT;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return lpYYRes;
} // End PrimProtoFnScalar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMon_EM_YY
//
//  Primitive scalar monadic function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMon_EM_YY
    (LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis,           // Ptr to axis token (may be NULL)
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    HGLOBAL      hGlbRes;
    APLSTYPE     aplTypeRes,
                 aplTypeRht;
    APLRANK      aplRankRht;
    APLNELM      aplNELMRht;
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    DBGENTER;

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Handle prototypes separately
    if (aplNELMRht EQ 0)
        return PrimProtoFnScalar_EM_YY (NULL,       // Ptr to left arg token
                                        lptkFunc,   // Ptr to function token
                                        lptkRhtArg, // Ptr to right arg token
                                        lptkAxis);  // Ptr to axis token (may be NULL)
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
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must trundle through the array
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // Get the global memory handle
                hGlbRes = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbRes));

                // Handle via subroutine
                hGlbRes = PrimFnMonGlb_EM (ClrPtrTypeDirAsGlb (hGlbRes),
                                           lptkFunc,
                                           lpPrimSpec);
                if (!hGlbRes)
                {
                    YYFree (lpYYRes); lpYYRes = NULL;

                    return NULL;
                } // End IF

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
                lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

                DBGLEAVE;

                return lpYYRes;
            } // End IF

            // Handle the immediate case

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
            } __except (CheckException (GetExceptionInformation (), "PrimFnMon_EM_YY #1"))
            {
#ifdef DEBUG
                dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
                DisplayException ();

                // Split cases based upon the ExceptionCode
                switch (MyGetExceptionCode ())
                {
                    case EXCEPTION_DOMAIN_ERROR:
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        YYFree (lpYYRes); lpYYRes = NULL;

                        return NULL;

                    case EXCEPTION_RESULT_FLOAT:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsSimpleNum (aplTypeRes)
                         && !IsSimpleFlt (aplTypeRes))
                        {
                            aplTypeRes = ARRAY_FLOAT;
#ifdef DEBUG
                            dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
                            goto RESTART_EXCEPTION_VARNAMED;
                        } // End IF

                        // Fall through to never-never-land

                    default:
                        // Display message for unhandled exception
                        DisplayException ();

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
            } __except (CheckException (GetExceptionInformation (), "PrimFnMon_EM_YY #2"))
            {
#ifdef DEBUG
                dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
                DisplayException ();

                // Split cases based upon the ExceptionCode
                switch (MyGetExceptionCode ())
                {
                    case EXCEPTION_DOMAIN_ERROR:
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        YYFree (lpYYRes); lpYYRes = NULL;

                        return NULL;

                    case EXCEPTION_RESULT_FLOAT:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsSimpleNum (aplTypeRes)
                         && !IsSimpleFlt (aplTypeRes))
                        {
                            aplTypeRes = ARRAY_FLOAT;
#ifdef DEBUG
                            dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
                            goto RESTART_EXCEPTION_VARIMMED;
                        } // End IF

                        // Fall through to never-never-land

                    default:
                        // Display message for unhandled exception
                        DisplayException ();

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
            // Get the global memory handle
            hGlbRes = lptkRhtArg->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbRes));

            // Handle via subroutine
            hGlbRes = PrimFnMonGlb_EM (ClrPtrTypeDirAsGlb (hGlbRes),
                                       lptkFunc,
                                       lpPrimSpec);
            if (!hGlbRes)
            {
                YYFree (lpYYRes); lpYYRes = NULL;

                return NULL;
            } // End IF

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

            DBGLEAVE;

            return lpYYRes;

        defstop
            break;
    } // End SWITCH

    DbgStop ();         // We should never get here

    YYFree (lpYYRes); lpYYRes = NULL;

    return NULL;
} // End PrimFnMon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonGlb_EM
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
    LPVOID   lpMemRht = NULL,   // Ptr to right arg global memory
             lpMemRes = NULL;   // Ptr to result    ...
    HGLOBAL  hGlbRes = NULL,    // Result global memory handle
             hGlbSub;           // Subarray ...
    APLSTYPE aplTypeRht,        // Right arg storage type
             aplTypeRes;        // Result    ...
    APLNELM  aplNELMRht;        // # elements in the array
    APLRANK  aplRankRht;        // The rank of the array
    APLINT   uRes,              // Result loop counter
             apaOffRht,         // Right arg APA offset
             apaMulRht;         // ...           multiplier
    APLUINT  ByteRes;           // # bytes in the result
    BOOL     bRet = TRUE;       // TRUE iff result is valid
    UINT     uBitIndex;         // Bit index when marching through Booleans

    DBGENTER;

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)
    // Get the Array Type, NELM, and Rank
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
    if (IsSimpleAPA (aplTypeRes))
    {
        if (!(*lpPrimSpec->ApaResultMon_EM) (NULL,
                                             lptkFunc,
                                             hGlbRht,
                                            &hGlbRes,
                                             aplRankRht,
                                             lpPrimSpec))
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } // End IF
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
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
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

    // lpMemRes now points to the result's data
    // lpMemRht now points to the right arg's data

    // If the right arg is an APA, ...
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
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
                    aplTypeRht2 = TranslateImmTypeToArrayType ((ClrPtrTypeIndAsSym (lpMemRht))->stFlags.ImmType);

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
                    lpSymSrc = ClrPtrTypeIndAsSym (lpMemRht);
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
                        *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeSym (lpSymDst);
                    } else
                        bRet = FALSE;
                    break;
                } // End PTRTYPE_STCONST

                case PTRTYPE_HGLOBAL:
                    // Handle via subroutine
                    hGlbSub = PrimFnMonGlb_EM (ClrPtrTypeIndAsGlb (lpMemRht),
                                               lptkFunc,
                                               lpPrimSpec);
                    if (hGlbSub)
                        *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeGlb (hGlbSub);
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
    } __except (CheckException (GetExceptionInformation (), "PrimFnMonGlb_EM"))
    {
#ifdef DEBUG
        dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
        DisplayException ();

        // Split cases based upon the ExceptionCode
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_DOMAIN_ERROR:
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                goto ERROR_EXIT;

            case EXCEPTION_RESULT_FLOAT:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                if (IsSimpleNum (aplTypeRes)
                 && !IsSimpleFlt (aplTypeRes))
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
                    dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
                    goto RESTART_EXCEPTION;
                } // End IF

                // Fall through to never-never-land

            default:
                // Display message for unhandled exception
                DisplayException ();

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
//  $PrimFnDyd_EM_YY
//
//  Primitive scalar function for dyadic fns
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDyd_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDyd_EM_YY
    (LPTOKEN    lptkLftArg,         // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN    lptkFunc,           // Ptr to function token
     LPTOKEN    lptkRhtArg,         // Ptr to right arg token
     LPTOKEN    lptkAxis,           // Ptr to axis token (may be NULL)
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLRANK      aplRankLft,            // Left arg rank
                 aplRankRht,            // Right ...
                 aplRankRes;            // Result   ...
    APLNELM      aplNELMLft,            // Left arg NELM
                 aplNELMRht,            // Right ...
                 aplNELMRes,            // Result   ...
                 aplNELMAxis;           // Axis     ...
    APLLONGEST   aplLongestLft,         // Left arg longest value
                 aplLongestRht;         // Right ...
    HGLOBAL      hGlbLft = NULL,        // Left arg global memory handle
                 hGlbRht = NULL,        // Right ...
                 hGlbRes = NULL,        // Result   ...
                 hGlbAxis = NULL;       // Axis     ...
    APLSTYPE     aplTypeLft,            // Left arg storage type
                 aplTypeRht,            // Right ...
                 aplTypeRes;            // Result   ...
    LPAPLUINT    lpMemAxisHead = NULL,  // Ptr to axis head
                 lpMemAxisTail = NULL;  // Ptr to axis tail
    LPVOID       lpMemLft = NULL,       // Ptr to left arg global memory
                 lpMemRht = NULL;       // Ptr to right ...
    APLINT       aplInteger;            // Temporary integer value
    BOOL         bRet = TRUE;           // TRUE iff result is valid
    LPPRIMFN_DYD_SNvSN lpPrimFn;        // Ptr to dyadic scalar SimpNest vs. SimpNest function
    LPPL_YYSTYPE lpYYRes = NULL;        // Ptr to the result

    DBGENTER;

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Handle prototypes separately
    if (aplNELMLft EQ 0
     || aplNELMRht EQ 0)
    {
        lpYYRes = PrimProtoFnScalar_EM_YY (lptkLftArg,  // Ptr to left arg token
                                           lptkFunc,    // Ptr to function token
                                           lptkRhtArg,  // Ptr to right arg token
                                           lptkAxis);   // Ptr to axis token (may be NULL)
        bRet = (lpYYRes NE NULL);

        goto NORMAL_EXIT;
    } // End IF

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
         || IsNested (aplTypeRes));

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

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
    if (IsSimpleAPA (aplTypeRes))
    {
        if (IsSimpleAPA (aplTypeLft))
            aplInteger = aplLongestRht;
        else
            aplInteger = aplLongestLft;
        if (!(*lpPrimSpec->ApaResultDyd_EM) (lpYYRes,
                                             lptkFunc,
                                             hGlbLft,
                                             hGlbRht,
                                            &hGlbRes,
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
    if (IsNested (aplTypeLft)
     && IsNested (aplTypeRht))
        // Left arg is NESTED, right arg is NESTED
        lpPrimFn = &PrimFnDydNestNest_EM;
    else
    if ((!IsNested (aplTypeLft))
     && IsNested (aplTypeRht))
        // Left arg is SIMPLE, right arg is NESTED
        lpPrimFn = &PrimFnDydSimpNest_EM;
    else
    if (IsNested (aplTypeLft)
     && !IsNested (aplTypeRht))
        // Left arg is NESTED, right arg is SIMPLE
        lpPrimFn = &PrimFnDydNestSimp_EM;
    else
    if ((!IsNested (aplTypeLft))
     && (!IsNested (aplTypeRht)))
        // Left arg is SIMPLE, right arg is SIMPLE
        lpPrimFn = &PrimFnDydSimpSimp_EM;
    else
    {
        DbgStop ();     // We should never get here

        return NULL;
    } // Endf IF/ELSE/...

    // Call the appropriate function
    if (!(*lpPrimFn) (lpYYRes,
                      lptkLftArg,
                      lptkFunc,
                      lptkRhtArg,
                      hGlbLft,
                      hGlbRht,
                     &hGlbRes,
                      lpMemLft,         // Points to Sig.nature
                      lpMemRht,         // ...
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
} // End PrimFnDyd_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSimpNest_EM
//
//  Dyadic primitive scalar function, left simple, right nested
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSimpNest_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydSimpNest_EM
    (LPPL_YYSTYPE  lpYYRes,         // Ptr to the result

     LPTOKEN       lptkLftArg,      // Ptr to left arg token
     LPTOKEN       lptkFunc,        // ...    function ...
     LPTOKEN       lptkRhtArg,      // ...    right arg ...

     HGLOBAL       hGlbLft,         // Left arg handle
     HGLOBAL       hGlbRht,         // Right ...
     HGLOBAL      *lphGlbRes,       // Ptr to result handle

     LPVOID        lpMemLft,        // Points to Sig.nature
     LPVOID        lpMemRht,        // ...

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
    LPVOID     lpMemRes = NULL;     // Ptr to result global memory
    BOOL       bRet = TRUE;         // TRUE iff result is valid
    APLINT     uRes;                // Loop counter
    APLINT     aplIntegerLft,
               aplIntegerRht,
               apaOffLft,
               apaMulLft,
               iDim;
    APLUINT    ByteAlloc;
    APLFLOAT   aplFloatLft,
               aplFloatRht;
    HGLOBAL    hGlbWVec = NULL,
               hGlbOdo = NULL,
               hGlbSub;
    LPAPLUINT  lpMemWVec = NULL,
               lpMemDimLft,
               lpMemDimRht,
               lpMemDimRes,
               lpMemOdo = NULL;
    APLCHAR    aplCharLft,
               aplCharRht;

    DBGENTER;

    // If the left arg is immediate, get the one and only value
    if (!lpMemLft)
        GetFirstValueToken (lptkLftArg,     // Ptr to left arg token
                           &aplIntegerLft,  // Ptr to integer result
                           &aplFloatLft,    // Ptr to float ...
                           &aplCharLft,     // Ptr to WCHAR ...
                            NULL,           // Ptr to longest ...
                            NULL,           // Ptr to lpSym/Glb ...
                            NULL,           // Ptr to ...immediate type ...
                            NULL);          // Ptr to array type ...
    else
    {
        // Skip over the header to the dimensions
        lpMemDimLft = VarArrayBaseToDim (lpMemLft);

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
    } // End IF/ELSE

    // If the left arg is APA, ...
    if (IsSimpleAPA (aplTypeLft))
    {
#define lpAPA       ((LPAPLAPA) lpMemLft)
        // Get the APA parameters
        apaOffLft = lpAPA->Off;
        apaMulLft = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (*lphGlbRes);

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);
    lpMemDimRes = VarArrayBaseToDim (lpMemRes);

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    if (IsNested (aplTypeRes))
    {
        // Fill nested result with PTR_REUSED
        //   in case we fail part way through
        *((LPAPLNESTED) lpMemRes) = PTR_REUSED;
        for (uRes = 1; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
            ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;
    } // End IF

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
        ByteAlloc = aplRankRes * sizeof (APLUINT);
        Assert (ByteAlloc EQ (UINT) ByteAlloc);
        hGlbOdo = DbgGlobalAlloc (GHND, (UINT) ByteAlloc);
        if (!hGlbOdo)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemOdo = MyGlobalLock (hGlbOdo);
    } // End IF

    // Loop through the result
    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
    {
        APLINT   uLft, uRht, uArg;
        APLSTYPE aplTypeHetLft,
                 aplTypeHetRht;

        // Copy in case we are heterogeneous
        aplTypeHetLft = aplTypeLft;

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
                    aplIntegerLft = GetNextInteger (lpMemLft, aplTypeLft, uLft);
                    aplFloatLft   = (APLFLOAT) aplIntegerLft;   // In case of type promotion

                    break;

                case ARRAY_FLOAT:
                    aplFloatLft   = GetNextFloat   (lpMemLft, aplTypeLft, uLft);

                    break;

                case ARRAY_CHAR:
                    aplCharLft    = ((LPAPLCHAR) lpMemLft)[uLft];

                    break;

                case ARRAY_HETERO:
                    aplTypeHetLft = GetNextHetero (lpMemLft, uLft, &aplIntegerLft, &aplFloatLft, &aplCharLft);

                    break;

                defstop
                    break;
            } // End SWITCH
        } else
            uRht = uRes;

        // Get the right arg element
        hGlbSub = ((LPAPLNESTED) lpMemRht)[uRht];

        // Split cases based upon the ptr type of the nested right arg
        switch (GetPtrTypeDir (hGlbSub))
        {
            case PTRTYPE_STCONST:
                GetFirstValueImm (((LPSYMENTRY) hGlbSub)->stFlags.ImmType,
                                  ((LPSYMENTRY) hGlbSub)->stData.stLongest,
                                 &aplIntegerRht,
                                 &aplFloatRht,
                                 &aplCharRht,
                                  NULL,
                                  NULL,
                                  NULL,
                                 &aplTypeHetRht);
                hGlbSub = PrimFnDydSiScSiSc_EM (lptkFunc,
                                                aplTypeHetLft,
                                                aplIntegerLft,
                                                aplFloatLft,
                                                aplCharLft,
                                                aplTypeHetRht,
                                                aplIntegerRht,
                                                aplFloatRht,
                                                aplCharRht,
                                                lpPrimSpec);
                if (!hGlbSub)
                    goto ERROR_EXIT;
                else
                    *((LPAPLNESTED) lpMemRes)++ = hGlbSub;
                break;

            case PTRTYPE_HGLOBAL:
                hGlbSub = PrimFnDydSiScNest_EM (lptkFunc,
                                                aplTypeHetLft,
                                                aplIntegerLft,
                                                aplFloatLft,
                                                aplCharLft,
                                                hGlbSub,
                                                lpPrimSpec);
                if (!hGlbSub)
                    goto ERROR_EXIT;
                else
                    *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeGlb (hGlbSub);
                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

ERROR_EXIT:
    bRet = FALSE;
NORMAL_EXIT:
    if (*lphGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;
    } // End IF

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
//  $PrimFnDydNestSimp_EM
//
//  Dyadic primitive scalar function, left nested, right simple
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydNestSimp_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydNestSimp_EM
    (LPPL_YYSTYPE  lpYYRes,         // Ptr to the result

     LPTOKEN       lptkLftArg,      // Ptr to left arg token
     LPTOKEN       lptkFunc,        // ...    function ...
     LPTOKEN       lptkRhtArg,      // ...    right arg ...

     HGLOBAL       hGlbLft,         // Left arg handle
     HGLOBAL       hGlbRht,         // Right ...
     HGLOBAL      *lphGlbRes,       // Ptr to result handle

     LPVOID        lpMemLft,        // Points to Sig.nature
     LPVOID        lpMemRht,        // ...

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
    LPVOID     lpMemRes = NULL;     // Ptr to result global memory
    BOOL       bRet = TRUE;         // TRUE iff result is valid
    APLINT     uRes;                // Loop counter
    APLINT     aplIntegerLft,
               aplIntegerRht,
               apaOffRht,
               apaMulRht,
               ByteAlloc,
               iDim;
    APLFLOAT   aplFloatLft,
               aplFloatRht;
    HGLOBAL    hGlbWVec = NULL,
               hGlbOdo = NULL,
               hGlbSub;
    LPAPLUINT  lpMemWVec = NULL,
               lpMemDimLft,
               lpMemDimRht,
               lpMemDimRes,
               lpMemOdo = NULL;
    APLCHAR    aplCharLft,
               aplCharRht;

    DBGENTER;

    // If the right arg is immediate, get the one and only value
    if (!lpMemRht)
       GetFirstValueToken (lptkRhtArg,      // Ptr to right arg token
                          &aplIntegerRht,   // Ptr to integer result
                          &aplFloatRht,     // Ptr to float ...
                          &aplCharRht,      // Ptr to WCHAR ...
                           NULL,            // Ptr to longest ...
                           NULL,            // Ptr to lpSym/Glb ...
                           NULL,            // Ptr to ...immediate type ...
                           NULL);           // Ptr to array type ...
    else
    {
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
    } // End IF/ELSE

    // If the right arg is APA, ...
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (*lphGlbRes);

    // Skip over the header to the dimensions
    lpMemDimLft = VarArrayBaseToDim (lpMemLft);
    lpMemDimRes = VarArrayBaseToDim (lpMemRes);

    // Skip over the header and dimensions to the data
    lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    if (IsNested (aplTypeRes))
    {
        // Fill nested result with PTR_REUSED
        //   in case we fail part way through
        *((LPAPLNESTED) lpMemRes) = PTR_REUSED;
        for (uRes = 1; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
            ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;
    } // End IF

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
        ByteAlloc = aplRankRes * sizeof (APLUINT);
        Assert (ByteAlloc EQ (UINT) ByteAlloc);
        hGlbOdo = DbgGlobalAlloc (GHND, (UINT) ByteAlloc);
        if (!hGlbOdo)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemOdo = MyGlobalLock (hGlbOdo);
    } // End IF

    // Loop through the result
    for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
    {
        APLINT   uLft, uRht, uArg;
        APLSTYPE aplTypeHetLft,
                 aplTypeHetRht;

        // Copy in case we are heterogeneous
        aplTypeHetRht = aplTypeRht;

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
                    aplIntegerRht = GetNextInteger (lpMemRht, aplTypeRht, uRht);
                    aplFloatRht   = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                    break;

                case ARRAY_FLOAT:
                    aplFloatRht   = GetNextFloat   (lpMemRht, aplTypeRht, uRht);

                    break;

                case ARRAY_CHAR:
                    aplCharRht    = ((LPAPLCHAR) lpMemRht)[uRht];

                    break;

                case ARRAY_HETERO:
                    aplTypeHetRht = GetNextHetero (lpMemRht, uRht, &aplIntegerRht, &aplFloatRht, &aplCharRht);

                    break;

                defstop
                    break;
            } // End SWITCH
        } else
            uLft = uRes;

        // Get the left arg element
        hGlbSub = ((LPAPLNESTED) lpMemLft)[uLft];

        // Split cases based upon the ptr type of the nested left arg
        switch (GetPtrTypeDir (hGlbSub))
        {
            case PTRTYPE_STCONST:
                GetFirstValueImm (((LPSYMENTRY) hGlbSub)->stFlags.ImmType,
                                  ((LPSYMENTRY) hGlbSub)->stData.stLongest,
                                 &aplIntegerLft,
                                 &aplFloatLft,
                                 &aplCharLft,
                                  NULL,
                                  NULL,
                                  NULL,
                                 &aplTypeHetLft);
                hGlbSub = PrimFnDydSiScSiSc_EM (lptkFunc,
                                                aplTypeHetLft,
                                                aplIntegerLft,
                                                aplFloatLft,
                                                aplCharLft,
                                                aplTypeHetRht,
                                                aplIntegerRht,
                                                aplFloatRht,
                                                aplCharRht,
                                                lpPrimSpec);
                if (!hGlbSub)
                    goto ERROR_EXIT;
                else
                    *((LPAPLNESTED) lpMemRes)++ = hGlbSub;
                break;

            case PTRTYPE_HGLOBAL:
                hGlbSub = PrimFnDydNestSiSc_EM (lptkFunc,
                                                aplTypeHetRht,
                                                aplIntegerRht,
                                                aplFloatRht,
                                                aplCharRht,
                                                hGlbSub,
                                                lpPrimSpec);
                if (!hGlbSub)
                    goto ERROR_EXIT;
                else
                    *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeGlb (hGlbSub);
                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

ERROR_EXIT:
    bRet = FALSE;
NORMAL_EXIT:
    if (*lphGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;
    } // End IF

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
//  $PrimFnDydNestSiSc_EM
//
//  Subroutine to PrimFnDydNestSimp_EM to handle right arg simple scalars
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydNestSiSc_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL PrimFnDydNestSiSc_EM
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
    APLINT   aplIntegerLft,
             uLft,
             apaOffLft,
             apaMulLft;
    APLFLOAT aplFloatLft;
    APLCHAR  aplCharLft;
    UINT     uBitIndex = 0;

    DBGENTER;

    // The left arg data is a valid HGLOBAL array
    Assert (IsGlbTypeVarDir (aplNestedLft));

    // Clear the identifying bits
    hGlbLft = ClrPtrTypeDirAsGlb (aplNestedLft);

    // Lock the memory to get a ptr to it
    lpMemLft = MyGlobalLock (hGlbLft);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemLft)
    // Get the Array Type, NELM, and Rank
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
    if (IsSimpleAPA (aplTypeRes))
    {
        if (!(*lpPrimSpec->ApaResultDyd_EM) (NULL,
                                             lptkFunc,
                                             hGlbLft,
                                             NULL,
                                            &hGlbRes,
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
         || IsNested (aplTypeRes));

    // Allocate space for result
    if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                    &hGlbRes,
                                     NULL,
                                     lpMemLft,
                                     aplRankRht,
                                     aplRankLft,
                                    &aplRankRes,
                                     aplTypeRes,
                                     aplNELMRht,
                                     aplNELMLft,
                                     aplNELMRes))
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

    // Skip over the header and dimensions to the data
    lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    // If the left arg is an APA, ...
    if (IsSimpleAPA (aplTypeLft))
    {
#define lpAPA       ((LPAPLAPA) lpMemLft)
        // Get the APA parameters
        apaOffLft = lpAPA->Off;
        apaMulLft = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // If simple result, ...
    if (IsSimpleNum (aplTypeRes))
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

    else
    // If nested result, ...
    if (IsNested (aplTypeRes))
    {
        // Loop through the left arg/result
        for (uLft = 0; uLft < (APLNELMSIGN) aplNELMLft; uLft++)
        {
            APLSTYPE aplTypeHetLft;

            // Copy in case we are heterogeneous
            aplTypeHetLft = aplTypeLft;

            // Get the left arg element
            hGlbSub = ((LPAPLNESTED) lpMemLft)[uLft];

            // Split cases based upon the ptr type of the nested left arg
            switch (GetPtrTypeDir (hGlbSub))
            {
                case PTRTYPE_STCONST:
                    GetFirstValueImm (((LPSYMENTRY) hGlbSub)->stFlags.ImmType,
                                      ((LPSYMENTRY) hGlbSub)->stData.stLongest,
                                     &aplIntegerLft,
                                     &aplFloatLft,
                                     &aplCharLft,
                                      NULL,
                                      NULL,
                                      NULL,
                                     &aplTypeHetLft);
                    hGlbSub = PrimFnDydSiScSiSc_EM (lptkFunc,
                                                    aplTypeHetLft,
                                                    aplIntegerLft,
                                                    aplFloatLft,
                                                    aplCharLft,
                                                    aplTypeRht,
                                                    aplIntegerRht,
                                                    aplFloatRht,
                                                    aplCharRht,
                                                    lpPrimSpec);
                    if (!hGlbSub)
                        goto ERROR_EXIT;
                    else
                        *((LPAPLNESTED) lpMemRes)++ = hGlbSub;
                    break;

                case PTRTYPE_HGLOBAL:
                    hGlbSub = PrimFnDydNestSiSc_EM (lptkFunc,
                                                    aplTypeRht,
                                                    aplIntegerRht,
                                                    aplFloatRht,
                                                    aplCharRht,
                                                    hGlbSub,
                                                    lpPrimSpec);
                    if (!hGlbSub)
                        goto ERROR_EXIT;
                    else
                        *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeGlb (hGlbSub);
                    break;

                defstop
                    break;
            } // End SWITCH
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
} // End PrimFnDydNestSiSc_EM
#undef  APPEND_NAME


//***************************************************************************
//  $FillToken
//
//  Fill in a token
//***************************************************************************

void FillToken
    (LPTOKEN lptkArg,           // Ptr to arg token
     LPVOID  lpMem,             // Ptr to either HGLOBAL or LPSYMENTRY
     UINT    tkCharIndex)       // Character index in case of error

{
    // Split cases based upon the ptr type
    switch (GetPtrTypeDir (lpMem))
    {
        case PTRTYPE_STCONST:
            lptkArg->tkFlags.TknType   = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType   = ((LPSYMENTRY) lpMem)->stFlags.ImmType;
////////////lptkArg->tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
            lptkArg->tkData.tkLongest  = ((LPSYMENTRY) lpMem)->stData.stLongest;
            lptkArg->tkCharIndex       = tkCharIndex;

            break;

        case PTRTYPE_HGLOBAL:
            lptkArg->tkFlags.TknType   = TKT_VARARRAY;
////////////lptkArg->tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////////lptkArg->tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
            lptkArg->tkData.tkGlbData  = lpMem;
            lptkArg->tkCharIndex       = tkCharIndex;

            break;

        defstop
            break;
    } // End SWITCH
} // End FillToken


//***************************************************************************
//  $PrimFnDydNestNest_EM
//
//  Dyadic primitive scalar function, left nested, right nested
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydNestNest_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydNestNest_EM
    (LPPL_YYSTYPE lpYYRes,          // Ptr to the result

     LPTOKEN      lptkLftArg,       // Ptr to left arg token
     LPTOKEN      lptkFunc,         // ...    function ...
     LPTOKEN      lptkRhtArg,       // ...    right arg ...

     HGLOBAL      hGlbLft,          // Left arg handle
     HGLOBAL      hGlbRht,          // Right ...
     HGLOBAL     *lphGlbRes,        // Ptr to result handle

     LPVOID       lpMemLft,         // Points to Sig.nature
     LPVOID       lpMemRht,         // ...

     LPAPLUINT    lpMemAxisHead,    // Ptr to axis values, fleshed out
     LPAPLUINT    lpMemAxisTail,    // Ptr to grade up of AxisHead

     APLRANK      aplRankLft,       // Left arg rank
     APLRANK      aplRankRht,       // Right ...
     APLRANK      aplRankRes,       // Result ...

     APLSTYPE     aplTypeLft,       // Left arg type
     APLSTYPE     aplTypeRht,       // Right ...
     APLSTYPE     aplTypeRes,       // Result ...

     APLNELM      aplNELMLft,       // Left arg NELM
     APLNELM      aplNELMRht,       // Right ...
     APLNELM      aplNELMRes,       // Result ...
     APLNELM      aplNELMAxis,      // Axis ...
     LPPRIMSPEC   lpPrimSpec)       // Ptr to local PRIMSPEC

{
    BOOL   bRet = TRUE;             // TRUE iff result is valid
    APLINT uRes;                    // Loop counter
    LPVOID lpMemRes = NULL;         // Ptr to result global memory

    DBGENTER;

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

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (*lphGlbRes);

    // Skip over the headers and dimensions to the data
    lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    // Loop through the left and right args
    for (uRes = 0; bRet && uRes < (APLNELMSIGN) aplNELMRes; uRes++)
    {
        TOKEN     tkLft = {0},
                  tkRht = {0};
        LPPL_YYSTYPE lpYYRes;           // Ptr to the result

        // Fill in the left arg token
        FillToken (&tkLft,
                    ((LPAPLNESTED) lpMemLft)[uRes % aplNELMLft],
                    lptkLftArg->tkCharIndex);
        // Fill in the right arg token
        FillToken (&tkRht,
                    ((LPAPLNESTED) lpMemRht)[uRes % aplNELMRht],
                    lptkRhtArg->tkCharIndex);
        // Call as dyadic function
        lpYYRes = (*lpPrimSpec->PrimFnDyd_EM_YY) (&tkLft,       // Ptr to left arg token
                                                   lptkFunc,    // Ptr to function token
                                                  &tkRht,       // Ptr to right arg token
                                                   NULL,        // Ptr to axis token
                                                   lpPrimSpec); // Ptr to local PRIMSPEC
        if (lpYYRes)
        {
            ((LPAPLNESTED) lpMemRes)[uRes] = lpYYRes->tkToken.tkData.tkGlbData;
            YYFree (lpYYRes); lpYYRes = NULL;
        } else
            bRet = FALSE;
    } // End FOR

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;

    if (bRet)
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF
ERROR_EXIT:
    DBGLEAVE;

    return bRet;
} // End PrimFnDydNestNest_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSingMult_EM
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
    APLSTYPE aplTypeHetRht;
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
                if (IsSimpleChar (aplTypeLft)
                 && IsSimpleChar (aplTypeRht))
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
                if ((IsSimpleNum (aplTypeLft) && IsSimpleChar (aplTypeRht))
                 || (IsSimpleNum (aplTypeRht) && IsSimpleChar (aplTypeLft)))
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
                if (IsSimpleHet (aplTypeRht))
                {
                    // Split cases based upon the left arg's storage type
                    switch (aplTypeLft)
                    {
                        case ARRAY_BOOL:    // Res = BOOL, Lft = BOOL (S)
                        case ARRAY_INT:     // Res = BOOL, Lft = INT  (S)
                            // Loop through the right arg
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                aplTypeHetRht = GetNextHetero (lpMemRht, uRes, &aplIntegerRht, &aplFloatRht, &aplCharRht);

                                // Split cases based upon the right arg's item's storage type
                                switch (aplTypeHetRht)
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
                                aplTypeHetRht = GetNextHetero (lpMemRht, uRes, &aplIntegerRht, &aplFloatRht, &aplCharRht);

                                // Split cases based upon the right arg's item's storage type
                                switch (aplTypeHetRht)
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
                                aplTypeHetRht = GetNextHetero (lpMemRht, uRes, &aplIntegerRht, &aplFloatRht, &aplCharRht);

                                // Split cases based upon the right arg's item's storage type
                                switch (aplTypeHetRht)
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
                    if (IsSimpleBool (aplTypeRht))
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
    } __except (CheckException (GetExceptionInformation (), "PrimFnDydSingMult_EM"))
    {
#ifdef DEBUG
        dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
        DisplayException ();

        // Split cases based upon the ExceptionCode
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_DOMAIN_ERROR:
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                return FALSE;

            case EXCEPTION_RESULT_FLOAT:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                if (IsSimpleNum (aplTypeRes)
                 && !IsSimpleFlt (aplTypeRes))
                {
                    aplTypeRes = ARRAY_FLOAT;

                    // Restart the pointers
                    lpMemRes = lpMemResStart;
                    lpMemRht = lpMemRhtStart;
#ifdef DEBUG
                    dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
                    goto RESTART_EXCEPTION;
                } // End IF

                // Fall through to never-never-land

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except

    return TRUE;
} // End PrimFnDydSingMult_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydMultSing_EM
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
    APLSTYPE aplTypeHetLft;
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
                if (IsSimpleChar (aplTypeLft)
                 && IsSimpleChar (aplTypeRht))
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
                if ((IsSimpleNum (aplTypeLft) && IsSimpleChar (aplTypeRht))
                 || (IsSimpleNum (aplTypeRht) && IsSimpleChar (aplTypeLft)))
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
                if (IsSimpleHet (aplTypeLft))
                {
                    // Split cases based upon the right arg's storage type
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:    // Res = BOOL,                  Rht = BOOL (S)
                        case ARRAY_INT:     // Res = BOOL,                  Rht = INT  (S)
                            // Loop through the left argument
                            for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                            {
                                aplTypeHetLft = GetNextHetero (lpMemLft, uRes, &aplIntegerLft, &aplFloatLft, &aplCharLft);

                                // Split cases based upon the left arg's item's storage type
                                switch (aplTypeHetLft)
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
                                aplTypeHetLft = GetNextHetero (lpMemLft, uRes, &aplIntegerLft, &aplFloatLft, &aplCharLft);

                                // Split cases based upon the left arg's item's storage type
                                switch (aplTypeHetLft)
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
                                aplTypeHetLft = GetNextHetero (lpMemLft, uRes, &aplIntegerLft, &aplFloatLft, &aplCharLft);

                                // Split cases based upon the left arg's item's storage type
                                switch (aplTypeHetLft)
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
                    if (IsSimpleBool (aplTypeLft))
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
    } __except (CheckException (GetExceptionInformation (), "PrimFnDydMultSing_EM"))
    {
#ifdef DEBUG
        dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
        DisplayException ();

        // Split cases based upon the ExceptionCode
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_DOMAIN_ERROR:
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                return FALSE;

            case EXCEPTION_RESULT_FLOAT:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                if (IsSimpleNum (aplTypeRes)
                 && !IsSimpleFlt (aplTypeRes))
                {
                    aplTypeRes = ARRAY_FLOAT;

                    // Restart the pointers
                    lpMemRes = lpMemResStart;
                    lpMemLft = lpMemLftStart;
#ifdef DEBUG
                    dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
                    goto RESTART_EXCEPTION;
                } // End IF

                // Fall through to never-never-land

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except

    return TRUE;
} // End PrimFnDydMultSing_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSiScNest_EM
//
//  Subroutine to PrimFnDydSimpNest_EM to handle left arg simple scalars
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSiScNest_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL PrimFnDydSiScNest_EM
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
    APLINT   aplIntegerRht,
             uRht,
             apaOffRht,
             apaMulRht;
    APLFLOAT aplFloatRht;
    APLCHAR  aplCharRht;
    UINT     uBitIndex = 0;

    DBGENTER;

    // The right arg data is a valid HGLOBAL array
    Assert (IsGlbTypeVarDir (aplNestedRht));

    // Clear the identifying bits
    hGlbRht = ClrPtrTypeDirAsGlb (aplNestedRht);

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)
    // Get the Array Type, NELM, and Rank
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
    if (IsSimpleAPA (aplTypeRes))
    {
        if (!(*lpPrimSpec->ApaResultDyd_EM) (NULL,
                                             lptkFunc,
                                             NULL,
                                             hGlbRht,
                                            &hGlbRes,
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
         || IsNested (aplTypeRes));

    // Allocate space for result
    if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                    &hGlbRes,
                                     NULL,
                                     lpMemRht,
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

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    // If the right arg is an APA, ...
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // If simple result, ...
    if (IsSimpleNum (aplTypeRes))
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
    else
    // If nested result, ...
    if (IsNested (aplTypeRes))
    {
        // Loop through the right arg/result
        for (uRht = 0; uRht < (APLNELMSIGN) aplNELMRht; uRht++)
        {
            APLSTYPE aplTypeHetRht;

            // Copy in case we are heterogeneous
            aplTypeHetRht = aplTypeRht;

            // Get the right arg element
            hGlbSub = ((LPAPLNESTED) lpMemRht)[uRht];

            // Split cases based upon the ptr type of the nested right arg
            switch (GetPtrTypeDir (hGlbSub))
            {
                case PTRTYPE_STCONST:
                    GetFirstValueImm (((LPSYMENTRY) hGlbSub)->stFlags.ImmType,
                                      ((LPSYMENTRY) hGlbSub)->stData.stLongest,
                                     &aplIntegerRht,
                                     &aplFloatRht,
                                     &aplCharRht,
                                      NULL,
                                      NULL,
                                      NULL,
                                     &aplTypeHetRht);
                    hGlbSub = PrimFnDydSiScSiSc_EM (lptkFunc,
                                                    aplTypeLft,
                                                    aplIntegerLft,
                                                    aplFloatLft,
                                                    aplCharLft,
                                                    aplTypeHetRht,
                                                    aplIntegerRht,
                                                    aplFloatRht,
                                                    aplCharRht,
                                                    lpPrimSpec);
                    if (!hGlbSub)
                        goto ERROR_EXIT;
                    else
                        *((LPAPLNESTED) lpMemRes)++ = hGlbSub;
                    break;

                case PTRTYPE_HGLOBAL:
                    hGlbSub = PrimFnDydNestSiSc_EM (lptkFunc,
                                                    aplTypeLft,
                                                    aplIntegerLft,
                                                    aplFloatLft,
                                                    aplCharLft,
                                                    hGlbSub,
                                                    lpPrimSpec);
                    if (!hGlbSub)
                        goto ERROR_EXIT;
                    else
                        *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeGlb (hGlbSub);
                    break;

                defstop
                    break;
            } // End SWITCH
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
} // End PrimFnDydSiScNest_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSiScSiSc_EM
//
//  Dyadic primitive scalar function, left simple scalar, right simple scalar
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSiScSiSc_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL PrimFnDydSiScSiSc_EM
    (LPTOKEN    lptkFunc,           // Ptr to function token
     APLSTYPE   aplTypeLft,         // Left arg storage type
     APLINT     aplIntegerLft,      // ...      as integer
     APLFLOAT   aplFloatLft,        // ...         float
     APLCHAR    aplCharLft,         // ...         char
     APLSTYPE   aplTypeRht,         // Right arg storage type
     APLINT     aplIntegerRht,      // ...       as integer
     APLFLOAT   aplFloatRht,        // ...          float
     APLCHAR    aplCharRht,         // ...          char
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    TOKEN    tkRes = {0};   // Result token
    APLSTYPE aplTypeRes;    // Result storage type

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (1,
                                               &aplTypeLft,
                                                lptkFunc,
                                                1,
                                               &aplTypeRht);
    if (aplTypeRes EQ ARRAY_ERROR)
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    if (PrimFnDydSiScSiScSub_EM (&tkRes,
                                  lptkFunc,
                                  aplTypeRes,
                                  aplTypeLft,
                                  aplIntegerLft,
                                  aplFloatLft,
                                  aplCharLft,
                                  aplTypeRht,
                                  aplIntegerRht,
                                  aplFloatRht,
                                  aplCharRht,
                                  lpPrimSpec))
        // Convert the immediate type and value in tkRes
        //   into an LPSYMENTRY
        return MakeSymEntry_EM (tkRes.tkFlags.ImmType,
                               &tkRes.tkData.tkLongest,
                                lptkFunc);
    else
        return NULL;
} // End PrimDydSiScSiSc_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSiScSiScSub_EM
//
//  Subroutine to PrimFnDydSiScSiSc_EM
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSiScSiScSub_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydSiScSiScSub_EM
    (LPTOKEN    lptkRes,            // Ptr to result token
     LPTOKEN    lptkFunc,           // Ptr to function token
     APLSTYPE   aplTypeRes,         // Result storage type
     APLSTYPE   aplTypeLft,         // Left arg storage type
     APLINT     aplIntegerLft,      // ...      as an integer
     APLFLOAT   aplFloatLft,        // ...            float
     APLCHAR    aplCharLft,         // ...            char
     APLSTYPE   aplTypeRht,         // Right arg storage type
     APLINT     aplIntegerRht,      // ...       as an integer
     APLFLOAT   aplFloatRht,        // ...             float
     APLCHAR    aplCharRht,         // ...             char
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    UINT immType;               // Result immediate type
    BOOL bRet = TRUE;           // TRUE iff result is valid
RESTART_EXCEPTION_IMMED:
    // Get the immediate type for the token
    immType = TranslateArrayTypeToImmType (aplTypeRes);

    Assert (IsImmNum (immType));

    // Fill in the result token
    lptkRes->tkFlags.TknType   = TKT_VARIMMED;
    lptkRes->tkFlags.ImmType   = immType;
////lptkRes->tkFlags.NoDisplay = 0; // Already zero from YYAlloc
    lptkRes->tkCharIndex       = lptkFunc->tkCharIndex;

    __try
    {
        // Split cases based upon the result's storage type
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:                    // Res = BOOL
                // If both arguments are BOOL,
                //   use BisBvB
                if (IsSimpleBool (aplTypeLft)
                 && IsSimpleBool (aplTypeRht))  // Res = BOOL, Lft = BOOL(S), Rht = BOOL(S)
                {
                    lptkRes->tkData.tkBoolean  =
                      (*lpPrimSpec->BisBvB) ((APLBOOL) aplIntegerLft,
                                             (APLBOOL) aplIntegerRht,
                                             lpPrimSpec);
                } else
                // If both arguments are integer-like (BOOL, INT, or APA),
                //   use BisIvI
                if (IsSimpleInt (aplTypeLft)
                 && IsSimpleInt (aplTypeRht))   // Res = BOOL, Lft = BOOL/INT/APA(S), Rht = BOOL/INT/APA(S)
                {
                    lptkRes->tkData.tkBoolean  =
                      (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                             aplIntegerRht,
                                             lpPrimSpec);
                } else
                // If both arguments are CHAR,
                //   use BisCvC
                if (IsSimpleChar (aplTypeLft)
                 && IsSimpleChar (aplTypeRht))  // Res = BOOL, Lft = CHAR(S), Rht = CHAR(S)
                {
                    lptkRes->tkData.tkBoolean  =
                      (*lpPrimSpec->BisCvC) (aplCharLft,
                                             aplCharRht,
                                             lpPrimSpec);
                } else
                // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                //   use BisFvF
                if ((IsSimpleFlt (aplTypeLft) && IsSimpleNum (aplTypeRht))   // Res = BOOL, Lft = FLOAT(S), Rht = BOOL/INT/APA/FLOAT(S)
                 || (IsSimpleFlt (aplTypeRht) && IsSimpleNum (aplTypeLft)))  // Res = BOOL, Lft = BOOL/INT/APA/FLOAT(S), Rht = FLOAT(S)
                {
                    lptkRes->tkData.tkBoolean  =
                      (*lpPrimSpec->BisFvF) (aplFloatLft,
                                             aplFloatRht,
                                             lpPrimSpec);
                } else
                {
                    // One arg is numeric, the other char
                    Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                         || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                    // If the function is UTF16_NOTEQUAL, the result is one
                    lptkRes->tkData.tkBoolean  = (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                } // End IF/ELSE/...

                break;

            case ARRAY_INT:                     // Res = INT
                // If both left and right arguments are integer-like (BOOL, INT, or APA),
                //   use IisIvI
                if (IsSimpleInt (aplTypeLft)
                 && IsSimpleInt (aplTypeRht))  // Res = INT, Lft = BOOL/INT/APA(S), Rht = BOOL/INT/APA(S)
                {
                    lptkRes->tkData.tkInteger  =
                      (*lpPrimSpec->IisIvI) (aplIntegerLft,
                                             aplIntegerRht,
                                             lpPrimSpec);
                } else
                // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                //   use IisFvF
                if ((IsSimpleFlt (aplTypeLft) && IsSimpleNum (aplTypeRht))   // Res = INT, Lft = FLOAT(S), Rht = BOOL/INT/APA/FLOAT(S)
                 || (IsSimpleFlt (aplTypeRht) && IsSimpleNum (aplTypeLft)))  // Res = INT, Lft = BOOL/INT/APA/FLOAT(S), Rht = FLOAT(S)
                {
                    lptkRes->tkData.tkInteger  =
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
                    lptkRes->tkData.tkFloat    =
                      (*lpPrimSpec->FisIvI) (aplIntegerLft,
                                             aplIntegerRht,
                                             lpPrimSpec);
                } else
                // If both arguments are simple numeric (BOOL, INT, APA, FLOAT),
                //   use FisFvF
                if (IsSimpleNum (aplTypeLft)
                 && IsSimpleNum (aplTypeRht))  // Res = FLOAT, Lft = BOOL/INT/APA/FLOAT(S), Rht = BOOL/INT/APA/FLOAT(S)
                {
                    lptkRes->tkData.tkFloat    =
                      (*lpPrimSpec->FisFvF) (aplFloatLft,
                                             aplFloatRht,
                                             lpPrimSpec);
                } else
                    DbgStop ();         // We should never get here
                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckException (GetExceptionInformation (), "PrimDydSiScSiScSub_EM"))
    {
#ifdef DEBUG
        dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
        DisplayException ();

        // Split cases based upon the ExceptionCode
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_DOMAIN_ERROR:
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                bRet = FALSE;

                goto ERROR_EXIT;

            case EXCEPTION_RESULT_FLOAT:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                if (IsSimpleNum (aplTypeRes)
                 && !IsSimpleFlt (aplTypeRes))
                {
                    aplTypeRes = ARRAY_FLOAT;
#ifdef DEBUG
                    dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
                    goto RESTART_EXCEPTION_IMMED;
                } // End IF

                // Fall through to never-never-land

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except
ERROR_EXIT:
    return bRet;
} // End PrimDydSiScSiScSub_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSimpSimp_EM
//
//  Dyadic primitive scalar function, left simple, right simple
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSimpSimp_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydSimpSimp_EM
    (LPPL_YYSTYPE lpYYRes,          // Ptr to the result

     LPTOKEN      lptkLftArg,       // Ptr to left arg token
     LPTOKEN      lptkFunc,         // ...    function ...
     LPTOKEN      lptkRhtArg,       // ...    right arg ...

     HGLOBAL      hGlbLft,          // Left arg handle
     HGLOBAL      hGlbRht,          // Right ...
     HGLOBAL     *lphGlbRes,        // Ptr to result handle

     LPVOID       lpMemLft,         // Points to Sig.nature
     LPVOID       lpMemRht,         // ...

     LPAPLUINT    lpMemAxisHead,    // Ptr to axis values, fleshed out
     LPAPLUINT    lpMemAxisTail,    // Ptr to grade up of AxisHead

     APLRANK      aplRankLft,       // Left arg rank
     APLRANK      aplRankRht,       // Right ...
     APLRANK      aplRankRes,       // Result ...

     APLSTYPE     aplTypeLft,       // Left arg type
     APLSTYPE     aplTypeRht,       // Right ...
     APLSTYPE     aplTypeRes,       // Result ...

     APLNELM      aplNELMLft,       // Left arg NELM
     APLNELM      aplNELMRht,       // Right ...
     APLNELM      aplNELMRes,       // Result ...
     APLNELM      aplNELMAxis,      // Axis ...
     LPPRIMSPEC   lpPrimSpec)       // Ptr to local PRIMSPEC

{
    LPVOID    lpMemRes = NULL;  // Ptr to result global memory
    BOOL      bRet = TRUE;      // TRUE iff result is valid
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
    APLUINT   ByteRes;          // # bytes in the result
    APLCHAR   aplCharLft,
              aplCharRht;

    DBGENTER;

    //***************************************************************
    // Both arguments are simple
    //***************************************************************

    // Lock the memory to get a ptr to it
    if (*lphGlbRes)
        lpMemRes = MyGlobalLock (*lphGlbRes);

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
            // Get the respective first values
            GetFirstValueToken (lptkLftArg,         // Ptr to left arg token
                               &aplIntegerLft,      // Ptr to integer result
                               &aplFloatLft,        // Ptr to float ...
                               &aplCharLft,         // Ptr to WCHAR ...
                                NULL,               // Ptr to longest ...
                                NULL,               // Ptr to lpSym/Glb ...
                                NULL,               // Ptr to ...immediate type ...
                                NULL);              // Ptr to array type ...
            GetFirstValueToken (lptkRhtArg,         // Ptr to right arg token
                               &aplIntegerRht,      // Ptr to integer result
                               &aplFloatRht,        // Ptr to float ...
                               &aplCharRht,         // Ptr to WCHAR ...
                                NULL,               // Ptr to longest ...
                                NULL,               // Ptr to lpSym/Glb ...
                                NULL,               // Ptr to ...immediate type ...
                                NULL);              // Ptr to array type ...
            bRet =
              PrimFnDydSiScSiScSub_EM (&lpYYRes->tkToken,
                                        lptkFunc,
                                        aplTypeRes,
                                        aplTypeLft,
                                        aplIntegerLft,
                                        aplFloatLft,
                                        aplCharLft,
                                        aplTypeRht,
                                        aplIntegerRht,
                                        aplFloatRht,
                                        aplCharRht,
                                        lpPrimSpec);
        } else
        // It's a singleton array
        {
            // Skip over the header and dimensions to the data
            lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

            // Get the respective values
            GetFirstValueToken (lptkLftArg,     // Ptr to left arg token
                               &aplIntegerLft,  // Ptr to integer result
                               &aplFloatLft,    // Ptr to float ...
                               &aplCharLft,     // Ptr to WCHAR ...
                                NULL,           // Ptr to longest ...
                                NULL,           // Ptr to lpSym/Glb ...
                                NULL,           // Ptr to ...immediate type ...
                                NULL);          // Ptr to array type ...
            GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                               &aplIntegerRht,  // Ptr to integer result
                               &aplFloatRht,    // Ptr to float ...
                               &aplCharRht,     // Ptr to WCHAR ...
                                NULL,           // Ptr to longest ...
                                NULL,           // Ptr to lpSym/Glb ...
                                NULL,           // Ptr to ...immediate type ...
                                NULL);          // Ptr to array type ...
RESTART_EXCEPTION_SINGLETON:
            __try
            {
            // Split cases based upon the result's storage type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:                    // Res = BOOL
                    // If both arguments are BOOL,
                    //   use BisBvB
                    if (IsSimpleBool (aplTypeLft)
                     && IsSimpleBool (aplTypeRht))  // Res = BOOL, Lft = BOOL(S), Rht = BOOL(S)
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
                    if (IsSimpleChar (aplTypeLft)
                     && IsSimpleChar (aplTypeRht))
                    {
                        *((LPAPLBOOL)  lpMemRes) =
                          (*lpPrimSpec->BisCvC) (aplCharLft,
                                                 aplCharRht,
                                                 lpPrimSpec);
                    } else
                    // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                    //   use BisFvF
                    if ((IsSimpleFlt (aplTypeLft) && IsSimpleNum (aplTypeRht))   // Res = BOOL, Lft = FLOAT(S), Rht = BOOL/INT/APA/FLOAT(S)
                     || (IsSimpleFlt (aplTypeRht) && IsSimpleNum (aplTypeLft)))  // Res = BOOL, Lft = BOOL/INT/APA/FLOAT(S), Rht = FLOAT(S)
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
                    if ((IsSimpleFlt (aplTypeLft) && IsSimpleNum (aplTypeRht))   // Res = INT, Lft = FLOAT(S), Rht = BOOL/INT/APA/FLOAT(S)
                     || (IsSimpleFlt (aplTypeRht) && IsSimpleNum (aplTypeLft)))  // Res = INT, Lft = BOOL/INT/APA/FLOAT(S), Rht = FLOAT(S)
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
            } __except (CheckException (GetExceptionInformation (), "PrimFnDydSimpSimp_EM #1"))
            {
#ifdef DEBUG
                dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
                DisplayException ();

                // Split cases based upon the ExceptionCode
                switch (GetExceptionCode ())
                {
                    case EXCEPTION_DOMAIN_ERROR:
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                           lptkFunc);
                        bRet = FALSE;

                        goto ERROR_EXIT;

                    case EXCEPTION_RESULT_FLOAT:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsSimpleNum (aplTypeRes)
                         && !IsSimpleFlt (aplTypeRes))
                        {
                            aplTypeRes = ARRAY_FLOAT;
#ifdef DEBUG
                            dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
                            goto RESTART_EXCEPTION_SINGLETON;
                        } // End IF

                        // Fall through to never-never-land

                    default:
                        // Display message for unhandled exception
                        DisplayException ();

                        break;
                } // End SWITCH
            } // End __try/__except

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = 0; // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0; // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
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

        // If the non-singleton arg is an APA, ...
        if (IsSimpleAPA (aplTypeArg))
        {
#define lpAPA       ((LPAPLAPA) lpMemDimArg)
            // Get the APA parameters
            apaOffLft = apaOffRht = lpAPA->Off;
            apaMulLft = apaMulRht = lpAPA->Mul;
#undef  lpAPA
        } // End IF

        // Get the value of the singleton
        if (aplNELMLft EQ 1)
            GetFirstValueToken (lptkLftArg,     // Ptr to left arg token
                               &aplIntegerLft,  // Ptr to integer result
                               &aplFloatLft,    // Ptr to float ...
                               &aplCharLft,     // Ptr to WCHAR ...
                                NULL,           // Ptr to longest ...
                                NULL,           // Ptr to lpSym/Glb ...
                                NULL,           // Ptr to ...immediate type ...
                                NULL);          // Ptr to array type ...
        else
            GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                               &aplIntegerRht,  // Ptr to integer result
                               &aplFloatRht,    // Ptr to float ...
                               &aplCharRht,     // Ptr to WCHAR ...
                                NULL,           // Ptr to longest ...
                                NULL,           // Ptr to lpSym/Glb ...
                                NULL,           // Ptr to ...immediate type ...
                                NULL);          // Ptr to array type ...
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
        // Check for error
        if (!bRet)
            goto ERROR_EXIT;

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
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

        // If the left arg is an APA, ...
        if (IsSimpleAPA (aplTypeLft))
        {
#define lpAPA       ((LPAPLAPA) lpMemLft)
            // Get the APA parameters
            apaOffLft = lpAPA->Off;
            apaMulLft = lpAPA->Mul;
#undef  lpAPA
        } // End IF

        // If the right arg is an APA, ...
        if (IsSimpleAPA (aplTypeRht))
        {
#define lpAPA       ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
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
            ByteRes = aplRankRes * sizeof (APLUINT);
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
            ByteRes = aplRankRes * sizeof (APLUINT);
            Assert (ByteRes EQ (UINT) ByteRes);
            hGlbOdo = DbgGlobalAlloc (GHND, (UINT) ByteRes);
            if (!hGlbOdo)
            {
                ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Lock the memory to get a ptr to it
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
                    if (IsSimpleBool (aplTypeLft)
                     && IsSimpleBool (aplTypeRht))  // Res = BOOL(Axis), Lft = BOOL, Rht = BOOL
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
                              (*lpPrimSpec->BisBvB) ((APLBOOL) GetNextInteger (lpMemLft, aplTypeLft, uLft),
                                                     (APLBOOL) GetNextInteger (lpMemRht, aplTypeRht, uRht),
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
                              (*lpPrimSpec->BisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uLft),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRht),
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
                    if (IsSimpleChar (aplTypeLft)
                     && IsSimpleChar (aplTypeRht))  // Res = BOOL(Axis), Lft = CHAR, Rht = CHAR
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
                    if ((IsSimpleFlt (aplTypeLft) && IsSimpleNum (aplTypeRht))   // Res = BOOL(Axis), Lft = FLOAT, Rht = BOOL/INT/APA/FLOAT
                     || (IsSimpleFlt (aplTypeRht) && IsSimpleNum (aplTypeLft)))  // Res = BOOL(Axis), Lft = BOOL/INT/APA/FLOAT, Rht = FLOAT
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
                              (*lpPrimSpec->BisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uLft),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRht),
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                    // If either arg is hetero and the other is simple, ...
                    if ((IsSimpleHet (aplTypeLft) && IsSimple (aplTypeRht))     // Res = BOOL(No Axis), Lft = HETERO, Rht = BOOL/INT/APA/FLOAT/CHAR/HETERO
                     || (IsSimpleHet (aplTypeRht) && IsSimple (aplTypeLft)))    // Res = BOOL(No Axis), Lft = BOOL/INT/APA/FLOAT/CHAR/HETERO, Rht = HETERO
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
                            // Get the next values and type
                            aplTypeLft = GetNextHetero (lpMemLft, uLft, &aplIntegerLft, &aplFloatLft, &aplCharLft);
                            aplTypeRht = GetNextHetero (lpMemRht, uRht, &aplIntegerRht, &aplFloatRht, &aplCharRht);

                            // Split cases based upon the left hetero's storage type
                            switch (aplTypeLft)
                            {
                                case ARRAY_BOOL:            // Lft = BOOL,  Rht = BOOL/INT/FLOAT/CHAR
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeRht)
                                    {
                                        case ARRAY_BOOL:    // Lft = BOOL,  Rht = BOOL
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisBvB) ((APLBOOL) aplIntegerLft,
                                                                     (APLBOOL) aplIntegerRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_INT:     // Lft = BOOL, Rht = INT
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisIvI) ((APLBOOL) aplIntegerLft,
                                                                               aplIntegerRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_FLOAT:   // Lft = BOOL, Rht = FLOAT
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisFvF) ((APLFLOAT) aplIntegerLft,
                                                                                aplFloatRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_CHAR:    // Lft = BOOL, Rht = CHAR
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_INT:             // Lft = INT,   Rht = BOOL/INT/FLOAT/CHAR
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeRht)
                                    {
                                        case ARRAY_BOOL:    // Lft = INT,  Rht = BOOL
                                        case ARRAY_INT:     // Lft = INT,  Rht = INT
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                                     aplIntegerRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_FLOAT:   // Lft = INT,  Rht = FLOAT
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                                     aplFloatRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_CHAR:    // Lft = INT,  Rht = CHAR
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_FLOAT:           // Lft = FLOAT, Rht = BOOL/INT/FLOAT/CHAR
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeRht)
                                    {
                                        case ARRAY_BOOL:    // Lft = FLOAT, Rht = BOOL
                                        case ARRAY_INT:     // Lft = FLOAT, Rht = INT
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                                     aplIntegerRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_FLOAT:   // Lft = FLOAT, Rht = FLOAT
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                                     aplFloatRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_CHAR:    // Lft = FLOAT, Rht = CHAR
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_CHAR:            // Lft = CHAR,  Rht = BOOL/INT/FLOAT/CHAR
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeRht)
                                    {
                                        case ARRAY_BOOL:    // Lft = CHAR,  Rht = BOOL
                                        case ARRAY_INT:     // Lft = CHAR,  Rht = INT
                                        case ARRAY_FLOAT:   // Lft = CHAR,  Rht = FLOAT
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        case ARRAY_CHAR:    // Lft = CHAR,  Rht = CHAR
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisCvC) (aplCharLft,
                                                                     aplCharRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

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
                              (*lpPrimSpec->IisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uLft),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRht),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                    // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                    //   use IisFvF
                    if ((IsSimpleFlt (aplTypeLft) && IsSimpleNum (aplTypeRht))   // Res = INT(Axis), Lft = FLOAT, Rht = BOOL/INT/APA/FLOAT
                     || (IsSimpleFlt (aplTypeRht) && IsSimpleNum (aplTypeLft)))  // Res = INT(Axis), Rht = BOOL/INT/APA/FLOAT, Rht = FLOAT
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
                              (*lpPrimSpec->IisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uLft),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRht),
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
                              (*lpPrimSpec->FisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uLft),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRht),
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
                              (*lpPrimSpec->FisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uLft),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRht),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                        DbgStop ();     // We should never get here
                    break;

                defstop
                    break;
            } // End SWITCH
            } __except (CheckException (GetExceptionInformation (), "PrimFnDydSimpSimp_EM #2"))
            {
#ifdef DEBUG
                dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
                DisplayException ();

                // Split cases based upon the ExceptionCode
                switch (GetExceptionCode ())
                {
                    case EXCEPTION_DOMAIN_ERROR:
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        return FALSE;

                    case EXCEPTION_RESULT_FLOAT:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsSimpleNum (aplTypeRes)
                         && !IsSimpleFlt (aplTypeRes))
                        {
                            aplTypeRes = ARRAY_FLOAT;

                            // Restart the pointers
                            lpMemRes = lpMemResStart;

                            // Re-initialize lpMemOdo
                            for (uRes = 0 ; uRes < (APLRANKSIGN) aplRankRes; uRes++)
                                lpMemOdo[uRes] = 0;
#ifdef DEBUG
                            dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
                            goto RESTART_EXCEPTION_AXIS;
                        } // End IF

                        // Fall through to never-never-land

                    default:
                        // Display message for unhandled exception
                        DisplayException ();

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
                    if (IsSimpleBool (aplTypeLft)
                     && IsSimpleBool (aplTypeRht))  // Res = BOOL(No Axis), Lft = BOOL, Rht = BOOL
                    {
                        // ***FIXME*** -- Optimize by chunking

                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisBvB) ((APLBOOL) GetNextInteger (lpMemLft, aplTypeLft, uRes),
                                                     (APLBOOL) GetNextInteger (lpMemRht, aplTypeRht, uRes),
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
                              (*lpPrimSpec->BisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uRes),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRes),
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
                    if (IsSimpleChar (aplTypeLft)
                     && IsSimpleChar (aplTypeRht))  // Res = BOOL(No Axis), Lft = CHAR, Rht = CHAR
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
                    if ((IsSimpleFlt (aplTypeLft) && IsSimpleNum (aplTypeRht))  // Res = BOOL(No Axis), Lft = FLOAT, Rht = BOOL/INT/APA/FLOAT
                     || (IsSimpleFlt (aplTypeRht) && IsSimpleNum (aplTypeLft))) // Res = BOOL(No Axis), Lft = BOOL/INT/APA/FLOAT, Rht = FLOAT
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            *((LPAPLBOOL)  lpMemRes) |=
                              (*lpPrimSpec->BisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uRes),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRes),
                                                     lpPrimSpec) << uBitIndex;
                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;                  // Start over
                                ((LPAPLBOOL) lpMemRes)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } else
                    // If either arg is hetero and the other is simple, ...
                    if ((IsSimpleHet (aplTypeLft) && IsSimple (aplTypeRht))     // Res = BOOL(No Axis), Lft = HETERO, Rht = BOOL/INT/APA/FLOAT/CHAR/HETERO
                     || (IsSimpleHet (aplTypeRht) && IsSimple (aplTypeLft)))    // Res = BOOL(No Axis), Lft = BOOL/INT/APA/FLOAT/CHAR/HETERO, Rht = HETERO
                    {
                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            // Get the next values and type
                            aplTypeLft = GetNextHetero (lpMemLft, uRes, &aplIntegerLft, &aplFloatLft, &aplCharLft);
                            aplTypeRht = GetNextHetero (lpMemRht, uRes, &aplIntegerRht, &aplFloatRht, &aplCharRht);

                            // Split cases based upon the left hetero's storage type
                            switch (aplTypeLft)
                            {
                                case ARRAY_BOOL:            // Lft = BOOL,  Rht = BOOL/INT/FLOAT/CHAR
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeRht)
                                    {
                                        case ARRAY_BOOL:    // Lft = BOOL,  Rht = BOOL
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisBvB) ((APLBOOL) aplIntegerLft,
                                                                     (APLBOOL) aplIntegerRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_INT:     // Lft = BOOL, Rht = INT
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisIvI) ((APLBOOL) aplIntegerLft,
                                                                               aplIntegerRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_FLOAT:   // Lft = BOOL, Rht = FLOAT
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisFvF) ((APLFLOAT) aplIntegerLft,
                                                                                aplFloatRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_CHAR:    // Lft = BOOL, Rht = CHAR
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_INT:             // Lft = INT,   Rht = BOOL/INT/FLOAT/CHAR
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeRht)
                                    {
                                        case ARRAY_BOOL:    // Lft = INT,  Rht = BOOL
                                        case ARRAY_INT:     // Lft = INT,  Rht = INT
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                                     aplIntegerRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_FLOAT:   // Lft = INT,  Rht = FLOAT
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                                     aplFloatRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_CHAR:    // Lft = INT,  Rht = CHAR
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_FLOAT:           // Lft = FLOAT, Rht = BOOL/INT/FLOAT/CHAR
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeRht)
                                    {
                                        case ARRAY_BOOL:    // Lft = FLOAT, Rht = BOOL
                                        case ARRAY_INT:     // Lft = FLOAT, Rht = INT
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisIvI) (aplIntegerLft,
                                                                     aplIntegerRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_FLOAT:   // Lft = FLOAT, Rht = FLOAT
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisFvF) (aplFloatLft,
                                                                     aplFloatRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        case ARRAY_CHAR:    // Lft = FLOAT, Rht = CHAR
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

                                    break;

                                case ARRAY_CHAR:            // Lft = CHAR,  Rht = BOOL/INT/FLOAT/CHAR
                                    // Split cases based upon the right hetero's storage type
                                    switch (aplTypeRht)
                                    {
                                        case ARRAY_BOOL:    // Lft = CHAR,  Rht = BOOL
                                        case ARRAY_INT:     // Lft = CHAR,  Rht = INT
                                        case ARRAY_FLOAT:   // Lft = CHAR,  Rht = FLOAT
                                            // One arg is numeric, the other char
                                            Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
                                                 || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);
                                            // If the function is UTF16_NOTEQUAL, the result is one
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL) << uBitIndex;

                                            break;

                                        case ARRAY_CHAR:    // Lft = CHAR,  Rht = CHAR
                                            *((LPAPLBOOL)  lpMemRes) |=
                                              (*lpPrimSpec->BisCvC) (aplCharLft,
                                                                     aplCharRht,
                                                                     lpPrimSpec) << uBitIndex;
                                            break;

                                        defstop
                                            break;
                                    } // End SWITCH

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
                              (*lpPrimSpec->IisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uRes),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRes),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                    // If either argument is FLOAT and the other is simple numeric (BOOL, INT, APA, or FLOAT),
                    //   use IisFvF
                    if ((IsSimpleFlt (aplTypeLft) && IsSimpleNum (aplTypeRht))   // Res = INT(No Axis), Lft = FLOAT, Rht = BOOL/INT/APA/FLOAT
                     || (IsSimpleFlt (aplTypeRht) && IsSimpleNum (aplTypeLft)))  // Res = INT(No Axis), Lft = BOOL/INT/APA/FLOAT, Rht = FLOAT
                    {
                        DbgBrk ();      // ***TESTME*** -- No such primitive

                        // Loop through the left/right args/result
                        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMRes; uRes++)
                        {
                            *((LPAPLINT)   lpMemRes)++ =
                              (*lpPrimSpec->IisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uRes),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRes),
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
                              (*lpPrimSpec->FisIvI) (GetNextInteger (lpMemLft, aplTypeLft, uRes),
                                                     GetNextInteger (lpMemRht, aplTypeRht, uRes),
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
                              (*lpPrimSpec->FisFvF) (GetNextFloat (lpMemLft, aplTypeLft, uRes),
                                                     GetNextFloat (lpMemRht, aplTypeRht, uRes),
                                                     lpPrimSpec);
                        } // End FOR
                    } else
                        DbgStop ();         // We should never get here
                    break;

                defstop
                    break;
            } // End SWITCH
            } __except (CheckException (GetExceptionInformation (), "PrimFnDydSimpSimp_EM #3"))
            {
#ifdef DEBUG
                dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
                DisplayException ();

                // Split cases based upon the ExceptionCode
                switch (GetExceptionCode ())
                {
                    case EXCEPTION_DOMAIN_ERROR:
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        return FALSE;

                    case EXCEPTION_RESULT_FLOAT:
                        MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                        if (IsSimpleNum (aplTypeRes)
                         && !IsSimpleFlt (aplTypeRes))
                        {
                            aplTypeRes = ARRAY_FLOAT;

                            // Restart the pointers
                            lpMemRes = lpMemResStart;
#ifdef DEBUG
                            dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
                            goto RESTART_EXCEPTION_NOAXIS;
                        } // End IF

                        // Fall through to never-never-land

                    default:
                        // Display message for unhandled exception
                        DisplayException ();

                        break;
                } // End SWITCH
            } // End __try/__except
        } // End IF/ELSE

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE/...

    // Unlock the result global memory in case TypeDemote actually demotes
    if (*lphGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;
    } // End IF

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);
ERROR_EXIT:
    if (*lphGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;
    } // End IF

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
//  $CalcLftRhtArgIndices
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
//  $TranslateQuadICIndex
//
//  Translate a given []IC index into an APLFLOAT return value
//***************************************************************************

APLFLOAT TranslateQuadICIndex
    (IC_INDICES icIndex)

{
    // Split cases based upon the DIV0 value
    switch (GetQuadICValue (icIndex))
    {
        case ICVAL_ZERO:
            return 0;

        case ICVAL_ONE:
            return 1;

        case ICVAL_DOMAIN_ERROR:
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        case ICVAL_POS_INFINITY:
            return PosInfinity;

        case ICVAL_NEG_INFINITY:
            return NegInfinity;

        defstop
            return 0;
    } // End SWITCH
} // TranslateQuadICIndex


//***************************************************************************
//  End of File: primspec.c
//***************************************************************************
