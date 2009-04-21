//***************************************************************************
//  NARS2000 -- Hash & Symbol Table Routines
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
#include <stdio.h>
#include <math.h>
#include "headers.h"
#include "symbolnames.h"


// **FIXME*** -- Distinguish between blocks (buckets) and entries
// as well as size vs. blocks


//***************************************************************************
//  $MaskTheHash
//
//  Calculate the hash mask for a given hash.
//
//  A family of hash masks consists of a sequence of numbers each of
//    which is a contiguous sequence of low-order 1s and each of which
//    is 1+2* the previous hash mask (i.e., (2^N)-1 for successive Ns)
//    (e.g. 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF, etc.).
//
//  As per the family of hash masks defined by Linear Hashing,
//    the hash mask starts out as DEF_HSHTAB_HASHMASK, lpHshTabSplitNext
//    points to the start of the hash table (lpHshTab), and the base size is
//    set to the size of the hash table (DEF_HSHTAB_INITNELM * sizeof (lpHshTab[0])).
//  As entries are split, lpHshTabSplitNext increments by iHshTabEPB
//    or DEF_GLBHSHTAB_EPB until it reaches the base size.
//  When lpHshTabSplitNext reaches the base size, the base NELM is
//    incremented by DEF_HSHTAB_INITNELM, lpHshTabSplitNext is reset
//    to point to the start of the hash table, and the hash mask is shifted
//    by one to the left filling with a 1 on the right.  As per the preceding
//    paragraph, the next split triggers a resize.
//
//  Finally, if the hash value AND the current hash mask yield an index
//    which is less than lpHshTabSplitNext, then the hash value is masked
//    with the next higher-order hash mask.
//***************************************************************************

UINT MaskTheHash
    (UINT        uHash,                 // The hash to mask
     LPHSHTABSTR lpHTS)                 // Ptr to HshTab struc

{
    UINT uHashMasked;                   // Masked value of uHash

    // Initial try
    uHashMasked = uHash & lpHTS->uHashMask;

    // The result is multiplied by iHshTabEPB because
    //   the resulting index should fall on a PrinHash entry.
    // That is, it's essentially a block index.
    if (uHashMasked < (UINT) ((lpHTS->lpHshTabSplitNext - lpHTS->lpHshTab) / lpHTS->iHshTabEPB))
    {
        // Because this hash is below lpHshTabSplitNext,
        //   we use the next hash function in sequence.
        uHashMasked = uHash & (1 + 2 * lpHTS->uHashMask);
        Assert (NULL NE lpHTS->lpHshTab[uHashMasked].NextSameHash);
        Assert (NULL NE lpHTS->lpHshTab[uHashMasked].PrevSameHash);
        Assert (uHashMasked < (UINT) (     lpHTS->iHshTabTotalNelm / lpHTS->iHshTabEPB));
        Assert (uHashMasked < (UINT) ((2 * lpHTS->iHshTabBaseNelm) / lpHTS->iHshTabEPB));
    } else
    {
        Assert (uHashMasked < (UINT) (     lpHTS->iHshTabTotalNelm / lpHTS->iHshTabEPB));
        Assert (uHashMasked < (UINT) ((2 * lpHTS->iHshTabBaseNelm) / lpHTS->iHshTabEPB));
    } // End IF/ELSE

    return lpHTS->iHshTabEPB * uHashMasked;
} // End MaskTheHash


#ifdef DEBUG
//***************************************************************************
//  $HshTabFrisk
//
//  Frisk the hash table
//***************************************************************************

UBOOL HshTabFrisk
    (LPHSHTABSTR  lpHTS)                // Ptr to HshTab struc

{
    LPHSHENTRY   lp,                    // Ptr to temporary HSHENTRY
                 lpNext,                // ...
                 lpBase,                // ...
                 lpEnd;                 // ...
    LPSYMENTRY   lps;                   // Ptr to temporary SYMENTRY
    HTFLAGS      htFlags;               // Local copy of HTE flags
    UINT         uHashMaskNext,         // Mask for next higher hash function
                 uMark;                 // Point above which all HTEs use a higher hash function
    static UBOOL bFrisk = FALSE;        // TRUE iff we're already frisking
    UBOOL        bRet = FALSE;          // TRUE iff the result is valid

    EnterCriticalSection (&CSOFrisk);

    // Avoid recursion
    if (bFrisk)
        bRet = TRUE;
    else
        bFrisk = TRUE;

    LeaveCriticalSection (&CSOFrisk);

    if (bRet)
        return bRet;

    __try
    {
        // Get hash mask for next higher hash function
        uHashMaskNext = 1 + 2 * lpHTS->uHashMask;

        // Ensure that the NextSameHash and PrevSameHash chains are valid
        for (lp = lpHTS->lpHshTab;
             lp NE &lpHTS->lpHshTab[lpHTS->iHshTabTotalNelm];
             lp++)
        {
            if (lp->NextSameHash NE LPHSHENTRY_NONE)
                Assert (lp EQ lp->NextSameHash->PrevSameHash);

            if (lp->PrevSameHash NE LPHSHENTRY_NONE)
                Assert (lp EQ lp->PrevSameHash->NextSameHash);
        } // End FOR

        // Ensure that all uHashAndMask values are up-to-date
        for (lp = lpHTS->lpHshTab;
             lp NE &lpHTS->lpHshTab[lpHTS->iHshTabTotalNelm];
             lp++)
        if (lp->htFlags.Inuse)
            Assert (lp->uHashAndMask EQ MaskTheHash (lp->uHash, lpHTS));

        // Ensure that all empty entries have zero flags
        //   (possibly except for .PrinHash),
        //   LPHSHENTRY_NONE in the two hash ptrs (unless .PrinHash),
        //   and zero everywhere else
        for (lp = lpHTS->lpHshTab;
             lp NE &lpHTS->lpHshTab[lpHTS->iHshTabTotalNelm];
             lp++)
        if (!lp->htFlags.Inuse)
        {
            htFlags = lp->htFlags;
            htFlags.PrinHash = FALSE;

            Assert (0 EQ *(UINT *) &htFlags);

            if (!lp->htFlags.PrinHash)
                Assert (lp->NextSameHash EQ LPHSHENTRY_NONE
                     && lp->PrevSameHash EQ LPHSHENTRY_NONE);
            Assert (lp->uHash           EQ 0
                 && lp->uHashAndMask    EQ 0
                 && lp->htGlbName       EQ NULL
                 && lp->htSymEntry      EQ NULL);
        } // End FOR/IF

        // Ensure that .PrinHash is set every iHshTabEPB entries
        //   and nowhere else
        for (lp = lpHTS->lpHshTab;
             lp NE &lpHTS->lpHshTab[lpHTS->iHshTabTotalNelm];
             lp++)
        {
            if (lp->htFlags.PrinHash)
                // Ensure .PrinHash is on a proper boundary
                Assert (0 EQ ((lp - lpHTS->lpHshTab) % lpHTS->iHshTabEPB));
            else
                Assert (!lp->htFlags.PrinHash);
        } // End FOR

        // Ensure that every HTE is in some NextSameHash chain starting
        //   from a .PrinHash entry, or is empty
        for (lp = lpHTS->lpHshTab;
             lp NE &lpHTS->lpHshTab[lpHTS->iHshTabTotalNelm];
             lp += lpHTS->iHshTabEPB)
        {
            // Ensure that all chains start at .PrinHash
            Assert (lp->PrevSameHash EQ LPHSHENTRY_NONE);

            // Trundle through the NextSameHash chain
            for (lpNext = lp;
                 lpNext NE LPHSHENTRY_NONE;
                 lpNext = lpNext->NextSameHash)
            {
                // Ensure not already set
                Assert (!lpNext->htFlags.Temp);

                // Set it
                lpNext->htFlags.Temp = TRUE;
            } // End FOR
        } // End FOR

        // Check and unset .Temp entries
        for (lp = lpHTS->lpHshTab;
             lp NE &lpHTS->lpHshTab[lpHTS->iHshTabTotalNelm];
             lp++)
        {
            Assert ((!lp->htFlags.Inuse)
                 || lp->htFlags.Temp);
            lp->htFlags.Temp = FALSE;
        } // End FOR

        uMark = (UINT) (lpHTS->lpHshTabSplitNext - lpHTS->lpHshTab);

        // Ensure that principal entries below lpHshTabSplitNext hashed with the
        //   next higher hash function all hash to below lpHshTabSplitNext.
        // In other words, we moved all of those which hashed to a higher bucket.
        for (lp  = lpHTS->lpHshTab;
             lp NE lpHTS->lpHshTabSplitNext;
             lp += lpHTS->iHshTabEPB)
        for (lpNext = lp;
             lpNext NE LPHSHENTRY_NONE;
             lpNext = lpNext->NextSameHash)
        if (lpNext->htFlags.Inuse)
        {
            Assert (((lpNext->uHash & uHashMaskNext) * lpHTS->iHshTabEPB) EQ MaskTheHash (lpNext->uHash, lpHTS));
            Assert (((lpNext->uHash & uHashMaskNext) * lpHTS->iHshTabEPB) < uMark);
            if (!(((lpNext->uHash & uHashMaskNext) * lpHTS->iHshTabEPB) < uMark))
            {
                DisplayHshTab (lpHTS);
                MBC ("DisplayHshTab");
            } // End IF
        } // End IF

        lpBase = &lpHTS->lpHshTabSplitNext[lpHTS->iHshTabBaseNelm];
        lpEnd  = &lpHTS->lpHshTab[lpHTS->iHshTabTotalNelm];

        uMark = (UINT) (lpBase - lpHTS->lpHshTab);

        // Ensure that principal entries at or above lpBase hashed with the
        //   next higher hash function all hash to at or above lpBase.
        // In other words, we moved only those which hashed to a higher bucket.
        for (lp  = lpBase;
             lp NE lpEnd;
             lp += lpHTS->iHshTabEPB)
        for (lpNext = lp;
             lpNext NE LPHSHENTRY_NONE;
             lpNext = lpNext->NextSameHash)
        if (lpNext->htFlags.Inuse)
            Assert (((lpNext->uHash & uHashMaskNext) * lpHTS->iHshTabEPB) >= uMark);

        // Ensure that the symbol table ptrs point back to the hash table
        for (lp = lpHTS->lpHshTab;
             lp NE &lpHTS->lpHshTab[lpHTS->iHshTabTotalNelm];
             lp++)
        if (lp->htFlags.Inuse)
        {
            if (lp->htFlags.CharIsValid)
            {
                Assert (lp->htFlags.htChar NE 0
                     && lp->htGlbName NE NULL
                     && lp->htSymEntry EQ NULL);
            } else
            {
                if (!(lp->htSymEntry
                 && lp->htSymEntry->stHshEntry EQ lp))
                {
                    DisplayHshTab (lpHTS);
                    DisplaySymTab (lpHTS, TRUE);
                    if (lp->htSymEntry EQ 0)
                        MBC ("HshTabFrisk:  lp->htSymEntry EQ 0");
                    else
                        MBC ("HshTabFrisk:  lp->htSymEntry->stHshEntry NE lp");
                } // End IF
            } // End IF/ELSE
        } // End FOR

        // If there's no global symbol table, ...
        if (!lpHTS->bGlbHshTab)
        {
            // Ensure that the hash table ptrs point back to the symbol table
            for (lps = lpHTS->lpSymTab;
                 lps NE lpHTS->lpSymTabNext;
                 lps++)
            // Check for NoValue entry
            if (IsSymNoValue (lps))
                continue;
            else
            if (lps->stHshEntry EQ 0)
            {
                DisplayHshTab (lpHTS);
                DisplaySymTab (lpHTS, TRUE);
                MBC ("HshTabFrisk:  lps->stHshEntry EQ 0");
            } else
            if (lps->stHshEntry->htFlags.CharIsValid)
            {
                Assert (lps->stHshEntry->htFlags.htChar NE 0
                     && lps->stHshEntry->htGlbName NE NULL
                     && lps->stHshEntry->htSymEntry EQ NULL);
            } else
            if (lps->stHshEntry->htSymEntry NE lps)
            {
                DisplayHshTab (lpHTS);
                DisplaySymTab (lpHTS, TRUE);
                MBC ("HshTabFrisk:  lps->stHshEntry->htSymEntry NE lps");
            } // End FOR/IF/...
        } // End IF
    } __except (CheckException (GetExceptionInformation (), L"HshTabFrisk"))
    {
        // Display message for unhandled exception
        DisplayException ();
    } // End __try/__except

    EnterCriticalSection (&CSOFrisk);

    bFrisk = FALSE;

    LeaveCriticalSection (&CSOFrisk);

    return TRUE;
} // End HshTabFrisk
#endif


