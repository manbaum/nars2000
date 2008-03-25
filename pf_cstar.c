//***************************************************************************
//	NARS2000 -- Primitive Function -- CircleStar
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
PRIMSPEC PrimSpecCircleStar =
{
	// Monadic functions
	&PrimFnMon_EM_YY,
	&PrimSpecCircleStarStorageTypeMon,
	NULL,	// &PrimFnMonCircleStarAPA_EM, -- Can't happen w/CircleStar

	NULL,	// &PrimFnMonCircleStarBisB, -- Can't happen w/CircleStar
	NULL,	// &PrimFnMonCircleStarBisI, -- Can't happen w/CircleStar
	NULL,	// &PrimFnMonCircleStarBisF, -- Can't happen w/CircleStar

////			   IisB,	 // Handled via type promotion (to IisI)
	NULL,	// &PrimFnMonCircleStarIisI, -- Can't happen w/CircleStar
	NULL,	// &PrimFnMonCircleStarIisF, -- Can't happen w/CircleStar

////			   FisB,	 // Handled via type promotion (to FisI)
	&PrimFnMonCircleStarFisI,
	&PrimFnMonCircleStarFisF,

	// Dyadic functions
	&PrimFnDyd_EM_YY,
	&PrimSpecCircleStarStorageTypeDyd,
	NULL,	// &PrimFnDydCircleStarAPA_EM, -- Can't happen w/CircleStar

	&PrimFnDydRightCaretUnderbarBisBvB,
	NULL,	// &PrimFnDydCircleStarBisIvI, -- Can't happen w/CircleStar
	NULL,	// &PrimFnDydCircleStarBisFvF, -- Can't happen w/CircleStar
	NULL,	// &PrimFnDydCircleStarBisCvC, -- Can't happen w/CircleStar

////				 IisBvB,	// Handled via type promotion (to IisIvI)
	NULL,	// &PrimFnDydCircleStarIisIvI, -- Can't happen w/CircleStar
	NULL,	// &PrimFnDydCircleStarIisFvF, -- Can't happen w/CircleStar

////				 FisBvB,	// Handled via type promotion (to FisIvI)
	&PrimFnDydCircleStarFisIvI,
	&PrimFnDydCircleStarFisFvF,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecCircleStar};
#endif


//***************************************************************************
//	$PrimFnCircleStar_EM_YY
//
//	Primitive function for monadic and dyadic CircleStar
//	   ("natural logarithm" and "logarithm")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimFnCircleStar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnCircleStar_EM_YY
	(LPTOKEN	   lptkLftArg,		// Ptr to left arg token (may be NULL if monadic)
	 LPTOKEN	   lptkFunc,		// Ptr to function token
	 LPTOKEN	   lptkRhtArg,		// Ptr to right arg token
	 LPTOKEN	   lptkAxis)		// Ptr to axis token (may be NULL)

