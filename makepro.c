//***************************************************************************
//  MAKEPRO -- Make a Prototype File
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

#define EQ ==
#define NE !=
typedef char *LPCHAR;
#define Assert(a)     ((a) || (DbgBrk (), DbgBrk (), 0))
void DbgBrk (void);


//****************************************************************************
//  $IsWhite
//
//  Determine if a char is white space
//****************************************************************************

BOOL IsWhite
    (char c)

{
    return (c EQ ' ' || c EQ '\t') ? TRUE : FALSE;
} // End IsWhite


//****************************************************************************
//  $SkipWhite
//
//  Skip over leading white space
//****************************************************************************

LPCHAR SkipWhite
    (LPCHAR lp)

{
    // Skip over white space
    while (IsWhite (*lp))
        lp++;

    return lp;
} // End SkipWhite


//***************************************************************************
//  CheckVarArgFns
//
//  Check on variable argument functions
//  Replace "... ..." strings with "    ..."
//***************************************************************************

void CheckVarArgFns
    (LPCHAR lpFileChar)

{
    while (lpFileChar = strstr (lpFileChar, "... ..."))
        FillMemory (lpFileChar, 3, ' ');
} // End CheckVarArgFns


//***************************************************************************
//  CheckAnachronism
//
//  Check on data modifiers
//  Replace "__cdecl *__cdecl" strings with "        *_cdecl"
//***************************************************************************

void CheckAnachronism
    (LPCHAR lpFileChar)

{
    while (lpFileChar = strstr (lpFileChar, "__cdecl *__cdecl"))
        FillMemory (strchr (lpFileChar, '*') + 1, 7, ' ');
} // End CheckAnachronism


//***************************************************************************
//  WinMain
//
//  Start the process
//***************************************************************************

int PASCAL WinMain
    (HINSTANCE   hInstance,
     HINSTANCE   hPrevInstance,     // Zero under Win32
     LPSTR       lpCmdLine,
     int         nCmdShow)

{
    LPCHAR              lpCompiler,
                        lpSwitches,
                        lpOutFile,
                        lpEndLine;
    SECURITY_ATTRIBUTES secAttr = {sizeof (SECURITY_ATTRIBUTES), NULL, TRUE};
    HANDLE              hOutFile,
                        hMap,
                        hView;
    STARTUPINFO         siBlk;
    PROCESS_INFORMATION piBlk;
    DWORD               dwFileSize;

    // Parse the command line:  {D:\path\to\compiler\cl.exe} {switches} {D:\path\to\outputfile.ext}

    // Skip over white space
    lpCmdLine = SkipWhite (lpCmdLine);

    Assert (*lpCmdLine EQ '{'); lpCmdLine++;

    lpCompiler = lpCmdLine;
    lpSwitches = strchr (lpCompiler, '}');
    *lpSwitches++ = '\0';

    // Skip over white space
    lpSwitches = SkipWhite (lpSwitches);

    Assert (*lpSwitches EQ '{'); lpSwitches++;
    lpOutFile = strchr (lpSwitches, '}');
    *lpOutFile++ = '\0';

    // Skip over white space
    lpOutFile = SkipWhite (lpOutFile);

    Assert (*lpOutFile EQ '{'); lpOutFile++;
    lpEndLine = strchr (lpOutFile, '}');
    *lpEndLine++ = '\0';

    // Create the handles we need for trhe STARTUPINFO block
    hOutFile =
      CreateFile (lpOutFile,                    // Pointer to name of the file
                  GENERIC_READ                  // Access (read-write) mode
                | GENERIC_WRITE,
                  FILE_SHARE_WRITE,             // Share mode
                 &secAttr,                      // Pointer to security descriptor
                  CREATE_ALWAYS,                // How to create
                  0                             // File attributes
                | FILE_ATTRIBUTE_NORMAL
                | FILE_FLAG_WRITE_THROUGH,
                  NULL);                        // Handle to file with attributes to copy
    if (hOutFile EQ NULL)
    {
        DbgBrk ();
        GetLastError ();
    } // End IF

    // Set th STARTUPINFO block
    siBlk.cb            = sizeof (STARTUPINFO);
    siBlk.lpReserved    = NULL;
    siBlk.lpDesktop     = NULL;
    siBlk.lpTitle       = "Protoype File Generation";
    siBlk.dwX           = 0;
    siBlk.dwY           = 0;
    siBlk.dwXSize       = 0;
    siBlk.dwYSize       = 0;
    siBlk.dwXCountChars = 0;
    siBlk.dwYCountChars = 0;
    siBlk.dwFlags       = 0
                        | STARTF_USESTDHANDLES
                        | STARTF_USESHOWWINDOW;
    siBlk.wShowWindow   = SW_HIDE;
    siBlk.cbReserved2   = 0;
    siBlk.lpReserved2   = NULL;
    siBlk.hStdInput     = NULL;
    siBlk.hStdOutput    = hOutFile;
    siBlk.hStdError     = NULL;

    // Create the process to output the prototype
    CreateProcess (lpCompiler,                  // Pointer to name of executable module
                   lpSwitches,                  // Pointer to command line string
                   NULL,                        // Pointer to process security attributes
                   NULL,                        // Pointer to thread security attributes
                   TRUE,                        // Handle inheritance flag
                   0                            // Creationflags
                 | NORMAL_PRIORITY_CLASS,
                   NULL,                        // Pointer to new environment block
                   NULL,                        // Pointer to current directory name
                  &siBlk,                       // Pointer to STARTUPINFO
                  &piBlk);                      // Pointer to PROCESS_INFORMATION
    WaitForSingleObject (piBlk.hProcess, INFINITE);

    // Get the file size (it always fits in a DWORD)
    dwFileSize = GetFileSize (hOutFile, NULL);

    // Now launder the output file to remove anomalies of some MS compilers

    // In particular, version 12 of Microsoft Visual Studio mishandles variable arguments
    //   and inserts an extra "..." into the prototype.

    hMap =
      CreateFileMapping (hOutFile,              // Handle to file to map
                         NULL,                  // Optional security attributes
                         PAGE_READWRITE,        // Protection for mapping object
                         0,                     // High-order 32 bits of object size
                         dwFileSize,            // Low-order 32 bits of object size
                         NULL);                 // Name of file-mapping object
    if (hMap EQ NULL)
    {
        DbgBrk ();
        GetLastError ();
    } // End IF

    hView =
      MapViewOfFile (hMap,                      // File-mapping object to map into address space
                     FILE_MAP_WRITE,            // Access mode
                     0,                         // High-order 32 bits of file offset
                     0,                         // Low-order 32 bits of file offset
                     dwFileSize);               // Number of bytes to map
    if (hView EQ NULL)
    {
        DbgBrk ();
        GetLastError ();
    } // End IF

    // Check on variable argument functions ("... ...")
    CheckVarArgFns ((LPCHAR) hView);

    // Check on data modifiers ("__cdecl *__cdecl")
    CheckAnachronism ((LPCHAR) hView);

    // We no longer need this resource
    UnmapViewOfFile (hView); hView = NULL;

    // We no longer need this resource
    CloseHandle (hMap); hMap = NULL;

    // We no longer need this resource
    CloseHandle (hOutFile); hOutFile = NULL;

    return 0;
} // End WinMain


//***************************************************************************
//  End of File: makepro.c
//***************************************************************************
