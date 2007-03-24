//' $Header:   P:/PVCS/MAX/TOOLS/DEPSCAN.C_V   1.1   18 Apr 1996 08:29:48   HENRY  $
//
// DEPSCAN.C - Dependency scanner, quick and dirty version
//
// Copyright (C) 1995-2007 Qualitas, Inc.  All rights reserved.
//
// To truly do the job right, this program should parse the project
// makefile to get the INCLUDE path (but we're already getting in some
// mighty deep water, dear friends).  We make some bald-faced assumptions
// here.  The idea is to TRY to catch all %CATROOT%include and local
// includes.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <direct.h>
#include <dos.h>
#include <errno.h>
#include <io.h>

#define VERSION "2.00"

#define MAXINCLUDES 300
#define MAXFILES	500 // 13 * 500 = 6.5K

#define COPYRIGHT  "DEPSCAN v" VERSION "  Copyright (C) 1995-2007 Qualitas, Inc.  All rights reserved.\n"

// Display empty header dependencies
int gnShowEmpty = 0;

// Process $(I) for .INC files
int gnProcessMaxInc = 0;

// Useful for debugging: show resolution failures
int gnShowFailures = 0;

// No macros
int gnSkipMacros = 0;

// No local includes
int gnSkipLocal = 0;

// Don't complain about misused <> and ""
int gnQuiet = 0;

// Show non-local include files in header dependencies
int gnShowInc = 0;

// Number of include files active.	Always points to free entry if < MAXINCLUDES
int nIncludes;

// Stack of pointers to include files, including opening " < or space
char *ppszIncludes[ MAXINCLUDES ];

// Number of files to process
int nFiles;

// List of files to process
char *ppszFiles[ MAXFILES ];

// Path to substitute $(H) for
char szIncPath[ _MAX_PATH ];

// Path to substitute $(I) for
char szMaxIncPath[ _MAX_PATH ];

// Path to substitute $(Q) for
char sz386Path[ _MAX_PATH ];

// Path for local includes
char szLclPath[ _MAX_PATH ];

// Path for local root
char szLclRoot[ _MAX_PATH ];

// Prefix for local includes (e.g. {$(P)})
char szLclPref[ 80 ];

// Additional path to search, and optional macro (e.g. -sret\=$(O))
#define ADDPATH 4
char szAddPath[ADDPATH][_MAX_PATH];
char szAddPMacro[ADDPATH][80];

// Additional paths to search, and optional macro (e.g. -tc:\max\qed\w16\=$(O))
// If there's a match with the -t option path, then the macro is used
// in the dependency list as in $(O)qed.pro.
#define NEWPATH 4
char szNewPath[NEWPATH][_MAX_PATH];
char szNewPMacro[NEWPATH][80];


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


//********************************* fopen_lfn *********************************

FILE *fopen_lfn
	(const char *szFile,
	 const char *pFlags)

{
	char szFileShort[256];

	// Translate the long filename to a short filename
	_asm
	{
		push	si;
		push	di;

		mov 	ax,0x7160;
		mov 	cx,0x8001;
		mov 	si,szFile;
		lea 	di,szFileShort;
		int 	0x21;

		pop 	di;
		pop 	si;
	}

	return fopen (szFileShort, pFlags);
} // End fopen_lfn


//********************************* _splitpath_lfn ****************************

void _splitpath_lfn
	(const char *szPath,
	 char *szDrv,
	 char *szDir,
	 char *szFName,
	 char *szExt)

{
	char *p, *q, *r;

	// Split the path in szPath (e.g. "C:\path\to\filename.ext")
	// szDrv		"C:"
	// szDir		"/path/to/"
	// szFName		"filename"
	// szExt		".ext"

	p = (char *) szPath;

	// Check for drive separator
	if (p[1] == ':')
	{
		if (szDrv)
		{
			szDrv[0] = p[0];
			szDrv[1] = p[1];
			szDrv[2] = '\0';
		} // End IF

		// Skip over the drive letter and separator
		p += 2;
	} // End IF

	// Find rightmost path separator
	r = p;
	while (q = strchr (r, '\\'))
		r = q + 1;

	if (p != r)
	{
		if (szDir)
		{
			strncpy (szDir, p, r - p);
			szDir[r - p] = '\0';
		} // End IF

		p = r;
	} else
	if (szDir)
		szDir[0] = '\0';

	// Find rightmost extension separator
	r = p;
	while (q = strchr (r, '.'))
		r = q + 1;

	if (p != r)
	{
		r--;		// Back to the extension sep
		if (szFName)
		{
			strncpy (szFName, p, r - p);
			szFName[r - p] = '\0';
		} // End IF

		p = r;
		if (szExt)
			strcpy (szExt, p);
	} else
	{
		if (szFName)
			strcpy (szFName, p);
		if (szExt)
			szExt[0] = '\0';
	} // End IF
} // End _splitpath_lfn

//********************************* _access_lfn *******************************

int _access_lfn
	(const char *szFile,
	 int		 mode)

{
	char szFileShort[256];

	// Translate the long filename to a short filename
	_asm
	{
		push	si;
		push	di;

		mov 	ax,0x7160;
		mov 	cx,0x8001;
		mov 	si,szFile;
		lea 	di,szFileShort;
		int 	0x21;

		pop 	di;
		pop 	si;
	}

	return _access (szFileShort, mode);
} // End _access_lfn


//********************************* _searchenv_lfn ****************************

void _searchenv_lfn
	(const char *szFile,
	 const char *VarName,
		   char *PathName)

