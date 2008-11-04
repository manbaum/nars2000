//***************************************************************************
//	NARS2000 -- Extern Variables
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

#include <commctrl.h>
#include "defines.h"
#include "datatype.h"
#include "tokens.h"
#include "primfns.h"
#include "symtab.h"
#include "pl_parse.h"
#include "Unicode.h"

// Define variables which are also used in the per tab structure
#include "primspec.h"


#ifdef DEFINE_VARS
  #define EXTERN
#else
  #define EXTERN extern
#endif

// Define struct for passing parameters to WM_NCCREATE/WM_CREATE
//	 for the Session Manager
typedef struct tagSM_CREATESTRUCTW
{
	HGLOBAL hGlbDPFE;		// 00:	Workspace DPFE global memory handle
	UBOOL	 bExecLX;		// 04:	TRUE iff execute []LX after successful load
							// 08:	Length
} SM_CREATESTRUCTW, UNALIGNED *LPSM_CREATESTRUCTW;


//***************************************************************************
//	Default global values of system variables -- these values
//	  are used to set the variables in a CLEAR WS.
//***************************************************************************

// Indeterminate Control Values
typedef enum tagIC_VALUES
{
	ICVAL_ZERO, 			// 00:	Result is 0
	ICVAL_ONE,				// 01:	... 	  1
	ICVAL_DOMAIN_ERROR, 	// 02:	... 	  DOMAIN ERROR
	ICVAL_POS_INFINITY, 	// 03:	... 	  + infinity
	ICVAL_NEG_INFINITY, 	// 04:	... 	  - infinity
	ICVAL_LENGTH,			// 05:	Length
} IC_VALUES;

// N.B.:  Whenever changing the above enum (IC_VALUES),
//	 be sure to make a corresponding change to
//	 <icIndexValues> in <customize.c>.

// Define the maximum allowable value for []IC
#define ICVAL_MAXVAL	(ICVAL_LENGTH - 1)

// Indeterminate Control Indices
typedef enum tagIC_INDICES
{
	ICNDX_DIV0, 			// 00:	  {div} 0
	ICNDX_LOG0, 			// 01:	  {log} 0
	ICNDX_QDOTn,			// 02:		!N for integer N < 0
	ICNDX_0MULPi,			// 03:	0 {times} _
	ICNDX_0MULNi,			// 04:	0 {times} {neg}_
	ICNDX_0DIV0,			// 05:	0 {div} 0
	ICNDX_PiDIVPi,			// 06:	_ {div} _	(same sign)
	ICNDX_NiDIVPi,			// 07:	_ {div} _	(different sign)
	ICNDX_0EXP0,			// 08:	0	*	0
	ICNDX_NEXPPi,			// 09:	L	*	_ for L <= -1
	ICNDX_0LOG0,			// 0A:	0 {log} 0
	ICNDX_0LOG1,			// 0B:	0 {log} 1
	ICNDX_1LOG0,			// 0C:	1 {log} 0
	ICNDX_1LOG1,			// 0D:	1 {log} 1
	ICNDX_LENGTH,			// 0E:	Length
} IC_INDICES;

// N.B.:  Whenever changing the above enum (IC_INDICES),
//	 be sure to make a corresponding change to
//	 <icIndexNames> in <customize.c>.

EXTERN
APLINT	 aplDefaultIC[ICNDX_LENGTH] 	// []IC
#ifdef DEFINE_VALUES
 = {ICVAL_POS_INFINITY, 	// 00:	  {div} 0
	ICVAL_NEG_INFINITY, 	// 01:	  {log} 0
	ICVAL_DOMAIN_ERROR, 	// 02:		! {neg}1
	ICVAL_DOMAIN_ERROR, 	// 03:	0 {times} _
	ICVAL_DOMAIN_ERROR, 	// 04:	0 {times} {neg}_
	ICVAL_ONE,				// 05:	0 {div} 0
	ICVAL_DOMAIN_ERROR, 	// 06:	_ {div} _	(same sign)
	ICVAL_DOMAIN_ERROR, 	// 07:	_ {div} _	(different sign)
	ICVAL_ONE,				// 08:	0	*	0
	ICVAL_DOMAIN_ERROR, 	// 09:	L	*	_ for L <= -1
	ICVAL_DOMAIN_ERROR, 	// 0A:	0 {log} 0
	ICVAL_DOMAIN_ERROR, 	// 0B:	0 {log} 1
	ICVAL_DOMAIN_ERROR, 	// 0C:	1 {log} 0
	ICVAL_ONE,				// 0D:	1 {log} 1
   }
#endif
;

EXTERN
HGLOBAL  hGlbQuadALX_CWS	,			// []ALX	([]dm)
		 hGlbQuadELX_CWS	,			// []ELX	([]dm)
		 hGlbQuadFC_SYS 	,			// []FC 	(L".,*0_" WS_UTF16_OVERBAR)
		 hGlbQuadFC_CWS 	,			// []FC 	hGlbQuadFC_SYS or from )LOAD
		 hGlbQuadIC_SYS 	,			// []IC 	(aplDefaultIC)
		 hGlbQuadIC_CWS 	,			// []IC 	hGlbQuadIC_SYS or from )LOAD
		 hGlbQuadLX_CWS 	,			// []LX 	(L"")
		 hGlbQuadSA_CWS 	,			// []SA 	(L"")
		 hGlbQuadWSID_CWS	,			// []WSID	(L"\0")
		 hGlbQuadPR_CWS 	;			// []PR 	(L"") (When an empty vector)
EXTERN
APLFLOAT fQuadCT_CWS		;			// []CT

EXTERN
APLBOOL  bQuadIO_CWS		;			// []IO

EXTERN
APLINT	 uQuadMF_CWS		;			// []MF

EXTERN
APLUINT  uQuadPP_CWS		,			// []PP
		 uQuadPW_CWS		,			// []PW
		 uQuadRL_CWS		;			// []RL

EXTERN
APLCHAR  cQuadPR_CWS		,			// []PR 	(L' ') (When a char scalar)
		 cQuadxSA_CWS		;			// []SA 	(0)    (in its index form as an integer)

