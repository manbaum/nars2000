//***************************************************************************
//  NARS2000 -- Primitive Function -- EqualUnderbar
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2009 Sudley Place Software

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
        return PrimFnSyntaxError_EM (lptkFunc);

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
    LPAPLNESTED   lpMemRht;
    APLSTYPE      aplTypeRht;
    APLNELM       aplNELMRht;
    APLRANK       aplRankRht;
    APLUINT       uRes,
                  uRht,
                  uTmp;
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;     // Ptr to Ctrl-Break flag

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

            // Clear the ptr type bits
            hGlbRht = ClrPtrTypeDir (hGlbRht);

            // Lock the memory to get a ptr to it
            lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)
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
                lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

                // Loop through the elements of the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Split cases based on the ptr type of the element
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
            MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

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

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydEqualUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydEqualUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE      aplTypeLft,       // Left arg storage type
                  aplTypeRht,       // Right ...
                  aplTypeTmp;       // Temp ...
    APLNELM       aplNELMLft,       // Left arg NELM
                  aplNELMRht;       // Right ...
    APLRANK       aplRankLft,       // Left arg rank
                  aplRankRht;       // Right ...
    HGLOBAL       hGlbLft,          // Left arg global memory handle
                  hGlbRht;          // Right ...
    LPVOID        lpMemLft,         // Ptr to left arg global memory
                  lpMemRht;         // Ptr to right ...
    LPTOKEN       lptkTmpArg;       // Ptr to temp arg token
    UBOOL         bNumLft,          // TRUE iff left arg is simple numeric
                  bNumRht;          // ...      right ...
    APLINT        aplIntegerLft,    // Left arg as integer
                  aplIntegerRht,    // Right ...
                  aplIntegerRes;    // Result   ...
    APLFLOAT      aplFloatLft,      // Left arg as float
                  aplFloatRht;      // Right ...
    APLCHAR       aplCharLft,       // Left arg as char
                  aplCharRht;       // Right ...
    LPPL_YYSTYPE  lpYYRes = NULL;   // Ptr to the result
    APLFLOAT      fQuadCT;          // []CT
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;     // Ptr to Ctrl-Break flag

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
    //    of the two (Simple Homogeneous < Simple Heterogeneous < Nested),
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
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Split based upon Simple vs. Hetero vs. Nested
    switch (2 * IsNested (aplTypeLft)
          + 1 * IsNested (aplTypeRht))
    {
        case 2 * 0 + 1 * 0:     // Lft = Simple, Rht = Simple
            // If both arguments are scalars, get the
            //   first values and compare them as they
            //   might be TKT_VARIMMED and thus not pointed
            //   to by the lpMemLft/Rht vars
            if (IsScalar (aplRankLft)
             && IsScalar (aplRankRht))
            {
                // Ensure Numeric vs. Numeric or Char vs. Char
                bNumLft = IsSimpleNum (aplTypeLft);
                bNumRht = IsSimpleNum (aplTypeRht);
                if (bNumLft NE bNumRht)
                    break;

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
            } // End IF

            aplIntegerRes =
              PrimFnDydEqualUnderbarSimple (lpMemLft, aplTypeLft, aplNELMLft, aplRankLft,
                                            lpMemRht, aplTypeRht, aplNELMRht, aplRankRht,
                                            TRUE, lpbCtrlBreak);
            break;

        case 2 * 0 + 1 * 1:     // Lft = Simple, Rht = Nested
        case 2 * 1 + 1 * 0:     // Lft = Nested, Rht = Simple
            break;

        case 2 * 1 + 1 * 1:     // Lft = Nested, Rht = Nested
            aplIntegerRes =
              PrimFnDydEqualUnderbarNested (lpMemLft, aplTypeLft, aplNELMLft, aplRankLft,
                                            lpMemRht, aplTypeRht, aplNELMRht, aplRankRht,
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
    lpYYRes->tkToken.tkData.tkBoolean  = (APLBOOL) (BIT0 & aplIntegerRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

ERROR_EXIT:
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

    return lpYYRes;
} // End PrimFnDydEqualUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydEqualUnderbarSimple
//
//  Subroutine to compare two simple arrays
//***************************************************************************

UBOOL PrimFnDydEqualUnderbarSimple
    (LPVOID   lpMemLft,         // Ptr to left arg header
     APLSTYPE aplTypeLft,       // Left arg storage type
     APLNELM  aplNELMLft,       // ...      NELM
     APLRANK  aplRankLft,       // ...      rank
     LPVOID   lpMemRht,         // Ptr to right arg header
     APLSTYPE aplTypeRht,       // Right arg storage type
     APLNELM  aplNELMRht,       // ...       NELM
     APLRANK  aplRankRht,       // ...       rank
     UBOOL    bHeader,          // TRUE iff lpMemLft/Rht point to header
                                //   otherwise they point to data
     LPUBOOL  lpbCtrlBreak)     // Ptr to Ctrl-Break flag

{
    APLINT        uDim,
                  apaOff,
                  apaMul;
    UINT          uBitMask = BIT0;
    APLINT        aplIntegerLft,
                  aplIntegerRht;
    APLFLOAT      aplFloatLft,
                  aplFloatRht;
    APLCHAR       aplCharLft,
                  aplCharRht;
    APLFLOAT      fQuadCT;          // []CT

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
        lpMemLft = VarArrayBaseToDim (lpMemLft);
        lpMemRht = VarArrayBaseToDim (lpMemRht);

        for (uDim = 0; uDim < (APLINT) aplRankLft; uDim++)
        if (*((LPAPLDIM) lpMemLft)++ NE *((LPAPLDIM) lpMemRht)++)
            return FALSE;
    } // End IF

    // lpMemLft and lpMemRht now point to the data

    // Split cases based upon the left arg's storage type
    switch (aplTypeLft)
    {
        case ARRAY_BOOL:            // Lft = BOOL, Rht = BOOL/INT/FLOAT/APA/CHAR/HETERO
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:    // Lft = BOOL, Rht = BOOL
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Optimize for Booleans
                        if ((aplNELMLft - uDim) >= BITS_IN_APLINT)
                        {
                            if ((*((LPAPLINT) lpMemLft))
                             NE (*((LPAPLINT) lpMemRht)))
                                return FALSE;
                            // Increment the loop counter
                            uDim += BITS_IN_APLINT - 1;

                            // Increment the global memory ptrs
                            ((LPAPLINT) lpMemLft)++;
                            ((LPAPLINT) lpMemRht)++;
                        } else
                        {
                            if ((uBitMask & *((LPAPLBOOL) lpMemLft))
                             NE (uBitMask & *((LPAPLBOOL) lpMemRht)))
                                return FALSE;

                            // Shift over the bit mask
                            uBitMask <<= 1;

                            // Check for end-of-byte
                            if (uBitMask EQ END_OF_BYTE)
                            {
                                uBitMask = BIT0;            // Start over
                                ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                                ((LPAPLBOOL) lpMemRht)++;   // ...
                            } // End IF
                        } // End IF/ELSE
                    } // End FOR

                    return TRUE;

                case ARRAY_INT:     // Lft = BOOL, Rht = INT
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        if (((uBitMask & *((LPAPLBOOL) lpMemLft)) ? TRUE : FALSE)
                         NE *((LPAPLINT) lpMemRht)++)
                            return FALSE;

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;            // Start over
                            ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    return TRUE;

                case ARRAY_FLOAT:   // Lft = BOOL, Rht = FLOAT
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        if (((uBitMask & *((LPAPLBOOL) lpMemLft)) ? TRUE : FALSE)
                         NE *((LPAPLFLOAT) lpMemRht)++)
                            return FALSE;

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;            // Start over
                            ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    return TRUE;

                case ARRAY_APA:     // Lft = BOOL, Rht = APA
#define lpAPA       ((LPAPLAPA) lpMemRht)
                    // Get the APA parameters
                    apaOff = lpAPA->Off;
                    apaMul = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        if (((uBitMask & *((LPAPLBOOL) lpMemLft)) ? TRUE : FALSE)
                         NE (apaOff + apaMul * uDim))
                            return FALSE;

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;            // Start over
                            ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    return TRUE;

                case ARRAY_CHAR:    // Lft = BOOL, Rht = CHAR
                    return FALSE;

                case ARRAY_HETERO:  // Lft = BOOL, Rht = HETERO
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Split cases based upon the hetero's storage type
                        switch (GetNextHetero (lpMemRht, uDim, &aplIntegerRht, &aplFloatRht, &aplCharRht))
                        {
                            case ARRAY_BOOL:    // Lft = BOOL, Rht = BOOL
                            case ARRAY_INT:     // Lft = BOOL, Rht = INT
                                if (((uBitMask & *((LPAPLBOOL) lpMemLft)) ? TRUE : FALSE) NE aplIntegerRht)
                                    return FALSE;
                                break;

                            case ARRAY_FLOAT:   // Lft = BOOL, Rht = FLOAT
                                if (((uBitMask & *((LPAPLBOOL) lpMemLft)) ? TRUE : FALSE) NE aplFloatRht)
                                    return FALSE;
                                break;

                            case ARRAY_CHAR:    // Lft = BOOL, Rht = CHAR
                                return FALSE;

                            defstop
                                break;
                        } // End SWITCH

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;            // Start over
                            ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    return TRUE;

                defstop
                    return FALSE;
            } // End SWITCH

        case ARRAY_INT:             // Lft = INT, Rht = INT/FLOAT/APA/CHAR/HETERO
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_INT:     // Lft = INT, Rht = INT
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        if (*((LPAPLINT) lpMemLft)++ NE *((LPAPLINT) lpMemRht)++)
                            return FALSE;
                    } // End FOR

                    return TRUE;

                case ARRAY_FLOAT:   // Lft = INT, Rht = FLOAT
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        if (!CompareCT ((APLFLOAT) *((LPAPLINT) lpMemLft)++, *((LPAPLFLOAT) lpMemRht)++, fQuadCT, NULL))
                            return FALSE;
                    } // End FOR

                    return TRUE;

                case ARRAY_APA:     // Lft = INT, Rht = APA
