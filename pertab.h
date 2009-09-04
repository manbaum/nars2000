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
    HSHTABSTR    htsPTD;            // 0C:  Current HTS (120 bytes)
    UINT         tkErrorCharIndex;  // 84:  Char index for lpwszQuadErrorMsg if ImmExec
    HGLOBAL      hGlbQuadDM;        // 88:  Quad-DM global memory handle

    HWND         hWndMC,            // 8C:  MDI Client window handle
                 hWndSM,            // 90:  Session Manager ...
                 hWndDB,            // 94:  Debugger     ...
                 hWndActive;        // 98:  Active MDI window when last switched out
    UINT         bTabTextState:1,   // 9C:  00000001:  TRUE iff the tab's text state is Highlight, FALSE if Normal
                 bExecLX:1,         //      00000002:  TRUE iff execute []LX after successful load
                 bExecuting:1,      //      00000004:  TRUE iff we're waiting for an execution to complete
                 :29;               //      FFFFFFF8:  Available bits
    HGLOBAL      hGlbCurLine;       // A0:  Current line global memory handle
    LPWCHAR      lpwszErrorMessage; // A4:  Ptr to error message to signal
    LPWCHAR      lpwszQuadErrorMsg; // A8   Used for []ERROR/[]ES messages
    UINT         uCaret;            // AC:  Position of the caret in the current line on error
    int          crIndex;           // B0:  Tab's color index
    APLINT       uQuadMF;           // B4:  []MF timer value (8 bytes)
    LPSYMENTRY   steZero,           // B8:  Ptr to STE for constant zero
                 steOne,            // BC:  ...                     one
                 steBlank,          // C0:  ...                     blank
                 steNoValue;        // C4:  ...            no-value result
    struct tagSIS_HEADER
                *lpSISBeg,          // C8:  Ptr to State Indicator Stack beginning
                *lpSISCur,          // CC:  ...                          current (may be NULL if SI is empty)
                *lpSISNxt;          // D0:  ...                          next
    struct tagPLLOCALVARS
                *lpPLCur;           // D4:  Ptr to current plLocalVars struct
                                    //      in thread creation order (NULL = none)
    WNDPROC lpfnOldEditCtrlWndProc; // D8:  Save area for old Edit Ctrl procedure

    // Magic function/operator handles and strucs
    HGLOBAL      hGlbMFO_MonIota,   // DC:  Extended Monadic Iota
                 hGlbMFO_DydIota,   // E0:  Extended Dyadic Iota
                 hGlbMFO_MonDnShoe, // E4:  Monadic Down Shoe
                 hGlbMFO_DydTilde,  // E8:  Dyadic Tilde
                 hGlbMFO_MonRank,   // EC:  Monadic Rank
                 hGlbMFO_DydRank,   // F0:  Dyadic Rank
                 hGlbMFO_Conform,   // F4:  Conform for Rank operator
                 hGlbMFO_MonFMT,    // F8:  Display function (monadic []FMT)
                 hGlbMFO_Box,       // FC:  Box     ...
                 hGlbMFO_MonVR;     //100:  Visual Representation function (monadic []VR)
    HSHTABSTR    htsPTD_MonVR;      //104:  HTS for monadic []VR (120 bytes)

    UINT         SILevel,           //17C:  Current State Indicator level
                 CurTabID,          //180:  ID of the corresponding tab
                 PrvTabID;          //184:  ID of the preceding tab (from which we were loaded)
    HANDLE       hSemaDelay;        //188:  Delay semaphore (NULL if no delay active)
    EXIT_TYPES   ImmExecExitType;   //18C:  ImmExec exit type (see EXIT_TYPES)
    PL_YYSTYPE   YYResExec,         //190:  Result from execute primitive
                                    //      Size = 2Ch for DEBUG, 20h otherwise
                 YYCaseExec;        //1BC:  Result from CASE arg execution
                                    //      Size = 2Ch for DEBUG, 20h otherwise
    LPPL_YYSTYPE lpStrand[STRAND_LEN];//1E8:  Ptrs to strand accumulators in parser (4*4 bytes)
    LPLOADWSGLBVARCONV
                 lpLoadWsGlbVarConv;//1F8:  Ptr to function to convert a FMTSTR_GLBOBJ to an HGLOBAL
    LPLOADWSGLBVARPARM
                 lpLoadWsGlbVarParm;//1FC:  Ptr to extra parms for LoadWsGlbVarConv
    LPMEMVIRTSTR lpLstMVS;          //200:  Ptr to last MEMVIRTSTR (NULL = none)
    LPWCHAR      lpwszFormat,       //204:  Ptr to formatting save area
                 lpwszBaseTemp,     //208:  Ptr to base of lpwszTemp
                 lpwszTemp;         //20C:  Ptr to temporary  ...
    UINT         uTempMaxSize,      //210:  Maximum size of lpwszTemp
                 uErrLine;          //214:  Error line # from []FX for )IN
    APLU3264     RegisterEBP;       //218:  Register EBP/RBP from an exception
    LPTOKEN      lptkCSIni,         //21C:  Ptr to start of CtrlStruc token stack (static)
                 lptkCSNxt;         //220:  Ptr to next available slot in CS ...  (dynamic)
    struct tagFORSTMT *
                 lpForStmtBase;     //224:  Ptr to base of FORSTMT stack
#ifndef UNISCRIBE
    IMLangFontLink
                *lpFontLink;        //228:  Ptr to FontLink struc
#endif
    APLCHAR      cQuadPR,           //22C:  []PR     (' ') (When a char scalar)
                 cQuadxSA;          //22E:  []SA     (0)   (in its index form as an integer)
    DWORD        dwThreadId;        //230:  Corresponding thread ID
    HANDLE       hExitphore;        //234:  Semaphore used to close a tab (may be NULL)
    HWND         hWndFENxt;         //238:  Next FE window handle (NULL = none)
                                    //23C:  Length
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
