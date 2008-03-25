//***************************************************************************
//	NARS2000 -- Sort Routines
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
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//	$ShellSort
//
//	A simple Shell sort of ptr objects
//***************************************************************************

void ShellSort
	(LPVOID *lpObj, 	// Ptr to objects to sort
	 UINT	 uSize, 	// # objects to sort
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
//	$CmpLPSYMENTRY
//
//	Compare two LPSYMENTRYs being sorted
//
//	Note that the names may include the characters
//	  {alpha}, {omega}, {delta}, {deltaunderbar}, and {overbar}.
//***************************************************************************

int CmpLPSYMENTRY
	(LPSYMENTRY lpSymLft,
	 LPSYMENTRY lpSymRht)

{
	LPAPLCHAR lpMemLft, 	// Ptr to left arg global memory
			  lpMemRht; 	// Ptr to right ...
	int 	  iCmp; 		// Comparison result

	// Lock the memory to get a ptr to it
	lpMemLft = MyGlobalLock (lpSymLft->stHshEntry->htGlbName);
	lpMemRht = MyGlobalLock (lpSymRht->stHshEntry->htGlbName);

	// Compare the two names
	iCmp = aplcmp (lpMemLft, lpMemRht);

	// We no longer need these ptrs
	MyGlobalUnlock (lpSymRht->stHshEntry->htGlbName); lpMemRht = NULL;
	MyGlobalUnlock (lpSymLft->stHshEntry->htGlbName); lpMemLft = NULL;

	return iCmp;
} // End CmpLPSYMENTRY


//***************************************************************************
//	$aplcmp
//
//	Compare two wide ASCIIZ strings the APL way
//	Return 1 if s1 > s2, -1 if s1 < s2, and 0 otherwise.
//	Essentially, return signum (s1 - s2).
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
	//	 whether or not s1 < s2.
	bRet = (len1 <= len2);		// Shorter before longer
	bSame = (len1 == len2); 	// If you prefer to consider
								//	 "foo" the same as "foo   ",
								//	 change this to bSame = 1;
	// Loop through the array of translate tables
	//	 where 3 = sizeof (*tt)
	for (i = 0; i < 3; i++)
	{
		pTT = paTT[i];			// Ptr to current TT
		t32 = (*pTT)[32];		// Lookup space in current TT

		// A short string is considered padded at
		//	 the end with blanks.
		// If the lengths differ, toggle the <bRet> state
		//	 once if there is a non-blank character
		//	 in the portion longer than the shorter
		//	 string.
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
		//	 comparing the characters through the
		//	 current translate table.
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
	//	 return the <bRet> state as -1 or 1.
	return bSame ? 0 : (1 - 2 * bRet);
} // End aplcmp


//***************************************************************************
//	$GradeUp
//
//	Grade up on a small number of APLUINTs (using the algorithm for GnomeSort)
//***************************************************************************

void GradeUp
	(LPAPLUINT lpSrc,		// Source
	 LPAPLUINT lpDst,		// Destination
	 APLUINT   uCount)		// # APLUINTs in the source

{
	APLUINT u1, u2, u3;

	// Start with {iota}uCount in lpDst
	for (u1 = 0; u1 < uCount; u1++)
		lpDst[u1] = u1;

	// Use Gnome sort on lpDst while comparing lpSrc
	u2 = 1; u3 = 2;
	while (u2 < uCount)
	{
		if (lpSrc[lpDst[u2 - 1]] <= lpSrc[lpDst[u2]])
		{
			u2 = u3;
			u3++;
		} else
		{
			u1			  = lpDst[u2 - 1];
			lpDst[u2 - 1] = lpDst[u2];
			lpDst[u2]	  = u1;

			if (u2 NE 1)
				u2--;
		} // End IF/ELSE
	} // End WHILE
} // End GradeUp


//***************************************************************************
//	$GnomeSort
//
//	Grade up on arbitrary objects using Gnome Sort
//***************************************************************************

void GnomeSort
	(LPAPLUINT lpDst,		// Destination
	 LPVOID    lpSrc,		// Source
	 APLUINT   uCount,		// # APLUINTs in the source
	 APLINT (*ObjCmp) (LPVOID, APLUINT, APLUINT, LPVOID), // Ptr to comparison routine
	 LPVOID    lpExtra) 	// Ptr to extra data (passed to ObjCmp)

{
	APLUINT u1, u2, u3;

	// Use Gnome sort on lpDst while comparing lpSrc
	u2 = 1; u3 = 2;
	while (u2 < uCount)
	{
		if (ObjCmp (lpSrc, lpDst[u2], lpDst[u2 - 1], lpExtra) >= 0)
////////if (lpSrc[lpDst[u2]]	 >= lpSrc[lpDst[u2 - 1]])
////////if (lpSrc[lpDst[u2 - 1]] <= lpSrc[lpDst[u2]])
		{
			u2 = u3;
			u3++;
		} else
		{
			u1			  = lpDst[u2 - 1];
			lpDst[u2 - 1] = lpDst[u2];
			lpDst[u2]	  = u1;

			if (u2 NE 1)
				u2--;
		} // End IF/ELSE
	} // End WHILE
} // End GnomeSort


//***************************************************************************
//	$HeapSort
//
//	Heapsort based on ideas of J.W.Williams/R.W.Floyd/S.Carlsson
//	  taken from http://en.wikipedia.org/wiki/Heapsort
//***************************************************************************

/////// data_i_LESS_THAN_(other)   (data[i] < other)
#define data_i_LESS_THAN_(other)   (ObjCmp (lpSrc, other, lpDst[i], lpExtra) > 0)
#define MOVE_i_TO_free	{lpDst[free] = lpDst[i]; free = i;}

#define APLUINT APLUINT

void sift_in
	(APLUINT   uCount,
	 LPAPLUINT lpDst,
	 LPVOID    lpSrc,
	 APLUINT   free_in,
	 APLUINT   next,
	 APLINT (*ObjCmp) (LPVOID, APLUINT, APLUINT, LPVOID), // Ptr to comparison routine
	 LPVOID    lpExtra)
{
	APLUINT i,
			free = free_in;

	// Sift up the free node
	for (i = 2 * free; i < uCount; i += i)
	{
		if (data_i_LESS_THAN_ (lpDst[i + 1]))
			i++;
		MOVE_i_TO_free
	} // End FOR

	// Special case in sift up if the last inner node has only 1 child
	if (i EQ uCount)
	   MOVE_i_TO_free
	// Sift down the new item next
	while (((i = free / 2) >= free_in)
		&& data_i_LESS_THAN_ (next))
	   MOVE_i_TO_free

	lpDst[free] = next;
} // End sift_in

void HeapSort
	(LPAPLUINT lpDst,		// Destination
	 LPVOID    lpSrc,		// Source
	 APLUINT   uCount,		// # APLUINTs in the source
	 APLINT (*ObjCmp) (LPVOID, APLUINT, APLUINT, LPVOID), // Ptr to comparison routine
	 LPVOID    lpExtra) 	// Ptr to extra data (passed to ObjCmp)
{
	APLUINT j;

	if (uCount <= 1)
		return;
	// Map addresses to indices 1 til count
	lpDst -= 1;

	// Build the heap structure
	for (j = uCount / 2; j >= 1; j--)
	{
	   APLUINT next = lpDst[j];

	   sift_in (uCount, lpDst, lpSrc, j, next, ObjCmp, lpExtra);
	} // End FOR

	// Search next by next remaining extremal element
	for (j = uCount - 1; j >= 1; j--)
	{
	   APLUINT next = lpDst[j + 1];

	   // Extract extremal element from the heap
	   lpDst[j + 1] = lpDst[1];

	   sift_in (j, lpDst, lpSrc, 1, next, ObjCmp, lpExtra);
	} // End FOR
} // End HeapSort


//***************************************************************************
//	End of File: sort.c
//***************************************************************************
