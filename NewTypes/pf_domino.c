//***************************************************************************
//  NARS2000 -- Primitive Function -- Domino
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2018 Sudley Place Software

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

#define __GSL_MATRIX_COMPLEX_LONG_DOUBLE_H__

#include "headers.h"
#include "pf_domino.mac"
#include <gsl/gsl_vector_double.h>
#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#undef  inline
#define inline


//***************************************************************************
//  $PrimFnDomino_EM_YY
//
//  Primitive function for monadic and dyadic Domino ("matrix inverse" and "matrix divide")
//***************************************************************************

LPPL_YYSTYPE PrimFnDomino_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DOMINO);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonDomino_EM_YY             (lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydDomino_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnDomino_EM_YY


//***************************************************************************
//  $PrimProtoFnDomino_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Domino
//***************************************************************************

LPPL_YYSTYPE PrimProtoFnDomino_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnDomino_EM_YY, // Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnDomino_EM_YY


//***************************************************************************
//  $PrimIdentFnDomino_EM_YY
//
//  Generate an identity element for the primitive function dyadic Domino
//***************************************************************************

LPPL_YYSTYPE PrimIdentFnDomino_EM_YY
    (LPTOKEN lptkRhtOrig,           // Ptr to original right arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeRht;           // Right arg storage type
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to identity element result
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL;       // Result    ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result
    LPAPLBOOL         lpMemRes;             // Ptr to result    ...
    APLNELM           aplNELMRes;           // Result NELM
    APLRANK           aplRankRht;           // Right arg rank
    APLDIM            uNumRows,             // # rows in the right arg
                      uNumCols;             // # cols ...
    APLUINT           ByteRes,              // # bytes in the result
                      uRes;                 // Loop counter

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig NE NULL);
    Assert (lptkFunc    NE NULL);
    Assert (lptkRhtArg  NE NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // The (right) identity function for dyadic Domino
    //   (L {domino} R) ("matrix divide") is
    //   ({iota}{first}{rho}R){jot}.={iota}{first}{rho}R.

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, NULL, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsRank3P (aplRankRht))
        goto RANK_EXIT;

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Calculate the # rows & cols in the result
    switch (aplRankRht)
    {
        case 0:                 // 1x1 matrix
            uNumRows =
            uNumCols = 1;

            break;

        case 1:                 // 1-col matrix
            uNumRows = *VarArrayBaseToDim (lpMemHdrRht);
            uNumCols = 1;

            break;

        case 2:
            uNumRows = (VarArrayBaseToDim (lpMemHdrRht))[0];
            uNumCols = (VarArrayBaseToDim (lpMemHdrRht))[1];

            break;

        defstop
            break;
    } // End SWITCH

    // Check for LENGTH ERROR
    if (IsMatrix (aplRankRht)
     && uNumRows < uNumCols)
        goto LENGTH_EXIT;

    // Check for DOMAIN ERROR
    if (!IsNumeric (aplTypeRht))
        goto DOMAIN_EXIT;

    // If the right arg is a scalar, the result is an immediate
    if (IsScalar (aplRankRht))
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_BOOL;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkBoolean  = 1;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // From here on, the right arg (and the result) is a vector or matrix
    //***************************************************************

    // Calculate the result NELM
    aplNELMRes = uNumRows * uNumRows;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_BOOL, aplNELMRes, aplRankRht);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_BOOL;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = (LPAPLBOOL) VarArrayBaseToDim (lpMemHdrRes);

    // Save the dimension(s)
    // Split cases based upon the rank of the right arg (same as the result)
    switch (aplRankRht)
    {
        case 1:                                         // Vector
            *((LPAPLDIM) lpMemRes)++ = uNumRows;        // Length

            break;

        case 2:                                         // Matrix
            *((LPAPLDIM) lpMemRes)++ = uNumRows;        // # rows
            *((LPAPLDIM) lpMemRes)++ = uNumRows;        // # cols

            break;

        defstop
            break;
    } // End SWITCH

    //***************************************************************
    // lpMemRes now points to its data
    //***************************************************************

    // Loop through the result setting the major diagonal bits
    for (uRes = 0; uRes < aplNELMRes; uRes += (uNumRows + 1))
        lpMemRes[uRes >> LOG2NBIB] |= BIT0 << (MASKLOG2NBIB & (UINT) uRes);

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

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
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

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
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

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimIdentFnDomino_EM_YY


//***************************************************************************
//  $PrimFnMonDomino_EM_YY
//
//  Primitive function for monadic Domino ("matrix inverse")
//***************************************************************************

