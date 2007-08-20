//***************************************************************************
//	NARS2000 -- Reference Count Routines
//***************************************************************************

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
			lpSig = MyGlobalLock (ClrPtrTypeDirGlb (hGlb));

			// Split cases based upon the array signature
			switch (((LPHEADER_SIGNATURE) lpSig)->nature)
			{
				case VARARRAY_HEADER_SIGNATURE:
#define lpHeader		((LPVARARRAY_HEADER) lpSig)
					// Don't change the reference count on Perms
					if (lpHeader->Perm)
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

			MyGlobalUnlock (ClrPtrTypeDirGlb (hGlb)); lpSig = NULL;

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
