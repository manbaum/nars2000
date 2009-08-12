//***************************************************************************
//  NARS2000 -- Defined Variables
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2009 Sudley Place Software

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

#ifdef DEBUG
  ////#define DEBUG_REFCNT
#endif

// Default definitions
#define DEF_APLFONT_INTNAME L"SImPL Medium"     // The default font internal name for NARS2000
#define DEF_APLFONT_EXTNAME L"SImPL-Medium"     // ...              external ...
#define DEF_APLFONT_EXT     L".ttf"             // ...                       extension
#define DEF_APLFONT_FILE    DEF_APLFONT_EXTNAME DEF_APLFONT_EXT // ... name w/ext
#define DEF_TCFONTNAME      L"Code2000"         // Default TabCtrl font
#ifndef UNISCRIBE
  #define DEF_ASFONTNAME    L"Code2000"         // Default Alternate SM font
#endif
#define DEF_SMFONTNAME      DEF_APLFONT_INTNAME
#define DEF_PRFONTNAME      DEF_APLFONT_INTNAME
#define DEF_CCFONTNAME      DEF_APLFONT_INTNAME
#define DEF_FEFONTNAME      DEF_APLFONT_INTNAME
#define DEF_MEFONTNAME      DEF_APLFONT_INTNAME
#define DEF_VEFONTNAME      DEF_APLFONT_INTNAME

#define DEF_TCLOGFONT       0,0,0,0,FW_BOLD  ,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,VARIABLE_PITCH | FF_ROMAN ,DEF_TCFONTNAME
#define DEF_SMLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_SMFONTNAME
#define DEF_PRLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_PRFONTNAME
#define DEF_CCLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_CCFONTNAME
#define DEF_FELOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_FEFONTNAME
#define DEF_MELOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_MEFONTNAME
#define DEF_VELOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_VEFONTNAME

#define DEF_TCPTSIZE        9           // Point size for TC font
#define DEF_SMPTSIZE       13           // ...            SM ...
#define DEF_PRPTSIZE       13           // ...            PR ...
#define DEF_CCPTSIZE       13           // ...            CC ...
#define DEF_FEPTSIZE       13           // ...            FE ...
#define DEF_MEPTSIZE       13           // ...            ME ...
#define DEF_VEPTSIZE       13           // ...            VE ...

#define DEF_INDENT          6           // Prompt indent
#define DEF_TABS            4           // Tab insertion
#define DEF_HISTLINES    3000           // # lines in history buffer

