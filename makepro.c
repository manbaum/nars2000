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

#include "pcre.h"

#define EQ ==
#define NE !=
typedef char *LPCHAR;
#define Assert(a)     ((a) || (DbgBrk (), DbgBrk (), 0))
void DbgBrk (void);

// Define UBOOL as an unsigned int
typedef unsigned int UBOOL, *LPUBOOL;


//****************************************************************************
//  $IsWhite
//
//  Determine if a char is white space
//****************************************************************************

UBOOL IsWhite
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
//  CheckAnachronism
//
//  Check on anachronisms (as the compiler so delicately calls them)
//***************************************************************************

void CheckAnachronism
    (LPCHAR lpHaystack, LPCHAR lpNeedle, LPCHAR lpReplace)

{
    UINT uReplaceLen = lstrlen (lpNeedle);

    while (lpHaystack = strstr (lpHaystack, lpNeedle))
        CopyMemory (lpHaystack, lpReplace, uReplaceLen);
} // End CheckAnachronism


//***************************************************************************
//  DeleteText
//
//  Delete (fill with blanks) given text
//***************************************************************************

void DeleteText
    (LPCHAR lpHaystack, LPCHAR lpNeedle)

{
    UINT uReplaceLen = lstrlen (lpNeedle);

    while (lpHaystack = strstr (lpHaystack, lpNeedle))
        FillMemory (lpHaystack, uReplaceLen, ' ');
} // End DeleteText


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
                        lpSrcFile,
                        lpTmpFile,
                        lpEndLine;
    SECURITY_ATTRIBUTES secAttr = {sizeof (SECURITY_ATTRIBUTES), NULL, TRUE};
    HANDLE              hOutFile = NULL,
                        hSrcFile = NULL,
                        hTmpFile = NULL,
                        hTmpMap = NULL,
                        hOutMap = NULL,
                        hTmpView = NULL,
                        hOutView = NULL;
    STARTUPINFO         siBlk;
    PROCESS_INFORMATION piBlk;
    DWORD               dwTmpFileSize,
                        dwOutFileSize,
                        dwTempPathLen;
    char                szTempPath[1024];
    UBOOL               bFilesEqual = FALSE;
    FILETIME            CreationTime,
                        LastAccessTime,
                        LastWriteTime;

    // Parse the command line:  {D:\path\to\compiler\cl.exe} {switches} {D:\path\to\sourcefile.ext} {D:\path\to\outputfile.ext}

    lpCompiler = lpCmdLine;

    // Skip over white space
    lpCompiler = SkipWhite (lpCompiler);

    Assert (*lpCompiler EQ '{'); lpCompiler++;
    lpSwitches = strchr (lpCompiler, '}');
    *lpSwitches++ = '\0';

    // Skip over white space
    lpSwitches = SkipWhite (lpSwitches);

    Assert (*lpSwitches EQ '{'); lpSwitches++;
    lpSrcFile = strchr (lpSwitches, '}');
    *lpSrcFile++ = ' ';

    // Skip over white space
    lpSrcFile = SkipWhite (lpSrcFile);

    Assert (*lpSrcFile EQ '{'); *lpSrcFile++ = ' ';
    lpOutFile = strchr (lpSrcFile, '}');
    *lpOutFile++ = '\0';

    // Skip over white space
    lpOutFile = SkipWhite (lpOutFile);

    Assert (*lpOutFile EQ '{'); lpOutFile++;
    lpEndLine = strchr (lpOutFile, '}');
    *lpEndLine++ = '\0';

#if 0
    {
        char szTemp[1024];

        wsprintf (szTemp,
                  "Compiler = <%s>.",
                  lpCompiler);
        MessageBox (NULL, szTemp, "makepro", MB_OK);

        wsprintf (szTemp,
                  "Switches = <%s>.",
                  lpSwitches);
        MessageBox (NULL, szTemp, "makepro", MB_OK);

        wsprintf (szTemp,
                  "SrcFile = <%s>.",
                  lpSrcFile);
        MessageBox (NULL, szTemp, "makepro", MB_OK);

        wsprintf (szTemp,
                  "OutFile = <%s>.",
                  lpOutFile);
        MessageBox (NULL, szTemp, "makepro", MB_OK);
    }
