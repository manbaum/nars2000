//***************************************************************************
//	NARS2000 -- System Function Routines
//***************************************************************************

#define STRICT
#include <windows.h>
#include <time.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "termcode.h"
#include "externs.h"
#include "pertab.h"
#include "sis.h"
#include "dfnhdr.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//	$ExecuteFn0
//
//	Execute a niladic function
//***************************************************************************

LPPL_YYSTYPE ExecuteFn0
	(LPPL_YYSTYPE lpYYFcn0) 	// Ptr to function PL_YYSTYPE

{
	LPPRIMFNS lpNameFcn;

	// tkData is an LPSYMENTRY
	Assert (GetPtrTypeDir (lpYYFcn0->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

	lpNameFcn = lpYYFcn0->tkToken.tkData.tkSym->stData.stNameFcn;

	if (lpYYFcn0->tkToken.tkData.tkSym->stFlags.FcnDir)
		// Call the execution routine
		return (*lpNameFcn) (NULL,
							&lpYYFcn0->tkToken,
							 NULL,
							 NULL);
	else
		// tkData is a valid HGLOBAL function array or user-defined function/operator
		Assert (IsGlbTypeFcnDir (lpNameFcn)
			 || IsGlbTypeDfnDir (lpNameFcn));

		// Split cases based upon the array signature
		switch (GetSignatureGlb (ClrPtrTypeDirGlb (lpNameFcn)))
		{
			case FCNARRAY_HEADER_SIGNATURE:
				return ExecFuncGlb_EM_YY (NULL, 						// Ptr to left arg token (may be NULL if monadic or niladic)
										  ClrPtrTypeDirGlb (lpNameFcn), // Function array global memory handle
										  NULL, 						// Ptr to right arg token (may be NULL if niladic)
										  NULL);						// Ptr to axis token (may be NULL)
			case DFN_HEADER_SIGNATURE:
				return ExecDfnGlb_EM_YY (ClrPtrTypeDirGlb (lpNameFcn),	// User-defined function/operator global memory handle
										 NULL,							// Ptr to left arg token (may be NULL if monadic)
										 lpYYFcn0,						// Ptr to function strand
										 NULL,							// Ptr to right arg token
										 LINENUM_ONE);					// Starting line # (see LINE_NUMS)
			defstop
				return NULL;
		} // SWITCH
} // End ExecuteFn0


//***************************************************************************
//	$SysFnAV_EM
//
//	System function:  []AV -- Atomic Vector
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnAV_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnAV_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (should be NULL)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	LPPL_YYSTYPE lpYYRes;			// Ptr to the result

	// This function is niladic
	Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
	{
		ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
								   lptkAxis);
		return NULL;
	} // End IF

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0; 	// Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbAV);
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	return lpYYRes;
} // End SysFnAV_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnCR_EM
//
//	System function:  []CR -- Canonical Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnCR_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnCR_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
	{
		ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
								   lptkAxis);
		return NULL;
	} // End IF

	// Split cases based upon monadic or dyadic
	if (lptkLftArg EQ NULL)
		return SysFnMonCR_EM (			  lptkFunc, lptkRhtArg, lptkAxis);
	else
		return SysFnDydCR_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnCR_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnMonCR_EM
//
//	Monadic []CR -- Canonical Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnMonCR_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonCR_EM
	(LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	APLSTYPE	   aplTypeRht;			// Right arg storage type
	APLNELM 	   aplNELMRht,			// Right arg NELM
				   aplNELMRes;			// Result	 ...
	APLRANK 	   aplRankRht;			// Right arg rank
	HGLOBAL 	   hGlbRht = NULL,		// Right arg global memory handle
				   hGlbRes = NULL,		// Result	 ...
				   hGlbData,			// Data 	 ...
				   hGlbTxtLine; 		// Line text ...
	LPVOID		   lpMemRht = NULL, 	// Ptr to right arg global memory
				   lpMemRes = NULL, 	// Ptr to result	...
				   lpMemData;			// Ptr to function data ...
	LPMEMTXT_UNION lpMemTxtLine;		// Ptr to header/line text global memory
	UINT		   uLineLen;			// Length of a text line
	APLLONGEST	   aplLongestRht;		// Right arg longest if immediate
	LPSYMENTRY	   lpSymEntry;			// Ptr to SYMENTRY
	STFLAGS 	   stFlags; 			// STE flags
	LPPL_YYSTYPE   lpYYRes = NULL;		// Ptr to the result
	LPAPLCHAR	   lpw; 				// Ptr to wide chars

	// Get the attributes (Type, NELM, and Rank)
	//	 of the right arg
	AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

	// Check for RANK ERROR
	if (aplRankRht > 1)
	{
		ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Check for DOMAIN ERROR
	if (aplTypeRht NE ARRAY_CHAR)
	{
		ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Get right arg's global ptrs
	aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

	// Clear the flags
	ZeroMemory (&stFlags, sizeof (stFlags));

	// Split cases based upon the right arg rank
	if (aplRankRht EQ 0)
		// Lookup the name in the symbol table
		// SymTabLookupName sets the .ObjName enum,
		//	 and the .Inuse flag
		lpSymEntry = SymTabLookupNameLength ((LPAPLCHAR) &aplLongestRht,
											 (UINT) aplNELMRht,
											&stFlags);
	else
	{
		// Skip over the header and dimensions to the data
		lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

		// Lookup the name in the symbol table
		// SymTabLookupName sets the .ObjName enum,
		//	 and the .Inuse flag
		lpSymEntry = SymTabLookupNameLength ((LPAPLCHAR) lpMemRht,
											 (UINT) aplNELMRht,
											&stFlags);
	} // End IF/ELSE

	// If not found,
	//	 or it's a System Name,
	//	 or without a value,
	//	 return 0 x 0 char matrix
	if (!lpSymEntry
	 ||  lpSymEntry->stFlags.ObjName EQ OBJNAME_SYS
	 || !lpSymEntry->stFlags.Value)
		// Not the signature of anything we know
		// Return a 0 x 0 char matric
		hGlbRes = SysFnMonCR_ALLOC_EM (0, 2, lpwszTemp, lptkFunc);
	else
	// If it's immediate, ...
	if (lpSymEntry->stFlags.Imm)
	{
		// Copy the function name
		lpw = CopySteName (lpwszTemp, lpSymEntry);

		// Append a left arrow
		*lpw++ = UTF16_LEFTARROW;

		// Append the single char
		*lpw++ = lpSymEntry->stData.stChar;

		// Calculate the result NELM
		aplNELMRes = lpw - lpwszTemp;

		// Finish the job via subroutine
		hGlbRes = SysFnMonCR_ALLOC_EM (aplNELMRes, 1, lpwszTemp, lptkFunc);
	} else
	{
		// Check for internal functions
		if (lpSymEntry->stFlags.FcnDir)
		{
			// Append the function name from the symbol table
			lpw = CopySteName (lpwszTemp, lpSymEntry);

			// Calculate the result NELM
			aplNELMRes = lpw - lpwszTemp;

			// Finish the job via subroutine
			hGlbRes = SysFnMonCR_ALLOC_EM (aplNELMRes, 1, lpwszTemp, lptkFunc);
		} else
		{
			// Get the global memory ptr
			hGlbData = lpSymEntry->stData.stGlbData;

			// Clear the ptr type bits
			hGlbData = ClrPtrTypeDirGlb (hGlbData);

			// Lock the memory to get a ptr to it
			lpMemData = MyGlobalLock (hGlbData);

			// Split cases based upon the array type
			switch (((LPHEADER_SIGNATURE) lpMemData)->nature)
			{
				case FCNARRAY_HEADER_SIGNATURE:
					// Get the line text global memory handle
					hGlbTxtLine = ((LPFCNARRAY_HEADER) lpMemData)->hGlbTxtLine;

					// Lock the memory to get a ptr to it
					lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

					// Get the length of the line text
					aplNELMRes = lpMemTxtLine->U;

					// Copy the function line text to global memory
					CopyMemory (lpwszTemp, &lpMemTxtLine->C, (UINT) aplNELMRes * sizeof (lpMemTxtLine->C));

					// We no longer need this ptr
					MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

					// Finish the job via subroutine
					hGlbRes = SysFnMonCR_ALLOC_EM (aplNELMRes, 1, lpwszTemp, lptkFunc);

					break;

				case DFN_HEADER_SIGNATURE:
				{
					LPDFN_HEADER  lpMemDfnHdr;		// Ptr to user-defined function/operator header ...
					LPFCNLINE	  lpFcnLines;		// Ptr to array of function line structs (FCNLINE[numFcnLines])
					UINT		  uNumLines,		// # function lines
								  uLine,			// Loop counter
								  uMaxLineLen;		// Length of the longest line
					APLUINT 	  ByteRes;			// # bytes in the result

					// Get ptr to user-defined function/operator header
					lpMemDfnHdr = (LPDFN_HEADER) lpMemData;

					// Lock the memory to get a ptr to it
					lpMemTxtLine = MyGlobalLock (lpMemDfnHdr->hGlbTxtHdr);

					// Get the length of the function header text
					uMaxLineLen = lpMemTxtLine->U;

					// We no longer need this ptr
					MyGlobalUnlock (lpMemDfnHdr->hGlbTxtHdr); lpMemTxtLine = NULL;

					// Get ptr to array of function line structs (FCNLINE[numFcnLines])
					lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

					// Get # function lines
					uNumLines = lpMemDfnHdr->numFcnLines;

					// Run through the function lines looking for the longest
					for (uLine = 0; uLine < uNumLines; uLine++)
					{
						// Get the line text global memory handle
						hGlbTxtLine = lpFcnLines->hGlbTxtLine;

						if (hGlbTxtLine)
						{
							// Lock the memory to get a ptr to it
							lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

							// Find the length of the longest line
							uMaxLineLen = max (uMaxLineLen, lpMemTxtLine->U);

							// We no longer need this ptr
							MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
						} // End IF

						// Skip to the next struct
						lpFcnLines++;
					} // End FOR

					// Calculate the result NELM ("1 +" includes the header)
					aplNELMRes = (1 + uNumLines) * uMaxLineLen;

					// Calculate the space needed for the result
					ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, 2);

					// Allocate space for the result
					// N.B.:  Conversion from APLUINT to UINT
					Assert (ByteRes EQ (UINT) ByteRes);
					hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
					if (!hGlbRes)
						ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
												   lptkFunc);
					else
					{
						// Lock the memory to get a ptr to it
						lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes 	((LPVARARRAY_HEADER) lpMemRes)

						// Fill in the header
						lpHeaderRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
						lpHeaderRes->ArrType	= ARRAY_CHAR;
////////////////////////lpHeaderRes->Perm		= 0;		// Already zero from GHND
////////////////////////lpHeaderRes->SysVar 	= 0;		// Already zero from GHND
						lpHeaderRes->RefCnt 	= 1;
						lpHeaderRes->NELM		= aplNELMRes;
						lpHeaderRes->Rank		= 2;

#undef	lpHeaderRes

						// Save the dimensions in the result ("1 +" includes the header)
						(VarArrayBaseToDim (lpMemRes))[0] = 1 + uNumLines;
						(VarArrayBaseToDim (lpMemRes))[1] = uMaxLineLen;

#define lpMemResChar	((LPAPLCHAR) lpMemRes)

						// Skip over the header and dimensions to the data
						lpMemResChar = VarArrayBaseToData (lpMemRes, 2);

						// Lock the memory to get a ptr to it
						lpMemTxtLine = MyGlobalLock (lpMemDfnHdr->hGlbTxtHdr);

						// Get the length of the function header text
						uLineLen = lpMemTxtLine->U;

						// Copy the function header text to the result
						CopyMemory (lpMemResChar, &lpMemTxtLine->C, uLineLen * sizeof (lpMemTxtLine->C));

						// We no longer need this ptr
						MyGlobalUnlock (lpMemDfnHdr->hGlbTxtHdr); lpMemTxtLine = NULL;

						// Fill the remainder of the line with blanks
						for (lpMemResChar += uLineLen;
							 uLineLen < uMaxLineLen;
							 uLineLen++)
							*lpMemResChar++ = L' ';

						// Get ptr to array of function line structs (FCNLINE[numFcnLines])
						lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

						// Run through the function lines copying each line text to the result
						for (uLine = 0; uLine < uNumLines; uLine++)
						{
							// Get the line text global memory handle
							hGlbTxtLine = lpFcnLines->hGlbTxtLine;

							if (hGlbTxtLine)
							{
								// Lock the memory to get a ptr to it
								lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

								// Get the length of the function line text
								uLineLen = lpMemTxtLine->U;

								// Copy the function header text to the result
								CopyMemory (lpMemResChar, &lpMemTxtLine->C, uLineLen * sizeof (lpMemTxtLine->C));

								// We no longer need this ptr
								MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

								// Fill the remainder of the line with blanks
								for (lpMemResChar += uLineLen;
									 uLineLen < uMaxLineLen;
									 uLineLen++)
									*lpMemResChar++ = L' ';
							} // End IF
#undef	lpMemResChar
							// Skip to the next struct
							lpFcnLines++;
						} // End FOR
					} // End IF/ELSE

					break;
				} // End DFN_HEADER_SIGNATURE

				case VARARRAY_HEADER_SIGNATURE: // Return a 0 x 0 char matrix
					// Finish the job via subroutine
					hGlbRes = SysFnMonCR_ALLOC_EM (0, 2, L"", lptkFunc);

					break;

				defstop
					break;
			} // End SWITCH

			// We no longer need this ptr
			MyGlobalUnlock (hGlbData); lpMemData = NULL;
		} // End IF/ELSE
	} // End IF/ELSE

	if (!hGlbRes)
		goto ERROR_EXIT;

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0; 	// Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;
ERROR_EXIT:
	if (hGlbRes && lpMemRes)
	{
		// We no longer need this ptr
		MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
	} // End IF

	// We no longer need this ptr
	if (hGlbRht && lpMemRht)
	{
		MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
	} // End IF

	return lpYYRes;
} // End SysFnMonCR_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnMonCR_ALLOC_EM
//
//	Subroutine to SysFnMonCR_EM for allocating and copying to the result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnMonCR_ALLOC_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL SysFnMonCR_ALLOC_EM
	(APLNELM   aplNELMRes,		// Result NELM
	 APLRANK   aplRankRes,		// ...	  rank
	 LPAPLCHAR lpw, 			// Ptr to result text
	 LPTOKEN   lptkFunc)		// Ptr to function token

