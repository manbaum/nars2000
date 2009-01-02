//***************************************************************************
//  NARS2000 -- Per Tab Data Header
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
////lpMemPTD->iNumLim            = 0;
////lpMemPTD->iStrLim            = 0;
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
    HGLOBAL      hGlbCurLine,       // 64:  Current line global memory handle
                 hGlbNum,           // 68:  NumAlp global memory handle
                 hGlbStr;           // 6C:  NumAlp global memory handle
    LPWCHAR      lpwszErrorMessage; // 70:  Ptr to error message to signal
    LPWCHAR      lpwszQuadErrorMsg; // 74   Used for []ERROR/[]ES messages
    UINT         uCaret;            // 78:  Position of the caret in the current line on error
    int          iNumLen,           // 7C:  # chars in lpszNumAlp
                 iStrLen,           // 80:  ...        lpwszString
                 iNumLim,           // 84:  Current limit for lpszNumAlp
                 iStrLim,           // 88:  ...               lpwszString
                 crIndex;           // 8C:  Tab's color index

    // Accumulation vars for constant integer, floating point, and string
    APLINT       aplInteger,        // 90:  Temporary Integer (8 bytes)
                 uQuadMF;           // 98:  []MF timer value (8 bytes)

    LPSYMENTRY   steZero,           // A0:  Ptr to STE for constant zero
                 steOne,            // A4:  ...                     one
                 steBlank,          // A8:  ...                     blank
                 steNoValue,        // AC:  ...            no-value result
                 lpSymQuadALX ,     // B0:  ...            []ALX
                 lpSymQuadCT  ,     // B4:  ...            []CT
                 lpSymQuadELX ,     // B8:  ...            []ELX
                 lpSymQuadFC  ,     // BC:  ...            []FC
                 lpSymQuadIC  ,     // C0:  ...            []IC
                 lpSymQuadIO  ,     // C4:  ...            []IO
                 lpSymQuadLX  ,     // C8:  ...            []LX
                 lpSymQuadPP  ,     // CC:  ...            []PP
                 lpSymQuadPR  ,     // D0:  ...            []PR
                 lpSymQuadPW  ,     // D4:  ...            []PW
                 lpSymQuadRL  ,     // D8:  ...            []RL
                 lpSymQuadSA  ,     // DC:  ...            []SA
                 lpSymQuadWSID,     // E0:  ...            []WSID
                 lpSymQuadZ   ;     // E4:  ...            []Z
    struct tagSIS_HEADER
                *lpSISBeg,          // E8:  Ptr to State Indicator Stack beginning
                *lpSISCur,          // EC:  ...                          current (may be NULL if SI is empty)
                *lpSISNxt;          // F0:  ...                          next
    struct tagPLLOCALVARS
                *lpPLCur;           // F4:  Ptr to current plLocalVars struct
                                    //      in thread creation order (NULL = none)
    WNDPROC lpfnOldListboxWndProc,  // F8:  Save area for old Listbox procedure
            lpfnOldEditCtrlWndProc; // FC:  Save area for old Edit Ctrl procedure

    // Magic function global memory handles
    HGLOBAL      hGlbMF_MonIota,    //100:  Extended Monadic Iota
                 hGlbMF_DydIota,    //104:  Extended Dyadic Iota
                 hGlbMF_MonUpShoe,  //108:  Monadic UpShoe
                 hGlbMF_DydTilde,   //10C:  Dyadic Tilde
                 hGlbMF_MonRank,    //110:  Monadic Rank
                 hGlbMF_DydRank,    //114:  Dyadic Rank
                 hGlbMF_Conform;    //118:  Conform for Rank operator

    UINT         SILevel,           //11C:  Current State Indicator level
                 CurTabID,          //120:  ID of the corresponding tab
                 PrvTabID;          //124:  ID of the preceding tab (from which we were loaded)
    HANDLE       hSemaDelay;        //128:  Delay semaphore (NULL if no delay active)
    EXIT_TYPES   ImmExecExitType;   //12C:  ImmExec exit type (see EXIT_TYPES)
    PL_YYSTYPE   YYResExec;         //130:  Result from execute primitive
                                    //      Size = 2Ch for DEBUG, 20h otherwise
    LPPL_YYSTYPE lpStrand[STRAND_LEN];//15C:  Ptrs to strand accumulators in parser (4 bytes each)
    LPLOADWSGLBVARCONV
                 lpLoadWsGlbVarConv;//164:  Ptr to function to convert a FMTSTR_GLBOBJ to an HGLOBAL
    LPLOADWSGLBVARPARM
                 lpLoadWsGlbVarParm;//168:  Ptr to extra parms for LoadWsGlbVarConv
    LPMEMVIRTSTR lpLstMVS;          //16C:  Ptr to last MEMVIRTSTR (NULL = none)
    LPWCHAR      lpwszFormat,       //170:  Ptr to formatting save area
                 lpwszBaseTemp,     //174:  Ptr to base of lpwszTemp
                 lpwszTemp;         //178:  Ptr to temporary  ...
    UINT         uTempMaxSize,      //17C:  Maximum size of lpwszTemp
                 RegisterEBP,       //180:  Register EBP from an exception
                 uErrLine;          //184:  Error line # from []FX for )IN
    LPTOKEN      lptkCSIni,         //188:  Ptr to start of CtrlStruc token stack (static)
                 lptkCSNxt;         //18C:  Ptr to next available slot in CS ...  (dynamic)
    struct tagFORSTMT *
                 lpForStmtBase;     //190:  Ptr to base of FORSTMT stack
#ifndef UNISCRIBE
    IMLangFontLink
                *lpFontLink;        //194:  Ptr to FontLink struc
#endif
    APLCHAR      cQuadPR,           //198:  []PR     (' ') (When a char scalar)
                 cQuadxSA;          //19A:  []SA     (0)   (in its index form as an integer)
    DWORD        dwThreadId;        //19C:  Corresponding thread ID
                                    //1A0:  Length
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
