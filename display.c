//***************************************************************************
//	NARS2000 -- Display Routines
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>
#include <float.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "termcode.h"
#include "externs.h"
#include "display.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


#ifdef DEBUG
// Resource debugging variables
extern int iCountGLOBAL[MAXOBJ];
extern HANDLE ahGLOBAL[MAXOBJ];
extern UINT auLinNumGLOBAL[MAXOBJ];
#endif

//***************************************************************************
//	ArrayDisplay_EM
//
//	Display an array
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME 	L" -- AssignDisplay_EM"
#else
#define APPEND_NAME
#endif

BOOL ArrayDisplay_EM
	(LPTOKEN lptkRes)

{
	LPAPLCHAR lpaplChar;

	// Split cases based upon the token type
	switch (lptkRes->tkFlags.TknType)
	{
		case TKT_VARNAMED:
			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lptkRes->tkData.lpVoid) EQ PTRTYPE_STCONST);

			// If it's not immediate, it's an array
			if (!lptkRes->tkData.lpSym->stFlags.Imm)
			{
				// stData is a valid HGLOBAL variable array
				Assert (IsGlbTypeVarDir (lptkRes->tkData.lpSym->stData.lpVoid));

				// Check for NoDisplay flag
				if (lptkRes->tkFlags.NoDisplay)
					return TRUE;
				DisplayGlbArr (lpwszFormat,
							   ClrPtrTypeDirGlb (lptkRes->tkData.lpSym->stData.stGlbData));
				return TRUE;
////			lpaplChar =
////			FormatGlobal (lpwszTemp,
////						  ClrPtrTypeDirGlb (lptkRes->tkData.lpSym->stData.stGlbData));
				break;
			} // End IF

			// Check for NoDisplay flag
			if (lptkRes->tkFlags.NoDisplay)
				return TRUE;

			// Handle the immediate case

			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lptkRes->tkData.lpVoid) EQ PTRTYPE_STCONST);

			// stData is immediate
			Assert (lptkRes->tkData.lpSym->stFlags.Imm);

			lpaplChar =
			FormatSymTabConst (lpwszTemp,
							   lptkRes->tkData.lpSym);
			break;

		case TKT_VARIMMED:	// The tkData is an immediate constant
			// Check for NoDisplay flag
			if (lptkRes->tkFlags.NoDisplay)
				return TRUE;

			lpaplChar =
			FormatImmed (lpwszTemp,
						 lptkRes->tkFlags.ImmType,
						 &lptkRes->tkData.lpVoid);
			break;

		case TKT_LISTPAR:	// The tkData is an HGLOBAL of an array of LPSYMENTRYs/HGLOBALs
			ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
									   lptkRes);
			return FALSE;

		case TKT_VARARRAY:	// The tkData is an HGLOBAL of an array of LPSYMENTRYs/HGLOBALs
			// Check for NoDisplay flag
			if (lptkRes->tkFlags.NoDisplay)
				return TRUE;

			switch (GetPtrTypeDir (lptkRes->tkData.lpVoid))
			{
				case PTRTYPE_STCONST:
					lpaplChar =
					FormatSymTabConst (lpwszTemp,
									   lptkRes->tkData.lpSym);
					break;

				case PTRTYPE_HGLOBAL:
					DisplayGlbArr (lpwszFormat,
								   ClrPtrTypeDirGlb (lptkRes->tkData.tkGlbData));
					return TRUE;
////				lpaplChar =
////				FormatGlobal (lpwszTemp,
////							  ClrPtrTypeDirGlb (lptkRes->tkData.tkGlbData));
					break;

				defstop
					return FALSE;
			} // End SWITCH

			break;

////////case TKT_STRAND:	// The tkData is an HGLOBAL of a single HGLOBAL
		defstop
			return FALSE;
	} // End SWITCH

	// Delete the last blank in case it matters,
	//	 and ensure properly terminated
	if (lpaplChar[-1] EQ L' ')
		*--lpaplChar = L'\0';
	else
		*lpaplChar = L'\0';

	// Display the line
	AppendLine (lpwszTemp, FALSE);

	return TRUE;
} // End ArrayDisplay_EM
#undef	APPEND_NAME


//***************************************************************************
//	DisplayGlbArr
//
//	Display a global array
//***************************************************************************

void DisplayGlbArr
	(LPAPLCHAR lpaplChar,
	 HGLOBAL   hGlb)

