//***************************************************************************
//  NARS2000 -- System Function Routines
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>
#include <time.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "termcode.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

BOOL bUseLocalTime = TRUE;


//***************************************************************************
//  ExecuteFn0
//
//  Execute a niladic function
//***************************************************************************

LPYYSTYPE ExecuteFn0
    (LPYYSTYPE lpYYFcn0)

{
    // tkData is an LPSYMENTRY
    Assert (GetPtrTypeDir (lpYYFcn0->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

    // Call the execution routine
    return (lpYYFcn0->tkToken.tkData.lpSym->stData.stNameFcn) (NULL,
                                                              &lpYYFcn0->tkToken,
                                                               NULL,
                                                               NULL);
} // ExecuteFn0


//***************************************************************************
//  SysFnDR_EM
//
//  System function:  []DR -- Data Representation
//***************************************************************************

LPYYSTYPE SysFnDR_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonDR_EM (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydDR_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnDR_EM


//***************************************************************************
//  SysFnMonDR_EM
//
//  Monadic []DR -- Data Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonDR_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnMonDR_EM
    (LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    HGLOBAL hGlbData;
    LPVOID  lpMem;

    // Get new index into YYRes
    YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Fill in the result token
    YYRes[YYResIndex].tkToken.tkFlags.TknType   = TKT_VARIMMED;
    YYRes[YYResIndex].tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////YYRes[YYResIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYResIndex].tkToken.tkFlags.Color     =
////YYRes[YYResIndex].tkToken.tkData.tkInteger  =
    YYRes[YYResIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

#define DR_BOOL     11
#define DR_CHAR    162
#define DR_INT     643
#define DR_FLOAT   645
#define DR_HETERO  326
#define DR_NESTED  327
#define DR_LIST    328
#define DR_APA     649

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, it's an HGLOBAL
            if (!lptkRhtArg->tkData.lpSym->stFlags.Imm)
            {
                hGlbData = lptkRhtArg->tkData.lpSym->stData.stGlbData;

                break;      // Continue with HGLOBAL case
            } // End IF

            // Handle the immediate case

            // stData is an immediate
            Assert (lptkRhtArg->tkData.lpSym->stFlags.Imm);

            // Split cases based upon the token's immediate type
            switch (lptkRhtArg->tkData.lpSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    YYRes[YYResIndex].tkToken.tkData.tkInteger = DR_BOOL;

                    return &YYRes[YYResIndex];

                case IMMTYPE_INT:
                    YYRes[YYResIndex].tkToken.tkData.tkInteger = DR_INT;

                    return &YYRes[YYResIndex];

                case IMMTYPE_FLOAT:
                    YYRes[YYResIndex].tkToken.tkData.tkInteger = DR_FLOAT;

                    return &YYRes[YYResIndex];

                case IMMTYPE_CHAR:
                    YYRes[YYResIndex].tkToken.tkData.tkInteger = 162;

                    return &YYRes[YYResIndex];

                defstop
                    return NULL;
            } // End SWITCH

            DbgStop ();         // We should never get here

        case TKT_VARIMMED:
            // Split cases based upon the token's immediate type
            switch (lptkRhtArg->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    YYRes[YYResIndex].tkToken.tkData.tkInteger = DR_BOOL;

                    return &YYRes[YYResIndex];

                case IMMTYPE_INT:
                    YYRes[YYResIndex].tkToken.tkData.tkInteger = DR_INT;

                    return &YYRes[YYResIndex];

                case IMMTYPE_FLOAT:
                    YYRes[YYResIndex].tkToken.tkData.tkInteger = DR_FLOAT;

                    return &YYRes[YYResIndex];

                case IMMTYPE_CHAR:
                    YYRes[YYResIndex].tkToken.tkData.tkInteger = DR_CHAR;

                    return &YYRes[YYResIndex];

                defstop
                    return NULL;
            } // End SWITCH

            DbgStop ();         // We should never get here

        case TKT_LIST:
            YYRes[YYResIndex].tkToken.tkData.tkInteger = DR_LIST;

            return &YYRes[YYResIndex];

        case TKT_VARARRAY:
            hGlbData = lptkRhtArg->tkData.tkGlbData;

            break;      // Continue with HGLOBAL case

        defstop
            break;
    } // End SWITCH

    // HGLOBAL case
    // tk/stData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (hGlbData));

    // Clear the PTRTYPE_*** flags
    hGlbData = ClrPtrTypeDirGlb (hGlbData);

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    // Split cases based upon the array storage type
    switch (lpHeader->ArrType)
#undef  lpHeader
    {
        case ARRAY_BOOL:
            YYRes[YYResIndex].tkToken.tkData.tkInteger = DR_BOOL;

            break;

        case ARRAY_INT:
            YYRes[YYResIndex].tkToken.tkData.tkInteger = DR_INT;

            break;

        case ARRAY_FLOAT:
            YYRes[YYResIndex].tkToken.tkData.tkInteger = DR_FLOAT;

            break;

        case ARRAY_CHAR:
            YYRes[YYResIndex].tkToken.tkData.tkInteger = DR_CHAR;

            break;

        case ARRAY_HETERO:
            YYRes[YYResIndex].tkToken.tkData.tkInteger = DR_HETERO;

            break;

        case ARRAY_NESTED:
            YYRes[YYResIndex].tkToken.tkData.tkInteger = DR_NESTED;

            break;

        case ARRAY_APA:
            YYRes[YYResIndex].tkToken.tkData.tkInteger = DR_APA;

            break;

////////case ARRAY_LIST:            // Handled above with TKT_LIST
        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;

    return &YYRes[YYResIndex];
} // End SysFnMonDR_EM
#undef  APPEND_NAME


//***************************************************************************
//  SysFnDydDR_EM
//
//  Dyadic []DR -- Data Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydDR_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnDydDR_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // ***FINISHME***

    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End SysFnDydDR_EM
#undef  APPEND_NAME


//***************************************************************************
//  SysFnSYSID_EM
//
//  System function:  []SYSID -- System Identifier
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnSYSID_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnSYSID_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    UINT       ByteRes;
    HGLOBAL    hGlbRes;
    LPVOID     lpMem;

    // Get new index into YYRes
    YYResIndex = (YYResIndex + 1) % NUMYYRES;

#define SYSID   L"NARS2000"
#define SYSID_NELM    (sizeof (SYSID) / sizeof (APLCHAR) - 1)

    // Calculate size of the result
    ByteRes = (UINT) CalcArraySize (ARRAY_CHAR, SYSID_NELM, 1);
////ByteRes = sizeof (VARARRAY_HEADER)
////        + sizeof (APLDIM) * 1       // It's a vector
////        + sizeof (APLCHAR) * SYSID_NELM;  // with this many elements

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, ByteRes);

    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    // Fill in the header
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->Perm       = 0;
////lpHeader->SysVar     = 0;
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = SYSID_NELM;
    lpHeader->Rank       = 1;

#undef  lpHeader

    // Fill in the dimension
    *(VarArrayBaseToDim (lpMem)) = SYSID_NELM;

    // Skip over the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, 1);

    // Copy the SYSID to the result
    CopyMemory (lpMem, SYSID, SYSID_NELM * sizeof (APLCHAR));

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMem = NULL;

    // Fill in the result token
    YYRes[YYResIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
////YYRes[YYResIndex].tkToken.tkFlags.ImmType   = 0;
////YYRes[YYResIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYResIndex].tkToken.tkFlags.Color     =
    YYRes[YYResIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    YYRes[YYResIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return &YYRes[YYResIndex];
} // End SysFnSYSID_EM
#undef  APPEND_NAME


//***************************************************************************
//  SysFnSYSVER_EM
//
//  System function:  []SYSVER -- System Version
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnSYSVER_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnSYSVER_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    UINT       ByteRes;
    HGLOBAL    hGlbRes;
    LPVOID     lpMem;
    char       szFileVer[32];
    LPAPLCHAR  p;
    HANDLE     hFile;

    // Get new index into YYRes
    YYResIndex = (YYResIndex + 1) % NUMYYRES;

#define SYSVER  L"0.00.001.0799  Tue Jan 16 17:43:45 2007  Win/32"
#define SYSVER_NELM    ((sizeof (SYSVER) / sizeof (APLCHAR)) - 1)

    // Calculate size of the result
    ByteRes = (UINT) CalcArraySize (ARRAY_CHAR, SYSVER_NELM, 1);
////ByteRes = sizeof (VARARRAY_HEADER)
////        + sizeof (APLDIM) * 1       // It's a vector
////        + sizeof (APLCHAR) * SYSVER_NELM;  // with this many elements

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, ByteRes);

    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    // Fill in the header
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->Perm       = 0;
////lpHeader->SysVar     = 0;
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = SYSVER_NELM;
    lpHeader->Rank       = 1;

#undef  lpHeader

    // Fill in the dimension
    *(VarArrayBaseToDim (lpMem)) = SYSVER_NELM;

    // Skip over the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, 1);

    p = (LPAPLCHAR) lpMem;

    // Read in the application's File Version String
    LclFileVersionStr (szAppDPFE, szFileVer);

    A2W (p, szFileVer);

    // Skip to the trailing zero
    p += lstrlenW (p);
    *p++ = L' ';    // Blank separators
    *p++ = L' ';

    // Open the executable file so we can read its internal timestamp
    hFile = CreateFile (szAppDPFE,              // lpFileName
                        GENERIC_READ,           // dwDesiredAccess
                        FILE_SHARE_READ,        // dwShareMode
                        NULL,                   // lpSecurityAttributes
                        OPEN_EXISTING,          // dwCreationDistribution
                        FILE_ATTRIBUTE_NORMAL,  // dwFlagsAndAttributes
                        NULL);                  // hTemplateFile
    if (hFile)
    {
        DWORD              dwTemp,
                           dwCount;
        IMAGE_DOS_HEADER   idh;
        IMAGE_NT_HEADERS32 inth;

        // Set the file pointer to read the e_lfanew value
        SetFilePointer (hFile, ((LPBYTE) &idh.e_lfanew) - (LPBYTE) &idh, NULL, FILE_BEGIN);

        // Read in the e_lfanew value
        ReadFile (hFile, &dwTemp, sizeof (dwTemp), &dwCount, NULL);

        // Add in the distance to the file timestamp
        dwTemp += ((LPBYTE) (&inth.FileHeader.TimeDateStamp)) - (LPBYTE) &inth;

        // Set file pointer to the file timestamp
        SetFilePointer (hFile, dwTemp, NULL, FILE_BEGIN);

        // Read in the file timestamp
        ReadFile (hFile, &dwTemp, sizeof (dwTemp), &dwCount, NULL);

        A2W (p, ConvTime (dwTemp));

        // Skip to the trailing zero
        p += lstrlenW (p);
        *p++ = L' ';    // Blank separators
        *p++ = L' ';

#define SYSTYPE     L"Win/32"

        CopyMemory (p, SYSTYPE, sizeof (SYSTYPE) - 1 * sizeof (APLCHAR));

        // We no longer need this handle
        CloseHandle (hFile); hFile = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMem = NULL;

    // Fill in the result token
    YYRes[YYResIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
////YYRes[YYResIndex].tkToken.tkFlags.ImmType   = 0;
////YYRes[YYResIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYResIndex].tkToken.tkFlags.Color     =
    YYRes[YYResIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    YYRes[YYResIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return &YYRes[YYResIndex];
} // End SysFnSYSVER_EM
#undef  APPEND_NAME


//***************************************************************************
//  SysFnTC_EM
//
//  System function:  []TC -- Terminal Control
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnTC_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnTC_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    UINT       ByteRes;
    HGLOBAL    hGlbRes;
    LPVOID     lpMem;

    // Get new index into YYRes
    YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Calculate size of the result
    ByteRes = (UINT) CalcArraySize (ARRAY_CHAR, 3, 1);
////ByteRes = sizeof (VARARRAY_HEADER)
////        + sizeof (APLDIM) * 1       // It's a vector
////        + sizeof (APLCHAR) * 3;     // and a three-element one at that

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, ByteRes);

    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    // Fill in the header
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->Perm       = 0;
////lpHeader->SysVar     = 0;
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = 3;
    lpHeader->Rank       = 1;

#undef  lpHeader

    // Fill in the dimension
    *(VarArrayBaseToDim (lpMem)) = 3;

    // Skip over the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, 1);

#define lpMemData   ((LPAPLCHAR) lpMem)

    lpMemData[0] = TCBS;    // Backspace
    lpMemData[1] = TCNL;    // Newline
    lpMemData[2] = TCLF;    // Linefeed

#undef  lpMemData

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMem = NULL;

    // Fill in the result token
    YYRes[YYResIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
////YYRes[YYResIndex].tkToken.tkFlags.ImmType   = 0;
////YYRes[YYResIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYResIndex].tkToken.tkFlags.Color     =
    YYRes[YYResIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    YYRes[YYResIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return &YYRes[YYResIndex];
} // End SysFnTC_EM
#undef  APPEND_NAME


//***************************************************************************
//  SysFnTCCom
//
//  System function:  []TCxxx -- Terminal Control, Common Routine
//***************************************************************************

LPYYSTYPE SysFnTCCom
    (WCHAR   wc,
     LPTOKEN lptkFunc)

{
    // Get new index into YYRes
    YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Fill in the result token
    YYRes[YYResIndex].tkToken.tkFlags.TknType   = TKT_VARIMMED;
    YYRes[YYResIndex].tkToken.tkFlags.ImmType   = IMMTYPE_CHAR;
////YYRes[YYResIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYResIndex].tkToken.tkFlags.Color     =
    YYRes[YYResIndex].tkToken.tkData.tkChar     = wc;
    YYRes[YYResIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return &YYRes[YYResIndex];
} // End SysFnTCCom


//***************************************************************************
//  SysFnTCBEL_EM
//
//  System function:  []TCBEL -- Terminal Control, Bell
//***************************************************************************

LPYYSTYPE SysFnTCBEL_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    return SysFnTCCom (TCBEL, lptkFunc);
} // End SysFnTCBEL_EM


//***************************************************************************
//  SysFnTCBS_EM
//
//  System function:  []TCBS -- Terminal Control, Backspace
//***************************************************************************

LPYYSTYPE SysFnTCBS_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    return SysFnTCCom (TCBS, lptkFunc);
} // End SysFnTCBS_EM


//***************************************************************************
//  SysFnTCDEL_EM
//
//  System function:  []TCDEL -- Terminal Control, Del
//***************************************************************************

LPYYSTYPE SysFnTCDEL_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    return SysFnTCCom (TCDEL, lptkFunc);
} // End SysFnTCDEL_EM


//***************************************************************************
//  SysFnTCESC_EM
//
//  System function:  []TCESC -- Terminal Control, Escape
//***************************************************************************

LPYYSTYPE SysFnTCESC_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    return SysFnTCCom (TCESC, lptkFunc);
} // End SysFnTCESC_EM


//***************************************************************************
//  SysFnTCFF_EM
//
//  System function:  []TCFF -- Terminal Control, Form Feed
//***************************************************************************

LPYYSTYPE SysFnTCFF_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    return SysFnTCCom (TCFF, lptkFunc);
} // End SysFnTCFF_EM


//***************************************************************************
//  SysFnTCHT_EM
//
//  System function:  []TCHT -- Terminal Control, Horizontal Tab
//***************************************************************************

LPYYSTYPE SysFnTCHT_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    return SysFnTCCom (TCHT, lptkFunc);
} // End SysFnTCHT_EM


//***************************************************************************
//  SysFnTCLF_EM
//
//  System function:  []TCLF -- Terminal Control, Linefeed
//***************************************************************************

LPYYSTYPE SysFnTCLF_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    return SysFnTCCom (TCLF, lptkFunc);
} // End SysFnTCLF_EM


//***************************************************************************
//  SysFnTCNL_EM
//
//  System function:  []TCNL -- Terminal Control, Newline
//***************************************************************************

LPYYSTYPE SysFnTCNL_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    return SysFnTCCom (TCNL, lptkFunc);
} // End SysFnTCNL_EM


//***************************************************************************
//  SysFnTCNUL_EM
//
//  System function:  []TCNUL -- Terminal Control, Nul
//***************************************************************************

LPYYSTYPE SysFnTCNUL_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    return SysFnTCCom (TCNUL, lptkFunc);
} // End SysFnTCNUL_EM


