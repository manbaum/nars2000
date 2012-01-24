//***************************************************************************
//  NARS2000 -- Primitive Function -- Pi
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2012 Sudley Place Software

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

    NULL,   // &PrimFnMonPiRisR, -- Can't happen w/Pi

////               VisR,    // Handled via type promotion (to VisV)
    NULL,   // &PrimFnMonPiVisV, -- Can't happen w/Pi

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

    NULL,   // &PrimFnDydPiBisRvR, -- Can't happen w/Pi
    &PrimFnDydPiRisRvR,

    NULL,   // &PrimFnDydPiBisVvV, -- Can't happen w/Pi
////                 VisRvR     // Handled via type promotion (to VisVvV)
    &PrimFnDydPiVisVvV,
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


#define INIT_FACTOR_CNT     100
#define INIT_FACTOR_INC     100


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
    APLSTYPE          aplTypeRht,       // Right arg storage type
                      aplTypeRes;       // Result ...
    APLNELM           aplNELMRht,       // Right arg NELM
                      aplNELMRes = 0;   // Result    ...
    APLRANK           aplRankRht;       // Right arg rank
    HGLOBAL           hGlbRht,          // Right arg global memory handle
                      hGlbRes = NULL;   // Result global memory handle
    LPVOID            lpMemRes = NULL;  // Ptr to result global memory
    APLINT            aplIntegerRht;    // Right arg integer value
    UBOOL             bRet;             // TRUE iff the result is valid
    APLUINT           ByteRes,          // # bytes in the result
                      uRes;             // Loop counter
    LPPL_YYSTYPE      lpYYRes = NULL;   // Ptr to result
    LPPLLOCALVARS     lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;     // Ptr to Ctrl-Break flag
    APLMPI            aplMPIRht = {0},  // Right arg as MP integer
                      aplMPIRes = {0};  // Result    ...
    LPVOID            lpMemRht;         // Ptr to right arg global memory
    MEMTMP            memTmp = {0};     // Temporary memory for factors

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

    // Get right arg's global ptr
    aplIntegerRht = GetGlbPtrs (lptkRhtArg, &hGlbRht);

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            // Save in local var
            mpz_init_set_sx (&aplMPIRht, aplIntegerRht);

            break;

        case ARRAY_FLOAT:
            // Attempt to convert the float to an integer using System CT
            aplIntegerRht = FloatToAplint_SCT ((APLFLOAT) aplIntegerRht, &bRet);
            if (!bRet)
                goto RIGHT_DOMAIN_EXIT;

            // Save in local var
            mpz_init_set_sx (&aplMPIRht, aplIntegerRht);

            break;

        case ARRAY_RAT:
            // Lock the memory to get a ptr to it
            lpMemRht = MyGlobalLock (hGlbRht);

            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayDataFmBase (lpMemRht);

            // Save in local var
            mpz_init_set (&aplMPIRht, mpq_numref ((LPAPLRAT) lpMemRht));

            // Is it an integer?
            bRet = mpq_integer_p ((LPAPLRAT) lpMemRht);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

            // If it's not an integer, ...
            if (!bRet)
                goto RIGHT_DOMAIN_EXIT;
            break;

        case ARRAY_VFP:
            // Lock the memory to get a ptr to it
            lpMemRht = MyGlobalLock (hGlbRht);

            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayDataFmBase (lpMemRht);

            // Save in local var
            mpz_init_set_f (&aplMPIRht, (LPAPLVFP) lpMemRht);

            // Is it an integer?
            bRet = mpf_integer_p ((LPAPLVFP) lpMemRht);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

            // If it's not an integer, ...
            if (!bRet)
                goto RIGHT_DOMAIN_EXIT;
            break;

        default:
            goto RIGHT_DOMAIN_EXIT;
    } // End SWITCH

    // Check the singleton arg
    if (mpz_sgn (&aplMPIRht) <= 0)
        goto RIGHT_DOMAIN_EXIT;

    // Allocate memory for the APLMPI factors
    memTmp.hGlbMem =
      DbgGlobalAlloc (GHND, INIT_FACTOR_CNT * sizeof (APLMPI));

    // Check for error
    if (memTmp.hGlbMem EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    memTmp.lpMemOrg =
    memTmp.lpMemNxt =
      MyGlobalLock (memTmp.hGlbMem);

    // Initialize the # allocated entries
    memTmp.uMaxEntry = INIT_FACTOR_CNT;

    // Factor the given number

    // Call common routine
    aplMPIRes =
      PrimFnPiCommon (&memTmp,              // Ptr to factor struc (may be NULL)
                      NUMTHEORY_FACTOR,     // Function index
                     &aplMPIRht,            // Value to factor
                      lpbCtrlBreak,         // Ptr to Ctrl-Break flag
                     &bRet);                // Ptr to TRUE iff the result is valid
    // Check for error
    if (!bRet)
    {
        if (mpz_cmp_si (&aplMPIRes, -1) EQ 0)
            goto NONCE_EXIT;
        else
            goto WSFULL_EXIT;
    } // End IF

    // Get the actual NELM
    aplNELMRes = memTmp.uNumEntry;

    Assert (aplNELMRes EQ mpz_get_sx (&aplMPIRes));

    // Check for Ctrl-Break
    if (CheckCtrlBreak (*lpbCtrlBreak))
        goto ERROR_EXIT;

    // Set the result storage type
    aplTypeRes = ARRAY_RAT;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result.
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Skip over the header to the dimension
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Save and skip over the actual dimension
    *((LPAPLDIM) lpMemRes)++ = aplNELMRes;

    // Copy the MPI data to the result as RAT
    for (uRes = 0; uRes < aplNELMRes; uRes++)
        // Copy the MPI value as a RAT
        mpq_init_set_z (((LPAPLRAT) lpMemRes)++, &memTmp.lpMemOrg[uRes]);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes;
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

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

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
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
NORMAL_EXIT:
    // Free the storage in memTmp.lpMemOrg (if any)
    for (uRes = 0; uRes < memTmp.uNumEntry; uRes++)
        Myz_clear (&memTmp.lpMemOrg[uRes]);

    // If we allocated memory for the factors, ...
    if (memTmp.lpMemOrg)
    {
        // We no longer need this ptr
        MyGlobalUnlock (memTmp.lpMemOrg); memTmp.lpMemOrg = memTmp.lpMemNxt = NULL;

        // We no longer need this storage
        MyGlobalFree (memTmp.hGlbMem); memTmp.hGlbMem = NULL;
    } // End IF

    // We no longer need this storage
    Myz_clear (&aplMPIRes);
    Myz_clear (&aplMPIRht);

    return lpYYRes;
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
    RaiseException (EXCEPTION_RESULT_RAT, 0, 0, NULL);

    return 0;           // To keep the compiler happy
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
    if (!bRet && aplIntegerLft NE NUMTHEORY_NUMPRIMES)
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

    return 0.0;         // To keep the compiler happy
} // End PrimFnDydPiFisIvI
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydPiRisRvR
//
//  Primitive scalar function dyadic Pi:  R {is} R fn R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydPiRisRvR"
#else
#define APPEND_NAME
#endif

