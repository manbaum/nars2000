//***************************************************************************
//  NARS2000 -- Per Tab Header
//***************************************************************************

#define INIT_PERTABVARS                                     \
    lpMemPTD->hGlbQuadDM         = hGlbMTChar;              \
    lpMemPTD->iMaxNumAlp         = DEF_NUMALP_MAXSIZE;      \
    lpMemPTD->iMaxString         = DEF_STRING_MAXSIZE;      \
    lpMemPTD->uHashMask          = DEF_HSHTAB_HASHMASK;     \
    lpMemPTD->iSymTabTotalSize   = DEF_SYMTAB_INITSIZE;     \
    lpMemPTD->iHshTabTotalSize   = DEF_HSHTAB_INITSIZE;     \
    lpMemPTD->iHshTabBaseSize    = DEF_HSHTAB_INITSIZE;     \
    lpMemPTD->iHshTabIncr        = DEF_HSHTAB_INCR;         \
    lpMemPTD->ExecCode           = EXEC_SUCCESS;            \
    lpMemPTD->execState.exType   = EX_IMM;

    // The following vars are already initialized to zero which is the default
    //   value, so they do not need to be explicitly set.
////lpMemPTD->numYYRes           = 0;
////lpMemPTD->SILevel            = 0;
////lpMemPTD->bTabTextState      = FALSE;
////lpMemPTD->bNegative          = FALSE;
////lpMemPTD->bNegExp            = FALSE;

// Structure for Per Tab Control Data
typedef struct tagPERTABDATA
{
    EXEC_CODES   ExecCode;          // 00:  Exception code
    LPPL_YYSTYPE lpYYRes;           // 04:  The result token
    UINT         numYYRes;          // 08:  # YYRes elements in the array
                                    //      pointed to by lpYYRes
    EXEC_STATE   execState;         // 0C:  Execution state

    // Symbol & hash table variables
    LPSYMENTRY   lpSymTab,          // 10:  Ptr to start of symbol table
                 lpSymTabNext;      // 14:  Ptr to next available STE
    int          iSymTabTotalSize,  // 18:  # STEs, currently
                 iHshTabTotalSize,  // 1C:  # HTEs, currently, including EPBs
                 iHshTabBaseSize,   // 20:  Base size of hash table
                 iHshTabIncr;       // 24:  Increment when looping through HT
    LPHSHENTRY   lpHshTab,          // 28:  Ptr to start of hash table
                 lpHshTabSplitNext; // 2C:  ...    next HTE to split (incremented by DEF_HSHTAB_NBLKS)
    UINT         uHashMask;         // 30:  Mask for all hash lookups

    HGLOBAL      hGlbQuadDM;        // 34:  Quad-DM global memory handle

    HWND         hWndMC,            // 38:  MDI Client window handle
                 hWndSM,            // 3C:  Session Manager ...
                 hWndDB,            // 40:  Debugger     ...
                 hWndActive;        // 44:  Active MDI window when last switched out

    BOOL         bTabTextState:1,   // 48:  00000001:  TRUE iff the tab's text state is Highlight, FALSE if Normal
                 bNegative:1,       //      00000002:  Sign bit for integer part
                 bNegExp:1,         //      00000004:  ...          exponent ...
                 Avail:29;          //      FFFFFFF8:  Available bits

    LPWCHAR      lpwszCurLine,      // 4C:  The contents of the line
                                    //      with the cursor on it.
                 lpwszTmpLine,      // 50:  Temporary holding area
                 lpwszErrorMessage; // 54:  Ptr to error message to signal
    LPCHAR       lpszNumAlp;        // 58:  Accumulator for integers & floating points & names
    LPWCHAR      lpwszString;       // 5C:  ...             strings
    UINT         uCaret;            // 60:  Position of the caret in the current line on error
    int          iMaxNumAlp,        // 64:  Maximum # chars in lpszNumAlp
                 iMaxString,        // 68:  Maximum # WCHARs in lpwszString
                 iNumAlpLen,        // 6C:  # chars in lpszNumAlp
                 iStringLen,        // 70:  ...        lpwszString
                 iLabelText,        // 74:  Offset in DPFE of the label text
                 crIndex;           // 78:  Tab's color index

    // Accumulation vars for constant integer, floating point, and string
    APLINT       aplInteger;        // 7C:  8-byte Integers

    LPSYMENTRY   steZero,           // 84:  Ptr to STE for constant zero
                 steBlank,          // 88: ...            ...      blank
                 steNoValue,        // 8C: ...            no-value result
                 lpSymQuadALX ,     // 90: ...            []ALX
                 lpSymQuadCT  ,     // 94: ...            []CT
                 lpSymQuadELX ,     // 98: ...            []ELX
                 lpSymQuadIO  ,     // 9C: ...            []IO
                 lpSymQuadLX  ,     // A0: ...            []LX
                 lpSymQuadPP  ,     // A4: ...            []PP
                 lpSymQuadPR  ,     // A8: ...            []PR
                 lpSymQuadPW  ,     // AC: ...            []PW
                 lpSymQuadRL  ,     // B0: ...            []RL
                 lpSymQuadSA  ,     // B4: ...            []SA
                 lpSymQuadWSID;     // B8: ...            []WSID
    struct tagSIS_HEADER
                *lpSISBeg,          // BC:  Ptr to State Indicator Stack beginning
                *lpSISCur,          // C0:  ...                          current (may be NULL if SI is empty)
                *lpSISNxt;          // C4:  ...                          next

    WNDPROC lpfnOldListboxWndProc,  // C8:  Save area for old Listbox procedure
            lpfnOldEditCtrlWndProc; // CC:  Save area for old Edit Control procedure

    UINT         SILevel;           // D0:  State Indicator level

    APLBOOL      bQuadxSA;          // D4:  []SA (in its index form)
    APLCHAR      cQuadPR;           // D5:  []PR     (' ') (When a char scalar)
    char         DPFE[_MAX_PATH];   // D7:  The Drive, Path, Filename, & Ext of the WS
                                    //1??:  Length
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
