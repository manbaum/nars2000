//***************************************************************************
//  NARS2000 -- System Variable Routines
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "datatype.h"
#include "resdebug.h"
#include "symtab.h"
#include "tokens.h"
#include "parse.h"
#include "Unicode.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#else
#define SysFnDR_EM      NULL
#define SysFnSYSID_EM   NULL
#define SysFnSYSVER_EM  NULL
#define SysFnTC_EM      NULL
#define SysFnTCBEL_EM   NULL
#define SysFnTCBS_EM    NULL
#define SysFnTCDEL_EM   NULL
#define SysFnTCESC_EM   NULL
#define SysFnTCFF_EM    NULL
#define SysFnTCHT_EM    NULL
#define SysFnTCLF_EM    NULL
#define SysFnTCNL_EM    NULL
#define SysFnTCNUL_EM   NULL
#define SysFnTS_EM      NULL
#define SysFnTYPE_EM    NULL
#endif


// Global values of system variables so we can use them
//   without having to access the actual system var

HGLOBAL  hGlbQuadALX,       // []ALX
         hGlbQuadELX,       // []ELX
         hGlbQuadLX,        // []LX
         hGlbQuadSA,        // []SA
         hGlbQuadWSID;      // []WSID
APLFLOAT fQuadCT;           // []CT
APLBOOL  bQuadDF,           // []DF
         bQuadIF,           // []IF
         bQuadIO;           // []IO
APLINT   uQuadPP,           // []PP
         uQuadPW,           // []PW
         uQuadRL;           // []RL
char     cQuadPR;           // []PR


typedef struct tagSYSNAME
{
    LPWCHAR     lpwszName;  // The name
    UINT        uValence;   // For system functions, Niladic(0), All others (1)
    BOOL        bSysVar;    // Izit a system variable (TRUE) or function (FALSE)?  If TRUE, uValence is ignored
    LPYYSTYPE (*NameFcn) (LPTOKEN, LPTOKEN, LPTOKEN, LPTOKEN); // Ptr to execution routine
} SYSNAME, *LPSYSNAME;

SYSNAME aSystemNames[] =
{ // Name                    Valence    Var?
    {WS_UCS2_QUAD          ,    9,      TRUE , NULL          },    //  0:  Output
    {WS_UCS2_QUOTEQUAD     ,    9,      TRUE , NULL          },    //  1:  Character Output
    {WS_UCS2_QUAD L"alx"   ,    9,      TRUE , NULL          },    //  2:  Attention Latent Expression
    {WS_UCS2_QUAD L"ct"    ,    9,      TRUE , NULL          },    //  3:  Comparison Tolerance
    {WS_UCS2_QUAD L"df"    ,    9,      TRUE , NULL          },    //  4:  Display Format
    {WS_UCS2_QUAD L"elx"   ,    9,      TRUE , NULL          },    //  5:  Error Latent Expression
    {WS_UCS2_QUAD L"if"    ,    9,      TRUE , NULL          },    //  6:  Iota Format
    {WS_UCS2_QUAD L"io"    ,    9,      TRUE , NULL          },    //  7:  Index Origin
    {WS_UCS2_QUAD L"lx"    ,    9,      TRUE , NULL          },    //  8:  Latent Expression
    {WS_UCS2_QUAD L"pp"    ,    9,      TRUE , NULL          },    //  9:  Printing Precision
    {WS_UCS2_QUAD L"pr"    ,    9,      TRUE , NULL          },    // 10:  Prompt Replacement
    {WS_UCS2_QUAD L"pw"    ,    9,      TRUE , NULL          },    // 11:  Printing Width
    {WS_UCS2_QUAD L"rl"    ,    9,      TRUE , NULL          },    // 12:  Random Link
    {WS_UCS2_QUAD L"sa"    ,    9,      TRUE , NULL          },    // 13:  Stop Action
    {WS_UCS2_QUAD L"wsid"  ,    9,      TRUE , NULL          },    // 14:  Workspace Identifier

    {WS_UCS2_QUAD L"ai"    ,    0,      FALSE, NULL          },    // Accounting Information
    {WS_UCS2_QUAD L"av"    ,    0,      FALSE, NULL          },    // Atomic Vector
    {WS_UCS2_QUAD L"dm"    ,    0,      FALSE, NULL          },    // Diagnostic Message
    {WS_UCS2_QUAD L"lc"    ,    0,      FALSE, NULL          },    // Line Counter
    {WS_UCS2_QUAD L"si"    ,    0,      FALSE, NULL          },    // State Indicator
    {WS_UCS2_QUAD L"sinl"  ,    0,      FALSE, NULL          },    // State Indicator w/Name List
    {WS_UCS2_QUAD L"sysid" ,    0,      FALSE, SysFnSYSID_EM },    // System Identifier
    {WS_UCS2_QUAD L"sysver",    0,      FALSE, SysFnSYSVER_EM},    // System Version
    {WS_UCS2_QUAD L"tc"    ,    0,      FALSE, SysFnTC_EM    },    // Terminal Control Characters
    {WS_UCS2_QUAD L"tcbel" ,    0,      FALSE, SysFnTCBEL_EM },    // Terminal Control Character, Bell
    {WS_UCS2_QUAD L"tcbs"  ,    0,      FALSE, SysFnTCBS_EM  },    // Terminal Control Character, Backspace
    {WS_UCS2_QUAD L"tcdel" ,    0,      FALSE, SysFnTCDEL_EM },    // Terminal Control Character, Delete
    {WS_UCS2_QUAD L"tcesc" ,    0,      FALSE, SysFnTCESC_EM },    // Terminal Control Character, Escape
    {WS_UCS2_QUAD L"tcff"  ,    0,      FALSE, SysFnTCFF_EM  },    // Terminal Control Character, Form Feed
    {WS_UCS2_QUAD L"tcht"  ,    0,      FALSE, SysFnTCHT_EM  },    // Terminal Control Character, Horizontal Tab
    {WS_UCS2_QUAD L"tclf"  ,    0,      FALSE, SysFnTCLF_EM  },    // Terminal Control Character, Line Feed
    {WS_UCS2_QUAD L"tcnl"  ,    0,      FALSE, SysFnTCNL_EM  },    // Terminal Control Character, New Line
    {WS_UCS2_QUAD L"tcnul" ,    0,      FALSE, SysFnTCNUL_EM },    // Terminal Control Character, Null
    {WS_UCS2_QUAD L"ts"    ,    0,      FALSE, SysFnTS_EM    },    // Time Stamp
    {WS_UCS2_QUAD L"wa"    ,    0,      FALSE, NULL          },    // Workspace Available

    {WS_UCS2_QUAD L"call"  ,    1,      FALSE, NULL          },    // Call Assembler Code
    {WS_UCS2_QUAD L"cr"    ,    1,      FALSE, NULL          },    // Canonical Representation
    {WS_UCS2_QUAD L"crl"   ,    1,      FALSE, NULL          },    // Canonical Representation, Line
    {WS_UCS2_QUAD L"crlpc" ,    1,      FALSE, NULL          },    // Canonical Representation, Public Comment
    {WS_UCS2_QUAD L"def"   ,    1,      FALSE, NULL          },    // Define Function
    {WS_UCS2_QUAD L"defl"  ,    1,      FALSE, NULL          },    // Define Function Line
    {WS_UCS2_QUAD L"dl"    ,    1,      FALSE, NULL          },    // Delay Execution
    {WS_UCS2_QUAD L"dr"    ,    1,      FALSE, SysFnDR_EM    },    // Data Representation
    {WS_UCS2_QUAD L"erase" ,    1,      FALSE, NULL          },    // Erase Names
    {WS_UCS2_QUAD L"error" ,    1,      FALSE, NULL          },    // Signal Error
    {WS_UCS2_QUAD L"ex"    ,    1,      FALSE, NULL          },    // Erase Names
    {WS_UCS2_QUAD L"fi"    ,    1,      FALSE, NULL          },    // Format Items
    {WS_UCS2_QUAD L"fmt"   ,    1,      FALSE, NULL          },    // Format
    {WS_UCS2_QUAD L"fx"    ,    1,      FALSE, NULL          },    // Fix Function
    {WS_UCS2_QUAD L"idlist",    1,      FALSE, NULL          },    // Identifier List
    {WS_UCS2_QUAD L"idloc" ,    1,      FALSE, NULL          },    // Identifier Localization
    {WS_UCS2_QUAD L"lock"  ,    1,      FALSE, NULL          },    // Lock Functions
    {WS_UCS2_QUAD L"mf"    ,    1,      FALSE, NULL          },    // Monitor Function
    {WS_UCS2_QUAD L"nc"    ,    1,      FALSE, NULL          },    // Name Classification
    {WS_UCS2_QUAD L"nl"    ,    1,      FALSE, NULL          },    // Name List
    {WS_UCS2_QUAD L"size"  ,    1,      FALSE, NULL          },    // Size of an object
    {WS_UCS2_QUAD L"ss"    ,    1,      FALSE, NULL          },    // Search String
    {WS_UCS2_QUAD L"stop"  ,    1,      FALSE, NULL          },    // Manage Stop Points
    {WS_UCS2_QUAD L"trace" ,    1,      FALSE, NULL          },    // Manage Trace Points
    {WS_UCS2_QUAD L"type"  ,    1,      FALSE, SysFnTYPE_EM  },    // Manage Trace Points
    {WS_UCS2_QUAD L"vi"    ,    1,      FALSE, NULL          },    // Verify Items
    {WS_UCS2_QUAD L"vr"    ,    1,      FALSE, NULL          },    // Vector Representation of a Function

    // ***FIXME*** Add more entries
};


