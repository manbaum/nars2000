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
    EXCEPTION_CODES ExceptionCode;  // 04:  Exception code
    LPUCHAR      ExceptionAddr;     // 08:  ...       address
    LPCHAR       ExceptionText;     // 0C:  ...       text
    LPUCHAR      lpInvalidAddr;     // 10:  Ptr to invalid address (if ExceptionCode EQ EXCEPTION_ACCESS_VIOLATION)
    LPPL_YYSTYPE lpYYRes;           // 14:  The result token
    UINT         numYYRes;          // 18:  # YYRes elements in the array
                                    //      pointed to by lpYYRes

    // Symbol & hash table variables
    LPSYMENTRY   lpSymTab,          // 1C:  Ptr to start of symbol table
                 lpSymTabNext;      // 20:  Ptr to next available STE
    int          iSymTabTotalSize,  // 24:  # STEs, currently
                 iHshTabTotalSize,  // 28:  # HTEs, currently, including EPBs
                 iHshTabBaseSize,   // 2C:  Base size of hash table
                 iHshTabIncr;       // 30:  Increment when looping through HT
    LPHSHENTRY   lpHshTab,          // 34:  Ptr to start of hash table
                 lpHshTabSplitNext; // 38:  ...    next HTE to split (incremented by DEF_HSHTAB_NBLKS)
    UINT         uHashMask,         // 3C:  Mask for all hash lookups
                 tkErrorCharIndex;  // 40:  Char index for lpwszQuadErrorMsg if ImmExec
    HGLOBAL      hGlbQuadDM;        // 44:  Quad-DM global memory handle

    HWND         hWndMC,            // 48:  MDI Client window handle
                 hWndSM,            // 4C:  Session Manager ...
                 hWndDB,            // 50:  Debugger     ...
                 hWndActive;        // 54:  Active MDI window when last switched out

    UINT         bTabTextState:1,   // 58:  00000001:  TRUE iff the tab's text state is Highlight, FALSE if Normal
                 bNegative:1,       //      00000002:  Sign bit for integer part
                 bNegExp:1,         //      00000004:  ...          exponent ...
                 Avail:29;          //      FFFFFFF8:  Available bits

    LPWCHAR      lpwszCurLine,      // 5C:  The contents of the line
                                    //      with the cursor on it.
                 lpwszTmpLine,      // 60:  Temporary holding area
                 lpwszErrorMessage; // 64:  Ptr to error message to signal
    LPWCHAR      lpwszQuadErrorMsg; // 68   Used for []ERROR/[]ES messages
    LPCHAR       lpszNumAlp;        // 6C:  Accumulator for integers & floating points & names
    LPWCHAR      lpwszString;       // 70:  ...             strings
    UINT         uCaret;            // 74:  Position of the caret in the current line on error
    int          iMaxNumAlp,        // 78:  Maximum # chars in lpszNumAlp
                 iMaxString,        // 7C:  Maximum # WCHARs in lpwszString
                 iNumAlpLen,        // 80:  # chars in lpszNumAlp
                 iStringLen,        // 84:  ...        lpwszString
                 crIndex;           // 88:  Tab's color index

    // Accumulation vars for constant integer, floating point, and string
    APLINT       aplInteger;        // 8C:  8-byte Integers

    LPSYMENTRY   steZero,           // 94:  Ptr to STE for constant zero
                 steOne,            // 98:  ...                     one
                 steBlank,          // 9C:  ...                     blank
                 steNoValue,        // A0:  ...            no-value result
                 lpSymQuadALX ,     // A4:  ...            []ALX
                 lpSymQuadCT  ,     // A8:  ...            []CT
                 lpSymQuadELX ,     // AC:  ...            []ELX
                 lpSymQuadFC  ,     // B0:  ...            []FC
                 lpSymQuadIC  ,     // B4:  ...            []IC
                 lpSymQuadIO  ,     // B8:  ...            []IO
                 lpSymQuadLX  ,     // BC:  ...            []LX
                 lpSymQuadPP  ,     // C0:  ...            []PP
                 lpSymQuadPR  ,     // C4:  ...            []PR
                 lpSymQuadPW  ,     // C8:  ...            []PW
                 lpSymQuadRL  ,     // CC:  ...            []RL
                 lpSymQuadSA  ,     // D0:  ...            []SA
                 lpSymQuadWSID;     // D4:  ...            []WSID
    struct tagSIS_HEADER
                *lpSISBeg,          // D8:  Ptr to State Indicator Stack beginning
                *lpSISCur,          // DC:  ...                          current (may be NULL if SI is empty)
                *lpSISNxt;          // E0:  ...                          next
    struct tagPLLOCALVARS
                *lpPLCur;           // E4:  Ptr to current plLocalVars struct
                                    //      in thread creation order (NULL = none)
    WNDPROC lpfnOldListboxWndProc,  // E8:  Save area for old Listbox procedure
            lpfnOldEditCtrlWndProc; // EC:  Save area for old Edit Control procedure

    // Magic function global memory handles
    HGLOBAL      hGlbMF_MonIota,    // F0:  Extended Monadic Iota
                 hGlbMF_DydIota,    // F4:  Extended Dyadic Iota
                 hGlbMF_MonUpShoe,  // F8:  Monadic UpShoe
                 hGlbMF_DydTilde,   // FC:  Dyadic Tilde
                 hGlbMF_MonRank,    //100:  Monadic Rank
                 hGlbMF_DydRank,    //104:  Dyadic Rank
                 hGlbMF_Conform;    //108:  Conform for Rank operator

    UINT         SILevel,           //10C:  Current State Indicator level
                 CurTabIndex,       //110:  Index of the corresponding tab
                 PrvTabIndex;       //114:  Index of the preceding tab (from which we were loaded)
    HANDLE       hSemaDelay;        //118:  Delay semaphore (NULL if no delay active)
    EXIT_TYPES   ImmExecExitType;   //11C:  ImmExec exit type (see EXIT_TYPES)
    PL_YYSTYPE   YYResExec;         //120:  Result from execute primitive
                                    //      Size = 38h for DEBUG, 28h otherwise
    APLCHAR      cQuadPR;           //158:  []PR     (' ') (When a char scalar)
    APLBOOL      bQuadxSA;          //15A:  []SA (in its index form)
                                    //15B:  Length
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