{
	LPVOID		lpMem;
	LPAPLCHAR	lpaplCharStart,
				lpwsz;
	APLSTYPE	aplType;
	APLNELM 	aplNELM;
	APLRANK 	aplRank;
	LPAPLDIM	lpMemDim;
	APLDIM		aplDimNCols,
				aplDimNRows,
				aplDimCol,
				aplLastDim;
	APLNELM 	aplNELMRes;
	LPFMTHEADER lpFmtHeader;
	LPFMTCOLSTR lpFmtColStr;
	UINT		uCol;

#ifdef DEBUG
	// Fill lpwszFormat with FFs so we can tell what we actually wrote
	FillMemory (lpaplChar, 1024, 0xFF);
#endif

	// Lock the memory to get a ptr to it
	lpMem = MyGlobalLock (hGlb);

#define lpHeader	((LPVARARRAY_HEADER) lpMem)

	// Get the header values
	aplType = lpHeader->ArrType;
	aplNELM = lpHeader->NELM;
	aplRank = lpHeader->Rank;

#undef	lpHeader

	// Skip over the header to the dimensions
	lpMemDim = VarArrayBaseToDim (lpMem);

	// Skip over the header and dimension to the data
	lpMem = VarArrayBaseToData (lpMem, aplRank);

	// Get the # columns
	if (aplRank EQ 0)
		aplDimNCols = aplDimNRows = 1;
	else
	{
		aplDimNCols = lpMemDim[aplRank - 1];

		// If there are no columns, ignore this
		if (aplDimNCols EQ 0)
			goto NORMAL_EXIT;

		// Get the # rows (across all planes)
		if (aplRank EQ 1)
			aplDimNRows = 1;
		else
			aplDimNRows = aplNELM / aplDimNCols;
	} // End IF/ELSE

	// Define a new FMTHEAD in the output
	ZeroMemory ((LPFMTHEADER) lpaplChar, sizeof (FMTHEADER));
////((LPFMTHEADER) lpaplChar)->lpFmtHeadUp = NULL;	// Filled in by ZeroMemory
	lpFmtHeader = (LPFMTHEADER) lpaplChar;
#ifdef DEBUG
	lpFmtHeader->Signature	 = FMTHEADER_SIGNATURE;
#endif
////lpFmtHeader->lpFmtRowUp  = NULL;				// Filled in by ZeroMemory
////lpFmtHeader->lpFmtColUp  = NULL;				// ...
////lpFmtHeader->lpFmtRow1st =						// Filled in below
////lpFmtHeader->lpFmtCol1st =						// ...
	lpFmtHeader->uRows		 = (UINT) aplDimNRows;
	lpFmtHeader->uCols		 = (UINT) aplDimNCols;
////lpFmtHeader->uFmtRows	 = 0;					// Filled in by ZeroMemory
////lpFmtHeader->uFmtInts	 = 0;					// ...
////lpFmtHeader->uFmtFrcs	 = 0;					// ...
////lpFmtHeader->uFmtTrBl	 = 0;					// ...
////lpFmtHeader->uDepth 	 = 0;					// ...

	// Define <aplDimNCols> FMTCOLSTRs in the output
	lpFmtColStr = (LPFMTCOLSTR) (&lpFmtHeader[1]);
	lpFmtHeader->lpFmtCol1st = lpFmtColStr;
	ZeroMemory (lpFmtColStr, (UINT) aplDimNCols * sizeof (FMTCOLSTR));
#ifdef DEBUG
	{
		APLDIM uCol;

		for (uCol = 0; uCol < aplDimNCols; uCol++)
			lpFmtColStr[uCol].Signature = FMTCOLSTR_SIGNATURE;
	}
#endif
	// Skip over the FMTCOLSTRs
	lpaplChar = lpaplCharStart = (LPAPLCHAR) (&lpFmtColStr[aplDimNCols]);

	// Save ptr to 1st child FMTROWSTR
	if (aplDimNRows)
		lpFmtHeader->lpFmtRow1st = (LPFMTROWSTR) lpaplChar;
	else
		lpFmtHeader->lpFmtRow1st = NULL;

	// Loop through the array appending the formatted values (separated by L'\0')
	//	 to the output vector, and accumulating the values in the appropriate
	//	 FMTCOLSTR & FMTROWSTR entries.

	// Split cases based upon the array's storage type
	switch (aplType)
	{
		case ARRAY_BOOL:
////////////lpaplChar =
			CompileArrBool	  ((LPAPLBOOL)	  lpMem,
							   lpFmtHeader,
							   lpFmtColStr,
							   lpaplChar,
							   aplDimNRows,
							   aplDimNCols,
							   aplRank,
							   lpMemDim);
			break;

		case ARRAY_INT:
////////////lpaplChar =
			CompileArrInteger ((LPAPLINT)	 lpMem,
							   lpFmtHeader,
							   lpFmtColStr,
							   lpaplChar,
							   aplDimNRows,
							   aplDimNCols,
							   aplRank,
							   lpMemDim);
			break;

		case ARRAY_FLOAT:
////////////lpaplChar =
			CompileArrFloat   ((LPAPLFLOAT)  lpMem,
							   lpFmtHeader,
							   lpFmtColStr,
							   lpaplChar,
							   aplDimNRows,
							   aplDimNCols,
							   aplRank,
							   lpMemDim);
			break;

		case ARRAY_APA:
////////////lpaplChar =
			CompileArrAPA	  ((LPAPLAPA)	 lpMem,
							   lpFmtHeader,
							   lpFmtColStr,
							   lpaplChar,
							   aplDimNRows,
							   aplDimNCols,
							   aplRank,
							   lpMemDim);
			break;

		case ARRAY_CHAR:
////////////lpaplChar =
			CompileArrChar	  ((LPAPLCHAR)	 lpMem,
							   lpFmtHeader,
							   lpFmtColStr,
							   lpaplChar,
							   aplDimNRows,
							   aplDimNCols,
							   aplRank,
							   lpMemDim);
			break;

		case ARRAY_HETERO:
////////////lpaplChar =
			CompileArrHetero  ((LPAPLHETERO) lpMem,
							   lpFmtHeader,
							   lpFmtColStr,
							   lpaplChar,
							   aplDimNRows,
							   aplDimNCols,
							   aplRank,
							   lpMemDim);
			break;

		case ARRAY_NESTED:
////////////lpaplChar =
			CompileArrNested  ((LPAPLNESTED) lpMem,
							   lpFmtHeader,
							   lpFmtColStr,
							   lpaplChar,
							   aplDimNRows,
							   aplDimNCols,
							   aplRank,
							   lpMemDim);
			break;

		defstop
			break;
	} // End SWITCH

	// Propogate the row & col count up the line
	PropogateRowColCount (lpFmtHeader,
						  FALSE);

	// Add up the width of each column to get the
	//	 # cols in the result
	for (aplLastDim = aplDimCol = 0; aplDimCol < aplDimNCols; aplDimCol++)
		aplLastDim += (lpFmtColStr[aplDimCol].uInts
					 + lpFmtColStr[aplDimCol].uFrcs);
	Assert (aplLastDim EQ (lpFmtHeader->uFmtInts
						 + lpFmtHeader->uFmtFrcs
						 + lpFmtHeader->uFmtTrBl));
	// Calculate the NELM of the result
	aplNELMRes = (lpFmtHeader->uFmtRows * (lpFmtHeader->uFmtInts
										 + lpFmtHeader->uFmtFrcs
										 + lpFmtHeader->uFmtTrBl));
#ifdef PREFILL
	// Fill the output area with all blanks
	for (lpaplChar = lpwszTemp, aplDimCol = 0; aplDimCol < aplNELMRes; aplDimCol++)
		*lpaplChar++ = L' ';
#endif
	lpaplChar = lpwszTemp;

	// Run through the array again processing the
	//	 output stream into lpwszTemp

	// Split cases based upon the array's storage type
	switch (aplType)
	{
		case ARRAY_BOOL:
		case ARRAY_INT:
		case ARRAY_FLOAT:
		case ARRAY_CHAR:
		case ARRAY_APA:
		case ARRAY_HETERO:
////////////lpaplChar =
			FmtArrSimple (lpFmtHeader,				// Ptr to FMTHEADER
						  lpFmtColStr,				// Ptr to vector of <aplDimNCols> FMTCOLSTRs
						  lpaplCharStart,			// Ptr to compiled input
						 &lpaplChar,				// Ptr to output string
						  aplDimNRows,				// # rows in this array
						  aplDimNCols,				// # cols in this array
						  aplLastDim,				// Length of last dim in result (NULL for !bRawOutput)
						  aplRank,					// Rank of this array
						  lpMemDim, 				// Ptr to this array's dimensions
						  TRUE);					// TRUE iff raw output
			break;

		case ARRAY_NESTED:
////////////lpaplChar =
			FmtArrNested (lpFmtHeader,				// Ptr to FMTHEADER
						  lpMem,					// Ptr to raw input
						  lpFmtColStr,				// Ptr to vector of <aplDimNCols> FMTCOLSTRs
						  lpaplCharStart,			// Ptr to compiled input
						 &lpaplChar,				// Ptr to ptr to output string
						  aplDimNRows,				// # rows in this array
						  aplDimNCols,				// # cols ...
						  aplLastDim,				// Length of last dim in result (NULL for !bRawOutput)
						  FALSE);					// TRUE iff raw (not {thorn}) output
			// Loop through the formatted rows
			for (lpwsz = lpwszTemp, uCol = 0; uCol < lpFmtHeader->uFmtRows; uCol++, lpwsz += aplLastDim)
			{
				WCHAR wch;

				// Because AppendLine works on single zero-terminated lines,
				//	 we need to create one
				wch = lpwsz[aplLastDim];		// Save the ending char
				lpwsz[aplLastDim] = L'\0';      // Terminate the line
				AppendLine (lpwsz, FALSE);		// Display the line
				lpwsz[aplLastDim] = wch;		// restore the ending char
			} // End FOR

			break;

		defstop
			break;
	} // End SWITCH
NORMAL_EXIT:
	// We no longer need this ptr
	MyGlobalUnlock (hGlb); lpMem = NULL;
} // End DisplayGlbArr


//***************************************************************************
//	FormatImmed
//
//	Format an immediate constant
//***************************************************************************

LPAPLCHAR FormatImmed
	(LPWCHAR	lpaplChar,
	 UINT		ImmType,
	 LPVOID 	lpData)

{
	WCHAR wc;

	switch (ImmType)
	{
		case IMMTYPE_BOOL:
			lpaplChar =
			FormatAplint (lpaplChar,
						  *(LPAPLBOOL) lpData);
			break;

		case IMMTYPE_INT:
			lpaplChar =
			FormatAplint (lpaplChar,
						  *(LPAPLINT) lpData);
			break;

		case IMMTYPE_CHAR:
			// Get the char
			wc = *(LPAPLCHAR) lpData;

			// Test for Terminal Control chars
			switch (wc)
			{
				case TCBEL: 	// Bel
				case TCBS:		// BS
				case TCDEL: 	// Del
				case TCESC: 	// Esc
				case TCFF:		// FF
				case TCHT:		// HT
				case TCLF:		// LF
				case TCNL:		// NL
				case TCNUL: 	// NUL
					DbgBrk ();	// ***FINISHME***








					break;
			} // End SWITCH

			lpaplChar +=
			wsprintfW (lpaplChar,
///////////////////////L"'%c' ",
					   L"%c ",
					   wc);
			break;

		case IMMTYPE_FLOAT:
			lpaplChar =
			FormatFloat (lpaplChar, *(LPAPLFLOAT) lpData);

			break;

		defstop
			break;
	} // End SWITCH

	return lpaplChar;
} // End FormatImmed


//***************************************************************************
//	FormatAplint
//
//	Format an APLINT
//***************************************************************************

LPAPLCHAR FormatAplint
	(LPAPLCHAR lpaplChar,
	 APLINT    aplInt)

