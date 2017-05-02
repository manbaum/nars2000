//***************************************************************************
//  NARS2000 -- Primitive Operator -- Combinatorial
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2017 Sudley Place Software

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

#define GlobalLock2(a)      a
#define GlobalUnlock2(a)

// Uncomment out the following line to look for unset values from PNR in PNI
// The timings don't show any advantage of doing this
//#define USE_PNI


//***************************************************************************
//  $PrimOpCombinatorial_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    dyadic operator Combinatorial
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpCombinatorial_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpCombinatorial_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if niladic/monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token (may be NULL if niladic)

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_DOUBLESHRIEK);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return
          PrimOpMonCombinatorial_EM_YY (lpYYFcnStrOpr,  // Ptr to operator function strand
                                        lptkRhtArg,     // Ptr to right arg token (may be NULL if niladic)
                                        FALSE);         // TRUE iff prototyping
    else
        return
          PrimOpDydCombinatorial_EM_YY (lptkLftArg,     // Ptr to left arg token
                                        lpYYFcnStrOpr,  // Ptr to operator function strand
                                        lptkRhtArg,     // Ptr to right arg token
                                        FALSE);         // TRUE iff prototyping
} // End PrimOpCombinatorial_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOpCombinatorial_EM_YY
//
//  Generate a prototype for the derived functions from Combinatorial
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoOpCombinatorial_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoOpCombinatorial_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if niladic/monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token (may be NULL if niladic)
     LPTOKEN      lptkAxisOpr)          // Ptr to axis token always NULL)

{
    if (lptkAxisOpr EQ NULL)
        // Check for axis operator
        lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive
    //   to the axis operator, so signal a syntax error if present
    //***************************************************************
    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called niladically/monadically
        //***************************************************************
        return
          PrimOpMonCombinatorial_EM_YY (lpYYFcnStrOpr, // Ptr to operator function strand
                                         lptkRhtArg,    // Ptr to right arg token (may be NULL if niladic)
                                         TRUE);         // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return
          PrimOpDydCombinatorial_EM_YY (lptkLftArg,    // Ptr to left arg token
                                         lpYYFcnStrOpr, // Ptr to operator function strand
                                         lptkRhtArg,    // Ptr to right arg token
                                         TRUE);         // TRUE iff prototyping
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    return NULL;
} // End PrimProtoOpCombinatorial_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpMonCombinatorial_EM_YY
//
//  Primitive operator for monadic derived function from Combinatorial
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonCombinatorial_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpMonCombinatorial_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,            // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,               // Ptr to right arg token (may be NULL if niladic)
     UBOOL        bPrototyping)             // TRUE iff prototyping

