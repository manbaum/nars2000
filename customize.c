//***************************************************************************
//	NARS2000 -- Customize Dialog Box
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

#define STRICT
#include <windows.h>
#include <windowsx.h>

#include "main.h"
#include "resdebug.h"
#include "resource.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//	$CustomizeDlgProc
//
//	Allow the user to customize the program
//***************************************************************************

__int3264 CALLBACK CustomizeDlgProc
	(HWND	hDlg,
	 UINT	message,
	 WPARAM wParam,
	 LPARAM lParam)

{
	static HFONT	  hFont = NULL;
	static HWND 	  hWndGroupBox,
					  hWndListBox,
					  hWndLast = NULL;
		   UINT 	  uSel, 					// Index of the current selection
					  uCnt; 					// Loop counter
		   HANDLE_PTR uIDD; 					// Item data:  IDD_xxx or HWND
		   RECT 	  rc;
	static POINT	  ptGroupBox;
	static SIZE 	  szGroupBox;
	static SAME_FONT_AS lclSameFontAs[SAMEFONTAS_LENGTH];

	static UINT 	 Radio1[] = {IDC_RADIO1A};
	static UINT 	 Radio2[] = {IDC_RADIO2A, IDC_RADIO2B};
	static UINT 	 Radio3[] = {IDC_RADIO3A, IDC_RADIO3B, IDC_RADIO3C};
	static UINT 	 Radio4[] = {IDC_RADIO4A, IDC_RADIO4B, IDC_RADIO4C, IDC_RADIO4D};
	static UINT 	 Radio5[] = {IDC_RADIO5A, IDC_RADIO5B, IDC_RADIO5C, IDC_RADIO5D, IDC_RADIO5E};
	static LPUINT	 RadioPtr[] = {&Radio1[0], &Radio2[0], &Radio3[0], &Radio4[0], &Radio5[0]};
		   HWND 	 hWndProp;					// Property page window handle


#define MYWM_INITDIALOG 				(WM_APP + 100)

	LCLODSAPI ("LB: ", hDlg, message, wParam, lParam);
	// Split cases based upon the message #
	switch (message)
	{
		case WM_INITDIALOG: 		// hwndFocus = (HWND) wParam; // handle of control to receive focus
									// lInitParam = lParam; 	  // initialization parameter
		{
			// If it's the first time through, ...
			if (lParam EQ 0)
			{
				// Get the IDC_CUST_LB window handle
				hWndListBox = GetDlgItem (hDlg, IDC_CUST_LB);

				// Loop through the selections
				for (uCnt = 0; uCnt < custStrucLen; uCnt++)
				{
					// Fill the ListBox with items and item data
					uSel = (UINT) SendMessage (hWndListBox, LB_ADDSTRING, 0, (LPARAM) custStruc[uCnt].lpTitle);
					SendMessage (hWndListBox, LB_SETITEMDATA, uSel, custStruc[uCnt].uIDD);

					// Mark as not initialized
					custStruc[uCnt].bInitialized = FALSE;
				} // End FOR

				// Get the IDC_GROUPBOX window handle
				hWndGroupBox = GetDlgItem (hDlg, IDC_GROUPBOX);

				// Get the coordinates of the groupbox in screen coordinates
				GetWindowRect (hWndGroupBox, &rc);

				// Copy to POINT & SIZE structs
				ptGroupBox.x  = rc.left;
				ptGroupBox.y  = rc.top;
				szGroupBox.cx = rc.right  - rc.left;
				szGroupBox.cy = rc.bottom - rc.top;

				// Convert these coordinates from screen-relative to dialog box-relative
				ScreenToClient (hDlg, &ptGroupBox);

				// Reposition the dialog to the center of the screen
				// ***FIXME*** -- Should we remember where it was the last time
				//				  and reposition it to there?
				CenterWindow (hDlg);

				// Tell the ListBox to change the current selection
				SendMessage (hWndListBox, LB_SETCURSEL, gInitCustomizeCategory, 0);

				// Tell the dialog the selection changed
				PostMessage (hDlg, WM_COMMAND, GET_WM_COMMAND_MPS (IDC_CUST_LB, 0, LBN_SELCHANGE));
			} // End IF

			return TRUE;			// Use the focus in wParam
		} // End WM_INITDIALOG

		case MYWM_INITDIALOG:
			if (!custStruc[wParam].bInitialized)
			{
				custStruc[wParam].bInitialized = TRUE;

				// Initialize each separate property page
				// Split cases based upon the IDD_xxx value
				switch (custStruc[wParam].uIDD)
				{
					case IDD_PROPPAGE_CLEARWS_VALUES:






						break;

					case IDD_PROPPAGE_DIRS:






						break;

					case IDD_PROPPAGE_FONTS:
						// Get the associated item data
						(HANDLE_PTR) hWndProp = SendMessage (hWndListBox, LB_GETITEMDATA, wParam, 0);

						// Copy global sameFontAs values to static local
						//	 and put into effect
						for (uCnt = 0; uCnt < SAMEFONTAS_LENGTH; uCnt++)
						{
							lclSameFontAs[uCnt] = glbSameFontAs[uCnt];
							SendMessage (GetDlgItem (hWndProp, RadioPtr[uCnt][glbSameFontAs[uCnt]]), BM_SETCHECK, TRUE, 0);
						} // End FOR

						break;

					case IDD_PROPPAGE_RANGE_LIMITS:
						// Get the associated item data
						(HANDLE_PTR) hWndProp = SendMessage (hWndListBox, LB_GETITEMDATA, wParam, 0);

						// Check the boxes of range limited system vars
						CheckDlgButton (hWndProp, IDC_RANGE_XB_CT, bRangeLimit.CT);
						CheckDlgButton (hWndProp, IDC_RANGE_XB_IO, bRangeLimit.IO);
						CheckDlgButton (hWndProp, IDC_RANGE_XB_IC, bRangeLimit.IC);
						CheckDlgButton (hWndProp, IDC_RANGE_XB_PP, bRangeLimit.PP);
						CheckDlgButton (hWndProp, IDC_RANGE_XB_PW, bRangeLimit.PW);
						CheckDlgButton (hWndProp, IDC_RANGE_XB_RL, bRangeLimit.RL);

						break;

					case IDD_PROPPAGE_SYNTAX_COLORING:






						break;

					case IDD_PROPPAGE_SYSTEM_VAR_RESET:






						break;

					case IDD_PROPPAGE_TAB_COLORS:






						break;

					case IDD_PROPPAGE_USER_PREFS:






						break;

					defstop
						break;
				} // End SWITCH
			} // End IF

			return TRUE;			// We handled the msg

		case WM_CLOSE:
			EndDialog (hDlg, TRUE); // Quit this dialog

			return TRUE;			// We handled the msg

		case WM_COMMAND:			// wNotifyCode = HIWORD(wParam); // Notification code
									// wID = LOWORD(wParam);		 // Item, control, or accelerator identifier
									// hwndCtl = (HWND) lParam; 	 // Handle of control
			// If the user pressed one of our buttons, ...
			switch (GET_WM_COMMAND_ID (wParam, lParam))
			{
				case IDOK:
					// Apply changes
					SendMessage (hDlg, WM_COMMAND, GET_WM_COMMAND_MPS (IDC_APPLY, 0, 0));

					// Close 'er up
					PostMessageW (hDlg, WM_CLOSE, 0, 0);

					return TRUE;	// We handled the msg

				case IDCANCEL:
					// Complain if abandoning changes




					// Close 'er up
					PostMessageW (hDlg, WM_CLOSE, 0, 0);

					return TRUE;	// We handled the msg

				case IDC_APPLY:
					// Apply changes

					//***************************************************************
					// CLEAR WS VALUES
					//***************************************************************

					wParam = IDD_PROPPAGE_CLEARWS_VALUES - IDD_PROPPAGE_START;
					if (custStruc[wParam].bInitialized)
					{
						// Get the associated item data (window handle of the Property Page)
						(HANDLE_PTR) hWndProp = SendMessage (hWndListBox, LB_GETITEMDATA, wParam, 0);






					} // End IF


					//***************************************************************
					// DIRECTORIES
					//***************************************************************

					wParam = IDD_PROPPAGE_DIRS - IDD_PROPPAGE_START;
					if (custStruc[wParam].bInitialized)
					{
						// Get the associated item data (window handle of the Property Page)
						(HANDLE_PTR) hWndProp = SendMessage (hWndListBox, LB_GETITEMDATA, wParam, 0);






					} // End IF


					//***************************************************************
					// FONTS
					//***************************************************************

					wParam = IDD_PROPPAGE_FONTS - IDD_PROPPAGE_START;
					if (custStruc[wParam].bInitialized)
					{
						// Get the associated item data (window handle of the Property Page)
						(HANDLE_PTR) hWndProp = SendMessage (hWndListBox, LB_GETITEMDATA, wParam, 0);






					} // End IF

					//***************************************************************
					// RANGE LIMITED VARS
					//***************************************************************

					wParam = IDD_PROPPAGE_RANGE_LIMITS - IDD_PROPPAGE_START;
					if (custStruc[wParam].bInitialized)
					{
						// Get the associated item data (window handle of the Property Page)
						(HANDLE_PTR) hWndProp = SendMessage (hWndListBox, LB_GETITEMDATA, wParam, 0);

						bRangeLimit.CT = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_CT);
						bRangeLimit.IO = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_IO);
						bRangeLimit.IC = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_IC);
						bRangeLimit.PP = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_PP);
						bRangeLimit.PW = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_PW);
						bRangeLimit.RL = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_RL);
					} // End IF

					//***************************************************************
					// SYNTAX COLORING
					//***************************************************************

					wParam = IDD_PROPPAGE_SYNTAX_COLORING - IDD_PROPPAGE_START;
					if (custStruc[wParam].bInitialized)
					{
						// Get the associated item data (window handle of the Property Page)
						(HANDLE_PTR) hWndProp = SendMessage (hWndListBox, LB_GETITEMDATA, wParam, 0);






					} // End IF

					//***************************************************************
					// SYSTEM VAR RESET
					//***************************************************************

					wParam = IDD_PROPPAGE_SYSTEM_VAR_RESET - IDD_PROPPAGE_START;
					if (custStruc[wParam].bInitialized)
					{
						// Get the associated item data (window handle of the Property Page)
						(HANDLE_PTR) hWndProp = SendMessage (hWndListBox, LB_GETITEMDATA, wParam, 0);






					} // End IF

					//***************************************************************
					// TAB COLORS
					//***************************************************************

					wParam = IDD_PROPPAGE_TAB_COLORS - IDD_PROPPAGE_START;
					if (custStruc[wParam].bInitialized)
					{
						// Get the associated item data (window handle of the Property Page)
						(HANDLE_PTR) hWndProp = SendMessage (hWndListBox, LB_GETITEMDATA, wParam, 0);






					} // End IF

					//***************************************************************
					// USER PREFERENCES
					//***************************************************************

					wParam = IDD_PROPPAGE_USER_PREFS - IDD_PROPPAGE_START;
					if (custStruc[wParam].bInitialized)
					{
						// Get the associated item data (window handle of the Property Page)
						(HANDLE_PTR) hWndProp = SendMessage (hWndListBox, LB_GETITEMDATA, wParam, 0);






					} // End IF

					return TRUE;	// We handled the msg

				case IDC_CUST_LB:
					// Split cases based upon the notification code
					switch (GET_WM_COMMAND_CMD (wParam, lParam))
					{
						case LBN_SELCHANGE:
							// Get the index of the current selection
							uSel = (UINT) SendMessage (hWndListBox, LB_GETCURSEL, 0, 0);

							// Save for next time
							gInitCustomizeCategory = uSel;

							// Get the associated item data (if any)
							uIDD = SendMessage (hWndListBox, LB_GETITEMDATA, uSel, 0);

							// If it's an IDD_xxx value, ...
							if (HIWORD (LODWORD (uIDD)) EQ 0)
							{
								hWndProp =
								  CreateDialogParam (_hInstance,
													 MAKEINTRESOURCE (uIDD),
													 hDlg,
										  (DLGPROC) &CustomizeDlgProc,
													 TRUE); 		// Anything non-zero so as to
																	//	 distinguish it from the
																	//	 initial entry
								// Save the window handle as the item data
								SendMessage (hWndListBox, LB_SETITEMDATA, uSel, (LPARAM) hWndProp);

								// Tell the dialog to initialize the new page
								PostMessage (hDlg, MYWM_INITDIALOG, uSel, 0);
							} else
								hWndProp = *(HWND *) &uIDD;
							// If there's an outgoing window, ...
							if (hWndLast)
							{
								// Position the outgoing window on the bottom
								SetWindowPos (hWndLast,
											  HWND_BOTTOM,
											  0,
											  0,
											  0,
											  0,
											  SWP_HIDEWINDOW
											| SWP_NOACTIVATE
											| SWP_NOMOVE
											| SWP_NOSIZE);
								EnableWindow (hWndLast, FALSE);
							} // End IF

							// Save incoming window handle as the last
							hWndLast = hWndProp;

							// Position the incoming window on the top
							SetWindowPos (hWndLast,
										  HWND_TOP,
										  ptGroupBox.x,
										  ptGroupBox.y,
										  szGroupBox.cx,
										  szGroupBox.cy,
										  SWP_SHOWWINDOW);
							EnableWindow (hWndLast, TRUE);

							break;

						default:
							break;
					} // End SWITCH

					return TRUE;	// We handled the msg

				case IDC_FONT1:
				case IDC_FONT2:
				case IDC_FONT3:
				case IDC_FONT4:
				case IDC_FONT5:
					// We care about BN_CLICKED only
					if (BN_CLICKED EQ GET_WM_COMMAND_CMD (wParam, lParam))
					{
						DbgBrk ();

						(*lpMyChooseFont[GET_WM_COMMAND_ID (wParam, lParam) - IDC_FONT1])();
					} // End IF

					return TRUE;	// We handled the msg

			} // End SWITCH

			break;
	} // End SWITCH

	return FALSE;			// We didn't handle the msg
} // End CustomizeDlgProc


//***************************************************************************
//	End of File: customize.c
//***************************************************************************
