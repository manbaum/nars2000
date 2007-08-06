//***************************************************************************
//  NARS2000 -- Sort Routines
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $ShellSort
//
//  A simple Shell sort of ptr objects
//***************************************************************************

void ShellSort
    (LPVOID *lpObj,     // Ptr to objects to sort
     UINT    uSize,     // # objects to sort
     int (*ObjCmp) (LPVOID, LPVOID)) // Ptr to comparison routine

{
  UINT i, j, increment;
  LPVOID *temp;

  increment = uSize / 2;

  while (increment > 0)
  {
    for (i=increment; i < uSize; i++)
    {
      j = i;
      temp = lpObj[i];
      while ((j >= increment) && (ObjCmp (lpObj[j-increment], temp) > 0))
      {
        lpObj[j] = lpObj[j - increment];
        j = j - increment;
      }
      lpObj[j] = temp;
    } // End FOR

    if (increment EQ 2)
       increment = 1;
    else
       increment = (UINT) (increment / 2.2);
  } // End WHILE
} // End ShellSort


//***************************************************************************
//  $CmpHGLOBAL
//
//  Compare two HGLOBALs being sorted
//
//  Note that the names may include the characters
//    {alpha}, {omega}, {delta}, {deltaunderbar}, and {overbar}.
//***************************************************************************

int CmpHGLOBAL
    (HGLOBAL hGlbLft,
     HGLOBAL hGlbRht)

{
    LPAPLCHAR lpMemLft,     // Ptr to left arg global memory
              lpMemRht;     // Ptr to right ...
    int       iCmp;         // Comparison result

    // Lock the memory to get a ptr to it
    lpMemLft = MyGlobalLock (hGlbLft);
    lpMemRht = MyGlobalLock (hGlbRht);

    // Compare the two names
    iCmp = aplcmp (lpMemLft, lpMemRht);

    // We no longer need these ptrs
    MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    MyGlobalUnlock (hGlbLft); lpMemLft = NULL;

    return iCmp;
} // End CmpHGLOBAL


//***************************************************************************
//  $aplcmp
//
//  Compare two wide ASCIIZ strings the APL way
//  Return 1 if s1 > s2, -1 if s1 < s2, and 0 otherwise.
//  Essentially, return (s1 - s2).
//***************************************************************************

int aplcmp (const LPWCHAR s1, const LPWCHAR s2)
{
// Define the translate tables (TTs)
static BYTE aTT0[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

static BYTE aTT1[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

static BYTE aTT2[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  0,  0,  0,  0,  0,  0,
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,  0,  0,  0,  0,  0,
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

    // Define array of pointers to the three TTs
    static BYTE (*paTT[])[256] = {&aTT0, &aTT1, &aTT2};

    UINT len1 = lstrlenW (s1),
         len2 = lstrlenW (s2);

    int MinLen, MaxLen, i, j;
    BOOL bRet, bSame;
    BYTE t32, t1j, t2j;
    BYTE (*pTT)[256];

    MinLen = min (len1, len2);
    MaxLen = max (len1, len2);

    // <bRet> is true or false depending upon
    //   whether or not s1 < s2.
    bRet = (len1 <= len2);      // Shorter before longer
    bSame = (len1 == len2);     // If you prefer to consider
                                //   "foo" the same as "foo   ",
                                //   change this to Same = 1;
    // Loop through the array of translate tables
    //   where 3 = sizeof (*tt)
    for (i = 0; i < 3; i++)
    {
        pTT = paTT[i];          // Ptr to current TT
        t32 = (*pTT)[32];       // Lookup space in current TT

        // A short string is considered padded at
        //   the end with blanks.
        // If the lengths differ, toggle the <bRet> state
        //   once if there is a non-blank character
        //   in the portion longer than the shorter
        //   string.
        if (len1 < len2)
        {
            for (j = MaxLen - 1; j >= MinLen; j--)
            {
                t2j = (*pTT)[(BYTE) s2[j]];
                bSame &= (t32 == t2j);

                if (t32 != t2j
                 && bRet == (t32 > t2j))
                {
                    bRet = !bRet;
                    break;
                } // End IF
            } // End FOR
        } else
        if (len1 > len2)
        {
            for (j = MaxLen - 1; j >= MinLen; j--)
            {
                t1j = (*pTT)[(BYTE) s1[j]];
                bSame &= (t32 == t1j);

                if (t1j != t32
                 && bRet == (t1j > t32))
                {
                    bRet = !bRet;
                    break;
                } // End IF
            } // End FOR
        } // End IF/ELSE

        // Loop through the common part of the strings
        //   comparing the characters through the
        //   current translate table.
        for (j = MinLen - 1; j >= 0; j--)
        {
            t1j = (*pTT)[(BYTE) s1[j]];
            t2j = (*pTT)[(BYTE) s2[j]];
            bSame &= (t1j == t2j);

            if (t1j != t2j
             && bRet == (t1j > t2j))
            {
                bRet = !bRet;
            } // End IF
        } // End FOR
    } // End FOR

    // If the strings are the same, return 0, else
    //   return the <bRet> state as -1 or 1.
    return bSame ? 0 : (1 - 2 * bRet);
} // End aplcmp


//***************************************************************************
//  End of File: sort.c
//***************************************************************************