{
	// Ensure not an overflow function
	Assert (lptkFunc->tkData.tkChar EQ UTF16_CIRCLESTAR);

	// Split cases based upon monadic or dyadic
	if (lptkLftArg EQ NULL)
		return (*lpPrimSpec->PrimFnMon_EM_YY) ( 		   lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
	else
		return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnCircleStar_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	$PrimSpecCircleStarStorageTypeMon
//
//	Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecCircleStarStorageTypeMon
	(APLNELM	aplNELMRht,
	 LPAPLSTYPE lpaplTypeRht,
	 LPTOKEN	lptkFunc)

{
	APLSTYPE aplTypeRes;

	// In case the right arg is an empty char,
	//	 change its type to BOOL
	if (aplNELMRht EQ 0 && IsSimpleChar (*lpaplTypeRht))
		*lpaplTypeRht = ARRAY_BOOL;

	if (IsSimpleChar (*lpaplTypeRht)
	 || *lpaplTypeRht EQ ARRAY_LIST)
		return ARRAY_ERROR;

	// The storage type of the result is
	//	 the same as that of the right arg
	aplTypeRes = *lpaplTypeRht;

	// Except that BOOL, INT and APA become FLOAT
	if (IsSimpleInt (aplTypeRes))
		aplTypeRes = ARRAY_FLOAT;

	return aplTypeRes;
} // End PrimSpecCircleStarStorageTypeMon


//***************************************************************************
//	$PrimFnMonCircleStarFisI
//
//	Primitive scalar function monadic CircleStar:  F {is} fn I
//***************************************************************************

APLFLOAT PrimFnMonCircleStarFisI
	(APLINT 	aplIntegerRht,
	 LPPRIMSPEC lpPrimSpec)

{
	// Check for indeterminates:  {log} 0
	if (aplIntegerRht EQ 0)
		return TranslateQuadICIndex (ICNDX_LOG0);

	return log ((APLFLOAT) aplIntegerRht);
} // End PrimFnMonCircleStarFisI


//***************************************************************************
//	$PrimFnMonCircleStarFisF
//
//	Primitive scalar function monadic CircleStar:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonCircleStarFisF
	(APLFLOAT	aplFloatRht,
	 LPPRIMSPEC lpPrimSpec)

{
	// Check for indeterminates:  {log} 0
	if (aplFloatRht EQ 0)
		return TranslateQuadICIndex (ICNDX_LOG0);

	return log (aplFloatRht);
} // End PrimFnMonCircleStarFisF


//***************************************************************************
//	$PrimSpecCircleStarStorageTypeDyd
//
//	Primitive dyadic scalar function special handling:	Storage type
//***************************************************************************

APLSTYPE PrimSpecCircleStarStorageTypeDyd
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

	// If both arguments are simple numeric,
	//	 the result is FLOAT
	if (IsSimpleNum (*lpaplTypeLft)
	 && IsSimpleNum (*lpaplTypeRht))
		aplTypeRes = ARRAY_FLOAT;
	else
		// Calculate the storage type of the result
		aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

	return aplTypeRes;
} // End PrimSpecCircleStarStorageTypeDyd


//***************************************************************************
//	$PrimFnDydCircleStarFisIvI
//
//	Primitive scalar function dyadic CircleStar:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydCircleStarFisIvI
	(APLINT 	aplIntegerLft,
	 APLINT 	aplIntegerRht,
	 LPPRIMSPEC lpPrimSpec)

{
	// Check for indeterminates:  B {log} B
	if (IsBooleanValue (aplIntegerLft)
	 && IsBooleanValue (aplIntegerRht))
	switch (2 * aplIntegerLft + 1 * aplIntegerRht)
	{
		case 2 * 0 + 1 * 0:
			return TranslateQuadICIndex (ICNDX_0LOG0);

		case 2 * 0 + 1 * 1:
			return TranslateQuadICIndex (ICNDX_0LOG1);

		case 2 * 1 + 1 * 0:
			return TranslateQuadICIndex (ICNDX_1LOG0);

		case 2 * 1 + 1 * 1:
			return TranslateQuadICIndex (ICNDX_1LOG1);

		defstop
			break;
	} // End SWITCH

	return log ((APLFLOAT) aplIntegerRht) / log ((APLFLOAT) aplIntegerLft);
} // End PrimFnDydCircleStarFisIvI


//***************************************************************************
//	$PrimFnDydCircleStarFisFvF
//
//	Primitive scalar function dyadic CircleStar:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydCircleStarFisFvF
	(APLFLOAT	aplFloatLft,
	 APLFLOAT	aplFloatRht,
	 LPPRIMSPEC lpPrimSpec)

{
	static UINT icndxLog[] = {ICNDX_0LOG0,
							  ICNDX_0LOG1,
							  ICNDX_1LOG0,
							  ICNDX_1LOG1};

	DbgBrk ();		// ***FIXME*** -- Check conversion from double to UINT

	// Check for indeterminates:  B {log} B
	if (IsBooleanValue (aplFloatLft)
	 && IsBooleanValue (aplFloatRht))
		return TranslateQuadICIndex (icndxLog[(UINT) (2 * aplFloatLft + 1 * aplFloatRht)]);
	return log (aplFloatRht) / log (aplFloatLft);
} // End PrimFnDydCircleStarFisFvF


//***************************************************************************
//	End of File: pf_cstar.c
//***************************************************************************
