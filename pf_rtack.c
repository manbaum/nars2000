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
//  $PrimFnRightTack_EM_YY
//
//  Primitive function for monadic and dyadic RightTack (ERROR and "right")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnRightTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnRightTack_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_RIGHTTACK);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

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
        return PrimFnMonRightTack_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydRightTack_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnRightTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnRightTack_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic RightTack
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnRightTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnRightTack_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnRightTack_EM_YY,  // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnRightTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonRightTack_EM_YY
//
//  Primitive function for monadic RightTack (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonRightTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonRightTack_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnSyntaxError_EM (lptkFunc);
} // End PrimFnMonRightTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydRightTack_EM_YY
//
//  Primitive function for dyadic RightTack ("right")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydRightTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydRightTack_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    HGLOBAL      hGlbData;
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_RIGHTTACK);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken = *lptkRhtArg;

    // If this is a global memory object, increment its reference count
    // Split cases based upon the token type
    switch (lpYYRes->tkToken.tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYRes->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

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
} // End PrimFnRightTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_rtack.c
//***************************************************************************
