//***************************************************************************
//  NARS2000 -- Primitive Operator -- Dot
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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
     LPTOKEN      lptkAxis)             // Ptr to axis token (always NULL)

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
//  $PrimIdentOpDot_EM_YY
//
//  Generate an identity element for the primitive operator dyadic Dot
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimIdentOpDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimIdentOpDot_EM_YY
    (LPTOKEN      lptkRhtOrig,          // Ptr to original right arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxisOpr)          // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYFcnStrLft,         // Ptr to left operand function strand
                 lpYYFcnStrRht,         // Ptr to right ...
                 lpYYRes;               // Ptr to result
    HGLOBAL      hGlbMFO;               // Magic function/operator global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    UBOOL        bNegate = FALSE;       // TRUE iff we should negate the result

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig   NE NULL);
    Assert (lpYYFcnStrOpr NE NULL);
    Assert (lptkRhtArg    NE NULL);

    //***************************************************************
    // This operator is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // The (right) identity function for dyadic Dot
    //   (L f.g R) ("inner product") is
    //   for +.{times} is
    //   ({iota}{neg}1{take}{rho} R){jot}.={iota}{neg}1{take}{rho} R.

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (NULL NE CheckAxisOper (lpYYFcnStrOpr))];
    lpYYFcnStrRht = &lpYYFcnStrLft[lpYYFcnStrLft->TknCount];

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_SYNTAX_EXIT;

    // Ensure the right operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     || IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_SYNTAX_EXIT;

    // Pick off the inner products we know how to handle
    if (lpYYFcnStrLft->TknCount EQ 1
     && lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
     && lpYYFcnStrRht->TknCount EQ 1
     && lpYYFcnStrRht->tkToken.tkFlags.TknType EQ TKT_FCNIMMED)
    {
        APLCHAR aplCharLft,
                aplCharRht;

        // Get the left & right single char functions
        aplCharLft = lpYYFcnStrLft->tkToken.tkData.tkChar;
        aplCharRht = lpYYFcnStrRht->tkToken.tkData.tkChar;

        // Substitute UPCARET for CIRCUMFLEX to simplify the comparisons
        if (aplCharLft EQ UTF16_CIRCUMFLEX)
            aplCharLft = UTF16_UPCARET;
        if (aplCharRht EQ UTF16_CIRCUMFLEX)
            aplCharRht = UTF16_UPCARET;

        // Check for +.{times}
        if (aplCharLft EQ UTF16_PLUS
         && aplCharRht EQ UTF16_TIMES)
            goto IDENT1;

        // Check for {or}.^
        if (aplCharLft EQ UTF16_DOWNCARET
         && aplCharRht EQ UTF16_UPCARET)
            goto IDENT1;

        // Check for ^.{ge}
        if (aplCharLft EQ UTF16_UPCARET
         && aplCharRht EQ UTF16_RIGHTCARETUNDERBAR)
            goto IDENT1;

        // Check for =.{ge}
        if (aplCharLft EQ UTF16_EQUAL
         && aplCharRht EQ UTF16_RIGHTCARETUNDERBAR)
            goto IDENT1;

        // Check for {ne}.^
        if (aplCharLft EQ UTF16_NOTEQUAL
         && aplCharRht EQ UTF16_UPCARET)
            goto IDENT1;

        // Check for ^.>
        if (aplCharLft EQ UTF16_UPCARET
         && aplCharRht EQ UTF16_RIGHTCARET)
            goto IDENT2;

        // Check for ^.{or}
        if (aplCharLft EQ UTF16_UPCARET
         && aplCharRht EQ UTF16_DOWNCARET)
            goto IDENT2;

        // Check for =.{or}
        if (aplCharLft EQ UTF16_EQUAL
         && aplCharRht EQ UTF16_DOWNCARET)
            goto IDENT2;

        // Check for {ne}.>
        if (aplCharLft EQ UTF16_NOTEQUAL
         && aplCharRht EQ UTF16_RIGHTCARET)
            goto IDENT2;

        goto NONCE_EXIT;
    } else
        goto NONCE_EXIT;
IDENT2:
    bNegate = TRUE;