//***************************************************************************
//  SymTabAppendSysName_EM
//
//  Append a system name to the symbol table
//***************************************************************************

BOOL SymTabAppendSysName_EM
    (LPSYSNAME lpSysName)

{
    STFLAGS    stFlags = {0};
    LPSYMENTRY lpSymEntry;

    // Set the flags of the entry we're appending
    if (lpSysName->bSysVar)
    {
        stFlags.SysVar =
        stFlags.Value  = 1;
    } else
    {
        if (lpSysName->uValence EQ 0)
            stFlags.SysFn0 = 1;
        else
            stFlags.SysFn12 = 1;
    } // End IF/ELSE

    stFlags.SysName =
    stFlags.Perm    =
    stFlags.Nocase  =
    stFlags.Inuse   = 1;

    lpSymEntry = SymTabAppendNewName_EM (lpSysName->lpwszName, &stFlags);

    // Check for error
    if (!lpSymEntry)
        return FALSE;

    // Save the address of the execution routine
    lpSymEntry->stData.stNameFcn = lpSysName->NameFcn;

    return TRUE;
} // End SymTabAppendSysName_EM


//***************************************************************************
//  AppendSystemNames_EM
//
//  Append all system names to the symbol table
//***************************************************************************

BOOL AppendSystemNames_EM (void)

{
    int i;

    Assert (HshTabFrisk ());

    // Append all system names
    for (i = 0; i < sizeof (aSystemNames) / sizeof (aSystemNames[0]); i++)
    {
        if (!SymTabAppendSysName_EM (&aSystemNames[i]))
            return FALSE;
    } // End FOR

    Assert (HshTabFrisk ());

    return TRUE;
} // End AppendSystemNames_EM


//***************************************************************************
//  AssignCharVector_EM
//
//  Assign a character vector to a name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AssignCharVector_EM"
#else
#define APPEND_NAME
#endif

void AssignCharVector_EM
    (LPWCHAR lpwszName,
     LPWCHAR lpwszString,
     UINT    SysVarValid,
     LPVOID  lpGlbVal)

