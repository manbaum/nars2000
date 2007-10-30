//***************************************************************************
//  NARS2000 -- State Indicator Stack Header
//***************************************************************************

typedef enum tagRESET_FLAGS
{
    RESETFLAG_NONE = 0,             // 00:  No resetting
    RESETFLAG_ONE,                  // 01:  Reset one level  -- {goto}
    RESETFLAG_ONE_INIT,             // 02:  Reset one level  -- {goto} (initial case)
    RESETFLAG_ALL,                  // 03:  Reset all levels -- )RESET
    RESETFLAG_QUADERROR_INIT,       // 04:  Reset one level  -- []ERROR (initialization)
    RESETFLAG_QUADERROR_EXEC,       // 05:  Reset one level  -- []ERROR (execute []ELX)
                                    // 06-07:  Available (3 bits)
} RESET_FLAGS;

// User-defined function/operator header signature
#define SIS_HEADER_SIGNATURE   ' SIS'

typedef struct tagSIS_HEADER
{
    HEADER_SIGNATURE Sig;           // 00:  SIS header signature
    HANDLE           hSemaphore,    // 04:  Semaphore handle
                     hSigaphore;    // 08:  Semaphore handle to signal on exit (NULL if none)
    HGLOBAL          hGlbDfnHdr,    // 0C:  User-defined function/operator global memory handle (may be NULL if not fcn/op)
                     hGlbFcnName;   // 10:  Function name global memory handle
    UINT             DfnType:4,     // 14:  0000000F:  User-defined function/operator Type (see DFN_TYPES)
                     FcnValence:3,  //      00000070:  User-defined function/operator Valence (see FCN_VALENCES)
                     DfnAxis:1,     //      00000080:  User-defined function/operator accepts axis value
                     Suspended:1,   //      00000100:  Function is suspended
                     ResetFlag:3,   //      00000E00:  SI stack is resetting (see RESET_FLAGS)
                     Perm:1,        //      00001000:  Permanent (i.e. Magic Function)
                     Avail:19;      //      FFFFE000:  Available bits
    UINT             CurLineNum,    // 18:  Current line # (origin-1)
                     NxtLineNum,    // 1C:  Next    ...
                     numLabels,     // 20:  # line labels
                     numFcnLines,   // 24:  # lines in the function (not counting the header)
                     numSymEntries, // 28:  # SYMENTRYs on the stack
                     QQPromptLen,   // 2C:  Quote-Quad input prompt length
                     ErrorCode;     // 30:  Error code (see ERROR_CODES)
    struct tagSIS_HEADER
                    *lpSISPrv,      // 34:  Ptr to previous SIS header (NULL = none)
                    *lpSISNxt;      // 38:  Ptr to next     ...         ...
    LPTOKEN          lptkFunc;      // 3C:  Ptr to function token for Quote-Quad input
                                    // 40:  Length
                                    // 40:  Array of SYMENTRYs (previous value for results, args, & locals)
} SIS_HEADER, *LPSIS_HEADER;


//***************************************************************************
//  End of File: sis.h
//***************************************************************************
