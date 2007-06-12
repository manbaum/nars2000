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
    (LPWCHAR lpwszTail)

{
    DbgBrk ();          // ***FINISHME***

    // If there is a specified WS Name,
    //   and the current []WSID is different from the specified WS Name,
    //   and a WS with the specified WS Name already exists,
    //   display an error
    //     "NOT SAVED, THIS WS IS ",[]WSID







    return FALSE;
} // End CmdSaveWS_EM


//***************************************************************************
//  End of File: savews.c
//***************************************************************************
