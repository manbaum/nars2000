//***************************************************************************
//  NARS2000 -- Reference Count Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2009 Sudley Place Software

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
#include "headers.h"


#ifdef DEBUG
extern HGLOBAL hGlbRC1,
               hGlbRC2;
#endif


//***************************************************************************
//  $ChangeRefCntDir_PTB
//
//  Increment or decrement the reference count
//    of a direct reference to an LPSYMENTRY or an HGLOBAL
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ChangeRefCntDir_PTB"
#else
#define APPEND_NAME
#endif

int ChangeRefCntDir_PTB
    (HGLOBAL hGlb,              // Global memory handle
     int     iIncr)             // Increment/decrement amount

{
    LPVOID lpSig;               // Ptr to signature
    UINT   RefCnt;              // Reference count

    // Split cases based upon the ptr type
    switch (GetPtrTypeDir (hGlb))
    {
        case PTRTYPE_STCONST:
            return 1;

        case PTRTYPE_HGLOBAL:
            // Clear the ptr type bits
            hGlb = ClrPtrTypeDir (hGlb);
#ifdef DEBUG
            if (hGlb EQ ClrPtrTypeDir (hGlbRC1))
                DbgBrk ();

            if (hGlb EQ ClrPtrTypeDir (hGlbRC2))
                DbgBrk ();
#endif
            // Lock the memory to get a ptr to it
            lpSig = MyGlobalLock (hGlb);

            // Split cases based upon the array signature
            switch (GetSignatureMem (lpSig))
            {
                case VARARRAY_HEADER_SIGNATURE:
#define lpHeader        ((LPVARARRAY_HEADER) lpSig)
                    // Don't change the reference count on Perms
                    if (lpHeader->PermNdx NE PERMNDX_NONE)
                    {
#ifdef DEBUG_REFCNT
                        dprintfWL0 (L"  RefCntNC in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt, FNLN);
#endif
                        RefCnt = NEG1U;

                        break;
                    } // End IF

                    // Change the reference count
#ifdef DEBUG_REFCNT
                    if (iIncr EQ 1)
                        dprintfWL0 (L"  RefCnt++ in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                    if (iIncr EQ -1)
                        dprintfWL0 (L"  RefCnt-- in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                        DbgStop ();
#endif
                    Assert (iIncr NE -1 || lpHeader->RefCnt NE 0);

                    // If we're to skip this RefCnt increment, ...
                    if (iIncr EQ 1
                     && lpHeader->SkipRefCntIncr)
                        // Clear the flag
                        lpHeader->SkipRefCntIncr = FALSE;
                    else
                        lpHeader->RefCnt += iIncr;
                    RefCnt = lpHeader->RefCnt;
#undef  lpHeader
                    break;

                case FCNARRAY_HEADER_SIGNATURE:
#define lpHeader        ((LPFCNARRAY_HEADER) lpSig)
                    // Change the reference count
#ifdef DEBUG_REFCNT
                    if (iIncr EQ 1)
                        dprintfWL0 (L"  RefCnt++ in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                    if (iIncr EQ -1)
                        dprintfWL0 (L"  RefCnt-- in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                        DbgStop ();
#endif
                    Assert (iIncr NE -1 || lpHeader->RefCnt NE 0);
                    lpHeader->RefCnt += iIncr;
                    RefCnt = lpHeader->RefCnt;
#undef  lpHeader
                    break;

                case DFN_HEADER_SIGNATURE:
#define lpHeader        ((LPDFN_HEADER) lpSig)

                    // Don't change the reference count on permanent functions (i.e. Magic Functions/Operators)
                    if (lpHeader->PermFn)
                    {
#ifdef DEBUG_REFCNT
                        dprintfWL0 (L"  RefCntNC in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt, FNLN);
#endif
                        RefCnt = NEG1U;

                        break;
                    } // End IF

                    // Change the reference count
#ifdef DEBUG_REFCNT
                    if (iIncr EQ 1)
                        dprintfWL0 (L"  RefCnt++ in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                    if (iIncr EQ -1)
                        dprintfWL0 (L"  RefCnt-- in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                        DbgStop ();
#endif
                    Assert (iIncr NE -1 || lpHeader->RefCnt NE 0);
                    lpHeader->RefCnt += iIncr;
                    RefCnt = lpHeader->RefCnt;
#undef  lpHeader
                    break;

                defstop
                    return -1;
            } // End SWITCH

            MyGlobalUnlock (hGlb); lpSig = NULL;

            return RefCnt;

        defstop
            return -1;
    } // End SWITCH
} // End ChangeRefCntDir_PTB
#undef  APPEND_NAME


//***************************************************************************
//  $IncrRefCntDir_PTB
//
//  Increment the reference count of a direct reference
//    to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int IncrRefCntDir_PTB
    (HGLOBAL hGlb)

{
    return ChangeRefCntDir_PTB (hGlb, 1);
} // End IncrRefCntDir_PTB


//***************************************************************************
//  $IncrRefCntInd_PTB
//
//  Increment the reference count of an indirect reference
//    to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int IncrRefCntInd_PTB
    (LPVOID lpMem)

{
    return ChangeRefCntDir_PTB (*(HGLOBAL *) lpMem, 1);
} // End IncrRefCntInd_PTB


//***************************************************************************
//  $DecrRefCntDir_PTB
//
//  Decrement the reference count of a direct reference
//    to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int DecrRefCntDir_PTB
    (HGLOBAL hGlb)

{
    return ChangeRefCntDir_PTB (hGlb, -1);
} // End DecrRefCntDir_PTB


//***************************************************************************
//  $DecrRefCntInd_PTB
//
//  Decrement the reference count of an indirect reference
//    to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int DecrRefCntInd_PTB
    (LPVOID lpMem)

{
    return ChangeRefCntDir_PTB (*(HGLOBAL *) lpMem, -1);
} // End DecrRefCntInd_PTB


//***************************************************************************
//  $GetRefCntGlb
//
//  Get the reference count from a global memory handle
//***************************************************************************

UINT GetRefCntGlb
    (HGLOBAL hGlbArg)           // Arg global memory handle

{
    LPVOID lpMemHdr;            // Ptr to global memory header
    UINT   uRefCnt;             // The array reference count

    // Clear the ptr type bits
    hGlbArg = ClrPtrTypeDir (hGlbArg);

    // Lock the memory to get a ptr to it
    lpMemHdr = MyGlobalLock (hGlbArg);

    // Split cases based upon the signature
    switch (GetSignatureMem (lpMemHdr))
    {
        case DFN_HEADER_SIGNATURE:
            // Get the reference count
            uRefCnt = ((LPDFN_HEADER) lpMemHdr)->RefCnt;

            break;

        case FCNARRAY_HEADER_SIGNATURE:
            // Get the reference count
            uRefCnt = ((LPFCNARRAY_HEADER) lpMemHdr)->RefCnt;

            break;

        case VARARRAY_HEADER_SIGNATURE:
            // Get the reference count
            uRefCnt = ((LPVARARRAY_HEADER) lpMemHdr)->RefCnt;

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbArg); lpMemHdr = NULL;

    return uRefCnt;
} // End GetRefCntGlb


//***************************************************************************
//  End of File: refcnt.c
//***************************************************************************
