//****************************************************************************
//	NARS2000 -- Main Header File
//****************************************************************************

#define EQ ==
#define NE !=

#define LOSHORT(l)	((short)((DWORD)(l) & 0xffff))
#define HISHORT(l)	((short)((DWORD)(l) >> 16))

#define TRUE	1
#define FALSE	0
#define NEG1U	((UINT) -1)

#define abs64(a)	(((a)>0)?a:-(a))
#define MB(a)	MessageBox (NULL, a, "NARS2000", MB_OK)
#define MBC(a)	if (MessageBox (NULL, a, "NARS2000", MB_OKCANCEL) EQ IDCANCEL) DbgBrk ()
#define IsGlbPtr(a) (GlobalFlags (a) NE GMEM_INVALID_HANDLE)
#define IsGlbTypeVarDir(a) (IsGlobalTypeArray ( 		   a, VARARRAY_HEADER_SIGNATURE))
#define IsGlbTypeVarInd(a) (IsGlobalTypeArray (*(LPVOID *) a, VARARRAY_HEADER_SIGNATURE))
#define IsGlbTypeFcnDir(a) (IsGlobalTypeArray ( 		   a, FCNARRAY_HEADER_SIGNATURE))
#define IsGlbTypeFcnInd(a) (IsGlobalTypeArray (*(LPVOID *) a, FCNARRAY_HEADER_SIGNATURE))
#define IsGlbTypeDfnDir(a) (IsGlobalTypeArray ( 		   a, DFN_HEADER_SIGNATURE))
#define IsGlbTypeNamDir(a) (IsGlobalTypeArray ( 		   a, VARNAMED_HEADER_SIGNATURE))
#define IsSymNoValue(a) 	((a)->stHshEntry EQ NULL				\
						  && (a)->stFlags.Perm						\
						  && (a)->stFlags.Value EQ 0				\
						  && (a)->stFlags.ObjName EQ OBJNAME_NONE	\
						  && (a)->stFlags.ObjType EQ NAMETYPE_UNK)
#define IsTokenNoValue(a)	((a)									\
						  && (a)->tkFlags.TknType EQ TKT_VARNAMED	\
						  && IsSymNoValue ((a)->tkData.tkSym))

#define ByteAddr(a,b)		(&(((LPBYTE) (a))[b]))

#define AplModI(m,a) PrimFnDydStileIisIvI (m, a, NULL)
#define AplModF(m,a) PrimFnDydStileFisFvF (m, a, NULL)

#define LOPART_DWORDLONG		((DWORDLONG) 0x00000000FFFFFFFF)
#define HIPART_DWORDLONG		((DWORDLONG) 0xFFFFFFFF00000000)

#define LODWORD(x)				( (DWORD) (   (x) & LOPART_DWORDLONG ) )
#define HIDWORD(x)				( (DWORD) ( ( (x) & HIPART_DWORDLONG ) >> 32 ) )

#define QWORD	ULONGLONG

#define defstop \
default:	\
	DbgStop();		// We should never get here

#define FNLN	FileNameOnly (__FILE__), __LINE__

#ifdef DEBUG

#define LCLODSAPI	ODSAPI

#define DBGENTER	if (gDbgLvl > 2) {DbgMsgW (L"Entering" APPEND_NAME);}
#define DBGLEAVE	if (gDbgLvl > 2) {DbgMsgW (L"Leaving " APPEND_NAME);}

#ifdef DEBUG_ALLOCFREE
#define DbgGlobalAlloc(uFlags,ByteRes) \
DbgGlobalAllocSub (uFlags, ByteRes, L"##GlobalAlloc in " APPEND_NAME L": %08X (%S#%d)", FNLN)

#define DbgGlobalFree(hGlbToken) \
dprintfW (L"**GlobalFree  in " APPEND_NAME L": %08X (%S#%d)", hGlbToken, FNLN); \
MyGlobalFree (hGlbToken);
#else
#define DbgGlobalAlloc(uFlags,ByteRes) \
MyGlobalAlloc (uFlags, ByteRes)

#define DbgGlobalFree(hGlbToken) \
MyGlobalFree (hGlbToken);
#endif

#ifdef DEBUG_REFCNT
#define DbgIncrRefCntDir(hGlbData) \
dprintfW (L"##RefCnt++ in " APPEND_NAME L": %08X (%S#%d)", ClrPtrTypeDir (hGlbData), FNLN); \
IncrRefCntDir (hGlbData);

#define DbgIncrRefCntInd(hGlbData) \
dprintfW (L"##RefCnt++ in " APPEND_NAME L": %08X (%S#%d)", ClrPtrTypeDir (hGlbData), FNLN); \
IncrRefCntInd (hGlbData);

