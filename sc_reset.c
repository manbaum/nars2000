//***************************************************************************
//  NARS2000 -- System Command:  )RESET
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
#include "sis.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $CmdReset_EM
//
//  Execute the system command:  )RESET [n]
//***************************************************************************

BOOL CmdReset_EM
    (LPWCHAR lpwszTail)         // Ptr to command line tail

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // ***FIXME*** -- Make sensitive to [n] in lpwszTail

    // If there's an SIS layer, ...
    if (lpMemPTD->lpSISCur)
    {
        // Set the reset flag
        lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_ALL;

        Assert (lpMemPTD->lpSISCur->hSemaphore NE NULL);

        // Signal the next layer to begin resetting
        ReleaseSemaphore (lpMemPTD->lpSISCur->hSemaphore, 1, NULL);
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return TRUE;
} // End CmdReset_EM


//***************************************************************************
//  End of File: sc_reset.c
//***************************************************************************
