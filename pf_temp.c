//***************************************************************************
//  NARS2000 -- Primitive Function -- EqualUnderbar
//***************************************************************************

#pragma pack (1)
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

// Same order as in ARRAY_TYPES
//                 BOOL, INT, FLOAT, CHAR, HETERO, NESTED, LIST, APA
UINT uTypeMap[] = {   0,   1,     2,    4,      5,      6,    7,   3};
// so that         BOOL < INT < FLOAT < APA < CHAR < HETERO < NESTED


//***************************************************************************
//  PrimFnEqualUnderbar_EM
//
//  Primitive function for monadic and dyadic EqualUnderbar ("depth" and "equivalent")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnEqualUnderbar_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnEqualUnderbar_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_EQUALUNDERBAR);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonEqualUnderbar_EM (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydEqualUnderbar_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnEqualUnderbar_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonEqualUnderbar_EM
//
//  Primitive function for monadic EqualUnderbar (depth)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonEqualUnderbar_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonEqualUnderbar_EM
    (LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    static  YYSTYPE YYRes;      // The result
    APLSTYPE        aplTypeRht;
    APLNELM         aplNELMRht;
    APLRANK         aplRankRht;
    HGLOBAL         hGlbRht;

    // Determine how deep the argument is.
    // Simple scalars:  0
    // Simple arrays:   1
    // Nested arrays: > 1

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

    // Fill in the result token
    YYRes.tkToken.tkFlags.TknType   = TKT_VARIMMED;
    YYRes.tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////YYRes.tkToken.tkFlags.NoDisplay = 0;
////YYRes.tkToken.tkFlags.Color     =
    YYRes.tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // If it's not nested,
    //   it's of depth 0 (scalar) or 1 (vector or higher)
    if (aplTypeRht NE ARRAY_NESTED)
        YYRes.tkToken.tkData.tkInteger = (aplRankRht NE 0);
    else
    {
        // Split cases based upon the right arg's token type
        switch (lptkRhtArg->tkFlags.TknType)
        {
            case TKT_VARNAMED:
                // tkData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lptkRhtArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

                // It can't be immediate as that's handled above
                Assert (!lptkRhtArg->tkData.lpSym->stFlags.Imm);

                // Get the global memory handle
                hGlbRht = lptkRhtArg->tkData.lpSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbRht));

                break;          // Continue with global case

            case TKT_VARARRAY:
                // Get the global memory handle
                hGlbRht = lptkRhtArg->tkData.tkGlbData;

                // tkData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbRht));

                break;          // Continue with global case

            case TKT_LIST:
                ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                           lptkFunc);
                return NULL;

            case TKT_VARIMMED:
            defstop
                break;
        } // End SWITCH

        // Recursively run through the elements of the nested array
        YYRes.tkToken.tkData.tkInteger = PrimFnMonEqualUnderBarGlb (hGlbRht);
    } // End IF/ELSE

    return &YYRes;
} // End PrimFnMonEqualUnderbar_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonEqualUnderBarGlb
//
//  Common subroutine to determine the depth of a global memory handle.
//***************************************************************************

APLINT PrimFnMonEqualUnderBarGlb
    (HGLOBAL hGlbRht)

