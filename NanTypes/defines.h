//***************************************************************************
//  NARS2000 -- Defined Variables
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

#if _WIN32_WINNT < 0x0600   // From BASETSD.H

#define MAXUINT64   ((UINT64)~((UINT64)0))
#define MAXINT64    ((INT64)(MAXUINT64 >> 1))
#define MININT64    ((INT64)~MAXINT64)

#endif      // _WIN32_WINNT < 0x0600

#ifdef DEBUG
  ////#define DEBUG_REFCNT
  #define    UTF16_REFCNT_NE1      UTF16_REPLACEMENTCHAR
  #define WS_UTF16_REFCNT_NE1   WS_UTF16_REPLACEMENTCHAR
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
#define DEF_FBFONTNAME      L"Fallback00-1F"
#define DEF_LWFONTNAME      DEF_APLFONT_INTNAME
#define DEF_PRFONTNAME      DEF_APLFONT_INTNAME
#define DEF_CCFONTNAME      DEF_APLFONT_INTNAME
#define DEF_FEFONTNAME      DEF_APLFONT_INTNAME

#define DEF_TCLOGFONT       0,0,0,0,FW_BOLD  ,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,VARIABLE_PITCH | FF_ROMAN ,DEF_TCFONTNAME
#define DEF_SMLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_SMFONTNAME
#define DEF_FBLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_FBFONTNAME
#define DEF_LWLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_LWFONTNAME
#define DEF_PRLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_PRFONTNAME
#define DEF_CCLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_CCFONTNAME
#define DEF_FELOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_FEFONTNAME

#define DEF_KBFONTNAME      DEF_APLFONT_INTNAME
#define DEF_KBLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_KBFONTNAME

#define DEF_TCPTSIZE        9           // Point size for TC font
#define DEF_SMPTSIZE       13           // ...            SM ...
#define DEF_KBPTSIZE       13           // ...            KB ...
#define DEF_FBPTSIZE       13           // ...            FB ...
#define DEF_LWPTSIZE       13           // ...            LW ...
#define DEF_PRPTSIZE       13           // ...            PR ...
#define DEF_CCPTSIZE       13           // ...            CC ...
#define DEF_FEPTSIZE       13           // ...            FE ...

#define SampleText      L"(" WS_UTF16_IOTA L"V)" WS_UTF16_EQUALUNDERBAR WS_UTF16_RIGHTSHOE WS_UTF16_JOT L".,/" WS_UTF16_IOTA WS_UTF16_DIERESIS L"V"

#define DEF_INDENT          6           // Prompt indent
#define DEF_TABS            4           // Tab insertion
#define DEF_RECENTFILES    15           // # entries in Recent Files
#define DEF_RATSEP         'r'          // Default rational # separator
#define DEF_RATSEP_WS     L"r"          // ...
#define DEF_VFPSEP         'v'          // ...     VFP        ...
#define DEF_VFPSEP_WS     L"v"          // ...
#define DEF_ARBSEP         '±'          // ...     ARB        ...
#define DEF_ARBSEP_S       "±"          // ...
#define DEF_ARBSEP_W      L'±'          // ...
#define DEF_ARBSEP_WS     L"±"          // ...
#define DEF_HC2SEP        L'J'          // ...     HC2
#define DEF_HC2SEP_WS     L"J"          // ...
#define DEF_HC4SEP        L"i",     \
                          L"j",     \
                          L"k"          // ...     Quaternion ...
#define DEF_HC8SEPDEF     L"i",     \
                          L"j",     \
                          L"k",     \
                          L"l",     \
                          L"ij",    \
                          L"jk",    \
                          L"kl"         // ...     Octonion Default
#define DEF_HC8SEPDIG     L"i",     \
                          L"j",     \
                          L"k",     \
                          L"os",    \
                          L"oi",    \
                          L"oj",    \
                          L"ok"         // ...     Octonion Digraphs
#define PFMN            &PrimFnMonNonceError_RE
#define PFDN            &PrimFnDydNonceError_RE
#define NONCE_RE        RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

