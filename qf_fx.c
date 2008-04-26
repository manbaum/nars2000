//***************************************************************************
//  NARS2000 -- System Function -- Quad FX
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
#include "dfnhdr.h"
#include "savefcn.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $SysFnFX_EM_YY
//
//  System function:  []FX -- Function Fix
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnFX_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnFX_EM_YY
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
        return SysFnMonFX_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydFX_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnFX_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonFX_EM_YY
//
//  Monadic []FX -- Function Fix
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonFX_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonFX_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    HGLOBAL      hGlbRes = NULL;    // Result global memory handle
    LPVOID       lpMemRes = NULL;   // Ptr to result global memory
    APLSTYPE     aplTypeRht,        // Right arg storage type
                 aplTypeItmRht;     // Right arg item storage type
    APLNELM      aplNELMRht;        // Right arg NELM
    APLRANK      aplRankItmRht;     // Right arg item rank
    APLUINT      uRht;              // Loop counter
    HGLOBAL      hGlbItmRht;        // Right arg item global memory handle
    LPVOID       lpMemRht = NULL;   // Ptr to right arg global memory
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    APLLONGEST   aplLongestItmRht;  // Right arg item immediate value
    IMM_TYPES    immTypeItmRht;     // Right arg item immediate type
    SF_FCNS      SF_Fcns = {0};     // Common struc for SaveFunction
    FX_PARAMS    FX_Params = {0};   // Local struc for  ...

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &FX_Params.aplRankRht, &FX_Params.aplColsRht);

    // Check for empty right arg
    if (aplNELMRht EQ 0)
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Get right arg's global ptrs
    FX_Params.aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &FX_Params.hGlbRht, &lpMemRht);

    // Fill in common values