LPPL_YYSTYPE PrimFnMonDomino_EM_YY
    (LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    APLSTYPE            aplTypeRht,             // Right arg storage type
                        aplTypeRes,             // Result    ...
                        aplTypeTmp;             // Temp      ...
    APLNELM             aplNELMRht,             // Right arg NELM
                        aplNELMRes;             // Result    ...
    APLRANK             aplRankRht,             // Right arg rank
                        aplRankRes;             // Result    ...
    HGLOBAL             hGlbRht = NULL,         // Right arg global memory handle
                        hGlbRes = NULL,         // Result    ...
                        hGlbSgl = NULL,         // Singleton ...
                        hGlbTmp = NULL;         // Temporary ...
    LPVARARRAY_HEADER   lpMemHdrRht = NULL,     // Ptr to right arg header
                        lpMemHdrRes = NULL,     // ...    result
                        lpMemHdrSgl = NULL;     // ...    singleton
    LPVOID              lpMemRht,               // Ptr to right arg global memory data
                        lpMemRes,               // Ptr to result    ...
                        lpMemSgl,               // Ptr to singleton ...
                        lpMemTmp = NULL;        // Ptr to temporary ...
    APLUINT             ByteRes;                // # bytes in the result
    APLDIM              uNumRows,               // # rows in the right arg
                        uNumCols,               // # cols ...
                        uRow;                   // Loop counter
    APLLONGEST          aplLongestRht;          // ...       as immediate
    LPPL_YYSTYPE        lpYYRes = NULL;         // Ptr to the result
    gsl_matrix         *lpGslMatrixU = NULL,    // GSL temp
                       *lpGslMatrixV = NULL;    // ...
    gsl_vector         *lpGslVectorS = NULL,    // ...
                       *lpGslVectorW = NULL,    // ...
                       *lpGslVectorI = NULL;    // ...
    gsl_matrix_complex *lpGslCMatrixU  = NULL,  // ...
                       *lpGslCMatrixInv = NULL; // ...
    gsl_permutation    *lpGslPermP = NULL;      // ...
    int                 iSizeofRes,             // # bytes in each result item
                        iHCDimRht;              // Right arg HC Dimension (1, 2, 4, 8)
    LPPLLOCALVARS       lpplLocalVars;          // Ptr to re-entrant vars
    LPUBOOL             lpbCtrlBreak;           // Ptr to Ctrl-Break flag
    APLINT              iSizeofTmp;             // Temp sizeof () datatype
    ALLTYPES            atRes = {0};            // Result arg as ALLTYPES
    LPAPLDIM            lpMemDimRht;            // Ptr to right arg dimensions
    LPPERTABDATA        lpMemPTD;               // Ptr to PerTabData global memory
    UBOOL               bRet = TRUE;            // TRUE iff the result is valid
    UINT                uRet = 0;               // Return code from InvertHCxF

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Skip over the header and dimensions to the data
    if (lpMemHdrRht NE NULL)
        // Point to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        // Point to the data
        lpMemRht = &aplLongestRht;

    // Calculate right arg HC Dimension (1, 2, 4, 8)
    iHCDimRht = TranslateArrayTypeToHCDim (aplTypeRht);

    // Check for RANK ERROR
    if (IsRank3P (aplRankRht))
        goto RANK_EXIT;

    // Calculate the # rows & cols in the result
    switch (aplRankRht)
    {
        case 0:                 // 1x1 matrix
            uNumRows =
            uNumCols = 1;

            break;

        case 1:                 // 1-col matrix
            uNumRows = *VarArrayBaseToDim (lpMemHdrRht);
            uNumCols = 1;

            break;

        case 2:
            uNumRows = (VarArrayBaseToDim (lpMemHdrRht))[0];
            uNumCols = (VarArrayBaseToDim (lpMemHdrRht))[1];

            break;

        defstop
            break;
    } // End SWITCH

    // Check for LENGTH ERROR
    if (IsMatrix (aplRankRht)
     && uNumRows < uNumCols)
        goto LENGTH_EXIT;

    // Check for DOMAIN ERROR
    if (!IsNumeric (aplTypeRht))
        goto DOMAIN_EXIT;

    // Calculate the result storage type
    if (IsSimpleNum (aplTypeRht))
        aplTypeRes = ARRAY_FLOAT;
    else
        aplTypeRes = aplTypeRht;

    // Promote the result storage type as appropriate (HCxI -> HCxF)
    if (IsAnyInt (aplTypeRes))
        aplTypeRes++;           // Relying on order of ARRAY_TYPES

    // If the right arg is a singleton, the result is an immediate or scalar global numeric,
    //   or vector or matrix simple or global numeric ...
    if (IsSingleton (aplNELMRht))
    {
        IMM_TYPES immTypeRes;
        APLFLOAT  aplFloatRht;

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
////////lpYYRes->tkToken.tkFlags.TknType   =            // Filled in below
////////lpYYRes->tkToken.tkFlags.ImmType   =            // Filled in below
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkData.tkFloat    =            // Filled in below
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
            case ARRAY_FLOAT:
                // Set the immediate type
                immTypeRes = IMMTYPE_FLOAT;

                // Get the first item
                aplFloatRht = GetNextFloat (lpMemRht, aplTypeRht, 0);

                // If the right arg is zero, the result is zero
                //   as per SVD
                if (aplFloatRht NE 0)
                    // Invert it
                    aplFloatRht = 1.0 / aplFloatRht;

                // Save in the result
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkFlags.ImmType   = immTypeRes;
                lpYYRes->tkToken.tkData.tkFloat    = aplFloatRht;

                break;

            case ARRAY_HC2I:
            case ARRAY_HC4I:
            case ARRAY_HC8I:
                // Set the immediate type
                immTypeRes = TranslateArrayTypeToImmType (aplTypeRes);

                // If the right arg is zero, the result is zero
                //   as per SVD
                if (IsZeroHCxI (lpMemRht, iHCDimRht))
                    // Make a copy of the right arg so we can copy it to the result
                    (*aTypeActPromote[aplTypeRes][aplTypeRes]) (lpMemRht, 0, &atRes);
                else
                // Split cases based upon the storage type
                switch (aplTypeRht)
                {
                    case ARRAY_HC2I:
                        // Invert it
                        PrimFnMonColonBarHC2FisHC2I (&atRes.aplHC2F, 0, lpMemRht, NULL);

                        break;

                    case ARRAY_HC4I:
                        // Invert it
                        PrimFnMonColonBarHC4FisHC4I (&atRes.aplHC4F, 0, lpMemRht, NULL);

                        break;

                    case ARRAY_HC8I:
                        // Invert it
                        PrimFnMonColonBarHC8FisHC8I (&atRes.aplHC8F, 0, lpMemRht, NULL);

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Save in the result
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
                lpYYRes->tkToken.tkFlags.ImmType   = immTypeRes;
                lpYYRes->tkToken.tkData.tkGlbData  =
                  MakeGlbEntry_EM (aplTypeRes,
                                  &atRes,
                                   FALSE,
                                   lptkFunc);
                break;

            case ARRAY_HC2F:
            case ARRAY_HC4F:
            case ARRAY_HC8F:
                // Set the immediate type
                immTypeRes = TranslateArrayTypeToImmType (aplTypeRes);  // Relying upon order in ARRAY_TYPES

                // If the right arg is zero, the result is zero
                //   as per SVD
                if (IsZeroHCxF (lpMemRht, iHCDimRht))
                    // Make a copy of the right arg so we can copy it to the result
                    (*aTypeActPromote[aplTypeRes][aplTypeRes]) (lpMemRht, 0, &atRes);
                else
                // Split cases based upon the storage type
                switch (aplTypeRht)
                {
                    case ARRAY_HC2F:
                        // Invert it
                        PrimFnMonColonBarHC2FisHC2F (&atRes.aplHC2F, 0, lpMemRht, NULL);

                        break;

                    case ARRAY_HC4F:
                        // Invert it
                        PrimFnMonColonBarHC4FisHC4F (&atRes.aplHC4F, 0, lpMemRht, NULL);

                        break;

                    case ARRAY_HC8F:
                        // Invert it
                        PrimFnMonColonBarHC8FisHC8F (&atRes.aplHC8F, 0, lpMemRht, NULL);

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Save in the result
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
                lpYYRes->tkToken.tkFlags.ImmType   = immTypeRes;
                lpYYRes->tkToken.tkData.tkGlbData  =
                  MakeGlbEntry_EM (aplTypeRes,
                                  &atRes,
                                   FALSE,
                                   lptkFunc);
                break;

            case ARRAY_RAT:
            case ARRAY_HC2R:
            case ARRAY_HC4R:
            case ARRAY_HC8R:
                // Set the immediate type
                immTypeRes = TranslateArrayTypeToImmType (aplTypeRes);  // Relying upon order in ARRAY_TYPES

                // If the right arg is zero, the result is zero
                //   as per SVD
                if (IsZeroHCxR (lpMemRht, iHCDimRht))
                    // Make a copy of the right arg so we can copy it to the result
                    (*aTypeActPromote[aplTypeRes][aplTypeRes]) (lpMemRht, 0, &atRes);
                else
                    // Split cases based upon the storage type
                    switch (aplTypeRht)
                    {
                        case ARRAY_HC1R:
                            // Invert it
                            PrimFnMonColonBarRisR       (&atRes.aplHC1R, 0, lpMemRht, NULL);

                            break;

                        case ARRAY_HC2R:
                            // Invert it
                            PrimFnMonColonBarHC2RisHC2R (&atRes.aplHC2R, 0, lpMemRht, NULL);

                            break;

                        case ARRAY_HC4R:
                            // Invert it
                            PrimFnMonColonBarHC4RisHC4R (&atRes.aplHC4R, 0, lpMemRht, NULL);

                            break;

                        case ARRAY_HC8R:
                            // Invert it
                            PrimFnMonColonBarHC8RisHC8R (&atRes.aplHC8R, 0, lpMemRht, NULL);

                            break;

                        defstop
                            break;
                    } // End SWITCH

                // Save in the result
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
                lpYYRes->tkToken.tkFlags.ImmType   = immTypeRes;
                lpYYRes->tkToken.tkData.tkGlbData  =
                  MakeGlbEntry_EM (aplTypeRes,
                                  &atRes,
                                   FALSE,
                                   lptkFunc);
                // Check for errors
                if (lpYYRes->tkToken.tkData.tkGlbData EQ NULL)
                    goto WSFULL_EXIT;
                break;

            case ARRAY_VFP:
            case ARRAY_HC2V:
            case ARRAY_HC4V:
            case ARRAY_HC8V:
                // Set the immediate type
                immTypeRes = TranslateArrayTypeToImmType (aplTypeRes);  // Relying upon order in ARRAY_TYPES

                // If the right arg is zero, the result is zero
                //   as per SVD
                if (IsZeroHCxV (lpMemRht, iHCDimRht))
                    // Make a copy of the right arg so we can copy it to the result
                    (*aTypeActPromote[aplTypeRes][aplTypeRes]) (lpMemRht, 0, &atRes);
                else
                // Split cases based upon the storage type
                switch (aplTypeRht)
                {
                    case ARRAY_HC1V:
                        // Invert it
                        PrimFnMonColonBarVisV       (&atRes.aplHC1V, 0, lpMemRht, NULL);

                        break;

                    case ARRAY_HC2V:
                        // Invert it
                        PrimFnMonColonBarHC2VisHC2V (&atRes.aplHC2V, 0, lpMemRht, NULL);

                        break;

                    case ARRAY_HC4V:
                        // Invert it
                        PrimFnMonColonBarHC4VisHC4V (&atRes.aplHC4V, 0, lpMemRht, NULL);

                        break;

                    case ARRAY_HC8V:
                        // Invert it
                        PrimFnMonColonBarHC8VisHC8V (&atRes.aplHC8V, 0, lpMemRht, NULL);

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Save in the result
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
                lpYYRes->tkToken.tkFlags.ImmType   = immTypeRes;
                lpYYRes->tkToken.tkData.tkGlbData  =
                  MakeGlbEntry_EM (aplTypeRes,
                                  &atRes,
                                   FALSE,
                                   lptkFunc);
                // Check for errors
                if (lpYYRes->tkToken.tkData.tkGlbData EQ NULL)
                    goto WSFULL_EXIT;
                break;

            defstop
                break;
        } // End SWITCH

        // Split cases basd upon the right arg rank
        switch (aplRankRht)
        {
            case 0:
                break;

            case 1:
            case 2:
                // Point to right arg dimensions
                lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

                // Convert the scalar result to a one-element vector or 1x1 matrix
                hGlbRes = AllocateGlobalArray (aplTypeRes, 1, aplRankRht, lpMemDimRht);
                if (hGlbRes EQ NULL)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemHdrRes = MyGlobalLockVar (hGlbRes);

                // Skip over the header and dimensions to the data
                lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

                // If the result is an immediate, ...
                if (lpYYRes->tkToken.tkFlags.TknType EQ TKT_VARIMMED)
                {
                    // Fill in the global memory handle
                    iSizeofRes = TranslateArrayTypeToSizeof (aplTypeRes);
                    CopyMemory (lpMemRes, &lpYYRes->tkToken.tkData.tkLongest, iSizeofRes);

                    // Change the token type to that of an array
                    lpYYRes->tkToken.tkFlags.TknType = TKT_VARARRAY;
                } else
                {
                    // Get the singleton global memory handle
                    hGlbSgl = lpYYRes->tkToken.tkData.tkGlbData;

                    // Lock the memory to get a ptr to it
                    lpMemHdrSgl = MyGlobalLockVar (hGlbSgl);

                    // Skip over the header and dimensions to the data
                    lpMemSgl = VarArrayDataFmBase (lpMemHdrSgl);

                    // Fill in the global memory handle
                    iSizeofRes = TranslateArrayTypeToSizeof (aplTypeRes);
                    CopyMemory (lpMemRes, lpMemSgl, iSizeofRes);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbSgl); lpMemHdrSgl = NULL;

                    // We no longer need this storage
                    MyGlobalFree (hGlbSgl); hGlbSgl = NULL;
                } // End IF/ELSE

                // Save in the result
                lpYYRes->tkToken.tkData.tkGlbData = MakePtrTypeGlb (hGlbRes);

                // We no longer need this ptr
                MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                break;

            defstop
                break;
        } // End SWITCH

        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // From here on, the right arg (and the result) is a vector or matrix
    //***************************************************************

    // If the right arg is an overdetermined global numeric array, ...
    if (uNumRows > uNumCols
     && IsGlbNum (aplTypeRht))
    {
        HGLOBAL      hGlbMFO;           // Magic function/operator ...
        LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

        // Get ptr to PerTabData global memory
        lpMemPTD = GetMemPTD ();

        // Get the magic function/operator global memory handle
        hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MonDomino];

        //  Use an internal magic function/operator.
        lpYYRes =
          ExecuteMagicFunction_EM_YY (NULL,         // Ptr to left arg token
                                      lptkFunc,     // Ptr to function token
                                      NULL,         // Ptr to function strand
                                      lptkRhtArg,   // Ptr to right arg token
                                      lptkAxis,     // Ptr to axis token
                                      hGlbMFO,      // Magic function/operator global memory handle
                                      NULL,         // Ptr to HSHTAB struc (may be NULL)
                                      LINENUM_ONE); // Starting line # type (see LINE_NUMS)
        goto NORMAL_EXIT;
    } // End IF

    // The rank of the result is the same as
    //   that of the right arg
    aplRankRes = aplRankRht;

    // Save the NELM of the result
    Assert (aplNELMRht EQ (uNumRows * uNumCols));
////aplNELMRes = uNumRows * uNumCols;
    aplNELMRes = aplNELMRht;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = (IsEmpty (aplNELMRes) && IsSimpleNum (aplTypeRes))
                         ? ARRAY_BOOL
                         : aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Fill in the dimensions
    if (IsVector (aplRankRes))
        *VarArrayBaseToDim (lpMemHdrRes) = uNumRows;
    else
    {
        // Reverse dimensions from right arg
        (VarArrayBaseToDim (lpMemHdrRes))[0] = uNumCols;
        (VarArrayBaseToDim (lpMemHdrRes))[1] = uNumRows;
    } // End IF/ELSE

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    //***************************************************************
    // lpMemRes and lpMemRht now point to their data
    //***************************************************************

    // Check for no rows as gsl_linalg_SV_decomp doesn't handle it well
    if (uNumRows EQ 0)
        goto YYALLOC_EXIT;

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_FLOAT:
            // Attempt to invert the HC1F matrix using GSL Real
            switch (InvertHC1F (uNumRows,           // # rows in the right arg
                                uNumCols,           // # cols ...
                                aplTypeRht,         // Right arg storage type
                                aplNELMRht,         // ...       NELM
                                aplRankRht,         // ...       rank
                                iHCDimRht,          // ...       HC Dimension (1, 2, 4, 8)
                                lpMemRht,           // Ptr to right arg global memory data
                                lpMemRes,           // Ptr to result    global memory data
                                lpMemPTD,           // Ptr to PerTabData global memory
                                lpbCtrlBreak,       // Ptr to Ctrl-Break flag
                       (LPVOID) lpGslMatrixU,       // Ptr to GSL temp
                       (LPVOID) lpGslMatrixV,       // ...
                       (LPVOID) lpGslVectorS,       // ...
                       (LPVOID) lpGslVectorW,       // ...
                       (LPVOID) lpGslVectorI))      // ...
            {
                case 0:
                    break;

                case 1:
                    goto WSFULL_EXIT;

                case 2:
                    goto DOMAIN_EXIT;

                case 3:
                    goto ERROR_EXIT;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_HC2F:
            // Set the temp storage type
            aplTypeTmp = ARRAY_FLOAT;

            // Can we convert the right arg to HC1F?
            hGlbTmp =
              DemoteMemToHCxF (aplTypeRht,      // Right arg storage type
                               aplNELMRht,      // ...       NELM
                               lpMemRht,        // Ptr to right arg global memory data
                               aplTypeTmp);     // Result storage type
            // If it worked, ...
            if (hGlbTmp NE NULL)
            {
                // Lock the memory to get a ptr to it
                lpMemTmp = MyGlobalLockInt (hGlbTmp);

                // Note that we re-use hGlbRes although it is bigger than we need

                // Attempt to invert the HC1F matrix using GSL Real instead of GSL Complex
                uRet =
                  InvertHC1F (uNumRows,           // # rows in the right arg
                              uNumCols,           // # cols ...
                              aplTypeTmp,         // Right arg storage type
                              aplNELMRht,         // ...       NELM
                              aplRankRht,         // ...       rank
                              iHCDimRht,          // ...       HC Dimension (1, 2, 4, 8)
                              lpMemTmp,           // Ptr to right arg global memory data
                              lpMemRes,           // Ptr to result    global memory data
                              lpMemPTD,           // Ptr to PerTabData global memory
                              lpbCtrlBreak,       // Ptr to Ctrl-Break flag
                     (LPVOID) lpGslMatrixU,       // Ptr to GSL temp
                     (LPVOID) lpGslMatrixV,       // ...
                     (LPVOID) lpGslVectorS,       // ...
                     (LPVOID) lpGslVectorW,       // ...
                     (LPVOID) lpGslVectorI);      // ...
                // We no longer need this ptr
                MyGlobalUnlock (hGlbTmp); lpMemTmp = NULL;

                // We no longer need this storage
                MyGlobalFree (hGlbTmp); hGlbTmp = NULL;

                // Split cases based upon the return code
                switch (uRet)
                {
                    case 0:
                        // Set the new result type
                        lpMemHdrRes->ArrType = aplTypeTmp;

                        break;

                    case 1:
                        goto WSFULL_EXIT;

                    case 2:
                        goto DOMAIN_EXIT;

                    case 3:
                        goto ERROR_EXIT;

                    defstop
                        break;
                } // End SWITCH
            } else
            {
                // Attempt to invert the HC2F matrix using GSL Complex instead of Gauss-Jordan on RATs
                switch (InvertHC2F (uNumRows,           // # rows in the right arg
                                    uNumCols,           // # cols ...
                                    aplTypeRht,         // Right arg storage type
                                    aplNELMRht,         // ...       NELM
                                    aplRankRht,         // ...       rank
                                    iHCDimRht,          // ...       HC Dimension (1, 2, 4, 8)
                                    lpMemRht,           // Ptr to right arg global memory data
                                    lpMemRes,           // Ptr to result    global memory data
                                    lpbCtrlBreak,       // Ptr to Ctrl-Break flag
                           (LPVOID) lpGslCMatrixU,      // Ptr to GSL temp
                           (LPVOID) lpGslCMatrixInv,    // ...
                           (LPVOID) lpGslPermP))        // ...
                {
                    case 0:
                        break;

                    case 1:
                        goto WSFULL_EXIT;

                    case 2:
                        goto DOMAIN_EXIT;

                    case 3:
                        goto ERROR_EXIT;

                    defstop
                        break;
                } // End SWITCH
            } // End IF/ELSE

            break;

        case ARRAY_HC4F:
        case ARRAY_HC8F:
            // Can we convert the right arg to HC1F?
            hGlbTmp =
              DemoteMemToHCxF (aplTypeRht,      // Right arg storage type
                               aplNELMRht,      // ...       NELM
                               lpMemRht,        // Ptr to right arg global memory data
                               ARRAY_FLOAT);    // Result storage type
            // If it worked, ...
            if (hGlbTmp NE NULL)
            {
                // Set the temp storage type
                aplTypeTmp = ARRAY_FLOAT;

                // Lock the memory to get a ptr to it
                lpMemTmp = MyGlobalLock000 (hGlbTmp);

                // Note that we re-use hGlbRes although it is bigger than we need

                // Attempt to invert the HC1F matrix using GSL Real instead of GSL Complex
                uRet =
                  InvertHC1F (uNumRows,           // # rows in the right arg
                              uNumCols,           // # cols ...
                              ARRAY_HC1F,         // Right arg storage type
                              aplNELMRht,         // ...       NELM
                              aplRankRht,         // ...       rank
                              iHCDimRht,          // ...       HC Dimension (1, 2, 4, 8)
                              lpMemTmp,           // Ptr to right arg global memory data
                              lpMemRes,           // Ptr to result    global memory data
                              lpMemPTD,           // Ptr to PerTabData global memory
                              lpbCtrlBreak,       // Ptr to Ctrl-Break flag
                     (LPVOID) lpGslMatrixU,       // Ptr to GSL temp
                     (LPVOID) lpGslMatrixV,       // ...
                     (LPVOID) lpGslVectorS,       // ...
                     (LPVOID) lpGslVectorW,       // ...
                     (LPVOID) lpGslVectorI);      // ...
                // We no longer need this ptr
                MyGlobalUnlock (hGlbTmp); lpMemTmp = NULL;

                // We no longer need this storage
                MyGlobalFree (hGlbTmp); hGlbTmp = NULL;

                // Split cases based upon the return code
                switch (uRet)
                {
                    case 0:
                        // Set the new result type
                        lpMemHdrRes->ArrType = aplTypeTmp;

                        break;

                    case 1:
                        goto WSFULL_EXIT;

                    case 2:
                        goto DOMAIN_EXIT;

                    case 3:
                        goto ERROR_EXIT;

                    defstop
                        break;
                } // End SWITCH

                break;
            } // End IF

            // The storage type of lpMemTmpLft & lpMemTmpRht is
            //   the same HC Dimension as that of the right arg, but as a HCxR
            //   so it can be run through GaussJordan, a HCxR-only routine.
            aplTypeTmp = uHCxR[aHCDimToIndex[iHCDimRht]];

            // Translate the temp array type to sizeof
            iSizeofTmp = TranslateArrayTypeToSizeof (aplTypeTmp);

            // Allocate a temp array to hold the FLTs when converted to RATs
            hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) (uNumRows * uNumCols * iSizeofTmp));
            if (hGlbTmp EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemTmp = MyGlobalLock000 (hGlbTmp);

            // Loop through the rows and cols
            for (uRow = 0; uRow < aplNELMRht; uRow++)
            {
                // Convert from HCxI/HCxF to HCxR
                (*aTypeActConvert[aplTypeRht][aplTypeTmp]) (lpMemRht, uRow, &atRes, NULL);

                // Copy over to the temp
                CopyMemory (&((LPBYTE) lpMemTmp)[uRow * iSizeofTmp], &atRes, (APLU3264) iSizeofTmp);

                // Zero the memory in case we might free it
                ZeroMemory (&atRes, sizeof (atRes));
            } // End FOR

            // Use Gauss-Jordan elimination to invert the matrix
            if (!GaussJordan (lpMemTmp, uNumRows, uNumCols, NULL, 0, iHCDimRht, lpbCtrlBreak))
            {
                // Free the temp RAT
                FreeRatMat (hGlbTmp, lpMemTmp, uNumRows, uNumCols, iHCDimRht); hGlbTmp = NULL; lpMemTmp = NULL;

                // Set the new temp type
                aplTypeTmp = ARRAY_HC2F;

                // Translate the temp array type to sizeof
                iSizeofTmp = TranslateArrayTypeToSizeof (aplTypeTmp);

                // Allocate a temp array to hold the HC4F/HC8Fs to HC2Fs
                hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) (uNumRows * uNumCols * iSizeofTmp));
                if (hGlbTmp EQ NULL)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemTmp = MyGlobalLock000 (hGlbTmp);

                // Loop through the rows and cols
                for (uRow = 0; uRow < aplNELMRht; uRow++)
                {
                    // Convert from HCxI/HCxF to HC2F
                    (*aTypeActConvert[aplTypeRht][aplTypeTmp]) (lpMemRht, uRow, &atRes, &bRet);

                    if (!bRet)
                    {
                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbTmp); lpMemTmp = NULL;

                        // We no longer need this stroage
                        DbgGlobalFree (hGlbTmp); hGlbTmp = NULL;

                        goto DOMAIN_EXIT;
                    } // End IF

                    // Copy over to the temp
                    CopyMemory (&((LPBYTE) lpMemTmp)[uRow * iSizeofTmp], &atRes, (APLU3264) iSizeofTmp);

                    // Zero the memory in case we might free it
                    ZeroMemory (&atRes, sizeof (atRes));
                } // End FOR

                // Note that we re-use hGlbRes although it is bigger than we need

                // Attempt to invert the HC2F matrix using GSL Complex instead of Gauss-Jordan on RATs
                uRet =
                  InvertHC2F (uNumRows,         // # rows in the right arg
                              uNumCols,         // # cols ...
                              ARRAY_HC2F,       // Right arg storage type
                              aplNELMRht,       // ...       NELM
                              aplRankRht,       // ...       rank
                              iHCDimRht,        // ...       HC Dimension (1, 2, 4, 8)
                              lpMemTmp,         // Ptr to right arg global memory data
                              lpMemRes,         // Ptr to result    global memory data
                              lpbCtrlBreak,     // Ptr to Ctrl-Break flag
                     (LPVOID) lpGslCMatrixU,    // Ptr to GSL temp
                     (LPVOID) lpGslCMatrixInv,  // ...
                     (LPVOID) lpGslPermP);      // ...
                // We no longer need this ptr
                MyGlobalUnlock (hGlbTmp); lpMemTmp = NULL;

                // We no longer need this storage
                MyGlobalFree (hGlbTmp); hGlbTmp = NULL;

                // Split cases based upon the return code
                switch (uRet)
                {
                    case 0:
                        // Set the new result type
                        lpMemHdrRes->ArrType = aplTypeTmp;

                        break;

                    case 1:
                        goto WSFULL_EXIT;

                    case 2:
                        goto DOMAIN_EXIT;

                    case 3:
                        goto ERROR_EXIT;
                } // End SWITCH

                break;
            } // End IF

            // Copy to the result and convert back to FLTs
            for (uRow = 0; uRow < (aplNELMRht * iHCDimRht); uRow++)
                // Copy and convert the entry
                ((LPAPLFLOAT) lpMemRes)[uRow] = mpq_get_d (&((LPAPLRAT) lpMemTmp)[uRow]);
            break;

        case ARRAY_RAT:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            // Loop through the rows and cols
            for (uRow = 0; uRow < (aplNELMRht * iHCDimRht); uRow++)
                // Initialize the matrix
                mpq_init_set (&((LPAPLRAT) lpMemRes)[uRow],
                              &((LPAPLRAT) lpMemRht)[uRow]);

            // Use Gauss-Jordan elimination to invert the matrix
            if (!GaussJordan (lpMemRes, uNumRows, uNumCols, NULL, 0, iHCDimRht, lpbCtrlBreak))
                goto DOMAIN_EXIT;

            break;

        case ARRAY_VFP:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            // Allocate a temp array to hold the VFPs when converted to RATs
            hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) (uNumRows * uNumCols * iHCDimRht * sizeof (APLRAT)));
            if (hGlbTmp EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemTmp = MyGlobalLock000 (hGlbTmp);

            // Loop through the rows and cols
            for (uRow = 0; uRow < (aplNELMRht * iHCDimRht); uRow++)
                // Copy and convert the entry
                mpq_init_set_fr (&((LPAPLRAT) lpMemTmp)[uRow],
                                 &((LPAPLVFP) lpMemRht)[uRow]);

            // Use Gauss-Jordan elimination to invert the matrix
            if (!GaussJordan (lpMemTmp, uNumRows, uNumCols, NULL, 0, iHCDimRht, lpbCtrlBreak))
                goto DOMAIN_EXIT;

            // Copy to the result and convert back to VFPs
            for (uRow = 0; uRow < (aplNELMRht * iHCDimRht); uRow++)
                // Copy and convert the entry
                mpfr_init_set_q (&((LPAPLVFP) lpMemRes)[uRow],
                                 &((LPAPLRAT) lpMemTmp)[uRow],
                                  MPFR_RNDN);
            break;

        defstop
            break;
    } // End SWITCH
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

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
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
    if (lpGslVectorI NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_vector_free (lpGslVectorI); lpGslVectorI = NULL;
    } // End IF

    if (lpGslVectorW NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_vector_free (lpGslVectorW); lpGslVectorW = NULL;
    } // End IF

    if (lpGslVectorS NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_vector_free (lpGslVectorS); lpGslVectorS = NULL;
    } // End IF

    if (lpGslMatrixV NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_matrix_free (lpGslMatrixV); lpGslMatrixV = NULL;
    } // End IF

    if (lpGslMatrixU NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_matrix_free (lpGslMatrixU); lpGslMatrixU = NULL;
    } // End IF

    if (lpGslCMatrixU NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_matrix_complex_free (lpGslCMatrixU); lpGslCMatrixU = NULL;
    } // End IF

    if (lpGslCMatrixInv NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_matrix_complex_free (lpGslCMatrixInv); lpGslCMatrixInv = NULL;
    } // End IF

    if (lpGslPermP NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_permutation_free (lpGslPermP); lpGslPermP = NULL;
    } // End IF

    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    // Free the temp RAT
    FreeRatMat (hGlbTmp, lpMemTmp, uNumRows, uNumCols, iHCDimRht);

    return lpYYRes;
} // End PrimFnMonDomino_EM_YY


