//***************************************************************************
//  NARS2000 -- Reference Count Routines
//***************************************************************************

#include <windows.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $ChangeRefCntDir
//
//  Increment or decrement the reference count
//    of a direct reference to an LPSYMENTRY or an HGLOBAL
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ChangeRefCntDir"
#else
#define APPEND_NAME
#endif

int ChangeRefCntDir
    (LPVOID lpMem,
     int    iIncr)

{
    LPVOID lpSign;
    UINT   RefCnt;

    // Split cases based upon the ptr type
    switch (GetPtrTypeDir (lpMem))
    {
        case PTRTYPE_STCONST:
            return 1;

        case PTRTYPE_HGLOBAL:
            // Get a ptr to the global memory
            lpSign = MyGlobalLock (ClrPtrTypeDirGlb (lpMem));

            // Split cases based upon the array signature
            switch (((LPHEADER_SIGNATURE) lpSign)->ature)
            {
                case VARARRAY_HEADER_SIGNATURE:
#define lpHeader        ((LPVARARRAY_HEADER) lpSign)
                    // Don't change the reference count on Perms
                    if (lpHeader->Perm)
                    {
#ifdef DEBUG
                        dprintfW (L"  RefCntNC in " APPEND_NAME L": %08X(res=%d) (%S#%d)", lpHeader, lpHeader->RefCnt, FNLN);
#endif
                        RefCnt = (UINT) -1;

                        break;
                    } // End IF

                    // Change the reference count
#ifdef DEBUG
                    dprintfW (L"  RefCnt   in " APPEND_NAME L": %08X(res=%d) (%S#%d)", lpHeader, lpHeader->RefCnt + iIncr, FNLN);
#endif
                    Assert (iIncr NE -1 || lpHeader->RefCnt NE 0);
                    lpHeader->RefCnt += iIncr;
                    RefCnt = lpHeader->RefCnt;
#undef  lpHeader
                    break;

                case FCNARRAY_HEADER_SIGNATURE:
#define lpHeader        ((LPFCNARRAY_HEADER) lpSign)
                    // Change the reference count
#ifdef DEBUG
                    dprintfW (L"  RefCnt   in " APPEND_NAME L": %08X(res=%d) (%S#%d)", lpHeader, lpHeader->RefCnt + iIncr, FNLN);
#endif
                    Assert (iIncr NE -1 || lpHeader->RefCnt NE 0);
                    lpHeader->RefCnt += iIncr;
                    RefCnt = lpHeader->RefCnt;
#undef  lpHeader
                    break;

                defstop
                    return -1;
            } // End SWITCH

            MyGlobalUnlock (ClrPtrTypeDirGlb (lpMem)); lpMem = NULL;

            return RefCnt;

        defstop
            return -1;
    } // End SWITCH
} // End ChangeRefCntDir
#undef  APPEND_NAME


//***************************************************************************
//  $IncrRefCntDir
//
//  Increment the reference count of a direct reference
//    to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int IncrRefCntDir
    (LPVOID lpMem)

{
    return ChangeRefCntDir (lpMem, 1);
} // End IncrRefCntDir


//***************************************************************************
//  $IncrRefCntInd
//
//  Increment the reference count of an indirect reference
//    to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int IncrRefCntInd
    (LPVOID lpMem)

{
    return ChangeRefCntDir (*(LPVOID *) lpMem, 1);
} // End IncrRefCntInd


//***************************************************************************
//  $DecrRefCntDir
//
//  Decrement the reference count of a direct reference
//    to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int DecrRefCntDir
    (LPVOID lpMem)

{
    return ChangeRefCntDir (lpMem, -1);
} // End DecrRefCntDir


//***************************************************************************
//  $DecrRefCntInd
//
//  Decrement the reference count of an indirect reference
//    to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int DecrRefCntInd
    (LPVOID lpMem)

{
    return ChangeRefCntDir (*(LPVOID *) lpMem, -1);
} // End DecrRefCntInd


//***************************************************************************
//  End of File: refcnt.c
//***************************************************************************
