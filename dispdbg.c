//***************************************************************************
//  NARS2000 -- Display Debug Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2009 Sudley Place Software

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#define STRICT
#include <windows.h>
#include "headers.h"


#ifdef DEBUG
// Resource debugging variables
extern HANDLE ahGLBALLOC[MAXOBJ];
extern UINT auLinNumGLBALLOC[MAXOBJ];
extern LPCHAR lpaFileNameGLBALLOC[MAXOBJ];

// Debug level for display of function-related info
UINT gFcnLvl = 3;
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayHshTab
//
//  Display the Hash Table entries
//***************************************************************************

void DisplayHshTab
    (LPHSHTABSTR lpHTS)             // Ptr to hshTab struc

{
    LPHSHENTRY lpHshEntry;          // Ptr to current HshTab entry
    int        i;                   // Loop counter
    WCHAR      wszTemp[1024];       // Ptr to temporary output area

    DbgMsgW (L"********** Hash Table **********************************");

    wsprintfW (wszTemp,
               L"lpHshTab = %p, SplitNext = %p, Last = %p",
               lpHTS->lpHshTab,
               lpHTS->lpHshTabSplitNext,
              &lpHTS->lpHshTab[lpHTS->iHshTabTotalNelm]);
    DbgMsgW (wszTemp);

    // Display the hash table
    for (lpHshEntry = lpHTS->lpHshTab, i = 0;
         i < lpHTS->iHshTabTotalNelm;
         lpHshEntry++, i++)
    {
        // Format the HTE
        FormatHTE (lpHshEntry, wszTemp, i);

        DbgMsgW (wszTemp);
    } // End FOR

    DbgMsgW (L"********** End Hash Table ******************************");

    UpdateDBWindow ();
} // End DisplayHshTab
#endif


//***************************************************************************
//  $FormatHTE
//
//  Format a HTE
//***************************************************************************

void FormatHTE
    (LPHSHENTRY lpHshEntry,
     LPWCHAR    wszTemp,
     UINT       i)

{
    WCHAR wszFlags[128] = {L'\0'};
    UINT  htFlags,
          j;

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
#define HT_FLAGNAMES_NROWS  countof (ahtFlagNames)

    // Check for invalid HshEntry
    if (lpHshEntry EQ NULL)
    {
        wsprintfW (wszTemp,
                   L"HT:%3d ***INVALID HSHENTRY (NULL)***",
                   i);
        return;
    } // End IF

    // Format the flags
    htFlags = *(LPUINT) &lpHshEntry->htFlags;
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
            wsprintfW (wszTemp,
                       L"HT:%3d uH=%08X, uH&M=%d, <%s>, ull=%I64X, Sym=%p",
                       i,
                       lpHshEntry->uHash,
                       lpHshEntry->uHashAndMask,
                      &wszFlags[1],
                       lpSymEntry->stData.stInteger,
                       lpSymEntry);
        else
        if (lpSymEntry->stFlags.ObjName NE OBJNAME_NONE)
        {
            LPWCHAR lpwGlbName;

            // Lock the memory to get a ptr to it
            lpwGlbName = GlobalLock (lpHshEntry->htGlbName); Assert (lpwGlbName NE NULL);

            wsprintfW (wszTemp,
                       L"HT:%3d uH=%08X, uH&M=%d, <%s>, <%s>, Sym=%p, %p-%p",
                       i,
                       lpHshEntry->uHash,
                       lpHshEntry->uHashAndMask,
                      &wszFlags[1],
                       lpwGlbName,
                       lpSymEntry,
                       lpHshEntry->NextSameHash,
                       lpHshEntry->PrevSameHash);
            // We no longer need this ptr
            GlobalUnlock (lpHshEntry->htGlbName); lpwGlbName = NULL;
        } // End IF/ELSE/IF
    } else
        wsprintfW (wszTemp,
                   L"HT:%3d (EMPTY) <%s>, Sym=%p, <%p-%p>",
                   i,
                  &wszFlags[1],
                   lpHshEntry->htSymEntry,
                   lpHshEntry->NextSameHash,
                   lpHshEntry->PrevSameHash);
} // End FormatHTE


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
    (LPHSHTABSTR lpHTS,             // Ptr to HshTab struc
     UBOOL       bDispAll)          // TRUE iff we're to display the entire SymTab

{
    LPSYMENTRY   lpSymEntry;        // Ptr to current SYMENTRY
    int          i;                 // Loop counter
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    WCHAR        wszTemp[1024];     // Ptr to temporary output area

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    if (bDispAll)
    {
        DbgMsgW (L"********** Symbol Table ********************************");
    } else
    {
        DbgMsgW (L"********** Symbol Table Referenced Non-SysNames ********");
    } // End IF/ELSE -- DO NOT REMOVE as the DbgMsgW macro needs
    //                  this because of the trailing semicolon

    wsprintfW (wszTemp,
               L"lpSymTab = %p, Last = %p",
               lpHTS->lpSymTab,
              &lpHTS->lpSymTab[lpHTS->iSymTabTotalNelm]);
    DbgMsgW (wszTemp);

    // Display the symbol table
    for (lpSymEntry = lpHTS->lpSymTab, i = 0;
         lpSymEntry NE lpHTS->lpSymTabNext;
         lpSymEntry++, i++)
    if (bDispAll ||
        lpSymEntry->stFlags.ObjName NE OBJNAME_SYS)
    {
        // Format the STE
        FormatSTE (lpSymEntry, wszTemp);

        DbgMsgW (wszTemp);
    } // End FOR

    DbgMsgW (L"********** End Symbol Table ****************************");

    UpdateDBWindow ();
} // End DisplaySymTab
#endif


//***************************************************************************
//  $FormatSTE
//
//  Format a STE
//***************************************************************************

void FormatSTE
    (LPSYMENTRY lpSymEntry,             // Ptr to the SYMENTRY to format
     LPWCHAR    wszTemp)                // Ptr to output save area

{
    WCHAR   wszFlags[128] = {L'\0'};
    STFLAGS stFlags;
    LPWCHAR lpwGlbName;
    int     j;                  // Loop counter

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
     {0x00000020,  L" Perm"       },
     {0x00000040,  L" Inuse"      },
     {0x00000080,  L" Value"      },
/////{0x00000700,  L" ObjName"    },
//// {0x00007800,  L" UsrType"    },
//// {0x000F8000,  L" SysVarValid"},
     {0x00100000,  L" UsrDfn"     },
     {0x00200000,  L" DfnLabel"   },
     {0x00400000,  L" DfnSysLabel"},
     {0x00800000,  L" DfnAxis"    },
     {0x01000000,  L" FcnDir"     },
    };