// Size of storage areas
#define DEF_NUM_INITNELM        (     1024)                 // Initial NELM for lpszNum
#define DEF_NUM_INCRNELM        (     1024)                 // Incremental ...
#define DEF_NUM_MAXNELM         (   4*1024)                 // Maximum ...
#define DEF_STR_INITNELM        (     1024)                 // Initial NELM for lpwszStr
#define DEF_STR_INCRNELM        (     1024)                 // Incremental ...
#define DEF_STR_MAXNELM         (  64*1024)                 // Maximum ...
////ine DEF_TOKENSTACK_INITNELM (   4*1024)                 // Initial NELM of token stack
////ine DEF_TOKENSTACK_INCRNELM (   4*1024)                 // Incremental ...
////ine DEF_TOKENSTACK_MAXNELM  (  64*1024)                 // Maximum ...
#define DEF_SIS_INITNELM        (   1*1024)                 // Initial NELM for State Indicator Stack
#define DEF_SIS_INCRNELM        (   1*1024)                 // Incremental ..
#define DEF_SIS_MAXNELM         (   4*1024)                 // Maximum ...
#define DEF_YYRES_INITNELM      (   1*1024)                 // Initial NELM of YYRes buffer
#define DEF_YYRES_INCRNELM      (   1*1024)                 // Incremental ...
#define DEF_YYRES_MAXNELM       (  16*1024)                 // Maximum ...
#define DEF_WPTDTEMP_INITNELM   (   4*1024)                 // Initial NELM of WCHAR PTD ...
#define DEF_WPTDTEMP_INCRNELM   (   1*1024)                 // Incremental ...
#define DEF_WPTDTEMP_MAXNELM    ( 256*1024)                 // Maximum ...
#define DEF_WGLBTEMP_INITNELM   (   4*1024)                 // Initial NELM of WCHAR GLB ...
#define DEF_WGLBTEMP_INCRNELM   (   1*1024)                 // Incremental ...
#define DEF_WGLBTEMP_MAXNELM    (  16*1024)                 // Maximum ...
#define DEF_DEBUG_INITNELM      (   1*1024)                 // Initial NELM of debug ...
#define DEF_DEBUG_INCRNELM      (   1*1024)                 // Incremental ...
#define DEF_DEBUG_MAXNELM       (  16*1024)                 // Maximum ...
#define DEF_WFORMAT_INITNELM    (   4*1024)                 // Initial NELM of WCHAR Formatting storage
#define DEF_WFORMAT_INCRNELM    (   4*1024)                 // Incremental ...
#define DEF_WFORMAT_MAXNELM     (2048*1024)                 // Maximum ...
#define DEF_UNDOBUF_INITNELM    (   4*1024)                 // Initial NELM of Undo buffer
#define DEF_UNDOBUF_INCRNELM    (   4*1024)                 // Incremental ...
#define DEF_UNDOBUF_MAXNELM     (  64*1024)                 // Maximum ...
#define DEF_QUADERROR_INITNELM  (   1*1024)                 // Initial NELM of []ERROR/[]ES buffer
#define DEF_QUADERROR_INCRNELM  (   1*1024)                 // Incremental ...
#define DEF_QUADERROR_MAXNELM   (  16*1024)                 // Maximum ...
#define DEF_STRAND_INITNELM     (   1*1024)                 // Initial NELM in tokens of the strand stack
#define DEF_STRAND_INCRNELM     (   1*1024)                 // Incremental ...
#define DEF_STRAND_MAXNELM      (   4*1024)                 // Maximum ...
#define DEF_DISPLAY_INITNELM    (   4*1024)                 // Initial NELM of WCHARs for Array Display
#define DEF_DISPLAY_INCRNELM    (   4*1024)                 // Incremental ...
#ifdef _WIN64
  #define DEF_DISPLAY_MAXNELM (8*1024*1024)                 // Maximum ...
#elif defined (_WIN32)
  #define DEF_DISPLAY_MAXNELM   ( 256*1024)                 // Maximum ...
#else
  #error Need code for this architecture.
#endif
#define DEF_GLBHSHTAB_NBLKS            128                  // Starting # blocks
#define DEF_GLBHSHTAB_EPB                2                  // # entries in each block
#define DEF_GLBHSHTAB_INITNELM  (DEF_GLBHSHTAB_NBLKS * DEF_GLBHSHTAB_EPB)   // Initial NELM of global HshTab for {symbol} names & values
#define DEF_GLBHSHTAB_INCRNELM  (DEF_GLBHSHTAB_INITNELM)    // Incremental ...
#define DEF_GLBHSHTAB_MAXNELM   (1024 * DEF_GLBHSHTAB_EPB)  // Maximum ...
#define DEF_GLBHSHTAB_HASHMASK  (DEF_GLBHSHTAB_NBLKS - 1)   // Starting hash mask
#define DEF_GLBHSHTAB_INCRFREE  (DEF_HSHTAB_PRIME % DEF_GLBHSHTAB_INITNELM)
#define DEF_CS_INITNELM         (   4*1024)                 // Initial NELM of Ctrl Struc token buffer
#define DEF_CS_INCRNELM         (   4*1024)                 // Incremental ...
#define DEF_CS_MAXNELM          (  64*1024)                 // Maximum ...
#define DEF_FORSTMT_INITNELM    (   1*1024)                 // Initial NELM of FORSTMT stack
#define DEF_FORSTMT_INCRNELM    (   1*1024)                 // Incremental ...
#define DEF_FORSTMT_MAXNELM     (   1*1024)                 // Maximum ...


