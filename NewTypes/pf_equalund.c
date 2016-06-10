//***************************************************************************
//  NARS2000 -- Primitive Function -- EqualUnderbar/NotEqualUnderbar
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2015 Sudley Place Software

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
//  $PrimFnEqualUnderbar_EM_YY
//
//  Primitive function for monadic and dyadic EqualUnderbar ("depth" and "match")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnEqualUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnEqualUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUALUNDERBAR);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonEqualUnderbar_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydEqualUnderbar_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnEqualUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnEqualUnderbar_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic EqualUnderbar
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnEqualUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnEqualUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnEqualUnderbar_EM_YY,  // Ptr to primitive function routine
                                    lptkLftArg,                 // Ptr to left arg token
                                    lptkFunc,                   // Ptr to function token
                                    lptkRhtArg,                 // Ptr to right arg token
                                    lptkAxis);                  // Ptr to axis token (may be NULL)
} // End PrimProtoFnEqualUnderbar_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonEqualUnderbar_EM_YY
//
//  Primitive function for monadic EqualUnderbar ("depth")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonEqualUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonEqualUnderbar_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeRht;
    APLNELM      aplNELMRht;
    APLRANK      aplRankRht;
    HGLOBAL      hGlbRht;
    LPPL_YYSTYPE lpYYRes;
    APLINT       aplIntegerRes;

    // Determine how deep the argument is.
    // Simple scalars:  0
    // Simple arrays:   1
    // Nested arrays: > 1

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // If it's not nested,
    //   it's of depth 0 (scalar) or 1 (vector or higher)
    if (!IsNested (aplTypeRht))
        aplIntegerRes = !IsScalar (aplRankRht);
    else
    {
        // Split cases based upon the right arg's token type
        switch (lptkRhtArg->tkFlags.TknType)
        {
            case TKT_VARNAMED:
                // tkData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

                // It can't be immediate as that's handled above
                Assert (!lptkRhtArg->tkData.tkSym->stFlags.Imm);

                // Get the global memory handle
                hGlbRht = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbRht));

                break;          // Continue with global case

            case TKT_VARARRAY:
                // Get the global memory handle
                hGlbRht = lptkRhtArg->tkData.tkGlbData;

                // tkData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbRht));

                break;          // Continue with global case

            case TKT_VARIMMED:
            defstop
                break;
        } // End SWITCH

        // Recursively run through the elements of the nested array
        aplIntegerRes = PrimFnMonEqualUnderBarGlb_PTB (hGlbRht);
    } // End IF/ELSE

    // Check for Ctrl-Break
    if (aplIntegerRes EQ -1)
        goto ERROR_EXIT;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkInteger  = aplIntegerRes;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
ERROR_EXIT:
    return NULL;
} // End PrimFnMonEqualUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonEqualUnderBarGlb_PTB
//
//  Common subroutine to determine the depth of a global memory handle
//    whose value is sensitive to Ptr Type Bits.
//***************************************************************************

APLINT PrimFnMonEqualUnderBarGlb_PTB
    (HGLOBAL hGlbRht)

{
    LPVARARRAY_HEADER lpMemHdrRht;      // Ptr to right arg header
    LPAPLNESTED       lpMemRht;
    APLSTYPE          aplTypeRht;
    APLNELM           aplNELMRht;
    APLRANK           aplRankRht;
    APLUINT           uRes,
                      uRht,
                      uTmp;
    LPPLLOCALVARS     lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;     // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Split cases based upon the ptr bits
    switch (GetPtrTypeDir (hGlbRht))
    {
        case PTRTYPE_STCONST:
            return 0;

        case PTRTYPE_HGLOBAL:
            // It's a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbRht));

            // Lock the memory to get a ptr to it
            lpMemHdrRht = MyGlobalLock (hGlbRht);

#define lpHeader    lpMemHdrRht
            // Get the Array Type, NELM, and Rank
            aplTypeRht = lpHeader->ArrType;
            aplNELMRht = lpHeader->NELM;
            aplRankRht = lpHeader->Rank;
#undef  lpHeader

            // Handle nested prototype
            if (IsEmpty (aplNELMRht)
             && IsNested (aplTypeRht))
                aplNELMRht++;

            // Start with 0 or 1 depending upon the rank
            uRes = !IsScalar (aplRankRht);

            // If it's nested, recurse
            if (IsNested (aplTypeRht))
            {
                // Skip over the header and dimensions
                lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

                // Loop through the elements of the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Split cases based upon the ptr type of the element
                    switch (GetPtrTypeDir (lpMemRht[uRht]))
                    {
                        case PTRTYPE_STCONST:
                            break;

                        case PTRTYPE_HGLOBAL:
                            uTmp = 1 + PrimFnMonEqualUnderBarGlb_PTB (lpMemRht[uRht]);
                            uRes = max (uRes, uTmp);

                            break;

                        defstop
                            break;
                    } // End
                } // End FOR
            } // End IF

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

            break;

        defstop
            break;
    } // End SWITCH

    return uRes;
