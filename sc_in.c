//***************************************************************************
//  NARS2000 -- System Command:  )IN
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

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $CmdIn_EM
//
//  Execute the system command:  )IN filename[.ext]
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CmdIn_EM"
#else
#define APPEND_NAME
#endif

BOOL CmdIn_EM
    (LPWCHAR lpwszTail)         // Ptr to command line tail

{
    WCHAR wszDrive[_MAX_DRIVE],
          wszDir  [_MAX_DIR],
          wszFname[_MAX_FNAME],
          wszExt  [_MAX_EXT];
    SECURITY_ATTRIBUTES secAttr = {sizeof (SECURITY_ATTRIBUTES), NULL, TRUE};
    HANDLE        hAtfFile = NULL,
                  hAtfMap = NULL,
                  hAtfView = NULL;
    DWORD         dwAtfFileSize;
    char          szTemp[1024];
    UINT          uLen;
    BOOL          bRet = FALSE;     // TRUE iff the result is valid

    // Split out the drive and path from the module filename
    _wsplitpath (lpwszTail, wszDrive, wszDir, wszFname, wszExt);

    // If there's no extension, supply one
    if (wszExt[0] EQ L'\0')
        lstrcpyW (wszExt, L".atf");

    // Put it all back together
    _wmakepath  (lpwszTail, wszDrive, wszDir, wszFname, wszExt);

    // See if we can open this file
    hAtfFile =
      CreateFileW (lpwszTail,                   // Pointer to name of the file
                   GENERIC_READ,                // Access (read-write) mode
                   FILE_SHARE_READ
                 | FILE_SHARE_WRITE,            // Share mode
                  &secAttr,                     // Pointer to security descriptor
                   OPEN_EXISTING,               // How to create
                   0                            // File attributes
                 | FILE_ATTRIBUTE_NORMAL,
                   NULL);                       // Handle to file with attributes to copy
    if (hAtfFile EQ NULL)
    {
        ReplaceLastLineCRPmt (ERRMSG_FILE_NOT_FOUND APPEND_NAME);

        goto ERROR_EXIT;
    } // End IF

    // Get the file size (it always fits in a DWORD)
    dwAtfFileSize = GetFileSize (hAtfFile, NULL);

    // Map the file
    hAtfMap =
      CreateFileMapping (hAtfFile,              // Handle to file to map
                         NULL,                  // Optional security attributes
                         PAGE_READONLY,         // Protection for mapping object
                         0,                     // High-order 32 bits of object size
                         dwAtfFileSize,         // Low-order 32 bits of object size
                         NULL);                 // Name of file-mapping object
    if (hAtfMap EQ NULL)
    {
        lstrcpy (szTemp, "Unable to create file mapping:  ");
        uLen = lstrlen (szTemp);

        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                      &szTemp[uLen],                // Pointer to message buffer
                       sizeof (szTemp) - uLen,      // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MB (szTemp);

        goto ERROR_EXIT;
    } // End IF

    // Get a view of the file
    hAtfView =
      MapViewOfFile (hAtfMap,                   // File-mapping object to map into address space
                     FILE_MAP_READ,             // Access mode
                     0,                         // High-order 32 bits of file offset
                     0,                         // Low-order 32 bits of file offset
                     dwAtfFileSize);            // Number of bytes to map
    if (hAtfView EQ NULL)
    {
        lstrcpy (szTemp, "Unable to map view of file:  ");
        uLen = lstrlen (szTemp);

        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                      &szTemp[uLen],                // Pointer to message buffer
                       sizeof (szTemp) - uLen,      // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MB (szTemp);

        goto ERROR_EXIT;
    } // End IF

    // hAtfView points to the file contents
























ERROR_EXIT:
    if (hAtfView)
    {
        CloseHandle (hAtfView); hAtfView = NULL;
    } // End IF

    if (hAtfMap)
    {
        CloseHandle (hAtfMap); hAtfMap = NULL;
    } // End IF

    if (hAtfFile)
    {
        CloseHandle (hAtfFile); hAtfFile = NULL;
    } // End IF

    IncorrectCommand ();

    return bRet;
} // End CmdIn_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: sc_in.c
//***************************************************************************
