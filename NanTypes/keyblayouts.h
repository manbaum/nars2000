//***************************************************************************
//  NARS2000 -- Keyboard Layouts
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


#ifndef KEYLAYOUTS_DEFINED

typedef unsigned int   UBOOL, *LPUBOOL;

typedef struct tagCHARCODE
{
    WCHAR wc[9];    // 000-111:  Shift- Ctrl- Alt-keys and Dead Key
////WCHAR u   ,     // 000:  No Shift- Ctrl- Alt-keys pressed
////      a   ,     // 001:  Alt-key pressed
////      c   ,     // 010:  Ctrl-key pressed
////      ac  ,     // 011:  Alt- Ctrl- keys pressed
////      s   ,     // 100:  Shift-key pressed
////      sa  ,     // 101:  Shift- Alt-keys pressed
////      sc  ,     // 110:  Shift- Ctrl-keys pressed
////      sca ;     // 111:  Shift- Ctrl- Alt-keys pressed
    UBOOL dk[8];    // TRUE iff this scan code in this keyb state is a dead key
} CHARCODE, *LPCHARCODE;

#define KBLEN                   128     // Length of a Keyboard Layout name

typedef struct tagKEYBLAYOUTS
{
    CHARCODE   aCharCodes[0x80];        // 0000:  aCharCodes table (0x80 x 8 x 2)
    UINT       uCharCodesLen;           // 0800:  # actual rows used in the table aCharCodes (<= 0x80)
    WCHAR      wszLayoutName[KBLEN];    // 0804:  Keyboard Layout Name
    UINT       uScanCode2B_RowNum;      // 0884:  Row # (Origin-0) of ScanCode 2B (Backslash & Stile):  1 or 2
    UBOOL      bExtraKeyRow3:1,         // 0888:  00000001:  TRUE iff there is an extra key on the left of row #3
               bUseCXV:1,               //        00000002:  TRUE iff this keyb layout uses Ctrl-C, -X, -V for Copy, Cut, Paste
               bUseZY:1,                //        00000004:  TRUE iff ...                   Ctrl-Z, -Y     for Undo, Redo
               bUseSEQ:1,               //        00000008:  TRUE iff ...                   Ctrl-S, -E, -Q for function editing commands
               bUseAltGR:1,             //        00000010:  TRUE iff ...                   AltGR as an  alias for pressing the Ctrl- and Alt-keys
               bReadOnly:1,             //        00000020:  TRUE iff the layout is read-only (i.e. built-in)
               :26;                     //        FFFFFFC0:  Available bits
                                        // 088C:  Length
} KEYBLAYOUTS, *LPKEYBLAYOUTS;

typedef struct tagKEYBLAYOUTS_BI
{
    LPCHARCODE lpCharCodes;             // 0000:  Ptr to aCharCodesxxx for built-in layouts
    UINT       uCharCodesLen;           // 0004:  # actual rows used in the table pointed to by lpCharCodes
    WCHAR      wszLayoutName[KBLEN];    // 0008:  Keyboard Layout Name
    UINT       uScanCode2B_RowNum;      // 0088:  Row # (Origin-0) of ScanCode 2B (Backslash & Stile):  1 or 2
    UBOOL      bExtraKeyRow3:1,         // 008C:  00000001:  TRUE iff there is an extra key on the left of row #3
               bUseCXV:1,               //        00000002:  TRUE iff this keyb layout uses Ctrl-C, -X, -V for Copy, Cut, Paste
               bUseZY:1,                //        00000004:  TRUE iff ...              uses Ctrl-Z, -Y     for Undo, Redo
               bUseSEQ:1,               //        00000008:  TRUE iff ...                   Ctrl-S, -E, -Q for function editing commands
               bUseAltGR:1,             //        00000010:  TRUE iff ...                   AltGR as an  alias for pressing the Ctrl- and Alt-keys
               :27;                     //        FFFFFFE0:  Available bits
                                        // 0090:  Length
} KEYBLAYOUTS_BI, *LPKEYBLAYOUTS_BI;

  #define KEYLAYOUTS_DEFINED
#endif


//***************************************************************************
//  End of File: keyblayouts.h
//***************************************************************************
