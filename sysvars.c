//***************************************************************************
//	NARS2000 -- System Variable Routines
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
#define SysFnAV_EM_YY		NULL
#define SysFnCR_EM_YY		NULL
#define SysFnDL_EM_YY		NULL
#define SysFnDM_EM_YY		NULL
#define SysFnDR_EM_YY		NULL
#define SysFnEM_EM_YY		NULL
#define SysFnERROR_EM_YY	NULL
#define SysFnES_EM_YY		NULL
#define SysFnET_EM_YY		NULL
#define SysFnEX_EM_YY		NULL
#define SysFnFX_EM_YY		NULL
#define SysFnLC_EM_YY		NULL
#define SysFnNC_EM_YY		NULL
#define SysFnNL_EM_YY		NULL
#define SysFnSIZE_EM_YY 	NULL
#define SysFnSYSID_EM_YY	NULL
#define SysFnSYSVER_EM_YY	NULL
#define SysFnTC_EM_YY		NULL
#define SysFnTCBEL_EM_YY	NULL
#define SysFnTCBS_EM_YY 	NULL
#define SysFnTCDEL_EM_YY	NULL
#define SysFnTCESC_EM_YY	NULL
#define SysFnTCFF_EM_YY 	NULL
#define SysFnTCHT_EM_YY 	NULL
#define SysFnTCLF_EM_YY 	NULL
#define SysFnTCNL_EM_YY 	NULL
#define SysFnTCNUL_EM_YY	NULL
#define SysFnTS_EM_YY		NULL
#define SysFnTYPE_EM_YY 	NULL
#define SysFnUCS_EM_YY		NULL
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
{ // Name						Valence 	  Var?	 Exec Routine		 SYS_VARS
	{WS_UTF16_QUAD L"alx"      , SYSVAR,      TRUE , NULL              , SYSVAR_ALX },  // Attention Latent Expression
	{WS_UTF16_QUAD L"ct"       , SYSVAR,      TRUE , NULL              , SYSVAR_CT  },  // Comparison Tolerance
	{WS_UTF16_QUAD L"elx"      , SYSVAR,      TRUE , NULL              , SYSVAR_ELX },  // Error Latent Expression
	{WS_UTF16_QUAD L"fc"       , SYSVAR,      TRUE , NULL              , SYSVAR_FC  },  // Format Control
	{WS_UTF16_QUAD L"ic"       , SYSVAR,      TRUE , NULL              , SYSVAR_IC  },  // Indeterminate Control
	{WS_UTF16_QUAD L"io"       , SYSVAR,      TRUE , NULL              , SYSVAR_IO  },  // Index Origin
	{WS_UTF16_QUAD L"lx"       , SYSVAR,      TRUE , NULL              , SYSVAR_LX  },  // Latent Expression
	{WS_UTF16_QUAD L"pp"       , SYSVAR,      TRUE , NULL              , SYSVAR_PP  },  // Print Precision
	{WS_UTF16_QUAD L"pr"       , SYSVAR,      TRUE , NULL              , SYSVAR_PR  },  // Prompt Replacement
	{WS_UTF16_QUAD L"pw"       , SYSVAR,      TRUE , NULL              , SYSVAR_PW  },  // Print Width
	{WS_UTF16_QUAD L"rl"       , SYSVAR,      TRUE , NULL              , SYSVAR_RL  },  // Random Link
	{WS_UTF16_QUAD L"sa"       , SYSVAR,      TRUE , NULL              , SYSVAR_SA  },  // Stop Action
	{WS_UTF16_QUAD L"wsid"     , SYSVAR,      TRUE , NULL              , SYSVAR_WSID},  // Workspace Identifier
	{WS_UTF16_QUAD L"prototype", SYSLBL,      TRUE , NULL              , 0          },  // User-defined function/operator entry point for Prototype
	{WS_UTF16_QUAD L"inverse"  , SYSLBL,      TRUE , NULL              , 0          },  // ...                                            Inverse
	{WS_UTF16_QUAD L"singleton", SYSLBL,      TRUE , NULL              , 0          },  // ...                                            Singleton

	{WS_UTF16_QUAD L"av"       ,      0,      FALSE, SysFnAV_EM_YY     , 0          },  // Atomic Vector
	{WS_UTF16_QUAD L"dm"       ,      0,      FALSE, SysFnDM_EM_YY     , 0          },  // Diagnostic Message
	{WS_UTF16_QUAD L"em"       ,      0,      FALSE, SysFnEM_EM_YY     , 0          },  // Event Message
	{WS_UTF16_QUAD L"et"       ,      0,      FALSE, SysFnET_EM_YY     , 0          },  // Event Type
	{WS_UTF16_QUAD L"lc"       ,      0,      FALSE, SysFnLC_EM_YY     , 0          },  // Line Counter
////{WS_UTF16_QUAD L"si"       ,      0,      FALSE, SysFnSI_EM_YY     , 0          },  // State Indicator
////{WS_UTF16_QUAD L"sinl"     ,      0,      FALSE, SysFnSINL_EM_YY   , 0          },  // State Indicator w/Name List
	{WS_UTF16_QUAD L"sysid"    ,      0,      FALSE, SysFnSYSID_EM_YY  , 0          },  // System Identifier
	{WS_UTF16_QUAD L"sysver"   ,      0,      FALSE, SysFnSYSVER_EM_YY , 0          },  // System Version
	{WS_UTF16_QUAD L"tc"       ,      0,      FALSE, SysFnTC_EM_YY     , 0          },  // Terminal Control Characters
	{WS_UTF16_QUAD L"tcbel"    ,      0,      FALSE, SysFnTCBEL_EM_YY  , 0          },  // Terminal Control Character, Bell
	{WS_UTF16_QUAD L"tcbs"     ,      0,      FALSE, SysFnTCBS_EM_YY   , 0          },  // Terminal Control Character, Backspace
	{WS_UTF16_QUAD L"tcdel"    ,      0,      FALSE, SysFnTCDEL_EM_YY  , 0          },  // Terminal Control Character, Delete
	{WS_UTF16_QUAD L"tcesc"    ,      0,      FALSE, SysFnTCESC_EM_YY  , 0          },  // Terminal Control Character, Escape
	{WS_UTF16_QUAD L"tcff"     ,      0,      FALSE, SysFnTCFF_EM_YY   , 0          },  // Terminal Control Character, Form Feed
	{WS_UTF16_QUAD L"tcht"     ,      0,      FALSE, SysFnTCHT_EM_YY   , 0          },  // Terminal Control Character, Horizontal Tab
	{WS_UTF16_QUAD L"tclf"     ,      0,      FALSE, SysFnTCLF_EM_YY   , 0          },  // Terminal Control Character, Line Feed
	{WS_UTF16_QUAD L"tcnl"     ,      0,      FALSE, SysFnTCNL_EM_YY   , 0          },  // Terminal Control Character, New Line
	{WS_UTF16_QUAD L"tcnul"    ,      0,      FALSE, SysFnTCNUL_EM_YY  , 0          },  // Terminal Control Character, Null
	{WS_UTF16_QUAD L"ts"       ,      0,      FALSE, SysFnTS_EM_YY     , 0          },  // Time Stamp
////{WS_UTF16_QUAD L"wa"       ,      0,      FALSE, SysFnWA_EM_YY     , 0          },  // Workspace Available

////{WS_UTF16_QUAD L"call"     ,      1,      FALSE, SysFnCALL_EM_YY   , 0          },  // Call Assembler Code
	{WS_UTF16_QUAD L"cr"       ,      1,      FALSE, SysFnCR_EM_YY     , 0          },  // Canonical Representation
////{WS_UTF16_QUAD L"crl"      ,      1,      FALSE, SysFnCRL_EM_YY    , 0          },  // Canonical Representation, Line
////{WS_UTF16_QUAD L"crlpc"    ,      1,      FALSE, SysFnCRLPC_EM_YY  , 0          },  // Canonical Representation, Public Comment
////{WS_UTF16_QUAD L"def"      ,      1,      FALSE, SysFnDEF_EM_YY    , 0          },  // Define Function
////{WS_UTF16_QUAD L"defl"     ,      1,      FALSE, SysFnDEFL_EM_YY   , 0          },  // Define Function Line
	{WS_UTF16_QUAD L"dl"       ,      1,      FALSE, SysFnDL_EM_YY     , 0          },  // Delay Execution
	{WS_UTF16_QUAD L"dr"       ,      1,      FALSE, SysFnDR_EM_YY     , 0          },  // Data Representation
////{WS_UTF16_QUAD L"erase"    ,      1,      FALSE, SysFnERASE_EM_YY  , 0          },  // Erase Names
	{WS_UTF16_QUAD L"error"    ,      1,      FALSE, SysFnERROR_EM_YY  , 0          },  // Signal Error
	{WS_UTF16_QUAD L"es"       ,      1,      FALSE, SysFnES_EM_YY     , 0          },  // Event Simulate
	{WS_UTF16_QUAD L"ex"       ,      1,      FALSE, SysFnEX_EM_YY     , 0          },  // Expunge Names
////{WS_UTF16_QUAD L"fi"       ,      1,      FALSE, SysFnFI_EM_YY     , 0          },  // Format Items
////{WS_UTF16_QUAD L"fmt"      ,      1,      FALSE, SysFnFMT_EM_YY    , 0          },  // Format
	{WS_UTF16_QUAD L"fx"       ,      1,      FALSE, SysFnFX_EM_YY     , 0          },  // Function Fix
////{WS_UTF16_QUAD L"idlist"   ,      1,      FALSE, SysFnIDLIST_EM_YY , 0          },  // Identifier List
////{WS_UTF16_QUAD L"idloc"    ,      1,      FALSE, SysFnIDLOC_EM_YY  , 0          },  // Identifier Localization
////{WS_UTF16_QUAD L"mf"       ,      1,      FALSE, SysFnMF_EM_YY     , 0          },  // Monitor Function
	{WS_UTF16_QUAD L"nc"       ,      1,      FALSE, SysFnNC_EM_YY     , 0          },  // Name Classification
	{WS_UTF16_QUAD L"nl"       ,      1,      FALSE, SysFnNL_EM_YY     , 0          },  // Name List
	{WS_UTF16_QUAD L"size"     ,      1,      FALSE, SysFnSIZE_EM_YY   , 0          },  // Size of an object
////{WS_UTF16_QUAD L"ss"       ,      1,      FALSE, SysFnSS_EM_YY     , 0          },  // Search String
////{WS_UTF16_QUAD L"stop"     ,      1,      FALSE, SysFnSTOP_EM_YY   , 0          },  // Manage Stop Points
////{WS_UTF16_QUAD L"trace"    ,      1,      FALSE, SysFnTRACE_EM_YY  , 0          },  // Manage Trace Points
	{WS_UTF16_QUAD L"type"     ,      1,      FALSE, SysFnTYPE_EM_YY   , 0          },  // Manage Trace Points
	{WS_UTF16_QUAD L"ucs"      ,      1,      FALSE, SysFnUCS_EM_YY    , 0          },  // Universal Character Set
////{WS_UTF16_QUAD L"vi"       ,      1,      FALSE, StsFnVI_EM    , 0          },  // Verify Items
////{WS_UTF16_QUAD L"vr"       ,      1,      FALSE, SysFnVR_EM_YY     , 0          },  // Vector Representation of a Function

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

	//***************************************************************
	// Create zilde
	//***************************************************************
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
	lpHeader->PermNdx	 = PERMNDX_ZILDE;	// So we don't free it
////lpHeader->SysVar	 = 0;				// Already zero from GHND
////lpHeader->RefCnt	 = 0;				// Ignore as this is perm
////lpHeader->NELM		 = 0;				// Already zero from GHND
	lpHeader->Rank		 = 1;

	// Mark as zero length
////*VarArrayBaseToDim (lpHeader) = 0;		// Already zero from GHND

	// We no longer need this ptr
	MyGlobalUnlock (hGlbZilde); lpHeader = NULL;

	//***************************************************************
	// Create initial value for []EM (3 x 0 char matrix)
	//***************************************************************
	hGlbM3x0Char = MyGlobalAlloc (GHND, (UINT) CalcArraySize (ARRAY_CHAR, 0, 2));
	if (!hGlbM3x0Char)
	{
		DbgStop (); 		// We should never get here
	} // End IF

	// Lock the memory to get a ptr to it
	lpHeader = MyGlobalLock (hGlbM3x0Char);

	// Fill in the header values
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = ARRAY_CHAR;
	lpHeader->PermNdx	 = PERMNDX_QUADEM;	// So we don't free it
////lpHeader->SysVar	 = 0;				// Already zero from GHND
////lpHeader->RefCnt	 = 0;				// Ignore as this is perm
////lpHeader->NELM		 = 0;				// Already zero from GHND
	lpHeader->Rank		 = 2;

	// Mark as shape 3 by 0
	(VarArrayBaseToDim (lpHeader))[0] = 3;
////(VarArrayBaseToDim (lpHeader))[1] = 0;	// Already zero from GHND

	// We no longer need this ptr
	MyGlobalUnlock (hGlbM3x0Char); lpHeader = NULL;

	//***************************************************************
	// Create various permanent vectors
	//***************************************************************
	hGlbQuadDM		 = MakePermCharVector (WS_QUADDM	 , PERMNDX_QUADDM);
	hGlbV0Char		 = MakePermCharVector (V0Char		 , PERMNDX_MTCHAR);
	hGlbSAEmpty 	 = hGlbV0Char;
	hGlbSAClear 	 = MakePermCharVector (SAClear		 , PERMNDX_SACLEAR);
	hGlbSAError 	 = MakePermCharVector (SAError		 , PERMNDX_SAERROR);
	hGlbSAExit		 = MakePermCharVector (SAExit		 , PERMNDX_SAEXIT);
	hGlbSAOff		 = MakePermCharVector (SAOff		 , PERMNDX_SAOFF);
	hGlbQuadWSID_CWS = hGlbV0Char;
	hGlbQuadFC_SYS	 = MakePermCharVector (DEF_QUADFC_CWS, PERMNDX_QUADFC);
	hGlbQuadIC_SYS	 = MakePermIntVector  (DEF_QUADIC_CWS, PERMNDX_QUADIC, ICNDX_LENGTH);

	// Create []AV
	MakeQuadAV ();
} // End MakePermVars
#undef	APPEND_NAME