{
    LPAPLNESTED lpMemRht;
    APLSTYPE    aplTypeRht;
    APLNELM     aplNELMRht;
    APLRANK     aplRankRht;
    APLUINT     uRes,
                uRht,
                uTmp;

    // Split cases based upon the ptr bits
    switch (GetPtrTypeDir (hGlbRht))
    {
        case PTRTYPE_STCONST:
            return 0;

        case PTRTYPE_HGLOBAL:
            // It's a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbRht));

            // Clear the ptr bits
            hGlbRht = ClrPtrTypeDirGlb (hGlbRht);

            // Lock the memory to get a ptr to it
            lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)

            // Save the type, NELM, and rank
            aplTypeRht = lpHeader->ArrType;
            aplNELMRht = lpHeader->NELM;
            aplRankRht = lpHeader->Rank;

#undef  lpHeader

            // Handle nested prototype
            if (aplNELMRht EQ 0
             && aplTypeRht EQ ARRAY_NESTED)
                aplNELMRht++;

            // Start with 0 or 1 depending upon the rank
            uRes = (aplRankRht NE 0);

            // If it's nested, recurse
            if (aplTypeRht EQ ARRAY_NESTED)
            {
                // Skip over the header and dimensions
                lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

                // Loop through the elements of the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Split cases based on the ptr type of the element
                    switch (GetPtrTypeDir (lpMemRht[uRht]))
                    {
                        case PTRTYPE_STCONST:
                            break;

                        case PTRTYPE_HGLOBAL:
                            uTmp = 1 + PrimFnMonEqualUnderBarGlb (lpMemRht[uRht]);
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
} // End PrimFnMonEqualUnderBarGlb


//***************************************************************************
//  PrimFnDydEqualUnderbar_EM
//
//  Primitive function for dyadic EqualUnderbar (equivalent)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydEqualUnderbar_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDydEqualUnderbar_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    static   YYSTYPE YYRes; // The result
    APLSTYPE aplTypeLft,
             aplTypeRht;
    APLNELM  aplNELMLft,
             aplNELMRht;
    APLRANK  aplRankLft,
             aplRankRht;
    HGLOBAL  hGlbLft,
             hGlbRht;
    LPVOID   lpMemLft,
             lpMemRht;
    LPTOKEN  lptkTmpArg;
    BOOL     bNumLft,
             bNumRht;
    APLINT   aplIntegerLft,
             aplIntegerRht;
    APLFLOAT aplFloatLft,
             aplFloatRht;
    APLCHAR  aplCharLft,
             aplCharRht;

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

    // Determine if two arrays are identical in
    //   rank, length, and value at all levels
    //   without regarrd to the array representation

    // Fill in the result token
    YYRes.tkToken.tkFlags.TknType   = TKT_VARIMMED;
    YYRes.tkToken.tkFlags.ImmType   = IMMTYPE_BOOL;
////YYRes.tkToken.tkFlags.NoDisplay = 0;
////YYRes.tkToken.tkFlags.Color     =
    YYRes.tkToken.tkData.tkBoolean  = FALSE;
    YYRes.tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Check for lists here -- the parser catches lists on the left
    if (aplTypeRht EQ ARRAY_LIST)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkRhtArg);
        return NULL;
    } // End IF

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
    } // End IF

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Split based upon Simple vs. Hetero vs. Nested
    switch (2 * (aplTypeLft EQ ARRAY_NESTED)
              + (aplTypeRht EQ ARRAY_NESTED))
    {
        case 0:         // Both Simple or Hetero



            // If both arguments are scalars, get the
            //   first values and compare them as they
            //   might be TKT_VARIMMED and thus not pointed
            //   to by the lpMemLft/Rht vars
            if (aplRankLft EQ 0
             && aplRankRht EQ 0)
            {
                // Ensure Numeric vs. Numeric or Char vs. Char
                bNumLft = IsSimpleNum (aplTypeLft);
                bNumRht = IsSimpleNum (aplTypeRht);
                if (bNumLft NE bNumRht)
                    break;

                // Get the respective first values
                FirstValue (lptkLftArg, &aplIntegerLft, &aplFloatLft, &aplCharLft, NULL, NULL, NULL);
                FirstValue (lptkRhtArg, &aplIntegerRht, &aplFloatRht, &aplCharRht, NULL, NULL, NULL);

                // Split cases into Numeric and Char
                if (bNumLft)
                {               // Both are numeric
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))
                        YYRes.tkToken.tkData.tkBoolean = (aplIntegerLft EQ aplIntegerRht);
                    else
                        YYRes.tkToken.tkData.tkBoolean = CompareCT (aplFloatLft, aplFloatRht, fQuadCT, NULL);
                } else          // Both are char
                    // Compare the values
                    YYRes.tkToken.tkData.tkBoolean = (aplCharLft EQ aplCharRht);

                break;
            } // End IF

            YYRes.tkToken.tkData.tkBoolean =
              PrimFnDydEqualUnderbarSimple (hGlbLft, lpMemLft, aplTypeLft, aplNELMLft, aplRankLft,
                                            hGlbRht, lpMemRht, aplTypeRht, aplNELMRht, aplRankLft);
            break;

        case 3:         // Both Nested
            YYRes.tkToken.tkData.tkBoolean =
              PrimFnDydEqualUnderbarNested (hGlbLft, lpMemLft, aplTypeLft, aplNELMLft, aplRankLft,
                                            hGlbRht, lpMemRht, aplTypeRht, aplNELMRht, aplRankLft);
            break;

        case 1:         // Lft = Simple, Rht = Nested
        case 2:         // Lft = Nested, Rht = Simple
            break;

        defstop
            break;
    } // End SWITCH
