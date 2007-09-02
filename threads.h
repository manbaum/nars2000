//***************************************************************************
//  NARS2000 -- Threads Header
//***************************************************************************

typedef struct tagCNT_THREAD    // Thread struct for CreateNewTabInThread
{
    HWND   hWndParent;          // 00:  Window handle of the parent
    LPSTR  lpszDPFE;            // 04:  Drive, Path, Filename, Ext of the workspace
    int    iTab;                // 08:  Insert the new tab to the left of this one
    HANDLE hThread;             // 0C:  Handle to the current thread
                                // 10:  Length
} CNT_THREAD, *LPCNT_THREAD;


typedef struct tagUTJ_THREAD    // Thread struct for PrimFnMonUpTackJotInThread
{
    HWND    hWndEC;             // 00:  Handle of Edit Control window
    HGLOBAL hGlbPTD;            // 04:  PerTabData global memory handle
    LPWCHAR lpwszCompLine;      // 08:  Ptr to complete line
    HANDLE  hSemaReset;         // 0C:  Semaphore Handle for )RESET
                                // 10:  Length
} UTJ_THREAD, *LPUTJ_THREAD;


typedef struct tagIE_THREAD     // Thread struct for ImmExecLineInThread
{
    HWND    hWndEC;             // 00:  Handle of Edit Control window
    HGLOBAL hGlbPTD;            // 04:  PerTabData global memory handle
    LPWCHAR lpwszCompLine;      // 08:  Ptr to complete line
    HGLOBAL hGlbWFSO;           // 0C:  WaitForSingleObject callback global memory handle
    BOOL    bFreeLine,          // 10:  TRUE iff we should free lpwszCompLine on completion
            bWaitUntilFini;     // 14:  TRUE iff wait until finished
    HANDLE  hSemaReset;         // 18:  Semaphore Handle for )RESET
                                // 1C:  Length
} IE_THREAD, *LPIE_THREAD;


typedef struct tagCDB_THREAD    // Thread strucr for CreateDebuggerInThread
{
    HGLOBAL hGlbPTD;            // 00:  PerTabData global memory handle
                                // 04:  Length
} CDB_THREAD, *LPCDB_THREAD;


//***************************************************************************
//  End of File: threads.c
//***************************************************************************