//***************************************************************************
//  SysFnTS_EM
//
//  System function:  []TS -- Time Stamp
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnTS_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnTS_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    SYSTEMTIME SystemTime;
    UINT       ByteRes;
    HGLOBAL    hGlbRes;
    LPVOID     lpMem;

    // Get new index into YYRes
    YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Calculate size of the result
    ByteRes = (UINT) CalcArraySize (ARRAY_CHAR, 7, 1);
////ByteRes = sizeof (VARARRAY_HEADER)
////        + sizeof (APLDIM) * 1       // It's a vector
////        + sizeof (APLINT) * 7;      // and a seven-element one at that

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, ByteRes);

    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    // Fill in the header
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->Perm       = 0;
////lpHeader->SysVar     = 0;
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = 7;
    lpHeader->Rank       = 1;

#undef  lpHeader

    // Fill in the dimension
    *(VarArrayBaseToDim (lpMem)) = 7;

    // Skip over the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, 1);

    // Get the current time
    if (bUseLocalTime)
        GetLocalTime  (&SystemTime);
    else
        GetSystemTime (&SystemTime);

#define lpMemData   ((LPAPLINT) lpMem)

    lpMemData[0] = SystemTime.wYear;
    lpMemData[1] = SystemTime.wMonth;
    lpMemData[2] = SystemTime.wDay;
    lpMemData[3] = SystemTime.wHour;
    lpMemData[4] = SystemTime.wMinute;
    lpMemData[5] = SystemTime.wSecond;
    lpMemData[6] = SystemTime.wMilliseconds;