{

#define MAXLEN	32

	WCHAR wszTemp[MAXLEN + 1];
	int   i;
	UINT  u;

	// Check the sign bit -- if set, save and make positive
	if (aplInt < 0)
	{
		*lpaplChar++ = UCS2_OVERBAR;
		aplInt = -aplInt;
	} // End IF

	// Format the number one digit at a time
	//	 in reverse order into lpwszTemp
	i = MAXLEN - 1;

	// Execute at least once so zero prints
	do
	{
		u = (UINT) (aplInt % 10);
		wszTemp[i--] = u + L'0';
		aplInt = aplInt / 10;
	} while (aplInt);

	// Copy the digits to the result
	for (i++; i < MAXLEN; i++)
		*lpaplChar++ = wszTemp[i];

	// Include a separator
	*lpaplChar++ = L' ';

	return lpaplChar;
} // End FormatAplint


//***************************************************************************
//	FormatFloat
//
//	Format a floating point number
//***************************************************************************

LPAPLCHAR FormatFloat
	(LPWCHAR  lpaplChar,
	 APLFLOAT fFloat)

{
	if (!_finite (fFloat))
	{
		if (fFloat < 0)
			*lpaplChar++ = UCS2_OVERBAR;
		*lpaplChar++ = L'_';
	} else
	{
#if TRUE
		LPAPLCHAR p, q;

		// ***FIXME*** -- _gcvt returns 17 digits
		//				  only 16 of which are accurate

		// Convert fFloat to an ASCII string
		_gcvt (fFloat, (int) uQuadPP, lpszTemp);

		// Convert from one-byte ASCII to two-byte UCS2
		A2W (lpaplChar, lpszTemp);

		// Check for minus sign in the mantissa
		if (lpaplChar[0] EQ L'-')
			lpaplChar[0] = UCS2_OVERBAR;

		// p points to the trailing zero
		p = lpaplChar + lstrlenW (lpaplChar);
		q = strchrW (lpaplChar, L'e');
		if (q)
		{
			// Check for trailing decimal point in the mantissa
			if (q[-1] EQ L'.')
			{
				// Use MoveMemory as the source and destin blocks overlap
				MoveMemory (q + 1, q, (1 + p++ - q) * sizeof (APLCHAR));
				*q++ = L'0';    // Change to zero and skip over so it ends with ".0"
			} // End IF

			*q++ = L'E';        // Change to uppercase and skip over

			// Check for minus sign in the exponent
			if (q[0] EQ L'-')
				*q++ = UCS2_OVERBAR;	// Change to high minus and skip over
			else
			// Check for plus sign in the exponent
			if (q[0] EQ L'+')
				// Delete by copying over
				CopyMemory (q, q + 1, (1 + p-- - (q + 1)) * sizeof (APLCHAR));

			// Check for leading 0s in the exponent
			while (q[0] EQ L'0')
				// Delete by copying over
				CopyMemory (q, q + 1, (1 + p-- - (q + 1)) * sizeof (APLCHAR));

		} else
		// Check for trailing decimal point in the mantissa
		if (p[-1] EQ '.')
			*--p = L'\0';        // Zap it and back up to the terminating zero

		// Point to the terminating zero
		lpaplChar = p;
#else
		int 	  iLen;
		LPAPLCHAR p, q, r;

		iLen =
		sprintfW (lpaplChar,
				  L"%.*g",
				  uQuadPP,
				  fFloat);
		// Strip trailing zeros
		for (p = &lpaplChar[lstrlenW (lpaplChar) - 1];
			 p > lpaplChar && *p EQ L'0';
			 p--)
			*p = L'\0';     // Zap it

		// Strip trailing decimal point
		if (*p EQ L'.')
			*p-- = L'\0';

		// If the resulting display is "0"
		//	 and fFloat is not zero, switch to
		//	 exponential format.
		iLen = lpaplChar[0] EQ '-'; // Skip over minus sign
		if (lpaplChar[iLen	  ] EQ L'0'
		 && lpaplChar[iLen + 1] EQ L'\0'
		 && fFloat NE 0)
		{
			p = lpaplChar +
			sprintfW (lpaplChar,
					  L"%.*E",
					  uQuadPP,
					  fFloat);
			// p points to the trailing zero

			q = strchrW (lpaplChar, 'E');

			// Strip leading '+' in exponent
			if (q[1] EQ L'+')
				CopyMemory (q + 1, q + 2, (1 + p-- - (q + 2)) * sizeof (APLCHAR));

			// Strip leading zeros in exponent
			r = q + (q[1] EQ L'-'); // Skip over negative sign
			while (r[1] EQ L'0')
				CopyMemory (r + 1, r + 2, (1 + p-- - (r + 2)) * sizeof (APLCHAR));

			// Insert high minus sign
			if (q[1] EQ L'-')
				q[1] = UCS2_OVERBAR;

			// Strip trailing blanks in the mantissa
			while (*--q EQ L'0')
				CopyMemory (q, q + 1, (1 + p-- - (q + 1)) * sizeof (APLCHAR));

			// Strip trailing decimal point
			if (*q EQ L'.')
				CopyMemory (q, q + 1, (1 + p-- - (q + 1)) * sizeof (APLCHAR));

			// Point to the last char
			p--;
		} // End IF

		// Insert high minus sign
		if (lpaplChar[0] EQ L'-')
			lpaplChar[0] = UCS2_OVERBAR;

		lpaplChar += (p + 1 - lpaplChar);
#endif
	} // End IF/ELSE/...

	// Append a separator
	*lpaplChar++ = L' ';

	return lpaplChar;
} // End FormatFloat


//***************************************************************************
//	FormatSymTabConst
//
//	Format a symbol table constant
//***************************************************************************

LPAPLCHAR FormatSymTabConst
	(LPWCHAR	lpaplChar,
	 LPSYMENTRY lpSymEntry)

{
	Assert (lpSymEntry->stFlags.Imm);

	return FormatImmed (lpaplChar,
						lpSymEntry->stFlags.ImmType,
					   &lpSymEntry->stData.lpVoid);
} // End FormatSymTabConst


#ifdef DEBUG
//***************************************************************************
//	DisplayHshTab
//
//	Display the Hash Table entries
//***************************************************************************

void DisplayHshTab
	(void)

{
	LPHSHENTRY lpHshEntry;
	int 	   i, j;

	typedef struct tagHT_FLAGNAMES
	{
		UINT	uMask;
		LPWCHAR lpwszName;
	} HT_FLAGNAMES, *LPHT_FLAGNAMES;

	// Hash Table flag names
	static HT_FLAGNAMES ahtFlagNames[] =
	{
	 {0x00001,	L" Inuse"    },
	 {0x00002,	L" PrinHash" },
	 {0x00004,	L" SymCopy"  },
	};

	DbgMsg ("********** Hash Table **********************************");

	wsprintf (lpszTemp,
			  "lpHshTab = %08X, SplitNext = %08X, Last = %08X",
			  lpHshTab,
			  lpHshTabSplitNext,
			  &lpHshTab[iHshTabTotalSize]);
	DbgMsg (lpszTemp);

	// Display the hash table
	for (lpHshEntry = lpHshTab, i = 0;
		 i < iHshTabTotalSize;
		 lpHshEntry++, i++)
	{
		WCHAR wszFlags[128] = {L'\0'};
		UINT  htFlags;

		// Format the flags
		htFlags = *(UINT *) &lpHshEntry->htFlags;
		for (j = 0;
			 j < (sizeof (ahtFlagNames) / sizeof (ahtFlagNames[0]));
			 j++)
		if (htFlags & ahtFlagNames[j].uMask)
			lstrcatW (wszFlags, ahtFlagNames[j].lpwszName);

		// In case we didn't find any matching flags,
		//	 set the second byte to zero as well as
		//	 when we do find flags, we skip over the
		//	 leading blank.
		if (wszFlags[0] EQ L'\0')
			wszFlags[1] =  L'\0';

		if (lpHshEntry->htFlags.Inuse)
		{
			LPSYMENTRY lpSymEntry;

			lpSymEntry = lpHshEntry->lpSymEntry;
			if (lpSymEntry->stFlags.Imm)
				wsprintfW (lpwszTemp,
						   L"HT:%3d uH=%08X, uH&M=%d, <%s>, ull=%08X%08X, Sym=%08X",
						   i,
						   lpHshEntry->uHash,
						   lpHshEntry->uHashAndMask,
						   &wszFlags[1],
						   HIDWORD (lpSymEntry->stData.stInteger),
						   LODWORD (lpSymEntry->stData.stInteger),
						   lpSymEntry);
			else
			if (lpSymEntry->stFlags.UsrName
			 || lpSymEntry->stFlags.SysName)
			{
				LPCHAR lpGlbName;

				lpGlbName = GlobalLock (lpHshEntry->hGlbName); Assert (lpGlbName NE NULL);

				wsprintfW (lpwszTemp,
						   L"HT:%3d uH=%08X, uH&M=%d, <%s>, <%s>, Sym=%08X, %08X-%08X",
						   i,
						   lpHshEntry->uHash,
						   lpHshEntry->uHashAndMask,
						   &wszFlags[1],
						   lpGlbName,
						   lpSymEntry,
						   lpHshEntry->NextSameHash,
						   lpHshEntry->PrevSameHash);
				// We no longer need this ptr
				GlobalUnlock (lpHshEntry->hGlbName); lpGlbName = NULL;
			} // End IF/ELSE/IF
		} else
			wsprintfW (lpwszTemp,
					   L"HT:%3d (EMPTY) <%s>, Sym=%08X, <%08X-%08X>",
					   i,
					   &wszFlags[1],
					   lpHshEntry->lpSymEntry,
					   lpHshEntry->NextSameHash,
					   lpHshEntry->PrevSameHash);
		DbgMsgW (lpwszTemp);
	} // End FOR

	DbgMsg ("********** End Hash Table ******************************");

	UpdateWindow (hWndDB);
} // End DisplayHshTab
#endif


