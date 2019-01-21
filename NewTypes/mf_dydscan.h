//***************************************************************************
//  NARS2000 -- Magic Function -- Dyadic Scan
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
//  Magic function/operator for dyadic derived function from the scan monadic operator
//***************************************************************************

static LPAPLCHAR DydBody[] =
{L":if 0=⎕NC '"S L"X'",
 L"  "S L"Z←"S L"L "S L"LO/(((¯1↓⍴"S L"R),0⌈(|"S L"L)-1)⍴"S L"LO/0⍴"S L"R),"S L"R",
 L":else",
 L"  "S L"Z←"S L"L "S L"LO/["S L"X](((("S L"X≠⍳⍴⍴"S L"R)/⍴"S L"R),0⌈(|"S L"L)-1)[⍋⍋"S L"X=⍳⍴⍴"S L"R]⍴"S L"LO/0⍴"S L"R),["S L"X]"S L"R",
 L":endif",
};

MAGIC_FCNOPR MFO_DydScan =
{L""S L"Z←"S L"L ("S L"LO " MFON_DydScan L"["S L"X]) "S L"R",
 DydBody,
 countof (DydBody),
};


//***************************************************************************
//  Magic function/operator for dyadic derived function from the scan monadic operator
//***************************************************************************

static LPAPLCHAR Dyd1Body[] =
{L":if 0=⎕NC '"S L"X'",
 L"  "S L"Z←"S L"L "S L"LO⌿(((0⌈(|"S L"L)-1),1↓⍴"S L"R)⍴"S L"LO/0⍴"S L"R)⍪"S L"R",
 L":else",
 L"  "S L"Z←"S L"L "S L"LO/["S L"X](((("S L"X≠⍳⍴⍴"S L"R)/⍴"S L"R),0⌈(|"S L"L)-1)[⍋⍋"S L"X=⍳⍴⍴"S L"R]⍴"S L"LO/0⍴"S L"R),["S L"X]"S L"R",
 L":endif",
};

MAGIC_FCNOPR MFO_DydScan1 =
{L""S L"Z←"S L"L ("S L"LO " MFON_DydScan1 L"["S L"X]) "S L"R",
 Dyd1Body,
 countof (Dyd1Body),
};

#undef  S


//***************************************************************************
//  End of File: mf_dydscan.h
//***************************************************************************