//***************************************************************************
//  NARS2000 -- System Function -- Quad EX
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"
#include "sis.h"
#include "dfnhdr.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $SysFnEX_EM_YY
//
//  System function:  []EX -- Expunge Name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnEX_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnEX_EM_YY
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
        return SysFnMonEX_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydEX_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnEX_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonEX_EM_YY
//
//  Monadic []EX -- Expunge Name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonEX_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonEX_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeRht;        // Right arg storage type
    APLNELM      aplNELMRht,        // Right arg NELM
                 aplNELMRes,        // Result NELM
                 aplNELMCol;        // Result column NELM
    APLRANK      aplRankRht;        // Right arg Rank
    APLLONGEST   aplLongestRht;     // Right arg longest if immediate
    HGLOBAL      hGlbRht = NULL,    // Right arg global memory handle
                 hGlbRes = NULL;    // Result    ...
    LPVOID       lpMemRht = NULL,   // Ptr to right arg global memory
                 lpMemRes = NULL;   // Ptr to result    ...
    LPAPLCHAR    lpMemDataRht,      // Ptr to right arg char data
                 lpMemDataStart;    // Ptr to start of identifier
    LPAPLBOOL    lpMemDataRes;      // Ptr to result Boolean data
    APLUINT      uRht,              // Loop counter
                 uCol,              // ...
                 ByteRes;           // # bytes in the result
    LPSYMENTRY   lpSymEntry;        // Ptr to SYMENTRY
    STFLAGS      stFlags;           // STE flags
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    BOOL         bRet = TRUE;       // TRUE iff result is valid
    UINT         uBitIndex;         // Bit index for looping through Boolean result

    // The right arg may be of three forms:
    //   1.  a scalar    name  as in 'a'
    //   2.  a vector of name  as in 'a' (not 'a b c')
    //   3.  a matrix of names as in 3 1{rho}'abc'

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

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
    bRet =
      CalcNumIDs (aplNELMRht,       // Right arg NELM
                  aplRankRht,       // Right arg rank
                  aplLongestRht,    // Right arg longest
                  TRUE,             // TRUE iff we allow multiple names in a vector
                  lpMemRht,         // Ptr to right arg global memory
                 &aplNELMRes,       // Ptr to # right arg IDs
                 &aplNELMCol);      // Ptr to # right arg cols (matrix only)
    // Note that if bRet EQ FALSE, aplNELMRes EQ 1

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_BOOL, aplNELMRes, 1);

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
    lpHeader->ArrType    = ARRAY_BOOL;
////lpHeader->Perm       = 0;           // Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimensions to the data
    lpMemDataRes = VarArrayBaseToData (lpMemRes, 1);

    // If we failed in CalcNumIDs, quit now
    if (!bRet)
        goto YYALLOC_EXIT;

    // Expunge each name

    // Initialize the bit index
    uBitIndex = 0;

    // Split cases based upon the right arg rank
    switch (aplRankRht)
    {
        case 0:
            // Lookup the name in the symbol table
            // SymTabLookupName sets the .ObjName enum,
            //   and the .Inuse flag
            ZeroMemory (&stFlags, sizeof (stFlags));
            lpSymEntry = SymTabLookupNameLength ((LPAPLCHAR) &aplLongestRht,
                                                 1,
                                                &stFlags);
            // If found, attempt to expunge the name
            // If not found, return a one if it's a valid name, zero otherwise
            if (lpSymEntry)
                *lpMemDataRes |= (ExpungeName (lpSymEntry)) << uBitIndex;
            else
                *lpMemDataRes |= (ValidName ((LPAPLCHAR) &aplLongestRht,
                                             1)) << uBitIndex;
            break;

        case 1:
            // Skip over the header and dimensions to the data
            lpMemDataRht = VarArrayBaseToData (lpMemRht, aplRankRht);

            // Loop through the right arg looking for identifiers
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
                    // SymTabLookupName sets the .ObjName enum,
                    //   and the .Inuse flag
                    ZeroMemory (&stFlags, sizeof (stFlags));
                    lpSymEntry = SymTabLookupNameLength (lpMemDataStart,
                                                        &lpMemDataRht[uRht] - lpMemDataStart,
                                                        &stFlags);
                    // If found, attempt to expunge the name
                    // If not found, return a one if it's a valid name, zero otherwise
                    if (lpSymEntry)
                        *lpMemDataRes |= (ExpungeName (lpSymEntry)) << uBitIndex;
                    else
                        *lpMemDataRes |= (ValidName (lpMemDataStart,
                                                    &lpMemDataRht[uRht] - lpMemDataStart)) << uBitIndex;
                    // Check for end-of-byte
                    if (++uBitIndex EQ NBIB)
                    {
                        uBitIndex = 0;      // Start over
                        lpMemDataRes++;     // Skip to next byte
                    } // End IF
                } else
                    break;
            } // End WHILE

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
                // SymTabLookupName sets the .ObjName enum,
                //   and the .Inuse flag
                ZeroMemory (&stFlags, sizeof (stFlags));
                lpSymEntry = SymTabLookupNameLength (&lpMemDataStart[uCol],
                                                      (UINT) (aplNELMCol - uCol),
                                                     &stFlags);
                // If found, attempt to expunge the name
                // If not found, return a one if it's a valid name, zero otherwise
                if (lpSymEntry)
                    *lpMemDataRes |= ExpungeName (lpSymEntry);
                else
                    *lpMemDataRes |= (ValidName (lpMemDataStart,
                                                 (UINT) (aplNELMCol - uCol))) << uBitIndex;
                // Check for end-of-byte
                if (++uBitIndex EQ NBIB)
                {
                    uBitIndex = 0;      // Start over
                    lpMemDataRes++;     // Skip to next byte
                } // End IF
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH
YYALLOC_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = lpMemDataRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
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
} // End SysFnMonEX_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  ExpungeName
//
//  Expunge a given name and
//    return a one iff successful
//***************************************************************************