#ifdef DEBUG
//***************************************************************************
//	DisplaySymTab
//
//	Display the Symbol Table entries
//
//	If bDispAll is FALSE, display only those non-SysName entries
//	  with a non-zero reference count.
//***************************************************************************

void DisplaySymTab
	(BOOL bDispAll)

{
	LPSYMENTRY lpSymEntry;
	int 	   i, j;

	typedef struct tagST_FLAGNAMES
	{
		UINT	uMask;
		LPWCHAR lpwszName;
	} ST_FLAGNAMES, *LPST_FLAGNAMES;

	// Symbol Table flag names
	static ST_FLAGNAMES astFlagNames[] =
	{
//// {0x000001,  L" Imm"        },
//// {0x00001E,  L" ImmType"    },
	 {0x000020,  L" SysName"    },
	 {0x000040,  L" SysVar"     },
	 {0x000080,  L" SysFn0"     },
	 {0x000100,  L" SysFn12"    },
	 {0x000200,  L" NotCase"    },
	 {0x000400,  L" Perm"       },
	 {0x000800,  L" Inuse"      },
	 {0x001000,  L" Value"      },
	 {0x002000,  L" UsrName"    },
	 {0x004000,  L" UsrVar"     },
	 {0x008000,  L" UsrFn0"     },
	 {0x010000,  L" UsrFn12"    },
	 {0x020000,  L" UsrOp1"     },
	 {0x040000,  L" UsrOp2"     },
//// {0x780000,  L" SysVarValid"},
	};

	if (bDispAll)
		DbgMsg ("********** Symbol Table ********************************");
	else
		DbgMsg ("********** Symbol Table Referenced Non-SysNames ******** ");

	wsprintf (lpszTemp,
			  "lpSymTab = %08X, Last = %08X",
			  lpSymTab,
			  &lpSymTab[iSymTabTotalSize]);
	DbgMsg (lpszTemp);

	// Display the symbol table
	for (lpSymEntry = lpSymTab, i = 0;
		 lpSymEntry NE lpSymTabNext;
		 lpSymEntry++, i++)
	if (bDispAll ||
		!lpSymEntry->stFlags.SysName)
	{
		WCHAR	wszFlags[128] = {L'\0'};
		STFLAGS stFlags;
		LPWCHAR lpGlbName;

		// Format the flags
		stFlags = lpSymEntry->stFlags;
		if (stFlags.Imm)
			wsprintfW (&wszFlags[lstrlenW (wszFlags)],
					   L" Imm/Type=%d",
					   stFlags.ImmType);
		for (j = 0;
			 j < (sizeof (astFlagNames) / sizeof (astFlagNames[0]));
			 j++)
		if ((*(UINT *) &stFlags) & astFlagNames[j].uMask)
			lstrcatW (wszFlags, astFlagNames[j].lpwszName);

		if (stFlags.SysVar)
			wsprintfW (&wszFlags[lstrlenW (wszFlags)],
					   L" SysVarValid=%d",
					   stFlags.SysVarValid);

		// In case we didn't find any matching flags,
		//	 set the second WCHAR to zero as well --
		//	 when we do find flags, we skip over the
		//	 leading blank.
		if (wszFlags[0] EQ L'\0')
			wszFlags[1] =  L'\0';

		if (lpSymEntry->stFlags.Inuse)
		{
			WCHAR wszName[128] = {'\0'};

			if (lpSymEntry->stFlags.UsrName
			 || lpSymEntry->stFlags.SysName)
			{
				LPHSHENTRY lpHshEntry;

				lpHshEntry = lpSymEntry->lpHshEntry;

				if (lpHshEntry)
				{
					lpGlbName = GlobalLock (lpHshEntry->hGlbName); Assert (lpGlbName NE NULL);

					lstrcpynW (wszName, lpGlbName, sizeof (wszName) / sizeof (wszName[0]));

					// We no longer need this ptr
					GlobalUnlock (lpHshEntry->hGlbName); lpGlbName = NULL;
				} // End IF
			} // End IF

			if (lpSymEntry->stFlags.Imm)
			{
				wsprintfW (lpwszTemp,
						   L"ST:%08X <%s> <%s>, ull=%08X%08X, Hsh=%08X",
						   lpSymEntry,
						   wszName,
						   &wszFlags[1],
						   HIDWORD (lpSymEntry->stData.stInteger),
						   LODWORD (lpSymEntry->stData.stInteger),
						   lpSymEntry->lpHshEntry);
			} else
			if (lpSymEntry->stFlags.UsrName
			 || lpSymEntry->stFlags.SysName)
			{
				LPHSHENTRY lpHshEntry;

				lpHshEntry = lpSymEntry->lpHshEntry;

				if (lpHshEntry)
				{
					wsprintfW (lpwszTemp,
							   L"ST:%08X <%s>, <%s>, Data=%08X, Hsh=%08X",
							   lpSymEntry,
							   wszName,
							   &wszFlags[1],
							   lpSymEntry->stData.lpVoid,
							   lpHshEntry);
				} else
					wsprintfW (lpwszTemp,
							   L"ST:%08X <******>, <%s>, Hsh=0",
							   lpSymEntry,
							   &wszFlags[1]);
			} // End IF/ELSE/IF
		} else
			wsprintfW (lpwszTemp,
					   L"ST:%08X (EMPTY) <%s>, Hsh=%08X",
					   lpSymEntry,
					   &wszFlags[1],
					   lpSymEntry->lpHshEntry);
		DbgMsgW (lpwszTemp);
	} // End FOR

	DbgMsg ("********** End Symbol Table ****************************");

	UpdateWindow (hWndDB);
} // End DisplaySymTab
#endif


#ifdef DEBUG
//***************************************************************************
//	DisplayHistory
//
//	Display the session manager history
//***************************************************************************

void DisplayHistory
	(void)

