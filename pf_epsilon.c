//***************************************************************************
//  NARS2000 -- Primitive Function -- Epsilon
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2008 Sudley Place Software

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
        goto SYNTAX_EXIT;

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
                       (ClrPtrTypeDirAsGlb (lptkRhtArg->tkData.tkSym->stData.stGlbData),        // HGLOBAL
                        lptkFunc);                                                              // Ptr to function token
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
                   (ClrPtrTypeDirAsGlb (lptkRhtArg->tkData.tkGlbData),  // HGLOBAL
                    lptkFunc);                                          // Ptr to function token
        defstop
            return NULL;
    } // End SWITCH

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
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
    APLUINT      ByteRes;       // # bytes in the result
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
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
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
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;
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
    APLSTYPE     aplTypeRes,        // Result storage type
                 aplTypePro;        // Prototype ...
    APLNELM      aplNELMRes;        // # elements in the result
    HGLOBAL      hGlbRes;           // Result global memory handle
    LPVOID       lpMemRes;          // Ptr to result global memory
    APLUINT      ByteRes;           // # bytes in the result
    UINT         uBitMask = 0x01,   // Bit mask for marching through Booleans
                 uBitIndex = 0;     // Bit index ...
    BOOL         bRet = TRUE;       // TRUE iff result is valid
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Traverse the array counting the # simple scalars
    //   and keeping track of the common storage type --
    //   it'll be simple homogeneous or heterogeneous.
    aplTypeRes = ARRAY_LIST;    // Initialize storage type
    aplNELMRes = 0;             // ...        count
    PrimFnMonEpsilonGlbCount (hGlbRht, &aplTypeRes, &aplNELMRes, &aplTypePro);

    // Handle empty result
    if (aplNELMRes EQ 0)
        aplTypeRes = aplTypePro;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, 1);

    // Allocate space for the result
    // N.B.: Conversion from APLUINT to UINT
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
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
                             ClrPtrTypeDirAsGlb (hGlbRht),
                             lptkFunc);
    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    return lpYYRes;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnMonEpsilonGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonEpsilonGlbCount
//
//  Count the # simple scalars in the global memory object
//***************************************************************************

