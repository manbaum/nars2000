//***************************************************************************
//  NARS2000 -- Primitive Function -- EqualUnderbar
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


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

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // If it's not nested,
    //   it's of depth 0 (scalar) or 1 (vector or higher)
    if (aplTypeRht NE ARRAY_NESTED)
        lpYYRes->tkToken.tkData.tkInteger = (aplRankRht NE 0);
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
                Assert (IsGlbTypeVarDir (hGlbRht));

                break;          // Continue with global case

            case TKT_VARARRAY:
                // Get the global memory handle
                hGlbRht = lptkRhtArg->tkData.tkGlbData;

                // tkData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbRht));

                break;          // Continue with global case

            case TKT_VARIMMED:
            defstop
                break;
        } // End SWITCH

        // Recursively run through the elements of the nested array
        lpYYRes->tkToken.tkData.tkInteger = PrimFnMonEqualUnderBarGlb (hGlbRht);
    } // End IF/ELSE

    return lpYYRes;
} // End PrimFnMonEqualUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonEqualUnderBarGlb
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

            // Clear the ptr type bits
            hGlbRht = ClrPtrTypeDirGlb (hGlbRht);

            // Lock the memory to get a ptr to it
            lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)

            // Get the Array Type, NELM, and Rank
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
    APLSTYPE     aplTypeLft,    // Left arg storage type
                 aplTypeRht,    // Right ...
                 aplTypeTmp;    // Temp ...
    APLNELM      aplNELMLft,    // Left arg NELM
                 aplNELMRht;    // Right ...
    APLRANK      aplRankLft,    // Left arg rank
                 aplRankRht;    // Right ...
    HGLOBAL      hGlbLft,       // Left arg global memory handle
                 hGlbRht;       // Right ...
    LPVOID       lpMemLft,      // Ptr to left arg global memory
                 lpMemRht;      // Ptr to right ...
    LPTOKEN      lptkTmpArg;    // Ptr to temp arg token
    BOOL         bNumLft,       // TRUE iff left arg is simple numeric
                 bNumRht;       // ...      right ...
    APLINT       aplIntegerLft, // Left arg as integer
                 aplIntegerRht; // Right ...
    APLFLOAT     aplFloatLft,   // Left arg as float
                 aplFloatRht;   // Right ...
    APLCHAR      aplCharLft,    // Left arg as char
                 aplCharRht;    // Right ...
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result
    APLFLOAT     fQuadCT;       // []CT

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

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

    // N.B.  We are relying upon type demotion here

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_BOOL;
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkBoolean  = FALSE;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

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
    switch (2 * (aplTypeLft EQ ARRAY_NESTED)
          + 1 * (aplTypeRht EQ ARRAY_NESTED))
    {
        case 2 * 0 + 1 * 0:     // Lft = Simple, Rht = Simple
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
                FirstValue (lptkLftArg,         // Ptr to left arg token
                           &aplIntegerLft,      // Ptr to integer result
                           &aplFloatLft,        // Ptr to float ...
                           &aplCharLft,         // Ptr to WCHAR ...
                            NULL,               // Ptr to longest ...
                            NULL,               // Ptr to lpSym/Glb ...
                            NULL,               // Ptr to ...immediate type ...
                            NULL);              // Ptr to array type ...
                FirstValue (lptkRhtArg,         // Ptr to right arg token
                           &aplIntegerRht,      // Ptr to integer result
                           &aplFloatRht,        // Ptr to float ...
                           &aplCharRht,         // Ptr to WCHAR ...
                            NULL,               // Ptr to longest ...
                            NULL,               // Ptr to lpSym/Glb ...
                            NULL,               // Ptr to ...immediate type ...
                            NULL);              // Ptr to array type ...
                // Split cases into Numeric and Char
                if (bNumLft)
                {               // Both are numeric
                    if (IsSimpleInt (aplTypeLft)
                     && IsSimpleInt (aplTypeRht))
                        lpYYRes->tkToken.tkData.tkBoolean = (aplIntegerLft EQ aplIntegerRht);
                    else
                        lpYYRes->tkToken.tkData.tkBoolean = CompareCT (aplFloatLft, aplFloatRht, fQuadCT, NULL);
                } else          // Both are char
                    // Compare the values
                    lpYYRes->tkToken.tkData.tkBoolean = (aplCharLft EQ aplCharRht);
                break;
            } // End IF

            lpYYRes->tkToken.tkData.tkBoolean =
              PrimFnDydEqualUnderbarSimple (lpMemLft, aplTypeLft, aplNELMLft, aplRankLft,
                                            lpMemRht, aplTypeRht, aplNELMRht, aplRankRht);
            break;

        case 2 * 0 + 1 * 1:     // Lft = Simple, Rht = Nested
        case 2 * 1 + 1 * 0:     // Lft = Nested, Rht = Simple
            break;

        case 2 * 1 + 1 * 1:     // Lft = Nested, Rht = Nested
            lpYYRes->tkToken.tkData.tkBoolean =
              PrimFnDydEqualUnderbarNested (lpMemLft, aplTypeLft, aplNELMLft, aplRankLft,
                                            lpMemRht, aplTypeRht, aplNELMRht, aplRankRht);
            break;

        defstop
            break;
    } // End SWITCH

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

