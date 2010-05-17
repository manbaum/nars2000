//***************************************************************************
//  NARS2000 -- PerTabData Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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
////lpMemPTD->hExitphore    = NULL;
////lpMemPTD->hWndFENxt     = NULL;

// Structure for Per Tab Control Data
typedef struct tagPERTABDATA
{
    HEADER_SIGNATURE Sig;           // 00:  Signature
    LPPL_YYSTYPE lpYYRes;           // 04:  The result token
    UINT         numYYRes;          // 08:  # YYRes elements in the array
                                    //      pointed to by lpYYRes

    // Symbol & hash table variables
    HSHTABSTR    htsPTD;            // 0C:  Current HTS (124 bytes)
    UINT         tkErrorCharIndex;  // 0C+HTS:  Char index for lpwszQuadErrorMsg if ImmExec
    HGLOBAL      hGlbQuadDM;        // 10+HTS:  Quad-DM global memory handle

    HWND         hWndMC,            // 14+HTS:  MDI Client window handle
                 hWndSM,            // 18+HTS:  Session Manager ...
                 hWndDB,            // 1C+HTS:  Debugger     ...
                 hWndActive;        // 20+HTS:  Active MDI window when last switched out
    UINT         bTabTextState:1,   // 24+HTS:  00000001:  TRUE iff the tab's text state is Highlight, FALSE if Normal
                 bExecLX:1,         //          00000002:  TRUE iff execute []LX after successful load
                 bExecuting:1,      //          00000004:  TRUE iff we're waiting for an execution to complete
                 bInTF:1,           //          00000008:  TRUE if we're fixing a function via []TF
                 :28;               //          FFFFFFF0:  Available bits
    HGLOBAL      hGlbCurLine;       // 28+HTS:  Current line global memory handle
    LPWCHAR      lpwszErrorMessage; // 2C+HTS:  Ptr to error message to signal
    LPWCHAR      lpwszQuadErrorMsg; // 30+HTS   Used for []ERROR/[]ES messages
    UINT         uCaret;            // 34+HTS:  Position of the caret in the current line on error
    int          crIndex;           // 38+HTS:  Tab's color index
    APLINT       uQuadMF;           // 3C+HTS:  []MF timer value (8 bytes)
    LPSYMENTRY   steZero,           // 44+HTS:  Ptr to STE for constant zero
                 steOne,            // 48+HTS:  ...                     one
                 steBlank,          // 4C+HTS:  ...                     blank
                 steNoValue;        // 50+HTS:  ...            no-value result
    struct tagSIS_HEADER
                *lpSISBeg,          // 54+HTS:  Ptr to State Indicator Stack beginning
                *lpSISCur,          // 58+HTS:  ...                          current (may be NULL if SI is empty)
                *lpSISNxt;          // 5C+HTS:  ...                          next
    struct tagPLLOCALVARS
                *lpPLCur;           // 60+HTS:  Ptr to current plLocalVars struct
                                    //      in thread creation order (NULL = none)
    WNDPROC lpfnOldEditCtrlWndProc; // 64+HTS:  Save area for old Edit Ctrl procedure

    // Magic function/operator handles and strucs
    HGLOBAL      hGlbMFO[MFOE_LENGTH];// 68+HTS: Magic function/operator global memory handles (40 bytes)
    HSHTABSTR    htsPTD_MonVR;      // 68+HTS+MFO:  HTS for monadic []VR (124 bytes)

    UINT         SILevel,           // 68+2*HTS+MFO:  Current State Indicator level
                 CurTabID,          // 6C+2*HTS+MFO:  ID of the corresponding tab
                 PrvTabID;          // 70+2*HTS+MFO:  ID of the preceding tab (from which we were loaded)
    HANDLE       hSemaDelay;        // 74+2*HTS+MFO:  Delay semaphore (NULL if no delay active)
    EXIT_TYPES   ImmExecExitType;   // 78+2*HTS+MFO:  ImmExec exit type (see EXIT_TYPES)
    PL_YYSTYPE   YYResExec,         // 7C+2*HTS+MFO:  Result from execute primitive
                                    //      Size = 2Ch for DEBUG, 20h otherwise
                 YYCaseExec;        // 7C+2*HTS+MFO+YYS:  Result from CASE arg execution
                                    //      Size = 2Ch for DEBUG, 20h otherwise
    LPPL_YYSTYPE lpStrand[STRAND_LEN];// 7C+2*HTS+MFO+2*YYS:  Ptrs to strand accumulators in parser (4*4 bytes)
    LPLOADWSGLBVARCONV
                 lpLoadWsGlbVarConv;// 8C+2*HTS+MFO+2*YYS:  Ptr to function to convert a FMTSTR_GLBOBJ to an HGLOBAL
    LPLOADWSGLBVARPARM
                 lpLoadWsGlbVarParm;// 90+2*HTS+MFO+2*YYS:  Ptr to extra parms for LoadWsGlbVarConv
    LPMEMVIRTSTR lpLstMVS;          // 94+2*HTS+MFO+2*YYS:  Ptr to last MEMVIRTSTR (NULL = none)
    LPWCHAR      lpwszFormat,       // 98+2*HTS+MFO+2*YYS:  Ptr to formatting save area
                 lpwszBaseTemp,     // 9C+2*HTS+MFO+2*YYS:  Ptr to base of lpwszTemp
                 lpwszTemp;         // A0+2*HTS+MFO+2*YYS:  Ptr to temporary  ...
    UINT         uTempMaxSize,      // A4+2*HTS+MFO+2*YYS:  Maximum size of lpwszTemp
                 uErrLine;          // A8+2*HTS+MFO+2*YYS:  Error line # from []FX for )IN
    APLU3264     RegisterEBP;       // AC+2*HTS+MFO+2*YYS:  Register EBP/RBP from an exception
    LPTOKEN      lptkCSIni,         // B0+2*HTS+MFO+2*YYS:  Ptr to start of CtrlStruc token stack (static)
                 lptkCSNxt;         // B4+2*HTS+MFO+2*YYS:  Ptr to next available slot in CS ...  (dynamic)
    struct tagFORSTMT *
                 lpForStmtBase;     // B8+2*HTS+MFO+2*YYS:  Ptr to base of FORSTMT stack
#ifndef UNISCRIBE
    IMLangFontLink
                *lpFontLink;        // BC+2*HTS+MFO+2*YYS:  Ptr to FontLink struc
#endif
    APLCHAR      cQuadPR,           // C0+2*HTS+MFO+2*YYS:  []PR     (' ') (When a char scalar) (2 bytes)
                 cQuadxSA;          // C2+2*HTS+MFO+2*YYS:  []SA     (0)   (in its index form as an integer) (2 bytes)
    DWORD        dwThreadId;        // C4+2*HTS+MFO+2*YYS:  Corresponding thread ID
    HANDLE       hExitphore;        // C8+2*HTS+MFO+2*YYS:  Semaphore used to close a tab (may be NULL)
    HWND         hWndFENxt;         // CC+2*HTS+MFO+2*YYS:  Next FE window handle (NULL = none)
                                    // D0+2*HTS+MFO+2*YYS:  Length
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