{
    LPSYMENTRY lpSymEntryDest;
    STFLAGS    stFlags = {0};
    int        iStringLen;
    HGLOBAL    hGlb;

    // Lookup the name in the symbol table
    // SymTabLookupName sets the .SysName or .UsrName flag,
    //   and the .Inuse flag
    lpSymEntryDest = SymTabLookupName (lpwszName, &stFlags);
    if (!lpSymEntryDest)        // It's not found???
    {
        ErrorMessageIndirect (ERRMSG_PROGRAM_ERROR APPEND_NAME);

        DbgStop ();             // We should never get here

        return;
    } // End IF

    // Get the string length (exluding the terminating zero)
    iStringLen = lstrlenW (lpwszString);

    // Allocate global memory for the array header,
    //   one dimension (it's a vector), and the string
    //   excluding the terminating zero.
    hGlb = DbgGlobalAlloc (GHND,
                           sizeof (VARARRAY_HEADER)
                         + sizeof (APLDIM) * 1
                         + iStringLen * sizeof (WCHAR));
    if (!hGlb)
    {
        ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);

        return;
    } else
    {
        LPWCHAR lpwsz;

        // Lock the handle, setup the header, and copy
        //   the string to the global memory
        lpwsz = MyGlobalLock (hGlb);

#define lpHeader    ((LPVARARRAY_HEADER) lpwsz)

        lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_CHAR;
////////lpHeader->Perm       = 0;
        lpHeader->SysVar     = 1;
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = iStringLen;
        lpHeader->Rank       = 1;

#undef  lpHeader

        *VarArrayBaseToDim (lpwsz) = iStringLen;
        CopyMemory (VarArrayBaseToData (lpwsz, 1),
                    lpwszString,
                    iStringLen * sizeof (APLCHAR));
        MyGlobalUnlock (hGlb); lpwsz = NULL;

        // Save in global ptr
        *((HGLOBAL *) lpGlbVal) = hGlb;
    } // End IF/ELSE

    // Save the global memory ptr
    lpSymEntryDest->stData.stGlbData = MakeGlbTypeGlb (hGlb);

    // Save the flags
    stFlags.SysVarValid = SysVarValid;
    *(UINT *)&lpSymEntryDest->stFlags |= *(UINT *)&stFlags;
} // End AssignCharVector_EM
#undef  APPEND_NAME


//***************************************************************************
//  AssignRealScalar_EM
//
//  Assign a real number scalar to a name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AssignRealScalar_EM"
#else
#define APPEND_NAME
#endif

void AssignRealScalar_EM
    (LPWCHAR  lpwszName,
     APLFLOAT fFloat,
     UINT     SysVarValid,
     LPVOID  lpGlbVal)

{
    LPSYMENTRY lpSymEntryDest;
    STFLAGS    stFlags = {0};

    // Lookup the name in the symbol table
    // SymTabLookupName sets the .SysName or .UsrName flag,
    //   and the .Inuse flag
    lpSymEntryDest = SymTabLookupName (lpwszName, &stFlags);
    if (!lpSymEntryDest)        // It's not found???
    {
        ErrorMessageIndirect (ERRMSG_PROGRAM_ERROR APPEND_NAME);

        DbgStop ();             // We should never get here

        return;
    } // End IF

    // Save the constant
    *((LPAPLFLOAT) lpGlbVal) = lpSymEntryDest->stData.stFloat = fFloat;

    // Mark as immediate floating point constant
    stFlags.Imm     = 1;
    stFlags.ImmType = IMMTYPE_FLOAT;

    // Save the flags
    stFlags.SysVarValid = SysVarValid;
    *(UINT *)&lpSymEntryDest->stFlags |= *(UINT *)&stFlags;
} // End AssignRealScalar_EM
#undef  APPEND_NAME


//***************************************************************************
//  AssignBoolScalar
//
//  Assign an Boolean scalar to a name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AssignBoolScalar_EM"
#else
#define APPEND_NAME
#endif

void AssignBoolScalar_EM
    (LPWCHAR lpwszName,
     APLBOOL aplBoolean,
     UINT    SysVarValid,
     LPVOID  lpGlbVal)

{
    LPSYMENTRY lpSymEntryDest;
    STFLAGS    stFlags = {0};

    // Lookup the name in the symbol table
    // SymTabLookupName sets the .SysName or .UsrName flag,
    //   and the .Inuse flag
    lpSymEntryDest = SymTabLookupName (lpwszName, &stFlags);
    if (!lpSymEntryDest)        // It's not found???
    {
        ErrorMessageIndirect (ERRMSG_PROGRAM_ERROR APPEND_NAME);

        DbgStop ();             // We should never get here

        return;
    } // End IF

    // Save the constant
    *((LPAPLBOOL) lpGlbVal) = lpSymEntryDest->stData.stBoolean = aplBoolean;

    // Mark as immediate Boolean constant
    stFlags.Imm     = 1;
    stFlags.ImmType = IMMTYPE_BOOL;

    // Save the flags
    stFlags.SysVarValid = SysVarValid;
    *(UINT *)&lpSymEntryDest->stFlags |= *(UINT *)&stFlags;
} // End AssignBoolScalar_EM
#undef  APPEND_NAME


//***************************************************************************
//  AssignIntScalar_EM
//
//  Assign an integer scalar to a name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AssignIntScalar_EM"
#else
#define APPEND_NAME
#endif

void AssignIntScalar_EM
    (LPWCHAR lpwszName,
     APLINT  aplInteger,
     UINT    SysVarValid,
     LPVOID  lpGlbVal)

{
    LPSYMENTRY lpSymEntryDest;
    STFLAGS    stFlags = {0};

    // Lookup the name in the symbol table
    // SymTabLookupName sets the .SysName or .UsrName flag,
    //   and the .Inuse flag
    lpSymEntryDest = SymTabLookupName (lpwszName, &stFlags);
    if (!lpSymEntryDest)        // It's not found???
    {
        ErrorMessageIndirect (ERRMSG_PROGRAM_ERROR APPEND_NAME);

        DbgStop ();             // We should never get here

        return;
    } // End IF

    // Save the constant
    *((LPAPLINT) lpGlbVal) = lpSymEntryDest->stData.stInteger = aplInteger;

    // Mark as immediate Integer constant
    stFlags.Imm     = 1;
    stFlags.ImmType = IMMTYPE_INT;

    // Save the flags
    stFlags.SysVarValid = SysVarValid;
    *(UINT *)&lpSymEntryDest->stFlags |= *(UINT *)&stFlags;
} // End AssignIntScalar_EM
#undef  APPEND_NAME


//***************************************************************************
//  AssignCharScalar_EM
//
//  Assign a character scalar to a name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AssignCharScalar_EM"
#else
#define APPEND_NAME
#endif

void AssignCharScalar_EM
    (LPWCHAR lpwszName,
     WCHAR   aplChar,
     UINT    SysVarValid,
     LPVOID  lpGlbVal)

