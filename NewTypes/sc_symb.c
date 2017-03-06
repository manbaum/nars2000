//***************************************************************************
//  NARS2000 -- System Command:  )SYMB
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2017 Sudley Place Software

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
#include "headers.h"
#include "debug.h"              // For xxx_TEMP_OPEN macros


//***************************************************************************
//  $CmdSymb_EM
//
//  Execute the system command:  )SYMB
//***************************************************************************

UBOOL CmdSymb_EM
    (LPWCHAR lpwszTail)             // Ptr to command line tail

{
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPHSHTABSTR  lphtsPTD;          // Ptr to current HTS
    LPSYMENTRY   lpSymTab;          // ...            STEs
    LPHSHENTRY   lpHshTab,          // ...            HTEs
                 lpHshNext;         // ...     ending HTE
    int          iSymTabMax      ,  // Maximum # HTEs
                 iSymTabTotal    ,  // Size of SymTab
                 iSymTabInuse = 0,  // # STEs in use
                 iHshTabMax      ,  // Maximum # STEs
                 iHshTabTotal    ,  // Size of HshTab
                 iHshTabInuse = 0;  // # HTEs in use
    WCHAR        wszTemp[1024];     // Output save area

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // If there's a command tail, ...
    if (lpwszTail[0] NE WC_EOS)
        return CmdSymbSet (lpwszTail);

    // Get a ptr to the HshTab & SymTab
    lphtsPTD = lpMemPTD->lphtsPTD;
    lpHshTab = lphtsPTD->lpHshTab;

    // Save current values
    iHshTabTotal = lphtsPTD->iHshTabTotalNelm;
    iSymTabTotal = lphtsPTD->iSymTabTotalNelm;

    // Set the ending ptr
    lpHshNext = &lpHshTab[iHshTabTotal];

    // Loop through the HTEs
    for (lpHshTab = lpHshTab;
         lpHshTab < lpHshNext;
         lpHshTab++)
    {
        // Count it into the total
        iHshTabTotal++;

        // If it's in use, ...
        if (lpHshTab->htFlags.Inuse)
            iHshTabInuse++;
    } // End FOR

    // Loop through the STEs
    for (lpSymTab = lphtsPTD->lpSymTab;
         lpSymTab < lphtsPTD->lpSymTabNext;
         lpSymTab++)
    {
        // Count it into the total
        iSymTabTotal++;

        // If it's in use, ...
        if (lpSymTab->stFlags.Inuse)
            iSymTabInuse++;
    } // End FOR

    // If the Hsh & Sym tabs are from global (not virtual) memory, ...
    if (lphtsPTD->bGlbHshSymTabs)
        // Mark as unlimited
        iHshTabTotal =
        iSymTabTotal = -1;
    else
    {
        // Use the larger
        iSymTabMax = max (((int) gSymTabSize), iSymTabTotal);
        iHshTabMax = max (((int) gHshTabSize), iHshTabTotal);
    } // End IF/ELSE

    // Format the output line
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"SymTab:  Max = %8u, Total = %8u, Inuse = %u\r\n"
               L"HshTab:  Max = %8u, Total = %8u, Inuse = %u",
                iSymTabMax  ,
                iSymTabTotal,
                iSymTabInuse,
                iHshTabMax  ,
                iHshTabTotal,
                iHshTabInuse);
    // Display the line
    AppendLine (wszTemp, FALSE, TRUE);

    return TRUE;
} // End CmdSymb_EM


//***************************************************************************
//  $CmdSymbSet
//
//  Set a new limit for the SymTab and HshTabs
//***************************************************************************

UBOOL CmdSymbSet
    (LPWCHAR lpwszTail)             // Ptr to command line tail

{
    // Tell 'em it's not implemented as yet
    AppendLine (ERRMSG_NONCE_ERROR, FALSE, TRUE);

    return TRUE;
} // End CmdSymbSet


//***************************************************************************
//  End of File: sc_symb.c
//***************************************************************************
