//***************************************************************************
//  NARS2000 -- Primitive Function -- Domino
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "gsl/gsl_matrix_double.h"
#include "gsl/gsl_permutation.h"
#include "gsl/gsl_linalg.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  PrimFnDomino_EM
//
//  Primitive function for monadic and dyadic Domino ("matrix inverse" and "matrix divide")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDomino_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDomino_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_DOMINO);

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
        return PrimFnMonDomino_EM             (lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydDomino_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnDomino_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonDomino_EM
//
//  Primitive function for monadic Domino (matrix inverse)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonDomino_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonDomino_EM
    (LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    BOOL     bRet = TRUE;
    APLSTYPE aplTypeRht;
    APLNELM  aplNELMRht,
             aplNELMRes;
    APLRANK  aplRankRht,
             aplRankRes;
    HGLOBAL  hGlbRht = NULL,
             hGlbRes = NULL;
    LPVOID   lpMemRht = NULL,
             lpMemRes = NULL;
    APLUINT  ByteRes;
    APLDIM   uNumRows,
             uNumCols,
             uCol;
    APLINT   apaOff,
             apaMul;
    APLFLOAT aplFloatRht;
    UINT     YYLclIndex,
             uBitMaskRht;
    gsl_matrix      *lpGslMatrixRht,
                    *lpGslMatrixRes;
    gsl_permutation *lpGslPerm;
    int              GslSignum;
    int              ErrCode;

////DbgBrk ();          // ***TESTME***

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Check for RANK ERROR
    if (aplRankRht > 2)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Check for LENGTH ERROR
    if (hGlbRht)
    {
        if(aplRankRht EQ 2
         && (VarArrayBaseToDim (lpMemRht))[0] < (VarArrayBaseToDim (lpMemRht))[1])
        {
            ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkFunc);
            goto ERROR_EXIT;
        } // End IF
    } // End IF

    // Check for DOMAIN ERROR
    if (!IsSimpleNum (aplTypeRht))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // The rank of the result is the same as max (rank of the right arg, 1)
    aplRankRes = max (aplRankRht, 1);

    // Calculate the # rows & cols in the result
    switch (aplRankRht)
    {
        case 0:
            uNumRows =
            uNumCols = 1;

            break;

        case 1:
            uNumRows = 1;
            uNumCols = *VarArrayBaseToDim (lpMemRht);

            break;

        case 2:
            uNumRows = (VarArrayBaseToDim (lpMemRht))[0];
            uNumCols = (VarArrayBaseToDim (lpMemRht))[1];

            break;

        defstop
            break;
    } // End SWITCH

    // ***FIXME***
    if (uNumRows NE uNumCols)
    {
        ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Save the NELM of the result
    aplNELMRes = uNumRows * uNumCols;

    // Calculate the space needed for the result
    ByteRes = CalcArraySize (ARRAY_FLOAT, aplNELMRes, aplRankRes);

    // Allocate space for the result
    // N.B.: Conversion from APLUINT to UINT.
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

    // Fill in the header values
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_FLOAT;
////lpHeader->Perm       = 0;       // Already zero from GHND
////lpHeader->SysVar     = 0;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;

#undef  lpHeader

    // Fill in the dimensions
    if (aplRankRes EQ 1)
        *VarArrayBaseToDim (lpMemRes) = uNumCols;
    else
    {
        (VarArrayBaseToDim (lpMemRes))[0] = uNumRows;
        (VarArrayBaseToDim (lpMemRes))[1] = uNumCols;
    } // End IF/ELSE

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    // Allocate space for the GSL matrices
    Assert (uNumRows EQ (UINT) uNumRows);
    Assert (uNumCols EQ (UINT) uNumCols);
    lpGslMatrixRht = gsl_matrix_alloc ((UINT) uNumRows, (UINT) uNumCols);
    lpGslMatrixRes = gsl_matrix_alloc ((UINT) uNumRows, (UINT) uNumCols);

    // Copy the right arg to the GSL matrix
    // Split cases based upon the right arg's rank
    switch (aplRankRht)
    {
        case 0:
            FirstValue (lptkRhtArg, NULL, &aplFloatRht, NULL, NULL, NULL, NULL);
            lpGslMatrixRht->data[0] = aplFloatRht;

            break;

        case 1:
        case 2:
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:
                    uBitMaskRht = 0x01;

                    for (uCol = 0; uCol < aplNELMRes; uCol++)
                    {
                        lpGslMatrixRht->data[uCol] = (uBitMaskRht & *(LPAPLBOOL) lpMemRht) ? 1 : 0;

                        // Shift over the right bit mask
                        uBitMaskRht <<= 1;

                        // Check for end-of-byte
                        if (uBitMaskRht EQ END_OF_BYTE)
                        {
                            uBitMaskRht = 0x01;         // Start over
                            ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:
                    for (uCol = 0; uCol < aplNELMRes; uCol++)
                        lpGslMatrixRht->data[uCol] = (APLFLOAT) ((LPAPLINT) lpMemRht)[uCol];
                    break;

                case ARRAY_FLOAT:
                    for (uCol = 0; uCol < aplNELMRes; uCol++)
                        lpGslMatrixRht->data[uCol] = ((LPAPLFLOAT) lpMemRht)[uCol];
                    break;

                case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemRht)
                    apaOff = lpAPA->Off;
                    apaMul = lpAPA->Mul;
#undef  lpAPA
                    for (uCol = 0; uCol < aplNELMRes; uCol++)
                        lpGslMatrixRht->data[uCol] = (APLFLOAT) (APLINT) (apaOff + apaMul * uCol);
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH

    // ***FIXME*** -- Should this be aplNELMRes instead of uNumCols??
////lpGslPerm = gsl_permutation_alloc ((UINT) uNumCols);
    lpGslPerm = gsl_permutation_alloc ((UINT) aplNELMRes);

    // No aborting on error!
    gsl_set_error_handler_off ();

////DbgBrk ();          // ***TESTME***

    ErrCode = gsl_linalg_LU_decomp (lpGslMatrixRht, lpGslPerm, &GslSignum);

    ErrCode = gsl_linalg_LU_invert (lpGslMatrixRht, lpGslPerm, lpGslMatrixRes);

    // Copy the GSL data to the result
    Assert (sizeof (double) EQ sizeof (APLFLOAT));
    ByteRes = aplNELMRes * sizeof (APLFLOAT);
    Assert (ByteRes EQ (UINT) ByteRes);
    CopyMemory (lpMemRes, lpGslMatrixRes->data, (UINT) ByteRes);

    // Free the permutation
    gsl_permutation_free (lpGslPerm);

    // Free the GSL matrix
    gsl_matrix_free (lpGslMatrixRht);

    // Fill in the result token
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
    YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;
    YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYLclIndex].tkToken.tkFlags.Color     =
    YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    YYRes[YYLclIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

ERROR_EXIT:
    bRet = FALSE;
NORMAL_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        if (!bRet)
        {
            // We no longer need this storage
            DbgGlobalFree (hGlbRes); hGlbRes = NULL;
        } // End IF
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
} // End PrimFnMonDomino_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydDomino_EM
//
//  Primitive function for dyadic Domino (matrix divide)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydDomino_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDydDomino_EM
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
             hGlbRht = NULL;
    LPVOID   lpMemLft = NULL,
             lpMemRht = NULL;
    BOOL     bRet = TRUE;
    UINT     YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Check for RANK ERRORs
    if (aplRankLft > 2 || aplRankRht > 2)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // ***FINISHME***
////DbgBrk ();

    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;










    goto NORMAL_EXIT;

ERROR_EXIT:
    bRet = FALSE;
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

    if (bRet)
        return &YYRes[YYLclIndex];
    else
        return NULL;
} // End PrimFnDydDomino_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_domino.c
//***************************************************************************
