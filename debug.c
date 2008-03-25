//***************************************************************************
//	NARS2000 -- My Routines For Debugging
//***************************************************************************

/***************************************************************************
	NARS2000 -- An Experimental APL Interpreter
	Copyright (C) 2006-2008 Sudley Place Software

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#define WINVER		 0x0501 // Needed for WINUSER.H definitions
#define _WIN32_WINNT 0x0501 // ...

#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

#include "main.h"
#include "resource.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"
#include "threads.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


CDB_THREAD cdbThread;			// Temporary global

#ifdef DEBUG
//***************************************************************************
//	$Assert
//***************************************************************************

void Assert
	(BOOL bAssert)

{
	if (!bAssert)
		DbgBrk ();
} // End Assert
#endif


#ifdef DEBUG
//***************************************************************************
//	$CreateDebuggerWindow
//
//	Create a debugger window
//***************************************************************************

void CreateDebuggerWindow
	(HGLOBAL hGlbPTD)		// PerTabData global memory handle

{
	DWORD dwThreadId;		// Thread ID

	// Pass parameters to thread
	cdbThread.hGlbPTD = hGlbPTD;

	// Create the thread
	CreateThread (NULL, 						// No security attrs
				  0,							// Use default stack size
				 &CreateDebuggerInThread,		// Starting routine
				 &cdbThread,					// Param to thread func
				  0,							// Creation flag
				 &dwThreadId);					// Returns thread id
} // End CreateDebuggerWindow
#endif


#ifdef DEBUG
//***************************************************************************
//	$CreateDebuggerInThread
//
//	Create debugger window in thread
//***************************************************************************

BOOL WINAPI CreateDebuggerInThread
	(LPCDB_THREAD lpcdbThread)

{
	HGLOBAL 	 hGlbPTD;		// PerTabData global memory handle
	LPPERTABDATA lpMemPTD;		// Ptr to PerTabData global memory
	HWND		 hWndMC,		// MDI Client window handle
				 hWndDB;		// Debugger window handle
	MSG 		 Msg;			// Message for GetMessage loop

#ifndef DEBUG
	__try
	{
#endif
		// Save the thread type ('DB')
		TlsSetValue (dwTlsType, (LPVOID) 'DB');

		// Extract values from the arg struc
		hGlbPTD = lpcdbThread->hGlbPTD;

		// Save the thread's PerTabData global memory handle
		TlsSetValue (dwTlsPerTabData, (LPVOID) hGlbPTD);

		// Lock the memory to get a ptr to it
		lpMemPTD = MyGlobalLock (hGlbPTD);

		// Get the MDI Client window handle
		hWndMC = lpMemPTD->hWndMC;

		// Create the debugger window
		hWndDB =
		lpMemPTD->hWndDB =
		CreateMDIWindow (szDBClass, 			// Class name
						 szDBTitle, 			// Window title
						 0
					   | WS_CLIPCHILDREN,		// Styles
						 CW_USEDEFAULT, 		// X-pos
						 CW_USEDEFAULT, 		// Y-pos
						 CW_USEDEFAULT, 		// Height
						 CW_USEDEFAULT, 		// Width
						 hWndMC,				// Parent
						 _hInstance,			// Instance
						 0);					// Extra data
		// We no longer need this ptr
		MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

		// If it didn't succeed, ...
		if (hWndDB EQ NULL)
			MB (pszNoCreateDBWnd);
		else
		{
			// Show and paint the window
			ShowWindow (hWndDB, SW_SHOWNORMAL);
			UpdateWindow (hWndDB);

			// Make sure we can communicate between windows
			AttachThreadInput (GetCurrentThreadId (), dwMainThreadId, TRUE);

			// Lock the memory to get a ptr to it
			lpMemPTD = MyGlobalLock (hGlbPTD);

			// Tell the SM we're active
			PostMessage (lpMemPTD->hWndSM, WM_PARENTNOTIFY, MAKEWPARAM (WM_CREATE, IDWC_SM_DB), (LPARAM) hWndDB);

			// We no longer need this ptr
			MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

			// Main message loop
			while (GetMessage (&Msg, NULL, 0, 0))
			{
				// Handle MDI messages and accelerators
				if (!TranslateMDISysAccel (hWndMC, &Msg)
				 && ((!hAccel) || !TranslateAccelerator (hWndMF, hAccel, &Msg)))
				{
					TranslateMessage (&Msg);
					DispatchMessage  (&Msg);
				} // End IF
			} // End WHILE

			// GetMessage returned FALSE for a Quit message
		} // End IF
#ifndef DEBUG
	} __except (CheckException (GetExceptionInformation (), "CreateDebuggerInThread"))
	{
		// Display message for unhandled exception
		DisplayException ();
	} // End __try/__except
#endif
	return 0;
} // End CreateDebuggerInThread
#endif


#ifdef DEBUG
//***************************************************************************
//	$DB_Create
//
//	Perform window-specific initialization
//***************************************************************************

void DB_Create
	(HWND hWnd)

{
} // End DB_Create
#endif


#ifdef DEBUG
//***************************************************************************
//	$DB_Delete
//
//	Perform window-specific uninitialization
//***************************************************************************

void DB_Delete
	(HWND hWnd)

{
} // End DB_Delete
#endif


#ifdef DEBUG
//***************************************************************************
//	$DBWndProc
//
//	Message processing routine for the Debugger window
//***************************************************************************

LRESULT APIENTRY DBWndProc
	(HWND hWnd, 	// Window handle
	 UINT message,	// Type of message
	 UINT wParam,	// Additional information
	 LONG lParam)	// ...

{
	char		 szTemp[1204];
	WCHAR		 wszTemp[1024];
	int 		 iLineNum,
				 iIndex,
				 iHeight;
	RECT		 rcClient;
	HWND		 hWndLB;
	HGLOBAL 	 hGlbPTD;		// PerTabData global memory handle
	LPPERTABDATA lpMemPTD;		// Ptr to PerTabData global memory

#define PROP_LINENUM	"iLineNum"

////LCLODSAPI ("DB: ", hWnd, message, wParam, lParam);

	// Get the window handle of the Listbox
	(long) hWndLB = GetWindowLong (hWnd, GWLDB_HWNDLB);

	switch (message)
	{
		case WM_CREATE:
		{
			iLineNum = 0;
			SetProp (hWnd, PROP_LINENUM, (HANDLE) iLineNum);

			// Create a listbox to fit inside this window
			hWndLB =
			CreateWindow ("LISTBOX",
						  "Debugger Listbox",   // For debugging only
						  0
						| WS_CHILD
						| WS_VSCROLL
						| WS_HSCROLL
						| WS_CLIPCHILDREN
						| LBS_NOINTEGRALHEIGHT
						| LBS_EXTENDEDSEL
						| LBS_MULTIPLESEL
						  , 					// Styles
						  0,					// X-position
						  0,					// Y-...
						  CW_USEDEFAULT,		// Width
						  CW_USEDEFAULT,		// Height
						  hWnd, 				// Parent window
						  (HMENU) IDWC_DB_LB,	// ID
						  _hInstance,			// Instance
						  0);					// lParam
			// Save for later use
			SetWindowLong (hWnd, GWLDB_HWNDLB, (long) hWndLB);

			// Show the windows
			ShowWindow (hWndLB, SW_SHOWNORMAL);
			ShowWindow (hWnd,	SW_SHOWNORMAL);

			// Get the thread's PerTabData global memory handle
			hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

			// Lock the memory to get a ptr to it
			lpMemPTD = MyGlobalLock (hGlbPTD);

			// Subclass the List Box so we can pass
			//	 certain WM_KEYDOWN messages to the
			//	 session manager.
			lpMemPTD->lpfnOldListboxWndProc = (WNDPROC)
			  SetWindowLongW (hWndLB,
							  GWL_WNDPROC,
							  (long) (WNDPROC) &LclListboxWndProc);
			// We no longer need this ptr
			MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

			// Initialize window-specific resources
			DB_Create (hWnd);

			// Use Post here as we need to wait for the LB window
			//	 to be drawn.
			PostMessage (hWnd, MYWM_INIT_DB, 0, 0);

			return FALSE;			// We handled the msg
		} // End WM_CREATE

		case MYWM_INIT_DB:
			// Tell the Listbox Control about its font
			SendMessageW (hWndLB, WM_SETFONT, (WPARAM) hFontSM, MAKELPARAM (TRUE, 0));

			return FALSE;			// We handled the msg

		case WM_SETFONT:			// hFont = (HFONT) wParam;
									// fRedraw = LOWORD (lParam);
		case WM_MOUSEWHEEL:
			// Pass these messages through to the ListBox
			SendMessageW (hWndLB, message, wParam, lParam);

			return FALSE;			// We handled the msg

		case WM_SYSCOLORCHANGE:
		case WM_SETTINGCHANGE:
			// Uninitialize window-specific resources
			DB_Delete (hWnd);

			// Initialize window-specific resources
			DB_Create (hWnd);

			break;					// Continue with next handler

#define fwSizeType	wParam
#define nWidth		(LOWORD(lParam))
#define nHeight 	(HIWORD(lParam))
		case WM_SIZE:				// fwSizeType = wParam; 	 // Resizing flag
									// nWidth = LOWORD(lParam);  // Width of client area
									// nHeight = HIWORD(lParam); // Height of client area
			if (fwSizeType NE SIZE_MINIMIZED)
				SetWindowPos (hWndLB,			// Window handle to position
							  0,				// SWP_NOZORDER
							  0,				// X-position
							  0,				// Y-...
							  nWidth,			// Width
							  nHeight,			// Height
							  SWP_NOZORDER		// Flags
							| SWP_SHOWWINDOW
							 );
			break;					// Continue with next handler ***MUST***
#undef	nHeight
#undef	nWidth
#undef	fwSizeType

		case MYWM_DBGMSGA:			// Single-char message
			iLineNum = (int) GetProp (hWnd, PROP_LINENUM);

			// Format the string with a preceding line #
			wsprintfA (szTemp,
					   "%4d:  %s",
					   ++iLineNum,
					   (LPCHAR) lParam);
			// Convert the string from A to W
			A2W (wszTemp, szTemp, sizeof (wszTemp) - 1);

			SetProp (hWnd, PROP_LINENUM, (HANDLE) iLineNum);

			// Add the string to the ListBox
			iIndex = SendMessageW (hWndLB, LB_ADDSTRING, 0, (LPARAM) wszTemp);

			// Scroll this item into view
			SendMessageW (hWnd, MYWM_DBGMSG_SCROLL, iIndex, 0);

			return FALSE;			// We handled the msg

		case MYWM_DBGMSGW:			// Double-char message
			iLineNum = (int) GetProp (hWnd, PROP_LINENUM);

			// Format the string with a preceding line #
			wsprintfW (wszTemp,
					   L"%4d:  %s",
					   ++iLineNum,
					   (LPWCHAR) lParam);

			SetProp (hWnd, PROP_LINENUM, (HANDLE) iLineNum);

			// Add the string to the ListBox
			iIndex = SendMessageW (hWndLB, LB_ADDSTRING, 0, (LPARAM) wszTemp);

			// Scroll this item into view
			SendMessageW (hWnd, MYWM_DBGMSG_SCROLL, iIndex, 0);

			return FALSE;			// We handled the msg

		case MYWM_DBGMSG_SCROLL:	// Scroll item into view
			iIndex = (int) wParam;	// Get the item #

			// Check for last line #
			if (iIndex EQ -1)
				iIndex = SendMessageW (hWndLB, LB_GETCOUNT, 0, 0);

			// Ensure the item just added is visible
			GetClientRect (hWndLB, &rcClient);

			// Common item height
			iHeight = SendMessageW (hWndLB, LB_GETITEMHEIGHT, 0, 0);

			// Less # whole items we can display
			iIndex -= (rcClient.bottom / iHeight) - 1;

			SendMessageW (hWndLB, LB_SETTOPINDEX, max (0, iIndex), 0);

			UpdateWindow (hWnd);	// Redraw the screen now

			return FALSE;			// We handled the msg

		case MYWM_DBGMSG_CLR:
			// Start over again
			SendMessageW (hWndLB, LB_RESETCONTENT, 0, 0);
			iLineNum = 0;
			SetProp (hWnd, PROP_LINENUM, (HANDLE) iLineNum);

			UpdateWindow (hWnd);	// Redraw the screen now

			return FALSE;			// We handled the msg

		case WM_CLOSE:
			DestroyWindow (hWnd);

			return FALSE;			// We handled the msg

		case MYWM_UNHOOK:
			// Get the thread's PerTabData global memory handle
			hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

			// In case hGlbPTD has already been freed, ...
			if (IsGlbPtr (hGlbPTD))
			{
				// Lock the memory to get a ptr to it
				lpMemPTD = MyGlobalLock (hGlbPTD);

				// Unhook the LclListboxWndProc
				SetWindowLongW (hWndLB,
								GWL_WNDPROC,
								(long) lpMemPTD->lpfnOldListboxWndProc);
				// We no longer need this ptr
				MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
			} // End IF

			return FALSE;			// We handled the msg

		case WM_DESTROY:
			// Unhook out hooks
			SendMessage (hWnd, MYWM_UNHOOK, 0, 0);

			// Remove all saved window properties
			EnumProps (hWnd, EnumCallbackRemoveProp);

			// Uninitialize window-specific resources
			DB_Delete (hWnd);

			// Tell the thread to quit, too
			PostQuitMessage (0);

			return FALSE;			// We handled the msg
	} // End SWITCH

////LCLODSAPI ("DBZ:", hWnd, message, wParam, lParam);
	return DefMDIChildProc (hWnd, message, wParam, lParam);
} // End DBWndProc
#endif


#ifdef DEBUG
//***************************************************************************
//	$LclListboxWndProc
//
//	Local window procedure for the debugger's LISTBOX
//	  to pass on certain WM_KEYDOWN messages to the session manager.
//***************************************************************************

LRESULT WINAPI LclListboxWndProc
	(HWND	hWnd,
	 UINT	message,
	 WPARAM wParam,
	 LPARAM lParam)

{
	HMENU		 hMenu;
	int 		 iSelCnt,
				 mfState,
				 i,
				 iTotalBytes;
	POINT		 ptScr;
	HGLOBAL 	 hGlbInd,
				 hGlbSel;
	LPINT		 lpInd;
	LPWCHAR 	 lpSel,
				 p;
	LRESULT 	 lResult;
	HGLOBAL 	 hGlbPTD;		// PerTabData global memory handle
	LPPERTABDATA lpMemPTD;		// Ptr to PerTabData global memory
	WNDPROC 	 lpfnOldListboxWndProc;

	// Get the thread's PerTabData global memory handle
	hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

	// Lock the memory to get a ptr to it
	lpMemPTD = MyGlobalLock (hGlbPTD);

	lpfnOldListboxWndProc = lpMemPTD->lpfnOldListboxWndProc;

	// We no longer need this ptr
	MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

	// Split cases
	switch (message)
	{
		case WM_COMMAND:
			switch (GET_WM_COMMAND_ID (wParam, lParam))
			{
				case IDM_COPY:
					// Get the # selected items
					iSelCnt = SendMessageW (hWnd, LB_GETSELCOUNT, 0, 0);

					// Allocate space for that many indices
					hGlbInd = GlobalAlloc (GHND, iSelCnt * sizeof (int));

					// Lock the memory to get a ptr to it
					lpInd = GlobalLock (hGlbInd);

					// Populate the array
					SendMessageW (hWnd, LB_GETSELITEMS, iSelCnt, (LPARAM) lpInd);

					// Loop through the selected items and calculate
					//	 the storage requirement for the collection
					for (iTotalBytes = i = 0; i < iSelCnt; i++)
						// The "2 +" is for the '\r' and '\n' at the end of each line
						iTotalBytes += sizeof (WCHAR) * (2 + SendMessageW (hWnd, LB_GETTEXTLEN, lpInd[i], 0));

					// Allocate storage for the entire collection
					hGlbSel = GlobalAlloc (GHND | GMEM_DDESHARE, iTotalBytes);

					// Lock the memory to get a ptr to it
					lpSel = GlobalLock (hGlbSel);

					// Copy the text to the array, separated by a newline
					for (p = lpSel, i = 0; i < iSelCnt; i++)
					{
						p += SendMessageW (hWnd, LB_GETTEXT, lpInd[i], (LPARAM) p);
						*p++ = '\r';
						*p++ = '\n';
					} // End FOR

					// We no longer need this ptr
					GlobalUnlock (hGlbSel); lpSel = NULL;

					// We no longer need this ptr
					GlobalUnlock (hGlbInd); lpInd = NULL;

					// Free the memory
					GlobalFree (hGlbInd); hGlbInd = NULL;

					// Prepare to put the data onto the clipboard
					OpenClipboard (hWnd);
					EmptyClipboard ();

					// Put the data onto the clipboard
					SetClipboardData (CF_UNICODETEXT,
									  hGlbSel);

					// We're finished with the clipboard
					CloseClipboard ();

////////////////////// Free the memory -- NO, the system now owns hGlbSel
////////////////////GlobalFree (hGlbSel); hGlbSel = NULL;

					break;

				case IDM_CUT:
					DbgBrk ();





					break;

				case IDM_DELETE:
					DbgClr ();

					break;

				case IDM_SELECTALL:
					// Select all items
					if (LB_ERR EQ SendMessageW (hWnd,
											   LB_SELITEMRANGE,
											   TRUE,
											   MAKELPARAM (0, -1)))
						DbgStop (); 	// We should never get here
					break;
			} // End SWITCH

			break;

		case WM_RBUTTONDOWN:				// fwKeys = wParam; 	   // key flags
											// xPos = LOWORD(lParam);  // horizontal position of cursor
											// yPos = HIWORD(lParam);  // vertical position of cursor

			// Ensure there are items selected
			iSelCnt = SendMessageW (hWnd, LB_GETSELCOUNT, 0, 0);

			mfState = (iSelCnt EQ 0) ? MF_GRAYED : MF_ENABLED;

			// Get the mouse position in screen coordinates
			GetCursorPos (&ptScr);

			// Create a popup menu
			hMenu = CreatePopupMenu ();

			AppendMenu (hMenu,					// Handle
						mfState
					  | MF_STRING,				// Flags
						IDM_COPY,
						"&Copy");
			AppendMenu (hMenu,					// Handle
						mfState
					  | MF_STRING,				// Flags
						IDM_CUT,
						"C&ut");
			AppendMenu (hMenu,					// Handle
						MF_ENABLED
					  | MF_STRING,				// Flags
						IDM_DELETE,
						"&Clear All");
			AppendMenu (hMenu,					// Handle
						MF_ENABLED
					  | MF_STRING,				// Flags
						IDM_SELECTALL,
						"Select &All");

			TrackPopupMenu (hMenu,				// Handle
							TPM_CENTERALIGN
						  | TPM_LEFTBUTTON
						  | TPM_RIGHTBUTTON,	// Flags
							ptScr.x,	// x-position
							ptScr.y,	// y-position
							0,			// Reserved (must be zero)
							hWnd,		// Handle of owner window
							NULL);		// Dismissal area outside rectangle (none)

			// Free the menu resources
			DestroyMenu (hMenu);

			break;

		case WM_KEYDOWN:			// nVirtKey = (int) wParam; 	// virtual-key code
									// lKeyData = lParam;			// Key data
#define nVirtKey ((int) wParam)
		{
			// Process the virtual key
			switch (nVirtKey)
			{
				case VK_F1:
				case VK_F2:
				case VK_F3:
				case VK_F4:
				case VK_F5:
				case VK_F6:
				case VK_F7:
				case VK_F8:
				case VK_F9:
				case VK_F10:
				case VK_F11:
				case VK_F12:
					// Get the thread's PerTabData global memory handle
					hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

					// Lock the memory to get a ptr to it
					lpMemPTD = MyGlobalLock (hGlbPTD);

					PostMessage (lpMemPTD->hWndSM, message, wParam, lParam);

					// We no longer need this ptr
					MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

					return FALSE;	// We handled the msg
			} // End SWITCH

			break;
		} // End WM_KEYDOWN
#undef	nVirtKey

		case WM_CLOSE:
		case WM_DESTROY:
			// Get the thread's PerTabData global memory handle
			hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

			// Lock the memory to get a ptr to it
			lpMemPTD = MyGlobalLock (hGlbPTD);

			// Unhook the LclListboxWndProc
			SetWindowLongW (hWnd,
							GWL_WNDPROC,
							(long) lpMemPTD->lpfnOldListboxWndProc);
			// We no longer need this ptr
			MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

			break;
	} // End SWITCH

	lResult = CallWindowProcW (lpfnOldListboxWndProc,
							   hWnd,
							   message,
							   wParam,
							   lParam); // Pass on down the line
	return lResult;
} // End LclListboxWndProc
#endif


#ifdef DEBUG
//***************************************************************************
//	$DbgMsg
//
//	Display a SBCS debug message
//***************************************************************************

void DbgMsg
	(LPCHAR szTemp)

{
	WCHAR		 wszTemp[1024];
	HGLOBAL 	 hGlbPTD;		// PerTabData global memory handle
	LPPERTABDATA lpMemPTD;		// Ptr to PerTabData global memory

	// Get the thread's PerTabData global memory handle
	hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

	// Lock the memory to get a ptr to it
	lpMemPTD = MyGlobalLock (hGlbPTD);

	if (lpMemPTD->hWndDB)
	{
		// Convert the string from A to W
		A2W (wszTemp, szTemp, sizeof (wszTemp) - 1);

		DbgMsgW (wszTemp);
	} // End IF

	// We no longer need this ptr
	MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End DbgMsg
#endif


#ifdef DEBUG
//***************************************************************************
//	$DbgMsgW
//
//	Display a DBCS debug message
//***************************************************************************

void DbgMsgW
	(LPWCHAR wszTemp)

{
	HGLOBAL 	 hGlbPTD;		// PerTabData global memory handle
	LPPERTABDATA lpMemPTD;		// Ptr to PerTabData global memory

	// Get the thread's PerTabData global memory handle
	hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

	// Lock the memory to get a ptr to it
	lpMemPTD = MyGlobalLock (hGlbPTD);

	if (lpMemPTD->hWndDB)
		SendMessageW (lpMemPTD->hWndDB, MYWM_DBGMSGW, 0, (LPARAM) wszTemp);

	// We no longer need this ptr
	MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End DbgMsgW
#endif


#ifdef DEBUG
//***************************************************************************
//	$DbgClear
//
//	Clear the debug window
//***************************************************************************

void DbgClr
	(void)

{
	HGLOBAL 	 hGlbPTD;		// PerTabData global memory handle
	LPPERTABDATA lpMemPTD;		// Ptr to PerTabData global memory

	// Get the thread's PerTabData global memory handle
	hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

	// Lock the memory to get a ptr to it
	lpMemPTD = MyGlobalLock (hGlbPTD);

	if (lpMemPTD->hWndDB)
		PostMessage (lpMemPTD->hWndDB, MYWM_DBGMSG_CLR, 0, 0);

	// We no longer need this ptr
	MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End DbgClr
#endif


#ifdef DEBUG
//***************************************************************************
//	$dprintf
//
//	Display a debug message a la printf
//***************************************************************************

int dprintf
	(LPCHAR lpszFmt,
	 ...)

{
	va_list vl;
	int 	i1, i2, i3, i4, i5, i6, i7, i8, iRet;
	char	szTemp[1024];

	// We hope that no one calls us with more than
	//	 eight arguments.
	// Note we must grab them separately this way
	//	 as using va_arg in the argument list to
	//	 wsprintf pushes the arguments in reverse
	//	 order.
	va_start (vl, lpszFmt);

	i1 = va_arg (vl, int);
	i2 = va_arg (vl, int);
	i3 = va_arg (vl, int);
	i4 = va_arg (vl, int);
	i5 = va_arg (vl, int);
	i6 = va_arg (vl, int);
	i7 = va_arg (vl, int);
	i8 = va_arg (vl, int);

	va_end (vl);

	iRet = sprintf (szTemp,
					lpszFmt,
					i1, i2, i3, i4, i5, i6, i7, i8);
	DbgMsg (szTemp);

	return iRet;
} // End dprintf
#endif


#ifdef DEBUG
//***************************************************************************
//	$dprintfW
//
//	Display a debug message a la printfW
//***************************************************************************

int dprintfW
	(LPWCHAR lpwszFmt,
	 ...)

{
	va_list vl;
	int 	i1, i2, i3, i4, i5, i6, i7, i8, iRet;
	WCHAR	wszTemp[1024];

	// We hope that no one calls us with more than
	//	 eight arguments.
	// Note we must grab them separately this way
	//	 as using va_arg in the argument list to
	//	 wsprintf pushes the arguments in reverse
	//	 order.
	va_start (vl, lpwszFmt);

	i1 = va_arg (vl, int);
	i2 = va_arg (vl, int);
	i3 = va_arg (vl, int);
	i4 = va_arg (vl, int);
	i5 = va_arg (vl, int);
	i6 = va_arg (vl, int);
	i7 = va_arg (vl, int);
	i8 = va_arg (vl, int);

	va_end (vl);

	iRet = wsprintfW (wszTemp,
					  lpwszFmt,
					  i1, i2, i3, i4, i5, i6, i7, i8);
	DbgMsgW (wszTemp);

	return iRet;
} // End dprintfW
#endif


#ifdef DEBUG
//***************************************************************************
//	$DbgGlobalAllocSub
//
//	Debug version of MyGlobalAlloc
//***************************************************************************

HGLOBAL DbgGlobalAllocSub
	(UINT	 uFlags,
	 UINT	 ByteRes,
	 LPWCHAR lpFmtStr,
	 LPSTR	 lpFileName,
	 UINT	 uLineNum)

{
	HGLOBAL hGlbRes;

	hGlbRes = MyGlobalAlloc (uFlags, ByteRes);

	if (hGlbRes)
	{
#ifdef DEBUG
		dprintfW (lpFmtStr, hGlbRes, lpFileName, uLineNum);
#endif
	} // End IF

	return hGlbRes;
} // End DbgGlobalAllocSub
#endif


#ifdef DEBUG
//***************************************************************************
//	$FileNameOnly
//
//	Extract the filename only from __FILE__
//***************************************************************************

LPCHAR FileNameOnly
	(LPCHAR lpFileName)

{
	LPCHAR p, q;

	p = lpFileName;

	for (p = lpFileName;
		 q = strchr (p, '\\');
		 p = q + 1)
		;
	return p;
} // End FileNameOnly
#endif


//***************************************************************************
//	End of File: debug.c
//***************************************************************************
