//***************************************************************************
//  NARS2000 -- Primitive Function -- DotDot
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

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
#include <math.h>
#include "headers.h"


//***************************************************************************
//  $PrimFnDotDot_EM_YY
//
//  Primitive function for monadic and dyadic DotDot (ERROR and "sequence")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDotDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDotDot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DOT);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonDotDot_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydDotDot_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnDotDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnDotDot_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic DotDot
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnDotDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnDotDot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnDotDot_EM_YY,     // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnDotDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonDotDot_EM_YY
//
//  Primitive function for monadic DotDot (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonDotDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonDotDot_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimFnMonDotDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydDotDot_EM_YY
//
//  Primitive function for dyadic DotDot ("sequence")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydDotDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydDotDot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeLft,        // Left arg storage type
                 aplTypeRht,        // Right ...
                 aplTypeRes;        // Result ...
    APLNELM      aplNELMLft,        // Left arg NELM
                 aplNELMRht,        // Right ...
                 aplNELMRes;        // Result ...
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankRht;        // Right ...
    APLLONGEST   aplLongestLft,     // left arg immediate value
                 aplLongestRht;     // Right ...
    APLINT       aplIntLft,         // Left arg start
                 aplIntStep;        // Left arg step as integer
    APLUINT      uCnt;              // Loop counter
    APLFLOAT     aplFltLft,         // Left arg as float
                 aplFltStep;        // Left arg step as float
    APLRAT       aplRatLft = {0},   // Left arg as RAT
                 aplRatStep = {0},  // Left arg step as RAT
                 aplRatTmp = {0};   // Temporary RAT
    APLVFP       aplVfpLft = {0},   // Left arg as VFP
                 aplVfpStep = {0},  // Left arg step as VFP
                 aplVfpTmp = {0};   // Temporary VFP
    APLUINT      ByteRes;           // # bytes in the result
    HGLOBAL      hGlbLft = NULL,    // Left arg global memory handle
                 hGlbRht = NULL,    // Right ...
                 hGlbRes = NULL;    // Result ...
    LPVOID       lpMemLft = NULL,   // Ptr to left arg global memory
                 lpMemRht = NULL,   // ...    right ...
                 lpMemRes = NULL;   // ...    result ...
    UBOOL        bRet = TRUE;       // TRUE iff the result is valid
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    ALLTYPES      atRht = {0};      // Right arg as ALLTYPES

    // 2..7   == 2 3 4 5 6 7
    // 2 2..7 == 2 4 6

    // Get the attributes (Type)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for LENGTH ERROR
    if (aplNELMLft NE 1
     && aplNELMLft NE 2)
        goto LEFT_LENGTH_EXIT;

    if (!IsSingleton (aplNELMRht))
        goto RIGHT_LENGTH_EXIT;

    // If either arg is nested, ...
    if (IsNested (aplTypeLft)
     || IsNested (aplTypeRht))
        return PrimFnDydDotDotNested_EM_YY (lptkLftArg, // Ptr to right arg token
                                            lptkFunc,   // Ptr to function token
                                            lptkRhtArg, // Ptr to right arg token
                                            lptkAxis);  // Ptr to axis token (may be NULL)
    // Get Left & right arg global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the left arg is a global, ...
    if (hGlbLft)
    {
        // Lock the memory to get a ptr to it
        lpMemLft = MyGlobalLock (hGlbLft);

        // Skip over the header and dimension
        lpMemLft = VarArrayDataFmBase (lpMemLft);
    } else
        lpMemLft = &aplLongestLft;

    // If the right arg is a global, ...
    if (hGlbRht)
    {
        // Lock the memory to get a ptr to it
        lpMemRht = MyGlobalLock (hGlbRht);

        // Skip over the header and dimension
        lpMemRht = VarArrayDataFmBase (lpMemRht);
    } else
        lpMemRht = &aplLongestRht;

    // Split cases based upon the left arg storage type
    switch (aplTypeLft)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            // Save as signed integer
            aplIntLft = aplLongestLft;

            // Save as result storage type
            aplTypeRes = ARRAY_APA;

            break;

        case ARRAY_FLOAT:
            // Save as float
            aplFltLft = *(LPAPLFLOAT) &aplLongestLft;

            // Save as result storage type
            aplTypeRes = ARRAY_FLOAT;

            break;

        case ARRAY_RAT:
            // Initialize temp vars
            mpq_init (&aplRatLft);
            mpq_init (&aplRatStep);
            mpq_init (&aplRatTmp);

            // Save as RAT
            mpq_set (&aplRatLft, (LPAPLRAT) lpMemLft);

            // Save as result storage type
            aplTypeRes = ARRAY_RAT;

            break;

        case ARRAY_VFP:
            // Initialize temp vars
            mpf_init (&aplVfpLft);
            mpf_init (&aplVfpStep);
            mpf_init (&aplVfpTmp);

            // Save as VFP
            mpf_set (&aplVfpLft, (LPAPLVFP) lpMemLft);

            // Save as result storage type
            aplTypeRes = ARRAY_VFP;

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            goto LEFT_DOMAIN_EXIT;

        defstop
            break;
    } // End SWITCH

    // If there's a step, ...
    if (!IsSingleton (aplNELMLft))
        // Split cases based upon the left arg storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                aplIntStep =
                  GetNextInteger (lpMemLft,     // Ptr to global memory
                                  aplTypeLft,   // Storage type
                                  1);           // Index
                break;

            case ARRAY_FLOAT:
                aplFltStep =
                  GetNextFloat (lpMemLft,       // Ptr to global memory
                                aplTypeLft,     // Storage type
                                1);             // Index
                break;

            case ARRAY_RAT:
                mpq_set (&aplRatStep, &((LPAPLRAT) lpMemLft)[1]);

                break;

            case ARRAY_VFP:
                mpf_set (&aplVfpStep, &((LPAPLVFP) lpMemLft)[1]);

                break;

            defstop
                break;
        } // End SWITCH
    else
        // Split cases based upon the left arg storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                aplIntStep = 1;

                break;

            case ARRAY_FLOAT:
                aplFltStep = 1;

                break;

            case ARRAY_RAT:
                mpq_set_ui (&aplRatStep, 1, 1);

                break;

            case ARRAY_VFP:
                mpf_set_ui (&aplVfpStep, 1);

                break;

            defstop
                break;
        } // End SWITCH

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_FLOAT:
        case ARRAY_RAT:
        case ARRAY_VFP:
            // Promote the right arg to the result type
            (*aTypeActConvert[aplTypeRht][(aplTypeRes EQ ARRAY_APA) ? ARRAY_INT : aplTypeRes])(lpMemRht, 0, &atRht);

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            goto RIGHT_DOMAIN_EXIT;

        defstop
            break;
    } // End SWITCH

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_APA:
            // Check for step 0
            if (aplIntStep EQ 0)
            {
                if (aplIntLft NE atRht.aplInteger)
                    goto LEFT_DOMAIN_EXIT;
                aplIntStep = 1;
            } // End IF

            // Set the sign of the step
            if (aplIntLft <= atRht.aplInteger)
                aplIntStep =  abs64 (aplIntStep);
            else
                aplIntStep = -abs64 (aplIntStep);
            // The NELM of the result is
            aplNELMRes = 1 + abs64 ((atRht.aplInteger - aplIntLft) / aplIntStep);

            break;

        case ARRAY_FLOAT:
            // Check for step 0
            if (aplFltStep EQ 0)
            {
                if (aplFltLft NE atRht.aplFloat)
                    goto LEFT_DOMAIN_EXIT;
                aplFltStep = 1;
            } // End IF

            // Set the sign of the step
            if (aplFltLft <= atRht.aplFloat)
                aplFltStep =  fabs (aplFltStep);
            else
                aplFltStep = -fabs (aplFltStep);
            // The NELM of the result is
            aplNELMRes = 1 + (APLINT) fabs ((atRht.aplFloat - aplFltLft) / aplFltStep);

            break;

        case ARRAY_RAT:
            // Check for step 0
            if (mpq_cmp_ui (&aplRatStep, 0, 1) EQ 0)
            {
                if (mpq_cmp (&aplRatLft, &atRht.aplRat) NE 0)
                    goto LEFT_DOMAIN_EXIT;
                mpq_set_ui (&aplRatStep, 1, 1);
            } // End IF

            // Set the sign of the step
            if (mpq_cmp (&aplRatLft, &atRht.aplRat) <= 0)
                mpq_abs (&aplRatStep, &aplRatStep);
            else
            {
                mpq_abs (&aplRatStep, &aplRatStep);
                mpq_neg (&aplRatStep, &aplRatStep);
            } // End IF/ELSE

            // The NELM of the result is
            mpq_sub (&aplRatTmp, &atRht.aplRat, &aplRatLft);
            mpq_div (&aplRatTmp, &aplRatTmp, &aplRatStep);
            mpq_abs (&aplRatTmp, &aplRatTmp);
            aplNELMRes = 1 + mpq_get_ctsa (&aplRatTmp, NULL);     // Rounding to nearest integer within SysCT

            break;

        case ARRAY_VFP:
            // Check for step 0
            if (mpf_cmp_ui (&aplVfpStep, 0) EQ 0)
            {
                if (mpf_cmp (&aplVfpLft, &atRht.aplVfp) NE 0)
                    goto LEFT_DOMAIN_EXIT;
                mpf_set_ui (&aplVfpStep, 1);
            } // End IF

            // Set the sign of the step
            if (mpf_cmp (&aplVfpLft, &atRht.aplVfp) <= 0)
                mpf_abs (&aplVfpStep, &aplVfpStep);
            else
            {
                mpf_abs (&aplVfpStep, &aplVfpStep);
                mpf_neg (&aplVfpStep, &aplVfpStep);
            } // End IF/ELSE

            // The NELM of the result is
            mpf_sub (&aplVfpTmp, &atRht.aplVfp, &aplVfpLft);
            mpf_div (&aplVfpTmp, &aplVfpTmp, &aplVfpStep);
            mpf_abs (&aplVfpTmp, &aplVfpTmp);
            aplNELMRes = 1 + mpf_get_ctsa (&aplVfpTmp, NULL);     // Rounding to nearest integer within SysCT

            break;

        defstop
            break;
    } // End SWITCH

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Now we can allocate the storage for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the result's dimension
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimension
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_APA:
            // Save the APA offset and multiplier
            ((LPAPLAPA) lpMemRes)->Off = aplIntLft;
            ((LPAPLAPA) lpMemRes)->Mul = aplIntStep;

            break;

        case ARRAY_FLOAT:
            // Loop through the result items
            for (uCnt = 0; uCnt < aplNELMRes; uCnt++)
                *((LPAPLFLOAT) lpMemRes)++ = aplFltLft + uCnt * aplFltStep;

            break;

        case ARRAY_RAT:
            // Loop through the result items
            for (uCnt = 0; uCnt < aplNELMRes; uCnt++)
            {
                mpq_init_set (((LPAPLRAT) lpMemRes)++, &aplRatLft);
                mpq_add (&aplRatLft, &aplRatLft, &aplRatStep);
            } // End FOR

            break;

        case ARRAY_VFP:
            // Loop through the result items
            for (uCnt = 0; uCnt < aplNELMRes; uCnt++)
            {
                mpf_init_set (((LPAPLVFP) lpMemRes)++, &aplVfpLft);
                mpf_add (&aplVfpLft, &aplVfpLft, &aplVfpStep);
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LEFT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
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
            MyGlobalUnlock (hGlbRes); lpMemRes  = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes  = NULL;
    } // End IF

    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft  = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht  = NULL;
    } // End IF

    // We no longer need these vars
    Myq_clear (&aplRatTmp);
    Myq_clear (&aplRatStep);
    Myq_clear (&aplRatLft);

    // We no longer need these vars
    Myf_clear (&aplVfpTmp);
    Myf_clear (&aplVfpStep);
    Myf_clear (&aplVfpLft);

    // If the result is a global numeric, ...
    if (IsGlbNum (aplTypeRes))
    {
        if (IsRat (aplTypeRes))
            Myq_clear (&atRht.aplRat);
        else
        if (IsVfp (aplTypeRes))
            Myf_clear (&atRht.aplVfp);
        else
            DbgStop ();
    } // End IF

    return lpYYRes;
} // End PrimFnDydDotDot_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydDotDotNested_EM_YY
//
//  Dyadic DotDot extended to nested arrays
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydDotDotNested_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydDotDotNested_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (must be NULL)