//***************************************************************************
//	$MakePermCharVector
//
//	Make a permanent character vector
//***************************************************************************

HGLOBAL MakePermCharVector
	(LPAPLCHAR lpwc,				// Ptr to char vector
	 PERM_NDX  permNdx) 			// PERM_NDX value

{
	return MakePermVectorCom (lpwc,
							  permNdx,
							  ARRAY_CHAR,
							  sizeof (APLCHAR),
							  lstrlenW (lpwc));
} // End MakePermCharVector


//***************************************************************************
//	$MakePermIntVector
//
//	Make a permanent integer vector
//***************************************************************************

HGLOBAL MakePermIntVector
	(LPAPLINT lpwc, 				// Ptr to int vector
	 PERM_NDX permNdx,				// PERM_NDX value
	 UINT	  uLen) 				// Length of the integer vector

{
	return MakePermVectorCom (lpwc,
							  permNdx,
							  ARRAY_INT,
							  sizeof (APLINT),
							  uLen);
} // End MakePermIntVector


//***************************************************************************
//	$MakePermVectorCom
//
//	Make a permanent vector
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- MakePermVectorCom"
#else
#define APPEND_NAME
#endif

HGLOBAL MakePermVectorCom
	(LPVOID   lpwc, 				// Ptr to common vector
	 PERM_NDX permNdx,				// PERM_NDX value
	 APLSTYPE aplTypeCom,			// Common array storage type
	 UINT	  aplSizeCom,			// Size of each common item
	 UINT	  uLen) 				// Length of the common value

{
	HGLOBAL hGlbRes;
	LPVARARRAY_HEADER lpHeader;

	// Note, we can't use DbgGlobalAlloc here as the
	//	 PTD has not been allocated as yet
	hGlbRes = MyGlobalAlloc (GHND, (UINT) CalcArraySize (aplTypeCom, uLen, 1));
	if (!hGlbRes)
	{
		DbgStop (); 		// We should never get here
	} // End IF

	// Lock the memory to get a ptr to it
	lpHeader = MyGlobalLock (hGlbRes);

	// Fill in the header values
	lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
	lpHeader->ArrType	 = aplTypeCom;
	lpHeader->PermNdx	 = permNdx; // So we don't free it
////lpHeader->SysVar	 = 0;		// Already zero from GHND
////lpHeader->RefCnt	 = 0;		// Ignore as this is perm
	lpHeader->NELM		 = uLen;
	lpHeader->Rank		 = 1;

	// Save the dimension
	*VarArrayBaseToDim (lpHeader) = uLen;

	// Skip over the header and dimensions to the data
	lpHeader = VarArrayBaseToData (lpHeader, 1);

	// Copy the data to memory
	CopyMemory (lpHeader, lpwc, uLen * aplSizeCom);

	// We no longer need this ptr
	MyGlobalUnlock (hGlbRes); lpHeader = NULL;

	return hGlbRes;
} // End MakePermVectorCom
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
	hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

	// Lock the memory to get a ptr to it
	lpMemPTD = MyGlobalLock (hGlbPTD);

	ptdSysVarSym[SYSVAR_UNK ] = NULL					;
	ptdSysVarSym[SYSVAR_ALX ] = &lpMemPTD->lpSymQuadALX ;
	ptdSysVarSym[SYSVAR_CT	] = &lpMemPTD->lpSymQuadCT	;
	ptdSysVarSym[SYSVAR_ELX ] = &lpMemPTD->lpSymQuadELX ;
	ptdSysVarSym[SYSVAR_FC	] = &lpMemPTD->lpSymQuadFC	;
	ptdSysVarSym[SYSVAR_IC	] = &lpMemPTD->lpSymQuadIC	;
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
//	$AssignGlobalCWS
//
//	Assign a global to a name
//***************************************************************************

BOOL AssignGlobalCWS
	(HGLOBAL	hGlbVal_CWS,
	 UINT		SysVarValid,
	 LPSYMENTRY lpSymEntryDest)

{
	// Make a copy of the CLEAR WS value, and
	//	 save the global memory ptr
	lpSymEntryDest->stData.stGlbData = CopySymGlbDirAsGlb (hGlbVal_CWS);

	// Save the validate index
	lpSymEntryDest->stFlags.SysVarValid = SysVarValid;

	// Mark as having a value
	lpSymEntryDest->stFlags.Value = 1;

	return TRUE;
} // End AssignGlobalCWS


//***************************************************************************
//	$AssignBoolScalarCWS
//
//	Assign an Boolean scalar to a name
//***************************************************************************

BOOL AssignBoolScalarCWS
	(APLBOOL	aplBoolean,
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
} // End AssignBoolScalarCWS


//***************************************************************************
//	$AssignIntScalarCWS
//
//	Assign an integer scalar to a name
//***************************************************************************

BOOL AssignIntScalarCWS
	(APLINT 	aplInteger,
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
} // End AssignIntScalarCWS


//***************************************************************************
//	$AssignRealScalarCWS
//
//	Assign a real number scalar to a name
//***************************************************************************

