//***************************************************************************
//  NARS2000 -- Primitive Function -- Pi
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

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
#include "common.h"

#ifndef PROTO
PRIMSPEC PrimSpecPi =
{
    // Monadic functions
    NULL,   // &PrimFnMon_EM_YY,
    NULL,   // &PrimSpecPiStorageTypeMon,
    NULL,   // &PrimFnMonPiAPA_EM, -- Can't happen w/Pi

    NULL,   // &PrimFnMonPiBisB, -- Can't happen w/Pi
    NULL,   // &PrimFnMonPiBisI, -- Can't happen w/Pi
    NULL,   // &PrimFnMonPiBisF, -- Can't happen w/Pi

////               IisB,     // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonPiIisI, -- Can't happen w/Pi
    NULL,   // &PrimFnMonPiIisF, -- Can't happen w/Pi

////               FisB,     // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonPiFisI, -- Can't happen w/Pi
    NULL,   // &PrimFnMonPiFisF, -- Can't happen w/Pi

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecPiStorageTypeDyd,
    NULL,   // &PrimFnDydPiAPA_EM, -- Can't happen w/Pi

    NULL,   // &PrimFnDydPiBisBvB, -- Can't happen w/Pi
    NULL,   // &PrimFnDydPiBisIvI, -- Can't happen w/Pi
    NULL,   // &PrimFnDydPiBisFvF, -- Can't happen w/Pi
    NULL,   // &PrimFnDydPiBisCvC, -- Can't happen w/Pi

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydPiIisIvI,
    &PrimFnDydPiIisFvF,

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydPiFisIvI,
    NULL,   // &PrimFnDydPiFisFvF, -- Can't happen w/Pi
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecPi};
#endif


typedef enum tagNUMTHEORY
{
    NUMTHEORY_FACTOR = -1 ,         // -1:  Factor function
    NUMTHEORY_DIVCNT      ,         // 00:  Divisor count function
    NUMTHEORY_DIVSUM      ,         // 01:  Divisor sum function

    NUMTHEORY_ISPRIME     ,         // 02:  TRUE iff the given # is prime
    NUMTHEORY_NEXTPRIME   ,         // 03:  Next prime after a given #
    NUMTHEORY_PREVPRIME   ,         // 04:  Prev prime before a given #
    NUMTHEORY_NTHPRIME    ,         // 05:  Nth prime
    NUMTHEORY_NUMPRIMES   ,         // 06:  # primes <= a given #

    NUMTHEORY_MOBIUS      ,         // 07:  Mobius function
    NUMTHEORY_TOTIENT     ,         // 08:  Totient function
    NUMTHEORY_LENGTH                // 09:  # entries
} NUMTHEORY, *LPNUMTHEORY;


typedef union tagWORDSPLIT
{
    APLUINT aplInteger;
    UINT    words[2];
} WORDSPLIT;


