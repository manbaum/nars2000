//***************************************************************************
//  NARS2000 -- Matrix Editor
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "resdebug.h"
#include "Unicode.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  MEWndProc
//
//  Message processing routine for the Matrix Editor window
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MEWndProc"
#else
#define APPEND_NAME
#endif

LRESULT APIENTRY MEWndProc
    (HWND hWnd,         // Window handle
     UINT message,      // Type of message
     UINT wParam,       // Additional information
     LONG lParam)       // ...

{
////static DWORD aHelpIDs[] = {
////                           IDOK,             IDH_OK,
////                           IDCANCEL,         IDH_CANCEL,
////                           IDC_APPLY,        IDH_APPLY,
////                           IDC_HELP2,        IDH_HELP2,
////                           0,             0,
////                          };
////ODSAPI ("ME: ", hWnd, message, wParam, lParam);
    switch (message)
    {
        case WM_CREATE:
            DbgBrk ();


            break;



    } // End SWITCH

////ODSAPI ("MEZ:", hWnd, message, wParam, lParam);
    return DefMDIChildProc (hWnd, message, wParam, lParam);
} // End MEWndProc
#undef  APPEND_NAME


//***************************************************************************
//  End of File: editmat.c
//***************************************************************************
