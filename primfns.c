//***************************************************************************
//  NARS2000 -- Primitive Functions
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
#include <float.h>
#include <math.h>
#include "headers.h"


//***************************************************************************
//  $IsTknFcnOpr
//
//  Return TRUE iff the given token is a function or operator
//***************************************************************************

UBOOL IsTknFcnOpr
    (LPTOKEN lptk)              // Ptr to token

{
    switch (TokenTypeFV (lptk))
    {
        case 'F':
        case '1':
        case '2':
        case '3':
            return TRUE;

        case 'V':
            return FALSE;

        defstop
            return 0;
    } // End SWITCH
} // End IsTknFcnOpr


//***************************************************************************
//  $TokenTypeFV
//
//  Return the type of a token
//
//  'F' = Function
//  'V' = Variable
//  '1' = Monadic operator
//  '2' = Dyadic operator
//  '3' = Ambiguous operator
//  '?' = None of the above
//
//***************************************************************************

char TokenTypeFV
    (LPTOKEN lptk)

{
    // Split cases based upon the token type
    switch (lptk->tkFlags.TknType)
    {
        case TKT_VARNAMED:
        case TKT_CHRSTRAND:
        case TKT_NUMSTRAND:
        case TKT_VARIMMED:
        case TKT_VARARRAY:
        case TKT_AXISIMMED:
        case TKT_AXISARRAY:
        case TKT_LISTINT:
        case TKT_STRAND:
            return 'V';

        case TKT_FCNIMMED:
        case TKT_FCNARRAY:
        case TKT_FCNNAMED:
        case TKT_FILLJOT:
            return 'F';

        case TKT_OP1IMMED:
        case TKT_OP1NAMED:
        case TKT_OPJOTDOT:
            return '1';

        case TKT_OP2IMMED:
        case TKT_OP2NAMED:
            return '2';

        case TKT_OP3IMMED:
        case TKT_OP3NAMED:
            return '3';

        case TKT_SYNTERR:
            return '?';

        case TKT_ASSIGN:
        case TKT_LISTSEP:
        case TKT_LABELSEP:
        case TKT_COLON:
        case TKT_LEFTPAREN:
        case TKT_RIGHTPAREN:
        case TKT_LEFTBRACKET:
        case TKT_RIGHTBRACKET:
        case TKT_LEFTBRACE:
        case TKT_RIGHTBRACE:
        case TKT_EOS:
        case TKT_EOL:
        case TKT_SOS:
        case TKT_LINECONT:
        case TKT_INPOUT:
        case TKT_LISTPAR:
        case TKT_LSTIMMED:
        case TKT_LSTARRAY:
        case TKT_LSTMULT:
        case TKT_STRNAMED:
        case TKT_CS_ANDIF:                  // Control structure:  ANDIF     (Data is Line/Stmt #)
        case TKT_CS_CASE:                   // ...                 CASE
        case TKT_CS_CASELIST:               // ...                 CASELIST
        case TKT_CS_CONTINUE:               // ...                 CONTINUE
        case TKT_CS_ELSE:                   // ...                 ELSE
        case TKT_CS_ELSEIF:                 // ...                 ELSEIF
        case TKT_CS_END:                    // ...                 END
        case TKT_CS_ENDFOR:                 // ...                 ENDFOR
        case TKT_CS_ENDFORLCL:              // ...                 ENDFORLCL
        case TKT_CS_ENDIF:                  // ...                 ENDIF
        case TKT_CS_ENDREPEAT:              // ...                 ENDREPEAT
        case TKT_CS_ENDSELECT:              // ...                 ENDSELECT
        case TKT_CS_ENDWHILE:               // ...                 ENDWHILE
        case TKT_CS_FOR:                    // ...                 FOR
        case TKT_CS_FOR2:                   // ...                 FOR2
        case TKT_CS_FORLCL:                 // ...                 FORLCL
        case TKT_CS_GOTO:                   // ...                 GOTO
        case TKT_CS_IF:                     // ...                 IF
        case TKT_CS_IF2:                    // ...                 IF2
        case TKT_CS_IN:                     // ...                 IN
        case TKT_CS_LEAVE:                  // ...                 LEAVE
        case TKT_CS_ORIF:                   // ...                 ORIF
        case TKT_CS_REPEAT:                 // ...                 REPEAT
        case TKT_CS_REPEAT2:                // ...                 REPEAT2
        case TKT_CS_RETURN:                 // ...                 RETURN
        case TKT_CS_SELECT:                 // ...                 SELECT
        case TKT_CS_SELECT2:                // ...                 SELECT2
        case TKT_CS_UNTIL:                  // ...                 UNTIL
        case TKT_CS_WHILE:                  // ...                 WHILE
        case TKT_CS_WHILE2:                 // ...                 WHILE2
        case TKT_CS_SKIPCASE:               // ...                 Special token
        case TKT_CS_SKIPEND:                // ...                 Special token
        case TKT_CS_NEC:                    // ...                 Special token
        case TKT_CS_EOL:                    // ...                 Special token
        defstop
            return '?';
    } // End SWITCH
} // End TokenTypeFV


//***************************************************************************
//  $RankOfGlb
//
//  Get the rank of a global memory object
//***************************************************************************

APLRANK RankOfGlb
    (HGLOBAL hGlb)

{
    LPVOID  lpMem;
    APLRANK aplRank;

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlb);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
    // Get the Rank
    aplRank = lpHeader->Rank;
#undef  lpHeader

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMem = NULL;

    return aplRank;
} // End RankOfGlb


//***************************************************************************
//  $AttrsOfToken
//
//  Return the attributes (Type, NELM, and Rank) of a token.
//***************************************************************************

void AttrsOfToken
    (LPTOKEN   lpToken,         // Ptr to token
     APLSTYPE *lpaplType,       // Ptr to token storage type (may be NULL)
     LPAPLNELM lpaplNELM,       // ...          NELM (may be NULL)
     LPAPLRANK lpaplRank,       // ...          rank (may be NULL)
     LPAPLDIM  lpaplCols)       // ...          # cols in the array (scalar = 1) (may be NULL)

{
    HGLOBAL hGlbData;           // Global memory handle

    // Split case based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                // Get the global handle
                hGlbData = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbData));

                break;      // Continue with HGLOBAL case
            } // End IF

            if (lpaplType)
                *lpaplType = TranslateImmTypeToArrayType (lpToken->tkData.tkSym->stFlags.ImmType);
            if (lpaplNELM)
                *lpaplNELM = 1;
            if (lpaplRank)
                *lpaplRank = 0;
            if (lpaplCols)
                *lpaplCols = 1;
            return;

        case TKT_VARIMMED:
        case TKT_LSTIMMED:
            if (lpaplType)
                *lpaplType = TranslateImmTypeToArrayType (lpToken->tkFlags.ImmType);
            if (lpaplNELM)
                *lpaplNELM = 1;
            if (lpaplRank)
                *lpaplRank = 0;
            if (lpaplCols)
                *lpaplCols = 1;
            return;

        case TKT_NUMSTRAND:
        case TKT_VARARRAY:
        case TKT_LISTPAR:
        case TKT_LSTARRAY:
            // Get the global handle
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbData));

            break;      // Continue with HGLOBAL case

        case TKT_LSTMULT:
            // Get the global handle
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL list array
            Assert (IsGlbTypeLstDir_PTB (hGlbData));

            break;      // Continue with HGLOBAL case

        defstop
            return;
    } // End SWITCH

    // Get the attributes (Type, NELM, and Rank) of the global
    AttrsOfGlb (hGlbData, lpaplType, lpaplNELM, lpaplRank, lpaplCols);
} // End AttrsOfToken


//***************************************************************************
//  $AttrsOfGlb
//
//  Return the attributes (Type, NELM, and Rank) of a global memory handle.
//***************************************************************************

void AttrsOfGlb
    (HGLOBAL   hGlbData,        // Memory handle
     APLSTYPE *lpaplType,       // Ptr to token storage type (may be NULL)
     LPAPLNELM lpaplNELM,       // ...          NELM (may be NULL)
     LPAPLRANK lpaplRank,       // ...          rank (may be NULL)
     LPAPLUINT lpaplCols)       // ...          # cols in the array (scalar = 1) (may be NULL)

