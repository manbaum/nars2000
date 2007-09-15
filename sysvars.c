//***************************************************************************
//	NARS2000 -- System Variable Routines
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "sysvars.h"
#include "externs.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#else
#define SysFnAV_EM		NULL
#define SysFnCR_EM		NULL
#define SysFnDL_EM		NULL
#define SysFnDM_EM		NULL
#define SysFnDR_EM		NULL
#define SysFnLC_EM		NULL
#define SysFnNC_EM		NULL
#define SysFnNL_EM		NULL
#define SysFnSIZE_EM	NULL
#define SysFnSYSID_EM	NULL
#define SysFnSYSVER_EM	NULL
#define SysFnTC_EM		NULL
#define SysFnTCBEL_EM	NULL
#define SysFnTCBS_EM	NULL
#define SysFnTCDEL_EM	NULL
#define SysFnTCESC_EM	NULL
#define SysFnTCFF_EM	NULL
#define SysFnTCHT_EM	NULL
#define SysFnTCLF_EM	NULL
#define SysFnTCNL_EM	NULL
#define SysFnTCNUL_EM	NULL
#define SysFnTS_EM		NULL
#define SysFnTYPE_EM	NULL
#define SysFnUCS_EM 	NULL
#endif

typedef struct tagSYSNAME
{
	LPWCHAR 	lpwszName;		// The name
	UINT		uValence;		// For system functions, Niladic(0), All others (1)
	BOOL		bSysVar;		// Izit a system variable (TRUE) or function (FALSE)?  If TRUE, uValence is ignored
	LPPRIMFNS	lpNameFcn;		// Ptr to execution routine
	SYS_VARS	sysVarIndex;	// Value of each System Var (0 = Unknown)
} SYSNAME, *LPSYSNAME;

#define SYSLBL		8
#define SYSVAR		9

SYSNAME aSystemNames[] =
{ // Name						Valence 	  Var?	 Exec Routine	 SYS_VARS
	{WS_UTF16_QUAD L"alx"      , SYSVAR,      TRUE , NULL          , SYSVAR_ALX },  //  0:  Attention Latent Expression
	{WS_UTF16_QUAD L"ct"       , SYSVAR,      TRUE , NULL          , SYSVAR_CT  },  //  1:  Comparison Tolerance
	{WS_UTF16_QUAD L"elx"      , SYSVAR,      TRUE , NULL          , SYSVAR_ELX },  //  2:  Error Latent Expression
	{WS_UTF16_QUAD L"io"       , SYSVAR,      TRUE , NULL          , SYSVAR_IO  },  //  3:  Index Origin
	{WS_UTF16_QUAD L"lx"       , SYSVAR,      TRUE , NULL          , SYSVAR_LX  },  //  4:  Latent Expression
	{WS_UTF16_QUAD L"pp"       , SYSVAR,      TRUE , NULL          , SYSVAR_PP  },  //  5:  Printing Precision
	{WS_UTF16_QUAD L"pr"       , SYSVAR,      TRUE , NULL          , SYSVAR_PR  },  //  6:  Prompt Replacement
	{WS_UTF16_QUAD L"pw"       , SYSVAR,      TRUE , NULL          , SYSVAR_PW  },  //  7:  Printing Width
	{WS_UTF16_QUAD L"rl"       , SYSVAR,      TRUE , NULL          , SYSVAR_RL  },  //  8:  Random Link
	{WS_UTF16_QUAD L"sa"       , SYSVAR,      TRUE , NULL          , SYSVAR_SA  },  //  9:  Stop Action
	{WS_UTF16_QUAD L"wsid"     , SYSVAR,      TRUE , NULL          , SYSVAR_WSID},  // 10:  Workspace Identifier
	{WS_UTF16_QUAD L"prototype", SYSLBL,      TRUE , NULL          , 0          },  // 11:  User-defined function/operator entry point for Prototype
	{WS_UTF16_QUAD L"inverse"  , SYSLBL,      TRUE , NULL          , 0          },  // 12:  ...                                            Inverse
	{WS_UTF16_QUAD L"singleton", SYSLBL,      TRUE , NULL          , 0          },  // 13:  ...                                            Singleton

	{WS_UTF16_QUAD L"av"       ,      0,      FALSE, SysFnAV_EM    , 0          },  // Atomic Vector
	{WS_UTF16_QUAD L"dm"       ,      0,      FALSE, SysFnDM_EM    , 0          },  // Diagnostic Message
	{WS_UTF16_QUAD L"lc"       ,      0,      FALSE, SysFnLC_EM    , 0          },  // Line Counter
////{WS_UTF16_QUAD L"si"       ,      0,      FALSE, SysFnSI_EM    , 0          },  // State Indicator
////{WS_UTF16_QUAD L"sinl"     ,      0,      FALSE, SysFnSINL_EM  , 0          },  // State Indicator w/Name List
	{WS_UTF16_QUAD L"sysid"    ,      0,      FALSE, SysFnSYSID_EM , 0          },  // System Identifier
	{WS_UTF16_QUAD L"sysver"   ,      0,      FALSE, SysFnSYSVER_EM, 0          },  // System Version
	{WS_UTF16_QUAD L"tc"       ,      0,      FALSE, SysFnTC_EM    , 0          },  // Terminal Control Characters
	{WS_UTF16_QUAD L"tcbel"    ,      0,      FALSE, SysFnTCBEL_EM , 0          },  // Terminal Control Character, Bell
	{WS_UTF16_QUAD L"tcbs"     ,      0,      FALSE, SysFnTCBS_EM  , 0          },  // Terminal Control Character, Backspace
	{WS_UTF16_QUAD L"tcdel"    ,      0,      FALSE, SysFnTCDEL_EM , 0          },  // Terminal Control Character, Delete
	{WS_UTF16_QUAD L"tcesc"    ,      0,      FALSE, SysFnTCESC_EM , 0          },  // Terminal Control Character, Escape
	{WS_UTF16_QUAD L"tcff"     ,      0,      FALSE, SysFnTCFF_EM  , 0          },  // Terminal Control Character, Form Feed
	{WS_UTF16_QUAD L"tcht"     ,      0,      FALSE, SysFnTCHT_EM  , 0          },  // Terminal Control Character, Horizontal Tab
	{WS_UTF16_QUAD L"tclf"     ,      0,      FALSE, SysFnTCLF_EM  , 0          },  // Terminal Control Character, Line Feed
	{WS_UTF16_QUAD L"tcnl"     ,      0,      FALSE, SysFnTCNL_EM  , 0          },  // Terminal Control Character, New Line
	{WS_UTF16_QUAD L"tcnul"    ,      0,      FALSE, SysFnTCNUL_EM , 0          },  // Terminal Control Character, Null
	{WS_UTF16_QUAD L"ts"       ,      0,      FALSE, SysFnTS_EM    , 0          },  // Time Stamp
////{WS_UTF16_QUAD L"wa"       ,      0,      FALSE, SysFnWA_EM    , 0          },  // Workspace Available

////{WS_UTF16_QUAD L"call"     ,      1,      FALSE, SysFnCALL_EM  , 0          },  // Call Assembler Code
	{WS_UTF16_QUAD L"cr"       ,      1,      FALSE, SysFnCR_EM    , 0          },  // Canonical Representation
////{WS_UTF16_QUAD L"crl"      ,      1,      FALSE, SysFnCRL_EM   , 0          },  // Canonical Representation, Line
////{WS_UTF16_QUAD L"crlpc"    ,      1,      FALSE, SysFnCRLPC_EM , 0          },  // Canonical Representation, Public Comment
////{WS_UTF16_QUAD L"def"      ,      1,      FALSE, SysFnDEF_EM   , 0          },  // Define Function
////{WS_UTF16_QUAD L"defl"     ,      1,      FALSE, SysFnDEFL_EM  , 0          },  // Define Function Line
	{WS_UTF16_QUAD L"dl"       ,      1,      FALSE, SysFnDL_EM    , 0          },  // Delay Execution
	{WS_UTF16_QUAD L"dr"       ,      1,      FALSE, SysFnDR_EM    , 0          },  // Data Representation
////{WS_UTF16_QUAD L"erase"    ,      1,      FALSE, SysFnERASE_EM , 0          },  // Erase Names
////{WS_UTF16_QUAD L"error"    ,      1,      FALSE, SysFnERROR_EM , 0          },  // Signal Error
////{WS_UTF16_QUAD L"ex"       ,      1,      FALSE, SysFnEX_EM    , 0          },  // Erase Names
////{WS_UTF16_QUAD L"fi"       ,      1,      FALSE, SysFnFI_EM    , 0          },  // Format Items
////{WS_UTF16_QUAD L"fmt"      ,      1,      FALSE, SysFnFMT_EM   , 0          },  // Format
////{WS_UTF16_QUAD L"fx"       ,      1,      FALSE, SysFnFX_EM    , 0          },  // Fix Function
////{WS_UTF16_QUAD L"idlist"   ,      1,      FALSE, SysFnIDLIST_EM, 0          },  // Identifier List
////{WS_UTF16_QUAD L"idloc"    ,      1,      FALSE, SysFnIDLOC_EM , 0          },  // Identifier Localization
////{WS_UTF16_QUAD L"mf"       ,      1,      FALSE, SysFnMF_EM    , 0          },  // Monitor Function
	{WS_UTF16_QUAD L"nc"       ,      1,      FALSE, SysFnNC_EM    , 0          },  // Name Classification
	{WS_UTF16_QUAD L"nl"       ,      1,      FALSE, SysFnNL_EM    , 0          },  // Name List
	{WS_UTF16_QUAD L"size"     ,      1,      FALSE, SysFnSIZE_EM  , 0          },  // Size of an object
////{WS_UTF16_QUAD L"ss"       ,      1,      FALSE, SysFnSS_EM    , 0          },  // Search String
////{WS_UTF16_QUAD L"stop"     ,      1,      FALSE, SysFnSTOP_EM  , 0          },  // Manage Stop Points
////{WS_UTF16_QUAD L"trace"    ,      1,      FALSE, SysFnTRACE_EM , 0          },  // Manage Trace Points
	{WS_UTF16_QUAD L"type"     ,      1,      FALSE, SysFnTYPE_EM  , 0          },  // Manage Trace Points
	{WS_UTF16_QUAD L"ucs"      ,      1,      FALSE, SysFnUCS_EM   , 0          },  // Universal Character Set
////{WS_UTF16_QUAD L"vi"       ,      1,      FALSE, StsFnVI_EM    , 0          },  // Verify Items
////{WS_UTF16_QUAD L"vr"       ,      1,      FALSE, SysFnVR_EM    , 0          },  // Vector Representation of a Function

	// ***FIXME*** Add more entries
};

// The # rows in the above table
#define ASYSTEMNAMES_NROWS	(sizeof (aSystemNames) / sizeof (aSystemNames[0]))


//***************************************************************************
//	$MakePermVars
//
//	Make various permanent variables
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- MakePermVars"
#else
#define APPEND_NAME
#endif

void MakePermVars
	(void)

