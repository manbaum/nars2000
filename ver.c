//***************************************************************************
//  NARS2000 -- Version Routines
//***************************************************************************

#define STRICT
#define WINVER       0x0500 // Needed for WINUSER.H definitions
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <winver.h>
#include <colors.h>

#include "main.h"
#include "resource.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

char szVarFileInfo[] = "\\VarFileInfo\\Translation";

HWND    hWndStatic;                 // Handle to static control
WNDPROC lpfnOldStaticWndProc;       // Save area for old Static Control procedure


//***************************************************************************
//  LclFileVersionStr
//
//  Get file version string
//***************************************************************************

void LclFileVersionStr
    (LPSTR pszFileName,
     LPSTR szFileVer)

{
    DWORD dwVerHandle;  // Version file handle
    DWORD dwVerSize;    // Size of the VERSIONINFO struc
    DWORD dwTrans;  // Translation value
    LPSTR lpVer, lpBuf;
    HLOCAL hLoc;
    UINT  cb;
    char szTemp[128];

    szTemp[0] = '\0';       // Ensure properly terminated in case we fail
    dwVerSize = GetFileVersionInfoSize (pszFileName, &dwVerHandle);

    if (dwVerSize EQ 0)     // If it didn't work, ...
        return;

    // Allocate space for the file version info
    hLoc = LocalAlloc (LPTR, (long) dwVerSize);
    lpVer = (LPSTR) hLoc;
    if (lpVer EQ NULL)      // If it didn't work
        return;

    // Read in the file version info
    if (!GetFileVersionInfo (pszFileName, dwVerHandle, dwVerSize, lpVer))
        goto FREE;

    // Get the translation string
    // Note that we must put the VarFileInfo string into read-write
    // memory because, although the prototype for VerQueryValue has
    // this string cast as LPCSTR, the Win 3.1x version of this API
    // call writes into the string (zapping the intermediate backslash
    // so it can lstrcmpi against various possible cases).
////if (!VerQueryValue (lpVer, "\\VarFileInfo\\Translation", &lpBuf, &cb))
    if (!VerQueryValue (lpVer, szVarFileInfo, &lpBuf, &cb))
        goto FREE;

    // Extract the translation value (with the swapped words)
    dwTrans = *(DWORD FAR *)lpBuf;
    wsprintf (szTemp, "\\StringFileInfo\\%08lX\\FileVersion",
            MAKELONG (HIWORD (dwTrans), LOWORD (dwTrans)));
    if (!VerQueryValue (lpVer, szTemp, &lpBuf, &cb))
        goto FREE;

    // Copy to local storage as we're about to free the memory
    lstrcpy (szFileVer, lpBuf);
FREE:
    LocalFree (hLoc);       // Free the local memory
} // End LclFileVersionStr


//***************************************************************************
//  AboutDlgProc
//
//  Just yer standard About box.
//***************************************************************************

BOOL CALLBACK AboutDlgProc
    (HWND   hDlg,
     UINT   message,
     WPARAM wParam,
     LPARAM lParam)

