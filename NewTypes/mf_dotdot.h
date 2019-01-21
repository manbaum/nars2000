//***************************************************************************
//  NARS2000 -- Magic Function -- DotDot
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
//  Magic function/operator for Extended Dyadic DotDot
//
//  Extended Dyadic DotDot -- Sequence Generator For Nested Arrays
//
//  This algorithm is a generalization of 2..5 proposed by John Scholes.
//***************************************************************************

static LPAPLCHAR DydBody[] =
{L"⎕IO←0 ⋄ ⎕CT←3E¯15",
 L""S L"L←2↑"S L"L,1",
 L""S L"Z←((⊃"S L"R)-0⊃"S L"L)÷|1⊃"S L"L",
 L""S L"L[1]←⊂(|1⊃"S L"L)××"S L"Z",
 L""S L"Z←"S L"L[0]+"S L"L[1]×⍳1+⊃⌊|"S L"Z",
};

MAGIC_FCNOPR MFO_DydDotDot =
{L""S L"Z←"S L"L " MFON_DydDotDot L" "S L"R;⎕IO ⎕CT",
 DydBody,
 countof (DydBody),
};

#undef  S


//***************************************************************************
//  End of File: mf_dotdot.h
//***************************************************************************