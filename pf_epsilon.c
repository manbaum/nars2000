//***************************************************************************
//  NARS2000 -- Primitive Function -- Epsilon
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


//***************************************************************************
//  PrimFnEpsilon_EM
//
//  Primitive function for monadic and dyadic Epsilon ("enlist" and "member")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnEpsilon_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnEpsilon_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_EPSILON);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonEpsilon_EM (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydEpsilon_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnPlus_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonEpsilon_EM
//
//  Primitive function for monadic Epsilon (enlist)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonEpsilon_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonEpsilon_EM
    (LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

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
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lptkRhtArg->tkData.lpSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.lpSym->stData.stGlbData));

                return PrimFnMonEpsilonGlb_EM (ClrPtrTypeDirGlb (lptkRhtArg->tkData.lpSym->stData.stGlbData),
                                               lptkFunc);
            } // End IF

            // Handle the immediate case
            return PrimFnMonEpsilonImm_EM (TranslateImmTypeToArrayType (lptkRhtArg->tkData.lpSym->stFlags.ImmType),
                                           lptkRhtArg->tkData.lpSym->stData.stLongest,
                                           lptkFunc);
        case TKT_VARIMMED:
            // Split cases based upon the token's immediate type
            switch (lptkRhtArg->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                case IMMTYPE_INT:
                case IMMTYPE_FLOAT:
                case IMMTYPE_CHAR:
                    return PrimFnMonEpsilonImm_EM (TranslateImmTypeToArrayType (lptkRhtArg->tkFlags.ImmType),
                                                   lptkRhtArg->tkData.tkLongest,
                                                   lptkFunc);
                defstop
                    return NULL;
            } // End SWITCH

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

            return PrimFnMonEpsilonGlb_EM (ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkGlbData),
                                          lptkFunc);
        case TKT_LIST:
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkFunc);
            return NULL;

        defstop
            return NULL;
    } // End SWITCH
} // End PrimFnMonEpsilon_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonEpsilonImm_EM
//
//  Monadic Epsilon on an immediate value
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonEpsilonImm_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonEpsilonImm_EM
    (APLSTYPE   aplTypeRes,
     APLLONGEST aplLongest,
     LPTOKEN    lptkFunc)

