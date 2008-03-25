//***************************************************************************
//	NARS2000 -- Primitive Functions Header
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

// Common TYPEDEF used to call all primitive functions
typedef struct tagPL_YYSTYPE *(*LPPRIMFNS) (struct tagTOKEN *,
											struct tagTOKEN *,
											struct tagTOKEN *,
											struct tagTOKEN *);

// Common TYPEDEF used to call all primitive operators
typedef struct tagPL_YYSTYPE *(*LPPRIMOPS) (struct tagTOKEN *,
											struct tagPL_YYSTYPE *,
											struct tagTOKEN *,
											struct tagTOKEN *);


//***************************************************************************
//	End of File: primfns.h
//***************************************************************************
