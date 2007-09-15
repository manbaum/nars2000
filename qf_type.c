//***************************************************************************
//  NARS2000 -- System Function -- Quad TYPE
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


//***************************************************************************
//  $SysFnTYPE_EM
//
//  System function:  []TYPE -- Prototype
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnTYPE_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnTYPE_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
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
        return SysFnMonTYPE_EM (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydTYPE_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnTYPE_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonTYPE_EM
//
//  Monadic []TYPE -- Prototype
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonTYPE_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonTYPE_EM
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    HGLOBAL      hGlbData,          //
                 hGlbRes;           // Result global memory handle
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Split cases based upon the token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, it's an HGLOBAL
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                break;      // Continue with HGLOBAL case
            } // End IF

            // Handle the immediate case

            // stData is an immediate
            Assert (lptkRhtArg->tkData.tkSym->stFlags.Imm);

            lpYYRes->tkToken.tkFlags.TknType = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType = lptkRhtArg->tkData.tkSym->stFlags.ImmType;

            // Split cases based upon the token's immediate type
            switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    lpYYRes->tkToken.tkData.tkBoolean = 0;

                    return lpYYRes;

                case IMMTYPE_INT:
                    lpYYRes->tkToken.tkData.tkInteger = 0;

                    return lpYYRes;

                case IMMTYPE_FLOAT:
                    lpYYRes->tkToken.tkData.tkFloat   = 0;

                    return lpYYRes;

                case IMMTYPE_CHAR:
                    lpYYRes->tkToken.tkData.tkChar    = L' ';

                    return lpYYRes;

                defstop
                    return NULL;
            } // End SWITCH

            DbgStop ();         // We should never get here

        case TKT_VARIMMED:
            lpYYRes->tkToken.tkFlags.TknType = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType = lptkRhtArg->tkFlags.ImmType;

            // Split cases based upon the token's immediate type
            switch (lptkRhtArg->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    lpYYRes->tkToken.tkData.tkBoolean = 0;

                    return lpYYRes;

                case IMMTYPE_INT:
                    lpYYRes->tkToken.tkData.tkInteger = 0;

                    return lpYYRes;

                case IMMTYPE_FLOAT:
                    lpYYRes->tkToken.tkData.tkFloat   = 0;

                    return lpYYRes;

                case IMMTYPE_CHAR:
                    lpYYRes->tkToken.tkData.tkChar    = L' ';

                    return lpYYRes;

                defstop
                    return NULL;
            } // End SWITCH

            DbgStop ();         // We should never get here

        case TKT_LISTPAR:
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkFunc);
            YYFree (lpYYRes); lpYYRes = NULL; return NULL;

        case TKT_VARARRAY:
            hGlbData = lptkRhtArg->tkData.tkGlbData;

            break;      // Continue with HGLOBAL case

        defstop
            break;
    } // End SWITCH

    // HGLOBAL case

    // tk/stData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (hGlbData));

    // Make the prototype
    hGlbRes = MakeMonPrototype_EM (ClrPtrTypeDirGlb (hGlbData),// Proto arg handle
                                   lptkFunc,        // Ptr to function token
                                   MP_CHARS);       // CHARs allowed
    if (!hGlbRes)
        return NULL;

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnMonTYPE_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydTYPE_EM
//
//  Dyadic []TYPE -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydTYPE_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydTYPE_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnSyntaxError_EM (lptkFunc);
} // End SysFnDydTYPE_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_type.c
//***************************************************************************
