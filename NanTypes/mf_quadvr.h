//***************************************************************************
//  NARS2000 -- Magic Function -- Quad VR
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
//  Magic function/operator for Monadic []VR
//
//  Monadic []VR -- Visual Representation
//
//  Return a visual representation of a user-defined function/operator
//***************************************************************************

static LPAPLCHAR MonBodyVR[] =
{L"⎕IO←0 ⋄ "S L"Z←1 ##.⎕CR "S L"R",
 L"→(0≠⍴"S L"Z)/"S L"L1 ⋄ "S L"Z←'' ⋄ →0",
 L""S L"L1:→(1=≡"S L"Z)/0",
 L""S L"L←'∇'∊0⊃"S L"Z",
 L""S L"Z←(∊((⊂[1]'LO<    ∇>P<[>Q<]>I6' ⎕FMT⍳⍴"S L"Z),¨"S L"Z),¨⊂⎕TCNL ⎕TCLF),'    ∇'",
 L"→"S L"L/0 ⋄ "S L"Z←"S L"Z,(' ',⍕2 ##.⎕AT "S L"R),' (UTC)'",
};

MAGIC_FCNOPR MFO_MonVR =
{L""S L"Z←" MFON_MonVR L" "S L"R;"S L"L ⎕IO",
 MonBodyVR,
 countof (MonBodyVR),
};

#undef  S


//***************************************************************************
//  End of File: mf_quadvr.h
//***************************************************************************