{
	LPVARARRAY_HEADER lpHeader; 	// Ptr to array header
	LPAPLCHAR		  lpMemRes; 	// Ptr to result global memory
	UINT			  uRes; 		// Loop counter

	// Create zilde
	// Note, we can't use DbgGlobalAlloc here as the
	//	 PTD has not been allocated as yet
	hGlbZilde = MyGlobalAlloc (GHND, (UINT) CalcArraySize (ARRAY_BOOL, 0, 1));
	if (!hGlbZilde)
	{
		DbgStop (); 		// We should never get here
	} // End IF

	// Lock the memory to get a ptr to it
	lpHeader = MyGlobalLock (hGlbZilde);

	// Fill in the header values
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = ARRAY_BOOL;
	lpHeader->Perm		 = 1;		// So we don't free it
////lpHeader->SysVar	 = 0;		// Already zero from GHND
////lpHeader->RefCnt	 = 0;		// Ignore as this is perm
////lpHeader->NELM		 = 0;		// Already zero from GHND
	lpHeader->Rank		 = 1;

	// Mark as zero length
////*VarArrayBaseToDim (lpHeader) = 0;	// Already zero from GHND

	// We no longer need this ptr
	MyGlobalUnlock (hGlbZilde); lpHeader = NULL;

	// Create various permanent char vectors
	hGlbQuadDM	= MakePermCharVector (WS_QUADDM);
	hGlbMTChar	= MakePermCharVector (MTChar);
	hGlbSAEmpty = hGlbMTChar;
	hGlbSAClear = MakePermCharVector (SAClear);
	hGlbSAError = MakePermCharVector (SAError);
	hGlbSAExit	= MakePermCharVector (SAExit);
	hGlbSAOff	= MakePermCharVector (SAOff);
	hGlbQuadWSID_CWS = hGlbMTChar;

	// Create []AV
	hGlbAV = MyGlobalAlloc (GHND, (UINT) CalcArraySize (ARRAY_CHAR, 256, 1));
	if (!hGlbAV)
	{
		DbgStop (); 		// We should never get here
	} // End IF

	// Lock the memory to get a ptr to it
	lpHeader = MyGlobalLock (hGlbAV);

	// Fill in the header values
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = ARRAY_CHAR;
	lpHeader->Perm		 = 1;		// So we don't free it
////lpHeader->SysVar	 = 0;		// Already zero from GHND
////lpHeader->RefCnt	 = 0;		// Ignore as this is perm
	lpHeader->NELM		 = 256;
	lpHeader->Rank		 = 1;

	// Fill in the dimension
	*VarArrayBaseToDim (lpHeader) = 256;

	// Skip over the header and dimensions to the data
	lpMemRes = VarArrayBaseToData (lpHeader, 1);

	// Fill in the result:	0-0x7E
	for (uRes = 0; uRes < 0x7F; uRes++)
		lpMemRes[uRes] = uRes;

	// Fill in the result:	0x7F-0xFF
	for (		 ; uRes < 0x100; uRes++)
		lpMemRes[uRes] = L' ';

	// Fill in the result:	miscellaneous 0x80-0xFF
	lpMemRes[0x80] = 0x00C7;			// C cedilla
	lpMemRes[0x81] = 0x00FC;			// u umlaut
	lpMemRes[0x82] = 0x00E9;			// e right
	lpMemRes[0x83] = 0x00E2;			// a circumflex
	lpMemRes[0x84] = 0x00E4;			// a umlaut
	lpMemRes[0x85] = 0x00E0;			// a left
	lpMemRes[0x86] = 0x00E5;			// a circle
	lpMemRes[0x87] = 0x00E7;			// c cedilla
	lpMemRes[0x88] = 0x00EA;			// e circumflex
	lpMemRes[0x89] = 0x00EB;			// e dieresis
	lpMemRes[0x8A] = 0x00E8;			// e left
	lpMemRes[0x8B] = 0x00EF;			// i dieresis
	lpMemRes[0x8C] = 0x00EE;			// i circumflex
	lpMemRes[0x8D] = 0x00EC;			// i left
	lpMemRes[0x8E] = 0x00C4;			// A dieresis
	lpMemRes[0x8F] = 0x00C2;			// A circumflex

	lpMemRes[0x90] = UTF16_QUAD;
	lpMemRes[0x91] = UTF16_QUOTEQUAD;
	lpMemRes[0x92] = UTF16_DOMINO;
	lpMemRes[0x93] = 0x00F4;			// o circumflex
	lpMemRes[0x94] = 0x00F6;			// o dieresis
	lpMemRes[0x95] = 0x00F2;			// o left
	lpMemRes[0x96] = 0x00FC;			// u dieresis
	lpMemRes[0x97] = 0x00F9;			// u left
	lpMemRes[0x98] = UTF16_DOWNTACK;
	lpMemRes[0x99] = 0x00D6;			// O dieresis
	lpMemRes[0x9A] = 0x00DC;			// U dieresis
	lpMemRes[0x9B] = 0x00F8;			// 0 slash
	lpMemRes[0x9C] = 0x00A3;			// Pound sign
	lpMemRes[0x9D] = UTF16_UPTACK;
	lpMemRes[0x9E] = 0x00  ;			// Pt		// ***FIXME***
	lpMemRes[0x9F] = UTF16_IBEAM;

	lpMemRes[0xA0] = 0x00E1;			// a right
	lpMemRes[0xA1] = 0x00ED;			// i right
	lpMemRes[0xA2] = 0x00F3;			// o right
	lpMemRes[0xA3] = 0x00FA;			// u right
	lpMemRes[0xA4] = 0x00F1;			// n tilde
	lpMemRes[0xA5] = 0x00D1;			// N tilde
	lpMemRes[0xA6] = 0x00AA;			// a underbar
	lpMemRes[0xA7] = 0x00BA;			// o underbar
	lpMemRes[0xA8] = 0x00BF;			// Inverted query
	lpMemRes[0xA9] = UTF16_UPSTILE;
	lpMemRes[0xAA] = 0x00AC;			// PL1 not
	lpMemRes[0xAB] = 0x00  ;			// Blank
	lpMemRes[0xAC] = UTF16_DOWNSHOE;
	lpMemRes[0xAD] = 0x00A1;			// Inverted shreik
	lpMemRes[0xAE] = UTF16_DOWNTACKJOT;
	lpMemRes[0xAF] = UTF16_UPTACKJOT;

	lpMemRes[0xB0] = 0x00  ;			// Filler
	lpMemRes[0xB1] = 0x00  ;			// Filler
	lpMemRes[0xB2] = 0x00  ;			// Filler
	lpMemRes[0xB3] = UTF16_STILE;
	lpMemRes[0xB4] = 0x00  ;			// Line draw
	lpMemRes[0xB5] = UTF16_CIRCLESTAR;
	lpMemRes[0xB6] = UTF16_DELTA;
	lpMemRes[0xB7] = UTF16_DEL;
	lpMemRes[0xB8] = UTF16_RIGHTARROW;
	lpMemRes[0xB9] = 0x00  ;			// Line draw
	lpMemRes[0xBA] = 0x00  ;			// Line draw
	lpMemRes[0xBB] = 0x00  ;			// Line draw
	lpMemRes[0xBC] = 0x00  ;			// Line draw
	lpMemRes[0xBD] = UTF16_LEFTARROW;
	lpMemRes[0xBE] = UTF16_DOWNSTILE;
	lpMemRes[0xBF] = 0x00  ;			// Line draw

	lpMemRes[0xC0] = 0x00  ;			// Line draw
	lpMemRes[0xC1] = 0x00  ;			// Line draw
	lpMemRes[0xC2] = 0x00  ;			// Line draw
	lpMemRes[0xC3] = 0x00  ;			// Line draw
	lpMemRes[0xC4] = 0x00  ;			// Line draw
	lpMemRes[0xC5] = 0x00  ;			// Line draw
	lpMemRes[0xC6] = UTF16_UPARROW;
	lpMemRes[0xC7] = UTF16_DOWNARROW;
	lpMemRes[0xC8] = 0x00  ;			// Line draw
	lpMemRes[0xC9] = 0x00  ;			// Line draw
	lpMemRes[0xCA] = 0x00  ;			// Line draw
	lpMemRes[0xCB] = 0x00  ;			// Line draw
	lpMemRes[0xCC] = 0x00  ;			// Line draw
	lpMemRes[0xCD] = 0x00  ;			// Line draw
	lpMemRes[0xCE] = 0x00  ;			// Line draw
	lpMemRes[0xCF] = UTF16_EQUALUNDERBAR;

	lpMemRes[0xD0] = UTF16_IOTAUNDERBAR;
	lpMemRes[0xD1] = UTF16_EPSILONUNDERBAR;
	lpMemRes[0xD2] = UTF16_DIERESISDOT;
	lpMemRes[0xD3] = UTF16_SQUAD;
	lpMemRes[0xD4] = UTF16_QUADSLOPE;
	lpMemRes[0xD5] = UTF16_QUADJOT;
	lpMemRes[0xD6] = UTF16_RIGHTTACK;
	lpMemRes[0xD7] = UTF16_LEFTTACK;
	lpMemRes[0xD8] = UTF16_DIAMOND;
	lpMemRes[0xD9] = 0x00  ;			// Line draw
	lpMemRes[0xDA] = 0x00  ;			// Line draw
	lpMemRes[0xDB] = 0x00  ;			// Filler
	lpMemRes[0xDC] = 0x00  ;			// Filler
	lpMemRes[0xDD] = 0x00  ;			// Filler
	lpMemRes[0xDE] = 0x00  ;			// Filler
	lpMemRes[0xDF] = 0x00  ;			// Filler

	lpMemRes[0xE0] = UTF16_ALPHA;
	lpMemRes[0xE1] = 0x00DF;			// Beta
	lpMemRes[0xE2] = UTF16_LEFTSHOE;
	lpMemRes[0xE3] = UTF16_RIGHTSHOE;
	lpMemRes[0xE4] = UTF16_LAMP;
	lpMemRes[0xE5] = UTF16_UPCARETTILDE;
	lpMemRes[0xE6] = UTF16_RHO;
	lpMemRes[0xE7] = UTF16_DOWNCARETTILDE;
	lpMemRes[0xE8] = UTF16_CIRCLESTILE;
	lpMemRes[0xE9] = UTF16_CIRCLEBAR;
	lpMemRes[0xEA] = UTF16_CIRCLE;
	lpMemRes[0xEB] = UTF16_DOWNCARET;
	lpMemRes[0xEC] = UTF16_IOTA;
	lpMemRes[0xED] = UTF16_CIRCLESLOPE;
	lpMemRes[0xEE] = UTF16_EPSILON;
	lpMemRes[0xEF] = UTF16_UPSHOE;

	lpMemRes[0xF0] = UTF16_SLASHBAR;
	lpMemRes[0xF1] = UTF16_SLOPEBAR;
	lpMemRes[0xF2] = UTF16_RIGHTCARETUNDERBAR;
	lpMemRes[0xF3] = UTF16_LEFTCARETUNDERBAR;
	lpMemRes[0xF4] = UTF16_NOTEQUAL;
	lpMemRes[0xF5] = UTF16_TIMES;
	lpMemRes[0xF6] = UTF16_COLONBAR;
	lpMemRes[0xF7] = UTF16_DELTAUNDERBAR;
	lpMemRes[0xF8] = UTF16_JOT;
	lpMemRes[0xF9] = UTF16_OMEGA;
	lpMemRes[0xFA] = UTF16_DELTILDE;
	lpMemRes[0xFB] = UTF16_DELTASTILE;
	lpMemRes[0xFC] = UTF16_DELSTILE;
	lpMemRes[0xFD] = UTF16_OVERBAR;
	lpMemRes[0xFE] = UTF16_DIERESIS;
	lpMemRes[0xFF] = 0x00  ;			// Blank

////lpMemRes[0x??] = UTF16_DIERESISDEL;
////lpMemRes[0x??] = UTF16_DIERESISDOWNTACK;
////lpMemRes[0x??] = UTF16_DIERESISCIRCLE;
////lpMemRes[0x??] = UTF16_DIERESISSTAR;
////lpMemRes[0x??] = UTF16_DIERESISTILDE;
////lpMemRes[0x??] = UTF16_STILETILDE;
////lpMemRes[0x??] = UTF16_ZILDE;

	// We no longer need this ptr
	MyGlobalUnlock (hGlbAV); lpMemRes = NULL;
} // End MakePermVars
#undef	APPEND_NAME


//***************************************************************************
//	$MakePermCharVector
//
//	Make a permanent character vector
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- MakePermCharVector"
#else
#define APPEND_NAME
#endif

