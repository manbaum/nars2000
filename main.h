//****************************************************************************
//  NARS2000 -- Main Header File
//****************************************************************************

#define EQ ==
#define NE !=

#define LOSHORT(l)  ((short)((DWORD)(l) & 0xffff))
#define HISHORT(l)  ((short)((DWORD)(l) >> 16))

#define TRUE    1
#define FALSE   0

#define abs64(a)    (((a)>0)?a:-(a))
#define MB(a)   MessageBox (NULL, a, "NARS2000", MB_OK)
#define MBC(a)  if (MessageBox (NULL, a, "NARS2000", MB_OKCANCEL) EQ IDCANCEL) DbgBrk ()
#define IsGlbPtr(a) (GlobalFlags (a) NE GMEM_INVALID_HANDLE)
#define IsGlbTypeVarDir(a) (IsGlobalTypeArray (a, VARARRAY_HEADER_SIGNATURE))
#define IsGlbTypeVarInd(a) (IsGlobalTypeArray (*(LPVOID *) a, VARARRAY_HEADER_SIGNATURE))
#define IsGlbTypeFcnDir(a) (IsGlobalTypeArray (a, FCNARRAY_HEADER_SIGNATURE))
#define IsGlbTypeFcnInd(a) (IsGlobalTypeArray (*(LPVOID *) a, FCNARRAY_HEADER_SIGNATURE))

#define AplModI(m,a) PrimFnDydStileIisIvI (m, a, NULL)
#define AplModF(m,a) PrimFnDydStileFisFvF (m, a, NULL)

#define LOPART_DWORDLONG        ((DWORDLONG) 0x00000000FFFFFFFF)
#define HIPART_DWORDLONG        ((DWORDLONG) 0xFFFFFFFF00000000)

#define LODWORD(x)              ( (DWORD) (   (x) & LOPART_DWORDLONG ) )
#define HIDWORD(x)              ( (DWORD) ( ( (x) & HIPART_DWORDLONG ) >> 32 ) )

#define QWORD   ULONGLONG

#define defstop \
default:    \
    DbgStop();      // We should never get here

#define FNLN    FileNameOnly (__FILE__), __LINE__

#ifdef DEBUG

#define DBGENTER    DbgMsgW (L"Entering" APPEND_NAME)
#define DBGLEAVE    DbgMsgW (L"Leaving " APPEND_NAME)

#define DbgGlobalAlloc(uFlags,ByteRes) \
DbgGlobalAllocSub (uFlags, ByteRes, L"##GlobalAlloc in " APPEND_NAME L": %08X (%S#%d)", FNLN)

#define DbgGlobalFree(hGlbToken) \
dprintfW (L"**GlobalFree  in " APPEND_NAME L": %08X (%S#%d)", hGlbToken, FNLN); \
MyGlobalFree (hGlbToken);

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

#define DBGENTER
#define DBGLEAVE

#define DbgGlobalAlloc(uFlags,ByteRes)  MyGlobalAlloc (uFlags, ByteRes);

#define DbgGlobalFree(hGlbToken)        MyGlobalFree (hGlbToken);

#define DbgIncrRefCntDir(hGlbData)      IncrRefCntDir (hGlbData);

#define DbgIncrRefCntInd(hGlbData)      IncrRefCntInd (hGlbData);

#define DbgDecrRefCntDir(hGlbData)      DecrRefCntDir (hGlbData);

#define DbgDecrRefCntInd(hGlbData)      DecrRefCntInd (hGlbData);

#define DbgMsg(a)
#define DbgMsgW(a)

#endif

//*************************** Window Data ********************************

// Define offsets in SMWNDCLASS window extra bytes
#define GWLSM_VKSTATE   0
#define GWLSM_EXTRA     GWLSM_VKSTATE  + 1 * sizeof (long)

// Define offsets in DBWNDCLASS window extra bytes
#define GWLDB_EXTRA     0

// Define offsets in FEWNDCLASS window extra bytes
#define GWLFE_HWNDEB    0               // Handle of the matching Edit box control
#define GWLFE_UNDO_BEG  GWLFE_HWNDEB   + 1 * sizeof (long)
#define GWLFE_UNDO_NXT  GWLFE_UNDO_BEG + 1 * sizeof (long)
#define GWLFE_UNDO_LST  GWLFE_UNDO_NXT + 1 * sizeof (long)
#define GWLFE_UNDO_GRP  GWLFE_UNDO_LST + 1 * sizeof (long)
#define GWLFE_LINECNT   GWLFE_UNDO_GRP + 1 * sizeof (long)
#define GWLFE_VKSTATE   GWLFE_LINECNT  + 1 * sizeof (long)
#define GWLFE_LASTKEY   GWLFE_VKSTATE  + 1 * sizeof (long)
#define GWLFE_CHANGED   GWLFE_LASTKEY  + 1 * sizeof (long)
#define GWLFE_EXTRA     GWLFE_CHANGED  + 1 * sizeof (long) // Total # extra bytes

// Define offsets in MEWNDCLASS window extra bytes
#define GWLME_EXTRA     0

// Define offsets in VEWNDCLASS window extra bytes
#define GWLVE_EXTRA     0


// Define local window messages
#define MYWM_MOVE           (WM_APP+ 0) // MF


typedef enum tagEXTYPE
{
    EX_IMMEX, EX_FN
} EXTYPE;

typedef struct tagEXECSTATE
{
    EXTYPE exType;
} EXECSTATE, *LPEXECSTATE;


typedef struct tagGLBHIST
{
    HGLOBAL *hGlb;
    UINT    First:1,            // This line is the first one in the array
            ContPrev:1,         // This line is connected to the previous line
            ContNext:1;         // ...                           next     ...
} GLBHIST, *LPGLBHIST;

typedef struct tagVKSTATE
{
    ULONG Shift:1,              // Left- or right-shift key up(0) or down(1)
////     lShift:1,
////     rShift:1,
          Alt:1,                // Left- or right-Alt key up(0) or down(1)
////     lAlt:1,
////     rAlt:1,
          Ctl:1,                // Left or -right Ctl key up(0) or down(1)
////     lCtl:1,
////     rCtl:1,
          Ins:1;                // Replace(0) or insert(1)
} VKSTATE, *LPVKSTATE;

#ifndef DEBUG
#define Assert(a)
#endif

#define DEF_STRAND_INITSIZE     65536       // Initial size in tokens of the strand stack
#define DEF_STRAND_MAXSIZE      1024*1024   // Maximum ...

// Resource debugging
#define MAXOBJ  12800

// Define bit masks
#define BIT0    0x00000001

// # bits in a byte
#define NBIB            8

// Log base 2 of NBIB
#define LOG2NBIB        3

// Mask for LOG2NBIB bits
#define MASKLOG2NBIB    ((1 << LOG2NBIB) - 1)

// End value for shift mask
#define END_OF_BYTE     (1 << NBIB)

// Width and height of each image in the image list
#define IMAGE_CX        16
#define IMAGE_CY        16


//***************************************************************************
//  End of File: main.h
//***************************************************************************
