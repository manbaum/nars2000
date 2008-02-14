//***************************************************************************
//  NARS2000 -- Primitive Function -- UpTack
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $PrimFnUpTack_EM_YY
//
//  Primitive function for monadic and dyadic UpTack (ERROR and "decode/base value")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnUpTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnUpTack_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_UPTACK);

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
        return PrimFnMonUpTack_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydUpTack_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnUpTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnUpTack_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic UpTack
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnUpTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnUpTack_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnUpTack_EM_YY,   // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnUpTack_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonUpTack_EM_YY
//
//  Primitive function for monadic UpTack (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonUpTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonUpTack_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc);
} // End PrimFnMonUpTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydUpTack_EM_YY
//
//  Primitive function for dyadic UpTack ("decode/base value")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydUpTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydUpTack_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeLft,        // Left arg storage type
                 aplTypeRht,        // Right ...
                 aplTypeRes;        // Result   ...
    APLNELM      aplNELMLft,        // Left arg NELM
                 aplNELMRht,        // Right ...
                 aplNELMRes;        // Result   ...
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankRht,        // Right ...
                 aplRankRes;        // Result   ...
    APLDIM       aplColsLft,        // Left arg last dim
                 aplRestLft,        // Left arg product of remaining dims
                 aplFrstRht,        // Right arg 1st dim
                 aplRestRht,        // Right arg product of remaining dims
                 aplInnrMax;        // Larger of inner dimensions
    APLLONGEST   aplLongestLft,     // Left arg immediate value
                 aplLongestRht;     // Right ...
    HGLOBAL      hGlbLft = NULL,    // Left arg global memory handle
                 hGlbRht = NULL,    // Right ...
                 hGlbRes = NULL;    // Result   ...
    LPVOID       lpMemLft = NULL,   // Ptr to left arg global memory
                 lpMemRht = NULL,   // Ptr to right ...
                 lpMemRes = NULL;   // Ptr to result   ...
    LPAPLDIM     lpMemDimLft,       // Ptr to left arg dimensions
                 lpMemDimRht,       // Ptr to right ...
                 lpMemDimRes;       // Ptr to result   ...
    APLUINT      ByteRes,           // # bytes in the result
                 uRes,              // Loop counter
                 uOutLft,           // Loop counter
                 uOutRht,           // Loop counter
                 uDimCopy;          // # dimensions to copy
    APLINT       iInnMax;           // Loop counter
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result

    // Get the attributes (Type,NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, &aplColsLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for LEFT & RIGHT DOMAIN ERRORs
    if (!IsSimpleNum (aplTypeLft)
     || !IsSimpleNum (aplTypeRht))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Get left & right arg global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Calculate length of right arg first dimension
    if (hGlbRht && aplRankRht > 0)
        aplFrstRht = *VarArrayBaseToDim (lpMemRht);
    else
        aplFrstRht = 1;

    // Check for LENGTH ERROR
    if (aplColsLft NE aplFrstRht
     && aplColsLft NE 1
     && aplFrstRht NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Calc larger of inner dimensions (in case of scalar extension)
    aplInnrMax = max (aplColsLft, aplFrstRht);

    // Calc product of the remaining dimensions in left arg
    if (aplColsLft)
        aplRestLft = aplNELMLft / aplColsLft;
    else
    for (aplRestLft = 1, uOutLft = 0; uOutLft < (aplRankLft - 1); uOutLft++)
        aplRestLft *= (VarArrayBaseToDim (lpMemLft))[uOutLft];

    // Calc product of the remaining dimensions in right arg
    if (aplFrstRht)
        aplRestRht = aplNELMRht / aplFrstRht;
    else
    for (aplRestRht = 1, uOutRht = 1; uOutRht < aplRankRht; uOutRht++)
        aplRestRht *= (VarArrayBaseToDim (lpMemRht))[uOutRht];

    // Calc result rank
    aplRankRes = max (aplRankLft, 1) + max (aplRankRht, 1) - 2;

    // Calc result NELM
    aplNELMRes = aplRestLft * aplRestRht;

    // Check for empty result
    if (aplNELMRes EQ 0)
        // Calc result Type
        aplTypeRes = ARRAY_BOOL;
    else
    // Calc result Type
    if (aplTypeLft EQ ARRAY_FLOAT
     || aplTypeRht EQ ARRAY_FLOAT)
        aplTypeRes = ARRAY_FLOAT;
    else
        aplTypeRes = ARRAY_INT;
RESTART_EXCEPTION:
    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

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
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;               // Already zero from GHND
////lpHeader->SysVar     = 0;               // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    if (hGlbLft)
        lpMemDimLft = VarArrayBaseToDim (lpMemLft);
    if (hGlbRht)
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);
    lpMemDimRes = VarArrayBaseToDim (lpMemRes);

    // Fill in the result's dimension
    //   by copying the left arg dimensions (except for the last)
    //   and then the right arg dimensions  (except for the first)

    // If the left arg is an array, ...
    if (hGlbLft)
    {
        // Calc # dimensions to copy
        uDimCopy = max (aplRankLft, 1) - 1;

        // Copy the dimensions
        CopyMemory (lpMemDimRes, lpMemDimLft, (UINT) uDimCopy * sizeof (APLDIM));

        // Skip over the copied dimensions
        lpMemDimRes += uDimCopy;
    } // End IF

    // If the right arg is an array, ...
    if (hGlbRht)
    {
        // Calc # dimensions to copy
        uDimCopy = max (aplRankRht, 1) - 1;

        // Copy the dimensions
        CopyMemory (lpMemDimRes, &lpMemDimRht[1], (UINT) uDimCopy * sizeof (APLDIM));

        // Skip over the copied dimensions
        lpMemDimRes += uDimCopy;
    } // End IF

    // Check for empty result
    if (aplNELMRes EQ 0)
        goto YYALLOC_EXIT;

    // Skip over the dimensions to the data
    lpMemRes = lpMemDimRes;

    // The left arg is treated as a two-dimensional array of shape
    //   aplRestLft aplColsLft

    // The right arg is treated as a two-dimensional array of shape
    //   aplFrstRht aplRestRht

    // Trundle through the left & right arg remaining dimensions
    for (uOutLft = 0; uOutLft < aplRestLft; uOutLft++)
    for (uOutRht = 0; uOutRht < aplRestRht; uOutRht++)
    {
        APLINT   aplIntAcc,
                 InnValInt;
        APLFLOAT aplFloatAcc,
                 InnValFlt;

        // Calc result index
        uRes = 1 * uOutRht + aplRestRht * uOutLft;

        // Initialize accumulator and weighting value
        aplIntAcc   = 0;
        aplFloatAcc = 0.0;
        InnValInt = 1;
        InnValFlt = 1.0;

        // Trundle through the inner dimensions, back to front
        for (iInnMax = aplInnrMax - 1; iInnMax >= 0; iInnMax--)
        {
            APLUINT uInnLft,            // Index into left arg
                    uInnRht;            // ...        right ...

            // Calc left inner index, taking into account scalar extension
            if (aplColsLft EQ 1)
                uInnLft = 1 * 0       + aplColsLft * uOutLft;
            else
                uInnLft = 1 * iInnMax + aplColsLft * uOutLft;

            // Calc right inner index, taking into account scalar extension
            if (aplFrstRht EQ 1)
                uInnRht = 1 * uOutRht + aplRestRht * 0      ;
            else
                uInnRht = 1 * uOutRht + aplRestRht * iInnMax;

            if (aplTypeRes EQ ARRAY_FLOAT)
            {
                APLFLOAT aplFloatLft,
                         aplFloatRht;

                // If the right arg is an array, ...
                if (hGlbRht)
                    // Get the next right arg value
                    GetNextValueGlb (hGlbRht, uInnRht, NULL, &aplLongestRht, NULL);

                // If the right arg is int, convert it to float
                if (IsSimpleInt (aplTypeRht))
                    aplFloatRht = (APLFLOAT) (APLINT) aplLongestRht;
                else
                    aplFloatRht = *(LPAPLFLOAT) &aplLongestRht;

                // Add into accumulator
                aplFloatAcc += InnValFlt * aplFloatRht;

                // If the left arg is an array, ...
                if (hGlbLft)
                    // Get the next left arg value
                    GetNextValueGlb (hGlbLft, uInnLft, NULL, &aplLongestLft, NULL);

                // If the left arg is int, convert it to float
                if (IsSimpleInt (aplTypeLft))
                    aplFloatLft = (APLFLOAT) (APLINT) aplLongestLft;
                else
                    aplFloatLft = *(LPAPLFLOAT) &aplLongestLft;

                // Multiply into the weighting value
                InnValFlt *= aplFloatLft;
            } else
            {
                // Get the next right arg value
                if (hGlbRht)
                    GetNextValueGlb (hGlbRht, uInnRht, NULL, &aplLongestRht, NULL);

                __try
                {
                    // Add into accumulator
                    aplIntAcc = iadd64 (aplIntAcc, imul64 (InnValInt, aplLongestRht, NULL), NULL);

                    // Get the next left arg value
                    if (hGlbLft)
                        GetNextValueGlb (hGlbLft, uInnLft, NULL, &aplLongestLft, NULL);

                    // Multiply into the weighting value
                    InnValInt = imul64 (InnValInt, aplLongestLft, NULL);
                } __except (CheckException (GetExceptionInformation (), "PrimFnDydUpTack_EM_YY"))
                {
                    switch (MyGetExceptionCode ())
                    {
                        case EXCEPTION_RESULT_FLOAT:
                            MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                            if (aplTypeRes NE ARRAY_FLOAT)
                            {
                                aplTypeRes = ARRAY_FLOAT;
#ifdef DEBUG
                                dprintfW (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
                                // We no longer need these ptrs
                                MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

                                // We no longer need this storage
                                FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;

                                goto RESTART_EXCEPTION;
                            } // End IF

                            // Fall through to never-never-land

                        default:
                            // Display message for unhandled exception
                            DisplayException ();
#ifdef DEBUG
                            DbgStop ();         // We should never get here
#endif
                            break;
                    } // End SWITCH
                } // End __try/__except
            } // End IF/ELSE
        } // End FOR

        // Save in result
        if (aplTypeRes EQ ARRAY_FLOAT)
            ((LPAPLFLOAT) lpMemRes)[uRes] = aplFloatAcc;
        else
            ((LPAPLINT)   lpMemRes)[uRes] = aplIntAcc;
    } // End FOR/FOR
YYALLOC_EXIT:
    // Unlock the result global memory in case TypeDemote actually demotes
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);
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

    return lpYYRes;
} // End PrimFnDydUpTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_utack.c
//***************************************************************************