{
    LPPL_YYSTYPE      lpYYFcnStrLft,        // Ptr to left operand function strand
                      lpYYRes = NULL;       // Ptr to the result
    APLSTYPE          aplTypeOpr,           // Storage type of the left operand
                      aplTypeRht;           // ...                 right arg
    APLNELM           aplNELMOpr,           // NELM of the left operand
                      aplNELMRht;           // ...         right arg
    APLRANK           aplRankOpr,           // Rank of the left oeprand
                      aplRankRht;           // ...         right arg
    HGLOBAL           hGlbOpr = NULL,       // Left operand global memory handle
                      hGlbRht = NULL;       // Right arg
    LPVARARRAY_HEADER lpMemHdrOpr = NULL,   // Ptr to left operand header
                      lpMemHdrRht = NULL;   // ...    right arg    ...
    APLLONGEST        aplLongestOpr;        // Left operand immediate data
    LPVOID            lpMemOpr,             // Ptr to left operand data
                      lpMemRht;             // ...    right arg data
    APLINT            aplFS,                // Function Selector
                      aplCvG;               // Count (0) v. Generate (1) flag
    UBOOL             bRet;                 // TRUE iff the result is valid
    LPTOKEN           lptkAxisOpr,          // Ptr to operator axis token (may be NULL)
                      lptkAxisLft;          // Ptr to left operand axis token (may be NULL)
    COMBARGS          combArgs;             // Combinatorial function args
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Zero the combArgs
    ZeroMemory (&combArgs, sizeof (combArgs));

    // Check for axis operator
    lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive to
    //   the axis operator, so signal a syntax error if present
    //***************************************************************

    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = GetMonLftOper (lpYYFcnStrOpr, lptkAxisOpr);

    // If the left operand is a Jot, ...
    if (IsTknFillJot (&lpYYFcnStrLft->tkToken))
        // Clear the caches
        return ClearCombCache_EM_YY (lptkRhtArg,
                                    &lpYYFcnStrOpr->tkToken);
    // Ensure the left operand is a variable
    if (IsTknFcnOpr (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // Check for axis operator in the left operand
    if (lpYYFcnStrLft->TknCount > 1
     && IsTknTypeAxis (lpYYFcnStrLft[1].tkToken.tkFlags.TknType))
        lptkAxisLft = &lpYYFcnStrLft[1].tkToken;
    else
        lptkAxisLft = NULL;

    // The syntax of this operator is
    //   FS!! L R

    // Get the attributes of the left operand
    AttrsOfToken (&lpYYFcnStrLft->tkToken, &aplTypeOpr, &aplNELMOpr, &aplRankOpr, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankOpr))
        goto LEFT_OPERAND_RANK_EXIT;

    // Check for LENGTH ERROR
    if (1 > aplNELMOpr
     ||     aplNELMOpr > 2)
        goto LEFT_OPERAND_LENGTH_EXIT;

    // Check for DOMAIN ERROR
    if (!IsNumeric (aplTypeOpr))
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // Get the global ptrs
    aplLongestOpr = GetGlbPtrs_LOCK (&lpYYFcnStrLft->tkToken, &hGlbOpr, &lpMemHdrOpr);

    // If the token is immediate, ...
    if (hGlbOpr EQ NULL)
        // Point to the data
        lpMemOpr = &aplLongestOpr;
    else
        // Point to the data
        lpMemOpr = VarArrayDataFmBase (lpMemHdrOpr);

    // Get the Function Selector as an integer
    aplFS = ConvertToInteger_SCT (aplTypeOpr, lpMemOpr, 0, &bRet);
    if (!bRet)
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // If the Count v. Generate flag is present, ...
    if (aplNELMOpr EQ 2)
    {
        // Get the CvG flag
        aplCvG = ConvertToInteger_SCT (aplTypeOpr, lpMemOpr, 1, &bRet);

        // The CvG flag must be Boolean
        if (!bRet || !IsBooleanValue (aplCvG))
            goto LEFT_OPERAND_DOMAIN_EXIT;
    } else
        // The default is to Count
        aplCvG = 0;

    // Get the attributes of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // The right arg must be a two-element vector
    if (!IsVector (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for LENGTH ERROR
    if (aplNELMRht NE 2)
        goto RIGHT_LENGTH_EXIT;

    // Check for DOMAIN ERROR
    if (IsPtrArray (aplTypeRht)
     || !IsNumeric (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Lock the memory to get a ptr to it
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Point to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // Get the # Balls as an INT
    combArgs.aplIntBalls = ConvertToInteger_SCT (aplTypeRht, lpMemRht, 0, &bRet);
    if (bRet)
        // Mark as valid
        combArgs.bIntBalls = TRUE;

    // Get the # Boxes as an INT
    combArgs.aplIntBoxes = ConvertToInteger_SCT (aplTypeRht, lpMemRht, 1, &bRet);
    if (bRet)
        // Mark as valid
        combArgs.bIntBoxes = TRUE;

    // Get the # Balls as a RAT
    combArgs.aplRatBalls = ConvertToRAT_SCT (aplTypeRht, lpMemRht, 0, &bRet);
    if (!bRet
     || !mpq_integer_p (&combArgs.aplRatBalls)
     ||  mpq_inf_p     (&combArgs.aplRatBalls))
        goto RIGHT_DOMAIN_EXIT;

    // Get the # Boxes as a RAT
    combArgs.aplRatBoxes = ConvertToRAT_SCT (aplTypeRht, lpMemRht, 1, &bRet);
    if (!bRet
     || !mpq_integer_p (&combArgs.aplRatBoxes)
     ||  mpq_inf_p     (&combArgs.aplRatBoxes))
        goto RIGHT_DOMAIN_EXIT;

    // Check for DOMAIN ERRORs
    if (combArgs.bIntBalls
     && combArgs.bIntBoxes)
    {
        if (combArgs.aplIntBalls < 0
         || combArgs.aplIntBoxes < 0)
            goto RIGHT_DOMAIN_EXIT;
    } else
        if (mpq_cmp_si (&combArgs.aplRatBalls, 0, 1) < 0
         || mpq_cmp_si (&combArgs.aplRatBoxes, 0, 1) < 0)
            goto RIGHT_DOMAIN_EXIT;

    // Set common values
    combArgs.lptkRhtArg   = lptkRhtArg;
    combArgs.lptkFunc     = &lpYYFcnStrOpr->tkToken;
    combArgs.lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    __try
    {
        // Split cases based upon the Function Selector
        switch (aplFS)
        {
            case   0:   // L Pigeons into R holes
                if (aplCvG EQ 0)
                    lpYYRes = FS000C_EM_YY (&combArgs);
                else
                    lpYYRes = FS000G_EM_YY (&combArgs);
                break;

            case   1:   // Partitions of the number L into at most R parts
                if (aplCvG EQ 0)
                    lpYYRes = FS001C_EM_YY (&combArgs);
                else
                    lpYYRes = FS001G_EM_YY (&combArgs);
                break;

            case   2:   // Partitions of the number L into R parts
                if (aplCvG EQ 0)
                    lpYYRes = FS002C_EM_YY (&combArgs);
                else
                    lpYYRes = FS002G_EM_YY (&combArgs);
                break;

            case  10:   // L Combinations of R items
                if (aplCvG EQ 0)
                    lpYYRes = FS010C_EM_YY (&combArgs);
                else
                    lpYYRes = FS010G_EM_YY (&combArgs);
                break;

            case  11:   // L Multisets of R items
                if (aplCvG EQ 0)
                    lpYYRes = FS011C_EM_YY (&combArgs);
                else
                    lpYYRes = FS011G_EM_YY (&combArgs);
                break;

            case  12:   // Compositions of the number L into R parts
                        // a.k.a. Partitions of the number L into R ordered parts
                if (aplCvG EQ 0)
                    lpYYRes = FS012C_EM_YY (&combArgs);
                else
                    lpYYRes = FS012G_EM_YY (&combArgs);
                break;

            case 100:   // L Pigeons into R holes
                if (aplCvG EQ 0)
                    lpYYRes = FS000C_EM_YY (&combArgs);
                else
                    lpYYRes = FS000G_EM_YY (&combArgs);
                break;

            case 101:   // Partitions of the set {{iota}L} into at most R parts
                if (aplCvG EQ 0)
                    lpYYRes = FS101C_EM_YY (&combArgs);
                else
                    lpYYRes = FS101G_EM_YY (&combArgs);
                break;

            case 102:   // Partitions of the set {{iota}L} into R parts
                if (aplCvG EQ 0)
                    lpYYRes = FS102C_EM_YY (&combArgs);
                else
                    lpYYRes = FS102G_EM_YY (&combArgs);
                break;

            case 110:   // L Permutations of R items
                if (aplCvG EQ 0)
                    lpYYRes = FS110C_EM_YY (&combArgs);
                else
                    lpYYRes = FS110G_EM_YY (&combArgs);
                break;

            case 111:   // L Tuples of R items
                if (aplCvG EQ 0)
                    lpYYRes = FS111C_EM_YY (&combArgs);
                else
                    lpYYRes = FS111G_EM_YY (&combArgs);
                break;

            case 112:   // Partitions of the set {{iota}L} into R ordered parts
                if (aplCvG EQ 0)
                    lpYYRes = FS112C_EM_YY (&combArgs);
                else
                    lpYYRes = FS112G_EM_YY (&combArgs);
                break;

            default:
                goto LEFT_OPERAND_DOMAIN_EXIT;
        } // End SWITCH
    } __finally
    {
        // We no longer need this storage
        Myq_clear (&combArgs.aplRatBalls);
        Myq_clear (&combArgs.aplRatBoxes);
    } // End __try/__finally

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;

    // Set the CharIndex
    lpYYRes->tkToken.tkCharIndex = lpYYFcnStrOpr->tkToken.tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken, FALSE);

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    goto ERROR_EXIT;

LEFT_OPERAND_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

LEFT_OPERAND_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

LEFT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (lpYYRes NE NULL)
    {
        // Free the right YYRes
        FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        if (lpMemHdrRht NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
        } // End IF
    } // End IF

    // We no longer need this storage
    Myq_clear (&combArgs.aplRatBalls);
    Myq_clear (&combArgs.aplRatBoxes);

    return lpYYRes;
} // End PrimOpMonCombinatorial_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS000C_EM_YY
//
//  Count L Pigeons into R Holes
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS000C_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS000C_EM_YY
    (LPCOMBARGS lpCombArgs)             // Ptr to Combinatorial args

{
    LPPL_YYSTYPE lpYYRes = NULL;        // Ptr to the result
    UBOOL        bRes;                  // The result
    APLINT       aplIntLft,             // L        as an INT
                 aplIntRht;             // R        ...

    // Get the left & right args as INTs
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBoxes;

    // The result is the Boolean value of L <= R

    // If the INT args are defined, ...
    if (lpCombArgs->bIntBalls
     && lpCombArgs->bIntBoxes)
        // Calculate the result
        bRes = aplIntLft <= aplIntRht;
    else
        // Calculate the result
        bRes = mpq_cmp (&lpCombArgs->aplRatBalls, &lpCombArgs->aplRatBoxes) <= 0;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_BOOL;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkBoolean  = bRes;
////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller

    return lpYYRes;
} // End FS000C_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS000G_EM_YY
//
//  Generate L Pigeons into R Holes
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS000G_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS000G_EM_YY
    (LPCOMBARGS lpCombArgs)             // Ptr to Combinatorial args

{
    LPPL_YYSTYPE lpYYRes = NULL;        // Ptr to the result
    APLDIM       aplDimRes[2];          // The result's dimensions
    HGLOBAL      hGlbRes = NULL;        // The result's global memory handle
    UINT         uRes;                  // # 1s in the trailing byte
    BYTE         uBytes[8]
                 = {0x01,
                    0x03,
                    0x07,
                    0x0F,
                    0x1F,
                    0x3F,
                    0x7F,
                    0xFF};
    APLINT       aplIntLft,             // L        as an INT
                 aplIntRht;             // R        ...

    // Get the left & right args as INTs
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBoxes;

    // The result is ((L<=R) R){rho}R{take}L{rho}1

    // If the # Boxes as an INT is defined, ...
    if (lpCombArgs->bIntBoxes)
        // Set the second dimension
        aplDimRes[1] = aplIntRht;
    else
        goto WSFULL_EXIT;

    // If the # Balls as an INT is defined, ...
    if (lpCombArgs->bIntBalls)
        // Set the first dimension
        aplDimRes[0] = aplIntLft <= aplIntRht;
    else
        // Set the first dimension
        aplDimRes[0] = mpq_cmp (&lpCombArgs->aplRatBalls, &lpCombArgs->aplRatBoxes) <= 0;

    // Allocate a result Boolean matrix of shape <aplDimRes>
    hGlbRes = AllocateGlobalArray (ARRAY_BOOL, aplDimRes[0] * aplDimRes[1], 2, (LPAPLDIM) &aplDimRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpCombArgs->lpbCtrlBreak))
        goto ERROR_EXIT;

    // If the result is non-empty, ...
    if (!IsEmpty (aplDimRes[0]))
    {
        LPVARARRAY_HEADER lpMemHdrRes = NULL;       // Ptr to the result's header
        LPAPLBOOL         lpMemRes;                 // Ptr to the result's global memory

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Fill in the leading full bytes of 1s
        FillMemory (lpMemRes, (size_t) (aplIntLft >> LOG2NBIB), 0xFF);

        // Calculate the # 1s in any trailing byte
        uRes = aplIntLft % NBIB;

        if (uRes NE 0)
            // Fill in the trailing byte
            lpMemRes[aplIntLft >> LOG2NBIB] = uBytes[uRes - 1];

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return lpYYRes;
} // End FS000G_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PNSubInt_RE
//
//  Count Partition Numbers as INTs
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PNSubInt_RE"
#else
#define APPEND_NAME
#endif

APLINT PNSubInt_RE
    (APLINT     aplIntLft,          // Left arg (# Balls)
     APLINT     aplIntRht,          // Right arg (# Boxes)
     LPUBOOL    lpbCtrlBreak)       // Ptr to Ctrl-Break flag

{
    LPAPLNESTED     lpMemComb = NULL;               /// Ptr to temp global memory
    APLUINT         ByteRes;                        // # bytes in the temp result
    LPAPLINT        lpaplInt1 = NULL,               // Ptr to PNI row to be added
                    lpaplInt2 = NULL;               // ...            with the value
    APLINT          aplIntRes;                      // The result
    UBOOL           bCSO,                           // TRUE iff the CSO has been entered
                    bRet;                           // TRUE iff the result is valid
    EXCEPTION_CODES exCode = EXCEPTION_CTRL_BREAK;  // Exception code in case we're to signal an exception

    // Assume we'll be successful
    bRet = TRUE;

    // Check for edge conditions
    if (aplIntLft < aplIntRht
     || aplIntLft <= 0
     || aplIntRht <= 0)
        return (aplIntLft EQ 0)
            && (aplIntRht EQ 0);

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    __try
    {
        __try
        {
            EnterCriticalSection (&CSOCombPNI); bCSO = TRUE;

            // If we need to expand ghGlbPNI, ...
            if (aplIntLft > gMaxLenPNI)
            {
                if (!NestedReAlloc (&ghGlbPNI, aplIntLft, gCurLenPNI, &gMaxLenPNI))
                    goto WSFULL_EXIT;
            } // End IF

            // Lock the memory to get a ptr to it
            lpMemComb = GlobalLock (ghGlbPNI);

            // If we need another row in ghGlbPNI, ...
            while (aplIntLft > gCurLenPNI)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Calclate # bytes in this row
                ByteRes = (gCurLenPNI + 1) * sizeof (APLINT);

                // Allocate an Item row
                lpMemComb[gCurLenPNI] = GlobalAlloc (GPTR, (APLU3264) ByteRes);

                // Check for error
                if (lpMemComb[gCurLenPNI] EQ NULL)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpaplInt1 = GlobalLock2 (lpMemComb[gCurLenPNI]);

                // Fill in the row with a leading and trailing 1 with all -1s in between
                FillMemory (&lpaplInt1[1], (APLU3264) ((gCurLenPNI - 1) * sizeof (APLINT)), 0xFF);

                lpaplInt1[0]          =
                lpaplInt1[gCurLenPNI] = 1;

                // We no longer need this ptr
                GlobalUnlock2 (lpMemComb[gCurLenPNI]); lpaplInt1 = NULL;

                // Count in another row
                gCurLenPNI++;
            } // End WHILE

            // Now calculate the PNI

            // Lock the memory to get a ptr to it
            lpaplInt2 = GlobalLock2 (lpMemComb[aplIntLft - 1]);

            // Get the value
            aplIntRes = lpaplInt2[aplIntRht - 1];

            // We no longer need this ptr
            GlobalUnlock2 (lpMemComb[aplIntLft - 1]); lpaplInt2 = NULL;

            // If it's not valid, ...
            if (aplIntRes EQ -1)
            {
                LeaveCriticalSection (&CSOCombPNI); bCSO = FALSE;

                // Recurse
                aplIntRes = iadd64 (PNSubInt_RE (aplIntLft - aplIntRht, aplIntRht    , lpbCtrlBreak),
                                    PNSubInt_RE (aplIntLft - 1        , aplIntRht - 1, lpbCtrlBreak),
                                   &bRet, 0);
                if (!bRet)
                    goto DOMAIN_EXIT;

                EnterCriticalSection (&CSOCombPNI); bCSO = TRUE;

                // Lock the memory to get a ptr to it
                lpaplInt2 = GlobalLock2 (lpMemComb[aplIntLft - 1]);

                // Save the value
                lpaplInt2[aplIntRht - 1] = aplIntRes;

                // We no longer need this ptr
                GlobalUnlock2 (lpMemComb[aplIntLft - 1]); lpaplInt2 = NULL;
            } // End IF
        } __finally
        {
            if (lpaplInt1 NE NULL)
            {
                // We no longer need this ptr
                GlobalUnlock2 (lpMemComb[gCurLenPNI]); lpaplInt1 = NULL;
            } // End IF

            if (lpaplInt2 NE NULL)
            {
                // We no longer need this ptr
                GlobalUnlock2 (lpMemComb[aplIntLft - 1]); lpaplInt2 = NULL;
            } // End IF

            if (lpMemComb NE NULL)
            {
                // We no longer need this ptr
                GlobalUnlock (ghGlbPNI); lpMemComb = NULL;
            } // End IF

            if (bCSO)
            {
                LeaveCriticalSection (&CSOCombPNI); bCSO = FALSE;
            } // End IF
        } // End __try/__finally
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                goto DOMAIN_EXIT;

            case EXCEPTION_WS_FULL:
                goto WSFULL_EXIT;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    // Mark as a DOMAIN ERROR
    exCode = EXCEPTION_DOMAIN_ERROR;

    goto ERROR_EXIT;

WSFULL_EXIT:
    // Mark as a WS FULL
    exCode = EXCEPTION_WS_FULL;

    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as NOT successful
    bRet = FALSE;
NORMAL_EXIT:
    if (lpaplInt1 NE NULL)
    {
        // We no longer need this ptr
        GlobalUnlock2 (lpMemComb[gCurLenPNI]); lpaplInt1 = NULL;
    } // End IF

    if (lpaplInt2 NE NULL)
    {
        // We no longer need this ptr
        GlobalUnlock2 (lpMemComb[aplIntLft - 1]); lpaplInt2 = NULL;
    } // End IF

    if (lpMemComb NE NULL)
    {
        // We no longer need this ptr
        GlobalUnlock (ghGlbPNI); lpMemComb = NULL;
    } // End IF

    if (bCSO)
    {
        LeaveCriticalSection (&CSOCombPNI); bCSO = FALSE;
    } // End IF

    if (!bRet)
        RaiseException (exCode, 0, 0, NULL);

    return aplIntRes;
} // End PNSubInt_RE
#undef  APPEND_NAME


//***************************************************************************
//  $PNSubRat_RE
//
//  Count Partition Numbers as RATs
//***************************************************************************

APLRAT PNSubRat_RE
    (APLINT     aplIntLft,          // Left arg (# Balls)
     APLINT     aplIntRht,          // Right arg (# Boxes)
     LPUBOOL    lpbCtrlBreak)       // Ptr to Ctrl-Break flag

{
    LPAPLNESTED     lpMemComb = NULL;               // Ptr to PNR vector of vectors of RATs
#ifdef USE_PNI
    LPAPLNESTED     lpMemPNI;                       // Ptr to PNI vector of vectors of INTs
    LPAPLINT        lpaplInt2 = NULL;               // Ptr to INTs
    APLINT          aplIntRes;                      // The result from PNI
#endif
    APLUINT         ByteRes;                        // # bytes in the temp result
    LPAPLRAT        lpaplRat1 = NULL,               // Ptr to RATs to be added
                    lpaplRat2 = NULL;               // ...    RATs with the value
    APLRAT          aplRatRes = {0},                // The result
                    aplRatTmp;                      // Temp as a RAT
    APLINT          i;                              // Loop counter
    UBOOL           bCSO,                           // TRUE iff the CSO has been entered
                    bRet;                           // TRUE iff the result is valid
    EXCEPTION_CODES exCode = EXCEPTION_CTRL_BREAK;  // Exception code in case we're to signal an exception

    // Assume we'll be successful
    bRet = TRUE;

    // Check for edge conditions
    if (aplIntLft < aplIntRht
     || aplIntLft <= 0
     || aplIntRht <= 0)
    {
        mpq_init_set (&aplRatRes, ((aplIntLft EQ 0)
                                && (aplIntRht EQ 0)) ? &mpqOne : &mpqZero);
        return aplRatRes;
    } // End IF

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    __try
    {
        EnterCriticalSection (&CSOCombPNR); bCSO = TRUE;

        // If we need to expand ghGlbPNR, ...
        if (aplIntLft > gMaxLenPNR)
        {
            if (!NestedReAlloc (&ghGlbPNR, aplIntLft, gCurLenPNR, &gMaxLenPNR))
                goto WSFULL_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemComb = GlobalLock (ghGlbPNR);

        // If we need another row in ghGlbPNR, ...
        while (aplIntLft > gCurLenPNR)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (lpbCtrlBreak))
                goto ERROR_EXIT;

            // Calclate # bytes in this row
            ByteRes = (gCurLenPNR + 1) * sizeof (APLRAT);

            // Allocate an Item row
            lpMemComb[gCurLenPNR] = GlobalAlloc (GPTR, (APLU3264) ByteRes);

            // Check for error
            if (lpMemComb[gCurLenPNR] EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpaplRat1 = GlobalLock2 (lpMemComb[gCurLenPNR]);

            // Fill in the row with a leading and trailing 1 with all -1s in between
            for (i = 1; i < gCurLenPNR; i++)
                mpq_init_set_sx (&lpaplRat1[i], -1, 1);

            mpq_init_set (&lpaplRat1[0],          &mpqOne);
            mpq_init_set (&lpaplRat1[gCurLenPNR], &mpqOne);

            // We no longer need this ptr
            GlobalUnlock2 (lpMemComb[gCurLenPNR]); lpaplRat1 = NULL;

            // Count in another row
            gCurLenPNR++;
        } // End WHILE

        // Now calculate the PNR

        // Lock the memory to get a ptr to it
        lpaplRat2 = GlobalLock2 (lpMemComb[aplIntLft - 1]);

        // Get the value
        mpq_init_set (&aplRatRes, &lpaplRat2[aplIntRht - 1]);

        // We no longer need this ptr
        GlobalUnlock2 (lpMemComb[aplIntLft - 1]); lpaplRat2 = NULL;

        // If it's not valid, ...
        if (mpq_cmp_si (&aplRatRes, -1, 1) EQ 0)
        {
#ifdef USE_PNI
            //***************************************************************
            // Look in PNI for this value
            //***************************************************************

            // If the row value is in PNI, ...
            if (aplIntLft <= gCurLenPNI)
            {
                // Lock the memory to get a ptr to it
                lpMemPNI = GlobalLock (ghGlbPNI);

                // Lock the memory to get a ptr to it
                lpaplInt2 = GlobalLock2 (lpMemPNI[aplIntLft - 1]);

                // Get the value
                aplIntRes = lpaplInt2[aplIntRht - 1];

                // We no longer need this ptr
                GlobalUnlock2 (lpMemPNI[aplIntLft - 1]); lpaplInt2 = NULL;

                // We no longer need this ptr
                GlobalUnlock (ghGlbPNI); lpMemPNI = NULL;
            } else
                // Mark as not present in PNI
                aplIntRes = -1;

            // If it's present in PNI, ...
            if (aplIntRes NE -1)
                // Save the value from PNI
                mpq_set_sx (&aplRatRes, aplIntRes, 1);
            else
#endif
            {
                // Prepare to recurse
                LeaveCriticalSection (&CSOCombPNR); bCSO = FALSE;

                Myq_clear (&aplRatRes);

                aplRatRes = PNSubRat_RE (aplIntLft - aplIntRht, aplIntRht    , lpbCtrlBreak);
                aplRatTmp = PNSubRat_RE (aplIntLft - 1        , aplIntRht - 1, lpbCtrlBreak);

                mpq_add (&aplRatRes, &aplRatRes, &aplRatTmp);

                Myq_clear (&aplRatTmp);

                EnterCriticalSection (&CSOCombPNR); bCSO = TRUE;
            } // End IF

            // Lock the memory to get a ptr to it
            lpaplRat2 = GlobalLock2 (lpMemComb[aplIntLft - 1]);

            // Save in the global struc
            mpq_set (&lpaplRat2[aplIntRht - 1], &aplRatRes);

            // We no longer need this ptr
            GlobalUnlock2 (lpMemComb[aplIntLft - 1]); lpaplRat2 = NULL;
        } // End IF
    } __finally
    {
        if (lpaplRat1 NE NULL)
        {
            // We no longer need this ptr
            GlobalUnlock2 (lpMemComb[gCurLenPNR]); lpaplRat1 = NULL;
        } // End IF

        if (lpaplRat2 NE NULL)
        {
            // We no longer need this ptr
            GlobalUnlock2 (lpMemComb[aplIntLft - 1]); lpaplRat2 = NULL;
        } // End IF

#ifdef USE_PNI
        if (lpaplInt2 NE NULL)
        {
            // We no longer need this ptr
            GlobalUnlock2 (lpMemPNI[aplIntLft - 1]); lpaplInt2 = NULL;
        } // End IF
#endif

        if (lpMemComb NE NULL)
        {
            // We no longer need this ptr
            GlobalUnlock (ghGlbPNR); lpMemComb = NULL;
        } // End IF

        if (bCSO)
        {
            LeaveCriticalSection (&CSOCombPNR); bCSO = FALSE;
        } // End IF
    } // End __try/__finally

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    // Mark as a WS FULL
    exCode = EXCEPTION_WS_FULL;

    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as NOT successful
    bRet = FALSE;
NORMAL_EXIT:
    if (lpaplRat1 NE NULL)
    {
        // We no longer need this ptr
        GlobalUnlock2 (lpMemComb[gCurLenPNR]); lpaplRat1 = NULL;
    } // End IF

    if (lpaplRat2 NE NULL)
    {
        // We no longer need this ptr
        GlobalUnlock2 (lpMemComb[aplIntLft - 1]); lpaplRat2 = NULL;
    } // End IF

#ifdef USE_PNI
    if (lpaplInt2 NE NULL)
    {
        // We no longer need this ptr
        GlobalUnlock2 (lpMemPNI[aplIntLft - 1]); lpaplInt2 = NULL;
    } // End IF
#endif

    if (lpMemComb NE NULL)
    {
        // We no longer need this ptr
        GlobalUnlock (ghGlbPNR); lpMemComb = NULL;
    } // End IF

    if (bCSO)
    {
        LeaveCriticalSection (&CSOCombPNR); bCSO = FALSE;
    } // End IF

    if (!bRet)
        RaiseException (exCode, 0, 0, NULL);

    return aplRatRes;
} // End PNSubRat_RE


//***************************************************************************
//  $SN2SubInt_RE
//
//  Count Sterling Numbers of the Second Kind as INTs
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SN2SubInt_RE"
#else
#define APPEND_NAME
#endif

APLINT SN2SubInt_RE
    (APLINT  aplIntLft,             // Left arg (# Balls)
     APLINT  aplIntRht,             // Right arg (# Boxes)
     LPUBOOL lpbCtrlBreak)          // Ptr to Ctrl-Break flag

{
    LPAPLNESTED     lpMemComb = NULL;               // Ptr to SN2I vector of vectors of INTs
    APLUINT         ByteRes;                        // # bytes in the temp result
    LPAPLINT        lpaplInt1 = NULL,               // Ptr to SN2I row to be added
                    lpaplInt2 = NULL;               // ...            with the value
    APLINT          aplIntRes;                      // The result
    UBOOL           bCSO,                           // TRUE iff the CSO has been entered
                    bRet;                           // TRUE iff the result is valid
    EXCEPTION_CODES exCode = EXCEPTION_CTRL_BREAK;  // Exception code in case we're to signal an exception

    // Assume we'll be successful
    bRet = TRUE;

    // Check for edge conditions
    if (aplIntLft < aplIntRht
     || aplIntLft <= 0
     || aplIntRht <= 0)
        return (aplIntLft EQ 0)
            && (aplIntRht EQ 0);

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    __try
    {
        __try
        {
            EnterCriticalSection (&CSOCombSN2I); bCSO = TRUE;

            // If we need to expand ghGlbSN2I, ...
            if (aplIntLft > gMaxLenSN2I)
            {
                if (!NestedReAlloc (&ghGlbSN2I, aplIntLft, gCurLenSN2I, &gMaxLenSN2I))
                    goto WSFULL_EXIT;
            } // End IF

            // Lock the memory to get a ptr to it
            lpMemComb = GlobalLock (ghGlbSN2I);

            // If we need another row in ghGlbSN2I, ...
            while (aplIntLft > gCurLenSN2I)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Calclate # bytes in this row
                ByteRes = (gCurLenSN2I + 1) * sizeof (APLINT);

                // Allocate an Item row
                lpMemComb[gCurLenSN2I] = GlobalAlloc (GPTR, (APLU3264) ByteRes);

                // Check for error
                if (lpMemComb[gCurLenSN2I] EQ NULL)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpaplInt1 = GlobalLock2 (lpMemComb[gCurLenSN2I]);

                // Fill in the row with a leading and trailing 1 with all -1s in between
                FillMemory (&lpaplInt1[1], (APLU3264) ((gCurLenSN2I - 1) * sizeof (APLINT)), 0xFF);

                lpaplInt1[0]          =
                lpaplInt1[gCurLenSN2I] = 1;

                // We no longer need this ptr
                GlobalUnlock2 (lpMemComb[gCurLenSN2I]); lpaplInt1 = NULL;

                // Count in another row
                gCurLenSN2I++;
            } // End WHILE

            // Now calculate the SN2I

            // Lock the memory to get a ptr to it
            lpaplInt2 = GlobalLock2 (lpMemComb[aplIntLft - 1]);

            // Get the value
            aplIntRes = lpaplInt2[aplIntRht - 1];

            // We no longer need this ptr
            GlobalUnlock2 (lpMemComb[aplIntLft - 1]); lpaplInt2 = NULL;

            // If it's not valid, ...
            if (aplIntRes EQ -1)
            {
                UBOOL bRet0,            // TRUE iff the temp result is valid
                      bRet1;            // ...

                LeaveCriticalSection (&CSOCombSN2I); bCSO = FALSE;

                // Recurse
                aplIntRes = iadd64 (imul64 (aplIntRht, SN2SubInt_RE (aplIntLft - 1, aplIntRht    , lpbCtrlBreak), &bRet0, 0),
                                                       SN2SubInt_RE (aplIntLft - 1, aplIntRht - 1, lpbCtrlBreak),
                                   &bRet1, 0);
                if (!bRet0 || !bRet1)
                    goto DOMAIN_EXIT;

                EnterCriticalSection (&CSOCombSN2I); bCSO = TRUE;

                // Lock the memory to get a ptr to it
                lpaplInt2 = GlobalLock2 (lpMemComb[aplIntLft - 1]);

                // Save the value
                lpaplInt2[aplIntRht - 1] = aplIntRes;

                // We no longer need this ptr
                GlobalUnlock2 (lpMemComb[aplIntLft - 1]); lpaplInt2 = NULL;
            } // End IF
        } __finally
        {
            if (lpaplInt1 NE NULL)
            {
                // We no longer need this ptr
                GlobalUnlock2 (lpMemComb[gCurLenSN2I]); lpaplInt1 = NULL;
            } // End IF

            if (lpaplInt2 NE NULL)
            {
                // We no longer need this ptr
                GlobalUnlock2 (lpMemComb[aplIntLft - 1]); lpaplInt2 = NULL;
            } // End IF

            if (lpMemComb NE NULL)
            {
                // We no longer need this ptr
                GlobalUnlock (ghGlbSN2I); lpMemComb = NULL;
            } // End IF

            if (bCSO)
            {
                LeaveCriticalSection (&CSOCombSN2I); bCSO = FALSE;
            } // End IF
        } // End __try/__finally
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                goto DOMAIN_EXIT;

            case EXCEPTION_WS_FULL:
                goto WSFULL_EXIT;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    // Mark as a DOMAIN ERROR
    exCode = EXCEPTION_DOMAIN_ERROR;

    goto ERROR_EXIT;

WSFULL_EXIT:
    // Mark as a WS FULL
    exCode = EXCEPTION_WS_FULL;

    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as NOT successful
    bRet = FALSE;
NORMAL_EXIT:
    if (lpaplInt1 NE NULL)
    {
        // We no longer need this ptr
        GlobalUnlock2 (lpMemComb[gCurLenSN2I]); lpaplInt1 = NULL;
    } // End IF

    if (lpaplInt2 NE NULL)
    {
        // We no longer need this ptr
        GlobalUnlock2 (lpMemComb[aplIntLft - 1]); lpaplInt2 = NULL;
    } // End IF

    if (lpMemComb NE NULL)
    {
        // We no longer need this ptr
        GlobalUnlock (ghGlbSN2I); lpMemComb = NULL;
    } // End IF

    if (bCSO)
    {
        LeaveCriticalSection (&CSOCombSN2I); bCSO = FALSE;
    } // End IF

    if (!bRet)
        RaiseException (exCode, 0, 0, NULL);

    return aplIntRes;
} // End SN2SubInt_RE
#undef  APPEND_NAME


//***************************************************************************
//  $SN2SubRat_RE
//
//  Count Partition Numbers as RATs
//***************************************************************************

APLRAT SN2SubRat_RE
    (APLINT  aplIntLft,             // Left arg (# Balls)
     APLINT  aplIntRht,             // Right arg (# Boxes)
     LPUBOOL lpbCtrlBreak)          // Ptr to Ctrl-Break flag

{
    LPAPLNESTED     lpMemComb = NULL;               // Ptr to SN2R vector of vectors of RATs
#ifdef USE_SN2I
    LPAPLNESTED     lpMemSN2I;                      // Ptr to SN2I vector of vectors of INTs
    LPAPLINT        lpaplInt2 = NULL;               // Ptr to INTs
    APLINT          aplIntRes;                      // The result from SN2I
#endif
    APLUINT         ByteRes;                        // # bytes in the temp result
    LPAPLRAT        lpaplRat1 = NULL,               // Ptr to RATs to be added
                    lpaplRat2 = NULL;               // ...    RATs with the value
    APLRAT          aplRatRes = {0},                // The result
                    aplRatTmp1,                     // Temp as a RAT
                    aplRatTmp2;                     // ...
    APLINT          i;                              // Loop counter
    UBOOL           bCSO,                           // TRUE iff the CSO has been entered
                    bRet;                           // TRUE iff the result is valid
    EXCEPTION_CODES exCode = EXCEPTION_CTRL_BREAK;  // Exception code in case we're to signal an exception

    // Assume we'll be successful
    bRet = TRUE;

    // Check for edge conditions
    if (aplIntLft < aplIntRht
     || aplIntLft <= 0
     || aplIntRht <= 0)
    {
        mpq_init_set (&aplRatRes, ((aplIntLft EQ 0)
                                && (aplIntRht EQ 0)) ? &mpqOne : &mpqZero);
        return aplRatRes;
    } // End IF

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    __try
    {
        EnterCriticalSection (&CSOCombSN2R); bCSO = TRUE;

        // If we need to expand ghGlbSN2R, ...
        if (aplIntLft > gMaxLenSN2R)
        {
            if (!NestedReAlloc (&ghGlbSN2R, aplIntLft, gCurLenSN2R, &gMaxLenSN2R))
                goto WSFULL_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemComb = GlobalLock (ghGlbSN2R);

        // If we need another row in ghGlbSN2R, ...
        while (aplIntLft > gCurLenSN2R)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (lpbCtrlBreak))
                goto ERROR_EXIT;

            // Calclate # bytes in this row
            ByteRes = (gCurLenSN2R + 1) * sizeof (APLRAT);

            // Allocate an Item row
            lpMemComb[gCurLenSN2R] = GlobalAlloc (GPTR, (APLU3264) ByteRes);

            // Check for error
            if (lpMemComb[gCurLenSN2R] EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpaplRat1 = GlobalLock2 (lpMemComb[gCurLenSN2R]);

            // Fill in the row with a leading and trailing 1 with all -1s in between
            for (i = 1; i < gCurLenSN2R; i++)
                mpq_init_set_sx (&lpaplRat1[i], -1, 1);

            mpq_init_set (&lpaplRat1[0],          &mpqOne);
            mpq_init_set (&lpaplRat1[gCurLenSN2R], &mpqOne);

            // We no longer need this ptr
            GlobalUnlock2 (lpMemComb[gCurLenSN2R]); lpaplRat1 = NULL;

            // Count in another row
            gCurLenSN2R++;
        } // End WHILE

        // Now calculate the SN2R

        // Lock the memory to get a ptr to it
        lpaplRat2 = GlobalLock2 (lpMemComb[aplIntLft - 1]);

        // Get the value
        mpq_init_set (&aplRatRes, &lpaplRat2[aplIntRht - 1]);

        // We no longer need this ptr
        GlobalUnlock2 (lpMemComb[aplIntLft - 1]); lpaplRat2 = NULL;

        // If it's not valid, ...
        if (mpq_cmp_si (&aplRatRes, -1, 1) EQ 0)
        {
#ifdef USE_SN2I
            //***************************************************************
            // Look in SN2I for this value
            //***************************************************************

            // If the row value is in SN2I, ...
            if (aplIntLft <= gCurLenSN2I)
            {
                // Lock the memory to get a ptr to it
                lpMemSN2I = GlobalLock (ghGlbSN2I);

                // Lock the memory to get a ptr to it
                lpaplInt2 = GlobalLock2 (lpMemSN2I[aplIntLft - 1]);

                // Get the value
                aplIntRes = lpaplInt2[aplIntRht - 1];

                // We no longer need this ptr
                GlobalUnlock2 (lpMemSN2I[aplIntLft - 1]); lpaplInt2 = NULL;

                // We no longer need this ptr
                GlobalUnlock (ghGlbSN2I); lpMemSN2I = NULL;
            } else
                // Mark as not present in SN2I
                aplIntRes = -1;

            // If it's present in SN2I, ...
            if (aplIntRes NE -1)
                // Save the value from SN2I
                mpq_set_sx (&aplRatRes, aplIntRes, 1);
            else
#endif
            {
                // Prepare to recurse
                LeaveCriticalSection (&CSOCombSN2R); bCSO = FALSE;

                aplRatTmp1 = SN2SubRat_RE (aplIntLft - 1, aplIntRht    , lpbCtrlBreak); // S(n-1, k)

                mpq_init_set_sx (&aplRatTmp2, aplIntRht, 1);                            // k

                mpq_mul (&aplRatRes, &aplRatTmp2, &aplRatTmp1);                         // k * S(n-1, k)

                Myq_clear (&aplRatTmp1);
                Myq_clear (&aplRatTmp2);

                aplRatTmp1 = SN2SubRat_RE (aplIntLft - 1, aplIntRht - 1, lpbCtrlBreak); // S(n-1, k-1)

                mpq_add (&aplRatRes, &aplRatRes, &aplRatTmp1);                          // S(n, k) = k * S(n-1, k) + S(n-1, k-1)

                Myq_clear (&aplRatTmp1);

                EnterCriticalSection (&CSOCombSN2R); bCSO = TRUE;
            } // End IF

            // Lock the memory to get a ptr to it
            lpaplRat2 = GlobalLock2 (lpMemComb[aplIntLft - 1]);

            // Save in the global struc
            mpq_set (&lpaplRat2[aplIntRht - 1], &aplRatRes);

            // We no longer need this ptr
            GlobalUnlock2 (lpMemComb[aplIntLft - 1]); lpaplRat2 = NULL;
        } // End IF
    } __finally
    {
        if (lpaplRat1 NE NULL)
        {
            // We no longer need this ptr
            GlobalUnlock2 (lpMemComb[gCurLenSN2R]); lpaplRat1 = NULL;
        } // End IF

        if (lpaplRat2 NE NULL)
        {
            // We no longer need this ptr
            GlobalUnlock2 (lpMemComb[aplIntLft - 1]); lpaplRat2 = NULL;
        } // End IF

#ifdef USE_SN2I
        if (lpaplInt2 NE NULL)
        {
            // We no longer need this ptr
            GlobalUnlock2 (lpMemSN2I[aplIntLft - 1]); lpaplInt2 = NULL;
        } // End IF
#endif

        if (lpMemComb NE NULL)
        {
            // We no longer need this ptr
            GlobalUnlock (ghGlbSN2R); lpMemComb = NULL;
        } // End IF

        if (bCSO)
        {
            LeaveCriticalSection (&CSOCombSN2R); bCSO = FALSE;
        } // End IF
    } // End __try/__finally

    goto NORMAL_EXIT;

WSFULL_EXIT:
    // Mark as a WS FULL
    exCode = EXCEPTION_WS_FULL;

    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as NOT successful
    bRet = FALSE;
NORMAL_EXIT:
    if (lpaplRat1 NE NULL)
    {
        // We no longer need this ptr
        GlobalUnlock2 (lpMemComb[gCurLenSN2R]); lpaplRat1 = NULL;
    } // End IF

    if (lpaplRat2 NE NULL)
    {
        // We no longer need this ptr
        GlobalUnlock2 (lpMemComb[aplIntLft - 1]); lpaplRat2 = NULL;
    } // End IF

#ifdef USE_SN2I
    if (lpaplInt2 NE NULL)
    {
        // We no longer need this ptr
        GlobalUnlock2 (lpMemSN2I[aplIntLft - 1]); lpaplInt2 = NULL;
    } // End IF
#endif

    if (lpMemComb NE NULL)
    {
        // We no longer need this ptr
        GlobalUnlock (ghGlbSN2R); lpMemComb = NULL;
    } // End IF

    if (bCSO)
    {
        LeaveCriticalSection (&CSOCombSN2R); bCSO = FALSE;
    } // End IF

    if (!bRet)
        RaiseException (exCode, 0, 0, NULL);

    return aplRatRes;
} // End SN2SubRat_RE


//***************************************************************************
//  $FS001C_EM_YY
//
//  Count Partitions of L into at most R Parts
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS001C_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS001C_EM_YY
    (LPCOMBARGS lpCombArgs)             // Ptr to Combinatorial args

{
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to the result
    APLINT        aplIntLft,            // L  as an INT
                  aplIntRht,            // R  ...
                  aplIntRes,            // The result as an integer
                  i;
    APLRAT        aplRatRes = {0},      // Result as a RAT
                  aplRatTmp;            // Temp   ...
    HGLOBAL       hGlbRes = NULL;       // The result's global memory handle
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // The result is the integer value of +/L PN{each} 0..R

    // Calculate the left & right args to L SN2 R as INTs
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBoxes;

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
        {
            UBOOL bRet;                 // TRUE iff the temp result is valid

            // Loop through the right args 0..R
            for (i = aplIntRes = 0; i <= aplIntRht; i++)
            {
                // Call common subroutine for INTs
                aplIntRes = iadd64 (aplIntRes, PNSubInt_RE (aplIntLft, i, lpbCtrlBreak), &bRet, 0);

                if (!bRet)
                    RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
            } // End FOR

        } else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                // Initialize to 0/1
                mpq_init (&aplRatRes);

                __try
                {
                    // Loop through the right args 0..R
                    for (i = 0; i <= lpCombArgs->aplIntBoxes; i++)
                    {
                        // Call common subroutine for RATs
                        aplRatTmp = PNSubRat_RE (aplIntLft, i, lpbCtrlBreak);

                        // Add into the result
                        mpq_add (&aplRatRes, &aplRatRes, &aplRatTmp);

                        // We no longer need this storage
                        Myq_clear (&aplRatTmp);
                    } // End FOR
                } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
                {
                    EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

                    dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

                    // We no longer need this storage
                    Myq_clear (&aplRatRes);

                    // Split cases based upon the exception code
                    switch (exCode)
                    {
                        case EXCEPTION_RESULT_FLOAT:
                        case EXCEPTION_DOMAIN_ERROR:
                            goto DOMAIN_EXIT;

                        case EXCEPTION_WS_FULL:
                            goto WSFULL_EXIT;

                        default:
                            RaiseException (exCode, 0, 0, NULL);

                            break;
                    } // End SWITCH
                } // End __try/__except

                break;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // If the result is a RAT, ...
    if (!IsMpqNULL (&aplRatRes))
    {
        LPVARARRAY_HEADER lpMemHdrRes = NULL;       // Ptr to the result's header
        LPAPLRAT          lpMemRes;                 // Ptr to the result's global memory

        // Allocate a result RAT scalar
        hGlbRes = AllocateGlobalArray (ARRAY_RAT, 1, 0, NULL);
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Copy the data to the result
        lpMemRes[0] = aplRatRes;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkInteger  = aplIntRes;
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } // End IF/ELSE

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lpCombArgs->lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // We no longer need this storage
    Myq_clear (&aplRatRes);
NORMAL_EXIT:
    return lpYYRes;
} // End FS001C_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS001S2
//
//  Allocate an item Integer vector of length <m>
//    and copy the array <a> there
//***************************************************************************

HGLOBAL FS001S2
    (APLINT   m,        // Length of the array to copy
     LPAPLINT a)        // Ptr to the array to copy

{
    HGLOBAL           hGlbMem;          // Item global memory handle
    LPVARARRAY_HEADER lpMemHdr = NULL;  // Ptr to the item header
    LPAPLINT          lpMem;            // Ptr to item data

    // Allocate an item Integer vector of length <m>
    hGlbMem = AllocateGlobalArray (ARRAY_INT, m, 1, &m);
    if (hGlbMem EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdr = MyGlobalLockVar (hGlbMem);

    // Skip over the header and dimensions to the data
    lpMem = VarArrayDataFmBase (lpMemHdr);

    // Copy <m{rho}a> to the output array
    CopyMemory (lpMem, a, (APLU3264) (m * sizeof (lpMem[0])));

    // We no longer need this ptr
    MyGlobalUnlock (hGlbMem); lpMemHdr = NULL;

    // Save in the output array
    return MakePtrTypeGlb (hGlbMem);

WSFULL_EXIT:
    return NULL;
} // End FS001S2


//***************************************************************************
//  $FS001S1
//
//  Generate partitions of L into at most R parts
//
//  This algorithm was taken from Knuth, TAoCP, Vol 4A, p. 392, Algorithm P.
//  The above algorithm was written in origin-0 for n > 0.
//***************************************************************************

void FS001S1
    (APLINT      L,             // L (# Balls)
     APLINT      R,             // R (# Boxes)
     LPAPLINT    a,             // Ptr to temp vector
     APLINT      nLenA,         // Length of Temp vector
     LPAPLNESTED z,             // Ptr to output array
     LPUBOOL     lpbCtrlBreak)  // Ptr to Ctrl-Break flag

{
    APLINT m, n, q, x;          // Temp vars
//P1:
    // Initialize
    a[0] = 0;

    if (L EQ 0
     || R EQ 0)
    {
        // Save <a> as an item in the result
        *z = FS001S2 (1, a);

        return;
    } // End IF

    for (n = 1; n < nLenA; n++)
        a[n] = 1;
    m = 1;
    n = L;
P2:
    a[m] = n;
    q = m-(n EQ 1);
P3:
    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    if (m <= R)
        // Save <1{drop}a> as an item in the result
        //   and skip to the next item in the output array
        *z++ = FS001S2 (m, &a[1]);

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    if (a[q] NE 2)
        goto P5;

    a[q] = 1;
    q--;
    m++;

    goto P3;

P5:
    if (q EQ 0)
        return;
    x = a[q] - 1;
    a[q] = x;
    n = 1 + m - q;
    m = q + 1;
P6:
    if (n <= x)
        goto P2;
    a[m] = x;
    m++;
    n -= x;

    goto P6;

ERROR_EXIT:
    return;
} // End FS001S1


//***************************************************************************
//  $FS001G_EM_YY
//
//  Generate Partitions of L into at most R Parts
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS001G_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS001G_EM_YY
    (LPCOMBARGS lpCombArgs)                 // Ptr to Combinatorial args

{
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    APLINT            aplIntRes;            // The result as an integer
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to the result's header
    LPAPLNESTED       lpMemRes;             // Ptr to the result's global memory
    LPAPLINT          lpMemTmpA = NULL;     // ...    temp vector
    HGLOBAL           hGlbRes = NULL;       // The result's global memory handle
    APLUINT           ByteRes;              // # bytes in the temp result
    APLINT            aplIntLft,            // R        as an INT
                      aplIntRht,            // L        ...
                      i;                    // Loop counter
    APLBOOL           bQuadIO;              // []IO
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // The result is an Integer matrix of shape <(+/L PN{each} 0..R) L>

    // Calculate the left & right args to PN as INTs
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBoxes;

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
        {
            // Loop through the right args 0..R
            for (i = aplIntRes = 0; i <= aplIntRht; i++)
            {
                // Calculate Res += L PN i
                aplIntRes += PNSubInt_RE (aplIntLft, i, lpbCtrlBreak);
            } // End IF
        } else
            goto WSFULL_EXIT;
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__ "FS001G"))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                goto DOMAIN_EXIT;

            case EXCEPTION_WS_FULL:
                goto WSFULL_EXIT;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Allocate a result Nested vector of length <L PN R>
    hGlbRes = AllocateGlobalArray (ARRAY_NESTED, aplIntRes, 1, &aplIntRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

////// If the result is not empty, ... (must set the prototype if empty)
////if (aplIntRes NE 0)
////{
        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Calculate the space needed for a vector of APLINTs of length <L+1>
        ByteRes = (aplIntLft + 1) * sizeof (APLINT);

        // Allocate a temp integer vector of length <L+1>
        lpMemTmpA = DbgGlobalAlloc (GPTR, (APLU3264) ByteRes);
        if (lpMemTmpA EQ NULL)
            goto WSFULL_EXIT;

        __try
        {
            // Call common subroutine
            FS001S1 (aplIntLft,         // # Balls
                     aplIntRht,         // # Boxes
                     lpMemTmpA,         // Ptr to temp vector of length <L+1>
                     aplIntLft + 1,     // Length of temp vector
                     lpMemRes,          // Ptr to the output array
                     lpbCtrlBreak);     // Ptr to Ctrl-Break flag

            // Check for Ctrl-Break
            if (CheckCtrlBreak (lpbCtrlBreak))
                goto ERROR_EXIT;
        } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__ "FS001S1"))
        {
            EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

            dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

            // Split cases based upon the exception code
            switch (exCode)
            {
                case EXCEPTION_WS_FULL:
                    goto WSFULL_EXIT;

                default:
                    RaiseException (exCode, 0, 0, NULL);

                    break;
            } // End SWITCH
        } // End __try/__except
////} // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lpCombArgs->lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        MyGlobalFree (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (lpMemTmpA NE NULL)
    {
        MyGlobalFree (lpMemTmpA); lpMemTmpA = NULL;
    } // End IF

    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF
    } // End IF

    return lpYYRes;
} // End FS001G_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS002C_EM_YY
//
//  Count Partitions of L into R Parts
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS002C_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS002C_EM_YY
    (LPCOMBARGS lpCombArgs)             // Ptr to Combinatorial args

{
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to the result
    APLINT        aplIntRes,            // The result as an integer
                  aplIntLft,            // R        as an INT
                  aplIntRht;            // L        ...
    APLRAT        aplRatRes   = {0};    // Result as a RAT
    HGLOBAL       hGlbRes = NULL;       // The result's global memory handle
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // The result is the integer value of L PN R

    // Calculate the left & right args to PN as INTs
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBoxes;

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
        {
            // Call common subroutine
            aplIntRes = PNSubInt_RE (aplIntLft, aplIntRht, lpbCtrlBreak);
        } else
            goto DOMAIN_EXIT;
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                goto DOMAIN_EXIT;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // If the result is a RAT, ...
    if (!IsMpqNULL (&aplRatRes))
    {
        LPVARARRAY_HEADER lpMemHdrRes = NULL;       // Ptr to the result's header
        LPAPLRAT          lpMemRes;                 // Ptr to the result's global memory

        // Allocate a result RAT scalar
        hGlbRes = AllocateGlobalArray (ARRAY_RAT, 1, 0, NULL);
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Copy the data to the result
        lpMemRes[0] = aplRatRes;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkInteger  = aplIntRes;
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } // End IF/ELSE

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lpCombArgs->lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // We no longer need this storage
    Myq_clear (&aplRatRes);
NORMAL_EXIT:
    return lpYYRes;
} // End FS002C_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS002S
//
//  Generate partitions of L Into R parts
//
//  This algorithm was taken from Knuth, TAoCP, Vol 4A, p. 392, Algorithm H.
//  The above algorithm was written in origin-1 for L >= R >= 2.
//***************************************************************************

void FS002S
    (APLINT   L,                // L (# Balls)
     APLINT   R,                // R (# Boxes)
     LPAPLINT a,                // Ptr to temp vector
     APLINT   nLenA,            // Length of Temp vector
     LPAPLINT m,                // Ptr to output array
     APLINT   nColM,            // Stride of the result
     LPUBOOL  lpbCtrlBreak)     // Ptr to Ctrl-Break flag

{
    APLINT s, j, x;             // Temp vars

#define I1  (1-1)               // Translations from origin-1 to origin-0
#define I2  (2-1)               // ...
#define IR  (R-1)               // ...
#define Ij  (j-1)               // ...

//H1:
    if (R EQ 0
     || L < R)
        return;

    // Initialize a[1] to a[nLenA - 1] with all 1s
    for (j = 1; j < (nLenA - 1); j++)
        a[j] = 1;

    a[I1] = 1 + L - R;
    a[IR + 1] = -1;
H2:
    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Copy result to the output array
    CopyMemory (m, a, (APLU3264) (nColM * sizeof (m[0])));

    // Skip to the next row
    m += nColM;

    if (R EQ 1)
        return;
    if (a[I2] >= (a[I1] - 1))
        goto H4;
    a[I1]--;
    a[I2]++;

    goto H2;

H4:
    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    j = 3;
    s = a[I1] + a[I2] - 1;

    while (a[Ij] >= (a[I1] - 1))
    {
        s += a[Ij];
        j++;
    } // End WHILE

    if (j > R)
        return;

    x = a[Ij] + 1;
    a[Ij] = x;
    j--;

    while (j > 1)
    {
        a[Ij] = x;
        s -= x;
        j--;
    } // End WHILE

    a[I1] = s;

    goto H2;

ERROR_EXIT:
    return;

#undef  Ij
#undef  IR
#undef  I2
#undef  I1
} // End FS002S


//***************************************************************************
//  $FS002G_EM_YY
//
//  Generate Partitions of L into R Parts
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS002G_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS002G_EM_YY
    (LPCOMBARGS lpCombArgs)                 // Ptr to Combinatorial args

{
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    APLINT            aplIntRes;            // The result as an integer
    APLDIM            aplDimRes[2];         // The result's dimensions
    APLNELM           aplNELMRes;           // Result NELM
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to the result's header
    LPAPLINT          lpMemRes,             // Ptr to the result's global memory
                      lpMemTmpA = NULL;     // ...    temp vector
    HGLOBAL           hGlbRes = NULL;       // The result's global memory handle
    APLUINT           ByteRes;              // # bytes in the temp result
    APLINT            aplIntLft,            // R        as an INT
                      aplIntRht;            // L        ...
    APLBOOL           bQuadIO;              // []IO
    UBOOL             bRet;                 // TRUE iff the result is valid
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // The result is an Integer matrix of shape (R*L) L

    // Calculate the left & right args to PN as INTs
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBoxes;

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
        {
            // Call common subroutine
            aplIntRes = PNSubInt_RE (aplIntLft, aplIntRht, lpbCtrlBreak);
        } else
            goto WSFULL_EXIT;
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                goto WSFULL_EXIT;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Set the dimensions
    aplDimRes[0] = aplIntRes;                                       // L PN R
    aplDimRes[1] = aplIntRht;                                       // R
    aplNELMRes   = imul64 (aplDimRes[0], aplDimRes[1], &bRet, 0);

    // Check for overflow
    if (!bRet)
        goto WSFULL_EXIT;

    // Allocate a result Integer matrix of shape <aplDimRes>
    hGlbRes = AllocateGlobalArray (ARRAY_INT, aplNELMRes, 2, (LPAPLDIM) &aplDimRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // If the result is not empty, ...
    if (!IsEmpty (aplNELMRes))
    {
        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Calculate the space needed for a vector of APLINTs of length <R+1>
        ByteRes = (aplIntRht + 1) * sizeof (APLINT);

        // Allocate a temp Integer vector of length <R+1>
        lpMemTmpA = DbgGlobalAlloc (GPTR, (APLU3264) ByteRes);
        if (lpMemTmpA EQ NULL)
            goto WSFULL_EXIT;

        // Call common subroutine
        FS002S (aplIntLft,          // # Balls
                aplIntRht,          // # Boxes
                lpMemTmpA,          // Ptr to temp vector of length <R+1>
                aplIntRht + 1,      // Length of temp vector
                lpMemRes,           // Ptr to the result
                aplIntRht,          // Stride of the result
                lpbCtrlBreak);      // Ptr to Ctrl-Break flag
        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (lpMemTmpA NE NULL)
    {
        MyGlobalFree (lpMemTmpA); lpMemTmpA = NULL;
    } // End IF

    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF
    } // End IF

    return lpYYRes;
} // End FS002G_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS010C_EM_YY
//
//  Count L Combinations of R Items
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS010C_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS010C_EM_YY
    (LPCOMBARGS lpCombArgs)             // Ptr to Combinatorial args

{
    LPPL_YYSTYPE lpYYRes = NULL;        // Ptr to the result
    APLINT       aplIntRes;             // The result as an integer
    APLRAT       aplRatRes   = {0};     // Result as a RAT
    HGLOBAL      hGlbRes = NULL;        // The result's global memory handle

    // The result is the integer value of L!R

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
            PrimFnDydQuoteDotIisIvI (&aplIntRes, 0, (LPALLTYPES) &lpCombArgs->aplIntBalls, (LPALLTYPES) &lpCombArgs->aplIntBoxes, NULL);
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                PrimFnDydQuoteDotRisRvR (&aplRatRes, 0, (LPALLTYPES) &lpCombArgs->aplRatBalls, (LPALLTYPES) &lpCombArgs->aplRatBoxes, NULL);

                break;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // If the result is a RAT, ...
    if (!IsMpqNULL (&aplRatRes))
    {
        LPVARARRAY_HEADER lpMemHdrRes = NULL;       // Ptr to the result's header
        LPAPLRAT          lpMemRes;                 // Ptr to the result's global memory

        // Allocate a result RAT scalar
        hGlbRes = AllocateGlobalArray (ARRAY_RAT, 1, 0, NULL);
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpCombArgs->lpbCtrlBreak))
            goto ERROR_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Copy the data to the result
        lpMemRes[0] = aplRatRes;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkInteger  = aplIntRes;
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } // End IF/ELSE

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // We no longer need this storage
    Myq_clear (&aplRatRes);
NORMAL_EXIT:
    return lpYYRes;
} // End FS010C_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS010S
//
//  Subroutine to FS010G, FS011G, FS012G, and FS110G
//
//  This algorithm was taken from Knuth, TAoCP, Vol 4A, p. 359, Algorithm T.
//  The above algorithm was written in origin-1 for t < n.
//***************************************************************************