{
    HGLOBAL hGlbMFO;                // Magic function/operator global memory handle

    Assert (lptkAxis EQ NULL);

    // Get the magic function/operator global memory handle
    hGlbMFO = GetMemPTD ()->hGlbMFO[MFOE_DydDotDot];

    //  Use an internal magic function/operator.
    return
      ExecuteMagicFunction_EM_YY (lptkLftArg,   // Ptr to left arg token
                                  lptkFunc,     // Ptr to function token
                                  NULL,         // Ptr to function strand
                                  lptkRhtArg,   // Ptr to right arg token
                                  lptkAxis,     // Ptr to axis token
                                  hGlbMFO,      // Magic function/operator global memory handle
                                  NULL,         // Ptr to HSHTAB struc (may be NULL)
                                  LINENUM_ONE); // Starting line # type (see LINE_NUMS)
} // End PrimFnDydDotDotNested_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  Magic function/operator for Extended Dyadic DotDot
//
//  Extended Dyadic DotDot -- Sequence Generator For Nested Arrays
//
//  This algorithm is a generalization of 2..5 proposed by John Scholes.
//***************************************************************************

static APLCHAR DydHeader[] =
  L"Z" $IS L"L " MFON_DydDotDot L" R;" $QUAD_IO L";" $QUAD_CT;

static APLCHAR DydLine1[] =
  $QUAD_IO $IS L"0" $DIAMOND $QUAD_CT $IS L"3E" $NEG L"15";

static APLCHAR DydLine2[] =
  L"L" $IS L"2" $TAKE L"L,1";

static APLCHAR DydLine3[] =
  L"Z" $IS L"L[0]+L[1]" $TIMES $IOTA L"1+" $DISCLOSE $FLOOR L"((" $DISCLOSE L"R)-0" $DISCLOSE L"L)" $DIVIDE L"1" $DISCLOSE L"L";

static LPAPLCHAR DydBody[] =
{DydLine1,
 DydLine2,
 DydLine3,
};

MAGIC_FCNOPR MFO_DydDotDot =
{DydHeader,
 DydBody,
 countof (DydBody),
};


//***************************************************************************
//  End of File: pf_dotdot.c
//***************************************************************************
