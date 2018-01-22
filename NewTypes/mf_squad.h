//***************************************************************************
//  NARS2000 -- Magic Function -- Squad
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2018 Sudley Place Software

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
//  Magic function for Dyadic Squad
//
//  Dyadic Squad -- index with or without axis
//
//  Return R[⊃∘.,/L]  or  R[⊃∘.,/1/¨¨L]
//***************************************************************************

static LPAPLCHAR DydBody[] =
{// Emulate L⌷[X] R or L⌷R
 L"Pro←0",
 L"L1::if 0≠⎕NC 'X'",
 L"  ⎕ERROR (~∧/X∊⍳⍴⍴R)/'AXIS ERROR'",
 L"  ⎕ERROR ((≢X)≢≢∪X)/'AXIS ERROR'",
 L"  A←((⍳⍴⍴R)∊X)\\L", // Double the \ to avoid it being confused with a character escape sequence
 L"  B←(⍳⍴⍴R)~X",
 L"  A[B]←⍳¨(⍴1/R)[B]",
 L":else",
 L"  A←L",
 L":end",
 
 L"⎕ERROR((⍴1/A)≢⍴⍴R)/'RANK ERROR'",
 
 // If ∘.,/A would fail, ...
 L":if 0=≡↑↑A",
 L"  fn←{⊃∘.,/1/¨¨⍵}",
 L":else",
 L"  fn←{⊃∘.,/⍵}",
 L":end",

 L":if Pro ⋄ Z←(⍴fn A)⍴R",
 L":else   ⋄ Z←R[fn A]",
 L":end    ⋄ →0",
 
 L"⎕PRO:Pro←1",
 L"→L1",
};

MAGIC_FCNOPR MFO_DydSquad =
{L"Z←L " MFON_DydSquad L"[X] R;A B Pro fn",
 DydBody,
 countof (DydBody),
};


//***************************************************************************
//  End of File: mf_squad.h
//***************************************************************************