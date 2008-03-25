//***************************************************************************
//	NARS2000 -- Primitive Function -- CircleSlope
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


//***************************************************************************
//	$PrimFnCircleSlope_EM_YY
//
//	Primitive function for monadic and dyadic CircleSlope ("reverse axes" and "transpose")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimFnCircleSlope_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnCircleSlope_EM_YY
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	// Ensure not an overflow function
	Assert (lptkFunc->tkData.tkChar EQ UTF16_CIRCLESLOPE);

	// If the right arg is a list, ...
	if (IsTknParList (lptkRhtArg))
		return PrimFnSyntaxError_EM (lptkFunc);

	// Split cases based upon monadic or dyadic
	if (lptkLftArg EQ NULL)
		return PrimFnMonCircleSlope_EM_YY ( 		   lptkFunc, lptkRhtArg, lptkAxis);
	else
		return PrimFnDydCircleSlope_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnCircleSlope_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	$PrimProtoFnCircleSlope_EM_YY
//
//	Generate a prototype for the primitive functions monadic & dyadic CircleSlope
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimProtoFnCircleSlope_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnCircleSlope_EM_YY
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	//***************************************************************
	// Called monadically or dyadically
	//***************************************************************

	// Convert to a prototype
	return PrimProtoFnMixed_EM_YY (&PrimFnCircleSlope_EM_YY,// Ptr to primitive function routine
									lptkLftArg, 			// Ptr to left arg token
									lptkFunc,				// Ptr to function token
									lptkRhtArg, 			// Ptr to right arg token
									lptkAxis);				// Ptr to axis token (may be NULL)
} // End PrimProtoFnCircleSlope_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	$PrimFnMonCircleSlope_EM_YY
//
//	Primitive function for monadic CircleSlope ("reverse axes")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimFnMonCircleSlope_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonCircleSlope_EM_YY
	(LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	APLSTYPE	 aplTypeRht;	// Right arg storage type
	APLNELM 	 aplNELMRht;	// ...		 NELM
	APLRANK 	 aplRankRht;	// ...		 rank
	HGLOBAL 	 hGlbLft;		// Left arg global memory handle
	LPVOID		 lpMemLft;		// Ptr to left arg global memory
	LPPL_YYSTYPE lpYYRes,		// Ptr to the result
				 lpYYRes2;		// Ptr to secondary result
	UINT		 ByteRes;		// # bytes in the result
	APLBOOL 	 bQuadIO;		// []IO

	// Get the current value of []IO
	bQuadIO = GetQuadIO ();

	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
		goto SYNTAX_EXIT;

	// Get the attributes (Type, NELM, and Rank) of the right arg
	AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

	// Calculate space needed for the left arg
	ByteRes = (UINT) CalcArraySize (ARRAY_APA, aplRankRht, 1);

	// Allocate storage for the left argument
	hGlbLft = DbgGlobalAlloc (GHND, ByteRes);
	if (!hGlbLft)
		goto WSFULL_EXIT;

	// Lock the memory to get a ptr to it
	lpMemLft = MyGlobalLock (hGlbLft);

#define lpHeader	((LPVARARRAY_HEADER) lpMemLft)
	// Fill in the header values
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = ARRAY_APA;
////lpHeader->PermNdx	 = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar	 = 0;			// Already zero from GHND
	lpHeader->RefCnt	 = 1;
	lpHeader->NELM		 = aplRankRht;
	lpHeader->Rank		 = 1;
#undef	lpHeader

	// Fill in the dimension
	*VarArrayBaseToDim (lpMemLft) = aplRankRht;

	// Skip over the header and dimensions to the data
	lpMemLft = VarArrayBaseToData (lpMemLft, 1);

	// Fill in the APA parameters
#define lpAPA		((LPAPLAPA) lpMemLft)
	lpAPA->Off = (aplRankRht - 1) + bQuadIO;
	lpAPA->Mul = -1;
#undef	lpAPA
	// We no longer need this ptr
	MyGlobalUnlock (hGlbLft); lpMemLft = NULL;

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the left arg token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0; 	// Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbLft);
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	// Call the dyadic function
	lpYYRes2 = PrimFnDydCircleSlope_EM_YY (&lpYYRes->tkToken,	// Ptr to left arg token
											lptkFunc,			// Ptr to function token
											lptkRhtArg, 		// Ptr to right arg token
											lptkAxis);			// Ptr to axis token (may be NULL)
	// We no longer need this storage
	FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;

	return lpYYRes2;

SYNTAX_EXIT:
	ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
							   lptkAxis);
	return NULL;

