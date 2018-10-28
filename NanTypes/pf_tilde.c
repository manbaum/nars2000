//***************************************************************************
//  NARS2000 -- Primitive Function -- Tilde
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2018 Sudley Place Software

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

    // Monadic Boolean result functions
    &PrimFnMonTildeBisB,
    &PrimFnMonTildeBisI,
    NULL,   // &PrimFnMonTildeBisF, -- Can't happen w/Tilde

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonTildeIisI, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeIisF, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeFisI, -- Can't happen w/Tilde
    &PrimFnMonTildeFisF,
    &PrimFnMonTildeRisR,
    &PrimFnMonTildeVisR,
    &PrimFnMonTildeVisV,

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonTildeHC2IisHC2I, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC2IisHC2F, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC2FisHC2I, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC2FisHC2F, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC2RisHC2R, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC2VisHC2R, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC2VisHC2V, -- Can't happen w/Tilde

    NULL,   // &PrimFnMonTildeHC4IisHC4I, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC4IisHC4F, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC4FisHC4I, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC4FisHC4F, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC4RisHC4R, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC4VisHC4R, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC4VisHC4V, -- Can't happen w/Tilde

    NULL,   // &PrimFnMonTildeHC8IisHC8I, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC8IisHC8F, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC8FisHC8I, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC8FisHC8F, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC8RisHC8R, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC8VisHC8R, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeHC8VisHC8V, -- Can't happen w/Tilde

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonTildeFisHC2I, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeFisHC2F, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeVisHC2R, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeVisHC2V, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeFisHC4I, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeFisHC4F, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeVisHC4R, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeVisHC4V, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeFisHC8I, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeFisHC8F, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeVisHC8R, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeVisHC8V, -- Can't happen w/Tilde

    // Monadic BAxF result BAxF arg functions (indexable)
    NULL,   // &PrimFnMonTildeBA1FisBA1F,
    NULL,   // &PrimFnMonTildeBA2FisBA2F,
    NULL,   // &PrimFnMonTildeBA4FisBA4F,
    NULL,   // &PrimFnMonTildeBA8FisBA8F,
    NULL,   // &PrimFnMonTildeARBisBA1F,
    NULL,   // &PrimFnMonTildeARBisBA2F,
    NULL,   // &PrimFnMonTildeARBisBA4F,
    NULL,   // &PrimFnMonTildeARBisBA8F,

    // Dyadic functions
    NULL,   // &PrimFnDyd_EM_YY, -- Dyadic Tilde not scalar
    NULL,   // &PrimSpecTildeStorageTypeDyd, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeAPA_EM, -- Dyadic Tilde not scalar

    // Dyadic Boolean result functions
    NULL,   // &PrimFnDydTildeBisBvB, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeBisIvI, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeBisFvF, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeBisCvC, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeBisHvH, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeBisVvV, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeBisRvR, -- Dyadic Tilde not scalar

    NULL,   // &PrimFnDydTildeBisHC2IvHC2I, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisHC2FvHC2F, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisHC2RvHC2R, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisHC2VvHC2V, -- Can't happen w/Tilde

    NULL,   // &PrimFnDydTildeBisHC4IvHC4I, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisHC4FvHC4F, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisHC4RvHC4R, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisHC4VvHC4V, -- Can't happen w/Tilde

    NULL,   // &PrimFnDydTildeBisHC8IvHC8I, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisHC8FvHC8F, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisHC8RvHC8R, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisHC8VvHC8V, -- Can't happen w/Tilde

    // Dyadic Boolean result functions with ARBs (indexable)
    NULL,   // &PrimFnDydTildeBisBA1FvBA1F, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisBA2FvBA2F, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisBA4FvBA4F, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisBA8FvBA8F, -- Can't happen w/Tilde

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydTildeIisIvI, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeIisFvF, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeFisIvI, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeFisFvF, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeRisRvR, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeVisRvR, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeVisVvV, -- Dyadic Tilde not scalar

    NULL,   // &PrimFnDydTildeHC2IisHC2IvHC2I, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC2IisHC2FvHC2F, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC2FisHC2IvHC2I, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC2FisHC2FvHC2F, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC2RisHC2RvHC2R, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC2VisHC2RvHC2R, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC2VisHC2VvHC2V, -- Dyadic Tilde not scalar

    NULL,   // &PrimFnDydTildeHC4IisHC4IvHC4I, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC4IisHC4FvHC4F, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC4FisHC4IvHC4I, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC4FisHC4FvHC4F, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC4RisHC4RvHC4R, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC4VisHC4RvHC4R, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC4VisHC4VvHC4V, -- Dyadic Tilde not scalar

    NULL,   // &PrimFnDydTildeHC8IisHC8IvHC8I, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC8IisHC8FvHC8F, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC8FisHC8IvHC8I, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC8FisHC8FvHC8F, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC8RisHC8RvHC8R, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC8VisHC8RvHC8R, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeHC8VisHC8VvHC8V, -- Dyadic Tilde not scalar

    // Dyadic BAxF result functions (indexable)
    NULL,   // &PrimFnDydTildeBA1FisBA1FvBA1F, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeBA2FisBA2FvBA2F, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeBA4FisBA4FvBA4F, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeBA8FisBA8FvBA8F, -- Dyadic Tilde not scalar

    // Monadic Boolean chunk functions
    &PrimFnMonTildeB64isB64,
    &PrimFnMonTildeB32isB32,
    &PrimFnMonTildeB16isB16,
    &PrimFnMonTildeB08isB08,

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnDydTildeB64isB64vB64, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeB32isB32vB32, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeB16isB16vB16, -- Dyadic Tilde not scalar
    NULL,   // &PrimFnDydTildeB08isB08vB08, -- Dyadic Tilde not scalar

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecTilde};
#endif


