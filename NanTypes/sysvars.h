//***************************************************************************
//  NARS2000 -- Default System Variable Values
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2019 Sudley Place Software

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

// Default values for system variables in a CLEAR WS
//   when not overridden by user preferences.
#define DEF_QUADCT_CWS      3E-15
#define DEF_QUADxCT_CWS     0x3CEB05876E5B0120      // 3E-15 in hex
#define DEF_QUADLR_CWS      L"r"
#define DEF_QUADLR_ALLOW    L"lr"
#define DEF_QUADHCM_CWS     L"i"
#define DEF_QUADHCM_ALLOW   L"iexdc"
#define DEF_QUADSUB_CWS     L"a"
#define DEF_QUADSUB_ALLOW   L"ad"
#define DEF_QUADLR_NAMES    L"Left", L"Right"
#define DEF_QUADDT_CWS      L"r"
#define DEF_QUADDT_ALLOW    L"rgp"
#define DEF_QUADDT_NAMES    L"Rectangular", L"Gaussian", L"Poisson"
#define DEF_QUADFC_CWS      aplDefaultFC
#define DEF_QUADFC_CWS_LEN  FCNDX_LENGTH
#define DEF_QUADFC_GLB      hGlbQuadFC_SYS
#define DEF_QUADFEATURE_CWS aplDefaultFEATURE
#define DEF_QUADFEATURE_GLB hGlbQuadFEATURE_SYS
#define DEF_QUADFPC_CWS     128
#define DEF_QUADIC_CWS      aplDefaultIC
#define DEF_QUADIC_CWS_LEN  ICNDX_LENGTH
#define DEF_QUADIC_GLB      hGlbQuadIC_SYS
#define DEF_QUADIO_CWS          1
#define DEF_QUADMF_CWS      TIMERSOURCE_PC2MS
#define DEF_QUADPP_CWS         10
#define DEF_QUADPW_CWS         80
#define DEF_QUADRL_CWS        1745365914586     // The 1st primitive root >= (-1+2*61)*2r3
#define DEF_QUADRL_CWS_WS   L"1745365914586"    // ...
#define DEF_QUADPR_CWS      L" "
#define DEF_QUADxSA_CWS         0   // 0 = "", 1 = "CLEAR", 2 = "ERROR", 3 = "EXIT", 4 = "OFF"
#define DEF_QUADLX_CWS      L""
#define DEF_QUADLX_GLB      hGlbV0Char
#define DEF_QUADALX_CWS     $QUAD_DM
#define DEF_QUADELX_CWS     $QUAD_DM
#define DEF_QUADALX_CWS_BR  L"{quad}DM"         // The braces form of DEF_QUADALX_CWS
#define DEF_QUADELX_CWS_BR  L"{quad}DM"         // ...                        ELX
#define DEF_QUADALX_GLB     hGlbQuadxLX
#define DEF_QUADELX_GLB     hGlbQuadxLX

// Characters to use as various separators and fills from []FC
#define DecSep      (GetQuadFCValue (FCNDX_DECIMAL_SEP))
#define ThouSep     (GetQuadFCValue (FCNDX_THOUSANDS_SEP))
#define OverFill    (GetQuadFCValue (FCNDX_OVERFLOW_FILL))
#define BlankFill   (GetQuadFCValue (FCNDX_BLANK_FILL))
#define NegSign     (GetQuadFCValue (FCNDX_NEGATIVE))
#define CndSep      (GetQuadFCValue (FCNDX_CNDSEP))

#define QUADRL_MODULUS      0x1FFFFFFFFFFFFFFF  // -1+2*61 is a prime

#define DEF_MIN_QUADCT      0                   // Minimum value for []CT
#define DEF_MAX_QUADCT      1E-10               // Maximum ...

#define DEF_MIN_QUADFEATURE 0                   // Minimum value for []FEATURE
#define DEF_MAX_QUADFEATURE 1                   // Maximum ...

#define DEF_MIN_QUADFPC     MPFR_PREC_MIN       // Minimum value for []FPC
#define DEF_MAX_QUADFPC    (MPFR_PREC_MAX/2)    // Maximum ...

#define DEF_MIN_QUADIC      ICVAL_MINVAL        // Minimum value for []IC
#define DEF_MAX_QUADIC      ICVAL_MAXVAL        // Maximum ...

