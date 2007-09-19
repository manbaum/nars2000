//***************************************************************************
//  NARS2000 -- Primitive Function -- DelStile & DeltaStile
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

typedef struct tagGRADEDATA
{
    APLSTYPE aplTypeRht;            // Right arg storage type
    APLUINT  aplNELMRest;           // Product of right arg dims after the first
    APLINT   apaOffRht,             // Right arg APA offset
             apaMulRht;             // Right arg APA multiplier
    int      iMul;                  // Multiplier for GradeUp vs. GradeDown
                                    //   1 for GradeUp, -1 for GradeDown
} GRADE_DATA, *LPGRADE_DATA;


//***************************************************************************
//  $PrimFnDeltaStile_EM_YY
//
//  Primitive function for monadic and dyadic DeltaStile
//    ("grade up numeric" and "grade up character")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDeltaStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDeltaStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DELTASTILE);

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
        return PrimFnMonGradeCom_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydGradeCom_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnDeltaStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDelStile_EM_YY
//
//  Primitive function for monadic and dyadic DelStile
//    ("grade down numeric" and "grade down character")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDelStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDelStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DELSTILE);

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
        return PrimFnMonGradeCom_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydGradeCom_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnDelStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnDeltaStile_EM_YY
//
//  Generate a prototype for the primitive functions
//    monadic & dyadic DeltaStile
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnDeltaStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnDeltaStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnDeltaStile_EM_YY,   // Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnDeltaStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnDelStile_EM_YY
//
//  Generate a prototype for the primitive functions
//    monadic & dyadic DelStile
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnDelStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnDelStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnDelStile_EM_YY,// Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnDelStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonGradeCom_EM_YY
//
//  Primitive function for monadic DelStile and DeltaStile
//    ("grade up/down numeric" and "grade up/down character")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonGradeCom_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonGradeCom_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLNELM      aplNELMRht,    // Right arg NELM
                 aplNELMRes;    // Result    ...
    APLRANK      aplRankRht;    // Right arg rank
    HGLOBAL      hGlbRht,       // Right arg global memory handle
                 hGlbRes;       // Result    ...
    APLUINT      ByteRes,       // # bytes in result
                 uRes;          // Loop counter
    LPVOID       lpMemRht,      // Ptr to right arg global memory
                 lpMemRes;      // Ptr to result    ...
    LPAPLDIM     lpMemDimRht;   // Ptr to right arg dimensions
    BOOL         bRet = TRUE;   // TRUE iff result is valid
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result
    APLBOOL      bQuadIO;       // []IO
    GRADE_DATA   gradeData;     // Data passed to GnomeSort

    // Mark as grade up or down
    gradeData.iMul = (lptkFunc->tkData.tkChar EQ UTF16_DELTASTILE) ? 1 : -1;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &gradeData.aplTypeRht, &aplNELMRht, &aplRankRht);

    // Check for RIGHT RANK ERROR
    if (aplRankRht EQ 0)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Check for RIGHT DOMAIN ERROR
    if (!IsSimpleNum (gradeData.aplTypeRht))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Get right arg global ptr
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Get the length of the first dimension (as result length)
    aplNELMRes = lpMemDimRht[0];

    // Calculate the length of the dimensions after the first one
    for (gradeData.aplNELMRest = 1, uRes = 1; uRes < aplRankRht; uRes++)
        gradeData.aplNELMRest *= lpMemDimRht[uRes];

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_INT, aplNELMRes, 1);

    // Allocate space for the result.
    // N.B. Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        // Mark as a WS FULL
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes     ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeaderRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeaderRes->ArrType    = ARRAY_INT;
////lpHeaderRes->Perm       = 0;        // Already zero from GHND
////lpHeaderRes->SysVar     = 0;        // Already zero from GHND
    lpHeaderRes->RefCnt     = 1;
    lpHeaderRes->NELM       = aplNELMRes;
    lpHeaderRes->Rank       = 1;

#undef  lpHeaderRes

    // Save the dimension in the result
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // If the right arg is an APA, ...
    if (gradeData.aplTypeRht EQ ARRAY_APA)
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        gradeData.apaOffRht = lpAPA->Off;
        gradeData.apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Start with {iota}aplNELMRes in lpMemRes
    for (uRes = 0; uRes < aplNELMRes; uRes++)
        ((LPAPLINT) lpMemRes)[uRes] = uRes;

    // Grade the array
    GnomeSort (lpMemRes,            // Ptr to result global memory
               lpMemRht,            // Ptr to right arg global memory
               aplNELMRes,          // Result NELM
              &PrimFnMonGradeComp,  // Ptr to comparison routine
              &gradeData);
    // Add in []IO
    for (uRes = 0; uRes < aplNELMRes; uRes++)
        ((LPAPLINT) lpMemRes)[uRes] += bQuadIO;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
ERROR_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnMonGradeCom_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonGradeComp
//
//  Comparison routine for PrimFnMonGradeCom
//***************************************************************************

APLINT PrimFnMonGradeComp
    (LPVOID       lpMemRht,         // Ptr to right arg array
     APLUINT      aplUIntLft,       // Left index
     APLUINT      aplUIntRht,       // Right index
     LPGRADE_DATA lpGradeData)      // Ptr to extra data