IDENT1:
    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the magic function/operator global memory handles
    hGlbMFO = lpMemPTD->hGlbMFO[MFOE_IdnDot];

    lpYYRes =
      ExecuteMagicFunction_EM_YY (NULL,                     // Ptr to left arg token
                                 &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                  lpYYFcnStrOpr,            // Ptr to function strand
                                  lptkRhtArg,               // Ptr to right arg token
                                  lptkAxisOpr,              // Ptr to axis token
                                  hGlbMFO,                  // Magic function/operator global memory handle
                                  NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                  LINENUM_ID);              // Starting line # type (see LINE_NUMS)
    if (bNegate)
    {
        HGLOBAL   hGlbRes;              // Result global memory handle
        LPAPLBOOL lpMemRes;             // Ptr to result global memory
        APLNELM   aplNELMRes,           // Result NELM in bits
                  aplNELMBytes;         // ...            bytes
        APLRANK   aplRankRes;           // ...    rank
        APLUINT   uRes;                 // Loop counter

        // Get the result global ptr
        GetGlbPtrs_LOCK (&lpYYRes->tkToken, &hGlbRes, &lpMemRes);

        // Get the NELM & rank
        aplNELMRes = ((LPVARARRAY_HEADER) lpMemRes)->NELM;
        aplRankRes = ((LPVARARRAY_HEADER) lpMemRes)->Rank;

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

        // Round up the NELM (in bits) to bytes to use as a loop counter
        aplNELMBytes = RoundUpBitsToBytes (aplNELMRes);

        // Loop through the result bytes
        for (uRes = 0; uRes < aplNELMBytes; uRes++)
            // Negate each byte
            *lpMemRes++ ^= 0xFF;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    goto ERROR_EXIT;

LEFT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

RIGHT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    return NULL;
} // End PrimIdentOpDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  Magic function/operator for identity function from the
//    inner product operator
//***************************************************************************

static APLCHAR IdnHeader[] =
  L"Z" $IS MFON_IdnDot L" R";

static APLCHAR IdnLine1[] =
  $QUAD_ID L":"
  L"Z" $IS $JOT L".=" $DUPLICATE $IOTA $NEG L"1" $TAKE L"1," $RHO L"R";

static LPAPLCHAR IdnBody[] =
{IdnLine1,
};

MAGIC_FCNOPR MFO_IdnDot =
{IdnHeader,
 IdnBody,
 countof (IdnBody),
};


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

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonDotCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpMonDotCommon_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,            // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,               // Ptr to right arg token
     UBOOL        bPrototyping)             // TRUE iff prototyping

{
    return PrimFnSyntaxError_EM (&lpYYFcnStrOpr->tkToken APPEND_NAME_ARG);
} // End PrimOpMonDotCommon_EM_YY
#undef  APPEND_NAME


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
    (LPTOKEN      lptkLftArg,               // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,            // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,               // Ptr to right arg token
     UBOOL        bPrototyping)             // TRUE iff prototyping

