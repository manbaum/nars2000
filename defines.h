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
#define DEF_CURWID_INS      5           // Cursor width for insert mode
#define DEF_CURWID_REP      2           // ...              replace ...
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
#define DEF_DISPLAY_MAXNELM     ( 256*1024)                 // Maximum ...
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
#define DEF_CHECKGROUP              TRUE
#define DEF_INSSTATE                TRUE
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
#define VER_COPYRIGHT1_STR          "Copyright © " VER_LEGALCOPYRIGHT_YEARS " " VER_COMPANYNAME_STR

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
#define  WCWNDCLASS     "WCClass"       // Web Color Names  ...
#define LWCWNDCLASS    L"WCClass"       // ...

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
//  End of File: defines.h
//***************************************************************************