HGLOBAL MakePermCharVector
	(LPWCHAR lpwc)

{
	HGLOBAL hGlbRes;
	UINT	uLen;
	LPVARARRAY_HEADER lpHeader;

	// Get the string length
	uLen = lstrlenW (lpwc);

	// Note, we can't use DbgGlobalAlloc here as the
	//	 PTD has not been allocated as yet
	hGlbRes = MyGlobalAlloc (GHND, (UINT) CalcArraySize (ARRAY_CHAR, uLen, 1));
	if (!hGlbRes)
	{
		DbgStop (); 		// We should never get here
	} // End IF

	// Lock the memory to get a ptr to it
	lpHeader = MyGlobalLock (hGlbRes);

	// Fill in the header values
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = ARRAY_CHAR;
	lpHeader->Perm		 = 1;		// So we don't free it
////lpHeader->SysVar	 = 0;		// Already zero from GHND
////lpHeader->RefCnt	 = 0;		// Ignore as this is perm
	lpHeader->NELM		 = uLen;
	lpHeader->Rank		 = 1;

	// Save the dimension
	*VarArrayBaseToDim (lpHeader) = uLen;

	// Skip over the header and dimensions to the data
	lpHeader = VarArrayBaseToData (lpHeader, 1);

	// Copy the data to memory
	CopyMemory (lpHeader, lpwc, uLen * sizeof (APLCHAR));

	// We no longer need this ptr
	MyGlobalUnlock (hGlbRes); lpHeader = NULL;

	return hGlbRes;
} // End MakePermCharVector
#undef	APPEND_NAME


//***************************************************************************
//	$SymTabAppendSysName_EM
//
//	Append a system name to the symbol table
//***************************************************************************

BOOL SymTabAppendSysName_EM
	(LPSYSNAME	 lpSysName,
	 LPSYMENTRY *lplpSysVarSym)

{
	STFLAGS    stFlags = {0};
	LPSYMENTRY lpSymEntry;

	// Set the flags of the entry we're appending
	if (lpSysName->bSysVar)
	{
		stFlags.ObjType 	= NAMETYPE_VAR;
		stFlags.DfnSysLabel = (lpSysName->uValence EQ SYSLBL);
	} else
	{
		if (lpSysName->uValence EQ 0)
			stFlags.ObjType = NAMETYPE_FN0;
		else
			stFlags.ObjType = NAMETYPE_FN12;
	} // End IF/ELSE

	// Set the flags for what we're appending
	stFlags.ObjName = OBJNAME_SYS;
	stFlags.Perm	=
	stFlags.NotCase =
	stFlags.Inuse	= 1;

	// Append the name as new
	lpSymEntry = SymTabAppendNewName_EM (lpSysName->lpwszName, &stFlags);

	// Check for error
	if (!lpSymEntry)
		return FALSE;

	// Save the LPSYMENTRY if requested
	if (lplpSysVarSym)
		*lplpSysVarSym = lpSymEntry;

	// Save the address of the execution routine
	lpSymEntry->stData.stNameFcn = lpSysName->lpNameFcn;

	return TRUE;
} // End SymTabAppendSysName_EM


//***************************************************************************
//	$InitSystemNames_EM
//
//	Append all system names to the symbol table
//***************************************************************************

BOOL InitSystemNames_EM
	(void)

{
	int 		 i; 			// Loop counter
	HGLOBAL 	 hGlbPTD;		// PerTabData global memory handle
	LPPERTABDATA lpMemPTD;		// Ptr to PerTabData global memory
	BOOL		 bRet = TRUE;	// TRUE iff result is valid
	LPSYMENTRY	*ptdSysVarSym[SYSVAR_LENGTH];

	// Get the thread's PerTabData global memory handle
	hGlbPTD = TlsGetValue (dwTlsPerTabData);

	// Lock the memory to get a ptr to it
	lpMemPTD = MyGlobalLock (hGlbPTD);

	ptdSysVarSym[SYSVAR_UNK ] = NULL					;
	ptdSysVarSym[SYSVAR_ALX ] = &lpMemPTD->lpSymQuadALX ;
	ptdSysVarSym[SYSVAR_CT	] = &lpMemPTD->lpSymQuadCT	;
	ptdSysVarSym[SYSVAR_ELX ] = &lpMemPTD->lpSymQuadELX ;
	ptdSysVarSym[SYSVAR_IO	] = &lpMemPTD->lpSymQuadIO	;
	ptdSysVarSym[SYSVAR_LX	] = &lpMemPTD->lpSymQuadLX	;
	ptdSysVarSym[SYSVAR_PP	] = &lpMemPTD->lpSymQuadPP	;
	ptdSysVarSym[SYSVAR_PR	] = &lpMemPTD->lpSymQuadPR	;
	ptdSysVarSym[SYSVAR_PW	] = &lpMemPTD->lpSymQuadPW	;
	ptdSysVarSym[SYSVAR_RL	] = &lpMemPTD->lpSymQuadRL	;
	ptdSysVarSym[SYSVAR_SA	] = &lpMemPTD->lpSymQuadSA	;
	ptdSysVarSym[SYSVAR_WSID] = &lpMemPTD->lpSymQuadWSID;

	Assert (HshTabFrisk ());

	// Append all system names
	for (i = 0; bRet && i < ASYSTEMNAMES_NROWS; i++)
	if (!SymTabAppendSysName_EM (&aSystemNames[i], ptdSysVarSym[aSystemNames[i].sysVarIndex]))
	{
		bRet = FALSE;

		break;
	} // End FOR/IF

	Assert (HshTabFrisk ());

	// We no longer need this ptr
	MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

	return bRet;
} // End InitSystemNames_EM


//***************************************************************************
//	$AssignCharVector_EM
//
//	Assign a character vector to a name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- AssignCharVector_EM"
#else
#define APPEND_NAME
#endif

BOOL AssignCharVector_EM
	(LPWCHAR	lpwszName,
	 HGLOBAL	hGlbVal_CWS,
	 UINT		SysVarValid,
	 LPSYMENTRY lpSymEntryDest)

{
	// Make a copy of the CLEAR WS value, and
	//	 save the global memory ptr
	lpSymEntryDest->stData.stGlbData = CopySymGlbDirGlb (hGlbVal_CWS);

	// Save the validate index
	lpSymEntryDest->stFlags.SysVarValid = SysVarValid;

	// Mark as having a value
	lpSymEntryDest->stFlags.Value = 1;

	return TRUE;
} // End AssignCharVector_EM
#undef	APPEND_NAME


//***************************************************************************
//	$AssignRealScalar_EM
//
//	Assign a real number scalar to a name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- AssignRealScalar_EM"
#else
#define APPEND_NAME
#endif

BOOL AssignRealScalar_EM
	(LPWCHAR	lpwszName,
	 APLFLOAT	fFloat,
	 UINT		SysVarValid,
	 LPSYMENTRY lpSymEntryDest)

{
	STFLAGS stFlags = {0};

	// Save the constant
	lpSymEntryDest->stData.stFloat = fFloat;

	// Mark as immediate floating point constant
	stFlags.Imm 	= 1;
	stFlags.ImmType = IMMTYPE_FLOAT;
	stFlags.Value	= 1;

	// Save the flags
	stFlags.SysVarValid = SysVarValid;
	*(PUINT_PTR) &lpSymEntryDest->stFlags |= *(PUINT_PTR) &stFlags;

	return TRUE;
} // End AssignRealScalar_EM
#undef	APPEND_NAME


//***************************************************************************
//	$AssignBoolScalar
//
//	Assign an Boolean scalar to a name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- AssignBoolScalar_EM"
#else
#define APPEND_NAME
#endif

BOOL AssignBoolScalar_EM
	(LPWCHAR	lpwszName,
	 APLBOOL	aplBoolean,
	 UINT		SysVarValid,
	 LPSYMENTRY lpSymEntryDest)

{
	STFLAGS stFlags = {0};

	// Save the constant
	lpSymEntryDest->stData.stBoolean = aplBoolean;

	// Mark as immediate Boolean constant
	stFlags.Imm 	= 1;
	stFlags.ImmType = IMMTYPE_BOOL;
	stFlags.Value	= 1;

	// Save the flags
	stFlags.SysVarValid = SysVarValid;
	*(PUINT_PTR) &lpSymEntryDest->stFlags |= *(PUINT_PTR) &stFlags;

	return TRUE;
} // End AssignBoolScalar_EM
#undef	APPEND_NAME


//***************************************************************************
//	$AssignIntScalar_EM
//
//	Assign an integer scalar to a name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- AssignIntScalar_EM"
#else
#define APPEND_NAME
#endif

BOOL AssignIntScalar_EM
	(LPWCHAR	lpwszName,
	 APLINT 	aplInteger,
	 UINT		SysVarValid,
	 LPSYMENTRY lpSymEntryDest)

{
	STFLAGS stFlags = {0};

	// Save the constant
	lpSymEntryDest->stData.stInteger = aplInteger;

	// Mark as immediate Integer constant
	stFlags.Imm 	= 1;
	stFlags.ImmType = IMMTYPE_INT;
	stFlags.Value	= 1;

	// Save the flags
	stFlags.SysVarValid = SysVarValid;
	*(PUINT_PTR) &lpSymEntryDest->stFlags |= *(PUINT_PTR) &stFlags;

	return TRUE;
} // End AssignIntScalar_EM
#undef	APPEND_NAME


//***************************************************************************
//	$AssignCharScalar_EM
//
//	Assign a character scalar to a name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- AssignCharScalar_EM"
#else
#define APPEND_NAME
#endif

BOOL AssignCharScalar_EM
	(LPWCHAR	lpwszName,
	 APLCHAR	aplChar,
	 UINT		SysVarValid,
	 LPSYMENTRY lpSymEntryDest)

{
	STFLAGS stFlags = {0};

	// Save the constant
	lpSymEntryDest->stData.stChar = aplChar;

	// Mark as immediate Character constant
	stFlags.Imm 	= 1;
	stFlags.ImmType = IMMTYPE_CHAR;
	stFlags.Value	= 1;

	// Save the flags
	stFlags.SysVarValid = SysVarValid;
	*(PUINT_PTR) &lpSymEntryDest->stFlags |= *(PUINT_PTR) &stFlags;

	return TRUE;
} // End AssignCharScalar_EM
#undef	APPEND_NAME


//***************************************************************************
//	$ValidateBoolean_EM
//
//	Validate a value about to be assigned to a Boolean system var.
//
//	We allow any numeric scalar or one-element vector whose value is 0 or 1.
//
//	The order of error checking is RANK, LENGTH, DOMAIN.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ValidateBoolean_EM"
#else
#define APPEND_NAME
#endif

BOOL ValidateBoolean_EM
	(LPTOKEN   lptkName,			// Ptr to name token
	 LPTOKEN   lpToken) 			// Ptr to value token