////SF_Fcns.bRet      =                     // Filled in by SaveFunctionCom
////SF_Fcns.uErrLine  =                     // ...
////SF_Fcns.lpSymName =                     // ...
    SF_Fcns.lptkFunc  = lptkFunc;           // Ptr to function token

    // Check for RIGHT RANK/DOMAIN ERRORs
    switch (FX_Params.aplRankRht)
    {
        case 0:                     // Right arg scalar
        case 1:                     // ...       vector
            // Fill in common values
            FX_Params.aplRowsRht = 1;

            if (IsNested (aplTypeRht))
            {
                // Ensure that each item is a char scalar/vector
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Get the next value from the right arg
                    GetNextValueMem (lpMemRht,              // Ptr to right arg global memory
                                     aplTypeRht,            // Right arg storage type
                                     uRht,                  // Right arg index
                                    &hGlbItmRht,            // Right arg item global memory handle
                                    &aplLongestItmRht,      // Ptr to right arg immediate value
                                    &immTypeItmRht);        // Ptr to right arg immediate type
                    // If the right arg item is an array, ...
                    if (hGlbItmRht)
                    {
                        // Get the right arg item global attrs
                        AttrsOfGlb (hGlbItmRht, &aplTypeItmRht, NULL, &aplRankItmRht, NULL);

                        // Ensure the item is a scalar/vector
                        if (aplRankItmRht > 1)
                        {
                            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                                       lptkFunc);
                            goto ERROR_EXIT;
                        } // End IF

                        // If this is the function header, ensure that it's not all blank
                        if (uRht EQ 0)
                        {
                            LPAPLCHAR lpMemItmRht;          // Ptr to right arg item global memory
                            APLNELM   aplNELMItmRht;        // Right arg item NELM

                            // Lock the memory to get a ptr to it
                            lpMemItmRht = MyGlobalLock (hGlbItmRht);

                            // Get the array NELM
#define lpHeader        ((LPVARARRAY_HEADER) lpMemRht)
                            aplNELMItmRht = lpHeader->NELM;

                            // Skip over the header to the data
                            lpMemItmRht = VarArrayBaseToData (lpMemRht, lpHeader->Rank);
#undef  lpHeader
                            // Check for all blanks
                            for (; aplNELMItmRht; aplNELMItmRht--)
                            if (lpMemItmRht[aplNELMItmRht - 1] NE L' ')
                                break;

                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbItmRht); lpMemItmRht = NULL;

                            if (aplNELMItmRht EQ 0)
                            {
                                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                           lptkFunc);
                                goto ERROR_EXIT;
                            } // End IF
                        } // End IF

                        // Ensure the item is simple char
                        if (!IsSimpleChar (aplTypeItmRht))
                        {
                            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                       lptkFunc);
                            goto ERROR_EXIT;
                        } // End IF
                    } else
                    {
                        // The right arg item is an immediate scalar

                        // If this is the function header, ensure that it's not empty
                        if (uRht EQ 0
                         && ((APLCHAR) FX_Params.aplLongestRht) EQ L' ')
                        {
                            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                       lptkFunc);
                            goto ERROR_EXIT;
                        } // End IF

                        // Ensure the item is simple char
                        if (!IsSimpleChar (aplTypeItmRht))
                        {
                            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                                       lptkFunc);
                            goto ERROR_EXIT;
                        } // End IF
                    } // End IF/ELSE
                } // End FOR

                // We no longer need this ptr
                MyGlobalUnlock (FX_Params.hGlbRht); lpMemRht = NULL;

                // Fill in common values
                SF_Fcns.SF_LineLen      = SF_LineLenN;      // Ptr to line length function
                SF_Fcns.SF_ReadLine     = SF_ReadLineN;     // Ptr to read line function
                SF_Fcns.SF_NumLines     = SF_NumLinesN;     // Ptr to get # lines function
                SF_Fcns.SF_CreationTime = SF_CreationTimeN; // Ptr to get function creation time
                SF_Fcns.SF_LastModTime  = SF_LastModTimeN;  // Ptr to get function last modification time
                SF_Fcns.SF_UndoBuffer   = SF_UndoBufferN;   // Ptr to get function Undo Buffer global memory handle
            } else
            if (!IsSimpleChar (aplTypeRht))
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } else
            {
                // Simple character scalar or vector

                // Check for DOMAIN ERROR
                if (aplNELMRht EQ 0)
                {
                    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                               lptkFunc);
                    goto ERROR_EXIT;
                } // End IF

                // Fill in common values
                SF_Fcns.SF_LineLen      = SF_LineLenSV;     // Ptr to line length function
                SF_Fcns.SF_ReadLine     = SF_ReadLineSV;    // Ptr to read line function
                SF_Fcns.SF_NumLines     = SF_NumLinesSV;    // Ptr to get # lines function
                SF_Fcns.SF_CreationTime = SF_CreationTimeSV;// Ptr to get function creation time
                SF_Fcns.SF_LastModTime  = SF_LastModTimeSV; // Ptr to get function last modification time
                SF_Fcns.SF_UndoBuffer   = SF_UndoBufferSV;  // Ptr to get function Undo Buffer global memory handle
            } // End IF/ELSE/...

            break;

        case 2:                     // Right arg matrix
            if (!IsSimpleChar (aplTypeRht))
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Fill in common values
            if (FX_Params.aplColsRht)
                FX_Params.aplRowsRht = aplNELMRht / FX_Params.aplColsRht;
            SF_Fcns.SF_LineLen      = SF_LineLenM;          // Ptr to line length function
            SF_Fcns.SF_ReadLine     = SF_ReadLineM;         // Ptr to read line function
            SF_Fcns.SF_NumLines     = SF_NumLinesM;         // Ptr to # lines function
            SF_Fcns.SF_CreationTime = SF_CreationTimeM;     // Ptr to get function creation time
            SF_Fcns.SF_LastModTime  = SF_LastModTimeM;      // Ptr to get function last modification time
            SF_Fcns.SF_UndoBuffer   = SF_UndoBufferM;       // Ptr to get function Undo Buffer global memory handle

            break;

        default:                    // Right arg rank > 2
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
    } // End SWITCH

    // Save ptr to local parameters
    SF_Fcns.LclParams = &FX_Params;

    // Call common routine
    if (SaveFunctionCom (NULL, &SF_Fcns))
    {
        HGLOBAL   htGlbName;        // Function name global memory handle
        LPAPLCHAR lpMemName;        // Ptr to function name global memory
        UINT      uNameLen;         // Function name length
        APLUINT   ByteRes;          // # bytes in the result

        // The function fix succeeded -- return the function name as the result

        // Get the function name global memory handle
        htGlbName = SF_Fcns.lpSymName->stHshEntry->htGlbName;

        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLock (htGlbName);

        // Get the length
        uNameLen = lstrlenW (lpMemName);

        //***************************************************************
        // Calculate space needed for the result
        //***************************************************************
        ByteRes = CalcArraySize (ARRAY_CHAR, uNameLen, 1);

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
            return NULL;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_CHAR;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = 0;               // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = uNameLen;
        lpHeader->Rank       = 1;
#undef  lpHeader

        // Fill in the result's dimension
        *VarArrayBaseToDim (lpMemRes) = uNameLen;

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, 1);

        // Copy the function name to the result
        CopyMemory (lpMemRes, lpMemName, uNameLen * sizeof (APLCHAR));

        // We no longer need this ptr
        MyGlobalUnlock (htGlbName); lpMemName = NULL;

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } else
    {
        // The function fix failed.

        // If the error line # is NEG1U, there is an error message, so return NULL.
        if (SF_Fcns.uErrLine EQ NEG1U)
            goto ERROR_EXIT;

        // Otherwise, return the error line # as an integer scalar (origin-sensitive)

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkInteger  = GetQuadIO () + SF_Fcns.uErrLine;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE
ERROR_EXIT:
    if (FX_Params.hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (FX_Params.hGlbRht); lpMemRht = NULL;
    } // End IF

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnMon_FX_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydFX_EM_YY
//
//  Dyadic []FX -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydFX_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydFX_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc);
} // End SysFnDydFX_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_fx.c
//***************************************************************************
