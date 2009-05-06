//***************************************************************************
//  NARS2000 -- Default System Variable Values
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

// Default values for system variables in a CLEAR WS
//   when not overridden by user preferences.
#define DEF_QUADCT_CWS      3E-15
#define DEF_QUADxCT_CWS     0x3CEB05876E5B0120      // 3E-15 in hex
#define DEF_QUADFC_CWS      L".,*0_" WS_UTF16_OVERBAR
#define DEF_QUADFC_CWS_BR   L".,*0_{overbar}"       // The braces form of DEF_QUAD_FC_CWS
#define DEF_QUADFC_CWS_LEN  ((sizeof (DEF_QUADFC_CWS) / sizeof (DEF_QUADFC_CWS[0])) - 1)
#define DEF_QUADFC_OVERFLOW L'0'
#define DEF_QUADIC_CWS      aplDefaultIC
#define DEF_QUADFC_GLB      hGlbQuadFC_SYS
#define DEF_QUADIC_GLB      hGlbQuadIC_SYS
#define DEF_QUADIO_CWS          1
#define DEF_QUADMF_CWS          2
#define DEF_QUADPP_CWS         10
#define DEF_QUADPW_CWS         80
#define DEF_QUADRL_CWS      16807
#define DEF_QUADPR_CWS      L" "
#define DEF_QUADxSA_CWS         0   // 0 = "", 1 = "CLEAR", 2 = "ERROR", 3 = "EXIT", 4 = "OFF"
#define DEF_QUADLX_CWS      L""
#define DEF_QUADLX_GLB      hGlbV0Char
#define WS_QUADDM           (WS_UTF16_QUAD L"DM")
#define DEF_QUADALX_CWS     WS_QUADDM
#define DEF_QUADELX_CWS     WS_QUADDM
#define DEF_QUADALX_CWS_BR  L"{quad}DM"         // The braces form of DEF_QUADALX_CWS
#define DEF_QUADELX_CWS_BR  L"{quad}DM"         // ...                        ELX
#define DEF_QUADALX_GLB     hGlbQuadDM
#define DEF_QUADELX_GLB     hGlbQuadDM

#define QUADRL_MODULUS      0x7FFFFFFF          // (2*31)-1 is a prime

#define DEF_MIN_QUADCT      0                   // Minimum value for []CT
#define DEF_MAX_QUADCT      1E-10               // Maximum ...

#define DEF_MIN_QUADIC      0                   // Minimum value for []IC
#define DEF_MAX_QUADIC      ICVAL_MAXVAL        // Maximum ...

#define DEF_MIN_QUADIO      0                   // Minimum value for []IO
#define DEF_MAX_QUADIO      1                   // Maximum ...

#define DEF_MIN_QUADPP      1                   // Minimum value for []PP
#define DEF_MAX_QUADPP     17                   // Maximum ...

#define DEF_MIN_QUADPW     30                   // Minimum value for []PW
#define DEF_MAX_QUADPW    255                   // Maximum ...

#define DEF_MIN_QUADRL      1                   // Minimum value for []RL
#define DEF_MAX_QUADRL      (QUADRL_MODULUS - 1)// Maximum ...


// System Comparison Tolerance
#define SYS_CT  3E-15

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

// Format Control
typedef enum tagFC_INDICES
{
    FCNDX_DECIMAL_SEP,      // 00:  Decimal separator (L'.')
    FCNDX_THOUSANDS_SEP,    // 01:  Thousands separator (L',')
    FCNDX_FBE_8_FILL,       // 02:  Format-by-example '8' fill (L'*')
    FCNDX_OVERFLOW_FILL,    // 03:  Format-by-example overflow fill (L'0')
    FCNDX_BLANK_FILL,       // 04:  Blank fill (L'_') (cannot be L",.0123456789")
    FCNDX_OVERBAR,          // 05:  Overbar (WS_UTF16_OVERBAR)
    FCNDX_LENGTH,           // 06:  Length
} FC_INDICES;


typedef struct tagSYSNAME
{
    LPWCHAR     lpwszName;      // Ptr to the name
    UINT        uValence;       // For system functions, Niladic(0), All others (1)
    UBOOL       bSysVar;        // Izit a system variable (TRUE) or function (FALSE)?  If TRUE, uValence is ignored
    LPPRIMFNS   lpNameFcn;      // Ptr to execution routine
    SYS_VARS    sysVarIndex;    // Value of each System Var (0 = Unknown)
} SYSNAME, *LPSYSNAME;


//***************************************************************************
//  End of File: sysvars.h
//***************************************************************************
