//***************************************************************************
//  NARS2000 -- System Command:  )COPY
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
//  $CmdCopy_EM
//
//  Execute the system command:  )COPY wsid [obj1 [obj2 ...]]
//***************************************************************************

BOOL CmdCopy_EM
    (LPWCHAR lpwszTail)         // Ptr to command line tail

{
    AppendLine (L"NONCE COMMAND", FALSE, TRUE);





    return FALSE;
} // End CmdCopy_EM


//***************************************************************************
//  End of File: sc_copy.c
//***************************************************************************
