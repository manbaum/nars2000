//***************************************************************************
//	NARS2000 -- Defined Variables
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

#ifdef DEBUG
  ////#define DEBUG_REFCNT
#endif

// Default definitions
#define DEF_APLFONT_INTNAME L"SImPL Medium"     // The default font internal name for NARS2000
#define DEF_APLFONT_EXTNAME L"SImPL-Medium"     // ...              external ...
#define DEF_APLFONT_TTF 	L".ttf"             // ...                       extension
#define DEF_APLFONT_FILE	DEF_APLFONT_EXTNAME DEF_APLFONT_TTF // ... name w/ext
#define DEF_TCFONTNAME		L"Tahoma"           // Default TabCtrl font
#define DEF_ASFONTNAME		L"Arial Unicode MS" // Default Alternate SM font
#define DEF_SMFONTNAME		DEF_APLFONT_INTNAME // Or L"APL385 Unicode"
#define DEF_PRFONTNAME		DEF_APLFONT_INTNAME // Or L"APL385 Unicode"
#define DEF_CCFONTNAME		DEF_APLFONT_INTNAME // Or L"APL385 Unicode"
#define DEF_FEFONTNAME		DEF_APLFONT_INTNAME // Or L"APL385 Unicode"
#define DEF_MEFONTNAME		DEF_APLFONT_INTNAME // Or L"APL385 Unicode"
#define DEF_VEFONTNAME		DEF_APLFONT_INTNAME // Or L"APL385 Unicode"

#define DEF_TCLOGFONT		0,0,0,0,FW_BOLD  ,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,VARIABLE_PITCH | FF_ROMAN ,DEF_TCFONTNAME
#define DEF_SMLOGFONT		0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_SMFONTNAME
#define DEF_PRLOGFONT		0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_PRFONTNAME
#define DEF_CCLOGFONT		0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_CCFONTNAME
#define DEF_FELOGFONT		0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_FEFONTNAME
#define DEF_MELOGFONT		0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_MEFONTNAME
#define DEF_VELOGFONT		0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_VEFONTNAME

#define DEF_TCPTSIZE	   10			// Point size for TC font
#define DEF_SMPTSIZE	   13			// ...			  SM ...
#define DEF_PRPTSIZE	   13			// ...			  PR ...
#define DEF_CCPTSIZE	   13			// ...			  CC ...
#define DEF_FEPTSIZE	   13			// ...			  FE ...
#define DEF_MEPTSIZE	   13			// ...			  ME ...
#define DEF_VEPTSIZE	   13			// ...			  VE ...

#define DEF_INDENT			6			// Prompt indent
#define DEF_TABS			4			// Tab insertion
#define DEF_CURWID_INS		5			// Cursor width for insert mode
#define DEF_CURWID_REP		2			// ...				replace ...
#define DEF_HISTLINES	 3000			// # lines in history buffer
#define DEF_TEXT_FG_COLOR	COLOR_RED
#define DEF_TEXT_BG_COLOR	COLOR_WHITE

