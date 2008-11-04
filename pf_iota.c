//***************************************************************************
//  NARS2000 -- Primitive Function -- Iota
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
#include "execmfn.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $PrimFnIota_EM_YY
//
//  Primitive function for monadic and dyadic iota ("index generator" and "index of")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnIota_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnIota_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_IOTA);

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
        return PrimFnMonIota_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydIota_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnIota_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnIota_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Iota
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnIota_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnIota_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnIota_EM_YY,   // Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnIota_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonIota_EM_YY
//
//  Primitive function for monadic iota ("index generator")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonIota_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonIota_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeRht;        // Right arg storage type
    APLNELM      aplNELMRht;        // Right arg NELM
    APLRANK      aplRankRht;        // Right arg rank
    HGLOBAL      hGlbRht,           // Right arg global memory handle
                 hGlbRes;           // Result    ...
    APLUINT      ByteRes;           // # bytes in the result
    LPVOID       lpMemRes;          // Ptr to result global memory
    UBOOL        bRet;              // TRUE iff result is valid
    APLLONGEST   aplLongestRht;     // Right arg iommediate value
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    APLBOOL      bQuadIO;           // []IO

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    //***************************************************************
    // Check the right argument for RANK, LENGTH, and DOMAIN ERRORs
    //***************************************************************
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    if (!IsSimpleNum (aplTypeRht))
        goto DOMAIN_EXIT;

    // Handle length != 1 args via magic function
    if (!IsSingleton (aplNELMRht))
        return PrimFnMonIotaVector_EM_YY (lptkFunc,     // Ptr to function token
                                          lptkRhtArg,   // Ptr to right arg token
                                          lptkAxis);    // Ptr to axis token (may be NULL)
    // Get right arg global ptrs
    aplLongestRht = GetGlbPtrs (lptkRhtArg, &hGlbRht);

    // If it's a global, get the first value
    if (hGlbRht)
        GetFirstValueGlb (hGlbRht,          // The global memory handle
                          NULL,             // Ptr to integer (or Boolean) (may be NULL)
                          NULL,             // ...    float (may be NULL)
                          NULL,             // ...    char (may be NULL)
                         &aplLongestRht,    // ...    longest (may be NULL)
                          NULL,             // ...    LPSYMENTRY or HGLOBAL (may be NULL)
                          NULL,             // ...    immediate type (see IMM_TYPES) (may be NULL)
                          NULL,             // ...    array type -- ARRAY_TYPES (may be NULL)
                          FALSE);           // TRUE iff we should expand LPSYMENTRY into immediate value
    // The singleton value is in aplLongestRht

    // If the storage type is float, attempt to convert it
    if (IsSimpleFlt (aplTypeRht))
    {
        // Attempt to convert the float to an integer using System CT
        aplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht, &bRet);

        if (!bRet)
            goto DOMAIN_EXIT;
    } // End IF

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_APA, abs64 (aplLongestRht), 1);

    // Allocate space for an APA
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_APA;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->PV0        = (bQuadIO EQ 0);
    lpHeader->PV1        = (bQuadIO EQ 1);
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = abs64 (aplLongestRht);
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Save the dimension in the result
    *VarArrayBaseToDim (lpMemRes) = abs64 (aplLongestRht);

    // Skip over the header and dimensions to the data (APLAPA struct)
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // Save the APA values
#define lpAPA       ((LPAPLAPA) lpMemRes)
    // Handle negative indices
    if (SIGN_APLINT (aplLongestRht))
        lpAPA->Off = bQuadIO + aplLongestRht;
    else
        lpAPA->Off = bQuadIO;

    lpAPA->Mul = 1;