#define DbgDecrRefCntDir(hGlbData) \
dprintfW (L"##RefCnt-- in " APPEND_NAME L": %08X (%S#%d)", ClrPtrTypeDir (hGlbData), FNLN); \
DecrRefCntDir (hGlbData);

#define DbgDecrRefCntInd(hGlbData) \
dprintfW (L"##RefCnt-- in " APPEND_NAME L": %08X (%S#%d)", ClrPtrTypeDir (hGlbData), FNLN); \
DecrRefCntInd (hGlbData);
#else
#define DbgIncrRefCntDir(hGlbData) \
IncrRefCntDir (hGlbData);

#define DbgIncrRefCntInd(hGlbData) \
IncrRefCntInd (hGlbData);

#define DbgDecrRefCntDir(hGlbData) \
DecrRefCntDir (hGlbData);

#define DbgDecrRefCntInd(hGlbData) \
DecrRefCntInd (hGlbData);
#endif

#define CheckMemStat()		_CheckMemStat ()

#else

#define LCLODSAPI

#define DBGENTER
#define DBGLEAVE

#define DbgGlobalAlloc(uFlags,ByteRes)	MyGlobalAlloc (uFlags, ByteRes);

#define DbgGlobalFree(hGlbToken)		MyGlobalFree (hGlbToken);

#define DbgIncrRefCntDir(hGlbData)		IncrRefCntDir (hGlbData);

#define DbgIncrRefCntInd(hGlbData)		IncrRefCntInd (hGlbData);

#define DbgDecrRefCntDir(hGlbData)		DecrRefCntDir (hGlbData);

#define DbgDecrRefCntInd(hGlbData)		DecrRefCntInd (hGlbData);

#define DbgMsg(a)
#define DbgMsgW(a)

#define CheckMemStat()

#endif

//*************************** Window Data ********************************

// Define offsets in DBWNDCLASS window extra bytes
#define GWLDB_HWNDLB	0									// Window handle of Listbox
#define GWLDB_EXTRA 	GWLDB_HWNDLB   + 1 * sizeof (long)	// Total # extra bytes

// Define common offset between the Session Manager and Function Editor
#define GWLSF_PERTAB	0									// HGLOBAL of corresponding PERTABDATA struc
#define GWLSF_HWNDEC	GWLSF_PERTAB   + 1 * sizeof (long)	// Handle of the matching Edit control
#define GWLSF_UNDO_INI	GWLSF_HWNDEC   + 1 * sizeof (long)	// Ptr to Undo stack, initial (as allocated)
#define GWLSF_UNDO_BEG	GWLSF_UNDO_INI + 1 * sizeof (long)	// ...				  beginning
#define GWLSF_UNDO_NXT	GWLSF_UNDO_BEG + 1 * sizeof (long)	// ...				  next
#define GWLSF_UNDO_LST	GWLSF_UNDO_NXT + 1 * sizeof (long)	// ...				  last
#define GWLSF_UNDO_GRP	GWLSF_UNDO_LST + 1 * sizeof (long)	// Value of next Undo group index
#define GWLSF_VKSTATE	GWLSF_UNDO_GRP + 1 * sizeof (long)	// Virtal Key state (VKSTATE struc)
#define GWLSF_LASTKEY	GWLSF_VKSTATE  + 1 * sizeof (long)	// Value of last WM_KEYDOWN key
#define GWLSF_CHANGED	GWLSF_LASTKEY  + 1 * sizeof (long)	// Boolean of whether or not the text has changed

// Define offsets in SMWNDCLASS window extra bytes
#define GWLSM_EXTRA 	GWLSF_CHANGED  + 1 * sizeof (long)	// Total # extra bytes

// Define offsets in FEWNDCLASS window extra bytes
#define GWLFE_EXTRA 	GWLSF_CHANGED  + 1 * sizeof (long)	// Total # extra bytes

// Define offsets in MEWNDCLASS window extra bytes
#define GWLME_EXTRA 	0									// Total # extra bytes

// Define offsets in VEWNDCLASS window extra bytes
#define GWLVE_EXTRA 	0									// Total # extra bytes


// Define local window messages
#define MYWM_MOVE			(WM_APP + 0)	// MF
#define MYWM_SETFOCUS		(WM_APP + 1)	// SM (SetFocus)
#define MYWM_IZITNAME		(WM_APP + 2)	// FE (Izit A Name)
#define MYWM_SAVE_FN		(WM_APP + 3)	// FE (SaveFunction)
#define MYWM_SAVECLOSE_FN	(WM_APP + 4)	// FE (SaveFunction/CloseFunction)
#define MYWM_SAVE_AS_FN 	(WM_APP + 5)	// FE (SaveAsFunction)
#define MYWM_CLOSE_FN		(WM_APP + 6)	// FE (CloseFunction)
#define MYWM_QUOTEQUAD		(WM_APP + 7)	// PL (Quote-Quad/Quad Input)

