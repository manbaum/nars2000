//***************************************************************************
//  NARS2000 -- Primitive Operator -- Slope
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

extern FASTBOOLFNS FastBoolFns[];   // ***FIXME*** -- move into externs.h


//***************************************************************************
//  $PrimOpSlope_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    monadic operator Slope ("scan" and "ERROR")
//***************************************************************************

LPPL_YYSTYPE PrimOpSlope_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLOPE
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLOPEBAR
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_SLOPE         // For when we come in via TKT_OP3NAMED
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_SLOPEBAR);    // ...

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (&lpYYFcnStrOpr->tkToken);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonSlope_EM_YY (lpYYFcnStrOpr, // Ptr to operator function strand
                                     lptkRhtArg);   // Ptr to right arg
    else
        return PrimOpDydSlope_EM_YY (lptkLftArg,    // Ptr to left arg token
                                     lpYYFcnStrOpr, // Ptr to operator function strand
                                     lptkRhtArg);   // Ptr to right arg token
} // End PrimOpSlope_EM_YY


//***************************************************************************
//  $PrimProtoOpSlope_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator Slope ("scan" and "ERROR")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpSlope_EM_YY
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
        return PrimOpMonSlopeCommon_EM_YY (lpYYFcnStrOpr,   // Ptr to operator function strand
                                           lptkRhtArg,      // Ptr to right arg token
                                           TRUE);           // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return PrimOpDydSlopeCommon_EM_YY (lptkLftArg,      // Ptr to left arg token
                                           lpYYFcnStrOpr,   // Ptr to operator function strand
                                           lptkRhtArg,      // Ptr to right arg token
                                           TRUE);           // TRUE iff prototyping
} // End PrimProtoOpSlope_EM_YY


//***************************************************************************
//  $PrimOpMonSlope_EM_YY
//
//  Primitive operator for monadic derived function from Slope ("scan")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonSlope_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpMonSlopeCommon_EM_YY (lpYYFcnStrOpr,       // Ptr to operator function strand
                                       lptkRhtArg,          // Ptr to right arg token
                                       FALSE);              // TRUE iff prototyping
} // End PrimOpMonSlope_EM_YY


//***************************************************************************
//  $PrimOpMonSlopeCommon_EM_YY
//
//  Primitive operator for monadic derived function from Slope ("scan")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonSlopeCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpMonSlopeCommon_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,            // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,               // Ptr to right arg token
     BOOL         bPrototyping)             // TRUE if prototyping