{
	APLSTYPE aplTypeRht;		// Right arg storage type
	APLNELM  aplNELMRht;		// Right arg NELM
	APLRANK  aplRankRht;		// Right arg rank
	HGLOBAL  hGlbRht;			// Right arg global memory handle
	LPVOID	 lpMemRht;			// Ptr to right arg global memory
	BOOL	 bRet = TRUE;		// TRUE iff result is valid
	LPWCHAR  lpwErrMsg = ERRMSG_DOMAIN_ERROR APPEND_NAME;
	APLINT	 aplInteger;
	APLBOOL  aplBoolean;

	// Split cases based upon the token type
	switch (lpToken->tkFlags.TknType)
	{
		case TKT_VARNAMED:
			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

			if (!lpToken->tkData.tkSym->stFlags.Imm)
			{
				// Get the HGLOBAL
				hGlbRht = lpToken->tkData.tkSym->stData.stGlbData;

				break;		// Continue with HGLOBAL processing
			} // End IF

			// Handle the immediate case

			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

			// Split cases based upon the symbol table immediate type
			switch (lpToken->tkData.tkSym->stFlags.ImmType)
			{
				case IMMTYPE_BOOL:
					aplBoolean = lpToken->tkData.tkSym->stData.stBoolean;

					break;

				case IMMTYPE_INT:
					// Test the value
					bRet = (lpToken->tkData.tkSym->stData.stInteger EQ 0
						 || lpToken->tkData.tkSym->stData.stInteger EQ 1);
					if (bRet)
						aplBoolean = (APLBOOL) (lpToken->tkData.tkSym->stData.stInteger);
					break;

				case IMMTYPE_FLOAT:
					// Convert the value to an integer using System CT
					aplInteger = FloatToAplint_SCT (lpToken->tkData.tkSym->stData.stFloat,
												   &bRet);
					// Test the value
					bRet = bRet && (aplInteger EQ 0 || aplInteger EQ 1);

					if (bRet)
						aplBoolean = (APLBOOL) aplInteger;
					break;

				case IMMTYPE_CHAR:
					bRet = FALSE;

					break;
			} // End SWITCH

			goto NORMAL_EXIT;

		case TKT_VARIMMED:
			// Split cases based upon the token immediate type
			switch (lpToken->tkFlags.ImmType)
			{
				case IMMTYPE_BOOL:
					aplBoolean = lpToken->tkData.tkBoolean;

					break;

				case IMMTYPE_INT:
					// Test the value
					bRet = (lpToken->tkData.tkInteger EQ 0
						 || lpToken->tkData.tkInteger EQ 1);
					if (bRet)
						aplBoolean = (APLBOOL) (lpToken->tkData.tkInteger);
					break;

				case IMMTYPE_FLOAT:
					// Convert the value to an integer using System CT
					aplInteger = FloatToAplint_SCT (lpToken->tkData.tkFloat,
												   &bRet);
					// Test the value
					bRet = bRet && (aplInteger EQ 0 || aplInteger EQ 1);

					if (bRet)
						aplBoolean = (APLBOOL) aplInteger;
					break;

				case IMMTYPE_CHAR:
					bRet = FALSE;

					break;
			} // End SWITCH

			goto NORMAL_EXIT;

		case TKT_LISTPAR:	// The tkData is an HGLOBAL of an array of HGLOBALs
			ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
									   lpToken);
			return FALSE;

		case TKT_VARARRAY:	// tkData is an HGLOBAL of an array of ???
			// Get the HGLOBAL
			hGlbRht = lpToken->tkData.tkGlbData;

			break;			// Continue with HGLOBAL processing

		defstop
			return FALSE;
	} // End SWITCH

	// st/tkData is a valid HGLOBAL variable array
	Assert (IsGlbTypeVarDir (hGlbRht));

	// Lock the memory to get a ptr to it
	lpMemRht = MyGlobalLock (ClrPtrTypeDirGlb (hGlbRht));

#define lpHeader	((LPVARARRAY_HEADER) lpMemRht)

	// Get the Array Type, NELM, and Rank
	aplTypeRht = lpHeader->ArrType;
	aplNELMRht = lpHeader->NELM;
	aplRankRht = lpHeader->Rank;

#undef	lpHeader

	// Skip over the header and dimensions to the data
	lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

	// Check for scalar or vector
	if (aplRankRht > 1)
	{
		lpwErrMsg = ERRMSG_RANK_ERROR APPEND_NAME;

		bRet = FALSE;
	} else
	// Check for singleton
	if (aplNELMRht NE 1)
	{
		lpwErrMsg = ERRMSG_LENGTH_ERROR APPEND_NAME;

		bRet = FALSE;
	} else
	// Split cases based upon the array type
	switch (aplTypeRht)
	{
		case ARRAY_BOOL:
			aplBoolean = *(LPAPLBOOL) lpMemRht;

			break;

		case ARRAY_INT:
			// Get the value
			aplInteger = *(LPAPLINT) lpMemRht;

			// Test the value
			bRet = (aplInteger EQ 0
				 || aplInteger EQ 1);
			if (bRet)
				aplBoolean = (APLBOOL) (*(LPAPLINT) lpMemRht);
			break;

		case ARRAY_CHAR:
		case ARRAY_HETERO:
		case ARRAY_NESTED:
			bRet = FALSE;

			break;

		case ARRAY_FLOAT:
			// Convert the value to an integer using System CT
			aplInteger = FloatToAplint_SCT (*(LPAPLFLOAT) lpMemRht,
										   &bRet);
			// Test the value
			bRet = (aplInteger EQ 0
				 || aplInteger EQ 1);
			if (bRet)
				aplBoolean = (APLBOOL) aplInteger;
			break;

		defstop
			break;
	} // End IF/ELSE/SWITCH

	// We no longer need this ptr
	MyGlobalUnlock (ClrPtrTypeDirGlb (hGlbRht)); lpMemRht = NULL;
NORMAL_EXIT:
	// If in error, set error message;
	//	 otherwise, save the value in the name
	if (!bRet)
		ErrorMessageIndirectToken (lpwErrMsg,
								   lpToken);
	else
	{
		lptkName->tkData.tkSym->stData.stBoolean = aplBoolean;
		lptkName->tkFlags.NoDisplay = 1;
	} // End IF

	return bRet;
} // End ValidateBoolean_EM
#undef	APPEND_NAME


//***************************************************************************
//	$ValidateInteger_EM
//
//	Validate a value about to be assigned to an integer system var.
//
//	We allow any numeric scalar or one-element vector whose value
//	  is integral and in a given range.
//
//	The order of error checking is RANK, LENGTH, DOMAIN.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ValidateInteger_EM"
#else
#define APPEND_NAME
#endif

BOOL ValidateInteger_EM
	(LPTOKEN  lptkName, 			// Ptr to name token
	 LPTOKEN  lptkExpr, 			// Ptr to value token
	 UINT	  uValidLo, 			// Low range value (inclusive)
	 UINT	  uValidHi) 			// High ...

{
	APLSTYPE aplTypeRht;		// Right arg storage type
	APLNELM  aplNELMRht;		// Right arg NELM
	APLRANK  aplRankRht;		// Right arg rank
	HGLOBAL  hGlbRht;			// Right arg global memory handle
	LPVOID	 lpMemRht;			// Ptr to right arg global memory
	BOOL	 bRet = TRUE;
	LPWCHAR  lpwErrMsg = ERRMSG_DOMAIN_ERROR APPEND_NAME;
	APLINT	 aplInteger;

	// Split cases based upon the token type
	switch (lptkExpr->tkFlags.TknType)
	{
		case TKT_VARNAMED:
			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lptkExpr->tkData.tkVoid) EQ PTRTYPE_STCONST);

			if (!lptkExpr->tkData.tkSym->stFlags.Imm)
			{
				// Get the HGLOBAL
				hGlbRht = lptkExpr->tkData.tkSym->stData.stGlbData;

				break;		// Continue with HGLOBAL processing
			} // End IF

			// Handle the immediate case

			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lptkExpr->tkData.tkVoid) EQ PTRTYPE_STCONST);

			// Split cases based upon the symbol table immediate type
			switch (lptkExpr->tkData.tkSym->stFlags.ImmType)
			{
				case IMMTYPE_BOOL:
					// Get the value
					aplInteger = lptkExpr->tkData.tkSym->stData.stBoolean;

					// Test the value
					bRet = (uValidLo <= aplInteger
						 && 			aplInteger <= uValidHi);
					break;

				case IMMTYPE_INT:
					// Get the value
					aplInteger = lptkExpr->tkData.tkSym->stData.stInteger;

					// Test the value
					bRet = (uValidLo <= aplInteger
						 && 			aplInteger <= uValidHi);
					break;

				case IMMTYPE_FLOAT:
					// Convert the value to an integer using System CT
					aplInteger = FloatToAplint_SCT (lptkExpr->tkData.tkSym->stData.stFloat,
												   &bRet);
					// Test the value
					bRet = (uValidLo <= aplInteger
						 && 			aplInteger <= uValidHi);
					break;

				case IMMTYPE_CHAR:
					bRet = FALSE;

					break;
			} // End SWITCH

			goto NORMAL_EXIT;

		case TKT_VARIMMED:
			// Split cases based upon the token immediate type
			switch (lptkExpr->tkFlags.ImmType)
			{
				case IMMTYPE_BOOL:
					// Get the value
					aplInteger = lptkExpr->tkData.tkBoolean;

					// Test the value
					bRet = (uValidLo <= aplInteger
						 && 			aplInteger <= uValidHi);
					break;

				case IMMTYPE_INT:
					// Get the value
					aplInteger = lptkExpr->tkData.tkInteger;

					// Test the value
					bRet = (uValidLo <= aplInteger
						 && 			aplInteger <= uValidHi);
					break;

				case IMMTYPE_FLOAT:
					// Convert the value to an integer using System CT
					aplInteger = FloatToAplint_SCT (lptkExpr->tkData.tkFloat,
												   &bRet);
					// Test the value
					bRet = (uValidLo <= aplInteger
						 && 			aplInteger <= uValidHi);
					break;

				case IMMTYPE_CHAR:
					bRet = FALSE;

					break;
			} // End SWITCH

			goto NORMAL_EXIT;

		case TKT_LISTPAR:	// The tkData is an HGLOBAL of an array of HGLOBALs
			ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
									   lptkExpr);

			return FALSE;

		case TKT_STRING:	// tkData is an HGLOBAL of an array of ???
		case TKT_VARARRAY:	// tkData is an HGLOBAL of an array of ???
			// Get the HGLOBAL
			hGlbRht = lptkExpr->tkData.tkGlbData;

			break;			// Continue with HGLOBAL processing

		defstop
			return FALSE;
	} // End SWITCH

	// st/tkData is a valid HGLOBAL variable array
	Assert (IsGlbTypeVarDir (hGlbRht));

	// Lock the memory to get a ptr to it
	lpMemRht = MyGlobalLock (ClrPtrTypeDirGlb (hGlbRht));

#define lpHeader	((LPVARARRAY_HEADER) lpMemRht)

	// Get the Array Type, NELM, and Rank
	aplTypeRht = lpHeader->ArrType;
	aplNELMRht = lpHeader->NELM;
	aplRankRht = lpHeader->Rank;

#undef	lpHeader

	// Skip over the header and dimensions to the data
	lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

	// Check for scalar or vector
	if (aplRankRht > 1)
	{
		lpwErrMsg = ERRMSG_RANK_ERROR APPEND_NAME;

		bRet = FALSE;
	} else
	// Check for singleton
	if (aplNELMRht NE 1)
	{
		lpwErrMsg = ERRMSG_LENGTH_ERROR APPEND_NAME;

		bRet = FALSE;
	} else
	// Split cases based upon the array type
	switch (aplTypeRht)
	{
		case ARRAY_BOOL:
			// Get the value
			aplInteger = *(LPAPLBOOL) lpMemRht;

			// Test the value
			bRet = (uValidLo <= aplInteger
				 && 			aplInteger <= uValidHi);
			break;

		case ARRAY_INT:
			// Get the value
			aplInteger = *(LPAPLINT) lpMemRht;

			// Test the value
			bRet = (uValidLo <= aplInteger
				 && 			aplInteger <= uValidHi);
			break;

		case ARRAY_CHAR:
		case ARRAY_HETERO:
		case ARRAY_NESTED:
			bRet = FALSE;

			break;

		case ARRAY_FLOAT:
			// Convert the value to an integer using System CT
			aplInteger = FloatToAplint_SCT (*(LPAPLFLOAT) lpMemRht,
										   &bRet);
			// Test the value
			bRet = (uValidLo <= aplInteger
				 && 			aplInteger <= uValidHi);
			break;

		defstop
			break;
	} // End IF/ELSE/SWITCH

	// We no longer need this ptr
	MyGlobalUnlock (ClrPtrTypeDirGlb (hGlbRht)); lpMemRht = NULL;
