//***************************************************************************
//  NARS2000 -- System Function Routines
//***************************************************************************

#define STRICT
#include <windows.h>
#include <time.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "termcode.h"
#include "externs.h"
#include "dfnhdr.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

BOOL bUseLocalTime = TRUE;


//***************************************************************************
//  $ExecuteFn0
//
//  Execute a niladic function
//***************************************************************************

LPYYSTYPE ExecuteFn0
    (LPTOKEN lptkFcn0)          // Ptr to function token

{
    LPPRIMFNS lpNameFcn;

    // tkData is an LPSYMENTRY
    Assert (GetPtrTypeDir (lptkFcn0->tkData.tkVoid) EQ PTRTYPE_STCONST);

    lpNameFcn = lptkFcn0->tkData.tkSym->stData.stNameFcn;

    if (lptkFcn0->tkData.tkSym->stFlags.FcnDir)
        // Call the execution routine
        return (*lpNameFcn) (NULL,
                             lptkFcn0,
                             NULL,
                             NULL);
    else
        return ExecFuncGlb_EM_YY (NULL,                         // Ptr to left arg token
                                  ClrPtrTypeDirGlb (lpNameFcn), // Function HGLOBAL
                                  NULL);                        // Ptr to right arg token
} // ExecuteFn0


//***************************************************************************
//  $SysFnCR_EM
//
//  System function:  []CR -- Canonical Representation
//***************************************************************************