BOOL AssignRealScalarCWS
	(APLFLOAT	fFloat,
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
} // End AssignRealScalarCWS


//***************************************************************************
//	$AssignCharScalarCWS
//
//	Assign a character scalar to a name
//***************************************************************************

BOOL AssignCharScalarCWS
	(APLCHAR	aplChar,
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
} // End AssignCharScalarCWS


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
	(LPTOKEN  lptkNamArg,			// Ptr to name token
	 LPTOKEN  lptkExpr, 			// Ptr to value token
	 UINT	  uValidLo, 			// Low range value (inclusive)
	 UINT	  uValidHi, 			// High ...
	 BOOL	  bRangeLimit)			// TRUE iff an incoming value outside
									//	 the given range [uValidLo, uValidHi]
									//	 is adjusted to be the closer range limit

{
	APLSTYPE aplTypeRht;			// Right arg storage type
	APLNELM  aplNELMRht;			// Right arg NELM
	APLRANK  aplRankRht;			// Right arg rank
	HGLOBAL  hGlbRht;				// Right arg global memory handle
	LPVOID	 lpMemRht;				// Ptr to right arg global memory
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
					bRet = ValidateIntegerTest (&aplInteger,		// Ptr to the integer to test
												 uValidLo,			// Low range value (inclusive)
												 uValidHi,			// High ...
												 bRangeLimit);		// TRUE iff we're range limiting
					break;

				case IMMTYPE_INT:
					// Get the value
					aplInteger = lptkExpr->tkData.tkSym->stData.stInteger;

					// Test the value
					bRet = ValidateIntegerTest (&aplInteger,		// Ptr to the integer to test
												 uValidLo,			// Low range value (inclusive)
												 uValidHi,			// High ...
												 bRangeLimit);		// TRUE iff we're range limiting
					break;

				case IMMTYPE_FLOAT:
					// Attempt to convert the float to an integer using System CT
					aplInteger = FloatToAplint_SCT (lptkExpr->tkData.tkSym->stData.stFloat,
												   &bRet);
					// Test the value
					if (bRangeLimit || bRet)
						bRet = ValidateIntegerTest (&aplInteger,	// Ptr to the integer to test
													 uValidLo,		// Low range value (inclusive)
													 uValidHi,		// High ...
													 bRangeLimit);	// TRUE iff we're range limiting
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
					bRet = ValidateIntegerTest (&aplInteger,		// Ptr to the integer to test
												 uValidLo,			// Low range value (inclusive)
												 uValidHi,			// High ...
												 bRangeLimit);		// TRUE iff we're range limiting
					break;

				case IMMTYPE_INT:
					// Get the value
					aplInteger = lptkExpr->tkData.tkInteger;

					// Test the value
					bRet = ValidateIntegerTest (&aplInteger,		// Ptr to the integer to test
												 uValidLo,			// Low range value (inclusive)
												 uValidHi,			// High ...
												 bRangeLimit);		// TRUE iff we're range limiting
					break;

				case IMMTYPE_FLOAT:
					// Attempt to convert the float to an integer using System CT
					aplInteger = FloatToAplint_SCT (lptkExpr->tkData.tkFloat,
												   &bRet);
					// Test the value
					if (bRangeLimit || bRet)
						bRet = ValidateIntegerTest (&aplInteger,	// Ptr to the integer to test
													 uValidLo,		// Low range value (inclusive)
													 uValidHi,		// High ...
													 bRangeLimit);	// TRUE iff we're range limiting
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
	lpMemRht = MyGlobalLock (ClrPtrTypeDirAsGlb (hGlbRht));

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
			aplInteger = BIT0 & *(LPAPLBOOL) lpMemRht;

			// Test the value
			bRet = ValidateIntegerTest (&aplInteger,				// Ptr to the integer to test
										 uValidLo,					// Low range value (inclusive)
										 uValidHi,					// High ...
										 bRangeLimit);				// TRUE iff we're range limiting
			break;

		case ARRAY_INT:
			// Get the value
			aplInteger = *(LPAPLINT) lpMemRht;

			// Test the value
			bRet = ValidateIntegerTest (&aplInteger,				// Ptr to the integer to test
										 uValidLo,					// Low range value (inclusive)
										 uValidHi,					// High ...
										 bRangeLimit);				// TRUE iff we're range limiting
			break;

		case ARRAY_CHAR:
		case ARRAY_HETERO:
		case ARRAY_NESTED:
			bRet = FALSE;

			break;

		case ARRAY_FLOAT:
			// Attempt to convert the float to an integer using System CT
			aplInteger = FloatToAplint_SCT (*(LPAPLFLOAT) lpMemRht,
										   &bRet);
			// Test the value
			if (bRangeLimit || bRet)
				bRet = ValidateIntegerTest (&aplInteger,			// Ptr to the integer to test
											 uValidLo,				// Low range value (inclusive)
											 uValidHi,				// High ...
											 bRangeLimit);			// TRUE iff we're range limiting
			break;

		defstop
			break;
	} // End IF/ELSE/SWITCH

	// We no longer need this ptr
	MyGlobalUnlock (ClrPtrTypeDirAsGlb (hGlbRht)); lpMemRht = NULL;
NORMAL_EXIT:
	// If in error, set error message;
	//	 otherwise, save the value in the name
	if (!bRet)
		ErrorMessageIndirectToken (lpwErrMsg,
								   lptkExpr);
	else
	{
		lptkNamArg->tkData.tkSym->stData.stInteger = aplInteger;
		lptkNamArg->tkFlags.NoDisplay = 1;
	} // End IF

	return bRet;
} // End ValidateInteger_EM
#undef	APPEND_NAME


//***************************************************************************
//	$ValidateIntegerTest
//
//	Validate an integer within a given range, possibly range limited
//***************************************************************************

BOOL ValidateIntegerTest
	(LPAPLINT lpaplInteger, 		// Ptr to the integer to test
	 APLINT   uValidLo, 			// Low range value (inclusive)
	 APLINT   uValidHi, 			// High ...
	 BOOL	  bRangeLimit)			// TRUE iff an incoming value outside
									//	 the given range [uValidLo, uValidHi]
									//	 is adjusted to be the closer range limit

{
	// If we're range limiting, ...
	if (bRangeLimit)
	{
		// If it's too small, use the lower limit
		if (*lpaplInteger < uValidLo)
			*lpaplInteger = uValidLo;
		// If it's too large, use the upper limit
		if (*lpaplInteger > uValidHi)
			*lpaplInteger = uValidHi;
		return TRUE;
	} else
		return (uValidLo <= *lpaplInteger
			 && 			*lpaplInteger <= uValidHi);
} // End ValidateIntegerTest


//***************************************************************************
//	$ValidateFloatTest
//
//	Validate a float within a given range, possibly range limited
//***************************************************************************

BOOL ValidateFloatTest
	(LPAPLFLOAT lpaplFloat, 		// Ptr to the float to test
	 APLFLOAT	fValidLo,			// Low range value (inclusive)
	 APLFLOAT	fValidHi,			// High ...
	 BOOL		bRangeLimit)		// TRUE iff an incoming value outside
									//	 the given range [uValidLo, uValidHi]
									//	 is adjusted to be the closer range limit

{
	// If we're range limiting, ...
	if (bRangeLimit)
	{
		// If it's too small, use the lower limit
		if (*lpaplFloat < fValidLo)
			*lpaplFloat = fValidLo;
		// If it's too large, use the upper limit
		if (*lpaplFloat > fValidHi)
			*lpaplFloat = fValidHi;
		return TRUE;
	} else
		return (fValidLo <= *lpaplFloat
			 && 			*lpaplFloat <= fValidHi);
} // End ValidateFloatTest


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
	(LPTOKEN  lptkNamArg,			// Ptr to name token
	 LPTOKEN  lpToken,				// Ptr to value token
	 APLFLOAT fValidLo, 			// Low range value (inclusive)
	 APLFLOAT fValidHi, 			// High ...
	 BOOL	  bRangeLimit)			// TRUE iff an incoming value outside
									//	 the given range [uValidLo, uValidHi]
									//	 is adjusted to be the closer range limit

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
					bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

					break;

				case IMMTYPE_INT:
					// Get the value
					aplFloat = (APLFLOAT) (lpToken->tkData.tkSym->stData.stInteger);

					// Test the value
					bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

					break;

				case IMMTYPE_FLOAT:
					// Get the value
					aplFloat = lpToken->tkData.tkSym->stData.stFloat;

					// Test the value
					bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

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
					bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

					break;

				case IMMTYPE_INT:
					// Get the value
					aplFloat = (APLFLOAT) (lpToken->tkData.tkInteger);

					// Test the value
					bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

					break;

				case IMMTYPE_FLOAT:
					// Get the value
					aplFloat = lpToken->tkData.tkFloat;

					// Test the value
					bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

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
	lpMemRht = MyGlobalLock (ClrPtrTypeDirAsGlb (hGlbRht));

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
			bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

			break;

		case ARRAY_INT:
			// Get the value
			aplFloat = (APLFLOAT) *(LPAPLINT) lpMemRht;

			// Test the value
			bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

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
			bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

			break;

		defstop
			break;
	} // End IF/ELSE/SWITCH

	// We no longer need this ptr
	MyGlobalUnlock (ClrPtrTypeDirAsGlb (hGlbRht)); lpMemRht = NULL;
