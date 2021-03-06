﻿//***************************************************************************
//  NARS2000 -- Magic Function -- JotDot
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2016 Sudley Place Software

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
//  Magic function/operator for identity function from the
//    outer product operator
//   f/0{rho} R.
//***************************************************************************

static APLCHAR IdnHeader[] =
  L"Z←(LO " MFON_IdnJotDot L") R";

static APLCHAR IdnLine1[] = 
  L"⎕ID:Z←LO/0⍴R";

static LPAPLCHAR IdnBody[] =
{IdnLine1,
};

MAGIC_FCNOPR MFO_IdnJotDot =
{IdnHeader,
 IdnBody,
 countof (IdnBody),
};


//***************************************************************************
//  End of File: mf_jotdot.h
//***************************************************************************