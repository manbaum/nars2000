//***************************************************************************
//  NARS2000 -- Primitive Operator -- Dot
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
//  $PrimOpDot_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    dyadic operator Dot ("ERROR" and "inner product")
//***************************************************************************

LPPL_YYSTYPE PrimOpDot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_DOT);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonDot_EM_YY (lpYYFcnStrOpr,   // Ptr to operator function strand
                                   lptkRhtArg);     // Ptr to right arg
    else
        return PrimOpDydDot_EM_YY (lptkLftArg,      // Ptr to left arg token
                                   lpYYFcnStrOpr,   // Ptr to operator function strand
                                   lptkRhtArg);     // Ptr to right arg token
} // End PrimOpDot_EM_YY


//***************************************************************************
//  $PrimProtoOpDot_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator Dot ("ERROR" and "inner product")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpDot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis)             // Ptr to axis token always NULL)

{
    Assert (lptkAxis EQ NULL);

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return PrimOpMonDotCommon_EM_YY (lpYYFcnStrOpr,         // Ptr to operator function strand
                                         lptkRhtArg,            // Ptr to right arg token
                                         TRUE);                 // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return PrimOpDydDotCommon_EM_YY (lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
                                         lpYYFcnStrOpr,         // Ptr to operator function strand
                                         lptkRhtArg,            // Ptr to right arg token
                                         TRUE);                 // TRUE iff prototyping
} // End PrimProtoOpDot_EM_YY


//***************************************************************************
//  $PrimOpMonDot_EM_YY
//
//  Primitive operator for monadic derived function from Dot ("ERROR")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonDot_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpMonDotCommon_EM_YY (lpYYFcnStrOpr,         // Ptr to operator function strand
                                     lptkRhtArg,            // Ptr to right arg token
                                     FALSE);                // TRUE iff prototyping
} // End PrimOpMonDot_EM_YY


//***************************************************************************
//  $PrimOpMonDotCommon_EM_YY
//
//  Primitive operator for monadic derived function from Dot ("ERROR")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonDotCommon_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff prototyping

{
    return PrimFnSyntaxError_EM (&lpYYFcnStrOpr->tkToken);
} // End PrimOpMonDotCommon_EM_YY


//***************************************************************************
//  $PrimOpDydDot_EM_YY
//
//  Primitive operator for dyadic derived function from Dot ("inner product")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydDot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpDydDotCommon_EM_YY (lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
                                     lpYYFcnStrOpr,         // Ptr to operator function strand
                                     lptkRhtArg,            // Ptr to right arg token
                                     FALSE);                // TRUE iff prototyping
} // End PrimOpDydDot_EM_YY


//***************************************************************************
//  $PrimOpDydDotCommon_EM_YY
//
//  Primitive operator for dyadic derived function from Dot ("inner product")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDydDotCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff prototyping

