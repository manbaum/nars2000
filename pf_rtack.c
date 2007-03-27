//***************************************************************************
//  NARS2000 -- Primitive Function -- RightTack
//***************************************************************************

#pragma pack (1)
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
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

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
                                   lptkAxis);
        return NULL;
    } // End IF

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonRightTack_EM (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydRightTack_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
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
    (LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    return PrimFnSyntaxError_EM (lptkFunc);
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
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    HGLOBAL hGlbData;
    UINT    YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = NewYYResIndex ();

    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_RIGHTTACK);

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

    // Fill in the result token
    YYRes[YYLclIndex].tkToken = *lptkRhtArg;

    // If this is a global memory object, increment its reference count
    // Split cases based upon the token type
    switch (YYRes[YYLclIndex].tkToken.tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (YYRes[YYLclIndex].tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!YYRes[YYLclIndex].tkToken.tkData.lpSym->stFlags.Imm)
            {
                // Get the global memory handle
                hGlbData = YYRes[YYLclIndex].tkToken.tkData.lpSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbData));

                // Fill in the result token
                YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;        // Already zero from ZeroMemory
////////////////YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;        // Already zero from ZeroMemory
                YYRes[YYLclIndex].tkToken.tkData.tkGlbData = CopySymGlbDir (hGlbData);
////////////////YYRes[YYLclIndex].tkToken.tkCharIndex       =           // Already set
            } else
            {
                // Fill in the result token
                YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARIMMED;
                YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = YYRes[YYLclIndex].tkToken.tkData.lpSym->stFlags.ImmType;
////////////////YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;        // Already zero from ZeroMemory
                YYRes[YYLclIndex].tkToken.tkData.tkLongest  = YYRes[YYLclIndex].tkToken.tkData.lpSym->stData.stLongest;
////////////////YYRes[YYLclIndex].tkToken.tkCharIndex       =           // Already set
            } // End IF/ELSE

            break;

        case TKT_VARIMMED:
            break;

        case TKT_VARARRAY:
            // Get the global memory handle
            hGlbData = YYRes[YYLclIndex].tkToken.tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbData));

            // Make a copy of the data
            YYRes[YYLclIndex].tkToken.tkData.tkGlbData = CopySymGlbDir (hGlbData);

            break;

        defstop
            break;
    } // End SWITCH

    return &YYRes[YYLclIndex];
} // End PrimFnRightTack_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_rtack.c
//***************************************************************************
