//***************************************************************************
//	NARS2000 -- Execution Functions
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
		switch (GetSignatureGlb (ClrPtrTypeDirAsGlb (lpNameFcn)))
		{
			case FCNARRAY_HEADER_SIGNATURE:
				// Execute a function array global memory handle
				return ExecFuncGlb_EM_YY (NULL, 							// Ptr to left arg token (may be NULL if monadic or niladic)
										  ClrPtrTypeDirAsGlb (lpNameFcn),	// Function array global memory handle
										  NULL, 							// Ptr to right arg token (may be NULL if niladic)
										  NULL);							// Ptr to axis token (may be NULL)
			case DFN_HEADER_SIGNATURE:
				// Execute a user-defined function/operator global memory handle
				return ExecDfnGlb_EM_YY (ClrPtrTypeDirAsGlb (lpNameFcn),	// User-defined function/operator global memory handle
										 NULL,								// Ptr to left arg token (may be NULL if monadic)
										 lpYYFcn0,							// Ptr to function strand
										 NULL,								// Ptr to right arg token
										 LINENUM_ONE);						// Starting line # (see LINE_NUMS)
			defstop
				return NULL;
		} // End SWITCH
} // End ExecuteFn0


//***************************************************************************
//	$ExecFunc_EM_YY
//
//	Execute a user-defined, system, or primitive function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ExecFunc_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecFunc_EM_YY
	(LPTOKEN	  lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPPL_YYSTYPE lpYYFcnStr,			// Ptr to function strand
	 LPTOKEN	  lptkRhtArg)			// Ptr to right arg token

