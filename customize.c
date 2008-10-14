//***************************************************************************
//  NARS2000 -- Customize Dialog Box
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

/* To Do

* Set initial char vector lengths
* Handle changes to char vector lengths
* Validate values on Apply
* Figure why Infinity doesn't display correctly (that is, why Uniscribe isn't used)
* Finish Tab Colors
* Finish Syntax Coloring
* Finish Directories
*

*/

#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

#include "main.h"
#include "resdebug.h"
#include "resource.h"
#include "externs.h"
#include "sysvars.h"
#include "unitranshdr.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


WCHAR wszCancelMessage[] = L"You have made changes to the Customize settings.  Save the changes?";


//***************************************************************************
//  $CustomizeDlgProc
//
//  Allow the user to customize the program
//***************************************************************************

__int3264 CALLBACK CustomizeDlgProc
    (HWND   hWnd,
     UINT   message,
     WPARAM wParam,
     LPARAM lParam)

{
    static HFONT      hFont = NULL;             //
    static HWND       hWndGroupBox,             // Dialog GroupBox window handle
                      hWndListBox,              // Dialog ListBox  ...
                      hWndLast,                 // Last (outgoing) ...
                      hDlg,                     // Dialog          ...
                      hWndApply,                // Apply button    ...
                      hWndIC_CB1,               // []IC ComboBox #1 ...
                      hWndIC_CB2;               // []IC ...      #2 ...
           UINT       uSel,                     // Index of the current selection
                      uShow,                    // Show/hide flag
                      uCnt;                     // Loop counter
           HANDLE_PTR uIDD;                     // Item data:  IDD_xxx or HWND
           RECT       rc;                       // Temporary rectangle
           LPAPLCHAR  lpMemChr,                 // Ptr to global memory
                      lpaplChar;                // ...
           LPAPLINT   lpMemInt;                 // ...
           HGLOBAL    hGlbInt;                  // Global memory handle
           APLNELM    aplNELM;                  // NELM of object
           APLUINT    ByteRes;                  // # bytes in the result
    static POINT      ptGroupBox;               // X- & Y- coordinates (in pixels) of the upper left corner of the GroupBox
    static SIZE       szGroupBox;               // Size (in pixels) of the GroupBox
    static FONTENUM   lclSameFontAs[FONTENUM_LENGTH];

    static UINT       FontsRadio1[] = {IDC_FONTS_RADIO1A},
                      FontsRadio2[] = {IDC_FONTS_RADIO2A, IDC_FONTS_RADIO2B},
                      FontsRadio3[] = {IDC_FONTS_RADIO3A, IDC_FONTS_RADIO3B, IDC_FONTS_RADIO3C},
                      FontsRadio4[] = {IDC_FONTS_RADIO4A, IDC_FONTS_RADIO4B, IDC_FONTS_RADIO4C, IDC_FONTS_RADIO4D},
                      FontsRadio5[] = {IDC_FONTS_RADIO5A, IDC_FONTS_RADIO5B, IDC_FONTS_RADIO5C, IDC_FONTS_RADIO5D, IDC_FONTS_RADIO5E},
                      FontsRadio6[] = {IDC_FONTS_RADIO6A, IDC_FONTS_RADIO6B, IDC_FONTS_RADIO6C, IDC_FONTS_RADIO6D, IDC_FONTS_RADIO6E, IDC_FONTS_RADIO6F},
                      FontsRadio7[] = {IDC_FONTS_RADIO7A, IDC_FONTS_RADIO7B, IDC_FONTS_RADIO7C, IDC_FONTS_RADIO7D, IDC_FONTS_RADIO7E, IDC_FONTS_RADIO7F, IDC_FONTS_RADIO7G};
    static LPUINT     FontsRadioPtr[] = {&FontsRadio1[0],
                                         &FontsRadio2[0],
                                         &FontsRadio3[0],
                                         &FontsRadio4[0],
                                         &FontsRadio5[0],
                                         &FontsRadio6[0],
                                         &FontsRadio7[0]};
    static UINT       ResetRadioCT[] = {IDC_RESET_CT_RADIO1, IDC_RESET_CT_RADIO2},
                      ResetRadioFC[] = {IDC_RESET_FC_RADIO1, IDC_RESET_FC_RADIO2},
                      ResetRadioIC[] = {IDC_RESET_IC_RADIO1, IDC_RESET_IC_RADIO2},
                      ResetRadioIO[] = {IDC_RESET_IO_RADIO1, IDC_RESET_IO_RADIO2},
                      ResetRadioPP[] = {IDC_RESET_PP_RADIO1, IDC_RESET_PP_RADIO2},
                      ResetRadioPW[] = {IDC_RESET_PW_RADIO1, IDC_RESET_PW_RADIO2},
                      ResetRadioRL[] = {IDC_RESET_RL_RADIO1, IDC_RESET_RL_RADIO2};
    static LPUINT     ResetRadioPtr[] = {&ResetRadioCT[0],
                                         &ResetRadioFC[0],
                                         &ResetRadioIC[0],
                                         &ResetRadioIO[0],
                                         &ResetRadioPP[0],
                                         &ResetRadioPW[0],
                                         &ResetRadioRL[0]};
           HWND       hWndProp,                 // Property page window handle
                      hWndProp1,                // ...
                      hWndProp2,                // ...
                      hWndFont,                 // Font property page ChooseFont button window handle
                      hWnd_UD;                  // []IO/[]PP/[]PW UpDown Control or its Buddy window handle
           LOGFONT    lf_ST;                    // Static text LOGFONT
    static HFONT      hFontNorm_ST = NULL,      // Normal static text font handle
                      hFontBold_ST = NULL;      // Bold   ...
    static APLINT     icValues[ICNDX_LENGTH];   // []IC local values
    static RESET_VARS lclResetVars;             // Local copy of bResetVars
           WCHAR      wszTemp[128];             // Temporary WCHAR storage
           char        szTemp[128];             // Temporary char storage

    typedef struct tagUNITRANS_STR
    {
        LPWCHAR lpwChar;            // Ptr to text
        UBOOL   bValidCopy,         // TRUE iff this entry is valid for Copy
                bValidPaste;        // ...                              Paste
        UINT    IncTransCopy,       // Incoming Translation for Copy
                IncTransPaste,      // ...                      Paste
                OutTransCopy,       // Outgoing Translation for Copy
                OutTransPaste;      // ...                      Paste
    } UNITRANS_STR;

    // The following var must be in the same order as UNITRANS_xxx
    static UNITRANS_STR unitransStr[] = {{L"APL+Win",             TRUE,   TRUE , 0, 0, 0, 0},
                                         {L"ISO Standard PDF",    TRUE,   TRUE , 1, 1, 1, 1},
                                         {L"APL2",                TRUE,   TRUE , 2, 2, 2, 2},
                                         {L"Dyalog",              TRUE,   TRUE , 3, 3, 3, 3},
                                         {L"PC/3270",             TRUE,   TRUE , 4, 4, 4, 4},
                                         {L"Normal (Unicode)",    FALSE,  TRUE , 0, 5, 6, 5},
                                         {L"NARS (Unicode)",      TRUE,   FALSE, 5, 0, 0, 0},
                                        };
#define UNITRANS_STR_LENGTH      (sizeof (unitransStr) / sizeof (UNITRANS_STR))

    // The following text is too long for a resource file (limit 256 chars), so we define it here
    //   and save it ino the dialog upon initialization of the appropriate Property Page
    static LPWCHAR    lpwFontsText = L"There are seven categories of fonts (Session Manager, etc.).  "
                                     L"Click the button to the left of the text to change the font "
                                     L"for that category.  Click a radio button to the right of "
                                     L"the text to make the font for that category the same as the "
                                     L"font for the category at the top of that column.  Buttons "
                                     L"are visible only if that choice makes sense.",
                      lpwResetText = L"When one of the following System Variables is assigned a simple empty vector, "
                                     L"the system assigns to it a value which is either the workspace CLEAR WS "
                                     L"value (user-controlled) or the System Default value (fixed).  This dialog "
                                     L"allows you to choose which value is used.";

#define MYWM_INITDIALOG                 (WM_APP + 100)
#define MYWM_ENABLECHOOSEFONT           (WM_APP + 101)

////LCLODSAPI ("CD: ", hDlg, message, wParam, lParam);
    // Split cases based upon the message #
    switch (message)
    {
        case WM_INITDIALOG:         // hwndFocus = (HWND) wParam; // handle of control to receive focus
                                    // lInitParam = lParam;       // initialization parameter
            // If it's the first time through, ...
            if (lParam EQ 0)
            {
                // Because we use this DlgProc as the procedure for the
                //   Property Pages, too, when we're called by a Property
                //   Page, the incoming window handle is that of the
                //   Property Page, not the original dialog.  By saving the
                //   original window handle in the static var hDlg, we
                //   always know the dialog handle.
                hDlg = hWnd;

                // Mark as no last window handle
                hWndLast = NULL;

                // Mark as no previous Bold Static Text font
                hFontBold_ST = NULL;

                // Save the window handle for the Apply button for later use
                hWndApply = GetDlgItem (hDlg, IDC_APPLY);

                // Get the IDC_CUST_LB window handle
                hWndListBox = GetDlgItem (hDlg, IDC_CUST_LB);

                // Loop through the selections
                for (uCnt = 0; uCnt < custStrucLen; uCnt++)
                {
                    // Fill the ListBox with items and item data
                    uSel = (UINT) SendMessageW (hWndListBox, LB_ADDSTRING, 0, (LPARAM) custStruc[uCnt].lpwTitle);
                    SendMessageW (hWndListBox, LB_SETITEMDATA, uSel, custStruc[uCnt].uIDD);

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
                //                and reposition it to there?
                CenterWindow (hDlg);

                // Tell the ListBox to change the current selection
                SendMessageW (hWndListBox, LB_SETCURSEL, gInitCustomizeCategory, 0);

                // Tell the dialog the selection changed
                PostMessageW (hDlg, WM_COMMAND, GET_WM_COMMAND_MPS (IDC_CUST_LB, 0, LBN_SELCHANGE));
            } // End IF

            return TRUE;            // Use the focus in wParam

        case MYWM_INITDIALOG:
            // Set the group box text
            SetDlgItemTextW (hDlg, IDC_GROUPBOX, custStruc[wParam].lpwTitle);

            // If the struc hasn't been initialized as yet, ...
            if (!custStruc[wParam].bInitialized)
            {
                UBOOL bApply;                       // TRUE iff the Apply button is enabled

                // Mark it as initialized for next time
                custStruc[wParam].bInitialized = TRUE;

                // Get the associated item data
                (HANDLE_PTR) hWndProp = SendMessageW (hWndListBox, LB_GETITEMDATA, wParam, 0);

                // The techniques used to initialize the Property Pages
                //   sometimes cause the Apply button to be enabled.
                // To avoid enabling it spuriously, we save its state
                //   here and restore that state after the Property Page
                //   is initialized.
                bApply = IsWindowEnabled (hWndApply);

                // Initialize each separate property page
                // Split cases based upon the IDD_xxx value
                switch (custStruc[wParam].uIDD)
                {
                    case IDD_PROPPAGE_CLEARWS_VALUES:
                        // Initialize the CLEAR WS values

                        // Set the font for each Edit Ctrl or Combobox
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_ALX_EC), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontSM, FALSE);
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_CT_EC ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontSM, FALSE);
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_ELX_EC), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontSM, FALSE);
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_FC_EC ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontSM, FALSE);
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_IO_EC ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontSM, FALSE);
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_IC_CB1), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontSM, FALSE);
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_IC_CB2), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontSM, FALSE);
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_LX_EC ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontSM, FALSE);
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_MF_CB ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontSM, FALSE);
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_PP_EC ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontSM, FALSE);
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_PW_EC ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontSM, FALSE);
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_RL_EC ), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontSM, FALSE);

                        // Save the normal font handle
                        hFontNorm_ST = (HFONT) SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_ALX_LEN), WM_GETFONT, 0, 0);

                        // Get the current LOGFONT struct and make it bold
                        GetObject (hFontNorm_ST, sizeof (lf_ST), &lf_ST);

                        // Set the new weight
                        lf_ST.lfWeight = FW_BOLD;

                        // Create a new HFONT from the changed LOGFONT
                        hFontBold_ST = MyCreateFontIndirect (&lf_ST);

                        //***************************************************************
                        // []ALX
                        //***************************************************************

                        // Lock the memory to get a ptr to it
                        lpMemChr = MyGlobalLock (hGlbQuadALX_CWS);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemChr)
                        // Get the NELM
                        aplNELM = lpHeader->NELM;