//***************************************************************************
//  $PrimFnPi_EM_YY
//
//  Primitive function for monadic and dyadic Pi ("Prime Decomposition" and "Number Theoretic")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnPi_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnPi_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_PI);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonPi_EM_YY              (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnPi_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnPi_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Pi
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnPi_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnPi_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // If the left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return
          PrimProtoFnScalar_EM_YY (lptkLftArg,      // Ptr to left arg token
                                   lptkFunc,        // Ptr to function token
                                   lptkRhtArg,      // Ptr to right arg token
                                   lptkAxis);       // Ptr to axis token (may be NULL)
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************

        // Convert to a prototype
        return
          PrimProtoFnMixed_EM_YY (&PrimFnPi_EM_YY,  // Ptr to primitive function routine
                                   lptkLftArg,      // Ptr to left arg token
                                   lptkFunc,        // Ptr to function token
                                   lptkRhtArg,      // Ptr to right arg token
                                   lptkAxis);       // Ptr to axis token (may be NULL)
} // End PrimProtoFnPi_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonPi_EM_YY
//
//  Primitive function for monadic Pi ("Prime Decomposition")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonPi_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonPi_EM_YY
    (LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeRht;       // Right arg storage type
    APLNELM           aplNELMRht,       // Right arg NELM
                      aplNELMRes,       // Result    ...
                      aplNELMMax;       // Maximum # elements in the result
    APLRANK           aplRankRht;       // Right arg rank
    HGLOBAL           hGlbRht,          // Right arg global memory handle
                      hGlbRes = NULL;   // Result global memory handle
    LPAPLINT          lpMemRes = NULL;  // Prt to result global memory
    LPVARARRAY_HEADER lpMemHdrRes;      // Ptr to result global memory header
    APLLONGEST        aplLongestRht;    // Right arg immediate value
    APLINT            aplIntegerRht;    // Right arg integer value
    UBOOL             bRet;             // TRUE iff the result is valid
    APLUINT           ByteRes;          // # bytes in the result
    LPPL_YYSTYPE      lpYYRes;          // Ptr to result
    LPPLLOCALVARS     lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;     // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, Rank)
    //   of the arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for RIGHT LENGTH ERROR
    if (!IsSingleton (aplNELMRht))
        goto RIGHT_LENGTH_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if (!IsSimpleNum (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Get right arg's global ptr
    aplLongestRht = GetGlbPtrs (lptkRhtArg, &hGlbRht);

    // Attempt to convert FLOAT right arg
    if (IsSimpleFlt (aplTypeRht))
    {
        // Attempt to convert the float to an integer using System CT
        aplIntegerRht = FloatToAplint_SCT ((APLFLOAT) aplLongestRht, &bRet);
        if (!bRet)
            goto RIGHT_DOMAIN_EXIT;
    } else
        aplIntegerRht = aplLongestRht;

    // Check the singleton arg
    if (aplIntegerRht <= 0)
        goto RIGHT_DOMAIN_EXIT;

    // Factor the given number

    // Guess at the # factors in the result
    aplNELMMax = 64;
    aplNELMRes = 0;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_INT, aplNELMMax, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result.
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = (LPVOID)
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
////lpHeader->NELM       = aplNELMRes;      // To be filled in later
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // Call common routine
    aplNELMRes =
      PrimFnPiCommon (lpMemRes,         // Ptr to result global memory (may be NULL)
                      NUMTHEORY_FACTOR, // Function index
                      aplIntegerRht,    // Value to factor
                      lpbCtrlBreak);    // Ptr to Ctrl-Break flag
    // Check for Ctrl-Break
    if (CheckCtrlBreak (*lpbCtrlBreak))
        goto ERROR_EXIT;

    // Save the actual NELM
    lpMemHdrRes->NELM = aplNELMRes;

    // Skip over the header to the dimension
    lpMemRes = VarArrayBaseToDim (lpMemHdrRes);

    // Save the actual dimension
    lpMemRes[0] = aplNELMRes;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_INT, aplNELMRes, 1);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Reallocate the array downwards
    hGlbRes = MyGlobalReAlloc (hGlbRes, (APLU3264) ByteRes, GMEM_MOVEABLE);

    // Allocate a new YYRes;
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

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
                               lptkRhtArg);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF

    return NULL;
} // End PrimFnMonPi_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecPiStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecPiStorageTypeDyd
    (APLNELM    aplNELMLft,
     LPAPLSTYPE lpaplTypeLft,
     LPTOKEN    lptkFunc,
     APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht)

