//***************************************************************************
//  NARS2000 -- System Function -- Quad TYPE
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2009 Sudley Place Software

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
#include "headers.h"


//***************************************************************************
//  $SysFnTYPE_EM_YY
//
//  System function:  []TYPE -- Prototype
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnTYPE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnTYPE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonTYPE_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydTYPE_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnTYPE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonTYPE_EM_YY
//
//  Monadic []TYPE -- Prototype
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonTYPE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonTYPE_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    HGLOBAL      hGlbRht,           // Right arg global memory handle
                 hSymGlbRes;        // Result    ...
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
                hGlbRht = lptkRhtArg->tkData.tkSym->stData.stGlbData;

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
                    lpYYRes->tkToken.tkData.tkBoolean = FALSE;

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
                    lpYYRes->tkToken.tkData.tkBoolean = FALSE;

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

        case TKT_VARARRAY:
            hGlbRht = lptkRhtArg->tkData.tkGlbData;

            break;      // Continue with HGLOBAL case

        defstop
            break;
    } // End SWITCH

    // HGLOBAL case

    // tk/stData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir_PTB (hGlbRht));

    // Make the prototype
    hSymGlbRes =
      MakeMonPrototype_EM_PTB (hGlbRht,     // Proto arg handle
                               lptkFunc,    // Ptr to function token
                               MP_CHARS);   // CHARs allowed
    if (!hSymGlbRes)
        return NULL;

    Assert (GetPtrTypeDir (hSymGlbRes) EQ PTRTYPE_HGLOBAL);

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = hSymGlbRes;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnMonTYPE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydTYPE_EM_YY
//
//  Dyadic []TYPE -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydTYPE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydTYPE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc);
} // End SysFnDydTYPE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_type.c
//***************************************************************************
