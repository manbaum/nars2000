//***************************************************************************
//  NARS2000 -- Display Debug Routines
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "resdebug.h"
#include "termcode.h"
#include "externs.h"
#include "dfnhdr.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


#ifdef DEBUG
// Resource debugging variables
extern int iCountGLOBAL[MAXOBJ];
extern HANDLE ahGLOBAL[MAXOBJ];
extern UINT auLinNumGLOBAL[MAXOBJ];
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayHshTab
//
//  Display the Hash Table entries
//***************************************************************************

void DisplayHshTab
    (void)

{
    LPHSHENTRY   lpHshEntry;
    int          i, j;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    typedef struct tagHT_FLAGNAMES
    {
        UINT    uMask;
        LPWCHAR lpwszName;
    } HT_FLAGNAMES, *LPHT_FLAGNAMES;

    // Hash Table flag names
    static HT_FLAGNAMES ahtFlagNames[] =
    {
     {0x00001,  L" Inuse"      },
     {0x00002,  L" PrinHash"   },
     {0x00004,  L" CharIsValid"},
     {0x00008,  L" Temp"       },
    };

// The # rows in the above table
#define HT_FLAGNAMES_NROWS  (sizeof (ahtFlagNames) / sizeof (ahtFlagNames[0]))

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    DbgMsg ("********** Hash Table **********************************");

    wsprintf (lpszDebug,
              "lpHshTab = %08X, SplitNext = %08X, Last = %08X",
              lpMemPTD->lpHshTab,
              lpMemPTD->lpHshTabSplitNext,
             &lpMemPTD->lpHshTab[lpMemPTD->iHshTabTotalSize]);
    DbgMsg (lpszDebug);

    // Display the hash table
    for (lpHshEntry = lpMemPTD->lpHshTab, i = 0;
         i < lpMemPTD->iHshTabTotalSize;
         lpHshEntry++, i++)
    {
        WCHAR wszFlags[128] = {L'\0'};
        UINT  htFlags;

        // Format the flags
        htFlags = *(PUINT_PTR) &lpHshEntry->htFlags;
        for (j = 0;
             j < HT_FLAGNAMES_NROWS;
             j++)
        if (htFlags & ahtFlagNames[j].uMask)
            lstrcatW (wszFlags, ahtFlagNames[j].lpwszName);

        // In case we didn't find any matching flags,
        //   set the second byte to zero as well as
        //   when we do find flags, we skip over the
        //   leading blank.
        if (wszFlags[0] EQ L'\0')
            wszFlags[1] =  L'\0';

        if (lpHshEntry->htFlags.Inuse
         && lpHshEntry->htSymEntry)
        {
            LPSYMENTRY lpSymEntry;

            lpSymEntry = lpHshEntry->htSymEntry;
            if (lpSymEntry->stFlags.Imm)
                wsprintfW (lpwszDebug,
                           L"HT:%3d uH=%08X, uH&M=%d, <%s>, ull=%08X%08X, Sym=%08X",
                           i,
                           lpHshEntry->uHash,
                           lpHshEntry->uHashAndMask,
                          &wszFlags[1],
                           HIDWORD (lpSymEntry->stData.stInteger),
                           LODWORD (lpSymEntry->stData.stInteger),
                           lpSymEntry);
            else
            if (lpSymEntry->stFlags.ObjName NE OBJNAME_NONE)
            {
                LPCHAR lpGlbName;

                // Lock the memory to get a ptr to it
                lpGlbName = GlobalLock (lpHshEntry->htGlbName); Assert (lpGlbName NE NULL);

                wsprintfW (lpwszDebug,
                           L"HT:%3d uH=%08X, uH&M=%d, <%s>, <%s>, Sym=%08X, %08X-%08X",
                           i,
                           lpHshEntry->uHash,
                           lpHshEntry->uHashAndMask,
                          &wszFlags[1],
                           lpGlbName,
                           lpSymEntry,
                           lpHshEntry->NextSameHash,
                           lpHshEntry->PrevSameHash);
                // We no longer need this ptr
                GlobalUnlock (lpHshEntry->htGlbName); lpGlbName = NULL;
            } // End IF/ELSE/IF
        } else
            wsprintfW (lpwszDebug,
                       L"HT:%3d (EMPTY) <%s>, Sym=%08X, <%08X-%08X>",
                       i,
                      &wszFlags[1],
                       lpHshEntry->htSymEntry,
                       lpHshEntry->NextSameHash,
                       lpHshEntry->PrevSameHash);
        DbgMsgW (lpwszDebug);
    } // End FOR

    DbgMsg ("********** End Hash Table ******************************");

    UpdateDBWindow ();

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End DisplayHshTab
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplaySymTab
//
//  Display the Symbol Table entries
//
//  If bDispAll is FALSE, display only those non-OBJNAME_SYS entries
//    with a non-zero reference count.
//***************************************************************************

void DisplaySymTab
    (BOOL bDispAll)

{
    LPSYMENTRY   lpSymEntry;    // Ptr to current SYMENTRY
    int          i, j;          // Loop counters
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    typedef struct tagST_FLAGNAMES
    {
        UINT    uMask;
        LPWCHAR lpwszName;
    } ST_FLAGNAMES, *LPST_FLAGNAMES;

    // Symbol Table flag names
    static ST_FLAGNAMES astFlagNames[] =
    {
//// {0x00000001,  L" Imm"        },
//// {0x0000001E,  L" ImmType"    },
     {0x00000020,  L" NotCase"    },
     {0x00000040,  L" Perm"       },
     {0x00000080,  L" Inuse"      },
     {0x00000100,  L" Value"      },
/////{0x00000600,  L" ObjName"    },
//// {0x00003800,  L" UsrType"    },
//// {0x0003C000,  L" SysVarValid"},
     {0x00040000,  L" UsrDfn"     },
     {0x00080000,  L" DfnLabel"   },
     {0x00100000,  L" DfnSysLabel"},
     {0x00200000,  L" DfnAxis"    },
     {0x00400000,  L" FcnDir"     },
    };

// The # rows in the above table
#define ST_FLAGNAMES_NROWS  (sizeof (astFlagNames) / sizeof (astFlagNames[0]))

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    if (bDispAll)
        DbgMsg ("********** Symbol Table ********************************");
    else
        DbgMsg ("********** Symbol Table Referenced Non-SysNames ********");

    wsprintf (lpszDebug,
              "lpSymTab = %08X, Last = %08X",
              lpMemPTD->lpSymTab,
              &lpMemPTD->lpSymTab[lpMemPTD->iSymTabTotalSize]);
    DbgMsg (lpszDebug);

    // Display the symbol table
    for (lpSymEntry = lpMemPTD->lpSymTab, i = 0;
         lpSymEntry NE lpMemPTD->lpSymTabNext;
         lpSymEntry++, i++)
    if (bDispAll ||
        lpSymEntry->stFlags.ObjName NE OBJNAME_SYS)
    {
        WCHAR   wszFlags[128] = {L'\0'};
        STFLAGS stFlags;
        LPWCHAR lpGlbName;

        // Format the flags
        stFlags = lpSymEntry->stFlags;
        if (stFlags.Imm)
            wsprintfW (&wszFlags[lstrlenW (wszFlags)],
                       L" Imm/Type=%d",
                       stFlags.ImmType);
        if (stFlags.ObjName NE OBJNAME_NONE)
            wsprintfW (&wszFlags[lstrlenW (wszFlags)],
                       L" ObjName=%s",
                       lpwObjNameStr[stFlags.ObjName]);
        if (stFlags.ObjType NE NAMETYPE_UNK)
            wsprintfW (&wszFlags[lstrlenW (wszFlags)],
                       L" ObjType=%s",
                       lpwNameTypeStr[stFlags.ObjType]);

        for (j = 0;
             j < ST_FLAGNAMES_NROWS;
             j++)
        if ((*(PUINT_PTR) &stFlags) & astFlagNames[j].uMask)
            lstrcatW (wszFlags, astFlagNames[j].lpwszName);

        if (IsNameTypeVar (stFlags.ObjType)
         && !stFlags.DfnSysLabel)
            wsprintfW (&wszFlags[lstrlenW (wszFlags)],
                       L" SysVarValid=%d",
                       stFlags.SysVarValid);

        // In case we didn't find any matching flags,
        //   set the second WCHAR to zero as well --
        //   when we do find flags, we skip over the
        //   leading blank.
        if (wszFlags[0] EQ L'\0')
            wszFlags[1] =  L'\0';

        if (lpSymEntry->stFlags.Inuse)
        {
#define WSZNAME_LEN     128
            WCHAR wszName[WSZNAME_LEN] = {'\0'};

            if (lpSymEntry->stFlags.ObjName NE OBJNAME_NONE)
            {
                LPHSHENTRY lpHshEntry;

                lpHshEntry = lpSymEntry->stHshEntry;

                if (lpHshEntry)
                {
                    lpGlbName = GlobalLock (lpHshEntry->htGlbName); Assert (lpGlbName NE NULL);

                    lstrcpynW (wszName, lpGlbName, WSZNAME_LEN);

                    // We no longer need this ptr
                    GlobalUnlock (lpHshEntry->htGlbName); lpGlbName = NULL;
                } // End IF
            } // End IF

            if (lpSymEntry->stFlags.Imm)
            {
                wsprintfW (lpwszDebug,
                           L"ST:%08X <%s> <%s>, ull=%08X%08X, Hsh=%08X",
                           lpSymEntry,
                           wszName,
                          &wszFlags[1],
                           HIDWORD (lpSymEntry->stData.stInteger),
                           LODWORD (lpSymEntry->stData.stInteger),
                           lpSymEntry->stHshEntry);
            } else
            if (lpSymEntry->stFlags.ObjName NE OBJNAME_NONE)
            {
                LPHSHENTRY lpHshEntry;

                lpHshEntry = lpSymEntry->stHshEntry;

                if (lpHshEntry)
                {
                    wsprintfW (lpwszDebug,
                               L"ST:%08X <%s>, <%s>, Data=%08X, Hsh=%08X",
                               lpSymEntry,
                               wszName,
                              &wszFlags[1],
                               lpSymEntry->stData.stVoid,
                               lpHshEntry);
                } else
                    wsprintfW (lpwszDebug,
                               L"ST:%08X <******>, <%s>, Hsh=0",
                               lpSymEntry,
                              &wszFlags[1]);
            } // End IF/ELSE/IF
        } else
            wsprintfW (lpwszDebug,
                       L"ST:%08X (EMPTY) <%s>, Hsh=%08X",
                       lpSymEntry,
                      &wszFlags[1],
                       lpSymEntry->stHshEntry);
        DbgMsgW (lpwszDebug);
    } // End FOR

    DbgMsg ("********** End Symbol Table ****************************");

    UpdateDBWindow ();

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End DisplaySymTab
#endif


#ifdef DEBUG
//***************************************************************************
//  $UpdateDBWindow
//
//  Call UpdateWindow on the DB window
//***************************************************************************

void UpdateDBWindow
    (void)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    UpdateWindow (lpMemPTD->hWndDB);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End UpdateDBWindow
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayGlobals
//
//  Display outstanding global memory objects
//***************************************************************************
void DisplayGlobals
    (UINT uDispGlb)     // 0 = Display non-permanent non-sysvars
                        // 1 = ...     non-sysvars
                        // 2 = ...     all globals

{
    int          i;
    HGLOBAL      hGlb;
    LPVOID       lpMem;
    APLDIM       aplDim;
    LPVOID       lpData;
    APLCHAR      aplArrChar[19];
    LPAPLCHAR    lpwsz;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    // Because we're inside DisplayGlobals, don't use
    //   MyGlobalLock/Unlock so as not to pollute the result
    lpMemPTD = GlobalLock (hGlbPTD);

    DbgMsg ("********** Globals *************************************");

    for (i = 0; i < MAXOBJ; i++)
    if (hGlb = ahGLOBAL[i])
    {
        // Note we don't use MyGlobalLock here as that function might well
        //   fail and trigger a hard error which we prefer to catch softly.

        // Lock the memory to get a ptr to it
        lpMem = GlobalLock (hGlb);
        if (!lpMem)
        {
            wsprintf (lpszDebug,
                      "hGlb=%08X *** INVALID ***",
                      hGlb);
            DbgMsg (lpszDebug);

            continue;
        } // End IF

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
        if (lpHeader->Sig.nature EQ VARARRAY_HEADER_SIGNATURE)
        {
            if (uDispGlb EQ 2
             || !lpHeader->SysVar)
            {
                // It's a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (MakePtrTypeGlb (hGlb)));

                if (lpHeader->Rank EQ 0)
                    aplDim = (APLDIM) -1;
                else
                    aplDim = *VarArrayBaseToDim (lpHeader);
                // Skip over the header and dimension to the data
                lpData = VarArrayBaseToData (lpHeader, lpHeader->Rank);

                // Split cases based upon the array storage type
                switch (lpHeader->ArrType)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                    case ARRAY_FLOAT:
                    case ARRAY_APA:
                        if (lpHeader->NELM EQ 0)
                        {
                            aplArrChar[0] = UTF16_ZILDE;
                            aplArrChar[1] = L'\0';
                        } else
                        {
                            lpwsz =
                            FormatImmed (aplArrChar,
                                         TranslateArrayTypeToImmType (lpHeader->ArrType),
                                         (LPAPLLONGEST) lpData);
                            // Delete the trailing blank
                            lpwsz[-1] = L'\0';
                        } // End IF/ELSE

                        break;

                    case ARRAY_HETERO:
                    case ARRAY_NESTED:
                        aplArrChar[0] = L'\0';

                        break;

                    case ARRAY_CHAR:
                        lstrcpynW (aplArrChar, lpData, 1 + (UINT) min (6, lpHeader->NELM));
                        aplArrChar[min (6, lpHeader->NELM)] = L'\0';

                        break;

                    case ARRAY_LIST:
                        lstrcpyW (aplArrChar, L"[...]");

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Check for non-permanents
                if (uDispGlb EQ 1
                 || uDispGlb EQ 2
                 || !lpHeader->Perm)
                {
                    wsprintfW (lpwszDebug,
                               L"hGlb=%08X, ArrType=%c%c, NELM=%3d, RC=%1d, Rank=%2d, Dim1=%3d, Lock=%d, Line#=%4d, (%s)",
                               hGlb,
                               L"BIFCHNLA"[lpHeader->ArrType],
                               L" *"[lpHeader->Perm],
                               LODWORD (lpHeader->NELM),
                               lpHeader->RefCnt,
                               LODWORD (lpHeader->Rank),
                               LODWORD (aplDim),
                               (MyGlobalFlags (hGlb) & GMEM_LOCKCOUNT) - 1,
                               auLinNumGLOBAL[i],
                               aplArrChar);
                    DbgMsgW (lpwszDebug);
                } // End IF
            } // End IF
        } else
#undef  lpHeader
#define lpHeader    ((LPFCNARRAY_HEADER) lpMem)
        if (lpHeader->Sig.nature EQ FCNARRAY_HEADER_SIGNATURE)
        {
            // It's a valid HGLOBAL function array
            Assert (IsGlbTypeFcnDir (MakePtrTypeGlb (hGlb)));

            wsprintf (lpszDebug,
                      "hGlb=%08X, NamTyp=%s, NELM=%3d, RC=%1d,                    Lock=%d, Line#=%4d",
                      hGlb,
                      lpNameTypeStr[lpHeader->NameType],
                      lpHeader->fcnNELM,
                      lpHeader->RefCnt,
                      (MyGlobalFlags (hGlb) & GMEM_LOCKCOUNT) - 1,
                      auLinNumGLOBAL[i]);
            DbgMsg (lpszDebug);
        } else
#undef  lpHeader
        if (uDispGlb EQ 2)
        {
            wsprintf (lpszDebug,
                      "hGlb=%08X -- No NARS/FCNS Signature",
                      hGlb);
            DbgMsg (lpszDebug);
        } // End IF/ELSE

        // We no longer need this ptr
        GlobalUnlock (hGlb); lpMem = NULL;
    } // End FOR/IF

    DbgMsg ("********** End Globals *********************************");

    UpdateDBWindow ();

    // We no longer need this ptr
    GlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End DisplayGlobals
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayTokens
//
//  Display the contents of the current token stream
//***************************************************************************

void DisplayTokens
    (HGLOBAL hGlbToken)

{
    LPTOKEN      lpToken;
    int          i, iLen;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    if (gDbgLvl <= 2)
        return;

    DbgMsg ("********** Tokens **************************************");

    // Ensure it's valid
    if (!hGlbToken)
    {
        DbgMsg ("DisplayTokens:  ***INAVLID HANDLE***:  hGlbToken == 0");
        return;
    } // End IF

    // Lock the memory to get a ptr to it
    lpToken = MyGlobalLock (hGlbToken);

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

#define lpHeader    ((LPTOKEN_HEADER) lpToken)
    wsprintf (lpszDebug,
              "lpToken = %08X, Version # = %d, TokenCnt = %d, PrevGroup = %d",
              lpToken,
              lpHeader->Version,
              lpHeader->TokenCnt,
              lpHeader->PrevGroup);
    DbgMsg (lpszDebug);

    iLen = lpHeader->TokenCnt;
#undef  lpHeader

    lpToken = TokenBaseToStart (lpToken);   // Skip over TOKEN_HEADER

    for (i = 0; i < iLen; i++, lpToken++)
    {
        wsprintf (lpszDebug,
                  "(%2d) Data=%08X%08X, CharIndex=%2d, Type=%s",
                  i,
                  HIDWORD (*(LPAPLINT) &lpToken->tkData.tkFloat),
                  LODWORD (*(LPAPLINT) &lpToken->tkData.tkFloat),
                  lpToken->tkCharIndex,
                  GetTokenTypeName (lpToken->tkFlags.TknType));
        DbgMsg (lpszDebug);
    } // End FOR

    DbgMsg ("********** End Tokens **********************************");

    // We no longer need this ptr
    MyGlobalUnlock (hGlbToken); lpToken = NULL;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End DisplayTokens
#endif


#ifdef DEBUG
//***************************************************************************
//  $GetTokenTypeName
//
//  Convert a token type value to a name
//***************************************************************************

LPCHAR GetTokenTypeName
    (UINT uType)

{
typedef struct tagTOKENNAMES
{
    LPCHAR lpsz;
    UINT   uTokenNum;
} TOKENNAMES, *LPTOKENNAMES;

static TOKENNAMES tokenNames[] =
{{"VARNAMED"  , TKT_VARNAMED }, // 01: Symbol table entry for a named var (data is LPSYMENTRY)
 {"STRING"    , TKT_STRING   }, // 02: String  (data is HGLOBAL)
 {"VARIMMED"  , TKT_VARIMMED }, // 03: Immediate data (data is immediate)
 {"COMMENT"   , TKT_COMMENT  }, // 04: Comment (data is NULL)
 {"ASSIGN"    , TKT_ASSIGN   }, // 05: Assignment symbol (data is UTF16_LEFTARROW)
 {"LISTSEP"   , TKT_LISTSEP  }, // 06: List separator    (data is ';')
 {"LABELSEP"  , TKT_LABELSEP }, // 07: Label ...         (data is ':')
 {"COLON"     , TKT_COLON    }, // 08: Colon             (data is ':')
 {"FCNIMMED"  , TKT_FCNIMMED }, // 09: Primitive function (any valence) (data is UTF16_***)
 {"OP1IMMED"  , TKT_OP1IMMED }, // 0A: Monadic primitive operator (data is UTF16_***)
 {"OP2IMMED"  , TKT_OP2IMMED }, // 0B: Dyadic  ...
 {"OP3IMMED"  , TKT_OP3IMMED }, // 0C: Ambiguous ...
 {"OPJOTDOT"  , TKT_OPJOTDOT }, // 0D: Outer product monadic operator (with right scope) (data is NULL)
 {"LPAREN"    , TKT_LPAREN   }, // 0E: Left paren (data is TKT_LPAREN)
 {"RPAREN"    , TKT_RPAREN   }, // 0F: Right ...   ...         RPAREN
 {"LBRACKET"  , TKT_LBRACKET }, // 10: Left bracket ...        LBRACKET
 {"RBRACKET"  , TKT_RBRACKET }, // 11: Right ...   ...         RBRACKET
 {"EOS"       , TKT_EOS      }, // 12: End-of-Stmt (data is length of stmt including this token)
 {"EOL"       , TKT_EOL      }, // 13: End-of-Line  ...
 {"SOS"       , TKT_SOS      }, // 14: Start-of-Stmt (data is NULL)
 {"LINECONT"  , TKT_LINECONT }, // 15: Line continuation (data is NULL)
 {"INPOUT"    , TKT_INPOUT   }, // 16: Input/Output (data is UTF16_QUAD or UTF16_QUOTEQUAD)
 {"STRAND"    , TKT_STRAND   }, // 17: Strand accumulating (data is LPTOKEN)
 {"LISTINT"   , TKT_LISTINT  }, // 18: List in parens    (data is HGLOBAL)
 {"LISTPAR"   , TKT_LISTPAR  }, // 19: List in parens    (data is HGLOBAL)
 {"LISTBR"    , TKT_LISTBR   }, // 1A: List in brackets  (data is HGLOBAL)
 {"VARARRAY"  , TKT_VARARRAY }, // 1B: Array of data (data is HGLOBAL)
 {"FCNARRAY"  , TKT_FCNARRAY }, // 1C: Array of functions (data is HGLOBAL)
 {"FCNNAMED"  , TKT_FCNNAMED }, // 1D: Symbol table entry for a named function (data is LPSYMENTRY)
 {"AXISIMMED" , TKT_AXISIMMED}, // 1E: An immediate axis specification (data is immediate)
 {"AXISARRAY" , TKT_AXISARRAY}, // 1F: An array of  ...   (data is HGLOBAL)
 {"OP1NAMED"  , TKT_OP1NAMED }, // 20: A named monadic primitive operator (data is LPSYMENTRY)
 {"OP2NAMED"  , TKT_OP2NAMED }, // 21: ...     dyadic  ...
 {"OP3NAMED"  , TKT_OP3NAMED }, // 22: ...     ambiguous ...
 {"STRNAMED"  , TKT_STRNAMED }, // 23: ...     strand  ...
};

// The # rows in the above table
#define TOKENNAMES_NROWS    (sizeof (tokenNames) / sizeof (tokenNames[0]))

    if (TOKENNAMES_NROWS > (uType - TKT_FIRST))
        return tokenNames[uType - TKT_FIRST].lpsz;
    else
    {
        static char szTemp[64];

        wsprintf (szTemp,
                  "GetTokenTypeName:  *** Unknown Token Type:  %d",
                  uType);
        return szTemp;
    } // End IF/ELSE
} // End GetTokenTypeName
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayFcnStrand
//
//  Display a function strand
//***************************************************************************

void DisplayFcnStrand
    (LPTOKEN lptkFunc)          // Ptr to function token

{
    HGLOBAL      hGlbData;      // Function array global memory handle
    LPWCHAR      lpaplChar;     // Ptr to output save area
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    lpaplChar = lpwszDebug;

    // Split cases based upon the token type
    switch (lptkFunc->tkFlags.TknType)
    {
        case TKT_VARNAMED:          // Because we don't distinguish between
                                    //   functions and variables in AssignName_EM
            DbgBrk ();          // ***FINISHME*** -- TKT_VARNAMED in DisplayFcnStrand






            goto NORMAL_EXIT;

        case TKT_FCNIMMED:
        case TKT_OP1IMMED:
        case TKT_OP2IMMED:
        case TKT_OP3IMMED:
            lpaplChar += wsprintfW (lpaplChar,
                                    L"NameType=%1d, NELM=%3d, RC=%2d, Fn:  ",
                                    NAMETYPE_FN12,  // lpHeader->NameType,
                                    1,              // LODWORD (lpHeader->NELM),
                                    0);             // lpHeader->RefCnt);
            *lpaplChar++ = lptkFunc->tkData.tkChar;

            break;

        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
        case TKT_OP3NAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkFunc->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, ...
            if (!lptkFunc->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lptkFunc->tkData.tkSym->stData.stGlbData;

                // stData is an internal function or a HGLOBAL function array
                //   or user-defined function/operator
                Assert (lptkFunc->tkData.tkSym->stFlags.FcnDir
                     || IsGlbTypeFcnDir (hGlbData)
                     || IsGlbTypeDfnDir (hGlbData));

                // Check for internal functions
                if (lptkFunc->tkData.tkSym->stFlags.FcnDir)
                {
                    // Start off with the header
                    lpaplChar += wsprintfW (lpaplChar,
                                            L"NameType=%1d, NELM=%3d, RC=%2d, Fn:  ",
                                            NAMETYPE_FN12,  // lpHeader->NameType,
                                            1,              // LODWORD (lpHeader->NELM),
                                            0);             // lpHeader->RefCnt);
                    // Display the function name from the symbol table
                    lpaplChar = CopySteName (lpaplChar, lptkFunc->tkData.tkSym);
                } else
                if (!lptkFunc->tkData.tkSym->stFlags.UsrDfn)
                    lpaplChar = DisplayFcnGlb (lpaplChar, ClrPtrTypeDirAsGlb (hGlbData), TRUE);
            } else
            {
                // Start off with the header
                lpaplChar += wsprintfW (lpaplChar,
                                        L"NameType=%1d, NELM=%3d, RC=%2d, Fn:  ",
                                        NAMETYPE_FN12,  // lpHeader->NameType,
                                        1,              // LODWORD (lpHeader->NELM),
                                        0);             // lpHeader->RefCnt);
                *lpaplChar++ = lptkFunc->tkData.tkSym->stData.stChar;
            } // End IF/ELSE

            break;

        case TKT_FCNARRAY:
            hGlbData = lptkFunc->tkData.tkGlbData;

            // tkData is an HGLOBAL function array
            Assert (IsGlbTypeFcnDir (hGlbData));

            lpaplChar = DisplayFcnGlb (lpaplChar, ClrPtrTypeDirAsGlb (hGlbData), TRUE);

            break;

        defstop
            break;
    } // End SWITCH

    // Ensure properly terminated
    *lpaplChar = L'\0';

    // Display the line in the debugging window
    DbgMsgW (lpwszDebug);
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End DisplayFcnStrand
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayFcnGlb
//
//  Display a function from an HGLOBAL
//***************************************************************************

LPWCHAR DisplayFcnGlb
    (LPWCHAR lpaplChar,         // Ptr to output save area
     HGLOBAL hGlbFcnArr,        // Function array global memory handle
     BOOL    bDispHeader)       // TRUE iff we're to display the header (NameType=...)

{
    LPVOID  lpMemFcnArr;    // Ptr to function array global memory
    UINT    fcnNELM;        // Function array NELM

    // Lock the memory to get a ptr to it
    lpMemFcnArr = MyGlobalLock (hGlbFcnArr);

#define lpHeader    ((LPFCNARRAY_HEADER) lpMemFcnArr)
    fcnNELM = lpHeader->fcnNELM;

    if (bDispHeader)
        lpaplChar += wsprintfW (lpaplChar,
                                L"NameType=%s, NELM=%3d, RC=%2d, Fn:  ",
                                lpwNameTypeStr[lpHeader->NameType],
                                fcnNELM,
                                lpHeader->RefCnt);
#undef  lpHeader

    lpaplChar =
      DisplayFcnSub (lpaplChar,
                     FcnArrayBaseToData (lpMemFcnArr),  // Skip over the header to the data (PL_YYSTYPEs)
                     fcnNELM);
    // We no longer need this ptr
    MyGlobalUnlock (hGlbFcnArr); lpMemFcnArr = NULL;

    return lpaplChar;
} // End DisplayFcnGlb
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayFcnSub
//
//  Display function subroutine
//***************************************************************************

LPWCHAR DisplayFcnSub
    (LPWCHAR      lpaplChar,    // Ptr to output save area
     LPPL_YYSTYPE lpYYMem,      // Ptr to function array
     UINT         fcnNELM)      // Function NELM

{
    HGLOBAL hGlbData;           // Function array global memory handle
    LPVOID  lpMemData;          // Ptr to function array global memory
    UINT    FcnCount;           // Function count

    // Split cases based upon the token type
    switch (lpYYMem[0].tkToken.tkFlags.TknType)
    {
        case TKT_OP1IMMED:
        case TKT_OP3IMMED:
            // Check for axis operator
            if (fcnNELM > 1
             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
            {
                // If there's a function, ...
                if (fcnNELM > 2)
                    lpaplChar =
                      DisplayFcnSub (lpaplChar, &lpYYMem[2], fcnNELM - 2);  // Fcn
                *lpaplChar++ = lpYYMem[0].tkToken.tkData.tkChar;            // Op1
                lpaplChar =
                  DisplayFcnSub (lpaplChar, &lpYYMem[1], 1);                // [X]
            } else
            {
                lpaplChar =
                  DisplayFcnSub (lpaplChar, &lpYYMem[1], fcnNELM - 1);      // Fcn
                *lpaplChar++ = lpYYMem[0].tkToken.tkData.tkChar;            // Op1
            } // End IF/ELSE/...

            break;

        case TKT_OP2IMMED:
            FcnCount = 1 + lpYYMem[1].FcnCount;
            lpaplChar =
              DisplayFcnSub (lpaplChar, &lpYYMem[1], lpYYMem[1].FcnCount);  // Lfcn
            *lpaplChar++ = lpYYMem[0].tkToken.tkData.tkChar;                // Op2
            if (lpYYMem[FcnCount].FcnCount > 1)
                *lpaplChar++ = L'(';
            lpaplChar =
              DisplayFcnSub (lpaplChar, &lpYYMem[FcnCount], lpYYMem[FcnCount].FcnCount);  // Rfcn
            if (lpYYMem[FcnCount].FcnCount > 1)
                *lpaplChar++ = L')';
            break;

        case TKT_FCNIMMED:
            *lpaplChar++ = lpYYMem[0].tkToken.tkData.tkChar;                // Fcn

            // Check for axis operator
            if (fcnNELM > 1
             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
                lpaplChar =
                  DisplayFcnSub (lpaplChar, &lpYYMem[1], 1);                // [X]

            break;

        case TKT_OP1NAMED:
            DbgBrk ();          // ***FINISHME*** -- TKT_OP1NAMED in DisplayFcnSub






            break;

        case TKT_OP2NAMED:
            DbgBrk ();          // ***FINISHME*** -- TKT_OP2NAMED in DisplayFcnSub





            break;

        case TKT_OP3NAMED:
            DbgBrk ();          // ***FINISHME*** -- TKT_OP3NAMED in DisplayFcnSub





            break;

        case TKT_FCNNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYMem->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not an immediate function, ...
            if (!lpYYMem->tkToken.tkData.tkSym->stFlags.Imm)
            {
                // Check for internal functions
                if (lpYYMem->tkToken.tkData.tkSym->stFlags.FcnDir)
                    // Copy the internal function name
                    lpaplChar =
                      CopySteName (lpaplChar,
                                   lpYYMem->tkToken.tkData.tkSym);
                else
                {
                    // Get the function array global memory handle
                    hGlbData = lpYYMem->tkToken.tkData.tkSym->stData.stGlbData;

                    // stData is a valid HGLOBAL function array
                    Assert (IsGlbTypeFcnDir (hGlbData));

                    // Display the function strand in global memory
                    lpaplChar =
                      DisplayFcnGlb (lpaplChar, ClrPtrTypeDirAsGlb (hGlbData), FALSE);
                } // End IF/ELSE
            } else
                // Handle the immediate case
                *lpaplChar++ = lpYYMem->tkToken.tkData.tkSym->stData.stChar;

            // Check for axis operator
            if (fcnNELM > 1
             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
                lpaplChar =
                  DisplayFcnSub (lpaplChar, &lpYYMem[1], 1);            // [X]

            break;

        case TKT_OPJOTDOT:
            *lpaplChar++ = UTF16_JOT;
            *lpaplChar++ = L'.';

            // Surround with parens if more than one token
            if (fcnNELM > 2)
                *lpaplChar++ = L'(';
            lpaplChar =
              DisplayFcnSub (lpaplChar, &lpYYMem[1], fcnNELM - 1);
            if (fcnNELM > 2)
                *lpaplChar++ = L')';

            break;

        case TKT_AXISIMMED:
            *lpaplChar++ = L'[';
            lpaplChar =
              FormatAplint (lpaplChar,
                            lpYYMem[0].tkToken.tkData.tkInteger);
            lpaplChar[-1] = L']';   // Overwrite the trailing blank

            break;

        case TKT_AXISARRAY:
            *lpaplChar++ = L'[';

            // Get the axis array global memory handle
            hGlbData = lpYYMem->tkToken.tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbData));
#if TRUE
            lstrcpyW (lpaplChar, L"TKT_AXISARRAY ");    // N.B.:  trailing blank is significant
            lpaplChar += lstrlenW (lpaplChar);
#else
            lpaplChar =
              FormatGlobal (lpaplChar, ClrPtrTypeDirGlb (hGlbData));
#endif
            if (lpaplChar[-1] EQ L' ')
                lpaplChar--;            // Back over the trailing blank
            *lpaplChar++ = L']';

            break;

        case TKT_VARIMMED:
            lpaplChar =
              FormatAplint (lpaplChar,
                            lpYYMem[0].tkToken.tkData.tkInteger);
            if (lpaplChar[-1] EQ L' ')
                lpaplChar--;            // Back over the trailing blank

            break;

        case TKT_VARARRAY:
            // Get the variable array global memory handle
            hGlbData = lpYYMem->tkToken.tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbData));
#if TRUE
            lstrcpyW (lpaplChar, L"TKT_VARARRAY ");     // N.B.:  trailing blank is significant
            lpaplChar += lstrlenW (lpaplChar);
#else
            lpaplChar =
              FormatGlobal (lpaplChar, ClrPtrTypeDirGlb (hGlbData));
#endif
            lpaplChar[-1] = L'\0';  // Overwrite the trailing blank

            break;

        case TKT_FCNARRAY:
            // Get the function array global memory handle
            hGlbData = lpYYMem->tkToken.tkData.tkGlbData;

            // tkData is a valid HGLOBAL function array
            //   or user-defined function/operator
            Assert (IsGlbTypeFcnDir (hGlbData)
                 || IsGlbTypeDfnDir (hGlbData));

            // Clear the ptr type bits
            hGlbData = ClrPtrTypeDirAsGlb (hGlbData);

            // Lock the memory to get a ptr to it
            lpMemData = MyGlobalLock (hGlbData);

            // Split cases based upon the array signature
            switch (((LPHEADER_SIGNATURE) lpMemData)->nature)
            {
                case FCNARRAY_HEADER_SIGNATURE:
#define lpHeader    ((LPFCNARRAY_HEADER) lpMemData)
                    fcnNELM = lpHeader->fcnNELM;
#undef  lpHeader
                    // Surround with parens if more than one token
                    if (fcnNELM > 1)
                        *lpaplChar++ = L'(';
                    lpaplChar =
                      DisplayFcnSub (lpaplChar,
                                     FcnArrayBaseToData (lpMemData),    // Skip over the header to the data (PL_YYSTYPEs)
                                     fcnNELM);
                    if (fcnNELM > 1)
                        *lpaplChar++ = L')';
                    break;

                case DFN_HEADER_SIGNATURE:
                    // Copy the user-defined function/operator name
                    lpaplChar =
                      CopySteName (lpaplChar,
                                   ((LPDFN_HEADER) lpMemData)->steFcnName);
                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbData); lpMemData = NULL;

            break;

        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYMem->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Copy the STE name
            lpaplChar =
              CopySteName (lpaplChar,
                           lpYYMem->tkToken.tkData.tkSym);
            break;

        defstop
            break;
    } // End SWITCH

    // Ensure properly terminated
    *lpaplChar = L'\0';

    return lpaplChar;
} // End DisplayFcnSub
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayFcnArr
//
//  Display a Function Array
//***************************************************************************

void DisplayFcnArr
    (HGLOBAL hGlbStr)           // Function array global memory handle

{
    Assert (IsGlbTypeFcnDir (MakePtrTypeGlb (hGlbStr)));

    DbgMsgW (L"********** Function Array ******************************");

    DisplayFcnGlb (lpwszDebug, hGlbStr, TRUE);

    DbgMsgW (lpwszDebug);

    DbgMsgW (L"********** End Function Array **************************");
} // End DisplayFcnArr
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayStrand
//
//  Display the strand stack
//***************************************************************************

void DisplayStrand
    (int strType)               // Strand type (STRAND_VAR or STRAND_FCN)

{
    LPPL_YYSTYPE  lp,
                  lpLast;
    LPPLLOCALVARS lpplLocalVars;
    HGLOBAL       hGlbPTD;      // PerTabData global memory handle
    LPPERTABDATA  lpMemPTD;     // Ptr to PerTabData global memory

    // Check debug level
    if (gDbgLvl <= 2)
        return;

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    switch (strType)
    {
        case STRAND_VAR:
            DbgMsg ("********** Variable Strands ****************************");

            break;

        case STRAND_FCN:
            DbgMsg ("********** Function Strands ****************************");

            break;

        defstop
            break;
    } // End SWITCH

    wsprintf (lpszDebug,
              "Start=%08X Base=%08X Next=%08X",
              lpplLocalVars->lpYYStrandStart[strType],
              lpplLocalVars->lpYYStrandBase[strType],
              lpplLocalVars->lpYYStrandNext[strType]);
    DbgMsg (lpszDebug);

    for (lp = lpplLocalVars->lpYYStrandStart[strType], lpLast = NULL;
         lp NE lpplLocalVars->lpYYStrandNext[strType];
         lp ++)
    {
        if (lpLast NE lp->lpYYStrandBase)
        {
            DbgMsg ("--------------------------------------------------------");
            lpLast  = lp->lpYYStrandBase;
        } // End IF

        wsprintf (lpszDebug,
                  "Strand (%08X): %-9.9s D=%08X CI=%2d TC=%1d FC=%1d IN=%1d F=%08X B=%08X",
                  lp,
                  GetTokenTypeName (lp->tkToken.tkFlags.TknType),
                  LODWORD (lp->tkToken.tkData.tkInteger),
                  lp->tkToken.tkCharIndex,
                  lp->TknCount,
                  lp->FcnCount,
                  lp->YYIndirect,
                  lp->lpYYFcn,
                  lpLast);
        DbgMsg (lpszDebug);
    } // End FOR

    DbgMsg ("********** End Strands *********************************");

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End DisplayStrand
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayUndo
//
//  Display the Undo Buffer
//***************************************************************************

void DisplayUndo
    (HWND hWnd)         // Window handle of the Edit Control

{
    UINT         uCharPos,
                 uLineCount;
    HGLOBAL      hGlbEC;
    LPWCHAR      lpwsz, p;
    HWND         hWndParent;
    LPUNDO_BUF   lpUndoBeg,     // Ptr to start of Undo Buffer
                 lpUndoNxt;     // ...    next available slot in the Undo Buffer
    BOOL         bShift;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    static LPWCHAR Actions[]={L"None",
                              L"Ins",
                              L"Rep",
                              L"Del",
                              L"Sel",
                              L"Back",
                              L"InsToggle"};

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    DbgMsg ("********** Undo Buffer *********************************");

    // Get the shift key state
    bShift = (GetKeyState (VK_SHIFT) & 0x8000);

    // Get the char position of the caret
    uCharPos = GetCurCharPos (hWnd);

    // Get the lines in the text
    uLineCount = SendMessageW (hWnd, EM_GETLINECOUNT, 0, 0);

    // Get the Edit Control's memory handle
    (long) hGlbEC = SendMessageW (hWnd, EM_GETHANDLE, 0, 0);

    // Display it
    dprintfW (L"Caret position = %d, # lines = %d, hGlbEC = %08X (%S#%d)",
              uCharPos,
              uLineCount,
              hGlbEC,
              FNLN);
    // Lock the memory to get a ptr to it
    lpwsz = MyGlobalLock (hGlbEC);

#define VIS_CR  L'\xAE'
#define VIS_NL  L'\xA9'
#define VIS_HT  L'\xBB'

    // Replace L'\r' with a visible char
    while (p = strchrW (lpwsz, L'\r'))
        *p = VIS_CR;

    // Replace L'\n' with a visible char
    while (p = strchrW (lpwsz, L'\n'))
        *p = VIS_NL;

    // Replace L'\t' with a visible char
    while (p = strchrW (lpwsz, L'\t'))
        *p = VIS_HT;

    // Display it
    dprintfW (L"Text = <%s>",
              lpwsz);

    // Restore L'\t'
    while (p = strchrW (lpwsz, VIS_HT))
        *p = L'\t';

    // Restore L'\n'
    while (p = strchrW (lpwsz, VIS_NL))
        *p = L'\n';

    // Restore L'\r'
    while (p = strchrW (lpwsz, VIS_CR))
        *p = L'\r';

    if (bShift)
        DbgBrk ();

    // We no longer need this ptr
    MyGlobalUnlock (hGlbEC); lpwsz = NULL;

    // Get the parent window handle
    hWndParent = GetParent (hWnd);

    // Get the ptrs to the next available slot in our Undo Buffer
    (long) lpUndoNxt = GetWindowLong (hWndParent, GWLSF_UNDO_NXT);
    (long) lpUndoBeg = GetWindowLong (hWndParent, GWLSF_UNDO_BEG);

    // Loop through the undo buffer entries
    for (; lpUndoBeg < lpUndoNxt; lpUndoBeg++)
    {
        wsprintfW (lpwszDebug,
                   L"Act = %9s, %2d-%2d, Group = %3d, Char = 0x%04X",
                   Actions[lpUndoBeg->Action],
                   lpUndoBeg->CharPosBeg,
                   lpUndoBeg->CharPosEnd,
                   lpUndoBeg->Group,
                   lpUndoBeg->Char);
        DbgMsgW (lpwszDebug);
    } // End FOR

    DbgMsg ("********** End Undo Buffer *****************************");

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End DisplayUndo
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayFnHdr
//
//  Display the function header
//***************************************************************************

void DisplayFnHdr
    (LPFHLOCALVARS lpfhLocalVars)

{
    WCHAR     wszTemp[1024];
    HGLOBAL   hGlbName;
    LPAPLCHAR lpMemName;
    UINT      uLen, uItm;

    // Initialize the output string
    lstrcpyW (wszTemp, L"Saving function header:  " WS_UTF16_DEL L" ");

    // Check for result
    if (lpfhLocalVars->lpYYResult)
    {
        // Get the strand length
        uLen = lpfhLocalVars->lpYYResult->uStrandLen;

        // Append a separator
        if (uLen > 1)
            lstrcatW (wszTemp, L"(");

        for (uItm = 0; uItm < uLen; uItm++)
        {
            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYResult[uItm].tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // If we're not at the last item, separate with a space
            if (uItm < (uLen - 1))
                lstrcatW (wszTemp, L" ");

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;
        } // End FOR

        // Append a separator
        if (uLen > 1)
            lstrcatW (wszTemp, L")");

        // Append a left arrow
        lstrcatW (wszTemp, WS_UTF16_LEFTARROW);
    } // End IF

    // Check for left argument
    if (lpfhLocalVars->lpYYLftArg)
    {
        // Get the strand length
        uLen = lpfhLocalVars->lpYYLftArg->uStrandLen;

        // Append a separator
        if (lpfhLocalVars->FcnValence EQ FCNVALENCE_AMB)
            lstrcatW (wszTemp, L"[");
        else
        if (uLen > 1)
            lstrcatW (wszTemp, L"(");

        for (uItm = 0; uItm < uLen; uItm++)
        {
            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYLftArg[uItm].tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // If we're not at the last item, separate with a space
            if (uItm < (uLen - 1))
                lstrcatW (wszTemp, L" ");

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;
        } // End FOR

        // Append a separator
        if (lpfhLocalVars->FcnValence EQ FCNVALENCE_AMB)
            lstrcatW (wszTemp, L"]");
        else
        if (uLen > 1)
            lstrcatW (wszTemp, L")");

        // Append a separator
        lstrcatW (wszTemp, L" ");
    } // End IF

    // Split cases based upon the user-defined function/operator type
    switch (lpfhLocalVars->DfnType)
    {
        case DFNTYPE_OP1:
        case DFNTYPE_OP2:
            // Append a separator
            lstrcatW (wszTemp, L"(");

            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYLftOpr->tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;

            // Append a separator
            lstrcatW (wszTemp, L" ");

            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYFcnName->tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;

            // Check for right operand
            if (lpfhLocalVars->lpYYRhtOpr)
            {
                // Append a separator
                lstrcatW (wszTemp, L" ");

                // Get the Name's global memory handle
                hGlbName = lpfhLocalVars->lpYYRhtOpr->tkToken.tkData.tkSym->stHshEntry->htGlbName;

                // Lock the memory to get a ptr to it
                lpMemName = MyGlobalLock (hGlbName);

                // Copy the name
                lstrcatW (wszTemp, lpMemName);

                // We no longer need this ptr
                MyGlobalUnlock (hGlbName); lpMemName = NULL;
            } // End IF

            // Append a separator
            lstrcatW (wszTemp, L")");

            break;

        case DFNTYPE_FCN:
            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYFcnName->tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;

            break;

        case DFNTYPE_UNK:
            lstrcatW (wszTemp, L"<empty>");

            break;

        defstop
            break;
    } // End SWITCH

    // Check for right argument
    if (lpfhLocalVars->lpYYRhtArg)
    {
        // Append a separator
        lstrcatW (wszTemp, L" ");

        // Get the strand length
        uLen = lpfhLocalVars->lpYYRhtArg->uStrandLen;

        // Append a separator
        if (uLen > 1)
            lstrcatW (wszTemp, L"(");

        for (uItm = 0; uItm < uLen; uItm++)
        {
            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYRhtArg[uItm].tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // If we're not at the last item, separate with a space
            if (uItm < (uLen - 1))
                lstrcatW (wszTemp, L" ");

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;
        } // End FOR

        // Append a separator
        if (uLen > 1)
            lstrcatW (wszTemp, L")");
    } // End IF

    // Display it in the debug window
    DbgMsgW (wszTemp);
} // End DisplayFnHdr
#endif


//***************************************************************************
//  End of File: dispdbg.c
//***************************************************************************
