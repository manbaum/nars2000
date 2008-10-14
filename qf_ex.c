//***************************************************************************
//  NARS2000 -- System Function -- Quad EX
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2008 Sudley Place Software

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

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"
#include "sis.h"
#include "fh_parse.h"

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
    UBOOL        bRet = TRUE;       // TRUE iff result is valid
    UINT         uBitIndex;         // Bit index for looping through Boolean result

    // The right arg may be of three forms:
    //   1.  a scalar    name  as in 'a'
    //   2.  a vector of names as in 'a' or 'a b c'
    //   3.  a matrix of names as in 3 1{rho}'abc'

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsRank3P (aplRankRht))
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Check for DOMAIN ERROR
    if (!IsSimple (aplTypeRht)
     || (!IsSimpleChar (aplTypeRht)
      && !IsEmpty (aplNELMRht)))
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
                *lpMemDataRes |= (IsValidName ((LPAPLCHAR) &aplLongestRht,
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
                                                        (UINT) (&lpMemDataRht[uRht] - lpMemDataStart),
                                                        &stFlags);
                    // If found, attempt to expunge the name
                    // If not found, return a one if it's a valid name, zero otherwise
                    if (lpSymEntry)
                        *lpMemDataRes |= (ExpungeName (lpSymEntry)) << uBitIndex;
                    else
                        *lpMemDataRes |= (IsValidName (lpMemDataStart,
                                                      (UINT) (&lpMemDataRht[uRht] - lpMemDataStart))) << uBitIndex;
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
                    *lpMemDataRes |= (ExpungeName (lpSymEntry)) << uBitIndex;
                else
                    *lpMemDataRes |= (IsValidName (lpMemDataStart,
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
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
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
//  $ExpungeName
//
//  Expunge a given name and
//    return a one iff successful
//***************************************************************************

APLBOOL ExpungeName
    (LPSYMENTRY lpSymEntry)         // Ptr to the Symbol Table Entry

{
    // Check for eraseability
    if (!EraseableName (lpSymEntry))
        return 0;

    // If the STE is not immediate and has a value, free the global memory handle
    if (!lpSymEntry->stFlags.Imm && lpSymEntry->stFlags.Value)
        FreeResultGlobalDFLV (lpSymEntry->stData.stGlbData);

    // Erase the Symbol Table Entry
    EraseSTE (lpSymEntry);

    return 1;
} // End ExpungeName


//***************************************************************************
//  $EraseSTE
//
//  Erase a Symbol Table Entry
//***************************************************************************

void EraseSTE
    (LPSYMENTRY lpSymEntry)

{
    // If the entry is not a system name, mark it as empty (e.g., VALUE ERROR)
    if (lpSymEntry->stFlags.ObjName NE OBJNAME_SYS)
    {
        STFLAGS stFlagsMT = {0};        // STE flags for empty entry

        // Fill in mask flag values for erased entry
        stFlagsMT.Inuse   = TRUE;       // Retain Inuse flag
        stFlagsMT.ObjName = NEG1U;      // ...    ObjName setting

        // Clear the STE flags & data
        *(UINT *) &lpSymEntry->stFlags &= *(UINT *) &stFlagsMT;
        lpSymEntry->stData.stLongest = 0;
    } // End IF
} // End EraseSTE


//***************************************************************************
//  $EraseableName
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
    switch (lpSymEntry->stFlags.stNameType)
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
            bRet = IsValidName (lpMemName, lstrlenW (lpMemName));

            // Not if it's a system name
            bRet &= !IsSysName (lpMemName);

            // We no longer need this ptr
            MyGlobalUnlock (htGlbName); lpMemName = NULL;

            return bRet;

////////case NAMETYPE_LST:
        defstop
            return 0;
    } // End SWITCH
} // End EraseableName


//***************************************************************************
//  $IzitSusPendent
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