{
    LPSYMENTRY lpSymEntryDest;
    STFLAGS    stFlags = {0};

    // Lookup the name in the symbol table
    // SymTabLookupName sets the .SysName or .UsrName flag,
    //   and the .Inuse flag
    lpSymEntryDest = SymTabLookupName (lpwszName, &stFlags);
    if (!lpSymEntryDest)        // It's not found???
    {
        ErrorMessageIndirect (ERRMSG_PROGRAM_ERROR APPEND_NAME);

        DbgStop ();             // We should never get here

        return;
    } // End IF

    // Save the constant
    *((LPAPLCHAR) lpGlbVal) = lpSymEntryDest->stData.stChar = aplChar;

    // Mark as immediate Character constant
    stFlags.Imm     = 1;
    stFlags.ImmType = IMMTYPE_CHAR;

    // Save the flags
    stFlags.SysVarValid = SysVarValid;
    *(UINT *)&lpSymEntryDest->stFlags |= *(UINT *)&stFlags;
} // End AssignCharScalar_EM
#undef  APPEND_NAME


//***************************************************************************
//  ValidateBoolean_EM
//
//  Validate a value about to be assigned to a Boolean system var.
//
//  We allow any numeric scalar or one-element vector whose value is 0 or 1.
//
//  The order of error checking is RANK, LENGTH, DOMAIN.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ValidateBoolean_EM"
#else
#define APPEND_NAME
#endif

BOOL ValidateBoolean_EM
    (LPYYSTYPE lpYYName,
     LPTOKEN   lpToken,
     LPAPLBOOL lpVal)

{
    LPVOID   lpMem, lpData;
    BOOL     bRet = TRUE;
    LPWCHAR  lpwErrMsg = ERRMSG_DOMAIN_ERROR APPEND_NAME;
    APLINT   aplInteger;

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            DbgBrk ();      // ***TESTME***

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

            if (!lpToken->tkData.lpSym->stFlags.Imm)
                break;      // Continue with HGLOBAL processing

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // Split case based upon the symbol table immediate type
            switch (lpToken->tkData.lpSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    *lpVal = lpToken->tkData.lpSym->stData.stBoolean;

                    break;

                case IMMTYPE_INT:
                    // Test the value
                    bRet = (lpToken->tkData.lpSym->stData.stInteger EQ 0
                         || lpToken->tkData.lpSym->stData.stInteger EQ 1);
                    if (bRet)
                        *lpVal = (APLBOOL) (lpToken->tkData.lpSym->stData.stInteger);
                    break;

                case IMMTYPE_FLOAT:
                    // Convert the value to an integer using System CT
                    aplInteger = FloatToAplint_SCT (lpToken->tkData.lpSym->stData.stFloat,
                                                    &bRet);
                    // Test the value
                    bRet = bRet && (aplInteger EQ 0 || aplInteger EQ 1);

                    if (bRet)
                        *lpVal = (APLBOOL) aplInteger;
                    break;

                case IMMTYPE_CHAR:
                    bRet = FALSE;

                    break;
            } // End SWITCH

            goto NORMAL_EXIT;

        case TKT_VARIMMED:
            // Split case based upon the token immediate type
            switch (lpToken->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    *lpVal = lpToken->tkData.tkBoolean;

                    break;

                case IMMTYPE_INT:
                    // Test the value
                    bRet = (lpToken->tkData.tkInteger EQ 0
                         || lpToken->tkData.tkInteger EQ 1);
                    if (bRet)
                        *lpVal = (APLBOOL) (lpToken->tkData.tkInteger);
                    break;

                case IMMTYPE_FLOAT:
                    // Convert the value to an integer using System CT
                    aplInteger = FloatToAplint_SCT (lpToken->tkData.tkFloat,
                                                    &bRet);
                    // Test the value
                    bRet = bRet && (aplInteger EQ 0 || aplInteger EQ 1);

                    if (bRet)
                        *lpVal = (APLBOOL) aplInteger;
                    break;

                case IMMTYPE_CHAR:
                    bRet = FALSE;

                    break;
            } // End SWITCH

            goto NORMAL_EXIT;

        case TKT_LIST:      // The tkData is an HGLOBAL of an array of HGLOBALs
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME, lpToken);

            return FALSE;

        case TKT_STRING:    // tkData is an HGLOBAL of an array of ???
        case TKT_VARARRAY:  // tkData is an HGLOBAL of an array of ???
            break;          // Continue with HGLOBAL processing

        defstop
            return FALSE;
    } // End SWITCH

    // tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (lpToken->tkData.tkGlbData));

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (ClrPtrTypeDirGlb (lpToken->tkData.tkGlbData));

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    // Skip over the header and dimensions to the data
    lpData = VarArrayBaseToData (lpHeader, lpHeader->Rank);

    // Check for scalar or vector
    if (lpHeader->Rank > 1)
    {
        lpwErrMsg = ERRMSG_RANK_ERROR APPEND_NAME;

        bRet = FALSE;
    } else
    // Check for singleton
    if (lpHeader->NELM NE 1)
    {
        lpwErrMsg = ERRMSG_LENGTH_ERROR APPEND_NAME;

        bRet = FALSE;
    } else
    // Split cases based upon the array type
    switch (lpHeader->ArrType)
    {
        case ARRAY_BOOL:
            *lpVal = *(LPAPLBOOL) lpData;

            break;

        case ARRAY_INT:
            // Get the value
            aplInteger = *(LPAPLINT) lpData;

            // Test the value
            bRet = (aplInteger EQ 0
                 || aplInteger EQ 1);
            if (bRet)
                *lpVal = (APLBOOL) (*(LPAPLINT) lpData);
            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            bRet = FALSE;

            break;

        case ARRAY_FLOAT:
            // Convert the value to an integer using System CT
            aplInteger = FloatToAplint_SCT (*(LPAPLFLOAT) lpData,
                                            &bRet);
            // Test the value
            bRet = (aplInteger EQ 0
                 || aplInteger EQ 1);
            if (bRet)
                *lpVal = (APLBOOL) aplInteger;
            break;

        defstop
            break;
    } // End IF/ELSE/SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (ClrPtrTypeDirGlb (lpToken->tkData.tkGlbData)); lpMem= NULL;
