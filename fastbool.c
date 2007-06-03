//***************************************************************************
//  NARS2000 -- Fast Booean Reduction and Scan Functions
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $FastBoolNonce
//***************************************************************************

void FastBoolNonce
    (APLSTYPE  aplTYpeRht,
     LPVOID    lpMemRht,
     LPVOID    lpMemRes,
     APLUINT   uDimLo,
     APLUINT   uDimAxRht,
     APLUINT   uDimAxRes,
     LPYYSTYPE lpYYFcnStrOpr)

{
    DbgBrk ();      // ***FINISHME***
} // End FastBoolNonce


FASTBOOLFCN FastBoolRedAnd;
#define FastBoolRedOr           FastBoolNonce
#define FastBoolRedNor          FastBoolNonce
#define FastBoolRedEqual        FastBoolNonce
#define FastBoolRedLess         FastBoolNonce
#define FastBoolRedLessEq       FastBoolNonce
#define FastBoolRedNotEq        FastBoolNonce
FASTBOOLFCN FastBoolRedPlus;
#define FastBoolRedMore         FastBoolNonce
#define FastBoolRedMoreEq       FastBoolNonce
#define FastBoolRedNand         FastBoolNonce

#define FastBoolScanAnd         FastBoolNonce
#define FastBoolScanOr          FastBoolNonce
#define FastBoolScanNor         FastBoolNonce
#define FastBoolScanEqual       FastBoolNonce
#define FastBoolScanLess        FastBoolNonce
#define FastBoolScanLessEq      FastBoolNonce
#define FastBoolScanNotEq       FastBoolNonce
#define FastBoolScanMore        FastBoolNonce
#define FastBoolScanMoreEq      FastBoolNonce
#define FastBoolScanNand        FastBoolNonce

FASTBOOLFNS FastBoolFns[] =                     // In the same order as enum FBFNS
{{ NULL               ,  NULL              },   // 00 = No entry so we can catch this as an error
 {&FastBoolRedAnd     , &FastBoolScanAnd   },   // 01 = And
 {&FastBoolRedOr      , &FastBoolScanOr    },   // 02 = Or
 {&FastBoolRedNand    , &FastBoolScanNand  },   // 03 = Nand
 {&FastBoolRedNor     , &FastBoolScanNor   },   // 04 = Nor
 {&FastBoolRedLess    , &FastBoolScanLess  },   // 05 = Less
 {&FastBoolRedLessEq  , &FastBoolScanLessEq},   // 06 = Less or equal
 {&FastBoolRedEqual   , &FastBoolScanEqual },   // 07 = Equal
 {&FastBoolRedMore    , &FastBoolScanMore  },   // 08 = More
 {&FastBoolRedMoreEq  , &FastBoolScanMoreEq},   // 09 = More or equal
 {&FastBoolRedNotEq   , &FastBoolScanNotEq },   // 0A = Not equal
 {&FastBoolRedAnd     , &FastBoolScanAnd   },   // 0B = Min
 {&FastBoolRedOr      , &FastBoolScanOr    },   // 0C = Max
 {&FastBoolRedPlus    ,  NULL              },   // 0D = Plus
 { NULL               ,  NULL              },   // 0E = Minus
 { NULL               ,  NULL              },   // 0F = Divide
};


//***************************************************************************
//  $FastBoolRedPlus
//
//  Fast Boolean "Plus" Reduction
//***************************************************************************

void FastBoolRedPlus
    (APLSTYPE  aplTypeRht,          // Right arg storage type
     LPVOID    lpMemRht,            // Ptr to right arg memory
     LPVOID    lpMemRes,            // Ptr to result    memory
     APLUINT   uDimLo,              // Product of dimensions below axis
     APLUINT   uDimAxRht,           // Length of right arg axis dimension
     APLUINT   uDimAxRes,           // ...       result    ...
     LPYYSTYPE lpYYFcnStrOpr)       // Ptr to operator function strand

