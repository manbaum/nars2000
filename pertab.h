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
    lpMemPTD->uQuadMF       = uQuadMF_CWS;          \
    SetCurrentFeatureCWS (lpMemPTD);                \
    lpMemPTD->hGlbQuadEM    = hGlbQuadEM_DEF;       \

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
////lpMemPTD->EventType     = EVENTTYPE_NOERROR;

// Structure for Per Tab Control Data
typedef struct tagPERTABDATA
{
    HEADER_SIGNATURE Sig;                   // Signature
    LPPL_YYSTYPE lpYYRes;                   // The result token
    UINT         numYYRes;                  // # YYRes elements in the array
                                            //   pointed to by lpYYRes

    // Symbol & hash table variables
    HSHTABSTR    htsPTD;                    // Current HTS
    UINT         tkErrorCharIndex;          // Char index for lpwszQuadErrorMsg if ImmExec

    HWND         hWndMC,                    // MDI Client window handle
                 hWndSM,                    // Session Manager ...
                 hWndDB,                    // Debugger     ...
                 hWndActive;                // Active MDI window when last switched out
    UINT         bTabTextState:1,           // 00000001:  TRUE iff the tab's text state is Highlight, FALSE if Normal
                 bExecLX:1,                 // 00000002:  TRUE iff execute []LX after successful load
                 bExecuting:1,              // 00000004:  TRUE iff we're waiting for an execution to complete
                 bInTF:1,                   // 00000008:  TRUE if we're fixing a function via []TF
                 :28;                       // FFFFFFF0:  Available bits
    HGLOBAL      hGlbCurLine;               // Current line global memory handle
    LPWCHAR      lpwszErrorMessage;         // Ptr to error message to signal
    LPWCHAR      lpwszQuadErrorMsg;         // Used for []ERROR/[]ES messages
    UINT         uCaret;                    // Position of the caret in the current line on error
    int          crIndex;                   // Tab's color index
    APLINT       uQuadMF;                   // []MF timer value (8 bytes)
    LPSYMENTRY   steZero,                   // Ptr to STE for constant zero
                 steOne,                    // ...                     one
                 steBlank,                  // ...                     blank
                 steNoValue;                // ...            no-value result
    struct tagSIS_HEADER
                *lpSISBeg,                  // Ptr to State Indicator Stack beginning
                *lpSISCur,                  // ...                          current (may be NULL if SI is empty)
                *lpSISNxt;                  // ...                          next
    struct tagPLLOCALVARS
                *lpPLCur;                   // Ptr to current plLocalVars struct
                                            //   in thread creation order (NULL = none)
    WNDPROC      lpfnOldEditCtrlWndProc;    // Save area for old Edit Ctrl procedure

    // Magic function/operator handles and strucs
    HGLOBAL      hGlbMFO[MFOE_LENGTH];      // Magic function/operator global memory handles
    HSHTABSTR    htsPTD_MonVR;              // HTS for monadic []VR

    UINT         SILevel,                   // Current State Indicator level
                 CurTabID,                  // ID of the corresponding tab
                 PrvTabID;                  // ID of the preceding tab (from which we were loaded)
    HANDLE       hSemaDelay;                // Delay semaphore (NULL if no delay active)
    EXIT_TYPES   ImmExecExitType;           // ImmExec exit type (see EXIT_TYPES)
    PL_YYSTYPE   YYResExec,                 // Result from execute primitive
                 YYCaseExec;                // Result from CASE arg execution
    LPPL_YYSTYPE lpStrand[STRAND_LEN];      // Ptrs to strand accumulators in parser
    LPLOADWSGLBVARCONV
                 lpLoadWsGlbVarConv;        // Ptr to function to convert a FMTSTR_GLBOBJ to an HGLOBAL
    LPLOADWSGLBVARPARM
                 lpLoadWsGlbVarParm;        // Ptr to extra parms for LoadWsGlbVarConv
    LPMEMVIRTSTR lpLstMVS;                  // Ptr to last MEMVIRTSTR (NULL = none)
    LPWCHAR      lpwszFormat,               // Ptr to formatting save area
                 lpwszBaseTemp,             // Ptr to base of lpwszTemp
                 lpwszTemp;                 // Ptr to temporary  ...
    UINT         uTempMaxSize,              // Maximum size of lpwszTemp
                 uErrLine;                  // Error line # from []FX for )IN
    APLU3264     RegisterEBP;               // Register EBP/RBP from an exception
    LPTOKEN      lptkCSIni,                 // Ptr to start of CtrlStruc token stack (static)
                 lptkCSNxt;                 // Ptr to next available slot in CS ...  (dynamic)
    struct tagFORSTMT *
                 lpForStmtBase;             // Ptr to base of FORSTMT stack
#ifndef UNISCRIBE
    IMLangFontLink
                *lpFontLink;                // Ptr to FontLink struc
#endif
    HGLOBAL      hGlbQuadEM;                // []EM global memory handle for global value
    EVENT_TYPES  EventType;                 // []ET global value (Major, Minor) (see EVENT_TYPES)
    APLCHAR      cQuadPR,                   // []PR     (' ') (When a char scalar) (2 bytes)
                 cQuadxSA;                  // []SA     (0)   (in its index form as an integer) (2 bytes)
    DWORD        dwThreadId;                // Corresponding thread ID
    HANDLE       hExitphore;                // Semaphore used to close a tab (may be NULL)
    HWND         hWndFENxt;                 // Next FE window handle (NULL = none)
    APLINT       aplCurrentFEATURE[FEATURENDX_LENGTH];  // Current values for []FEATURE
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