#undef  lpAPA

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
NORMAL_EXIT:
    return lpYYRes;
} // End PrimFnMonIota_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonIotaVector_EM_YY
//
//  Monadic iota extended to length != 1 numeric arguments
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonIotaVector_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonIotaVector_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLNELM      aplNELMRht;        // Right arg NELM
    APLUINT      ByteRes;           // # bytes in the result
    LPVOID       lpMemRes;          // Ptr to result global memory
    HGLOBAL      hGlbPTD,           // PerTabData global memory handle
                 hGlbRes,           // Result     ...
                 hGlbMF;            // Magic function ...
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result

    Assert (lptkAxis EQ NULL);

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, NULL, &aplNELMRht, NULL, NULL);

    // Handle length zero args directly
    if (IsEmpty (aplNELMRht))
    {
        // Calculate space needed for the result
        ByteRes = CalcArraySize (ARRAY_NESTED, 1, 0);

        // Allocate space for the result
        hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (!hGlbRes)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_NESTED;
////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
        lpHeader->PV0        =
        lpHeader->PV1        = FALSE;
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = 1;
        lpHeader->Rank       = 0;
#undef  lpHeader

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, 0);

        // Save the only element {zilde}
        *((LPAPLNESTED) lpMemRes) = MakePtrTypeGlb (hGlbZilde);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

        goto YYALLOC_EXIT;
    } else
    {
        // Get the PerTabData global memory handle
        hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        // Get the magic function global memory handle
        hGlbMF = lpMemPTD->hGlbMF_MonIota;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

        //  Return the matrix of indices
        //  Use an internal magic function.
        return
          ExecuteMagicFunction_EM_YY (NULL,         // Ptr to left arg token
                                      lptkFunc,     // Ptr to function token
                                      NULL,         // Ptr to function strand
                                      lptkRhtArg,   // Ptr to right arg token
                                      lptkAxis,     // Ptr to axis token
                                      hGlbMF,       // Magic function global memory handle
                                      LINENUM_ONE); // Starting line # type (see LINE_NUMS)
    } // End IF/ELSE

YYALLOC_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return lpYYRes;
} // End PrimFnMonIotaVector_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  Magic function for Extended Monadic Iota
//
//  Extended Monadic Iota -- Index Generator For Arrays
//
//  Return an array of indices appropriate to an array of
//    the same shape as the right arg.
//
//  I believe this algorithm was first created by Carl M. Cheney.
//***************************************************************************

static APLCHAR MonHeader[] =
  $Z $IS $F L" " $R;

static APLCHAR MonLine1[] =
  $Z $IS $DISCLOSE $JOT L".,/" $IOTA $EACH $R;

static LPAPLCHAR MonBody[] =
{MonLine1,
};

MAGIC_FUNCTION MF_MonIota =
{MonHeader,
 MonBody,
 itemsizeof (MonBody),
};