{
    APLSTYPE aplTypeRes;

    // In case the left arg is an empty char,
    //   change its type to BOOL
    if (IsEmpty (aplNELMLft) && IsSimpleChar (*lpaplTypeLft))
        *lpaplTypeLft = ARRAY_BOOL;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsEmpty (aplNELMRht) && IsSimpleChar (*lpaplTypeRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // Calculate the storage type of the result
    aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // No Boolean or Integer results for Pi
    if (IsSimpleNum (aplTypeRes))
        aplTypeRes = ARRAY_INT;

    return aplTypeRes;
} // End PrimSpecPiStorageTypeDyd


//***************************************************************************
//  $PrimFnDydPiIisIvI
//
//  Primitive scalar function dyadic Pi:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydPiIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT        aplIntegerRes;        // The integer result
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check the singleton right arg
    if (aplIntegerRht <= 0)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // Split cases based upon the left arg
    switch (aplIntegerLft)
    {
        case NUMTHEORY_DIVCNT:          // Divisor function (count of divisors of N)
        case NUMTHEORY_DIVSUM:          // Divisor function (sum of divisors of N)
        case NUMTHEORY_MOBIUS:          // Mobius function
        case NUMTHEORY_TOTIENT:         // Totient function (# positive integers coprime to N)
            // Call common routine
            aplIntegerRes =
              PrimFnPiCommon (NULL,             // Ptr to result global memory (may be NULL)
                              aplIntegerLft,    // Function index
                              aplIntegerRht,    // Value to factor
                              lpbCtrlBreak);    // Ptr to Ctrl-Break flag
            break;

        case NUMTHEORY_ISPRIME:         // TRUE iff the given # is prime
            return PrimFnPiIsPrime   (aplIntegerRht, lpbCtrlBreak);

        case NUMTHEORY_NEXTPRIME:       // Next prime after a given #
            return PrimFnPiNextPrime (aplIntegerRht, lpbCtrlBreak);

        case NUMTHEORY_PREVPRIME:       // Prev prime before a given #
            return PrimFnPiPrevPrime (aplIntegerRht, lpbCtrlBreak);

        case NUMTHEORY_NTHPRIME:        // Nth prime
            return PrimFnPiNthPrime  (aplIntegerRht, lpbCtrlBreak);

        case NUMTHEORY_NUMPRIMES:       // # primes <= a given #
            return PrimFnPiNumPrimes (aplIntegerRht, lpbCtrlBreak);

        default:
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    } // End SWITCH

    return aplIntegerRes;
} // End PrimFnDydPiIisIvI


//***************************************************************************
//  $PrimFnDydPiIisFvF
//
//  Primitive scalar function dyadic Pi:  I {is} F fn F
//***************************************************************************

APLINT PrimFnDydPiIisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT aplIntegerLft,
           aplIntegerRht;
    UBOOL  bRet;

    // Attempt to convert the float to an integer using System CT
    aplIntegerLft = FloatToAplint_SCT (aplFloatLft, &bRet);
    if (!bRet)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // Attempt to convert the float to an integer using System CT
    aplIntegerRht = FloatToAplint_SCT (aplFloatRht, &bRet);
    if (!bRet)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return PrimFnDydPiIisIvI (aplIntegerLft, aplIntegerRht, lpPrimSpec);
} // End PrimFnDydPiIisFvF


//***************************************************************************
//  $PrimFnDydPiFisIvI
//
//  Primitive scalar function dyadic Pi:  F {is} I fn I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnPiFnDydPiFisIvI"
#else
#define APPEND_NAME
#endif

APLFLOAT PrimFnDydPiFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return 0.0;
} // End PrimFnDydPiFisIvI
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnPiCommon
//
//  Common primitive function for monadic/dyadic Pi ("Prime Decomposition")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnPiCommon"
#else
#define APPEND_NAME
#endif

APLNELM PrimFnPiCommon
    (LPAPLUINT lpMemRes,            // Ptr to result global memory (may be NULL)
     APLUINT   uFcnIndex,           // Function index (see enum NUMTHEORY)
     APLINT    aplIntegerRht,       // Value to factor
     LPUBOOL   lpbCtrlBreak)        // Ptr to Ctrl-Break flag

