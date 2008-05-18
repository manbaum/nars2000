//***************************************************************************
//  NARS2000 -- Extern Variables
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

// Default definitions
#define DEF_TCFONTNAME      "Georgia"
#define DEF_SMFONTNAME      "SImPL"             // Or "APL385 Unicode"
#define DEF_FEFONTNAME      "SImPL"             // Or "APL385 Unicode"
#define DEF_MEFONTNAME      "SImPL"             // Or "APL385 Unicode"
#define DEF_VEFONTNAME      "SImPL"             // Or "APL385 Unicode"

#define DEF_TCLOGFONT       0,0,0,0,FW_BOLD  ,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,VARIABLE_PITCH | FF_ROMAN ,DEF_TCFONTNAME
#define DEF_SMLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_SMFONTNAME
#define DEF_FELOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_FEFONTNAME
#define DEF_MELOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_MEFONTNAME
#define DEF_VELOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_VEFONTNAME

#define DEF_TCPTSIZE        8           // Point size for TC font
#define DEF_SMPTSIZE       13           // ...            SM ...
#define DEF_FEPTSIZE       13           // ...            FE ...
#define DEF_MEPTSIZE       13           // ...            ME ...
#define DEF_VEPTSIZE       13           // ...            VE ...

#define DEF_INDENT          6           // Prompt indent
#define DEF_TABS            4           // Tab insertion
#define DEF_CURWID_INS      5           // Cursor width for insert mode
#define DEF_CURWID_REP      2           // ...              replace ...
#define DEF_HISTLINES    3000           // # lines in history buffer
#define DEF_TEXT_FG_COLOR   COLOR_RED
#define DEF_TEXT_BG_COLOR   COLOR_WHITE

// Size of storage areas
#define DEF_NUM_INITSIZE        (      256)                 // Initial size for lpszNum
#define DEF_NUM_INCRSIZE        (      256)                 // Incremental ...
#define DEF_NUM_MAXSIZE         (   4*1024)                 // Maximum ...
#define DEF_STR_INITSIZE        (      256)                 // Initial size for lpwszStr
#define DEF_STR_INCRSIZE        (      256)                 // Incremental ...
#define DEF_STR_MAXSIZE         (   4*1024)                 // Maximum ...
////ine DEF_TOKENSTACK_INITSIZE (   4*1024)                 // Initial size of token stack
////ine DEF_TOKENSTACK_INCRSIZE (   4*1024)                 // Incremental ...
////ine DEF_TOKENSTACK_MAXSIZE  (  64*1024)                 // Maximum ...
#define DEF_SIS_INITSIZE        (   1*1024)                 // Initial size for State Indicator Stack
#define DEF_SIS_INCRSIZE        (   1*1024)                 // Incremental ..
#define DEF_SIS_MAXSIZE         (   4*1024)                 // Maximum ...
#define DEF_YYRES_INITSIZE      (   1*1024)                 // Initial size of YYRes buffer
#define DEF_YYRES_INCRSIZE      (   1*1024)                 // Incremental ...
#define DEF_YYRES_MAXSIZE       (  16*1024)                 // Maximum ...
#define DEF_CTEMP_INITSIZE      (   4*1024)                 // Initial size of char  temporary storage
#define DEF_CTEMP_INCRSIZE      (   1*1024)                 // Incremental ...
#define DEF_CTEMP_MAXSIZE       (  16*1024)                 // Maximum ...
#define DEF_WTEMP_INITSIZE      (   4*1024)                 // Initial size of WCHAR ...
#define DEF_WTEMP_INCRSIZE      (   1*1024)                 // Incremental ...
#define DEF_WTEMP_MAXSIZE       (  16*1024)                 // Maximum ...
#define DEF_DEBUG_INITSIZE      (   1*1024)                 // Initial size of debug ...
#define DEF_DEBUG_INCRSIZE      (   1*1024)                 // Incremental ...
#define DEF_DEBUG_MAXSIZE       (  16*1024)                 // Maximum ...
#define DEF_WFORMAT_INITSIZE    (   4*1024)                 // Initial size of WCHAR Formatting storage
#define DEF_WFORMAT_INCRSIZE    (   4*1024)                 // Incremental ...
#define DEF_WFORMAT_MAXSIZE     ( 256*1024)                 // Maximum ...
#define DEF_UNDOBUF_INITSIZE    (   4*1024)                 // Initial size of Undo buffer
#define DEF_UNDOBUF_INCRSIZE    (   4*1024)                 // Incremental ...
#define DEF_UNDOBUF_MAXSIZE     (  64*1024)                 // Maximum ...
#define DEF_QUADERROR_INITSIZE  (   1*1024)                 // Initial size of []ERROR/[]ES buffer
#define DEF_QUADERROR_INCRSIZE  (   1*1024)                 // Incremental ...
#define DEF_QUADERROR_MAXSIZE   (  16*1024)                 // Maximum ...
#define DEF_STRAND_INITSIZE     (   1*1024)                 // Initial size in tokens of the strand stack
#define DEF_STRAND_INCRSIZE     (   1*1024)                 // Incremental ...
#define DEF_STRAND_MAXSIZE      (   4*1024)                 // Maximum ...
#define DEF_DISPLAY_INITSIZE    (   4*1024)                 // Initial size of WCHARs for Array Display
#define DEF_DISPLAY_INCRSIZE    (   4*1024)                 // Incremental ...
#define DEF_DISPLAY_MAXSIZE     ( 256*1024)                 // Maximum ...


// Global Options
#define DEF_NEWTABONCLEAR   TRUE
#define DEF_NEWTABONLOAD    TRUE


// Date/time formats
#define DATETIME_OUT        L"dd/mm/yyyy hh:mm:ss"
#define DATETIME_FMT        L"%02d/%02d/%04d %2d:%02d:%02d"
#define DATETIME_LEN        (sizeof (DATETIME_OUT) / sizeof (WCHAR))


#include <commctrl.h>
#include "datatype.h"
#include "primfns.h"
#include "symtab.h"
#include "tokens.h"
#include "parse.h"
#include "Unicode.h"

// Define variables which are also used in the per tab structure
#include "primspec.h"


#ifdef DEFINE_VARS
  #define EXTERN
#else
  #define EXTERN extern
#endif

// Define struct for passing parameters to WM_NCCREATE/WM_CREATE
//   for the Session Manager
typedef struct tagSM_CREATESTRUCTW
{
    HGLOBAL hGlbDPFE;       // 00:  Workspace DPFE global memory handle
    BOOL    bExecLX;        // 04:  TRUE iff execute []LX after successful load
                            // 08:  Length
} SM_CREATESTRUCTW, UNALIGNED *LPSM_CREATESTRUCTW;


//***************************************************************************
//  Default global values of system variables -- these values
//    are used to set the variables in a CLEAR WS.
//***************************************************************************

// Indeterminate Control Values
typedef enum tagIC_VALUES
{
    ICVAL_ZERO,             // 00:  Result is 0
    ICVAL_ONE,              // 01:  ...       1
    ICVAL_DOMAIN_ERROR,     // 02:  ...       DOMAIN ERROR
    ICVAL_POS_INFINITY,     // 03:  ...       + infinity
    ICVAL_NEG_INFINITY,     // 04:  ...       - infinity
    ICVAL_LENGTH,           // 05:  Length
} IC_VALUES;

// Define the maximum allowable value for []IC
#define ICVAL_MAXVAL    (ICVAL_LENGTH - 1)

// Indeterminate Control Indices
typedef enum tagIC_INDICES
{
    ICNDX_DIV0,             // 00:    {div} 0
    ICNDX_LOG0,             // 01:    {log} 0
    ICNDX_QDOTn,            // 02:      !N for integer N < 0
    ICNDX_0MULPi,           // 03:  0 {times} _
    ICNDX_0MULNi,           // 04:  0 {times} {neg}_
    ICNDX_0DIV0,            // 05:  0 {div} 0
    ICNDX_PiDIVPi,          // 06:  _ {div} _   (same sign)
    ICNDX_NiDIVPi,          // 07:  _ {div} _   (different sign)
    ICNDX_0EXP0,            // 08:  0   *   0
    ICNDX_0LOG0,            // 09:  0 {log} 0
    ICNDX_0LOG1,            // 0A:  0 {log} 1
    ICNDX_1LOG0,            // 0B:  1 {log} 0
    ICNDX_1LOG1,            // 0C:  1 {log} 1
    ICNDX_LENGTH,           // 0D:  Length
} IC_INDICES;

