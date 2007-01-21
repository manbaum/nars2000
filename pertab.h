//***************************************************************************
//  NARS2000 -- Per Tab Header
//***************************************************************************

// Structure for Per Tab Control Data
typedef struct tagPERTABDATA
{
#define DEFINE_VARS
#include "pertabdefs.h"
#undef  DEFINE_VARS

    char    DPFE[_MAX_PATH];    // The Drive, Path, Filename, & Ext of the WS
    int     iLabelText;         // Offset in DPFE of the label text
    HWND    hWndActive;         // Active MDI window when last switched out
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
