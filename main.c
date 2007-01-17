//***************************************************************************
//  NARS2000 -- Open Source APL Interpreter
//***************************************************************************

#define COMPILE_MULTIMON_STUBS
//#define WINVER       0x04FF // Needed for WINUSER.H definitions
//#define _WIN32_WINNT 0x04FF // ...

#pragma pack (1)
#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <windowsx.h16>
#include <commctrl.h>   // Includes the common control header
//#include <multimon.h>   // Multiple monitor support
//#include <QControl.h>
#include <limits.h>
#include <direct.h>
#include "main.h"
#include "resdebug.h"
#include "resource.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif



//************************** Data Area **************************************

#define DEF_CTEMP_MAXSIZE   1024        // Maximum size of char  temporary storage
#define DEF_CTEMP_INITSIZE  1024        // Initial ...
#define DEF_WTEMP_MAXSIZE   1024        // Maximum size of WCHAR ...
#define DEF_WTEMP_INITSIZE  1024        // Initial ...


#ifdef DEBUG_ODS
extern char DebugFile[];
extern HANDLE DebugHandle;
#endif

char *lpszTemp;                         // Used for temporary storage
WCHAR *lpwszTemp;                       // ...

HINSTANCE _hInstance;                   // Global instance handle
HICON hIconMF_Large, hIconMF_Small,     // Icon handles
      hIconSM_Large, hIconSM_Small,
      hIconDB_Large, hIconDB_Small;
HFONT hFontSW;                          // Font ...
HWND hWndTC,                            // Global Tab Control window handle
     hWndMF,                            // ...    Master Frame ...
     hWndMC,                            // ...    MDI Client   ...
     hWndSM,                            // ...    Session Manager ...
     hWndDB,                            // ...    Debugger     ...
     hWndTT;                            // ...    ToolTip      ...
int nMinState,                          // Minimized state as per WinMain
    iScrollSize,                        // Width of a vertical scrollbar
    MFSizeState = SIZE_RESTORED;        // Size state for MF (SIZE_xxx)
HANDLE hAccel;                          // Keyboard accelerators
POINT MFPosCtr;                         // X- and Y- center of Master Frame Window position
SIZE  MFSize,                           // Size of Master Frame Window window rectangle
      MCSize;                           // ...     MDI Client   ...
BOOL fHelp = FALSE,                     // TRUE iff we displayed help
     bCommandLine;                      // ...      there is a filename on the command line
char szAppDPFE  [_MAX_PATH],            // .EXE drive, path, filename.ext
     szHlpDPFE  [_MAX_PATH],            // .HLP ...
     szInitDir  [_MAX_PATH],            // Initial directory for File Open & Save
     szOpenFile [_MAX_PATH];            // Save area for multiple files to open

#ifdef DEBUG
#define APPEND_DEBUG " (DEBUG)"
#else
#define APPEND_DEBUG
#endif

char pszAppName[]       = "NARS2000" APPEND_DEBUG;                  // Application name for MessageBox

char szMFTitle[]        = "NARS2000" APPEND_DEBUG,                  // Master frame window title
     szSMTitle[]        = "NARS2000 Session Manager" APPEND_DEBUG,  // Session Manager ...
     szMCTitle[]        = "NARS2000 MDI Client Window" APPEND_DEBUG,// MDI Client ... (for debugging purposes only)
     szDBTitle[]        = "NARS2000 Debugger Window" APPEND_DEBUG;  // Debugger ...

#define MFWNDCLASS      "MFClass"       // Master Frame Window class
#define MCWNDCLASS      "MDIClient"     // MDI Client   ...
#define SMWNDCLASS      "SMClass"       // Session Manager ...
#define DBWNDCLASS      "DBClass"       // Debugger     ...

char szMFClass[]        = MFWNDCLASS,   // Master Frame Window class
     szMCClass[]        = MCWNDCLASS,   // MDI Client   ...
     szSMClass[]        = SMWNDCLASS,   // Session Manager ...
     szDBClass[]        = DBWNDCLASS;   // Debugger     ...

char pszNoRegMFWndClass[]   = "Unable to register window class <" MFWNDCLASS ">.",
     pszNoRegSMWndClass[]   = "Unable to register window class <" SMWNDCLASS ">.",
     pszNoRegDBWndClass[]   = "Unable to register window class <" DBWNDCLASS ">.";

