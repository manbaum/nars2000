//***************************************************************************
//  NARS2000 -- Axis Functions
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2019 Sudley Place Software

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
//  $CheckAxisOper
//
//  Check for axis operator
//***************************************************************************

LPTOKEN CheckAxisOper
    (LPPL_YYSTYPE lpYYFcnStr)       // Ptr to function strand

{
    if (lpYYFcnStr NE NULL
     && lpYYFcnStr->TknCount > 1
     && IsTknTypeAxis (lpYYFcnStr[1].tkToken.tkFlags.TknType))
        return &lpYYFcnStr[1].tkToken;
    else
        return NULL;
} // End CheckAxisOper


//***************************************************************************
//  $CheckAxisImm
//
//  Subroutine to CheckAxis_EM for immediate values
//***************************************************************************

UBOOL CheckAxisImm
    (IMM_TYPES    immType,          // Type of the immediate value
     APLLONGEST   aplLongest,       // The immediate value
     LPTOKEN      lptkAxis,         // The Axis values
     APLRANK      aplRankCmp,       // Comparison rank
     LPUBOOL      lpbFract,         // Return TRUE iff fractional values are present,
                                    //   (may be NULL if fractional values not allowed)
     LPAPLINT     lpaplLastAxis,    // Return last axis value or ceiling if fractional
                                    //   (may be NULL if caller is not interested)
     LPAPLNELM    lpaplNELMAxis,    // Return # elements in axis
                                    //   (may be NULL if caller is not interested)
     HGLOBAL      *lphGlbAxis,      // Ptr to HGLOBAL where the cleaned up axis
                                    //   is to be stored.  If the return is FALSE,
                                    //   this ptr must be set to NULL.
                                    //   (may be NULL if caller is not interested)
     LPAPLNELM    lpaplNELM,        // Local var w/NELM
     LPAPLINT    *lplpAxisStart,    // Ptr to ptr to start of Axis values in *lphGlbAxis
     LPAPLINT    *lplpAxisHead,     // ...                    user axis values in *lphGlbAxis
     LPAPLUINT    lpaplAxisContLo,  // Contiguous low axis (not NULL)
     LPPERTABDATA lpMemPTD)         // Ptr to PertabData global memory