// Size of storage areas
#define DEF_NUM_INITNELM        (     1024)                 // Initial NELM for lpszNum
#define DEF_NUM_INCRNELM        (     1024)                 // Incremental ...
////ine DEF_NUM_MAXNELM         (   4*1024)                 // Maximum ...
#define DEF_STR_INITNELM        (     1024)                 // Initial NELM for lpwszStr
#define DEF_STR_INCRNELM        (     1024)                 // Incremental ...
////ine DEF_STR_MAXNELM         ( 128*1024)                 // Maximum ...
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
#define DEF_WGLBTEMP_INITNELM   (   4*1024)                 // Initial NELM of WCHAR GLB ...
#define DEF_WGLBTEMP_INCRNELM   (   1*1024)                 // Incremental ...
#define DEF_WGLBTEMP_MAXNELM    (  16*1024)                 // Maximum ...
#define DEF_DEBUG_INITNELM      (   1*1024)                 // Initial NELM of debug ...
#define DEF_DEBUG_INCRNELM      (   1*1024)                 // Incremental ...
#define DEF_DEBUG_MAXNELM       (  16*1024)                 // Maximum ...
#define DEF_WFORMAT_INITNELM    (   4*1024)                 // Initial NELM of WCHAR Formatting storage
#define DEF_WFORMAT_INCRNELM    (   4*1024)                 // Incremental ...
#define DEF_UNDOBUF_INITNELM    (   4*1024)                 // Initial NELM of Undo buffer
#define DEF_UNDOBUF_INCRNELM    (   4*1024)                 // Incremental ...
#define DEF_UNDOBUF_MAXNELM     ( 128*1024)                 // Maximum ...
#define DEF_QUADERROR_INITNELM  (   1*1024)                 // Initial NELM of []ERROR/[]ES buffer
#define DEF_QUADERROR_INCRNELM  (   1*1024)                 // Incremental ...
#define DEF_QUADERROR_MAXNELM   (  16*1024)                 // Maximum ...
#define DEF_STRAND_INITNELM     (   1*1024)                 // Initial NELM in tokens of the strand stack
#define DEF_STRAND_INCRNELM     (   1*1024)                 // Incremental ...
#define DEF_STRAND_MAXNELM      (   4*1024)                 // Maximum ...
#define DEF_DISPLAY_INITNELM    (   4*1024)                 // Initial NELM of WCHARs for Array Display
#define DEF_DISPLAY_INCRNELM    (   4*1024)                 // Incremental ...
#ifdef _WIN64
  #define DEF_WPTDTEMP_MAXNELM (100*1024*1024)              // Maximum ...
  #define DEF_WFORMAT_MAXNELM  (100*1024*1024)              // Maximum ...
  #define DEF_DISPLAY_MAXNELM  (100*1024*1024)              // Maximum ...
#elif defined (_WIN32)
  #define DEF_WPTDTEMP_MAXNELM ( 64*1024*1024)              // Maximum ...
  #define DEF_WFORMAT_MAXNELM  ( 64*1024*1024)              // Maximum ...
  #define DEF_DISPLAY_MAXNELM  (     256*1024)              // Maximum ...
#else
  #error Need code for this architecture.
#endif
#define DEF_GLBHSHTAB_NBLKS            128*1024             // Starting # blocks
#define DEF_GLBHSHTAB_EPB                2                  // # entries in each block
#define DEF_GLBHSHTAB_INITNELM  (DEF_GLBHSHTAB_NBLKS * DEF_GLBHSHTAB_EPB)   // Initial NELM of global HshTab for {symbol} names & values
#define DEF_GLBHSHTAB_INCRNELM  (DEF_GLBHSHTAB_INITNELM)    // Incremental ...
#define DEF_GLBHSHTAB_MAXNELM   (1024 * 1024 * DEF_GLBHSHTAB_EPB)   // Maximum ...
//efine DEF_GLBHSHTAB_HASHMASK  (DEF_GLBHSHTAB_NBLKS - 1)   // Starting hash mask
//efine DEF_GLBHSHTAB_INCRFREE  (DEF_HSHTAB_PRIME % DEF_GLBHSHTAB_INITNELM)

#define DEF_GLBSYMTAB_NBLKS            128*1024             // Starting # blocks
#define DEF_GLBSYMTAB_EPB                2                  // # entries in each block
#define DEF_GLBSYMTAB_INITNELM  (DEF_GLBSYMTAB_NBLKS * DEF_GLBSYMTAB_EPB)   // Initial NELM of global SymTab for ST Constants, {symbol} names & values
#define DEF_GLBSYMTAB_INCRNELM  (DEF_GLBSYMTAB_INITNELM)    // Incremental ...
#define DEF_GLBSYMTAB_MAXNELM   (1024 * 1024 * DEF_GLBSYMTAB_EPB)   // Maximum ...
//efine DEF_GLBSYMTAB_HASHMASK  (DEF_GLBSYMTAB_NBLKS - 1)   // Starting hash mask
//efine DEF_GLBSYMTAB_INCRFREE  (DEF_SYMTAB_PRIME % DEF_GLBSYMTAB_INITNELM)

