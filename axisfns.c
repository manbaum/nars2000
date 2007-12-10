//***************************************************************************
//  NARS2000 -- Axis Functions
//***************************************************************************

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
//  $CheckAxisOper
//
//  Check for axis operator
//***************************************************************************

LPTOKEN CheckAxisOper
    (LPPL_YYSTYPE lpYYFcnStr)       // Ptr to function strand

{
    if (lpYYFcnStr
     && lpYYFcnStr->FcnCount
     && lpYYFcnStr->FcnCount > 1
     && (lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
      || lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
        return &lpYYFcnStr[1].tkToken;
    else
        return NULL;
} // End CheckAxisOper


//***************************************************************************
//  $CheckAxisImm
//
//  Subroutine to CheckAxis_EM for immediate values
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckAxisImm"
#else
#define APPEND_NAME
#endif

BOOL CheckAxisImm
    (UINT       immType,        // Type of the immediate value
     APLLONGEST aplLongest,     // The immediate value
     LPTOKEN    lptkAxis,       // The Axis values
     APLRANK    aplRankCmp,     // Comparison rank
     LPBOOL     lpbFract,       // Return TRUE iff fractional values are present,
                                //   (may be NULL if fractional values not allowed)
     LPAPLINT   lpaplLastAxis,  // Return last axis value or ceiling if fractional
                                //   (may be NULL if caller is not interested)
     LPAPLNELM  lpaplNELMAxis,  // Return # elements in axis
                                //   (may be NULL if caller is not interested)
     HGLOBAL    *lphGlbAxis,    // Ptr to HGLOBAL where the cleaned up axis
                                //   is to be stored.  If the return is FALSE,
                                //   this ptr must be set to NULL.
                                //   (may be NULL if caller is not interested)
     LPAPLNELM  lpaplNELM,      // Local var w/NELM
     LPAPLINT  *lplpAxisStart,  // Ptr to ptr to start of Axis values in *lphGlbAxis
     LPAPLINT  *lplpAxisHead,   // ...                    user axis values in *lphGlbAxis
     LPAPLUINT  lpaplAxisContLo)// Contiguous low axis (not NULL)

{
    APLUINT  ByteAxis;          // # bytes for the axis global
    APLRANK  aplRank;           // Maximum rank for comparison
    BOOL     bRet = TRUE;       // TRUE iff result is valid
    UINT     u;                 // Loop counter
    LPAPLINT lpAxisTail;        // Ptr to grade up of AxisHead
    APLBOOL  bQuadIO;           // []IO

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

    if (lphGlbAxis)
    {
        // Calculate space needed for axis
        // If the comparison rank is zero, the allocation
        //   size is zero, and the lock ptr is zero
        //   which GlobalLock will treat as an error,
        //   returning a zero ptr, so we max aplRankCmp with 1.
        ByteAxis = sizeof (APLINT) * 2 * max (aplRankCmp, 1);

        // Allocate storage for the axis vector
        // N.B. Conversion from APLUINT to UINT.
        Assert (ByteAxis EQ (UINT) ByteAxis);
        *lphGlbAxis = DbgGlobalAlloc (GHND, (UINT) ByteAxis);
        if (!*lphGlbAxis)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkAxis);
            return FALSE;
        } // End IF

        // Lock the memory to get a ptr it
        *lplpAxisStart = *lplpAxisHead = MyGlobalLock (*lphGlbAxis);

        // Point to the start of the trailing axes
        lpAxisTail = &(*lplpAxisHead)[aplRankCmp - *lpaplNELM];
    } // End IF

    // Split cases based upon the immediate axis type
    switch (lptkAxis->tkFlags.ImmType)
    {
        case IMMTYPE_BOOL:
        case IMMTYPE_INT:
            // Get the Boolean or Integer value,
            //   less the current index origin
            aplRank = aplLongest - bQuadIO;

            // Ensure it's within range
            // Note that because aplRank and aplRankCmp
            //   are unsigned, we don't need to check
            //   against zero
            bRet = (aplRank < aplRankCmp);

            break;

        case IMMTYPE_FLOAT:     // Ensure it's close enough
            // Attempt to convert the float to an integer using System CT
            aplRank = FloatToAplint_SCT (lptkAxis->tkData.tkFloat, &bRet);
            aplRank -= bQuadIO; // Less the index origin

            // If fractional values are allowed,
            //   return whether or not they are present
            if (lpbFract)
                *lpbFract = !bRet;

            // If fractional values allowed and are present, ...
            if (lpbFract && !bRet)
                bRet = TRUE;

            // Ensure it's within range
            // Note that because aplRank and aplRankCmp
            //   are unsigned, we don't need to check
            //   against zero
            bRet = bRet && (aplRank < aplRankCmp);

            break;

        case IMMTYPE_CHAR:      // No chars allowed
            return FALSE;

        defstop
            return FALSE;
    } // End SWITCH

    if (bRet && lphGlbAxis)
    {
        // Save the remaining values
        for (u = 0; u < aplRankCmp; u++)
        if (u NE aplRank)
            *(*lplpAxisHead)++ = u;

        // Save the trailing value
        *lpAxisTail++ = aplRank;
    } // End IF

    // Save the contiguous low axis
    *lpaplAxisContLo = aplRank;

    // Save the last (and only) value
    if (lpaplLastAxis)
        *lpaplLastAxis = aplRank;
    return bRet;
} // End CheckAxisImm
#undef  APPEND_NAME