EXTERN
APLINT   aplDefaultIC[ICNDX_LENGTH]     // []IC
#ifdef DEFINE_VALUES
 = {ICVAL_POS_INFINITY,     // 00:    {div} 0
    ICVAL_NEG_INFINITY,     // 01:    {log} 0
    ICVAL_DOMAIN_ERROR,     // 02:      ! {neg}1
    ICVAL_DOMAIN_ERROR,     // 03:  0 {times} _
    ICVAL_DOMAIN_ERROR,     // 04:  0 {times} {neg}_
    ICVAL_ONE,              // 05:  0 {div} 0
    ICVAL_DOMAIN_ERROR,     // 06:  _ {div} _   (same sign)
    ICVAL_DOMAIN_ERROR,     // 07:  _ {div} _   (different sign)
    ICVAL_ONE,              // 08:  0   *   0
    ICVAL_DOMAIN_ERROR,     // 09:  0 {log} 0
    ICVAL_DOMAIN_ERROR,     // 0A:  0 {log} 1
    ICVAL_DOMAIN_ERROR,     // 0B:  1 {log} 0
    ICVAL_ONE,              // 0C:  1 {log} 1
   }
#endif
;

EXTERN
HGLOBAL  hGlbQuadALX_CWS    ,           // []ALX    ([]dm)
         hGlbQuadELX_CWS    ,           // []ELX    ([]dm)
         hGlbQuadFC_SYS     ,           // []FC     (L".,*0_" WS_UTF16_OVERBAR)
         hGlbQuadFC_CWS     ,           // []FC     hGlbQuadFC_SYS or from )LOAD
         hGlbQuadIC_SYS     ,           // []IC     (aplDefaultIC)
         hGlbQuadIC_CWS     ,           // []IC     hGlbQuadIC_SYS or from )LOAD
         hGlbQuadLX_CWS     ,           // []LX     (L"")
         hGlbQuadSA_CWS     ,           // []SA     (L"")
         hGlbQuadWSID_CWS   ,           // []WSID   (L"\0")
         hGlbQuadPR_CWS     ;           // []PR     (L"") (When an empty vector)
EXTERN
APLFLOAT fQuadCT_CWS        ;           // []CT

EXTERN
APLBOOL  bQuadIO_CWS        ,           // []IO
         bQuadxSA_CWS       ;           // []SA (in its index form)

EXTERN
APLUINT  uQuadPP_CWS        ,           // []PP
         uQuadPW_CWS        ,           // []PW
         uQuadRL_CWS        ;           // []RL

EXTERN
APLCHAR  cQuadPR_CWS        ;           // []PR     (L' ') (When a char scalar)

typedef struct tagRANGELIMIT
{
    BOOL CT:1,
         IC:1,
         IO:1,
         PP:1,
         PW:1,
         RL:1;
} RANGELIMIT;

EXTERN
RANGELIMIT bRangeLimit
#ifdef DEFINE_VALUES
//  CT    IC    IO    PP    PW    RL
= {TRUE, TRUE, TRUE, TRUE, TRUE, TRUE}
#endif
;

EXTERN
BOOL bAdjustPW
#ifdef DEFINE_VALUES
= TRUE
#endif
;


//***************************************************************************
//  Application values
//***************************************************************************

EXTERN
HINSTANCE _hInstance;                   // Global instance handle

EXTERN
HANDLE hAccel;                          // Keyboard accelerators

EXTERN
HMENU hMenuSM,                          // Handle for Session Manager menu
      hMenuFE,                          // ...        Function Editor ...
      hMenuME,                          // ...        Matrix   ...
      hMenuVE,                          // ...        Vector   ...
      hMenuSMWindow,                    // ...        window entry in hMenuSM
      hMenuFEWindow,                    // ...        ...             hMenuFE
      hMenuMEWindow,                    // ...        ...             hMenuME
      hMenuVEWindow;                    // ...        ...             hMenuVE

#ifdef DEBUG
  #define  APPEND_DEBUG  " (DEBUG)"
  #define LAPPEND_DEBUG L" (DEBUG)"
#else
  #define  APPEND_DEBUG
  #define LAPPEND_DEBUG
#endif

EXTERN
WCHAR lpwszAppName[]                    // Application name for MessageBox
#ifdef DEFINE_VALUES
 = L"NARS2000" LAPPEND_DEBUG
#endif
;

EXTERN
char lpszAppName[]                      // Application name for MessageBox
#ifdef DEFINE_VALUES
 = "NARS2000" APPEND_DEBUG
#endif
;

EXTERN
WCHAR wszAppDPFE  [_MAX_PATH],          // .EXE drive, path, filename.ext
//////wszInitDir  [_MAX_PATH],          // Initial directory for File Open & Save
      wszHlpDPFE  [_MAX_PATH];          // .HLP ...
EXTERN
char szOpenFile [_MAX_PATH];            // Save area for multiple files to open

EXTERN
WCHAR wszWorkDir[_MAX_PATH];            // Load/Save workspaces directory

#define WS_WKSNAME  L"workspaces"       // Name of Workspaces subdirectory under main dir

EXTERN
CRITICAL_SECTION CSO0,                  // Critical Section Object #0
                 CSO1,                  // ...                     #1
                 CSOPL;                 // ...                     for ParseLine


//***************************************************************************
// Thread-related Variables
//***************************************************************************

EXTERN
DWORD dwMainThreadId;                   // Thread ID of the main application

EXTERN
DWORD dwTlsType,                        // Thread type (e.g.
                                        //   'MF' for Master Frame,
                                        //   'TC' for Tab Control,
                                        //   'PL' for ParseLine,
                                        //   etc.)
      dwTlsPlLocalVars,                 // Ptr to lpplLocalVars for PL thread only
      dwTlsFhLocalVars,                 // Ptr to lpfhLocalVars for SM and PL threads
      dwTlsPerTabData;                  // Ptr to PerTabData    for TC, SM, and PL threads

//***************************************************************************
// Temporary storage
//***************************************************************************

EXTERN
LPCHAR lpszTemp;                        // Used for temporary char storage

EXTERN
LPWCHAR lpwszTemp;                      // Used for temporary WCHAR storage

EXTERN
UCHAR gDbgLvl                           // Debug level 0 = none
#ifdef DEFINE_VALUES
 = 0
#endif
;

EXTERN
LPWCHAR lpwNameTypeStr[]
#ifdef DEFINE_VALUES
 = NAMETYPE_WSTRPTR
#endif
;

#ifdef DEBUG
  EXTERN
  LPCHAR lpszDebug;                     // Used for temporary storage of char
                                        //   debug output
  EXTERN
  LPWCHAR lpwszDebug;                   // Used for temporary storage of WCHAR
                                        //   debug output
  EXTERN
  LPCHAR lpNameTypeStr[]
  #ifdef DEFINE_VALUES
   = NAMETYPE_STRPTR
  #endif
  ;

  EXTERN
  LPWCHAR lpwObjNameStr[]
  #ifdef DEFINE_VALUES
   = OBJNAME_WSTRPTR
  #endif
  ;
#endif

//***************************************************************************
//  Primitive function and operator tables
//***************************************************************************

EXTERN
LPPRIMFNS PrimFnsTab[256];              // The jump table for all primitive functions

EXTERN
LPPRIMFNS PrimProtoFnsTab[256];         // The jump table for all primitive functions/operator prototypes

EXTERN
LPPRIMSPEC PrimSpecTab[256];            // The table of corresponding LPPRIMSPECs
                                        //   for all of the primitive scalar functions
typedef struct tagPRIMFLAGS
{
    WORD Index:4,                       // 000F:  Function index (see FBFN_INDS)
         Available:5,                   // 01F0:  Available flag bits
         IdentElem:1,                   // 0200:  TRUE iff this function has an identity element
         DydScalar:1,                   // 0400:  ...                    is scalar dyadic
         MonScalar:1,                   // 0800:  ...                       ...    monadic
         Alter:1,                       // 1000:  ...                       alternating
         AssocBool:1,                   // 2000:  ...                       associative on Booleans only
         AssocNumb:1,                   // 4000:  ...                       associative on all numbers
         FastBool:1;                    // 8000:  Boolean function w/reduction & scan can be sped up
                                        // 0000:  No available bits
} PRIMFLAGS, *LPPRIMFLAGS;

EXTERN
PRIMFLAGS PrimFlags[256];               // The flag tables for all primitive functions/operators