// The # rows in the above table
#define ST_FLAGNAMES_NROWS  countof (astFlagNames)

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
    if (stFlags.stNameType NE NAMETYPE_UNK)
        wsprintfW (&wszFlags[lstrlenW (wszFlags)],
                   L" stNameType=%s",
                   lpwNameTypeStr[stFlags.stNameType]);

    for (j = 0;
         j < ST_FLAGNAMES_NROWS;
         j++)
    if ((*(UINT *) &stFlags) & astFlagNames[j].uMask)
        lstrcatW (wszFlags, astFlagNames[j].lpwszName);

    if (IsNameTypeVar (stFlags.stNameType)
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
        LPSYMENTRY lpPrvEntry;

        if (lpSymEntry->stFlags.ObjName NE OBJNAME_NONE)
        {
            LPHSHENTRY lpHshEntry;

            lpHshEntry = lpSymEntry->stHshEntry;

            if (lpHshEntry)
            {
                lpwGlbName = GlobalLock (lpHshEntry->htGlbName); Assert (lpwGlbName NE NULL);

                lstrcpynW (wszName, lpwGlbName, WSZNAME_LEN);

                // We no longer need this ptr
                GlobalUnlock (lpHshEntry->htGlbName); lpwGlbName = NULL;
            } // End IF
        } // End IF

        // Get the previous (shadowed) entry (if any)
        lpPrvEntry = lpSymEntry->stPrvEntry;

        if (lpSymEntry->stFlags.Imm)
        {
            wsprintfW (wszTemp,
                       L"ST:%p <%s> <%s>, ull=%I64X, Hsh=%p, Prv=%p",
                       lpSymEntry,
                       wszName,
                      &wszFlags[1],
                       lpSymEntry->stData.stInteger,
                       lpSymEntry->stHshEntry,
                       lpPrvEntry);
        } else
        if (lpSymEntry->stFlags.ObjName NE OBJNAME_NONE)
        {
            LPHSHENTRY lpHshEntry;

            lpHshEntry = lpSymEntry->stHshEntry;

            if (lpHshEntry)
            {
                wsprintfW (wszTemp,
                           L"ST:%p <%s>, <%s>, Data=%p, Hsh=%p, Prv=%p",
                           lpSymEntry,
                           wszName,
                          &wszFlags[1],
                           lpSymEntry->stData.stVoid,
                           lpHshEntry,
                           lpPrvEntry);
            } else
                wsprintfW (wszTemp,
                           L"ST:%p <******>, <%s>, Hsh=0, Prv=%p",
                           lpSymEntry,
                          &wszFlags[1],
                           lpPrvEntry);
        } // End IF/ELSE/IF
    } else
        wsprintfW (wszTemp,
                   L"ST:%p (EMPTY) <%s>, Hsh=%p, Prv=%p",
                   lpSymEntry,
                  &wszFlags[1],
                   lpSymEntry->stHshEntry,
                   lpSymEntry->stPrvEntry);
} // End FormatSTE


#ifdef DEBUG
//***************************************************************************
//  $UpdateDBWindow
//
//  Call UpdateWindow on the DB window
//***************************************************************************

void UpdateDBWindow
    (void)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    UpdateWindow (lpMemPTD->hWndDB);
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
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    WCHAR        wszTemp[1024];     // Ptr to temporary output area

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    DbgMsgW (L"********** Globals *************************************");

#define MAX_VAL_LEN     12
    for (i = 0; i < MAXOBJ; i++)
    if (hGlb = ahGLBALLOC[i])
    {
        // Note we don't use MyGlobalLock here as that function might well
        //   fail and trigger a hard error which we prefer to catch softly.

        // Lock the memory to get a ptr to it
        lpMem = GlobalLock (hGlb);
        if (!lpMem)
        {
            wsprintfW (wszTemp,
                       L"hGlb=%p *** INVALID ***",
                       hGlb);
            DbgMsgW (wszTemp);

            continue;
        } // End IF

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
        if (lpHeader->Sig.nature EQ VARARRAY_HEADER_SIGNATURE)
        {
            // If we're to display all globals or
            //   this one is not a sysvar, ...
            if (uDispGlb EQ 2
             || !lpHeader->SysVar)
            {
                // It's a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (MakePtrTypeGlb (hGlb)));

                if (IsScalar (lpHeader->Rank))
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
                        if (IsEmpty (lpHeader->NELM))
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
                        lstrcpynW (aplArrChar, lpData, 1 + (APLU3264) min (MAX_VAL_LEN, lpHeader->NELM));
                        aplArrChar[min (MAX_VAL_LEN, lpHeader->NELM)] = L'\0';

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
                 || lpHeader->PermNdx EQ PERMNDX_NONE)
                {
                    wsprintfW (wszTemp,
                               L"hGlb=%p AType=%c%c NELM=%3d RC=%1d%cRnk=%2d Dim1=%3d Lck=%d (%S#%d) (%s)",
                               hGlb,
                               ArrayTypeAsChar[lpHeader->ArrType],
                               L" *"[lpHeader->PermNdx NE PERMNDX_NONE],
                               LODWORD (lpHeader->NELM),
                               lpHeader->RefCnt,
                               L" *"[lpHeader->RefCnt > 1],
                               LODWORD (lpHeader->Rank),
                               LODWORD (aplDim),
                               (MyGlobalFlags (hGlb) & GMEM_LOCKCOUNT) - 1,
                               lpaFileNameGLBALLOC[i],
                               auLinNumGLBALLOC[i],
                               aplArrChar);
                    DbgMsgW (wszTemp);
                } // End IF
            } // End IF
        } else
#undef  lpHeader
#define lpHeader    ((LPFCNARRAY_HEADER) lpMem)
        if (lpHeader->Sig.nature EQ FCNARRAY_HEADER_SIGNATURE)
        {
            // It's a valid HGLOBAL function array
            Assert (IsGlbTypeFcnDir (MakePtrTypeGlb (hGlb)));

            wsprintfW (wszTemp,
                       L"hGlb=%p NType=%sNELM=%3d RC=%1d%c                Lck=%d (%S#%4d)",
                       hGlb,
                       lpwNameTypeStr[lpHeader->fnNameType],
                       lpHeader->tknNELM,
                       lpHeader->RefCnt,
                       L" *"[lpHeader->RefCnt > 1],
                       (MyGlobalFlags (hGlb) & GMEM_LOCKCOUNT) - 1,
                       lpaFileNameGLBALLOC[i],
                       auLinNumGLBALLOC[i]);
            DbgMsgW (wszTemp);
        } else
