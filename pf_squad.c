//***************************************************************************
//	NARS2000 -- Primitive Function -- Squad
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//	PrimFnSquad_EM
//
//	Primitive function for monadic and dyadic Squad (ERROR and "index")
//***************************************************************************

LPYYSTYPE PrimFnSquad_EM
	(LPTOKEN lptkLftArg,
	 LPTOKEN lptkFunc,
	 LPTOKEN lptkRhtArg,
	 LPTOKEN lptkAxis)

{
	// Ensure not an overflow function
	Assert (lptkFunc->tkData.tkChar EQ UCS2_SQUAD);

	// Split cases based upon monadic or dyadic
	if (lptkLftArg EQ NULL)
		return PrimFnMonSquad_EM			 (lptkFunc, lptkRhtArg, lptkAxis);
	else
		return PrimFnDydSquad_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnSquad_EM


//***************************************************************************
//	PrimFnMonSquad_EM
//
//	Primitive function for monadic Squad (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimFnMonSquad_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonSquad_EM
	(LPTOKEN lptkFunc,
	 LPTOKEN lptkRhtArg,
	 LPTOKEN lptkAxis)

{
	return PrimFnSyntaxError_EM (lptkFunc);
} // End PrimFnMonSquad_EM
#undef	APPEND_NAME


//***************************************************************************
//	PrimFnDydSquad_EM
//
//	Primitive function for dyadic Squad ("index") ("scatter indexing")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimFnDydSquad_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDydSquad_EM
	(LPTOKEN lptkLftArg,
	 LPTOKEN lptkFunc,
	 LPTOKEN lptkRhtArg,
	 LPTOKEN lptkAxis)

{
	APLLONGEST aplLongestRht;
	APLSTYPE   aplTypeLft,		// The storage type of the left arg
			   aplTypeRht;		// ...					   right ...
	APLNELM    aplNELMLft,		// The # elements in the left arg
			   aplNELMRht;		// ...					 right ...
	APLRANK    aplRankLft,		// The rank of the left arg
			   aplRankRht;		// ...			   right ...
	UINT	   YYLclIndex;

	// Split cases based upon the right arg's token type
	switch (lptkRhtArg->tkFlags.TknType)
	{
		case TKT_VARNAMED:
			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lptkRhtArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

			// If it's not immediate, we must look inside the array
			if (!lptkRhtArg->tkData.lpSym->stFlags.Imm)
			{
				// stData is a valid HGLOBAL variable array
				Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.lpSym->stData.stGlbData));

				return PrimFnDydSquadGlb_EM (lptkLftArg,
											 ClrPtrTypeDirGlb (lptkRhtArg->tkData.lpSym->stData.stGlbData),
											 lptkAxis,
											 lptkFunc);
			} // End IF

			// Handle the immediate case

			// Get the immediate value
			aplLongestRht = lptkRhtArg->tkData.lpSym->stData.stLongest;

			break;

		case TKT_VARIMMED:
			// Get the immediate value
			aplLongestRht = lptkRhtArg->tkData.tkLongest;

			break;


		case TKT_VARARRAY:
			// tkData is a valid HGLOBAL variable array
			Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

			return PrimFnDydSquadGlb_EM (lptkLftArg,
										 ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkGlbData),
										 lptkAxis,
										 lptkFunc);
		case TKT_LISTPAR:
			ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
									   lptkFunc);
			return NULL;

		defstop
			return NULL;
	} // End SWITCH

	// Common immediate case, value in <aplLongestRht>

	// The only allowed left arg is an empty vector

	// Get the attributes (Type, NELM, and Rank) of the left & right args
	AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
	AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

	// Check for RANK ERROR
	if (aplRankLft NE 1)
	{
		ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Check for LENGTH ERROR
	if (aplNELMLft NE 0)
	{
		ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Check for DOMAIN ERROR
	if (!IsSimpleNH (aplTypeLft))
	{
		ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Get new index into YYRes
	YYLclIndex = NewYYResIndex ();

	// Fill in the result token
	YYRes[YYLclIndex].tkToken.tkFlags.TknType	= TKT_VARIMMED;
	YYRes[YYLclIndex].tkToken.tkFlags.ImmType	= aplTypeRht;
////YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;	// Already zero from ZeroMemory
	YYRes[YYLclIndex].tkToken.tkData.tkLongest	= aplLongestRht;
	YYRes[YYLclIndex].tkToken.tkCharIndex		= lptkFunc->tkCharIndex;

	return &YYRes[YYLclIndex];
} // End PrimFnDydSquad_EM
#undef	APPEND_NAME


//***************************************************************************
//	PrimFnDydSquadGlb_EM
//
//	Dyadic Squad ("index") ("scatter indexing") on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimFnDydSquadGlb_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDydSquadGlb_EM
	(LPTOKEN lptkLftArg,
	 HGLOBAL hGlbRht,
	 LPTOKEN lptkAxis,
	 LPTOKEN lptkFunc)

{

	APLSTYPE aplTypeLft,		// The storage type of the left arg
			 aplTypeRht;		// ...					   right ...
	APLNELM  aplNELMLft,		// The # elements in left arg
			 aplNELMRht;		// ...				 right ...
	APLRANK  aplRankLft,		// The rank of the left arg
			 aplRankRht;		// ...			   right ...
	HGLOBAL  hGlbLft = NULL;
	LPVOID	 lpMemLft = NULL,
			 lpMemRht = NULL;
	BOOL	 bRet = TRUE;
	UINT	 YYLclIndex;

	// Get new index into YYRes
	YYLclIndex = NewYYResIndex ();

	// Get the attributes (Type, NELM, and Rank) of the left & right args
	AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
	AttrsOfGlb	 (hGlbRht	, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

	// Get left and right arg's global ptrs
	GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
	lpMemRht = MyGlobalLock (hGlbRht);

	// Check for axis present
	if (lptkAxis NE NULL)
	{
		// Check the axis values, fill in # elements in axis
		if (!CheckAxis_EM (lptkAxis,		// The axis token
						   aplRankRht,		// All values less than this
						   FALSE,			// TRUE iff scalar or one-element vector only
						   TRUE,			// TRUE iff want sorted axes
						   FALSE,			// TRUE iff axes must be contiguous
						   FALSE,			// TRUE iff duplicate axes are allowed
						   NULL,			// TRUE iff fractional values allowed
						   NULL,			// Return last axis value
						   NULL,			// Return # elements in axis vector
						   NULL))			// Return HGLOBAL with APLINT axis values
			return NULL;
	} // End IF/ELSE

	DbgBrk ();				// ***FINISHME***

	// Split cases based upon the

















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
		return &YYRes[YYLclIndex];
	else
		return NULL;
} // End PrimFnDydSquadGlb_EM
#undef	APPEND_NAME


//***************************************************************************
//	ArrayIndex_EM
//
//	Index of an array, e.g., L[R]  or  L[R1;...;Rn]
//	The form L[R] can use either (or both) Reach and/or Scatter indexing,
//	  whereas the form L[R1;...;Rn] can use rectangular indexing only.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ArrayIndex_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE ArrayIndex_EM
	(LPTOKEN lptkLftArg,
	 LPTOKEN lptkRhtArg)

{
	APLSTYPE	aplTypeLft, 		// The storage type of the left arg
				aplTypeRht, 		// ...					   right ...
				aplTypeRch, 		// ...					   reach ...
				aplTypeInd, 		// ...					   index ...
				aplTypeRes; 		// ...					   result
	APLNELM 	aplNELMLft, 		// The # elements in left arg
				aplNELMRht, 		// ...				 right ...
				aplNELMRch, 		// ...				 reach ...
				aplNELMInd, 		// ...				 index ...
				aplNELMRes; 		// ...				 result
	APLRANK 	aplRankLft, 		// The rank of the left arg
				aplRankRht, 		// ...			   right ...
				aplRankRch, 		// ...			   reach ...
				aplRankInd, 		// ...			   index ...
				aplRankRes, 		// ...			   result
				aplRankData;		// ...			   data  ...
	HGLOBAL 	hGlbLft = NULL, 	// Global memory handle to the left arg
				hGlbRht = NULL, 	// ...						   right ...
				hGlbRes = NULL, 	// ...						   result
				hGlbRch = NULL, 	// ...						   reach ...
				hGlbData;			// ...						   data  ...
	LPVOID		lpMemLft = NULL,	// Ptr to contents of the left arg
				lpMemRht = NULL,	// ...					  right ...
				lpMemRch = NULL,	// ...					  reach ...
				lpMemRes = NULL,	// ...					  result
				lpMemData;			// ...					  data	...
	LPAPLDIM	lpMemDimLft;		// Ptr to the dimensions of the left arg
	LPAPLNESTED lpMemInd;			// Ptr to contents of the index arg
	BOOL		bRet = TRUE;
	UINT		YYLclIndex;
	APLNELM 	uRht,
				uDim,
				uRes;
	APLUINT 	ByteRes;

	// Get new index into YYRes
	YYLclIndex = NewYYResIndex ();

	// Get the attributes (Type, NELM, and Rank) of the left & right args
	AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
	AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

	// Map APA left arg to INT result
	if (aplTypeLft EQ ARRAY_APA)
		aplTypeRes = ARRAY_INT;
	else
		aplTypeRes = aplTypeLft;

	// Get left and right arg's global ptrs
	GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
	GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

	// Skip over the header and dimensions to the data
	lpMemInd = VarArrayBaseToData (lpMemRht, aplRankRht);

	//***************************************************************
	// Calculate the NELM and rank of the result
	//***************************************************************
	if (aplNELMRht EQ 1 			// Single array index matches all ranks
	 && aplTypeLft EQ ARRAY_NESTED) // and it's Reach Indexing
	{
		DbgBrk ();

		// Get the global memory handle of the single index array
		hGlbRch = ClrPtrTypeIndGlb (lpMemInd);

		// Get the attributes (Type, NELM, and Rank) of the next index array
		AttrsOfGlb (hGlbRch, &aplTypeRch, &aplNELMRch, &aplRankRch, NULL);

		// Save the NELM and rank
		aplNELMRes = aplNELMRch;
		aplRankRes = aplRankRch;
	} else
	{
		DbgBrk ();

		// Check for RANK ERROR
		if (aplNELMRht NE aplRankLft)
		{
			ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
									   lptkLftArg);
			goto ERROR_EXIT;
		} // End IF

		DbgBrk ();

		// Loop through the elements of the right arg
		aplRankRes = 0; aplNELMRes = 1;
		for (uRht = 0; uRht < aplNELMRht; uRht++, lpMemInd++)
		{
			// Split cases based upon the ptr type of the element
			switch (GetPtrTypeInd (lpMemInd))
			{
				case PTRTYPE_STCONST:		// Skip over scalars
					break;

				case PTRTYPE_HGLOBAL:
					// Get the attributes (Type, NELM, and Rank) of the next index array
					AttrsOfGlb (ClrPtrTypeIndGlb (lpMemInd), &aplTypeInd, &aplNELMInd, &aplRankInd, NULL);

					// Check for DOMAIN ERROR
					if (!IsSimpleNum (aplTypeInd))
					{
						ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
												   lptkLftArg);
						goto ERROR_EXIT;
					} // End IF

					// Accumulate into NELM and rank
					aplNELMRes *= aplNELMInd;
					aplRankRes += aplRankInd;

					break;

				defstop
					break;
			} // End SWITCH
		} // End FOR
	} // End IF/ELSE

	//***************************************************************
	// Calculate space needed for the result
	//***************************************************************
	ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

	//***************************************************************
	// Allocate space for the result
	// N.B.:  Conversion from APLUINT to UINT.
	//***************************************************************
	Assert (ByteRes EQ (UINT) ByteRes);
	hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
	if (!hGlbRes)
	{
		ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
								   lptkLftArg);
		goto ERROR_EXIT;
	} // End IF

	// Lock the memory to get a ptr to it
	lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader	((LPVARARRAY_HEADER) lpMemRes)

	// Fill in the header
	lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = aplTypeRes;
////lpHeader->Perm		 = 0;			// Already zero from GHND
////lpHeader->SysVar	 = 0;			// Already zero from GHND
	lpHeader->RefCnt	 = 1;
	lpHeader->NELM		 = aplNELMRes;
	lpHeader->Rank		 = aplRankRes;

#undef	lpHeader

	// Skip over the header to the dimensions
	lpMemRes = VarArrayBaseToDim (lpMemRes);

	//***************************************************************
	// Copy the appropriate dimensions from the right arg
	//	 to the result's dimension
	//***************************************************************
	if (aplNELMRht EQ 1 			// Single array index matches all ranks
	 && aplTypeLft EQ ARRAY_NESTED) // and it's Reach Indexing
	{
		DbgBrk ();

		// Lock the memory to get a ptr to it
		lpMemRch = MyGlobalLock (hGlbRch);

		// Skip over the header to the dimensions
		lpMemRch = VarArrayBaseToDim (lpMemRch);

		// Loop through the dimensions of the right arg
		for (uDim = 0; uDim < aplRankRch; uDim++)
			*((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRch)++;

		// lpMemRch now points to the data
		// lpMemRes now points to the data
	} else
	{
		// Skip over the header and dimensions to the data
		lpMemInd = VarArrayBaseToData (lpMemRht, aplRankRht);

		// Loop through the elements of the right arg
		for (uRht = 0; uRht < aplNELMRht; uRht++, lpMemInd++)
		{
			// Split cases based upon the ptr type of the element
			switch (GetPtrTypeInd (lpMemInd))
			{
				case PTRTYPE_STCONST:		// Skip over scalars
					break;

				case PTRTYPE_HGLOBAL:
					// Get the HGLOBAL of the next index array
					hGlbData = ClrPtrTypeIndGlb (lpMemInd);

					// Lock the memory to get a ptr to it
					lpMemData = MyGlobalLock (hGlbData);

					// Get the rank of the element
					aplRankData = ((LPVARARRAY_HEADER) lpMemData)->Rank;

					// Skip over the header to the dimensions
					lpMemData = VarArrayBaseToDim (lpMemData);

					// Loop through the dimensions of the element
					for (uDim = 0; uDim < aplRankData; uDim++)
						*((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemData)++;

					// We no longer need this ptr
					MyGlobalUnlock (hGlbData); lpMemData = NULL;

					break;

				defstop
					break;
			} // End SWITCH
		} // End FOR

		// lpMemRes now points to the data
	} // End IF/ELSE

	//***************************************************************
	// Fill nested result with PTR_REUSED in case we fail mid-copy
	//***************************************************************
	if (aplTypeRes EQ ARRAY_NESTED)
	for (uRes = 0; uRes < aplNELMRes; uRes++)
		((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;

	// Skip over the header to the dimensions
	lpMemDimLft = VarArrayBaseToDim (lpMemLft);

	// Skip over the header and dimensions to the data
	lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

	//***************************************************************
	// Copy the data to the result
	//***************************************************************
	if (aplNELMRht EQ 1 			// Single array index matches all ranks
	 && aplTypeLft EQ ARRAY_NESTED) // and it's Reach Indexing
	{
		DbgBrk ();

		// Loop through the reach arg
		for (uRht = 0; bRet && uRht < aplNELMRch; uRht++, ((LPAPLNESTED) lpMemRes)++)
			// Recurse through the left & reach args
			bRet =
			ReachAcross_EM (lpMemRes,							// Put the result here
						  &((LPAPLNESTED) lpMemRch)[uRht],		// The current reach arg
						   aplTypeRch,							// The storage type of the reach arg
						   lpMemLft,							// Get the results from here
						   aplTypeLft,							// The storage type of the left arg
						   aplRankLft,							// The rank of the left arg
						   lpMemDimLft, 						// Ptr to the dimensions of the left arg
						   lptkLftArg); 						// Ptr to "function" token
		if (!bRet)
			goto ERROR_EXIT;
	} else
	{
		DbgBrk ();

		// Rectangular indexing (Scatter indexing if aplNELMRht EQ 1)

		// Split cases based upon the left arg storage type
		switch (aplTypeLft)
		{
			case ARRAY_BOOL:







				break;

			case ARRAY_INT:







				break;

			case ARRAY_FLOAT:







				break;

			case ARRAY_CHAR:







				break;

			case ARRAY_APA:







				break;

			case ARRAY_HETERO:







				break;

			case ARRAY_NESTED:







				break;

			defstop
				break;
		} // End SWITCH
	} // End IF/ELSE

	// Fill in the result token
	YYRes[YYLclIndex].tkToken.tkFlags.TknType	= TKT_VARARRAY;
////YYRes[YYLclIndex].tkToken.tkFlags.ImmType	= 0;	// Already zero from ZeroMemory
////YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;	// Already zero from ZeroMemory
	YYRes[YYLclIndex].tkToken.tkData.tkGlbData	= MakeGlbTypeGlb (TypeDemote (hGlbRes));
	YYRes[YYLclIndex].tkToken.tkCharIndex		= lptkLftArg->tkCharIndex;

	goto NORMAL_EXIT;

ERROR_EXIT:
	bRet = FALSE;

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
NORMAL_EXIT:
	if (hGlbRch && lpMemRch)
	{
		// We no longer need this ptr
		MyGlobalUnlock (hGlbRch); lpMemRch = NULL;
	} // End IF

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

	if (hGlbLft && lpMemLft)
	{
		// We no longer need this ptr
		MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
	} // End IF

	if (bRet)
		return &YYRes[YYLclIndex];
	else
		return NULL;
} // End ArrayIndex_EM
#undef	APPEND_NAME


//***************************************************************************
//	ReachAcross_EM
//
//	Reach across the reach arg for each element of the result.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ReachAcross_EM"
#else
#define APPEND_NAME
#endif

BOOL ReachAcross_EM
	(LPAPLNESTED lpMemRes,		// Put the result here
	 LPVOID 	 lpMemRch,		// The current reach arg
	 APLSTYPE	 aplTypeRch,	// The storage type of the reach arg
	 LPVOID 	 lpMemLft,		// Get the results from here
	 APLSTYPE	 aplTypeLft,	// The storage type of the left arg
	 APLRANK	 aplRankLft,	// The rank of the left arg
	 LPAPLDIM	 lpMemDimLft,	// Ptr to the dimensions of the left arg
	 LPTOKEN	 lptkFunc)		// Pt to function token

{
	BOOL	   bRet = TRUE;
	LPVOID	   lpMemSub;
	APLSTYPE   aplTypeSub;
	APLNELM    aplNELMSub;
	APLRANK    aplRankSub;

	DbgBrk ();

	// Split cases based upon the ptr type of the reach arg
	switch (GetPtrTypeInd (lpMemRch))
	{
		case PTRTYPE_STCONST:
			bRet =
			ReachSymTabConst_EM (lpMemRes,			// Put the result here
								 lpMemRch,			// Ptr to the reach symbol table const
								 lpMemLft,			// Get the results from here
								 aplTypeLft,		// The storage type of the left arg
								 aplRankLft,		// The rank of the left arg
								 lpMemDimLft,		// Ptr to the dimensions of the left arg
								 lptkFunc); 		// Ptr to the function token
			break;

#undef	lpSymRch

		case PTRTYPE_HGLOBAL:
			// The global should point to a scalar or a vector of scalars or vectors
			//	 which serves as the reach index into the left arg

			// Lock the memory to get a ptr to it
			lpMemSub = MyGlobalLock (ClrPtrTypeIndGlb (lpMemRch));

#define lpHeader		((LPVARARRAY_HEADER) lpMemSub)

			aplTypeSub = lpHeader->ArrType;
			aplNELMSub = lpHeader->NELM;
			aplRankSub = lpHeader->Rank;

#undef	lpHeader

			// Check for RANK ERROR
			if (aplRankSub > 1)
			{
				ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
										   lptkFunc);
				bRet = FALSE;
			} else
			{
				DbgBrk ();

				// Skip over the header and dimensions to the data
				lpMemSub = VarArrayBaseToData (lpMemSub, aplRankSub);

				// Reach down through the left arg using successive
				//	 elements of the reach arg as indices
				bRet =
				ReachDown_EM (lpMemRes, 			// Put the result here
							  lpMemSub, 			// Ptr to the reach arg
							  aplNELMSub,			// The NELM of the reach arg
							  lpMemLft, 			// Get the results from here
							  aplTypeLft,			// The storage type of the left arg
							  aplRankLft,			// The rank of the left arg
							  lpMemDimLft,			// Ptr to the dimensions of the left arg
							  lptkFunc);			// Ptr to function token
			} // End IF/ELSE

			// We no longer need this ptr
			MyGlobalUnlock (ClrPtrTypeIndGlb (lpMemRch)); lpMemSub = NULL;

			break;

		defstop
			break;
	} // End SWITCH

	return bRet;
} // End ReachAcross_EM
#undef	APPEND_NAME


//***************************************************************************
//	ReachSymTabConst_EM
//
//	Index the left arg with a symbol table constant
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ReachSymTabConst_EM"
#else
#define APPEND_NAME
#endif

BOOL ReachSymTabConst_EM
	(LPVOID   lpMemRes, 		// Put the result here
	 LPVOID   lpMemRch, 		// Ptr to the reach symbol table constant
	 LPVOID   lpMemLft, 		// Get the results from here
	 APLSTYPE aplTypeLft,		// The storage type of the left arg
	 APLRANK  aplRankLft,		// The rank of the left arg
	 LPAPLDIM lpMemDimLft,		// Ptr to the dimensions of the left arg
	 LPTOKEN  lptkFunc) 		// Ptr to the function token

{
	APLINT aplInteger;
	BOOL   bRet = TRUE;

	// Check for RANK ERROR
	if (aplRankLft NE 1)
	{
		ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
								   lptkFunc);
		return FALSE;
	} // End IF

#define lpSymRch		(*(LPSYMENTRY *) lpMemRch)

	// Split cases based upon the immediate type of the reach arg
	switch (lpSymRch->stFlags.ImmType)
	{
		case IMMTYPE_BOOL:
			aplInteger = lpSymRch->stData.stBoolean;

			break;

		case IMMTYPE_INT:
			aplInteger = lpSymRch->stData.stInteger;

			break;

		case IMMTYPE_FLOAT:
			aplInteger = FloatToAplint_SCT (lpSymRch->stData.stFloat, &bRet);
			if (bRet)
				break;

			// Otherwise, fall through to common DOMAIN ERROR

		case IMMTYPE_CHAR:
			ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
									   lptkFunc);
			return FALSE;

		defstop
			break;
	} // End SWITCH

	// Convert to origin-0
	aplInteger -= bQuadIO;

	// Check for INDEX ERROR
	if (aplInteger < 0
	 || aplInteger >= (APLINT) lpMemDimLft[0])
	{
		ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
								   lptkFunc);
		return FALSE;
	} // End IF

	// Use aplInteger to index lpMemLft
	bRet =
	ValueAsSymentry (lpMemRes,
					 lpMemLft,
					 aplTypeLft,
					 aplInteger,
					 lptkFunc);
	return bRet;
} // End ReachSymTabConst_EM
#undef	APPEND_NAME