{
    APLUINT  ByteAxis;              // # bytes for the axis global
    APLRANK  aplRank;               // Maximum rank for comparison
    UBOOL    bRet = TRUE;           // TRUE iff result is valid
    UINT     uCnt;                  // Loop counter
    LPAPLINT lpAxisTail;            // Ptr to grade up of AxisHead
    APLBOOL  bQuadIO;               // []IO

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // The immediate value minus []IO
    //   must be an integer in the range
    //   [0, aplRankCmp - 1], inclusive.

    // Set # elements
    *lpaplNELM = 1;

    // Return the # elements
    if (lpaplNELMAxis NE NULL)
        *lpaplNELMAxis = *lpaplNELM;

    if (lphGlbAxis NE NULL)
    {
        APLUINT aplMaxAlloc;

        // If the comparison rank is zero, the allocation
        //   size is zero, and the lock ptr is zero
        //   which GlobalLock will treat as an error,
        //   returning a zero ptr, so we max aplRankCmp with 1.
        aplMaxAlloc = max (aplRankCmp, 1);

        // Use larger as we may need to store *lpaplNELM axes
        //   in *lphGlbAxis
        aplMaxAlloc = max (aplMaxAlloc, *lpaplNELM);

        // Calculate space needed for axis
        ByteAxis = sizeof (APLUINT) * 2 * aplMaxAlloc;

        // Check for overflow
        if (ByteAxis NE (APLU3264) ByteAxis)
            goto ERROR_EXIT;

        *lphGlbAxis = DbgGlobalAlloc (GHND, (APLU3264) ByteAxis);
        // Allocate storage for the axis vector
        if (*lphGlbAxis EQ NULL)
            goto ERROR_EXIT;

        // Lock the memory to get a ptr it
        *lplpAxisStart = *lplpAxisHead = MyGlobalLock000 (*lphGlbAxis);

        // Point to the start of the trailing axes
        lpAxisTail = &(*lplpAxisHead)[aplMaxAlloc-*lpaplNELM];
    } // End IF

    // Split cases based upon the immediate axis type
    switch (lptkAxis->tkFlags.ImmType)
    {
        case IMMTYPE_BOOL:
        case IMMTYPE_INT:
            // Get the Boolean or Integer value,
            //   less the current index origin
            aplRank = aplLongest - bQuadIO;

            // Check for negative indices [-aplRankCmp, -1]
            if (SIGN_APLRANK (aplRank)
             && gbAllowNegIndices)
                aplRank += aplRankCmp;

            // Ensure it's within range
            // Note that because aplRank and aplRankCmp
            //   are unsigned, we don't need to check
            //   for below zero
            bRet = (aplRank < aplRankCmp);

            break;

        case IMMTYPE_FLOAT:     // Ensure it's close enough
            // Attempt to convert the float to an integer using System []CT
            aplRank = FloatToAplint_SCT (lptkAxis->tkData.tkFloat, &bRet);
            aplRank -= bQuadIO; // Less the index origin

            // Check for negative indices [-aplRankCmp, -1]
            if (SIGN_APLRANK (aplRank)
             && gbAllowNegIndices)
                aplRank += aplRankCmp;

            // If fractional values are allowed,
            //   return whether or not they are present
            if (lpbFract NE NULL)
                *lpbFract = !bRet;

            // If fractional values allowed and are present, ...
            if (lpbFract NE NULL
             && !bRet)
                bRet = TRUE;

            // Ensure it's within range
            // Note that because aplRank and aplRankCmp
            //   are unsigned, we don't need to check
            //   for below zero
            bRet &= (aplRank < aplRankCmp);

            break;

        case IMMTYPE_CHAR:      // No chars allowed
            goto ERROR_EXIT;

        defstop
            goto ERROR_EXIT;
    } // End SWITCH

    if (bRet
     && lphGlbAxis NE NULL)
    {
        // Save the remaining values
        for (uCnt = 0; uCnt < aplRankCmp; uCnt++)
        if (uCnt NE aplRank)
            *(*lplpAxisHead)++ = uCnt;

        // Save the trailing value
        *lpAxisTail++ = aplRank;
    } // End IF

    // Save the contiguous low axis
    *lpaplAxisContLo = aplRank;

    // Save the last (and only) value
    if (lpaplLastAxis NE NULL)
        *lpaplLastAxis = aplRank;

    if (bRet)
        goto NORMAL_EXIT;
    else
        goto ERROR_EXIT;

ERROR_EXIT:
    if ( lphGlbAxis NE NULL
     && *lphGlbAxis NE NULL)
    {
        if (*lplpAxisStart NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (*lphGlbAxis); *lplpAxisStart = *lplpAxisHead = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (*lphGlbAxis); *lphGlbAxis = NULL;
    } // End IF

    // Mark as in error
    bRet = FALSE;
NORMAL_EXIT:
    return bRet;
} // End CheckAxisImm


//***************************************************************************
//  $CheckAxisGlb_EM
//
//  Subroutine to CheckAxis_EM for global values
//***************************************************************************

UBOOL CheckAxisGlb_EM
    (HGLOBAL      hGlbData,             // The global handle to check
     LPTOKEN      lptkAxis,             // The Axis values
     APLRANK      aplRankCmp,           // Comparison rank
     UBOOL        bSingleton,           // TRUE iff scalar or one-element vector only
                                        //   is allowed
     UBOOL        bSortAxes,            // TRUE iff the axes should be sorted
                                        //   (i.e., the order of the axes is unimportant)
     UBOOL        bContiguous,          // TRUE iff the axes must be contiguous
     UBOOL        bAllowDups,           // TRUE iff duplicate axes are allowed
     LPUBOOL      lpbFract,             // Return TRUE iff fractional values are present,
                                        //   (may be NULL if fractional values not allowed)
     LPAPLINT     lpaplLastAxis,        // Return last axis value or ceiling if fractional
                                        //   (may be NULL if caller is not interested)
     LPAPLNELM    lpaplNELMAxis,        // Return # elements in axis
                                        //   (may be NULL if caller is not interested)
     HGLOBAL     *lphGlbAxis,           // Ptr to HGLOBAL where the cleaned up axis
                                        //   is to be stored.  If the return is FALSE,
                                        //   this ptr must be set to NULL.
                                        //   (may be NULL if caller is not interested)
     LPAPLNELM    lpaplNELM,            // Local var for NELM
     LPAPLINT    *lplpAxisStart,        // Ptr to ptr to start of Axis values in *lphGlbAxis
     LPAPLINT    *lplpAxisHead,         // ...                    user axis values in *lphGlbAxis
     LPAPLUINT    lpaplAxisContLo,      // Contiguous low axis (not NULL)
     LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    UBOOL             bRet = TRUE;      // TRUE iff the result is valid
    LPVARARRAY_HEADER lpMemHdrData;     // Ptr to data header
    LPVOID            lpMemData;        // Ptr to incoming data global memory
    LPAPLBOOL         lpMemDup = NULL;  // Ptr to duplciate axes global memory
    HGLOBAL           hGlbDup = NULL;   // Duplicate axes global memory handle
    UINT              uBitMask;         // Bit mask for looping through Booleans
    APLUINT           ByteDup,          // # bytes for the duplicate axis test
                      ByteAxis,         // # bytes for the axis vector
                      uCnt;             // Loop counter
    APLSTYPE          aplTypeLcl;       // Incoming data storage type
    APLRANK           aplRankLcl;       // Incoming data rank
    LPAPLINT          lpAxisTail;       // Ptr to grade up of AxisHead
    APLBOOL           bQuadIO;          // []IO

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // If the data is a list, ....
    if (IsGlbTypeLstDir_PTB (hGlbData))
        return FALSE;

    // st/tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir_PTB (hGlbData));

    // Lock the memory to get a ptr to it
    lpMemHdrData = MyGlobalLockVar (hGlbData);

    // Get the Array Type, NELM, and Rank
    aplTypeLcl = lpMemHdrData->ArrType;
   *lpaplNELM  = lpMemHdrData->NELM;
    aplRankLcl = lpMemHdrData->Rank;

    // Check the axis rank and the NELM (if singletons only)
    if (IsRank2P (aplRankLcl)
     || (bSingleton && !IsSingleton (*lpaplNELM))
     || IsSimpleChar (aplTypeLcl)
     || IsPtrArray (aplTypeLcl))
        goto ERROR_EXIT;

    // Return the # elements
    if (lpaplNELMAxis NE NULL)
        *lpaplNELMAxis = *lpaplNELM;

    if (lphGlbAxis NE NULL)
    {
        APLUINT aplMaxAlloc;

        // If the comparison rank is zero, the allocation
        //   size is zero, and the lock ptr is zero
        //   which GlobalLock will treat as an error,
        //   returning a zero ptr, so we max aplRankCmp with 1.
        aplMaxAlloc = max (aplRankCmp, 1);

        // Use larger as we may need to store *lpaplNELM axes
        //   in *lphGlbAxis
        aplMaxAlloc = max (aplMaxAlloc, *lpaplNELM);

        // Calculate space needed for axis
        ByteAxis = sizeof (APLUINT) * 2 * aplMaxAlloc;

        // Check for overflow
        if (ByteAxis NE (APLU3264) ByteAxis)
            goto WSFULL_EXIT;

        // Allocate storage for the axis vector
        *lphGlbAxis = DbgGlobalAlloc (GHND, (APLU3264) ByteAxis);
        if (*lphGlbAxis EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        *lplpAxisStart = *lplpAxisHead = MyGlobalLock000 (*lphGlbAxis);

        // Point to the start of the trailing axes
        lpAxisTail = &(*lplpAxisHead)[aplMaxAlloc-*lpaplNELM];
    } // End IF

    // If the comparison rank is zero, the allocation
    //   size is zero, and the lock ptr is zero
    //   which GlobalLock will treat as an error,
    //   returning a zero ptr, so we max aplRankCmp with 1.
    ByteDup = RoundUpBitsToBytes (max (aplRankCmp, 1));

    // Check for overflow
    if (ByteDup NE (APLU3264) ByteDup)
        goto WSFULL_EXIT;

    // Allocate global memory bit vector to test for duplicates
    hGlbDup = DbgGlobalAlloc (GHND, (APLU3264) ByteDup);
    if (hGlbDup EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to the
    //   duplicate indices testing area
    lpMemDup = MyGlobalLockInt (hGlbDup);       // Might be only 1 byte

    // Skip over the header and dimensions to the data
    lpMemData = VarArrayDataFmBase (lpMemHdrData);

    // If the axis value is an empty char array, ...
    if (IsCharEmpty (aplTypeLcl, *lpaplNELM))
        // Treat it as an empty Boolean array
        aplTypeLcl = ARRAY_BOOL;

    // Loop through the elements
    for (uCnt = 0; bRet && uCnt < *lpaplNELM; uCnt++)
    {
        // Attempt to convert the items of the axis to an INT
        aplRankLcl = ConvertToInteger_SCT (aplTypeLcl, lpMemData, uCnt, &bRet) - bQuadIO;

        // If the conversion failed,
        if (!bRet)
        {
            // See if the value is a real part fractional FLT or something close to that.
            // The best way is to check the imaginary values for zero in which case
            //   the item is not fractional.
            if (IzitImaginary (aplTypeLcl, lpMemData))
                goto ERROR_EXIT;
        } // End IF

        // Check for negative indices [-aplRankCmp, -1]
        if (SIGN_APLRANK (aplRankLcl)
         && gbAllowNegIndices)
            aplRankLcl += aplRankCmp;

        // If fractional values are allowed,
        //   return whether or not they are present
        if (lpbFract NE NULL)
            *lpbFract |= !bRet;

        // If fractional values allowed and are present, ...
        if (lpbFract NE NULL
         && !bRet)
            bRet = TRUE;

        // Ensure it's within range
        // Note that because aplRankLcl and aplRankCmp
        //   are unsigned, we don't need to check
        //   for below zero
        bRet &= (aplRankLcl < aplRankCmp);

        // Save the next trailing value
        //   if asked to and not sorting
        //   the axes.
        if (bRet
         && lphGlbAxis NE NULL
         && !bSortAxes)
            *lpAxisTail++ = aplRankLcl;

        // Test for duplicates
        if (bRet)
            bRet = TestDupAxis (lpMemDup, aplRankLcl, bAllowDups);
    } // End FOR

    // Save the last value
    if (lpaplLastAxis NE NULL)
        *lpaplLastAxis = aplRankLcl;

    // Fill in the leading axis values
    if (bRet
     && lphGlbAxis NE NULL
     && !bAllowDups)
    {
        uBitMask = BIT0;

        // Loop through the lpMemDup values looking for zeros
        for (uCnt = 0; uCnt < aplRankCmp; uCnt++)
        {
            // If the bit is a zero, ...
            if ((uBitMask & *lpMemDup) EQ 0)
                // Save the next value
                *(*lplpAxisHead)++ = uCnt;
            else            // the bit is a one
            if (bSortAxes)  // and we're sorting axes
                // Fill in the trailing axis values in order
                *lpAxisTail++ = uCnt;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = BIT0;        // Start over
                lpMemDup++;             // Skip to next byte
            } // End IF
        } // End FOR
    } // End IF

    // If the axes must be contiguous, check that
    if (bRet
     && (bContiguous || bAllowDups))
    {
        // Unlock and lock the memory to reset the
        //   ptr to the start
        MyGlobalUnlock (hGlbDup); lpMemDup = NULL;
        lpMemDup = MyGlobalLockInt (hGlbDup);   // Might be only 1 byte

        uBitMask = BIT0;

        // Look for the first 1
        for (uCnt = 0; uCnt < aplRankCmp; uCnt++)
        {
            // If it's a 1, break
            if (uBitMask & *lpMemDup)
                break;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = BIT0;        // Start over
                lpMemDup++;             // Skip to next byte
            } // End IF
        } // End FOR

        // Save as contiguous low axis
        *lpaplAxisContLo = uCnt;

        // Look for consecutive 1s
        for (; uCnt < aplRankCmp; uCnt++)
        {
            // If it's a 0, break
            if (!(uBitMask & *lpMemDup))
                break;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = BIT0;        // Start over
                lpMemDup++;             // Skip to next byte
            } // End IF
        } // End FOR

        // Save as highest contiguous axis
        if (lpaplLastAxis NE NULL)
            *lpaplLastAxis = uCnt - 1;

        // Look for consecutive 0s
        for (; uCnt < aplRankCmp; uCnt++)
        {
            // If it's a 1, break
            if (uBitMask & *lpMemDup)
                break;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = BIT0;        // Start over
                lpMemDup++;             // Skip to next byte
            } // End IF
        } // End FOR

        // If we're not at the end, the axes
        //   are not contiguous
        bRet = (uCnt EQ aplRankCmp);
    } // End IF

    // If we allow duplicates, ...
    if (bRet
     && bAllowDups
     && aplRankCmp NE 0)
    {
        // Unlock and lock the memory to reset the
        //   ptr to the start
        MyGlobalUnlock (hGlbDup); lpMemDup = NULL;
        lpMemDup = MyGlobalLockInt (hGlbDup);   // Might be only 1 byte

        // If so (it's slicing dyadic transpose), so the axes
        //   must be contiguous starting at []IO.
        bRet = (*lpMemDup & BIT0);
    } // End IF

    if (bRet)
        goto NORMAL_EXIT;
    else
        goto ERROR_EXIT;
WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

ERROR_EXIT:
    if ( lphGlbAxis NE NULL
     && *lphGlbAxis NE NULL)
    {
        if (*lplpAxisStart NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (*lphGlbAxis); *lplpAxisStart = *lplpAxisHead = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (*lphGlbAxis); *lphGlbAxis = NULL;
    } // End IF

    // Mark as in error
    bRet = FALSE;
NORMAL_EXIT:
    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbDup, lpMemDup);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMemHdrData = NULL;

    return bRet;
} // End CheckAxisGlb_EM


//***************************************************************************
//  $AllocateAllAxes
//
//  Allocate space for a non-APL all axes vector
//***************************************************************************

UBOOL AllocateAllAxes
    (HGLOBAL    *lphGlbAxis,        // Ptr to HGLOBAL where the cleaned up axis
                                    //   is to be stored.  If the return is FALSE,
                                    //   this ptr must be set to NULL.
                                    //   (may be NULL if caller is not interested)
     APLRANK    aplRankCmp,         // Comparison rank
     LPAPLINT   lpaplLastAxis,      // Return last axis value or ceiling if fractional
                                    //   (may be NULL if caller is not interested)
     LPAPLNELM  lpaplNELMAxis)      // Return # elements in axis
                                    //   (may be NULL if caller is not interested)