#define DEF_MIN_QUADIO      0                   // Minimum value for []IO
#define DEF_MAX_QUADIO      1                   // Maximum ...

#define DEF_MIN_QUADPP      1                   // Minimum value for []PP
#define DEF_MAX_QUADPP_IEEE 17                  // Maximum ...            for IEEE 64-bits
#define DEF_MAX_QUADPP_VFP  INT32_MAX           // Maximum ...            for VFP numbers
#define DEF_MAX_QUADPP_UDM (INT32_MAX - 1)      // Maximum ...            for use with UDM_SETRANGE32

#define DEF_MIN_QUADPW     30                   // Minimum value for []PW
#define DEF_MAX_QUADPW      (INT32_MAX - 1)     // Maximum ...

#define DEF_MIN_QUADRL      1                   // Minimum value for []RL
#define DEF_MAX_QUADRL      (QUADRL_MODULUS - 1)// Maximum ...

#define DEF_MIN_QUADEIG     1                   // Minimum value for {domino}{variant} N
#define DEF_MAX_QUADEIG     5                   // Maximum ...


// System Comparison Tolerance
#define SYS_CT  1E-10

// GCD/LCM Comparison Tolerance
#define GCD_CT  1E-10

// Various string constants for []SA
#define V0Char      L""

#define SAEmpty     L""
#define SAClear     L"CLEAR"
#define SAError     L"ERROR"
#define SAExit      L"EXIT"
#define SAOff       L"OFF"

typedef enum tagSA_VALUES
{
    SAVAL_Empty = 0,        // 00:  ""
    SAVAL_Clear,            // 01:  "CLEAR"
    SAVAL_Error,            // 02:  "ERROR"
    SAVAL_Exit,             // 03:  "EXIT"
    SAVAL_Off               // 04:  "OFF"
                            // 05-07:  Available entries (3 bits)
} SA_VALUES;


typedef enum tagSYS_VARS
{
    SYSVAR_UNK = 0 ,            // 00:  Unknown name
    SYSVAR_ALX     ,            // 01:  []ALX
    SYSVAR_CT      ,            // 02:  []CT
    SYSVAR_DM      ,            // 03:  []DM
    SYSVAR_LR      ,            // 04:  []LR
    SYSVAR_DT      ,            // 05:  []DT
    SYSVAR_ELX     ,            // 06:  []ELX
    SYSVAR_FC      ,            // 07:  []FC
    SYSVAR_FEATURE ,            // 08:  []FEATURE
    SYSVAR_FPC     ,            // 09:  []FPC
    SYSVAR_IC      ,            // 0A:  []IC
    SYSVAR_IO      ,            // 0B:  []IO
    SYSVAR_LX      ,            // 0C:  []LX
    SYSVAR_PP      ,            // 0D:  []PP
    SYSVAR_PR      ,            // 0E:  []PR
    SYSVAR_PW      ,            // 0F:  []PW
    SYSVAR_RL      ,            // 10:  []RL
    SYSVAR_SA      ,            // 11:  []SA
    SYSVAR_WSID    ,            // 12:  []WSID
    SYSVAR_Z       ,            // 13:  []Z
    SYSVAR_LENGTH               // 14:  # entries in this enum
                                // 15-1F:  Available entries (5 bits)
} SYS_VARS, *LPSYS_VARS;


typedef struct tagSYSNAME
{
    LPWCHAR     lpwszName;      // Ptr to the name
    UINT        uValence;       // For system functions, Niladic(0), All others (1)
    UBOOL       bSysVar,        // Izit a system variable (TRUE) or function (FALSE)?  If TRUE, uValence is ignored
                bStdSysName;    // TRUE iff this name is in the Extended APL Standard
    LPPRIMFNS   lpNameFcn;      // Ptr to execution routine
    SYS_VARS    sysVarIndex;    // Value of each System Var (0 = Unknown)
} SYSNAME, *LPSYSNAME;


// Names of System Labels (MUST all be lowercase)
#define SYSLBL_ID       L"id"
#define SYSLBL_INV      L"inv"
#define SYSLBL_MS       L"ms"
#define SYSLBL_PRO      L"pro"


//***************************************************************************
//  End of File: sysvars.h
//***************************************************************************
