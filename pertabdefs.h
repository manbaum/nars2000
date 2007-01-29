//***************************************************************************
//	NARS2000 -- Per Tab Definitions
//***************************************************************************

//***************************************************************************
//	The variables defined here are also to be saved in
//	  the per tab structure.
//
//	When defining a new variable here, you *MUST* also
//	  define it in PERTABVARS macro.
//***************************************************************************

#ifdef DEFINE_VARS
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN
HWND hWndMC,					// Global MDI Client window handle
	 hWndSM,					// ...	  Session Manager ...
	 hWndDB,					// ...	  Debugger	   ...
	 hWndLB;					// ...	  Debugger's Listbox

EXTERN
WNDPROC lpfnOldListboxWndProc;	// Save area for old Listbox procedure

EXTERN
LPWCHAR lpwszCurLine;			// The contents of the line
								//	 with the cursor on it.
								// This contents of this var are changed
								//	 by editing the line in the window,
								//	 but the contents of lpGlbHist[iCurLine].hGlb
								//	 are not changed
EXTERN
LPCHAR	lpszNumAlp; 			// Accumulator for integers & floating points & names

EXTERN
LPWCHAR lpwszString;			// ...			   strings

EXTERN
int 	iMaxNumAlp
#ifdef DEFINE_VALUES
 = DEF_NUMALP_MAXSIZE			// Maximum # chars in lpszNumAlp
#endif
,
		iMaxString
#ifdef DEFINE_VALUES
 = DEF_STRING_MAXSIZE			// ...		 WCHARs in lpwszString
#endif
;

EXTERN
HGLOBAL hGlbHist;				// Global handle to array of history lines

EXTERN
int cxWindowPixels, 			// Size of SM client area in pixels
	cyWindowPixels, 			// ...
	nWindowChars,				// ...					  in chars
	nWindowLines;				// ...						 lines

EXTERN
BOOL bCurLineChanged;			// Has the current line changed?

/*

The vertical window and history buffer has several
points of interest -- all origin-0 indices into lpGlbHist:

---->		First line in History buffer		iFirstBufferLine (0)

---->		First line in the window			iFirstWindowLine

---->		Current line (always visible)		iCurLine

---->		Last line in the window 			iLastWindowLine

---->		Last line in History buffer 		iLastBufferLine (DEF_HISTLINES-1)

In addition, there is the index of the last valid line in the
history buffer (iLastValidLine), that is, the last line for
which memory is allocated.

iFirstBufferLine <= iCurLine <= (iLastValidLine + 1)
iLastValidLine <= iLastBufferLine


The horizontal window has several points of interest -- all
origin-0 indices into lpwszCurLine (transposed for convenience):

---->		First char in buffer				iFirstBufferChar (0)

---->		First char in the window			iFirstWindowChar

---->		Current char (always visible)		iCurChar

---->		Last char in the window 			iLastWindowChar

---->		Last char in buffer 				iLastBufferChar (DEF_MAXLINELEN - 1)

In addition, there is the index of the last valid char in
lpwszCurLine (iCurLineLen), that is, lstrlenW (lpwszCurLine).

 */

EXTERN
int iFirstBufferLine			// Index into lpGlbHist of the first line
								//	 in the history buffer (ALWAYS 0).
#ifdef DEFINE_VALUES
= 0
#endif
,
	iLastBufferLine 			// Index into lpGlbHist of the last line
								//	 in the history buffer (ALWAYS DEF_HISTLINES-1)
#ifdef DEFINE_VALUES
= DEF_HISTLINES-1
#endif
,
	iFirstWindowLine			// Index into lpGlbHist of the top line
								//	 in the window -- changed by vertical
								//	 scrolling.
#ifdef DEFINE_VALUES
= 0
#endif
,
	iCurLine					// Index into lpGlbHist of the line with
								//	 the cursor on it -- changed by up and
								//	 down cursor movements.
#ifdef DEFINE_VALUES
= 0
#endif
,
	iLastWindowLine,			// Index into lpGlbHist of the bottom line
								//	 in the window -- changed by vertical
								//	 scrolling or window resizing.
	iLastValidLine				// Index into lpGlbHist of the last line
								//	 which is defined (contents are not NULL)
								//	 -- changed by the user pressing Enter
								//	 (and thus executing a statement), or by
								//	 normal program or system command
								//	 output or by error messages.