LPYYSTYPE SysFnCR_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonCR_EM (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydCR_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnCR_EM


//***************************************************************************
//  $SysFnMonCR_EM
//
//  Monadic []CR -- Canonical Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonCR_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnMonCR_EM
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE      aplTypeRht;       // Right arg storage type
    APLNELM       aplNELMRht,       // Right arg NELM
                  aplNELMRes;       // Result    ...
    APLRANK       aplRankRht;       // Right arg rank
    HGLOBAL       hGlbRht = NULL,   // Right arg global memory handle
                  hGlbRes = NULL,   // Result    ...
                  hGlbData,         // Data      ...
                  hGlbTxtLine;      // Line text ...
    LPVOID        lpMemRht = NULL,  // Ptr to right arg global memory
                  lpMemRes = NULL,  // Ptr to result    ...
                  lpMemData;        // Ptr to function data ...
    LPMEMTXTUNION lpMemTxtLine;     // Ptr to header/line text global memory
    UINT          uLineLen;         // Length of a text line
    APLLONGEST    aplLongestRht;    // Right arg longest if immediate
    LPSYMENTRY    lpSymEntry;       // Ptr to SYMENTRY
    STFLAGS       stFlags;          // STE flags
    LPYYSTYPE     lpYYRes = NULL;   // Ptr to the result
    LPAPLCHAR     lpw;              // Ptr to wide chars

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Check for RANK ERROR
    if (aplRankRht > 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Check for DOMAIN ERROR
    if (aplTypeRht NE ARRAY_CHAR)
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Clear the flags
    ZeroMemory (&stFlags, sizeof (stFlags));

    // Split cases based upon the right arg rank
    if (aplRankRht EQ 0)
        // Lookup the name in the symbol table
        // SymTabLookupName sets the .SysName or .UsrName flag,
        //   and the .Inuse flag
        lpSymEntry = SymTabLookupNameLength ((LPAPLCHAR) &aplLongestRht,
                                             (UINT) aplNELMRht,
                                            &stFlags);
    else
    {
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

        // Lookup the name in the symbol table
        // SymTabLookupName sets the .SysName or .UsrName flag,
        //   and the .Inuse flag
        lpSymEntry = SymTabLookupNameLength ((LPAPLCHAR) lpMemRht,
                                             (UINT) aplNELMRht,
                                            &stFlags);
    } // End IF/ELSE

    // If not found,
    //   or not a User Name,
    //   or not with a value,
    //   return 0 x 0 char matrix
    if (!lpSymEntry
     || !lpSymEntry->stFlags.UsrName
     || !lpSymEntry->stFlags.Value)
        // Not the signature of anything we know
        // Return a 0 x 0 char matric
        hGlbRes = SysFnMonCR_ALLOC_EM (0, 2, lpwszTemp, lptkFunc);
    else
    // If it's immediate, ...
    if (lpSymEntry->stFlags.Imm)
    {
        // Copy the function name
        lpw = CopyFcnName (lpwszTemp, lpSymEntry->stHshEntry->hGlbName);

        // Append a left arrow
        *lpw++ = UTF16_LEFTARROW;

        // Append the single char
        *lpw++ = lpSymEntry->stData.stChar;

        // Calculate the result NELM
        aplNELMRes = lpw - lpwszTemp;

        // Finish the job via subroutine
        hGlbRes = SysFnMonCR_ALLOC_EM (aplNELMRes, 1, lpwszTemp, lptkFunc);
    } else
    {
        // Check for internal functions
        if (lpSymEntry->stFlags.FcnDir)
        {
            // Append the function name from the symbol table
            lpw = CopyFcnName (lpwszTemp, lpSymEntry->stHshEntry->hGlbName);

            // Calculate the result NELM
            aplNELMRes = lpw - lpwszTemp;

            // Finish the job via subroutine
            hGlbRes = SysFnMonCR_ALLOC_EM (aplNELMRes, 1, lpwszTemp, lptkFunc);
        } else
        {
            // Get the global memory ptr
            hGlbData = lpSymEntry->stData.stGlbData;

            // Clear the ptr type bits
            hGlbData = ClrPtrTypeDirGlb (hGlbData);

            // Lock the memory to get a ptr to it
            lpMemData = MyGlobalLock (hGlbData);

            // Split cases based upon the array type
            switch (((LPHEADER_SIGNATURE) lpMemData)->nature)
            {
                case FCNARRAY_HEADER_SIGNATURE:
                    // Get the line text global memory handle
                    hGlbTxtLine = ((LPFCNARRAY_HEADER) lpMemData)->hGlbTxtLine;

                    // Lock the memory to get a ptr to it
                    lpMemTxtLine.V = MyGlobalLock (hGlbTxtLine);

                    // Get the line length
                    aplNELMRes = lstrlenW (lpMemTxtLine.C);

                    // Copy the function line text to global memory
                    CopyMemory (lpwszTemp, lpMemTxtLine.C, (UINT) aplNELMRes * sizeof (APLCHAR));

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine.V = NULL;

                    // Finish the job via subroutine
                    hGlbRes = SysFnMonCR_ALLOC_EM (aplNELMRes, 1, lpwszTemp, lptkFunc);

                    break;

                case DFN_HEADER_SIGNATURE:
                {
                    LPDFN_HEADER  lpMemDfnHdr;      // Ptr to defined function header ...
                    LPFCNLINE     lpFcnLines;       // Ptr to array of function line structs (FCNLINE[numFcnLines])
                    UINT          uNumLines,        // # function lines
                                  uLine,            // Loop counter
                                  uMaxLineLen;      // Length of the longest line
                    APLUINT       ByteRes;          // # bytes in the result

                    // Get ptr to defined function header
                    lpMemDfnHdr = (LPDFN_HEADER) lpMemData;

                    // Lock the memory to get a ptr to it
                    lpMemTxtLine.V = MyGlobalLock (lpMemDfnHdr->hGlbTxtHdr);

                    // Get the length of the function header text
                    uMaxLineLen = lpMemTxtLine.U[0];

                    // We no longer need this ptr
                    MyGlobalUnlock (lpMemDfnHdr->hGlbTxtHdr); lpMemTxtLine.V = NULL;

                    // Get ptr to array of function line structs (FCNLINE[numFcnLines])
                    lpFcnLines = (LPFCNLINE) &((LPBYTE) lpMemDfnHdr)[lpMemDfnHdr->offFcnLines];

                    // Get # function lines
                    uNumLines = lpMemDfnHdr->numFcnLines;

                    // Run through the function lines looking for the longest
                    for (uLine = 0; uLine < uNumLines; uLine++)
                    {
                        // Get the line text global memory handle
                        hGlbTxtLine = lpFcnLines->hGlbTxtLine;

                        if (hGlbTxtLine)
                        {
                            // Lock the memory to get a ptr to it
                            lpMemTxtLine.V = MyGlobalLock (hGlbTxtLine);

                            // Find the length of the longest line
                            uMaxLineLen = max (uMaxLineLen, lpMemTxtLine.U[0]);

                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine.V = NULL;
                        } // End IF

                        // Skip to the next struct
                        lpFcnLines++;
                    } // End FOR

                    // Calculate the result NELM ("1 +" includes the header)
                    aplNELMRes = (1 + uNumLines) * uMaxLineLen;

                    // Calculate the space needed for the result
                    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, 2);

                    // Allocate space for the result
                    // N.B.:  Conversion from APLUINT to UINT
                    Assert (ByteRes EQ (UINT) ByteRes);
                    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
                    if (!hGlbRes)
                        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                                   lptkFunc);
                    else
                    {
                        // Lock the memory to get a ptr to it
                        lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes     ((LPVARARRAY_HEADER) lpMemRes)

                        // Fill in the header
                        lpHeaderRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                        lpHeaderRes->ArrType    = ARRAY_CHAR;
////////////////////////lpHeaderRes->Perm       = 0;        // Already zero from GHND
////////////////////////lpHeaderRes->SysVar     = 0;        // Already zero from GHND
                        lpHeaderRes->RefCnt     = 1;
                        lpHeaderRes->NELM       = aplNELMRes;
                        lpHeaderRes->Rank       = 2;

#undef  lpHeaderRes

                        // Save the dimensions in the result ("1 +" includes the header)
                        (VarArrayBaseToDim (lpMemRes))[0] = 1 + uNumLines;
                        (VarArrayBaseToDim (lpMemRes))[1] = uMaxLineLen;

#define lpMemResChar    ((LPAPLCHAR) lpMemRes)

                        // Skip over the header and dimensions to the data
                        lpMemResChar = VarArrayBaseToData (lpMemRes, 2);

                        // Lock the memory to get a ptr to it
                        lpMemTxtLine.V = MyGlobalLock (lpMemDfnHdr->hGlbTxtHdr);

                        // Get the length of the function header text
                        uLineLen = *lpMemTxtLine.U;

                        // Copy the function header text to the result
                        CopyMemory (lpMemResChar, &lpMemTxtLine.U[1], uLineLen * sizeof (APLCHAR));

                        // We no longer need this ptr
                        MyGlobalUnlock (lpMemDfnHdr->hGlbTxtHdr); lpMemTxtLine.V = NULL;

                        // Fill the remainder of the line with blanks
                        for (lpMemResChar += uLineLen;
                             uLineLen < uMaxLineLen;
                             uLineLen++)
                            *lpMemResChar++ = L' ';

                        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
                        lpFcnLines = (LPFCNLINE) &((LPBYTE) lpMemDfnHdr)[lpMemDfnHdr->offFcnLines];

                        // Run through the function lines copying each line text to the result
                        for (uLine = 0; uLine < uNumLines; uLine++)
                        {
                            // Get the line text global memory handle
                            hGlbTxtLine = lpFcnLines->hGlbTxtLine;

                            if (hGlbTxtLine)
                            {
                                // Lock the memory to get a ptr to it
                                lpMemTxtLine.V = MyGlobalLock (hGlbTxtLine);

                                // Get the length of the function line text
                                uLineLen = *lpMemTxtLine.U;

                                // Copy the function header text to the result
                                CopyMemory (lpMemResChar, &lpMemTxtLine.U[1], uLineLen * sizeof (APLCHAR));

                                // We no longer need this ptr
                                MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine.V = NULL;

                                // Fill the remainder of the line with blanks
                                for (lpMemResChar += uLineLen;
                                     uLineLen < uMaxLineLen;
                                     uLineLen++)
                                    *lpMemResChar++ = L' ';
                            } // End IF
#undef  lpMemResChar
                            // Skip to the next struct
                            lpFcnLines++;
                        } // End FOR
                    } // End IF/ELSE

                    break;
                } // End DFN_HEADER_SIGNATURE

                case VARARRAY_HEADER_SIGNATURE: // Return a 0 x 0 char matrix
                    // Finish the job via subroutine
                    hGlbRes = SysFnMonCR_ALLOC_EM (0, 2, L"", lptkFunc);

                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbData); lpMemData = NULL;
        } // End IF/ELSE
    } // End IF/ELSE

    if (!hGlbRes)
        goto ERROR_EXIT;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
