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
    UINT         uHashMask,         // 2C:  Mask for all hash lookups
                 tkErrorCharIndex;  // 30:  Char index for lpwszQuadErrorMsg if ImmExec
    HGLOBAL      hGlbQuadDM;        // 34:  Quad-DM global memory handle

    HWND         hWndMC,            // 38:  MDI Client window handle
                 hWndSM,            // 3C:  Session Manager ...
                 hWndDB,            // 40:  Debugger     ...
                 hWndActive;        // 44:  Active MDI window when last switched out

    UINT         bTabTextState:1,   // 48:  00000001:  TRUE iff the tab's text state is Highlight, FALSE if Normal
                 bNegative:1,       //      00000002:  Sign bit for integer part
                 bNegExp:1,         //      00000004:  ...          exponent ...
                 Avail:29;          //      FFFFFFF8:  Available bits

    LPWCHAR      lpwszCurLine,      // 4C:  The contents of the line
                                    //      with the cursor on it.
                 lpwszTmpLine,      // 50:  Temporary holding area
                 lpwszErrorMessage; // 54:  Ptr to error message to signal
    LPWCHAR      lpwszQuadErrorMsg; // 58   Used for []ERROR messages
    LPCHAR       lpszNumAlp;        // 5C:  Accumulator for integers & floating points & names
    LPWCHAR      lpwszString;       // 60:  ...             strings
    UINT         uCaret;            // 64:  Position of the caret in the current line on error
    int          iMaxNumAlp,        // 68:  Maximum # chars in lpszNumAlp
                 iMaxString,        // 6C:  Maximum # WCHARs in lpwszString
                 iNumAlpLen,        // 70:  # chars in lpszNumAlp
                 iStringLen,        // 74:  ...        lpwszString
                 iLabelText,        // 78:  Offset in DPFE of the label text
                 crIndex;           // 7C:  Tab's color index

    // Accumulation vars for constant integer, floating point, and string
    APLINT       aplInteger;        // 80:  8-byte Integers

    LPSYMENTRY   steZero,           // 88:  Ptr to STE for constant zero
                 steOne,            // 8C:  ...                     one
                 steBlank,          // 90:  ...                     blank
                 steNoValue,        // 94:  ...            no-value result
                 lpSymQuadALX ,     // 98:  ...            []ALX
                 lpSymQuadCT  ,     // 9C:  ...            []CT
                 lpSymQuadELX ,     // A0:  ...            []ELX
                 lpSymQuadIO  ,     // A4:  ...            []IO
                 lpSymQuadLX  ,     // A8:  ...            []LX
                 lpSymQuadPP  ,     // AC:  ...            []PP
                 lpSymQuadPR  ,     // B0:  ...            []PR
                 lpSymQuadPW  ,     // B4:  ...            []PW
                 lpSymQuadRL  ,     // B8:  ...            []RL
                 lpSymQuadSA  ,     // BC:  ...            []SA
                 lpSymQuadWSID;     // C0:  ...            []WSID
    struct tagSIS_HEADER
                *lpSISBeg,          // C4:  Ptr to State Indicator Stack beginning
                *lpSISCur,          // C8:  ...                          current (may be NULL if SI is empty)
                *lpSISNxt;          // CC:  ...                          next
    struct tagPLLOCALVARS
                *lpPLCur;           // D0:  Ptr to current plLocalVars struct
                                    //      in thread creation order (NULL = none)
    WNDPROC lpfnOldListboxWndProc,  // D4:  Save area for old Listbox procedure
            lpfnOldEditCtrlWndProc; // D8:  Save area for old Edit Control procedure

    UINT         SILevel;           // DC:  Current State Indicator level
    HANDLE       hSemaDelay;        // E0:  Delay semaphore (NULL if no delay active)
    PL_YYSTYPE   YYResExec;         // E4:  Result from execute primitive

    APLCHAR      cQuadPR;           //114:  []PR     (' ') (When a char scalar)
    APLBOOL      bQuadxSA;          //116:  []SA (in its index form)
    char         DPFE[_MAX_PATH];   //117:  The Drive, Path, Filename, & Ext of the WS
                                    //21B:  Length
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
