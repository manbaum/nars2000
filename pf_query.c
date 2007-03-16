//***************************************************************************
//  NARS2000 -- Primitive Function -- Query
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>
#include <math.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "primspec.h"
#include "sysvars.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


#ifndef PROTO
PRIMSPEC PrimSpecQuery =
{
    // Monadic functions
    &PrimFnMon_EM,
    &PrimSpecQueryStorageTypeMon,
    NULL,   // &PrimFnMonQueryAPA_EM, -- Can't happen w/Query

    NULL,   // &PrimFnMonQueryBisB, -- Can't happen w/Query
    NULL,   // &PrimFnMonQueryBisI, -- Can't happen w/Query
    NULL,   // &PrimFnMonQueryBisF, -- Can't happen w/Query

////               IisB,     // Handled via type promotion (to IisI)
    &PrimFnMonQueryIisI,
    &PrimFnMonQueryIisF,

////               FisB,     // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonQueryFisI, -- Can't happen w/Query
    NULL,   // &PrimFnMonQueryFisF, -- can't happen w/Query

    // Dyadic functions
    NULL,   // &PrimFnDyd_EM, -- Can't happen w/Query
    NULL,   // &PrimSpecQueryStorageTypeDyd, -- Can't happen w/Query
    NULL,   // &PrimFnDydQueryAPA_EM, -- Can't happen w/Query

    NULL,   // &PrimFnDydQueryBisBvB, -- Can't happen w/Query
    NULL,   // &PrimFnDydQueryBisIvI, -- Can't happen w/Query
    NULL,   // &PrimFnDydQueryBisFvF, -- Can't happen w/Query
    NULL,   // &PrimFnDydQueryBisCvC, -- Can't happen w/Query

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydQueryIisIvI,
    NULL,   // &PrimFnDydQueryIisFvF, -- Can't happen w/Query

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydQueryFisIvI, -- Can't happen w/Query
    NULL,   // &PrimFnDydQueryFisFvF, -- Can't happen w/Query

    // Miscellaneous
    &ExecCode,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecQuery};
#endif


//***************************************************************************
//  PrimFnQuery_EM
//
//  Primitive function for monadic and dyadic Query ("roll" and "deal")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnQuery_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnQuery_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_QUERY);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return PrimFnDydQuery_EM           (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnQuery_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimSpecQueryStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecQueryStorageTypeMon
    (APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht,
     LPTOKEN    lptkFunc)

{
    APLSTYPE aplTypeRes;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (aplNELMRht EQ 0 && *lpaplTypeRht EQ ARRAY_CHAR)
        *lpaplTypeRht = ARRAY_BOOL;

    if (*lpaplTypeRht EQ ARRAY_CHAR
     || *lpaplTypeRht EQ ARRAY_LIST)
        return ARRAY_ERROR;

    // The storage type of the result is
    //   the same as that of the right arg
    //   except SimpleNum goes to INT
    if (IsSimpleNum (*lpaplTypeRht))
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = *lpaplTypeRht;
    return aplTypeRes;
} // End PrimSpecQueryStorageTypeMon


//***************************************************************************
//  PrimFnMonQueryIisI
//
//  Primitive scalar function monadic Query:  I {is} fn I
//***************************************************************************

APLINT PrimFnMonQueryIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for DOMAIN ERROR
    if (aplIntegerRht < bQuadIO)
        RaiseException (EXEC_DOMAIN_ERROR, 0, 0, NULL);

    // Calculate new QuadRL
    uQuadRL = (uQuadRL * DEF_QUADRL_CWS) % QUADRL_MODULUS;

    // Save uQuadRL back into the system variable
    // ***FIXME*** -- do this in PostProcessing
    lpSymQuadRL->stData.stInteger = uQuadRL;

    // Reduce the argument if too large
    if (aplIntegerRht > QUADRL_MODULUS)
        aplIntegerRht = AplModI (aplIntegerRht, QUADRL_MODULUS);

    // Ye old Linear Congruential Generator
    return bQuadIO + (uQuadRL * aplIntegerRht) / QUADRL_MODULUS;
} // End PrimFnMonQueryIisI


//***************************************************************************
//  PrimFnMonQueryIisF
//
//  Primitive scalar function monadic Query:  I {is} fn F
//***************************************************************************

APLINT PrimFnMonQueryIisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for DOMAIN ERROR
    if (aplFloatRht < bQuadIO
     || aplFloatRht NE floor (aplFloatRht)
     || aplFloatRht >= Float2Pow53)
        RaiseException (EXEC_DOMAIN_ERROR, 0, 0, NULL);

    // Calculate new QuadRL
    uQuadRL = (uQuadRL * DEF_QUADRL_CWS) % QUADRL_MODULUS;

    // Save uQuadRL back into the system variable
    // ***FIXME*** -- do this in PostProcessing
    lpSymQuadRL->stData.stInteger = uQuadRL;

    // Reduce the argument if too large
    if (aplFloatRht > QUADRL_MODULUS)
        aplFloatRht = AplModF (aplFloatRht, QUADRL_MODULUS);

    // Ye old Linear Congruential Generator
    return bQuadIO + (uQuadRL * (APLINT) aplFloatRht) / QUADRL_MODULUS;
} // End PrimFnMonQueryIisF


