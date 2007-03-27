//***************************************************************************
//  LINECNT -- Count the # lines and bytes in files
//***************************************************************************

#define STRICT
#include <stdio.h>
#include <dos.h>

#define EQ ==
#define NE !=

char szHelpMsg[] =
"LINECNT -- Version 1.00 -- Line and byte counter\r\n"
"   (C) Copyright 2007 Qualitas, Inc.  All rights reserved\r\n"
"\r\n"
"Syntax:  linecnt [filespec [filespec] ...]\r\n"
"  where  filespec  is a file specification such as\r\n"
"    *.c or c:\\util\\foo\\bar.c\r\n";

typedef unsigned long DWORD;
typedef unsigned int   WORD;

struct _find_t_lfn
{
    DWORD FileAttrs;
    DWORD FileCreationTime_Hi;
    DWORD FileCreationTime_Lo;
    DWORD LastAccessTime_Hi;
    DWORD LastAccessTime_Lo;
    DWORD LastModificationTime_Hi;
    DWORD LastModificationTime_Lo;
    DWORD FileSize_Hi;
    DWORD FileSize_Lo;
    char  Unused[8];
    char  name[260];
    char  ShortName[14];
    WORD  FileHandle;
};

typedef struct tagACCUM
{
    int  files;     // File count
    long lines,     // File line count
         size;      // File byte count
} ACCUM, *LPACCUM;


//***************************************************************************
//  _dos_findfirst_lfn
//***************************************************************************

unsigned int _dos_findfirst_lfn
    (const    char      *szFile,
     WORD                Attrs,
     struct _find_t_lfn *findstr)

{
    unsigned char uAttr;
    WORD          uRet, uFileHandle;

    uAttr = (char) Attrs;

    _asm
    {
        push    si;
        push    di;

        mov     ax,0x714E;
        mov     cl,uAttr;   // Get attributes to look for
        mov     ch,0;       // No attributes required
        mov     si,1;       // Use MS-DOS date/time
        mov     dx,szFile;  // DS:DX ==> input ASCIIZ LFN
        mov     di,findstr; // ES:DI ==> input structure
        int     0x21;
        mov     uFileHandle,ax;// Save FileHandle
        mov     uRet,0;     // Assume OK
        jnc     OK;         // Jump if OK

        mov     uRet,ax     // Save error code
OK:
        pop     di;
        pop     si;
    }

    findstr->FileHandle = uFileHandle;

    return uRet;
} // End _dos_findfirst_lfn


//***************************************************************************
//  _dos_findnext_lfn
//***************************************************************************

unsigned int _dos_findnext_lfn
    (struct _find_t_lfn *findstr)

{
    WORD uRet;

    uRet = findstr->FileHandle;

    _asm
    {
        push    si;
        push    di;

        mov     ax,0x714F;
        mov     bx,uRet;    // Get FileHandle
        mov     di,findstr; // ES:DI ==> input structure
        mov     si,1;       // Use MS-DOS date/time
        int     0x21;
        mov     uRet,0;     // Assume OK
        jnc     OK;         // Jump if OK

        mov     uRet,ax;    // Save error code
    OK:
        pop     di;
        pop     si;
    }

    return uRet;            // Return code (error) or 0 (OK)
} // End _dos_findnext_lfn


//***************************************************************************
//  _dos_findclose_lfn
//***************************************************************************

void _dos_findclose_lfn
    (struct _find_t_lfn *findstr)

{
    WORD uRet;

    uRet = findstr->FileHandle;

    _asm
    {
        mov     ax,0x71a1;
        mov     bx,uRet;    // Get FileHandle
        int     0x21;
    }
} // End _dos_findclose_lfn


//********************************* fopen_lfn *********************************

FILE *fopen_lfn
    (const char *szFile,
     const char *pFlags)