NORMAL_EXIT:
    // If in error, set error message;
    //   otherwise, save the value in the name
    if (!bRet)
        ErrorMessageIndirectToken (lpwErrMsg, lpToken);
    else
    {
        lpYYName->tkToken.tkData.lpSym->stData.stBoolean = *lpVal;
        lpYYName->tkToken.tkFlags.NoDisplay = 1;
    } // End IF

    return bRet;
#undef  lpHeader
} // End ValidateBoolean_EM
#undef  APPEND_NAME


//***************************************************************************
//  ValidateInteger_EM
//
//  Validate a value about to be assigned to an integer system var.
//
//  We allow any numeric scalar or one-element vector whose value
//    is integral and in a given range.
//
//  The order of error checking is RANK, LENGTH, DOMAIN.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ValidateInteger_EM"
#else
#define APPEND_NAME
#endif

BOOL ValidateInteger_EM
    (LPYYSTYPE lpYYName,
     LPTOKEN   lpToken,
     UINT      uValidLo,        // Low range value (inclusive)
     UINT      uValidHi,        // High ...
     LPAPLINT  lpVal)

{
    LPVOID   lpMem, lpData;
    BOOL     bRet = TRUE;
    LPWCHAR  lpwErrMsg = ERRMSG_DOMAIN_ERROR APPEND_NAME;
    APLINT   aplInteger;

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            DbgBrk ();      // ***TESTME***

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

            if (!lpToken->tkData.lpSym->stFlags.Imm)
                break;      // Continue with HGLOBAL processing

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // Split case based upon the symbol table immediate type
            switch (lpToken->tkData.lpSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    // Get the value
                    aplInteger = lpToken->tkData.lpSym->stData.stBoolean;

                    // Test the value
                    if (uValidLo <= aplInteger
                     &&             aplInteger <= uValidHi)
                        *lpVal = aplInteger;
                    else
                        bRet = FALSE;
                    break;

                case IMMTYPE_INT:
                    // Get the value
                    aplInteger = lpToken->tkData.lpSym->stData.stInteger;

                    // Test the value
                    if (uValidLo <= aplInteger
                     &&             aplInteger <= uValidHi)
                        *lpVal = aplInteger;
                    else
                        bRet = FALSE;
                    break;

                case IMMTYPE_FLOAT:
                    // Convert the value to an integer using System CT
                    aplInteger = FloatToAplint_SCT (lpToken->tkData.lpSym->stData.stFloat,
                                                    &bRet);
                    // Test the value
                    if (uValidLo <= aplInteger
                     &&             aplInteger <= uValidHi)
                        *lpVal = aplInteger;
                    else
                        bRet = FALSE;
                    break;

                case IMMTYPE_CHAR:
                    bRet = FALSE;

                    break;
            } // End SWITCH

            goto NORMAL_EXIT;

        case TKT_VARIMMED:
            // Split case based upon the token immediate type
            switch (lpToken->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    // Get the value
                    aplInteger = lpToken->tkData.tkBoolean;

                    // Test the value
                    if (uValidLo <= aplInteger
                     &&             aplInteger <= uValidHi)
                        *lpVal = aplInteger;
                    else
                        bRet = FALSE;
                    break;

                case IMMTYPE_INT:
                    // Get the value
                    aplInteger = lpToken->tkData.tkInteger;

                    // Test the value
                    if (uValidLo <= aplInteger
                     &&             aplInteger <= uValidHi)
                        *lpVal = aplInteger;
                    else
                        bRet = FALSE;
                    break;

                case IMMTYPE_FLOAT:
                    // Convert the value to an integer using System CT
                    aplInteger = FloatToAplint_SCT (lpToken->tkData.tkFloat,
                                                    &bRet);
                    // Test the value
                    if (uValidLo <= aplInteger
                     &&             aplInteger <= uValidHi)
                        *lpVal = aplInteger;
                    else
                        bRet = FALSE;
                    break;

                case IMMTYPE_CHAR:
                    bRet = FALSE;

                    break;
            } // End SWITCH

            goto NORMAL_EXIT;

        case TKT_LIST:      // The tkData is an HGLOBAL of an array of HGLOBALs
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME, lpToken);

            return FALSE;

        case TKT_STRING:    // tkData is an HGLOBAL of an array of ???
        case TKT_VARARRAY:  // tkData is an HGLOBAL of an array of ???
            break;          // Continue with HGLOBAL processing

        defstop
            return FALSE;
    } // End SWITCH

    // tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (lpToken->tkData.tkGlbData));

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (ClrPtrTypeDirGlb (lpToken->tkData.tkGlbData));

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    // Skip over the header and dimensions to the data
    lpData = VarArrayBaseToData (lpHeader, lpHeader->Rank);

    // Check for scalar or vector
    if (lpHeader->Rank > 1)
    {
        lpwErrMsg = ERRMSG_RANK_ERROR APPEND_NAME;

        bRet = FALSE;
    } else
    // Check for singleton
    if (lpHeader->NELM NE 1)
    {
        lpwErrMsg = ERRMSG_LENGTH_ERROR APPEND_NAME;

        bRet = FALSE;
    } else
    // Split cases based upon the array type
    switch (lpHeader->ArrType)
    {
        case ARRAY_BOOL:
            // Get the value
            aplInteger = *(LPAPLBOOL) lpData;

            // Test the value
            if (uValidLo <= aplInteger
             &&             aplInteger <= uValidHi)
                *lpVal = aplInteger;
            else
                bRet = FALSE;
            break;

        case ARRAY_INT:
            // Get the value
            aplInteger = *(LPAPLINT) lpData;

            // Test the value
            if (uValidLo <= aplInteger
             &&             aplInteger <= uValidHi)
                *lpVal = aplInteger;
            else
                bRet = FALSE;
            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            bRet = FALSE;

            break;

        case ARRAY_FLOAT:
            // Convert the value to an integer using System CT
            aplInteger = FloatToAplint_SCT (*(LPAPLFLOAT) lpData,
                                            &bRet);
            // Test the value
            if (uValidLo <= aplInteger
             &&             aplInteger <= uValidHi)
                *lpVal = aplInteger;
            else
                bRet = FALSE;
            break;

        defstop
            break;
    } // End IF/ELSE/SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (ClrPtrTypeDirGlb (lpToken->tkData.tkGlbData)); lpMem= NULL;
