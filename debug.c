//***************************************************************************
//	NARS2000 -- My Routines For Debugging
//***************************************************************************

#define WINVER		 0x0501 // Needed for WINUSER.H definitions
#define _WIN32_WINNT 0x0501 // ...

#pragma pack (1)
#define STRICT
#include <windows.h>
#include <windowsx.h>

#include "main.h"
#include "resource.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//	DbgBrk
//***************************************************************************

//// void __declspec (naked) __cdecl DbgBrk
////	(void)
//// {
////	 _asm {  int	 1			 };
////	 _asm {  ret			 };
//// } // End DbgBrk

//// void DbgBrk (void)
////
//// {
////	 _asm int 1;
//// } // End DbgBrk


#ifdef DEBUG
//***************************************************************************
//	Assert
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
//	DB_Create
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
//	DB_Delete
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
//	DBWndProc
//
//	Message processing routine for the Debugger window
//***************************************************************************

LRESULT APIENTRY DBWndProc
	(HWND hWnd, 	// Window handle
	 UINT message,	// Type of message
	 UINT wParam,	// Additional information
	 LONG lParam)	// ...

{
	char	   szTemp[1204];
	WCHAR	   wszTemp[1024];
	int 	   iLineNum,
			   iIndex,
			   iHeight;
	RECT	   rcClient;

#define PROP_LINENUM	"iLineNum"

////ODSAPI ("DB: ", hWnd, message, wParam, lParam);
	switch (message)
	{
		case WM_NCCREATE:			// lpcs = (LPCREATESTRUCT) lParam
			hWndDB = hWnd;

			break;					// Continue with next handler

		case WM_CREATE:
			iLineNum = 0;
			SetProp (hWnd, PROP_LINENUM, (HANDLE) iLineNum);

			// Create a listbox to fit inside this window
			hWndLB =
			CreateWindow ("LISTBOX",
						  "Debugger Listbox",   // For debugging only
						  0
						| WS_CHILD
						| WS_VSCROLL
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
			// Show the windows
			ShowWindow (hWndLB, SW_SHOWNORMAL);
			ShowWindow (hWnd,	SW_SHOWNORMAL);

			// Subclass the LISTBOX so we can pass
			//	 certain WM_KEYDOWN messages to the
			//	 session manager.
			lpfnOldListboxWndProc = (WNDPROC)
			  SetWindowLongW (hWndLB,
							  GWL_WNDPROC,
							  (long) (WNDPROC) &LclListboxWndProc);
			// Initialize window-specific resources
			DB_Create (hWnd);

			return FALSE;			// We handled the msg

		case WM_SETFONT:
			SendMessage (hWndLB, message, wParam, lParam);

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
		case WM_SIZE:				// fwSizeType = wParam; 	 // resizing flag
									// nWidth = LOWORD(lParam);  // width of client area
									// nHeight = HIWORD(lParam); // height of client area
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

		case WM_USER + 0:			// Single-char message
			iLineNum = (int) GetProp (hWnd, PROP_LINENUM);

			// Format the string with a preceding line #
			wsprintfA (szTemp,
					   "%4d:  %s",
					   ++iLineNum,
					   (LPCHAR) lParam);
			A2W (wszTemp, szTemp);	// Convert the string from A to W

			SetProp (hWnd, PROP_LINENUM, (HANDLE) iLineNum);

			// Call common code
			SendMessageW (hWnd, WM_USER + 2, 0, (LPARAM) wszTemp);

			return FALSE;			// We handled the msg

		case WM_USER + 1:			// Double-char message
			iLineNum = (int) GetProp (hWnd, PROP_LINENUM);

			// Format the string with a preceding line #
			wsprintfW (wszTemp,
					   L"%4d:  %s",
					   ++iLineNum,
					   (LPWCHAR) lParam);
			SetProp (hWnd, PROP_LINENUM, (HANDLE) iLineNum);

			// Call common code
			SendMessageW (hWnd, WM_USER + 2, 0, (LPARAM) wszTemp);

			return FALSE;			// We handled the msg

		case WM_USER + 2:			// Common code
			iIndex = SendMessageW (hWndLB, LB_ADDSTRING, 0, lParam);

			// Ensure the item just added is visible
			GetClientRect (hWndLB, &rcClient);

			// Common item height
			iHeight = SendMessage (hWndLB, LB_GETITEMHEIGHT, 0, 0);

			// Less # whole items we can display
			iIndex -= (rcClient.bottom / iHeight) - 1;

			SendMessage (hWndLB, LB_SETTOPINDEX, max (0, iIndex), 0);

			UpdateWindow (hWnd);	// Redraw the screen now

			return FALSE;			// We handled the msg

		case WM_USER + 10:
			// Start over again
			SendMessage (hWndLB, LB_RESETCONTENT, 0, 0);
////////////iLineNum = (int) GetProp (PROP_LINENUM);
			iLineNum = 0;
			SetProp (hWnd, PROP_LINENUM, (HANDLE) iLineNum);

			UpdateWindow (hWnd);	// Redraw the screen now

			return FALSE;			// We handled the msg

		case WM_CLOSE:
			DestroyWindow (hWnd);

			return FALSE;			// We handled the msg

		case WM_DESTROY:
			// Uninitialize window-specific resources
			DB_Delete (hWnd);

			return FALSE;			// We handled the msg
	} // End SWITCH

////ODSAPI ("DBZ:", hWnd, message, wParam, lParam);
	return DefMDIChildProc (hWnd, message, wParam, lParam);
} // End DBWndProc
#endif


#ifdef DEBUG
//***************************************************************************
//	LclListboxWndProc
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
	HMENU hMenu;
	int   iSelCnt,
		  mfState,
		  i,
		  iTotalBytes;
	POINT ptScr;
	HGLOBAL hGlbInd,
			hGlbSel;
	LPINT lpInd;
	LPWCHAR lpSel, p;

	// Split cases
	switch (message)
	{
		case WM_COMMAND:
			switch (GET_WM_COMMAND_ID (wParam, lParam))
			{
				case IDM_COPY:
					// Get the # selected items
////////////////////iSelCnt = SendMessage (hWndLB, LB_GETSELCOUNT, 0, 0);
					iSelCnt = SendMessage (hWnd, LB_GETSELCOUNT, 0, 0);

					// Allocate space for that many indices
					hGlbInd = GlobalAlloc (GHND, iSelCnt * sizeof (int));

					// Lock the memory to get a ptr to it
					lpInd = GlobalLock (hGlbInd);

					// Populate the array
////////////////////SendMessage (hWndLB, LB_GETSELITEMS, iSelCnt, (LPARAM) lpInd);
					SendMessage (hWnd, LB_GETSELITEMS, iSelCnt, (LPARAM) lpInd);

					// Loop through the selected items and calculate
					//	 the storage requirement for the collection
					for (iTotalBytes = i = 0; i < iSelCnt; i++)
						// The "2 +" is for the '\r' and '\n' at the end of each line
////////////////////////iTotalBytes += sizeof (WCHAR) * (2 + SendMessage (hWndLB, LB_GETTEXTLEN, lpInd[i], 0));
						iTotalBytes += sizeof (WCHAR) * (2 + SendMessage (hWnd, LB_GETTEXTLEN, lpInd[i], 0));

					// Allocate storage for the entire collection
					hGlbSel = GlobalAlloc (GHND | GMEM_DDESHARE, iTotalBytes);

					// Lock the memory to get a ptr to it
					lpSel = GlobalLock (hGlbSel);

					// Copy the text to the array, separated by a newline
					for (p = lpSel, i = 0; i < iSelCnt; i++)
					{
////////////////////////p += SendMessageW (hWndLB, LB_GETTEXT, lpInd[i], (LPARAM) p);
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
////////////////////if (LB_ERR EQ SendMessage (hWndLB,
					if (LB_ERR EQ SendMessage (hWnd,
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
			iSelCnt = SendMessage (hWnd, LB_GETSELCOUNT, 0, 0);

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
					SendMessage (hWndSM, message, wParam, lParam);

					return FALSE;	// We handled the msg
			} // End SWITCH

			break;
		} // End WM_KEYDOWN
#undef	nVirtKey
	} // End SWITCH

	return CallWindowProcW (lpfnOldListboxWndProc,
							hWnd,
							message,
							wParam,
							lParam); // Pass on down the line
} // End LclListboxWndProc
#endif


#ifdef DEBUG
//***************************************************************************
//	DbgMsg
//
//	Display a SBCS debug message
//***************************************************************************

void DbgMsg
	(LPCHAR szTemp)

{
	if (hWndDB)
		SendMessage (hWndDB, WM_USER + 0, 0, (LPARAM) szTemp);
} // End DbgMsg
#endif


#ifdef DEBUG
//***************************************************************************
//	DbgMsgW
//
//	Display a DBCS debug message
//***************************************************************************

void DbgMsgW
	(LPWCHAR wszTemp)

{
	if (hWndDB)
		SendMessageW (hWndDB, WM_USER + 1, 0, (LPARAM) wszTemp);
} // End DbgMsgW
#endif


#ifdef DEBUG
//***************************************************************************
//	DbgClear
//
//	Clear the debug window
//***************************************************************************

void DbgClr
	(void)

{
	PostMessage (hWndDB, WM_USER + 10, 0, 0);
} // End DbgClr
#endif


#ifdef DEBUG
//***************************************************************************
//	dprintf
//
//	Display a debug message a la printf
//***************************************************************************

void dprintf
	(LPCHAR lpszFmt, ...)

{
	va_list vl;
	int 	i1, i2, i3, i4;

	// We hope that no one calls us with more than
	//	 four arguments.
	// Note we must grab them separately this way
	//	 as using va_arg in the argument list to
	//	 wsprintf pushes the arguments in reverse
	//	 order.
	va_start (vl, lpszFmt);

	i1 = va_arg (vl, int);
	i2 = va_arg (vl, int);
	i3 = va_arg (vl, int);
	i4 = va_arg (vl, int);

	wsprintf (lpszTemp,
			  lpszFmt,
			  i1, i2, i3, i4);
	DbgMsg (lpszTemp);

	va_end (vl);
} // End dprintf
#endif


#ifdef DEBUG
//***************************************************************************
//	dprintfW
//
//	Display a debug message a la printf
//***************************************************************************

void dprintfW
	(LPWCHAR lpwszFmt, ...)

{
	va_list vl;
	int 	i1, i2, i3, i4;

	// We hope that no one calls us with more than
	//	 four arguments.
	// Note we must grab them separately this way
	//	 as using va_arg in the argument list to
	//	 wsprintf pushes the arguments in reverse
	//	 order.
	va_start (vl, lpwszFmt);

	i1 = va_arg (vl, int);
	i2 = va_arg (vl, int);
	i3 = va_arg (vl, int);
	i4 = va_arg (vl, int);

	wsprintfW (lpwszTemp,
			  lpwszFmt,
			  i1, i2, i3, i4);
	DbgMsgW (lpwszTemp);

	va_end (vl);
} // End dprintfW
#endif


#ifdef DEBUG
//***************************************************************************
//	DbgGlobalAllocSub
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
//	FileNameOnly
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
