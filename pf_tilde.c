//***************************************************************************
//  NARS2000 -- Primitive Function -- Tilde
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

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


#ifndef PROTO
PRIMSPEC PrimSpecTilde =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecTildeStorageTypeMon,
    NULL,   // &PrimFnMonTildeAPA_EM, -- Can't happen w/Tilde

    &PrimFnMonTildeBisB,
    &PrimFnMonTildeBisI,
    NULL,   // &PrimFnMonTildeBisF, -- Can't happen w/Tilde

////               IisB,     // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonTildeIisI, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeIisF, -- Can't happen w/Tilde

////               FisB,     // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonTildeFisI, -- Can't happen w/Tilde
    &PrimFnMonTildeFisF,

    // Dyadic functions
    NULL,   // &PrimFnDyd_EM_YY, -- Can't happen w/Tilde
    NULL,   // &PrimSpecTildeStorageTypeDyd, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeAPA_EM, -- Can't happen w/Tilde

    NULL,   // &PrimFnDydTildeBisBvB, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisIvI, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisFvF, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisCvC, -- Can't happen w/Tilde

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydTildeIisIvI,
    NULL,   // &PrimFnDydTildeIisFvF, -- Can't happen w/Tilde

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydTildeFisIvI, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeFisFvF, -- Can't happen w/Tilde

    &PrimFnMonTildeB64isB64,
    &PrimFnMonTildeB32isB32,
    &PrimFnMonTildeB16isB16,
    &PrimFnMonTildeB08isB08,

    NULL,   // &PrimFnDydTildeB64isB64vB64, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeB32isB32vB32, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeB16isB16vB16, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeB08isB08vB08, -- Can't happen w/Tilde
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecTilde};
#endif


//***************************************************************************
//  $PrimFnTilde_EM_YY
//
//  Primitive function for monadic and dyadic Tilde ("not" and "without")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnTilde_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnTilde_EM_YY
    (LPTOKEN lptkLftArg,             // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_TILDE
         || lptkFunc->tkData.tkChar EQ UTF16_TILDE2);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return PrimFnDydTilde_EM_YY           (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnTilde_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnTilde_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Tilde
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnTilde_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnTilde_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // If the left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return PrimProtoFnScalar_EM_YY
               (lptkLftArg,         // Ptr to left arg token
                lptkFunc,           // Ptr to function token
                lptkRhtArg,         // Ptr to right arg token
                lptkAxis);          // Ptr to axis token (may be NULL)
    else
    {
        //***************************************************************
        // Called dyadically
        //***************************************************************

        // Convert to a prototype
        return PrimProtoFnMixed_EM_YY (&PrimFnTilde_EM_YY,  // Ptr to primitive function routine
                                        lptkLftArg,         // Ptr to left arg token
                                        lptkFunc,           // Ptr to function token
                                        lptkRhtArg,         // Ptr to right arg token
                                        lptkAxis);          // Ptr to axis token (may be NULL)
    } // End IF/ELSE
} // End PrimProtoFnTilde_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimIdentFnTilde_EM_YY
//
//  Generate an identity element for the primitive function dyadic Tilde
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimIdentFnTilde_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimIdentFnTilde_EM_YY
    (LPTOKEN lptkRhtOrig,           // Ptr to original right arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLRANK      aplRankRht;        // Right arg rank
    LPPL_YYSTYPE lpYYRes;           // Ptr to result

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig NE NULL);
    Assert (lptkFunc    NE NULL);
    Assert (lptkRhtArg  NE NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // The (left) identity function for dyadic Tilde
    //   (L~R) ("without") is
    //   {zilde}.

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, NULL, NULL, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbZilde);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimIdentFnTilde_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecTildeStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecTildeStorageTypeMon
    (APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht,
     LPTOKEN    lptkFunc)

{
    APLSTYPE aplTypeRes;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsEmpty (aplNELMRht) && IsSimpleChar (*lpaplTypeRht))
        *lpaplTypeRht = ARRAY_BOOL;

    if (IsSimpleChar (*lpaplTypeRht)
     || *lpaplTypeRht EQ ARRAY_LIST)
        return ARRAY_ERROR;

    // The storage type of the result is
    //   the same as that of the right arg
    //   except SimpleFlt stays the same
    //   and SimpleNum goes to BOOL
    if (IsSimpleNum (*lpaplTypeRht)
     && !IsSimpleFlt (*lpaplTypeRht))
        aplTypeRes = ARRAY_BOOL;
    else
        aplTypeRes = *lpaplTypeRht;

    return aplTypeRes;
} // End PrimSpecTildeStorageTypeMon


//***************************************************************************
//  $PrimFnMonTildeBisB
//
//  Primitive scalar function monadic Tilde:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonTildeBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return !aplBooleanRht;
} // End PrimFnMonTildeBisB


//***************************************************************************
//  $PrimFnMonTildeB64isB64
//
//  Primitive scalar function monadic Tilde:  B64 {is} fn B64
//***************************************************************************

