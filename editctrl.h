//***************************************************************************
//  NARS2000 -- Edit Control Header
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

#define EM_SETPAINTHOOK     0x00BF

#define GWLEC_ES            0
#define GWLEC_PAINTHOOK     GWLEC_ES        + 1 * sizeof(long)
#define GWLEC_EXTRA         GWLEC_PAINTHOOK + 1 * sizeof(long)


typedef int PAINTHOOK (HWND, HDC, int, int, LPWSTR, int, int);
typedef PAINTHOOK *LPPAINTHOOK;

#define MYWM_INIT_EC        (WM_USER + 0)
#define MYWM_REDO           (WM_USER + 1)
#define MYWM_SELECTALL      (WM_USER + 2)
#define MYWM_KEYDOWN        (WM_USER + 3)
#define MYWM_PASTE_APLWIN   (WM_USER + 4)
#define MYWM_PASTE_APL2     (WM_USER + 5)
#define MYWM_PASTE_ISO      (WM_USER + 6)

#define FCN_INDENT  6

#define WM_REDO     (WM_USER + 0)

#define GWL_ID              (-12)

typedef struct tagNMEDITCTRL
{
    NMHDR  nmHdr;           // 00:  Initial struc
    UINT   cbSize;          // 0C:  Byte size of NMCARETWIDTH struc
    UINT  *lpCaretWidth;    // 10:  Ptr to default caret width
                            // 14:  Length
} NMEDITCTRL, *LPNMEDITCTRL;


//***************************************************************************
//  End of File: editctrl.h
//***************************************************************************