{
    APLSTYPE      aplTypeLft,           // Left arg storage type
                  aplTypeRht,           // Right ...
                  aplTypeCmp,           // Comparison ...
                  aplTypeRes;           // Result   ...
    APLNELM       aplNELMLft,           // Left arg NELM
                  aplNELMRht,           // Right ...
                  aplNELMRes;           // Result   ...
    APLRANK       aplRankLft,           // Left arg rank
                  aplRankRht,           // Right ...
                  aplRankRes;           // Result   ...
    APLDIM        aplColsLft,           // Left arg last dim
                  aplRestLft,           // Left arg product of remaining dims
                  aplFrstRht,           // Right arg 1st dim
                  aplRestRht,           // Right arg product of remaining dims
                  aplInnrMax;           // Larger of inner dimensions
    APLLONGEST    aplLongestLft,        // Left arg immediate value
                  aplLongestRht;        // Right ...
    HGLOBAL       hGlbLft = NULL,       // Left arg global memory handle
                  hGlbRht = NULL,       // Right ...
                  hGlbRes = NULL,       // Result   ...
                  hGlbItm;              // Arg item ...
    LPVOID        lpMemLft = NULL,      // Ptr to left arg global memory
                  lpMemRht = NULL,      // Ptr to right ...
                  lpMemRes = NULL;      // Ptr to result   ...
    LPAPLDIM      lpMemDimLft,          // Ptr to left arg dimensions
                  lpMemDimRht,          // Ptr to right ...
                  lpMemDimRes;          // Ptr to result   ...
    APLUINT       ByteRes,              // # bytes in the result
                  uRes,                 // Loop counter
                  uOutLft,              // Loop counter
                  uOutRht,              // Loop counter
                  uDimCopy;             // # dimensions to copy
    APLINT        iInnMax;              // Loop counter
    LPPL_YYSTYPE  lpYYRes = NULL,       // Ptr to the result
                  lpYYRes2,             // Ptr to secondary result
                  lpYYFcnStrLft,        // Ptr to left operand function strand
                  lpYYFcnStrRht;        // Ptr to right ...
    LPTOKEN       lptkAxis;             // Ptr to axis token (may be NULL)
    LPPRIMFNS     lpPrimProtoLft,       // Ptr to left operand prototype function
                  lpPrimProtoRht;       // Ptr to right ...
    TOKEN         tkItmLft = {0},       // Left arg item token
                  tkItmRht = {0},       // Right ...
                  tkItmRed;             // Reduction ...
    IMM_TYPES     immTypeItm;           // Arg item immediate type
    LPSYMENTRY    lpSymTmp;             // Ptr to temporary LPSYMENTRY
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    LPPRIMSPEC    lpPrimSpecLft,        // Ptr to left arg local PRIMSPEC
                  lpPrimSpecRht;        // ...    right ...
    LPPRIMFLAGS   lpPrimFlagsLft,       // Ptr to left arg PrimFlags entry
                  lpPrimFlagsRht;       // ...    right ...

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check for axis operator
    lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive to
    //   the axis operator, so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];
    lpYYFcnStrRht = &lpYYFcnStrLft[lpYYFcnStrLft->TknCount];

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_SYNTAX_EXIT;

    // Ensure the right operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     || IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_SYNTAX_EXIT;

    // Get a ptr to the left & right prototype function
    if (bPrototyping)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoLft = GetPrototypeFcnPtr (lpYYFcnStrLft);
        if (!lpPrimProtoLft)
            goto LEFT_NONCE_EXIT;

        // Get the appropriate prototype function ptr
        lpPrimProtoRht = GetPrototypeFcnPtr (lpYYFcnStrRht);
        if (!lpPrimProtoRht)
            goto RIGHT_NONCE_EXIT;
    } else
        lpPrimProtoLft = lpPrimProtoRht = NULL;

    // Get a ptr to the Primitive Function Flags
    lpPrimFlagsLft = GetPrimFlagsPtr (lpYYFcnStrLft);
    lpPrimFlagsRht = GetPrimFlagsPtr (lpYYFcnStrRht);

    // Get the attributes (Type,NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, &aplColsLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get left & right arg global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Calculate length of right arg first dimension
    if (hGlbRht && !IsScalar (aplRankRht))
        aplFrstRht = *VarArrayBaseToDim (lpMemRht);
    else
        aplFrstRht = 1;

    // Check for LENGTH ERROR
    if (aplColsLft NE aplFrstRht
     && !IsUnitDim (aplColsLft)     // Note that the EAS extends scalars or one-element vectors only
     && !IsUnitDim (aplFrstRht))    // ...
        goto LENGTH_EXIT;

    // Calc larger of inner dimensions (in case of scalar extension)
    aplInnrMax = max (aplColsLft, aplFrstRht);

    // Calc product of the remaining dimensions in left arg
    if (aplColsLft)
        aplRestLft = aplNELMLft / aplColsLft;
    else
    for (aplRestLft = 1, uOutLft = 0; uOutLft < (aplRankLft - 1); uOutLft++)
        aplRestLft *= (VarArrayBaseToDim (lpMemLft))[uOutLft];

    // Calc product of the remaining dimensions in right arg
    if (aplFrstRht)
        aplRestRht = aplNELMRht / aplFrstRht;
    else
    for (aplRestRht = 1, uOutRht = 1; uOutRht < aplRankRht; uOutRht++)
        aplRestRht *= (VarArrayBaseToDim (lpMemRht))[uOutRht];

    // Calc result rank
    aplRankRes = max (aplRankLft, 1) + max (aplRankRht, 1) - 2;

    // Calc result NELM
    aplNELMRes = aplRestLft * aplRestRht;

    // If the comparison function is scalar dyadic, and
    //   both args are simple non-hetero, and
    //   the reduction function is scalar dyadic, ...
    if (lpPrimFlagsRht->DydScalar
     && IsSimpleNH (aplTypeLft)
     && IsSimpleNH (aplTypeRht)
     && lpPrimFlagsLft->DydScalar)
    {
        // Get the left & right arg lpPrimSpec
        lpPrimSpecLft = PrimSpecTab[SymTrans (&lpYYFcnStrLft->tkToken)];
        lpPrimSpecRht = PrimSpecTab[SymTrans (&lpYYFcnStrRht->tkToken)];

        // Calculate the storage type of the comparison result
        aplTypeCmp = (*lpPrimSpecRht->StorageTypeDyd) (aplColsLft,
                                                      &aplTypeLft,
                                                      &lpYYFcnStrRht->tkToken,
                                                       aplFrstRht,
                                                      &aplTypeRht);
        if (aplTypeCmp EQ ARRAY_ERROR)
            goto DOMAIN_EXIT;
        // For the moment, APA is treated as INT
        if (IsSimpleAPA (aplTypeCmp))
            aplTypeCmp = ARRAY_INT;
RESTART_INNERPROD_CMP:
        // Calculate the storage type of the reduction result
        aplTypeRes = (*lpPrimSpecLft->StorageTypeDyd) (aplInnrMax,
                                                      &aplTypeCmp,
                                                      &lpYYFcnStrLft->tkToken,
                                                       aplInnrMax,
                                                      &aplTypeCmp);
        if (aplTypeRes EQ ARRAY_ERROR)
            goto DOMAIN_EXIT;

        // If the result is empty, make it Boolean
        if (IsEmpty (aplNELMRes))
            aplTypeRes = ARRAY_BOOL;
    } else
        // The result storage type is assumed to be NESTED,
        //   but we'll call TypeDemote at the end just in case.
        aplTypeRes = ARRAY_NESTED;