{
    LPVOID  lpMemData;          // Ptr to global memory

    // Clear the ptr type bits in case set on the way in
    hGlbData = ClrPtrTypeDir (hGlbData);

    // Lock the memory to get a ptr to it
    lpMemData = MyGlobalLock (hGlbData);

    // Split cases based upon the array signature
    switch (GetSignatureMem (lpMemData))
    {
        case LSTARRAY_HEADER_SIGNATURE:
#define lpHeader    ((LPLSTARRAY_HEADER) lpMemData)
            if (lpaplType)
                *lpaplType = ARRAY_LIST;
            if (lpaplNELM)
                *lpaplNELM = lpHeader->NELM;
            if (lpaplRank)
                *lpaplRank = 1;
            if (lpaplCols)
                *lpaplCols = lpHeader->NELM;
#undef  lpHeader
            break;

        case VARARRAY_HEADER_SIGNATURE:
#define lpHeader    ((LPVARARRAY_HEADER) lpMemData)
            if (lpaplType)
                *lpaplType = lpHeader->ArrType;
            if (lpaplNELM)
                *lpaplNELM = lpHeader->NELM;
            if (lpaplRank)
                *lpaplRank = lpHeader->Rank;
            if (lpaplCols)
            {
                if (!IsScalar (lpHeader->Rank))
                    // Skip over the header to the dimensions
                    *lpaplCols = (VarArrayBaseToDim (lpMemData))[lpHeader->Rank - 1];
                else
                    *lpaplCols = 1;
            } // End IF
#undef  lpHeader
            break;

        case DFN_HEADER_SIGNATURE:
        case FCNARRAY_HEADER_SIGNATURE:
        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMemData = NULL;
} // End AttrsOfGlb


//***************************************************************************
//  $StorageType
//
//  Return the storage type for a dyadic scalar function
//    given the left and right storage types and the function.
//***************************************************************************

APLSTYPE StorageType
    (APLSTYPE aplTypeLft,   // Storage type of left arg
     LPTOKEN  lptkFunc,     // Immediate function
     APLSTYPE aplTypeRht)   // Storage type of right arg

{
    APLSTYPE aplTypeRes;

    // Both arguments are within bounds
    Assert (aplTypeLft < ARRAY_LENGTH);
    Assert (aplTypeRht < ARRAY_LENGTH);

    // tkData is an immediate function
    Assert (lptkFunc->tkFlags.TknType EQ TKT_FCNIMMED);

    // Get the common storage type
    aplTypeRes = aTypePromote[aplTypeLft][aplTypeRht];

    // Split cases based upon the result type
    switch (aplTypeRes)
    {
        case ARRAY_ERROR:
            return aplTypeRes;

        case ARRAY_HETERO:
            // A mixed result is from CHAR or HETERO vs. numeric (BOOL, INT, APA, FLOAT) or NESTED
            // The result is BOOL or NESTED if the function is equal or not-equal, otherwise
            //   the result is ERROR.
            if (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
             || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL)
            {
                if (IsNested (aplTypeLft)
                 || IsNested (aplTypeRht))
                    return ARRAY_NESTED;
                else
                    return ARRAY_BOOL;
            } else
                return ARRAY_ERROR;

        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
            if (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
             || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL)
                return ARRAY_BOOL;
            else
                return aplTypeRes;

        case ARRAY_NESTED:
            return aplTypeRes;

        case ARRAY_CHAR:
            if (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
             || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL)
                return ARRAY_BOOL;
            else
                return ARRAY_ERROR;

        case ARRAY_APA:
        defstop
            return ARRAY_ERROR;
    } // End SWITCH
} // StorageType


//***************************************************************************
//  $IncrOdometer
//
//  Increment the odometer in lpMemOdo subject to the
//    limits in lpMemDim[lpMemAxis] or lpMemDim, all
//    of length <aplRank>.
//***************************************************************************

void IncrOdometer
    (LPAPLUINT lpMemOdo,    // Ptr to the values to increment
     LPAPLDIM  lpMemDim,    // Ptr to the limits on each dimension
     LPAPLINT  lpMemAxis,   // Ptr to optional index vector (may be NULL)
     APLRANK   aplRank)     // # elements to which each of the
                            //   ptrs point

{
    APLINT iOdo;

    // Loop through the odometer values from right to left
    // Note we use a signed index variable because we're
    //   walking backwards and the test against zero must be
    //   made as a signed variable.
    if (lpMemAxis)
    {
        for (iOdo = aplRank - 1; iOdo >= 0; iOdo--)
        if (++lpMemOdo[iOdo] EQ lpMemDim[lpMemAxis[iOdo]])
            lpMemOdo[iOdo] = 0;
        else
            break;
    } else
    {
        for (iOdo = aplRank - 1; iOdo >= 0; iOdo--)
        if (++lpMemOdo[iOdo] EQ lpMemDim[iOdo])
            lpMemOdo[iOdo] = 0;
        else
            break;
    } // End IF/ELSE
} // End IncrOdometer


//***************************************************************************
//  $PrimScalarFnDydAllocate_EM
//
//  Allocate space for result of a primitive scalar dyadic function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimScalarFnDydAllocate_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimScalarFnDydAllocate_EM
    (LPTOKEN    lptkFunc,       // Ptr to function token

     HGLOBAL   *lphGlbRes,      // Return HGLOBAL of the result

     LPVOID     lpMemLft,       // Ptr to left arg memory (may be NULL if monadic)
     LPVOID     lpMemRht,       // ...    right ...

     APLRANK    aplRankLft,     // Left arg rank
     APLRANK    aplRankRht,     // Right ...
     LPAPLRANK  lpaplRankRes,   // Ptr to result rank

     APLSTYPE   aplTypeRes,     // Result type

     APLNELM    aplNELMLft,     // Left arg NELM
     APLNELM    aplNELMRht,     // Right ...
     APLNELM    aplNELMRes)     // Result ...

{
    APLUINT  ByteRes;           // # bytes in the result
    LPVOID   lpMemRes;          // Ptr to locked memory in result
    LPAPLDIM lpMemDimArg;
    APLUINT  uRes;
    LPVOID   lpMem;

    // Initialize the result
    *lphGlbRes = NULL;

    // Split cases based upon the singleton status
    //   of the left & right args
    if (IsSingleton (aplNELMLft)
     && IsSingleton (aplNELMRht))
    {
        //***************************************************************
        // Both args are singletons
        //***************************************************************

        // The rank of the result is the larger of the two args
        *lpaplRankRes = max (aplRankLft, aplRankRht);

        // If it's a simple numeric scalar, it'll
        //   be stored as an immediate token,
        //   not global memory.
        if (IsScalar (*lpaplRankRes)
         && IsSimpleNum (aplTypeRes))
            return TRUE;
    } else
    if (IsSingleton (aplNELMLft)
     || IsSingleton (aplNELMRht))
    {
        //***************************************************************
        // One of the args is a singleton, the other not
        //***************************************************************

        // The rank of the result is the rank of the non-singleton
        if (!IsSingleton (aplNELMLft))
            *lpaplRankRes = aplRankLft;
        else
            *lpaplRankRes = aplRankRht;
    } else
    {
        //***************************************************************
        // Neither arg is a singleton
        //***************************************************************

        // The rank of the result is the larger of the two args
        *lpaplRankRes = max (aplRankLft, aplRankRht);
    } // End IF/ELSE/...

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, *lpaplRankRes);

    // Allocate space for the result.
    // N.B. Conversion from APLUINT to UINT.
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;
    *lphGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!*lphGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (*lphGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = *lpaplRankRes;
#undef  lpHeader

    // Fill in the dimensions
    if (IsSingleton (aplNELMLft)
     && IsSingleton (aplNELMRht))
    {
        // Fill in the dimensions (all 1)
        for (uRes = 0; uRes < *lpaplRankRes; uRes++)
            (VarArrayBaseToDim (lpMemRes))[uRes] = 1;
    } else
    if (IsSingleton (aplNELMLft)
     || IsSingleton (aplNELMRht))
    {
        // Copy the ptr of the non-singleton argument
        if (!IsSingleton (aplNELMLft))
            lpMemDimArg = lpMemLft;
        else
            lpMemDimArg = lpMemRht;

        // Copy the dimensions from the non-singleton arg
        //   to the result
        for (uRes = 0; uRes < *lpaplRankRes; uRes++)
            (VarArrayBaseToDim (lpMemRes))[uRes] = (VarArrayBaseToDim (lpMemDimArg))[uRes];
    } else
    {
        // Get the higher rank's pointer
        //   to copy its dimensions to the result
        if (aplRankLft > aplRankRht)
            lpMemDimArg = lpMemLft;
        else
            lpMemDimArg = lpMemRht;

        // Copy the dimensions to the result
        for (uRes = 0; uRes < *lpaplRankRes; uRes++)
            (VarArrayBaseToDim (lpMemRes))[uRes] = (VarArrayBaseToDim (lpMemDimArg))[uRes];
    } // End IF/ELSE/...

    // Fill nested result with PTR_REUSED
    //   in case we fail part way through
    if (IsNested (aplTypeRes))
    {
        // Skip over the header and dimensions to the data
        lpMem = VarArrayBaseToData (lpMemRes, *lpaplRankRes);

        // Fill in the prototype
        *((LPAPLNESTED) lpMem) = PTR_REUSED;
        for (uRes = 1; uRes < aplNELMRes; uRes++)
            ((LPAPLNESTED) lpMem)[uRes] = PTR_REUSED;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;

    return TRUE;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return FALSE;
} // End PrimScalarFnDydAllocate_EM
#undef  APPEND_NAME


//***************************************************************************
//  $MakeMonPrototype_EM_PTB
//
//  Make a prototype copy of a single global memory object
//    whose value is sensitive to Ptr Type Bits.
//  The result might be a LPSYMENTRY or HGLOBAL
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeMonPrototype_EM_PTB"
#else
#define APPEND_NAME
#endif

HGLOBAL MakeMonPrototype_EM_PTB
    (HGLOBAL    hGlbArr,            // Incoming array handle
     LPTOKEN    lptkFunc,           // Ptr to function token
     MAKE_PROTO mpEnum)             // See MAKE_PROTO

{
    HGLOBAL      hGlbTmp,           // Temporary global memory handle
                 hSymGlbProto;      // Prototype ...
    LPVOID       lpMemArr = NULL,   // Ptr to array global memory
                 lpMemTmp = NULL;   // Ptr to tmp global memory
    LPVARARRAY_HEADER lpHeader;
    APLSTYPE     aplType;
    APLNELM      aplNELM;
    APLRANK      aplRank;
    UINT         u;
    APLNELM      uLen;
    UBOOL        bRet = TRUE;       // TRUE iff result is valid
    APLUINT      ByteRes;           // # bytes in the result
    LPSYMENTRY   lpSymArr,
                 lpSymRes;
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    DBGENTER;

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Split cases based upon the ptr type
    switch (GetPtrTypeDir (hGlbArr))
    {
        case PTRTYPE_STCONST:
            // If it's numeric, ...
            if (IsImmNum (((LPSYMENTRY) hGlbArr)->stFlags.ImmType))
                return lpMemPTD->steZero;
            else
                return lpMemPTD->steBlank;

        case PTRTYPE_HGLOBAL:
            break;

        defstop
            break;
    } // End SWITCH

    // Make a copy of the array as we're changing it
    hGlbArr = CopyArray_EM (hGlbArr,
                            lptkFunc);
    if (!hGlbArr)
        return NULL;

    // Lock the memory to get a ptr to it
    lpMemArr = lpHeader = MyGlobalLock (hGlbArr);

    aplType = lpHeader->ArrType;
    aplNELM = lpHeader->NELM;
    aplRank = lpHeader->Rank;

    // Point to the data
    lpMemArr = VarArrayBaseToData (lpMemArr, aplRank);

    // Split cases based upon the array type
    switch (aplType)
    {
        case ARRAY_BOOL:
            // Calculate # bytes of Boolean data
            uLen = RoundUpBitsToBytes (aplNELM);

            // Zero the memory
            ZeroMemory (lpMemArr, (APLU3264) uLen);

            break;

        case ARRAY_INT:
            // Zero the memory
            ZeroMemory (lpMemArr, (APLU3264) aplNELM * sizeof (APLINT));

            break;

        case ARRAY_FLOAT:
            // Zero the memory
            ZeroMemory (lpMemArr, (APLU3264) aplNELM * sizeof (APLFLOAT));

            break;

        case ARRAY_CHAR:
            // Split cases based upon mpEnum
            switch (mpEnum)
            {
                case MP_CHARS:
                    break;

                case MP_NUMONLY:
                    // If the arg is non-empty, that's an error
                    if (!IsEmpty (aplNELM))
                        goto DOMAIN_ERROR_EXIT;

                    // Change the storage type to Boolean
                    lpHeader->ArrType = ARRAY_BOOL;

                    break;

                case MP_NUMCONV:
                    // Convert the chars to numerics by allocating
                    //   new (Boolean) storage and copying over
                    //   the dimensions

                    // Calculate space needed for the result
                    ByteRes = CalcArraySize (ARRAY_BOOL, aplNELM, aplRank);

                    // Allocate space for the result.
                    // N.B. Conversion from APLUINT to UINT.
                    if (ByteRes NE (APLU3264) ByteRes)
                        goto WSFULL_EXIT;
                    hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                    if (!hGlbTmp)
                        goto WSFULL_EXIT;

                    // Lock the memory to get a ptr to it
                    lpMemTmp = MyGlobalLock (hGlbTmp);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemTmp)
                    // Fill in the header
                    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                    lpHeader->ArrType    = ARRAY_BOOL;
////////////////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////////////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
                    lpHeader->RefCnt     = 1;
                    lpHeader->NELM       = aplNELM;
                    lpHeader->Rank       = aplRank;
#undef  lpHeader

                    // Copy the dimensions to the result
                    CopyMemory (VarArrayBaseToDim (lpMemTmp),
                                VarArrayBaseToDim (lpHeader),
                                (APLU3264) aplRank * sizeof (APLDIM));

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbTmp); lpMemTmp = NULL;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbArr); lpMemArr = NULL;

                    // We no longer need this storage
                    FreeResultGlobalVar (hGlbArr); hGlbArr = NULL;

                    // Copy the global handle
                    hGlbArr = hGlbTmp;

                    goto NORMAL_EXIT;

                defstop
                    break;
            } // End SWITCH

            // Convert the chars to blanks
            Assert (aplNELM EQ (APLU3264) aplNELM);
            FillMemoryW (lpMemArr, (APLU3264) aplNELM, L' ');

            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemArr)
            // To make a prototype APA, set its
            //   offset and multiplier to zero
            lpAPA->Off =
            lpAPA->Mul = 0;