{
    APLUINT ByteRes;
    HGLOBAL hGlbRes;
    LPVOID  lpMemRes;

    // Get new index into YYRes
    YYResIndex = (YYResIndex + 1) % NUMYYRES;

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
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
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

    // Split cases based upon the result's storage type
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

    // Fill in the result token
    YYRes[YYResIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
////YYRes[YYResIndex].tkToken.tkFlags.ImmType   = 0;
////YYRes[YYResIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYResIndex].tkToken.tkFlags.Color     =
    YYRes[YYResIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    YYRes[YYResIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return &YYRes[YYResIndex];
} // End PrimFnMonEpsilonImm_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonEpsilonGlb_EM
//
//  Monadic Epsilon on a global memory object:
//     Count the # scalars and the common storage type
//     Allocate storage
//     Copy the data to the result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonEpsilonGlb_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonEpsilonGlb_EM
    (HGLOBAL hGlbRht,
     LPTOKEN lptkFunc)

{
    LPVOID     lpMemRes;
    APLSTYPE   aplTypeRes;        // The storage type of the result
    APLNELM    aplNELMRes;        // # elements in the result
    HGLOBAL    hGlbRes;
    APLUINT    ByteRes;
    UINT       uBitMask = 0x01,
               uBitIndex = 0;
    BOOL       bRet = TRUE;

    // Get new index into YYRes
    YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Traverse the array counting the # simple scalars
    //   and keeping track of the common storage type --
    //   it'll be simple homogeneous or heterogeneous.
    aplTypeRes = ARRAY_LIST;    // Initialize storage type
    aplNELMRes = 0;             // ...        count
    PrimFnMonEpsilonGlbCount (hGlbRht, &aplTypeRes, &aplNELMRes);

    // Calculate the space needed for the result
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
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;       // Already zero from GHND
////lpHeader->SysVar     = 0;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;

#undef  lpHeader

    // Save the dimension in the result
    *VarArrayBaseToDim (lpMemRes) = 1;

    // Skip over the header and dimension to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // Copy the data from the right arg to the result
    PrimFnMonEpsilonGlbCopy (aplTypeRes,
                            &lpMemRes,
                            &uBitIndex,
                             ClrPtrTypeIndGlb (hGlbRht),
                             lptkFunc);
    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Fill in the result token
    YYRes[YYResIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
////YYRes[YYResIndex].tkToken.tkFlags.ImmType   = 0;
////YYRes[YYResIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYResIndex].tkToken.tkFlags.Color     =
    YYRes[YYResIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    YYRes[YYResIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return &YYRes[YYResIndex];
} // End PrimFnMonEpsilonGlb_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonEpsilonGlbCount
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

    // Save the type and NELM
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;

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
            DbgBrk ();          // ***TESTME***

            // Empty args don't contribute to the result,
            //   so we don't want to risk changing the storage
            //   type to HETERO by running through aplTypeArr.
            if (aplNELMRht NE 0)
            {
                *lpaplTypeRes =aplTypeArr[*lpaplTypeRes][aplTypeRht];
                *lpaplNELMRes += aplNELMRht;
            } // End IF

            break;

        case ARRAY_NESTED:
            DbgBrk ();          // ***TESTME***

            // Loop through the elements
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // It's a valid HGLOBAL variable array
                Assert (IsGlbTypeVarInd (lpMemRht));

                PrimFnMonEpsilonGlbCount (ClrPtrTypeIndGlb (lpMemRht),
                                          lpaplTypeRes,
                                          lpaplNELMRes);
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
} // End PrimFnMonEpsilonGlbCount


//***************************************************************************
//  PrimFnMonEpsilonGlbCopy
//
//  Copy the data from the right arg to the result
//***************************************************************************

void PrimFnMonEpsilonGlbCopy
    (APLSTYPE aplTypeRes,
     LPVOID   *lplpMemRes,
     LPUINT   lpuBitIndex,
     HGLOBAL  hGlbRht,
     LPTOKEN  lptkFunc)

{
    LPVOID     lpMemRht;
    APLSTYPE   aplTypeRht;
    APLNELM    aplNELMRht;
    UINT       uBitMask = 0x01;
    APLUINT    uRht;
    APLRANK    aplRankRht;
    APLUINT    ByteRes;
    APLINT     apaOffRht,
               apaMulRht;
    APLLONGEST aplVal;

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)

    // Save the Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;

#undef  lpHeader

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

    // Skip past the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // If the right arg is an APA,
    //   get its parameters
    if (aplTypeRht EQ ARRAY_APA)
    {

#define lpHeader    ((LPAPLAPA) lpMemRht)

        apaOffRht = lpHeader->Off;
        apaMulRht = lpHeader->Mul;

#undef  lpHeader

    } // End IF

    // Split cases based upon the result's storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:                    // Res = BOOL, Rht = BOOL/NESTED
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:            // Res = BOOL, Rht = BOOL
                    DbgBrk ();          // ***TESTME***

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
                            **((LPAPLBOOL *) lplpMemRes) |= ((uBitMask & *((LPAPLBOOL) lpMemRht)) ? 1 : 0) << *lpuBitIndex;

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

                case ARRAY_NESTED:          // Res = BOOL, Rht = NESTED
                    DbgBrk ();          // ***TESTME***

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    {
                        // It's a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarInd (lpMemRht));

                        // Copy the data to the result
                        PrimFnMonEpsilonGlbCopy (aplTypeRes,
                                                 lplpMemRes,
                                                 lpuBitIndex,
                                                 ClrPtrTypeIndGlb (lpMemRht),
                                                 lptkFunc);
                    } // End FOR

                    break;

                case ARRAY_INT:             // Can't happen w/Boolean result
                case ARRAY_FLOAT:           // ...
                case ARRAY_CHAR:            // ...
                case ARRAY_APA:             // ...
                case ARRAY_HETERO:          // ...
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_INT:                     // Res = INT, Rht = BOOL/INT/APA/NESTED
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:            // Res = INT, Rht = BOOL
                    DbgBrk ();          // ***TESTME***

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

                case ARRAY_INT:             // Res = INT, Rht = INT
                    DbgBrk ();          // ***TESTME***

                    // The result's and right arg's storage type are the same,
                    //   so just copy the data
                    // N.B.: Conversion from APLUINT to UINT.
                    ByteRes = sizeof (APLINT) * aplNELMRht;
                    Assert (ByteRes EQ (UINT) ByteRes);
                    CopyMemory (*lplpMemRes, lpMemRht, (UINT) ByteRes);
                    ((LPAPLINT) *lplpMemRes) += aplNELMRht;

                    break;

                case ARRAY_APA:             // Res = INT, Rht = APA
                    DbgBrk ();          // ***TESTME***

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                        *(*(LPAPLINT *) lplpMemRes)++ = apaOffRht + apaMulRht * uRht;
                    break;

                case ARRAY_NESTED:          // Res = INT, Rht = NESTED
                    DbgBrk ();          // ***TESTME***

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    {
                        // It's a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarInd (lpMemRht));

                        // Copy the data to the result
                        PrimFnMonEpsilonGlbCopy (aplTypeRes,
                                                 lplpMemRes,
                                                 lpuBitIndex,
                                                 ClrPtrTypeIndGlb (lpMemRht),
                                                 lptkFunc);
                    } // End FOR

                    break;

                case ARRAY_FLOAT:           // Can't happen w/Integer result
                case ARRAY_CHAR:            // ...
                case ARRAY_HETERO:          // ...
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_FLOAT:                   // Res = FLOAT, Rht = BOOL/INT/FLOAT/APA/NESTED
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:            // Res = FLOAT, Rht = BOOL
                    DbgBrk ();          // ***TESTME***

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

                case ARRAY_INT:             // Res = FLOAT, Rht = INT
                    DbgBrk ();          // ***TESTME***

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                        *(*(LPAPLFLOAT *) lplpMemRes)++ = (APLFLOAT) *(LPAPLINT) lpMemRht;
                    break;

                case ARRAY_FLOAT:           // Res = FLOAT, Rht = FLOAT
                    DbgBrk ();          // ***TESTME***

                    // The result's and right arg's storage type are the same,
                    //   so just copy the data
                    // N.B.: Conversion from APLUINT to UINT.
                    ByteRes = sizeof (APLFLOAT) * aplNELMRht;
                    Assert (ByteRes EQ (UINT) ByteRes);
                    CopyMemory (*lplpMemRes, lpMemRht, (UINT) ByteRes);
                    ((LPAPLFLOAT) *lplpMemRes) += aplNELMRht;

                    break;

                case ARRAY_APA:             // Res = FLOAT, Rht = APA
                    DbgBrk ();          // ***TESTME***

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                        *(*(LPAPLFLOAT *) lplpMemRes)++ = (APLFLOAT) (APLINT) (apaOffRht + apaMulRht * uRht);
                    break;

                case ARRAY_NESTED:          // Res = FLOAT, Rht = NESTED
                    DbgBrk ();          // ***TESTME***

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    {
                        // It's a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarInd (lpMemRht));

                        // Copy the data to the result
                        PrimFnMonEpsilonGlbCopy (aplTypeRes,
                                                 lplpMemRes,
                                                 lpuBitIndex,
                                                 ClrPtrTypeIndGlb (lpMemRht),
                                                 lptkFunc);
                    } // End FOR

                    break;

                case ARRAY_CHAR:            // Can't happen w/Float result
                case ARRAY_HETERO:          // ...
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_CHAR:                    // Res = CHAR, Rht = CHAR/NESTED
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_CHAR:            // Res = CHAR, Rht = CHAR
                    DbgBrk ();          // ***TESTME***

                    // The result's and right arg's storage type are the same,
                    //   so just copy the data
                    // N.B.: Conversion from APLUINT to UINT.
                    ByteRes = sizeof (APLCHAR) * aplNELMRht;
                    Assert (ByteRes EQ (UINT) ByteRes);
                    CopyMemory (*lplpMemRes, lpMemRht, (UINT) ByteRes);
                    ((LPAPLCHAR) *lplpMemRes) += aplNELMRht;

                    break;

                case ARRAY_NESTED:          // Res = CHAR, Rht = NESTED
                    DbgBrk ();          // ***TESTME***

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    {
                        // It's a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarInd (lpMemRht));

                        // Copy the data to the result
                        PrimFnMonEpsilonGlbCopy (aplTypeRes,
                                                 lplpMemRes,
                                                 lpuBitIndex,
                                                 ClrPtrTypeIndGlb (lpMemRht),
                                                 lptkFunc);
                    } // End FOR

                    break;

                case ARRAY_BOOL:            // Can't happen w/CHAR result
                case ARRAY_INT:             // ...
                case ARRAY_APA:             // ...
                case ARRAY_HETERO:          // ...
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_HETERO:                  // Res = HETERO, Rht = BOOL/INT/FLOAT/APA/CHAR/HETERO/NESTED
            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:            // Res = HETERO, Rht = BOOL
                    DbgBrk ();          // ***TESTME***

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

                case ARRAY_INT:             // Res = HETERO, Rht = INT
                    DbgBrk ();          // ***TESTME***

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        aplVal = *(LPAPLINT) lpMemRht;
                        *(*(LPAPLHETERO *) lplpMemRes)++ = MakeSymEntry_EM (IMMTYPE_INT, &aplVal, lptkFunc);
                    } // End FOR

                    break;

                case ARRAY_FLOAT:           // Res = HETERO, Rht = FLOAT
                    DbgBrk ();          // ***TESTME***

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        aplVal = *(LPAPLLONGEST) (LPAPLFLOAT) lpMemRht;
                        *(*(LPAPLHETERO *) lplpMemRes)++ = MakeSymEntry_EM (IMMTYPE_FLOAT, &aplVal, lptkFunc);
                    } // End FOR

                    break;

                case ARRAY_APA:             // Res = HETERO, Rht = APA
                    DbgBrk ();          // ***TESTME***

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        aplVal = apaOffRht + apaMulRht * uRht;
                        *(*(LPAPLHETERO *) lplpMemRes)++ = MakeSymEntry_EM (IMMTYPE_INT, &aplVal, lptkFunc);
                    } // End FOR

                    break;

                case ARRAY_CHAR:            // Res = HETERO, Rht = CHAR
                    DbgBrk ();          // ***TESTME***

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        aplVal = *(LPAPLCHAR) lpMemRht;
                        *(*(LPAPLHETERO *) lplpMemRes)++ = MakeSymEntry_EM (IMMTYPE_CHAR, &aplVal, lptkFunc);
                    } // End FOR

                    break;

                case ARRAY_HETERO:          // Res = HETERO, Rht = HETERO
                    DbgBrk ();          // ***TESTME***

                    // The result's and right arg's storage type are the same,
                    //   so just copy the data
                    // N.B.: Conversion from APLUINT to UINT.
                    ByteRes = sizeof (APLHETERO) * aplNELMRht;
                    Assert (ByteRes EQ (UINT) ByteRes);
                    CopyMemory (*lplpMemRes, lpMemRht, (UINT) ByteRes);
                    ((LPAPLHETERO) *lplpMemRes) += aplNELMRht;

                    break;

                case ARRAY_NESTED:          // Res = HETERO, Rht = NESTED
                    DbgBrk ();          // ***TESTME***

                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLNESTED) lpMemRht)++)
                    {
                        // It's a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarInd (lpMemRht));

                        // Copy the data to the result
                        PrimFnMonEpsilonGlbCopy (aplTypeRes,
                                                 lplpMemRes,
                                                 lpuBitIndex,
                                                 ClrPtrTypeIndGlb (lpMemRht),
                                                 lptkFunc);
                    } // End FOR

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_APA:                     // Can't happen w/Enlist
        case ARRAY_NESTED:                  // ...
        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
} // End PrimFnMonEpsilonGlbCopy


//***************************************************************************
//  PrimFnDydEpsilon_EM
//
//  Primitive function for dyadic Epsilon (member)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydEpsilon_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDydEpsilon_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
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
    BOOL     bRet = TRUE;

    // Get new index into YYRes
    YYResIndex = (YYResIndex + 1) % NUMYYRES;

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

    DbgBrk ();          // ***FINISHME***

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Check for RANK ERROR
    if (aplRankLft NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkLftArg);
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF















ERROR_EXIT:
    if (hGlbLft && lpMemLft)
    {
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (bRet)
        return &YYRes[YYResIndex];
    else
        return NULL;
} // End PrimFnDydEpsilon_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_Epsilon.c
//***************************************************************************
