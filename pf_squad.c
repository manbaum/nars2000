//***************************************************************************
//  NARS2000 -- Primitive Function -- Squad
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
//  $PrimFnSquad_EM_YY
//
//  Primitive function for monadic and dyadic Squad (ERROR and "rectangular indexing")
//***************************************************************************

LPPL_YYSTYPE PrimFnSquad_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_SQUAD);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonSquad_EM_YY             (lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydSquad_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnSquad_EM_YY


//***************************************************************************
//  $PrimProtoFnSquad_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Squad
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnSquad_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnSquad_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnSquad_EM_YY,  // Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnSquad_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonSquad_EM_YY
//
//  Primitive function for monadic Squad (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonSquad_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonSquad_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc);
} // End PrimFnMonSquad_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSquad_EM_YY
//
//  Primitive function for dyadic Squad ("rectangular indexing")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSquad_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydSquad_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLLONGEST   aplLongestRht;     // The value of the right arg
    APLSTYPE     aplTypeLft;        // The storage type of the left arg
    APLNELM      aplNELMLft;        // The # elements in the left arg
    APLRANK      aplRankLft;        // The rank of the left arg
    IMM_TYPES    immTypeRht;        // The immediate type of the right arg (see IMM_TYPES)
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        HGLOBAL hGlbRht;

        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // Get the right arg global memory handle
                hGlbRht = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbRht));

                // Clear the ptr type bits
                hGlbRht = ClrPtrTypeDir (hGlbRht);

                return PrimFnDydSquadGlb_EM_YY
                       (lptkLftArg,         // Ptr to left arg token
                       &hGlbRht,            // Ptr to right arg global memory handle
                        lptkAxis,           // Ptr to axis token (may be NULL)
                        lptkFunc,           // Ptr to function token
                        NULL,               // Ptr to result global memory handle
                        NULL);              // Ptr to set arg token
            } // End IF

            // Handle the immediate case

            // Get the immediate value & type
            aplLongestRht = lptkRhtArg->tkData.tkSym->stData.stLongest;
            immTypeRht    = lptkRhtArg->tkData.tkSym->stFlags.ImmType;

            break;

        case TKT_VARIMMED:
            // Get the immediate value & type
            aplLongestRht = lptkRhtArg->tkData.tkLongest;
            immTypeRht    = lptkRhtArg->tkFlags.ImmType;

            break;


        case TKT_VARARRAY:
            // Get the right arg global memory handle
            hGlbRht = lptkRhtArg->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbRht));

            // Clear the ptr type bits
            hGlbRht = ClrPtrTypeDir (hGlbRht);

            return PrimFnDydSquadGlb_EM_YY
                   (lptkLftArg,         // Ptr to left arg token
                   &hGlbRht,            // Ptr to right arg global memory handle
                    lptkAxis,           // Ptr to axis token (may be NULL)
                    lptkFunc,           // Ptr to function token
                    NULL,               // Ptr to result global memory handle
                    NULL);              // Ptr to set arg token
        defstop
            return NULL;
    } // End SWITCH

    // Common immediate case, value in <aplLongestRht>

    // The only allowed left arg is an empty vector

    // Get the attributes (Type, NELM, and Rank) of the left arg
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);

    // Check for RANK ERROR
    if (!IsVector (aplRankLft))
        goto RANK_EXIT;

    // Check for LENGTH ERROR
    if (!IsEmpty (aplNELMLft))
        goto LENGTH_EXIT;

    // Check for DOMAIN ERROR
    if (!IsSimpleNH (aplTypeLft))
        goto DOMAIN_EXIT;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = immTypeRht;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkLongest  = aplLongestRht;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnDydSquad_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSquadGlb_EM_YY
//
//  Dyadic Squad ("rectangular indexing") on a right arg global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSquadGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydSquadGlb_EM_YY
    (LPTOKEN    lptkLftArg,             // Ptr to left arg token
     HGLOBAL   *lphGlbRht,              // Ptr to right arg global memory handle
     LPTOKEN    lptkAxis,               // Ptr to axis token (may be NULL)
     LPTOKEN    lptkFunc,               // Ptr to function token
     HGLOBAL   *lphGlbRes,              // Ptr to result global memory handle (may be NULL if not assignment)
     LPTOKEN    lptkSetArg)             // Ptr to set arg token (may be NULL if not assignment)

