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
#include "headers.h"


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
    size_t       uSymTabMax      ,  // Maximum # HTEs
                 uSymTabTotal    ,  // Size of SymTab
                 uSymTabInuse = 0,  // # STEs in use
                 uHshTabMax      ,  // Maximum # STEs
                 uHshTabTotal    ,  // Size of HshTab
                 uHshTabInuse = 0;  // # HTEs in use
    WCHAR        wszTemp[1024];     // Output save area

    // If there's a command tail, ...
    if (lpwszTail[0] NE WC_EOS)
        return CmdSymbSet_EM (lpwszTail);

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get a ptr to the HshTab & SymTab
    lphtsPTD = lpMemPTD->lphtsPTD;
    lpHshTab = lphtsPTD->lpHshTab;

    // Save current values
    uHshTabTotal = lphtsPTD->iHshTabTotalNelm;
    uSymTabTotal = lphtsPTD->iSymTabTotalNelm;

    // Set the ending ptr
    lpHshNext = &lpHshTab[uHshTabTotal];

    // Loop through the HTEs
    for (lpHshTab = lpHshTab;
         lpHshTab < lpHshNext;
         lpHshTab++)
    {
        // Count it into the total
        uHshTabTotal++;

        // If it's in use, ...
        if (lpHshTab->htFlags.Inuse)
            uHshTabInuse++;
    } // End FOR

    // Loop through the STEs
    for (lpSymTab = lphtsPTD->lpSymTab;
         lpSymTab < lphtsPTD->lpSymTabNext;
         lpSymTab++)
    {
        // Count it into the total
        uSymTabTotal++;

        // If it's in use, ...
        if (lpSymTab->stFlags.Inuse)
            uSymTabInuse++;
    } // End FOR

    // If the Hsh & Sym tabs are from global (not virtual) memory, ...
    if (lphtsPTD->bGlbHshSymTabs)
        // Mark as unlimited
        uHshTabTotal =
        uSymTabTotal = -1;
    else
    {
        // Use the larger
        uSymTabMax = max (gSymTabSize, uSymTabTotal);
        uHshTabMax = max (gHshTabSize, uHshTabTotal);
    } // End IF/ELSE

#define X   WS_UTF16_TIMES
    // Format the output line
    MySprintfW (wszTemp,
                sizeof (wszTemp),
               L"             Maximum         # " X L" Units   Current    In Use  Command Line Switch=Default\r\n"
               L"LclSymTab: %9d (%8d " X L" %4u) %9d %9d  -LclSymTabSize=%d\r\n"
               L"LclHshTab: %9d (%8d " X L" %4u) %9d %9d  -LclHshTabSize=%d\r\n"
               L"AFOSymTab: %9d (%8d " X L" %4u)                      -AFOSymTabSize=%d\r\n"
               L"AFOHshTab: %9d (%8d " X L" %4u)                      -AFOHshTabSize=%d\r\n"
               L"MFOSymTab: %9d (%8d " X L" %4u)                      -MFOSymTabSize=%d\r\n"
               L"MFOHshTab: %9d (%8d " X L" %4u)                      -MFOHshTabSize=%d\r\n"
               L"GLBSymTab: %9d (%8d " X L" %4u)                      -GLBSymTabSize=%d\r\n"
               L"GLBHshTab: %9d (%8d " X L" %4u)                      -GLBHshTabSize=%d",
#undef  X
                uSymTabMax                                  ,
                uSymTabMax  / LCL_SYMTABSIZE_MUL            ,
                LCL_SYMTABSIZE_MUL                          ,
                uSymTabTotal                                ,
                uSymTabInuse                                ,
                DEF_SYMTAB_MAXNELM / LCL_SYMTABSIZE_MUL     ,

                uHshTabMax                                  ,
                uHshTabMax  / LCL_HSHTABSIZE_MUL            ,
                LCL_HSHTABSIZE_MUL                          ,
                uHshTabTotal                                ,
                uHshTabInuse                                ,
                DEF_HSHTAB_MAXNELM / LCL_HSHTABSIZE_MUL     ,

                gAFOSymTabSize                              ,
                gAFOSymTabSize / AFO_SYMTABSIZE_MUL         ,
                AFO_SYMTABSIZE_MUL                          ,
                DEF_AFO_SYMTAB_MAXNELM / AFO_SYMTABSIZE_MUL ,

                gAFOHshTabSize                              ,
                gAFOHshTabSize / AFO_HSHTABSIZE_MUL         ,
                AFO_HSHTABSIZE_MUL                          ,
                DEF_AFO_HSHTAB_MAXNELM / AFO_HSHTABSIZE_MUL ,

                gMFOSymTabSize                              ,
                gMFOSymTabSize / MFO_SYMTABSIZE_MUL         ,
                MFO_SYMTABSIZE_MUL                          ,
                DEF_MFO_SYMTAB_MAXNELM / MFO_SYMTABSIZE_MUL ,

                gMFOHshTabSize                              ,
                gMFOHshTabSize / MFO_HSHTABSIZE_MUL         ,
                MFO_HSHTABSIZE_MUL                          ,
                DEF_MFO_HSHTAB_MAXNELM / MFO_HSHTABSIZE_MUL ,

                gGLBSymTabSize                              ,
                gGLBSymTabSize / MFO_SYMTABSIZE_MUL         ,
                LCL_SYMTABSIZE_MUL                          ,
                DEF_GLBSYMTAB_MAXNELM / LCL_SYMTABSIZE_MUL  ,

                gGLBHshTabSize                              ,
                gGLBHshTabSize / LCL_HSHTABSIZE_MUL         ,
                LCL_HSHTABSIZE_MUL                          ,
                DEF_GLBHSHTAB_MAXNELM / LCL_HSHTABSIZE_MUL);
    // Display the line
    AppendLine (wszTemp, FALSE, TRUE);

    return TRUE;
} // End CmdSymb_EM


//***************************************************************************
//  $CmdSymbSet_EM
//
//  Set a new limit for the SymTab and HshTabs
//
//  The line should be of the form
//
//  )SYMB sss hhh
//
//  where sss is the new maximum size of SymTab
//  and   hhh is the new maximum size of HshTab
//***************************************************************************

UBOOL CmdSymbSet_EM
    (LPWCHAR lpwszTail)             // Ptr to command line tail

{
    // Tell 'em it's not implemented as yet
    AppendLine (ERRMSG_NONCE_ERROR, FALSE, TRUE);

    return TRUE;
} // End CmdSymbSet_EM


//***************************************************************************
//  End of File: sc_symb.c
//***************************************************************************