{
	LPPRIMFNS lpPrimFn; 			// Ptr to direct primitive or system function
	LPTOKEN   lptkAxis; 			// Ptr to axis token (may be NULL)
	HGLOBAL   hGlbFcn;				// Function array global memory handle

	DBGENTER;

	Assert (lpYYFcnStr->YYInuse);

	// Check for NoValue
	if (IsTokenNoValue (lptkLftArg)
	 || IsTokenNoValue (lptkRhtArg))
		goto VALUE_EXIT;

	// Split cases based upon the function token type
	switch (lpYYFcnStr->tkToken.tkFlags.TknType)
	{
		case TKT_FCNIMMED:
			lpPrimFn = PrimFnsTab[SymTrans (&lpYYFcnStr->tkToken)];
			if (!lpPrimFn)
				goto SYNTAX_EXIT;

			return (*lpPrimFn) (lptkLftArg, &lpYYFcnStr->tkToken, lptkRhtArg, NULL);

		case TKT_FCNNAMED:
			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lpYYFcnStr->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

			// If the LPSYMENTRY is not immediate, it must be an HGLOBAL
			if (!lpYYFcnStr->tkToken.tkData.tkSym->stFlags.Imm)
			{
				STFLAGS stFlags;

				// Get the STE flags of the first token
				stFlags = lpYYFcnStr->tkToken.tkData.tkSym->stFlags;

				// Skip assertion if it's some kind of function/operator
				if (IsNameTypeFnOp (stFlags.ObjType))
				{
					LPPRIMFNS lpNameFcn;

					// Get the address of the execution routine
					lpNameFcn = lpYYFcnStr->tkToken.tkData.tkSym->stData.stNameFcn;

					// If it's an internal function, go there
					if (stFlags.FcnDir)
						return (*lpNameFcn) (lptkLftArg, &lpYYFcnStr->tkToken, lptkRhtArg, NULL);

					// Use the HGLOBAL
					hGlbFcn = lpNameFcn;
				} else
					// Use the HGLOBAL
					hGlbFcn = lpYYFcnStr->tkToken.tkData.tkSym->stData.stGlbData;

				// stData is a valid HGLOBAL function array
				//	 or user-defined function/operator
				Assert (IsGlbTypeFcnDir (hGlbFcn)
					 || IsGlbTypeDfnDir (hGlbFcn));

				// If it's a user-defined function/operator, ...
				if (stFlags.UsrDfn)
					return ExecDfnGlb_EM_YY (ClrPtrTypeDirAsGlb (hGlbFcn),	// User-defined function/operator global memory handle
											 lptkLftArg,					// Ptr to left arg token (may be NULL if monadic)
											 lpYYFcnStr,					// Ptr to function strand
											 lptkRhtArg,					// Ptr to right arg token
											 LINENUM_ONE);					// Starting line # (see LINE_NUMS)
				else
					// Execute a function array global memory handle
					return ExecFuncGlb_EM_YY (lptkLftArg,					// Ptr to left arg token (may be NULL if monadic or niladic)
											  ClrPtrTypeDirAsGlb (hGlbFcn), // Function array global memory handle
											  lptkRhtArg,					// Ptr to right arg token (may be NULL if niladic)
											  NULL);						// Ptr to axis token (may be NULL)
			} // End IF

			// Handle the immediate case

			// Split cases based upon the immediate type
			switch (lpYYFcnStr->tkToken.tkData.tkSym->stFlags.ImmType)
			{
				case IMMTYPE_PRIMFCN:
				{
					TOKEN tkFn = {0};

					lpPrimFn = PrimFnsTab[FcnTrans (lpYYFcnStr->tkToken.tkData.tkSym->stData.stChar)];
					if (!lpPrimFn)
						goto SYNTAX_EXIT;

					// Fill in for PrimFn*** test
					tkFn.tkFlags.TknType   = TKT_FCNIMMED;
					tkFn.tkFlags.ImmType   = GetImmTypeFcn (lpYYFcnStr->tkToken.tkData.tkSym->stData.stChar);
////////////////////tkFn.tkFlags.NoDisplay = 0; 	// Already zero from {0}
					tkFn.tkData.tkChar	   = lpYYFcnStr->tkToken.tkData.tkSym->stData.stChar;
					tkFn.tkCharIndex	   = lpYYFcnStr->tkToken.tkCharIndex;

					return (*lpPrimFn) (lptkLftArg, &tkFn, lptkRhtArg, NULL);
				} // End IMMTYPE_PRIMFCN

				defstop
					break;
			} // End SWITCH

		case TKT_FCNARRAY:
			// Get the HGLOBAL
			hGlbFcn = lpYYFcnStr->tkToken.tkData.tkGlbData;

			// tkData is a valid HGLOBAL function array
			//	 or user-defined function/operator
			Assert (IsGlbTypeFcnDir (hGlbFcn)
				 || IsGlbTypeDfnDir (hGlbFcn));

			// If it's a user-defined function/operator, ...
			switch (GetSignatureGlb (ClrPtrTypeDirAsGlb (hGlbFcn)))
			{
				case FCNARRAY_HEADER_SIGNATURE:
					// Check for axis operator
					lptkAxis = CheckAxisOper (lpYYFcnStr);

					// Execute a function array global memory handle
					return ExecFuncGlb_EM_YY (lptkLftArg,					// Ptr to left arg token (may be NULL if monadic)
											  ClrPtrTypeDirAsGlb (hGlbFcn), // Function array global memory handle
											  lptkRhtArg,					// Ptr to right arg token
											  lptkAxis);					// Ptr to axis token (may be NULL)
				case DFN_HEADER_SIGNATURE:
					// Execute a user-defined function/operator global memory handle
					return ExecDfnGlb_EM_YY (ClrPtrTypeDirAsGlb (hGlbFcn),	// User-defined function/operator global memory handle
											 lptkLftArg,					// Ptr to left arg token (may be NULL if monadic)
											 lpYYFcnStr,					// Ptr to function strand
											 lptkRhtArg,					// Ptr to right arg token
											 LINENUM_ONE);					// Starting line # (see LINE_NUMS)
				defstop
					break;
			} // End SWITCH

		defstop
			break;
	} // End SWITCH

	return NULL;

SYNTAX_EXIT:
	ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
							  &lpYYFcnStr->tkToken);
	return NULL;

VALUE_EXIT:
	ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
							   (lptkLftArg NE NULL) ? lptkLftArg
													: lptkRhtArg);
	return NULL;
} // End ExecFunc_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	$ExecFuncGlb_EM_YY
//
//	Execute a function in global memory
//***************************************************************************

