//***************************************************************************
//	NARS2000 -- Parser Grammar for function headers
//***************************************************************************

//***************************************************************************
//	Parse a line of pre-tokenized tokens.
//***************************************************************************

%{
#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "dfnhdr.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

#ifdef DEBUG
#define YYERROR_VERBOSE
#define YYDEBUG 1
#define YYFPRINTF				fh_yyfprintf
#endif

#define    YYSTYPE	   FH_YYSTYPE
#define  LPYYSTYPE	 LPFH_YYSTYPE
#define tagYYSTYPE	tagFH_YYSTYPE

// The following #defines are needed to allow multiple parses
//	 to coexist in the same file
#define yy_symbol_print 		fh_yy_symbol_print
#define yy_symbol_value_print	fh_yy_symbol_value_print
#define yy_reduce_print 		fh_yy_reduce_print
#define yydestruct				fh_yydestruct

#define DbgMsgW2(a) DbgMsgW(a)
////#define DbgMsgW2(a) DbgMsgW(a); DbgBrk ()

%}

%pure-parser
%name-prefix="fh_yy"
%parse-param {LPFHLOCALVARS lpfhLocalVars}
%lex-param	 {LPFHLOCALVARS lpfhLocalVars}
%token NAMEUNK NAMEOPR NAMESYS ASSIGN LINECONT UNK

%start Header

%%

/*

The set of cases we must handle is as follows:

Result
------
	1.	**Empty**
	2.	Z{is}
	3.	(Z){is}
	4.	(Z1 Z2 ...){is}


Left Arg
--------
	1.	**Empty**
	2.	L
	3.	(L)
	4.	(L1 L2 ...)
	5.	[L]
	6.	[L1 L2 ...]
	7.	[(L)]
	8.	[(L1 L2 ...)]

Fcn/Opr
-------
	1.	FOO
	2.	(FOO)
	3.	(LO FOO)
	4.	(LO FOO RO)
	5.	FOO[X]
	6.	(FOO[X])
	7.	(LO FOO[X])
	8.	(LO FOO[X] RO)

Right Arg
---------
	1.	R
	2.	(R)
	3.	(R1 R2 ...)

This yields 768 (=4 x 8 x 8 x 3) distinct Monadic/Dyadic Function/Operator headers
and 		  8 (=4 x 1 x 2 x 1) distinct Niladic Function headers
for a total of 776 (=768 + 8) Defined Function headers, not counting locals.

 */


AxisOpr:
	  NAMEOPR '[' NAMEUNK ']'   {DbgMsgW2 (L"%%AxisOpr:  NAMEUNK[NAMEUNK]");
								 InitHdrStrand (&$1);
								 PushHdrStrand (&$1);
								 PushHdrStrand (&$3);
								 $$ = *MakeHdrStrand (&$1);
								 lpfhLocalVars->DfnAxis  = 1;
								 lpfhLocalVars->lpYYAxisOpr = $3.lpYYStrandIndirect;
								 $$ = $1;
								}
	;

OpenList:
			   NAMEUNK			{DbgMsgW2 (L"%%OpenList:  NAMEUNK");
								 InitHdrStrand (&$1);
								 $$ = *PushHdrStrand (&$1);
								}
	| OpenList NAMEUNK			{DbgMsgW2 (L"%%OpenList:  OpenList NAMEUNK");
								 $$ = *PushHdrStrand (&$2);
								}
	;

AxisList:
	  '('          AxisOpr          ')'
								{DbgMsgW2 (L"%%AxisList:  (AxisOpr)");
								 InitHdrStrand (&$2);
								 $2.Indirect = 1;
								 PushHdrStrand (&$2);
								 $$ = *MakeHdrStrand (&$2);
								}
	| '(' NAMEUNK  AxisOpr          ')'
								{DbgMsgW2 (L"%%AxisList:  (NAMEUNK AxisOpr)");
								 PushHdrStrand (&$2);
								 $3.Indirect = 1;
								 PushHdrStrand (&$3);
								 $$ = *MakeHdrStrand (&$2);
								}
	| '(' NAMEUNK  AxisOpr NAMEUNK ')'
								{DbgMsgW2 (L"%%AxisList:  (NAMEUNK AxisOpr NAMEUNK)");
								 PushHdrStrand (&$2);
								 $3.Indirect = 1;
								 PushHdrStrand (&$3);
								 PushHdrStrand (&$4);
								 $$ = *MakeHdrStrand (&$2);
								}
	;