#define DEF_CS_INITNELM         (     4*1024)               // Initial NELM of Ctrl Struc token buffer
#define DEF_CS_INCRNELM         (     4*1024)               // Incremental ...
#define DEF_CS_MAXNELM          (    64*1024)               // Maximum ...
#define DEF_FORSTMT_INITNELM    (     1*1024)               // Initial NELM of FORSTMT stack
#define DEF_FORSTMT_INCRNELM    (     1*1024)               // Incremental ...
#define DEF_FORSTMT_MAXNELM     (     1*1024)               // Maximum ...
#define DEF_LFTSTK_INITNELM     (     4*1024)               // Initial NELM of 2by2 left stack
#define DEF_LFTSTK_INCRNELM     (     4*1024)               // incremental ...
#define DEF_LFTSTK_MAXNELM      (   128*1024)               // Maximum ...
#define DEF_RHTSTK_INITNELM     DEF_LFTSTK_INITNELM         // Initial NELM of 2by2 left stack
#define DEF_RHTSTK_INCRNELM     DEF_LFTSTK_INCRNELM         // incremental ...
#define DEF_RHTSTK_MAXNELM      DEF_LFTSTK_MAXNELM          // Maximum ...


// Global Options for User Preferences
#define DEF_ADJUSTPW                TRUE
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
#define DEF_DISPFCNLINENUMS         TRUE
#define DEF_DISPMPSUF               FALSE
#define DEF_OUTPUTDEBUG             FALSE
#define DEF_OUTPUTEXCRLF            TRUE
#define DEF_SHOWNETERRS             FALSE


// Global Options for HC Preferences
#define DEF_DISP0IMAG               TRUE
#define DEF_DISPINFIX               TRUE
#define DEF_DISPOCTODIG             FALSE


// Range limits for []vars
#define DEF_RANGELIMIT_CT           TRUE
#define DEF_RANGELIMIT_FEATURE      TRUE
#define DEF_RANGELIMIT_FPC          TRUE
#define DEF_RANGELIMIT_IC           TRUE
#define DEF_RANGELIMIT_IO           TRUE
#define DEF_RANGELIMIT_PP           TRUE
#define DEF_RANGELIMIT_PW           TRUE
#define DEF_RANGELIMIT_RL           TRUE


// Empty assignment to []vars as Fixed System value (TRUE) or .ini file value (FALSE)
#define DEF_RESETVARS_CT            FALSE
#define DEF_RESETVARS_LR            FALSE
#define DEF_RESETVARS_DT            FALSE
#define DEF_RESETVARS_FC            FALSE
#define DEF_RESETVARS_FEATURE       FALSE
#define DEF_RESETVARS_FPC           FALSE
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

#define LVER_LEGALCOPYRIGHT_YEARS  L"2006-2019"     // ***COPYRIGHT***
#define LVER_COMPANYNAME_STR       L"Sudley Place Software"
#define LVER_COPYRIGHT1_STR        L"Copyright " WS_UTF16_COPYRIGHT L" " LVER_LEGALCOPYRIGHT_YEARS L" " LVER_COMPANYNAME_STR

#define VER_LEGALCOPYRIGHT_YEARS    "2006-2019"     // ***COPYRIGHT***
#define VER_COMPANYNAME_STR         "Sudley Place Software"
#define VER_COPYRIGHT1_STR          "Copyright "    UTF16_COPYRIGHT  " "  VER_LEGALCOPYRIGHT_YEARS  " "  VER_COMPANYNAME_STR

#define PROP_STATUSMSG             L"StatusMsg"


//***************************************************************************
//  Floating point constants
//***************************************************************************

#define POS_INFINITY            (0x7FF0000000000000)
#define NEG_INFINITY            (0xFFF0000000000000)
#define QUIET_NAN               (0x7FF8000000000000)
#define REAL_INDEFINITE         (0xFFF8000000000000)
#define FLOAT2POW53             (0x4340000000000000)
#define FLOATPI                 (0x400921FB54442D18)
#define FLOATGAMMA              (0x3FE2788CFC6FB619)
#define FLOATE                  (0x4005BF0A8B145769)


//***************************************************************************
//  Window Classes
//***************************************************************************

#define  MFWNDCLASS     "MFClass"       // Master Frame Window class
#define LMFWNDCLASS    L"MFClass"       // ...
#define  SMWNDCLASS     "SMClass"       // Session Manager ...
#define LSMWNDCLASS    L"SMClass"       // ...
#ifdef DEBUG
  #define  DBWNDCLASS   "DBClass"       // Debugger     ...
  #define LDBWNDCLASS  L"DBClass"       // ...