//***************************************************************************
//  Fast Boolean Reduction and Scan tables
//***************************************************************************

typedef enum tagFBFN_INDS               // Fast Boolean function indices
{   PF_INDEX_UNK = 0 ,                  // 00 = No entry so we can catch this as an error
    PF_INDEX_LESS    ,                  // 01 = Index for "less than" ...
    PF_INDEX_OR      ,                  // 02 = ...       "or"    ...
    PF_INDEX_NOR     ,                  // 03 = ...       "nor"   ...
    PF_INDEX_MOREEQ  ,                  // 04 = ...       "more than or equal" ...
    PF_INDEX_NAND    ,                  // 05 = ...       "nand"  ...
    PF_INDEX_MORE    ,                  // 06 = ...       "more than" ...
    PF_INDEX_LESSEQ  ,                  // 07 = ...       "less than or equal" ...
    PF_INDEX_AND     ,                  // 08 = ...       "and"   ...
    PF_INDEX_EQUAL   ,                  // 09 = ...       "equal" ...
    PF_INDEX_NOTEQUAL,                  // 0A = ...       "not equal" ...
    PF_INDEX_MAX     ,                  // 0B = ...       "max"   ...
    PF_INDEX_MIN     ,                  // 0C = ...       "min"   ...
    PF_INDEX_PLUS    ,                  // 0D = ...       "plus"  ...
    PF_INDEX_MINUS   ,                  // 0E = ...       "minus" ...
    PF_INDEX_DIVIDE  ,                  // 0F = ...       "divide" ...
    PF_INDEX_NEXT                       // No available entries (4 bits)
                                        // If another entry is made, be sure
                                        //   to increase Index:4 to Index:5
                                        //   in tagPRIMFLAGS
} FBFN_INDS;

EXTERN
APLFLOAT PrimIdent[PF_INDEX_NEXT];      // Primitive scalar function identity elements
                                        //   in the same order as FBFN_INDS

typedef void (FASTBOOLFCN) (APLSTYPE     aplTypeRht,        // Right arg storage type
                            APLNELM      aplNELMRht,        // Right arg NELM
                            LPVOID       lpMemRht,          // Ptr to right arg global memory
                            LPVOID       lpMemRes,          // Ptr to result    ...
                            APLUINT      uDimLo,            // Product of dimensions below axis
                            APLUINT      uDimAxRht,         // Length of right arg axis dimension
                            FBFN_INDS    uIndex,            // FBFN_INDS value (e.g., index into FastBoolFns[])
                            LPPL_YYSTYPE lpYYFcnStrOpr);    // Ptr to operator function strand
typedef FASTBOOLFCN *LPFASTBOOLFCN;

typedef struct tagFASTBOOLFNS
{
    LPFASTBOOLFCN lpReduction;          // 00:  Ptr to Fast Boolean reduction routine
    LPFASTBOOLFCN lpScan;               // 04:  ...                 scan      ...
    UINT          NotMarker:1,          // 08:  00000001:  Complement of Marker
                  IdentElem:1,          //      00000002:  Identity element (if it exists)
                  Suffix:1;             //      00000004:  Suffix equivalence value
                                        //      FFFFFFF8:  Available bits
                                        // 0C:  Length
} FASTBOOLFNS, *LPFASTBOOLFNS;