NORMAL_EXIT:
    // If in error, set error message;
    //   otherwise, save the value in the name
    if (!bRet)
        ErrorMessageIndirectToken (lpwErrMsg, lpToken);
    else
    {
        lpYYName->tkToken.tkData.lpSym->stData.stInteger = *lpVal;
        lpYYName->tkToken.tkFlags.NoDisplay = 1;
    } // End IF

    return bRet;
#undef  lpHeader
} // End ValidateInteger_EM
#undef  APPEND_NAME


//***************************************************************************
//  ValidateFloat_EM
//
//  Validate a value about to be assigned to an float system var.
//
//  We allow any numeric scalar or one-element vector whose value
//    is in a given range.
//
//  The order of error checking is RANK, LENGTH, DOMAIN.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ValidateFloat_EM"
#else
#define APPEND_NAME
#endif

BOOL ValidateFloat_EM
    (LPYYSTYPE  lpYYName,
     LPTOKEN    lpToken,
     APLFLOAT   fValidLo,       // Low range value (inclusive)
     APLFLOAT   fValidHi,       // High ...
     LPAPLFLOAT lpVal)

{
    LPVOID   lpMem, lpData;
    BOOL     bRet = TRUE;
    LPWCHAR  lpwErrMsg = ERRMSG_DOMAIN_ERROR APPEND_NAME;
    APLFLOAT aplFloat;

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            DbgBrk ();      // ***TESTME***

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

            if (!lpToken->tkData.lpSym->stFlags.Imm)
                break;      // Continue with HGLOBAL processing

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // Split case based upon the symbol table immediate type
            switch (lpToken->tkData.lpSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    // Get the value
                    aplFloat = lpToken->tkData.lpSym->stData.stBoolean;

                    // Test the value
                    if (fValidLo <= aplFloat
                     &&             aplFloat <= fValidHi)
                        *lpVal = aplFloat;
                    else
                        bRet = FALSE;
                    break;

                case IMMTYPE_INT:
                    // Get the value
                    aplFloat = (APLFLOAT) (lpToken->tkData.lpSym->stData.stInteger);

                    // Test the value
                    if (fValidLo <= aplFloat
                     &&             aplFloat <= fValidHi)
                        *lpVal = aplFloat;
                    else
                        bRet = FALSE;
                    break;

                case IMMTYPE_FLOAT:
                    aplFloat = lpToken->tkData.lpSym->stData.stFloat;

                    // Test the value
                    if (fValidLo <= aplFloat
                     &&             aplFloat <= fValidHi)
                        *lpVal = aplFloat;
                    else
                        bRet = FALSE;
                    break;

                case IMMTYPE_CHAR:
                    bRet = FALSE;

                    break;
            } // End SWITCH

            goto NORMAL_EXIT;

        case TKT_VARIMMED:
            // Split case based upon the token immediate type
            switch (lpToken->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    // Get the value
                    aplFloat = lpToken->tkData.tkBoolean;

                    // Test the value
                    if (fValidLo <= aplFloat
                     &&             aplFloat <= fValidHi)
                        *lpVal = aplFloat;
                    else
                        bRet = FALSE;
                    break;

                case IMMTYPE_INT:
                    // Get the value
                    aplFloat = (APLFLOAT) (lpToken->tkData.tkInteger);

                    // Test the value
                    if (fValidLo <= aplFloat
                     &&             aplFloat <= fValidHi)
                        *lpVal = aplFloat;
                    else
                        bRet = FALSE;
                    break;

                case IMMTYPE_FLOAT:
                    // Convert the value to an integer using System CT
                    aplFloat = lpToken->tkData.tkFloat;

                    // Test the value
                    if (fValidLo <= aplFloat
                     &&             aplFloat <= fValidHi)
                        *lpVal = aplFloat;
                    else
                        bRet = FALSE;
                    break;

                case IMMTYPE_CHAR:
                    bRet = FALSE;

                    break;
            } // End SWITCH

            goto NORMAL_EXIT;

        case TKT_LIST:      // The tkData is an HGLOBAL of an array of HGLOBALs
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME, lpToken);

            return FALSE;

        case TKT_STRING:    // tkData is an HGLOBAL of an array of ???
        case TKT_VARARRAY:  // tkData is an HGLOBAL of an array of ???
            break;          // Continue with HGLOBAL processing

        defstop
            return FALSE;
    } // End SWITCH

    // tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (lpToken->tkData.tkGlbData));

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (ClrPtrTypeDirGlb (lpToken->tkData.tkGlbData));

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    // Skip over the header and dimensions to the data
    lpData = VarArrayBaseToData (lpHeader, lpHeader->Rank);

    // Check for scalar or vector
    if (lpHeader->Rank > 1)
    {
        lpwErrMsg = ERRMSG_RANK_ERROR APPEND_NAME;

        bRet = FALSE;
    } else
    // Check for singleton
    if (lpHeader->NELM NE 1)
    {
        lpwErrMsg = ERRMSG_LENGTH_ERROR APPEND_NAME;

        bRet = FALSE;
    } else
    // Split cases based upon the array type
    switch (lpHeader->ArrType)
    {
        case ARRAY_BOOL:
            // Get the value
            aplFloat = *(LPAPLBOOL) lpData;

            // Test the value
            if (fValidLo <= aplFloat
             &&             aplFloat <= fValidHi)
                *lpVal = aplFloat;
            else
                bRet = FALSE;
            break;

        case ARRAY_INT:
            // Get the value
            aplFloat = (APLFLOAT) *(LPAPLINT) lpData;

            // Test the value
            if (fValidLo <= aplFloat
             &&             aplFloat <= fValidHi)
                *lpVal = aplFloat;
            else
                bRet = FALSE;
            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            bRet = FALSE;

            break;

        case ARRAY_FLOAT:
            // Convert the value to an integer using System CT
            aplFloat = *(LPAPLFLOAT) lpData;

            // Test the value
            if (fValidLo <= aplFloat
             &&             aplFloat <= fValidHi)
                *lpVal = aplFloat;
            else
                bRet = FALSE;
            break;

        defstop
            break;
    } // End IF/ELSE/SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (ClrPtrTypeDirGlb (lpToken->tkData.tkGlbData)); lpMem= NULL;
