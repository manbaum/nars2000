//***************************************************************************
//	NARS2000 -- Goto Functions
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

#include "aplerrors.h"
#include "main.h"
#include "resdebug.h"
#include "externs.h"
#include "dfnhdr.h"
#include "pertab.h"
#include "sis.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//	$GotoLine_EM
//
//	Handle {goto} LineNum
//
//	Return EXITTYPE_GOTO_ZILDE if we're going to {zilde}
//		   EXITTYPE_GOTO_LINE  if we're going to a valid line num
//		   EXITTYPE_ERROR if error
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- GotoLine_EM"
#else
#define APPEND_NAME
#endif

EXIT_TYPES GotoLine_EM
	(LPTOKEN lptkRhtArg,		// Ptr to right arg token
	 LPTOKEN lptkFunc)			// Ptr to function token

{
	HGLOBAL 	 hGlbPTD;			// PerTabData global memory handle
	LPPERTABDATA lpMemPTD;			// Ptr to PerTabData global memory
	EXIT_TYPES	 exitType;			// Return value
	APLSTYPE	 aplTypeRht;		// Right arg storage type
	APLNELM 	 aplNELMRht;		// ...		 NELM
	APLRANK 	 aplRankRht;		// ...		 rank
	IMM_TYPES	 immType;			// Right arg first value immediate type
	APLINT		 aplIntegerRht; 	// First value as integer
	APLFLOAT	 aplFloatRht;		// ...			  float
	LPSIS_HEADER lpSISCur;			// Ptr to current SIS header

	// Get the thread's PerTabData global memory handle
	hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

	// Lock the memory to get a ptr to it
	lpMemPTD = MyGlobalLock (hGlbPTD);

	// Get the attributes (Type, NELM, and Rank)
	//	 of the right arg
	AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

	// Check for RANK ERROR
	if (aplRankRht > 1)
		goto RANK_EXIT;

	// Get the first value
	GetFirstValueToken (lptkRhtArg, 	// Ptr to right arg token
					   &aplIntegerRht,	// Ptr to integer result
					   &aplFloatRht,	// Ptr to float ...
						NULL,			// Ptr to WCHAR ...
						NULL,			// Ptr to longest ...
						NULL,			// Ptr to lpSym/Glb ...
					   &immType,		// Ptr to ...immediate type ...
						NULL);			// Ptr to array type ...
	if (immType EQ IMMTYPE_ERROR
	 || IsImmChr (immType))
		goto DOMAIN_EXIT;

	if (IsImmFlt (immType))
	{
		BOOL bRet;

		// Attempt to convert the float to an integer using System CT
		aplIntegerRht = FloatToAplint_SCT (aplFloatRht, &bRet);
		if (!bRet)
			goto DOMAIN_EXIT;
	} // End IF

	// If the right arg is empty or out of range, ...
	if (aplNELMRht EQ 0
	 || aplIntegerRht < 0
	 || aplIntegerRht > 0x7FFFFFFF)
	{
		exitType = EXITTYPE_GOTO_ZILDE;

		goto NORMAL_EXIT;
	} // End IF

	exitType = EXITTYPE_GOTO_LINE;

	// Split cases based upon the function type
	switch (lpMemPTD->lpSISCur->DfnType)
	{
		case DFNTYPE_IMM:		// Restart execution in a suspended function
			// If there's a suspended function, ...
			if (lpMemPTD->lpSISCur->lpSISPrv)
			{
				// Save as the next line #
				lpMemPTD->lpSISCur->lpSISPrv->NxtLineNum = (UINT) aplIntegerRht;

				// Mark as no longer suspended
				lpMemPTD->lpSISCur->lpSISPrv->Suspended = FALSE;
			} // End IF

			break;

		case DFNTYPE_OP1:
		case DFNTYPE_OP2:
		case DFNTYPE_FCN:		// Jump to a new line #
			// If there's a suspended function, ...
			if (lpMemPTD->lpSISCur)
			{
				// Save as the next line #
				lpMemPTD->lpSISCur->NxtLineNum = (UINT) aplIntegerRht;

				// Mark as no longer suspended
				lpMemPTD->lpSISCur->Suspended = FALSE;
			} // End IF

			break;

		case DFNTYPE_EXEC:
		case DFNTYPE_QUAD:
			// Peel back to the first non-Imm/Exec layer
			//	 starting with the previous SIS header
			lpSISCur = GetSISLayer (lpMemPTD->lpSISCur->lpSISPrv);

			// If there's a suspended user-defined function/operator, ...
			if (lpSISCur
			 && (lpSISCur->DfnType EQ DFNTYPE_OP1
			  || lpSISCur->DfnType EQ DFNTYPE_OP2
			  || lpSISCur->DfnType EQ DFNTYPE_FCN))
			{
				// Save as the next line #
				lpSISCur->NxtLineNum = (UINT) aplIntegerRht;

				// Mark as no longer suspended
				lpSISCur->Suspended = FALSE;

				// Save the suspended function's semaphore as the one to signal
				lpMemPTD->lpSISCur->hSigaphore = lpSISCur->hSemaphore;
			} // End IF

			break;

		defstop
			break;
	} // End SWITCH

	goto NORMAL_EXIT;

RANK_EXIT:
	ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
							   lptkFunc);
	goto ERROR_EXIT;

DOMAIN_EXIT:
	ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
							   lptkFunc);
	goto ERROR_EXIT;

ERROR_EXIT:
	// Mark as in error
	exitType = EXITTYPE_ERROR;
NORMAL_EXIT:
	// We no longer need this ptr
	MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

	return exitType;
} // End GotoLine_EM
#undef	APPEND_NAME


//***************************************************************************
//	End of File: goto.c
//***************************************************************************