List:
	  '(' OpenList ')'          {DbgMsgW2 (L"%%List:  '(' OpenList ')'");
								 $$ = *MakeHdrStrand (&$2);
								}
	;

Result:
	  List	  ASSIGN			{DbgMsgW2 (L"%%Result:  List" WS_UTF16_LEFTARROW);
								 lpfhLocalVars->lpYYResult = $1.lpYYStrandBase;
								 $$ = $1;
								}
	| NAMEUNK ASSIGN			{DbgMsgW2 (L"%%Result:  NAMEUNK" WS_UTF16_LEFTARROW);
								 InitHdrStrand (&$1);
								 PushHdrStrand (&$1);
								 $$ = *MakeHdrStrand (&$1);
								 lpfhLocalVars->lpYYResult = $1.lpYYStrandBase;
								}
	;

OptArg:
	  '[' List     ']'          {DbgMsgW2 (L"%%OptArg:  [List]");
								 $$ = $2;
								}
	| '[' OpenList ']'          {DbgMsgW2 (L"%%OptArg:  [OpenList]");
								 $$ = *MakeHdrStrand (&$2);
								}
	| '(' '[' OpenList ']' ')'  {DbgMsgW2 (L"%%OptArg:  ([OpenList])");
								 $$ = *MakeHdrStrand (&$3);
								}
	;

RhtArg:
	  List						{DbgMsgW2 (L"%%RhtArg:  List");
								 $$ = $1;
								}
	| NAMEUNK					{DbgMsgW2 (L"%%RhtArg:  NAMEUNK");
								 InitHdrStrand (&$1);
								 PushHdrStrand (&$1);
								 $$ = *MakeHdrStrand (&$1);
								}
	;