#define lpAPA       ((LPAPLAPA) lpMemRht)
                    // Get the APA parameters
                    apaOff = lpAPA->Off;
                    apaMul = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        if (*((LPAPLINT) lpMemLft)++ NE (apaOff + apaMul * uDim))
                            return FALSE;
                    } // End FOR

                    return TRUE;

                case ARRAY_CHAR:    // Lft = INT, Rht = CHAR
                    return FALSE;

                case ARRAY_HETERO:  // Lft = INT, Rht = HETERO
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Split cases based upon the hetero's storage type
                        switch (GetNextHetero (lpMemRht, uDim, &aplIntegerRht, &aplFloatRht, &aplCharRht))
                        {
                            case ARRAY_BOOL:    // Lft = INT, Rht = BOOL
                            case ARRAY_INT:     // Lft = INT, Rht = INT
                                if (*((LPAPLINT) lpMemLft)++ NE aplIntegerRht)
                                    return FALSE;
                                break;

                            case ARRAY_FLOAT:   // Lft = INT, Rht = FLOAT
                                if ((APLFLOAT) *((LPAPLINT) lpMemLft)++ NE aplFloatRht)
                                    return FALSE;
                                break;

                            case ARRAY_CHAR:    // Lft = INT, Rht = CHAR
                                return FALSE;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    return TRUE;

                case ARRAY_BOOL:    // Lft = INT, Rht = BOOL    (Can't happen)
                defstop
                    return FALSE;
            } // End SWITCH

        case ARRAY_FLOAT:           // Lft = FLOAT, Rht = FLOAT/APA/CHAR/HETERO
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_FLOAT:   // Lft = FLOAT, Rht = FLOAT
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        if (!CompareCT (*((LPAPLFLOAT) lpMemLft)++, *((LPAPLFLOAT) lpMemRht)++, fQuadCT, NULL))
                            return FALSE;
                    } // End FOR

                    return TRUE;

                case ARRAY_APA:     // Lft = FLOAT, Rht = APA