BOOL PrimFnDydEqualUnderbarSimple
    (LPVOID   lpMemLft,
     APLSTYPE aplTypeLft,
     APLNELM  aplNELMLft,
     APLRANK  aplRankLft,
     LPVOID   lpMemRht,
     APLSTYPE aplTypeRht,
     APLNELM  aplNELMRht,
     APLRANK  aplRankRht)

{
    APLINT   uDim,
             apaOff,
             apaMul;
    UINT     uBitMask = 0x01;
    APLINT   aplIntegerLft,
             aplIntegerRht;
    APLFLOAT aplFloatLft,
             aplFloatRht;
    APLCHAR  aplCharLft,
             aplCharRht;
    APLFLOAT fQuadCT;           // []CT

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Ensure same rank and # elements
    if (aplRankLft NE aplRankRht
     || aplNELMLft NE aplNELMRht)
        return FALSE;

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
                        if ((uBitMask & *((LPAPLBOOL) lpMemLft))
                         NE (uBitMask & *((LPAPLBOOL) lpMemRht)))
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
#define lpAPA       ((LPAPLAPA) lpMemLft)
                    // Get the APA parameters
                    apaOff = lpAPA->Off;
                    apaMul = lpAPA->Mul;
#undef  lpAPA
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

                case ARRAY_CHAR:    // Lft = BOOL, Rht = CHAR
                    return FALSE;

                case ARRAY_HETERO:  // Lft = BOOL, Rht = HETERO
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
                        // Split cases based upon the hetero's storage type
                        switch (GetNextHetero (lpMemRht, uDim, &aplIntegerRht, &aplFloatRht, &aplCharRht))
                        {
                            case ARRAY_BOOL:    // Lft = BOOL, Rht = BOOL
                            case ARRAY_INT:     // Lft = BOOL, Rht = INT
                                if (((uBitMask & *((LPAPLBOOL) lpMemLft)) ? 1 : 0) NE aplIntegerRht)
                                    return FALSE;
                                break;

                            case ARRAY_FLOAT:   // Lft = BOOL, Rht = FLOAT
                                if (((uBitMask & *((LPAPLBOOL) lpMemLft)) ? 1 : 0) NE aplFloatRht)
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
                            uBitMask = 0x01;            // Start over
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
                    if (*((LPAPLINT) lpMemLft)++ NE *((LPAPLINT) lpMemRht)++)
                        return FALSE;
                    return TRUE;

                case ARRAY_FLOAT:   // Lft = INT, Rht = FLOAT
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    if (!CompareCT ((APLFLOAT) *((LPAPLINT) lpMemLft)++, *((LPAPLFLOAT) lpMemRht)++, fQuadCT, NULL))
                        return FALSE;
                    return TRUE;

                case ARRAY_APA:     // Lft = INT, Rht = APA
#define lpAPA       ((LPAPLAPA) lpMemRht)
                    // Get the APA parameters
                    apaOff = lpAPA->Off;
                    apaMul = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    if (*((LPAPLINT) lpMemLft)++ NE (apaOff + apaMul * uDim))
                        return FALSE;
                    return TRUE;

                case ARRAY_CHAR:    // Lft = INT, Rht = CHAR
                    return FALSE;

                case ARRAY_HETERO:  // Lft = INT, Rht = HETERO
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
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
                    if (!CompareCT (*((LPAPLFLOAT) lpMemLft)++, *((LPAPLFLOAT) lpMemRht)++, fQuadCT, NULL))
                        return FALSE;
                    return TRUE;

                case ARRAY_APA:     // Lft = FLOAT, Rht = APA
#define lpAPA       ((LPAPLAPA) lpMemRht)
                    // Get the APA parameters
                    apaOff = lpAPA->Off;
                    apaMul = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    if (!CompareCT (*((LPAPLFLOAT) lpMemLft)++, (APLFLOAT) (apaOff + apaMul * uDim), fQuadCT, NULL))
                        return FALSE;
                    return TRUE;

                case ARRAY_CHAR:    // Lft = FLOAT, Rht = CHAR
                    return FALSE;

                case ARRAY_HETERO:  // Lft = FLOAT, Rht = HETERO
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
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
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_APA:     // Lft = APA, Rht = APA
                    // Compare the APA offsets and multipliers
#define lpAPA       ((LPAPLAPA) lpMemLft)
                    return ((lpAPA->Off EQ lpAPA->Off)
                         && (lpAPA->Mul EQ lpAPA->Mul));
#undef  lpAPA
                case ARRAY_CHAR:    // Lft = APA, Rht = CHAR
                    return FALSE;

                case ARRAY_HETERO:  // Lft = APA, Rht = HETERO
#define lpAPA       ((LPAPLAPA) lpMemLft)
                    // Get the APA parameters
                    apaOff = lpAPA->Off;
                    apaMul = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
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
                    if (*((LPAPLCHAR) lpMemLft)++ NE *((LPAPLCHAR) lpMemRht)++)
                        return FALSE;
                    return TRUE;

                case ARRAY_HETERO:  // Lft = CHAR, Rht = HETERO
                    // Loop through the elements
                    for (uDim = 0; uDim < (APLINT) aplNELMLft; uDim++)
                    {
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
} // End PrimFnDydEqualUnderbarSimple


//***************************************************************************
//  $PrimFnDydEqualUnderbarNested
//
//  Subroutine to compare two nested arrays
//***************************************************************************

BOOL PrimFnDydEqualUnderbarNested
    (LPVOID   lpMemLft,
     APLSTYPE aplTypeLft,
     APLNELM  aplNELMLft,
     APLRANK  aplRankLft,
     LPVOID   lpMemRht,
     APLSTYPE aplTypeRht,
     APLNELM  aplNELMRht,
     APLRANK  aplRankRht)

{
    APLUINT  uDim;
    APLNELM  aplNELMLft2,
             aplNELMRht2;
    LPVOID   lpMemLft2,
             lpMemRht2;
    APLINT   aplIntegerLft,
             aplIntegerRht;
    APLFLOAT aplFloatLft,
             aplFloatRht;
    APLCHAR  aplCharLft,
             aplCharRht;
    UINT     ptrType;
    BOOL     bRet = TRUE;
    APLFLOAT fQuadCT;           // []CT

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Ensure same rank and # elements
    if (aplRankLft NE aplRankRht
     || aplNELMLft NE aplNELMRht)
        return FALSE;

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
    for (uDim = 0; bRet && uDim < aplNELMLft; uDim++, ((LPAPLNESTED) lpMemLft)++, ((LPAPLNESTED) lpMemRht)++)
    {
        // The ptr types must be the same
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
                if (aplTypeLft EQ ARRAY_CHAR
                 && aplTypeRht EQ ARRAY_CHAR)
                {
                    if (aplCharLft NE aplCharRht)
                        return FALSE;
                    else
                        break;
                } // End IF

                // Strip out char vs. num and num vs. char
                if (aplTypeLft EQ ARRAY_CHAR
                 || aplTypeRht EQ ARRAY_CHAR)
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
                AttrsOfGlb (ClrPtrTypeIndGlb (lpMemLft), &aplTypeLft, &aplNELMLft2, &aplRankLft, NULL);
                AttrsOfGlb (ClrPtrTypeIndGlb (lpMemRht), &aplTypeRht, &aplNELMRht2, &aplRankRht, NULL);

                // Ensure same rank and # elements
                if (aplRankLft NE aplRankRht
                 || aplNELMLft NE aplNELMRht)
                    return FALSE;

                // Lock the memory to get a ptr to it
                lpMemLft2 = MyGlobalLock (ClrPtrTypeIndGlb (lpMemLft));
                lpMemRht2 = MyGlobalLock (ClrPtrTypeIndGlb (lpMemRht));

                // Split based upon Simple vs. Hetero vs. Nested
                switch (2 * (aplTypeLft EQ ARRAY_NESTED)
                      + 1 * (aplTypeRht EQ ARRAY_NESTED))
                {
                    case 2 * 0 + 1 * 0:     // Lft = Simple, Rht = Simple
                        // Because this function is commutative, we can switch
                        //    the two args without loss of generality.
                        // Switch the args so that the left arg is the "simpler"
                        //    of the two (Simple Homogeneous < Simple Heterogeneous < Nested),
                        //    and within Simple Homogeneous, BOOL < INT < FLOAT < APA < CHAR
                        if (uTypeMap[aplTypeLft] > uTypeMap[aplTypeRht])
                            bRet = PrimFnDydEqualUnderbarSimple (lpMemRht2, aplTypeRht, aplNELMRht2, aplRankRht,
                                                                 lpMemLft2, aplTypeLft, aplNELMLft2, aplRankLft);
                        else
                            bRet = PrimFnDydEqualUnderbarSimple (lpMemLft2, aplTypeLft, aplNELMLft2, aplRankLft,
                                                                 lpMemRht2, aplTypeRht, aplNELMRht2, aplRankRht);
                        break;

                    case 2 * 0 + 1 * 1:     // Lft = Simple, Rht = Nested
                    case 2 * 1 + 1 * 0:     // Lft = Nested, Rht = Simple
                        bRet = FALSE;

                        break;

                    case 2 * 1 + 1 * 1:     // Lft = Nested, Rht = Nested
                        bRet = PrimFnDydEqualUnderbarNested (lpMemLft2, aplTypeLft, aplNELMLft2, aplRankLft,
                                                             lpMemRht2, aplTypeRht, aplNELMRht2, aplRankRht);
                        break;

                    defstop
                        break;
                } // End SWITCH

                // We no longer need these ptrs
                MyGlobalUnlock (ClrPtrTypeIndGlb (lpMemLft)); lpMemLft2 = NULL;
                MyGlobalUnlock (ClrPtrTypeIndGlb (lpMemRht)); lpMemRht2 = NULL;

                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR

    return bRet;
} // End PrimFnDydEqualUnderbarNested


//***************************************************************************
//  End of File: pf_equalund.c
//***************************************************************************