ERROR_EXIT:
    return -1;
} // End PrimFnMonEqualUnderBarGlb_PTB


//***************************************************************************
//  $PrimFnDydEqualUnderbar_EM_YY
//
//  Primitive function for dyadic EqualUnderbar ("match")
//***************************************************************************

LPPL_YYSTYPE PrimFnDydEqualUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return
      PrimFnDydEqualUnderbarCom_EM_YY (lptkLftArg,  // Ptr to left arg token
                                       lptkFunc,    // Ptr to function token
                                       lptkRhtArg,  // Ptr to right arg token
                                       lptkAxis,    // Ptr to axis token (may be NULL)
                                       FALSE);      // TRUE iff it's NotEqualUnderbar
} // End PrimFnDydEqualUnderbar_EM_YY


//***************************************************************************
//  $PrimFnDydEqualUnderbarCom_EM_YY
//
//  Common routine for dyadic EqualUnderbar ("match")
//    and NotEqualUnderbar ("mismatch")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydEqualUnderbarCom_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydEqualUnderbarCom_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis,              // Ptr to axis token (may be NULL)
     UBOOL   bNotEqual)             // TRUE iff it's NotEqualUnderbar

{
    APLSTYPE          aplTypeLft,       // Left arg storage type
                      aplTypeRht,       // Right ...
                      aplTypeTmp;       // Temp ...
    APLNELM           aplNELMLft,       // Left arg NELM
                      aplNELMRht;       // Right ...
    APLRANK           aplRankLft,       // Left arg rank
                      aplRankRht;       // Right ...
    HGLOBAL           hGlbLft,          // Left arg global memory handle
                      hGlbRht;          // Right ...
    LPVARARRAY_HEADER lpMemHdrLft,      // Ptr to left arg header
                      lpMemHdrRht;      // ...    right ...
    LPVOID            lpMemLft,         // Ptr to left arg global memory
                      lpMemRht;         // Ptr to right ...
    LPTOKEN           lptkTmpArg;       // Ptr to temp arg token
    UBOOL             bNumLft,          // TRUE iff left arg is simple numeric
                      bNumRht,          // ...      right ...
                      bHeader = TRUE;   // TRUE iff lpMemHdrLft/Rht point to header
                                        //   otherwise they point to data
    APLINT            aplIntegerLft,    // Left arg as integer
                      aplIntegerRht,    // Right ...
                      aplIntegerRes;    // Result   ...
    APLFLOAT          aplFloatLft,      // Left arg as float
                      aplFloatRht;      // Right ...
    APLCHAR           aplCharLft,       // Left arg as char
                      aplCharRht;       // Right ...
    APLLONGEST        aplLongestLft,    // Left arg as an immediate
                      aplLongestRht;    // Right ...
    LPPL_YYSTYPE      lpYYRes = NULL;   // Ptr to the result
    APLFLOAT          fQuadCT;          // []CT
    LPPLLOCALVARS     lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;     // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Initialize the result
    aplIntegerRes = FALSE;

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Determine if two arrays are identical in
    //   rank, length, and value at all levels
    //   without regard to the array representation

    // N.B.  We are relying upon type demotion here

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Because this function is commutative, we can switch
    //    the two args without loss of generality.
    // Switch the args so that the left arg is the "simpler"
    //    of the two (Simple Homogeneous < Simple Heterogeneous < Nested < RAT < VFP),
    //    and within Simple Homogeneous, BOOL < INT < FLOAT < APA < CHAR
    if (uTypeMap[aplTypeLft] > uTypeMap[aplTypeRht])
    {
        lptkTmpArg = lptkLftArg;
        lptkLftArg = lptkRhtArg;
        lptkRhtArg = lptkTmpArg;

        aplTypeTmp = aplTypeLft;
        aplTypeLft = aplTypeRht;
        aplTypeRht = aplTypeTmp;
    } // End IF

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Split based upon Simple vs. Hetero/Nested
    switch (2 * IsPtrArray (aplTypeLft)
          + 1 * IsPtrArray (aplTypeRht))
    {
        case 2 * 0 + 1 * 0:     // Lft = Simple, Rht = Simple
            // Ensure Simple Numeric vs. Simple Numeric or Char vs. Char
            bNumLft = IsNumeric (aplTypeLft);
            bNumRht = IsNumeric (aplTypeRht);
            if (bNumLft NE bNumRht)
                break;

            // If both arguments are scalars, get the
            //   first values and compare them as they
            //   might be TKT_VARIMMED and thus not pointed
            //   to by the lpMemHdrLft/Rht vars
            if (IsScalar (aplRankLft)
             && IsScalar (aplRankRht))
            {
                // If either arg is global numeric , ...
                if (IsGlbNum (aplTypeLft)
                 || IsGlbNum (aplTypeRht))
                {
                    // If the left arg is global, ...
                    if (lpMemHdrLft)
                        // Skip over the header to the data
                        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
                    else
                        // Point to the data
                        lpMemLft = &aplLongestLft;

                    // If the right arg is global, ...
                    if (lpMemHdrRht)
                        // Skip over the header to the data
                        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
                    else
                        // Point to the data
                        lpMemRht = &aplLongestRht;
                    bHeader = FALSE;
                } else
                {
                    // Get the respective first values
                    GetFirstValueToken (lptkLftArg,     // Ptr to left arg token
                                       &aplIntegerLft,  // Ptr to integer result
                                       &aplFloatLft,    // Ptr to float ...
                                       &aplCharLft,     // Ptr to WCHAR ...
                                        NULL,           // Ptr to longest ...
                                        NULL,           // Ptr to lpSym/Glb ...
                                        NULL,           // Ptr to ...immediate type ...
                                        NULL);          // Ptr to array type ...
                    GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                                       &aplIntegerRht,  // Ptr to integer result
                                       &aplFloatRht,    // Ptr to float ...
                                       &aplCharRht,     // Ptr to WCHAR ...
                                        NULL,           // Ptr to longest ...
                                        NULL,           // Ptr to lpSym/Glb ...
                                        NULL,           // Ptr to ...immediate type ...
                                        NULL);          // Ptr to array type ...
                    // Split cases into Numeric and Char
                    if (bNumLft)
                    {               // Both are numeric
                        if (IsSimpleInt (aplTypeLft)
                         && IsSimpleInt (aplTypeRht))
                            aplIntegerRes = (aplIntegerLft EQ aplIntegerRht);
                        else
                            aplIntegerRes = CompareCT (aplFloatLft, aplFloatRht, fQuadCT, NULL);
                    } else          // Both are char
                        // Compare the values
                        aplIntegerRes = (aplCharLft EQ aplCharRht);
                    break;
                } // End IF/ELSE
            } // End IF

            // If the header is present, ...
            if (bHeader)
                // Compare ordered values
                aplIntegerRes =
                  PrimFnDydEqualUnderbarSimpleOrd (lpMemHdrLft, aplTypeLft, aplNELMLft, aplRankLft,
                                                   lpMemHdrRht, aplTypeRht, aplNELMRht, aplRankRht,
                                                   bHeader    , lpbCtrlBreak);
            else
                // Compare ordered values
                aplIntegerRes =
                  PrimFnDydEqualUnderbarSimpleOrd (lpMemLft   , aplTypeLft, aplNELMLft, aplRankLft,
                                                   lpMemRht   , aplTypeRht, aplNELMRht, aplRankRht,
                                                   bHeader    , lpbCtrlBreak);
            break;

        case 2 * 0 + 1 * 1:     // Lft = Simple,        Rht = Nested/Hetero
        case 2 * 1 + 1 * 0:     // Lft = Nested/Hetero, Rht = Simple
            break;

        case 2 * 1 + 1 * 1:     // Lft = Nested/Hetero, Rht = Nested/Hetero
            aplIntegerRes =
              PrimFnDydEqualUnderbarNested (lpMemHdrLft, aplTypeLft, aplNELMLft, aplRankLft,
                                            lpMemHdrRht, aplTypeRht, aplNELMRht, aplRankRht,
                                            TRUE, lpbCtrlBreak);
            break;

        defstop
            break;
    } // End SWITCH

    // Check for Ctrl-Break
    if (aplIntegerRes EQ -1)
        goto ERROR_EXIT;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_BOOL;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkBoolean  = (APLBOOL) ((BIT0 & aplIntegerRes) NE bNotEqual);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbLft && lpMemHdrLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    if (hGlbRht && lpMemHdrRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnDydEqualUnderbarCom_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydEqualUnderbarSimpleOrd
//
//  Subroutine to compare two ordered simple arrays
//***************************************************************************

UBOOL PrimFnDydEqualUnderbarSimpleOrd
    (LPVOID   lpMemHdrLft,      // Ptr to left arg header
     APLSTYPE aplTypeLft,       // Left arg storage type
     APLNELM  aplNELMLft,       // ...      NELM
     APLRANK  aplRankLft,       // ...      rank
     LPVOID   lpMemHdrRht,      // Ptr to right arg header
     APLSTYPE aplTypeRht,       // Right arg storage type
     APLNELM  aplNELMRht,       // ...       NELM
     APLRANK  aplRankRht,       // ...       rank
     UBOOL    bHeader,          // TRUE iff lpMemHdrLft/Rht point to header
                                //   otherwise they point to data
     LPUBOOL  lpbCtrlBreak)     // Ptr to Ctrl-Break flag

{
    LPVOID        lpMemLft,         // Ptr to left arg global memory
                  lpMemRht;         // ...    right ...
    APLINT        uDim;             // Loop counter
    APLFLOAT      fQuadCT;          // []CT
    UBOOL         bRet = TRUE;      // TRUE iff the result is valid
    APLSTYPE      aplTypeCom;       // Common arg stprage type
    ALLTYPES      atLft = {0},      // Left arg as ALLTYPES
                  atRht = {0};      // Right ...
    int           iHCDim;           // HC Dimension (1, 2, 4, 8)

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Ensure same rank and # elements
    if (aplRankLft NE aplRankRht
     || aplNELMLft NE aplNELMRht)
        return FALSE;

    // Ensure the dimensions are the same
    if (!IsScalar (aplRankLft)
     && bHeader)
    {
        // Skip over the headers to the dimensions
        lpMemLft = VarArrayBaseToDim (lpMemHdrLft);
        lpMemRht = VarArrayBaseToDim (lpMemHdrRht);

        for (uDim = 0; uDim < (APLINT) aplRankLft; uDim++)
        if (*((LPAPLDIM) lpMemLft)++ NE *((LPAPLDIM) lpMemRht)++)
            return FALSE;
    } else
    // If lpMemLft/lpMemRht point to their headers, ...
    if (bHeader)
    {
        // If the left arg is global numeric, ...
        if (IsGlbNum (aplTypeLft))
            // Skip over the header and dimensions to the data
            lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
        // If the right arg is global numeric, ...
        if (IsGlbNum (aplTypeRht))
            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    } else
    {
        // Point to the data
        lpMemLft = lpMemHdrLft;
        lpMemRht = lpMemHdrRht;
    } // End IF/ELSE

    // lpMemLft and lpMemRht now point to the data

    // Calculate the common storage type
    aplTypeCom = aTypePromote[aplTypeLft][aplTypeRht];

    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDim = TranslateArrayTypeToHCDim (aplTypeCom);

    // Loop through the elements
    for (uDim = 0; bRet && uDim < (APLINT) aplNELMLft; uDim++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Promote the left arg to the common type
        (*aTypeActPromote[aplTypeLft][aplTypeCom]) (lpMemLft, uDim, &atLft);

        // Promote the right arg to the common type
        (*aTypeActPromote[aplTypeRht][aplTypeCom]) (lpMemRht, uDim, &atRht);

        // Split cases based upon the common storage type
        switch (aplTypeCom)
        {
            case ARRAY_BOOL:
                bRet = (atLft.aplBoolean EQ atRht.aplBoolean);

                break;

            case ARRAY_INT:
            case ARRAY_APA:
                bRet = (atLft.aplInteger EQ atRht.aplInteger);

                break;

            case ARRAY_FLOAT:
                bRet = CompareCT (atLft.aplFloat,
                                  atRht.aplFloat,
                                  fQuadCT,
                                  NULL);
                break;

            case ARRAY_RAT:
                // Compare the two RATs relative to []CT
#ifdef RAT_EXACT
                bRet = mpq_cmp     (&atLft.aplRat, &atRht.aplRat         ) EQ 0;
#else
                bRet = mpq_cmp_ct  ( atLft.aplRat,  atRht.aplRat, fQuadCT) EQ 0;
#endif
                break;

            case ARRAY_VFP:
                // Compare the two VFPs relative to []CT
                bRet = mpfr_cmp_ct (atLft.aplVfp, atRht.aplVfp, fQuadCT) EQ 0;

                break;

            case ARRAY_HC2I:
            case ARRAY_HC4I:
            case ARRAY_HC8I:
                // Use the Equal function
                bRet = EqualHCxIvHCxI (&atLft, &atRht, iHCDim, fQuadCT, __FUNCTION__);

                break;

            case ARRAY_HC2F:
            case ARRAY_HC4F:
            case ARRAY_HC8F:
                // Use the Equal function
                bRet = EqualHCxFvHCxF (&atLft, &atRht, iHCDim, fQuadCT, __FUNCTION__);

                break;

            case ARRAY_HC2R:
            case ARRAY_HC4R:
            case ARRAY_HC8R:
                // Use the Equal function
                bRet = EqualHCxRvHCxR (&atLft, &atRht, iHCDim, fQuadCT, __FUNCTION__);

                break;

            case ARRAY_HC2V:
            case ARRAY_HC4V:
            case ARRAY_HC8V:
                // Use the Equal function
                bRet = EqualHCxVvHCxV (&atLft, &atRht, iHCDim, fQuadCT, __FUNCTION__);

                break;

            case ARRAY_HETERO:
            {
                IMM_TYPES immTypeLft,       // Left arg immediate type
                          immTypeRht;       // Right ...

                // Get the two immediate types
                immTypeLft = atLft.aplHetero->stFlags.ImmType;
                immTypeRht = atRht.aplHetero->stFlags.ImmType;

                // If the immediate types are incompatible, ...
                if (IsImmChr (immTypeLft) NE IsImmChr (immTypeRht))
                    bRet = FALSE;
                else
                if (IsImmChr (immTypeLft))
                    // Compare 'em
                    bRet = atLft.aplHetero->stData.stChar EQ atRht.aplHetero->stData.stChar;
                else
                {
                    APLSTYPE  aplTypeLft2,      // Left item storage type
                              aplTypeRht2,      // Right ...
                              aplTypeCom2;      // Common ...
                    ALLTYPES  atSubLft = {0},   // Left arg item as ALLTYPES
                              atSubRht = {0};   // Right ...

                    // Translate them to array type
                    aplTypeLft2 = TranslateImmTypeToArrayType (immTypeLft);
                    aplTypeRht2 = TranslateImmTypeToArrayType (immTypeRht);

                    // Calculate the common storage type
                    aplTypeCom2 = aTypePromote[aplTypeLft2][aplTypeRht2];

                    // Convert the left arg to the common type
                    (*aTypeActPromote[aplTypeLft2][aplTypeCom]) (&atLft, 0, &atSubLft);
                    (*aTypeActPromote[aplTypeRht2][aplTypeCom]) (&atRht, 0, &atSubRht);

                    // Split cases based upon the immediate type
                    switch (aplTypeCom2)
                    {
                        case ARRAY_BOOL:
                            // Compare 'em
                            bRet = atSubLft.aplBoolean EQ atSubRht.aplBoolean;

                            break;

                        case ARRAY_INT:
                            // Compare 'em
                            bRet = atSubLft.aplInteger EQ atSubRht.aplInteger;

                            break;

                        case ARRAY_FLOAT:
                            // Compare 'em
                            bRet = atSubLft.aplFloat   EQ atSubRht.aplFloat  ;

                            break;

                        case ARRAY_HC2I:
                        case ARRAY_HC4I:
                        case ARRAY_HC8I:
                        case ARRAY_HC2F:
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
                        defstop
                            break;
                    } // End SWITCH
                } // End IF/ELSE

                break;
            } // End ARRAY_HETERO

            case ARRAY_CHAR:
                bRet = (atLft.aplChar EQ atRht.aplChar);

                break;

            defstop
                break;
        } // End SWITCH

        // Free the old atLft & atRht
        (*aTypeFree[aplTypeCom]) (&atLft, 0);
        (*aTypeFree[aplTypeCom]) (&atRht, 0);
    } // End FOR

    goto NORMAL_EXIT;

ERROR_EXIT:
    bRet = -1;
NORMAL_EXIT:
    return bRet;
} // End PrimFnDydEqualUnderbarSimpleOrd


//***************************************************************************
//  $PrimFnDydEqualUnderbarSimpleUnord
//
//  Subroutine to compare two unordered simple arrays
//***************************************************************************

UBOOL PrimFnDydEqualUnderbarSimpleUnord
    (LPVOID   lpMemHdrLft,      // Ptr to left arg header
     APLSTYPE aplTypeLft,       // Left arg storage type
     APLNELM  aplNELMLft,       // ...      NELM
     APLRANK  aplRankLft,       // ...      rank
     LPVOID   lpMemHdrRht,      // Ptr to right arg header
     APLSTYPE aplTypeRht,       // Right arg storage type
     APLNELM  aplNELMRht,       // ...       NELM
     APLRANK  aplRankRht,       // ...       rank
     UBOOL    bHeader,          // TRUE iff lpMemHdrLft/Rht point to header
                                //   otherwise they point to data
     LPUBOOL  lpbCtrlBreak)     // Ptr to Ctrl-Break flag

{
    // Because this function is commutative, we can switch
    //    the two args without loss of generality.
    // Switch the args so that the left arg is the "simpler"
    //    of the two (Simple Homogeneous < Simple Heterogeneous < Nested),
    //    and within Simple Homogeneous, BOOL < INT < FLOAT < APA < CHAR
    if (uTypeMap[aplTypeLft] > uTypeMap[aplTypeRht])
        return PrimFnDydEqualUnderbarSimpleOrd (lpMemHdrRht, aplTypeRht, aplNELMRht, aplRankRht,
                                                lpMemHdrLft, aplTypeLft, aplNELMLft, aplRankLft,
                                                bHeader, lpbCtrlBreak);
    else
        return PrimFnDydEqualUnderbarSimpleOrd (lpMemHdrLft, aplTypeLft, aplNELMLft, aplRankLft,
                                                lpMemHdrRht, aplTypeRht, aplNELMRht, aplRankRht,
                                                bHeader, lpbCtrlBreak);
} // End PrimFnDydEqualUnderbarSimpleUnord


//***************************************************************************
//  $PrimFnDydEqualUnderbarNested
//
//  Subroutine to compare two nested arrays
//***************************************************************************

UBOOL PrimFnDydEqualUnderbarNested
    (LPVOID   lpMemHdrLft,      // Ptr to left arg header or data
     APLSTYPE aplTypeLft,       // Left arg storage type
     APLNELM  aplNELMLft,       // ...      NELM
     APLRANK  aplRankLft,       // ...      rank
     LPVOID   lpMemHdrRht,      // Ptr to right arg header or data
     APLSTYPE aplTypeRht,       // Right arg storage type
     APLNELM  aplNELMRht,       // ...       NELM
     APLRANK  aplRankRht,       // ...       rank
     UBOOL    bHeader,          // TRUE iff lpMemHdrLft/Rht point to header
                                //   otherwise they point to data
     LPUBOOL  lpbCtrlBreak)     // Ptr to Ctrl-Break flag

{
    APLUINT           uDim;
    APLNELM           aplNELMLoop;
    LPVARARRAY_HEADER lpMemHdrLft2,     // Ptr to left arg (secondary) header
                      lpMemHdrRht2;     // ...    right ...
    LPVOID            lpMemLft,         // Ptr to left arg global memory
                      lpMemRht;         // ...    right ...
    APLINT            aplIntegerLft,
                      aplIntegerRht;
    APLFLOAT          aplFloatLft,
                      aplFloatRht;
    APLCHAR           aplCharLft,
                      aplCharRht;
    UINT              ptrType;
    UBOOL             bRet = TRUE;
    APLFLOAT          fQuadCT;          // []CT

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Ensure same rank and # elements
    if (aplRankLft NE aplRankRht
     || aplNELMLft NE aplNELMRht)
        return FALSE;

    // Handle empty nested cases
    if (IsNested (aplTypeLft))
        aplNELMLft = max (aplNELMLft, 1);
////if (IsNested (aplTypeRht))
////    aplNELMRht = max (aplNELMRht, 1);

    // Save as loop limit
    aplNELMLoop = aplNELMLft;

    // If lpMemHdrLft/Rht point to their respective headers, ...
    if (bHeader)
    {
        // Skip over the headers to the dimensions (data if scalar)
        lpMemLft = VarArrayBaseToDim (lpMemHdrLft);
        lpMemRht = VarArrayBaseToDim (lpMemHdrRht);

        // Ensure the dimensions are the same
        for (uDim = 0; uDim < aplRankLft; uDim++)
        if (*((LPAPLDIM) lpMemLft)++ NE *((LPAPLDIM) lpMemRht)++)
            return FALSE;
    } else
    {
        // Point to the data
        lpMemLft = lpMemHdrLft;
        lpMemRht = lpMemHdrRht;
    } // End IF/ELSE

    // lpMemLft and lpMemRht now point to the data

    // Loop through the elements
    for (uDim = 0; bRet && uDim < aplNELMLoop; uDim++, ((LPAPLNESTED) lpMemLft)++, ((LPAPLNESTED) lpMemRht)++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // The ptr types must be the same
        ptrType = GetPtrTypeInd (lpMemLft);
        if (ptrType NE GetPtrTypeInd (lpMemRht))
        {
            APLSTYPE aplTypeSub;
            APLNELM  aplNELMSub;
            APLRANK  aplRankSub;
            HGLOBAL  hGlbSub;
            UBOOL    bRet;
            LPVOID   lpMemOth;

            // If the left arg is an HGLOBAL, ...
            if (ptrType EQ PTRTYPE_HGLOBAL)
            {
                // Get the attributes (Type, NELM, and Rank)
                AttrsOfGlb (*(LPAPLNESTED) lpMemLft, &aplTypeSub, &aplNELMSub, &aplRankSub, NULL);

                // If the item is not a global numeric scalar, ...
                if (!IsScalar (aplRankSub)
                 || !IsGlbNum (aplTypeSub))
                    return FALSE;

                // Get the global memory handle
                hGlbSub = *(LPAPLNESTED) lpMemLft;

                // Lock the memory to get a ptr to it
                lpMemHdrLft = MyGlobalLock (hGlbSub);

                // Skip over the header and dimensions to the data
                lpMemLft = VarArrayDataFmBase (lpMemHdrLft);

                // Point the other to its data
                lpMemOth = &(*(LPAPLHETERO) lpMemRht)->stData.stLongest;

                bRet =
                  PrimFnDydEqualUnderbarSimpleUnord (lpMemOth, TranslateImmTypeToArrayType ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType), 1, 0,
                                                     lpMemLft, aplTypeSub,                                                               1, 0,
                                                     FALSE, lpbCtrlBreak);
            } else
            {
                // Get the attributes (Type, NELM, and Rank)
                AttrsOfGlb (*(LPAPLNESTED) lpMemRht, &aplTypeSub, &aplNELMSub, &aplRankSub, NULL);

                // If the item is not a global numeric scalar, ...
                if (!IsScalar (aplRankSub)
                 || !IsGlbNum (aplTypeSub))
                    return FALSE;

                // Get the global memory handle
                hGlbSub = *(LPAPLNESTED) lpMemRht;

                // Lock the memory to get a ptr to it
                lpMemHdrRht = MyGlobalLock (hGlbSub);

                // Skip over the header and dimensions to the data
                lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

                // Point the other to its data
                lpMemOth = &(*(LPAPLHETERO) lpMemLft)->stData.stLongest;

                bRet =
                  PrimFnDydEqualUnderbarSimpleUnord (lpMemOth, TranslateImmTypeToArrayType ((*(LPAPLHETERO) lpMemLft)->stFlags.ImmType), 1, 0,
                                                     lpMemRht, aplTypeSub,                                                               1, 0,
                                                     FALSE, lpbCtrlBreak);
            } // End IF/ELSE

            // We no longer need this ptr
            MyGlobalUnlock (hGlbSub); lpMemRht = NULL;

            return bRet;
        } // End IF

        // Split cases based upon the ptr type of the elements
        switch (ptrType)
        {
            case PTRTYPE_STCONST:
                // Get the contents of the two LPSYMENTRYs
                // The index is zero because we increment the ptrs in the FOR loop
                aplTypeLft = GetNextHetero (lpMemLft, 0, &aplIntegerLft, &aplFloatLft, &aplCharLft, NULL);
                aplTypeRht = GetNextHetero (lpMemRht, 0, &aplIntegerRht, &aplFloatRht, &aplCharRht, NULL);

                // Strip out char vs. char
                if (IsSimpleChar (aplTypeLft)
                 && IsSimpleChar (aplTypeRht))
                {
                    if (aplCharLft NE aplCharRht)
                        return FALSE;
                    else
                        break;
                } // End IF

                // Strip out char vs. num and num vs. char
                if (IsSimpleChar (aplTypeLft)
                 || IsSimpleChar (aplTypeRht))
                    return FALSE;

                // Strip out BOOL/INT vs. BOOL/INT
                if (IsSimpleInt (aplTypeLft)
                 && IsSimpleInt (aplTypeRht))
                {
                    if (aplIntegerLft NE aplIntegerRht)
                        return FALSE;
                    else
                        break;
                } // End IF

                // Finally, handle FLOAT vs. FLOAT
                if (!CompareCT (aplFloatLft, aplFloatRht, fQuadCT, NULL))
                    return FALSE;
                break;

            case PTRTYPE_HGLOBAL:
                // Get the attrs (Type, NELM, and Rank) of the left and right elements
                // Note that we overwrite the incoming parameters aplTypeXXX and aplRankXXX
                //   as we no longer need those variables.
                AttrsOfGlb (*(LPAPLNESTED) lpMemLft, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
                AttrsOfGlb (*(LPAPLNESTED) lpMemRht, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

                // Ensure same rank and # elements
                if (aplRankLft NE aplRankRht
                 || aplNELMLft NE aplNELMRht)
                    return FALSE;

                // Lock the memory to get a ptr to it
                lpMemHdrLft2 = MyGlobalLock (ClrPtrTypeInd (lpMemLft));
                lpMemHdrRht2 = MyGlobalLock (ClrPtrTypeInd (lpMemRht));

                // Split based upon Simple vs. Hetero vs. Nested
                switch (2 * IsNested (aplTypeLft)
                      + 1 * IsNested (aplTypeRht))
                {
                    case 2 * 0 + 1 * 0:     // Lft = Simple, Rht = Simple
                        bRet = PrimFnDydEqualUnderbarSimpleUnord (lpMemHdrRht2, aplTypeRht, aplNELMRht, aplRankRht,
                                                                  lpMemHdrLft2, aplTypeLft, aplNELMLft, aplRankLft,
                                                                  TRUE, lpbCtrlBreak);
                        break;

                    case 2 * 0 + 1 * 1:     // Lft = Simple, Rht = Nested
                    case 2 * 1 + 1 * 0:     // Lft = Nested, Rht = Simple
                        bRet = FALSE;

                        break;

                    case 2 * 1 + 1 * 1:     // Lft = Nested, Rht = Nested
                        bRet = PrimFnDydEqualUnderbarNested (lpMemHdrLft2, aplTypeLft, aplNELMLft, aplRankLft,
                                                             lpMemHdrRht2, aplTypeRht, aplNELMRht, aplRankRht,
                                                             TRUE, lpbCtrlBreak);
                        break;

                    defstop
                        break;
                } // End SWITCH

                // We no longer need these ptrs
                MyGlobalUnlock (ClrPtrTypeInd (lpMemLft)); lpMemHdrLft2 = NULL;
                MyGlobalUnlock (ClrPtrTypeInd (lpMemRht)); lpMemHdrRht2 = NULL;

                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR

    return bRet;
ERROR_EXIT:
    return -1;
} // End PrimFnDydEqualUnderbarNested


//***************************************************************************
//  $PrimFnNotEqualUnderbar_EM_YY
//
//  Primitive function for monadic and dyadic NotEqualUnderbar (ERROR and "mismatch")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnNotEqualUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnNotEqualUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_NOTEQUALUNDERBAR);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonNotEqualUnderbar_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydNotEqualUnderbar_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnNotEqualUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnNotEqualUnderbar_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic NotEqualUnderbar
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnNotEqualUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnNotEqualUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnNotEqualUnderbar_EM_YY,   // Ptr to primitive function routine
                                    lptkLftArg,                     // Ptr to left arg token
                                    lptkFunc,                       // Ptr to function token
                                    lptkRhtArg,                     // Ptr to right arg token
                                    lptkAxis);                      // Ptr to axis token (may be NULL)
} // End PrimProtoFnNotEqualUnderbar_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonNotEqualUnderbar_EM_YY
//
//  Primitive function for monadic NotEqualUnderbar ("tally")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonNotEqualUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonNotEqualUnderbar_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    HGLOBAL      hGlbRht = NULL;    // Right arg global memory handle
    LPVOID       lpMemRht = NULL;   // Ptr to right arg global memory
    APLRANK      aplRankRht;        // Right arg rank
    APLINT       aplIntegerRes;     // The result value
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, Rank)
    //   of the arg
    AttrsOfToken (lptkRhtArg, NULL, NULL, &aplRankRht, NULL);

    // Get right arg's global ptr
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg is a non-scalar, ...
    if (!IsScalar (aplRankRht))
        // Skip over the header to the dimensions and
        //   extract the first dimension
        aplIntegerRes = *VarArrayBaseToDim (lpMemRht);
    else
        // Use a tally of 1 for a scalar
        aplIntegerRes = 1;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkInteger  = aplIntegerRes;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnMonNotEqualUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydNotEqualUnderbar_EM_YY
//
//  Primitive function for dyadic NotEqualUnderbar ("mismatch")
//***************************************************************************

LPPL_YYSTYPE PrimFnDydNotEqualUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return
      PrimFnDydEqualUnderbarCom_EM_YY (lptkLftArg,  // Ptr to left arg token
                                       lptkFunc,    // Ptr to function token
                                       lptkRhtArg,  // Ptr to right arg token
                                       lptkAxis,    // Ptr to axis token (may be NULL)
                                       TRUE);       // TRUE iff it's NotEqualUnderbar
} // End PrimFnDydNotEqualUnderbar_EM_YY


//***************************************************************************
//  End of File: pf_equalund.c
//***************************************************************************