NORMAL_EXIT:
	// If in error, set error message;
	//	 otherwise, save the value in the name
	if (!bRet)
		ErrorMessageIndirectToken (lpwErrMsg,
								   lptkExpr);
	else
	{
		lptkName->tkData.tkSym->stData.stInteger = aplInteger;
		lptkName->tkFlags.NoDisplay = 1;
	} // End IF

	return bRet;
} // End ValidateInteger_EM
#undef	APPEND_NAME


//***************************************************************************
//	$ValidateFloat_EM
//
//	Validate a value about to be assigned to a float system var.
//
//	We allow any numeric scalar or one-element vector whose value
//	  is in a given range.
//
//	The order of error checking is RANK, LENGTH, DOMAIN.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ValidateFloat_EM"
#else
#define APPEND_NAME
#endif

BOOL ValidateFloat_EM
	(LPTOKEN	lptkName,			// Ptr to name token
	 LPTOKEN	lpToken,			// Ptr to value token
	 APLFLOAT	fValidLo,			// Low range value (inclusive)
	 APLFLOAT	fValidHi)			// High ...

{
	HGLOBAL  hGlbRht;				// Right arg global memory handle
	LPVOID	 lpMemRht;				// Ptr to right arg global memory
	BOOL	 bRet = TRUE;			// TRUE iff result is valid
	LPWCHAR  lpwErrMsg = ERRMSG_DOMAIN_ERROR APPEND_NAME;
	APLFLOAT aplFloat;

	// Split cases based upon the token type
	switch (lpToken->tkFlags.TknType)
	{
		case TKT_VARNAMED:
			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

			if (!lpToken->tkData.tkSym->stFlags.Imm)
			{
				// Get the HGLOBAL
				hGlbRht = lpToken->tkData.tkSym->stData.stGlbData;

				break;		// Continue with HGLOBAL processing
			} // End IF

			// Handle the immediate case

			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

			// Split cases based upon the symbol table immediate type
			switch (lpToken->tkData.tkSym->stFlags.ImmType)
			{
				case IMMTYPE_BOOL:
					// Get the value
					aplFloat = lpToken->tkData.tkSym->stData.stBoolean;

					// Test the value
					bRet = (fValidLo <= aplFloat
						 && 			aplFloat <= fValidHi);
					break;

				case IMMTYPE_INT:
					// Get the value
					aplFloat = (APLFLOAT) (lpToken->tkData.tkSym->stData.stInteger);

					// Test the value
					bRet = (fValidLo <= aplFloat
						 && 			aplFloat <= fValidHi);
					break;

				case IMMTYPE_FLOAT:
					aplFloat = lpToken->tkData.tkSym->stData.stFloat;

					// Test the value
					bRet = (fValidLo <= aplFloat
						 && 			aplFloat <= fValidHi);
					break;

				case IMMTYPE_CHAR:
					bRet = FALSE;

					break;
			} // End SWITCH

			goto NORMAL_EXIT;

		case TKT_VARIMMED:
			// Split cases based upon the token immediate type
			switch (lpToken->tkFlags.ImmType)
			{
				case IMMTYPE_BOOL:
					// Get the value
					aplFloat = lpToken->tkData.tkBoolean;

					// Test the value
					bRet = (fValidLo <= aplFloat
						 && 			aplFloat <= fValidHi);
					break;

				case IMMTYPE_INT:
					// Get the value
					aplFloat = (APLFLOAT) (lpToken->tkData.tkInteger);

					// Test the value
					bRet = (fValidLo <= aplFloat
						 && 			aplFloat <= fValidHi);
					break;

				case IMMTYPE_FLOAT:
					// Convert the value to an integer using System CT
					aplFloat = lpToken->tkData.tkFloat;

					// Test the value
					bRet = (fValidLo <= aplFloat
						 && 			aplFloat <= fValidHi);
					break;

				case IMMTYPE_CHAR:
					bRet = FALSE;

					break;
			} // End SWITCH

			goto NORMAL_EXIT;

		case TKT_LISTPAR:	// The tkData is an HGLOBAL of an array of HGLOBALs
			ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
									   lpToken);
			return FALSE;

		case TKT_STRING:	// tkData is an HGLOBAL of an array of ???
		case TKT_VARARRAY:	// tkData is an HGLOBAL of an array of ???
			// Get the HGLOBAL
			hGlbRht = lpToken->tkData.tkGlbData;

			break;			// Continue with HGLOBAL processing

		defstop
			return FALSE;
	} // End SWITCH

	// st/tkData is a valid HGLOBAL variable array
	Assert (IsGlbTypeVarDir (hGlbRht));

	// Lock the memory to get a ptr to it
	lpMemRht = MyGlobalLock (ClrPtrTypeDirGlb (hGlbRht));

#define lpHeader	((LPVARARRAY_HEADER) lpMemRht)

	// Skip over the header and dimensions to the data
	lpMemRht = VarArrayBaseToData (lpHeader, lpHeader->Rank);

	// Check for scalar or vector
	if (lpHeader->Rank > 1)
	{
		lpwErrMsg = ERRMSG_RANK_ERROR APPEND_NAME;

		bRet = FALSE;
	} else
	// Check for singleton
	if (lpHeader->NELM NE 1)
	{
		lpwErrMsg = ERRMSG_LENGTH_ERROR APPEND_NAME;

		bRet = FALSE;
	} else
	// Split cases based upon the array type
	switch (lpHeader->ArrType)
	{
		case ARRAY_BOOL:
			// Get the value
			aplFloat = *(LPAPLBOOL) lpMemRht;

			// Test the value
			bRet = (fValidLo <= aplFloat
				 && 			aplFloat <= fValidHi);
			break;

		case ARRAY_INT:
			// Get the value
			aplFloat = (APLFLOAT) *(LPAPLINT) lpMemRht;

			// Test the value
			bRet = (fValidLo <= aplFloat
				 && 			aplFloat <= fValidHi);
			break;

		case ARRAY_CHAR:
		case ARRAY_HETERO:
		case ARRAY_NESTED:
			bRet = FALSE;

			break;

		case ARRAY_FLOAT:
			// Convert the value to an integer using System CT
			aplFloat = *(LPAPLFLOAT) lpMemRht;

			// Test the value
			bRet = (fValidLo <= aplFloat
				 && 			aplFloat <= fValidHi);
			break;

		defstop
			break;
	} // End IF/ELSE/SWITCH

	// We no longer need this ptr
	MyGlobalUnlock (ClrPtrTypeDirGlb (hGlbRht)); lpMemRht = NULL;
NORMAL_EXIT:
	// If in error, set error message;
	//	 otherwise, save the value in the name
	if (!bRet)
		ErrorMessageIndirectToken (lpwErrMsg,
								   lpToken);
	else
	{
		lptkName->tkData.tkSym->stData.stFloat = aplFloat;
		lptkName->tkFlags.NoDisplay = 1;
	} // End IF

	return bRet;
#undef	lpHeader
} // End ValidateFloat_EM
#undef	APPEND_NAME


//***************************************************************************
//	$ValidateCharVector_EM
//
//	Validate a value about to be assigned to a character vector system var.
//
//	We allow any character scalar or vector.
//
//	The order of error checking is RANK, LENGTH, DOMAIN.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ValidateCharVector_EM"
#else
#define APPEND_NAME
#endif

BOOL ValidateCharVector_EM
	(LPTOKEN  lptkName, 			// Ptr to name token
	 LPTOKEN  lpToken,				// Ptr to value token
	 BOOL	  bWSID)				// TRUE iff this is []WSID

{
	LPVOID	 lpMemRht,				// Ptr to right arg global memory
			 lpMemRes;				// Ptr to result	...
	BOOL	 bRet = TRUE,			// TRUE iff result is valid
			 bScalar = FALSE;		// TRUE iff right arg is scalar
	LPWCHAR  lpwErrMsg = ERRMSG_DOMAIN_ERROR APPEND_NAME;
	APLCHAR  aplChar;				// Right arg first char
	APLNELM  aplNELMRht,			// Right arg NELM
			 aplNELMRes;			// Result	 ...
	APLRANK  aplRankRht;			// Right arg rank
	HGLOBAL  hGlbRht,				// Right arg global memory handle
			 hGlbRes;				// Result	 ...
	APLUINT  ByteRes;				// # bytes in the result

	// Split cases based upon the token type
	switch (lpToken->tkFlags.TknType)
	{
		case TKT_VARNAMED:
			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

			// Split cases based upon the symbol table immediate type
			if (!lpToken->tkData.tkSym->stFlags.Imm)
			{
				// Get the HGLOBAL
				hGlbRht = lpToken->tkData.tkSym->stData.stGlbData;

				break;		// Continue with HGLOBAL processing
			} // End IF

			// Handle the immediate case

			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

			// Split cases based upon the symbol table immediate type
			switch (lpToken->tkData.tkSym->stFlags.ImmType)
			{
				case IMMTYPE_BOOL:
				case IMMTYPE_INT:
				case IMMTYPE_FLOAT:
					bRet = FALSE;

					break;

				case IMMTYPE_CHAR:
					aplChar = lpToken->tkData.tkSym->stData.stChar;

					bScalar = TRUE;

					goto MAKE_VECTOR;
			} // End SWITCH

			break;

		case TKT_VARIMMED:
			// Split cases based upon the token immediate type
			switch (lpToken->tkFlags.ImmType)
			{
				case IMMTYPE_BOOL:
				case IMMTYPE_INT:
				case IMMTYPE_FLOAT:
					bRet = FALSE;

					break;

				case IMMTYPE_CHAR:
					aplChar = lpToken->tkData.tkChar;

					bScalar = TRUE;

					goto MAKE_VECTOR;
			} // End SWITCH

			goto NORMAL_EXIT;

		case TKT_LISTPAR:	// The tkData is an HGLOBAL of an array of HGLOBALs
			ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
									   lpToken);
			return FALSE;

		case TKT_STRING:	// tkData is an HGLOBAL of an array of ???
		case TKT_VARARRAY:	// tkData is an HGLOBAL of an array of ???
			// Get the HGLOBAL
			hGlbRht = lpToken->tkData.tkGlbData;

			break;			// Continue with HGLOBAL processing

		defstop
			return FALSE;
	} // End SWITCH

	// st/tkData is a valid HGLOBAL variable array
	Assert (IsGlbTypeVarDir (hGlbRht));

	// Lock the memory to get a ptr to it
	lpMemRht = MyGlobalLock (ClrPtrTypeDirGlb (hGlbRht));

#define lpHeader	((LPVARARRAY_HEADER) lpMemRht)

	// Get the right arg NELM & rank
	aplNELMRht = lpHeader->NELM;
	aplRankRht = lpHeader->Rank;

	// Skip over the header and dimensions to the data
	lpMemRht = VarArrayBaseToData (lpHeader, aplRankRht);

	// Check for scalar or vector
	if (aplRankRht > 1)
	{
		lpwErrMsg = ERRMSG_RANK_ERROR APPEND_NAME;

		bRet = FALSE;
	} else
	// Split cases based upon the array type
	switch (lpHeader->ArrType)
	{
		case ARRAY_BOOL:
		case ARRAY_INT:
		case ARRAY_FLOAT:
		case ARRAY_HETERO:
		case ARRAY_NESTED:
			bRet = FALSE;

			break;

		case ARRAY_CHAR:
			aplChar = *(LPAPLCHAR) lpMemRht;

			bScalar = (aplRankRht EQ 0);

			break;

		defstop
			break;
	} // End IF/ELSE/SWITCH