NORMAL_EXIT:
    // If in error, set error message;
    //   otherwise, save the value in the name
    if (!bRet)
        ErrorMessageIndirectToken (lpwErrMsg, lpToken);
    else
    {
        lpYYName->tkToken.tkData.lpSym->stData.stFloat = *lpVal;
        lpYYName->tkToken.tkFlags.NoDisplay = 1;
    } // End IF

    return bRet;
#undef  lpHeader
} // End ValidateFloat_EM
#undef  APPEND_NAME


//***************************************************************************
//  ValidateALX_EM
//
//  Validate a value about to be assigned to Quad-ALX
//***************************************************************************

BOOL ValidateALX_EM
    (LPYYSTYPE lpYYName,
     LPTOKEN lpToken)

{
    // Ensure the argument is a character scalar (promoted to a vector)
    //   or vector.

    // ***FINISHME***


    return FALSE;
} // End ValidateALX_EM


//***************************************************************************
//  ValidateCT_EM
//
//  Validate a value about to be assigned to Quad-CT
//***************************************************************************

BOOL ValidateCT_EM
    (LPYYSTYPE lpYYName,
     LPTOKEN lpToken)

{
    // Ensure the argument is a real scalar or
    //   one-element vector (demoted to a scalar)
    //   between 0 and 1E-10 inclusive.

    return ValidateFloat_EM (lpYYName, lpToken, 0, 1E-10, &fQuadCT);
} // End ValidateCT_EM


//***************************************************************************
//  ValidateDF_EM
//
//  Validate a value about to be assigned to Quad-DF
//
//  We allow any numeric singleton whose value is 0 or 1.
//
//  The order of error checking is RANK, LENGTH, DOMAIN.
//***************************************************************************

BOOL ValidateDF_EM
    (LPYYSTYPE lpYYName,
     LPTOKEN lpToken)

{
    // Ensure the argument is a Boolean scalar or
    //   one-element vector (demoted to a scalar).

    return ValidateBoolean_EM (lpYYName, lpToken, &bQuadDF);
} // End ValidateDF_EM


//***************************************************************************
//  ValidateELX_EM
//
//  Validate a value about to be assigned to Quad-ELX
//***************************************************************************

BOOL ValidateELX_EM
    (LPYYSTYPE lpYYName,
     LPTOKEN lpToken)

{
    // Ensure the argument is a character scalar (promoted to a vector)
    //   or vector

    // ***FINISHME***


    return FALSE;
} // End ValidateELX_EM


//***************************************************************************
//  ValidateIF_EM
//
//  Validate a value about to be assigned to Quad-IF
//
//  We allow any numeric singleton whose value is 0 or 1.
//
//  The order of error checking is RANK, LENGTH, DOMAIN.
//***************************************************************************

BOOL ValidateIF_EM
    (LPYYSTYPE lpYYName,
     LPTOKEN lpToken)

{
    // Ensure the argument is a Boolean scalar or
    //   one-element vector (demoted to a scalar).

    return ValidateBoolean_EM (lpYYName, lpToken, &bQuadIF);
} // End ValidateIF_EM


//***************************************************************************
//  ValidateIO_EM
//
//  Validate a value about to be assigned to Quad-IO
//
//  We allow any numeric singleton whose value is 0 or 1.
//
//  The order of error checking is RANK, LENGTH, DOMAIN.
//***************************************************************************

BOOL ValidateIO_EM
    (LPYYSTYPE lpYYName,
     LPTOKEN lpToken)

{
    // Ensure the argument is a Boolean scalar or
    //   one-element vector (demoted to a scalar).

    return ValidateBoolean_EM (lpYYName, lpToken, &bQuadIO);
} // End ValidateIO_EM


//***************************************************************************
//  ValidateLX_EM
//
//  Validate a value about to be assigned to Quad-LX
//***************************************************************************

BOOL ValidateLX_EM
    (LPYYSTYPE lpYYName,
     LPTOKEN lpToken)

{
    // Ensure the argument is a character scalar (promoted to a vector)
    //   or vector.

    // ***FINISHME***


    return FALSE;
} // End ValidateLX_EM


//***************************************************************************
//  ValidatePP_EM
//
//  Validate a value about to be assigned to Quad-PP
//***************************************************************************

BOOL ValidatePP_EM
    (LPYYSTYPE lpYYName,
     LPTOKEN lpToken)

{
    // Ensure the argument is an integer scalar or
    //   one-element vector (demoted to a scalar)
    //   in the range from 1 to 17, inclusive.

    return ValidateInteger_EM (lpYYName, lpToken, 1, 17, &uQuadPP);
} // End ValidatePP_EM


//***************************************************************************
//  ValidatePR_EM
//
//  Validate a value about to be assigned to Quad-PR
//***************************************************************************

BOOL ValidatePR_EM
    (LPYYSTYPE lpYYName,
     LPTOKEN lpToken)

{
    // Ensure the argument is a character scalar, or
    //   one-element vector (demoted to a scalar), or
    //   an empty vector.

    // ***FINISHME***


    return FALSE;
} // End ValidatePR_EM


//***************************************************************************
//  ValidatePW_EM
//
//  Validate a value about to be assigned to Quad-PW
//***************************************************************************

BOOL ValidatePW_EM
    (LPYYSTYPE lpYYName,
     LPTOKEN lpToken)

{
    // Ensure the argument is an integer scalar or
    //   one-element vector (demoted to a scalar)
    //   in the range from 30 to 255, inclusive.

    return ValidateInteger_EM (lpYYName, lpToken, 30, 255, &uQuadPP);
} // End ValidatePW_EM


//***************************************************************************
//  ValidateRL_EM
//
//  Validate a value about to be assigned to Quad-RL
//***************************************************************************

BOOL ValidateRL_EM
    (LPYYSTYPE lpYYName,
     LPTOKEN lpToken)