// Global Options
#define DEF_ADJUSTPW                TRUE
#define DEF_UNDERBARTOLOWERCASE     FALSE
#define DEF_NEWTABONCLEAR           TRUE
#define DEF_NEWTABONLOAD            TRUE
#define DEF_USELOCALTIME            TRUE
#define DEF_BACKUPONLOAD            TRUE
#define DEF_BACKUPONSAVE            TRUE
#define DEF_NOCOPYRIGHTMSG          FALSE
#define DEF_DEFAULTPASTE            UNITRANS_NORMAL
#define DEF_DEFAULTCOPY             UNITRANS_NARS
#define DEF_SYNTCLRFCNS             TRUE
#define DEF_SYNTCLRSESS             TRUE
#define DEF_SYNTCLRPRNT             FALSE
#define DEF_CHECKGROUP              TRUE
#define DEF_INSSTATE                TRUE
#define DEF_REVDBLCLK               FALSE
#define DEF_VIEWSTATUSBAR           TRUE


// Range limits for []vars
#define DEF_RANGELIMIT_CT           TRUE
#define DEF_RANGELIMIT_IC           TRUE
#define DEF_RANGELIMIT_IO           TRUE
#define DEF_RANGELIMIT_PP           TRUE
#define DEF_RANGELIMIT_PW           TRUE
#define DEF_RANGELIMIT_RL           TRUE


// Empty assignment to []vars as Fixed System value (TRUE) or .ini file value (FALSE)
#define DEF_RESETVARS_CT            FALSE
#define DEF_RESETVARS_FC            FALSE
#define DEF_RESETVARS_IC            FALSE
#define DEF_RESETVARS_IO            FALSE
#define DEF_RESETVARS_PP            FALSE
#define DEF_RESETVARS_PW            FALSE
#define DEF_RESETVARS_RL            FALSE


// Date/time formats
#define DATETIME_OUT        L"dd/mm/yyyy hh:mm:ss"
#define DATETIME_FMT        L"%02d/%02d/%04d %2d:%02d:%02d"
#define DATETIME_LEN        strcountof (DATETIME_OUT)


#ifdef DEBUG
  #define    APPEND_DEBUG  " (DEBUG)"
  #define WS_APPEND_DEBUG L" (DEBUG)"
#else
  #define    APPEND_DEBUG
  #define WS_APPEND_DEBUG
#endif

#define    APPNAME      "NARS2000"
#define WS_APPNAME     L"NARS2000"

#define WS_WKSNAME  L"workspaces"       // Name of Workspaces subdirectory under main dir

#define LVER_LEGALCOPYRIGHT_YEARS  L"2006-9"
#define LVER_COMPANYNAME_STR       L"Sudley Place Software"
#define LVER_COPYRIGHT1_STR        L"Copyright " WS_UTF16_COPYRIGHT L" " LVER_LEGALCOPYRIGHT_YEARS L" " LVER_COMPANYNAME_STR

#define VER_LEGALCOPYRIGHT_YEARS    "2006-9"
#define VER_COMPANYNAME_STR         "Sudley Place Software"
#define VER_COPYRIGHT1_STR          "Copyright � " VER_LEGALCOPYRIGHT_YEARS " " VER_COMPANYNAME_STR

#define PROP_STATUSMSG             L"StatusMsg"


//***************************************************************************
//  Window Classes
//***************************************************************************

#define  MCWNDCLASS     "MDIClient"     // MDI Client window class
#define LMCWNDCLASS    L"MDIClient"     // MDI Client ...
#define  SMWNDCLASS     "SMClass"       // Session Manager ...
#define LSMWNDCLASS    L"SMClass"       // Session Manager ...
#ifdef DEBUG
  #define  DBWNDCLASS   "DBClass"       // Debugger     ...
  #define LDBWNDCLASS  L"DBClass"       // Debugger     ...
#endif
#ifdef PERFMONON
  #define  PMWNDCLASS   "PMClass"       // Performance Monitoring ...
  #define LPMWNDCLASS  L"PMClass"       // ...
#endif
#define  FEWNDCLASS     "FEClass"       // Function Editor ...
#define LFEWNDCLASS    L"FEClass"       // ...
#define  MEWNDCLASS     "MEClass"       // Matrix Editor ...
#define LMEWNDCLASS    L"MEClass"       // ...
#define  VEWNDCLASS     "VEClass"       // Vector Editor ...
#define LVEWNDCLASS    L"VEClass"       // ...
#define  ECWNDCLASS     "ECClass"       // Edit Ctrl ...
#define LECWNDCLASS    L"ECClass"       // ...
#define  CCWNDCLASS     "CCClass"       // Crash Control ...
#define LCCWNDCLASS    L"CCClass"       // ...