#undef  lpHeader
                        // Skip over the header and dimension to the data
                        lpMemChr = VarArrayBaseToData (lpMemChr, 1);

                        // Copy to temp string so we can zero-terminate it
                        CopyMemory (lpwszGlbTemp, lpMemChr, (__int3264) aplNELM * sizeof (APLCHAR));

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbQuadALX_CWS); lpMemChr = NULL;

                        // Ensure properly terminated
                        lpwszGlbTemp[aplNELM] = L'\0';

                        // Set the text
                        SetDlgItemTextW (hWndProp, IDC_CLEARWS_ALX_EC, lpwszGlbTemp);

                        //***************************************************************
                        // []CT
                        //***************************************************************

                        // Format the value
                        lpMemChr =
                          FormatFloatFC (lpwszGlbTemp,                  // Ptr to output save area
                                         fQuadCT_CWS,                   // The value to format
                                         DEF_MAX_QUADPP,                // Precision to use
                                         L'.',                          // Char to use as decimal separator
                                         UTF16_OVERBAR,                 // Char to use as overbar
                                         DEF_DTOA_MODE);                // DTOA mode (Mode 2: max (ndigits, 1))
                        // Zap the trailing blank
                        lpMemChr[-1] = L'\0';

                        // Set the text
                        SetDlgItemTextW (hWndProp, IDC_CLEARWS_CT_EC,  lpwszGlbTemp);

                        //***************************************************************
                        // []ELX
                        //***************************************************************

                        // Lock the memory to get a ptr to it
                        lpMemChr = MyGlobalLock (hGlbQuadELX_CWS);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemChr)
                        // Get the NELM
                        aplNELM = lpHeader->NELM;
#undef  lpHeader
                        // Skip over the header and dimension to the data
                        lpMemChr = VarArrayBaseToData (lpMemChr, 1);

                        // Copy to temp string so we can zero-terminate it
                        CopyMemory (lpwszGlbTemp, lpMemChr, (__int3264) aplNELM * sizeof (APLCHAR));

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbQuadELX_CWS); lpMemChr = NULL;

                        // Ensure properly terminated
                        lpwszGlbTemp[aplNELM] = L'\0';

                        // Set the text
                        SetDlgItemTextW (hWndProp, IDC_CLEARWS_ELX_EC, lpwszGlbTemp);

                        //***************************************************************
                        // []FC
                        //***************************************************************

                        // Lock the memory to get a ptr to it
                        lpMemChr = MyGlobalLock (hGlbQuadFC_CWS);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemChr)
                        // Get the NELM
                        aplNELM = lpHeader->NELM;
#undef  lpHeader
                        // Skip over the header and dimension to the data
                        lpMemChr = VarArrayBaseToData (lpMemChr, 1);

                        // Copy to temp string so we can zero-terminate it
                        CopyMemory (lpwszGlbTemp, lpMemChr, (__int3264) aplNELM * sizeof (APLCHAR));

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbQuadFC_CWS); lpMemChr = NULL;

                        // Ensure properly terminated
                        lpwszGlbTemp[aplNELM] = L'\0';

                        // Set the text
                        SetDlgItemTextW (hWndProp, IDC_CLEARWS_FC_EC,  lpwszGlbTemp);

                        //***************************************************************
                        // []IO
                        //***************************************************************

                        // Get the []IO UpDown Control window handle
                        hWnd_UD = GetDlgItem (hWndProp, IDC_CLEARWS_IO_UD);

                        // Set the range
                        SendMessageW (hWnd_UD, UDM_SETRANGE, 0, MAKELONG (DEF_MAX_QUADIO, DEF_MIN_QUADIO));

                        // Set the initial position
                        SendMessageW (hWnd_UD, UDM_SETPOS, 0, MAKELONG (bQuadIO_CWS, 0));

                        //***************************************************************
                        // []IC
                        //***************************************************************

                        // Get the window handle to the ComboBox of index names & values
                        hWndIC_CB1 = GetDlgItem (hWndProp, IDC_CLEARWS_IC_CB1);
                        hWndIC_CB2 = GetDlgItem (hWndProp, IDC_CLEARWS_IC_CB2);

                        // Insert the []IC index names
                        for (uCnt = 0; uCnt < ICNDX_LENGTH; uCnt++)
                            SendMessageW (hWndIC_CB1, CB_ADDSTRING, 0, (LPARAM) (HANDLE_PTR) icIndexNames[uCnt]);

                        // Insert the []IC index values
                        for (uCnt = 0; uCnt < ICVAL_LENGTH; uCnt++)
                            SendMessageW (hWndIC_CB2, CB_ADDSTRING, 0, (LPARAM) (HANDLE_PTR) icIndexValues[uCnt]);

                        // Lock the memory to get a ptr to it
                        lpMemInt = MyGlobalLock (hGlbQuadIC_CWS);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemInt)
                        // Get the NELM
                        aplNELM = lpHeader->NELM;