//***************************************************************************
//  $InvertHC1F
//
//  Attempt to invert a HC1F matrix
//
//  Return value:  0 = Success
//                 1 = WSFULL_EXIT
//                 2 = DOMAIN_EXIT
//                 3 = ERROR_EXIT
//***************************************************************************

UINT InvertHC1F
    (APLDIM              uNumRows,          // # rows in the right arg
     APLDIM              uNumCols,          // # cols ...
     APLSTYPE            aplTypeRht,        // Right arg storage type
     APLNELM             aplNELMRht,        // ...       NELM
     APLRANK             aplRankRht,        // ...       rank
     int                 iHCDimRht,         // ...       HC Dimension (1, 2, 4, 8)
     LPVOID              lpMemRht,          // Ptr to right arg global memory data
     LPAPLFLOAT          lpMemRes,          // Ptr to result    global memory data
     LPPERTABDATA        lpMemPTD,          // Ptr to PerTabData global memory
     LPUBOOL             lpbCtrlBreak,      // Ptr to Ctrl-Break flag
     LPVOID             _lpGslMatrixU,      // GSL temp
     LPVOID             _lpGslMatrixV,      // ...
     LPVOID             _lpGslVectorS,      // ...
     LPVOID             _lpGslVectorW,      // ...
     LPVOID             _lpGslVectorI)      // ...