APLRAT PrimFnDydPiRisRvR
    (APLRAT     aplRatLft,
     APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    LPPERTABDATA  lpMemPTD;             // Ptr to PerTabData global memory
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLRAT        mpqRes = {0};         // The result
    APLINT        aplIntegerLft;        // Left arg as an integer
    UBOOL         bRet;                 // TRUE iff the result is valid
    APLMPI        aplMPIRht = {0},      // Right arg as MP integer
                  aplMPIRes = {0};      // Result    ...
    APLBOOL       bQuadIO;              // []IO
    EXCEPTION_CODES exceptionCode = EXCEPTION_SUCCESS;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check the (singleton) left arg
    if (!mpq_integer_p (&aplRatLft)
/////|| mpq_cmp_ui (&aplRatLft,                0, 1) <  0   // Unnecessary as the next comp is unsigned
     || mpq_cmp_ui (&aplRatLft, NUMTHEORY_LENGTH, 1) >= 0)
        goto LEFT_DOMAIN_EXIT;

    // Get the left arg as an integer
    //  (Ignore the value in bRet as we know it's an integer)
    aplIntegerLft = mpq_get_sx (&aplRatLft, &bRet);

    // Allow Nth Prime right arg to be 0 if origin-0
    if (!(mpq_cmp_ui (&aplRatRht, bQuadIO, 1) >= 0
      && aplIntegerLft EQ NUMTHEORY_NTHPRIME))
    {
        // Check the (singleton) right arg
        if (mpq_sgn (&aplRatRht) <= 0)
            goto RIGHT_DOMAIN_EXIT;
    } // End IF

    // Initialize the temp
    mpz_init (&aplMPIRht);

    // Allow Num Primes right arg to be fractional
    if (aplIntegerLft EQ NUMTHEORY_NUMPRIMES)
    {
        mpq_init_set (&mpqRes, &aplRatRht);
        mpq_floor    (&mpqRes, &mpqRes);
        mpz_set      (&aplMPIRht, mpq_numref (&mpqRes));
        Myq_clear    (&mpqRes);
    } else
        // Copy the right arg as an integer
        mpz_set (&aplMPIRht, mpq_numref (&aplRatRht));
    __try
    {
    // Split cases based upon the left arg
    switch (aplIntegerLft)
    {
        case NUMTHEORY_DIVCNT:          // Divisor function (count of divisors of N)
        case NUMTHEORY_DIVSUM:          // Divisor function (sum of divisors of N)
        case NUMTHEORY_MOBIUS:          // Mobius function
        case NUMTHEORY_TOTIENT:         // Totient function (# positive integers coprime to N)
            // Call common routine
            aplMPIRes =
              PrimFnPiCommon (NULL,             // Ptr to factor struc (may be NULL)
                              aplIntegerLft,    // Function index
                             &aplMPIRht,        // Value to factor
                              lpbCtrlBreak,     // Ptr to Ctrl-Break flag
                             &bRet);            // Ptr to TRUE iff the result is valid
            // Check for error
            if (!bRet)
                goto NONCE_EXIT;
            break;

        case NUMTHEORY_ISPRIME:         // TRUE iff the given # is prime
            aplMPIRes =
              PrimFnPiIsPrime   (aplMPIRht, lpbCtrlBreak, lpMemPTD);
            break;

        case NUMTHEORY_NEXTPRIME:       // Next prime after a given #
            aplMPIRes =
              PrimFnPiNextPrime (aplMPIRht, lpbCtrlBreak, lpMemPTD);
            break;

        case NUMTHEORY_PREVPRIME:       // Prev prime before a given #
            aplMPIRes =
              PrimFnPiPrevPrime (aplMPIRht, lpbCtrlBreak, lpMemPTD);
            break;

        case NUMTHEORY_NTHPRIME:        // Nth prime
            aplMPIRes =
              PrimFnPiNthPrime  (aplMPIRht, lpbCtrlBreak, lpMemPTD);
            break;

        case NUMTHEORY_NUMPRIMES:       // # primes <= a given #
            aplMPIRes =
              PrimFnPiNumPrimes (aplMPIRht, lpbCtrlBreak, lpMemPTD);
            break;

        defstop
            break;
    } // End SWITCH
    } __except (CheckException (GetExceptionInformation (), L"PrimFnDydPiVisVvV"))
    {
#ifdef DEBUG
        dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #1: %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
        // Save the exception code
        exceptionCode = MyGetExceptionCode ();

        // Split cases based upon the ExceptionCode
        switch (exceptionCode)
        {
            case EXCEPTION_DOMAIN_ERROR:
            case EXCEPTION_NONCE_ERROR:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                goto ERROR_EXIT;

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except

    // Initialize the result to 0/1
    mpq_init (&mpqRes);

    // Copy the return value to the numerator of the result
    mpz_set (mpq_numref (&mpqRes), &aplMPIRes);

    goto NORMAL_EXIT;

NONCE_EXIT:
    exceptionCode = EXCEPTION_NONCE_ERROR;

    goto NORMAL_EXIT;

LEFT_DOMAIN_EXIT:
RIGHT_DOMAIN_EXIT:
    exceptionCode = EXCEPTION_DOMAIN_ERROR;
ERROR_EXIT:
NORMAL_EXIT:
    Myz_clear (&aplMPIRes);
    Myz_clear (&aplMPIRht);

    if (exceptionCode NE EXCEPTION_SUCCESS)
        RaiseException (exceptionCode, 0, 0, NULL);

    return mpqRes;
} // End PrimFnDydPiRisRvR
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydPiVisVvV
//
//  Primitive scalar function dyadic Pi:  V {is} V fn V
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydPiVisVvV"
#else
#define APPEND_NAME
#endif

APLVFP PrimFnDydPiVisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0};
    APLRAT mpqLft = {0},
           mpqRht = {0},
           mpqRes = {0};
    EXCEPTION_CODES exceptionCode = EXCEPTION_SUCCESS;

    // Convert the VFP to a RAT
    mpq_init_set_f (&mpqLft, &aplVfpLft);
    mpq_init_set_f (&mpqRht, &aplVfpRht);

    __try
    {
        // Call the RvR code
        mpqRes = PrimFnDydPiRisRvR (mpqLft, mpqRht, lpPrimSpec);
    } __except (CheckException (GetExceptionInformation (), L"PrimFnDydPiVisVvV"))
    {
#ifdef DEBUG
        dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #1: %2d (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
        // Save the exception code
        exceptionCode = MyGetExceptionCode ();

        // Split cases based upon the ExceptionCode
        switch (exceptionCode)
        {
            case EXCEPTION_DOMAIN_ERROR:
            case EXCEPTION_NONCE_ERROR:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                goto ERROR_EXIT;

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except

    // Convert the RAT to a VFP
    mpf_init_set_q (&mpfRes, &mpqRes);
ERROR_EXIT:
    // We no longer need this storage
    Myq_clear (&mpqRes);
    Myq_clear (&mpqRht);
    Myq_clear (&mpqLft);

    if (exceptionCode NE EXCEPTION_SUCCESS)
        RaiseException (exceptionCode, 0, 0, NULL);

    return mpfRes;      // To keep the compiler happy
} // End PrimFnDydPiVisVvV
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

APLMPI PrimFnPiCommon
    (LPMEMTMP     lpMemTmp,         // Ptr to factor struc (may be NULL)
     APLUINT      uFcnIndex,        // Function index (see enum NUMTHEORY)
     LPAPLMPI     mpzRht,           // Ptr to value to factor
     LPUBOOL      lpbCtrlBreak,     // Ptr to Ctrl-Break flag
     LPUBOOL      lpbRet)           // Ptr to TRUE iff the result is valid

{
    APLUINT      uRes,              // Loop counter
                 uPrime;            // The current prime
    UINT         uPrimeCnt;         // # occurrences of a specific prime
    APLNELM      aplNELMRes;        // Result NELM
    PROCESSPRIME procPrime;         // Struct for args to ProcessPrime
    APLMPI       aplTmp = {0},
                 aplPrime = {0};

    // Initialize the function vars
    procPrime.uFcnIndex    = uFcnIndex;
    procPrime.bSquareFree  = TRUE;
    mpz_init_set    (&procPrime.mpzTotient, mpzRht);
    mpz_init_set_ui (&procPrime.mpzDivisor, 1);
    procPrime.lpbCtrlBreak = lpbCtrlBreak;
    get_random_seeds (&procPrime.seed1, &procPrime.seed2);

    // Initialize the result
    *lpbRet = TRUE;

    // Initialize the temps
    mpz_init (&aplTmp);
    mpz_init (&aplPrime);

    // Run through the list of small primes
    // This handles all numbers < PRECOMPUTED_PRIME_NEXT2 = ~1e10 (~33 bits)
    for (aplNELMRes = uPrime = uRes = 0;
         mpz_cmp_ui (mpzRht, 1) NE 0
      && uRes < PRECOMPUTED_NUM_PRIMES;
         uRes++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Initialize the prime
        uPrime += prime_delta[uRes];
        mpz_set_ui (&aplPrime, (UINT) uPrime);

        // If it's divisible, ...
        if (mpz_divisible_p (mpzRht, &aplPrime))
        {
            // Remove all occurrences of this prime from mpzRht
            uPrimeCnt = (UINT) mpz_remove (mpzRht, mpzRht, &aplPrime);

            // Count it in
            aplNELMRes += uPrimeCnt;

            // Process this prime factor
            ProcessPrime (aplPrime, uPrimeCnt, &procPrime);

            // If we're saving the values, ...
            if (lpMemTmp)
            while (uPrimeCnt--)
            {
                // Resize the factor struc if needed
                if (!ResizeFactorStruc (lpMemTmp))
                    goto WSFULL_EXIT;

                // Save in the result
                mpz_init_set (lpMemTmp->lpMemNxt++, &aplPrime); lpMemTmp->uNumEntry++;
            } // End WHILE
        } // End IF
    } // End FOR

    // Check against the smallest # not factorable by trial division
    if (mpz_cmp_ui (mpzRht, 1) > 0
     && mpz_cmp_sx (mpzRht, PRECOMPUTED_PRIME_NEXT2) < 0)   // = 100003L*100003L
    {
         // Count it in
         aplNELMRes++;

        // Process this prime factor
        ProcessPrime (*mpzRht, 1, &procPrime);

        // If we're saving the values, ...
        if (lpMemTmp)
        {
            // Resize the factor struc if needed
            if (!ResizeFactorStruc (lpMemTmp))
                goto WSFULL_EXIT;

            // Save in the result
            mpz_init_set (lpMemTmp->lpMemNxt++, mpzRht); lpMemTmp->uNumEntry++;
        } // End IF
    } else
    // Loop until we have no more factors
    while (mpz_cmp_ui (mpzRht, 1) > 0)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Try to factor aplIntegerRht using various methods
        Myz_clear (&aplPrime);
        aplPrime = PrimeFactor (*mpzRht, &procPrime, lpbRet);

        // Check for error
        if (!*lpbRet)
            goto NONCE_EXIT;

        // If it's divisible, ...
        if (mpz_divisible_p (mpzRht, &aplPrime))
        {
            // Remove all occurrences of this prime from mpzRht
            uPrimeCnt = (UINT) mpz_remove (mpzRht, mpzRht, &aplPrime);

            // Count it in
            aplNELMRes += uPrimeCnt;

            // Process this prime factor
            ProcessPrime (aplPrime, uPrimeCnt, &procPrime);

            // If we're saving the values, ...
            if (lpMemTmp)
            while (uPrimeCnt--)
            {
                // Resize the factor struc if needed
                if (!ResizeFactorStruc (lpMemTmp))
                    goto WSFULL_EXIT;

                // Save in the result
                mpz_init_set (lpMemTmp->lpMemNxt++, &aplPrime); lpMemTmp->uNumEntry++;
            } // End WHILE
        } // End IF
    } // End WHILE

    // Split cases based upon the function index
    switch (uFcnIndex)
    {
        case NUMTHEORY_FACTOR:
            mpz_set_sx (&aplPrime, aplNELMRes);

            break;

        case NUMTHEORY_DIVCNT:
        case NUMTHEORY_DIVSUM:
            // Copy to result var
            mpz_set (&aplPrime, &procPrime.mpzDivisor);

            break;

        case NUMTHEORY_MOBIUS:
            // If the prime factors are all unique, ...
            if (procPrime.bSquareFree)
                // Return (-1) * (the parity of the # factors)
                mpz_set_sx (&aplPrime, (aplNELMRes & BIT0) ? -1 : 1);
            else
                // The prime factors are not all unique
                mpz_set_ui (&aplPrime, 0);
            break;

        case NUMTHEORY_TOTIENT:
            // Copy to result var
            mpz_set (&aplPrime, &procPrime.mpzTotient);

            break;

        case NUMTHEORY_ISPRIME:
        case NUMTHEORY_NEXTPRIME:
        case NUMTHEORY_PREVPRIME:
        case NUMTHEORY_NTHPRIME:
        case NUMTHEORY_NUMPRIMES:
        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

NONCE_EXIT:
    mpz_set_sx (&aplPrime, -1);

    goto ERROR_EXIT;

WSFULL_EXIT:
    mpz_set_sx (&aplPrime, -2);

    goto ERROR_EXIT;

ERROR_EXIT:
    // If we're saving the values, ...
    if (lpMemTmp)
    for (uRes = 0; uRes < lpMemTmp->uNumEntry; uRes++)
        Myz_clear (&lpMemTmp->lpMemOrg[uRes]);
NORMAL_EXIT:
    // We no longer need this storage
    Myz_clear (&aplTmp);
    Myz_clear (&procPrime.mpzDivisor);
    Myz_clear (&procPrime.mpzTotient);

    return aplPrime;
} // End PrimFnPiCommon
#undef  APPEND_NAME


//***************************************************************************
//  $ResizeFactorStruc
//
//  Resize the factor memory struc if needed
//***************************************************************************

UBOOL ResizeFactorStruc
    (LPMEMTMP lpMemTmp)

{
    HGLOBAL  hGlbMem;
    APLU3264 uFactors;

    // If we're at the current maximum, ...
    if (lpMemTmp->uNumEntry EQ lpMemTmp->uMaxEntry)
    {
        // Compute the # factors
        uFactors = (lpMemTmp->lpMemNxt - lpMemTmp->lpMemOrg);

        // Unlock so we may reallocate it
        MyGlobalUnlock (lpMemTmp->lpMemOrg); lpMemTmp->lpMemOrg = lpMemTmp->lpMemNxt = NULL;

        hGlbMem =
          MyGlobalReAlloc (lpMemTmp->hGlbMem,
                           (lpMemTmp->uMaxEntry + INIT_FACTOR_INC) * sizeof (APLMPI),
                           GMEM_MOVEABLE | GMEM_ZEROINIT);
        if (hGlbMem)
        {
            // Save the (possibly new) global memory handle
            lpMemTmp->hGlbMem = hGlbMem;

            // Lock the memory to get a ptr to it
            lpMemTmp->lpMemOrg = MyGlobalLock (lpMemTmp->hGlbMem);

            // Point to the current entry
            lpMemTmp->lpMemNxt = lpMemTmp->lpMemOrg + uFactors;

            // Count in the new maximum
            lpMemTmp->uMaxEntry += INIT_FACTOR_INC;
        } else
            return FALSE;
    } // End IF

    return TRUE;
} // End ResizeFactorStruc


//***************************************************************************
//  $PrimeFactor
//
//  Return either a prime factor of a given #
//    or the number itself if it's prime
//***************************************************************************

APLMPI PrimeFactor
    (APLMPI         mpzNumber,          // The number to factor
     LPPROCESSPRIME lpProcPrime,        // Ptr to PROCESSPRIME struc
     LPUBOOL        lpbRet)             // Ptr to TRUE iff the result is valid

{
    APLMPI  mpzFactor1 = {0},           // Factor #1
            mpzFactor2 = {0},           // ...     2
            mpzRes     = {0};           // Result
    APLUINT uVal,                       // Temporary value
            bits;                       // # bits in the value
#ifdef DEBUG
    char    szTemp1[1024],              // Temporary output save area
            szTemp2[1024];              // ...
#endif

    // get the bit size of the incoming number
    bits = mpz_sizeinbase (&mpzNumber, 2);

    // Initialize the result & temps
    mpz_init_set (&mpzRes, &mpzNumber);
    mpz_init     (&mpzFactor1);
    mpz_init     (&mpzFactor2);

    if (bits <= SMALL_COMPOSITE_CUTOFF_BITS)            // 85
    {
        if (bits <= PRECOMPUTED_PRIME_NEXT2_LOG2)       // = 33
            goto NORMAL_EXIT;
        else
        if (bits <= 60)
        {
            uVal = MySqufof (&mpzRes, lpProcPrime->lpbCtrlBreak);

            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpProcPrime->lpbCtrlBreak))
                goto BREAK_EXIT;

            switch (uVal)
            {
                case 0:
                case 1:
                    // Try Pollard's Rho
                    if (MyRho (&mpzRes, &mpzFactor1, lpProcPrime))
                    {
#ifdef DEBUG
                        dprintfWL0 (L"MyRho:     %S", mpz_get_str (szTemp1, 10, &mpzFactor1));
#endif
                        Myz_clear (&mpzRes);
                        mpzRes = PrimeFactor (mpzFactor1, lpProcPrime, lpbRet);

                        // Check for error
                        if (!*lpbRet)
                            goto NONCE_EXIT;

                        goto NORMAL_EXIT;
                    } else
                    {
                        uVal = MyTinyqs (&mpzRes, &mpzFactor1, &mpzFactor2, lpProcPrime->lpbCtrlBreak);

                        if (mpz_cmp_ui (&mpzFactor1, 1) EQ 0)
                            uVal = 0;
                    } // End IF
#ifdef DEBUG
                    if (uVal)
                        dprintfWL0 (L"MyTinyqs:  %S %S", mpz_get_str (szTemp1, 10, &mpzFactor1), mpz_get_str (szTemp2, 10, &mpzFactor2));
#endif
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpProcPrime->lpbCtrlBreak))
                        goto BREAK_EXIT;

                    break;

                default:
#ifdef DEBUG
                    dprintfWL0 (L"MySqufof:  %I64u", uVal);
#endif
                    // If the value is within trial division, ...
                    if (uVal < PRECOMPUTED_PRIME_NEXT2)
                    {
                        // It's a prime:  return it
                        mpz_set_sx (&mpzRes, uVal);

                        goto NORMAL_EXIT;
                    } // End IF

                    // Otherwise, it may be composite:  try to factor it
                    mpz_set_sx (&mpzFactor1, uVal);

                    Myz_clear (&mpzRes);
                    mpzRes = PrimeFactor (mpzFactor1, lpProcPrime, lpbRet);

                    // Check for error
                    if (!*lpbRet)
                        goto NONCE_EXIT;

                    goto NORMAL_EXIT;
            } // End SWITCH
        } else
        {
            uVal = MyTinyqs (&mpzRes, &mpzFactor1, &mpzFactor2, lpProcPrime->lpbCtrlBreak);

            if (mpz_cmp_ui (&mpzFactor1, 1) EQ 0)
                uVal = 0;
#ifdef DEBUG
            if (uVal)
                dprintfWL0 (L"MyTinyqs:  %S %S", mpz_get_str (szTemp1, 10, &mpzFactor1), mpz_get_str (szTemp2, 10, &mpzFactor2));
#endif
        } // End IF

        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpProcPrime->lpbCtrlBreak))
            goto BREAK_EXIT;

        if (uVal)
        {
            if (IsMpz1 (&mpzFactor1))
                mpz_set (&mpzRes, &mpzFactor2);
            else
            if (IsMpz1 (&mpzFactor2))
                mpz_set (&mpzRes, &mpzFactor1);
            else
            {
                Myz_clear (&mpzRes);

                // Try again with the smaller # so as to try to return factors in ascending order
                mpzRes = PrimeFactor ((mpz_cmp (&mpzFactor1, &mpzFactor2) < 0) ? mpzFactor1 : mpzFactor2, lpProcPrime, lpbRet);

                // Check for error
                if (!*lpbRet)
                    goto NONCE_EXIT;
            } // End IF/ELSE/...

            goto NORMAL_EXIT;
        } // End IF
    } // End IF
