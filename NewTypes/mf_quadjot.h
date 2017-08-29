//***************************************************************************
//  NARS2000 -- Magic Function -- Matrix Operator
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2017 Sudley Place Software

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
//  Magic function/operator for Domino with a function right operand
//    a.k.a. Matrix Operator
//***************************************************************************

static LPAPLCHAR MatOprBody[] =
{L"⎕PRO:",
 // Calculate the {L} LO of the (square) matrix R
 L":if 0=⎕NC 'L' ⋄ L←⊢ ⋄ :endif",
 L"(Eval Evec)←⌹⍠3 R",
 L"⎕ERROR ((⍴Eval)≠⍴∪Eval)/'EIGENVALUE ERROR'",
 L"Z←Evec+.×(∘⌻L LO Eval)+.×⌹Evec",
};

MAGIC_FCNOPR MFO_MatOpr =
{L"Z←{L} (LO " MFON_MatOpr L") R;Eval Evec T",
 MatOprBody,
 countof (MatOprBody),
};


//***************************************************************************
//  End of File: mf_quadjot.h
//***************************************************************************