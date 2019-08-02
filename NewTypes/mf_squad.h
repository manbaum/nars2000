//***************************************************************************
//  NARS2000 -- Magic Function -- Squad
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2019 Sudley Place Software

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

#define S   WS_MFOLCL

//***************************************************************************
//  Magic function for Dyadic Squad
//
//  Dyadic Squad -- index with or without axis
//
//  Return R[⊃∘.,/L]  or  R[⊃∘.,/1/¨¨L]
//***************************************************************************

static LPAPLCHAR DydBody[] =
{// Emulate L⌷[X] R or L⌷R
 L""S L"Pro←0",
 L""S L"L1:"
 L"⎕ERROR (1<⍴⍴"S L"L)/'RANK ERROR'",
 L":if 0=⎕NC '"S L"X'",
 L":andif (≢"S L"L)<⍴⍴"S L"R",        // This feature was designed and implemented by Dyalog
 L"  "S L"X←⍳≢"S L"L",
 L":end",
 L":if 0≠⎕NC '"S L"X'",
 L"  ⎕ERROR (1<⍴⍴"S L"X)/'AXIS ERROR'",
 L"  ⎕ERROR ((≢"S L"L)≢≢"S L"X)/'LENGTH ERROR'",
 L"  ⎕ERROR (~∧/"S L"X∊⍳⍴⍴"S L"R)/'AXIS ERROR'",
 L"  ⎕ERROR ((≢"S L"X)≢≢∪"S L"X)/'AXIS ERROR'",
 L"  "S L"A←((⍳⍴⍴"S L"R)∊"S L"X)\\(1/"S L"L)[⍋"S L"X]",  // Double the \ to avoid it being confused with a character escape sequence
 L"  "S L"B←(⍳⍴⍴"S L"R)~"S L"X",
 L"  "S L"A["S L"B]←⍳¨(⍴1/"S L"R)["S L"B]",
 L":else",
 L"  "S L"A←"S L"L",
 L":end",
 
 L"⎕ERROR((⍴1/"S L"A)≢⍴⍴"S L"R)/'RANK ERROR'",
 
 // If ∘.,/A would fail, ...
 L":if 0=≡↑↑"S L"A",
 L"  "S L"fn←{⊃∘.,/1/¨¨⍵}",
 L":else",
 L"  "S L"fn←{⊃∘.,/⍵}",
 L":end",

 L":if "S L"Pro ⋄ "S L"Z←(⍴"S L"fn "S L"A)⍴"S L"R",
 L":else   ⋄ "S L"Z←"S L"R["S L"fn "S L"A]",
 L":end    ⋄ →0",
 
 L"⎕PRO:"S L"Pro←1",
 L"→"S L"L1",
};

MAGIC_FCNOPR MFO_DydSquad =
{L""S L"Z←"S L"L " MFON_DydSquad L"["S L"X] "S L"R;"S L"A "S L"B "S L"Pro "S L"fn",
 DydBody,
 countof (DydBody),
};

#undef  S


//***************************************************************************
//  End of File: mf_squad.h
//***************************************************************************