NoResHdr:						// N.B. that this production does not need to return a result (in $$)
								//	 because it calculates all the information we need into
								//	 lpfhLocalVars->lpYY... and lpfhLocalVars->u???Valence
			  NAMEUNK			{DbgMsgW2 (L"%%NoResHdr:  NAMEUNK");                // Niladic function
								 InitHdrStrand (&$1);
								 PushHdrStrand (&$1);
								 MakeHdrStrand (&$1);

								 lpfhLocalVars->lpYYFcnName = $1.lpYYStrandBase;
								 lpfhLocalVars->DfnType 	= DFNTYPE_FCN;			// Mark as a function
								 lpfhLocalVars->FcnValence	= FCNVALENCE_NIL;		// Mark as niladic
								}

	|		  AxisOpr  RhtArg	{DbgMsgW2 (L"%%NoResHdr:  AxisOpr RhtArg");         // Monadic function w/axis operator
								 InitHdrStrand (&$1);
								 PushHdrStrand (&$1);
								 MakeHdrStrand (&$1);

								 lpfhLocalVars->lpYYFcnName = $1.lpYYStrandBase;
								 lpfhLocalVars->lpYYRhtArg	= $2.lpYYStrandBase;
								 lpfhLocalVars->DfnType 	= DFNTYPE_FCN;			// Mark as a function
								 lpfhLocalVars->FcnValence	= FCNVALENCE_MON;		// Mark as monadic
								}

	|		  NAMEUNK  RhtArg	{DbgMsgW2 (L"%%NoResHdr:  NAMEUNK RhtArg");         // Monadic function
								 InitHdrStrand (&$1);
								 PushHdrStrand (&$1);
								 MakeHdrStrand (&$1);

								 lpfhLocalVars->lpYYFcnName = $1.lpYYStrandBase;
								 lpfhLocalVars->lpYYRhtArg	= $2.lpYYStrandBase;
								 lpfhLocalVars->DfnType 	= DFNTYPE_FCN;			// Mark as a function
								 lpfhLocalVars->FcnValence	= FCNVALENCE_MON;		// Mark as monadic
								}

	| NAMEUNK AxisOpr  RhtArg	{DbgMsgW2 (L"%%NoResHdr:  NAMEUNK AxisOpr RhtArg"); // Dyadic function w/axis operator
								 InitHdrStrand (&$1);
								 PushHdrStrand (&$1);
								 MakeHdrStrand (&$1);

								 InitHdrStrand (&$2);
								 PushHdrStrand (&$2);
								 MakeHdrStrand (&$2);

								 lpfhLocalVars->lpYYLftArg	= $1.lpYYStrandBase;
								 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
								 lpfhLocalVars->lpYYRhtArg	= $3.lpYYStrandBase;
								 lpfhLocalVars->DfnType 	= DFNTYPE_FCN;			// Mark as a function
								 lpfhLocalVars->FcnValence	= FCNVALENCE_DYD;		// Mark as dyadic
								}
	| NAMEUNK NAMEUNK  RhtArg	{DbgMsgW2 (L"%%NoResHdr:  NAMEUNK NAMEUNK RhtArg"); // Dyadic function
								 InitHdrStrand (&$1);
								 PushHdrStrand (&$1);
								 MakeHdrStrand (&$1);

								 InitHdrStrand (&$2);
								 PushHdrStrand (&$2);
								 MakeHdrStrand (&$2);

								 lpfhLocalVars->lpYYLftArg	= $1.lpYYStrandBase;
								 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
								 lpfhLocalVars->lpYYRhtArg	= $3.lpYYStrandBase;
								 lpfhLocalVars->DfnType 	= DFNTYPE_FCN;			// Mark as a function
								 lpfhLocalVars->FcnValence	= FCNVALENCE_DYD;		// Mark as dyadic
								}
	| List	  AxisOpr  RhtArg	{DbgMsgW2 (L"%%NoResHdr:  List AxisOpr RhtArg");    // Dyadic function w/axis operator
								 InitHdrStrand (&$2);
								 PushHdrStrand (&$2);
								 MakeHdrStrand (&$2);

								 lpfhLocalVars->lpYYLftArg	= $1.lpYYStrandBase;
								 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
								 lpfhLocalVars->lpYYRhtArg	= $3.lpYYStrandBase;
								 lpfhLocalVars->DfnType 	= DFNTYPE_FCN;			// Mark as a function
								 lpfhLocalVars->FcnValence	= FCNVALENCE_DYD;		// Mark as dyadic
								}
	| List	  NAMEUNK  RhtArg	{DbgMsgW2 (L"%%NoResHdr:  List NAMEUNK RhtArg");    // Dyadic function
								 InitHdrStrand (&$2);
								 PushHdrStrand (&$2);
								 MakeHdrStrand (&$2);

								 lpfhLocalVars->lpYYLftArg	= $1.lpYYStrandBase;
								 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
								 lpfhLocalVars->lpYYRhtArg	= $3.lpYYStrandBase;
								 lpfhLocalVars->DfnType 	= DFNTYPE_FCN;			// Mark as a function
								 lpfhLocalVars->FcnValence	= FCNVALENCE_DYD;		// Mark as dyadic
								}
	| OptArg  AxisOpr  RhtArg	{DbgMsgW2 (L"%%NoResHdr:  OptArg AxisOpr RhtArg");  // Bivalent function w/axis operator
								 InitHdrStrand (&$2);
								 PushHdrStrand (&$2);
								 MakeHdrStrand (&$2);

								 lpfhLocalVars->lpYYLftArg	= $1.lpYYStrandBase;
								 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
								 lpfhLocalVars->lpYYRhtArg	= $3.lpYYStrandBase;
								 lpfhLocalVars->DfnType 	= DFNTYPE_FCN;			// Mark as a function
								 lpfhLocalVars->FcnValence	= FCNVALENCE_AMB;		// Mark as ambivalent
								}
	| OptArg  NAMEUNK  RhtArg	{DbgMsgW2 (L"%%NoResHdr:  OptArg NAMEUNK RhtArg");  // Bivalent function
								 InitHdrStrand (&$2);
								 PushHdrStrand (&$2);
								 MakeHdrStrand (&$2);

								 lpfhLocalVars->lpYYLftArg	= $1.lpYYStrandBase;
								 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
								 lpfhLocalVars->lpYYRhtArg	= $3.lpYYStrandBase;
								 lpfhLocalVars->DfnType 	= DFNTYPE_FCN;			// Mark as a function
								 lpfhLocalVars->FcnValence	= FCNVALENCE_AMB;		// Mark as ambivalent
								}
	|		 AxisList  RhtArg	{DbgMsgW2 (L"%%NoResHdr:  AxisList RhtArg");        // Mon/Dyd operator, monadic derived function w/axis operator
								 if (!GetOprName (&$1))
									 YYERROR;

								 lpfhLocalVars->lpYYRhtArg	= $2.lpYYStrandBase;
								 lpfhLocalVars->FcnValence	= FCNVALENCE_MON;		// Mark as monadic
								}
	|		  List	   RhtArg	{DbgMsgW2 (L"%%NoResHdr:  List RhtArg");            // Mon/Dyd operator, monadic derived function
								 if (!GetOprName (&$1))
									 YYERROR;

								 lpfhLocalVars->lpYYRhtArg	= $2.lpYYStrandBase;
								 lpfhLocalVars->FcnValence	= FCNVALENCE_MON;		// Mark as monadic
								}
	| NAMEUNK AxisList RhtArg	{DbgMsgW2 (L"%%NoResHdr:  NAMEUNK AxisList RhtArg");// Mon/Dyd operator, dyadic derived function w/axis operator
								 if (!GetOprName (&$2))
									 YYERROR;

								 lpfhLocalVars->lpYYLftArg	= $1.lpYYStrandBase;
								 lpfhLocalVars->lpYYRhtArg	= $3.lpYYStrandBase;
								 lpfhLocalVars->FcnValence	= FCNVALENCE_DYD;		// Mark as dyadic
								}
	| NAMEUNK List	   RhtArg	{DbgMsgW2 (L"%%NoResHdr:  NAMEUNK List RhtArg");    // Mon/Dyd operator, dyadic derived function
								 if (!GetOprName (&$2))
									 YYERROR;

								 lpfhLocalVars->lpYYLftArg	= $1.lpYYStrandBase;
								 lpfhLocalVars->lpYYRhtArg	= $3.lpYYStrandBase;
								 lpfhLocalVars->FcnValence	= FCNVALENCE_DYD;		// Mark as dyadic
								}
	| List	  AxisList RhtArg	{DbgMsgW2 (L"%%NoResHdr:  List AxisList RhtArg");   // Mon/Dyd operator, dyadic derived function w/axis operator
								 if (!GetOprName (&$2))
									 YYERROR;

								 lpfhLocalVars->lpYYLftArg	= $1.lpYYStrandBase;
								 lpfhLocalVars->lpYYRhtArg	= $3.lpYYStrandBase;
								 lpfhLocalVars->FcnValence	= FCNVALENCE_DYD;		// Mark as dyadic
								}
	| List	  List	   RhtArg	{DbgMsgW2 (L"%%NoResHdr:  List List RhtArg");       // Mon/Dyd operator, dyadic derived function
								 if (!GetOprName (&$2))
									 YYERROR;

								 lpfhLocalVars->lpYYLftArg	= $1.lpYYStrandBase;
								 lpfhLocalVars->lpYYRhtArg	= $3.lpYYStrandBase;
								 lpfhLocalVars->FcnValence	= FCNVALENCE_DYD;		// Mark as dyadic
								}
	| OptArg  AxisList RhtArg	{DbgMsgW2 (L"%%NoResHdr:  OptArg AxisList RhtArg"); // Mon/Dyd operator, ambivalent derived function w/axis operator
								 if (!GetOprName (&$2))
									 YYERROR;

								 lpfhLocalVars->lpYYLftArg	= $1.lpYYStrandBase;
								 lpfhLocalVars->lpYYRhtArg	= $3.lpYYStrandBase;
								 lpfhLocalVars->FcnValence	= FCNVALENCE_AMB;		// Mark as ambivalent
								}
	| OptArg  List	   RhtArg	{DbgMsgW2 (L"%%NoResHdr:  OptArg List RhtArg");     // Mon/Dyd operator, ambivalent derived function
								 if (!GetOprName (&$2))
									 YYERROR;

								 lpfhLocalVars->lpYYLftArg	= $1.lpYYStrandBase;
								 lpfhLocalVars->lpYYRhtArg	= $3.lpYYStrandBase;
								 lpfhLocalVars->FcnValence	= FCNVALENCE_AMB;		// Mark as ambivalent
								}
	;

