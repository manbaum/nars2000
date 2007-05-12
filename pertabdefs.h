//***************************************************************************
//  NARS2000 -- Per Tab Definitions
//***************************************************************************

//***************************************************************************
//  The variables defined here are also to be saved in
//    the per tab structure.
//
//  When defining a new variable here, you *MUST* also
//    define it in PERTABVARS macro unless it is truly global
//    such as <YYRes> and <YYResIndex>.
//***************************************************************************

#ifdef DEFINE_VARS
#define EXTERN
#else
#define EXTERN extern
#endif

#define NUMYYRES    10          // # YYRes elements in the array
EXTERN
YYSTYPE YYRes[NUMYYRES];        // The result token


EXTERN
UINT YYResIndex                 // Current index into YYRes
#ifdef DEFINE_VALUES
= 0
#endif
;

EXTERN
HWND hWndMC,                    // Global MDI Client window handle
     hWndSM,                    // ...    Session Manager ...
     hWndDB;                    // ...    Debugger     ...

EXTERN
WNDPROC lpfnOldListboxWndProc;  // Save area for old Listbox procedure

EXTERN
WNDPROC lpfnOldEditCtrlWndProc; // Save area for old Edit Control procedure

EXTERN
LPWCHAR lpwszCurLine,           // The contents of the line
                                //   with the cursor on it.
        lpwszTmpLine;           // Temporary holding area
EXTERN
LPCHAR  lpszNumAlp;             // Accumulator for integers & floating points & names

EXTERN
LPWCHAR lpwszString;            // ...             strings

EXTERN
int     iMaxNumAlp
#ifdef DEFINE_VALUES
 = DEF_NUMALP_MAXSIZE           // Maximum # chars in lpszNumAlp
#endif
,
        iMaxString
#ifdef DEFINE_VALUES
 = DEF_STRING_MAXSIZE           // ...       WCHARs in lpwszString
#endif
;

// Current global values of system variables so we can use them
//   without having to access the actual system variable.
EXTERN
HGLOBAL  hGlbQuadALX,           // []ALX    ([]dm)
         hGlbQuadELX,           // []ELX    ([]dm)
         hGlbQuadLX,            // []LX     ("")
         hGlbQuadSA,            // []SA     ("")
         hGlbQuadWSID,          // []WSID   ("")
         hGlbQuadPR;            // []PR     ("") (When an empty vector)
EXTERN
APLFLOAT fQuadCT;               // []CT
EXTERN
APLBOOL  bQuadIO,               // []IO
         bQuadxSA;              // []SA (in its index form)
EXTERN
APLINT   uQuadPP,               // []PP
         uQuadPW,               // []PW
         uQuadRL;               // []RL
EXTERN
APLCHAR  cQuadPR;               // []PR     (' ') (When a char scalar)

// Hash table variables
EXTERN
LPHSHENTRY  lpHshTab,           // Ptr to start of hash table
            lpHshTabSplitNext;  // ...    next HTE to split (incremented by DEF_HSHTAB_NBLKS)
EXTERN
UINT        uHashMask           // Mask for all hash lookups
#ifdef DEFINE_VALUES
 = DEF_HSHTAB_HASHMASK
#endif
;

// Symbol & hash table variables
EXTERN
LPSYMENTRY  lpSymTab,           // Ptr to start of symbol table
            lpSymTabNext;       // Ptr to next available STE

EXTERN
int         iSymTabTotalSize    // # STEs currently
#ifdef DEFINE_VALUES
 = DEF_SYMTAB_INITSIZE
#endif
,
            iHshTabTotalSize    // # HTEs currently including EPBs
#ifdef DEFINE_VALUES
 = DEF_HSHTAB_INITSIZE
#endif
,
            iHshTabBaseSize     // Base size of hash table
#ifdef DEFINE_VALUES
 = DEF_HSHTAB_INITSIZE
#endif
,
            iHshTabIncr         // Increment when looping through ST