#ifdef DEFINE_VALUES
= -1
#endif
,
	iFirstBufferChar			// Index into lpwszCurLine of the first char
								//	 in the buffer (ALWAYS 0)
#ifdef DEFINE_VALUES
= 0
#endif
,
	iFirstWindowChar,			// Index into lpwszCurLine of the first char
								//	 in the window -- changed by horizontal
								//	 scrolling.
	iCurChar,					// Horizontal position of the caret (in chars)
								// The vertical position is iCurLine - iFirstWindowLine (in chars).
	iLastWindowChar,			// Index into lpwszCurLine of the last char
								//	 in the window -- changed by horizontal
								//	 scrolling or window resizing.
	iLastBufferChar 			// Index into lpwszCurLine of the last char
								//	 in the buffer (always (DEF_MAXLINELEN - 1))
#ifdef DEFINE_VALUES
= DEF_MAXLINELEN - 1
#endif
,
	iCurLineLen;				// Length of lpwszCurLine (not including the trailing zero)


// Current global values of system variables so we can use them
//	 without having to access the actual system variable.
EXTERN
HGLOBAL  hGlbQuadALX,			// []ALX	([]dm)
		 hGlbQuadELX,			// []ELX	([]dm)
		 hGlbQuadLX,			// []LX 	("")
		 hGlbQuadSA,			// []SA 	("")
		 hGlbQuadWSID,			// []WSID	("")
		 hGlbQuadPR;			// []PR 	("") (When an empty vector)
EXTERN
APLFLOAT fQuadCT;				// []CT
EXTERN
APLBOOL  bQuadDF,				// []DF
		 bQuadIF,				// []IF
		 bQuadIO,				// []IO
		 bQuadxSA;				// []SA (in its index form)
EXTERN
APLINT	 uQuadPP,				// []PP
		 uQuadPW,				// []PW
		 uQuadRL;				// []RL
EXTERN
APLCHAR  cQuadPR;				// []PR 	(' ') (When a char scalar)

// Hash table variables
EXTERN
LPHSHENTRY	lpHshTab,			// Ptr to start of hash table
			lpHshTabSplitNext;	// ...	  next HTE to split (incremented by DEF_HSHTAB_NBLKS)
EXTERN
UINT		uHashMask			// Mask for all hash lookups
#ifdef DEFINE_VALUES
 = DEF_HSHTAB_HASHMASK
#endif
;

// Symbol table variables
EXTERN
LPSYMENTRY	lpSymTab,			// Ptr to start of symbol table
			lpSymTabNext;		// Ptr to next available STE

EXTERN
int 		iSymTabTotalSize	// # STEs currently
#ifdef DEFINE_VALUES
 = DEF_SYMTAB_INITSIZE
#endif
,
			iHshTabTotalSize	// # HTEs currently including EPBs
#ifdef DEFINE_VALUES
 = DEF_HSHTAB_INITSIZE
#endif
,
			iHshTabBaseSize 	// Base size of hash table
#ifdef DEFINE_VALUES
 = DEF_HSHTAB_INITSIZE
#endif
,
			iHshTabIncr 		// Increment when looping through ST
#ifdef DEFINE_VALUES
 = DEF_HSHTAB_INCR
#endif
;

EXTERN
LPWCHAR lpwszErrorMessage;		// Ptr to error message to signal

#ifndef ENUMS_DEFINED
typedef enum tagEXEC_CODES
{
	EXEC_SUCCESS = 0 ,			// All OK
////EXEC_RESULT_BOOL ,			// Result should be Boolean
////EXEC_RESULT_INT  ,			// ...				Integer
	EXEC_RESULT_FLOAT,			// ...				Float
	EXEC_DOMAIN_ERROR,			// Signal a DOMAIN ERROR
} EXEC_CODES;
#endif

EXTERN
EXEC_CODES ExecCode 			// Exception code
#ifdef DEFINE_VALUES
 = {EXEC_SUCCESS}
#endif
;

EXTERN
BOOL bTabTextState; 			// Tab's text state:  Highlight (TRUE) or Normal (FALSE)