#endif

    // Get a path to temporary files
    dwTempPathLen =
      GetTempPath (sizeof (szTempPath), szTempPath);

    lpTmpFile = &szTempPath[dwTempPathLen + 1];

    // Get a temporary filename
    GetTempFileName (szTempPath, "PRO", 0, lpTmpFile);

    // Create the handle we need for the STARTUPINFO block
    hTmpFile =
      CreateFile (lpTmpFile,                    // Pointer to name of the file
                  GENERIC_READ                  // Access (read-write) mode
                | GENERIC_WRITE,
                  FILE_SHARE_READ
                | FILE_SHARE_WRITE,             // Share mode
                 &secAttr,                      // Pointer to security descriptor
                  CREATE_ALWAYS,                // How to create
                  0                             // File attributes
                | FILE_ATTRIBUTE_TEMPORARY
                | FILE_FLAG_WRITE_THROUGH,
                  NULL);                        // Handle to file with attributes to copy
    if (hTmpFile EQ NULL)
    {
        MessageBox (NULL, "Unable to create hTmpFile", "makepro", MB_OK | MB_ICONEXCLAMATION);
        DbgBrk ();
        GetLastError ();
    } // End IF

    // Set the STARTUPINFO block
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
    siBlk.hStdOutput    = hTmpFile;
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
    dwTmpFileSize = GetFileSize (hTmpFile, NULL);
    if (dwTmpFileSize EQ 0)
    {
        hOutFile =
          CreateFile (lpOutFile,                    // Pointer to name of the file
                      GENERIC_READ                  // Access (read-write) mode
                    | GENERIC_WRITE,
                      FILE_SHARE_READ
                    | FILE_SHARE_WRITE,             // Share mode
                     &secAttr,                      // Pointer to security descriptor
                      OPEN_ALWAYS,                  // How to create
                      0                             // File attributes
                    | FILE_ATTRIBUTE_NORMAL
                    | FILE_FLAG_WRITE_THROUGH,
                      NULL);                        // Handle to file with attributes to copy
        if (hOutFile EQ NULL)
        {
            MessageBox (NULL, "Unable to create hOutFile", "makepro", MB_OK | MB_ICONEXCLAMATION);
            DbgBrk ();
            GetLastError ();
        } // End IF

        goto NORMAL_EXIT;
    } // End IF

    // Now launder the output file to remove anomalies of some MS compilers

    // In particular, version 12 of Microsoft Visual Studio mishandles variable arguments
    //   and inserts an extra "..." into the prototype.

    hTmpMap =
      CreateFileMapping (hTmpFile,              // Handle to file to map
                         NULL,                  // Optional security attributes
                         PAGE_READWRITE,        // Protection for mapping object
                         0,                     // High-order 32 bits of object size
                         dwTmpFileSize,         // Low-order 32 bits of object size
                         NULL);                 // Name of file-mapping object
    if (hTmpMap EQ NULL)
    {
        MessageBox (NULL, "Unable to create hTmpMap", "makepro", MB_OK | MB_ICONEXCLAMATION);
        DbgBrk ();
        GetLastError ();
    } // End IF

    hTmpView =
      MapViewOfFile (hTmpMap,                   // File-mapping object to map into address space
                     FILE_MAP_WRITE,            // Access mode
                     0,                         // High-order 32 bits of file offset
                     0,                         // Low-order 32 bits of file offset
                     dwTmpFileSize);            // Number of bytes to map
    if (hTmpView EQ NULL)
    {
        MessageBox (NULL, "Unable to create hTmpView", "makepro", MB_OK | MB_ICONEXCLAMATION);
        DbgBrk ();
        GetLastError ();
    } // End IF

    // Check on unacceptable constructions
    // These constructions, although generated by the compiler as output from /Zg,
    //   are then rejected by it on input.  Go figure.
    CheckAnachronism ((LPCHAR) hTmpView, "... ...",
                                         "...    ");
    CheckAnachronism ((LPCHAR) hTmpView, "__cdecl *__cdecl",
                                         "__cdecl *       ");
    CheckAnachronism ((LPCHAR) hTmpView, "__stdcall *__stdcall",
                                         "__stdcall *         ");
    // Delete some text
    // These statements, although generated by the compiler as output from /Zg,
    //   are then rejected by it on input.  Go figure.
    DeleteText ((LPCHAR) hTmpView, "__forceinline extern void *__cdecl PtrToPtr64(const void *p);");
    DeleteText ((LPCHAR) hTmpView,               "extern void *__cdecl PtrToPtr64(const void *p);");
    DeleteText ((LPCHAR) hTmpView, "__forceinline extern void *__cdecl Ptr64ToPtr(const void *p);");
    DeleteText ((LPCHAR) hTmpView,               "extern void *__cdecl Ptr64ToPtr(const void *p);");
    DeleteText ((LPCHAR) hTmpView, "__forceinline extern void *__cdecl HandleToHandle64(const void *h);");
    DeleteText ((LPCHAR) hTmpView,               "extern void *__cdecl HandleToHandle64(const void *h);");
    DeleteText ((LPCHAR) hTmpView, "__forceinline extern void *__cdecl Handle64ToHandle(const void *h);");
    DeleteText ((LPCHAR) hTmpView,               "extern void *__cdecl Handle64ToHandle(const void *h);");

    // Launder the file to remove extra spaces, CR/LFs, and variable names












    // Now check this file against the previous (if any) output file

    // See if there is a previous copy of the file by attempting to open it
    hOutFile =
      CreateFile (lpOutFile,                    // Pointer to name of the file
                  GENERIC_READ                  // Access (read-write) mode
                | GENERIC_WRITE,
                  FILE_SHARE_READ
                | FILE_SHARE_WRITE,             // Share mode
                 &secAttr,                      // Pointer to security descriptor
                  OPEN_ALWAYS,                  // How to create
                  0                             // File attributes
                | FILE_ATTRIBUTE_NORMAL
                | FILE_FLAG_WRITE_THROUGH,
                  NULL);                        // Handle to file with attributes to copy
    if (hOutFile EQ NULL)
    {
        MessageBox (NULL, "Unable to create hOutFile", "makepro", MB_OK | MB_ICONEXCLAMATION);
        DbgBrk ();
        GetLastError ();
    } // End IF

    // Get the file size (it always fits in a DWORD)
    dwOutFileSize = GetFileSize (hOutFile, NULL);

    // Compare the two file sizes
    if (dwTmpFileSize EQ dwOutFileSize)
    {
        hOutMap =
          CreateFileMapping (hOutFile,              // Handle to file to map
                             NULL,                  // Optional security attributes
                             PAGE_READWRITE,        // Protection for mapping object
                             0,                     // High-order 32 bits of object size
                             dwOutFileSize,         // Low-order 32 bits of object size
                             NULL);                 // Name of file-mapping object
        if (hOutMap EQ NULL)
        {
            MessageBox (NULL, "Unable to create hOutMap #1", "makepro", MB_OK | MB_ICONEXCLAMATION);
            DbgBrk ();
            GetLastError ();
        } // End IF

        hOutView =
          MapViewOfFile (hOutMap,                   // File-mapping object to map into address space
                         FILE_MAP_WRITE,            // Access mode
                         0,                         // High-order 32 bits of file offset
                         0,                         // Low-order 32 bits of file offset
                         dwOutFileSize);            // Number of bytes to map
        if (hOutView EQ NULL)
        {
            MessageBox (NULL, "Unable to create hOutView #1", "makepro", MB_OK | MB_ICONEXCLAMATION);
            DbgBrk ();
            GetLastError ();
        } // End IF

        // Compare the contents
        bFilesEqual =
          memcmp (hTmpView, hOutView, dwOutFileSize) EQ 0;

        // We no longer need this resource
        UnmapViewOfFile (hOutView); hOutView = NULL;

        // We no longer need this resource
        CloseHandle (hOutMap); hOutMap = NULL;
    } // End IF

    // If the files are equal, ...
    if (bFilesEqual)
    {
        // Open the source file (read only)
        hSrcFile =
          CreateFile (lpSrcFile,                    // Pointer to name of the file
                      GENERIC_READ                  // Access (read-write) mode
                    | GENERIC_WRITE,                // Needed for SetFileTime
                      FILE_SHARE_READ
                    | FILE_SHARE_WRITE,             // Share mode
                     &secAttr,                      // Pointer to security descriptor
                      OPEN_EXISTING,                // How to create
                      0                             // File attributes
                    | FILE_ATTRIBUTE_NORMAL
                    | FILE_FLAG_WRITE_THROUGH,
                      NULL);                        // Handle to file with attributes to copy
        if (hSrcFile EQ NULL)
        {
            MessageBox (NULL, "Unable to create hSrcFile", "makepro", MB_OK | MB_ICONEXCLAMATION);
            DbgBrk ();
            GetLastError ();
        } // End IF

        // Copy the file time of the source file (.c) to the output file (.pro)
        //   so we don't run this program again
        GetFileTime (hSrcFile, &CreationTime, &LastAccessTime, &LastWriteTime);
////    SetFileTime (hOutFile, &CreationTime, &LastAccessTime, &LastWriteTime);

        // We no longer need this resource
        CloseHandle (hSrcFile); hSrcFile = NULL;

        // We no longer need this resource
        CloseHandle (hOutFile); hOutFile = NULL;
    } else
    {
        // We no longer need this resource
        CloseHandle (hOutFile); hOutFile = NULL;

        // Create the file as empty
        hOutFile =
          CreateFile (lpOutFile,                    // Pointer to name of the file
                      GENERIC_READ                  // Access (read-write) mode
                    | GENERIC_WRITE,
                      FILE_SHARE_READ
                    | FILE_SHARE_WRITE,             // Share mode
                     &secAttr,                      // Pointer to security descriptor
                      CREATE_ALWAYS,                // How to create
                      0                             // File attributes
                    | FILE_ATTRIBUTE_NORMAL
                    | FILE_FLAG_WRITE_THROUGH,
                      NULL);                        // Handle to file with attributes to copy
        if (hOutFile EQ NULL)
        {
            MessageBox (NULL, "Unable to create hOutFile", "makepro", MB_OK | MB_ICONEXCLAMATION);
            DbgBrk ();
            GetLastError ();
        } // End IF

        // Use the tmp file size
        dwOutFileSize = dwTmpFileSize;

        // Map the file
        hOutMap =
          CreateFileMapping (hOutFile,              // Handle to file to map
                             NULL,                  // Optional security attributes
                             PAGE_READWRITE,        // Protection for mapping object
                             0,                     // High-order 32 bits of object size
                             dwOutFileSize,         // Low-order 32 bits of object size
                             NULL);                 // Name of file-mapping object
        if (hOutMap EQ NULL)
        {
            MessageBox (NULL, "Unable to create hOutMap #2", "makepro", MB_OK | MB_ICONEXCLAMATION);
            DbgBrk ();
            GetLastError ();
        } // End IF

        // View the file
        hOutView =
          MapViewOfFile (hOutMap,                   // File-mapping object to map into address space
                         FILE_MAP_WRITE,            // Access mode
                         0,                         // High-order 32 bits of file offset
                         0,                         // Low-order 32 bits of file offset
                         dwOutFileSize);            // Number of bytes to map
        if (hOutView EQ NULL)
        {
            MessageBox (NULL, "Unable to create hOutView #2", "makepro", MB_OK | MB_ICONEXCLAMATION);
            DbgBrk ();
            GetLastError ();
        } // End IF

        // Copy the tmp file to the out file
        CopyMemory (hOutView, hTmpView, dwTmpFileSize);

        // We no longer need this resource
        UnmapViewOfFile (hOutView); hOutView = NULL;

        // We no longer need this resource
        CloseHandle (hOutMap); hOutMap = NULL;

        // We no longer need this resource
        CloseHandle (hOutFile); hOutFile = NULL;
    } // End IF
NORMAL_EXIT:
    if (hTmpView)
    {
        // We no longer need this resource
        UnmapViewOfFile (hTmpView); hTmpView = NULL;
    } // End IF

    if (hTmpMap)
    {
        // We no longer need this resource
        CloseHandle (hTmpMap); hTmpMap = NULL;
    } // End IF

    if (hOutFile)
    {
        // We no longer need this resource
        CloseHandle (hOutFile); hOutFile = NULL;
    } // End IF

    if (hTmpFile)
    {
        // We no longer need this resource
        CloseHandle (hTmpFile); hTmpFile = NULL;

        // We no longer need this resource
        DeleteFile (lpTmpFile);
    } // End IF

    return 0;
} // End WinMain


//***************************************************************************
//  End of File: makepro.c
//***************************************************************************