#ifdef MPQS_ENABLED
    if (MyFactor_mpqs (&mpzRes))
        goto NORMAL_EXIT;
#endif
    if (mpz_likely_prime_p (&mpzRes, GetMemPTD ()->randState, 0))
    {
#ifdef DEBUG
        dprintfWL0 (L"prime?:  %S", mpz_get_str (szTemp1, 10, &mpzRes));
#endif
        goto NORMAL_EXIT;
    } // End IF

    *lpbRet = FALSE;
NONCE_EXIT:
BREAK_EXIT:
NORMAL_EXIT:
    Myz_clear (&mpzFactor1);
    Myz_clear (&mpzFactor2);

    return mpzRes;
} // End PrimeFactor


//***************************************************************************
//  $ProcessPrime
//
//  Process a prime according to the function index
//***************************************************************************

void ProcessPrime
    (APLMPI         aplMPIRht,      // The prime to process
     UINT           uPrimeCnt,      // The prime exponent
     LPPROCESSPRIME lpProcPrime)    // Ptr to PROCESSPRIME struc

{
    APLMPI  mpzTmp = {0};           // Temporary

    // Initialize the temp
    mpz_init (&mpzTmp);

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
            {
////////////////lpProcPrime->uTotient -= lpProcPrime->uTotient / uPrime;
                mpz_divexact (&mpzTmp, &lpProcPrime->mpzTotient, &aplMPIRht);
                mpz_sub      (&lpProcPrime->mpzTotient, &lpProcPrime->mpzTotient, &mpzTmp);
            } // End IF

            break;

        case NUMTHEORY_DIVCNT:
            // If this prime divides N, ...
            if (uPrimeCnt)
            {
////////////////lpProcPrime->uDivisor *= (uPrimeCnt + 1);
                mpz_set_sx (&mpzTmp, uPrimeCnt + 1);
                mpz_mul (&lpProcPrime->mpzDivisor, &lpProcPrime->mpzDivisor, &mpzTmp);
            } // End IF

            break;

        case NUMTHEORY_DIVSUM:
            // If this prime divides N, ...
            if (uPrimeCnt)
            {
                APLMPI mpzTmp2 = {0};

                // Initialize the temp to 0
                mpz_init (&mpzTmp2);

                // Tmp2 = p - 1
                mpz_sub_ui (&mpzTmp2, &aplMPIRht, 1);

                // Tmp = exp (p, uPrimeCnt + 1)
                mpz_pow_ui (&mpzTmp, &aplMPIRht, uPrimeCnt + 1);

                // Tmp = Tmp - 1
                mpz_sub_ui (&mpzTmp, &mpzTmp, 1);

                // Tmp = Tmp / (p - 1)
                mpz_div    (&mpzTmp, &mpzTmp, &mpzTmp2);

////////////////lpProcPrime->uDivisor *= uAccum;
                mpz_mul (&lpProcPrime->mpzDivisor, &lpProcPrime->mpzDivisor, &mpzTmp);

                // We no longer need this storage
                Myz_clear (&mpzTmp2);
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

    // We no longer need this storage
    Myz_clear (&mpzTmp);
} // End ProcessPrime


//***************************************************************************
//  $PrimFnPiIsPrime
//
//  Return TRUE or FALSE for whether or not a given # is a prime
//***************************************************************************

APLMPI PrimFnPiIsPrime
    (APLMPI        aplMPIArg,       // The number to check
     LPUBOOL       lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPPERTABDATA  lpMemPTD)        // Ptr to PerTabData global memory

{
    APLMPI mpzRes = {0};            // The result

    // Initialize the result to 0
    mpz_init (&mpzRes);

    if (mpz_cmp_ui (&aplMPIArg, 1) > 0)
        mpz_set_ui (&mpzRes, mpz_likely_prime_p (&aplMPIArg, lpMemPTD->randState, 0));

    return mpzRes;
} // End PrimFnPiIsPrime


//***************************************************************************
//  $PrimFnPiNextPrime
//
//  Return the next prime after than a given #
//***************************************************************************

APLMPI PrimFnPiNextPrime
    (APLMPI        aplMPIArg,       // The number to check
     LPUBOOL       lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPPERTABDATA  lpMemPTD)        // Ptr to PerTabData global memory

{
    // Handle special cases
    if (mpz_cmp_ui (&aplMPIArg, 2) <= 0)
    {
        APLMPI mpzRes = {0};            // The result

        mpz_init_set (&mpzRes, &aplMPIArg);
        mpz_add_ui   (&mpzRes, &mpzRes, 1);

        return mpzRes;
    } else
        // Calculate the next likely prime
        return mpz_next_prime (&aplMPIArg, lpbCtrlBreak, lpMemPTD);
} // End PrimFnPiNextPrime


//***************************************************************************
//  $PrimFnPiPrevPrime
//
//  Return the Prev prime before a given #
//***************************************************************************

APLMPI PrimFnPiPrevPrime
    (APLMPI        aplMPIArg,       // The number to check
     LPUBOOL       lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPPERTABDATA  lpMemPTD)        // Ptr to PerTabData global memory

{
    APLMPI mpzRes = {0};            // The result

    // Handle special cases
    if (mpz_cmp_ui (&aplMPIArg, 2) <= 0)
    {
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        return mpzRes;
    } else
    if (mpz_cmp_ui (&aplMPIArg, 3) EQ 0)
    {
        mpz_init_set_sx (&mpzRes, 2);

        return mpzRes;
    } else
        // Calculate the previous likely prime
        return mpz_prev_prime (&aplMPIArg, lpbCtrlBreak, lpMemPTD);
} // End PrimFnPiPrevPrime


//***************************************************************************
//  $NthPrime
//
//  Calculate the Nth prime if it's between small consecutive powers of ten
//***************************************************************************

UBOOL NthPrime
    (LPAPLMPI mpzRes,               // The current power of ten
     LPAPLMPI aplMPIArg,            // The number to check (origin-sensitive)
     LPUBOOL  lpbCtrlBreak,         // Ptr to Ctrl-Break flag
     UBOOL    bQuadIO,              // []IO
     APLUINT  aplIntRes,            // The result if mpzRes EQ aplMPIArg (0 = use char)
     LPCHAR   lpCharRes)            // Character form of the result if it's too large for a 64-bit integer (NULL is not used)

{
    UBOOL  bRet = FALSE;            // TRUE iff the result is valid or Ctrl-Break pressed
    APLMPI mpzTmp = {0},            // Temporary
           mpzTen = {0},            // ...
           mpzArg = {0};            // ...

    // Initialize the temps
    mpz_init (&mpzTmp);
    mpz_init (&mpzTen);
    mpz_init (&mpzArg);

    // Convert to origin-1
    mpz_add_ui (&mpzArg, aplMPIArg, 1 - GetQuadIO ());

    // Copy as the next power of ten
    mpz_mul_ui (&mpzTmp, mpzRes, 10);

    // If the origin-1 arg is between these powers of ten, ...
    if (mpz_cmp ( mpzRes, &mpzArg) <= 0
     && mpz_cmp (&mpzArg, &mpzTmp) <  0)
    {
        // Copy the power of ten
        mpz_set (&mpzTen,  mpzRes);

        // Set the Nth prime for the power of ten in mpzRes
        if (lpCharRes EQ NULL)
            mpz_set_sx  (mpzRes, aplIntRes);
        else
            mpz_set_str (mpzRes, lpCharRes, 10);

        while (mpz_cmp (&mpzArg, &mpzTen) > 0)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            // Increment the count
            mpz_add_ui (&mpzTen, &mpzTen, 1);

            // Calculate the next prime
            mpz_nextprime (mpzRes, mpzRes);
        } // End WHILE

        // Mark as valid result
        bRet = TRUE;
    } // End IF

    goto NORMAL_EXIT;

ERROR_EXIT:
    // Mark as Ctrl-Break pressed
    bRet = TRUE;
NORMAL_EXIT:
    // We no longer need this storage
    Myz_clear (&mpzArg);
    Myz_clear (&mpzTen);
    Myz_clear (&mpzTmp);

    return bRet;
} // End NthPrime


