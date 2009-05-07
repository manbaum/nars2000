//***************************************************************************
//  NARS2000 -- Magic Function Execution Header
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

// Magic Function Names
#define MFN_MonIota         L"#MonIota"
#define MFN_DydIota         L"#DydIota"
#define MFN_MonDnShoe       L"#MonDnShoe"
#define MFN_DydTilde        L"#DydTilde"
#define MFN_MonRank         L"#MonRank"
#define MFN_DydRank         L"#DydRank"
#define MFN_Conform         L"#Conform"
#define MFN_MonFMT          L"#MonFMT"
#define MFN_Box             L"#Box"
#define MFN_MonVR           L"#MonVR"

// Magic function variables
#define $COMMABAR       WS_UTF16_COMMABAR
#define $DECODE         WS_UTF16_UPTACK
#define $DIAMOND        WS_UTF16_DIAMOND
#define $DROP           WS_UTF16_DOWNARROW
#define $DEL            WS_UTF16_DEL
#define $DISCLOSE       WS_UTF16_RIGHTSHOE
#define $EACH           WS_UTF16_DIERESIS
#define $ENCLOSE        WS_UTF16_LEFTSHOE
#define $ENCODE         WS_UTF16_DOWNTACK
#define $EPSILON        WS_UTF16_EPSILON
#define $FIRST          WS_UTF16_UPARROW
#define $FORMAT         WS_UTF16_DOWNTACKJOT
#define $GE             WS_UTF16_RIGHTCARETUNDERBAR
#define $GOTO           WS_UTF16_RIGHTARROW
#define $IOTA           WS_UTF16_IOTA
#define $IS             WS_UTF16_LEFTARROW
#define $JOT            WS_UTF16_JOT
#define $LDC_LT_HORZ    WS_UTF16_LDC_LT_HORZ
#define $LDC_LT_VERT    WS_UTF16_LDC_LT_VERT
#define $LDC_LT_UL      WS_UTF16_LDC_LT_UL
#define $LDC_LT_UR      WS_UTF16_LDC_LT_UR
#define $LDC_LT_LL      WS_UTF16_LDC_LT_LL
#define $LDC_LT_LR      WS_UTF16_LDC_LT_LR
#define $LDC_LT_VERT_R  WS_UTF16_LDC_LT_VERT_R
#define $LDC_LT_VERT_L  WS_UTF16_LDC_LT_VERT_L
#define $LDC_LT_HORZ_D  WS_UTF16_LDC_LT_HORZ_D
#define $LDC_LT_HORZ_U  WS_UTF16_LDC_LT_HORZ_U
#define $MATCH          WS_UTF16_EQUALUNDERBAR
#define $MAX            WS_UTF16_UPSTILE
#define $MIN            WS_UTF16_DOWNSTILE
#define $NEG            WS_UTF16_OVERBAR
#define $NOTEQUAL       WS_UTF16_NOTEQUAL
#define $QUAD           WS_UTF16_QUAD
#define $QUAD_PROTOTYPE WS_UTF16_QUAD L"PROTOTYPE"
#define $QUAD_ERROR     WS_UTF16_QUAD L"ERROR"
#define $QUAD_NC        WS_UTF16_QUAD L"NC"
#define $QUAD_IO        WS_UTF16_QUAD L"IO"
#define $RANK           WS_UTF16_DIERESISJOT
#define $REVERSE        WS_UTF16_CIRCLESTILE
#define $ROTATE         WS_UTF16_CIRCLESTILE
#define $RHO            WS_UTF16_RHO
#define $TAKE           WS_UTF16_UPARROW
#define $TILDE          WS_UTF16_TILDE
#define $TRANSPOSE      WS_UTF16_CIRCLESLOPE
#define $ZILDE          WS_UTF16_ZILDE


typedef struct tagMAGIC_FUNCTION
{
    LPAPLCHAR  Header;              // 00:  Ptr to function/operator header
    LPAPLCHAR *Body;                // 04:  Ptr to array of function lines
    UINT       numFcnLines;         // 08:  # function lines in the body
                                    // 0C:  Length
} MAGIC_FUNCTION, *LPMAGIC_FUNCTION;

typedef struct tagINIT_MF
{
    LPHSHTABSTR  lpHTS;             // 00:  Ptr to HSHTABSTR struc
    LPMEMVIRTSTR lpLclMemVirtStr;   // 04:  Ptr to local MemVirtStr
    UINT         uPtdMemVirtStart,  // 08:  Starting offset into lpLclMemVirtStr
                 uPtdMemVirtEnd;    // 0C:  Ending   ...
                                    // 10:  Length
} INIT_MF, *LPINIT_MF;


//***************************************************************************
//  End of File: execmfn.h
//***************************************************************************
