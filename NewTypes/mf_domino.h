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
//  Magic function/operator for Monadic Domino for Global Numerics
//
//  Return the pseudo-inverse of an array of global numerics.
//***************************************************************************

static APLCHAR MonHeader[] =
  L""S L"Z←" MFON_MonDomino L" "S L"R";

static APLCHAR MonLine1[] =
  L""S L"Z←(⌽⍴"S L"R)⍴(⌹(⍉"S L"Z)+.×"S L"Z)+.×⍉"S L"Z←⍪"S L"R";

static LPAPLCHAR MonBody[] =
{MonLine1,
};

MAGIC_FCNOPR MFO_MonDomino =
{MonHeader,
 MonBody,
 countof (MonBody),
};


//***************************************************************************
//  Magic function/operator for Dyadic Domino for Global Numerics
//
//  Return the pseudo-inverse of an array of global numerics.
//***************************************************************************

static APLCHAR DydHeader[] =
  L""S L"Z←"S L"L " MFON_DydDomino L" "S L"R";

static APLCHAR DydLine1[] =
  L""S L"Z←(⌹"S L"R)+.×"S L"L";

static LPAPLCHAR DydBody[] =
{DydLine1,
};

MAGIC_FCNOPR MFO_DydDomino =
{DydHeader,
 DydBody,
 countof (DydBody),
};

#undef  S


//***************************************************************************
//  End of File: mf_domino.h
//***************************************************************************