{
    // Ensure the argument is an integer scalar or
    //   one-element vector (demoted to a scalar)
    //   in the range from 1 to (-2)+2*31, inclusive.

    return ValidateInteger_EM (lpYYName, lpToken, 1, 0x7FFFFFFE, &uQuadPP);
} // End ValidateRL_EM


//***************************************************************************
//  ValidateSA_EM
//
//  Validate a value about to be assigned to Quad-SA
//***************************************************************************

BOOL ValidateSA_EM
    (LPYYSTYPE lpYYName,
     LPTOKEN lpToken)

{
    // Ensure the argument is a character scalar (promoted to a vector)
    //   or vector, and a valid Stop Action value
    //   ('', 'EXIT', 'ERROR', 'CLEAR', 'OFF')

    // ***FINISHME***


    return FALSE;
} // End ValidateSA_EM


//***************************************************************************
//  ValidateWSID_EM
//
//  Validate a value about to be assigned to Quad-WSID
//***************************************************************************

BOOL ValidateWSID_EM
    (LPYYSTYPE lpYYName,
     LPTOKEN lpToken)

{
    // Ensure the argument is a character scalar (promoted to a vector)
    //   or vector, and a valid workspace name

    // ***FINISHME***


    return FALSE;
} // End ValidateWSID_EM


enum SYSVAR_VALID
{
    SYSVAR_VALID_ALX = 0,   //  0:
    SYSVAR_VALID_CT  ,      //  1:
    SYSVAR_VALID_DF  ,      //  2:
    SYSVAR_VALID_ELX ,      //  3:
    SYSVAR_VALID_IF  ,      //  4:
    SYSVAR_VALID_IO  ,      //  5:
    SYSVAR_VALID_LX  ,      //  6:
    SYSVAR_VALID_PP  ,      //  7:
    SYSVAR_VALID_PR  ,      //  8:
    SYSVAR_VALID_PW  ,      //  9:
    SYSVAR_VALID_RL  ,      // 10:
    SYSVAR_VALID_SA  ,      // 11:
    SYSVAR_VALID_WSID,      // 12:

    SYSVAR_VALID_LENGTH     // 13: Length of the enum *MUST* be last
};

// Use as in:  (*aSysVarValid[SYSVAR_VALID_IO]) (lpYYName, &lpYYExpr->tkToken);
BOOL (*aSysVarValid[SYSVAR_VALID_LENGTH]) (LPYYSTYPE, LPTOKEN);


//***************************************************************************
//  InitSystemVars
//
//  Initialize all system vars
//***************************************************************************

BOOL InitSystemVars
    (void)

{
    // Assign the validation routines
    aSysVarValid[SYSVAR_VALID_ALX ] = ValidateALX_EM ;
    aSysVarValid[SYSVAR_VALID_CT  ] = ValidateCT_EM  ;
    aSysVarValid[SYSVAR_VALID_DF  ] = ValidateDF_EM  ;
    aSysVarValid[SYSVAR_VALID_ELX ] = ValidateELX_EM ;
    aSysVarValid[SYSVAR_VALID_IF  ] = ValidateIF_EM  ;
    aSysVarValid[SYSVAR_VALID_IO  ] = ValidateIO_EM  ;
    aSysVarValid[SYSVAR_VALID_LX  ] = ValidateLX_EM  ;
    aSysVarValid[SYSVAR_VALID_PP  ] = ValidatePP_EM  ;
    aSysVarValid[SYSVAR_VALID_PR  ] = ValidatePR_EM  ;
    aSysVarValid[SYSVAR_VALID_PW  ] = ValidatePW_EM  ;
    aSysVarValid[SYSVAR_VALID_RL  ] = ValidateRL_EM  ;
    aSysVarValid[SYSVAR_VALID_SA  ] = ValidateSA_EM  ;
    aSysVarValid[SYSVAR_VALID_WSID] = ValidateWSID_EM;

    // Assign default values to the system vars
    AssignCharVector_EM(WS_UCS2_QUAD L"alx" , WS_UCS2_QUAD L"DM", SYSVAR_VALID_ALX , &hGlbQuadALX ); // Attention Latent Expression
    AssignRealScalar_EM(WS_UCS2_QUAD L"ct"  , 3E-15             , SYSVAR_VALID_CT  , &fQuadCT     ); // Comparison Tolerance
    AssignBoolScalar_EM(WS_UCS2_QUAD L"df"  , 1                 , SYSVAR_VALID_DF  , &bQuadDF     ); // Display Format
    AssignCharVector_EM(WS_UCS2_QUAD L"elx" , WS_UCS2_QUAD L"DM", SYSVAR_VALID_ELX , &hGlbQuadELX ); // Error Latent Expression
    AssignBoolScalar_EM(WS_UCS2_QUAD L"if"  , 1                 , SYSVAR_VALID_IF  , &bQuadIF     ); // Iota Format
    AssignBoolScalar_EM(WS_UCS2_QUAD L"io"  , 1                 , SYSVAR_VALID_IO  , &bQuadIO     ); // Index Origin
    AssignCharVector_EM(WS_UCS2_QUAD L"lx"  , L""               , SYSVAR_VALID_LX  , &hGlbQuadLX  ); // Latent Expression
    AssignIntScalar_EM (WS_UCS2_QUAD L"pp"  , 10                , SYSVAR_VALID_PP  , &uQuadPP     ); // Printing Precision
    AssignCharScalar_EM(WS_UCS2_QUAD L"pr"  , L' '              , SYSVAR_VALID_PR  , &cQuadPR     ); // Prompt Replacement
    AssignIntScalar_EM (WS_UCS2_QUAD L"pw"  , 80                , SYSVAR_VALID_PW  , &uQuadPW     ); // Printing Width
    AssignIntScalar_EM (WS_UCS2_QUAD L"rl"  , 16807             , SYSVAR_VALID_RL  , &uQuadRL     ); // Random Link
    AssignCharVector_EM(WS_UCS2_QUAD L"sa"  , L""               , SYSVAR_VALID_SA  , &hGlbQuadSA  ); // Stop Action
    AssignCharVector_EM(WS_UCS2_QUAD L"wsid", L""               , SYSVAR_VALID_WSID, &hGlbQuadWSID); // Workspace Identifier

    return TRUE;
} // End InitSystemVars


//***************************************************************************
//  End of File: sysvars.c
//***************************************************************************