ERROR_EXIT:
    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    return &YYRes;
} // End PrimFnDydEqualUnderbar_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydEqualUnderbarSimple
//
//  Subroutine to compare two simple arrays
//***************************************************************************

BOOL PrimFnDydEqualUnderbarSimple
    (HGLOBAL  hGlbLft,
     LPVOID   lpMemLft,
     APLSTYPE aplTypeLft,
     APLNELM  aplNELMLft,
     APLRANK  aplRankLft,
     HGLOBAL  hGlbRht,
     LPVOID   lpMemRht,
     APLSTYPE aplTypeRht,
     APLNELM  aplNELMRht,
     APLRANK  aplRankRht)

{
    APLINT   uDim,
             apaOff,
             apaMul;
    BOOL     bNumLft,
             bNumRht;
    UINT     uBitMask = 0x01;

    // Ensure same rank and # elements
    if (aplRankLft NE aplRankRht
     || aplNELMLft NE aplNELMRht)
        return FALSE;

    DbgBrk ();          // ***TESTME***

    // Ensure the dimensions are the same
    if (aplRankLft NE 0)
    {
        // Skip over the headers to the dimensions
        lpMemLft = VarArrayBaseToDim (lpMemLft);
        lpMemRht = VarArrayBaseToDim (lpMemRht);

        for (uDim = 0; uDim < (APLINT) aplRankLft; uDim++)
        if (*((LPAPLDIM) lpMemLft)++ NE *((LPAPLDIM) lpMemRht)++)
            return FALSE;
    } // End IF

    // lpMemLft and lpMemRht now point to the data

    // Ensure Numeric vs. Numeric or Char vs. Char
    bNumLft = IsSimpleNum (aplTypeLft);
    bNumRht = IsSimpleNum (aplTypeRht);
    if (bNumLft NE bNumRht)
        return FALSE;

    // Split cases into Numeric and Char
    if (bNumLft)
    {               // Both are numeric
        // Compare the values

        // Split cases based upon the left arg's storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:            // Lft = BOOL
                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Lft = BOOL, Rht = BOOL
                        // Loop through the elements
                        for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                        {
                            if (((uBitMask & *((LPAPLBOOL) lpMemLft)) ? 1 : 0)
                             NE ((uBitMask & *((LPAPLBOOL) lpMemRht)) ? 1 : 0))
                                return FALSE;

                            // Shift over the bit mask
                            uBitMask <<= 1;

                            // Check for end-of-byte
                            if (uBitMask EQ END_OF_BYTE)
                            {
                                uBitMask = 0x01;            // Start over
                                ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                            } // End IF
                        } // End FOR

                        return TRUE;

                    case ARRAY_INT:     // Lft = BOOL, Rht = INT
                        // Loop through the elements
                        for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                        {
                            if (((uBitMask & *((LPAPLBOOL) lpMemLft)) ? 1 : 0)
                             NE *((LPAPLINT) lpMemRht)++)
                                return FALSE;

                            // Shift over the bit mask
                            uBitMask <<= 1;

                            // Check for end-of-byte
                            if (uBitMask EQ END_OF_BYTE)
                            {
                                uBitMask = 0x01;            // Start over
                                ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                            } // End IF
                        } // End FOR

                        return TRUE;

                    case ARRAY_FLOAT:   // Lft = BOOL, Rht = FLOAT
                        // Loop through the elements
                        for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                        {
                            if (((uBitMask & *((LPAPLBOOL) lpMemLft)) ? 1 : 0)
                             NE *((LPAPLFLOAT) lpMemRht)++)
                                return FALSE;

                            // Shift over the bit mask
                            uBitMask <<= 1;

                            // Check for end-of-byte
                            if (uBitMask EQ END_OF_BYTE)
                            {
                                uBitMask = 0x01;            // Start over
                                ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                            } // End IF
                        } // End FOR

                        return TRUE;

                    case ARRAY_APA:     // Lft = BOOL, Rht = APA
#define lpHeader    ((LPAPLAPA) lpMemLft)
                        apaOff = lpHeader->Off;
                        apaMul = lpHeader->Mul;
#undef  lpHeader
                        // Loop through the elements
                        for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                        {
                            if (((uBitMask & *((LPAPLBOOL) lpMemLft)) ? 1 : 0)
                             NE (apaOff + apaMul * uDim))
                                return FALSE;

                            // Shift over the bit mask
                            uBitMask <<= 1;

                            // Check for end-of-byte
                            if (uBitMask EQ END_OF_BYTE)
                            {
                                uBitMask = 0x01;            // Start over
                                ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                            } // End IF
                        } // End FOR

                        return TRUE;

                    defstop
                        return FALSE;
                } // End SWITCH

            case ARRAY_INT:             // Lft = INT
                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Lft = INT, Rht = BOOL
                        // Loop through the elements
                        for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                        {
                            if (*((LPAPLINT) lpMemLft)++
                             NE ((uBitMask & *((LPAPLBOOL) lpMemRht)) ? 1 : 0))
                                return FALSE;

                            // Shift over the bit mask
                            uBitMask <<= 1;

                            // Check for end-of-byte
                            if (uBitMask EQ END_OF_BYTE)
                            {
                                uBitMask = 0x01;            // Start over
                                ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                            } // End IF
                        } // End FOR

                        return TRUE;

                    case ARRAY_INT:     // Lft = INT, Rht = INT
                        // Loop through the elements
                        for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                        if (*((LPAPLINT) lpMemLft)++ NE *((LPAPLINT) lpMemRht)++)
                            return FALSE;
                        return TRUE;

                    case ARRAY_FLOAT:   // Lft = INT, Rht = FLOAT
                        // Loop through the elements
                        for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                        if (CompareCT ((APLFLOAT) *((LPAPLINT) lpMemLft)++, *((LPAPLFLOAT) lpMemRht)++, fQuadCT, NULL))
                            return FALSE;
                        return TRUE;

                    case ARRAY_APA:     // Lft = INT, Rht = APA
#define lpHeader    ((LPAPLAPA) lpMemRht)
                        apaOff = lpHeader->Off;
                        apaMul = lpHeader->Mul;
#undef  lpHeader
                        // Loop through the elements
                        for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                        if (*((LPAPLINT) lpMemLft)++ NE (apaOff + apaMul * uDim))
                            return FALSE;
                        return TRUE;

                    defstop
                        return FALSE;
                } // End SWITCH

            case ARRAY_FLOAT:           // Lft = FLOAT
                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Lft = FLOAT, Rht = BOOL
                        // Loop through the elements
                        for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                        {
                            if (*((LPAPLFLOAT) lpMemLft)++
                             NE ((uBitMask & *((LPAPLBOOL) lpMemRht)) ? 1 : 0))
                                return FALSE;

                            // Shift over the bit mask
                            uBitMask <<= 1;

                            // Check for end-of-byte
                            if (uBitMask EQ END_OF_BYTE)
                            {
                                uBitMask = 0x01;            // Start over
                                ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                            } // End IF
                        } // End FOR

                        return TRUE;

                    case ARRAY_INT:     // Lft = FLOAT, Rht = INT
                        // Loop through the elements
                        for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                        if (CompareCT (*((LPAPLFLOAT) lpMemLft)++, (APLFLOAT) *((LPAPLINT) lpMemRht)++, fQuadCT, NULL))
                            return FALSE;
                        return TRUE;

                    case ARRAY_FLOAT:   // Lft = FLOAT, Rht = FLOAT
                        // Loop through the elements
                        for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                        if (CompareCT (*((LPAPLFLOAT) lpMemLft)++, *((LPAPLFLOAT) lpMemRht)++, fQuadCT, NULL))
                            return FALSE;
                        return TRUE;

                    case ARRAY_APA:     // Lft = FLOAT, Rht = APA