NORMAL_EXIT:
	// If in error, set error message;
	//	 otherwise, save the value in the name
	if (!bRet)
		ErrorMessageIndirectToken (lpwErrMsg,
								   lpToken);
	else
	{
		lptkNamArg->tkData.tkSym->stData.stFloat = aplFloat;
		lptkNamArg->tkFlags.NoDisplay = 1;
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
	(LPTOKEN  lptkNamArg,			// Ptr to name token
	 LPTOKEN  lpToken,				// Ptr to value token
	 BOOL	  bWSID)				// TRUE iff this is []WSID

{
	LPVOID	 lpMemRht,				// Ptr to right arg global memory
			 lpMemRes;				// Ptr to result	...
	BOOL	 bRet = TRUE,			// TRUE iff result is valid
			 bScalar = FALSE;		// TRUE iff right arg is scalar
	LPWCHAR  lpwErrMsg = ERRMSG_DOMAIN_ERROR APPEND_NAME;
	APLCHAR  aplChar;				// Right arg first char
	APLSTYPE aplTypeRht;			// Right arg storage type
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
	lpMemRht = MyGlobalLock (ClrPtrTypeDirAsGlb (hGlbRht));

#define lpHeader	((LPVARARRAY_HEADER) lpMemRht)
	// Get the right arg Type, NELM, and Rank
	aplTypeRht = lpHeader->ArrType;
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
	switch (aplTypeRht)
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
	MyGlobalUnlock (ClrPtrTypeDirAsGlb (hGlbRht)); lpMemRht = NULL;

	if (!bRet)
		goto ERROR_EXIT;

	// If the argument is a scalar, make a vector out of it
	if (bScalar)
		goto MAKE_VECTOR;

	// If this is []WSID, expand the name
	if (bWSID)
	{
		// Lock the memory to get a ptr to it
		lpMemRht = MyGlobalLock (ClrPtrTypeDirAsGlb (hGlbRht));

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
			MyGlobalUnlock (ClrPtrTypeDirAsGlb (hGlbRht)); lpMemRht = NULL;

			goto ALLOC_VECTOR;
		} else
		{
			// The result is an empry char vector
			hGlbRes = hGlbV0Char;

			// We no longer need this ptr
			MyGlobalUnlock (ClrPtrTypeDirAsGlb (hGlbRht)); lpMemRht = NULL;

			goto NORMAL_EXIT;
		} // End IF/ELSE
	} // End IF

	// Copy the right arg global as the result
	hGlbRes = CopySymGlbDir (hGlbRht);

	goto NORMAL_EXIT;

MAKE_VECTOR:
	aplNELMRes = 1;

	// If the []var is []WSID, expand the name to be fully-qualified
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
////////lpHeader->PermNdx	 = PERMNDX_NONE;// Already zero from GHND
////////lpHeader->SysVar	 = 0;			// Already zero from GHND
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
		FreeResultGlobalVar (ClrPtrTypeDirAsGlb (lptkNamArg->tkData.tkSym->stData.stGlbData));

		// Save as new value
		lptkNamArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbRes);
		lptkNamArg->tkFlags.NoDisplay = 1;
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
	(LPTOKEN  lptkNamArg,			// Ptr to name token
	 LPTOKEN  lptkRhtArg,			// Ptr to right arg token
	 UINT	  uValidLo, 			// Low range value (inclusive)
	 UINT	  uValidHi, 			// High ...
	 BOOL	  bRangeLimit)			// TRUE iff an incoming value outside
									//	 the given range [uValidLo, uValidHi]
									//	 is adjusted to be the closer range limit

{
	LPVOID	 lpMemRht,				// Ptr to right arg global memory
			 lpMemIniRht,			// ...
			 lpMemRes;				// Ptr to result	...
	BOOL	 bRet = TRUE,			// TRUE iff result is valid
			 bScalar = FALSE;		// TRUE iff right arg is a scalar
	LPWCHAR  lpwErrMsg = ERRMSG_DOMAIN_ERROR APPEND_NAME;
	APLINT	 aplInteger,			// Right arg first integer
			 apaOffRht, 			// Right arg APA offset
			 apaMulRht; 			// Right arg APA multiplier
	APLSTYPE aplTypeRht;			// Right arg storage type
	APLNELM  aplNELMRht,			// Right arg NELM
			 aplNELMRes;			// Result	 ...
	APLRANK  aplRankRht;			// Right arg rank
	HGLOBAL  hGlbRht,				// Right arg global memory handle
			 hGlbRes;				// Result	 ...
	APLUINT  ByteRes,				// # bytes in the result
			 uRht;					// Loop counter
	UINT	 uBitMask;				// Bit mask for looping through Booleans

	// Split cases based upon the token type
	switch (lptkRhtArg->tkFlags.TknType)
	{
		case TKT_VARNAMED:
			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

			// Split cases based upon the symbol table immediate type
			if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
			{
				// Get the HGLOBAL
				hGlbRht = lptkRhtArg->tkData.tkSym->stData.stGlbData;

				break;		// Continue with HGLOBAL processing
			} // End IF

			// Handle the immediate case

			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

			// Split cases based upon the symbol table immediate type
			switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
			{
				case IMMTYPE_BOOL:
				case IMMTYPE_INT:
					aplInteger = lptkRhtArg->tkData.tkSym->stData.stInteger;

					// Test the value
					bRet = ValidateIntegerTest (&aplInteger,		// Ptr to the integer to test
												 uValidLo,			// Low range value (inclusive)
												 uValidHi,			// High ...
												 bRangeLimit);		// TRUE iff we're range limiting
					if (bRet)
					{
						bScalar = TRUE;

						goto MAKE_VECTOR;
					} else
						goto ERROR_EXIT;

					break;

				case IMMTYPE_FLOAT:
					// Attempt to convert the float to an integer using System CT
					aplInteger = FloatToAplint_SCT (lptkRhtArg->tkData.tkSym->stData.stFloat,
												   &bRet);
					if (bRet)
					{
						// Test the value
						bRet = ValidateIntegerTest (&aplInteger,	// Ptr to the integer to test
													 uValidLo,		// Low range value (inclusive)
													 uValidHi,		// High ...
													 bRangeLimit);	// TRUE iff we're range limiting
						if (bRet)
						{
							bScalar = TRUE;

							goto MAKE_VECTOR;
						} else
							goto ERROR_EXIT;
					} // End IF

					break;

				case IMMTYPE_CHAR:
					bRet = FALSE;

					break;

			} // End SWITCH

			break;

		case TKT_VARIMMED:
			// Split cases based upon the token immediate type
			switch (lptkRhtArg->tkFlags.ImmType)
			{
				case IMMTYPE_BOOL:
				case IMMTYPE_INT:
					aplInteger = lptkRhtArg->tkData.tkInteger;

					// Test the value
					bRet = ValidateIntegerTest (&aplInteger,		// Ptr to the integer to test
												 uValidLo,			// Low range value (inclusive)
												 uValidHi,			// High ...
												 bRangeLimit);		// TRUE iff we're range limiting
					if (bRet)
					{
						bScalar = TRUE;

						goto MAKE_VECTOR;
					} else
						goto ERROR_EXIT;

					break;

				case IMMTYPE_FLOAT:
					// Attempt to convert the float to an integer using System CT
					aplInteger = FloatToAplint_SCT (lptkRhtArg->tkData.tkFloat,
												   &bRet);
					if (bRet)
					{
						// Test the value
						bRet = ValidateIntegerTest (&aplInteger,	// Ptr to the integer to test
													 uValidLo,		// Low range value (inclusive)
													 uValidHi,		// High ...
													 bRangeLimit);	// TRUE iff we're range limiting
						if (bRet)
						{
							bScalar = TRUE;

							goto MAKE_VECTOR;
						} else
							goto ERROR_EXIT;
					} // End IF

					break;

				case IMMTYPE_CHAR:
					bRet = FALSE;

					break;
			} // End SWITCH

			goto NORMAL_EXIT;

		case TKT_LISTPAR:	// The tkData is an HGLOBAL of an array of HGLOBALs
			ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
									   lptkRhtArg);
			return FALSE;

		case TKT_VARARRAY:	// tkData is an HGLOBAL of an array of ???
			// Get the HGLOBAL
			hGlbRht = lptkRhtArg->tkData.tkGlbData;

			break;			// Continue with HGLOBAL processing

		defstop
			return FALSE;
	} // End SWITCH

	// st/tkData is a valid HGLOBAL variable array
	Assert (IsGlbTypeVarDir (hGlbRht));

	// Lock the memory to get a ptr to it
	lpMemIniRht = lpMemRht = MyGlobalLock (ClrPtrTypeDirAsGlb (hGlbRht));

#define lpHeader	((LPVARARRAY_HEADER) lpMemRht)
	// Get the Array Type, NELM, and Rank
	aplTypeRht = lpHeader->ArrType;
	aplNELMRht = lpHeader->NELM;
	aplRankRht = lpHeader->Rank;