// Size of storage areas
#define DEF_NUM_INITSIZE		(	  1024) 				// Initial size for lpszNum
#define DEF_NUM_INCRSIZE		(	  1024) 				// Incremental ...
#define DEF_NUM_MAXSIZE 		(	4*1024) 				// Maximum ...
#define DEF_STR_INITSIZE		(	  1024) 				// Initial size for lpwszStr
#define DEF_STR_INCRSIZE		(	  1024) 				// Incremental ...
#define DEF_STR_MAXSIZE 		(  64*1024) 				// Maximum ...
////ine DEF_TOKENSTACK_INITSIZE (	4*1024) 				// Initial size of token stack
////ine DEF_TOKENSTACK_INCRSIZE (	4*1024) 				// Incremental ...
////ine DEF_TOKENSTACK_MAXSIZE	(  64*1024) 				// Maximum ...
#define DEF_SIS_INITSIZE		(	1*1024) 				// Initial size for State Indicator Stack
#define DEF_SIS_INCRSIZE		(	1*1024) 				// Incremental ..
#define DEF_SIS_MAXSIZE 		(	4*1024) 				// Maximum ...
#define DEF_YYRES_INITSIZE		(	1*1024) 				// Initial size of YYRes buffer
#define DEF_YYRES_INCRSIZE		(	1*1024) 				// Incremental ...
#define DEF_YYRES_MAXSIZE		(  16*1024) 				// Maximum ...
#define DEF_WPTDTEMP_INITSIZE	(	4*1024) 				// Initial size of WCHAR PTD ...
#define DEF_WPTDTEMP_INCRSIZE	(	1*1024) 				// Incremental ...
#define DEF_WPTDTEMP_MAXSIZE	( 256*1024) 				// Maximum ...
#define DEF_WGLBTEMP_INITSIZE	(	4*1024) 				// Initial size of WCHAR GLB ...
#define DEF_WGLBTEMP_INCRSIZE	(	1*1024) 				// Incremental ...
#define DEF_WGLBTEMP_MAXSIZE	(  16*1024) 				// Maximum ...
#define DEF_DEBUG_INITSIZE		(	1*1024) 				// Initial size of debug ...
#define DEF_DEBUG_INCRSIZE		(	1*1024) 				// Incremental ...
#define DEF_DEBUG_MAXSIZE		(  16*1024) 				// Maximum ...
#define DEF_WFORMAT_INITSIZE	(	4*1024) 				// Initial size of WCHAR Formatting storage
#define DEF_WFORMAT_INCRSIZE	(	4*1024) 				// Incremental ...
#define DEF_WFORMAT_MAXSIZE 	(1024*1024) 				// Maximum ...
#define DEF_UNDOBUF_INITSIZE	(	4*1024) 				// Initial size of Undo buffer
#define DEF_UNDOBUF_INCRSIZE	(	4*1024) 				// Incremental ...
#define DEF_UNDOBUF_MAXSIZE 	(  64*1024) 				// Maximum ...
#define DEF_QUADERROR_INITSIZE	(	1*1024) 				// Initial size of []ERROR/[]ES buffer
#define DEF_QUADERROR_INCRSIZE	(	1*1024) 				// Incremental ...
#define DEF_QUADERROR_MAXSIZE	(  16*1024) 				// Maximum ...
#define DEF_STRAND_INITSIZE 	(	1*1024) 				// Initial size in tokens of the strand stack
#define DEF_STRAND_INCRSIZE 	(	1*1024) 				// Incremental ...
#define DEF_STRAND_MAXSIZE		(	4*1024) 				// Maximum ...
#define DEF_DISPLAY_INITSIZE	(	4*1024) 				// Initial size of WCHARs for Array Display
#define DEF_DISPLAY_INCRSIZE	(	4*1024) 				// Incremental ...
#define DEF_DISPLAY_MAXSIZE 	( 256*1024) 				// Maximum ...
#define DEF_GLBHSHTAB_NBLKS 		   128					// Starting # blocks
#define DEF_GLBHSHTAB_EPB				 2					// # entries in each block
#define DEF_GLBHSHTAB_INITSIZE	(DEF_GLBHSHTAB_NBLKS * DEF_GLBHSHTAB_EPB)	// Initial size of global HshTab for {symbol} names & values
#define DEF_GLBHSHTAB_INCRSIZE	(DEF_GLBHSHTAB_INITSIZE)	// Incremental ...
#define DEF_GLBHSHTAB_MAXSIZE	(1024 * DEF_GLBHSHTAB_EPB)	// Maximum ...
#define DEF_GLBHSHTAB_HASHMASK	(DEF_GLBHSHTAB_NBLKS - 1)	// Starting hash mask
#define DEF_GLBHSHTAB_INCR		(DEF_HSHTAB_PRIME % DEF_GLBHSHTAB_INITSIZE)
#define DEF_CS_INITSIZE 		(	4*1024) 				// Initial size of Ctrl Struc token buffer
#define DEF_CS_INCRSIZE 		(	4*1024) 				// Incremental ...
#define DEF_CS_MAXSIZE			(  64*1024) 				// Maximum ...
#define DEF_FORSTMT_INITSIZE	(	1*1024) 				// Initial size of FORSTMT stack
#define DEF_FORSTMT_INCRSIZE	(	1*1024) 				// Incremental ...
#define DEF_FORSTMT_MAXSIZE 	(	1*1024) 				// Maximum ...


