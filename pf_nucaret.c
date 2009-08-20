//***************************************************************************
//  NARS2000 -- Primitive Function -- UpCaretTilde
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
PRIMSPEC PrimSpecUpCaretTilde = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecUpCaretTildeStorageTypeMon, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeAPA_EM, -- Can't happen w/UpCaretTilde

    NULL,   // &PrimFnMonUpCaretTildeBisB, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeBisI, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeBisF, -- Can't happen w/UpCaretTilde

////                      IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonUpCaretTildeIisI, -- Can't happend w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeIisF, -- Can't happend w/UpCaretTilde

////                      FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonUpCaretTildeFisI, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeFisF, -- Can't happen w/UpCaretTilde

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecUpCaretTildeStorageTypeDyd,
    NULL,   // &PrimFnDydUpCaretTildeAPA_EM, -- Can't happen w/UpCaretTilde

    &PrimFnDydUpCaretTildeBisBvB,
    &PrimFnDydUpCaretTildeBisIvI,
    &PrimFnDydUpCaretTildeBisFvF,
    NULL,   // &PrimFnDydUpCaretTildeBisCvC, -- Can't happen w/UpCaretTilde

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydUpCaretTildeIisIvI, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeIisFvF, -- Can't happen w/UpCaretTilde

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydUpCaretTildeFisIvI, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeFisFvF, -- Can't happen w/UpCaretTilde
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecUpCaretTilde};
#endif


//***************************************************************************
//  $PrimFnUpCaretTilde_EM_YY
//
//  Primitive function for monadic and dyadic UpCaretTilde (ERROR and "nand")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnUpCaretTilde_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnUpCaretTilde_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_UPCARETTILDE);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnUpCaretTilde_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecUpCaretTildeStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpCaretTildeStorageTypeDyd
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

    // All simple numerics return Boolean
    if (IsSimpleNum (aplTypeRes))
        aplTypeRes = ARRAY_BOOL;

    return aplTypeRes;
} // End PrimSpecUpCaretTildeStorageTypeDyd


//***************************************************************************
//  $PrimFnDydUpCaretTildeBisBvB
//
//  Primitive scalar function dyadic UpCaretTilde:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydUpCaretTildeBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return !(aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretTildeBisBvB


//***************************************************************************
//  $PrimFnDydUpCaretTildeIisIvI
//
//  Primitive scalar function dyadic UpCaretTilde:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydUpCaretTildeBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    if (!IsBooleanValue (aplIntegerLft)
     || !IsBooleanValue (aplIntegerRht))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    return !(((APLBOOL) aplIntegerLft) & (APLBOOL) aplIntegerRht);
} // End PrimFnDydUpCaretTildeBisIvI


//***************************************************************************
//  $PrimFnDydUpCaretTildeBisFvF
//
//  Primitive scalar function dyadic UpCaretTilde:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydUpCaretTildeBisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT aplLft,
           aplRht;
    UBOOL  bRet = TRUE;

    // Attempt to convert the float to an integer using System CT
    aplLft = FloatToAplint_SCT (aplFloatLft, &bRet);
    if (bRet)
        // Attempt to convert the float to an integer using System CT
        aplRht = FloatToAplint_SCT (aplFloatRht, &bRet);
    if (!bRet
     || !IsBooleanValue (aplLft)
     || !IsBooleanValue (aplRht))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return !(((APLBOOL) aplLft) & (APLBOOL) aplRht);
} // End PrimFnDydUpCaretTildeBisFvF


//***************************************************************************
//  End of File: pf_nucaret.c
//***************************************************************************