{
    APLUINT   ByteAxis;             // # bytes for the axis vector
    LPAPLUINT lpMemAxis;            // Ptr to axis values, fleshed out by CheckAxis_EM
    UINT      uCnt;                 // Loop counter

    Assert (lphGlbAxis NE NULL);

    //***************************************************************
    // Calculate space needed for axis
    //
    // If the comparison rank is zero, the allocation
    //   size is zero, and the lock ptr is zero
    //   which GlobalLock will treat as an error,
    //   returning a zero ptr, so we max aplRankCmp with 1.
    //***************************************************************
    ByteAxis = sizeof (APLUINT) * 2 * max (aplRankCmp, 1);

    //***************************************************************
    // Check for overflow
    //***************************************************************
    if (ByteAxis NE (APLU3264) ByteAxis)
        goto WSFULL_EXIT;

    //***************************************************************
    // Allocate storage for the axis vector
    //***************************************************************
    *lphGlbAxis = DbgGlobalAlloc (GHND, (APLU3264) ByteAxis);
    if (*lphGlbAxis EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemAxis = MyGlobalLock000 (*lphGlbAxis);

    // Fill the memory with [0, alpRankCmp-1]
    for (uCnt = 0; uCnt < aplRankCmp; uCnt++)
        *lpMemAxis++ = uCnt;

    // Second verse same as the first
    for (uCnt = 0; uCnt < aplRankCmp; uCnt++)
        *lpMemAxis++ = uCnt;

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbAxis); lpMemAxis = NULL;

    // Save the last value
    if (lpaplLastAxis NE NULL)
        *lpaplLastAxis = uCnt;

    // Return the # elements
    if (lpaplNELMAxis NE NULL)
        *lpaplNELMAxis = aplRankCmp;

    return TRUE;

WSFULL_EXIT:
    return FALSE;
} // End AllocateAllAxes


