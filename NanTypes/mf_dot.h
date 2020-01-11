//***************************************************************************
//  NARS2000 -- Magic Function -- Dot
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2020 Sudley Place Software

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
//    inner product operator
//***************************************************************************

static LPAPLCHAR IdnBody[] =
{L"⎕ID:"S L"Z←∘⌻((¯1*⎕LR='r')↑⍴1/"S L"R)⍴1",
};

MAGIC_FCNOPR MFO_IdnDot =
{L""S L"Z←" MFON_IdnDot L" "S L"R",
 IdnBody,
 countof (IdnBody),
};


//***************************************************************************
//  Magic function/operator for APL2 definition of inner product.
//***************************************************************************

static LPAPLCHAR DydBody[] =
{L":if 0=⍴⍴"S L"L ⋄ "S L"L←⊂"S L"L ⋄ :else ⋄ "S L"L←⊂[(⎕IO-1)+⍴⍴"S L"L] "S L"L ⋄ :end",
 L":if 0=⍴⍴"S L"R ⋄ "S L"R←⊂"S L"R ⋄ :else ⋄ "S L"R←⊂[ ⎕IO       ] "S L"R ⋄ :end",
 L""S L"Z←"S L"LO/¨"S L"L∘."S L"RO "S L"R",
};

MAGIC_FCNOPR MFO_DydDot2 =
{L""S L"Z←"S L"L ("S L"LO " MFON_DydDot2 L" "S L"RO) "S L"R",
 DydBody,
 countof (DydBody),
};




//***************************************************************************
//  Magic function/operator for initialization of the determinant operator's
//    subroutines.
//
//	This algorithm was taken from SATN-42 (Sharp APL Technical Notes),
//	  1982-04-01, "Determinant-Like Functions Produced by the Dot Operator",
//	  by K.E. Iverson (http://www.jsoftware.com/papers/satn42.htm),
//	  with minor changes.
//***************************************************************************

// The call to ⎕EC traps errors which occur when reducing an empty or singleton column
//   if the left operand has no identity element.  For example, if ⍺⍺ is an AFO
//   such as {⍺-⍵} and the last coordinate of ⍵ is 0 or 1, then ⍺⍺/⍵ signals an
//   error, because ⍺⍺ has no identity element.  Instead, we need the ability to  
//   specify an identity element for this AFO such as {⍺-⍵ ⋄ ⎕ID:0}.
static LPAPLCHAR IniBody[] =
{MFON_MonDotAll L"←{⎕IO←1 ⋄ y←" MFON_MonDotPerm L"⍴⍵ ⋄ k←⍴⍴y ⋄ ((⍳k),k)⍉⍵[y;]}",
 MFON_MonDotPerm L"←{s←(⌊/⍵)↑⌽⍳1↑⍵ ⋄ (s,⍴s)⍴110 1‼⍴s}",
 MFON_MonDotCr L"←{0=⍴⍴⍵:⍵ ⋄ 0=1⍴⎕EC '⍺⍺/⍵':∇(¯1↓⍴⍵)⍴⍵ ⋄ ∇ ⍺⍺/⍵}",
 MFON_MonExecute L"←{⎕FPC←⍺ ⋄ ##.⍎⍵}",
};

MAGIC_FCNOPR MFO_AfoMfoInit =
{MFON_AfoMfoInit,
 IniBody,
 countof (IniBody),
};

//***************************************************************************
//  Magic function/operator for determinant operator from the
//    inner product operator.
//
//	This algorithm was taken from SATN-42 (Sharp APL Technical Notes),
//	  1982-04-01, "Determinant-Like Functions Produced by the Dot Operator",
//	  by K.E. Iverson (http://www.jsoftware.com/papers/satn42.htm), 
//	  with minor changes.
//***************************************************************************

//L"⍝ Generalized determinant operator";

static LPAPLCHAR MonBody[] =
{L""S L"Z←⊃"S L"LO " MFON_MonDotCr L" "S L"RO/" MFON_MonDotAll L"⍪"S L"R ⋄ →0",
 L"⎕PRO:"S L"Z←⊃"S L"LO/⍬",
};

MAGIC_FCNOPR MFO_MonDot =
{L""S L"Z←("S L"LO " MFON_MonDot L" "S L"RO) "S L"R",
 MonBody,
 countof (MonBody),
};


//***************************************************************************
//  Magic function/operator for determinant operator from the
//    inner product operator.
//
//	This algorithm is used to determine the result of the the determinant
//    operator on a singleton scalar/vector/matrix.
//***************************************************************************

static LPAPLCHAR DetSingBody[] =
{L""S L"Z←⊃"S L"LO/⊃"S L"RO/"S L"R",
};

MAGIC_FCNOPR MFO_DetSing =
{L""S L"Z←("S L"LO " MFON_DetSing L" "S L"RO) "S L"R",
 DetSingBody,
 countof (DetSingBody),
};


//***************************************************************************
//  Magic function/operator for determinant operator from the
//    inner product operator.
//
//	Calculate the permanent of R (a.k.a. +.×R)
//    using H.J. Ryser's algorithm in Combinatorial Mathematics (1963),
//    #14 of the Carus Mathematical Monographs, published by the
//    Mathematical Association of America, ISBN 0-88385-014-1, p. 26-27.
//***************************************************************************

static LPAPLCHAR DetPermBody[] =
{L"⎕IO←0 ⋄ ("S L"m "S L"n)←⍴"S L"R",
 L":if "S L"m>"S L"n ⋄ "S L"Z←+/∇¨⊂[1 2] "S L"R[10 1‼"S L"n "S L"m;]",
 L":else",
 L"  "S L"B←("S L"n⍴2)⊤(¯1+2*"S L"n)..0",
 L"  "S L"S←"S L"n-+⌿"S L"B",
 L"  "S L"SS←("S L"n+1)⍴0 ⋄ "S L"SS["S L"S]+←×/¨+/¨(⊂[0] "S L"B)/¨⊂"S L"R",
 L"  "S L"I←⍳"S L"m ⋄ "S L"J←"S L"I!"S L"I+"S L"n-"S L"m",
 L"  "S L"Z←-/"S L"J×"S L"SS[("S L"n-"S L"m)+⍳"S L"m]",
 L":endif",
};

MAGIC_FCNOPR MFO_DetPerm =
{L""S L"Z←" MFON_DetPerm L" "S L"R;"S L"m "S L"n "S L"B "S L"S ⎕IO "S L"I "S L"J "S L"SS",
 DetPermBody,
 countof (DetPermBody),
};

#undef  S


//***************************************************************************
//  End of File: mf_dot.h
//***************************************************************************