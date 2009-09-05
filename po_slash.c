//***************************************************************************
//  NARS2000 -- Primitive Operator -- Slash
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
//  $PrimOpSlash_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    monadic operator Slash ("reduction" and "N-wise reduction")
//***************************************************************************

LPPL_YYSTYPE PrimOpSlash_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLASH
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLASHBAR
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_SLASH         // For when we come in via TKT_OP3NAMED
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_SLASHBAR);    // ...

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (&lpYYFcnStrOpr->tkToken);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonSlash_EM_YY (lpYYFcnStrOpr, // Ptr to operator function strand
                                     lptkRhtArg);   // Ptr to right arg
    else
        return PrimOpDydSlash_EM_YY (lptkLftArg,    // Ptr to left arg token
                                     lpYYFcnStrOpr, // Ptr to operator function strand
                                     lptkRhtArg);   // Ptr to right arg token
} // End PrimOpSlash_EM_YY


//***************************************************************************
//  $PrimProtoOpSlash_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator Slash ("reduction" and "N-wise reduction")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpSlash_EM_YY
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
        return PrimOpMonSlashCommon_EM_YY (lpYYFcnStrOpr,   // Ptr to operator function strand
                                           lptkRhtArg,      // Ptr to right arg token
                                           TRUE);           // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return PrimOpDydSlashCommon_EM_YY (lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
                                           lpYYFcnStrOpr,   // Ptr to operator function strand
                                           lptkRhtArg,      // Ptr to right arg token
                                           TRUE);           // TRUE iff prototyping
} // End PrimProtoOpSlash_EM_YY


//***************************************************************************
//  $PrimOpMonSlash_EM_YY
//
//  Primitive operator for monadic derived function from Slash ("reduction")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonSlash_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpMonSlashCommon_EM_YY (lpYYFcnStrOpr,       // Ptr to operator function strand
                                       lptkRhtArg,          // Ptr to right arg token
                                       FALSE);
} // End PrimOpMonSlash_EM_YY


//***************************************************************************
//  $PrimOpMonSlashCommon_EM_YY
//
//  Primitive operator for monadic derived function from Slash ("reduction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonSlashCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpMonSlashCommon_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,            // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,               // Ptr to right arg token
     UBOOL        bPrototyping)             // TRUE iff prototyping