{
    APLUINT uRest;
    APLNELM aplNELMRest;

    // Get the Grade Data
    aplNELMRest = lpGradeData->aplNELMRest;

    // Split cases based upon the right arg storage type
    switch (lpGradeData->aplTypeRht)
    {
        case ARRAY_BOOL:
            // Compare the hyper-planes of the right arg
            for (uRest = 0; uRest < aplNELMRest; uRest++)
            {
                APLUINT aplBitLft,
                        aplBitRht;
                UINT    uBitMask;

                // Get the left hand indexed bit
                aplBitLft = aplUIntLft * aplNELMRest + uRest;
                uBitMask = 1 << ((int) (aplBitLft % NBIB));
                aplBitLft = (uBitMask & ((LPAPLBOOL) lpMemRht)[aplBitLft >> LOG2NBIB]) ? 1 : 0;

                // Get the right hand indexed bit
                aplBitRht = aplUIntRht * aplNELMRest + uRest;
                uBitMask = 1 << ((int) (aplBitRht % NBIB));
                aplBitRht = (uBitMask & ((LPAPLBOOL) lpMemRht)[aplBitRht >> LOG2NBIB]) ? 1 : 0;

                // Split cases based upon the signum of the difference
                switch (PrimFnMonTimesIisI (aplBitLft - aplBitRht,
                                            NULL))
                {
                    case 1:
                        return  1 * lpGradeData->iMul;

                    case 0:
                        break;

                    case -1:
                        return -1 * lpGradeData->iMul;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        case ARRAY_INT:
            // Compare the hyper-planes of the right arg
            for (uRest = 0; uRest < aplNELMRest; uRest++)
            // Split cases based upon the signum of the difference
            switch (PrimFnMonTimesIisI (((LPAPLINT) lpMemRht)[aplUIntLft * aplNELMRest + uRest]
                                      - ((LPAPLINT) lpMemRht)[aplUIntRht * aplNELMRest + uRest],
                                        NULL))
            {
                case 1:
                    return  1 * lpGradeData->iMul;

                case 0:
                    break;

                case -1:
                    return -1 * lpGradeData->iMul;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        case ARRAY_FLOAT:
            // Compare the hyper-planes of the right arg
            for (uRest = 0; uRest < aplNELMRest; uRest++)
            // Split cases based upon the signum of the difference
            switch (PrimFnMonTimesIisF (((LPAPLFLOAT) lpMemRht)[aplUIntLft * aplNELMRest + uRest]
                                      - ((LPAPLFLOAT) lpMemRht)[aplUIntRht * aplNELMRest + uRest],
                                        NULL))
            {
                case 1:
                    return  1 * lpGradeData->iMul;

                case 0:
                    break;

                case -1:
                    return -1 * lpGradeData->iMul;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        case ARRAY_APA:
            // Compare the hyper-planes of the right arg
            for (uRest = 0; uRest < aplNELMRest; uRest++)
            // Split cases based upon the signum of the difference
////////////switch (PrimFnMonTimesIisI ((lpGradeData->apaOffRht + lpGradeData->apaMulRht * (aplUIntLft * aplNELMRest + uRest))
////////////                          - (lpGradeData->apaOffRht + lpGradeData->apaMulRht * (aplUIntRht * aplNELMRest + uRest)),
////////////                            NULL))
            switch (PrimFnMonTimesIisI (lpGradeData->apaMulRht * (aplUIntLft - aplUIntRht),
                                        NULL))
            {
                case 1:
                    return  1 * lpGradeData->iMul;

                case 0:
                    break;

                case -1:
                    return -1 * lpGradeData->iMul;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        defstop
            break;
    } // End SWITCH

    return 0;
} // End PrimFnMonGradeComp


//***************************************************************************
//  $PrimFnDydGradeCom_EM_YY
//
//  Primitive function for dyadic DelStile and DeltaStile
//    ("grade up/down numeric")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydGradeCom_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydGradeCom_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeLft;        // Left arg storage type
    APLNELM      aplNELMLft,        // Left arg NELM
                 aplNELMRht;        // Right ...
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankRht;        // Right ...
    HGLOBAL      hGlbLft = NULL,    // Left arg global memory handle
                 hGlbRht = NULL,    // Right ...
                 hGlbRes = NULL;    // Result   ...
    LPVOID       lpMemLft = NULL,   // Ptr to left arg global memory
                 lpMemRht = NULL,   // Ptr to right ...
                 lpMemRes = NULL;   // Ptr to result   ...
    APLUINT      uLft;              // Loop counter
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    APLBOOL      bQuadIO;           // []IO
    GRADE_DATA   gradeData;         // Data passed to GnomeSort

    // Mark as grade up or down
    gradeData.iMul = (lptkFunc->tkData.tkChar EQ UTF16_DELTASTILE) ? 1 : -1;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &gradeData.aplTypeRht, &aplNELMRht, &aplRankRht);

    // Check for LEFT RANK ERROR
    if (aplRankLft EQ 0)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkLftArg);
        goto ERROR_EXIT;
    } // End IF

    // Check for RIGHT RANK ERROR
    if (aplRankRht EQ 0)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkRhtArg);
        goto ERROR_EXIT;
    } // End IF

    // Check for LEFT LENGTH ERROR
    if (aplNELMLft EQ 0)
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkLftArg);
        goto ERROR_EXIT;
    } // End IF

    // Check for RIGHT DOMAIN ERROR
    if (gradeData.aplTypeRht NE ARRAY_CHAR
     && aplNELMRht NE 0)
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkRhtArg);
        goto ERROR_EXIT;
    } // End IF

    // Check for LEFT DOMAIN ERROR
    if (aplTypeLft NE ARRAY_CHAR)
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkLftArg);
        goto ERROR_EXIT;
    } // End IF

    return PrimFnNonceError_EM (lptkFunc);

    DbgBrk ();          // ***FINISHME*** -- PrimFnDydGradeCom_EM_YY

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Allocate an array to hold the HGLOBALs of the translate tables



    // Allocate 128KB arrays for the translate tables,
    //   one per left arg dimension
    for (uLft = 0; uLft < aplRankLft; uLft++)
    {



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
} // End PrimFnDydGradeCom_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_deltastile.c
//***************************************************************************