WSFULL_EXIT:
	ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
							   lptkFunc);
	return NULL;
} // End PrimFnMonCircleSlope_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	$PrimFnDydCircleSlope_EM_YY
//
//	Primitive function for dyadic CircleSlope ("transpose")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimFnDydCircleSlope_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydCircleSlope_EM_YY
	(LPTOKEN lptkLftArg,			// Ptr to left arg token
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	APLSTYPE	 aplTypeLft,		// Left arg storage type
				 aplTypeRht,		// Right ...
				 aplTypeRes;		// Result	...
	APLNELM 	 aplNELMLft,		// Left arg NELM
				 aplNELMRht,		// Right ...
				 aplNELMRes;		// Result	...
	APLRANK 	 aplRankLft,		// Left arg rank
				 aplRankRht,		// Right ...
				 aplRankRes;		// Result	...
	HGLOBAL 	 hGlbLft = NULL,	// Left arg global memory handle
				 hGlbRht = NULL,	// Right ...
				 hGlbRes = NULL,	// Result	...
				 hGlbAxis = NULL,	// Axis 	...
				 hGlbWVec = NULL,	// Weighting vector ...
				 hGlbOdo = NULL;	// Odometer ...
	LPAPLDIM	 lpMemDimRht,		// Ptr to right arg dimensions
				 lpMemDimRes;		// Ptr to result	...
	APLDIM		 uMinDim;			//
	LPVOID		 lpMemLft = NULL,	// Ptr to left arg global memory
				 lpMemRht = NULL,	// Ptr to right ...
				 lpMemRes = NULL;	// Ptr to result   ...
	LPAPLUINT	 lpMemAxis = NULL,	// Ptr to axis global memory
				 lpMemGrUp, 		// Ptr to grade up ...
				 lpMemWVec = NULL,	// Ptr to weighting vector ...
				 lpMemOdo = NULL;	// Ptr to odometer ...
	APLUINT 	 ByteRes,			// # bytes in the result
				 uRht,				// Right arg loop counter
				 uRes,				// Result	 ...
				 uOdo;				// Odometer  ...
	LPPL_YYSTYPE lpYYRes = NULL;	// Ptr to the result
	UINT		 uBitIndex, 		// Bit index for marching through Booleans
				 uBitMask;			// Bit mask  ...
	APLINT		 iDim,				// Dimension loop counter
				 apaOffRht, 		// Right arg APA offset
				 apaMulRht; 		// ...			 multiplier

	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
		goto SYNTAX_EXIT;

	// Get the attributes (Type, NELM, and Rank) of the left & right args
	AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
	AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

	// Get left and right arg's global ptrs
	GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
	GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

	// Check for RANK ERROR
	if (aplRankLft > 1)
		goto RANK_EXIT;

	// Check for LENGTH ERROR
	if (aplNELMLft NE aplRankRht)
		goto LENGTH_EXIT;

	// Treat the left arg as an axis
	if (!CheckAxis_EM (lptkLftArg,		// The "axis" token
					   aplRankRht,		// All values less than this
					   FALSE,			// TRUE iff scalar or one-element vector only
					   FALSE,			// TRUE iff want sorted axes
					   TRUE,			// TRUE iff axes must be contiguous
					   TRUE,			// TRUE iff duplicate axes are allowed
					   NULL,			// TRUE iff fractional values allowed
					  &aplRankRes,		// Return last axis value
					   NULL,			// Return # elements in axis vector
					  &hGlbAxis))		// Return HGLOBAL with APLUINT axis values
		goto DOMAIN_EXIT;

	// Map APA right arg to INT result
	if (IsSimpleAPA (aplTypeRht))
		aplTypeRes = ARRAY_INT;
	else
		aplTypeRes = aplTypeRht;

	// Strip out the simple scalar right argument case
	if (aplRankRht EQ 0 && IsSimpleNH (aplTypeRes))
	{
		// Allocate a new YYRes
		lpYYRes = YYAlloc ();

		// Split cases based upon the right arg's token type
		switch (lptkRhtArg->tkFlags.TknType)
		{
			case TKT_VARNAMED:
				// tkData is an LPSYMENTRY
				Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

				// If it's not immediate, we must look inside the array
				if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
				{
					// stData is a valid HGLOBAL variable array
					Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkSym->stData.stGlbData));

					// If we ever get here, we must have missed a type demotion
					DbgStop ();
				} // End IF

				// Handle the immediate case

				// Fill in the result token
				lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
				lpYYRes->tkToken.tkFlags.ImmType   = lptkRhtArg->tkData.tkSym->stFlags.ImmType;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
				lpYYRes->tkToken.tkData.tkLongest  = lptkRhtArg->tkData.tkSym->stData.stLongest;
				lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

				break;

			case TKT_VARIMMED:
				// Fill in the result token
				lpYYRes->tkToken = *lptkRhtArg;

				break;

			defstop
				break;
		} // End SWITCH

		goto NORMAL_EXIT;
	} // End IF

	// The rank of the result is one more than the highest axis value
	aplRankRes++;

	// Lock the memory to get a ptr to it
	lpMemAxis = MyGlobalLock (hGlbAxis);

	// Point to the grade-up of the first
	//	 <aplRankRht> values in lpMemAxis
	lpMemGrUp = &lpMemAxis[aplRankRht];

	// Skip over the header to the dimensions
	lpMemDimRht = VarArrayBaseToDim (lpMemRht);

	// Skip over the header and dimensions to the data
	lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

	// Calculate the NELM of the result
	aplNELMRes = 1;
	if (aplRankRes NE 0)
	{
		uMinDim = lpMemDimRht[lpMemGrUp[0]];
		for (uRht = 1; uRht < aplRankRht; uRht++)
		{
			// Compare the values corresponding to the indices
			//	 to see if they are the same (and thus coalescing)
			if (lpMemAxis[lpMemGrUp[uRht - 1]] EQ lpMemAxis[lpMemGrUp[uRht]])
				uMinDim = min (uMinDim, lpMemDimRht[lpMemGrUp[uRht]]);
			else
			{
				aplNELMRes *= uMinDim;
				uMinDim = lpMemDimRht[lpMemGrUp[uRht]];
			} // End IF/ELSE
		} // End FOR

		// Count in the last dimension
		aplNELMRes *= uMinDim;
	} // End IF

	// Calculate space needed for the result
	ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

	// Allocate space for the result
	// N.B. Conversion from APLUINT to UINT.
	Assert (ByteRes EQ (UINT) ByteRes);
	hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
	if (!hGlbRes)
		goto WSFULL_EXIT;

	// Lock the memory to get a ptr to it
	lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader	((LPVARARRAY_HEADER) lpMemRes)
	// Fill in the header values
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = aplTypeRes;
////lpHeader->PermNdx	 = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar	 = 0;			// Already zero from GHND
	lpHeader->RefCnt	 = 1;
	lpHeader->NELM		 = aplNELMRes;
	lpHeader->Rank		 = aplRankRes;
