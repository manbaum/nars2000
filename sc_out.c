//***************************************************************************
//  NARS2000 -- System Command:  )OUT
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

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $CmdOut_EM
//
//  Execute the system command:  )OUT filename.ext
//***************************************************************************

BOOL CmdOut_EM
    (LPWCHAR lpwszTail)         // Ptr to command line tail

{
    BOOL          bRet = FALSE;     // TRUE iff the result is valid
















    IncorrectCommand ();

    return bRet;
} // End CmdIn_EM


//***************************************************************************
//  End of File: sc_out.c
//***************************************************************************
