//***************************************************************************
//  NARS2000 -- State Indicator Stack Header
//***************************************************************************

// Defined function header signature
#define SIS_HEADER_SIGNATURE   ' SIS'

typedef struct tagSIS_HEADER
{
    HEADER_SIGNATURE Sig;           // 00:  SIS header signature
    HANDLE           hThread;       // 04:  Thread handle
    HGLOBAL          hGlbDfnHdr,    // 09:  Defined function global memory handle
                     hGlbFcnName;   // 0C:  Function name global memory handle
    UINT             DfnType:2,     // 10:  00000003:  Defined Function Type (see DFNTYPES enum)
                     FcnValence:2,  //      0000000C:  Defined/Derived Function Valence (see FCNVALENCES enum)
                     DfnAxis:1,     //      00000010:  Defined/Derived Function accepts axis value
                     Suspended:1,   //      00000020:  Function is suspended
                     GotoLine:1,    //      00000040:  CurLineNum came from {goto}
                     Avail:25;      //      FFFFFF80:  Available bits
    UINT             CurLineNum,    // 14:  Current line # (origin-1)
                     numLabels,     // 18:  # line labels
                     numResultSTE,  // 1C:  # result STEs (may be zero if no result)
                     offResultSTE,  // 20:  Offset to result STEs (ignored if numResultSTE is zero)
                     numLftArgSTE,  // 24:  # left arg STEs (may be zero if niladic/monadic)
                     offLftArgSTE,  // 28:  Offset to left arg STEs (ignored if numLftArgSTE is zero)
                     numRhtArgSTE,  // 2C:  # right arg STEs (may be zero if niladic)
                     offRhtArgSTE,  // 30   Offset to right arg STEs (ignored if numRhtArgSTE is zero)
                     numLocalsSTE,  // 34:  # right arg STEs (may be zero if niladic)
                     offLocalsSTE,  // 38:  Offset to start of function lines (FCNLINE[nLines])
                     numFcnLines,   // 3C:  # lines in the function (not counting the header)
                     numSymEntries, // 40:  # SYMENTRYs on the stack
                     SILevel;       // 44:  SI Level
    struct tagSIS_HEADER
                    *lpSISPrv,      // 48:  Ptr to previous SIS header (NULL = none)
                    *lpSISNxt;      // 4C:  Ptr to next     ...         ...

                                    // 50:  Length
                                    // 50:  Array of SYMENTRYs (previous value for results, args, & locals)
} SIS_HEADER, *LPSIS_HEADER;


//***************************************************************************
//  End of File: sis.h
//***************************************************************************
