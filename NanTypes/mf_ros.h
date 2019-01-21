//***************************************************************************
//  NARS2000 -- Magic Function -- Reduction Of Singletons
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
//  Magic function/operator for Reduction of Singletons,
//    type #1, left identity element
//***************************************************************************

static LPAPLCHAR RoS1LBody[] =
{L"⎕PRO:",
 L":if "S L"L ⋄ "S L"R←⊂"S L"R ⋄ :endif",
 L""S L"Z←("S L"LO/0⍴"S L"R) "S L"LO¨(("S L"X≠⍳⍴⍴"S L"R)/⍴"S L"R)⍴"S L"R",
 L":if "S L"L ⋄ "S L"Z←⊃"S L"Z ⋄ :endif",
};

MAGIC_FCNOPR MFO_RoS1L =
{L""S L"Z←"S L"L ("S L"LO " MFON_RoS1L L"["S L"X]) "S L"R",
 RoS1LBody,
 countof (RoS1LBody),
};


//***************************************************************************
//  Magic function/operator for Reduction of Singletons,
//    type #1, right identity element
//***************************************************************************

static LPAPLCHAR RoS1RBody[] =
{L"⎕PRO:",
 L":if "S L"L ⋄ "S L"R←⊂"S L"R ⋄ :endif",
 L""S L"Z←((("S L"X≠⍳⍴⍴"S L"R)/⍴"S L"R)⍴"S L"R) "S L"LO¨"S L"LO/0⍴"S L"R",
 L":if "S L"L ⋄ "S L"Z←⊃"S L"Z ⋄ :endif",
};

MAGIC_FCNOPR MFO_RoS1R =
{L""S L"Z←"S L"L ("S L"LO " MFON_RoS1R L"["S L"X]) "S L"R",
 RoS1RBody,
 countof (RoS1RBody),
};


//***************************************************************************
//  Magic function/operator for Reduction of Singletons, type #2
//***************************************************************************

static LPAPLCHAR RoS2Body[] =
{L"⎕PRO:"S L"Z←⊃¨"S L"LO/¨⊂¨"S L"R",
};

MAGIC_FCNOPR MFO_RoS2 =
{L""S L"Z←("S L"LO " MFON_RoS2 L") "S L"R",
 RoS2Body,
 countof (RoS2Body),
};


//***************************************************************************
//  Magic function/operator for Reduction of Singletons, type #3
//***************************************************************************

static LPAPLCHAR RoS3Body[] =
{L"⎕PRO:"S L"Z←⊃¨"S L"LO/¨⊂¨1/"S L"R",
};

MAGIC_FCNOPR MFO_RoS3 =
{L""S L"Z←("S L"LO " MFON_RoS3 L") "S L"R",
 RoS3Body,
 countof (RoS3Body),
};

#undef  S


//***************************************************************************
//  End of File: mf_ros.h
//***************************************************************************