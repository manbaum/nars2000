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

#define INIT_PERTABVARS                             \
    lpMemPTD->Sig.nature    = ' DTP';               \
    lpMemPTD->hGlbQuadDM    = hGlbV0Char;           \
    lpMemPTD->uQuadMF       = uQuadMF_CWS;          \

    // The following vars are already initialized to zero which is the default
    //   value from GHND, so they do not need to be set explicitly.
////lpMemPTD->numYYRes      = 0;
////lpMemPTD->bTabTextState = FALSE;
////lpMemPTD->lpSISCur      = NULL;
////lpMemPTD->lpPLCur       = NULL;
////lpMemPTD->SILevel       = 0;
////lpMemPTD->hSemaDelay    = NULL;
////lpMemPTD->lpLstMVS      = NULL;
#ifndef UNISCRIBE
////lpMemPTD->lpFontLink    = NULL;
#endif

// Structure for Per Tab Control Data
typedef struct tagPERTABDATA
{
    HEADER_SIGNATURE Sig;           // 00:  Signature
    LPPL_YYSTYPE lpYYRes;           // 04:  The result token
    UINT         numYYRes;          // 08:  # YYRes elements in the array
                                    //      pointed to by lpYYRes

    // Symbol & hash table variables
    HSHTABSTR    htsPTD;            // 0C:  HshTab struc (60 bytes)
    UINT         tkErrorCharIndex;  // 48:  Char index for lpwszQuadErrorMsg if ImmExec
    HGLOBAL      hGlbQuadDM;        // 4C:  Quad-DM global memory handle

    HWND         hWndMC,            // 50:  MDI Client window handle
                 hWndSM,            // 54:  Session Manager ...
                 hWndDB,            // 58:  Debugger     ...
                 hWndActive;        // 5C:  Active MDI window when last switched out
    UINT         bTabTextState:1,   // 60:  00000001:  TRUE iff the tab's text state is Highlight, FALSE if Normal
                 bExecLX:1,         //      00000002:  TRUE iff execute []LX after successful load
                 bExecuting:1,      //      00000004:  TRUE iff we're waiting for an execution to complete
                 :29;               //      FFFFFFF8:  Available bits
    HGLOBAL      hGlbCurLine;       // 64:  Current line global memory handle
    LPWCHAR      lpwszErrorMessage; // 68:  Ptr to error message to signal
    LPWCHAR      lpwszQuadErrorMsg; // 6C   Used for []ERROR/[]ES messages
    UINT         uCaret;            // 70:  Position of the caret in the current line on error
    int          crIndex;           // 74:  Tab's color index
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

    // Magic function handles and strucs
    HGLOBAL      hGlbMF_MonIota,    // DC:  Extended Monadic Iota
                 hGlbMF_DydIota,    // E0:  Extended Dyadic Iota
                 hGlbMF_MonDnShoe,  // E4:  Monadic Down Shoe
                 hGlbMF_DydTilde,   // E8:  Dyadic Tilde
                 hGlbMF_MonRank,    // EC:  Monadic Rank
                 hGlbMF_DydRank,    // F0:  Dyadic Rank
                 hGlbMF_Conform,    // F4:  Conform for Rank operator
                 hGlbMF_MonFMT,     // F8:  Display function (monadic []FMT)
                 hGlbMF_Box;        // FC:  Box     ...
    HSHTABSTR    htsPTD_MonFMT;     //100:  HTS for monadic []FMT (60 bytes)

    UINT         SILevel,           //13C:  Current State Indicator level
                 CurTabID,          //140:  ID of the corresponding tab
                 PrvTabID;          //144:  ID of the preceding tab (from which we were loaded)
    HANDLE       hSemaDelay;        //148:  Delay semaphore (NULL if no delay active)
    EXIT_TYPES   ImmExecExitType;   //14C:  ImmExec exit type (see EXIT_TYPES)
    PL_YYSTYPE   YYResExec,         //150:  Result from execute primitive
                                    //      Size = 2Ch for DEBUG, 20h otherwise
                 YYCaseExec;        //17C:  Result from CASE arg execution
                                    //      Size = 2Ch for DEBUG, 20h otherwise
    LPPL_YYSTYPE lpStrand[STRAND_LEN];//1A8:  Ptrs to strand accumulators in parser (4 bytes each)
    LPLOADWSGLBVARCONV
                 lpLoadWsGlbVarConv;//1AC:  Ptr to function to convert a FMTSTR_GLBOBJ to an HGLOBAL
    LPLOADWSGLBVARPARM
                 lpLoadWsGlbVarParm;//1B0:  Ptr to extra parms for LoadWsGlbVarConv
    LPMEMVIRTSTR lpLstMVS;          //1B4:  Ptr to last MEMVIRTSTR (NULL = none)
    LPWCHAR      lpwszFormat,       //1B8:  Ptr to formatting save area
                 lpwszBaseTemp,     //1BC:  Ptr to base of lpwszTemp
                 lpwszTemp;         //1C0:  Ptr to temporary  ...
    UINT         uTempMaxSize,      //1C4:  Maximum size of lpwszTemp
                 RegisterEBP,       //1C8:  Register EBP from an exception
                 uErrLine;          //1CC:  Error line # from []FX for )IN
    LPTOKEN      lptkCSIni,         //1D0:  Ptr to start of CtrlStruc token stack (static)
                 lptkCSNxt;         //1D4:  Ptr to next available slot in CS ...  (dynamic)
    struct tagFORSTMT *
                 lpForStmtBase;     //1D8:  Ptr to base of FORSTMT stack
#ifndef UNISCRIBE
    IMLangFontLink
                *lpFontLink;        //1DC:  Ptr to FontLink struc
#endif
    APLCHAR      cQuadPR,           //1E0:  []PR     (' ') (When a char scalar)
                 cQuadxSA;          //1E2:  []SA     (0)   (in its index form as an integer)
    DWORD        dwThreadId;        //1E4:  Corresponding thread ID
                                    //1E8:  Length
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