// This array translates a byte index into
//   [byte][0] = the index of the first 0 in the byte (from right to left)
//   [byte][1] = the index of the first 1 in the byte (from right to left)
//   [byte][2] = the sum of the bits in the byte
//   [byte][3] = its {notequal} scan
//   [byte][4] = its {equal} scan
EXTERN
UCHAR FastBoolTrans[256][5]
#ifdef DEFINE_VALUES
  // Generated by an APL function
   = {
  {0,8,0,  0,170}, // 00000000
  {1,0,1,255, 85}, // 00000001
  {0,1,1,254, 84}, // 00000010
  {2,0,2,  1,171}, // 00000011
  {0,2,1,252, 86}, // 00000100
  {1,0,2,  3,169}, // 00000101
  {0,1,2,  2,168}, // 00000110
  {3,0,3,253, 87}, // 00000111

  {0,3,1,248, 82}, // 00001000
  {1,0,2,  7,173}, // 00001001
  {0,1,2,  6,172}, // 00001010
  {2,0,3,249, 83}, // 00001011
  {0,2,2,  4,174}, // 00001100
  {1,0,3,251, 81}, // 00001101
  {0,1,3,250, 80}, // 00001110
  {4,0,4,  5,175}, // 00001111

  {0,4,1,240, 90}, // 00010000
  {1,0,2, 15,165}, // 00010001
  {0,1,2, 14,164}, // 00010010
  {2,0,3,241, 91}, // 00010011
  {0,2,2, 12,166}, // 00010100
  {1,0,3,243, 89}, // 00010101
  {0,1,3,242, 88}, // 00010110
  {3,0,4, 13,167}, // 00010111

  {0,3,2,  8,162}, // 00011000
  {1,0,3,247, 93}, // 00011001
  {0,1,3,246, 92}, // 00011010
  {2,0,4,  9,163}, // 00011011
  {0,2,3,244, 94}, // 00011100
  {1,0,4, 11,161}, // 00011101
  {0,1,4, 10,160}, // 00011110
  {5,0,5,245, 95}, // 00011111

  {0,5,1,224, 74}, // 00100000
  {1,0,2, 31,181}, // 00100001
  {0,1,2, 30,180}, // 00100010
  {2,0,3,225, 75}, // 00100011
  {0,2,2, 28,182}, // 00100100
  {1,0,3,227, 73}, // 00100101
  {0,1,3,226, 72}, // 00100110
  {3,0,4, 29,183}, // 00100111

  {0,3,2, 24,178}, // 00101000
  {1,0,3,231, 77}, // 00101001
  {0,1,3,230, 76}, // 00101010
  {2,0,4, 25,179}, // 00101011
  {0,2,3,228, 78}, // 00101100
  {1,0,4, 27,177}, // 00101101
  {0,1,4, 26,176}, // 00101110
  {4,0,5,229, 79}, // 00101111

  {0,4,2, 16,186}, // 00110000
  {1,0,3,239, 69}, // 00110001
  {0,1,3,238, 68}, // 00110010
  {2,0,4, 17,187}, // 00110011
  {0,2,3,236, 70}, // 00110100
  {1,0,4, 19,185}, // 00110101
  {0,1,4, 18,184}, // 00110110
  {3,0,5,237, 71}, // 00110111

  {0,3,3,232, 66}, // 00111000
  {1,0,4, 23,189}, // 00111001
  {0,1,4, 22,188}, // 00111010
  {2,0,5,233, 67}, // 00111011
  {0,2,4, 20,190}, // 00111100
  {1,0,5,235, 65}, // 00111101
  {0,1,5,234, 64}, // 00111110
  {6,0,6, 21,191}, // 00111111

  {0,6,1,192,106}, // 01000000
  {1,0,2, 63,149}, // 01000001
  {0,1,2, 62,148}, // 01000010
  {2,0,3,193,107}, // 01000011
  {0,2,2, 60,150}, // 01000100
  {1,0,3,195,105}, // 01000101
  {0,1,3,194,104}, // 01000110
  {3,0,4, 61,151}, // 01000111

  {0,3,2, 56,146}, // 01001000
  {1,0,3,199,109}, // 01001001
  {0,1,3,198,108}, // 01001010
  {2,0,4, 57,147}, // 01001011
  {0,2,3,196,110}, // 01001100
  {1,0,4, 59,145}, // 01001101
  {0,1,4, 58,144}, // 01001110
  {4,0,5,197,111}, // 01001111

  {0,4,2, 48,154}, // 01010000
  {1,0,3,207,101}, // 01010001
  {0,1,3,206,100}, // 01010010
  {2,0,4, 49,155}, // 01010011
  {0,2,3,204,102}, // 01010100
  {1,0,4, 51,153}, // 01010101
  {0,1,4, 50,152}, // 01010110
  {3,0,5,205,103}, // 01010111

  {0,3,3,200, 98}, // 01011000
  {1,0,4, 55,157}, // 01011001
  {0,1,4, 54,156}, // 01011010
  {2,0,5,201, 99}, // 01011011
  {0,2,4, 52,158}, // 01011100
  {1,0,5,203, 97}, // 01011101
  {0,1,5,202, 96}, // 01011110
  {5,0,6, 53,159}, // 01011111

  {0,5,2, 32,138}, // 01100000
  {1,0,3,223,117}, // 01100001
  {0,1,3,222,116}, // 01100010
  {2,0,4, 33,139}, // 01100011
  {0,2,3,220,118}, // 01100100
  {1,0,4, 35,137}, // 01100101
  {0,1,4, 34,136}, // 01100110
  {3,0,5,221,119}, // 01100111

  {0,3,3,216,114}, // 01101000
  {1,0,4, 39,141}, // 01101001
  {0,1,4, 38,140}, // 01101010
  {2,0,5,217,115}, // 01101011
  {0,2,4, 36,142}, // 01101100
  {1,0,5,219,113}, // 01101101
  {0,1,5,218,112}, // 01101110
  {4,0,6, 37,143}, // 01101111

  {0,4,3,208,122}, // 01110000
  {1,0,4, 47,133}, // 01110001
  {0,1,4, 46,132}, // 01110010
  {2,0,5,209,123}, // 01110011
  {0,2,4, 44,134}, // 01110100
  {1,0,5,211,121}, // 01110101
  {0,1,5,210,120}, // 01110110
  {3,0,6, 45,135}, // 01110111

  {0,3,4, 40,130}, // 01111000
  {1,0,5,215,125}, // 01111001
  {0,1,5,214,124}, // 01111010
  {2,0,6, 41,131}, // 01111011
  {0,2,5,212,126}, // 01111100
  {1,0,6, 43,129}, // 01111101
  {0,1,6, 42,128}, // 01111110
  {7,0,7,213,127}, // 01111111

  {0,7,1,128, 42}, // 10000000
  {1,0,2,127,213}, // 10000001
  {0,1,2,126,212}, // 10000010
  {2,0,3,129, 43}, // 10000011
  {0,2,2,124,214}, // 10000100
  {1,0,3,131, 41}, // 10000101
  {0,1,3,130, 40}, // 10000110
  {3,0,4,125,215}, // 10000111

  {0,3,2,120,210}, // 10001000
  {1,0,3,135, 45}, // 10001001
  {0,1,3,134, 44}, // 10001010
  {2,0,4,121,211}, // 10001011
  {0,2,3,132, 46}, // 10001100
  {1,0,4,123,209}, // 10001101
  {0,1,4,122,208}, // 10001110
  {4,0,5,133, 47}, // 10001111

  {0,4,2,112,218}, // 10010000
  {1,0,3,143, 37}, // 10010001
  {0,1,3,142, 36}, // 10010010
  {2,0,4,113,219}, // 10010011
  {0,2,3,140, 38}, // 10010100
  {1,0,4,115,217}, // 10010101
  {0,1,4,114,216}, // 10010110
  {3,0,5,141, 39}, // 10010111

  {0,3,3,136, 34}, // 10011000
  {1,0,4,119,221}, // 10011001
  {0,1,4,118,220}, // 10011010
  {2,0,5,137, 35}, // 10011011
  {0,2,4,116,222}, // 10011100
  {1,0,5,139, 33}, // 10011101
  {0,1,5,138, 32}, // 10011110
  {5,0,6,117,223}, // 10011111

  {0,5,2, 96,202}, // 10100000
  {1,0,3,159, 53}, // 10100001
  {0,1,3,158, 52}, // 10100010
  {2,0,4, 97,203}, // 10100011
  {0,2,3,156, 54}, // 10100100
  {1,0,4, 99,201}, // 10100101
  {0,1,4, 98,200}, // 10100110
  {3,0,5,157, 55}, // 10100111

  {0,3,3,152, 50}, // 10101000
  {1,0,4,103,205}, // 10101001
  {0,1,4,102,204}, // 10101010
  {2,0,5,153, 51}, // 10101011
  {0,2,4,100,206}, // 10101100
  {1,0,5,155, 49}, // 10101101
  {0,1,5,154, 48}, // 10101110
  {4,0,6,101,207}, // 10101111

  {0,4,3,144, 58}, // 10110000
  {1,0,4,111,197}, // 10110001
  {0,1,4,110,196}, // 10110010
  {2,0,5,145, 59}, // 10110011
  {0,2,4,108,198}, // 10110100
  {1,0,5,147, 57}, // 10110101
  {0,1,5,146, 56}, // 10110110
  {3,0,6,109,199}, // 10110111

  {0,3,4,104,194}, // 10111000
  {1,0,5,151, 61}, // 10111001
  {0,1,5,150, 60}, // 10111010
  {2,0,6,105,195}, // 10111011
  {0,2,5,148, 62}, // 10111100
  {1,0,6,107,193}, // 10111101
  {0,1,6,106,192}, // 10111110
  {6,0,7,149, 63}, // 10111111

  {0,6,2, 64,234}, // 11000000
  {1,0,3,191, 21}, // 11000001
  {0,1,3,190, 20}, // 11000010
  {2,0,4, 65,235}, // 11000011
  {0,2,3,188, 22}, // 11000100
  {1,0,4, 67,233}, // 11000101
  {0,1,4, 66,232}, // 11000110
  {3,0,5,189, 23}, // 11000111

  {0,3,3,184, 18}, // 11001000
  {1,0,4, 71,237}, // 11001001
  {0,1,4, 70,236}, // 11001010
  {2,0,5,185, 19}, // 11001011
  {0,2,4, 68,238}, // 11001100
  {1,0,5,187, 17}, // 11001101
  {0,1,5,186, 16}, // 11001110
  {4,0,6, 69,239}, // 11001111

  {0,4,3,176, 26}, // 11010000
  {1,0,4, 79,229}, // 11010001
  {0,1,4, 78,228}, // 11010010
  {2,0,5,177, 27}, // 11010011
  {0,2,4, 76,230}, // 11010100
  {1,0,5,179, 25}, // 11010101
  {0,1,5,178, 24}, // 11010110
  {3,0,6, 77,231}, // 11010111

  {0,3,4, 72,226}, // 11011000
  {1,0,5,183, 29}, // 11011001
  {0,1,5,182, 28}, // 11011010
  {2,0,6, 73,227}, // 11011011
  {0,2,5,180, 30}, // 11011100
  {1,0,6, 75,225}, // 11011101
  {0,1,6, 74,224}, // 11011110
  {5,0,7,181, 31}, // 11011111

  {0,5,3,160, 10}, // 11100000
  {1,0,4, 95,245}, // 11100001
  {0,1,4, 94,244}, // 11100010
  {2,0,5,161, 11}, // 11100011
  {0,2,4, 92,246}, // 11100100
  {1,0,5,163,  9}, // 11100101
  {0,1,5,162,  8}, // 11100110
  {3,0,6, 93,247}, // 11100111

  {0,3,4, 88,242}, // 11101000
  {1,0,5,167, 13}, // 11101001
  {0,1,5,166, 12}, // 11101010
  {2,0,6, 89,243}, // 11101011
  {0,2,5,164, 14}, // 11101100
  {1,0,6, 91,241}, // 11101101
  {0,1,6, 90,240}, // 11101110
  {4,0,7,165, 15}, // 11101111

  {0,4,4, 80,250}, // 11110000
  {1,0,5,175,  5}, // 11110001
  {0,1,5,174,  4}, // 11110010
  {2,0,6, 81,251}, // 11110011
  {0,2,5,172,  6}, // 11110100
  {1,0,6, 83,249}, // 11110101
  {0,1,6, 82,248}, // 11110110
  {3,0,7,173,  7}, // 11110111

  {0,3,5,168,  2}, // 11111000
  {1,0,6, 87,253}, // 11111001
  {0,1,6, 86,252}, // 11111010
  {2,0,7,169,  3}, // 11111011
  {0,2,6, 84,254}, // 11111100
  {1,0,7,171,  1}, // 11111101
  {0,1,7,170,  0}, // 11111110
  {8,0,8, 85,255}, // 11111111
  }
#endif
;


//***************************************************************************
//  Tab Control vars
//***************************************************************************

// Default tab stops
EXTERN
UINT uTabStops
#ifdef DEFINE_VALUES
 = DEF_TABS
#endif
;

// Default six-space indent
EXTERN
WCHAR wszIndent[DEF_INDENT + 1]
#ifdef DEFINE_VALUES
 = {L' ',L' ',L' ',L' ',L' ',L' ',L'\0'}
#endif
;

EXTERN
int gLstTabID                           // ID of the previous (outgoing) tab (-1 = none)
#ifdef DEFINE_VALUES
 = -1
#endif
,
    gCurTabID                           // ID of the current (incoming) tab  (-1 = none)
