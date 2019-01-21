//***************************************************************************
//  NARS2000 -- Magic Function -- DieresisJot
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
//  Magic function/operator for monadic derived function from the rank dyadic operator
//
//  This operator was originally based upon code found in the paper by J. Philip Benkhard,
//    "Extending structure, type, and expression in APL2", ACM SIGAPL APL Quote Quad,
//    v.21 n.4, p.20-38, Aug. 1991, but later was modified to reflect the
//    definition of the Rank operator in the ISO-IEC 13751 Extended APL Standard.
//***************************************************************************

static LPAPLCHAR MonBody[] =
{
  L""S L"YR←(1↑⌽3⍴⌽"S L"Y)⌊⍴⍴"S L"R",
  L":if 0>"S L"YR ⋄ "S L"YR←0⌈"S L"YR+⍴⍴"S L"R ⋄ :end",
  L""S L"Z←⎕EC '"S L"LO¨⊂[(-"S L"YR)↑⍳⍴⍴"S L"R] "S L"R'",
  L":select ⎕IO⊃"S L"Z",
  L":case 0",
  L"  "S L"YR←(2+⎕IO)⊃"S L"Z",
  L"  ⎕ERROR (∧\\"S L"YR≠⎕TCNL)/"S L"YR",
  L":case 3",
  L"  ⎕EX '"S L"Z'",
  L":else",
  L"  "S L"Z←(2+⎕IO)⊃"S L"Z",
  L"  :if 0=⎕NC '"S L"X' ⋄ "S L"Z←⊃"S L"Z",
  L"  :else         ⋄ "S L"Z←⊃["S L"X] "S L"Z ⋄ :end",
  L":end",
  L"→0 ",
  
  L"⎕PRO:"S L"YR←(1↑⌽3⍴⌽"S L"Y)⌊⍴⍴"S L"R",
  L":if 0>"S L"YR ⋄ "S L"YR←0⌈"S L"YR+⍴⍴"S L"R ⋄ :end",
  L""S L"Z←⎕EC '"S L"LO¨¨⊂[(-"S L"YR)↑⍳⍴⍴"S L"R]¨0⍴⊂"S L"R'",
  L":select ⎕IO⊃"S L"Z",
  L":case 0",
  L"  "S L"YR←(2+⎕IO)⊃"S L"Z",
  L"  ⎕ERROR (∧\\"S L"YR≠⎕TCNL)/"S L"YR",
  L":case 3",
  L"  ⎕EX '"S L"Z'",
  L":else",
  L"  "S L"Z←⊃(2+⎕IO)⊃"S L"Z",
  L"  :if 0=⎕NC '"S L"X' ⋄ "S L"Z←⊃"S L"Z",
  L"  :else         ⋄ "S L"Z←⊃["S L"X] "S L"Z ⋄ :end",
  L":end",
};

MAGIC_FCNOPR MFO_MonRank =
{
  L""S L"Z←("S L"LO " MFON_MonRank L"["S L"X] "S L"Y) "S L"R;"S L"YR",
  MonBody,
  countof (MonBody),
};


//***************************************************************************
//  Magic function/operator for dyadic derived function from the rank dyadic operator
//
//  This operator was originally based upon code found in the paper by J. Philip Benkhard,
//    "Extending structure, type, and expression in APL2", ACM SIGAPL APL Quote Quad,
//    v.21 n.4, p.20-38, Aug. 1991, but later was modified to reflect the
//    definition of the Rank operator in the ISO-IEC 13751 Extended APL Standard.
//***************************************************************************

static LPAPLCHAR DydBody[] =
{
  L"("S L"YL "S L"YR)←(1↓⌽3⍴⌽"S L"Y)⌊(⍴⍴"S L"L),⍴⍴"S L"R",
  L":if 0>"S L"YL ⋄ "S L"YL←0⌈"S L"YL+⍴⍴"S L"L ⋄ :end",
  L":if 0>"S L"YR ⋄ "S L"YR←0⌈"S L"YR+⍴⍴"S L"R ⋄ :end",
  L""S L"Z←⎕EC '(⊂[(-"S L"YL)↑⍳⍴⍴"S L"L] "S L"L) "S L"LO¨⊂[(-"S L"YR)↑⍳⍴⍴"S L"R] "S L"R'",
  L":select ⎕IO⊃"S L"Z",
  L":case 0",
  L"  "S L"YR←(2+⎕IO)⊃"S L"Z",
  L"  ⎕ERROR (∧\\"S L"YR≠⎕TCNL)/"S L"YR",
  L":case 3",
  L"  ⎕EX '"S L"Z'",
  L":else",
  L"  "S L"Z←(2+⎕IO)⊃"S L"Z",
  L"  :if 0=⎕NC '"S L"X' ⋄ "S L"Z←⊃"S L"Z",
  L"  :else         ⋄ "S L"Z←⊃["S L"X] "S L"Z ⋄ :end",
  L":end",
  L"→0 ",

  L"⎕PRO:("S L"YL "S L"YR)←(1↓⌽3⍴⌽"S L"Y)⌊(⍴⍴"S L"L),⍴⍴"S L"R",
  L":if 0>"S L"YL ⋄ "S L"YL←0⌈"S L"YL+⍴⍴"S L"L ⋄ :end",
  L":if 0>"S L"YR ⋄ "S L"YR←0⌈"S L"YR+⍴⍴"S L"R ⋄ :end",
  L""S L"Z←⎕EC '(⊂[(-"S L"YL)↑⍳⍴⍴"S L"L]¨0⍴⊂"S L"L) "S L"LO¨¨⊂[(-"S L"YR)↑⍳⍴⍴"S L"R]¨0⍴⊂"S L"R'",
  L":select ⎕IO⊃"S L"Z",
  L":case 0",
  L"  "S L"YR←(2+⎕IO)⊃"S L"Z",
  L"  ⎕ERROR (∧\\"S L"YR≠⎕TCNL)/"S L"YR",
  L":case 3",
  L"  ⎕EX '"S L"Z'",
  L":else",
  L"  "S L"Z←⊃(2+⎕IO)⊃"S L"Z",
  L"  :if 0=⎕NC '"S L"X' ⋄ "S L"Z←⊃"S L"Z",
  L"  :else         ⋄ "S L"Z←⊃["S L"X] "S L"Z ⋄ :end",
  L":end",
};  

MAGIC_FCNOPR MFO_DydRank =
{
  L""S L"Z←"S L"L ("S L"LO " MFON_DydRank L"["S L"X] "S L"Y) "S L"R;"S L"YL "S L"YR",
  DydBody,
  countof (DydBody),
};

#undef  S


//***************************************************************************
//  End of File: mf_dijot.h
//***************************************************************************