//***************************************************************************
//	NARS2000 -- Primitive Function -- LeftCaret
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
#include <math.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


#ifndef PROTO
PRIMSPEC PrimSpecLeftCaret = {
	// Monadic functions
	&PrimFnMonValenceError_EM,
	NULL,	// &PrimSpecLeftCaretStorageTypeMon, -- Can't happen w/LeftCaret
	NULL,	// &PrimFnMonLeftCaretAPA_EM, -- Can't happen w/LeftCaret

	NULL,	// &PrimFnMonLeftCaretBisB, -- Can't happen w/LeftCaret
	NULL,	// &PrimFnMonLeftCaretBisI, -- Can't happen w/LeftCaret
	NULL,	// &PrimFnMonLeftCaretBisF, -- Can't happen w/LeftCaret

////				   IisB,   // Handled via type promotion (to IisI)
	NULL,	// &PrimFnMonLeftCaretIisI, -- Can't happen w/LeftCaret
	NULL,	// &PrimFnMonLeftCaretIisF, -- Can't happen w/LeftCaret

////				   FisB,   // Handled via type promotion (to FisI)
	NULL,	// &PrimFnMonLeftCaretFisI, -- Can't happen w/LeftCaret
	NULL,	// &PrimFnMonLeftCaretFisF, -- Can't happen w/LeftCaret

	// Dyadic functions
	&PrimFnDyd_EM_YY,
	&PrimSpecLeftCaretStorageTypeDyd,
	NULL,	// &PrimFnDydLeftCaretAPA_EM, -- Can't happen w/LeftCaret

	&PrimFnDydLeftCaretBisBvB,
	&PrimFnDydLeftCaretBisIvI,
	&PrimFnDydLeftCaretBisFvF,
	NULL,	// &PrimFnDydLeftCaretBisCvC, -- Can't happen w/LeftCaret

////				 IisBvB,	// Handled via type promotion (to IisIvI)
	NULL,	// &PrimFnDydLeftCaretIisIvI, -- Can't happen w/LeftCaret
	NULL,	// &PrimFnDydLeftCaretIisFvF, -- Can't happen w/LeftCaret

////				 FisBvB,	// Handled via type promotion (to FisIvI)
	NULL,	// &PrimFnDydLeftCaretFisIvI, -- Can't happen w/LeftCaret
	NULL,	// &PrimFnDydLeftCaretFisFvF, -- Can't happen w/LeftCaret
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecLeftCaret};
#endif


//***************************************************************************
//	$PrimFnLeftCaret_EM_YY
//
//	Primitive function for monadic and dyadic LeftCaret (ERROR and "greater than")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimFnLeftCaret_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnLeftCaret_EM_YY
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	// Ensure not an overflow function
	Assert (lptkFunc->tkData.tkChar EQ UTF16_LEFTCARET);

	// Split cases based upon monadic or dyadic
	if (lptkLftArg EQ NULL)
		return (*lpPrimSpec->PrimFnMon_EM_YY) ( 		   lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
	else
		return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnLeftCaret_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	$PrimSpecLeftCaretStorageTypeDyd
//
//	Primitive dyadic scalar function special handling:	Storage type
//***************************************************************************

APLSTYPE PrimSpecLeftCaretStorageTypeDyd
	(APLNELM	aplNELMLft,
	 LPAPLSTYPE lpaplTypeLft,
	 LPTOKEN	lptkFunc,
	 APLNELM	aplNELMRht,
	 LPAPLSTYPE lpaplTypeRht)

{
	APLSTYPE aplTypeRes;

	// In case the left arg is an empty char,
	//	 change its type to BOOL
	if (aplNELMLft EQ 0 && IsSimpleChar (*lpaplTypeLft))
		*lpaplTypeLft = ARRAY_BOOL;

	// In case the right arg is an empty char,
	//	 change its type to BOOL
	if (aplNELMRht EQ 0 && IsSimpleChar (*lpaplTypeRht))
		*lpaplTypeRht = ARRAY_BOOL;

	// Calculate the storage type of the result
	aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

	// All simple numeric results are Boolean
	if (IsSimpleNum (aplTypeRes))
		aplTypeRes = ARRAY_BOOL;

	return aplTypeRes;
} // End PrimSpecLeftCaretStorageTypeDyd


//***************************************************************************
//	$PrimFnDydLeftCaretBisBvB
//
//	Primitive scalar function dyadic LeftCaret:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydLeftCaretBisBvB
	(APLBOOL	aplBooleanLft,
	 APLBOOL	aplBooleanRht,
	 LPPRIMSPEC lpPrimSpec)

{
	return aplBooleanLft < aplBooleanRht;
} // End PrimFnDydLeftCaretBisBvB


//***************************************************************************
//	$PrimFnDydLeftCaretBisIvI
//
//	Primitive scalar function dyadic LeftCaret:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydLeftCaretBisIvI
	(APLINT 	aplIntegerLft,
	 APLINT 	aplIntegerRht,
	 LPPRIMSPEC lpPrimSpec)

{
	return (aplIntegerLft < aplIntegerRht);
} // End PrimFnDydLeftCaretBisIvI


//***************************************************************************
//	$PrimFnDydLeftCaretBisFvF
//
//	Primitive scalar function dyadic LeftCaret:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydLeftCaretBisFvF
	(APLFLOAT	aplFloatLft,
	 APLFLOAT	aplFloatRht,
	 LPPRIMSPEC lpPrimSpec)

{
	// If the left and right arguments are tolerantly-equal, return 0
	if (PrimFnDydEqualBisFvF (aplFloatLft, aplFloatRht, lpPrimSpec))
		return 0;

	// Otherwise, return the natural result
	return (aplFloatLft < aplFloatRht);
} // End PrimFnDydLeftCaretBisFvF


//***************************************************************************
//	End of File: pf_lcaret.c
//***************************************************************************
