//***************************************************************************
//  NARS2000 -- Default System Variable Values
//***************************************************************************

// Default values for system variables in a CLEAR WS
//   when not overridden by user preferences.
#define DEF_QUADCT_CWS  3E-15
#define DEF_QUADxCT_CWS 0x3CEB05876E5B0120  // 3E-15 in hex
#define DEF_QUADFC_CWS  L".,*0_" WS_UTF16_OVERBAR
#define DEF_QUADFC_GLB  hGlbQuadFC
#define DEF_QUADIO_CWS      1
#define DEF_QUADPP_CWS     10
#define DEF_QUADPW_CWS     80
#define DEF_QUADRL_CWS  16807
#define DEF_QUADPR_CWS   L' '
#define DEF_QUADxSA_CWS     0   // 0 = "", 1 = "CLEAR", 2 = "ERROR", 3 = "EXIT", 4 = "OFF"
#define DEF_QUADLX_CWS    L""
#define DEF_QUADLX_GLB  hGlbV0Char
#define WS_QUADDM       (WS_UTF16_QUAD L"DM")
#define DEF_QUADALX_CWS WS_QUADDM
#define DEF_QUADELX_CWS WS_QUADDM
#define DEF_QUADALX_GLB hGlbQuadDM
#define DEF_QUADELX_GLB hGlbQuadDM

#define QUADRL_MODULUS  0x7FFFFFFF      // (2*31)-1 is a prime

#define DEF_MIN_QUADPP   1                  // Minimum value for QuadPP
#define DEF_MAX_QUADPP  17                  // Maximum ...

#define DEF_MIN_QUADPW  30                  // Minimum value for QuadPW
#define DEF_MAX_QUADPW 255                  // Maximum ...

#define DEF_MIN_QUADRL   1                  // Minimum value for QuadRL
#define DEF_MAX_QUADRL (QUADRL_MODULUS - 1) // Maximum ...


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
} SA_VALUES;

// Format Control
typedef enum tagFC_VALUES
{
    FCVAL_DECIMAL_SEP,      // 00:  Decimal separator (L'.')
    FCVAL_THOUSANDS_SEP,    // 01:  Thousands separator (L',')
    FCVAL_FBE_8_FILL,       // 02:  Format-by-example '8' fill (L'*')
    FCVAL_OVERFLOW_FILL,    // 03:  Format-by-example overflow fill (L'0')
    FCVAL_BLANK_FILL,       // 04:  Blank fill (L'_')
    FCVAL_OVERBAR,          // 05:  Overbar (WS_UTF16_OVERBAR)
    FCVAL_LENGTH,           // 06:  Length
} FC_VALUES;


//***************************************************************************
//  End of File: sysvars.h
//***************************************************************************