//***************************************************************************
//  $CheckAxis_EM
//
//  Check an axis value against a given rank
//***************************************************************************

UBOOL CheckAxis_EM
    (LPTOKEN    lptkAxis,           // The Axis values
     APLRANK    aplRankCmp,         // Comparison rank
     UBOOL      bSingleton,         // TRUE iff scalar or one-element vector only
                                    //   is allowed
     UBOOL      bSortAxes,          // TRUE iff the axes should be sorted
                                    //   (i.e., the order of the axes is unimportant)
     UBOOL      bContiguous,        // TRUE iff the axes must be contiguous
     UBOOL      bAllowDups,         // TRUE iff duplicate axes are allowed
     LPUBOOL    lpbFract,           // Return TRUE iff fractional values are present,
                                    //   (may be NULL if fractional values not allowed)
     LPAPLINT   lpaplLastAxis,      // Return last axis value or ceiling if fractional
                                    //   (may be NULL if caller is not interested)
     LPAPLNELM  lpaplNELMAxis,      // Return # elements in axis
                                    //   (may be NULL if caller is not interested)
     HGLOBAL    *lphGlbAxis)        // Ptr to HGLOBAL where the cleaned up axis
                                    //   is to be stored.  If the return is FALSE,
                                    //   this ptr must be set to NULL.
                                    //   (may be NULL if caller is not interested)
{
    UBOOL        bRet = TRUE;           // TRUE iff the result is valid
    APLNELM      aplNELM;               //
    LPAPLINT     lpAxisStart = NULL,    // Ptr to start of Axis values in *lphGlbAxis
                 lpAxisHead = NULL;     // ...             user axis values in *lphGlbAxis
    UINT         uCnt;                     // Loop counter
    APLUINT      aplAxisContLo;         // Contiguous low axis
    HGLOBAL      hGlbData = NULL;       //
    IMM_TYPES    immType;               //
    APLLONGEST   aplLongest;            //
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // If the caller requests, an axis vector is allocated
    //   of length <2 * aplRankCmp>.  In the first <aplRankCmp>
    //   values, the trailing <*lpaplNELMAxis> values are
    //   the actual values in the axis (converted to origin-0
    //   APLINTs), and the leading values are all the other
    //   values in {iota}aplRankCmp.
    // The second <aplRankCmp> values are the grade-up of
    //   the first <aplRankCmp> values.

    // Mark as no fractional values
    if (lpbFract NE NULL)
        *lpbFract = FALSE;

    // If there's no axis token, allocate storage anyway
    if (lptkAxis EQ NULL)
    {
        // Allocate space for all axes
        if (!AllocateAllAxes (lphGlbAxis,
                              aplRankCmp,
                              lpaplLastAxis,
                              lpaplNELMAxis))
            goto WSFULL_EXIT;
        else
            goto NORMAL_EXIT;
    } // End IF

    // Split cases based upon the axis token type
    switch (lptkAxis->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkAxis->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, it's an HGLOBAL
            if (!lptkAxis->tkData.tkSym->stFlags.Imm)
                // Get the global handle
                hGlbData = lptkAxis->tkData.tkSym->stData.stGlbData;
            else
            {
                // Get the immediate type and value
                immType    = lptkAxis->tkData.tkSym->stFlags.ImmType;
                aplLongest = lptkAxis->tkData.tkSym->stData.stLongest;
            } // End IF/ELSE

            break;

        case TKT_VARIMMED:
        case TKT_AXISIMMED:
            // Get the immediate type and value
            immType    = lptkAxis->tkFlags.ImmType;
            aplLongest = *GetPtrTknLongest (lptkAxis);

            break;

        case TKT_VARARRAY:
        case TKT_AXISARRAY:
        case TKT_LSTMULT:           // From +/[] ...
            // Get the global handle
            hGlbData = lptkAxis->tkData.tkGlbData;

            break;

        defstop
            break;
    } // End SWITCH

    if (hGlbData NE NULL)
        // Handle the global case
        bRet = CheckAxisGlb_EM (hGlbData,       // The global handle to check
                                lptkAxis,       // The axis values
                                aplRankCmp,     // Comparison rank
                                bSingleton,     // TRUE iff scalar or one-element vector only
                                                //   is allowed
                                bSortAxes,      // TRUE iff the axes should be sorted
                                                //   (i.e., the order of the axes is unimportant)
                                bContiguous,    // TRUE iff the axes must be contiguous
                                bAllowDups,     // TRUE iff duplicate axes are allowed
                                lpbFract,       // Return TRUE iff fractional values are present,
                                                //   (may be NULL if fractional values not allowed)
                                lpaplLastAxis,  // Return last axis value or ceiling if fractional
                                                //   (may be NULL if caller is not interested)
                                lpaplNELMAxis,  // Return # elements in axis
                                                //   (may be NULL if caller is not interested)
                                lphGlbAxis,     // Ptr to HGLOBAL where the cleaned up axis
                                                //   is to be stored.  If the return is FALSE,
                                                //   this ptr must be set to NULL.
                                                //   (may be NULL if caller is not interested)
                               &aplNELM,        // Local var for NELM
                               &lpAxisStart,    // Ptr to ptr to start of Axis values in *lphGlbAxis
                               &lpAxisHead,     // ...                    user axis values in *lphGlbAxis
                               &aplAxisContLo,  // Contiguous low axis
                                lpMemPTD);      // Ptr to PerTabData global memory
    else
        // Handle the immediate case
        bRet = CheckAxisImm (immType,
                             aplLongest,
                             lptkAxis,
                             aplRankCmp,    // Comparison rank
                             lpbFract,      // Return TRUE iff fractional values are present,
                                            //   (may be NULL if fractional values not allowed)
                             lpaplLastAxis, // Return last axis value or ceiling if fractional
                                            //   (may be NULL if caller is not interested)
                             lpaplNELMAxis, // Return # elements in axis
                                            //   (may be NULL if caller is not interested)
                             lphGlbAxis,    // Ptr to HGLOBAL where the cleaned up axis
                                            //   is to be stored.  If the return is FALSE,
                                            //   this ptr must be set to NULL.
                                            //   (may be NULL if caller is not interested)
                            &aplNELM,       // Local var for NELM
                            &lpAxisStart,   // Ptr to ptr to start of Axis values in *lphGlbAxis
                            &lpAxisHead,    // ...                    user axis values in *lphGlbAxis
                            &aplAxisContLo, // Contiguous low axis
                             lpMemPTD);     // Ptr to PerTabData global memory
    // If bad values, it's an AXIS ERROR
    if (!bRet)
        goto AXIS_EXIT;

    if ( lphGlbAxis NE NULL
     && *lphGlbAxis NE NULL)
    {
        if (bContiguous
         && !bAllowDups)
        {
            // If the axes are to be contiguous and no duplicates,
            //   this must be ravel (contiguous and duplicates is transpose)
            //   in which case the axes must be represented as
            //   A{is}{iota}aplRankCmp {diamond} A[X[{gradeup}X]]{is}X
            //   not as they are above for the dyadic scalar functions.

            // We accomplish this by forming the above result in the
            //   second half of lpAxisStart, and copying it to the
            //   first half.

            // A{is}{iota}aplRankCmp
            for (uCnt = 0; uCnt < aplRankCmp; uCnt++)
                lpAxisStart[aplRankCmp + uCnt] = uCnt;

            // A[X[{gradeup}X]]{is}X
            for (uCnt = 0; uCnt < aplNELM; uCnt++)
                lpAxisStart[aplRankCmp + aplAxisContLo + uCnt] = lpAxisHead[uCnt];

            // Copy back to the first half
            CopyMemory (lpAxisStart, &lpAxisStart[aplRankCmp], (APLU3264) (aplRankCmp * sizeof (lpAxisStart[0])));
        } // End IF

        // Place the grade-up of lpAxisStart into &lpAxisStart[aplRankCmp]
        if (bAllowDups)
            GradeUp (lpAxisStart, &lpAxisStart[aplRankCmp], aplRankCmp);
        else
            // Because these values are guaranteed to be a permutation
            //   vector, we can use an address sort.
            for (uCnt = 0; uCnt < aplRankCmp; uCnt++)
                lpAxisStart[aplRankCmp + lpAxisStart[uCnt]] = uCnt;

        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbAxis); lpAxisStart = lpAxisHead = NULL;
    } // End IF/ELSE

    goto NORMAL_EXIT;

