//***************************************************************************
//  NARS2000 -- System Function -- Quad NL
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $SysFnNL_EM_YY
//
//  System function:  []NL -- Name List
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNL_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNL_EM_YY
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
        return SysFnMonNL_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydNL_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnNL_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonNL_EM_YY
//
//  Monadic []NL -- Name List (by Class)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonNL_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonNL_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return SysFnDydNL_EM_YY (NULL,          // Ptr to left arg token
                             lptkFunc,      // Ptr to function token
                             lptkRhtArg,    // Ptr to right arg token
                             lptkAxis);     // Ptr to axis token (may be NULL)
} // End SysFnMonNL_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydNL_EM_YY
//
//  Dyadic []NL -- Name List (by Alphabet and Class)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydNC_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydNL_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeLft,        // Right arg storage type
                 aplTypeRht;        // Right arg storage type
    APLNELM      aplNELMLft,        // Right arg NELM
                 aplNELMRht;        // Right arg NELM
    APLRANK      aplRankLft,        // Right arg rank
                 aplRankRht;        // Right arg rank
    HGLOBAL      hGlbLft = NULL,    // Left arg global memory handle
                 hGlbRht = NULL,    // Right ...
                 hGlbRes = NULL;    // Result    ...
    LPVOID       lpMemRht = NULL;   // Ptr to right arg global memory
    LPAPLCHAR    lpMemLft = NULL,   // Ptr to left   ...
                 lpMemRes = NULL;   // Ptr to result    ...
    APLUINT      uLft,              // Loop counter
                 uRht,              // Loop counter
                 ByteRes;           // # bytes in the result
    APLLONGEST   aplLongestLft,     // Left arg immediate value
                 aplLongestRht;     // Right ...
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    APLINT       apaOffRht,         // Right arg APA offset
                 apaMulRht;         // Right arg APA multiplier
    UINT         uBitMask,          // Mask for looping through Booleans
                 nameClasses = 0,   // Bit flags for each nameclass 1 through (NAMECLASS_LENp1 - 1)
                 uNameLen,          // Length of the current name
                 uMaxNameLen = 0,   // Length of longest name
                 uSymCnt,           // Count of # matching STEs
                 uSymNum;           // Loop counter
    BOOL         bRet;              // TRUE iff result is valid
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPSYMENTRY   lpSymEntry;        // Ptr to current SYMENTRY
    LPAPLCHAR    lpMemName;         // Ptr to name global memory
    LPSYMENTRY  *lpSymSort;         // Ptr to LPSYMENTRYs for sorting

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // If there's a left arg, ...
    if (lptkLftArg)
    {
        // Get the attributes (Type, NELM, and Rank)
        //   of the left arg
        AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);

        // Check for LEFT RANK ERROR
        if (aplRankLft > 1)
        {
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Check for LEFT DOMAIN ERROR
        if (!IsSimple (aplTypeLft)
         || (aplTypeLft NE ARRAY_CHAR
          && aplNELMLft NE 0))
        {
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Get left arg global ptr
        aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);

        // If the left arg is immediate, point to its one char
        if (hGlbLft EQ NULL)
            lpMemLft = &(APLCHAR) aplLongestLft;
        else
            // Skip over the header and dimensions to the data
            lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

        // Validate the chars in the left arg
        for (uLft = 0; uLft < aplNELMLft; uLft++)
        if (!IzitNameChar (lpMemLft[uLft]))
        {
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End FOR/IF
    } // End IF

    // Check for RIGHT RANK ERROR
    if (aplRankRht > 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Check for RIGHT DOMAIN ERROR
    if (!IsSimpleNum (aplTypeRht))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Get right arg global ptr
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg is an HGLOBAL, ...
    if (hGlbRht)
    {
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
                // Loop through the right arg elements
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    uBitMask = 1 << (MASKLOG2NBIB & (UINT) uRht);

                    // Get the next value
                    aplLongestRht = (uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? 1 : 0;

                    if (aplLongestRht EQ 0
                     || aplLongestRht >= NAMECLASS_LENp1)
                    {
                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        goto ERROR_EXIT;
                    } // End IF

                    // Mark as nameclass aplLongestRht
                    nameClasses |= 1 << (UINT) aplLongestRht;
                } // End FOR

                break;

            case ARRAY_INT:
                // Loop through the right arg elements
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Get the next value
                    aplLongestRht = ((LPAPLINT) lpMemRht)[uRht];

                    if (aplLongestRht EQ 0
                     || aplLongestRht >= NAMECLASS_LENp1)
                    {
                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        goto ERROR_EXIT;
                    } // End IF

                    // Mark as nameclass aplLongestRht
                    nameClasses |= 1 << (UINT) aplLongestRht;
                } // End FOR

                break;

            case ARRAY_FLOAT:
                // Loop through the right arg elements
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Get the next value
                    aplLongestRht = FloatToAplint_SCT (((LPAPLFLOAT) lpMemRht)[uRht], &bRet);

                    if (!bRet
                     || aplLongestRht EQ 0
                     || aplLongestRht >= NAMECLASS_LENp1)
                    {
                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        goto ERROR_EXIT;
                    } // End IF

                    // Mark as nameclass aplLongestRht
                    nameClasses |= 1 << (UINT) aplLongestRht;
                } // End FOR

                break;

            case ARRAY_APA:
#define lpAPA           ((LPAPLAPA) lpMemRht)
                // Get the APA parameters
                apaOffRht = lpAPA->Off;
                apaMulRht = lpAPA->Mul;
#undef  lpAPA
                // Loop through the right arg elements
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Get the next value
                    aplLongestRht = apaOffRht + apaMulRht * uRht;

                    if (aplLongestRht EQ 0
                     || aplLongestRht >= NAMECLASS_LENp1)
                    {
                        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                   lptkFunc);
                        goto ERROR_EXIT;
                    } // End IF

                    // Mark as nameclass aplLongestRht
                    nameClasses |= 1 << (UINT) aplLongestRht;
                } // End FOR

                break;

            defstop
                break;
        } // End SWITCH
    } else
    // If the right arg is an immediate value
    {
        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_APA:
            case ARRAY_INT:
                if (aplLongestRht EQ 0
                 || aplLongestRht >= NAMECLASS_LENp1)
                {
                    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                               lptkFunc);
                    goto ERROR_EXIT;
                } // End IF

                // Mark as nameclass aplLongestRht
                nameClasses |= 1 << (UINT) aplLongestRht;

                break;

            case ARRAY_FLOAT:
                // Convert from float to APLINT
                aplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht, &bRet);
                if (!bRet
                 || aplLongestRht EQ 0
                 || aplLongestRht >= NAMECLASS_LENp1)
                {
                    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                               lptkFunc);
                    goto ERROR_EXIT;
                } // End IF

                // Mark as nameclass aplLongestRht
                nameClasses |= 1 << (UINT) aplLongestRht;

                break;

            defstop
                break;
        } // End SWITCH
    } // End IF/ELSE

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Initialize the LPSYMENTRY sort array
    lpSymSort = (LPSYMENTRY *) lpwszTemp;

    // Loop through the symbol table looking for STEs
    //   with one of the right arg name classes
    for (lpSymEntry = lpMemPTD->lpSymTab, uSymCnt = 0;
         lpSymEntry < lpMemPTD->lpSymTabNext;
         lpSymEntry++)
    if (lpSymEntry->stFlags.Inuse                   // It's in use
     && nameClasses & (1 << CalcNameClass (lpSymEntry))) // It's in one of the specified name classes
    {
        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLock (lpSymEntry->stHshEntry->htGlbName);

        // If there's a left arg, ensure the first char of the name
        //   is in the left arg
        if (lptkLftArg)
        {
            for (uLft = 0; uLft < aplNELMLft; uLft++)
            if (lpMemName[0] EQ lpMemLft[uLft])
                break;

            // If there's no match, continue looking
            if (uLft EQ aplNELMLft)
                continue;
        } // End IF

        // Get the name length
        uNameLen = lstrlenW (lpMemName);

        // Find the longest name
        uMaxNameLen = max (uMaxNameLen, uNameLen);

        // We no longer need this ptr
        MyGlobalUnlock (lpSymEntry->stHshEntry->htGlbName); lpMemName = NULL;

        // Save the LPSYMENTRY ptr for later use
        lpSymSort[uSymCnt] = lpSymEntry;

        // Count in another matching name
        uSymCnt++;
    } // End FOR/IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Sort the HGLOBALs
    ShellSort (lpSymSort, uSymCnt, CmpLPSYMENTRY);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, uSymCnt * uMaxNameLen * sizeof (APLCHAR), 2);

    //***************************************************************
    // Now we can allocate the storage for the result
    // N.B.:  Conversion from APLUINT to UINT.
    //***************************************************************
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

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->Perm       = 0;               // Already zero from GHND
////lpHeader->SysVar     = 0;               // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = uSymCnt * uMaxNameLen;
    lpHeader->Rank       = 2;
#undef  lpHeader

    // Fill in the result's dimensions
    (VarArrayBaseToDim (lpMemRes))[0] = uSymCnt;
    (VarArrayBaseToDim (lpMemRes))[1] = uMaxNameLen;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, 2);

    // Loop through the sorted STEs copying the names to the result
    for (uSymNum = 0; uSymNum < uSymCnt; uSymNum++)
    {
        // Copy the STE name to the result
        lpMemRes = CopySteName (lpMemRes, lpSymSort[uSymNum]);

        // Fill in the tail of the name with blanks
        // Could use FillMemoryW ??
        for (uRht = uNameLen; uRht < uMaxNameLen; uRht++)
            *lpMemRes++ = L' ';
    } // End FOR

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
ERROR_EXIT:
    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnDydNL_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_nl.c
//***************************************************************************