#endif
#ifdef PERFMONON
  #define  PMWNDCLASS   "PMClass"       // Performance Monitoring ...
  #define LPMWNDCLASS  L"PMClass"       // ...
#endif
#define  FEWNDCLASS     "FEClass"       // Function Editor ...
#define LFEWNDCLASS    L"FEClass"       // ...
#define  ECWNDCLASS     "ECClass"       // Edit Ctrl ...
#define LECWNDCLASS    L"ECClass"       // ...
#define  CCWNDCLASS     "CCClass"       // Crash Control ...
#define LCCWNDCLASS    L"CCClass"       // ...
#define  PBWNDCLASS     "PBClass"       // Progress Bar Window class
#define LPBWNDCLASS    L"PBClass"       // ...
#define  FW_RBWNDCLASS  "FW_RBClass"    // Font Window in Rebar Ctrl ...
#define LFW_RBWNDCLASS L"FW_RBClass"    // ...
#define  LW_RBWNDCLASS  "LW_RBClass"    // Language Window in Rebar Ctrl ...
#define LLW_RBWNDCLASS L"LW_RBClass"    // ...

#define LCL_WC_STATUSBARA    "LclStatusBar"
#define LCL_WC_STATUSBARW   L"LclStatusBar"

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

#define LOPART_LONGLONG     ((LONGLONG)  0x00000000FFFFFFFF)
#define HIPART_LONGLONG     ((LONGLONG)  0xFFFFFFFF00000000)

#define QWORD   ULONGLONG

#define PAGESIZE        (4 * 1024)

#define LIBCMD_SWITCH   L'?'

#ifdef _WIN64
#define WS_BITSIZE      L".64"
#else
#define WS_BITSIZE      L".32"
#endif


//***************************************************************************
//  Debugging
//***************************************************************************

// Wide char __FILE__, __LINE__, and __FUNCTION __ macros
#define SMASH(x,y)      x##y
#define WIDEN(x)        SMASH (L,x)
#define WIDEN2(x,y)     SMASH (L,x) SMASH (L,y)
#define WIDEN3(x,y,z)   SMASH (L,x) SMASH (L,y) SMASH (L,z)
#define WFILE           WIDEN (__FILE__)

#define TOWL2(p)    L#p
#define TOWL1(p)    TOWL2 (p)
#define WLINE       TOWL1 (__LINE__)

#define WFNLN       WFILE,__LINE__
#define WFCN        WIDEN (__FUNCTION__)

#define FNLN        FileNameOnly (__FILE__), __LINE__
#define FNLNFCN     FNLN,__FUNCTION__
#define WFNLNFCN    WFNLN,WFCN

#define DbgBrk          __debugbreak
#define DbgStop         __debugbreak

#define defstop \
default:        \
    DbgStop();

#ifdef DEBUG
  #include "WineHQ\WineCom.h"
  #define LCLODS        ODS
  #define LCLODSDD      ODSDD
  #define LCLODSRECT    ODSRECT
  #define LCLODSSIZE    ODSSIZE
  #define LCLODSAPI     ODSAPI

  #define APPEND_NAME L" -- " WIDEN(__FUNCTION__)
  #define APPEND_NAME_ARG , APPEND_NAME
#else
  #define LCLODS
  #define LCLODSDD
  #define LCLODSRECT
  #define LCLODSSIZE
  #define LCLODSAPI

  #define APPEND_NAME
  #define APPEND_NAME_ARG
  #define oprintfW
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
#define GWLSF_HGLBDFNHDR GWLSF_HWNDEC     + 1 * sizeof (HANDLE_PTR)  // ...   pre-existing function global memory handle (may be NULL)
#define GWLSF_UNDO_BEG   GWLSF_HGLBDFNHDR + 1 * sizeof (HANDLE_PTR)  // ...                beginning
#define GWLSF_UNDO_NXT  GWLSF_UNDO_BEG + 1 * sizeof (HANDLE_PTR)    // ...                next
#define GWLSF_UNDO_LST  GWLSF_UNDO_NXT + 1 * sizeof (HANDLE_PTR)    // ...                last
#define GWLSF_UNDO_GRP  GWLSF_UNDO_LST + 1 * sizeof (HANDLE_PTR)    // Value of next Undo group index
#define GWLSF_LASTKEY   GWLSF_UNDO_GRP + 1 * sizeof (long)          // Value of last WM_KEYDOWN key
#define GWLSF_CHANGED   GWLSF_LASTKEY  + 1 * sizeof (long)          // Boolean of whether or not the text has changed
#define GWLSF_FLN       GWLSF_CHANGED  + 1 * sizeof (long)          // Boolean of whether or not function line #s are to be displayed
#define GWLSF_LPMVS     GWLSF_FLN      + 1 * sizeof (long)          // Ptr to LPMEMVIRTSTR