{
    APLSTYPE          aplTypeRht,           // Right arg storage type
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
                      uRes,                 // Result loop counter
                      ByteRes;              // # bytes in the result
    APLINT            apaOffRht,            // Right arg APA offset
                      apaMulRht;            // ...           multiplier
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL;       // Result    ...
    LPVOID            lpMemRht = NULL,      // Ptr to right arg global memory
                      lpMemRes = NULL;      // Ptr to result    ...
    LPAPLDIM          lpMemDimRht;          // Ptr to right arg dimensions
    BOOL              bRet = TRUE,          // TRUE iff result is valid
                      bFastBool = FALSE;    // TRUE iff this is a Fast Boolean operation
    LPPRIMSPEC        lpPrimSpec;           // Ptr to local PRIMSPEC
    LPPRIMFLAGS       lpPrimFlags;          // Ptr to corresponding PrimFlags entry
    TOKEN             tkLftArg = {0},       // Left arg token
                      tkRhtArg = {0};       // Right ...
    LPTOKEN           lptkAxis;             // Ptr to axis token (may be NULL)
    LPPL_YYSTYPE      lpYYRes = NULL,       // Ptr to the result
                      lpYYFcnStrLft;        // Ptr to left operand function strand
    LPPRIMFNS         lpPrimProtoLft;       // Ptr to left operand prototype function
    LPSYMENTRY        lpSymTmp;             // Ptr to temporary LPSYMENTRY
    LPVARARRAY_HEADER lpMemHdrRes;          // Ptr to result header

    // Check for axis operator
    lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken))
        goto LEFT_SYNTAX_EXIT;

    // Get a ptr to the prototype function for the first symbol (a function or operator)
    if (bPrototyping)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoLft = GetPrototypeFcnPtr (lpYYFcnStrLft);
        if (!lpPrimProtoLft)
            goto LEFT_NONCE_EXIT;
    } else
        lpPrimProtoLft = NULL;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

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
        // if Slope, use last dimension
        if (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLOPE
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_SLOPE)
            aplAxis = aplRankRht - 1;
        else
            // Otherwise, it's SlopeBar on the first dimension
            aplAxis = 0;
    } // End IF/ELSE

    // Get right arg's global ptr
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg is a scalar, return it
    if (IsScalar (aplRankRht))
    {
        lpYYRes = PrimOpMonSlashScalar_EM_YY (lptkRhtArg,       // Ptr to right arg token
                                              hGlbRht,          // Right arg global memory handle
                                              lpMemRht,         // Ptr to right arg global memory
                                              lpYYFcnStrOpr,    // Ptr to operator function strand
                                              bPrototyping);    // TRUE iff prototyping
        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // From here on, the right arg is a vector or higher rank array
    //***************************************************************

    // The rank of the result is the same as that of the right arg
    aplRankRes = aplRankRht;

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
    aplNELMRes = imul64 (uDimLo, uDimHi, &bRet);
    if (bRet || IsZeroDim (uDimAxRht))
        aplNELMRes = imul64 (aplNELMRes, uDimAxRht, &bRet);
    if (!bRet)
        goto WSFULL_EXIT;

    // If the right arg is empty, return it
    if (IsEmpty (aplNELMRht))
    {
        hGlbRes = CopySymGlbDirAsGlb (MakePtrTypeGlb (hGlbRht));

        goto YYALLOC_EXIT;
    } // End IF

    // Get a ptr to the Primitive Function Flags
    lpPrimFlags = GetPrimFlagsPtr (lpYYFcnStrLft);

    // If the product of the dimensions above
    //   the axis dimension is one, and
    //   this is a primitive function, and
    //   the right arg is Boolean or APA Boolean, and
    //   the axis dimension is > 1, and
    //   we're not doing prototypes, then
    //   check for the possibility of doing a
    //   Fast Boolean Scan
    if (IsUnitDim (uDimHi)
     && lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
     && (IsSimpleBool (aplTypeRht)
      || (IsSimpleAPA (aplTypeRht)
       && IsBooleanValue (apaOffRht)
       && apaMulRht EQ 0 ))
     && IsMultiDim (uDimAxRht)
     && lpPrimProtoLft EQ NULL
     && lpPrimFlags
     && lpPrimFlags->FastBool)
    {
        // Mark as a Fast Boolean operation
        bFastBool = TRUE;

        // The storage type is Boolean
        aplTypeRes = ARRAY_BOOL;
    } else
    // If the operand is a primitive scalar dyadic function,
    //   calculate the storage type of the result,
    //   otherwise, assume it's ARRAY_NESTED
    if (lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
     && lpPrimFlags->DydScalar)
    {
        // If the function is equal or not-equal, and the right
        //   arg is not Boolean, make the result storage type
        //   nested and let TypeDemote figure it out in the end.
        // This avoids complications with =\3 1{rho}'abc' which
        //   is typed as Boolean but blows up to HETERO.
        if ((lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_EQUAL
          || lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_NOTEQUAL)
         && !IsSimpleBool (aplTypeRht))
            aplTypeRes = ARRAY_NESTED;
        else
        {
            // Get the corresponding lpPrimSpec
            lpPrimSpec = PrimSpecTab[SymTrans (&lpYYFcnStrLft->tkToken)];

            // Calculate the storage type of the result
            aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (1,
                                                       &aplTypeRht,
                                                       &lpYYFcnStrLft->tkToken,
                                                        1,
                                                       &aplTypeRht);
            if (aplTypeRes EQ ARRAY_ERROR)
                goto DOMAIN_EXIT;
        } // End IF/ELSE
    } else
        aplTypeRes = ARRAY_NESTED;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Allocate space for the result
    // N.B. Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = lpMemHdrRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Copy the dimensions from the right arg to the result
    for (uDim = 0; uDim < aplRankRht; uDim++)
        *((LPAPLDIM) lpMemRes)++ = lpMemDimRht[uDim];

    // lpMemRes now points to its data

    if (IsNested (aplTypeRes))
    {
        // Fill nested result with PTR_REUSED
        //   in case we fail part way through
        *((LPAPLNESTED) lpMemRes) = PTR_REUSED;
        for (uRes = 1; uRes < aplNELMRes; uRes++)
            ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;
    } // End IF

    // If this is a fast Boolean operation, ...
    if (bFastBool)
    {
        LPFASTBOOLFCN lpFastBool;           // Ptr to Fast Boolean reduction routine

        // Get a ptr to the appropriate Fast Boolean routine
        lpFastBool = FastBoolFns[lpPrimFlags->Index].lpScan;

        // Call it
        (*lpFastBool) (aplTypeRht,          // Right arg storage type
                       aplNELMRht,          // Right arg NELM
                       lpMemRht,            // Ptr to right arg memory
                       lpMemRes,            // Ptr to result    memory
                       uDimLo,              // Product of dimensions below axis
                       uDimAxRht,           // Length of right arg axis dimension
                       lpPrimFlags->Index,  // FBFN_INDS value (e.g., index into FastBoolFns[])
                       lpYYFcnStrOpr);      // Ptr to operator function strand
    } else
    {
        // Fill in the right arg token
////////tkRhtArg.tkFlags.TknType   =            // To be filled in below
////////tkRhtArg.tkFlags.ImmType   =            // To be filled in below
////////tkRhtArg.tkFlags.NoDisplay = 0;         // Already zero from {0}
////////tkRhtArg.tkData.tkGlbData  =            // To be filled in below
        tkRhtArg.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

        // Fill in the left arg token
////////tkLftArg.tkFlags.TknType   =            // To be filled in below
////////tkLftArg.tkFlags.ImmType   =            // To be filled in below
////////tkLftArg.tkFlags.NoDisplay = 0;         // Already zero from {0}
////////tkLftArg.tkData.tkGlbData  =            // To be filled in below
        tkLftArg.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;
RESTART_EXCEPTION:
        // Loop through the right arg calling the
        //   function strand between data, storing in the
        //   result
        for (uLo = 0; uLo < uDimLo; uLo++)
        for (uHi = 0; uHi < uDimHi; uHi++)
        {
            APLUINT uAx;            // Loop counter for uDimAxRht
            APLINT  iDim;           // Loop counter for aplIntegerLftAbs

            // Calculate the starting index in the right arg of this vector
            uDimRht = uLo * uDimHi * uDimAxRht + uHi;

            // There are uDimAxRht elements in the result to be generated now
            //   each being successive reductions of leading elements in the
            //   vector under consideration.

            // If this function is associative, speed it up
            if (lpPrimFlags
             && ((lpPrimFlags->AssocBool && IsSimpleBool (aplTypeRht))
              || (lpPrimFlags->AssocNumb && IsSimpleNum (aplTypeRht))))
            {
                // Calculate the first index in this vector
                uRht = uDimRht + 0 * uDimHi;

                // Get the first element as the left arg
                GetValueIntoToken (uRht,            // Index to use
                                   lpMemRht,        // Ptr to global memory object to index
                                   aplTypeRht,      // Storage type of the arg
                                   apaOffRht,       // APA offset (if needed)
                                   apaMulRht,       // APA multiplier (if needed)
                                  &tkLftArg);       // Ptr to token in which to place the value
                // In case we blew up, check to see if we must blow up tkLftArg
                if (IsSimpleFlt (aplTypeRes)
                 && IsSimpleInt (aplTypeRht))
                {
                    // Change the immediate type & value
                    tkLftArg.tkFlags.ImmType = IMMTYPE_FLOAT;
                    tkLftArg.tkData.tkFloat  = (APLFLOAT) tkLftArg.tkData.tkInteger;
                } // End IF

                // Split cases based upon the token type of the left arg
                switch (tkLftArg.tkFlags.TknType)
                {
                    case TKT_VARIMMED:
                        // Split cases based upon the result storage type
                        switch (aplTypeRes)
                        {
                            case ARRAY_BOOL:
                                // Check for blow up (to INT or FLOAT -- can this ever happen??)
                                if (IsImmBool (tkLftArg.tkFlags.ImmType))
                                    // Save in the result as a BOOL
                                    ((LPAPLBOOL) lpMemRes)[uRht >> LOG2NBIB] |=
                                      (BIT0 & tkLftArg.tkData.tkBoolean) << (MASKLOG2NBIB & (UINT) uRht);
                                else
                                {
                                    DbgBrk ();      // ***TESTME*** -- Can this ever happen??





                                } // End IF/ELSE

                                break;

                            case ARRAY_INT:
                                // Check for blow up (to FLOAT)
                                if (IsImmInt (tkLftArg.tkFlags.ImmType))
                                    // Save in the result as an INT
                                    ((LPAPLINT) lpMemRes)[uRht] = tkLftArg.tkData.tkInteger;
                                else
                                {
                                    // It's now a FLOAT result
                                    aplTypeRes = ARRAY_FLOAT;

                                    // Tell the header about it
                                    lpMemHdrRes->ArrType = aplTypeRes;

                                    goto RESTART_EXCEPTION;
                                } // End IF/ELSE

                                break;

                            case ARRAY_FLOAT:
                                // Save in the result as a FLOAT
                                ((LPAPLFLOAT) lpMemRes)[uRht] = tkLftArg.tkData.tkFloat;

                                break;

                            case ARRAY_NESTED:
                                // Save in the result as an LPSYMENTRY
                                ((LPAPLNESTED) lpMemRes)[uRht] =
                                lpSymTmp =
                                  MakeSymEntry_EM (tkLftArg.tkFlags.ImmType,    // Immediate type
                                                  &tkLftArg.tkData.tkLongest,   // Ptr to immediate value
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
                        ((LPAPLNESTED) lpMemRes)[uRht] = tkLftArg.tkData.tkGlbData;

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Loop forwards through the elements along the specified axis
                for (uAx = 1; uAx < uDimAxRht; uAx++)
                {
                    // Calculate the index of the next element in this vector
                    uRht = uDimRht + uAx * uDimHi;

                    // Get the next element as the right arg
                    GetValueIntoToken (uRht,        // Index to use
                                       lpMemRht,    // Ptr to global memory object to index
                                       aplTypeRht,  // Storage type of the arg
                                       apaOffRht,   // APA offset (if needed)
                                       apaMulRht,   // APA multiplier (if needed)
                                      &tkRhtArg);   // Ptr to token in which to place the value
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
                        // Copy the result to the left arg token
                        tkLftArg = lpYYRes->tkToken;

                        // Free the YYRes (but not the storage)
                        YYFree (lpYYRes); lpYYRes = NULL;
                    } else
                        goto ERROR_EXIT;

                    // Split cases based upon the token type of the left arg
                    switch (tkLftArg.tkFlags.TknType)
                    {
                        case TKT_VARIMMED:
                            // Split cases based upon the result storage type
                            switch (aplTypeRes)
                            {
                                case ARRAY_BOOL:
                                    // Check for blow up (to INT or FLOAT -- can this ever happen??)
                                    if (IsImmBool (tkLftArg.tkFlags.ImmType))
                                        // Save in the result as a BOOL
                                        ((LPAPLBOOL) lpMemRes)[uRht >> LOG2NBIB] |=
                                          (BIT0 & tkLftArg.tkData.tkBoolean) << (MASKLOG2NBIB & (UINT) uRht);
                                    else
                                    {
                                        DbgBrk ();      // ***TESTME*** -- Can this ever happen??





                                    } // End IF/ELSE

                                    break;

                                case ARRAY_INT:
                                    // Check for blow up (to FLOAT)
                                    if (IsImmInt (tkLftArg.tkFlags.ImmType))
                                        // Save in the result as an INT
                                        ((LPAPLINT) lpMemRes)[uRht] = tkLftArg.tkData.tkInteger;
                                    else
                                    {
                                        // It's now a FLOAT result
                                        aplTypeRes = ARRAY_FLOAT;

                                        // Tell the header about it
                                        lpMemHdrRes->ArrType = aplTypeRes;

                                        goto RESTART_EXCEPTION;
                                    } // End IF/ELSE

                                    break;

                                case ARRAY_FLOAT:
                                    // Save in the result as a FLOAT
                                    ((LPAPLFLOAT) lpMemRes)[uRht] = tkLftArg.tkData.tkFloat;

                                    break;

                                case ARRAY_NESTED:
                                    // Save in the result as an LPSYMENTRY
                                    ((LPAPLNESTED) lpMemRes)[uRht] =
                                    lpSymTmp =
                                      MakeSymEntry_EM (tkLftArg.tkFlags.ImmType,    // Immediate type
                                                      &tkLftArg.tkData.tkLongest,   // Ptr to immediate value
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
                            ((LPAPLNESTED) lpMemRes)[uRht] = tkLftArg.tkData.tkGlbData;

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End FOR
            } else
            // Loop through the ending indices of the reduction, that is,
            //   each reduction starts at uDimRht and goes through the
            //   successive elements up to and including uDimRht + uAx * uDimHi
            //   for uAx running through 0 to uDimAxRht - 1.
            // At this point, the left arg function is non-associative.
            for (uAx = 0; uAx < uDimAxRht; uAx++)
            {
                // Reduce the uDimAxRht values starting at
                //   uRht = uDimRht +   0 * uDimHi;
                // through
                //   uRht = uDimRht + uAx * uDimHi;

                // Calculate the index of last element in this vector
                uRht = uDimRht + uAx * uDimHi;

                // Get the last element as the right arg
                GetValueIntoToken (uRht,            // Index to use
                                   lpMemRht,        // Ptr to global memory object to index
                                   aplTypeRht,      // Storage type of the arg
                                   apaOffRht,       // APA offset (if needed)
                                   apaMulRht,       // APA multiplier (if needed)
                                  &tkRhtArg);       // Ptr to token in which to place the value
                // In case we blew up, check to see if we must blow up tkRhtArg
                if (IsSimpleFlt (aplTypeRes)
                 && IsSimpleInt (aplTypeRht))
                {
                    // Change the immediate type & value
                    tkRhtArg.tkFlags.ImmType = IMMTYPE_FLOAT;
                    tkRhtArg.tkData.tkFloat  = (APLFLOAT) tkRhtArg.tkData.tkInteger;
                } // End IF

                // Loop backwards through the elements along the specified axis
                for (iDim = uAx - 1; iDim >= 0; iDim--)
                {
                    // Calculate the index of the previous element in this vector
                    uRht = uDimRht + iDim * uDimHi;

                    // Get the previous element as the left arg
                    GetValueIntoToken (uRht,        // Index to use
                                       lpMemRht,    // Ptr to global memory object to index
                                       aplTypeRht,  // Storage type of the arg
                                       apaOffRht,   // APA offset (if needed)
                                       apaMulRht,   // APA multiplier (if needed)
                                      &tkLftArg);   // Ptr to token in which to place the value
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

                // Get the index into the result
                uRht = uDimRht + uAx * uDimHi;

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
                                    ((LPAPLBOOL) lpMemRes)[uRht >> LOG2NBIB] |=
                                      (BIT0 & tkRhtArg.tkData.tkBoolean) << (MASKLOG2NBIB & (UINT) uRht);
                                else
                                {
                                    DbgBrk ();      // ***TESTME*** -- Can this ever happen??





                                } // End IF/ELSE

                                break;

                            case ARRAY_INT:
                                // Check for blow up (to FLOAT)
                                if (IsImmInt (tkRhtArg.tkFlags.ImmType))
                                    // Save in the result as an INT
                                    ((LPAPLINT) lpMemRes)[uRht] = tkRhtArg.tkData.tkInteger;
                                else
                                {
                                    // It's now a FLOAT result
                                    aplTypeRes = ARRAY_FLOAT;

                                    // Tell the header about it
                                    lpMemHdrRes->ArrType = aplTypeRes;

                                    goto RESTART_EXCEPTION;
                                } // End IF/ELSE

                                break;

                            case ARRAY_FLOAT:
                                // Save in the result as a FLOAT
                                ((LPAPLFLOAT) lpMemRes)[uRht] = tkRhtArg.tkData.tkFloat;

                                break;

                            case ARRAY_NESTED:
                                // Save in the result as an LPSYMENTRY
                                ((LPAPLNESTED) lpMemRes)[uRht] =
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
                        ((LPAPLNESTED) lpMemRes)[uRht] = tkRhtArg.tkData.tkGlbData;

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            // Because we didn't CopyArray_EM on the tkGlbData,
            //   we don't need to free the right arg token
        } // End FOR/FOR
    } // End IF/ELSE
YYALLOC_EXIT:
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
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

LEFT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

LEFT_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
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
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
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
} // End PrimOpMonSlopeCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydSlope_EM_YY
//
//  Primitive operator for dyadic derived function from Slope ("ERROR")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydSlope_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpDydSlopeCommon_EM_YY (lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
                                       lpYYFcnStrOpr,   // Ptr to operator function strand
                                       lptkRhtArg,      // Ptr to right arg token
                                       FALSE);          // TRUE iff prototyping
} // End PrimOpDydSlope_EM_YY


//***************************************************************************
//  $PrimOpDydSlopeCommon_EM_YY
//
//  Primitive operator for dyadic derived function from Slope ("ERROR")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydSlopeCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     BOOL         bPrototyping)         // TRUE if prototyping

{
    return PrimFnSyntaxError_EM (&lpYYFcnStrOpr->tkToken);
} // End PrimOpDydSlopeCommon_EM_YY


//***************************************************************************
//  End of File: po_slope.c
//***************************************************************************