{
	HGLOBAL    hGlbRes; 		// Result global memory handle
	LPVOID	   lpMemRes;		// Ptr to result	...
	APLUINT    ByteRes; 		// # bytes in the result

	// Calculate space needed for the result
	ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, aplRankRes);

	// Allocate space for the result
	// N.B.:  Conversion from APLUINT to UINT
	Assert (ByteRes EQ (UINT) ByteRes);
	hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
	if (!hGlbRes)
	{
		ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Lock the memory to get a ptr to it
	lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader		((LPVARARRAY_HEADER) lpMemRes)

	// Fill in the header
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = ARRAY_CHAR;
////lpHeader->Perm		 = 0;			// Already zero from GHND
////lpHeader->SysVar	 = 0;			// Already zero from GHND
	lpHeader->RefCnt	 = 1;
	lpHeader->NELM		 = aplNELMRes;
	lpHeader->Rank		 = aplRankRes;

#undef	lpHeader

	// Save the dimension in the result
	*VarArrayBaseToDim (lpMemRes) = aplNELMRes;

	// Skip over the header and dimensions to the result
	lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

	// Copy the function text to the result
	CopyMemory (lpMemRes, lpw, (UINT) aplNELMRes * sizeof (APLCHAR));

	// We no longer need this ptr
	MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

	return hGlbRes;
} // End SysFnMonCR_ALLOC_EM
#undef	APPEND_NAME


//***************************************************************************
//	$CopySteName
//
//	Copy a STE name
//***************************************************************************

LPAPLCHAR CopySteName
	(LPAPLCHAR	lpMemRes,		// Ptr to result global memory
	 LPSYMENTRY lpSymEntry) 	// Function name global memory handle

{
	LPAPLCHAR lpMemName;		// Ptr to function name global memory

	// Lock the memory to get a ptr to it
	lpMemName = MyGlobalLock (lpSymEntry->stHshEntry->htGlbName);

	// Copy the function name
	lstrcpyW (lpMemRes, lpMemName);

	// We no longer need this ptr
	MyGlobalUnlock (lpSymEntry->stHshEntry->htGlbName); lpMemName = NULL;

	// Point to the end
	return &lpMemRes[lstrlenW (lpMemRes)];
} // End CopySteName


//***************************************************************************
//	$SysFnDydCR_EM
//
//	Dyadic []CR -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnDydCR_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydCR_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	return PrimFnSyntaxError_EM (lptkFunc);
} // End SysFnDydCR_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnDL_EM
//
//	System function:  []DL -- Delay
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnDL_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDL_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
	{
		ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
								   lptkAxis);
		return NULL;
	} // End IF

	// Split cases based upon monadic or dyadic
	if (lptkLftArg EQ NULL)
		return SysFnMonDL_EM (			  lptkFunc, lptkRhtArg, lptkAxis);
	else
		return SysFnDydDL_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnDL_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnMonDL_EM
//
//	Monadic []DL -- Delay
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnMonDL_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonDL_EM
	(LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	APLSTYPE	 aplTypeRht;		// Right arg storage type
	APLNELM 	 aplNELMRht;		// Right arg NELM
	APLRANK 	 aplRankRht;		// Right arg rank
	APLFLOAT	 aplFloatRht;		// Right arg float value
	DWORD		 dwTickCount;		// The current tick count (time in millseconds since W was started)
	LPPL_YYSTYPE lpYYRes;			// Ptr to the result
	HGLOBAL 	 hGlbPTD;			// PerTabData global memory handle
	LPPERTABDATA lpMemPTD;			// Ptr to PerTabData global memory

	// Get the attributes (Type, NELM, and Rank)
	//	 of the right arg
	AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

	// Check for RANK ERROR
	if (aplRankRht > 1)
	{
		ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Check for LENGTH ERROR
	if (aplNELMRht NE 1)
	{
		ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Check for DOMAIN ERROR
	if (!IsSimpleNum (aplTypeRht))
	{
		ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Get the one (and only) value from the right arg
	FirstValue (lptkRhtArg, 			// Ptr to right arg token
				NULL,					// Ptr to integer result
			   &aplFloatRht,			// Ptr to float ...
				NULL,					// Ptr to WCHAR ...
				NULL,					// Ptr to longest ...
				NULL,					// Ptr to lpSym/Glb ...
				NULL,					// Ptr to ...immediate type ...
				NULL);					// Ptr to array type ...
	// Get the thread's PerTabData global memory handle
	hGlbPTD = TlsGetValue (dwTlsPerTabData);

	// Lock the memory to get a ptr to it
	lpMemPTD = MyGlobalLock (hGlbPTD);

	// Create a semaphore so we can be interrupted
	lpMemPTD->hSemaDelay =
	CreateSemaphore (NULL,				// No security attrs
					 0, 				// Initial count (non-signalled)
					 64*1024,			// Maximum count
					 NULL); 			// No name
	// Get the current tick count so we can subtract it from the next tick count
	dwTickCount = GetTickCount ();

	// Scale the value from float seconds to DWORD milliseconds
	//	 and wait for that long
	WaitForSingleObject (lpMemPTD->hSemaDelay,
				 (DWORD) (aplFloatRht * 1000));
	// Determine how long we waited
	dwTickCount = GetTickCount () - dwTickCount;

	// Close the semaphore handle as it is no longer needed
	CloseHandle (lpMemPTD->hSemaDelay); lpMemPTD->hSemaDelay = NULL;

	// We no longer need this ptr
	MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

	// Convert from DWORD milliseconds to float seconds
	aplFloatRht = ((APLFLOAT) dwTickCount) / 1000;

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
	lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_FLOAT;
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkFloat    = aplFloatRht;
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	return lpYYRes;
} // End SysFnMonDL_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnDydDL_EM
//
//	Dyadic []DL -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnDydDL_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydDL_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	return PrimFnSyntaxError_EM (lptkFunc);
} // End SysFnDydDL_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnDM_EM
//
//	System function:  []DM -- Diagnostic Message
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnDM_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDM_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (should be NULL)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token  (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	LPPL_YYSTYPE lpYYRes;		// Ptr to the result
	HGLOBAL 	 hGlbRes;		// Result global memory handle
	HGLOBAL 	 hGlbPTD;		// PerTabData global memory handle
	LPPERTABDATA lpMemPTD;		// Ptr to PerTabData global memory

	// This function is niladic
	Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
	{
		ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
								   lptkAxis);
		return NULL;
	} // End IF

	// Get the thread's PerTabData global memory handle
	hGlbPTD = TlsGetValue (dwTlsPerTabData);

	// Lock the memory to get a ptr to it
	lpMemPTD = MyGlobalLock (hGlbPTD);

	// Get the global memory handle for quad-DM
	hGlbRes = CopySymGlbDir (MakeGlbTypeGlb (lpMemPTD->hGlbQuadDM));

	// We no longer need this ptr
	MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

	// Allocate a YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0; 	// Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkGlbData  = hGlbRes;
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	return lpYYRes;
} // End SysFnDM_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnDR_EM
//
//	System function:  []DR -- Data Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnDR_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDR_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
	{
		ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
								   lptkAxis);
		return NULL;
	} // End IF

	// Split cases based upon monadic or dyadic
	if (lptkLftArg EQ NULL)
		return SysFnMonDR_EM (			  lptkFunc, lptkRhtArg, lptkAxis);
	else
		return SysFnDydDR_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnDR_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnMonDR_EM