//***************************************************************************
//  $HshTabDelink
//
//  Delink a hash table entry
//***************************************************************************

void HshTabDelink
    (LPHSHENTRY lpHshEntry)

{
    // We must not delink a .PrinHash entry as that
    //   may be the only link to a non-.PrinHash entry
    if (lpHshEntry->htFlags.PrinHash)
        return;

    // The following commented out entries express
    //   what we'd like to do if we didn't have to
    //   check for LPHSHENTRY_NONE.
////lpHshEntry->NextSameHash->PrevSameHash = lpHshEntry->PrevSameHash;
////lpHshEntry->PrevSameHash->NextSameHash = lpHshEntry->NextSameHash;
////lpHshEntry->NextSameHash =
////lpHshEntry->PrevSameHash = LPSYMENTRY_NONE;

    if (lpHshEntry->NextSameHash NE LPHSHENTRY_NONE)
    {
        Assert (lpHshEntry->PrevSameHash EQ LPHSHENTRY_NONE
             || !lpHshEntry->NextSameHash->htFlags.PrinHash);
        lpHshEntry->NextSameHash->PrevSameHash = lpHshEntry->PrevSameHash;//**
    } // End IF

    if (lpHshEntry->PrevSameHash NE LPHSHENTRY_NONE)
        lpHshEntry->PrevSameHash->NextSameHash = lpHshEntry->NextSameHash;

    lpHshEntry->NextSameHash =
    lpHshEntry->PrevSameHash = LPHSHENTRY_NONE;
} // End HshTabDelink


//***************************************************************************
//  $HshTabLink
//
//  Link the destination entry into the hash entry
//***************************************************************************

void HshTabLink
    (LPHSHENTRY lpHshEntryHash,
     LPHSHENTRY lpHshEntryDest)

{
    // This entry must be a principal one
    Assert (lpHshEntryHash->htFlags.PrinHash);

    // Ensure they have the same masked hash
    Assert (lpHshEntryHash->uHashAndMask EQ lpHshEntryDest->uHashAndMask);

    // Something to do only if they are different entries
    if (lpHshEntryDest NE lpHshEntryHash)
    {
        Assert (lpHshEntryDest->NextSameHash EQ LPHSHENTRY_NONE
              && lpHshEntryDest->PrevSameHash EQ LPHSHENTRY_NONE);

        // The following commented out entries express
        //   what we'd like to do if we didn't have to
        //   check for LPHSHENTRY_NONE.
////////lpHshEntryDest->PrevSameHash = lpHshEntryHash;
////////lpHshEntryDest->NextSameHash = lpHshEntryHash->NextSameHash;
////////lpHshEntryHash->NextSameHash->PrevSameHash = lpHshEntryDest;
////////lpHshEntryHash->NextSameHash = lpHshEntryDest;
////////lpHshEntryHash->PrevSameHash = LPHSHENTRY_NONE;     // Already set

        Assert (lpHshEntryHash EQ LPHSHENTRY_NONE
             || !lpHshEntryDest->htFlags.PrinHash);
        lpHshEntryDest->PrevSameHash = lpHshEntryHash;//**
        if (lpHshEntryHash NE LPHSHENTRY_NONE)
        {
            lpHshEntryDest->NextSameHash = lpHshEntryHash->NextSameHash;
            if (lpHshEntryHash->NextSameHash NE LPHSHENTRY_NONE)
            {
                Assert (lpHshEntryDest EQ LPHSHENTRY_NONE
                     || !lpHshEntryHash->NextSameHash->htFlags.PrinHash);
                lpHshEntryHash->NextSameHash->PrevSameHash = lpHshEntryDest;//**
            } // End IF

            lpHshEntryHash->NextSameHash = lpHshEntryDest;
        } else
        {
            // The only way we can set the NextSameHash ptr to NONE
            //   is if this is a .PrinHash entry.
            Assert (lpHshEntryDest->htFlags.PrinHash);

            lpHshEntryDest->NextSameHash = LPHSHENTRY_NONE;
        } // End IF/ELSE
    } // End IF
} // End HshTabLink


//***************************************************************************
//  $HshTabResize_EM
//
//  Resize the hash table
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- HshTabResize_EM"
#else
#define APPEND_NAME
#endif

UBOOL HshTabResize_EM
    (LPHSHTABSTR lpHTS)                 // Ptr to HshTab struc

{
    LPHSHENTRY   lpHshTabNew;
    int          iResize,               // Resize value (in # HTEs)
                 i,                     // Loop counter
                 iHshTabNewSize;        // New # HTEs in the HshTab
    UBOOL        bRet = FALSE;          // TRUE iff result is valid

    // Get the resize value
    iResize = lpHTS->iHshTabIncrNelm;

    Assert (HshTabFrisk (lpHTS));
#ifdef DEBUG
    if (TlsGetValue (dwTlsPerTabData))
        dprintfW (L"||| Resizing the hash table from %u to %u (%S#%d)",
                  lpHTS->iHshTabTotalNelm,
                  lpHTS->iHshTabTotalNelm + iResize,
                  FNLN);
#endif
    // We need more entries
    iHshTabNewSize = lpHTS->iHshTabTotalNelm + iResize;
    lpHshTabNew =
      MyVirtualAlloc (lpHTS->lpHshTab,
                      iHshTabNewSize * sizeof (lpHTS->lpHshTab[0]),
                      MEM_COMMIT,
                      PAGE_READWRITE);
    if (!lpHshTabNew)
        goto HSHTAB_FULL_EXIT;

    Assert (lpHshTabNew EQ lpHTS->lpHshTab);

    // Initialize the principal hash entry (1st one in each block).
    for (i = lpHTS->iHshTabTotalNelm; i < iHshTabNewSize; i += lpHTS->iHshTabEPB)
        lpHTS->lpHshTab[i].htFlags.PrinHash = TRUE;

    // Initialize the next & prev same hash STE values
    for (i = lpHTS->iHshTabTotalNelm; i < iHshTabNewSize; i++)
    {
        lpHTS->lpHshTab[i].NextSameHash =
        lpHTS->lpHshTab[i].PrevSameHash = LPHSHENTRY_NONE;
    } // End FOR

    // Set new hash table size
    lpHTS->iHshTabTotalNelm = iHshTabNewSize;

    // Because iHshTabTotalNelm changed, we need to change iHshTabIncr
    lpHTS->iHshTabIncrFree = DEF_HSHTAB_PRIME % (UINT) lpHTS->iHshTabTotalNelm;

    Assert (1 EQ gcdAplInt (lpHTS->iHshTabIncrFree, lpHTS->iHshTabTotalNelm, NULL));

    Assert (HshTabFrisk (lpHTS));

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

HSHTAB_FULL_EXIT:
    ErrorMessageIndirect (ERRMSG_HASH_TABLE_FULL APPEND_NAME);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return bRet;
} // End HshTabResize_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SymTabResize_EM
//
//  Resize the symbol table
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SymTabResize_EM"
#else
#define APPEND_NAME
#endif

UBOOL SymTabResize_EM
    (LPHSHTABSTR lpHTS)                 // Ptr to HshTab struc

{
    LPSYMENTRY   lpSymTabNew;           // Ptr to the re-allocated Symbol Table
    int          iResize,               // Resize value (in # STEs)
                 i,                     // Loop counter
                 iSymTabNewNelm;        // New # STEs in SymTab
    UBOOL        bRet = FALSE;          // TRUE iff result is valid

    // Get the resize value
    iResize = lpHTS->uSymTabIncrNelm;

    Assert (HshTabFrisk (lpHTS));
#ifdef DEBUG
    if (TlsGetValue (dwTlsPerTabData))
        DbgMsgW (L"||| Resizing the symbol table");
#endif
    // We need more entries
    iSymTabNewNelm = lpHTS->iSymTabTotalNelm + iResize;
    lpSymTabNew =
      MyVirtualAlloc (lpHTS->lpSymTab,
                      iSymTabNewNelm * sizeof (lpHTS->lpSymTab[0]),
                      MEM_COMMIT,
                      PAGE_READWRITE);
    if (!lpSymTabNew)
        goto SYMTAB_FULL_EXIT;

    Assert (lpSymTabNew EQ lpHTS->lpSymTab);

    // Initialize the stHshEntry, stPrvEntry, stSymLink, and stSILevel values
    for (i = lpHTS->iSymTabTotalNelm; i < iSymTabNewNelm; i++)
    {
        lpHTS->lpSymTab[i].stHshEntry = LPHSHENTRY_NONE;
        lpHTS->lpSymTab[i].stPrvEntry =
        lpHTS->lpSymTab[i].stSymLink  = NULL;
        lpHTS->lpSymTab[i].stSILevel  = 0;
    } // End FOR

    // Set new symbol table NELM
    lpHTS->iSymTabTotalNelm = iSymTabNewNelm;

    Assert (HshTabFrisk (lpHTS));

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

SYMTAB_FULL_EXIT:
    ErrorMessageIndirect (ERRMSG_SYMBOL_TABLE_FULL APPEND_NAME);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return bRet;
} // End SymTabResize_EM
#undef  APPEND_NAME


//***************************************************************************
//  $HshTabSplitNextEntry
//
//  Split the entry pointed to by lpHshTabSplitNext .
//
//  This routine is called whenever we attempt to append a new entry
//    and the bucket to which it hashes is full.  Note that we do not
//    split the bucket which is full, but the bucket pointed to by
//    lpHshTabSplitNext (which might be the same as the full bucket,
//    but not likely).
//***************************************************************************

UBOOL HshTabSplitNextEntry_EM
    (UINT        uHash,                 // The hash value
     LPHSHTABSTR lpHTS)                 // Ptr to HshTab struc