{
    APLSTYPE      aplTypeLft,               // Left arg storage type
                  aplTypeRht,               // Right ...
                  aplTypeCmp,               // Comparison ...
                  aplTypeRes;               // Result   ...
    APLNELM       aplNELMLft,               // Left arg NELM
                  aplNELMRht,               // Right ...
                  aplNELMRes;               // Result   ...
    APLRANK       aplRankLft,               // Left arg rank
                  aplRankRht,               // Right ...
                  aplRankRes;               // Result   ...
    APLDIM        aplColsLft,               // Left arg last dim
                  aplRestLft,               // Left arg product of remaining dims
                  aplFrstRht,               // Right arg 1st dim
                  aplRestRht,               // Right arg product of remaining dims
                  aplInnrMax;               // Larger of inner dimensions
    APLLONGEST    aplLongestLft,            // Left arg immediate value
                  aplLongestRht;            // Right ...
    HGLOBAL       hGlbLft = NULL,           // Left arg global memory handle
                  hGlbRht = NULL,           // Right ...
                  hGlbRes = NULL,           // Result   ...
                  hGlbItm,                  // Arg item ...
                  hGlbPro = NULL;           // Prototype global memory handle
    LPVOID        lpMemLft = NULL,          // Ptr to left arg global memory
                  lpMemRht = NULL,          // Ptr to right ...
                  lpMemRes = NULL;          // Ptr to result   ...
    LPAPLDIM      lpMemDimLft,              // Ptr to left arg dimensions
                  lpMemDimRht,              // Ptr to right ...
                  lpMemDimRes;              // Ptr to result   ...
    APLUINT       ByteRes,                  // # bytes in the result
                  uRes,                     // Loop counter
                  uOutLft,                  // Loop counter
                  uOutRht,                  // Loop counter
                  uDimCopy;                 // # dimensions to copy
    APLINT        iInnMax,                  // Loop counter
                  apaOffLft,                // Left arg APA offset
                  apaMulLft,                // ...          multiplier
                  apaOffRht,                // Right arg APA offset
                  apaMulRht;                // ...           multiplier
    APLFLOAT      aplFloatIdent;            // Identity element
    LPPL_YYSTYPE  lpYYRes = NULL,           // Ptr to the result
                  lpYYRes2,                 // Ptr to secondary result
                  lpYYFcnStrLft,            // Ptr to left operand function strand
                  lpYYFcnStrRht;            // Ptr to right ...
    LPTOKEN       lptkAxisOpr,              // Ptr to operator axis token (may be NULL)
                  lptkAxisLft,              // ...    left operand axis token (may be NULL)
                  lptkAxisRht;              // ...    right ...
    LPPRIMFNS     lpPrimProtoLft = NULL,    // Ptr to left operand prototype function
                  lpPrimProtoRht = NULL;    // Ptr to right ...
    TOKEN         tkItmLft = {0},           // Left arg item token
                  tkItmRht = {0},           // Right ...
                  tkItmRed,                 // Reduction ...
                  tkProLft = {0},           // Left arg prototype token
                  tkProRht = {0};           // Right ...
    IMM_TYPES     immTypeItm;               // Arg item immediate type
    LPSYMENTRY    lpSymTmp;                 // Ptr to temporary LPSYMENTRY
    LPPLLOCALVARS lpplLocalVars;            // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;             // Ptr to Ctrl-Break flag
    LPPRIMSPEC    lpPrimSpecLft,            // Ptr to left operand local PRIMSPEC
                  lpPrimSpecRht;            // ...    right ...
    LPPRIMFLAGS   lpPrimFlagsLft,           // Ptr to left operand PrimFlags entry
                  lpPrimFlagsRht;           // ...    right ...
    LPPRIMIDENT   lpPrimIdentLft;           // Ptr to left operand PrimIdent entry
    UBOOL         bRet = TRUE,              // TRUE iff result is valid
                  bNrmIdent = FALSE,        // TRUE iff reducing an empty array with a primitive scalar dyadic function
                  bPrimIdent = FALSE;       // TRUE iff reducing an empty array with a primitive or
                                            //   user-defined function/operator

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check for operator axis token
    lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive to
    //   the axis operator, so signal a syntax error if present
    //***************************************************************
    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxisOpr NE NULL)];
    lpYYFcnStrRht = &lpYYFcnStrLft[lpYYFcnStrLft->TknCount];

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_SYNTAX_EXIT;

    // Ensure the right operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     || IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_SYNTAX_EXIT;

    // Check for left operand axis operator
    lptkAxisLft = CheckAxisOper (lpYYFcnStrLft);

    // Check for right operand axis operator
    lptkAxisRht = CheckAxisOper (lpYYFcnStrRht);

    // Get a ptr to the left & right prototype function
    if (bPrototyping)
    {
       // Get the appropriate prototype function ptrs
       lpPrimProtoLft = GetPrototypeFcnPtr (lpYYFcnStrLft);
       lpPrimProtoRht = GetPrototypeFcnPtr (lpYYFcnStrRht);

        if (!lpPrimProtoLft)
            goto LEFT_NONCE_EXIT;

        if (!lpPrimProtoRht)
            goto RIGHT_NONCE_EXIT;
    } // End IF

    // Get a ptr to the Primitive Function Flags
    lpPrimFlagsLft = GetPrimFlagsPtr (lpYYFcnStrLft);
    lpPrimFlagsRht = GetPrimFlagsPtr (lpYYFcnStrRht);
    if (lpPrimFlagsLft)
        lpPrimIdentLft = &PrimIdent[lpPrimFlagsLft->Index];
    else
        lpPrimIdentLft = NULL;

    // Use all zero PrimFlags if not present
    if (!lpPrimFlagsLft)
        lpPrimFlagsLft = &PrimFlags0;
    if (!lpPrimFlagsRht)
        lpPrimFlagsRht = &PrimFlags0;

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
    aplNELMRes = _imul64 (aplRestLft, aplRestRht, &bRet);
    if (!bRet)
        goto WSFULL_EXIT;

    // Handle prototypes specially
    if (IsEmpty (aplNELMRes)
     || bPrototyping)
    {
        if (!lpPrimProtoLft)
            // Get the appropriate prototype function ptr
            lpPrimProtoLft = GetPrototypeFcnPtr (lpYYFcnStrLft);
        if (!lpPrimProtoLft)
            goto LEFT_NONCE_EXIT;
    } else
        lpPrimProtoLft = NULL;

    // If the result is empty or the axis dimension is zero, ...
    if ((IsEmpty (aplNELMRes)
      || IsZeroDim (aplInnrMax)))
    {
        // If we're also prototyping, ...
        if (bPrototyping)
            goto LEFT_NONCE_EXIT;

        // If the reduction function is primitive scalar dyadic, ...
        if (lpPrimFlagsLft->DydScalar)
        {
            // If it's not an immediate primitive function,
            //   or it is, but is without an identity element,
            //   signal a DOMAIN ERROR
            if (lpYYFcnStrLft->tkToken.tkFlags.TknType NE TKT_FCNIMMED
             || !lpPrimFlagsLft->IdentElem)
                goto LEFTOPR_DOMAIN_EXIT;

            // Get the identity element
            aplFloatIdent = lpPrimIdentLft->fIdentElem;

            // If the identity element is Boolean or we're prototyping,
            //   the result is, too
            if (lpPrimIdentLft->IsBool
             || lpPrimProtoLft)
                aplTypeRes = ARRAY_BOOL;
            else
                aplTypeRes = ARRAY_FLOAT;

            // Mark as reducing empty array with a primitive scalar dyadic function
            //   to produce its identity element
            bNrmIdent = TRUE;
        } else
        {
            // The result is nested
            aplTypeRes = ARRAY_NESTED;

            // Mark as reducing empty array with a primitive or
            //   user-defined function/operator
            //   to produce its identity element
            bPrimIdent = TRUE;
        } // End IF/ELSE
    } else
    // If the comparison function is scalar dyadic, and
    //   and there's no right operand axis,
    //   both args are simple non-hetero, and
    //   the reduction function is scalar dyadic, ...
    if (lpPrimFlagsRht->DydScalar
     && lptkAxisRht EQ NULL
     && IsSimpleNH (aplTypeLft)
     && IsSimpleNH (aplTypeRht)
     && lpPrimFlagsLft->DydScalar)
    {
        // Get the left & right arg lpPrimSpec
        lpPrimSpecLft = PrimSpecTab[SymTrans (&lpYYFcnStrLft->tkToken)];
        lpPrimSpecRht = PrimSpecTab[SymTrans (&lpYYFcnStrRht->tkToken)];

        // Calculate the storage type of the comparison result
        aplTypeCmp =
          (*lpPrimSpecRht->StorageTypeDyd) (aplColsLft,
                                           &aplTypeLft,
                                           &lpYYFcnStrRht->tkToken,
                                            aplFrstRht,
                                           &aplTypeRht);
        if (aplTypeCmp EQ ARRAY_ERROR)
            goto RIGHTOPR_DOMAIN_EXIT;
        // For the moment, APA is treated as INT
        if (IsSimpleAPA (aplTypeCmp))
            aplTypeCmp = ARRAY_INT;
RESTART_INNERPROD_CMP:
        // Calculate the storage type of the reduction result
        aplTypeRes =
          (*lpPrimSpecLft->StorageTypeDyd) (aplInnrMax,
                                           &aplTypeCmp,
                                           &lpYYFcnStrLft->tkToken,
                                            aplInnrMax,
                                           &aplTypeCmp);
        if (aplTypeRes EQ ARRAY_ERROR)
            goto LEFTOPR_DOMAIN_EXIT;

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

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Now we can allocate the storage for the result
    //***************************************************************
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
                            tkItmLft.tkData.tkGlbData  = CopySymGlbDir_PTB (hGlbItm);
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
                    tkItmLft.tkData.tkGlbData  = CopySymGlbDir_PTB (hGlbItm);
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
                            tkItmRht.tkData.tkGlbData  = CopySymGlbDir_PTB (hGlbItm);
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

        // If the right operand has no prototype function, ...
        if (!lpPrimProtoRht)
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
                                      lptkAxisOpr);     // Ptr to operator axis token (may be NULL)
        // Free the left & right arg tokens
        FreeResultTkn (&tkItmLft);
        FreeResultTkn (&tkItmRht);

        // If it succeeded, ...
        if (lpYYRes)
        {
            // Check for NoValue
            if (IsTokenNoValue (&lpYYRes->tkToken))
            {
                // Free the YYRes (but not the storage)
                YYFree (lpYYRes); lpYYRes = NULL;

                goto VALUE_EXIT;
            } // End IF

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
                      CopySymGlbDir_PTB (lpYYRes->tkToken.tkData.tkGlbData);
            } // End IF

            // Free the result item
            FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
        } else
            goto ERROR_EXIT;

        goto YYALLOC_EXIT;
    } // End IF

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

    // The left arg is treated as a two-dimensional array of shape
    //   aplRestLft aplColsLft

    // The right arg is treated as a two-dimensional array of shape
    //   aplFrstRht aplRestRht

    // If this is a primitive or user-defined function/operator identity element, ...
    if (bPrimIdent)
    {
        if (!FillIdentityElement_EM (lpMemRes,              // Ptr to result global memory
                                     aplNELMRes,            // Result NELM
                                     aplTypeRes,            // Result storage type
                                     lpYYFcnStrLft,         // Ptr to left operand function strand
                                     lptkLftArg,            // Ptr to left arg token
                                     lpYYFcnStrRht,         // Ptr to right operand function strand
                                     lptkRhtArg))           // Ptr to right arg token
            goto ERROR_EXIT;
    } else
    // If this is primitive scalar dyadic function identity element, ...
    if (bNrmIdent)
    {
        // The zero case is done (GHND)

        // If we're not doing prototypes, ...
        if (!lpPrimProtoLft)
        {
            // Check for Boolean identity element
            if (lpPrimIdentLft->IsBool)
            {
                // Check for identity element 1
                if (lpPrimIdentLft->bIdentElem)
                {
                    APLNELM uNELMRes;

                    // Calculate the # bytes in the result, rounding up
                    uNELMRes = (aplNELMRes + (NBIB - 1)) >> LOG2NBIB;

                    for (uRes = 0; uRes < uNELMRes; uRes++)
                        *((LPAPLBOOL) lpMemRes)++ = 0xFF;
                } // End IF
            } else
            for (uRes = 0; uRes < aplNELMRes; uRes++)
                *((LPAPLFLOAT) lpMemRes)++ = aplFloatIdent;
        } // End IF
    } else
    // If the result is simple non-hetero, ...
    if (IsSimpleNH (aplTypeRes))
    {
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

                            // Lock the memory to get a ptr to it
                            lpMemLft = MyGlobalLock (hGlbLft);
                        } // End IF

                        if (hGlbRht && lpMemRht)
                        {
                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

                            // Lock the memory to get a ptr to it
                            lpMemRht = MyGlobalLock (hGlbRht);
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
        // If the inner dimensions are zero, ...
        if (IsEmpty (aplInnrMax))
        {
            GLBSYM hGlbSym;                 // Ptr to Identity Element
#ifdef DEBUG
            if (IsSimpleNum (aplTypeRes))
                Assert (IsSimpleBool (aplTypeRes));
#endif
            DbgStop (); // ***FIXME***






            // Fill the array with the Identity Element of the
            //   left operand
            hGlbSym =
              GetIdentityElement_EM (lpYYFcnStrLft,     // Ptr to left operand function strand
                                     lptkLftArg,        // Ptr to left arg token
                                     lpYYFcnStrRht,     // Ptr to right operand function strand
                                     lptkRhtArg);       // Ptr to right arg token
            // Check for error
            if (hGlbSym.hGlb EQ NULL)
                goto ERROR_EXIT;

            // Trundle through the left & right arg remaining dimensions
            for (uOutLft = 0; uOutLft < aplRestLft; uOutLft++)
            for (uOutRht = 0; uOutRht < aplRestRht; uOutRht++)
                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (hGlbSym.hGlb);

            // Split cases based upon whether or not the Identity Element is immediate
            switch (GetPtrTypeDir (hGlbSym.hGlb))
            {
                case PTRTYPE_STCONST:
                    break;

                case PTRTYPE_HGLOBAL:
                    // Free the Identity Element storage
                     FreeResultGlobalDFLV (hGlbSym.hGlb);

                    break;

                defstop
                    break;
            } // End SWITCH
        } else
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
                        tkItmLft.tkData.tkGlbData = CopySymGlbDir_PTB (hGlbItm);
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
                        tkItmRht.tkData.tkGlbData = CopySymGlbDir_PTB (hGlbItm);
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
                    lpYYRes =
                      (*lpPrimProtoRht) (&tkItmLft,         // Ptr to left arg token
                                (LPTOKEN) lpYYFcnStrRht,    // Ptr to right operand function strand
                                         &tkItmRht,         // Ptr to right arg token
                                          lptkAxisOpr);     // Ptr to operator axis token (may be NULL)
                else
                    lpYYRes =
                      ExecFuncStr_EM_YY (&tkItmLft,         // Ptr to left arg token
                                          lpYYFcnStrRht,    // Ptr to right operand function strand
                                         &tkItmRht,         // Ptr to right arg token
                                          lptkAxisOpr);     // Ptr to operator axis token (may be NULL)
                // Free the left & right arg tokens
                FreeResultTkn (&tkItmLft);
                FreeResultTkn (&tkItmRht);

                // If it succeeded, ...
                if (lpYYRes)
                {
                    // Check for NoValue
                    if (IsTokenNoValue (&lpYYRes->tkToken))
                    {
                        // Free the YYRes (but not the storage)
                        YYFree (lpYYRes); lpYYRes = NULL;

                        goto VALUE_EXIT;
                    } // End IF

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
                            lpYYRes2 =
                              (*lpPrimProtoLft) (&lpYYRes->tkToken, // Ptr to left arg token
                                        (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand function strand
                                                 &tkItmRed,         // Ptr to right arg token
                                                  lptkAxisOpr);     // Ptr to operator axis token (may be NULL)
                        else
                            lpYYRes2 =
                              ExecFuncStr_EM_YY (&lpYYRes->tkToken, // Ptr to left arg token
                                                  lpYYFcnStrLft,    // Ptr to left operand function strand
                                                 &tkItmRed,         // Ptr to right arg token
                                                  lptkAxisOpr);     // Ptr to operator axis token (may be NULL)
                        // Free the result item & reduction tokens
                        FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
                        FreeResultTkn (&tkItmRed);

                        // If it succeeded, ...
                        if (lpYYRes2)
                        {
                            // Check for NoValue
                            if (IsTokenNoValue (&lpYYRes2->tkToken))
                            {
                                // Free the YYRes (but not the storage)
                                YYFree (lpYYRes2); lpYYRes2 = NULL;

                                goto VALUE_EXIT;
                            } // End IF

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
                    if (iInnMax NE (APLINT) (aplInnrMax - 1))
                        FreeResultTkn (&tkItmRed);

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
                  CopySymGlbDir_PTB (tkItmRed.tkData.tkGlbData);
            // Free the accumulated reduction token
            FreeResultTkn (&tkItmRed);
        } // End FOR/FOR
    } // End IF/ELSE/...
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
                               lptkAxisOpr);
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

LEFTOPR_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

RIGHTOPR_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

VALUE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
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
    if (hGlbPro)
    {
        FreeResultGlobalVar (hGlbPro); hGlbPro = NULL;
    } // End IF

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
//  $ExecDydProto_EM
//
//  Execute a dyadic function (strand) between left & right prototypes
//***************************************************************************

UBOOL ExecDydProto_EM
    (LPTOKEN       lptkLftArg,              // Ptr to left arg token
     LPPL_YYSTYPE  lpYYFcnStrLft,           // Ptr to left operand function strand (for tkCharIndex only)
     LPTOKEN       lptkRhtArg,              // Ptr to right arg token
     LPPL_YYSTYPE  lpYYFcnStrRht,           // Ptr to right operand function strand
     LPPRIMFNS     lpPrimProtoRht,          // Ptr to right operand prototype function
     LPPL_YYSTYPE *lplpYYRes)               // Ptr to ptr to the result

{
    APLLONGEST    aplLongestProLft,         // Left arg prototype immediate value
                  aplLongestProRht;         // Right ...
    HGLOBAL       hGlbProLft,               // Left arg prototype global memory handle
                  hGlbProRht;               // Right ...
    TOKEN         tkProLft = {0},           // Left arg prototype token
                  tkProRht = {0};           // Right ...
    IMM_TYPES     immTypeProLft,            // Left arg prototype immediate type
                  immTypeProRht;            // Right ...

    // Get the prototype of the left arg ({take} L)
    GetFirstValueToken (lptkLftArg,         // Ptr to left arg token
                        NULL,               // Ptr to integer result
                        NULL,               // Ptr to float ...
                        NULL,               // Ptr to WCHAR ...
                       &aplLongestProLft,   // Ptr to longest ...
                       &hGlbProLft,         // Ptr to lpSym/Glb ...
                       &immTypeProLft,      // Ptr to ...immediate type ...
                        NULL);              // Ptr to array type ...
    // If the prototype is a global, ...
    if (hGlbProLft)
    {
        // Setup a token for the left arg prototype to pass to the function
        tkProLft.tkFlags.TknType   = TKT_VARARRAY;
////////tkProLft.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////tkProLft.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
        tkProLft.tkData.tkGlbData  = CopySymGlbDirAsGlb (hGlbProLft);
        tkProLft.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;
    } else
    {
        // Setup a token for the left arg prototype to pass to the function
        tkProLft.tkFlags.TknType   = TKT_VARIMMED;
        tkProLft.tkFlags.ImmType   = immTypeProLft;
////////tkProLft.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
        tkProLft.tkData.tkLongest  = aplLongestProLft;
        tkProLft.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;
    } // End IF/ELSE

    // Get the prototype of the right arg ({take} R)
    GetFirstValueToken (lptkRhtArg,         // Ptr to right arg token
                        NULL,               // Ptr to integer result
                        NULL,               // Ptr to float ...
                        NULL,               // Ptr to WCHAR ...
                       &aplLongestProRht,   // Ptr to longest ...
                       &hGlbProRht,         // Ptr to lpSym/Glb ...
                       &immTypeProRht,      // Ptr to ...immediate type ...
                        NULL);              // Ptr to array type ...
    // If the prototype is a global, ...
    if (hGlbProRht)
    {
        // Setup a token for the right arg prototype to pass to the function
        tkProRht.tkFlags.TknType   = TKT_VARARRAY;
////////tkProRht.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////tkProRht.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
        tkProRht.tkData.tkGlbData  = CopySymGlbDirAsGlb (hGlbProRht);
        tkProRht.tkCharIndex       = lpYYFcnStrRht->tkToken.tkCharIndex;
    } else

    {
        // Setup a token for the right arg prototype to pass to the function
        tkProRht.tkFlags.TknType   = TKT_VARIMMED;
        tkProRht.tkFlags.ImmType   = immTypeProRht;
////////tkProRht.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
        tkProRht.tkData.tkLongest  = aplLongestProRht;
        tkProRht.tkCharIndex       = lpYYFcnStrRht->tkToken.tkCharIndex;
    } // End IF/ELSE

    // Execute the right operand between the left & right prototypes
    //   (({take} L) RhtOpr {take} R)
    // Note that we cast the function strand to LPTOKEN
    //   to bridge the two types of calls -- one to a primitive
    //   function which takes a function token, and one to a
    //   primitive operator which takes a function strand
    *lplpYYRes =
      (*lpPrimProtoRht) (&tkProLft,         // Ptr to left arg token
                (LPTOKEN) lpYYFcnStrRht,    // Ptr to right operand function strand
                         &tkProRht,         // Ptr to right arg token
                          NULL);            // Ptr to axis token (may be NULL)
    // Free the left & right arg tokens
    FreeResultTkn (&tkProLft);
    FreeResultTkn (&tkProRht);

    // Return TRUE if it worked, FALSE otherwise
    return (*lplpYYRes NE NULL);
} // End ExecDydProto_EM


//***************************************************************************
//  End of File: po_dot.c
//***************************************************************************