#undef  lpAPA
            break;

        case ARRAY_NESTED:
            // Empty nested arrays have one element (the prototype)
            aplNELM = max (aplNELM, 1);

            // Fall through to common HETERO/NESTED code

        case ARRAY_HETERO:
            for (u = 0; bRet && u < aplNELM; u++)
            // Split cases based upon the element's ptr type
            switch (GetPtrTypeInd (lpMemArr))
            {
                case PTRTYPE_STCONST:
                    lpSymArr = *(LPAPLHETERO) lpMemArr;

                    // It's an immediate
                    Assert (lpSymArr->stFlags.Imm);

                    // Split cases based upon the immediate type
                    switch (lpSymArr->stFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:
                        case IMMTYPE_INT:
                        case IMMTYPE_FLOAT:
                            lpSymRes = GetSteZero ();

                            break;

                        case IMMTYPE_CHAR:
                            // Split cases based upon mpEnum
                            switch (mpEnum)
                            {
                                case MP_CHARS:
                                    lpSymRes = lpMemPTD->steBlank;

                                    break;

                                case MP_NUMONLY:
                                    goto DOMAIN_ERROR_EXIT;

                                    break;

                                case MP_NUMCONV:
                                    lpSymRes = lpMemPTD->steZero;

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        defstop
                            goto ERROR_EXIT;
                    } // End SWITCH

                    if (lpSymRes)
                        // Save back into array
                        *((LPAPLHETERO) lpMemArr)++ = lpSymRes;
                    else
                        goto SYMTAB_ERROR_EXIT;
                    break;

                case PTRTYPE_HGLOBAL:
                    Assert (IsNested (aplType));

                    // It's a valid HGLOBAL array
                    Assert (IsGlbTypeVarInd_PTB (lpMemArr));

                    hSymGlbProto =
                      MakeMonPrototype_EM_PTB (*(LPAPLNESTED) lpMemArr, // Proto arg handle
                                               lptkFunc,                // Ptr to function token
                                               mpEnum);                 // Pass flag through
                    if (hSymGlbProto)
                    {
                        // We no longer need this storage
                        FreeResultGlobalVar (ClrPtrTypeInd (lpMemArr)); *((LPAPLNESTED) lpMemArr) = NULL;

                        *((LPAPLNESTED) lpMemArr)++ = hSymGlbProto;
                    } else
                        bRet = FALSE;
                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

DOMAIN_ERROR_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

SYMTAB_ERROR_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYMBOL_TABLE_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    bRet = FALSE;
NORMAL_EXIT:
    if (hGlbArr && lpMemArr)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArr); lpMemArr = NULL;
    } // End IF

    if (!bRet)
    {
        // We no longer need this storage
        FreeResultGlobalVar (hGlbArr); hGlbArr = NULL;
    } // End IF

    DBGLEAVE;

    return MakePtrTypeGlb (hGlbArr);
} // End MakeMonPrototype_EM_PTB
#undef  APPEND_NAME


//***************************************************************************
//  $MakeDydPrototype_EM_PTB
//
//  Make a prototype from a dyadic scalar function between
//    two global memory objects whose values are sensitive
//    to Ptr Type Bits.
//
//  Instead of returning one of the args as the result,
//    we create a result by trundling through the args
//    using scalar extension as well as checking for
//    RANK, LENGTH, and DOMAIN ERRORs between the args.
//
//  For example, adding empty vectors whose prototypes are
//    0 (0 0) and (0 0) 0 should produce an empty vector
//    whose prototype is (0 0) (0 0) by scalar extension.
//  Also, adding empty vectors whose prototypes are
//    (0 0) and (0 0 0) should produce a LENGTH ERROR.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeDydPrototype_EM_PTB"
#else
#define APPEND_NAME
#endif

HGLOBAL MakeDydPrototype_EM_PTB
    (HGLOBAL   hGlbLft,                 // Left arg global memory handle (may be NULL if immediate)
     IMM_TYPES immTypeLft,              // Left arg storage type if immediate (see IMM_TYPES)
     LPTOKEN   lptkFunc,                // Ptr to function token
     HGLOBAL   hGlbRht,                 // Right arg global memory handle (may be NULL if immediate)
     IMM_TYPES immTypeRht,              // Right arg storage type if immediate (see IMM_TYPES)
     LPTOKEN   lptkAxis)                // Ptr to axis token (may be NULL)