#undef  lpHeader
#define lpHeader    ((LPDFN_HEADER) lpMem)
        if (lpHeader->Sig.nature EQ DFN_HEADER_SIGNATURE)
        {
            LPDFN_HEADER lpMemDfnHdr;           // Ptr to user-defined function/operator header
            APLUINT      uNameLen;              // User-defined function/operator name length

            // It's a valid HGLOBAL user-defined function/operator
            Assert (IsGlbTypeDfnDir (MakePtrTypeGlb (hGlb)));

            // Lock to get a ptr to it
            lpMemDfnHdr = MyGlobalLock (hGlb);

            // Copy the user-defined function/operator name
            CopySteName (lpMemPTD->lpwszTemp,       // Ptr to global memory
                         lpMemDfnHdr->steFcnName,   // Ptr to function symbol table entry
                        &uNameLen);                 // Ptr to name length (may be NULL)
            // We no longer need this ptr
            MyGlobalUnlock (hGlb); lpMemDfnHdr = NULL;

            // If we're to display all globals or
            //   this one is not a Magic Function, ...
            if (uDispGlb EQ 2
             || lpMemPTD->lpwszTemp[0] NE L'#')
            {
                // Copy the name to local storage
                lstrcpynW (aplArrChar, lpMemPTD->lpwszTemp, 1 + (APLU3264) min (MAX_VAL_LEN, uNameLen));
                aplArrChar[min (MAX_VAL_LEN, uNameLen)] = L'\0';

                wsprintfW (wszTemp,
                           L"hGlb=%p DType=%c  NELM=%3d RC=%1d%c                Lck=%d (%S#%4d) (%s)",
                           hGlb,
                           cDfnTypeStr[lpHeader->DfnType],
                           lpHeader->numFcnLines,
                           lpHeader->RefCnt,
                           L" *"[lpHeader->RefCnt > 1],
                           (MyGlobalFlags (hGlb) & GMEM_LOCKCOUNT) - 1,
                           lpaFileNameGLBALLOC[i],
                           auLinNumGLBALLOC[i],
                           aplArrChar);
                DbgMsgW (wszTemp);
            } // End IF
        } else
#undef  lpHeader
        if (uDispGlb EQ 2)
        {
            wsprintfW (wszTemp,
                       L"hGlb=%p -- No NARS/FCNS Signature",
                       hGlb);
            DbgMsgW (wszTemp);
        } // End IF/ELSE

        // We no longer need this ptr
        GlobalUnlock (hGlb); lpMem = NULL;
    } // End FOR/IF
#undef  MAX_VAL_LEN

    DbgMsgW (L"********** End Globals *********************************");

    UpdateDBWindow ();
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
    LPTOKEN      lpToken;           // Ptr to temporary token
    int          i,                 // Loop counter
                 iLen;              // Token count
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    WCHAR        wszTemp[1024];     // Ptr to temporary output area

    // Check debug level
    if (gDbgLvl < 3)
        return;

    DbgMsgW (L"********** Tokens **************************************");

    // Ensure it's valid
    if (!hGlbToken)
    {
        DbgMsgW (L"DisplayTokens:  ***INAVLID HANDLE***:  hGlbToken EQ 0");
        return;
    } // End IF

    // Lock the memory to get a ptr to it
    lpToken = MyGlobalLock (hGlbToken);

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

#define lpHeader    ((LPTOKEN_HEADER) lpToken)
    wsprintfW (wszTemp,
               L"lpToken = %p, Version # = %d, TokenCnt = %d, PrevGroup = %d",
               lpToken,
               lpHeader->Version,
               lpHeader->TokenCnt,
               lpHeader->PrevGroup);
    DbgMsgW (wszTemp);

    iLen = lpHeader->TokenCnt;
#undef  lpHeader

    lpToken = TokenBaseToStart (lpToken);   // Skip over TOKEN_HEADER

    for (i = 0; i < iLen; i++, lpToken++)
    {
        wsprintfW (wszTemp,
                   L"(%2d) Data=%I64X, CharIndex=%2d, Type=%S",
                   i,
                   *(LPAPLINT) &lpToken->tkData.tkFloat,
                   lpToken->tkCharIndex,
                   GetTokenTypeName (lpToken->tkFlags.TknType));
        DbgMsgW (wszTemp);
    } // End FOR

    DbgMsgW (L"********** End Tokens **********************************");

    // We no longer need this ptr
    MyGlobalUnlock (hGlbToken); lpToken = NULL;
} // End DisplayTokens
#endif


#ifdef DEBUG
//***************************************************************************
//  $GetTokenTypeName
//
//  Convert a token type value to a name
//***************************************************************************

LPCHAR GetTokenTypeName
    (TOKEN_TYPES uType)