{
	char szFileShort[256];
	char szFName[_MAX_PATH];
	char szExt[_MAX_PATH];
	char szLclPathName[_MAX_PATH];
	WORD uRet;

	// Translate the long filename to a short filename
	_asm
	{
		push	si;
		push	di;

		mov 	ax,0x7160;
		mov 	cx,0x8001;
		mov 	si,szFile;
		lea 	di,szFileShort;
		int 	0x21;

		mov 	uRet,ax;		// Save error code or OK (0)

		pop 	di;
		pop 	si;
	}

	// If there was an error (FILE NOT FOUND),
	//	 use the original name
	_splitpath_lfn ((uRet == 0) ? szFileShort : szFile, NULL, NULL, szFName, szExt);

	strcat (szFName, szExt);

	// Searching
	if (!gnQuiet)
	{
		fprintf (stderr, "Searching for \"%s\" in %s.\n", szFName, VarName);
		fprintf (stderr, "Environment \"%s\" is %s\n", VarName, getenv (VarName));

	}

	_searchenv (szFName, VarName, szLclPathName);

	// If it failed, ...
	if (szLclPathName[0] == '\0')
		PathName[0] = '\0';
	else
	{
		if (!gnQuiet)
			fprintf (stderr, "File found in \"%s\"\n", szLclPathName);
		// Translate the short filename to a long filename
		_asm
		{
			push	si;
			push	di;

			mov 	ax,0x7160;
			mov 	cx,0x8002;
			lea 	si,szLclPathName;
			mov 	di,PathName;
			int 	0x21;

			pop 	di;
			pop 	si;
		}
		if (!gnQuiet)
			fprintf (stderr, "File found in \"%s\"\n", PathName);
	} // End IF/ELSE
} // End _searchenv_lfn


//********************************* CheckIncludes *****************************
// Add include file to list if not already there.  If no room, bitch.
// Returns origin:1 index of laundered name if a valid dependency, 0 if not.
int
CheckIncludes( const char *pszInclude, const char *pszIncSrchPath ) {

	char szDrive[ _MAX_PATH ], szDir[ _MAX_DIR ], szFName[ _MAX_FNAME ],
	szExt[ _MAX_EXT ], szPath[ _MAX_PATH ];
	char szBuff[_MAX_PATH];
	int n, k;
	char cDir = '\0';
	char *pszMacro = NULL;

	// Make a copy without the trailing " or >
	// If no " or >, leave room for leading space
	char *psz = (char *)malloc( strlen( pszInclude ) + 2 );

	if (!psz)
	{
		fprintf( stderr, "Memory allocation failure in CheckIncludes(%s)\n",
				pszInclude );
		return 0;
	} // Unable to copy

	strcpy (psz, pszInclude);
	if (!*psz)
	{
		free( psz );
		return 0;
	} // Nothing to do

	if (!gnQuiet)
		fprintf (stderr, "\nSearching for %s in include path:  %s\n", psz, pszIncSrchPath);

	if (strpbrk( psz, "\"<>" )) // Clobber trailing character (assumed to be " or >)
		psz[ strlen( psz ) - 1 ] = '\0'; // Truncate
	else	// Blast in leading space
		sprintf( psz, " %s", pszInclude );

	// Resolve to a complete path by searching INCLUDE or INCASM
	// FIXME this won't handle things like <sys\stat.h>, but we don't
	// care about that anyway and don't use that mechanism in $(I).
	_searchenv_lfn (psz + 1, pszIncSrchPath, szPath);

	_splitpath_lfn (szPath, szDrive, szDir, szFName, szExt);
	strcpy (szBuff, szDrive);
	strcat (szBuff, szDir);

	// Check for additional search paths
	if (!pszMacro)
	for (k = 0; k < ADDPATH; k++)
	if (szAddPath[k][0]
	 && !_stricmp (szAddPath[k], szBuff))
	{
/////// sprintf (szPath, "%s%s", szAddPath[k], psz + 1 );
		if (_access_lfn (szPath, 0)) // Not found
			szPath[0] = '\0';
		else						// User-defined string to use
		{
			pszMacro = szAddPMacro[k];
			break;
		}
	} // Not in path, try additional directory

	// Check for alternate paths with macro substitution
	if (!pszMacro)
	for (k = 0; k < NEWPATH; k++)
	if (szNewPath[k][0]
	 && !_stricmp (szNewPath[k], szBuff))
	{
		if (_access_lfn (szPath, 0)) // Not found
			szPath[0] = '\0';
		else
		{
			pszMacro = szNewPMacro[k];
			break;
		} // User-defined string to use
	} // Not in path, try additional directory

	// If not found, ignore it.
	if (!szPath[0])
	{
		if (gnShowFailures)
			fprintf (stderr, "\nUnable to find %s in include path\n", psz + 1);
		free (psz);
		return 0;
	} // Not found

	// Split the path to see if it's one we're interested in
	_splitpath_lfn (szPath, szDrive, szDir, szFName, szExt);

	// Skip this file if it's a .PRO -- *FIXME* -- make this an argument
	if (!stricmp (szExt, ".pro"))
	{
		free (psz);
		return 0;
	} // End IF

	// FIXME Filter out .rc extensions

	// Reconstruct path to match with
	strcat (szDrive, szDir);

////if (!strnicmp (szDrive, szLclPath, strlen (szDrive))) // Delete local path
	if (!stricmp (szDrive, szLclPath))				// Delete local path
		szDrive[0] = '\0';

	if (!stricmp (szDrive, szIncPath))				// Abbreviation for $(MAXROOT)include
		cDir = 'H';
	else if (!stricmp (szDrive, szMaxIncPath))		// Abbreviation for $(MAXROOT)inc
		cDir = 'I';
	else if (!stricmp (szDrive, sz386Path)) 		// Abbreviation for $(MAXROOT)386max
		cDir = 'Q';
	else if (!pszMacro && szDrive[0])				// Not local or in $(MAXROOT)include, etc.
	{
		if (!gnQuiet)
			fprintf (stderr, "\nIgnoring %s\n", szPath);
		free (psz);
		return 0;
	}

////if (!gnQuiet)
////	fprintf (stderr, "Found:  \"%s\"\n", );

	if (szDrive[0]) 								// Construct $(I), $(C), $(H), or $(Q) path
	{
		// Piss and moan if there's a double-quoted include in $(I) or $(H)
		if (*psz == '"'
		 && !gnQuiet
		 && strnicmp (szDrive, szLclPath, strlen (szDrive)))	// and not local
			fprintf (stderr, "\ninclude %s: double quotes should only be used for local includes.\n", psz + 1 );

		if (!gnQuiet)
		  fprintf (stderr, "\nszDrive = %s, szLclPath = %s,\n      psz = %s, pszMacro = %s.\n",
						   szDrive, szLclPath, psz, pszMacro);
		if (!pszMacro)
			sprintf (szDrive, "%c$(%c)%s%s", *psz, cDir, szFName, szExt);
		else
			sprintf (szDrive, "%c%s%s%s", *psz, pszMacro, szFName, szExt);
		//fprintf (stderr, "\nszDrive = %s.\n", szDrive);
	} else											// Local
	{
		// If ignoring local includes, end of line
		if (gnSkipLocal)
		{
			if (!gnQuiet)
				fprintf (stderr, "Ignoring local include %s\n", psz);
			free( psz );
			return 0;
		} // Skipping all local includes

		// Piss and moan if <> is used for a local include
		if (*psz == '<' && !gnQuiet)
			fprintf( stderr, "\ninclude %s: <> should only be used for non-local includes.\n", psz + 1 );

		sprintf (szDrive, "%c%s%s", *psz, szFName, szExt);
	}

	free (psz);
	psz = strdup (szDrive);
	if (!psz)
	{
		fprintf (stderr, "\n!strdup() failed!\n");
		return 0;
	} // failure

	// psz is either " or < or space followed by
	// $(I)fname.ext or fname.ext.
	// Check for it in the list.
	for (n = 0; n < nIncludes; n++)
	if (!stricmp (psz + 1, ppszIncludes[n] + 1))
	{
		if (!gnQuiet)
			fprintf (stderr, "Ignoring existing include %s\n", psz);
		free( psz );
		return (n + 1);
	} // Found a match

	// No match.  If no room, bitch loudly.
	if (nIncludes >= MAXINCLUDES)
	{
		fprintf (stderr, "\n!!! No room for include %s !!!\n", psz + 1);
		free (psz);
		return 0;
	} // No room

////// Ignore MASM .INC files in $(MAXROOT)inc unless specified
////if (!gnProcessMaxInc && (cDir == 'I') || (cDir == 'H'))
////{
////	if (!gnQuiet)
////		fprintf (stderr, "Ignoring MASM .INC include %s\n", psz);
////	free (psz);
////	return 0;
////} // Ignoring files in $(MAXROOT)inc

	// Stick it, bump it, and we're outta here
	// fprintf (stderr, "\npsz = %s.\n", psz);
	ppszIncludes[nIncludes] = psz;
	nIncludes++;

	return nIncludes;
} // CheckIncludes()


