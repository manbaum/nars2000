//***************************************************************************
//  NARS2000 -- Primitive Function -- Equal
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

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


#ifndef PROTO
PRIMSPEC PrimSpecEqual = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecEqualStorageTypeMon, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualAPA_EM, -- Can't happen w/Equal

    NULL,   // &PrimFnMonEqualBisB, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualBisI, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualBisF, -- Can't happen w/Equal

////               IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonEqualIisI, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualIisF, -- Can't happen w/Equal

////               FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonEqualFisI, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualFisF, -- Can't happen w/Equal

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecEqualStorageTypeDyd,
    NULL,   // &PrimFnDydEqualAPA_EM, -- Can't happen w/Equal

    &PrimFnDydEqualBisBvB,
    &PrimFnDydEqualBisIvI,
    &PrimFnDydEqualBisFvF,
    &PrimFnDydEqualBisCvC,

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydEqualIisIvI, -- Can't happen w/Equal
    NULL,   // &PrimFnDydEqualIisFvF, -- Can't happen w/Equal

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydEqualFisIvI, -- Can't happen w/Equal
    NULL,   // &PrimFnDydEqualFisFvF, -- Can't happen w/Equal
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecEqual};
#endif


//***************************************************************************
//  $PrimFnEqual_EM_YY
//
//  Primitive function for monadic and dyadic Equal (ERROR and "equal")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnEqual_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnEqual_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnEqual_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecEqualStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecEqualStorageTypeDyd
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

    // All simple numeric results are Boolean
    if (IsSimpleNum (aplTypeRes))
        aplTypeRes = ARRAY_BOOL;

    return aplTypeRes;
} // End PrimSpecEqualStorageTypeDyd


//***************************************************************************
//  $PrimFnDydEqualBisBvB
//
//  Primitive scalar function dyadic Equal:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydEqualBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft EQ aplBooleanRht);
} // End PrimFnDydEqualBisBvB


//***************************************************************************
//  $PrimFnDydEqualBisIvI
//
//  Primitive scalar function dyadic Equal:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydEqualBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplIntegerLft EQ aplIntegerRht);
} // End PrimFnDydEqualBisIvI


//***************************************************************************
//  $PrimFnDydEqualBisFvF
//
//  Primitive scalar function dyadic Equal:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydEqualBisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT fQuadCT;           // []CT

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    return CompareCT (aplFloatLft, aplFloatRht, fQuadCT, lpPrimSpec);
} // End PrimFnDydEqualBisFvF


//***************************************************************************
//  $PrimFnDydEqualBisCvC
//
//  Primitive scalar function dyadic Equal:  B {is} C fn C
//***************************************************************************

APLBOOL PrimFnDydEqualBisCvC
    (APLCHAR    aplCharLft,
     APLCHAR    aplCharRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplCharLft EQ aplCharRht);
} // End PrimFnDydEqualBisCvC


//***************************************************************************
//  End of File: pf_equal.c
//***************************************************************************
