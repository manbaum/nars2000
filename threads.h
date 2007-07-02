//***************************************************************************
//  NARS2000 -- Threads Header
//***************************************************************************

typedef struct tagCNT_THREAD
{
    HWND   hWndParent;          // 00:  Window handle of the parent
    LPSTR  lpszDPFE;            // 04:  Drive, Path, Filename, Ext of the workspace
    int    iTab;                // 08:  Insert the new tab to the left of this one
    HANDLE hThread;             // 0C:  Handle to the current thread
                                // 10:  Length
} CNT_THREAD, *LPCNT_THREAD;

typedef struct tagCDB_THREAD
{
    HGLOBAL hGlbPTD;            // 00:  Handle to PerTabData
    HANDLE  hTimer;             // 04:  Handle to waitable timer
                                // 08:  Length
} CDB_THREAD, *LPCDB_THREAD;


//***************************************************************************
//  End of File: threads.c
//***************************************************************************