RESTART_INNERPROD_RES:
    // Calculate space needed for the result
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
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    if (hGlbLft)
        lpMemDimLft = VarArrayBaseToDim (lpMemLft);
    if (hGlbRht)
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);
    lpMemDimRes = VarArrayBaseToDim (lpMemRes);

    // Fill in the result's dimension
    //   by copying the left arg dimensions (except for the last)
    //   and then the right arg dimensions  (except for the first)
    if (hGlbLft)
    {
        // Calc # dimensions to copy
        uDimCopy = max (aplRankLft, 1) - 1;

        // Copy the dimensions
        CopyMemory (lpMemDimRes, lpMemDimLft, (APLU3264) uDimCopy * sizeof (APLDIM));

        // Skip over the copied dimensions
        lpMemDimRes += uDimCopy;
    } else
    {
        // Fill in the left arg item token
        tkItmLft.tkFlags.TknType   = TKT_VARIMMED;
        tkItmLft.tkFlags.ImmType   = TranslateArrayTypeToImmType (aplTypeLft);
////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
        tkItmLft.tkData.tkLongest  = aplLongestLft;
    } // End IF/ELSE

    // Fill in common fields
    tkItmLft.tkCharIndex = lptkLftArg->tkCharIndex;

    if (hGlbRht)
    {
        // Calc # dimensions to copy
        uDimCopy = max (aplRankRht, 1) - 1;

        // Copy the dimensions
        CopyMemory (lpMemDimRes, &lpMemDimRht[1], (APLU3264) uDimCopy * sizeof (APLDIM));

        // Skip over the copied dimensions
        lpMemDimRes += uDimCopy;
    } else
    {
        // Fill in the right arg item token
        tkItmRht.tkFlags.TknType   = TKT_VARIMMED;
        tkItmRht.tkFlags.ImmType   = TranslateArrayTypeToImmType (aplTypeRht);
////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
        tkItmRht.tkData.tkLongest  = aplLongestRht;
    } // End IF/ELSE

    // Fill in common fields
    tkItmRht.tkCharIndex = lptkRhtArg->tkCharIndex;

    // Skip over the dimensions to the data
    lpMemRes = lpMemDimRes;

    if (IsNested (aplTypeRes))
    {
        // Fill nested result with PTR_REUSED
        //   in case we fail part way through
        *((LPAPLNESTED) lpMemRes) = PTR_REUSED;
        for (uRes = 1; uRes < aplNELMRes; uRes++)
            ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;
    } // End IF

    // Check for empty result
    if (IsEmpty (aplNELMRes))
    {
        // Fill in the prototype which is
        //   leftOperand / {first}leftarg rightOperand {first}rightArg

        // If the left arg is an array, ...
        if (hGlbLft)
        {
            // If the left arg is empty, get its prototype
            if (IsEmpty (aplNELMLft))
            {
                // Split cases based upon the left arg storage type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                    case ARRAY_FLOAT:
                        // Fill in the left arg item token
                        tkItmLft.tkFlags.TknType   = TKT_VARIMMED;
                        tkItmLft.tkFlags.ImmType   = IMMTYPE_BOOL;
////////////////////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmLft.tkData.tkLongest  = 0;

                        break;

                    case ARRAY_CHAR:
                        // Fill in the left arg item token
                        tkItmLft.tkFlags.TknType   = TKT_VARIMMED;
                        tkItmLft.tkFlags.ImmType   = IMMTYPE_CHAR;
////////////////////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmLft.tkData.tkLongest  = L' ';

                        break;

                    case ARRAY_NESTED:
                        // Get the first left arg value
                        GetNextValueGlb (hGlbLft,               // The global memory handle
                                         0,                     // Index into item
                                        &hGlbItm,               // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                        &aplLongestLft,         // Ptr to result immediate value (may be NULL)
                                        &immTypeItm);           // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                        // If the item is an array, ...
                        if (hGlbItm)
                        {
                            // Fill in the left arg item token
                            tkItmLft.tkFlags.TknType   = TKT_VARARRAY;
                            tkItmLft.tkFlags.ImmType   = IMMTYPE_ERROR;
////////////////////////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                            tkItmLft.tkData.tkGlbData  = CopySymGlbDir (hGlbItm);
                        } else
                        {
                            // Fill in the left arg item token
                            tkItmLft.tkFlags.TknType   = TKT_VARIMMED;
                            tkItmLft.tkFlags.ImmType   = immTypeItm;
////////////////////////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                            tkItmLft.tkData.tkLongest  = aplLongestLft;
                        } // End IF/ELSE

                        break;

                    case ARRAY_HETERO:          // Can't occur:  there are no empty HETEROs
                    defstop
                        break;
                } // End SWITCH
            } else
            {
                // Get the first left arg value
                GetNextValueGlb (hGlbLft,               // The global memory handle
                                 0,                     // Index into item
                                &hGlbItm,               // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                &aplLongestLft,         // Ptr to result immediate value (may be NULL)
                                &immTypeItm);           // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                // If the item is an array, ...
                if (hGlbItm)
                {
                    // Fill in the left arg item token
                    tkItmLft.tkFlags.TknType   = TKT_VARARRAY;
                    tkItmLft.tkFlags.ImmType   = IMMTYPE_ERROR;
////////////////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                    tkItmLft.tkData.tkGlbData  = CopySymGlbDir (hGlbItm);
                } else
                {
                    // Fill in the left arg item token
                    tkItmLft.tkFlags.TknType   = TKT_VARIMMED;
                    tkItmLft.tkFlags.ImmType   = immTypeItm;
////////////////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                    tkItmLft.tkData.tkLongest  = aplLongestLft;
                } // End IF/ELSE
            } // End IF/ELSE
        } // End IF

        // If the right arg is an array, ...
        if (hGlbRht)
        {
            // If the right arg is empty, get its prototype
            if (IsEmpty (aplNELMRht))
            {
                // Split cases based upon the right arg storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                    case ARRAY_FLOAT:
                        // Fill in the right arg item token
                        tkItmRht.tkFlags.TknType   = TKT_VARIMMED;
                        tkItmRht.tkFlags.ImmType   = IMMTYPE_BOOL;
////////////////////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmRht.tkData.tkLongest  = 0;

                        break;

                    case ARRAY_CHAR:
                        // Fill in the right arg item token
                        tkItmRht.tkFlags.TknType   = TKT_VARIMMED;
                        tkItmRht.tkFlags.ImmType   = IMMTYPE_CHAR;
////////////////////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmRht.tkData.tkLongest  = L' ';

                        break;

                    case ARRAY_NESTED:
                        // Get the first right arg value
                        GetNextValueGlb (hGlbRht,               // The global memory handle
                                         0,                     // Index into item
                                        &hGlbItm,               // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                        &aplLongestRht,         // Ptr to result immediate value (may be NULL)
                                        &immTypeItm);           // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                        // If the item is an array, ...
                        if (hGlbItm)
                        {
                            // Fill in the left arg item token
                            tkItmRht.tkFlags.TknType   = TKT_VARARRAY;
                            tkItmRht.tkFlags.ImmType   = IMMTYPE_ERROR;
////////////////////////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                            tkItmRht.tkData.tkGlbData  = CopySymGlbDir (hGlbItm);
                        } else
                        {
                            // Fill in the left arg item token
                            tkItmRht.tkFlags.TknType   = TKT_VARIMMED;
                            tkItmRht.tkFlags.ImmType   = immTypeItm;
////////////////////////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                            tkItmRht.tkData.tkLongest  = aplLongestRht;
                        } // End IF/ELSE

                        break;

                    case ARRAY_HETERO:          // Can't occur:  there are no empty HETEROs
                    defstop
                        break;
                } // End SWITCH
            } else
            {
                // Get the first right arg value
                GetNextValueGlb (hGlbRht,                       // The global memory handle
                                 0,                             // Index into item
                                &hGlbItm,                       // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                &aplLongestRht,                 // Ptr to result immediate value (may be NULL)
                                &immTypeItm);                   // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                // If the item is an array, ...
                if (hGlbItm)
                {
                    // Fill in the right arg item token
                    tkItmRht.tkFlags.TknType   = TKT_VARARRAY;
                    tkItmRht.tkFlags.ImmType   = IMMTYPE_ERROR;
////////////////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                    tkItmRht.tkData.tkGlbData  = CopySymGlbDirAsGlb (hGlbItm);
                } else
                {
                    // Fill in the right arg item token
                    tkItmRht.tkFlags.TknType   = TKT_VARIMMED;
                    tkItmRht.tkFlags.ImmType   = immTypeItm;
////////////////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                    tkItmRht.tkData.tkLongest  = aplLongestRht;
                } // End IF/ELSE
            } // End IF/ELSE
        } // End IF

        // Get the right operand prototype function
        if (lpPrimProtoRht EQ NULL)
        {
            // Get the appropriate prototype function ptr
            lpPrimProtoRht = GetPrototypeFcnPtr (lpYYFcnStrRht);
            if (!lpPrimProtoRht)
                goto RIGHT_NONCE_EXIT;
        } // End IF

        // Execute the right operand between the left & right items

        // Note that we cast the function strand to LPTOKEN
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        lpYYRes = (*lpPrimProtoRht) (&tkItmLft,         // Ptr to left arg token
                            (LPTOKEN) lpYYFcnStrRht,    // Ptr to right operand function strand
                                     &tkItmRht,         // Ptr to right arg token
                                      lptkAxis);        // Ptr to axis token (may be NULL)
        // Free the left & right arg tokens
        FreeResult (&tkItmLft);
        FreeResult (&tkItmRht);

        // If it succeeded, ...
        if (lpYYRes)
        {
            // If the result is nested, ...
            if (IsNested (aplTypeRes))
            {
                // If the result is an immediate, ...
                if (lpYYRes->tkToken.tkFlags.TknType EQ TKT_VARIMMED)
                {
                    *((LPAPLNESTED) lpMemRes)++ =
                    lpSymTmp =
                      MakeSymEntry_EM (lpYYRes->tkToken.tkFlags.ImmType,    // Immediate type
                                      &lpYYRes->tkToken.tkData.tkLongest,   // Ptr to immediate value
                                      &lpYYRes->tkToken);                   // Ptr to function token
                    if (!lpSymTmp)
                        goto ERROR_EXIT;
                } else
                    *((LPAPLNESTED) lpMemRes)++ =
                      CopySymGlbDir (lpYYRes->tkToken.tkData.tkGlbData);
            } // End IF

            // Free the result item
            FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;
        } else
            goto ERROR_EXIT;

        goto YYALLOC_EXIT;
    } // End IF

    // The left arg is treated as a two-dimensional array of shape
    //   aplRestLft aplColsLft

    // The right arg is treated as a two-dimensional array of shape
    //   aplFrstRht aplRestRht

    // If the result is simple non-hetero, ...
    if (IsSimpleNH (aplTypeRes))
    {
        APLINT   apaOffLft,         // Left arg APA offset
                 apaMulLft,         // ...          multiplier
                 apaOffRht,         // Right arg APA offset
                 apaMulRht;         // ...           multiplier
        APLUINT  uInnLft,           // Index into left arg
                 uInnRht;           // ...        right ...
        TOKEN    tkRes = {0};       // Temporary token result
        APLINT   aplIntegerLft,     // Left arg integer
                 aplIntegerRht,     // Right ...
                 aplIntegerCmpLft,  // Left comparison arg integer
                 aplIntegerCmpRht;  // Right ...
        APLFLOAT aplFloatLft,       // Left arg float
                 aplFloatRht,       // Right ...
                 aplFloatCmpLft,    // Left comparison arg float
                 aplFloatCmpRht;    // Right ...
        APLCHAR  aplCharLft,        // Left arg char
                 aplCharRht;        // Right ...
        UINT     uBitIndex;         // Bit index for looping through Boolean result

        // Initialize the bit index
        uBitIndex = 0;

        // If the left arg is not immediate, ...
        if (lpMemLft)
            lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
        else
            lpMemLft = &aplLongestLft;

        // If the right arg is not immediate, ...
        if (lpMemRht)
            lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
        else
            lpMemRht = &aplLongestRht;

        // If the left arg is APA, fill in the offset and multiplier
        if (IsSimpleAPA (aplTypeLft))
        {
#define lpAPA       ((LPAPLAPA) lpMemLft)
            // Get the APA parameters
            apaOffLft = lpAPA->Off;
            apaMulLft = lpAPA->Mul;
#undef  lpAPA
        } // End IF

        // If the right arg is APA, fill in the offset and multiplier
        if (IsSimpleAPA (aplTypeRht))
        {
#define lpAPA       ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOffRht = lpAPA->Off;
            apaMulRht = lpAPA->Mul;
#undef  lpAPA
        } // End IF

        // Trundle through the left & right arg remaining dimensions
        for (uOutLft = 0; uOutLft < aplRestLft; uOutLft++)
        for (uOutRht = 0; uOutRht < aplRestRht; uOutRht++)
        {
////////////// Calc result index -- unneeded as we compute the result in row major order
////////////uRes = 1 * uOutRht + aplRestRht * uOutLft;
////////////
            // Trundle through the inner dimensions, back to front
            for (iInnMax = aplInnrMax - 1; iInnMax >= 0; iInnMax--)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (*lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Calc left inner index, taking into account scalar extension
                if (IsUnitDim (aplColsLft))
                    uInnLft = 1 * 0       + aplColsLft * uOutLft;
                else
                    uInnLft = 1 * iInnMax + aplColsLft * uOutLft;

                // Calc right inner index, taking into account scalar extension
                if (IsUnitDim (aplFrstRht))
                    uInnRht = 1 * uOutRht + aplRestRht * 0      ;
                else
                    uInnRht = 1 * uOutRht + aplRestRht * iInnMax;

                // Split cases based upon the left arg storage type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:
                        aplIntegerLft = GetNextInteger (lpMemLft, aplTypeLft, uInnLft);
                        aplFloatLft   = (APLFLOAT) aplIntegerLft;   // In case of type promotion

                        break;

                    case ARRAY_INT:
                        aplIntegerLft = ((LPAPLINT)   lpMemLft)[uInnLft];
                        aplFloatLft   = (APLFLOAT) aplIntegerLft;   // In case of type promotion

                        break;

                    case ARRAY_APA:
                        aplIntegerLft = apaOffLft + apaMulLft * uInnLft;
                        aplFloatLft   = (APLFLOAT) aplIntegerLft;   // In case of type promotion

                        break;

                    case ARRAY_FLOAT:
                        aplFloatLft   = ((LPAPLFLOAT) lpMemLft)[uInnLft];

                        break;

                    case ARRAY_CHAR:
                        aplCharLft    = ((LPAPLCHAR)  lpMemLft)[uInnLft];

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Split cases based upon the right arg storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:
                        aplIntegerRht = GetNextInteger (lpMemRht, aplTypeRht, uInnRht);
                        aplFloatRht   = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                        break;

                    case ARRAY_INT:
                        aplIntegerRht = ((LPAPLINT)   lpMemRht)[uInnRht];
                        aplFloatRht   = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                        break;

                    case ARRAY_APA:
                        aplIntegerRht = apaOffRht + apaMulRht * uInnRht;
                        aplFloatRht   = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                        break;

                    case ARRAY_FLOAT:
                        aplFloatRht   = ((LPAPLFLOAT) lpMemRht)[uInnRht];

                        break;

                    case ARRAY_CHAR:
                        aplCharRht    = ((LPAPLCHAR)  lpMemRht)[uInnRht];

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Execute the comparison function between the left and right args
                if (PrimFnDydSiScSiScSub_EM (&tkRes,                        // Result token
                                             &lpYYFcnStrRht->tkToken,       // Comparison function
                                              aplTypeCmp,                   // Comparison storage type
                                              aplTypeLft,                   // Left arg storage type
                                              aplIntegerLft,                // ...      Boolean/Integer
                                              aplFloatLft,                  // ...      Float
                                              aplCharLft,                   // ...      Char
                                              aplTypeRht,                   // Right arg storage type
                                              aplIntegerRht,                // ...      Boolean/Integer
                                              aplFloatRht,                  // ...      Float
                                              aplCharRht,                   // ...      Char
                                              lpPrimSpecRht))               // Ptr to comparison function PRIMSPEC
                {
                    Assert (tkRes.tkFlags.TknType EQ TKT_VARIMMED);

                    // Check for type promotion
                    if (aplTypeCmp NE TranslateImmTypeToArrayType (tkRes.tkFlags.ImmType))
                    {
                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

                        // We no longer need this resource
                        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

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

                        // Save as the new storage type
                        aplTypeCmp = TranslateImmTypeToArrayType (tkRes.tkFlags.ImmType);

                        goto RESTART_INNERPROD_CMP;
                    } // End IF
                } else
                    goto ERROR_EXIT;

                // If this is not the first time, do the reduction
                if (iInnMax NE (APLINT) (aplInnrMax - 1))
                {
                    // Split cases based upon the comparison immediate type
                    switch (tkRes.tkFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:
                            aplIntegerCmpLft = (BIT0 & tkRes.tkData.tkBoolean);

                            break;

                        case IMMTYPE_INT:
                            aplIntegerCmpLft = tkRes.tkData.tkInteger;

                            break;

                        case IMMTYPE_FLOAT:
                            aplFloatCmpLft   = tkRes.tkData.tkFloat;

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // Execute the left operand between the item result and the accumulated reduction
                    if (PrimFnDydSiScSiScSub_EM (&tkRes,                    // Result token
                                                 &lpYYFcnStrLft->tkToken,   // Reduction function
                                                  aplTypeRes,               // Result storage type
                                                  aplTypeCmp,               // Comparison storage type
                                                  aplIntegerCmpLft,         // Left comparison arg Boolean/Integer
                                                  aplFloatCmpLft,           // ...                 Float
                                                  0,                        // No primitive scalar dyadic function returns a char
                                                  aplTypeCmp,               // Comparison storage type
                                                  aplIntegerCmpRht,         // Right comparison arg Boolean/Integer
                                                  aplFloatCmpRht,           // ...                  Float
                                                  0,                        // No primitive scalar dyadic function returns a char
                                                  lpPrimSpecLft))           // Ptr to reduction function PRIMSPEC
                    {
                        Assert (tkRes.tkFlags.TknType EQ TKT_VARIMMED);

                        // Check for type promotion
                        if (aplTypeRes NE TranslateImmTypeToArrayType (tkRes.tkFlags.ImmType))
                        {
                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

                            // We no longer need this resource
                            FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

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

                            // Save as the new storage type
                            aplTypeRes = TranslateImmTypeToArrayType (tkRes.tkFlags.ImmType);

                            goto RESTART_INNERPROD_RES;
                        } // End IF
                    } else
                        goto ERROR_EXIT;
                } // End IF

                // Copy the last reduction result as the new reduction right arg
                // Split cases based upon the previous result immediate type
                switch (tkRes.tkFlags.ImmType)
                {
                    case IMMTYPE_BOOL:
                        aplIntegerCmpRht = (BIT0 &tkRes.tkData.tkBoolean);

                        break;

                    case IMMTYPE_INT:
                        aplIntegerCmpRht = tkRes.tkData.tkInteger;

                        break;

                    case IMMTYPE_FLOAT:
                        aplFloatCmpRht   = tkRes.tkData.tkFloat;

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            // Split cases based upon the result storage type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:
                    // Save in the result
                    *((LPAPLBOOL) lpMemRes) |= (BIT0 & aplIntegerCmpRht) << uBitIndex;

                    // Check for end-of-byte
                    if (++uBitIndex EQ NBIB)
                    {
                        uBitIndex = 0;              // Start over
                        ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                    } // End IF

                    break;

                case ARRAY_INT:
                    // Save the accumulated reduction in the result
                    *((LPAPLINT)   lpMemRes)++ = aplIntegerCmpRht;

                    break;

                case ARRAY_FLOAT:
                    // Save the accumulated reduction in the result
                    *((LPAPLFLOAT) lpMemRes)++ = aplFloatCmpRht;

                    break;

                defstop
                    break;
            } // End SWITCH
        } // End FOR/FOR
    } else
    {
        // Trundle through the left & right arg remaining dimensions
        for (uOutLft = 0; uOutLft < aplRestLft; uOutLft++)
        for (uOutRht = 0; uOutRht < aplRestRht; uOutRht++)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

////////////// Calc result index -- unneeded as we compute the result in row major order
////////////uRes = 1 * uOutRht + aplRestRht * uOutLft;
////////////
            // Trundle through the inner dimensions, back to front
            for (iInnMax = aplInnrMax - 1; iInnMax >= 0; iInnMax--)
            {
                APLUINT uInnLft,            // Index into left arg
                        uInnRht;            // ...        right ...

                // Calc left inner index, taking into account scalar extension
                if (IsUnitDim (aplColsLft))
                    uInnLft = 1 * 0       + aplColsLft * uOutLft;
                else
                    uInnLft = 1 * iInnMax + aplColsLft * uOutLft;

                // Calc right inner index, taking into account scalar extension
                if (IsUnitDim (aplFrstRht))
                    uInnRht = 1 * uOutRht + aplRestRht * 0      ;
                else
                    uInnRht = 1 * uOutRht + aplRestRht * iInnMax;

                // If the left arg is an array, ...
                if (hGlbLft)
                {
                    // Get the next left arg value
                    GetNextValueGlb (hGlbLft,                       // The global memory handle
                                     uInnLft,                       // Index into item
                                    &hGlbItm,                       // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                    &aplLongestLft,                 // Ptr to result immediate value (may be NULL)
                                    &immTypeItm);                   // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                    // If the item is an array, ...
                    if (hGlbItm)
                    {
                        // Fill in the left arg item token
                        tkItmLft.tkFlags.TknType = TKT_VARARRAY;
                        tkItmLft.tkFlags.ImmType = IMMTYPE_ERROR;
////////////////////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmLft.tkData.tkGlbData = CopySymGlbDir (hGlbItm);
                    } else
                    {
                        // Fill in the left arg item token
                        tkItmLft.tkFlags.TknType = TKT_VARIMMED;
                        tkItmLft.tkFlags.ImmType = immTypeItm;
////////////////////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmLft.tkData.tkLongest = aplLongestLft;
                    } // End IF/ELSE
                } // End IF

                // If the right arg is an array, ...
                if (hGlbRht)
                {
                    // Get the next right arg value
                    GetNextValueGlb (hGlbRht,                       // The global memory handle
                                     uInnRht,                       // Index into item
                                    &hGlbItm,                       // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                    &aplLongestRht,                 // Ptr to result immediate value (may be NULL)
                                    &immTypeItm);                   // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                    // If the item is an array, ...
                    if (hGlbItm)
                    {
                        // Fill in the right arg item token
                        tkItmRht.tkFlags.TknType = TKT_VARARRAY;
                        tkItmRht.tkFlags.ImmType = IMMTYPE_ERROR;
////////////////////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmRht.tkData.tkGlbData = CopySymGlbDir (hGlbItm);
                    } else
                    {
                        // Fill in the right arg item token
                        tkItmRht.tkFlags.TknType = TKT_VARIMMED;
                        tkItmRht.tkFlags.ImmType = immTypeItm;
////////////////////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmRht.tkData.tkLongest = aplLongestRht;
                    } // End IF/ELSE
                } // End IF

                // Execute the right operand between the left & right items
                if (lpPrimProtoRht)
                    // Note that we cast the function strand to LPTOKEN
                    //   to bridge the two types of calls -- one to a primitive
                    //   function which takes a function token, and one to a
                    //   primitive operator which takes a function strand
                    lpYYRes = (*lpPrimProtoRht) (&tkItmLft,         // Ptr to left arg token
                                        (LPTOKEN) lpYYFcnStrRht,    // Ptr to right operand function strand
                                                 &tkItmRht,         // Ptr to right arg token
                                                  lptkAxis);        // Ptr to axis token (may be NULL)
                else
                    lpYYRes = ExecFuncStr_EM_YY (&tkItmLft,         // Ptr to left arg token
                                                  lpYYFcnStrRht,    // Ptr to right operand function strand
                                                 &tkItmRht,         // Ptr to right arg token
                                                  lptkAxis);        // Ptr to axis token (may be NULL)
                // Free the left & right arg tokens
                FreeResult (&tkItmLft);
                FreeResult (&tkItmRht);

                // If it succeeded, ...
                if (lpYYRes)
                {
                    // If this is the first time, there's no reduction
                    if (iInnMax EQ (APLINT) (aplInnrMax - 1))
                    {
                        // Copy the result to the accumulated reduction token
                        tkItmRed = lpYYRes->tkToken;

                        // Free the result item (but not the storage)
                        YYFree (lpYYRes); lpYYRes = NULL;
                    } else
                    {
                        // Execute the left operand between the item result and the accumulated reduction
                        if (lpPrimProtoLft)
                            // Note that we cast the function strand to LPTOKEN
                            //   to bridge the two types of calls -- one to a primitive
                            //   function which takes a function token, and one to a
                            //   primitive operator which takes a function strand
                            lpYYRes2 = (*lpPrimProtoLft) (&lpYYRes->tkToken, // Ptr to left arg token
                                                 (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand function strand
                                                          &tkItmRed,         // Ptr to right arg token
                                                           lptkAxis);        // Ptr to axis token (may be NULL)
                        else
                            lpYYRes2 = ExecFuncStr_EM_YY (&lpYYRes->tkToken, // Ptr to left arg token
                                                           lpYYFcnStrLft,    // Ptr to left operand function strand
                                                          &tkItmRed,         // Ptr to right arg token
                                                           lptkAxis);        // Ptr to axis token (may be NULL)
                        // Free the result item & reduction tokens
                        FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;
                        FreeResult (&tkItmRed);

                        // If it succeeded, ...
                        if (lpYYRes2)
                        {
                            // Copy the result to the accumulated reduction token
                            tkItmRed = lpYYRes2->tkToken;

                            // Free the result item (but not the storage)
                            YYFree (lpYYRes2); lpYYRes2 = NULL;
                        } else
                            goto ERROR_EXIT;
                    } // End IF/ELSE
                } else
                {
                    // If this is not the first time, free the reduction result
                    if (iInnMax EQ (APLINT) (aplInnrMax - 1))
                        FreeResult (&tkItmRed);

                    goto ERROR_EXIT;
                } // End IF/ELSE
            } // End FOR

            // Save the accumulated reduction in the result

            // If the accumulated reduction is an immediate, ...
            if (tkItmRed.tkFlags.TknType EQ TKT_VARIMMED)
            {
                *((LPAPLNESTED) lpMemRes)++ =
                lpSymTmp =
                  MakeSymEntry_EM (tkItmRed.tkFlags.ImmType,    // Immediate type
                                  &tkItmRed.tkData.tkLongest,   // Ptr to immediate value
                                  &tkItmRed);                   // Ptr to function token
                if (!lpSymTmp)
                    goto ERROR_EXIT;
            } else
                *((LPAPLNESTED) lpMemRes)++ =
                  CopySymGlbDir (tkItmRed.tkData.tkGlbData);
            // Free the accumulated reduction token
            FreeResult (&tkItmRed);
        } // End FOR/FOR
    } // End IF
YYALLOC_EXIT:
    // Unlock the result global memory in case TypeDemote actually demotes
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

LEFT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

RIGHT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

LEFT_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

RIGHT_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
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
} // End PrimOpDydDotCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_dot.c
//***************************************************************************
