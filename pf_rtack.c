//***************************************************************************
//  NARS2000 -- Primitive Function -- RightTack
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  PrimFnRightTack_EM
//
//  Primitive function for monadic and dyadic RightTack (ERROR and "right")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnRightTack_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnRightTack_EM
    (LPTOKEN       lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN       lptkFunc,        // Ptr to function token
     LPTOKEN       lptkRhtArg,      // Ptr to right arg token
     LPTOKEN       lptkAxis,        // Ptr to axis token (may be NULL)
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_RIGHTTACK);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis,
                                   lpplLocalVars);
        return NULL;
    } // End IF

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonRightTack_EM (            lptkFunc, lptkRhtArg, lptkAxis, lpplLocalVars);
    else
        return PrimFnDydRightTack_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpplLocalVars);
} // End PrimFnRightTack_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonRightTack_EM
//
//  Primitive function for monadic RightTack (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonRightTack_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonRightTack_EM
    (LPTOKEN       lptkFunc,        // Ptr to function token
     LPTOKEN       lptkRhtArg,      // Ptr to right arg token
     LPTOKEN       lptkAxis,        // Ptr to axis token (may be NULL)
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    return PrimFnSyntaxError_EM (lptkFunc, lpplLocalVars);
} // End PrimFnMonRightTack_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydRightTack_EM
//
//  Primitive function for dyadic RightTack ("left")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydRightTack_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDydRightTack_EM
    (LPTOKEN       lptkLftArg,      // Ptr to left arg token
     LPTOKEN       lptkFunc,        // Ptr to function token
     LPTOKEN       lptkRhtArg,      // Ptr to right arg token
     LPTOKEN       lptkAxis,        // Ptr to axis token (may be NULL)
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    HGLOBAL   hGlbData;
    LPYYSTYPE lpYYRes;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_RIGHTTACK);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis,
                                   lpplLocalVars);
        YYFree (lpYYRes); lpYYRes = NULL; return NULL;
    } // End IF

    // Fill in the result token
    lpYYRes->tkToken = *lptkRhtArg;

    // If this is a global memory object, increment its reference count
    // Split cases based upon the token type
    switch (lpYYRes->tkToken.tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYRes->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lpYYRes->tkToken.tkData.tkSym->stFlags.Imm)
            {
                // Get the global memory handle
                hGlbData = lpYYRes->tkToken.tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbData));

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;         // Already zero from YYAlloc
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;         // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData = CopySymGlbDir (hGlbData);
////////////////lpYYRes->tkToken.tkCharIndex       =            // Already set
            } else
            {
                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkFlags.ImmType   = lpYYRes->tkToken.tkData.tkSym->stFlags.ImmType;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;         // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkLongest  = lpYYRes->tkToken.tkData.tkSym->stData.stLongest;
////////////////lpYYRes->tkToken.tkCharIndex       =            // Already set
            } // End IF/ELSE

            break;

        case TKT_VARIMMED:
            break;

        case TKT_VARARRAY:
            // Get the global memory handle
            hGlbData = lpYYRes->tkToken.tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbData));

            // Make a copy of the data
            lpYYRes->tkToken.tkData.tkGlbData = CopySymGlbDir (hGlbData);

            break;

        defstop
            break;
    } // End SWITCH

    return lpYYRes;
} // End PrimFnRightTack_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_rtack.c
//***************************************************************************