//***************************************************************************
//  $PrimFnPiNthPrime
//
//  Return the Nth prime
//***************************************************************************

APLMPI PrimFnPiNthPrime
    (APLMPI        aplMPIArg,       // The number to check
     LPUBOOL       lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPPERTABDATA  lpMemPTD)        // Ptr to PerTabData global memory

{
    APLMPI  mpzRes = {0};           // The result
    APLBOOL bQuadIO;                // []IO

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Handle special cases
    // The following answers were obtained from
    //   http://oeis.org/A006988/
    mpz_init_set_ui (&mpzRes, 1);                   // 1E0
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 2, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E1
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 29, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E2
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 541, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E3
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 7919, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E4
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 104729, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E5
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 1299709, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E6
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 15485863, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E7
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 179424673, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E8
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 2038074743, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E9
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 22801763489, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E10
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 252097800623, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E11
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 2760727302517, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E12
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 29996224275833, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E13
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 323780508946331, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E14
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 3475385758524527, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E15
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 37124508045065437, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E16
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 394906913903735329, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E17
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 4185296581467695669, NULL))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E18
    if (NthPrime (&mpzRes, &aplMPIArg, lpbCtrlBreak, bQuadIO, 0, "44211790234832169331"))
        goto NORMAL_EXIT;

    // We no longer need this storage
    Myz_clear (&mpzRes);

    RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

