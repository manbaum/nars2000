//***************************************************************************
//  NARS2000 -- Primitive Function -- Epsilon
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


//***************************************************************************
//  $PrimFnEpsilon_EM_YY
//
//  Primitive function for monadic and dyadic Epsilon ("enlist" and "member of")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnEpsilon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnEpsilon_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_EPSILON);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonEpsilon_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydEpsilon_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnEpsilon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnEpsilon_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Epsilon
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnEpsilon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnEpsilon_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnEpsilon_EM_YY,// Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnEpsilon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonEpsilon_EM_YY
//
//  Primitive function for monadic Epsilon ("enlist")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonEpsilon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonEpsilon_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
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

    //***************************************************************
    // Loop through the right arg counting all of the simple scalars
    //   and saving their storage type.  The result will be either
    //   simple homogeneous or simple heterogeneous.
    //***************************************************************

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkSym->stData.stGlbData));

                return PrimFnMonEpsilonGlb_EM_YY
                       (ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkSym->stData.stGlbData),  // HGLOBAL
                        lptkFunc);                                                      // Ptr to function token
            } // End IF

            // Handle the immediate case
            return PrimFnMonEpsilonImm_EM_YY
                   (TranslateImmTypeToArrayType (lptkRhtArg->tkData.tkSym->stFlags.ImmType),    // Immediate type
                    lptkRhtArg->tkData.tkSym->stData.stLongest,                                 // Immediate value
                    lptkFunc);                                                                  // Ptr to function token
        case TKT_VARIMMED:
            return PrimFnMonEpsilonImm_EM_YY
                   (TranslateImmTypeToArrayType (lptkRhtArg->tkFlags.ImmType),  // Immediate type
                    lptkRhtArg->tkData.tkLongest,                               // Immediate value
                    lptkFunc);                                                  // Ptr to function token
        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

            return PrimFnMonEpsilonGlb_EM_YY
                   (ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkGlbData),    // HGLOBAL
                    lptkFunc);                                          // Ptr to function token
        defstop
            return NULL;
    } // End SWITCH
} // End PrimFnMonEpsilon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonEpsilonImm_EM_YY
//
//  Monadic Epsilon on an immediate value
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonEpsilonImm_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonEpsilonImm_EM_YY
    (ARRAY_TYPES aplTypeRes,    // Right arg storage type
     APLLONGEST  aplLongest,    // Right arg immediate value
     LPTOKEN     lptkFunc)      // Ptr to function token

{
    APLUINT      ByteRes;       // # bytes needed for the result
    HGLOBAL      hGlbRes;       // Result global memory handle
    LPVOID       lpMemRes;      // Ptr to result global memory
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result

    // Calculate space needed for the result (a one-element vector)
    ByteRes = CalcArraySize (aplTypeRes, 1, 1);

    // Allocate space for the result
    // N.B.: Conversion from APLUINT to UINT
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       =          // Already zero from GHND
////lpHeader->SysVar     = 0;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = 1;
    lpHeader->Rank       = 1;

#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemRes) = 1;

    // Skip over the header and dimension to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:
            // Save the Boolean value
            *((LPAPLBOOL)  lpMemRes) = (APLBOOL)  aplLongest;

            break;

        case ARRAY_INT:
            // Save the Integer value
            *((LPAPLINT)   lpMemRes) = (APLINT)   aplLongest;

            break;

        case ARRAY_FLOAT:
            // Save the Float value
            *((LPAPLFLOAT) lpMemRes) = *(LPAPLFLOAT) &aplLongest;

            break;

        case ARRAY_CHAR:
            // Save the Char value
            *((LPAPLCHAR)  lpMemRes) = (APLCHAR)  aplLongest;

            break;

        defstop
            break;
    } // End SWITCH

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

    return lpYYRes;
} // End PrimFnMonEpsilonImm_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonEpsilonGlb_EM_YY
//
//  Monadic Epsilon on a global memory object:
//     Count the # scalars and the common storage type
//     Allocate storage
//     Copy the data to the result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonEpsilonGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonEpsilonGlb_EM_YY
    (HGLOBAL hGlbRht,               // Handle to right arg
     LPTOKEN lptkFunc)              // Ptr to function token

