//***************************************************************************
//  NARS2000 -- System Function -- Quad SIZE
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


//***************************************************************************
//  $SysFnSIZE_EM_YY
//
//  System function:  []SIZE -- Size of an identifier
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnSIZE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnSIZE_EM_YY
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
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonSIZE_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydSIZE_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnSIZE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonSIZE_EM_YY
//
//  Monadic []SIZE -- Size of an identifier
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonSIZE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonSIZE_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeRht;        // Right arg storage type
    APLNELM      aplNELMRht,        // Right arg NELM
                 aplNELMRes,        // Result    ...
                 aplNELMCol;        // Result column NELM
    APLRANK      aplRankRht;        // Right arg Rank
    APLLONGEST   aplLongestRht;     // Right arg longest if immediate
    HGLOBAL      hGlbRht = NULL,    // Right arg global memory handle
                 hGlbRes = NULL;    // Result    ...
    LPVOID       lpMemRht = NULL,   // Ptr to right arg global memory
                 lpMemRes = NULL;   // Ptr to result    ...
    LPAPLCHAR    lpMemDataRht,      // Ptr to right arg char data
                 lpMemDataStart;    // Ptr to start of identifier
    LPAPLINT     lpMemDataRes;      // Ptr to result integer data
    APLUINT      uRht,              // Loop counter
                 uCol,              // ...
                 ByteRes;           // # bytes in the result
    LPSYMENTRY   lpSymEntry;        // Ptr to SYMENTRY
    STFLAGS      stFlags;           // STE flags
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result

    // The right arg may be of three forms:
    //   1.  a scalar    name  as in 'a'
    //   2.  a vector of names as in 'a b c'
    //   3.  a matrix of names as in 3 1{rho}'abc'

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsRank3P (aplRankRht))
        goto RANK_EXIT;

    // Check for DOMAIN ERROR
    if (!IsSimple (aplTypeRht)
     || (!IsSimpleChar (aplTypeRht)
      && !IsEmpty (aplNELMRht)))
        goto DOMAIN_EXIT;

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Calculate the # identifiers in the argument
    //   allowing for vector and matrix with multiple names
    CalcNumIDs (aplNELMRht,         // Right arg NELM
                aplRankRht,         // Right arg rank
                aplLongestRht,      // Right arg longest
                TRUE,               // TRUE iff we allow multiple names in a vector
                lpMemRht,           // Ptr to right arg global memory
               &aplNELMRes,         // Ptr to # right arg IDs
               &aplNELMCol);        // Ptr to # right arg cols (matrix only)
    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_INT, aplNELMRes, 1);

    // Allocate space for the result
    // N.B. Conversion from APLUINT to UINT
    Assert (ByteRes EQ (APLU3264) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimensions to the data
    lpMemDataRes = VarArrayBaseToData (lpMemRes, 1);

    // Calculate the size of each element

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
            // If not found, return zero
            if (!lpSymEntry)
                *lpMemDataRes++ = 0;
            else
                *lpMemDataRes++ = CalcSymentrySize (lpSymEntry);
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
                                                        (APLU3264) (&lpMemDataRht[uRht] - lpMemDataStart),
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
                // SymTabLookupName sets the .ObjName enum,
                //   and the .Inuse flag
                ZeroMemory (&stFlags, sizeof (stFlags));
                lpSymEntry = SymTabLookupNameLength (&lpMemDataStart[uCol],
                                                      (APLU3264) (aplNELMCol - uCol),
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
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
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
} // End SysFnMonSize_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydSIZE_EM_YY
//
//  Dyadic []SIZE -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydSIZE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydSIZE_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc);
} // End SysFnDydSIZE_EM_YY
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
    if (IsNameTypeVar (lpSymEntry->stFlags.stNameType))
    {
        // Start with the size of the SYMENTRY
        aplSize = sizeof (SYMENTRY);

        // Recurse through the array returning the total size
        aplSize += CalcGlbSize (lpSymEntry->stData.stGlbData);
    } else
    // If it is a user function/operator, ...
    if (IsNameTypeFnOp (lpSymEntry->stFlags.stNameType))
    {
        HGLOBAL      hGlbDfnHdr;        // User-defined function/operator header global memory handle
        LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header ...
        LPFCNLINE    lpFcnLines;        // Ptr to array of function line structs (FCNLINE[numFcnLines])
        UINT         uNumLines,         // # function lines
                     uLine;             // Loop counter

        // Get the global memory handle
        hGlbDfnHdr = lpSymEntry->stData.stGlbData;

        // stData is a valid HGLOBAL function array or user-defined function/operator
        Assert (IsGlbTypeFcnDir (hGlbDfnHdr)
             || IsGlbTypeDfnDir (hGlbDfnHdr));

        // Clear the ptr type bits
        hGlbDfnHdr = ClrPtrTypeDirAsGlb (hGlbDfnHdr);

        // Split cases based upon the user-defined function/operator bit
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
            if (lpMemDfnHdr->hGlbMonInfo)
                aplSize += MyGlobalSize (lpMemDfnHdr->hGlbMonInfo);
            // Get ptr to array of function line structs (FCNLINE[numFcnLines])
            lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

            // Get # function lines
            uNumLines = lpMemDfnHdr->numFcnLines;

            // Loop through the function lines
            for (uLine = 0; uLine < uNumLines; uLine++)
            {
                if (lpFcnLines->hGlbTxtLine)
                    aplSize += MyGlobalSize (lpFcnLines->hGlbTxtLine);

                if (lpFcnLines->hGlbTknLine)
                    aplSize += MyGlobalSize (lpFcnLines->hGlbTknLine);

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
                    + MyGlobalSize (ClrPtrTypeDirAsGlb (lpSymEntry->stData.stGlbData));
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

    hGlbData = ClrPtrTypeDirAsGlb (hGlbData);

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
//  End of File: qf_size.c
//***************************************************************************