ERROR_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    // We no longer need this ptr
    if (hGlbRht && lpMemRht)
    {
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnMonCR_EM
#undef  APPEND_NAME


//***************************************************************************
//  SysFnMonCR_ALLOC_EM
//
//  Subroutine to SysFnMonCR_EM for allocating and copying to the result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonCR_ALLOC_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL SysFnMonCR_ALLOC_EM
    (APLNELM   aplNELMRes,      // Result NELM
     APLRANK   aplRankRes,      // ...    rank
     LPAPLCHAR lpw,             // Ptr to result text
     LPTOKEN   lptkFunc)        // Ptr to function token

{
    HGLOBAL    hGlbRes;         // Result global memory handle
    LPVOID     lpMemRes;        // Ptr to result    ...
    APLUINT    ByteRes;         // # bytes in the result

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, aplRankRes);

    // Allocate space for the result
    // N.B.:  Conversion from APLUINT to UINT
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes     ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeaderRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeaderRes->ArrType    = ARRAY_CHAR;
////lpHeaderRes->Perm       = 0;        // Already zero from GHND
////lpHeaderRes->SysVar     = 0;        // Already zero from GHND
    lpHeaderRes->RefCnt     = 1;
    lpHeaderRes->NELM       = aplNELMRes;
    lpHeaderRes->Rank       = aplRankRes;

#undef  lpHeaderRes

    // Save the dimension in the result
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimensions to the result
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    // Copy the function text to the result
    CopyMemory (lpMemRes, lpw, (UINT) aplNELMRes * sizeof (APLCHAR));

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    return hGlbRes;
} // End SysFnMonCR_ALLOC_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CopyFcnName
//
//  Copy a function name
//***************************************************************************

LPAPLCHAR CopyFcnName
    (LPAPLCHAR lpMemRes,        // Ptr to result global memory
     HGLOBAL   hGlbName)        // Function name global memory handle

{
    LPAPLCHAR lpMemName;        // Ptr to function name global memory

    // Lock the memory to get a ptr to it
    lpMemName = MyGlobalLock (hGlbName);

    // Copy the function name
    lstrcpyW (lpMemRes, lpMemName);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbName); lpMemName = NULL;

    // Point to the end
    return &lpMemRes[lstrlenW (lpMemRes)];
} // End CopyFcnName


//***************************************************************************
//  $SysFnDydCR_EM
//
//  Dyadic []CR -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydCR_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnDydCR_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token

{
    return PrimFnSyntaxError_EM (lptkFunc);
} // End SysFnDydCR_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDR_EM
//
//  System function:  []DR -- Data Representation
//***************************************************************************

LPYYSTYPE SysFnDR_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonDR_EM (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydDR_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnDR_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonDR_EM
//
//  Monadic []DR -- Display the Data Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonDR_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnMonDR_EM
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    HGLOBAL hGlbData;
    LPVOID  lpMem;
    LPYYSTYPE lpYYRes;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkData.tkInteger  =   (filled in below)
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

#define DR_SHOW             0   // Return a character vector representation
#define DR_BOOL           100   //    1 bit  per value
#define DR_CHAR          1601   //   16 bits ...
#define DR_INT           6402   //   64 ...
#define DR_FLOAT         6403   //   64 ...
#define DR_APA           6404   //   64 ...
#define DR_HETERO        3205   //   32 ...
#define DR_NESTED        3206   //   32 ...
#define DR_LIST          3207   //   32 ...
#define DR_RATIONAL         8   //   ?? ...
#define DR_COMPLEX      12809   //  128 ...
#define DR_QUATERNIONS  25610   //  256 ...
#define DR_OCTONIONS    51211   //  512 ...

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, it's an HGLOBAL
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                break;      // Continue with HGLOBAL case
            } // End IF

            // Handle the immediate case

            // stData is an immediate
            Assert (lptkRhtArg->tkData.tkSym->stFlags.Imm);

            // Split cases based upon the token's immediate type
            switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    lpYYRes->tkToken.tkData.tkInteger = DR_BOOL;

                    return lpYYRes;

                case IMMTYPE_INT:
                    lpYYRes->tkToken.tkData.tkInteger = DR_INT;

                    return lpYYRes;

                case IMMTYPE_FLOAT:
                    lpYYRes->tkToken.tkData.tkInteger = DR_FLOAT;

                    return lpYYRes;

                case IMMTYPE_CHAR:
                    lpYYRes->tkToken.tkData.tkInteger = DR_CHAR;

                    return lpYYRes;

                defstop
                    return NULL;
            } // End SWITCH

            DbgStop ();         // We should never get here

        case TKT_VARIMMED:
            // Split cases based upon the token's immediate type
            switch (lptkRhtArg->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    lpYYRes->tkToken.tkData.tkInteger = DR_BOOL;

                    return lpYYRes;

                case IMMTYPE_INT:
                    lpYYRes->tkToken.tkData.tkInteger = DR_INT;

                    return lpYYRes;

                case IMMTYPE_FLOAT:
                    lpYYRes->tkToken.tkData.tkInteger = DR_FLOAT;

                    return lpYYRes;

                case IMMTYPE_CHAR:
                    lpYYRes->tkToken.tkData.tkInteger = DR_CHAR;

                    return lpYYRes;

                defstop
                    return NULL;
            } // End SWITCH

            DbgStop ();         // We should never get here

        case TKT_LISTPAR:
            lpYYRes->tkToken.tkData.tkInteger = DR_LIST;

            return lpYYRes;

        case TKT_VARARRAY:
            hGlbData = lptkRhtArg->tkData.tkGlbData;

            break;      // Continue with HGLOBAL case

        defstop
            break;
    } // End SWITCH

    // HGLOBAL case
    // tk/stData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (hGlbData));

    // Clear the ptr type bits
    hGlbData = ClrPtrTypeDirGlb (hGlbData);

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    // Split cases based upon the array storage type
    switch (lpHeader->ArrType)
