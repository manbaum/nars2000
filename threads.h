//***************************************************************************
//  NARS2000 -- Threads Header
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

typedef struct tagCNT_THREAD    // Thread struct for CreateNewTabInThread
{
    HWND    hWndParent;         // 00:  Window handle of the parent
    HGLOBAL hGlbDPFE;           // 04:  Workspace DPFE global memory handle
    int     iTab;               // 08:  Insert the new tab to the left of this one
    BOOL    bExecLX;            // 0C:  TRUE iff execute []LX after successful load
    HANDLE  hThread;            // 10:  Handle to the current thread
                                // 14:  Length
} CNT_THREAD, *LPCNT_THREAD;


typedef struct tagUTJ_THREAD    // Thread struct for PrimFnMonUpTackJotInThread
{
    HWND    hWndEC;             // 00:  Handle of Edit Control window
    HGLOBAL hGlbPTD;            // 04:  PerTabData global memory handle
    LPWCHAR lpwszCompLine;      // 08:  Ptr to complete line
                                // 0C:  Length
} UTJ_THREAD, *LPUTJ_THREAD;


typedef struct tagIE_THREAD     // Thread struct for ImmExecStmtInThread
{
    HWND    hWndEC;             // 00:  Handle of Edit Control window
    HGLOBAL hGlbPTD;            // 04:  PerTabData global memory handle
    LPWCHAR lpwszCompLine;      // 08:  Ptr to complete line
    HGLOBAL hGlbWFSO;           // 0C:  WaitForSingleObject callback global memory handle
    BOOL    bFreeLine,          // 10:  TRUE iff we should free lpwszCompLine on completion
            bWaitUntilFini;     // 14:  TRUE iff wait until finished
                                // 18:  Length
} IE_THREAD, *LPIE_THREAD;


typedef struct tagCDB_THREAD    // Thread strucr for CreateDebuggerInThread
{
    HGLOBAL hGlbPTD;            // 00:  PerTabData global memory handle
                                // 04:  Length
} CDB_THREAD, *LPCDB_THREAD;


//***************************************************************************
//  End of File: threads.c
//***************************************************************************