{
    APLUINT      uRes,              // Loop counter
                 uPrime,            // The current prime
                 uPrimeCnt;         // # occurrences of a specific prime
    APLNELM      aplNELMRes;        // Result NELM
    PROCESSPRIME procPrime;         // Struct for args to ProcessPrime

    // Initialize the function vars
    procPrime.uFcnIndex    = uFcnIndex;
    procPrime.bSquareFree  = TRUE;
    procPrime.uTotient     = aplIntegerRht;
    procPrime.uDivisor     = 1;
    procPrime.lpbCtrlBreak = lpbCtrlBreak;
    get_random_seeds (&procPrime.seed1, &procPrime.seed2);

    // Run through the list of small primes
    // This handles all numbers < PRECOMPUTED_PRIME_NEXT2 = ~1e10 (~33 bits)
    for (aplNELMRes = uPrime = uRes = 0;
         aplIntegerRht NE 1
      && uRes < PRECOMPUTED_NUM_PRIMES;
         uRes++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Initialize the prime count and the prime
        uPrimeCnt = 0;
        uPrime += prime_delta[uRes];

        // While this prime is a factor, ...
        while (0 EQ (aplIntegerRht % uPrime))
        {
            // If we're saving the values, ...
            if (lpMemRes)
                // Save in the result
                *lpMemRes++ = uPrime;

            // Divide it out
            aplIntegerRht /= uPrime;

            // Count it in
            aplNELMRes++;
            uPrimeCnt++;
        } // End WHILE

        // If this prime divides N, ...
        if (uPrimeCnt)
            // Process this prime factor
            ProcessPrime (uPrime, uPrimeCnt, &procPrime);
    } // End FOR

    // Check against the smallest # not factorable by trial division
    if (1 < aplIntegerRht
     &&     aplIntegerRht < PRECOMPUTED_PRIME_NEXT2)    // = 100003L*100003L
    {
        // If we're saving the values, ...
        if (lpMemRes)
            // Save in the result
            *lpMemRes++ = aplIntegerRht;

         // Count it in
         aplNELMRes++;

        // Process this prime factor
        ProcessPrime (aplIntegerRht, 1, &procPrime);
    } else
    // Loop until we have no more factors
    while (aplIntegerRht > 1)
    {
        mp_t mpNumber;                      // Number to factor

        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Save the integer into mpNumber
        mp_set (&mpNumber, aplIntegerRht);

        // Try to factor aplIntegerRht using various methods
        uPrime = PrimeFactor (&mpNumber, &procPrime);

        // Initialize the prime count
        uPrimeCnt = 0;
        while (0 EQ (aplIntegerRht % uPrime))
        {
            // If we're saving the values, ...
            if (lpMemRes)
                // Save in the result
                *lpMemRes++ = uPrime;

            // Divide it out
            aplIntegerRht /= uPrime;

            // Count it in
            aplNELMRes++;
            uPrimeCnt++;
        } // End WHILE

        // Process this prime factor
        ProcessPrime (uPrime, uPrimeCnt, &procPrime);
    } // End WHILE

    // Split cases based upon the function index
    switch (uFcnIndex)
    {
        case NUMTHEORY_FACTOR:
            return aplNELMRes;

        case NUMTHEORY_DIVCNT:
        case NUMTHEORY_DIVSUM:
                return procPrime.uDivisor;

        case NUMTHEORY_MOBIUS:
            // If the prime factors are all unique, ...
            if (procPrime.bSquareFree)
                // Return (-1) * (the parity of the # factors)
                return (aplNELMRes & BIT0) ? -1 : 1;
            else
                // The prime factors are not all unique
                return 0;

        case NUMTHEORY_TOTIENT:
                return procPrime.uTotient;

        case NUMTHEORY_ISPRIME:
        case NUMTHEORY_NEXTPRIME:
        case NUMTHEORY_PREVPRIME:
        case NUMTHEORY_NTHPRIME:
        case NUMTHEORY_NUMPRIMES:
        defstop
            break;
    } // End SWITCH
ERROR_EXIT:
    return -2;
} // End PrimFnPiCommon
#undef  APPEND_NAME


//***************************************************************************
//  $PrimeFactor
//
//  Return either a prime factor of a given #
//    or the number itself if it's prime
//***************************************************************************