LPPL_YYSTYPE ExecFuncGlb_EM_YY
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 HGLOBAL hGlbFcn,				// Handle to function strand
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	LPPL_YYSTYPE lpYYFcnStr,		// Ptr to function strand
				 lpYYRes;			// Ptr to the result

	// Lock the memory to get a ptr to it
	lpYYFcnStr = MyGlobalLock (hGlbFcn);

	// Skip over the header to the data (PL_YYSTYPEs)
	lpYYFcnStr = FcnArrayBaseToData (lpYYFcnStr);

	// The contents of the global memory object consist of
	//	 a series of PL_YYSTYPEs in RPN order.
	lpYYRes = ExecFuncStr_EM_YY (lptkLftArg,	// Ptr to left arg token
								 lpYYFcnStr,	// Ptr to function strand
								 lptkRhtArg,	// Ptr to right arg token
								 lptkAxis); 	// Ptr to axis token
	// We no longer need this ptr
	MyGlobalUnlock (hGlbFcn); lpYYFcnStr = NULL;

	return lpYYRes;
} // End ExecFuncGlb_EM_YY


//***************************************************************************
//	$ExecFuncStr_EM_YY
//
//	Execute a function strand
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ExecFuncStr_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecFuncStr_EM_YY
	(LPTOKEN	  lptkLftArg,		// Ptr to left arg token (may be NULL if monadic)
	 LPPL_YYSTYPE lpYYFcnStr,		// Ptr to function strand
	 LPTOKEN	  lptkRhtArg,		// Ptr to right arg token
	 LPTOKEN	  lptkAxis) 		// Ptr to axis token (may be NULL)

