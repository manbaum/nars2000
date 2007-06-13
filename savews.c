//***************************************************************************
//  NARS2000 -- Save Workspace
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif



//***************************************************************************
//  $CmdSaveWS_EM
//
//  Execute the system command:  )SAVE
//***************************************************************************

BOOL CmdSaveWS_EM
    (LPWCHAR lpwszTail)     // Command line tail with NO leading blanks

{
    DbgBrk ();          // ***FINISHME***

    // If there is a specified WS Name,
    //   and the current []WSID is different from the specified WS Name,
    //   and a WS with the specified WS Name already exists,
    //   display an error
    //     "NOT SAVED, THIS WS IS ",[]WSID
    if (*lpwszTail)
    {
        // Change to the root directory
        _wchdir ();

        // Split the path into its component parts




    } // End IF

    // The format of a saved ws is as an .ini file
    //   (named *.ws.nars) with the following sections:
    //   [General]    (General settings)
    //   [Fns.0]      (Global functions)
    //   [Fns.1]      (Functions at level 1 in the SI stack)
    //   [Fns.nnn]    (Functions at level nnn in the SI stack)
    //   [Vars.0]     (Global variables)
    //   [Vars.1]     (Variables at level 1 in the SI stack)
    //   [Vars.nnn]   (Variables at level nnn in the SI stack)
    // Each of the above sections has content of
    //   1          = Name      (1st name)
    //   2          = Name      (2nd name)
    //   xxx        = Name      (xxxth name)
    // Corresponding to each of the above names, there is a
    //   separate section [nnn.Name], for example,
    //   [0.Name]     (The contents of the name "Name" at level 0)





    return FALSE;
} // End CmdSaveWS_EM


//***************************************************************************
//  End of File: savews.c
//***************************************************************************