{
    APLDIM uRow,                            // Loop counter
           uCol;                            // ...
    int    ErrCode;                         // Error code

#define lpGslMatrixU    ((gsl_matrix *) _lpGslMatrixU)  // GSL temp
#define lpGslMatrixV    ((gsl_matrix *) _lpGslMatrixV)  // ...
#define lpGslVectorS    ((gsl_vector *) _lpGslVectorS)  // ...
#define lpGslVectorW    ((gsl_vector *) _lpGslVectorW)  // ...
#define lpGslVectorI    ((gsl_vector *) _lpGslVectorI)  // ...

    // Allocate space for the GSL matrices
    Assert (uNumRows EQ (APLU3264) uNumRows);
    Assert (uNumCols EQ (APLU3264) uNumCols);

    lpGslMatrixU = gsl_matrix_alloc  ((APLU3264) uNumRows, (APLU3264) uNumCols);    // M x N
    lpGslMatrixV = gsl_matrix_alloc  ((APLU3264) uNumCols, (APLU3264) uNumCols);    // N x N
    lpGslVectorS = gsl_vector_alloc  ((APLU3264) uNumCols);                         // N
    lpGslVectorW = gsl_vector_alloc  ((APLU3264) uNumCols);                         // N

    // Allocate and zero a working vector (for the identity matrix)
    lpGslVectorI = gsl_vector_calloc  ((APLU3264) uNumRows); // M

    // Check the return codes for the above allocations
    if (GSL_ENOMEM EQ (HANDLE_PTR) lpGslMatrixU
     || GSL_ENOMEM EQ (HANDLE_PTR) lpGslMatrixV
     || GSL_ENOMEM EQ (HANDLE_PTR) lpGslVectorS
     || GSL_ENOMEM EQ (HANDLE_PTR) lpGslVectorW
     || GSL_ENOMEM EQ (HANDLE_PTR) lpGslVectorI)
        goto WSFULL_EXIT;

    // Copy the right arg to the GSL matrix U
    for (uCol = 0; uCol < aplNELMRht; uCol++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        lpGslMatrixU->data[uCol] = GetNextFloat (lpMemRht, aplTypeRht, uCol);
    } // End FOR

    // Calculate the SVD (Singular Value Decomposition)
    //   with the result in U and S (S is the diagonal of the matrix)
    ErrCode =
      gsl_linalg_SV_decomp (lpGslMatrixU,               // M x N
                            lpGslMatrixV,               // N x N
                            lpGslVectorS,               // N
                            lpGslVectorW);              // N
    // Check the error code
    if (ErrCode NE GSL_SUCCESS)
        goto DOMAIN_EXIT;

    // Solve the system Ax = b
    //   where x is the result, and b is the MxM identity matrix

    // The solution rows are now L[;I] +.x V +.x (1/S) +.x U*
    //   where L is an identity matrix
    for (uRow = 0; uRow < uNumRows; uRow++)             // 0 to M-1
    {
        // If we're not at the first row, clear the previous value
        if (uRow NE 0)
            lpGslVectorI->data[uRow - 1] = 0;

        // Set the next value
        lpGslVectorI->data[uRow] = 1;

        // Call GSL function to solve for this column
        ErrCode =
          gsl_linalg_SV_solve (lpGslMatrixU,            // M x N
                               lpGslMatrixV,            // N x N
                               lpGslVectorS,            // N
                               lpGslVectorI,            // M
                               lpGslVectorW);           // N
        // Check the error code
        if (ErrCode NE GSL_SUCCESS)
            goto DOMAIN_EXIT;

        // Copy the values from W to the result
        for (uCol = 0; uCol < uNumCols; uCol++)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (lpbCtrlBreak))
                goto ERROR_EXIT;

            lpMemRes[uCol * uNumRows + uRow] = lpGslVectorW->data[uCol];
        } // End FOR
    } // End FOR

    return 0;

