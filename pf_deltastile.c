//***************************************************************************
//  NARS2000 -- Primitive Function -- DelStile & DeltaStile
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


#define GRADE_ROUTINE       HeapSort


//***************************************************************************
//  $PrimFnDeltaStile_EM_YY
//
//  Primitive function for monadic and dyadic DeltaStile
//    ("grade up numeric" and "grade up character")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDeltaStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDeltaStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DELTASTILE);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonGradeCommon_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis, FALSE);
    else
        return PrimFnDydGradeCommon_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, FALSE);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnDeltaStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDelStile_EM_YY
//
//  Primitive function for monadic and dyadic DelStile
//    ("grade down numeric" and "grade down character")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDelStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDelStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DELSTILE);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonGradeCommon_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis, FALSE);
    else
        return PrimFnDydGradeCommon_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, FALSE);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimFnDelStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnDeltaStile_EM_YY
//
//  Generate a prototype for the primitive functions
//    monadic & dyadic DeltaStile
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnDeltaStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnDeltaStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnDeltaStile_EM_YY, // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token (may be NULL if monadic)
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnDeltaStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnDelStile_EM_YY
//
//  Generate a prototype for the primitive functions
//    monadic & dyadic DelStile
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnDelStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnDelStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnDelStile_EM_YY,   // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token (may be NULL if monadic)
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnDelStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonGradeCommon_EM_YY
//
//  Primitive function for monadic DelStile and DeltaStile
//    ("grade up/down numeric")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonGradeCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonGradeCommon_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis,              // Ptr to axis token (may be NULL)
     UBOOL   bRavelArg)             // TRUE iff we're to treat the right arg as ravelled

