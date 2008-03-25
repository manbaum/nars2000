//***************************************************************************
//	NARS2000 -- System Command:  )DROP
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

#include "main.h"
#include "externs.h"
#include "aplerrors.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//	$CmdDrop_EM
//
//	Execute the system command:  )DROP wsid
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- CmdDrop_EM"
#else
#define APPEND_NAME
#endif

BOOL CmdDrop_EM
	(LPWCHAR lpwszTail) 			// Ptr to command line tail

{
	WCHAR wszTailDPFE[_MAX_PATH];	// Save area for canonical form of given ws name
	BOOL  bRet = FALSE; 			// TRUE iff result is valid
	WCHAR wszTemp[19 + 1];			// Output save area

	// If there is a specified WS Name, ...
	if (*lpwszTail)
	{
		// Convert the given workspace name into a canonical form
		MakeWorkspaceNameCanonical (wszTailDPFE, lpwszTail, wszSaveDir);

		// Append the common workspace extension
		lstrcatW (wszTailDPFE, WS_WKSEXT);

		// Attempt to open the workspace
		if (!_wremove (wszTailDPFE))
		{
			// Display the current date & time of the drop
			FormatCurDateTime (wszTemp);

			AppendLine (wszTemp, FALSE, TRUE);

			// Mark as successful
			bRet = TRUE;
		} else
			AppendLine (ERRMSG_WS_NOT_FOUND, FALSE, TRUE);
	} else
		IncorrectCommand ();

	return bRet;
} // End CmdDrop_EM
#undef	APPEND_NAME


//***************************************************************************
//	End of File: sc_drop.c
//***************************************************************************