#undef  lpHeader
    {
        case ARRAY_BOOL:
            lpYYRes->tkToken.tkData.tkInteger = DR_BOOL;

            break;

        case ARRAY_INT:
            lpYYRes->tkToken.tkData.tkInteger = DR_INT;

            break;

        case ARRAY_FLOAT:
            lpYYRes->tkToken.tkData.tkInteger = DR_FLOAT;

            break;

        case ARRAY_CHAR:
            lpYYRes->tkToken.tkData.tkInteger = DR_CHAR;

            break;

        case ARRAY_HETERO:
            lpYYRes->tkToken.tkData.tkInteger = DR_HETERO;

            break;

        case ARRAY_NESTED:
            lpYYRes->tkToken.tkData.tkInteger = DR_NESTED;

            break;

        case ARRAY_APA:
            lpYYRes->tkToken.tkData.tkInteger = DR_APA;

            break;

////////case ARRAY_LIST:            // Handled above with TKT_LIST
        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;

    return lpYYRes;
} // End SysFnMonDR_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydDR_EM
//
//  Dyadic []DR -- Convert To A Data Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydDR_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnDydDR_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE  aplTypeLft,
              aplTypeRht;
    APLNELM   aplNELMLft,
              aplNELMRht;
    APLRANK   aplRankLft,
              aplRankRht;
    HGLOBAL   hGlbRes = NULL;
    APLINT    aplIntegerLft;
    APLFLOAT  aplFloatLft;
    LPYYSTYPE lpYYRes;

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Check for LEFT RANK ERROR
    if (aplRankLft > 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Check for LEFT LENGTH ERROR
    if (aplNELMLft NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Check for LEFT DOMAIN ERROR
    if (!IsSimpleNum (aplTypeLft))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Get the first (and only) value
    FirstValue (lptkLftArg,         // Ptr to left arg token
               &aplIntegerLft,      // Ptr to integer result
               &aplFloatLft,        // Ptr to float ...
                NULL,               // Ptr to WCHAR ...
                NULL,               // Ptr to longest ...
                NULL,               // Ptr to lpSym/Glb ...
                NULL,               // Ptr to ...immediate type ...
                NULL);              // Ptr to array type ...

    // If it's a float, attempt to convert it to integer
    if (aplTypeLft EQ ARRAY_FLOAT)
    {
        BOOL bRet;

        aplIntegerLft = FloatToAplint_SCT (aplFloatLft, &bRet);
        if (!bRet)
        {
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                       lptkFunc);
            return NULL;
        } // End IF
    } // End IF

    // Ensure the left arg is valid
    switch (aplIntegerLft)
    {
        case DR_SHOW:
            // Return a character representation
            return SysFnDydDR_SHOW_EM (aplTypeRht, lptkFunc);

        case DR_BOOL:
        case DR_CHAR:
        case DR_INT:
        case DR_FLOAT:
            ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                       lptkFunc);
            return NULL;

            DbgBrk ();      // ***FINISHME***





            break;

        case DR_APA:
        case DR_HETERO:
        case DR_NESTED:
        case DR_LIST:
        case DR_RATIONAL:
        case DR_COMPLEX:
        case DR_QUATERNIONS:
        case DR_OCTONIONS:
            // ***DEBUG***
            hGlbRes = DR_FloatToChar_EM (lptkRhtArg, lptkFunc);

            break;

        default:
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                       lptkFunc);
            return NULL;
    } // End SWITCH

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnDydDR_EM
#undef  APPEND_NAME


//***************************************************************************
//  SysFnDydDR_SHOW_EM
//
//  Return a character representation of the storage type
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydDR_SHOW_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnDydDR_SHOW_EM
    (APLSTYPE aplTypeRht,
     LPTOKEN  lptkFunc)

{
    LPAPLCHAR lpw;          // Ptr to WCHAR text
    APLUINT   ByteRes;      // # bytes in the result
    HGLOBAL   hGlbRes;      // Result global memory handle
    LPVOID    lpMemRes;     // Ptr to result global memory
    LPYYSTYPE lpYYRes;      // Ptr to the result
    APLNELM   aplNELMRes;   // Result NELM

    // Split cases based upon the rigth arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            lpw = L"Boolean:  1 bit per element";

            break;

        case ARRAY_INT:
            lpw = L"Integer:  64 bits per element";

            break;

        case ARRAY_FLOAT:
            lpw = L"Floating Point:  64 bits per element";

            break;

        case ARRAY_CHAR:
            lpw = L"Character:  16 bits per element";

            break;

        case ARRAY_APA:
            lpw = L"Arithmetic Progression Array:  192 bits total";

            break;

        case ARRAY_LIST:
            lpw = L"List:  32 bits per element";

            break;

        case ARRAY_NESTED:
            lpw = L"Nested Array:  32 bits per element";

            break;

        case ARRAY_HETERO:
            lpw = L"Heterogeneous Array:  32 bits per element";

            break;

        defstop
            break;
    } // End SWITCH

    // Get the result NELM
    aplNELMRes = lstrlenW (lpw);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, 1);

    // Allocate space for the result
    // N.B.:  Conversion from APLUINT to UINT
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes     ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeaderRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeaderRes->ArrType    = ARRAY_CHAR;
////lpHeaderRes->Perm       = 0;        // Already zero from GHND
////lpHeaderRes->SysVar     = 0;        // Already zero from GHND
    lpHeaderRes->RefCnt     = 1;
    lpHeaderRes->NELM       = aplNELMRes;
    lpHeaderRes->Rank       = 1;

#undef  lpHeaderRes

    // Save the dimension in the result
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Point to the data (APLAPA struct)
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // Copy the text to the result
    CopyMemory (lpMemRes, lpw, (UINT) aplNELMRes * sizeof (APLCHAR));

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnDydDR_SHOW_EM
#undef  APPEND_NAME


//***************************************************************************
//  $DR_FloatToChar_EM
//
//  QuadDR subroutine to convert FP to WCHAR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- DR_FloatToChar_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL DR_FloatToChar_EM
    (LPTOKEN lptkRhtArg,
     LPTOKEN lptkFunc)