//
//	Monadic []DR -- Display the Data Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnMonDR_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonDR_EM
	(LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	HGLOBAL 	 hGlbData;
	LPVOID		 lpMem;
	LPPL_YYSTYPE lpYYRes;			// Ptr to the result

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
	lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
////lpYYRes->tkToken.tkData.tkInteger  =   (filled in below)
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

#define DR_SHOW 			0	// Return a character vector representation
#define DR_BOOL 		  100	//	  1 bit  per value
#define DR_CHAR 		 1601	//	 16 bits ...
#define DR_INT			 6402	//	 64 ...
#define DR_FLOAT		 6403	//	 64 ...
#define DR_APA			 6404	//	 64 ...
#define DR_HETERO		 3205	//	 32 ...
#define DR_NESTED		 3206	//	 32 ...
#define DR_LIST 		 3207	//	 32 ...
#define DR_RATIONAL 		8	//	 ?? ...
#define DR_COMPLEX		12809	//	128 ...
#define DR_QUATERNIONS	25610	//	256 ...
#define DR_OCTONIONS	51211	//	512 ...

	// Split cases based upon the right arg's token type
	switch (lptkRhtArg->tkFlags.TknType)
	{
		case TKT_VARNAMED:
			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

			// If it's not immediate, it's an HGLOBAL
			if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
			{
				hGlbData = lptkRhtArg->tkData.tkSym->stData.stGlbData;

				break;		// Continue with HGLOBAL case
			} // End IF

			// Handle the immediate case

			// stData is an immediate
			Assert (lptkRhtArg->tkData.tkSym->stFlags.Imm);

			// Split cases based upon the token's immediate type
			switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
			{
				case IMMTYPE_BOOL:
					lpYYRes->tkToken.tkData.tkInteger = DR_BOOL;

					return lpYYRes;

				case IMMTYPE_INT:
					lpYYRes->tkToken.tkData.tkInteger = DR_INT;

					return lpYYRes;

				case IMMTYPE_FLOAT:
					lpYYRes->tkToken.tkData.tkInteger = DR_FLOAT;

					return lpYYRes;

				case IMMTYPE_CHAR:
					lpYYRes->tkToken.tkData.tkInteger = DR_CHAR;

					return lpYYRes;

				defstop
					return NULL;
			} // End SWITCH

			DbgStop (); 		// We should never get here

		case TKT_VARIMMED:
			// Split cases based upon the token's immediate type
			switch (lptkRhtArg->tkFlags.ImmType)
			{
				case IMMTYPE_BOOL:
					lpYYRes->tkToken.tkData.tkInteger = DR_BOOL;

					return lpYYRes;

				case IMMTYPE_INT:
					lpYYRes->tkToken.tkData.tkInteger = DR_INT;

					return lpYYRes;

				case IMMTYPE_FLOAT:
					lpYYRes->tkToken.tkData.tkInteger = DR_FLOAT;

					return lpYYRes;

				case IMMTYPE_CHAR:
					lpYYRes->tkToken.tkData.tkInteger = DR_CHAR;

					return lpYYRes;

				defstop
					return NULL;
			} // End SWITCH

			DbgStop (); 		// We should never get here

		case TKT_LISTPAR:
			lpYYRes->tkToken.tkData.tkInteger = DR_LIST;

			return lpYYRes;

		case TKT_VARARRAY:
			hGlbData = lptkRhtArg->tkData.tkGlbData;

			break;		// Continue with HGLOBAL case

		defstop
			break;
	} // End SWITCH

	// HGLOBAL case
	// tk/stData is a valid HGLOBAL variable array
	Assert (IsGlbTypeVarDir (hGlbData));

	// Clear the ptr type bits
	hGlbData = ClrPtrTypeDirGlb (hGlbData);

	// Lock the memory to get a ptr to it
	lpMem = MyGlobalLock (hGlbData);

#define lpHeader	((LPVARARRAY_HEADER) lpMem)

	// Split cases based upon the array storage type
	switch (lpHeader->ArrType)
#undef	lpHeader
	{
		case ARRAY_BOOL:
			lpYYRes->tkToken.tkData.tkInteger = DR_BOOL;

			break;

		case ARRAY_INT:
			lpYYRes->tkToken.tkData.tkInteger = DR_INT;

			break;

		case ARRAY_FLOAT:
			lpYYRes->tkToken.tkData.tkInteger = DR_FLOAT;

			break;

		case ARRAY_CHAR:
			lpYYRes->tkToken.tkData.tkInteger = DR_CHAR;

			break;

		case ARRAY_HETERO:
			lpYYRes->tkToken.tkData.tkInteger = DR_HETERO;

			break;

		case ARRAY_NESTED:
			lpYYRes->tkToken.tkData.tkInteger = DR_NESTED;

			break;

		case ARRAY_APA:
			lpYYRes->tkToken.tkData.tkInteger = DR_APA;

			break;

////////case ARRAY_LIST:			// Handled above with TKT_LIST
		defstop
			break;
	} // End SWITCH

	// We no longer need this ptr
	MyGlobalUnlock (hGlbData); lpMem = NULL;

	return lpYYRes;
} // End SysFnMonDR_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnDydDR_EM
//
//	Dyadic []DR -- Convert To A Data Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnDydDR_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydDR_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	APLSTYPE	 aplTypeLft,
				 aplTypeRht;
	APLNELM 	 aplNELMLft,
				 aplNELMRht;
	APLRANK 	 aplRankLft,
				 aplRankRht;
	HGLOBAL 	 hGlbRes = NULL;
	APLINT		 aplIntegerLft;
	APLFLOAT	 aplFloatLft;
	LPPL_YYSTYPE lpYYRes;			// Ptr to the result

	// Get the attributes (Type, NELM, and Rank)
	//	 of the left & right args
	AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
	AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

	// Check for LEFT RANK ERROR
	if (aplRankLft > 1)
	{
		ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Check for LEFT LENGTH ERROR
	if (aplNELMLft NE 1)
	{
		ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Check for LEFT DOMAIN ERROR
	if (!IsSimpleNum (aplTypeLft))
	{
		ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Get the first (and only) value
	FirstValue (lptkLftArg, 		// Ptr to left arg token
			   &aplIntegerLft,		// Ptr to integer result
			   &aplFloatLft,		// Ptr to float ...
				NULL,				// Ptr to WCHAR ...
				NULL,				// Ptr to longest ...
				NULL,				// Ptr to lpSym/Glb ...
				NULL,				// Ptr to ...immediate type ...
				NULL);				// Ptr to array type ...

	// If it's a float, ...
	if (aplTypeLft EQ ARRAY_FLOAT)
	{
		BOOL bRet;

		// Attempt to convert the float to an integer
		aplIntegerLft = FloatToAplint_SCT (aplFloatLft, &bRet);
		if (!bRet)
		{
			ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
									   lptkFunc);
			return NULL;
		} // End IF
	} // End IF

	// Ensure the left arg is valid
	switch (aplIntegerLft)
	{
		case DR_SHOW:
			// Return a character representation
			return SysFnDydDR_SHOW_EM (aplTypeRht, lptkFunc);

		case DR_BOOL:
		case DR_CHAR:
		case DR_INT:
		case DR_FLOAT:
			ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
									   lptkFunc);
			return NULL;

			DbgBrk ();		// ***FINISHME*** -- SysFnMonDR_EM





			break;

		case DR_APA:
		case DR_HETERO:
		case DR_NESTED:
		case DR_LIST:
		case DR_RATIONAL:
		case DR_COMPLEX:
		case DR_QUATERNIONS:
		case DR_OCTONIONS:
			// ***DEBUG***
			hGlbRes = DR_FloatToChar_EM (lptkRhtArg, lptkFunc);

			break;

		default:
			ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
									   lptkFunc);
			return NULL;
	} // End SWITCH

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0; 	// Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	return lpYYRes;
} // End SysFnDydDR_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnDydDR_SHOW_EM
//
//	Return a character representation of the storage type
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnDydDR_SHOW_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydDR_SHOW_EM
	(APLSTYPE aplTypeRht,
	 LPTOKEN  lptkFunc)

{
	LPAPLCHAR	 lpw;			// Ptr to WCHAR text
	APLUINT 	 ByteRes;		// # bytes in the result
	HGLOBAL 	 hGlbRes;		// Result global memory handle
	LPVOID		 lpMemRes;		// Ptr to result global memory
	LPPL_YYSTYPE lpYYRes;		// Ptr to the result
	APLNELM 	 aplNELMRes;	// Result NELM

	// Split cases based upon the rigth arg storage type
	switch (aplTypeRht)
	{
		case ARRAY_BOOL:
			lpw = L"Boolean:  1 bit per element";

			break;

		case ARRAY_INT:
			lpw = L"Integer:  64 bits per element";

			break;

		case ARRAY_FLOAT:
			lpw = L"Floating Point:  64 bits per element";

			break;

		case ARRAY_CHAR:
			lpw = L"Character:  16 bits per element";

			break;

		case ARRAY_APA:
			lpw = L"Arithmetic Progression Array:  128 bits total";

			break;

		case ARRAY_LIST:
			lpw = L"List:  32 bits per element";

			break;

		case ARRAY_NESTED:
			lpw = L"Nested Array:  32 bits per element";

			break;

		case ARRAY_HETERO:
			lpw = L"Heterogeneous Array:  32 bits per element";

			break;

		defstop
			break;
	} // End SWITCH

	// Get the result NELM
	aplNELMRes = lstrlenW (lpw);

	// Calculate space needed for the result
	ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, 1);

	// Allocate space for the result
	// N.B.:  Conversion from APLUINT to UINT
	Assert (ByteRes EQ (UINT) ByteRes);
	hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
	if (!hGlbRes)
	{
		ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Lock the memory to get a ptr to it
	lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes 	((LPVARARRAY_HEADER) lpMemRes)

	// Fill in the header
	lpHeaderRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeaderRes->ArrType	= ARRAY_CHAR;
////lpHeaderRes->Perm		= 0;		// Already zero from GHND
////lpHeaderRes->SysVar 	= 0;		// Already zero from GHND
	lpHeaderRes->RefCnt 	= 1;
	lpHeaderRes->NELM		= aplNELMRes;
	lpHeaderRes->Rank		= 1;

#undef	lpHeaderRes

	// Save the dimension in the result
	*VarArrayBaseToDim (lpMemRes) = aplNELMRes;

	// Point to the data (APLAPA struct)
	lpMemRes = VarArrayBaseToData (lpMemRes, 1);

	// Copy the text to the result
	CopyMemory (lpMemRes, lpw, (UINT) aplNELMRes * sizeof (APLCHAR));

	// We no longer need this ptr
	MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0; 	// Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	return lpYYRes;
} // End SysFnDydDR_SHOW_EM
#undef	APPEND_NAME


//***************************************************************************
//	$DR_FloatToChar_EM
//
//	QuadDR subroutine to convert FP to WCHAR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- DR_FloatToChar_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL DR_FloatToChar_EM
	(LPTOKEN lptkRhtArg,
	 LPTOKEN lptkFunc)