{
    LPHSHENTRY   lpHshEntrySrc,         // Ptr to source HSHENTRY
                 lpHshEntryNext,        // Ptr to next   ...
                 lpHshEntryDest,        // Ptr to destination ...
                 lpHshEntryHash;        // Ptr to split + base ...
    int          i, iCnt;               // Loop counters
#ifdef DEBUG
////int          iCntMoved = 0;
#endif
    UINT         uHashMaskNext;         // Mask for next higher hash function
    UBOOL        bRet = TRUE;           // TRUE iff result is valid

    Assert (HshTabFrisk (lpHTS));
#ifdef DEBUG
    if (TlsGetValue (dwTlsPerTabData))
        dprintfW (L"||| Splitting Hash Table entry %u to %u (%S#%d)",
                  lpHTS->lpHshTabSplitNext,
                 &lpHTS->lpHshTabSplitNext[lpHTS->iHshTabBaseNelm],
                  FNLN);
#endif
    // Ensure that &lpHshTabSplitNext[iHshTabBaseNelm] has been allocated.
    // If not, allocate it now
    if (&lpHTS->lpHshTabSplitNext[lpHTS->iHshTabBaseNelm] >= &lpHTS->lpHshTab[lpHTS->iHshTabTotalNelm]
     && !HshTabResize_EM (lpHTS))
    {
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF

    Assert (&lpHTS->lpHshTabSplitNext[lpHTS->iHshTabBaseNelm] < &lpHTS->lpHshTab[lpHTS->iHshTabTotalNelm]);

    // Now we can split the entry pointed by lpHshTabSplitNext.
    // Splitting this entry means, remasking the entries using
    //   the next higher hash function, and moving the ones
    //   which hash to the larger bucket #.

    // Count the # entries in the chain
    for (lpHshEntrySrc = lpHTS->lpHshTabSplitNext,
           iCnt = 0;
         lpHshEntrySrc NE LPHSHENTRY_NONE;
         iCnt++,
           lpHshEntrySrc = lpHshEntrySrc->NextSameHash)
    {}

    // Get hash mask for next higher hash function
    uHashMaskNext = 1 + 2 * lpHTS->uHashMask;

    // Save the hash entry
    lpHshEntryHash = &lpHTS->lpHshTabSplitNext[lpHTS->iHshTabBaseNelm];

    // Move the entries which hash to a higher bucket to
    //   &lpHshTabSplitNext[iHshTabBaseNelm], de-link them from the old
    //   chain, and link them into a new chain.
    for (i = 0,
           lpHshEntryNext = lpHTS->lpHshTabSplitNext;
         i < iCnt;
         i++)
    {
        // Copy to source entry as lpHshEntrySrc->NextSameHash
        //   may have been clobbered
        lpHshEntrySrc  = lpHshEntryNext;

        // If we were to delink lpHshEntrySrc, the ptr to its NextSameHash
        //   is clobbered, so we save it here
        lpHshEntryNext = lpHshEntrySrc->NextSameHash;

        if (lpHshEntrySrc->htFlags.Inuse                    // If this entry is in use, and
         && ((lpHshEntrySrc->uHash & uHashMaskNext) * lpHTS->iHshTabEPB)
          NE lpHshEntrySrc->uHashAndMask)                   //   it hashes to a higher bucket, ...
        {
            HTFLAGS htFlags;
////////////LPHSHENTRY lp;
#ifdef DEBUG
////        static LPHSHENTRY lp = (LPHSHENTRY) 0xF70090;
#endif
////////////Assert (HshTabFrisk (lpHTS));

            // Save as new uHashAndMask value
            lpHshEntrySrc->uHashAndMask = (lpHshEntrySrc->uHash & uHashMaskNext) * lpHTS->iHshTabEPB;

            // Don't split
            lpHshEntryDest = FindNextFreeUsingHTE_EM (lpHshEntryHash, uHash, FALSE, lpHTS);
#ifdef DEBUG
            if (i EQ 0)
                Assert (lpHshEntryDest EQ lpHshEntryHash);  // The 1st time only
////        if (lp EQ lpHshEntrySrc)
////        {
////            WCHAR wszTemp[1024];    // Ptr to temporary output area
////
////            wsprintfW (wszTemp,
////                       L"HshTabSplitNextEntry:  Entry %p <-- %p (%04X) (%04X)",
////                       lpHshEntryDest,
////                       lpHshEntrySrc,
////                       lpHshEntrySrc->uHashAndMask,
////                       uHashMask);
////            MBC (wszTemp);
////            lp = lpHshEntryDest;
////        } // End IF
#endif
            // Copy the entire HSHENTRY to its new location
            //   preserving the destination's .PrinHash flag
            htFlags = lpHshEntrySrc->htFlags;
            htFlags.PrinHash = lpHshEntryDest->htFlags.PrinHash;
            CopyMemory (lpHshEntryDest, lpHshEntrySrc, sizeof (HSHENTRY));
            lpHshEntryDest->htFlags = htFlags;

            // Repoint the matching symbol table entry
            //   to the new hash table entry
            if (!lpHshEntrySrc->htFlags.CharIsValid)
            {
                LPSYMENTRY lpSymEntrySrc;

                // Get a ptr to the SYMENTRY we're changing
                lpSymEntrySrc = lpHshEntrySrc->htSymEntry;

                // Update the main entry
                lpSymEntrySrc->stHshEntry = lpHshEntryDest;

                // Loop through all other SYMENTRYs in the same shadow chain
                while (lpSymEntrySrc->stPrvEntry)
                {
                    lpSymEntrySrc = lpSymEntrySrc->stPrvEntry;
                    lpSymEntrySrc->stHshEntry = lpHshEntryDest;
                } // End WHILE
            } // End IF

            // Clear the old entry, preserving the .PrinHash flag
            //   and the previous and next hash pointers
            htFlags.PrinHash = lpHshEntrySrc->htFlags.PrinHash;
            ZeroMemory (lpHshEntrySrc, sizeof (HSHENTRY));
            lpHshEntrySrc->htFlags.PrinHash = htFlags.PrinHash;
            lpHshEntrySrc->NextSameHash = lpHshEntryDest->NextSameHash;
            Assert (lpHshEntryDest->PrevSameHash EQ LPHSHENTRY_NONE
                 || !lpHshEntrySrc->htFlags.PrinHash);
            lpHshEntrySrc->PrevSameHash = lpHshEntryDest->PrevSameHash;//**

            // Set to NONE so we can pass error checking in HshTabLink
            //   unless this is the first entry in the sequence
            lpHshEntryDest->NextSameHash =
            lpHshEntryDest->PrevSameHash = LPHSHENTRY_NONE;

            // Delink the old entry from the old chain
            HshTabDelink (lpHshEntrySrc);

            // Link the destination entry into the hash entry
            HshTabLink (lpHshEntryHash, lpHshEntryDest);

////////////// Remask entries below this point to update their uHashAndMask
////////////for (lp = lpHshTab;
////////////     lp NE &lpHshTab[iHshTabTotalNelm];
////////////     lp++)
////////////if (lp->htFlags.Inuse)
////////////if (lp->uHashAndMask NE MaskTheHash (lp->uHash, lpHTS))
////////////{
////////////    DbgBrk ();
////////////    lp->uHashAndMask = MaskTheHash (lp->uHash, lpHTS);
////////////} // End IF
////////////
////////////Assert (HshTabFrisk (lpHTS));
#ifdef DEBUG
////////////// Count in another entry moved
////////////iCntMoved++;
#endif
        } // End IF
    } // End FOR

    // We have split an entry, so point to the next block to split
    lpHTS->lpHshTabSplitNext += lpHTS->iHshTabEPB;

    // If we're at the end of the table base,
    //   reset the appropriate variables
    if ((lpHTS->lpHshTabSplitNext - lpHTS->lpHshTab) EQ lpHTS->iHshTabBaseNelm)
    {
        lpHTS->lpHshTabSplitNext = lpHTS->lpHshTab;
        lpHTS->iHshTabBaseNelm <<= 1;
        Assert (lpHTS->iHshTabBaseNelm <= lpHTS->iHshTabTotalNelm);
#ifdef DEBUG
        if (TlsGetValue (dwTlsPerTabData))
            dprintfW (L"||| Shifting uHashMask from %04X to %04X (%S#%d)",
                      lpHTS->uHashMask,
                      uHashMaskNext,
                      FNLN);
#endif
        lpHTS->uHashMask = uHashMaskNext;
        Assert (lpHTS->uHashMask < (UINT) (2 * lpHTS->iHshTabBaseNelm / lpHTS->iHshTabEPB));
    } // End IF

#ifdef DEBUG
////{
////    WCHAR wszTemp[1024];    // Ptr to temporary output area
////
////    wsprintfW (wszTemp,
////               L"@@@@ %d hash table entr%s moved by HshTabSplitNextEntry",
////               iCntMoved,
////               (iCntMoved EQ 1) ? L"y" : L"ies");
////    DbgMsgW (wszTemp);
////}
#endif

    Assert (HshTabFrisk (lpHTS));
ERROR_EXIT:
    return bRet;
} // End HshTabSplitNextEntry_EM


//***************************************************************************
//  $FindNextFreeUsingHash_SPLIT_EM
//
//  Find the next free hash table entry after a give one.
//  The search is first within the same block from low to high,
//    then within each following block from high to low within
//    the block, except for the principal hash (first) entry in
//    the block.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FindNextFreeUsingHash_SPLIT_EM"
#else
#define APPEND_NAME
#endif

LPHSHENTRY FindNextFreeUsingHash_SPLIT_EM
    (UINT        uHash,                     // The hash value to find
     UBOOL       bSplitNext,                // TRUE iff we should split if not found
     LPHSHTABSTR lpHTS)                     // Ptr to HshTab struc

{
    LPHSHENTRY   lpHshEntry,                // Ptr to temporary HSHENTRY
                 lpHshEntryHash;            // ...
    int          i;                         // Loop counter
    UBOOL        bFirstTime = TRUE;         // TRUE iff this run through if the first time

    DBGENTER;

    while (TRUE)
    {
        lpHshEntryHash = &lpHTS->lpHshTab[MaskTheHash (uHash, lpHTS)];

        // Loop through the current block looking for
        //   a free (not inuse) entry
        // Note that because this is the same block as uHash,
        //   this search allows PrinHash.
        for (lpHshEntry = lpHshEntryHash, i = 0;
             lpHshEntry->htFlags.Inuse
          && i < lpHTS->iHshTabEPB;
             lpHshEntry++, i++)
        {}

        // If we found it, return it
        if (i < lpHTS->iHshTabEPB)
            goto NORMAL_EXIT;
        else
        if (bSplitNext)
        {
            UBOOL bTemp;

            // Split the SplitNext entry
            bTemp = HshTabSplitNextEntry_EM (uHash, lpHTS);
            Assert (bTemp);

            // Because we split an entry (and possibly resized the table)
            //   we need to respecify some variables.
            lpHshEntryHash = &lpHTS->lpHshTab[MaskTheHash (uHash, lpHTS)];

            // Loop through the current block looking for
            //   a free (not inuse) entry
            // Note that because this is the same block as uHash,
            //   this search allows PrinHash.
            for (lpHshEntry = lpHshEntryHash, i = 0;
                 lpHshEntry->htFlags.Inuse
              && i < lpHTS->iHshTabEPB;
                 lpHshEntry++, i++)
            {}

            // If we found it, return it
            if (i < lpHTS->iHshTabEPB)
                goto NORMAL_EXIT;
        } // End IF/ELSE/IF

        // As we didn't find a free entry, continue searching
        //   wrapping around to the start if necessary.
        lpHshEntry = lpHshEntryHash;
        do {
            // Increment to the next entry (relatively prime to total size)
            lpHshEntry += lpHTS->iHshTabIncrFree;

            // If we're at or beyond the end, wrap to the start
            if (lpHshEntry >= &lpHTS->lpHshTab[lpHTS->iHshTabTotalNelm])
                lpHshEntry -= lpHTS->iHshTabTotalNelm;

            // Check for not Inuse and not PrinHash
            if (!(lpHshEntry->htFlags.Inuse || lpHshEntry->htFlags.PrinHash))
                break;
        } while (lpHshEntry NE lpHshEntryHash);

        // If we found it, return it
        if (lpHshEntry NE lpHshEntryHash)
            goto NORMAL_EXIT;

        // As we didn't find a free entry, try to expand the hash table
        if (!HshTabResize_EM (lpHTS))
            goto ERROR_EXIT;

        // Go around again unless we've done this before
        if (bFirstTime)
            bFirstTime = FALSE;
        else
            break;
    } // End WHILE

    ErrorMessageIndirect (ERRMSG_HASH_TABLE_FULL APPEND_NAME);
ERROR_EXIT:
    lpHshEntry = NULL;
NORMAL_EXIT:
    DBGLEAVE;

    return lpHshEntry;
} // End FindNextFreeUsingHash_SPLIT_EM
#undef  APPEND_NAME


//***************************************************************************
//  $FindNextFreeUsingHTE_EM
//
//  Find the next free hash table entry after a give one.
//  The search is first within the same block from low to high,
//    then within each following block from high to low within
//    the block, except for the principal hash (first) entry in
//    the block.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FindNextFreeUsingHTE_EM"
#else
#define APPEND_NAME
#endif

LPHSHENTRY FindNextFreeUsingHTE_EM
    (LPHSHENTRY  lpHshEntryHash,            // Ptr to HshTabEntry
     UINT        uHash,                     // The has value to find
     UBOOL       bSplitNext,                // TRUE iff we should split if not found
     LPHSHTABSTR lpHTS)                     // Ptr to HshTab struc

{
    LPHSHENTRY   lpHshEntry;                // Ptr to temporary HSHENTRY
    int          i;                         // Loop counter
    UBOOL        bFirstTime = TRUE;         // TRUE iff this run through if the first time

    DBGENTER;

    while (TRUE)
    {
        // Loop through the current block looking for
        //   a free (not inuse) entry
        // Note that because this is the same block as uHash,
        //   this search allows PrinHash.
        for (lpHshEntry = lpHshEntryHash, i = 0;
             lpHshEntry->htFlags.Inuse
          && i < lpHTS->iHshTabEPB;
             lpHshEntry++, i++)
        {};

        // If we found it, return it
        if (i < lpHTS->iHshTabEPB)
            goto NORMAL_EXIT;
        else
        if (bSplitNext)
        {
            UBOOL bTemp;

            // Split the SplitNext entry
            bTemp = HshTabSplitNextEntry_EM (uHash, lpHTS);
            Assert (bTemp);

            // Because we split an entry (and possibly resized the table)
            //   we need to respecify some variables.
            lpHshEntryHash = &lpHTS->lpHshTab[MaskTheHash (uHash, lpHTS)];

            // Loop through the current block looking for
            //   a free (not inuse) entry
            // Note that because this is the same block as uHash,
            //   this search allows PrinHash.
            for (lpHshEntry = lpHshEntryHash, i = 0;
                 lpHshEntry->htFlags.Inuse
              && i < lpHTS->iHshTabEPB;
                 lpHshEntry++, i++)
            {}

            // If we found it, return it
            if (i < lpHTS->iHshTabEPB)
                goto NORMAL_EXIT;
        } // End IF/ELSE/IF

        // As we didn't find a free entry, continue searching
        //   wrapping around to the start if necessary.
        lpHshEntry = lpHshEntryHash;
        do {
            // Increment to the next entry (relatively prime to total size)
            lpHshEntry += lpHTS->iHshTabIncrFree;

            // If we're at or beyond the end, wrap to the start
            if (lpHshEntry >= &lpHTS->lpHshTab[lpHTS->iHshTabTotalNelm])
                lpHshEntry -= lpHTS->iHshTabTotalNelm;

            // Check for not Inuse and not PrinHash
            if (!(lpHshEntry->htFlags.Inuse || lpHshEntry->htFlags.PrinHash))
                break;
        } while (lpHshEntry NE lpHshEntryHash);

        // If we found it, return it
        if (lpHshEntry NE lpHshEntryHash)
            goto NORMAL_EXIT;

        // As we didn't find a free entry, try to expand the hash table
        if (!HshTabResize_EM (lpHTS))
            goto ERROR_EXIT;

        // Go around again unless we've done this before
        if (bFirstTime)
            bFirstTime = FALSE;
        else
            break;
    } // End WHILE

    ErrorMessageIndirect (ERRMSG_HASH_TABLE_FULL APPEND_NAME);
ERROR_EXIT:
    lpHshEntry = NULL;
NORMAL_EXIT:
    DBGLEAVE;

    return lpHshEntry;
} // End FindNextFreeUsingHTE_EM
#undef  APPEND_NAME


//***************************************************************************
//  $HshTabLookupCharHash
//
//  Lookup a char in the global hash table based upon the char & hash
//***************************************************************************

LPHSHENTRY HshTabLookupCharHash
    (APLCHAR aplChar,                   // The char to lookup
     UINT    uHash)                     // The hash of the char

{
    LPHSHENTRY   lpHshEntry;            // Ptr to the result
    HTFLAGS      htMaskFlags = {0};     // Hash table mask flags
    LPHSHTABSTR  lpHTS;                 // Ptr to HshTab struc

    // Get a ptr to the global HshTab struc
    lpHTS = &htsGLB;

    // Set mask flags
    htMaskFlags.Inuse       =
    htMaskFlags.CharIsValid = TRUE;

    // Loop through the hash table entries
    for (lpHshEntry = &lpHTS->lpHshTab[MaskTheHash (uHash, lpHTS)];
         lpHshEntry NE LPHSHENTRY_NONE;
         lpHshEntry = lpHshEntry->NextSameHash)
#ifdef DEBUG
    if (!lpHshEntry)
    {
        DisplayHshTab (lpHTS);
        DbgStop ();             // We should never get here
    } else
#endif
    {
        // Check the flags
        if (((*(UINT *) &lpHshEntry->htFlags) & *(UINT *) &htMaskFlags)
             EQ *(UINT *) &htMaskFlags
         && aplChar EQ lpHshEntry->htFlags.htChar)
            break;
    } // End FOR/IF/ELSE

    return lpHshEntry;
} // End HshTabLookupCharHash


//***************************************************************************
//  $HshTabLookupNameHash
//
//  Lookup a char in the global has table based upon the name & hash
//***************************************************************************

LPHSHENTRY HshTabLookupNameHash
    (LPWCHAR  lpwCharName,              // Ptr to the name to lookup
     UINT     uLen,                     // Length of lpwCharName
     UINT     uHash)                    // The hash of the char

{
    LPHSHENTRY   lpHshEntry;            // Ptr to the result
    HTFLAGS      htMaskFlags = {0};     // Hash table mask flags
    LPHSHTABSTR  lpHTS;                 // Ptr to HshTab struc

    // Get a ptr to the global HshTab struc
    lpHTS = &htsGLB;

    // Set mask flags
    htMaskFlags.Inuse       =
    htMaskFlags.CharIsValid = TRUE;

    // Loop through the hash table entries
    for (lpHshEntry = &lpHTS->lpHshTab[MaskTheHash (uHash, lpHTS)];
         lpHshEntry NE LPHSHENTRY_NONE;
         lpHshEntry = lpHshEntry->NextSameHash)
#ifdef DEBUG
    if (!lpHshEntry)
    {
        DisplayHshTab (lpHTS);
        DbgStop ();             // We should never get here
    } else
#endif
    {
        // Check the flags
        if (((*(UINT *) &lpHshEntry->htFlags) & *(UINT *) &htMaskFlags)
             EQ *(UINT *) &htMaskFlags
         && strncmpW (lpHshEntry->lpwCharName, lpwCharName, uLen) EQ 0)
            break;
    } // End FOR/IF/ELSE

    return lpHshEntry;
} // End HshTabLookupNameHash


//***************************************************************************
//  $AppendSymbolValue
//
//  Append the value of a {symbol} to the global hash table
//***************************************************************************

UBOOL AppendSymbolValue
    (APLCHAR aplChar,                   // The corresponding char
     LPWCHAR lpwCharName)               // Ptr to symbol name

{
    LPHSHENTRY  lpHshEntryDest,         // Ptr to the result
                lpHshEntryHash;         // Ptr to principal hash entry
    UINT        uHash;                  // The hash of the char
    UBOOL       bRet = FALSE;           // TRUE iff result is valid
    LPHSHTABSTR lpHTS;                  // Ptr to HshTab struc

    // Get a ptr to the global HshTab struc
    lpHTS = &htsGLB;

    // Hash the char
    uHash = hashlittle
           ((const uint32_t *) &aplChar,    // A ptr to the char to hash
             sizeof (aplChar),              // The # bytes pointed to
             0);                            // Initial value or previous hash
    // Look up the char
    lpHshEntryDest = HshTabLookupCharHash (aplChar, uHash);
    if (lpHshEntryDest EQ LPHSHENTRY_NONE)
    {
        // This char isn't in the HT -- find the next free entry
        //   in the hash table, split if necessary
        lpHshEntryDest = FindNextFreeUsingHash_SPLIT_EM (uHash, TRUE, lpHTS);

        // If it's invalid, quit
        if (!lpHshEntryDest)
            goto ERROR_EXIT;

        // We *MUST NOT* call MaskTheHash until after the call
        //   to FindNextFreeUsingHash_SPLIT_EM as that call might well
        //   split a HTE and change lpHshTabSplitNext and uHashMask,
        //   and thus the result of MaskTheHash.
        lpHshEntryHash = &lpHTS->lpHshTab[MaskTheHash (uHash, lpHTS)];

        // This entry must be a principal one
        Assert (lpHshEntryHash->htFlags.PrinHash);

        // Mark this hash table entry as in use
        lpHshEntryDest->htFlags.Inuse       =
        lpHshEntryDest->htFlags.CharIsValid = TRUE;
        lpHshEntryDest->htFlags.htChar      = aplChar;

        // Save the ptr to the name
        lpHshEntryDest->lpwCharName  = lpwCharName;

        // Save hash value (so we don't have to rehash on split)
        lpHshEntryDest->uHash        = uHash;
        lpHshEntryDest->uHashAndMask = MaskTheHash (uHash, lpHTS);

        // Link the destination entry into the hash entry
        HshTabLink (lpHshEntryHash, lpHshEntryDest);

        // Mark as successful
        bRet = TRUE;
    } else
        DbgStop ();     // We should never get here
ERROR_EXIT:
    Assert (HshTabFrisk (lpHTS));

    return bRet;
} // End AppendSymbolValue


//***************************************************************************
//  $AppendSymbolName
//
//  Append a {symbol} name to the hash table
//***************************************************************************

UBOOL AppendSymbolName
    (LPWCHAR lpwCharName,               // Ptr to symbol name
     APLCHAR aplChar)                   // The corresponding char

{
    LPHSHENTRY  lpHshEntryDest,         // Ptr to the result
                lpHshEntryHash;         // Ptr to principal hash entry
    UINT        uHash;                  // The hash of the name
    UBOOL       bRet = FALSE;           // TRUE iff result is valid
    LPHSHTABSTR lpHTS;                  // Ptr to HshTab struc

    // Get a ptr to the global HshTab struc
    lpHTS = &htsGLB;

    // Hash the name
    uHash = hashlittle
           ((const uint32_t *) lpwCharName, // A ptr to the name to hash
             lstrlenW (lpwCharName) * sizeof (lpwCharName[0]), // The # bytes pointed to
             0);                            // Initial value or previous hash
    // Look up the name
    lpHshEntryDest = HshTabLookupNameHash (lpwCharName, lstrlenW (lpwCharName), uHash);
    if (lpHshEntryDest EQ LPHSHENTRY_NONE)
    {
        // This name isn't in the HT -- find the next free entry
        //   in the hash table, split if necessary
        lpHshEntryDest = FindNextFreeUsingHash_SPLIT_EM (uHash, TRUE, lpHTS);

        // If it's invalid, quit
        if (!lpHshEntryDest)
            goto ERROR_EXIT;

        // We *MUST NOT* call MaskTheHash until after the call
        //   to FindNextFreeUsingHash_SPLIT_EM as that call might well
        //   split a HTE and change lpHshTabSplitNext and uHashMask,
        //   and thus the result of MaskTheHash.
        lpHshEntryHash = &lpHTS->lpHshTab[MaskTheHash (uHash, lpHTS)];

        // This entry must be a principal one
        Assert (lpHshEntryHash->htFlags.PrinHash);

        // Mark this hash table entry as in use
        lpHshEntryDest->htFlags.Inuse       =
        lpHshEntryDest->htFlags.CharIsValid = TRUE;
        lpHshEntryDest->htFlags.htChar      = aplChar;

        // Save the ptr to the name
        lpHshEntryDest->lpwCharName  = lpwCharName;

        // Save hash value (so we don't have to rehash on split)
        lpHshEntryDest->uHash        = uHash;
        lpHshEntryDest->uHashAndMask = MaskTheHash (uHash, lpHTS);

        // Link the destination entry into the hash entry
        HshTabLink (lpHshEntryHash, lpHshEntryDest);

        // Mark as successful
        bRet = TRUE;
    } else
        DbgStop ();     // We should never get here
ERROR_EXIT:
    Assert (HshTabFrisk (lpHTS));

    return bRet;
} // End AppendSymbolName


//***************************************************************************
//  $InitSymbolNamesValues
//
//  Append all {symbol} names and values to the hash table
//***************************************************************************

UBOOL InitSymbolNamesValues
    (void)

{
    UINT uSym;          // Loop counter

    // Loop through all symbol names/values
    for (uSym = 0; uSym < aSymbolNames_NRows; uSym++)
    {
        // Because we may use multiple names for each char,
        //   append the first one only
        if ((uSym EQ 0) || aSymbolNames[uSym].Symbol NE aSymbolNames[uSym - 1].Symbol)
        if (!AppendSymbolValue (aSymbolNames[uSym].Symbol,
                                aSymbolNames[uSym].lpwName))
            return FALSE;

        if (!AppendSymbolName  (aSymbolNames[uSym].lpwName,
                                aSymbolNames[uSym].Symbol))
            return FALSE;
    } // End FOR

    return TRUE;
} // End InitSymbolNamesValues


//***************************************************************************
//  $CharToSymbolName
//
//  Translate a char to its canonical {name}
//***************************************************************************

LPWCHAR CharToSymbolName
    (APLCHAR aplChar)

{
    UINT       uHash;               // The hash of the char
    LPHSHENTRY lpHshEntryDest;      // Ptr to the matching hash table entry

    // Hash the char
    uHash = hashlittle
           ((const uint32_t *) &aplChar,    // A ptr to the char to hash
             sizeof (aplChar),              // The # bytes pointed to
             0);                            // Initial value or previous hash
    // Look up the char
    lpHshEntryDest = HshTabLookupCharHash (aplChar, uHash);
    if (lpHshEntryDest NE LPHSHENTRY_NONE)
        return lpHshEntryDest->lpwCharName;
    else
        return NULL;
} // End CharToSymbolName


//***************************************************************************
//  $SymbolNameToChar
//
//  Translate a {symbol} name to its char allowing for {\xXXXX} and {\uXXXXXX}
//***************************************************************************

WCHAR SymbolNameToChar
    (LPWCHAR lpwCharName)

{
    UINT       uHash;               // The hash of the char
    LPHSHENTRY lpHshEntryDest;      // Ptr to the matching hash table entry
    WCHAR      wcRes;               // Char result
    LPWCHAR    lpwCharEnd;          // Temporary ptr
    UINT       uChr;                // Loop counter
    // Powers of 16 for converting {\xXXX} or {\uXXXXXX} to a number
    static UINT Pow16[] = {16 * 16 * 16 * 16 * 16,
                           16 * 16 * 16 * 16,
                           16 * 16 * 16,
                           16 * 16,
                           16,
                           1};

    Assert (lpwCharName[0] EQ L'{');

    // Check for hex-notation as in {\xXXXX} or {\uXXXXXX}
    if (lpwCharName[1] EQ L'\\'
     && (lpwCharName[2] EQ L'x'
      || lpwCharName[2] EQ L'u'))
    {
        UINT uLen;

        // Get the # hex digits
        uLen = (lpwCharName[2] EQ L'x') ? 4 : 6;

        // Loop through the uLen hex digits, converting them to a number
        for (uChr = wcRes = 0; uChr < uLen; uChr++)
        {
            WCHAR wChr = lpwCharName[3 + uChr];

            if (L'0' <= wChr
             &&         wChr <= L'9')
                wcRes += (     wChr - L'0') * Pow16[uChr + (6 - uLen)];
            else
            if (L'A' <= wChr
             &&         wChr <= L'F')
                wcRes += (10 + wChr - L'A') * Pow16[uChr + (6 - uLen)];
            else
            if (L'a' <= wChr
             &&         wChr <= L'f')
                wcRes += (10 + wChr - L'a') * Pow16[uChr + (6 - uLen)];
            else
                // We should never get here
                DbgStop ();
        } // End FOR
    } else
    {
        // Find the matching L'}'
        lpwCharEnd = SkipPastCharW (lpwCharName, L'}');

        Assert (lpwCharEnd NE NULL);

        // Hash the name
        uHash = hashlittle
               ((const uint32_t *) lpwCharName, // A ptr to the name to hash
                 (lpwCharEnd - lpwCharName) * sizeof (lpwCharName[0]),  // The # bytes pointed to
                 0);                            // Initial value or previous hash
        // Look up the char
        lpHshEntryDest = HshTabLookupNameHash (lpwCharName, (UINT) (lpwCharEnd - lpwCharName), uHash);
        if (lpHshEntryDest NE LPHSHENTRY_NONE)
            wcRes =lpHshEntryDest->htFlags.htChar;
        else
            wcRes =L'\0';
    } // End IF/ELSE

    return wcRes;
} // End SymbolNameToChar


//***************************************************************************
//  $SymTabLookupChar
//
//  Lookup a char based upon hash, flags, and value
//***************************************************************************

LPSYMENTRY SymTabLookupChar
    (UINT      uHash,
     APLCHAR   aplChar,
     LPSTFLAGS lpstFlags)

{
    LPHSHENTRY   lpHshEntry;
    STFLAGS      stMaskFlags = {0};
    UINT         uHashMasked;
    LPSYMENTRY   lpSymEntry = NULL;
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPHSHTABSTR  lpHTS;         // Ptr to HshTab struc

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get a ptr to the HshTab struc
    lpHTS = &lpMemPTD->htsPTD;

    // Set mask flags
    stMaskFlags.Imm     =
    stMaskFlags.Perm    =
    stMaskFlags.Value   =
    stMaskFlags.Inuse   = TRUE;
    stMaskFlags.ImmType = NEG1U;

    // Save common value
    uHashMasked = MaskTheHash (uHash, lpHTS);

    while (TRUE)
    {
        for (lpHshEntry = &lpHTS->lpHshTab[uHashMasked];
             lpHshEntry NE LPHSHENTRY_NONE;
             lpHshEntry = lpHshEntry->NextSameHash)
#ifdef DEBUG
        if (!lpHshEntry)
        {
            DisplayHshTab (lpHTS);
            DbgStop ();             // We should never get here
        } else
#endif
        {
            // Check the flags
            if (lpHshEntry->htFlags.Inuse
             && !lpHshEntry->htFlags.CharIsValid
             && (((*(UINT *) &lpHshEntry->htSymEntry->stFlags) & *(UINT *) &stMaskFlags)
              EQ ((*(UINT *) lpstFlags)                        & *(UINT *) &stMaskFlags))
             && aplChar EQ lpHshEntry->htSymEntry->stData.stChar)
            {
                lpSymEntry = lpHshEntry->htSymEntry;

                break;
            } // End IF
        } // End FOR/IF/ELSE

        if (lpSymEntry NE NULL
         || lpHTS->lpHshTabPrv EQ NULL)
            break;

        // Search through the previous HshTab
        lpHTS = lpHTS->lpHshTabPrv;
    } // End WHILE

    return lpSymEntry;
} // End SymTabLookupChar


//***************************************************************************
//  $SymTabLookupNumber
//
//  Lookup a number based upon hash, flags, and value
//***************************************************************************

LPSYMENTRY SymTabLookupNumber
    (UINT      uHash,           // The hashed value
     APLINT    aplInteger,      // The integer/Boolean to lookup
     LPSTFLAGS lpstNeedFlags)   // The flags we require

{
    LPHSHENTRY   lpHshEntry;
    STFLAGS      stMaskFlags = {0};
    UINT         uHashMasked;
    LPSYMENTRY   lpSymEntry = NULL;
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPHSHTABSTR  lpHTS;         // Ptr to HshTab struc

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get a ptr to the HshTab struc
    lpHTS = &lpMemPTD->htsPTD;

    // Set mask flags
    stMaskFlags.Imm   =
    stMaskFlags.Perm    =
    stMaskFlags.Value   =
    stMaskFlags.Inuse   = TRUE;
    stMaskFlags.ImmType = NEG1U;

    // Save common value
    uHashMasked = MaskTheHash (uHash, lpHTS);

#ifdef DEBUG
////DisplayHshTab (lpHTS);
#endif
    while (TRUE)
    {
        for (lpHshEntry = &lpHTS->lpHshTab[uHashMasked];
             lpHshEntry NE LPHSHENTRY_NONE;
             lpHshEntry = lpHshEntry->NextSameHash)
#ifdef DEBUG
        if (!lpHshEntry)
        {
            DisplayHshTab (lpHTS);
            DbgStop ();             // We should never get here
        } else
#endif
        {
            // Check the flags
            if (lpHshEntry->htFlags.Inuse
             && !lpHshEntry->htFlags.CharIsValid
             && (((*(UINT *) &lpHshEntry->htSymEntry->stFlags) & *(UINT *) &stMaskFlags)
              EQ ((*(UINT *) lpstNeedFlags)                    & *(UINT *) &stMaskFlags))
             && aplInteger EQ lpHshEntry->htSymEntry->stData.stInteger)
            {
                lpSymEntry = lpHshEntry->htSymEntry;

                break;
            } // End IF
        } // End FOR/IF/ELSE

        if (lpSymEntry NE NULL
         || lpHTS->lpHshTabPrv EQ NULL)
            break;

        // Search through the previous HshTab
        lpHTS = lpHTS->lpHshTabPrv;
    } // End WHILE

    return lpSymEntry;
} // End SymTabLookupNumber


//***************************************************************************
//  $SymTabLookupFloat
//
//  Lookup a Floating Point number based upon hash, flags, and value
//***************************************************************************

LPSYMENTRY SymTabLookupFloat
    (UINT      uHash,
     APLFLOAT  fFloat,
     LPSTFLAGS lpstFlags)

{
    LPHSHENTRY   lpHshEntry;
    STFLAGS      stMaskFlags = {0};
    UINT         uHashMasked;
    LPSYMENTRY   lpSymEntry = NULL;
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPHSHTABSTR  lpHTS;         // Ptr to HshTab struc

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get a ptr to the HshTab struc
    lpHTS = &lpMemPTD->htsPTD;

    // Set mask flags
    stMaskFlags.Imm   =
    stMaskFlags.Perm    =
    stMaskFlags.Value   =
    stMaskFlags.Inuse   = TRUE;
    stMaskFlags.ImmType = NEG1U;

    // Save common value
    uHashMasked = MaskTheHash (uHash, lpHTS);

    while (TRUE)
    {
        for (lpHshEntry = &lpHTS->lpHshTab[uHashMasked];
             lpHshEntry NE LPHSHENTRY_NONE;
             lpHshEntry = lpHshEntry->NextSameHash)
#ifdef DEBUG
        if (!lpHshEntry)
        {
            DisplayHshTab (lpHTS);
            DbgStop ();             // We should never get here
        } else
#endif
        {
            // Check the flags
            if (lpHshEntry->htFlags.Inuse
             && !lpHshEntry->htFlags.CharIsValid
             && (((*(UINT *) &lpHshEntry->htSymEntry->stFlags) & *(UINT *) &stMaskFlags)
              EQ ((*(UINT *) lpstFlags)                        & *(UINT *) &stMaskFlags))
             && fFloat EQ lpHshEntry->htSymEntry->stData.stFloat)
            {
                lpSymEntry = lpHshEntry->htSymEntry;

                break;
            } // End IF
        } // End FOR/IF/ELSE

        if (lpSymEntry NE NULL
         || lpHTS->lpHshTabPrv EQ NULL)
            break;

        // Search through the previous HshTab
        lpHTS = lpHTS->lpHshTabPrv;
    } // End WHILE

    return lpSymEntry;
} // End SymTabLookupFloat


//***************************************************************************
//  $SymTabLookupName
//
//  Lookup a named entry based upon hash, flags, and value
//***************************************************************************

LPSYMENTRY SymTabLookupName
    (LPWCHAR   lpwszString,                             // Ptr to the name to lookup
     LPSTFLAGS lpstFlags)                               // Ptr to flags filter

{
    return
      SymTabLookupNameLength (lpwszString,              // Ptr to the name to lookup
                              lstrlenW (lpwszString),   // Length of the name
                              lpstFlags);               // Ptr to flags filter
} // End SymTabLookupName


//***************************************************************************
//  $hashlittleConv
//
//  Wrapper around <hashlittle> which converts _alphabet_
//    to lowercase before hashing.
//***************************************************************************

uint32_t hashlittleConv
    (LPWCHAR     lpwName,       // Ptr to name
     size_t      length,        // Length in WCHARs (***NOT BYTES***)
     uint32_t    initval)       // Initial value for hash

{
    LPWCHAR  lpwConv,           // Ptr to global memory for the converted key
             lpwTmp;            // Ptr to output name
    uint32_t uhash;             // The result
    size_t   uCnt;              // Loop counter

    // Allocate virtual memory to hold the converted name
    lpwConv =
      MyVirtualAlloc (NULL,                 // Any address (FIXED SIZE)
                      (APLU3264) length * sizeof (WCHAR),
                      MEM_COMMIT | MEM_TOP_DOWN,
                      PAGE_READWRITE);
    if (lpwConv EQ NULL)        // ***FIXME*** -- Better error handling needed
        return 0;

    // Loop through the name converting _alphabet_ to lowercase
    for (uCnt = 0,
          lpwTmp = lpwConv;
        uCnt < length;
        uCnt++)
    if (UTF16_A_ <= *lpwName
     &&             *lpwName <= UTF16_Z_)
        *lpwTmp++ = L'a' + (*lpwName++ - UTF16_A_);
    else
        *lpwTmp++ = *lpwName++;

    // Hash the converted name
    uhash = hashlittle (lpwConv, length * sizeof (WCHAR), initval);

    // We no longer need this storage
    MyVirtualFree (lpwConv, 0, MEM_RELEASE); lpwConv = NULL;

    return uhash;
} // End hashlittleConv


//***************************************************************************
//  $SymTabLookupNameLength
//
//  Lookup a named entry based upon hash, flags, and value
//***************************************************************************

LPSYMENTRY SymTabLookupNameLength
    (LPWCHAR   lpwszString,         // Ptr to the name to lookup
     int       iLen,                // Length of the name
     LPSTFLAGS lpstFlags)           // Ptr to flags filter

{
    LPHSHENTRY   lpHshEntry;
    STFLAGS      stMaskFlags = {0};
    UINT         uHash;
    LPSYMENTRY   lpSymEntry = NULL;
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    WCHAR        sysName[32];       // Temp storage for sysnames in lowercase
    LPWCHAR      lpwszName;         // Ptr to name
    LPHSHTABSTR  lpHTS;             // Ptr to HshTab struc

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get a ptr to the HshTab struc
    lpHTS = &lpMemPTD->htsPTD;

    // Skip over trailing white space
    while (iLen && lpwszString[iLen - 1] EQ L' ')
        iLen--;

    if (iLen EQ 0)
        goto ERROR_EXIT;

    // If this is a sysname, ...
    if (IsSysName (lpwszString))
    {
        // Copy the sysname to local storage
        CopyMemory (sysName, lpwszString, iLen * sizeof (lpwszString[0]));

        // Convert it to lowercase
        CharLowerBuffW (sysName, iLen);

        lpwszName = sysName;
    } else
        lpwszName = lpwszString;

    // Hash the name
    uHash = hashlittleConv
            (lpwszName,                     // A ptr to the name to hash
             iLen,                          // The # bytes pointed to
             0);                            // Initial value or previous hash
    // If the caller hasn't set this field, set it ourselves
    if (lpstFlags->ObjName EQ OBJNAME_NONE)
    {
        // Set the flags of the entry we're looking for
        if (IsSysName (lpwszName))
            lpstFlags->ObjName = OBJNAME_SYS;
        else
        if (IsMFName (lpwszName))
            lpstFlags->ObjName = OBJNAME_MF;
        else
            lpstFlags->ObjName = OBJNAME_USR;
    } // End IF

    // Mark as in use
    lpstFlags->Inuse   = TRUE;

    // Set mask flags
    stMaskFlags.ObjName = NEG1U;
    stMaskFlags.Inuse   = TRUE;

    while (TRUE)
    {
        for (lpHshEntry = &lpHTS->lpHshTab[MaskTheHash (uHash, lpHTS)];
             lpHshEntry NE LPHSHENTRY_NONE;
             lpHshEntry = lpHshEntry->NextSameHash)
        {
            // Check the flags
            if (lpHshEntry->htFlags.Inuse
             && !lpHshEntry->htFlags.CharIsValid
             && (((*(UINT *) &lpHshEntry->htSymEntry->stFlags) & *(UINT *) &stMaskFlags)
              EQ ((*(UINT *) lpstFlags)                        & *(UINT *) &stMaskFlags)))
            {
                LPWCHAR lpwGlbName;
                int     iCmp, iCnt;
#ifdef DEBUG
////////////////DisplayHshTab (lpHTS);
#endif
                Assert (lpHshEntry->htGlbName NE NULL);

                // Lock the memory to get a ptr to it
                lpwGlbName = MyGlobalLock (lpHshEntry->htGlbName);

#define ToLowerUnd(wch)     ((UTF16_A_ <= wch && wch <= UTF16_Z_) ? L'a' + (wch - UTF16_A_) : wch)

                // Compare sensitive to case
                for (iCnt = iCmp = 0; iCnt < iLen; iCnt++)
                if (ToLowerUnd (lpwGlbName[iCnt]) NE ToLowerUnd (lpwszName[iCnt]))
                {
                    iCmp = 1;

                    break;
                } // End FOR/IF

                // Ensure that the next char in the hash entry is a
                //   valid second char in a name
                if (iCmp EQ 0
                 && IsValid2ndCharInName (lpwGlbName[iLen]))
                    iCmp = 1;

                // We no longer need this ptr
                MyGlobalUnlock (lpHshEntry->htGlbName); lpwGlbName = NULL;

                // If it matched, return it
                if (iCmp EQ 0)
                {
                    lpSymEntry = lpHshEntry->htSymEntry;

                    break;
                } // End IF
            } // End IF
        } // End FOR

        if (lpSymEntry NE NULL
         || lpHTS->lpHshTabPrv EQ NULL)
            break;

        // Search through the previous HshTab
        lpHTS = lpHTS->lpHshTabPrv;
    } // End WHILE
ERROR_EXIT:
    return lpSymEntry;
} // End SymTabLookupNameLength


//// //***************************************************************************
//// //  $MakePermSymEntry_EM
//// //
//// //  Make a permanent SYMENTRY with a given type and value
//// //***************************************************************************
////
//// LPSYMENTRY MakePermSymEntry_EM
////     (IMM_TYPES    immType,          // ImmType to use (see IMM_TYPES)
////      LPAPLLONGEST lpVal,            // Value to use
////      LPTOKEN      lptkFunc)         // Ptr to token to use in case of error
////
//// {
////     LPSYMENTRY lpSymDst;
////
////     // Split cases based upon the immediate type
////     switch (immType)
////     {
////         case IMMTYPE_BOOL:
////             lpSymDst = SymTabAppendPermInteger_EM (*(LPAPLBOOL)  lpVal);
////
////             break;
////
////         case IMMTYPE_INT:
////             lpSymDst = SymTabAppendPermInteger_EM (*(LPAPLINT)   lpVal);
////
////             break;
////
////         case IMMTYPE_CHAR:
////             lpSymDst = SymTabAppendPermChar_EM    (*(LPAPLCHAR)  lpVal);
////
////             break;
////
////         case IMMTYPE_FLOAT:
////             lpSymDst = SymTabAppendPermFloat_EM   (*(LPAPLFLOAT) lpVal);
////
////             break;
////
////         defstop
////             lpSymDst = NULL;
////
////             break;
////     } // End SWITCH
////
////     // If it failed, set the error token
////     if (!lpSymDst)
////         ErrorMessageSetToken (lptkFunc);
////
////     return lpSymDst;
//// } // End MakePermSymEntry_EM


//***************************************************************************
//  $MakeSymEntry_EM
//
//  Make a SYMENTRY with a given type and value
//***************************************************************************

LPSYMENTRY MakeSymEntry_EM
    (IMM_TYPES    immType,          // ImmType to use (see IMM_TYPES)
     LPAPLLONGEST lpVal,            // Ptr to value to use
     LPTOKEN      lptkFunc)         // Ptr to token to use in case of error

{
    LPSYMENTRY lpSymDst;        // Ptr to dest SYMENTRY

    // Split cases based upon the immediate type
    switch (immType)
    {
        case IMMTYPE_BOOL:
            lpSymDst = SymTabAppendInteger_EM (*(LPAPLBOOL)  lpVal);

            break;

        case IMMTYPE_INT:
            lpSymDst = SymTabAppendInteger_EM (*(LPAPLINT)   lpVal);

            break;

        case IMMTYPE_CHAR:
            lpSymDst = SymTabAppendChar_EM    (*(LPAPLCHAR)  lpVal);

            break;

        case IMMTYPE_FLOAT:
            lpSymDst = SymTabAppendFloat_EM   (*(LPAPLFLOAT) lpVal);

            break;

        defstop
            lpSymDst = NULL;

            break;
    } // End SWITCH

    // If it failed, set the error token
    if (!lpSymDst)
        ErrorMessageSetToken (lptkFunc);

    return lpSymDst;
} // End MakeSymEntry_EM


//***************************************************************************
//  $CopyImmSymEntry_EM
//
//  Copy an immediate symbol table entry
//***************************************************************************

LPSYMENTRY CopyImmSymEntry_EM
    (LPSYMENTRY lpSymSrc,   // LPSYMENTRY to use
     IMM_TYPES  immType,    // ImmType to use (unless -1) (see IMM_TYPES)
     LPTOKEN    lpToken)    // Ptr to token to use in case of error

{
    LPSYMENTRY lpSymDst;

    // stData is an immediate
    Assert (lpSymSrc->stFlags.Imm);

    // If unspecified, use the one in the SYMENTRY
    if (immType EQ -1)
        immType = lpSymSrc->stFlags.ImmType;

    // Split cases based upon the immediate data type
    switch (immType)
    {
        case IMMTYPE_BOOL:
            lpSymDst = SymTabAppendInteger_EM (lpSymSrc->stData.stBoolean);

            break;

        case IMMTYPE_INT:
            lpSymDst = SymTabAppendInteger_EM (lpSymSrc->stData.stInteger);

            break;

        case IMMTYPE_CHAR:
            lpSymDst = SymTabAppendChar_EM    (lpSymSrc->stData.stChar);

            break;

        case IMMTYPE_FLOAT:
            lpSymDst = SymTabAppendFloat_EM   (lpSymSrc->stData.stFloat);

            break;

        defstop
            return NULL;
    } // End SWITCH

    // If it failed, set the error token
    if (!lpSymDst)
        ErrorMessageSetToken (lpToken);

    return lpSymDst;
} // End CopyImmSymEntry_EM


//***************************************************************************
//  $SymTabAppendInteger_EM
//
//  Append a Boolean or long long integer to the symbol table
//***************************************************************************

LPSYMENTRY SymTabAppendInteger_EM
    (APLINT aplInteger)             // The integer to append

{
    return SymTabAppendIntegerCommon_EM (aplInteger, FALSE);
} // End SymTabAppendInteger_EM


//***************************************************************************
//  $SymTabAppendPermInteger_EM
//
//  Append a permanent Boolean or long long integer to the symbol table
//***************************************************************************

LPSYMENTRY SymTabAppendPermInteger_EM
    (APLINT aplInteger)             // The integer to append

{
    return SymTabAppendIntegerCommon_EM (aplInteger, TRUE);
} // End SymTabAppendPermInteger_EM


//***************************************************************************
//  $SymTabAppendIntegerCommon_EM
//
//  Append a Boolean or long long integer to the symbol table
//***************************************************************************

LPSYMENTRY SymTabAppendIntegerCommon_EM
    (APLINT aplInteger,             // The integer to append
     UBOOL  bPerm)                  // TRUE iff this STE is permanent

{
    LPSYMENTRY   lpSymEntryDest;    // Ptr to destin STE
    LPHSHENTRY   lpHshEntryDest;    // Ptr to destin HTE
    UINT         uHash;             // The hash of the value to append
    STFLAGS      stNeedFlags = {0}; // The flags we require
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPHSHTABSTR  lpHTS;             // Ptr to HshTab struc

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get a ptr to the HshTab & SymTab strucs
    lpHTS = &lpMemPTD->htsPTD;

    // Split off common Boolean cases
    if (aplInteger EQ 0 && lpMemPTD->steZero)
    {
        lpSymEntryDest = lpMemPTD->steZero;

        goto NORMAL_EXIT;
    } // End IF

    if (aplInteger EQ 1 && lpMemPTD->steOne)
    {
        lpSymEntryDest = lpMemPTD->steOne;

        goto NORMAL_EXIT;
    } // End IF

    // Hash the integer
    uHash = hashlittle
           ((const uint32_t *) &aplInteger, // A ptr to the integer to hash
            IsBooleanValue (aplInteger) ? sizeof (APLBOOL) : sizeof (APLINT),   // The # bytes pointed to
            0);                             // Initial value or previous hash
    // Set the flags of the entry we're looking for
    stNeedFlags.Perm  = bPerm;
    stNeedFlags.Imm   =
    stNeedFlags.Value =
    stNeedFlags.Inuse = TRUE;

    if (IsBooleanValue (aplInteger))
        stNeedFlags.ImmType = IMMTYPE_BOOL;
    else
        stNeedFlags.ImmType = IMMTYPE_INT;

    // Lookup the number in the symbol table
    lpSymEntryDest = SymTabLookupNumber (uHash, aplInteger, &stNeedFlags);
    if (!lpSymEntryDest)
    {
        LPHSHENTRY lpHshEntryHash;

        // This constant isn't in the ST -- find the next free entry in the HT
        lpHshEntryDest = FindNextFreeUsingHash_SPLIT_EM (uHash, TRUE, lpHTS);

        // If it's invalid, quit
        if (!lpHshEntryDest)
            goto ERROR_EXIT;

        // We *MUST NOT* call MaskTheHash until after the call
        //   to FindNextFreeUsingHash_SPLIT_EM as that call might well
        //   split a HTE and change lpHshTabSplitNext and uHashMask,
        //   and thus the result of MaskTheHash.
        lpHshEntryHash = &lpHTS->lpHshTab[MaskTheHash (uHash, lpHTS)];

        // This entry must be a principal one
        Assert (lpHshEntryHash->htFlags.PrinHash);

        // Ensure there's enough room in the symbol table for one more entry
        if (((lpHTS->lpSymTabNext - lpHTS->lpSymTab) >= lpHTS->iSymTabTotalNelm)
         && (!SymTabResize_EM (lpHTS)))
            goto ERROR_EXIT;

        // Mark this hash table entry as in use
        lpHshEntryDest->htFlags.Inuse = TRUE;

        // Save as return result
        lpSymEntryDest = lpHTS->lpSymTabNext++;

        // Save the constant
        lpSymEntryDest->stData.stInteger = aplInteger;

        // Save the symbol table flags
        stNeedFlags.Perm = bPerm;
        *(UINT *) &lpSymEntryDest->stFlags |= *(UINT *) &stNeedFlags;

        // Save hash value (so we don't have to rehash on split)
        lpHshEntryDest->uHash        = uHash;
        lpHshEntryDest->uHashAndMask = MaskTheHash (uHash, lpHTS);

        // Link the destination entry into the hash entry
        HshTabLink (lpHshEntryHash, lpHshEntryDest);

        // Save ptrs to each other in the symbol and hash tables
        lpHshEntryDest->htSymEntry = lpSymEntryDest;
        lpSymEntryDest->stHshEntry = lpHshEntryDest;

        // Clear other elements of the SYMENTRY
        lpSymEntryDest->stPrvEntry =
        lpSymEntryDest->stSymLink  = NULL;
        lpSymEntryDest->stSILevel  = 0;
    } // End IF
ERROR_EXIT:
NORMAL_EXIT:
    Assert (HshTabFrisk (lpHTS));

    return lpSymEntryDest;
} // End SymTabAppendIntegerCommon_EM


//***************************************************************************
//  $SymTabAppendFloat_EM
//
//  Append a Floating Point number to the symbol table
//***************************************************************************

LPSYMENTRY SymTabAppendFloat_EM
    (APLFLOAT aplFloat)             // The float to append

{
    return SymTabAppendFloatCommon_EM (aplFloat, FALSE);
} // End SymTabAppendFloat_EM


//***************************************************************************
//  $SymTabAppendPermFloat_EM
//
//  Append a permanent Floating Point number to the symbol table
//***************************************************************************

LPSYMENTRY SymTabAppendPermFloat_EM
    (APLFLOAT aplFloat)             // The float to append

{
    return SymTabAppendFloatCommon_EM (aplFloat, TRUE);
} // End SymTabAppendPermFloat_EM


//***************************************************************************
//  $SymTabAppendFloatCommon_EM
//
//  Append a Floating Point number to the symbol table
//***************************************************************************

LPSYMENTRY SymTabAppendFloatCommon_EM
    (APLFLOAT aplFloat,             // The float to append
     UBOOL    bPerm)                // TRUE iff this STE is permanent

{
    LPSYMENTRY   lpSymEntryDest;    // Ptr to destin STE
    LPHSHENTRY   lpHshEntryDest;    // Ptr to destin HTE
    UINT         uHash;             // The hash of the value to append
    STFLAGS      stNeedFlags = {0}; // The flags we require
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPHSHTABSTR  lpHTS;             // Ptr to HshTab struc

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get a ptr to the HshTab & SymTab strucs
    lpHTS = &lpMemPTD->htsPTD;

    // Hash the float
    uHash = hashlittle
           ((const uint32_t *) &aplFloat,   // A ptr to the float to hash
             sizeof (aplFloat),             // The # bytes pointed to
             0);                            // Initial value or previous hash
    // Set the flags of the entry we're looking for
    stNeedFlags.Perm    = bPerm;
    stNeedFlags.Imm     =
    stNeedFlags.Value   =
    stNeedFlags.Inuse   = TRUE;
    stNeedFlags.ImmType = IMMTYPE_FLOAT;

    // Lookup the number in the symbol table
    lpSymEntryDest = SymTabLookupFloat (uHash, aplFloat, &stNeedFlags);
    if (!lpSymEntryDest)
    {
        LPHSHENTRY lpHshEntryHash;

        // This constant isn't in the ST -- find the next free entry in the HT
        lpHshEntryDest = FindNextFreeUsingHash_SPLIT_EM (uHash, TRUE, lpHTS);

        // If it's invalid, quit
        if (!lpHshEntryDest)
            goto ERROR_EXIT;

        // We *MUST NOT* call MaskTheHash until after the call
        //   to FindNextFreeUsingHash_SPLIT_EM as that call might well
        //   split a HTE and change lpHshTabSplitNext and uHashMask,
        //   and thus the result of MaskTheHash.
        lpHshEntryHash = &lpHTS->lpHshTab[MaskTheHash (uHash, lpHTS)];

        // This entry must be a principal one
        Assert (lpHshEntryHash->htFlags.PrinHash);

        // Ensure there's enough room in the symbol table for one more entry
        if (((lpHTS->lpSymTabNext - lpHTS->lpSymTab) >=lpHTS->iSymTabTotalNelm)
         && (!SymTabResize_EM (lpHTS)))
            goto ERROR_EXIT;

        // Mark this hash table entry as in use
        lpHshEntryDest->htFlags.Inuse = TRUE;

        // Save as return result
        lpSymEntryDest = lpHTS->lpSymTabNext++;

        // Save the constant
        lpSymEntryDest->stData.stFloat = aplFloat;

        // Save the symbol table flags
        stNeedFlags.Perm = bPerm;
        *(UINT *) &lpSymEntryDest->stFlags |= *(UINT *) &stNeedFlags;

        // Save hash value (so we don't have to rehash on split)
        lpHshEntryDest->uHash        = uHash;
        lpHshEntryDest->uHashAndMask = MaskTheHash (uHash, lpHTS);

        // Link the destination entry into the hash entry
        HshTabLink (lpHshEntryHash, lpHshEntryDest);

        // Save ptrs to each other in the symbol and hash tables
        lpHshEntryDest->htSymEntry = lpSymEntryDest;
        lpSymEntryDest->stHshEntry = lpHshEntryDest;

        // Clear other elements of the SYMENTRY
        lpSymEntryDest->stPrvEntry =
        lpSymEntryDest->stSymLink  = NULL;
        lpSymEntryDest->stSILevel  = 0;
    } // End IF
ERROR_EXIT:
    Assert (HshTabFrisk (lpHTS));

    return lpSymEntryDest;
} // End SymTabAppendFloatCommon_EM


//***************************************************************************
//  $SymTabAppendChar_EM
//
//  Append a character to the symbol table
//***************************************************************************

LPSYMENTRY SymTabAppendChar_EM
    (APLCHAR aplChar)               // The char to append

{
    return SymTabAppendCharCommon_EM (aplChar, FALSE);
} // End SymTabAppendChar_EM


//***************************************************************************
//  $SymTabAppendPermChar_EM
//
//  Append a permanent character to the symbol table
//***************************************************************************

LPSYMENTRY SymTabAppendPermChar_EM
    (APLCHAR aplChar)               // The char to append

{
    return SymTabAppendCharCommon_EM (aplChar, TRUE);
} // End SymTabAppendPermChar_EM


//***************************************************************************
//  $SymTabAppendCharCommon_EM
//
//  Append a character to the symbol table
//***************************************************************************

LPSYMENTRY SymTabAppendCharCommon_EM
    (APLCHAR aplChar,               // The char to append
     UBOOL   bPerm)                 // TRUE iff this STE is permanent

{
    LPSYMENTRY   lpSymEntryDest;    // Ptr to destin STE
    LPHSHENTRY   lpHshEntryDest;    // Ptr to destin HTE
    UINT         uHash;             // The hash of the value to append
    STFLAGS      stNeedFlags = {0}; // The flags we require
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPHSHTABSTR  lpHTS;             // Ptr to HshTab struc

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get a ptr to the HshTab & SymTab strucs
    lpHTS = &lpMemPTD->htsPTD;

    // Hash the char
    uHash = hashlittle
           ((const uint32_t *) &aplChar,    // A ptr to the char to hash
             sizeof (aplChar),              // The # bytes pointed to
             0);                            // Initial value or previous hash
    // Set the flags of the entry we're looking for
    stNeedFlags.Perm    = bPerm;
    stNeedFlags.Imm     =
    stNeedFlags.Value   =
    stNeedFlags.Inuse   = TRUE;
    stNeedFlags.ImmType = IMMTYPE_CHAR;

    // Lookup the char in the symbol table
    lpSymEntryDest = SymTabLookupChar (uHash, aplChar, &stNeedFlags);
    if (!lpSymEntryDest)
    {
        LPHSHENTRY lpHshEntryHash;

        // This constant isn't in the ST -- find the next free entry
        lpHshEntryDest = FindNextFreeUsingHash_SPLIT_EM (uHash, TRUE, lpHTS);

        // If it's invalid, quit
        if (!lpHshEntryDest)
            goto ERROR_EXIT;

        // We *MUST NOT* call MaskTheHash until after the call
        //   to FindNextFreeUsingHash_SPLIT_EM as that call might well
        //   split a HTE and change lpHshTabSplitNext and uHashMask,
        //   and thus the result of MaskTheHash.
        lpHshEntryHash = &lpHTS->lpHshTab[MaskTheHash (uHash, lpHTS)];

        // This entry must be a principal one
        Assert (lpHshEntryHash->htFlags.PrinHash);

        // Ensure there's enough room in the symbol table for one more entry
        if (((lpHTS->lpSymTabNext - lpHTS->lpSymTab) >= lpHTS->iSymTabTotalNelm)
         && (!SymTabResize_EM (lpHTS)))
            goto ERROR_EXIT;

        // Mark this hash table entry as in use
        lpHshEntryDest->htFlags.Inuse = TRUE;

        // Save as return result
        lpSymEntryDest = lpHTS->lpSymTabNext++;

        // Save the constant
        lpSymEntryDest->stData.stChar = aplChar;

        // Save the symbol table flags
        stNeedFlags.Perm = bPerm;
        *(UINT *) &lpSymEntryDest->stFlags |= *(UINT *) &stNeedFlags;

        // Save hash value (so we don't have to rehash on split)
        lpHshEntryDest->uHash        = uHash;
        lpHshEntryDest->uHashAndMask = MaskTheHash (uHash, lpHTS);

        // Link the destination entry into the hash entry
        HshTabLink (lpHshEntryHash, lpHshEntryDest);

        // Save ptrs to each other in the symbol and hash tables
        lpHshEntryDest->htSymEntry = lpSymEntryDest;
        lpSymEntryDest->stHshEntry = lpHshEntryDest;

        // Clear other elements of the SYMENTRY
        lpSymEntryDest->stPrvEntry =
        lpSymEntryDest->stSymLink  = NULL;
        lpSymEntryDest->stSILevel  = 0;
    } // End IF
ERROR_EXIT:
    Assert (HshTabFrisk (lpHTS));

    return lpSymEntryDest;
} // End SymTabAppendCharCommon_EM


//***************************************************************************
//  $SymTabAppendName_EM
//
//  Append a name to the symbol table
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SymTabAppendName_EM"
#else
#define APPEND_NAME
#endif

LPSYMENTRY SymTabAppendName_EM
    (LPWCHAR   lpwszString,                     // Ptr to name
     LPSTFLAGS lpstFlags)                       // Ptr to incoming stFlags (may be NULL)

{
    LPSYMENTRY lpSymEntry;
    STFLAGS    stFlags = {0};

    // If no incoming flags, ...
    if (lpstFlags EQ NULL)
        lpstFlags = &stFlags;

    // Lookup the name in the symbol table
    // SymTabLookupName sets the .ObjName enum,
    //   and the .Inuse flag
    lpSymEntry = SymTabLookupName (lpwszString, lpstFlags);

    // If not found and it's a system name, fail
    //    as we don't handle unknown system names
    if (!lpSymEntry)
    {
        if (lpstFlags->ObjName EQ OBJNAME_SYS)
            goto SYNTAX_EXIT;
        else
            lpSymEntry =
              SymTabAppendNewName_EM (lpwszString, lpstFlags);
    } // End IF

    // Save flags if from caller
    if (lpstFlags NE &stFlags)
        lpSymEntry->stFlags = *lpstFlags;

    return lpSymEntry;

SYNTAX_EXIT:
    ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);

    return NULL;
} // End SymTabAppendName_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SymTabAppendNewName_EM
//
//  Append a new name to the symbol table
//    (no need to look it up as we know it isn't there)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SymTabAppendNewName_EM"
#else
#define APPEND_NAME
#endif