#undef  lpHeader
                        // Skip over the header and dimension to the data
                        lpMemInt = VarArrayBaseToData (lpMemInt, 1);

                        // Set the current selection to the first name
                        //   and its corresponding value
                        SendMessageW (hWndIC_CB1, CB_SETCURSEL, 0, 0);
                        SendMessageW (hWndIC_CB2, CB_SETCURSEL, (APLU3264) lpMemInt[0], 0);

                        // Initialize the []IC local values
                        for (uCnt = 0; uCnt < ICNDX_LENGTH; uCnt++)
                        if (uCnt < aplNELM)
                            icValues[uCnt] = lpMemInt[uCnt];
                        else
                            icValues[uCnt] = aplDefaultIC[uCnt];

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbQuadIC_CWS); lpMemInt = NULL;

                        //***************************************************************
                        // []LX
                        //***************************************************************

                        // Lock the memory to get a ptr to it
                        lpMemChr = MyGlobalLock (hGlbQuadLX_CWS);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemChr)
                        // Get the NELM
                        aplNELM = lpHeader->NELM;
#undef  lpHeader
                        // Skip over the header and dimension to the data
                        lpMemChr = VarArrayBaseToData (lpMemChr, 1);

                        // Copy to temp string so we can zero-terminate it
                        CopyMemory (lpwszGlbTemp, lpMemChr, (__int3264) aplNELM * sizeof (APLCHAR));

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbQuadLX_CWS); lpMemChr = NULL;

                        // Ensure properly terminated
                        lpwszGlbTemp[aplNELM] = L'\0';

                        // Set the text
                        SetDlgItemTextW (hWndProp, IDC_CLEARWS_LX_EC,  lpwszGlbTemp);

                        //***************************************************************
                        // []MF
                        //***************************************************************

                        // Format the value
                        lpaplChar =
                          FormatAplint (lpwszGlbTemp,                   // Ptr to output save area
                                        uQuadMF_CWS);                   // The value to format
                        // Zap the trailing blank
                        lpaplChar[-1] = L'\0';

                        // Get the window handle for the Monitor Function combo box
                        hWndProp1 = GetDlgItem (hWndProp, IDC_CLEARWS_MF_CB);

                        // Append the possible values
                        SendMessageW (hWndProp1, CB_ADDSTRING, 0, (LPARAM) L"Millisecond Timer");
                        SendMessageW (hWndProp1, CB_ADDSTRING, 0, (LPARAM) L"High Resolution Timer");

                        // Set the current selection to the CLEAR WS value
                        SendMessageW (hWndProp1, CB_SETCURSEL, (APLU3264) uQuadMF_CWS - 1, 0);

                        //***************************************************************
                        // []PP
                        //***************************************************************

                        // Get the []IO UpDown Control window handle
                        hWnd_UD = GetDlgItem (hWndProp, IDC_CLEARWS_PP_UD);

                        // Set the range
                        SendMessageW (hWnd_UD, UDM_SETRANGE, 0, MAKELONG (DEF_MAX_QUADPP, DEF_MIN_QUADPP));

                        // Set the initial position
                        SendMessageW (hWnd_UD, UDM_SETPOS, 0, MAKELONG (uQuadPP_CWS, 0));

                        //***************************************************************
                        // []PW
                        //***************************************************************

                        // Get the []IO UpDown Control window handle
                        hWnd_UD = GetDlgItem (hWndProp, IDC_CLEARWS_PW_UD);

                        // Set the range
                        SendMessageW (hWnd_UD, UDM_SETRANGE, 0, MAKELONG (DEF_MAX_QUADPW, DEF_MIN_QUADPW));

                        // Set the initial position
                        SendMessageW (hWnd_UD, UDM_SETPOS, 0, MAKELONG (uQuadPW_CWS, 0));

                        //***************************************************************
                        // []RL
                        //***************************************************************

                        // Format the value
                        lpaplChar =
                          FormatAplint (lpwszGlbTemp,                   // Ptr to output save area
                                        uQuadRL_CWS);                   // The value to format
                        // Zap the trailing blank
                        lpaplChar[-1] = L'\0';

                        // Set the text
                        SetDlgItemTextW (hWndProp, IDC_CLEARWS_RL_EC,  lpwszGlbTemp);

                        break;

////////////////////case IDD_PROPPAGE_DIRS:
////////////////////    DbgBrk ();      // ***FINISHME***
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////    break;

                    case IDD_PROPPAGE_FONTS:
                        // Initialize the instructions
                        SetWindowTextW (GetDlgItem (hWndProp, IDC_FONTS_TEXT), lpwFontsText);

                        // Initialize the structs
                        for (uCnt = 0; uCnt < FONTENUM_LENGTH; uCnt++)
                        {
                            // Set the font for the IDC_FONTx buttons
                            SendMessageW (GetDlgItem (hWndProp, IDC_FONT1 + uCnt), WM_SETFONT, (WPARAM) (HANDLE_PTR) hFontSM, MAKELPARAM (TRUE, 0));

                            // Copy the global SameFontAs value to the local
                            lclSameFontAs[uCnt]      = glbSameFontAs[uCnt];

                            // Copy the global CHOOSEFONT data to the local
                            fontStruc[uCnt].cfLcl    = *fontStruc[uCnt].lpcf;

                            // Clear the changed flag
                            fontStruc[uCnt].bChanged = FALSE;

                            // Set the radio button initial states
                            SendMessageW (GetDlgItem (hWndProp, FontsRadioPtr[uCnt][glbSameFontAs[uCnt]]), BM_SETCHECK, TRUE, 0);

                            // Simulate the user clicking the radio button
                            SendMessageW (hDlg, WM_COMMAND, GET_WM_COMMAND_MPS (FontsRadioPtr[uCnt][glbSameFontAs[uCnt]], NULL, 0));
                        } // End FOR

                        break;

                    case IDD_PROPPAGE_RANGE_LIMITS:
                        // Check the boxes of Range Limited system vars
                        CheckDlgButton (hWndProp, IDC_RANGE_XB_CT, bRangeLimit.CT);
                        CheckDlgButton (hWndProp, IDC_RANGE_XB_IC, bRangeLimit.IC);
                        CheckDlgButton (hWndProp, IDC_RANGE_XB_IO, bRangeLimit.IO);
                        CheckDlgButton (hWndProp, IDC_RANGE_XB_PP, bRangeLimit.PP);
                        CheckDlgButton (hWndProp, IDC_RANGE_XB_PW, bRangeLimit.PW);
                        CheckDlgButton (hWndProp, IDC_RANGE_XB_RL, bRangeLimit.RL);

                        break;

////////////////////case IDD_PROPPAGE_SYNTAX_COLORING:
////////////////////    DbgBrk ();      // ***FINISHME***
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////    break;

                    case IDD_PROPPAGE_SYSTEM_VAR_RESET:
                        // Initialize the instructions
                        SetWindowTextW (GetDlgItem (hWndProp, IDC_RESET_TEXT), lpwResetText);

                        // Initialize the local copy of bResetVars
                        lclResetVars = bResetVars;

                        // Set the radio button initial states
                        SendMessageW (GetDlgItem (hWndProp, ResetRadioPtr[0][lclResetVars.CT]), BM_SETCHECK, TRUE, 0);
                        SendMessageW (GetDlgItem (hWndProp, ResetRadioPtr[1][lclResetVars.FC]), BM_SETCHECK, TRUE, 0);
                        SendMessageW (GetDlgItem (hWndProp, ResetRadioPtr[2][lclResetVars.IC]), BM_SETCHECK, TRUE, 0);
                        SendMessageW (GetDlgItem (hWndProp, ResetRadioPtr[3][lclResetVars.IO]), BM_SETCHECK, TRUE, 0);
                        SendMessageW (GetDlgItem (hWndProp, ResetRadioPtr[4][lclResetVars.PP]), BM_SETCHECK, TRUE, 0);
                        SendMessageW (GetDlgItem (hWndProp, ResetRadioPtr[5][lclResetVars.PW]), BM_SETCHECK, TRUE, 0);
                        SendMessageW (GetDlgItem (hWndProp, ResetRadioPtr[6][lclResetVars.RL]), BM_SETCHECK, TRUE, 0);

                        break;