char pszNoCreateMFWnd[]     = "Unable to create Master Frame window",
     pszNoCreateMCWnd[]     = "Unable to create MDI Client window",
     pszNoCreateSMWnd[]     = "Unable to create Session Manager window",
     pszNoCreateDBWnd[]     = "Unable to create Debugger window",
     pszNoCreateTTWnd[]     = "Unable to create ToolTip window";


//***************************************************************************
//  MF_Create
//
//  Perform window-specific initialization
//***************************************************************************

void MF_Create (HWND hWnd)

{
    // Read the default scrollbar width
    iScrollSize = GetSystemMetrics (SM_CXVSCROLL);
} // End MF_Create


//***************************************************************************
//  MF_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void MF_Delete (HWND hWnd)

{
} // End MF_Delete


//***************************************************************************
//  CreateToolTip
//
//  Creates the ToolTip window and initializes it.
//***************************************************************************

HWND CreateToolTip (HWND hWndParent)

{
    HWND hWnd;

    // Create the ToolTip window
    hWnd =
    CreateWindowEx (0L,             // Extended styles
#if QTT
                    WC_TOOLTIPS,    // Class for Qcontrols
#else
                    TOOLTIPS_CLASS, // Class for MS Controls
#endif
                    NULL,           // Window title
                    TTS_NOANIMATE
                  | TTS_ALWAYSTIP,  // Styles
                    CW_USEDEFAULT,  // X-coord
                    CW_USEDEFAULT,  // Y-...
                    CW_USEDEFAULT,  // Width
                    CW_USEDEFAULT,  // Height
                    NULL,           // Parent window
                    NULL,           // Menu
                    _hInstance,     // Instance
                    NULL);          // No extra data

    if (hWnd EQ NULL)
    {
        MB (pszNoCreateTTWnd);
        return NULL;        // Stop the whole process
    } // End IF

    return hWnd;
} // End CreateToolTip


//***************************************************************************
//  CreateChildWindows
//
//  Create the child windows under the parent window
//***************************************************************************

BOOL CreateChildWindows (HWND hWnd)

{
    RECT rc;                // Rectangle for setting size of window
    CLIENTCREATESTRUCT ccs; // For MDI Client window

    // Get the size and position of the parent window.
    GetClientRect (hWnd, &rc);

    // Create the ToolTip window first so that
    // the other windows can reference it.
    hWndTT = CreateToolTip (hWnd);

    if (hWndTT EQ NULL)
        return FALSE;       // Stop the whole process

    // Create the MDI client window
    MCSize.cx = rc.right;   // Set to full width
    MCSize.cy = rc.bottom;  // ...         height

    // Fill in the CLIENTCREATESTRUCT for the MDI Client
    ccs.hWindowMenu = GetSubMenu (GetMenu (hWnd), IDMPOS_WINDOW);
    ccs.idFirstChild = IDM_CHILDWINDOW;
    hWndMC =
    CreateWindowEx (WS_EX_CLIENTEDGE,       // Extended styles
                    szMCClass,              // Class name
                    szMCTitle,              // Window title (for debugging purposes only)
////                MDIS_ALLCHILDSTYLES,    // Styles
                    WS_CHILD
                  | WS_BORDER
                  | WS_CLIPCHILDREN,        // Styles
                    0,                      // X-coordinate
                    0,                      // Y-...
                    MCSize.cx,              // Width
                    MCSize.cy,              // Height
                    hWnd,                   // Parent
                    (HMENU) 2,              // Child identifier
                    _hInstance,             // hInstance
                    &ccs);                  // lParam
    if (hWndMC EQ NULL)
    {
        MB (pszNoCreateMCWnd);
        return FALSE;       // Stop the whole process
    } // End IF

    // Show and paint the window
    ShowWindow (hWndMC, SW_SHOWNORMAL);
    UpdateWindow (hWndMC);

    // Create the Debugger window first
    //   so it can be used by subsequent windows
    hWndDB =
    CreateMDIWindow (szDBClass,             // Class name
                     szDBTitle,             // Window title
                     0,                     // Styles
                     CW_USEDEFAULT,         // X-pos
                     CW_USEDEFAULT,         // Y-pos
                     CW_USEDEFAULT,         // Height
                     CW_USEDEFAULT,         // Width
                     hWndMC,                // Parent
                     _hInstance,            // Instance
                     0);                    // No extra data
    if (hWndDB EQ NULL)
    {
        MB (pszNoCreateDBWnd);
        return FALSE;       // Stop the whole process
    } // End IF

    // Create the Session Manager window
    hWndSM =
    CreateMDIWindow (szSMClass,             // Class name
                     szSMTitle,             // Window title
                     WS_VSCROLL             // Styles
                   | WS_HSCROLL,
                     CW_USEDEFAULT,         // X-pos
                     CW_USEDEFAULT,         // Y-pos
                     CW_USEDEFAULT,         // Height
                     CW_USEDEFAULT,         // Width
                     hWndMC,                // Parent
                     _hInstance,            // Instance
                     0);                    // No extra data
    if (hWndSM EQ NULL)
    {
        MB (pszNoCreateSMWnd);
        return FALSE;       // Stop the whole process
    } // End IF

    return TRUE;            // Tell 'em it worked
} // End CreateChildWindows