NORMAL_EXIT:
    return mpzRes;
} // End PrimFnPiNthPrime


//***************************************************************************
//  $NumPrimes
//
//  Calculate the # primes between small consecutive powers of ten
//***************************************************************************

UBOOL NumPrimes
    (LPAPLMPI mpzRes,               // The current power of ten
     LPAPLMPI aplMPIArg,            // The number to check
     LPUBOOL  lpbCtrlBreak,         // Ptr to Ctrl-Break flag
     APLUINT  aplIntRes)            // The result if mpzRes EQ aplMPIArg

{
    UBOOL  bRet = FALSE;            // TRUE iff the result is valid or Ctrl-Break pressed
    APLMPI mpzTmp = {0},            // Temporary
           mpzTen = {0};            // ...

    // Initialize the temps
    mpz_init (&mpzTmp);
    mpz_init (&mpzTen);

    // Copy as the next power of ten
    mpz_mul_ui (&mpzTmp, mpzRes, 10);

    // If the origin-1 arg is between these powers of ten, ...
    if (mpz_cmp ( mpzRes, aplMPIArg) <= 0
     && mpz_cmp (aplMPIArg, &mpzTmp) <  0)
    {
        // Copy as the current power of ten
        mpz_set (&mpzTen, mpzRes);

        // Initialize the current count
        mpz_set_sx (mpzRes, aplIntRes);

        // Loop through mpz_nextprime
        while (mpz_cmp (aplMPIArg, &mpzTen) > 0)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            // Calculate the next prime
            mpz_nextprime (&mpzTen, &mpzTen);

            // Count in another prime
            mpz_add_ui (mpzRes, mpzRes, 1);
        } // End WHILE

        // If the arg is < the prime, ...
        if (mpz_cmp (aplMPIArg, &mpzTen) < 0)
            // Subtract out the extra one
            mpz_sub_ui (mpzRes, mpzRes, 1);

        // Mark as valid result
        bRet = TRUE;
    } // End IF

    goto NORMAL_EXIT;

