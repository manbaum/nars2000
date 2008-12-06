//****************************************************************************
//  NARS2000 -- Main Header File
//****************************************************************************

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

// Disable stack checking
#pragma check_stack(off)

#include "macros.h"

#define EQ ==
#define NE !=

#define TRUE    1
#define FALSE   0
#define NEG1U   (   (UINT) -1)
#define NEG1A   ((APLUINT) -1)

#define LOPART_DWORDLONG    ((DWORDLONG) 0x00000000FFFFFFFF)
#define HIPART_DWORDLONG    ((DWORDLONG) 0xFFFFFFFF00000000)

#define QWORD   ULONGLONG

#define PAGESIZE        (4 * 1024)

#define defstop \
default:        \
    DbgStop();  \
    nop ();

#define FNLN    FileNameOnly (__FILE__), __LINE__

#ifdef DEBUG
  #define LCLODS        ODS
  #define LCLODSDD      ODSDD
  #define LCLODSRECT    ODSRECT
  #define LCLODSSIZE    ODSSIZE
  #define LCLODSAPI     ODSAPI

  #define DBGENTER      if (gDbgLvl > 2) {DbgMsgW (L"Entering" APPEND_NAME);}
  #define DBGLEAVE      if (gDbgLvl > 2) {DbgMsgW (L"Leaving " APPEND_NAME);}
#else
  #define LCLODS
  #define LCLODSDD
  #define LCLODSRECT
  #define LCLODSSIZE
  #define LCLODSAPI

  #define DBGENTER
  #define DBGLEAVE
#endif


//*************************** Window Data ********************************

// Define offsets in DBWNDCLASS window extra bytes
#define GWLDB_HWNDLB    0                                           // Window handle of Listbox
#define GWLDB_EXTRA     GWLDB_HWNDLB   + 1 * sizeof (long)          // Total # extra bytes

// Define common offset between the Session Manager and Function Editor
#define GWLSF_PERTAB    0                                           // HGLOBAL of corresponding PERTABDATA struc
#define GWLSF_HWNDEC    GWLSF_PERTAB   + 1 * sizeof (HANDLE_PTR)    // Handle of the matching Edit control
#define GWLSF_UNDO_INI  GWLSF_HWNDEC   + 1 * sizeof (HANDLE_PTR)    // Ptr to Undo stack, initial (as allocated)
#define GWLSF_UNDO_BEG  GWLSF_UNDO_INI + 1 * sizeof (HANDLE_PTR)    // ...                beginning
#define GWLSF_UNDO_NXT  GWLSF_UNDO_BEG + 1 * sizeof (HANDLE_PTR)    // ...                next
#define GWLSF_UNDO_LST  GWLSF_UNDO_NXT + 1 * sizeof (HANDLE_PTR)    // ...                last
#define GWLSF_UNDO_GRP  GWLSF_UNDO_LST + 1 * sizeof (HANDLE_PTR)    // Value of next Undo group index
#define GWLSF_VKSTATE   GWLSF_UNDO_GRP + 1 * sizeof (long)          // Virtal Key state (VKSTATE struc)
#define GWLSF_LASTKEY   GWLSF_VKSTATE  + 1 * sizeof (long)          // Value of last WM_KEYDOWN key
#define GWLSF_CHANGED   GWLSF_LASTKEY  + 1 * sizeof (long)          // Boolean of whether or not the text has changed
#define GWLSF_LPMVS     GWLSF_CHANGED  + 1 * sizeof (long)          // Ptr to LPMEMVIRTSTR

// Define offsets in SMWNDCLASS window extra bytes
#define GWLSM_EXTRA     GWLSF_LPMVS    + 1 * sizeof (HANDLE_PTR)    // Total # extra bytes

// Define offsets in FEWNDCLASS window extra bytes
#define GWLFE_EXTRA     GWLSF_LPMVS    + 1 * sizeof (HANDLE_PTR)    // Total # extra bytes

// Define offsets in MEWNDCLASS window extra bytes
#define GWLME_EXTRA     0                                           // Total # extra bytes

// Define offsets in PMWNDCLASS window extra bytes
#define GWLPM_HWNDLB    0                                           // Window handle of Listbox
#define GWLPM_EXTRA     GWLPM_HWNDLB   + 1 * sizeof (long)          // Total # extra bytes

// Define offsets in VEWNDCLASS window extra bytes
#define GWLVE_EXTRA     0                                           // Total # extra bytes

// Define offsets in CCWNDCLASS window extra bytes
#define GWLCC_EXTRA     0                                           // Total # extra bytes

// Define offsets in WCWNDCLASS window extra bytes
#define GWLWC_EXTRA     0                                           // Total # extra bytes


// Define local window messages
#define MYWM_MOVE           (WM_APP + 0)    // MF
#define MYWM_SETFOCUS       (WM_APP + 1)    // SM (SetFocus)
#define MYWM_IZITNAME       (WM_APP + 2)    // FE (Izit A Name)
#define MYWM_SAVE_FN        (WM_APP + 3)    // FE (SaveFunction)
#define MYWM_SAVECLOSE_FN   (WM_APP + 4)    // FE (SaveFunction/CloseFunction)
#define MYWM_SAVE_AS_FN     (WM_APP + 5)    // FE (SaveAsFunction)
#define MYWM_CLOSE_FN       (WM_APP + 6)    // FE (CloseFunction)
#define MYWM_QUOTEQUAD      (WM_APP + 7)    // PL (Quote-Quad/Quad Input)
#define MYWM_INIT_SMDB      (WM_APP + 8)    // SM (Initialize SM/DB windows)
#define MYWM_ERRMSG         (WM_APP + 9)    // SM (Display error message)
#define MYWM_SAVE_WS        (WM_APP +10)    // SM (Save workspace)
#define MYWM_DISPMB         (WM_APP +11)    // CC (Display MessageBox)

