//***************************************************************************
//	NARS2000 -- System Function -- Quad LC
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
#include "sis.h"
#include "dfnhdr.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//	$SysFnLC_EM_YY
//
//	System function:  []LC -- Line Counter
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- SysFnLC_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnLC_EM_YY
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
	hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

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
////lpHeader->PermNdx	 = PERMNDX_NONE;// Already zero from GHND
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
	lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
	lpYYRes->tkToken.tkCharIndex	   = lptkFunc->tkCharIndex;

	// We no longer need this ptr
	MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

	return lpYYRes;
} // End SysFnLC_EM_YY
#undef	APPEND_NAME


//***************************************************************************
//	End of File: qf_lc.c
//***************************************************************************