//********************************* ScanFile **********************************
// Scan a file for dependencies.  Return 0 if OK, -1 if an error occurred.
int
ScanFile (const char *pszFile, int bDisplay, int bForceHeader, int bForceASM)
{
	FILE *pf;
	char szBuff[ 256 ];
	char szFName[ _MAX_FNAME ], szExt[ _MAX_EXT ];
	static char szPath[ _MAX_PATH ];
	char *pszTok1, *pszTok2;
	const char *pszOrg = pszFile;
	int bRC, bASM = 0;
	int nInc;
	int nHeaderShown = 1;
	int nLocal = 0;
	int k;

	//fprintf (stderr, "Scanning file:  %s.\n", pszFile);

	// If a $(H) include, expand full path.
	if (!strncmp( pszFile, "$(H)", 4 ))
	{
		strcpy (szPath, szIncPath);
		strcat (szPath, pszFile + 4);
		pszFile = szPath;
	} else if (!strncmp( pszFile, "$(I)", 4 ))
	{
		strcpy (szPath, szMaxIncPath);
		strcat (szPath, pszFile + 4);
		pszFile = szPath;
	} else if (!strncmp( pszFile, "$(Q)", 4 ))
	{
		strcpy (szPath, sz386Path);
		strcat (szPath, pszFile + 4);
		pszFile = szPath;
		nLocal = 1;
	} else
	{
		for (k = 0; k < ADDPATH; k++)
		if (szAddPMacro[k][0]
		 && !strncmp (pszFile, szAddPMacro[k], strlen (szAddPMacro[k])))
		{
			strcpy (szPath, szAddPath[k] );
			strcat (szPath, pszFile + strlen (szAddPMacro[k]));
			pszFile = szPath;
			nLocal = 1;
			break;
		} // Make szAddPath[k] path

		for (k = 0; k < NEWPATH; k++)
		if (szNewPMacro[k][0]
		 && !strnicmp (pszFile, szNewPMacro[k], strlen (szNewPMacro[k])))
		{
			strcpy (szPath, szNewPath[k]);
			strcat (szPath, pszFile + strlen (szNewPMacro[k]));
			pszFile = szPath;
			nLocal = 1;
			break;
		} // Make szNewPath[k] path
	}

	pf = fopen_lfn (pszFile, "r");
	if (!pf)
	{
		fprintf( stderr, "Couldn't open %s\n\n", pszFile );
		return -1;
	} // Open failed

	_splitpath_lfn (pszFile, NULL, NULL, szFName, szExt);

	strupr( szFName );

	// If an .RC file, we need to scan for some additional keywords.
	// Note that any file with extension .$(LANG) is an RC file.
	bRC =	!stricmp( szExt, ".rc" )
		 || !stricmp( szExt, ".ver" )
		 || !stricmp( szExt, ".us" )    // US
		 || !stricmp( szExt, ".gr" )    // German
		 || !stricmp( szExt, ".fr" );   // French

	// .ASM and .INC files use different syntax
	if (bForceASM)
		bASM = 1;
	else
	if (!bRC)
	{
		bASM = !stricmp( szExt, ".asm" ) ||
			   !stricmp( szExt, ".inc" ) ||
			   !stricmp( szExt, ".mac" );

		// Also recognize .a$(LANG) as .asm
		if (!bASM)
		{
			bASM =	!stricmp( szExt, ".aus" ) ||    // US
					!stricmp( szExt, ".agr" ) ||    // German
					!stricmp( szExt, ".afr" );      // French
		} // Not already recognized as .asm or .rc
	} // Not already recognized as .rc

	if (bDisplay)
	{
		// If we're skipping macros, turn off display for all non-headers.
		if (gnSkipMacros)
		{
			bDisplay = 0;
		} // Turn it off (assuming it's not a header)

		if (bRC)
		{
			if (bDisplay)
				printf( "%s_RCDEP =", szFName );
		} else // RC file
		if (!stricmp( szExt, ".h" ) ||
			!stricmp( szExt, ".hpp" ) ||
			!stricmp( szExt, ".inc" ) ||
			bForceHeader)
		{
			// Now check for skipping local includes
			bDisplay = !gnSkipLocal || !nLocal; // Turn it back on if not local

			if (bDisplay)
			{
				if (gnShowEmpty) // Display even if empty
					printf( "%s:", pszOrg );
				else			// Defer display until we know we've got something
					nHeaderShown = 0;
			} // Displaying

		} else // .H or .HPP file
		if (bASM)
		{
			if (bDisplay)
				printf( "%s_ADEP =", szFName );
		} else // .ASM file
			if (bDisplay)
				printf( "%s_DEP =", szFName );// Normal file
	} // Displaying

	while (fgets (szBuff, sizeof (szBuff), pf) != NULL)
	{
		pszTok1 = strtok (szBuff, " \t\r\n");
		pszTok2 = strtok (NULL,   " \t\r\n");

		if (!pszTok1 || !pszTok2)	// Ignore empty lines
			continue;

		if (bRC && bDisplay)
		{
			if (!stricmp( pszTok2, "ICON" ) ||
				!stricmp( pszTok2, "BITMAP" ) ||
				!stricmp( pszTok2, "CURSOR" ))
			{
				//fprintf( stderr, "RC! %s %s\n", pszTok1, pszTok2 );
				do {
					pszTok2 = strtok( NULL, " \t\r\n" );
				} while (pszTok2 && *pszTok2 != '"');
				if (pszTok2 && strlen( pszTok2 ) > 1)
				{
					pszTok2++; // Skip leading quote
					pszTok2[ strlen( pszTok2 ) - 1 ] = '\0';
					if (*pszTok2)
					{
						if (!nHeaderShown)
						{
							nHeaderShown = 1;
							printf( "%s:", pszOrg );
						} // .H file: haven't displayed name yet

						//fprintf (stderr, "RC! %s %s %s\n", pszTok1, pszTok2, szLclPref);

						printf( "\\\n\t%s%s",
						////////_access_lfn (pszTok2, 0 ) ? "" : szLclPref,
								szLclPref,
								pszTok2 );
					} // Filename is not empty
				} // Got filename
				continue;
			} // Found one
		//else fprintf( stderr, "RC  %s %s", pszTok1, pszTok2 );
		} // Check for ICON, BITMAP, CURSOR statements
		//else fprintf( stderr, ">%s %s", pszTok1, pszTok2 );

		// Handle .ASM files separately
		if (bASM)
		{
			if (stricmp( pszTok1, "include" ))
			{
				//fprintf( stderr, " (MASM %s!=include)\n", pszTok1 );
				continue;
			} // Not an include

			//fprintf( stderr, "\nChecking INCASM %s\n", pszTok2 );
			nInc = CheckIncludes( pszTok2, "INCASM" );
			if (!nInc)
			{
				continue;
			} // Not an include we care about

		} else // .ASM or .INC
		{
			if (stricmp( pszTok1, "#include" ))
			{
				//fprintf( stderr, " (%s!=#include)\n", pszTok1 );
				continue;
			} // Not an include

			//fprintf( stderr, "\nChecking INCLUDE %s\n", pszTok2 );
			nInc = CheckIncludes( pszTok2, "INCLUDE" );
			if (!nInc)
			{
				continue;
			} // Not an include we're interested in

		} // .C, .CPP, .H, .HPP, .RC, all other types

		if (bDisplay)
		{
			if (!nHeaderShown)
			{
				nHeaderShown = 1;
				printf( "%s:", pszOrg );
			} // .H or .INC file: haven't displayed name yet
			printf( "\\\n\t%s%s",
				_access_lfn (ppszIncludes[nInc - 1] + 1, 0 ) ? "" : szLclPref,
							 ppszIncludes[nInc - 1] + 1);
		} // Displaying
	} // Not EOF

	fclose (pf);

	if (bDisplay && nHeaderShown)	// Displaying and it wasn't an empty .H
		printf ("\n\n");

	return 0;
} // ScanFile()