void FS010S
    (APLINT    sm,              // Stride for m
     APLINT    t,               // # Balls
     APLINT    n,               // # Boxes
     LPAPLINT  c,               // Ptr to temp
     LPAPLINT  m,               // Ptr to result
     LPUBOOL   lpbCtrlBreak)    // Ptr to Ctrl-Break flag

{
    APLINT i,           // Temp vars
           j,           // ...
           x;           // ...

#define I1  (1-1)
#define I2  (2-1)
#define Ij  (j-1)

    // Initialize the (t + 2) elements of the temp
    for (i = 0; i < t; i++)
        c[i] = i;
    c[t + 0] = n;
    c[t + 1] = 0;
    j = t;

    if (t EQ n)
    {
        // Copy the temp to the result
        CopyMemory (m, c, (APLU3264) (t * sizeof (m[0])));

        goto T7;
    } // End IF
T2:
    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Copy the temp to the result
    CopyMemory (m, c, (APLU3264) (t * sizeof (m[0])));

    // Skip to the next row in the result
    m += sm;

    if (j > 0)
    {
        x = j; goto T6;
    } // End IF

    if (c[I2] > (c[I1] + 1))
    {
        c[I1]++; goto T2;
    } // End IF

    j = 2;
T4:
    c[Ij - 1] = j - 2;
    x = c[Ij] + 1;
    if (x EQ c[Ij + 1])
    {
        j++;
        goto T4;
    } // End IF

    if (j > t)
        goto T7;

T6:
    c[Ij] = x;
    j--;
    goto T2;

T7:
ERROR_EXIT:
        return;
#undef  Ij
#undef  I2
#undef  I1
} // End FS010S