LPSYMENTRY SymTabAppendNewName_EM
    (LPWCHAR   lpwszString,
     LPSTFLAGS lpstFlags)

{
    int          iLen;
    LPWCHAR      lpwGlbName;        // Ptr to SymEntry's name's global memory
    UINT         uHash;
    LPSYMENTRY   lpSymEntryDest = NULL;
    LPHSHENTRY   lpHshEntryDest,
                 lpHshEntryHash;
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPHSHTABSTR  lpHTS;             // Ptr to HshTab struc

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get a ptr to the HshTab & SymTab strucs
    lpHTS = &lpMemPTD->htsPTD;

    Assert (HshTabFrisk (lpHTS));

    // Get the string length in units of WCHAR
    iLen = lstrlenW (lpwszString);

    // Hash the name
    uHash = hashlittleConv
           (lpwszString,                    // A ptr to the name to hash
             iLen,                          // The # bytes pointed to
             0);                            // Initial value or previous hash
    // This name isn't in the ST -- find the next free entry
    //   in the hash table, split if necessary
    lpHshEntryDest = FindNextFreeUsingHash_SPLIT_EM (uHash, TRUE, lpHTS);

    // If it's invalid, quit
    if (!lpHshEntryDest)
        goto ERROR_EXIT;

    // Ensure there's enough room in the symbol table for one more entry
    if (((lpHTS->lpSymTabNext - lpHTS->lpSymTab) >= lpHTS->iSymTabTotalNelm)
     && (!SymTabResize_EM (lpHTS)))
        goto ERROR_EXIT;

    // Mark this hash table entry as in use
    lpHshEntryDest->htFlags.Inuse = TRUE;

    // Save as return result
    lpSymEntryDest = lpHTS->lpSymTabNext++;

    // Zero the entry
    ZeroMemory (lpSymEntryDest, sizeof (*lpSymEntryDest));

    // Get a ptr to the corresponding hash entry
    // N.B.  It's very important to call MaskTheHash *AFTER*
    //   calling FindNextFreeUsingHash_SPLIT_EM as lpHshTabSplitNext
    //   and UhashMask might change, and thus the result of MaskTheHash.
    lpHshEntryHash = &lpHTS->lpHshTab[MaskTheHash (uHash, lpHTS)];
    Assert (lpHshEntryHash->htFlags.PrinHash);

    // Allocate global memory for the name (" + 1" for the terminating zero)
    lpHshEntryDest->htGlbName = DbgGlobalAlloc (GHND, (iLen + 1) * sizeof (WCHAR));
    if (!lpHshEntryDest->htGlbName)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpwGlbName = MyGlobalLock (lpHshEntryDest->htGlbName);

    // Copy the name to the global memory
    lstrcpyW (lpwGlbName, lpwszString);

    // We no longer need this ptr
    MyGlobalUnlock (lpHshEntryDest->htGlbName); lpwGlbName = NULL;

    // Save the flags
    lpSymEntryDest->stFlags = *lpstFlags;

    // Save hash value (so we don't have to rehash on split)
    lpHshEntryDest->uHash        = uHash;
    lpHshEntryDest->uHashAndMask = MaskTheHash (uHash, lpHTS);

    // Link the destination entry into the hash entry
    HshTabLink (lpHshEntryHash, lpHshEntryDest);

    // Save ptrs to each other in the symbol and hash tables
    lpHshEntryDest->htSymEntry = lpSymEntryDest;
    lpSymEntryDest->stHshEntry = lpHshEntryDest;

    // Clear other elements of the SYMENTRY
    lpSymEntryDest->stPrvEntry =
    lpSymEntryDest->stSymLink  = NULL;
    lpSymEntryDest->stSILevel  = 0;

    Assert (HshTabFrisk (lpHTS));

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);

    lpSymEntryDest = NULL;

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return lpSymEntryDest;
} // End SymTabAppendNewName_EM
#undef  APPEND_NAME