void PrimFnMonEpsilonGlbCount
    (HGLOBAL    hGlbRht,                            // Right arg global memory handle
     LPAPLSTYPE lpaplTypeRes,                       // Ptr to result storage type
     LPAPLNELM  lpaplNELMRes,                       // Ptr to result NELM
     LPAPLSTYPE lpaplTypePro)                       // Ptr to prototype storage type

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
    // Get the Array Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // If the right arg is empty, return the

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
            } else
                *lpaplTypePro = aplTypeRht;

            break;

        case ARRAY_NESTED:
            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

            // Take prototypes into account
            aplNELMRht = max (aplNELMRht, 1);

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

                    PrimFnMonEpsilonGlbCount (ClrPtrTypeIndAsGlb (lpMemRht),
                                              lpaplTypeRes,
                                              lpaplNELMRes,
                                              lpaplTypePro);
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
    APLUINT    ByteRes;             // # bytes in the result
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
    if (IsSimpleAPA (aplTypeRht))
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
                                                     ClrPtrTypeIndAsGlb (lpMemRht),
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
                                                     ClrPtrTypeIndAsGlb (lpMemRht),
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
                                                     ClrPtrTypeIndAsGlb (lpMemRht),
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
                                                     ClrPtrTypeIndAsGlb (lpMemRht),
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
                                                     ClrPtrTypeIndAsGlb (lpMemRht),
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
    APLSTYPE     aplTypeLft,        // Left arg storage type
                 aplTypeRht;        // Right ...
    APLNELM      aplNELMLft,        // Left arg NELM
                 aplNELMRht;        // Right ...
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankRht;        // Right ...
    HGLOBAL      hGlbLft,           // Left arg global memory handle
                 hGlbRht,           // Right ...
                 hGlbRes;           // Result   ...
    LPVOID       lpMemLft,          // Ptr to left arg global memory
                 lpMemRht;          // Ptr to right ...
    LPAPLBOOL    lpMemRes = NULL;   // Ptr to result   ...
    APLUINT      ByteRes;           // # bytes in the result
    APLLONGEST   aplLongestLft,     // Left arg immediate value
                 aplLongestRht;     // Right ...
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
        goto SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    //***************************************************************
    // Calculate space needed for the result
    //***************************************************************
    ByteRes = CalcArraySize (ARRAY_BOOL, aplNELMLft, aplRankLft);

    //***************************************************************
    // Now we can allocate the storage for the result
    // N.B.:  Conversion from APLUINT to UINT.
    //***************************************************************
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_BOOL;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = 0;               // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMLft;
    lpHeader->Rank       = aplRankLft;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = (LPAPLBOOL) VarArrayBaseToDim (lpMemRes);

    // Fill in the result's dimension
    if (lpMemLft)
    {
        // Skip over the header to the dimensions
        lpMemLft = VarArrayBaseToDim (lpMemLft);

        // Copy the left arg dimensions to the result
        CopyMemory (lpMemRes, lpMemLft, (UINT) aplRankLft * sizeof (APLDIM));

        // Skip over the dimensions to the data
        lpMemRes = VarArrayDimToData (lpMemRes, aplRankLft);
        lpMemLft = VarArrayDimToData (lpMemLft, aplRankLft);
    } else
        // Point to the left arg immediate value
        lpMemLft = &aplLongestLft;

    if (lpMemRht)
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
    else
        // Point to the right arg immediate value
        lpMemRht = &aplLongestRht;

    // If the result is non-empty, and
    //    the right arg is non-empty, and
    //    the left and right args are not different types (numeric vs. char), ...
    if (!(aplNELMLft EQ 0
       || aplNELMRht EQ 0
       || (IsSimpleNum (aplTypeLft) && IsSimpleChar (aplTypeRht))
       || (IsSimpleNum (aplTypeRht) && IsSimpleChar (aplTypeLft))))
    {
        // Split cases based upon the left & right arg storage types
        if (IsSimpleBool (aplTypeLft) && IsSimpleBool (aplTypeRht))
            // Handle APLBOOL vs. APLBOOL
            PrimFnDydEpsilonBvB (lpMemRes,              // Ptr to result global memory data
                                 aplNELMLft,            // Left arg NELM
                                 lpMemLft,              // Ptr to left arg global memory data
                                 aplNELMRht,            // Right arg NELM
                                 lpMemRht);             // Ptr to right arg global memory data
        else
        if (IsSimpleInt (aplTypeLft) && IsSimpleInt (aplTypeRht))
        {
            // Handle APLBOOL/APLINT/APLAPA vs. APLBOOL/APLINT/APLAPA
            if (!PrimFnDydEpsilonIvI_EM (lpMemRes,      // Ptr to result global memory data
                                         aplTypeLft,    // Left arg storage type
                                         aplNELMLft,    // Left arg NELM
                                         lpMemLft,      // Ptr to left arg global memory data
                                         aplTypeRht,    // Right arg storage type
                                         aplNELMRht,    // Right arg NELM
                                         lpMemRht,      // Ptr to right arg global memory data
                                         lptkFunc))     // Ptr to function token
                goto ERROR_EXIT;
        } else
        if (IsSimpleChar (aplTypeLft) && IsSimpleChar (aplTypeRht))
        {
            // Handle APLCHAR vs. APLCHAR
            if (!PrimFnDydEpsilonCvC_EM (lpMemRes,      // Ptr to result global memory data
                                         aplNELMLft,    // Left arg NELM
                                         lpMemLft,      // Ptr to left arg global memory data
                                         aplNELMRht,    // Right arg NELM
                                         lpMemRht,      // Ptr to right arg global memory data
                                         lptkFunc))     // Ptr to function token
                goto ERROR_EXIT;
        } else
            // Handle all other combinations
            PrimFnDydEpsilonOther (lpMemRes,            // Ptr to result global memory data
                                   aplTypeLft,          // Left arg storage type
                                   aplNELMLft,          // Left arg NELM
                                   lpMemLft,            // Ptr to left arg global memory data
                                   aplTypeRht,          // Right arg storage type
                                   aplNELMRht,          // Right arg NELM
                                   lpMemRht,            // Ptr to right arg global memory data
                                   lptkFunc);           // Ptr to function token
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
    } // End IF
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
} // End PrimFnDydEpsilon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydEpsilonBvB
//
//  Dyadic epsilon between Boolean args
//***************************************************************************