{
    APLNELM       aplNELMRht,       // Right arg NELM
                  aplNELMRes;       // Result    ...
    APLRANK       aplRankRht;       // Right arg rank
    HGLOBAL       hGlbRht = NULL,   // Right arg global memory handle
                  hGlbRes = NULL;   // Result    ...
    APLUINT       ByteRes,          // # bytes in the result
                  uRes;             // Loop counter
    LPVOID        lpMemRht = NULL,  // Ptr to right arg global memory
                  lpMemRes = NULL;  // Ptr to result    ...
    LPAPLDIM      lpMemDimRht;      // Ptr to right arg dimensions
    UBOOL         bRet = TRUE;      // TRUE iff result is valid
    LPPL_YYSTYPE  lpYYRes = NULL;   // Ptr to the result
    APLBOOL       bQuadIO;          // []IO
    GRADE_DATA    gradeData;        // Data passed to GRADE_ROUTINE
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    gradeData.lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Mark as grade up or down
    gradeData.iMul           = (lptkFunc->tkData.tkChar EQ UTF16_DELTASTILE) ? 1 : -1;
    gradeData.aplRankLft     = 0;
    gradeData.lpMemTTHandles = NULL;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &gradeData.aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RIGHT RANK ERROR
    if (IsScalar (aplRankRht))
        goto RANK_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if (!IsSimpleNum (gradeData.aplTypeRht))
        goto DOMAIN_EXIT;

    // Get right arg global ptr
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Save the
    gradeData.PV0 = ((LPVARARRAY_HEADER) lpMemRht)->PV0;
    gradeData.PV1 = ((LPVARARRAY_HEADER) lpMemRht)->PV1;

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // If we're ravelling the right arg, ...
    if (bRavelArg)
    {
        // Get the NELM of the right arg (as result length)
        gradeData.aplNELMRht = aplNELMRes = aplNELMRht;

        // Calculate the length of the dimensions after the first one
        gradeData.aplNELMRest = 1;
    } else
    {
        // Get the length of the first dimension (as result length)
        gradeData.aplNELMRht = aplNELMRes = lpMemDimRht[0];

        // Calculate the length of the dimensions after the first one
        for (gradeData.aplNELMRest = 1, uRes = 1; uRes < aplRankRht; uRes++)
            gradeData.aplNELMRest *= lpMemDimRht[uRes];
    } // End IF/ELSE

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_INT, aplNELMRes, 1);

    // Allocate space for the result.
    // N.B. Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (APLU3264) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->PV0        = (gradeData.PV0 || gradeData.PV1) && bQuadIO EQ 0;
    lpHeader->PV1        = (gradeData.PV0 || gradeData.PV1) && bQuadIO EQ 1;
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Save the dimension in the result
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // If the right arg is an APA, ...
    if (IsSimpleAPA (gradeData.aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        gradeData.apaOffRht = lpAPA->Off;
        gradeData.apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // If the right arg is a PV, grade it specially
    if (gradeData.PV0 || gradeData.PV1)
    {
        if (!PermVecGrade (lpMemRes,            // Ptr to result global memory
                           lpMemRht,            // Ptr to right arg global memory
                          &gradeData))          // Ptr to extra grade data
            goto ERROR_EXIT;
    } else
    // If the right arg is a Boolean vector, grade it specially
    if (IsSimpleBool (gradeData.aplTypeRht) && IsVector (aplRankRht))
    {
        if (!BoolVecGrade (lpMemRes,            // Ptr to result global memory
                           lpMemRht,            // Ptr to right arg global memory
                          &gradeData))          // Ptr to extra grade data
            goto ERROR_EXIT;
    } else
    {
        // Initialize the result with {iota}aplNELMRes (origin-0)
        for (uRes = 0; uRes < aplNELMRes; uRes++)
            ((LPAPLINT) lpMemRes)[uRes] = uRes;

        // Grade the array
        if (!GRADE_ROUTINE (lpMemRes,               // Ptr to result global memory
                            lpMemRht,               // Ptr to right arg global memory
                            aplNELMRes,             // Result NELM
                           &PrimFnGradeCompare,     // Ptr to comparison routine
                            gradeData.lpbCtrlBreak, // Ptr to Ctrl-Break flag
                           &gradeData))             // Ptr to extra grade data
            goto ERROR_EXIT;
    } // End IF/ELSE

    // Add in []IO
    for (uRes = 0; uRes < aplNELMRes; uRes++)
        ((LPAPLINT) lpMemRes)[uRes] += bQuadIO;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

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
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnMonGradeCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PermVecGrade
//
//  Grade a permutation vectora
//***************************************************************************

UBOOL PermVecGrade
    (LPAPLUINT    lpMemRes,         // Ptr to result global memory
     LPAPLUINT    lpMemRht,         // Ptr to right arg global memory
     LPGRADE_DATA lpGradeData)      // Ptr to extra data

{
    APLUINT uRht,                   // Loop counter
            PV1;                    // Right arg PV1
    APLNELM aplNELMRht,             // Right arg NELM
            aplNELMRht1;            // Right arg NELM - 1 - PV1
    APLINT  apaOffRht,              // Right arg APA offset
            apaMulRht;              // ...           multiplier

    // Get Grade Data parameters
    apaOffRht  = lpGradeData->apaOffRht;
    apaMulRht  = lpGradeData->apaMulRht;
    aplNELMRht = lpGradeData->aplNELMRht;
    PV1        = lpGradeData->PV1;

    // If it's Grade Up, ...
    if (lpGradeData->iMul EQ 1)
    {
        // Split cases based upon the right arg storage type
        switch (lpGradeData->aplTypeRht)
        {
            case ARRAY_INT:
                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Check for Ctrl-Break
                    if (*lpGradeData->lpbCtrlBreak)
                        goto ERROR_EXIT;

                    lpMemRes[*lpMemRht++ - PV1] = uRht;
                } // End FOR

                break;

            case ARRAY_APA:
                // Initialize the first element
                apaOffRht -= PV1;

                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++, apaOffRht += apaMulRht)
                {
                    // Check for Ctrl-Break
                    if (*lpGradeData->lpbCtrlBreak)
                        goto ERROR_EXIT;

                    lpMemRes[apaOffRht] = uRht;
                } // End FOR

                break;

            defstop
                break;
        } // End SWITCH
    } else
    {
        // Factor out common data
        aplNELMRht1 = (aplNELMRht - 1) + PV1;

        // Split cases based upon the right arg storage type
        switch (lpGradeData->aplTypeRht)
        {
            case ARRAY_INT:
                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Check for Ctrl-Break
                    if (*lpGradeData->lpbCtrlBreak)
                        goto ERROR_EXIT;

                    lpMemRes[aplNELMRht1 - *lpMemRht++] = uRht;
                } // End FOR

                break;

            case ARRAY_APA:
                // Loop through the right arg
                for (uRht = 0; uRht < aplNELMRht; uRht++, apaOffRht += apaMulRht)
                {
                    // Check for Ctrl-Break
                    if (*lpGradeData->lpbCtrlBreak)
                        goto ERROR_EXIT;

                    lpMemRes[aplNELMRht1 - apaOffRht] = uRht;
                } // End FOR

                break;

            defstop
                break;
        } // End SWITCH
    } // End IF/ELSE

    return TRUE;