//********************************* under_dir *********************************
// Return 1 if currently under this dir, 0 if not
int
under_dir( const char *pszRoot ) {

	char szCurrent[ _MAX_PATH ];
	char szRootCopy[ _MAX_PATH ];
	int nLen;

	_getcwd( szCurrent, sizeof( szCurrent ) - 1 );

	szCurrent[ sizeof( szCurrent ) - 1 ] = '\0';

	// Make sure there's no trailing backslash
	strcpy( szRootCopy, pszRoot );
	nLen = strlen( szRootCopy );
	if (szRootCopy[ nLen - 1 ] == '\\') {
	szRootCopy[ nLen - 1 ] = '\0';
	} // Lop off trailing bs

	strlwr( szCurrent );
	strlwr( szRootCopy );

	if (strstr( szCurrent, szRootCopy ) == szCurrent) {
	return 1;
	} // We're under it
	else {
	return 0;
	} // Not

} // under_dir()


//********************************* CopyTokn **********************************
// Copy a token until we find a character in the delimiter set.
// Return number of characters copied.
int
CopyTokn( char *pszDest, const char *pszSrc, const char *pszDelim ) {

	int nCopied = 0;

	while (*pszSrc && !strchr( pszDelim, *pszSrc )) {
	*pszDest++ = *pszSrc++;
	nCopied++;
	}
	*pszDest = '\0';

	return nCopied;

} // CopyTokn()