{
    APLSTYPE      aplTypeLft,           // Left arg storage type of the left arg
                  aplTypeRht,           // Right ...
                  aplTypeRes,           // Result   ...
                  aplTypeSet,           // Set  ...
                  aplTypePro;           // Promoted ...
    APLNELM       aplNELMLft,           // Left arg NELM
                  aplNELMRht,           // Right ...
                  aplNELMAxis,          // Axis ...
                  aplNELMRes,           // Result   ...
                  aplNELMSet;           // Set  ...
    APLRANK       aplRankLft,           // Left arg rank
                  aplRankRht,           // Right ...
                  aplRankRes,           // Result   ...
                  aplRankSet,           // Set  ...
                  aplRankN1Res,         // Result rank without length 1 dimensions
                  aplRankN1Set;         // Set    ...
    HGLOBAL       hGlbLft = NULL,       // Left arg global memory handle
                  hGlbRht = NULL,       // Right ...
                  hGlbRht2 = NULL,      // ...
                  hGlbAxis = NULL,      // Axis ...
                  hGlbSet = NULL,       // Set  ...
                  hGlbSubSet = NULL,    // Set arg item ...
                  hGlbRes = NULL,       // Result   ...
                  hGlbOdo = NULL;       // Odometer ...
    LPVOID        lpMemLft = NULL,      // Ptr to left arg global memory
                  lpMemRht = NULL,      // Ptr to right ...
                  lpMemRes = NULL,      // Ptr to result   ...
                  lpMemSet = NULL;      // Ptr to set  ...
    APLUINT       uLft,                 // Loop counter
                  uRht,                 // Loop counter
                  ByteRes,              // # bytes in the result
                  uWVal,                // Weighting value
                  aplIntAcc,            // Accumulation value
                  uRes,                 // Loop counter
                  uSet,                 // Loop counter
                  uDimSet,              // Loop counter
                  uSub,                 // Loop counter
                  aplIndexSet;          // Index into set arg
    APLINT        iLft,                 // Loop counter
                  iAxisNxt;             // Index of next axis value
    LPAPLUINT     lpMemAxis = NULL,     // Ptr to axis global memory
                  lpMemAxisAct,         // Ptr to actual axis values
                  lpMemAxisLst,         // Ptr to (last + 1) of actual axis values
                  lpMemAxisEli,         // Ptr to elided axis values
                  lpMemOdo = NULL,      // Ptr to odometer global memory
                  lpMemLimLft;          // Ptr to left arg limit vector
    LPAPLDIM      lpMemDimRht,          // Ptr to right arg dimensions
                  lpMemDimRes,          // Ptr to result    ...
                  lpMemDimSet,          // Ptr to set   ...
                  lpMemDimSub;          // Ptr to sub   ...
    APLLONGEST    aplLongestLft,        // Left arg as immediate value
                  aplLongestSet;        // Set  ...
    UINT          uBitMask;             // Bit mask when looping through Booleans
    UBOOL         bRet = TRUE;          // TRUE iff result is valid
    APLBOOL       bQuadIO;              // []IO
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to the result
    LPSYMENTRY    lpSymTmp;             // Ptr to temporary LPSYMENTRY
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get the right arg global memory handle
    hGlbRht = *lphGlbRht;

    // Get the attributes (Type, NELM, and Rank) of the left, right, and set args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfGlb   (hGlbRht   , &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);
    if (lptkSetArg)
        AttrsOfToken (lptkSetArg, &aplTypeSet, &aplNELMSet, &aplRankSet, NULL);

    //***************************************************************
    // Check for axis present
    //  Even if it's not present, force a full axis vector result
    //***************************************************************
    // Check the axis values, fill in # elements in axis
    if (!CheckAxis_EM (lptkAxis,        // The axis token
                       aplRankRht,      // All values less than this
                       FALSE,           // TRUE iff scalar or one-element vector only
                       TRUE,            // TRUE iff want sorted axes
                       FALSE,           // TRUE iff axes must be contiguous
                       FALSE,           // TRUE iff duplicate axes are allowed
                       NULL,            // TRUE iff fractional values allowed
                       NULL,            // Return last axis value
                      &aplNELMAxis,     // Return # elements in axis vector
                      &hGlbAxis))       // Return HGLOBAL with APLUINT axis values
        goto ERROR_EXIT;

    // Check for LENGTH ERROR
    if (aplNELMLft NE aplNELMAxis)
        goto LENGTH_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (IsSimpleCH (aplTypeLft))
        goto DOMAIN_EXIT;

    // Calc result storage type
    if (IsSimpleAPA (aplTypeRht))
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeRht;

    // Get left arg global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);

    // Skip over the header and dimensions to the data
    if (aplTypeLft EQ ARRAY_LIST)
        lpMemLft = LstArrayBaseToData (lpMemLft);
    else
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Initialize NELM and rank to their identity elements
    aplNELMRes = 1;
    aplRankRes = aplRankN1Res = 0;

    //***************************************************************
    // Trundle through the left arg accumulating the
    //   sum of ranks and product of NELMs
    // Note that for simple NH arrays, the NELM and Rank above are all we need
    //***************************************************************
    if (!IsSimpleNH (aplTypeLft))   // Meaning we accept '' as a valid left arg
    {
        HGLOBAL  hGlbSub;       // Left arg item global memory handle
        APLSTYPE aplTypeSub;    // Left arg item storage type
        APLNELM  aplNELMSub;    // Left arg item NELM
        APLRANK  aplRankSub;    // Left arg item rank

        // Check for empty left arg
        if (IsEmpty (aplNELMLft))
        {
            // The prototype for the left arg must be
            //   a nested numeric array or an empty list
            if (!IsGlbTypeLstDir_PTB (MakePtrTypeGlb (hGlbLft)))
            {
                // Get next value from the left arg
                GetNextValueMem (lpMemLft,      // Ptr to left arg global memory
                                 aplTypeLft,    // Left arg storage type
                                 0,             // Left arg index
                                &hGlbSub,       // Left arg item LPSYMENTRY or HGLOBAL (may be NULL)
                                 NULL,          // Ptr to left arg immediate value
                                 NULL);         // Ptr to left arg immediate type
                // The item must be a global
                Assert (GetPtrTypeDir (hGlbSub) EQ PTRTYPE_HGLOBAL);

                // Get the global attrs
                AttrsOfGlb (hGlbSub, &aplTypeSub, &aplNELMSub, NULL, NULL);

                // Check for DOMAIN ERROR
                if (!IsSimpleNum (aplTypeSub))
                    goto DOMAIN_EXIT;
            } // End IF
        } else
        for (uLft = 0; uLft < aplNELMLft; uLft++)
        {
            APLLONGEST aplLongestSub;           // Left arg item immediate value
            IMM_TYPES  immTypeSub;              // Left arg item immediate type

            // Get next value from the left arg
            GetNextValueMem (lpMemLft,          // Ptr to left arg global memory
                             aplTypeLft,        // Left arg storage type
                             uLft,              // Left arg index
                            &hGlbSub,           // Left arg item LPSYMENTRY or HGLOBAL (may be NULL)
                            &aplLongestSub,     // Ptr to left arg immediate value
                            &immTypeSub);       // Ptr to left arg immediate type
            // If the left arg item is a global, ...
            if (hGlbSub)
            {
                // Get the global attrs
                AttrsOfGlb (hGlbSub, &aplTypeSub, &aplNELMSub, &aplRankSub, NULL);

                // Ensure the item is simple numeric
                if (!IsSimpleNum (aplTypeSub))
                    goto DOMAIN_EXIT;

                // Accumulate the NELM & rank
                aplNELMRes *= aplNELMSub;
                aplRankRes += aplRankSub;

                // If we're assigning, count non-length 1 dimensions for <aplRankN1Res>
                if (lptkSetArg && aplRankSub)
                {
                    // Lock the memory to get a ptr to it
                    lpMemDimSub = MyGlobalLock (ClrPtrTypeDir (hGlbSub));

                    // Skip over the header to the dimensions
                    lpMemDimSub = VarArrayBaseToDim (lpMemDimSub);

                    // Loop through the dimensions counting the non-length 1 dimensions
                    for (uSub = 0; uSub < aplRankSub; uSub++)
                        aplRankN1Res +=  !IsUnitDim (*lpMemDimSub++);

                    // We no longer need this ptr
                    MyGlobalUnlock (ClrPtrTypeDir (hGlbSub)); lpMemDimSub = NULL;
                } // End IF
            } else
            // The left arg item is immediate (in <aplLongestSub> and of type <immTypeSub>)
            {
                // Ensure the left arg item is simple numeric
                if (!IsImmNum (immTypeSub))
                    goto DOMAIN_EXIT;

                // Accumulate the NELM & rank
                aplNELMRes   *= 1;          // No action:  compiler will eliminate
                aplRankRes   += 0;          // ...
                aplRankN1Res += 0;          // ...
            } // End IF/ELSE
        } // End FOR
    } // End IF

    // Handle elided axes

    // Lock the memory to get a ptr to it
    lpMemAxis = MyGlobalLock (hGlbAxis);

    // Loop through the elided axes
    for (uRes = 0; uRes < (aplRankRht - aplNELMAxis); uRes++)
    {
        // Accumulate the NELM & rank
        aplNELMRes   *= lpMemDimRht[lpMemAxis[uRes]];
        aplRankRes   += 1;
        aplRankN1Res += !IsUnitDim (lpMemDimRht[lpMemAxis[uRes]]);
    } // End FOR

    // If we're not assigning, ...
    if (lptkSetArg EQ NULL)
    {
        //***************************************************************
        // Calculate space needed for the result
        //***************************************************************
        ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

        //***************************************************************
        // Now we can allocate the storage for the result
        // N.B.:  Conversion from APLUINT to UINT.
        //***************************************************************
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
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMRes;
        lpHeader->Rank       = aplRankRes;
#undef  lpHeader

        // Skip over the header to the dimensions
        lpMemDimRes = VarArrayBaseToDim (lpMemRes);
    } else
    // We are assigning
    {
        // Initialize
        aplRankN1Set = 0;

        // Get set arg global ptrs
        aplLongestSet = GetGlbPtrs_LOCK (lptkSetArg, &hGlbSet, &lpMemSet);

        if (lpMemSet)
        {
            // Skip over the header to the dimensions
            lpMemDimSet = VarArrayBaseToDim (lpMemSet);

            // Loop through the dimensions counting the non-length 1 dimensions
            for (uSet = 0; uSet < aplRankSet; uSet++)
                aplRankN1Set +=  !IsUnitDim (lpMemDimSet[uSet]);
        } // End IF

        // Check for RANK ERROR between the result arg & set arg ranks
        if (!IsSingleton (aplNELMSet)
         && aplRankN1Res NE aplRankN1Set)
            goto RANK_EXIT;
    } // End IF/ELSE

    //***************************************************************
    // Calculate space needed for odometer and limit vectors
    //***************************************************************
    ByteRes = aplRankRht * sizeof (APLUINT) * 2;

    //***************************************************************
    // Now we can allocate the storage for the odometer & limit vectors
    // N.B.:  Conversion from APLUINT to UINT.
    //***************************************************************
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;
    hGlbOdo = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbOdo)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemOdo = MyGlobalLock (hGlbOdo);

    // Calc ptr to limit vector
    lpMemLimLft = &lpMemOdo[aplRankRht];

    // Calc ptr to first of actual axes
    lpMemAxisAct = &lpMemAxis[aplRankRht - aplNELMAxis];

    // Calc ptr to (last + 1) of actual dimensions
    lpMemAxisLst = &lpMemAxis[aplRankRht];

    // Initialize axis index
    iAxisNxt = uLft = 0;

    // Initialize set arg dimension loop counter
    uDimSet = 0;

    //***************************************************************
    // Copy dimensions from the right arg or left arg items
    //    to the result
    //***************************************************************
