//***************************************************************************
//  NARS2000 -- Hypercomplex Units & Rotate Tables
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2016 Sudley Place Software

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


EXTERNchar gUnitCoeffs4[8][4]
#ifdef DEFINE_VALUES
 = {
    {  1,  0,  0,  0},
    { -1,  0,  0,  0},
    {  0,  1,  0,  0},
    {  0, -1,  0,  0},
    {  0,  0,  1,  0},
    {  0,  0, -1,  0},
    {  0,  0,  0,  1},
    {  0,  0,  0, -1},
   }
#endif
;


EXTERN
UCHAR gGcdRotateTab2[9]
#ifdef DEFINE_VALUES
 = {
       1,   // --
       2,   // 0-
       2,   // +-
       1,   // -0
       0,   // 00
       0,   // +0
       3,   // -+
       3,   // 0+
       0,   // ++
   }
#endif
;


EXTERN
UCHAR gGcdRotateTab4[81][2]
#ifdef DEFINE_VALUES
 = {
    {  0,  1},  // ----
    {  2,  6},  // 0---
    {  0,  1},  // +---
    {  0,  1},  // -0--
    {  0,  4},  // 00--
    {  0,  4},  // +0--
    {  0,  7},  // -+--
    {  0,  4},  // 0+--
    {  0,  4},  // ++--
    {  0,  1},  // --0-
    {  0,  6},  // 0-0-
    {  0,  6},  // +-0-
    {  0,  1},  // -00-
    {  0,  6},  // 000-
    {  0,  6},  // +00-
    {  0,  3},  // -+0-
    {  0,  3},  // 0+0-
    {  2,  3},  // ++0-
    {  0,  3},  // --+-
    {  0,  6},  // 0-+-
    {  0,  6},  // +-+-
    {  2,  5},  // -0+-
    {  0,  6},  // 00+-
    {  0,  6},  // +0+-
    {  0,  3},  // -++-
    {  0,  3},  // 0++-
    {  0,  4},  // +++-
    {  0,  1},  // ---0
    {  0,  2},  // 0--0
    {  0,  2},  // +--0
    {  0,  1},  // -0-0
    {  0,  4},  // 00-0
    {  0,  4},  // +0-0
    {  2,  1},  // -+-0
    {  0,  4},  // 0+-0
    {  0,  4},  // ++-0
    {  0,  1},  // --00
    {  0,  2},  // 0-00
    {  0,  2},  // +-00
    {  0,  1},  // -000
    {  0,  0},  // 0000
    {  0,  0},  // +000
    {  0,  3},  // -+00
    {  0,  3},  // 0+00
    {  0,  0},  // ++00
    {  0,  5},  // --+0
    {  0,  5},  // 0-+0
    {  2,  0},  // +-+0
    {  0,  5},  // -0+0
    {  0,  5},  // 00+0
    {  0,  0},  // +0+0
    {  0,  3},  // -++0
    {  0,  3},  // 0++0
    {  0,  0},  // +++0
    {  0,  5},  // ---+
    {  0,  2},  // 0--+
    {  0,  2},  // +--+
    {  0,  7},  // -0-+
    {  0,  7},  // 00-+
    {  2,  4},  // +0-+
    {  0,  7},  // -+-+
    {  0,  7},  // 0+-+
    {  0,  2},  // ++-+
    {  2,  2},  // --0+
    {  0,  2},  // 0-0+
    {  0,  2},  // +-0+
    {  0,  7},  // -00+
    {  0,  7},  // 000+
    {  0,  0},  // +00+
    {  0,  7},  // -+0+
    {  0,  7},  // 0+0+
    {  0,  0},  // ++0+
    {  0,  5},  // --++
    {  0,  5},  // 0-++
    {  0,  6},  // +-++
    {  0,  5},  // -0++
    {  0,  5},  // 00++
    {  0,  0},  // +0++
    {  0,  0},  // -+++
    {  2,  7},  // 0+++
    {  0,  0},  // ++++
   }
#endif
;


//***************************************************************************
//  End of File: hc_vars.h
//***************************************************************************