Locals:
			  LINECONT			{DbgMsgW2 (L"%%Locals:  LINECONT");
								}
	|		  ';'      NAMEUNK  {DbgMsgW2 (L"%%Locals:  ':' NAMEUNK");
								 InitHdrStrand (&$2);
								 $$ = *PushHdrStrand (&$2);
								}
	|		  ';'      NAMESYS  {DbgMsgW2 (L"%%Locals:  ':' NAMESYS");
								 if (!$2.tkToken.tkData.tkSym->stFlags.Value)
									 YYERROR;
								 InitHdrStrand (&$2);
								 $$ = *PushHdrStrand (&$2);
								}
	| Locals  LINECONT			{DbgMsgW2 (L"%%Locals:  Locals LINECONT");
								}
	| Locals  ';'      NAMEUNK  {DbgMsgW2 (L"%%Locals:  Locals ';' NAMEUNK");
								 $$ = *PushHdrStrand (&$3);
								}
	| Locals  ';'      NAMESYS  {DbgMsgW2 (L"%%Locals:  Locals ':' NAMESYS");
								 if (!$3.tkToken.tkData.tkSym->stFlags.Value)
									 YYERROR;
								 $$ = *PushHdrStrand (&$3);
								}
	;

Header:
		/* Empty */ 			{DbgMsgW2 (L"%%Header:  <empty>");
#ifdef DEBUG
								 DisplayFnHdr (lpfhLocalVars);
#endif
								}
	|		  error 			{DbgMsgW2 (L"%%Header:  error");
								 YYABORT;
								}

	|		  NoResHdr			{DbgMsgW2 (L"%%Header:  NoResHdr");
#ifdef DEBUG
								 DisplayFnHdr (lpfhLocalVars);
#endif
								}
	|		  NoResHdr Locals	{DbgMsgW2 (L"%%Header:  NoResHdr Locals");
								 lpfhLocalVars->lpYYLocals = MakeHdrStrand (&$2);
#ifdef DEBUG
								 DisplayFnHdr (lpfhLocalVars);
#endif
								}
	| Result  NoResHdr			{DbgMsgW2 (L"%%Header:  Result NoResHdr");
#ifdef DEBUG
								 DisplayFnHdr (lpfhLocalVars);
#endif
								}
	| Result  NoResHdr Locals	{DbgMsgW2 (L"%%Header:  Result NoResHdr Locals");
								 lpfhLocalVars->lpYYLocals = MakeHdrStrand (&$3);
#ifdef DEBUG
								 DisplayFnHdr (lpfhLocalVars);
#endif
								}
	;

