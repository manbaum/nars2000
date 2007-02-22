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
//  Primitive function for monadic and dyadic RightTack (ERROR and "left")
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
    static YYSTYPE YYRes;   // The result
    HGLOBAL hGlbData;

    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_RIGHTTACK);

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
        return PrimFnMonSyntaxError_EM (lptkFunc, NULL, NULL, NULL);
    else
    {
        // Fill in the result token
        YYRes.tkToken = *lptkLftArg;

        // If this is a global memory object, increment its reference count
        // Split cases based upon the token type
        switch (YYRes.tkToken.tkFlags.TknType)
        {
            case TKT_VARNAMED:
                // tkData is an LPSYMENTRY
                Assert (GetPtrTypeDir (YYRes.tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

                // If it's not immediate, we must look inside the array
                if (!YYRes.tkToken.tkData.lpSym->stFlags.Imm)
                {
                    // Get the global memory handle
                    hGlbData = YYRes.tkToken.tkData.lpSym->stData.stGlbData;

                    // stData is a valid HGLOBAL variable array
                    Assert (IsGlbTypeVarDir (hGlbData));

                    // Fill in the result token
                    YYRes.tkToken.tkFlags.TknType = TKT_VARARRAY;
////////////////////YYRes.tkToken.tkFlags.ImmType   = 0;        // Already zero from static
////////////////////YYRes.tkToken.tkFlags.NoDisplay = 0;        // Already zero from static
////////////////////YYRes.tkToken.tkFlags.Color     =
////////////////////YYRes.tkToken.tkCharIndex       =           // Already set

                    // Make a copy of the data
                    YYRes.tkToken.tkData.tkGlbData =
                      MakeGlbTypeGlb (CopyArray_EM (hGlbData, FALSE, NULL));
                } else
                {
                    // Fill in the result token
                    YYRes.tkToken.tkFlags.TknType = TKT_VARIMMED;
                    YYRes.tkToken.tkFlags.ImmType   = YYRes.tkToken.tkData.lpSym->stFlags.ImmType;
////////////////////YYRes.tkToken.tkFlags.NoDisplay = 0;        // Already zero from static
////////////////////YYRes.tkToken.tkFlags.Color     =
////////////////////YYRes.tkToken.tkCharIndex       =           // Already set
                    YYRes.tkToken.tkData.tkLongest  = YYRes.tkToken.tkData.lpSym->stData.stLongest;

                } // End IF/ELSE

                break;

            case TKT_VARIMMED:
                break;

            case TKT_VARARRAY:
                // Get the global memory handle
                hGlbData = YYRes.tkToken.tkData.tkGlbData;

                // tkData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbData));

                // Make a copy of the data
                YYRes.tkToken.tkData.tkGlbData =
                  MakeGlbTypeGlb (CopyArray_EM (hGlbData, FALSE, NULL));
                break;

            defstop
                break;
        } // End SWITCH

        return &YYRes;
    } // End IF/ELSE
} // End PrimFnRightTack_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_rtack.c
//***************************************************************************