{
    APLSTYPE aplTypeRht;        // Right arg storage type
    APLNELM  aplNELMRht;        // Right arg NELM
    APLRANK  aplRankRht;        // Right arg rank
    APLUINT  ByteRes,           // # bytes needed for the result
             uRes;              // Result loop counter
    HGLOBAL  hGlbRht,           // Right arg global memory handle
             hGlbRes;           // Result    ...
    LPVOID   lpMemRht = NULL,   // Ptr to right arg global memory
             lpMemRes = NULL;   // Ptr to result    ...
    APLFLOAT aplFloatRht;       // Temporary float

    // Get the attributes (Type, NELM, and Rank)
    //   of the right args
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Ensure the right arg is float
    if (aplTypeRht NE ARRAY_FLOAT)
    {
        ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Convert the FP argument to displayable chars

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRht * 16, aplRankRht + 1);

    // Allocate space for the result.
    // N.B. Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->Perm       = 0;   // Already zero from GHND
////lpHeader->SysVar     = 0;   // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht * 16;
    lpHeader->Rank       = aplRankRht + 1;

#undef  lpHeader

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);
    if (lpMemRht)
    {
        lpMemRht = VarArrayBaseToDim (lpMemRht);

        //***************************************************************
        // Copy the dimensions from the right arg
        //   to the result's dimension
        //***************************************************************
        for (uRes = 0; uRes < aplRankRht; uRes++)
            *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;
    } // End IF

    // The last dimension is 16
    *((LPAPLDIM) lpMemRes)++ = 16;

    // lpMemRes now points to the result's data

    // If the right arg is not an immediate, ...
    if (lpMemRht)
    {
        // Loop through the right arg converting it to the result
        for (uRes = 0; uRes < aplNELMRht; uRes++, ((LPAPLCHAR) lpMemRes += 16))
            FloatToAplchar (*((LPAPLFLOAT) lpMemRht)++, (LPAPLCHAR) lpMemRes);
    } else
    // The right arg is an immediate
    {
        // Get the first (and only) value
        FirstValue (lptkRhtArg,         // Ptr to right arg token
                    NULL,               // Ptr to integer result
                   &aplFloatRht,        // Ptr to float ...
                    NULL,               // Ptr to WCHAR ...
                    NULL,               // Ptr to longest ...
                    NULL,               // Ptr to lpSym/Glb ...
                    NULL,               // Ptr to ...immediate type ...
                    NULL);              // Ptr to array type ...
        FloatToAplchar (aplFloatRht, (LPAPLCHAR) lpMemRes);
    } // End IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // We no longer need this ptr
    if (lpMemRht)
    {
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return hGlbRes;
} // End DR_FloatToChar_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CalcNumIDs
//
//  Calculate the # identifiers in an arg
//***************************************************************************

void CalcNumIDs
    (APLNELM    aplNELMRht,         // Right arg NELM
     APLRANK    aplRankRht,         // Right arg rank
     APLLONGEST aplLongestRht,      // Right arg longest
     LPAPLCHAR  lpMemRht,           // Ptr to right arg global memory
     LPAPLNELM  lpaplNELMRes,       // Ptr to # right arg IDs
     LPAPLNELM  lpaplNELMCol)       // Ptr to # right arg cols (matrix only)

{
    APLUINT uRht;               // Loop counter

    // Split cases based upon the right arg rank
    switch (aplRankRht)
    {
        case 0:
            *lpaplNELMRes = (L' ' NE (APLCHAR) aplLongestRht);

            break;

        case 1:
            *lpaplNELMRes = 0;      // Initialize

            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

            // Loop throught the right arg looking for identifiers
            uRht = 0;
            while (TRUE)
            {
                // Skip over white space
                while (uRht < aplNELMRht && lpMemRht[uRht] EQ L' ')
                    uRht++;
                if (uRht < aplNELMRht)
                {
                    // Count in another element in the result
                    (*lpaplNELMRes)++;

                    // Skip over black space
                    while (uRht < aplNELMRht && lpMemRht[uRht] NE L' ')
                        uRht++;
                } else
                    break;
            } // End FOR

            break;

        case 2:
            *lpaplNELMRes = (VarArrayBaseToDim (lpMemRht))[0];
            *lpaplNELMCol = (VarArrayBaseToDim (lpMemRht))[1];

            break;

        defstop
            break;
    } // End IF
} // End CalcNumIDs


//***************************************************************************
//  $SysFnSIZE_EM
//
//  System function:  []SIZE -- Size of an identifier
//***************************************************************************

