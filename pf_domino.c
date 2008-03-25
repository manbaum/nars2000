//***************************************************************************
//	NARS2000 -- Primitive Function -- Domino
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

#define __GSL_MATRIX_COMPLEX_LONG_DOUBLE_H__

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "gsl/gsl_vector_double.h"
#include "gsl/gsl_matrix_double.h"
#include "gsl/gsl_permutation.h"
#include "gsl/gsl_linalg.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//	$PrimFnDomino_EM_YY
//
//	Primitive function for monadic and dyadic Domino ("matrix inverse" and "matrix divide")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimFnDomino_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDomino_EM_YY
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	// Ensure not an overflow function
	Assert (lptkFunc->tkData.tkChar EQ UTF16_DOMINO);

	// If the right arg is a list, ...
	if (IsTknParList (lptkRhtArg))
		return PrimFnSyntaxError_EM (lptkFunc);

	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
		goto SYNTAX_EXIT;

	// Split cases based upon monadic or dyadic
	if (lptkLftArg EQ NULL)
		return PrimFnMonDomino_EM_YY			 (lptkFunc, lptkRhtArg, lptkAxis);
	else
		return PrimFnDydDomino_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
SYNTAX_EXIT:
	ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
							   lptkAxis);
	return NULL;
} // End PrimFnDomino_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	$PrimProtoFnDomino_EM_YY
//
//	Generate a prototype for the primitive functions monadic & dyadic Domino
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimProtoFnDomino_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnDomino_EM_YY
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	//***************************************************************
	// Called monadically or dyadically
	//***************************************************************

	// Convert to a prototype
	return PrimProtoFnMixed_EM_YY (&PrimFnDomino_EM_YY, // Ptr to primitive function routine
									lptkLftArg, 		// Ptr to left arg token
									lptkFunc,			// Ptr to function token
									lptkRhtArg, 		// Ptr to right arg token
									lptkAxis);			// Ptr to axis token (may be NULL)
} // End PrimProtoFnDomino_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	$PrimFnMonDomino_EM_YY
//
//	Primitive function for monadic Domino ("matrix inverse")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimFnMonDomino_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonDomino_EM_YY
	(LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	APLSTYPE	 aplTypeRht;		// Right arg storage type
	APLNELM 	 aplNELMRht,		// Right arg NELM
				 aplNELMRes;		// Result	 ...
	APLRANK 	 aplRankRht,		// Right arg rank
				 aplRankRes;		// Result	 ...
	HGLOBAL 	 hGlbRht = NULL,	// Right arg global memory handle
				 hGlbRes = NULL;	// Result	 ...
	LPVOID		 lpMemRht = NULL,	// Ptr to right arg global memory
				 lpMemRes = NULL;	// Ptr to result	...
	APLUINT 	 ByteRes;			// # bytes in the result
	APLDIM		 uNumRows,
				 uNumCols,
				 uRow,
				 uCol,
				 uTmp;
	APLINT		 apaOffRht, 		// Right arg APA offset
				 apaMulRht; 		// ...			 multiplier
	APLFLOAT	 aplFloatRht;		// Right arg temporary float
	LPPL_YYSTYPE lpYYRes = NULL;	// Ptr to the result
	UINT		 uBitMask;			// Bit mask for marching through Booleans
	gsl_matrix	*lpGslMatrixU = NULL,
				*lpGslMatrixV = NULL;
	gsl_vector	*lpGslVectorS = NULL,
				*lpGslVectorW = NULL;
	int 		 ErrCode;

	// Get the attributes (Type, NELM, and Rank)
	//	 of the right arg
	AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

	// Get right arg's global ptrs
	GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

	// Check for RANK ERROR
	if (aplRankRht > 2)
		goto RANK_EXIT;

	// Calculate the # rows & cols in the result
	switch (aplRankRht)
	{
		case 0:
			uNumRows =
			uNumCols = 1;
			GetFirstValueToken (lptkRhtArg, 	// Ptr to right arg token
								NULL,			// Ptr to integer result
							   &aplFloatRht,	// Ptr to float ...
								NULL,			// Ptr to WCHAR ...
								NULL,			// Ptr to longest ...
								NULL,			// Ptr to lpSym/Glb ...
								NULL,			// Ptr to ...immediate type ...
								NULL);			// Ptr to array type ...
			break;

		case 1:
			uNumRows = *VarArrayBaseToDim (lpMemRht);
			uNumCols = 1;

			break;

		case 2:
			uNumRows = (VarArrayBaseToDim (lpMemRht))[0];
			uNumCols = (VarArrayBaseToDim (lpMemRht))[1];

			break;

		defstop
			break;
	} // End SWITCH

	// Check for LENGTH ERROR
	if (aplRankRht EQ 2
	 && uNumRows < uNumCols)
		goto LENGTH_EXIT;

	// Check for DOMAIN ERROR
	if (!IsSimpleNum (aplTypeRht))
		goto DOMAIN_EXIT;

	// The rank of the result is the same as
	//	 max (rank of the right arg, 1)
	aplRankRes = max (aplRankRht, 1);

	// Save the NELM of the result
	Assert (aplNELMRht EQ (uNumRows * uNumCols));
////aplNELMRes = uNumRows * uNumCols;
	aplNELMRes = aplNELMRht;

	// Calculate space needed for the result
	ByteRes = CalcArraySize (ARRAY_FLOAT, aplNELMRes, aplRankRes);

	// Allocate space for the result
	// N.B.: Conversion from APLUINT to UINT.
	Assert (ByteRes EQ (UINT) ByteRes);
	hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
	if (!hGlbRes)
		goto WSFULL_EXIT;

	// Lock the memory to get a ptr to it
	lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader	((LPVARARRAY_HEADER) lpMemRes)
	// Fill in the header values
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = ARRAY_FLOAT;
////lpHeader->PermNdx	 = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar	 = 0;			// Already zero from GHND
	lpHeader->RefCnt	 = 1;
	lpHeader->NELM		 = aplNELMRes;
	lpHeader->Rank		 = aplRankRes;
#undef	lpHeader

	// Fill in the dimensions
	if (aplRankRes EQ 1)
		*VarArrayBaseToDim (lpMemRes) = uNumRows;
	else
	{
		// Reverse dimensions from right arg
		(VarArrayBaseToDim (lpMemRes))[0] = uNumCols;
		(VarArrayBaseToDim (lpMemRes))[1] = uNumRows;
	} // End IF/ELSE

	// Skip over the header and dimensions to the data
	if (aplRankRht NE 0)
		lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
	lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

	// No aborting on error!
	gsl_set_error_handler_off ();

	// Allocate space for the GSL matrices
	Assert (uNumRows EQ (UINT) uNumRows);
	Assert (uNumCols EQ (UINT) uNumCols);

	lpGslMatrixU = gsl_matrix_alloc  ((UINT) uNumRows, (UINT) uNumCols);	// M x N
	lpGslMatrixV = gsl_matrix_alloc  ((UINT) uNumCols, (UINT) uNumCols);	// N x N
	lpGslVectorS = gsl_vector_alloc  ((UINT) uNumCols); 					// N
	lpGslVectorW = gsl_vector_alloc  ((UINT) uNumCols); 					// N

	// Check the return codes for the above allocations
	if (GSL_ENOMEM EQ (int) lpGslMatrixU
	 || GSL_ENOMEM EQ (int) lpGslMatrixV
	 || GSL_ENOMEM EQ (int) lpGslVectorS
	 || GSL_ENOMEM EQ (int) lpGslVectorW)
		goto WSFULL_EXIT;

	// Copy the right arg to the GSL matrix U
	// Split cases based upon the right arg's rank
	switch (aplRankRht)
	{
		case 0: 		// Scalar
			lpGslMatrixU->data[0] = aplFloatRht;

			break;

		case 1: 		// Vector
		case 2: 		// Matrix
			// Split cases based upon the right arg storage type
			switch (aplTypeRht)
			{
				case ARRAY_BOOL:
					uBitMask = 0x01;

					for (uCol = 0; uCol < aplNELMRht; uCol++)
					{
						lpGslMatrixU->data[uCol] = (uBitMask & *(LPAPLBOOL) lpMemRht) ? 1 : 0;

						// Shift over the bit mask
						uBitMask <<= 1;

						// Check for end-of-byte
						if (uBitMask EQ END_OF_BYTE)
						{
							uBitMask = 0x01;			// Start over
							((LPAPLBOOL) lpMemRht)++;	// Skip to next byte
						} // End IF
					} // End FOR

					break;

				case ARRAY_INT:
					for (uCol = 0; uCol < aplNELMRht; uCol++)
						lpGslMatrixU->data[uCol] = (APLFLOAT) ((LPAPLINT) lpMemRht)[uCol];
					break;

				case ARRAY_FLOAT:
					ByteRes = aplNELMRes * sizeof (APLFLOAT);
					Assert (ByteRes EQ (UINT) ByteRes);
					Assert (sizeof (double) EQ sizeof (APLFLOAT));
					CopyMemory (lpGslMatrixU->data, lpMemRht, (UINT) ByteRes);
////////////////////for (uCol = 0; uCol < aplNELMRht; uCol++)
////////////////////	lpGslMatrixU->data[uCol] = ((LPAPLFLOAT) lpMemRht)[uCol];
					break;

				case ARRAY_APA:
#define lpAPA		((LPAPLAPA) lpMemRht)
					// Get the APA parameters
					apaOffRht = lpAPA->Off;
					apaMulRht = lpAPA->Mul;
#undef	lpAPA
					for (uCol = 0; uCol < aplNELMRht; uCol++)
						lpGslMatrixU->data[uCol] = (APLFLOAT) (APLINT) (apaOffRht + apaMulRht * uCol);
					break;

				defstop
					break;
			} // End SWITCH

			break;

		defstop
			break;
	} // End SWITCH

	// Calculate the SVD (Singular Value Decomposition)
	//	 with the result in U and S (S is the diagonal of the matrix)
	ErrCode = gsl_linalg_SV_decomp (lpGslMatrixU,		// M x N
									lpGslMatrixV,		// N x N
									lpGslVectorS,		// N
									lpGslVectorW);		// N
	// Check the error code
	if (ErrCode NE GSL_SUCCESS)
		goto DOMAIN_EXIT;

	// Free the GSL work vector
	gsl_vector_free (lpGslVectorW); lpGslVectorW = NULL;

//// #ifdef DEBUG
////	 // Display matrix U (M x N)
////	 dprintf ("=== Displaying Matrix U (M x N) = (%d, %d) ===", (UINT) uNumRows, (UINT) uNumCols);
////	 for (uRow = 0; uRow < uNumRows; uRow++)
////	 {
////		 char szTemp[128];
////		 char *p = szTemp;
////
////		 p += sprintf (p, "Row %d = ", uRow);
////
////		 for (uCol = 0; uCol < uNumCols; uCol++)
////			 p += sprintf (p, "%G, ", lpGslMatrixU->data[uRow * uNumCols + uCol]);
////		 p [-2] = '\0';
////		 DbgMsg (szTemp);
////	 } // End FOR
////
////	 // Display matrix V (N x N)
////	 dprintf ("=== Displaying Matrix V (N x N) = (%d, %d) ===", (UINT) uNumCols, (UINT) uNumCols);
////	 for (uRow = 0; uRow < uNumCols; uRow++)
////	 {
////		 char szTemp[128];
////		 char *p = szTemp;
////
////		 p += sprintf (p, "Row %d = ", uRow);
////
////		 for (uCol = 0; uCol < uNumCols; uCol++)
////			 p += sprintf (p, "%G, ", lpGslMatrixV->data[uRow * uNumCols + uCol]);
////		 p [-2] = '\0';
////		 DbgMsg (szTemp);
////	 } // End FOR
//// #endif

#define lpMemData	((LPAPLFLOAT) lpMemRes)

	// The solution is now V +.x (1/S) +.x Transpose (U)

	// Calculate U = (1/S) +.x U*
	// Actually, we calculate U = (1/S) +.x U
	//	 S	is of size N (logically, N x N because it is really an N x N diagonal matrix)
	//	 U	is of size M x N
	//	 U* is of size N x M
	// We reuse U but we don't transpose it so we can
	//	 multiply it by S in place.
	for (uCol = 0; uCol < uNumCols; uCol++) 	// 0 to N-1
	{
		APLFLOAT S = lpGslVectorS->data[uCol];

		for (uRow = 0; uRow < uNumRows; uRow++) // 0 to M-1
			lpGslMatrixU->data[uRow * uNumCols + uCol] /= S;
	} // End FOR

//// #ifdef DEBUG
////	 // Display matrix U (M x N)
////	 dprintf ("=== Displaying Matrix U (M x N) = (%d, %d) ===", (UINT) uNumRows, (UINT) uNumCols);
////	 for (uRow = 0; uRow < uNumRows; uRow++)
////	 {
////		 char szTemp[128];
////		 char *p = szTemp;
////
////		 p += sprintf (p, "Row %d = ", uRow);
////
////		 for (uCol = 0; uCol < uNumCols; uCol++)
////			 p += sprintf (p, "%G, ", lpGslMatrixU->data[uRow * uNumCols + uCol]);
////		 p [-2] = '\0';
////		 DbgMsg (szTemp);
////	 } // End FOR
//// #endif

	// Calculate V +.x U
	// Actually, we calculate V +.x U*
	//	 because we didn't transpose U above
	for (uRow = 0; uRow < uNumRows; uRow++) 	// 0 to M-1
	for (uCol = 0; uCol < uNumCols; uCol++) 	// 0 to N-1
	{
		APLFLOAT S = 0;

		for (uTmp = 0; uTmp < uNumCols; uTmp++) // 0 to N-1
			S += lpGslMatrixV->data[uCol * uNumCols + uTmp] * lpGslMatrixU->data[uRow * uNumCols + uTmp];

		lpMemData[uCol * uNumRows + uRow] = S;
	} // End FOR

#undef	lpMemData

	// Free the GSL vector
	gsl_vector_free (lpGslVectorS); lpGslVectorS = NULL;

	// Free the GSL matrices
	gsl_matrix_free (lpGslMatrixV); lpGslMatrixV = NULL;
	gsl_matrix_free (lpGslMatrixU); lpGslMatrixU = NULL;

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0; 	// Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	goto NORMAL_EXIT;

RANK_EXIT:
	ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
							   lptkFunc);
	goto ERROR_EXIT;