WSFULL_EXIT:
    return 1;

DOMAIN_EXIT:
    return 2;

ERROR_EXIT:
    return 3;

#undef  lpGslVectorI
#undef  lpGslVectorW
#undef  lpGslVectorS
#undef  lpGslMatrixV
#undef  lpGslMatrixU
} // End InvertHC1F


//***************************************************************************
//  $InvertHC2F
//
//  Attempt to invert a HC2F matrix
//
//  Return value:  0 = Success
//                 1 = WSFULL_EXIT
//                 2 = DOMAIN_EXIT
//                 3 = ERROR_EXIT
//***************************************************************************

UINT InvertHC2F
    (APLDIM              uNumRows,          // # rows in the right arg
     APLDIM              uNumCols,          // # cols ...
     APLSTYPE            aplTypeRht,        // Right arg storage type
     APLNELM             aplNELMRht,        // ...       NELM
     APLRANK             aplRankRht,        // ...       rank
     int                 iHCDimRht,         // ...       HC Dimension (1, 2, 4, 8)
     LPVOID              lpMemRht,          // Ptr to right arg global memory data
     LPVOID              lpMemRes,          // Ptr to result    global memory data
     LPUBOOL             lpbCtrlBreak,      // Ptr to Ctrl-Break flag
     LPVOID             _lpGslCMatrixU,     // GSL temp
     LPVOID             _lpGslCMatrixInv,   // ...
     LPVOID             _lpGslPermP)        // ...

{
    APLDIM  uCol;               // Loop counter
    int     ErrCode,            // Error code
            signum,             // Sign of the LU permutation
            i;                  // Loop counter
    APLUINT ByteRes;            // # bytes in the result

#define lpGslCMatrixU   ((gsl_matrix_complex *) _lpGslCMatrixU  )   // GSL temp
#define lpGslCMatrixInv ((gsl_matrix_complex *) _lpGslCMatrixInv)   // ...
#define lpGslPermP      ((gsl_permutation    *) _lpGslPermP     )   // ...

    // Allocate space for the GSL matrices
    Assert (uNumRows EQ (APLU3264) uNumRows);
    Assert (uNumCols EQ (APLU3264) uNumCols);

    lpGslCMatrixU   = gsl_matrix_complex_alloc ((APLU3264) uNumRows, (APLU3264) uNumCols);  // M x N
    lpGslCMatrixInv = gsl_matrix_complex_alloc ((APLU3264) uNumRows, (APLU3264) uNumCols);  // M x N
    lpGslPermP      = gsl_permutation_alloc    ((APLU3264) uNumRows);                       // M

    // Check the return codes for the above allocations
    if (GSL_ENOMEM EQ (HANDLE_PTR) lpGslCMatrixU
     || GSL_ENOMEM EQ (HANDLE_PTR) lpGslCMatrixInv
     || GSL_ENOMEM EQ (HANDLE_PTR) lpGslPermP)
        goto WSFULL_EXIT;

    // Copy the right arg to the GSL matrix U
    // Split cases based upon the right arg's rank
    switch (aplRankRht)
    {
////////case 0:         // Scalar
////////    lpGslMatrixU->data[0] = aplFloatRht;
////////
////////    break;
////////
        case 1:         // Vector
        case 2:         // Matrix
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_HC2I:
                    for (uCol = 0; uCol < aplNELMRht; uCol++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            ((LPAPLHC2F) lpGslCMatrixU->data)[uCol].parts[i] =
                              (APLFLOAT) ((LPAPLHC2I) lpMemRht)[uCol].parts[i];
                    } // End FOR

                    break;

                case ARRAY_HC2F:
                    Assert (sizeof (double) EQ sizeof (APLFLOAT));

                    // Calculate space needed for the result
                    ByteRes = aplNELMRht * sizeof (APLHC2F);

                    // Check for overflow
                    if (ByteRes NE (APLU3264) ByteRes)
                        goto WSFULL_EXIT;

                    // Copy the HC2F data to the GSL temp
                    CopyMemory (lpGslCMatrixU->data, lpMemRht, (APLU3264) ByteRes);

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH

    // Calculate the LU (Lower/Upper) Decomposition of the square matrix LU
    //   with the result in LU and P (P is a temporary used by LU decomp only)
    ErrCode =
      gsl_linalg_complex_LU_decomp (lpGslCMatrixU,      // M x N
                                    lpGslPermP,         // M
                                   &signum);            // Ptr to sign of the perm
    // Check the error code
    if (ErrCode NE GSL_SUCCESS)
        goto DOMAIN_EXIT;

    // Calculate the inverse
    ErrCode =
      gsl_linalg_complex_LU_invert (lpGslCMatrixU,      // M x N
                                    lpGslPermP,         // M
                                    lpGslCMatrixInv);   // M x N
    // Check the error code
    if (ErrCode NE GSL_SUCCESS)
        goto DOMAIN_EXIT;

    // Calculate space needed for the result
    ByteRes = aplNELMRht * sizeof (APLHC2F);

    // Copy the data from the inverse to the result
    CopyMemory (lpMemRes, lpGslCMatrixInv->data, (APLU3264) ByteRes);

    return 0;

WSFULL_EXIT:
    return 1;

DOMAIN_EXIT:
    return 2;

ERROR_EXIT:
    return 3;

#undef  lpGslPermP
#undef  lpGslCMatrixInv
#undef  lpGslCMatrixU
} // End InvertHC2F


//***************************************************************************
//  $FreeRatMat
//
//  Free a temp RAT matrix
//***************************************************************************

void FreeRatMat
    (HGLOBAL  hGlbTmp,          // Global memory handle to free
     LPAPLRAT lpMemTmp,         // Ptr to global memory data
     APLDIM   uNumRows,         // # rows in the right arg
     APLDIM   uNumCols,         // # cols ...
     int      iHCDimRht)        // HC Dimension (1, 2, 4, 8)

{
    APLDIM uRow,                // Loop counter
           uCol;                // ...

    // Free the temp RAT
    if (hGlbTmp NE NULL)
    {
        if (lpMemTmp NE NULL)
        {
            // Loop through the rows and cols
            for (uRow = 0; uRow < uNumRows; uRow++)
            for (uCol = 0; uCol < uNumCols; uCol++)
                Myhcxr_clear (&lpMemTmp[(uRow * uNumRows + uCol) * iHCDimRht], iHCDimRht);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbTmp); lpMemTmp = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbTmp); hGlbTmp = NULL;
    } // End IF
} // End FreeRatMat


//***************************************************************************
//  Magic function/operator for Monadic Domino for Global Numerics
//
//  Return the pseudo-inverse of an array of global numerics.
//***************************************************************************
//***************************************************************************
//  Magic function/operator for Dyadic Domino for Global Numerics
//
//  Return the pseudo-inverse of an array of global numerics.
//***************************************************************************

#include "mf_domino.h"


//***************************************************************************
//  $PrimFnDydDomino_EM_YY
//
//  Primitive function for dyadic Domino ("matrix divide")
//***************************************************************************