LPYYSTYPE SysFnSIZE_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonSIZE_EM (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydSIZE_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnSIZE_EM


//***************************************************************************
//  $SysFnMonSIZE_EM
//
//  Monadic []SIZE -- Size of an identifier
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonSIZE_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnMonSIZE_EM
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token

{
    APLSTYPE   aplTypeRht;      // Right arg storage type
    APLNELM    aplNELMRht,      // Right arg NELM
               aplNELMRes,      // Result    ...
               aplNELMCol;      // Result column NELM
    APLRANK    aplRankRht;      // Right arg Rank
    APLLONGEST aplLongestRht;   // Right arg longest if immediate
    HGLOBAL    hGlbRht = NULL,  // Right arg global memory handle
               hGlbRes = NULL;  // Result    ...
    LPVOID     lpMemRht = NULL, // Ptr to right arg global memory
               lpMemRes = NULL; // Ptr to result    ...
    LPAPLCHAR  lpMemDataRht,    // Ptr to right arg char data
               lpMemDataStart;  // Ptr to start of identifier
    LPAPLINT   lpMemDataRes;    // Ptr to result integer data
    APLUINT    uRht,            // Loop counter
               uCol,            // ...
               ByteRes;         // # bytes in the result
    LPSYMENTRY lpSymEntry;      // Ptr to SYMENTRY
    STFLAGS    stFlags;         // STE flags
    LPYYSTYPE  lpYYRes = NULL;  // Ptr to the result

    // The right arg may be of two forms:
    //   1.  a vector of names as in 'a b c'
    //   2.  a matrix of names as in 3 1{rho}'abc'

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Check for RANK ERROR
    if (aplRankRht > 2)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Check for DOMAIN ERROR
    if (!IsSimple (aplTypeRht)
     || (aplTypeRht NE ARRAY_CHAR
      && aplNELMRht NE 0))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Calculate the # identifiers in the argument
    //   allowing for vector and matrix with multiple names
    CalcNumIDs (aplNELMRht,         // Right arg NELM
                aplRankRht,         // Right arg rank
                aplLongestRht,      // Right arg longest
                lpMemRht,           // Ptr to right arg global memory
               &aplNELMRes,         // Ptr to # right arg IDs
               &aplNELMCol);        // Ptr to # right arg cols (matrix only)
    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_INT, aplNELMRes, 1);

    // Allocate space for the result
    // N.B. Conversion from APLUINT to UINT
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->Perm       = 0;           // Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;

#undef  lpHeader

    // Fill in the dimension
    *(VarArrayBaseToDim (lpMemRes)) = aplNELMRes;

    // Skip over the header and dimensions to the data
    lpMemDataRes = VarArrayBaseToData (lpMemRes, 1);

    // Calculate the size of each element

    // Split cases based upon the right arg rank
    switch (aplRankRht)
    {
        case 0:
            // Lookup the name in the symbol table
            // SymTabLookupName sets the .SysName or .UsrName flag,
            //   and the .Inuse flag
            ZeroMemory (&stFlags, sizeof (stFlags));
            lpSymEntry = SymTabLookupNameLength ((LPAPLCHAR) &aplLongestRht,
                                                 1,
                                                &stFlags);
            // If not found, return zero
            if (!lpSymEntry)
                *lpMemDataRes++ = 0;
            else
                *lpMemDataRes++ = CalcSymentrySize (lpSymEntry);
            break;

        case 1:
            // Skip over the header and dimensions to the data
            lpMemDataRht = VarArrayBaseToData (lpMemRht, aplRankRht);

            // Loop throught the right arg looking for identifiers
            uRht = 0;
            while (TRUE)
            {
                // Skip over white space
                while (uRht < aplNELMRht && lpMemDataRht[uRht] EQ L' ')
                    uRht++;
                if (uRht < aplNELMRht)
                {
                    // Save the starting ptr
                    lpMemDataStart = &lpMemDataRht[uRht];

                    // Skip over black space
                    while (uRht < aplNELMRht && lpMemDataRht[uRht] NE L' ')
                        uRht++;
                    // Lookup the name in the symbol table
                    // SymTabLookupName sets the .SysName or .UsrName flag,
                    //   and the .Inuse flag
                    ZeroMemory (&stFlags, sizeof (stFlags));
                    lpSymEntry = SymTabLookupNameLength (lpMemDataStart,
                                                        &lpMemDataRht[uRht] - lpMemDataStart,
                                                        &stFlags);
                    // If not found, return 0
                    if (!lpSymEntry)
                        *lpMemDataRes++ = 0;
                    else
                        *lpMemDataRes++ = CalcSymentrySize (lpSymEntry);
                } else
                    break;
            } // End FOR

            break;

        case 2:
            // Skip over the header and dimensions to the data
            lpMemDataRht = VarArrayBaseToData (lpMemRht, aplRankRht);

            for (uRht = 0; uRht < aplNELMRes; uRht++)
            {
                // Point to the start of the data
                lpMemDataStart = &lpMemDataRht[aplNELMCol * uRht];

                // Skip over leading white space
                uCol = 0;
                while (uCol < aplNELMCol && lpMemDataStart[uCol] EQ L' ')
                    uCol++;

                // Lookup the name in the symbol table
                // SymTabLookupName sets the .SysName or .UsrName flag,
                //   and the .Inuse flag
                ZeroMemory (&stFlags, sizeof (stFlags));
                lpSymEntry = SymTabLookupNameLength (&lpMemDataStart[uCol],
                                                      (UINT) (aplNELMCol - uCol),
                                                     &stFlags);
                // If not found, return 0
                if (!lpSymEntry)
                    *lpMemDataRes++ = 0;
                else
                    *lpMemDataRes++ = CalcSymentrySize (lpSymEntry);
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

ERROR_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbRes)
    {
        // We no longer need this storage
        DbgGlobalFree (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    // We no longer need this ptr
    if (hGlbRht && lpMemRht)
    {
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnMonSize_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydSIZE_EM
//
//  Dyadic []SIZE -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydSIZE_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnDydSIZE_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token

{
    return PrimFnSyntaxError_EM (lptkFunc);
} // End SysFnDydSIZE_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CalcSymentrySize
//
//  Calculate the size of a SYMENTRY
//***************************************************************************

APLINT CalcSymentrySize
    (LPSYMENTRY lpSymEntry)

{
    APLUINT aplSize = 0;        // The result

    // If it's an immediate (any type) or an internal function, ...
    if (lpSymEntry->stFlags.Imm
     || lpSymEntry->stFlags.FcnDir)
        aplSize = sizeof (SYMENTRY);
    else
    // If it has no value, ...
    if (!lpSymEntry->stFlags.Value)
        aplSize = 0;
    else
    // If it is a user variable, ...
    if (lpSymEntry->stFlags.UsrType EQ NAMETYPE_VAR)
    {
        // Start with the size of the SYMENTRY
        aplSize = sizeof (SYMENTRY);

        // Recurse through the array returning the total size
        aplSize += CalcGlbSize (lpSymEntry->stData.stGlbData);
    } else
    // If it is a user function/operator, ...
    if (IsNameTypeFnOp (lpSymEntry->stFlags.UsrType))
    {
        HGLOBAL      hGlbDfnHdr;        // Defined function header global memory handle
        LPDFN_HEADER lpMemDfnHdr;       // Ptr to defined function header ...
        LPFCNLINE    lpFcnLines;        // Ptr to array of function line structs (FCNLINE[numFcnLines])
        UINT         uNumLines,         // # function lines
                     uLine;             // Loop counter

        // Get the global memory handle
        hGlbDfnHdr = lpSymEntry->stData.stGlbData;

        // stData is a valid HGLOBAL function array or defined function
        Assert (IsGlbTypeFcnDir (hGlbDfnHdr)
             || IsGlbTypeDfnDir (hGlbDfnHdr));

        // Clear the ptr type bits
        hGlbDfnHdr = ClrPtrTypeDirGlb (hGlbDfnHdr);

        // Split cases based upon the defined function bit
        if (lpSymEntry->stFlags.UsrDfn)
        {
            // Lock the memory to get a ptr to it
            lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

            // Start with the size of the DFN_HEADER
            aplSize =   sizeof (DFN_HEADER)
                      + sizeof (LPSYMENTRY) * (lpMemDfnHdr->numResultSTE
                                             + lpMemDfnHdr->numLftArgSTE
                                             + lpMemDfnHdr->numRhtArgSTE
                                             + lpMemDfnHdr->numLocalsSTE)
                      + sizeof (FCNLINE) * lpMemDfnHdr->numFcnLines;
            // Get ptr to array of function line structs (FCNLINE[numFcnLines])
            lpFcnLines = (LPFCNLINE) &((LPBYTE) lpMemDfnHdr)[lpMemDfnHdr->offFcnLines];

            // Get # function lines
            uNumLines = lpMemDfnHdr->numFcnLines;

            // Loop through the function lines
            for (uLine = 0; uLine < uNumLines; uLine++)
            {
                if (lpFcnLines->hGlbTxtLine)
                    aplSize += MyGlobalSize (lpFcnLines->hGlbTxtLine);

                if (lpFcnLines->hGlbTknLine)
                    aplSize += MyGlobalSize (lpFcnLines->hGlbTknLine);

                if (lpFcnLines->hGlbMonInfo)
                    aplSize += MyGlobalSize (lpFcnLines->hGlbMonInfo);

                // Skip to the next struct
                lpFcnLines++;
            } // End FOR

            // Add in the size of the function header text
            if (lpMemDfnHdr->hGlbTxtHdr)
                aplSize += MyGlobalSize (lpMemDfnHdr->hGlbTxtHdr);

            // Add in the size of the function header tokenized
            if (lpMemDfnHdr->hGlbTknHdr)
                aplSize += MyGlobalSize (lpMemDfnHdr->hGlbTknHdr);

            // Add in the size of the function Undo buffer
            if (lpMemDfnHdr->hGlbUndoBuff)
                aplSize += MyGlobalSize (lpMemDfnHdr->hGlbUndoBuff);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
        } else
        // Otherwise, it's a function array
            // Start with the size of the SYMENTRY
            aplSize = sizeof (SYMENTRY)
                    + MyGlobalSize (ClrPtrTypeDirGlb (lpSymEntry->stData.stGlbData));
    } else
    // Otherwise, its size is zero
        aplSize = 0;

    return aplSize;
} // End CalcSymentrySize


//***************************************************************************
//  $CalcGlbSize
//
//  Calculate the size of a global memory variable
//***************************************************************************

APLUINT CalcGlbSize
    (HGLOBAL hGlbData)

{
    APLUINT     aplSize = 0;        // The result
    LPAPLNESTED lpMemData;          // Ptr to the global memory
    APLSTYPE    aplType;            // Data storage type
    APLNELM     aplNELM;            // Data NELM
    APLRANK     aplRank;            // Data rank
    APLUINT     uData;              // Loop counter

    // stData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (hGlbData));

    hGlbData = ClrPtrTypeDirGlb (hGlbData);

    aplSize += MyGlobalSize (hGlbData);

    // Lock the memory to get a ptr to it
    lpMemData = MyGlobalLock (hGlbData);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemData)

    // Get the Array Type, NELM, and Rank
    aplType = lpHeader->ArrType;
    aplNELM = lpHeader->NELM;
    aplRank = lpHeader->Rank;

#undef  lpHeader

    // If the array is simple, that's all
    if (IsSimple (aplType))
        goto NORMAL_EXIT;

    // Skip over the header and dimensions to the data
    lpMemData = VarArrayBaseToData (lpMemData, aplRank);

    // Loop through the array adding the sizes
    for (uData = 0; uData < aplNELM; uData++)
    // Split cases based upon the pointer type
    switch (GetPtrTypeDir (lpMemData[uData]))
    {
        case PTRTYPE_STCONST:
            aplSize += sizeof (SYMENTRY);

            break;

        case PTRTYPE_HGLOBAL:
            aplSize += CalcGlbSize (lpMemData[uData]);

            break;

        defstop
            break;
    } // End FOR/SWITCH
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMemData = NULL;

    return aplSize;
} // End CalcGlbSize


//***************************************************************************
//  $SysFnSYSID_EM
//
//  System function:  []SYSID -- System Identifier
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnSYSID_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnSYSID_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token

{
    UINT       ByteRes;
    HGLOBAL    hGlbRes;
    LPVOID     lpMem;
    LPYYSTYPE lpYYRes;

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

#define SYSID   L"NARS2000"
#define SYSID_NELM    (sizeof (SYSID) / sizeof (APLCHAR) - 1)

    // Calculate space needed for the result
    ByteRes = (UINT) CalcArraySize (ARRAY_CHAR, SYSID_NELM, 1);

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
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->Perm       = 0;           // Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
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

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnSYSID_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnSYSVER_EM
//
//  System function:  []SYSVER -- System Version
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnSYSVER_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnSYSVER_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    UINT       ByteRes;
    HGLOBAL    hGlbRes;
    LPVOID     lpMem;
    char       szFileVer[32];
    LPAPLCHAR  p;
    HANDLE     hFile;
    LPYYSTYPE  lpYYRes;

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

#define SYSVER  L"0.00.001.0799  Tue Jan 16 17:43:45 2007  Win/32"
#define SYSVER_NELM    ((sizeof (SYSVER) / sizeof (APLCHAR)) - 1)

    // Calculate space needed for the result
    ByteRes = (UINT) CalcArraySize (ARRAY_CHAR, SYSVER_NELM, 1);

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
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->Perm       = 0;           // Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
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

    A2W (p, szFileVer, SYSVER_NELM);

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

        A2W (p, ConvTime (dwTemp), SYSVER_NELM);

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

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnSYSVER_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnTC_EM
//
//  System function:  []TC -- Terminal Control
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnTC_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnTC_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    UINT       ByteRes;
    HGLOBAL    hGlbRes;
    LPVOID     lpMem;
    LPYYSTYPE lpYYRes;

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    // Calculate space needed for the result
    ByteRes = (UINT) CalcArraySize (ARRAY_CHAR, 3, 1);

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
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->Perm       = 0;           // Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
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

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnTC_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnTCCom
//
//  System function:  []TCxxx -- Terminal Control, Common Routine
//***************************************************************************

LPYYSTYPE SysFnTCCom
    (WCHAR   wc,
     LPTOKEN lptkFunc)

{
    LPYYSTYPE lpYYRes;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_CHAR;
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkChar     = wc;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnTCCom


//***************************************************************************
//  $SysFnTCBEL_EM
//
//  System function:  []TCBEL -- Terminal Control, Bell
//***************************************************************************

LPYYSTYPE SysFnTCBEL_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    return SysFnTCCom (TCBEL, lptkFunc);
} // End SysFnTCBEL_EM


//***************************************************************************
//  $SysFnTCBS_EM
//
//  System function:  []TCBS -- Terminal Control, Backspace
//***************************************************************************

LPYYSTYPE SysFnTCBS_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    return SysFnTCCom (TCBS, lptkFunc);
} // End SysFnTCBS_EM


//***************************************************************************
//  $SysFnTCDEL_EM
//
//  System function:  []TCDEL -- Terminal Control, Del
//***************************************************************************

LPYYSTYPE SysFnTCDEL_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    return SysFnTCCom (TCDEL, lptkFunc);
} // End SysFnTCDEL_EM


//***************************************************************************
//  $SysFnTCESC_EM
//
//  System function:  []TCESC -- Terminal Control, Escape
//***************************************************************************

LPYYSTYPE SysFnTCESC_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    return SysFnTCCom (TCESC, lptkFunc);
} // End SysFnTCESC_EM


//***************************************************************************
//  $SysFnTCFF_EM
//
//  System function:  []TCFF -- Terminal Control, Form Feed
//***************************************************************************

LPYYSTYPE SysFnTCFF_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    return SysFnTCCom (TCFF, lptkFunc);
} // End SysFnTCFF_EM


//***************************************************************************
//  $SysFnTCHT_EM
//
//  System function:  []TCHT -- Terminal Control, Horizontal Tab
//***************************************************************************

LPYYSTYPE SysFnTCHT_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    return SysFnTCCom (TCHT, lptkFunc);
} // End SysFnTCHT_EM


