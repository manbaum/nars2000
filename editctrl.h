//***************************************************************************
//  NARS2000 -- Edit Control Header
//***************************************************************************


#define EM_SETPAINTHOOK     0x00BF

#define GWLEC_ES            0
#define GWLEC_PAINTHOOK     GWLEC_ES        + 1 * sizeof(long)
#define GWLEC_EXTRA         GWLEC_PAINTHOOK + 1 * sizeof(long)


typedef int PAINTHOOK (HDC, int, int, LPWSTR, int, int);
typedef PAINTHOOK *LPPAINTHOOK;

#define MYWM_DRAWLINENUMS   (WM_USER + 0)
#define MYWM_PASTE_APLWIN   (WM_USER + 1)
#define MYWM_REDO           (WM_USER + 2)
#define MYWM_SELECTALL      (WM_USER + 3)


//***************************************************************************
//  End of File: editctrl.h
//***************************************************************************
