//***************************************************************************
//	NARS2000 -- Save Function Header
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

#define LPFX_SAVE2	struct tagFX_SAVE *

typedef struct tagFX_SAVE
{
	HGLOBAL    hGlbRht; 			// 00:	Right arg global memory handle
	APLRANK    aplRankRht;			// 04:	Right arg rank
	APLLONGEST aplLongestRht;		// 0C:	Right arg immediate value
	APLDIM	   aplRowsRht,			// 14:	Right arg # rows
			   aplColsRht;			// 1C:	... 		cols
	BOOL	   bRet;				// 24:	TRUE iff result is valid
	UINT	   uErrLine;			// 28:	If (!bRet), the line in error (origin-0) (NEG1U = Error Message valid)
	LPSYMENTRY lpSymName;			// 2C:	If (bRet), ptr to SYMENTRY of the function name
	LPTOKEN    lptkFunc;			// 30:	Ptr to function token ([]FX only)
	UINT (*SF_LineLen) (HWND, LPFX_SAVE2, UINT);				// 34:	Ptr to get line len function
	void (*SF_ReadLine) (HWND, LPFX_SAVE2, UINT, LPAPLCHAR);	// 38:	Ptr to read line function
	UINT (*SF_NumLines) (HWND, LPFX_SAVE2); 					// 3C:	Ptr to get # function lines function
} FX_SAVE, *LPFX_SAVE;


//***************************************************************************
//	End of File: savefcn.h
//***************************************************************************