#undef	lpHeader

	// Skip over the header to the dimensions
	lpMemRes = lpMemDimRes = VarArrayBaseToDim (lpMemRes);

	// Fill in the dimensions
	if (aplRankRes NE 0)
	{
		uMinDim = lpMemDimRht[lpMemGrUp[0]];
		for (uRht = 1; uRht < aplRankRht; uRht++)
		{
			// Compare the values corresponding to the indices
			//	 to see if they are the same (and thus coalescing)
			if (lpMemAxis[lpMemGrUp[uRht - 1]] EQ lpMemAxis[lpMemGrUp[uRht]])
				uMinDim = min (uMinDim, lpMemDimRht[lpMemGrUp[uRht]]);
			else
			{
				*((LPAPLDIM) lpMemRes)++ = uMinDim;
				uMinDim = lpMemDimRht[lpMemGrUp[uRht]];
			} // End IF/ELSE
		} // End FOR

		// Save the last dimension
		*((LPAPLDIM) lpMemRes)++ = uMinDim;
	} // End IF

	// lpMemRes now points to the result's data

	// Handle empty nested array results (prototypes)
	if (aplNELMRes EQ 0 && IsNested (aplTypeRes))
	{
		*((LPAPLNESTED) lpMemRes) = CopySymGlbInd (lpMemRht);

		goto PROTO_EXIT;
	} // End IF

	//***************************************************************
	// Allocate space for the weighting vector which is
	//	 {times}{backscan}1{drop}({rho}R),1
	// N.B.  Conversion from APLUINT to UINT.
	//***************************************************************
	ByteRes = aplRankRht * sizeof (APLUINT);

	// In case the result is a scalar, allocate at least
	//	 one byte so the GlobalLock doesn't fail -- Windows
	//	 doesn't handle the empty case well.
	ByteRes = max (ByteRes, 1);

	Assert (ByteRes EQ (UINT) ByteRes);
	hGlbWVec = DbgGlobalAlloc (GHND, (UINT) ByteRes);
	if (!hGlbWVec)
		goto WSFULL_EXIT;

	// Lock the memory to get a ptr to it
	lpMemWVec = MyGlobalLock (hGlbWVec);

	// Loop through the dimensions of the result in reverse
	//	 order {backscan} and compute the cumulative product
	//	 (weighting vector).
	// Note we use a signed index variable because we're
	//	 walking backwards and the test against zero must be
	//	 made as a signed variable.
	for (uRes = 1, iDim = aplRankRht - 1; iDim >= 0; iDim--)
	{
		lpMemWVec[iDim] = uRes;
		uRes *= lpMemDimRht[iDim];
	} // End FOR

	//***************************************************************
	// Allocate space for the odometer array, one value per dimension
	//	 in the result, with values initially all zero (thanks to GHND).
	// N.B.  Conversion from APLUINT to UINT.
	//***************************************************************
	ByteRes = aplRankRes * sizeof (APLUINT);

	// In case the result is a scalar, allocate at least
	//	 one byte so the GlobalLock doesn't fail -- Windows
	//	 doesn't handle the empty case well.
	ByteRes = max (ByteRes, 1);

	Assert (ByteRes EQ (UINT) ByteRes);
	hGlbOdo = DbgGlobalAlloc (GHND, (UINT) ByteRes);
	if (!hGlbOdo)
		goto WSFULL_EXIT;

	// Lock the memory to get a ptr to it
	lpMemOdo = MyGlobalLock (hGlbOdo);

	// Copy the data to the result

	// Split cases based upon the result/right arg storage type
	switch (aplTypeRht)
	{
		case ARRAY_BOOL:
			uBitIndex = 0;

			// Loop through the elements in the result
			for (uRes = 0; uRes < aplNELMRes; uRes++)
			{
				// Use the index in lpMemOdo to calculate the
				//	 corresponding index in lpMemRes where the
				//	 next value from lpMemRht goes.
				for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
					uRht += lpMemOdo[lpMemAxis[uOdo]] * lpMemWVec[uOdo];

				// Increment the odometer in lpMemOdo subject to
				//	 the values in lpMemDimRes
				IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

				uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) uRht);

				// Copy element # uRht from the right arg to lpMemRes[uRes]
				*((LPAPLBOOL) lpMemRes) |= ((uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? 1 : 0) << uBitIndex;

				// Check for end-of-byte
				if (++uBitIndex EQ NBIB)
				{
					uBitIndex = 0;				// Start over
					((LPAPLBOOL) lpMemRes)++;	// Skip to next byte
				} // End IF
			} // End FOR

			break;

		case ARRAY_INT:
			// Loop through the elements in the result
			for (uRes = 0; uRes < aplNELMRes; uRes++)
			{
				// Use the index in lpMemOdo to calculate the
				//	 corresponding index in lpMemRes where the
				//	 next value from lpMemRht goes.
				for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
					uRht += lpMemOdo[lpMemAxis[uOdo]] * lpMemWVec[uOdo];

				// Increment the odometer in lpMemOdo subject to
				//	 the values in lpMemDimRes
				IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

				// Copy element # uRht from the right arg to lpMemRes[uRes]
				((LPAPLINT) lpMemRes)[uRes] = ((LPAPLINT) lpMemRht)[uRht];
			} // End FOR

			break;

		case ARRAY_FLOAT:
			// Loop through the elements in the result
			for (uRes = 0; uRes < aplNELMRes; uRes++)
			{
				// Use the index in lpMemOdo to calculate the
				//	 corresponding index in lpMemRes where the
				//	 next value from lpMemRht goes.
				for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
					uRht += lpMemOdo[lpMemAxis[uOdo]] * lpMemWVec[uOdo];

				// Increment the odometer in lpMemOdo subject to
				//	 the values in lpMemDimRes
				IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

				// Copy element # uRht from the right arg to lpMemRes[uRes]
				((LPAPLFLOAT) lpMemRes)[uRes] = ((LPAPLFLOAT) lpMemRht)[uRht];
			} // End FOR

			break;

		case ARRAY_APA:
#define lpAPA		((LPAPLAPA) lpMemRht)
			// Get the APA parameters
			apaOffRht = lpAPA->Off;
			apaMulRht = lpAPA->Mul;
#undef	lpAPA
			// Loop through the elements in the result
			for (uRes = 0; uRes < aplNELMRes; uRes++)
			{
				// Use the index in lpMemOdo to calculate the
				//	 corresponding index in lpMemRes where the
				//	 next value from lpMemRht goes.
				for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
					uRht += lpMemOdo[lpMemAxis[uOdo]] * lpMemWVec[uOdo];

				// Increment the odometer in lpMemOdo subject to
				//	 the values in lpMemDimRes
				IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

				// Copy element # uRht from the right arg to lpMemRes[uRes]
				((LPAPLINT) lpMemRes)[uRes] = apaOffRht + apaMulRht * uRht;
			} // End FOR

			break;

		case ARRAY_CHAR:
			// Loop through the elements in the result
			for (uRes = 0; uRes < aplNELMRes; uRes++)
			{
				// Use the index in lpMemOdo to calculate the
				//	 corresponding index in lpMemRes where the
				//	 next value from lpMemRht goes.
				for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
					uRht += lpMemOdo[lpMemAxis[uOdo]] * lpMemWVec[uOdo];

				// Increment the odometer in lpMemOdo subject to
				//	 the values in lpMemDimRes
				IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

				// Copy element # uRht from the right arg to lpMemRes[uRes]
				((LPAPLCHAR) lpMemRes)[uRes] = ((LPAPLCHAR) lpMemRht)[uRht];
			} // End FOR

			break;

		case ARRAY_HETERO:
		case ARRAY_NESTED:
			// Loop through the elements in the result
			for (uRes = 0; uRes < aplNELMRes; uRes++)
			{
				// Use the index in lpMemOdo to calculate the
				//	 corresponding index in lpMemRes where the
				//	 next value from lpMemRht goes.
				for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
					uRht += lpMemOdo[lpMemAxis[uOdo]] * lpMemWVec[uOdo];

				// Increment the odometer in lpMemOdo subject to
				//	 the values in lpMemDimRes
				IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

				// Copy element # uRht from the right arg to lpMemRes[uRes]
				((LPAPLHETERO) lpMemRes)[uRes] = CopySymGlbDir (((LPAPLHETERO) lpMemRht)[uRht]);
			} // End FOR

			break;

		defstop
			break;
	} // End SWITCH