LPPL_YYSTYPE PrimFnDydDomino_EM_YY
    (LPTOKEN lptkLftArg,                // Ptr to left arg token
     LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    APLSTYPE            aplTypeLft,             // Left arg storage type
                        aplTypeRht,             // Right ...
                        aplTypeTmp,             // Temp      ...
                        aplTypeRes;             // Result    ...
    APLNELM             aplNELMLft,             // Left arg NELM
                        aplNELMRht,             // Right ...
                        aplNELMRes;             // Result   ...
    APLRANK             aplRankLft,             // Left arg rank
                        aplRankRht,             // Right ...
                        aplRankRes;             // Result   ...
    HGLOBAL             hGlbLft = NULL,         // Left arg global memory handle
                        hGlbRht = NULL,         // Right ...
                        hGlbRes = NULL,         // Result   ...
                        hGlbTmpLft = NULL,      // Temporary left ...
                        hGlbTmpRht = NULL;      // ...       right ...
    LPVARARRAY_HEADER   lpMemHdrLft = NULL,     // Ptr to left  arg header
                        lpMemHdrRht = NULL,     // ...    right ...
                        lpMemHdrRes = NULL;     // ...    result
    LPVOID              lpMemLft,               // Ptr to left arg global memory
                        lpMemRht,               // Ptr to right ...
                        lpMemRes;               // Ptr to result   ...
    LPVOID              lpMemTmpLft = NULL,     // Ptr to temporary left arg global memory
                        lpMemTmpRht = NULL;     // ...             right ...
    APLUINT             ByteRes;                // # bytes in the result
    APLDIM              uNumRowsLft,            //
                        uNumColsLft,            //
                        uNumRowsRht,            //
                        uNumColsRht,            //
                        uNumRowsRes,            //
                        uNumColsRes,            //
                        uRow,                   //
                        uCol;                   //
    APLFLOAT            aplFloatLft,            //
                        aplFloatRht,            //
                        aplFloatRes;            //
    UBOOL               bRet = TRUE;            // TRUE iff result is valid
    LPPL_YYSTYPE        lpYYRes = NULL;         // Ptr to the result
    gsl_matrix         *lpGslMatrixU = NULL,    // Gsl temp
                       *lpGslMatrixV = NULL;    // ...
    gsl_vector         *lpGslVectorS = NULL,    // ...
                       *lpGslVectorW = NULL,    // ...
                       *lpGslVectorX = NULL,    // ...
                       *lpGslVectorB = NULL;    // ...
    gsl_matrix_complex *lpGslCMatrixU = NULL;   // ...
    gsl_vector_complex *lpGslCVectorX = NULL;   // ...
    gsl_permutation    *lpGslPermP = NULL;      // ...
    int                 ErrCode,                // Error code
                        signum,                 // Sign of the LU permutation
                        i,                      // Loop counter
                        iHCDimLft,              // Left arg HC Dimension (1, 2, 4, 8)
                        iHCDimRht,              // Right ...
                        iHCDimRes;              // Result    ...
    APLLONGEST          aplLongestLft,          // Left arg if immediate
                        aplLongestRht;          // Right ...
    LPPLLOCALVARS       lpplLocalVars;          // Ptr to re-entrant vars
    LPUBOOL             lpbCtrlBreak;           // Ptr to Ctrl-Break flag
////APLRAT              aplRatTmp = {0};        // Temporary RAT
    ALLTYPES            atTmp = {0};            // Temp as ALLTYPES
    APLINT              iSizeofTmp,             // Temp sizeof () datatype
                        iSizeofRes;             // Result ...

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Calculate left and right arg HC Dimension (1, 2, 4, 8)
    iHCDimLft = TranslateArrayTypeToHCDim (aplTypeLft);
    iHCDimRht = TranslateArrayTypeToHCDim (aplTypeRht);

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Check for RANK ERROR
    if (IsRank3P (aplRankLft) || IsRank3P (aplRankRht))
        goto RANK_EXIT;

    // Check for DOMAIN ERROR
    if (!IsNumeric (aplTypeLft)
     || !IsNumeric (aplTypeRht))
        goto DOMAIN_EXIT;

    // If the left arg is immediate, ...
    if (lpMemHdrLft EQ NULL)
        // Point to the data
        lpMemLft = &aplLongestLft;
    else
        // Skip over the header and dimensions
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);

    // If the right arg is immediate, ...
    if (lpMemHdrRht EQ NULL)
        // Point to the data
        lpMemRht = &aplLongestRht;
    else
        // Skip over the header and dimensions
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // Calculate the # rows & cols in the right arg
    switch (aplRankRht)
    {
        case 0:                 // 1x1 matrix
            uNumRowsRht =
            uNumColsRht = 1;
            if (!IsGlbNum (aplTypeRht))
                aplFloatRht = GetNextFloat (lpMemRht, aplTypeRht, 0);

            break;

        case 1:                 // 1-col matrix
            uNumRowsRht = *VarArrayBaseToDim (lpMemHdrRht);
            uNumColsRht = 1;

            break;

        case 2:
            uNumRowsRht = (VarArrayBaseToDim (lpMemHdrRht))[0];
            uNumColsRht = (VarArrayBaseToDim (lpMemHdrRht))[1];

            break;

        defstop
            break;
    } // End SWITCH

    // Calculate the # rows & cols in the left arg
    switch (aplRankLft)
    {
        case 0:                 // 1x1 matrix
            uNumRowsLft =
            uNumColsLft = 1;
            if (!IsGlbNum (aplTypeLft))
                aplFloatLft = GetNextFloat (lpMemLft, aplTypeLft, 0);

            break;

        case 1:                 // 1-col matrix
            uNumRowsLft = *VarArrayBaseToDim (lpMemHdrLft);
            uNumColsLft = 1;

            break;

        case 2:
            uNumRowsLft = (VarArrayBaseToDim (lpMemHdrLft))[0];
            uNumColsLft = (VarArrayBaseToDim (lpMemHdrLft))[1];

            break;

        defstop
            break;
    } // End SWITCH

    // Check for LENGTH ERROR
    if (uNumRowsRht <  uNumColsRht
     || uNumRowsLft NE uNumRowsRht)
        goto LENGTH_EXIT;

    // Save the # rows & cols in the result
    uNumRowsRes = uNumColsRht;
    uNumColsRes = uNumColsLft;

    // Save the rank of the result
    aplRankRes = aplRankLft + aplRankRht;
    aplRankRes = max (aplRankRes, 2) - 2;

    // Save the NELM of the result
    aplNELMRes = uNumRowsRes * uNumColsRes;

    // Calculate the result storage type
    aplTypeRes = aTypePromote[aplTypeLft][aplTypeRht];

    // If the result is simple, ...
    if (IsSimpleNum (aplTypeRes))
        // Make it a float
        aplTypeRes = ARRAY_FLOAT;
    else
    // Promote the result storage type as appropriate
    if (IsAnyInt (aplTypeRes))
        aplTypeRes++;           // Relying on order of ARRAY_TYPES

    // Translate the array type to sizeof
    iSizeofRes = TranslateArrayTypeToSizeof (aplTypeRes);

    // Calculate result arg HC Dimension (1, 2, 4, 8)
    iHCDimRes = TranslateArrayTypeToHCDim (aplTypeRes);

    // If the args are overdetermined global numeric arrays, ...
    if (uNumRowsRht > uNumColsRht
     && !IsEmpty (aplNELMRes)
     && (IsGlbNum (aplTypeLft)
      || IsGlbNum (aplTypeRht)))
    {
        HGLOBAL      hGlbMFO;           // Magic function/operator ...
        LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

        // Get ptr to PerTabData global memory
        lpMemPTD = GetMemPTD ();

        // Get the magic function/operator global memory handle
        hGlbMFO = lpMemPTD->hGlbMFO[MFOE_DydDomino];

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
    } // End IF

    // Calculate space needed for the result
    if (!IsScalar (aplRankRes)
     || IsGlbNum (aplTypeRes))
    {
        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Allocate space for the result
        hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    lpMemHdrRes
        // Fill in the header values
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = (IsEmpty (aplNELMRes) && IsSimpleNum (aplTypeRes))
                             ? ARRAY_BOOL
                             : aplTypeRes;
////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMRes;
        lpHeader->Rank       = aplRankRes;
#undef  lpHeader

        // Fill in the dimensions
        if (IsVector (aplRankRes))
            *VarArrayBaseToDim (lpMemHdrRes) = uNumRowsRes;
        else
        if (IsMatrix (aplRankRes))
        {
            (VarArrayBaseToDim (lpMemHdrRes))[0] = uNumRowsRes;
            (VarArrayBaseToDim (lpMemHdrRes))[1] = uNumColsRes;
        } // End IF/ELSE
    } // End IF

    // Skip over the header and dimensions to the data
    if (!IsScalar (aplRankRes)
     || IsGlbNum (aplTypeRes))
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Check for no rows as gsl_linalg_SV_decomp doesn't handle it well
    if (IsEmpty (uNumRowsRht)
     || IsEmpty (aplNELMRes))
        goto YYALLOC_EXIT;

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_FLOAT:
            // Ensure no overflow
            Assert (uNumRowsLft EQ (APLU3264) uNumRowsLft);
            Assert (uNumRowsRht EQ (APLU3264) uNumRowsRht);

            // Allocate space for the GSL matrices
            lpGslMatrixU = gsl_matrix_alloc  ((APLU3264) uNumRowsRht, (APLU3264) uNumColsRht);  // M x N
            lpGslMatrixV = gsl_matrix_alloc  ((APLU3264) uNumColsRht, (APLU3264) uNumColsRht);  // N x N
            lpGslVectorS = gsl_vector_alloc  ((APLU3264) uNumColsRht);                          // N
            lpGslVectorW = gsl_vector_alloc  ((APLU3264) uNumColsRht);                          // N
            lpGslVectorX = gsl_vector_alloc  ((APLU3264) uNumColsRht);                          // N
            lpGslVectorB = gsl_vector_alloc  ((APLU3264) uNumRowsRht);                          // M

            // Check the return codes for the above allocations
            if (GSL_ENOMEM EQ (HANDLE_PTR) lpGslMatrixU
             || GSL_ENOMEM EQ (HANDLE_PTR) lpGslMatrixV
             || GSL_ENOMEM EQ (HANDLE_PTR) lpGslVectorS
             || GSL_ENOMEM EQ (HANDLE_PTR) lpGslVectorW
             || GSL_ENOMEM EQ (HANDLE_PTR) lpGslVectorX)
                goto WSFULL_EXIT;

            // Copy the right arg to the GSL matrix U
            for (uCol = 0; uCol < aplNELMRht; uCol++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                lpGslMatrixU->data[uCol] = GetNextFloat (lpMemRht, aplTypeRht, uCol);
            } // End FOR

            // Calculate the SVD (Singular Value Decomposition)
            //   with the result in U and S (S is the diagonal of the matrix)
            ErrCode =
              gsl_linalg_SV_decomp (lpGslMatrixU,               // M x N
                                    lpGslMatrixV,               // N x N
                                    lpGslVectorS,               // N
                                    lpGslVectorW);              // N
            // Check the error code
            if (ErrCode NE GSL_SUCCESS)
                goto DOMAIN_EXIT;

            // Free the GSL work vector
            gsl_vector_free (lpGslVectorW); lpGslVectorW = NULL;

            Assert (uNumRowsLft EQ lpGslVectorB->size);

            // The solution rows are now L[;I] +.x V +.x (1/S) +.x U*
            for (uCol = 0; uCol < uNumColsLft; uCol++)
            {
                // Copy the next column from L to B
                for (uRow = 0; uRow < uNumRowsLft; uRow++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    lpGslVectorB->data[uRow] = GetNextFloat (lpMemLft, aplTypeLft, uRow * uNumColsLft + uCol);
                } // End FOR

                // Call GSL function to solve for this column
                ErrCode =
                  gsl_linalg_SV_solve (lpGslMatrixU,            // M x N
                                       lpGslMatrixV,            // N x N
                                       lpGslVectorS,            // N
                                       lpGslVectorB,            // M
                                       lpGslVectorX);           // N
                // Check the error code
                if (ErrCode NE GSL_SUCCESS)
                    goto DOMAIN_EXIT;

#define lpMemData   ((LPAPLFLOAT) lpMemRes)

                // Copy X to the next column in the result
                Assert (uNumRowsRes EQ lpGslVectorX->size);
                if (IsScalar (aplRankRes))
                    aplFloatRes = lpGslVectorX->data[0];
                else
                for (uRow = 0; uRow < uNumRowsRes; uRow++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    lpMemData[uRow * uNumColsRes + uCol] = lpGslVectorX->data[uRow];
                } // End IF/ELSE/...
#undef  lpMemData
            } // End FOR

            break;

        case ARRAY_HC2F:
            // Ensure no overflow
            Assert (uNumRowsLft EQ (APLU3264) uNumRowsLft);
            Assert (uNumRowsRht EQ (APLU3264) uNumRowsRht);

            // Allocate space for the GSL matrices
            lpGslCMatrixU = gsl_matrix_complex_alloc ((APLU3264) uNumRowsRht, (APLU3264) uNumColsRht);  // M x N
            lpGslCVectorX = gsl_vector_complex_alloc ((APLU3264) uNumRowsRht);                          // M
            lpGslPermP    = gsl_permutation_alloc    ((APLU3264) uNumRowsRht);                          // M

            // Check the return codes for the above allocations
            if (GSL_ENOMEM EQ (HANDLE_PTR) lpGslCMatrixU
             || GSL_ENOMEM EQ (HANDLE_PTR) lpGslCVectorX
             || GSL_ENOMEM EQ (HANDLE_PTR) lpGslPermP)
                goto WSFULL_EXIT;

            // Copy the right arg to the GSL matrix U
            // Split cases based upon the right arg's rank
            switch (aplRankRht)
            {
////////////////case 0:         // Scalar
////////////////    lpGslCMatrixU->data[0] = aplFloatRht;
////////////////
////////////////    break;
////////////////
                case 1:         // Vector
                case 2:         // Matrix
                    // Split cases based upon the right arg storage type
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:
                        case ARRAY_INT:
                        case ARRAY_APA:
                        case ARRAY_FLOAT:
                            Assert (iHCDimRht EQ 1);

                            for (uCol = 0; uCol < aplNELMRht; uCol++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                ((LPAPLHC2F) lpGslCMatrixU->data)[uCol].parts[0] = GetNextFloat (lpMemRht, aplTypeRht, uCol);
                                ((LPAPLHC2F) lpGslCMatrixU->data)[uCol].parts[1] = 0;
                            } // End FOR

                            break;

                        case ARRAY_HC2I:
                            for (uCol = 0; uCol < aplNELMRht; uCol++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                // Loop through all of the parts
                                for (i = 0; i < iHCDimRht; i++)
                                    ((LPAPLHC2F) lpGslCMatrixU->data)[uCol].parts[i] = (APLFLOAT) ((LPAPLHC2I) lpMemRht)[uCol].parts[i];
                            } // End FOR

                            break;

                        case ARRAY_HC2F:
                            Assert (sizeof (double) EQ sizeof (APLFLOAT));

                            // Calculate space needed for the result
                            ByteRes = aplNELMRht * sizeof (APLHC2F);

                            // Check for overflow
                            if (ByteRes NE (APLU3264) ByteRes)
                                goto WSFULL_EXIT;

                            // Copy the HC2F data to the GSL temp
                            CopyMemory (lpGslCMatrixU->data, lpMemRht, (APLU3264) ByteRes);

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            // Calculate the LU (Lower/Upper) Decomposition of the square matrix LU
            //   with the result in LU and P (P is a temporary used by LU decomp only)
            ErrCode =
              gsl_linalg_complex_LU_decomp (lpGslCMatrixU,      // M x N
                                            lpGslPermP,         // M
                                           &signum);            // Ptr to sign of the perm
            // Check the error code
            if (ErrCode NE GSL_SUCCESS)
                goto DOMAIN_EXIT;

            // The solution rows are now L[;I] +.x V +.x (1/S) +.x U*
            for (uCol = 0; uCol < uNumColsLft; uCol++)
            {
                // Copy the next column from L to B
                if (IsScalar (aplRankLft))
                    lpGslCVectorX->data[0] = aplFloatLft;
                else
                // Split cases based upon the left arg's storage type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                    case ARRAY_APA:
                    case ARRAY_FLOAT:
                        Assert (uNumRowsLft EQ lpGslCVectorX->size);
                        Assert (iHCDimLft EQ 1);

                        for (uRow = 0; uRow < uNumRowsLft; uRow++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Copy the parts to the temp
                            lpGslCVectorX->data[uRow * iHCDimRes + 0] =
                              GetNextFloat (lpMemLft, aplTypeLft, uRow * uNumColsLft + uCol);
                            lpGslCVectorX->data[uRow * iHCDimRes + 1] = 0.0;
                        } // End FOR

                        break;

                    case ARRAY_HC2I:
                        Assert (uNumRowsLft EQ lpGslCVectorX->size);

                        for (uRow = 0; uRow < uNumRowsLft; uRow++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Loop through all of the parts
                            for (i = 0; i < iHCDimLft; i++)
                                // Copy the parts to the temp
                                lpGslCVectorX->data[uRow * iHCDimRes + i] =
                                  (APLFLOAT) ((LPAPLINT) lpMemLft)[(uRow * uNumColsLft + uCol) * iHCDimLft + i];
                        } // End FOR

                        break;

                    case ARRAY_HC2F:
                        Assert (uNumRowsLft EQ lpGslCVectorX->size);

                        for (uRow = 0; uRow < uNumRowsLft; uRow++)
                        {
                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Loop through all of the parts
                            for (i = 0; i < iHCDimLft; i++)
                                // Copy the parts to the temp
                                lpGslCVectorX->data[uRow * iHCDimRes + i] =
                                  ((LPAPLFLOAT) lpMemLft)[(uRow * uNumColsLft + uCol) * iHCDimLft + i];
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Solve the linear system
                ErrCode =
                  gsl_linalg_complex_LU_svx (lpGslCMatrixU,         // M x N
                                             lpGslPermP,            // M
                                             lpGslCVectorX);        // M
                // Check the error code
                if (ErrCode NE GSL_SUCCESS)
                    goto DOMAIN_EXIT;

                // Copy X to the next column in the result
                Assert (uNumRowsRes EQ lpGslCVectorX->size);
                if (IsScalar (aplRankRes))
                    aplFloatRes = lpGslCVectorX->data[0];
                else
                for (uRow = 0; uRow < uNumRowsRes; uRow++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRes; i++)
                        // Copy the parts to the result
                        ((LPAPLHC2F) lpMemRes)[uRow * uNumColsLft + uCol].parts[i] =
                          ((LPAPLHC2F) lpGslCVectorX->data)[uRow].parts[i];
                } // End IF/ELSE/...
            } // End FOR

            break;

        case ARRAY_HC4F:
        case ARRAY_HC8F:

        case ARRAY_RAT:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:

        case ARRAY_VFP:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            // Ensure no overflow
            Assert (uNumRowsLft EQ (APLU3264) uNumRowsLft);
            Assert (uNumRowsRht EQ (APLU3264) uNumRowsRht);

            // The storage type of lpMemTmpLft & lpMemTmpRht is
            //   the same HC Dimension as that of the result, but as a HCxR
            //   so it can be run through GaussJordan, a HCxR-only routine.
            aplTypeTmp = uHCxR[aHCDimToIndex[iHCDimRes]];

            // Translate the temp array type to sizeof
            iSizeofTmp = TranslateArrayTypeToSizeof (aplTypeTmp);

            // Create the temporary left and right matrices
            hGlbTmpLft = DbgGlobalAlloc (GHND, (APLU3264) (uNumRowsLft * uNumColsLft * iSizeofTmp));
            hGlbTmpRht = DbgGlobalAlloc (GHND, (APLU3264) (uNumRowsRht * uNumColsRht * iSizeofTmp));

            if (hGlbTmpLft EQ NULL
             || hGlbTmpRht EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get ptrs to 'em
            lpMemTmpLft = MyGlobalLock000 (hGlbTmpLft);
            lpMemTmpRht = MyGlobalLock000 (hGlbTmpRht);

            // Loop through the left arg rows and cols
            for (uRow = 0; uRow < uNumRowsLft; uRow++)
            for (uCol = 0; uCol < uNumColsLft; uCol++)
            {
                // Convert the left arg item to the result type as a HCxR
                (*aTypeActConvert[aplTypeLft][aplTypeTmp]) (lpMemLft, uRow * uNumColsLft + uCol, &atTmp, &bRet);

                // Check for error
                if (!bRet)
                    goto DOMAIN_EXIT;

                // Copy to the matrix entry
                CopyMemory (ByteAddr (lpMemTmpLft, (uRow * uNumColsLft + uCol) * iSizeofTmp), &atTmp, (APLU3264) iSizeofTmp);

                // Zero the memory in case we might free it
                ZeroMemory (&atTmp, sizeof (atTmp));
            } // End FOR/FOR

            // Loop through the right arg rows and cols
            for (uRow = 0; uRow < uNumRowsRht; uRow++)
            for (uCol = 0; uCol < uNumColsRht; uCol++)
            {
                // Convert the right arg item to the result type as an HCxR
                (*aTypeActConvert[aplTypeRht][aplTypeTmp]) (lpMemRht, uRow * uNumColsRht + uCol, &atTmp, NULL);

                // Copy to the matrix entry
                CopyMemory (ByteAddr (lpMemTmpRht, (uRow * uNumColsRht + uCol) * iSizeofTmp), &atTmp, (APLU3264) iSizeofTmp);

                // Zero the memory in case we might free it
                ZeroMemory (&atTmp, sizeof (atTmp));
            } // End FOR/FOR

            // Solve the equation with the result in lpMemTmpLft
            bRet = GaussJordan (lpMemTmpRht, uNumRowsRht, uNumColsRht, lpMemTmpLft, uNumColsLft, iHCDimRes, lpbCtrlBreak);

            // If it succeeded, ...
            if (bRet)
            {
                Assert (uNumColsLft EQ uNumColsRes);

                // Loop through the result rows and cols
                for (uRow = 0; uRow < uNumRowsRes; uRow++)
                for (uCol = 0; uCol < uNumColsRes; uCol++)
                {
                    // Convert the temp to the result type
                    (*aTypeActConvert[aplTypeTmp][aplTypeRes]) (lpMemTmpLft, uRow * uNumColsLft + uCol, &atTmp, NULL);

                    // Copy the converted data to the result
                    CopyMemory (ByteAddr (lpMemRes, (uRow * uNumColsLft + uCol) * iSizeofRes),
                               &atTmp,
                                (APLU3264) iSizeofRes);
                    // Zero the memory in case we might free it
                    ZeroMemory (&atTmp, sizeof (atTmp));
                } // End FOR/FOR
            } else
                goto DOMAIN_EXIT;
            break;

        case ARRAY_BOOL:                // Can't happen w/Domino
        case ARRAY_INT:                 // ...
        case ARRAY_APA:                 // ...
        case ARRAY_HC2I:                // ...
        case ARRAY_HC4I:                // ...
        case ARRAY_HC8I:                // ...
        defstop
            break;
    } // End SWITCH
YYALLOC_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    if (IsScalar (aplRankRes)
     && !IsGlbNum (aplTypeRes))
    {
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_FLOAT;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkFloat    = aplFloatRes;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } else
    {
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
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
    // Mark as in error
    bRet = FALSE;
NORMAL_EXIT:
    if (lpGslVectorB NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_vector_free (lpGslVectorB); lpGslVectorB = NULL;
    } // End IF

    if (lpGslVectorX NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_vector_free (lpGslVectorX); lpGslVectorX = NULL;
    } // End IF

    if (lpGslVectorW NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_vector_free (lpGslVectorW); lpGslVectorW = NULL;
    } // End IF

    if (lpGslVectorS NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_vector_free (lpGslVectorS); lpGslVectorS = NULL;
    } // End IF

    if (lpGslMatrixV NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_matrix_free (lpGslMatrixV); lpGslMatrixV = NULL;
    } // End IF

    if (lpGslMatrixU NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_matrix_free (lpGslMatrixU); lpGslMatrixU = NULL;
    } // End IF

    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        if (!bRet)
        {
            // We no longer need this storage
            FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
        } // End IF
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

    if (hGlbTmpLft NE NULL)
    {
        if (lpMemTmpLft NE NULL)
        {
            // Loop through the rows and cols
            for (uRow = 0; uRow < uNumRowsLft; uRow++)
            for (uCol = 0; uCol < uNumColsLft; uCol++)
                Myhcxr_clear (ByteAddr (lpMemTmpLft, (uRow * uNumColsLft + uCol) * iSizeofTmp), iHCDimRes);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbTmpLft); lpMemTmpLft = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbTmpLft); hGlbTmpLft = NULL;
    } // End IF

    if (hGlbTmpRht NE NULL)
    {
        if (lpMemTmpRht NE NULL)
        {
            // Loop through the rows and cols
            for (uRow = 0; uRow < uNumRowsRht; uRow++)
            for (uCol = 0; uCol < uNumColsRht; uCol++)
                Myhcxr_clear (ByteAddr (lpMemTmpRht, (uRow * uNumColsRht + uCol) * iSizeofTmp), iHCDimRes);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbTmpRht); lpMemTmpRht = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbTmpRht); hGlbTmpRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnDydDomino_EM_YY


//***************************************************************************
//  $GaussJordan
//
//  Perform Gauss-Jordan elimination on a square HCxR matrix
//   or between left and right matrices.
//***************************************************************************

UBOOL GaussJordan
    (LPVOID   lpMemRht2,            // Ptr to right arg/result as APLRAT matrix
     APLDIM   uNumRows,             // # rows in the left/right arg/result
     APLDIM   uNumColsRht,          // # cols ...
     LPVOID   lpMemLft2,            // Ptr to left arg as APLRAT matrix (may be NULL)
     APLDIM   uNumColsLft,          // # cols in the left arg
     int      iHCDimRht,            // Right arg HC Dimension (1, 2, 4, 8)
     LPUBOOL  lpbCtrlBreak)         // Ptr to Ctrl-Break flag

{
    UBOOL   bRet = FALSE;           // TRUE iff the result is valid
    APLDIM  uRow,                   // Loop counter
            uCol,                   // ...
            uCol2,                  // ...
            uInd,                   // ...
            uRowInd,                // ...
            uNumInds = 0;           // # indices
    HGLOBAL hGlbAux = NULL;         // Global memory handle for temp auxiliary matrix
    HGLOBAL hGlbInd = NULL;         // Global memory handle for temp index vector

    // Split cases based upon the HC Dimension
    switch (iHCDimRht)
    {
        case 1:
            PF_DOMINO_MAC (((LPAPLHC1R) lpMemRht2), ((LPAPLHC1R) lpMemLft2), 1);

        case 2:
            PF_DOMINO_MAC (((LPAPLHC2R) lpMemRht2), ((LPAPLHC2R) lpMemLft2), 2);

        case 4:
            PF_DOMINO_MAC (((LPAPLHC4R) lpMemRht2), ((LPAPLHC4R) lpMemLft2), 4);

        case 8:
            PF_DOMINO_MAC (((LPAPLHC8R) lpMemRht2), ((LPAPLHC8R) lpMemLft2), 8);

        defstop
            return FALSE;
    } // End SWITCH
} // End GaussJordan


//***************************************************************************
//  $DisplayRat
//
//  Display a RAT matrix
//***************************************************************************

void DisplayRat
    (LPWCHAR   lpwszTitle,
     LPAPLHC4R lpMemRht,
     APLDIM    uNumRows,
     APLDIM    uNumCols)

{
    APLDIM  uRow,                       // Loop counter
            uCol;                       // ...
    WCHAR   wszTemp[1024];
    LPWCHAR lpwszTemp;

    // Display the title
    DbgMsgW (lpwszTitle);

    // Loop through the rows
    for (uRow = 0; uRow < uNumRows; uRow++)
    {
        lpwszTemp = &wszTemp[0];

        // Loop through the cols
        for (uCol = 0; uCol < uNumCols; uCol++)
            lpwszTemp =
              FormatAplHC4R (lpwszTemp,
                            &lpMemRht[(uRow * uNumCols) + uCol]);
        DbgMsgW (wszTemp);
    } // End FOR
} // End DisplayRat


//***************************************************************************
//  End of File: pf_domino.c
//***************************************************************************