#ifdef DEFINE_VALUES
 = DEF_HSHTAB_INCR
#endif
;

EXTERN
LPWCHAR lpwszErrorMessage;      // Ptr to error message to signal

#ifndef ENUMS_DEFINED
typedef enum tagEXEC_CODES
{
    EXEC_SUCCESS = 0 ,          // All OK
////EXEC_RESULT_BOOL ,          // Result should be Boolean
////EXEC_RESULT_INT  ,          // ...              Integer
    EXEC_RESULT_FLOAT,          // ...              Float
    EXEC_DOMAIN_ERROR,          // Signal a DOMAIN ERROR
} EXEC_CODES;
#endif

EXTERN
EXEC_CODES ExecCode             // Exception code
#ifdef DEFINE_VALUES
 = {EXEC_SUCCESS}
#endif
;

EXTERN
BOOL bTabTextState              // Tab's text state:  Highlight (TRUE) or Normal (FALSE)
#ifdef DEFINE_VALUES
 = FALSE
#endif
;

#define INIT_PERTABVARS                     \
    iMaxNumAlp       = DEF_NUMALP_MAXSIZE;  \
    iMaxString       = DEF_STRING_MAXSIZE;  \
    hGlbQuadWSID     = CopySymGlbDirGlb (hGlbQuadWSID_CWS); \
    uHashMask        = DEF_HSHTAB_HASHMASK; \
    iSymTabTotalSize = DEF_SYMTAB_INITSIZE; \
    iHshTabTotalSize = DEF_HSHTAB_INITSIZE; \
    iHshTabBaseSize  = DEF_HSHTAB_INITSIZE; \
    iHshTabIncr      = DEF_HSHTAB_INCR;     \
    ExecCode         = EXEC_SUCCESS;        \
    bTabTextState    = FALSE;


#ifndef PERTABVARS
#define PERTABVARS                                  \
    /* Save/restore the various handles and ptrs */ \
/***Assign (hWndMC               ); ***/            \
    Assign (hWndSM               );                 \
    Assign (hWndDB               );                 \
    Assign (lpfnOldListboxWndProc);                 \
    Assign (lpfnOldEditCtrlWndProc);                \
    Assign (lpwszCurLine         );                 \
    Assign (lpwszTmpLine         );                 \
    Assign (lpszNumAlp           );                 \
    Assign (lpwszString          );                 \
    Assign (iMaxNumAlp           );                 \
    Assign (iMaxString           );                 \
                                                    \
    /* Save/restore system variable values */       \
    Assign (hGlbQuadALX          );                 \
    Assign (hGlbQuadELX          );                 \
    Assign (hGlbQuadLX           );                 \
    Assign (hGlbQuadSA           );                 \
    Assign (hGlbQuadWSID         );                 \
    Assign (hGlbQuadPR           );                 \
    Assign (fQuadCT              );                 \
    Assign (bQuadIO              );                 \
    Assign (bQuadxSA             );                 \
    Assign (uQuadPP              );                 \
    Assign (uQuadPW              );                 \
    Assign (uQuadRL              );                 \
    Assign (cQuadPR              );                 \
                                                    \
    /* Symbol & Hash table variables */             \
    Assign (lpHshTab             );                 \
    Assign (lpHshTabSplitNext    );                 \
    Assign (uHashMask            );                 \
    Assign (lpSymTab             );                 \
    Assign (lpSymTabNext         );                 \
    Assign (iSymTabTotalSize     );                 \
    Assign (iHshTabTotalSize     );                 \
    Assign (iHshTabBaseSize      );                 \
    Assign (iHshTabIncr          );                 \
                                                    \
    Assign (lpwszErrorMessage    );                 \
    Assign (ExecCode             );                 \
    Assign (bTabTextState        );
#endif

#undef  EXTERN


//***************************************************************************
//  End of File: pertabdefs.h
//***************************************************************************