#define DIALOGCLASS    L"#32770"        // Dialog class

#define CQUADPR_MT      L'\xFFFF'       // cQuadPR value indicating it's empty


//***************************************************************************
//  Threads
//***************************************************************************

#define TLSTYPE_DB      ((LPVOID) 'DB')     // Debugger
#define TLSTYPE_IE      ((LPVOID) 'IE')     // Immediate Execution
#define TLSTYPE_MF      ((LPVOID) 'MF')     // Master Frame
#define TLSTYPE_PL      ((LPVOID) 'PL')     // ParseLine
#define TLSTYPE_TC      ((LPVOID) 'TC')     // Tab Control


//***************************************************************************
//  Miscellaneous constants
//***************************************************************************

#ifdef _WIN64
  #define WS_WINSTR     L"Win64"
#elif defined (_WIN32)
  #define WS_WINSTR     L"Win32"
#else
  #error Need code for this architecture.
#endif

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


//***************************************************************************
//  Debugging
//***************************************************************************

#define DbgStop()       __debugbreak ()

#define defstop \
default:        \
    DbgStop();

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


//***************************************************************************
//  Window extra byte offsets
//***************************************************************************

// Define offsets in DBWNDCLASS window extra bytes
#define GWLDB_HWNDLB    0                                           // Window handle of Listbox
#define GWLDB_EXTRA     GWLDB_HWNDLB   + 1 * sizeof (HANDLE_PTR)    // Total # extra bytes

// Define common offset between the Session Manager and Function Editor
#define GWLSF_PERTAB    0                                           // Ptr to PerTabData global memory
#define GWLSF_HWNDEC    GWLSF_PERTAB   + 1 * sizeof (HANDLE_PTR)    // ...           Edit Control window
#define GWLSF_UNDO_BEG  GWLSF_HWNDEC   + 1 * sizeof (HANDLE_PTR)    // ...                beginning
#define GWLSF_UNDO_NXT  GWLSF_UNDO_BEG + 1 * sizeof (HANDLE_PTR)    // ...                next
#define GWLSF_UNDO_LST  GWLSF_UNDO_NXT + 1 * sizeof (HANDLE_PTR)    // ...                last
#define GWLSF_UNDO_GRP  GWLSF_UNDO_LST + 1 * sizeof (HANDLE_PTR)    // Value of next Undo group index
#define GWLSF_LASTKEY   GWLSF_UNDO_GRP + 1 * sizeof (long)          // Value of last WM_KEYDOWN key
#define GWLSF_CHANGED   GWLSF_LASTKEY  + 1 * sizeof (long)          // Boolean of whether or not the text has changed
#define GWLSF_LPMVS     GWLSF_CHANGED  + 1 * sizeof (long)          // Ptr to LPMEMVIRTSTR

// Define offsets in SMWNDCLASS window extra bytes
#define GWLSM_EXTRA     GWLSF_LPMVS    + 1 * sizeof (HANDLE_PTR)    // Total # extra bytes

// Define offsets in FEWNDCLASS window extra bytes
#define GWLFE_HWNDPRV   GWLSF_LPMVS    + 1 * sizeof (HANDLE_PTR)    // Next window handle in linked list (NULL = none)
#define GWLFE_HWNDNXT   GWLFE_HWNDPRV  + 1 * sizeof (HANDLE_PTR)    // Previous ...
#define GWLFE_EXTRA     GWLFE_HWNDNXT  + 1 * sizeof (HANDLE_PTR)    // Total # extra bytes

// Define offsets in MEWNDCLASS window extra bytes
#define GWLME_EXTRA     0                                           // Total # extra bytes

// Define offsets in PMWNDCLASS window extra bytes
#define GWLPM_HWNDLB    0                                           // Window handle of Listbox
#define GWLPM_EXTRA     GWLPM_HWNDLB   + 1 * sizeof (HANDLE_PTR)    // Total # extra bytes

// Define offsets in VEWNDCLASS window extra bytes
#define GWLVE_EXTRA     0                                           // Total # extra bytes

// Define offsets in CCWNDCLASS window extra bytes
#define GWLCC_EXTRA     0                                           // Total # extra bytes

