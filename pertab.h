//***************************************************************************
//  NARS2000 -- Per Tab Data Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2009 Sudley Place Software

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

#ifndef UNISCRIBE
  #define COBJMACROS
  #include <mlang.h>
#endif
#include "workspace.h"

#define INIT_PERTABVARS                                         \
    lpMemPTD->Sig.nature              = ' DTP';                 \
    lpMemPTD->hGlbQuadDM              = hGlbV0Char;             \
    lpMemPTD->uQuadMF                 = uQuadMF_CWS;            \
    lpMemPTD->iSymTabTotalSize        = DEF_SYMTAB_INITSIZE;    \
    lpMemPTD->htsPTD.uHashMask        = DEF_HSHTAB_HASHMASK;    \
    lpMemPTD->htsPTD.iHshTabTotalSize = DEF_HSHTAB_INITSIZE;    \
    lpMemPTD->htsPTD.iHshTabBaseSize  = DEF_HSHTAB_INITSIZE;    \
    lpMemPTD->htsPTD.iHshTabIncr      = DEF_HSHTAB_INCR;        \
    lpMemPTD->htsPTD.iHshTabEPB       = DEF_HSHTAB_EPB;         \
    lpMemPTD->htsPTD.iHshTabIncrSize  = DEF_HSHTAB_INCRSIZE;    \
    lpMemPTD->ExceptionCode           = EXCEPTION_SUCCESS;

    // The following vars are already initialized to zero which is the default
    //   value from GHND, so they do not need to be set explicitly.
////lpMemPTD->numYYRes           = 0;
////lpMemPTD->bTabTextState      = FALSE;
////lpMemPTD->lpSISCur           = NULL;
////lpMemPTD->lpPLCur            = NULL;
////lpMemPTD->SILevel            = 0;
////lpMemPTD->hSemaDelay         = NULL;
////lpMemPTD->lpLstMVS           = NULL;
#ifndef UNISCRIBE
////lpMemPTD->lpFontLink         = NULL;
#endif