//***************************************************************************
//	ValueAsSymentry
//
//	Extract a particular value from an array as a LPSYMENTRY
//***************************************************************************

BOOL ValueAsSymentry
	(LPVOID   lpMemRes, 	// Put the result here
	 LPVOID   lpMemLft, 	// Get the result from here
	 APLSTYPE aplTypeLft,	// The storage type of the arg
	 APLINT   aplInteger,	// The index to use
	 LPTOKEN  lptkFunc) 	// The function token

{
	APLINT	   apaOff,
			   apaMul;
	APLLONGEST aplVal;

   // Split cases based upon the storage type of the left arg
   switch (aplTypeLft)
   {
	   case ARRAY_BOOL:
		   aplVal = ((1 << (aplInteger % NBIB)) & ((LPAPLBOOL) lpMemLft)[aplInteger >> LOG2NBIB]) ? 1 : 0;
		   *((LPAPLNESTED) lpMemRes) =
			 MakeSymEntry_EM (IMMTYPE_BOOL,
							 &aplVal,
							  lptkFunc);
		   break;

	   case ARRAY_INT:
		   aplVal = ((LPAPLINT) lpMemLft)[aplInteger];
		   *((LPAPLNESTED) lpMemRes) =
			 MakeSymEntry_EM (IMMTYPE_INT,
							 &aplVal,
							  lptkFunc);
		   break;

	   case ARRAY_FLOAT:
		   aplVal = *(LPAPLLONGEST) &((LPAPLFLOAT) lpMemLft)[aplInteger];
		   *((LPAPLNESTED) lpMemRes) =
			 MakeSymEntry_EM (IMMTYPE_FLOAT,
							 &aplVal,
							  lptkFunc);
		   break;

	   case ARRAY_CHAR:
		   aplVal = ((LPAPLCHAR) lpMemLft)[aplInteger];
		   *((LPAPLNESTED) lpMemRes) =
			 MakeSymEntry_EM (IMMTYPE_CHAR,
							 &aplVal,
							  lptkFunc);
		   break;

	   case ARRAY_APA:

#define lpaplAPA		((LPAPLAPA) lpMemLft)

			apaOff = lpaplAPA->Off;
			apaMul = lpaplAPA->Mul;

			aplVal = apaOff + apaMul * aplInteger;
			*((LPAPLNESTED) lpMemRes) =
			  MakeSymEntry_EM (IMMTYPE_INT,
							  &aplVal,
							   lptkFunc);
			break;

#undef	lpaplAPA

		case ARRAY_HETERO:
		case ARRAY_NESTED:
			*((LPAPLNESTED) lpMemRes) = CopySymGlbDir (((LPAPLNESTED) lpMemLft)[aplInteger]);

			break;

		defstop
			break;
	} // End SWITCH

	return TRUE;
} // End ValueAsSymentry


