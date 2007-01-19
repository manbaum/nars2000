//***************************************************************************
//  NARS2000 -- Default System Variable Values
//***************************************************************************

// Default values for system variables in a CLEAR WS
//   when not overridden by user preferences.
#define DEF_QUADCT_CWS  3E-15
#define DEF_QUADxCT_CWS 0x3CEB05876E5B0120  // 3E-15 in hex
#define DEF_QUADDF_CWS      1
#define DEF_QUADIF_CWS      1
#define DEF_QUADIO_CWS      1
#define DEF_QUADPP_CWS     10
#define DEF_QUADPW_CWS     80
#define DEF_QUADRL_CWS  16807
#define DEF_QUADPR_CWS   L' '
#define DEF_QUADxSA_CWS     0   // 0 = "", 1 = "CLEAR", 2 = "ERROR", 3 = "EXIT", 4 = "OFF"
#define DEF_QUADLX_CWS    L""
#define DEF_QUADALX_CWS (WS_UCS2_QUAD L"DM")
#define DEF_QUADELX_CWS (WS_UCS2_QUAD L"DM")

// System Comparison Tolerance
#define SYS_CT  3E-15

// Various string constants for []SA
#define MTChar      L""

#define SAEmpty     L""
#define SAClear     L"CLEAR"
#define SAError     L"ERROR"
#define SAExit      L"EXIT"
#define SAOff       L"OFF"

enum SA_VALUES
{
    SAVAL_Empty = 0,
    SAVAL_Clear,
    SAVAL_Error,
    SAVAL_Exit,
    SAVAL_Off
};


//***************************************************************************
//  End of File: sysvars.h
//***************************************************************************