#undef	lpHeader

	// Skip over the header and dimensions to the data
	lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

	// Ensure scalar or vector
	if (aplRankRht > 1)
	{
		lpwErrMsg = ERRMSG_RANK_ERROR APPEND_NAME;

		bRet = FALSE;
	} else
	// Split cases based upon the right arg storage type
	switch (aplTypeRht)
	{
		case ARRAY_BOOL:
			uBitMask = 0x01;

			// Test all the values
			for (uRht = 0; uRht < aplNELMRht; uRht++)
			{
				// Get the value
				aplInteger = (uBitMask & *(LPAPLBOOL) lpMemRht) ? 1 : 0;

				// Test the value
				bRet = ValidateIntegerTest (&aplInteger,			// Ptr to the integer to test
											 uValidLo,				// Low range value (inclusive)
											 uValidHi,				// High ...
											 bRangeLimit);			// TRUE iff we're range limiting
				if (!bRet)
					goto ERROR_EXIT;

				// Shift over the bit mask
				uBitMask <<= 1;

				// Check for end-of-byte
				if (uBitMask EQ END_OF_BYTE)
				{
					uBitMask = 0x01;			// Start over
					((LPAPLBOOL) lpMemRht)++;	// Skip to next byte
				} // End IF
			} // End FOR

			bScalar = (aplRankRht EQ 0);

			break;

		case ARRAY_INT:
			// Test all the values
			for (uRht = 0; uRht < aplNELMRht; uRht++)
			{
				// Get the value
				aplInteger = ((LPAPLINT) lpMemRht)[uRht];

				// Test the value
				bRet = ValidateIntegerTest (&aplInteger,			// Ptr to the integer to test
											 uValidLo,				// Low range value (inclusive)
											 uValidHi,				// High ...
											 bRangeLimit);			// TRUE iff we're range limiting
				if (!bRet)
					goto ERROR_EXIT;
			} // End FOR

			bScalar = (aplRankRht EQ 0);

			break;

		case ARRAY_FLOAT:
			// Test all the values
			for (uRht = 0; uRht < aplNELMRht; uRht++)
			{
				// Attempt to convert the float to an integer using System CT
				aplInteger = FloatToAplint_SCT (((LPAPLFLOAT) lpMemRht)[uRht],
											   &bRet);
				// Test the value
				bRet = ValidateIntegerTest (&aplInteger,			// Ptr to the integer to test
											 uValidLo,				// Low range value (inclusive)
											 uValidHi,				// High ...
											 bRangeLimit);			// TRUE iff we're range limiting
				if (!bRet)
					goto ERROR_EXIT;
			} // End FOR

			bScalar = (aplRankRht EQ 0);

			break;

		case ARRAY_APA:
#define lpAPA			((LPAPLAPA) lpMemRht)
			// Get the APA parameters
			apaOffRht = lpAPA->Off;
			apaMulRht = lpAPA->Mul;
#undef	lpAPA
			// Test all the values
			for (uRht = 0; uRht < aplNELMRht; uRht++)
			{
				// Get the value
				aplInteger = apaOffRht + apaMulRht * uRht;

				// Test the value
				bRet = ValidateIntegerTest (&aplInteger,			// Ptr to the integer to test
											 uValidLo,				// Low range value (inclusive)
											 uValidHi,				// High ...
											 bRangeLimit);			// TRUE iff we're range limiting
				if (!bRet)
					goto ERROR_EXIT;
			} // End FOR

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

	// Split cases based upon the storage type of the right arg
	switch (aplTypeRht)
	{
		case ARRAY_INT:
			// Copy the right arg global as the result
			hGlbRes = CopySymGlbDir (hGlbRht);

			break;

		case ARRAY_BOOL:
		case ARRAY_FLOAT:
		case ARRAY_APA:
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
	////////////lpHeader->PermNdx	 = PERMNDX_NONE;// Already zero from GHND
	////////////lpHeader->SysVar	 = 0;			// Already zero from GHND
				lpHeader->RefCnt	 = 1;
				lpHeader->NELM		 = aplNELMRht;
				lpHeader->Rank		 = 1;
	#undef	lpHeader

				// Save the dimension
				*VarArrayBaseToDim (lpMemRes) = aplNELMRht;

				// Skip over the header and dimensions to the data
				lpMemRes = VarArrayBaseToData (lpMemRes, 1);

				// Point back to the start of the right data
				lpMemRht = lpMemIniRht;

				// Loop through the right arg, converting to integers
				//	 and copying them to the result
				for (uRht = 0; uRht < aplNELMRht; uRht++)
				// Split cases based upon the storage type of the right arg
				switch (aplTypeRht)
				{
					case ARRAY_BOOL:
						if (uBitMask & *(LPAPLBOOL) lpMemRht)
							*((LPAPLINT) lpMemRes) = 1;

						// Skip over the result value
						((LPAPLINT) lpMemRes)++;

						// Shift over the bit mask
						uBitMask <<= 1;

						// Check for end-of-byte
						if (uBitMask EQ END_OF_BYTE)
						{
							uBitMask = 0x01;			// Start over
							((LPAPLBOOL) lpMemRht)++;	// Skip to next byte
						} // End IF

						break;

					case ARRAY_FLOAT:
						// Attempt to convert the float to an integer using System CT
						aplInteger = FloatToAplint_SCT (*((LPAPLFLOAT) lpMemRht)++,
													   &bRet);
						if (bRet)
							*((LPAPLINT) lpMemRes)++ = aplInteger;
						else
							break;

					case ARRAY_APA:
						*((LPAPLINT) lpMemRes)++ = apaOffRht + apaMulRht * uRht;

						break;

					defstop
						break;
				} // End FOR/SWITCH

				// We no longer need this ptr
				MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
			} else
				bRet = FALSE;
			break;

		defstop
			break;
	} // End SWITCH

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
////////lpHeader->PermNdx	 = PERMNDX_NONE;// Already zero from GHND
////////lpHeader->SysVar	 = 0;			// Already zero from GHND
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
		MyGlobalUnlock (ClrPtrTypeDirAsGlb (hGlbRht)); lpMemRht = NULL;
	} // End IF

	// If in error, set error message;
	//	 otherwise, save the value in the name
	if (!bRet)
		ErrorMessageIndirectToken (lpwErrMsg,
								   lptkRhtArg);
	else
	{
		// Free the old value
		FreeResultGlobalVar (ClrPtrTypeDirAsGlb (lptkNamArg->tkData.tkSym->stData.stGlbData));

		// Save as new value
		lptkNamArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbRes);
		lptkNamArg->tkFlags.NoDisplay = 1;
	} // End IF

	return bRet;
#undef	lpHeader
} // End ValidateIntegerVector_EM
#undef	APPEND_NAME


//***************************************************************************
//	$ValidSetALX_EM
//
//	Validate a value before assigning it to []ALX
//***************************************************************************

BOOL ValidSetALX_EM
	(LPTOKEN lptkNamArg,			// Ptr to name arg token
	 LPTOKEN lptkRhtArg)			// Ptr to right arg token

{
	// Ensure the argument is a character scalar (promoted to a vector)
	//	 or a character vector.
	return ValidateCharVector_EM (lptkNamArg, lptkRhtArg, FALSE);
} // End ValidSetALX_EM


//***************************************************************************
//	$ValidNdxChar
//
//	Validate a single value before assigning it to
//	  a position in a char-only SysVar.
//***************************************************************************

BOOL ValidNdxChar
	(APLINT 	  aplIntegerLst,			// The origin-0 index value (in case the position is important)
	 APLSTYPE	  aplTypeRht,				// Right arg storage type
	 LPAPLLONGEST lpaplLongestRht,			// Ptr to the right arg value
	 LPIMM_TYPES  lpimmTypeRht) 			// Ptr to right arg immediate type (may be NULL)

{
	// Split cases based upon the right arg storage type
	switch (aplTypeRht)
	{
		case ARRAY_BOOL:
		case ARRAY_INT:
		case ARRAY_FLOAT:
		case ARRAY_APA:
		case ARRAY_HETERO:
		case ARRAY_NESTED:
			return FALSE;

		case ARRAY_CHAR:
			return TRUE;

		defstop
			return FALSE;
	} // End SWITCH
} // End ValidNdxChar


//***************************************************************************
//	$ValidSetCT_EM
//
//	Validate a value before assigning it to []CT
//***************************************************************************

BOOL ValidSetCT_EM
	(LPTOKEN lptkNamArg,			// Ptr to name arg token
	 LPTOKEN lptkRhtArg)			// Ptr to right arg token

{
	// Ensure the argument is a real scalar or
	//	 one-element vector (demoted to a scalar)
	//	 between DEF_MIN_QUADCT and DEF_MAX_QUADCT inclusive.
	return ValidateFloat_EM (lptkNamArg,			// Ptr to name arg token
							 lptkRhtArg,			// Ptr to right arg token
							 DEF_MIN_QUADCT,		// Minimum value
							 DEF_MAX_QUADCT,		// Maximum ...
							 bRangeLimit.CT);		// TRUE iff range limiting
} // End ValidSetCT_EM


//***************************************************************************
//	$ValidNdxCT
//
//	Validate a single value before assigning it to a position in []CT.
//
//	We allow any number between DEF_MIN_QUADCT and DEF_MAX_QUADCT inclusive.
//***************************************************************************

BOOL ValidNdxCT
	(APLINT 	  aplIntegerLst,			// The origin-0 index value (in case the position is important)
	 APLSTYPE	  aplTypeRht,				// Right arg storage type
	 LPAPLLONGEST lpaplLongestRht,			// Ptr to the right arg value
	 LPIMM_TYPES  lpimmTypeRht) 			// Ptr to right arg immediate type (may be NULL)

{
	// Split cases based upon the right arg storage type
	switch (aplTypeRht)
	{
		case ARRAY_BOOL:
		case ARRAY_INT:
		case ARRAY_APA:
			// The right arg is integer -- convert to float
			*((LPAPLFLOAT) lpaplLongestRht) = (APLFLOAT) (APLINT) *lpaplLongestRht;

			// Set the new immediate type
			if (lpimmTypeRht)
				*lpimmTypeRht = IMMTYPE_FLOAT;
			break;

		case ARRAY_FLOAT:
			break;

		case ARRAY_CHAR:
		case ARRAY_HETERO:
		case ARRAY_NESTED:
			return FALSE;

		defstop
			break;
	} // End SWITCH

	// Test the value
	return ValidateFloatTest ((LPAPLFLOAT) lpaplLongestRht,
							  DEF_MIN_QUADCT,
							  DEF_MAX_QUADCT,
							  bRangeLimit.CT);
} // End ValidNdxCT


//***************************************************************************
//	$ValidSetELX_EM
//
//	Validate a value before assigning it to []ELX
//***************************************************************************

