//***************************************************************************
//  NARS2000 -- Magic Function -- Monadic ⎕FMT
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

// ⍪(⊂'L"'),¨(1 ⎕cr '#...'),¨⊂'",'
// ⍪2⌽¨(⊂'",L"'),¨1 ⎕cr '#...'

//***************************************************************************
//  Magic function/operator for Monadic []FMT
//
//  Monadic []FMT -- Boxed Display of Arrays
//
//  Return a boxed representation of an array.
//***************************************************************************

static LPAPLCHAR MonBodyFMT[] =
{L"⎕IO←0 ⋄ ⎕FEATURE[0]←1",
 L":if 1<≡"S L"R",
 L"  "S L"Type←'∊'",
 L":else",
 L"  :if 0∊⍴"S L"R",
 L"    "S L"Proto←↑"S L"R",
 L"  :else",
 L"    "S L"Proto←"S L"R",
 L"  :endif",
 L"  "S L"Type←(0 ∅ ' '∊⊤"S L"Proto)/'~~─¯'[0 1,2+0∊⍴"S L"R]",
 L"  :select ×/⍴"S L"Type",
 L"  :case 1",
 L"    "S L"Type←⍬⍴"S L"Type",
 L"  :case 2",
 L"    "S L"Type←'+'",
 L"  :endselect",
 L":endif",
 L""S L"Proto←0",
 L":if 1≥≡"S L"Z←1/"S L"R",
 L"  :if 0=⍴⍴"S L"R",
 L"    "S L"Z←⍕"S L"Z ⋄ "S L"Z←(1,⍴"S L"Z)⍴"S L"Z",
 L"    :if "S L"Type='─'",
 L"      "S L"Z←"S L"Z⍪'¯'",
 L"    :elseif "S L"Type='~'",
 L"      "S L"Z←"S L"Z⍪'~'",
 L"    :endif",
 L"    :return",
 L"  :endif",
 L"  "S L"Z←((-2⌈⍴⍴"S L"Z)↑1,⍴"S L"Z)⍴"S L"Z←' ',⍕"S L"Z",
 L"  :if 0∊⍴"S L"R",
 L"    :if "S L"Type='~'",
 L"      "S L"Z←1 2⍴' 0'",
 L"    :else",
 L"      "S L"Z←1 1⍴' '",
 L"    :endif",
 L"  :endif",
 L"  "S L"Z←"S L"Z " MFON_BoxFMT L" "S L"R",
 L":else",
 L"  :if 0∊⍴"S L"R",
 L"    "S L"Z←" MFON_MonFMT L"↑"S L"R",
 L"    "S L"Z←"S L"Z " MFON_BoxFMT L" "S L"R",
 L"  :else",
 L"    "S L"Z←''",
 L"    :forlcl "S L"I :in "S L"R",
 L"      "S L"A←" MFON_MonFMT L" "S L"I",
 L"      :if 0=≡"S L"I",
 L"        "S L"A←' '⍪"S L"A",
 L"      :endif",
 L"      "S L"Z←"S L"Z,⊂"S L"A",
 L"    :endforlcl",
 L"    :if 1=⍴⍴"S L"R",
 L"      "S L"Z←0 ¯1↓⊃,/(∊⌈/⍴¨"S L"Z)[0]↑[0]¨"S L"Z,¨' '",
 L"    :else",
 L"      "S L"Z←⊃(⍴"S L"R)⍴"S L"Z",
 L"    :endif",
 L"    "S L"Z←((-3⌈⍴⍴"S L"Z)↑1,⍴"S L"Z)⍴"S L"Z",
 L"    "S L"Z←(((¯3↓⍴"S L"Z),(⍴"S L"Z)[¯2],×/(⍴"S L"Z)[¯3 ¯1])⍴,[¯2 ¯3] "S L"Z) " MFON_BoxFMT L" "S L"R",
 L"  :endif",                                                     
 L":endif",
};

MAGIC_FCNOPR MFO_MonFMT =
{L""S L"Z←" MFON_MonFMT L" "S L"R;⎕IO "S L"Type "S L"Lft "S L"Top "S L"Proto "S L"A ⎕FEATURE",
 MonBodyFMT,
 countof (MonBodyFMT),
};


//***************************************************************************
//  Magic function/operator for Box FMT
//
//  Box function -- Boxed Display For Arrays
//
//  Place line drawing chars around an array.
//***************************************************************************

static LPAPLCHAR MonBodyBoxFMT[] =
{L""S L"LD←⍕(0≠⍴⍴"S L"R)/¯1↑⍴"S L"R",
 L""S L"RD←⍕⍪¯1↓⍴"S L"R",
 L""S L"RD←(+/"S L"RD=' ')⌽"S L"RD",
 L""S L"D←¯2+⍴⍴"S L"Z",
 L":if "S L"D>0",
 L":while 2<⍴⍴"S L"Z",
 L"  "S L"Z←,[¯3 ¯2] "S L"Z,[¯2]' '",
 L":endwhile",
 L""S L"Z←(-"S L"D)↓[0]"S L"Z",
 L":endif",
 L""S L"Z←(¯2↑1,⍴"S L"Z)⍴"S L"Z ⋄ "S L"Z←((⍴"S L"Z)⌈(¯1↑⍴"S L"RD),⍴"S L"LD)↑"S L"Z",
 L":if 1≥⍴⍴"S L"R",
 L"  "S L"RD←((1↑⍴"S L"Z),1)⍴'│'",
 L":else",
 L"  "S L"RD←⍉(1↑⍴"S L"Z)↑[1] "S L"RD ⋄ "S L"RD[(,"S L"RD=' ')/,⍳⍴"S L"RD]←'│'",
 L":endif",
 L""S L"RD←'┬'⍪"S L"RD⍪'┴' ⋄ "S L"RD[0 ¯1;0]←'┌└'",
 L""S L"Z←"S L"Z,'│'",
 L""S L"D←≡"S L"R",
 L":if 1<"S L"D",
 L"  "S L"D←⍕"S L"D",
 L"  "S L"Z[⍳-⍴"S L"D;¯1]←⌽"S L"D",
 L":endif",
 L""S L"Z←'─'⍪"S L"Z⍪'─'",
 L""S L"Z[0 ¯1;¯1]←'┐┘'",
 L""S L"Z[0;⍳⍴"S L"LD]←"S L"LD",
 L""S L"Z[¯1;0]←"S L"Type",
 L""S L"Z←"S L"RD,"S L"Z",
};

MAGIC_FCNOPR MFO_BoxFMT =
{L""S L"Z←"S L"Z " MFON_BoxFMT L" "S L"R;"S L"RD "S L"LD "S L"D",
 MonBodyBoxFMT,
 countof (MonBodyBoxFMT),
};

#undef  S


//***************************************************************************
//  End of File: mf_quadfmt.h
//***************************************************************************