////////////////////case IDD_PROPPAGE_TAB_COLORS:
////////////////////    DbgBrk ();      // ***FINISHME***
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////    break;

                    case IDD_PROPPAGE_USER_PREFS:
                        // Check the boxes of User Preferences
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_ADJUSTPW           , OptionFlags.bAdjustPW           );
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_UNDERBARTOLOWERCASE, OptionFlags.bUnderbarToLowercase);
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_NEWTABONCLEAR      , OptionFlags.bNewTabOnClear      );
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_NEWTABONLOAD       , OptionFlags.bNewTabOnLoad       );
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_USELOCALTIME       , OptionFlags.bUseLocalTime       );
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_BACKUPONLOAD       , OptionFlags.bBackupOnLoad       );
                        CheckDlgButton (hWndProp, IDC_USER_PREFS_XB_BACKUPONSAVE       , OptionFlags.bBackupOnSave       );

                        // Get the window handle for the Paste & Copy combo boxes
                        hWndProp1 = GetDlgItem (hWndProp, IDC_USER_PREFS_CB_DEFAULTPASTE);
                        hWndProp2 = GetDlgItem (hWndProp, IDC_USER_PREFS_CB_DEFAULTCOPY);

                        // Loop through the Unitrans Copy/Paste options
                        for (uCnt = 0; uCnt < UNITRANS_STR_LENGTH; uCnt++)
                        {
                            if (unitransStr[uCnt].bValidPaste)
                                SendMessageW (hWndProp1, CB_ADDSTRING, 0, (LPARAM) unitransStr[uCnt].lpwChar);
                            if (unitransStr[uCnt].bValidCopy)
                                SendMessageW (hWndProp2, CB_ADDSTRING, 0, (LPARAM) unitransStr[uCnt].lpwChar);
                        } // End FOR

                        // Set the current selection to the user preference value
                        SendMessageW (hWndProp1, CB_SETCURSEL, unitransStr[OptionFlags.uDefaultPaste].IncTransPaste, 0);
                        SendMessageW (hWndProp2, CB_SETCURSEL, unitransStr[OptionFlags.uDefaultCopy ].IncTransCopy , 0);

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Restore the Apply button enabl/disable state
                EnableWindow (hWndApply, bApply);
            } // End IF

            return TRUE;            // We handled the msg

        case MYWM_ENABLECHOOSEFONT:                     // ctrlId  = (UINT) wParam;
                                                        // bEnable = (UBOOL) lParam;
            // Get the associated item data (window handle of the Property Page)
            (HANDLE_PTR) hWndProp = SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_FONTS - IDD_PROPPAGE_START, 0);

            // Get the SW_xxx flag
            uShow = lParam ? SW_SHOWNORMAL : SW_HIDE;

            // Show/Hide the radio buttons under the endpoint
            for (uSel = wParam - IDC_FONT1, uCnt = uSel + 1; uCnt < FONTENUM_LENGTH; uCnt++)
                ShowWindow (GetDlgItem (hWndProp, FontsRadioPtr[uCnt][uSel]), uShow);

            // Get the window handle of the ChooseFont button
            hWndFont = GetDlgItem (hWndProp, wParam);

            // Show/Hide the ChooseFont button
            ShowWindow (hWndFont, uShow);

            // Turn off the hilite state
            // ***FIXME*** -- Why doesn't this work??
            SendMessageW (hWndFont, BM_SETSTATE, FALSE, 0);

            // Enable/disable the corresponding ChooseFont button
            EnableWindow (hWndFont, lParam);

            // Loop through the ending radio buttons to see if we should
            //   show/hide the radio buttons to the left
            for (uCnt = 1; uCnt < FONTENUM_LENGTH; uCnt++)
            {
                // If this ending button is checked, ...
                if (IsDlgButtonChecked (hWndProp, FontsRadioPtr[uCnt][uCnt]))
                {
                    // If there are checked radio buttons underneath, ...
                    for (uSel = uCnt + 1; uSel < FONTENUM_LENGTH; uSel++)
                    if (IsDlgButtonChecked (hWndProp, FontsRadioPtr[uSel][uCnt]))
                    {
                        // Loop through the radio buttons to the left of this ending button
                        for (uSel = 0; uSel < uCnt; uSel++)
                            // Hide the radio buttons to the left of this ending button
                            ShowWindow (GetDlgItem (hWndProp, FontsRadioPtr[uCnt][uSel]), SW_HIDE);
                        break;
                    } // End FOR
                } else
                    uSel = FONTENUM_LENGTH;

                // If there were no checked radio buttons underneath, ...
                if (uSel EQ FONTENUM_LENGTH)
                {
                    // Loop through the radio buttons to the left of this ending button
                    for (uSel = 0; uSel < uCnt; uSel++)
                    // If the ending button for this column is checked, ...
                    if (IsDlgButtonChecked (hWndProp, FontsRadioPtr[uSel][uSel]))
                        // Show the radio button to the left of this ending button
                        ShowWindow (GetDlgItem (hWndProp, FontsRadioPtr[uCnt][uSel]), SW_SHOWNORMAL);
                } // End IF
            } // End FOR

            // Enable the Apply button
            EnableWindow (hWndApply, TRUE);

            return TRUE;            // We handled the msg

        case WM_CLOSE:
            SendMessageW (hDlg, WM_COMMAND, GET_WM_COMMAND_MPS (IDCANCEL, 0, 0));

            return TRUE;            // We handled the msg

        case WM_COMMAND:            // wNotifyCode = HIWORD(wParam); // Notification code
                                    // wID = LOWORD(wParam);         // Item, control, or accelerator identifier
                                    // hwndCtl = (HWND) lParam;      // Handle of control
            // If the user pressed one of our buttons, ...
            switch (GET_WM_COMMAND_ID (wParam, lParam))
            {
                case IDOK:
                    // Apply changes
                    SendMessageW (hDlg, WM_COMMAND, GET_WM_COMMAND_MPS (IDC_APPLY, 0, 0));

                    // Close 'er up
                    EndDialog (hDlg, TRUE); // Quit this dialog

                    return TRUE;    // We handled the msg

                case IDCANCEL:
                    // Complain if abandoning changes
                    if (IsWindowEnabled (hWndApply))
                    // Split cases based upon the user response
                    switch (MessageBoxW (NULL, wszCancelMessage, WS_APPNAME, MB_YESNOCANCEL | MB_ICONQUESTION))
                    {
                        case IDYES:
                            // Apply changes
                            SendMessageW (hDlg, WM_COMMAND, GET_WM_COMMAND_MPS (IDC_APPLY, 0, 0));

                            break;

                        case IDNO:
                            break;

                        case IDCANCEL:
                            return TRUE;

                        defstop
                            break;
                    } // End IF/SWITCH

                    // Close 'er up
                    EndDialog (hDlg, TRUE); // Quit this dialog

                    return TRUE;    // We handled the msg

                case IDC_APPLY:
                    // Apply changes

                    //***************************************************************
                    // CLEAR WS VALUES
                    //***************************************************************

                    // Get the Property Page index
                    uCnt = IDD_PROPPAGE_CLEARWS_VALUES - IDD_PROPPAGE_START;
                    if (custStruc[uCnt].bInitialized)
                    {
                        LPWCHAR wp;

                        // Get the associated item data (window handle of the Property Page)
                        (HANDLE_PTR) hWndProp = SendMessageW (hWndListBox, LB_GETITEMDATA, uCnt, 0);

                        //***************************************************************
                        // []ALX
                        //***************************************************************

                        // Read in and save the new []ALX value
                        GetCLEARWSValue (hWndProp, IDC_CLEARWS_ALX_EC, &hGlbQuadALX_CWS);

                        //***************************************************************
                        // []CT
                        //***************************************************************

                        // Tell the Edit Ctrl how big our buffer is
                        lpwszGlbTemp[0] = 1024;

                        // Get the []CT text
                        SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_CT_EC), EM_GETLINE, 0, (LPARAM) lpwszGlbTemp);

                        // Save as current ptr
                        wp = lpwszGlbTemp;

                        // Convert overbar to high minus
                        while (wp = strchrW (wp, UTF16_OVERBAR))
                            *wp++ = L'-';

                        // Convert the format string to ASCII
                        W2A (szTemp, lpwszGlbTemp, sizeof (szTemp) - 1);

                        // Convert this text to a
                        //   using David Gay's routines
                        fQuadCT_CWS = strtod (szTemp, NULL);

                        //***************************************************************
                        // []ELX
                        //***************************************************************

                        // Read in and save the new []ELX value
                        GetCLEARWSValue (hWndProp, IDC_CLEARWS_ELX_EC, &hGlbQuadELX_CWS);

                        //***************************************************************
                        // []FC
                        //***************************************************************

                        // Read in and save the new []FC value
                        GetCLEARWSValue (hWndProp, IDC_CLEARWS_FC_EC, &hGlbQuadFC_CWS);

                        //***************************************************************
                        // []IO
                        //***************************************************************
                        bQuadIO_CWS =
                          GetDlgItemInt (hWndProp, IDC_CLEARWS_IO_EC, NULL, FALSE);

                        //***************************************************************
                        // []IC
                        //***************************************************************

                        // Calculate space needed for the result
                        ByteRes = CalcArraySize (ARRAY_INT, ICNDX_LENGTH, 1);

                        // Allocate space for the data
                        // Note, we can't use DbgGlobalAlloc here as the
                        //   PTD has not been allocated as yet
                        hGlbInt = MyGlobalAlloc (GHND, (APLU3264) ByteRes);
                        if (hGlbInt)
                        {
                            // Free the current value
                            DbgGlobalFree (hGlbQuadIC_CWS); hGlbQuadIC_CWS = NULL;

                            // Save the new global memory handle
                            hGlbQuadIC_CWS = hGlbInt;

                            // Lock the memory to get a ptr to it
                            lpMemInt = MyGlobalLock (hGlbInt);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemInt)
                            // Fill in the header values
                            lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                            lpHeader->ArrType    = ARRAY_INT;