#define lpAPA       ((LPAPLAPA) lpMemRht)
                    // Get the APA parameters
                    apaOff = lpAPA->Off;
                    apaMul = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        if (!CompareCT (*((LPAPLFLOAT) lpMemLft)++, (APLFLOAT) (apaOff + apaMul * uDim), fQuadCT, NULL))
                            return FALSE;
                    } // End FOR

                    return TRUE;

                case ARRAY_CHAR:    // Lft = FLOAT, Rht = CHAR
                    return FALSE;

                case ARRAY_HETERO:  // Lft = FLOAT, Rht = HETERO
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Split cases based upon the hetero's storage type
                        switch (GetNextHetero (lpMemRht, uDim, &aplIntegerRht, &aplFloatRht, &aplCharRht))
                        {
                            case ARRAY_BOOL:    // Lft = FLOAT, Rht = BOOL
                            case ARRAY_INT:     // Lft = FLOAT, Rht = INT
                                if (*((LPAPLFLOAT) lpMemLft)++ NE (APLFLOAT) aplIntegerRht)
                                    return FALSE;
                                break;

                            case ARRAY_FLOAT:   // Lft = FLOAT, Rht = FLOAT
                                if (*((LPAPLFLOAT) lpMemLft)++ NE aplFloatRht)
                                    return FALSE;
                                break;

                            case ARRAY_CHAR:    // Lft = FLOAT, Rht = CHAR
                                return FALSE;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    return TRUE;

                case ARRAY_BOOL:    // Lft = FLOAT, Rht = BOOL  (Can't happen)
                case ARRAY_INT:     // Lft = FLOAT, Rht = INT   (Can't happen)
                defstop
                    return FALSE;
            } // End SWITCH

        case ARRAY_APA:             // Lft = APA, Rht = APA/CHAR/HETERO