APLB64 PrimFnMonTildeB64isB64
    (APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~aplBooleanRht;
} // End PrimFnMonTildeB64isB64


//***************************************************************************
//  $PrimFnMonTildeB32isB32
//
//  Primitive scalar function monadic Tilde:  B32 {is} fn B32
//***************************************************************************

APLB32 PrimFnMonTildeB32isB32
    (APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~aplBooleanRht;
} // End PrimFnMonTildeB32isB32


//***************************************************************************
//  $PrimFnMonTildeB16isB16
//
//  Primitive scalar function monadic Tilde:  B16 {is} fn B16
//***************************************************************************

APLB16 PrimFnMonTildeB16isB16
    (APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~aplBooleanRht;
} // End PrimFnMonTildeB16isB16


//***************************************************************************
//  $PrimFnMonTildeB08isB08
//
//  Primitive scalar function monadic Tilde:  B08 {is} fn B08
//***************************************************************************

APLB08 PrimFnMonTildeB08isB08
    (APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~aplBooleanRht;
} // End PrimFnMonTildeB08isB08


//***************************************************************************
//  $PrimFnMonTildeBisI
//
//  Primitive scalar function monadic Tilde:  B {is} fn I
//***************************************************************************

APLBOOL PrimFnMonTildeBisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    if (!IsBooleanValue (aplIntegerRht))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    return !(APLBOOL) aplIntegerRht;
} // End PrimFnMonTildeBisI


//***************************************************************************
//  $PrimFnMonTildeFisF
//
//  Primitive scalar function monadic Tilde:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonTildeFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // If the float is between 0 and 1, ...
    if (0.0 <= aplFloatRht
     &&        aplFloatRht <= 1.0)
        // Return the complement in 1.0
        return 1.0 - aplFloatRht;

    RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return 0.0;             // To keep the compiler happy
} // End PrimFnMonTildeFisF


//***************************************************************************
//  $PrimFnDydTilde_EM_YY
//
//  Primitive function for dyadic Tilde ("without")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTilde_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydTilde_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLRANK      aplRankLft;        // Left arg rank
    LPPL_YYSTYPE lpYYRes = NULL,    // Ptr to the result
                 lpYYRes1;          // Ptr to temporary result
    TOKEN        tkFunc = {0};      // Function token

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the left arg
    AttrsOfToken (lptkLftArg, NULL, NULL, &aplRankLft, NULL);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Setup the Epsilon function token
    tkFunc.tkFlags.TknType   = TKT_FCNIMMED;
    tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////tkFunc.tkFlags.NoDisplay = FALSE;           // Already zero from = {0}
    tkFunc.tkData.tkChar     = UTF16_EPSILON;
    tkFunc.tkCharIndex       = lptkFunc->tkCharIndex;

    // Execute L {epsilon} R
    lpYYRes =
      PrimFnDydEpsilon_EM_YY (lptkLftArg,       // Ptr to left arg token
                             &tkFunc,           // Ptr to function token
                              lptkRhtArg,       // Ptr to right arg token
                              NULL);            // Ptr to axis token (may be NULL)
    // If it failed, ...
    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;

    // Setup the Tilde function token
////tkFunc.tkFlags.TknType   = TKT_FCNIMMED;    // Already set above
////tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN; // Already set above
////tkFunc.tkFlags.NoDisplay = FALSE;           // Already zero from = {0}
    tkFunc.tkData.tkChar     = UTF16_TILDE;
////tkFunc.tkCharIndex       = lptkFunc->tkCharIndex;   // Already set above

    // Execute ~Z
    lpYYRes1 =
      PrimFnMon_EM_YY (&tkFunc,                 // Ptr to function token
                       &lpYYRes->tkToken,       // Ptr to right arg token
                        NULL,                   // Ptr to axis token (may be NULL)
                        lpPrimSpec);            // Ptr to local PRIMSPEC
    // We no longer need this storage
    FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

    // If it failed, ...
    if (lpYYRes1 EQ NULL)
        goto ERROR_EXIT;

    // Setup the Slash function token
////tkFunc.tkFlags.TknType   = TKT_FCNIMMED;    // Already set above
////tkFunc.tkFlags.ImmType   = IMMTYPE_PRIMFCN; // Already set above
////tkFunc.tkFlags.NoDisplay = FALSE;           // Already zero from = {0}
    tkFunc.tkData.tkChar     = UTF16_SLASH;
////tkFunc.tkCharIndex       = lptkFunc->tkCharIndex;   // Already set above

    // Execute Z/L
    lpYYRes =
      PrimFnDydSlash_EM_YY (&lpYYRes1->tkToken, // Ptr to left arg token
                            &tkFunc,            // Ptr to function token
                             lptkLftArg,        // Ptr to right arg token
                             NULL);             // Ptr to axis token (may be NULL)
    // We no longer need this storage
    FreeResult (lpYYRes1); YYFree (lpYYRes1); lpYYRes1 = NULL;

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return lpYYRes;
} // End PrimFnDydTilde_EM_YY


//***************************************************************************
//  End of File: pf_tilde.c
//***************************************************************************
