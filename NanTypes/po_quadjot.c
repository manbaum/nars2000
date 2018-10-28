//***************************************************************************
//  NARS2000 -- Primitive Operator -- QuadJot
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
#include "headers.h"
#define DEFINE_VALUES
#include "po_quadjot.h"
#undef  DEFINE_VALUES


//***************************************************************************
//  $PrimOpQuadJot_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    monadic operator QuadJot ("matrix op" and "matrix op")
//***************************************************************************

LPPL_YYSTYPE PrimOpQuadJot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_QUADJOT);

    return
      PrimOpQuadJotCommon_EM_YY (lptkLftArg,            // Ptr to left arg token (may be NULL)
                                 lpYYFcnStrOpr,         // Ptr to operator function strand
                                 lptkRhtArg,            // Ptr to right arg token (may be NULL if niladic)
                                 FALSE);                // TRUE iff prototyping
} // End PrimOpQuadJot_EM_YY


//***************************************************************************
//  $PrimProtoOpQuadJot_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator QuadJot ("matrix op" and "matrix op")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpQuadJot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis)             // Ptr to axis token always NULL)

{
    Assert (lptkAxis EQ NULL);

    return
      PrimOpQuadJotCommon_EM_YY (lptkLftArg,            // Ptr to left arg token (may be NULL)
                                 lpYYFcnStrOpr,         // Ptr to operator function strand
                                 lptkRhtArg,            // Ptr to right arg token (may be NULL if niladic)
                                 TRUE);                 // TRUE iff prototyping
} // End PrimProtoOpQuadJot_EM_YY


//***************************************************************************
//  $PrimOpQuadJotCommon_EM_YY
//
//  Primitive operator for monadic/dyadic derived function from QuadJot ("matrix op")
//***************************************************************************

LPPL_YYSTYPE PrimOpQuadJotCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE if prototyping

