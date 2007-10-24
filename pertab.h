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
    UINT         ExceptionAddr;     // 04:  ...       address
    LPCHAR       ExceptionText;     // ??:  ...       text
    LPPL_YYSTYPE lpYYRes;           // 08:  The result token
    UINT         numYYRes;          // 0C:  # YYRes elements in the array
                                    //      pointed to by lpYYRes

    // Symbol & hash table variables
    LPSYMENTRY   lpSymTab,          // 10:  Ptr to start of symbol table
                 lpSymTabNext;      // 14:  Ptr to next available STE
    int          iSymTabTotalSize,  // 18:  # STEs, currently
                 iHshTabTotalSize,  // 1C:  # HTEs, currently, including EPBs
                 iHshTabBaseSize,   // 20:  Base size of hash table
                 iHshTabIncr;       // 24:  Increment when looping through HT
    LPHSHENTRY   lpHshTab,          // 28:  Ptr to start of hash table
                 lpHshTabSplitNext; // 2C:  ...    next HTE to split (incremented by DEF_HSHTAB_NBLKS)
    UINT         uHashMask,         // 30:  Mask for all hash lookups
                 tkErrorCharIndex;  // 34:  Char index for lpwszQuadErrorMsg if ImmExec
    HGLOBAL      hGlbQuadDM;        // 38:  Quad-DM global memory handle

    HWND         hWndMC,            // 3C:  MDI Client window handle
                 hWndSM,            // 40:  Session Manager ...
                 hWndDB,            // 44:  Debugger     ...
                 hWndActive;        // 48:  Active MDI window when last switched out

    UINT         bTabTextState:1,   // 4C:  00000001:  TRUE iff the tab's text state is Highlight, FALSE if Normal
                 bNegative:1,       //      00000002:  Sign bit for integer part
                 bNegExp:1,         //      00000004:  ...          exponent ...
                 Avail:29;          //      FFFFFFF8:  Available bits

    LPWCHAR      lpwszCurLine,      // 50:  The contents of the line
                                    //      with the cursor on it.
                 lpwszTmpLine,      // 54:  Temporary holding area
                 lpwszErrorMessage; // 58:  Ptr to error message to signal
    LPWCHAR      lpwszQuadErrorMsg; // 5C   Used for []ERROR messages
    LPCHAR       lpszNumAlp;        // 60:  Accumulator for integers & floating points & names
    LPWCHAR      lpwszString;       // 64:  ...             strings
    UINT         uCaret;            // 68:  Position of the caret in the current line on error
    int          iMaxNumAlp,        // 6C:  Maximum # chars in lpszNumAlp
                 iMaxString,        // 70:  Maximum # WCHARs in lpwszString
                 iNumAlpLen,        // 74:  # chars in lpszNumAlp
                 iStringLen,        // 78:  ...        lpwszString
                 iLabelText,        // 7C:  Offset in DPFE of the label text
                 crIndex;           // 80:  Tab's color index

    // Accumulation vars for constant integer, floating point, and string
    APLINT       aplInteger;        // 84:  8-byte Integers

    LPSYMENTRY   steZero,           // 8C:  Ptr to STE for constant zero
                 steOne,            // 90:  ...                     one
                 steBlank,          // 94:  ...                     blank
                 steNoValue,        // 98:  ...            no-value result
                 lpSymQuadALX ,     // 9C:  ...            []ALX
                 lpSymQuadCT  ,     // A0:  ...            []CT
                 lpSymQuadELX ,     // A4:  ...            []ELX
                 lpSymQuadIO  ,     // A8:  ...            []IO
                 lpSymQuadLX  ,     // AC:  ...            []LX
                 lpSymQuadPP  ,     // B0:  ...            []PP
                 lpSymQuadPR  ,     // B4:  ...            []PR
                 lpSymQuadPW  ,     // B8:  ...            []PW
                 lpSymQuadRL  ,     // BC:  ...            []RL
                 lpSymQuadSA  ,     // C0:  ...            []SA
                 lpSymQuadWSID;     // C4:  ...            []WSID
    struct tagSIS_HEADER
                *lpSISBeg,          // C8:  Ptr to State Indicator Stack beginning
                *lpSISCur,          // CC:  ...                          current (may be NULL if SI is empty)
                *lpSISNxt;          // D0:  ...                          next
    struct tagPLLOCALVARS
                *lpPLCur;           // D4:  Ptr to current plLocalVars struct
                                    //      in thread creation order (NULL = none)
    WNDPROC lpfnOldListboxWndProc,  // D8:  Save area for old Listbox procedure
            lpfnOldEditCtrlWndProc; // DC:  Save area for old Edit Control procedure

    UINT         SILevel;           // E0:  Current State Indicator level
    HANDLE       hSemaDelay;        // E4:  Delay semaphore (NULL if no delay active)
    PL_YYSTYPE   YYResExec;         // E8:  Result from execute primitive

    APLCHAR      cQuadPR;           //118:  []PR     (' ') (When a char scalar)
    APLBOOL      bQuadxSA;          //11A:  []SA (in its index form)
    char         DPFE[_MAX_PATH];   //11B:  The Drive, Path, Filename, & Ext of the WS
                                    //21F:  Length
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