// Define offsets in WCWNDCLASS window extra bytes
#define GWLWC_EXTRA     0                                           // Total # extra bytes


//***************************************************************************
//  Local window messages
//***************************************************************************

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
#define MYWM_RESIZE         (WM_APP +12)    // MF (Resize to display Status Bar)
#define MYWM_NOTIFY         (WM_APP +13)    // EC (Pass on of WM_NOTIFY from EC)
#define MYWM_CMPNAME        (WM_APP +14)    // FE (Compare function names)
#define MYWM_RBUTTONDBLCLK  (WM_APP +15)    // FE/SM (Right-double-click)

// Define Debug window messages
#define MYWM_INIT_DB        (WM_APP +50)    // DB
#define MYWM_DBGMSGA        (WM_APP +51)    // DB
#define MYWM_DBGMSGW        (WM_APP +52)    // DB
#define MYWM_DBGMSG_CLR     (WM_APP +53)    // DB
#define MYWM_DBGMSG_SCROLL  (WM_APP +54)    // DB
#define MYWM_UNHOOK         (WM_APP +55)    // DB


//***************************************************************************
//  Character constants -- ASCII and Wide characters and strings
//***************************************************************************

#define AC_EOS          '\0'        // 00:  End-of-string
#define AC_LF           '\n'        // 0A:  Linefeed
#define AC_CR           '\r'        // 0D:  Carriage Return
#define AC_BLANK        ' '         // 20:  Space
#define AC_LEFTPAREN    '('         // 28:  Left Paren
#define AC_STAR         '*'         // 2A:  Asterisk
#define AC_X            'X'         // 58:  Capital X
#define AC_SLOPE        '\\'        // 5C:  Slope
#define AC_CRLF         '\r\n'      // 0D0A:  Carriage Return Linefeed

#define AS_LF           "\n"        // 0A:  Linefeed
#define AS_CR           "\r"        // 0D:  Carriage Return
#define AS_CRLF         "\r\n"      // 0D0A:  Carriage Return Linefeed
#define EOL_LEN         2           // Length of EOL ("\r\n")

#define WC_EOS          L'\0'       // 00:  End-of-string
#define WC_HT           L'\t'       // 09:  Horizontal Tab
#define WC_LF           L'\n'       // 0A:  Linefeed
#define WC_CR           L'\r'       // 0D:  Carriage Return
#define WC_DQ           L'\"'       // 22:  Double Quote
#define WC_SQ           L'\''       // 27:  Single Quote
#define WC_SLOPE        L'\\'       // 5C:  Slope

#define WS_HT           L"\t"       // 09:  Horizontal Tab
#define WS_LF           L"\n"       // 0A:  Linefeed
#define WS_CR           L"\r"       // 0D:  Carriage Return
#define WS_DQ           L"\""       // 22:  Double Quote
#define WS_SQ           L"\'"       // 27:  Single Quote
#define WS_SLOPE        L"\\"       // 5C:  Slope

#define WS_CRLF         L"\r\n"     // 0D0A:  CR/LF


//***************************************************************************
// Wide-char routines From <string.h>
//***************************************************************************

#define strchrW         wcschr
#define strncmpW        wcsncmp
#define strpbrkW        wcspbrk
#define strspnW         wcsspn


//***************************************************************************
//  Image list constants
//***************************************************************************

// Width and height of each image in the image list
#define IMAGE_CX        16
#define IMAGE_CY        16


//***************************************************************************
//  Bit, byte, etc. constants
//***************************************************************************

// Define bit masks
#define BIT0    0x01
#define BIT1    0x02
#define BIT2    0x04
#define BIT3    0x08
#define BIT4    0x10
#define BIT5    0x20
#define BIT6    0x40
#define BIT7    0x80

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


//***************************************************************************
//  WM_PRINTCLIENT flags
//***************************************************************************

// Extra flags for WM_PRINTCLIENT to indicate ...
#define PRF_PRINTCLIENT 0x80000000L     // called from WM_PRINTCLIENT
#define PRF_SELECTION   0x40000000L     // to print the selection
#define PRF_CURRENTPAGE 0x20000000L     // to print the current page


//***************************************************************************
// Resource debugging size
//***************************************************************************

#define MAXOBJ  128000



//***************************************************************************
//  End of File: defines.h
//***************************************************************************