%%

//***************************************************************************
//	Start of C program
//***************************************************************************

//***************************************************************************
//	$ParseHeader
//
//	Parse a function header
//***************************************************************************

BOOL ParseHeader
	(HWND		   hWndEC,			// Window handle of Edit Control
	 HGLOBAL	   hGlbTknHdr,		// Tokenized header global memory handle
	 LPFHLOCALVARS lpfhLocalVars,	// Ptr to Local vars
	 BOOL		   DisplayErr)		// TRUE iff want error messages displayed

{
	BOOL bRet = FALSE,			// TRUE iff result is valid
		 OldDisplayErr; 		// Save area for old DisplayErr

	// Save the window handle
	lpfhLocalVars->hWndEC = hWndEC;

	// Save the error display flag
	OldDisplayErr = lpfhLocalVars->DisplayErr;
	lpfhLocalVars->DisplayErr = DisplayErr;

	// Save the thread's ptr to local vars
	TlsSetValue (dwTlsFhLocalVars, (LPVOID) lpfhLocalVars);

	// Save the token header global memory handle
	lpfhLocalVars->hGlbTknHdr = hGlbTknHdr;

	// Lock the handle to get a ptr to it, and set the variables
	UTLockAndSet (lpfhLocalVars->hGlbTknHdr, &lpfhLocalVars->t2);

	// Initialize the base & next strand ptrs
	lpfhLocalVars->lpYYStrandBase =
	lpfhLocalVars->lpYYStrandNext = lpfhLocalVars->lpYYStrandStart;

	// Skip over TOKEN_HEADER
	lpfhLocalVars->lpStart = TokenBaseToStart (lpfhLocalVars->t2.lpBase);

	// Skip over the starting EOL
	lpfhLocalVars->lpNext  = &lpfhLocalVars->lpStart[1];

	// Mark the stopping point
	lpfhLocalVars->lpStop  = &lpfhLocalVars->lpStart[lpfhLocalVars->lpStart->tkData.tkChar];

	// Start off with no error
	lpfhLocalVars->tkErrorCharIndex = NEG1U;

	// Allocate virtual memory for the Header Strand accumulator
	lpfhLocalVars->lpYYStrandStart =
		VirtualAlloc (NULL, 	 // Any address
					  DEF_STRAND_MAXSIZE * sizeof (FH_YYSTYPE),
					  MEM_RESERVE,
					  PAGE_READWRITE);
	if (!lpfhLocalVars->lpYYStrandStart)
	{
		// ***FIXME*** -- WS FULL before we got started???
		DbgMsg ("ParseHeader:  VirtualAlloc for <lpfhLocalVars->lpYYStrandStart> failed");

		goto ERROR_EXIT;	// Mark as failed
	} // End IF

	// Commit the intial size
	VirtualAlloc (lpfhLocalVars->lpYYStrandStart,
				  DEF_STRAND_INITSIZE * sizeof (FH_YYSTYPE),
				  MEM_COMMIT,
				  PAGE_READWRITE);

	// Use VirtualAlloc for the parser stack
	// ***FIXME***














#if YYDEBUG
	// Enable debugging
	yydebug = 1;
#endif

	// Parse the header, check for errors
	bRet = fh_yyparse (lpfhLocalVars) EQ 0;

	// Restore the error display flag
	lpfhLocalVars->DisplayErr = OldDisplayErr;
ERROR_EXIT:
	if (lpfhLocalVars->lpYYStrandStart)
	{
		VirtualFree (lpfhLocalVars->lpYYStrandStart, 0, MEM_RELEASE); lpfhLocalVars->lpYYStrandStart = NULL;
	} // End IF

	// We no longer need this ptr
	MyGlobalUnlock (lpfhLocalVars->hGlbTknHdr); lpfhLocalVars->t2.lpBase = NULL;

	return bRet;
} // End ParseHeader