{
    static HFONT hFont = NULL;

    // Split cases
    switch (message)
    {
        case WM_INITDIALOG:
        {
            char szTemp[512], szFileVer[32];

            // Read in the application's File Version String
            LclFileVersionStr (szAppDPFE, szFileVer);

            // Format the version #
            wsprintf (szTemp, lpszVersion, szFileVer);

            // Write out the version string
            SetDlgItemText (hDlg, IDC_VERSION, szTemp);

            CenterWindow (hDlg);    // Reposition the window to the center of the screen

            // Get the IDC_LINK window handle
            hWndStatic = GetDlgItem (hDlg, IDC_LINK);

            // Subclass the IDC_LINK control
            //   so we can handle WM_LBUTTONDOWN
            lpfnOldStaticWndProc =
              (WNDPROC) SetWindowLong (hWndStatic,
                                       GWL_WNDPROC,
                                       (long) (WNDPROC) &LclStaticWndProc);
            // Set the cursor for the static control to a hand



            return TRUE;            // Use the focus in wParam
        } // End WM_INITDIALOG

        case WM_CTLCOLORSTATIC:     // hdc = (HDC) wParam;   // Handle of display context
                                    // hwnd = (HWND) lParam; // Handle of static control
#define hDC     ((HDC)  wParam)
#define hWnd    ((HWND) lParam)

            // We handle IDC_LINK static window only
            if (hWnd == hWndStatic)
            {
                LOGFONT lf;

                // Set the text color
                SetTextColor (hDC, COLOR_BLUE);

                // Set the background text color
                SetBkColor (hDC, GetSysColor (COLOR_BTNFACE));

                // Get the font handle
                hFont = (HFONT) SendMessage (hWnd, WM_GETFONT, 0, 0);

                // Get the LOGFONT structure for the font
                GetObject (hFont, sizeof (lf), &lf);

                // Change to an underlined font
                lf.lfUnderline = TRUE;

                // Create a new font, now underlined
                hFont = CreateFontIndirect (&lf);

                // Select it into the DC
                SelectObject (hDC, hFont);

                // Return handle of brush for background
                return (int) CreateSolidBrush (GetSysColor (COLOR_BTNFACE));
            } else
                break;
#undef  hWnd
#undef  hDC

        case WM_CLOSE:
            // If it's still around, delete the font we created
            if (hFont)
            {
                DeleteObject (hFont); hFont = NULL;
            } // End IF

            EndDialog (hDlg, TRUE); // Quit this dialog

            return TRUE;            // We handled the msg

        case WM_COMMAND:
            // If the user pressed one of our buttons, ...
            switch (GET_WM_COMMAND_ID (wParam, lParam))
            {
                case IDOK:
                    PostMessage (hDlg, WM_CLOSE, 0, 0);

                    return TRUE;    // We handled the msg
            } // End switch (wParam)

            break;
    } // End SWITCH

    return FALSE;           // We didn't handle the msg
} // End AboutDlgProc


//***************************************************************************
//  LclStaticWndProc
//
//  Local window procedure for the About Box
//  static control to handle left clicks.
//***************************************************************************

LRESULT WINAPI LclStaticWndProc
    (HWND   hWnd,
     UINT   message,
     WPARAM wParam,
     LPARAM lParam)

{
    // Split cases
    switch (message)
    {
        case WM_LBUTTONDOWN:
        {
            char szStaticText[128];             // Text of static control

            // The user clicked on our static text --
            //   make it behave like a hyperlink

            // Get the static text so we can pass it to ShellExecute
            GetWindowText (hWndStatic,
                           szStaticText,
                           sizeof (szStaticText) - 1);
            // Call the shell to invoke the browser
            ShellExecute (hWnd,             // Handle to parent window
                          "open",           // Operation to perform
                          szStaticText,     // Object on which to perform operation
                          NULL,             // Executable file parameters
                          NULL,             // Default directory
                          SW_SHOWNORMAL     // How window is to shown when opened
                         );
            return FALSE;           // We handled the msg
        } // End WM_LBUTTONDOWN

        case WM_SETCURSOR:          // hwnd = (HWND) wParam;       // handle of window with cursor
                                    // nHittest = LOWORD(lParam);  // hit-test code
                                    // wMouseMsg = HIWORD(lParam); // mouse-message identifier
            // Set a new cursor to indicate that
            //   this is a hyperlink.
            SetCursor (LoadCursor (NULL, IDC_HAND));

            return FALSE;           // We handled the msg
    } // End SWITCH

    return CallWindowProc (lpfnOldStaticWndProc,
                           hWnd,
                           message,
                           wParam,
                           lParam); // Pass on down the line
} // End LclStaticWndProc


//***************************************************************************
//  End of File: ver.c
//***************************************************************************
