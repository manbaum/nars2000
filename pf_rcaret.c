//***************************************************************************
//	NARS2000 -- Primitive Function -- RightCaret
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
PRIMSPEC PrimSpecRightCaret = {
	// Monadic functions
	&PrimFnMonValenceError_EM,
	NULL,	// &PrimSpecRightCaretStorageTypeMon, -- Can't happen w/RightCaret
	NULL,	// &PrimFnMonRightCaretAPA_EM, -- Can't happen w/RightCaret

	NULL,	// &PrimFnMonRightCaretBisB, -- Can't happen w/RightCaret
	NULL,	// &PrimFnMonRightCaretBisI, -- Can't happen w/RightCaret
	NULL,	// &PrimFnMonRightCaretBisF, -- Can't happen w/RightCaret

////					IisB,	// Handled via type promotion (to IisI)
	NULL,	// &PrimFnMonRightCaretIisI, -- Can't happend w/RightCaret
	NULL,	// &PrimFnMonRightCaretIisF, -- Can't happend w/RightCaret

////					FisB,	// Handled via type promotion (to FisI)
	NULL,	// &PrimFnMonRightCaretFisI, -- Can't happen w/RightCaret
	NULL,	// &PrimFnMonRightCaretFisF, -- Can't happen w/RightCaret

	// Dyadic functions
	&PrimFnDyd_EM_YY,
	&PrimSpecRightCaretStorageTypeDyd,
	NULL,	// &PrimFnDydRightCaretAPA_EM, -- Can't happen w/RightCaret

	&PrimFnDydRightCaretBisBvB,
	&PrimFnDydRightCaretBisIvI,
	&PrimFnDydRightCaretBisFvF,
	NULL,	// &PrimFnDydRightCaretBisCvC, -- Can't happen w/RightCaret

////				 IisBvB,	// Handled via type promotion (to IisIvI)
	NULL,	// &PrimFnDydRightCaretIisIvI, -- Can't happen w/RightCaret
	NULL,	// &PrimFnDydRightCaretIisFvF, -- Can't happen w/RightCaret

////				 FisBvB,	// Handled via type promotion (to FisIvI)
	NULL,	// &PrimFnDydRightCaretFisIvI, -- Can't happen w/RightCaret
	NULL,	// &PrimFnDydRightCaretFisFvF, -- Can't happen w/RightCaret
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecRightCaret};
#endif


//***************************************************************************
//	$PrimFnRightCaret_EM_YY
//
//	Primitive function for monadic and dyadic RightCaret (ERROR and "less than")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimFnRightCaret_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnRightCaret_EM_YY
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	// Ensure not an overflow function
	Assert (lptkFunc->tkData.tkChar EQ UTF16_RIGHTCARET);

	// Split cases based upon monadic or dyadic
	if (lptkLftArg EQ NULL)
		return (*lpPrimSpec->PrimFnMon_EM_YY) ( 		   lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
	else
		return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnRightCaret_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	$PrimSpecRightCaretStorageTypeDyd
//
//	Primitive dyadic scalar function special handling:	Storage type
//***************************************************************************

APLSTYPE PrimSpecRightCaretStorageTypeDyd
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
} // End PrimSpecRightCaretStorageTypeDyd


//***************************************************************************
//	$PrimFnDydRightCaretBisBvB
//
//	Primitive scalar function dyadic RightCaret:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydRightCaretBisBvB
	(APLBOOL	aplBooleanLft,
	 APLBOOL	aplBooleanRht,
	 LPPRIMSPEC lpPrimSpec)

{
	return aplBooleanLft > aplBooleanRht;
} // End PrimFnDydRightCaretBisBvB


//***************************************************************************
//	$PrimFnDydRightCaretBisIvI
//
//	Primitive scalar function dyadic RightCaret:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydRightCaretBisIvI
	(APLINT 	aplIntegerLft,
	 APLINT 	aplIntegerRht,
	 LPPRIMSPEC lpPrimSpec)

{
	return (aplIntegerLft > aplIntegerRht);
} // End PrimFnDydRightCaretBisIvI


//***************************************************************************
//	$PrimFnDydRightCaretBisFvF
//
//	Primitive scalar function dyadic RightCaret:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydRightCaretBisFvF
	(APLFLOAT	aplFloatLft,
	 APLFLOAT	aplFloatRht,
	 LPPRIMSPEC lpPrimSpec)

{
	// If the left and right arguments are tolerantly-equal, return 0
	if (PrimFnDydEqualBisFvF (aplFloatLft, aplFloatRht, lpPrimSpec))
		return 0;

	// Otherwise, return the natural result
	return (aplFloatLft > aplFloatRht);
} // End PrimFnDydRightCaretBisFvF


//***************************************************************************
//	End of File: pf_rcaret.c
//***************************************************************************