{
	APLSTYPE aplTypeRht;		// Right arg storage type
	APLNELM  aplNELMRht;		// Right arg NELM
	APLRANK  aplRankRht;		// Right arg rank
	APLUINT  ByteRes,			// # bytes needed for the result
			 uRes;				// Result loop counter
	HGLOBAL  hGlbRht,			// Right arg global memory handle
			 hGlbRes;			// Result	 ...
	LPVOID	 lpMemRht = NULL,	// Ptr to right arg global memory
			 lpMemRes = NULL;	// Ptr to result	...
	APLFLOAT aplFloatRht;		// Temporary float

	// Get the attributes (Type, NELM, and Rank)
	//	 of the right args
	AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

	// Ensure the right arg is float
	if (aplTypeRht NE ARRAY_FLOAT)
	{
		ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Convert the FP argument to displayable chars

	// Calculate space needed for the result
	ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRht * 16, aplRankRht + 1);

	// Allocate space for the result.
	// N.B. Conversion from APLUINT to UINT.
	Assert (ByteRes EQ (UINT) ByteRes);
	hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
	if (!hGlbRes)
	{
		ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Lock the memory to get a ptr to it
	lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader	((LPVARARRAY_HEADER) lpMemRes)

	// Fill in the header
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = ARRAY_CHAR;
////lpHeader->Perm		 = 0;	// Already zero from GHND
////lpHeader->SysVar	 = 0;	// Already zero from GHND
	lpHeader->RefCnt	 = 1;
	lpHeader->NELM		 = aplNELMRht * 16;
	lpHeader->Rank		 = aplRankRht + 1;

#undef	lpHeader

	// Get right arg's global ptrs
	GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

	// Skip over the header to the dimensions
	lpMemRes = VarArrayBaseToDim (lpMemRes);
	if (lpMemRht)
	{
		lpMemRht = VarArrayBaseToDim (lpMemRht);

		//***************************************************************
		// Copy the dimensions from the right arg
		//	 to the result's dimension
		//***************************************************************
		for (uRes = 0; uRes < aplRankRht; uRes++)
			*((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;
	} // End IF

	// The last dimension is 16
	*((LPAPLDIM) lpMemRes)++ = 16;

	// lpMemRes now points to the result's data

	// If the right arg is not an immediate, ...
	if (lpMemRht)
	{
		// Loop through the right arg converting it to the result
		for (uRes = 0; uRes < aplNELMRht; uRes++, ((LPAPLCHAR) lpMemRes += 16))
			FloatToAplchar (*((LPAPLFLOAT) lpMemRht)++, (LPAPLCHAR) lpMemRes);
	} else
	// The right arg is an immediate
	{
		// Get the first (and only) value
		FirstValue (lptkRhtArg, 		// Ptr to right arg token
					NULL,				// Ptr to integer result
				   &aplFloatRht,		// Ptr to float ...
					NULL,				// Ptr to WCHAR ...
					NULL,				// Ptr to longest ...
					NULL,				// Ptr to lpSym/Glb ...
					NULL,				// Ptr to ...immediate type ...
					NULL);				// Ptr to array type ...
		FloatToAplchar (aplFloatRht, (LPAPLCHAR) lpMemRes);
	} // End IF/ELSE

	// We no longer need this ptr
	MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

	// We no longer need this ptr
	if (lpMemRht)
	{
		MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
	} // End IF

	return hGlbRes;
} // End DR_FloatToChar_EM
#undef	APPEND_NAME


//***************************************************************************
//	$CalcNumIDs
//
//	Calculate the # identifiers in an arg
//***************************************************************************

BOOL CalcNumIDs
	(APLNELM	aplNELMRht, 		// Right arg NELM
	 APLRANK	aplRankRht, 		// Right arg rank
	 APLLONGEST aplLongestRht,		// Right arg longest
	 BOOL		bVectorOfNames, 	// TRUE iff we allow multiple names in a vector (e.g., 'a b c')
	 LPAPLCHAR	lpMemRht,			// Ptr to right arg global memory
	 LPAPLNELM	lpaplNELMRes,		// Ptr to # right arg IDs
	 LPAPLNELM	lpaplNELMCol)		// Ptr to # right arg cols (matrix only)

{
	APLUINT uRht;				// Loop counter

	// Split cases based upon the right arg rank
	switch (aplRankRht)
	{
		case 0:
			*lpaplNELMRes = (L' ' NE (APLCHAR) aplLongestRht);

			break;

		case 1:
			*lpaplNELMRes = 0;		// Initialize

			// Skip over the header and dimensions to the data
			lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

			// Loop through the right arg looking for identifiers
			uRht = 0;
			while (TRUE)
			{
				// Skip over leading white space
				while (uRht < aplNELMRht && lpMemRht[uRht] EQ L' ')
					uRht++;
				if (uRht < aplNELMRht)
				{
					// If multiple names in a vector are not allowed
					//	 and this is the second name, ...
					if (!bVectorOfNames
					 && *lpaplNELMRes EQ 1)
						return FALSE;

					// Count in another element in the result
					(*lpaplNELMRes)++;

					// Skip over black space
					while (uRht < aplNELMRht && lpMemRht[uRht] NE L' ')
						uRht++;
				} else
					break;
			} // End FOR

			break;

		case 2:
			*lpaplNELMRes = (VarArrayBaseToDim (lpMemRht))[0];
			*lpaplNELMCol = (VarArrayBaseToDim (lpMemRht))[1];

			break;

		defstop
			break;
	} // End IF

	return TRUE;
} // End CalcNumIDs


//***************************************************************************
//	$SysFnLC_EM
//
//	System function:  []LC -- Line Counter
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnLC_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnLC_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (should be NULL)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	UINT		 ByteRes;			// # bytes in the result
	APLNELM 	 aplNELMRes;		// Result NELM
	HGLOBAL 	 hGlbRes;			// Result global memory handle
	LPVOID		 lpMemRes;			// Ptr to result global memory
	LPPL_YYSTYPE lpYYRes;			// Ptr to the result
	HGLOBAL 	 hGlbPTD;			// PerTabData global memory handle
	LPPERTABDATA lpMemPTD;			// Ptr to PerTabData global memory
	LPSIS_HEADER lpSISCur;			// Ptr to current SIS layer

	// This function is niladic
	Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
	{
		ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
								   lptkAxis);
		return NULL;
	} // End IF

	// Get the thread's PerTabData global memory handle
	hGlbPTD = TlsGetValue (dwTlsPerTabData);

	// Lock the memory to get a ptr to it
	lpMemPTD = MyGlobalLock (hGlbPTD);

	// Trundle through the SI stack counting the # layers with
	//	 a user-defined function/operator
	for (aplNELMRes = 0, lpSISCur = lpMemPTD->lpSISCur;
		 lpSISCur;
		 lpSISCur = lpSISCur->lpSISPrv)
	if (lpSISCur->DfnType EQ DFNTYPE_OP1
	 || lpSISCur->DfnType EQ DFNTYPE_OP2
	 || lpSISCur->DfnType EQ DFNTYPE_FCN)
		aplNELMRes++;

	// Calculate space needed for the result
	ByteRes = (UINT) CalcArraySize (ARRAY_INT, aplNELMRes, 1);

	// Allocate space for the result
	hGlbRes = DbgGlobalAlloc (GHND, ByteRes);
	if (!hGlbRes)
	{
		ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Lock the memory to get a ptr to it
	lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader	((LPVARARRAY_HEADER) lpMemRes)

	// Fill in the header
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = ARRAY_INT;
////lpHeader->Perm		 = 0;			// Already zero from GHND
////lpHeader->SysVar	 = 0;			// Already zero from GHND
	lpHeader->RefCnt	 = 1;
	lpHeader->NELM		 = aplNELMRes;
	lpHeader->Rank		 = 1;

#undef	lpHeader

	// Fill in the dimension
	*VarArrayBaseToDim (lpMemRes) = aplNELMRes;

	// Skip over the header and dimensions to the data
	lpMemRes = VarArrayBaseToData (lpMemRes, 1);

	// Trundle through the SI stack copying the line #
	//	 from layers with a user-defined function/operator
	for (lpSISCur = lpMemPTD->lpSISCur;
		 lpSISCur;
		 lpSISCur = lpSISCur->lpSISPrv)
	if (lpSISCur->DfnType EQ DFNTYPE_OP1
	 || lpSISCur->DfnType EQ DFNTYPE_OP2
	 || lpSISCur->DfnType EQ DFNTYPE_FCN)
		*((LPAPLINT) lpMemRes)++ = lpSISCur->CurLineNum;

	// We no longer need this ptr
	MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0; 	// Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	// We no longer need this ptr
	MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

	return lpYYRes;
} // End SysFnLC_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnNC_EM
//
//	System function:  []NC -- Name Class
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnNC_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNC_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
	{
		ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
								   lptkAxis);
		return NULL;
	} // End IF

	// Split cases based upon monadic or dyadic
	if (lptkLftArg EQ NULL)
		return SysFnMonNC_EM (			  lptkFunc, lptkRhtArg, lptkAxis);
	else
		return SysFnDydNC_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnNC_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnMonNC_EM
//
//	Monadic []NC -- Name Class
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnMonNC_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonNC_EM
	(LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	APLSTYPE	 aplTypeRht;		// Right arg storage type
	APLNELM 	 aplNELMRht,		// Right arg NELM
				 aplNELMRes,		// Result NELM
				 aplNELMCol;		// Result column NELM
	APLRANK 	 aplRankRht;		// Right arg Rank
	APLLONGEST	 aplLongestRht; 	// Right arg longest if immediate
	HGLOBAL 	 hGlbRht = NULL,	// Right arg global memory handle
				 hGlbRes = NULL;	// Result	 ...
	LPVOID		 lpMemRht = NULL,	// Ptr to right arg global memory
				 lpMemRes = NULL;	// Ptr to result	...
	LPAPLCHAR	 lpMemDataRht,		// Ptr to right arg char data
				 lpMemDataStart;	// Ptr to start of identifier
	LPAPLINT	 lpMemDataRes;		// Ptr to result integer data
	APLUINT 	 uRht,				// Loop counter
				 uCol,				// ...
				 ByteRes;			// # bytes in the result
	LPSYMENTRY	 lpSymEntry;		// Ptr to SYMENTRY
	STFLAGS 	 stFlags;			// STE flags
	LPPL_YYSTYPE lpYYRes = NULL;	// Ptr to the result
	BOOL		 bRet = TRUE;		// TRUE iff result is valid

	// The right arg may be of three forms:
	//	 1.  a scalar	 name  as in 'a'
	//	 2.  a vector of name  as in 'a' (not 'a b c')
	//	 3.  a matrix of names as in 3 1{rho}'abc'

	// Get the attributes (Type, NELM, and Rank)
	//	 of the right arg
	AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

	// Check for RANK ERROR
	if (aplRankRht > 2)
	{
		ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Check for DOMAIN ERROR
	if (!IsSimple (aplTypeRht)
	 || (aplTypeRht NE ARRAY_CHAR
	  && aplNELMRht NE 0))
	{
		ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Get right arg's global ptrs
	aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

	// Calculate the # identifiers in the argument
	//	 allowing for vector and matrix with multiple names
	bRet =
	CalcNumIDs (aplNELMRht, 		// Right arg NELM
				aplRankRht, 		// Right arg rank
				aplLongestRht,		// Right arg longest
////////////////FALSE,				// TRUE iff we allow multiple names in a vector
				TRUE,				// TRUE iff we allow multiple names in a vector
				lpMemRht,			// Ptr to right arg global memory
			   &aplNELMRes, 		// Ptr to # right arg IDs
			   &aplNELMCol);		// Ptr to # right arg cols (matrix only)
	// Note that if bRet EQ FALSE, aplNELMRes EQ 1

	// Calculate space needed for the result
	ByteRes = CalcArraySize (ARRAY_INT, aplNELMRes, 1);

	// Allocate space for the result
	// N.B. Conversion from APLUINT to UINT
	Assert (ByteRes EQ (UINT) ByteRes);
	hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
	if (!hGlbRes)
	{
		ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
								   lptkFunc);
		goto ERROR_EXIT;
	} // End IF

	// Lock the memory to get a ptr to it
	lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader	((LPVARARRAY_HEADER) lpMemRes)

	// Fill in the header
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = ARRAY_INT;
////lpHeader->Perm		 = 0;			// Already zero from GHND
////lpHeader->SysVar	 = 0;			// Already zero from GHND
	lpHeader->RefCnt	 = 1;
	lpHeader->NELM		 = aplNELMRes;
	lpHeader->Rank		 = 1;

#undef	lpHeader

	// Fill in the dimension
	*VarArrayBaseToDim (lpMemRes) = aplNELMRes;

	// Skip over the header and dimensions to the data
	lpMemDataRes = VarArrayBaseToData (lpMemRes, 1);

	// Calculate the name class of each element

	// If we failed in CalcNumIDs, quit now
	if (!bRet)
	{
		*lpMemDataRes++ = NAMECLASS_INV;

		goto YYALLOC_EXIT;
	} // End IF

	// Split cases based upon the right arg rank
	switch (aplRankRht)
	{
		case 0:
			// Lookup the name in the symbol table
			// SymTabLookupName sets the .ObjName enum,
			//	 and the .Inuse flag
			ZeroMemory (&stFlags, sizeof (stFlags));
			lpSymEntry = SymTabLookupNameLength ((LPAPLCHAR) &aplLongestRht,
												 1,
												&stFlags);
			// If not found, return NAMECLASS_INV or NAMECLASS_AVL
			if (!lpSymEntry)
				*lpMemDataRes++ = ValidName ((LPAPLCHAR) &aplLongestRht, 1)
								? NAMECLASS_AVL : NAMECLASS_INV;
			else
				*lpMemDataRes++ = CalcNameClass (lpSymEntry);
			break;

		case 1:
			// Skip over the header and dimensions to the data
			lpMemDataRht = VarArrayBaseToData (lpMemRht, aplRankRht);

			// Loop through the right arg looking for identifiers
			uRht = 0;
			while (TRUE)
			{
				// Skip over white space
				while (uRht < aplNELMRht && lpMemDataRht[uRht] EQ L' ')
					uRht++;
				if (uRht < aplNELMRht)
				{
					// Save the starting ptr
					lpMemDataStart = &lpMemDataRht[uRht];

					// Skip over black space
					while (uRht < aplNELMRht && lpMemDataRht[uRht] NE L' ')
						uRht++;
					// Lookup the name in the symbol table
					// SymTabLookupName sets the .ObjName enum,
					//	 and the .Inuse flag
					ZeroMemory (&stFlags, sizeof (stFlags));
					lpSymEntry = SymTabLookupNameLength (lpMemDataStart,
														&lpMemDataRht[uRht] - lpMemDataStart,
														&stFlags);
					// If not found, return NAMECLASS_INV or NAMECLASS_AVL
					if (!lpSymEntry)
						*lpMemDataRes++ = ValidName (lpMemDataStart, &lpMemDataRht[uRht] - lpMemDataStart)
										? NAMECLASS_AVL : NAMECLASS_INV;
					else
						*lpMemDataRes++ = CalcNameClass (lpSymEntry);
				} else
					break;
			} // End WHILE

			break;

		case 2:
			// Skip over the header and dimensions to the data
			lpMemDataRht = VarArrayBaseToData (lpMemRht, aplRankRht);

			for (uRht = 0; uRht < aplNELMRes; uRht++)
			{
				// Point to the start of the data
				lpMemDataStart = &lpMemDataRht[aplNELMCol * uRht];

				// Skip over leading white space
				uCol = 0;
				while (uCol < aplNELMCol && lpMemDataStart[uCol] EQ L' ')
					uCol++;

				// Lookup the name in the symbol table
				// SymTabLookupName sets the .ObjName enum,
				//	 and the .Inuse flag
				ZeroMemory (&stFlags, sizeof (stFlags));
				lpSymEntry = SymTabLookupNameLength (&lpMemDataStart[uCol],
													  (UINT) (aplNELMCol - uCol),
													 &stFlags);
				// If not found, return NAMECLASS_INV or NAMECLASS_AVL
				if (!lpSymEntry)
					*lpMemDataRes++ = ValidName (&lpMemDataStart[uCol], (UINT) (aplNELMCol - uCol))
									? NAMECLASS_AVL : NAMECLASS_INV;
				else
					*lpMemDataRes++ = CalcNameClass (lpSymEntry);
			} // End FOR

			break;

		defstop
			break;
	} // End SWITCH
YYALLOC_EXIT:
	// We no longer need this ptr
	MyGlobalUnlock (hGlbRes); lpMemRes = lpMemDataRes = NULL;

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0; 	// Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	goto NORMAL_EXIT;

ERROR_EXIT:
	if (hGlbRes)
	{
		if (lpMemRes)
		{
			// We no longer need this ptr
			MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
		} // End IF

		// We no longer need this storage
		DbgGlobalFree (hGlbRes); hGlbRes = NULL;
	} // End IF
NORMAL_EXIT:
	if (hGlbRes && lpMemRes)
	{
		// We no longer need this ptr
		MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
	} // End IF

	// We no longer need this ptr
	if (hGlbRht && lpMemRht)
	{
		MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
	} // End IF

	return lpYYRes;
} // End SysFnMonNC_EM
#undef	APPEND_NAME


//***************************************************************************
//	$ValidName
//
//	Determine if a name is validly constructed
//
//	A name is validly constructed if
//
//	 first:  QUAD | QUOTEQUAD | ALPHABETIC;
//	 second: ALPHABETIC | NUMERIC;
//	 name:	 OVERBAR | first second;
//
//***************************************************************************

BOOL ValidName
	(LPAPLCHAR lpaplChar,
	 UINT	   uLen)

{
	UINT uNam;

	// Check the first char
	if (lpaplChar[0] EQ UTF16_QUAD
	 || lpaplChar[0] EQ UTF16_QUOTEQUAD
	 || lpaplChar[0] EQ UTF16_DELTA
	 || lpaplChar[0] EQ UTF16_DELTAUNDERBAR
	 || lpaplChar[0] EQ UTF16_OVERBAR
	 || (L'a' <= lpaplChar[0]
	  &&		 lpaplChar[0] <= L'z')
	 || (L'A' <= lpaplChar[0]
	  &&		 lpaplChar[0] <= L'Z'))
	{
		// If the first char is overbar, it must be the only char
		if (lpaplChar[0] EQ UTF16_OVERBAR
		 && uLen NE 1)
			return FALSE;

		// Loop through the rest of the chars
		for (uNam = 1; uNam < uLen; uNam++)
		if (!(L'a' <= lpaplChar[uNam]
		   &&		  lpaplChar[uNam] <= L'z')
		  || (L'A' <= lpaplChar[uNam]
		   &&		  lpaplChar[uNam] <= L'Z')
		  || (L'0' <= lpaplChar[uNam]
		   &&		  lpaplChar[uNam] <= L'9')
		  || lpaplChar[0] EQ UTF16_DELTA
		  || lpaplChar[0] EQ UTF16_DELTAUNDERBAR)
			return FALSE;
		return TRUE;
	} // End IF

	return FALSE;
} // End ValidName


//***************************************************************************
//	$CalcNameClass
//
//	Calculate the name class of each element
//
//	-1 = Invalid name or unknown sysname
//	 0 = Available name
//	 1 = User label
//	 2 = User variable
//	 3 = User-defined function
//	 4 = User-defined operator (monadic or dyadic)
//	 5 = System variable
//	 6 = System function
//	 7 = System label
//***************************************************************************

APLINT CalcNameClass
	(LPSYMENTRY lpSymEntry)

{
	// Split cases based upon the Name Type
	switch (lpSymEntry->stFlags.ObjType)
	{
		case NAMETYPE_UNK:
			return NAMECLASS_AVL;

		case NAMETYPE_VAR:
			if (lpSymEntry->stFlags.DfnLabel)
				return NAMECLASS_USRLBL;
			else
			if (lpSymEntry->stFlags.DfnSysLabel)
				return NAMECLASS_SYSLBL;
			else
			if (lpSymEntry->stFlags.ObjName EQ OBJNAME_USR)
				return NAMECLASS_USRVAR;
			else
				return NAMECLASS_SYSVAR;

		case NAMETYPE_FN0:
		case NAMETYPE_FN12:
			if (lpSymEntry->stFlags.ObjName EQ OBJNAME_USR)
				return NAMECLASS_USRFCN;
			else
				return NAMECLASS_SYSFCN;

		case NAMETYPE_OP1:
		case NAMETYPE_OP2:
			return NAMECLASS_USROPR;

		case NAMETYPE_LST:
		defstop
			return NAMECLASS_INV;
	} // End SWITCH
} // End CalcNameClass


//***************************************************************************
//	$SysFnDydNC_EM
//
//	Dyadic []NC -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnDydNC_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydNC_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	return PrimFnSyntaxError_EM (lptkFunc);
} // End SysFnDydNC_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnNL_EM
//
//	System function:  []NL -- Name List
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnNL_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNL_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
	{
		ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
								   lptkAxis);
		return NULL;
	} // End IF

	// Split cases based upon monadic or dyadic
	if (lptkLftArg EQ NULL)
		return SysFnMonNL_EM (			  lptkFunc, lptkRhtArg, lptkAxis);
	else
		return SysFnDydNL_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnNL_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnMonNL_EM
