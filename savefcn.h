//***************************************************************************
//  NARS2000 -- Save Function Header
//***************************************************************************

#define LPFX_SAVE2  struct tagFX_SAVE *

typedef struct tagFX_SAVE
{
    HGLOBAL    hGlbRht;             // 00:  Right arg global memory handle
    APLRANK    aplRankRht;          // 04:  Right arg rank
    APLLONGEST aplLongestRht;       // 0C:  Right arg immediate value
    APLDIM     aplRowsRht,          // 14:  Right arg # rows
               aplColsRht;          // 1C:  ...         cols
    BOOL       bRet;                // 24:  TRUE iff result is valid
    UINT       uErrLine;            // 28:  If (!bRet), the line in error (origin-0) (NEG1U = Error Message valid)
    LPSYMENTRY lpSymName;           // 2C:  If (bRet), ptr to SYMENTRY of the function name
    LPTOKEN    lptkFunc;            // 30:  Ptr to function token ([]FX only)
    UINT (*SF_LineLen) (HWND, LPFX_SAVE2, UINT);                // 34:  Ptr to get line len function
    void (*SF_ReadLine) (HWND, LPFX_SAVE2, UINT, LPAPLCHAR);    // 38:  Ptr to read line function
    UINT (*SF_NumLines) (HWND, LPFX_SAVE2);                     // 3C:  Ptr to get # function lines function
} FX_SAVE, *LPFX_SAVE;


//***************************************************************************
//  End of File: savefcn.h
//***************************************************************************