{
typedef struct tagTOKENNAMES
{
    LPCHAR lpsz;
    UINT   uTokenNum;
} TOKENNAMES, *LPTOKENNAMES;

static TOKENNAMES tokenNames[] =
{{"VARNAMED"    , TKT_VARNAMED      },  // 01: Symbol table entry for a named var (data is LPSYMENTRY)
 {"CHRSTRAND"   , TKT_CHRSTRAND     },  // 02: Character strand  (data is HGLOBAL)
 {"NUMSTRAND"   , TKT_NUMSTRAND     },  // 03: Numeric   ...     (data is HGLOBAL)
 {"VARIMMED"    , TKT_VARIMMED      },  // 04: Immediate data (data is immediate)
 {"ASSIGN"      , TKT_ASSIGN        },  // 05: Assignment symbol (data is UTF16_LEFTARROW)
 {"LISTSEP"     , TKT_LISTSEP       },  // 06: List separator    (data is ';')
 {"LABELSEP"    , TKT_LABELSEP      },  // 07: Label ...         (data is ':')
 {"COLON"       , TKT_COLON         },  // 08: Colon             (data is ':')
 {"FCNIMMED"    , TKT_FCNIMMED      },  // 09: Primitive function (any valence) (data is UTF16_***)
 {"OP1IMMED"    , TKT_OP1IMMED      },  // 0A: Monadic primitive operator (data is UTF16_***)
 {"OP2IMMED"    , TKT_OP2IMMED      },  // 0B: Dyadic  ...
 {"OP3IMMED"    , TKT_OP3IMMED      },  // 0C: Ambiguous ...
 {"OPJOTDOT"    , TKT_OPJOTDOT      },  // 0D: Outer product monadic operator (with right scope) (data is NULL)
 {"LEFTPAREN"   , TKT_LEFTPAREN     },  // 0E: Left paren (data is TKT_LEFTPAREN)
 {"RIGHTPAREN"  , TKT_RIGHTPAREN    },  // 0F: Right ...   ...         RIGHTPAREN
 {"LEFTBRACKET" , TKT_LEFTBRACKET   },  // 10: Left bracket ...        LEFTBRACKET
 {"RIGHTBRACKET", TKT_RIGHTBRACKET  },  // 11: Right ...   ...         RIGHTBRACKET
 {"LEFTBRACE"   , TKT_LEFTBRACE     },  // 12: Left bracket ...        LEFTBRACKET
 {"RIGHTBRACE"  , TKT_RIGHTBRACE    },  // 13: Right ...   ...         RIGHTBRACKET
 {"EOS"         , TKT_EOS           },  // 14: End-of-Stmt (data is length of stmt including this token)
 {"EOL"         , TKT_EOL           },  // 15: End-of-Line  ...
 {"SOS"         , TKT_SOS           },  // 16: Start-of-Stmt (data is NULL)
 {"LINECONT"    , TKT_LINECONT      },  // 17: Line continuation (data is NULL)
 {"INPOUT"      , TKT_INPOUT        },  // 18: Input/Output (data is UTF16_QUAD or UTF16_QUOTEQUAD)
 {"VARARRAY"    , TKT_VARARRAY      },  // 19: Array of data (data is HGLOBAL)
 {"CS_ANDIF"    , TKT_CS_ANDIF      },  // 1A: Control Structure:  ANDIF     (Data is Line/Stmt #)
 {"CS_CASE"     , TKT_CS_CASE       },  // 1B: ...                 CASE       ...
 {"CS_CASELIST" , TKT_CS_CASELIST   },  // 1C: ...                 CASELIST   ...
 {"CS_CONTINUE" , TKT_CS_CONTINUE   },  // 1D: ...                 CONTINUE   ...
 {"CS_ELSE"     , TKT_CS_ELSE       },  // 1E: ...                 ELSE       ...
 {"CS_ELSEIF"   , TKT_CS_ELSEIF     },  // 1F: ...                 ELSEIF     ...
 {"CS_END"      , TKT_CS_END        },  // 20: ...                 END        ...
 {"CS_ENDFOR"   , TKT_CS_ENDFOR     },  // 21: ...                 ENDFOR     ...
 {"CS_ENDFORLCL", TKT_CS_ENDFORLCL  },  // 22: ...                 ENDFORLCL  ...
 {"CS_ENDIF"    , TKT_CS_ENDIF      },  // 23: ...                 ENDIF      ...
 {"CS_ENDREPEAT", TKT_CS_ENDREPEAT  },  // 24: ...                 ENDREPEAT  ...
 {"CS_ENDSELECT", TKT_CS_ENDSELECT  },  // 25: ...                 ENDSELECT  ...
 {"CS_ENDWHILE" , TKT_CS_ENDWHILE   },  // 26: ...                 ENDWHILE   ...
 {"CS_FOR"      , TKT_CS_FOR        },  // 27: ...                 FOR        ...
 {"CS_FOR2"     , TKT_CS_FOR2       },  // 28: ...                 FOR2       ...
 {"CS_FORLCL"   , TKT_CS_FORLCL     },  // 29: ...                 FORLCL     ...
 {"CS_GOTO"     , TKT_CS_GOTO       },  // 2A: ...                 GOTO       ...
 {"CS_IF"       , TKT_CS_IF         },  // 2B: ...                 IF         ...
 {"CS_IF2"      , TKT_CS_IF2        },  // 2C: ...                 IF2        ...
 {"CS_IN"       , TKT_CS_IN         },  // 2D: ...                 IN         ...
 {"CS_LEAVE"    , TKT_CS_LEAVE      },  // 2E: ...                 LEAVE      ...
 {"CS_ORIF"     , TKT_CS_ORIF       },  // 2F: ...                 ORIF       ...
 {"CS_REPEAT"   , TKT_CS_REPEAT     },  // 30: ...                 REPEAT     ...
 {"CS_REPEAT2"  , TKT_CS_REPEAT2    },  // 31: ...                 REPEAT2    ...
 {"CS_RETURN"   , TKT_CS_RETURN     },  // 32: ...                 RETURN     ...
 {"CS_SELECT"   , TKT_CS_SELECT     },  // 33: ...                 SELECT     ...
 {"CS_SELECT2"  , TKT_CS_SELECT2    },  // 34: ...                 SELECT2    ...
 {"CS_UNTIL"    , TKT_CS_UNTIL      },  // 35: ...                 UNTIL      ...
 {"CS_WHILE"    , TKT_CS_WHILE      },  // 36: ...                 WHILE      ...
 {"CS_WHILE2"   , TKT_CS_WHILE2     },  // 37: ...                 WHILE2     ...
 {"CS_SKIPCASE" , TKT_CS_SKIPCASE   },  // 38: ...                 Special token
 {"CS_SKIPEND"  , TKT_CS_SKIPEND    },  // 39: ...                 Special token
 {"SYNTERR"     , TKT_SYNTERR       },  // 3A: ...                 Special token
 {"STRAND"      , TKT_STRAND        },  // 3B: Strand accumulating (data is LPTOKEN)
 {"LISTINT"     , TKT_LISTINT       },  // 3C: List in parens    (data is HGLOBAL)
 {"LISTPAR"     , TKT_LISTPAR       },  // 3D: List in parens    (data is HGLOBAL)
 {"LSTIMMED"    , TKT_LSTIMMED      },  // 3E: List in brackets, single element, immed (data is immediate)
 {"LSTARRAY"    , TKT_LSTARRAY      },  // 3F: List in brackets, single element, array (data is HGLOBAL)
 {"LSTMULT"     , TKT_LSTMULT       },  // 40: List in brackets, multiple elements (data is HGLOBAL)
 {"FCNARRAY"    , TKT_FCNARRAY      },  // 41: Array of functions (data is HGLOBAL)
 {"FCNNAMED"    , TKT_FCNNAMED      },  // 42: Symbol table entry for a named function (data is LPSYMENTRY)
 {"AXISIMMED"   , TKT_AXISIMMED     },  // 43: An immediate axis specification (data is immediate)
 {"AXISARRAY"   , TKT_AXISARRAY     },  // 44: An array of  ...   (data is HGLOBAL)
 {"OP1NAMED"    , TKT_OP1NAMED      },  // 45: A named monadic primitive operator (data is LPSYMENTRY)
 {"OP2NAMED"    , TKT_OP2NAMED      },  // 46: ...     dyadic  ...
 {"OP3NAMED"    , TKT_OP3NAMED      },  // 47: ...     ambiguous ...
 {"STRNAMED"    , TKT_STRNAMED      },  // 48: ...     strand  ...
 {"CS_NEC"      , TKT_CS_NEC        },  // 49: Control Structure:  Special token
 {"CS_EOL"      , TKT_CS_EOL        },  // 4A: ...                 Special token
};

// The # rows in the above table
#define TOKENNAMES_NROWS    countof (tokenNames)

    if (TOKENNAMES_NROWS > (uType - TKT_FIRST))
        return tokenNames[uType - TKT_FIRST].lpsz;
    else
    {
        static char szTemp[64];

        wsprintf (szTemp,
                  "***Unknown Token Type:  %d***",
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
    (LPTOKEN lptkFunc,              // Ptr to function token
     UBOOL   bDispHdr)              // TRUE iff we're to display the header

{
    HGLOBAL      hGlbData;          // Function array global memory handle
    LPWCHAR      lpaplChar,         // Ptr to output save area
                 lpaplCharIni;      // Initial ptr to output save area
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    // Check debug level
    if (bDispHdr
#ifdef DEBUG
     && gDbgLvl < gFcnLvl
#endif
     )
        return;

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get a ptr to a temporary save area
    lpaplChar = lpaplCharIni = lpMemPTD->lpwszTemp;

    // Split cases based upon the token type
    switch (lptkFunc->tkFlags.TknType)
    {
        case TKT_VARNAMED:          // Because we don't distinguish between
                                    //   functions and variables in AssignName_EM
            DbgBrk ();          // ***FINISHME*** -- TKT_VARNAMED in DisplayFcnStrand






            break;

        case TKT_FCNIMMED:
        case TKT_OP1IMMED:
        case TKT_OP2IMMED:
        case TKT_OP3IMMED:
            if (bDispHdr)
                lpaplChar += wsprintfW (lpaplChar,
                                        L"fnNameType=%1d, NELM=%3d, RC=%2d, Fn:  ",
                                        NAMETYPE_FN12,  // lpHeader->fnNameType,
                                        1,              // LODWORD (lpHeader->NELM),
                                        0);             // lpHeader->RefCnt);
            // Translate from INDEX_xxx to UTF16_xxx
            *lpaplChar++ = TranslateFcnOprToChar (lptkFunc->tkData.tkChar);

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
                    if (bDispHdr)
                        // Start off with the header
                        lpaplChar += wsprintfW (lpaplChar,
                                                L"fnNameType=%1d, NELM=%3d, RC=%2d, Fn:  ",
                                                NAMETYPE_FN12,  // lpHeader->fnNameType,
                                                1,              // LODWORD (lpHeader->NELM),
                                                0);             // lpHeader->RefCnt);
                    // Display the function name from the symbol table
                    lpaplChar =
                      CopySteName (lpaplChar,               // Ptr to result global memory
                                   lptkFunc->tkData.tkSym,  // Ptr to function symbol table entry
                                   NULL);                   // Ptr to name length (may be NULL)
                } else
                if (!lptkFunc->tkData.tkSym->stFlags.UsrDfn)
                    lpaplChar =
                      DisplayFcnGlb (lpaplChar,         // Ptr to output save area
                                     hGlbData,          // Function array global memory handle
                                     bDispHdr,          // TRUE iff we're to display the header
                                     NULL,              // Ptr to function to convert an HGLOBAL to FMTSTR_GLBOBJ (may be NULL)
                                     NULL);             // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
            } else
            {
                if (bDispHdr)
                    // Start off with the header
                    lpaplChar += wsprintfW (lpaplChar,
                                            L"fnNameType=%1d, NELM=%3d, RC=%2d, Fn:  ",
                                            NAMETYPE_FN12,  // lpHeader->fnNameType,
                                            1,              // LODWORD (lpHeader->NELM),
                                            0);             // lpHeader->RefCnt);
                *lpaplChar++ = lptkFunc->tkData.tkSym->stData.stChar;
            } // End IF/ELSE

            break;

        case TKT_FCNARRAY:
            hGlbData = lptkFunc->tkData.tkGlbData;

            // tkData is an HGLOBAL function array
            Assert (IsGlbTypeFcnDir (hGlbData));

            lpaplChar =
              DisplayFcnGlb (lpaplChar,                 // Ptr to output save area
                             hGlbData,                  // Function array global memory handle
                             bDispHdr,                  // TRUE iff we're to display the header
                             NULL,                      // Ptr to function to convert an HGLOBAL to FMTSTR_GLBOBJ (may be NULL)
                             NULL);                     // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
            break;

        defstop
            break;
    } // End SWITCH

    // Ensure properly terminated
    *lpaplChar = L'\0';

    if (bDispHdr)
        // Display the line in the debugging window
        DbgMsgW (lpaplCharIni);
    else
        // Display the line in the Session Manager window
        AppendLine (lpaplCharIni, FALSE, TRUE);
} // End DisplayFcnStrand
#endif


//***************************************************************************
//  $DisplayFcnGlb
//
//  Display a function from an HGLOBAL
//***************************************************************************

LPWCHAR DisplayFcnGlb
    (LPWCHAR             lpaplChar,             // Ptr to output save area
     HGLOBAL             hGlbFcnArr,            // Function array global memory handle
     UBOOL               bDispHeader,           // TRUE iff we're to display the header
     LPSAVEDWSGLBVARCONV lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL to FMTSTR_GLBOBJ (may be NULL)
     LPSAVEDWSGLBVARPARM lpSavedWsGlbVarParm)   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)

