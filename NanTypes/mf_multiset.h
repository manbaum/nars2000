//***************************************************************************
//  NARS2000 -- Magic Function -- Multiset
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
//  Magic function for Multiset Asymmetric Difference
//
//  Dyadic Tilde -- Multiset Asymmetric Difference
//
//  On scalar or vector args, return the elements in L~R.
//***************************************************************************

static LPAPLCHAR DydBodyMAD[] =
{L"⎕MS:"S L"Z←(~"S L"L∊⍦"S L"R)/"S L"L ⋄ →0",
 L"⎕PRO:"S L"Z←⊤(⊤"S L"L) " MFON_MAD L" ⊤"S L"R",
};

MAGIC_FCNOPR MFO_MAD =
{L""S L"Z←"S L"L " MFON_MAD L" "S L"R",
 DydBodyMAD,
 countof (DydBodyMAD),
};

//***************************************************************************
//  Magic function for Multiset Symmetric Difference
//
//  Dyadic Section -- Multiset Symmetric Difference
//
//  On scalar or vector args, return the elements in (L~$R),R~$L.
//***************************************************************************

static LPAPLCHAR DydBodyMSD[] =
{L"⎕MS:"S L"Z←("S L"L " MFON_MAD L" "S L"R),"S L"R " MFON_MAD L" "S L"L ⋄ →0",
 L"⎕PRO:"S L"Z←⊤(⊤"S L"L) " MFON_MSD L" ⊤"S L"R",
};

MAGIC_FCNOPR MFO_MSD =
{L""S L"Z←"S L"L " MFON_MSD L" "S L"R",
 DydBodyMSD,
 countof (DydBodyMSD),
};

//***************************************************************************
//  Magic function for Multiset Union
//
//  Dyadic DownShoe -- Multiset Union
//
//  On scalar or vector args, return the multiset union of the two args
//***************************************************************************

static LPAPLCHAR DydBodyMU[] =
{L"⎕MS:"S L"Z←"S L"L,"S L"R " MFON_MAD L" "S L"L ⋄ →0",
 L"⎕PRO:"S L"Z←⊤(⊤"S L"L) " MFON_MU L" ⊤"S L"R",
};

MAGIC_FCNOPR MFO_MU =
{L""S L"Z←"S L"L " MFON_MU L" "S L"R",
 DydBodyMU,
 countof (DydBodyMU),
};

//***************************************************************************
//  Magic function for Multiset Intersection
//
//  Dyadic UpShoe -- Intersection
//
//  On scalar or vector args, return the multiset intersection of the two args
//***************************************************************************

static LPAPLCHAR DydBodyMI[] =
{L"⎕MS:"S L"Z←("S L"L∊⍦"S L"R)/"S L"L ⋄ →0",
 L"⎕PRO:"S L"Z←⊤(⊤"S L"L) " MFON_MI L" ⊤"S L"R",
};

MAGIC_FCNOPR MFO_MI =
{L""S L"Z←"S L"L " MFON_MI L" "S L"R",
 DydBodyMI,
 countof (DydBodyMI),
};

//***************************************************************************
//  Magic function for Multiset Proper Subset
//
//  Dyadic LeftShoe -- Multiset Proper Subset
//
//  On scalar or vector args, return TRUE iff L is a proper multisubset of R.
//***************************************************************************

static LPAPLCHAR DydBodyMLRS[] =
{L"⎕MS:"S L"Z←("S L"L " MFON_MLRSU L" "S L"R)^"S L"L≢⍦"S L"R ⋄ →0",
 L"⎕PRO:"S L"Z←⊤(⊤"S L"L) " MFON_MLRS L" ⊤"S L"R",
};

MAGIC_FCNOPR MFO_MLRS =
{L""S L"Z←"S L"L " MFON_MLRS L" "S L"R",
 DydBodyMLRS,
 countof (DydBodyMLRS),
};

//***************************************************************************
//  Magic function for Multiset Subset
//
//  Dyadic LeftShoeUnderbar -- Multiset Subset
//
//  On scalar or vector args, return TRUE iff L is a multisubset of R.
//***************************************************************************

static LPAPLCHAR DydBodyMLRSU[] =
{L"⎕MS:"S L"Z←~0∊"S L"L " MFON_MEO L" "S L"R ⋄ →0",
 L"⎕PRO:"S L"Z←⊤(⊤"S L"L) " MFON_MLRSU L" ⊤"S L"R",
};

MAGIC_FCNOPR MFO_MLRSU =
{L""S L"Z←"S L"L " MFON_MLRSU L" "S L"R",
 DydBodyMLRSU,
 countof (DydBodyMLRSU),
};