#undef	lpHeader

	// We no longer need this ptr
	MyGlobalUnlock (ClrPtrTypeDirGlb (hGlbRht)); lpMemRht = NULL;

	if (!bRet)
		goto ERROR_EXIT;

	// If the argument is a scalar, make a vector out of it
	if (bScalar)
		goto MAKE_VECTOR;

	// If this is []WSID, expand the name
	if (bWSID)
	{
		// Lock the memory to get a ptr to it
		lpMemRht = MyGlobalLock (ClrPtrTypeDirGlb (hGlbRht));

		// Skip over the header and dimensions to the data
		lpMemRht = VarArrayBaseToData (lpMemRht, 1);

#define lpMemChar		((LPAPLCHAR) lpMemRht)

		// Delete leading blanks
		aplNELMRes = aplNELMRht;
		while (aplNELMRes && *lpMemChar EQ L' ')
		{
			aplNELMRes--;
			lpMemChar++;
		} // End WHILE

		// Delete trailing blanks
		while (aplNELMRes && lpMemChar[aplNELMRes - 1] EQ L' ')
			aplNELMRes--;

#undef	lpMemChar

		// If there's anything left, ...
		if (aplNELMRes)
		{
			// Copy the right arg to a location with a terminating zero
			CopyMemory (lpwszTemp, lpMemRht, (UINT) aplNELMRht * sizeof (APLCHAR));
			lpwszTemp[aplNELMRht] = L'\0';

			// Convert the []WSID workspace name into a canonical form
			MakeWorkspaceNameCanonical (lpwszTemp, lpwszTemp, wszSaveDir);

			// Get length of the name as the NELM
			aplNELMRes = lstrlenW (lpwszTemp);

			// We no longer need this ptr
			MyGlobalUnlock (ClrPtrTypeDirGlb (hGlbRht)); lpMemRht = NULL;

			goto ALLOC_VECTOR;
		} else
		{
			// The result is an empry char vector
			hGlbRes = hGlbMTChar;

			// We no longer need this ptr
			MyGlobalUnlock (ClrPtrTypeDirGlb (hGlbRht)); lpMemRht = NULL;

			goto NORMAL_EXIT;
		} // End IF/ELSE
	} // End IF

	// Copy the right arg global as the result
	hGlbRes = CopySymGlbDir (hGlbRht);

	goto NORMAL_EXIT;

MAKE_VECTOR:
	aplNELMRes = 1;

	// If the Quad-var is []WSID, expand the name to be fully-qualified
	if (bWSID)
	{
		WCHAR wsz[2];

		// Save the char as a string
		wsz[0] = aplChar;
		wsz[1] = L'\0';

		// Convert the []WSID workspace name into a canonical form
		MakeWorkspaceNameCanonical (lpwszTemp, wsz, wszSaveDir);

		// Get length of the name as the NELM
		aplNELMRes = lstrlenW (lpwszTemp);
	} // End IF
ALLOC_VECTOR:
	// Calculate space needed for the result
	//	 (a character vector)
	ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, 1);

	// Allocate space for the result
	hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
	if (hGlbRes NE NULL)
	{
		// Lock the memory to get a ptr to it
		lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader	((LPVARARRAY_HEADER) lpMemRes)

		// Fill in the header values
		lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
		lpHeader->ArrType	 = ARRAY_CHAR;
////////lpHeader->Perm		 = 0;		// Already zero from GHND
////////lpHeader->SysVar	 = 0;		// Already zero from GHND
		lpHeader->RefCnt	 = 1;
		lpHeader->NELM		 = aplNELMRes;
		lpHeader->Rank		 = 1;

#undef	lpHeader

		// Save the dimension
		*VarArrayBaseToDim (lpMemRes) = aplNELMRes;

		// Skip over the header and dimensions to the data
		lpMemRes = VarArrayBaseToData (lpMemRes, 1);

		if (bWSID)
			CopyMemory (lpMemRes, lpwszTemp, (UINT) aplNELMRes * sizeof (APLCHAR));
		else
			*((LPAPLCHAR) lpMemRes) = aplChar;

		// We no longer need this ptr
		MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
	} else
		bRet = FALSE;
ERROR_EXIT:
NORMAL_EXIT:
	// If in error, set error message;
	//	 otherwise, save the value in the name
	if (!bRet)
		ErrorMessageIndirectToken (lpwErrMsg,
								   lpToken);
	else
	{
		// Free the old value
		FreeResultGlobalVar (ClrPtrTypeDirGlb (lptkName->tkData.tkSym->stData.stGlbData));

		// Save as new value
		lptkName->tkData.tkSym->stData.stGlbData = MakeGlbTypeGlb (hGlbRes);
		lptkName->tkFlags.NoDisplay = 1;
	} // End IF

	return bRet;
#undef	lpHeader
} // End ValidateCharVector_EM
#undef	APPEND_NAME


//***************************************************************************
//	$ValidateIntegerVector_EM
//
//	Validate a value about to be assigned to a integer vector system var.
//
//	We allow any integer scalar or vector.
//
//	The order of error checking is RANK, LENGTH, DOMAIN.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ValidateIntegerVector_EM"
#else
#define APPEND_NAME
#endif

BOOL ValidateIntegerVector_EM
	(LPTOKEN  lptkName, 			// Ptr to name token
	 LPTOKEN  lpToken)				// Ptr to value token

{
	LPVOID	 lpMemRht,				// Ptr to right arg global memory
			 lpMemRes;				// Ptr to result	...
	BOOL	 bRet = TRUE,			// TRUE iff result is valid
			 bScalar = FALSE;		// TRUE iff right arg is a scalar
	LPWCHAR  lpwErrMsg = ERRMSG_DOMAIN_ERROR APPEND_NAME;
	APLINT	 aplInteger;			// Right arg first integer
	APLSTYPE aplTypeRht;			// Right arg storage type
	APLNELM  aplNELMRht,			// Right arg NELM
			 aplNELMRes;			// Result	 ...
	APLRANK  aplRankRht;			// Right arg rank
	HGLOBAL  hGlbRht,				// Right arg global memory handle
			 hGlbRes;				// Result	 ...
	APLUINT  ByteRes,				// # bytes in the result
			 uRht;					// Loop counter

	// Split cases based upon the token type
	switch (lpToken->tkFlags.TknType)
	{
		case TKT_VARNAMED:
			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

			// Split cases based upon the symbol table immediate type
			if (!lpToken->tkData.tkSym->stFlags.Imm)
			{
				// Get the HGLOBAL
				hGlbRht = lpToken->tkData.tkSym->stData.stGlbData;

				break;		// Continue with HGLOBAL processing
			} // End IF

			// Handle the immediate case

			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

			// Split cases based upon the symbol table immediate type
			switch (lpToken->tkData.tkSym->stFlags.ImmType)
			{
				case IMMTYPE_BOOL:
				case IMMTYPE_INT:
					aplInteger = lpToken->tkData.tkSym->stData.stInteger;

					bScalar = TRUE;

					goto MAKE_VECTOR;

				case IMMTYPE_FLOAT:
					// Convert the value to an integer using System CT
					aplInteger = FloatToAplint_SCT (lpToken->tkData.tkSym->stData.stFloat,
												   &bRet);
					if (bRet)
					{
						bScalar = TRUE;

						goto MAKE_VECTOR;
					} // End IF

					break;

				case IMMTYPE_CHAR:
					bRet = FALSE;

					break;

			} // End SWITCH

			break;

		case TKT_VARIMMED:
			// Split cases based upon the token immediate type
			switch (lpToken->tkFlags.ImmType)
			{
				case IMMTYPE_BOOL:
				case IMMTYPE_INT:
					aplInteger = lpToken->tkData.tkInteger;

					bScalar = TRUE;

					goto MAKE_VECTOR;

				case IMMTYPE_FLOAT:
					// Convert the value to an integer using System CT
					aplInteger = FloatToAplint_SCT (lpToken->tkData.tkFloat,
												   &bRet);
					if (bRet)
					{
						bScalar = TRUE;

						goto MAKE_VECTOR;
					} // End IF

					break;

				case IMMTYPE_CHAR:
					bRet = FALSE;

					break;
			} // End SWITCH

			goto NORMAL_EXIT;

		case TKT_LISTPAR:	// The tkData is an HGLOBAL of an array of HGLOBALs
			ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
									   lpToken);
			return FALSE;

		case TKT_VARARRAY:	// tkData is an HGLOBAL of an array of ???
			// Get the HGLOBAL
			hGlbRht = lpToken->tkData.tkGlbData;

			break;			// Continue with HGLOBAL processing

		defstop
			return FALSE;
	} // End SWITCH

	// st/tkData is a valid HGLOBAL variable array
	Assert (IsGlbTypeVarDir (hGlbRht));

	// Lock the memory to get a ptr to it
	lpMemRht = MyGlobalLock (ClrPtrTypeDirGlb (hGlbRht));

#define lpHeader	((LPVARARRAY_HEADER) lpMemRht)

	// Get the Array Type, NELM, and Rank
	aplTypeRht = lpHeader->ArrType;
	aplNELMRht = lpHeader->NELM;
	aplRankRht = lpHeader->Rank;

#undef	lpHeader

	// Skip over the header and dimensions to the data
	lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

	// Check for scalar or vector
	if (aplRankRht > 1)
	{
		lpwErrMsg = ERRMSG_RANK_ERROR APPEND_NAME;

		bRet = FALSE;
	} else
	// Split cases based upon the array type
	switch (aplTypeRht)
	{
		case ARRAY_BOOL:
		case ARRAY_INT:
			aplInteger = *(LPAPLINT) lpMemRht;

			bScalar = (aplRankRht EQ 0);

			break;

		case ARRAY_FLOAT:
			// Convert the value to an integer using System CT
			aplInteger = FloatToAplint_SCT (*(LPAPLFLOAT) lpMemRht,
										   &bRet);
			bScalar = (aplRankRht EQ 0);

			break;

		case ARRAY_CHAR:
		case ARRAY_HETERO:
		case ARRAY_NESTED:
			bRet = FALSE;

			break;

		defstop
			break;
	} // End IF/ELSE/SWITCH

	if (!bRet)
		goto ERROR_EXIT;

	// If the argument is a scalar, make a vector out of it
	if (bScalar)
		goto MAKE_VECTOR;

	// If the right arg is a simple Boolean/Integer, copy the right arg
	if (IsSimpleInt (aplTypeRht))
		// Copy the right arg global as the result
		hGlbRes = CopySymGlbDir (hGlbRht);
	else
	{
		// The right arg is a vector of floating point numbers

		// Calculate space needed for a duplicate vector of integers
		ByteRes = CalcArraySize (ARRAY_INT, aplNELMRht, 1);

		// Allocate space for the result
		hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
		if (hGlbRes NE NULL)
		{
			// Lock the memory to get a ptr to it
			lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader	((LPVARARRAY_HEADER) lpMemRes)

			// Fill in the header values
			lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
			lpHeader->ArrType	 = ARRAY_INT;
////////////lpHeader->Perm		 = 0;		// Already zero from GHND
////////////lpHeader->SysVar	 = 0;		// Already zero from GHND
			lpHeader->RefCnt	 = 1;
			lpHeader->NELM		 = aplNELMRht;
			lpHeader->Rank		 = 1;

#undef	lpHeader

			// Save the dimension
			*VarArrayBaseToDim (lpMemRes) = aplNELMRht;

			// Skip over the header and dimensions to the data
			lpMemRes = VarArrayBaseToData (lpMemRes, 1);

			// Loop through the right arg, converting to integers
			for (uRht = 0; uRht < aplNELMRht; uRht++)
			{
				// Convert the value to an integer using System CT
				aplInteger = FloatToAplint_SCT (*((LPAPLFLOAT) lpMemRht)++,
											   &bRet);
				if (bRet)
					*((LPAPLINT) lpMemRes)++ = aplInteger;
				else
					break;
			} // End FOR

			// We no longer need this ptr
			MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
		} else
			bRet = FALSE;
	} // End IF/ELSE

	goto NORMAL_EXIT;

