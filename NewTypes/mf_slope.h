//***************************************************************************
//  NARS2000 -- Magic Function -- Slope
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2017 Sudley Place Software

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


//***************************************************************************
//  This file is encoded in UTF-8 format
//    DO NOT ATTEMPT TO EDIT IT WITH A NON-UNICODE EDITOR.
//***************************************************************************


//***************************************************************************
//  Magic function for Dyadic Slope
//
//  Dyadic Slope -- expand with or without axis
//
//  Return (1⌈|L)/[X] (L>0)\[X] R
//***************************************************************************

static LPAPLCHAR DydBody[] =
{// Emulate L\[X] R for Integer L
 L"Z←(1⌈|L)/[X] (L>0)\\[X] R",	// Double the \ to avoid it being confused with a character escape sequence
};

MAGIC_FCNOPR MFO_DydSlope =			
{L"Z←L " MFON_DydSlope L"[X] R",
 DydBody,
 countof (DydBody),
};


//***************************************************************************
//  End of File: mf_slope.h
//***************************************************************************