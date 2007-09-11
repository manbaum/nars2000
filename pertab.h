//***************************************************************************
//  NARS2000 -- Per Tab Header
//***************************************************************************

#define INIT_PERTABVARS                                     \
    lpMemPTD->Sig.nature         = ' DTP';                  \
    lpMemPTD->hGlbQuadDM         = hGlbMTChar;              \
    lpMemPTD->iMaxNumAlp         = DEF_NUMALP_MAXSIZE;      \
    lpMemPTD->iMaxString         = DEF_STRING_MAXSIZE;      \
    lpMemPTD->uHashMask          = DEF_HSHTAB_HASHMASK;     \
    lpMemPTD->iSymTabTotalSize   = DEF_SYMTAB_INITSIZE;     \
    lpMemPTD->iHshTabTotalSize   = DEF_HSHTAB_INITSIZE;     \
    lpMemPTD->iHshTabBaseSize    = DEF_HSHTAB_INITSIZE;     \
    lpMemPTD->iHshTabIncr        = DEF_HSHTAB_INCR;         \
    lpMemPTD->ExceptionCode      = EXCEPTION_SUCCESS;

    // The following vars are already initialized to zero which is the default
    //   value, so they do not need to be explicitly set.
////lpMemPTD->numYYRes           = 0;
////lpMemPTD->bTabTextState      = FALSE;
////lpMemPTD->bNegative          = FALSE;
////lpMemPTD->bNegExp            = FALSE;
////lpMemPTD->lpSISCur           = NULL;
////lpMemPTD->lpPLCur            = NULL;
////lpMemPTD->SILevel            = 0;
////lpmemPTD->hSemaDelay         = NULL;

// Structure for Per Tab Control Data
typedef struct tagPERTABDATA
{
    HEADER_SIGNATURE Sig;           // 00:  Signature
    EXCEPTION_CODES ExceptionCode;  // 00:  Exception code
    LPPL_YYSTYPE lpYYRes;           // 04:  The result token
    UINT         numYYRes;          // 08:  # YYRes elements in the array
                                    //      pointed to by lpYYRes

    // Symbol & hash table variables
    LPSYMENTRY   lpSymTab,          // 0C:  Ptr to start of symbol table
                 lpSymTabNext;      // 10:  Ptr to next available STE
    int          iSymTabTotalSize,  // 14:  # STEs, currently
                 iHshTabTotalSize,  // 18:  # HTEs, currently, including EPBs
                 iHshTabBaseSize,   // 1C:  Base size of hash table
                 iHshTabIncr;       // 20:  Increment when looping through HT
    LPHSHENTRY   lpHshTab,          // 24:  Ptr to start of hash table
                 lpHshTabSplitNext; // 28:  ...    next HTE to split (incremented by DEF_HSHTAB_NBLKS)
    UINT         uHashMask;         // 2C:  Mask for all hash lookups

    HGLOBAL      hGlbQuadDM;        // 30:  Quad-DM global memory handle

    HWND         hWndMC,            // 34:  MDI Client window handle
                 hWndSM,            // 38:  Session Manager ...
                 hWndDB,            // 3C:  Debugger     ...
                 hWndActive;        // 40:  Active MDI window when last switched out

    UINT         bTabTextState:1,   // 44:  00000001:  TRUE iff the tab's text state is Highlight, FALSE if Normal
                 bNegative:1,       //      00000002:  Sign bit for integer part
                 bNegExp:1,         //      00000004:  ...          exponent ...
                 Avail:29;          //      FFFFFFF8:  Available bits

    LPWCHAR      lpwszCurLine,      // 48:  The contents of the line
                                    //      with the cursor on it.
                 lpwszTmpLine,      // 4C:  Temporary holding area
                 lpwszErrorMessage; // 50:  Ptr to error message to signal
    LPCHAR       lpszNumAlp;        // 54:  Accumulator for integers & floating points & names
    LPWCHAR      lpwszString;       // 58:  ...             strings
    UINT         uCaret;            // 5C:  Position of the caret in the current line on error
    int          iMaxNumAlp,        // 60:  Maximum # chars in lpszNumAlp
                 iMaxString,        // 64:  Maximum # WCHARs in lpwszString
                 iNumAlpLen,        // 68:  # chars in lpszNumAlp
                 iStringLen,        // 6C:  ...        lpwszString
                 iLabelText,        // 70:  Offset in DPFE of the label text
                 crIndex;           // 74:  Tab's color index

    // Accumulation vars for constant integer, floating point, and string
    APLINT       aplInteger;        // 78:  8-byte Integers

    LPSYMENTRY   steZero,           // 80:  Ptr to STE for constant zero
                 steOne,            // 84:  ...                     one
                 steBlank,          // 88:  ...                     blank
                 steNoValue,        // 8C:  ...            no-value result
                 lpSymQuadALX ,     // 90:  ...            []ALX
                 lpSymQuadCT  ,     // 94:  ...            []CT
                 lpSymQuadELX ,     // 98:  ...            []ELX
                 lpSymQuadIO  ,     // 9C:  ...            []IO
                 lpSymQuadLX  ,     // A0:  ...            []LX
                 lpSymQuadPP  ,     // A4:  ...            []PP
                 lpSymQuadPR  ,     // A8:  ...            []PR
                 lpSymQuadPW  ,     // AC:  ...            []PW
                 lpSymQuadRL  ,     // B0:  ...            []RL
                 lpSymQuadSA  ,     // B4:  ...            []SA
                 lpSymQuadWSID;     // B8:  ...            []WSID
    struct tagSIS_HEADER
                *lpSISBeg,          // BC:  Ptr to State Indicator Stack beginning
                *lpSISCur,          // C0:  ...                          current (may be NULL if SI is empty)
                *lpSISNxt;          // C4:  ...                          next
    struct tagPLLOCALVARS
                *lpPLCur;           // C8:  Ptr to current plLocalVars struct
                                    //      in thread creation order (NULL = none)
    WNDPROC lpfnOldListboxWndProc,  // CC:  Save area for old Listbox procedure
            lpfnOldEditCtrlWndProc; // D0:  Save area for old Edit Control procedure

    UINT         SILevel;           // D4:  Current State Indicator level
    HANDLE       hSemaDelay;        // D8:  Delay semaphore (NULL if no delay active)
    PL_YYSTYPE   YYResExec;         // D8:  Result from execute primitive

    APLCHAR      cQuadPR;           //108:  []PR     (' ') (When a char scalar)
    APLBOOL      bQuadxSA;          //10A:  []SA (in its index form)
    char         DPFE[_MAX_PATH];   //10B:  The Drive, Path, Filename, & Ext of the WS
                                    //20F:  Length
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