{
    LPPL_YYSTYPE      lpYYFcnStrLft;        // Ptr to function strand
////LPPRIMFNS         lpPrimProtoLft;       // Ptr to left operand prototype function
    LPTOKEN           lptkAxisOpr;          // Ptr to operator axis token (may be NULL)
    LPPL_YYSTYPE      lpYYRes = NULL;       // The result
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeRes;           // Result    ...
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMRes;           // Result    ...
    APLRANK           aplRankRht;           // Right arg rank
    APLLONGEST        aplLongestRht;        // Right arg as immediate
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL,       // Result    ...
                      hGlbRht2 = NULL;      // Base arg global memory handle
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    LPVOID            lpMemRht,             // Ptr to right arg global memory data
                      lpMemRes;             // ...    result    ...
    APLDIM            NxN[2],               // The array dimensions
                      aplColsRht,           // Right arg # cols
                      aplRowsRht;           // ...         rows
    UBOOL             bRet;                 // TRUE iff the result is valid
    APLNELM           iCntV,                // Loop counter for vectors
                      iCntM,                // ...              matrices
                      iCntZO,               // ...              result column offset
                      iCntMO,               // ...              matrix ...
                      iCnt,                 // ...
                      jCnt;                 // ...
    int               i,                    // ...
                      j,                    // ...
                      k,                    // ...
                      iHCDimRht,            // HC dimension (1, 2, 4, 8)
                      iSizeofRht;           // Byte size of a single right arg item
    // Check for axis operator
    lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive
    //   to the axis operator, so signal a syntax error if present
    //***************************************************************

    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = GetMonLftOper (lpYYFcnStrOpr, lptkAxisOpr); Assert (lpYYFcnStrLft NE NULL);

    // Ensure the left operand is a function or jot
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     && !IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_SYNTAX_EXIT;

    Assert (!bPrototyping);

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, &aplColsRht);

    // Check for DOMAIN ERRORs
    if (!IsNumeric (aplTypeRht)
     && !IsNested  (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Fill in the array dimensions
    NxN[0] =
    NxN[1] = aplColsRht;

    // Calculate the result storage type
    if (IsSimpleAPA (aplTypeRht))
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeRht;

    // Calculate the right arg HC dimension (1, 2, 4, 8)
    iHCDimRht = TranslateArrayTypeToHCDim (aplTypeRht);

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the right arg is immediate, ...
    if (lpMemHdrRht EQ NULL)
        // Point to the data
        lpMemRht = &aplLongestRht;
    else
        // Skip over the header and dimensions
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // If the left operand is a jot, ...
    if (IsTknFillJot (&lpYYFcnStrLft->tkToken))
    {
        // If the left arg is present, ...
        if (lptkLftArg NE NULL)
            goto LEFT_VALENCE_EXIT;

        // If we're to create a diagonal matrix, ...
        if (IsVector (aplRankRht))
        {
            //***************************************************************
            // Create a diagonal matrix with the (vector) right arg on the diagonal
            //***************************************************************

            // If the arg is nested, ...
            if (IsNested (aplTypeRht))
            {
                APLSTYPE          aplTypeItm,
                                  aplTypeBase;
                APLNELM           aplNELMItm;
                APLRANK           aplRankItm;
                APLDIM            aplColsItm,
                                  aplRowsItm,
                                  aplColsRes;
                HGLOBAL           hGlbItm = NULL;
                LPVARARRAY_HEADER lpMemHdrItm = NULL;
                LPVOID            lpMemItm;
                int               iHCDimItm,
                                  iHCDimRes,
                                  iCntR,
                                  iCntC;
                ALLTYPES          atItm = {0};

                // Loop through the nested right arg
                //   validating that the items are all simple or global numeric vectors or square matrices
                //   and accumulating the result Type and NELM.

                // Initialize the result type & Cols
                aplTypeRes = ARRAY_INIT;
                aplColsRes = iCntM = 0;

                // Loop through the items of the nested vector
                for (iCnt = 0; iCnt < aplNELMRht; iCnt++)
                {
                    // Get the attributes (Type, NELM, and Rank) of the item
                    AttrsOfSymGlb_PTB (((LPAPLNESTED) lpMemRht)[iCnt], &aplTypeItm, &aplNELMItm, &aplRankItm, &aplColsItm);

                    // Check for RANK ERROR
                    if (IsRank3P (aplRankItm))
                        goto RIGHT_RANK_EXIT;

                    // Promote the type as appropriate
                    aplTypeRes = aTypePromote[aplTypeRes][aplTypeItm];

                    // If it's a matrix, ensure it's square
                    if (IsMatrix (aplRankItm))
                    {
                        // If the item is empty, ...
                        if (IsZeroDim (aplColsItm))
                            aplRowsItm = 0;
                        else
                            aplRowsItm = aplNELMItm / aplColsItm;

                        // If it's not square, ...
                        if (aplColsItm NE aplRowsItm)
                            goto RIGHT_LENGTH_EXIT;
                    } // End IF

                    // Accumulate the result NELM
                    aplColsRes += aplColsItm;
                } // End FOR

                // Fill in the array dimensions
                NxN[0] =
                NxN[1] = aplColsRes;

                // Calculate the result NELM
                aplNELMRes = imul64 (aplColsRes, aplColsRes, &bRet, 0);

                // Check for overflow
                if (!bRet)
                    goto WSFULL_EXIT;

                // Calculate the item's HC dimension (1, 2, 4, 8)
                iHCDimRes = TranslateArrayTypeToHCDim (aplTypeRes);

                // Allocate a matrix
                hGlbRes = AllocateGlobalArray (aplTypeRes, aplNELMRes, 2, &NxN[0]);
                if (hGlbRes EQ NULL)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemHdrRes = MyGlobalLockVar (hGlbRes);

                // Skip over the header and dimensions to the data
                lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

                //***************************************************************
                // Copy the right arg to the diagonal of the result
                //***************************************************************

                // Initialize the result column offset (ZO)
                iCntZO = 0;

                // Calculate the base result type (IFRV)
                aplTypeBase = aToSimple[aplTypeRes];

                // If the result is a global numeric, ...
                if (IsGlbNum (aplTypeRes))
                // Initialize the entire matrix
                // Loop through the rows
                for (iCnt = 0; iCnt < aplColsRes; iCnt++)
                // Loop through the cols
                for (jCnt = 0; jCnt < aplColsRes; jCnt++)
                // Loop through all of the parts
                for (i = 0; i < iHCDimRes; i++)
                // Split cases based upon the base result storage type
                switch (aplTypeBase)
                {
                    case ARRAY_RAT:
                            // Initialize to 0/1
                            mpq_init   (&((LPAPLRAT) lpMemRes)[i + iHCDimRes * (jCnt + iCnt * aplColsRes)]);
                        break;

                    case ARRAY_VFP:
                            // Initialize to 0
                            mpfr_init0 (&((LPAPLVFP) lpMemRes)[i + iHCDimRes * (jCnt + iCnt * aplColsRes)]);
                        break;

                    case ARRAY_ARB:
                            // Initi/lize to 0/0
                            Myarb_init (&((LPAPLARB) lpMemRes)[i + iHCDimRes * (jCnt + iCnt * aplColsRes)]);
                        break;

                    defstop
                        break;
                } // End IF/FOR/...

                // Loop through the items of the nested vector
                for (iCnt = 0; iCnt < aplNELMRht; iCnt++)
                {
                    // Get the Item handle
                    hGlbItm = ((LPAPLNESTED) lpMemRht)[iCnt];

                    // Get the attributes (Type, NELM, and Rank) of the item
                    AttrsOfSymGlb_PTB (hGlbItm, &aplTypeItm, NULL, &aplRankItm, &aplColsItm);

                    // Split cases based upon the element's ptr type
                    switch (GetPtrTypeDir (hGlbItm))
                    {
                        case PTRTYPE_STCONST:
                            // Zap the headrr ptr
                            lpMemHdrItm = NULL;

                            // Point to the data
                            lpMemItm = &((LPSYMENTRY) hGlbItm)->stData.stLongest;

                            break;

                        case PTRTYPE_HGLOBAL:
                            // Lock the memory to get a ptr to it
                            lpMemHdrItm = MyGlobalLockVar (hGlbItm);

                            // Skip over the header and dimensions to the data
                            lpMemItm = VarArrayDataFmBase (lpMemHdrItm);

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // Calculate the item's HC dimension (1, 2, 4, 8)
                    iHCDimItm = TranslateArrayTypeToHCDim (aplTypeItm);

                    // Split cases based upon the result arg storage type
                    switch (aplTypeRes)
                    {
                        case ARRAY_BOOL:
                            // If the item is a matrix, ...
                            if (IsMatrix (aplRankItm))
                            {
                                // Copy the matrix to the diagonal

                                // Loop through the rows of the result offset (ZO) & item offset (MO)
                                for (iCntMO = iCntR = 0; iCntR < aplColsItm; iCntR++, iCntZO += aplColsRes - aplColsItm          )
                                // Loop through the cols of the result offset (ZO) & item offset (MO)
                                for (         iCntC = 0; iCntC < aplColsItm; iCntC++, iCntZO++                         , iCntMO++)
                                // Loop through all of the parts
                                for (i = 0; i < iHCDimItm; i++)
                                    // Copy the next value to the diagonal
                                    NextBooleanByte (                lpMemRes, i + iHCDimRes * iCntZO) |=
                                      ShiftBoolean  (GetNextBoolean (lpMemItm, i + iHCDimItm * iCntMO),
                                                                               i + iHCDimRes * iCntZO);
                                // Skip to the next result offset (ZO)
                                iCntZO += aplColsItm;
                            } else
                            // If the item is a scalar or vector, ...
                            {
                                // Copy the scalar/vector to the diagonal

                                // Loop through the item
                                for (iCntMO = 0; iCntMO < aplColsItm; iCntMO++, iCntZO += aplColsRes + 1)
                                // Loop through all of the parts
                                for (i = 0; i < iHCDimItm; i++)
                                    // Copy the next value to the diagonal
                                    NextBooleanByte (                lpMemRes, i + iHCDimRes * iCntZO) |=
                                      ShiftBoolean  (GetNextBoolean (lpMemItm, i + iHCDimItm * iCntMO),
                                                                               i + iHCDimRes * iCntZO);
                            } // End IF/ELSE

                            break;

                        case ARRAY_APA:
                        case ARRAY_INT:
                        case ARRAY_HC2I:
                        case ARRAY_HC4I:
                        case ARRAY_HC8I:

                        case ARRAY_FLOAT:       // Because INTs and FLTs are the same size
                        case ARRAY_HC2F:        //   and the following code makes no reference
                        case ARRAY_HC4F:        //   to the actual value
                        case ARRAY_HC8F:
                            // If the item is a matrix, ...
                            if (IsMatrix (aplRankItm))
                            {
                                // Copy the matrix to the diagonal

                                // Loop through the rows of the result offset (ZO) & item offset (MO)
                                for (iCntMO = iCntR = 0; iCntR < aplColsItm; iCntR++, iCntZO += aplColsRes - aplColsItm          )
                                // Loop through the cols of the result offset (ZO) & item offset (MO)
                                for (         iCntC = 0; iCntC < aplColsItm; iCntC++, iCntZO++                         , iCntMO++)
                                // Loop through all of the parts
                                for (i = 0; i < iHCDimItm; i++)
                                    // Copy the next value to the diagonal
                                    ((LPAPLINT) lpMemRes)[i + iHCDimRes * iCntZO] =
                                      GetNextInteger (lpMemItm, aplTypeItm, i + iHCDimItm * iCntMO);
                                // Skip to the next result offset (ZO)
                                iCntZO += aplColsItm;
                            } else
                            // If the item is a scalar or vector, ...
                            {
                                // Copy the scalar/vector to the diagonal

                                // Loop through the item
                                for (iCntMO = 0; iCntMO < aplColsItm; iCntMO++, iCntZO += aplColsRes + 1)
                                // Loop through all of the parts
                                for (i = 0; i < iHCDimItm; i++)
                                    // Copy the next value to the diagonal
                                    ((LPAPLINT) lpMemRes)[i + iHCDimRes * iCntZO] =
                                      GetNextInteger (lpMemItm, aplTypeItm, i + iHCDimItm * iCntMO);
                            } // End IF/ELSE

                            break;

                        case ARRAY_RAT:
                        case ARRAY_HC2R:
                        case ARRAY_HC4R:
                        case ARRAY_HC8R:

                        case ARRAY_VFP:
                        case ARRAY_HC2V:
                        case ARRAY_HC4V:
                        case ARRAY_HC8V:

                        case ARRAY_ARB:
                        case ARRAY_BA2F:
                        case ARRAY_BA4F:
                        case ARRAY_BA8F:
                            // If the item is a matrix, ...
                            if (IsMatrix (aplRankItm))
                            {
                                // Copy the matrix to the diagonal

                                // Loop through the rows of the result offset (ZO) & item offset (MO)
                                for (iCntMO = iCntR = 0; iCntR < aplColsItm; iCntR++, iCntZO += aplColsRes - aplColsItm          )
                                // Loop through the cols of the result offset (ZO) & item offset (MO)
                                for (         iCntC = 0; iCntC < aplColsItm; iCntC++, iCntZO++                         , iCntMO++)
                                {
                                    // Promote the item to a RAT/VFP/ARB
                                    (*aTypeActPromote[aplTypeItm][aplTypeRes]) (lpMemItm, iCntMO, &atItm);

                                    // Loop through all of the parts
                                    for (i = 0; i < iHCDimItm; i++)
                                    // Split cases based upon the base result type
                                    switch (aplTypeBase)
                                    {
                                        case ARRAY_RAT:
                                            // Copy the next value to the diagonal
                                            mpq_set (&((LPAPLRAT) lpMemRes)[i + iHCDimRes * iCntZO], &atItm.aplHC8R.parts[0]);

                                            break;

                                        case ARRAY_VFP:
                                            // Copy the next value to the diagonal
                                            mpfr_set (&((LPAPLVFP) lpMemRes)[i + iHCDimRes * iCntZO], &atItm.aplHC8V.parts[0], MPFR_RNDN);

                                            break;

                                        case ARRAY_ARB:
                                            // Copy the next value to the diagonal
                                            arb_set (&((LPAPLARB) lpMemRes)[i + iHCDimRes * iCntZO], &atItm.aplBA8F.parts[0]);

                                            break;

                                        defstop
                                            break;
                                    } // End FOR/SWITCH

                                    // Free the old atItm
                                    (*aTypeFree[aplTypeRes]) (&atItm, 0);
                                } // End FOR/FOR

                                // Skip to the next result offset (ZO)
                                iCntZO += aplColsItm;
                            } else
                            // If the item is a scalar or vector, ...
                            {
                                // Copy the scalar/vector to the diagonal

                                // Loop through the item
                                for (iCntMO = 0; iCntMO < aplColsItm; iCntMO++, iCntZO += aplColsRes + 1)
                                {
                                    // Promote the item to a RAT/VFP/ARB
                                    (*aTypeActPromote[aplTypeItm][aplTypeRes]) (lpMemItm, iCntMO, &atItm);

                                    // Loop through all of the parts
                                    for (i = 0; i < iHCDimItm; i++)
                                    // Split cases based upon the base result type
                                    switch (aplTypeBase)
                                    {
                                        case ARRAY_RAT:
                                            // Copy the next value to the diagonal
                                            mpq_set  (&((LPAPLRAT) lpMemRes)[i + iHCDimRes * iCntZO], &atItm.aplHC8R.parts[0]);

                                            break;

                                        case ARRAY_VFP:
                                            // Copy the next value to the diagonal
                                            mpfr_set (&((LPAPLVFP) lpMemRes)[i + iHCDimRes * iCntZO], &atItm.aplHC8V.parts[0], MPFR_RNDN);

                                            break;

                                        case ARRAY_ARB:
                                            // Copy the next value to the diagonal
                                            arb_set (&((LPAPLARB) lpMemRes)[i + iHCDimRes * iCntZO], &atItm.aplBA8F.parts[0]);

                                            break;

                                        defstop
                                            break;
                                    } // End FOR/SWITCH

                                    // Free the old atItm
                                    (*aTypeFree[aplTypeRes]) (&atItm, 0);
                                } // End FOR
                            } // End IF/ELSE

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End FOR
            } else
            {
                // Calculate the result NELM
                aplNELMRes = imul64 (aplColsRht, aplColsRht, &bRet, 0);

                // Check for overflow
                if (!bRet)
                    goto WSFULL_EXIT;

                // Allocate a matrix
                hGlbRes = AllocateGlobalArray (aplTypeRes, aplNELMRes, 2, &NxN[0]);
                if (hGlbRes EQ NULL)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemHdrRes = MyGlobalLockVar (hGlbRes);

                // Skip over the header and dimensions to the data
                lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

                //***************************************************************
                // Copy the right arg to the diagonal of the result
                //***************************************************************

                // Split cases based upon the right arg storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:
                        // Loop through the right arg
                        for (iCntV = iCntM = 0; iCntV < aplNELMRht; iCntV++, iCntM += aplNELMRht + 1)
                            // Copy the next value to the diagonal
                            ((LPAPLBOOL) lpMemRes)[iCntM >> LOG2NBIB] |=
                              GetNextInteger (lpMemRht, aplTypeRht, iCntV) << (MASKLOG2NBIB & (UINT) iCntM);
                        break;

                    case ARRAY_APA:
                        // Loop through the right arg
                        for (iCntV = iCntM = 0; iCntV < aplNELMRht; iCntV++, iCntM += aplNELMRht + 1)
                            // Copy the next value to the diagonal
                            ((LPAPLINT) lpMemRes)[iCntM] = GetNextInteger (lpMemRht, aplTypeRht, iCntV);
                        break;

                    case ARRAY_INT:
                    case ARRAY_HC2I:
                    case ARRAY_HC4I:
                    case ARRAY_HC8I:
                        // Loop through the right arg
                        for (iCntV = iCntM = 0; iCntV < aplNELMRht; iCntV++, iCntM += aplNELMRht + 1)
                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            // Copy the next value to the diagonal
                            ((LPAPLINT) lpMemRes)[i + iHCDimRht * iCntM] = ((LPAPLINT) lpMemRht)[i + iHCDimRht * iCntV];
                        break;

                    case ARRAY_FLOAT:
                    case ARRAY_HC2F:
                    case ARRAY_HC4F:
                    case ARRAY_HC8F:
                        // Loop through the right arg
                        for (iCntV = iCntM = 0; iCntV < aplNELMRht; iCntV++, iCntM += aplNELMRht + 1)
                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            // Copy the next value to the diagonal
                            ((LPAPLFLOAT) lpMemRes)[i + iHCDimRht * iCntM] = ((LPAPLFLOAT) lpMemRht)[i + iHCDimRht * iCntV];
                        break;

                    case ARRAY_RAT:
                    case ARRAY_HC2R:
                    case ARRAY_HC4R:
                    case ARRAY_HC8R:
                        // Loop through the right arg
                        for (iCntV = iCntM = 0; iCntV < aplNELMRht; iCntV++, iCntM += aplNELMRht + 1)
                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            // Copy the next value to the diagonal
                            mpq_init_set (&((LPAPLRAT) lpMemRes)[i + iHCDimRht * iCntM], &((LPAPLRAT) lpMemRht)[i + iHCDimRht * iCntV]);

                        // Initialize the off diagonal elements

                        // Loop through the rows
                        for (iCnt = 0; iCnt < aplColsRht; iCnt++)
                        // Loop through the cols
                        for (jCnt = 0; jCnt < aplColsRht; jCnt++)
                        // If it's off diagonal, ...
                        if (iCnt NE jCnt)
                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            // Initialize to 0/1
                            mpq_init (&((LPAPLRAT) lpMemRes)[i + iHCDimRht * (jCnt + iCnt * aplColsRht)]);
                        break;

                    case ARRAY_VFP:
                    case ARRAY_HC2V:
                    case ARRAY_HC4V:
                    case ARRAY_HC8V:
                        // Loop through the right arg
                        for (iCntV = iCntM = 0; iCntV < aplNELMRht; iCntV++, iCntM += aplNELMRht + 1)
                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            // Copy the next value to the diagonal
                            mpfr_init_set (&((LPAPLVFP) lpMemRes)[i + iHCDimRht * iCntM], &((LPAPLVFP) lpMemRht)[i + iHCDimRht * iCntV], MPFR_RNDN);

                        // Initialize the off diagonal elements

                        // Loop through the rows
                        for (iCnt = 0; iCnt < aplColsRht; iCnt++)
                        // Loop through the cols
                        for (jCnt = 0; jCnt < aplColsRht; jCnt++)
                        // If it's off diagonal, ...
                        if (iCnt NE jCnt)
                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            // Initialize to 0
                            mpfr_init0 (&((LPAPLVFP) lpMemRes)[i + iHCDimRht * (jCnt + iCnt * aplColsRht)]);
                        break;

                    case ARRAY_ARB:
                    case ARRAY_BA2F:
                    case ARRAY_BA4F:
                    case ARRAY_BA8F:
                        // Loop through the right arg
                        for (iCntV = iCntM = 0; iCntV < aplNELMRht; iCntV++, iCntM += aplNELMRht + 1)
                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            // Copy the next value to the diagonal
                            arb_init_set (&((LPAPLARB) lpMemRes)[i + iHCDimRht * iCntM], &((LPAPLARB) lpMemRht)[i + iHCDimRht * iCntV]);

                        // Initialize the off diagonal elements

                        // Loop through the rows
                        for (iCnt = 0; iCnt < aplColsRht; iCnt++)
                        // Loop through the cols
                        for (jCnt = 0; jCnt < aplColsRht; jCnt++)
                        // If it's off diagonal, ...
                        if (iCnt NE jCnt)
                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            // Initialize to 0/0
                            Myarb_init (&((LPAPLARB) lpMemRes)[i + iHCDimRht * (jCnt + iCnt * aplColsRht)]);
                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End IF/ELSE

            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
            lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

            // Unlock the result global memory in case TypeDemote actually demotes
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

            // See if it fits into a lower (but not necessarily smaller) datatype
            TypeDemote (&lpYYRes->tkToken, FALSE);
        } else
        // If we're to create the matrix representation, ...
        if (IsScalar (aplRankRht))
        {
            //***************************************************************
            // Create the matrix representation of the scalar right arg
            //***************************************************************

            // Calculate the right arg HC dimension (1, 2, 4, 8)
            iHCDimRht = TranslateArrayTypeToHCDim (aplTypeRht);

            // Translate the array type to sizeof
            iSizeofRht = TranslateArrayTypeToSizeof (aplTypeRht);

            // Calculate the result type (IFRV)
            aplTypeRes = aToSimple[aplTypeRht];

            // Calculate the result NELM
            aplNELMRes = iHCDimRht * iHCDimRht;

            // Fill in the array dimensions
            NxN[0] =
            NxN[1] = iHCDimRht;

            // Allocate a global square matrix of side iHCDimRht
            hGlbRes = AllocateGlobalArray (aplTypeRes, aplNELMRes, 2, &NxN[0]);
            if (hGlbRes EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemHdrRes = MyGlobalLockVar (hGlbRes);

            // Skip over the header and dimensions to the data
            lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

            // lpMemRht points to the scalar value as IFRV of any HC dimension
            // The Booolean case is handled by aplLongestRht to which lpMemRht points
            // There is no such thing as a scalar APA

            // Split cases based upon the HC dimension
            switch (iHCDimRht)
            {
                case 1:
                    // Split cases based upon the result storage type
                    switch (aplTypeRes)
                    {
                        case ARRAY_INT:
                        case ARRAY_FLOAT:
                            // Copy the single coefficient
                            CopyMemory (lpMemRes, lpMemRht, iSizeofRht);

                            break;

                        case ARRAY_RAT:
                            // Initialize and copy a HC RAT
                            mpq_init_set ((LPAPLRAT) lpMemRes, (LPAPLRAT) lpMemRht);

                            break;

                        case ARRAY_VFP:
                            // Initialize and copy a HC VFP
                            mpfr_init_set ((LPAPLVFP) lpMemRes, (LPAPLVFP) lpMemRht, MPFR_RNDN);

                            break;

                        case ARRAY_ARB:
                            // Initialize and copy a HC AFP
                            arb_init_set ((LPAPLARB) lpMemRes, (LPAPLARB) lpMemRht);

                            break;

                        case ARRAY_HC2I:
                        case ARRAY_HC2F:
                        case ARRAY_HC2R:
                        case ARRAY_HC2V:

                        case ARRAY_HC4I:
                        case ARRAY_HC4F:
                        case ARRAY_HC4R:
                        case ARRAY_HC4V:

                        case ARRAY_HC8I:
                        case ARRAY_HC8F:
                        case ARRAY_HC8R:
                        case ARRAY_HC8V:

                        case ARRAY_BA2F:
                        case ARRAY_BA4F:
                        case ARRAY_BA8F:
                        defstop
                            break;
                    } // End SWITCH

                    break;

                case 2:
                    // Represent the Complex number as a 2x2 matrix
                    //   using the standard Complex number matrix form
                    //   as A = (a, -b,
                    //           b,  a)

                    // Split cases based upon the result storage type
                    switch (aplTypeRes)
                    {
                        case ARRAY_INT:
                            MATREP_INT (2, 2);

                            break;

                        case ARRAY_FLOAT:
                            MATREP_FLT (2, 2);

                            break;

                        case ARRAY_RAT:
                            MATREP_RAT (2, 2);

                            break;

                        case ARRAY_VFP:
                            MATREP_VFP (2, 2);

                            break;

                        case ARRAY_ARB:
                            MATREP_ARB (2, 2);

                            break;

                        case ARRAY_HC2I:
                        case ARRAY_HC2F:
                        case ARRAY_HC2R:
                        case ARRAY_HC2V:

                        case ARRAY_HC4I:
                        case ARRAY_HC4F:
                        case ARRAY_HC4R:
                        case ARRAY_HC4V:

                        case ARRAY_HC8I:
                        case ARRAY_HC8F:
                        case ARRAY_HC8R:
                        case ARRAY_HC8V:

                        case ARRAY_BA2F:
                        case ARRAY_BA4F:
                        case ARRAY_BA8F:
                        defstop
                            break;
                    } // End SWITCH

                    break;

                case 4:
                    // Represent the Quaternion number as a 4x4 matrix
                    //   using one of the 48 Quaternion number matrix forms
                    //   as A = (a, -b, -c, -d,
                    //           b,  a, -d,  c,
                    //           c,  d,  a, -b,
                    //           d, -c,  b,  a)

                    // Split cases based upon the result storage type
                    switch (aplTypeRes)
                    {
                        case ARRAY_INT:
                            MATREP_INT (4, 4);

                            break;

                        case ARRAY_FLOAT:
                            MATREP_FLT (4, 4);

                            break;

                        case ARRAY_RAT:
                            MATREP_RAT (4, 4);

                            break;

                        case ARRAY_VFP:
                            MATREP_VFP (4, 4);

                            break;

                        case ARRAY_ARB:
                            MATREP_ARB (4, 4);

                            break;

                        case ARRAY_HC2I:
                        case ARRAY_HC2F:
                        case ARRAY_HC2R:
                        case ARRAY_HC2V:

                        case ARRAY_HC4I:
                        case ARRAY_HC4F:
                        case ARRAY_HC4R:
                        case ARRAY_HC4V:

                        case ARRAY_HC8I:
                        case ARRAY_HC8F:
                        case ARRAY_HC8R:
                        case ARRAY_HC8V:

                        case ARRAY_BA2F:
                        case ARRAY_BA4F:
                        case ARRAY_BA8F:
                        defstop
                            break;
                    } // End SWITCH

                    break;

                case 8:
                {
                    APLCHAR cQuadLR = GetQuadLR ();

                    // Represent the Octonion number as a 4x4 matrix
                    //   using one of the 2 Octonion number matrix forms
                    //   depending upon the setting of []LR

                    // Split cases based upon the result storage type
                    switch (aplTypeRes)
                    {
                        case ARRAY_INT:
                            // Split cases based upon the value of []LR
                            switch (cQuadLR)
                            {
                                case 'l':
                                    MATREP_INT (8, 8L);

                                    break;

                                case 'r':
                                    MATREP_INT (8, 8R);

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case ARRAY_FLOAT:
                            // Split cases based upon the value of []LR
                            switch (cQuadLR)
                            {
                                case 'l':
                                    MATREP_FLT (8, 8L);

                                    break;

                                case 'r':
                                    MATREP_FLT (8, 8R);

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case ARRAY_RAT:
                            // Split cases based upon the value of []LR
                            switch (cQuadLR)
                            {
                                case 'l':
                                    MATREP_RAT (8, 8L);

                                    break;

                                case 'r':
                                    MATREP_RAT (8, 8R);

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case ARRAY_VFP:
                            // Split cases based upon the value of []LR
                            switch (cQuadLR)
                            {
                                case 'l':
                                    MATREP_VFP (8, 8L);

                                    break;

                                case 'r':
                                    MATREP_VFP (8, 8R);

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case ARRAY_ARB:
                            // Split cases based upon the value of []LR
                            switch (cQuadLR)
                            {
                                case 'l':
                                    MATREP_ARB (8, 8L);

                                    break;

                                case 'r':
                                    MATREP_ARB (8, 8R);

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case ARRAY_HC2I:
                        case ARRAY_HC2F:
                        case ARRAY_HC2R:
                        case ARRAY_HC2V:

                        case ARRAY_HC4I:
                        case ARRAY_HC4F:
                        case ARRAY_HC4R:
                        case ARRAY_HC4V:

                        case ARRAY_HC8I:
                        case ARRAY_HC8F:
                        case ARRAY_HC8R:
                        case ARRAY_HC8V:

                        case ARRAY_BA2F:
                        case ARRAY_BA4F:
                        case ARRAY_BA8F:
                        defstop
                            break;
                    } // End SWITCH

                    break;
                } // case 8

                defstop
                    break;
            } // End SWITCH

            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
            lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;
        } else
            goto RIGHT_RANK_EXIT;
    } else
    {
        HGLOBAL      hGlbMFO;                   // Magic function/operator ...
        LPPERTABDATA lpMemPTD;                  // Ptr to PerTabData global memory
        LPTOKEN      lptkAxisOpr;               // Ptr to axis token (may be NULL)
        TOKEN        tkRhtArg = {0};            // Temp token for right arg
        HGLOBAL      hGlbRes2;                  // Result global memory handle

        //***************************************************************
        // Execute the left operand on the matrix right arg as a matrix function
        //***************************************************************

        // Validate the right arg as a square matrix
        if (!IsMatrix (aplRankRht))
            goto RIGHT_RANK_EXIT;

        if (IsZeroDim (aplColsRht))
            aplRowsRht = 0;
        else
            aplRowsRht = aplNELMRht / aplColsRht;

        // If it's not square, ...
        if (aplColsRht NE aplRowsRht)
            goto RIGHT_LENGTH_EXIT;

        // Copy the right arg token
        CopyAll (&tkRhtArg, lptkRhtArg);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

        // If the demotion to simple is NOT successful, ...
        if (!SimpleDemote (hGlbRht,
                          &hGlbRht2,
                          &bRet))
        {
            Assert (hGlbRht2 EQ NULL);

            // Check for error
            if (!bRet)
                goto RIGHT_DOMAIN_EXIT;
            else
                goto WSFULL_EXIT;
        } else
        {
            // Note that we need to delete the demoted right arg (hGlbRht2) on exit

            // If we demoted, ...
            if (hGlbRht2 NE NULL)
            {
                // Save as the new global handle and storage type
                hGlbRht = hGlbRht2;

                // Save in temp right arg token
                tkRhtArg.tkData.tkGlbData = MakePtrTypeGlb (hGlbRht);
            } // End IF
        } // End IF/ELSE

        // Get ptr to PerTabData global memory
        lpMemPTD = GetMemPTD ();

        // Get the magic function/operator global memory handle
        hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MatOpr];

        // Check for axis operator
        lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

        // Use an internal magic function/operator.
        lpYYRes =
          ExecuteMagicOperator_EM_YY (lptkLftArg,               // Ptr to left arg token
                                     &lpYYFcnStrOpr->tkToken,   // Ptr to operator token
                                      lpYYFcnStrLft,            // Ptr to left operand function strand
                                      lpYYFcnStrOpr,            // Ptr to function strand
                                      NULL,                     // Ptr to right operand function strand (may be NULL)
                                     &tkRhtArg,                 // Ptr to right arg token
                                      lptkAxisOpr,              // Ptr to axis token (may be NULL)
                                      hGlbMFO,                  // Magic function/operator global memory handle
                                      NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                      LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
        // If we demoted the right arg, ...
        if (hGlbRht2 NE NULL)
        {
            // We no longer need this storage
            FreeResultGlobalVar (hGlbRht2); hGlbRht2 = NULL;
        } // End IF

        // Check for error
        if (lpYYRes EQ NULL)
            goto ERROR_EXIT;

        //***************************************************************
        // Now see if we can demote the result
        //***************************************************************

        // Get result's global ptrs
        GetGlbPtrs_LOCK (&lpYYRes->tkToken, &hGlbRes, NULL);

        // If the demotion to simple is successful, ...
        if (SimpleDemote (hGlbRes,
                         &hGlbRes2,
                          NULL))
        {
            // If we demoted, ...
            if (hGlbRes2 NE NULL)
            {
                // Free the old result
                FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;

                // Save in the result
                lpYYRes->tkToken.tkData.tkGlbData = MakePtrTypeGlb (hGlbRes2);
            } // End IF
        } // End IF
    } // End IF/ELSE

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    goto ERROR_EXIT;

LEFT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LEFT_VALENCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALENCE_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkRhtArg);
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
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRht2 NE NULL)
    {
        // We no longer need this storage
        FreeResultGlobalVar (hGlbRht2); hGlbRht2 = NULL;
    } // End IF

    if (hGlbRes NE NULL
     && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    if (hGlbRht NE NULL
     && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimOpQuadJotCommon_EM_YY


//***************************************************************************
//  Magic functions/operators for Matrix Operator
//***************************************************************************

#include "mf_quadjot.h"


//***************************************************************************
//  End of File: po_quadjot.c
//***************************************************************************