MAKE_VECTOR:
	aplNELMRes = 1;

	// Calculate space needed for the result
	//	 (an integer vector)
	ByteRes = CalcArraySize (ARRAY_INT, aplNELMRes, 1);

	// Allocate space for the result
	hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
	if (hGlbRes NE NULL)
	{
		// Lock the memory to get a ptr to it
		lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader	((LPVARARRAY_HEADER) lpMemRes)

		// Fill in the header values
		lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
		lpHeader->ArrType	 = ARRAY_INT;
////////lpHeader->Perm		 = 0;		// Already zero from GHND
////////lpHeader->SysVar	 = 0;		// Already zero from GHND
		lpHeader->RefCnt	 = 1;
		lpHeader->NELM		 = aplNELMRes;
		lpHeader->Rank		 = 1;

#undef	lpHeader

		// Save the dimension
		*VarArrayBaseToDim (lpMemRes) = aplNELMRes;

		// Skip over the header and dimensions to the data
		lpMemRes = VarArrayBaseToData (lpMemRes, 1);

		*((LPAPLINT) lpMemRes) = aplInteger;

		// We no longer need this ptr
		MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
	} else
		bRet = FALSE;
ERROR_EXIT:
NORMAL_EXIT:
	if (hGlbRht && lpMemRht)
	{
		// We no longer need this ptr
		MyGlobalUnlock (ClrPtrTypeDirGlb (hGlbRht)); lpMemRht = NULL;
	} // End IF

	// If in error, set error message;
	//	 otherwise, save the value in the name
	if (!bRet)
		ErrorMessageIndirectToken (lpwErrMsg,
								   lpToken);
	else
	{
		// Free the old value
		FreeResultGlobalVar (ClrPtrTypeDirGlb (lptkName->tkData.tkSym->stData.stGlbData));

		// Save as new value
		lptkName->tkData.tkSym->stData.stGlbData = MakeGlbTypeGlb (hGlbRes);
		lptkName->tkFlags.NoDisplay = 1;
	} // End IF

	return bRet;
#undef	lpHeader
} // End ValidateIntegerVector_EM
#undef	APPEND_NAME


//***************************************************************************
//	$ValidateALX_EM
//
//	Validate a value about to be assigned to Quad-ALX
//***************************************************************************

BOOL ValidateALX_EM
	(LPTOKEN lptkName,				// Ptr to name token
	 LPTOKEN lpToken)				// Ptr to value token

{
	// Ensure the argument is a character scalar (promoted to a vector)
	//	 or a character vector.
	return ValidateCharVector_EM (lptkName, lpToken, FALSE);
} // End ValidateALX_EM


//***************************************************************************
//	$ValidateCT_EM
//
//	Validate a value about to be assigned to Quad-CT
//***************************************************************************

BOOL ValidateCT_EM
	(LPTOKEN lptkName,				// Ptr to name token
	 LPTOKEN lpToken)				// Ptr to value token

{
	// Ensure the argument is a real scalar or
	//	 one-element vector (demoted to a scalar)
	//	 between 0 and 1E-10 inclusive.
	return ValidateFloat_EM (lptkName, lpToken, 0, 1E-10);
} // End ValidateCT_EM


//***************************************************************************
//	$ValidateELX_EM
//
//	Validate a value about to be assigned to Quad-ELX
//***************************************************************************

BOOL ValidateELX_EM
	(LPTOKEN lptkName,				// Ptr to name token
	 LPTOKEN lpToken)				// Ptr to value token

{
	// Ensure the argument is a character scalar (promoted to a vector)
	//	 or a character vector.
	return ValidateCharVector_EM (lptkName, lpToken, FALSE);
} // End ValidateELX_EM


//***************************************************************************
//	$ValidateIO_EM
//
//	Validate a value about to be assigned to Quad-IO
//
//	We allow any numeric singleton whose value is 0 or 1.
//
//	The order of error checking is RANK, LENGTH, DOMAIN.
//***************************************************************************

BOOL ValidateIO_EM
	(LPTOKEN lptkName,				// Ptr to name token
	 LPTOKEN lpToken)				// Ptr to value token

{
	// Ensure the argument is a Boolean scalar or
	//	 one-element vector (demoted to a scalar).
	return ValidateBoolean_EM (lptkName, lpToken);
} // End ValidateIO_EM


//***************************************************************************
//	$ValidateLX_EM
//
//	Validate a value about to be assigned to Quad-LX
//***************************************************************************

BOOL ValidateLX_EM
	(LPTOKEN lptkName,				// Ptr to name token
	 LPTOKEN lpToken)				// Ptr to value token

{
	// Ensure the argument is a character scalar (promoted to a vector)
	//	 or a character vector.
	return ValidateCharVector_EM (lptkName, lpToken, FALSE);
} // End ValidateLX_EM


//***************************************************************************
//	$ValidatePP_EM
//
//	Validate a value about to be assigned to Quad-PP
//***************************************************************************

BOOL ValidatePP_EM
	(LPTOKEN lptkName,				// Ptr to name token
	 LPTOKEN lpToken)				// Ptr to value token

{
	// Ensure the argument is an integer scalar or
	//	 one-element vector (demoted to a scalar)
	//	 in the range for []PP.
	return ValidateInteger_EM (lptkName,			// Ptr to token name
							   lpToken, 			// Ptr to token value
							   DEF_MIN_QUADPP,		// Minimum value
							   DEF_MAX_QUADPP); 	// Maximum ...
} // End ValidatePP_EM


//***************************************************************************
//	$ValidatePR_EM
//
//	Validate a value about to be assigned to Quad-PR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ValidatePR_EM"
#else
#define APPEND_NAME
#endif

BOOL ValidatePR_EM
	(LPTOKEN lptkName,				// Ptr to name token
	 LPTOKEN lpToken)				// Ptr to value token

{
	HGLOBAL 	 hGlbRht;		// Right arg global memory handle
	LPVOID		 lpMemRht;		// Ptr to right arg global memory
	BOOL		 bRet = TRUE;	// TRUE iff result is valid
	LPWCHAR 	 lpwErrMsg = ERRMSG_DOMAIN_ERROR APPEND_NAME;
	HGLOBAL 	 hGlbPTD;		// PerTabData global memory handle
	LPPERTABDATA lpMemPTD;		// Ptr to PerTabData global memory

	// Get the thread's PerTabData global memory handle
	hGlbPTD = TlsGetValue (dwTlsPerTabData);

	// Lock the memory to get a ptr to it
	lpMemPTD = MyGlobalLock (hGlbPTD);

	// Ensure the argument is a character scalar, or
	//	 one-element vector (demoted to a scalar), or
	//	 an empty vector.

	// Split cases based upon the token type
	switch (lpToken->tkFlags.TknType)
	{
		case TKT_VARNAMED:
			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

			if (!lpToken->tkData.tkSym->stFlags.Imm)
			{
				// Get the HGLOBAL
				hGlbRht = lpToken->tkData.tkSym->stData.stGlbData;

				break;		// Continue with HGLOBAL processing
			} // End IF

			// Handle the immediate case

			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

			// Split cases based upon the token immediate type
			switch (lpToken->tkData.tkSym->stFlags.ImmType)
			{
				case IMMTYPE_BOOL:
				case IMMTYPE_INT:
				case IMMTYPE_FLOAT:
					bRet = FALSE;

					break;

				case IMMTYPE_CHAR:
					lpMemPTD->cQuadPR = lpToken->tkData.tkSym->stData.stChar;

					goto MAKE_SCALAR;
			} // End SWITCH

			break;

		case TKT_VARIMMED:
			// Split cases based upon the token immediate type
			switch (lpToken->tkFlags.ImmType)
			{
				case IMMTYPE_BOOL:
				case IMMTYPE_INT:
				case IMMTYPE_FLOAT:
					bRet = FALSE;

					break;

				case IMMTYPE_CHAR:
					lpMemPTD->cQuadPR = lpToken->tkData.tkChar;

					goto MAKE_SCALAR;
			} // End SWITCH

			break;

		case TKT_LISTPAR:	// The tkData is an HGLOBAL of an array of HGLOBALs
			ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
									   lptkName);
			return FALSE;

		case TKT_STRING:	// tkData is an HGLOBAL of an array of ???
		case TKT_VARARRAY:	// tkData is an HGLOBAL of an array of ???
			// Get the HGLOBAL
			hGlbRht = lpToken->tkData.tkGlbData;

			break;			// Continue with HGLOBAL processing

		defstop
			return FALSE;
	} // End SWITCH

	// tkData is a valid HGLOBAL variable array
	Assert (IsGlbTypeVarDir (hGlbRht));

	// Lock the memory to get a ptr to it
	lpMemRht = MyGlobalLock (ClrPtrTypeDirGlb (hGlbRht));

#define lpHeader	((LPVARARRAY_HEADER) lpMemRht)

	// Skip over the header and dimensions to the data
	lpMemRht = VarArrayBaseToData (lpHeader, lpHeader->Rank);

	// Check for scalar or vector
	if (lpHeader->Rank > 1)
	{
		lpwErrMsg = ERRMSG_RANK_ERROR APPEND_NAME;

		bRet = FALSE;
	} else
	// Check for empty or singleton
	if (lpHeader->NELM > 1)
	{
		lpwErrMsg = ERRMSG_LENGTH_ERROR APPEND_NAME;

		bRet = FALSE;
	} else
	// Split cases based upon the array type
	switch (lpHeader->ArrType)
	{
		case ARRAY_BOOL:
		case ARRAY_INT:
		case ARRAY_FLOAT:
		case ARRAY_HETERO:
		case ARRAY_NESTED:
			bRet = FALSE;

			break;

		case ARRAY_CHAR:
			// Izit an empty vector?
			if (lpHeader->Rank EQ 1 && lpHeader->NELM EQ 0)
				lpMemPTD->cQuadPR = 0;
			else
				lpMemPTD->cQuadPR = *(LPAPLCHAR) lpMemRht;
			break;

		defstop
			break;
	} // End IF/ELSE/SWITCH

	// We no longer need this ptr
	MyGlobalUnlock (ClrPtrTypeDirGlb (hGlbRht)); lpMemRht = NULL;

#undef	lpHeader

MAKE_SCALAR:
	// If in error, set error message;
	//	 otherwise, save the value in the name
	if (!bRet)
		ErrorMessageIndirectToken (lpwErrMsg,
								   lpToken);
	else
	{
		lptkName->tkData.tkSym->stFlags.Imm = (lpMemPTD->cQuadPR NE 0);
		if (lpMemPTD->cQuadPR EQ 0)
			lptkName->tkData.tkSym->stData.stGlbData = MakeGlbTypeGlb (hGlbMTChar);
		else
			lptkName->tkData.tkSym->stData.stChar = lpMemPTD->cQuadPR;
		lptkName->tkFlags.NoDisplay = 1;
	} // End IF

	// We no longer need this ptr
	MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

	return bRet;
} // End ValidatePR_EM
#undef	APPEND_NAME


//***************************************************************************
//	$ValidatePW_EM
//
//	Validate a value about to be assigned to Quad-PW
//***************************************************************************

BOOL ValidatePW_EM
	(LPTOKEN lptkName,				// Ptr to name token
	 LPTOKEN lpToken)				// Ptr to value token

{
	// Ensure the argument is an integer scalar or
	//	 one-element vector (demoted to a scalar)
	//	 in the range for []PW.
	return ValidateInteger_EM (lptkName,			// Ptr to token name
							   lpToken, 			// Ptr to token value
							   DEF_MIN_QUADPW,		// Minimum value
							   DEF_MAX_QUADPW); 	// Maximum ...
} // End ValidatePW_EM


//***************************************************************************
//	$ValidateRL_EM
//
//	Validate a value about to be assigned to Quad-RL
//***************************************************************************

BOOL ValidateRL_EM
	(LPTOKEN lptkName,				// Ptr to name token
	 LPTOKEN lpToken)				// Ptr to value token

