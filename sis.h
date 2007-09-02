//***************************************************************************
//  NARS2000 -- State Indicator Stack Header
//***************************************************************************

// User-defined function/operator header signature
#define SIS_HEADER_SIGNATURE   ' SIS'

typedef struct tagSIS_HEADER
{
    HEADER_SIGNATURE Sig;           // 00:  SIS header signature
    HANDLE           hSemaphore,    // 04:  Semaphore handle
                     hSigaphore,    // 08:  Semaphore handle to signal on exit (NULL if none)
                     hSemaReset;    // 0C:  Semaphore handle for )RESET
    HGLOBAL          hGlbDfnHdr,    // 10:  User-defined function/operator global memory handle
                     hGlbFcnName;   // 14:  Function name global memory handle
    UINT             DfnType:4,     // 18:  0000000F:  User-defined function/operator Type (see enum DFN_TYPES)
                     FcnValence:3,  //      00000070:  User-defined function/operator Valence (see enum FCN_VALENCES)
                     DfnAxis:1,     //      00000080:  User-defined function/operator accepts axis value
                     Suspended:1,   //      00000100:  Function is suspended
                     Resetting:1,   //      00000200:  SI stack is resetting
                     Reset1Lvl:1,   //      00000400:  SI stack is resetting one level of suspension
                     Avail:21;      //      FFFFF800:  Available bits
    UINT             CurLineNum,    // 1C:  Current line # (origin-1)
                     NxtLineNum,    // 20:  Next    ...
                     numLabels,     // 24:  # line labels
                     numResultSTE,  // 28:  # result STEs (may be zero if no result)
                     offResultSTE,  // 2C:  Offset to result STEs (ignored if numResultSTE is zero)
                     numLftArgSTE,  // 30:  # left arg STEs (may be zero if niladic/monadic)
                     offLftArgSTE,  // 34:  Offset to left arg STEs (ignored if numLftArgSTE is zero)
                     numRhtArgSTE,  // 38:  # right arg STEs (may be zero if niladic)
                     offRhtArgSTE,  // 3C:  Offset to right arg STEs (ignored if numRhtArgSTE is zero)
                     numLocalsSTE,  // 40:  # right arg STEs (may be zero if niladic)
                     offLocalsSTE,  // 44:  Offset to start of function lines (FCNLINE[nLines])
                     numFcnLines,   // 48:  # lines in the function (not counting the header)
                     numSymEntries, // 4C:  # SYMENTRYs on the stack
                     QQPromptLen,   // 50:  Quote-Quad input prompt length
                     ErrorCode;     // 54:  Error code (see ERRORCODES)
    struct tagSIS_HEADER
                    *lpSISPrv,      // 58:  Ptr to previous SIS header (NULL = none)
                    *lpSISNxt;      // 5C:  Ptr to next     ...         ...
    LPTOKEN          lptkFunc;      // 60:  Ptr to function token for Quote-Quad input
                                    // 64:  Length
                                    // 64:  Array of SYMENTRYs (previous value for results, args, & locals)
} SIS_HEADER, *LPSIS_HEADER;


//***************************************************************************
//  End of File: sis.h
//***************************************************************************