//***************************************************************************
//  PrimFnDydQuery_EM
//
//  Primitive function for dyadic Query ("deal")
//
//  This algorithm was taken from a paper by R. A. Kelley,
//    "APLGOL, an Experimental Structured Programming Language",
//    IBM Journal of Research and Delvelopment, January 1973, pp. 69-73.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydQuery_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDydQuery_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    APLSTYPE aplTypeLft,
             aplTypeRht;
    APLNELM  aplNELMLft,
             aplNELMRht;
    APLRANK  aplRankLft,
             aplRankRht;
    HGLOBAL  hGlbLft = NULL,
             hGlbRht = NULL,
             hGlbRes = NULL;
    LPVOID   lpMemLft = NULL,
             lpMemRht = NULL;
    LPAPLINT lpMemRes = NULL;
    APLINT   aplIntegerLft,
             aplIntegerRht;
    APLFLOAT aplFloatLft,
             aplFloatRht;
    APLUINT  ByteRes;
    APLINT   uLft,
             uRht,
             uTmp,
             uSub;
    BOOL     bRet = TRUE;
    UINT     YYLclIndex;

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

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Check for LEFT/RIGHT RANK ERRORs
    if (aplRankLft > 1
     || aplRankRht > 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkLftArg);
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF

    // Check for LEFT/RIGHT LENGTH ERRORs
    if (aplNELMLft NE 1
     || aplNELMRht NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkLftArg);
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF

    // Get the respective first values
    FirstValue (lptkLftArg, &aplIntegerLft, &aplFloatLft, NULL, NULL, NULL, NULL, NULL);
    FirstValue (lptkRhtArg, &aplIntegerRht, &aplFloatRht, NULL, NULL, NULL, NULL, NULL);

    // Check for LEFT/RIGHT DOMAIN ERRORs
    bRet = (aplTypeLft NE ARRAY_CHAR
         && aplTypeRht NE ARRAY_CHAR);
    if (bRet)
    {
        if (aplTypeLft EQ ARRAY_FLOAT)
            aplIntegerLft = FloatToAplint_SCT (aplFloatLft, &bRet);
        if (bRet && aplTypeRht EQ ARRAY_FLOAT)
            aplIntegerRht = FloatToAplint_SCT (aplFloatRht, &bRet);
    } // End IF

    if (!bRet
     || aplIntegerLft < 0
     || aplIntegerRht < 0)
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkLftArg);
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF

    // Calculate space needed for the result
    // If we're using the brute force algorithm,
    //   allocate only what is needed
    if (aplIntegerLft < (aplIntegerRht / 16))
        ByteRes = CalcArraySize (ARRAY_INT, aplIntegerLft, 1);
    else
    // Otherwise, allocate space for aplIntegerRht integers,
    //   and downsize it at the end.
        ByteRes = CalcArraySize (ARRAY_INT, aplIntegerRht, 1);

    // Allocate space for the result
    // N.B. Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF

    // Get new index into YYRes
    YYLclIndex = NewYYResIndex ();

    // Fill in the result token
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
////YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;    // Already zero from ZeroMemory
////YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;    // Already zero from ZeroMemory
    YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    YYRes[YYLclIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->Perm       = 0;           // Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplIntegerLft;
    lpHeader->Rank       = 1;

#undef  lpHeader

    // Skip over the header to the dimension
    *VarArrayBaseToDim (lpMemRes) = aplIntegerLft;

    // Skip over the header and dimension to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // Split cases based upon the ratio of the left to right arg
    if (aplIntegerLft < (aplIntegerRht / 16))
    {
        // The short case
        for (uLft = 0; uLft < aplIntegerLft; uLft++)
        {
            // Get a random number
            uTmp = PrimFnMonQueryIisI (aplIntegerRht, NULL);

            // Search for it in the existing set
            for (uSub = 0; uSub < uLft; uSub++)
            if (uTmp EQ lpMemRes[uSub])
                break;

            // If not found, save it in the result
            if (uSub EQ uLft)
                lpMemRes[uLft] = uTmp;
            else
                uLft--;
        } // End FOR
    } else
    {
        // The long case

        // File the result with {iota}aplIntegerRht
        for (uLft = 0; uLft < aplIntegerRht; uLft++)    // Z {is} {iota} R
            lpMemRes[uLft] = uLft + bQuadIO;

        // Loop through the elements
        for (uLft = 0; uLft < aplIntegerLft; uLft++)    // :for I :in ({iota} L) - {quad}IO
        {
                                                        // J {is} I + (?R - I) - {quad}IO
            uRht = uLft + PrimFnMonQueryIisI (aplIntegerRht - uLft, NULL) - bQuadIO;
            uTmp           = lpMemRes[uLft];            // Z[{quad}IO + I J] {is} Z[{quad}IO + J I]
            lpMemRes[uLft] = lpMemRes[uRht];
            lpMemRes[uRht] = uTmp;
        } // End IF                                     // :endfor

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

        // Reallocate the memory down to the value of the left arg
        hGlbRes =
        MyGlobalReAlloc (hGlbRes,
                         MyGlobalSize (hGlbRes) - (UINT) (aplIntegerRht - aplIntegerLft) * sizeof (APLINT),
                         GHND);
        YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    } // End IF/ELSE
ERROR_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

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

    if (bRet)
        return &YYRes[YYLclIndex];
    else
        return NULL;
} // End PrimFnDydQuery_EM


//***************************************************************************
//  End of File: pf_query.c
//***************************************************************************