BOOL ValidSetELX_EM
	(LPTOKEN lptkNamArg,			// Ptr to name arg token
	 LPTOKEN lptkRhtArg)			// Ptr to right arg token

{
	// Ensure the argument is a character scalar (promoted to a vector)
	//	 or a character vector.
	return ValidateCharVector_EM (lptkNamArg, lptkRhtArg, FALSE);
} // End ValidSetELX_EM


//***************************************************************************
//	$ValidSetFC_EM
//
//	Validate a value before assigning it to []FC
//***************************************************************************

BOOL ValidSetFC_EM
	(LPTOKEN lptkNamArg,			// Ptr to name arg token
	 LPTOKEN lptkRhtArg)			// Ptr to right arg token

{
	APLSTYPE aplTypeRht;			// Right arg storage type
	APLNELM  aplNELMRht;			// ...		 NELM
	APLRANK  aplRankRht;			// ...		 rank

	// Get the attributes (Type, NELM, and Rank)
	//	 of the right arg
	AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

	// If the right arg is an empty simple vector, convert the result to
	//	 the default values
	if ((IsSimpleChar (aplTypeRht) || IsSimpleNum (aplTypeRht))
	 && aplNELMRht EQ 0
	 && aplRankRht EQ 1)
	{
		// Free the named arg
		FreeResultGlobalVar (ClrPtrTypeDirAsGlb (lptkNamArg->tkData.tkSym->stData.stGlbData));

		// Save as new value
		lptkNamArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbQuadFC_SYS);
		lptkNamArg->tkFlags.NoDisplay = 1;

		return TRUE;
	} // End IF

	// Ensure the argument is a character scalar (promoted to a vector)
	//	 or a character vector.
	return ValidateCharVector_EM (lptkNamArg, lptkRhtArg, FALSE);
} // End ValidSetFC_EM


//***************************************************************************
//	$ValidSetIC_EM
//
//	Validate a value before assigning it to []IC.
//
//	We allow any numeric singleton or vector whose values are
//	  DEF_MIN_QUADIC through DEF_MAX_QUADIC.
//***************************************************************************

BOOL ValidSetIC_EM
	(LPTOKEN lptkNamArg,			// Ptr to name arg token
	 LPTOKEN lptkRhtArg)			// Ptr to right arg token

{
	APLSTYPE aplTypeRht;			// Right arg storage type
	APLNELM  aplNELMRht;			// ...		 NELM
	APLRANK  aplRankRht;			// ...		 rank

	// Get the attributes (Type, NELM, and Rank)
	//	 of the right arg
	AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

	// If the right arg is an empty simple vector, convert the result to
	//	 the default values
	if ((IsSimpleChar (aplTypeRht) || IsSimpleNum (aplTypeRht))
	 && aplNELMRht EQ 0
	 && aplRankRht EQ 1)
	{
		// Free the named arg
		FreeResultGlobalVar (ClrPtrTypeDirAsGlb (lptkNamArg->tkData.tkSym->stData.stGlbData));

		// Save as new value
		lptkNamArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbQuadIC_SYS);
		lptkNamArg->tkFlags.NoDisplay = 1;

		return TRUE;
	} // End IF

	// Ensure the argument is an integer scalar (promoted to a vector)
	//	 or an integer vector
	return ValidateIntegerVector_EM (lptkNamArg,
									 lptkRhtArg,
									 DEF_MIN_QUADIC,
									 DEF_MAX_QUADIC,
									 bRangeLimit.IC);
} // End ValidSetIC_EM


//***************************************************************************
//	$ValidNdxIC
//
//	Validate a single value before assigning it to a position in []IC.
//
//	We allow any number between DEF_MIN_QUADIC and DEF_MAX_QUADIC inclusive.
//***************************************************************************

BOOL ValidNdxIC
	(APLINT 	  aplIntegerLst,			// The origin-0 index value (in case the position is important)
	 APLSTYPE	  aplTypeRht,				// Right arg storage type
	 LPAPLLONGEST lpaplLongestRht,			// Ptr to the right arg value
	 LPIMM_TYPES  lpimmTypeRht) 			// Ptr to right arg immediate type (may be NULL)

{
	BOOL bRet;								// TRUE iff the result is valid

	// Split cases based upon the right arg storage type
	switch (aplTypeRht)
	{
		case ARRAY_BOOL:
		case ARRAY_INT:
		case ARRAY_APA:
			break;

		case ARRAY_FLOAT:
			// The right arg is float -- convert to integer
			*lpaplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) lpaplLongestRht, &bRet);
			if ((!bRangeLimit.IC) && !bRet)
				return bRet;
			// Set the new immediate type
			if (lpimmTypeRht)
				*lpimmTypeRht = IMMTYPE_INT;
			break;

		case ARRAY_CHAR:
		case ARRAY_HETERO:
		case ARRAY_NESTED:
			return FALSE;

		defstop
			break;
	} // End SWITCH

	// Test the value
	return ValidateIntegerTest ((LPAPLINT) lpaplLongestRht, 		// Ptr to the integer to test
								DEF_MIN_QUADIC, 					// Low range value (inclusive)
								DEF_MAX_QUADIC, 					// High ...
								bRangeLimit.IC);					// TRUE iff we're range limiting
} // End ValidNdxIC


//***************************************************************************
//	$ValidSetIO_EM
//
//	Validate a value before assigning it to []IO
//
//	We allow any numeric singleton whose value is 0 or 1.
//***************************************************************************

BOOL ValidSetIO_EM
	(LPTOKEN lptkNamArg,				// Ptr to name arg token
	 LPTOKEN lptkRhtArg)				// Ptr to right arg token

{
	// Ensure the argument is a Boolean scalar or
	//	 one-element vector (demoted to a scalar).
	return ValidateInteger_EM (lptkNamArg,			// Ptr to name arg token
							   lptkRhtArg,			// Ptr to right arg token
							   DEF_MIN_QUADIO,		// Minimum value
							   DEF_MAX_QUADIO,		// Maximum ...
							   bRangeLimit.IO); 	// TRUE iff range limiting
} // End ValidSetIO_EM


//***************************************************************************
//	$ValidNdxIO
//
//	Validate a single value before assigning it to a position in []IO.
//
//	We allow any number between DEF_MIN_QUADIO and DEF_MAX_QUADIO inclusive.
//***************************************************************************

BOOL ValidNdxIO
	(APLINT 	  aplIntegerLst,			// The origin-0 index value (in case the position is important)
	 APLSTYPE	  aplTypeRht,				// Right arg storage type
	 LPAPLLONGEST lpaplLongestRht,			// Ptr to the right arg value
	 LPIMM_TYPES  lpimmTypeRht) 			// Ptr to right arg immediate type (may be NULL)

{
	BOOL bRet;								// TRUE iff the result is valid

	// Split cases based upon the right arg storage type
	switch (aplTypeRht)
	{
		case ARRAY_FLOAT:
			// The right arg is float -- convert to integer
			*lpaplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) lpaplLongestRht, &bRet);
			if ((!bRangeLimit.IO) && !bRet)
				return bRet;

			// Fall through to set the new immediate type

		case ARRAY_BOOL:
		case ARRAY_INT:
		case ARRAY_APA:
			// Set the new immediate type (in cae it was integer/APA)
			if (lpimmTypeRht)
				*lpimmTypeRht = IMMTYPE_BOOL;
			break;

		case ARRAY_CHAR:
		case ARRAY_HETERO:
		case ARRAY_NESTED:
			return FALSE;

		defstop
			break;
	} // End SWITCH

	// Test the value
	return ValidateIntegerTest ((LPAPLINT) lpaplLongestRht, 		// Ptr to the integer to test
								DEF_MIN_QUADIO, 					// Low range value (inclusive)
								DEF_MAX_QUADIO, 					// High ...
								bRangeLimit.IO);					// TRUE iff we're range limiting
} // End ValidNdxIO


//***************************************************************************
//	$ValidSetLX_EM
//
//	Validate a value before assigning it to []LX
//***************************************************************************

BOOL ValidSetLX_EM
	(LPTOKEN lptkNamArg,				// Ptr to name arg token
	 LPTOKEN lptkRhtArg)				// Ptr to right arg token

{
	// Ensure the argument is a character scalar (promoted to a vector)
	//	 or a character vector.
	return ValidateCharVector_EM (lptkNamArg, lptkRhtArg, FALSE);
} // End ValidSetLX_EM


//***************************************************************************
//	$ValidSetPP_EM
//
//	Validate a value before assigning it to []PP
//***************************************************************************

BOOL ValidSetPP_EM
	(LPTOKEN lptkNamArg,			// Ptr to name arg token
	 LPTOKEN lptkRhtArg)			// Ptr to right arg token

{
	// Ensure the argument is an integer scalar or
	//	 one-element vector (demoted to a scalar)
	//	 in the range for []PP.
	return ValidateInteger_EM (lptkNamArg,			// Ptr to name arg token
							   lptkRhtArg,			// Ptr to right arg token
							   DEF_MIN_QUADPP,		// Minimum value
							   DEF_MAX_QUADPP,		// Maximum ...
							   bRangeLimit.PP); 	// TRUE iff range limiting
} // End ValidSetPP_EM


//***************************************************************************
//	$ValidNdxPP
//
//	Validate a single value before assigning it to a position in []PP.
//
//	We allow any number between DEF_MIN_QUADPP and DEF_MAX_QUADPP inclusive.
//***************************************************************************

BOOL ValidNdxPP
	(APLINT 	  aplIntegerLst,			// The origin-0 index value (in case the position is important)
	 APLSTYPE	  aplTypeRht,				// Right arg storage type
	 LPAPLLONGEST lpaplLongestRht,			// Ptr to the right arg value
	 LPIMM_TYPES  lpimmTypeRht) 			// Ptr to right arg immediate type (may be NULL)