{
    char szFileShort[256];

    // Translate the long filename to a short filename
    _asm
    {
        push    si;
        push    di;

        mov     ax,0x7160;
        mov     cx,0x8001;
        mov     si,szFile;
        lea     di,szFileShort;
        int     0x21;

        pop     di;
        pop     si;
    }

    return fopen (szFileShort, pFlags);
} // End fopen_lfn


//***************************************************************************
//  number_format
//
//  Format a number and insert commas as necessary
//***************************************************************************

void number_format
    (char *szOutBuf,
     char *szFormat,
     long  lNum)

{
    char szLclBuf[32];
    int  iLclLen, iOutLen, i;

    // Format the number
    sprintf (szLclBuf, szFormat, lNum);

    // Get the length of the local buffer
    iLclLen = strlen (szLclBuf);

    // Calculate the length of the output buffer
    iOutLen = iLclLen + ((iLclLen - 1) / 3);

    // Fill the output with blanks and terminate it
    memset (szOutBuf, ' ', iOutLen);
    szOutBuf[iOutLen] = '\0';

    // Loop through the local buffer
    i = 0;
    while (iLclLen--)
    {
        if (i NE 0
         && szLclBuf[iLclLen] NE ' '
         && (i % 3) EQ 0)
            szOutBuf[--iOutLen] = ',';

        szOutBuf[--iOutLen] = szLclBuf[iLclLen];
        i++;
    } // End FOR
} // End number_format


//***************************************************************************
//  Main
//
//  Start the process
//***************************************************************************

int main
    (int   argc,        // # arguments
     char *argv[],      // [0] = filename, [1] = 1st argument, etc.
     char *envp[])      // [0] = 1st environment var, etc.

{
    FILE  *fp;
    int    i;
    struct _find_t_lfn find = {0};
    ACCUM  Accum[20] = {0};
    char   szLines[32],
           szSize [32];

    // The argument line should look like
    //   *.c *.h *.rc

    if (argc EQ 1)      // Filename only
    {
        fprintf (stderr, szHelpMsg);
        return 1;
    } // End IF

    for (i = 1; i < argc; i++)
    {
        if (!_dos_findfirst_lfn (argv[i], _A_ARCH | _A_RDONLY, &find))
        do
        {
            char szLine[2048];

            // Open the file
            fp = fopen_lfn (find.name, "r");

            // Count in another file
            Accum[i-1].files++;

            // Count in the file size
            fseek (fp, 0, SEEK_END);
            Accum[i-1].size += ftell (fp);

            // Reset to the beginning
            fseek (fp, 0, SEEK_SET);

            // Loop through the file reading in entire lines
            //   so as to get the line count
            while (fgets (szLine, sizeof (szLine), fp) != NULL)
            {
                _asm {
                      int 1;
                     }
                Accum[i-1].lines++;
            } // End WHILE

            // Close the file
            fclose (fp);
        } while (!_dos_findnext_lfn (&find));
    } // End FOR

    // Close the find handle
    _dos_findclose_lfn (&find);

    // Display the totals
    for (i = 1; i < argc; i++)
    {
        number_format (szLines, "%12lu", Accum[i-1].lines);
        number_format (szSize , "%12lu", Accum[i-1].size );

        fprintf (stdout,
                 "%s lines  %s bytes  %5u files:  %s\n",
                 szLines,
                 szSize,
                 Accum[i-1].files,
                 argv[i]);
        Accum[argc].lines += Accum[i-1].lines;
        Accum[argc].size  += Accum[i-1].size ;
        Accum[argc].files += Accum[i-1].files;
    } // End FOR

    number_format (szLines, "%12lu", Accum[argc].lines);
    number_format (szSize , "%12lu", Accum[argc].size );
    fprintf (stdout, "-----------------------------------------------------------------\n");
    fprintf (stdout,
             "%s lines  %s bytes  %5u files",
             szLines,
             szSize,
             Accum[argc].files);
    return 0;
} // End Main


//***************************************************************************
//  End of File: linecnt.c
//***************************************************************************