#define lpAPA       ((LPAPLAPA) lpMemLft)
            // Get the APA parameters
            apaOff = lpAPA->Off;
            apaMul = lpAPA->Mul;
#undef  lpAPA
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_APA:     // Lft = APA, Rht = APA
                    // If the args are empty, they're equal
                    if (aplNELMLft EQ 0)
                        return TRUE;

                    // Compare the APA offsets and multipliers
#define lpAPA       ((LPAPLAPA) lpMemRht)
                    return ((apaOff EQ lpAPA->Off)
                         && (apaMul EQ lpAPA->Mul));
#undef  lpAPA
                case ARRAY_CHAR:    // Lft = APA, Rht = CHAR
                    return FALSE;

                case ARRAY_HETERO:  // Lft = APA, Rht = HETERO
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Split cases based upon the hetero's storage type
                        switch (GetNextHetero (lpMemRht, uDim, &aplIntegerRht, &aplFloatRht, &aplCharRht))
                        {
                            case ARRAY_BOOL:    // Lft = APA, Rht = BOOL
                            case ARRAY_INT:     // Lft = APA, Rht = INT
                                if ((apaOff + apaMul * uDim) NE aplIntegerRht)
                                    return FALSE;
                                break;

                            case ARRAY_FLOAT:   // Lft = APA, Rht = FLOAT
                                if ((apaOff + apaMul * uDim) NE aplFloatRht)
                                    return FALSE;
                                break;

                            case ARRAY_CHAR:    // Lft = APA, Rht = CHAR
                                return FALSE;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    return TRUE;

                case ARRAY_BOOL:    // Lft = APA, Rht = BOOL    (Can't happen)
                case ARRAY_INT:     // Lft = APA, Rht = INT     (Can't happen)
                case ARRAY_FLOAT:   // Lft = APA, Rht = FLOAT   (Can't happen)
                defstop
                    return FALSE;
            } // End SWITCH

        case ARRAY_CHAR:            // Lft = CHAR, Rht = CHAR/HETERO
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_CHAR:    // Lft = CHAR, Rht = CHAR
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        if (*((LPAPLCHAR) lpMemLft)++ NE *((LPAPLCHAR) lpMemRht)++)
                            return FALSE;
                    } // End FOR

                    return TRUE;

                case ARRAY_HETERO:  // Lft = CHAR, Rht = HETERO
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Split cases based upon the hetero's storage type
                        switch (GetNextHetero (lpMemRht, uDim, &aplIntegerRht, &aplFloatRht, &aplCharRht))
                        {
                            case ARRAY_BOOL:    // Lft = CHAR, Rht = BOOL
                            case ARRAY_INT:     // Lft = CHAR, Rht = INT
                            case ARRAY_FLOAT:   // Lft = CHAR, Rht = FLOAT
                                return FALSE;

                            case ARRAY_CHAR:    // Lft = CHAR, Rht = CHAR
                                if (*((LPAPLCHAR) lpMemLft)++ NE aplCharRht)
                                    return FALSE;
                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End FOR

                    return TRUE;

                case ARRAY_BOOL:    // Lft = CHAR, Rht = BOOL   (Can't happen)
                case ARRAY_INT:     // Lft = CHAR, Rht = INT    (Can't happen)
                case ARRAY_FLOAT:   // Lft = CHAR, Rht = FLOAT  (Can't happen)
                case ARRAY_APA:     // Lft = CHAR, Rht = APA    (Can't happen)
                defstop
                    return FALSE;
            } // End SWITCH

        case ARRAY_HETERO:          // Lft = HETERO, Rht = HETERO
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_HETERO:  // Lft = HETERO, Rht = HETERO
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Get the next values and type
                        aplTypeLft = GetNextHetero (lpMemLft, uDim, &aplIntegerLft, &aplFloatLft, &aplCharLft);
                        aplTypeRht = GetNextHetero (lpMemRht, uDim, &aplIntegerRht, &aplFloatRht, &aplCharRht);

                        // Split cases based upon the left hetero's storage type
                        switch (aplTypeLft)
                        {
                            case ARRAY_BOOL:            // Lft = BOOL,  Rht = BOOL/INT/FLOAT/CHAR
                            case ARRAY_INT:             // Lft = INT,   Rht = BOOL/INT/FLOAT/CHAR
                                // Split cases based upon the right hetero's storage type
                                switch (aplTypeRht)
                                {
                                    case ARRAY_BOOL:    // Lft = BOOL,  Rht = BOOL
                                    case ARRAY_INT:     // Lft = BOOL,  Rht = INT
                                        if (aplIntegerLft NE aplIntegerRht)
                                            return FALSE;
                                        break;

                                    case ARRAY_FLOAT:   // Lft = BOOL,  Rht = FLOAT
                                        if ((APLFLOAT) aplIntegerLft NE aplFloatRht)
                                            return FALSE;
                                        break;

                                    case ARRAY_CHAR:    // Lft = BOOL,  Rht = CHAR
                                        return FALSE;

                                    defstop
                                        return FALSE;
                                } // End SWITCH

                                break;

                            case ARRAY_FLOAT:           // Lft = FLOAT, Rht = BOOL/INT/FLOAT/CHAR
                                // Split cases based upon the right hetero's storage type
                                switch (aplTypeRht)
                                {
                                    case ARRAY_BOOL:    // Lft = FLOAT, Rht = BOOL
                                    case ARRAY_INT:     // Lft = FLOAT, Rht = INT
                                        if (aplFloatLft NE (APLFLOAT) aplIntegerRht)
                                            return FALSE;
                                        break;

                                    case ARRAY_FLOAT:   // Lft = FLOAT, Rht = FLOAT
                                        if (aplFloatLft NE aplFloatRht)
                                            return FALSE;
                                        break;

                                    case ARRAY_CHAR:    // Lft = FLOAT, Rht = CHAR
                                        return FALSE;

                                    defstop
                                        return FALSE;
                                } // End SWITCH

                                break;

                            case ARRAY_CHAR:            // Lft = CHAR,  Rht = BOOL/INT/FLOAT/CHAR
                                // Split cases based upon the right hetero's storage type
                                switch (aplTypeRht)
                                {
                                    case ARRAY_BOOL:    // Lft = CHAR,  Rht = BOOL
                                    case ARRAY_INT:     // Lft = CHAR,  Rht = INT
                                    case ARRAY_FLOAT:   // Lft = CHAR,  Rht = FLOAT
                                        return FALSE;

                                    case ARRAY_CHAR:    // Lft = CHAR,  Rht = CHAR
                                        if (aplCharLft NE aplCharRht)
                                            return FALSE;
                                        break;

                                    defstop
                                        return FALSE;
                                } // End SWITCH

                                break;

                            defstop
                                return FALSE;
                        } // End SWITCH
                    } // End FOR

                    return TRUE;

                defstop
                    return FALSE;
            } // End SWITCH

        defstop
            return FALSE;
    } // End SWITCH