ERROR_EXIT:
    return FALSE;
} // End PermVecGrade


//***************************************************************************
//  $BoolVecGrade
//
//  Grade a Boolean vectora
//***************************************************************************

UBOOL BoolVecGrade
    (LPAPLUINT    lpMemRes,         // Ptr to result global memory
     LPAPLBOOL    lpMemRht,         // Ptr to right arg global memory
     LPGRADE_DATA lpGradeData)      // Ptr to extra data

{
    APLUINT   uRht,                 // Loop counter
              u1s,                  // # 1s in the right arg
              uVal;                 // Byte value
    APLNELM   aplNELMRht,           // Right arg NELM
              aplNELMRht1;          // Right arg NELM / 8
    LPAPLBOOL lpMemBool;            // Ptr to right arg
    UINT      uBitIndex,            // Index for looping through Booleans
              uBitMask,             // Mask  ...
              uNBIB;                // # bits in the last byte
    APLUINT   aplOff0,              // Result offset of next 1 index
              aplOff1;              // ...                   0 ...

    // Get Grade Data parameters
    aplNELMRht = lpGradeData->aplNELMRht;
    aplNELMRht1 = RoundUpBitsToBytes (aplNELMRht);

    // Copy ptr to right arg global memory
    lpMemBool = lpMemRht;

    // Loop through the right arg bytes
    for (uRht = u1s = 0; uRht < aplNELMRht1; uRht++)
    {
        // Check for Ctrl-Break
        if (*lpGradeData->lpbCtrlBreak)
            goto ERROR_EXIT;

        // Count the # 1s
        u1s += FastBoolTrans[*lpMemBool++][FBT_BITSUM];
    } // End IF

    // Restart the ptr
    lpMemBool = lpMemRht;

    // Initialize bit index & mask
    uBitIndex = 0;
    uBitMask  = BIT0;

    // If it's Grade Up, ...
    if (lpGradeData->iMul EQ 1)
    {
        // Calculate offsets for storing 0s and 1s
        aplOff0 = 0;
        aplOff1 = aplNELMRht - u1s;
    } else
    {
        // Calculate offsets for storing 0s and 1s
        aplOff0 = u1s;
        aplOff1 = 0;
    } // End IF/ELSE

    // Loop through the right arg bytes again
    for (uRht = 0; uRht < aplNELMRht1; uRht++)
    {
        // Check for Ctrl-Break
        if (*lpGradeData->lpbCtrlBreak)
            goto ERROR_EXIT;

        // Perhaps there's a faster way to loop through the bits.
        // There certainly is in assembler language.

        // Get the next byte
        uVal = *lpMemBool++;

        // Check for end-of-vector
        if (uRht EQ (aplNELMRht1 - 1))
            uNBIB = (UINT) (aplNELMRht - ((aplNELMRht1 - 1) << LOG2NBIB));
        else
            uNBIB = NBIB;

        // Loop through the bits in the byte
        while (TRUE)
        {
            if (uVal & uBitMask)
                lpMemRes[aplOff1++] = (uRht << LOG2NBIB) + uBitIndex;
            else
                lpMemRes[aplOff0++] = (uRht << LOG2NBIB) + uBitIndex;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
                uBitMask = BIT0;            // Start over

            // Check for end-of-byte
            if (++uBitIndex EQ uNBIB)
            {
                uBitIndex = 0;              // Start over
                break;
            } // End IF
        } // End WHILE
    } // End FOR

    return TRUE;
ERROR_EXIT:
    return FALSE;
} // End BoolVecGrade


