//***************************************************************************
//  NARS2000 -- State Indicator Stack Header
//***************************************************************************

// Defined function header signature
#define SIS_HEADER_SIGNATURE   ' SIS'

typedef struct tagSIS_HEADER
{
    HEADER_SIGNATURE Sig;           // 00:  SIS header signature
    HANDLE           hSemaphore,    // 04:  Semaphore handle
                     hSigaphore;    // 08:  Semaphore handle to signal on exit (NULL if none)
    HGLOBAL          hGlbDfnHdr,    // 0C:  Defined function global memory handle
                     hGlbFcnName;   // 10:  Function name global memory handle
    UINT             DfnType:4,     // 14:  0000000F:  Defined Function Type (see DFNTYPES enum)
                     FcnValence:3,  //      00000070:  Defined/Derived Function Valence (see FCNVALENCES enum)
                     DfnAxis:1,     //      00000080:  Defined/Derived Function accepts axis value
                     Suspended:1,   //      00000100:  Function is suspended
                     Avail:23;      //      FFFFFE00:  Available bits
    UINT             CurLineNum,    // 18:  Current line # (origin-1)
                     NxtLineNum,    // 1C:  Next    ...
                     numLabels,     // 20:  # line labels
                     numResultSTE,  // 24:  # result STEs (may be zero if no result)
                     offResultSTE,  // 28:  Offset to result STEs (ignored if numResultSTE is zero)
                     numLftArgSTE,  // 2C:  # left arg STEs (may be zero if niladic/monadic)
                     offLftArgSTE,  // 30:  Offset to left arg STEs (ignored if numLftArgSTE is zero)
                     numRhtArgSTE,  // 34:  # right arg STEs (may be zero if niladic)
                     offRhtArgSTE,  // 38   Offset to right arg STEs (ignored if numRhtArgSTE is zero)
                     numLocalsSTE,  // 3C:  # right arg STEs (may be zero if niladic)
                     offLocalsSTE,  // 40:  Offset to start of function lines (FCNLINE[nLines])
                     numFcnLines,   // 44:  # lines in the function (not counting the header)
                     numSymEntries, // 48:  # SYMENTRYs on the stack
                     SILevel,       // 4C:  SI Level
                     QQPromptLen;   // 50:  Quote-Quad input prompt length
    struct tagSIS_HEADER
                    *lpSISPrv,      // 54:  Ptr to previous SIS header (NULL = none)
                    *lpSISNxt;      // 58:  Ptr to next     ...         ...
    LPTOKEN          lptkFunc;      // 5C:  Ptr to function token for Quote-Quad input
                                    // 60:  Length
                                    // 60:  Array of SYMENTRYs (previous value for results, args, & locals)
} SIS_HEADER, *LPSIS_HEADER;


//***************************************************************************
//  End of File: sis.h
//***************************************************************************