{
	// Ensure the argument is an integer scalar or
	//	 one-element vector (demoted to a scalar)
	//	 in the range for []RL.
	return ValidateInteger_EM (lptkName,			// Ptr to token name
							   lpToken, 			// Ptr to token value
							   DEF_MIN_QUADRL,		// Minimum value
							   DEF_MAX_QUADRL); 	// Maximum ...
} // End ValidateRL_EM


//***************************************************************************
//	$ValidateSA_EM
//
//	Validate a value about to be assigned to Quad-SA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ValidateSA_EM"
#else
#define APPEND_NAME
#endif

BOOL ValidateSA_EM
	(LPTOKEN lptkName,				// Ptr to name token
	 LPTOKEN lpToken)				// Ptr to value token

{
	APLSTYPE	 aplTypeRht;	// Right arg storage type
	APLNELM 	 aplNELMRht;	// Right arg NELM
	APLRANK 	 aplRankRht;	// Right arg rank
	HGLOBAL 	 hGlbRht,		// Right arg global memory handle
				 hGlbRes;		// Result	 ...
	LPVOID		 lpMemRht;		// Ptr to right arg global memory
	BOOL		 bRet = TRUE;	// TRUE iff result is valid
	LPWCHAR 	 lpwErrMsg = ERRMSG_DOMAIN_ERROR APPEND_NAME;
	HGLOBAL 	 hGlbPTD;		// PerTabData global memory handle
	LPPERTABDATA lpMemPTD;		// Ptr to PerTabData global memory

	// Get the thread's PerTabData global memory handle
	hGlbPTD = TlsGetValue (dwTlsPerTabData);

	// Lock the memory to get a ptr to it
	lpMemPTD = MyGlobalLock (hGlbPTD);

	// Ensure the argument is a character scalar (promoted to a vector)
	//	 or vector, and a valid Stop Action value
	//	 ('', 'EXIT', 'ERROR', 'CLEAR', 'OFF') uppercase only

	// Split cases based upon the token type
	switch (lpToken->tkFlags.TknType)
	{
		case TKT_VARNAMED:
			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

			if (!lpToken->tkData.tkSym->stFlags.Imm)
			{
				// Get the HGLOBAL
				hGlbRht = lpToken->tkData.tkSym->stData.stGlbData;

				break;		// Continue with HGLOBAL processing
			} // End IF

			// Handle the immediate case

			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

			// Fall through to common error code

		case TKT_VARIMMED:
			ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
									   lptkName);
			return FALSE;

		case TKT_LISTPAR:	// The tkData is an HGLOBAL of an array of HGLOBALs
			ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
									   lptkName);
			return FALSE;

		case TKT_STRING:	// tkData is an HGLOBAL of an array of ???
		case TKT_VARARRAY:	// tkData is an HGLOBAL of an array of ???
			// Get the HGLOBAL
			hGlbRht = lpToken->tkData.tkGlbData;

			break;			// Continue with HGLOBAL processing

		defstop
			return FALSE;
	} // End SWITCH

	// tkData is a valid HGLOBAL variable array
	Assert (IsGlbTypeVarDir (hGlbRht));

	// Lock the memory to get a ptr to it
	lpMemRht = MyGlobalLock (ClrPtrTypeDirGlb (hGlbRht));

#define lpHeader	((LPVARARRAY_HEADER) lpMemRht)

	// Get the Array Type, NELM, and Rank
	aplTypeRht = lpHeader->ArrType;
	aplNELMRht = lpHeader->NELM;
	aplRankRht = lpHeader->Rank;

#undef	lpHeader

	// Skip over the header and dimensions to the data
	lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

	// Check for vector
	if (aplRankRht NE 1)
	{
		lpwErrMsg = ERRMSG_RANK_ERROR APPEND_NAME;

		bRet = FALSE;
	} else
	// Split cases based upon the array type
	switch (aplTypeRht)
	{
		case ARRAY_CHAR:
			// Check for the various possible values for []SA
			switch (aplNELMRht)
			{
				case 0: 	// ""
					hGlbRes = hGlbSAEmpty;
					lpMemPTD->bQuadxSA = SAVAL_Empty;

					break;

				case 3: 	// "OFF"
					if (memcmp (lpMemRht, SAOff  , sizeof (SAOff)	- sizeof (APLCHAR)) EQ 0)
					{
						hGlbRes = hGlbSAOff;
						lpMemPTD->bQuadxSA = SAVAL_Off;
					} else
						bRet = FALSE;
					break;

				case 4: 	// "EXIT"
					if (memcmp (lpMemRht, SAExit , sizeof (SAExit)	- sizeof (APLCHAR)) EQ 0)
					{
						hGlbRes = hGlbSAExit;
						lpMemPTD->bQuadxSA = SAVAL_Exit;
					} else
						bRet = FALSE;
					break;

				case 5: 	// "CLEAR", "ERROR"
					if (memcmp (lpMemRht, SAClear, sizeof (SAClear) - sizeof (APLCHAR)) EQ 0)
					{
						hGlbRes = hGlbSAClear;
						lpMemPTD->bQuadxSA = SAVAL_Clear;
					} else
					if (memcmp (lpMemRht, SAError, sizeof (SAError) - sizeof (APLCHAR)) EQ 0)
					{
						hGlbRes = hGlbSAError;
						lpMemPTD->bQuadxSA = SAVAL_Error;
					} else
						bRet = FALSE;
					break;

				default:
					bRet = FALSE;

					break;
			} // End SWITCH

			break;

		case ARRAY_BOOL:
		case ARRAY_INT:
		case ARRAY_APA:
		case ARRAY_FLOAT:
		case ARRAY_HETERO:
		case ARRAY_NESTED:
			bRet = FALSE;

			break;

		defstop
			break;
	} // End IF/ELSE/SWITCH

	// We no longer need this ptr
	MyGlobalUnlock (ClrPtrTypeDirGlb (hGlbRht)); lpMemRht = NULL;

	// If in error, set error message;
	//	 otherwise, save the value in the name
	if (!bRet)
		ErrorMessageIndirectToken (lpwErrMsg,
								   lpToken);
	else
	{
		// Free the old value
		FreeResultGlobalVar (ClrPtrTypeDirGlb (lptkName->tkData.tkSym->stData.stGlbData));

		// Save as new value
		lptkName->tkData.tkSym->stData.stGlbData = MakeGlbTypeGlb (hGlbRes);
		lptkName->tkFlags.NoDisplay = 1;
	} // End IF

	// We no longer need this ptr
	MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

	return bRet;
} // End ValidateSA_EM
#undef	APPEND_NAME


//***************************************************************************
//	$ValidateWSID_EM
//
//	Validate a value about to be assigned to Quad-WSID
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ValidateWSID_EM"
#else
#define APPEND_NAME
#endif

BOOL ValidateWSID_EM
	(LPTOKEN lptkName,				// Ptr to name token
	 LPTOKEN lpToken)				// Ptr to value token

{
	// Ensure the argument is a character scalar (promoted to a vector)
	//	 or a character vector.
	return ValidateCharVector_EM (lptkName, lpToken, TRUE);
} // End ValidateWSID_EM
#undef	APPEND_NAME


//***************************************************************************
//	$InitSystemVars
//
//	Initialize all system vars
//***************************************************************************

BOOL InitSystemVars
	(void)

{
	HGLOBAL 	 hGlbPTD;		// PerTabData global memory handle
	LPPERTABDATA lpMemPTD;		// Ptr to PerTabData global memory

	// Get the thread's PerTabData global memory handle
	hGlbPTD = TlsGetValue (dwTlsPerTabData);

	// Lock the memory to get a ptr to it
	lpMemPTD = MyGlobalLock (hGlbPTD);

	// Set the validation routines
	aSysVarValid[SYSVAR_ALX ] = ValidateALX_EM ;
	aSysVarValid[SYSVAR_CT	] = ValidateCT_EM  ;
	aSysVarValid[SYSVAR_ELX ] = ValidateELX_EM ;
	aSysVarValid[SYSVAR_IO	] = ValidateIO_EM  ;
	aSysVarValid[SYSVAR_LX	] = ValidateLX_EM  ;
	aSysVarValid[SYSVAR_PP	] = ValidatePP_EM  ;
	aSysVarValid[SYSVAR_PR	] = ValidatePR_EM  ;
	aSysVarValid[SYSVAR_PW	] = ValidatePW_EM  ;
	aSysVarValid[SYSVAR_RL	] = ValidateRL_EM  ;
	aSysVarValid[SYSVAR_SA	] = ValidateSA_EM  ;
	aSysVarValid[SYSVAR_WSID] = ValidateWSID_EM;

	// Assign default values to the system vars
	if (!AssignCharVector_EM (WS_UTF16_QUAD L"alx" , hGlbQuadALX_CWS   , SYSVAR_ALX , lpMemPTD->lpSymQuadALX      )) return FALSE;   // Attention Latent Expression
	if (!AssignRealScalar_EM (WS_UTF16_QUAD L"ct"  , fQuadCT_CWS       , SYSVAR_CT  , lpMemPTD->lpSymQuadCT       )) return FALSE;   // Comparison Tolerance
	if (!AssignCharVector_EM (WS_UTF16_QUAD L"elx" , hGlbQuadELX_CWS   , SYSVAR_ELX , lpMemPTD->lpSymQuadELX      )) return FALSE;   // Error Latent Expression
	if (!AssignBoolScalar_EM (WS_UTF16_QUAD L"io"  , bQuadIO_CWS       , SYSVAR_IO  , lpMemPTD->lpSymQuadIO       )) return FALSE;   // Index Origin
	if (!AssignCharVector_EM (WS_UTF16_QUAD L"lx"  , hGlbQuadLX_CWS    , SYSVAR_LX  , lpMemPTD->lpSymQuadLX       )) return FALSE;   // Latent Expression
	if (!AssignIntScalar_EM  (WS_UTF16_QUAD L"pp"  , uQuadPP_CWS       , SYSVAR_PP  , lpMemPTD->lpSymQuadPP       )) return FALSE;   // Printing Precision
	if (cQuadPR_CWS EQ 0)
	{
		if (!AssignCharVector_EM (WS_UTF16_QUAD L"pr", hGlbQuadPR_CWS  , SYSVAR_PR  , lpMemPTD->lpSymQuadPR       )) return FALSE;   // Prompt Replacement
	} else
	{
		if (!AssignCharScalar_EM (WS_UTF16_QUAD L"pr", cQuadPR_CWS     , SYSVAR_PR  , lpMemPTD->lpSymQuadPR       )) return FALSE;   // Prompt Replacement
	} // End IF
	if (!AssignIntScalar_EM  (WS_UTF16_QUAD L"pw"  , uQuadPW_CWS       , SYSVAR_PW  , lpMemPTD->lpSymQuadPW       )) return FALSE;   // Printing Width
	if (!AssignIntScalar_EM  (WS_UTF16_QUAD L"rl"  , uQuadRL_CWS       , SYSVAR_RL  , lpMemPTD->lpSymQuadRL       )) return FALSE;   // Random Link
	if (!AssignCharVector_EM (WS_UTF16_QUAD L"sa"  , hGlbQuadSA_CWS    , SYSVAR_SA  , lpMemPTD->lpSymQuadSA       )) return FALSE;   // Stop Action
	if (!AssignCharVector_EM (WS_UTF16_QUAD L"wsid", hGlbQuadWSID_CWS  , SYSVAR_WSID, lpMemPTD->lpSymQuadWSID     )) return FALSE;   // Workspace Identifier

	// Save the index value
	lpMemPTD->bQuadxSA = bQuadxSA_CWS;

	// We no longer need this ptr
	MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

	return TRUE;
} // End InitSystemVars


//***************************************************************************
//	End of File: sysvars.c
//***************************************************************************