{
    LPFCNARRAY_HEADER lpHeader;                 // Ptr to function array header
    LPPL_YYSTYPE      lpMemFcnArr;              // ...                   global memory
    UINT              tknNELM;                  // # tokens in the function array
    NAME_TYPES        fnNameType;               // Function array name type

    // Clear the ptr type bits
    hGlbFcnArr = ClrPtrTypeDirAsGlb (hGlbFcnArr);

    // Lock the memory to get a ptr to it
    lpHeader = MyGlobalLock (hGlbFcnArr);

    tknNELM    = lpHeader->tknNELM;
    fnNameType = lpHeader->fnNameType;

    if (bDispHeader)
        lpaplChar += wsprintfW (lpaplChar,
                                L"fnNameType=%s, NELM=%3d, RC=%2d%c, Fn:  ",
                                lpwNameTypeStr[lpHeader->fnNameType],
                                tknNELM,
                                lpHeader->RefCnt,
                                L" *"[lpHeader->RefCnt > 1]);
    // Skip over the function array header
    lpMemFcnArr = FcnArrayBaseToData (lpHeader);

    // Check for Trains
    if (fnNameType EQ NAMETYPE_TRN)
    {
        // Skip to the next entry
        lpMemFcnArr = &lpMemFcnArr[tknNELM];

        // Start with surrounding parens
        *lpaplChar++ = L'(';

        // Loop through the function array entries
        while (tknNELM--)
        {
            lpaplChar =
              DisplayFcnSub (lpaplChar,             // Ptr to output save area
                           --lpMemFcnArr,           // Ptr to function array data
                             1,                     // Token NELM
                             lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL to FMTSTR_GLBOBJ (may be NULL)
                             lpSavedWsGlbVarParm);  // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
            // Append visual separator
            *lpaplChar++ = L' ';
        } // End WHILE

        // Ending paren
        lpaplChar[-1] = L')';

        // Ensure properly terminated
        *lpaplChar = L'\0';
    } else
        lpaplChar =
          DisplayFcnSub (lpaplChar,             // Ptr to output save area
                         lpMemFcnArr,           // Ptr to function array data
                         tknNELM,               // Token NELM
                         lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL to FMTSTR_GLBOBJ (may be NULL)
                         lpSavedWsGlbVarParm);  // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
    // We no longer need this ptr
    MyGlobalUnlock (hGlbFcnArr); lpMemFcnArr = NULL;

    return lpaplChar;
} // End DisplayFcnGlb