//***************************************************************************
//	$ValidSysName
//
//	Validate the name as a system variable name
//***************************************************************************

BOOL ValidSysName
	(LPTOKEN lptkName)		// Ptr to name token

{
	return (lptkName->tkData.tkSym->stFlags.ObjName EQ OBJNAME_SYS);
} // End ValidSysName


//***************************************************************************
//	$fh_yylex
//
//	Lexical analyzer for Bison
//***************************************************************************

int fh_yylex
	(LPFH_YYSTYPE  lpYYLval,			// Ptr to lval
	 LPFHLOCALVARS lpfhLocalVars)		// Ptr to local fhLocalVars

{
	// Return the current token
	lpYYLval->tkToken			 = *lpfhLocalVars->lpNext;
	lpYYLval->uStrandLen		 =
	lpYYLval->Indirect			 = 0;
	lpYYLval->lpYYStrandIndirect = NULL;
	lpYYLval->lpYYStrandBase	 = lpfhLocalVars->lpYYStrandBase;

	// Check for stopping point
	if (lpfhLocalVars->lpStop EQ lpfhLocalVars->lpNext)
		return '\0';

	// Split cases based upon the token type
	switch (lpfhLocalVars->lpNext++->tkFlags.TknType)
	{
		case TKT_VARNAMED:
			// If the token is a sysname, return NAMESYS
			if (lpfhLocalVars->lpNext[-1].tkData.tkSym->stFlags.ObjName EQ OBJNAME_SYS)
				return NAMESYS;
			else
			{
				// If the next token is a left bracket, return NAMEOPR
				if (lpfhLocalVars->lpNext->tkFlags.TknType EQ TKT_LBRACKET)
					return NAMEOPR;
				else
					return NAMEUNK;
			} // End IF

		case TKT_ASSIGN:
			return ASSIGN;

		case TKT_LISTSEP:
			return ';';

		case TKT_LPAREN:
			return '(';

		case TKT_RPAREN:
			return ')';

		case TKT_LBRACKET:
			return '[';

		case TKT_RBRACKET:
			return ']';

		default:
			return UNK;
	} // End SWITCH
} // End fh_yylex


