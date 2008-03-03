//***************************************************************************
//  NARS2000 -- Primitive Function -- DownTack
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
//  $PrimFnDownTack_EM_YY
//
//  Primitive function for monadic and dyadic DownTack (ERROR and "encode/representation")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDownTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDownTack_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DOWNTACK);

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
        return PrimFnMonDownTack_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydDownTack_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnDownTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnDownTack_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic DownTack
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnDownTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnDownTack_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnDownTack_EM_YY,   // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnDownTack_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonDownTack_EM_YY
//
//  Primitive function for monadic DownTack (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonDownTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonDownTack_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc);
} // End PrimFnMonDownTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydDownTack_EM_YY
//
//  Primitive function for dyadic DownTack ("encode/representation")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydDownTack_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydDownTack_EM_YY
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
    APLDIM       aplColsLft,        // Left arg # cols
                 aplRowsLft,        // Left arg # rows
                 aplRestLft;        // Left arg product of remaining dimensions
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
                 uLftCol,           // Loop counter
                 uLftRst,           // Loop counter
                 uRht;              // Loop counter
    APLINT       iLftRow;           // Loop counter
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

    // Calc result NELM, Rank, & Type
    aplNELMRes = aplNELMLft * aplNELMRht;
    aplRankRes = aplRankLft + aplRankRht;
    if (aplNELMRes EQ 0)
        aplTypeRes = ARRAY_BOOL;
    else
    if (aplTypeLft EQ ARRAY_FLOAT
     || aplTypeRht EQ ARRAY_FLOAT)
        aplTypeRes = ARRAY_FLOAT;
    else
        aplTypeRes = ARRAY_INT;

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
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
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
    //   by copying the left arg dimensions
    //   and then the right arg dimensions
    if (hGlbLft)
    {
        CopyMemory (lpMemDimRes, lpMemDimLft, (UINT) aplRankLft * sizeof (APLDIM));

        // Skip over the copied dimensions
        lpMemDimRes += aplRankLft;

        // Calc the # rows in the left arg
        if (aplRankLft > 1)
            aplRowsLft = (VarArrayBaseToDim (lpMemLft))[aplRankLft - 2];
        else
        {
            aplRowsLft = aplColsLft;
            aplColsLft = 1;
        } // End IF/ELSE

        // Calc the product of remaining dimensions
        if (aplRowsLft NE 0
         && aplColsLft NE 0)
            aplRestLft = aplNELMLft / (aplRowsLft * aplColsLft);

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
    } else
    {
        aplRowsLft = 1;
        lpMemLft = &aplLongestLft;
    } // End IF/ELSE

    if (hGlbRht)
    {
        CopyMemory (lpMemDimRes, lpMemDimRht, (UINT) aplRankRht * sizeof (APLDIM));

        // Skip over the copied dimensions
        lpMemDimRes += aplRankRht;

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
    } else
        lpMemRht = &aplLongestRht;

    // Check for empty result
    if (aplNELMRes EQ 0)
        goto YYALLOC_EXIT;

    // Skip over the dimensions to the data
    lpMemRes = lpMemDimRes;

    // Trundle through the right arg
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Get the next right arg value
        if (hGlbRht)
            GetNextValueGlb (hGlbRht, uRht, NULL, &aplLongestRht, NULL);

        // The left arg is treated as a three-dimensional array of shape
        //   aplRestLft aplRowsLft aplColsLft

        // Trundle through the left arg remaning dimensions & cols
        for (uLftRst = 0; uLftRst < aplRestLft; uLftRst++)
        for (uLftCol = 0; uLftCol < aplColsLft; uLftCol++)
        {
            APLFLOAT aplFloatRht;
            APLINT   aplIntRht;

            if (aplTypeRes EQ ARRAY_FLOAT)
            {
                // If the right arg is int, convert it to float
                if (IsSimpleInt (aplTypeRht))
                    aplFloatRht = (APLFLOAT) (APLINT) aplLongestRht;
                else
                    aplFloatRht = *(LPAPLFLOAT) &aplLongestRht;
            } else
                aplIntRht = aplLongestRht;

            // Trundle through the left arg rows from back to front
            for (iLftRow = aplRowsLft - 1; iLftRow >= 0; iLftRow--)
            {
                APLUINT  uLft,
                         uRes;

                // Get left arg index
                uLft = 1 * uLftCol + aplColsLft * (1 * iLftRow + aplRowsLft * uLftRst);

                // Get result index
                uRes = 1 * uRht + aplNELMRht * uLft;

                if (aplTypeRes EQ ARRAY_FLOAT)
                {
                    APLFLOAT aplFloatLft,
                             aplFloatRes;

                    // If the left arg is int, convert it to float
                    aplFloatLft = GetNextFloat (lpMemLft, aplTypeLft, uLft);

                    // Calculate the result item
                    aplFloatRes = AplModF (aplFloatLft, aplFloatRht);

                    // Save in the result
                    ((LPAPLFLOAT) lpMemRes)[uRes] = aplFloatRes;

                    // If the modulus is zero, we're finished with this row
                    if (aplFloatLft EQ 0)
                        break;
                    // Subtract from the right arg item
                    aplFloatRht = (aplFloatRht - aplFloatRes) / aplFloatLft;
                } else
                {
                    APLINT aplIntLft,
                           aplIntRes;

                    // Get the left arg int
                    if (hGlbLft)
                        aplIntLft = GetNextInteger (lpMemLft, aplTypeLft, uLft);
                    else
                        aplIntLft = aplLongestLft;

                    // Calculate the result item
                    aplIntRes = AplModI (aplIntLft, aplIntRht);

                    // Save in the result
                    ((LPAPLINT)   lpMemRes)[uRes] = aplIntRes;

                    // If the modulus is zero, we're finished with this row
                    if (aplIntLft EQ 0)
                        break;
                    // Subtract from the right arg item
                    aplIntRht = (aplIntRht - aplIntRes) / aplIntLft;
                } // End IF/ELSE
            } // End FOR
        } // End FOR/FOR
    } // End FOR
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
} // End PrimFnDydDownTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_dtack.c
//***************************************************************************