ERROR_EXIT:
    return -1;
} // End PrimFnDydEqualUnderbarSimple


//***************************************************************************
//  $PrimFnDydEqualUnderbarNested
//
//  Subroutine to compare two nested arrays
//***************************************************************************

UBOOL PrimFnDydEqualUnderbarNested
    (LPVOID   lpMemLft,         // Ptr to left arg header or data
     APLSTYPE aplTypeLft,       // Left arg storage type
     APLNELM  aplNELMLft,       // ...      NELM
     APLRANK  aplRankLft,       // ...      rank
     LPVOID   lpMemRht,         // Ptr to right arg header or data
     APLSTYPE aplTypeRht,       // Right arg storage type
     APLNELM  aplNELMRht,       // ...       NELM
     APLRANK  aplRankRht,       // ...       rank
     UBOOL    bHeader,          // TRUE iff lpMemLft/Rht point to header
                                //   otherwise they point to data
     LPUBOOL  lpbCtrlBreak)     // Ptr to Ctrl-Break flag

{
    APLUINT       uDim;
    APLNELM       aplNELMLoop;
    LPVOID        lpMemLft2,
                  lpMemRht2;
    APLINT        aplIntegerLft,
                  aplIntegerRht;
    APLFLOAT      aplFloatLft,
                  aplFloatRht;
    APLCHAR       aplCharLft,
                  aplCharRht;
    UINT          ptrType;
    UBOOL         bRet = TRUE;
    APLFLOAT      fQuadCT;          // []CT

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

    // If lpMemLft/Rht point to their respective headers, ...
    if (bHeader)
    {
        // Skip over the headers to the dimensions (data if scalar)
        lpMemLft = VarArrayBaseToDim (lpMemLft);
        lpMemRht = VarArrayBaseToDim (lpMemRht);

        // Ensure the dimensions are the same
        for (uDim = 0; uDim < aplRankLft; uDim++)
        if (*((LPAPLDIM) lpMemLft)++ NE *((LPAPLDIM) lpMemRht)++)
            return FALSE;
    } // End IF

    // lpMemLft and lpMemRht now point to the data

    // Loop through the elements
    for (uDim = 0; bRet && uDim < aplNELMLoop; uDim++, ((LPAPLNESTED) lpMemLft)++, ((LPAPLNESTED) lpMemRht)++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // The ptr types must be the same
        // ***FIXME*** -- If we ever move simple scalars out of the ST, check this assumption
        ptrType = GetPtrTypeInd (lpMemLft);
        if (ptrType NE GetPtrTypeInd (lpMemRht))
            return FALSE;

        // Split cases based upon the ptr type of the elements
        switch (ptrType)
        {
            case PTRTYPE_STCONST:
                // Get the contents of the two LPSYMENTRYs
                // The index is zero because we increment the ptrs in the FOR loop
                aplTypeLft = GetNextHetero (lpMemLft, 0, &aplIntegerLft, &aplFloatLft, &aplCharLft);
                aplTypeRht = GetNextHetero (lpMemRht, 0, &aplIntegerRht, &aplFloatRht, &aplCharRht);

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
                lpMemLft2 = MyGlobalLock (ClrPtrTypeInd (lpMemLft));
                lpMemRht2 = MyGlobalLock (ClrPtrTypeInd (lpMemRht));

                // Split based upon Simple vs. Hetero vs. Nested
                switch (2 * IsNested (aplTypeLft)
                      + 1 * IsNested (aplTypeRht))
                {
                    case 2 * 0 + 1 * 0:     // Lft = Simple, Rht = Simple
                        // Because this function is commutative, we can switch
                        //    the two args without loss of generality.
                        // Switch the args so that the left arg is the "simpler"
                        //    of the two (Simple Homogeneous < Simple Heterogeneous < Nested),
                        //    and within Simple Homogeneous, BOOL < INT < FLOAT < APA < CHAR
                        if (uTypeMap[aplTypeLft] > uTypeMap[aplTypeRht])
                            bRet = PrimFnDydEqualUnderbarSimple (lpMemRht2, aplTypeRht, aplNELMRht, aplRankRht,
                                                                 lpMemLft2, aplTypeLft, aplNELMLft, aplRankLft,
                                                                 TRUE, lpbCtrlBreak);
                        else
                            bRet = PrimFnDydEqualUnderbarSimple (lpMemLft2, aplTypeLft, aplNELMLft, aplRankLft,
                                                                 lpMemRht2, aplTypeRht, aplNELMRht, aplRankRht,
                                                                 TRUE, lpbCtrlBreak);
                        break;

                    case 2 * 0 + 1 * 1:     // Lft = Simple, Rht = Nested
                    case 2 * 1 + 1 * 0:     // Lft = Nested, Rht = Simple
                        bRet = FALSE;

                        break;

                    case 2 * 1 + 1 * 1:     // Lft = Nested, Rht = Nested
                        bRet = PrimFnDydEqualUnderbarNested (lpMemLft2, aplTypeLft, aplNELMLft, aplRankLft,
                                                             lpMemRht2, aplTypeRht, aplNELMRht, aplRankRht,
                                                             TRUE, lpbCtrlBreak);
                        break;

                    defstop
                        break;
                } // End SWITCH

                // We no longer need these ptrs
                MyGlobalUnlock (ClrPtrTypeInd (lpMemLft)); lpMemLft2 = NULL;
                MyGlobalUnlock (ClrPtrTypeInd (lpMemRht)); lpMemRht2 = NULL;

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
//  End of File: pf_equalund.c
//***************************************************************************