//***************************************************************************
//  $AllocHshTab
//
//  Allocate virtual memory for the HshTab
//***************************************************************************

UBOOL AllocHshTab
    (LPMEMVIRTSTR lpLclMemVirtStr,  // Ptr to this entry in MemVirtStr
     LPHSHTABSTR  lpHTS,            // Ptr to HshTab Struc
     UINT         uHshTabInitBlks,  // # blocks in this HshTab
     UINT         uHshTabIncrNelm,  // # HTEs by which to resize when low
     UINT         uHshTabMaxNelm)   // Maximum # HTEs

{
    UINT uHshTabInitNelm,           // Initial NELM of HshTab
         uCnt;                      // Loop counter

    // Validate the incoming constants

    // The # blocks must be a power of two
    if (uHshTabInitBlks EQ 0 || uHshTabInitBlks & (uHshTabInitBlks - 1))
        DbgStop ();

    // The maximum NELM must be divisible by DEF_HSHTAB_EPB
    if (uHshTabMaxNelm % DEF_HSHTAB_EPB)
        DbgStop ();

    // Calculate the initial # entries
    uHshTabInitNelm = uHshTabInitBlks * DEF_HSHTAB_EPB;

    // The amount to resize must be a divisor of uHshTabInitNelm
    if (uHshTabInitNelm % uHshTabIncrNelm)
        DbgStop ();

    // The maximum size must be >= the initial size
    if (uHshTabMaxNelm < uHshTabInitNelm)
        DbgStop ();

    // Initialize HTS fields
    lpHTS->iHshTabTotalNelm = uHshTabInitNelm;
    lpHTS->iHshTabBaseNelm  = uHshTabInitNelm;
    lpHTS->iHshTabIncrFree  = DEF_HSHTAB_PRIME % uHshTabInitNelm;
    lpHTS->iHshTabIncrNelm  = uHshTabIncrNelm;
    lpHTS->uHashMask        = uHshTabInitBlks - 1;
    lpHTS->iHshTabEPB       = DEF_HSHTAB_EPB;

    // Allocate virtual memory for the hash table
    lpLclMemVirtStr->IncrSize = uHshTabIncrNelm * sizeof (lpHTS->lpHshTab[0]);
    lpLclMemVirtStr->MaxSize  = uHshTabMaxNelm  * sizeof (lpHTS->lpHshTab[0]);
    lpLclMemVirtStr->IniAddr  = (LPVOID)
    lpHTS->lpHshTab =
      GuardAlloc (NULL,             // Any address
                  lpLclMemVirtStr->MaxSize,
                  MEM_RESERVE,
                  PAGE_READWRITE);
    if (!lpLclMemVirtStr->IniAddr)
        return FALSE;

    // Link this struc into the chain
    LinkMVS (lpLclMemVirtStr);

    // Commit the intial size
    MyVirtualAlloc (lpLclMemVirtStr->IniAddr,
                    uHshTabInitNelm * sizeof (lpHTS->lpHshTab[0]),
                    MEM_COMMIT,
                    PAGE_READWRITE);
    // Initialize the principal hash entry (1st one in each block).
    // This entry is never overwritten with an entry with a
    //   different hash value.
    for (uCnt = 0; uCnt < uHshTabInitNelm; uCnt += DEF_HSHTAB_EPB)
        lpHTS->lpHshTab[uCnt].htFlags.PrinHash = TRUE;

    // Initialize the next & prev same HTE values
    for (uCnt = 0; uCnt < uHshTabInitNelm; uCnt++)
    {
        lpHTS->lpHshTab[uCnt].NextSameHash =
        lpHTS->lpHshTab[uCnt].PrevSameHash = LPHSHENTRY_NONE;
    } // End FOR

    // Initialize next split entry
    lpHTS->lpHshTabSplitNext = lpHTS->lpHshTab;

    return TRUE;
} // End AllocHshTab


