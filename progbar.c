//***************************************************************************
//  NARS2000 -- Progress Bar Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2012 Sudley Place Software

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
#include "headers.h"


typedef struct tagPBWD
{
    RECT rcClient;      // Client rectangle
    UINT uCurPos,       // Current position
         uState,        // Current state (PBST_xxx)
         uMinVal,       // Minimum value of the range
         uMaxVal,       // Maximum ...
         uStep;         // Step value
} PBWD, *LPPBWD;


//***************************************************************************
//  $PB_Create
//
//  Perform window-specific initialization
//***************************************************************************

void PB_Create
    (HWND hWnd)

{
} // End PB_Create


//***************************************************************************
//  $PB_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void PB_Delete
    (HWND hWnd)

{
} // End PB_Delete


//***************************************************************************
//  $PBWndProc
//
//  Message processing routine for the Progress Bar window
//***************************************************************************

LRESULT APIENTRY PBWndProc
    (HWND   hWnd,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    HGLOBAL hGlbWD;     // Window data global memory handle
    LPPBWD  lpMemWD;    // Ptr to per-window data global memory
    UINT    uCurPos,    // Current position
            uState,     // ...     state
            uStep;      // ...     step
    DWORD   dwRange;    // Current range

////LCLODSAPI ("PB: ", hWnd, message, wParam, lParam);
    switch (message)
    {
        case WM_CREATE:
            // Allocate global memory for the per window data
            hGlbWD = GlobalAlloc (GHND, sizeof (PBWD));
            if (!hGlbWD)
                return -1;          // Fail the create

            // Save it for later use
            SetWindowLongPtrW (hWnd, GWLPB_HGLB, (HANDLE_PTR) hGlbWD);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Initialize the struct
            lpMemWD->uCurPos = 0;
            lpMemWD->uState  = PBST_NORMAL;
            lpMemWD->uMinVal = 0;
            lpMemWD->uMaxVal = 100;
            lpMemWD->uStep   = 10;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            // Initialize window-specific resources
            PB_Create (hWnd);

            break;

        case PBM_GETSTEP:
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Get the current step
            uStep = lpMemWD->uState;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            return uStep;           // Return the current step

        case PBM_SETSTEP:           // uStep = (UINT) wParam;
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Save the old current step
            uStep = lpMemWD->uCurPos;

            // Save the current step
            lpMemWD->uStep = (UINT) wParam;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            // Redraw the window
            InvalidateRect (hWnd, NULL, FALSE);

            return uStep;           // Return the previous current step

        case PBM_GETPOS:
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Get the current position
            uCurPos = lpMemWD->uCurPos;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            return uCurPos;         // Return the current position

        case PBM_SETPOS:            // uCurPos = (UINT) wParam;
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Save the old current position
            uCurPos = lpMemWD->uCurPos;

            // Save the current position
            lpMemWD->uCurPos = (UINT) wParam;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            // Redraw the window
            InvalidateRect (hWnd, NULL, FALSE);

            return uCurPos;         // Return the previous current position

        case PBM_GETSTATE:
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Get the current state
            uState = lpMemWD->uState;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            return uState;          // Return the current state

        case PBM_SETSTATE:          // uState = (UINT) wParam
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Save the old current state
            uState = lpMemWD->uState;

            // Save the current state
            lpMemWD->uState = (UINT) wParam;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            // Redraw the window
            InvalidateRect (hWnd, NULL, FALSE);

            return uState;          // Return the previous current state

        case PBM_SETRANGE32:        // uMinVal = (UINT) wParam;
                                    // uMaxVal = (UINT) lParam;
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Save the old current range
            dwRange = (DWORD) MAKELONG (lpMemWD->uMinVal, lpMemWD->uMaxVal);

            // Save the new range
            lpMemWD->uMinVal = (UINT) wParam;
            lpMemWD->uMaxVal = (UINT) lParam;

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            // Redraw the window
            InvalidateRect (hWnd, NULL, FALSE);

            return dwRange;         // Return the previous range

        case WM_ERASEBKGND:
            // In order to reduce screen flicker, we handle erase background
            // in the WM_PAINT message for the child windows.
            return TRUE;            // We erased the background

        case WM_PAINT:              // hdc = (HDC) wParam; // the device context to draw in
        {
            RECT        rcUpdate,   // Update rectangle
                        rcClient;   // Client ...
            PAINTSTRUCT ps;         // Paint struct
            COLORREF    crTop;      // Background color at the top

            // Get the update rectangle
            if (!GetUpdateRect (hWnd, &rcUpdate, FALSE))
                return FALSE;       // We handled the msg

            // If there's no incoming HDC, ...
            if (((HDC) wParam) EQ NULL)
                // Layout the drop cloths
                BeginPaint (hWnd, &ps);
            else
                ps.hdc = (HDC) wParam;
            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // Lock the memory to get a ptr to it
            lpMemWD = GlobalLock (hGlbWD);

            // Get the client rect
            GetClientRect (hWnd, &rcClient);

            // If the min != max and min > max, ...
            if (lpMemWD->uMinVal < lpMemWD->uMaxVal)
            {
                // Split cases based upon the PB state
                switch (lpMemWD->uState)
                {
                    case PBST_ERROR:
                        // Set the color
                        crTop = DEF_SCN_RED;

                        break;

                    case PBST_PAUSED:
                        // Set the color
                        crTop = DEF_SCN_DARKGOLDENROD;

                        break;

                    case PBST_NORMAL:
                    default:
                        // Set the color
                        crTop = DEF_SCN_GREEN;

                        break;
                } // End SWITCH

                // Make a copy of the client rect
                CopyRect (&rcUpdate, &rcClient);

                // Scale the current position to the client rectangle
                // And set as left half of the PB
                rcUpdate.right = rcClient.left
                               + ((rcClient.right - rcClient.left)
                                * (lpMemWD->uCurPos - lpMemWD->uMinVal))
                                 / (lpMemWD->uMaxVal - lpMemWD->uMinVal);
                // Fill in the top left half of the PB
                rcUpdate.bottom = (rcUpdate.bottom + rcUpdate.top) / 2;
                FillTabBackground (ps.hdc, &rcUpdate, crTop        , DEF_SCN_WHITE);

                // Fill in the bottom left half of the PB
                rcUpdate.top    = rcUpdate.bottom;
                rcUpdate.bottom = rcClient.bottom;
                FillTabBackground (ps.hdc, &rcUpdate, DEF_SCN_WHITE, crTop        );

                // Make rectangle for right half of PB
                rcUpdate.top   = rcClient.top;
                rcUpdate.left  = rcUpdate.right;
                rcUpdate.right = rcClient.right;

                // Fill in the right half of the PB
                //   with a background brush
                FillRect (ps.hdc, &rcUpdate, GetSysColorBrush (COLOR_BTNFACE));
            } // End IF

            // We no longer need this resource
            GlobalUnlock (hGlbWD); lpMemWD = NULL;

            // If there's no incoming HDC, ...
            if (((HDC) wParam) EQ NULL)
                // Break out the turpentine
                EndPaint (hWnd, &ps);
            return FALSE;           // We handled the msg
        } // End WM_PAINT

        case WM_DESTROY:
            // Uninitialize window-specific resources
            PB_Delete (hWnd);

            // Get the per-window data global memory handle
            (HANDLE_PTR) hGlbWD = GetWindowLongPtrW (hWnd, GWLPB_HGLB);

            // We no longer need this storage
            GlobalFree (hGlbWD); hGlbWD = NULL; SetWindowLongPtrW (hWnd, GWLPB_HGLB, (HANDLE_PTR) hGlbWD);

            return FALSE;           // We handled the msg

        default:
            break;
    } // End SWITCH

////LCLODSAPI ("PBZ:", hWnd, message, wParam, lParam);
    return DefWindowProcW (hWnd, message, wParam, lParam);
} // End PBWndProc


//***************************************************************************
//  End of File: progressbar.c
//***************************************************************************