{
    APLSTYPE     aplTypeRes;        // The storage type of the result
    APLNELM      aplNELMRes;        // # elements in the result
    HGLOBAL      hGlbRes;           // Result global memory handle
    LPVOID       lpMemRes;          // Ptr to result global memory
    APLUINT      ByteRes;           // # bytes needed for the result
    UINT         uBitMask = 0x01,   // Bit mask for marching through Booleans
                 uBitIndex = 0;     // Bit index ...
    BOOL         bRet = TRUE;       // TRUE iff result is valid
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Traverse the array counting the # simple scalars
    //   and keeping track of the common storage type --
    //   it'll be simple homogeneous or heterogeneous.
    aplTypeRes = ARRAY_LIST;    // Initialize storage type
    aplNELMRes = 0;             // ...        count
    PrimFnMonEpsilonGlbCount (hGlbRht, &aplTypeRes, &aplNELMRes);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, 1);

    // Allocate space for the result
    // N.B.: Conversion from APLUINT to UINT
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;       // Already zero from GHND
////lpHeader->SysVar     = 0;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;

#undef  lpHeader

    // Save the dimension in the result
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimension to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // Copy the data from the right arg to the result
    PrimFnMonEpsilonGlbCopy (aplTypeRes,
                            &lpMemRes,
                            &uBitIndex,
                             ClrPtrTypeDirGlb (hGlbRht),
                             lptkFunc);
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

    // See if it fits into a lower (but not necessarily smaller) datatype
    lpYYRes->tkToken = *TypeDemote (&lpYYRes->tkToken);

    return lpYYRes;
} // End PrimFnMonEpsilonGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonEpsilonGlbCount
//
//  Count the # simple scalars in the global memory object
//***************************************************************************

void PrimFnMonEpsilonGlbCount
    (HGLOBAL    hGlbRht,
     LPAPLSTYPE lpaplTypeRes,
     LPAPLNELM  lpaplNELMRes)

{
    LPVOID   lpMemRht;
    APLSTYPE aplTypeRht;
    APLNELM  aplNELMRht;
    APLRANK  aplRankRht;
    APLUINT  uRht;
    static APLSTYPE aplTypeArr[ARRAY_LENGTH][ARRAY_LENGTH] =
    //      BOOL          INT           FLOAT         CHAR          HETERO        NESTED        INIT          APA
    {{ARRAY_BOOL  , ARRAY_INT   , ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_BOOL  , ARRAY_INT   },  // BOOL
     {ARRAY_INT   , ARRAY_INT   , ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_INT   , ARRAY_INT   },  // INT
     {ARRAY_FLOAT , ARRAY_FLOAT , ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_FLOAT , ARRAY_FLOAT },  // FLOAT
     {ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_CHAR  , ARRAY_HETERO, ARRAY_NESTED, ARRAY_CHAR  , ARRAY_HETERO},  // CHAR
     {ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_HETERO, ARRAY_HETERO},  // HETERO
     {ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED},  // NESTED
     {ARRAY_BOOL  , ARRAY_INT   , ARRAY_FLOAT , ARRAY_CHAR  , ARRAY_HETERO, ARRAY_NESTED, ARRAY_LIST  , ARRAY_INT   },  // INIT
     {ARRAY_INT   , ARRAY_INT   , ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_INT   , ARRAY_INT   },  // APA
    };

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)

    // Get the Array Type and NELM
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;

#undef  lpHeader

    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_CHAR:
        case ARRAY_APA:
        case ARRAY_HETERO:
            // Empty args don't contribute to the result,
            //   so we don't want to risk changing the storage
            //   type to HETERO by running through aplTypeArr.
            if (aplNELMRht NE 0)
            {
                *lpaplTypeRes = aplTypeArr[*lpaplTypeRes][aplTypeRht];
                (*lpaplNELMRes) += aplNELMRht;
            } // End IF

            break;

        case ARRAY_NESTED:
            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

            // Loop through the elements
            for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
            // Split cases based upon the ptr type
            switch (GetPtrTypeInd (lpMemRht))
            {
                case PTRTYPE_STCONST:
                    *lpaplTypeRes = aplTypeArr[*lpaplTypeRes][TranslateImmTypeToArrayType ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType)];
                    (*lpaplNELMRes)++;

                    break;

                case PTRTYPE_HGLOBAL:
                    // It's a valid HGLOBAL variable array
                    Assert (IsGlbTypeVarInd (lpMemRht));

                    PrimFnMonEpsilonGlbCount (ClrPtrTypeIndGlb (lpMemRht),
                                              lpaplTypeRes,
                                              lpaplNELMRes);
                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
} // End PrimFnMonEpsilonGlbCount