#define uAxisNxt    ((APLUINT) iAxisNxt)
    while (uAxisNxt < aplRankRht)
    {
        if (aplNELMAxis EQ aplRankRht       // No or full axis operator
         || (lpMemAxisAct < lpMemAxisLst    //   or ptr is within range
          && uAxisNxt EQ *lpMemAxisAct))    //     and next axis value is an actual one
        {
            HGLOBAL hGlbSub;

            // Skip over the actual axis
            lpMemAxisAct++;

            // If the left arg is a global, ...
            if (hGlbLft)
            {
                IMM_TYPES immTypeSub;           // Item immediate type

                // Get next value from the left arg
                GetNextValueMem (lpMemLft,      // Ptr to left arg global memory
                                 aplTypeLft,    // Left arg storage type
                                 uLft++,        // Left arg index
                                &hGlbSub,       // Left arg item LPSYMENTRY or HGLOBAL (may be NULL)
                                 NULL,          // Ptr to left arg immediate value (may be NULL)
                                &immTypeSub);   // Ptr to left arg immediate type (may be NULL)
                                                // The above ptr is needed (but unused) to force
                                                //   GetNextValueMem to return hGlbSub as an HGLOBAL only
                // If the left arg item is a global, ...
                if (hGlbSub)
                {
                    APLNELM  aplNELMSub;
                    APLRANK  aplRankSub;
                    LPAPLDIM lpMemSub;
                    UBOOL    bRet = TRUE;       // TRUE iff the result is valid

                    // Get the global attrs
                    AttrsOfGlb (hGlbSub, NULL, &aplNELMSub, &aplRankSub, NULL);

                    // Lock the memory to get a ptr to it
                    lpMemSub = MyGlobalLock (ClrPtrTypeDir (hGlbSub));

                    // Skip over the header to the dimensions
                    lpMemSub = VarArrayBaseToDim (lpMemSub);

                    // If we're not assigning, ...
                    if (lptkSetArg EQ NULL)
                        // Fill in the result's dimension
                        CopyMemory (lpMemDimRes, lpMemSub, (APLU3264) aplRankSub * sizeof (APLDIM));
                    else
                    // We are assigning
                    // Loop through the sub-item dimensions
                    for (uSub = 0; uSub < aplRankSub; uSub++, uDimSet++)
                    {
                        // Skip over length 1 dimensions in the sub-item
                        while (uSub < aplRankSub
                            && IsUnitDim (lpMemSub[uSub]))
                            uSub++;

                        // Skip over length 1 dimensions in the set arg
                        while (uDimSet < aplRankSet
                            && IsUnitDim (lpMemDimSet[uDimSet]))
                            uDimSet++;

                        // Compare the left arg item & set arg dimensions
                        if (uSub    < aplRankSub
                         && uDimSet < aplRankSet
                         && lpMemSub[uSub] NE lpMemDimSet[uDimSet])
                        {
                            // Mark as an error (LENGTH)
                            bRet = FALSE;

                            break;
                        } // End IF
                    } // End IF/ELSE/FOR

                    // We no longer need this ptr
                    MyGlobalUnlock (ClrPtrTypeDir (hGlbSub)); lpMemSub = NULL;

                    // Check for error
                    if (!bRet)
                        goto LENGTH_EXIT;

                    // If we're not assigning, ...
                    if (lptkSetArg EQ NULL)
                        // Skip over the copied dimensions
                        lpMemDimRes += aplRankSub;

                    // Fill in the left arg item limit
                    *lpMemLimLft++ = aplNELMSub;
                } else
                // The left arg item is immediate
                    // Fill in the left arg item limit
                    *lpMemLimLft++ = 1;
            } else
            // The left arg is immediate (in <aplLongestLft> of array type <aplTypeLft>
            {
                // No dimensions to copy for scalar left arg

                // Fill in the left arg item limit
                *lpMemLimLft++ = 1;
            } // End IF/ELSE
        } else  // Elided dimension:  use dimension from right arg
        {
            // If we're not assigning, ...
            if (lptkSetArg EQ NULL)
                // Fill in the result's dimension
                *lpMemDimRes++ = lpMemDimRht[uAxisNxt];
            else
            // We are assigning
            {
                // Copy index
                uRht = uAxisNxt;

                // Skip over length 1 dimensions in the right arg
                while (uRht < aplRankRht
                    && IsUnitDim (lpMemDimRht[uRht]))
                    uRht++;

                // Skip over length 1 dimensions in the set arg
                while (uDimSet < aplRankSet
                    && IsUnitDim (lpMemDimSet[uDimSet]))
                    uDimSet++;

                // Compare the right arg & set arg dimensions
                if (uRht    < aplRankRht
                 && uDimSet < aplRankSet
                 && lpMemDimRht[uRht] NE lpMemDimSet[uDimSet++])
                    goto LENGTH_EXIT;
            } // End IF/ELSE

            // Fill in the left arg item limit
            *lpMemLimLft++ = lpMemDimRht[uAxisNxt];
        } // End IF/ELSE

        // Skip to next dimension index
        uAxisNxt++;
    } // End IF/WHILE
