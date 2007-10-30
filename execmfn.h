//***************************************************************************
//  NARS2000 -- Magic Function Execution Header
//***************************************************************************

// Magic function variables
#define $F          L"F"
#define $L          L"L"
#define $L1         L"L1"
#define $O          L"O"
#define $R          L"R"
#define $Z          L"Z"

#define $DECODE     WS_UTF16_UPTACK
#define $DIAMOND    WS_UTF16_DIAMOND
#define $DROP       WS_UTF16_DOWNARROW
#define $ENCLOSE    WS_UTF16_LEFTSHOE
#define $ENCODE     WS_UTF16_DOWNTACK
#define $EPSILON    WS_UTF16_EPSILON
#define $GOTO       WS_UTF16_RIGHTARROW
#define $IOTA       WS_UTF16_IOTA
#define $IS         WS_UTF16_LEFTARROW
#define $NEG        WS_UTF16_OVERBAR
#define $NOTEQUAL   WS_UTF16_NOTEQUAL
#define $QUAD_IO    WS_UTF16_QUAD L"IO"
#define $RHO        WS_UTF16_RHO
#define $TAKE       WS_UTF16_UPARROW


typedef struct tagMAGIC_FUNCTION
{
    LPAPLCHAR Header;           // Ptr to function header
    LPAPLCHAR *Body;            // Ptr to array of function lines
    UINT      numFcnLines;      // # function lines in the body
} MAGIC_FUNCTION, *LPMAGIC_FUNCTION;


//***************************************************************************
//  End of File: execmfn.h
//***************************************************************************
