//***************************************************************************
//	NARS2000 -- Primitive Function -- DownShoe
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
#include "execmfn.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//	$PrimFnDownShoe_EM_YY
//
//	Primitive function for monadic and dyadic DownShoe ("unique" and ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimFnDownShoe_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDownShoe_EM_YY
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	// Ensure not an overflow function
	Assert (lptkFunc->tkData.tkChar EQ UTF16_DOWNSHOE);

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
		return PrimFnMonDownShoe_EM_YY (			lptkFunc, lptkRhtArg, lptkAxis);
	else
		return PrimFnDydDownShoe_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
SYNTAX_EXIT:
	ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
							   lptkAxis);
	return NULL;
} // End PrimFnDownShoe_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	$PrimProtoFnDownShoe_EM_YY
//
//	Generate a prototype for the primitive functions monadic & dyadic DownShoe
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimProtoFnDownShoe_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnDownShoe_EM_YY
	(LPTOKEN lptkLftArg,			// Ptr to left arg token (may be NULL if monadic)
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	//***************************************************************
	// Called monadically or dyadically
	//***************************************************************

	// Convert to a prototype
	return PrimProtoFnMixed_EM_YY (&PrimFnDownShoe_EM_YY,	// Ptr to primitive function routine
									lptkLftArg, 			// Ptr to left arg token
									lptkFunc,				// Ptr to function token
									lptkRhtArg, 			// Ptr to right arg token
									lptkAxis);				// Ptr to axis token (may be NULL)
} // End PrimProtoFnDownShoe_EM
#undef	APPEND_NAME


//***************************************************************************
//	$PrimFnMonDownShoe_EM_YY
//
//	Primitive function for monadic DownShoe ("unique")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimFnMonDownShoe_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonDownShoe_EM_YY
	(LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	APLRANK 	 aplRankRht;		// Right arg rank
	HGLOBAL 	 hGlbPTD;			// PerTabData global memory handle
	LPPERTABDATA lpMemPTD;			// Ptr to PerTabData global memory
	LPPL_YYSTYPE lpYYRes = NULL;	// Prt to result

	// Get the right arg rank
	AttrsOfToken (lptkRhtArg, NULL, NULL, &aplRankRht, NULL);

	// Check for RIGHT RANK ERROR
	if (aplRankRht > 1)
		goto RANK_EXIT;

	// Get the PerTabData global memory handle
	hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

	// Lock the memory to get a ptr to it
	lpMemPTD = MyGlobalLock (hGlbPTD);

	//	Return the unique elements in the right arg.
	//	Use an internal magic function.
	lpYYRes =
	  ExecuteMagicFunction_EM_YY (NULL, 						// Ptr to left arg token
								  lptkFunc, 					// Ptr to function token
								  lptkRhtArg,					// Ptr to right arg token
								  lptkAxis, 					// Ptr to axis token
								  lpMemPTD->hGlbMF_MonUpShoe);	// Magic function global memory handle
	// We no longer need this ptr
	MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

	goto NORMAL_EXIT;

RANK_EXIT:
	ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
							   lptkFunc);
	goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
	return lpYYRes;
} // End PrimFnMonDownShoe_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	Magic function for monadic DownShoe
//
//	Monadic DownShoe -- Unique
//
//	On scalar or vector right args, return the unique values
//***************************************************************************

static APLCHAR Header[] =
  $Z $IS $F L" " $R;

static APLCHAR Line1[] =
  $Z $IS L"((" $R $IOTA $R L")=" $IOTA $RHO $R L")/" $R $IS L"," $R;

static LPAPLCHAR Body[] =
{Line1,
};

MAGIC_FUNCTION MF_MonUpShoe =
{Header,
 Body,
 sizeof (Body) / sizeof (Body[0]),
};


//***************************************************************************
//	$PrimFnDydDownShoe_EM_YY
//
//	Primitive function for dyadic DownShoe (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- PrimFnDydDownShoe_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydDownShoe_EM_YY
	(LPTOKEN lptkLftArg,			// Ptr to left arg token
	 LPTOKEN lptkFunc,				// Ptr to function token
	 LPTOKEN lptkRhtArg,			// Ptr to right arg token
	 LPTOKEN lptkAxis)				// Ptr to axis token (may be NULL)

{
	return PrimFnValenceError_EM (lptkFunc);
} // End PrimFnDydDownShoe_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	End of File: pf_dshoe.c
//***************************************************************************
