//***************************************************************************
//  NARS2000 -- Primitive Function -- Times
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2008 Sudley Place Software

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
PRIMSPEC PrimSpecTimes =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecTimesStorageTypeMon,
    NULL,   // &PrimFnMonTimesAPA_EM, -- Can't happen w/Times

    &PrimFnMonTimesBisB,
    NULL,   // &PrimFnMonTimesBisI, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesBisF, -- Can't happen w/Times

////               IisB,     // Handled via type promotion (to IisI)
    &PrimFnMonTimesIisI,
    &PrimFnMonTimesIisF,

////               FisB,     // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonTimesFisI, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesFisF, -- Can't happen w/Times

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecTimesStorageTypeDyd,
    &PrimFnDydTimesAPA_EM,

    &PrimFnDydUpCaretBisBvB,
    NULL,   // &PrimFnDydTimesBisIvI, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisFvF, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisCvC, -- Can't happen w/Times

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydTimesIisIvI,
    NULL,   // &PrimFnDydTimesIisFvF, -- Can't happen w/Times

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydTimesFisIvI,
    &PrimFnDydTimesFisFvF,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecTimes};
#endif


//***************************************************************************
//  $PrimFnTimes_EM_YY
//
//  Primitive function for monadic and dyadic Times ("signum" and "multiplication")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnTimes_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnTimes_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_TIMES);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnTimes_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecTimesStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecTimesStorageTypeMon
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
    aplTypeRes = *lpaplTypeRht;

    // Except that FLOAT and APA become INT
    if (IsSimpleFlt (aplTypeRes)
     || IsSimpleAPA (aplTypeRes))
        return ARRAY_INT;

    return aplTypeRes;
} // End PrimSpecTimesStorageTypeMon


//***************************************************************************
//  $PrimFnMonTimesBisB
//
//  Primitive scalar function monadic Times:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonTimesBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonTimesBisB


//***************************************************************************
//  $PrimFnMonTimesIisI
//
//  Primitive scalar function monadic Times:  I {is} fn I
//***************************************************************************

APLINT PrimFnMonTimesIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    if (aplIntegerRht < 0)
        return -1;
    else
    if (aplIntegerRht EQ 0)
        return  0;
    else
        return  1;
} // End PrimFnMonTimesIisI


//***************************************************************************
//  $PrimFnMonTimesIisF
//
//  Primitive scalar function monadic Times:  I {is} fn F
//***************************************************************************

APLINT PrimFnMonTimesIisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    if (aplFloatRht < 0)
        return -1;
    else
    if (aplFloatRht EQ 0)
        return  0;
    else
        return  1;
} // End PrimFnMonTimesIisF


//***************************************************************************
//  $PrimSpecTimesStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecTimesStorageTypeDyd
    (APLNELM    aplNELMLft,
     LPAPLSTYPE lpaplTypeLft,
     LPTOKEN    lptkFunc,
     APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht)

{
    APLSTYPE aplTypeRes;

    // In case the left arg is an empty char,
    //   change its type to BOOL
    if (IsEmpty (aplNELMLft) && IsSimpleChar (*lpaplTypeLft))
        *lpaplTypeLft = ARRAY_BOOL;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsEmpty (aplNELMRht) && IsSimpleChar (*lpaplTypeRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // Calculate the storage type of the result
    aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // Special case multiplication with APA
    if (aplTypeRes EQ ARRAY_INT                            // Res = INT
     && (!IsSingleton (aplNELMLft) || !IsSingleton (aplNELMRht))    // Not both singletons
     && ((IsSingleton (aplNELMLft) && IsSimpleAPA (*lpaplTypeRht))   // Non-singleton is APA
      || (IsSingleton (aplNELMRht) && IsSimpleAPA (*lpaplTypeLft)))) // ...
        aplTypeRes = ARRAY_APA;

    return aplTypeRes;
} // End PrimSpecTimesStorageTypeDyd


//***************************************************************************
//  $PrimFnDydTimesIisIvI
//
//  Primitive scalar function dyadic Times:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydTimesIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return imul64 (aplIntegerLft, aplIntegerRht);
} // End PrimFnDydTimesIisIvI