//***************************************************************************
//  $PrimFnDydIota_EM_YY
//
//  Primitive function for dyadic iota ("index of")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydIota_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydIota_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE      aplTypeLft,       // Left arg storage type
                  aplTypeRht;       // Right ...
    APLNELM       aplNELMLft,       // Left arg NELM
                  aplNELMRht;       // Right ...
    APLRANK       aplRankLft,       // Left arg rank
                  aplRankRht;       // Right ...
    HGLOBAL       hGlbLft = NULL,   // Left arg global memory handle
                  hGlbRht = NULL,   // Right ...
                  hGlbRes = NULL;   // Result   ...
    LPVARARRAY_HEADER lpHeaderLft;  // Ptr to left arg header
    LPVOID        lpMemLft,         // Ptr to left arg global memory
                  lpMemRht;         // Ptr to right ...
    LPAPLUINT     lpMemRes = NULL;  // Ptr to result   ...
    UBOOL         bQuadIO;          // []IO
    APLUINT       NotFound;         // Not found value
    APLUINT       ByteRes;          // # bytes in the result
    APLLONGEST    aplLongestLft,    // Left arg immediate value
                  aplLongestRht;    // Right ...
    LPPL_YYSTYPE  lpYYRes = NULL;   // Ptr to the result
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;     // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    Assert (lptkAxis EQ NULL);

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for LEFT RANK ERROR
    if (IsScalar (aplRankLft))
        goto RANK_EXIT;
    else
    // Check for extended dyadic iota
    if (IsMultiRank (aplRankLft))
    {
        HGLOBAL      hGlbPTD,           // PerTabData global memory handle
                     hGlbMF;            // Magic function global memory handle
        LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

        // Get the PerTabData global memory handle
        hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        // Get the magic function global memory handle
        hGlbMF = lpMemPTD->hGlbMF_DydIota;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

        //  Extend to aplRankLft > 1 args by returning
        //    an array of index vectors where the length
        //    of each vector is aplRankLft.
        //  Use an internal magic function.
        return
          ExecuteMagicFunction_EM_YY (lptkLftArg,   // Ptr to left arg token
                                      lptkFunc,     // Ptr to function token
                                      NULL,         // Ptr to function strand
                                      lptkRhtArg,   // Ptr to right arg token
                                      lptkAxis,     // Ptr to axis token
                                      hGlbMF,       // Magic function global memory handle
                                      LINENUM_ONE); // Starting line # type (see LINE_NUMS)
    } // End IF

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    //***************************************************************
    // Calculate space needed for the result
    //***************************************************************
    ByteRes = CalcArraySize (ARRAY_INT, aplNELMRht, aplRankRht);

    //***************************************************************
    // Now we can allocate the storage for the result
    // N.B.:  Conversion from APLUINT to UINT.
    //***************************************************************
    Assert (ByteRes EQ (APLU3264) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

    // Save ptr to left arg header
    lpHeaderLft = lpMemLft;

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = (LPAPLUINT) VarArrayBaseToDim (lpMemRes);

    // Fill in the result's dimension
    if (lpMemRht)
    {
        // Skip over the header to the dimensions
        lpMemRht = VarArrayBaseToDim (lpMemRht);

        // Copy the left arg dimensions to the result
        CopyMemory (lpMemRes, lpMemRht, (APLU3264) aplRankRht * sizeof (APLDIM));

        // Skip over the dimensions to the data
        lpMemRes = VarArrayDimToData (lpMemRes, aplRankRht);
        lpMemRht = VarArrayDimToData (lpMemRht, aplRankRht);
    } else
        // Point to the right arg immediate value
        lpMemRht = &aplLongestRht;

    if (lpMemLft)
        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
    else
        // Point to the left arg immediate value
        lpMemLft = &aplLongestLft;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Calculate the NotFound value
    NotFound = bQuadIO + aplNELMLft;

    // If the left arg is non-empty, and
    //    the right arg is non-empty, and
    //    the left and right args are not different types (numeric vs. char), ...
    if (!(IsEmpty (aplNELMLft)
       || IsEmpty (aplNELMRht)
       || (IsSimpleNum (aplTypeLft) && IsSimpleChar (aplTypeRht))
       || (IsSimpleNum (aplTypeRht) && IsSimpleChar (aplTypeLft))))
    {
        // Split cases based upon the left & right arg storage types
        if (IsSimpleBool (aplTypeLft) && IsSimpleBool (aplTypeRht))
        {
            // Handle APLBOOL vs. APLBOOL
            if (!PrimFnDydIotaBvB (lpMemRes,        // Ptr to result global memory data
                                   aplNELMLft,      // Left arg NELM
                                   lpMemLft,        // Ptr to left arg global memory data
                                   aplNELMRht,      // Right arg NELM
                                   lpMemRht,        // Ptr to right arg global memory data
                                   bQuadIO,         // []IO
                                   NotFound,        // Not found value
                                   lpbCtrlBreak))   // Ptr to Ctrl-Break flag
                goto ERROR_EXIT;
        } else
        if (IsPermVector (lpHeaderLft))
        {
            // Handle PV vs. APLBOOL/APLINT/APLAPA
            if (!PrimFnDydIotaPvI_EM (lpMemRes,     // Ptr to result global memory data
                                      lpHeaderLft,  // Ptr to left arg header
                                      aplTypeLft,   // Left arg storage type
                                      aplNELMLft,   // Left arg NELM
                                      lpMemLft,     // Ptr to left arg global memory data
                                      aplTypeRht,   // Right arg storage type
                                      aplNELMRht,   // Right arg NELM
                                      lpMemRht,     // Ptr to right arg global memory data
                                      bQuadIO,      // []IO
                                      NotFound,     // Not found value
                                      lpbCtrlBreak, // Ptr to Ctrl-Break flag
                                      lptkFunc))    // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsSimpleInt (aplTypeLft) && IsSimpleInt (aplTypeRht))
        {
            // Handle APLBOOL/APLINT/APLAPA vs. APLBOOL/APLINT/APLAPA
            if (!PrimFnDydIotaIvI_EM (lpMemRes,     // Ptr to result global memory data
                                      aplTypeLft,   // Left arg storage type
                                      aplNELMLft,   // Left arg NELM
                                      lpMemLft,     // Ptr to left arg global memory data
                                      aplTypeRht,   // Right arg storage type
                                      aplNELMRht,   // Right arg NELM
                                      lpMemRht,     // Ptr to right arg global memory data
                                      bQuadIO,      // []IO
                                      NotFound,     // Not found value
                                      lpbCtrlBreak, // Ptr to Ctrl-Break flag
                                      lptkFunc))    // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsSimpleChar (aplTypeLft) && IsSimpleChar (aplTypeRht))
        {
            // Handle APLCHAR vs. APLCHAR
            if (!PrimFnDydIotaCvC_EM (lpMemRes,     // Ptr to result global memory data
                                      aplNELMLft,   // Left arg NELM
                                      lpMemLft,     // Ptr to left arg global memory data
                                      aplNELMRht,   // Right arg NELM
                                      lpMemRht,     // Ptr to right arg global memory data
                                      bQuadIO,      // []IO
                                      NotFound,     // Not found value
                                      lpbCtrlBreak, // Ptr to Ctrl-Break flag
                                      lptkFunc))    // Ptr to function token
                goto ERROR_EXIT;
        } else
        {
            // Handle all other combinations
            if (!PrimFnDydIotaOther (lpMemRes,      // Ptr to result global memory data
                                     aplTypeLft,    // Left arg storage type
                                     aplNELMLft,    // Left arg NELM
                                     lpMemLft,      // Ptr to left arg global memory data
                                     aplTypeRht,    // Right arg storage type
                                     aplNELMRht,    // Right arg NELM
                                     lpMemRht,      // Ptr to right arg global memory data
                                     bQuadIO,       // []IO
                                     NotFound,      // Not found value
                                     lpbCtrlBreak,  // Ptr to Ctrl-Break flag
                                     lptkFunc))     // Ptr to function token
                goto ERROR_EXIT;
        } // End IF/ELSE/...
    } else
    {
        APLUINT uRht;           // Loop counter

        // Fill the result with NotFound
        for (uRht = 0; uRht < aplNELMRht; uRht++)
            *lpMemRes++ = NotFound;
    } // End IF/ELSE

    // Unlock the result global memory in case TypeDemote actually demotes
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
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
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
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

    return lpYYRes;
} // End PrimFnDydIota_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydIotaBvB
//
//  Dyadic iota between Boolean args
//***************************************************************************

UBOOL PrimFnDydIotaBvB
    (LPAPLUINT lpMemRes,            // Ptr to result global memory data
     APLNELM   aplNELMLft,          // Left arg NELM
     LPAPLBOOL lpMemLft,            // Ptr to left arg global memory data
     APLNELM   aplNELMRht,          // Right arg (and result) NELM
     LPAPLBOOL lpMemRht,            // Ptr to right arg global memory data
     UBOOL     bQuadIO,             // []IO
     APLUINT   NotFound,            // Not found value
     LPUBOOL   lpbCtrlBreak)        // Ptr to Ctrl-Break flag

{
    UBOOL   Found0,                 // TRUE iff there is at least one 0 in the right arg
            Found1;                 // ...                            1 ...
    APLUINT Index0,                 // Index of the first 0 in the left arg
            Index1,                 // ...                1 ...
            BytesInLftBits,         // # bytes in the left arg data
            BytesInRhtBits,         // ...            right ...
            uLft,                   // Loop counter
            uRht,                   // Loop counter
            uTmp;                   // Temporary
    UINT    uBitMaskRht;            // Bit mask for the right arg

    // The result is either all NotFounds       (if left arg is empty -- already ruled out), or
    //                      same as Lft         (if left arg is all 1s), or
    //                      complement of Lft   (if left arg is all 0s), or
    //                      all 1s              (if left arg has at least one 0 and one 1)

    // Calculate the # bytes in the left arg (result) data
    BytesInLftBits = RoundUpBitsToBytes (aplNELMLft);

    // Calculate the # bytes in the right arg data
    BytesInRhtBits = RoundUpBitsToBytes (aplNELMRht);

    // Search the left arg for a 0
    for (Found0 = FALSE, Index0 = bQuadIO, uLft = 0; uLft < (BytesInLftBits - 1); uLft++)
    {
        uTmp = FastBoolTrans[lpMemLft[uLft]][0];
        Index0 += uTmp;

        if (uTmp < NBIB)
        {
            Found0 = TRUE;

            break;
        } // End IF
    } // End FOR

    // If we didn't find a 0, check the last byte (may be short)
    if (!Found0)
    {
        uTmp = FastBoolTrans[lpMemLft[uLft]][0];
        Index0 += uTmp;
    } // End IF

    // Search the left arg for a 1
    for (Found1 = FALSE, Index1 = bQuadIO, uLft = 0; uLft < (BytesInLftBits - 1); uLft++)
    {
        uTmp = FastBoolTrans[lpMemLft[uLft]][1];
        Index1 += uTmp;

        if (uTmp < NBIB)
        {
            Found1 = TRUE;

            break;
        } // End IF
    } // End FOR

    // If we didn't find a 1, check the last byte (may be short)
    if (!Found1)
    {
        uTmp = FastBoolTrans[lpMemLft[uLft]][1];
        Index1 += uTmp;
    } // End IF

    uBitMaskRht = BIT0;

    // Loop throught the right arg,
    //   saving in the result Index0 for each 0 in the right arg, and
    //                        Index1 for each 1 in the right arg
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        if (*lpMemRht & uBitMaskRht)
            *lpMemRes++ = Index1;
        else
            *lpMemRes++ = Index0;

        // Shift over the right bit mask
        uBitMaskRht <<= 1;

        // Check for end-of-byte
        if (uBitMaskRht EQ END_OF_BYTE)
        {
            uBitMaskRht = BIT0;     // Start over
            lpMemRht++;             // Skip to next byte
        } // End IF
    } // End FOR

    return TRUE;
ERROR_EXIT:
    return FALSE;
} // End PrimFnDydIotaBvB


//***************************************************************************
//  $PrimFnDydIotaIvI_EM
//
//  Dyadic iota between simple ints (APLBOOL/APLINT/APLAPA)
//***************************************************************************

UBOOL PrimFnDydIotaIvI_EM
    (LPAPLUINT lpMemRes,            // Ptr to result global memory data
     APLSTYPE  aplTypeLft,          // Left arg storage type
     APLNELM   aplNELMLft,          // Left arg NELM
     LPVOID    lpMemLft,            // Ptr to left arg global memory data
     APLSTYPE  aplTypeRht,          // Right arg storage type
     APLNELM   aplNELMRht,          // Right arg NELM
     LPVOID    lpMemRht,            // Ptr to right arg global memory data
     UBOOL     bQuadIO,             // []IO
     APLUINT   NotFound,            // Not found value
     LPUBOOL   lpbCtrlBreak,        // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)            // Ptr to function token

{
    APLUINT   uLft,                 // Loop counter
              uRht;                 // Loop counter
    APLINT    aplIntegerLft,        // Left arg integer
              aplIntegerRht;        // Right arg integer

    // ***FIXME*** -- Here's where we might do something clever such as
    //   sort the right arg or perhaps hash it assuming one or both args
    //   are large enough to warrant such extra work.

    // Also, we could pick off BvI and BvA quite easily.  So much to do, so little time.

    // For the moment, we'll go quadratic
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        aplIntegerRht =
          GetNextInteger (lpMemRht,
                          aplTypeRht,
                          uRht);
        // Loop through the left arg
        for (uLft = 0; uLft < aplNELMLft; uLft++)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            aplIntegerLft =
              GetNextInteger (lpMemLft,
                              aplTypeLft,
                              uLft);
            if (aplIntegerLft EQ aplIntegerRht)
            {
                // Set the result value
                *lpMemRes++ = uLft + bQuadIO;

                break;
            } // End IF
        } // End FOR

        // If we didn't find the value, ...
        if (uLft EQ aplNELMLft)
            // Set the result value
            *lpMemRes++ = NotFound;
    } // End FOR

    return TRUE;
ERROR_EXIT:
    return FALSE;
} // End PrimFnDydIotaIvI_EM