APLUINT PrimeFactor
    (LPVOID         mpNumber,           // The number to factor (really a mp_t*)
     LPPROCESSPRIME lpProcPrime)        // Ptr to PROCESSPRIME struc

{
    mp_t    mpFactor1,                  // Factor #1
            mpFactor2;                  // ...     2
    APLUINT uVal;                       // Temporary value
    UINT    bits;                       // # bits in the value
#ifdef DEBUG
    mp_t *mpValue;

    mpValue = mpNumber;
#else
  #define mpValue   ((mp_t *) mpNumber)
#endif

    bits = mp_bits (mpValue);

    if (bits <= SMALL_COMPOSITE_CUTOFF_BITS)
    {
        mp_clear (&mpFactor1);
        mp_clear (&mpFactor2);

        if (bits <= PRECOMPUTED_PRIME_NEXT2_LOG2)       // = 33
            return mp_get (mpValue);
        else
        if (bits <= 60)
        {
            uVal = squfof (mpValue, lpProcPrime->lpbCtrlBreak);

            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpProcPrime->lpbCtrlBreak))
                goto ERROR_EXIT;

            switch (uVal)
            {
                case 0:
                case 1:
                    // Try Pollard's Rho
                    if (rho (mpValue, &mpFactor1, lpProcPrime))
                        return PrimeFactor (&mpFactor1, lpProcPrime);
                    else
                        uVal = tinyqs (mpValue, &mpFactor1, &mpFactor2, lpProcPrime->lpbCtrlBreak);
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpProcPrime->lpbCtrlBreak))
                        goto ERROR_EXIT;

                    break;

                default:
                    // If the value is within trial division, ...
                    if (uVal < PRECOMPUTED_PRIME_NEXT2)
                        return uVal;

                    mpFactor1.nwords = 1;
                    mpFactor1.val[0] = (uint32) uVal;

                    return PrimeFactor (&mpFactor1, lpProcPrime);
            } // End SWITCH
        } else
            uVal = tinyqs (mpValue, &mpFactor1, &mpFactor2, lpProcPrime->lpbCtrlBreak);

        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpProcPrime->lpbCtrlBreak))
            goto ERROR_EXIT;

        if (uVal)
        {
            if (mp_is_one (&mpFactor1))
                return mp_get (&mpFactor2);
            else
            if (mp_is_one (&mpFactor2))
                return mp_get (&mpFactor1);
            else
                // Try again with the smaller # so as to try to return factors in ascending order
                return PrimeFactor ((mp_cmp (&mpFactor1, &mpFactor2) < 0) ? &mpFactor1 : &mpFactor2, lpProcPrime);
        } // End IF
    } // End IF

    return mp_get (mpValue);

ERROR_EXIT:
    return 0;
#ifndef DEBUG
  #undef  mpValue
#endif
} // End PrimeFactor


//***************************************************************************
//  $ProcessPrime
//
//  Process a prime according to the function index
//***************************************************************************

void ProcessPrime
    (APLUINT        uPrime,         // The prime to process
     APLUINT        uPrimeCnt,      // The prime exponent
     LPPROCESSPRIME lpProcPrime)    // Ptr to PROCESSPRIME struc