void PrimFnDydEpsilonBvB
    (LPAPLBOOL lpMemRes,            // Ptr to result global memory data
     APLNELM   aplNELMLft,          // Left arg (and result) NELM
     LPAPLBOOL lpMemLft,            // Ptr to left arg global memory data
     APLNELM   aplNELMRht,          // Right arg NELM
     LPAPLBOOL lpMemRht)            // Ptr to right arg global memory data

{
    BOOL    Found0,                 // TRUE iff there is at least one 0 in the right arg
            Found1;                 // ...                            1 ...
    APLUINT BytesInLftBits,         // # bytes in the left arg data
            BytesInRhtBits,         // ...            right ...
            uLft,                   // Loop counter
            uRht;                   // Loop counter

    // The result is either all 0s              (if right arg is empty -- already ruled out), or
    //                      same as Lft         (if right arg is all 1s), or
    //                      complement of Lft   (if right arg is all 0s), or
    //                      all 1s              (if right arg has at least one 0 and one 1)

    // Calculate the # bytes in the left arg (result) data
    BytesInLftBits = RoundUpBits8 (aplNELMLft);

    // Calculate the # bytes in the right arg data
    BytesInRhtBits = RoundUpBits8 (aplNELMRht);

    // Search the right arg for a 0
    for (Found0 = FALSE, uRht = 0; uRht < (BytesInRhtBits - 1); uRht++)
    if (FastBoolTrans[lpMemRht[uRht]][0] < NBIB)
    {
        Found0 = TRUE;

        break;
    } // End FOR/IF

    // If we didn't find a 0, check the last byte (may be short)
    if (!Found0)
        Found0 = (FastBoolTrans[lpMemRht[uRht]][0] < (MASKLOG2NBIB & aplNELMRht));

    // Search the right arg for a 1
    for (Found1 = FALSE, uRht = 0; uRht < (BytesInRhtBits - 1); uRht++)
    if (FastBoolTrans[lpMemRht[uRht]][1] < NBIB)
    {
        Found1 = TRUE;

        break;
    } // End FOR/IF

    // If we didn't find a 1, check the last byte (may be short)
    if (!Found1)
        Found1 = (FastBoolTrans[lpMemRht[uRht]][1] < (MASKLOG2NBIB & aplNELMRht));

    // If we found both a 0 and a 1, the result is all 1s
    if (Found0 && Found1)
    {
        // Fill in all but the last byte
        FillMemory (lpMemRes, (UINT) BytesInLftBits - 1, 0xFF);

        // Handle the last byte specially
        lpMemRes[BytesInLftBits - 1] |= (BIT0 << (MASKLOG2NBIB & aplNELMLft)) - 1;
    } else
    // If we found only 0s, the result is the complement of the left arg
    if (Found0)
    {
        // Copy all but the last byte
        for (uLft = 0; uLft < (BytesInLftBits - 1); uLft++)
            *lpMemRes++ = ~*lpMemLft++;

        // Handle the last byte specially
        lpMemRes[BytesInLftBits - 1] |= ((BIT0 << (MASKLOG2NBIB & aplNELMLft)) - 1) & ~*lpMemLft;
    } else
    // If we found only 1s, the result is the same as the left arg
    if (Found1)
        CopyMemory (lpMemRes, lpMemLft, (UINT) BytesInLftBits);
} // End PrimFnDydEpsilonBvB


//***************************************************************************
//  $PrimFnDydEpsilonIvI_EM
//
//  Dyadic epsilon between simple ints (APLBOOL/APLINT/APLAPA)
//***************************************************************************

BOOL PrimFnDydEpsilonIvI_EM
    (LPAPLBOOL lpMemRes,            // Ptr to result global memory data
     APLSTYPE  aplTypeLft,          // Left arg storage type
     APLNELM   aplNELMLft,          // Left arg NELM
     LPVOID    lpMemLft,            // Ptr to left arg global memory data
     APLSTYPE  aplTypeRht,          // Right arg storage type
     APLNELM   aplNELMRht,          // Right arg NELM
     LPVOID    lpMemRht,            // Ptr to right arg global memory data
     LPTOKEN   lptkFunc)            // Ptr to function token