// Define offsets in SMWNDCLASS window extra bytes
#define GWLSM_EXTRA     GWLSF_LPMVS    + 1 * sizeof (HANDLE_PTR)    // Total # extra bytes

// Define offsets in FEWNDCLASS window extra bytes
#define GWLFE_HWNDPRV   GWLSF_LPMVS    + 1 * sizeof (HANDLE_PTR)    // Next window handle in linked list (NULL = none)
#define GWLFE_HWNDNXT   GWLFE_HWNDPRV  + 1 * sizeof (HANDLE_PTR)    // Previous ...
#define GWLFE_EXTRA     GWLFE_HWNDNXT  + 1 * sizeof (HANDLE_PTR)    // Total # extra bytes

// Define offsets in PMWNDCLASS window extra bytes
#define GWLPM_HWNDLB    0                                           // Window handle of Listbox
#define GWLPM_EXTRA     GWLPM_HWNDLB   + 1 * sizeof (HANDLE_PTR)    // Total # extra bytes

// Define offsets in PBWNDCLASS window extra bytes
#define GWLPB_HGLB      0                                           // Window data global memory handle
#define GWLPB_EXTRA     GWLPB_HGLB     + 1 * sizeof (HANDLE_PTR)    // Total # extra bytes

// Define offsets in FW_RBWNDCLASS window extra bytes
#define GWLFW_RB_EXTRA  0                                           // Total # extra bytes

// Define offsets in LW_RBWNDCLASS window extra bytes
#define GWLLW_RB_EXTRA  0                                           // Total # extra bytes

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
#define MYWM_ERRMSG         (WM_APP + 8)    // SM (Display error message)
#define MYWM_SAVE_WS        (WM_APP + 9)    // SM (Save workspace)
#define MYWM_DISPMB         (WM_APP +10)    // CC (Display MessageBox)
#define MYWM_RESIZE         (WM_APP +11)    // MF (Resize to display Status Bar)
#define MYWM_NOTIFY         (WM_APP +12)    // EC (Pass on of WM_NOTIFY from EC)
#define MYWM_CMPNAME        (WM_APP +13)    // FE (Compare function names)
#define MYWM_RBUTTONDBLCLK  (WM_APP +14)    // FE/SM (Right-double-click)
#define MYWM_NEWFONT        (WM_APP +15)    // WS_FW (Set a new font and point size)
#define MYWM_GETCLIENTSIZE  (WM_APP +16)    // LW_FW (Get size of client area before the window is created)
#define MYWM_CLEARCHAR      (WM_APP +17)    // LW_FW (Clear highlight around any previously outlined char)
#define MYWM_CREATEFCN      (WM_APP +18)    // SM (Create a new function)
#define MYWM_LPMEMPTD       (WM_APP +19)    // MF (Pass lpMemPTD for TLS)
#define MYWM_PROMPT         (WM_APP +20)    // MF (Display the prompt)
#define MYWM_INIT_EC        (WM_APP +21)    // EC (Initialize local Edit Ctrl)
#define MYWM_DNL_SUCCESS    (WM_APP +22)    // DL (TRUE iff the download succeeded)
#define MYWM_MOUSELEAVE     (WM_APP +23)    // RB (Local version of WM_MOUSELEAVE)
#define MYWM_POST_CREATE    (WM_APP +24)    // MF (Post-WM_CREATE processing)

// Define Debug window messages
#define MYWM_INIT_DB        (WM_APP +50)    // DB
#define MYWM_DBGMSGA        (WM_APP +51)    // DB
#define MYWM_DBGMSGW        (WM_APP +52)    // DB
#define MYWM_DBGMSG_CLR     (WM_APP +53)    // DB
#define MYWM_DBGMSG_SCROLL  (WM_APP +54)    // DB
#define MYWM_UNHOOK         (WM_APP +55)    // DB


//***************************************************************************
//  Keyboard State Bits
//***************************************************************************

#define KS_SHIFT        4
#define KS_CTRL         2
#define KS_ALT          1
#define KS_NONE         0


//***************************************************************************
//  Global dead keys
//***************************************************************************

// Define a set of global dead keys
#define    GLB_DEAD_KEY_A     0xFFF0
#define    GLB_DEAD_KEY_C     0xFFF1
#define    GLB_DEAD_KEY_D     0xFFF2
#define    GLB_DEAD_KEY_G     0xFFF3
#define    GLB_DEAD_KEY_T     0xFFF4