//***************************************************************************
//  $PrimFnMonEpsilonGlbCopy
//
//  Copy the data from the right arg to the result
//***************************************************************************

void PrimFnMonEpsilonGlbCopy
    (APLSTYPE aplTypeRes,           // Result type
     LPVOID  *lplpMemRes,           // Ptr to ptr to result memory
     LPUINT   lpuBitIndex,          // Ptr to uBitIndex
     HGLOBAL  hGlbRht,              // Handle to right arg
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    APLSTYPE   aplTypeRht;          // Right arg storage type
    APLNELM    aplNELMRht;          // Right arg NELM
    APLRANK    aplRankRht;          // Right arg rank
    LPVOID     lpMemRht;            // Ptr to right arg global memory
    UINT       uBitMask = 0x01;     // Bit mask for marching through Booleans
    APLUINT    uRht;                // Right arg loop counter
    APLUINT    ByteRes;             // # bytes needed for the result
    APLINT     apaOffRht,           // Right arg APA offset
               apaMulRht;           // ...           multiplier
    APLLONGEST aplVal;              // Temporary value

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)

    // Get the Array Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;

#undef  lpHeader

    // Skip past the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // If the right arg is an APA, ...
    if (aplTypeRht EQ ARRAY_APA)
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Split cases based upon the result's storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:                            // Res = BOOL  , Rht = BOOL/NESTED
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:                    // Res = BOOL  , Rht = BOOL
                    // The result's and right arg's storage type are the same,
                    //   so just copy the data if it's aligned
                    if (*lpuBitIndex EQ 0)
                    {
                        // N.B.: Conversion from APLUINT to UINT.
                        ByteRes = sizeof (APLBOOL) * RoundUpBits8 (aplNELMRht);
                        Assert (ByteRes EQ (UINT) ByteRes);
                        CopyMemory (*lplpMemRes, lpMemRht, (UINT) ByteRes);

                        // Unless the right arg is a multiple of NBIB bits,
                        //   the following += is one too large
                        ((LPAPLBOOL) *lplpMemRes) += RoundUpBits8 (aplNELMRht);

                        // Calculate the next bit index
                        *lpuBitIndex = (UINT) (aplNELMRht % NBIB);

                        // Account for non-NBIB # bits in right arg
                        if (*lpuBitIndex)
                            ((LPAPLBOOL) *lplpMemRes)--;
                    } else
                    {
                        // Loop through the elements
                        for (uRht = 0; uRht < aplNELMRht; uRht++)
                        {
                            *(*(LPAPLBOOL *) lplpMemRes) |= ((uBitMask & *((LPAPLBOOL) lpMemRht)) ? 1 : 0) << *lpuBitIndex;

                            // Check for end-of-byte
                            if (++*lpuBitIndex EQ NBIB)
                            {
                                *lpuBitIndex = 0;               // Start over
                                (*(LPAPLBOOL *) lplpMemRes)++;  // Skip to next byte
                            } // End IF

                            // Shift over the bit mask
                            uBitMask <<= 1;

                            // Check for end-of-byte
                            if (uBitMask EQ END_OF_BYTE)
                            {
                                uBitMask = 0x01;                // Start over
                                ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                            } // End IF
                        } // End FOR
                    } // End IF

                    break;

                case ARRAY_NESTED:                  // Res = BOOL  , Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    // Split cases based upon the ptr type
                    switch (GetPtrTypeInd (lpMemRht))
                    {
                        case PTRTYPE_STCONST:       // Res = BOOL  , Rht = NESTED:BOOL/INT/FLOAT/CHAR
                            // Split cases based upon the right arg's immediate type
                            switch ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType)
                            {
                                case IMMTYPE_BOOL:  // Res = BOOL  , Rht = NESTED:BOOL
                                    *(*(LPAPLBOOL *) lplpMemRes) |= ((*(LPAPLHETERO) lpMemRht)->stData.stBoolean) << *lpuBitIndex;

                                    // Check for end-of-byte
                                    if (++*lpuBitIndex EQ NBIB)
                                    {
                                        *lpuBitIndex = 0;               // Start over
                                        (*(LPAPLBOOL *) lplpMemRes)++;  // Skip to next byte
                                    } // End IF

                                    break;

                                case IMMTYPE_INT:   // Res = BOOL  , Rht = NESTED:INT    (Can't happen w/Boolean result)
                                case IMMTYPE_FLOAT: // Res = BOOL  , Rht = NESTED:FLOAT  (...)
                                case IMMTYPE_CHAR:  // Res = BOOL  , Rht = NESTED:CHAR   (...)
                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case PTRTYPE_HGLOBAL:       // Res = BOOL  , Rht = NESTED:NESTED
                            // It's a valid HGLOBAL variable array
                            Assert (IsGlbTypeVarInd (lpMemRht));

                            // Copy the data to the result
                            PrimFnMonEpsilonGlbCopy (aplTypeRes,
                                                     lplpMemRes,
                                                     lpuBitIndex,
                                                     ClrPtrTypeIndGlb (lpMemRht),
                                                     lptkFunc);
                            break;

                        defstop
                            break;
                    } // End FOR/SWITCH

                    break;

                case ARRAY_INT:                     // Res = BOOL  , Rht = INT    (Can't happen w/Boolean result)
                case ARRAY_FLOAT:                   // Res = BOOL  , Rht = FLOAT  (...)
                case ARRAY_CHAR:                    // Res = BOOL  , Rht = CHAR   (...)
                case ARRAY_APA:                     // Res = BOOL  , Rht = APA    (...)
                case ARRAY_HETERO:                  // Res = BOOL  , Rht = HETERO (...)
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_INT:                             // Res = INT   , Rht = BOOL/INT/APA/NESTED
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:                    // Res = INT   , Rht = BOOL
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        *(*(LPAPLINT *) lplpMemRes)++ = (uBitMask & *((LPAPLBOOL) lpMemRht)) ? 1 : 0;

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = 0x01;                // Start over
                            ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:                     // Res = INT   , Rht = INT
                    // The result's and right arg's storage type are the same,
                    //   so just copy the data
                    // N.B.: Conversion from APLUINT to UINT.
                    ByteRes = sizeof (APLINT) * aplNELMRht;
                    Assert (ByteRes EQ (UINT) ByteRes);
                    CopyMemory (*lplpMemRes, lpMemRht, (UINT) ByteRes);
                    ((LPAPLINT) *lplpMemRes) += aplNELMRht;

                    break;

                case ARRAY_APA:                     // Res = INT   , Rht = APA
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                        *(*(LPAPLINT *) lplpMemRes)++ = apaOffRht + apaMulRht * uRht;
                    break;

                case ARRAY_NESTED:                  // Res = INT   , Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    // Split cases based upon the ptr type
                    switch (GetPtrTypeInd (lpMemRht))
                    {
                        case PTRTYPE_STCONST:       // Res = INT   , Rht = NESTED:BOOL/INT/FLOAT/CHAR
                            // Split cases based upon the right arg's immediate type
                            switch ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType)
                            {
                                case IMMTYPE_BOOL:  // Res = INT   , Rht = NESTED:BOOL
                                    *(*(LPAPLINT *) lplpMemRes)++ = (*(LPAPLHETERO) lpMemRht)->stData.stBoolean;

                                    break;

                                case IMMTYPE_INT:   // Res = INT   , Rht = NESTED:INT
                                    *(*(LPAPLINT *) lplpMemRes)++ = (*(LPAPLHETERO) lpMemRht)->stData.stInteger;

                                    break;

                                case IMMTYPE_FLOAT: // Res = INT   , Rht = NESTED:FLOAT  (Can't happen w/Integer result)
                                case IMMTYPE_CHAR:  // Res = INT   , Rht = NESTED:CHAR   (...)
                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case PTRTYPE_HGLOBAL:       // Res = INT   , Rht = NESTED:NESTED
                            // It's a valid HGLOBAL variable array
                            Assert (IsGlbTypeVarInd (lpMemRht));

                            // Copy the data to the result
                            PrimFnMonEpsilonGlbCopy (aplTypeRes,
                                                     lplpMemRes,
                                                     lpuBitIndex,
                                                     ClrPtrTypeIndGlb (lpMemRht),
                                                     lptkFunc);
                            break;

                        defstop
                            break;
                    } // End FOR/SWITCH

                    break;

                case ARRAY_FLOAT:                   // Res = INT   , Rht = FLOAT  (Can't happen w/Integer result)
                case ARRAY_CHAR:                    // Res = INT   , Rht = CHAR   (...)
                case ARRAY_HETERO:                  // Res = INT   , Rht = HETERO (...)
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_FLOAT:                           // Res = FLOAT , Rht = BOOL/INT/FLOAT/APA/NESTED
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:                    // Res = FLOAT , Rht = BOOL
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        *(*(LPAPLFLOAT *) lplpMemRes)++ = (uBitMask & *((LPAPLBOOL) lpMemRht)) ? 1 : 0;

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = 0x01;                // Start over
                            ((LPAPLBOOL) lpMemRht)++;       // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:                     // Res = FLOAT , Rht = INT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                        *(*(LPAPLFLOAT *) lplpMemRes)++ = (APLFLOAT) *((LPAPLINT) lpMemRht)++;
                    break;

                case ARRAY_FLOAT:                   // Res = FLOAT , Rht = FLOAT
                    // The result's and right arg's storage type are the same,
                    //   so just copy the data
                    // N.B.: Conversion from APLUINT to UINT.
                    ByteRes = sizeof (APLFLOAT) * aplNELMRht;
                    Assert (ByteRes EQ (UINT) ByteRes);
                    CopyMemory (*lplpMemRes, lpMemRht, (UINT) ByteRes);
                    ((LPAPLFLOAT) *lplpMemRes) += aplNELMRht;

                    break;

                case ARRAY_APA:                     // Res = FLOAT , Rht = APA
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                        *(*(LPAPLFLOAT *) lplpMemRes)++ = (APLFLOAT) (APLINT) (apaOffRht + apaMulRht * uRht);
                    break;

                case ARRAY_NESTED:                  // Res = FLOAT , Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    // Split cases based upon the ptr type
                    switch (GetPtrTypeInd (lpMemRht))
                    {
                        case PTRTYPE_STCONST:       // Res = FLOAT , Rht = NESTED:BOOL/INT/FLOAT/CHAR
                            // Split cases based upon the right arg's immediate type
                            switch ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType)
                            {
                                case IMMTYPE_BOOL:  // Res = FLOAT , Rht = NESTED:BOOL
                                    *(*(LPAPLFLOAT *) lplpMemRes)++ = (*(LPAPLHETERO) lpMemRht)->stData.stBoolean;

                                    break;

                                case IMMTYPE_INT:   // Res = FLOAT , Rht = NESTED:INT
                                    *(*(LPAPLFLOAT *) lplpMemRes)++ = (APLFLOAT) (*(LPAPLHETERO) lpMemRht)->stData.stInteger;

                                    break;

                                case IMMTYPE_FLOAT: // Res = FLOAT , Rht = NESTED:FLOAT
                                    *(*(LPAPLFLOAT *) lplpMemRes)++ = (*(LPAPLHETERO) lpMemRht)->stData.stFloat;

                                    break;

                                case IMMTYPE_CHAR:  // Res = FLOAT , Rht = NESTED:CHAR   (Can't happen w/Float result)
                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case PTRTYPE_HGLOBAL:       // Res = FLOAT , Rht = NESTED:NESTED
                            // It's a valid HGLOBAL variable array
                            Assert (IsGlbTypeVarInd (lpMemRht));

                            // Copy the data to the result
                            PrimFnMonEpsilonGlbCopy (aplTypeRes,
                                                     lplpMemRes,
                                                     lpuBitIndex,
                                                     ClrPtrTypeIndGlb (lpMemRht),
                                                     lptkFunc);
                            break;

                        defstop
                            break;
                    } // End FOR/SWITCH

                    break;

                case ARRAY_CHAR:                    // Res = FLOAT , Rht = CHAR   (Can't happen w/Float result)
                case ARRAY_HETERO:                  // Res = FLOAT , Rht = HETERO (...)
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_CHAR:                            // Res = CHAR  , Rht = CHAR/NESTED
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_CHAR:                    // Res = CHAR  , Rht = CHAR
                    // The result's and right arg's storage type are the same,
                    //   so just copy the data
                    // N.B.: Conversion from APLUINT to UINT.
                    ByteRes = sizeof (APLCHAR) * aplNELMRht;
                    Assert (ByteRes EQ (UINT) ByteRes);
                    CopyMemory (*lplpMemRes, lpMemRht, (UINT) ByteRes);
                    ((LPAPLCHAR) *lplpMemRes) += aplNELMRht;

                    break;

                case ARRAY_NESTED:                  // Res = CHAR  , Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    // Split cases based upon the ptr type
                    switch (GetPtrTypeInd (lpMemRht))
                    {
                        case PTRTYPE_STCONST:       // Res = CHAR  , Rht = NESTED:BOOL/INT/FLOAT/CHAR
                            // Split cases based upon the right arg's immediate type
                            switch ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType)
                            {
                                case IMMTYPE_CHAR:  // Res = CHAR  , Rht = NESTED:CHAR
                                    *(*(LPAPLCHAR *) lplpMemRes)++ = (*(LPAPLHETERO) lpMemRht)->stData.stChar;

                                    break;

                                case IMMTYPE_BOOL:  // Res = CHAR  , Rht = NESTED:BOOL    (Can't happen w/Char result)
                                case IMMTYPE_INT:   // Res = CHAR  , Rht = NESTED:INT     (...)
                                case IMMTYPE_FLOAT: // Res = CHAR  , Rht = NESTED:FLOAT   (...)
                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case PTRTYPE_HGLOBAL:       // Res = CHAR  , Rht = NESTED:NESTED
                            // It's a valid HGLOBAL variable array
                            Assert (IsGlbTypeVarInd (lpMemRht));

                            // Copy the data to the result
                            PrimFnMonEpsilonGlbCopy (aplTypeRes,
                                                     lplpMemRes,
                                                     lpuBitIndex,
                                                     ClrPtrTypeIndGlb (lpMemRht),
                                                     lptkFunc);
                            break;

                        defstop
                            break;
                    } // End FOR/SWITCH

                    break;

                case ARRAY_BOOL:                    // Res = CHAR  , Rht = BOOL   (Can't happen w/Char result)
                case ARRAY_INT:                     // Res = CHAR  , Rht = INT    (...)
                case ARRAY_APA:                     // Res = CHAR  , Rht = APA    (...)
                case ARRAY_HETERO:                  // Res = CHAR  , Rht = HETERO (...)
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_HETERO:                          // Res = HETERO, Rht = BOOL/INT/FLOAT/APA/CHAR/HETERO/NESTED
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:                    // Res = HETERO, Rht = BOOL
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        aplVal = (uBitMask & *((LPAPLBOOL) lpMemRht)) ? 1 : 0;
                        *(*(LPAPLHETERO *) lplpMemRes)++ = MakeSymEntry_EM (IMMTYPE_BOOL, &aplVal, lptkFunc);

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = 0x01;         // Start over
                            ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:                     // Res = HETERO, Rht = INT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        aplVal = *((LPAPLINT) lpMemRht)++;
                        *(*(LPAPLHETERO *) lplpMemRes)++ = MakeSymEntry_EM (IMMTYPE_INT, &aplVal, lptkFunc);
                    } // End FOR

                    break;

                case ARRAY_FLOAT:                   // Res = HETERO, Rht = FLOAT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        aplVal = *(LPAPLLONGEST) ((LPAPLFLOAT) lpMemRht)++;
                        *(*(LPAPLHETERO *) lplpMemRes)++ = MakeSymEntry_EM (IMMTYPE_FLOAT, &aplVal, lptkFunc);
                    } // End FOR

                    break;

                case ARRAY_APA:                     // Res = HETERO, Rht = APA
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        aplVal = apaOffRht + apaMulRht * uRht;
                        *(*(LPAPLHETERO *) lplpMemRes)++ = MakeSymEntry_EM (IMMTYPE_INT, &aplVal, lptkFunc);
                    } // End FOR

                    break;

                case ARRAY_CHAR:                    // Res = HETERO, Rht = CHAR
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        aplVal = *((LPAPLCHAR) lpMemRht)++;
                        *(*(LPAPLHETERO *) lplpMemRes)++ = MakeSymEntry_EM (IMMTYPE_CHAR, &aplVal, lptkFunc);
                    } // End FOR

                    break;

                case ARRAY_HETERO:                  // Res = HETERO, Rht = HETERO
                    // The result's and right arg's storage type are the same,
                    //   so just copy the data
                    // N.B.: Conversion from APLUINT to UINT.
                    ByteRes = sizeof (APLHETERO) * aplNELMRht;
                    Assert (ByteRes EQ (UINT) ByteRes);
                    CopyMemory (*lplpMemRes, lpMemRht, (UINT) ByteRes);
                    ((LPAPLHETERO) *lplpMemRes) += aplNELMRht;

                    break;

                case ARRAY_NESTED:                  // Res = HETERO, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    // Split cases based upon the ptr type
                    switch (GetPtrTypeInd (lpMemRht))
                    {
                        case PTRTYPE_STCONST:       // Res = HETERO, Rht = NESTED:BOOL/INT/FLOAT/CHAR
                            // Split cases based upon the right arg's immediate type
                            switch ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType)
                            {
                                case IMMTYPE_BOOL:  // Res = HETERO, Rht = NESTED:BOOL
                                    aplVal = (*(LPAPLHETERO) lpMemRht)->stData.stBoolean;
                                    *(*(LPAPLNESTED *) lplpMemRes)++ = MakeSymEntry_EM (IMMTYPE_BOOL, &aplVal, lptkFunc);

                                    break;

                                case IMMTYPE_INT:   // Res = HETERO, Rht = NESTED:INT
                                    aplVal = (*(LPAPLHETERO) lpMemRht)->stData.stInteger;
                                    *(*(LPAPLNESTED *) lplpMemRes)++ = MakeSymEntry_EM (IMMTYPE_INT, &aplVal, lptkFunc);

                                    break;

                                case IMMTYPE_FLOAT: // Res = HETERO, Rht = NESTED:FLOAT
                                    aplVal = *(LPAPLLONGEST) &(*(LPAPLHETERO) lpMemRht)->stData.stFloat;
                                    *(*(LPAPLNESTED *) lplpMemRes)++ = MakeSymEntry_EM (IMMTYPE_FLOAT, &aplVal, lptkFunc);

                                    break;


                                case IMMTYPE_CHAR:  // Res = HETERO, Rht = NESTED:CHAR
                                    aplVal = (*(LPAPLHETERO) lpMemRht)->stData.stChar;
                                    *(*(LPAPLNESTED *) lplpMemRes)++ = MakeSymEntry_EM (IMMTYPE_CHAR, &aplVal, lptkFunc);

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case PTRTYPE_HGLOBAL:       // Res = HETERO, Rht = NESTED:NESTED
                            // It's a valid HGLOBAL variable array
                            Assert (IsGlbTypeVarInd (lpMemRht));

                            // Copy the data to the result
                            PrimFnMonEpsilonGlbCopy (aplTypeRes,
                                                     lplpMemRes,
                                                     lpuBitIndex,
                                                     ClrPtrTypeIndGlb (lpMemRht),
                                                     lptkFunc);
                            break;

                        defstop
                            break;
                    } // End FOR/SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_APA:                             // Res = APA    (Can't happen w/Enlist)
        case ARRAY_NESTED:                          // Res = NESTED (...)
        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
} // End PrimFnMonEpsilonGlbCopy


//***************************************************************************
//  $PrimFnDydEpsilon_EM_YY
//
//  Primitive function for dyadic Epsilon ("member of")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydEpsilon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydEpsilon_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeLft,
                 aplTypeRht;
    APLNELM      aplNELMLft,
                 aplNELMRht;
    APLRANK      aplRankLft,
                 aplRankRht;
    HGLOBAL      hGlbLft,
                 hGlbRht;
    LPVOID       lpMemLft,
                 lpMemRht;
    BOOL         bRet = TRUE;
    LPPL_YYSTYPE lpYYRes;

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

    return PrimFnNonceError_EM (lptkFunc);

    DbgBrk ();          // ***FINISHME*** -- PrimFnDydEpsilon_EM_YY

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);
















    // Allocate a new YYRes
    lpYYRes = YYAlloc ();







    if (hGlbLft && lpMemLft)
    {
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (bRet)
        return lpYYRes;
    else
        return NULL;
} // End PrimFnDydEpsilon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_epsilon.c
//***************************************************************************
