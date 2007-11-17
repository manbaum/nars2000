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
    return PrimFnValenceError_EM (lptkFunc);
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
    return PrimFnDydTackCommon_YY (lptkRhtArg, lptkFunc);
} // End PrimFnRightTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydTackCommon_YY
//
//  Common subroutine to PrimFnDydUpTack_EM_YY & PrimFnDydDownTack_EM_YY
//***************************************************************************

LPPL_YYSTYPE PrimFnDydTackCommon_YY
    (LPTOKEN lptkArg,               // Ptr to arg token
     LPTOKEN lptkFunc)              // Ptr to function token

{
    APLSTYPE     aplType;           // Arg storage type
    APLLONGEST   aplLongest;        // Arg immediate value
    HGLOBAL      hGlbArg;           // Arg global memory handle
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Get the attributes (Type)
    //   of the arg
    AttrsOfToken (lptkArg, &aplType, NULL, NULL, NULL);

    // Get arg global ptr
    aplLongest = GetGlbPtrs_LOCK (lptkArg, &hGlbArg, NULL);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();
    lpYYRes->tkToken.tkCharIndex           = lptkFunc->tkCharIndex;

    // If the arg is a global, ...
    if (hGlbArg)
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDirAsGlb (hGlbArg);
////////lpYYRes->tkToken.tkCharIndex       =           // Already set
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = TranslateArrayTypeToImmType (aplType);
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkLongest  = aplLongest;
////////lpYYRes->tkToken.tkCharIndex       =           // Already set
    } // End IF/ELSE

    return lpYYRes;
} // End PrimFnDydTackCommon_YY


//***************************************************************************
//  End of File: pf_rtack.c
//***************************************************************************