{
    APLUINT   uLo,
              uDimRht,
              uBitCount = 0,
              uAccum;
    UCHAR     uByteMask,
              uByteMaskIni,
              uBitsInMask;
    LPAPLBOOL lpaplBool;

    // If the right arg is an APA, handle it specially
    if (aplTypeRht EQ ARRAY_APA)
    {
        // The possibilities are as follows:
        //   1.  N   {rho}0   or 1
        //   2.  N 2 {rho}0 1 or 1 0 as an APA
        //       (presumably coming from N 2{rho}{iota}2 in origin-0
        //        and which was type demoted to Boolean -- as far as
        //        I know, this doesn't happen, but you never know)
        // The difference between the above two cases is in the APA
        //   multiplier -- in the first case it's zero, and in the
        //   second case it's either 1 or -1.

        // Check the APA multiplier
#define lpAPA       ((LPAPLAPA) lpMemRht)
        if (lpAPA->Mul NE 0)
            *((LPAPLINT) lpMemRes) = 1;             // That's +/0 1 or +/1 0
        else
        if (lpAPA->Off)                             // If it's non-zero (i.e., 1), ...
        for (uLo = 0; uLo < uDimLo; uLo++)
            *((LPAPLINT) lpMemRes)++ = lpAPA->Len;  // That's +/N{rho}1
#undef  lpAPA
        return;         // We're done
    } // End IF

    // Calculate the mask for the first byte
    uByteMaskIni = (UCHAR) ((1 << min (uDimAxRht, 8)) - 1);

    // Get ptr to first byte
    lpaplBool = (LPAPLBOOL) lpMemRht;

    // Loop through the right arg calling the
    //   function strand between data, storing in the
    //   result
    for (uLo = 0; uLo < uDimLo; uLo++)
#define uDimHi      1       // This is a requirement for all Fast Boolean operations
////for (uHi = 0; uHi < uDimHi; uHi++)      // uDimHi EQ 1, so this loop is unnecessary
#define uHi         0       // A consequence of removing the above loop
    {
        // Calculate the starting index in the right arg/result of this vector
        uDimRht = uLo * uDimHi * uDimAxRht + uHi;

        // Calculate the byte mask
        uByteMask = (UCHAR) (uByteMaskIni << (uDimRht & MASKLOG2NBIB));

        // Get +/ on first byte or tail thereof
        uAccum = FastBoolRed[uByteMask & *lpaplBool];

        // Update the # bits processed so far (modulo NBIB)
        uBitsInMask = FastBoolRed[uByteMask];
        uBitCount += uBitsInMask;
        lpaplBool += (uBitCount >= NBIB);
        uBitCount &= MASKLOG2NBIB;

        // Loop through the elements along the specified axis
        //   in chunks of bytes
        while (uDimAxRht > (uBitsInMask + NBIB))
        {
            uAccum += FastBoolRed[*lpaplBool++];
            uBitsInMask += NBIB;
        } // End IF

        // If there are more bits in this axis, ...
        if (uDimAxRht > uBitsInMask)
        {
            uByteMask = (UCHAR) ((1 << (uDimAxRht - uBitsInMask)) - 1);
            uAccum += FastBoolRed[uByteMask & *lpaplBool];

            // Update the # bits processed so far (modulo NBIB)
            uBitCount += FastBoolRed[uByteMask];
            lpaplBool += (uBitCount >= NBIB);
            uBitCount &= MASKLOG2NBIB;
        } // End IF

        // Save in the result
        *((LPAPLINT) lpMemRes)++ = uAccum;
    } // End FOR
#undef  uHi
#undef  uDimHi
} // End FastBoolRedPlus


//***************************************************************************
//  $FastBoolRedAnd
//
//  Fast Boolean "And" Reduction
//***************************************************************************

void FastBoolRedAnd
    (APLSTYPE  aplTypeRht,          // Right arg storage type
     LPVOID    lpMemRht,            // Ptr to right arg memory
     LPVOID    lpMemRes,            // Ptr to result    memory
     APLUINT   uDimLo,              // Product of dimensions below axis
     APLUINT   uDimAxRht,           // Length of right arg axis dimension
     APLUINT   uDimAxRes,           // ...       result    ...
     LPYYSTYPE lpYYFcnStrOpr)       // Ptr to operator function strand

{
    APLUINT   uLo,
              uDimRht,
              uDimRes,
              uBitCount = 0,
              uAccum;
    UCHAR     uByteMask,
              uByteMaskIni,
              uBitsInMask;
    LPAPLBOOL lpaplBool;

    // Calculate the mask for the first byte
    uByteMaskIni = (UCHAR) ((1 << min (uDimAxRht, 8)) - 1);

    // Get ptr to first byte
    lpaplBool = (LPAPLBOOL) lpMemRht;

    // Loop through the right arg calling the
    //   function strand between data, storing in the
    //   result
    for (uLo = 0; uLo < uDimLo; uLo++)
#define uDimHi      1       // This is a requirement for all Fast Boolean operations
////for (uHi = 0; uHi < uDimHi; uHi++)      // uDimHi EQ 1, so this loop is unnecessary
#define uHi         0       // A consequence of removing the above loop
    {
        // Calculate the starting index in the right arg/result of this vector
        uDimRht = uLo * uDimHi * uDimAxRht + uHi;
        uDimRes = uLo * uDimHi * 1         + uHi;

        // Calculate the byte mask
        uByteMask = (UCHAR) (uByteMaskIni << (uDimRht & MASKLOG2NBIB));
        uBitsInMask = FastBoolRed[uByteMask];

        // Get reduction on first byte or tail thereof
        uAccum = (uBitsInMask EQ FastBoolRed[uByteMask & *lpaplBool]);

        // If we're not done, ...
        if (uAccum)
        {
            // Update the # bits processed so far (modulo NBIB)
            uBitCount += uBitsInMask;
            lpaplBool += (uBitCount >= NBIB);
            uBitCount &= MASKLOG2NBIB;

            // Loop through the elements along the specified axis
            //   in chunks of bytes
            while (uAccum && uDimAxRht > (uBitsInMask + NBIB))
            {
                uAccum = (NBIB EQ FastBoolRed[*lpaplBool++]);
                uBitsInMask += NBIB;
            } // End IF

            // If there are more bits in this axis, ...
            if (uDimAxRht > uBitsInMask)
            {
                uByteMask = (UCHAR) ((1 << (uDimAxRht - uBitsInMask)) - 1);
                uAccum = (uBitsInMask EQ FastBoolRed[uByteMask & *lpaplBool]);

                // Update the # bits processed so far (modulo NBIB)
                uBitCount += FastBoolRed[uByteMask];
                lpaplBool += (uBitCount >= NBIB);
                uBitCount &= MASKLOG2NBIB;
            } // End IF
        } // End IF

        // Check to see if we need to skip to the next result byte
        uDimRes &= MASKLOG2NBIB;
        if (uDimRes EQ 0)
            ((LPAPLBOOL) lpMemRes)++;

        // Save in the result
        *((LPAPLBOOL) lpMemRes) |= uAccum << uDimRes;
    } // End FOR
#undef  uHi
#undef  uDimHi
} // End FastBoolRedAnd


//***************************************************************************
//  End of File: fastbool.c
//***************************************************************************