//***************************************************************************
//  $DisplayFcnSub
//
//  Display function subroutine
//***************************************************************************

LPWCHAR DisplayFcnSub
    (LPWCHAR             lpaplChar,             // Ptr to output save area
     LPPL_YYSTYPE        lpYYMem,               // Ptr to function array data
     UINT                tknNELM,               // Token NELM
     LPSAVEDWSGLBVARCONV lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL to FMTSTR_GLBOBJ (may be NULL)
     LPSAVEDWSGLBVARPARM lpSavedWsGlbVarParm)   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)

{
    HGLOBAL hGlbData;           // Function array global memory handle
    LPVOID  lpMemData;          // Ptr to function array global memory
    UINT    TknCount;           // Token count
    UBOOL   bIsImmed;           // TRUE if the named var is an immediate

    // Split cases based upon the token type
    switch (lpYYMem[0].tkToken.tkFlags.TknType)
    {
        case TKT_OP1IMMED:
        case TKT_OP3IMMED:
            // Check for axis operator
            if (tknNELM > 1
             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
            {
                // If there's a function, ...
                if (tknNELM > 2)
                    lpaplChar =
                      DisplayFcnSub (lpaplChar,                                         // Fcn
                                    &lpYYMem[2],
                                     tknNELM - 2,
                                     lpSavedWsGlbVarConv,
                                     lpSavedWsGlbVarParm);
                // Translate from INDEX_xxx to UTF16_xxx
                *lpaplChar++ = TranslateFcnOprToChar (lpYYMem[0].tkToken.tkData.tkChar);// Op1
                lpaplChar =
                  DisplayFcnSub (lpaplChar,                                             // [X]
                                &lpYYMem[1],
                                 1,
                                 lpSavedWsGlbVarConv,
                                 lpSavedWsGlbVarParm);
            } else
            {
                if (tknNELM > 1)
                    lpaplChar =
                      DisplayFcnSub (lpaplChar,                                         // Fcn
                                    &lpYYMem[1],
                                     tknNELM - 1,
                                     lpSavedWsGlbVarConv,
                                     lpSavedWsGlbVarParm);
                // Translate from INDEX_xxx to UTF16_xxx
                *lpaplChar++ = TranslateFcnOprToChar (lpYYMem[0].tkToken.tkData.tkChar);// Op1
            } // End IF/ELSE/...

            break;

        case TKT_OP2IMMED:
            TknCount = 1 + lpYYMem[1].TknCount;
            lpaplChar =
              DisplayFcnSub (lpaplChar,                                                 // Lfcn
                            &lpYYMem[1],
                             lpYYMem[1].TknCount,
                             lpSavedWsGlbVarConv,
                             lpSavedWsGlbVarParm);
            // Translate from INDEX_xxx to UTF16_xxx
            *lpaplChar++ = TranslateFcnOprToChar (lpYYMem[0].tkToken.tkData.tkChar);    // Op2
            if (lpYYMem[TknCount].TknCount > 1)
                *lpaplChar++ = L'(';
            lpaplChar =
              DisplayFcnSub (lpaplChar,                                                 // Rfcn
                            &lpYYMem[TknCount],
                             lpYYMem[TknCount].TknCount,
                             lpSavedWsGlbVarConv,
                             lpSavedWsGlbVarParm);
            if (lpYYMem[TknCount].TknCount > 1)
                *lpaplChar++ = L')';
            break;

        case TKT_FCNIMMED:
            // Translate from INDEX_xxx to UTF16_xxx
            *lpaplChar++ = TranslateFcnOprToChar (lpYYMem[0].tkToken.tkData.tkChar);    // Fcn

            // Check for axis operator
            if (tknNELM > 1
             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
                lpaplChar =
                  DisplayFcnSub (lpaplChar,                                             // [X]
                                &lpYYMem[1],
                                 1,
                                 lpSavedWsGlbVarConv,
                                 lpSavedWsGlbVarParm);
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
                      CopySteName (lpaplChar,                       // Ptr to result global memory
                                   lpYYMem->tkToken.tkData.tkSym,   // Ptr to function symbol table entry
                                   NULL);                           // Ptr to name length (may be NULL)
                else
                {
                    // Get the function array global memory handle
                    hGlbData = lpYYMem->tkToken.tkData.tkSym->stData.stGlbData;

                    // Split cases based upon the array signature
                    switch (GetSignatureGlb (hGlbData))
                    {
                        LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header

                        case FCNARRAY_HEADER_SIGNATURE:
                            // Display the function strand in global memory
                            lpaplChar =
                              DisplayFcnGlb (lpaplChar,                 // Ptr to output save area
                                             hGlbData,                  // Function array global memory handle
                                             FALSE,                     // TRUE iff we're to display the header
                                             lpSavedWsGlbVarConv,       // Ptr to function to convert an HGLOBAL to FMTSTR_GLBOBJ (may be NULL)
                                             lpSavedWsGlbVarParm);      // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                            break;

                        case DFN_HEADER_SIGNATURE:
                            // Clear the ptr type bits
                            hGlbData = ClrPtrTypeDirAsGlb (hGlbData);

                            // Lock the memory to get a ptr to it
                            lpMemDfnHdr = MyGlobalLock (hGlbData);

                            // Append the function name from the symbol table
                            lpaplChar = CopySteName (lpaplChar, lpMemDfnHdr->steFcnName, NULL);

                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbData); lpMemDfnHdr = NULL;

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End IF/ELSE
            } else
                // Handle the immediate case
                *lpaplChar++ = lpYYMem->tkToken.tkData.tkSym->stData.stChar;

            // Check for axis operator
            if (tknNELM > 1
             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
                lpaplChar =
                  DisplayFcnSub (lpaplChar,                                             // [X]
                                &lpYYMem[1],
                                 1,
                                 lpSavedWsGlbVarConv,
                                 lpSavedWsGlbVarParm);
            break;

        case TKT_OPJOTDOT:
            *lpaplChar++ = UTF16_JOT;
            *lpaplChar++ = L'.';

            // Surround with parens if more than one token
            if (tknNELM > 2)
                *lpaplChar++ = L'(';
            if (tknNELM > 1)
                lpaplChar =
                  DisplayFcnSub (lpaplChar,                                             // Fcn
                                &lpYYMem[1],
                                tknNELM - 1,
                                 lpSavedWsGlbVarConv,
                                 lpSavedWsGlbVarParm);
            if (tknNELM > 2)
                *lpaplChar++ = L')';
            break;

        case TKT_AXISIMMED:
            *lpaplChar++ = L'[';
            lpaplChar =
              FormatImmed (lpaplChar,           // ***FIXME*** Use FormatImmedFC ??
                           lpYYMem[0].tkToken.tkFlags.ImmType,
                          &lpYYMem[0].tkToken.tkData.tkLongest);
            lpaplChar[-1] = L']';   // Overwrite the trailing blank

            break;

        case TKT_AXISARRAY:
            *lpaplChar++ = L'[';

            // Get the axis array global memory handle
            hGlbData = lpYYMem->tkToken.tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbData));

            // Clear the ptr type bits
            hGlbData = ClrPtrTypeDirAsGlb (hGlbData);

            // If there's a callback function, use it
            if (lpSavedWsGlbVarConv)
                lpaplChar =
                  (*lpSavedWsGlbVarConv) (lpaplChar,                // Ptr to output save area
                                          hGlbData,                 // Object global memory handle
                                          lpSavedWsGlbVarParm);     // Ptr to extra parameters for lpSavedWsGlbVarConv
            else
            {
                *lpaplChar++ = UTF16_HORIZELLIPSIS;
                *lpaplChar++ = L' ';                        // N.B.:  trailing blank is significant
            } // End IF

            if (lpaplChar[-1] EQ L' ')
                lpaplChar--;            // Back over the trailing blank
            *lpaplChar++ = L']';

            break;

        case TKT_VARIMMED:
            lpaplChar =
              FormatImmed (lpaplChar,           // ***FIXME*** Use FormatImmedFC ??
                           lpYYMem[0].tkToken.tkFlags.ImmType,
                          &lpYYMem[0].tkToken.tkData.tkLongest);
            if (lpaplChar[-1] EQ L' ')
                lpaplChar--;            // Back over the trailing blank
            break;

        case TKT_VARARRAY:
            // Get the variable array global memory handle
            hGlbData = lpYYMem->tkToken.tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbData));

            // If there's a callback function, use it
            if (lpSavedWsGlbVarConv)
                lpaplChar =
                  (*lpSavedWsGlbVarConv) (lpaplChar,                // Ptr to output save area
                                          hGlbData,                 // Object global memory handle
                                          lpSavedWsGlbVarParm);     // Ptr to extra parameters for lpSavedWsGlbVarConv
            else
            {
                *lpaplChar++ = UTF16_HORIZELLIPSIS;
                *lpaplChar++ = L' ';                        // N.B.:  trailing blank is significant
            } // End IF

            if (lpaplChar[-1] EQ L' ')
                *--lpaplChar = L'\0';   // Overwrite the trailing blank
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
                    lpaplChar =
                      DisplayFcnGlb (lpaplChar,             // Ptr to output save area
                                     hGlbData,              // Function array global memory handle
                                     FALSE,                 // TRUE iff we're to display the header
                                     lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL to FMTSTR_GLBOBJ (may be NULL)
                                     lpSavedWsGlbVarParm);  // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                    break;

                case DFN_HEADER_SIGNATURE:
                    // If there's a callback function, use it
                    if (lpSavedWsGlbVarConv)
                        lpaplChar =
                          (*lpSavedWsGlbVarConv) (lpaplChar,                // Ptr to output save area
                                                  MakePtrTypeGlb (hGlbData),// Object global memory handle
                                                  lpSavedWsGlbVarParm);     // Ptr to extra parameters for lpSavedWsGlbVarConv
                    else
                        // Copy the user-defined function/operator name
                        lpaplChar =
                          CopySteName (lpaplChar,                               // Ptr to result global memory
                                       ((LPDFN_HEADER) lpMemData)->steFcnName,  // Ptr to function symbol table entry
                                       NULL);                                   // Ptr to name length (may be NULL)
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

            // Determine if the contents are an immediate
            bIsImmed = lpYYMem->tkToken.tkData.tkSym->stFlags.Imm;

            if (!bIsImmed)
            {
                // Get the variable array global memory handle
                hGlbData = lpYYMem->tkToken.tkData.tkSym->stData.stGlbData;

                // tkData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbData));
            } // End IF

            // If there's a callback function, use it
            if (!bIsImmed && lpSavedWsGlbVarConv)
                lpaplChar =
                  (*lpSavedWsGlbVarConv) (lpaplChar,                // Ptr to output save area
                                          hGlbData,                 // Object global memory handle
                                          lpSavedWsGlbVarParm);     // Ptr to extra parameters for lpSavedWsGlbVarConv
            else
                // Copy the STE name
                lpaplChar =
                  CopySteName (lpaplChar,                           // Ptr to result global memory
                               lpYYMem->tkToken.tkData.tkSym,       // Ptr to function symbol table entry
                               NULL);                               // Ptr to name length (may be NULL)
            if (lpaplChar[-1] EQ L' ')
                *--lpaplChar = L'\0';   // Overwrite the trailing blank
            break;

        case TKT_OP1NAMED:          // At the moment, named operators are all one char
        case TKT_OP2NAMED:          //   symbols (no assignment of {jot}{dot}), so they
        case TKT_OP3NAMED:          //   are all immediates and are handled as such above.
        defstop
            break;
    } // End SWITCH

    // Ensure properly terminated
    *lpaplChar = L'\0';

    return lpaplChar;
} // End DisplayFcnSub