//***************************************************************************
//	$fh_yyerror
//
//	Error callback from Bison
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- fh_yyerror"
#else
#define APPEND_NAME
#endif

void fh_yyerror 					// Called for Bison syntax error
	(LPFHLOCALVARS lpfhLocalVars,	// Ptr to local fhLocalVars
	 LPCHAR 	   s)				// Ptr to error msg

{
	char szTemp[10244], *p;
	UINT uCharIndex;

#ifdef DEBUG
	DbgMsg (s);
#endif

	// Get and save the character index position
	uCharIndex = lpfhLocalVars->lpNext->tkCharIndex;
	lpfhLocalVars->tkErrorCharIndex = uCharIndex;

	// Check for SYNTAX ERROR
#define ERR 	"syntax error"
	lstrcpyn (szTemp, s, sizeof (ERR)); 	// Note: Terminates the string, too
	if (lstrcmp (szTemp, ERR) EQ 0)
	{
		wsprintf (szTemp, "Syntax Error in line 0, position %d -- function NOT saved.", uCharIndex);
		p = szTemp;

		goto DISPLAYCAT;
#undef	ERR
	} // End IF

#define ERR 	"memory exhausted"
	lstrcpyn (szTemp, s, sizeof (ERR)); 	// Note: Terminates the string, too
	if (lstrcmp (szTemp, ERR) EQ 0)
	{
		wsprintf (szTemp, "Insufficient memory -- function NOT saved.");
		p = szTemp;

		goto DISPLAYCAT;
#undef	ERR
	} // End IF

	// Use the error message as given
	p = s;

	goto DISPLAY;

DISPLAYCAT:
#ifdef DEBUG
	lstrcat (szTemp, "(");
	lstrcat (szTemp,  s );
	lstrcat (szTemp, ")");
#endif
DISPLAY:
	// Display a message box
	MessageBox (lpfhLocalVars->hWndEC,
				p,
				lpszAppName,
				MB_OK | MB_ICONWARNING | MB_APPLMODAL);
} // End fh_yyerror
#undef	APPEND_NAME


//***************************************************************************
//	$fh_yyfprintf
//
//	Debugger output
//***************************************************************************

void fh_yyfprintf
	(FILE  *hfile,			// Ignore this
	 LPCHAR lpszFmt,		// Format string
	 ...)					// Zero or more arguments

{
////#ifdef DEBUG
#if FALSE
	va_list vl;
	int 	i1, i2, i3;
	static char szTemp[256] = {'\0'};

	va_start (vl, lpszFmt);

	// Bison uses no more than three arguments.
	// Note we must grab them separately this way
	//	 as using va_arg in the argument list to
	//	 wsprintf pushes the arguments in reverse
	//	 order.
	i1 = va_arg (vl, int);
	i2 = va_arg (vl, int);
	i3 = va_arg (vl, int);

	va_end (vl);

	wsprintf (lpszDebug,
			  lpszFmt,
			  i1, i2, i3);
	// Accumulate into local buffer because
	//	 Bison calls this function multiple
	//	 times for the same line, terminating
	//	 the last call for the line with a LF.
	lstrcat (szTemp, lpszDebug);

	// Check last character.
	i1 = lstrlen (szTemp);

	// If it's a LF, it's time to flush the buffer.
	if (szTemp[i1 - 1] EQ '\n')
	{
		szTemp[i1 - 1] = '\0';  // Remove trailing LF
								//	 because we're displaying
								//	 in a GUI.
		DbgMsg (szTemp);		// Display in my debugger window.

		szTemp[0] = '\0';       // Restart the buffer
	} // End IF/ELSE
#endif
} // End fh_yyfprintf


//***************************************************************************
//	$InitHdrStrand
//
//	Initialize the header strand stack.
//***************************************************************************

void InitHdrStrand
	(LPFH_YYSTYPE lpYYArg)			// Ptr to the incoming argument

{
	LPFHLOCALVARS lpfhLocalVars;	// Ptr to local fhLocalVars

	// Get this thread's LocalVars ptr
	lpfhLocalVars = (LPFHLOCALVARS) TlsGetValue (dwTlsFhLocalVars);

	// Set the base of this strand to the next available location
	lpYYArg->lpYYStrandBase 	  =
	lpfhLocalVars->lpYYStrandBase = lpfhLocalVars->lpYYStrandNext;
} // End InitHdrStrand


