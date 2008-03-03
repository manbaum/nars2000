//***************************************************************************
//  NARS2000 -- System Command:  )LOAD
//***************************************************************************

#define STRICT
#include <windows.h>
#include <stdio.h>
#include <time.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"
#include "aplerrors.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $CmdLoad_EM
//
//  Execute the system command:  )LOAD wsid
//***************************************************************************

BOOL CmdLoad_EM
    (LPWCHAR lpwszTail)         // Ptr to command line tail

{


    // If there's no WSID, that's an error
    if (lpwszTail[0] EQ L'\0')
    {
        IncorrectCommand ();

        return FALSE;
    } // End IF






    AppendLine (L"NONCE COMMAND", FALSE, TRUE);






    return FALSE;
} // End CmdLoad_EM


//***************************************************************************
//  End of File: sc_load.c
//***************************************************************************
