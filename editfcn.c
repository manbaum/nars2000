//***************************************************************************
//  NARS2000 -- Function Editor
//***************************************************************************

#define STRICT
#include <windows.h>

#include "colors.h"
#include "main.h"
#include "resource.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

WCHAR debugText[] =
L"z" WS_UCS2_LEFTARROW L"[(L1 L2 L3)] (LO FOO RO) (R1 R2 R3 R4)\r\n"
L"z" WS_UCS2_LEFTARROW WS_UCS2_DELTASTILE WS_UCS2_DELTASTILE L"R1"
;

char szCloseMessage[] = "You have changed the body of this function; save the changes?";

WNDPROC lpfnOldEditCtrlWndProc;         // Save area for old Edit Control procedure

COLORREF crLineNum = RGB (143,188,143);   // Darkseagreen


//***************************************************************************
//  CreateFcnWindow
//
//  Create a function window
//***************************************************************************

BOOL CreateFcnWindow
    (LPWCHAR lpwszLine)

{
    HWND hWnd;
    WCHAR wszFEClass[] = LFEWNDCLASS;

    // ***FIXME*** -- What do we do with this handle??

    hWnd =
    CreateMDIWindowW (wszFEClass,           // Class name
                      wszFETitle,           // Window title
                      0,                    // Styles
                      CW_USEDEFAULT,        // X-pos
                      CW_USEDEFAULT,        // Y-pos
                      CW_USEDEFAULT,        // Height
                      CW_USEDEFAULT,        // Width
                      hWndMC,               // Parent
                      _hInstance,           // Instance
                      (LPARAM) &lpwszLine[1]);  // Extra data
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

LRESULT APIENTRY FEWndProc
    (HWND hWnd,         // Window handle
     UINT message,      // Type of message
     UINT wParam,       // Additional information
     LONG lParam)       // ...

{
    HWND    hWndEB;         // Handle of Edit box window
    int     iMaxLimit;      // Maximum # chars in edit control
    VKSTATE vkState;        // Virtual key state (Shift, Alt, Ctrl)
    long    lvkState;       // Temporary var for vkState
    RECT    rcFmtEB;        // Formatting rectangle for the Edit Control

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
        case WM_CREATE:             // lpcs = (LPCREATESTRUCT) lParam; // Structure with creation data

#define lpMDIcs     ((LPMDICREATESTRUCT) (((LPCREATESTRUCT) lParam)->lpCreateParams))

            // Initialize variables
            cfFE.hwndOwner = hWnd;
            ZeroMemory (&vkState, sizeof (vkState));
            SetWindowLong (hWnd, GWLFE_LINECNT, 1);

            // Save in window extra bytes
            vkState.Ins = 1;        // Initially inserting ***FIXME*** Make it an option
            SetWindowLong (hWnd, GWLFE_VKSTATE, *(long *) &vkState);

            // Create an edit box within which we can edit
            hWndEB =
            CreateWindowExW (0L,                    // Extended styles
                             L"EDIT",               // Class name
                             L"",                   // Initial text
                             0
                           | WS_CHILD
                           | WS_VSCROLL
                           | WS_VISIBLE
                           | ES_MULTILINE
                           | ES_WANTRETURN
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

////        // Show and paint the window
////        ShowWindow (hWndEB, SW_SHOWNORMAL);
////        UpdateWindow (hWndEB);  // Redraw the screen now

            // Subclass the edit control so we can handle some of its messages
            lpfnOldEditCtrlWndProc = (WNDPROC)
              SetWindowLongW (hWndEB,
                              GWL_WNDPROC,
                              (long) (WNDPROC) &LclEditCtrlWndProc);

            // Give it the focus
            SetFocus (hWndEB);

            // Tell the edit box about its font
            SendMessage (hWndEB, WM_SETFONT, (WPARAM) hFontFE, 0);

            // Display the caret
            ShowCaret (hWndEB);

            // Set the initial text
            SendMessageW (hWndEB, WM_SETTEXT, 0, (LPARAM) lpMDIcs->lParam);

            // Paint the window
            UpdateWindow (hWndEB);

            break;

#undef  lpMDIcs

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
#define nWidth      (LOWORD (lParam))
#define nHeight     (HIWORD (lParam))
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
            // Get the handle to the edit control
            hWndEB = (HWND) GetWindowLong (hWnd, GWLFE_HWNDEB);

////////    // Get the current vkState
////////    lvkState = GetWindowLong (hWnd, GWLFE_VKSTATE);
////////    vkState = *(LPVKSTATE) &lvkState;

            // Pass on to the edit ctrl
            SetFocus (hWndEB);

            break;
////////
////////case WM_KILLFOCUS:
////////    DestroyCaret ();    // 'cause we just lost the focus
////////
////////    break;

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

                case EN_SETFOCUS:                   // idEditCtrl = (int) LOWORD(wParam); // identifier of edit control
                                                    // wNotifyCode = HIWORD(wParam);      // notification code
                                                    // hwndEditCtrl = (HWND) lParam;      // handle of edit control
                    // Get the formatting rectangle
                    SendMessage (hWndEB, EM_GETRECT, 0, (LPARAM) &rcFmtEB);

                    // Move the rectangle over enough chars (6) to provide room
                    //   for line numbers
                    rcFmtEB.left = 6 * cxAveCharFE;

                    // Tell the control about this change
                    SendMessage (hWndEB, EM_SETRECT, 0, (LPARAM) &rcFmtEB);

                    // ***DEBUG*** -- Get the formatting rect again
                    SendMessage (hWndEB, EM_GETRECT, 0, (LPARAM) &rcFmtEB);

                    // Get the current vkState
                    lvkState = GetWindowLong (hWnd, GWLFE_VKSTATE);
                    vkState = *(LPVKSTATE) &lvkState;

                    // Create a default sized system caret for display
                    DestroyCaret ();        // 'cause we're changing the cursor width
                    MyCreateCaret (hWndEB, &vkState, cyAveCharFE, NULL);
                    if (vkState.Ins)
                        DbgMsg ("FE:  EN_SETFOCUS(1):  MyCreateCaret");
                    else
                        DbgMsg ("FE:  EN_SETFOCUS(0):  MyCreateCaret");

                    // Paint the window
                    UpdateWindow (hWndEB);

                    break;

                case EN_UPDATE:                     // idEditCtrl = (int) LOWORD(wParam); // identifier of edit control
                                                    // hwndEditCtrl = (HWND) lParam;      // handle of edit control
                    // Split cases based upon the last key
                    switch (GetWindowLong (hWnd, GWLFE_LASTKEY))
                    {
                        case VK_BACK:
                        case VK_RETURN:
                        case VK_DELETE:
                            // Draw the line #s
                            DrawLineNumsFE (hWndEB);

                            break;
                    } // End SWITCH

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
//  LclEditCtrlWndProc
//
//  Local window procedure for the Edit Control
//***************************************************************************

LRESULT WINAPI LclEditCtrlWndProc
    (HWND   hWnd,
     UINT   message,
     WPARAM wParam,
     LPARAM lParam)

{
    VKSTATE vkState;        // Virtual key state (Shift, Alt, Ctrl)
    long    lvkState;       // Temporary var for vkState
    HWND    hWndParent;     // Handle of parent (FE) window
    LRESULT lResult;        // Result from calling original handler

    // Split cases
    switch (message)
    {
#define nVirtKey ((int) wParam)
        case WM_KEYDOWN:            // nVirtKey = (int) wParam;     // Virtual-key code
                                    // lKeyData = lParam;           // Key data
            // Get the handle of the parent window
            hWndParent = GetParent (hWnd);

            // Save the key in the parent window's extra bytes
            SetWindowLong (hWndParent, GWLFE_LASTKEY, nVirtKey);

            // Process the virtual key
            switch (nVirtKey)
            {
                case VK_INSERT:         // Insert
                    // Get the current vkState
                    lvkState = GetWindowLong (hWndParent, GWLFE_VKSTATE);
                    vkState = *(LPVKSTATE) &lvkState;

                    vkState.Ins = !vkState.Ins;

                    // Save in window extra bytes
                    SetWindowLong (hWndParent, GWLFE_VKSTATE, *(long *) &vkState);

                    // Create a default sized system caret for display
                    DestroyCaret ();        // 'cause we're changing the cursor width
                    MyCreateCaret (hWnd, &vkState, cyAveCharFE, NULL);
                    DbgMsg ("FE:  VK_INSERT:    MyCreateCaret");

                    break;
            } // End SWITCH

            // We need to pass this message on to the next handler
            //   so WM_CHAR & WM_SYSCHAR can process it.
            break;
#undef  nVirtKey

#define chCharCode ((char) wParam)
        case WM_SYSCHAR:            // chCharCode = (TCHAR) wParam; // character code
                                    // lKeyData = lParam;           // Key data
        {
            int   iChar;
            WCHAR wch[2] = {L"\0"};

            // Handle Shifted & unshifted Alt chars
            //  e.g., 'a' = 97, 'z' = 122

            iChar = chCharCode - ' ';
            if (iChar < (sizeof (aCharCode) / sizeof (CHARCODE)))
            {
                // Get the Alt- char code
                wch[0] = aCharCode[iChar].alt;

                // If it's valid, return it
                if (wch[0])
                {
////                // Unset the selection
////                SendMessage (hWnd, EM_SETSEL, (WPARAM) -1, (LPARAM) -1);

                    // Insert the translated char into the text
                    SendMessageW (hWnd, EM_REPLACESEL, (WPARAM) TRUE, (LPARAM) wch);

                    return FALSE;       // We handled the msg
                } else
                // Otherwise, DbgMsg it
                {
#ifdef DEBUG
                    wsprintfW (lpwszTemp,
                               L"SYSCHAR:  chCharCode = %d, %c",
                               chCharCode,
                               chCharCode);
                    DbgMsgW (lpwszTemp);
#endif
                } // End IF/ELSE
            } else
            {
#ifdef DEBUG
                wsprintfW (lpwszTemp,
                           L"SYSCHAR:  chCharCode = %d, %c",
                           chCharCode,
                           chCharCode);
                DbgMsgW (lpwszTemp);
#endif
            } // End IF/ELSE

            break;
        } // End WM_SYSCHAR
#undef  chCharCode

        case WM_PAINT:
        case WM_PASTE:
        case WM_CUT:
        case WM_UNDO:
            lResult = CallWindowProcW (lpfnOldEditCtrlWndProc,
                                       hWnd,
                                       message,
                                       wParam,
                                       lParam); // Pass on down the line
            // Draw the line #s
            DrawLineNumsFE (hWnd);

            return lResult;
    } // End SWITCH

    return CallWindowProcW (lpfnOldEditCtrlWndProc,
                            hWnd,
                            message,
                            wParam,
                            lParam); // Pass on down the line
} // End LclEditCtrlWndProc


//***************************************************************************
//  DrawLineNumsFE
//
//  Draw the line #s in an FE window
//***************************************************************************

void DrawLineNumsFE
    (HWND hWnd)

{
    HDC     hDC;            // Device context
    RECT    rcPaint;        // Paint rectangle
    UINT    uLen,           // Length of string
            uLineCount,     // # lines in the edit control
            uPrevCount,     // ...                        , the previous time
            uCnt;           // Counter
    WCHAR   wszLineNum[8];  // Line # (e.g. L"[00000]\0"
    HWND    hWndParent;     // Window handle of the parent (i.e. hWndFE)

    // Get the handle to the parent window (hWndFE)
    hWndParent = GetParent (hWnd);

    // Get a device context
    hDC = MyGetDC (hWnd);

    // Set our DC attributes
    SetAttrs (hDC, hFontFE, crLineNum, COLOR_WHITE);

    // Get the # lines in the text
    uLineCount = SendMessage (hWnd, EM_GETLINECOUNT, 0, 0);

    // Loop through the line #s
    for (uCnt = 0; uCnt < uLineCount; uCnt++)
    {
        // Format the line #
        wsprintfW (wszLineNum,
                   L"[%d]",
                   uCnt);
        uLen = lstrlenW (wszLineNum);

        // Calculate the rectangle size of the line #s
        SetRectEmpty (&rcPaint);
        DrawTextW (hDC,
                   wszLineNum,
                   uLen,
                   &rcPaint,
                   0
                 | DT_CALCRECT);
        // Move the rectangle down
        rcPaint.top    += uCnt * cyAveCharFE;
        rcPaint.bottom += uCnt * cyAveCharFE;

        // Draw the line #s
        DrawTextW (hDC,
                   wszLineNum,
                   uLen,
                   &rcPaint,
                   0
                 | DT_LEFT);
    } // End FOR

    // Get the previous line count
    uPrevCount = GetWindowLong (hWndParent, GWLFE_LINECNT);

#define EMPTY_LINE  L"      "

    uLen = lstrlenW (EMPTY_LINE);

    // In case we deleted lines, draw more empty line #s
    for (; uCnt < uPrevCount; uCnt++)
    {
        // Move the rectangle down
        rcPaint.top    += cyAveCharFE;
        rcPaint.bottom += cyAveCharFE;

        DrawTextW (hDC,
                   EMPTY_LINE,
                   uLen,
                   &rcPaint,
                   0
                 | DT_LEFT);
    } // End FOR

#undef  EMPTY_LINE

    // Save the current line count for the next time
    SetWindowLong (hWndParent, GWLFE_LINECNT, uLineCount);

    // We no longer need this DC
    MyReleaseDC (hWnd, hDC);
} // End DrawLineNumsFE


//***************************************************************************
//  QueryCloseFE
//
//  Return a Boolean result on whether we can close
//***************************************************************************

BOOL QueryCloseFE
    (HWND hWnd)

{
    // If the text hasn't changed, continue with close
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
