//***************************************************************************
//  NARS2000 -- Magic Function -- DieresisDownTack
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
//  Magic function/operator for identity function from the
//    Convolution operator
//***************************************************************************

static LPAPLCHAR IdnBody[] =
{L"⎕ID:Z←"S L"LO/"S L"RO/⍬",
};

MAGIC_FCNOPR MFO_IdnConv =
{L""S L"Z←("S L"LO " MFON_IdnConv L" "S L"RO) "S L"R",
 IdnBody,
 countof (IdnBody),
};


//***************************************************************************
//  Magic function/operator for dyadic derived function from the convolution dyadic operator
//
//  This operator is based upon design from the original NARS system.
//***************************************************************************

//  ⍝ Convolution of LO and RO between L and R
static LPAPLCHAR DydBody[] =
{L""S L"L←1/"S L"L ⋄ "S L"R←1/"S L"R ⋄ "S L"Z←⍬",
 L"⎕ERROR ((1≠≢⍴"S L"L)∨1≠≢⍴"S L"R)/'RANK ERROR'",
 L""S L"LN←≢"S L"L ⋄ "S L"RN←≢"S L"R",
 //  ⍝ Short leading
 L":if 1<"S L"LN⌊"S L"RN",
 L"  :forlcl "S L"I :in 1..¯1+"S L"LN⌊"S L"RN",
 L"    "S L"Z←"S L"Z,("S L"I↑"S L"L) "S L"LO."S L"RO ⌽"S L"I↑"S L"R",
 L"  :end",
 L":end",
 //  ⍝ Full middle
 L"  :forlcl "S L"I :in 0..|"S L"LN-"S L"RN",
 L"    :if "S L"LN≥"S L"RN",
 L"      "S L"Z←"S L"Z,("S L"RN↑"S L"I↓"S L"L) "S L"LO."S L"RO ⌽"S L"R",
 L"    :else",
 L"      "S L"Z←"S L"Z,"S L"L "S L"LO."S L"RO ⌽"S L"LN↑"S L"I↓"S L"R",
 L"    :end",
 L"  :end",
 //  ⍝ Short trailing
 L":if 1<"S L"LN⌊"S L"RN",
 L"  :forlcl "S L"I :in -⌽1..¯1+"S L"LN⌊"S L"RN",
 L"    "S L"Z←"S L"Z,("S L"I↑"S L"L) "S L"LO."S L"RO ⌽"S L"I↑"S L"R",
 L"  :end",
 L":end",
};

MAGIC_FCNOPR MFO_DydConv =
{L""S L"Z←"S L"L ("S L"LO " MFON_DydConv L" "S L"RO) "S L"R;"S L"LN "S L"RN",
 DydBody,
 countof (DydBody),
};

#undef  S


//***************************************************************************
//  End of File: mf_didtack.h
//***************************************************************************