#ifdef DEFINE_VALUES
 = -1
#endif
,
    gOverTabIndex                       // Index of the tab the mouse is over
                                        // As this is a transient value, we store it as
                                        //   an index rather than an ID
#ifdef DEFINE_VALUES
 = -1
#endif
;

EXTERN
HWND hWndTC,                            // Global Tab Control window handle
     hWndMF,                            // ...    Master Frame ...
     hWndTT;                            // ...    ToolTip      ...

EXTERN
HGLOBAL hGlbZilde,
        hGlbQuadDM,
        hGlbQuadFC,
        hGlbQuadIC,
        hGlbV0Char,
        hGlbM3x0Char,
        hGlbSAEmpty,
        hGlbSAClear,
        hGlbSAError,
        hGlbSAExit,
        hGlbSAOff,
        hGlbQuadWSID_CWS,
        hGlbAV;

EXTERN
APLFLOAT PosInfinity,                   // Positive infinity
         NegInfinity,                   // Negative ...
         Float2Pow53;                   // 2*53 in floating point

typedef enum tagSYS_VARS
{
    SYSVAR_UNK = 0,             // 00:  Unknown name
    SYSVAR_ALX ,                // 01:  []ALX
    SYSVAR_CT  ,                // 02:  []CT
    SYSVAR_ELX ,                // 03:  []ELX
    SYSVAR_FC  ,                // 04:  []FC
    SYSVAR_IC  ,                // 05:  []IC
    SYSVAR_IO  ,                // 06:  []IO
    SYSVAR_LX  ,                // 07:  []LX
    SYSVAR_PP  ,                // 08:  []PP
    SYSVAR_PR  ,                // 09:  []PR
    SYSVAR_PW  ,                // 0A:  []PW
    SYSVAR_RL  ,                // 0B:  []RL
    SYSVAR_SA  ,                // 0C:  []SA
    SYSVAR_WSID,                // 0D:  []WSID
    SYSVAR_LENGTH               // 0E:  # entries in the enum
                                // 0F-1F:  Available entries (5 bits)
} SYS_VARS;

typedef BOOL (*ASYSVARVALIDSET) (LPTOKEN, LPTOKEN);

EXTERN
// Use as in:  (*aSysVarValidSet[SYSVAR_IO]) (lptkNamArg, lptkRhtArg);
ASYSVARVALIDSET aSysVarValidSet[SYSVAR_LENGTH];

typedef BOOL (*ASYSVARVALIDNDX) (APLINT, APLSTYPE, LPAPLLONGEST, LPIMM_TYPES);

EXTERN
// Use as in:  (*aSysVarValidNdx[SYSVAR_IO]) (aplIntegerLst, lpaplIntegerRht, &immTypeRht);
ASYSVARVALIDNDX aSysVarValidNdx[SYSVAR_LENGTH];

EXTERN
int MFSizeState                         // Size state for MF (SIZE_xxx)
#ifdef DEFINE_VALUES
 = SIZE_RESTORED
#endif
;

EXTERN
POINT MFPosCtr;                         // X- and Y- center of Master Frame Window position

EXTERN
SIZE  MFSize;                           // Size of Master Frame Window window rectangle

EXTERN
int iLogPixelsY;                        // # logical pixels/inch in screen Y-dimension

EXTERN
HBITMAP hBitMapLineCont;                // Bitmap for the line continuation char

EXTERN
int     iLCWidth;                       // Width of the line continuation column

EXTERN
BITMAP  bmLineCont;                     // Bitmap metrics for the line continuation char

// FONTS
EXTERN
LOGFONT lfSM                            // LOGFONT for the SM
#ifdef DEFINE_VALUES
 = {DEF_SMLOGFONT}
#endif
,
        lfTC                            // LOGFONT for the TC
#ifdef DEFINE_VALUES
 = {DEF_TCLOGFONT}
#endif
,
        lfFE                            // LOGFONT for the FE
#ifdef DEFINE_VALUES
 = {DEF_FELOGFONT}
#endif
,
        lfME                            // LOGFONT for the ME
#ifdef DEFINE_VALUES
 = {DEF_MELOGFONT}
#endif
,
        lfVE                            // LOGFONT for the VE
#ifdef DEFINE_VALUES
 = {DEF_VELOGFONT}
#endif
;

;

EXTERN
HFONT hFontTC,                          // Handle to font for the TC
      hFontSM,                          // ...                    SM
      hFontFE,                          // ...                    FE
      hFontME,                          // ...                    ME
      hFontVE;                          // ...                    VE

EXTERN
CHOOSEFONT cfTC,                        // Global for ChooseFont for the TC
           cfSM,                        // ...                           SM
           cfFE,                        // ...                           FE
           cfME,                        // ...                           ME
           cfVE;                        // ...                           VE

EXTERN
TEXTMETRIC tmTC,                        // Global for TEXTMETRIC for the TC
           tmSM,                        // ...                           SM
           tmFE,                        // ...                           FE
           tmME,                        // ...                           ME
           tmVE;                        // ...                           VE

EXTERN
long cxAveCharTC, cyAveCharTC,          // Size of an average character in the TC font
     cxAveCharSM, cyAveCharSM,          // ...                                 SM ...
     cxAveCharFE, cyAveCharFE,          // ...                                 FE ...
     cxAveCharME, cyAveCharME,          // ...                                 ME ...
     cxAveCharVE, cyAveCharVE;          // ...                                 VE ...

EXTERN
WNDPROC lpfnOldTabCtrlWndProc;          // Save area for old Tab Control procedure

#ifdef DEBUG_ODS
  EXTERN
  char DebugFile[];

  EXTERN
  HANDLE DebugHandle;
#endif

// Define global options
EXTERN
BOOL bNewTabOnClear
#ifdef DEFINE_VALUES
 = DEF_NEWTABONCLEAR
#endif
,
     bNewTabOnLoad
#ifdef DEFINE_VALUES
 = DEF_NEWTABONLOAD
#endif
,
     bUseLocalTime
#ifdef DEFINE_VALUES
 = TRUE;
#endif
;

EXTERN
char pszNoInsertTCTab[]
#ifdef DEFINE_VALUES
 = "Unable to create a new Tab"
#endif
,
     pszNoEditPrimFn[]
#ifdef DEFINE_VALUES
 = "NONCE ERROR:  Unable to edit named primitive function/operator"
#endif
,
     pszNoCreateFEEditCtrl[]
#ifdef DEFINE_VALUES
 = "Unable to create the Edit Control in the Function Editor"
#endif
,
     pszNoCreateSMEditCtrl[]
#ifdef DEFINE_VALUES
 = "Unable to create the Edit Control in the Session Manager"
#endif
,
     pszNoCreateMCWnd[]
#ifdef DEFINE_VALUES
 = "Unable to create MDI Client window"
#endif
,
     pszNoCreateSMWnd[]
#ifdef DEFINE_VALUES
 = "Unable to create Session Manager window"
#endif
#ifdef DEBUG
  ,
       pszNoCreateDBWnd[]
  #ifdef DEFINE_VALUES
   = "Unable to create Debugger window"
  #endif
#endif
,
     pszNoCreateFEWnd[]
#ifdef DEFINE_VALUES
 = "Unable to create Function Editor window"
#endif
,
     pszNoCreateMEWnd[]
#ifdef DEFINE_VALUES
 = "Unable to create Matrix Editor window"
#endif
,
     pszNoCreateVEWnd[]
#ifdef DEFINE_VALUES
 = "Unable to create Vector Editor window"
#endif
;

EXTERN
WCHAR wszMCTitle[]                      // MDI Client ... (for debugging purposes only)
#ifdef DEFINE_VALUES
 = L"NARS2000 MDI Client Window" LAPPEND_DEBUG
#endif
,
      wszSMTitle[]                      // Session Manager ...
#ifdef DEFINE_VALUES
 = L"NARS2000 Session Manager" LAPPEND_DEBUG
#endif
#ifdef DEBUG
  ,
      wszDBTitle[]                      // Debugger ...
  #ifdef DEFINE_VALUES
   = L"NARS2000 Debugger Window" LAPPEND_DEBUG
  #endif
#endif
,
      wszFETitle[]                      // Function Editor ...
#ifdef DEFINE_VALUES
 = L"NARS2000 Function Editor" LAPPEND_DEBUG
#endif
,
      wszMETitle[]                      // Matrix Editor ...
#ifdef DEFINE_VALUES
 = L"NARS2000 Matrix Editor" LAPPEND_DEBUG
