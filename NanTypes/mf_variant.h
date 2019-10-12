//***************************************************************************
//  NARS2000 -- Magic Function -- Variant Operator
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
//  Magic function/operator for rising/falling factorials
//***************************************************************************

static LPAPLCHAR DydVOFactBody[] =
{L"⎕PRO:",
 L"⎕IO←0",
 L"→(2=≢"S L"X)/"S L"L0 ⋄ "S L"X←"S L"X,1",
 L""S L"L0:→(" S L"X[0]≠0)/"S L"L1 ⋄ "S L"X[0]←(-×"S L"R)×⌈|"S L"R÷"S L"X[1]",
 L""S L"L1:"S L"Z←×/¨"S L"R+((×"S L"X[0])×|"S L"X[1])×⊂⍳|"S L"X[0]",
};

MAGIC_FCNOPR MFO_DydVOFact =
{L""S L"Z←" MFON_DydVOFact L"["S L"X] "S L"R;⎕IO",
 DydVOFactBody,
 countof (DydVOFactBody),
};


//***************************************************************************
//  Magic function/operator for Execute with local ⎕FPC
//***************************************************************************

static LPAPLCHAR MonExecuteBody[] =
{L"⎕PRO:",
 L"⎕FPC←"S L"L",
 L""S L"Z←##.⍎"S L"R",
};

MAGIC_FCNOPR MFO_MonExecute =
{L""S L"Z←"S L"L " MFON_MonExecute L" "S L"R;⎕FPC",
 MonExecuteBody,
 countof (MonExecuteBody),
};

#undef  S


//***************************************************************************
//  End of File: mf_variant.h
//***************************************************************************