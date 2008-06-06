//***************************************************************************
//  NARS2000 -- Workspace Header
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

// Section names
#define SECTNAME_GENERAL                L"General"
#define SECTNAME_GLOBALS                L"Globals"
#define SECTNAME_VARS                   L"Vars"
#define SECTNAME_FCNS                   L"Fcns"
#define SECTNAME_SI                     L"SI"

// Key names
#define KEYNAME_COUNT                   L"Count"
#define KEYNAME_VERSION                 L"Version"
#define KEYNAME_UNDO                    L"Undo"
#define KEYNAME_SILEVEL                 L"SILevel"
#define KEYNAME_CREATIONTIME            L"CreationTime"
#define KEYNAME_LASTMODTIME             L"LastModTime"
#define KEYNAME_USERDEFINED             L"UserDefined"

// Format string for [Globals] section keyname
#define FMTCHR_LEAD             L':'
#define FMTSTR_LEAD             L":"
#define FMTSTR_GLBCNT           FMTSTR_LEAD L"%u"
#define FMTSTR_GLBOBJ           FMTSTR_LEAD L"%p"
#define FMTSTR_DATETIME         L"%08X%08X"


typedef struct tagSAVEDWSGLBVARPARM
{
    LPAPLCHAR   lpMemSaveWSID;          // Ptr to saved WS file DPFE
    LPUINT      lpGlbCnt;               // Ptr to [Globals] count
    LPSYMENTRY  lpSymEntry;             // Ptr to this global's SYMENTRY
    LPSYMENTRY *lplpSymLink;            // Ptr to ptr to SYMENTRY link
} SAVEDWSGLBVARPARM, *LPSAVEDWSGLBVARPARM;

typedef LPAPLCHAR (*LPSAVEDWSGLBVARCONV) (LPAPLCHAR, HGLOBAL, LPSAVEDWSGLBVARPARM);


typedef struct tagLOADWSGLBVARPARM
{
    LPWCHAR     lpwSrc;                 // Ptr to next available byte
    UINT        uMaxSize;               // Maximum size of lpwSrc
    HWND        hWndEC;                 // Edit Control window handle
    LPSYMENTRY *lplpSymLink;            // Ptr to ptr to SYMENTRY link
    LPAPLCHAR   lpwszDPFE;              // Ptr to saved WS file DPFE
    LPWCHAR    *lplpwErrMsg;            // Ptr to ptr to (constant) error message text
} LOADWSGLBVARPARM, *LPLOADWSGLBVARPARM;

typedef HGLOBAL (*LPLOADWSGLBVARCONV) (UINT, LPLOADWSGLBVARPARM);


//***************************************************************************
//  End of File: workspace.h
//***************************************************************************