// Structure for Per Tab Control Data
typedef struct tagPERTABDATA
{
    HEADER_SIGNATURE Sig;           // 00:  Signature
    EXCEPTION_CODES ExceptionCode;  // 04:  Exception code
    LPUCHAR      ExceptionAddr;     // 08:  ...       address
    LPWCHAR      ExceptionText;     // 0C:  ...       text
    LPUCHAR      lpInvalidAddr;     // 10:  Ptr to invalid address (if ExceptionCode EQ EXCEPTION_ACCESS_VIOLATION)
    LPPL_YYSTYPE lpYYRes;           // 14:  The result token
    UINT         numYYRes;          // 18:  # YYRes elements in the array
                                    //      pointed to by lpYYRes

    // Symbol & hash table variables
    LPSYMENTRY   lpSymTab,          // 1C:  Ptr to start of symbol table
                 lpSymTabNext;      // 20:  Ptr to next available STE
    int          iSymTabTotalSize;  // 24:  # STEs, currently
    HSHTABSTR    htsPTD;            // 28:  HshTab struc (32 bytes)
    UINT         tkErrorCharIndex;  // 48:  Char index for lpwszQuadErrorMsg if ImmExec
    HGLOBAL      hGlbQuadDM;        // 4C:  Quad-DM global memory handle

    HWND         hWndMC,            // 50:  MDI Client window handle
                 hWndSM,            // 54:  Session Manager ...
                 hWndDB,            // 58:  Debugger     ...
                 hWndActive;        // 5C:  Active MDI window when last switched out
    UINT         bTabTextState:1,   // 60:  00000001:  TRUE iff the tab's text state is Highlight, FALSE if Normal
                 bExecLX:1,         //      00000002:  TRUE iff execute []LX after successful load
                 bExecuting:1,      //      00000004:  TRUE iff we're waiting for an execution to complete
                 Avail:29;          //      FFFFFFF8:  Available bits
    HGLOBAL      hGlbCurLine;       // 64:  Current line global memory handle
    LPWCHAR      lpwszErrorMessage; // 68:  Ptr to error message to signal
    LPWCHAR      lpwszQuadErrorMsg; // 6C   Used for []ERROR/[]ES messages
    UINT         uCaret;            // 70:  Position of the caret in the current line on error
    int          crIndex;           // 74:  Tab's color index

    // Accumulation vars for constant integer, floating point, and string
    APLINT       uQuadMF;           // 78:  []MF timer value (8 bytes)

    LPSYMENTRY   steZero,           // 7C:  Ptr to STE for constant zero
                 steOne,            // 80:  ...                     one
                 steBlank,          // 84:  ...                     blank
                 steNoValue,        // 88:  ...            no-value result
                 lpSymQuadALX ,     // 8C:  ...            []ALX
                 lpSymQuadCT  ,     // 90:  ...            []CT
                 lpSymQuadELX ,     // 94:  ...            []ELX
                 lpSymQuadFC  ,     // 98:  ...            []FC
                 lpSymQuadIC  ,     // 9C:  ...            []IC
                 lpSymQuadIO  ,     // A0:  ...            []IO
                 lpSymQuadLX  ,     // A4:  ...            []LX
                 lpSymQuadPP  ,     // A8:  ...            []PP
                 lpSymQuadPR  ,     // AC:  ...            []PR
                 lpSymQuadPW  ,     // B0:  ...            []PW
                 lpSymQuadRL  ,     // B4:  ...            []RL
                 lpSymQuadSA  ,     // B8:  ...            []SA
                 lpSymQuadWSID,     // BC:  ...            []WSID
                 lpSymQuadZ   ;     // C0:  ...            []Z
    struct tagSIS_HEADER
                *lpSISBeg,          // C4:  Ptr to State Indicator Stack beginning
                *lpSISCur,          // C8:  ...                          current (may be NULL if SI is empty)
                *lpSISNxt;          // CC:  ...                          next
    struct tagPLLOCALVARS
                *lpPLCur;           // D0:  Ptr to current plLocalVars struct
                                    //      in thread creation order (NULL = none)
    WNDPROC lpfnOldListboxWndProc,  // D4:  Save area for old Listbox procedure
            lpfnOldEditCtrlWndProc; // D8:  Save area for old Edit Ctrl procedure

    // Magic function global memory handles
    HGLOBAL      hGlbMF_MonIota,    // DC:  Extended Monadic Iota
                 hGlbMF_DydIota,    // E0:  Extended Dyadic Iota
                 hGlbMF_MonUpShoe,  // E4:  Monadic UpShoe
                 hGlbMF_DydTilde,   // E8:  Dyadic Tilde
                 hGlbMF_MonRank,    // EC:  Monadic Rank
                 hGlbMF_DydRank,    // F0:  Dyadic Rank
                 hGlbMF_Conform;    // F4:  Conform for Rank operator

    UINT         SILevel,           // F8:  Current State Indicator level
                 CurTabID,          // FC:  ID of the corresponding tab
                 PrvTabID;          //100:  ID of the preceding tab (from which we were loaded)
    HANDLE       hSemaDelay;        //104:  Delay semaphore (NULL if no delay active)
    EXIT_TYPES   ImmExecExitType;   //108:  ImmExec exit type (see EXIT_TYPES)
    PL_YYSTYPE   YYResExec,         //10C:  Result from execute primitive
                                    //      Size = 2Ch for DEBUG, 20h otherwise
                 YYCaseExec;        //138:  Result from CASE arg execution
                                    //      Size = 2Ch for DEBUG, 20h otherwise
    LPPL_YYSTYPE lpStrand[STRAND_LEN];//164:  Ptrs to strand accumulators in parser (4 bytes each)
    LPLOADWSGLBVARCONV
                 lpLoadWsGlbVarConv;//168:  Ptr to function to convert a FMTSTR_GLBOBJ to an HGLOBAL
    LPLOADWSGLBVARPARM
                 lpLoadWsGlbVarParm;//16C:  Ptr to extra parms for LoadWsGlbVarConv
    LPMEMVIRTSTR lpLstMVS;          //170:  Ptr to last MEMVIRTSTR (NULL = none)
    LPWCHAR      lpwszFormat,       //174:  Ptr to formatting save area
                 lpwszBaseTemp,     //178:  Ptr to base of lpwszTemp
                 lpwszTemp;         //17C:  Ptr to temporary  ...
    UINT         uTempMaxSize,      //180:  Maximum size of lpwszTemp
                 RegisterEBP,       //184:  Register EBP from an exception
                 uErrLine;          //188:  Error line # from []FX for )IN
    LPTOKEN      lptkCSIni,         //18C:  Ptr to start of CtrlStruc token stack (static)
                 lptkCSNxt;         //190:  Ptr to next available slot in CS ...  (dynamic)
    struct tagFORSTMT *
                 lpForStmtBase;     //194:  Ptr to base of FORSTMT stack
#ifndef UNISCRIBE
    IMLangFontLink
                *lpFontLink;        //198:  Ptr to FontLink struc
#endif
    APLCHAR      cQuadPR,           //19C:  []PR     (' ') (When a char scalar)
                 cQuadxSA;          //19E:  []SA     (0)   (in its index form as an integer)
    DWORD        dwThreadId;        //1A0:  Corresponding thread ID
                                    //1A4:  Length
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