LENGTH_EXIT:
	ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
							   lptkFunc);
	goto ERROR_EXIT;

DOMAIN_EXIT:
	ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
							   lptkFunc);
	goto ERROR_EXIT;

WSFULL_EXIT:
	ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
							   lptkFunc);
	goto ERROR_EXIT;

ERROR_EXIT:
	if (hGlbRes)
	{
		if (lpMemRes)
		{
			// We no longer need this ptr
			MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
		} // End IF

		// We no longer need this storage
		FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
	} // End IF

	if (lpGslVectorW)
	{
		// We no longer need this storage and ptr
		gsl_vector_free (lpGslVectorW); lpGslVectorW = NULL;
	} // End IF

	if (lpGslVectorS)
	{
		// We no longer need this storage and ptr
		gsl_vector_free (lpGslVectorS); lpGslVectorS = NULL;
	} // End IF

	if (lpGslMatrixV)
	{
		// We no longer need this storage and ptr
		gsl_matrix_free (lpGslMatrixV); lpGslMatrixV = NULL;
	} // End IF

	if (lpGslMatrixU)
	{
		// We no longer need this storage and ptr
		gsl_matrix_free (lpGslMatrixU); lpGslMatrixU = NULL;
	} // End IF
NORMAL_EXIT:
	if (hGlbRes && lpMemRes)
	{
		// We no longer need this ptr
		MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
	} // End IF

	if (hGlbRht && lpMemRht)
	{
		// We no longer need this ptr
		MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
	} // End IF

	return lpYYRes;
} // End PrimFnMonDomino_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	$PrimFnDydDomino_EM_YY
//
//	Primitive function for dyadic Domino ("matrix divide")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimFnDydDomino_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydDomino_EM_YY
	(LPTOKEN lptkLftArg,			// Ptr to left arg token
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	APLSTYPE	 aplTypeLft,		// Left arg storage type
				 aplTypeRht;		// Right ...
	APLNELM 	 aplNELMLft,		// Left arg NELM
				 aplNELMRht,		// Right ...
				 aplNELMRes;		// Result	...
	APLRANK 	 aplRankLft,		// Left arg rank
				 aplRankRht,		// Right ...
				 aplRankRes;		// Result	...
	HGLOBAL 	 hGlbLft = NULL,	// Left arg global memory handle
				 hGlbRht = NULL,	// Right ...
				 hGlbRes = NULL;	// Result	...
	LPVOID		 lpMemLft = NULL,	// Ptr to left arg global memory
				 lpMemRht = NULL,	// Ptr to right ...
				 lpMemRes = NULL;	// Ptr to result   ...
	APLUINT 	 ByteRes;			// # bytes in the result
	APLDIM		 uNumRowsLft,		//
				 uNumColsLft,		//
				 uNumRowsRht,		//
				 uNumColsRht,		//
				 uNumRowsRes,		//
				 uNumColsRes,		//
				 uRow,				//
				 uCol;				//
	APLINT		 apaOffLft, 		// Left arg APA offset
				 apaMulLft, 		// ...			 multiplier
				 apaOffRht, 		// Right arg APA offset
				 apaMulRht; 		// ...			 multiplier
	APLFLOAT	 aplFloatLft,		//
				 aplFloatRht,		//
				 aplFloatRes;		//
	BOOL		 bRet = TRUE;		// TRUE iff result is valid
	LPPL_YYSTYPE lpYYRes;			// Ptr to the result
	UINT		 uBitMask;			// Bit mask for marching through Booleans
	gsl_matrix	*lpGslMatrixU = NULL,
				*lpGslMatrixV = NULL;
	gsl_vector	*lpGslVectorS = NULL,
				*lpGslVectorW = NULL,
				*lpGslVectorX = NULL,
			   *lpGslVectorB = NULL;
	int 		ErrCode;

	// Get the attributes (Type, NELM, and Rank)
	//	 of the left & right args
	AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
	AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

	// Get left and right arg's global ptrs
	GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
	GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

	// Check for RANK ERROR
	if (aplRankLft > 2 || aplRankRht > 2)
		goto RANK_EXIT;

	// Calculate the # rows & cols in the right arg
	switch (aplRankRht)
	{
		case 0:
			uNumRowsRht =
			uNumColsRht = 1;
			GetFirstValueToken (lptkRhtArg, 	// Ptr to right arg token
								NULL,			// Ptr to integer result
							   &aplFloatRht,	// Ptr to float ...
								NULL,			// Ptr to WCHAR ...
								NULL,			// Ptr to longest ...
								NULL,			// Ptr to lpSym/Glb ...
								NULL,			// Ptr to ...immediate type ...
								NULL);			// Ptr to array type ...
			break;

		case 1:
			uNumRowsRht = *VarArrayBaseToDim (lpMemRht);
			uNumColsRht = 1;

			break;

		case 2:
			uNumRowsRht = (VarArrayBaseToDim (lpMemRht))[0];
			uNumColsRht = (VarArrayBaseToDim (lpMemRht))[1];

			break;

		defstop
			break;
	} // End SWITCH

	// Calculate the # rows & cols in the left arg
	switch (aplRankLft)
	{
		case 0:
			uNumRowsLft =
			uNumColsLft = 1;
			GetFirstValueToken (lptkLftArg, 	// Ptr to left arg token
								NULL,			// Ptr to integer result
							   &aplFloatLft,	// Ptr to float ...
								NULL,			// Ptr to WCHAR ...
								NULL,			// Ptr to longest ...
								NULL,			// Ptr to lpSym/Glb ...
								NULL,			// Ptr to ...immediate type ...
								NULL);			// Ptr to array type ...
			break;

		case 1:
			uNumRowsLft = *VarArrayBaseToDim (lpMemLft);
			uNumColsLft = 1;

			break;

		case 2:
			uNumRowsLft = (VarArrayBaseToDim (lpMemLft))[0];
			uNumColsLft = (VarArrayBaseToDim (lpMemLft))[1];

			break;

		defstop
			break;
	} // End SWITCH

	// Check for LENGTH ERROR
	if (uNumRowsRht <  uNumColsRht
	 || uNumRowsLft NE uNumRowsRht)
		goto LENGTH_EXIT;

	// Check for DOMAIN ERROR
	if (!IsSimpleNum (aplTypeLft)
	 || !IsSimpleNum (aplTypeRht))
		goto DOMAIN_EXIT;

	// Save the # rows & cols in the result
	uNumRowsRes = uNumColsRht;
	uNumColsRes = uNumColsLft;

	// Save the rank of the result
	aplRankRes = aplRankLft + aplRankRht;
	aplRankRes = max (aplRankRes, 2) - 2;

	// Save the NELM of the result
	aplNELMRes = uNumRowsRes * uNumColsRes;

	// Calculate space needed for the result
	if (aplRankRes NE 0)
	{
		// Calculate space needed for the result
		ByteRes = CalcArraySize (ARRAY_FLOAT, aplNELMRes, aplRankRes);

		// Allocate space for the result
		// N.B.: Conversion from APLUINT to UINT.
		Assert (ByteRes EQ (UINT) ByteRes);
		hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
		if (!hGlbRes)
			goto WSFULL_EXIT;

		// Lock the memory to get a ptr to it
		lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader	((LPVARARRAY_HEADER) lpMemRes)
		// Fill in the header values
		lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
		lpHeader->ArrType	 = ARRAY_FLOAT;
////////lpHeader->PermNdx	 = PERMNDX_NONE;// Already zero from GHND
////////lpHeader->SysVar	 = 0;			// Already zero from GHND
		lpHeader->RefCnt	 = 1;
		lpHeader->NELM		 = aplNELMRes;
		lpHeader->Rank		 = aplRankRes;
#undef	lpHeader

		// Fill in the dimensions
		if (aplRankRes EQ 1)
			*VarArrayBaseToDim (lpMemRes) = uNumRowsRes;
		else
		if (aplRankRes EQ 2)
		{
			(VarArrayBaseToDim (lpMemRes))[0] = uNumRowsRes;
			(VarArrayBaseToDim (lpMemRes))[1] = uNumColsRes;
		} // End IF/ELSE
	} // End IF

	// Skip over the header and dimensions to the data
	if (aplRankLft NE 0)
		lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
	if (aplRankRht NE 0)
		lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
	if (aplRankRes NE 0)
		lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

	// No aborting on error!
	gsl_set_error_handler_off ();

	// Allocate space for the GSL matrices
	Assert (uNumRowsLft EQ (UINT) uNumRowsLft);
	Assert (uNumRowsRht EQ (UINT) uNumRowsRht);

	lpGslMatrixU = gsl_matrix_alloc  ((UINT) uNumRowsRht, (UINT) uNumColsRht);	// M x N
	lpGslMatrixV = gsl_matrix_alloc  ((UINT) uNumColsRht, (UINT) uNumColsRht);	// N x N
	lpGslVectorS = gsl_vector_alloc  ((UINT) uNumColsRht);						// N
	lpGslVectorW = gsl_vector_alloc  ((UINT) uNumColsRht);						// N
	lpGslVectorX = gsl_vector_alloc  ((UINT) uNumColsRht);						// N
	lpGslVectorB = gsl_vector_alloc  ((UINT) uNumRowsRht);						// M

	// Check the return codes for the above allocations
	if (GSL_ENOMEM EQ (int) lpGslMatrixU
	 || GSL_ENOMEM EQ (int) lpGslMatrixV
	 || GSL_ENOMEM EQ (int) lpGslVectorS
	 || GSL_ENOMEM EQ (int) lpGslVectorW
	 || GSL_ENOMEM EQ (int) lpGslVectorX)
		goto WSFULL_EXIT;

	// Copy the right arg to the GSL matrix U
	// Split cases based upon the right arg's rank
	switch (aplRankRht)
	{
		case 0: 		// Scalar
			lpGslMatrixU->data[0] = aplFloatRht;

			break;

		case 1: 		// Vector
		case 2: 		// Matrix
			// Split cases based upon the right arg storage type
			switch (aplTypeRht)
			{
				case ARRAY_BOOL:
					uBitMask = 0x01;

					for (uCol = 0; uCol < aplNELMRht; uCol++)
					{
						lpGslMatrixU->data[uCol] = (uBitMask & *(LPAPLBOOL) lpMemRht) ? 1 : 0;

						// Shift over the bit mask
						uBitMask <<= 1;

						// Check for end-of-byte
						if (uBitMask EQ END_OF_BYTE)
						{
							uBitMask = 0x01;			// Start over
							((LPAPLBOOL) lpMemRht)++;	// Skip to next byte
						} // End IF
					} // End FOR

					break;

				case ARRAY_INT:
					for (uCol = 0; uCol < aplNELMRht; uCol++)
						lpGslMatrixU->data[uCol] = (APLFLOAT) ((LPAPLINT) lpMemRht)[uCol];
					break;

				case ARRAY_FLOAT:
					ByteRes = aplNELMRht * sizeof (APLFLOAT);
					Assert (ByteRes EQ (UINT) ByteRes);
					Assert (sizeof (double) EQ sizeof (APLFLOAT));
					CopyMemory (lpGslMatrixU->data, lpMemRht, (UINT) ByteRes);
////////////////////for (uCol = 0; uCol < aplNELMRht; uCol++)
////////////////////	lpGslMatrixU->data[uCol] = ((LPAPLFLOAT) lpMemRht)[uCol];
					break;

				case ARRAY_APA:
#define lpAPA		((LPAPLAPA) lpMemRht)
					// Get the APA parameters
					apaOffRht = lpAPA->Off;
					apaMulRht = lpAPA->Mul;
#undef	lpAPA
					for (uCol = 0; uCol < aplNELMRht; uCol++)
						lpGslMatrixU->data[uCol] = (APLFLOAT) (APLINT) (apaOffRht + apaMulRht * uCol);
					break;

				defstop
					break;
			} // End SWITCH

			break;

		defstop
			break;
	} // End SWITCH

	// Calculate the SVD (Singular Value Decomposition)
	//	 with the result in U and S (S is the diagonal of the matrix)
	ErrCode = gsl_linalg_SV_decomp (lpGslMatrixU,		// M x N
									lpGslMatrixV,		// N x N
									lpGslVectorS,		// N
									lpGslVectorW);		// N
	// Check the error code
	if (ErrCode NE GSL_SUCCESS)
		goto DOMAIN_EXIT;

	// Free the GSL work vector
	gsl_vector_free (lpGslVectorW); lpGslVectorW = NULL;

	// The solution rows are now L[;I] +.x V +.x (1/S) +.x Transpose (U)
	for (uCol = 0; uCol < uNumColsLft; uCol++)
	{
		// Copy the next column from L to B
		if (aplRankLft EQ 0)
			lpGslVectorB->data[0] = aplFloatLft;
		else
		// Split cases based upon the left arg's storage type
		switch (aplTypeLft)
		{
			case ARRAY_BOOL:
				uBitMask = 0x01;

				Assert (uNumRowsLft EQ lpGslVectorB->size);
				for (uRow = 0; uRow < uNumRowsLft; uRow++)
				{
					lpGslVectorB->data[uRow] = (uBitMask & *(LPAPLBOOL) lpMemLft) ? 1 : 0;

					// Shift over the bit mask
					uBitMask <<= 1;

					// Check for end-of-byte
					if (uBitMask EQ END_OF_BYTE)
					{
						uBitMask = 0x01;			// Start over
						((LPAPLBOOL) lpMemLft)++;	// Skip to next byte
					} // End IF
				} // End FOR

				break;

			case ARRAY_INT:
				Assert (uNumRowsLft EQ lpGslVectorB->size);
				for (uRow = 0; uRow < uNumRowsLft; uRow++)
					lpGslVectorB->data[uRow] = (APLFLOAT) ((LPAPLINT) lpMemLft)[uRow * uNumColsLft + uCol];
				break;

			case ARRAY_FLOAT:
				Assert (uNumRowsLft EQ lpGslVectorB->size);
				for (uRow = 0; uRow < uNumRowsLft; uRow++)
					lpGslVectorB->data[uRow] = ((LPAPLFLOAT) lpMemLft)[uRow * uNumColsLft + uCol];
				break;

			case ARRAY_APA:
				Assert (uNumRowsLft EQ lpGslVectorB->size);
#define lpAPA		((LPAPLAPA) lpMemLft)
				// Get the APA parameters
				apaOffLft = lpAPA->Off;
				apaMulLft = lpAPA->Mul;
#undef	lpAPA
				for (uRow = 0; uRow < uNumRowsLft; uRow++)
					lpGslVectorB->data[uRow] = (APLFLOAT) (APLINT) (apaOffLft + apaMulLft * (uRow * uNumColsLft + uCol));
				break;

			defstop
				break;
		} // End SWITCH

		// Call GSL function to solve for this column
		ErrCode = gsl_linalg_SV_solve (lpGslMatrixU,
									   lpGslMatrixV,
									   lpGslVectorS,
									   lpGslVectorB,
									   lpGslVectorX);
		// Check the error code
		if (ErrCode NE GSL_SUCCESS)
			goto DOMAIN_EXIT;

#define lpMemData	((LPAPLFLOAT) lpMemRes)

		// Copy X to the next column in the result
		Assert (uNumRowsRes EQ lpGslVectorX->size);
		if (aplRankRes EQ 0)
			aplFloatRes = lpGslVectorX->data[0];
		else
		for (uRow = 0; uRow < uNumRowsRes; uRow++)
			lpMemData[uRow * uNumColsRes + uCol] = lpGslVectorX->data[uRow];

#undef	lpMemData
	} // End FOR

	// Free the GSL vectors
	gsl_vector_free (lpGslVectorB); lpGslVectorB = NULL;
	gsl_vector_free (lpGslVectorX); lpGslVectorX = NULL;
	gsl_vector_free (lpGslVectorS); lpGslVectorS = NULL;

	// Free the GSL matrices
	gsl_matrix_free (lpGslMatrixV); lpGslMatrixV = NULL;
	gsl_matrix_free (lpGslMatrixU); lpGslMatrixU = NULL;

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	if (aplRankRes EQ 0)
	{
		lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
		lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_FLOAT;
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
		lpYYRes->tkToken.tkData.tkFloat    = aplFloatRes;
		lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;
	} else
	{
		lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0; 	// Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
		lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
		lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;
	} // End IF/ELSE

	goto NORMAL_EXIT;

RANK_EXIT:
	ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
							   lptkFunc);
	goto ERROR_EXIT;