{
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeRht2,          // Right arg secondary storage type
                      aplTypeTmp,           // Temp right arg ...
                      aplTypeRes;           // Result    ...
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMRes;           // Result    ...
    APLRANK           aplRankRht,           // Right arg rank
                      aplRankRes;           // Result    ...
    APLUINT           aplAxis,              // The (one and only) axis value
                      uLo,                  // uDimLo loop counter
                      uHi,                  // uDimHi ...
                      uDim,                 // Loop counter
                      uDimLo,               // Product of dimensions below axis
                      uDimHi,               // ...                   above ...
                      uDimRht,              // Starting index in right arg of current vector
                      uDimAxRht,            // Right arg axis dimension
                      uRht,                 // Right arg loop counter
                      uPrv,                 // Previous right arg loop counter
                      uRes,                 // Result loop counter
                      ByteRes;              // # bytes in the result
    APLINT            iDim,                 // Integer dimension loop counter
                      apaOffRht,            // Right arg APA offset
                      apaMulRht;            // ...           multiplier
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL;       // Result    ...
    LPVOID            lpMemRht = NULL,      // Ptr to right arg global memory
                      lpMemRes = NULL;      // Ptr to result    ...
    LPAPLDIM          lpMemDimRht,          // Ptr to right arg dimensions
                      lpMemDimRes;          // Ptr to result    ...
    HGLOBAL           hGlbPro = NULL;       // Prototype global memory handle
    APLFLOAT          aplFloatIdent;        // Identity element
    UBOOL             bRet = TRUE,          // TRUE iff result is valid
                      bPrimDydScal = FALSE, // TRUE iff the left operand is a Primitive Dyadic Scalar function
                      bNrmIdent = FALSE,    // TRUE iff reducing an empty array with a primitive scalar dyadic function
                      bUsrDfnOpr = FALSE,   // TRUE iff reducing an empty array with a user-defined function/operator
                      bCatIdent = FALSE,    // TRUE iff reducing an empty array with catenate
                      bFastBool = FALSE;    // TRUE iff this is a Fast Boolean operation
    LPPRIMFNS         lpPrimProtoLft;       // Ptr to left operand prototype function
    LPPRIMSPEC        lpPrimSpecLft;        // Ptr to left operand PRIMSPEC
    LPPRIMFLAGS       lpPrimFlagsLft;       // Ptr to left operand PrimFlags entry
    LPPRIMIDENT       lpPrimIdentLft;       // Ptr to left operand PrimIdent entry
    LPPL_YYSTYPE      lpYYRes = NULL,       // Ptr to the result
                      lpYYFcnStrLft;        // Ptr to left operand function strand
    TOKEN             tkLftArg = {0},       // Left arg token
                      tkRhtArg = {0};       // Right ...
    LPTOKEN           lptkAxis;             // Ptr to axis token (may be NULL)
    LPSYMENTRY        lpSymTmp;             // Ptr to temporary LPSYMENTRY
    UINT              uBitIndex;            // Bit index for looping through Booleans
    LPVARARRAY_HEADER lpMemHdrRes;          // Ptr to result header
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLLONGEST        aplLongestRht;        // Right arg immediate value

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check for axis operator
    lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Reduction allows a single integer axis value only
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRht,      // All values less than this
                           TRUE,            // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // Return TRUE iff fractional values present
                          &aplAxis,         // Return last axis value
                           NULL,            // Return # elements in axis vector
                           NULL))           // Return HGLOBAL with APLINT axis values
            return NULL;
    } else
    {
        // No axis specified:
        // if Slash, use last dimension
        if (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLASH
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_SLASH)
            aplAxis = aplRankRht - 1;
        else
            // Otherwise, it's SlashBar on the first dimension
            aplAxis = 0;
    } // End IF/ELSE

    // Get right arg's global ptr
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg is a scalar, return it
    if (IsScalar (aplRankRht))
    {
        lpYYRes =
          PrimOpMonSlashScalar_EM_YY (lptkRhtArg,       // Ptr to right arg token
                                      aplTypeRht,       // Right arg storage type
                                      aplLongestRht,    // Right arg immediate value
                                      hGlbRht,          // Right arg global memory handle
                                      lpYYFcnStrOpr,    // Ptr to operator function strand
                                      lpYYFcnStrLft,    // Ptr to left operand
                                      bPrototyping);    // TRUE iff prototyping
        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // From here on, the right arg is a vector or higher rank array
    //***************************************************************

    // The rank of the result is one less than that of the right arg
    aplRankRes = aplRankRht - 1;

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Skip over the header and dimensions to the data
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

    //***************************************************************
    // Calculate product of dimensions before, at, and after the axis dimension
    //***************************************************************

    // Calculate the product of the right arg's dimensions below the axis dimension
    uDimLo = 1;
    for (uDim = 0; uDim < aplAxis; uDim++)
        uDimLo *= lpMemDimRht[uDim];

    // Get the length of the axis dimension
    uDimAxRht = lpMemDimRht[uDim++];

    // Calculate the product of the right arg's dimensions above the axis dimension
    uDimHi = 1;
    for (; uDim < aplRankRht; uDim++)
        uDimHi *= lpMemDimRht[uDim];

    // Calculate the result NELM
    aplNELMRes = _imul64 (uDimLo, uDimHi, &bRet);
    if (!bRet)
        goto WSFULL_EXIT;

    // Handle prototypes specially
    if (IsEmpty (aplNELMRes)
     || bPrototyping)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoLft = GetPrototypeFcnPtr (lpYYFcnStrLft);
        if (!lpPrimProtoLft)
            goto LEFT_NONCE_EXIT;
    } else
        lpPrimProtoLft = NULL;

    // Get a ptr to the Primitive Function Flags
    lpPrimFlagsLft = GetPrimFlagsPtr (lpYYFcnStrLft);
    if (lpPrimFlagsLft)
        lpPrimIdentLft = &PrimIdent[lpPrimFlagsLft->Index];
    else
        lpPrimIdentLft = NULL;

    // Use all zero PrimFlags if not present
    if (!lpPrimFlagsLft)
        lpPrimFlagsLft = &PrimFlags0;

    // If the result is empty or the axis dimension is zero,
    //   and the function is user-defined, ...
    if ((IsEmpty (aplNELMRes)
      || IsZeroDim (uDimAxRht))
     && IsTknUsrDfn (&lpYYFcnStrLft->tkToken))
    {
        // The result is nested
        aplTypeRes = ARRAY_NESTED;

        // Mark as reducing empty array with user-defined function/operator
        //   to produce its identity element
        bUsrDfnOpr = TRUE;
    } else
    // If the result is empty or the axis dimension is zero,
    //   and the function is catenate, ...
    if ((IsEmpty (aplNELMRes)
      || IsZeroDim (uDimAxRht))
     && (lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
      && (lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_COMMA
       || lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_COMMABAR)))
    {
        // The result is nested
        aplTypeRes = ARRAY_NESTED;

        // Mark as reducing empty array with catenate
        //   to produce its identity element
        bCatIdent = TRUE;
    } else
    // If the axis dimension is zero and the array is simple
    //   get the identity element for the left operand or
    //   signal a DOMAIN ERROR
    if (IsZeroDim (uDimAxRht)
     && IsSimple (aplTypeRht))
    {
        // If it's not an immediate primitive function,
        //   or it is, but is without an identity element,
        //   signal a DOMAIN ERROR
        if (lpYYFcnStrLft->tkToken.tkFlags.TknType NE TKT_FCNIMMED
         || !lpPrimFlagsLft->IdentElem)
            goto DOMAIN_EXIT;

        // Get the identity element
        aplFloatIdent = lpPrimIdentLft->fIdentElem;

        // If the identity element is Boolean or we're prototyping,
        //   the result is too
        if (lpPrimIdentLft->IsBool
         || lpPrimProtoLft NE NULL)
            aplTypeRes = ARRAY_BOOL;
        else
            aplTypeRes = ARRAY_FLOAT;

        // Mark as reducing empty array with a primitive scalar dyadic function
        //   to produce its identity element
        bNrmIdent = TRUE;
    } else
    // If the product of the dimensions above
    //   the axis dimension is one, and
    //   this is a primitive function, and
    //   the right arg is Boolean or APA Boolean, and
    //   the axis dimension is > 1, and
    //   we're not doing prototypes, then
    //   check for the possibility of doing a
    //   Fast Boolean Reduction
    if (IsUnitDim (uDimHi)
     && lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
     && (IsSimpleBool (aplTypeRht)
      || (IsSimpleAPA (aplTypeRht)
       && IsBooleanValue (apaOffRht)
       && apaMulRht EQ 0 ))
     && IsMultiDim (uDimAxRht)
     && lpPrimProtoLft EQ NULL
     && (lpPrimFlagsLft->FastBool || lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_PLUS))
    {
        // Mark as a Fast Boolean operation
        bFastBool = TRUE;

        // If this is "plus", then the storage type is Integer
        if (lpPrimFlagsLft->Index EQ PF_INDEX_PLUS)
            aplTypeRes = ARRAY_INT;
        else
        // Otherwise, it's Boolean
            aplTypeRes = ARRAY_BOOL;
    } else
    // If the operand is a primitive scalar dyadic function,
    //   and the right arg is simple NH,
    //   calculate the storage type of the result,
    //   otherwise, assume it's ARRAY_NESTED
    if (lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
     && lpPrimFlagsLft->DydScalar
     && IsSimpleNH (aplTypeRht))
    {
        // If the function is equal or not-equal, and the right
        //   arg is not Boolean, make the result storage type
        //   nested and let TypeDemote figure it out in the end.
        // This avoids complications with =/3 1{rho}'abc' which
        //   is typed as Boolean but blows up to CHAR.
        if ((lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_EQUAL
          || lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_NOTEQUAL)
         && !IsSimpleBool (aplTypeRht))
            aplTypeRes = ARRAY_NESTED;
        else
        {
            // Get the corresponding lpPrimSpecLft
            lpPrimSpecLft = PrimSpecTab[SymTrans (&lpYYFcnStrLft->tkToken)];

            // Calculate the storage type of the result
            aplTypeRes =
              (*lpPrimSpecLft->StorageTypeDyd) (aplNELMRht,
                                               &aplTypeRht,
                                               &lpYYFcnStrLft->tkToken,
                                                aplNELMRht,
                                               &aplTypeRht);
            if (aplTypeRes EQ ARRAY_ERROR)
                goto DOMAIN_EXIT;

            // Mark as a primitive scalar dyadic function
            bPrimDydScal = TRUE;
        } // End IF/ELSE
    } else
        aplTypeRes = ARRAY_NESTED;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Allocate space for the result
    // N.B. Conversion from APLUINT to UINT.
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = lpMemHdrRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = lpMemDimRes = VarArrayBaseToDim (lpMemRes);

    // Copy the dimensions from the right arg to the result
    //   except for the axis dimension
    for (uDim = 0; uDim < aplRankRht; uDim++)
    if (uDim NE aplAxis)
        *((LPAPLDIM) lpMemRes)++ = lpMemDimRht[uDim];

    // lpMemRes now points to its data

    // If the result is nested, ...
    if (IsNested (aplTypeRes))
    {
        // Fill nested result with PTR_REUSED
        //   in case we fail part way through
        *((LPAPLNESTED) lpMemRes) = PTR_REUSED;
        for (uRes = 1; uRes < aplNELMRes; uRes++)
            ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;
    } // End IF

    // If this is catenate identity element, ...
    if (bCatIdent)
    {
        IMM_TYPES immTypePro;           // Prototype immediate type
        UBOOL     bCalcPro;             // TRUE iff we need to calculate the prototype
        LPTOKEN   lptkAxis2;            // Ptr to secondary axis token
        APLUINT   aplAxis2;             // The catenate axis value
        APLRANK   aplRankPro;           // Prototype rank

        // The prototype of the result is the same as that of the right arg except the
        //   catenate axis (lptkAxis2) is zero.
        // That is, we need 0/[aplAxis2] {take} R

        // Get the prototype of the right arg ({take} R)
        GetFirstValueToken (lptkRhtArg,         // Ptr to right arg token
                            NULL,               // Ptr to integer result
                            NULL,               // Ptr to float ...
                            NULL,               // Ptr to WCHAR ...
                            NULL,               // Ptr to longest ...
                           &hGlbPro,            // Ptr to lpSym/Glb ...
                           &immTypePro,         // Ptr to ...immediate type ...
                            NULL);              // Ptr to array type ...
        // Mark as needing more computation if the prototype is a global
        bCalcPro = (hGlbPro NE NULL);

        // If the prototype is a global, ...
        if (bCalcPro)
            // Get the attributes (Type, NELM, and Rank) of the prototype
            AttrsOfGlb (hGlbPro, NULL, NULL, &aplRankPro, NULL);
        else
        // The prototype is an immediate
        {
            // The prototype of the result is an empty vector
            //   of the same type as the prototype
            if (IsImmChr (immTypePro))
                hGlbPro = hGlbV0Char;
            else
                hGlbPro = hGlbZilde;

            // Set the prototype rank to that of a vector
            //   so ,[1] and commabar validate
            aplRankPro = 1;
        } // End IF

        // Check for axis operator
        lptkAxis2 = CheckAxisOper (lpYYFcnStrLft);
        if (lptkAxis2)
        {
            // Catentate allows integer axis values only
            // Note that there is no identity element for laminate reduction
            if (!CheckAxis_EM (lptkAxis2,       // The axis token
                               aplRankPro,      // All values less than this
                               TRUE,            // TRUE iff scalar or one-element vector only
                               FALSE,           // TRUE iff want sorted axes
                               FALSE,           // TRUE iff axes must be contiguous
                               FALSE,           // TRUE iff duplicate axes are allowed
                               NULL,            // Return TRUE iff fractional values present
                              &aplAxis2,        // Return last axis value
                               NULL,            // Return # elements in axis vector
                               NULL))           // Return HGLOBAL with APLINT axis values
                goto ERROR_EXIT;
        } else
        {
            // No axis specified:
            //   if comma, use last dimension
            if (lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_COMMA)
                aplAxis2 = max (0, (APLRANKSIGN) aplRankPro - 1);
            else
            {
                Assert (lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_COMMABAR);

                // Otherwise, it's CommaBar on the first dimension
                aplAxis2 = 0;
            } // End IF/ELSE
        } // End IF/ELSE

        // If we need to calculate the prototype
        if (bCalcPro)
        {
            TOKEN        tkLft = {0},                   // Token for temporary left arg
                         tkRht = {0},                   // ...                 right ...
                         tkFcn = {0};                   // Ptr to function token
            LPPL_YYSTYPE lpYYPro;                       // Ptr to prototype result

            // Setup the left arg token
            tkLft.tkFlags.TknType   = TKT_VARIMMED;
            tkLft.tkFlags.ImmType   = IMMTYPE_BOOL;
////////////tkLft.tkFlags.NoDisplay = FALSE;            // Already zero from = {0}
////////////tkLft.tkData.tkLongest  = 0;                // Already zero ftom = {0}

            // Setup the right arg token
            tkRht.tkFlags.TknType   = TKT_VARARRAY;
////////////tkRht.tkFlags.ImmType   = IMMTYPE_ERROR;    // Already zero from = {0}
////////////tkRht.tkFlags.NoDisplay = FALSE;            // Already zero from = {0}
            tkRht.tkData.tkGlbData  = MakePtrTypeGlb (hGlbPro);

            // Setup the function token
            tkFcn.tkFlags.TknType   = TKT_FCNIMMED;
            tkFcn.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////////////tkFcn.tkFlags.NoDisplay = FALSE;            // Already zero from = {0}
            tkFcn.tkData.tkChar     = (lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_COMMABAR) ? UTF16_SLASHBAR
                                                                                               : UTF16_SLASH;
            // Compute 0/[aplAxis2] hGlbPro
            lpYYPro =
              PrimFnDydSlash_EM_YY (&tkLft,             // Ptr to left arg token
                                    &tkFcn,             // Ptr to function token
                                    &tkRht,             // Ptr to right arg token
                                     lptkAxis2);        // Ptr to axis token (may be NULL)
            if (lpYYPro EQ NULL)
                goto ERROR_EXIT;
            Assert (lpYYPro->tkToken.tkFlags.TknType EQ TKT_VARARRAY);

            // Get the global memory handle as the prototype
            hGlbPro = lpYYPro->tkToken.tkData.tkGlbData;

            // Free the temporary result
            YYFree (lpYYPro); lpYYPro = NULL;
        } // End IF/ELSE

        // Set the ptr type bits
        hGlbPro = MakePtrTypeGlb (hGlbPro);

        // Save the identity element in the result
        *((LPAPLNESTED) lpMemRes)++ = hGlbPro;
        for (uRes = 1; uRes < aplNELMRes; uRes++)
            *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (hGlbPro);
    } else
    // If this is user-defined function/operator identity element, ...
    if (bUsrDfnOpr)
    {
        IMM_TYPES  immTypePro;      // Prototype immediate type
        TOKEN      tkProto = {0};   // Prototype token
        HGLOBAL    hGlbIdn;         // Identity element global memory handle
        APLLONGEST aplLongestPro,   // Prototype element immediate value
                   aplLongestIdn;   // Identity  ...
        LPVOID     hSymGlbIdn;      // ...               LPSYMENTRY or HGLOBAL
        APLSTYPE   aplTypeIdn;      // ...               storage type

        // Get the prototype of the right arg ({take} R)
        GetFirstValueToken (lptkRhtArg,         // Ptr to right arg token
                            NULL,               // Ptr to integer result
                            NULL,               // Ptr to float ...
                            NULL,               // Ptr to WCHAR ...
                           &aplLongestPro,      // Ptr to longest ...
                           &hGlbPro,            // Ptr to lpSym/Glb ...
                           &immTypePro,         // Ptr to ...immediate type ...
                            NULL);              // Ptr to array type ...
        // If the prototype is a global, ...
        if (hGlbPro)
        {
            // Setup a token for the right arg prototype to pass to the function
            tkProto.tkFlags.TknType   = TKT_VARARRAY;
////////////tkProto.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////////tkProto.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
            tkProto.tkData.tkGlbData  = MakePtrTypeGlb (hGlbPro);
            tkProto.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;
        } else
        {
            // Setup a token for the right arg prototype to pass to the function
            tkProto.tkFlags.TknType   = TKT_VARIMMED;
            tkProto.tkFlags.ImmType   = immTypePro;
////////////tkProto.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
            tkProto.tkData.tkLongest  = aplLongestPro;
            tkProto.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;
        } // End IF/ELSE

        // Get the user-defined function/operator's identity element
        lpYYRes =
          ExecFuncStrLine_EM_YY (NULL,              // Ptr to left arg token (may be NULL)
                                 lpYYFcnStrLft,     // Ptr to left operand function strand
                                &tkProto,           // Ptr to right arg token
                                 NULL,              // Ptr to axis token (may be NULL)
                                 LINENUM_IDENTITY); // Starting line # (see LINE_NUMS)
        if (!lpYYRes)
            goto ERROR_EXIT;
        // Get identity element's global ptr (if any)
        aplLongestIdn = GetGlbPtrs (&lpYYRes->tkToken, &hGlbIdn);

        // If the result is global, ...
        if (hGlbIdn)
            hSymGlbIdn = CopySymGlbDirAsGlb (hGlbIdn);
        else
        {
            // Get the attributes (Type, NELM, and Rank) of the identity element
            AttrsOfToken (&lpYYRes->tkToken, &aplTypeIdn, NULL, NULL, NULL);

            // Convert the immediate value to a SYMENTRY
            hSymGlbIdn =
              MakeSymEntry_EM (TranslateArrayTypeToImmType (aplTypeIdn),
                              &aplLongestIdn,
                              &lpYYFcnStrLft->tkToken);
        } // End IF/ELSE

        // Free the YYRes (and the storage)
        FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;

        if (!hSymGlbIdn)
            goto ERROR_EXIT;

        // Save the identity element in the result
        *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (hSymGlbIdn);

        for (uRes = 1; uRes < aplNELMRes; uRes++)
            *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (hSymGlbIdn);
    } else
    // If this is primitive scalar dyadic function identity element, ...
    if (bNrmIdent)
    {
        // The zero case is done (GHND)

        // If we're not doing prototypes, ...
        if (lpPrimProtoLft EQ NULL)
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
    // If this is a fast Boolean operation, ...
    if (bFastBool)
    {
        LPFASTBOOLFCN lpFastBool;               // Ptr to Fast Boolean reduction routine

        // Get a ptr to the appropriate Fast Boolean routine
        lpFastBool = FastBoolFns[lpPrimFlagsLft->Index].lpReduction;

        // Call it
        (*lpFastBool) (aplTypeRht,              // Right arg storage type
                       aplNELMRht,              // Right arg NELM
                       lpMemRht,                // Ptr to right arg memory
                       lpMemRes,                // Ptr to result    memory
                       uDimLo,                  // Product of dimensions below axis
                       uDimAxRht,               // Length of right arg axis dimension
                       lpPrimFlagsLft->Index,   // FBFN_INDS value (e.g., index into FastBoolFns[])
                       lpYYFcnStrOpr);          // Ptr to operator function strand
    } else
    // If this is a nested result from an empty right arg, ...
    if (IsEmpty (aplNELMRht)
     && IsNested (aplTypeRes))
    {
        APLNELM aplNELMNst;

        // Calculate the # elements in the result
        aplNELMNst = max (aplNELMRes, 1);

        // If the right arg is nested or simple hetero, ...
        if (IsNested (aplTypeRht)
         || IsSimpleHet (aplTypeRht))
            // Loop through the right arg/result
            for (uRes = 0; uRes < aplNELMNst; uRes++)
                // Copy the right arg's prototype
                *((LPAPLNESTED) lpMemRes)++ = CopySymGlbInd (lpMemRht);
        else
        {
            LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

            // Get ptr to PerTabData global memory
            lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

            // Loop through the right arg/result
            for (uRes = 0; uRes < aplNELMNst; uRes++)
                // Make & save a prototype of the right arg
                *((LPAPLNESTED) lpMemRes)++ =
                  (IsSimpleNum (aplTypeRht)) ? lpMemPTD->steZero
                                             : lpMemPTD->steBlank;
        } // End IF
    } else
    // If this is +/APA
    if (IsSimpleAPA (aplTypeRht)
     && lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
     && lpYYFcnStrLft->tkToken.tkData.tkChar   EQ L'+')
    {
RESTART_EXCEPTION_APA:
        // If the result is integer
        if (IsSimpleInt (aplTypeRes))
        {
            // Loop through the right arg calling the
            //   function strand between data, storing in the
            //   result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                // Calculate the starting index in the right arg of this vector
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;

                // The indices of the right arg are
                //   uDimRht + uDimHi * {iota} uDimAxRht

                // The result is
                //   +/apaOffRht + apaMulRht * (uDimRht + uDimHi * {iota} uDimAxRht)
                // = +/apaOffRht + apaMulRht * uDimRht + apalMulRht * uDimHi * {iota} uDimAxRht
                // = uDimAxRht * (apaOffRht + apaMulRht * uDumRht) + +/apaMulRht * uDimHi *   {iota} uDimAxRht
                // = uDimAxRht * (apaOffRht + apaMulRht * uDumRht) +   apaMulRht * uDimHi * +/{iota} uDimAxRht
                // = uDimAxRht * (apaOffRht + apaMulRht * uDumRht) +   apaMulRht * uDimHi * (uDimAxRht * (uDimAxRht - 1)) / 2
                __try
                {
////////////////////*((LPAPLINT) lpMemRes)++ = uDimAxRht * (apaOffRht + apaMulRht * uDimRht)
////////////////////                         + apaMulRht * uDimHi * (uDimAxRht * (uDimAxRht - 1)) / 2;
                    *((LPAPLINT) lpMemRes)++ = iadd64 (imul64 (uDimAxRht, iadd64 (apaOffRht, imul64 (apaMulRht, uDimRht))),
                                                       imul64 (apaMulRht, imul64 (uDimHi, imul64 (uDimAxRht, isub64 (uDimAxRht, 1)) / 2)));
                } __except (CheckException (GetExceptionInformation (), L"PrimFnMon_EM_YY #1"))
                {
#ifdef DEBUG
                    dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #1: %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
                    // Split cases based upon the ExceptionCode
                    switch (MyGetExceptionCode ())
                    {
                        case EXCEPTION_RESULT_FLOAT:
                            MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                            if (IsSimpleNum (aplTypeRes)
                             && !IsSimpleFlt (aplTypeRes))
                            {
                                // It's now a FLOAT result
                                aplTypeRes = ARRAY_FLOAT;

                                // Tell the header about it
                                lpMemHdrRes->ArrType = aplTypeRes;

                                // Restart the pointer
                                lpMemRes = VarArrayBaseToData (lpMemHdrRes, aplRankRes);
#ifdef DEBUG
                                dprintfWL9 (L"!!Restarting Exception in " APPEND_NAME L" #1: %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
                                goto RESTART_EXCEPTION_APA;
                            } // End IF

                            // Fall through to never-never-land

                        default:
                            // Display message for unhandled exception
                            DisplayException ();

                            break;
                    } // End SWITCH
                } // End __try/__except
            } // End FOR/FOR
        } else
        // The result is float
        {
            APLFLOAT uFltDimRht,
                     apaFltOffRht,
                     apaFltMulRht,
                     uFltDimAxRht,
                     uFltDimHi;

            // Convert vars to float
            apaFltOffRht = (APLFLOAT) apaOffRht;
            apaFltMulRht = (APLFLOAT) apaMulRht;
            uFltDimAxRht = (APLFLOAT) (APLINT) uDimAxRht;
            uFltDimHi    = (APLFLOAT) (APLINT) uDimHi;

            // Loop through the right arg calling the
            //   function strand between data, storing in the
            //   result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                // Calculate the starting index in the right arg of this vector
                uFltDimRht = (APLFLOAT) (APLINT) (uLo * uDimHi * uDimAxRht + uHi);

                // The indices of the right arg are
                //   uDimRht + uDimHi * {iota} uDimAxRht

                // The result is
                //   +/apaOffRht + apaMulRht * (uDimRht + uDimHi * {iota} uDimAxRht)
                // = +/apaOffRht + apaMulRht * uDimRht + apalMulRht * uDimHi * {iota} uDimAxRht
                // = uDimAxRht * (apaOffRht + apaMulRht * uDumRht) + +/apaMulRht * uDimHi *   {iota} uDimAxRht
                // = uDimAxRht * (apaOffRht + apaMulRht * uDumRht) +   apaMulRht * uDimHi * +/{iota} uDimAxRht
                // = uDimAxRht * (apaOffRht + apaMulRht * uDumRht) +   apaMulRht * uDimHi * (uDimAxRht * (uDimAxRht - 1)) / 2
////////////////*((LPAPLINT) lpMemRes)++ = uDimAxRht * (apaOffRht + apaMulRht * uDimRht)
////////////////                         + apaMulRht * uDimHi * (uDimAxRht * (uDimAxRht - 1)) / 2;
                *((LPAPLFLOAT) lpMemRes)++ = uFltDimAxRht * (apaFltOffRht + apaFltMulRht * uFltDimRht)
                                           + apaFltMulRht * uFltDimHi * (uFltDimAxRht * (uFltDimAxRht - 1)) / 2;
            } // End FOR/FOR
        } // End IF/ELSE
    } else
    {
        // Fill in the right arg token
////////tkRhtArg.tkFlags.TknType   =            // To be filled in below
////////tkRhtArg.tkFlags.ImmType   =            // To be filled in below
////////tkRhtArg.tkFlags.NoDisplay = FALSE;     // Already zero from {0}
////////tkRhtArg.tkData.tkGlbData  =            // To be filled in below
        tkRhtArg.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

        // Fill in the left arg token
////////tkLftArg.tkFlags.TknType   =            // To be filled in below
////////tkLftArg.tkFlags.ImmType   =            // To be filled in below
////////tkLftArg.tkFlags.NoDisplay = FALSE;     // Already zero from {0}
////////tkLftArg.tkData.tkGlbData  =            // To be filled in below
        tkLftArg.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

        // Initialize the secondary right arg storage type
        aplTypeRht2 = aplTypeRht;
RESTART_EXCEPTION:
        // Initialize in case Boolean
        uBitIndex = 0;

        // Loop through the right arg calling the
        //   function strand between data, storing in the
        //   result
        for (uLo = 0; uLo < uDimLo; uLo++)
        for (uHi = 0; uHi < uDimHi; uHi++)
        {
            // Calculate the starting index in the right arg of this vector
            uDimRht = uLo * uDimHi * uDimAxRht + uHi;

            // Calculate the index of last element in this vector
            uRht = uDimRht + (uDimAxRht - 1) * uDimHi;

            // Copy the right arg type to a temp so we can respecify
            //   it in case we blow up
            aplTypeTmp = aplTypeRht;

            // If the left operand is a Primitive Dyadic Scalar function, ...
            if (bPrimDydScal)
            {
                APLINT   aplIntegerLft,             // Left arg as integer
                         aplIntegerRht;             // Right ...
                APLFLOAT aplFloatLft,               // Left arg as float
                         aplFloatRht;               // Right ...
                APLCHAR  aplCharLft,                // Left arg as char
                         aplCharRht;                // Right ...

                // Split cases based upon the right arg storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:
                        tkRhtArg.tkFlags.ImmType  = IMMTYPE_BOOL;
                        tkRhtArg.tkData.tkInteger =
                        aplIntegerRht             = GetNextInteger (lpMemRht, aplTypeRht, uRht);
                        aplFloatRht               = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                        break;

                    case ARRAY_INT:
                        tkRhtArg.tkFlags.ImmType  = IMMTYPE_INT;
                        tkRhtArg.tkData.tkInteger =
                        aplIntegerRht             = ((LPAPLINT)   lpMemRht)[uRht];
                        aplFloatRht               = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                        break;

                    case ARRAY_APA:
                        tkRhtArg.tkFlags.ImmType  = IMMTYPE_INT;
                        tkRhtArg.tkData.tkInteger =
                        aplIntegerRht             = apaOffRht + apaMulRht * uRht;
                        aplFloatRht               = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                        break;

                    case ARRAY_FLOAT:
                        tkRhtArg.tkFlags.ImmType  = IMMTYPE_FLOAT;
                        tkRhtArg.tkData.tkFloat   =
                        aplFloatRht               = ((LPAPLFLOAT) lpMemRht)[uRht];

                        break;

                    case ARRAY_CHAR:
                        tkRhtArg.tkFlags.ImmType  = IMMTYPE_CHAR;
                        tkRhtArg.tkData.tkChar    =
                        aplCharRht                = ((LPAPLCHAR)  lpMemRht)[uRht];

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Set the token & immediate types in case uDimAxRht EQ 1
                tkRhtArg.tkFlags.TknType = TKT_VARIMMED;

                // Loop backwards through the elements along the specified axis
                for (iDim = uDimAxRht - 2, uPrv = uDimRht + iDim * uDimHi; iDim >= 0; iDim--, uPrv -= uDimHi)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

////////////////////// Calculate the index of the previous element in this vector (now done in the FOR stmt)
////////////////////uPrv = uDimRht + iDim * uDimHi;

                    // Split cases based upon the left (actually right) arg storage type
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:
                            aplIntegerLft = GetNextInteger (lpMemRht, aplTypeRht, uPrv);
                            aplFloatLft   = (APLFLOAT) aplIntegerLft;   // In case of type promotion

                            break;

                        case ARRAY_INT:
                            aplIntegerLft = ((LPAPLINT)   lpMemRht)[uPrv];
                            aplFloatLft   = (APLFLOAT) aplIntegerLft;   // In case of type promotion

                            break;

                        case ARRAY_APA:
                            aplIntegerLft = apaOffRht + apaMulRht * uPrv;
                            aplFloatLft   = (APLFLOAT) aplIntegerLft;   // In case of type promotion

                            break;

                        case ARRAY_FLOAT:
                            aplFloatLft   = ((LPAPLFLOAT) lpMemRht)[uPrv];

                            break;

                        case ARRAY_CHAR:
                            aplCharLft    = ((LPAPLCHAR)  lpMemRht)[uPrv];

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    // Execute the Primitive Dyadic Scalar function
                    //   between the args
////#define FAST_TEST
#ifdef FAST_TEST
                    aplLongestRht += aplLongestLft;
#else
                    if (!PrimFnDydSiScSiScSub_EM (&tkRhtArg,
                                                  &lpYYFcnStrLft->tkToken,
                                                   aplTypeRes,
                                                   aplTypeRht2,
                                                   aplIntegerLft,
                                                   aplFloatLft,
                                                   aplCharLft,
                                                   aplTypeTmp,
                                                   aplIntegerRht,
                                                   aplFloatRht,
                                                   aplCharRht,
                                                   lpPrimSpecLft))
                        goto ERROR_EXIT;

                    Assert (tkRhtArg.tkFlags.TknType EQ TKT_VARIMMED);

                    // Copy the result type as the temporary right arg type
                    aplTypeTmp = aplTypeRes;

                    // Split cases based upon the result immediate type
                    switch (tkRhtArg.tkFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:
                            aplIntegerRht = (BIT0 &tkRhtArg.tkData.tkBoolean);
                            aplFloatRht   = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                            break;

                        case IMMTYPE_INT:
                            aplIntegerRht = tkRhtArg.tkData.tkInteger;
                            aplFloatRht   = (APLFLOAT) aplIntegerRht;   // In case of type promotion

                            break;

                        case IMMTYPE_FLOAT:
                            aplFloatRht   = tkRhtArg.tkData.tkFloat;
                            aplTypeRht2   =
                            aplTypeRes    =
                            lpMemHdrRes->ArrType = ARRAY_FLOAT;

                            break;

                        case IMMTYPE_CHAR:                          // Can't happen
                        defstop
                            break;
                    } // End SWITCH
#endif
                } // End FOR

#ifdef FAST_TEST
                // Fill in the result token
                tkRhtArg.tkFlags.TknType  = TKT_VARIMMED;
                tkRhtArg.tkFlags.ImmType  = IMMTYPE_INT;
                tkRhtArg.tkData.tkLongest = aplLongestRht;
#endif
            } else
            {
                // Get the last element as the right arg
                GetNextValueMemIntoToken (uRht,             // Index to use
                                          lpMemRht,         // Ptr to global memory object to index
                                          aplTypeRht,       // Storage type of the arg
                                          apaOffRht,        // APA offset (if needed)
                                          apaMulRht,        // APA multiplier (if needed)
                                         &tkRhtArg);        // Ptr to token in which to place the value
                // In case we blew up, check to see if we must blow up tkRhtArg
                if (IsSimpleFlt (aplTypeRes)
                 && IsSimpleInt (aplTypeRht))
                {
                    // Change the immediate type & value
                    tkRhtArg.tkFlags.ImmType = IMMTYPE_FLOAT;
                    tkRhtArg.tkData.tkFloat  = (APLFLOAT) tkRhtArg.tkData.tkInteger;
                } // End IF

                // Loop backwards through the elements along the specified axis
                for (iDim = uDimAxRht - 2; iDim >= 0; iDim--)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Calculate the index of the previous element in this vector
                    uPrv = uDimRht + iDim * uDimHi;

                    // Get the previous element as the left arg
                    GetNextValueMemIntoToken (uPrv,         // Index to use
                                              lpMemRht,     // Ptr to global memory object to index
                                              aplTypeRht,   // Storage type of the arg
                                              apaOffRht,    // APA offset (if needed)
                                              apaMulRht,    // APA multiplier (if needed)
                                             &tkLftArg);    // Ptr to token in which to place the value
                    // Execute the left operand between the left & right args
                    if (lpPrimProtoLft)
                        // Note that we cast the function strand to LPTOKEN
                        //   to bridge the two types of calls -- one to a primitive
                        //   function which takes a function token, and one to a
                        //   primitive operator which takes a function strand
                        lpYYRes = (*lpPrimProtoLft) (&tkLftArg,     // Ptr to left arg token
                                            (LPTOKEN) lpYYFcnStrLft,// Ptr to left operand function strand
                                                     &tkRhtArg,     // Ptr to right arg token
                                                      NULL);        // Ptr to axis token (may be NULL)
                    else
                        lpYYRes = ExecFuncStr_EM_YY (&tkLftArg,     // Ptr to left arg token
                                                      lpYYFcnStrLft,// Ptr to left operand function strand
                                                     &tkRhtArg,     // Ptr to right arg token
                                                      NULL);        // Ptr to axis token (may be NULL)
                    // Free the left & right arg tokens
                    FreeResult (&tkLftArg);
                    FreeResult (&tkRhtArg);

                    // If it succeeded, ...
                    if (lpYYRes)
                    {
                        // Copy the result to the right arg token
                        tkRhtArg = lpYYRes->tkToken;

                        // Free the YYRes (but not the storage)
                        YYFree (lpYYRes); lpYYRes = NULL;
                    } else
                        goto ERROR_EXIT;
                } // End FOR
            } // End IF/ELSE

            // Split cases based upon the token type of the right arg (result)
            switch (tkRhtArg.tkFlags.TknType)
            {
                case TKT_VARIMMED:
                    // Split cases based upon the result storage type
                    switch (aplTypeRes)
                    {
                        case ARRAY_BOOL:
                            // Check for blow up (to INT or FLOAT -- can this ever happen??)
                            if (IsImmBool (tkRhtArg.tkFlags.ImmType))
                            {
                                // Save in the result as a BOOL
                                *((LPAPLBOOL) lpMemRes) |= (BIT0 & tkRhtArg.tkData.tkBoolean) << uBitIndex;

                                // Check for end-of-byte
                                if (++uBitIndex EQ NBIB)
                                {
                                    uBitIndex = 0;              // Start over
                                    ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                                } // End IF
                            } else
                            {
                                DbgBrk ();      // ***TESTME*** -- Can this ever happen??





                            } // End IF/ELSE

                            break;

                        case ARRAY_INT:
                            // Check for blow up (to FLOAT)
                            if (IsImmInt (tkRhtArg.tkFlags.ImmType))
                                // Save in the result as an INT
                                *((LPAPLINT) lpMemRes)++ = tkRhtArg.tkData.tkInteger;
                            else
                            {
                                // It's now a FLOAT result
                                aplTypeRes = ARRAY_FLOAT;

                                // Tell the header about it
                                lpMemHdrRes->ArrType = aplTypeRes;

                                // Restart the pointer
                                lpMemRes = VarArrayBaseToData (lpMemHdrRes, aplRankRes);

                                goto RESTART_EXCEPTION;
                            } // End IF/ELSE

                            break;

                        case ARRAY_FLOAT:
                            // Save in the result as a FLOAT
                            *((LPAPLFLOAT) lpMemRes)++ = tkRhtArg.tkData.tkFloat;

                            break;

                        case ARRAY_NESTED:
                            // Save in the result as an LPSYMENTRY
                            *((LPAPLNESTED) lpMemRes)++ =
                            lpSymTmp =
                              MakeSymEntry_EM (tkRhtArg.tkFlags.ImmType,    // Immediate type
                                              &tkRhtArg.tkData.tkLongest,   // Ptr to immediate value
                                              &lpYYFcnStrOpr->tkToken);     // Ptr to function token
                            if (!lpSymTmp)
                                goto ERROR_EXIT;
                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                case TKT_VARARRAY:
                    // Save in the result as an HGLOBAL
                    *((LPAPLNESTED) lpMemRes)++ = tkRhtArg.tkData.tkGlbData;

                    break;

                defstop
                    break;
            } // End SWITCH

            // Because we didn't CopyArray_EM on the tkGlbData,
            //   we don't need to free tkRhtArg.
        } // End FOR/FOR
    } // End IF/ELSE

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
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