// Define Debug window messages
#define MYWM_INIT_DB        (WM_APP +12)    // DB
#define MYWM_DBGMSGA        (WM_APP +13)    // DB
#define MYWM_DBGMSGW        (WM_APP +14)    // DB
#define MYWM_DBGMSG_CLR     (WM_APP +15)    // DB
#define MYWM_DBGMSG_SCROLL  (WM_APP +16)    // DB
#define MYWM_UNHOOK         (WM_APP +17)    // DB


typedef enum tagEXCEPTION_CODES // Exception Codes
{
    EXCEPTION_SUCCESS = 0 ,     // 00:  All OK
////EXCEPTION_RESULT_BOOL ,     //      Result should be Boolean
////EXCEPTION_RESULT_INT  ,     //      ...              Integer
    EXCEPTION_RESULT_FLOAT,     // 01:  ...              Float
    EXCEPTION_DOMAIN_ERROR,     // 02:  Signal a DOMAIN ERROR
    EXCEPTION_LIMIT_ERROR,      // 03:  Signal a LIMIT ERROR
    EXCEPTION_CTRL_BREAK,       // 04:  Ctrl-Break pressed
} EXCEPTION_CODES;

typedef struct tagGLBHIST
{
    HGLOBAL *hGlb;              // 00:  Ptr to HGLOBAL
    UINT    First:1,            // 04:  00000001:  This line is the first one in the array
            ContPrev:1,         //      00000002:  This line is connected to the previous line
            ContNext:1,         //      00000004:  ...                           next     ...
            Avail:29;           //      FFFFFFF8:  Available bits
                                // 08:  Length
} GLBHIST, *LPGLBHIST;

typedef struct tagVKSTATE
{
    UINT  Shift:1,              // 00000001:  Left- or right-shift key up(0) or down(1)
////     lShift:1,
////     rShift:1,
          Alt:1,                // 00000002:  Left- or right-Alt key up(0) or down(1)
////     lAlt:1,
////     rAlt:1,
          Ctl:1,                // 00000004:  Left or -right Ctl key up(0) or down(1)
////     lCtl:1,
////     rCtl:1,
          Ins:1,                // 00000008:  Replace(0) or insert(1)
          Avail:28;             // FFFFFFF0:  Available bits
} VKSTATE, *LPVKSTATE;

typedef enum tagMAKE_PROTO
{
    MP_CHARS,                   // 00:  Chars allowed in MakePrototype arg
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
                                // 0D-0F:  Available entries (4 bits)
} EXIT_TYPES, *LPEXITTYPES;

// Resource debugging
#define MAXOBJ  128000

// Define bit masks
#define BIT0    0x00000001
#define BIT1    0x00000010
#define BIT2    0x00000100
#define BIT3    0x00001000
#define BIT4    0x00010000
#define BIT5    0x00100000
#define BIT6    0x01000000
#define BIT7    0x10000000

// # bits in a byte
#define NBIB            8

// Log base 2 of NBIB
#define LOG2NBIB        3

// Mask for LOG2NBIB bits
#define MASKLOG2NBIB    ((BIT0 << LOG2NBIB) - 1)    // a.k.a. (NBIB - 1)

// # bits in a word
#define NBIW           16

// Log base 2 of NBIW
#define LOG2NBIW        4

// Mask for LOG2NBIW bits
#define MASKLOG2NBIW    ((BIT0 << LOG2NBIW) - 1)    // a.k.a. (NBIW - 1)

// # bits in a dword
#define NBID           32

// Log base 2 of NBID
#define LOG2NBID        5

// Mask for LOG2NBID bits
#define MASKLOG2NBID    ((BIT0 << LOG2NBID) - 1)    // a.k.a. (NBID - 1)

// # bits in a qword
#define NBIQ           64

// Log base 2 of NBIQ
#define LOG2NBIQ        6

// Mask for LOG2NBIQ bits
#define MASKLOG2NBIQ    ((BIT0 << LOG2NBIQ) - 1)    // a.k.a. (NBIQ - 1)

// End value for shift mask
#define END_OF_BYTE     (BIT0 << NBIB)

// Width and height of each image in the image list
#define IMAGE_CX        16
#define IMAGE_CY        16

// From <string.h>
#define strchrW         wcschr
#define strncmpW        wcsncmp

// Default DTOA mode    // ***FIXME*** -- use different modes at different points
#define DEF_DTOA_MODE   2

// Extra flags for WM_PRINTCLIENT to indicate ...
#define PRF_PRINTCLIENT 0x80000000L     // called from WM_PRINTCLIENT
#define PRF_SELECTION   0x40000000L     // to print the selection
#define PRF_CURRENTPAGE 0x20000000L     // to print the current page

// Define UBOOL as an unsigned int
typedef unsigned int UBOOL, *LPUBOOL;


//***************************************************************************
//  End of File: main.h
//***************************************************************************