//***************************************************************************
//  $SysFnTCLF_EM
//
//  System function:  []TCLF -- Terminal Control, Linefeed
//***************************************************************************

LPYYSTYPE SysFnTCLF_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    return SysFnTCCom (TCLF, lptkFunc);
} // End SysFnTCLF_EM


//***************************************************************************
//  $SysFnTCNL_EM
//
//  System function:  []TCNL -- Terminal Control, Newline
//***************************************************************************

LPYYSTYPE SysFnTCNL_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    return SysFnTCCom (TCNL, lptkFunc);
} // End SysFnTCNL_EM


//***************************************************************************
//  $SysFnTCNUL_EM
//
//  System function:  []TCNUL -- Terminal Control, Nul
//***************************************************************************

LPYYSTYPE SysFnTCNUL_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    return SysFnTCCom (TCNUL, lptkFunc);
} // End SysFnTCNUL_EM


//***************************************************************************
//  $SysFnTS_EM
//
//  System function:  []TS -- Time Stamp
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnTS_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnTS_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    SYSTEMTIME SystemTime;
    UINT       ByteRes;
    HGLOBAL    hGlbRes;
    LPVOID     lpMem;
    LPYYSTYPE  lpYYRes;

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    // Calculate space needed for the result
    ByteRes = (UINT) CalcArraySize (ARRAY_INT, 7, 1);

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
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->Perm       = 0;           // Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
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

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnTS_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnTYPE_EM
//
//  System function:  []TYPE -- Prototype
//***************************************************************************