#endif
,
      wszVETitle[]                      // Vector Editor ...
#ifdef DEFINE_VALUES
 = L"NARS2000 Vector Editor" LAPPEND_DEBUG
#endif
;

#define  MCWNDCLASS     "MDIClient"     // MDI Client window class
#define LMCWNDCLASS    L"MDIClient"     // MDI Client ...
#define  SMWNDCLASS     "SMClass"       // Session Manager ...
#define LSMWNDCLASS    L"SMClass"       // Session Manager ...
#ifdef DEBUG
  #define  DBWNDCLASS   "DBClass"       // Debugger     ...
  #define LDBWNDCLASS  L"DBClass"       // Debugger     ...
#endif
#define  FEWNDCLASS     "FEClass"       // Function Editor ...
#define LFEWNDCLASS    L"FEClass"       // Function Editor ...
#define  MEWNDCLASS     "MEClass"       // Matrix Editor ...
#define LMEWNDCLASS    L"MEClass"       // Matrix Editor ...
#define  VEWNDCLASS     "VEClass"       // Vector Editor ...
#define LVEWNDCLASS    L"VEClass"       // Vector Editor ...
#define  ECWNDCLASS     "ECClass"       // Edit Control ...
#define LECWNDCLASS    L"ECClass"       // Edit Control ...

typedef enum tagMEMVIRTENUM
{
    MEMVIRT_SZTEMP = 0,                 // 00:  lpszTemp
    MEMVIRT_WSZTEMP,                    // 01:  lpwszTemp
#ifdef DEBUG
    MEMVIRT_SZDEBUG,                    // 02:  lpszDebug
    MEMVIRT_WSZDEBUG,                   // 03:  lpwszDebug
#endif
    MEMVIRT_LENGTH                      // 04:  # entries
} MEMVIRTENUM;

#define MVS     struct tagMEMVIRTSTR

typedef struct tagMEMVIRTSTR
{
    MVS     *lpPrvMVS,                  // Ptr to previous link (NULL = none)
            *lpNxtMVS;                  // Ptr to next     ...
    LPUCHAR IniAddr;                    // Initial address
    UINT    IncrSize,                   // Incremental size in bytes
            MaxSize;                    // Maximum     ...
#ifdef DEBUG
    LPCHAR  lpText;                     // Ptr to (const) description of this item
#endif
} MEMVIRTSTR, *LPMEMVIRTSTR;

EXTERN
MEMVIRTSTR memVirtStr[MEMVIRT_LENGTH];

EXTERN
UINT uMemVirtCnt
#ifdef DEFINE_VALUES
 = MEMVIRT_LENGTH
#endif
;

EXTERN
HIMAGELIST hImageList;                  // Handle to the common image list

// Same order as in ARRAY_TYPES
// so that BOOL < INT < FLOAT < APA < CHAR < HETERO < NESTED
EXTERN
UINT uTypeMap[]
#ifdef DEFINE_VALUES
//  BOOL, INT, FLOAT, CHAR, HETERO, NESTED, LIST, APA
 = {   0,   1,     2,    4,      5,      6,    7,   3}
#endif
;

APLSTYPE aTypePromote[ARRAY_LENGTH][ARRAY_LENGTH]
#ifdef DEFINE_VALUES
=
//      BOOL          INT           FLOAT         CHAR        HETERO        NESTED        LIST         APA
{{ARRAY_BOOL  , ARRAY_INT   , ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_ERROR , ARRAY_INT   },  // BOOL
 {ARRAY_INT   , ARRAY_INT   , ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_ERROR , ARRAY_INT   },  // INT
 {ARRAY_FLOAT , ARRAY_FLOAT , ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_ERROR , ARRAY_FLOAT },  // FLOAT
 {ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_CHAR  , ARRAY_HETERO, ARRAY_NESTED, ARRAY_ERROR , ARRAY_HETERO},  // CHAR
 {ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_ERROR , ARRAY_HETERO},  // HETERO
 {ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_ERROR , ARRAY_NESTED},  // NESTED
 {ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR },  // LIST
 {ARRAY_INT   , ARRAY_INT   , ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_ERROR , ARRAY_INT   },  // APA
}
#endif
;

typedef struct
{
    char  nrm;      // 00:  Normal           (shifted & unshifted) (unused)
    WCHAR alt;      // 01:  Alt key pressed  (shifted & unshifted)
} CHARCODE;

// If you are looking for places on the keyboard to put a new symbol,
//   there are fifteen free Alt-Shift- combinations:
//     Alt-'A'
//     Alt-'B'
//     Alt-'C'
//     Alt-'D'
//     Alt-'F'
//     Alt-'K'
//     Alt-'S'
//     Alt-'U'
//     Alt-'Q'
//     Alt-'R'
//     Alt-'V'
//     Alt-'W'
//     Alt-'X'
//     Alt-'Y'
//     Alt-'Z'
//     Alt-'<'
//     Alt-'>'
//     Alt-'?'
//
//   as well as several duplicated symbols:
//
//     Symbol & Name              Keystroke
//     --------------------------------------------------
//     * Asterisk                 Alt-'p'
//     = Equal                    Alt-'5'
//     < Left Caret               Alt-'3'
//     ? Question Mark            Alt-'q'
//     > Right Caret              Alt-'7'
//     ! Shreik                   Alt-'_'
//     ' Single Quote             Alt-'k'
//     | Stile                    Alt-'m'
//     ~ Tilde                    Alt-'t'
//     ^ Up Caret (Circumflex)    Alt-'0'

EXTERN
CHARCODE aCharCodes[1+126-32]   // This ordering follows the ASCII charset
                                //   from 32 to 126 inclusive