LEFT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

LEFT_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
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

    return lpYYRes;
} // End PrimOpMonSlashCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpMonSlashScalar_EM_YY
//
//  Handle the case where the right arg is a scalar
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonSlashScalar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpMonSlashScalar_EM_YY
    (LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     APLSTYPE     aplTypeRht,           // Right arg storage type
     APLLONGEST   aplLongestRht,        // Right arg immediate value
     HGLOBAL      hGlbRht,              // Right arg global memory handle
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPPL_YYSTYPE lpYYFcnStrLft,        // Ptr to left operand
     UBOOL        bPrototyping)         // TRUE iff prototyping

{
    HGLOBAL      hGlbPro = NULL;        // Prototype global memory handle
    LPPL_YYSTYPE lpYYRes = NULL;        // Ptr to the result

    // If it's a global, ...
    if (hGlbRht)
    {
        // If we're prototyping, ...
        if (bPrototyping)
        {
            // Make a copy of the right arg
            hGlbPro =
            hGlbRht =
              MakeMonPrototype_EM (MakePtrTypeGlb (hGlbRht),    // Proto arg global memory handle
                                  &lpYYFcnStrOpr->tkToken,      // Ptr to function token
                                   MP_NUMONLY);                 // Numerics only
            if (!hGlbRht)
                goto WSFULL_EXIT;

            Assert (GetPtrTypeDir (hGlbRht) EQ PTRTYPE_HGLOBAL);
        } else
            DbgIncrRefCntDir (MakePtrTypeGlb (hGlbRht));
    } else  // It's an immediate
    {
        // If we're prototyping, ...
        if (bPrototyping)
            aplLongestRht = IsSimpleChar (aplTypeRht) ? L' ' : 0;
    } // End IF/ELSE

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // If it's a global, ...
    if (hGlbRht)
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRht);
////////lpYYRes->tkToken.tkCharIndex       =                // Filled in below
    } else  // It's an immediate
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = TranslateArrayTypeToImmType (aplTypeRht);
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkLongest  = aplLongestRht;
////////lpYYRes->tkToken.tkCharIndex       =                // Filled in below
    } // End IF/ELSE

    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbPro)
    {
        // We no longer need this storage
        FreeResultGlobalVar (hGlbPro); hGlbPro = NULL;
    } // End IF
