//***************************************************************************
//  NARS2000 -- System Function -- Quad CR
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "dfnhdr.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $SysFnCR_EM
//
//  System function:  []CR -- Canonical Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnCR_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnCR_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis);
        return NULL;
    } // End IF

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonCR_EM (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydCR_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnCR_EM
#undef  APPEND_NAME


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

LPPL_YYSTYPE SysFnMonCR_EM
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE       aplTypeRht;          // Right arg storage type
    APLNELM        aplNELMRht,          // Right arg NELM
                   aplNELMRes;          // Result    ...
    APLRANK        aplRankRht;          // Right arg rank
    HGLOBAL        hGlbRht = NULL,      // Right arg global memory handle
                   hGlbRes = NULL,      // Result    ...
                   hGlbData,            // Data      ...
                   hGlbTxtLine;         // Line text ...
    LPVOID         lpMemRht = NULL,     // Ptr to right arg global memory
                   lpMemRes = NULL,     // Ptr to result    ...
                   lpMemData;           // Ptr to function data ...
    LPMEMTXT_UNION lpMemTxtLine;        // Ptr to header/line text global memory
    UINT           uLineLen;            // Length of a text line
    APLLONGEST     aplLongestRht;       // Right arg longest if immediate
    LPSYMENTRY     lpSymEntry;          // Ptr to SYMENTRY
    STFLAGS        stFlags;             // STE flags
    LPPL_YYSTYPE   lpYYRes = NULL;      // Ptr to the result
    LPAPLCHAR      lpw;                 // Ptr to wide chars

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
        // SymTabLookupName sets the .ObjName enum,
        //   and the .Inuse flag
        lpSymEntry = SymTabLookupNameLength ((LPAPLCHAR) &aplLongestRht,
                                             (UINT) aplNELMRht,
                                            &stFlags);
    else
    {
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

        // Lookup the name in the symbol table
        // SymTabLookupName sets the .ObjName enum,
        //   and the .Inuse flag
        lpSymEntry = SymTabLookupNameLength ((LPAPLCHAR) lpMemRht,
                                             (UINT) aplNELMRht,
                                            &stFlags);
    } // End IF/ELSE

    // If not found,
    //   or it's a System Name,
    //   or without a value,
    //   return 0 x 0 char matrix
    if (!lpSymEntry
     ||  lpSymEntry->stFlags.ObjName EQ OBJNAME_SYS
     || !lpSymEntry->stFlags.Value)
        // Not the signature of anything we know
        // Return a 0 x 0 char matric
        hGlbRes = SysFnMonCR_ALLOC_EM (0, 2, lpwszTemp, lptkFunc);
    else
    // If it's immediate, ...
    if (lpSymEntry->stFlags.Imm)
    {
        // Copy the function name
        lpw = CopySteName (lpwszTemp, lpSymEntry);

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
            lpw = CopySteName (lpwszTemp, lpSymEntry);

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
                    lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

                    // Get the length of the line text
                    aplNELMRes = lpMemTxtLine->U;

                    // Copy the function line text to global memory
                    CopyMemory (lpwszTemp, &lpMemTxtLine->C, (UINT) aplNELMRes * sizeof (lpMemTxtLine->C));

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

                    // Finish the job via subroutine
                    hGlbRes = SysFnMonCR_ALLOC_EM (aplNELMRes, 1, lpwszTemp, lptkFunc);

                    break;

                case DFN_HEADER_SIGNATURE:
                {
                    LPDFN_HEADER  lpMemDfnHdr;      // Ptr to user-defined function/operator header ...
                    LPFCNLINE     lpFcnLines;       // Ptr to array of function line structs (FCNLINE[numFcnLines])
                    UINT          uNumLines,        // # function lines
                                  uLine,            // Loop counter
                                  uMaxLineLen;      // Length of the longest line
                    APLUINT       ByteRes;          // # bytes in the result

                    // Get ptr to user-defined function/operator header
                    lpMemDfnHdr = (LPDFN_HEADER) lpMemData;

                    // Lock the memory to get a ptr to it
                    lpMemTxtLine = MyGlobalLock (lpMemDfnHdr->hGlbTxtHdr);

                    // Get the length of the function header text
                    uMaxLineLen = lpMemTxtLine->U;

                    // We no longer need this ptr
                    MyGlobalUnlock (lpMemDfnHdr->hGlbTxtHdr); lpMemTxtLine = NULL;

                    // Get ptr to array of function line structs (FCNLINE[numFcnLines])
                    lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

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
                            lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

                            // Find the length of the longest line
                            uMaxLineLen = max (uMaxLineLen, lpMemTxtLine->U);

                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
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
                        lpMemTxtLine = MyGlobalLock (lpMemDfnHdr->hGlbTxtHdr);

                        // Get the length of the function header text
                        uLineLen = lpMemTxtLine->U;

                        // Copy the function header text to the result
                        CopyMemory (lpMemResChar, &lpMemTxtLine->C, uLineLen * sizeof (lpMemTxtLine->C));

                        // We no longer need this ptr
                        MyGlobalUnlock (lpMemDfnHdr->hGlbTxtHdr); lpMemTxtLine = NULL;

                        // Fill the remainder of the line with blanks
                        for (lpMemResChar += uLineLen;
                             uLineLen < uMaxLineLen;
                             uLineLen++)
                            *lpMemResChar++ = L' ';

                        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
                        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

                        // Run through the function lines copying each line text to the result
                        for (uLine = 0; uLine < uNumLines; uLine++)
                        {
                            // Get the line text global memory handle
                            hGlbTxtLine = lpFcnLines->hGlbTxtLine;

                            if (hGlbTxtLine)
                            {
                                // Lock the memory to get a ptr to it
                                lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

                                // Get the length of the function line text
                                uLineLen = lpMemTxtLine->U;

                                // Copy the function header text to the result
                                CopyMemory (lpMemResChar, &lpMemTxtLine->C, uLineLen * sizeof (lpMemTxtLine->C));

                                // We no longer need this ptr
                                MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

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
//  $SysFnMonCR_ALLOC_EM
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

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->Perm       = 0;           // Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;

#undef  lpHeader

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
//  $CopySteName
//
//  Copy a STE name
//***************************************************************************

LPAPLCHAR CopySteName
    (LPAPLCHAR  lpMemRes,       // Ptr to result global memory
     LPSYMENTRY lpSymEntry)     // Function name global memory handle

{
    LPAPLCHAR lpMemName;        // Ptr to function name global memory

    // Lock the memory to get a ptr to it
    lpMemName = MyGlobalLock (lpSymEntry->stHshEntry->htGlbName);

    // Copy the function name
    lstrcpyW (lpMemRes, lpMemName);

    // We no longer need this ptr
    MyGlobalUnlock (lpSymEntry->stHshEntry->htGlbName); lpMemName = NULL;

    // Point to the end
    return &lpMemRes[lstrlenW (lpMemRes)];
} // End CopySteName


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

LPPL_YYSTYPE SysFnDydCR_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnSyntaxError_EM (lptkFunc);
} // End SysFnDydCR_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_cr.c
//***************************************************************************