{
	int 	  i;
	char	  szConvert[1024];
	LPGLBHIST lpGlbHist;
	HGLOBAL   hGlb;
	LPWCHAR   wszLine;

	// Display the hGlbHist entries

	DbgMsg ("********** History *************************************");

	// Get the global handle to the history buffer
	// Note we don't use MyGlobalLock here as the caller
	//	 might have one of the handles locked and MyGlobalLock
	//	 expects the lock count to be zero.
	lpGlbHist = GlobalLock (hGlbHist); Assert (lpGlbHist NE NULL);

	// Output relevant variables
	wsprintf (lpszTemp,
			  "SM:  iCurLine = %d, iLastValidLine = %d",
			  iCurLine,
			  iLastValidLine
			 );
	DbgMsg (lpszTemp);
	wsprintf (lpszTemp,
			  "SM:  iCurChar = %d, nWindowChars = %d, nWindowLines = %d",
			  iCurChar,
			  nWindowChars,
			  nWindowLines
			 );
	DbgMsg (lpszTemp);
	wsprintf (lpszTemp,
			  "SM:  iFirstWindowChar = %d, iLastWindowChar = %d",
			  iFirstWindowChar,
			  iLastWindowChar
			 );
	DbgMsg (lpszTemp);

	// Loop through the entries
	for (i = 0; i < 10; i++)
	if (hGlb = lpGlbHist[i].hGlb)
	{
		// Lock the memory to get a ptr to it
		wszLine = GlobalLock (hGlb); Assert (wszLine NE NULL);

		// Convert the WCHAR line wszLine to a single-byte string
		W2A (szConvert, wszLine, sizeof (szConvert) - 1);

		wsprintf (lpszTemp,
				  "SM:  %2d:  <%s> (%d) ContPrev=%d, ContNext=%d",
				  i,
				  szConvert,
				  lstrlen (szConvert),
				  lpGlbHist[i].ContPrev,
				  lpGlbHist[i].ContNext
				  );
		DbgMsg (lpszTemp);

		// We no longer need this ptr
		MyGlobalUnlock (hGlb); wszLine = NULL;
	} // End FOR/IF

	// Convert the WCHAR line wszLine to a single-byte string
	W2A (szConvert, lpwszCurLine, sizeof (szConvert) - 1);

	wsprintf (lpszTemp,
			  "SM:   *:  <%s> (%d)",
			  szConvert,
			  lstrlen (szConvert)
			  );
	DbgMsg (lpszTemp);

	// We no longer need this ptr
	GlobalUnlock (hGlbHist); lpGlbHist = NULL;

	DbgMsg ("********** End History *********************************");

	UpdateWindow (hWndDB);
} // End DisplayHistory
#endif


#ifdef DEBUG
//***************************************************************************
//	DisplayGlobals
//
//	Display outstanding global memory objects
//***************************************************************************
void DisplayGlobals
	(BOOL bDispAll)

{
	int 	  i;
	HGLOBAL   hGlb;
	LPVOID	  lpMem;
	APLDIM	  aplDim;
	LPVOID	  lpData;
	APLCHAR   aplArrChar[19];
	LPAPLCHAR lpwsz;

	DbgMsg ("********** Globals *************************************");

	for (i = 0; i < MAXOBJ; i++)
	if (hGlb = ahGLOBAL[i])
	{
		lpMem = GlobalLock (hGlb); Assert (lpMem NE NULL);

		if (!lpMem)
		{
			wsprintf (lpszTemp,
					  "hGlb=%08X *** INVALID ***",
					  hGlb);
			DbgMsg (lpszTemp);

			continue;
		} // End IF

#define lpHeader	((LPVARARRAY_HEADER) lpMem)
		if (lpHeader->Sign.ature EQ VARARRAY_HEADER_SIGNATURE)
		{
			if (bDispAll
			 || !lpHeader->SysVar)
			{
				// It's a valid HGLOBAL variable array
				Assert (IsGlbTypeVarDir (MakeGlbTypeGlb (hGlb)));

				if (lpHeader->Rank EQ 0)
					aplDim = (APLDIM) -1;
				else
					aplDim = *VarArrayBaseToDim (lpHeader);
				// Skip over the header and dimension to the data
				lpData = VarArrayBaseToData (lpHeader, lpHeader->Rank);

				// Split cases based upon the array storage type
				switch (lpHeader->ArrType)
				{
					case ARRAY_BOOL:
					case ARRAY_INT:
					case ARRAY_FLOAT:
					case ARRAY_APA:
						if (lpHeader->NELM EQ 0)
						{
							aplArrChar[0] = UCS2_ZILDE;
							aplArrChar[1] = L'\0';
						} else
						{
							lpwsz =
							FormatImmed (aplArrChar,
										 TranslateArrayTypeToImmType (lpHeader->ArrType),
										 lpData);
							// Delete the trailing blank
							lpwsz[-1] = L'\0';
						} // End IF/ELSE

						break;

					case ARRAY_HETERO:
					case ARRAY_NESTED:
						aplArrChar[0] = L'\0';

						break;

					case ARRAY_CHAR:
						lstrcpynW (aplArrChar, lpData, 1 + (UINT) min (6, lpHeader->NELM));
						aplArrChar[min (6, lpHeader->NELM)] = L'\0';

						break;

					defstop
						break;
				} // End SWITCH

				wsprintfW (lpwszTemp,
						   L"hGlb=%08X, ArrType=%c%c, NELM=%3d, RC=%1d, Rank=%2d, Dim1=%3d, Line#=%4d, (%s)",
						   hGlb,
						   L"BIFCHNLA"[lpHeader->ArrType],
						   L" *"[lpHeader->Perm],
///////////////////////////HIDWORD (lpHeader->NELM),
						   LODWORD (lpHeader->NELM),
						   lpHeader->RefCnt,
						   LODWORD (lpHeader->Rank),
						   LODWORD (aplDim),
						   auLinNumGLOBAL[i],
						   aplArrChar);
				DbgMsgW (lpwszTemp);
			} // End IF
		} else
#undef	lpHeader
#define lpHeader	((LPFCNARRAY_HEADER) lpMem)
		if (lpHeader->Sign.ature EQ FCNARRAY_HEADER_SIGNATURE)
		{
			// It's a valid HGLOBAL function array
			Assert (IsGlbTypeFcnDir (MakeGlbTypeGlb (hGlb)));

			wsprintf (lpszTemp,
					  "hGlb=%08X, FcnType=%1d,  NELM=%3d, RC=%1d,                    Line#=%4d",
					  hGlb,
					  lpHeader->FcnType,
					  LODWORD (lpHeader->NELM),
					  lpHeader->RefCnt,
					  auLinNumGLOBAL[i]);
			DbgMsg (lpszTemp);
		} else
#undef	lpHeader
		if (bDispAll)
		{
			wsprintf (lpszTemp,
					  "hGlb=%08X -- No NARS/FCNS Signature",
					  hGlb);
			DbgMsg (lpszTemp);
		} // End IF/ELSE

		// We no longer need this ptr
		GlobalUnlock (hGlb); lpMem = NULL;
	} // End FOR/IF

	DbgMsg ("********** End Globals *********************************");

	UpdateWindow (hWndDB);
} // End DisplayGlobals
#endif


#ifdef DEBUG
//***************************************************************************
//	DisplayTokens
//
//	Display the contents of the current token stream
//***************************************************************************

void DisplayTokens
	(HGLOBAL hGlbToken)

{
	LPTOKEN lpToken;
	int i, iLen;

	DbgMsg ("********** Tokens **************************************");

	// Ensure it's valid
	if (!hGlbToken)
	{
		DbgMsg ("DisplayTokens:  ***INAVLID HANDLE***:  hGlbToken == 0");
		return;
	} // End IF

	// Note we don't use MyGlobalLock & MyGlobalUnlock
	//	 as they expect the lock count to be zero
	//	 which need not be the way our caller left it.
	lpToken = GlobalLock (hGlbToken); Assert (lpToken NE NULL);

	// Ensure it's valid
	if (!lpToken)
	{
		DbgMsg ("DisplayTokens:  ***INAVLID HANDLE***:  GlobalLock failed");
		return;
	} // End IF

#define lpHeader	((LPTOKEN_HEADER) lpToken)

	wsprintf (lpszTemp,
			  "lpToken = %08X, Version # = %d, TokenCnt = %d, PrevGroup = %d",
			  lpToken,
			  lpHeader->iVersion,
			  lpHeader->iTokenCnt,
			  lpHeader->iPrevGroup);
	DbgMsg (lpszTemp);

	iLen = lpHeader->iTokenCnt;

#undef	lpHeader

	lpToken = TokenBaseToStart (lpToken);	// Skip over TOKEN_HEADER

	for (i = 0; i < iLen; i++, lpToken++)
	{
		wsprintf (lpszTemp,
				  "(%2d) Data=%08X%08X, CharIndex=%2d, Type=%s",
				  i,
				  HIDWORD (*(LPAPLINT) &lpToken->tkData.tkFloat),
				  LODWORD (*(LPAPLINT) &lpToken->tkData.tkFloat),
				  lpToken->tkCharIndex,
				  GetTokenTypeName (lpToken->tkFlags.TknType));
		DbgMsg (lpszTemp);
	} // End FOR

	DbgMsg ("********** End Tokens **********************************");

	// We no longer need this ptr
	GlobalUnlock (hGlbToken); lpToken = NULL;
} // End DisplayTokens
#endif


