//***************************************************************************
//  NARS2000 -- Primitive Function -- Tilde
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


#ifndef PROTO
PRIMSPEC PrimSpecTilde =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecTildeStorageTypeMon,
    NULL,   // &PrimFnMonTildeAPA_EM, -- Can't happen w/Tilde

    &PrimFnMonTildeBisB,
    &PrimFnMonTildeBisI,
    &PrimFnMonTildeBisF,

////               IisB,     // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonTildeIisI, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeIisF, -- Can't happen w/Tilde

////               FisB,     // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonTildeFisI, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeFisF, -- can't happen w/Tilde

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
    Assert (lptkFunc->tkData.tkChar EQ UTF16_TILDE);

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
    //   except SimpleNum goes to BOOL
    if (IsSimpleNum (*lpaplTypeRht))
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
//  $PrimFnMonTildeBisF
//
//  Primitive scalar function monadic Tilde:  B {is} fn F
//***************************************************************************

APLBOOL PrimFnMonTildeBisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT aplIntRht;
    UBOOL  bRet;

    // Attempt to convert the float to an integer using System CT
    aplIntRht = FloatToAplint_SCT (aplFloatRht, &bRet);

    if ((!bRet) || !IsBooleanValue (aplIntRht))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    return !(APLBOOL) aplIntRht;
} // End PrimFnMonTildeBisF


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
    HGLOBAL      hGlbMFO;           // Magic function/operator global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

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

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get the magic function/operator global memory handle
    hGlbMFO = lpMemPTD->hGlbMFO_DydTilde;

    //  Return the elements in L not in R.
    //  Use an internal magic function/operator.
    lpYYRes =
      ExecuteMagicFunction_EM_YY (lptkLftArg,   // Ptr to left arg token
                                  lptkFunc,     // Ptr to function token
                                  NULL,         // Ptr to function strand
                                  lptkRhtArg,   // Ptr to right arg token
                                  lptkAxis,     // Ptr to axis token
                                  hGlbMFO,      // Magic function/operator global memory handle
                                  NULL,         // Ptr to HSHTAB struc (may be NULL)
                                  LINENUM_ONE); // Starting line # type (see LINE_NUMS)
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
//  Magic function/operator for dyadic Tilde
//
//  Dyadic Tilde -- Without
//
//  Return the elements in L not in R.
//***************************************************************************

static APLCHAR Header[] =
  L"Z" $IS L"L " MFON_DydTilde L" R";

static APLCHAR Line1[] =
  L"Z" $IS L"(~L" $EPSILON L"R)/L";

static LPAPLCHAR Body[] =
{Line1,
};

MAGIC_FCNOPR MFO_DydTilde =
{Header,
 Body,
 countof (Body),
};


//***************************************************************************
//  End of File: pf_tilde.c
//***************************************************************************