{
    APLUINT uAccum;                 // Product accumulator

    // Split cases based upon the function index
    switch (lpProcPrime->uFcnIndex)
    {
        case NUMTHEORY_FACTOR:
            break;

        case NUMTHEORY_MOBIUS:
            // Accumulate the square-free state
            lpProcPrime->bSquareFree &= (uPrimeCnt <= 1);

            break;

        case NUMTHEORY_TOTIENT:
            // If this prime divides N, ...
            if (uPrimeCnt)
                lpProcPrime->uTotient -= lpProcPrime->uTotient / uPrime;
            break;

        case NUMTHEORY_DIVCNT:
            // If this prime divides N, ...
            if (uPrimeCnt)
                lpProcPrime->uDivisor *= (uPrimeCnt + 1);
            break;

        case NUMTHEORY_DIVSUM:
            // If this prime divides N, ...
            if (uPrimeCnt++)
            {
                // Initialize the product accumulator
                uAccum = 1;

                while (uPrimeCnt--)
                    uAccum = imul64 (uAccum, uPrime);   // uAccum = exp (p, uPrimeCnt + 1)
                uAccum = (uAccum - 1) / (uPrime - 1);   // uAccum = exp (p, uPrimeCnt + 1) / (p - 1)

                lpProcPrime->uDivisor *= uAccum;
            } // End IF

            break;

        case NUMTHEORY_ISPRIME:
        case NUMTHEORY_NEXTPRIME:
        case NUMTHEORY_PREVPRIME:
        case NUMTHEORY_NTHPRIME:
        case NUMTHEORY_NUMPRIMES:
        defstop
            break;
    } // End SWITCH
} // End ProcessPrime


//***************************************************************************
//  $PrimFnPiIsPrime
//
//  Return TRUE or FALSE for whether or not a given # is a prime
//***************************************************************************

UBOOL PrimFnPiIsPrime
    (APLINT  aplInteger,            // The number to check
     LPUBOOL lpbCtrlBreak)          // Ptr to Ctrl-Break flag

{
    uint32 seed1,                   // Random seeds
           seed2;                   // ...
    mp_t   mpValue;                 // Temporary MP value

    // Handle special case
    if (aplInteger EQ 1)
        return FALSE;

    // Initialize the seeds
    get_random_seeds (&seed1, &seed2);

    // Initialize the struc
    mp_set (&mpValue, aplInteger);

    return mp_is_prime (&mpValue, &seed1, &seed2, lpbCtrlBreak);
} // End PrimFnPiIsPrime


//***************************************************************************
//  $PrimFnPiNextPrime
//
//  Return the next prime after than a given #
//***************************************************************************

APLINT PrimFnPiNextPrime
    (APLINT  aplInteger,            // The number to check
     LPUBOOL lpbCtrlBreak)          // Ptr to Ctrl-Break flag

{
    uint32 seed1,                   // Random seeds
           seed2;                   // ...
    mp_t   mpValue;                 // Temporary MP value

    // Handle special cases
    if (aplInteger <= 2)
        return aplInteger + 1;

    // Initialize the seeds
    get_random_seeds (&seed1, &seed2);

    // Initialize the struc
    mp_set (&mpValue, aplInteger);

    return iadd64 (aplInteger, mp_next_prime (&mpValue, &mpValue, &seed1, &seed2, lpbCtrlBreak));
} // End PrimFnPiNextPrime


//***************************************************************************
//  $PrimFnPiPrevPrime
//
//  Return the Prev prime before a given #
//***************************************************************************

APLINT PrimFnPiPrevPrime
    (APLINT  aplInteger,            // The number to check
     LPUBOOL lpbCtrlBreak)          // Ptr to Ctrl-Break flag

{
    uint32 seed1,                   // Random seeds
           seed2;                   // ...
    mp_t   mpValue;                 // Temporary MP value

    // Handle special cases
    if (aplInteger <= 2)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    else
    if (aplInteger EQ 3)
        return 2;

    // Initialize the seeds
    get_random_seeds (&seed1, &seed2);

    // Initialize the struc
    mp_set (&mpValue, aplInteger);

    return isub64 (aplInteger, mp_prev_prime (&mpValue, &mpValue, &seed1, &seed2, lpbCtrlBreak));
} // End PrimFnPiPrevPrime


//***************************************************************************
//  $PrimFnPiNthPrime
//
//  Return the Nth prime
//***************************************************************************

APLINT PrimFnPiNthPrime
    (APLINT  aplInteger,            // The number to check
     LPUBOOL lpbCtrlBreak)          // Ptr to Ctrl-Break flag