APLBOOL ExpungeName
    (LPSYMENTRY lpSymEntry)

{
    STFLAGS stFlagsMT = {0};    // STE flags for empty entry

    // Fill in mask flag values for empty entry
    stFlagsMT.Inuse   = 1;          // Retain Inuse flag
    stFlagsMT.ObjName = NEG1U;      // ...    ObjName setting

    // Check for eraseability
    if (!EraseableName (lpSymEntry))
        return 0;

    // If the STE is not immediate, free the global memory handle
    if (!lpSymEntry->stFlags.Imm)
        FreeResultGlobalDFV (ClrPtrTypeDirAsGlb (lpSymEntry->stData.stGlbData));

    // If the entry is not a system name, mark it as empty (e.g., VALUE ERROR)
    if (lpSymEntry->stFlags.ObjName NE OBJNAME_SYS)
    {
        // Clear the STE flags & data
        *(PUINT_PTR) &lpSymEntry->stFlags &= *(PUINT_PTR) &stFlagsMT;
        lpSymEntry->stData.stLongest = 0;
    } // End IF

    return 1;
} // End ExpungeName


//***************************************************************************
//  EraseableName
//
//  Return a one iff the name is erasable
//***************************************************************************

APLBOOL EraseableName
    (LPSYMENTRY lpSymEntry)

{
    HGLOBAL   htGlbName;        // Name global memory handle
    LPAPLCHAR lpMemName;        // Ptr to name global memory
    APLBOOL   bRet;             // TRUE iff eraseable name

    // Split cases based upon the Name Type
    switch (lpSymEntry->stFlags.ObjType)
    {
        case NAMETYPE_UNK:
        case NAMETYPE_VAR:
        case NAMETYPE_FN0:
        case NAMETYPE_FN12:
        case NAMETYPE_OP1:
        case NAMETYPE_OP2:
            // If the name is suspended or pendent, it's not eraseable
            if (IzitSusPendent (lpSymEntry))
                return 0;

            // Get the name global memory handle
            htGlbName = lpSymEntry->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (htGlbName);

            // Izit a valid name?
            bRet = ValidName (lpMemName, lstrlenW (lpMemName));

            // Not if the first char is Quad or Quote-quad
            bRet &= (lpMemName[0] NE UTF16_QUAD && lpMemName[0] NE UTF16_QUOTEQUAD);

            // We no longer need this ptr
            MyGlobalUnlock (htGlbName); lpMemName = NULL;

            return bRet;

////////case NAMETYPE_LST:
        defstop
            return 0;
    } // End SWITCH
} // End EraseableName


//***************************************************************************
//  IzitSusPendent
//
//  Return a one if the name is that of a suspended or
//    pendent defined function/operator
//***************************************************************************

APLBOOL IzitSusPendent
    (LPSYMENTRY lpSymEntry)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    APLBOOL      bRet = FALSE;  // TRUE iff name is suspended or pendent
    LPSIS_HEADER lpSISCur;      // Ptr to current SIS layer
    HGLOBAL      htGlbName;     // Name global memory handle
    LPAPLCHAR    lpMemName,     // Ptr to name global memory
                 lpFcnName;     // Ptr to function name global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get a ptr to the innermost SIS layer
    lpSISCur = lpMemPTD->lpSISCur;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Get the name global memory handle
    htGlbName = lpSymEntry->stHshEntry->htGlbName;

    // Lock the memory to get a ptr to it
    lpMemName = MyGlobalLock (htGlbName);

    while (lpSISCur && !bRet)
    {
        // Split cases based upon the function type
        switch (lpSISCur->DfnType)
        {
            case DFNTYPE_IMM:
            case DFNTYPE_EXEC:
            case DFNTYPE_QUAD:
                break;

            case DFNTYPE_OP1:
            case DFNTYPE_OP2:
            case DFNTYPE_FCN:
                // Lock the memory to get a ptr to it
                lpFcnName = MyGlobalLock (lpSISCur->hGlbFcnName);

                // Compare the names
                bRet = (lstrcmpW (lpMemName, lpFcnName) EQ 0);

                // We no longer need this ptr
                MyGlobalUnlock (lpSISCur->hGlbFcnName); lpFcnName = NULL;

                break;

            case DFNTYPE_UNK:
            defstop
                break;
        } // End SWITCH

        // Skip to the previous SIS layer
        lpSISCur = lpSISCur->lpSISPrv;
    } // End WHILE

    // We no longer need this ptr
    MyGlobalUnlock (htGlbName); lpMemName = NULL;

    return bRet;
} // End IzitSusPendent


//***************************************************************************
//  $SysFnDydEX_EM_YY
//
//  Dyadic []EX -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydEX_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydEX_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token

     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc);
} // End SysFnDydEX_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_ex.c
//***************************************************************************