//
//	Monadic []NL -- Name List (by Class)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnMonNL_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonNL_EM
	(LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	return SysFnDydNL_EM (NULL, 		// Ptr to left arg token
						  lptkFunc, 	// Ptr to function token
						  lptkRhtArg,	// Ptr to right arg token
						  lptkAxis);	// Ptr to axis token (may be NULL)
} // End SysFnMonNL_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnDydNL_EM
//
//	Dyadic []NL -- Name List (by Alphabet and Class)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnDydNC_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydNL_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	LPPL_YYSTYPE lpYYRes = NULL;




	return lpYYRes;
} // End SysFnDydNL_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnSIZE_EM
//
//	System function:  []SIZE -- Size of an identifier
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnSIZE_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnSIZE_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
	{
		ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
								   lptkAxis);
		return NULL;
	} // End IF

	// Split cases based upon monadic or dyadic
	if (lptkLftArg EQ NULL)
		return SysFnMonSIZE_EM (			lptkFunc, lptkRhtArg, lptkAxis);
	else
		return SysFnDydSIZE_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnSIZE_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnMonSIZE_EM
//
//	Monadic []SIZE -- Size of an identifier
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnMonSIZE_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonSIZE_EM
	(LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	APLSTYPE	 aplTypeRht;		// Right arg storage type
	APLNELM 	 aplNELMRht,		// Right arg NELM
				 aplNELMRes,		// Result	 ...
				 aplNELMCol;		// Result column NELM
	APLRANK 	 aplRankRht;		// Right arg Rank
	APLLONGEST	 aplLongestRht; 	// Right arg longest if immediate
	HGLOBAL 	 hGlbRht = NULL,	// Right arg global memory handle
				 hGlbRes = NULL;	// Result	 ...
	LPVOID		 lpMemRht = NULL,	// Ptr to right arg global memory
				 lpMemRes = NULL;	// Ptr to result	...
	LPAPLCHAR	 lpMemDataRht,		// Ptr to right arg char data
				 lpMemDataStart;	// Ptr to start of identifier
	LPAPLINT	 lpMemDataRes;		// Ptr to result integer data
	APLUINT 	 uRht,				// Loop counter
				 uCol,				// ...
				 ByteRes;			// # bytes in the result
	LPSYMENTRY	 lpSymEntry;		// Ptr to SYMENTRY
	STFLAGS 	 stFlags;			// STE flags
	LPPL_YYSTYPE lpYYRes = NULL;	// Ptr to the result

	// The right arg may be of three forms:
	//	 1.  a scalar	 name  as in 'a'
	//	 2.  a vector of names as in 'a b c'
	//	 3.  a matrix of names as in 3 1{rho}'abc'

	// Get the attributes (Type, NELM, and Rank)
	//	 of the right arg
	AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

	// Check for RANK ERROR
	if (aplRankRht > 2)
	{
		ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Check for DOMAIN ERROR
	if (!IsSimple (aplTypeRht)
	 || (aplTypeRht NE ARRAY_CHAR
	  && aplNELMRht NE 0))
	{
		ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Get right arg's global ptrs
	aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

	// Calculate the # identifiers in the argument
	//	 allowing for vector and matrix with multiple names
	CalcNumIDs (aplNELMRht, 		// Right arg NELM
				aplRankRht, 		// Right arg rank
				aplLongestRht,		// Right arg longest
				TRUE,				// TRUE iff we allow multiple names in a vector
				lpMemRht,			// Ptr to right arg global memory
			   &aplNELMRes, 		// Ptr to # right arg IDs
			   &aplNELMCol);		// Ptr to # right arg cols (matrix only)
	// Calculate space needed for the result
	ByteRes = CalcArraySize (ARRAY_INT, aplNELMRes, 1);

	// Allocate space for the result
	// N.B. Conversion from APLUINT to UINT
	Assert (ByteRes EQ (UINT) ByteRes);
	hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
	if (!hGlbRes)
	{
		ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
								   lptkFunc);
		goto ERROR_EXIT;
	} // End IF

	// Lock the memory to get a ptr to it
	lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader	((LPVARARRAY_HEADER) lpMemRes)

	// Fill in the header
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = ARRAY_INT;
////lpHeader->Perm		 = 0;			// Already zero from GHND
////lpHeader->SysVar	 = 0;			// Already zero from GHND
	lpHeader->RefCnt	 = 1;
	lpHeader->NELM		 = aplNELMRes;
	lpHeader->Rank		 = 1;

#undef	lpHeader

	// Fill in the dimension
	*VarArrayBaseToDim (lpMemRes) = aplNELMRes;

	// Skip over the header and dimensions to the data
	lpMemDataRes = VarArrayBaseToData (lpMemRes, 1);

	// Calculate the size of each element

	// Split cases based upon the right arg rank
	switch (aplRankRht)
	{
		case 0:
			// Lookup the name in the symbol table
			// SymTabLookupName sets the .ObjName enum,
			//	 and the .Inuse flag
			ZeroMemory (&stFlags, sizeof (stFlags));
			lpSymEntry = SymTabLookupNameLength ((LPAPLCHAR) &aplLongestRht,
												 1,
												&stFlags);
			// If not found, return zero
			if (!lpSymEntry)
				*lpMemDataRes++ = 0;
			else
				*lpMemDataRes++ = CalcSymentrySize (lpSymEntry);
			break;

		case 1:
			// Skip over the header and dimensions to the data
			lpMemDataRht = VarArrayBaseToData (lpMemRht, aplRankRht);

			// Loop through the right arg looking for identifiers
			uRht = 0;
			while (TRUE)
			{
				// Skip over white space
				while (uRht < aplNELMRht && lpMemDataRht[uRht] EQ L' ')
					uRht++;
				if (uRht < aplNELMRht)
				{
					// Save the starting ptr
					lpMemDataStart = &lpMemDataRht[uRht];

					// Skip over black space
					while (uRht < aplNELMRht && lpMemDataRht[uRht] NE L' ')
						uRht++;
					// Lookup the name in the symbol table
					// SymTabLookupName sets the .ObjName enum,
					//	 and the .Inuse flag
					ZeroMemory (&stFlags, sizeof (stFlags));
					lpSymEntry = SymTabLookupNameLength (lpMemDataStart,
														&lpMemDataRht[uRht] - lpMemDataStart,
														&stFlags);
					// If not found, return 0
					if (!lpSymEntry)
						*lpMemDataRes++ = 0;
					else
						*lpMemDataRes++ = CalcSymentrySize (lpSymEntry);
				} else
					break;
			} // End FOR

			break;

		case 2:
			// Skip over the header and dimensions to the data
			lpMemDataRht = VarArrayBaseToData (lpMemRht, aplRankRht);

			for (uRht = 0; uRht < aplNELMRes; uRht++)
			{
				// Point to the start of the data
				lpMemDataStart = &lpMemDataRht[aplNELMCol * uRht];

				// Skip over leading white space
				uCol = 0;
				while (uCol < aplNELMCol && lpMemDataStart[uCol] EQ L' ')
					uCol++;

				// Lookup the name in the symbol table
				// SymTabLookupName sets the .ObjName enum,
				//	 and the .Inuse flag
				ZeroMemory (&stFlags, sizeof (stFlags));
				lpSymEntry = SymTabLookupNameLength (&lpMemDataStart[uCol],
													  (UINT) (aplNELMCol - uCol),
													 &stFlags);
				// If not found, return 0
				if (!lpSymEntry)
					*lpMemDataRes++ = 0;
				else
					*lpMemDataRes++ = CalcSymentrySize (lpSymEntry);
			} // End FOR

			break;

		defstop
			break;
	} // End SWITCH

	// We no longer need this ptr
	MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0; 	// Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	goto NORMAL_EXIT;

ERROR_EXIT:
	if (hGlbRes && lpMemRes)
	{
		// We no longer need this ptr
		MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
	} // End IF

	if (hGlbRes)
	{
		// We no longer need this storage
		DbgGlobalFree (hGlbRes); hGlbRes = NULL;
	} // End IF
NORMAL_EXIT:
	if (hGlbRes && lpMemRes)
	{
		// We no longer need this ptr
		MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
	} // End IF

	// We no longer need this ptr
	if (hGlbRht && lpMemRht)
	{
		MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
	} // End IF

	return lpYYRes;
} // End SysFnMonSize_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnDydSIZE_EM
//
//	Dyadic []SIZE -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnDydSIZE_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydSIZE_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	return PrimFnSyntaxError_EM (lptkFunc);
} // End SysFnDydSIZE_EM
#undef	APPEND_NAME