//***************************************************************************
//  $AllocSymTab
//
//  Allocate virtual memory for the SymTab
//***************************************************************************

UBOOL AllocSymTab
    (LPMEMVIRTSTR lpLclMemVirtStr,  // Ptr to this entry in MemVirtStr
     LPPERTABDATA lpMemPTD,         // Ptr to PerTabData global memory
     LPHSHTABSTR  lpHTS,            // Ptr to HshTab Struc
     UBOOL        bInitSTEs,        // TRUE iff we shoudl initialize STEs
     UINT         uSymTabInitNelm,  // Initial # STEs in SymTab
     UINT         uSymTabIncrNelm,  // # STEs by which to resize when low
     UINT         uSymTabMaxNelm)   // Maximum # STEs

{
    UBOOL bRet = TRUE;              // TRUE iff the result is valid

    // Validate the incoming constants

    // The amount to resize must be a divisor of uHshTabInitNelm
    if (uSymTabInitNelm % uSymTabIncrNelm)
        DbgStop ();

    // The maximum NELM must be >= the initial NELM
    if (uSymTabMaxNelm < uSymTabInitNelm)
        DbgStop ();

    // Initialize HTS fields
    lpHTS->uSymTabIncrNelm  = uSymTabIncrNelm;
    lpHTS->iSymTabTotalNelm = uSymTabInitNelm;

    // Allocate virtual memory for the symbol table
    lpLclMemVirtStr->IncrSize = uSymTabIncrNelm * sizeof (lpHTS->lpSymTab[0]);
    lpLclMemVirtStr->MaxSize  = uSymTabMaxNelm  * sizeof (lpHTS->lpSymTab[0]);
    lpLclMemVirtStr->IniAddr  = (LPVOID)
    lpHTS->lpSymTab =
      GuardAlloc (NULL,             // Any address
                  lpLclMemVirtStr->MaxSize,
                  MEM_RESERVE,
                  PAGE_READWRITE);
    if (!lpLclMemVirtStr->IniAddr)
        return FALSE;

    // Link this struc into the chain
    LinkMVS (lpLclMemVirtStr);

    // Commit the intial size
    MyVirtualAlloc (lpLclMemVirtStr->IniAddr,
                    uSymTabInitNelm * sizeof (lpHTS->lpSymTab[0]),
                    MEM_COMMIT,
                    PAGE_READWRITE);
    // Initialize next available entry
    lpHTS->lpSymTabNext = lpHTS->lpSymTab;

    // If we should initialize the STEs, ...
    if (bInitSTEs)
    {
        // Initialize the Symbol Table Entry for the constants zero, one, blank, and No Value
        lpMemPTD->steZero    = SymTabAppendPermInteger_EM (0);
        lpMemPTD->steOne     = SymTabAppendPermInteger_EM (1);
        lpMemPTD->steBlank   = SymTabAppendPermChar_EM    (L' ');
        lpMemPTD->steNoValue = lpHTS->lpSymTabNext++;

        if (lpMemPTD->steZero    EQ NULL
         || lpMemPTD->steOne     EQ NULL
         || lpMemPTD->steBlank   EQ NULL
         || lpMemPTD->steNoValue EQ NULL)
            bRet = FALSE;

        // If the result is valid, ...
        if (bRet)
        {
            // Set the flags for the NoValue entry
            lpMemPTD->steNoValue->stFlags.Perm       = TRUE;
            lpMemPTD->steNoValue->stFlags.ObjName    = OBJNAME_NONE;
            lpMemPTD->steNoValue->stFlags.stNameType = NAMETYPE_UNK;

            Assert (IsSymNoValue (lpMemPTD->steNoValue));
        } // End IF
    } // End IF

    return bRet;
} // End AllocSymTab


//***************************************************************************
//  End of File: symtab.c
//***************************************************************************
