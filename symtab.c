//***************************************************************************
//  NARS2000 -- Hash & Symbol Table Routines
//***************************************************************************

#define STRICT
#include <windows.h>
#include <stdio.h>
#include <math.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

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
//    set to the size of the hash table (DEF_HSHTAB_INITSIZE).
//  As entries are split, lpHshTabSplitNext increments by DEF_HSHTAB_EPB
//    until it reaches the base size.
//  When lpHshTabSplitNext reaches the base size, the base size is
//    incremented by DEF_HSHTAB_INITSIZE, lpHshTabSplitNext is reset
//    to point to the start of the hash table, and the hash mask is shifted
//    by one to the left filling with a 1 on the right.  As per the preceding
//    paragraph, the next split triggers a resize.
//
//  Finally, if the hash value AND the current hash mask yield an index
//    which is less than lpHshTabSplitNext, then the hash value is masked
//    with the next higher-order hash mask.
//***************************************************************************

UINT MaskTheHash
    (uHash)

{
    UINT         uHashMasked;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Initial try
    uHashMasked = uHash & lpMemPTD->uHashMask;

    // The result is multiplied by DEF_HSHTAB_EPB because
    //   the resulting index should fall on a PrinHash entry.
    // That is, it's essentially a block index.
    if (uHashMasked < (UINT) ((lpMemPTD->lpHshTabSplitNext - lpMemPTD->lpHshTab) / DEF_HSHTAB_EPB))
    {
        // Because this hash is below lpHshTabSplitNext,
        //   we use the next hash function in sequence.
        uHashMasked = uHash & (1 + 2 * lpMemPTD->uHashMask);
        Assert ((int) lpMemPTD->lpHshTab[uHashMasked].NextSameHash);
        Assert ((int) lpMemPTD->lpHshTab[uHashMasked].PrevSameHash);
        Assert (uHashMasked < (UINT) (lpMemPTD->iHshTabTotalSize / DEF_HSHTAB_EPB));
        Assert (uHashMasked < (UINT) ((2 * lpMemPTD->iHshTabBaseSize) / DEF_HSHTAB_EPB));
    } else
    {
        Assert (uHashMasked < (UINT) (lpMemPTD->iHshTabTotalSize / DEF_HSHTAB_EPB));
        Assert (uHashMasked < (UINT) ((2 * lpMemPTD->iHshTabBaseSize) / DEF_HSHTAB_EPB));
    } // End IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return DEF_HSHTAB_EPB * uHashMasked;
} // End MaskTheHash


#ifdef DEBUG
//***************************************************************************
//  $HshTabFrisk
//
//  Frisk the hash table
//***************************************************************************

BOOL HshTabFrisk
    (void)

{
    LPHSHENTRY   lp, lpNext, lpBase, lpEnd;
    LPSYMENTRY   lps;
    HTFLAGS      htFlags;
    UINT         uHashMaskNext,
                 uMark;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get hash mask for next higher hash function
    uHashMaskNext = 1 + 2 * lpMemPTD->uHashMask;

    // Ensure that the NextSameHash and PrevSameHash chains are valid
    for (lp = lpMemPTD->lpHshTab;
         lp NE &lpMemPTD->lpHshTab[lpMemPTD->iHshTabTotalSize];
         lp++)
    {
        if (lp->NextSameHash NE LPHSHENTRY_NONE)
            Assert (lp EQ lp->NextSameHash->PrevSameHash);

        if (lp->PrevSameHash NE LPHSHENTRY_NONE)
            Assert (lp EQ lp->PrevSameHash->NextSameHash);
    } // End FOR

    // Ensure that all uHashAndMask values are up-to-date
    for (lp = lpMemPTD->lpHshTab;
         lp NE &lpMemPTD->lpHshTab[lpMemPTD->iHshTabTotalSize];
         lp++)
    if (lp->htFlags.Inuse)
        Assert (lp->uHashAndMask EQ MaskTheHash (lp->uHash));

    // Ensure that all empty entries have zero flags
    //   (possibly except for .PrinHash),
    //   LPHSHENTRY_NONE in the two hash ptrs (unless .PrinHash),
    //   and zero everywhere else
    for (lp = lpMemPTD->lpHshTab;
         lp NE &lpMemPTD->lpHshTab[lpMemPTD->iHshTabTotalSize];
         lp++)
    if (!lp->htFlags.Inuse)
    {
        htFlags = lp->htFlags;
        htFlags.PrinHash = FALSE;

        Assert (0 EQ *(PUINT_PTR) &htFlags);

        if (!lp->htFlags.PrinHash)
            Assert (lp->NextSameHash EQ LPHSHENTRY_NONE
                 && lp->PrevSameHash EQ LPHSHENTRY_NONE);
        Assert (lp->uHash           EQ 0
             && lp->uHashAndMask    EQ 0
             && lp->htGlbName       EQ NULL
             && lp->htSymEntry      EQ NULL);
    } // End FOR/IF

    // Ensure that .PrinHash is set every DEF_HSHTAB_EPB entries
    //   and nowhere else
    for (lp = lpMemPTD->lpHshTab;
         lp NE &lpMemPTD->lpHshTab[lpMemPTD->iHshTabTotalSize];
         lp++)
    {
        if (lp->htFlags.PrinHash)
            // Ensure .PrinHash is on a proper boundary
            Assert (0 EQ ((lp - lpMemPTD->lpHshTab) % DEF_HSHTAB_EPB));
        else
            Assert (!lp->htFlags.PrinHash);
    } // End FOR

    // Ensure that every HTE is in some NextSameHash chain starting
    //   from a .PrinHash entry, or is empty
    for (lp = lpMemPTD->lpHshTab;
         lp NE &lpMemPTD->lpHshTab[lpMemPTD->iHshTabTotalSize];
         lp += DEF_HSHTAB_EPB)
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
    for (lp = lpMemPTD->lpHshTab;
         lp NE &lpMemPTD->lpHshTab[lpMemPTD->iHshTabTotalSize];
         lp++)
    {
        Assert ((!lp->htFlags.Inuse)
             || lp->htFlags.Temp);
        lp->htFlags.Temp = FALSE;
    } // End FOR

    uMark = (UINT) (lpMemPTD->lpHshTabSplitNext - lpMemPTD->lpHshTab);

    // Ensure that principal entries below lpHshTabSplitNext hashed with the
    //   next higher hash function all hash to below lpHshTabSplitNext.
    // In other words, we moved all of those which hashed to a higher bucket.
    for (lp  = lpMemPTD->lpHshTab;
         lp NE lpMemPTD->lpHshTabSplitNext;
         lp += DEF_HSHTAB_EPB)
    for (lpNext = lp;
         lpNext NE LPHSHENTRY_NONE;
         lpNext = lpNext->NextSameHash)
    if (lpNext->htFlags.Inuse)
    {
        Assert (((lpNext->uHash & uHashMaskNext) * DEF_HSHTAB_EPB) EQ MaskTheHash (lpNext->uHash));
        Assert (((lpNext->uHash & uHashMaskNext) * DEF_HSHTAB_EPB) < uMark);
        if (!(((lpNext->uHash & uHashMaskNext) * DEF_HSHTAB_EPB) < uMark))
        {
            DisplayHshTab ();
            MBC ("DisplayHshTab");
        } // End IF
    } // End IF

    lpBase = &lpMemPTD->lpHshTabSplitNext[lpMemPTD->iHshTabBaseSize];
    lpEnd  = &lpMemPTD->lpHshTab[lpMemPTD->iHshTabTotalSize];

    uMark = (UINT) (lpBase - lpMemPTD->lpHshTab);

    // Ensure that principal entries at or above lpBase hashed with the
    //   next higher hash function all hash to at or above lpBase.
    // In other words, we moved only those which hashed to a higher bucket.
    for (lp  = lpBase;
         lp NE lpEnd;
         lp += DEF_HSHTAB_EPB)
    for (lpNext = lp;
         lpNext NE LPHSHENTRY_NONE;
         lpNext = lpNext->NextSameHash)
    if (lpNext->htFlags.Inuse)
        Assert (((lpNext->uHash & uHashMaskNext) * DEF_HSHTAB_EPB) >= uMark);

    // Ensure that the symbol table ptrs point back to the hash table
    for (lp = lpMemPTD->lpHshTab;
         lp NE &lpMemPTD->lpHshTab[lpMemPTD->iHshTabTotalSize];
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
                DisplayHshTab ();
                DisplaySymTab (TRUE);
                MBC ("HshTabFrisk:  lp->htSymEntry EQ 0");
            } // End IF
        } // End IF/ELSE
    } // End FOR

    // Ensure that the hash table ptrs point back to the symbol table
    for (lps = lpMemPTD->lpSymTab;
/////////lps NE &lpMemPTD->lpSymTab[lpMemPTD->iSymTabTotalSize];
         lps NE lpMemPTD->lpSymTabNext;
         lps++)
    // Check for NoValue entry
    if (IsSymNoValue (lps))
        continue;
    else
    if (lps->stHshEntry->htFlags.CharIsValid)
    {
        Assert (lps->stHshEntry->htFlags.htChar NE 0
             && lps->stHshEntry->htGlbName NE NULL
             && lps->stHshEntry->htSymEntry EQ NULL);
    } else
    {
        if (!(lps->stHshEntry
         && lps->stHshEntry->htSymEntry EQ lps))
        {
            DisplayHshTab ();
            DisplaySymTab (TRUE);
            MBC ("HshTabFrisk:  lp->stHshEntry EQ 0");
        } // End IF
    } // End IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

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