#ifdef DEBUG
//***************************************************************************
//	GetTokenTypeName
//
//	Convert a token type value to a name
//***************************************************************************

LPCHAR GetTokenTypeName
	(UINT uType)

{
typedef struct tagTOKENNAMES
{
	LPCHAR lpsz;
	UINT   uTokenNum;
} TOKENNAMES, *LPTOKENNAMES;

static TOKENNAMES tokenNames[] =
{{"VARNAMED"  , TKT_VARNAMED }, //  1: Symbol table entry for a named var (data is LPSYMENTRY)
 {"STRING"    , TKT_STRING   }, //  2: String  (data is HGLOBAL)
 {"VARIMMED"  , TKT_VARIMMED }, //  3: Immediate data (data is immediate)
 {"COMMENT"   , TKT_COMMENT  }, //  4: Comment (data is HGLOBAL)
 {"ASSIGN"    , TKT_ASSIGN   }, //  5: Assignment symbol (data is UCS2_LEFTARROW)
 {"LISTSEP"   , TKT_LISTSEP  }, //  6: List separator    (data is ';')
 {"FCNIMMED"  , TKT_FCNIMMED }, //  7: Primitive function (any valence) (data is UCS2_***)
 {"OP1IMMED"  , TKT_OP1IMMED }, //  8: Monadic primitive operator (data is UCS2_***)
 {"OP2IMMED"  , TKT_OP2IMMED }, //  9: Dyadic  ...
 {"JOTDOT"    , TKT_JOTDOT   }, // 10: Outer product monadic operator (with right scope) (data is NULL)
 {"LPAREN"    , TKT_LPAREN   }, // 11: Left paren (data is TKT_LPAREN)
 {"RPAREN"    , TKT_RPAREN   }, // 12: Right ...   ...         RPAREN
 {"LBRACKET"  , TKT_LBRACKET }, // 13: Left bracket ...        LBRACKET
 {"RBRACKET"  , TKT_RBRACKET }, // 14: Right ...   ...         RBRACKET
 {"EOS"       , TKT_EOS      }, // 15: End-of-Stmt (data is length of stmt including this token)
 {"EOL"       , TKT_EOL      }, // 16: End-of-Line  ...
 {"LINECONT"  , TKT_LINECONT }, // 17: Line continuation (data is NULL)
 {"INPOUT"    , TKT_INPOUT   }, // 18: Input/Output (data is UCS2_QUAD or UCS2_QUOTEQUAD)
 {"STRAND"    , TKT_STRAND   }, // 19: Strand accumulating (data is LPTOKEN)
 {"LISTINT"   , TKT_LISTINT  }, // 20: List in parens    (data is HGLOBAL)
 {"LISTPAR"   , TKT_LISTPAR  }, // 21: List in parens    (data is HGLOBAL)
 {"LISTBR"    , TKT_LISTBR   }, // 22: List in brackets  (data is HGLOBAL)
 {"VARARRAY"  , TKT_VARARRAY }, // 23: Array of data (data is HGLOBAL)
 {"FCNARRAY"  , TKT_FCNARRAY }, // 24: Array of functions (data is HGLOBAL)
 {"FCNNAMED"  , TKT_FCNNAMED }, // 25: Symbol table entry for a named function (data is LPSYMENTRY)
 {"AXISIMMED" , TKT_AXISIMMED}, // 26: An immediate axis specification (data is immediate)
 {"AXISARRAY" , TKT_AXISARRAY}, // 27: An array of  ...   (data is HGLOBAL)
 {"OP1NAMED"  , TKT_OP1NAMED }, // 28: A named monadic primitive operator (data is LPSYMENTRY)
 {"OP2NAMED"  , TKT_OP2NAMED }, // 29: ...     dyadic  ...
 {"STRNAMED"  , TKT_STRNAMED }, // 30: ...     strand  ...
};

	if ((sizeof (tokenNames) / sizeof (tokenNames[0])) > (uType - TKT_FIRST))
		return tokenNames[uType - TKT_FIRST].lpsz;
	else
	{
		static char szTemp[64];

		wsprintf (szTemp,
				  "GetTokenTypeName:  *** Unknown Token Type:  %d",
				  uType);
		return szTemp;
	} // End IF/ELSE
} // End GetTokenTypeName
#endif


#ifdef DEBUG
//***************************************************************************
//	DisplayFcnStrand
//
//	Display a function strand
//***************************************************************************

void DisplayFcnStrand
	(LPTOKEN lpToken)

{
	HGLOBAL hGlbData;
	LPWCHAR lpaplChar = lpwszTemp;

	// Split cases based upon the token type
	switch (lpToken->tkFlags.TknType)
	{
		case TKT_VARNAMED:			// Because we don't distinguish between
									//	 functions and variables in AssignName_EM
			return;

		case TKT_FCNIMMED:
		case TKT_OP1IMMED:
		case TKT_OP2IMMED:
			lpaplChar += wsprintfW (lpaplChar,
									L"FcnType=%1d, NELM=%3d, RC=%2d, Fn:  ",
									FCNTYPE_FCN,// lpHeader->FcnType,
									1,			// LODWORD (lpHeader->NELM),
									0); 		// lpHeader->RefCnt);
			*lpaplChar++ = lpToken->tkData.tkChar;

			break;

		case TKT_FCNNAMED:
		case TKT_OP1NAMED:
		case TKT_OP2NAMED:
			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

			// If it's not immediate, ...
			if (!lpToken->tkData.lpSym->stFlags.Imm)
			{
				hGlbData = lpToken->tkData.lpSym->stData.stGlbData;

				// stData is an HGLOBAL function array
				Assert (IsGlbTypeFcnDir (hGlbData));

				lpaplChar = DisplayFcnGlb (lpaplChar, ClrPtrTypeDirGlb (hGlbData), TRUE);
			} else
			{
				lpaplChar += wsprintfW (lpaplChar,
										L"FcnType=%1d, NELM=%3d, RC=%2d, Fn:  ",
										FCNTYPE_FCN,// lpHeader->FcnType,
										1,			// LODWORD (lpHeader->NELM),
										0); 		// lpHeader->RefCnt);
				*lpaplChar++ = lpToken->tkData.lpSym->stData.stChar;
			} // End IF/ELSE

			break;

		case TKT_FCNARRAY:
			hGlbData = lpToken->tkData.tkGlbData;

			// tkData is an HGLOBAL function array
			Assert (IsGlbTypeFcnDir (hGlbData));

			lpaplChar = DisplayFcnGlb (lpaplChar, ClrPtrTypeDirGlb (hGlbData), TRUE);

			break;

		defstop
			break;
	} // End SWITCH

	// Ensure properly terminated
	*lpaplChar = L'\0';

	// Display the line in the debugging window
	DbgMsgW (lpwszTemp);
} // End DisplayFcnStrand
#endif


#ifdef DEBUG
//***************************************************************************
//	DisplayFcnGlb
//
//	Display a function from an HGLOBAL
//***************************************************************************

LPWCHAR DisplayFcnGlb
	(LPWCHAR lpaplChar,
	 HGLOBAL hGlb,
	 BOOL	 bDispHeader)