////////////////////////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////////////////////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
                            lpHeader->RefCnt     = 1;
                            lpHeader->NELM       = ICNDX_LENGTH;
                            lpHeader->Rank       = 1;
#undef  lpHeader
                            // Save the dimension
                            *VarArrayBaseToDim (lpMemInt) = ICNDX_LENGTH;

                            // Skip over the header and dimensions to the data
                            lpMemInt = VarArrayBaseToData (lpMemInt, 1);

                            // Copy and convert the values to the result
                            CopyMemory (lpMemInt, icValues, ICNDX_LENGTH * sizeof (icValues[0]));

                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbInt); lpMemInt = NULL;
                        } // End IF

                        //***************************************************************
                        // []LX
                        //***************************************************************

                        // Read in and save the new []LX value
                        GetCLEARWSValue (hWndProp, IDC_CLEARWS_LX_EC, &hGlbQuadLX_CWS);

                        //***************************************************************
                        // []MF
                        //***************************************************************
                        uQuadMF_CWS =
                          1 + SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_MF_CB), CB_GETCURSEL, 0, 0);

                        //***************************************************************
                        // []PP
                        //***************************************************************
                        uQuadPP_CWS =
                          GetDlgItemInt (hWndProp, IDC_CLEARWS_PP_EC, NULL, FALSE);

                        //***************************************************************
                        // []PW
                        //***************************************************************
                        uQuadPW_CWS =
                          GetDlgItemInt (hWndProp, IDC_CLEARWS_PW_EC, NULL, FALSE);

                        //***************************************************************
                        // []RL
                        //***************************************************************
                        uQuadRL_CWS =
                          GetDlgItemInt (hWndProp, IDC_CLEARWS_RL_EC, NULL, FALSE);
                    } // End IF


                    //***************************************************************
                    // DIRECTORIES
                    //***************************************************************

////////////////////// Get the Property Page index
////////////////////uCnt = IDD_PROPPAGE_DIRS - IDD_PROPPAGE_START;
////////////////////if (custStruc[uCnt].bInitialized)
////////////////////{
////////////////////    // Get the associated item data (window handle of the Property Page)
////////////////////    (HANDLE_PTR) hWndProp = SendMessageW (hWndListBox, LB_GETITEMDATA, uCnt, 0);
////////////////////
////////////////////    DbgBrk ();      // ***FINISHME***
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////} // End IF


                    //***************************************************************
                    // FONTS
                    //***************************************************************

                    // Get the Property Page index
                    uCnt = IDD_PROPPAGE_FONTS - IDD_PROPPAGE_START;
                    if (custStruc[uCnt].bInitialized)
                    {
                        // Get the associated item data (window handle of the Property Page)
                        (HANDLE_PTR) hWndProp = SendMessageW (hWndListBox, LB_GETITEMDATA, uCnt, 0);

                        // Loop through the fonts
                        for (uCnt = 0; uCnt < FONTENUM_LENGTH; uCnt++)
                        {
                            // If the font is the same as another, or
                            //   its SameFontAs state changed, ...
                            if (lclSameFontAs[uCnt] NE (FONTENUM) uCnt
                             || glbSameFontAs[uCnt] NE lclSameFontAs[uCnt])
                            {
                                // If its SameFontAs state changed, ...
                                if (glbSameFontAs[uCnt] NE lclSameFontAs[uCnt])
                                {
                                    // Note that we don't copy the CHOOSEFONT struc values
                                    //   (.lpcf) as they are specific to the category
                                    *fontStruc[uCnt].lplf = *fontStruc[lclSameFontAs[uCnt]].lplf;
                                    *fontStruc[uCnt].lptm = *fontStruc[lclSameFontAs[uCnt]].lptm;

                                    // Copy the local SameFontAs value to the global
                                    glbSameFontAs[uCnt] = lclSameFontAs[uCnt];
                                } // End IF
                            } else
                            // If the font changed, ...
                            if (fontStruc[uCnt].bChanged)
                            {
                                // Copy the local CHOOSEFONT value to the global
                                *fontStruc[uCnt].lpcf = fontStruc[uCnt].cfLcl;

                                // Call the CreateNewFontXX for this font
                                (*fontStruc[uCnt].lpCreateNewFont)();
                            } // End IF
                        } // End FOR
                    } // End IF

                    //***************************************************************
                    // RANGE LIMITED VARS
                    //***************************************************************

                    // Get the Property Page index
                    uCnt = IDD_PROPPAGE_RANGE_LIMITS - IDD_PROPPAGE_START;
                    if (custStruc[uCnt].bInitialized)
                    {
                        // Get the associated item data (window handle of the Property Page)
                        (HANDLE_PTR) hWndProp = SendMessageW (hWndListBox, LB_GETITEMDATA, uCnt, 0);

                        bRangeLimit.CT = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_CT);
                        bRangeLimit.IC = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_IC);
                        bRangeLimit.IO = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_IO);
                        bRangeLimit.PP = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_PP);
                        bRangeLimit.PW = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_PW);
                        bRangeLimit.RL = IsDlgButtonChecked (hWndProp, IDC_RANGE_XB_RL);
                    } // End IF

                    //***************************************************************
                    // SYNTAX COLORING
                    //***************************************************************

