//***************************************************************************
//  NARS2000 -- Primitive Function -- UpTack
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2016 Sudley Place Software

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
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonUpTack_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydUpTack_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
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
    return PrimProtoFnMixed_EM_YY (&PrimFnUpTack_EM_YY,     // Ptr to primitive function routine
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
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
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
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht,           // Right ...
                      aplTypeRes;           // Result   ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht,           // Right ...
                      aplNELMRes;           // Result   ...
    APLRANK           aplRankLft,           // Left arg rank
                      aplRankRht,           // Right ...
                      aplRankRes;           // Result   ...
    APLDIM            aplColsLft,           // Left arg last dim
                      aplRestLft,           // Left arg product of remaining dims
                      aplFrstRht,           // Right arg 1st dim
                      aplRestRht,           // Right arg product of remaining dims
                      aplInnrMax;           // Larger of inner dimensions
    APLLONGEST        aplLongestLft,        // Left arg immediate value
                      aplLongestRht;        // Right ...
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL,       // Right ...
                      hGlbRes = NULL;       // Result   ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg header
                      lpMemHdrRht = NULL,   // ...    right ...
                      lpMemHdrRes = NULL;   // ...    result   ...
    LPVOID            lpMemLft,             // Ptr to left arg global memory
                      lpMemRht,             // ...    right ...
                      lpMemRes;             // ...    result   ...
    LPAPLDIM          lpMemDimLft,          // Ptr to left arg dimensions
                      lpMemDimRht,          // Ptr to right ...
                      lpMemDimRes;          // Ptr to result   ...
    APLUINT           ByteRes,              // # bytes in the result
                      uRes,                 // Loop counter
                      uOutLft,              // Loop counter
                      uOutRht,              // Loop counter
                      uDimCopy;             // # dimensions to copy
    APLINT            iInnMax;              // Loop counter
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    ALLTYPES          atLft = {0},          // Left arg as ALLTYPES
                      atRht = {0},          // Right ...
                      atAdd = {0},          // Temp        ...
                      atMul = {0},          // Temp        ...
                      atAcc = {0},          // Accumulator ...
                      atVal = {0};          // Value       ...
    int               iHCDimRes,            // Result HC Dimension (1, 2, 4, 8)
                      i;                    // Loop counter

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type,NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, &aplColsLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get left & right arg global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Calculate length of right arg first dimension
    if (hGlbRht NE NULL && !IsScalar (aplRankRht))
        aplFrstRht = *VarArrayBaseToDim (lpMemHdrRht);
    else
        aplFrstRht = 1;

    // Check for LENGTH ERROR
    if (aplColsLft NE aplFrstRht
     && !IsUnitDim (aplColsLft)
     && !IsUnitDim (aplFrstRht))
        goto LENGTH_EXIT;

    // Weed out numbers whose Floor does not have Fractionality
    //   such as Character, Nested, Hetero, Octonions, and Quaternions w/o Hurwitz
    if (!HasFractionality (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Weed out numbers whose Floor does not have Fractionality
    //   such as Character, Nested, Hetero, Octonions, and Quaternions w/o Hurwitz
    if (!HasFractionality (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Re-calculate the inner dimensions based upon
    //   scalar/one-element vector extension for empty args
    if ((IsScalar (aplRankLft)
      || IsVectorSing (aplNELMLft, aplRankLft))
     && IsEmpty (aplFrstRht))
        aplColsLft = aplFrstRht;
    else
    if ((IsScalar (aplRankRht)
      || IsVectorSing (aplNELMRht, aplRankRht))
     && IsEmpty (aplColsLft))
        aplFrstRht = aplColsLft;

    // Calc larger of inner dimensions (in case of scalar extension)
    aplInnrMax = max (aplColsLft, aplFrstRht);

    // Calc product of the remaining dimensions in left arg
    if (aplColsLft)
        aplRestLft = aplNELMLft / aplColsLft;
    else
    if (hGlbLft NE NULL)
        for (aplRestLft = 1, uOutLft = 0; uOutLft < (aplRankLft - 1); uOutLft++)
            aplRestLft *= (VarArrayBaseToDim (lpMemHdrLft))[uOutLft];
    else
        aplRestLft = 1;

    // Calc product of the remaining dimensions in right arg
    if (aplFrstRht)
        aplRestRht = aplNELMRht / aplFrstRht;
    else
    if (hGlbRht NE NULL)
        for (aplRestRht = 1, uOutRht = 1; uOutRht < aplRankRht; uOutRht++)
            aplRestRht *= (VarArrayBaseToDim (lpMemHdrRht))[uOutRht];
    else
        aplRestRht = 1;

    // Calc result rank
    aplRankRes = max (aplRankLft, 1) + max (aplRankRht, 1) - 2;

    // Calc result NELM
    aplNELMRes = aplRestLft * aplRestRht;

    // Calc result Type
    aplTypeRes = aTypePromote[aplTypeLft][aplTypeRht];

    // Check for empty and non-global numeric result
    if (IsEmpty (aplNELMRes)
     && !IsGlbNum (aplTypeRes))
        // Calc result Type
        aplTypeRes = ARRAY_BOOL;
    else
    {
        // Promote Boolean to integer
        if (IsSimpleBool (aplTypeRes))
            aplTypeRes = ARRAY_INT;
    } // End IF/ELSE
RESTART_EXCEPTION:
    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDimRes = TranslateArrayTypeToHCDim (aplTypeRes);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Now we can allocate the storage for the result
    //***************************************************************
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        lpMemHdrRes
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    if (hGlbLft NE NULL)
        lpMemDimLft = VarArrayBaseToDim (lpMemHdrLft);
    if (hGlbRht NE NULL)
        lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // Fill in the result's dimension
    //   by copying the left arg dimensions (except for the last)
    //   and then the right arg dimensions  (except for the first)

    // If the left arg is an array, ...
    if (hGlbLft NE NULL)
    {
        // Calc # dimensions to copy
        uDimCopy = max (aplRankLft, 1) - 1;

        // Copy the dimensions
        CopyMemory (lpMemDimRes, lpMemDimLft, (APLU3264) uDimCopy * sizeof (APLDIM));

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    } else
        // Point to the data
        lpMemLft = &aplLongestLft;

    // If the right arg is an array, ...
    if (hGlbRht NE NULL)
    {
        // Calc # dimensions to copy
        uDimCopy = max (aplRankRht, 1) - 1;

        // Copy the dimensions
        CopyMemory (lpMemDimRes, &lpMemDimRht[1], (APLU3264) uDimCopy * sizeof (APLDIM));

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    } else
        // Point to the data
        lpMemRht = &aplLongestRht;

    // Check for empty result
    if (IsEmpty (aplNELMRes))
        goto YYALLOC_EXIT;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // The left arg is treated as a two-dimensional array of shape
    //   aplRestLft aplColsLft

    // The right arg is treated as a two-dimensional array of shape
    //   aplFrstRht aplRestRht

    // Trundle through the left & right arg remaining dimensions
    for (uOutLft = 0; uOutLft < aplRestLft; uOutLft++)
    for (uOutRht = 0; uOutRht < aplRestRht; uOutRht++)
    {
        // Calc result index
        uRes = 1 * uOutRht + aplRestRht * uOutLft;

        // Split cases based upon the result storage type
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
                // Initialize accumulator and weighting value
                atAcc.aplInteger = 0;
                atVal.aplInteger = 1;

                break;

            case ARRAY_FLOAT:
                // Initialize accumulator and weighting value
                atAcc.aplFloat = 0.0;
                atVal.aplFloat = 1.0;

                break;

            case ARRAY_RAT:
                // Initialize accumulator and weighting value
                mphc1r_init (&atAcc.aplHC1R);
                mphc1r_init (&atVal.aplHC1R);
                mpq_set_si  (&atVal.aplRat, 1, 1);

                break;

            case ARRAY_VFP:
                // Initialize accumulator and weighting value
                mphc1v_init0 (&atAcc.aplHC1V);
                mphc1v_init0 (&atVal.aplHC1V);
                mpfr_set_si  (&atVal.aplVfp, 1, MPFR_RNDN);

                break;

            case ARRAY_HC2I:
            case ARRAY_HC4I:
                // Initialize accumulator and weighting value
////////////////ZeroMemory (&atAcc, sizeof (atAcc));        // Already zero from = {0}
////////////////ZeroMemory (&atVal, sizeof (atVal));        // ...
                atVal.aplInteger = 1;

                break;

            case ARRAY_HC2F:
            case ARRAY_HC4F:
                // Initialize accumulator and weighting value
////////////////ZeroMemory (&atAcc, sizeof (atAcc));        // Already zero from = {0}
////////////////ZeroMemory (&atVal, sizeof (atVal));        // ...
                atVal.aplFloat   = 1.0;

                break;


            case ARRAY_HC2R:
                // Initialize accumulator and weighting value
                mphc2r_init (&atAcc.aplHC2R);
                mphc2r_init (&atVal.aplHC2R);
                mpq_set_si  (&atVal.aplRat, 1, 1);

                break;

            case ARRAY_HC4R:
                // Initialize accumulator and weighting value
                mphc4r_init (&atAcc.aplHC4R);
                mphc4r_init (&atVal.aplHC4R);
                mpq_set_si  (&atVal.aplRat, 1, 1);

                break;

            case ARRAY_HC2V:
                // Initialize accumulator and weighting value
                mphc2v_init0 (&atAcc.aplHC2V);
                mphc2v_init0 (&atVal.aplHC2V);
                mpfr_set_si  (&atVal.aplVfp, 1, MPFR_RNDN);

                break;

            case ARRAY_HC4V:
                // Initialize accumulator and weighting value
                mphc4v_init0 (&atAcc.aplHC4V);
                mphc4v_init0 (&atVal.aplHC4V);
                mpfr_set_si  (&atVal.aplVfp, 1, MPFR_RNDN);

                break;

            case ARRAY_CHAR:            // Can't happen w/UpTack
            case ARRAY_APA:             // ...
            case ARRAY_HETERO:          // ...
            case ARRAY_NESTED:          // ...
            case ARRAY_HC8I:            // ...
            case ARRAY_HC8F:            // ...
            case ARRAY_HC8R:            // ...
            case ARRAY_HC8V:            // ...
            defstop
                break;
        } // End SWITCH

        // Trundle through the inner dimensions, back to front
        for (iInnMax = aplInnrMax - 1; iInnMax >= 0; iInnMax--)
        {
            APLUINT uInnLft,            // Index into left arg
                    uInnRht;            // ...        right ...

            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            // Calc left inner index, taking into account scalar extension
            if (IsUnitDim (aplColsLft))
                uInnLft = 1 * 0       + aplColsLft * uOutLft;
            else
                uInnLft = 1 * iInnMax + aplColsLft * uOutLft;

            // Calc right inner index, taking into account scalar extension
            if (IsUnitDim (aplFrstRht))
                uInnRht = 1 * uOutRht + aplRestRht * 0      ;
            else
                uInnRht = 1 * uOutRht + aplRestRht * iInnMax;

            // Promote the left and right arg items to the result type
            (aTypeActPromote[aplTypeLft][aplTypeRes]) (lpMemLft, uInnLft, &atLft);
            (aTypeActPromote[aplTypeRht][aplTypeRes]) (lpMemRht, uInnRht, &atRht);

            // Split cases based upon the result storage type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:
                case ARRAY_INT:
                    __try
                    {
                        // Add into accumulator
                        atAcc.aplInteger = iadd64_RE (atAcc.aplInteger, imul64_RE (atVal.aplInteger, GetNextInteger (lpMemRht, aplTypeRht, uInnRht), FLOAT), FLOAT);

                        // Multiply into the weighting value
                        atVal.aplInteger = imul64_RE (atVal.aplInteger, GetNextInteger (lpMemLft, aplTypeLft, uInnLft), FLOAT);
                    } __except (CheckException (GetExceptionInformation (), L"PrimFnDydUpTack_EM_YY"))
                    {
                        switch (MyGetExceptionCode ())
                        {
                            case EXCEPTION_RESULT_FLOAT:
                                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                                if (!IsSimpleFlt (aplTypeRes))
                                {
                                    aplTypeRes = ARRAY_FLOAT;

                                    dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);

                                    // We no longer need these ptrs
                                    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                                    // We no longer need this storage
                                    FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

                                    goto RESTART_EXCEPTION;
                                } // End IF

                                // Fall through to never-never-land

                            default:
                                // Display message for unhandled exception
                                DisplayException ();

                                break;
                        } // End SWITCH
                    } // End __try/__except

                    break;

                case ARRAY_FLOAT:
                    // Add into accumulator
                    atAcc.aplFloat += atVal.aplFloat * GetNextFloat (lpMemRht, aplTypeRht, uInnRht);

                    // Multiply into the weighting value
                    atVal.aplFloat *= GetNextFloat (lpMemLft, aplTypeLft, uInnLft);

                    break;

                case ARRAY_RAT:
                    // Add into accumulator
////////////////////atAcc.aplRat += atVal.aplRat * GetNextRat (lpMemRht, aplTypeRht, uInnRht);
////////////////////mpq_mul (&atAdd.aplRat, &atVal.aplRat, &atRht.aplRat);
                    atAdd.aplRat = MulHC1R_RE (atVal.aplRat, atRht.aplRat);
                    mpq_add (&atAcc.aplRat, &atAcc.aplRat, &atAdd.aplRat);

                    // Free old atAdd
                    (*aTypeFree[aplTypeRes]) (&atAdd, 0);

                    // Multiply into the weighting value
////////////////////atVal.aplRat *= atLft.aplRat;
                    mpq_mul (&atVal.aplRat, &atVal.aplRat, &atLft.aplRat);

                    break;

                case ARRAY_VFP:
                    // Add into accumulator
////////////////////atAcc.aplVfp += atVal.aplVfp * atRht.aplVfp;
////////////////////mpfr_mul (&atAdd.aplVfp, &atVal.aplVfp, &atRht.aplVfp, MPFR_RNDN);
                    atAdd.aplVfp = MulHC1V_RE (atVal.aplVfp, atRht.aplVfp);
                    mpfr_add (&atAcc.aplVfp, &atAcc.aplVfp, &atAdd.aplVfp, MPFR_RNDN);

                    // Free old atAdd
                    (*aTypeFree[aplTypeRes]) (&atAdd, 0);

                    // Multiply into the weighting value
////////////////////atVal.aplVfp *= atLft.aplVfp;
                    mpfr_mul (&atVal.aplVfp, &atVal.aplVfp, &atLft.aplVfp, MPFR_RNDN);

                    break;

                case ARRAY_HC2I:
                    __try
                    {
                        // Add into accumulator
////////////////////////atAcc.aplHCxI += atRht.aplHCxI * atVal.aplHCxI
                        atAcc.aplHC2I = AddHC2I_RE (atAcc.aplHC2I,
                                                    MulHC2I_RE (atRht.aplHC2I,
                                                                atVal.aplHC2I));
                        // Multiply into the weighting value on the left
////////////////////////atVal.aplHCxI = atLft.aplHCxI * atVal.aplHCxI
                        atVal.aplHC2I = MulHC2I_RE (atLft.aplHC2I,
                                                    atVal.aplHC2I);
                    } __except (CheckException (GetExceptionInformation (), L"PrimFnDydUpTack_EM_YY"))
                    {
                        switch (MyGetExceptionCode ())
                        {
                            case EXCEPTION_RESULT_FLOAT:
                            case EXCEPTION_RESULT_HC2F:
                                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                                if (!IsSimpleFlt (aplTypeRes))
                                {
                                    aplTypeRes = ARRAY_HC2F;

                                    dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);

                                    // We no longer need these ptrs
                                    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                                    // We no longer need this storage
                                    FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

                                    goto RESTART_EXCEPTION;
                                } // End IF

                                // Fall through to never-never-land

                            default:
                                // Display message for unhandled exception
                                DisplayException ();

                                break;
                        } // End SWITCH
                    } // End __try/__except

                    break;

                case ARRAY_HC4I:
                    __try
                    {
                        // Add into accumulator
////////////////////////atAcc.aplHCxI += atRht.aplHCxI * atVal.aplHCxI;
                        atAcc.aplHC4I = AddHC4I_RE (atAcc.aplHC4I,
                                                    MulHC4I_RE (atRht.aplHC4I,
                                                                atVal.aplHC4I));
                        // Multiply into the weighting value on the left
////////////////////////atVal.aplHCxI = atLft.aplHCxI * atVal.aplHCxI;
                        atVal.aplHC4I = MulHC4I_RE (atLft.aplHC4I,
                                                    atVal.aplHC4I);
                    } __except (CheckException (GetExceptionInformation (), L"PrimFnDydUpTack_EM_YY"))
                    {
                        switch (MyGetExceptionCode ())
                        {
                            case EXCEPTION_RESULT_FLOAT:
                            case EXCEPTION_RESULT_HC2F:
                            case EXCEPTION_RESULT_HC4F:
                                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                                if (!IsSimpleFlt (aplTypeRes))
                                {
                                    aplTypeRes = ARRAY_HC4F;

                                    dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);

                                    // We no longer need these ptrs
                                    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                                    // We no longer need this storage
                                    FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

                                    goto RESTART_EXCEPTION;
                                } // End IF

                                // Fall through to never-never-land

                            default:
                                // Display message for unhandled exception
                                DisplayException ();

                                break;
                        } // End SWITCH
                    } // End __try/__except

                    break;

////            case ARRAY_HC8I:
////                __try
////                {
////                    // Add into accumulator
////////////////////////atAcc.aplHCxI += atRht.aplHCxI * atVal.aplHCxI;
////                    atAcc.aplHC8I = AddHC8I_RE (atAcc.aplHC8I,
////                                                MulHC8I_RE (atRht.aplHC8I,
////                                                            atVal.aplHC8I));
////                    // Multiply into the weighting value on the left
////////////////////////atVal.aplHCxI = atLft.aplHCxI * atVal.aplHCxI;
////                    atVal.aplHC8I = MulHC8I_RE (atLft.aplHC8I,
////                                                atVal.aplHC8I);
////                } __except (CheckException (GetExceptionInformation (), L"PrimFnDydUpTack_EM_YY"))
////                {
////                    switch (MyGetExceptionCode ())
////                    {
////                        case EXCEPTION_RESULT_FLOAT:
////                        case EXCEPTION_RESULT_HC2F:
////                        case EXCEPTION_RESULT_HC4F:
////                        case EXCEPTION_RESULT_HC8F:
////                            MySetExceptionCode (EXCEPTION_SUCCESS); // Reset
////
////                            if (!IsSimpleFlt (aplTypeRes))
////                            {
////                                aplTypeRes = ARRAY_HC8F;
////
////                                dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L": %2d (%S#%d)", MyGetExceptionCode (), FNLN);
////
////                                // We no longer need these ptrs
////                                MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
////
////                                // We no longer need this storage
////                                FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
////
////                                goto RESTART_EXCEPTION;
////                            } // End IF
////
////                            // Fall through to never-never-land
////
////                        default:
////                            // Display message for unhandled exception
////                            DisplayException ();
////
////                            break;
////                    } // End SWITCH
////                } // End __try/__except
////
////                break;

                case ARRAY_HC2F:
                    // Add into accumulator
////////////////////atAcc.aplHCxF += atRht.aplHCxF * atVal.aplHCxF;
                    atAcc.aplHC2F = AddHC2F_RE (atAcc.aplHC2F,
                                                MulHC2F_RE (atRht.aplHC2F,
                                                            atVal.aplHC2F));
                    // Multiply into the weighting value on the left
////////////////////atVal.aplHCxF = atLft.aplHCxF * atVal.aplHCxF;
                    atVal.aplHC2F = MulHC2F_RE (atLft.aplHC2F,
                                                atVal.aplHC2F);
                    break;

                case ARRAY_HC4F:
                    // Add into accumulator
////////////////////atAcc.aplHCxF += atRht.aplHCxF * atVal.aplHCxF;
                    atAcc.aplHC4F = AddHC4F_RE (atAcc.aplHC4F,
                                                MulHC4F_RE (atRht.aplHC4F,
                                                            atVal.aplHC4F));
                    // Multiply into the weighting value on the left
////////////////////atVal.aplHCxF = atLft.aplHCxF * atVal.aplHCxF;
                    atVal.aplHC4F = MulHC4F_RE (atLft.aplHC4F,
                                                atVal.aplHC4F);
                    break;

////            case ARRAY_HC8F:
////                // Add into accumulator
////////////////////atAcc.aplHCxF += atRht.aplHCxF * atVal.aplHCxF;
////                atAcc.aplHC8F = AddHC8F_RE (atAcc.aplHC8F,
////                                            MulHC8F_RE (atRht.aplHC8F,
////                                                        atVal.aplHC8F));
////                // Multiply into the weighting value on the left
////////////////////atVal.aplHCxF = atLft.aplHCxF * atVal.aplHCxF;
////                atVal.aplHC8F = MulHC8F_RE (atLft.aplHC8F,
////                                            atVal.aplHC8F);
////                break;

                case ARRAY_HC2R:
                    // Add into accumulator
////////////////////atAcc.aplHCxR += atRht.aplHCxR * atVal.aplHCxR;
////////////////////atAcc.aplHC2R = AddHC2R_RE (atAcc.aplHC2R,
////////////////////                            MulHC2R_RE (atRht.aplHC2R,
////////////////////                                        atVal.aplHC2R));
                    atMul.aplHC2R = MulHC2R_RE (atRht.aplHC2R,
                                                atVal.aplHC2R);
                    atAdd.aplHC2R = AddHC2R_RE (atAcc.aplHC2R,
                                                atMul.aplHC2R);
                    mphc2r_set (&atAcc.aplHC2R, &atAdd.aplHC2R);

                    // Free old atAdd and atMul
                    (*aTypeFree[aplTypeRes]) (&atAdd, 0);
                    (*aTypeFree[aplTypeRes]) (&atMul, 0);

                    // Multiply into the weighting value on the left
////////////////////atVal.aplHCxR = atLft.aplHCxR * atVal.aplHCxR;
////////////////////atVal.aplHC2R = MulHC2R_RE (atLft.aplHC2R,
////////////////////                            atVal.aplHC2R);
                    atMul.aplHC2R = MulHC2R_RE (atLft.aplHC2R,
                                                atVal.aplHC2R);
                    mphc2r_set (&atVal.aplHC2R, &atMul.aplHC2R);

                    // Free old atMul
                    (*aTypeFree[aplTypeRes]) (&atMul, 0);

                    break;

                case ARRAY_HC4R:
                    // Add into accumulator
////////////////////atAcc.aplHCxR += atRht.aplHCxR * atVal.aplHCxR;
////////////////////atAcc.aplHC4R = AddHC4R_RE (atAcc.aplHC4R,
////////////////////                            MulHC4R_RE (atRht.aplHC4R,
////////////////////                                        atVal.aplHC4R));
                    atMul.aplHC4R = MulHC4R_RE (atRht.aplHC4R,
                                                atVal.aplHC4R);
                    atAdd.aplHC4R = AddHC4R_RE (atAcc.aplHC4R,
                                                atMul.aplHC4R);
                    mphc4r_set (&atAcc.aplHC4R, &atAdd.aplHC4R);

                    // Free old atAdd and atMul
                    (*aTypeFree[aplTypeRes]) (&atAdd, 0);
                    (*aTypeFree[aplTypeRes]) (&atMul, 0);

                    // Multiply into the weighting value on the left
////////////////////atVal.aplHCxR = atLft.aplHCxR * atVal.aplHCxR;
////////////////////atVal.aplHC4R = MulHC4R_RE (atLft.aplHC4R,
////////////////////                            atVal.aplHC4R);
                    atMul.aplHC4R = MulHC4R_RE (atLft.aplHC4R,
                                                atVal.aplHC4R);
                    mphc4r_set (&atVal.aplHC4R, &atMul.aplHC4R);

                    // Free old atMul
                    (*aTypeFree[aplTypeRes]) (&atMul, 0);

                    break;

////            case ARRAY_HC8R:
////                // Add into accumulator
////////////////////atAcc.aplHCxR += atRht.aplHCxR * atVal.aplHCxR;
////////////////////atAcc.aplHC8R = AddHC8R_RE (atAcc.aplHC8R,
////////////////////                            MulHC8R_RE (atRht.aplHC8R,
////////////////////                                        atVal.aplHC8R));
////                atMul.aplHC8R = MulHC8R_RE (atRht.aplHC8R,
////                                            atVal.aplHC8R);
////                atAdd.aplHC8R = AddHC8R_RE (atAcc.aplHC8R,
////                                            atMul.aplHC8R);
////                mphc8r_set (&atAcc.aplHC8R, &atAdd.aplHC8R);
////
////                // Free old atAdd and atMul
////                (*aTypeFree[aplTypeRes]) (&atAdd, 0);
////                (*aTypeFree[aplTypeRes]) (&atMul, 0);
////
////                // Multiply into the weighting value on the left
////////////////////atVal.aplHCxR = atLft.aplHCxR * atVal.aplHCxR;
////////////////////atVal.aplHC8R = MulHC8R_RE (atLft.aplHC8R,
////////////////////                            atVal.aplHC8R);
////                atMul.aplHC8R = MulHC8R_RE (atLft.aplHC8R,
////                                            atVal.aplHC8R);
////                mphc8r_set (&atVal.aplHC8R, &atMul.aplHC8R);
////
////                // Free old atMul
////                (*aTypeFree[aplTypeRes]) (&atMul, 0);
////
////                break;

                case ARRAY_HC2V:
                    // Add into accumulator
////////////////////atAcc.aplHCxV += atRht.aplHCxV * atVal.aplHCxV;
////////////////////atAcc.aplHC2V = AddHC2V_RE (atAcc.aplHC2V,
////////////////////                            MulHC2V_RE (atRht.aplHC2V,
////////////////////                                        atVal.aplHC2V));
                    atMul.aplHC2V = MulHC2V_RE (atRht.aplHC2V,
                                                atVal.aplHC2V);
                    atAdd.aplHC2V = AddHC2V_RE (atAcc.aplHC2V,
                                                atMul.aplHC2V);
                    mphc2v_set (&atAcc.aplHC2V, &atAdd.aplHC2V);

                    // Free old atAdd and atMul
                    (*aTypeFree[aplTypeRes]) (&atAdd, 0);
                    (*aTypeFree[aplTypeRes]) (&atMul, 0);

                    // Multiply into the weighting value on the left
////////////////////atVal.aplHCxV = atLft.aplHCxV * atVal.aplHCxV;
////////////////////atVal.aplHC2V = MulHC2V_RE (atLft.aplHC2V,
////////////////////                            atVal.aplHC2V);
                    atMul.aplHC2V = MulHC2V_RE (atLft.aplHC2V,
                                                atVal.aplHC2V);
                    mphc2v_set (&atVal.aplHC2V, &atMul.aplHC2V);

                    // Free old atMul
                    (*aTypeFree[aplTypeRes]) (&atMul, 0);

                    break;

                case ARRAY_HC4V:
                    // Add into accumulator
////////////////////atAcc.aplHCxV += atRht.aplHCxV * atVal.aplHCxV;
////////////////////atAcc.aplHC4V = AddHC4V_RE (atAcc.aplHC4V,
////////////////////                            MulHC4V_RE (atRht.aplHC4V,
////////////////////                                        atVal.aplHC4V));
                    atMul.aplHC4V = MulHC4V_RE (atRht.aplHC4V,
                                                atVal.aplHC4V);
                    atAdd.aplHC4V = AddHC4V_RE (atAcc.aplHC4V,
                                                atMul.aplHC4V);
                    mphc4v_set (&atAcc.aplHC4V, &atAdd.aplHC4V);

                    // Free old atAdd and atMul
                    (*aTypeFree[aplTypeRes]) (&atAdd, 0);
                    (*aTypeFree[aplTypeRes]) (&atMul, 0);

                    // Multiply into the weighting value on the left
////////////////////atVal.aplHCxV = atLft.aplHCxV * atVal.aplHCxV;
////////////////////atVal.aplHC4V = MulHC4V_RE (atLft.aplHC4V,
////////////////////                            atVal.aplHC4V);
                    atMul.aplHC4V = MulHC4V_RE (atLft.aplHC4V,
                                                atVal.aplHC4V);
                    mphc4v_set (&atVal.aplHC4V, &atMul.aplHC4V);

                    // Free old atMul
                    (*aTypeFree[aplTypeRes]) (&atMul, 0);

                    break;

////            case ARRAY_HC8V:
////                // Add into accumulator
////////////////////atAcc.aplHCxV += atRht.aplHCxV * atVal.aplHCxV;
////////////////////atAcc.aplHC8V = AddHC8V_RE (atAcc.aplHC8V,
////////////////////                            MulHC8V_RE (atRht.aplHC8V,
////////////////////                                        atVal.aplHC8V));
////                atMul.aplHC8V = MulHC8V_RE (atRht.aplHC8V,
////                                            atVal.aplHC8V);
////                atAdd.aplHC8V = AddHC8V_RE (atAcc.aplHC8V,
////                                            atMul.aplHC8V);
////                mphc8v_set (&atAcc.aplHC8V, &atAdd.aplHC8V);
////
////                // Free old atAdd and atMul
////                (*aTypeFree[aplTypeRes]) (&atAdd, 0);
////                (*aTypeFree[aplTypeRes]) (&atMul, 0);
////
////                // Multiply into the weighting value on the left
////////////////////atVal.aplHCxV = atLft.aplHCxV * atVal.aplHCxV;
////////////////////atVal.aplHC8V = MulHC8V_RE (atLft.aplHC8V,
////////////////////                            atVal.aplHC8V);
////                atMul.aplHC8V = MulHC8V_RE (atLft.aplHC8V,
////                                            atVal.aplHC8V);
////                mphc8v_set (&atVal.aplHC8V, &atMul.aplHC8V);
////
////                // Free old atMul
////                (*aTypeFree[aplTypeRes]) (&atMul, 0);
////
////                break;

                case ARRAY_APA:
                case ARRAY_CHAR:
                case ARRAY_HETERO:
                case ARRAY_NESTED:
                defstop
                    break;
            } // End SWITCH

            // Free the old atLft and atRht (if any)
            (*aTypeFree[aplTypeRes]) (&atLft, 0);
            (*aTypeFree[aplTypeRes]) (&atRht, 0);
        } // End FOR

        // Split cases based upon the result storage type
        switch (aplTypeRes)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
                // Save in the result
                ((LPAPLINT)   lpMemRes)[uRes] = atAcc.aplInteger;

                break;

            case ARRAY_FLOAT:
                // Save in the result
                ((LPAPLFLOAT) lpMemRes)[uRes] = atAcc.aplFloat;

                break;

            case ARRAY_RAT:
                // Save in the result
                mpq_init_set (&((LPAPLRAT) lpMemRes)[uRes], &atAcc.aplRat);

                break;

            case ARRAY_VFP:
                // Save in the result
                mpfr_init_copy (&((LPAPLVFP) lpMemRes)[uRes], &atAcc.aplVfp);

                break;

            case ARRAY_HC2I:
            case ARRAY_HC4I:
////        case ARRAY_HC8I:
                // Loop through all of the parts
                for (i = 0; i < iHCDimRes; i++)
                    // Save in the result
                    *((LPAPLINT) lpMemRes)++ = atAcc.aplHC8I.parts[i];
                break;

            case ARRAY_HC2F:
            case ARRAY_HC4F:
////        case ARRAY_HC8F:
                // Loop through all of the parts
                for (i = 0; i < iHCDimRes; i++)
                    // Save in the result
                    *((LPAPLFLOAT) lpMemRes)++ = atAcc.aplHC8F.parts[i];
                break;

            case ARRAY_HC2R:
            case ARRAY_HC4R:
////        case ARRAY_HC8R:
                // Loop through all of the parts
                for (i = 0; i < iHCDimRes; i++)
                    // Save in the result
                    mpq_init_set (((LPAPLRAT) lpMemRes)++, &atAcc.aplHC8R.parts[i]);
                break;

            case ARRAY_HC2V:
            case ARRAY_HC4V:
////        case ARRAY_HC8V:
                // Loop through all of the parts
                for (i = 0; i < iHCDimRes; i++)
                    // Save in the result
                    mpfr_init_set (((LPAPLVFP) lpMemRes)++, &atAcc.aplHC8V.parts[i], MPFR_RNDN);
                break;

            case ARRAY_CHAR:            // Can't happen w/UpTack
            case ARRAY_APA:             // ...
            case ARRAY_HETERO:          // ...
            case ARRAY_NESTED:          // ...
            defstop
                break;
        } // End SWITCH

        // Free the old atAcc and atVal (if any)
        (*aTypeFree[aplTypeRes]) (&atAcc, 0);
        (*aTypeFree[aplTypeRes]) (&atVal, 0);
    } // End FOR/FOR
YYALLOC_EXIT:
    // Unlock the result global memory in case TypeDemote actually demotes
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken, FALSE);

    goto NORMAL_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
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
    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnDydUpTack_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_utack.c
//***************************************************************************
