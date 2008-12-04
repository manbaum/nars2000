//***************************************************************************
//  NARS2000 -- Syntax Coloring Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2008 Sudley Place Software

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

#define DEF_SCN_TRANSPARENT    CLR_INVALID

#define DEF_SC_GLBNAME         DEF_SCN_RED        , DEF_SCN_TRANSPARENT
#define DEF_SC_LCLNAME         DEF_SCN_GRAY       , DEF_SCN_TRANSPARENT
#define DEF_SC_LABEL           DEF_SCN_MAROON     , DEF_SCN_TRANSPARENT
#define DEF_SC_PRIMITIVE       DEF_SCN_NAVY       , DEF_SCN_TRANSPARENT
#define DEF_SC_SYSFCN          DEF_SCN_NAVY       , DEF_SCN_TRANSPARENT
#define DEF_SC_GLBSYSVAR       DEF_SCN_PURPLE     , DEF_SCN_TRANSPARENT
#define DEF_SC_LCLSYSVAR       DEF_SCN_PURPLE     , DEF_SCN_TRANSPARENT
#define DEF_SC_CTRLSTRUC       DEF_SCN_MAROON     , DEF_SCN_TRANSPARENT
#define DEF_SC_NUMCONST        DEF_SCN_GRAY       , DEF_SCN_TRANSPARENT
#define DEF_SC_CHRCONST        DEF_SCN_TEAL       , DEF_SCN_TRANSPARENT
#define DEF_SC_COMMENT         DEF_SCN_GREEN      , DEF_SCN_TRANSPARENT
#define DEF_SC_MATCHGRP1       DEF_SCN_BLUE       , DEF_SCN_TRANSPARENT
#define DEF_SC_MATCHGRP2       DEF_SCN_DARKGREEN  , DEF_SCN_TRANSPARENT
#define DEF_SC_MATCHGRP3       DEF_SCN_DARKCYAN   , DEF_SCN_TRANSPARENT
#define DEF_SC_MATCHGRP4       DEF_SCN_DARKMAGENTA, DEF_SCN_TRANSPARENT
#define DEF_SC_UNMATCHGRP      DEF_SCN_BLUE       , DEF_SCN_LIGHTCYAN
#define DEF_SC_UNNESTED        DEF_SCN_BLUE       , DEF_SCN_LIGHTPINK
#define DEF_SC_UNK             DEF_SCN_BLUE       , DEF_SCN_VIOLET

#define DEF_SC_WHITE           DEF_SCN_WHITE      , DEF_SCN_TRANSPARENT

// Syntax Coloring categories
typedef enum tagSC_TYPE
{
    SC_GLBNAME = 0,     // 00:  Global Name
    SC_LCLNAME,         // 01:  Local  ...
    SC_LABEL,           // 02:  Label (including trailing colon)
    SC_PRIMITIVE,       // 03:  Primitive Function/Operator (including quad, quote-quad, left/right arrows,
                        //      del, semicolon, and diamond)
    SC_SYSFCN,          // 04:  System Function (including leading quad)
    SC_GLBSYSVAR,       // 05:  Global System Variable (including leading quad)
    SC_LCLSYSVAR,       // 06:  Local  ...
    SC_CTRLSTRUC,       // 07:  Control Structure (including leading colon)
    SC_NUMCONST,        // 08:  Numeric constant (including leading overbar)
    SC_CHRCONST,        // 09:  Character constant (including single- or double-quote marks)
    SC_COMMENT,         // 0A:  Comment (including comment symbol)
    SC_MATCHGRP1,       // 0B:  Matched Grouping Symbols [] () {}
    SC_MATCHGRP2,       // 0C:  Matched Grouping Symbols [] () {}
    SC_MATCHGRP3,       // 0D:  Matched Grouping Symbols [] () {}
    SC_MATCHGRP4,       // 0E:  Matched Grouping Symbols [] () {}
    SC_UNMATCHGRP,      // 0F:  Unmatched Grouping Symbols [] () {} ' "
    SC_UNNESTED,        // 10:  Improperly Nested Grouping Symbols [] () {}
    SC_UNK,             // 11:  Unknown symbol
    SC_LENGTH           // 12:  Length
                        //      Because this enum is origin-0, this value is the # valid columns.
} SCTYPE, *LPSCTYPE;

#define NUM_MATCHGRPS       4

// N.B.:  Whenever changing the above enum (SC_TYPE),
//   be sure to make a corresponding change to
//   <gSyntaxColors> and <gSyntClrBGTrans> in <externs.h>,
//   <KEYNAME_SC_xxx> and <aColorKeyNames> in <inifile.c>,
//   <#define IDC_SYNTCLR_xxx> in <resource.h>, and
//   <SYNTAX COLORING -- WM_COMMAND> in <CustomizeDlgProc> in <customize.c>.
// If changing the # SC_MATCHGRPn,
//   be sure to make a corresponding change to
//   <aMatchGrp> in <GroupDoneCom> in <tokenize.c>.

typedef struct tagSYNTAX_COLORS
{
    COLORREF crFore,        // 00:  Foreground color
             crBack;        // 04:  Background color (-1 = transparent)
} SYNTAXCOLORS, *LPSYNTAXCOLORS;


//***************************************************************************
//  End of File: syntaxcolors.h
//***************************************************************************