#ifdef DEFINE_VALUES
=
{
//Nrm Alt
{' ', 0                       },  // Space             32
{'!', UTF16_EQUALUNDERBAR     },  // Quote-dot         33
{'"', 0                       },  // Quotation mark    34
{'#', UTF16_DELSTILE          },  // Number sign       35
{'$', UTF16_DELTASTILE        },  // Dollar sign       36
{'%', UTF16_CIRCLESTILE       },  // Percent sign      37
{'&', UTF16_CIRCLEBAR         },  // Ampersand         38
{'\'',UTF16_DOWNTACKJOT       },  // Apostrophe        39
{'(', UTF16_DOWNCARETTILDE    },  // Left paren        40
{')', UTF16_UPCARETTILDE      },  // Right paren       41
{'*', UTF16_CIRCLESTAR        },  // Star              42
{'+', UTF16_DOMINO            },  // Plus sign         43
{',', UTF16_LAMP              },  // Comma             44
{'-', UTF16_TIMES             },  // Bar               45
{'.', UTF16_SLOPEBAR          },  // Dot               46
{'/', UTF16_SLASHBAR          },  // Slash             47
{'0', UTF16_UPCARET           },  // 0                 48
{'1', UTF16_DIERESIS          },  // 1                 49
{'2', UTF16_OVERBAR           },  // 2                 50
{'3', UTF16_LEFTCARET         },  // 3                 51
{'4', UTF16_LEFTCARETUNDERBAR },  // 4                 52
{'5', UTF16_EQUAL             },  // 5                 53
{'6', UTF16_RIGHTCARETUNDERBAR},  // 6                 54
{'7', UTF16_RIGHTCARET        },  // 7                 55
{'8', UTF16_NOTEQUAL          },  // 8                 56
{'9', UTF16_DOWNCARET         },  // 9                 57
{':', 0                       },  // Colon             58
{';', UTF16_UPTACKJOT         },  // Semicolon         59
{'<', 0                       },  // Less              60
{'=', UTF16_COLONBAR          },  // Equal             61
{'>', 0                       },  // More              62
{'?', 0                       },  // Query             63
{'@', UTF16_CIRCLEMIDDLEDOT   },  // At sign           64
{'A', 0                       },  // A                 65
{'B', 0                       },  // B                 66
{'C', 0                       },  // C                 67
{'D', 0                       },  // D                 68
{'E', UTF16_EPSILONUNDERBAR   },  // E                 69
{'F', 0                       },  // F                 70
{'G', UTF16_DIERESISDEL       },  // G                 71
{'H', UTF16_DELTAUNDERBAR     },  // H                 72
{'I', UTF16_IOTAUNDERBAR      },  // I                 73
{'J', UTF16_DIERESISJOT       },  // J                 74
{'K', 0                       },  // K                 75
{'L', UTF16_SQUAD             },  // L                 76
{'M', UTF16_STILETILDE        },  // M                 77
{'N', UTF16_DIERESISDOWNTACK  },  // N                 78
{'O', UTF16_DIERESISCIRCLE    },  // O                 79
{'P', UTF16_DIERESISSTAR      },  // P                 80
{'Q', 0                       },  // Q                 81
{'R', 0                       },  // R                 82
{'S', 0                       },  // S                 83
{'T', UTF16_DIERESISTILDE     },  // T                 84
{'U', 0                       },  // U                 85
{'V', 0                       },  // V                 86
{'W', 0                       },  // W                 87
{'X', 0                       },  // X                 88
{'Y', 0                       },  // Y                 89
{'Z', 0                       },  // Z                 90
{'[', UTF16_LEFTARROW         },  // Left bracket      91
{'\\',UTF16_RIGHTTACK         },  // Slope             92
{']', UTF16_RIGHTARROW        },  // Right bracket     93
{'^', UTF16_CIRCLESLOPE       },  // Up caret          94
{'_', UTF16_QUOTEDOT          },  // Underbar          95
{'`', UTF16_DIAMOND           },  // Grave accent      96
{'a', UTF16_ALPHA             },  // a                 97
{'b', UTF16_UPTACK            },  // b                 98
{'c', UTF16_UPSHOE            },  // c                 99
{'d', UTF16_DOWNSTILE         },  // d                100
{'e', UTF16_EPSILON           },  // e                101
{'f', UTF16_INFINITY          },  // f                102
{'g', UTF16_DEL               },  // g                103
{'h', UTF16_DELTA             },  // h                104
{'i', UTF16_IOTA              },  // i                105
{'j', UTF16_JOT               },  // j                106
{'k', UTF16_APOSTROPHE        },  // k                107
{'l', UTF16_QUAD              },  // l                108
{'m', UTF16_STILE             },  // m                109
{'n', UTF16_DOWNTACK          },  // n                110
{'o', UTF16_CIRCLE            },  // o                111
{'p', UTF16_STAR              },  // p                112
{'q', UTF16_QUERY             },  // q                113
{'r', UTF16_RHO               },  // r                114
{'s', UTF16_UPSTILE           },  // s                115
{'t', UTF16_TILDE             },  // t                116
{'u', UTF16_DOWNARROW         },  // u                117
{'v', UTF16_DOWNSHOE          },  // v                118
{'w', UTF16_OMEGA             },  // w                119
{'x', UTF16_RIGHTSHOE         },  // x                120
{'y', UTF16_UPARROW           },  // y                121
{'z', UTF16_LEFTSHOE          },  // z                122
{'{', UTF16_QUOTEQUAD         },  // Left brace       123
{'|', UTF16_LEFTTACK          },  // Stile            124
{'}', UTF16_ZILDE             },  // Right brace      125
{'~', UTF16_COMMABAR          },  // Tilde            126
}
#endif
;

// The # rows in the above table
#define ACHARCODES_NROWS    (sizeof (aCharCodes) / sizeof (aCharCodes[0]))

typedef struct tagSYMBOLNAMES
{
    WCHAR   Symbol;     // Symbol
    LPWCHAR lpwName;    // Ptr to name
} SYMBOLNAMES, *LPSYMBOLNAMES;

// The # rows in the below table
#define ASYMBOLNAMES_NROWS  91