//********************************* SyntErr ***********************************
int
SyntErr( int nType ) {

	fprintf( stderr,
	"Syntax: DEPSCAN [options] [@infile] filename.ext [filename.ext ...]\n"
	"where options may be-\n"
	"-e       includes empty header dependencies\n"
	"-h or -? print this message\n"
	"-i       includes $(H) and $(I) header dependencies\n"
	"-l       skip local includes\n"
	"-m       list $(I) include dependencies from $(MAXROOT)inc\n"
	"-n       no macros or local headers ($(H) and $(I) if -i)\n"
	"-pprefix prepends prefix to all local dependencies\n"
	"-q       quiet mode (don't complain about incorrectly used <> and \"\")\n"
	"-rdir    set new root to be used instead of MAXROOT or CATROOT\n"
	"-sdir=macro[,dir2=macro2]\n"
	"         adds backslash-terminated dir to search path and uses macro\n"
	"         for dependencies found in it.\n"
	"-tdir=macro,[dir2=macro2]\n"
	"         If there's a match with the -t option path, then the macro is used\n"
	"         in the dependency list as in $(O)qed.pro.\n"
	"-x       displays additional data for debugging.\n"
	"@infile may be specified to contain options and / or filespecs.\n"
			 );
	return nType;

} // SyntErr()


//********************************* GetSearch *********************************
// Get search path with optional macro specified.
// Returns number of characters skipped
int
GetSearch( char *pszAddPath, char *pszAddPMac, const char *pszArg ) {

	int nRet;
	char *pszEq;

	nRet = CopyTokn( pszAddPath, pszArg, ", \t" );

	// Now find = in pszAddPath
	pszEq = strchr( pszAddPath, '=' );

	*pszAddPMac = '\0';
	if (pszEq) {

	// Terminate it
	*pszEq = '\0';

	// Copy it
	strcpy( pszAddPMac, pszEq + 1 );

	} // Found terminator and optional macro
	//else {
	//	fprintf( stderr, "Error processing %c%s - no = specified\n", pszArg[ 0 ], &pszArg[ j ] );
	//	return -1;
	//}

	if (!gnQuiet)
	fprintf( stderr, "Additional search = %s, macro = %s\n",
				pszAddPath, pszAddPMac );

	return nRet;

} // GetSearch()


//********************************* _dos_findfirst_lfn ******************************

unsigned int _dos_findfirst_lfn
	(const	  char		*szFile,
	 WORD				 Attrs,
	 struct _find_t_lfn *findstr)

{
	unsigned char uAttr;
	WORD		  uRet, uFileHandle;

	uAttr = (char) Attrs;

	_asm
	{
		push	si;
		push	di;

		mov 	ax,0x714E;
		mov 	cl,uAttr;	// Get attributes to look for
		mov 	ch,0;		// No attributes required
		mov 	si,1;		// Use MS-DOS date/time
		mov 	dx,szFile;	// DS:DX ==> input ASCIIZ LFN
		mov 	di,findstr; // ES:DI ==> input structure
		int 	0x21;
		mov 	uFileHandle,ax;// Save FileHandle
		mov 	uRet,0; 	// Assume OK
		jnc 	OK; 		// Jump if OK

		mov 	uRet,ax 	// Save error code
OK:
		pop 	di;
		pop 	si;
	}

	findstr->FileHandle = uFileHandle;

	return uRet;
} // End _dos_findfirst_lfn


//********************************* _dos_findnext_lfn *******************************

unsigned int _dos_findnext_lfn
	(struct _find_t_lfn *findstr)

{
	WORD uRet;

	uRet = findstr->FileHandle;

	_asm
	{
		push	si;
		push	di;

		mov 	ax,0x714F;
		mov 	bx,uRet;	// Get FileHandle
		mov 	di,findstr; // ES:DI ==> input structure
		mov 	si,1;		// Use MS-DOS date/time
		int 	0x21;
		mov 	uRet,0; 	// Assume OK
		jnc 	OK; 		// Jump if OK

		mov 	uRet,ax;	// Save error code
	OK:
		pop 	di;
		pop 	si;
	}

	return uRet;			// Return code (error) or 0 (OK)
} // End _dos_findnext_lfn