//***************************************************************************
//	$CalcSymentrySize
//
//	Calculate the size of a SYMENTRY
//***************************************************************************

APLINT CalcSymentrySize
	(LPSYMENTRY lpSymEntry)

{
	APLUINT aplSize = 0;		// The result

	// If it's an immediate (any type) or an internal function, ...
	if (lpSymEntry->stFlags.Imm
	 || lpSymEntry->stFlags.FcnDir)
		aplSize = sizeof (SYMENTRY);
	else
	// If it has no value, ...
	if (!lpSymEntry->stFlags.Value)
		aplSize = 0;
	else
	// If it is a user variable, ...
	if (IsNameTypeVar (lpSymEntry->stFlags.ObjType))
	{
		// Start with the size of the SYMENTRY
		aplSize = sizeof (SYMENTRY);

		// Recurse through the array returning the total size
		aplSize += CalcGlbSize (lpSymEntry->stData.stGlbData);
	} else
	// If it is a user function/operator, ...
	if (IsNameTypeFnOp (lpSymEntry->stFlags.ObjType))
	{
		HGLOBAL 	 hGlbDfnHdr;		// User-defined function/operator header global memory handle
		LPDFN_HEADER lpMemDfnHdr;		// Ptr to user-defined function/operator header ...
		LPFCNLINE	 lpFcnLines;		// Ptr to array of function line structs (FCNLINE[numFcnLines])
		UINT		 uNumLines, 		// # function lines
					 uLine; 			// Loop counter

		// Get the global memory handle
		hGlbDfnHdr = lpSymEntry->stData.stGlbData;

		// stData is a valid HGLOBAL function array or user-defined function/operator
		Assert (IsGlbTypeFcnDir (hGlbDfnHdr)
			 || IsGlbTypeDfnDir (hGlbDfnHdr));

		// Clear the ptr type bits
		hGlbDfnHdr = ClrPtrTypeDirGlb (hGlbDfnHdr);

		// Split cases based upon the user-defined function/operator bit
		if (lpSymEntry->stFlags.UsrDfn)
		{
			// Lock the memory to get a ptr to it
			lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

			// Start with the size of the DFN_HEADER
			aplSize =	sizeof (DFN_HEADER)
					  + sizeof (LPSYMENTRY) * (lpMemDfnHdr->numResultSTE
											 + lpMemDfnHdr->numLftArgSTE
											 + lpMemDfnHdr->numRhtArgSTE
											 + lpMemDfnHdr->numLocalsSTE)
					  + sizeof (FCNLINE) * lpMemDfnHdr->numFcnLines;
			// Get ptr to array of function line structs (FCNLINE[numFcnLines])
			lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

			// Get # function lines
			uNumLines = lpMemDfnHdr->numFcnLines;

			// Loop through the function lines
			for (uLine = 0; uLine < uNumLines; uLine++)
			{
				if (lpFcnLines->hGlbTxtLine)
					aplSize += MyGlobalSize (lpFcnLines->hGlbTxtLine);

				if (lpFcnLines->hGlbTknLine)
					aplSize += MyGlobalSize (lpFcnLines->hGlbTknLine);

				if (lpFcnLines->hGlbMonInfo)
					aplSize += MyGlobalSize (lpFcnLines->hGlbMonInfo);

				// Skip to the next struct
				lpFcnLines++;
			} // End FOR

			// Add in the size of the function header text
			if (lpMemDfnHdr->hGlbTxtHdr)
				aplSize += MyGlobalSize (lpMemDfnHdr->hGlbTxtHdr);

			// Add in the size of the function header tokenized
			if (lpMemDfnHdr->hGlbTknHdr)
				aplSize += MyGlobalSize (lpMemDfnHdr->hGlbTknHdr);

			// Add in the size of the function Undo buffer
			if (lpMemDfnHdr->hGlbUndoBuff)
				aplSize += MyGlobalSize (lpMemDfnHdr->hGlbUndoBuff);

			// We no longer need this ptr
			MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
		} else
		// Otherwise, it's a function array
			// Start with the size of the SYMENTRY
			aplSize = sizeof (SYMENTRY)
					+ MyGlobalSize (ClrPtrTypeDirGlb (lpSymEntry->stData.stGlbData));
	} else
	// Otherwise, its size is zero
		aplSize = 0;

	return aplSize;
} // End CalcSymentrySize


//***************************************************************************
//	$CalcGlbSize
//
//	Calculate the size of a global memory variable
//***************************************************************************

APLUINT CalcGlbSize
	(HGLOBAL hGlbData)

{
	APLUINT 	aplSize = 0;		// The result
	LPAPLNESTED lpMemData;			// Ptr to the global memory
	APLSTYPE	aplType;			// Data storage type
	APLNELM 	aplNELM;			// Data NELM
	APLRANK 	aplRank;			// Data rank
	APLUINT 	uData;				// Loop counter

	// stData is a valid HGLOBAL variable array
	Assert (IsGlbTypeVarDir (hGlbData));

	hGlbData = ClrPtrTypeDirGlb (hGlbData);

	aplSize += MyGlobalSize (hGlbData);

	// Lock the memory to get a ptr to it
	lpMemData = MyGlobalLock (hGlbData);

#define lpHeader		((LPVARARRAY_HEADER) lpMemData)

	// Get the Array Type, NELM, and Rank
	aplType = lpHeader->ArrType;
	aplNELM = lpHeader->NELM;
	aplRank = lpHeader->Rank;

#undef	lpHeader

	// If the array is simple, that's all
	if (IsSimple (aplType))
		goto NORMAL_EXIT;

	// Skip over the header and dimensions to the data
	lpMemData = VarArrayBaseToData (lpMemData, aplRank);

	// Loop through the array adding the sizes
	for (uData = 0; uData < aplNELM; uData++)
	// Split cases based upon the pointer type
	switch (GetPtrTypeDir (lpMemData[uData]))
	{
		case PTRTYPE_STCONST:
			aplSize += sizeof (SYMENTRY);

			break;

		case PTRTYPE_HGLOBAL:
			aplSize += CalcGlbSize (lpMemData[uData]);

			break;

		defstop
			break;
	} // End FOR/SWITCH
NORMAL_EXIT:
	// We no longer need this ptr
	MyGlobalUnlock (hGlbData); lpMemData = NULL;

	return aplSize;
} // End CalcGlbSize


//***************************************************************************
//	$SysFnSYSID_EM
//
//	System function:  []SYSID -- System Identifier
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnSYSID_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnSYSID_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (should be NULL)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	UINT		 ByteRes;			// # bytes in the result
	HGLOBAL 	 hGlbRes;			// Result global memory handle
	LPVOID		 lpMemRes;			// Ptr to result global memory
	LPPL_YYSTYPE lpYYRes;			// Ptr to the result

	// This function is niladic
	Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
	{
		ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
								   lptkAxis);
		return NULL;
	} // End IF

#define SYSID	L"NARS2000"
#define SYSID_NELM	  (sizeof (SYSID) / sizeof (APLCHAR) - 1)

	// Calculate space needed for the result
	ByteRes = (UINT) CalcArraySize (ARRAY_CHAR, SYSID_NELM, 1);

	// Allocate space for the result
	hGlbRes = DbgGlobalAlloc (GHND, ByteRes);
	if (!hGlbRes)
	{
		ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Lock the memory to get a ptr to it
	lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader	((LPVARARRAY_HEADER) lpMemRes)

	// Fill in the header
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = ARRAY_CHAR;
////lpHeader->Perm		 = 0;			// Already zero from GHND
////lpHeader->SysVar	 = 0;			// Already zero from GHND
	lpHeader->RefCnt	 = 1;
	lpHeader->NELM		 = SYSID_NELM;
	lpHeader->Rank		 = 1;

#undef	lpHeader

	// Fill in the dimension
	*VarArrayBaseToDim (lpMemRes) = SYSID_NELM;

	// Skip over the header and dimensions to the data
	lpMemRes = VarArrayBaseToData (lpMemRes, 1);

	// Copy the SYSID to the result
	CopyMemory (lpMemRes, SYSID, SYSID_NELM * sizeof (APLCHAR));

	// We no longer need this ptr
	MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0; 	// Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	return lpYYRes;
} // End SysFnSYSID_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnSYSVER_EM
//
//	System function:  []SYSVER -- System Version
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnSYSVER_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnSYSVER_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (should be NULL)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	UINT		 ByteRes;			// # bytes in the result
	HGLOBAL 	 hGlbRes;			// Result global memory handle
	LPVOID		 lpMemRes;			// Ptr to result global memory
	char		 szFileVer[32]; 	//
	LPAPLCHAR	 p; 				//
	HANDLE		 hFile; 			//
	LPPL_YYSTYPE lpYYRes;			// Ptr to the result

	// This function is niladic
	Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
	{
		ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
								   lptkAxis);
		return NULL;
	} // End IF

