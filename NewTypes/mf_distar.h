//***************************************************************************
//  NARS2000 -- Magic Function -- DieresisStar
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
//  Magic function/operator for monadic derived function from the Power
//    dyadic operator for Invewrse +∘÷/ (Continued Fractions)
//***************************************************************************

static LPAPLCHAR InvPJDRedBody[] =
{L":if 14=⎕DR "S L"R ⋄ "S L"L←∞",
 L":elseif 0=⎕NC '"S L"L' ⋄ "S L"L←⎕PP ⋄ :end",
 S L"Z←1/"S L"a←⌊"S L"b←"S L"R",
 S L"L1:"S L"a←"S L"b-"S L"a",
 L"→(("S L"a=0)∨"S L"L≤⍴"S L"Z)/0",
 S L"b←÷"S L"a",
 S L"Z←"S L"Z,"S L"a←⌊"S L"b",
 L"→"S L"L1",
};

MAGIC_FCNOPR MFO_InvPJDRed =
{S L"Z←{"S L"L} " MFON_InvPJDRed L" "S L"R;"S L"a "S L"b",
 InvPJDRedBody,
 countof (InvPJDRedBody),
};


//***************************************************************************
//  Magic function/operator for monadic derived function from the Power
//    dyadic operator for Inverse Base Value
//***************************************************************************

static LPAPLCHAR InvBVBody[] =
{S L"L←9○"S L"L",
 S L"Z←((1+⌊"S L"L⍟⌈/|,"S L"R)⍴"S L"L)⊤"S L"R",
};

MAGIC_FCNOPR MFO_InvBV =
{S L"Z←"S L"L " MFON_InvBV L" "S L"R",
 InvBVBody,
 countof (InvBVBody),
};

#undef  S


//***************************************************************************
//  End of File: mf_distar.h
//***************************************************************************