//***************************************************************************
//  $PrimFnTilde_EM_YY
//
//  Primitive function for monadic and dyadic Tilde ("not" and "without")
//***************************************************************************

LPPL_YYSTYPE PrimFnTilde_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharTilde (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return PrimFnDydTilde_EM_YY           (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnTilde_EM_YY


//***************************************************************************
//  $PrimProtoFnTilde_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Tilde
//***************************************************************************

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


//***************************************************************************
//  $PrimIdentFnTilde_EM_YY
//
//  Generate an identity element for the primitive function dyadic Tilde
//***************************************************************************

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
    if (IsCharEmpty (*lpaplTypeRht, aplNELMRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // The storage type of the result is
    //   the same as that of the right arg
    //   except SimpleFlt stays the same
    //   and SimpleNum goes to BOOL
    aplTypeRes = *lpaplTypeRht;

    // Is the result HC?
    if (IsHCAny (aplTypeRes))
        // Tell the caller to demote the data to HC dimension 1
        return ARRAY_REALONLY;

    // Split cases based upon the storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            aplTypeRes = ARRAY_BOOL;

            break;

        case ARRAY_FLOAT:
        case ARRAY_RAT:
        case ARRAY_VFP:
        case ARRAY_ARB:
        case ARRAY_NESTED:
            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
            aplTypeRes = ARRAY_ERROR;

            break;

        defstop
            break;
    } // End SWITCH

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
//  $PrimFnMonTildeFisF
//
//  Primitive scalar function monadic Tilde:  F {is} fn F
//***************************************************************************

void PrimFnMonTildeFisF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLINT aplIntegerRht;
    UBOOL  bRet;

    // If the float is between 0 and 1, ...
    if (0.0 <= lpatRht->aplFloat
     &&        lpatRht->aplFloat <= 1.0)
        // Return the complement in 1.0
        lpMemRes[uRes] = 1.0 - lpatRht->aplFloat;
    else
    {
        // Attempt to convert the float to an integer using System []CT
        aplIntegerRht = FloatToAplint_SCT (lpatRht->aplFloat, &bRet);
        if (!bRet
         || !IsBooleanValue (aplIntegerRht))
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
        else
            lpMemRes[uRes] = (APLFLOAT) (1 - aplIntegerRht);
    } // End IF/ELSE
} // End PrimFnMonTildeFisF


//***************************************************************************
//  $PrimFnMonTildeRisR
//
//  Primitive scalar function monadic Tilde:  R {is} fn R
//***************************************************************************

void PrimFnMonTildeRisR
    (LPAPLRAT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    if (IsMpq0 (&lpatRht->aplRat))
        mpq_init_set_ui (&lpMemRes[uRes], 1, 1);
    else
    if (IsMpq1 (&lpatRht->aplRat))
        mpq_init_set_ui (&lpMemRes[uRes], 0, 1);
    else
    if (mpq_cmp_ui (&lpatRht->aplRat, 0, 1) > 0
     && mpq_cmp_ui (&lpatRht->aplRat, 1, 1) < 0)
    {
        mpq_init_set_ui (&lpMemRes[uRes], 1, 1);
        mpq_sub         (&lpMemRes[uRes], &lpMemRes[uRes], &lpatRht->aplRat);
    } else
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
} // End PrimFnMonTildeRisR


//***************************************************************************
//  $PrimFnMonTildeVisR
//
//  Primitive scalar function monadic Tilde:  V {is} fn R
//***************************************************************************

void PrimFnMonTildeVisR
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_RAT][ARRAY_VFP]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnMonTildeVisV (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc1v_clear (&atRht.aplHC1V);
} // End PrimFnMonTildeVisR


//***************************************************************************
//  $PrimFnMonTildeVisV
//
//  Primitive scalar function monadic Tilde:  V {is} fn V
//***************************************************************************

void PrimFnMonTildeVisV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    UBOOL  bRet;
    APLINT aplIntegerRht;

    if (mpfr_cmp_ui (&lpatRht->aplVfp, 0) >= 0
     && mpfr_cmp_ui (&lpatRht->aplVfp, 1) <= 0)
    {
        mpfr_init_set_ui (&lpMemRes[uRes], 1, MPFR_RNDN);
        mpfr_sub         (&lpMemRes[uRes], &lpMemRes[uRes], &lpatRht->aplVfp, MPFR_RNDN);
    } else
    {
        // Attempt to convert the float to an integer using System []CT
        aplIntegerRht = mpfr_get_sctsx (&lpatRht->aplVfp, &bRet);
        if (!bRet
         || !IsBooleanValue (aplIntegerRht))
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
        else
            mpfr_init_set_ui (&lpMemRes[uRes], 1 - (UINT) aplIntegerRht, MPFR_RNDN);
    } // End IF/ELSE
} // End PrimFnMonTildeVisV


