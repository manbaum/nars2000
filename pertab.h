//***************************************************************************
//  NARS2000 -- Per Tab Header
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

#define INIT_PERTABVARS                                     \
    lpMemPTD->Sig.nature         = ' DTP';                  \
    lpMemPTD->hGlbQuadDM         = hGlbV0Char;              \
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
    LPUCHAR      ExceptionAddr;     // 04:  ...       address
    LPCHAR       ExceptionText;     // ??:  ...       text
    LPUCHAR      lpInvalidAddr;     // ??:  Ptr to invalid address (if ExceptionCode EQ EXCEPTION_ACCESS_VIOLATION)
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
    LPWCHAR      lpwszQuadErrorMsg; // 5C   Used for []ERROR/[]ES messages
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
                 lpSymQuadFC  ,     // A8:  ...            []FC
                 lpSymQuadIC  ,     // AC:  ...            []IC
                 lpSymQuadIO  ,     // B0:  ...            []IO
                 lpSymQuadLX  ,     // B4:  ...            []LX
                 lpSymQuadPP  ,     // B8:  ...            []PP
                 lpSymQuadPR  ,     // BC:  ...            []PR
                 lpSymQuadPW  ,     // C0:  ...            []PW
                 lpSymQuadRL  ,     // C4:  ...            []RL
                 lpSymQuadSA  ,     // C8:  ...            []SA
                 lpSymQuadWSID;     // CC:  ...            []WSID
    struct tagSIS_HEADER
                *lpSISBeg,          // D0:  Ptr to State Indicator Stack beginning
                *lpSISCur,          // D4:  ...                          current (may be NULL if SI is empty)
                *lpSISNxt;          // D8:  ...                          next
    struct tagPLLOCALVARS
                *lpPLCur;           // DC:  Ptr to current plLocalVars struct
                                    //      in thread creation order (NULL = none)
    WNDPROC lpfnOldListboxWndProc,  // E0:  Save area for old Listbox procedure
            lpfnOldEditCtrlWndProc; // E4:  Save area for old Edit Control procedure

    // Magic function global memory handles
    HGLOBAL      hGlbMF_MonIota,    // E8:  Extended Monadic Iota
                 hGlbMF_DydIota,    // EC:  Extended Dyadic Iota
                 hGlbMF_MonUpShoe,  // F0:  Monadic UpShoe
                 hGlbMF_DydTilde,   // F4:  Dyadic Tilde
                 hGlbMF_MonRank,    // F8:  Monadic Rank
                 hGlbMF_DydRank,    // FC:  Dyadic Rank
                 hGlbMF_MonRankAxis,//100:  Monadic Rank
                 hGlbMF_DydRankAxis,//104:  Dyadic Rank
                 hGlbMF_Conform;    //108:  Conform for Rank operator

    UINT         SILevel;           //10C:  Current State Indicator level
    HANDLE       hSemaDelay;        //110:  Delay semaphore (NULL if no delay active)
    EXIT_TYPES   ImmExecExitType;   //114:  ImmExec exit type (see EXIT_TYPES)
    PL_YYSTYPE   YYResExec;         //118:  Result from execute primitive

    APLCHAR      cQuadPR;           //148:  []PR     (' ') (When a char scalar)
    APLBOOL      bQuadxSA;          //14A:  []SA (in its index form)
    char         DPFE[_MAX_PATH];   //14B:  The Drive, Path, Filename, & Ext of the WS
                                    //24F:  Length
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