{
    APLSTYPE    aplTypeLft,             // Left arg storage type
                aplTypeRht,             // Right ...
                aplTypeRes;             // Result ...
    APLNELM     aplNELMLft,             // Left arg NELM
                aplNELMRht,             // Right ...
                aplNELMRes,             // Result   ...
                aplNELMAxis;            // Axis value ...
    APLRANK     aplRankLft,             // Left arg rank
                aplRankRht,             // Right ...
                aplRankRes;             // Result ...
    HGLOBAL     hGlbRes = NULL,         // Result global memory handle
                hGlbAxis = NULL,        // Axis value ...
                hGlbWVec = NULL,        // Weighting vector ...
                hGlbOdo = NULL,         // Odometer ...
                hGlbSub;                // Subarray ...
    LPAPLNESTED lpMemLft = NULL,        // Ptr to left arg global memory
                lpMemRht = NULL,        // Ptr to right ...
                lpMemRes = NULL;        // Ptr to result   ...
    LPAPLDIM    lpMemDimRes;            // Ptr to result dimensions
    APLUINT     ByteRes,                // # bytes in the result
                uLft,                   // Loop counter
                uRht,                   // Loop counter
                uRes;                   // Loop counter
    APLINT      iDim;                   // Dimension loop counter
    UBOOL       bBoolFn;                // TRUE iff the function is equal or not-equal
    LPPRIMSPEC  lpPrimSpec;             // Ptr to local PRIMSPEC
    LPAPLUINT   lpMemAxisHead = NULL,   // Ptr to axis values, fleshed out
                lpMemAxisTail = NULL,   // Ptr to grade up of AxisHead
                lpMemOdo = NULL,        // Ptr to odometer global memory
                lpMemWVec = NULL;       // Ptr to weighting vector ...

    // Get the attributes (Type, NELM, and Rank)
    //   of the left arg
    if (hGlbLft)        // If not immediate, ...
        AttrsOfGlb (hGlbLft, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    else
    {
        aplTypeLft = TranslateImmTypeToArrayType (immTypeLft);
        aplNELMLft = 1;
        aplRankLft = 0;
    } // End IF

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    if (hGlbRht)        // If not immediate, ...
        AttrsOfGlb (hGlbRht, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);
    else
    {
        aplTypeRht = TranslateImmTypeToArrayType (immTypeRht);
        aplNELMRht = 1;
        aplRankRht = 0;
    } // End IF

    // Handle scalar extension with empty other arg
    if (IsEmpty (aplNELMLft)
     && hGlbRht EQ NULL)
        aplTypeRht = ARRAY_BOOL;
    if (IsEmpty (aplNELMRht)
     && hGlbLft EQ NULL)
        aplTypeLft = ARRAY_BOOL;

    // The rank of the result is the larger of the two args
    //   unless one is a singleton.
    if ( IsSingleton (aplNELMLft) && !IsSingleton (aplNELMRht) && aplRankLft > aplRankRht)
        aplRankRes = aplRankRht;
    else
    if (!IsSingleton (aplNELMLft) &&  IsSingleton (aplNELMRht) && aplRankLft < aplRankRht)
        aplRankRes = aplRankLft;
    else
        aplRankRes = max (aplRankLft, aplRankRht);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Check the axis values, fill in # elements in axis
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRes,      // All values less than this
                           FALSE,           // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // TRUE iff fractional values allowed
                           NULL,            // Return last axis value
                          &aplNELMAxis,     // Return # elements in axis vector
                          &hGlbAxis))       // Return HGLOBAL with APLINT axis values
            return NULL;

        // Lock the memory to get a ptr to it
        lpMemAxisHead = MyGlobalLock (hGlbAxis);

        // Get pointer to the axis tail (where the [X] values are)
        lpMemAxisTail = &lpMemAxisHead[aplRankRes - aplNELMAxis];
    } else
        // No axis is the same as all axes
        aplNELMAxis = aplRankRes;

    // Get the corresponding lpPrimSpec
    lpPrimSpec = PrimSpecTab[SymTrans (lptkFunc)];

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                               &aplTypeLft,
                                                lptkFunc,
                                                aplNELMRht,
                                               &aplTypeRht);
    if (aplTypeRes EQ ARRAY_ERROR)
        goto DOMAIN_EXIT;

    Assert (IsSimpleNum (aplTypeRes)
         || IsNested (aplTypeRes));

    // Lock the memory to get a ptr to it
    if (hGlbLft)
        lpMemLft = MyGlobalLock (ClrPtrTypeDir (hGlbLft));

    // Lock the memory to get a ptr to it
    if (hGlbRht)
        lpMemRht = MyGlobalLock (ClrPtrTypeDir (hGlbRht));

    // Check for RANK and LENGTH ERRORs
    if (!CheckRankLengthError_EM (aplRankRes,
                                  aplRankLft,
                                  aplNELMLft,
                                  lpMemLft,
                                  aplRankRht,
                                  aplNELMRht,
                                  lpMemRht,
                                  aplNELMAxis,
                                  lpMemAxisTail,
                                  lptkFunc))
        goto ERROR_EXIT;

    // The NELM of the result is the larger of the two args
    //   unless one is empty
    if (IsEmpty (aplNELMLft) || IsEmpty (aplNELMRht))
        aplNELMRes = 0;
    else
        aplNELMRes = max (aplNELMLft, aplNELMRht);

    // Handle APA result separately
    if (IsSimpleAPA (aplTypeRes))
    {
        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

        // Allocate space for the result.
        // N.B. Conversion from APLUINT to UINT.
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;
        hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (!hGlbRes)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = aplTypeRes;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMRes;
        lpHeader->Rank       = aplRankRes;
#undef  lpHeader

        // Copy the dimensions to the result
        if (aplRankRes EQ aplRankLft)
            CopyMemory (VarArrayBaseToDim (lpMemRes),
                        VarArrayBaseToDim (lpMemLft),
                        (APLU3264) aplNELMRes * sizeof (APLDIM));
        else
            CopyMemory (VarArrayBaseToDim (lpMemRes),
                        VarArrayBaseToDim (lpMemRht),
                        (APLU3264) aplNELMRes * sizeof (APLDIM));
        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

#define lpAPA       ((LPAPLAPA) lpMemRes)
        // To make a prototype APA, set its
        //   offset and multiplier to zero
        lpAPA->Off =
        lpAPA->Mul = 0;
#undef  lpAPA

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

        // Set the ptr type bits
        hGlbRes = MakePtrTypeGlb (hGlbRes);

        goto NORMAL_EXIT;
    } // End IF

    // If the result is simple numeric,
    //   convert it to Boolean
    if (IsSimpleNum (aplTypeRes))
        aplTypeRes = ARRAY_BOOL;

    // Get the function type
    bBoolFn = lptkFunc->tkData.tkChar EQ UTF16_EQUAL
           || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL;

    // If the left arg is immediate, the result is the prototype of the other arg
    if (hGlbLft EQ NULL)
        hGlbRes =
          MakeMonPrototype_EM_PTB (hGlbRht,     // Proto arg handle
                                   lptkFunc,    // Ptr to function token
                                   bBoolFn ? MP_NUMCONV : MP_NUMONLY);
    else
    if (hGlbRht EQ NULL)
        hGlbRes =
          MakeMonPrototype_EM_PTB (hGlbLft,     // Proto arg handle
                                   lptkFunc,    // Ptr to function token
                                   bBoolFn ? MP_NUMCONV : MP_NUMONLY);
    else
    {
        // Neither arg is immediate

        // Allocate space for result
        if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                        &hGlbRes,
                                         lpMemLft,
                                         lpMemRht,
                                         aplRankLft,
                                         aplRankRht,
                                        &aplRankRes,
                                         aplTypeRes,
                                         aplNELMLft,
                                         aplNELMRht,
                                         aplNELMRes))
            goto ERROR_EXIT;

        // Take into account nested prototypes
        if (IsNested (aplTypeLft))
            aplNELMLft = max (aplNELMLft, 1);
        if (IsNested (aplTypeRht))
            aplNELMRht = max (aplNELMRht, 1);
        if (IsNested (aplTypeRes))
            aplNELMRes = max (aplNELMRes, 1);

        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbRes);      // ***FIXME*** -- Why???

        // Skip over the header to the dimensions
        lpMemDimRes = VarArrayBaseToDim (lpMemRes);

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

        // If either arg is not simple, loop through the result
        // Otherwise, the result is all zero (already filled in by GHND).
        if (!IsSimple (aplTypeLft)
         || !IsSimple (aplTypeRht))
        {
            // Handle axis if present
            if (aplNELMAxis NE aplRankRes)
            {
                //***************************************************************
                // Allocate space for the weighting vector which is
                //   {times}{backscan}1{drop}({rho}Z),1
                // N.B.  Conversion from APLUINT to UINT.
                //***************************************************************
                ByteRes = aplRankRes * sizeof (APLUINT);
                if (ByteRes NE (APLU3264) ByteRes)
                    goto WSFULL_EXIT;
                hGlbWVec = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                if (!hGlbWVec)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemWVec = MyGlobalLock (hGlbWVec);

                // Loop through the dimensions of the result in reverse
                //   order {backscan} and compute the cumulative product
                //   (weighting vector).
                // Note we use a signed index variable because we're
                //   walking backwards and the test against zero must be
                //   made as a signed variable.
                for (uRes = 1, iDim = aplRankRes - 1; iDim >= 0; iDim--)
                {
                    lpMemWVec[iDim] = uRes;
                    uRes *= lpMemDimRes[iDim];
                } // End FOR

                //***************************************************************
                // Allocate space for the odometer array, one value per dimension
                //   in the right arg, with values initially all zero (thanks to GHND).
                // N.B.  Conversion from APLUINT to UINT.
                //***************************************************************
                ByteRes = aplRankRes * sizeof (APLUINT);
                if (ByteRes NE (APLU3264) ByteRes)
                    goto WSFULL_EXIT;
                hGlbOdo = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                if (!hGlbOdo)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemOdo = MyGlobalLock (hGlbOdo);
            } // End IF

            // Loop through the elements of the result
            for (uRes = 0; uRes < aplNELMRes; uRes++)
            {
                // If there's an axis, ...
                if (lptkAxis
                 && aplNELMAxis NE aplRankRes)
                {
                    APLUINT uArg;       // Loop counter

                    // Loop through the odometer values accumulating
                    //   the weighted sum
                    for (uArg = 0, uRht = aplRankRes - aplNELMAxis; uRht < aplRankRes; uRht++)
                        uArg += lpMemOdo[lpMemAxisHead[uRht]] * lpMemWVec[uRht];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRes
                    IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

                    // Use the just computed index for the argument
                    //   with the smaller rank
                    if (aplRankLft < aplRankRht)
                    {
                        uLft = uArg;
                        uRht = uRes;
                    } else
                    {
                        uRht = uArg;
                        uLft = uRes;
                    } // End IF/ELSE
                } else
                {
                    // Handle 0 modulus as C fails miserably
                    uLft = mod64 (uRes, aplNELMLft);
                    uRht = mod64 (uRes, aplNELMRht);
                } // End IF/ELSE

                // If the left arg element is simple or an STE,
                //   the result element is the prototype
                //   of the right arg element
                if (IsSimple (aplTypeLft)
                 || GetPtrTypeDir (lpMemLft[uLft]) EQ PTRTYPE_STCONST)
                {
                    hGlbSub =
                      MakeMonPrototype_EM_PTB (lpMemRht[uRht],                  // Proto arg handle
                                               lptkFunc,                        // Ptr to function token
                                               bBoolFn ? MP_NUMCONV : MP_NUMONLY);
                    if (!hGlbSub)
                        goto ERROR_EXIT;
                    *lpMemRes++ = hGlbSub;
                } else
                // If the right arg element is simple or an STE,
                //   the result element is the prototype
                //   of the left arg element
                if (IsSimple (aplTypeRht)
                 || GetPtrTypeDir (lpMemRht[uRht]) EQ PTRTYPE_STCONST)
                {
                    hGlbSub =
                      MakeMonPrototype_EM_PTB (lpMemLft[uLft],                  // Proto arg handle
                                               lptkFunc,                        // Ptr to function token
                                               bBoolFn ? MP_NUMCONV : MP_NUMONLY);
                    if (!hGlbSub)
                        goto ERROR_EXIT;
                    *lpMemRes++ = hGlbSub;
                } else
                {
                    // Both args are nested HGLOBALs
                    hGlbSub =
                      MakeDydPrototype_EM_PTB (lpMemLft[uLft],  // Left arg proto handle
                                               0,               // Left arg immediate type (irrelevant as it's an HGLOBAL)
                                               lptkFunc,        // Ptr to function token
                                               lpMemRht[uRht],  // Right arg proto handle
                                               0,               // Right arg immediate type (irrelevant as it's an HGLOBAL)
                                               NULL);           // Ptr to axis token (may be NULL)
                    if (!hGlbSub)
                        goto ERROR_EXIT;
                    *lpMemRes++ = hGlbSub;
                } // End IF/ELSE/...
            } // End FOR
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

        // Set the ptr type bits
        hGlbRes = MakePtrTypeGlb (hGlbRes);
    } // End IF/ELSE/...

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
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
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (ClrPtrTypeDir (hGlbRht)); lpMemRht = NULL;
    } // End IF

    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (ClrPtrTypeDir (hGlbLft)); lpMemLft = NULL;
    } // End IF

    if (hGlbWVec)
    {
        if (lpMemWVec)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbWVec); lpMemWVec = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbWVec); hGlbWVec = NULL;
    } // End IF

    if (hGlbOdo)
    {
        if (lpMemOdo)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbOdo); lpMemOdo = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbOdo); hGlbOdo = NULL;
    } // End IF

    if (hGlbAxis)
    {
        if (lpMemAxisHead)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbAxis); lpMemAxisHead = lpMemAxisTail = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbAxis); hGlbAxis = NULL;
    } // End IF

    return hGlbRes;
} // End MakeDydPrototype_EM_PTB
#undef  APPEND_NAME


//***************************************************************************
//  $IsFirstSimpleGlb
//
//  Is the first element of a global simple?
//***************************************************************************

UBOOL IsFirstSimpleGlb
    (HGLOBAL *lphGlbRht,
     LPCHAR   lpaplTypeRes)

{
    LPVOID     lpMemRht;
    APLSTYPE   aplTypeRht;
    APLRANK    aplRankRht;
    UBOOL      bRet = TRUE;
    LPSYMENTRY lpSym;
    HGLOBAL    hGlbFirst;

    // It's a valid HGLOBAL array
    Assert (IsGlbTypeVarDir_PTB (*lphGlbRht));

    *lphGlbRht = ClrPtrTypeDir (*lphGlbRht);

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (*lphGlbRht);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRht)
    // Get the Array Type and Rank
    aplTypeRht = lpHeader->ArrType;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // It's a nested element
    Assert (IsNested (aplTypeRht));

    // Point to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Split cases based upon the element's ptr type
    switch (GetPtrTypeInd (lpMemRht))
    {
        case PTRTYPE_STCONST:       // a {is} 1 {diamond} 0{rho}a (2 3)
            lpSym = *(LPAPLHETERO) lpMemRht;

            // It's an immediate
            Assert (lpSym->stFlags.Imm);
            *lpaplTypeRes = TranslateImmTypeToArrayType (lpSym->stFlags.ImmType);

            break;

        case PTRTYPE_HGLOBAL:
            // Get the HGLOBAL
            hGlbFirst = *(LPAPLNESTED) lpMemRht;

            bRet = FALSE;

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRht); lpMemRht = NULL;

    // If not simple, return the HGLOBAL of the first element
    if (!bRet)
        *lphGlbRht = hGlbFirst;

    return bRet;
} // End IsFirstSimpleGlb


//***************************************************************************
//  $CopySymGlbDir_PTB
//
//  Copy a direct LPSYMENTRY or HGLOBAL incrementing the reference count
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopySymGlbDir"
#else
#define APPEND_NAME
#endif

HGLOBAL CopySymGlbDir_PTB
    (LPVOID lpSymGlb)