PROTO_EXIT:
	// Unlock the result global memory in case TypeDemote actually demotes
	if (hGlbRes && lpMemRes)
	{
		// We no longer need this ptr
		MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
	} // End IF

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0; 	// Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	// See if it fits into a lower (but not necessarily smaller) datatype
	TypeDemote (&lpYYRes->tkToken);

	goto NORMAL_EXIT;

RANK_EXIT:
	ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
							   lptkLftArg);
	goto ERROR_EXIT;

LENGTH_EXIT:
	ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
							   lptkLftArg);
	goto ERROR_EXIT;

DOMAIN_EXIT:
	ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
							   lptkLftArg);
	goto ERROR_EXIT;

SYNTAX_EXIT:
	ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
							   lptkAxis);
	goto ERROR_EXIT;

WSFULL_EXIT:
	ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
							   lptkFunc);
	goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
	if (hGlbWVec)
	{
		if (lpMemWVec)
		{
			// We no longer need this ptr
			MyGlobalUnlock (hGlbWVec); lpMemWVec = NULL;
		} // End IF

		// We no longer need this storage
		DbgGlobalFree (hGlbWVec); hGlbWVec = NULL;
	} // End IF

	if (hGlbOdo)
	{
		if (lpMemOdo)
		{
			// We no longer need this ptr
			MyGlobalUnlock (hGlbOdo); lpMemOdo = NULL;
		} // End IF

		// We no longer need this storage
		DbgGlobalFree (hGlbOdo); hGlbOdo = NULL;
	} // End IF

	if (hGlbAxis)
	{
		if (lpMemAxis)
		{
			// We no longer need this ptr
			MyGlobalUnlock (hGlbAxis); lpMemAxis = NULL;
		} // End IF

		// We no longer need this storage
		DbgGlobalFree (hGlbAxis); hGlbAxis = NULL;
	} // End IF

	if (hGlbLft && lpMemLft)
	{
		// We no longer need this ptr
		MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
	} // End IF

	if (hGlbRht && lpMemRht)
	{
		// We no longer need this ptr
		MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
	} // End IF

	if (hGlbRes && lpMemRes)
	{
		// We no longer need this ptr
		MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
	} // End IF

	return lpYYRes;
} // End PrimFnDydCircleSlope_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	End of File: pf_cslope.c
//***************************************************************************