LENGTH_EXIT:
	ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
							   lptkFunc);
	goto ERROR_EXIT;

DOMAIN_EXIT:
	ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
							   lptkFunc);
	goto ERROR_EXIT;

WSFULL_EXIT:
	ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
							   lptkFunc);
	goto ERROR_EXIT;

ERROR_EXIT:
	// Mark as in error
	bRet = FALSE;

	if (lpGslVectorB)
	{
		// We no longer need this storage and ptr
		gsl_vector_free (lpGslVectorB); lpGslVectorB = NULL;
	} // End IF

	if (lpGslVectorX)
	{
		// We no longer need this storage and ptr
		gsl_vector_free (lpGslVectorX); lpGslVectorX = NULL;
	} // End IF

	if (lpGslVectorW)
	{
		// We no longer need this storage and ptr
		gsl_vector_free (lpGslVectorW); lpGslVectorW = NULL;
	} // End IF

	if (lpGslVectorS)
	{
		// We no longer need this storage and ptr
		gsl_vector_free (lpGslVectorS); lpGslVectorS = NULL;
	} // End IF

	if (lpGslMatrixV)
	{
		// We no longer need this storage and ptr
		gsl_matrix_free (lpGslMatrixV); lpGslMatrixV = NULL;
	} // End IF

	if (lpGslMatrixU)
	{
		// We no longer need this storage and ptr
		gsl_matrix_free (lpGslMatrixU); lpGslMatrixU = NULL;
	} // End IF
NORMAL_EXIT:
	if (hGlbRes)
	{
		if (lpMemRes)
		{
			// We no longer need this ptr
			MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
		} // End IF

		if (!bRet)
		{
			// We no longer need this storage
			FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
		} // End IF
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

	if (bRet)
		return lpYYRes;
	else
		return NULL;
} // End PrimFnDydDomino_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	End of File: pf_domino.c
//***************************************************************************