#define lpHeader    ((LPAPLAPA) lpMemRht)
                        apaOff = lpHeader->Off;
                        apaMul = lpHeader->Mul;
#undef  lpHeader
                        // Loop through the elements
                        for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                        if (CompareCT (*((LPAPLFLOAT) lpMemLft)++, (APLFLOAT) (apaOff + apaMul * uDim), fQuadCT, NULL))
                            return FALSE;
                        return TRUE;

                    defstop
                        return FALSE;
                } // End SWITCH

            case ARRAY_APA:             // Lft = APA
#define lpHeader    ((LPAPLAPA) lpMemLft)
                        apaOff = lpHeader->Off;
                        apaMul = lpHeader->Mul;
#undef  lpHeader
                // Split cases based upon the right arg's storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:    // Lft = APA, Rht = BOOL
                        // Loop through the elements
                        for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                        {
                            if ((apaOff + apaMul * uDim)
                             NE ((uBitMask & *((LPAPLBOOL) lpMemRht)) ? 1 : 0))
                                return FALSE;

                            // Shift over the bit mask
                            uBitMask <<= 1;

                            // Check for end-of-byte
                            if (uBitMask EQ END_OF_BYTE)
                            {
                                uBitMask = 0x01;            // Start over
                                ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                            } // End IF
                        } // End FOR

                        return TRUE;

                    case ARRAY_INT:     // Lft = APA, Rht = INT
                        // Loop through the elements
                        for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                        if ((apaOff + apaMul * uDim) NE *((LPAPLINT) lpMemRht)++)
                            return FALSE;
                        return TRUE;

                    case ARRAY_FLOAT:   // Lft = APA, Rht = FLOAT
                        // Loop through the elements
                        for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                        if (CompareCT ((APLFLOAT) (apaOff + apaMul * uDim), *((LPAPLFLOAT) lpMemRht)++, fQuadCT, NULL))
                            return FALSE;
                        return TRUE;

                    case ARRAY_APA:     // Lft = INT, Rht = APA
                        // Compare the APA offsets and multipliers
                        if ((((LPAPLAPA) lpMemLft)->Off
                          NE ((LPAPLAPA) lpMemRht)->Off)
                         || (((LPAPLAPA) lpMemLft)->Mul
                          NE ((LPAPLAPA) lpMemLft)->Mul))
                            return FALSE;
                        return TRUE;

                    defstop
                        return FALSE;
                } // End SWITCH

            defstop
                return FALSE;
        } // End SWITCH
    } else
    {               // Both are char
        // Compare the values
        for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
        if (*((LPAPLCHAR) lpMemLft)++ NE *((LPAPLCHAR) lpMemRht)++)
            return FALSE;
        return TRUE;
    } // End IF/ELSE
} // End PrimFnDydEqualUnderbarSimple


