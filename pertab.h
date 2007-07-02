//***************************************************************************
//  NARS2000 -- Per Tab Header
//***************************************************************************

#define INIT_PERTABVARS                                 \
    lpMemPTD->iMaxNumAlp       = DEF_NUMALP_MAXSIZE;    \
    lpMemPTD->iMaxString       = DEF_STRING_MAXSIZE;    \
    lpMemPTD->uHashMask        = DEF_HSHTAB_HASHMASK;   \
    lpMemPTD->iSymTabTotalSize = DEF_SYMTAB_INITSIZE;   \
    lpMemPTD->iHshTabTotalSize = DEF_HSHTAB_INITSIZE;   \
    lpMemPTD->iHshTabBaseSize  = DEF_HSHTAB_INITSIZE;   \
    lpMemPTD->iHshTabIncr      = DEF_HSHTAB_INCR;       \
    lpMemPTD->ExecCode         = EXEC_SUCCESS;          \
    lpMemPTD->bTabTextState    = FALSE;

// Structure for Per Tab Control Data
typedef struct tagPERTABDATA
{
    EXEC_CODES ExecCode;            // 00:  Exception code

#define NUMYYRES    10              // # YYRes elements in the array
    YYSTYPE YYRes[NUMYYRES];        // 04:  The result token

    // Symbol & hash table variables
    LPSYMENTRY lpSymTab,            // Ptr to start of symbol table
               lpSymTabNext;        // Ptr to next available STE
    int        iSymTabTotalSize,    // # STEs, currently
               iHshTabTotalSize,    // # HTEs, currently, including EPBs
               iHshTabBaseSize,     // Base size of hash table
               iHshTabIncr;         // Increment when looping through HT
    LPHSHENTRY lpHshTab,            // Ptr to start of hash table
               lpHshTabSplitNext;   // ...    next HTE to split (incremented by DEF_HSHTAB_NBLKS)
    UINT       uHashMask;           // Mask for all hash lookups

    APLBOOL bQuadxSA;               // []SA (in its index form)
    APLCHAR cQuadPR;                // []PR     (' ') (When a char scalar)

    HWND hWndMC,                    // MDI Client window handle
         hWndSM,                    // Session Manager ...
         hWndDB;                    // Debugger     ...

    BOOL bTabTextState;             // Tab's text state:  Highlight (TRUE) or Normal (FALSE)

    WNDPROC lpfnOldListboxWndProc,  // Save area for old Listbox procedure
            lpfnOldEditCtrlWndProc; // Save area for old Edit Control procedure

    LPWCHAR lpwszCurLine,           // The contents of the line
                                    //   with the cursor on it.
            lpwszTmpLine,           // Temporary holding area
            lpwszErrorMessage;      // Ptr to error message to signal
    LPCHAR  lpszNumAlp;             // Accumulator for integers & floating points & names
    LPWCHAR lpwszString;            // ...             strings
    int     iMaxNumAlp,             // Maximum # chars in lpszNumAlp
            iMaxString,             // Maximum # WCHARs in lpwszString
            iNumAlpLen,             // # chars in lpszNumAlp
            iStringLen;             // ...        lpwszString

    // Accumulation vars for constant integer, floating point, and string
    APLINT  aplInteger;             // 8-byte Integers
    BOOL    bNegative,              // Sign bit for integer part
            bNegExp;                // ...          exponent ...

    LPSYMENTRY steZero,             // Ptr to STE for constant zero
               steBlank,            // ...            ...      blank
               lpSymQuadALX ,       // ...            []ALX
               lpSymQuadCT  ,       // ...            []CT
               lpSymQuadELX ,       // ...            []ELX
               lpSymQuadIO  ,       // ...            []IO
               lpSymQuadLX  ,       // ...            []LX
               lpSymQuadPP  ,       // ...            []PP
               lpSymQuadPR  ,       // ...            []PR
               lpSymQuadPW  ,       // ...            []PW
               lpSymQuadRL  ,       // ...            []RL
               lpSymQuadSA  ,       // ...            []SA
               lpSymQuadWSID;       // ...            []WSID

    int        iLabelText;          // Offset in DPFE of the label text
    HWND       hWndActive;          // Active MDI window when last switched out
    int        crIndex;             // Tab's color index
    char       DPFE[_MAX_PATH];     // The Drive, Path, Filename, & Ext of the WS
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