//***************************************************************************
//  $FS010G_EM_YY
//
//  Generate L Combinations of R Items
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS010G_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS010G_EM_YY
    (LPCOMBARGS lpCombArgs)                 // Ptr to Combinatorial args

{
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    APLINT            aplIntRes,            // The result as an integer
                      i;                    // Temp
    APLDIM            aplDimRes[2];         // The result's dimensions
    APLNELM           aplNELMRes;           // Result NELM
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to the result's header
    LPAPLINT          lpMemRes,             // Ptr to the result's global memory
                      lpMemTmp = NULL;      // ...    temp         ...
    HGLOBAL           hGlbRes = NULL;       // The result's global memory handle
    UBOOL             bRet;                 // TRUE iff the result is valid
    APLUINT           ByteTmp;              // # bytes in the Temp
    APLBOOL           bQuadIO;              // []IO
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // The result is an integer matrix of shape (L!R) L

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
            PrimFnDydQuoteDotIisIvI (&aplIntRes, 0, (LPALLTYPES) &lpCombArgs->aplIntBalls, (LPALLTYPES) &lpCombArgs->aplIntBoxes, NULL);
        else
            goto WSFULL_EXIT;
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                goto WSFULL_EXIT;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Set the dimensions
    aplDimRes[0] = aplIntRes;
    aplDimRes[1] = lpCombArgs->aplIntBalls;
    aplNELMRes   = imul64 (aplDimRes[0], aplDimRes[1], &bRet, 0);

    // Check for overflow
    if (!bRet)
        goto WSFULL_EXIT;

    // Allocate a result Integer matrix of shape <aplDimRes>
    hGlbRes = AllocateGlobalArray (ARRAY_INT, aplNELMRes, 2, (LPAPLDIM) &aplDimRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Calculate space needed for a temp Integer vector of length <aplDimRes[1] + 2>
    ByteTmp = imul64 (aplDimRes[1] + 2, sizeof (APLINT), &bRet, 0);
    if (!bRet
     || ByteTmp NE (APLU3264) ByteTmp)
        goto WSFULL_EXIT;

    // Allocate a temp Integer vector of length <aplDimRes[1] + 2>
    lpMemTmp = DbgGlobalAlloc (GPTR, (APLU3264) ByteTmp);
    if (lpMemTmp EQ NULL)
        goto WSFULL_EXIT;

    // If the result is not empty, ...
    if (!IsEmpty (aplNELMRes))
    {
        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

#define u   lpCombArgs->aplIntBalls
#define t   lpCombArgs->aplIntBalls
#define n   lpCombArgs->aplIntBoxes
#define m   lpMemRes
#define c   lpMemTmp
        // Generate the result into <lpMemRes>
        FS010S (u, t, n, c, m, lpbCtrlBreak);
#undef  c
#undef  n
#undef  m
#undef  t
#undef  u
        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // If []IO EQ 1, ...
        if (bQuadIO)
        {
            // Skip over the header and dimensions to the data
            lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

            // Loop through all of the elements in the result
            for (i = 0; i < (APLINT) aplNELMRes; i++)
                // Add in []IO
                lpMemRes[i]++;
        } // End IF

    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (lpMemTmp NE NULL)
    {
        MyGlobalFree (lpMemTmp); lpMemTmp = NULL;
    } // End IF

    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF
    } // End IF

    return lpYYRes;
} // End FS010G_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS011C_EM_YY
//
//  Count L-MultiCombinations of R Items
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS011C_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS011C_EM_YY
    (LPCOMBARGS lpCombArgs)             // Ptr to Combinatorial args

{
    LPPL_YYSTYPE lpYYRes = NULL;        // Ptr to the result
    APLINT       aplIntRes;             // The result as an integer
    APLRAT       aplRatRes   = {0};     // Result as a RAT
    HGLOBAL      hGlbRes = NULL;        // The result's global memory handle
    APLINT       aplIntLft,             // L        as an INT
                 aplIntRht;             // L+R-1    ...
    APLRAT       aplRatLft = {0},       // L        as a RAT
                 aplRatRht = {0};       // L+R-1    ...

    // The result is the integer value of L!L+R-1

    // Calculate the left & right args to L!R as INTs
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBalls + lpCombArgs->aplIntBoxes - 1;

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
            PrimFnDydQuoteDotIisIvI (&aplIntRes, 0, (LPALLTYPES) &aplIntLft, (LPALLTYPES) &aplIntRht, NULL);
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                // Calculate the left & right args to L!R as RATs
                aplRatLft = lpCombArgs->aplRatBalls;
                mpq_init_set_sx (&aplRatRht,  lpCombArgs->aplIntBalls, 1);              // L
                mpq_add         (&aplRatRht, &aplRatRht, &lpCombArgs->aplRatBoxes);     // L+R
                mpq_sub         (&aplRatRht, &aplRatRht, &mpqOne);                      // L+R-1

                __try
                {
                    PrimFnDydQuoteDotRisRvR (&aplRatRes, 0, (LPALLTYPES) &aplRatLft, (LPALLTYPES) &aplRatRht, NULL);
                } __finally
                {
                    Myq_clear (&aplRatRht);
////////////////////Myq_clear (&aplRatLft);
                } // End __try/__finally

                break;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // If the result is a RAT, ...
    if (!IsMpqNULL (&aplRatRes))
    {
        LPVARARRAY_HEADER lpMemHdrRes = NULL;       // Ptr to the result's header
        LPAPLRAT          lpMemRes;                 // Ptr to the result's global memory

        // Allocate a result RAT scalar
        hGlbRes = AllocateGlobalArray (ARRAY_RAT, 1, 0, NULL);
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpCombArgs->lpbCtrlBreak))
            goto ERROR_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Copy the data to the result
        lpMemRes[0] = aplRatRes;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkInteger  = aplIntRes;
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } // End IF/ELSE

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // We no longer need this storage
    Myq_clear (&aplRatRes);
    Myq_clear (&aplRatRht);
NORMAL_EXIT:
    return lpYYRes;
} // End FS011C_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS011G_EM_YY
//
//  Generate L-MultiCombinations of R Items
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS011G_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS011G_EM_YY
    (LPCOMBARGS lpCombArgs)                 // Ptr to Combinatorial args

