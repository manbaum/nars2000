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
    LPCHAR       ExceptionText;     // 08:  ...       text
    LPUCHAR      lpInvalidAddr;     // 0C:  Ptr to invalid address (if ExceptionCode EQ EXCEPTION_ACCESS_VIOLATION)
    LPPL_YYSTYPE lpYYRes;           // 10:  The result token
    UINT         numYYRes;          // 14:  # YYRes elements in the array
                                    //      pointed to by lpYYRes

    // Symbol & hash table variables
    LPSYMENTRY   lpSymTab,          // 18:  Ptr to start of symbol table
                 lpSymTabNext;      // 1C:  Ptr to next available STE
    int          iSymTabTotalSize,  // 20:  # STEs, currently
                 iHshTabTotalSize,  // 24:  # HTEs, currently, including EPBs
                 iHshTabBaseSize,   // 28:  Base size of hash table
                 iHshTabIncr;       // 2C:  Increment when looping through HT
    LPHSHENTRY   lpHshTab,          // 30:  Ptr to start of hash table
                 lpHshTabSplitNext; // 34:  ...    next HTE to split (incremented by DEF_HSHTAB_NBLKS)
    UINT         uHashMask,         // 38:  Mask for all hash lookups
                 tkErrorCharIndex;  // 3C:  Char index for lpwszQuadErrorMsg if ImmExec
    HGLOBAL      hGlbQuadDM;        // 40:  Quad-DM global memory handle

    HWND         hWndMC,            // 44:  MDI Client window handle
                 hWndSM,            // 48:  Session Manager ...
                 hWndDB,            // 4C:  Debugger     ...
                 hWndActive;        // 50:  Active MDI window when last switched out

    UINT         bTabTextState:1,   // 54:  00000001:  TRUE iff the tab's text state is Highlight, FALSE if Normal
                 bNegative:1,       //      00000002:  Sign bit for integer part
                 bNegExp:1,         //      00000004:  ...          exponent ...
                 Avail:29;          //      FFFFFFF8:  Available bits

    LPWCHAR      lpwszCurLine,      // 58:  The contents of the line
                                    //      with the cursor on it.
                 lpwszTmpLine,      // 5C:  Temporary holding area
                 lpwszErrorMessage; // 60:  Ptr to error message to signal
    LPWCHAR      lpwszQuadErrorMsg; // 64   Used for []ERROR/[]ES messages
    LPCHAR       lpszNumAlp;        // 68:  Accumulator for integers & floating points & names
    LPWCHAR      lpwszString;       // 6C:  ...             strings
    UINT         uCaret;            // 70:  Position of the caret in the current line on error
    int          iMaxNumAlp,        // 74:  Maximum # chars in lpszNumAlp
                 iMaxString,        // 78:  Maximum # WCHARs in lpwszString
                 iNumAlpLen,        // 7C:  # chars in lpszNumAlp
                 iStringLen,        // 80:  ...        lpwszString
                 crIndex;           // 84:  Tab's color index

    // Accumulation vars for constant integer, floating point, and string
    APLINT       aplInteger;        // 88:  8-byte Integers

    LPSYMENTRY   steZero,           // 90:  Ptr to STE for constant zero
                 steOne,            // 94:  ...                     one
                 steBlank,          // 98:  ...                     blank
                 steNoValue,        // 9C:  ...            no-value result
                 lpSymQuadALX ,     // A0:  ...            []ALX
                 lpSymQuadCT  ,     // A4:  ...            []CT
                 lpSymQuadELX ,     // A8:  ...            []ELX
                 lpSymQuadFC  ,     // AC:  ...            []FC
                 lpSymQuadIC  ,     // B0:  ...            []IC
                 lpSymQuadIO  ,     // B4:  ...            []IO
                 lpSymQuadLX  ,     // B8:  ...            []LX
                 lpSymQuadPP  ,     // BC:  ...            []PP
                 lpSymQuadPR  ,     // C0:  ...            []PR
                 lpSymQuadPW  ,     // C4:  ...            []PW
                 lpSymQuadRL  ,     // C8:  ...            []RL
                 lpSymQuadSA  ,     // CC:  ...            []SA
                 lpSymQuadWSID;     // D0:  ...            []WSID
    struct tagSIS_HEADER
                *lpSISBeg,          // D4:  Ptr to State Indicator Stack beginning
                *lpSISCur,          // D8:  ...                          current (may be NULL if SI is empty)
                *lpSISNxt;          // DC:  ...                          next
    struct tagPLLOCALVARS
                *lpPLCur;           // E0:  Ptr to current plLocalVars struct
                                    //      in thread creation order (NULL = none)
    WNDPROC lpfnOldListboxWndProc,  // E4:  Save area for old Listbox procedure
            lpfnOldEditCtrlWndProc; // E8:  Save area for old Edit Control procedure

    // Magic function global memory handles
    HGLOBAL      hGlbMF_MonIota,    // EC:  Extended Monadic Iota
                 hGlbMF_DydIota,    // F0:  Extended Dyadic Iota
                 hGlbMF_MonUpShoe,  // F4:  Monadic UpShoe
                 hGlbMF_DydTilde,   // F8:  Dyadic Tilde
                 hGlbMF_MonRank,    // FC:  Monadic Rank
                 hGlbMF_DydRank,    //100:  Dyadic Rank
                 hGlbMF_Conform;    //104:  Conform for Rank operator

    UINT         SILevel,           //108:  Current State Indicator level
                 TabIndex;          //10C:  Index of the corresponding tab
    HANDLE       hSemaDelay;        //110:  Delay semaphore (NULL if no delay active)
    EXIT_TYPES   ImmExecExitType;   //114:  ImmExec exit type (see EXIT_TYPES)
    PL_YYSTYPE   YYResExec;         //118:  Result from execute primitive
                                    //      Size = 38h for DEBUG, 28h otherwise
    APLCHAR      cQuadPR;           //150:  []PR     (' ') (When a char scalar)
    APLBOOL      bQuadxSA;          //152:  []SA (in its index form)
                                    //153:  Length
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