//***************************************************************************
//	ReachDown_EM
//
//	Reach down recursively through the reach & left args
//	  to put the reached for item into the result.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ReachDown_EM"
#else
#define APPEND_NAME
#endif

BOOL ReachDown_EM
	(LPAPLNESTED lpMemRes,		// Put the result here
	 LPVOID 	 lpMemRch,		// The current reach arg
	 APLNELM	 aplNELMRch,	// The NELM of the reach arg
	 LPVOID 	 lpMemLft,		// Get the results from here
	 APLSTYPE	 aplTypeLft,	// The storage type of the left arg
	 APLRANK	 aplRankLft,	// The rank of the left arg
	 LPAPLDIM	 lpMemDimLft,	// Ptr to the dimensions of the left arg
	 LPTOKEN	 lptkFunc)		// Ptr to function token

{
	BOOL	 bRet = TRUE;
	LPVOID	 lpMemSub;
	APLSTYPE aplTypeSub;
	APLNELM  aplNELMSub;
	APLRANK  aplRankSub;
	APLINT	 iRch,
			 aplWeight,
			 aplIndex,
			 aplInteger,
			 apaOff,
			 apaMul;

	DbgBrk ();

	// The global should point to a scalar or a vector of scalars or vectors
	//	 which serves as the reach index into the left arg

	// Stop when no more elements remain
	if (aplNELMRch EQ 0)
		*lpMemRes = CopySymGlbInd (lpMemLft);
	else
	{
		// Use the first element of lpMemSub to index lpMemLft

		// Split cases based upon the ptr type of the reach element
		switch (GetPtrTypeInd (lpMemRch))
		{
			case PTRTYPE_STCONST:
				bRet =
				ReachSymTabConst_EM (lpMemRes,			// Put the result here
									 lpMemRch,			// Ptr to the reach symbol table const
									 lpMemLft,			// Get the results from here
									 aplTypeLft,		// The storage type of the left arg
									 aplRankLft,		// The rank of the left arg
									 lpMemDimLft,		// Ptr to the dimensions of the left arg
									 lptkFunc); 		// Ptr to the function token
				break;

			case PTRTYPE_HGLOBAL:
				// Lock the memory to get a ptr to it
				lpMemSub = MyGlobalLock (ClrPtrTypeIndGlb (lpMemRch));

#define lpHeader		((LPVARARRAY_HEADER) lpMemSub)

				aplTypeSub = lpHeader->ArrType;
				aplNELMSub = lpHeader->NELM;
				aplRankSub = lpHeader->Rank;

#undef	lpHeader

				// Check for RANK ERROR
				if (aplRankSub > 1)
				{
					ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
											   lptkFunc);
					bRet = FALSE;
				} else
				// Check for LENGTH ERROR
				if (aplNELMSub NE aplRankLft)
				{
					ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
											   lptkFunc);
					bRet = FALSE;
				} else
				{
					// Skip over the header and dimensions to the data
					lpMemSub = VarArrayBaseToData (lpMemSub, aplRankSub);

					// Check each element in lpMemSub against lpMemDimLft
					//	 for INDEX ERROR as well as DOMAIN ERROR
					//	 and, at the same time, weigh each element as
					//	 per lpMemDimLft to get the appropriate index
					//	 into lpMemLft.

					// Initialize the weight & index accumulator
					aplWeight = 1;
					aplIndex  = 0;

					// Split cases based upon the storage type of the reach index
					switch (aplTypeSub)
					{
						case ARRAY_BOOL:
							// Loop through the reach index, backwards
							for (iRch = aplNELMSub - 1; bRet && iRch >= 0; iRch--)
							{
								// Get the next index & convert to origin-0
								aplInteger = (((1 << (iRch % NBIB)) & ((LPAPLBOOL) lpMemSub)[iRch >> LOG2NBIB]) ? 1 : 0) - bQuadIO;

								// Check for INDEX ERROR
								if (aplInteger < 0
								 || aplInteger >= (APLINT) lpMemDimLft[iRch])
								{
									ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
															   lptkFunc);
									bRet = FALSE;
								} else
								{
									// Add into index & multiply into weight
									aplIndex  += aplWeight * aplInteger;
									aplWeight *= lpMemDimLft[iRch];
								} // End IF/LSE
							} // End FOR

							break;

						case ARRAY_INT:
							// Loop through the reach index, backwards
							for (iRch = aplNELMSub - 1; bRet && iRch >= 0; iRch--)
							{
								// Get the next index & convert to origin-0
								aplInteger = ((LPAPLINT) lpMemSub)[iRch] - bQuadIO;

								// Check for INDEX ERROR
								if (aplInteger < 0
								 || aplInteger >= (APLINT) lpMemDimLft[iRch])
								{
									ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
															   lptkFunc);
									bRet = FALSE;
								} else
								{
									// Add into index & multiply into weight
									aplIndex  += aplWeight * aplInteger;
									aplWeight *= lpMemDimLft[iRch];
								} // End IF/LSE
							} // End FOR

							break;

						case ARRAY_FLOAT:
							// Loop through the reach index, backwards
							for (iRch = aplNELMSub - 1; bRet && iRch >= 0; iRch--)
							{
								// Get the next index & convert to origin-0
								aplInteger = FloatToAplint_SCT (((LPAPLFLOAT) lpMemSub)[iRch], &bRet) - bQuadIO;

								// Check for INDEX ERROR
								if (!bRet
								 || aplInteger < 0
								 || aplInteger >= (APLINT) lpMemDimLft[iRch])
								{
									ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
															   lptkFunc);
									bRet = FALSE;
								} else
								{
									// Add into index & multiply into weight
									aplIndex  += aplWeight * aplInteger;
									aplWeight *= lpMemDimLft[iRch];
								} // End IF/LSE
							} // End FOR

							break;

						case ARRAY_APA:

#define lpaplAPA		((LPAPLAPA) lpMemSub)

							apaOff = lpaplAPA->Off;
							apaMul = lpaplAPA->Mul;

#undef	lpaplAPA

							// Loop through the reach index, backwards
							for (iRch = aplNELMSub - 1; bRet && iRch >= 0; iRch--)
							{
								// Get the next index & convert to origin-0
								aplInteger = (apaOff + apaMul * iRch) - bQuadIO;

								// Check for INDEX ERROR
								if (aplInteger < 0
								 || aplInteger >= (APLINT) lpMemDimLft[iRch])
								{
									ErrorMessageIndirectToken (ERRMSG_INDEX_ERROR APPEND_NAME,
															   lptkFunc);
									bRet = FALSE;
								} else
								{
									// Add into index & multiply into weight
									aplIndex  += aplWeight * aplInteger;
									aplWeight *= lpMemDimLft[iRch];
								} // End IF/LSE
							} // End FOR

							break;

						case ARRAY_CHAR:
						case ARRAY_HETERO:
						case ARRAY_NESTED:
							ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
													   lptkFunc);
							bRet = FALSE;

							break;

						defstop
							break;
					} // End SWITCH

					// Check for LENGTH ERROR if the left arg is simple (Depth < 2)
					//	 and the # reach indices are > 1.
					if (IsSimple (aplTypeLft)
					 && aplNELMRch > 1)
					{
						ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
												   lptkFunc);
						bRet = FALSE;
					} else
					{
						// Now use aplIndex to extract the next item from lpMemLft
						//	 if it's simple
						if (IsSimple (aplTypeLft))
							bRet =
							ValueAsSymentry (lpMemRes,
											 lpMemLft,
											 aplTypeLft,
											 aplIndex,
											 lptkFunc);
						else
						{
							HGLOBAL hGlbSub;

							// Get the global memory handle
							hGlbSub = *(LPAPLNESTED) lpMemSub;

							// Split cases based upon the ptr type
							switch (GetPtrTypeDir (hGlbSub))
							{
								case PTRTYPE_STCONST:





									break;

								case PTRTYPE_HGLOBAL:




									// Get the global memory handle

									// Lock the memory to get a ptr to it
									lpMemSub = MyGlobalLock (ClrPtrTypeDirGlb (hGlbSub));

									// Reach down through the left arg using successive
									//	 elements of the reach arg as indices
									bRet =
									ReachDown_EM (lpMemRes, 			// Put the result here
												  lpMemSub, 			// Ptr to the reach arg
												  aplNELMSub,			// The NELM of the reach arg
												  lpMemLft, 			// Get the results from here
												  aplTypeLft,			// The storage type of the left arg
												  aplRankLft,			// The rank of the left arg
												  lpMemDimLft,			// Ptr to the dimensions of the left arg
												  lptkFunc);			// Ptr to function token
									// We no loner need this ptr
									MyGlobalUnlock (ClrPtrTypeDirGlb (hGlbSub));

									break;

								defstop
									break;
							} // End SWITCH
						} // End IF/ELSE
					} // End IF/ELSE
				} // End IF/ELSE

				// We no longer need this ptr
				MyGlobalUnlock (ClrPtrTypeIndGlb (lpMemRch)); lpMemSub = NULL;

				break;

			defstop
				break;
		} // End SWITCH
	} // End IF/ELSE

	return bRet;
} // End ReachDown_EM
#undef	APPEND_NAME


//***************************************************************************
//	End of File: pf_squad.c
//***************************************************************************