// Define Debug window messages
#define MYWM_INIT_DB		(WM_APP + 0)	// DB
#define MYWM_DBGMSGA		(WM_APP + 1)	// DB
#define MYWM_DBGMSGW		(WM_APP + 2)	// DB
#define MYWM_DBGMSG_CLR 	(WM_APP + 3)	// DB
#define MYWM_DBGMSG_SCROLL	(WM_APP + 4)	// DB


typedef enum tagEXCEPTION_CODES // Exception Codes
{
	EXCEPTION_SUCCESS = 0 , 	// 00:	All OK
////EXCEPTION_RESULT_BOOL , 	//		Result should be Boolean
////EXCEPTION_RESULT_INT  , 	//		... 			 Integer
	EXCEPTION_RESULT_FLOAT, 	// 01:	... 			 Float
	EXCEPTION_DOMAIN_ERROR, 	// 02:	Signal a DOMAIN ERROR
	EXCEPTION_CTRL_BREAK,		// 03:	Ctrl-Break pressed
} EXCEPTION_CODES;

typedef struct tagGLBHIST
{
	HGLOBAL *hGlb;				// 00:	Ptr to HGLOBAL
	UINT	First:1,			// 04:	00000001:  This line is the first one in the array
			ContPrev:1, 		//		00000002:  This line is connected to the previous line
			ContNext:1, 		//		00000004:  ...							 next	  ...
			Avail:29;			//		FFFFFFF8:  Available bits
								// 08:	Length
} GLBHIST, *LPGLBHIST;

typedef struct tagVKSTATE
{
	ULONG Shift:1,				// 00000001:  Left- or right-shift key up(0) or down(1)
////	 lShift:1,
////	 rShift:1,
		  Alt:1,				// 00000002:  Left- or right-Alt key up(0) or down(1)
////	 lAlt:1,
////	 rAlt:1,
		  Ctl:1,				// 00000004:  Left or -right Ctl key up(0) or down(1)
////	 lCtl:1,
////	 rCtl:1,
		  Ins:1,				// 00000008:  Replace(0) or insert(1)
		  Avail:28; 			// FFFFFFF0:  Available bits
} VKSTATE, *LPVKSTATE;

typedef enum tagMAKEPROTO
{
	MP_CHARS,					// 00:	Chars allowed in MakePrototype arg
	MP_NUMONLY, 				// 01:	Numerics only ...
	MP_NUMCONV					// 02:	Convert chars to numerics ...
} MAKEPROTO;

typedef enum tagEXIT_TYPES
{
	EXITTYPE_GOTO_ZILDE = 0,	// 00:	{goto} {zilde}
	EXITTYPE_GOTO_LINE, 		// 01:	{goto} LineNum
	EXITTYPE_GOTO_RESET,		// 02:	{goto}
	EXITTYPE_ERROR, 			// 03:	ERROR
	EXITTYPE_NODISPLAY, 		// 04:	Value already displayed
	EXITTYPE_NOVALUE,			// 05:	No value returned
								// 06-07:  Available entries (3 bits)
} EXIT_TYPES;

#ifndef DEBUG
#define Assert(a)
#endif

#define DEF_STRAND_INITSIZE    (  64*1024)	// Initial size in tokens of the strand stack
#define DEF_STRAND_MAXSIZE	   (1024*1024)	// Maximum ...

// Resource debugging
#define MAXOBJ	128000

// Define bit masks
#define BIT0	0x00000001
#define BIT1	0x00000010
#define BIT2	0x00000100
#define BIT3	0x00001000
#define BIT4	0x00010000
#define BIT5	0x00100000
#define BIT6	0x01000000
#define BIT7	0x10000000

// # bits in a byte
#define NBIB			8

// Log base 2 of NBIB
#define LOG2NBIB		3

// Mask for LOG2NBIB bits
#define MASKLOG2NBIB	((1 << LOG2NBIB) - 1)

// # bits in a dword
#define NBID		   32

// Log base 2 of NBID
#define LOG2NBID		5

// Mask for LOG2NBID bits
#define MASKLOG2NBID	((1 << LOG2NBID) - 1)

// End value for shift mask
#define END_OF_BYTE 	(1 << NBIB)

// Width and height of each image in the image list
#define IMAGE_CX		16
#define IMAGE_CY		16

// Extensions
#define WKSEXT			".ws.nars"
#define WS_WKSEXT		L".ws.nars"

//***************************************************************************
//	End of File: main.h
//***************************************************************************