//***************************************************************************
//  RestoreAllEnumProc
//
//  EnumChildWindows callback to restore all MDI Child windows
//
//  lParam = unused
//***************************************************************************

BOOL CALLBACK RestoreAllEnumProc (HWND hWnd, LPARAM lParam)

{
    // When an MDI child window is minimized, Windows creates two windows: an
    // icon and the icon title.  The parent of the icon title window is set to
    // the MDI client window, which confines the icon title to the MDI client
    // area.  The owner of the icon title is set to the MDI child window.
    if (GetWindow (hWnd, GW_OWNER))     // If it's an icon title window, ...
        return TRUE;                    // skip it, and continue enumerating

    // Restore the window in case it's an icon
    SendMessage (hWndMC, WM_MDIRESTORE, (WPARAM) hWnd, 0);

    return TRUE;                        // Continue enumerating
} // End RestoreAllEnumProc


//***************************************************************************
//  MFWndProc
//
//  Message processing routine for the Master Frame window
//***************************************************************************

LRESULT APIENTRY
MFWndProc (HWND hWnd,       // Window handle
           UINT message,    // Type of message
           UINT wParam,     // Additional information
           LONG lParam)     // ...

{
    RECT rcDtop;            // Rectangle for desktop
////static DWORD aHelpIDs[] = {
////                           IDOK,             IDH_OK,
////                           IDCANCEL,         IDH_CANCEL,
////                           IDC_APPLY,        IDH_APPLY,
////                           IDC_HELP2,        IDH_HELP2,
////                           0,             0,
////                          };
////ODSAPI ("MF: ", hWnd, message, wParam, lParam);
    switch (message)
    {
////////case WM_KEYDOWN:
////////    ODSAPI ("MF: ", hWnd, message, wParam, lParam);
////////
////////    break;
////////
        case WM_NCCREATE:       // lpcs = (LPCREATESTRUCT) lParam
            hWndMF = hWnd;

            break;                  // Continue with next handler

        case WM_CREATE:
        {
            // Create the child windows
            if (CreateChildWindows (hWnd) EQ FALSE)
                return -1;          // Stop the whole process

            // Read in the registry entries
            ReadReg (hWnd);

////        // Ensure the position is valid
////        if (MFPosCtr.x > rcDtop.right)  // If center is right of right, ...
////            MFPosCtr.x = rcDtop.right;
////        if (MFPosCtr.y > rcDtop.bottom) // If center is below the bottom, ...
////            MFPosCtr.y = rcDtop.bottom;
////
            // Check the last SizeState
            if (MFSizeState EQ SIZE_MAXIMIZED)
                nMinState = SW_MAXIMIZE;

            // Reposition the window to previous center & size
            MoveWindowPosSize (hWnd, MFPosCtr, MFSize);

            ShowWindow (hWnd, nMinState);   // Show as per request

            // Initialize window-specific resources
            MF_Create (hWnd);

            break;                  // Continue with next handler
        } // WM_CREATE

        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
        {
            HWND hWndMD;

            // Forward this to all child windows
            for (hWndMD = GetWindow (hWndMC, GW_CHILD);
                 hWndMD;
                 hWndMD = GetWindow (hWndMD, GW_HWNDNEXT))
                PostMessage (hWndMD, message, wParam, lParam);

            PostMessage (hWndTT, message, wParam, lParam);

            // Uninitialize window-specific resources
            MF_Delete (hWnd);

            // Initialize window-specific resources
            MF_Create (hWnd);

            break;                  // Continue with next handler
        } // End WM_SYSCOLORCHANGE/WM_SETTINGCHANGE

        case WM_SIZE:                       // uFlags = (UINT) wParam
                                            // cx = LOWORD (lParam)
                                            // cy = HIWORD (lParam)
            if (wParam EQ SIZE_MAXIMIZED
             || wParam EQ SIZE_RESTORED)
            {
                SIZE S;

                // Save the current Maximized or Normal state
                MFSizeState = wParam;

                S.cx = LOWORD (lParam);
                S.cy = HIWORD (lParam);

                // Convert client area size to window size
                MFSize = ClientToWindowSize (hWnd, S);

                // Because we track the center position of the window,
                // we need to modify that as well.  Note that we needn't
                // specify lParam as our MYWM_MOVE code doesn't use it.
                PostMessage (hWnd, MYWM_MOVE, 0, 0);
            } // End IF

            break;                  // Continue with next handler ***MUST***

        case MYWM_MOVE:
            if (!IsIconic (hWnd))   // If we're not minimized, otherwise
            {                       // the positions are of the icon
                GetWindowRect (hWnd, &rcDtop);  // Get current window positions
                MFPosCtr.x = rcDtop.left + MFSize.cx / 2; // Convert to center coords
                MFPosCtr.y = rcDtop.top  + MFSize.cy / 2; // ...
            } // End IF

            return FALSE;           // We handled the msg

        case WM_MOVE:
            PostMessage (hWnd, MYWM_MOVE, 0, 0);

            break;                  // Continue with next handler

////////case WM_HELP:
////////{
////////    LPHELPINFO lpHI;
////////
////////    lpHI = (LPHELPINFO) lParam;
////////
////////    if (lpHI->iContextType EQ HELPINFO_WINDOW)  // Must be for control
////////    {
////////        WinHelp (lpHI->hItemHandle,
////////                 szHlpDPFE,
////////                 HELP_WM_HELP,
////////                 (DWORD) (LPVOID) aHelpIDs);
////////        fHelp = TRUE;
////////    } // End IF
////////
////////    return FALSE;           // We handled the message
////////
////////} // End WM_HELP
////////
////////case WM_CONTEXTMENU:
////////    if (hWndTreeView NE (HWND) wParam)
////////    {
////////        WinHelp ((HWND) wParam,
////////                 szHlpDPFE,
////////                 HELP_CONTEXTMENU,
////////                 (DWORD) (LPVOID) aHelpIDs);
////////        fHelp = TRUE;
////////    } // End IF
////////    return FALSE;           // We handled the message

        case WM_COMMAND:            // wNotifyCode = HIWORD(wParam); // notification code
                                    // wID = LOWORD(wParam);         // item, control, or accelerator identifier
                                    // hwndCtl = (HWND) lParam;      // handle of control
            switch (GET_WM_COMMAND_ID (wParam, lParam))
            {
                case IDM_EXIT:
                    PostMessage (hWnd, WM_CLOSE, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_TILE_HORZ:
                    PostMessage (hWndMC, WM_MDITILE, MDITILE_HORIZONTAL, 0);

                    return FALSE;       // We handled the msg

                case IDM_TILE_VERT:
                    PostMessage (hWndMC, WM_MDITILE, MDITILE_VERTICAL, 0);

                    return FALSE;       // We handled the msg

                case IDM_CASCADE:
                    // In this case, we don't care whether or not there
                    // are any child windows as we're restoring them all anyway.
                    EnumChildWindows (hWndMC, RestoreAllEnumProc, 0);

                    PostMessage (hWndMC, WM_MDICASCADE, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_ARRANGE_ICONS:
                    PostMessage (hWndMC, WM_MDIICONARRANGE, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_SCREENFONT:
                    // Display a Font Dialog so the user can choose
                    //   a new font for the screen
                    MyChooseFont ();

                    return FALSE;       // We handled the msg

                case IDM_HELP_CONTENTS:
                    WinHelp (hWnd, szHlpDPFE, HELP_INDEX, 0);

                    return FALSE;       // We handled the msg

                case IDM_ABOUT:
                    DialogBox (_hInstance,
                               MAKEINTRESOURCE (IDD_ABOUT),
                               hWnd,
                               &AboutDlgProc);
                    return FALSE;       // We handled the msg
            } // End SWITCH

            break;                  // Continue with next handler ***MUST***

        case WM_ERASEBKGND:
            // In order to reduce screen flicker, we handle erase background
            // in the WM_PAINT message for the child windows.
            return TRUE;            // We erased the background

        case WM_QUERYENDSESSION:
        case WM_CLOSE:
        {
            if (fHelp && szHlpDPFE[0])   // If we used help and there's a valid .HLP file, ...
            {
                WinHelp (hWnd, szHlpDPFE, HELP_QUIT, 0); // Quit it
                fHelp = FALSE;
            } // End IF

            // Save environment variables
            SaveEnvironment ();

            // This also tells the child windows to close
            DestroyWindow (hWnd);

            break;                  // Continue with default handler
        } // End WM_CLOSE

        case WM_DESTROY:
            // Uninitialize window-specific resources
            MF_Delete (hWnd);

            // Say goodbye
            PostQuitMessage (0);

            break;                  // Continue with default handler
    } // End SWITCH

////ODSAPI ("MFZ:", hWnd, message, wParam, lParam);
    return DefFrameProc (hWnd, hWndMC, message, wParam, lParam);
} // End MFWndProc


//***************************************************************************
//  InitApplication
//
//  Initializes window data and registers window class
//***************************************************************************

BOOL InitApplication (HANDLE hInstance)     // Current instance

{
    WNDCLASSEX wc = {sizeof (WNDCLASSEX)};

    // Fill in Master Frame window class structure
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC) MFWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = hIconMF_Large;
    wc.hIconSm = hIconMF_Small;
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = MAKEINTRESOURCE (IDR_MENU);
    wc.lpszClassName = szMFClass;

    // Register the Master Frame window class
    if (!RegisterClassEx (&wc))
    {
        MB (pszNoRegMFWndClass);
        return FALSE;
    } // End IF

    // Fill in Session Manager window class structure
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC) SMWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = GWLSM_EXTRA;
    wc.hInstance = hInstance;
    wc.hIcon = hIconSM_Large;
    wc.hIconSm = hIconSM_Small;
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject (WHITE_BRUSH);
    wc.lpszMenuName = MAKEINTRESOURCE (IDR_MENU);
    wc.lpszClassName = szSMClass;

    // Register the Session Manager window class
    if (!RegisterClassEx (&wc))
    {
        MB (pszNoRegSMWndClass);
        return FALSE;
    } // End IF

    // Fill in Debugger window class structure
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC) DBWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = GWLDB_EXTRA;
    wc.hInstance = hInstance;
    wc.hIcon = hIconDB_Large;
    wc.hIconSm = hIconDB_Small;
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject (WHITE_BRUSH);
    wc.lpszMenuName = MAKEINTRESOURCE (IDR_MENU);
    wc.lpszClassName = szDBClass;

    // Register the Debugger window class
    if (!RegisterClassEx (&wc))
    {
        MB (pszNoRegDBWndClass);
        return FALSE;
    } // End IF

    return TRUE;
} // End InitApplication


