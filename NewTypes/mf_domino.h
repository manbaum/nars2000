//***************************************************************************
//  NARS2000 -- Magic Function -- Domino
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
//  Magic function/operator for Monadic Domino for Overdetermined Global Numerics
//
//  Return the pseudo-inverse of an array of Overdetermined global numerics.
//    by Berthold K. P. Horn:
//
//    http://people.csail.mit.edu/bkph/articles/Pseudo_Inverse.pdf
//***************************************************************************

static LPAPLCHAR MonBody[] =
{
  L""S L"Z←(⌽⍴"S L"R)⍴(⌹(⍉"S L"Z)+.×"S L"Z)+.×⍉"S L"Z←⍪"S L"R",
};

MAGIC_FCNOPR MFO_MonDomino =
{
  L""S L"Z←" MFON_MonDomino L" "S L"R",
  MonBody,
  countof (MonBody),
};


//***************************************************************************
//  Magic function/operator for Dyadic Domino for Overdetermined Global Numerics
//
//  Return the pseudo-inverse of an array of Overdetermined global numerics.
//***************************************************************************

static LPAPLCHAR DydBody[] =
{
  L""S L"Z←(⌹"S L"R)+.×"S L"L",
};

MAGIC_FCNOPR MFO_DydDomino =
{
  L""S L"Z←"S L"L " MFON_DydDomino L" "S L"R",
  DydBody,
  countof (DydBody),
};


//***************************************************************************
//  Magic function/operator for Monadic Domino for Underdetermined matrices
//
//  Return the pseudo-inverse of an array of Underdetermined matrices.
//    by Berthold K. P. Horn:
//
//    http://people.csail.mit.edu/bkph/articles/Pseudo_Inverse.pdf
//***************************************************************************

static LPAPLCHAR MonBody2[] =
{
  L""S L"Z←(⍉"S L"R)+.×⌹"S L"R+.×⍉"S L"R",
};

MAGIC_FCNOPR MFO_MonDomino2 =
{
  L""S L"Z←" MFON_MonDomino2 L" "S L"R",
  MonBody2,
  countof (MonBody2),
};


//***************************************************************************
//  Magic function/operator for Dyadic Domino for Underdetermined matrices
//
//  Return the pseudo-inverse of an array of Underdetermined matrices
//***************************************************************************

MAGIC_FCNOPR MFO_DydDomino2 =
{
  L""S L"Z←"S L"L " MFON_DydDomino2 L" "S L"R",
  DydBody,
  countof (DydBody),
};

#undef  S


//***************************************************************************
//  End of File: mf_domino.h
//***************************************************************************