NORMAL_EXIT:
    return lpYYRes;
} // End PrimOpMonSlashScalar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SymGlbToToken
//
//  Move a Sym/Glb value to a token
//***************************************************************************

void SymGlbToToken
    (LPTOKEN lptkArg,
     HGLOBAL hSymGlb)

{
    // Split cases based upon the element's ptr type
    switch (GetPtrTypeDir (hSymGlb))
    {
        case PTRTYPE_STCONST:
            lptkArg->tkFlags.TknType   = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType   = ((LPSYMENTRY) hSymGlb)->stFlags.ImmType;
////////////lptkArg->tkFlags.NoDisplay = FALSE;         // Already zero by the caller
            lptkArg->tkData.tkLongest  = ((LPSYMENTRY) hSymGlb)->stData.stLongest;
////////////lptkArg->tkCharIndex       =                // Filled in by the caller

            break;

        case PTRTYPE_HGLOBAL:
            lptkArg->tkFlags.TknType   = TKT_VARARRAY;
////////////lptkArg->tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero by caller
////////////lptkArg->tkFlags.NoDisplay = FALSE;         // Already zero by the caller
            lptkArg->tkData.tkGlbData  = hSymGlb;
////////////lptkArg->tkCharIndex       =                // Filled in by the caller

            break;

        defstop
            break;
    } // End SWITCH
} // End SymGlbToToken