ERROR_EXIT:
    // Mark as Ctrl-Break pressed
    bRet = TRUE;
NORMAL_EXIT:
    // We no longer need this storage
    Myz_clear (&mpzTen);
    Myz_clear (&mpzTmp);

    return bRet;
} // End NumPrimes


//***************************************************************************
//  $PrimFnPiNumPrimes
//
//  Return # primes <= a given #
//***************************************************************************

APLMPI PrimFnPiNumPrimes
    (APLMPI        aplMPIArg,       // The number to check
     LPUBOOL       lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPPERTABDATA  lpMemPTD)        // Ptr to PerTabData global memory

{
    APLMPI mpzRes = {0};            // The result

    // Handle special cases
    // The following answers were obtained from
    //   http://oeis.org/A006880
    mpz_init_set_ui (&mpzRes, 1);                   // 1E0
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 0))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E1
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 4))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E2
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 25))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E3
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 168))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E4
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 1229))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E5
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 9592))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E6
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 78498))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E7
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 664579))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E8
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 5761455))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E9
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 50847534))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E10
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 455052511))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E11
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 4118054813))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E12
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 37607912018))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E13
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 346065536839))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E14
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 3204941750802))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E15
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 29844570422669))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E16
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 279238341033925))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E17
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 2623557157654233))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E18
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 24739954287740860))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E19
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 234057667276344607))
        goto NORMAL_EXIT;

    mpz_mul_ui (&mpzRes, &mpzRes, 10);              // 1E20
    if (NumPrimes (&mpzRes, &aplMPIArg, lpbCtrlBreak, 2220819602560918840))
        goto NORMAL_EXIT;

    // We no longer need this storage
    Myz_clear (&mpzRes);

    RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

