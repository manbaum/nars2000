//***************************************************************************
//  NARS2000 -- Magic Function -- Iota
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
//  Magic function/operator for Extended Monadic Iota
//
//  Extended Monadic Iota -- Index Generator For Arrays
//
//  Return an array of indices appropriate to an array of
//    the same shape as the right arg.
//
//  I believe this algorithm was first created by Carl M. Cheney.
//***************************************************************************

static LPAPLCHAR MonBody[] =
{L""S L"Z←⊃∘.,/⍳¨"S L"R",
};

MAGIC_FCNOPR MFO_MonIota =
{L""S L"Z←" MFON_MonIota L" "S L"R",
 MonBody,
 countof (MonBody),
};

  
//***************************************************************************
//  Magic function/operator for extended dyadic iota
//
//  Extended dyadic iota
//
//  On rank NE 1 left args, return an array of vector indices
//    such that A[A iota R] is R, assuming that all of R is in A.
//***************************************************************************

static LPAPLCHAR DydBody[] =
{L""S L"O←⎕IO ⋄ ⎕IO←0",
 L":if 0=⍴⍴"S L"L ⋄ Z←(⍴"S L"R)⍴⊂⍬",
 L":else",
 L"  "S L"Z←(0,⍴"S L"L)⊤(,"S L"L)⍳"S L"R",
 L"  "S L"A←⍸1=0⌷[0] "S L"Z",
 L"  "S L"Z[(⍳1+⍴⍴"S L"L)∘.,"S L"A]←⊃[0] (⍴"S L"A)⍴⊂0,⍴"S L"L",
 L"  "S L"Z←⊂[0] "S L"O+1↓[0] "S L"Z",
 L":endif",
};

MAGIC_FCNOPR MFO_DydIota =
{L""S L"Z←"S L"L " MFON_DydIota L" "S L"R;⎕IO "S L"O "S L"A",
 DydBody,
 countof (DydBody),
};

#undef  S
  

//***************************************************************************
//  End of File: mf_iota.h
//***************************************************************************