//***************************************************************************
//  $PrimFnDydGradeCommon_EM_YY
//
//  Primitive function for dyadic DelStile and DeltaStile
//    ("grade up/down character")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydGradeCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydGradeCommon_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis,              // Ptr to axis token (may be NULL)
     UBOOL   bRavelArg)             // TRUE iff we're to treat the right arg as ravelled

{
    APLSTYPE      aplTypeLft;               // Left arg storage type
    APLNELM       aplNELMLft,               // Left arg NELM
                  aplNELMRht,               // Right ...
                  aplNELMRes;               // Result    ...
    APLRANK       aplRankLft,               // Left arg rank
                  aplRankRht;               // Right ...
    HGLOBAL       hGlbLft = NULL,           // Left arg global memory handle
                  hGlbRht = NULL,           // Right ...
                  hGlbRes = NULL;           // Result   ...
    LPVOID        lpMemLft = NULL,          // Ptr to left arg global memory
                  lpMemRht = NULL,          // Ptr to right ...
                  lpMemRes = NULL;          // Ptr to result   ...
    LPAPLDIM      lpMemDimLft,              // Ptr to left arg dimensions
                  lpMemDimRht;              // Ptr to right ...
    APLUINT       ByteRes,                  // # bytes in the result
                  uDim,                     // Loop counter
                  uRes,                     // Loop counter
                  uBegLen,                  // Product of leading dimensions
                  uBeg,                     // Loop counter
                  uEndLen,                  // Product of trailing dimensions
                  uEnd;                     // Loop counter
    APLINT        iLft;                     // Loop counter
    LPPL_YYSTYPE  lpYYRes = NULL;           // Ptr to the result
    APLBOOL       bQuadIO;                  // []IO
    GRADE_DATA    gradeData;                // Data passed to GRADE_ROUTINE
    HGLOBAL       hGlbTTHandles = NULL;     // TT Handles global memory handle
    LPTT_HANDLES  lpMemTTHandles = NULL;    // Ptr to TT handles global memory
    LPPLLOCALVARS lpplLocalVars;            // Ptr to re-entrant vars

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    gradeData.lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Mark as grade up or down
    gradeData.iMul = (lptkFunc->tkData.tkChar EQ UTF16_DELTASTILE) ? 1 : -1;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &gradeData.aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for LEFT RANK ERROR
    if (IsScalar (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for RIGHT RANK ERROR
    if (IsScalar (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for LEFT LENGTH ERROR
    if (IsEmpty (aplNELMLft))
        goto LEFT_LENGTH_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsSimpleChar (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if (!IsSimpleChar (gradeData.aplTypeRht)
     && !IsEmpty (aplNELMRht))
        goto RIGHT_DOMAIN_EXIT;

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Allocate an array to hold the HGLOBALs of the translate tables
    hGlbTTHandles = DbgGlobalAlloc (GHND, (APLU3264) aplRankLft * sizeof (TT_HANDLES));
    if (!hGlbTTHandles)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemTTHandles = MyGlobalLock (hGlbTTHandles);

    // Allocate 16-bit arrays of APLCHARs for the translate tables,
    //   one per left arg dimension
    for (uDim = 0; uDim < aplRankLft; uDim++)
    {
        // Allocate space for the TT -- note we don't use GHND (which includes GMEM_ZEROINIT)
        //    as we'll initialize it ourselves
        lpMemTTHandles[uDim].hGlbTT = DbgGlobalAlloc (GMEM_MOVEABLE, APLCHAR_SIZE * sizeof (APLCHAR));
        if (!lpMemTTHandles[uDim].hGlbTT)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemTTHandles[uDim].lpMemTT = MyGlobalLock (lpMemTTHandles[uDim].hGlbTT);

        // Fill with 0xFFs as identity element for min comparisons
        FillMemory (lpMemTTHandles[uDim].lpMemTT, APLCHAR_SIZE * sizeof (APLCHAR), 0xFF);
    } // End FOR

    // Save in gradeData
    gradeData.lpMemTTHandles = lpMemTTHandles;
    gradeData.aplRankLft     = aplRankLft;

    // Skip over the header to the dimensions
    lpMemDimLft = VarArrayBaseToDim (lpMemLft);

    // Skip over the header and dimensions to the data
    lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

    // Convert the left arg into multiple TTs
    for (uEndLen = 1, iLft = aplRankLft - 1; iLft >= 0; iLft--)
    {
        APLUINT   uDimLen;      // Length of dimension iLft
        APLCHAR   aplChar;      // The next char
        LPAPLCHAR lpMemTT;      // Ptr to current TT

        // Get a ptr to the current TT
        lpMemTT = lpMemTTHandles[iLft].lpMemTT;

        // Get the dimension length
        uDimLen = lpMemDimLft[iLft];

        Assert (uDimLen <= APLCHAR_SIZE);

        // Calculate product of the leading dimensions
        uBegLen = aplNELMLft / (uDimLen * uEndLen);

        // Run through the left arg along the iLft dimension
        //   (of length lpMemDimLft[iLft])
        //   filling in the elements of the corresponding TT.
        for (uBeg = 0; uBeg < uBegLen; uBeg++)
        for (uDim = 0; uDim < uDimLen; uDim++)
        for (uEnd = 0; uEnd < uEndLen; uEnd++)
        {
            // Check for Ctrl-Break
            if (*gradeData.lpbCtrlBreak)
                goto ERROR_EXIT;

            // Get the next char
            aplChar = ((LPAPLCHAR) lpMemLft)[uEnd + uEndLen * (uDimLen * uBeg + uDim)];

            // Compare with existing index
            lpMemTT[aplChar] = min (lpMemTT[aplChar], (APLCHAR) uDim);
        } // End FOR/FOR

        // Calculate product of trailing dimensions
        uEndLen *= uDimLen;
    } // End FOR

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // If we're ravelling the right arg, ...
    if (bRavelArg)
    {
        // Get the NELM of the right arg (as result length)
        gradeData.aplNELMRht = aplNELMRes = aplNELMRht;

        // Calculate the length of the dimensions after the first one
        gradeData.aplNELMRest = 1;
    } else
    {
        // Get the length of the first dimension (as result length)
        gradeData.aplNELMRht = aplNELMRes = lpMemDimRht[0];

        // Calculate the length of the dimensions after the first one
        for (gradeData.aplNELMRest = 1, uRes = 1; uRes < aplRankRht; uRes++)
            gradeData.aplNELMRest *= lpMemDimRht[uRes];
    } // End IF/ELSE

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_INT, aplNELMRes, 1);

    // Allocate space for the result.
    // N.B. Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (APLU3264) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Save the dimension in the result
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // If the right arg is an APA, ...
    if (IsSimpleAPA (gradeData.aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        gradeData.apaOffRht = lpAPA->Off;
        gradeData.apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Initialize the result with {iota}aplNELMRes (origin-0)
    for (uRes = 0; uRes < aplNELMRes; uRes++)
        ((LPAPLINT) lpMemRes)[uRes] = uRes;

    // Grade the array
    if (!GRADE_ROUTINE (lpMemRes,               // Ptr to result global memory
                        lpMemRht,               // Ptr to right arg global memory
                        aplNELMRes,             // Result NELM
                       &PrimFnGradeCompare,     // Ptr to comparison routine
                        gradeData.lpbCtrlBreak, // Ptr to Ctrl-Break flag
                       &gradeData))             // Ptr to extra grade data
        goto ERROR_EXIT;
    // Add in []IO
    for (uRes = 0; uRes < aplNELMRes; uRes++)
        ((LPAPLINT) lpMemRes)[uRes] += bQuadIO;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LEFT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
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

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbTTHandles)
    {
        if (lpMemTTHandles EQ NULL)
            // Lock the memory to get a ptr to it
            lpMemTTHandles = MyGlobalLock (hGlbTTHandles);

        for (uDim = 0; uDim < aplRankLft; uDim++)
        if (lpMemTTHandles[uDim].hGlbTT)
        {
            if (lpMemTTHandles[uDim].lpMemTT)
            {
                // We no longer need this ptr
                MyGlobalUnlock (lpMemTTHandles[uDim].hGlbTT); lpMemTTHandles[uDim].lpMemTT = NULL;
            } // End IF

            // We no longer need this storage
            DbgGlobalFree (lpMemTTHandles[uDim].hGlbTT); lpMemTTHandles[uDim].hGlbTT = NULL;
        } // End FOR/IF

        // We no longer need this ptr
        MyGlobalUnlock (hGlbTTHandles); lpMemTTHandles = NULL;

        // We no longer need this storage
        DbgGlobalFree (hGlbTTHandles); hGlbTTHandles = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnDydGradeCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnGradeCompare
//
//  Comparison routine for PrimFnMonGradeCommon_EM_YY & PrimFnDydGradeCommon_EM_YY
//
//  Return Signum (Lft - Rht)
//***************************************************************************

APLINT PrimFnGradeCompare
    (LPVOID       lpMemRht,         // Ptr to right arg array
     APLUINT      aplUIntLft,       // Left index
     APLUINT      aplUIntRht,       // Right index
     LPGRADE_DATA lpGradeData)      // Ptr to extra data

{
    APLUINT      uRest,             // Loop counter
                 uDim;              // Loop counter
    APLINT       iRest;             // Loop counter
    APLNELM      aplNELMRest;       // Loop limit
    APLRANK      aplRankLft;        // Loop limit
    UBOOL        bSame,             // TRUE if the two planes are the same
                 bToggle;           // Toggle value between chars
    LPTT_HANDLES lpMemTTHandles;    // Ptr to TT handles global memory
    LPAPLCHAR    lpMemTT;           // Ptr to current TT

    // Get the product of all but the first dimension
    aplNELMRest = lpGradeData->aplNELMRest;

    // Split cases based upon the right arg storage type
    switch (lpGradeData->aplTypeRht)
    {
        case ARRAY_BOOL:
            // Compare the hyper-planes of the right arg
            for (uRest = 0; uRest < aplNELMRest; uRest++)
            {
                APLUINT aplBitLft,              // Left arg bit value
                        aplBitRht;              // Right ...
                UINT    uBitMask;               // Left & right arg bit mask

                // Get the left hand indexed bit
                aplBitLft = aplUIntLft * aplNELMRest + uRest;
                uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) aplBitLft);
                aplBitLft = (uBitMask & ((LPAPLBOOL) lpMemRht)[aplBitLft >> LOG2NBIB]) ? TRUE : FALSE;

                // Get the right hand indexed bit
                aplBitRht = aplUIntRht * aplNELMRest + uRest;
                uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) aplBitRht);
                aplBitRht = (uBitMask & ((LPAPLBOOL) lpMemRht)[aplBitRht >> LOG2NBIB]) ? TRUE : FALSE;

                // Split cases based upon the signum of the difference
                switch (PrimFnMonTimesIisI (aplBitLft - aplBitRht,
                                            NULL))
                {
                    case 1:
                        return  1 * lpGradeData->iMul;

                    case 0:
                        break;

                    case -1:
                        return -1 * lpGradeData->iMul;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            // The hyper-planes are equal -- compare indices so the sort is stable
            return PrimFnMonTimesIisI (aplUIntLft - aplUIntRht, NULL);

        case ARRAY_INT:
            // Compare the hyper-planes of the right arg
            for (uRest = 0; uRest < aplNELMRest; uRest++)
            // Split cases based upon the signum of the difference
            switch (PrimFnMonTimesIisI (((LPAPLINT) lpMemRht)[aplUIntLft * aplNELMRest + uRest]
                                      - ((LPAPLINT) lpMemRht)[aplUIntRht * aplNELMRest + uRest],
                                        NULL))
            {
                case 1:
                    return  1 * lpGradeData->iMul;

                case 0:
                    break;

                case -1:
                    return -1 * lpGradeData->iMul;

                defstop
                    break;
            } // End FOR/SWITCH

            // The hyper-planes are equal -- compare indices so the sort is stable
            return PrimFnMonTimesIisI (aplUIntLft - aplUIntRht, NULL);

        case ARRAY_FLOAT:
            // Compare the hyper-planes of the right arg
            for (uRest = 0; uRest < aplNELMRest; uRest++)
            // Split cases based upon the signum of the difference
            switch (PrimFnMonTimesIisF (((LPAPLFLOAT) lpMemRht)[aplUIntLft * aplNELMRest + uRest]
                                      - ((LPAPLFLOAT) lpMemRht)[aplUIntRht * aplNELMRest + uRest],
                                        NULL))
            {
                case 1:
                    return  1 * lpGradeData->iMul;

                case 0:
                    break;

                case -1:
                    return -1 * lpGradeData->iMul;

                defstop
                    break;
            } // End FOR/SWITCH

            // The hyper-planes are equal -- compare indices so the sort is stable
            return PrimFnMonTimesIisI (aplUIntLft - aplUIntRht, NULL);

        case ARRAY_APA:
            // Compare the hyper-planes of the right arg
            for (uRest = 0; uRest < aplNELMRest; uRest++)
            // Split cases based upon the signum of the difference
////////////switch (PrimFnMonTimesIisI ((lpGradeData->apaOffRht + lpGradeData->apaMulRht * (aplUIntLft * aplNELMRest + uRest))
////////////                          - (lpGradeData->apaOffRht + lpGradeData->apaMulRht * (aplUIntRht * aplNELMRest + uRest)),
////////////                            NULL))
            switch (PrimFnMonTimesIisI (lpGradeData->apaMulRht * (aplUIntLft - aplUIntRht),
                                        NULL))
            {
                case 1:
                    return  1 * lpGradeData->iMul;

                case 0:
                    break;

                case -1:
                    return -1 * lpGradeData->iMul;

                defstop
                    break;
            } // End FOR/SWITCH

            // The hyper-planes are equal -- compare indices so the sort is stable
            return PrimFnMonTimesIisI (aplUIntLft - aplUIntRht, NULL);

        case ARRAY_CHAR:
            // Get # dimensions in left arg (# TTs)
            aplRankLft = lpGradeData->aplRankLft;

            // Get ptr to array of TTs
            lpMemTTHandles = lpGradeData->lpMemTTHandles;

            // Initialize vars
            bSame = bToggle = TRUE;

            // Loop through the TTs
            for (uDim = 0; uDim < aplRankLft; uDim++)
            {
                // Get a ptr to the current TT
                lpMemTT = lpMemTTHandles[uDim].lpMemTT;

                // Compare the hyper-planes of the right arg
                for (iRest = aplNELMRest - 1; iRest >= 0; iRest--)
                {
                    APLCHAR aplCharLft,         // Left char
                            aplCharRht,         // Right ...
                            aplIndLft,          // Left char index
                            aplIndRht;          // Right ...

                    // Get the left & right CHARs
                    aplCharLft = ((LPAPLCHAR) lpMemRht)[aplUIntLft * aplNELMRest + iRest];
                    aplCharRht = ((LPAPLCHAR) lpMemRht)[aplUIntRht * aplNELMRest + iRest];

                    aplIndLft = lpMemTT[aplCharLft];
                    aplIndRht = lpMemTT[aplCharRht];

                    // If the indices are different, ...
                    if (aplIndLft NE aplIndRht)
                    {
                        // Mark as different
                        bSame = FALSE;

                        // Calculate toggle state
                        if (bToggle EQ (UBOOL) (aplIndLft > aplIndRht))
                            bToggle = !bToggle;
                    } // End IF
                } // End FOR
            } // End FOR

            if (bSame)
                // The hyper-planes are equal -- compare indices so the sort is stable
                return PrimFnMonTimesIisI (aplUIntLft - aplUIntRht, NULL);
            else
                return (bToggle ? -1 : 1) * lpGradeData->iMul;
        defstop
            break;
    } // End SWITCH

    // We should never get here
    DbgStop ();

    return 0;
} // End PrimFnGradeCompare


//***************************************************************************
//  End of File: pf_deltastile.c
//***************************************************************************