#define SYSVER	L"0.00.001.0799  Tue Jan 16 17:43:45 2007  Win/32"
#define SYSVER_NELM    ((sizeof (SYSVER) / sizeof (APLCHAR)) - 1)

	// Calculate space needed for the result
	ByteRes = (UINT) CalcArraySize (ARRAY_CHAR, SYSVER_NELM, 1);

	// Allocate space for the result
	hGlbRes = DbgGlobalAlloc (GHND, ByteRes);
	if (!hGlbRes)
	{
		ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Lock the memory to get a ptr to it
	lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader	((LPVARARRAY_HEADER) lpMemRes)

	// Fill in the header
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = ARRAY_CHAR;
////lpHeader->Perm		 = 0;			// Already zero from GHND
////lpHeader->SysVar	 = 0;			// Already zero from GHND
	lpHeader->RefCnt	 = 1;
	lpHeader->NELM		 = SYSVER_NELM;
	lpHeader->Rank		 = 1;

#undef	lpHeader

	// Fill in the dimension
	*VarArrayBaseToDim (lpMemRes) = SYSVER_NELM;

	// Skip over the header and dimensions to the data
	lpMemRes = VarArrayBaseToData (lpMemRes, 1);

	p = (LPAPLCHAR) lpMemRes;

	// Read in the application's File Version String
	LclFileVersionStr (szAppDPFE, szFileVer);

	A2W (p, szFileVer, SYSVER_NELM);

	// Skip to the trailing zero
	p += lstrlenW (p);
	*p++ = L' ';    // Blank separators
	*p++ = L' ';

	// Open the executable file so we can read its internal timestamp
	hFile = CreateFile (szAppDPFE,				// lpFileName
						GENERIC_READ,			// dwDesiredAccess
						FILE_SHARE_READ,		// dwShareMode
						NULL,					// lpSecurityAttributes
						OPEN_EXISTING,			// dwCreationDistribution
						FILE_ATTRIBUTE_NORMAL,	// dwFlagsAndAttributes
						NULL);					// hTemplateFile
	if (hFile)
	{
		DWORD			   dwTemp,
						   dwCount;
		IMAGE_DOS_HEADER   idh;
		IMAGE_NT_HEADERS32 inth;

		// Set the file pointer to read the e_lfanew value
		SetFilePointer (hFile, ((LPBYTE) &idh.e_lfanew) - (LPBYTE) &idh, NULL, FILE_BEGIN);

		// Read in the e_lfanew value
		ReadFile (hFile, &dwTemp, sizeof (dwTemp), &dwCount, NULL);

		// Add in the distance to the file timestamp
		dwTemp += ((LPBYTE) (&inth.FileHeader.TimeDateStamp)) - (LPBYTE) &inth;

		// Set file pointer to the file timestamp
		SetFilePointer (hFile, dwTemp, NULL, FILE_BEGIN);

		// Read in the file timestamp
		ReadFile (hFile, &dwTemp, sizeof (dwTemp), &dwCount, NULL);

		A2W (p, ConvTime (dwTemp), SYSVER_NELM);

		// Skip to the trailing zero
		p += lstrlenW (p);
		*p++ = L' ';    // Blank separators
		*p++ = L' ';

#define SYSTYPE 	L"Win/32"

		CopyMemory (p, SYSTYPE, sizeof (SYSTYPE) - 1 * sizeof (APLCHAR));

		// We no longer need this handle
		CloseHandle (hFile); hFile = NULL;
	} // End IF

	// We no longer need this ptr
	MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0; 	// Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	return lpYYRes;
} // End SysFnSYSVER_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnTC_EM
//
//	System function:  []TC -- Terminal Control
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnTC_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnTC_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (should be NULL)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	UINT		 ByteRes;			// # bytes in the result
	HGLOBAL 	 hGlbRes;			// Result global memory handle
	LPVOID		 lpMemRes;			// Ptr to result global memory
	LPPL_YYSTYPE lpYYRes;			// Ptr to the result

	// This function is niladic
	Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
	{
		ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
								   lptkAxis);
		return NULL;
	} // End IF

	// Calculate space needed for the result
	ByteRes = (UINT) CalcArraySize (ARRAY_CHAR, 3, 1);

	// Allocate space for the result
	hGlbRes = DbgGlobalAlloc (GHND, ByteRes);
	if (!hGlbRes)
	{
		ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Lock the memory to get a ptr to it
	lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader	((LPVARARRAY_HEADER) lpMemRes)

	// Fill in the header
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = ARRAY_CHAR;
////lpHeader->Perm		 = 0;			// Already zero from GHND
////lpHeader->SysVar	 = 0;			// Already zero from GHND
	lpHeader->RefCnt	 = 1;
	lpHeader->NELM		 = 3;
	lpHeader->Rank		 = 1;

#undef	lpHeader

	// Fill in the dimension
	*VarArrayBaseToDim (lpMemRes) = 3;

	// Skip over the header and dimensions to the data
	lpMemRes = VarArrayBaseToData (lpMemRes, 1);

#define lpMemData	((LPAPLCHAR) lpMemRes)

	lpMemData[0] = TCBS;	// Backspace
	lpMemData[1] = TCNL;	// Newline
	lpMemData[2] = TCLF;	// Linefeed

#undef	lpMemData

	// We no longer need this ptr
	MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0; 	// Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	return lpYYRes;
} // End SysFnTC_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnTCCom_EM
//
//	System function:  []TCxxx -- Terminal Control, Common Routine
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnTCCOM_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnTCCom_EM
	(WCHAR	 wc,					// Char to return
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	LPPL_YYSTYPE lpYYRes;			// Ptr to the result

	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
	{
		ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
								   lptkAxis);
		return NULL;
	} // End IF

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
	lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_CHAR;
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkChar	   = wc;
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	return lpYYRes;
} // End SysFnTCCom_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnTCBEL_EM
//
//	System function:  []TCBEL -- Terminal Control, Bell
//***************************************************************************

LPPL_YYSTYPE SysFnTCBEL_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (should be NULL)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	// This function is niladic
	Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

	return SysFnTCCom_EM (TCBEL, lptkFunc, lptkAxis);
} // End SysFnTCBEL_EM


//***************************************************************************
//	$SysFnTCBS_EM
//
//	System function:  []TCBS -- Terminal Control, Backspace
//***************************************************************************

LPPL_YYSTYPE SysFnTCBS_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (should be NULL)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	// This function is niladic
	Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

	return SysFnTCCom_EM (TCBS, lptkFunc, lptkAxis);
} // End SysFnTCBS_EM


//***************************************************************************
//	$SysFnTCDEL_EM
//
//	System function:  []TCDEL -- Terminal Control, Del
//***************************************************************************

LPPL_YYSTYPE SysFnTCDEL_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (should be NULL)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	// This function is niladic
	Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

	return SysFnTCCom_EM (TCDEL, lptkFunc, lptkAxis);
} // End SysFnTCDEL_EM


//***************************************************************************
//	$SysFnTCESC_EM
//
//	System function:  []TCESC -- Terminal Control, Escape
//***************************************************************************

LPPL_YYSTYPE SysFnTCESC_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (should be NULL)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	// This function is niladic
	Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

	return SysFnTCCom_EM (TCESC, lptkFunc, lptkAxis);
} // End SysFnTCESC_EM


//***************************************************************************
//	$SysFnTCFF_EM
//
//	System function:  []TCFF -- Terminal Control, Form Feed
//***************************************************************************

LPPL_YYSTYPE SysFnTCFF_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (should be NULL)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	// This function is niladic
	Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

	return SysFnTCCom_EM (TCFF, lptkFunc, lptkAxis);
} // End SysFnTCFF_EM


//***************************************************************************
//	$SysFnTCHT_EM
//
//	System function:  []TCHT -- Terminal Control, Horizontal Tab
//***************************************************************************

LPPL_YYSTYPE SysFnTCHT_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (should be NULL)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	// This function is niladic
	Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

	return SysFnTCCom_EM (TCHT, lptkFunc, lptkAxis);
} // End SysFnTCHT_EM


//***************************************************************************
//	$SysFnTCLF_EM
//
//	System function:  []TCLF -- Terminal Control, Linefeed
//***************************************************************************

LPPL_YYSTYPE SysFnTCLF_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (should be NULL)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	// This function is niladic
	Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

	return SysFnTCCom_EM (TCLF, lptkFunc, lptkAxis);
} // End SysFnTCLF_EM


//***************************************************************************
//	$SysFnTCNL_EM
//
//	System function:  []TCNL -- Terminal Control, Newline
//***************************************************************************

LPPL_YYSTYPE SysFnTCNL_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (should be NULL)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	// This function is niladic
	Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

	return SysFnTCCom_EM (TCNL, lptkFunc, lptkAxis);
} // End SysFnTCNL_EM


//***************************************************************************
//	$SysFnTCNUL_EM
//
//	System function:  []TCNUL -- Terminal Control, Nul
//***************************************************************************

LPPL_YYSTYPE SysFnTCNUL_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (should be NULL)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	// This function is niladic
	Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

	return SysFnTCCom_EM (TCNUL, lptkFunc, lptkAxis);
} // End SysFnTCNUL_EM


//***************************************************************************
//	$SysFnTS_EM
//
//	System function:  []TS -- Time Stamp
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnTS_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnTS_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (should be NULL)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	SYSTEMTIME	 SystemTime;		// Struct for the system time
	UINT		 ByteRes;			// # bytes in the result
	HGLOBAL 	 hGlbRes;			// Result global memory handle
	LPVOID		 lpMemRes;			// Ptr to result global memory
	LPPL_YYSTYPE lpYYRes;			// Ptr to the result

	// This function is niladic
	Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
	{
		ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
								   lptkAxis);
		return NULL;
	} // End IF

	// Calculate space needed for the result
	ByteRes = (UINT) CalcArraySize (ARRAY_INT, 7, 1);

	// Allocate space for the result
	hGlbRes = DbgGlobalAlloc (GHND, ByteRes);
	if (!hGlbRes)
	{
		ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
								   lptkFunc);
		return NULL;
	} // End IF

	// Lock the memory to get a ptr to it
	lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader	((LPVARARRAY_HEADER) lpMemRes)

	// Fill in the header
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = ARRAY_INT;
////lpHeader->Perm		 = 0;			// Already zero from GHND
////lpHeader->SysVar	 = 0;			// Already zero from GHND
	lpHeader->RefCnt	 = 1;
	lpHeader->NELM		 = 7;
	lpHeader->Rank		 = 1;

#undef	lpHeader

	// Fill in the dimension
	*VarArrayBaseToDim (lpMemRes) = 7;

	// Skip over the header and dimensions to the data
	lpMemRes = VarArrayBaseToData (lpMemRes, 1);

	// Get the current time
	if (bUseLocalTime)
		GetLocalTime  (&SystemTime);
	else
		GetSystemTime (&SystemTime);

#define lpMemData	((LPAPLINT) lpMemRes)

	lpMemData[0] = SystemTime.wYear;
	lpMemData[1] = SystemTime.wMonth;
	lpMemData[2] = SystemTime.wDay;
	lpMemData[3] = SystemTime.wHour;
	lpMemData[4] = SystemTime.wMinute;
	lpMemData[5] = SystemTime.wSecond;
	lpMemData[6] = SystemTime.wMilliseconds;

#undef	lpMemData

	// We no longer need this ptr
	MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0; 	// Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	return lpYYRes;
} // End SysFnTS_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnTYPE_EM
//
//	System function:  []TYPE -- Prototype
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnTYPE_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnTYPE_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
	{
		ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
								   lptkAxis);
		return NULL;
	} // End IF

	// Split cases based upon monadic or dyadic
	if (lptkLftArg EQ NULL)
		return SysFnMonTYPE_EM (			lptkFunc, lptkRhtArg, lptkAxis);
	else
		return SysFnDydTYPE_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnTYPE_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnMonTYPE_EM
//
//	Monadic []TYPE -- Prototype
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnMonTYPE_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonTYPE_EM
	(LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	HGLOBAL 	 hGlbData,			//
				 hGlbRes;			// Result global memory handle
	LPPL_YYSTYPE lpYYRes;			// Ptr to the result

	// Allocate a new YYRes
	lpYYRes = YYAlloc ();

	// Split cases based upon the token type
	switch (lptkRhtArg->tkFlags.TknType)
	{
		case TKT_VARNAMED:
			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

			// If it's not immediate, it's an HGLOBAL
			if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
			{
				hGlbData = lptkRhtArg->tkData.tkSym->stData.stGlbData;

				break;		// Continue with HGLOBAL case
			} // End IF

			// Handle the immediate case

			// stData is an immediate
			Assert (lptkRhtArg->tkData.tkSym->stFlags.Imm);

			lpYYRes->tkToken.tkFlags.TknType = TKT_VARIMMED;
			lpYYRes->tkToken.tkFlags.ImmType = lptkRhtArg->tkData.tkSym->stFlags.ImmType;

			// Split cases based upon the token's immediate type
			switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
			{
				case IMMTYPE_BOOL:
					lpYYRes->tkToken.tkData.tkBoolean = 0;

					return lpYYRes;

				case IMMTYPE_INT:
					lpYYRes->tkToken.tkData.tkInteger = 0;

					return lpYYRes;

				case IMMTYPE_FLOAT:
					lpYYRes->tkToken.tkData.tkFloat   = 0;

					return lpYYRes;

				case IMMTYPE_CHAR:
					lpYYRes->tkToken.tkData.tkChar	  = L' ';

					return lpYYRes;

				defstop
					return NULL;
			} // End SWITCH

			DbgStop (); 		// We should never get here

		case TKT_VARIMMED:
			lpYYRes->tkToken.tkFlags.TknType = TKT_VARIMMED;
			lpYYRes->tkToken.tkFlags.ImmType = lptkRhtArg->tkFlags.ImmType;

			// Split cases based upon the token's immediate type
			switch (lptkRhtArg->tkFlags.ImmType)
			{
				case IMMTYPE_BOOL:
					lpYYRes->tkToken.tkData.tkBoolean = 0;

					return lpYYRes;

				case IMMTYPE_INT:
					lpYYRes->tkToken.tkData.tkInteger = 0;

					return lpYYRes;

				case IMMTYPE_FLOAT:
					lpYYRes->tkToken.tkData.tkFloat   = 0;

					return lpYYRes;

				case IMMTYPE_CHAR:
					lpYYRes->tkToken.tkData.tkChar	  = L' ';

					return lpYYRes;

				defstop
					return NULL;
			} // End SWITCH

			DbgStop (); 		// We should never get here

		case TKT_LISTPAR:
			ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
									   lptkFunc);
			YYFree (lpYYRes); lpYYRes = NULL; return NULL;

		case TKT_VARARRAY:
			hGlbData = lptkRhtArg->tkData.tkGlbData;

			break;		// Continue with HGLOBAL case

		defstop
			break;
	} // End SWITCH

	// HGLOBAL case

	// tk/stData is a valid HGLOBAL variable array
	Assert (IsGlbTypeVarDir (hGlbData));

	// Make the prototype
	hGlbRes = MakeMonPrototype_EM (ClrPtrTypeDirGlb (hGlbData),// Proto arg handle
								   lptkFunc,		// Ptr to function token
								   MP_CHARS);		// CHARs allowed
	if (!hGlbRes)
		return NULL;

	// Fill in the result token
	lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0; 	// Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
	lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	return lpYYRes;
} // End SysFnMonTYPE_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnDydTYPE_EM
//
//	Dyadic []TYPE -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnDydTYPE_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydTYPE_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	return PrimFnSyntaxError_EM (lptkFunc);
} // End SysFnDydTYPE_EM
#undef	APPEND_NAME