{
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    APLINT            aplIntRes;            // The result as an integer
    APLRAT            aplRatRes   = {0};    // Result as a RAT
    APLDIM            aplDimRes[2];         // The result's dimensions
    APLNELM           aplNELMRes;           // Result NELM
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to the result's header
    LPAPLINT          lpMemRes,             // Ptr to the result's global memory
                      lpMemTmp = NULL;      // ...    temp         ...
    HGLOBAL           hGlbRes = NULL;       // The result's global memory handle
    APLINT            i,                    // Loop counters
                      j;                    // ...
    APLINT            aplIntLft,            // L        as an INT
                      aplIntRht;            // L+R-1    ...
    APLRAT            aplRatLft = {0},      // L        as a RAT
                      aplRatRht = {0};      // L+R-1    ...
    UBOOL             bRet;                 // TRUE iff the result is valid
    APLUINT           ByteTmp;              // # bytes in the Temp
    APLBOOL           bQuadIO;              // []IO
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // The result is an Integer matrix of shape (L!L+R-1) L

    // Calculate the left & right args to L!R as INTs
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBalls + lpCombArgs->aplIntBoxes - 1;

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
            PrimFnDydQuoteDotIisIvI (&aplIntRes, 0, (LPALLTYPES) &aplIntLft, (LPALLTYPES) &aplIntRht, NULL);
        else
            goto WSFULL_EXIT;
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                goto WSFULL_EXIT;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Set the dimensions
    aplDimRes[0] = aplIntRes;                                       // L!R
    aplDimRes[1] = aplIntLft;                                       // L
    aplNELMRes   = imul64 (aplDimRes[0], aplDimRes[1], &bRet, 0);

    // Check for overflow
    if (!bRet)
        goto WSFULL_EXIT;

    // Allocate a result Integer matrix of shape <aplDimRes>
    hGlbRes = AllocateGlobalArray (ARRAY_INT, aplNELMRes, 2, (LPAPLDIM) &aplDimRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Calculate the space needed for a temp vector of shape <aplDimRes[1] + 2>
    ByteTmp = imul64 (aplDimRes[1] + 2, sizeof (APLINT), &bRet, 0);
    if (!bRet
     || ByteTmp NE (APLU3264) ByteTmp)
        goto WSFULL_EXIT;

    // Allocate a temp Integer vector of length <aplDimRes[1] + 2>
    lpMemTmp = DbgGlobalAlloc (GPTR, (APLU3264) ByteTmp);
    if (lpMemTmp EQ NULL)
        goto WSFULL_EXIT;

    // If the result is not empty, ...
    if (!IsEmpty (aplNELMRes))
    {
        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

#define u   aplIntLft                       // L
#define t   aplIntLft                       // L
#define n   aplIntRht                       // R
#define m   lpMemRes
#define c   lpMemTmp
        // Generate the result into <lpMemRes>
        FS010S (u, t, n, c, m, lpbCtrlBreak);
#undef  c
#undef  n
#undef  m
#undef  t
#undef  u
        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Loop through all of the rows & cols in the result
        for (i = 0; i < (APLINT) aplDimRes[0]; i++)
        for (j = 0; j < (APLINT) aplDimRes[1]; j++)
            // Subtract {iota}L from each row
            lpMemRes[i * aplDimRes[1] + j] -= j - bQuadIO;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (lpMemTmp NE NULL)
    {
        MyGlobalFree (lpMemTmp); lpMemTmp = NULL;
    } // End IF

    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF
    } // End IF

    return lpYYRes;
} // End FS011G_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS012C_EM_YY
//
//  Count Compositions of L into R Parts
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS012C_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS012C_EM_YY
    (LPCOMBARGS lpCombArgs)             // Ptr to Combinatorial args

{
    LPPL_YYSTYPE lpYYRes = NULL;        // Ptr to the result
    APLINT       aplIntRes;             // The result as an integer
    APLRAT       aplRatRes   = {0};     // Result as a RAT
    HGLOBAL      hGlbRes = NULL;        // The result's global memory handle
    APLINT       aplIntLft,             // L        as an INT
                 aplIntRht;             // L+R-1    ...
    APLRAT       aplRatLft = {0},       // L        as a RAT
                 aplRatRht = {0};       // L+R-1    ...

    // The result is the integer value of (R-1)!L-1

    // Calculate the left & right args to L!R as INTs
    aplIntLft = lpCombArgs->aplIntBoxes - 1;
    aplIntRht = lpCombArgs->aplIntBalls - 1;

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
            PrimFnDydQuoteDotIisIvI (&aplIntRes, 0, (LPALLTYPES) &aplIntLft, (LPALLTYPES) &aplIntRht, NULL);
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                // Calculate the left & right args to L!R as RATs
                mpq_init_set_sx (&aplRatLft,  lpCombArgs->aplIntBoxes, 1);              // R
                mpq_sub         (&aplRatLft, &aplRatLft, &mpqOne);                      // R-1
                mpq_init_set_sx (&aplRatRht,  lpCombArgs->aplIntBalls, 1);              // L
                mpq_sub         (&aplRatRht, &aplRatRht, &mpqOne);                      // L-1

                __try
                {
                    PrimFnDydQuoteDotRisRvR (&aplRatRes, 0, (LPALLTYPES) &aplRatLft, (LPALLTYPES) &aplRatRht, NULL);
                } __finally
                {
                    Myq_clear (&aplRatRht);
                    Myq_clear (&aplRatLft);
                } // End __try/__finally

                break;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // If the result is a RAT, ...
    if (!IsMpqNULL (&aplRatRes))
    {
        LPVARARRAY_HEADER lpMemHdrRes = NULL;       // Ptr to the result's header
        LPAPLRAT          lpMemRes;                 // Ptr to the result's global memory

        // Allocate a result RAT scalar
        hGlbRes = AllocateGlobalArray (ARRAY_RAT, 1, 0, NULL);
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpCombArgs->lpbCtrlBreak))
            goto ERROR_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Copy the data to the result
        lpMemRes[0] = aplRatRes;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkInteger  = aplIntRes;
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } // End IF/ELSE

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // We no longer need this storage
    Myq_clear (&aplRatRes);
    Myq_clear (&aplRatRht);
    Myq_clear (&aplRatLft);
NORMAL_EXIT:
    return lpYYRes;
} // End FS012C_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS012G_EM_YY
//
//  Generate Compositions of L into R Parts
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS012G_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS012G_EM_YY
    (LPCOMBARGS lpCombArgs)                 // Ptr to Combinatorial args