NORMAL_EXIT:
    return mpzRes;
} // End PrimFnPiNumPrimes


//***************************************************************************
//  $MyTinyqs
//***************************************************************************

uint32 MyTinyqs
    (LPAPLMPI n,
     LPAPLMPI factor1,
     LPAPLMPI factor2,
     LPUBOOL  lpbCtrlBreak)

{
    mp_t   N,                       // Temporary vars
           Factor1,                 // ...
           Factor2;                 // ...
    uint32 uRes;                    // The result
    size_t nWords;

    // Initialize the mp_t vars
    mp_clear (&N);
    mp_clear (&Factor1);
    mp_clear (&Factor2);

    // Convert the APLMPI args to mp_t args
    mpz_export (&N.val[0],                  // rop:     Output save area
                &nWords,                    // countp:  # words written to output save area
                 -1,                        // order:   Least significant word first
                 sizeof (N.val[0]),         // size:    Size of each word in the output save area
                 -1,                        // endian:  Little-endian
                  0,                        // nails:   # nails
                  n);                       // op:      Ptr to input
    N.nwords = (uint32) nWords;

    mpz_export (&Factor1.val[0],            // rop:     Output save area
                &nWords,                    // countp:  # words written to output save area
                 -1,                        // order:   Least significant word first
                 sizeof (Factor1.val[0]),   // size:    Size of each word in the output save area
                 -1,                        // endian:  Little-endian
                  0,                        // nails:   # nails
                  factor1);                 // op:      Ptr to input
    Factor1.nwords = (uint32) nWords;

    mpz_export (&Factor2.val[0],            // rop:     Output save area
                &nWords,                    // countp:  # words written to output save area
                 -1,                        // order:   Least significant word first
                 sizeof (Factor2.val[0]),   // size:    Size of each word in the output save area
                 -1,                        // endian:  Little-endian
                  0,                        // nails:   # nails
                  factor2);                 // op:      Ptr to input
    Factor2.nwords = (uint32) nWords;

    // Call the original handler
    uRes = tinyqs (&N, &Factor1, &Factor2, lpbCtrlBreak);

    // Convert the mp_t args to APLMPI
    mpz_import (n,                          // rop:     Output save area
                N.nwords,                   // count:   # words in the input data
                -1,                         // order:   Least significant word first
                sizeof (N.val[0]),          // size:    Size of each word in the input
                -1,                         // endian:  Little-endian
                 0,                         // nails:   # nails
                &N.val[0]);                 // op:      Ptr to input
    mpz_import (factor1,                    // rop:     Output save area
                Factor1.nwords,             // count:   # words in the input data
                -1,                         // order:   Least significant word first
                sizeof (Factor1.val[0]),    // size:    Size of each word in the input
                -1,                         // endian:  Little-endian
                 0,                         // nails:   # nails
                &Factor1.val[0]);           // op:      Ptr to input
    mpz_import (factor2,                    // rop:     Output save area
                Factor2.nwords,             // count:   # words in the input data
                -1,                         // order:   Least significant word first
                sizeof (Factor2.val[0]),    // size:    Size of each word in the input
                -1,                         // endian:  Little-endian
                 0,                         // nails:   # nails
                &Factor2.val[0]);           // op:      Ptr to input
    return uRes;
} // End MyTinyqs