////////////////////// Get the Property Page index
////////////////////uCnt = IDD_PROPPAGE_SYNTAX_COLORING - IDD_PROPPAGE_START;
////////////////////if (custStruc[uCnt].bInitialized)
////////////////////{
////////////////////    // Get the associated item data (window handle of the Property Page)
////////////////////    (HANDLE_PTR) hWndProp = SendMessageW (hWndListBox, LB_GETITEMDATA, uCnt, 0);
////////////////////
////////////////////    DbgBrk ();      // ***FINISHME***
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////} // End IF

                    //***************************************************************
                    // SYSTEM VAR RESET
                    //***************************************************************

                    // Get the Property Page index
                    uCnt = IDD_PROPPAGE_SYSTEM_VAR_RESET - IDD_PROPPAGE_START;
                    if (custStruc[uCnt].bInitialized)
                    {
                        // Get the associated item data (window handle of the Property Page)
                        (HANDLE_PTR) hWndProp = SendMessageW (hWndListBox, LB_GETITEMDATA, uCnt, 0);

                        bResetVars.CT = IsDlgButtonChecked (hWndProp, IDC_RESET_CT_RADIO2);
                        bResetVars.FC = IsDlgButtonChecked (hWndProp, IDC_RESET_FC_RADIO2);
                        bResetVars.IC = IsDlgButtonChecked (hWndProp, IDC_RESET_IC_RADIO2);
                        bResetVars.IO = IsDlgButtonChecked (hWndProp, IDC_RESET_IO_RADIO2);
                        bResetVars.PP = IsDlgButtonChecked (hWndProp, IDC_RESET_PP_RADIO2);
                        bResetVars.PW = IsDlgButtonChecked (hWndProp, IDC_RESET_PW_RADIO2);
                        bResetVars.RL = IsDlgButtonChecked (hWndProp, IDC_RESET_RL_RADIO2);
                    } // End IF

                    //***************************************************************
                    // TAB COLORS
                    //***************************************************************