//********************************* _dos_findclose_lfn ******************************

void _dos_findclose_lfn
	(struct _find_t_lfn *findstr)

{
	WORD uRet;

	uRet = findstr->FileHandle;

	_asm
	{
		mov 	ax,0x71a1;
		mov 	bx,uRet;	// Get FileHandle
		int 	0x21;
	}
} // End _dos_findclose_lfn


//********************************* FindMatchingFiles *************************
// Find matching files
int
FindMatchingFiles (char *pszArg, char szSpecPath[], char *pszArgZ)
{
	struct _find_t_lfn find = {0};
	int iRet;

	if (!_dos_findfirst_lfn (pszArg, _A_ARCH | _A_RDONLY, &find))
	do
	{
		ppszFiles [nFiles] = (char *) malloc (strlen (szSpecPath) + strlen (find.name) + 1);
		if (!ppszFiles[ nFiles ])
		{
			fprintf (stderr, "malloc() failure on %s\n", pszArgZ);
			iRet = -1;
			goto EXIT;
		}

		sprintf (ppszFiles [nFiles], "%s%s", szSpecPath, find.name);

		nFiles++;

		if (nFiles == MAXFILES)
		{
			fprintf( stderr, "!!! Too many files - truncating list !!!\n" );
			iRet = -1;
			goto EXIT;
		} // Sound the alarum
	} while (!_dos_findnext_lfn (&find));

	iRet = 0;
EXIT:
	_dos_findclose_lfn (&find);

	return iRet;
} // FindMatchingFiles ()


//********************************* ProcessOpts *******************************
// Return 0 if OK or -1 if failure or invalid options.
int
ProcessOpts( FILE *pfIn, int nArgs, char *ppszArgv[] ) {

	int i = 1, j, k;
	char *pszArg;
	char szArgBuff[ _MAX_PATH ];
	static char szSpecPath[ _MAX_PATH ], szSpecDir[ _MAX_DIR ];

	// Check for empty arg list
	if (!pfIn && nArgs < 2) {
	fprintf( stderr, "No arguments specified.\n" );
	return SyntErr( -1 );
	} // Nothing to do

	// We have either an argv list or an open file
	do {
	if (pfIn) {
		if (fscanf( pfIn, "%s", szArgBuff ) < 1) {
		if (feof( pfIn )) {
			return 0;
		} // End of file
		else {
			return -1;
		} // Read error
		} // End of file or error
		pszArg = szArgBuff;
	} // Reading from file
	else {
		pszArg = ppszArgv[ i ];
	} // Reading from list

	//fprintf (stderr, "arg=%s\n", pszArg);
	if (pszArg[0] == '-' || pszArg[0] == '/')
	{
		for (j = 1; pszArg[j]; j++)
		switch (pszArg[j])
		{
			case 'e':
			case 'E':
				gnShowEmpty = 1;
				break;

			case 'h':
			case 'H':
			case '?':
				return SyntErr( -1 );

			case 'i':
			case 'I':
				gnShowInc = 1;
				break;

			case 'l':
			case 'L':
				gnSkipLocal = 1;
				break;

			case 'm':
			case 'M':
				gnProcessMaxInc = 1;
				break;

			case 'n':
			case 'N':
				gnSkipMacros = 1;
				break;

			case 'p':
			case 'P':
				j += CopyTokn( szLclPref, &pszArg[ j + 1 ], " \t" );
				if (!gnQuiet)
					fprintf( stderr, "Local prefix = %s\n", szLclPref );
				break;

			case 'q':
			case 'Q':
				gnQuiet = 1;
				break;

			case 'r':
			case 'R':
				j += CopyTokn( szLclRoot, &pszArg[ j + 1 ], " \t" );
				if (!gnQuiet)
					fprintf( stderr, "Local root = %s\n", szLclRoot );
				break;

			case 's':
			case 'S':
				for (k = 0; k < ADDPATH; k++)
				if (!szAddPath[k][0])
				{
////////////		fprintf( stderr, "next=%s\n", &pszArg[ j + 1 ] );
					j += GetSearch (szAddPath[k], szAddPMacro[k], &pszArg[j + 1]);
					if (pszArg[j + 1] == ',')
						j++;	// Skip over the separator
					else
						break;
				}
				break;

			case 't':
			case 'T':
				for (k = 0; k < NEWPATH; k++)
				if (!szNewPath[k][0])
				{
////////////		fprintf( stderr, "next=%s\n", &pszArg[ j + 1 ] );
					j += GetSearch (szNewPath[k], szNewPMacro[k], &pszArg[j + 1]);
					if (pszArg[j + 1] == ',')
						j++;	// Skip over the separator
					else
						break;
				} // Additional arg
				break;

			case 'x':
			case 'X':
				gnShowFailures = 1;
				break;

			default:
				fprintf( stderr, "Unknown option '%c'\n", pszArg[ j ] );
				return SyntErr( -1 );
		} // for all options in this group

		continue;

	} // Process option switch

	// Handle @ args
	if (pszArg[ 0 ] == '@') {

		FILE *pf = fopen_lfn (&pszArg[ 1 ], "r");

		if (pf) {
		int nRes = ProcessOpts( pf, 0, NULL );
		fclose( pf );
		if (nRes < 0) {
			return nRes;
		} // Failed
		} // File opened
		else {
		fprintf( stderr, "Error: could not open %s\n", &pszArg[ 1 ] );
		return -1;
		} // Open failed

		continue;

	} // Open @ file

	// Handle search path inside {...}
	if (pszArg[0] == '{')
	{
		char *pszSC0, *pszSCZ;

		pszArg++;		// Skip over left brace

		// Check for matching right brace
		pszSCZ = strchr (pszArg, '}');
		if (pszSCZ == NULL)
		{
		fprintf (stderr, "Missing right brace:  %s\n", pszArg);
		return -1;
		}
		// Try each directory in turn
		while (pszArg < pszSCZ)
		{
		// Find the end of the directory
		pszSC0 = strchr (pszArg, ';');
		if (pszSC0 == NULL)
			pszSC0 = pszSCZ;
		*pszSC0 = '\0';
		strcpy (szSpecDir, pszArg);
		strcat (szSpecDir, pszSCZ + 1);

		if (FindMatchingFiles (szSpecDir, pszArg, pszSCZ + 1) == -1)
			return -1;
		pszArg = pszSC0 + 1;		// Skip to next dir
		} // End WHILE
	} else
	{
		_splitpath_lfn (pszArg, szSpecPath, szSpecDir, NULL, NULL);
		strcat( szSpecPath, szSpecDir );

		if (FindMatchingFiles (pszArg, szSpecPath, pszArg) == -1)
		return -1;
	} // End if searchpath
	} while (pfIn || ++i < nArgs);

	return 0;

} // ProcessOpts()