{
	BOOL bRet;								// TRUE iff the result is valid

	// Split cases based upon the right arg storage type
	switch (aplTypeRht)
	{
		case ARRAY_BOOL:
		case ARRAY_INT:
		case ARRAY_APA:
			break;

		case ARRAY_FLOAT:
			// The right arg is float -- convert to integer
			*lpaplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) lpaplLongestRht, &bRet);
			if ((!bRangeLimit.PP) && !bRet)
				return bRet;
			// Set the new immediate type
			if (lpimmTypeRht)
				*lpimmTypeRht = IMMTYPE_INT;
			break;

		case ARRAY_CHAR:
		case ARRAY_HETERO:
		case ARRAY_NESTED:
			return FALSE;

		defstop
			break;
	} // End SWITCH

	// Test the value
	return ValidateIntegerTest ((LPAPLINT) lpaplLongestRht, 		// Ptr to the integer to test
								DEF_MIN_QUADPP, 					// Low range value (inclusive)
								DEF_MAX_QUADPP, 					// High ...
								bRangeLimit.PP);					// TRUE iff we're range limiting
} // End ValidNdxPP


//***************************************************************************
//	$ValidSetPR_EM
//
//	Validate a value before assigning it to []PR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ValidSetPR_EM"
#else
#define APPEND_NAME
#endif

BOOL ValidSetPR_EM
	(LPTOKEN lptkNamArg,			// Ptr to name arg token
	 LPTOKEN lptkRhtArg)			// Ptr to right arg token

{
	HGLOBAL 	 hGlbRht;		// Right arg global memory handle
	LPVOID		 lpMemRht;		// Ptr to right arg global memory
	BOOL		 bRet = TRUE;	// TRUE iff result is valid
	LPWCHAR 	 lpwErrMsg = ERRMSG_DOMAIN_ERROR APPEND_NAME;
	HGLOBAL 	 hGlbPTD;		// PerTabData global memory handle
	LPPERTABDATA lpMemPTD;		// Ptr to PerTabData global memory

	// Get the thread's PerTabData global memory handle
	hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

	// Lock the memory to get a ptr to it
	lpMemPTD = MyGlobalLock (hGlbPTD);

	// Ensure the argument is a character scalar, or
	//	 one-element vector (demoted to a scalar), or
	//	 an empty vector.

	// Split cases based upon the token type
	switch (lptkRhtArg->tkFlags.TknType)
	{
		case TKT_VARNAMED:
			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

			if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
			{
				// Get the HGLOBAL
				hGlbRht = lptkRhtArg->tkData.tkSym->stData.stGlbData;

				break;		// Continue with HGLOBAL processing
			} // End IF

			// Handle the immediate case

			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

			// Split cases based upon the token immediate type
			switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
			{
				case IMMTYPE_BOOL:
				case IMMTYPE_INT:
				case IMMTYPE_FLOAT:
					bRet = FALSE;

					break;

				case IMMTYPE_CHAR:
					lpMemPTD->cQuadPR = lptkRhtArg->tkData.tkSym->stData.stChar;

					goto MAKE_SCALAR;
			} // End SWITCH

			break;

		case TKT_VARIMMED:
			// Split cases based upon the token immediate type
			switch (lptkRhtArg->tkFlags.ImmType)
			{
				case IMMTYPE_BOOL:
				case IMMTYPE_INT:
				case IMMTYPE_FLOAT:
					bRet = FALSE;

					break;

				case IMMTYPE_CHAR:
					lpMemPTD->cQuadPR = lptkRhtArg->tkData.tkChar;

					goto MAKE_SCALAR;
			} // End SWITCH

			break;

		case TKT_LISTPAR:	// The tkData is an HGLOBAL of an array of HGLOBALs
			ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
									   lptkNamArg);
			return FALSE;

		case TKT_STRING:	// tkData is an HGLOBAL of an array of ???
		case TKT_VARARRAY:	// tkData is an HGLOBAL of an array of ???
			// Get the HGLOBAL
			hGlbRht = lptkRhtArg->tkData.tkGlbData;

			break;			// Continue with HGLOBAL processing

		defstop
			return FALSE;
	} // End SWITCH

	// tkData is a valid HGLOBAL variable array
	Assert (IsGlbTypeVarDir (hGlbRht));

	// Lock the memory to get a ptr to it
	lpMemRht = MyGlobalLock (ClrPtrTypeDirAsGlb (hGlbRht));

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
	MyGlobalUnlock (ClrPtrTypeDirAsGlb (hGlbRht)); lpMemRht = NULL;

#undef	lpHeader

MAKE_SCALAR:
	// If in error, set error message;
	//	 otherwise, save the value in the name
	if (!bRet)
		ErrorMessageIndirectToken (lpwErrMsg,
								   lptkRhtArg);
	else
	{
		lptkNamArg->tkData.tkSym->stFlags.Imm = (lpMemPTD->cQuadPR NE 0);
		if (lpMemPTD->cQuadPR EQ 0)
			lptkNamArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbV0Char);
		else
			lptkNamArg->tkData.tkSym->stData.stChar = lpMemPTD->cQuadPR;
		lptkNamArg->tkFlags.NoDisplay = 1;
	} // End IF

	// We no longer need this ptr
	MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

	return bRet;
} // End ValidSetPR_EM
#undef	APPEND_NAME


//***************************************************************************
//	$ValidSetPW_EM
//
//	Validate a value before assigning it to []PW
//***************************************************************************

BOOL ValidSetPW_EM
	(LPTOKEN lptkNamArg,			// Ptr to name arg token
	 LPTOKEN lptkRhtArg)			// Ptr to right arg token

{
	// Ensure the argument is an integer scalar or
	//	 one-element vector (demoted to a scalar)
	//	 in the range for []PW.
	return ValidateInteger_EM (lptkNamArg,			// Ptr to name arg token
							   lptkRhtArg,			// Ptr to right arg token
							   DEF_MIN_QUADPW,		// Minimum value
							   DEF_MAX_QUADPW,		// Maximum ...
							   bRangeLimit.PW); 	// TRUE iff range limiting
} // End ValidSetPW_EM


//***************************************************************************
//	$ValidNdxPW
//
//	Validate a single value before assigning it to a position in []PW.
//
//	We allow any number between DEF_MIN_QUADPW and DEF_MAX_QUADPW inclusive.
//***************************************************************************

BOOL ValidNdxPW
	(APLINT 	  aplIntegerLst,			// The origin-0 index value (in case the position is important)
	 APLSTYPE	  aplTypeRht,				// Right arg storage type
	 LPAPLLONGEST lpaplLongestRht,			// Ptr to the right arg value
	 LPIMM_TYPES  lpimmTypeRht) 			// Ptr to right arg immediate type (may be NULL)

{
	BOOL bRet;								// TRUE iff the result is valid

	// Split cases based upon the right arg storage type
	switch (aplTypeRht)
	{
		case ARRAY_BOOL:
		case ARRAY_INT:
		case ARRAY_APA:
			break;

		case ARRAY_FLOAT:
			// The right arg is float -- convert to integer
			*lpaplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) lpaplLongestRht, &bRet);
			if ((!bRangeLimit.PW) && !bRet)
				return bRet;
			// Set the new immediate type
			if (lpimmTypeRht)
				*lpimmTypeRht = IMMTYPE_INT;
			break;

		case ARRAY_CHAR:
		case ARRAY_HETERO:
		case ARRAY_NESTED:
			return FALSE;

		defstop
			break;
	} // End SWITCH

	// Test the value
	return ValidateIntegerTest ((LPAPLINT) lpaplLongestRht, 		// Ptr to the integer to test
								DEF_MIN_QUADPW, 					// Low range value (inclusive)
								DEF_MAX_QUADPW, 					// High ...
								bRangeLimit.PW);					// TRUE iff we're range limiting
} // End ValidNdxPW


//***************************************************************************
//	$ValidSetRL_EM
//
//	Validate a value before assigning it to []RL
//***************************************************************************

BOOL ValidSetRL_EM
	(LPTOKEN lptkNamArg,			// Ptr to name arg token
	 LPTOKEN lptkRhtArg)			// Ptr to right arg token

{
	// Ensure the argument is an integer scalar or
	//	 one-element vector (demoted to a scalar)
	//	 in the range for []RL.
	return ValidateInteger_EM (lptkNamArg,			// Ptr to name arg token
							   lptkRhtArg,			// Ptr to right arg token
							   DEF_MIN_QUADRL,		// Minimum value
							   DEF_MAX_QUADRL,		// Maximum ...
							   bRangeLimit.RL); 	// TRUE iff range limiting
} // End ValidSetRL_EM


//***************************************************************************
//	$ValidNdxRL
//
//	Validate a single value before assigning it to a position in []RL.
//
//	We allow any number between DEF_MIN_QUADRL and DEF_MAX_QUADRL inclusive.
//***************************************************************************

BOOL ValidNdxRL
	(APLINT 	  aplIntegerLst,			// The origin-0 index value (in case the position is important)
	 APLSTYPE	  aplTypeRht,				// Right arg storage type
	 LPAPLLONGEST lpaplLongestRht,			// Ptr to the right arg value
	 LPIMM_TYPES  lpimmTypeRht) 			// Ptr to right arg immediate type (may be NULL)