#define WS_GLB_DEAD_KEY_A   L"\xFFF0"
#define WS_GLB_DEAD_KEY_C   L"\xFFF1"
#define WS_GLB_DEAD_KEY_D   L"\xFFF2"
#define WS_GLB_DEAD_KEY_G   L"\xFFF3"
#define WS_GLB_DEAD_KEY_T   L"\xFFF4"

#define WS_GLB_DEAD_KEYS    WS_GLB_DEAD_KEY_A \
                            WS_GLB_DEAD_KEY_C \
                            WS_GLB_DEAD_KEY_D \
                            WS_GLB_DEAD_KEY_G \
                            WS_GLB_DEAD_KEY_T

//***************************************************************************
//  Character constants -- ASCII and Wide characters and strings
//***************************************************************************

#define AC_EOS          '\0'        // 00:  End-of-string
#define AC_BS           '\b'        // 08:  Backspace
#define AC_HT           '\t'        // 09:  Horizontal Tab
#define AC_LF           '\n'        // 0A:  Linefeed
#define AC_FF           '\f'        // 0C:  FormFeed
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
#define WC_BEL          L'\x0007'   // 07:  Bell
#define WC_BS           L'\b'       // 08:  Backspace
#define WC_HT           L'\t'       // 09:  Horizontal Tab
#define WC_LF           L'\n'       // 0A:  Linefeed
#define WC_FF           L'\f'       // 0C:  FormFeed
#define WC_CR           L'\r'       // 0D:  Carriage Return
#define WC_ESC          L'\x001B'   // 1B:  Escape
#define WC_DQ           L'\"'       // 22:  Double Quote
#define WC_SQ           L'\''       // 27:  Single Quote
#define WC_SLOPE        L'\\'       // 5C:  Slope
#define WC_LC           L'\x27A5'   // 27A5:  Line Continuation marker

#define WS_BS           L"\b"       // 08:  Backspace
#define WS_HT           L"\t"       // 09:  Horizontal Tab
#define WS_LF           L"\n"       // 0A:  Linefeed
#define WS_FF           L"\f"       // 0C:  FormFeed
#define WS_CR           L"\r"       // 0D:  Carriage Return
#define WS_DQ           L"\""       // 22:  Double Quote
#define WS_SQ           L"\'"       // 27:  Single Quote
#define WS_SLOPE        L"\\"       // 5C:  Slope
#define WS_LC           L"\x27A5"   // 27A5:  Line Continuation marker

#define WS_CRLF         L"\r\n"     // 0D0A:    CR/LF    (hard line-break)
#define WS_CRCRLF       L"\r\r\n"   // 0D0D0A:  CR/CR/LF (soft line-break)
#define WS_WHITE        L" \r\n"    // 200D0A:  White space

#define AC_MFOLCL        '$'        // Initial character of MFO local var
#define WC_MFOLCL       L'$'        // ...
#define WS_MFOLCL       L"$"        // ...

#define DEF_UNDERFLOW       L'_'    // Default underflow char
#define DEF_UNDERFLOW_WS    L"_"    // ...
#define DEF_EXPONENT_UC     L'E'    // ...     exponent  ...
#define DEF_EXPONENT_LC     L'e'    // ...
#define DEF_EXPONENT_UC_WS  L"E"    // ...
#define DEF_EXPONENT_LC_WS  L"e"    // ...

// Define text for infinity
#define  TEXT_INFINITY       "{infinity}"
#define LTEXT_INFINITY      L"{infinity}"

// Define text for NaN
#define  TEXT_NAN            "NaN"
#define LTEXT_NAN           L"NaN"
#define  TEXT_NAN2           "{NaN}"
#define LTEXT_NAN2          L"{NaN}"

#define DEF_POSINFINITY_CHAR     '!'
#define DEF_POSINFINITY_STR      "!"
#define DEF_NEGINFINITY_STR     "-!"
#define DEF_NAN_CHAR              '#'
#define DEF_NAN_STR               "#"

// Define text for NaN
#define  TEXT_NAN2           "{NaN}"
#define LTEXT_NAN2          L"{NaN}"

// Use when translating to non-WCHAR code in <pn_parse.y>
#define INFINITY1       DEF_POSINFINITY_CHAR
#define INFINITY1_STR   DEF_POSINFINITY_STR
#define OVERBAR1        '-'
#define OVERBAR1_STR    "-"
#define NaN1            DEF_NAN_CHAR
#define NaN1_STR        DEF_NAN_STR