// Struct for whether or a System var is range limited
typedef struct tagRANGELIMIT
{
	UBOOL CT:1,
		  IC:1,
		  IO:1,
		  PP:1,
		  PW:1,
		  RL:1;
} RANGELIMIT;

EXTERN
RANGELIMIT bRangeLimit
#ifdef DEFINE_VALUES
= {DEF_RANGELIMIT_CT,		// []CT
   DEF_RANGELIMIT_IC,		// []IC
   DEF_RANGELIMIT_IO,		// []IO
   DEF_RANGELIMIT_PP,		// []PP
   DEF_RANGELIMIT_PW,		// []PW
   DEF_RANGELIMIT_RL,		// []RL
  }
#endif
;

// Struct for whether or not the value given to a System Var
//	 when an empty vector is assigned to it is the system default
//	 constant such as DEF_QUADxx_CWS (TRUE) or the value saved
//	 in the .ini file (FALSE)
typedef struct tagRESET_VARS
{
	UBOOL CT:1,
		  FC:1,
		  IC:1,
		  IO:1,
		  PP:1,
		  PW:1,
		  RL:1;
} RESET_VARS;

EXTERN
RESET_VARS bResetVars
#ifdef DEFINE_VALUES
= {DEF_RESETVARS_CT,		// []CT
   DEF_RESETVARS_FC,		// []FC
   DEF_RESETVARS_IC,		// []IC
   DEF_RESETVARS_IO,		// []IO
   DEF_RESETVARS_PP,		// []PP
   DEF_RESETVARS_PW,		// []PW
   DEF_RESETVARS_RL,		// []RL
  }
#endif
;


//***************************************************************************
//	Application values
//***************************************************************************

EXTERN
HINSTANCE _hInstance;					// Global instance handle

EXTERN
HANDLE hAccel;							// Keyboard accelerators

EXTERN
HMENU hMenuSM,							// Handle for Session Manager menu
	  hMenuFE,							// ...		  Function Editor ...
	  hMenuME,							// ...		  Matrix   ...
	  hMenuVE,							// ...		  Vector   ...
	  hMenuSMWindow,					// ...		  window entry in hMenuSM
	  hMenuFEWindow,					// ...		  ...			  hMenuFE
	  hMenuMEWindow,					// ...		  ...			  hMenuME
	  hMenuVEWindow;					// ...		  ...			  hMenuVE

EXTERN
WCHAR lpwszAppName[]					// Application name for MessageBox
#ifdef DEFINE_VALUES
 = WS_APPNAME WS_APPEND_DEBUG
#endif
;

EXTERN
char lpszAppName[]						// Application name for MessageBox
#ifdef DEFINE_VALUES
 = APPNAME APPEND_DEBUG
#endif
;

EXTERN
WCHAR wszAppDPFE [_MAX_PATH],			// .EXE drive, path, filename.ext
	  wszHlpDPFE [_MAX_PATH],			// .HLP ...
	  wszFntDPFE [_MAX_PATH],			// APL font file ...
	  wszLoadFile[_MAX_PATH],			// Save area for initial workspace to load
	  wszFileVer[64];					// File version string

EXTERN
LPWCHAR lpwszIniFile,					// Ptr to "APPNAME.ini" file
		lpwszWorkDir;					// Ptr to WS_WKSNAME dir

EXTERN
UBOOL bCSO; 							// TRUE iff Critical Sections defined

EXTERN
CRITICAL_SECTION CSO0,					// Critical Section Object #0
				 CSO1,					// ...					   #1
#ifdef DEBUG
				 CSOFrisk,				// ...					   for HshTabFrisk
#endif
#ifdef RESDEBUG
				 CSORsrc,				// ...					   for _SaveObj/_DeleObj
#endif
				 CSOPL; 				// ...					   for ParseLine