//***************************************************************************
//  $PrimOpDydSlash_EM_YY
//
//  Primitive operator for dyadic derived function from Slash ("N-wise reduction")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydSlash_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpDydSlashCommon_EM_YY (lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
                                       lpYYFcnStrOpr,   // Ptr to operator function strand
                                       lptkRhtArg,      // Ptr to right arg token
                                       FALSE);          // TRUE iff prototyping
} // End PrimOpDydSlash_EM_YY


//***************************************************************************
//  $PrimOpDydSlashCommon_EM_YY
//
//  Primitive operator for dyadic derived function from Slash ("N-wise reduction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydSlashCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDydSlashCommon_EM_YY
    (LPTOKEN      lptkLftArg,               // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,            // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,               // Ptr to right arg token
     UBOOL        bPrototyping)             // TRUE iff prototyping

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht,           // Right ...
                      aplTypeRes;           // Result   ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht,           // Right ...
                      aplNELMRes;           // Result   ...
    APLRANK           aplRankLft,           // Left arg rank
                      aplRankRht,           // Right ...
                      aplRankRes;           // Result   ...
    APLINT            aplIntegerLft,        // Left arg integer
                      aplIntegerLftAbs,     // ...              absolute value
                      apaOffRht,            // Right arg APA offset
                      apaMulRht;            // ...           multiplier
    APLFLOAT          aplFloatLft,          // Left arg float
                      aplFloatIdent;        // Identity element
    APLUINT           aplAxis,              // The (one and only) axis value
                      uLo,                  // uDimLo loop counter
                      uHi,                  // uDimHi ...
                      uDim,                 // Loop counter
                      uDimLo,               // Product of dimensions below axis
                      uDimHi,               // ...                   above ...
                      uDimRht,              // Starting index in right arg of current vector
                      uDimAxRht,            // Right arg axis dimension length
                      uDimAxRes,            // Result    ...
                      uRht,                 // Right arg loop counter
                      uRes;                 // Result    ...
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL;       // Result    ...
    LPVOID            lpMemRht = NULL,      // Ptr to right arg global memory
                      lpMemRes = NULL;      // Ptr to result    ...
    LPAPLDIM          lpMemDimRht;          // Ptr to right arg dimensions
    UBOOL             bRet = TRUE;          // TRUE iff result is valid
    LPPL_YYSTYPE      lpYYRes = NULL,       // Ptr to the result
                      lpYYFcnStrLft;        // Ptr to the left operand strand
    TOKEN             tkLftArg = {0},       // Left arg token
                      tkRhtArg = {0};       // Right ...
    LPTOKEN           lptkAxis;             // Ptr to axis token (may be NULL)
    LPPRIMFNS         lpPrimProtoLft;       // Ptr to left operand prototype function
    LPPRIMSPEC        lpPrimSpecLft;        // Ptr to left operand PRIMSPEC
    LPPRIMFLAGS       lpPrimFlagsLft;       // Ptr to left operand PrimFlags entry
    LPPRIMIDENT       lpPrimIdentLft;       // Ptr to left operand PrimIdent entry
    LPSYMENTRY        lpSymTmp;             // Ptr to temporary LPSYMENTRY
    LPVARARRAY_HEADER lpMemHdrRes;          // Ptr to result header
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check for axis operator
    lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Handle prototypes specially
    // Get a ptr to the prototype function for the first symbol (a function or operator)
    if (bPrototyping
     || IsEmpty (aplNELMLft)
     || IsEmpty (aplNELMRht))
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoLft = GetPrototypeFcnPtr (lpYYFcnStrLft);
        if (!lpPrimProtoLft)
            goto LEFT_NONCE_EXIT;
    } else
        lpPrimProtoLft = NULL;

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for LEFT LENGTH ERROR
    if (!IsSingleton (aplNELMLft))
        goto LEFT_LENGTH_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsSimpleNum (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Get the one (and only) value from the left arg
    GetFirstValueToken (lptkLftArg,         // Ptr to left arg token
                       &aplIntegerLft,      // Ptr to integer result
                       &aplFloatLft,        // Ptr to float ...
                        NULL,               // Ptr to WCHAR ...
                        NULL,               // Ptr to longest ...
                        NULL,               // Ptr to lpSym/Glb ...
                        NULL,               // Ptr to ...immediate type ...
                        NULL);              // Ptr to array type ...
    // If the left arg is a float,
    //   attempt to convert it to an integer
    if (IsSimpleFlt (aplTypeLft))
    {
        // Attempt to convert the float to an integer using System CT
        aplIntegerLft = FloatToAplint_SCT (aplFloatLft, &bRet);
        if (!bRet)
            goto LEFT_DOMAIN_EXIT;
    } // End IF

    // Calculate the absolute value of aplIntegerLft
    aplIntegerLftAbs = abs64 (aplIntegerLft);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Reverse allows a single integer axis value only
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRht,      // All values less than this
                           TRUE,            // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // Return TRUE iff fractional values present
                          &aplAxis,         // Return last axis value
                           NULL,            // Return # elements in axis vector
                           NULL))           // Return HGLOBAL with APLINT axis values
            return NULL;
    } else
    {
        // No axis specified:
        // if Slash, use last dimension
        if (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLASH
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_SLASH)
            aplAxis = max (aplRankRht, 1) - 1;
        else
            // Otherwise, it's SlashBar on the first dimension
            aplAxis = 0;
    } // End IF/ELSE

    //***************************************************************
    // Calculate product of dimensions before, at, and after the axis dimension
    //***************************************************************

    // Get right arg's global ptr
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg is not an immediate, ...
    if (lpMemRht)
    {
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

        // Calculate the product of the right arg's dimensions below the axis dimension
        uDimLo = 1;
        for (uDim = 0; uDim < aplAxis; uDim++)
            uDimLo *= lpMemDimRht[uDim];

        // Get the length of the axis dimension
        if (IsScalar (aplRankRht))
            uDimAxRht = 1;
        else
            uDimAxRht = lpMemDimRht[uDim];

        // Calculate the product of the right arg's dimensions above the axis dimension
        uDimHi = 1;
        for (uDim++; uDim < aplRankRht; uDim++)
            uDimHi *= lpMemDimRht[uDim];
    } else
        uDimLo = uDimAxRht = uDimHi = 1;

    // Check for more LEFT DOMAIN ERRORs
    if (aplIntegerLftAbs > (APLINT) (1 + uDimAxRht))
        goto LEFT_DOMAIN_EXIT;

    // Calculate the result axis dimension
    uDimAxRes = 1 + uDimAxRht - aplIntegerLftAbs;

    // The rank of the result is the same as that of the right arg
    //   or 1 whichever is higher
    aplRankRes = max (aplRankRht, 1);

    // Get a ptr to the Primitive Function Flags
    lpPrimFlagsLft = GetPrimFlagsPtr (lpYYFcnStrLft);
    if (lpPrimFlagsLft)
        lpPrimIdentLft = &PrimIdent[lpPrimFlagsLft->Index];
    else
        lpPrimIdentLft = NULL;

    // Use all zero PrimFlags if not present
    if (!lpPrimFlagsLft)
        lpPrimFlagsLft = &PrimFlags0;

    // If the left arg is zero, or
    //    the absolute value of the left arg is (uDimAxRht + 1),
    //    the result is a reshape of LeftOperandIdentityElement,
    //    so we need that value
    if (aplIntegerLft EQ 0
     || aplIntegerLftAbs EQ (APLINT) (uDimAxRht + 1))
    {
        // If it's not an immediate primitive function,
        //   or it is, but is without an identity element,
        //   signal a LEFT DOMAIN ERROR
        if (!lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
         || !lpPrimFlagsLft->IdentElem)
            goto LEFT_DOMAIN_EXIT;

        // Get the identity element
        aplFloatIdent = lpPrimIdentLft->fIdentElem;

        // If the identity element is Boolean or we're prototyping,
        //   the result is, too
        if (lpPrimIdentLft->IsBool
         || lpPrimProtoLft NE NULL)
            aplTypeRes = ARRAY_BOOL;
        else
            aplTypeRes = ARRAY_FLOAT;
    } else
    // If the operand is a primitive scalar dyadic function,
    //   calculate the storage type of the result,
    //   otherwise, assume it's ARRAY_NESTED
    if (lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
     && lpPrimFlagsLft->DydScalar)
    {
        // If the function is equal or not-equal, and the right
        //   arg is not Boolean, make the result storage type
        //   nested and let TypeDemote figure it out in the end.
        // This avoids complications with =/3 1{rho}'abc' which
        //   is typed as Boolean but blows up to CHAR.
        if ((lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_EQUAL
          || lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_NOTEQUAL)
         && !IsSimpleBool (aplTypeRht))
            aplTypeRes = ARRAY_NESTED;
        else
        {
            // Get the corresponding lpPrimSpecLft
            lpPrimSpecLft = PrimSpecTab[SymTrans (&lpYYFcnStrLft->tkToken)];

            // Calculate the storage type of the result
            aplTypeRes =
              (*lpPrimSpecLft->StorageTypeDyd) (1,
                                               &aplTypeRht,
                                               &lpYYFcnStrLft->tkToken,
                                                1,
                                               &aplTypeRht);
            if (aplTypeRes EQ ARRAY_ERROR)
                goto DOMAIN_EXIT;
        } // End IF/ELSE
    } else
        // Assume that the result storage type is nested
        //   but we'll call TypeDemote at the end just in case
        aplTypeRes = ARRAY_NESTED;

    // Calculate the result NELM
    aplNELMRes = _imul64 (uDimLo, uDimHi, &bRet);
    if (bRet || IsZeroDim (uDimAxRes))
        aplNELMRes = _imul64 (aplNELMRes, uDimAxRes, &bRet);
    if (!bRet)
        goto WSFULL_EXIT;

    //***************************************************************
    // Pick off special cases of the left arg
    //***************************************************************

    // If the left arg is zero, the result is
    //   ({rho} Result) {rho} LeftOperandIdentityElement
    //   where ({rho} Result) is ({rho} RightArg)
    if (aplIntegerLft EQ 0)
    {
        // Allocate storage for the result
        if (!PrimOpDydSlashAllocate_EM
             (aplTypeRht,           // Right arg storage type
              max (aplRankRht, 1),  // Right arg rank
              aplTypeRes,           // Result storage type
              aplNELMRes,           // Result NELM
              aplRankRes,           // Result rank
              lpMemRht,             // Ptr to right arg global memory
              lpMemDimRht,          // Ptr to right arg dimensions
              aplAxis,              // The (one and only) axis value
              uDimAxRes,            // Result axis value dimension length
             &hGlbRes,              // Ptr to result global memory handle
             &lpMemRes,             // Ptr to ptr to result global memory
             &apaOffRht,            // Ptr to right arg APA offset
             &apaMulRht,            // ...                  multiplier
              lpYYFcnStrOpr))       // Ptr to operator function strand
            goto ERROR_EXIT;

        // Fill in the data values (i.e., replicate the identity element)

        // The zero case is done (already zero from GHND)

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
    } else
    // If the absolute value of the left arg is one, the result is
    //   the right arg
    if (aplIntegerLftAbs EQ 1 && hGlbRht)
        hGlbRes = CopySymGlbDirAsGlb (hGlbRht);
    else
    // If the left arg is uDimAxRht, the result is
    //   ({rho} Result) {rho} LeftOperand /[X] RightArg
    //   where ({rho} Result) is ({rho} RightArg) with
    //   ({rho} RightArg})[X] set to one.
    if (aplIntegerLft EQ (APLINT) uDimAxRht)
    {
        // Reduce the right arg
        lpYYRes = PrimOpMonSlashCommon_EM_YY (lpYYFcnStrOpr,    // Ptr to operator function strand
                                              lptkRhtArg,       // Ptr to right arg
                                              bPrototyping);    // TRUE iff prototyping
        // Insert a unit dimension into the result
        if (!PrimOpDydSlashInsertDim_EM (lpYYRes,       // Ptr to the result
                                         aplAxis,       // The (one and only) axis value
                                         uDimAxRes,     // Result axis dimension length
                                        &hGlbRes,       // Ptr to the result global memory handle
                                         lpYYFcnStrOpr))// Ptr to operator function strand
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } else
    // If the left arg is -uDimAxRht, the result is
    //   ({rho} Result) {rho} LeftOperand /[X] {reverse}[X] RightArg
    //   where ({rho} Result) is ({rho} RightArg) with
    //   ({rho} RightArg})[X] set to one
    if (aplIntegerLft EQ -(APLINT) uDimAxRht)
    {
        LPPL_YYSTYPE lpYYRes2;

        // Reverse the right arg along the specified axis
        lpYYRes2 = PrimFnMonCircleStile_EM_YY (&lpYYFcnStrOpr->tkToken, // Ptr to function token
                                                lptkRhtArg,             // Ptr to right arg token
                                                lptkAxis);              // Ptr to axis token (may be NULL)
        // If it failed, ...
        if (!lpYYRes2)
            goto ERROR_EXIT;

        // Reduce the reversed right arg along the specified axis
        lpYYRes = PrimOpMonSlashCommon_EM_YY (lpYYFcnStrOpr,        // Ptr to operator function strand
                                             &lpYYRes2->tkToken,    // Ptr to right arg
                                              bPrototyping);        // TRUE iff prototyping
        // Free the result of the function execution
        FreeResult (&lpYYRes2->tkToken); YYFree (lpYYRes2); lpYYRes2 = NULL;

        // Insert a unit dimension into the result
        if (!PrimOpDydSlashInsertDim_EM (lpYYRes,       // Ptr to the result
                                         aplAxis,       // The (one and only) axis value
                                         uDimAxRes,     // Result axis dimension length
                                        &hGlbRes,       // Ptr to the result global memory handle
                                         lpYYFcnStrOpr))// Ptr to operator function strand
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } else
    // If the absolute value of the left arg is (uDimAxRht + 1), the result is
    //   ({rho} result) {rho} LeftOperandIdentityElement
    //   where ({rho} Result) is ({rho} RightArg) with
    //   ({rho} RightArg})[X] set to zero
    if (aplIntegerLftAbs EQ (APLINT) (uDimAxRht + 1))
    {
        // The result is empty and the prototype is the identity element
        //   for the left operand ==> numeric ==> zero
        //   ==> the result storage type is Boolean
        aplTypeRes = ARRAY_BOOL;

        // Allocate storage for the result
        if (!PrimOpDydSlashAllocate_EM
             (aplTypeRht,           // Right arg storage type
              max (aplRankRht, 1),  // Right arg rank
              aplTypeRes,           // Result storage type
              aplNELMRes,           // Result NELM
              aplRankRes,           // Result rank
              lpMemRht,             // Ptr to right arg global memory
              lpMemDimRht,          // Ptr to right arg dimensions
              aplAxis,              // The (one and only) axis value
              uDimAxRes,            // Result axis value dimension length

             &hGlbRes,              // Ptr to result global memory handle
             &lpMemRes,             // Ptr to ptr to result global memory
             &apaOffRht,            // Ptr to right arg APA offset
             &apaMulRht,            // ...                  multiplier
              lpYYFcnStrOpr))       // Ptr to operator function strand
            goto ERROR_EXIT;
    } else
    {
        // Otherwise, we're out of special cases

        // Allocate storage for the result
        if (!PrimOpDydSlashAllocate_EM
             (aplTypeRht,           // Right arg storage type
              max (aplRankRht, 1),  // Right arg rank
              aplTypeRes,           // Result storage type
              aplNELMRes,           // Result NELM
              aplRankRes,           // Result rank
              lpMemRht,             // Ptr to right arg global memory
              lpMemDimRht,          // Ptr to right arg dimensions
              aplAxis,              // The (one and only) axis value
              uDimAxRes,            // Result axis value dimension length

             &hGlbRes,              // Ptr to result global memory handle
             &lpMemRes,             // Ptr to ptr to result global memory
             &apaOffRht,            // Ptr to right arg APA offset
             &apaMulRht,            // ...                  multiplier
              lpYYFcnStrOpr))       // Ptr to operator function strand
            goto ERROR_EXIT;

        // Save a ptr to the array header
        // Note that PrimOpDydSlashAllocate returns a ptr to the data
        //   so we have to relock/unlock
        lpMemHdrRes = MyGlobalLock (hGlbRes); MyGlobalUnlock (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

        // Fill in the right arg token
////////tkRhtArg.tkFlags.TknType   =            // To be filled in below
////////tkRhtArg.tkFlags.ImmType   =            // To be filled in below
////////tkRhtArg.tkFlags.NoDisplay = FALSE;     // Already zero from {0}
////////tkRhtArg.tkData.tkGlbData  =            // To be filled in below
        tkRhtArg.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

        // Fill in the left arg token
////////tkLftArg.tkFlags.TknType   =            // To be filled in below
////////tkLftArg.tkFlags.ImmType   =            // To be filled in below
////////tkLftArg.tkFlags.NoDisplay = FALSE;     // Already zero from {0}
////////tkLftArg.tkData.tkGlbData  =            // To be filled in below
        tkLftArg.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;
RESTART_EXCEPTION:
        // Initialize index into the result
        uRes = 0;

        // Loop through the right arg calling the
        //   function strand between data, storing in the
        //   result.
        for (uLo = 0; uLo < uDimLo; uLo++)
        for (uHi = 0; uHi < uDimHi; uHi++)
        {
            APLUINT uAx;            // Loop counter for uDimAxRes
            APLINT  iDim;           // Loop counter for aplIntegerLftAbs

            // Calculate the starting index in the right arg of this vector
            uDimRht = uLo * uDimHi * uDimAxRht + uHi;

            // There are uDimAxRes elements in the result to be generated now
            for (uAx = 0; uAx < uDimAxRes; uAx++)
            {
                // Reduce the aplIntegerLftAbs values starting at
                //   uRht = uDimRht + uAx * uDimHi;
                // through
                //   uRht = uDimRht + (uAx + aplIntegerLftAbs - 1) * uDimHi;

                // Split cases based upon the sign of the left arg
                if (aplIntegerLft > 0)
                {
                    // Calculate the index of last element in this vector
                    uRht = uDimRht + (uAx + aplIntegerLftAbs - 1) * uDimHi;

                    // Get the last element as the right arg
                    GetNextValueMemIntoToken (uRht,             // Index to use
                                              lpMemRht,         // Ptr to global memory object to index
                                              aplTypeRht,       // Storage type of the arg
                                              apaOffRht,        // APA offset (if needed)
                                              apaMulRht,        // APA multiplier (if needed)
                                             &tkRhtArg);        // Ptr to token in which to place the value
                    // In case we blew up, check to see if we must blow up tkRhtArg
                    if (IsSimpleFlt (aplTypeRes)
                     && IsSimpleInt (aplTypeRht))
                    {
                        // Change the immediate type & value
                        tkRhtArg.tkFlags.ImmType = IMMTYPE_FLOAT;
                        tkRhtArg.tkData.tkFloat  = (APLFLOAT) tkRhtArg.tkData.tkInteger;
                    } // End IF

                    // Loop backwards through the elements along the specified axis
                    for (iDim = aplIntegerLftAbs - 2; iDim >= 0; iDim--)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Calculate the index of the previous element in this vector
                        uRht = uDimRht + (uAx + iDim) * uDimHi;

                        // Get the previous element as the left arg
                        GetNextValueMemIntoToken (uRht,         // Index to use
                                                  lpMemRht,     // Ptr to global memory object to index
                                                  aplTypeRht,   // Storage type of the arg
                                                  apaOffRht,    // APA offset (if needed)
                                                  apaMulRht,    // APA multiplier (if needed)
                                                 &tkLftArg);    // Ptr to token in which to place the value
                        // Execute the left operand between the left & right args
                        if (lpPrimProtoLft)
                            // Note that we cast the function strand to LPTOKEN
                            //   to bridge the two types of calls -- one to a primitive
                            //   function which takes a function token, and one to a
                            //   primitive operator which takes a function strand
                            lpYYRes = (*lpPrimProtoLft) (&tkLftArg,         // Ptr to left arg token
                                                (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand function strand
                                                         &tkRhtArg,         // Ptr to right arg token
                                                          lptkAxis);        // Ptr to axis token (may be NULL)
                        else
                            lpYYRes = ExecFuncStr_EM_YY (&tkLftArg,         // Ptr to left arg token
                                                          lpYYFcnStrLft,    // Ptr to function strand
                                                         &tkRhtArg,         // Ptr to right arg token
                                                          lptkAxis);        // Ptr to axis token (may be NULL)
                        // Free the left & right arg tokens
                        FreeResult (&tkRhtArg);
                        FreeResult (&tkLftArg);

                        // If it succeeded, ...
                        if (lpYYRes)
                        {
                            // Copy the result to the right arg token
                            tkRhtArg = lpYYRes->tkToken;

                            // Free the YYRes (but not the storage)
                            YYFree (lpYYRes); lpYYRes = NULL;
                        } else
                            goto ERROR_EXIT;
                    } // End FOR
                } else
                {
                    // Calculate the index of first element in this vector
                    uRht = uDimRht + uAx * uDimHi;

                    // Get the first element as the right arg
                    GetNextValueMemIntoToken (uRht,             // Index to use
                                              lpMemRht,         // Ptr to global memory object to index
                                              aplTypeRht,       // Storage type of the arg
                                              apaOffRht,        // APA offset (if needed)
                                              apaMulRht,        // APA multiplier (if needed)
                                             &tkRhtArg);        // Ptr to token in which to place the value
                    // In case we blew up, check to see if we must blow up tkRhtArg
                    if (IsSimpleFlt (aplTypeRes)
                     && IsSimpleInt (aplTypeRht))
                    {
                        // Change the immediate type & value
                        tkRhtArg.tkFlags.ImmType = IMMTYPE_FLOAT;
                        tkRhtArg.tkData.tkFloat  = (APLFLOAT) tkRhtArg.tkData.tkInteger;
                    } // End IF

                    // Loop forwards through the elements along the specified axis
                    for (iDim = 1; iDim < aplIntegerLftAbs; iDim++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Calculate the index of the next element in this vector
                        uRht = uDimRht + (uAx + iDim) * uDimHi;

                        // Get the next element as the left arg
                        GetNextValueMemIntoToken (uRht,         // Index to use
                                                  lpMemRht,     // Ptr to global memory object to index
                                                  aplTypeRht,   // Storage type of the arg
                                                  apaOffRht,    // APA offset (if needed)
                                                  apaMulRht,    // APA multiplier (if needed)
                                                 &tkLftArg);    // Ptr to token in which to place the value
                        // Execute the left operand between the left & right args
                        if (lpPrimProtoLft)
                            // Note that we cast the function strand to LPTOKEN
                            //   to bridge the two types of calls -- one to a primitive
                            //   function which takes a function token, and one to a
                            //   primitive operator which takes a function strand
                            lpYYRes = (*lpPrimProtoLft) (&tkLftArg,         // Ptr to left arg token
                                                (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand function strand
                                                         &tkRhtArg,         // Ptr to right arg token
                                                          lptkAxis);        // Ptr to axis token (may be NULL)
                        else
                            lpYYRes = ExecFuncStr_EM_YY (&tkLftArg,         // Ptr to left arg token
                                                          lpYYFcnStrLft,    // Ptr to function strand
                                                         &tkRhtArg,         // Ptr to right arg token
                                                          lptkAxis);        // Ptr to axis token (may be NULL)
                        // Free the left & right arg tokens
                        FreeResult (&tkRhtArg);
                        FreeResult (&tkLftArg);

                        // If it succeeded, ...
                        if (lpYYRes)
                        {
                            // Copy the result to the right arg token
                            tkRhtArg = lpYYRes->tkToken;

                            // Free the YYRes (but not the storage)
                            YYFree (lpYYRes); lpYYRes = NULL;
                        } else
                            goto ERROR_EXIT;
                    } // End FOR
                } // End IF/ELSE

                // Split cases based upon the token type of the right arg (result)
                switch (tkRhtArg.tkFlags.TknType)
                {
                    case TKT_VARIMMED:
                        // Split cases based upon the result storage type
                        switch (aplTypeRes)
                        {
                            case ARRAY_BOOL:
                                // Check for blow up (to INT or FLOAT -- can this ever happen??)
                                if (IsImmBool (tkRhtArg.tkFlags.ImmType))
                                    // Save in the result as a BOOL
                                    ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |=
                                      (BIT0 & tkRhtArg.tkData.tkBoolean) << (MASKLOG2NBIB & (UINT) uRes);
                                else
                                {
                                    DbgBrk ();      // ***TESTME*** -- Can this ever happen??





                                } // End IF/ELSE

                                break;

                            case ARRAY_INT:
                                // Check for blow up (to FLOAT)
                                if (IsImmInt (tkRhtArg.tkFlags.ImmType))
                                    // Save in the result as an INT
                                    ((LPAPLINT) lpMemRes)[uRes] = tkRhtArg.tkData.tkInteger;
                                else
                                {
                                    // It's now a FLOAT result
                                    aplTypeRes = ARRAY_FLOAT;

                                    // Tell the header about it
                                    lpMemHdrRes->ArrType = aplTypeRes;

                                    // Restart the pointer
                                    lpMemRes = VarArrayBaseToData (lpMemHdrRes, aplRankRes);

                                    goto RESTART_EXCEPTION;
                                } // End IF/ELSE

                                break;

                            case ARRAY_FLOAT:
                                // Save in the result as a FLOAT
                                ((LPAPLFLOAT) lpMemRes)[uRes] = tkRhtArg.tkData.tkFloat;

                                break;

                            case ARRAY_NESTED:
                                // Save in the result as an LPSYMENTRY
                                ((LPAPLNESTED) lpMemRes)[uRes] =
                                lpSymTmp =
                                  MakeSymEntry_EM (tkRhtArg.tkFlags.ImmType,    // Immediate type
                                                  &tkRhtArg.tkData.tkLongest,   // Ptr to immediate value
                                                  &lpYYFcnStrOpr->tkToken);     // Ptr to function token
                                if (!lpSymTmp)
                                    goto ERROR_EXIT;
                                break;

                            defstop
                                break;
                        } // End SWITCH

                        break;

                    case TKT_VARARRAY:
                        // Save in the result as an HGLOBAL
                        ((LPAPLNESTED) lpMemRes)[uRes] = tkRhtArg.tkData.tkGlbData;

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Skip to the next element in the result
                uRes++;
            } // End FOR
        } // End FOR/FOR
    } // End IF/ELSE/.../FOR/FOR

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
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

LEFT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

LEFT_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

LEFT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (lpYYRes)
    {
        YYFree (lpYYRes); lpYYRes = NULL;
    } // End IF

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

    return lpYYRes;
} // End PrimOpDydSlashCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydSlashInsertDim_EM
//
//  Insert a unit dimension into a result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydSlashInsertDim_EM_YY"
#else
#define APPEND_NAME
#endif

UBOOL PrimOpDydSlashInsertDim_EM
    (LPPL_YYSTYPE lpYYRes,          // Ptr to the result
     APLUINT      aplAxis,          // The (one and only) axis value
     APLUINT      uDimAxRes,        // Result axis dimension length
     HGLOBAL     *lphGlbRes,        // Ptr to result global memory handle
     LPPL_YYSTYPE lpYYFcnStrOpr)    // Ptr to operator function strand

{
    HGLOBAL hGlbTmp;            // Temporary global memory handle
    APLUINT ByteRes;            // # bytes in the result
    LPVOID  lpMemRes;           // Ptr to result global memory

    // If it failed, ...
    if (!lpYYRes)
        return FALSE;

    // Check for immediate result
    if (lpYYRes->tkToken.tkFlags.TknType EQ TKT_VARIMMED)
    {
        IMM_TYPES  immType;         // Datatype of immediate result (see IMM_TYPES)
        APLLONGEST aplLongest;      // Value of immediate result
        APLUINT    ByteRes;         // # bytes in the result
        APLSTYPE   aplTypeRes;      // Result storage type

        // Save the immediate type
        immType = lpYYRes->tkToken.tkFlags.ImmType;

        // Save the value
        aplLongest = lpYYRes->tkToken.tkData.tkLongest;

        // Calculate the result storage type
        aplTypeRes = TranslateImmTypeToArrayType (immType);

        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRes, 1, 1); // One-element vector
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;
        // Allocate space for the result
        hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (!hGlbTmp)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbTmp);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
        // Fill in the header values
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = aplTypeRes;
////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = 1;
        lpHeader->Rank       = 1;
#undef  lpHeader

        // Fill in the dimension
        *VarArrayBaseToDim (lpMemRes) = 1;

        // Skip over the header and dimension
        lpMemRes = VarArrayBaseToData (lpMemRes, 1);

        // Fill in the value
        // Split cases based upon the immediate type
        switch (immType)
        {
            case IMMTYPE_BOOL:
                *((LPAPLBOOL)  lpMemRes) = (APLBOOL) aplLongest;

                break;

            case IMMTYPE_INT:
                *((LPAPLINT)   lpMemRes) = (APLINT) aplLongest;

                break;

            case IMMTYPE_FLOAT:
                *((LPAPLFLOAT) lpMemRes) = *(LPAPLFLOAT) &aplLongest;

                break;

            case IMMTYPE_CHAR:
                *((LPAPLCHAR)  lpMemRes) = (APLCHAR) aplLongest;

                break;

            defstop
                break;
        } // End SWITCH

        // We no longer need this ptr
        MyGlobalUnlock (hGlbTmp); lpMemRes = NULL;

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbTmp);
////////lpYYRes->tkToken.tkCharIndex       =        // Already filled in by caller

        return TRUE;
    } // End IF

    // Get the global memory handle
    hGlbTmp = lpYYRes->tkToken.tkData.tkGlbData;

    Assert (IsGlbTypeVarDir (hGlbTmp));

    // Clear the ptr type bits
    hGlbTmp = ClrPtrTypeDir (hGlbTmp);

    // The result is the same as lpYYRes except we need
    //   to insert a unit dimension between aplAxis and
    //   aplAxis + 1.

    // Get the size of the temporary global memory
    //   plus a new dimension
    ByteRes = MyGlobalSize (hGlbTmp) + sizeof (APLDIM);
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;
    // Resize the global memory to include a new dimension
    *lphGlbRes = MyGlobalReAlloc (hGlbTmp,
                                  (APLU3264) ByteRes,
                                  GHND);
    // Check for errors
    if (*lphGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // If the handle is moveable, GlobalReAlloc returns the same handle
    Assert (hGlbTmp EQ *lphGlbRes);

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (*lphGlbRes);

    // Move the memory upwards to make room for a new dimension
    // Use MoveMemory as the source and destin blocks overlap
    MoveMemory (ByteAddr (lpMemRes, sizeof (VARARRAY_HEADER) + sizeof (APLDIM) * (aplAxis + 1)),
                ByteAddr (lpMemRes, sizeof (VARARRAY_HEADER) + sizeof (APLDIM) *  aplAxis     ),
                (APLU3264) (ByteRes  - (sizeof (VARARRAY_HEADER) + sizeof (APLDIM) *  aplAxis     )));
    // Increase the rank by one
    ((LPVARARRAY_HEADER) lpMemRes)->Rank++;

    // Insert the new dimension
    (VarArrayBaseToDim (lpMemRes))[aplAxis] = uDimAxRes;

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;

    return TRUE;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    return FALSE;
} // End PrimOpDydSlashInsertDim_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydSlashAllocate_EM
//
//  Allocate storage for the result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydSlashAllocate_EM_YY"
#else
#define APPEND_NAME
#endif