#define INIT_PERTABVARS 					\
	iMaxNumAlp		 = DEF_NUMALP_MAXSIZE;	\
	iMaxString		 = DEF_STRING_MAXSIZE;	\
	iFirstBufferLine = 0;					\
	iLastBufferLine  = DEF_HISTLINES-1; 	\
	iFirstWindowLine = 0;					\
	iCurLine		 = 0;					\
	iLastValidLine	 = -1;					\
	iFirstBufferChar = 0;					\
	iLastBufferChar  = DEF_MAXLINELEN - 1;	\
	uHashMask		 = DEF_HSHTAB_HASHMASK; \
	iSymTabTotalSize = DEF_SYMTAB_INITSIZE; \
	iHshTabTotalSize = DEF_HSHTAB_INITSIZE; \
	iHshTabBaseSize  = DEF_HSHTAB_INITSIZE; \
	iHshTabIncr 	 = DEF_HSHTAB_INCR; 	\
	ExecCode		 = EXEC_SUCCESS;		\
	hGlbQuadWSID	 = CopyArray_EM (hGlbQuadWSID_CWS, FALSE, NULL); \
	bTabTextState	 = FALSE;


#ifndef PERTABVARS
#define PERTABVARS									\
	/* Save/restore the various handles and ptrs */ \
	Assign (hWndMC				 ); 				\
	Assign (hWndSM				 ); 				\
	Assign (hWndDB				 ); 				\
	Assign (hWndLB				 ); 				\
	Assign (lpfnOldListboxWndProc); 				\
	Assign (lpwszCurLine		 ); 				\
	Assign (lpszNumAlp			 ); 				\
	Assign (lpwszString 		 ); 				\
	Assign (iMaxNumAlp			 ); 				\
	Assign (iMaxString			 ); 				\
	Assign (hGlbHist			 ); 				\
													\
	/* Save/restore miscellaneous values */ 		\
	Assign (cxWindowPixels		 ); 				\
	Assign (cyWindowPixels		 ); 				\
	Assign (nWindowChars		 ); 				\
	Assign (nWindowLines		 ); 				\
	Assign (bCurLineChanged 	 ); 				\
	Assign (iFirstBufferLine	 ); 				\
	Assign (iLastBufferLine 	 ); 				\
	Assign (iFirstWindowLine	 ); 				\
	Assign (iCurLine			 ); 				\
	Assign (iLastWindowLine 	 ); 				\
	Assign (iLastValidLine		 ); 				\
	Assign (iFirstBufferChar	 ); 				\
	Assign (iFirstWindowChar	 ); 				\
	Assign (iCurChar			 ); 				\
	Assign (iLastWindowChar 	 ); 				\
	Assign (iLastBufferChar 	 ); 				\
	Assign (iCurLineLen 		 ); 				\
													\
	/* Save/restore system variable values */		\
	Assign (hGlbQuadALX 		 ); 				\
	Assign (hGlbQuadELX 		 ); 				\
	Assign (hGlbQuadLX			 ); 				\
	Assign (hGlbQuadSA			 ); 				\
	Assign (hGlbQuadWSID		 ); 				\
	Assign (hGlbQuadPR			 ); 				\
	Assign (fQuadCT 			 ); 				\
	Assign (bQuadDF 			 ); 				\
	Assign (bQuadIF 			 ); 				\
	Assign (bQuadIO 			 ); 				\
	Assign (bQuadxSA			 ); 				\
	Assign (uQuadPP 			 ); 				\
	Assign (uQuadPW 			 ); 				\
	Assign (uQuadRL 			 ); 				\
	Assign (cQuadPR 			 ); 				\
													\
	/* Symbol & Hash table variables */ 			\
	Assign (lpHshTab			 ); 				\
	Assign (lpHshTabSplitNext	 ); 				\
	Assign (uHashMask			 ); 				\
	Assign (lpSymTab			 ); 				\
	Assign (lpSymTabNext		 ); 				\
	Assign (iSymTabTotalSize	 ); 				\
	Assign (iHshTabTotalSize	 ); 				\
	Assign (iHshTabBaseSize 	 ); 				\
	Assign (iHshTabIncr 		 ); 				\
													\
	Assign (lpwszErrorMessage	 ); 				\
	Assign (ExecCode			 ); 				\
	Assign (bTabTextState		 );
#endif

#undef	EXTERN


//***************************************************************************
//	End of File: pertabdefs.h
//***************************************************************************
