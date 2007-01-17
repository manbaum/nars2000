//' $Header$

//****************************************************************************
//  NARS2000 -- Main Header File
//****************************************************************************

#include <Qualitas.h>

#define MB(a)   MessageBox (NULL, a, "NARS2000", MB_OK)
#define MBC(a)  if (MessageBox (NULL, a, "NARS2000", MB_OKCANCEL) EQ IDCANCEL) DbgBrk ()
#define IsGlbPtr(a) (GlobalFlags (a) NE GMEM_INVALID_HANDLE)
#define IsGlbTypeVarDir(a) (IsGlobalTypeArray (a, VARARRAY_HEADER_SIGNATURE))
#define IsGlbTypeVarInd(a) (IsGlobalTypeArray (*(LPVOID *) a, VARARRAY_HEADER_SIGNATURE))
#define IsGlbTypeFcnDir(a) (IsGlobalTypeArray (a, FCNARRAY_HEADER_SIGNATURE))
#define IsGlbTypeFcnInd(a) (IsGlobalTypeArray (*(LPVOID *) a, FCNARRAY_HEADER_SIGNATURE))

#define LOPART_DWORDLONG        ((DWORDLONG) 0x00000000FFFFFFFF)
#define HIPART_DWORDLONG        ((DWORDLONG) 0xFFFFFFFF00000000)

// System Comparison Tolerance
#define SYS_CT  3E-15

#define LODWORD(x)              ( (DWORD) (   (x) & LOPART_DWORDLONG ) )
#define HIDWORD(x)              ( (DWORD) ( ( (x) & HIPART_DWORDLONG ) >> 32 ) )

#define defstop \
default:    \
    DbgStop();      // We should never get here

#define FNLN    FileNameOnly (__FILE__), __LINE__

#ifdef DEBUG

#define DBGENTER    DbgMsgW (L"Entering" APPEND_NAME)
#define DBGEXIT     DbgMsgW (L"Exiting " APPEND_NAME)

#define DbgGlobalAlloc(uFlags,ByteRes) \
DbgGlobalAllocSub (uFlags, ByteRes, L"##GlobalAlloc in " APPEND_NAME L": %08X (%S#%d)", FNLN);

#define DbgGlobalFree(hGlbToken) \
dprintfW (L"**GlobalFree in " APPEND_NAME L": %08X (%S#%d)", hGlbToken, FNLN); \
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
#define DBGEXIT

#define DbgGlobalAlloc(uFlags,ByteRes)  MyGlobalAlloc (uFlags, ByteRes);

#define DbgGlobalFree(hGlbToken)        MyGlobalFree (hGlbToken);

#define DbgIncrRefCntDir(hGlbData)      IncrRefCntDir (hGlbData);

#define DbgIncrRefCntInd(hGlbData)      IncrRefCntInd (hGlbData);

#define DbgDecrRefCntDir(hGlbData)      DecrRefCntDir (hGlbData);

#define DbgDecrRefCntInd(hGlbData)      DecrRefCntInd (hGlbData);

#endif

//*************************** Window Data ********************************

// Define offsets in SMWNDCLASS window extra bytes
#define GWLSM_EXTRA     0
//#define GWLSM_HGLB      0
//#define GWLSM_HFONT     GWLSM_HGLB + 1 * sizeof (long)  // Offset of hFontAPL
//#define GWLSM_EXTRA     GWLSM_HFONT + 1 * sizeof (long) // Total # extra bytes
#define GWLDB_EXTRA     0


// Define local window messages
#define MYWM_MOVE           (WM_APP+ 0) // MF


enum EXTYPE {EX_IMMEX, EX_FN};

typedef struct tagEXECSTATE
{
    enum EXTYPE exType;
} EXECSTATE;


typedef struct tagGLBHIST
{
    HGLOBAL *hGlb;
    UINT    First:1,            // This line is the first one in the array
            ContPrev:1,         // This line is connected to the previous line
            ContNext:1;         // ...                           next     ...
} GLBHIST, *LPGLBHIST;

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


//***************************************************************************
//  End of File: main.h
//***************************************************************************