////////////////////// Get the Property Page index
////////////////////uCnt = IDD_PROPPAGE_TAB_COLORS - IDD_PROPPAGE_START;
////////////////////if (custStruc[uCnt].bInitialized)
////////////////////{
////////////////////    // Get the associated item data (window handle of the Property Page)
////////////////////    (HANDLE_PTR) hWndProp = SendMessageW (hWndListBox, LB_GETITEMDATA, uCnt, 0);
////////////////////
////////////////////    DbgBrk ();      // ***FINISHME***
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////
////////////////////} // End IF

                    //***************************************************************
                    // USER PREFERENCES
                    //***************************************************************

                    // Get the Property Page index
                    uCnt = IDD_PROPPAGE_USER_PREFS - IDD_PROPPAGE_START;
                    if (custStruc[uCnt].bInitialized)
                    {
                        // Get the associated item data (window handle of the Property Page)
                        (HANDLE_PTR) hWndProp = SendMessageW (hWndListBox, LB_GETITEMDATA, uCnt, 0);

                        OptionFlags.bAdjustPW            = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_ADJUSTPW           );
                        OptionFlags.bUnderbarToLowercase = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_UNDERBARTOLOWERCASE);
                        OptionFlags.bNewTabOnClear       = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_NEWTABONCLEAR      );
                        OptionFlags.bNewTabOnLoad        = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_NEWTABONLOAD       );
                        OptionFlags.bUseLocalTime        = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_USELOCALTIME       );
                        OptionFlags.bBackupOnLoad        = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_BACKUPONLOAD       );
                        OptionFlags.bBackupOnSave        = IsDlgButtonChecked (hWndProp, IDC_USER_PREFS_XB_BACKUPONSAVE       );

                        // Get the window handle for the Paste & Copy combo boxes
                        hWndProp1 = GetDlgItem (hWndProp, IDC_USER_PREFS_CB_DEFAULTPASTE);
                        hWndProp2 = GetDlgItem (hWndProp, IDC_USER_PREFS_CB_DEFAULTCOPY);

                        // Get the current selection of the user preference value
                        OptionFlags.uDefaultPaste = unitransStr[SendMessageW (hWndProp1, CB_GETCURSEL, 0, 0)].OutTransPaste;
                        OptionFlags.uDefaultCopy  = unitransStr[SendMessageW (hWndProp2, CB_GETCURSEL, 0, 0)].OutTransCopy ;
                    } // End IF

                    // Disable the Apply button
                    EnableWindow (hWndApply, FALSE);

                    return TRUE;    // We handled the msg

                case IDC_CUST_LB:
                    // Split cases based upon the notification code
                    switch (GET_WM_COMMAND_CMD (wParam, lParam))
                    {
                        case LBN_SELCHANGE:
                            // Get the index of the current selection
                            uSel = (UINT) SendMessageW (hWndListBox, LB_GETCURSEL, 0, 0);

                            // Save for next time
                            gInitCustomizeCategory = uSel;

                            // Get the associated item data (if any)
                            uIDD = SendMessageW (hWndListBox, LB_GETITEMDATA, uSel, 0);

                            // If it's an IDD_xxx value, ...
                            if (HIWORD (LODWORD (uIDD)) EQ 0)
                            {
                                hWndProp =
                                  CreateDialogParamW (_hInstance,
                                                      MAKEINTRESOURCEW (uIDD),
                                                      hDlg,
                                           (DLGPROC) &CustomizeDlgProc,
                                                      TRUE);        // Anything non-zero so as to
                                                                    //   distinguish it from the
                                                                    //   initial entry
                                // Save the window handle as the item data
                                SendMessageW (hWndListBox, LB_SETITEMDATA, uSel, (LPARAM) hWndProp);

                                // Tell the dialog to initialize the new page
                                PostMessageW (hDlg, MYWM_INITDIALOG, uSel, 0);
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

                    return TRUE;    // We handled the msg

                //***************************************************************
                // FONTS
                //***************************************************************

                case IDC_FONTS_RADIO2A:
                    // Set the local SameFontAs value
                    lclSameFontAs[1] = GET_WM_COMMAND_ID (wParam, lParam) - IDC_FONTS_RADIO2A;

                    // Disable the corresponding ChooseFont button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONT2, FALSE);

                    return TRUE;    // We handled the msg

                case IDC_FONTS_RADIO3A:
                case IDC_FONTS_RADIO3B:
                    // Set the local SameFontAs value
                    lclSameFontAs[2] = GET_WM_COMMAND_ID (wParam, lParam) - IDC_FONTS_RADIO3A;

                    // Disable the corresponding ChooseFont button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONT3, FALSE);

                    return TRUE;    // We handled the msg

                case IDC_FONTS_RADIO4A:
                case IDC_FONTS_RADIO4B:
                case IDC_FONTS_RADIO4C:
                    // Set the local SameFontAs value
                    lclSameFontAs[3] = GET_WM_COMMAND_ID (wParam, lParam) - IDC_FONTS_RADIO4A;

                    // Disable the corresponding ChooseFont button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONT4, FALSE);

                    return TRUE;    // We handled the msg

                case IDC_FONTS_RADIO5A:
                case IDC_FONTS_RADIO5B:
                case IDC_FONTS_RADIO5C:
                case IDC_FONTS_RADIO5D:
                    // Set the local SameFontAs value
                    lclSameFontAs[4] = GET_WM_COMMAND_ID (wParam, lParam) - IDC_FONTS_RADIO5A;

                    // Disable the corresponding ChooseFont button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONT5, FALSE);

                    return TRUE;    // We handled the msg

                case IDC_FONTS_RADIO6A:
                case IDC_FONTS_RADIO6B:
                case IDC_FONTS_RADIO6C:
                case IDC_FONTS_RADIO6D:
                case IDC_FONTS_RADIO6E:
                    // Set the local SameFontAs value
                    lclSameFontAs[5] = GET_WM_COMMAND_ID (wParam, lParam) - IDC_FONTS_RADIO6A;

                    // Disable the corresponding ChooseFont button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONT6, FALSE);

                    return TRUE;    // We handled the msg

                case IDC_FONTS_RADIO7A:
                case IDC_FONTS_RADIO7B:
                case IDC_FONTS_RADIO7C:
                case IDC_FONTS_RADIO7D:
                case IDC_FONTS_RADIO7E:
                case IDC_FONTS_RADIO7F:
                    // Set the local SameFontAs value
                    lclSameFontAs[6] = GET_WM_COMMAND_ID (wParam, lParam) - IDC_FONTS_RADIO7A;

                    // Disable the corresponding ChooseFont button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONT7, FALSE);

                    return TRUE;    // We handled the msg

                case IDC_FONTS_RADIO1A:
                    // Set the local SameFontAs value
                    lclSameFontAs[0] = GET_WM_COMMAND_ID (wParam, lParam) - IDC_FONTS_RADIO1A;

                    // Enable the corresponding ChooseFont button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONT1, TRUE);

                    return TRUE;    // We handled the msg

                case IDC_FONTS_RADIO2B:
                    // Set the local SameFontAs value
                    lclSameFontAs[1] = GET_WM_COMMAND_ID (wParam, lParam) - IDC_FONTS_RADIO2A;

                    // Enable the corresponding ChooseFont button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONT2, TRUE);

                    return TRUE;    // We handled the msg

                case IDC_FONTS_RADIO3C:
                    // Set the local SameFontAs value
                    lclSameFontAs[2] = GET_WM_COMMAND_ID (wParam, lParam) - IDC_FONTS_RADIO3A;

                    // Enable the corresponding ChooseFont button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONT3, TRUE);

                    return TRUE;    // We handled the msg

                case IDC_FONTS_RADIO4D:
                    // Set the local SameFontAs value
                    lclSameFontAs[3] = GET_WM_COMMAND_ID (wParam, lParam) - IDC_FONTS_RADIO4A;

                    // Enable the corresponding ChooseFont button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONT4, TRUE);

                    return TRUE;    // We handled the msg

                case IDC_FONTS_RADIO5E:
                    // Set the local SameFontAs value
                    lclSameFontAs[4] = GET_WM_COMMAND_ID (wParam, lParam) - IDC_FONTS_RADIO5A;

                    // Enable the corresponding ChooseFont button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONT5, TRUE);

                    return TRUE;    // We handled the msg

                case IDC_FONTS_RADIO6F:
                    // Set the local SameFontAs value
                    lclSameFontAs[5] = GET_WM_COMMAND_ID (wParam, lParam) - IDC_FONTS_RADIO6A;

                    // Enable the corresponding ChooseFont button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONT6, TRUE);

                    return TRUE;    // We handled the msg

                case IDC_FONTS_RADIO7G:
                    // Set the local SameFontAs value
                    lclSameFontAs[6] = GET_WM_COMMAND_ID (wParam, lParam) - IDC_FONTS_RADIO7A;

                    // Enable the corresponding ChooseFont button
                    SendMessageW (hDlg, MYWM_ENABLECHOOSEFONT, IDC_FONT7, TRUE);

                    return TRUE;    // We handled the msg

                case IDC_FONT1:
                case IDC_FONT2:
                case IDC_FONT3:
                case IDC_FONT4:
                case IDC_FONT5:
                case IDC_FONT6:
                case IDC_FONT7:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ GET_WM_COMMAND_CMD (wParam, lParam))
                    {
                        // Get the index of the font
                        uCnt = GET_WM_COMMAND_ID (wParam, lParam) - IDC_FONT1;

                        // Get the associated item data (window handle of the Property Page)
                        (HANDLE_PTR) hWndProp = SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_FONTS - IDD_PROPPAGE_START, 0);

                        // Get the ChooseFont button window handle
                        hWndFont = GetDlgItem (hWndProp, IDC_FONT1 + uCnt);

                        // Ask the user to choose a font, and keep track of
                        //   whether or not a font was chosen
                        // If the font changed, ...
                        if (ChooseFont (&fontStruc[uCnt].cfLcl))
                        {
                            // Mark as changed
                            fontStruc[uCnt].bChanged = TRUE;

                            // Set the text for the IDC_FONTx button upon set
                            SetWindowTextW (hWndFont, SET_BUTTON_TEXT);

                            // Enable the Apply button
                            EnableWindow (hWndApply, TRUE);
                        } // End IF

                        // Turn off the hilite state
                        // ***FIXME*** -- Why doesn't this work??
                        SendMessageW (hWndFont, BM_SETSTATE, FALSE, 0);
                    } // End IF

                    return FALSE;   // We didn't handle the msg

                //***************************************************************
                // RANGE LIMITED VARS
                //***************************************************************

                case IDC_RANGE_XB_CT:
                case IDC_RANGE_XB_IO:
                case IDC_RANGE_XB_IC:
                case IDC_RANGE_XB_PP:
                case IDC_RANGE_XB_PW:
                case IDC_RANGE_XB_RL:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ GET_WM_COMMAND_CMD (wParam, lParam))
                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    return TRUE;    // We handled the msg

                //***************************************************************
                // USER PREFERENCES
                //***************************************************************

                case IDC_USER_PREFS_XB_ADJUSTPW:
                case IDC_USER_PREFS_XB_UNDERBARTOLOWERCASE:
                case IDC_USER_PREFS_XB_NEWTABONCLEAR:
                case IDC_USER_PREFS_XB_NEWTABONLOAD:
                case IDC_USER_PREFS_XB_USELOCALTIME:
                case IDC_USER_PREFS_XB_BACKUPONLOAD:
                case IDC_USER_PREFS_XB_BACKUPONSAVE:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ GET_WM_COMMAND_CMD (wParam, lParam))
                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    return TRUE;    // We handled the msg

                case IDC_USER_PREFS_CB_DEFAULTPASTE:
                    // We care about CBN_SELCHANGE only
                    if (CBN_SELCHANGE EQ GET_WM_COMMAND_CMD (wParam, lParam))
                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    return TRUE;    // We handled the msg

                case IDC_USER_PREFS_CB_DEFAULTCOPY:
                    // We care about CBN_SELCHANGE only
                    if (CBN_SELCHANGE EQ GET_WM_COMMAND_CMD (wParam, lParam))
                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    return TRUE;    // We handled the msg

                //***************************************************************
                // CLEAR WS VALUES
                //***************************************************************

                case IDC_CLEARWS_ALX_EC:
                case IDC_CLEARWS_ELX_EC:
                case IDC_CLEARWS_FC_EC:
                case IDC_CLEARWS_LX_EC:
                    // We care about EN_CHANGE only
                    if (EN_CHANGE EQ GET_WM_COMMAND_CMD (wParam, lParam))
                    {
                        UINT    uID_EC;
                        UBOOL   bFC;
                        HWND    hWnd_EC,
                                hWnd_ST;

                        // Get the associated item data (window handle of the Property Page)
                        (HANDLE_PTR) hWndProp = SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_CLEARWS_VALUES - IDD_PROPPAGE_START, 0);

                        // Get the Edit Ctrl ID
                        uID_EC = GET_WM_COMMAND_ID (wParam, lParam);

                        // Get the Edit Ctrl & Static Text window handles
                        hWnd_EC = GetDlgItem (hWndProp, uID_EC    );
                        hWnd_ST = GetDlgItem (hWndProp, uID_EC + 1);

                        // Get the text length and format it into the static text on the right
                        uSel = (UINT) SendMessageW (hWnd_EC, EM_LINELENGTH, 0, 0);

                        // If this is the []FC Edit Ctrl and the length is not the expected value, ...
                        bFC = ((uID_EC EQ IDC_CLEARWS_FC_EC) && (uSel NE DEF_QUADFC_CWS_LEN));

                        // Format the text length
                        wsprintfW (wszTemp,
                                   L"Character Vector (%u)",
                                   uSel);
                        // Tell the Static Text control about the new font
                        SendMessageW (hWnd_ST,
                                      WM_SETFONT,
                                      (WPARAM) (bFC ? hFontBold_ST : hFontNorm_ST),
                                      MAKELPARAM (FALSE, 0));
                        // Display the message and text length
                        SetWindowTextW (hWnd_ST, wszTemp);

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    return TRUE;    // We handled the msg

                case IDC_CLEARWS_CT_EC:
                    // Split cases based upon the command
                    switch (GET_WM_COMMAND_CMD (wParam, lParam))
                    {
                        APLFLOAT fQuadVar;

                        case EN_CHANGE:
                            // Enable the Apply button
                            EnableWindow (hWndApply, TRUE);

                            break;

                        case EN_KILLFOCUS:
                            // If we're leaving this control, validate the number
                            //   and complain if not valid.

                            // Get the associated item data (window handle of the Property Page)
                            (HANDLE_PTR) hWndProp = SendMessageW (hWndListBox, LB_GETITEMDATA, IDD_PROPPAGE_CLEARWS_VALUES - IDD_PROPPAGE_START, 0);

                            // Tell the Edit Ctrl how big our buffer is
                            lpwszGlbTemp[0] = 1024;

                            // Get the text
                            SendMessageW (GetDlgItem (hWndProp, IDC_CLEARWS_CT_EC), EM_GETLINE, 0, (LPARAM) lpwszGlbTemp);

                            if (lpwszGlbTemp[0])
                            {
                                LPWCHAR wp;

                                // Ensure empty
                                wszTemp[0] = L'\0';

                                // Save as current ptr
                                wp = lpwszGlbTemp;

                                // Convert overbar to high minus
                                while (wp = strchrW (wp, UTF16_OVERBAR))
                                    *wp++ = L'-';

                                // Scan the value and one additional field to check for extra chars
                                swscanf (lpwszGlbTemp, L"%le%1ws", &fQuadVar, wszTemp);

                                // Ensure non-negative and no extra chars
                                if (fQuadVar >= 0
                                 && wszTemp[0] EQ L'\0')
                                    break;
                            } // End IF

                            // Display the error message
                            MessageBoxW (hWndProp, L"This Field is not a correctly formed non-negative number.  Please fix it before changing focus.", lpwszAppName, MB_OK | MB_ICONERROR);

                            // return the focus to the Edit Ctrl
                            SetFocus (GetDlgItem (hWndProp, IDC_CLEARWS_CT_EC));

                            break;

                        default:
                            break;
                    } // End SWITCH

                    return TRUE;    // We handled the msg

                case IDC_CLEARWS_IC_CB1:
                    // We care about CBN_SELCHANGE only
                    if (CBN_SELCHANGE EQ GET_WM_COMMAND_CMD (wParam, lParam))
                    {
                        // Get the changed index
                        uSel = (UINT) SendMessageW (hWndIC_CB1, CB_GETCURSEL, 0, 0);

                        // Display the corresponding selection
                        SendMessageW (hWndIC_CB2, CB_SETCURSEL, (APLU3264) icValues[uSel], 0);

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    return TRUE;    // We handled the msg

                case IDC_CLEARWS_IC_CB2:
                    // We care about CBN_SELCHANGE only
                    if (CBN_SELCHANGE EQ GET_WM_COMMAND_CMD (wParam, lParam))
                    {
                        // Get the current index
                        uSel = (UINT) SendMessageW (hWndIC_CB1, CB_GETCURSEL, 0, 0);

                        // Save as current value
                        icValues[uSel] = (UINT) SendMessageW (hWndIC_CB2, CB_GETCURSEL, 0, 0);

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    return TRUE;    // We handled the msg

                case IDC_CLEARWS_MF_CB:
                    // We care about CBN_SELCHANGE only
                    if (CBN_SELCHANGE EQ GET_WM_COMMAND_CMD (wParam, lParam))
                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);

                    return TRUE;    // We handled the msg

                //***************************************************************
                // SYSTEM VAR RESET
                //***************************************************************

                case IDC_RESET_CT_RADIO1:
                case IDC_RESET_CT_RADIO2:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ GET_WM_COMMAND_CMD (wParam, lParam))
                    {
                        // Save the new setting in our local copy
                        lclResetVars.CT = (IDC_RESET_CT_RADIO2 EQ GET_WM_COMMAND_ID (wParam, lParam));

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    return TRUE;    // We handled the msg

                case IDC_RESET_IC_RADIO1:
                case IDC_RESET_IC_RADIO2:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ GET_WM_COMMAND_CMD (wParam, lParam))
                    {
                        // Save the new setting in our local copy
                        lclResetVars.IC = (IDC_RESET_IC_RADIO2 EQ GET_WM_COMMAND_ID (wParam, lParam));

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    return TRUE;    // We handled the msg

                case IDC_RESET_FC_RADIO1:
                case IDC_RESET_FC_RADIO2:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ GET_WM_COMMAND_CMD (wParam, lParam))
                    {
                        // Save the new setting in our local copy
                        lclResetVars.FC = (IDC_RESET_FC_RADIO2 EQ GET_WM_COMMAND_ID (wParam, lParam));

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    return TRUE;    // We handled the msg

                case IDC_RESET_IO_RADIO1:
                case IDC_RESET_IO_RADIO2:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ GET_WM_COMMAND_CMD (wParam, lParam))
                    {
                        // Save the new setting in our local copy
                        lclResetVars.IO = (IDC_RESET_IO_RADIO2 EQ GET_WM_COMMAND_ID (wParam, lParam));

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    return TRUE;    // We handled the msg

                case IDC_RESET_PP_RADIO1:
                case IDC_RESET_PP_RADIO2:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ GET_WM_COMMAND_CMD (wParam, lParam))
                    {
                        // Save the new setting in our local copy
                        lclResetVars.PP = (IDC_RESET_PP_RADIO2 EQ GET_WM_COMMAND_ID (wParam, lParam));

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    return TRUE;    // We handled the msg

                case IDC_RESET_PW_RADIO1:
                case IDC_RESET_PW_RADIO2:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ GET_WM_COMMAND_CMD (wParam, lParam))
                    {
                        // Save the new setting in our local copy
                        lclResetVars.PW = (IDC_RESET_PW_RADIO2 EQ GET_WM_COMMAND_ID (wParam, lParam));

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    return TRUE;    // We handled the msg

                case IDC_RESET_RL_RADIO1:
                case IDC_RESET_RL_RADIO2:
                    // We care about BN_CLICKED only
                    if (BN_CLICKED EQ GET_WM_COMMAND_CMD (wParam, lParam))
                    {
                        // Save the new setting in our local copy
                        lclResetVars.RL = (IDC_RESET_RL_RADIO2 EQ GET_WM_COMMAND_ID (wParam, lParam));

                        // Enable the Apply button
                        EnableWindow (hWndApply, TRUE);
                    } // End IF

                    return TRUE;    // We handled the msg
            } // End SWITCH

            break;

        case WM_DESTROY:
            // Free allocated resources
            if (hFontBold_ST)
            {
                DeleteObject (hFontBold_ST); hFontBold_ST = NULL;
            } // End IF

            break;
    } // End SWITCH

    return FALSE;           // We didn't handle the msg
} // End CustomizeDlgProc