{
    APLUINT uPrime;                 // The result
    UINT    uRes = 0;               // Loop counter
    uint32  seed1,                  // Random seeds
            seed2;                  // ...
    mp_t    mpValue;                // Temporary MP value

    // Handle special cases
    // The following answers were obtained from
    //   http://oeis.org/A006988/
    // Split cases based upon the value
    switch (aplInteger)
    {
        case 10000:                 // 1E4
            return 104729;

        case 100000:                // 1E5
            return 1299709;

        case 1000000:               // 1E6
            return 15485863;

        case 10000000:              // 1E7
            return 179424673;

        case 100000000:             // 1E8
            return 2038074743;

        case 1000000000:            // 1E9
            return 22801763489;

        case 10000000000:           // 1E10
            return 252097800623;

        case 100000000000:          // 1E11
            return 2760727302517;

        case 1000000000000:         // 1E12
            return 29996224275833;

        case 10000000000000:        // 1E13
            return 323780508946331;

        case 100000000000000:       // 1E14
            return 3475385758524527;

        case 1000000000000000:      // 1E15
            return 37124508045065437;

        case 10000000000000000:     // 1E16
            return 394906913903735329;

        case 100000000000000000:    // 1E17
            return 4185296581467695669;

////////case 1000000000000000000:   // 1E18
////////    return 44211790234832169331;        // Constant too big
    } // End SWITCH

    // Check the list of small primes
    if (aplInteger <= PRECOMPUTED_NUM_PRIMES)
    {
        uPrime = 0;
        while (aplInteger--)
            uPrime += prime_delta[uRes++];
        return uPrime;
    } else
    {
        // Initialize
        get_random_seeds (&seed1, &seed2);
        uPrime      = PRECOMPUTED_PRIME_MAX;
        aplInteger -= PRECOMPUTED_NUM_PRIMES;

        while (aplInteger--)
        {
            // Save the current prime into mpValue
            mp_set (&mpValue, uPrime);

            // Calculate the next prime
            uPrime =
              iadd64 (uPrime, mp_next_prime (&mpValue, &mpValue, &seed1, &seed2, lpbCtrlBreak));

            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;
        } // End WHILE

        return uPrime;
    } // End IF/ELSE

ERROR_EXIT:
    return 0;
} // End PrimFnPiNthPrime


//***************************************************************************
//  $PrimFnPiNumPrimes
//
//  Return # primes <= a given #
//***************************************************************************

APLINT PrimFnPiNumPrimes
    (APLINT  aplInteger,            // The number to check
     LPUBOOL lpbCtrlBreak)          // Ptr to Ctrl-Break flag

{
    uint32 seed1,                   // Random seeds
           seed2;                   // ...
    mp_t   mpValue;                 // Temporary MP value
    APLUINT uRes;                   // Loop counter
    APLINT  uPrime;                 // The current prime
    UBOOL   bRet;                   // TRUE iff the result is valid

    // Handle special cases
    // The following answers were obtained from
    //   http://oeis.org/A006880
    // Split cases based upon the value
    switch (aplInteger)
    {
        case 10000:                 // 1E4
            return 1229;

        case 100000:                // 1E5
            return 9592;

        case 1000000:               // 1E6
            return 78498;

        case 10000000:              // 1E7
            return 664579;

        case 100000000:             // 1E8
            return 5761455;

        case 1000000000:            // 1E9
            return 50847534;

        case 10000000000:           // 1E10
            return 455052511;

        case 100000000000:          // 1E11
            return 4118054813;

        case 1000000000000:         // 1E12
            return 37607912018;

        case 10000000000000:        // 1E13
            return 346065536839;

        case 100000000000000:       // 1E14
            return 3204941750802;

        case 1000000000000000:      // 1E15
            return 29844570422669;

        case 10000000000000000:     // 1E16
            return 279238341033925;

        case 100000000000000000:    // 1E17
            return 2623557157654233;

        case 1000000000000000000:   // 1E18
            return 24739954287740860;

        case 10000000000000000000:  // 1E19
            return 234057667276344607;

////////case 100000000000000000000: // 1E20     // Constant too big
////////    return 2220819602560918840;
    } // End SWITCH

    // Run through the list of small primes
    for (uPrime = uRes = 0;
         uRes < PRECOMPUTED_NUM_PRIMES;
         uRes++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Calculate the next prime
        uPrime += prime_delta[uRes];

        // Check the prime
        if (uPrime > aplInteger)
            return uRes;
    } // End FOR

    // Initialize the seeds
    get_random_seeds (&seed1, &seed2);

    // Loop through mp_next_prime
    while (TRUE)
    {
        // Initialize the struc
        mp_set (&mpValue, uPrime);

        // Calculate the next prime
        uPrime =
          _iadd64 (uPrime, mp_next_prime (&mpValue, &mpValue, &seed1, &seed2, lpbCtrlBreak), &bRet);

        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Check the prime
        if (!bRet || uPrime > aplInteger)
            return uRes;
        // Count in another prime
        uRes++;
    } // End WHILE

    return uRes;
ERROR_EXIT:
    return -2;
} // End PrimFnPiNumPrimes