{
	LPPRIMFNS lpPrimFn; 			// Ptr to function address
	HGLOBAL   hGlbFcn;				// Function strand or user-defined function/operator global memory handle

	// Note that lptkAxis is not NULL for (say) L {take}[1]{each} R

	// Split cases based upon the type of the first token
	switch (lpYYFcnStr->tkToken.tkFlags.TknType)
	{
		case TKT_OP1IMMED:
		case TKT_OP1NAMED:
		case TKT_OP3IMMED:
		case TKT_OP3NAMED:
			return ExecOp1_EM_YY
				   (lptkLftArg, 	// Ptr to left arg token
					lpYYFcnStr, 	// Ptr to operator function strand
					lptkRhtArg);	// Ptr to right arg token

		case TKT_OP2IMMED:
		case TKT_OP2NAMED:
		case TKT_OPJOTDOT:
			return ExecOp2_EM_YY
				   (lptkLftArg, 	// Ptr to left arg token
					lpYYFcnStr, 	// Ptr to operator function strand
					lptkRhtArg);	// Ptr to right arg token

		case TKT_FCNIMMED:	// Either F or F[X]
			Assert (lpYYFcnStr->FcnCount EQ 1
				 || lpYYFcnStr->FcnCount EQ 2);

			// Check for axis operator
			lptkAxis = CheckAxisOper (lpYYFcnStr);

			lpPrimFn = PrimFnsTab[SymTrans (&lpYYFcnStr->tkToken)];
			if (!lpPrimFn)
				goto SYNTAX_EXIT;

			return (*lpPrimFn) (lptkLftArg, &lpYYFcnStr->tkToken, lptkRhtArg, lptkAxis);

		case TKT_FCNARRAY:
								// 1.  User-defined operator
								//	 e.g., Z{is}L (F FOO G) R
								//		   +foo- 1 2
								// 2.  User-defined function
								//	 e.g., Z{is}FOO R
								//		   3{jot}foo 1 2
								// 3.  User-defined function/operator w/axis operator
								//	 e.g., Z{is}FOO[X] R
								//		   foo[1] 1 2
								// 4.  Monadic operator derived function
								//	 e.g., f{is}/[1]
								//		   +f 2 3{rho}{iota}6
			// Get the global memory handle
			hGlbFcn = lpYYFcnStr->tkToken.tkData.tkGlbData;

			// tkData is a valid HGLOBAL function array
			//	 or user-defined function/operator
			Assert (IsGlbTypeFcnDir (hGlbFcn)
				 || IsGlbTypeDfnDir (hGlbFcn));

			// Split cases based upon the array signature
			switch (GetSignatureGlb (ClrPtrTypeDirAsGlb (hGlbFcn)))
			{
				case FCNARRAY_HEADER_SIGNATURE:
					// Check for axis operator
					lptkAxis = CheckAxisOper (lpYYFcnStr);

					// Execute a function array global memory handle
					return ExecFuncGlb_EM_YY (lptkLftArg,					// Ptr to left arg token (may be NULL if monadic)
											  ClrPtrTypeDirAsGlb (hGlbFcn), // Function array global memory handle
											  lptkRhtArg,					// Ptr to right arg token
											  lptkAxis);					// Ptr to axis token (may be NULL)
				case DFN_HEADER_SIGNATURE:
					// Execute a user-defined function/operator global memory handle
					return ExecDfnGlb_EM_YY (ClrPtrTypeDirAsGlb (hGlbFcn),	// User-defined function/operator global memory handle
											 lptkLftArg,					// Ptr to left arg token (may be NULL if monadic)
											 lpYYFcnStr,					// Ptr to function strand
											 lptkRhtArg,					// Ptr to right arg token
											 LINENUM_ONE);					// Starting line # (see LINE_NUMS)
				defstop
					break;
			} // End SWITCH

		case TKT_FCNNAMED:
			Assert (lpYYFcnStr->FcnCount EQ 1
				 || lpYYFcnStr->FcnCount EQ 2);

			// Check for axis operator
			lptkAxis = CheckAxisOper (lpYYFcnStr);

			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lpYYFcnStr->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

			// Get the global memory handle or function address if direct
			lpPrimFn = lpYYFcnStr->tkToken.tkData.tkSym->stData.stNameFcn;

			// Check for internal functions
			if (lpYYFcnStr->tkToken.tkData.tkSym->stFlags.FcnDir)
				return (*lpPrimFn) (lptkLftArg, &lpYYFcnStr->tkToken, lptkRhtArg, lptkAxis);

			// tkData is a valid HGLOBAL function array
			Assert (IsGlbTypeFcnDir (lpPrimFn));

			return ExecFuncGlb_EM_YY (lptkLftArg,						// Ptr to left arg token (may be NULL if monadic or niladic)
									  ClrPtrTypeDirAsGlb (lpPrimFn),	// Function array global memory handle
									  lptkRhtArg,						// Ptr to right arg token (may be NULL if niladic)
									  lptkAxis);						// Ptr to axis token (may be NULL)
		defstop
			return NULL;
	} // End SWITCH

SYNTAX_EXIT:
	ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
							  &lpYYFcnStr->tkToken);
	return NULL;
} // End ExecFuncStr_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	$ExecOp1_EM_YY
//
//	Execute a monadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ExecOp1_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecOp1_EM_YY
	(LPTOKEN	  lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPPL_YYSTYPE lpYYFcnStrOpr,		// Ptr to operator function strand
	 LPTOKEN	  lptkRhtArg)			// Ptr to right arg token