{
	BOOL bRet;								// TRUE iff the result is valid

	// Split cases based upon the right arg storage type
	switch (aplTypeRht)
	{
		case ARRAY_BOOL:
		case ARRAY_INT:
		case ARRAY_APA:
			break;

		case ARRAY_FLOAT:
			// The right arg is float -- convert to integer
			*lpaplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) lpaplLongestRht, &bRet);
			if ((!bRangeLimit.RL) && !bRet)
				return bRet;
			// Set the new immediate type
			if (lpimmTypeRht)
				*lpimmTypeRht = IMMTYPE_INT;
			break;

		case ARRAY_CHAR:
		case ARRAY_HETERO:
		case ARRAY_NESTED:
			return FALSE;

		defstop
			break;
	} // End SWITCH

	// Test the value
	return ValidateIntegerTest ((LPAPLINT) lpaplLongestRht, 		// Ptr to the integer to test
								DEF_MIN_QUADRL, 					// Low range value (inclusive)
								DEF_MAX_QUADRL, 					// High ...
								bRangeLimit.RL);					// TRUE iff we're range limiting
} // End ValidNdxRL


//***************************************************************************
//	$ValidSetSA_EM
//
//	Validate a value before assigning it to []SA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ValidSetSA_EM"
#else
#define APPEND_NAME
#endif

BOOL ValidSetSA_EM
	(LPTOKEN lptkNamArg,			// Ptr to name arg token
	 LPTOKEN lptkRhtArg)			// Ptr to right arg token

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
	hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

	// Lock the memory to get a ptr to it
	lpMemPTD = MyGlobalLock (hGlbPTD);

	// Ensure the argument is a character scalar (promoted to a vector)
	//	 or vector, and a valid Stop Action value
	//	 ('', 'EXIT', 'ERROR', 'CLEAR', 'OFF') uppercase only

	// Split cases based upon the token type
	switch (lptkRhtArg->tkFlags.TknType)
	{
		case TKT_VARNAMED:
			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

			if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
			{
				// Get the HGLOBAL
				hGlbRht = lptkRhtArg->tkData.tkSym->stData.stGlbData;

				break;		// Continue with HGLOBAL processing
			} // End IF

			// Handle the immediate case

			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

			// Fall through to common error code

		case TKT_VARIMMED:
			ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
									   lptkNamArg);
			return FALSE;

		case TKT_LISTPAR:	// The tkData is an HGLOBAL of an array of HGLOBALs
			ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
									   lptkNamArg);
			return FALSE;

		case TKT_STRING:	// tkData is an HGLOBAL of an array of ???
		case TKT_VARARRAY:	// tkData is an HGLOBAL of an array of ???
			// Get the HGLOBAL
			hGlbRht = lptkRhtArg->tkData.tkGlbData;

			break;			// Continue with HGLOBAL processing

		defstop
			return FALSE;
	} // End SWITCH

	// tkData is a valid HGLOBAL variable array
	Assert (IsGlbTypeVarDir (hGlbRht));

	// Lock the memory to get a ptr to it
	lpMemRht = MyGlobalLock (ClrPtrTypeDirAsGlb (hGlbRht));

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
	MyGlobalUnlock (ClrPtrTypeDirAsGlb (hGlbRht)); lpMemRht = NULL;

	// If in error, set error message;
	//	 otherwise, save the value in the name
	if (!bRet)
		ErrorMessageIndirectToken (lpwErrMsg,
								   lptkRhtArg);
	else
	{
		// Free the old value
		FreeResultGlobalVar (ClrPtrTypeDirAsGlb (lptkNamArg->tkData.tkSym->stData.stGlbData));

		// Save as new value
		lptkNamArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbRes);
		lptkNamArg->tkFlags.NoDisplay = 1;
	} // End IF

	// We no longer need this ptr
	MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

	return bRet;
} // End ValidSetSA_EM
#undef	APPEND_NAME


//***************************************************************************
//	$ValidSetWSID_EM
//
//	Validate a value before assigning it to []WSID
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- ValidSetWSID_EM"
#else
#define APPEND_NAME
#endif

BOOL ValidSetWSID_EM
	(LPTOKEN lptkNamArg,			// Ptr to name arg token
	 LPTOKEN lptkRhtArg)			// Ptr to right arg token

{
	// Ensure the argument is a character scalar (promoted to a vector)
	//	 or a character vector.
	return ValidateCharVector_EM (lptkNamArg, lptkRhtArg, TRUE);
} // End ValidSetWSID_EM
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
	hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

	// Lock the memory to get a ptr to it
	lpMemPTD = MyGlobalLock (hGlbPTD);

	// Set the array set validation routines
	aSysVarValidSet[SYSVAR_ALX ] = ValidSetALX_EM ;
	aSysVarValidSet[SYSVAR_CT  ] = ValidSetCT_EM  ;
	aSysVarValidSet[SYSVAR_ELX ] = ValidSetELX_EM ;
	aSysVarValidSet[SYSVAR_FC  ] = ValidSetFC_EM  ;
	aSysVarValidSet[SYSVAR_IC  ] = ValidSetIC_EM  ;
	aSysVarValidSet[SYSVAR_IO  ] = ValidSetIO_EM  ;
	aSysVarValidSet[SYSVAR_LX  ] = ValidSetLX_EM  ;
	aSysVarValidSet[SYSVAR_PP  ] = ValidSetPP_EM  ;
	aSysVarValidSet[SYSVAR_PR  ] = ValidSetPR_EM  ;
	aSysVarValidSet[SYSVAR_PW  ] = ValidSetPW_EM  ;
	aSysVarValidSet[SYSVAR_RL  ] = ValidSetRL_EM  ;
	aSysVarValidSet[SYSVAR_SA  ] = ValidSetSA_EM  ;
	aSysVarValidSet[SYSVAR_WSID] = ValidSetWSID_EM;

	// Set the array index validation routine
	aSysVarValidNdx[SYSVAR_ALX ] = ValidNdxChar   ;
	aSysVarValidNdx[SYSVAR_CT  ] = ValidNdxCT	  ;
	aSysVarValidNdx[SYSVAR_ELX ] = ValidNdxChar   ;
	aSysVarValidNdx[SYSVAR_FC  ] = ValidNdxChar   ;
	aSysVarValidNdx[SYSVAR_IC  ] = ValidNdxIC	  ;
	aSysVarValidNdx[SYSVAR_IO  ] = ValidNdxIO	  ;
	aSysVarValidNdx[SYSVAR_LX  ] = ValidNdxChar   ;
	aSysVarValidNdx[SYSVAR_PP  ] = ValidNdxPP	  ;
	aSysVarValidNdx[SYSVAR_PR  ] = ValidNdxChar   ;
	aSysVarValidNdx[SYSVAR_PW  ] = ValidNdxPW	  ;
	aSysVarValidNdx[SYSVAR_RL  ] = ValidNdxRL	  ;
	aSysVarValidNdx[SYSVAR_SA  ] = ValidNdxChar   ;
	aSysVarValidNdx[SYSVAR_WSID] = ValidNdxChar   ;

	// Assign default values to the system vars
	if (!AssignGlobalCWS	 (hGlbQuadALX_CWS	, SYSVAR_ALX , lpMemPTD->lpSymQuadALX	   )) return FALSE;   // Attention Latent Expression
	if (!AssignRealScalarCWS (fQuadCT_CWS		, SYSVAR_CT  , lpMemPTD->lpSymQuadCT	   )) return FALSE;   // Comparison Tolerance
	if (!AssignGlobalCWS	 (hGlbQuadELX_CWS	, SYSVAR_ELX , lpMemPTD->lpSymQuadELX	   )) return FALSE;   // Error Latent Expression
	if (!AssignGlobalCWS	 (hGlbQuadFC_CWS	, SYSVAR_FC  , lpMemPTD->lpSymQuadFC	   )) return FALSE;   // Format Control
	if (!AssignGlobalCWS	 (hGlbQuadIC_CWS	, SYSVAR_IC  , lpMemPTD->lpSymQuadIC	   )) return FALSE;   // Indeterminate Control
	if (!AssignBoolScalarCWS (bQuadIO_CWS		, SYSVAR_IO  , lpMemPTD->lpSymQuadIO	   )) return FALSE;   // Index Origin
	if (!AssignGlobalCWS	 (hGlbQuadLX_CWS	, SYSVAR_LX  , lpMemPTD->lpSymQuadLX	   )) return FALSE;   // Latent Expression
	if (!AssignIntScalarCWS  (uQuadPP_CWS		, SYSVAR_PP  , lpMemPTD->lpSymQuadPP	   )) return FALSE;   // Print Precision
	if (cQuadPR_CWS EQ 0)
	{
		if (!AssignGlobalCWS (hGlbQuadPR_CWS	, SYSVAR_PR  , lpMemPTD->lpSymQuadPR	   )) return FALSE;   // Prompt Replacement
	} else
	{
		if (!AssignCharScalarCWS (cQuadPR_CWS	, SYSVAR_PR  , lpMemPTD->lpSymQuadPR	   )) return FALSE;   // Prompt Replacement
	} // End IF
	if (!AssignIntScalarCWS  (uQuadPW_CWS		, SYSVAR_PW  , lpMemPTD->lpSymQuadPW	   )) return FALSE;   // Print Width
	if (!AssignIntScalarCWS  (uQuadRL_CWS		, SYSVAR_RL  , lpMemPTD->lpSymQuadRL	   )) return FALSE;   // Random Link
	if (!AssignGlobalCWS	 (hGlbQuadSA_CWS	, SYSVAR_SA  , lpMemPTD->lpSymQuadSA	   )) return FALSE;   // Stop Action
	if (!AssignGlobalCWS	 (hGlbQuadWSID_CWS	, SYSVAR_WSID, lpMemPTD->lpSymQuadWSID	   )) return FALSE;   // Workspace Identifier

	// Save the index value
	lpMemPTD->bQuadxSA = bQuadxSA_CWS;

	// We no longer need this ptr
	MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

	return TRUE;
} // End InitSystemVars


//***************************************************************************
//	End of File: sysvars.c
//***************************************************************************
