//***************************************************************************
//  NARS2000 -- Primitive Function -- NotEqual
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
PRIMSPEC PrimSpecNotEqual = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecNotEqualStorageTypeMon, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualAPA_EM, -- Can't happen w/NotEqual

    NULL,   // &PrimFnMonNotEqualBisB, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualBisI, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualBisF, -- Can't happen w/NotEqual

////&PrimFnMonNotEqualIisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonNotEqualIisI, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualIisF, -- Can't happen w/NotEqual

////&PrimFnMonNotEqualFisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonNotEqualFisI, -- Can't happen w/NotEqual
    NULL,   // &PrimFnMonNotEqualFisF, -- Can't happen w/NotEqual

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecNotEqualStorageTypeDyd,
    NULL,   // &PrimFnDydNotEqualAPA_EM, -- Can't happen w/NotEqual

    &PrimFnDydNotEqualBisBvB,
    &PrimFnDydNotEqualBisIvI,
    &PrimFnDydNotEqualBisFvF,
    &PrimFnDydNotEqualBisCvC,

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydNotEqualIisIvI, -- Can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualIisFvF, -- Can't happen w/NotEqual

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydNotEqualFisIvI, -- Can't happen w/NotEqual
    NULL,   // &PrimFnDydNotEqualFisFvF, -- Can't happen w/NotEqual
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecNotEqual};
#endif


//***************************************************************************
//  $PrimFnNotEqual_EM_YY
//
//  Primitive function for monadic and dyadic NotEqual (ERROR and "NotEqual")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnNotEqual_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnNotEqual_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnNotEqual_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecNotEqualStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecNotEqualStorageTypeDyd
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
} // End PrimSpecNotEqualStorageTypeDyd


//***************************************************************************
//  $PrimFnDydNotEqualBisBvB
//
//  Primitive scalar function dyadic NotEqual:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydNotEqualBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft NE aplBooleanRht);
} // End PrimFnDydNotEqualBisBvB


//***************************************************************************
//  $PrimFnDydNotEqualBisIvI
//
//  Primitive scalar function dyadic NotEqual:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydNotEqualBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplIntegerLft NE aplIntegerRht);
} // End PrimFnDydNotEqualBisIvI


//***************************************************************************
//  $PrimFnDydNotEqualBisFvF
//
//  Primitive scalar function dyadic NotEqual:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydNotEqualBisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return !PrimFnDydEqualBisFvF (aplFloatLft, aplFloatRht, lpPrimSpec);
} // End PrimFnDydNotEqualBisFvF


//***************************************************************************
//  $PrimFnDydNotEqualBisCvC
//
//  Primitive scalar function dyadic NotEqual:  B {is} C fn C
//***************************************************************************

APLBOOL PrimFnDydNotEqualBisCvC
    (APLCHAR    aplCharLft,
     APLCHAR    aplCharRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplCharLft NE aplCharRht);
} // End PrimFnDydNotEqualBisCvC


//***************************************************************************
//  End of File: pf_nequal.c
//***************************************************************************