{
	LPVOID	  lpMem;
	APLNELM   aplNELM;
	LPYYSTYPE lpYYMem;

	// Lock the memory to get a ptr to it
	lpMem = MyGlobalLock (hGlb);

#define lpHeader	((LPFCNARRAY_HEADER) lpMem)
	aplNELM = lpHeader->NELM;
	if (bDispHeader)
		lpaplChar += wsprintfW (lpaplChar,
////////////////////////////////L"FcnType=%1d, NELM=%08X%08X, RC=%2d, Fn:  ",
								L"FcnType=%1d, NELM=%3d, RC=%2d, Fn:  ",
								lpHeader->FcnType,
////////////////////////////////HIDWORD (lpHeader->NELM),
								LODWORD (lpHeader->NELM),
								lpHeader->RefCnt);
#undef	lpHeader

	// Skip over the header to the data
	lpYYMem = FcnArrayBaseToData (lpMem);

	lpaplChar = DisplayFcnSub (lpaplChar, lpYYMem, aplNELM);

////switch (aplNELM)
////{
////	case 2: 				// e.g., {fn}{op1} or {fn}{jotdot}
////		// Handle {jotdot} in reverse
////		if (((LPYYSTYPE) lpMem)[1].tkToken.tkFlags.TknType EQ TKT_JOTDOT)
////		{
////			lpaplChar =
////			  DisplayFcnSub (lpaplChar, &((LPYYSTYPE) lpMem)[1].tkToken);
////			lpaplChar =
////			  DisplayFcnSub (lpaplChar, &((LPYYSTYPE) lpMem)[0].tkToken;
////		} else
////		{
////			lpaplChar =
////			  DisplayFcnSub (lpaplChar, &((LPYYSTYPE) lpMem)[0].tkToken);
////			lpaplChar =
////			  DisplayFcnSub (lpaplChar, &((LPYYSTYPE) lpMem)[1].tkToken);
////		} // End IF/ELSE
////
////		break;
////
////	case 3: 				// e.g., {fn}{op2}{fn}
////		lpaplChar =
////		  DisplayFcnSub (lpaplChar, &((LPYYSTYPE) lpMem)[0].tkToken);
////		lpaplChar =
////		  DisplayFcnSub (lpaplChar, &((LPYYSTYPE) lpMem)[1].tkToken);
////		lpaplChar =
////		  DisplayFcnSub (lpaplChar, &((LPYYSTYPE) lpMem)[2].tkToken);
////
////		break;
////
////	defstop
////		break;
////} // End SWITCH

	// We no longer need this ptr
	MyGlobalUnlock (hGlb); lpMem = lpYYMem = NULL;

	return lpaplChar;
} // End DisplayFcnGlb
#endif


#ifdef DEBUG
//***************************************************************************
//	DisplayFcnSub
//
//	Display function subroutine
//***************************************************************************

LPWCHAR DisplayFcnSub
	(LPWCHAR   lpaplChar,
	 LPYYSTYPE lpYYMem,
	 APLNELM   aplNELM)