{
    APLUINT   uLft,                 // Loop counter
              uRht;                 // Loop counter
    APLINT    aplIntegerLft,        // Left arg integer
              aplIntegerRht;        // Right arg integer
    UINT      uBitIndex;            // Bit index for marching through Booleans

    // Here's where we might do something clever such as sort the right arg
    //   or perhaps hash it assuming one or both args are large enough
    //   to warrant such extra work.

    uBitIndex = 0;

    // For the moment, we'll do an outer product
    for (uLft = 0; uLft < aplNELMLft; uLft++)
    {
        aplIntegerLft =
          GetNextInteger (lpMemLft,
                          aplTypeLft,
                          uLft);
        // Loop through the right arg
        for (uRht = 0; uRht < aplNELMRht; uRht++)
        {
            aplIntegerRht =
              GetNextInteger (lpMemRht,
                              aplTypeRht,
                              uRht);
            if (aplIntegerLft EQ aplIntegerRht)
            {
                // Set the result bit
                *lpMemRes |= (BIT0 << uBitIndex);

                break;
            } // End IF
        } // End FOR

        // Check for end-of-byte
        if (++uBitIndex EQ NBIB)
        {
            uBitIndex = 0;              // Start over
            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
        } // End IF
    } // End FOR

    return TRUE;
} // End PrimFnDydEpsilonIvI_EM


//***************************************************************************
//  $PrimFnDydEpsilonCvC_EM
//
//  Dyadic epsilon between char args
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydEpsilonCvC_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydEpsilonCvC_EM
    (LPAPLBOOL lpMemRes,            // Ptr to result global memory data
     APLNELM   aplNELMLft,          // Left arg (and result) NELM
     LPAPLCHAR lpMemLft,            // Ptr to left arg global memory data
     APLNELM   aplNELMRht,          // Right arg NELM
     LPAPLCHAR lpMemRht,            // Ptr to right arg global memory data
     LPTOKEN   lptkFunc)            // Ptr to function token

{
    LPAPLBOOL lpMemTT;              // Translate table gloabl memory handle
    UINT      ByteTT;               // # bytes in the Translate Table
    APLUINT   uLft,                 // Loop counter
              uRht;                 // Loop counter
    APLCHAR   aplChar;              // Temporary char
    UINT      uBitIndex;            // Bit index for marching through Booleans

    // Calculate # bytes in the TT at one bit per 16-bit index (APLCHAR)
    ByteTT = (UINT) RoundUpBits8 (APLCHAR_SIZE);

    // Allocate space for a ByteTT Translate Table
    // Note that this allocation is GPTR (GMEM_FIXED | GMEM_ZEROINIT)
    //   because we'll use it quickly and then free it.
    lpMemTT = MyGlobalAlloc (GPTR, ByteTT);
    if (!lpMemTT)
        goto WSFULL_EXIT;

    // Trundle through the right arg marking the TT
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Get the APLCHAR from the right arg
        aplChar = *lpMemRht++;

        lpMemTT[aplChar >> LOG2NBIB] |= BIT0 << (MASKLOG2NBIB & aplChar);
    } // End FOR

    // Trundle through the left arg looking the chars in the TT
    //   and setting the result bit
    uBitIndex = 0;
    for (uLft = 0; uLft < aplNELMLft; uLft++)
    {
        // Get the APLCHAR from the left arg
        aplChar = *lpMemLft++;

        // If the char is in the TT, ...
        if (lpMemTT[aplChar >> LOG2NBIB] & (BIT0 << (MASKLOG2NBIB & aplChar)))
            *lpMemRes |= (BIT0 << uBitIndex);
        // Check for end-of-byte
        if (++uBitIndex EQ NBIB)
        {
            uBitIndex = 0;              // Start over
            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
        } // End IF
    } // End FOR

    // We no longer need this storage
    MyGlobalFree (lpMemTT); lpMemTT = NULL;

    return TRUE;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return FALSE;
} // End PrimFnDydEpsilonCvC_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydEpsilonOther
//
//  Dyadic epsilon between all other arg combinations
//***************************************************************************

