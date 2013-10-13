//***************************************************************************
//  NARS2000 -- Magic Function -- Dot
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2013 Sudley Place Software

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
//    inner product operator
//***************************************************************************

static APLCHAR IdnHeader[] =
  L"Z←" MFON_IdnDot L" R";

static APLCHAR IdnLine1[] = 
  L"⎕ID:Z←∘.=⍨⍳¯1↑1,⍴R";

static LPAPLCHAR IdnBody[] =
{IdnLine1,
};

MAGIC_FCNOPR MFO_IdnDot =
{IdnHeader,
 IdnBody,
 countof (IdnBody),
};

//***************************************************************************
//  Magic function/operator for determinant function from the
//    inner product operator
//***************************************************************************

static APLCHAR MonHeader[] =
  L"Z←(LO " MFON_MonDot L" RO) R";

//L"⍝ Generalized determinant operator";

static APLCHAR MonLine1[] = 
  L"⎕ERROR (2≠⍴⍴R)/'RANK ERROR'";

static APLCHAR MonLine2[] = 
  L"⎕ERROR (≠/⍴R)/'LENGTH ERROR'";

static APLCHAR MonLine3[] = 
  L":if ∨/0 1∊⍴R ⋄ Z←⍬⍴R ⋄ →0 ⋄ :end";

static APLCHAR MonLine4[] = 
  L"Z←⊃LO/((1⍴⍴R)⍴R) RO¨∇¨(⊂[1+⎕IO] (⍴R)⍴~(1+1⍴⍴R)↑1)/¨⊂1 0↓R";

static APLCHAR MonLine5[] = 
  L"→0";

static APLCHAR MonLine6[] =
  L"⎕PRO:Z←⊃LO/⍬";

static LPAPLCHAR MonBody[] =
{MonLine1,
 MonLine2,
 MonLine3,
 MonLine4,
 MonLine5,
 MonLine6,
};

MAGIC_FCNOPR MFO_MonDot =
{MonHeader,
 MonBody,
 countof (MonBody),
};


//***************************************************************************
//  End of File: mf_dot.h
//***************************************************************************