{
	HGLOBAL hGlbData;

	// Split cases based upon the token type
	switch (lpYYMem[0].tkToken.tkFlags.TknType)
	{
		case TKT_OP1IMMED:
			// Check for axis operator
			if (aplNELM > 1
			 && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
			  || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
			{
				if (aplNELM > 2)
					lpaplChar =
					  DisplayFcnSub (lpaplChar, &lpYYMem[2], aplNELM - 2);	// fcn
				*lpaplChar++ = lpYYMem[0].tkToken.tkData.tkChar;			// Op1
				lpaplChar =
				  DisplayFcnSub (lpaplChar, &lpYYMem[1], 1);				// [X]
			} else
			{
				lpaplChar =
				  DisplayFcnSub (lpaplChar, &lpYYMem[1], aplNELM - 1);		// fcn
				*lpaplChar++ = lpYYMem[0].tkToken.tkData.tkChar;			// Op1
			} // End IF/ELSE/...

			break;

		case TKT_OP2IMMED:
			lpaplChar =
			  DisplayFcnSub (lpaplChar, &lpYYMem[1], lpYYMem[1].FcnCount);	// Lfcn
			*lpaplChar++ = lpYYMem[0].tkToken.tkData.tkChar;				// Op2
			if (lpYYMem[2].FcnCount > 1)
				*lpaplChar++ = L'(';
			lpaplChar =
			  DisplayFcnSub (lpaplChar, &lpYYMem[2], lpYYMem[2].FcnCount);	// Rfcn
			if (lpYYMem[2].FcnCount > 1)
				*lpaplChar++ = L')';
			break;

		case TKT_FCNIMMED:
			*lpaplChar++ = lpYYMem[0].tkToken.tkData.tkChar;				// fcn

			// Check for axis operator
			if (aplNELM > 1
			 && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
			  || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
			{
				lpaplChar =
				  DisplayFcnSub (lpaplChar, &lpYYMem[1], 1);				// [X]
			} // End IF

			break;

		case TKT_OP1NAMED:
			DbgBrk ();			// ***FINISHME***






			break;

		case TKT_OP2NAMED:
			DbgBrk ();			// ***FINISHME***





			break;

		case TKT_FCNNAMED:
			DbgBrk ();			// ***TESTME***

			// tkData is an LPSYMENTRY
			Assert (GetPtrTypeDir (lpYYMem->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

			// If it's not an immediate function, ...
			if (!lpYYMem->tkToken.tkData.lpSym->stFlags.Imm)
			{
				hGlbData = lpYYMem->tkToken.tkData.lpSym->stData.stGlbData;

				// stData is a valid HGLOBAL function array
				Assert (IsGlbTypeFcnDir (hGlbData));

				// Display the function strand in global memory
				lpaplChar = DisplayFcnGlb (lpaplChar, ClrPtrTypeDirGlb (hGlbData), FALSE);
			} else
				// Handle the immediate case
				*lpaplChar++ = lpYYMem->tkToken.tkData.lpSym->stData.stChar;

			// Check for axis operator
			if (aplNELM > 1
			 && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
			  || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
			{
				lpaplChar =
				  DisplayFcnSub (lpaplChar, &lpYYMem[1], 1);			// [X]
			} // End IF

			break;

		case TKT_JOTDOT:
			*lpaplChar++ = UCS2_JOT;
			*lpaplChar++ = L'.';

			// Surround with parens if more than one token
			if (aplNELM > 2)
				*lpaplChar++ = L'(';
			lpaplChar =
			  DisplayFcnSub (lpaplChar, &lpYYMem[1], aplNELM - 1);
			if (aplNELM > 2)
				*lpaplChar++ = L')';

			break;

		case TKT_AXISIMMED:
			*lpaplChar++ = L'[';
			lpaplChar =
			  FormatAplint (lpaplChar,
							lpYYMem[0].tkToken.tkData.tkInteger);
			lpaplChar[-1] = L']';   // Overwrite the trailing blank

			break;

		case TKT_AXISARRAY:
			*lpaplChar++ = L'[';

			hGlbData = lpYYMem->tkToken.tkData.tkGlbData;

			// tkData is a valid HGLOBAL variable array
			Assert (IsGlbTypeVarDir (hGlbData));
#if TRUE
			lstrcpyW (lpaplChar, L"TXT_AXISARRAY ");    // N.B.:  trailing blank is significant
			lpaplChar += lstrlenW (lpaplChar);
#else
			lpaplChar = FormatGlobal (lpaplChar, ClrPtrTypeDirGlb (hGlbData));
#endif
			lpaplChar[-1] = L']';   // Overwrite the trailing blank

			break;

		case TKT_VARIMMED:
			lpaplChar =
			  FormatAplint (lpaplChar,
							lpYYMem[0].tkToken.tkData.tkInteger);
			lpaplChar[-1] = L'\0';  // Overwrite the trailing blank

			break;

		case TKT_VARARRAY:
			hGlbData = lpYYMem->tkToken.tkData.tkGlbData;

			// tkData is a valid HGLOBAL variable array
			Assert (IsGlbTypeVarDir (hGlbData));
#if TRUE
			lstrcpyW (lpaplChar, L"TXT_VARARRAY ");     // N.B.:  trailing blank is significant
			lpaplChar += lstrlenW (lpaplChar);
#else
			lpaplChar =
			  FormatGlobal (lpaplChar, ClrPtrTypeDirGlb (hGlbData));
#endif
			lpaplChar[-1] = L'\0';  // Overwrite the trailing blank

			break;

		case TKT_VARNAMED:
			DbgBrk ();		// ***FINISHME***











			break;

		defstop
			break;
	} // End SWITCH

	return lpaplChar;
} // End DisplayFcnSub
#endif


//// #ifdef DEBUG
//// //***************************************************************************
//// //  DisplayFcnSub
//// //
//// //  Display function subroutine
//// //***************************************************************************
////
//// LPWCHAR DisplayFcnSub
////	 (LPWCHAR lpaplChar,
////	  LPTOKEN lpToken,
////	  BOOL	  bParensIfGlb)
////
//// {
////	 // Split cases based upon the token type
////	 switch (lpToken->tkFlags.TknType)
////	 {
////		 case TKT_JOTDOT:
////			 *lpaplChar++ = UCS2_JOT;
////			 *lpaplChar++ = L'.';
////
////			 break;
////
////		 case TKT_FCNIMMED:
////		 case TKT_OP1IMMED:
////		 case TKT_OP2IMMED:
////		 case TKT_LPAREN:
////		 case TKT_RPAREN:
////			 *lpaplChar++ = lpToken->tkData.tkChar;
////
////			 break;
////
////		 case TKT_FCNNAMED:
////			 // tkData is an LPSYMENTRY
////			 Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);
////
////			 // If it's an immediate
////			 if (lpToken->tkData.lpSym->stFlags.Imm)
////				 *lpaplChar++ = lpToken->tkData.lpSym->stData.stChar;
////			 else
////			 {
////				 // stData is a valid HGLOBAL function array
////				 Assert (IsGlbTypeFcnDir (lpToken->tkData.lpSym->stData.stGlbData));
////
////				 if (bParensIfGlb)
////					 *lpaplChar++ = L'(';
////
////				 lpaplChar =
////				 DisplayFcnGlb (lpaplChar,
////								ClrPtrTypeDirGlb (lpToken->tkData.lpSym->stData.stGlbData),
////								FALSE);
////				 if (bParensIfGlb)
////					 *lpaplChar++ = L')';
////			 } // End IF/ELSE
////
////			 break;
////
////		 case TKT_FCNARRAY:
////			 // tkData is a valid HGLOBAL function array
////			 Assert (IsGlbTypeFcnDir (lpToken->tkData.tkGlbData));
////
////			 *lpaplChar++ = L'(';
////
////			 lpaplChar =
////			 DisplayFcnGlb (lpaplChar,
////							ClrPtrTypeDirGlb (lpToken->tkData.tkGlbData),
////							FALSE);
////			 *lpaplChar++ = L')';
////
////			 break;
////
////		 defstop
////			 break;
////	 } // End SWITCH
////
////	 return lpaplChar;
//// } // End DisplayFcnSub
//// #endif


#ifdef DEBUG
//***************************************************************************
//	DisplayStrand
//
//	Display the strand stack
//***************************************************************************

void DisplayStrand
	(int strType)

{
	LPYYSTYPE lp, lpLast;

	switch (strType)
	{
		case VARSTRAND:
			DbgMsg ("********** Variable Strands ****************************");

			break;

		case FCNSTRAND:
			DbgMsg ("********** Function Strands ****************************");

			break;

		defstop
			break;
	} // End SWITCH

	wsprintf (lpszTemp,
			  "Start=%08X Base=%08X Next=%08X",
			  gplLocalVars.lpYYStrandStart[strType],
			  gplLocalVars.lpYYStrandBase[strType],
			  gplLocalVars.lpYYStrandNext[strType]);
	DbgMsg (lpszTemp);

	for (lp = gplLocalVars.lpYYStrandStart[strType], lpLast = NULL;
		 lp NE gplLocalVars.lpYYStrandNext[strType];
		 lp ++)
	{
		if (lpLast NE lp->unYYSTYPE.lpYYStrandBase)
		{
			DbgMsg ("--------------------------------------------------------");
			lpLast = lp->unYYSTYPE.lpYYStrandBase;
		} // End IF

		wsprintf (lpszTemp,
				  "Strand (%08X): %-9.9s D=%08X CI=%2d TC=%1d FC=%1d IN=%1d F=%08X B=%08X",
				  lp,
				  GetTokenTypeName (lp->tkToken.tkFlags.TknType),
				  LODWORD (lp->tkToken.tkData.tkInteger),
				  lp->tkToken.tkCharIndex,
				  lp->TknCount,
				  lp->FcnCount,
				  lp->Indirect,
				  lp->lpYYFcn,
				  lpLast);
		DbgMsg (lpszTemp);
	} // End FOR

	DbgMsg ("********** End Strands *********************************");
} // End DisplayStrand
#endif


#ifdef DEBUG
//***************************************************************************
//	DisplayUndo
//
//	Display the Undo Buffer
//***************************************************************************

void DisplayUndo
	(HWND hWnd)

{
	UINT	  uCharPos,
			  uLineCount;
	HGLOBAL   hGlbMem;
	LPWCHAR   lpwsz, p;
	HWND	  hWndParent;
	LPUNDOBUF lpUndoBeg,	// Ptr to start of Undo Buffer
			  lpUndoNxt;	// ...	  next available slot in the Undo Buffer
	static LPWCHAR Actions[]={L"None",
							  L"Ins",
							  L"Rep",
							  L"Del",
							  L"Sel",
							  L"Back",
							  L"InsToggle"};

	DbgMsg ("********** Undo Buffer *********************************");

	// Get the char position of the caret
	uCharPos = GetCurCharPos (hWnd);

	// Get the lines in the text
	uLineCount = SendMessageW (hWnd, EM_GETLINECOUNT, 0, 0);

	// Display it
	dprintf ("Caret position = %d, # lines = %d",
			 uCharPos,
			 uLineCount);

	// Get the Edit Control's memory handle
	(long) hGlbMem = SendMessageW (hWnd, EM_GETHANDLE, 0, 0);

	// Lock the memory to get a ptr to it
	lpwsz = MyGlobalLock (hGlbMem);

#define VIS_CR	L'\xAE'
#define VIS_NL	L'\xA9'
#define VIS_HT	L'\xBB'

	// Replace L'\r' with a visible char
	while (p = strchrW (lpwsz, L'\r'))
		*p = VIS_CR;

	// Replace L'\n' with a visible char
	while (p = strchrW (lpwsz, L'\n'))
		*p = VIS_NL;

	// Replace L'\t' with a visible char
	while (p = strchrW (lpwsz, L'\t'))
		*p = VIS_HT;

	// Display it
	dprintfW (L"Text = <%s>",
			  lpwsz);

	// Restore L'\t'
	while (p = strchrW (lpwsz, VIS_HT))
		*p = L'\t';

	// Restore L'\n'
	while (p = strchrW (lpwsz, VIS_NL))
		*p = L'\n';

	// Restore L'\r'
	while (p = strchrW (lpwsz, VIS_CR))
		*p = L'\r';

	// We no longer need this ptr
	MyGlobalUnlock (hGlbMem); lpwsz = NULL;

	// Get the parent window handle
	hWndParent = GetParent (hWnd);

	// Get the ptrs to the next available slot in our Undo Buffer
	(long) lpUndoNxt = GetWindowLong (hWndParent, GWLFE_UNDO_NXT);
	(long) lpUndoBeg = GetWindowLong (hWndParent, GWLFE_UNDO_BEG);

	// Loop through the undo buffer entries
	for (; lpUndoBeg < lpUndoNxt; lpUndoBeg++)
	{
		wsprintfW (lpwszTemp,
				   L"Act = %9s, %2d-%2d, Group = %3d, Char = 0x%04X",
				   Actions[lpUndoBeg->Action],
				   lpUndoBeg->CharPosBeg,
				   lpUndoBeg->CharPosEnd,
				   lpUndoBeg->Group,
				   lpUndoBeg->Char);
		DbgMsgW (lpwszTemp);
	} // End FOR

	DbgMsg ("********** End Undo Buffer *****************************");
} // End DisplayUndo
#endif


//***************************************************************************
//	End of File: display.c
//***************************************************************************
