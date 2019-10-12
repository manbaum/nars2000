//***************************************************************************
//  NARS2000 -- Name Classes
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

#ifndef NAMECLASS_H
#define NAMECLASS_H

// Name classes used in []NC and []NL
typedef enum tagNAME_CLASS
{
    NAMECLASS_INV = -1,     // -1 = Invalid name or unknown sysname
    NAMECLASS_AVL = 0,      //  0 = Available name
    NAMECLASS_USRLBL,       //  1 = User label
    NAMECLASS_USRVAR,       //  2 = User variable
    NAMECLASS_USRFCN,       //  3 = User-defined function   (any # args:  0, 1, or 2)
    NAMECLASS_USROPR,       //  4 = User-defined operator   (either # operands:  1 or 2)
    NAMECLASS_SYSVAR,       //  5 = System variable
    NAMECLASS_SYSFCN,       //  6 = System function         (any # args:  0, 1, or 2)
    NAMECLASS_SYSOPR,       //  7 = System operator         (either # operands:  1 or 2)
    NAMECLASS_UNUSED1,      //  8 = Start of unused area
    NAMECLASS_UNUSED2 = 20, // 20 = End   ...
    NAMECLASS_SYSLBL,       // 21 = System label
    NAMECLASS_UNUSED3,      // 22 = (Unused)
    NAMECLASS_MAGFCN,       // 23 = Magic function          (any # args:  0, 1, or 2)
    NAMECLASS_MAGOPR,       // 24 = Magic operator          (either # operands:  1 or 2)
    NAMECLASS_LENp1,        // 25 = # valid entries + 1
} NAME_CLASS;
//  Note that the left shifts (BIT0 <<) in <SysFnDydNL_EM_YY>
//    assume that the name class values are <= 63.  If you add
//    nameclasses to invalidate that assumption, be sure to
//    make <SysFnDydNL_EM_YY> work, too.

// N.B.:  Whenever changing the above enum (NAME_CLASS),
//   be sure to make a corresponding change to
//   <bNameClassValid> in <qf_nl.c>.

#endif  /* NAMECLASS_H */


//***************************************************************************
//  End of File: nameclass.h
//***************************************************************************