#ifdef DEBUG
//***************************************************************************
//  $DisplayFcnArr
//
//  Display a Function Array
//***************************************************************************

void DisplayFcnArr
    (HGLOBAL hGlbStr)               // Function array global memory handle

{
    WCHAR wszTemp[1024];            // Ptr to temporary output area

    Assert (IsGlbTypeFcnDir (MakePtrTypeGlb (hGlbStr)));

    // Check debug level
    if (gDbgLvl < gFcnLvl)
        return;

    DbgMsgW (L"********** Function Array ******************************");

    DisplayFcnGlb (wszTemp,         // Ptr to output save area
                   hGlbStr,         // Function array global memory handle
                   TRUE,            // TRUE iff we're to display the header
                   NULL,            // Ptr to function to convert an HGLOBAL to FMTSTR_GLBOBJ (may be NULL)
                   NULL);           // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
    DbgMsgW (wszTemp);

    DbgMsgW (L"********** End Function Array **************************");
} // End DisplayFcnArr
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayFcnLine
//
//  Display a given function line
//***************************************************************************

void DisplayFcnLine
    (HGLOBAL      hGlbTxtLine,      // Function line text global memory handle (may be NULL if uLineNum EQ NEG1U)
     LPPERTABDATA lpMemPTD,         // Ptr to PerTabData global memory
     UINT         uLineNum)         // Function line # (NEG1U for terminating)