//***************************************************************************
//  $PrimFnMonTildeBA1FisBA1F
//
//  Primitive scalar function monadic Tilde:  BA1F {is} fn BA1F
//***************************************************************************

void PrimFnMonTildeBA1FisBA1F
    (LPAPLARB   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    UBOOL  bRet;
    APLINT aplIntegerRht;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    if (arb_cmp_ui (&lpatRht->aplArb, 0) >= 0
     && arb_cmp_ui (&lpatRht->aplArb, 1) <= 0)
    {
        arb_init_set_sx (&lpMemRes[uRes], 1);
        arb_sub         (&lpMemRes[uRes], &lpMemRes[uRes], &lpatRht->aplArb, prec);
    } else
    {
        // Attempt to convert the float to an integer using System []CT
        aplIntegerRht = arb_get_sctsx (&lpatRht->aplArb, &bRet);
        if (!bRet
         || !IsBooleanValue (aplIntegerRht))
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
        else
            arb_init_set_sx (&lpMemRes[uRes], 1 - (UINT) aplIntegerRht);
    } // End IF/ELSE
} // End PrimFnMonTildeBA1FisBA1F


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
//  $PrimFnDydTilde_EM_YY
//
//  Primitive function for dyadic Tilde ("without")
//***************************************************************************

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