{
	// Split cases based upon the type of the monadic operator
	switch (lpYYFcnStrOpr->tkToken.tkData.tkChar)
	{
		case UTF16_SLASH:		// Reduction along the last coordinate
		case UTF16_SLASHBAR:	// Reduction along the first coordinate
		case INDEX_OPSLASH: 	// Reduction along the last coordinate
		case INDEX_OPSLASHBAR:	// Reduction along the first coordinate
			return PrimOpSlash_EM_YY (lptkLftArg,	// Ptr to left arg token (may be NULL if monadic)
									  lpYYFcnStrOpr,// Ptr to operator function strand
									  lptkRhtArg);	// Ptr to right arg token
		case UTF16_SLOPE:		// Scan along the last coordinate
		case UTF16_SLOPEBAR:	// Scan along the first coordinate
		case INDEX_OPSLOPE: 	// Scan along the last coordinate
		case INDEX_OPSLOPEBAR:	// Scan along the first coordinate
			return PrimOpSlope_EM_YY (lptkLftArg,	// Ptr to left arg token (may be NULL if monadic)
									  lpYYFcnStrOpr,// Ptr to operator function strand
									  lptkRhtArg);	// Ptr to right arg token
		case UTF16_STILETILDE:	// Partition
			return PrimFnNonceError_EM (&lpYYFcnStrOpr->tkToken);

			DbgBrk ();			// ***FINISHME*** -- UTF16_STILETILDE






			return NULL;

		case UTF16_DIERESIS:	// Each
			return PrimOpDieresis_EM_YY (lptkLftArg,		// Ptr to left arg token (may be NULL if monadic)
										 lpYYFcnStrOpr, 	// Ptr to operator function strand
										 lptkRhtArg);		// Ptr to right arg token
		case UTF16_DIERESISTILDE:	// Commute/Duplicate
			return PrimOpDieresisTilde_EM_YY (lptkLftArg,	// Ptr to left arg token (may be NULL if monadic)
											  lpYYFcnStrOpr,// Ptr to operator function strand
											  lptkRhtArg);	// Ptr to right arg token
		case UTF16_CIRCLEMIDDLEDOT: // Null Op
			return PrimOpCircleMiddleDot_EM_YY (lptkLftArg, 	// Ptr to left arg token (may be NULL if monadic)
												lpYYFcnStrOpr,	// Ptr to operator function strand
												lptkRhtArg);	// Ptr to right arg token
		defstop
			return NULL;
	} // End SWITCH
} // End ExecOp1_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	$ExecOp2_EM_YY
//
//	Execute a dyadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ExecOp2_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecOp2_EM_YY
	(LPTOKEN	  lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPPL_YYSTYPE lpYYFcnStrOpr,		// Ptr to operator function strand
	 LPTOKEN	  lptkRhtArg)			// Ptr to right arg token

{
	LPTOKEN lptkAxis;			// Ptr to axis token (may be NULL)

	// Check for axis operator
	lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

	// Split cases based upon the type of the dyadic operator
	switch (lpYYFcnStrOpr->tkToken.tkData.tkChar)
	{
		case UTF16_JOTDOT:		// Outer product
			return PrimOpJotDot_EM_YY (lptkLftArg,		// Ptr to left arg token (may be NULL if monadic)
									   lpYYFcnStrOpr,	// Ptr to operator function strand
									   lptkRhtArg); 	// Ptr to right arg token
		case UTF16_DOT: 		// Inner product
			return PrimOpDot_EM_YY	  (lptkLftArg,		// Ptr to left arg token (may be NULL if monadic)
									   lpYYFcnStrOpr,	// Ptr to operator function strand
									   lptkRhtArg); 	// Ptr to right arg token
		case UTF16_JOT: 		// Compose
			return PrimOpJot_EM_YY	  (lptkLftArg,		// Ptr to left arg token (may be NULL if monadic)
									   lpYYFcnStrOpr,	// Ptr to operator function strand
									   lptkRhtArg); 	// Ptr to right arg token
		case UTF16_DIERESISDEL: // Dual
			return PrimFnNonceError_EM (&lpYYFcnStrOpr->tkToken);

			DbgBrk ();			// ***FINISHME*** -- UTF16_DIERESISDEL






			return NULL;		// break;

		case UTF16_DIERESISJOT: // Rank
			return PrimFnNonceError_EM (&lpYYFcnStrOpr->tkToken);

			DbgBrk ();			// ***FINISHME*** -- UTF16_DIERESISJOT






			return NULL;		// break;

		case UTF16_DIERESISDOWNTACK:// Convolution
			return PrimFnNonceError_EM (&lpYYFcnStrOpr->tkToken);

			DbgBrk ();			// ***FINISHME*** -- UTF16_DIERESISDOWNTACK






			return NULL;		// break;

		case UTF16_DIERESISSTAR:// Power
			return PrimFnNonceError_EM (&lpYYFcnStrOpr->tkToken);

			DbgBrk ();			// ***FINISHME*** -- UTF16_DIERESISSTAR






			return NULL;		// break;

		defstop
			return NULL;
	} // End SWITCH
} // End ExecOp2_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	End of File: execfns.c
//***************************************************************************
