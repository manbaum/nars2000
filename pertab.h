//***************************************************************************
//  NARS2000 -- PerTabData Header
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
    UINT         tkErrorCharIndex;  // 88:  Char index for lpwszQuadErrorMsg if ImmExec
    HGLOBAL      hGlbQuadDM;        // 8C:  Quad-DM global memory handle

    HWND         hWndMC,            // 90:  MDI Client window handle
                 hWndSM,            // 94:  Session Manager ...
                 hWndDB,            // 98:  Debugger     ...
                 hWndActive;        // 9C:  Active MDI window when last switched out
    UINT         bTabTextState:1,   // A0:  00000001:  TRUE iff the tab's text state is Highlight, FALSE if Normal
                 bExecLX:1,         //      00000002:  TRUE iff execute []LX after successful load
                 bExecuting:1,      //      00000004:  TRUE iff we're waiting for an execution to complete
                 :29;               //      FFFFFFF8:  Available bits
    HGLOBAL      hGlbCurLine;       // A4:  Current line global memory handle
    LPWCHAR      lpwszErrorMessage; // A8:  Ptr to error message to signal
    LPWCHAR      lpwszQuadErrorMsg; // AC   Used for []ERROR/[]ES messages
    UINT         uCaret;            // B0:  Position of the caret in the current line on error
    int          crIndex;           // B4:  Tab's color index
    APLINT       uQuadMF;           // B8:  []MF timer value (8 bytes)
    LPSYMENTRY   steZero,           // BC:  Ptr to STE for constant zero
                 steOne,            // C0:  ...                     one
                 steBlank,          // C4:  ...                     blank
                 steNoValue;        // C8:  ...            no-value result
    struct tagSIS_HEADER
                *lpSISBeg,          // CC:  Ptr to State Indicator Stack beginning
                *lpSISCur,          // D0:  ...                          current (may be NULL if SI is empty)
                *lpSISNxt;          // D4:  ...                          next
    struct tagPLLOCALVARS
                *lpPLCur;           // D8:  Ptr to current plLocalVars struct
                                    //      in thread creation order (NULL = none)
    WNDPROC lpfnOldEditCtrlWndProc; // DC:  Save area for old Edit Ctrl procedure

    // Magic function/operator handles and strucs
    HGLOBAL      hGlbMFO_MonIota,   // E0:  Extended Monadic Iota
                 hGlbMFO_DydIota,   // E4:  Extended Dyadic Iota
                 hGlbMFO_MonDnShoe, // E8:  Monadic Down Shoe
                 hGlbMFO_DydTilde,  // EC:  Dyadic Tilde
                 hGlbMFO_MonRank,   // F0:  Monadic Rank
                 hGlbMFO_DydRank,   // F4:  Dyadic Rank
                 hGlbMFO_Conform,   // F8:  Conform for Rank operator
                 hGlbMFO_MonFMT,    // FC:  Display function (monadic []FMT)
                 hGlbMFO_Box,       //100:  Box     ...
                 hGlbMFO_MonVR;     //104:  Visual Representation function (monadic []VR)
    HSHTABSTR    htsPTD_MonVR;      //108:  HTS for monadic []VR (124 bytes)

    UINT         SILevel,           //184:  Current State Indicator level
                 CurTabID,          //188:  ID of the corresponding tab
                 PrvTabID;          //18C:  ID of the preceding tab (from which we were loaded)
    HANDLE       hSemaDelay;        //190:  Delay semaphore (NULL if no delay active)
    EXIT_TYPES   ImmExecExitType;   //194:  ImmExec exit type (see EXIT_TYPES)
    PL_YYSTYPE   YYResExec,         //198:  Result from execute primitive
                                    //      Size = 2Ch for DEBUG, 20h otherwise
                 YYCaseExec;        //1C4:  Result from CASE arg execution
                                    //      Size = 2Ch for DEBUG, 20h otherwise
    LPPL_YYSTYPE lpStrand[STRAND_LEN];//1F0:  Ptrs to strand accumulators in parser (4*4 bytes)
    LPLOADWSGLBVARCONV
                 lpLoadWsGlbVarConv;//200:  Ptr to function to convert a FMTSTR_GLBOBJ to an HGLOBAL
    LPLOADWSGLBVARPARM
                 lpLoadWsGlbVarParm;//204:  Ptr to extra parms for LoadWsGlbVarConv
    LPMEMVIRTSTR lpLstMVS;          //208:  Ptr to last MEMVIRTSTR (NULL = none)
    LPWCHAR      lpwszFormat,       //20C:  Ptr to formatting save area
                 lpwszBaseTemp,     //210:  Ptr to base of lpwszTemp
                 lpwszTemp;         //214:  Ptr to temporary  ...
    UINT         uTempMaxSize,      //218:  Maximum size of lpwszTemp
                 uErrLine;          //21C:  Error line # from []FX for )IN
    APLU3264     RegisterEBP;       //220:  Register EBP/RBP from an exception
    LPTOKEN      lptkCSIni,         //224:  Ptr to start of CtrlStruc token stack (static)
                 lptkCSNxt;         //228:  Ptr to next available slot in CS ...  (dynamic)
    struct tagFORSTMT *
                 lpForStmtBase;     //22C:  Ptr to base of FORSTMT stack
#ifndef UNISCRIBE
    IMLangFontLink
                *lpFontLink;        //230:  Ptr to FontLink struc
#endif
    APLCHAR      cQuadPR,           //234:  []PR     (' ') (When a char scalar)
                 cQuadxSA;          //236:  []SA     (0)   (in its index form as an integer)
    DWORD        dwThreadId;        //238:  Corresponding thread ID
    HANDLE       hExitphore;        //23C:  Semaphore used to close a tab (may be NULL)
    HWND         hWndFENxt;         //240:  Next FE window handle (NULL = none)
                                    //244:  Length
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