BOOL HshTabResize_EM
    (int iResize)

{
    LPHSHENTRY   lpHshTabNew;
    int          i,
                 iHshTabNewSize;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    BOOL         bRet = TRUE;   // TRUE iff result is valid

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    Assert (HshTabFrisk ());
#ifdef DEBUG
    dprintfW (L"||| Resizing the hash table from %08X to %08X (%S#%d)",
              lpMemPTD->iHshTabTotalSize,
              lpMemPTD->iHshTabTotalSize + iResize,
              FNLN);
#endif
    // We need more entries
    iHshTabNewSize = lpMemPTD->iHshTabTotalSize + iResize;
    lpHshTabNew = VirtualAlloc (lpMemPTD->lpHshTab,
                                iHshTabNewSize * sizeof (HSHENTRY),
                                MEM_COMMIT,
                                PAGE_READWRITE);
    if (!lpHshTabNew)
    {
        ErrorMessageIndirect (ERRMSG_HASH_TABLE_FULL APPEND_NAME);

        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF

    Assert (lpHshTabNew EQ lpMemPTD->lpHshTab);

    // Initialize the principal hash entry (1st one in each block).
    // This entry is never overwritten with an entry with a
    //   different hash value.
    for (i = lpMemPTD->iHshTabTotalSize; i < iHshTabNewSize; i += DEF_HSHTAB_EPB)
        lpMemPTD->lpHshTab[i].htFlags.PrinHash = 1;

    // Initialize the next & prev same hash STE values
    for (i = lpMemPTD->iHshTabTotalSize; i < iHshTabNewSize; i++)
    {
        lpMemPTD->lpHshTab[i].NextSameHash =
        lpMemPTD->lpHshTab[i].PrevSameHash = LPHSHENTRY_NONE;
    } // End FOR

    // Set new hash table size
    lpMemPTD->iHshTabTotalSize = iHshTabNewSize;

    // Because iHshTabTotalSize changed, we need to change iHshTabIncr
    lpMemPTD->iHshTabIncr = DEF_HSHTAB_PRIME % (UINT) lpMemPTD->iHshTabTotalSize;

    Assert (1 EQ gcdAplInt (lpMemPTD->iHshTabIncr, lpMemPTD->iHshTabTotalSize, NULL));

    Assert (HshTabFrisk ());
ERROR_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

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

BOOL SymTabResize_EM
    (int iResize)

{
    LPSYMENTRY   lpSymTabNew;
    int          i, iSymTabNewSize;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    BOOL         bRet = TRUE;   // TRUE iff result is valid

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    Assert (HshTabFrisk ());

    DbgMsg ("||| Resizing the symbol table");

    // We need more entries
    iSymTabNewSize = lpMemPTD->iSymTabTotalSize + iResize;
    lpSymTabNew = VirtualAlloc (lpMemPTD->lpSymTab,
                                iSymTabNewSize * sizeof (SYMENTRY),
                                MEM_COMMIT,
                                PAGE_READWRITE);
    if (!lpSymTabNew)
    {
        ErrorMessageIndirect (ERRMSG_SYMBOL_TABLE_FULL APPEND_NAME);

        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF

    Assert (lpSymTabNew EQ lpMemPTD->lpSymTab);

    // Initialize the lpHshEntry values
    for (i = lpMemPTD->iSymTabTotalSize; i < iSymTabNewSize; i++)
        lpMemPTD->lpSymTab[i].stHshEntry = LPHSHENTRY_NONE;

    // Set new symbol table size
    lpMemPTD->iSymTabTotalSize = iSymTabNewSize;

    Assert (HshTabFrisk ());
ERROR_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

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

BOOL HshTabSplitNextEntry_EM
    (UINT uHash)

{
    LPHSHENTRY   lpHshEntrySrc,
                 lpHshEntryNext,
                 lpHshEntryDest,
                 lpHshEntryHash;
    int          i, iCnt;
#ifdef DEBUG
////int          iCntMoved = 0;
#endif
    UINT         uHashMaskNext;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    BOOL         bRet = TRUE;   // TRUE iff result is valid

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    Assert (HshTabFrisk ());
#ifdef DEBUG
    dprintfW (L"||| Splitting Hash Table entry %08X to %08X (%S#%d)",
              lpMemPTD->lpHshTabSplitNext,
             &lpMemPTD->lpHshTabSplitNext[lpMemPTD->iHshTabBaseSize],
              FNLN);
#endif
    // Ensure that &lpMemPTD->lpHshTabSplitNext[lpMemPTD->iHshTabBaseSize] has been allocated.
    // If not, allocate it now
    if (&lpMemPTD->lpHshTabSplitNext[lpMemPTD->iHshTabBaseSize] >= &lpMemPTD->lpHshTab[lpMemPTD->iHshTabTotalSize]
     && !HshTabResize_EM (DEF_HSHTAB_RESIZE))
    {
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF

    Assert (&lpMemPTD->lpHshTabSplitNext[lpMemPTD->iHshTabBaseSize] < &lpMemPTD->lpHshTab[lpMemPTD->iHshTabTotalSize]);

    // Now we can split the entry pointed by lpHshTabSplitNext.
    // Splitting this entry means, remasking the entries using
    //   the next higher hash function, and moving the ones
    //   which hash to the larger bucket #.

    // Count the # entries in the chain
    for (lpHshEntrySrc = lpMemPTD->lpHshTabSplitNext,
           iCnt = 0;
         lpHshEntrySrc NE LPHSHENTRY_NONE;
         iCnt++,
           lpHshEntrySrc = lpHshEntrySrc->NextSameHash)
    {};

    // Get hash mask for next higher hash function
    uHashMaskNext = 1 + 2 * lpMemPTD->uHashMask;

    // Save the hash entry
    lpHshEntryHash = &lpMemPTD->lpHshTabSplitNext[lpMemPTD->iHshTabBaseSize];

    // Move the entries which hash to a higher bucket to
    //   &lpMemPTD->lpHshTabSplitNext[lpMemPTD->iHshTabBaseSize], de-link them from the old
    //   chain, and link them into a new chain.
    for (i = 0,
           lpHshEntryNext = lpMemPTD->lpHshTabSplitNext;
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
         && ((lpHshEntrySrc->uHash & uHashMaskNext) * DEF_HSHTAB_EPB)
          NE lpHshEntrySrc->uHashAndMask)                   //   it hashes to a higher bucket, ...
        {
            HTFLAGS htFlags;
////////////LPHSHENTRY lp;
#ifdef DEBUG
////        static LPHSHENTRY lp = (LPHSHENTRY) 0xF70090;
#endif
////////////Assert (HshTabFrisk ());

            // Save as new uHashAndMask value
            lpHshEntrySrc->uHashAndMask = (lpHshEntrySrc->uHash & uHashMaskNext) * DEF_HSHTAB_EPB;

            lpHshEntryDest = FindNextFreeUsingHTE_EM (lpHshEntryHash, uHash, FALSE);    // Don't split
#ifdef DEBUG
            if (i EQ 0)
                Assert (lpHshEntryDest EQ lpHshEntryHash);  // The 1st time only
////        if (lp EQ lpHshEntrySrc)
////        {
////            wsprintf (lpszDebug,
////                      "HshTabSplitNextEntry:  Entry %08X <-- %08X (%04X) (%04X)",
////                      lpHshEntryDest,
////                      lpHshEntrySrc,
////                      lpHshEntrySrc->uHashAndMask,
////                      uHashMask);
////            MBC (lpszDebug);
////            lp = lpHshEntryDest;
////        } // End IF
#endif
            // Copy the entire HSHENTRY to its new location
            //   preserving the destination's .PrinHash flag
            htFlags = lpHshEntrySrc->htFlags;
            htFlags.PrinHash = lpHshEntryDest->htFlags.PrinHash;
            MoveMemory (lpHshEntryDest, lpHshEntrySrc, sizeof (HSHENTRY));
            lpHshEntryDest->htFlags = htFlags;

            // Repoint the matching symbol table entry
            //   to the new hash table entry
            if (!lpHshEntrySrc->htFlags.CharIsValid)
                lpHshEntrySrc->htSymEntry->stHshEntry = lpHshEntryDest;

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
////////////     lp NE &lpHshTab[iHshTabTotalSize];
////////////     lp++)
////////////if (lp->htFlags.Inuse)
////////////if (lp->uHashAndMask NE MaskTheHash (lp->uHash))
////////////{
////////////    DbgBrk ();
////////////    lp->uHashAndMask = MaskTheHash (lp->uHash);
////////////} // End IF
////////////
////////////Assert (HshTabFrisk ());
#ifdef DEBUG
////////////// Count in another entry moved
////////////iCntMoved++;
#endif
        } // End IF
    } // End FOR

    // We have split an entry, so point to the next block to split
    lpMemPTD->lpHshTabSplitNext += DEF_HSHTAB_EPB;

    // If we're at the end of the table base,
    //   reset the appropriate variables
    if ((lpMemPTD->lpHshTabSplitNext - lpMemPTD->lpHshTab) EQ lpMemPTD->iHshTabBaseSize)
    {
        lpMemPTD->lpHshTabSplitNext = lpMemPTD->lpHshTab;
        lpMemPTD->iHshTabBaseSize <<= 1;
        Assert (lpMemPTD->iHshTabBaseSize <= lpMemPTD->iHshTabTotalSize);
#ifdef DEBUG
        dprintfW (L"||| Shifting uHashMask from %04X to %04X (%S#%d)",
                  lpMemPTD->uHashMask,
                  uHashMaskNext,
                  FNLN);
#endif
        lpMemPTD->uHashMask = uHashMaskNext;
        Assert (lpMemPTD->uHashMask < (UINT) (2 * lpMemPTD->iHshTabBaseSize / DEF_HSHTAB_EPB));
    } // End IF

#ifdef DEBUG
////{ // ***DEBUG***
////    wsprintf (lpszDebug,
////              "@@@@ %d hash table entr%s moved by HshTabSplitNextEntry",
////              iCntMoved,
////              (iCntMoved EQ 1) ? "y" : "ies");
////    DbgMsg (lpszDebug);
////} // ***DEBUG*** END
#endif

    Assert (HshTabFrisk ());
ERROR_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

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
    (UINT  uHash,
     BOOL  bSplitNext)

{
    LPHSHENTRY   lpHshEntry,
                 lpHshEntryHash;
    int          i;
    BOOL         bFirstTime = TRUE;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    DBGENTER;

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    while (TRUE)
    {
        lpHshEntryHash = &lpMemPTD->lpHshTab[MaskTheHash (uHash)];

        // Loop through the current block looking for
        //   a free (not inuse) entry
        // Note that because this is the same block as uHash,
        //   this search allows PrinHash.
        for (lpHshEntry = lpHshEntryHash, i = 0;
             lpHshEntry->htFlags.Inuse
          && i < DEF_HSHTAB_EPB;
             lpHshEntry++, i++)
        {};

        // If we found it, return it
        if (i < DEF_HSHTAB_EPB)
            goto NORMAL_EXIT;
        else
        if (bSplitNext)
        {
            BOOL bTemp;

            // Split the SplitNext entry
            bTemp = HshTabSplitNextEntry_EM (uHash);
            Assert (bTemp);

            // Because we split an entry (and possibly resized the table)
            //   we need to respecify some variables.
            lpHshEntryHash = &lpMemPTD->lpHshTab[MaskTheHash (uHash)];

            // Loop through the current block looking for
            //   a free (not inuse) entry
            // Note that because this is the same block as uHash,
            //   this search allows PrinHash.
            for (lpHshEntry = lpHshEntryHash, i = 0;
                 lpHshEntry->htFlags.Inuse
              && i < DEF_HSHTAB_EPB;
                 lpHshEntry++, i++)
            {};

            // If we found it, return it
            if (i < DEF_HSHTAB_EPB)
                goto NORMAL_EXIT;
        } // End IF/ELSE/IF

        // As we didn't find a free entry, continue searching
        //   wrapping around to the start if necessary.
        lpHshEntry = lpHshEntryHash;
        do {
            // Increment to the next entry (relatively prime to total size)
            lpHshEntry += lpMemPTD->iHshTabIncr;

            // If we're at or beyond the end, wrap to the start
            if (lpHshEntry >= &lpMemPTD->lpHshTab[lpMemPTD->iHshTabTotalSize])
                lpHshEntry -= lpMemPTD->iHshTabTotalSize;

            // Check for not Inuse and not PrinHash
            if (!(lpHshEntry->htFlags.Inuse || lpHshEntry->htFlags.PrinHash))
                break;
        } while (lpHshEntry NE lpHshEntryHash);

        // If we found it, return it
        if (lpHshEntry NE lpHshEntryHash)
            goto NORMAL_EXIT;

        // As we didn't find a free entry, try to expand the hash table
        if (!HshTabResize_EM (DEF_HSHTAB_RESIZE))
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

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

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
    (LPHSHENTRY lpHshEntryHash,
     UINT       uHash,
     BOOL       bSplitNext)

{
    LPHSHENTRY   lpHshEntry;
    int          i;
    BOOL         bFirstTime = TRUE;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    DBGENTER;

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    while (TRUE)
    {
        // Loop through the current block looking for
        //   a free (not inuse) entry
        // Note that because this is the same block as uHash,
        //   this search allows PrinHash.
        for (lpHshEntry = lpHshEntryHash, i = 0;
             lpHshEntry->htFlags.Inuse
          && i < DEF_HSHTAB_EPB;
             lpHshEntry++, i++)
        {};

        // If we found it, return it
        if (i < DEF_HSHTAB_EPB)
            goto NORMAL_EXIT;
        else
        if (bSplitNext)
        {
            BOOL bTemp;

            // Split the SplitNext entry
            bTemp = HshTabSplitNextEntry_EM (uHash);
            Assert (bTemp);

            // Because we split an entry (and possibly resized the table)
            //   we need to respecify some variables.
            lpHshEntryHash = &lpMemPTD->lpHshTab[MaskTheHash (uHash)];

            // Loop through the current block looking for
            //   a free (not inuse) entry
            // Note that because this is the same block as uHash,
            //   this search allows PrinHash.
            for (lpHshEntry = lpHshEntryHash, i = 0;
                 lpHshEntry->htFlags.Inuse
              && i < DEF_HSHTAB_EPB;
                 lpHshEntry++, i++)
            {};

            // If we found it, return it
            if (i < DEF_HSHTAB_EPB)
                goto NORMAL_EXIT;
        } // End IF/ELSE/IF

        // As we didn't find a free entry, continue searching
        //   wrapping around to the start if necessary.
        lpHshEntry = lpHshEntryHash;
        do {
            // Increment to the next entry (relatively prime to total size)
            lpHshEntry += lpMemPTD->iHshTabIncr;

            // If we're at or beyond the end, wrap to the start
            if (lpHshEntry >= &lpMemPTD->lpHshTab[lpMemPTD->iHshTabTotalSize])
                lpHshEntry -= lpMemPTD->iHshTabTotalSize;

            // Check for not Inuse and not PrinHash
            if (!(lpHshEntry->htFlags.Inuse || lpHshEntry->htFlags.PrinHash))
                break;
        } while (lpHshEntry NE lpHshEntryHash);

        // If we found it, return it
        if (lpHshEntry NE lpHshEntryHash)
            goto NORMAL_EXIT;

        // As we didn't find a free entry, try to expand the hash table
        if (!HshTabResize_EM (DEF_HSHTAB_RESIZE))
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

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpHshEntry;
} // End FindNextFreeUsingHTE_EM
#undef  APPEND_NAME


//***************************************************************************
//  $HshTabLookupChar
//
//  Lookup a char based upon the char
//***************************************************************************

LPHSHENTRY HshTabLookupChar
    (APLCHAR aplChar,           // The char to lookup
     UINT    uHash)             // The hash of the char

{
    LPHSHENTRY   lpHshEntry;        // Ptr to the result
    HTFLAGS      htMaskFlags = {0}; // Hash table mask flags
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Set mask flags
    htMaskFlags.CharIsValid = 1;

    // Loop through the hash table entries
    for (lpHshEntry = &lpMemPTD->lpHshTab[MaskTheHash (uHash)];
         lpHshEntry NE LPHSHENTRY_NONE;
         lpHshEntry = lpHshEntry->NextSameHash)
#ifdef DEBUG
    if (!lpHshEntry)
    {
        DisplayHshTab ();
        DbgStop ();             // We should never get here
    } else
#endif
    {
        // Check the flags
        if (lpHshEntry->htFlags.Inuse
         && ((*(PUINT_PTR) &lpHshEntry->htFlags) & *(PUINT_PTR) &htMaskFlags)
             EQ *(PUINT_PTR) &htMaskFlags
         && aplChar EQ lpHshEntry->htFlags.htChar)
            break;
    } // End FOR/IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpHshEntry;
} // End HshTabLookupChar


//***************************************************************************
//  $AppendSymbolName
//
//  Append the name and value of a symbol to the hash table
//***************************************************************************

BOOL AppendSymbolName
    (APLCHAR aplChar,
     LPCHAR  lpCharName)

{
    LPHSHENTRY   lpHshEntryDest,        // Ptr to the result
                 lpHshEntryHash;        // Ptr to principal hash entry
    UINT         uHash;                 // The hash of the char
    BOOL         bRet = FALSE;          // TRUE iff result is valid
    HGLOBAL      hGlbPTD;               // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Hash the char
    uHash = hashlittle
           ((const uint32_t *) &aplChar,    // A ptr to the char to hash
             sizeof (aplChar),              // The # values pointed to
             0);                            // Initial value or previous hash
    // Look up the char
    lpHshEntryDest = HshTabLookupChar (aplChar, uHash);
    if (lpHshEntryDest EQ LPHSHENTRY_NONE)
    {
        // This name isn't in the HT -- find the next free entry
        //   in the hash table, split if necessary
        lpHshEntryDest = FindNextFreeUsingHash_SPLIT_EM (uHash, TRUE);

        // If it's invalid, quit
        if (!lpHshEntryDest)
            goto ERROR_EXIT;

        // We *MUST NOT* call MaskTheHash until after the call
        //   to FindNextFreeUsingHash_SPLIT_EM as that call might well
        //   split a HTE and change lpHshTabSplitNext and uHashMask,
        //   and thus the result of MaskTheHash.
        lpHshEntryHash = &lpMemPTD->lpHshTab[MaskTheHash (uHash)];

        // This entry must be a principal one
        Assert (lpHshEntryHash->htFlags.PrinHash);

        // Mark this hash table entry as in use
        lpHshEntryDest->htFlags.Inuse       =
        lpHshEntryDest->htFlags.CharIsValid = 1;
        lpHshEntryDest->htFlags.htChar      = aplChar;

        // Save the ptr to the name
        lpHshEntryDest->htGlbName = lpCharName;

        // Save hash value (so we don't have to rehash on split)
        lpHshEntryDest->uHash        = uHash;
        lpHshEntryDest->uHashAndMask = MaskTheHash (uHash);

        // Link the destination entry into the hash entry
        HshTabLink (lpHshEntryHash, lpHshEntryDest);

        // Mark as successful
        bRet = TRUE;
    } else
        DbgStop ();     // We should never get here
ERROR_EXIT:
    Assert (HshTabFrisk ());

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End AppendSymbolName


//***************************************************************************
//  $InitSymbolNames
//
//  Append all symbol names and values to the hash table
//***************************************************************************

BOOL InitSymbolNames
    (void)

{
    UINT uSym;          // Loop counter

    // Check for too few rows in the table
    if (aSymbolNames[ASYMBOLNAMES_NROWS - 1].Symbol EQ 0)
        DbgStop ();         // We should never get here
    else
    // Loop through all symbol names/values
    for (uSym = 0; uSym < ASYMBOLNAMES_NROWS; uSym++)
    if (!AppendSymbolName (aSymbolNames[uSym].Symbol,
                           aSymbolNames[uSym].lpName))
        return FALSE;

    return TRUE;
} // End InitSymbolNames


//***************************************************************************
//  $SymbolToName
//
//  Translate a symbol to its canonical name
//***************************************************************************

LPCHAR SymbolToName
    (APLCHAR aplChar)

{
    UINT       uHash;               // The hash of the char
    LPHSHENTRY lpHshEntryDest;      // Ptr to the matching hash table entry

    // Hash the char
    uHash = hashlittle
           ((const uint32_t *) &aplChar,    // A ptr to the char to hash
             sizeof (aplChar),              // The # values pointed to
             0);                            // Initial value or previous hash
    // Look up the char
    lpHshEntryDest = HshTabLookupChar (aplChar, uHash);
    if (lpHshEntryDest NE LPHSHENTRY_NONE)
        return lpHshEntryDest->htGlbName;
    else
        return NULL;
} // End SymbolToName


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
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Set mask flags
    stMaskFlags.Imm     =
    stMaskFlags.Perm    =
    stMaskFlags.Value   =
    stMaskFlags.Inuse   = 1;
    stMaskFlags.ImmType = NEG1U;

    // Save common value
    uHashMasked = MaskTheHash (uHash);

    for (lpHshEntry = &lpMemPTD->lpHshTab[uHashMasked];
         lpHshEntry NE LPHSHENTRY_NONE;
         lpHshEntry = lpHshEntry->NextSameHash)
#ifdef DEBUG
    if (!lpHshEntry)
    {
        DisplayHshTab ();
        DbgStop ();             // We should never get here
    } else
#endif
    {
        // Check the flags
        if (lpHshEntry->htFlags.Inuse
         && !lpHshEntry->htFlags.CharIsValid
         && ((*(PUINT_PTR) &lpHshEntry->htSymEntry->stFlags) & *(PUINT_PTR) &stMaskFlags)
             EQ *(PUINT_PTR) lpstFlags
         && aplChar EQ lpHshEntry->htSymEntry->stData.stChar)
        {
            lpSymEntry = lpHshEntry->htSymEntry;

            break;
        } // End IF
    } // End FOR/IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

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
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Set mask flags
    stMaskFlags.Imm   =
    stMaskFlags.Perm  =
    stMaskFlags.Value =
    stMaskFlags.Inuse = 1;
    stMaskFlags.ImmType = NEG1U;

    // Save common value
    uHashMasked = MaskTheHash (uHash);

#ifdef DEBUG
////DisplayHshTab ();
#endif
    for (lpHshEntry = &lpMemPTD->lpHshTab[uHashMasked];
         lpHshEntry NE LPHSHENTRY_NONE;
         lpHshEntry = lpHshEntry->NextSameHash)
#ifdef DEBUG
    if (!lpHshEntry)
    {
        DisplayHshTab ();
        DbgStop ();             // We should never get here
    } else
#endif
    {
        // Check the flags
        if (lpHshEntry->htFlags.Inuse
         && !lpHshEntry->htFlags.CharIsValid
         && ((*(PUINT_PTR) &lpHshEntry->htSymEntry->stFlags) & *(PUINT_PTR) &stMaskFlags)
             EQ *(PUINT_PTR) lpstNeedFlags
         && aplInteger EQ lpHshEntry->htSymEntry->stData.stInteger)
        {
            lpSymEntry = lpHshEntry->htSymEntry;

            break;
        } // End IF
    } // End FOR/IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

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
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Set mask flags
    stMaskFlags.Imm   =
    stMaskFlags.Perm  =
    stMaskFlags.Value =
    stMaskFlags.Inuse = 1;
    stMaskFlags.ImmType = NEG1U;

    // Save common value
    uHashMasked = MaskTheHash (uHash);

    for (lpHshEntry = &lpMemPTD->lpHshTab[uHashMasked];
         lpHshEntry NE LPHSHENTRY_NONE;
         lpHshEntry = lpHshEntry->NextSameHash)
#ifdef DEBUG
    if (!lpHshEntry)
    {
        DisplayHshTab ();
        DbgStop ();             // We should never get here
    } else
#endif
    {
        // Check the flags
        if (lpHshEntry->htFlags.Inuse
         && !lpHshEntry->htFlags.CharIsValid
         && ((*(PUINT_PTR) &lpHshEntry->htSymEntry->stFlags) & *(PUINT_PTR) &stMaskFlags)
             EQ *(PUINT_PTR) lpstFlags
         && fFloat EQ lpHshEntry->htSymEntry->stData.stFloat)
        {
            lpSymEntry = lpHshEntry->htSymEntry;

            break;
        } // End IF
    } // End FOR/IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpSymEntry;
} // End SymTabLookupFloat


//***************************************************************************
//  $SymTabLookupName
//
//  Lookup a named entry based upon hash, flags, and value
//***************************************************************************

LPSYMENTRY SymTabLookupName
    (LPWCHAR   lpwszString,
     LPSTFLAGS lpstFlags)

{
    return SymTabLookupNameLength (lpwszString,
                                   lstrlenW (lpwszString),
                                   lpstFlags);
} // End SymTabLookupName


//***************************************************************************
//  $SymTabLookupNameLength
//
//  Lookup a named entry based upon hash, flags, and value
//***************************************************************************

LPSYMENTRY SymTabLookupNameLength
    (LPWCHAR   lpwszString,
     int       iLen,
     LPSTFLAGS lpstFlags)

{
    LPHSHENTRY   lpHshEntry;
    STFLAGS      stMaskFlags = {0};
    UINT         uHash;
    LPSYMENTRY   lpSymEntry = NULL;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Skip over trailing white space
    while (iLen && lpwszString[iLen - 1] EQ L' ')
        iLen--;

    if (iLen EQ 0)
        return NULL;

    // Hash the name
    uHash = hashlittle
           ((const uint32_t *) lpwszString, // A ptr to the name to hash
             iLen * sizeof (WCHAR),         // The # values pointed to
             0);                            // Initial value or previous hash
    // Set the flags of the entry we're looking for
    if (lpwszString[0] EQ UTF16_QUAD
     || lpwszString[0] EQ UTF16_QUOTEQUAD)
        lpstFlags->ObjName = OBJNAME_SYS;
    else
        lpstFlags->ObjName = OBJNAME_USR;

    // Mark as in use
    lpstFlags->Inuse   = 1;

    // Set mask flags
    stMaskFlags.ObjName = NEG1U;
    stMaskFlags.Inuse   = 1;

    for (lpHshEntry = &lpMemPTD->lpHshTab[MaskTheHash (uHash)];
         lpHshEntry NE LPHSHENTRY_NONE;
         lpHshEntry = lpHshEntry->NextSameHash)
    {
        // Check the flags
        if (lpHshEntry->htFlags.Inuse
         && !lpHshEntry->htFlags.CharIsValid
         && ((*(PUINT_PTR) &lpHshEntry->htSymEntry->stFlags) & *(PUINT_PTR) &stMaskFlags)
             EQ *(PUINT_PTR) lpstFlags)
        {
            LPWCHAR lpGlbName;
            int     iCmp;
#ifdef DEBUG
////////////DisplayHshTab ();
#endif
            Assert (lpHshEntry->htGlbName NE NULL);

            // Lock the memory to get a ptr to it
            lpGlbName = MyGlobalLock (lpHshEntry->htGlbName);

            // Compare with or without sensitivity to case
            if (lpHshEntry->htSymEntry->stFlags.NotCase)
                iCmp = _wcsnicmp (lpGlbName, lpwszString, iLen);
            else
                iCmp =  wcsncmp  (lpGlbName, lpwszString, iLen);

            // We no longer need this ptr
            MyGlobalUnlock (lpHshEntry->htGlbName); lpGlbName = NULL;

            // If it matched, return it
            if (iCmp EQ 0)
            {
                lpSymEntry = lpHshEntry->htSymEntry;

                break;
            } // End IF
        } // End IF
    } // End FOR

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpSymEntry;
} // End SymTabLookupNameLength


//***************************************************************************
//  $MakePermSymEntry_EM
//
//  Make a permanent SYMENTRY with a given type and value
//***************************************************************************

LPSYMENTRY MakePermSymEntry_EM
    (IMM_TYPES    immType,          // ImmType to use (see IMM_TYPES)
     LPAPLLONGEST lpVal,            // Value to use
     LPTOKEN      lptkFunc)         // Ptr to token to use in case of error

{
    LPSYMENTRY lpSymDst;

    // Split cases based upon the immediate type
    switch (immType)
    {
        case IMMTYPE_BOOL:
            lpSymDst = SymTabAppendPermInteger_EM (*(LPAPLBOOL)  lpVal);

            break;

        case IMMTYPE_INT:
            lpSymDst = SymTabAppendPermInteger_EM (*(LPAPLINT)   lpVal);

            break;

        case IMMTYPE_CHAR:
            lpSymDst = SymTabAppendPermChar_EM    (*(LPAPLCHAR)  lpVal);

            break;

        case IMMTYPE_FLOAT:
            lpSymDst = SymTabAppendPermFloat_EM   (*(LPAPLFLOAT) lpVal);

            break;

        defstop
            lpSymDst = NULL;

            break;
    } // End SWITCH

    // If it failed, set the error token
    if (!lpSymDst)
        ErrorMessageSetToken (lptkFunc);

    return lpSymDst;
} // End MakePermSymEntry_EM


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
    (APLINT aplInteger)

{
    return SymTabAppendIntegerCommon_EM (aplInteger, FALSE);
} // End SymTabAppendInteger_EM


//***************************************************************************
//  $SymTabAppendPermInteger_EM
//
//  Append a permanent Boolean or long long integer to the symbol table
//***************************************************************************

LPSYMENTRY SymTabAppendPermInteger_EM
    (APLINT aplInteger)

{
    return SymTabAppendIntegerCommon_EM (aplInteger, TRUE);
} // End SymTabAppendPermInteger_EM


//***************************************************************************
//  $SymTabAppendIntegerCommon_EM
//
//  Append a Boolean or long long integer to the symbol table
//***************************************************************************

LPSYMENTRY SymTabAppendIntegerCommon_EM
    (APLINT aplInteger,
     BOOL   bPerm)

{
    LPSYMENTRY   lpSymEntryDest;    // Ptr to dest SYMENTRY
    LPHSHENTRY   lpHshEntryDest;    // Ptr to dest HSHENTRY
    UINT         uHash;             // The hased value
    STFLAGS      stNeedFlags = {0}; // The flags we require
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

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
             IsBooleanValue (aplInteger) ? sizeof (APLBOOL) : sizeof (APLINT), // The # values pointed to
             0);                            // Initial value or previous hash
    // Set the flags of the entry we're looking for
    stNeedFlags.Perm  = bPerm;
    stNeedFlags.Imm   =
    stNeedFlags.Value =
    stNeedFlags.Inuse = 1;

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
        lpHshEntryDest = FindNextFreeUsingHash_SPLIT_EM (uHash, TRUE);

        // If it's invalid, quit
        if (!lpHshEntryDest)
            goto ERROR_EXIT;

        // We *MUST NOT* call MaskTheHash until after the call
        //   to FindNextFreeUsingHash_SPLIT_EM as that call might well
        //   split a HTE and change lpHshTabSplitNext and uHashMask,
        //   and thus the result of MaskTheHash.
        lpHshEntryHash = &lpMemPTD->lpHshTab[MaskTheHash (uHash)];

        // This entry must be a principal one
        Assert (lpHshEntryHash->htFlags.PrinHash);

        // Ensure there's enough room in the symbol table for one more entry
        if (((lpMemPTD->lpSymTabNext - lpMemPTD->lpSymTab) >= lpMemPTD->iSymTabTotalSize)
         && (!SymTabResize_EM (DEF_SYMTAB_RESIZE)))
            goto ERROR_EXIT;

        // Mark this hash table entry as in use
        lpHshEntryDest->htFlags.Inuse = 1;

        // Save as return result
        lpSymEntryDest = lpHshEntryDest->htSymEntry = lpMemPTD->lpSymTabNext++;

        // Save the constant
        lpSymEntryDest->stData.stInteger = aplInteger;

        // Save the symbol table flags
        stNeedFlags.Perm = bPerm;
        *(PUINT_PTR) &lpSymEntryDest->stFlags |= *(PUINT_PTR) &stNeedFlags;

        // Save hash value (so we don't have to rehash on split)
        lpHshEntryDest->uHash        = uHash;
        lpHshEntryDest->uHashAndMask = MaskTheHash (uHash);

        // Link the destination entry into the hash entry
        HshTabLink (lpHshEntryHash, lpHshEntryDest);

        // In the hash table, save ptr to the symbol table
        lpHshEntryDest->htSymEntry = lpSymEntryDest;

        // In the symbol table, save ptr to the hash table
        lpSymEntryDest->stHshEntry = lpHshEntryDest;
    } // End IF
ERROR_EXIT:
NORMAL_EXIT:
    Assert (HshTabFrisk ());

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpSymEntryDest;
} // End SymTabAppendIntegerCommon_EM


//***************************************************************************
//  $SymTabAppendFloat_EM
//
//  Append a Floating Point number to the symbol table
//***************************************************************************

LPSYMENTRY SymTabAppendFloat_EM
    (APLFLOAT aplFloat)

{
    return SymTabAppendFloatCommon_EM (aplFloat, FALSE);
} // End SymTabAppendFloat_EM


//***************************************************************************
//  $SymTabAppendPermFloat_EM
//
//  Append a permanent Floating Point number to the symbol table
//***************************************************************************

LPSYMENTRY SymTabAppendPermFloat_EM
    (APLFLOAT aplFloat)

{
    return SymTabAppendFloatCommon_EM (aplFloat, TRUE);
} // End SymTabAppendPermFloat_EM


//***************************************************************************
//  $SymTabAppendFloatCommon_EM
//
//  Append a Floating Point number to the symbol table
//***************************************************************************

LPSYMENTRY SymTabAppendFloatCommon_EM
    (APLFLOAT aplFloat,
     BOOL     bPerm)

{
    LPSYMENTRY   lpSymEntryDest;
    LPHSHENTRY   lpHshEntryDest;
    UINT         uHash;
    STFLAGS      stFlags = {0};
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Hash the float
    uHash = hashlittle
           ((const uint32_t *) &aplFloat,   // A ptr to the float to hash
             sizeof (aplFloat),             // The # values pointed to
             0);                            // Initial value or previous hash
    // Set the flags of the entry we're looking for
    stFlags.Imm   =
    stFlags.Perm  =
    stFlags.Value =
    stFlags.Inuse = 1;
    stFlags.ImmType = IMMTYPE_FLOAT;

    // Lookup the number in the symbol table
    lpSymEntryDest = SymTabLookupFloat (uHash, aplFloat, &stFlags);
    if (!lpSymEntryDest)
    {
        LPHSHENTRY lpHshEntryHash;

        // This constant isn't in the ST -- find the next free entry in the HT
        lpHshEntryDest = FindNextFreeUsingHash_SPLIT_EM (uHash, TRUE);

        // If it's invalid, quit
        if (!lpHshEntryDest)
            goto ERROR_EXIT;

        // We *MUST NOT* call MaskTheHash until after the call
        //   to FindNextFreeUsingHash_SPLIT_EM as that call might well
        //   split a HTE and change lpHshTabSplitNext and uHashMask,
        //   and thus the result of MaskTheHash.
        lpHshEntryHash = &lpMemPTD->lpHshTab[MaskTheHash (uHash)];

        // This entry must be a principal one
        Assert (lpHshEntryHash->htFlags.PrinHash);

        // Ensure there's enough room in the symbol table for one more entry
        if (((lpMemPTD->lpSymTabNext - lpMemPTD->lpSymTab) >=lpMemPTD-> iSymTabTotalSize)
         && (!SymTabResize_EM (DEF_SYMTAB_RESIZE)))
            goto ERROR_EXIT;

        // Mark this hash table entry as in use
        lpHshEntryDest->htFlags.Inuse = 1;

        // Save as return result
        lpSymEntryDest = lpHshEntryDest->htSymEntry = lpMemPTD->lpSymTabNext++;

        // Save the constant
        lpSymEntryDest->stData.stFloat = aplFloat;

        // Save the symbol table flags
        stFlags.Perm = bPerm;
        *(PUINT_PTR) &lpSymEntryDest->stFlags |= *(PUINT_PTR) &stFlags;

        // Save hash value (so we don't have to rehash on split)
        lpHshEntryDest->uHash        = uHash;
        lpHshEntryDest->uHashAndMask = MaskTheHash (uHash);

        // Link the destination entry into the hash entry
        HshTabLink (lpHshEntryHash, lpHshEntryDest);

        // In the hash table, save ptr to the symbol table
        lpHshEntryDest->htSymEntry = lpSymEntryDest;

        // In the symbol table, save ptr to the hash table
        lpSymEntryDest->stHshEntry = lpHshEntryDest;
    } // End IF
ERROR_EXIT:
    Assert (HshTabFrisk ());

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpSymEntryDest;
} // End SymTabAppendFloatCommon_EM


//***************************************************************************
//  $SymTabAppendChar_EM
//
//  Append a character to the symbol table
//***************************************************************************

LPSYMENTRY SymTabAppendChar_EM
    (APLCHAR aplChar)

{
    return SymTabAppendCharCommon_EM (aplChar, FALSE);
} // End SymtabAppendChar_EM


//***************************************************************************
//  $SymTabAppendPermChar_EM
//
//  Append a permanent character to the symbol table
//***************************************************************************

LPSYMENTRY SymTabAppendPermChar_EM
    (APLCHAR aplChar)

{
    return SymTabAppendCharCommon_EM (aplChar, TRUE);
} // End SymtabAppendPermChar_EM


//***************************************************************************
//  $SymTabAppendCharCommon_EM
//
//  Append a character to the symbol table
//***************************************************************************

LPSYMENTRY SymTabAppendCharCommon_EM
    (APLCHAR aplChar,
     BOOL    bPerm)

{
    LPSYMENTRY   lpSymEntryDest;
    LPHSHENTRY   lpHshEntryDest;
    UINT         uHash;
    STFLAGS      stFlags = {0};
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Hash the char
    uHash = hashlittle
           ((const uint32_t *) &aplChar,    // A ptr to the char to hash
             sizeof (aplChar),              // The # values pointed to
             0);                            // Initial value or previous hash
    // Set the flags of the entry we're looking for
    stFlags.Imm   =
    stFlags.Perm  =
    stFlags.Value =
    stFlags.Inuse = 1;
    stFlags.ImmType = IMMTYPE_CHAR;

    // Lookup the char in the symbol table
    lpSymEntryDest = SymTabLookupChar (uHash, aplChar, &stFlags);
    if (!lpSymEntryDest)
    {
        LPHSHENTRY lpHshEntryHash;

        // This constant isn't in the ST -- find the next free entry
        lpHshEntryDest = FindNextFreeUsingHash_SPLIT_EM (uHash, TRUE);

        // If it's invalid, quit
        if (!lpHshEntryDest)
            goto ERROR_EXIT;

        // We *MUST NOT* call MaskTheHash until after the call
        //   to FindNextFreeUsingHash_SPLIT_EM as that call might well
        //   split a HTE and change lpHshTabSplitNext and uHashMask,
        //   and thus the result of MaskTheHash.
        lpHshEntryHash = &lpMemPTD->lpHshTab[MaskTheHash (uHash)];

        // This entry must be a principal one
        Assert (lpHshEntryHash->htFlags.PrinHash);

        // Ensure there's enough room in the symbol table for one more entry
        if (((lpMemPTD->lpSymTabNext - lpMemPTD->lpSymTab) >= lpMemPTD->iSymTabTotalSize)
         && (!SymTabResize_EM (DEF_SYMTAB_RESIZE)))
            goto ERROR_EXIT;

        // Mark this hash table entry as in use
        lpHshEntryDest->htFlags.Inuse = 1;

        // Save as return result
        lpSymEntryDest = lpHshEntryDest->htSymEntry = lpMemPTD->lpSymTabNext++;

        // Save the constant
        lpSymEntryDest->stData.stChar = aplChar;

        // Save the symbol table flags
        stFlags.Perm = bPerm;
        *(PUINT_PTR) &lpSymEntryDest->stFlags |= *(PUINT_PTR) &stFlags;

        // Save hash value (so we don't have to rehash on split)
        lpHshEntryDest->uHash        = uHash;
        lpHshEntryDest->uHashAndMask = MaskTheHash (uHash);

        // Link the destination entry into the hash entry
        HshTabLink (lpHshEntryHash, lpHshEntryDest);

        // In the hash table, save ptr to the symbol table
        lpHshEntryDest->htSymEntry = lpSymEntryDest;

        // In the symbol table, save ptr to the hash table
        lpSymEntryDest->stHshEntry = lpHshEntryDest;
    } // End IF
ERROR_EXIT:
    Assert (HshTabFrisk ());

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

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
    (LPWCHAR lpwszString)

{
    LPSYMENTRY lpSymEntry;
    STFLAGS    stFlags = {0};

    // Lookup the name in the symbol table
    // SymTabLookupName sets the .ObjName enum,
    //   and the .Inuse flag
    lpSymEntry = SymTabLookupName (lpwszString, &stFlags);

    // If not found and it's a system name, fail
    //    as we don't handle unknown system names
    if (!lpSymEntry)
    {
        if (stFlags.ObjName EQ OBJNAME_SYS)
        {
            ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);

            return NULL;
        } else
            lpSymEntry = SymTabAppendNewName_EM (lpwszString, &stFlags);
    } // End IF

    return lpSymEntry;
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
    LPWCHAR      lpGlbName;
    UINT         uHash;
    LPSYMENTRY   lpSymEntryDest = NULL;
    LPHSHENTRY   lpHshEntryDest,
                 lpHshEntryHash;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    Assert (HshTabFrisk ());

    // Get the string length in units of WCHAR
    iLen = lstrlenW (lpwszString);

    // Hash the name
    uHash = hashlittle
           ((const uint32_t *) lpwszString, // A ptr to the name to hash
             iLen * sizeof (WCHAR),         // The # values pointed to
             0);                            // Initial value or previous hash
    // This name isn't in the ST -- find the next free entry
    //   in the hash table, split if necessary
    lpHshEntryDest = FindNextFreeUsingHash_SPLIT_EM (uHash, TRUE);

    // If it's invalid, quit
    if (!lpHshEntryDest)
        goto ERROR_EXIT;

    // Ensure there's enough room in the symbol table for one more entry
    if (((lpMemPTD->lpSymTabNext - lpMemPTD->lpSymTab) >= lpMemPTD->iSymTabTotalSize)
     && (!SymTabResize_EM (DEF_SYMTAB_RESIZE)))
        goto ERROR_EXIT;

    // Mark this hash table entry as in use
    lpHshEntryDest->htFlags.Inuse = 1;

    // Save as return result
    lpSymEntryDest = lpHshEntryDest->htSymEntry = lpMemPTD->lpSymTabNext++;

    // Get a ptr to the corresponding hash entry
    // N.B.  It's very important to call MaskTheHash *AFTER*
    //   calling FindNextFreeUsingHash_SPLIT_EM as lpHshTabSplitNext
    //   and UhashMask might change, and thus the result of MaskTheHash.
    lpHshEntryHash = &lpMemPTD->lpHshTab[MaskTheHash (uHash)];
    Assert (lpHshEntryHash->htFlags.PrinHash);

    // Allocate global memory for the name ("+1" for the terminating zero)
    lpHshEntryDest->htGlbName = DbgGlobalAlloc (GHND, (iLen + 1) * sizeof (WCHAR));
    if (!lpHshEntryDest->htGlbName)
    {
        ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);

        lpSymEntryDest = NULL;

        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpGlbName = MyGlobalLock (lpHshEntryDest->htGlbName);

    // Copy the name to the global memory
    lstrcpyW (lpGlbName, lpwszString);

    // We no longer need this ptr
    MyGlobalUnlock (lpHshEntryDest->htGlbName); lpGlbName = NULL;

    // Save the flags
    *(PUINT_PTR) &lpSymEntryDest->stFlags |= *(PUINT_PTR) lpstFlags;

    // Save hash value (so we don't have to rehash on split)
    lpHshEntryDest->uHash        = uHash;
    lpHshEntryDest->uHashAndMask = MaskTheHash (uHash);

    // Link the destination entry into the hash entry
    HshTabLink (lpHshEntryHash, lpHshEntryDest);

    // Save ptrs to each other in the symbol and hash tables
    lpHshEntryDest->htSymEntry = lpSymEntryDest;
    lpSymEntryDest->stHshEntry = lpHshEntryDest;

    Assert (HshTabFrisk ());
ERROR_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpSymEntryDest;
} // End SymTabAppendNewName_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: symtab.c
//***************************************************************************