{
    LPMEMTXT_UNION lpMemTxtLine;
    LPAPLCHAR      lpMemFcnName;

    if (gDbgLvl < 2)
        return;

    // If the handle is valid, ...
    if (hGlbTxtLine)
        // Lock the memory to get a ptr to it
        lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

    // Lock the memory to get a ptr to it
    lpMemFcnName = MyGlobalLock (lpMemPTD->lpSISCur->hGlbFcnName);

    if (uLineNum EQ NEG1U)
        wsprintfW (lpMemPTD->lpwszTemp,
                   L"Terminating <%s>",
                   lpMemFcnName);
    else
        wsprintfW (lpMemPTD->lpwszTemp,
                   L"Executing line %d of <%s>:  %s",
                   uLineNum,
                   lpMemFcnName,
                  &lpMemTxtLine->C);
    DbgMsgW (lpMemPTD->lpwszTemp);

    // We no longer need this ptr
    MyGlobalUnlock (lpMemPTD->lpSISCur->hGlbFcnName); lpMemFcnName = NULL;

    // If the handle is valid, ...
    if (hGlbTxtLine)
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
} // End DisplayFcnline
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
    LPPERTABDATA  lpMemPTD;         // Ptr to PerTabData global memory
    WCHAR         wszTemp[1024];    // Ptr to temporary output area

    // Split cases based upon the strand type
    //   so we can display or not depending upon the type
    switch (strType)
    {
        case STRAND_VAR:
            // Check debug level
            if (gDbgLvl < 3)
                return;
            break;

        case STRAND_FCN:
            // Check debug level
            if (gDbgLvl < gFcnLvl)
                return;
            break;

        defstop
            break;
    } // End SWITCH

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    switch (strType)
    {
        case STRAND_VAR:
            DbgMsgW (L"********** Variable Strands ****************************");

            break;

        case STRAND_FCN:
            DbgMsgW (L"********** Function Strands ****************************");

            break;

        defstop
            break;
    } // End SWITCH

    wsprintfW (wszTemp,
               L"Start=%p Base=%p Next=%p",
               lpplLocalVars->lpYYStrandStart[strType],
               lpplLocalVars->lpYYStrandBase[strType],
               lpplLocalVars->lpYYStrandNext[strType]);
    DbgMsgW (wszTemp);

    for (lp = lpplLocalVars->lpYYStrandStart[strType], lpLast = NULL;
         lp NE lpplLocalVars->lpYYStrandNext[strType];
         lp ++)
    {
        if (lpLast NE lp->lpYYStrandBase)
        {
            DbgMsgW (L"--------------------------------------------------------");
            lpLast  = lp->lpYYStrandBase;
        } // End IF

        wsprintfW (wszTemp,
                   L"Strand (%p): %-9.9S D=%8I64X CI=%2d TC=%1d IN=%1d F=%p B=%p",
                   lp,
                   GetTokenTypeName (lp->tkToken.tkFlags.TknType),
                   lp->tkToken.tkData.tkInteger,
                   lp->tkToken.tkCharIndex,
                   lp->TknCount,
                   lp->YYIndirect,
                   lp->lpYYFcnBase,
                   lpLast);
        DbgMsgW (wszTemp);
    } // End FOR

    DbgMsgW (L"********** End Strands *********************************");
} // End DisplayStrand
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayUndo
//
//  Display the Undo Buffer
//***************************************************************************

void DisplayUndo
    (HWND hWnd)         // Window handle of the Edit Ctrl

{
    UINT         uCharPos,
                 uLineCount;
    HGLOBAL      hGlbEC;
    LPWCHAR      lpwsz, p;
    HWND         hWndParent;
    LPUNDO_BUF   lpUndoBeg,             // Ptr to start of Undo Buffer
                 lpUndoNxt;             // ...    next available slot in the Undo Buffer
    UBOOL        bShift;
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    WCHAR        wszTemp[1024];         // Ptr to temporary output area
    static LPWCHAR Actions[]={L"None",
                              L"Ins",
                              L"Rep",
                              L"Del",
                              L"Sel",
                              L"Back",
                              L"InsToggle"};

    // Check debug level
    if (gDbgLvl < 3)
        return;

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    DbgMsgW (L"********** Undo Buffer *********************************");

    // Get the shift key state
    bShift = (GetKeyState (VK_SHIFT) & 0x8000);

    // Get the char position of the caret
    uCharPos = GetCurCharPos (hWnd);

    // Get the lines in the text
    uLineCount = (UINT) SendMessageW (hWnd, EM_GETLINECOUNT, 0, 0);

    // Get the Edit Ctrl's memory handle
    (HANDLE_PTR) hGlbEC = SendMessageW (hWnd, EM_GETHANDLE, 0, 0);

    // Display it
    dprintfWL9 (L"Caret position = %d, # lines = %d, hGlbEC = %p (%S#%d)",
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
    dprintfWL9 (L"Text = <%s>",
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
    (HANDLE_PTR) lpUndoNxt = GetWindowLongPtrW (hWndParent, GWLSF_UNDO_NXT);
    (HANDLE_PTR) lpUndoBeg = GetWindowLongPtrW (hWndParent, GWLSF_UNDO_BEG);

    // Loop through the undo buffer entries
    for (; lpUndoBeg < lpUndoNxt; lpUndoBeg++)
    {
        wsprintfW (wszTemp,
                   L"Act = %9s, %2d-%2d, Group = %3d, Char = 0x%04X",
                   Actions[lpUndoBeg->Action],
                   lpUndoBeg->CharPosBeg,
                   lpUndoBeg->CharPosEnd,
                   lpUndoBeg->Group,
                   lpUndoBeg->Char);
        DbgMsgW (wszTemp);
    } // End FOR

    DbgMsgW (L"********** End Undo Buffer *****************************");
} // End DisplayUndo
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayFnHdr
//
//  Display the function header
//***************************************************************************

void DisplayFnHdr
    (LPFHLOCALVARS lpfhLocalVars)       // Ptr to Function Header local vars

{
    WCHAR     wszTemp[1024];            // Ptr to temporary output area
    HGLOBAL   hGlbName;
    LPAPLCHAR lpMemName;
    UINT      uLen, uItm;

    // Check debug level
    if (gDbgLvl < gFcnLvl)
        return;

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


#ifdef DEBUG
//***************************************************************************
//  $DisplayYYRes
//
//  Display a YYRes
//***************************************************************************

void DisplayYYRes
    (LPPL_YYSTYPE lpYYRes)

{
    LPDWORD lpdw;

    lpdw = (DWORD *) &lpYYRes->tkToken;

    wsprintfW (lpwszGlbTemp,
               L"%08X-%08X-%08X-%08X",
               lpdw[0],
               lpdw[1],
               lpdw[2],
               lpdw[3]);
    MBWC (lpwszGlbTemp);
} // End DisplayYYRes
#endif


//***************************************************************************
//  End of File: dispdbg.c
//***************************************************************************