//***************************************************************************
//  $PrimFnDydIotaPvI_EM
//
//  Dyadic iota between a PV and simple ints (APLBOOL/APLINT/APLAPA)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydIotaPvI_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydIotaPvI_EM
    (LPAPLUINT         lpMemRes,        // Ptr to result global memory data
     LPVARARRAY_HEADER lpHeaderLft,     // Ptr to left arg header
     APLSTYPE          aplTypeLft,      // Left arg storage type
     APLNELM           aplNELMLft,      // Left arg NELM
     LPAPLUINT         lpMemLft,        // Ptr to left arg global memory data
     APLSTYPE          aplTypeRht,      // Right arg storage type
     APLNELM           aplNELMRht,      // Right arg NELM
     LPVOID            lpMemRht,        // Ptr to right arg global memory data
     UBOOL             bQuadIO,         // []IO
     APLUINT           NotFound,        // Not found value
     LPUBOOL           lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPTOKEN           lptkFunc)        // Ptr to function token

{
    APLUINT   uLft,                     // Loop counter
              uRht;                     // Loop counter
    APLINT    aplIntegerLft,            // Left arg integer
              aplIntegerRht;            // Right arg integer
    HGLOBAL   hGlbInv = NULL;           // Inverse indices global memory handle
    LPAPLUINT lpMemInv = NULL;          // Ptr to inverse indices global memory
    UBOOL     bRet = FALSE;             // TRUE iff the result is valid

    // Allocate a temporary array to hold the inverse indices
    hGlbInv = DbgGlobalAlloc (GHND, (APLU3264) (aplNELMLft * sizeof (APLUINT)));
    if (!hGlbInv)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemInv = MyGlobalLock (hGlbInv);

    // Loop through the left arg converting it to
    //   origin-0 inverse indices
    for (uLft = 0; uLft < aplNELMLft; uLft++)
    {
        // Get the next integer
        aplIntegerLft =
          GetNextInteger (lpMemLft,
                          aplTypeLft,
                          uLft);
        // Save as origin-0 inverse index
        lpMemInv[aplIntegerLft - lpHeaderLft->PV1] = uLft + bQuadIO;
    } // End FOR

    // Loop through the right arg looking up each value
    //   in the inverse indices and saving the answer
    //   in the result
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Get the next integer
        aplIntegerRht =
          GetNextInteger (lpMemRht,
                          aplTypeRht,
                          uRht);
        // Convert to same origin as the inverse indices
        aplIntegerRht = _isub64 (aplIntegerRht,  lpHeaderLft->PV1, &bRet);

        // If there's no overflow, and it's within range, ...
        if (bRet
         && (0 <= aplIntegerRht
          &&      aplIntegerRht <= ((APLINT) aplNELMLft - 1)))
            // Lookup in the inverse indices
            *lpMemRes++ = lpMemInv[aplIntegerRht];
        else
            // It's out of range
            *lpMemRes++ = NotFound;
    } // End FOR

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbInv)
    {
        if (lpMemInv)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbInv); lpMemInv = NULL;
        } // End IF

        // We no longer need this resource
        MyGlobalFree (hGlbInv); hGlbInv = NULL;
    } // End IF

    return bRet;
} // End PrimFnDydIotaPvI_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydIotaCvC_EM
//
//  Dyadic iota between char args
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydIotaCvC_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydIotaCvC_EM
    (LPAPLUINT lpMemRes,            // Ptr to result global memory data
     APLNELM   aplNELMLft,          // Left arg (and result) NELM
     LPAPLCHAR lpMemLft,            // Ptr to left arg global memory data
     APLNELM   aplNELMRht,          // Right arg NELM
     LPAPLCHAR lpMemRht,            // Ptr to right arg global memory data
     UBOOL     bQuadIO,             // []IO
     APLUINT   NotFound,            // Not found value
     LPUBOOL   lpbCtrlBreak,        // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)            // Ptr to function token

{
    LPAPLUINT lpMemTT = NULL;       // Translate table global memory handle
    UINT      ByteTT;               // # bytes in the Translate Table
    APLUINT   uRht;                 // Loop counter
    APLINT    iLft;                 // ...
    UBOOL     bRet = FALSE;         // TRUE iff the result is valid

    // Calculate # bytes in the TT at one APLUINT per 16-bit index (APLCHAR)
    ByteTT = APLCHAR_SIZE * sizeof (APLUINT);

    // Allocate space for a ByteTT Translate Table
    // Note that this allocation is GMEM_FIXED
    //   because we'll use it quickly and then free it.
    lpMemTT = MyGlobalAlloc (GMEM_FIXED, ByteTT);
    if (!lpMemTT)
        goto WSFULL_EXIT;

    // Trundle through the TT setting each value to NotFound
    for (uRht = 0; uRht < APLCHAR_SIZE; uRht++)
        lpMemTT[uRht] = NotFound;

    // Trundle through the left arg backwards marking the TT
    for (iLft = aplNELMLft - 1, lpMemLft += iLft;
         iLft >= 0;
         iLft--)
        lpMemTT[*lpMemLft--] = iLft + bQuadIO;

    // Trundle through the right arg looking for the chars
    //   in the TT and setting the result value
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Save the index from the TT in the result
        *lpMemRes++ = lpMemTT[*lpMemRht++];
    } // End FOR

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (lpMemTT)
    {
        // We no longer need this storage
        MyGlobalFree (lpMemTT); lpMemTT = NULL;
    } // End IF

    return bRet;
} // End PrimFnDydIotaCvC_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydIotaOther
//
//  Dyadic iota between all other arg combinations
//***************************************************************************

