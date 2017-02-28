//***************************************************************************
//  NARS2000 -- Keyboard Variables
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
//  Keyboards and Character Codes
//***************************************************************************

typedef struct tagKEYDATA
{
    UINT repeatCount:16,            // Repeat count
         scanCode:8,                // Scan code
         extendedKey:1,             // TRUE iff extended key (right-Alt, Right-Ctl)
         reserved:4,                // Reserved
         contextCode:1,             // TRUE iff Alt key is down while key is pressed
         previousState:1,           // TRUE iff key is down before msg sent
         transitionState:1;         // TRUE iff key is being released
} KEYDATA, *LPKEYDATA;


// Keyboard accelerators
//   Not used with Customize Dialog for keyboards but left in to
//     to see how it's done.
EXTERN
HANDLE hAccel
#ifdef DEFINE_VALUES
= NULL
#endif
;

typedef struct tagVKSTATE
{
    UINT  Shift:1,                  // 00000001:  Left- or right-shift key up(0) or down(1)
          Alt:1,                    // 00000002:  Left- or right-Alt key up(0) or down(1)
          Ctl:1,                    // 00000004:  Left or -right Ctl key up(0) or down(1)
          Ins:1,                    // 00000008:  Replace(0) or insert(1)
          :28;                      // FFFFFFF0:  Available bits
} VKSTATE, *LPVKSTATE;

#include "keyblayouts.h"            // To define <KEYBLAYOUTS> struc

EXTERN
KEYBLAYOUTS aKeybLayoutAct;         // Active keyboard layout

EXTERN
UINT uGlbKeybLayoutNumAct,          // # of active keyboard layout
     uGlbKeybLayoutNumVis,          // # of visible ...
     uGlbKeybLayoutCount,           // Total # keyboard layouts (built-in + user-defined)
     uGlbKeybLayoutUser,            // # user-defined keyboard layouts
     uKeybUnibase,                  // Keyboard Unicode base:  10 or 16 for Current Char
     uKeybChar,                     // Current Character
     uKeybTCNum,                    // Initial keyboard TabCtrl index
     uKeybState;                    // Initial keyboard state:  0 to 7 (3 bits, Shift(4), Ctrl(2), Alt(1))

EXTERN
HGLOBAL hGlbKeybLayouts;            // Keyboard layout global memory handle
                                    // The contents of this handle is an array of KEYBLAYOUTS strucs
EXTERN
WCHAR wszGlbKeybLayoutAct[KBLEN],   // Active global keyboard layout name
      wszGlbKeybLayoutVis[KBLEN];   // Visible .....

EXTERN
LOGFONTW lfKB                       // LOGFONTW for the Keyboard
#ifdef DEFINE_VALUES
 = {DEF_KBLOGFONT}
#endif
;


//***************************************************************************
//  End of File: keyboards.h
//***************************************************************************