//********************************* Main **************************************
int
main (int argc, char *argv[], char *envp[])
{
	int i, k, nRes = -1, bDone;
	const char *pszCatroot;
	char szTmpPath[_MAX_PATH];

////fprintf (stderr, COPYRIGHT);

#if 0 //********************************************
	// Dump argv
	fprintf( stderr, "argv:\n" );
	for (i = 0; argv[ i ]; i++) {
		fprintf( stderr, "  %d: %s\n", i, argv[ i ] );
	}

	// Dump environment
	fprintf( stderr, "command line env:\n" );
	for (i = 0; envp[ i ]; i++) {
		fprintf( stderr, "  %d: %s\n", i, envp[ i ] );
	}

	fprintf( stderr, "_environ:\n" );
	for (i = 0; _environ[ i ]; i++) {
		fprintf( stderr, "  %d: %s\n", i, _environ[ i ] );
		if (i > 10) {
		break;
		}
	}
#endif	//*******************************************

	if (argc < 2)	// No filenames specified
		return SyntErr (-1);

	// Initialize include list
	memset (ppszIncludes, 0, sizeof (ppszIncludes));
	nIncludes = 0;

	szLclPref[0] = '\0';
	szLclRoot[0] = '\0';
	for (k = 0; k < ADDPATH; k++)
		szAddPath[k][0] = '\0';
	for (k = 0; k < NEWPATH; k++)
		szNewPath[k][0] = '\0';

	nFiles = 0;
	if (ProcessOpts (NULL, argc, argv) < 0)
	{
		return -1;
	} // Invalid options

	if (!nFiles)
	{
		fprintf (stderr, "Nothing to do!\n" );
		return -1;
	} // No files

	// If -r is specified, use that instead of "CATROOT"
	if (szLclRoot[0])
		pszCatroot = szLclRoot;
	else
		pszCatroot = getenv ("CATROOT");

	// If we're not currently under CATROOT, use MAXROOT
	if (!pszCatroot || !under_dir (pszCatroot))
	{
		pszCatroot = getenv ("MAXROOT");
		if (!pszCatroot)
		{
			fprintf( stderr, "MAXROOT not defined.\n" );
			return -1;
		} // Can't get MAXROOT
	} // Under MAXROOT

	// Set up szIncPath, szMaxIncPath, sz386Path and szLclPath
////sprintf (szIncPath, "%sinclude\\",  pszCatroot);
	strcpy (szIncPath,		getenv ("HDIR"));
////sprintf (szMaxIncPath,	"%sinc\\",  pszCatroot);
	strcpy (szMaxIncPath,	getenv ("INCDIR"));
////sprintf (sz386Path, "%s386max\\",   pszCatroot);
	strcpy (sz386Path,		getenv ("MAXDIR"));

	_getcwd (szLclPath, sizeof( szLclPath ) );
	if (szLclPath[ 0 ] && szLclPath[ strlen( szLclPath ) - 1 ] != '\\') // Assert trailing BS
		strcat( szLclPath, "\\" );

	// Set C environment's INCLUDE path to look only in %CATROOT%include
	// (so we can use _searchenv_lfn)
	strcpy (szTmpPath, "INCLUDE=");
	strcat (szTmpPath, szLclPath);
	strcat (szTmpPath, ";");
////strcat (szTmpPath, pszCatroot);
////strcat (szTmpPath, "include");
	strcat (szTmpPath, szIncPath);

	for (k = 0; k < ADDPATH; k++)
	if (szAddPath[k][0])
	{
		strcat (szTmpPath, ";");
		strcat (szTmpPath, szAddPath[k]);
	} // End IF

	for (k = 0; k < NEWPATH; k++)
	if (szNewPath[k][0])
	{
		strcat (szTmpPath, ";");
		strcat (szTmpPath, szNewPath[k]);
	} // End IF

////if (!gnQuiet)
////	fprintf (stderr, "Setting %s\n", szTmpPath);
////if (_putenv (strdup (szTmpPath)) < 0)
////{
////	fprintf (stderr, "Unable to modify local environment.  INCLUDE=%s\n",
////			 getenv ("INCLUDE"));
////	return -1;
////} // Failed to set

	// Set INCASM path to look in %MAXROOT%inc, %MAXROOT%include, and
	// %MAXROOT%386max.
	strcpy (szTmpPath, "INCASM=");
////strcat (szTmpPath, pszCatroot);
////strcat (szTmpPath, "inc;");
	strcat (szTmpPath, szMaxIncPath);
	strcat (szTmpPath, ";");
////strcat (szTmpPath, pszCatroot);
////strcat (szTmpPath, "include;");
	strcat (szTmpPath, szIncPath);
	strcat (szTmpPath, ";");
////strcat (szTmpPath, pszCatroot);
////strcat (szTmpPath, "386max");
	strcat (szTmpPath, sz386Path);

	for (k = 0; k < ADDPATH; k++)
	if (szAddPath[k][0])
	{
		strcat (szTmpPath, ";");
		strcat (szTmpPath, szAddPath[k]);
	} // End IF

	for (k = 0; k < NEWPATH; k++)
	if (szNewPath[k][0])
	{
		strcat (szTmpPath, ";");
		strcat (szTmpPath, szNewPath[k]);
	} // End IF

	if (!gnQuiet)
		fprintf (stderr, "Setting %s\n", szTmpPath);
	if (_putenv (strdup (szTmpPath)) < 0)
	{
		fprintf (stderr, "Unable to add INCASM=%s\nto local environment.\n",
				szTmpPath );
		return -1;
	} // Failed

	if (gnShowFailures && !gnQuiet)
	{
		fprintf (stderr, "Local path = %s\n"
						 "$(H) = %s\n"
						 "$(I) = %s\n"
						 "$(Q) = %s\n"
						 "INCLUDE=%s\n",
						 szLclPath, szIncPath, szMaxIncPath, sz386Path, getenv ("INCLUDE"));
	}

	if (!gnQuiet)
		fprintf (stderr, "Sorting file list...\n");

	// Use a simple bubble sort, but compare only file basenames
	do
	{
		char szFn1[_MAX_FNAME], szFn2[_MAX_FNAME];
		char *pszSwap;

		bDone = 1;
		for (i = 0; i < nFiles - 1; i++)
		{
			_splitpath_lfn (ppszFiles[i], NULL, NULL, szFn1, NULL);
			_splitpath_lfn (ppszFiles[i + 1], NULL, NULL, szFn2, NULL);

			if (stricmp( szFn1, szFn2 ) > 0)
			{
				pszSwap = ppszFiles[i];
				ppszFiles[i] = ppszFiles[i + 1];
				ppszFiles[i + 1] = pszSwap;

				bDone = 0;
			} // Swap 'em

		} // for all elements

	} while (!bDone);

	if (!gnQuiet)
	{
		fprintf (stderr, "Scanning source file dependencies (%d files)...\n", nFiles);
		printf ("\n# Dependencies generated by DepScan v" VERSION "\n\n");
	} // End IF

	for (i = 0; i < nFiles; i++)
	{
		// Ignore same second files
		if (i != 0)
		{
			char szFname1[_MAX_FNAME], szFname2[_MAX_FNAME];
			char szExt1[_MAX_EXT], szExt2[_MAX_EXT];

			_splitpath_lfn (ppszFiles[i - 1], NULL, NULL, szFname1, szExt1);
			strcat (szFname1, szExt1);

			_splitpath_lfn (ppszFiles[i],		  NULL, NULL, szFname2, szExt2);
			strcat (szFname2, szExt2);

			if (stricmp (szFname1, szFname2) == 0)
				continue;
		}
		if (!gnQuiet)
			fprintf (stderr, "\rScanning %s              ", ppszFiles[i]);
		nRes = ScanFile (ppszFiles[ i ], 1, 0, 0);
		free (ppszFiles[i]);
	} // for all files

	if (!gnQuiet)
		fprintf (stderr, "\rScanning header dependencies (pass 1)...\n");

	// Now scan include files
	// FIXME Do an n-pass approach until we're not snagging any new ones,
	// don't display, then sort and display 'em all...
	for (i = 0; i < nIncludes; i++) 	// for all includes
		nRes = ScanFile (ppszIncludes[i] + 1, 0, 1,
						 ppszIncludes[i][0] == ' ');

	if (!gnQuiet)
		fprintf (stderr, "\rSorting %u header dependencies...\n", nIncludes);

////for (i = 0; i < nIncludes; i++)
////	fprintf (stderr, "File = %s.\n", ppszIncludes[i]);

	do									// Use a simple bubble sort.
	{
		char *pszSwap;

		bDone = 1;
		for (i = 0; i < nIncludes - 1; i++)
		if (stricmp (ppszIncludes[i] + 1, ppszIncludes[i + 1] + 1) > 0) // If [i] > [i+1], ...
		{
			pszSwap = ppszIncludes[i + 1];
			ppszIncludes[i + 1] = ppszIncludes[i];
			ppszIncludes[i] = pszSwap;
			bDone = 0;
		} // Swap 'em
	} while (!bDone);

	if (!gnQuiet)
	{
		fprintf( stderr, "\rScanning header dependencies (pass 2)...\n" );
		printf( "\n# Header dependencies\n\n" );
	} // End IF

	// Now scan include files
	// FIXME Do an n-pass approach until we're not snagging any new ones,
	// don't display, then sort and display 'em all...
	for (i = 0; i < nIncludes; i++)
	{
		if (gnShowInc ||
			(strnicmp (ppszIncludes[i] + 1, "$(I)", 4)
		  && strnicmp (ppszIncludes[i] + 1, "$(H)", 4)))
		{
			nRes = ScanFile (ppszIncludes[ i ] + 1, 1, 1,
							 ppszIncludes[i][0] == ' ');
		} // Showing stuff from $(I) and $(H) or it's local or in $(C)
		//if (nRes < 0) {
		//	break;
		//} // Error occurred
	} // for all includes

	if (!gnQuiet)
	fprintf( stderr, "\nDone.\n" );

	return nRes;

} // main()