#undef  uAxisNxt

    // If we're not assigning, ...
    if (lptkSetArg EQ NULL)
    {
        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

        // If the result is nested or hetero, fill the data with PTR_REUSED
        //   in case we fail along the way
        if (IsSimpleHet (aplTypeRes)
         || IsNested (aplTypeRes))
        {
            // Fill in first one in case of nested prototypes
            if (IsNested (aplTypeRes))
                ((LPAPLNESTED) lpMemRes)[0] = PTR_REUSED;

            // Loop through the rest of the items
            for (uRes = 0; uRes < aplNELMRes; uRes++)
                ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;
        } // End IF
    } else
    // We are assigning
    {
        if (lpMemSet)
        {
            // Skip over the header and dimensions to the data
            lpMemSet = VarArrayBaseToData (lpMemSet, aplRankSet);

            if (IsSingleton (aplNELMSet))
                // Get the first item from the set arg
                GetNextItemMem (lpMemSet,           // Ptr to item global memory data
                                aplTypeSet,         // Item storage type
                                0,                  // Index into item
                               &hGlbSubSet,         // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                               &aplLongestSet);     // Ptr to result immediate value (may be NULL)
            else
                aplIndexSet = 0;
        } // End IF

        // If the right arg should be promoted, ...
        if (QueryPromote (aplTypeRht, aplTypeSet, &aplTypePro))
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemRht = lpMemDimRht = NULL;

            // Promote the right arg
            if (!TypePromoteGlb_EM (&hGlbRht, aplTypePro, lptkFunc))
                goto ERROR_EXIT;

            // Save the new type
            aplTypeRht = aplTypePro;
        } else
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

            // Because this operation changes the named array,
            //   we need to copy the entire array first.  The
            //   caller of this code deletes the old array.
            hGlbRht2 = CopyArray_EM (hGlbRht, lptkSetArg);
            if (!hGlbRht2)
                goto ERROR_EXIT;

            // Save as new global memory handle
            *lphGlbRht = hGlbRht = hGlbRht2;
        } // End IF/ELSE

        // Save the new global memory handle
        if (lphGlbRes)
            *lphGlbRes = hGlbRht;

        // Lock the memory to get a ptr to it
        lpMemRht = MyGlobalLock (hGlbRht);
    } // End IF/ELSE

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Calc ptr to limit vector
    lpMemLimLft = &lpMemOdo[aplRankRht];

    // Calc ptr to last of elided dimensions
    lpMemAxisEli = &lpMemAxis[aplRankRht - aplNELMAxis - 1];

    // Initialize bit mask when looping through Booleans
    uBitMask = BIT0;

    //***************************************************************
    // Calculate each index into the right arg using a weighting value
    //   from the limit vector and copy the corresponding item from
    //   the right arg to the result.
    //***************************************************************
    for (uRes = 0; uRes < aplNELMRes; uRes++)
    {
        HGLOBAL    hGlbSub;
        APLLONGEST aplLongestSub;
        IMM_TYPES  immTypeSub;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Calc ptr to last of actual axes
        lpMemAxisAct = &lpMemAxis[aplRankRht - 1];

        // Initialize axis index
        iAxisNxt = aplRankRht - 1;

        // Initialize left arg last index
        iLft = aplNELMLft - 1;

        // Initialize accumulation and weighting values
        aplIntAcc = 0;
        uWVal = 1;

        // Run through the axis values from back to front
        //   accumulating in <aplIntAcc> an index into the right arg
        //   using the weighting value <uWVal>
#define uAxisNxt    ((APLUINT) iAxisNxt)
        while (iAxisNxt >= 0)
        {
            APLLONGEST aplLongestNxt;
            IMM_TYPES  immTypeNxt;

            // Split cases based upon whether or not the dimension is actual or elided
            if (aplNELMAxis EQ aplRankRht       // No or full axis operator
             || (lpMemAxisAct > lpMemAxisEli    //   or ptr is within range
              && uAxisNxt EQ *lpMemAxisAct))    //     and next axis value is an actual one
            {
                // Skip back over the actual axis
                lpMemAxisAct--;

                // If the left arg is a global, ...
                if (hGlbLft)
                {
                    // Get the array of indices from the left arg
                    GetNextValueMem (lpMemLft,          // Ptr to left arg global memory
                                     aplTypeLft,        // Left arg storage type
                                     iLft--,            // Left arg index
                                    &hGlbSub,           // Left arg item LPSYMENTRY or HGLOBAL (may be NULL)
                                    &aplLongestSub,     // Ptr to left arg immediate value
                                    &immTypeSub);       // Ptr to left arg immediate type
                    // If the left arg item is a global, ...
                    if (hGlbSub)
                    {
                        // The index value is the <lpMemOdo[iAxisNxt]> value in <hGlbSub>
                        GetNextValueGlb (hGlbSub,               // The global memory handle
                                         lpMemOdo[iAxisNxt],    // Index into item
                                         NULL,                  // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                        &aplLongestNxt,         // Ptr to result immediate value (may be NULL)
                                        &immTypeNxt);           // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                        // If the index value is float, attempt to convert it to int
                        if (IsImmFlt (immTypeNxt))
                        {
                            // Attempt to convert the float to an integer using System CT
                            aplLongestNxt = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestNxt, &bRet);
                            if (!bRet)
                                goto DOMAIN_EXIT;
                        } // End IF
                    } else
                    // The left arg item value is immediate
                    //   (in <aplLongestSub> and of immediate type <immTypeSub>)
                    {
                        Assert (lpMemOdo[iAxisNxt] EQ 0);

                        // If the index value is float, attempt to convert it to int
                        if (IsImmFlt (immTypeSub))
                        {
                            // Attempt to convert the float to an integer using System CT
                            aplLongestNxt = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestSub, &bRet);
                            if (!bRet)
                                goto DOMAIN_EXIT;
                        } else
                            aplLongestNxt = aplLongestSub;
                    } // End IF/ELSE
                } else
                // The left arg item is immediate
                //   (in <aplLongestLft> of array type <aplTypeLft>)
                {
                    Assert (lpMemOdo[iAxisNxt] EQ 0);

                    hGlbSub = NULL;
                    aplLongestSub = aplLongestLft;
                    immTypeSub    = TranslateArrayTypeToImmType (aplTypeLft);

                    // If the index value is float, attempt to convert it to int
                    if (IsImmFlt (immTypeSub))
                    {
                        // Attempt to convert the float to an integer using System CT
                        aplLongestNxt = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestSub, &bRet);
                        if (!bRet)
                            goto DOMAIN_EXIT;
                    } else
                        aplLongestNxt = aplLongestSub;
                } // End IF/ELSE

                // Convert to origin-0
                aplLongestNxt -= bQuadIO;
            } else  // Elided dimension:  use dimension from right arg
                // Copy dimension index (origin-0)
                aplLongestNxt = lpMemOdo[iAxisNxt];

            // Check for negative indices [-lpMemDimRht[iAxisNxt], -1]
            if (SIGN_APLLONGEST (aplLongestNxt))
                aplLongestNxt += lpMemDimRht[iAxisNxt];

            // Ensure that the value is within range
            // Note that because <aplLongestNxt> is unsigned,
            //   we don't have to test for negative
            if (aplLongestNxt >= lpMemDimRht[iAxisNxt])
                goto DOMAIN_EXIT;

            // Accumulate the index
            aplIntAcc += uWVal * aplLongestNxt;

            // Shift over the weighting value
            uWVal *= lpMemDimRht[iAxisNxt];

            // Skip back to next dimension index
            iAxisNxt--;
        } // End WHILE

        // If we're not assigning, ...
        if (lptkSetArg EQ NULL)
        {
            // Extract the <aplIntAcc> value from the right arg
            //   and save into the result
            GetNextValueMem (lpMemRht,          // Ptr to right arg global memory
                             aplTypeRht,        // Right arg storage type
                             aplIntAcc,         // Right arg index
                            &hGlbSub,           // Right arg item LPSYMENTRY or HGLOBAL (may be NULL)
                            &aplLongestSub,     // Ptr to right arg immediate value
                            &immTypeSub);       // Ptr to right arg immediate type
            // If the right arg item is a global, ...
            if (hGlbSub)
                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (hGlbSub);
            else
            // The right arg item is immediate
            //   (in <aplLongestSub> of immediate type <immTypeSub>)
            {
                // Split cases based upon the result storage type
                switch (aplTypeRes)
                {
                    case ARRAY_BOOL:
                        // Save in result
                        if (aplLongestSub)
                            *((LPAPLBOOL) lpMemRes) |= uBitMask;

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;            // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte

                        } // End IF

                        break;

                    case ARRAY_INT:
                        // Save in result
                        *((LPAPLINT) lpMemRes)++ = (APLINT) aplLongestSub;

                        break;

                    case ARRAY_FLOAT:
                        // If the right arg item is int, convert it to float
                        if (IsImmInt (immTypeSub))
                            *((LPAPLFLOAT) lpMemRes)++ = (APLFLOAT) (APLINT) aplLongestSub;
                        else
                            *((LPAPLFLOAT) lpMemRes)++ = *(LPAPLFLOAT) &aplLongestSub;
                        break;

                    case ARRAY_CHAR:
                        // Save in result
                        *((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestSub;

                        break;

                    case ARRAY_HETERO:
                    case ARRAY_NESTED:
                        // Save in result
                        *((LPAPLNESTED) lpMemRes)++ =
                        lpSymTmp =
                          MakeSymEntry_EM (immTypeSub,      // Immediate type
                                          &aplLongestSub,   // Ptr to immediate value
                                           lptkFunc);       // Ptr to function token
                        if (!lpSymTmp)
                            goto ERROR_EXIT;
                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End IF/ELSE
        } else
        // We are assigning
        {
            // Get the next item from the set arg
            if (!IsSingleton (aplNELMSet))
                GetNextItemMem (lpMemSet,               // Ptr to item global memory data
                                aplTypeSet,             // Item storage type
                                aplIndexSet++,          // Index into item
                               &hGlbSubSet,             // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                               &aplLongestSet);         // Ptr to result immediate value (may be NULL)
            // Replace the <aplIntAcc> element in hGlbRht
            //   with <aplLongestSet> or <hGlbSubSet> depending upon <aplTypeRht>
            if (!ArrayIndexReplace_EM (aplTypeRht,      // Right arg storage type
                                       lpMemRht,        // Ptr to right arg global memory
                                       aplIntAcc,       // Index into right arg
                                       aplTypeSet,      // Set arg storage type
                                       aplLongestSet,   // Set arg immediate value
                                       hGlbSubSet,      // Set arg global memory handle
                                       lptkFunc))       // Ptr to function token
                goto ERROR_EXIT;
        } // End IF/ELSE

        // Increment the odometer in lpMemOdo subject to
        //   the values in lpMemLimLft
        IncrOdometer (lpMemOdo, lpMemLimLft, NULL, aplRankRht);
    } // End FOR

    // If we're not assigning, ...
    if (lptkSetArg EQ NULL)
    {
        // Unlock the result global memory in case TypeDemote actually demotes
        if (hGlbRes && lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        // See if it fits into a lower (but not necessarily smaller) datatype
        TypeDemote (&lpYYRes->tkToken);
    } else
        // We are assigning
        // Return pseudo-value indicating success
        lpYYRes = PTR_REUSED;

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
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

        // If this handle is the same as *lphGlbRes, zap that value, too
        if (lphGlbRes && *lphGlbRes EQ hGlbRes)
            *lphGlbRes = NULL;

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF

    if (hGlbRht2)
    {
        if (lpMemRht)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht2); lpMemRht = NULL;
        } // End IF

        // If this handle is the same as *lphGlbRes, zap that value, too
        if (lphGlbRes && *lphGlbRes EQ hGlbRht2)
            *lphGlbRes = NULL;

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRht2); *lphGlbRht = hGlbRht2 = NULL;
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

    if (hGlbSet && lpMemSet)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbSet); lpMemSet = NULL;
    } // End IF

    if (hGlbAxis)
    {
        if (lpMemAxis)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbAxis); lpMemAxis = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbAxis); hGlbAxis = NULL;
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

    return lpYYRes;
} // End PrimFnDydSquadGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_squad.c
//***************************************************************************