AXIS_EXIT:
    ErrorMessageIndirectToken (ERRMSG_AXIS_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

ERROR_EXIT:
    if ( lphGlbAxis NE NULL
     && *lphGlbAxis NE NULL)
    {
        if (lpAxisStart NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (*lphGlbAxis); lpAxisStart = lpAxisHead = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (*lphGlbAxis); *lphGlbAxis = NULL;
    } // End IF

    // Mark as in error
    bRet = FALSE;
NORMAL_EXIT:
    return bRet;
} // End CheckAxis_EM


//***************************************************************************
//  $TestDupAxis
//
//  Test an axis value for duplicates
//***************************************************************************

UBOOL TestDupAxis
    (LPAPLBOOL lpMemDup,
     APLRANK   aplRank,
     UBOOL     bAllowDups)

{
    UBOOL bRet = TRUE;
    UINT  uBitMask;

    // Calculate the bit mask
    uBitMask = (BIT0 << (UINT) (aplRank % NBIB));

    // See if this value has already been seen
    if (!bAllowDups)
        bRet = !(uBitMask & lpMemDup[aplRank >> LOG2NBIB]);

    // Set this value for the next time if necessary
    if (bRet
     || bAllowDups)
        lpMemDup[aplRank >> LOG2NBIB] |= uBitMask;

    return bRet;
} // End TestDupAxis


//***************************************************************************
//  End of File: axisfns.c
//***************************************************************************
