//***************************************************************************
//  NARS2000 -- Magic Function -- DownShoe
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
//  Magic function for dyadic DownShoe
//
//  Dyadic Down Shoe -- Union
//
//  Return the elements in L or in R.
//***************************************************************************

static LPAPLCHAR DydBody[] =
{L""S L"Z←"S L"L,"S L"R~"S L"L",
};

MAGIC_FCNOPR MFO_DydDnShoe =
{L""S L"Z←"S L"L " MFON_DydDnShoe L" "S L"R",
 DydBody,
 countof (DydBody),
};

#undef  S


//***************************************************************************
//  End of File: mf_dshoe.h
//***************************************************************************