//***************************************************************************
//  $PrimFnDydTimesFisIvI
//
//  Primitive scalar function dyadic Times:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydTimesFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    UBOOL  bRet = TRUE;
    APLINT aplRes;

    aplRes = _imul64 (aplIntegerLft, aplIntegerRht, &bRet);
    if (bRet)
        return (APLFLOAT) aplRes;

    return ((APLFLOAT) aplIntegerLft) * (APLFLOAT) aplIntegerRht;
} // End PrimFnDydTimesFisIvI


//***************************************************************************
//  $PrimFnDydTimesFisFvF
//
//  Primitive scalar function dyadic Times:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydTimesFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  0 {times} _
    if ((aplFloatLft EQ 0
      && aplFloatRht EQ PosInfinity)
     || (aplFloatLft EQ PosInfinity
      && aplFloatRht EQ 0))
        return TranslateQuadICIndex (ICNDX_0MULPi);

    // Check for indeterminates:  0 {times} {neg}_
    if ((aplFloatLft EQ 0
      && aplFloatRht EQ NegInfinity)
     || (aplFloatLft EQ NegInfinity
      && aplFloatRht EQ 0))
        return TranslateQuadICIndex (ICNDX_0MULNi);

    return (aplFloatLft * aplFloatRht);
} // End PrimFnDydTimesFisFvF


//***************************************************************************
//  $PrimFnDydTimesAPA_EM
//
//  Dyadic Times, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesAPA_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydTimesAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // Ptr to result token (may be NULL)

     LPTOKEN      lptkFunc,         // Ptr to function token

     HGLOBAL      hGlbLft,          // HGLOBAL of left arg (may be NULL if simple)
     HGLOBAL      hGlbRht,          // ...        right ...
     HGLOBAL     *lphGlbRes,        // Ptr to result handle

     APLRANK      aplRankLft,       // Left arg rank
     APLRANK      aplRankRht,       // Right ...

     APLNELM      aplNELMLft,       // Left arg NELM
     APLNELM      aplNELMRht,       // Right ...

     APLINT       aplInteger,       // The integer from the simple side
     LPPRIMSPEC   lpPrimSpec)       // Ptr to local PRIMSPEC

{
    APLRANK aplRankRes;             // Result rank
    LPVOID  lpMemRes;               // Ptr to result global memory
    UBOOL   bRet = FALSE;           // TRUE iff the result is valid

    DBGENTER;

    //***************************************************************
    // The result is an APA, one of the args is a simple singleton,
    //   the other an APA
    //***************************************************************

    // Axis may be anything

    // Get a ptr to each arg's header
    //   and the value of the singleton
    // Also, in case we're doing (1 1 1{rho}1) fn {iota}4
    //   set the rank of the result to the rank of
    //   the APA.  The rank passed into this function
    //   is the larger of the left and right ranks,
    //   which for the above example, is wrong.

    // Split cases based upon the arg's NELM
    //   (one of the arg's must be a singleton)
    if (!IsSingleton (aplNELMLft))
    {
        *lphGlbRes = CopyArray_EM (hGlbLft, lptkFunc);
        aplRankRes = aplRankLft;
    } else
    if (!IsSingleton (aplNELMRht))
    {
        *lphGlbRes = CopyArray_EM (hGlbRht, lptkFunc);
        aplRankRes = aplRankRht;
    } else
        DbgStop ();     // We should never get here

    if (!*lphGlbRes)
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (*lphGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

#define lpAPA       ((LPAPLAPA) lpMemRes)

    // Multiply the singleton's value to the result offset and multiplier
    lpAPA->Off *= aplInteger;
    lpAPA->Mul *= aplInteger;

#undef  lpAPA

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;

    // Fill in the result token
    if (lpYYRes)
    {
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
    } // End IF

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    DBGLEAVE;

    return bRet;
} // End PrimFnDydTimesAPA_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_times.c
//***************************************************************************