#undef  lpMemData

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMem = NULL;

    // Fill in the result token
    YYRes[YYResIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
////YYRes[YYResIndex].tkToken.tkFlags.ImmType   = 0;
////YYRes[YYResIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYResIndex].tkToken.tkFlags.Color     =
    YYRes[YYResIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    YYRes[YYResIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return &YYRes[YYResIndex];
} // End SysFnTS_EM
#undef  APPEND_NAME


//***************************************************************************
//  SysFnTYPE_EM
//
//  System function:  []TYPE -- Prototype
//***************************************************************************

LPYYSTYPE SysFnTYPE_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonTYPE_EM          (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydSyntaxError_EM  (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, NULL);
} // End SysFnTYPE_EM


//***************************************************************************
//  SysFnMonTYPE_EM
//
//  Monadic []TYPE -- Prototype
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonTYPE_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnMonTYPE_EM
    (LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    HGLOBAL hGlbData,
            hGlbRes;

    // Get new index into YYRes
    YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Split cases based upon the token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, it's an HGLOBAL
            if (!lptkRhtArg->tkData.lpSym->stFlags.Imm)
            {
                hGlbData = lptkRhtArg->tkData.lpSym->stData.stGlbData;

                break;      // Continue with HGLOBAL case
            } // End IF

            // Handle the immediate case

            // stData is an immediate
            Assert (lptkRhtArg->tkData.lpSym->stFlags.Imm);

            YYRes[YYResIndex].tkToken.tkFlags.TknType = TKT_VARIMMED;
            YYRes[YYResIndex].tkToken.tkFlags.ImmType = lptkRhtArg->tkData.lpSym->stFlags.ImmType;

            // Split cases based upon the token's immediate type
            switch (lptkRhtArg->tkData.lpSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    YYRes[YYResIndex].tkToken.tkData.tkBoolean = 0;

                    return &YYRes[YYResIndex];

                case IMMTYPE_INT:
                    YYRes[YYResIndex].tkToken.tkData.tkInteger = 0;

                    return &YYRes[YYResIndex];

                case IMMTYPE_FLOAT:
                    YYRes[YYResIndex].tkToken.tkData.tkFloat   = 0;

                    return &YYRes[YYResIndex];

                case IMMTYPE_CHAR:
                    YYRes[YYResIndex].tkToken.tkData.tkChar    = L' ';

                    return &YYRes[YYResIndex];

                defstop
                    return NULL;
            } // End SWITCH

            DbgStop ();         // We should never get here

        case TKT_VARIMMED:
            YYRes[YYResIndex].tkToken.tkFlags.TknType = TKT_VARIMMED;
            YYRes[YYResIndex].tkToken.tkFlags.ImmType = lptkRhtArg->tkFlags.ImmType;

            // Split cases based upon the token's immediate type
            switch (lptkRhtArg->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    YYRes[YYResIndex].tkToken.tkData.tkBoolean = 0;

                    return &YYRes[YYResIndex];

                case IMMTYPE_INT:
                    YYRes[YYResIndex].tkToken.tkData.tkInteger = 0;

                    return &YYRes[YYResIndex];

                case IMMTYPE_FLOAT:
                    YYRes[YYResIndex].tkToken.tkData.tkFloat   = 0;

                    return &YYRes[YYResIndex];

                case IMMTYPE_CHAR:
                    YYRes[YYResIndex].tkToken.tkData.tkChar    = L' ';

                    return &YYRes[YYResIndex];

                defstop
                    return NULL;
            } // End SWITCH

            DbgStop ();         // We should never get here

        case TKT_LIST:
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkFunc);
            return NULL;

        case TKT_VARARRAY:
            hGlbData = lptkRhtArg->tkData.tkGlbData;

            break;      // Continue with HGLOBAL case

        defstop
            break;
    } // End SWITCH

    // HGLOBAL case

    // tk/stData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (hGlbData));

    // Make the prototype
    hGlbRes = MakePrototype_EM (ClrPtrTypeDirGlb (hGlbData),
                                lptkFunc,
                                FALSE);   // Allow CHARs
    if (!hGlbRes)
        return NULL;

    // Fill in the result token
    YYRes[YYResIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
////YYRes[YYResIndex].tkToken.tkFlags.ImmType   = 0;
////YYRes[YYResIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYResIndex].tkToken.tkFlags.Color     =
    YYRes[YYResIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    YYRes[YYResIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return &YYRes[YYResIndex];
} // End SysFnMonTYPE_EM


//***************************************************************************
//  ConvTime
//
//  Convert to ASCII string in the form of
//    Wed Jan 02 02:03:55 1980
//***************************************************************************

LPSTR ConvTime
    (DWORD time)
{
    LPSTR p;

    p = asctime (gmtime ((const time_t *) &time));

    // Zap the "\n" so it prints on one line
    p [lstrlen (p) - 1] = '\0';

    return p;
} // End ConvTime ()


//***************************************************************************
//  End of File: sysfns.c
//***************************************************************************