void PrimFnDydEpsilonOther
    (LPAPLBOOL lpMemRes,            // Ptr to result global memory data
     APLSTYPE  aplTypeLft,          // Left arg storage type
     APLNELM   aplNELMLft,          // Left arg NELM
     LPVOID    lpMemLft,            // Ptr to left arg global memory data
     APLSTYPE  aplTypeRht,          // Right arg storage type
     APLNELM   aplNELMRht,          // Right arg NELM
     LPVOID    lpMemRht,            // Ptr to right arg global memory data
     LPTOKEN   lptkFunc)            // Ptr to function token

{
    HGLOBAL    hGlbSubLft,              // Left arg item global memory handle
               hGlbSubRht;              // Right ...
    APLLONGEST aplLongestSubLft,        // Left arg item immediate value
               aplLongestSubRht;        // Right ...
    IMM_TYPES  immTypeSubLft,           // Left arg item immediate type
               immTypeSubRht;           // Right ...
    UINT       uBitIndex;               // Bit index for marching through Booleans
    APLFLOAT   fQuadCT;                 // []CT
    APLUINT    uLft,                    // Loop counter
               uRht;                    // ...

    // This leaves:  Left vs. Right
    //                B   vs.    FHN
    //                I   vs.    FHN
    //                A   vs.    FHN
    //                F   vs. BIAFHN
    //                H   vs. BIAFHN
    //                N   vs. BIAFHN

    uBitIndex = 0;

    // Get the current value of []CT
    fQuadCT = GetQuadCT ();

    // Loop through the left arg
    for (uLft = 0; uLft < aplNELMLft; uLft++)
    {
        // Get the next value from the left arg
        GetNextValueMem (lpMemLft,              // Ptr to left arg global memory
                         aplTypeLft,            // Left arg storage type
                         uLft,                  // Left arg index
                        &hGlbSubLft,            // Left arg item global memory handle
                        &aplLongestSubLft,      // Ptr to left arg immediate value
                        &immTypeSubLft);        // Ptr to left arg immediate type
        // Loop through the right arg
        for (uRht = 0; uRht < aplNELMRht; uRht++)
        {
            // Get the next value from the right arg
            GetNextValueMem (lpMemRht,              // Ptr to right arg global memory
                             aplTypeRht,            // Right arg storage type
                             uRht,                  // Right arg index
                            &hGlbSubRht,            // Right arg item global memory handle
                            &aplLongestSubRht,      // Ptr to right arg immediate value
                            &immTypeSubRht);        // Ptr to right arg immediate type
            // If both items are globals, ...
            if ((hGlbSubLft NE NULL) && (hGlbSubRht NE NULL))
            {
                TOKEN tkSubLft = {0},       // Left arg item token
                      tkSubRht = {0};       // Right ...

                // Fill in the left arg item token
                tkSubLft.tkFlags.TknType   = TKT_VARARRAY;
////////////////tkSubLft.tkFlags.ImmType   = 0;     // Already zero from = {0}
////////////////tkSubLft.tkFlags.NoDisplay = 0;     // Already zero from = {0}
                tkSubLft.tkData.tkGlbData  = MakePtrTypeGlb (hGlbSubLft);
                tkSubLft.tkCharIndex       = lptkFunc->tkCharIndex;

                // Fill in the right arg item token
                tkSubRht.tkFlags.TknType   = TKT_VARARRAY;
////////////////tkSubRht.tkFlags.ImmType   = 0;     // Already zero from = {0}
////////////////tkSubRht.tkFlags.NoDisplay = 0;     // Already zero from = {0}
                tkSubRht.tkData.tkGlbData  = MakePtrTypeGlb (hGlbSubRht);
                tkSubRht.tkCharIndex       = lptkFunc->tkCharIndex;

                // Use match to determine equality
                if (PrimFnDydEqualUnderbar_EM_YY (&tkSubLft,        // Ptr to left arg token
                                                   lptkFunc,        // Ptr to function token
                                                  &tkSubRht,        // Ptr to right arg token
                                                   NULL))           // Ptr to axis token (may be NULL)
                    goto SET_RESULT_BIT;
            } else
            // If both items are simple, ...
            if ((hGlbSubLft EQ NULL) && (hGlbSubRht EQ NULL))
            {
                // If both items are APLCHARs, ...
                if (IsImmChr (immTypeSubLft) && IsImmChr (immTypeSubRht))
                {
                    // Compare the APLCHARs
                    if (((APLCHAR) aplLongestSubLft) EQ (APLCHAR) aplLongestSubRht)
                        goto SET_RESULT_BIT;
                } else
                // If both items are numeric, ...
                if (IsImmNum (immTypeSubLft) && IsImmNum (immTypeSubRht))
                {
                    // This leaves BIF vs. BIF

                    // Split cases based upon the left arg item immediate type
                    switch (immTypeSubLft)
                    {
                        case IMMTYPE_BOOL:              // Lft = BOOL
                            // Split cases based upon the right arg item immediate type
                            switch (immTypeSubRht)
                            {
                                case IMMTYPE_BOOL:      // Lft = BOOL, Rht = BOOL
                                    if (((APLBOOL) aplLongestSubLft) EQ (APLBOOL) aplLongestSubRht)
                                        goto SET_RESULT_BIT;
                                    break;

                                case IMMTYPE_INT:       // Lft = BOOL, Rht = INT
                                    if (((APLBOOL) aplLongestSubLft) EQ (APLINT)  aplLongestSubRht)
                                        goto SET_RESULT_BIT;
                                    break;

                                case IMMTYPE_FLOAT:     // Lft = BOOL, Rht = FLOAT
                                    if (CompareCT ((APLBOOL) aplLongestSubLft, *(LPAPLFLOAT) &aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_BIT;
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case IMMTYPE_INT:               // Lft = INT
                            // Split cases based upon the right arg item immediate type
                            switch (immTypeSubRht)
                            {
                                case IMMTYPE_BOOL:      // Lft = INT, Rht = BOOL
                                    if (((APLINT)  aplLongestSubLft) EQ (APLBOOL) aplLongestSubRht)
                                        goto SET_RESULT_BIT;
                                    break;

                                case IMMTYPE_INT:       // Lft = INT, Rht = INT
                                    if (((APLINT)  aplLongestSubLft) EQ (APLINT)  aplLongestSubRht)
                                        goto SET_RESULT_BIT;
                                    break;

                                case IMMTYPE_FLOAT:     // Lft = INT, Rht = FLOAT
                                    if (CompareCT ((APLFLOAT) (APLINT)  aplLongestSubLft, *(LPAPLFLOAT) &aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_BIT;
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        case IMMTYPE_FLOAT:             // Lft = FLOAT
                            // Split cases based upon the right arg item immediate type
                            switch (immTypeSubRht)
                            {
                                case IMMTYPE_BOOL:      // Lft = FLOAT, Rht = BOOL
                                    if (CompareCT (*(LPAPLFLOAT) &aplLongestSubLft, (APLBOOL) aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_BIT;
                                    break;

                                case IMMTYPE_INT:       // Lft = FLOAT, Rht = INT
                                    if (CompareCT (*(LPAPLFLOAT) &aplLongestSubLft, (APLFLOAT) (APLINT) aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_BIT;
                                    break;

                                case IMMTYPE_FLOAT:     // Lft = FLOAT, Rht = FLOAT
                                    if (CompareCT (*(LPAPLFLOAT) &aplLongestSubLft, *(LPAPLFLOAT) &aplLongestSubRht, fQuadCT, NULL))
                                        goto SET_RESULT_BIT;
                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End IF/ELSE/...
            } // End IF/ELSE/...

            continue;
SET_RESULT_BIT:
            // Set the result bit
            *lpMemRes |= (BIT0 << uBitIndex);

            break;
        } // End FOR

        // Check for end-of-byte
        if (++uBitIndex EQ NBIB)
        {
            uBitIndex = 0;      // Start over
            lpMemRes++;         // Skip to next byte
        } // End IF
    } // End FOR
} // End PrimFnDydEpsilonOther


//***************************************************************************
//  End of File: pf_epsilon.c
//***************************************************************************
