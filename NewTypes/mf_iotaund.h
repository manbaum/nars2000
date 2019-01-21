//***************************************************************************
//  NARS2000 -- Magic Function -- IotaUnderbar
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
//  Magic function/operator for Extended Dyadic Iota Underbar
//
//  Extended Dyadic Iota Underbar -- Matrix Iota
//
//  Return an array of indices from searching for the
//    array  of trailing subarrays from the right arg in the
//    vector of trailing subarrays from the left arg.
//***************************************************************************

static LPAPLCHAR DydBody[] =
{L""S L"Z←(⊂⍤¯1 "S L"L)⍳⊂⍤(¯1+⍴⍴"S L"L) "S L"R ⋄ →0",
 L"⎕MS:"S L"Z←(⊂⍤¯1 "S L"L)⍳⊂⍤(¯1+⍴⍴"S L"L) "S L"R",
};

MAGIC_FCNOPR MFO_DydIotaUnderbar =
{L""S L"Z←"S L"L " MFON_DydIotaUnderbar L" "S L"R",
 DydBody,
 countof (DydBody),
};

#undef  S
  

//***************************************************************************
//  End of File: mf_iotaund.h
//***************************************************************************