//***************************************************************************
//  UninitApplication
//
//  Uninitialize application-specific resources
//***************************************************************************

void UninitApplication (HINSTANCE hInstance)

{
} // UninitApplication


//***************************************************************************
//  InitInstance
//
//  Saves instance handle and creates main window
//***************************************************************************

BOOL InitInstance (HANDLE hInstance)

{
    // Save in global variable
    _hInstance = hInstance;

    // Allocate virtual memory for the char temporary storage
    lpszTemp = VirtualAlloc (NULL,          // Any address
                             DEF_CTEMP_MAXSIZE * sizeof (char),
                             MEM_RESERVE,
                             PAGE_READWRITE);
    if (!lpszTemp)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("InitInstance:  VirtualAlloc for <lpszTemp> failed");

        return FALSE;       // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (lpszTemp,
                  DEF_CTEMP_INITSIZE * sizeof (char),
                  MEM_COMMIT,
                  PAGE_READWRITE);

    // Allocate virtual memory for the WCHAR temporary storage
    lpwszTemp = VirtualAlloc (NULL,         // Any address
                              DEF_WTEMP_MAXSIZE * sizeof (WCHAR),
                              MEM_RESERVE,
                              PAGE_READWRITE);
    if (!lpwszTemp)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("InitInstance:  VirtualAlloc for <lpwszTemp> failed");

        return FALSE;       // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (lpwszTemp,
                  DEF_WTEMP_INITSIZE * sizeof (WCHAR),
                  MEM_COMMIT,
                  PAGE_READWRITE);

    // Read in the icons
    hIconMF_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_MF_LARGE));
    hIconMF_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_MF_SMALL));
    hIconSM_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_SM_LARGE));
    hIconSM_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_SM_SMALL));
    hIconDB_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_DB_LARGE));
    hIconDB_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_DB_SMALL));

    // Get keyboard accelerators
    hAccel = LoadAccelerators (hInstance, MAKEINTRESOURCE (IDR_ACCEL));

    return TRUE;
} // End InitInstance