UBOOL PrimOpDydSlashAllocate_EM
    (APLRANK      aplTypeRht,       // Right arg storage type
     APLRANK      aplRankRht,       // Right arg rank
     APLSTYPE     aplTypeRes,       // Result storage type
     APLNELM      aplNELMRes,       // Result NELM
     APLRANK      aplRankRes,       // Result rank
     LPVOID       lpMemRht,         // Ptr to right arg global memory
     LPAPLDIM     lpMemDimRht,      // Ptr to right arg dimensions
     APLUINT      aplAxis,          // Axis value
     APLUINT      uDimAxRes,        // Result axis value

     HGLOBAL     *lphGlbRes,        // Ptr to result global memory handle
     LPVOID      *lplpMemRes,       // Ptr to ptr to result global memory
     LPAPLINT     lpapaOffRht,      // Ptr to right arg APA offset
     LPAPLINT     lpapaMulRht,      // ...                  multiplier
     LPPL_YYSTYPE lpYYFcnStrOpr)    // Ptr to operator function strand

{
    APLUINT  ByteRes,           // # bytes in the result
             uRes,              // Result loop counter
             uDim;              // Dimension loop counter

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Allocate space for the result
    // N.B. Conversion from APLUINT to UINT.
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;
    *lphGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!*lphGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    *lplpMemRes = MyGlobalLock (*lphGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) *lplpMemRes)
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    *lplpMemRes = VarArrayBaseToDim (*lplpMemRes);

    // Copy the dimensions from the right arg to the result
    //   except for the axis dimension
    for (uDim = 0; uDim < aplRankRht; uDim++)
    if (uDim EQ aplAxis)
        *((LPAPLDIM) *lplpMemRes)++ = uDimAxRes;
    else
        *((LPAPLDIM) *lplpMemRes)++ = lpMemDimRht[uDim];

    // lpMemRes now points to its data

    // If the right arg is an APA, ...
    if (IsSimpleAPA (aplTypeRht))
    {
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        *lpapaOffRht = lpAPA->Off;
        *lpapaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    if (IsNested (aplTypeRes))
    {
        // Fill nested result with PTR_REUSED
        //   in case we fail part way through
        *((LPAPLNESTED) *lplpMemRes) = PTR_REUSED;
        for (uRes = 1; uRes < aplNELMRes; uRes++)
            ((LPAPLNESTED) *lplpMemRes)[uRes] = PTR_REUSED;
    } // End IF

    return TRUE;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    return FALSE;
} // End PrimOpDydSlashAllocate_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_slash.c
//***************************************************************************