//***************************************************************************
//  $MySqufof
//***************************************************************************

uint32 MySqufof
    (LPAPLMPI n,
     LPUBOOL  lpbCtrlBreak)

{
    uint32 uRes;                    // The result
    mp_t   N;                       // Temporary var
    size_t nWords;

    // Initialize the mp_t var
    mp_clear (&N);

    // Convert the APLMPI arg to mp_t arg
    mpz_export (&N.val[0],                  // rop:     Output save area
                &nWords,                    // countp:  # words written to output save area
                 -1,                        // order:   Least significant word first
                 sizeof (N.val[0]),         // size:    Size of each word in the output save area
                 -1,                        // endian:  Little-endian
                  0,                        // nails:   # nails
                  n);                       // op:      Ptr to input
    N.nwords = (uint32) nWords;

    // Call the original handler
    uRes = squfof (&N, lpbCtrlBreak);

    // Convert the mp_t args to APLMPI
    mpz_import (n,                          // rop:     Output save area
                N.nwords,                   // count:   # words in the input data
                -1,                         // order:   Least significant word first
                sizeof (N.val[0]),          // size:    Size of each word in the input
                -1,                         // endian:  Little-endian
                 0,                         // nails:   # nails
                &N.val[0]);                 // op:      Ptr to input
    return uRes;
} // End MySqufof


//***************************************************************************
//  $MyRho
//***************************************************************************

uint32 MyRho
    (LPAPLMPI n,
     LPAPLMPI reduced_n,
     LPPROCESSPRIME lpProcPrime)

{
    uint32 uRes;                    // The result
    mp_t   N,                       // Temporary vars
           Reduced_N;               // ...
    size_t nWords;

    // Initialize the mp_t vars
    mp_clear (&N);
    mp_clear (&Reduced_N);

    // Convert the APLMPI args to mp_t args
    mpz_export (&N.val[0],                  // rop:     Output save area
                &nWords,                    // countp:  # words written to output save area
                 -1,                        // order:   Least significant word first
                 sizeof (N.val[0]),         // size:    Size of each word in the output save area
                 -1,                        // endian:  Little-endian
                  0,                        // nails:   # nails
                  n);                       // op:      Ptr to input
    N.nwords = (uint32) nWords;

    mpz_export (&Reduced_N.val[0],          // rop:     Output save area
                &nWords,                    // countp:  # words written to output save area
                 -1,                        // order:   Least significant word first
                 sizeof (Reduced_N.val[0]), // size:    Size of each word in the output save area
                 -1,                        // endian:  Little-endian
                  0,                        // nails:   # nails
                  reduced_n);               // op:      Ptr to input
    Reduced_N.nwords = (uint32) nWords;

    // Call the original handler
    uRes = rho (&N, &Reduced_N, lpProcPrime);

    // Convert the mp_t args to APLMPI
    mpz_import (n,                          // rop:     Output save area
                N.nwords,                   // count:   # words in the input data
                -1,                         // order:   Least significant word first
                sizeof (N.val[0]),          // size:    Size of each word in the input
                -1,                         // endian:  Little-endian
                 0,                         // nails:   # nails
                &N.val[0]);                 // op:      Ptr to input
    mpz_import (reduced_n,                  // rop:     Output save area
                Reduced_N.nwords,           // count:   # words in the input data
                -1,                         // order:   Least significant word first
                sizeof (Reduced_N.val[0]),  // size:    Size of each word in the input
                -1,                         // endian:  Little-endian
                 0,                         // nails:   # nails
                &Reduced_N.val[0]);         // op:      Ptr to input
    return uRes;
} // End MyRho


#ifdef MPQS_ENABLED
//***************************************************************************
//  $MyFactor_mpqs
//***************************************************************************

UBOOL MyFactor_mpqs
    (LPAPLMPI n)

{
    factor_list_t factor_list;
    msieve_obj    obj;
    mp_t          N;                        // Temporary var
    UBOOL         bRet;                     // TRUE iff the result is valid

    size_t nWords;

    // Initialize the mp_t var
    mp_clear (&N);

    // Convert the APLMPI arg to mp_t arg
    mpz_export (&N.val[0],                  // rop:     Output save area
                &nWords,                    // countp:  # words written to output save area
                 -1,                        // order:   Least significant word first
                 sizeof (N.val[0]),         // size:    Size of each word in the output save area
                 -1,                        // endian:  Little-endian
                  0,                        // nails:   # nails
                  n);                       // op:      Ptr to input
    N.nwords = (uint32) nWords;

    bRet = factor_mpqs (&obj, &N, &factor_list);

    // Convert the mp_t arg to APLMPI
    mpz_import (n,                          // rop:     Output save area
                N.nwords,                   // count:   # words in the input data
                -1,                         // order:   Least significant word first
                sizeof (N.val[0]),          // size:    Size of each word in the input
                -1,                         // endian:  Little-endian
                 0,                         // nails:   # nails
                &N.val[0]);                 // op:      Ptr to input
    return bRet;
} // End MyFactor_mpqs
#endif


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
