// $Header:   P:/PVCS/MAX/TOOLS/VDIR.CPV   1.0   28 May 1996 16:13:34   HENRY  $
//
// VDIR.CPP - Directory verify utility
//
// Copyright (C) 1996 Qualitas, Inc.  All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <errno.h>

#include <dosdefs.h>
#include <statutil.h>

int
main( int argc, char *argv[] ) {

	int n, nRet = 0;
	int nCreate = 0, nVerbose = 0;

	fprintf( stderr, "VDIR 2.0 Copyright (C) 1996 Qualitas, Inc.  All rights reserved.\n" );

	// Pass 1: go through and read switches
	for (n = 1; n < argc; n++) {

		// Ignore non-switches
		if (!strchr( "/-", argv[ n ][ 0 ])) continue;

		int nSwitch;
		char cSwitch;

		for (nSwitch = 1; cSwitch = argv[ n ][ nSwitch ]; nSwitch++) {

			switch (cSwitch) {

				case 'v':
				case 'V':
					nVerbose++;
					break;

				case 'c':
				case 'C':
					nCreate++;
					break;

				case '?':
				case 'h':
				case 'H':
				default:
					fprintf( stderr, "Syntax:\n  vdir [-c] [-v] dirlist\n"
							"    -c creates missing directories\n"
							"    -v turns on verbose reporting\n"
							"    dirlist is a list of one or more directories, ending with or without \\\n"
							"    Default behavior is to test for existence of all directories in dirlist,\n"
							"    and return errorlevel set to number that are missing.\n" );
					return -1;

			} // switch

		} // for all switches in this arg

	} // for all args (switches)

	// Pass 2: process subdirectories
	for (n = 1; n < argc; n++) {

		// Ignore switches
		if (strchr( "/-", argv[ n ][ 0 ])) continue;

		static char szTemp[ _MAX_PATH ];

		strcpy( szTemp, argv[ n ] );
		RemoveBS( szTemp );

		unsigned uAttr;

		if (_dos_getfileattr( szTemp, &uAttr )) {
			uAttr = 0xffff;
		} // Failed to access file

		if (nCreate) {

			if (uAttr != 0xffff) {
				if (uAttr &_A_SUBDIR) {
					if (nVerbose) fprintf( stderr, "VDIR: directory %s already exists.\n", szTemp );
				} // Already exists
				else {
					fprintf( stderr, "VDIR Err: cannot create %s, already exists.\n", szTemp );
				} // Exists but is not a subdir
			} // File or directory exists
			else if (RMakeDir( szTemp )) {
				fprintf( stderr, "  VDIR error: cannot create %s\n", szTemp );
				nRet++;
			}
			else /***if (nVerbose)***/ {
				fprintf( stderr, "  Created %s\n", szTemp );
			}

		} // Make directories
		else {

			if (uAttr == 0xffff) {
				if (nVerbose) {
					fprintf( stderr, "  Dir %s does not exist, -c flag not specified.\n", szTemp );
				}
				nRet++;
			} // Can't get attributes because file doesn't exist
			else if (!(uAttr & _A_SUBDIR)) {
				if (nVerbose) {
					fprintf( stderr, "  %s exists but is not a directory\n", szTemp );
				}
				nRet++;
			} // Not a subdirectory
			else if (nVerbose) {
				fprintf( stderr, "  %s exists.\n", szTemp );
			} // All's OK

		} // Assert them

	} // for all args (not switches)

	return nRet;

} // main()