LRESULT WINAPI EditWndProcA (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI EditWndProcW (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


//***************************************************************************
// Thread-related Variables
//***************************************************************************

EXTERN
DWORD dwMainThreadId;					// Thread ID of the main application

EXTERN
DWORD dwTlsType,						// Thread type (e.g.
										//	 'MF' for Master Frame,
										//	 'TC' for Tab Control,
										//	 'PL' for ParseLine,
										//	 etc.)
	  dwTlsPlLocalVars, 				// Ptr to lpplLocalVars for PL thread only
	  dwTlsFhLocalVars, 				// Ptr to lpfhLocalVars for SM and PL threads
	  dwTlsPerTabData;					// Ptr to PerTabData	for TC, SM, and PL threads

//***************************************************************************
// Temporary storage
//***************************************************************************

EXTERN
LPWCHAR lpwszGlbTemp;					// Used for temporary WCHAR storage

EXTERN
UCHAR gDbgLvl							// Debug level 0 = none
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

EXTERN
LPWCHAR lpwObjNameStr[]
#ifdef DEFINE_VALUES
 = OBJNAME_WSTRPTR
#endif
;

//***************************************************************************
//	Primitive function and operator tables
//***************************************************************************

EXTERN
LPPRIMFNS PrimFnsTab[256];				// The jump table for all primitive functions

EXTERN
LPPRIMFNS PrimProtoFnsTab[256]; 		// The jump table for all primitive functions/operator prototypes

EXTERN
LPPRIMSPEC PrimSpecTab[256];			// The table of corresponding LPPRIMSPECs
										//	 for all of the primitive scalar functions
typedef struct tagPRIMFLAGS
{
	WORD Index	  :4,					// 000F:  Function index (see FBFN_INDS)
		 Available:5,					// 01F0:  Available flag bits
		 IdentElem:1,					// 0200:  TRUE iff this function has an identity element
		 DydScalar:1,					// 0400:  ...					 is scalar dyadic
		 MonScalar:1,					// 0800:  ...						...    monadic
		 Alter	  :1,					// 1000:  ...						alternating
		 AssocBool:1,					// 2000:  ...						associative on Booleans only
		 AssocNumb:1,					// 4000:  ...						associative on all numbers
		 FastBool :1;					// 8000:  Boolean function w/reduction & scan can be sped up
										// 0000:  No available bits
} PRIMFLAGS, *LPPRIMFLAGS;

EXTERN
PRIMFLAGS PrimFlags[256];				// The flag tables for all primitive functions/operators


//***************************************************************************
//	Fast Boolean Reduction and Scan tables
//***************************************************************************

typedef enum tagFBFN_INDS				// Fast Boolean function indices
{	PF_INDEX_UNK = 0 ,					// 00 = No entry so we can catch this as an error
	PF_INDEX_LESS	 ,					// 01 = Index for "less than" ...
	PF_INDEX_OR 	 ,					// 02 = ... 	  "or"    ...
	PF_INDEX_NOR	 ,					// 03 = ... 	  "nor"   ...
	PF_INDEX_MOREEQ  ,					// 04 = ... 	  "more than or equal" ...
	PF_INDEX_NAND	 ,					// 05 = ... 	  "nand"  ...
	PF_INDEX_MORE	 ,					// 06 = ... 	  "more than" ...
	PF_INDEX_LESSEQ  ,					// 07 = ... 	  "less than or equal" ...
	PF_INDEX_AND	 ,					// 08 = ... 	  "and"   ...
	PF_INDEX_EQUAL	 ,					// 09 = ... 	  "equal" ...
	PF_INDEX_NOTEQUAL,					// 0A = ... 	  "not equal" ...
	PF_INDEX_MAX	 ,					// 0B = ... 	  "max"   ...
	PF_INDEX_MIN	 ,					// 0C = ... 	  "min"   ...
	PF_INDEX_PLUS	 ,					// 0D = ... 	  "plus"  ...
	PF_INDEX_MINUS	 ,					// 0E = ... 	  "minus" ...
	PF_INDEX_DIVIDE  ,					// 0F = ... 	  "divide" ...
	PF_INDEX_NEXT						// No available entries (4 bits)
										// If another entry is made, be sure
										//	 to increase Index:4 to Index:5
										//	 in tagPRIMFLAGS
} FBFN_INDS;

EXTERN
APLFLOAT PrimIdent[PF_INDEX_NEXT];		// Primitive scalar function identity elements
										//	 in the same order as FBFN_INDS

typedef void (FASTBOOLFCN) (APLSTYPE	 aplTypeRht,		// Right arg storage type
							APLNELM 	 aplNELMRht,		// Right arg NELM
							LPVOID		 lpMemRht,			// Ptr to right arg global memory
							LPVOID		 lpMemRes,			// Ptr to result	...
							APLUINT 	 uDimLo,			// Product of dimensions below axis
							APLUINT 	 uDimAxRht, 		// Length of right arg axis dimension
							FBFN_INDS	 uIndex,			// FBFN_INDS value (e.g., index into FastBoolFns[])
							LPPL_YYSTYPE lpYYFcnStrOpr);	// Ptr to operator function strand
typedef FASTBOOLFCN *LPFASTBOOLFCN;

typedef struct tagFASTBOOLFNS
{
	LPFASTBOOLFCN lpReduction;			// 00:	Ptr to Fast Boolean reduction routine
	LPFASTBOOLFCN lpScan;				// 04:	... 				scan	  ...
	UINT		  NotMarker:1,			// 08:	00000001:  Complement of Marker
				  IdentElem:1,			//		00000002:  Identity element (if it exists)
				  Suffix   :1,			//		00000004:  Suffix equivalence value
				  Avail    :29; 		//		FFFFFFF8:  Available bits
										// 0C:	Length
} FASTBOOLFNS, *LPFASTBOOLFNS;

// This array translates a byte index into
//	 [byte][0] = the index of the first 0 in the byte (from right to left)
//	 [byte][1] = the index of the first 1 in the byte (from right to left)
//	 [byte][2] = the sum of the bits in the byte
//	 [byte][3] = its {notequal} scan
//	 [byte][4] = its {equal} scan
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
//	Tab Control vars
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
int gLstTabID							// ID of the previous (outgoing) tab (-1 = none)
#ifdef DEFINE_VALUES
 = -1
#endif
,
	gCurTabID							// ID of the current (incoming) tab  (-1 = none)
#ifdef DEFINE_VALUES
 = -1
#endif
,
	gOverTabIndex						// Index of the tab the mouse is over
										// As this is a transient value, we store it as
										//	 an index rather than an ID
#ifdef DEFINE_VALUES
 = -1
#endif
;

EXTERN
HWND hWndTC,							// Global Tab Control window handle
	 hWndMF,							// ...	  Master Frame ...
	 hWndCC,							// ...	  Crash Control ...
	 hWndCC_LB, 						// ...	  Crash Control Listbox ...
	 hWndTT;							// ...	  ToolTip	   ...

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
APLFLOAT PosInfinity,					// Positive infinity
		 NegInfinity,					// Negative ...
		 Float2Pow53;					// 2*53 in floating point

typedef enum tagSYS_VARS
{
	SYSVAR_UNK = 0, 			// 00:	Unknown name
	SYSVAR_ALX ,				// 01:	[]ALX
	SYSVAR_CT  ,				// 02:	[]CT
	SYSVAR_ELX ,				// 03:	[]ELX
	SYSVAR_FC  ,				// 04:	[]FC
	SYSVAR_IC  ,				// 05:	[]IC
	SYSVAR_IO  ,				// 06:	[]IO
	SYSVAR_LX  ,				// 07:	[]LX
	SYSVAR_PP  ,				// 08:	[]PP
	SYSVAR_PR  ,				// 09:	[]PR
	SYSVAR_PW  ,				// 0A:	[]PW
	SYSVAR_RL  ,				// 0B:	[]RL
	SYSVAR_SA  ,				// 0C:	[]SA
	SYSVAR_WSID,				// 0D:	[]WSID
	SYSVAR_Z   ,				// 0E:	[]Z
	SYSVAR_LENGTH				// 0F:	# entries in the enum
								// 10-1F:  Available entries (5 bits)
} SYS_VARS;

typedef UBOOL (*ASYSVARVALIDSET) (LPTOKEN, LPTOKEN);

EXTERN
// Use as in:  (*aSysVarValidSet[SYSVAR_IO]) (lptkNamArg, lptkRhtArg);
ASYSVARVALIDSET aSysVarValidSet[SYSVAR_LENGTH];

typedef UBOOL (*ASYSVARVALIDNDX) (APLINT, APLSTYPE, LPAPLLONGEST, LPIMM_TYPES);

EXTERN
// Use as in:  (*aSysVarValidNdx[SYSVAR_IO]) (aplIntegerLst, lpaplIntegerRht, &immTypeRht);
ASYSVARVALIDNDX aSysVarValidNdx[SYSVAR_LENGTH];

EXTERN
int MFSizeState 						// Size state for MF (SIZE_xxx)
#ifdef DEFINE_VALUES
 = SIZE_RESTORED
#endif
;

EXTERN
POINT MFPosCtr; 						// X- and Y- center of Master Frame Window position

EXTERN
SIZE  MFSize;							// Size of Master Frame Window window rectangle

EXTERN
HBITMAP hBitMapLineCont;				// Bitmap for the line continuation char

EXTERN
int 	iLCWidth;						// Width of the line continuation column

EXTERN
BITMAP	bmLineCont; 					// Bitmap metrics for the line continuation char

EXTERN
HCURSOR hCursorWait,					// Hourglass cursor
		hCursorIdle;					// Arrow	 ...

// FONTS
EXTERN
LOGFONTW lfSM							// LOGFONTW for the SM
#ifdef DEFINE_VALUES
 = {DEF_SMLOGFONT}
#endif
,
		 lfPR							// LOGFONTW for the Printer
#ifdef DEFINE_VALUES
 = {DEF_PRLOGFONT}
#endif
,
		 lfCC							// LOGFONTW for the CC
#ifdef DEFINE_VALUES
 = {DEF_CCLOGFONT}
#endif
,
		 lfTC							// LOGFONTW for the TC
#ifdef DEFINE_VALUES
 = {DEF_TCLOGFONT}
#endif
,
		 lfFE							// LOGFONTW for the FE
#ifdef DEFINE_VALUES
 = {DEF_FELOGFONT}
#endif
,
		 lfME							// LOGFONTW for the ME
#ifdef DEFINE_VALUES
 = {DEF_MELOGFONT}
#endif
,
		 lfVE							// LOGFONTW for the VE
#ifdef DEFINE_VALUES
 = {DEF_VELOGFONT}
#endif
;

EXTERN
HFONT hFontTC,							// Handle to font for the TC
#ifndef UNISCRIBE
	  hFontAlt, 						// ...					  Alternate SM
#endif
	  hFontSM,							// ...					  SM
	  hFontPR,							// ...					  Printer
	  hFontCC,							// ...					  CC
	  hFontFE,							// ...					  FE
	  hFontME,							// ...					  ME
	  hFontVE;							// ...					  VE

EXTERN
CHOOSEFONTW cfTC,						// Global for ChooseFont for the TC
			cfSM,						// ...							 SM
			cfPR,						// ...							 Printer
			cfCC,						// ...							 CC
			cfFE,						// ...							 FE
			cfME,						// ...							 ME
			cfVE;						// ...							 VE

EXTERN
TEXTMETRIC tmTC,						// Global for TEXTMETRIC for the TC
		   tmSM,						// ...							 SM
		   tmPR,						// ...							 Printer
		   tmCC,						// ...							 CC
		   tmFE,						// ...							 FE
		   tmME,						// ...							 ME
		   tmVE;						// ...							 VE

EXTERN
long cxAveCharTC, cyAveCharTC,			// Size of an average character in the TC font
	 cxAveCharSM, cyAveCharSM,			// ...								   SM ...
	 cxAveCharPR, cyAveCharPR,			// ...								   PR ...
	 cxAveCharFE, cyAveCharFE,			// ...								   FE ...
	 cxAveCharME, cyAveCharME,			// ...								   ME ...
	 cxAveCharVE, cyAveCharVE;			// ...								   VE ...

EXTERN
WNDPROC lpfnOldTabCtrlWndProc;			// Save area for old Tab Control procedure

EXTERN
char pszNoInsertTCTab[]
#ifdef DEFINE_VALUES
 = "Unable to create a new Tab"
#endif
,
	 pszNoEditPrimFns[]
#ifdef DEFINE_VALUES
 = "NONCE ERROR:  Unable to edit named primitive function/operators"
#endif
,
	 pszNoEditVars[]
#ifdef DEFINE_VALUES
 = "NONCE ERROR:  Unable to edit named variables"
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
#ifdef PERFMONON
  ,
	   pszNoCreatePMWnd[]
  #ifdef DEFINE_VALUES
   = "Unable to create Performance Monitoring window"
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
WCHAR wszMCTitle[]						// MDI Client ... (for debugging purposes only)
#ifdef DEFINE_VALUES
 = WS_APPNAME L" MDI Client Window" WS_APPEND_DEBUG
#endif
,
	  wszSMTitle[]						// Session Manager ...
#ifdef DEFINE_VALUES
 = WS_APPNAME L" Session Manager" WS_APPEND_DEBUG
#endif
#ifdef DEBUG
  ,
	  wszDBTitle[]						// Debugger ...
  #ifdef DEFINE_VALUES
   = WS_APPNAME L" Debugger Window" WS_APPEND_DEBUG
  #endif
#endif
#ifdef PERFMONON
  ,
	  wszPMTitle[]						// Performance Monitoring ...
  #ifdef DEFINE_VALUES
   = WS_APPNAME L" Performance Monitoring Window" WS_APPEND_DEBUG
  #endif
#endif
,
	  wszFETitle[]						// Function Editor ...
#ifdef DEFINE_VALUES
 = WS_APPNAME L" Function Editor" WS_APPEND_DEBUG
#endif
,
	  wszMETitle[]						// Matrix Editor ...
#ifdef DEFINE_VALUES
 = WS_APPNAME L" Matrix Editor" WS_APPEND_DEBUG
#endif
,
	  wszVETitle[]						// Vector Editor ...
#ifdef DEFINE_VALUES
 = WS_APPNAME L" Vector Editor" WS_APPEND_DEBUG
#endif
;

typedef enum tagMEMVIRTENUM
{
	MEMVIRT_WSZGLBTEMP = 0, 			// 00:	lpwszGlbTemp
	MEMVIRT_GLBHSHTAB,					// 01:	Global HshTab for {symbol} names & values
	MEMVIRT_LENGTH						// 02:	# entries
} MEMVIRTENUM;

#define MVS 	struct tagMEMVIRTSTR

typedef struct tagMEMVIRTSTR
{
	MVS 	*lpPrvMVS,					// 00:	Ptr to previous link (NULL = none)
			*lpNxtMVS;					// 04:	Ptr to next 	...
	LPUCHAR IniAddr;					// 08:	Initial address
	UINT	IncrSize,					// 0C:	Incremental size in bytes
			MaxSize;					// 10:	Maximum 	...
#ifdef DEBUG
	LPCHAR	lpText; 					// 14:	Ptr to (const) description of this item
										// 18:	Length
#else
										// 14:	Length
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

typedef struct tagHSHTABSTR
{
	LPHSHENTRY lpHshTab,				// 00:	Ptr to start of HshTab
			   lpHshTabSplitNext;		// 04:	...    next HTE to split (incremented by DEF_HSHTAB_NBLKS)
	int 	   iHshTabBaseSize, 		// 08:	Base size of hash table
			   iHshTabTotalSize,		// 0C:	# HTEs, currently, including EPBs
			   iHshTabIncr, 			// 10:	Increment when looping through HshTab
			   iHshTabIncrSize, 		// 14:	Incremental size
			   iHshTabEPB;				// 18:	# entries per block
	UINT	   uHashMask;				// 1C:	Mask for all HshTab lookups
} HSHTABSTR, *LPHSHTABSTR;				// 20:	Length

EXTERN
HSHTABSTR htsGLB;						// Global HshTab struc

EXTERN
HIMAGELIST hImageList;					// Handle to the common image list

// Same order as in ARRAY_TYPES
// so that BOOL < INT < FLOAT < APA < CHAR < HETERO < NESTED
EXTERN
UINT uTypeMap[]
#ifdef DEFINE_VALUES
//	BOOL, INT, FLOAT, CHAR, HETERO, NESTED, LIST, APA
 = {   0,	1,	   2,	 4, 	 5, 	 6,    7,	3}
#endif
;

APLSTYPE aTypePromote[ARRAY_LENGTH + 1][ARRAY_LENGTH + 1]
#ifdef DEFINE_VALUES
=
//		BOOL		  INT			FLOAT		  CHAR		  HETERO		NESTED		  LIST		   APA				INIT
{{ARRAY_BOOL  , ARRAY_INT	, ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_ERROR , ARRAY_INT	, ARRAY_BOOL  },  // BOOL
 {ARRAY_INT   , ARRAY_INT	, ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_ERROR , ARRAY_INT	, ARRAY_INT   },  // INT
 {ARRAY_FLOAT , ARRAY_FLOAT , ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_ERROR , ARRAY_FLOAT , ARRAY_FLOAT },  // FLOAT
 {ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_CHAR	, ARRAY_HETERO, ARRAY_NESTED, ARRAY_ERROR , ARRAY_HETERO, ARRAY_CHAR  },  // CHAR
 {ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_ERROR , ARRAY_HETERO, ARRAY_HETERO},  // HETERO
 {ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_ERROR , ARRAY_NESTED, ARRAY_NESTED},  // NESTED
 {ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR },  // LIST
 {ARRAY_INT   , ARRAY_INT	, ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_ERROR , ARRAY_INT	, ARRAY_INT   },  // APA
 {ARRAY_BOOL  , ARRAY_INT	, ARRAY_FLOAT , ARRAY_CHAR	, ARRAY_HETERO, ARRAY_NESTED, ARRAY_ERROR , ARRAY_INT	, ARRAY_ERROR },  // INIT
}
#endif
;

typedef struct
{
	char  nrm;		// 00:	Normal			 (shifted & unshifted) (unused)
	WCHAR alt;		// 01:	Alt key pressed  (shifted & unshifted)
} CHARCODE;

// If you are looking for places on the keyboard to put a new symbol,
//	 there are several free Alt-Shift- combinations:
//	   Alt-'A'
//	   Alt-'B'
//	   Alt-'C'
//	   Alt-'D'
//	   Alt-'F'
//	   Alt-'K'
//	   Alt-'S'
//	   Alt-'U'
//	   Alt-'Q'
//	   Alt-'R'
//	   Alt-'V'
//	   Alt-'W'
//	   Alt-'X'
//	   Alt-'Y'
//	   Alt-'Z'
//	   Alt-'<'
//	   Alt-'>'
//	   Alt-'?'
//
//	 as well as several duplicated symbols:
//
//	   Symbol & Name			  Keystroke
//	   --------------------------------------------------
//	   * Asterisk				  Alt-'p'
//	   = Equal					  Alt-'5'
//	   < Left Caret 			  Alt-'3'
//	   ? Question Mark			  Alt-'q'
//	   > Right Caret			  Alt-'7'
//	   ! Shreik 				  Alt-'_'
//	   ' Single Quote             Alt-'k'
//	   | Stile					  Alt-'m'
//	   ~ Tilde					  Alt-'t'
//	   ^ Up Caret (Circumflex)	  Alt-'0'

EXTERN
CHARCODE aCharCodes[1+126-32]	// This ordering follows the ASCII charset
								//	 from 32 to 126 inclusive
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
#define ACHARCODES_NROWS	(sizeof (aCharCodes) / sizeof (aCharCodes[0]))

// The # rows in the aSymbolNames table
EXTERN
UINT aSymbolNames_NRows
#ifdef DEFINE_VALUES
= (sizeof (aSymbolNames) / sizeof (aSymbolNames[0]))
#endif
;

// Translate tables between APL2 and NARS charsets
EXTERN
WCHAR APL2_ASCIItoNARS[257]
#ifdef DEFINE_VALUES
=
{
//	   x0		  x1		 x2 		x3		   x4		  x5		 x6 		x7		   x8		  x9		 xA 		xB		   xC		  xD		 xE 		xF
	L'\x0000', L'\x0001', L'\x0002', L'\x0003', L'\x0004', L'\x0005', L'\x0006', L'\x0007', L'\x0008', L'\x0009', L'\x000A', L'\x000B', L'\x000C', L'\x000D', L'\x000E', L'\x000F', // 0x
	L'\x0010', L'\x0011', L'\x0012', L'\x0013', L'\x0014', L'\x0015', L'\x0016', L'\x0017', L'\x0018', L'\x0019', L'\x001A', L'\x001B', L'\x001C', L'\x001D', L'\x001E', L'\x001F', // 1x
	L' '     , L'!'     , L'"'     , L'#'     , L'$'     , L'%'     , L'&'     , L'\''    , L'('     , L')'     , L'*'     , L'+'     , L','     , L'-'     , L'.'     , L'/'     , // 2x
	L'0'     , L'1'     , L'2'     , L'3'     , L'4'     , L'5'     , L'6'     , L'7'     , L'8'     , L'9'     , L':'     , L';'     , L'<'     , L'='     , L'>'     , L'?'     , // 3x
	L'@'     , L'A'     , L'B'     , L'C'     , L'D'     , L'E'     , L'F'     , L'G'     , L'H'     , L'I'     , L'J'     , L'K'     , L'L'     , L'M'     , L'N'     , L'O'     , // 4x
	L'P'     , L'Q'     , L'R'     , L'S'     , L'T'     , L'U'     , L'V'     , L'W'     , L'X'     , L'Y'     , L'Z'     , L'['     , L'\\'    , L']'     , L'^'     , L'_'     , // 5x
	L'`'     , L'a'     , L'b'     , L'c'     , L'd'     , L'e'     , L'f'     , L'g'     , L'h'     , L'i'     , L'j'     , L'k'     , L'l'     , L'm'     , L'n'     , L'o'     , // 6x
	L'p'     , L'q'     , L'r'     , L's'     , L't'     , L'u'     , L'v'     , L'w'     , L'x'     , L'y'     , L'z'     , L'{'     , L'|'     , L'}'     , L'~'     , L'\x007F', // 7x
	L'\x00C7', L'\x00FC', L'\x00E9', L'\x00E2', L'\x00E4', L'\x00E0', L'\x00E5', L'\x00E7', L'\x00EA', L'\x00EB', L'\x00E8', L'\x00EF', L'\x00EE', L'\x00EC', L'\x00C4', L'\x00C5', // 8x
	L'\x2395', L'\x235E', L'\x2339', L'\x00F4', L'\x00F6', L'\x00F2', L'\x00FB', L'\x00F9', L'\x22A4', L'\x00D6', L'\x00DC', L'\x00F8', L'\x00A3', L'\x22A5', L'\x20A7', L'\x2336', // 9x
	L'\x00E1', L'\x00ED', L'\x00F3', L'\x00FA', L'\x00F1', L'\x00D1', L'\x00AA', L'\x00BA', L'\x00BF', L'\x2308', L'\x00AC', L'\x00BD', L'\x222A', L'\x00A1', L'\x2355', L'\x234E', // Ax
	L'\x2591', L'\x2592', L'\x2593', L'\x2502', L'\x2524', L'\x235F', L'\x2206', L'\x2207', L'\x2192', L'\x2563', L'\x2551', L'\x2557', L'\x255D', L'\x2190', L'\x230A', L'\x2510', // Bx
	L'\x2514', L'\x2534', L'\x252C', L'\x251C', L'\x2500', L'\x253C', L'\x2191', L'\x2193', L'\x255A', L'\x2554', L'\x2569', L'\x2566', L'\x2560', L'\x2550', L'\x256C', L'\x2261', // Cx
	L'\x2378', L'\x2377', L'\x2235', L'\x2337', L'\x2342', L'\x233B', L'\x22A2', L'\x22A3', L'\x22C4', L'\x2518', L'\x250C', L'\x2588', L'\x2584', L'\x00A6', L'\x00CC', L'\x2580', // Dx
	L'\x237A', L'\x2379', L'\x2282', L'\x2283', L'\x235D', L'\x2372', L'\x2374', L'\x2371', L'\x233D', L'\x2296', L'\x25CB', L'\x2228', L'\x2373', L'\x2349', L'\x220A', L'\x2229', // Ex
	L'\x233F', L'\x2340', L'\x2265', L'\x2264', L'\x2260', L'\x00D7', L'\x00F7', L'\x2359', L'\x2218', L'\x2375', L'\x236B', L'\x234B', L'\x2352', L'\x00AF', L'\x00A8', L'\x00A0', // Fx
}
#endif
,

	  APL2_EBCDICtoNARS[257]
#ifdef DEFINE_VALUES
=
{
//	   x0		  x1		 x2 		x3		   x4		  x5		 x6 		x7		   x8		  x9		 xA 		xB		   xC		  xD		 xE 		xF
	L'\x0000', L'\x0001', L'\x0002', L'\x0003', L'\x0004', L'\x0005', L'\x0006', L'\x0007', L'\x0008', L'\x0009', L'\x000A', L'\x000B', L'\x000C', L'\x000D', L'\x000E', L'\x000F', // 0x
	L'\x0010', L'\x0011', L'\x0012', L'\x0013', L'\x0014', L'\x0015', L'\x0016', L'\x0017', L'\x0018', L'\x0019', L'\x001A', L'\x001B', L'\x001C', L'\x001D', L'\x001E', L'\x001F', // 1x
	L' '     , L'!'     , L'"'     , L'#'     , L'$'     , L'%'     , L'&'     , L'\''    , L'('     , L')'     , L'*'     , L'+'     , L','     , L'-'     , L'.'     , L'/'     , // 2x
	L'0'     , L'1'     , L'2'     , L'3'     , L'4'     , L'5'     , L'6'     , L'7'     , L'8'     , L'9'     , L':'     , L';'     , L'<'     , L'='     , L'>'     , L'?'     , // 3x
	L'@'     , L'A'     , L'B'     , L'C'     , L'D'     , L'E'     , L'F'     , L'G'     , L'H'     , L'I'     , L'J'     , L'K'     , L'L'     , L'M'     , L'N'     , L'O'     , // 4x
	L'P'     , L'Q'     , L'R'     , L'S'     , L'T'     , L'U'     , L'V'     , L'W'     , L'X'     , L'Y'     , L'Z'     , L'['     , L'\\'    , L']'     , L'^'     , L'_'     , // 5x
	L'`'     , L'a'     , L'b'     , L'c'     , L'd'     , L'e'     , L'f'     , L'g'     , L'h'     , L'i'     , L'j'     , L'k'     , L'l'     , L'm'     , L'n'     , L'o'     , // 6x
	L'p'     , L'q'     , L'r'     , L's'     , L't'     , L'u'     , L'v'     , L'w'     , L'x'     , L'y'     , L'z'     , L'{'     , L'|'     , L'}'     , L'~'     , L'\xE036', // 7x
	L'\xE037', L'\xE038', L'\xE039', L'\xE03A', L'\xE03B', L'\xE03C', L'\xE03D', L'\xE03E', L'\xE03F', L'\xE040', L'\xE041', L'\xE042', L'\xE043', L'\xE044', L'\xE045', L'\xE046', // 8x
	L'\x2395', L'\x235E', L'\x2339', L'\xE047', L'\xE048', L'\xE049', L'\xE04A', L'\xE04B', L'\x22A4', L'\xE04C', L'\xE04D', L'\x00F8', L'\xE04E', L'\x22A5', L'\xE04F', L'\x2336', // 9x
	L'\x00E1', L'\x00ED', L'\x00F3', L'\x00FA', L'\x00F1', L'\x00D1', L'\x00AA', L'\x00BA', L'\x00BF', L'\x2308', L'\x00AC', L'\x00BD', L'\x222A', L'\x00A1', L'\x2355', L'\x234E', // Ax
	L'\x2591', L'\x2592', L'\x2593', L'\x2502', L'\x2524', L'\x235F', L'\x2206', L'\x2207', L'\x2192', L'\x2563', L'\x2551', L'\x2557', L'\x255D', L'\x2190', L'\x230A', L'\x2510', // Bx
	L'\x2514', L'\x2534', L'\x252C', L'\x251C', L'\x2500', L'\x253C', L'\x2191', L'\x2193', L'\x255A', L'\x2554', L'\x2569', L'\x2566', L'\x2560', L'\x2550', L'\x256C', L'\x2261', // Cx
	L'\x2378', L'\x2377', L'\x2235', L'\x2337', L'\x2342', L'\x233B', L'\x22A2', L'\x22A3', L'\x22C4', L'\x2518', L'\x250C', L'\x2588', L'\x2584', L'\x00A6', L'\x00CC', L'\x2580', // Dx
	L'\x237A', L'\x2379', L'\x2282', L'\x2283', L'\x235D', L'\x2372', L'\x2374', L'\x2371', L'\x233D', L'\x2296', L'\x25CB', L'\x2228', L'\x2373', L'\x2349', L'\x220A', L'\x2229', // Ex
	L'\x233F', L'\x2340', L'\x2265', L'\x2264', L'\x2260', L'\x00D7', L'\x00F7', L'\x2359', L'\x2218', L'\x2375', L'\x236B', L'\x234B', L'\x2352', L'\x00AF', L'\x00A8', L'\x00A0', // Fx
}
#endif
;


// Define global option flags
typedef struct tagOPTIONFLAGS
{
	UINT bAdjustPW			 :1,	// 00000001:  TRUE iff WM_SIZE changes []PW
		 bUnderbarToLowercase:1,	// 00000002:  ...	   Paste of underbar letters translates to lowercase
		 bNewTabOnClear 	 :1,	// 00000004:  ...	   )CLEAR creates a new tab
		 bNewTabOnLoad		 :1,	// 00000008:  ...	   )LOAD  ...
		 bUseLocalTime		 :1,	// 00000010:  ...	   LocalTime is used instead of SystemTime (GMT)
		 bBackupOnLoad		 :1,	// 00000020:  ...	   make a backup copy on all )LOADs
		 bBackupOnSave		 :1,	// 00000040:  ...	   make a backup copy on all )SAVEs
		 bNoCopyrightMsg	 :1,	// 00000080:  ...	   suppress the copright msg at startup
		 uDefaultPaste		 :4,	// 00000F00:  Index of default Paste translation (see UNI_TRANS)
		 uDefaultCopy		 :4,	// 0000F000:  Index of default Paste translation (see UNI_TRANS)
		 Avail				 :16;	// FFFF0000:  Available bits
} OPTIONFLAGS, *LPOPTIONFLAGS;

// N.B.:  Whenever changing the above struct (OPTIONFLAGS),
//	 be sure to make a corresponding change to
//	 <OptionFlags> in <externs.h>.

EXTERN
OPTIONFLAGS OptionFlags
#ifdef DEFINE_VALUES
= {DEF_ADJUSTPW,
   DEF_UNDERBARTOLOWERCASE,
   DEF_NEWTABONCLEAR,
   DEF_NEWTABONLOAD,
   DEF_USELOCALTIME,
   DEF_BACKUPONLOAD,
   DEF_BACKUPONSAVE,
   DEF_NOCOPYRIGHTMSG,
   DEF_DEFAULTPASTE,
   DEF_DEFAULTCOPY}
#endif
;


typedef enum tagFONTENUM
{
	FONTENUM_SM = 0,					// 00:	Session Manager font
	FONTENUM_FE,						// 01:	Function Editor
	FONTENUM_PR,						// 02:	Printer
	FONTENUM_CC,						// 03:	Crash Control window
	FONTENUM_TC,						// 04:	Tab Control
	FONTENUM_VE,						// 05:	Vector Editor
	FONTENUM_ME,						// 06:	Matrix Editor
	FONTENUM_LENGTH,					// 07:	Length
} FONTENUM, *LPFONTENUM;

EXTERN
FONTENUM glbSameFontAs[FONTENUM_LENGTH];

void CreateNewFontSM (void);
void CreateNewFontFE (void);
void CreateNewFontPR (void);
void CreateNewFontCC (void);
void CreateNewFontTC (void);
void CreateNewFontME (void);
void CreateNewFontVE (void);

typedef struct tagFONTSTRUC
{
	LPLOGFONTW	  lplf; 					// Ptr to LOGFONTW	  struct for this font
	LPCHOOSEFONTW lpcf; 					// Ptr to CHOOSEFONTW ...
	LPTEXTMETRIC  lptm; 					// Ptr to TEXTMETRIC  ...
	int 		  iPtSize;					// Default point size
	UBOOL		  bPrinter, 				// TRUE iff this font is for the printer
				  bChanged; 				// TRUE iff ChooseFont changed the font (the user exited via OK)
	void		(*lpCreateNewFont) (void);	// Ptr to CreateNewFontXX for this font
	LPWCHAR 	  lpwTitle; 				// Ptr to window title
	CHOOSEFONTW   cfLcl;					// Local CHOOSEFONTW while Customize Dialog is running
} FONTSTRUC, *LPFONTSTRUC;

EXTERN
FONTSTRUC fontStruc[FONTENUM_LENGTH]
#ifdef DEFINE_VALUES
= {{&lfSM, &cfSM, &tmSM, DEF_SMPTSIZE, FALSE, FALSE, &CreateNewFontSM, L"Session Manager Font"   },  // Session Manager
   {&lfFE, &cfFE, &tmFE, DEF_FEPTSIZE, FALSE, FALSE, &CreateNewFontFE, L"Function Editor Font"   },  // Function Editor
   {&lfPR, &cfPR, &tmPR, DEF_PRPTSIZE, TRUE , FALSE, &CreateNewFontPR, L"Printer Font"           },  // Printer
   {&lfCC, &cfCC, &tmCC, DEF_CCPTSIZE, FALSE, FALSE, &CreateNewFontCC, L"Crash Window Font"      },  // Crash window
   {&lfTC, &cfTC, &tmTC, DEF_TCPTSIZE, FALSE, FALSE, &CreateNewFontTC, L"Tab Control Font"       },  // Tab Control
   {&lfVE, &cfVE, &tmVE, DEF_VEPTSIZE, FALSE, FALSE, &CreateNewFontME, L"Vector Editor Font"     },  // Vector Editor
   {&lfME, &cfME, &tmME, DEF_MEPTSIZE, FALSE, FALSE, &CreateNewFontVE, L"Matrix Editor Font"     },  // Matrix Editor
  }
#endif
;


typedef struct tagCUSTOMIZE
{
	LPWCHAR lpwTitle;
	UINT	uIDD;
	UBOOL	bInitialized;
} CUSTOMIZE, *LPCUSTOMIZE;

EXTERN
CUSTOMIZE custStruc[]
#ifdef DEFINE_VALUES
 =
{	{L"CLEAR WS Values"         , IDD_PROPPAGE_CLEARWS_VALUES   ,  FALSE},  // 00
////{L"Directories"             , IDD_PROPPAGE_DIRS             ,  FALSE},  // 01
	{L"Fonts"                   , IDD_PROPPAGE_FONTS            ,  FALSE},  // 02
	{L"Range Limited Vars"      , IDD_PROPPAGE_RANGE_LIMITS     ,  FALSE},  // 03
////{L"Syntax Coloring"         , IDD_PROPPAGE_SYNTAX_COLORING  ,  FALSE},  // 04
	{L"System Variable Reset"   , IDD_PROPPAGE_SYSTEM_VAR_RESET ,  FALSE},  // 05
////{L"Tab Colors"              , IDD_PROPPAGE_TAB_COLORS       ,  FALSE},  // 06
	{L"User Preferences"        , IDD_PROPPAGE_USER_PREFS       ,  FALSE},  // 07
}
#endif
;

EXTERN
UINT custStrucLen
#ifdef DEFINE_VALUES
= itemsizeof (custStruc)
#endif
;

#define DEF_INIT_CATEGORY	(IDD_PROPPAGE_FONTS - IDD_PROPPAGE_START)	// Fonts

EXTERN
int gInitCustomizeCategory
#ifdef DEFINE_VALUES
= DEF_INIT_CATEGORY
#endif
;

typedef enum tagUNDO_ACTS
{
	undoNone = 0,		// 0000:  No action
	undoIns,			// 0001:  Insert a character
	undoRep,			// 0002:  Replace a character
	undoDel,			// 0003:  Delete one or more characters
	undoSel,			// 0004:  Select one or more characters
	undoInsToggle,		// 0005:  Toggle the insert mode
						// 0006-FFFF:  Available entries (16 bits)
} UNDO_ACTS;

// Define the corresponding one-letter actions
#define UndoActToChar	L"NIRDST"

#define UNDO_NOGROUP	0

typedef struct tagUNDO_BUF
{
	UINT  CharPosBeg,	// 00:	Beginning character position (from start of text),
						//		-1 = current position
		  CharPosEnd,	// 04:	Ending	  ...
		  Group;		// 08:	Group index identifies actions to be performed together,
						//		0 = no grouping
	short Action;		// 0C:	Action (see UNDO_ACTS)
	WCHAR Char; 		// 0E:	The character (if any),
						//		 0 = none
						// 10:	Length
} UNDO_BUF, *LPUNDO_BUF;


typedef union tagMEMTXT_UNION
{
	struct
	{
		UINT U; 			// 00:	The line length
		union
		{
			APLCHAR C;		// 04:	Followed by an APLCHAR
			WORD	W;		// 04:	... 		 a WORD
		};
	};
} MEMTXT_UNION, *LPMEMTXT_UNION;

typedef void (*LPERRHANDFN) (LPWCHAR lpwszMsg,
							 LPWCHAR lpwszLine,
							 UINT uCaret,
							 HWND hWndEC);
typedef enum tagERROR_CODES
{
	ERRORCODE_NONE = 0, 	// 00:	No error
	ERRORCODE_ALX,			// 01:	Signal []ALX
	ERRORCODE_ELX,			// 02:	Signal []ELX
} ERROR_CODES;

typedef enum tagLINE_NUMS	// Starting line #s
{
	LINENUM_ONE = 0,		// 00:	Line #1
	LINENUM_IDENTITY,		// 01:	Line []IDENTITY
	LINENUM_INVERSE,		// 02:	Line []INVERSE
	LINENUM_PROTOTYPE,		// 03:	Line []PROTOTYPE
	LINENUM_SINGLETON,		// 04:	Line []SINGLETON
} LINE_NUMS;

#define ENUMS_DEFINED
#undef	EXTERN


//***************************************************************************
//	End of File: externs.h
//***************************************************************************