//***************************************************************************
//  $CheckAxisGlb
//
//  Subroutine to CheckAxis_EM for global values
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckAxisGlb"
#else
#define APPEND_NAME
#endif

BOOL CheckAxisGlb
    (HGLOBAL    hGlbData,       // The global handle to check
     LPTOKEN    lptkAxis,       // The Axis values
     APLRANK    aplRankCmp,     // Comparison rank
     BOOL       bSingleton,     // TRUE iff scalar or one-element vector only
                                //   is allowed
     BOOL       bSortAxes,      // TRUE iff the axes should be sorted
                                //   (i.e., the order of the axes is unimportant)
     BOOL       bContiguous,    // TRUE iff the axes must be contiguous
     BOOL       bAllowDups,     // TRUE iff duplicate axes are allowed
     LPBOOL     lpbFract,       // Return TRUE iff fractional values are present,
                                //   (may be NULL if fractional values not allowed)
     LPAPLINT   lpaplLastAxis,  // Return last axis value or ceiling if fractional
                                //   (may be NULL if caller is not interested)
     LPAPLNELM  lpaplNELMAxis,  // Return # elements in axis
                                //   (may be NULL if caller is not interested)
     HGLOBAL   *lphGlbAxis,     // Ptr to HGLOBAL where the cleaned up axis
                                //   is to be stored.  If the return is FALSE,
                                //   this ptr must be set to NULL.
                                //   (may be NULL if caller is not interested)
     LPAPLNELM  lpaplNELM,      // Local var for NELM
     LPAPLINT  *lplpAxisStart,  // Ptr to ptr to start of Axis values in *lphGlbAxis
     LPAPLINT  *lplpAxisHead,   // ...                    user axis values in *lphGlbAxis
     LPAPLUINT  lpaplAxisContLo)// Contiguous low axis (not NULL)

