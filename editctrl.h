//***************************************************************************
//  NARS2000 -- Edit Control Header
//***************************************************************************


#define EM_SETPAINTHOOK     0x00BF

#define GWLEC_ES            0
#define GWLEC_PAINTHOOK     GWLEC_ES        + 1 * sizeof(long)
#define GWLEC_EXTRA         GWLEC_PAINTHOOK + 1 * sizeof(long)


typedef int PAINTHOOK (HDC, int, int, LPWSTR, int, int);
typedef PAINTHOOK *LPPAINTHOOK;

#define MYWM_INIT_EC        (WM_USER + 0)
#define MYWM_REDO           (WM_USER + 1)
#define MYWM_SELECTALL      (WM_USER + 2)
#define MYWM_KEYDOWN        (WM_USER + 3)
#define MYWM_PASTE_APLWIN   (WM_USER + 4)
#define MYWM_PASTE_APL2     (WM_USER + 5)

#define FCN_INDENT  6

#define WM_REDO     (WM_USER + 0)

#define DEF_UNDOBUF_INITSIZE     4*1024
#define DEF_UNDOBUF_MAXSIZE     64*1024

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
