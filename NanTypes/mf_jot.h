//***************************************************************************
//  NARS2000 -- Magic Function -- Jot (Mask and Mesh)
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
//  Magic function for dyadic Mask
//
//  Dyadic Mask
//***************************************************************************

static LPAPLCHAR DydMaskBody[] =
{
L":if 0=⍴⍴"S L"L ⋄ "S L"L←(⍴"S L"R)⍴"S L"L ⋄ :end",
L":if 0=⍴⍴"S L"R ⋄ "S L"R←(⍴"S L"L)⍴"S L"R ⋄ :end",

//L"⎕ERROR ((⍴⍴"S L"L)≢⍴⍴"S L"R)/'"S L"RANK ERROR'",
//L"⎕ERROR (( ⍴"S L"L)≢ ⍴"S L"R)/'"S L"LENGTH ERROR'",
//L"⎕ERROR (1<⍴⍴"S L"LO)/'RANK ERROR'",
//L"⎕ERROR ((⍴"S L"L)["S L"X]≢⍴"S L"LO)/'LENGTH_ERROR'",

L":if 0=⎕NC '"S L"X' ⋄ "S L"X←(⍴⍴"S L"R)-~⎕IO ⋄ :end",
L":if 0=⍴⍴"S L"LO ⋄ "S L"LO←(⍴"S L"L)["S L"X]⍴"S L"LO ⋄ :end",

S L"Z←(|"S L"LO~0)/["S L"X] (⊂(⍋×"S L"LO,-"S L"LO)[⍋⍋×"S L"LO~0])⌷["S L"X] "S L"L,["S L"X] "S L"R",
};

MAGIC_FCNOPR MFO_DydMask =
{S L"Z←"S L"L ("S L"LO " MFON_DydMask L"["S L"X]) "S L"R",
 DydMaskBody,
 countof (DydMaskBody),
};

//***************************************************************************
//  Magic function for dyadic Mesh
//
//  Dyadic Mesh
//***************************************************************************

static LPAPLCHAR DydMeshBody[] =
{
L":if 0=⎕NC '"S L"X' ⋄ "S L"X←(⍴⍴"S L"R)-~⎕IO ⋄ :end",

L":if 0=⍴⍴"S L"L ⋄ "S L"Y←⍴"S L"R ⋄ "S L"Y["S L"X]←+/"S L"LO<0 ⋄ "S L"L←"S L"Y⍴"S L"L ⋄ :end",
L":if 0=⍴⍴"S L"R ⋄ "S L"Y←⍴"S L"L ⋄ "S L"Y["S L"X]←+/"S L"LO>0 ⋄ "S L"R←"S L"Y⍴"S L"R ⋄ :end",

//L"⎕ERROR ((⍴⍴"S L"L)≢⍴⍴"S L"R)/'RANK ERROR'",
L"⎕ERROR ((⍴"S L"L)["S L"X]≠+/"S L"LO<0)/'LENGTH ERROR'",
L"⎕ERROR ((⍴"S L"R)["S L"X]≠+/"S L"LO>0)/'LENGTH ERROR'",
//L"⎕ERROR ((⍴"S L"L)[(⍳⍴⍴"S L"L)~"S L"X]≢(⍴"S L"R)[(⍳⍴⍴"S L"R)~"S L"X])/'LENGTH ERROR'",

S L"Z←(|"S L"LO)\\["S L"X] (⊂⍋⍋×"S L"LO~0)⌷["S L"X] "S L"L,["S L"X] "S L"R",    // Double the \ to avoid it being confused with a character escape sequence
};

MAGIC_FCNOPR MFO_DydMesh =
{S L"Z←"S L"L ("S L"LO " MFON_DydMesh L"["S L"X]) "S L"R;"S L"Y",
 DydMeshBody,
 countof (DydMeshBody),
};


//***************************************************************************
//  Magic function for Identity Function for Compose
//
//  Identity Function for Compose
//***************************************************************************

static LPAPLCHAR IdnJotBody[] =
{
 S L"Z←"S L"RO ⊃"S L"LO/0⍴⊂"S L"R",    
};    
    
MAGIC_FCNOPR MFO_IdnJot =
{S L"Z←("S L"LO " MFON_IdnJot L" "S L"RO) "S L"R",
 IdnJotBody,
 countof (IdnJotBody),
};

#undef  S


//***************************************************************************
//  End of File: mf_jot.h
//***************************************************************************