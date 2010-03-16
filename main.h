//****************************************************************************
//  NARS2000 -- Main Header File
//****************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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

// Disable stack checking
#pragma check_stack(off)


typedef enum tagEXCEPTION_CODES // Exception Codes
{
    EXCEPTION_SUCCESS = 0 ,     // 00:  All OK
    EXCEPTION_RESULT_FLOAT,     // 01:  Result should be Float
    EXCEPTION_DOMAIN_ERROR,     // 02:  Signal a DOMAIN ERROR
    EXCEPTION_LIMIT_ERROR,      // 03:  Signal a LIMIT ERROR
    EXCEPTION_CTRL_BREAK,       // 04:  Ctrl-Break pressed
} EXCEPTION_CODES;

typedef struct tagVKSTATE
{
    UINT  Shift:1,              // 00000001:  Left- or right-shift key up(0) or down(1)
          Alt:1,                // 00000002:  Left- or right-Alt key up(0) or down(1)
          Ctl:1,                // 00000004:  Left or -right Ctl key up(0) or down(1)
          Ins:1,                // 00000008:  Replace(0) or insert(1)
          :28;                  // FFFFFFF0:  Available bits
} VKSTATE, *LPVKSTATE;

typedef enum tagMAKE_PROTO
{
    MP_CHARS,                   // 00:  Chars allowed in MakeMonPrototype arg
    MP_NUMONLY,                 // 01:  Numerics only ...
    MP_NUMCONV                  // 02:  Convert chars to numerics ...
} MAKE_PROTO;

typedef enum tagEXIT_TYPES
{
    EXITTYPE_NONE = 0,          // 00:  Undefined
    EXITTYPE_GOTO_ZILDE,        // 01:  {goto} {zilde}
    EXITTYPE_GOTO_LINE,         // 02:  {goto} LineNum
    EXITTYPE_RESET_ONE,         // 03:  {goto}
    EXITTYPE_RESET_ONE_INIT,    // 04:  {goto}  (first time)
    EXITTYPE_RESET_ALL,         // 05:  )RESET
    EXITTYPE_QUADERROR_INIT,    // 06:  []ERROR/[]ES -- initialization
    EXITTYPE_QUADERROR_EXEC,    // 07:  []ERROR/[]ES -- execute []ELX
    EXITTYPE_ERROR,             // 08:  ERROR
    EXITTYPE_STOP,              // 09:  Stop processing, cannot continue
    EXITTYPE_DISPLAY,           // 0A:  Value not already displayed
    EXITTYPE_NODISPLAY,         // 0B:  Value already displayed
    EXITTYPE_NOVALUE,           // 0C:  No value returned
    EXITTYPE_RETURNxLX,         // 0D:  Return from []ELX/[]ALX
                                // 0E-0F:  Available entries (4 bits)
} EXIT_TYPES, *LPEXITTYPES;

// Define UBOOL as an unsigned int
typedef unsigned int UBOOL, *LPUBOOL;


//***************************************************************************
//  End of File: main.h
//***************************************************************************
