//***************************************************************************
//  NARS2000 -- Magic Function -- Dot
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
//  Magic function/operator for identity function from the
//    inner product operator
//***************************************************************************

static LPAPLCHAR IdnBody[] =
{L"⎕ID:Z←∘.=⍨⍳¯1↑1,⍴R",
};

MAGIC_FCNOPR MFO_IdnDot =
{L"Z←" MFON_IdnDot L" R",
 IdnBody,
 countof (IdnBody),
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
};

MAGIC_FCNOPR MFO_MonDotInit =
{MFON_MonDotInit,
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
{L"Z←⊃LO " MFON_MonDotCr L" RO/" MFON_MonDotAll L"⍪R ⋄ →0",
 L"⎕PRO:Z←⊃LO/⍬",
};

MAGIC_FCNOPR MFO_MonDot =
{L"Z←(LO " MFON_MonDot L" RO) R",
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
{L"Z←⊃LO/⊃RO/R",
};

MAGIC_FCNOPR MFO_DetSing =
{L"Z←(LO " MFON_DetSing L" RO) R",
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
{L"⎕IO←0 ⋄ (m n)←⍴R",
 L":if m>n ⋄ Z←+/∇¨⊂[1 2] R[10 1‼n m;]",
 L":else",
 L"  B←(n⍴2)⊤(¯1+2*n)..0",
 L"  S←n-+⌿B",
 L"  SS←(n+1)⍴0 ⋄ SS[S]+←×/¨+/¨(⊂[0] B)/¨⊂R",
 L"  I←⍳m ⋄ J←I!I+n-m",
 L"  Z←-/J×SS[(n-m)+⍳m]",
 L":endif",
};

MAGIC_FCNOPR MFO_DetPerm =
{L"Z←" MFON_DetPerm L" R;m n B S ⎕IO I J SS",
 DetPermBody,
 countof (DetPermBody),
};


//***************************************************************************
//  End of File: mf_dot.h
//***************************************************************************