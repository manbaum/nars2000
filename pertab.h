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
////lpMemPTD->iNumLim            = 0;
////lpMemPTD->iStrLim            = 0;
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
                 bExecLX:1,         //      00000008:  TRUE iff execute []LX after successful load
                 Avail:28;          //      FFFFFFF0:  Available bits
    HGLOBAL      hGlbCurLine,       // 5C:  Current line global memory handle
                 hGlbNum,           // 60:  NumAlp global memory handle
                 hGlbStr;           // 64:  NumAlp global memory handle
    LPWCHAR      lpwszErrorMessage; // 68:  Ptr to error message to signal
    LPWCHAR      lpwszQuadErrorMsg; // 6C   Used for []ERROR/[]ES messages
    UINT         uCaret;            // 70:  Position of the caret in the current line on error
    int          iNumLen,           // 74:  # chars in lpszNumAlp
                 iStrLen,           // 88:  ...        lpwszString
                 iNumLim,           // 8C:  Current limit for lpszNumAlp
                 iStrLim,           // 90:  ...               lpwszString
                 crIndex;           // 94:  Tab's color index

    // Accumulation vars for constant integer, floating point, and string
    APLINT       aplInteger;        // 94:  8-byte Integers

    LPSYMENTRY   steZero,           // 9C:  Ptr to STE for constant zero
                 steOne,            // A0:  ...                     one
                 steBlank,          // A4:  ...                     blank
                 steNoValue,        // A8:  ...            no-value result
                 lpSymQuadALX ,     // AC:  ...            []ALX
                 lpSymQuadCT  ,     // B0:  ...            []CT
                 lpSymQuadELX ,     // B4:  ...            []ELX
                 lpSymQuadFC  ,     // B8:  ...            []FC
                 lpSymQuadIC  ,     // BC:  ...            []IC
                 lpSymQuadIO  ,     // C0:  ...            []IO
                 lpSymQuadLX  ,     // C4:  ...            []LX
                 lpSymQuadPP  ,     // C8:  ...            []PP
                 lpSymQuadPR  ,     // CC:  ...            []PR
                 lpSymQuadPW  ,     // D0:  ...            []PW
                 lpSymQuadRL  ,     // D4:  ...            []RL
                 lpSymQuadSA  ,     // D8:  ...            []SA
                 lpSymQuadWSID;     // DC:  ...            []WSID
    struct tagSIS_HEADER
                *lpSISBeg,          // E0:  Ptr to State Indicator Stack beginning
                *lpSISCur,          // E4:  ...                          current (may be NULL if SI is empty)
                *lpSISNxt;          // E8:  ...                          next
    struct tagPLLOCALVARS
                *lpPLCur;           // EC:  Ptr to current plLocalVars struct
                                    //      in thread creation order (NULL = none)
    WNDPROC lpfnOldListboxWndProc,  // F0:  Save area for old Listbox procedure
            lpfnOldEditCtrlWndProc; // F4:  Save area for old Edit Control procedure

    // Magic function global memory handles
    HGLOBAL      hGlbMF_MonIota,    // F8:  Extended Monadic Iota
                 hGlbMF_DydIota,    // FC:  Extended Dyadic Iota
                 hGlbMF_MonUpShoe,  //100:  Monadic UpShoe
                 hGlbMF_DydTilde,   //104:  Dyadic Tilde
                 hGlbMF_MonRank,    //108:  Monadic Rank
                 hGlbMF_DydRank,    //10C:  Dyadic Rank
                 hGlbMF_Conform;    //110:  Conform for Rank operator

    UINT         SILevel,           //114:  Current State Indicator level
                 CurTabID,          //118:  ID of the corresponding tab
                 PrvTabID;          //11C:  ID of the preceding tab (from which we were loaded)
    HANDLE       hSemaDelay;        //120:  Delay semaphore (NULL if no delay active)
    EXIT_TYPES   ImmExecExitType;   //124:  ImmExec exit type (see EXIT_TYPES)
    PL_YYSTYPE   YYResExec;         //128:  Result from execute primitive
                                    //      Size = 38h for DEBUG, 28h otherwise
    LPPL_YYSTYPE lpStrand[STRAND_LEN];//160:  Ptrs to strand accumulators in parser
    APLCHAR      cQuadPR;           //164:  []PR     (' ') (When a char scalar)
    APLBOOL      bQuadxSA;          //166:  []SA (in its index form)
                                    //167:  Length
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