//***************************************************************************
//  UninitInstance
//
//  Uninitialize instance-specific resources
//***************************************************************************

void UninitInstance (HINSTANCE hInstance)

{
    // Free the temporary storage
    if (lpszTemp)
    {
        VirtualFree (lpszTemp, 0, MEM_RELEASE); lpszTemp = NULL;
    } // End IF

    if (lpwszTemp)
    {
        VirtualFree (lpwszTemp, 0, MEM_RELEASE); lpwszTemp = NULL;
    } // End IF

    // No need to destroy either the cursors or icons
} // UninitInstance


//***************************************************************************
//  ParseCommandLine
//
//  Parse any command line
//***************************************************************************

BOOL ParseCommandLine (LPSTR lpCmdLine)

{
    char *p;

    // Skip over leading space
    p = SkipWhite (lpCmdLine);

    if (*p)
    {
        // Copy to buffer
        lstrcpy (szOpenFile, p);

        // Mark as present
        bCommandLine = TRUE;
    } // End IF

    return TRUE;
} // ParseCommandLine


//***************************************************************************
//  WinMain
//
//  Start the process
//***************************************************************************

int PASCAL WinMain (HINSTANCE   hInstance,
                    HINSTANCE   hPrevInstance,  // Zero under Win32
                    LPSTR       lpCmdLine,
                    int         nCmdShow)
{
    MSG  Msg;
    RECT rcClient;

    // Ensure that the common control DLL is loaded.
    InitCommonControls ();

    // Get the DPFE for our files
    GetFileNames (hInstance);

    // Save initial state
    nMinState = nCmdShow;

    // If there's a command line, parse it
    if (!ParseCommandLine (lpCmdLine))
        return -1;                  // Exit

    // Perform initializations that apply to a specific instance
    if (!InitInstance (hInstance))
        goto EXIT2;

    // Register the window class
    if (!InitApplication (hInstance))
        goto EXIT3;

    // Make sure the common controls DLL is loaded
    InitCommonControls ();

    // Initialize ChooseFont argument here
    //   so its settings will be present
    //   for the second and subsequent time
    //   the common dialog is called.
    InitChooseFont ();

    // Create the tab control window
    DbgBrk ();
    hWndTC =
        CreateWindowEx (0L,                             // Extended styles
                        WC_TABCONTROL,                  // Class
                        "",                             // Title
                        WS_CHILD
                      | WS_CLIPSIBLINGS
                      | WS_VISIBLE,                     // Styles
                        CW_USEDEFAULT, CW_USEDEFAULT,   // X- and Y-coord
                        CW_USEDEFAULT, CW_USEDEFAULT,   // X- and Y-size
                        NULL,                           // Parent window
                        NULL,                           // Menu
                        _hInstance,                     // Instance
                        NULL);                          // No extra data

    GetClientRect (hWndTC, &rcClient);

    // Create the Master Frame window
    hWndMF =
        CreateWindowEx (0L,                             // Extended styles
                        szMFClass,                      // Class
                        szMFTitle,                      // Title
                        WS_OVERLAPPEDWINDOW,            // Style
                        0, 0,                           // X- and Y-coord
                        rcClient.right,                 // X-size
                        rcClient.bottom,                // Y-size
                        hWndTC,                         // Parent window
                        NULL,                           // Menu
                        _hInstance,                     // Instance
                        NULL);                          // No extra data
    if (hWndMF EQ NULL)
    {
        MB (pszNoCreateMFWnd);
        return FALSE;
    } // End IF

    // Make the window visible; update its client area
    ShowWindow (hWndMF, nCmdShow);
    UpdateWindow (hWndMF);

    // Main message loop
    while (GetMessage(&Msg, NULL, 0, 0))
    {
        // Handle MDI messages and accelerators
        if (!TranslateMDISysAccel (hWndMC, &Msg)
         && ((!hAccel) || !TranslateAccelerator (hWndMF, hAccel, &Msg)))
        {
            TranslateMessage (&Msg);
            DispatchMessage (&Msg);
        } // End IF
    } // End WHILE
    // GetMessage returned FALSE for a Quit message
EXIT3:
    UninitApplication (hInstance);
EXIT2:
    UninitInstance (hInstance);

    return 0;
} // End WinMain


//***************************************************************************
//  End of File: main.c
//***************************************************************************
