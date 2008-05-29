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
////lpMemPTD->bNegative          = FALSE;
////lpMemPTD->bNegExp            = FALSE;
////lpMemPTD->iNumLim            = 0;
////lpMemPTD->iStrLim            = 0;
////lpMemPTD->lpSISCur           = NULL;
////lpMemPTD->lpPLCur            = NULL;
////lpMemPTD->SILevel            = 0;
////lpmemPTD->hSemaDelay         = NULL;
////lpmemPTD->lpLstMVS           = NULL;
#ifndef UNISCRIBE
////lpmemPTD->lpFontLink         = NULL;
#endif

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
    int          iSymTabTotalSize;  // 24:  # STEs, currently
    HSHTABSTR    htsPTD;            // 28:  HshTab struc (32 bytes)
    UINT         tkErrorCharIndex;  // 48:  Char index for lpwszQuadErrorMsg if ImmExec
    HGLOBAL      hGlbQuadDM;        // 4C:  Quad-DM global memory handle

    HWND         hWndMC,            // 50:  MDI Client window handle
                 hWndSM,            // 54:  Session Manager ...
                 hWndDB,            // 58:  Debugger     ...
                 hWndActive;        // 5C:  Active MDI window when last switched out

    UINT         bTabTextState:1,   // 60:  00000001:  TRUE iff the tab's text state is Highlight, FALSE if Normal
                 bNegative:1,       //      00000002:  Sign bit for integer part
                 bNegExp:1,         //      00000004:  ...          exponent ...
                 bExecLX:1,         //      00000008:  TRUE iff execute []LX after successful load
                 Avail:28;          //      FFFFFFF0:  Available bits
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
    APLINT       aplInteger;        // 90:  8-byte Integers

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
                 CurTabID,          //110:  ID of the corresponding tab
                 PrvTabID;          //114:  ID of the preceding tab (from which we were loaded)
    HANDLE       hSemaDelay;        //118:  Delay semaphore (NULL if no delay active)
    EXIT_TYPES   ImmExecExitType;   //11C:  ImmExec exit type (see EXIT_TYPES)
    PL_YYSTYPE   YYResExec;         //120:  Result from execute primitive
                                    //      Size = 2Ch for DEBUG, 20h otherwise
    LPPL_YYSTYPE lpStrand[STRAND_LEN];//14C:  Ptrs to strand accumulators in parser (4 bytes each)
    LPLOADWSGLBVARCONV
                 lpLoadWsGlbVarConv;//154:  Ptr to function to convert a FMTSTR_GLBOBJ to an HGLOBAL
    LPLOADWSGLBVARPARM
                 lpLoadWsGlbVarParm;//158:  Ptr to extra parms for LoadWsGlbVarConv
    LPMEMVIRTSTR lpLstMVS;          //164:  Ptr to last MEMVIRTSTR (NULL = none)
    LPWCHAR      lpwszFormat;       //168:  Ptr to formatting save area
#ifndef UNISCRIBE
    IMLangFontLink
                *lpFontLink;        //16C:  Ptr to FontLink struc
#endif
    APLCHAR      cQuadPR,           //170:  []PR     (' ') (When a char scalar)
                 cQuadxSA;          //172:  []SA     (0)   (in its index form as an integer)
                                    //174:  Length
} PERTABDATA, *LPPERTABDATA;


//***************************************************************************
//  End of File: pertab.h
//***************************************************************************