{
    // Split cases based upon the ptr type
    switch (GetPtrTypeDir (lpSymGlb))
    {
        case PTRTYPE_STCONST:
            return lpSymGlb;

        case PTRTYPE_HGLOBAL:
            // Increment the reference count in global memory
            DbgIncrRefCntDir_PTB ((HGLOBAL) lpSymGlb);

            return lpSymGlb;

        defstop
            return NULL;
    } // End SWITCH
} // End CopySymGlbDir_PTB
#undef  APPEND_NAME


//***************************************************************************
//  $CopyArray_EM
//
//  Make a copy of a global memory ptr array
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopyArray_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL CopyArray_EM
    (HGLOBAL hGlbSrc,       // Source handle
     LPTOKEN lptkFunc)      // Ptr to function token

{
    SIZE_T       dwSize;
    LPVOID       lpMemSrc, lpMemSrcBase,
                 lpMemDst, lpMemDstBase;
    HGLOBAL      hGlbDst,
                 hGlbTmp;
    APLSTYPE     aplType;
    APLNELM      aplNELM;
    APLRANK      aplRank;
    LPSYMENTRY   lpSymSrc,
                 lpSymDst;
    LPPL_YYSTYPE lpMemFcn;                  // Ptr to function array data
    APLNELM      u;
    UBOOL        bRet = TRUE;

    // Clear the ptr type bits
    hGlbSrc = ClrPtrTypeDir (hGlbSrc);

    // Get the size of the global memory object
    dwSize = MyGlobalSize (hGlbSrc);
    Assert (dwSize NE 0);

    // Allocate storage for the copy of the array
    hGlbDst = DbgGlobalAlloc (GHND, dwSize);
    if (hGlbDst)
    {
        // Lock both memory blocks
        lpMemDst = MyGlobalLock (hGlbDst);
        lpMemSrc = MyGlobalLock (hGlbSrc);

        // Copy source to destin
        CopyMemory (lpMemDst, lpMemSrc, dwSize);

        // Split cases based upon the array type
        switch (GetSignatureMem (lpMemDst))
        {
            case VARARRAY_HEADER_SIGNATURE:
                // Set the reference count in case it was a TKT_VARNAMED
#ifdef DEBUG_REFCNT
                dprintfWL0 (L"##RefCnt=1 in " APPEND_NAME L":        %p(res=1) (%S#%d)", hGlbDst, FNLN);
#endif
#define lpHeader    ((LPVARARRAY_HEADER) lpMemDst)
                // Clear the PermNdx flags
                lpHeader->PermNdx = PERMNDX_NONE;

                // Set the RefCnt
                lpHeader->RefCnt = 1;

                // Clear the PermVec bits
                lpHeader->PV0 =
                lpHeader->PV1 = FALSE;

                // Recurse through the array, copying all the global ptrs
                aplType = lpHeader->ArrType;
                aplNELM = lpHeader->NELM;
                aplRank = lpHeader->Rank;
#undef  lpHeader

                lpMemDstBase = lpMemDst = VarArrayBaseToData (lpMemDst, aplRank);
                lpMemSrcBase = lpMemSrc = VarArrayBaseToData (lpMemSrc, aplRank);

                // Split cases based upon the array type
                switch (aplType)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                    case ARRAY_FLOAT:
                    case ARRAY_CHAR:
                    case ARRAY_APA:
                        break;

                    case ARRAY_HETERO:
                    case ARRAY_NESTED:
                        // Handle the empty case
                        aplNELM = max (aplNELM, 1);

                        // Fill nested result with PTR_REUSED
                        //   in case we fail part way through
                        for (u = 0; u < aplNELM; u++)
                            *((LPAPLNESTED) lpMemDst)++ = PTR_REUSED;

                        // Start the destin ptr over again
                        lpMemDst = lpMemDstBase;

                        // Loop through the source and destin arrays
                        for (u = 0;
                             u < aplNELM;
                             u++, ((LPAPLNESTED) lpMemDst)++, ((LPAPLNESTED) lpMemSrc)++)
                        // Split cases based upon the ptr type
                        switch (GetPtrTypeInd (lpMemSrc))
                        {
                            case PTRTYPE_STCONST:
                                lpSymSrc = *(LPAPLHETERO) lpMemSrc;

                                // It's an immediate
                                Assert (lpSymSrc->stFlags.Imm);

                                // Copy it
                                lpSymDst = CopyImmSymEntry_EM (lpSymSrc,
                                                               -1,
                                                               lptkFunc);
                                if (lpSymDst)
                                    // Save into the destin
                                    *((LPAPLHETERO) lpMemDst) = lpSymDst;
                                else
                                    bRet = FALSE;
                                break;

                            case PTRTYPE_HGLOBAL:
                                Assert (IsNested (aplType));

                                // It's a valid HGLOBAL array
                                Assert (IsGlbTypeVarInd_PTB (lpMemSrc));

                                // Copy the array
                                hGlbTmp = CopyArray_EM (*(LPAPLNESTED) lpMemSrc,
                                                        lptkFunc);
                                if (hGlbTmp)
                                    // Save into the destin
                                    *((LPAPLNESTED) lpMemDst) = MakePtrTypeGlb (hGlbTmp);
                                else
                                    bRet = FALSE;
                                break;

                            defstop
                                break;
                        } // End FOR/SWITCH

                        break;

                    defstop
                        break;
                } // End SWITCH

                break;

            case FCNARRAY_HEADER_SIGNATURE:
#define lpHeader    ((LPFCNARRAY_HEADER) lpMemDst)
                // Set the RefCnt
                lpHeader->RefCnt = 1;

                // Get the NELM
                aplNELM = lpHeader->tknNELM;

                // If there is a line text global memory handle, ...
                if (lpHeader->hGlbTxtLine)
                    lpHeader->hGlbTxtLine =
                      // Copy the memory to a new handle, ignoring failure
                      CopyGlbMemory (lpHeader->hGlbTxtLine, TRUE);
#undef  lpHeader
                // Skip over the header to the data
                lpMemFcn = FcnArrayBaseToData (lpMemDst);

                // Loop through the items
                for (u = 0; u < aplNELM; u++, lpMemFcn++)
                // Split cases based upon the token type
                switch (lpMemFcn->tkToken.tkFlags.TknType)
                {
                    case TKT_VARARRAY:
                    case TKT_FCNARRAY:
                        DbgIncrRefCntDir_PTB (lpMemFcn->tkToken.tkData.tkGlbData);

                        break;

                    default:
                        break;
                } // End FOR/SWITCH

                break;

            case LSTARRAY_HEADER_SIGNATURE:     // No call for these as yet
            case VARNAMED_HEADER_SIGNATURE:     // ...
            defstop
                break;
        } // End SWITCH

        // We no longer need these ptrs
        MyGlobalUnlock (hGlbDst); lpMemDst = NULL;
        MyGlobalUnlock (hGlbSrc); lpMemSrc = NULL;

        if (!bRet)
        {
            FreeResultGlobalVar (hGlbDst); hGlbDst = NULL;
        } // End IF
    } else
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
    return hGlbDst;
} // End CopyArray_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CopyGlbAsType_EM
//
//  Copy a simple global as a given (simple and possibly wider
//    but never narrower) storage type
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopyGlbAsType_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL CopyGlbAsType_EM
    (HGLOBAL  hGlbArg,                  // Arg global memory handle
     APLSTYPE aplTypeRes,               // Result storage type
     LPTOKEN  lptkFunc)                 // Ptr to function token