//***************************************************************************
//	$PushHdrStrand_YY
//
//	Push a header token onto the strand stack.
//***************************************************************************

LPFH_YYSTYPE PushHdrStrand
	(LPFH_YYSTYPE lpYYArg)			// Ptr to the incoming argument

{
	LPFHLOCALVARS lpfhLocalVars;	// Ptr to local plLocalVars

	// Get this thread's LocalVars ptr
	lpfhLocalVars = (LPFHLOCALVARS) TlsGetValue (dwTlsFhLocalVars);

	// Copy the strand base to the result
	lpYYArg->lpYYStrandBase = lpfhLocalVars->lpYYStrandBase;

	// Return our own position so the next user
	//	 of this token can refer to it.
	if (!lpYYArg->lpYYStrandIndirect)
		lpYYArg->lpYYStrandIndirect = lpfhLocalVars->lpYYStrandNext;

	// Save this token on the strand stack
	//	 and skip over it
	*lpfhLocalVars->lpYYStrandNext++ = *lpYYArg;

	return lpYYArg;
} // End PushHdrStrand


//***************************************************************************
//	$MakeHdrStrand
//
//	Make a header strand
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- MakeHdrStrand"
#else
#define APPEND_NAME
#endif

LPFH_YYSTYPE MakeHdrStrand
	(LPFH_YYSTYPE lpYYArg)			// Ptr to incoming token

{
	LPFHLOCALVARS lpfhLocalVars;	// Ptr to local fhLocalVars

	// Get this thread's LocalVars ptr
	lpfhLocalVars = (LPFHLOCALVARS) TlsGetValue (dwTlsFhLocalVars);

	// Save the # elements in the strand
	lpYYArg->lpYYStrandBase->uStrandLen =
	  lpfhLocalVars->lpYYStrandNext - lpYYArg->lpYYStrandBase;

	// Save the next position as the new base
	lpfhLocalVars->lpYYStrandBase = lpfhLocalVars->lpYYStrandNext;

	// Return the base of the current strand
	return lpYYArg->lpYYStrandBase;
} // End MakeHdrStrand
#undef	APPEND_NAME


//***************************************************************************
//	$GetOprName
//
//	Extract the function/operator/operand names from a list
//***************************************************************************

BOOL GetOprName
	(LPFH_YYSTYPE lpYYArg)

{
	LPFHLOCALVARS lpfhLocalVars;	// Ptr to local fhLocalVars

	// Get this thread's LocalVars ptr
	lpfhLocalVars = (LPFHLOCALVARS) TlsGetValue (dwTlsFhLocalVars);

	// Split cases based upon the strand length
	switch (lpYYArg->uStrandLen)
	{
		case 1: 		// Function
			lpfhLocalVars->DfnType	   = DFNTYPE_FCN;
			lpfhLocalVars->lpYYFcnName = &lpYYArg->lpYYStrandBase[0];

			return TRUE;

		case 2: 		// Monadic operator
			lpfhLocalVars->DfnType	   = DFNTYPE_OP1;
			lpfhLocalVars->lpYYFcnName = &lpYYArg->lpYYStrandBase[0];
			lpfhLocalVars->lpYYRhtOpr  = &lpYYArg->lpYYStrandBase[1];

			return TRUE;

		case 3: 		// Dyadic operator
			lpfhLocalVars->DfnType	   = DFNTYPE_OP2;
			lpfhLocalVars->lpYYLftOpr  = &lpYYArg->lpYYStrandBase[0];
			lpfhLocalVars->lpYYFcnName = &lpYYArg->lpYYStrandBase[1];
			lpfhLocalVars->lpYYRhtOpr  = &lpYYArg->lpYYStrandBase[2];

			return TRUE;

		default:
			if (lpfhLocalVars->DisplayErr)
			{
				lpfhLocalVars->lpNext->tkCharIndex = lpYYArg->tkToken.tkCharIndex;
				fh_yyerror (lpfhLocalVars, "syntax error");
			} // End IF

			return FALSE;
	} // End SWITCH
} // End GetOprName


//***************************************************************************
//	End of File: dfnhdr.y
//***************************************************************************