LPYYSTYPE SysFnTYPE_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonTYPE_EM (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydTYPE_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnTYPE_EM


//***************************************************************************
//  $SysFnMonTYPE_EM
//
//  Monadic []TYPE -- Prototype
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonTYPE_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnMonTYPE_EM
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    HGLOBAL hGlbData,
            hGlbRes;
    LPYYSTYPE lpYYRes;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Split cases based upon the token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, it's an HGLOBAL
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                break;      // Continue with HGLOBAL case
            } // End IF

            // Handle the immediate case

            // stData is an immediate
            Assert (lptkRhtArg->tkData.tkSym->stFlags.Imm);

            lpYYRes->tkToken.tkFlags.TknType = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType = lptkRhtArg->tkData.tkSym->stFlags.ImmType;

            // Split cases based upon the token's immediate type
            switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    lpYYRes->tkToken.tkData.tkBoolean = 0;

                    return lpYYRes;

                case IMMTYPE_INT:
                    lpYYRes->tkToken.tkData.tkInteger = 0;

                    return lpYYRes;

                case IMMTYPE_FLOAT:
                    lpYYRes->tkToken.tkData.tkFloat   = 0;

                    return lpYYRes;

                case IMMTYPE_CHAR:
                    lpYYRes->tkToken.tkData.tkChar    = L' ';

                    return lpYYRes;

                defstop
                    return NULL;
            } // End SWITCH

            DbgStop ();         // We should never get here

        case TKT_VARIMMED:
            lpYYRes->tkToken.tkFlags.TknType = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType = lptkRhtArg->tkFlags.ImmType;

            // Split cases based upon the token's immediate type
            switch (lptkRhtArg->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    lpYYRes->tkToken.tkData.tkBoolean = 0;

                    return lpYYRes;

                case IMMTYPE_INT:
                    lpYYRes->tkToken.tkData.tkInteger = 0;

                    return lpYYRes;

                case IMMTYPE_FLOAT:
                    lpYYRes->tkToken.tkData.tkFloat   = 0;

                    return lpYYRes;

                case IMMTYPE_CHAR:
                    lpYYRes->tkToken.tkData.tkChar    = L' ';

                    return lpYYRes;

                defstop
                    return NULL;
            } // End SWITCH

            DbgStop ();         // We should never get here

        case TKT_LISTPAR:
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkFunc);
            YYFree (lpYYRes); lpYYRes = NULL; return NULL;

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
    hGlbRes = MakePrototype_EM (ClrPtrTypeDirGlb (hGlbData),// Proto arg handle
                                lptkFunc,       // Ptr to function token
                                MP_CHARS);      // CHARs allowed
    if (!hGlbRes)
        return NULL;

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnMonTYPE_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydTYPE_EM
//
//  Dyadic []TYPE -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydTYPE_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE SysFnDydTYPE_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token

{
    return PrimFnSyntaxError_EM (lptkFunc);
} // End SysFnDydTYPE_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ConvTime
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