// Global Options
#define DEF_ADJUSTPW				TRUE
#define DEF_UNDERBARTOLOWERCASE 	FALSE
#define DEF_NEWTABONCLEAR			TRUE
#define DEF_NEWTABONLOAD			TRUE
#define DEF_USELOCALTIME			TRUE
#define DEF_BACKUPONLOAD			TRUE
#define DEF_BACKUPONSAVE			TRUE
#define DEF_NOCOPYRIGHTMSG			FALSE
#define DEF_DEFAULTPASTE			UNITRANS_NORMAL
#define DEF_DEFAULTCOPY 			UNITRANS_NARS


// Range limits for []vars
#define DEF_RANGELIMIT_CT			TRUE
#define DEF_RANGELIMIT_IC			TRUE
#define DEF_RANGELIMIT_IO			TRUE
#define DEF_RANGELIMIT_PP			TRUE
#define DEF_RANGELIMIT_PW			TRUE
#define DEF_RANGELIMIT_RL			TRUE


// Empty assignment to []vars as Fixed System value (TRUE) or .ini file value (FALSE)
#define DEF_RESETVARS_CT			FALSE
#define DEF_RESETVARS_FC			FALSE
#define DEF_RESETVARS_IC			FALSE
#define DEF_RESETVARS_IO			FALSE
#define DEF_RESETVARS_PP			FALSE
#define DEF_RESETVARS_PW			FALSE
#define DEF_RESETVARS_RL			FALSE


// Date/time formats
#define DATETIME_OUT		L"dd/mm/yyyy hh:mm:ss"
#define DATETIME_FMT		L"%02d/%02d/%04d %2d:%02d:%02d"
#define DATETIME_LEN		(sizeof (DATETIME_OUT) / sizeof (WCHAR))


#ifdef DEBUG
  #define	 APPEND_DEBUG  " (DEBUG)"
  #define WS_APPEND_DEBUG L" (DEBUG)"
#else
  #define	 APPEND_DEBUG
  #define WS_APPEND_DEBUG
#endif

#define    APPNAME		"NARS2000"
#define WS_APPNAME	   L"NARS2000"

#define WS_WKSNAME	L"workspaces"       // Name of Workspaces subdirectory under main dir


#define  MCWNDCLASS 	"MDIClient"     // MDI Client window class
#define LMCWNDCLASS    L"MDIClient"     // MDI Client ...
#define  SMWNDCLASS 	"SMClass"       // Session Manager ...
#define LSMWNDCLASS    L"SMClass"       // Session Manager ...
#ifdef DEBUG
  #define  DBWNDCLASS	"DBClass"       // Debugger     ...
  #define LDBWNDCLASS  L"DBClass"       // Debugger     ...
#endif
#ifdef PERFMONON
  #define  PMWNDCLASS	"PMClass"       // Performance Monitoring ...
  #define LPMWNDCLASS  L"PMClass"       // ...
#endif
#define  FEWNDCLASS 	"FEClass"       // Function Editor ...
#define LFEWNDCLASS    L"FEClass"       // Function Editor ...
#define  MEWNDCLASS 	"MEClass"       // Matrix Editor ...
#define LMEWNDCLASS    L"MEClass"       // Matrix Editor ...
#define  VEWNDCLASS 	"VEClass"       // Vector Editor ...
#define LVEWNDCLASS    L"VEClass"       // Vector Editor ...
#define  ECWNDCLASS 	"ECClass"       // Edit Ctrl ...
#define LECWNDCLASS    L"ECClass"       // Edit Ctrl ...
#define  CCWNDCLASS 	"CCClass"       // Crash Control ...
#define LCCWNDCLASS    L"CCClass"       // Crash Control ...

#define CQUADPR_MT		L'\xFFFF'       // cQuadPR value indicating it's empty

//***************************************************************************
//	Threads
//***************************************************************************

#define TLSTYPE_DB		((LPVOID) 'DB')     // Debugger
#define TLSTYPE_IE		((LPVOID) 'IE')     // Immediate Execution
#define TLSTYPE_MF		((LPVOID) 'MF')     // Master Frame
#define TLSTYPE_PL		((LPVOID) 'PL')     // ParseLine
#define TLSTYPE_TC		((LPVOID) 'TC')     // Tab Control


//***************************************************************************
//	End of File: defines.h
//***************************************************************************