#ifndef NOQUADA
#define WQUADa          L"abcdefghijklmnopqrstuvwxyz"
#define WQUADA          L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//                           A     B     C     D     E     F     G     H     I     J     K     L     M
//                           N     O     P     Q     R     S     T     U     V     W     X     Y     Z
#define WQUADâ          L"\x00E2\x0062\x0109\x0064\x00EA\x0066\x011D\x0125\x00EE\x0135\x006B\x006C\x006D"   \
                        L"\x006E\x00F4\x0070\x0071\x0072\x0073\x0074\x0075\x0076\x0077\x0078\x0079\x007A"
#define WQUADÂ          L"\x00C2\x0042\x0108\x0044\x00CA\x0046\x011C\x0124\x00CE\x0134\x004B\x004C\x004D"   \
                        L"\x004E\x00D4\x0050\x0051\x0052\x015C\x0054\x00DB\x0056\x0174\x0058\x0176\x1E90"
#define WQUADä          L"\x00E4\x0062\x0063\x0064\x00EB\x0066\x0067\x1E27\x00EF\x006A\x006B\x006C\x006D"   \
                        L"\x006E\x00F6\x0070\x0071\x0072\x0073\x1E97\x00FC\x0076\x1E85\x1E8D\x00FF\x007A"
#define WQUADÄ          L"\x00C4\x0042\x0043\x0044\x00CB\x0046\x0047\x1E26\x00CF\x004A\x004B\x004C\x004D"   \
                        L"\x004E\x00D6\x0050\x0051\x0052\x0053\x0054\x00DC\x0056\x1E84\x1E8C\x0178\x005A"
#define WQUADá          L"\x00E1\x0062\x0107\x0064\x00E9\x0066\x01F5\x0068\x00ED\x006A\x1E31\x013A\x1E3F"   \
                        L"\x0144\x00F3\x1E55\x0071\x0155\x015B\x0074\x00FA\x0076\x1E83\x0078\x00FD\x017A"
#define WQUADÁ          L"\x00C1\x0042\x0106\x0044\x00C9\x0046\x01F4\x0048\x00CD\x004A\x1E30\x0139\x1E3E"   \
                        L"\x0143\x00D3\x1E54\x0051\x0154\x015A\x0054\x00DA\x0056\x1E82\x0058\x00DD\x0179"
#define WQUADà          L"\x00E0\x0062\x0063\x0064\x00E8\x0066\x0067\x0068\x00EC\x006A\x006B\x006C\x006D"   \
                        L"\x01F9\x00F2\x0070\x0071\x0072\x0073\x0074\x00F9\x0076\x1E81\x0078\x1EF3\x007A"
#define WQUADÀ          L"\x00C0\x0042\x0043\x0044\x00C8\x0046\x0047\x0048\x00CC\x004A\x004B\x004C\x004D"   \
                        L"\x01F8\x00D2\x0050\x0051\x0052\x0053\x0054\x00D9\x0056\x1E80\x0058\x1EF2\x005A"
#define WQUADã          L"\x00E3\x0062\x0063\x0064\x1EBD\x0066\x0067\x0068\x0129\x006A\x006B\x006C\x006D"   \
                        L"\x00F1\x00F5\x0070\x0071\x0072\x0073\x0074\x0169\x1E7D\x0077\x0078\x1EF9\x007A"
#define WQUADÃ          L"\x00C3\x0042\x0043\x0044\x1EBC\x0046\x0047\x0048\x0128\x004A\x004B\x004C\x004D"   \
                        L"\x00D1\x00D5\x0050\x0051\x0052\x0053\x0054\x0168\x1E7C\x0057\x0058\x1EF8\x005A"
#endif

//***************************************************************************
// Wide-char routines From <string.h>
//***************************************************************************

#define strchrW         wcschr
#define strncmpW        wcsncmp
#define strncmpi        _strnicmp
#define strncmpiW       _wcsnicmp
#define strpbrkW        wcspbrk
#define strspnW         wcsspn
#define strcspnW        wcscspn
#define strtolW         wcstol
#define isspaceW        iswspace
#define atofW           _wtof
#define sscanfW         swscanf
#define fgetsW          fgetws
#define fopenW          _wfopen
#define strlwrW         _wcslwr
#define strdupW         _wcsdup
#define tolowerW        towlower
#define IsDigit         isdigit
#define IsDigitW        iswdigit
#define strlenW         wcslen
#define strstrW         wcsstr


//***************************************************************************
//  Bit, byte, etc. constants
//***************************************************************************

