//***************************************************************************
//  NARS2000 -- Magic Function -- DieresisJot
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2018 Sudley Place Software

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
  L"YR←(1↑⌽3⍴⌽Y)⌊⍴⍴R",
  L":if 0>YR ⋄ YR←0⌈YR+⍴⍴R ⋄ :end",
  L"Z←⎕EC 'LO¨⊂[(-YR)↑⍳⍴⍴R] R'",
  L":select ⎕IO⊃Z",
  L":case 0",
  L"  YR←(2+⎕IO)⊃Z",
  L"  ⎕ERROR (∧\\YR≠⎕TCNL)/YR",
  L":case 3",
  L"  ⎕EX 'Z'",
  L":else",
  L"  Z←(2+⎕IO)⊃Z",
  L"  :if 0=⎕NC 'X' ⋄ Z←⊃Z",
  L"  :else         ⋄ Z←⊃[X] Z ⋄ :end",
  L":end",
  L"→0 ",
  
  L"⎕PRO:YR←(1↑⌽3⍴⌽Y)⌊⍴⍴R",
  L":if 0>YR ⋄ YR←0⌈YR+⍴⍴R ⋄ :end",
  L"Z←⎕EC 'LO¨¨⊂[(-YR)↑⍳⍴⍴R]¨0⍴⊂R'",
  L":select ⎕IO⊃Z",
  L":case 0",
  L"  YR←(2+⎕IO)⊃Z",
  L"  ⎕ERROR (∧\\YR≠⎕TCNL)/YR",
  L":case 3",
  L"  ⎕EX 'Z'",
  L":else",
  L"  Z←⊃(2+⎕IO)⊃Z",
  L"  :if 0=⎕NC 'X' ⋄ Z←⊃Z",
  L"  :else         ⋄ Z←⊃[X] Z ⋄ :end",
  L":end",
};

MAGIC_FCNOPR MFO_MonRank =
{
  L"Z←(LO " MFON_MonRank L"[X] Y) R;YR",
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
  L"(YL YR)←(1↓⌽3⍴⌽Y)⌊(⍴⍴L),⍴⍴R",
  L":if 0>YL ⋄ YL←0⌈YL+⍴⍴L ⋄ :end",
  L":if 0>YR ⋄ YR←0⌈YR+⍴⍴R ⋄ :end",
  L"Z←⎕EC '(⊂[(-YL)↑⍳⍴⍴L] L) LO¨⊂[(-YR)↑⍳⍴⍴R] R'",
  L":select ⎕IO⊃Z",
  L":case 0",
  L"  YR←(2+⎕IO)⊃Z",
  L"  ⎕ERROR (∧\\YR≠⎕TCNL)/YR",
  L":case 3",
  L"  ⎕EX 'Z'",
  L":else",
  L"  Z←(2+⎕IO)⊃Z",
  L"  :if 0=⎕NC 'X' ⋄ Z←⊃Z",
  L"  :else         ⋄ Z←⊃[X] Z ⋄ :end",
  L":end",
  L"→0 ",

  L"⎕PRO:(YL YR)←(1↓⌽3⍴⌽Y)⌊(⍴⍴L),⍴⍴R",
  L":if 0>YL ⋄ YL←0⌈YL+⍴⍴L ⋄ :end",
  L":if 0>YR ⋄ YR←0⌈YR+⍴⍴R ⋄ :end",
  L"Z←⎕EC '(⊂[(-YL)↑⍳⍴⍴L]¨0⍴⊂L) LO¨¨⊂[(-YR)↑⍳⍴⍴R]¨0⍴⊂R'",
  L":select ⎕IO⊃Z",
  L":case 0",
  L"  YR←(2+⎕IO)⊃Z",
  L"  ⎕ERROR (∧\\YR≠⎕TCNL)/YR",
  L":case 3",
  L"  ⎕EX 'Z'",
  L":else",
  L"  Z←⊃(2+⎕IO)⊃Z",
  L"  :if 0=⎕NC 'X' ⋄ Z←⊃Z",
  L"  :else         ⋄ Z←⊃[X] Z ⋄ :end",
  L":end",
};  

MAGIC_FCNOPR MFO_DydRank =
{
  L"Z←L (LO " MFON_DydRank L"[X] Y) R;YL;YR",
  DydBody,
  countof (DydBody),
};


//***************************************************************************
//  End of File: mf_dijot.h
//***************************************************************************