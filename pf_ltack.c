//***************************************************************************
//  NARS2000 -- Primitive Function -- LeftTack
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
//  PrimFnLeftTack_EM
//
//  Primitive function for monadic and dyadic LeftTack (ERROR and "left")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnLeftTack_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnLeftTack_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_LEFTTACK);

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
        return PrimFnMonLeftTack_EM (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydLeftTack_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnLeftTack_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonLeftTack_EM
//
//  Primitive function for monadic LeftTack (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonLeftTack_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonLeftTack_EM
    (LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    return PrimFnSyntaxError_EM (lptkFunc);
} // End PrimFnMonLeftTack_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydLeftTack_EM
//
//  Primitive function for dyadic LeftTack ("left")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydLeftTack_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDydLeftTack_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    HGLOBAL hGlbData;
    UINT    YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = NewYYResIndex ();

    // Fill in the result token
    YYRes[YYLclIndex].tkToken = *lptkLftArg;

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
////////////////YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;    // Already zero from ZeroMemory
////////////////YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;    // Already zero from ZeroMemory
                YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = CopySymGlbDir (hGlbData);
////////////////YYRes[YYLclIndex].tkToken.tkCharIndex       =           // Already set
            } else
            {
                // Fill in the result token
                YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARIMMED;
                YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = YYRes[YYLclIndex].tkToken.tkData.lpSym->stFlags.ImmType;
////////////////YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;    // Already zero from ZeroMemory
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
} // End PrimFnLeftTack_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_ltack.c
//***************************************************************************