// Translate table for symbols to names
EXTERN
SYMBOLNAMES aSymbolNames[ASYMBOLNAMES_NROWS]
#ifdef DEFINE_VALUES
=
{
// No keystroke equivalents for these as yet
  {UTF16_DIERESISDOT          , L"{dieresisdot}"        },
  {UTF16_NOTEQUALUNDERBAR     , L"{notequalunderbar}"   },
  {UTF16_IBEAM                , L"{ibeam}"              },
  {UTF16_QUADJOT              , L"{quadjot}"            },
  {UTF16_QUADSLOPE            , L"{quadslope}"          },
  {UTF16_QUADLEFTARROW        , L"{quadleftarrow}"      },
  {UTF16_QUADRIGHTARROW       , L"{quadrightarrow}"     },
  {UTF16_QUADUPARROW          , L"{quaduparrow}"        },
  {UTF16_QUADDOWNARROW        , L"{quaddownarrow}"      },

// The alphabet, unshifted
  {UTF16_ALPHA                , L"{alpha}"              },  // Alt-'a' - alpha
  {UTF16_UPTACK               , L"{uptack}"             },  // Alt-'b' - base
  {UTF16_UPSHOE               , L"{upshoe}"             },  // Alt-'c' - intersection
  {UTF16_DOWNSTILE            , L"{downstile}"          },  // Alt-'d' - floor
  {UTF16_EPSILON              , L"{epsilon}"            },  // Alt-'e' - epsilon
  {UTF16_INFINITY             , L"{infinity}"           },  // Alt-'f' - infinity
  {UTF16_DEL                  , L"{del}"                },  // Alt-'g' - del
  {UTF16_DELTA                , L"{delta}"              },  // Alt-'h' - delta
  {UTF16_IOTA                 , L"{iota}"               },  // Alt-'i' - iota
  {UTF16_JOT                  , L"{jot}"                },  // Alt-'j' - jot
  {UTF16_APOSTROPHE           , L"{apostrophe}"         },  // Alt-'k' - single-quote
  {UTF16_QUAD                 , L"{quad}"               },  // Alt-'l' - quad (9109??)
  {UTF16_STILE                , L"{stile}"              },  // Alt-'m' - modulus
  {UTF16_DOWNTACK             , L"{downtack}"           },  // Alt-'n' - representation
  {UTF16_CIRCLE               , L"{circle}"             },  // Alt-'o' - circle
  {UTF16_STAR                 , L"{star}"               },  // Alt-'p' - power
  {UTF16_QUERY                , L"{query}"              },  // Alt-'q' - question-mark
  {UTF16_RHO                  , L"{rho}"                },  // Alt-'r' - rho
  {UTF16_UPSTILE              , L"{upstile}"            },  // Alt-'s' - ceiling
  {UTF16_TILDE                , L"{tilde}"              },  // Alt-'t' - tilde
  {UTF16_DOWNARROW            , L"{downarrow}"          },  // Alt-'u' - down arrow
  {UTF16_DOWNSHOE             , L"{downshoe}"           },  // Alt-'v' - union
  {UTF16_OMEGA                , L"{omega}"              },  // Alt-'w' - omega
  {UTF16_RIGHTSHOE            , L"{rightshoe}"          },  // Alt-'x' - disclose
  {UTF16_UPARROW              , L"{uparrow}"            },  // Alt-'y' - up arrow
  {UTF16_LEFTSHOE             , L"{leftshoe}"           },  // Alt-'z' - enclose

// The alphabet, shifted
//{UTF16_                     ,                         },  // Alt-'A' - (none)
//{UTF16_                     ,                         },  // Alt-'B' - (none)
//{UTF16_                     ,                         },  // Alt-'C' - (none)
//{UTF16_                     ,                         },  // Alt-'D' - (none)
  {UTF16_EPSILONUNDERBAR      , L"{epsilonunderbar}"    },  // Alt-'E' - epsilon-underbar
//{UTF16_                     ,                         },  // Alt-'F' - (none)
  {UTF16_DIERESISDEL          , L"{dieresisdel}"        },  // Alt-'G' - Dual operator        (frog)
  {UTF16_DELTAUNDERBAR        , L"{deltaunderbar}"      },  // Alt-'H' - delta-underbar
  {UTF16_IOTAUNDERBAR         , L"{iotaunderbar}"       },  // Alt-'I' - iota-underbar
  {UTF16_DIERESISJOT          , L"{dieresisjot}"        },  // Alt-'J' - Rank operator        (hoot)
//{UTF16_                     ,                         },  // Alt-'K' - (none)
  {UTF16_SQUAD                , L"{squad}"              },  // Alt-'L' - squad
  {UTF16_STILETILDE           , L"{stiletilde}"         },  // Alt-'M' - Partition operator   (dagger)
  {UTF16_DIERESISDOWNTACK     , L"{dieresisdowntack}"   },  // Alt-'N' - Convolution operator (snout)
  {UTF16_DIERESISCIRCLE       , L"{dieresiscircle}"     },  // Alt-'O' - Rank operator w/Axis (holler)
  {UTF16_DIERESISSTAR         , L"{dieresisstar}"       },  // Alt-'P' - Power operator       (sourpuss)
//{UTF16_                     ,                         },  // Alt-'Q' - (none)
//{UTF16_                     ,                         },  // Alt-'R' - (none)
//{UTF16_                     ,                         },  // Alt-'S' - (none)
  {UTF16_DIERESISTILDE        , L"{dieresistilde}"      },  // Alt-'T' - Commute operator     (frown)
//{UTF16_                     ,                         },  // Alt-'U' - (none)
//{UTF16_                     ,                         },  // Alt-'V' - (none)
//{UTF16_                     ,                         },  // Alt-'W' - (none)
//{UTF16_                     ,                         },  // Alt-'X' - (none)
//{UTF16_                     ,                         },  // Alt-'Y' - (none)
//{UTF16_                     ,                         },  // Alt-'Z' - (none)

// Top row, unshifted
  {UTF16_DIAMOND              , L"{diamond}"            },  // Alt-'`' - diamond (9674??)
  {UTF16_DIERESIS             , L"{dieresis}"           },  // Alt-'1' - dieresis
  {UTF16_OVERBAR              , L"{overbar}"            },  // Alt-'2' - high minus
  {UTF16_LEFTCARET            , L"{leftcaret}"          },  // Alt-'3' - less
  {UTF16_LEFTCARETUNDERBAR    , L"{leftcaretunderbar}"  },  // Alt-'4' - not more
  {UTF16_EQUAL                , L"{equal}"              },  // Alt-'5' - equal
  {UTF16_RIGHTCARETUNDERBAR   , L"{rightcaretunderbar}" },  // Alt-'6' - not less
  {UTF16_RIGHTCARET           , L"{rightcaret}"         },  // Alt-'7' - more
  {UTF16_NOTEQUAL             , L"{notequal}"           },  // Alt-'8' - not equal
  {UTF16_DOWNCARET            , L"{downcaret}"          },  // Alt-'9' - or
  {UTF16_UPCARET              , L"{upcaret}"            },  // Alt-'0' - and (94??)
  {UTF16_TIMES                , L"{times}"              },  // Alt-'-' - times
  {UTF16_COLONBAR             , L"{colonbar}"           },  // Alt-'=' - divide

// Top row, shifted
  {UTF16_COMMABAR             , L"{commabar}"           },  // Alt-'~' - comma-bar
  {UTF16_EQUALUNDERBAR        , L"{equalunderbar}"      },  // Alt-'!' - match
  {UTF16_CIRCLEMIDDLEDOT      , L"{circlemiddledot}"    },  // Alt-'@' - circle-middle-dot
  {UTF16_DELSTILE             , L"{delstile}"           },  // Alt-'#' - grade-down
  {UTF16_DELTASTILE           , L"{deltastile}"         },  // Alt-'$' - grade-up
  {UTF16_CIRCLESTILE          , L"{circlestile}"        },  // Alt-'%' - rotate
  {UTF16_CIRCLESLOPE          , L"{circleslope}"        },  // Alt-'^' - transpose
  {UTF16_CIRCLEBAR            , L"{circlebar}"          },  // Alt-'&' - circle-bar
  {UTF16_CIRCLESTAR           , L"{circlestar}"         },  // Alt-'*' - log
  {UTF16_DOWNCARETTILDE       , L"{downcarettilde}"     },  // Alt-'(' - nor
  {UTF16_UPCARETTILDE         , L"{upcarettilde}"       },  // Alt-')' - nand
  {UTF16_QUOTEDOT             , L"{quotedot}"           },  // Alt-'_' - quote-dot
  {UTF16_DOMINO               , L"{domino}"             },  // Alt-'+' - domino

// Second row, unshifted
  {UTF16_LEFTARROW            , L"{leftarrow}"          },  // Alt-'[' - left arrow
  {UTF16_RIGHTARROW           , L"{rightarrow}"         },  // Alt-']' - right arrow
  {UTF16_LEFTTACK             , L"{lefttack}"           },  // Alt-'\' - left tack

// Second row, shifted
  {UTF16_QUOTEQUAD            , L"{quotequad}"          },  // Alt-'{' - quote-quad
  {UTF16_ZILDE                , L"{zilde}"              },  // Alt-'}' - zilde
  {UTF16_RIGHTTACK            , L"{righttack}"          },  // Alt-'|' - right tack

// Third row, unshifted
  {UTF16_UPTACKJOT            , L"{uptackjot}"          },  // Alt-';' - execute
  {UTF16_DOWNTACKJOT          , L"{downtackjot}"        },  // Alt-'\''- format

// Third row, shifted
//{UTF16_                     ,                         },  // Alt-':' - (none)
//{UTF16_                     ,                         },  // Alt-'"' - (none)

// Fourth row, unshifted
  {UTF16_LAMP                 , L"{lamp}"               },  // Alt-',' - comment
  {UTF16_SLOPEBAR             , L"{slopebar}"           },  // Alt-'.' - slope-bar
  {UTF16_SLASHBAR             , L"{slashbar}"           },  // Alt-'/' - slash-bar

// Fourth row, shifted
//{UTF16_                     ,                         },  // Alt-'<' - (none)
//{UTF16_                     ,                         },  // Alt-'>' - (none)
//{UTF16_                     ,                         },  // Alt-'?' - (none)

// Non-Alt key equivalents (these are the only ones we need for SaveWS)
  {UTF16_DOUBLEQUOTE          , L"{doublequote}"        },  // 22:  Double quote
  {UTF16_POUND                , L"{pound}"              },  // 23:  Pound
  {UTF16_COMMA                , L"{comma}"              },  // 2C:  Comma
  {UTF16_SLOPE                , L"{slope}"              },  // 5C:  Slope
  {UTF16_CIRCUMFLEX           , L"{circumflex}"         },  // 5E:  Circumflex
  {UTF16_LBRACE               , L"{leftbrace}"          },  // 7B:  Left brace
  {UTF16_STILE2               , L"{stile2}"             },  // 7C:  Stile (a.k.a. 0x2223)
  {UTF16_RBRACE               , L"{rightbrace}"         },  // 7D:  Right brace
}
#endif
;

typedef enum tagUNDO_ACTS
{
    undoNone = 0,       // 0000:  No action
    undoIns,            // 0001:  Insert a character
    undoRep,            // 0002:  Replace a character
    undoDel,            // 0003:  Delete one or more characters
    undoSel,            // 0004:  Select one or more characters
    undoInsToggle,      // 0005:  Toggle the insert mode
                        // 0006-FFFF:  Available entries (16 bits)
} UNDO_ACTS;

// Define the corresponding one-letter actions
#define UndoActToChar   L"NIRDST"

#define UNDO_NOGROUP    0

typedef struct tagUNDO_BUF
{
    UINT  CharPosBeg,   // 00:  Beginning character position (from start of text),
                        //      -1 = current position
          CharPosEnd,   // 04:  Ending    ...
          Group;        // 08:  Group index identifies actions to be performed together,
                        //      0 = no grouping
    short Action;       // 0C:  Action (see UNDO_ACTS)
    WCHAR Char;         // 0E:  The character (if any),
                        //       0 = none
                        // 10:  Length
} UNDO_BUF, *LPUNDO_BUF;


typedef union tagMEMTXT_UNION
{
    struct
    {
        UINT U;             // 00:  The line length
        union
        {
            APLCHAR C;      // 04:  Followed by an APLCHAR
            WORD    W;      // 04:  ...          a WORD
        };
    };
} MEMTXT_UNION, *LPMEMTXT_UNION;

typedef void (*LPERRHANDFN) (LPWCHAR lpwszMsg,
                             LPWCHAR lpwszLine,
                             UINT uCaret,
                             HWND hWndEC);
typedef enum tagERROR_CODES
{
    ERRORCODE_NONE = 0,     // 00:  No error
    ERRORCODE_ALX,          // 01:  Signal []ALX
    ERRORCODE_ELX,          // 02:  Signal []ELX
} ERROR_CODES;

typedef enum tagLINE_NUMS   // Starting line #s
{
    LINENUM_ONE = 0,        // 00:  Line #1
    LINENUM_IDENTITY,       // 01:  Line []IDENTITY
    LINENUM_INVERSE,        // 02:  Line []INVERSE
    LINENUM_PROTOTYPE,      // 03:  Line []PROTOTYPE
    LINENUM_SINGLETON,      // 04:  Line []SINGLETON
} LINE_NUMS;

#define ENUMS_DEFINED
#undef  EXTERN


//***************************************************************************
//  End of File: externs.h
//***************************************************************************