{
    APLSTYPE     aplTypeArg;            // Arg storage type
    APLNELM      aplNELMArg;            // Arg/result NELM
    APLRANK      aplRankArg;            // Arg/result rank
    APLUINT      ByteRes;               // # bytes in the result
    HGLOBAL      hGlbRes = NULL;        // Result global memory handle
    LPVOID       lpMemArg = NULL,       // Ptr to arg global memory
                 lpMemRes = NULL;       // Ptr to result ...
    APLUINT      uArg;                  // Loop counter
    UINT         uBitMask;              // Bit mask for looping through Booleans
    APLINT       apaOffArg,             // Arg APA offset
                 apaMulArg,             // ...     multiplier
                 aplInteger;            // Temporary integer
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPSYMENTRY   lpSym0,                // LPSYMENTRY for constant zero
                 lpSym1,                // ...                     one
                 lpSymTmp;              // Ptr to temporary LPSYMENTRY

    // Get the attributes (Type, NELM, and Rank) of the arg
    AttrsOfGlb (hGlbArg, &aplTypeArg, &aplNELMArg, &aplRankArg, NULL);

    // Lock the memory to get a ptr to it
    lpMemArg = MyGlobalLock (hGlbArg);

    // Calculate space for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMArg, aplRankArg);

    // Allocate space for the result
    // N.B.:  Conversion from APLUINT to UINT
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMArg;
    lpHeader->Rank       = aplRankArg;
#undef  lpHeader

    // Copy the dimensions
    CopyMemory (VarArrayBaseToDim (lpMemRes),
                VarArrayBaseToDim (lpMemArg),
                (APLU3264) aplRankArg * sizeof (APLDIM));
    // Skip over the header to the data
    lpMemArg = VarArrayBaseToData (lpMemArg, aplRankArg);
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankArg);

    // Copy the existing named values to the result
    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:                    // Res = BOOL, Arg = BOOL/APA
            // Split cases based upon the arg storage type
            switch (aplTypeArg)
            {
                case ARRAY_BOOL:            // Res = BOOL, Arg = BOOL
                    // Copy the arg elements to the result
                    CopyMemory (lpMemRes, lpMemArg, (APLU3264) RoundUpBitsToBytes (aplNELMArg));

                    break;

                case ARRAY_APA:             // Res = BOOL, Arg = APA
#define lpAPA       ((LPAPLAPA) lpMemArg)
                    // Get the APA parameters
                    apaOffArg = lpAPA->Off;
                    apaMulArg = lpAPA->Mul;
#undef  lpAPA
                    if (apaMulArg NE 0 || !IsBooleanValue (apaOffArg))
                        goto DOMAIN_EXIT;

                    // Do something only for 1s
                    if (apaOffArg)
                        FillBitMemory (lpMemRes, aplNELMArg);
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_INT:                     // Res = INT, Arg = BOOL/INT/APA
            // Split cases based upon the arg storage type
            switch (aplTypeArg)
            {
                case ARRAY_BOOL:            // Res = INT, Arg = BOOL
                    uBitMask = BIT0;

                    // Loop through the arg elements
                    for (uArg = 0; uArg < aplNELMArg; uArg++)
                    {
                        if (uBitMask & *(LPAPLBOOL) lpMemArg)
                            *((LPAPLINT) lpMemRes) = 1;
                        ((LPAPLINT) lpMemRes)++;

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;            // Start over
                            ((LPAPLBOOL) lpMemArg)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:             // Res = INT, Arg = INT
                    // Copy the arg elements to the result
                    CopyMemory (lpMemRes, lpMemArg, (APLU3264) aplNELMArg * sizeof (APLINT));

                    break;

                case ARRAY_APA:             // Res = INT, Arg = APA
#define lpAPA       ((LPAPLAPA) lpMemArg)
                    // Get the APA parameters
                    apaOffArg = lpAPA->Off;
                    apaMulArg = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the arg elements
                    for (uArg = 0; uArg < aplNELMArg; uArg++)
                        *((LPAPLINT) lpMemRes)++ = apaOffArg + apaMulArg * uArg;
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_FLOAT:                   // Res = FLOAT, Arg = BOOL/INT/APA/FLOAT
            // Split cases based upon the arg storage type
            switch (aplTypeArg)
            {
                case ARRAY_BOOL:            // Res = FLOAT, Arg = BOOL
                    uBitMask = BIT0;

                    // Loop through the arg elements
                    for (uArg = 0; uArg < aplNELMArg; uArg++)
                    {
                        if (uBitMask & *(LPAPLBOOL) lpMemArg)
                            *((LPAPLFLOAT) lpMemRes) = 1;
                        ((LPAPLFLOAT) lpMemRes)++;

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;            // Start over
                            ((LPAPLBOOL) lpMemArg)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:             // Res = FLOAT, Arg = INT
                    // Loop through the arg elements
                    for (uArg = 0; uArg < aplNELMArg; uArg++)
                        *((LPAPLFLOAT) lpMemRes)++ =
                          (APLFLOAT) *((LPAPLINT) lpMemArg)++;
                    break;

                case ARRAY_APA:             // Res = FLOAT, Arg = APA
#define lpAPA       ((LPAPLAPA) lpMemArg)
                    // Get the APA parameters
                    apaOffArg = lpAPA->Off;
                    apaMulArg = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the arg elements
                    for (uArg = 0; uArg < aplNELMArg; uArg++)
                        *((LPAPLFLOAT) lpMemRes)++ =
                          (APLFLOAT) (APLINT) (apaOffArg + apaMulArg * uArg);
                    break;

                case ARRAY_FLOAT:           // Res = FLOAT, Arg = FLOAT
                    // Copy the arg elements to the result
                    CopyMemory (lpMemRes, lpMemArg, (APLU3264) aplNELMArg * sizeof (APLFLOAT));

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_CHAR:
            // Copy the memory to the result
            CopyMemory (lpMemRes, lpMemArg, (APLU3264) aplNELMArg * sizeof (APLCHAR));

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Copy the arg to the result

            // Fill nested result with PTR_REUSED
            //   in case we fail part way through

            // Fill in the prototype
            *((LPAPLNESTED) lpMemRes) = PTR_REUSED;
            for (uArg = 1; uArg < aplNELMArg; uArg++)
                ((LPAPLNESTED) lpMemRes)[uArg] = PTR_REUSED;

            // Split cases based upon the arg storage type
            switch (aplTypeArg)
            {
                case ARRAY_BOOL:
                    // Get ptr to PerTabData global memory
                    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                    lpSym0 = lpMemPTD->steZero;
                    lpSym1 = lpMemPTD->steOne;

                    uBitMask = BIT0;

                    // Loop through the arg elements
                    for (uArg = 0; uArg < aplNELMArg; uArg++)
                    {
                        // Save an LPSYMENTRY in the result
                        *((LPAPLNESTED) lpMemRes)++ =
                          (uBitMask & *(LPAPLBOOL) lpMemArg) ? lpSym1 : lpSym0;

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;            // Start over
                            ((LPAPLBOOL) lpMemArg)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:
                    // Loop through the arg elements
                    for (uArg = 0; uArg < aplNELMArg; uArg++)
                    {
                        // Save an LPSYMENTRY in the result
                        *((LPAPLNESTED) lpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_INT,                 // Immediate type
                                           ((LPAPLINT) lpMemArg)++,     // Ptr to immediate value
                                           lptkFunc);                   // Ptr to function token
                        if (!lpSymTmp)
                            goto ERROR_EXIT;
                    } // End IF

                    break;

                case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemArg)
                    // Get the APA parameters
                    apaOffArg = lpAPA->Off;
                    apaMulArg = lpAPA->Mul;
#undef  lpAPA
                    // Loop through the arg elements
                    for (uArg = 0; uArg < aplNELMArg; uArg++)
                    {
                        // Calculate the value
                        aplInteger = apaOffArg + apaMulArg * uArg;

                        // Save an LPSYMENTRY in the result
                        *((LPAPLNESTED) lpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_INT,                 // Immediate type
                                          &aplInteger,                  // Ptr to immediate value
                                           lptkFunc);                   // Ptr to function token
                        if (!lpSymTmp)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_FLOAT:
                    // Loop through the arg elements
                    for (uArg = 0; uArg < aplNELMArg; uArg++)
                    {
                        // Save an LPSYMENTRY in the result
                        *((LPAPLNESTED) lpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_FLOAT,               // Immediate type
                            (LPAPLLONGEST) ((LPAPLFLOAT) lpMemArg)++,   // Ptr to immediate value
                                           lptkFunc);                   // Ptr to function token
                        if (!lpSymTmp)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_CHAR:
                    // Loop through the arg elements
                    for (uArg = 0; uArg < aplNELMArg; uArg++)
                    {
                        // Save an LPSYMENTRY in the result
                        *((LPAPLNESTED) lpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (IMMTYPE_CHAR,                // Immediate type
                            (LPAPLLONGEST) ((LPAPLCHAR) lpMemArg)++,    // Ptr to immediate value
                                           lptkFunc);                   // Ptr to function token
                        if (!lpSymTmp)
                            goto ERROR_EXIT;
                    } // End FOR

                    break;

                case ARRAY_HETERO:
                case ARRAY_NESTED:
                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbArg && lpMemArg)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemArg = NULL;
    } // End IF

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    return hGlbRes;
} // End CopyGlbAsType_EM
#undef  APPEND_NAME


//***************************************************************************
//  $IsGlobalTypeArray_PTB
//
//  Confirm that an HGLOBAL is a valid variable or function array, or a
//    user-defined function/operator.
//***************************************************************************

UBOOL IsGlobalTypeArray_PTB
    (HGLOBAL hGlb,
     UINT    Signature)

{
    LPVOID lpMem;
    UBOOL  bRet = TRUE;

    // It's an HGLOBAL
    switch (GetPtrTypeDir (hGlb))
    {
        case PTRTYPE_STCONST:
            return FALSE;

        case PTRTYPE_HGLOBAL:
            break;

        defstop
            break;
    } // End SWITCH

    // Clear the ptr type bits
    hGlb = ClrPtrTypeDir (hGlb);

    // It's a valid handle
    bRet = bRet && IsValidHandle (hGlb);

    if (bRet)
    {
        // Lock the memory to get a ptr to it
        lpMem = GlobalLock (hGlb); Assert (lpMem NE NULL);

        // Split cases based upon the signature
        switch (GetSignatureMem (lpMem))
        {
            case DFN_HEADER_SIGNATURE:
#define lpHeader    ((LPDFN_HEADER) lpMem)
                // Ensure it has the correct signature
                bRet = (lpHeader->Sig.nature EQ Signature
                     && lpHeader->RefCnt > 0);
#undef  lpHeader
                break;

            case FCNARRAY_HEADER_SIGNATURE:
#define lpHeader    ((LPFCNARRAY_HEADER) lpMem)
                // Ensure it has the correct signature
                bRet = (lpHeader->Sig.nature EQ Signature
                     && lpHeader->RefCnt > 0);
#undef  lpHeader
                break;

            case LSTARRAY_HEADER_SIGNATURE:
#define lpHeader    ((LPLSTARRAY_HEADER) lpMem)
                // Ensure it has the correct signature
                bRet = (lpHeader->Sig.nature EQ Signature);
#undef  lpHeader
                break;

            case VARARRAY_HEADER_SIGNATURE:
#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
                // Ensure it has the correct signature
                bRet = (lpHeader->Sig.nature EQ Signature)
                    && ((lpHeader->PermNdx NE PERMNDX_NONE) || (lpHeader->RefCnt > 0));
#undef  lpHeader
                break;

            case VARNAMED_HEADER_SIGNATURE:
#define lpHeader    ((LPVARNAMED_HEADER) lpMem)
                // Ensure it has the correct signature
                bRet = (lpHeader->Sig.nature EQ Signature);
#undef  lpHeader
                break;

            defstop
                break;
        } // End SWITCH

        // We no longer need this ptr
        GlobalUnlock (hGlb); lpMem = NULL;
    } // End IF

    return bRet;
} // End IsGlobalTypeArray_PTB


//***************************************************************************
//  $CheckRankLengthError_EM
//
//  Check a dyadic scalar function for RANK and LENGTH ERRORs
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckRankLengthError_EM"
#else
#define APPEND_NAME
#endif

UBOOL CheckRankLengthError_EM
    (APLRANK  aplRankRes,           // Result rank
     APLRANK  aplRankLft,           // Left arg ...
     APLNELM  aplNELMLft,           // ...      NELM
     LPVOID   lpMemLft,             // Ptr to left arg memory
     APLRANK  aplRankRht,           // Right arg rank
     APLNELM  aplNELMRht,           // ...       NELM
     LPVOID   lpMemRht,             // Ptr to right arg memory
     APLNELM  aplNELMAxis,          // Axis NELM
     LPAPLINT lpMemAxisTail,        // Ptr to grade up of AxisHead
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    APLINT uRes;

    //***************************************************************
    // If the ranks don't match, then it gets complicated:
    //
    //   If both arguments are singletons, the result rank
    //     (and shape) is the same rank (and shape) as the
    //     argument with the larger rank.
    //   If only one argument is a singleton, the result rank
    //     (and shape) is the same as the rank (and shape) of the
    //     non-singleton argument.
    //   If either argument is a singleton, any valid axis values
    //     are allowed.  That is, any scalar or vector X such that
    //     {and}/X{epsilon}{iota}{rho}{rho}Z.
    //   If no axis is specified, it is equivalent to the axis being
    //     {iota}{rho}{rho}Z.
    //   If neither argument is a singleton and axis is specified,
    //     then it must be true that
    //       ({rho}L)[X]{match}{rho}R
    //     assuming that L is the argument of lower rank.  If not,
    //     switch L and R in the above expression.
    //***************************************************************

    // OUTER RANK and LENGTH ERRORs are possible only if
    //   neither argument is a singleton
    if (!IsSingleton (aplNELMLft) && !IsSingleton (aplNELMRht))
    {
        // Check for OUTER RANK ERROR
        if ((aplRankLft <  aplRankRht && (aplRankLft NE aplNELMAxis))
         || (aplRankLft >  aplRankRht && (aplRankRht NE aplNELMAxis)))
            goto RANK_EXIT;

        // If axis not full, ...
        if (aplNELMAxis NE aplRankRes)
        {
            if (aplRankLft < aplRankRht)
            {
                // Check for OUTER LENGTH ERROR
                for (uRes = 0; uRes < (APLRANKSIGN) aplRankLft; uRes++)
                if ((VarArrayBaseToDim (lpMemLft))[uRes] !=
                    (VarArrayBaseToDim (lpMemRht))[lpMemAxisTail[uRes]])
                {
                    uRes = (APLINT) -1; // Mark as in error

                    break;
                } // End FOR/IF
            } else
            if (aplRankLft > aplRankRht)
            {
                // Check for OUTER LENGTH ERROR
                for (uRes = 0; uRes < (APLRANKSIGN) aplRankRht; uRes++)
                if ((VarArrayBaseToDim (lpMemLft))[lpMemAxisTail[uRes]] !=
                    (VarArrayBaseToDim (lpMemRht))[uRes])
                {
                    uRes = (APLINT) -1; // Mark as in error

                    break;
                } // End FOR/IF
            } else
                uRes = (APLINT) -1; // Mark as in error (equal ranks with axis)
        } else
        // If axis full (or no axis) and ranks the same, ...
        if (aplRankLft EQ aplRankRht)
        {
            // Check for OUTER LENGTH ERROR
            for (uRes = 0; uRes < (APLRANKSIGN) aplRankRes; uRes++)
            if ((VarArrayBaseToDim (lpMemLft))[uRes] !=
                (VarArrayBaseToDim (lpMemRht))[uRes])
            {
                uRes = (APLINT) -1; // Mark as in error

                break;
            } // End FOR/IF
        } else
            uRes = (APLINT) -1; // Mark as in error (unequal ranks w/o axis)

        // Check for error
        if (uRes EQ (APLINT) -1)
            goto LENGTH_EXIT;
    } // End IF

    return TRUE;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    return FALSE;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    return FALSE;
} // End CheckRankLengthError_EM
#undef  APPEND_NAME


//***************************************************************************
//  $RoundUpBitsToBytes
//
//  Round up bits to a byte (8 bits/byte) and convert to # bytes
//***************************************************************************

APLINT RoundUpBitsToBytes
    (APLNELM aplNELM)           // NELM to convert

{
    return ((aplNELM + (NBIB - 1)) & ~MASKLOG2NBIB) >> LOG2NBIB;
} // End RoundUpBitsToBytes


//***************************************************************************
//  $RoundUpBitsInArray
//
//  Round up bits to a dword (32 bits/dword) and convert to # bytes
//***************************************************************************

APLINT RoundUpBitsInArray
    (APLNELM aplNELM)           // NELM to convert

{
    return ((aplNELM + (NBID - 1)) & ~MASKLOG2NBID) >> LOG2NBIB;
} // End RoundUpBitsInArray


//***************************************************************************
//  $BytesIn
//
//  Return the # bytes in an array of a given type and NELM
//***************************************************************************

APLUINT BytesIn
    (APLSTYPE aplTypeRht,               // Right arg storage type
     APLNELM  aplNELMRht)               // Right arg NELM

{
    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:                // 1 bit per element
            return RoundUpBitsToBytes (aplNELMRht);

        case ARRAY_INT:                 // 8 bytes per element
        case ARRAY_FLOAT:
            return aplNELMRht * sizeof (APLINT);

        case ARRAY_CHAR:                // 2 byte per element
            return aplNELMRht * sizeof (APLCHAR);

        case ARRAY_APA:                 // 2 8-byte elements
            return sizeof (APLAPA);

        case ARRAY_HETERO:              // 4 bytes per element
            return aplNELMRht * sizeof (APLHETERO);

        case ARRAY_NESTED:              // 4 bytes per element
            return aplNELMRht * sizeof (APLNESTED);

        defstop
            return 0;
    } // End SWITCH
} // End BytesIn


//***************************************************************************
//  $abs64
//
//  Return the absolute value of a 64-bit integer
//
//  Originally, this function was a macro as in
//
//    #define abs64(a)    (((a)>=0)?a:-(a))
//
//  however, the first time I tried
//
//    abs64 (*((LPAPLINT) lpMemLft)++)
//
//  I decided it was better off as a function.
//***************************************************************************

APLINT abs64
    (APLINT aplInt)

{
    return (aplInt >= 0) ? aplInt : -aplInt;
} // End abs64


//***************************************************************************
//  $mod64
//
//  Return the modulus of a 64-bit integer by a 64-bit integer
//
//  I'd like to use C's bulltin function a % b, but it signals a
//    Divide By Zero error if b EQ 0.
//***************************************************************************

APLINT mod64
    (APLINT aplLft,             // Left arg (the value -- moduland?)
     APLINT aplRht)             // Right arg (the modulus)

{
    if (aplRht)
        return aplLft % aplRht;
    else
        return aplLft;
} // End mod64


//***************************************************************************
//  $_iadd64
//
//  Add two 64-bit integers retaining maximum precision
//    while handling overflow.
//***************************************************************************

APLINT _iadd64
    (APLINT  aplLft,            // Left arg
     APLINT  aplRht,            // Right ...
     LPUBOOL lpbRet)            // Is the result valid?? (may be NULL)

{
    APLINT aplRes;              // The result
    UBOOL  bRet;                // TRUE iff the result is valid

    // Call an assembler routine
    bRet = iAsmAdd64 (&aplRes, aplLft, aplRht);

    // If the caller handles the overflow, tell 'em whether or not it did
    if (lpbRet)
        *lpbRet = bRet;
    else
    // Otherwise, if it overflowed, ...
    if (!bRet)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    return aplRes;
} // End _iadd64


//***************************************************************************
//  $_isub64
//
//  Subtract two 64-bit integers retaining maximum precision
//    while handling overflow.
//***************************************************************************

APLINT _isub64
    (APLINT  aplLft,            // Left arg
     APLINT  aplRht,            // Right ...
     LPUBOOL lpbRet)            // Is the result valid?? (may be NULL)

{
    APLINT aplRes;              // The result
    UBOOL  bRet;                // TRUE iff the result is valid

    // Call an assembler routine
    bRet = iAsmSub64 (&aplRes, aplLft, aplRht);

    // If the caller handles the overflow, tell 'em whether or not it did
    if (lpbRet)
        *lpbRet = bRet;
    else
    // Otherwise, if it overflowed, ...
    if (!bRet)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    return aplRes;
} // End _isub64


//***************************************************************************
//  $_imul64
//
//  Multiply two 64-bit integers retaining maximum precision
//    while handling overflow.
//***************************************************************************

APLINT _imul64
    (APLINT  aplLft,            // Left arg
     APLINT  aplRht,            // Right ...
     LPUBOOL lpbRet)            // Is the result valid?? (may be NULL)

{
    APLINT aplRes;              // The result
    UBOOL  bRet;                // TRUE iff the result is valid

    // Call an assembler routine
    bRet = iAsmMul64 (&aplRes, aplLft, aplRht);

    // If the caller handles the overflow, tell 'em whether or not it did
    if (lpbRet)
        *lpbRet = bRet;
    else
    // Otherwise, if it overflowed, ...
    if (!bRet)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    return aplRes;
} // End _imul64


//***************************************************************************
//  $CalcArraySize
//
//  Calculate the size in bytes of an array
//***************************************************************************

APLUINT CalcArraySize
    (ARRAY_TYPES aplType,       // Result storage type
     APLNELM     aplNELM,       // ...    NELM
     APLRANK     aplRank)       // ...    rank

{
    APLUINT ByteRes;            // # bytes in the result

    Assert (0 <= aplType
         &&      aplType < ARRAY_LENGTH);

    // Split cases based upon the result storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            ByteRes = sizeof (APLBOOL)   * RoundUpBitsInArray (aplNELM);

            break;

        case ARRAY_INT:
            ByteRes = sizeof (APLINT)    * aplNELM;

            break;

        case ARRAY_FLOAT:
            ByteRes = sizeof (APLFLOAT)  * aplNELM;

            break;

        case ARRAY_CHAR:
            // Add in one element so we always have
            //   a zero-terminated string
            ByteRes = sizeof (APLCHAR)   * (aplNELM + 1);

            break;

        case ARRAY_APA:
            ByteRes = sizeof (APLAPA);

            break;

        case ARRAY_HETERO:
            ByteRes = sizeof (APLHETERO) * aplNELM;

            break;

        case ARRAY_LIST:
            return sizeof (APLLIST)   * aplNELM
                 + sizeof (LSTARRAY_HEADER);

        case ARRAY_NESTED:
            // Make room for the prototype
            ByteRes = sizeof (APLNESTED) * max (aplNELM, 1);

            break;

        defstop
            break;
    } // End SWITCH

    // Add in the size of the header and dimension
    ByteRes += CalcHeaderSize (aplRank);

    return ByteRes;
} // End CalcArraySize


//***************************************************************************
//  $CalcHeaderSize
//
//  Calculate the size in bytes of the header of an array
//***************************************************************************

APLUINT CalcHeaderSize
    (APLRANK aplRank)

{
    return sizeof (VARARRAY_HEADER)
         + sizeof (APLDIM) * aplRank;
} // End CalcHeaderSize


//***************************************************************************
//  $CalcFcnSize
//
//  Calculate the size in bytes of a function array
//***************************************************************************

APLUINT CalcFcnSize
    (UINT fcnCount)             // # fcns in the array

{
    return sizeof (FCNARRAY_HEADER) + fcnCount * sizeof (PL_YYSTYPE);
} // End CalcFcnSize


//***************************************************************************
//  $MakeNoValue_YY
//
//  Return a NoValue PL_YYSTYPE entry
//***************************************************************************

LPPL_YYSTYPE MakeNoValue_YY
    (LPTOKEN lptkFunc)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARNAMED;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
    lpYYRes->tkToken.tkFlags.NoDisplay = TRUE;
    lpYYRes->tkToken.tkData.tkSym      = lpMemPTD->steNoValue;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End MakeNoValue_YY


//***************************************************************************
//  $GetSISLayer
//
//  Peel back to the first non-Imm/Exec layer
//***************************************************************************

LPSIS_HEADER GetSISLayer
    (LPSIS_HEADER lpSISCur)             // Ptr to current SIS layer

{
    while (lpSISCur
        && (lpSISCur->DfnType EQ DFNTYPE_IMM
         || lpSISCur->DfnType EQ DFNTYPE_EXEC))
        lpSISCur = lpSISCur->lpSISPrv;

    return lpSISCur;
} // End GetSISLayer


//***************************************************************************
//  $FillSISNxt
//
//  Fill in the SIS Header for lpSISNxt
//***************************************************************************

void FillSISNxt
    (LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     HANDLE       hSemaphore,           // Semaphore handle
     DFN_TYPES    DfnType,              // DFNTYPE_xxx
     FCN_VALENCES FcnValence,           // FCNVALENCE_xxx
     UBOOL        Suspended,            // TRUE iff starts Suspended
     UBOOL        Restartable,          // TRUE iff restartable
     UBOOL        LinkIntoChain)        // TRUE iff we should link this entry into the SIS chain

{
    lpMemPTD->SILevel++;

    // Clear to default value
    ZeroMemory (lpMemPTD->lpSISNxt, sizeof (*lpMemPTD->lpSISNxt));

    // Create another level on the SI stack
    lpMemPTD->lpSISNxt->Sig.nature    = SIS_HEADER_SIGNATURE;
    lpMemPTD->lpSISNxt->hSemaphore    = hSemaphore;
////lpMemPTD->lpSISNxt->hSigaphore    = NULL;           // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->hGlbDfnHdr    = NULL;           // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->hGlbFcnName   = NULL;           // Already zero from ZeroMemory
    lpMemPTD->lpSISNxt->hGlbQuadEM    = hGlbQuadEM;
    lpMemPTD->lpSISNxt->DfnType       = DfnType;
    lpMemPTD->lpSISNxt->FcnValence    = FcnValence;
////lpMemPTD->lpSISNxt->DfnAxis       = FALSE;          // Already zero from ZeroMemory
    lpMemPTD->lpSISNxt->Suspended     = Suspended;
////lpMemPTD->lpSISNxt->ResetFlag     = RESETFLAG_NONE; // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->PermFn        = FALSE;          // Already zero from ZeroMemory
    lpMemPTD->lpSISNxt->Restartable   = Restartable;
////lpMemPTD->lpSISNxt->Unwind        = FALSE;          // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->Avail         = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->EventType     = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->CurLineNum    = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->NxtLineNum    = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->NxtTknNum     = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->numLabels     = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->numFcnLines   = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->QQPromptLen   = 0;              // Already zero from ZeroMemory
////lpMemPTD->lpSISNxt->ErrorCode     = ERRORCODE_NONE; // Already zero from ZeroMemory
    lpMemPTD->lpSISNxt->lpSISPrv      = lpMemPTD->lpSISCur;
    lpMemPTD->lpSISNxt->lpSISNxt      = (LPSIS_HEADER) ByteAddr (lpMemPTD->lpSISNxt, sizeof (SIS_HEADER));
////lpMemPTD->lpSISNxt->lptkFunc      = NULL;           // Already zero from ZeroMemory

    // Fill in the FORSTMT ptr
    if (lpMemPTD->lpSISCur)
        lpMemPTD->lpSISNxt->lpForStmtBase =
        lpMemPTD->lpSISNxt->lpForStmtNext = lpMemPTD->lpSISCur->lpForStmtNext;
    else
        lpMemPTD->lpSISNxt->lpForStmtBase =
        lpMemPTD->lpSISNxt->lpForStmtNext = lpMemPTD->lpForStmtBase;
#ifdef DEBUG
    dprintfWL9 (L"~~Localize:    %p (%s)", lpMemPTD->lpSISNxt, L"FillSISNxt");
#endif
    // Link this SIS into the chain, if requested
    if (LinkIntoChain)
    {
        if (lpMemPTD->lpSISCur)
            lpMemPTD->lpSISCur->lpSISNxt = lpMemPTD->lpSISNxt;
        lpMemPTD->lpSISCur = lpMemPTD->lpSISNxt;
        lpMemPTD->lpSISNxt = lpMemPTD->lpSISNxt->lpSISNxt;
    } // End IF
} // End FillSISNxt


//***************************************************************************
//  $PassSigaphore
//
// If this hSigaphore is not for this level, pass it on up the line
//***************************************************************************

LPSIS_HEADER PassSigaphore
    (LPPERTABDATA lpMemPTD,
     HANDLE       hSigaphore)
{
    if (hSigaphore
     && lpMemPTD->lpSISCur
     && hSigaphore NE lpMemPTD->lpSISCur->hSemaphore)
    {
        Assert (lpMemPTD->lpSISCur->hSigaphore EQ NULL);

        // Pass it on up the line
        lpMemPTD->lpSISCur->hSigaphore = hSigaphore;
        hSigaphore = NULL;
    } // End IF

    return hSigaphore;
} // End PassSigaphore


//***************************************************************************
//  $GetImmTypeFcn
//
//  Get the IMMTYPE_PRIMFCN value corresponding to a primitive function
//***************************************************************************

UINT GetImmTypeFcn
    (WCHAR wch)                 // Immediate function symbol (UTF16_xxx)

{
    return IMMTYPE_PRIMFCN;

////// Split cases based upon the immediate function symbol
////switch (wch)
////{
////    case UTF16_BAR:
////    case UTF16_CIRCLE:
////    case UTF16_CIRCLESTAR:
////    case UTF16_COLONBAR:
////    case UTF16_DOWNSTILE:
////    case UTF16_PLUS:
////    case UTF16_QUOTEDOT:
////    case UTF16_STAR:
////    case UTF16_STILE:
////    case UTF16_TIMES:
////    case UTF16_UPSTILE:
////        return IMMTYPE_PRIMFCN_SS;      // Monadic scalar     , dyadic scalar
////
////    case UTF16_QUERY:
////    case UTF16_TILDE:
////        return IMMTYPE_PRIMFCN_SM;      // Monadic scalar     , dyadic mixed/error
////
////    case UTF16_DOWNCARET:
////    case UTF16_DOWNCARETTILDE:
////    case UTF16_EQUAL:
////    case UTF16_LEFTCARET:
////    case UTF16_LEFTCARETUNDERBAR:
////    case UTF16_NOTEQUAL:
////    case UTF16_RIGHTCARET:
////    case UTF16_RIGHTCARETUNDERBAR:
////    case UTF16_UPCARET:
////    case UTF16_UPCARETTILDE:
////        return IMMTYPE_PRIMFCN_MS;      // Monadic mixed/error, dyadic scalar
////
////    case UTF16_ALPHA:
////    case UTF16_CIRCLEBAR:
////    case UTF16_CIRCLESLOPE:
////    case UTF16_CIRCLESTILE:
////    case UTF16_COMMA:
////    case UTF16_COMMABAR:
////    case UTF16_DELSTILE:
////    case UTF16_DELTASTILE:
////    case UTF16_DOMINO:
////    case UTF16_DOWNARROW:
////    case UTF16_DOWNTACK:
////    case UTF16_DOWNTACKJOT:
////    case UTF16_EPSILON:
////    case UTF16_EPSILONUNDERBAR:
////    case UTF16_EQUALUNDERBAR:
////    case UTF16_IOTA:
////    case UTF16_IOTAUNDERBAR:
////    case UTF16_LEFTSHOE:
////    case UTF16_LEFTTACK:
////    case UTF16_OMEGA:
////    case UTF16_RIGHTSHOE:
////    case UTF16_RIGHTTACK:
////    case UTF16_RHO:
////    case UTF16_SLASH:
////    case UTF16_SLASHBAR:
////    case UTF16_SLOPE:
////    case UTF16_SLOPEBAR:
////    case UTF16_SQUAD:
////    case UTF16_UPARROW:
////    case UTF16_UPSHOE:
////    case UTF16_UPTACK:
////    case UTF16_UPTACKJOT:
////        return IMMTYPE_PRIMFCN_MM;      // Monadic mixed/error, dyadic mixed/error
////
////    defstop
////        return 0;
////} // End SWITCH
} // End GetImmTypeFcn


//***************************************************************************
//  $IsTknTypeNamed
//
//  Return TRUE iff the given token type is named
//***************************************************************************

UBOOL IsTknTypeNamed
    (TOKEN_TYPES tknType)

{
    // Split cases based upon the token type
    switch (tknType)
    {
        case TKT_VARNAMED:
        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
        case TKT_OP3NAMED:
            return TRUE;

        default:
            return FALSE;
    } // End SWITCH
} // End IsTknTypeNamed


//***************************************************************************
//  $IsTknTypeNamedFcnOpr
//
//  Return TRUE iff the given token type is a named fcn/opr
//***************************************************************************

UBOOL IsTknTypeNamedFcnOpr
    (TOKEN_TYPES tknType)

{
    // Split cases based upon the token type
    switch (tknType)
    {
        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
        case TKT_OP3NAMED:
            return TRUE;

        default:
            return FALSE;
    } // End SWITCH
} // End IsTknTypeNamedFcnOpr


//***************************************************************************
//  $IsTknTypeNamedVar
//
//  Return TRUE iff the given token type is a named var
//***************************************************************************

UBOOL IsTknTypeNamedVar
    (TOKEN_TYPES tknType)

{
    // Split cases based upon the token type
    switch (tknType)
    {
        case TKT_VARNAMED:
            return TRUE;

        default:
            return FALSE;
    } // End SWITCH
} // End IsTknTypeNamedVar


//***************************************************************************
//  $IsTknUsrDfn
//
//  Return TRUE iff the given token is a user-defined function
//***************************************************************************

UBOOL IsTknUsrDfn
    (LPTOKEN lptkVar)

{
    HGLOBAL hGlb;

    // If the token is immediate, it can't be a user fn
    if (IsTknImmed (lptkVar))
        return FALSE;

    // Get the global memory handle (if any)
    hGlb = GetGlbHandle (lptkVar);

    if (hGlb EQ NULL)
        return FALSE;

    return (IsGlbTypeDfnDir_PTB (MakePtrTypeGlb (hGlb)));
} // End IsTknUsrDfn


//***************************************************************************
//  $IsTknImmed
//
//  Return TRUE iff the given token is immediate
//***************************************************************************

UBOOL IsTknImmed
    (LPTOKEN lptkVar)                       // Ptr to var token

{
    // Split cases based upon the token type
    switch (lptkVar->tkFlags.TknType)
    {
        case TKT_VARIMMED:
        case TKT_FCNIMMED:
        case TKT_OP1IMMED:
        case TKT_OP2IMMED:
        case TKT_OP3IMMED:
        case TKT_AXISIMMED:
        case TKT_LSTIMMED:
        case TKT_OPJOTDOT:
            return TRUE;

        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkVar->tkData.tkVoid) EQ PTRTYPE_STCONST);

            return lptkVar->tkData.tkSym->stFlags.Imm;

        default:
            return FALSE;
    } // End SWITCH
} // End IsTknImmed


//***************************************************************************
//  $SetVarArraySkipRefCntFlag
//
//  Set SkipRefCntIncr flag in a variable array.
//***************************************************************************

void SetVarArraySkipRefCntFlag
    (LPTOKEN lptkVar)                   // Ptr to var token

{
    HGLOBAL           hGlbVar;
    LPVARARRAY_HEADER lpMemVar;

    GetGlbPtrs_LOCK (lptkVar, &hGlbVar, &lpMemVar);
    if (hGlbVar)
    {
        // Set the flag
        lpMemVar->SkipRefCntIncr = TRUE;

        MyGlobalUnlock (hGlbVar); lpMemVar = NULL;
    } // End IF
} // End SetVarArrayFlags


//***************************************************************************
//  End of File: primfns.c
//***************************************************************************