// Define bit masks
#define BIT0    0x0001
#define BIT1    0x0002
#define BIT2    0x0004
#define BIT3    0x0008
#define BIT4    0x0010
#define BIT5    0x0020
#define BIT6    0x0040
#define BIT7    0x0080
#define BIT8    0x0100
#define BIT9    0x0200
#define BIT10   0x0400
#define BIT11   0x0800
#define BIT12   0x1000
#define BIT13   0x2000
#define BIT14   0x4000
#define BIT15   0x8000

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

#define MAXOBJ  (1024*1024)


//***************************************************************************
//  Window Properties
//***************************************************************************

#define PROP_IDMPOSFN   L"PROP_IDMPOSFN"
#define PROP_NTHREADS   L"PROP_NTHREADS"
#define PROP_FIRSTVISIBLELINE   L"FirstVisibleLine"


//***************************************************************************
//  RAT & VFP
//***************************************************************************

// Define the following symbol iff comparison of two RATs is meant to be exact
//   as opposed to sensitive to []CT.
////#define RAT_EXACT


//***************************************************************************
//  Timers
//***************************************************************************

#define ID_TIMER_UPDCHK        1
#define ID_TIMER_EDIT       1729
#define ID_TIMER_EXEC       1730


//***************************************************************************
//  Progress Bar
//***************************************************************************

typedef struct tagCOLORBLEND *LPCOLORBLEND;     // Dummy entry for .pro files only

#if _WIN32_WINNT < 0x0600

#define PBM_GETSTEP             (WM_USER+13)
#define PBM_GETBKCOLOR          (WM_USER+14)
#define PBM_GETBARCOLOR         (WM_USER+15)
#define PBM_SETSTATE            (WM_USER+16) // wParam = PBST_[State] (NORMAL, ERROR, PAUSED)
#define PBM_GETSTATE            (WM_USER+17)

#define PBST_NORMAL             0x0001
#define PBST_ERROR              0x0002
#define PBST_PAUSED             0x0003
#endif      // _WIN32_WINNT < 0x0600

#define PBM_GETBUDDY            (WM_USER+18)
#define PBM_SETBUDDY            (WM_USER+19)


//***************************************************************************
//  Residue functions
//***************************************************************************

#define AplModI     ModHC1I
#define AplModF     ModHC1F
#define AplModR     ModHC1R
#define AplModV     ModHC1V


//***************************************************************************
//  Item Comparisons with []CT, no integer test
//***************************************************************************

#define CmpCT_F(aplLft,aplRht,fQuadCT,CMP)                      \
       (flt_cmp_ct   ( aplLft,  aplRht, fQuadCT, FALSE) CMP 0)

#ifdef RAT_EXACT
    // Compare the two RATs
#define CmpCT_R(aplLft,aplRht,fQuadCT,CMP)                      \
        (mpq_cmp     (&aplLft, &aplRht                ) CMP 0)
#else
    // Compare the two RATs relative to []CT
#define CmpCT_R(aplLft,aplRht,fQuadCT,CMP)                      \
        (mpq_cmp_ct  (&aplLft, &aplRht, fQuadCT, FALSE) CMP 0)
#endif

#define CmpCT_V(aplLft,aplRht,fQuadCT,CMP)                      \
        (mpfr_cmp_ct (&aplLft, &aplRht, fQuadCT, FALSE) CMP 0)

#define CmpCT_B(aplLft,aplRht,fQuadCT,CMP)                      \
        (arb_cmp_ct (&aplLft, &aplRht, fQuadCT, FALSE) CMP 0)


//***************************************************************************
//  Item Comparisons with SYS_CT, allow integer test
//***************************************************************************

#define CmpSCT_F(aplLft,aplRht,fQuadCT,CMP)                     \
       (flt_cmp_ct   ( aplLft,  aplRht, fQuadCT, TRUE ) CMP 0)

#ifdef RAT_EXACT
    // Compare the two RATs
#define CmpSCT_R(aplLft,aplRht,fQuadCT,CMP)                     \
        (mpq_cmp     (&aplLft, &aplRht                ) CMP 0)
#else
    // Compare the two RATs relative to []CT
#define CmpSCT_R(aplLft,aplRht,fQuadCT,CMP)                     \
        (mpq_cmp_ct  (&aplLft, &aplRht, fQuadCT, TRUE ) CMP 0)
#endif

#define CmpSCT_V(aplLft,aplRht,fQuadCT,CMP)                     \
        (mpfr_cmp_ct (&aplLft, &aplRht, fQuadCT, TRUE ) CMP 0)

#define CmpSCT_B(aplLft,aplRht,fQuadCT,CMP)                     \
        (arb_cmp_ct (&aplLft, &aplRht, fQuadCT, TRUE ) CMP 0)


//***************************************************************************
//  End of File: defines.h
//***************************************************************************