{
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    APLINT            aplIntRes;            // The result as an integer
    APLDIM            aplDimRes[2];         // The result's dimensions
    APLNELM           aplNELMRes;           // Result NELM
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to the result's header
    LPAPLINT          lpMemRes,             // Ptr to the result's global memory
                      lpMemTmp = NULL;      // ...    temp         ...
    HGLOBAL           hGlbRes = NULL;       // The result's global memory handle
    APLINT            i,                    // Loop counters
                      j;                    // ...
    APLINT            aplIntLft,            // R-1      as an INT
                      aplIntRht,            // L-1      ...
                      aplIntTmp;            // Temp var
    UBOOL             bRet;                 // TRUE iff the result is valid
    APLUINT           ByteTmp;              // # bytes in the Temp
    APLBOOL           bQuadIO;              // []IO
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // The result is an Integer matrix of shape ((R-1)!L-1) R

    // Calculate the left & right args to L!R as INTs
    aplIntLft = lpCombArgs->aplIntBalls - 1;
    aplIntRht = lpCombArgs->aplIntBoxes - 1;

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
            PrimFnDydQuoteDotIisIvI (&aplIntRes, 0, (LPALLTYPES) &aplIntRht, (LPALLTYPES) &aplIntLft, NULL);
        else
            goto WSFULL_EXIT;
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                goto WSFULL_EXIT;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Restore these values
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBoxes;

    // Set the dimensions
    aplDimRes[0] = aplIntRes;                                       // (R-1)!L-1
    aplDimRes[1] = aplIntRht;                                       // R
    aplNELMRes   = imul64 (aplDimRes[0], aplDimRes[1], &bRet, 0);

    // Check for overflow
    if (!bRet)
        goto WSFULL_EXIT;

    // Allocate a result Integer matrix of shape <aplDimRes>
    hGlbRes = AllocateGlobalArray (ARRAY_INT, aplNELMRes, 2, (LPAPLDIM) &aplDimRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Calculate space needed for a temp Integer vector of length <aplDimRes[1] + 2>
    ByteTmp = imul64 (aplDimRes[1] + 2, sizeof (APLINT), &bRet, 0);
    if (!bRet
     || ByteTmp NE (APLU3264) ByteTmp)
        goto WSFULL_EXIT;

    // Allocate a temp Integer vector of length <aplDimRes[1] + 2>
    lpMemTmp = DbgGlobalAlloc (GPTR, (APLU3264) ByteTmp);
    if (lpMemTmp EQ NULL)
        goto WSFULL_EXIT;

    // If the result is not empty, ...
    if (!IsEmpty (aplNELMRes))
    {
        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

#define u   aplIntRht                       // R
#define t   aplIntRht-1                     // R-1
#define n   aplIntLft-1                     // L-1
#define m   lpMemRes
#define c   lpMemTmp
        // Generate the result into <lpMemRes>
        FS010S (u, t, n, c, m, lpbCtrlBreak);
#undef  c
#undef  n
#undef  m
#undef  t
#undef  u
        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Save value of L-1
        aplIntTmp = aplIntLft - 1;

        // Loop through all of the rows in the result
        for (i = 0; i < (APLINT) aplDimRes[0]; i++)
        {
            APLINT aplLast,
                   aplNext;

            // Save L-1 as the last element in the row
            lpMemRes[(i + 1) * aplDimRes[1] - 1] = aplIntTmp;

            // Save initial "Last" element
            aplLast = lpMemRes[i * aplDimRes[1]]++;

            // Loop through all of the cols in the result
            for (j = 1; j < (APLINT) aplDimRes[1]; j++)     // ***FIXME*** -- Merge these two loops
            {
                // Save as "Next" element
                aplNext = lpMemRes[i * aplDimRes[1] + j];

                // Perform -2 Scan
                lpMemRes[i * aplDimRes[1] + j] -= aplLast;

                // Save as next "Last" element
                aplLast = aplNext;
            } // End FOR
        } // End FOR
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (lpMemTmp NE NULL)
    {
        MyGlobalFree (lpMemTmp); lpMemTmp = NULL;
    } // End IF

    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF
    } // End IF

    return lpYYRes;
} // End FS012G_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS101C_EM_YY
//
//  Count Partitions of the set {{iota}L} into at most R Parts
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS101C_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS101C_EM_YY
    (LPCOMBARGS lpCombArgs)             // Ptr to Combinatorial args

{
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to the result
    APLINT        aplIntLft,            // L  as an INT
                  aplIntRht,            // R  ...
                  aplIntRes,            // The result as an integer
                  i;                    // Loop counter
    APLRAT        aplRatRes = {0},      // Result as a RAT
                  aplRatTmp;            // Temp   ...
    HGLOBAL       hGlbRes = NULL;       // The result's global memory handle
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // The result is the integer value of +/L SN2{each} 0..R

    // Calculate the left & right args to +/L SN2{each} 0..R as INTs
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBoxes;

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
        {
            UBOOL bRet;                 // TRUE iff the temp result is valid

            // Loop through the right args 0..R
            for (i = aplIntRes = 0; i <= aplIntRht; i++)
            {
                // Call common subroutine for INTs
                aplIntRes = iadd64 (aplIntRes, SN2SubInt_RE (aplIntLft, i, lpbCtrlBreak), &bRet, 0);

                if (!bRet)
                    RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
            } // End FOR
        } else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                // Initialize to 0/1
                mpq_init (&aplRatRes);

                __try
                {
                    // Loop through the right args 0..R
                    for (i = 0; i <= lpCombArgs->aplIntBoxes; i++)
                    {
                        // Call common subroutine for RATs
                        aplRatTmp = SN2SubRat_RE (aplIntLft, i, lpbCtrlBreak);

                        // Add into the result
                        mpq_add (&aplRatRes, &aplRatRes, &aplRatTmp);

                        // We no longer need this storage
                        Myq_clear (&aplRatTmp);
                    } // End FOR
                } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
                {
                    EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

                    dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

                    // We no longer need this storage
                    Myq_clear (&aplRatRes);

                    // Split cases based upon the exception code
                    switch (exCode)
                    {
                        case EXCEPTION_RESULT_FLOAT:
                        case EXCEPTION_DOMAIN_ERROR:
                            goto DOMAIN_EXIT;

                        case EXCEPTION_WS_FULL:
                            goto WSFULL_EXIT;

                        default:
                            RaiseException (exCode, 0, 0, NULL);

                            break;
                    } // End SWITCH
                } // End __try/__except

                break;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // If the result is a RAT, ...
    if (!IsMpqNULL (&aplRatRes))
    {
        LPVARARRAY_HEADER lpMemHdrRes = NULL;       // Ptr to the result's header
        LPAPLRAT          lpMemRes;                 // Ptr to the result's global memory

        // Allocate a result RAT scalar
        hGlbRes = AllocateGlobalArray (ARRAY_RAT, 1, 0, NULL);
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Copy the data to the result
        lpMemRes[0] = aplRatRes;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkInteger  = aplIntRes;
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } // End IF/ELSE

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lpCombArgs->lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // We no longer need this storage
    Myq_clear (&aplRatRes);
NORMAL_EXIT:
    return lpYYRes;
} // End FS101C_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS101S1
//
//  Generate partitions of the set {{iota}L} into at most R parts
//
//  This algorithm was taken from Knuth, TAoCP, Vol 4A, p. 416, Algorithm H.
//  The above algorithm was written in origin-1 for n >= 2.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS101S1"
#else
#define APPEND_NAME
#endif

void FS101S1
    (APLINT      n,                 // L
     APLINT      t,                 // R
     LPAPLINT    a,                 // Temp vector of length L
     LPAPLINT    b,                 // ...
     LPAPLNESTED z,                 // Ptr to output array
     APLBOOL     bQuadIO,           // []IO
     LPTOKEN     lptkFunc,          // Ptr to function in case of error
     LPUBOOL     lpbCtrlBreak)      // Ptr to Ctrl-Break flag

{
    APLINT m, j, k;

#define Ij      (j-1)
#define In      (n-1)

//H1:
    // Initialize <a>
    FillMemory (a, (APLU3264) (n *sizeof (a[0])), 0);

    if (n <= 1
     || t EQ 0)
    {
        // Save <a> as an item in the result
        *z = FS102S2 (n < t, a, bQuadIO, lpbCtrlBreak);
        if (*z EQ NULL)
            goto WSFULL_EXIT;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        return;
    } // End IF

    for (j = 0; j < n; j++)
        b[j] = 1;
    m = 1;
H2:
    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // If t is not in a, ...
    for (k = 0; k < n; k++)
    {
        if (t EQ a[k])
            break;
    } // End FOR

    if (k EQ n)
    {
        // Save <a> as an item in the result
        *z = FS102S2 (n, a, bQuadIO, lpbCtrlBreak);
        if (*z EQ NULL)
            goto WSFULL_EXIT;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;
        // Skip to the next item in the output array
        z++;
    } // End IF

    if (a[In] EQ m)
        goto H4;
//H3:
    a[In]++;

    goto H2;

H4:
    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    j = n - 1;
    while (a[Ij] EQ b[Ij])
        j--;
//H5:
    if (j EQ 1)
        return;
    a[Ij]++;
//H6:
    m = b[Ij] + (a[Ij] EQ b[Ij]);
    j++;
    while (j < n)
    {
        a[Ij] = 0;
        b[Ij] = m;
        j++;
    } // End WHILE

    a[In] = 0;

    goto H2;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
ERROR_EXIT:
    return;
#undef  In
#undef  Ij
} // End FS101S1
#undef  APPEND_NAME


//***************************************************************************
//  $FS101G_EM_YY
//
//  Generate Partitions of the set {{iota}L} into at most R Parts
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS101G_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS101G_EM_YY
    (LPCOMBARGS lpCombArgs)                 // Ptr to Combinatorial args

{
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    APLINT            aplIntRes;            // The result as an integer
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to the result's header
    LPAPLNESTED       lpMemRes;             // Ptr to the result's global memory
    LPAPLINT          lpMemTmpA = NULL,     // ...    temp         ...
                      lpMemTmpB = NULL;     // ...
    HGLOBAL           hGlbRes = NULL;       // The result's global memory handle
    APLINT            aplIntLft,            // L        as an INT
                      aplIntLftProto,       // L        as an INT prototype length
                      aplIntRht,            // R        ...
                      i;
    UBOOL             bRet;                 // TRUE iff the result is valid
    APLUINT           ByteTmp;              // # bytes in the Temp
    APLBOOL           bQuadIO;              // []IO
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // The result is a Nested vector of length +/L SN2{each} 0..R

    // Calculate the left & right args to +/L SN2{each} 0..R as INTs
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBoxes;

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
        {
            // Loop through the right args 0..R
            for (i = aplIntRes = 0; i <= aplIntRht; i++)
            {
                // Calculate Res += L SN2 i
                aplIntRes = iadd64 (aplIntRes, SN2SubInt_RE (aplIntLft, i, lpbCtrlBreak), &bRet, 0);

                if (!bRet)
                    goto DOMAIN_EXIT;
            } // End FOR
        } else
            goto WSFULL_EXIT;
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                goto WSFULL_EXIT;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Calculate prototype lengths
    aplIntLftProto = max (aplIntLft, 1);

    // Allocate a result Nested vector of length <+/L SN2{each} 0..R >
    hGlbRes = AllocateGlobalArray (ARRAY_NESTED, aplIntRes, 1, &aplIntRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Calculate space needed for a temp Integer vector of length <max (L, 1)>
    ByteTmp = imul64 (aplIntLftProto, sizeof (APLINT), &bRet, 0);
    if (!bRet
     || ByteTmp NE (APLU3264) ByteTmp)
        goto WSFULL_EXIT;

    // Allocate a temp Integer vector of length <L>
    lpMemTmpA = DbgGlobalAlloc (GPTR, (APLU3264) ByteTmp);
    if (lpMemTmpA EQ NULL)
        goto WSFULL_EXIT;

    // Allocate a temp Integer vector of length <L>
    lpMemTmpB = DbgGlobalAlloc (GPTR, (APLU3264) ByteTmp);
    if (lpMemTmpB EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    Assert (HshTabFrisk (GetMemPTD ()->lphtsPTD));

    // Generate the result into <lpMemRes>
    FS101S1 (aplIntLft,                 // L
             aplIntRht,                 // R
             lpMemTmpA,                 // a    of length <L>
             lpMemTmpB,                 // b    of length <L>
             lpMemRes,                  // z    of length <aplIntRes>
             bQuadIO,                   // []IO
             lpCombArgs->lptkFunc,      // Ptr to function in case of error
             lpbCtrlBreak);             // Ptr to Ctrl-Break flag
    Assert (HshTabFrisk (GetMemPTD ()->lphtsPTD));

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lpCombArgs->lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (lpMemTmpB NE NULL)
    {
        MyGlobalFree (lpMemTmpB); lpMemTmpB = NULL;
    } // End IF

    if (lpMemTmpA NE NULL)
    {
        MyGlobalFree (lpMemTmpA); lpMemTmpA = NULL;
    } // End IF

    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF
    } // End IF

    return lpYYRes;
} // End FS101G_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS102C_EM_YY
//
//  Count Partitions of the set {{iota}L} into R Parts
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS102C_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS102C_EM_YY
    (LPCOMBARGS lpCombArgs)             // Ptr to Combinatorial args

{
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to the result
    APLINT        aplIntLft,            // L  as an INT
                  aplIntRht,            // R  ...
                  aplIntRes;            // The result as an integer
    APLRAT        aplRatRes = {0};      // Result as a RAT
    HGLOBAL       hGlbRes = NULL;       // The result's global memory handle
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // The result is the integer value of L SN2 R

    // Calculate the left & right args to L SN2 R as INTs
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBoxes;

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
            // Call common subroutine for INTs
            aplIntRes = SN2SubInt_RE (aplIntLft, aplIntRht, lpbCtrlBreak);
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                // Call common subroutine for RATs
                aplRatRes = SN2SubRat_RE (aplIntLft, aplIntRht, lpbCtrlBreak);

                break;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // If the result is a RAT, ...
    if (!IsMpqNULL (&aplRatRes))
    {
        LPVARARRAY_HEADER lpMemHdrRes = NULL;       // Ptr to the result's header
        LPAPLRAT          lpMemRes;                 // Ptr to the result's global memory

        // Allocate a result RAT scalar
        hGlbRes = AllocateGlobalArray (ARRAY_RAT, 1, 0, NULL);
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Copy the data to the result
        lpMemRes[0] = aplRatRes;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkInteger  = aplIntRes;
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } // End IF/ELSE

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // We no longer need this storage
    Myq_clear (&aplRatRes);
NORMAL_EXIT:
    return lpYYRes;
} // End FS102C_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS102S2
//
//  Split the argument <a> in RGS format into separate items in the result
//
//  Note that the items created here are all internally of size <L>,
//    but may be of length shorter than <L>.
//***************************************************************************

HGLOBAL FS102S2
    (APLINT      n,                 // L
     LPAPLINT    a,                 // Temp vector of length L
     APLBOOL     bQuadIO,           // []IO
     LPUBOOL     lpbCtrlBreak)      // Ptr to Ctrl-Break flag

{
    HGLOBAL           hGlbRes = NULL,       // Result global memory handle
                      hGlbGrd = NULL,       // Grade temp global memory handle
                      hGlbBlk = NULL,       // Block ...
                      hGlbTmp = NULL;       // Temp       ...
    LPAPLINT          lpMemGrd = NULL,      // Ptr to grade temp global memory
                      lpMemBlk = NULL;      // ...    block ...
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to the result header
    LPAPLNESTED       lpMemRes;             // ...               global memory
    APLINT            i, j,                 // Loop counters
                      iMatch,               // The current Match value
                      iLen = 0;             // The # items in the result

    // Aloocate a Nested vector of length n
    hGlbRes = AllocateGlobalArray (ARRAY_NESTED, n, 1, &n);

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Check for error
    if (hGlbRes NE NULL)
    {
        APLUINT ByteRes;                // # bytes to allocate
        UBOOL   bCont;                  // TRUE iff the FOR loop is to continue

        // Set the PTB bit
        hGlbRes = MakePtrTypeGlb (hGlbRes);

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // If L EQ 0, ...
        if (n EQ 0)
        {
            // Append an empty integer vector to the result
            //   as the prototype
            *lpMemRes++ = hGlbZilde;
        } else
        {
            // Calculate space needed for a temp Integer vector of length <n>
            ByteRes = n * sizeof (APLINT);

            // Allocate a temp Integer vector of length <n>
            //   to hold the grade of this row
            hGlbGrd = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);

            // Check for error
            if (hGlbGrd EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemGrd = MyGlobalLock (hGlbGrd);

            // GradeUp the array <a>
            GradeUp (a,                 // Source
                     lpMemGrd,          // Destination
                     n);                // Count (in units of APLINTs)

            // Check for Ctrl-Break
            if (CheckCtrlBreak (lpbCtrlBreak))
                goto ERROR_EXIT;

            // Allocate a temp Integer vector of length <n>
            //   to hold the block of this row
            hGlbBlk = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);

            // Check for error
            if (hGlbBlk EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemBlk = MyGlobalLock (hGlbBlk);

            // Initialize the Match value
            for (iMatch = 0; iMatch < n; iMatch++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Initialize the continuation flag
                bCont = TRUE;

                // Loop through the elements of <a> in GradeUp order
                //   grouping all elements whose value matches ???
                for (i = j = 0; bCont && (i < n); i++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // If the next element in GradeUp order is a macth, ...
                    switch (signumint (iMatch - a[lpMemGrd[i]]))
                    {
                        case  1:    // iMatch  > a[...] => earlier iMatch; continue processing
                            break;

                        case  0:    // iMatch EQ a[...] => new item in the block
                            lpMemBlk[j++] = lpMemGrd[i] + bQuadIO;

                            break;

                        case -1:    // iMatch  < a[...] => end this block and stop processing
                            // Append an integer vector to the result
                            *lpMemRes++ = FS001S2 (j, lpMemBlk);

                            // Check for Ctrl-Break
                            if (CheckCtrlBreak (lpbCtrlBreak))
                                goto ERROR_EXIT;

                            // Count in another item
                            iLen++;

                            // Stop processing
                            bCont = FALSE;

                            break;
                    } // End SWITCH
                } // End FOR

                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // If we found no entries for this block, ...
                if (j EQ 0)
                    // Quit the outer FOR loop
                    break;
                else
                if (bCont)
                {
                    // Append an integer vector to the result
                    *lpMemRes++ = FS001S2 (j, lpMemBlk);

                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Count in another item
                    iLen++;
                } // End IF/ELSE/...
            } // End FOR
        } // End IF/ELSE
    } // End IF

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Resize the NELM and dimension
    lpMemHdrRes->NELM = iLen;
    *(VarArrayBaseToDim (lpMemHdrRes)) = iLen;

    goto NORMAL_EXIT;

ERROR_EXIT:
    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // Free the result
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbBlk NE NULL)
    {
        if (lpMemBlk NE NULL)
        {
            MyGlobalUnlock (hGlbBlk); lpMemBlk = NULL;
        } // End IF

        MyGlobalFree (hGlbBlk); hGlbBlk = NULL;
    } // End IF

    if (hGlbGrd NE NULL)
    {
        if (lpMemGrd NE NULL)
        {
            MyGlobalUnlock (hGlbGrd); lpMemGrd = NULL;
        } // End IF

        MyGlobalFree (hGlbGrd); hGlbGrd = NULL;
    } // End IF

    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    return hGlbRes;

WSFULL_EXIT:
    return NULL;
} // End FS102S2


//***************************************************************************
//  $FS102S1
//
//  Generate partitions of the set {{iota}L} into R parts
//
//  This algorithm was taken from Knuth, TAoCP, Vol 4A, p. 416, Algorithm H.
//  The above algorithm was written in origin-1 for n >= 2.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS102S1"
#else
#define APPEND_NAME
#endif

void FS102S1
    (APLINT      n,                 // L
     APLINT      t,                 // R
     UBOOL       bResEmpty,         // TRUE iff the result is empty
     LPAPLINT    a,                 // Temp vector of length L
     LPAPLINT    b,                 // ...
     LPAPLNESTED z,                 // Ptr to output array
     APLBOOL     bQuadIO,           // []IO
     LPTOKEN     lptkFunc,          // Ptr to function in case of error
     LPUBOOL     lpbCtrlBreak)      // Ptr to Ctrl-Break flag

{
    APLINT m, j, k, x;

#define Ij      (j-1)
#define In      (n-1)

//H1:
    // Initialize <a> to zero
    FillMemory (a, (APLU3264) (n *sizeof (a[0])), 0);

    if (n <= 1
     || n < t
     || t EQ 0)
    {
        // Save <a> as an item in the result
        *z = FS102S2 (!bResEmpty, a, bQuadIO, lpbCtrlBreak);
        if (*z EQ NULL)
            goto WSFULL_EXIT;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        return;
    } // End IF

    for (j = 0; j < n; j++)
        b[j] = 1;
    m = 1;
H2:
    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // If (t-1) EQ max/a, ...
    for (k = 0, x = -2; k < n; k++)
    {
        if (a[k] > x)
            x = a[k];
    } // End FOR

    if ((t - 1) EQ x)
    {
        // Save <a> as an item in the result
        *z = FS102S2 (n, a, bQuadIO, lpbCtrlBreak);
        if (*z EQ NULL)
            goto WSFULL_EXIT;

        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        z++;    // Skip to the next item in the output array
    } // End IF

    if (a[In] EQ m)
        goto H4;
//H3:
    a[In]++;

    goto H2;

H4:
    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    j = n - 1;
    while (a[Ij] EQ b[Ij])
        j--;
//H5:
    if (j EQ 1)
        return;
    a[Ij]++;
//H6:
    m = b[Ij] + (a[Ij] EQ b[Ij]);
    j++;
    while (j < n)
    {
        a[Ij] = 0;
        b[Ij] = m;
        j++;
    } // End WHILE

    a[In] = 0;

    goto H2;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
ERROR_EXIT:
    return;
#undef  In
#undef  Ij
} // End FS102S1
#undef  APPEND_NAME


//***************************************************************************
//  $FS102G_EM_YY
//
//  Generate Partitions of the set {{iota}L} into R Parts
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS102G_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS102G_EM_YY
    (LPCOMBARGS lpCombArgs)                 // Ptr to Combinatorial args

{
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    APLINT            aplIntRes;            // The result as an integer
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to the result's header
    LPAPLNESTED       lpMemRes;             // Ptr to the result's global memory
    LPAPLINT          lpMemTmpA = NULL,     // ...    temp         ...
                      lpMemTmpB = NULL;     // ...
    HGLOBAL           hGlbRes = NULL;       // The result's global memory handle
    APLINT            aplIntLft,            // L        as an INT
                      aplIntLftProto,       // L        as an INT prototype length
                      aplIntRht;            // R        ...
    UBOOL             bRet;                 // TRUE iff the result is valid
    APLUINT           ByteTmp;              // # bytes in the Temp
    APLBOOL           bQuadIO;              // []IO
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // The result is a Nested vector of length L SN2 R

    // Calculate the left & right args to L SN2 R as INTs
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBoxes;

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
            // Calculate L SN2 R as an INT
            aplIntRes = SN2SubInt_RE (aplIntLft, aplIntRht, lpbCtrlBreak);
        else
            goto WSFULL_EXIT;
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                goto WSFULL_EXIT;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Allocate a result Nested vector of length <L SN2 R>
    hGlbRes = AllocateGlobalArray (ARRAY_NESTED, aplIntRes, 1, &aplIntRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Calculate prototype lengths
    aplIntLftProto = max (aplIntLft, 1);

    // Calculate space needed for a temp Integer vector of length <max (L, 1)>
    ByteTmp = imul64 (aplIntLftProto, sizeof (APLINT), &bRet, 0);
    if (!bRet
     || ByteTmp NE (APLU3264) ByteTmp)
        goto WSFULL_EXIT;

    // Allocate a temp Integer vector of length <max (L, 1)>
    lpMemTmpA = DbgGlobalAlloc (GPTR, (APLU3264) ByteTmp);
    if (lpMemTmpA EQ NULL)
        goto WSFULL_EXIT;

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Allocate a temp Integer vector of length <max (L, 1)>
    lpMemTmpB = DbgGlobalAlloc (GPTR, (APLU3264) ByteTmp);
    if (lpMemTmpB EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    Assert (HshTabFrisk (GetMemPTD ()->lphtsPTD));

    // Generate the result into <lpMemRes>
    FS102S1 (aplIntLft,                 // L
             aplIntRht,                 // R
             aplIntRes EQ 0,            // TRUE iff the result is empty
             lpMemTmpA,                 // a        of length <L>
             lpMemTmpB,                 // b        of length <L>
             lpMemRes,                  // z        of length <L SN2 R>
             bQuadIO,                   // []IO
             lpCombArgs->lptkFunc,      // Ptr to function in case of error
             lpbCtrlBreak);             // Ptr to Ctrl-Break flag
    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    Assert (HshTabFrisk (GetMemPTD ()->lphtsPTD));

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (lpMemTmpB NE NULL)
    {
        MyGlobalFree (lpMemTmpB); lpMemTmpB = NULL;
    } // End IF

    if (lpMemTmpA NE NULL)
    {
        MyGlobalFree (lpMemTmpA); lpMemTmpA = NULL;
    } // End IF

    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF
    } // End IF

    return lpYYRes;
} // End FS102G_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS110C_EM_YY
//
//  Count L Permutations of R Items
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS110C_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS110C_EM_YY
    (LPCOMBARGS lpCombArgs)             // Ptr to Combinatorial args

{
    LPPL_YYSTYPE lpYYRes = NULL;        // Ptr to the result
    APLINT       aplIntRes;             // The result as an integer
    APLRAT       aplRatRes   = {0};     // Result as a RAT
    HGLOBAL      hGlbRes = NULL;        // The result's global memory handle
    APLINT       aplIntLft,             // L        as an INT
                 aplIntRht,             // R        ...
                 aplIntTmp,             // Temp
                 i;                     // Loop counter
    UBOOL        bRet;                  // TRUE iff the result is valid
    APLRAT       aplRatLft = {0},       // L        as a RAT
                 aplRatRht = {0},       // R        ...
                 aplRatTmp = {0};       // Temp     ...

    // The result is the integer value of Rx(R-1)x(R-2)...x(R-(L-1))

    // Calculate the left & right args to the Falling Factorial as INTs
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBoxes;

    // Check for out of range
    if (aplIntLft <= 0)
        // The result is 1
        aplIntRes = 1;
    else
    {
        __try
        {
            // If the INT args are valid, ...
            if (lpCombArgs->bIntBalls
             && lpCombArgs->bIntBoxes)
            {
                // Evaluate the product Rx(R-1)x(R-2)...x(R-(L-1))
                for (i = 0, aplIntRes = 1, aplIntTmp = aplIntRht; i < aplIntLft; i++, aplIntTmp--)
                {
                    aplIntRes = imul64 (aplIntRes, aplIntTmp, &bRet, 0);
                    if (!bRet)
                        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
                } // End FOR
            } else
                RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
        } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
        {
            EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

            dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

            // Split cases based upon the exception code
            switch (exCode)
            {
                case EXCEPTION_RESULT_FLOAT:
                case EXCEPTION_DOMAIN_ERROR:
                    // If the left arg is valid, ...
                    if (lpCombArgs->bIntBalls)
                    {
                        // Calculate the left & right args to the Falling Factorial as RATs
                        mpq_init_set_sx (&aplRatTmp,  lpCombArgs->aplIntBoxes, 1);              // R

                        // Initialize the result to 1/1
                        mpq_init_set (&aplRatRes, &mpqOne);

                        // Evaluate the product Rx(R-1)x(R-2)...x(R-(L-1))
                        for (i = 0; i < aplIntLft; i++)
                        {
                            // Muiltiply by L-i
                            mpq_mul (&aplRatRes, &aplRatRes, &aplRatTmp);

                            // Decrement multiplier
                            mpq_sub (&aplRatTmp, &aplRatTmp, &mpqOne);
                        } // End FOR
                    } else
                        goto DOMAIN_EXIT;
                    break;

                default:
                    RaiseException (exCode, 0, 0, NULL);

                    break;
            } // End SWITCH
        } // End __try/__except
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // If the result is a RAT, ...
    if (!IsMpqNULL (&aplRatRes))
    {
        LPVARARRAY_HEADER lpMemHdrRes = NULL;       // Ptr to the result's header
        LPAPLRAT          lpMemRes;                 // Ptr to the result's global memory

        // Allocate a result RAT scalar
        hGlbRes = AllocateGlobalArray (ARRAY_RAT, 1, 0, NULL);
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        if (CheckCtrlBreak (lpCombArgs->lpbCtrlBreak))
            goto ERROR_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Copy the data to the result
        lpMemRes[0] = aplRatRes;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkInteger  = aplIntRes;
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } // End IF/ELSE

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lpCombArgs->lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // We no longer need this storage
    Myq_clear (&aplRatTmp);
    Myq_clear (&aplRatRes);
    Myq_clear (&aplRatRht);
    Myq_clear (&aplRatLft);
NORMAL_EXIT:
    return lpYYRes;
} // End FS110C_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS110S
//
//  Generate R-permutations of R items
//  This algorithm was taken from Knuth, TAoCP, Vol 4A, p. 322, Algorithm P.
//  The above algorithm was written in origin-1 for n >= 1.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS110S"
#else
#define APPEND_NAME
#endif

void FS110S
    (APLINT    sm,              // Stride for m
     APLINT    n,               // # Balls
     LPAPLINT  a,               // Ptr to temp
     LPAPLINT  c,               // ...
     LPAPLINT  o,               // ...
     LPAPLINT  m,               // Ptr to result
     APLBOOL   bQuadIO,         // []IO
     LPUBOOL   lpbCtrlBreak)    // Ptr to Ctrl-Break flag

{
    APLINT i,           // Loop counter
           j,           // Temp var
           q,           // ...
           s,           // ...
           t;           // ...

#define Ij  (j-1)
    // Check for none
    if (n EQ 0)
        return;
//P1:
    // Zero the temp c
    ZeroMemory (c, (APLU3264) (n * sizeof (c[0])));

    // Fill the temp o with 1s and the temp a with {iota}n (at the outer origin)
    for (i = 0; i < n; i++)
    {
        o[i] = 1;
        a[i] = i + bQuadIO;
    } // End FOR
P2:
    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Copy a to m
    CopyMemory (m, a, (APLU3264) (n * sizeof (m[0])));

    // Skip to the next row in the result
    m += sm;
//P3:
    j = n;
    s = 0;
P4:
    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    q = c[Ij] + o[Ij];
    if (q < 0)
        goto P7;
    if (q EQ j)
        goto P6;
//P5:
    t = a[Ij + s - c[Ij]];
    a[Ij + s - c[Ij]] = a[Ij + s - q];
    a[Ij + s - q] = t;
    c[Ij] = q;

    goto P2;

P6:
    if (j EQ 1)
        return;
    s++;
P7:
    o[Ij] = -o[Ij];
    j--;

    goto P4;

ERROR_EXIT:
    return;
#undef  Ij
} // End FS110S
#undef  APPEND_NAME


//***************************************************************************
//  $FS110G_EM_YY
//
//  Generate L Permutations of R Items
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS110G_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS110G_EM_YY
    (LPCOMBARGS lpCombArgs)                 // Ptr to Combinatorial args

{
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    APLINT            aplIntRes;            // The result as an integer
    APLDIM            aplDimRes[2];         // The result's dimensions
    APLNELM           aplNELMRes;           // Result NELM
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to the result's header
    LPAPLINT          lpMemRes,             // Ptr to the result's global memory
                      lpMemTmpM = NULL,     // ...    temp         ...
                      lpMemTmpA = NULL,     // ...
                      lpMemTmpC = NULL,     // ...
                      lpMemTmpO = NULL,     // ...
                      lpMemTmpT = NULL,     // ...
                      lpMemTmpLp2 = NULL;   // ...
    HGLOBAL           hGlbRes = NULL;       // The result's global memory handle
    APLINT            i,                    // Loop counters
                      iT,                   // ...
                      iM,                   // ...
                      j;                    // ...
    APLINT            aplIntLft,            // R-1      as an INT
                      aplIntRht,            // L-1      ...
                      aplIntFac,            // !L
                      aplIntCmb,            // L!R
                      aplIntTmp;            // Temp var
    UBOOL             bRet;                 // TRUE iff the result is valid
    APLUINT           ByteTmp;              // # bytes in the Temp
    APLBOOL           bQuadIO;              // []IO
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // The result is an Integer matrix of shape (Rx(R-1)x(R-2)...x(R-(L-1))) L

    // Calculate the left & right args to the Falling Factorial as INTs
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBoxes;

    // Check for out of range
    if (aplIntLft <= 0)
        // The result is 1
        aplIntRes = 1;
    else
    // Check for out of range
    if (aplIntLft > aplIntRht)
        // The result is 0
        aplIntRes = 0;
    else
    {
        // Calculate Rx(R-1)x(R-2)...x(R-(L-1))
        // Calculate L!R
        __try
        {
            // If the INT args are valid, ...
            if (lpCombArgs->bIntBalls
             && lpCombArgs->bIntBoxes)
            {
                // Evaluate the product Rx(R-1)x(R-2)...x(R-(L-1))
                for (i = 0, aplIntRes = 1, aplIntTmp = aplIntRht; i < aplIntLft; i++, aplIntTmp--)
                {
                    aplIntRes = imul64 (aplIntRes, aplIntTmp, &bRet, 0);
                    if (!bRet)
                        goto WSFULL_EXIT;
                } // End FOR

                // Calculate !L as an INT
                PrimFnMonQuoteDotIisI   (&aplIntFac, 0, (LPALLTYPES) &aplIntLft, NULL);

                // Calculate L!R as an INT
                PrimFnDydQuoteDotIisIvI (&aplIntCmb, 0, (LPALLTYPES) &aplIntLft, (LPALLTYPES) &aplIntRht, NULL);
            } else
                goto WSFULL_EXIT;
        } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
        {
            EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

            dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

            // Split cases based upon the exception code
            switch (exCode)
            {
                case EXCEPTION_RESULT_FLOAT:
                case EXCEPTION_DOMAIN_ERROR:
                    goto WSFULL_EXIT;

                    break;

                default:
                    RaiseException (exCode, 0, 0, NULL);

                    break;
            } // End SWITCH
        } // End __try/__except
    } // End IF

    // Set the dimensions
    aplDimRes[0] = aplIntRes;                                       // Rx(R-1)x(R-2)...x(R-(L-1))
    aplDimRes[1] = aplIntLft;                                       // L
    aplNELMRes   = imul64 (aplDimRes[0], aplDimRes[1], &bRet, 0);

    // Check for overflow
    if (!bRet)
        goto WSFULL_EXIT;

    // Allocate a result Integer matrix of shape <aplDimRes>
    hGlbRes = AllocateGlobalArray (ARRAY_INT, aplNELMRes, 2, (LPAPLDIM) &aplDimRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    if (CheckCtrlBreak (lpCombArgs->lpbCtrlBreak))
        goto ERROR_EXIT;

    // If the result is not empty, ...
    if (!IsEmpty (aplNELMRes))
    {
        // Calculate space needed for a temp Integer vector of length <L>
        ByteTmp = imul64 (aplIntLft, sizeof (APLINT), &bRet, 0);

        // Check for overflow
        if (!bRet
         || ByteTmp NE (APLU3264) ByteTmp)
            goto WSFULL_EXIT;

        // Allocate a temp Integer vector of length <L>
        lpMemTmpA = DbgGlobalAlloc (GPTR, (APLU3264) ByteTmp);
        if (lpMemTmpA EQ NULL)
            goto WSFULL_EXIT;

        // Allocate a temp Integer vector of length <L>
        lpMemTmpC = DbgGlobalAlloc (GPTR, (APLU3264) ByteTmp);
        if (lpMemTmpC EQ NULL)
            goto WSFULL_EXIT;

        // Allocate a temp Integer vector of length <L>
        lpMemTmpO = DbgGlobalAlloc (GPTR, (APLU3264) ByteTmp);
        if (lpMemTmpO EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // If L=R, ...
        if (aplIntLft EQ aplIntRht)
            // Fill the temp Integer matrix with <perm L> in origin-[]IO
            FS110S (aplIntLft,              // Stride for m
                    aplIntLft,              // # Balls
                    lpMemTmpA,              // Ptr to temp of length <L>
                    lpMemTmpC,              // ...                   <L>
                    lpMemTmpO,              // ...                   <L>
                    lpMemRes,               // m output array of shape <aplIntRes L>
                    bQuadIO,                // []IO
                    lpbCtrlBreak);          // Ptr to Ctrl-Break flag
            // Check for Ctrl-Break
            if (CheckCtrlBreak (lpbCtrlBreak))
                goto ERROR_EXIT;
        else
        {
            // Calculate space needed for a temp Integer matrix of shape <(!L) L>
            ByteTmp = imul64 (aplIntFac, aplIntLft      , &bRet, 0);

            // Check for overflow
            if (!bRet
             || ByteTmp NE (APLU3264) ByteTmp)
                goto WSFULL_EXIT;

            ByteTmp = imul64 (ByteTmp  , sizeof (APLINT), &bRet, 0);

            // Check for overflow
            if (!bRet
             || ByteTmp NE (APLU3264) ByteTmp)
                goto WSFULL_EXIT;

            // Allocate a temp Integer matrix of shape <(!L) L>
            lpMemTmpM = DbgGlobalAlloc (GPTR, (APLU3264) ByteTmp);
            if (lpMemTmpM EQ NULL)
                goto WSFULL_EXIT;

            // Calculate space needed for a temp Integer vector of length <L + 2>
            ByteTmp = imul64 (aplIntLft + 2, sizeof (APLINT), &bRet, 0);

            // Check for overflow
            if (!bRet
             || ByteTmp NE (APLU3264) ByteTmp)
                goto WSFULL_EXIT;

            // Allocate a temp Integer vector of length <L + 2>
            lpMemTmpLp2 = DbgGlobalAlloc (GPTR, (APLU3264) ByteTmp);
            if (lpMemTmpLp2 EQ NULL)
                goto WSFULL_EXIT;

            // Fill the temp Integer matrix with <perm L> in origin-0
            FS110S (aplIntLft,              // Stride for m
                    aplIntLft,              // # Balls
                    lpMemTmpA,              // Ptr to temp of length <L>
                    lpMemTmpC,              // ...                   <L>
                    lpMemTmpO,              // ...                   <L>
                    lpMemTmpM,              // m output array of shape <(!L) L>
                    0,                      // []IO
                    lpbCtrlBreak);          // Ptr to Ctrl-Break flag
            // Check for Ctrl-Break
            if (CheckCtrlBreak (lpbCtrlBreak))
                goto ERROR_EXIT;
            // Fill the result Integer matrix with L comb R
            //   tucked against the top of the result so
            //   we can process it top down filling in from the bottom

            // Calculate the ptr to the last matrix of shape (L!R) L
            //   at the top of lpMemRes
            lpMemTmpT = &lpMemRes[(aplIntRes - aplIntCmb) * aplIntLft];

#define u   lpCombArgs->aplIntBalls     // Stride for m
#define t   lpCombArgs->aplIntBalls     // L
#define n   lpCombArgs->aplIntBoxes     // R
#define c   lpMemTmpLp2                 // Temp var (length L+2)
#define m   lpMemTmpT                   // m output array of shape (L!R) L
        // Generate the <L comb R> result into <lpMemTmpLp2>
        FS010S (u, t, n, c, m, lpbCtrlBreak);
#undef  c
#undef  m
#undef  n
#undef  t
#undef  u
            // Check for Ctrl-Break
            if (CheckCtrlBreak (lpbCtrlBreak))
                goto ERROR_EXIT;

            // Loop through the rows of lpMemTmpT (a.k.a. L comb R) of shape (L!R) L
            for (iT = 0; iT < aplIntCmb; iT++)
            {
                LPAPLINT pT,
                         pM;

                // Copy the next row from the <lpMemTmpT> source to a temp
                //   as we can overwrite it on the very last row
                CopyMemory (lpMemTmpA, &lpMemTmpT[iT * aplIntLft], (APLU3264) (aplIntLft * sizeof (lpMemTmpT[0])));

                // Point to the next row in the <lpMemTmpT> source
                pT = lpMemTmpA;

                // Loop through the rows of lpMemTmpM (a.k.a. perm L) of shape (!L) L
                for (iM = 0; iM < aplIntFac; iM++)
                {
                    // Point to the next row in the <lpMemTmpM> source
                    pM = &lpMemTmpM[iM * aplIntLft];

                    // Loop through the cols of both lpMemTmpT and lpMemTmpM
                    for (j = 0; j < aplIntLft; j++)
                        // Use item <j> from <pM> as the index into <pT>
                        //   and save that value into <lpMemRes>
                        *lpMemRes++ = bQuadIO + pT[*pM++];
                } // End FOR
            } // End FOR
        } // End IF
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (lpMemTmpLp2 NE NULL)
    {
        MyGlobalFree (lpMemTmpLp2); lpMemTmpLp2 = NULL;
    } // End IF

    if (lpMemTmpO NE NULL)
    {
        MyGlobalFree (lpMemTmpO); lpMemTmpO = NULL;
    } // End IF

    if (lpMemTmpC NE NULL)
    {
        MyGlobalFree (lpMemTmpC); lpMemTmpC = NULL;
    } // End IF

    if (lpMemTmpA NE NULL)
    {
        MyGlobalFree (lpMemTmpA); lpMemTmpA = NULL;
    } // End IF

    if (lpMemTmpM NE NULL)
    {
        MyGlobalFree (lpMemTmpM); lpMemTmpM = NULL;
    } // End IF

    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF
    } // End IF
    return lpYYRes;
} // End FS110G_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS111C_EM_YY
//
//  Count L Tuples of R Items
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS111C_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS111C_EM_YY
    (LPCOMBARGS lpCombArgs)             // Ptr to Combinatorial args

{
    LPPL_YYSTYPE lpYYRes = NULL;        // Ptr to the result
    APLINT       aplIntRes;             // The result as an integer
    APLRAT       aplRatRes   = {0};     // Result as a RAT
    HGLOBAL      hGlbRes = NULL;        // The result's global memory handle
    APLINT       aplIntLft,             // L        as an INT
                 aplIntRht;             // L+R-1    ...
    APLRAT       aplRatLft = {0},       // L        as a RAT
                 aplRatRht = {0};       // L+R-1    ...

    // The result is the integer value of R*L

    // Calculate the left & right args to R*L as INTs
    aplIntLft = lpCombArgs->aplIntBoxes;
    aplIntRht = lpCombArgs->aplIntBalls;

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
            // Note that aplIntLft & aplIntRht may be clobbered by this routine
            PrimFnDydStarIisIvI (&aplIntRes, 0, (LPALLTYPES) &aplIntLft, (LPALLTYPES) &aplIntRht, NULL);
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                // Calculate the left & right args to L!R as RATs
                mpq_init_set_sx (&aplRatLft,  lpCombArgs->aplIntBoxes, 1);              // R
                mpq_init_set_sx (&aplRatRht,  lpCombArgs->aplIntBalls, 1);              // L

                __try
                {
                    // Note that aplRatLft & aplRatRht may be clobbered by this routine
                    PrimFnDydStarRisRvR (&aplRatRes, 0, (LPALLTYPES) &aplRatLft, (LPALLTYPES) &aplRatRht, NULL);
                } __finally
                {
                    Myq_clear (&aplRatRht);
                    Myq_clear (&aplRatLft);
                } // End __try/__finally

                break;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // If the result is a RAT, ...
    if (!IsMpqNULL (&aplRatRes))
    {
        LPVARARRAY_HEADER lpMemHdrRes = NULL;       // Ptr to the result's header
        LPAPLRAT          lpMemRes;                 // Ptr to the result's global memory

        // Allocate a result RAT scalar
        hGlbRes = AllocateGlobalArray (ARRAY_RAT, 1, 0, NULL);
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        if (CheckCtrlBreak (lpCombArgs->lpbCtrlBreak))
            goto ERROR_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Copy the data to the result
        lpMemRes[0] = aplRatRes;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkInteger  = aplIntRes;
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } // End IF/ELSE

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // We no longer need this storage
    Myq_clear (&aplRatRes);
    Myq_clear (&aplRatRht);
    Myq_clear (&aplRatLft);
NORMAL_EXIT:
    return lpYYRes;
} // End FS111C_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS111G_EM_YY
//
//  Generate L Tuples of R Items
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS111G_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS111G_EM_YY
    (LPCOMBARGS lpCombArgs)                 // Ptr to Combinatorial args

{
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    APLINT            aplIntRes;            // The result as an integer
    APLDIM            aplDimRes[2];         // The result's dimensions
    APLNELM           aplNELMRes;           // Result NELM
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to the result's header
    LPAPLINT          lpMemRes;             // Ptr to the result's global memory
    HGLOBAL           hGlbRes = NULL;       // The result's global memory handle
    APLINT            aplIntLft,            // R        as an INT
                      aplIntRht;            // L        ...
    APLINT            i,                    // Loop counters
                      j,                    // ...
                      t;                    // Temp var
    UBOOL             bRet;                 // TRUE iff the result is valid
    APLBOOL           bQuadIO;              // []IO

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // The result an Integer matrix is of shape (R*L) L

    // Calculate the left & right args to R*L as INTs
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBoxes;

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
        {
            // Check for indeterminates
            if (aplIntLft <= 0
             && aplIntRht <= 0)
                // Let's say thast 0*0 is 1
                aplIntRes = 1;
            else
                // Note that aplIntLft & aplIntRht may be clobbered by this routine
                PrimFnDydStarIisIvI (&aplIntRes, 0, (LPALLTYPES) &aplIntRht, (LPALLTYPES) &aplIntLft, NULL);
        } else
            goto WSFULL_EXIT;
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                goto WSFULL_EXIT;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Restore these values in case they were clobbered
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBoxes;

    // Set the dimensions
    aplDimRes[0] = aplIntRes;                                       // R*L
    aplDimRes[1] = aplIntLft;                                       // L
    aplNELMRes   = imul64 (aplDimRes[0], aplDimRes[1], &bRet, 0);

    // Check for overflow
    if (!bRet)
        goto WSFULL_EXIT;

    // Allocate a result Integer matrix of shape <aplDimRes>
    hGlbRes = AllocateGlobalArray (ARRAY_INT, aplNELMRes, 2, (LPAPLDIM) &aplDimRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    if (CheckCtrlBreak (lpCombArgs->lpbCtrlBreak))
        goto ERROR_EXIT;

    // If the result is not empty, ...
    if (!IsEmpty (aplNELMRes))
    {
        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Loop through the rows of the result
        for (i = 0; i < aplIntRes; i++)
        // Loop through the cols of the result, backwards
        for (j = aplIntLft - 1, t = i; j >= 0; j--)
        {
            // Calculate the residue
            lpMemRes[i * aplIntLft + j] = (t % aplIntRht) + bQuadIO;

            // Divide out another radix
            t /= aplIntRht;
        } // End FOR
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // We no longer need this storage
NORMAL_EXIT:
    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF
    } // End IF

    return lpYYRes;
} // End FS111G_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS112C_EM_YY
//
//  Count Partitions of the set {{iota}L} into R Ordered Parts
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS112C_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS112C_EM_YY
    (LPCOMBARGS lpCombArgs)             // Ptr to Combinatorial args

{
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to the result
    APLINT        aplIntLft,            // L  as an INT
                  aplIntRht,            // R  ...
                  aplIntRes;            // The result as an integer
    APLRAT        aplRatRes = {0},      // Result as a RAT
                  aplRatTmp;
    UBOOL         bRet;                 // TRUE iff the temp result is valid
    HGLOBAL       hGlbRes = NULL;       // The result's global memory handle
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // The result is the integer value of (!R) * L SN2 R

    // Calculate the left & right args to (!R) * L SN2 R as INTs
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBoxes;

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
        {
            // Calculate !R as an INT
            PrimFnMonQuoteDotIisI (&aplIntRes, 0, (LPALLTYPES) &aplIntRht, NULL);

            // Calculate L SN2 R as an INT
            aplIntRes = imul64 (aplIntRes, SN2SubInt_RE (aplIntLft, aplIntRht, lpbCtrlBreak), &bRet, 0);

            if (!bRet)
                RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
        } else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                // Initialize to 0/1
                mpq_init (&aplRatRes);

                // Initialize a temp to <aplIntRht>
                mpq_init_set_sx (&aplRatTmp, aplIntRht, 1);

                // Calculate !R as a RAT
                PrimFnMonQuoteDotRisR (&aplRatRes, 0, (LPALLTYPES) &aplRatTmp, NULL);

                // We no longer need this storage
                Myq_clear (&aplRatTmp);

                // Calculate L SN2 R as a RAT
                aplRatTmp = SN2SubRat_RE (aplIntLft, aplIntRht, lpbCtrlBreak);

                // Calculate (!R) * L SN2 R
                mpq_mul (&aplRatRes, &aplRatRes, &aplRatTmp);

                // We no longer need this storage
                Myq_clear (&aplRatTmp);

                break;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // If the result is a RAT, ...
    if (!IsMpqNULL (&aplRatRes))
    {
        LPVARARRAY_HEADER lpMemHdrRes = NULL;       // Ptr to the result's header
        LPAPLRAT          lpMemRes;                 // Ptr to the result's global memory

        // Allocate a result RAT scalar
        hGlbRes = AllocateGlobalArray (ARRAY_RAT, 1, 0, NULL);
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        if (CheckCtrlBreak (lpCombArgs->lpbCtrlBreak))
            goto ERROR_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrRes = MyGlobalLockVar (hGlbRes);

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Copy the data to the result
        lpMemRes[0] = aplRatRes;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkInteger  = aplIntRes;
////////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller
    } // End IF/ELSE

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // We no longer need this storage
    Myq_clear (&aplRatRes);
NORMAL_EXIT:
    return lpYYRes;
} // End FS112C_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $FS112G_EM_YY
//
//  Generate Partitions of the set {{iota}L} into R Ordered Parts
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FS112G_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE FS112G_EM_YY
    (LPCOMBARGS lpCombArgs)                 // Ptr to Combinatorial args

{
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to the result's header
    LPAPLNESTED       lpMemRes;             // Ptr to the result's global memory
    LPAPLINT          lpMemTmpA = NULL,     // ...    temp         ...
                      lpMemTmpC = NULL,     // ...
                      lpMemTmpO = NULL,     // ...
                      lpMemTmpM = NULL,     // ...
                      lpMemTmpB = NULL;     // ...
    LPAPLNESTED       lpMemTmpS = NULL;     // ...
    HGLOBAL           hGlbRes = NULL;       // The result's global memory handle
    APLINT            aplIntRes,            // (!R) * L SN2 R
                      aplIntLft,            // L        as an INT
                      aplIntLftProto,       // L        as an INT prototype length
                      aplIntRht,            // R        ...
                      aplIntRhtProto,       // R        ...       prototype length
                      aplIntSN2,            // L SN2 R
                      aplIntSN2Proto,       // L SN2 R            prototype length
                      aplIntFac,            // !R
                      i, j, k;              // Loop counters
    UBOOL             bRet;                 // TRUE iff the result is valid
    APLUINT           ByteTmp;              // # bytes in the Temp
    APLBOOL           bQuadIO;              // []IO
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // The result is a vector of length L SN2 R

    // Calculate the left & right args to L SN2 R as INTs
    aplIntLft = lpCombArgs->aplIntBalls;
    aplIntRht = lpCombArgs->aplIntBoxes;

    __try
    {
        // If the INT args are valid, ...
        if (lpCombArgs->bIntBalls
         && lpCombArgs->bIntBoxes)
        {
            // Calculate !R as an INT
            PrimFnMonQuoteDotIisI (&aplIntFac, 0, (LPALLTYPES) &aplIntRht, NULL);

            // Calculate        L SN2 R as an INT
            aplIntSN2 = SN2SubInt_RE (aplIntLft, aplIntRht, lpbCtrlBreak);

            // Calculate (!R) * L SN2 R as an INT
            aplIntRes = imul64 (aplIntFac, aplIntSN2, &bRet, 0);

            if (!bRet)
                goto WSFULL_EXIT;
        } else
            goto WSFULL_EXIT;
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Split cases based upon the exception code
        switch (exCode)
        {
            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_DOMAIN_ERROR:
                goto WSFULL_EXIT;

            default:
                RaiseException (exCode, 0, 0, NULL);

                break;
        } // End SWITCH
    } // End __try/__except

    // Calculate prototype lengths
    aplIntLftProto = max (aplIntLft, 1);
    aplIntRhtProto = max (aplIntRht, 1);
    aplIntSN2Proto = max (aplIntSN2, 1);

    // Allocate a result Nested vector of length <aplIntRes>
    hGlbRes = AllocateGlobalArray (ARRAY_NESTED, aplIntRes, 1, &aplIntRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    //***************************************************************************
    //  lpMemTmpA is used in both FS110S of length R and FS102S1 of length L
    //  For convenience, we allocate room for the larger of L and R so it can
    //    be used in both places.
    //***************************************************************************

    // Calculate max (max (L, R), 1)
    ByteTmp = max (aplIntLft, aplIntRht);
    ByteTmp = max (ByteTmp, 1);

    // Calculate space needed for a temp Integer vector of length <max (max (L, R), 1)>
    ByteTmp = imul64 (ByteTmp, sizeof (APLINT), &bRet, 0);
    if (!bRet
     || ByteTmp NE (APLU3264) ByteTmp)
        goto WSFULL_EXIT;

    // Allocate a temp Integer vector of length <max (max (L, R), 1)>
    lpMemTmpA = DbgGlobalAlloc (GPTR, (APLU3264) ByteTmp);
    if (lpMemTmpA EQ NULL)
        goto WSFULL_EXIT;

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Calculate space needed for a temp Integer vector of length <max (R, 1)>
    ByteTmp = imul64 (aplIntRhtProto, sizeof (APLINT), &bRet, 0);
    if (!bRet
     || ByteTmp NE (APLU3264) ByteTmp)
        goto WSFULL_EXIT;

    // Allocate a temp Integer vector of length <max (R, 1)>
    lpMemTmpC = DbgGlobalAlloc (GPTR, (APLU3264) ByteTmp);
    if (lpMemTmpC EQ NULL)
        goto WSFULL_EXIT;

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Allocate a temp Integer vector of length <max (R, 1)>
    lpMemTmpO = DbgGlobalAlloc (GPTR, (APLU3264) (ByteTmp));
    if (lpMemTmpO EQ NULL)
        goto WSFULL_EXIT;

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Calculate space needed for a temp Integer matrix of shape <(!R), max (R, 1)>
    ByteTmp = imul64 (aplIntFac, ByteTmp, &bRet, 0);

    // Check for overflow
    if (!bRet
     || ByteTmp NE (APLU3264) ByteTmp)
        goto WSFULL_EXIT;

    // Allocate a temp Integer matrix of shape <(!R), max (R, 1)>
    lpMemTmpM = DbgGlobalAlloc (GPTR, (APLU3264) ByteTmp);
    if (lpMemTmpM EQ NULL)
        goto WSFULL_EXIT;

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Calculate space needed for a temp Integer vector of length <max (L, 1)>
    ByteTmp = imul64 (aplIntLftProto, sizeof (APLINT), &bRet, 0);

    // Check for overflow
    if (!bRet
     || ByteTmp NE (APLU3264) ByteTmp)
        goto WSFULL_EXIT;

    // Allocate a temp Integer vector of length <max (L, 1)>
    lpMemTmpB = DbgGlobalAlloc (GPTR, (APLU3264) ByteTmp);
    if (lpMemTmpB EQ NULL)
        goto WSFULL_EXIT;

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Calculate space needed for a temp Nested vector of length <max (L SN2 R, 1)>
    ByteTmp = imul64 (aplIntSN2Proto, sizeof (APLNESTED), &bRet, 0);
    if (!bRet
     || ByteTmp NE (APLU3264) ByteTmp)
        goto WSFULL_EXIT;

    // Allocate a temp Nested vector of length <L SN2 R>
    lpMemTmpS = DbgGlobalAlloc (GPTR, (APLU3264) ByteTmp);
    if (lpMemTmpS EQ NULL)
        goto WSFULL_EXIT;

    // Fill the temp Integer matrix with <perm R> in origin-0
    FS110S (aplIntRht,              // Stride for m
            aplIntRht,              // # Balls
            lpMemTmpA,              // Ptr to temp of length <max (L, R>>
            lpMemTmpC,              // ...                   <R>
            lpMemTmpO,              // ...                   <R>
            lpMemTmpM,              // m output array of shape <(!R) R>
            0,                      // []IO
            lpbCtrlBreak);          // Ptr to Ctrl-Break flag
    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Fill the temp Nested vector of length <L SN2 R>
    FS102S1 (aplIntLft,                 // L
             aplIntRht,                 // R
             aplIntRes EQ 0,            // TRUE iff the result is empty
             lpMemTmpA,                 // a      of length <max (L, R)>
             lpMemTmpB,                 // b      of length <L>
             lpMemTmpS,                 // z      of length <L SN2 R>
             bQuadIO,                   // []IO
             lpCombArgs->lptkFunc,      // Ptr to function in case of error
             lpbCtrlBreak);             // Ptr to Ctrl-Break flag
    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpbCtrlBreak))
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // If the result is empty, ...
    if (aplIntRes EQ 0)
        // Go through the factorial loop only once
        aplIntFac = 1;

    // Loop through the items of the Nested vector <lpMemTmpS> of length <L SN2 R>
    for (i = 0; i < aplIntSN2Proto; i++)
    {
        LPVARARRAY_HEADER lpMemHdrTmpSi = NULL, // Ptr to temp header
                          lpMemHdrTmp = NULL;   // ...
        LPAPLNESTED       lpMemTmpSi;           // Ptr to temp global memory

        // Lock the memory to get a ptr to it
        lpMemHdrTmpSi = MyGlobalLockVar (lpMemTmpS[i]);

        // Skip over the header and dimensions to the data
        lpMemTmpSi = VarArrayDataFmBase (lpMemHdrTmpSi);    // Of length ???

        // This var points to R items which are to be permuted as per the rows of <lpMemTmpM>

        // Loop through the rows of the Integer matrix <lpMemTmpM> of shape <(!R) R>
        for (j = 0; j < aplIntFac; j++)
        {
            HGLOBAL     hGlbTmp;                    // Temp global memory handle
            LPAPLNESTED lpMemTmp;                   // Ptr to temp global memory

            // Allocate space for an item Nested vector of length <R>
            hGlbTmp = AllocateGlobalArray (ARRAY_NESTED, aplIntRht, 1, &aplIntRht);
            if (hGlbTmp EQ NULL)
                goto WSFULL_EXIT;

            if (CheckCtrlBreak (lpCombArgs->lpbCtrlBreak))
                goto ERROR_EXIT;

            // Lock the memory to get a ptr to it
            lpMemHdrTmp = MyGlobalLockVar (hGlbTmp);

            // Skip over the header and dimensions to the data
            lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

            // Loop through the cols of the Integer matrix <lpMemTmpM> of shape <(!R) R>
            for (k = 0; k < aplIntRhtProto; k++)
            {
                // Save the next (permuted) item
                *lpMemTmp++ = CopySymGlbDir_PTB (lpMemTmpSi[(aplIntSN2 EQ 0) ? 0 : lpMemTmpM[j * aplIntRht + k]]);
            } // End IF/ELSE/FOR

            // We no longer need this ptr
            MyGlobalUnlock (hGlbTmp); lpMemHdrTmp = NULL;

            // Save this item in the result
            *lpMemRes++ = MakePtrTypeGlb (hGlbTmp);
        } // End FOR

        // We no longer need this ptr
        MyGlobalUnlock (lpMemTmpS[i]); lpMemHdrTmpSi = NULL;

        // We no longer need this storage
        FreeResultGlobalVar (lpMemTmpS[i]); lpMemTmpS[i] = NULL;
    } // End FOR

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lpCombArgs->lptkRhtArg);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    CheckMemStat ();

    if (lpMemTmpS NE NULL)
    {
        MyGlobalFree (lpMemTmpS); lpMemTmpS = NULL;
    } // End IF

    if (lpMemTmpB NE NULL)
    {
        MyGlobalFree (lpMemTmpB); lpMemTmpB = NULL;
    } // End IF

    if (lpMemTmpM NE NULL)
    {
        MyGlobalFree (lpMemTmpM); lpMemTmpM = NULL;
    } // End IF

    if (lpMemTmpO NE NULL)
    {
        MyGlobalFree (lpMemTmpO); lpMemTmpO = NULL;
    } // End IF

    if (lpMemTmpC NE NULL)
    {
        MyGlobalFree (lpMemTmpC); lpMemTmpC = NULL;
    } // End IF

    CheckMemStat ();

    if (lpMemTmpA NE NULL)
    {
        MyGlobalFree (lpMemTmpA); lpMemTmpA = NULL;
    } // End IF

    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF
    } // End IF

    return lpYYRes;
} // End FS112G_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydCombinatorial_EM_YY
//
//  Primitive operator for dyadic derived function from Combinatorial (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydCombinatorial_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDydCombinatorial_EM_YY
    (LPTOKEN      lptkLftArg,               // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,            // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,               // Ptr to right arg token (may be NULL if niladic)
     UBOOL        bPrototyping)             // TRUE iff prototyping

{
    return
      PrimFnValenceError_EM (&lpYYFcnStrOpr->tkToken APPEND_NAME_ARG);
} // End PrimOpDydCombinatorial_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $NestedReAlloc
//
//  Expand a nested array by GlobalReAlloc
//***************************************************************************

UBOOL NestedReAlloc
    (HGLOBAL *lphGlb,           // Ptr to HGLOBAL to ReAlloc
     APLINT   aplIntLft,        // New max length
     APLINT   CurLen,           // Current length
     LPAPLINT lpMaxLen)         // Ptr to max length

{
    APLUINT     ByteRes;        // # bytes in the temp result
    HGLOBAL     hGlbTmp;
    LPAPLNESTED lpMemComb = NULL;
    LPAPLNESTED lpMemTmp;

    // Calculate # bytes needed
    ByteRes = aplIntLft * sizeof (APLNESTED);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Attempt to reallocate the original HGLOBAL
    hGlbTmp = GlobalReAlloc (*lphGlb, (APLU3264) ByteRes, GHND);

    // Check for error
    if (hGlbTmp EQ NULL)
        goto WSFULL_EXIT;
    else
    if (*lphGlb NE hGlbTmp)
    {
        // Lock the two global memory handles to get a ptr to them
        lpMemComb = GlobalLock (*lphGlb);
        lpMemTmp  = GlobalLock (hGlbTmp );

        // Copy the old memory to the new
        CopyMemory (lpMemTmp, lpMemComb, (APLU3264) (CurLen * sizeof (APLNESTED)));

        // We no longer need these ptrs
        GlobalUnlock (*lphGlb); lpMemComb = NULL;
        GlobalUnlock (hGlbTmp); lpMemTmp  = NULL;

        // Free the old handle
        GlobalFree (*lphGlb); *lphGlb = NULL;

        // Save as the new handle
        *lphGlb = hGlbTmp;
    } // End IF/ELSE

    // Save as new maximum
    *lpMaxLen = aplIntLft;

    return TRUE;

WSFULL_EXIT:
    return FALSE;
} // End NestedReAlloc


//***************************************************************************
//  $ClearCombCache_EM_YY
//
//  Clear the Combinatorial caches
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ClearCombCache_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ClearCombCache_EM_YY
    (LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkFunc)                      // Ptr to function token

{
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    APLSTYPE          aplTypeRht;           // Storage type of the right arg
    APLNELM           aplNELMRht,           // NELM of the right arg
                      aplNELMRes;           // ...         result
    APLRANK           aplRankRht;           // Rank of the right arg
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL;       // Result's  ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg header
                      lpMemHdrRes = NULL;   // ...    result    ...
    APLLONGEST        aplLongestRht;        // Right arg immediate data
    LPVOID            lpMemRht;             // Ptr to right arg data
    LPAPLCHAR         lpMemRes;             // Ptr to the result's global memory
    UBOOL             bRet;                 // TRUE iff the result is valid

    // Ensure that the right arg is a scalar or one-element vector with value 1

    // Get the attributes of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // The right arg must be a scalar or vector
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for LENGTH ERROR
    if (aplNELMRht NE 1)
        goto RIGHT_LENGTH_EXIT;

    // Check for DOMAIN ERROR
    if (!IsNumeric (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Lock the memory to get a ptr to it
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the data is global, ...
    if (hGlbRht NE NULL)
    {
        // Point to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

        // Convert the arg to an integer
        aplLongestRht = ConvertToInteger_SCT (aplTypeRht, lpMemRht, 0, &bRet);

        if (!bRet)
            goto RIGHT_DOMAIN_EXIT;
    } // End IF

    // Check for value 1
    if (aplLongestRht NE 1)
        goto RIGHT_DOMAIN_EXIT;

    // Uninitialize Combinatorial cache
    UninitCombCache ();

    // Initialize Combinatorial cache
    InitCombCache ();

    // Return a message

#define MSG     L"Cache cleared"

    // Save the NELM
    aplNELMRes = strcountof (MSG);

    // Allocate a result Char vector of length <aplIntRes>
    hGlbRes = AllocateGlobalArray (ARRAY_CHAR, aplNELMRes, 1, &aplNELMRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Copy the message to the result
    CopyMemory (lpMemRes, MSG, (APLU3264) (aplNELMRes * sizeof (lpMemRes[0])));

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
////lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex; // Set by caller

    return lpYYRes;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
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
    return NULL;
} // End ClearCombCache_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_comb.c
//***************************************************************************
