//***************************************************************************
//  NARS2000 -- Threads Header
//***************************************************************************

typedef struct tagCNT_THREAD
{
    HWND   hWndParent;          // Window handle of the parent
    LPSTR  lpszDPFE;            // Drive, Path, Filename, Ext of the workspace
    int    iTab;                // Insert the new tab to the left of this one
    HANDLE hThread;             // Handle to the current thread
} CNT_THREAD, *LPCNT_THREAD;

typedef struct tagCDB_THREAD
{
    HGLOBAL hGlbPTD;            // Handle to PerTabData
    HANDLE  hTimer;             // Handle to waitable timer
} CDB_THREAD, *LPCDB_THREAD;


//***************************************************************************
//  End of File: threads.c
//***************************************************************************