{
    BOOL     bRet = TRUE;
    LPVOID   lpMem,
             lpDup = NULL;
    HGLOBAL  hGlbDup = NULL;
    UINT     uBitMask;
    APLUINT  ByteDup,
             ByteAxis,
             u;
    APLSTYPE aplTypeLcl;
    APLRANK  aplRankLcl;
    LPAPLINT lpAxisTail;        // Ptr to grade up of AxisHead
    APLBOOL  bQuadIO;           // []IO

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // st/tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (hGlbData));

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (ClrPtrTypeDirAsGlb (hGlbData));

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
    // Get the Array Type, NELM, and Rank
    aplTypeLcl = lpHeader->ArrType;
   *lpaplNELM  = lpHeader->NELM;
    aplRankLcl = lpHeader->Rank;
#undef  lpHeader

    // Check the axis rank and the NELM (if singletons only)
    if ((aplRankLcl > 1)
     || (bSingleton && *lpaplNELM NE 1))
        goto ERROR_EXIT;

    // Return the # elements
    if (lpaplNELMAxis NE NULL)
        *lpaplNELMAxis = *lpaplNELM;

    if (lphGlbAxis)
    {
        // Calculate space needed for axis
        // If the comparison rank is zero, the allocation
        //   size is zero, and the lock ptr is zero
        //   which GlobalLock will treat as an error,
        //   returning a zero ptr, so we max aplRankCmp with 1.
        ByteAxis = sizeof (APLINT) * 2 * max (aplRankCmp, 1);

        // Allocate storage for the axis vector
        // N.B. Conversion from APLUINT to UINT.
        Assert (ByteAxis EQ (UINT) ByteAxis);
        *lphGlbAxis = DbgGlobalAlloc (GHND, (UINT) ByteAxis);
        if (!*lphGlbAxis)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkAxis);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        *lplpAxisStart = *lplpAxisHead = MyGlobalLock (*lphGlbAxis);

        // Point to the start of the trailing axes
        lpAxisTail = &(*lplpAxisHead)[aplRankCmp - *lpaplNELM];
    } // End IF

    // If the comparison rank is zero, the allocation
    //   size is zero, and the lock ptr is zero
    //   which GlobalLock will treat as an error,
    //   returning a zero ptr, so we max aplRankCmp with 1.
    ByteDup = RoundUpBits8 (max (aplRankCmp, 1));

    // Allocate global memory bit vector to test for duplicates
    // N.B.  Conversion from APLUINT to UINT.
    Assert (ByteDup EQ (UINT) ByteDup);
    hGlbDup = DbgGlobalAlloc (GHND, (UINT) ByteDup);
    if (!hGlbDup)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkAxis);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to the
    //   duplicate indices testing area
    lpDup = MyGlobalLock (hGlbDup);

    // Skip over the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, aplRankLcl);

    // Split cases based upon the array type
    switch (aplTypeLcl)
    {
        case ARRAY_BOOL:
            uBitMask = 0x01;

            // Loop through the elements
            for (u = 0; bRet && u < *lpaplNELM; u++)
            {
                // Get the next bit value
                aplRankLcl = (uBitMask & *(LPAPLBOOL) lpMem) ? 1 : 0;
                aplRankLcl -= bQuadIO; // Less the index origin

                // Save the next trailing value
                //   if asked to and not sorting
                //   the axes.
                if (lphGlbAxis && !bSortAxes)
                    *lpAxisTail++ = aplRankLcl;

                // Test against the comparison rank
                bRet = (aplRankLcl < aplRankCmp);

                // Test for duplicates
                if (bRet)
                    bRet = TestDupAxis (lpDup, aplRankLcl, bAllowDups);

                if (bRet)
                {
                    // Shift over the bit mask
                    uBitMask <<= 1;

                    // Check for end-of-byte
                    if (uBitMask EQ END_OF_BYTE)
                    {
                        uBitMask = 0x01;        // Start over
                        ((LPAPLBOOL) lpMem)++;  // Skip to next byte
                    } // End IF
                } // End IF
            } // End FOR

            break;

        case ARRAY_INT:

#define lpaplInteger    ((LPAPLINT) lpMem)

            // Loop through the elements
            for (u = 0; bRet && u < *lpaplNELM; u++)
            {
                aplRankLcl = *lpaplInteger++ - bQuadIO;

                // Save the next trailing value
                //   if asked to and not sorting
                //   the axes.
                if (lphGlbAxis && !bSortAxes)
                    *lpAxisTail++ = aplRankLcl;

                // Ensure it's within range
                // Note that because aplRank and aplRankCmp
                //   are unsigned, we don't need to check
                //   against zero
                bRet = (aplRankLcl < aplRankCmp);

                // Test for duplicates
                if (bRet)
                    bRet = TestDupAxis (lpDup, aplRankLcl, bAllowDups);
            } // End FOR

#undef  lpaplInteger

            break;

        case ARRAY_FLOAT:

#define lpaplFloat      ((LPAPLFLOAT) lpMem)

            // Loop through the elements
            for (u = 0; bRet && u < *lpaplNELM; u++)
            {
                // Attempt to convert the float to an integer using System CT
                aplRankLcl = FloatToAplint_SCT (*lpaplFloat++, &bRet);
                aplRankLcl -= bQuadIO; // Less the index origin

                // If fractional values are allowed,
                //   return whether or not they are present
                if (lpbFract)
                    *lpbFract |= !bRet;

                // Save the next trailing value
                //   if asked to and not sorting
                //   the axes.
                if (lphGlbAxis && !bSortAxes)
                    *lpAxisTail++ = aplRankLcl;

                // If fractional values allowed and are present, ...
                if (lpbFract && !bRet)
                    bRet = TRUE;

                // Ensure it's within range
                // Note that because aplRank and aplRankCmp
                //   are unsigned, we don't need to check
                //   against zero
                bRet = bRet && (aplRankLcl < aplRankCmp);

                // Test for duplicates
                if (bRet)
                    bRet = TestDupAxis (lpDup, aplRankLcl, bAllowDups);
            } // End FOR

#undef  lpaplFloat

            break;

        case ARRAY_APA:
        {
            APLINT apaOff,
                   apaMul,
                   apaLen;

#define lpAPA       ((LPAPLAPA) lpMem)

            // Get the APA parameters
            apaOff = lpAPA->Off;
            apaMul = lpAPA->Mul;
            apaLen = *lpaplNELM;

#undef  lpAPA

            // Convert to origin-0
            apaOff -= bQuadIO;

            // It's sufficient to check the length and
            //   the first and last values for validity.
            bRet = (apaLen EQ 0)
                || (((apaOff + apaMul *           0 ) < (APLRANKSIGN) aplRankCmp)
                 && ((apaOff + apaMul * (apaLen - 1)) < (APLRANKSIGN) aplRankCmp));

            // Save the trailing axis values
            if (bRet)
            for (u = 0; bRet && u < (APLUINT) apaLen; u++)
            {
                // Get the next value
                aplRankLcl = apaOff + apaMul * u;

                // Save the next trailing value
                //   if asked to and not sorting
                //   the axes.
                if (lphGlbAxis && !bSortAxes)
                    *lpAxisTail++ = aplRankLcl;

                // Note there's no possibility of duplicates,
                //   however, we still need to fill in the
                //   bit array of values so we can fill in
                //   the leading axis values from the zeroes
                //   lpDup.
                // Test for duplicates
                if (bRet)
                    bRet = TestDupAxis (lpDup, aplRankLcl, bAllowDups);
            } // End FOR

            break;
        } // End ARRAY_APA

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
        case ARRAY_LIST:
            goto ERROR_EXIT;

            break;

        defstop
            break;
    } // End SWITCH

    // Save the last value
    if (lpaplLastAxis)
        *lpaplLastAxis = aplRankLcl;

    // Fill in the leading axis values
    if (bRet && lphGlbAxis && !bAllowDups)
    {
        uBitMask = 0x01;

        // Loop through the lpDup values looking for zeros
        for (u = 0; u < aplRankCmp; u++)
        {
            // If the bit is a zero, ...
            if (!(uBitMask & *(LPAPLBOOL) lpDup))
                // Save the next value
                *(*lplpAxisHead)++ = u;
            else            // the bit is a one
            if (bSortAxes)  // and we're sorting axes
                // Fill in the trailing axis values in order
                *lpAxisTail++ = u;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = 0x01;        // Start over
                ((LPAPLBOOL) lpDup)++;  // Skip to next byte
            } // End IF
        } // End FOR
    } // End IF

    // If the axes must be contiguous, check that
    if (bRet && bContiguous)
    {
        // Unlock and lock the memory to reset the
        //   ptr to the start
        MyGlobalUnlock (hGlbDup); lpDup = NULL;
        lpDup = MyGlobalLock (hGlbDup);

        uBitMask = 0x01;

        // Look for the first 1
        for (u = 0; u < aplRankCmp; u++)
        {
            // If it's a 1, break
            if (uBitMask & *(LPAPLBOOL) lpDup)
                break;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = 0x01;        // Start over
                ((LPAPLBOOL) lpDup)++;  // Skip to next byte
            } // End IF
        } // End FOR

        // Save as contiguous low axis
        *lpaplAxisContLo = u;

        // Look for consecutive 1s
        for (; u < aplRankCmp; u++)
        {
            // If it's a 0, break
            if (!(uBitMask & *(LPAPLBOOL) lpDup))
                break;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = 0x01;        // Start over
                ((LPAPLBOOL) lpDup)++;  // Skip to next byte
            } // End IF
        } // End FOR

        // Save as highest contiguous axis
        if (lpaplLastAxis)
            *lpaplLastAxis = u - 1;

        // Look for consecutive 0s
        for (; u < aplRankCmp; u++)
        {
            // If it's a 1, break
            if (uBitMask & *(LPAPLBOOL) lpDup)
                break;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = 0x01;        // Start over
                ((LPAPLBOOL) lpDup)++;  // Skip to next byte
            } // End IF
        } // End FOR

        // If we're not at the end, the axes
        //   are not contiguous
        bRet = (u EQ aplRankCmp);
    } // End IF

    goto NORMAL_EXIT;