UBOOL PrimFnDydIotaOther
    (LPAPLUINT lpMemRes,            // Ptr to result global memory data
     APLSTYPE  aplTypeLft,          // Left arg storage type
     APLNELM   aplNELMLft,          // Left arg NELM
     LPVOID    lpMemLft,            // Ptr to left arg global memory data
     APLSTYPE  aplTypeRht,          // Right arg storage type
     APLNELM   aplNELMRht,          // Right arg NELM
     LPVOID    lpMemRht,            // Ptr to right arg global memory data
     UBOOL     bQuadIO,             // []IO
     APLUINT   NotFound,            // Not found value
     LPUBOOL   lpbCtrlBreak,        // Ptr to Ctrl-Break flag
     LPTOKEN   lptkFunc)            // Ptr to function token

{
    HGLOBAL      hGlbSubLft,        // Left arg item global memory handle
                 hGlbSubRht;        // Right ...
    APLLONGEST   aplLongestSubLft,  // Left arg item immediate value
                 aplLongestSubRht;  // Right ...
    IMM_TYPES    immTypeSubLft,     // Left arg item immediate type
                 immTypeSubRht;     // Right ...
    APLFLOAT     fQuadCT;           // []CT
    APLUINT      uLft,              // Loop counter
                 uRht;              // ...
    LPPL_YYSTYPE lpYYTmp;           // Ptr to the temporary result
    UBOOL        bCmp;              // TRUE iff the comparison is TRUE

    // This leaves:  Left vs. Right
    //                B   vs.    FHN
    //                I   vs.    FHN
    //                A   vs.    FHN
    //                F   vs. BIAFHN
    //                H   vs. BIAFHN
    //                N   vs. BIAFHN

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Loop through the right arg
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        TOKEN tkSubRht = {0};       // Right arg item token

        // Get the next value from the right arg
        GetNextValueMem (lpMemRht,              // Ptr to right arg global memory
                         aplTypeRht,            // Right arg storage type
                         uRht,                  // Right arg index
                        &hGlbSubRht,            // Right arg item global memory handle
                        &aplLongestSubRht,      // Ptr to right arg immediate value
                        &immTypeSubRht);        // Ptr to right arg immediate type
        // Fill in the right arg item token
        tkSubRht.tkFlags.TknType   = TKT_VARARRAY;
////////tkSubRht.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////tkSubRht.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
        tkSubRht.tkData.tkGlbData  = MakePtrTypeGlb (hGlbSubRht);
        tkSubRht.tkCharIndex       = NEG1U;

        // Loop through the left arg
        for (uLft = 0; uLft < aplNELMLft; uLft++)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            // Get the next value from the left arg
            GetNextValueMem (lpMemLft,              // Ptr to left arg global memory
                             aplTypeLft,            // Left arg storage type
                             uLft,                  // Left arg index
                            &hGlbSubLft,            // Left arg item global memory handle
                            &aplLongestSubLft,      // Ptr to left arg immediate value
                            &immTypeSubLft);        // Ptr to left arg immediate type
            // If both items are globals, ...
            if ((hGlbSubLft NE NULL) && (hGlbSubRht NE NULL))
            {
                TOKEN tkSubLft = {0};       // Left arg item token

                // Fill in the left arg item token
                tkSubLft.tkFlags.TknType   = TKT_VARARRAY;
////////////////tkSubLft.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////////////tkSubLft.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
                tkSubLft.tkData.tkGlbData  = MakePtrTypeGlb (hGlbSubLft);
                tkSubLft.tkCharIndex       = NEG1U;

                // Use match to determine equality
                lpYYTmp =
                  PrimFnDydEqualUnderbar_EM_YY (&tkSubLft,      // Ptr to left arg token
                                                 lptkFunc,      // Ptr to function token
                                                &tkSubRht,      // Ptr to right arg token
                                                 NULL);         // Ptr to axis token (may be NULL)
                // Save the result of the comparison
                bCmp = lpYYTmp->tkToken.tkData.tkBoolean;

                // Free the temporary result
                YYFree (lpYYTmp); lpYYTmp = NULL;

                if (bCmp)
                    goto SET_RESULT_VALUE;
            } else
            // If both items are simple, ...
            if ((hGlbSubLft EQ NULL) && (hGlbSubRht EQ NULL))
            {
                // If both items are APLCHARs, ...
                if (IsImmChr (immTypeSubLft) && IsImmChr (immTypeSubRht))
                {
                    // Compare the APLCHARs
                    if (((APLCHAR) aplLongestSubLft) EQ (APLCHAR) aplLongestSubRht)
                        goto SET_RESULT_VALUE;
                } else
                // If both items are numeric, ...
                if (IsImmNum (immTypeSubLft) && IsImmNum (immTypeSubRht))
                {
                    // This leaves BIF vs. BIF

                    // Split cases based upon the left arg item immediate type
                    switch (immTypeSubLft)
                    {
                        case IMMTYPE_BOOL:              // Lft = BOOL
                            // Split cases based upon the right arg item immediate type
                            switch (immTypeSubRht)
                            {
                                case IMMTYPE_BOOL:      // Lft = BOOL, Rht = BOOL
                                    if (((APLBOOL) aplLongestSubLft) EQ (APLBOOL) aplLongestSubRht)
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_INT:       // Lft = BOOL, Rht = INT
                                    if (((APLBOOL) aplLongestSubLft) EQ (APLINT)  aplLongestSubRht)
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_FLOAT:     // Lft = BOOL, Rht = FLOAT
                                    if (CompareCT ((APLBOOL) aplLongestSubLft, *(LPAPLFLOAT) &aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_VALUE;
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case IMMTYPE_INT:               // Lft = INT
                            // Split cases based upon the right arg item immediate type
                            switch (immTypeSubRht)
                            {
                                case IMMTYPE_BOOL:      // Lft = INT, Rht = BOOL
                                    if (((APLINT)  aplLongestSubLft) EQ (APLBOOL) aplLongestSubRht)
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_INT:       // Lft = INT, Rht = INT
                                    if (((APLINT)  aplLongestSubLft) EQ (APLINT)  aplLongestSubRht)
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_FLOAT:     // Lft = INT, Rht = FLOAT
                                    if (CompareCT ((APLFLOAT) (APLINT)  aplLongestSubLft, *(LPAPLFLOAT) &aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_VALUE;
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case IMMTYPE_FLOAT:             // Lft = FLOAT
                            // Split cases based upon the right arg item immediate type
                            switch (immTypeSubRht)
                            {
                                case IMMTYPE_BOOL:      // Lft = FLOAT, Rht = BOOL
                                    if (CompareCT (*(LPAPLFLOAT) &aplLongestSubLft, (APLBOOL) aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_INT:       // Lft = FLOAT, Rht = INT
                                    if (CompareCT (*(LPAPLFLOAT) &aplLongestSubLft, (APLFLOAT) (APLINT) aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_VALUE;
                                    break;

                                case IMMTYPE_FLOAT:     // Lft = FLOAT, Rht = FLOAT
                                    if (CompareCT (*(LPAPLFLOAT) &aplLongestSubLft, *(LPAPLFLOAT) &aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_VALUE;
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End IF/ELSE/...
            } // End IF/ELSE/...
        } // End FOR

        // We didn't find the it, so
        //   set the result value to NotFound
        *lpMemRes ++ = NotFound;

        continue;

SET_RESULT_VALUE:
        // Set the result value
        *lpMemRes++ = uLft + bQuadIO;
    } // End FOR

    return TRUE;
ERROR_EXIT:
    return FALSE;
} // End PrimFnDydIotaOther


//***************************************************************************
//  Magic function for extended dyadic iota
//
//  Extended dyadic iota
//
//  On rank > 1 left args, return an array of vector indices
//    such that A[A iota R] is R, assuming that all of R is in A.
//***************************************************************************

static APLCHAR DydHeader[] =
  $Z $IS $L L" " $F L" " $R L";" $QUAD_IO L";" $O;

static APLCHAR DydLine1[] =
  $O $IS $QUAD_IO
  $DIAMOND $QUAD_IO $IS L"0";

static APLCHAR DydLine2[] =
  $Z $IS $ENCLOSE L"[0]" $O L"+(1+" $RHO $L L")" $ENCODE L"(" $NEG L"1" $DROP L",(1+" $RHO $L L")" $TAKE $L L")" $IOTA $R;

static LPAPLCHAR DydBody[] =
{DydLine1,
 DydLine2,
};

MAGIC_FUNCTION MF_DydIota =
{DydHeader,
 DydBody,
 itemsizeof (DydBody),
};


//***************************************************************************
//  End of File: pf_iota.c
//***************************************************************************
