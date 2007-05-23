//***************************************************************************
//  NARS2000 -- Primitive Function -- LeftTack
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
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

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
//  PrimProtoFnLeftTack_EM
//
//  Generate a prototype for the primitive functions monadic & dyadic LeftTack
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnLeftTack_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimProtoFnLeftTack_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM (&PrimFnLeftTack_EM, // Ptr to primitive function routine
                                 lptkLftArg,        // Ptr to left arg token
                                 lptkFunc,          // Ptr to function token
                                 lptkRhtArg,        // Ptr to right arg token
                                 lptkAxis);         // Ptr to axis token (may be NULL)
////     HGLOBAL   hGlbData;
////     LPYYSTYPE lpYYRes;
////
////     //***************************************************************
////     // This function is not sensitive to the axis operator,
////     //   so signal a syntax error if present
////     //***************************************************************
////
////     if (lptkAxis NE NULL)
////     {
////         ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
////                                    lptkAxis);
////         return NULL;
////     } // End IF
////
////     // If the left arg is not present, ...
////     if (lptkLftArg EQ NULL)
////         //***************************************************************
////         // Called monadically
////         //***************************************************************
////         return PrimFnSyntaxError_EM (lptkFunc);
////     else
////     {
////         //***************************************************************
////         // Called dyadically
////         //***************************************************************
////
////         // The result is the prototype of the left arg
////
////         // Allocate a new YYRes
////         lpYYRes = YYAlloc ();
////
////         // Fill in the result token
////         lpYYRes->tkToken = *lptkLftArg;
////
////         // Split cases based upon the token type of the result
////         switch (lpYYRes->tkToken.tkFlags.TknType)
////         {
////             case TKT_VARNAMED:
////                 // tkData is an LPSYMENTRY
////                 Assert (GetPtrTypeDir (lpYYRes->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);
////
////                 // If it's not immediate, we must look inside the array
////                 if (!lpYYRes->tkToken.tkData.tkSym->stFlags.Imm)
////                 {
////                     // Get the global memory handle
////                     hGlbData = lpYYRes->tkToken.tkData.tkSym->stData.stGlbData;
////
////                     // stData is a valid HGLOBAL variable array
////                     Assert (IsGlbTypeVarDir (hGlbData));
////
////                     // Make the prototype
////                     hGlbData = MakePrototype_EM (hGlbData,      // Proto arg handle
////                                                  lptkFunc,      // Ptr to function token
////                                                  MP_CHARS);     // CHARs allowed
////                     // Fill in the result token
////                     lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
//// ////////////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
//// ////////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
////                     lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbData);
//// ////////////////////lpYYRes->tkToken.tkCharIndex       =           // Already set
////                 } else
////                 {
////                     // Fill in the result token
////                     lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
////                     lpYYRes->tkToken.tkFlags.ImmType   = lpYYRes->tkToken.tkData.tkSym->stFlags.ImmType;
//// ////////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
////                     // Convert to 0 or blank
////                     if (lpYYRes->tkToken.tkData.tkSym->stFlags.ImmType EQ IMMTYPE_CHAR)
////                         lpYYRes->tkToken.tkData.tkBoolean = L' ';
////                     else
////                         lpYYRes->tkToken.tkData.tkBoolean = 0;
//// ////////////////////lpYYRes->tkToken.tkCharIndex       =           // Already set
////
////                 } // End IF/ELSE
////
////                 break;
////
////             case TKT_VARIMMED:
////                 // Convert to 0 or blank
////                 if (lpYYRes->tkToken.tkFlags.ImmType EQ IMMTYPE_CHAR)
////                     lpYYRes->tkToken.tkData.tkChar    = L' ';
////                 else
////                     lpYYRes->tkToken.tkData.tkBoolean = 0;
////                 break;
////
////             case TKT_VARARRAY:
////                 // Get the global memory handle
////                 hGlbData = lpYYRes->tkToken.tkData.tkGlbData;
////
////                 // tkData is a valid HGLOBAL variable array
////                 Assert (IsGlbTypeVarDir (hGlbData));
////
////                 // Make the prototype
////                 hGlbData = MakePrototype_EM (ClrPtrTypeDirGlb (hGlbData), // Proto arg handle
////                                              lptkFunc,      // Ptr to function token
////                                              MP_CHARS);     // CHARs allowed
////                 // Save the prototype handle
////                 lpYYRes->tkToken.tkData.tkGlbData = MakeGlbTypeGlb (hGlbData);
////
////                 break;
////
////             defstop
////                 break;
////         } // End SWITCH
////
////         return lpYYRes;
////     } // End IF
} // End PrimProtoFnLeftTack_EM
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
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

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
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    HGLOBAL   hGlbData;
    LPYYSTYPE lpYYRes;

    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_RIGHTTACK);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken = *lptkLftArg;

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
////////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDir (hGlbData);
////////////////lpYYRes->tkToken.tkCharIndex       =           // Already set
            } else
            {
                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkFlags.ImmType   = lpYYRes->tkToken.tkData.tkSym->stFlags.ImmType;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkLongest  = lpYYRes->tkToken.tkData.tkSym->stData.stLongest;
////////////////lpYYRes->tkToken.tkCharIndex       =           // Already set
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
} // End PrimFnLeftTack_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_ltack.c
//***************************************************************************