//***************************************************************************
//  PrimFnDydEqualUnderbarNested
//
//  Subroutine to compare two nested arrays
//***************************************************************************

BOOL PrimFnDydEqualUnderbarNested
    (HGLOBAL  hGlbLft,
     LPVOID   lpMemLft,
     APLSTYPE aplTypeLft,
     APLNELM  aplNELMLft,
     APLRANK  aplRankLft,
     HGLOBAL  hGlbRht,
     LPVOID   lpMemRht,
     APLSTYPE aplTypeRht,
     APLNELM  aplNELMRht,
     APLRANK  aplRankRht)

{
    APLUINT  uDim;

    // Ensure same rank and # elements
    if (aplRankLft NE aplRankRht
     || aplNELMLft NE aplNELMRht)
        return FALSE;

    DbgBrk ();          // ***FINISHME***

    // Ensure the dimensions are the same
    if (aplRankLft NE 0)
    {
        // Skip over the headers to the dimensions
        lpMemLft = VarArrayBaseToDim (lpMemLft);
        lpMemRht = VarArrayBaseToDim (lpMemRht);

        for (uDim = 0; uDim < aplRankLft; uDim++)
        if (*((LPAPLDIM) lpMemLft)++ NE *((LPAPLDIM) lpMemRht)++)
            return FALSE;
    } // End IF

    // lpMemLft and lpMemRht now point to the data

    // Loop through the elements
    for (uDim = 0; uDim < aplNELMLft; uDim++)
    {
    // Split cases based upon the





    } // End FOR

    return TRUE;
} // End PrimFnDydEqualUnderbarNested


//***************************************************************************
//  End of File: pf_equalund.c
//***************************************************************************