ERROR_EXIT:
    bRet = FALSE;
NORMAL_EXIT:
    if (hGlbDup)
    {
        if (lpDup)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbDup); lpDup = NULL;
        } // End IF

        // We no longer need this HGLOBAL
        DbgGlobalFree (hGlbDup); hGlbDup = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (ClrPtrTypeDirAsGlb (hGlbData)); lpMem = NULL;

    return bRet;
} // End CheckAxisGlb
#undef  APPEND_NAME


//***************************************************************************
//  $CheckAxis_EM
//
//  Check an axis value against a given rank
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckAxis_EM"
#else
#define APPEND_NAME
#endif

BOOL CheckAxis_EM
    (LPTOKEN    lptkAxis,       // The Axis values
     APLRANK    aplRankCmp,     // Comparison rank
     BOOL       bSingleton,     // TRUE iff scalar or one-element vector only
                                //   is allowed
     BOOL       bSortAxes,      // TRUE iff the axes should be sorted
                                //   (i.e., the order of the axes is unimportant)
     BOOL       bContiguous,    // TRUE iff the axes must be contiguous
     BOOL       bAllowDups,     // TRUE iff duplicate axes are allowed
     LPBOOL     lpbFract,       // Return TRUE iff fractional values are present,
                                //   (may be NULL if fractional values not allowed)
     LPAPLINT   lpaplLastAxis,  // Return last axis value or ceiling if fractional
                                //   (may be NULL if caller is not interested)
     LPAPLNELM  lpaplNELMAxis,  // Return # elements in axis
                                //   (may be NULL if caller is not interested)
     HGLOBAL    *lphGlbAxis)    // Ptr to HGLOBAL where the cleaned up axis
                                //   is to be stored.  If the return is FALSE,
                                //   this ptr must be set to NULL.
                                //   (may be NULL if caller is not interested)
{
    BOOL       bRet = TRUE;
    APLNELM    aplNELM;
    LPAPLINT   lpAxisStart,     // Ptr to ptr to start of Axis values in *lphGlbAxis
               lpAxisHead;      // ...                    user axis values in *lphGlbAxis
    UINT       u;
    APLUINT    aplAxisContLo;   // Contiguous low axis
    HGLOBAL    hGlbData = NULL;
    UINT       immType;
    APLLONGEST aplLongest;

    // Ensure the object is a subset of {iota}aplRankCmp
    //   with no duplicates, and not necessarily in order.

    // If the caller requests, an axis vector is allocated
    //   of length <2 * aplRankCmp>.  In the first <aplRankCmp>
    //   values, the trailing <*lpaplNELMAxis> values are
    //   the actual values in the axis (converted to origin-0
    //   APLINTs), and the leading values are all the other
    //   values in {iota}aplRankCmp.
    // The second <aplRankCmp> values are the grade-up of
    //   the first <aplRankCmp> values.

    // Mark as no fractional values
    if (lpbFract)
        *lpbFract = FALSE;
    // Split cases based upon the axis token type
    switch (lptkAxis->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkAxis->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, it's an HGLOBAL
            if (!lptkAxis->tkData.tkSym->stFlags.Imm)
            {
                // Get the global handle
                hGlbData = lptkAxis->tkData.tkSym->stData.stGlbData;
            } else
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
            aplLongest = lptkAxis->tkData.tkLongest;

            break;

        case TKT_VARARRAY:
        case TKT_AXISARRAY:
            // Get the global handle
            hGlbData = lptkAxis->tkData.tkGlbData;

            break;

        defstop
            break;
    } // End SWITCH

    if (hGlbData)
        // Handle the global case
        bRet = CheckAxisGlb (hGlbData,      // The global handl;e to check
                             lptkAxis,      // The axis values
                             aplRankCmp,    // Comparison rank
                             bSingleton,    // TRUE iff scalar or one-element vector only
                                            //   is allowed
                             bSortAxes,     // TRUE iff the axes should be sorted
                                            //   (i.e., the order of the axes is unimportant)
                             bContiguous,   // TRUE iff the axes must be contiguous
                             bAllowDups,    // TRUE iff duplicate axes are allowed
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
                            &aplAxisContLo);// Contiguous low axis
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
                            &aplAxisContLo);// Contiguous low axis
    // If bad values, it's an AXIS ERROR
    if (!bRet)
    {
        ErrorMessageIndirectToken (ERRMSG_AXIS_ERROR APPEND_NAME,
                                   lptkAxis);
        if (lphGlbAxis && *lphGlbAxis)
        {
            // We no longer need this ptr
            MyGlobalUnlock (*lphGlbAxis); lpAxisStart = lpAxisHead = NULL;

            // We no longer need this storage
            DbgGlobalFree (*lphGlbAxis); *lphGlbAxis = NULL;
        } // End IF
    } else
    if (lphGlbAxis && *lphGlbAxis)
    {
        if (bContiguous && !bAllowDups)
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
            for (u = 0; u < aplRankCmp; u++)
                lpAxisStart[aplRankCmp + u] = u;

            // A[X[{gradeup}X]]{is}X
            for (u = 0; u < aplNELM; u++)
                lpAxisStart[aplRankCmp + aplAxisContLo + u] = lpAxisHead[u];

            // Copy back to the first half
            CopyMemory (lpAxisStart, &lpAxisStart[aplRankCmp], (UINT) (aplRankCmp * sizeof (lpAxisStart[0])));
        } // End IF

        // Place the grade-up of lpAxisStart into &lpAxisStart[aplRankCmp]
        if (bAllowDups)
            GradeUp (lpAxisStart, &lpAxisStart[aplRankCmp], aplRankCmp);
        else
            // Because these values are guaranteed to be a permutation
            //   vector, we can use an address sort.
            for (u = 0; u < aplRankCmp; u++)
                lpAxisStart[aplRankCmp + lpAxisStart[u]] = u;

        if (lphGlbAxis)
        {
            // We no longer need this ptr
            MyGlobalUnlock (*lphGlbAxis); lpAxisStart = lpAxisHead = NULL;
        } // End IF
    } // End IF/ELSE

    return bRet;
} // End CheckAxis_EM
#undef  APPEND_NAME


//***************************************************************************
//  $TestDupAxis
//
//  Test an axis value for duplicates
//***************************************************************************

BOOL TestDupAxis
    (LPVOID  lpDup,
     APLRANK aplRank,
     BOOL    bAllowDups)

{
    BOOL bRet = TRUE;
    UINT uBitMask;

    // Calculate the bit mask
    uBitMask = (BIT0 << (UINT) (aplRank % NBIB));

    // See if this value has already been seen
    if (!bAllowDups)
        bRet = !(uBitMask & ((LPAPLBOOL) lpDup)[aplRank >> LOG2NBIB]);

    // Set this value for the next time if necessary
    if (bRet || bAllowDups)
        ((LPAPLBOOL) lpDup)[aplRank >> LOG2NBIB] |= uBitMask;

    return bRet;
} // End TestDupAxis


//***************************************************************************
//  End of File: axisfns.c
//***************************************************************************