//***************************************************************************
//  $mp_set
//
//  Initialize and set a number into a mp_t struc
//***************************************************************************

void mp_set
    (LPVOID mpNumber,               // The output (really a mp_t*)
     APLINT aplInteger)             // The input

{
    UINT uVal;                      // Loop counter
#ifdef DEBUG
    mp_t *mpValue;

    mpValue = mpNumber;
#else
  #define mpValue   ((mp_t *) mpNumber)
#endif

    // Initialize the struc
    mp_clear (mpValue);

    for (uVal = 0; aplInteger; aplInteger >>= 32, mpValue->nwords++, uVal++)
        mpValue->val[uVal] = (UINT) aplInteger;
#ifndef DEBUG
  #undef  mpValue
#endif
} // End mp_set


//***************************************************************************
//  $mp_get
//
//  Convert a mp_t number to a 64-bit integer
//***************************************************************************

APLUINT mp_get
    (LPVOID mpNumber)               // The number to convert (really a mp_t*)

{
    WORDSPLIT wSplit;               // Union for the result
#ifdef DEBUG
    mp_t *mpValue;

    mpValue = mpNumber;
#else
  #define mpValue   ((mp_t *) mpNumber)
#endif

    // Save the words in the result
    wSplit.words[0] = mpValue->val[0];
    wSplit.words[1] = mpValue->val[1];

    return wSplit.aplInteger;
#ifndef DEBUG
  #undef  mpValue
#endif
} // End mp_get


//***************************************************************************
//  $get_random_seeds
//***************************************************************************

void get_random_seeds
    (UINT *seed1,
     UINT *seed2)

{
    uint32 tmp_seed1, tmp_seed2;
    uint64 high_res_time;

    /* In a multithreaded program, every msieve object
       should have two unique, non-correlated seeds
       chosen for it */

    /* <Shrug> For everyone else, sample the current time,
       the high-res timer (hopefully not correlated to the
       current time), and the process ID. Multithreaded
       applications should fold in the thread ID too */

    high_res_time = read_clock ();
    tmp_seed1 = ((uint32) (high_res_time >> 32) ^
                 (uint32) time (NULL)) *
                 (uint32) getpid ();
    tmp_seed2 = (uint32) high_res_time;

    /* The final seeds are the result of a multiplicative
       hash of the initial seeds */

    *seed1 = tmp_seed1 * ((uint32)40499 * 65543);
    *seed2 = tmp_seed2 * ((uint32)40499 * 65543);
} // End get_random_seeds


//***************************************************************************
//  $read_clock
//***************************************************************************

uint64 read_clock
    (void)

{
    LARGE_INTEGER ret;
    QueryPerformanceCounter (&ret);
    return ret.QuadPart;
} // End read_clock


//***************************************************************************
//  End of File: pf_pi.c
//***************************************************************************