//***************************************************************************
//  Magic function for Multiset Index Of
//
//  Dyadic Iota -- Multiset Index Of
//
//  On scalar or vector args, return the elements in L⍳⍦R.
//***************************************************************************

static LPAPLCHAR DydBodyMIO[] =
{L"⎕MS:"S L"L←1/"S L"L⋄"S L"Z←((⍴"S L"L)⍴⍋⍋"S L"L⍳"S L"L,"S L"R)⍳(⍴"S L"R)⍴⍋⍋"S L"L⍳"S L"R,"S L"L ⋄ →0",
 L"⎕PRO:"S L"Z←⊤(⊤"S L"L) " MFON_MIO L" ⊤"S L"R",
};

MAGIC_FCNOPR MFO_MIO =
{L""S L"Z←"S L"L " MFON_MIO L" "S L"R",
 DydBodyMIO,
 countof (DydBodyMIO),
};

//***************************************************************************
//  Magic function for Multiset Element Of
//
//  Dyadic Epsilon -- Multiset Element Of
//
//  On scalar or vector args, return the elements in L∊⍦R.
//***************************************************************************

static LPAPLCHAR DydBodyMEO[] =
{L"⎕MS:"S L"L←1/"S L"L⋄"S L"Z←((⍴"S L"L)⍴⍋⍋"S L"L⍳"S L"L,"S L"R)∊(⍴"S L"R)⍴⍋⍋"S L"L⍳"S L"R,"S L"L ⋄ →0",
 L"⎕PRO:"S L"Z←⊤(⊤"S L"L) " MFON_MEO L" ⊤"S L"R",
};

MAGIC_FCNOPR MFO_MEO =
{L""S L"Z←"S L"L " MFON_MEO L" "S L"R",
 DydBodyMEO,
 countof (DydBodyMEO),
};

//***************************************************************************
//  Magic function for Multiset Match
//
//  Dyadic Match -- Multiset Match
//
//  On scalar or vector args, return the elements in L≡⍦R.
//***************************************************************************

static LPAPLCHAR DydBodyMM[] =
{L"⎕MS:"S L"Z←(⍴1/"S L"L)≡⍴1/"S L"R ⋄ →"S L"Z↓0",
 L""S L"Z←∧/"S L"L∊⍦"S L"R ⋄ →0",
 L"⎕PRO:"S L"Z←⊤(⊤"S L"L) " MFON_MM L" ⊤"S L"R",
};

MAGIC_FCNOPR MFO_MM =
{L""S L"Z←"S L"L " MFON_MM L" "S L"R",
 DydBodyMM,
 countof (DydBodyMM),
};

//***************************************************************************
//  Magic function for Multiset Multiplicities
//
//  Monadic DownShoe -- Multiset Multiplicities
//
//  On scalar or vector args, return the multiplicities of the unique elements
//    in the same order as the unique elements.
//***************************************************************************

static LPAPLCHAR DydBodyMMUL[] =
{L"⎕MS:"S L"Z←¯2-/⍸1,(2≠/"S L"R[⍋(1/"S L"R)⍳"S L"R]),1⋄→0",
 L"⎕PRO:"S L"Z←⊤ " MFON_MMUL L" ⊤"S L"R",
};

MAGIC_FCNOPR MFO_MMUL =
{L""S L"Z←" MFON_MMUL L" "S L"R",
 DydBodyMMUL,
 countof (DydBodyMMUL),
};


//***************************************************************************
//  Magic function/operator for Multiset Dyadic Iota Underbar
//
//  Multiset Dyadic Iota Underbar -- Matrix Iota
//
//  Return an array of indices from searching for the
//    array  of trailing subarrays from the right arg in the
//    vector of trailing subarrays from the left arg.
//***************************************************************************

static LPAPLCHAR DydBodyMDIU[] =
{L"⎕MS:"S L"Z←(⊂⍤¯1 "S L"L)⍳⍦⊂⍤(¯1+⍴⍴"S L"L) "S L"R ⋄ →0",
 L"⎕PRO:"S L"Z←⊤(⊤"S L"L) " MFON_MDIU L" ⊤"S L"R",
};

MAGIC_FCNOPR MFO_MDIU =
{L""S L"Z←"S L"L " MFON_MDIU L" "S L"R",
 DydBodyMDIU,
 countof (DydBodyMDIU),
};

#undef  S
  

//***************************************************************************
//  End of File: mf_multiset.h
//***************************************************************************