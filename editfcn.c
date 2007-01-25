//***************************************************************************
//  NARS2000 -- Function Editor
//***************************************************************************

#define STRICT
#include <windows.h>
#include <windowsx.h16>

#include "main.h"
#include "resource.h"
#include "resdebug.h"
#include "Unicode.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

WCHAR debugText[] =
L"[0]   z" WS_UCS2_LEFTARROW L"[(L1 L2 L3)] (LFN FOO RFN) (R1 R2 R3 R4)\r\n"
L"[1]   z" WS_UCS2_LEFTARROW WS_UCS2_DELTASTILE WS_UCS2_DELTASTILE L"R1"
;

char szCloseMessage[] = "You changed the body of this function; save the changes?";


//***************************************************************************
//  CreateFcnWindow
//
//  Create a function window
//***************************************************************************

BOOL CreateFcnWindow
    (LPWCHAR lpwszLine)

{
    HWND hWnd;

    // ***FIXME*** -- What do we do with this handle??

    hWnd =
    CreateMDIWindow (szFEClass,             // Class name
                     szFETitle,             // Window title
                     0,                     // Styles
                     CW_USEDEFAULT,         // X-pos
                     CW_USEDEFAULT,         // Y-pos
                     CW_USEDEFAULT,         // Height
                     CW_USEDEFAULT,         // Width
                     hWndMC,                // Parent
                     _hInstance,            // Instance
                     0);                    // No extra data
    if (hWnd EQ NULL)
    {
        MB (pszNoCreateFEWnd);
        return FALSE;
    } // End IF

    // Make the window visible; update its client area
    ShowWindow (hWnd, SW_SHOWNORMAL);
    UpdateWindow (hWnd);

    return TRUE;
} // End CreateFcnWindow


//***************************************************************************
//  EB_Create
//
//  Perform window-specific initialization
//***************************************************************************

void EB_Create
    (HWND hWnd)

{
} // End EB_Create


//***************************************************************************
//  EB_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void EB_Delete
    (HWND hWnd)

{
} // End EB_Delete


//***************************************************************************
//  FEWndProc
//
//  Message processing routine for the Function Editor window
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FEWndProc"
#else
#define APPEND_NAME
#endif

LRESULT APIENTRY
FEWndProc (HWND hWnd,       // Window handle
           UINT message,    // Type of message
           UINT wParam,     // Additional information
           LONG lParam)     // ...

