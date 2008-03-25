//***************************************************************************
//	NARS2000 -- Reference Count Routines
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

#include <windows.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"
#include "dfnhdr.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//	$ChangeRefCntDir
//
//	Increment or decrement the reference count
//	  of a direct reference to an LPSYMENTRY or an HGLOBAL
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ChangeRefCntDir"
#else
#define APPEND_NAME
#endif

int ChangeRefCntDir
	(HGLOBAL hGlb,				// Global memory handle
	 int	 iIncr) 			// Increment/decrement amount

{
	LPVOID lpSig;				// Ptr to signature
	UINT   RefCnt;				// Reference count

	// Split cases based upon the ptr type
	switch (GetPtrTypeDir (hGlb))
	{
		case PTRTYPE_STCONST:
			return 1;

		case PTRTYPE_HGLOBAL:
			// Lock the memory to get a ptr to it
			lpSig = MyGlobalLock (ClrPtrTypeDirAsGlb (hGlb));

			// Split cases based upon the array signature
			switch (((LPHEADER_SIGNATURE) lpSig)->nature)
			{
				case VARARRAY_HEADER_SIGNATURE:
#define lpHeader		((LPVARARRAY_HEADER) lpSig)
					// Don't change the reference count on Perms
					if (lpHeader->PermNdx NE PERMNDX_NONE)
					{
#ifdef DEBUG
////////////////////////dprintfW (L"  RefCntNC in " APPEND_NAME L": %08X(res=%d) (%S#%d)", lpHeader, lpHeader->RefCnt, FNLN);
#endif
						RefCnt = NEG1U;

						break;
					} // End IF

					// Change the reference count
#ifdef DEBUG
////////////////////dprintfW (L"  RefCnt   in " APPEND_NAME L": %08X(res=%d) (%S#%d)", lpHeader, lpHeader->RefCnt + iIncr, FNLN);
#endif
					Assert (iIncr NE -1 || lpHeader->RefCnt NE 0);
					lpHeader->RefCnt += iIncr;
					RefCnt = lpHeader->RefCnt;
#undef	lpHeader
					break;

				case FCNARRAY_HEADER_SIGNATURE:
#define lpHeader		((LPFCNARRAY_HEADER) lpSig)
					// Change the reference count
#ifdef DEBUG
////////////////////dprintfW (L"  RefCnt   in " APPEND_NAME L": %08X(res=%d) (%S#%d)", lpHeader, lpHeader->RefCnt + iIncr, FNLN);
#endif
					Assert (iIncr NE -1 || lpHeader->RefCnt NE 0);
					lpHeader->RefCnt += iIncr;
					RefCnt = lpHeader->RefCnt;
#undef	lpHeader
					break;

				case DFN_HEADER_SIGNATURE:
#define lpHeader		((LPDFN_HEADER) lpSig)

					// Don't change the reference count on permanent functions (i.e. Magic Functions)
					if (lpHeader->PermFn)
					{
#ifdef DEBUG
////////////////////////dprintfW (L"  RefCntNC in " APPEND_NAME L": %08X(res=%d) (%S#%d)", lpHeader, lpHeader->RefCnt, FNLN);
#endif
						RefCnt = NEG1U;

						break;
					} // End IF
#ifdef DEBUG
////////////////////dprintfW (L"  RefCnt   in " APPEND_NAME L": %08X(res=%d) (%S#%d)", lpHeader, lpHeader->RefCnt + iIncr, FNLN);
#endif
					Assert (iIncr NE -1 || lpHeader->RefCnt NE 0);
					lpHeader->RefCnt += iIncr;
					RefCnt = lpHeader->RefCnt;
#undef	lpHeader
					break;

				defstop
					return -1;
			} // End SWITCH

			MyGlobalUnlock (ClrPtrTypeDirAsGlb (hGlb)); lpSig = NULL;

			return RefCnt;

		defstop
			return -1;
	} // End SWITCH
} // End ChangeRefCntDir
#undef	APPEND_NAME


//***************************************************************************
//	$IncrRefCntDir
//
//	Increment the reference count of a direct reference
//	  to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int IncrRefCntDir
	(LPVOID lpMem)

{
	return ChangeRefCntDir (lpMem, 1);
} // End IncrRefCntDir


//***************************************************************************
//	$IncrRefCntInd
//
//	Increment the reference count of an indirect reference
//	  to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int IncrRefCntInd
	(LPVOID lpMem)

{
	return ChangeRefCntDir (*(LPVOID *) lpMem, 1);
} // End IncrRefCntInd


//***************************************************************************
//	$DecrRefCntDir
//
//	Decrement the reference count of a direct reference
//	  to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int DecrRefCntDir
	(LPVOID lpMem)

{
	return ChangeRefCntDir (lpMem, -1);
} // End DecrRefCntDir


//***************************************************************************
//	$DecrRefCntInd
//
//	Decrement the reference count of an indirect reference
//	  to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int DecrRefCntInd
	(LPVOID lpMem)

{
	return ChangeRefCntDir (*(LPVOID *) lpMem, -1);
} // End DecrRefCntInd


//***************************************************************************
//	End of File: refcnt.c
//***************************************************************************
