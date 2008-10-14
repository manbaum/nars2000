//***************************************************************************
//  NARS2000 -- System Command:  )ERASE
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
#include <stdio.h>
#include <time.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"
#include "aplerrors.h"
#include "pertab.h"
#include "fh_parse.h"
#include "sis.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $CmdErase_EM
//
//  Execute the system command:  )ERASE var[ var]
//***************************************************************************

UBOOL CmdErase_EM
    (LPWCHAR lpwszTail)             // Ptr to command line tail

{
    APLUINT      uRht;              // Loop counter
////LPAPLCHAR    lpMemDataRht,      // Ptr to right arg char data
    LPAPLCHAR    lpMemDataStart;    // Ptr to start of identifier
    LPSYMENTRY   lpSymEntry;        // Ptr to STE
    STFLAGS      stFlags;           // STE flags
    UBOOL        bNotErasedHeader = FALSE; // TRUE iff the NotErased header
                                    //   has been displayed
    int          iNotErasedWidth = 0; // Width (so far) of NotErased line

    // Loop through the right arg looking for identifiers
    uRht = 0;
    while (TRUE)
    {
        // Skip over white space
        while (lpwszTail[uRht] EQ L' ')
            uRht++;
        if (lpwszTail[uRht])
        {
            // Save the starting ptr
            lpMemDataStart = &lpwszTail[uRht];

            // Skip over black space
            while (lpwszTail[uRht] && lpwszTail[uRht] NE L' ')
                uRht++;
            // Lookup the name in the symbol table
            // SymTabLookupName sets the .ObjName enum,
            //   and the .Inuse flag
            ZeroMemory (&stFlags, sizeof (stFlags));
            lpSymEntry = SymTabLookupNameLength (lpMemDataStart,
                                                (UINT) (&lpwszTail[uRht] - lpMemDataStart),
                                                &stFlags);
            // If found, attempt to expunge the name
            // If not found, ignore it if it's a valid name
            // If not expunged, display the name after the NotErased header
            if (lpSymEntry)
            {
                // Attempt to expunge the global value
                if (!ExpungeName (GetGlobalSTE (lpSymEntry)))
                    ExpungeError (lpSymEntry, &bNotErasedHeader, &iNotErasedWidth);
            } else
            {
                if (!IsValidName (lpMemDataStart,
                                 (UINT) (&lpwszTail[uRht] - lpMemDataStart)))
                    ExpungeError (lpSymEntry, &bNotErasedHeader, &iNotErasedWidth);
            } // End IF/ELSE
        } else
            break;
    } // End WHILE

    // If we displayed any unerased names, end the line
    if (bNotErasedHeader)
        AppendLine (L"", TRUE, TRUE);
    return TRUE;
} // End CmdErase_EM


//***************************************************************************
//  ExpungeError
//
//  Display the NotErased header followed by the STE name
//    wrapping at the page width
//***************************************************************************

void ExpungeError
    (LPSYMENTRY lpSymEntry,         // Ptr to the STE
     LPUBOOL    lpbNotErasedHeader, // TRUE iff the NotErased header has been displayed
     LPINT      lpiNotErasedWidth)  // Current width of the NotErased line

{
    HGLOBAL   htGlbName;            // STE name global memory handle
    LPAPLCHAR lpwGlbName;           // Ptr to STE name
    int       iLen;                 // Length of the STE name
    APLUINT   uQuadPW;              // []PW

    // Get the current value of []PW
    uQuadPW = GetQuadPW ();

    // If the header has not been displayed as yet, do so now
    if (!lpbNotErasedHeader)
    {
        AppendLine (ERRMSG_NOT_ERASED, FALSE, FALSE);
        *lpbNotErasedHeader = TRUE;
        *lpiNotErasedWidth = (sizeof (ERRMSG_NOT_ERASED) / sizeof (APLCHAR)) - 1;
    } // End IF

    // Get the STE name global memory handle
    htGlbName = lpSymEntry->stHshEntry->htGlbName;

    // Lock the memory to get a ptr to it
    lpwGlbName = MyGlobalLock (htGlbName);

    // Get the length of the name
    iLen = lstrlenW (lpwGlbName);

    if ((*lpiNotErasedWidth + 2 + iLen) > uQuadPW)
    {
        // End the line
        AppendLine (L"", TRUE, TRUE);
        *lpiNotErasedWidth = 0;
    } // End IF

    // Output leading blanks if not at line start
    if (*lpiNotErasedWidth)
    {
        AppendLine (L"  ", TRUE, FALSE);
        *lpiNotErasedWidth += 2;
    } // End IF

    // Output the STE name
    AppendLine (lpwGlbName, TRUE, FALSE);
    *lpiNotErasedWidth += iLen;

    // We no longer need this ptr
    MyGlobalUnlock (htGlbName); lpwGlbName = NULL;
} // End ExpungeError


//***************************************************************************
//  GetGlobalSTE
//
//  Return a ptr to the global STE for a given STE
//***************************************************************************

LPSYMENTRY GetGlobalSTE
    (LPSYMENTRY lpSymEntry)         // Ptr to the STE

{
    // Loop through the shadow chain of previous STEs
    while (lpSymEntry->stPrvEntry)
        lpSymEntry = lpSymEntry->stPrvEntry;
    return lpSymEntry;
} // End GetGlobalSTE

//***************************************************************************
//  End of File: sc_erase.c
//***************************************************************************