//***************************************************************************
//	$ConvTime
//
//	Convert to ASCII string in the form of
//	  Wed Jan 02 02:03:55 1980
//***************************************************************************

LPSTR ConvTime
	(DWORD time)
{
	LPSTR p;

	p = asctime (gmtime ((const time_t *) &time));

	// Zap the "\n" so it prints on one line
	p [lstrlen (p) - 1] = '\0';

	return p;
} // End ConvTime ()


//***************************************************************************
//	$SysFnUCS_EM
//
//	System function:  []UCS -- Universal Character Set
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnUCS_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnUCS_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (should be NULL)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token (should be NULL)
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	//***************************************************************
	// This function is not sensitive to the axis operator,
	//	 so signal a syntax error if present
	//***************************************************************

	if (lptkAxis NE NULL)
	{
		ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
								   lptkAxis);
		return NULL;
	} // End IF

	// Split cases based upon monadic or dyadic
	if (lptkLftArg EQ NULL)
		return SysFnMonUCS_EM ( 		   lptkFunc, lptkRhtArg, lptkAxis);
	else
		return SysFnDydUCS_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnUCS_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnMonUCS_EM
//
//	Monadic []UCS -- Universal Character Set
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnMonUCS_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonUCS_EM
	(LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	APLSTYPE	   aplTypeRht,			// Right arg storage type
				   aplTypeRes;			// Result	 ...
	APLNELM 	   aplNELMRht;			// Right arg NELM
	APLRANK 	   aplRankRht;			// Right arg rank
	HGLOBAL 	   hGlbRht = NULL,		// Right arg global memory handle
				   hGlbRes = NULL;		// Result	 ...
	APLLONGEST	   aplLongestRht;		// Right arg longest if immediate
	LPVOID		   lpMemRht = NULL, 	// Ptr to right arg global memory
				   lpMemRes = NULL; 	// Ptr to result	...
	LPPL_YYSTYPE   lpYYRes = NULL;		// Ptr to the result
	BOOL		   bRet;				// TRUE iff result if valid
	IMM_TYPES	   immTypeRes;			// Result immediate type
	APLINT		   apaOffRht,			// Right arg APA offset
				   apaMulRht;			// ...			 multiplier
	APLUINT 	   ByteRes, 			// # bytes in result
				   uRht;				// Loop counter
	UINT		   uBitMask;			// Bit mask for marching through Booleans
	APLHETERO	   aplHeteroRht;		// Right arg value as APLHETERO

	// Get the attributes (Type, NELM, and Rank)
	//	 of the right arg
	AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

	// Check for DOMAIN ERROR
	if (!IsSimple (aplTypeRht))
	{
		ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
								   lptkFunc);
		goto ERROR_EXIT;
	} // End IF

	// Get right arg's global ptrs
	aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

	// If the right arg/result is a scalar, ...
	if (aplRankRht EQ 0)
	{
		// If the right arg is a simple numeric scalar, ...
		if (IsSimpleNum (aplTypeRht))
		{
			// If the right arg is a simple float scalar, ...
			if (aplTypeRht EQ ARRAY_FLOAT)
			{
				// Attempt to convert the float to an integer
				aplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht, &bRet);
				if (!bRet)
				{
					ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
											   lptkFunc);
					goto ERROR_EXIT;
				} // End IF
			} // End IF

			// Check for out of range
			if (64*1024 <= aplLongestRht)
			{
				ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
										   lptkFunc);
				goto ERROR_EXIT;
			} // End IF

			// Set the result immediate type
			immTypeRes = IMMTYPE_CHAR;
		} else
			// Set the result immediate type
			immTypeRes = IMMTYPE_INT;

		// Allocate a new YYRes
		lpYYRes = YYAlloc ();

		// Fill in the result token
		lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
		lpYYRes->tkToken.tkFlags.ImmType   = immTypeRes;
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0; 	// Already zero from YYAlloc
		lpYYRes->tkToken.tkData.tkLongest  = aplLongestRht;
		lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

		goto NORMAL_EXIT;
	} // End IF

	// From here on, the result is a global memory handle

	// Split cases based upon the right arg storage type
	switch (aplTypeRht)
	{
		case ARRAY_BOOL:
		case ARRAY_INT:
		case ARRAY_FLOAT:
		case ARRAY_APA:
			aplTypeRes = ARRAY_CHAR;

			break;

		case ARRAY_CHAR:
			aplTypeRes = ARRAY_INT;

			break;

		case ARRAY_HETERO:
			aplTypeRes = ARRAY_HETERO;

			break;

		defstop
			break;
	} // End SWITCH

	// Calculate space needed for the result
	ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

	// Allocate space for the result
	hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
	if (!hGlbRes)
	{
		ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
								   lptkFunc);
		goto ERROR_EXIT;
	} // End IF

	// Lock the memory to get a ptr to it
	lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader		((LPVARARRAY_HEADER) lpMemRes)

	// Fill in the header
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = aplTypeRes;
////lpHeader->Perm		 = 0;			// Already zero from GHND
////lpHeader->SysVar	 = 0;			// Already zero from GHND
	lpHeader->RefCnt	 = 1;
	lpHeader->NELM		 = aplNELMRht;
	lpHeader->Rank		 = aplRankRht;

#undef	lpHeader

	// Copy the dimensions to the result
	CopyMemory (VarArrayBaseToDim (lpMemRes),
				VarArrayBaseToDim (lpMemRht),
				(UINT) aplRankRht * sizeof (APLDIM));
	// Skip over the header and dimensions to the data
	lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
	lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRht);

	// Split cases based upon the right arg storage type
	switch (aplTypeRht)
	{
		case ARRAY_BOOL:
			uBitMask = 0x01;

			for (uRht = 0; uRht < aplNELMRht; uRht++)
			{
				*((LPAPLCHAR) lpMemRes)++ = (APLCHAR) (uBitMask & *(LPAPLBOOL) lpMemRht);

				// Shift over the right arg bit mask
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
			for (uRht = 0; uRht < aplNELMRht; uRht++)
			{
				// Check for out of range as an APLUINT
				//	 so we don't have to deal with negatives
				if (64*1024 <= *(LPAPLUINT) lpMemRht)
				{
					ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
											   lptkFunc);
					goto ERROR_EXIT;
				} // End IF

				*((LPAPLCHAR) lpMemRes)++ = (APLCHAR) *((LPAPLINT) lpMemRht)++;
			} // End IF

			break;

		case ARRAY_FLOAT:
			for (uRht = 0; uRht < aplNELMRht; uRht++)
			{
				// Attempt to convert the float to an integer
				aplLongestRht = FloatToAplint_SCT (*((LPAPLFLOAT) lpMemRht)++, &bRet);
				if (!bRet)
				{
					ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
											   lptkFunc);
					goto ERROR_EXIT;
				} // End IF

				// Check for out of range as an APLUINT
				//	 so we don't have to deal with negatives
				if (64*1024 <= aplLongestRht)
				{
					ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
											   lptkFunc);
					goto ERROR_EXIT;
				} // End IF

				*((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestRht;
			} // End FOR

			break;

		case ARRAY_CHAR:
			for (uRht = 0; uRht < aplNELMRht; uRht++)
				*((LPAPLINT) lpMemRes)++ = *((LPAPLCHAR) lpMemRht)++;
			break;

		case ARRAY_APA:
#define lpAPA			((LPAPLAPA) lpMemRht)
			// Get the APA parameters
			apaOffRht = lpAPA->Off;
			apaMulRht = lpAPA->Mul;
#undef	lpAPA
			// Check for out of range
			if (0 > (apaOffRht + apaMulRht * 0)
			 || 	(apaOffRht + apaMulRht * (aplNELMRht - 1)) >= 64*1024)
			{
				ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
										   lptkFunc);
				goto ERROR_EXIT;
			} // End IF

			for (uRht = 0; uRht < aplNELMRht; uRht++)
				*((LPAPLCHAR) lpMemRes)++ = (APLCHAR) (apaOffRht + apaMulRht * uRht);
			break;

		case ARRAY_HETERO:
			for (uRht = 0; uRht < aplNELMRht; uRht++)
			{
				aplHeteroRht  = *((LPAPLHETERO) lpMemRht)++;
				aplLongestRht = aplHeteroRht->stData.stLongest;

				// Split cases based upon the immediate type
				switch (aplHeteroRht->stFlags.ImmType)
				{
					case IMMTYPE_BOOL:
						*((LPAPLHETERO) lpMemRes)++ = SymTabAppendChar_EM	 ((APLBOOL) aplLongestRht);

						break;

					case IMMTYPE_FLOAT:
						// Attempt to convert the float to an integer
						aplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht, &bRet);
						if (!bRet)
						{
							ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
													   lptkFunc);
							goto ERROR_EXIT;
						} // End IF

						// Fall through to common code

					case IMMTYPE_INT:
						// Check for out of range as an APLUINT
						//	 so we don't have to deal with negatives
						if (64*1024 <= aplLongestRht)
						{
							ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
													   lptkFunc);
							goto ERROR_EXIT;
						} // End IF

						*((LPAPLHETERO) lpMemRes)++ = SymTabAppendChar_EM	 ((APLCHAR) aplLongestRht);

						break;

					case IMMTYPE_CHAR:
						*((LPAPLHETERO) lpMemRes)++ = SymTabAppendInteger_EM ((APLCHAR) aplLongestRht);

						break;

					defstop
						break;
				} // End SWITCH
			} // End FOR

			break;

		defstop
			break;
	} // End SWITCH

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
	lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	goto NORMAL_EXIT;

ERROR_EXIT:
	if (hGlbRes)
	{
		if (lpMemRes)
		{
			// We no longer need this ptr
			MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
		} // End IF

		// We no longer need this storage
		DbgGlobalFree (hGlbRes); hGlbRes = NULL;
	} // End IF
NORMAL_EXIT:
	// We no longer need this ptr
	if (hGlbRht && lpMemRht)
	{
		MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
	} // End IF

	return lpYYRes;
} // End SysnMonUCS_EM
#undef	APPEND_NAME


//***************************************************************************
//	$SysFnDydUCS_EM
//
//	Dyadic []UCS -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnDydUCS_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydUCS_EM
	(LPTOKEN lptkLftArg,			// Ptr to left arg token
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	return PrimFnSyntaxError_EM (lptkFunc);
} // End SysFnDydUCS_EM
#undef	APPEND_NAME


//***************************************************************************
//	End of File: sysfns.c
//***************************************************************************