//***************************************************************************
//  $GetCLEARWSValue
//
//  Read in and save a CLEAR WS value
//***************************************************************************

void GetCLEARWSValue
    (HWND     hWndProp,                 // Dialog property page window handle
     UINT     uID,                      // Edit Ctrl ID
     HGLOBAL *lphGlbVal)                // Ptr to global memory handle

{
    UINT      uLen;                     // Temporary length
    APLNELM   aplNELMVal;               // NELM of object
    HGLOBAL   hGlbChr;                  // Global memory handle
    LPAPLCHAR lpMemChr;                 // Ptr to global memory
    APLUINT   ByteRes;                  // # bytes in the result

    // Tell the Edit Ctrl how big our buffer is
    lpwszGlbTemp[0] = 1024;

    // Get the text
    SendMessageW (GetDlgItem (hWndProp, uID), EM_GETLINE, 0, (LPARAM) lpwszGlbTemp);

    // Get the text length
    uLen = lstrlenW (lpwszGlbTemp);

    // Get the existing array attributes
    AttrsOfGlb (*lphGlbVal, NULL, &aplNELMVal, NULL, NULL);

    // If the lengths are the same and the reference count is 1, ...
    if (aplNELMVal EQ uLen
     && GetRefCntGlb (*lphGlbVal) EQ 1)
        // Lock the memory to get a ptr to it
        lpMemChr = MyGlobalLock (*lphGlbVal);
    else
    {
        // Calculate space needed for the result
        ByteRes = CalcArraySize (ARRAY_INT, uLen, 1);

        // Allocate space for the data
        // Note, we can't use DbgGlobalAlloc here as the
        //   PTD has not been allocated as yet
        hGlbChr = MyGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (hGlbChr)
        {
            // Free the current value
            DbgGlobalFree (*lphGlbVal); *lphGlbVal = NULL;

            // Save the new global memory handle
            *lphGlbVal = hGlbChr;

            // Lock the memory to get a ptr to it
            lpMemChr = MyGlobalLock (hGlbChr);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemChr)
            // Fill in the header values
            lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = ARRAY_CHAR;
////////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = uLen;
            lpHeader->Rank       = 1;
#undef  lpHeader
            // Save the dimension
            *VarArrayBaseToDim (lpMemChr) = uLen;
        } // End IF

        // Skip over the header and dimensions to the data
        lpMemChr = VarArrayBaseToData (lpMemChr, 1);

        // Copy the values
        CopyMemory (lpMemChr, lpwszGlbTemp, uLen * sizeof (lpwszGlbTemp[0]));

        // We no longer need this ptr
        MyGlobalUnlock (hGlbChr); lpMemChr = NULL;
    } // End IF/ELSE
} // End GetCLEARWSValue


//***************************************************************************
//  End of File: customize.c
//***************************************************************************