{
    HWND hWndEB;            // Handle of Edit box window
    int  iMaxLimit;         // Maximum # chars in edit control

////static DWORD aHelpIDs[] = {
////                           IDOK,             IDH_OK,
////                           IDCANCEL,         IDH_CANCEL,
////                           IDC_APPLY,        IDH_APPLY,
////                           IDC_HELP2,        IDH_HELP2,
////                           0,             0,
////                          };
////ODSAPI ("FE: ", hWnd, message, wParam, lParam);
    switch (message)
    {
        case WM_CREATE:
            // Create an edit box within which we can edit
            hWndEB =
            CreateWindowExW (0L,                    // Extended styles
                             L"EDIT",               // Class name
                             debugText,             // Initial text
                             WS_CHILD
                           | WS_VSCROLL
                           | WS_VISIBLE
                           | ES_MULTILINE
                           | ES_NOHIDESEL           // ***TESTME***
                           | ES_AUTOHSCROLL
                           | ES_AUTOVSCROLL,        // Styles
                             0,                     // X-position
                             0,                     // Y-...
                             CW_USEDEFAULT,         // Width
                             CW_USEDEFAULT,         // Height
                             hWnd,                  // Parent window
                             (HMENU) IDWC_FE_EB,    // ID
                             _hInstance,            // Instance
                             0);                    // lParam
            if (hWndEB EQ NULL)
            {
                MB (pszNoCreateFEEditCtrl);

                return -1;          // Stop the whole process
            } // End IF

            // Save in window extra bytes
            SetWindowLong (hWnd, GWLFE_HWNDEB, (long) hWndEB);

            // Give it the focus
            SetFocus (hWndEB);

            // Tell the edit box about its font
            SendMessage (hWndEB, WM_SETFONT, (WPARAM) hFontFE, 0);

            // Display the caret
            ShowCaret (hWndEB);

            // Set the cursor to the first char (after the line #) on the first line
            SetCaretPos (6, 0);

            break;

        case WM_SETFONT:
            // Get the handle to the edit control
            hWndEB = (HWND) GetWindowLong (hWnd, GWLFE_HWNDEB);

            // Pass it on the the edit control
            SendMessage (hWndEB, message, wParam, lParam);

            break;

        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
            // Uninitialize window-specific resources
            EB_Delete (hWnd);

            // Initialize window-specific resources
            EB_Create (hWnd);

            break;                  // Continue with next handler

#define fwSizeType  wParam
#define nWidth      (LOWORD(lParam))
#define nHeight     (HIWORD(lParam))
        case WM_SIZE:               // fwSizeType = wParam;      // resizing flag
                                    // nWidth = LOWORD(lParam);  // width of client area
                                    // nHeight = HIWORD(lParam); // height of client area
            // Get the handle to the edit control
            hWndEB = (HWND) GetWindowLong (hWnd, GWLFE_HWNDEB);

            if (fwSizeType NE SIZE_MINIMIZED)
                SetWindowPos (hWndEB,           // Window handle to position
                              0,                // SWP_NOZORDER
                              0,                // X-position
                              0,                // Y-...
                              nWidth,           // Width
                              nHeight,          // Height
                              SWP_NOZORDER      // Flags
                            | SWP_SHOWWINDOW
                             );
            break;                  // Continue with next handler ***MUST***
#undef  nHeight
#undef  nWidth
#undef  fwSizeType

        case WM_SETFOCUS:
            SetFocus ((HWND) GetWindowLong (hWnd, GWLFE_HWNDEB));

            break;

////    case WM_KILLFOCUS:
////        DestroyCaret ();    // 'cause we just lost the focus
////
////        break;

#define wNotifyCode     (HIWORD (wParam))
#define wID             (LOWORD (wParam))
#define hWndCtrl        ((HWND) lParam)
        case WM_COMMAND:            // wNotifyCode = HIWORD(wParam); // Notification code
                                    // wID = LOWORD(wParam);         // Item, control, or accelerator identifier
                                    // hwndCtl = (HWND) lParam;      // Handle of control
            // Get the handle to the edit control
            hWndEB = (HWND) GetWindowLong (hWnd, GWLFE_HWNDEB);

            // This message should be from the edit control
            Assert (wID      EQ IDWC_FE_EB);
            Assert (hWndCtrl EQ hWndEB);

            // Split cases based upon the notify code
            switch (wNotifyCode)
            {
                case EN_MAXTEXT:    // idEditCtrl = (int) LOWORD(wParam); // identifier of edit control
                                    // hwndEditCtrl = (HWND) lParam;      // handle of edit control
                    // The edit control has exceed its maximum # chars
                    DbgBrk ();      // ***TESTME***

                    // The default maximum is 32K, so we increase it by that amount
                    iMaxLimit = SendMessage (hWndEB, EM_GETLIMITTEXT, 0, 0);
                    SendMessage (hWndEB, EM_SETLIMITTEXT, iMaxLimit + 32*1024, 0);

                    break;

                case EN_CHANGE:
                    // The contents of the edit control have changed,
                    // set the changed flag */
                    SetWindowLong (hWnd, GWLFE_CHANGED, TRUE);

                    break;

////            case EN_SETFOCUS:   // 0x0100
////            case EN_KILLFOCUS:  // 0x0200
////            case EN_CHANGE:     // 0x0300
////            case EN_UPDATE:     // 0x0400
////            case EN_ERRSPACE:   // 0x0500
////            case EN_MAXTEXT:    // 0x0501
////            case EN_HSCROLL:    // 0x0601
////            case EN_VSCROLL:    // 0x0602
                    break;
            } // End IF

            break;
#undef  hWndCtrl
#undef  wID
#undef  wNotifyCode

        case WM_QUERYENDSESSION:
        case WM_CLOSE:
            // If it's OK to close the window, do so, else ignore
            if (!QueryCloseFE (hWnd))
                return FALSE;       // Do not close
            break;                  // Default action is to close

        case WM_DESTROY:
            // Uninitialize window-specific resources
            EB_Delete (hWnd);

            break;
    } // End SWITCH

////ODSAPI ("FEZ:", hWnd, message, wParam, lParam);
    return DefMDIChildProc (hWnd, message, wParam, lParam);
} // End FEWndProc
#undef  APPEND_NAME


//***************************************************************************
//  QueryCloseFE
//
//  Return a Boolean result on whether we can close
//***************************************************************************

BOOL QueryCloseFE
    (HWND hWnd)

{
    // If the text has changed, ask the user
    if (!GetWindowLong (hWnd, GWLFE_CHANGED))
        return TRUE;

    // Ask the user what to do
    switch (MessageBox (hWnd, szCloseMessage, pszAppName, MB_YESNOCANCEL | MB_ICONQUESTION))
    {
        case IDYES:         // Save the function
            DbgBrk ();      // ***FINISHME***



            return TRUE;    // Now it's OK to close

        case IDNO:          // Do not save the function
            return TRUE;

        case IDCANCEL:      // Not OK to close
        default:            // Problem with MessageBox
            return FALSE;
    } // End SWITCH
} // End QueryClose


//***************************************************************************
//  End of File: editfcn.c
//***************************************************************************
