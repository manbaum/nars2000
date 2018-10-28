//***************************************************************************
//  NARS2000 -- Multi Precision Functions
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2018 Sudley Place Software

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
#include <math.h>               // For fabs
#include "headers.h"

#define int32   int


//***************************************************************************
//  $mp_alloc
//
//  Multi Precision Allocation function
//***************************************************************************

LPVOID mp_alloc
    (size_t size)           // Allocation size

{
    LPVOID lpMemRes;

    lpMemRes =
#ifdef DEBUG
      MyHeapAlloc (GetProcessHeap (),
                   0
                 | HEAP_ZERO_MEMORY
                 | HEAP_GENERATE_EXCEPTIONS
                   ,
                   size);
#else
      malloc (size);
#endif
    if (lpMemRes EQ NULL)
    {
#ifdef DEBUG
        APLINT aplSize = size;

        dprintfWL0 (L"###Heap allocation failure:  size = %I64u", aplSize);
#endif
        longjmp (heapFull, 1);
    } else
    {
#ifdef DEBUG
        if (gDbgLvl >= gVfpLvl)
        {
            WCHAR wszTemp[256];

            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                       L"MPFNS(Alloc):    %p (%u)",
                        lpMemRes,
                        size);
            DbgMsgW (wszTemp);
        } // End IF
#endif
    } // End IF/ELSE

    return lpMemRes;
} // End mp_alloc


//***************************************************************************
//  $mp_realloc
//
//  Multi Precision Reallocation function
//***************************************************************************

LPVOID mp_realloc
    (LPVOID lpMem,          // Ptr to start of memory block
     size_t old_size,       // Old size
     size_t new_size)       // New size

{
    LPVOID lpMemRes;

    lpMemRes =
#ifdef DEBUG
      MyHeapReAlloc (GetProcessHeap (),
                     0
                   | HEAP_ZERO_MEMORY
                   | HEAP_GENERATE_EXCEPTIONS
                     ,
                     lpMem,
                     new_size);
#else
      realloc (lpMem, new_size);
#endif
    if (lpMemRes EQ NULL)
    {
#ifdef DEBUG
        APLINT aplOldSize = old_size,
               aplUseSize = malloc_usable_size (lpMem),
               aplNewSize = new_size;

        dprintfWL0 (L"###Heap re-allocation failure:  OldSize = %I64u, UseSize = %I64u, NewSize = %I64u", aplOldSize, aplUseSize, aplNewSize);
#endif
        longjmp (heapFull, 2);
    } else
    {
#ifdef DEBUG
        if (gDbgLvl >= gVfpLvl)
        {
            WCHAR wszTemp[256];

            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                       L"MPFNS(ReAlloc):  %p (%u) to %p (%u)",
                        lpMem,
                        old_size,
                        lpMemRes,
                        new_size);
            DbgMsgW (wszTemp);
        } // End IF
#endif
    } // End IF/ELSE

    return lpMemRes;
} // End mp_realloc


//***************************************************************************
//  $mp_free
//
//  Multi Precision Free function
//***************************************************************************

void mp_free
    (LPVOID lpMem,          // Ptr to start of memory block
     size_t size)           // Size         ...

{
#ifdef DEBUG
    MyHeapFree (GetProcessHeap (),
                0,
                lpMem);
#else
    free (lpMem);
#endif
#ifdef DEBUG
    if (gDbgLvl >= gVfpLvl)
    {
        WCHAR wszTemp[256];

        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"MPFNS(Free):     %p (%u)",
                    lpMem,
                    size);
        DbgMsgW (wszTemp);
    } // End IF
#endif
} // End mp_free


//***************************************************************************
//  $mpz_invalid
//***************************************************************************

mpir_ui mpz_invalid
    (enum MP_ENUM mp_enum,
     mpz_t   rop,
     mpz_t   op1,
     mpz_t   op2,
     mpz_t   op3,
     mpir_ui a,
     mpir_ui b)

{
    UBOOL bSameSign;            // TRUE iff the infinities are the same sign

    // Split cases based upon the MP_ENUM value
    switch (mp_enum)
    {
        case MP_ADD:        // op1 and op2 are infinity of opposite signs
        case MP_SUB:        // op1 and op2 are infinite of the same sign
            mpz_QuadICValue (op1,
                             ICNDX_InfSUBInf,
                             op2,
                             rop,
                             FALSE);
            break;

        case MP_DIV:        // op1 and op2 are infinite
            bSameSign = (mpz_sgn (op1) EQ mpz_sgn (op2));

            mpz_QuadICValue (op1,
                             bSameSign ? ICNDX_PiDIVPi
                                       : ICNDX_NiDIVPi,
                             op2,
                             rop,
                             FALSE);
            break;

        case MP_MUL             :
        case MP_CDIV_Q          :
        case MP_CDIV_R          :
        case MP_CDIV_QR         :
        case MP_CDIV_R_UI       :
        case MP_CDIV_QR_UI      :
        case MP_CDIV_R_2EXP     :
        case MP_FDIV_Q          :
        case MP_FDIV_R          :
        case MP_FDIV_QR         :
        case MP_FDIV_R_UI       :
        case MP_FDIV_QR_UI      :
        case MP_FDIV_R_2EXP     :
        case MP_TDIV_Q          :
        case MP_TDIV_R          :
        case MP_TDIV_QR         :
        case MP_TDIV_R_UI       :
        case MP_TDIV_QR_UI      :
        case MP_TDIV_R_2EXP     :
        case MP_SQRT            :
        case MP_RELDIFF         :
        case MP_AND             :
        case MP_IOR             :
        case MP_XOR             :
        case MP_COM             :
        case MP_GCD             :
        case MP_LCM             :
        case MP_POW_UI          :
        case MP_UI_POW_UI       :
        case MP_PosMODNi        :
        case MP_NegMODNi        :
        case MP_PosMODPi        :
        case MP_NegMODPi        :
        case MP_NiMODPos        :
        case MP_NiMODNeg        :
        case MP_PiMODPos        :
        case MP_PiMODNeg        :
        case MP_MODUINi         :
        case MP_MODUIPi         :
        case MP_DIVISIBLE_P     :
        case MP_DIVISIBLE_UI_P  :
        case MP_DIVISIBLE_2EXP_P:
            DbgStop ();             // ***FIXME***

            break;

        defstop
            break;
    } // End SWITCH

    return 0;       // To keep the compiler happy
} // End mpz_invalid


//***************************************************************************
//  $mpq_invalid
//***************************************************************************

mpir_ui mpq_invalid
    (enum MP_ENUM mp_enum,
     mpq_t   rop,
     mpq_t   op1,
     mpq_t   op2,
     mpq_t   op3,
     mpir_ui d)

{
    UBOOL bSameSign;            // TRUE iff the infinities are the same sign

    // Split cases based upon the MP_ENUM value
    switch (mp_enum)
    {
        case MP_ADD:        // op1 and op2 are infinity of opposite signs
        case MP_SUB:        // op1 and op2 are infinite of the same sign
            mpq_QuadICValue (op1,
                             ICNDX_InfSUBInf,
                             op2,
                             rop,
                             FALSE);
            break;

        case MP_DIV:        // op1 and op2 are infinite
            bSameSign = (mpq_sgn (op1) EQ mpq_sgn (op2));

            mpq_QuadICValue (op1,
                             bSameSign ? ICNDX_PiDIVPi
                                       : ICNDX_NiDIVPi,
                             op2,
                             rop,
                             FALSE);
            break;

        defstop
            break;
    } // End SWITCH

    return 0;       // To keep the compiler happy
} // End mpq_invalid


//***************************************************************************
//  $mpfr_invalid
//***************************************************************************

mpir_ui mpfr_invalid
    (enum MP_ENUM mp_enum,
     mpfr_t  rop,
     mpfr_t  op1,
     mpfr_t  op2,
     mpfr_t  op3,
     mpir_ui d)

{
    UBOOL bSameSign;            // TRUE iff the infinities are the same sign

    // Split cases based upon the MP_ENUM value
    switch (mp_enum)
    {
        case MP_ADD:        // op1 and op2 are infinity of opposite signs
        case MP_SUB:        // op1 and op2 are infinite of the same sign
            mpfr_QuadICValue (op1,
                              ICNDX_InfSUBInf,
                              op2,
                              rop,
                              FALSE);
            break;

        case MP_DIV:        // op1 and op2 are infinite
            bSameSign = (mpfr_sgn (op1) EQ mpfr_sgn (op2));

            mpfr_QuadICValue (op1,
                              bSameSign ? ICNDX_PiDIVPi
                                        : ICNDX_NiDIVPi,
                              op2,
                              rop,
                              FALSE);
            break;

        case MP_RELDIFF:    // op1 is infinite and op2 might be, too
                            // Returns (abs (op1 - op2)) / op1
            mpfr_sub (rop, op1, op2, MPFR_RNDN);
            mpfr_abs (rop, rop,      MPFR_RNDN);
            mpfr_div (rop, rop, op1, MPFR_RNDN);

            break;

        case MP_SQRT:       // op1 is negative infinity
                            // Handled in <PrimFnMonRootVisV> and
                            //            <PrimFnMonRootVisVvV>
            Myf_clear (op1);

            NONCE_RE        // sqrt (-Inf)

            break;

        defstop
            break;
    } // End SWITCH

    return 0;       // To keep the compiler happy
} // End mpfr_invalid


//***************************************************************************
//  $mpfr_exact+p
//
//  Return TRUE iff the input is "exact"
//***************************************************************************

UBOOL mpfr_exact_p
    (mpfr_t x)

{
    mpfr_exp_t  exp;        // The Exponent
    mpfr_prec_t uPrec;      // The input's precision
    UBOOL       bRet;       // The result

    // Handle special cases (Zero, NaN, Inf)
    if (!mpfr_regular_p (x))
        return TRUE;

    // Get the input precision
    uPrec = mpfr_get_prec (x);

    // Get the number's binary Exponent
    exp = mpfr_get_exp (x);

    // We consider a MPFR number to be "exact"
    //   if abs (exp) < (uPrec - 10), that is
    //   if the absolute value of its binary exponent (exp) is <
    //      10 less than the number's precision
#define FUDGE   10      // An arbitrary choice subject to review
    uPrec = max (uPrec, FUDGE) - FUDGE;
#undef  FUDGE
    bRet = (abs (exp) < uPrec);

    return bRet;
} // End mpfr_exact_p


//***************************************************************************
//  MPZ Functions
//***************************************************************************

//***************************************************************************
//  $mpz_QuadICValue
//
//  Return the appropriate []IC value
//***************************************************************************

LPAPLMPI mpz_QuadICValue
    (LPAPLMPI   aplMpiLft,          // Left arg
     IC_INDICES icIndex,            // []IC index
     LPAPLMPI   aplMpiRht,          // Right arg
     LPAPLMPI   mpzRes,             // Result
     UBOOL      bNegate)            // TRUE iff we should negate result

{
    switch (GetQuadICValue (icIndex))
    {
        case ICVAL_NEG1:
            // Initialize the result to -1
            mpz_init_set_si (mpzRes, -1);

            break;

        case ICVAL_ZERO:
            // Initialize the result to 0
            mpz_init (mpzRes);

            break;

        case ICVAL_ONE:
            // Initialize the result to 1
            mpz_init_set_si (mpzRes,  1);

            break;

        case ICVAL_DOMAIN_ERROR:
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        case ICVAL_POS_INFINITY:
            mpz_init_set (mpzRes, &mpzPosInfinity);

            break;

        case ICVAL_NEG_INFINITY:
            mpz_init_set (mpzRes, &mpzNegInfinity);

            break;

        case ICVAL_NAN:
            // Initialize to a NaN
            mpz_set_nan (mpzRes);

            break;

        case ICVAL_LEFT:
            mpz_init_set (mpzRes, aplMpiLft);

            break;

        case ICVAL_RIGHT:
            mpz_init_set (mpzRes, aplMpiRht);

            break;

        defstop
            break;
    } // End SWITCH

    // If we should negate, ...
    if (bNegate)
        mpz_neg (mpzRes, mpzRes);

    return mpzRes;
} // End mpz_QuadICValue


//***************************************************************************
//  $mpz_init_set_fr
//
//  Save an APLVFP value as an MP Integer
//***************************************************************************

void mpz_init_set_fr
    (mpz_ptr  dest,
     LPAPLVFP val)

{
    mpz_init (dest);
    mpz_set_fr (dest, val, MPFR_RNDN);
} // End mpz_init_set_fr


#if FALSE
//***************************************************************************
//  $mpz_get_sa
//
//  Convert an APLMPI to an APLINT
//***************************************************************************

APLINT mpz_get_sa
    (mpz_ptr src,               // Ptr to source value
     LPUBOOL lpbRet)            // Ptr to TRUE iff result is valid (may be NULL)

{
    UBOOL        bRet;
    APLUINTUNION dwSplit;

    if (lpbRet EQ NULL)
        lpbRet = &bRet;
    // Check the range
    *lpbRet = (0 <= mpz_cmp (src, &mpzMinInt)
            &&      mpz_cmp (src, &mpzMaxInt) <= 0);
#if GMP_LIMB_BITS == 32
    dwSplit.dwords[0] = mpz_getlimbn (src, 0);
    dwSplit.dwords[1] = mpz_getlimbn (src, 1);
#else
    dwSplit.aplInt    = mpz_getlimbn (src, 0);
#endif
    if (mpz_sgn (src) < 0)
        return -dwSplit.aplInt;
    else
        return  dwSplit.aplInt;
} // End mpz_get_sa
#endif


//***************************************************************************
//  $mpz_cmp_sx
//
//  Compare an MP Integer with an APLINT value
//***************************************************************************

int mpz_cmp_sx
    (mpz_ptr dest,
     APLINT  val)

{
    MP_INT aplTmp = {0};
    int    iRes;

    mpz_init_set_sx (&aplTmp, val);
    iRes = mpz_cmp (dest, &aplTmp);
    mpz_clear (&aplTmp);

    return iRes;
} // End mpz_cmp_sx


//***************************************************************************
//  $mpz_fits_sx_p
//
//  Does the arg fit in a 64-bit signed integer?
//***************************************************************************

UBOOL mpz_fits_sx_p
    (LPAPLMPI lpaplMPI)             // The number to check

{
    return (mpz_cmp_sx (lpaplMPI, MAX_APLINT) <= 0
         && mpz_cmp_sx (lpaplMPI, MIN_APLINT) >= 0);
} // End mpz_fits_sx_p


//***************************************************************************
//  $mpz_next_prime
//
//  Return the value of the next likely prime
//***************************************************************************

UBOOL mpz_next_prime
    (LPAPLMPI      mpzRes,          // The result
     LPAPLMPI      mpzArg,          // The starting number
     LPUBOOL       lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPPERTABDATA  lpMemPTD)        // Ptr to PerTabData global memory

{
    // Copy the arg
    mpz_set (mpzRes, mpzArg);

    // Increment to the previous odd number
    mpz_add_ui (mpzRes, mpzRes, mpz_odd_p (mpzRes) ? 2 : 1);

    while (!mpz_likely_prime_p (mpzRes, lpMemPTD->randStateGMP, 0))
    {
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        mpz_add_ui (mpzRes, mpzRes, 2);
    } // End WHILE

    return TRUE;
ERROR_EXIT:
    return FALSE;
} // End mpz_next_prime


//***************************************************************************
//  $mpz_prev_prime
//
//  Return the value of the previous likely prime
//***************************************************************************

UBOOL mpz_prev_prime
    (LPAPLMPI      mpzRes,          // The result
     LPAPLMPI      mpzArg,          // The starting number
     LPUBOOL       lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPPERTABDATA  lpMemPTD)        // Ptr to PerTabData global memory

{
    // Copy the arg
    mpz_set (mpzRes, mpzArg);

    // Decrement to the previous odd number
    mpz_sub_ui (mpzRes, mpzRes, mpz_odd_p (mpzRes) ? 2 : 1);

    while (!mpz_likely_prime_p (mpzRes, lpMemPTD->randStateGMP, 0))
    {
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        mpz_sub_ui (mpzRes, mpzRes, 2);
    } // End WHILE

    return TRUE;
ERROR_EXIT:
    return FALSE;
} // End mpz_prev_prime


//***************************************************************************
//  $Myz_init
//***************************************************************************

void Myz_init
    (LPAPLMPI mpzVal)

{
#ifdef DEBUG
    if (mpzVal->_mp_d NE NULL)
        DbgStop ();
#endif
    mpz_init (mpzVal);
} // End Myz_init


//***************************************************************************
//  $Myz_clear
//***************************************************************************

void Myz_clear
    (MP_INT *mpzVal)

{
    if (mpzVal->_mp_d NE NULL)
    {
        mpz_clear (mpzVal);
        mpzVal->_mp_d = NULL;
    } // End IF
} // End Myz_clear


//***************************************************************************
//  MPQ Functions
//***************************************************************************

//***************************************************************************
//  $mpq_QuadICValue
//
//  Return the appropriate []IC value
//***************************************************************************

LPAPLRAT mpq_QuadICValue
    (LPAPLRAT   aplRatLft,          // Left arg
     IC_INDICES icIndex,            // []IC index
     LPAPLRAT   aplRatRht,          // Right arg
     LPAPLRAT   mpqRes,             // Result
     UBOOL      bNegate)            // TRUE iff we should negate result

{
    switch (GetQuadICValue (icIndex))
    {
        case ICVAL_NEG1:
            // Initialize the result to -1
            mpq_init_set_si (mpqRes, -1, 1);

            break;

        case ICVAL_ZERO:
            // Initialize the result to 0
            mpq_init (mpqRes);

            break;

        case ICVAL_ONE:
            // Initialize the result to 1
            mpq_init_set_si (mpqRes,  1, 1);

            break;

        case ICVAL_DOMAIN_ERROR:
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        case ICVAL_POS_INFINITY:
            mpq_init_set (mpqRes, &mpqPosInfinity);

            break;

        case ICVAL_NEG_INFINITY:
            mpq_init_set (mpqRes, &mpqNegInfinity);

            break;

        case ICVAL_NAN:
            // Initialize to a NaN
            mpq_set_nan (mpqRes);

            break;

        case ICVAL_LEFT:
            mpq_init_set (mpqRes, aplRatLft);

            break;

        case ICVAL_RIGHT:
            mpq_init_set (mpqRes, aplRatRht);

            break;

        defstop
            break;
    } // End SWITCH

    // If we should negate, ...
    if (bNegate)
        mpq_neg (mpqRes, mpqRes);

    return mpqRes;
} // End mpq_QuadICValue


//***************************************************************************
//  $mpq_init_set
//
//  Save a Rational value as Rational
//***************************************************************************

void mpq_init_set
    (mpq_ptr dest,
     mpq_ptr val)

{
    mpq_init (dest);
    mpq_set (dest, val);
} // End mpq_init_set


//***************************************************************************
//  $mpq_init_set_sx
//
//  Save an APLINT value as a Rational
//***************************************************************************

void mpq_init_set_sx
    (mpq_ptr dest,
     APLINT  num,
     APLUINT den)

{
    mpq_init (dest);
    mpq_set_sx (dest, num, den);
} // End mpq_init_set_sx


//***************************************************************************
//  $mpq_init_set_si
//
//  Save an int value as a Rational
//***************************************************************************

void mpq_init_set_si
    (mpq_ptr dest,
     int     num,
     UINT    den)

{
    mpq_init (dest);
    mpq_set_si (dest, num, den);
} // End mpq_init_set_si


//***************************************************************************
//  $mpq_init_set_ui
//
//  Save an unsigned int value as a Rational
//***************************************************************************

void mpq_init_set_ui
    (mpq_ptr dest,
     mpir_ui num,
     mpir_ui den)

{
    mpq_init (dest);
    mpq_set_ui (dest, num, den);
} // End mpq_init_set_si


//***************************************************************************
//  $mpq_init_set_d
//
//  Save an APLFLOAT value as a Rational
//***************************************************************************

void mpq_init_set_d
    (mpq_ptr  dest,
     APLFLOAT val)

{
    mpq_init (dest);
    mpq_set_d (dest, val);
} // End mpq_init_set_d


//***************************************************************************
//  $mpq_init_set_fr
//
//  Save a VFP value as a Rational
//***************************************************************************

void mpq_init_set_fr
    (mpq_ptr  dest,
     mpfr_ptr src)

{
    mpf_t mpfTmp = {0};

    // Initialize the dest & temp
    mpq_init (dest);
    mpf_init (mpfTmp);

    // Convert the MPFR to an MPF
    mpfr_get_f (mpfTmp, src, MPFR_RNDN);

    // Convert the MPF to a RAT
    mpq_set_f (dest, mpfTmp);

    // We no longer need this storage
    mpf_clear (mpfTmp);
} // End mpq_init_set_fr


//***************************************************************************
//  $mpq_init_set_arb
//
//  Save a ARB value as a Rational
//***************************************************************************

void mpq_init_set_arb
    (mpq_ptr dest,
     arb_ptr src)

{
    mpf_t  mpfTmp = {0};
    APLVFP aplVfp = {0};

    // Initialize the dest & temp
    mpq_init  (dest);
    mpf_init  (mpfTmp);

    // Convert the ARB to an MPFR
    aplVfp = arb_get_mpfr (src);

    // Convert the MPFR to an MPF
    mpfr_get_f (mpfTmp, &aplVfp, MPFR_RNDN);

    // Convert the MPF to a RAT
    mpq_set_f (dest, mpfTmp);

    // We no longer need this storage
    mpf_clear  (mpfTmp);
    mpfr_clear (&aplVfp);
} // End mpq_init_set_arb


//***************************************************************************
//  $mpq_init_set_z
//
//  Save a MPI value as a Rational
//***************************************************************************

void mpq_init_set_z
    (mpq_ptr dest,
     mpz_ptr src)

{
    mpq_init (dest);
    mpq_set_z (dest, src);
} // End mpq_init_set_z


//***************************************************************************
//  $mpq_init_set_str
//
//  Save a string value as a Rational
//***************************************************************************

void mpq_init_set_str
    (mpq_ptr dest,
     char   *str,
     int     base)

{
    mpq_init (dest);
    mpq_set_str (dest, str, base);
} // End mpq_init_set_str


//***************************************************************************
//  $mpq_get_sx
//
//  Convert an APLRAT to an APLINT
//***************************************************************************

APLINT mpq_get_sx
    (mpq_ptr src,           // Ptr to source value
     LPUBOOL lpbRet)        // TRUE iff the result is valid (may be NULL)

{
    APLMPI mpzDiv = {0};
    APLINT aplInteger;
    UBOOL  bRet;

    if (lpbRet EQ NULL)
        lpbRet = &bRet;
    // Check the range
    *lpbRet = (0 <= mpz_cmp (mpq_numref (src), &mpzMinInt)
            &&      mpz_cmp (mpq_numref (src), &mpzMaxInt) <= 0)
            && mpq_integer_p (src);
    // Initialize the quotient
    mpz_init (&mpzDiv);

    // Divide the numerator by the denominator
    mpz_fdiv_q (&mpzDiv, mpq_numref (src), mpq_denref (src));

    // Convert the MPI to an integer
    aplInteger = mpz_get_sx (&mpzDiv);

    // We no longer need this storage
    Myz_clear (&mpzDiv);

    return aplInteger;
} // End mpq_get_sx


//***************************************************************************
//  $mpq_get_ctsx
//
//  Convert an APLRAT to an APLINT within []CT
//***************************************************************************

APLINT _mpq_get_ctsx
    (mpq_ptr  src,          // Ptr to source value
     APLFLOAT fQuadCT,      // []CT
     LPUBOOL  lpbRet,       // TRUE iff the result is valid (may be NULL)
     UBOOL    bIntegerTest) // TRUE iff this is an integer test

{
    APLVFP mpfSrc  = {0};
    APLINT aplInt;

    // Initialize and convert the RAT to a VFP
    mpfr_init_set_q (&mpfSrc, src, MPFR_RNDN);

    // Use the MPFR routine
    aplInt = _mpfr_get_ctsx (&mpfSrc, fQuadCT, lpbRet, bIntegerTest);

    // We no longer need this storage
    Myf_clear (&mpfSrc);

    return aplInt;
} // End _mpq_get_ctsx


//***************************************************************************
//  $mpq_set_sx
//
//  Save an APLINT value as a Rational
//***************************************************************************

void mpq_set_sx
    (mpq_ptr dest,
     APLINT  num,
     APLUINT den)

{
    MP_INT mpzNum = {0},
           mpzDen = {0};

    Assert (den NE 0);

    // If the numerator is zero, ...
    if (num EQ 0)
        mpq_set_ui (dest, 0, 1);
    else
    {
        // Initialize the numerator and denominator
        mpz_set_sx (mpq_numref (dest), num);
        mpz_set_sx (mpq_denref (dest), den);

        // Only if we need to canonicalize, ...
        if (den NE 1)
            // Canonicalize the Rational
            mpq_canonicalize (dest);
    } // End IF
} // End mpq_set_sx


//***************************************************************************
//  $mpq_sub_ui
//***************************************************************************

void mpq_sub_ui
    (mpq_ptr dest,
     mpq_ptr src,
     mpir_ui num,
     mpir_ui den)

{
    APLRAT mpqTmp = {0};

    // Initialize the temp
    mpq_init (&mpqTmp);

    // Save the UINT as a RAT
    mpq_set_ui (&mpqTmp, num, den);

    // Subtract the UINT
    mpq_sub (dest, src, &mpqTmp);

    // We no longer need this storage
    Myq_clear (&mpqTmp);
} // End mpq_sub_ui


//***************************************************************************
//  $mpq_mul_si
//***************************************************************************

void mpq_mul_si
    (mpq_ptr dest,
     mpq_ptr src,
     mpir_si num,
     mpir_si den)

{
    APLRAT mpqTmp = {0};

    // Initialize the temp
    mpq_init (&mpqTmp);

    // Save the INT as a RAT
    mpq_set_si (&mpqTmp, num, den);

    // Multiply the INT
    mpq_mul (dest, src, &mpqTmp);

    // We no longer need this storage
    Myq_clear (&mpqTmp);
} // End mpq_mul_si


//***************************************************************************
//  $mpq_pow_ui
//***************************************************************************

void mpq_pow_ui
    (mpq_ptr dest,
     mpq_ptr src,
     mpir_ui exp)

{
    // Calculate the pow of the numerator
    mpz_pow_ui (mpq_numref (dest), mpq_numref (src), exp);
    mpz_pow_ui (mpq_denref (dest), mpq_denref (src), exp);

    // Canonicalize the result
    mpq_canonicalize (dest);
} // End mpq_pow_ui


//***************************************************************************
//  $mpq_floor
//***************************************************************************

void mpq_floor
    (mpq_ptr dest,
     mpq_ptr src)

{
    // Divide the numerator by the denominator
    mpz_fdiv_q (mpq_numref (dest), mpq_numref (src), mpq_denref (src));

    // Set the denominator to 1
    mpz_set_ui (mpq_denref (dest), 1);
} // End mpq_floor


//***************************************************************************
//  $mpq_ceil
//***************************************************************************

void mpq_ceil
    (mpq_ptr dest,
     mpq_ptr src)

{
    // Divide the numerator by the denominator
    mpz_cdiv_q (mpq_numref (dest), mpq_numref (src), mpq_denref (src));

    // Set the denominator to 1
    mpz_set_ui (mpq_denref (dest), 1);
} // End mpq_ceil


//***************************************************************************
//  $mpq_mod
//
//  Calculate aplOpr mod (aplMod)
//***************************************************************************

void mpq_mod
    (mpq_ptr dest,          // Destination
     mpq_ptr aplOpr,        // Operand
     mpq_ptr aplMod)        // Modulus

{
    APLRAT aplTmp = {0};

    // Initialize the temp
    mpq_init (&aplTmp);

    mpq_div   (&aplTmp,  aplOpr,  aplMod);      // T = L / R
    mpq_floor (&aplTmp, &aplTmp);               // T = floor (T)
    mpq_mul   (&aplTmp,  aplMod, &aplTmp);      // T = R * T
    mpq_sub   (dest   ,  aplOpr, &aplTmp);      // Z = L - T

    // We no longer need this storage
    Myq_clear (&aplTmp);
} // End mpq_mod


//***************************************************************************
//  $mpq_mod_sx
//
//  Calculate aplOpr % aplMod (a.k.a. aplMod | aplOpr)
//***************************************************************************

void mpq_mod_sx
    (mpq_ptr dest,          // Destination
     mpq_ptr aplOpr,        // Operand
     APLINT  aplMod)        // Modulus

{
    APLRAT aplDiv = {0};

    mpq_init_set_sx (&aplDiv, aplMod, 1);
    mpq_mod         ( dest,   aplOpr, &aplDiv);

    // We no longer need this storage
    Myq_clear (&aplDiv);
} // End mpq_mod_sx


//***************************************************************************
//  $mpq_cmp_abs
//
//  Compare the absolute value of two RATs
//
//  Return +1 if Lft >  Rht
//          0 if Lft EQ Rht
//         -1 if Lft <  Rht
//***************************************************************************

int mpq_cmpabs
    (LPAPLRAT lpaplRatLft,      // Left arg
     LPAPLRAT lpaplRatRht)      // Right arg

{
    APLRAT aplAbsLft = {0},
           aplAbsRht = {0};
    int    iRet;

    // Copy the left arg and compute its absolute value
    mpq_init_set (&aplAbsLft, lpaplRatLft);
    mpq_abs      (&aplAbsLft,  &aplAbsLft);

    // Copy the right arg and compute its absolute value
    mpq_init_set (&aplAbsRht, lpaplRatRht);
    mpq_abs      (&aplAbsRht,  &aplAbsRht);

    // Compare the absolute values
    iRet = mpq_cmp (&aplAbsLft, &aplAbsRht);

    // We no longer need this storage
    Myq_clear (&aplAbsRht);
    Myq_clear (&aplAbsLft);

    return iRet;
} // End mpq_cmpabs


//***************************************************************************
//  $_mpq_cmp_ct
//
//  Compare two RATs relative to a given comparison tolerance
//
//  Return +1 if Lft >  Rht
//          0 if Lft EQ Rht
//         -1 if Lft <  Rht
//***************************************************************************

int mpq_cmp_ct
    (LPAPLRAT lpaplLft,             // Ptr to left arg
     LPAPLRAT lpaplRht,             // ...    right arg
     APLFLOAT fQuadCT,              // []CT
     UBOOL    bIntegerTest)         // TRUE iff this is an integer test

{
    int iRet;                       // Result of mpq_cmp

////#define CT_DEBUG

#if defined (DEBUG) && defined (CT_DEBUG)
    WCHAR  wszTemp[1024];
    APLVFP mpfFmt = {0};

    strcpyW (wszTemp, L"Lft: "); *FormatAplRat (&wszTemp[lstrlenW (wszTemp)], lpaplLft) = WC_EOS; DbgMsgW (wszTemp);
    strcpyW (wszTemp, L"Rht: "); *FormatAplRat (&wszTemp[lstrlenW (wszTemp)], lpaplRht) = WC_EOS; DbgMsgW (wszTemp);
#endif
    // Compare 'em without tolerance
    iRet = mpq_cmp (lpaplLft, lpaplRht);

    // If args are unequal,
    //   and []CT is non-zero,
    //   and neither arg is either infinity, ...
    if (iRet NE 0                               // Lft NE Rht
     && fQuadCT NE 0                            // []CT NE 0
     && !IsMpqInfinity (lpaplLft)
     && !IsMpqInfinity (lpaplRht))
    {
        APLRAT mpqLftAbs = {0},     // Absolute value of left arg
               mpqRhtAbs = {0},     // ...               right ...
               mpqSYS_CT = {0};     // SYS_CT as an mpq
        int    sgnLft,              // Left arg sign
               sgnRht;              // Right ...

        // Use an algorithm similar to the one in flt_cmp_ct

        // Initialize the temps to 0/1
        mpq_init (&mpqLftAbs);
        mpq_init (&mpqRhtAbs);

        // Initialize mpq_SYS_CT
        mpq_init_set_d (&mpqSYS_CT, SYS_CT);

        // Get the signs
        sgnLft = signumrat (lpaplLft);
        sgnRht = signumrat (lpaplRht);

        // Get the absolute values
        mpq_abs (&mpqLftAbs, lpaplLft);
        mpq_abs (&mpqRhtAbs, lpaplRht);

        // If this is an integer test, allow comparison with zero
        if (bIntegerTest
         && sgnLft EQ 0
         && mpq_cmp (&mpqRhtAbs, &mpqSYS_CT) <= 0)
            iRet = 0;
        else
        if (bIntegerTest
         && sgnRht EQ 0
         && mpq_cmp (&mpqLftAbs, &mpqSYS_CT) <= 0)
            iRet = 0;
        else
        if (!IsMpq0 (lpaplLft)          // Lft NE 0
         && !IsMpq0 (lpaplRht)          // Rht NE 0
         && sgnLft EQ sgnRht)           // Signs are the same, ...
        {
            APLRAT mpqCT     = {0},
                   mpqHoodLo = {0};

            // Initialize the temps to 0/1
            mpq_init (&mpqCT);
            mpq_init (&mpqHoodLo);

            // Convert the comparison tolerance
            mpq_set_d (&mpqCT, fQuadCT);
#if defined (DEBUG) && defined (CT_DEBUG)
            strcpyW (wszTemp, L"CT:  "); *FormatAplRat (&wszTemp[lstrlenW (wszTemp)], &mpqCT) = WC_EOS; DbgMsgW (wszTemp);
#endif
            // Calculate the low end of the left neighborhood of (|Rht)
////////////aplHoodLo = aplRhtAbs - aplRhtAbs * fQuadCT;
            mpq_mul (&mpqHoodLo, &mpqRhtAbs, &mpqCT    );
            mpq_sub (&mpqHoodLo, &mpqRhtAbs, &mpqHoodLo);
#if defined (DEBUG) && defined (CT_DEBUG)
            strcpyW (wszTemp, L"Lo1: "); *FormatAplRat (&wszTemp[lstrlenW (wszTemp)], &mpqHoodLo) = WC_EOS; DbgMsgW (wszTemp);
            strcpyW (wszTemp, L"L1 : "); *FormatAplRat (&wszTemp[lstrlenW (wszTemp)], &mpqLftAbs) = WC_EOS; DbgMsgW (wszTemp);
            strcpyW (wszTemp, L"R1 : "); *FormatAplRat (&wszTemp[lstrlenW (wszTemp)], &mpqRhtAbs) = WC_EOS; DbgMsgW (wszTemp);
#endif
            // If (|Rht) is greater than (|Lft),
            // and (|Lft) is in the
            //    left-neighborhood of (|Rht) with CT, return 0 (equal)
////////////if (aplHoodLo <= aplLftAbs
//////////// &&              aplLftAbs < aplRhtAbs)
////////////    return TRUE;
            if (mpq_cmp (&mpqHoodLo, &mpqLftAbs)             <= 0
             && mpq_cmp (            &mpqLftAbs, &mpqRhtAbs) <  0)
                iRet = 0;
            else
            {
                // Calculate the low end of the left neighborhood of (|Lft)
////////////////aplHoodLo = aplLftAbs - aplLftAbs * fQuadCT;
                mpq_mul (&mpqHoodLo, &mpqLftAbs, &mpqCT    );
                mpq_sub (&mpqHoodLo, &mpqLftAbs, &mpqHoodLo);
#if defined (DEBUG) && defined (CT_DEBUG)
            strcpyW (wszTemp, L"Lo2: "); *FormatAplRat (&wszTemp[lstrlenW (wszTemp)], &mpqHoodLo) = WC_EOS; DbgMsgW (wszTemp);
            strcpyW (wszTemp, L"R2 : "); *FormatAplRat (&wszTemp[lstrlenW (wszTemp)], &mpqRhtAbs) = WC_EOS; DbgMsgW (wszTemp);
            strcpyW (wszTemp, L"L2 : "); *FormatAplRat (&wszTemp[lstrlenW (wszTemp)], &mpqLftAbs) = WC_EOS; DbgMsgW (wszTemp);
#endif
                // If (|Lft) is greater than (|Rht),
                // and (|Rht) is in the
                //    left-neighborhood of (|Lft) with CT, return 0 (equal)
////////////////if (aplHoodLo <= aplRhtAbs
//////////////// &&              aplRhtAbs < aplLftAbs)
////////////////    return TRUE;
                if (mpq_cmp (&mpqHoodLo, &mpqRhtAbs)             <= 0
                 && mpq_cmp (            &mpqRhtAbs, &mpqLftAbs) <  0)
                    iRet = 0;
            } // End IF/ELSE

            // We no longer need this storage
            Myq_clear (&mpqHoodLo);
            Myq_clear (&mpqCT    );
        } // End IF/ELSE

        Myq_clear (&mpqRhtAbs);
        Myq_clear (&mpqLftAbs);
    } // End IF

    return iRet;
} // End mpq_cmp_ct


//***************************************************************************
//  $Myq_init
//***************************************************************************

void Myq_init
    (LPAPLRAT mpqVal)

{
#ifdef DEBUG
    if (mpqVal->_mp_num._mp_d NE NULL)
        DbgStop ();
#endif
    mpq_init (mpqVal);
} // End Myq_init


//***************************************************************************
//  $Myq_clear
//
//  Free a RAT unless already clear
//***************************************************************************

void Myq_clear
    (LPAPLRAT mpqVal)

{
    if (mpq_numref (mpqVal)->_mp_d NE NULL)
    {
        mpz_clear (mpq_numref (mpqVal));
        mpqVal->_mp_num._mp_d = NULL;
    } // End IF

    if (mpq_denref (mpqVal)->_mp_d NE NULL)
    {
        mpz_clear (mpq_denref (mpqVal));
        mpqVal->_mp_den._mp_d = NULL;
    } // End IF
} // End Myq_clear


//***************************************************************************
//  $mpq_integer_p
//
//  Return TRUE iff the numerator of op is evenly divisible by its denominator
//***************************************************************************

int mpq_integer_p
    (mpq_t op)                  // Source

{
    if (mpq_inf_p (op))
        return TRUE;
    else
        return mpz_divisible_p (mpq_numref (op), mpq_denref (op));
} // mpq_integer_p


//***************************************************************************
//  MPFR Functions
//***************************************************************************

//***************************************************************************
//  $mpfr_QuadICValue
//
//  Return the appropriate []IC value
//***************************************************************************

LPAPLVFP mpfr_QuadICValue
    (LPAPLVFP   aplVfpLft,          // Left arg
     IC_INDICES icIndex,            // []IC index
     LPAPLVFP   aplVfpRht,          // Right arg
     LPAPLVFP   mpfRes,             // Result
     UBOOL      bNegate)            // TRUE iff we should negate result

{
    switch (GetQuadICValue (icIndex))
    {
        case ICVAL_NEG1:
            // Initialize the result to -1
            mpfr_init_set_si (mpfRes, -1, MPFR_RNDN);

            break;

        case ICVAL_ZERO:
            // Initialize the result to 0
            mpfr_init0 (mpfRes);

            break;

        case ICVAL_ONE:
            // Initialize the result to 1
            mpfr_init_set_si (mpfRes,  1, MPFR_RNDN);

            break;

        case ICVAL_DOMAIN_ERROR:
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        case ICVAL_POS_INFINITY:
            mpfr_init_set (mpfRes, &mpfPosInfinity, MPFR_RNDN);

            break;

        case ICVAL_NEG_INFINITY:
            mpfr_init_set (mpfRes, &mpfNegInfinity, MPFR_RNDN);

            break;

        case ICVAL_NAN:
            // Initialize to a NaN
            mpfr_init (mpfRes);

            break;

        case ICVAL_LEFT:
            mpfr_init_copy (mpfRes, aplVfpLft);

            break;

        case ICVAL_RIGHT:
            mpfr_init_copy (mpfRes, aplVfpRht);

            break;

        defstop
            break;
    } // End SWITCH

    // If we should negate, ...
    if (bNegate)
        mpfr_neg (mpfRes, mpfRes, MPFR_RNDN);

    return mpfRes;
} // End mpfr_QuadICValue


//***************************************************************************
//  $mpfr_copy
//
//  Save an MPFR value as a Variable FP with the same precision
//***************************************************************************

void mpfr_copy
    (mpfr_ptr dest,
     mpfr_ptr val)

{
    // If the source is finite, ...
    if (!mpfr_inf_p (val)
     && !mpfr_nan_p (val))
        mpfr_set_prec (dest, mpfr_get_prec (val));
    mpfr_set (dest, val, MPFR_RNDN);
} // End mpfr_copy


//***************************************************************************
//  $mpfr_init_copy
//
//  Save an MPFR value as a Variable FP with the same precision
//***************************************************************************

void mpfr_init_copy
    (mpfr_ptr dest,
     mpfr_ptr val)

{
    // If the source is finite, ...
    if (!mpfr_inf_p (val)
     && !mpfr_nan_p (val))
        mpfr_init2 (dest, mpfr_get_prec (val));
    else
        mpfr_init  (dest);
    mpfr_set (dest, val, MPFR_RNDN);
} // End mpfr_init_copy


//***************************************************************************
//  $mpfr_init_set
//
//  Save a VFP value as a Variable FP
//***************************************************************************

void mpfr_init_set
    (mpfr_ptr   dest,
     mpfr_ptr   src,
     mpfr_rnd_t rnd)

{
    mpfr_init (dest);       // OK not to be mpfr_init0
    mpfr_set (dest, src, rnd);
} // End mpfr_init_set


//***************************************************************************
//  $mpfr_init_set_sx
//
//  Save an APLINT value as a Variable FP
//***************************************************************************

void mpfr_init_set_sx
    (mpfr_ptr   dest,
     APLINT     val,
     mpfr_rnd_t rnd)

{
    mpfr_init (dest);       // OK not to be mpfr_init0
    mpfr_set_sx (dest, val, rnd);
} // End mpfr_init_set_sx


//***************************************************************************
//  $mpfr_init_set_ui
//
//  Save a UINT value as a Variable FP
//***************************************************************************

void mpfr_init_set_ui
    (mpfr_ptr          dest,
     unsigned long int val,
     mpfr_rnd_t        rnd)

{
    mpfr_init (dest);       // OK not to be mpfr_init0 to avoid recursion
    mpfr_set_ui (dest, val, rnd);
} // End mpfr_init_set_ui


//***************************************************************************
//  $mpfr_init_set_si
//
//  Save an INT value as a Variable FP
//***************************************************************************

void mpfr_init_set_si
    (mpfr_ptr   dest,
     long int   val,
     mpfr_rnd_t rnd)

{
    mpfr_init (dest);       // OK not to be mpfr_init0
    mpfr_set_si (dest, val, rnd);
} // End mpfr_init_set_si


//***************************************************************************
//  $mpfr_init_set_d
//
//  Save a FLT value as a Variable FP
//***************************************************************************

void mpfr_init_set_d
    (mpfr_ptr   dest,
     APLFLOAT   val,
     mpfr_rnd_t rnd)

{
    mpfr_init (dest);       // OK not to be mpfr_init0
    mpfr_set_d (dest, val, rnd);
} // End mpfr_init_set_d


//***************************************************************************
//  $mpfr_init_set_z
//
//  Save a MPZ value as a Variable FP
//***************************************************************************

void mpfr_init_set_z
    (mpfr_ptr   dest,
     LPAPLMPI   val,
     mpfr_rnd_t rnd)

{
    mpfr_init (dest);       // OK not to be mpfr_init0
    mpfr_set_z (dest, val, rnd);
} // End mpfr_init_set_z


//***************************************************************************
//  $mpfr_init_set_q
//
//  Save a RAT value as a Variable FP
//***************************************************************************

void mpfr_init_set_q
    (mpfr_ptr   dest,
     LPAPLRAT   val,
     mpfr_rnd_t rnd)

{
    mpfr_init (dest);       // OK not to be mpfr_init0
    mpfr_set_q (dest, val, rnd);
} // End mpfr_init_set_q


//***************************************************************************
//  $mpfr_get_sx
//
//  Convert a VFP to an APLINT
//***************************************************************************

APLINT mpfr_get_sx
    (mpfr_ptr src,          // Ptr to source value
     LPUBOOL lpbRet)        // TRUE iff the result is valid (may be NULL)

{
     UBOOL bRet;

     if (lpbRet EQ NULL)
         lpbRet = &bRet;
     // See if it fits
     *lpbRet = mpfr_fits_intmax_p (src, MPFR_RNDN);

     // If the fractional part is zero and the integer part is in range, ...
     if (*lpbRet)
         return mpfr_get_sj (src, MPFR_RNDN);
     else
         // Return a known value
         return 0;
} // End mpfr_get_sx


//***************************************************************************
//  $mpfr_get_sctsx
//
//  Convert an APLVFP to an APLINT within system []CT
//***************************************************************************

APLINT mpfr_get_sctsx
    (mpfr_ptr src,          // Ptr to source value
     LPUBOOL  lpbRet)       // TRUE iff the result is valid (may be NULL)

{
    return _mpfr_get_ctsx (src, SYS_CT, lpbRet, TRUE);
} // End mpfr_get_sctsx


//***************************************************************************
//  $mpfr_get_ctsx
//
//  Convert an APLVFP to an APLINT within []CT
//***************************************************************************

APLINT _mpfr_get_ctsx
    (mpfr_ptr src,          // Ptr to source value
     APLFLOAT fQuadCT,      // []CT
     LPUBOOL  lpbRet,       // TRUE iff the result is valid (may be NULL)
     UBOOL    bIntegerTest) // TRUE iff this is an integer test

{
    APLVFP mpfTmp1 = {0},
           mpfTmp2 = {0},
           mpfSrc  = {0};
    APLINT aplInt = 0;
    UBOOL  bRet = TRUE;

    if (lpbRet EQ NULL)
        lpbRet = &bRet;

    // Handle special case of 0 as <mpfr_reldiff> returns 1 (= 0{div}0)
    if (mpfr_zero_p (src))
        *lpbRet = TRUE;
    else
    // Handle special case of infinity as <mpfr_get_sx> fails
    if (mpfr_inf_p (src))
        longjmp (heapFull, 3);
    else
    {
        // Initialize the temps to 0
        mpfr_init0 (&mpfTmp1);
        mpfr_init0 (&mpfTmp2);
        mpfr_init0 (&mpfSrc);

        // Copy the source
        mpfr_copy (&mpfSrc, src);

        // Get the floor
        mpfr_floor (&mpfTmp1, src);

        // Get the ceil
        mpfr_ceil  (&mpfTmp2, src);

        // Compare the number and its floor
        if (mpfr_cmp_ct (&mpfSrc, &mpfTmp1, fQuadCT, bIntegerTest) EQ 0)
            // Return the floor
            aplInt = mpfr_get_sx (&mpfTmp1, lpbRet);
        else
        // Compare the number and its ceiling
        if (mpfr_cmp_ct (&mpfSrc, &mpfTmp2, fQuadCT, bIntegerTest) EQ 0)
            // Return the ceiling
            aplInt = mpfr_get_sx (&mpfTmp2, lpbRet);
        else
        {
            // Return the floor even though it isn't within []CT
            aplInt = mpfr_get_sx (&mpfTmp1, lpbRet);

            // Mark as not within []CT
            *lpbRet = FALSE;
        } // End IF/ELSE/...

        // We no longer need this storage
        Myf_clear (&mpfSrc);
        Myf_clear (&mpfTmp2);
        Myf_clear (&mpfTmp1);
    } // Endf IF/ELSE

    return aplInt;
} // End _mpfr_get_ctsx


//// //***************************************************************************
//// //  $mpfr_get_ux
//// //
//// //  Convert a VFP to an APLUINT
//// //***************************************************************************
////
//// APLUINT mpfr_get_ux
////     (mpfr_ptr src,             // Ptr to source value
////      LPUBOOL lpbRet)           // TRUE iff the result is valid (may be NULL)
////
//// {
////     UBOOL bRet;
////
////     if (lpbRet EQ NULL)
////         lpbRet = &bRet;
////     // See if it fits
////     *lpbRet = mpfr_fits_uintmax_p (src, MPFR_RNDN);
////
////     // If the fractional part is zero and the integer part is in range, ...
////     if (*lpbRet)
////         return mpfr_get_uj (src, MPFR_RNDN);
////     else
////         // Return a known value
////         return 0;
//// } // End mpfr_get_ux


//***************************************************************************
//  $mpfr_mod
//
//  Calculate aplOpr % aplMod (a.k.a. aplMod | aplOpr)
//***************************************************************************

void mpfr_mod
    (mpfr_ptr dest,         // Destination
     mpfr_ptr aplOpr,       // Operand
     mpfr_ptr aplMod)       // Modulus

{
#if FALSE                   // ***FIXME*** -- Check rounding
    mpfr_t mpfrOpr = {0},
           mpfrMod = {0};

    // Copy the Opr & Mod as non-negative numbers
    mpfr_init_copy (mpfrOpr, aplOpr);
    mpfr_init_copy (mpfrMod, aplMod);

    if (mpfr_sgn (mpfrOpr) < 0)
        mpfr_neg (mpfrOpr, mpfrOpr, MPFR_RNDZ);
    if (mpfr_sgn (mpfrMod) < 0)
        mpfr_neg (mpfrMod, mpfrMod, MPFR_RNDZ);

    // Compute the modulus
    mpfr_fmod (dest, mpfrOpr, mpfrMod, MPFR_RNDZ);

    // If the arguments are of opposite sign
    //   and the result so far is non-zero,
    //   replace the result with its complement
    //   in the modulus.
    if ((mpfr_sgn (aplMod) > 0) NE (mpfr_sgn (aplOpr) > 0)
     && mpfr_sgn (dest) NE 0)
        mpfr_sub (dest, aplMod, dest, MPFR_RNDZ);

    // The sign of the result is the sign of the left arg
    if (mpfr_sgn (aplMod) < 0)
        mpfr_neg (dest, dest, MPFR_RNDZ);
#else
    // Due to precision limitations, the result of <mpfr_mod_sub>
    //   might not be less than the modulus, so we iterate until it is
    mpfr_mod_sub (dest, aplOpr, aplMod);
    while (mpfr_cmp_abs (dest, aplMod) > 0)
        mpfr_mod_sub (dest, dest  , aplMod);
#endif
} // End mpfr_mod


//***************************************************************************
//  $mpfr_mod_sx
//
//  Calculate aplOpr % aplMod (a.k.a. aplMod | aplOpr)
//***************************************************************************

void mpfr_mod_sx
    (mpfr_ptr dest,         // Destination
     mpfr_ptr aplOpr,       // Operand
     APLINT   aplMod)       // Modulus

{
    APLVFP aplDiv = {0};

    mpfr_init_set_sx (&aplDiv, aplMod, 1);
    mpfr_mod         ( dest,   aplOpr, &aplDiv);

    // We no longer need this storage
    Myf_clear (&aplDiv);
} // End mpfr_mod_sx


//***************************************************************************
//  $mpfr_mod_sub
//
//  Calculate aplOpr % aplMod (a.k.a. aplMod | aplOpr)
//***************************************************************************

void mpfr_mod_sub
    (mpfr_ptr dest,         // Destination
     mpfr_ptr aplOpr,       // Operand
     mpfr_ptr aplMod)       // Modulus

{
////#define MOD_DEBUG

    APLVFP aplTmp = {0};
#if defined (DEBUG) && defined (MOD_DEBUG)
    char szTemp1[1024],
         szTemp2[1024];
    int  expptr1,
         expptr2;
#endif

    // Initialize the temp to 0
    mpfr_init0 (&aplTmp);

#if defined (DEBUG) && defined (MOD_DEBUG)
    mpfr_get_str (szTemp1, &expptr1, 10, 200, aplOpr, MPFR_RNDN);
    mpfr_get_str (szTemp2, &expptr2, 10, 200, aplMod, MPFR_RNDN);
#endif
    mpfr_div   (&aplTmp,  aplOpr,  aplMod, MPFR_RNDN);
#if defined (DEBUG) && defined (MOD_DEBUG)
    mpfr_get_str (szTemp1, &expptr1, 10, 200, &aplTmp, MPFR_RNDN);
#endif
    mpfr_floor (&aplTmp, &aplTmp);
#if defined (DEBUG) && defined (MOD_DEBUG)
    mpfr_get_str (szTemp1, &expptr1, 10, 200, &aplTmp, MPFR_RNDN);
#endif
    mpfr_mul   (&aplTmp,  aplMod, &aplTmp, MPFR_RNDN);
#if defined (DEBUG) && defined (MOD_DEBUG)
    mpfr_get_str (szTemp1, &expptr1, 10, 200, &aplTmp, MPFR_RNDN);
#endif
    mpfr_sub   (dest   ,  aplOpr, &aplTmp, MPFR_RNDN);
#if defined (DEBUG) && defined (MOD_DEBUG)
    mpfr_get_str (szTemp1, &expptr1, 10, 200, dest, MPFR_RNDN);
#endif

    // We no longer need this storage
    Myf_clear (&aplTmp);
} // End mpfr_mod_sub


//***************************************************************************
//  $mpfr_cmp_ct
//
//  Compare two VFPs relative to a given comparison tolerance
//
//  Return +1 if Lft >  Rht
//          0 if Lft EQ Rht
//         -1 if Lft <  Rht
//***************************************************************************

int mpfr_cmp_ct
    (LPAPLVFP lpaplLft,             // Ptr to left arg
     LPAPLVFP lpaplRht,             // ...    right ...
     APLFLOAT fQuadCT,              // []CT
     UBOOL    bIntegerTest)         // TRUE iff this is an integer test

{
    int iRet;                       // Result of mpfr_cmp

    // Compare 'em without tolerance
    iRet = mpfr_cmp (lpaplLft, lpaplRht);

    // If args are unequal,
    //   and []CT is non-zero,
    //   and neither arg is either infinity, ...
    if (iRet NE 0                               // Lft NE Rht
     && fQuadCT NE 0                            // []CT NE 0
     && !IsMpfInfinity (lpaplLft)
     && !IsMpfInfinity (lpaplRht))
    {
        APLVFP mpfLftAbs = {0},     // Absolute value of left arg
               mpfRhtAbs = {0};     // ...               right ...
        int    sgnLft,              // Left arg sign
               sgnRht;              // Right ...

        // Use an algorithm similar to the one in flt_cmp_ct

        // Initialize the temps to 0
        mpfr_init0 (&mpfLftAbs);
        mpfr_init0 (&mpfRhtAbs);

        // Get the signs
        sgnLft = signumvfp (lpaplLft);
        sgnRht = signumvfp (lpaplRht);

        // Get the absolute values
        mpfr_abs (&mpfLftAbs, lpaplLft, MPFR_RNDN);
        mpfr_abs (&mpfRhtAbs, lpaplRht, MPFR_RNDN);

        // If this is an integer test, allow comparison with zero
        if (bIntegerTest
         && sgnLft EQ 0
         && mpfr_cmp_d (&mpfRhtAbs, SYS_CT) <= 0)
            iRet = 0;
        else
        if (bIntegerTest
         && sgnRht EQ 0
         && mpfr_cmp_d (&mpfLftAbs, SYS_CT) <= 0)
            iRet = 0;
        else
        if (!IsMpf0 (lpaplLft)       // Lft NE 0
         && !IsMpf0 (lpaplRht)       // Rht NE 0
         && sgnLft EQ sgnRht)           // Signs are the same, ...
        {
            APLVFP mpfCT     = {0},
                   mpfHoodLo = {0};

            // Initialize the temps to 0
            mpfr_init0 (&mpfCT);
            mpfr_init0 (&mpfHoodLo);

            // Convert the comparison tolerance
            mpfr_set_d (&mpfCT, fQuadCT, MPFR_RNDN);

            // Calculate the low end of the left neighborhood of (|Rht)
////////////aplHoodLo = aplRhtAbs - aplRhtAbs * fQuadCT;
            mpfr_mul (&mpfHoodLo, &mpfRhtAbs, &mpfCT    , MPFR_RNDN);
            mpfr_sub (&mpfHoodLo, &mpfRhtAbs, &mpfHoodLo, MPFR_RNDN);

            // If (|Rht) is greater than (|Lft),
            // and (|Lft) is in the
            //    left-neighborhood of (|Rht) with CT, return 0 (equal)
////////////if (aplHoodLo <= aplLftAbs
//////////// &&              aplLftAbs < aplRhtAbs)
////////////    return TRUE;
            if (mpfr_cmp (&mpfHoodLo, &mpfLftAbs)             <= 0
             && mpfr_cmp (            &mpfLftAbs, &mpfRhtAbs) <  0)
                iRet = 0;
            else
            {
                // Calculate the low end of the left neighborhood of (|Lft)
////////////////aplHoodLo = aplLftAbs - aplLftAbs * fQuadCT;
                mpfr_mul (&mpfHoodLo, &mpfLftAbs, &mpfCT    , MPFR_RNDN);
                mpfr_sub (&mpfHoodLo, &mpfLftAbs, &mpfHoodLo, MPFR_RNDN);

                // If (|Lft) is greater than (|Rht),
                // and (|Rht) is in the
                //    left-neighborhood of (|Lft) with CT, return 0 (equal)
////////////////if (aplHoodLo <= aplRhtAbs
//////////////// &&              aplRhtAbs < aplLftAbs)
////////////////    return TRUE;
                if (mpfr_cmp (&mpfHoodLo, &mpfRhtAbs)             <= 0
                 && mpfr_cmp (            &mpfRhtAbs, &mpfLftAbs) <  0)
                    iRet = 0;
            } // End IF/ELSE

            // We no longer need this storage
            Myf_clear (&mpfHoodLo);
            Myf_clear (&mpfCT    );
        } // End IF/ELSE

        Myf_clear (&mpfRhtAbs);
        Myf_clear (&mpfLftAbs);
    } // End IF

    return iRet;
} // End mpfr_cmp_ct


//***************************************************************************
//  $mpfr_ui_cmp
//
//  Compare an unsigned long int and a VFP
//***************************************************************************

int mpfr_ui_cmp
    (unsigned long int op1,     // Left arg
     mpfr_t             op2)     // Right arg

{
    return -mpfr_cmp_ui (op2, op1);
} // End mpfr_ui_cmp


//***************************************************************************
//  $mpfr_si_cmp
//
//  Compare a signed long int and a VFP
//***************************************************************************

int mpfr_si_cmp
    (signed long int op1,       // Left arg
     mpfr_t           op2)       // Right arg

{
    return -mpfr_cmp_si (op2, op1);
} // End mpfr_si_cmp


//***************************************************************************
//  $mpfr_bin_ui
//
//  Compute a binomial coefficient as a VFP
//***************************************************************************

void mpfr_bin_ui
    (mpfr_t rop,        // Ptr to result
     mpfr_t op1,        // ...    right arg (in the APL sense of L!R)
     long   op2)        // ...    left arg   ...

{
    mpz_t lpaplRht,
          lpaplRes;

    Assert (mpfr_integer_p (op1));

    // Initialize to 0
    mpz_init (lpaplRht);
    mpz_init (lpaplRes);

    // Convert the right arg to an MPI
    mpfr_get_z (lpaplRht, op1, MPFR_RNDN);

    // Compute the binomial coefficient:  Z = R L
    mpz_bin_ui (lpaplRes, lpaplRht, op2);

    // Convert the result back to VFP
    mpfr_set_z (rop, lpaplRes, MPFR_RNDN);

    mpz_clear (lpaplRht);
    mpz_clear (lpaplRes);
} // End mpfr_bin_ui


//***************************************************************************
//  $Myf_init
//***************************************************************************

void Myf_init
    (LPAPLVFP mpfVal)

{
#ifdef DEBUG
    if (mpfVal->_mpfr_d NE NULL)
        DbgStop ();
#endif
    mpfr_init0 (mpfVal);
} // End Myf_init


//***************************************************************************
//  $Myf_clear
//
//  Free a VFP unless already clear
//***************************************************************************

void Myf_clear
    (LPAPLVFP mpfVal)

{
    if (mpfVal->_mpfr_d NE NULL)
    {
        mpfr_clear (mpfVal);
        mpfVal->_mpfr_d = NULL;
    } // End IF
} // End Myf_clear


//***************************************************************************
//  ARB Functions
//***************************************************************************

//***************************************************************************
//  $arb_get_ctsx
//
//  Convert an APLBA1F to an APLINT within []CT
//***************************************************************************

APLINT _arb_get_ctsx
    (LPAPLBA1F src,             // Ptr to source value
     APLFLOAT  fQuadCT,         // []CT
     LPUBOOL   lpbRet,          // TRUE iff the result is valid (may be NULL)
     UBOOL     bIntegerTest)    // TRUE iff this is an integer test

{
    APLVFP mpfSrc;
    APLINT aplInt;
#ifdef DEBUG
    if (guTest > 0)
        ArbOut (L"_arb_get_ctsx = ", src);
#endif
    // Convert the ARB to a VFP
    mpfSrc = arb_get_mpfr (src);

#ifdef DEBUG
    if (guTest > 0)
        VfpOut (L"mpfSrc = ", &mpfSrc);
#endif
    // Use the MPFR routine
    aplInt = _mpfr_get_ctsx (&mpfSrc, fQuadCT, lpbRet, bIntegerTest);

    // We no longer need this storage
    Myf_clear (&mpfSrc);

    return aplInt;
} // End _arb_get_ctsx


//***************************************************************************
//  $arb_get_d
//***************************************************************************

double arb_get_d
    (LPAPLARB lpArb)

{
    return arf_get_d (arb_midref (lpArb), ARF_RND_NEAR);
} // End arb_get_d


//***************************************************************************
//  $arb_get_q
//***************************************************************************

APLRAT arb_get_q
    (LPAPLARB lpArb)

{
    APLVFP aplVfp = {0};
    APLRAT aplRat = {0};
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Initialize to 0
    mpfr_init0 (&aplVfp);
    mpq_init   (&aplRat);

    // Convert from ARB to MPFR
    arf_get_mpfr2 (&aplVfp, arb_midref (lpArb), prec);

    // Convert from MPFR to MPQ
    mpfr_get_q (&aplRat, &aplVfp);

    // Free the temps
    mpfr_clear (&aplVfp);

    return aplRat;
} // End arb_get_q


//***************************************************************************
//  $arb_get_mpfr
//***************************************************************************

APLVFP arb_get_mpfr
    (LPAPLARB lpArb)

{
    APLVFP aplVfp;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Initialize the temp to 0
    mpfr_init0 (&aplVfp);

    arf_get_mpfr2 (&aplVfp, arb_midref (lpArb), prec);

    return aplVfp;
} // End arb_get_mpfr


//***************************************************************************
//  $arb_inits
//***************************************************************************

void arb_inits
    (arb_ptr x, ...)

{
    va_list vl;
////int cnt = 0;

    // Initialize the variable list
    va_start (vl, x);

    // Loop through the list
    while (x NE NULL)
    {
        Myarb_init (x);
////    cnt++;

        x = va_arg (vl, arb_ptr);
    } // End WHILE

////printf ("inits cnt = ", cnt);
////CRLF
////
    // End the variable list
    va_end (vl);
} // End arb_inits


//***************************************************************************
//  $arb_clears
//***************************************************************************

void arb_clears
    (arb_ptr x, ...)

{
    va_list vl;
////int cnt = 0;

    // Initialize the variable list
    va_start (vl, x);

    // Loop through the list
    while (x NE NULL)
    {
        Myarb_clear (x);
////    cnt++;

        x = va_arg (vl, arb_ptr);
    } // End WHILE

////printf ("clears cnt = ", cnt);
////CRLF

    // End the variable list
    va_end (vl);
} // End arb_clears


//***************************************************************************
//  $arb_init_set
//***************************************************************************

void arb_init_set
    (arb_t x,
     arb_t y)

{
    Myarb_init (x);
    arb_set    (x, y);
} // End arb_init_set


//***************************************************************************
//  $IsArbNULL
//
//  Is the given value all zero?
//***************************************************************************

int IsArbNULL
    (LPAPLBA1F op)              // Source

{
    // Check for special cases
    return (arb_midref (op)->exp       EQ 0
         && arb_midref (op)->size      EQ 0
         && arb_midref (op)->d.ptr.d   EQ NULL
         && arb_midref (op)->d.noptr.d EQ NULL
         && arb_radref (op)->exp       EQ 0
         && arb_radref (op)->man       EQ 0
           );
} // End IsArbNULL


//***************************************************************************
//  $IsZeroBAxF
//***************************************************************************

UBOOL IsZeroBAxF
    (LPVOID lpaplRht,               // Ptr to right arg
     int    iHCDimRht)              // HC Dimension (1, 2, 4, 8)

{
    int i;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < iHCDimRht; i++)
    if (!IsArb0 (&((LPAPLBA8F) lpaplRht)->parts[i]))
        return FALSE;

    return TRUE;
} // End IsZeroBAxF


//***************************************************************************
//  $arbX_init_set
//***************************************************************************

void arbX_init_set
    (LPVOID lpDst,
     LPVOID lpSrc,
     int    iHCDim)

{
    int i;

    // Loop through all the parts
    for (i = 0; i < iHCDim; i++)
        // Initialize to 0/0
        arb_init_set (&((LPAPLBA8F) lpDst)->parts[i], &((LPAPLBA8F) lpSrc)->parts[i]);
} // End arbX_init_set


#ifdef DEBUG
//***************************************************************************
//  $arb2_init_set
//***************************************************************************

void arb2_init_set
    (LPAPLBA2F lpDst,
     LPAPLBA2F lpSrc)

{
    arbX_init_set (lpDst, lpSrc, 2);
} // End arb2_init_set
#endif


#ifdef DEBUG
//***************************************************************************
//  $arb4_init_set
//***************************************************************************

void arb4_init_set
    (LPAPLBA4F lpDst,
     LPAPLBA4F lpSrc)

{
    arbX_init_set (lpDst, lpSrc, 4);
} // End arb4_init_set
#endif


#ifdef DEBUG
//***************************************************************************
//  $arb8_init_set
//***************************************************************************

void arb8_init_set
    (LPAPLBA8F lpDst,
     LPAPLBA8F lpSrc)

{
    arbX_init_set (lpDst, lpSrc, 8);
} // End arb8_init_set
#endif


//***************************************************************************
//  $arbX_init
//
//  Initialize an ARB unless already initialized
//***************************************************************************

void arbX_init
    (LPVOID lparbVal,
     int    iHCDim)

{
    int i;

    for (i = 0; i < iHCDim; i++)
    {
#ifdef DEBUG
        if (((LPAPLBA8F) lparbVal)->parts[i].mid.d.ptr.d NE NULL)
            DbgStop ();
#endif
        arb_init (&((LPAPLBA8F) lparbVal)->parts[i]);
    } // End FOR
} // End arbX_init


#ifdef DEBUG
//***************************************************************************
//  $arb1_init
//***************************************************************************

void arb1_init
    (LPAPLARB lpArb)

{
    arbX_init (lpArb, 1);
} // End arb1_init
#endif


#ifdef DEBUG
//***************************************************************************
//  $arb2_init
//***************************************************************************

void arb2_init
    (LPAPLBA2F lpArb)

{
    arbX_init (lpArb, 2);
} // End arb2_init
#endif


#ifdef DEBUG
//***************************************************************************
//  $arb4_init
//***************************************************************************

void arb4_init
    (LPAPLBA4F lpArb)

{
    arbX_init (lpArb, 4);
} // End arb4_init
#endif


#ifdef DEBUG
//***************************************************************************
//  $arb8_init
//***************************************************************************

void arb8_init
    (LPAPLBA8F lpArb)

{
    arbX_init (lpArb, 8);
} // End arb8_init
#endif


//***************************************************************************
//  $Myarf_init
//***************************************************************************

void Myarf_init
    (arf_t arf)

{
#ifdef DEBUG
    if (arf->d.ptr.d NE NULL)
        DbgStop ();
#endif
    arf_init (arf);
} // End Myarf_init


//***************************************************************************
//  $arbX_clear
//
//  Free an ARB unless already clear
//***************************************************************************

void arbX_clear
    (LPVOID lparbVal,
     int    iHCDim)

{
    int i;
//  WCHAR wszTemp[256];
//
//  MySprintfW (wszTemp,
//              sizeof (wszTemp),
//             L"iHCDim = %I64d\r\n",
//              iHCDim);
//  MBW (wszTemp);

    for (i = 0; i < iHCDim; i++)
#ifdef DEBUG
////if (((LPAPLBA8F) lparbVal)->parts[i].mid.d.  ptr.d NE NULL
//// || ((LPAPLBA8F) lparbVal)->parts[i].mid.d.noptr.d NE NULL)
#endif
    {
////    static APLARB aplArb;
////                  aplArb = ((LPAPLBA8F) lparbVal)->parts[i];

        arb_clear (&((LPAPLBA8F) lparbVal)->parts[i]);
////    arb_clear (&aplArb);
#ifdef DEBUG
        ((LPAPLBA8F) lparbVal)->parts[i].mid.d.  ptr.d = NULL;
////    ((LPAPLBA8F) lparbVal)->parts[i].mid.d.noptr.d = NULL;
////    aplArb                          .mid.d.noptr.d = NULL;
#endif
    } // End FOR/IF
} // End arbX_clear


#ifdef DEBUG
//***************************************************************************
//  $arb1_clear
//***************************************************************************

void arb1_clear
    (LPAPLBA1F lpArb)

{
    arbX_clear (lpArb, 1);
} // End arb1_clear
#endif


#ifdef DEBUG
//***************************************************************************
//  $arb2_clear
//***************************************************************************

void arb2_clear
    (LPAPLBA2F lpArb)

{
    arbX_clear (lpArb, 2);
} // End arb2_clear
#endif


#ifdef DEBUG
//***************************************************************************
//  $arb4_clear
//***************************************************************************

void arb4_clear
    (LPAPLBA4F lpArb)

{
    arbX_clear (lpArb, 4);
} // End arb4_clear
#endif


#ifdef DEBUG
//***************************************************************************
//  $arb8_clear
//***************************************************************************

void arb8_clear
    (LPAPLBA8F lpArb)

{
    arbX_clear (lpArb, 8);
} // End arb8_clear
#endif


//***************************************************************************
//  $arbX_init_set_sx
//***************************************************************************

void arbX_init_set_sx
    (LPVOID lpDst,
     APLINT aplInt,
     int    X)

{
    int i;

    // Set the Real part
    arb_init_set_sx (lpDst, aplInt);

    // Loop through the imaginary parts
    for (i = 1; i < X; i++)
        // Initialize to 0/0
        arb_init (&((LPAPLBA8F) lpDst)->parts[i]);
} // End arbX_init_set_sx


//***************************************************************************
//  $arb_init_set_sx
//***************************************************************************

void arb_init_set_sx
    (LPAPLBA1F lpDst,               // Ptr to destin value
     APLINT    aplInt)              // 64-bit integer

{
    Myarb_init (lpDst);
    arb_set_sx (lpDst, aplInt);
} // End arb_init_set_sx


#ifdef DEBUG
//***************************************************************************
//  $arb2_init_set_sx
//***************************************************************************

void arb2_init_set_sx
    (LPAPLBA2F lpArb,
     APLINT    aplInt)

{
    arbX_init_set_sx (lpArb, aplInt, 2);
} // End arb2_init_set_sx
#endif


#ifdef DEBUG
//***************************************************************************
//  $arb4_init_set_sx
//***************************************************************************

void arb4_init_set_sx
    (LPAPLBA4F lpArb,
     APLINT    aplInt)

{
    arbX_init_set_sx (lpArb, aplInt, 4);
} // End arb4_init_set_sx
#endif


#ifdef DEBUG
//***************************************************************************
//  $arb8_init_set_sx
//***************************************************************************

void arb8_init_set_sx
    (LPAPLBA8F lpArb,
     APLINT    aplInt)

{
    arbX_init_set_sx (lpArb, aplInt, 8);
} // End arb8_init_set_sx
#endif


//***************************************************************************
//  $arb_set_sx
//***************************************************************************

void arb_set_sx
    (LPAPLARB lpArb,                // Ptr to destin value
     APLINT   aplInt)               // 64-bit integer

{
    mpq_t  q;
    fmpq_t fmpq;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Initialize the temps
    mpq_init  (q);
    fmpq_init (fmpq);

    // Convert the value from INT to MPQ
    mpq_set_sx (q, aplInt, 1);

    // Convert the value from MPQ to FMPQ
    fmpq_set_mpq (fmpq, q);

    // Convert the value from FMPQ to ARB
    arb_set_fmpq (lpArb, fmpq, prec);

    // Free the temps
    fmpq_clear (fmpq);
    mpq_clear  (q);
} // End arb_set_sx


//***************************************************************************
//  $arb_set_q
//***************************************************************************

void arb_set_q
    (LPAPLARB lpArb,                // Ptr to destin value
     LPAPLRAT lpaplRat)             // RAT

{
    fmpq_t fmpq;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Initialize the temps
    fmpq_init (fmpq);

    // Convert the value from MPQ to FMPQ
    fmpq_set_mpq (fmpq, lpaplRat);

    // Convert the value from FMPQ to ARB
    arb_set_fmpq (lpArb, fmpq, prec);

    // Free the temps
    fmpq_clear (fmpq);
} // End arb_set_q


//***************************************************************************
//  $arb_init_set_q
//***************************************************************************

void arb_init_set_q
    (LPAPLARB lpArb,                // Ptr to destin value
     LPAPLRAT lpaplRat)             // RAT

{
    Myarb_init (lpArb);
    arb_set_q  (lpArb, lpaplRat);
} // End arb_init_set_q


//***************************************************************************
//  $arb_set_mpfr
//***************************************************************************

void arb_set_mpfr
    (LPAPLARB lpArb,                // Ptr to destin value
     LPAPLVFP lpaplVfp)             // RAT

{
    mpfr_prec_t uPrec;

    // Convert the value from MPFR to ARB
    arf_set_mpfr (arb_midref (lpArb), lpaplVfp);

    // If the number is inexact, ...
    if (!mpfr_exact_p (lpaplVfp))
    {
        // Get the precision of the MPFR number
        uPrec = mpfr_get_prec (lpaplVfp);

        // Set the Radius to 2*-uPrec
        mag_set_ui_2exp_si (arb_radref (lpArb), 1, -uPrec);
    } // End IF
} // End arb_set_mpfr


//***************************************************************************
//  $arb_init_set_mpfr
//***************************************************************************

void arb_init_set_mpfr
    (LPAPLARB lpArb,                // Ptr to destin value
     LPAPLVFP lpaplVfp)             // VFP

{
    Myarb_init   (lpArb);
    arb_set_mpfr (lpArb, lpaplVfp);
} // End arb_init_set_mpfr


//***************************************************************************
//  $arb_init_set_d
//***************************************************************************

void arb_init_set_d
    (LPAPLARB lpArb,                // Ptr to destin value
     double   aplFlt)               // FLT

{
    Myarb_init (lpArb);
    arb_set_d  (lpArb, aplFlt);
} // End arb_init_set_d


//***************************************************************************
//  $arb_init_set_arf
//***************************************************************************

void arb_init_set_arf
    (LPAPLARB lpArb,                // Ptr to destin value
     arf_t    lpArf)                // ARF

{
    Myarb_init  (lpArb);
    arb_set_arf (lpArb, lpArf);
} // End arb_init_set_arf


//***************************************************************************
//  $arbX_set
//***************************************************************************

void arbX_set
    (LPVOID           lpDst,
     LPVOID           lpSrc,
     int              N)

{
    int i;

    // Loop through all the parts
    for (i = 0; i < N; i++)
        arb_set (&((LPAPLBA8F) lpDst)->parts[i],
                 &((LPAPLBA8F) lpSrc)->parts[i]);
} // End arbX_set


#ifdef DEBUG
//***************************************************************************
//  $arb2_set
//***************************************************************************

void arb2_set
    (LPAPLBA2F lpDst,
     LPAPLBA2F lpSrc)

{
    arbX_set (lpDst, lpSrc, 2);
} // End arb2_set
#endif


#ifdef DEBUG
//***************************************************************************
//  $arb4_set
//***************************************************************************

void arb4_set
    (LPAPLBA4F lpDst,
     LPAPLBA4F lpSrc)

{
    arbX_set (lpDst, lpSrc, 4);
} // End arb4_set
#endif


#ifdef DEBUG
//***************************************************************************
//  $arb8_set
//***************************************************************************

void arb8_set
    (LPAPLBA8F lpDst,
     LPAPLBA8F lpSrc)

{
    arbX_set (lpDst, lpSrc, 8);
} // End arb8_set
#endif


//***************************************************************************
//  $arbXf_nan_p
//
//  Is any part of the arg a NaN?
//***************************************************************************

UBOOL arbXf_nan_p
    (LPVOID lpaplRht,
     int    iHCDimRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < iHCDimRht; i++)
    if (arb_nan_p (&((LPAPLBA8F) lpaplRht)->parts[i]))
        return TRUE;

    return FALSE;
} // End arbXf_nan_p


//***************************************************************************
//  $arb_set_inf
//***************************************************************************

void arb_set_inf
    (LPAPLARB lpDst,
     int      sign)

{
    // Split cases based upon the sign
    switch (sign)
    {
        case -1:
            arb_neg_inf (lpDst);

            break;

        case  1:
            arb_pos_inf (lpDst);

            break;

        case  0:
        defstop
            break;
    } // End SWITCH
} // End arb_set_inf


//***************************************************************************
//  $arb_QuadICValue
//
//  Return the appropriate []IC value
//***************************************************************************

LPAPLBA1F arb_QuadICValue
    (LPAPLBA1F  aplArbLft,          // Left arg
     IC_INDICES icIndex,            // []IC index
     LPAPLBA1F  aplArbRht,          // Right arg
     LPAPLBA1F  arbRes,             // Result
     UBOOL      bNegate)            // TRUE iff we should negate result

{
    switch (GetQuadICValue (icIndex))
    {
        case ICVAL_NEG1:
            // Initialize the result to -1
            Myarb_init (arbRes);
            arb_set_si (arbRes, -1);

            break;

        case ICVAL_ZERO:
            // Initialize the result to 0/0
            Myarb_init (arbRes);

            break;

        case ICVAL_ONE:
            // Initialize the result to 1
            Myarb_init (arbRes);
            arb_one    (arbRes);

            break;

        case ICVAL_DOMAIN_ERROR:
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        case ICVAL_POS_INFINITY:
            arb_init_set (arbRes, &arbPosInfinity);

            break;

        case ICVAL_NEG_INFINITY:
            arb_init_set (arbRes, &arbNegInfinity);

            break;

        case ICVAL_NAN:
            // Initialize to a NaN
            Myarb_init (arbRes);

            break;

        case ICVAL_LEFT:
            arb_init_set (arbRes, aplArbLft);

            break;

        case ICVAL_RIGHT:
            arb_init_set (arbRes, aplArbRht);

            break;

        defstop
            break;
    } // End SWITCH

    // If we should negate, ...
    if (bNegate)
        arb_neg (arbRes, arbRes);

    return arbRes;
} // End arb_QuadICValue


//***************************************************************************
//  $arb_cmp_ct
//
//  Compare two ARBs relative to a given comparison tolerance
//
//  Return +1 if Lft >  Rht
//          0 if Lft EQ Rht
//         -1 if Lft <  Rht
//***************************************************************************

int arb_cmp_ct
    (LPAPLARB lpaplLft,             // Ptr to left arg
     LPAPLARB lpaplRht,             // ...    right ...
     APLFLOAT fQuadCT,              // []CT
     UBOOL    bIntegerTest)         // TRUE iff this is an integer test

{
    int iRet;                       // Result of arb_cmp

////#define CT_DEBUG

#if defined (DEBUG) && defined (CT_DEBUG)
    ArbOut (L"Lft: ", lpaplLft);
    ArbOut (L"Rht: ", lpaplRht);
#endif
    // Compare 'em without tolerance
    iRet = arb_cmp (lpaplLft, lpaplRht);

    // If args are unequal,
    //   and []CT is non-zero,
    //   and neither arg is either infinity, ...
    if (iRet NE 0                               // Lft NE Rht
     && fQuadCT NE 0                            // []CT NE 0
     && !IsArbInfinity (lpaplLft)
     && !IsArbInfinity (lpaplRht))
    {
        APLARB arbLftAbs = {0},     // Absolute value of left arg
               arbRhtAbs = {0};     // ...               right ...
        int    sgnLft,              // Left arg sign
               sgnRht;              // Right ...

        // Use an algorithm similar to the one in flt_cmp_ct

        // Initialize the temps to 0/0
        Myarb_init (&arbLftAbs);
        Myarb_init (&arbRhtAbs);

        // Get the signs
        sgnLft = arb_sign (lpaplLft);
        sgnRht = arb_sign (lpaplRht);

        // Get the absolute values
        arb_abs2 (&arbLftAbs, lpaplLft);
        arb_abs2 (&arbRhtAbs, lpaplRht);

        // If this is an integer test, allow comparison with zero
        if (bIntegerTest
         && sgnLft EQ 0
         && arf_cmp_d (arb_midref (&arbRhtAbs), SYS_CT) <= 0)
            iRet = 0;
        else
        if (bIntegerTest
         && sgnRht EQ 0
         && arf_cmp_d (arb_midref (&arbLftAbs), SYS_CT) <= 0)
            iRet = 0;
        else
        if (!IsArb0 (lpaplLft)          // Lft NE 0
         && !IsArb0 (lpaplRht)          // Rht NE 0
         && sgnLft EQ sgnRht)           // Signs are the same, ...
        {
            APLARB arbCT     = {0},
                   arbHoodLo = {0};
            mp_limb_signed_t prec = ARB_PREC_FPC;

            // Initialize the temps to 0/0
            Myarb_init (&arbCT);
            Myarb_init (&arbHoodLo);

            // Convert the comparison tolerance
            arb_set_d (&arbCT, fQuadCT);
#if defined (DEBUG) && defined (CT_DEBUG)
            ArbOut (L"CT:  ", &arbCT);
#endif
            // Calculate the low end of the left neighborhood of (|Rht)
////////////aplHoodLo = aplRhtAbs - aplRhtAbs * fQuadCT;
            arb_mul (&arbHoodLo, &arbRhtAbs, &arbCT    , prec);
            arb_sub (&arbHoodLo, &arbRhtAbs, &arbHoodLo, prec);
#if defined (DEBUG) && defined (CT_DEBUG)
            ArbOut (L"Lo1: ", &arbHoodLo);
            ArbOut (L"L1 : ", &arbLftAbs);
            ArbOut (L"R1 : ", &arbRhtAbs);
#endif
            // If (|Rht) is greater than (|Lft),
            // and (|Lft) is in the
            //    left-neighborhood of (|Rht) with CT, return 0 (equal)
////////////if (aplHoodLo <= aplLftAbs
//////////// &&              aplLftAbs < aplRhtAbs)
////////////    return TRUE;
            if (arb_cmp (&arbHoodLo, &arbLftAbs)             <= 0
             && arb_cmp (            &arbLftAbs, &arbRhtAbs) <  0)
                iRet = 0;
            else
            {
                // Calculate the low end of the left neighborhood of (|Lft)
////////////////aplHoodLo = aplLftAbs - aplLftAbs * fQuadCT;
                arb_mul (&arbHoodLo, &arbLftAbs, &arbCT    , prec);
                arb_sub (&arbHoodLo, &arbLftAbs, &arbHoodLo, prec);
#if defined (DEBUG) && defined (CT_DEBUG)
            ArbOut (L"Lo2: ", &arbHoodLo);
            ArbOut (L"R2 : ", &arbRhtAbs);
            ArbOut (L"L2 : ", &arbLftAbs);
#endif
                // If (|Lft) is greater than (|Rht),
                // and (|Rht) is in the
                //    left-neighborhood of (|Lft) with CT, return 0 (equal)
////////////////if (aplHoodLo <= aplRhtAbs
//////////////// &&              aplRhtAbs < aplLftAbs)
////////////////    return TRUE;
                if (arb_cmp (&arbHoodLo, &arbRhtAbs)             <= 0
                 && arb_cmp (            &arbRhtAbs, &arbLftAbs) <  0)
                    iRet = 0;
            } // End IF/ELSE

            // We no longer need this storage
            Myarb_clear (&arbHoodLo);
            Myarb_clear (&arbCT    );
        } // End IF/ELSE

        Myarb_clear (&arbRhtAbs);
        Myarb_clear (&arbLftAbs);
    } // End IF

    return iRet;
} // End arb_cmp_ct


//***************************************************************************
//  $Myarb_floor
//***************************************************************************

void Myarb_floor
    (LPAPLARB lpDst,
     LPAPLARB lpSrc)

{
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Use ARB's definition
    arb_floor (lpDst, lpSrc, prec);

    return;

#if TRUE
    // Get the Floor of the Midpoint
    arf_floor (arb_midref (lpDst), arb_midref (lpSrc));
    mag_zero  (arb_radref (lpDst));
#else
    APLVFP vfpTmp1 = {0},
           vfpTmp2 = {0};

    // Get the corresponding VFP
    vfpTmp1 = arb_get_mpfr (lpSrc);

    // Calculate the Floor
    vfpTmp2 = FloorHC1V (vfpTmp1);

    // Save the MPFR in the result
    arb_set_mpfr (lpDst, &vfpTmp2);
    mag_zero     (arb_radref (lpDst));

    // We no longer need this storage
    mpfr_clear (&vfpTmp2);
    mpfr_clear (&vfpTmp1);
#endif
} // End Myarb_floor


//***************************************************************************
//  $Myarb_ceil
//***************************************************************************

void Myarb_ceil
    (LPAPLARB lpDst,
     LPAPLARB lpSrc)

{
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Use ARB's definition
    arb_ceil  (lpDst, lpSrc, prec);

    return;

#if TRUE
    // Get the Ceiling of the Midpoint
    arf_ceil  (arb_midref (lpDst), arb_midref (lpSrc));
    mag_zero  (arb_radref (lpDst));
#else
    APLVFP vfpTmp1 = {0},
           vfpTmp2 = {0};

    // Get the corresponding VFP
    vfpTmp1 = arb_get_mpfr (lpSrc);

    // Calculate the Ceiling
    vfpTmp2 = CeilHC1V (vfpTmp1);

    // Save the MPFR in the result
    arb_set_mpfr (lpDst, &vfpTmp2);
    mag_zero     (arb_radref (lpDst));

    // We no longer need this storage
    mpfr_clear (&vfpTmp2);
    mpfr_clear (&vfpTmp1);
#endif
} // End Myarb_ceil


//***************************************************************************
//  $DistBA2F
//***************************************************************************

APLARB DistBA2F
    (APLBA2F aplLft,                    // Left arg
     APLBA2F aplRht)                    // Right ...

{
    return MagBA2F (SubBA2F_RE (aplLft, aplRht));
} // End DistBA2F


//***************************************************************************
//  $DistBA4F
//***************************************************************************

APLARB DistBA4F
    (APLBA4F aplLft,                    // Left arg
     APLBA4F aplRht)                    // Right ...

{
    return MagBA4F (SubBA4F_RE (aplLft, aplRht));
} // End DistBA4F


//***************************************************************************
//  $DistBA8F
//***************************************************************************

APLARB DistBA8F
    (APLBA8F aplLft,                    // Left arg
     APLBA8F aplRht)                    // Right ...

{
    return MagBA8F (SubBA8F_RE (aplLft, aplRht));
} // End DistBA8F


//***************************************************************************
//  $IsOneBAxF
//***************************************************************************

UBOOL IsOneBAxF
    (LPVOID lpaplRht,               // Ptr to right arg
     int    iHCDimRht)              // HC Dimension (1, 2, 4, 8)

{
    int    i;
    APLARB j = arbOne;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < iHCDimRht; i++, j = arbZero)
    if (arb_cmp (&((LPAPLBA8F) lpaplRht)->parts[i], &j) NE 0)
        return FALSE;

    return TRUE;
} // End IsOneBAxF


//***************************************************************************
//  $arb_mul_d
//***************************************************************************

void arb_mul_d
    (arb_t dst,
     arb_t src,
     double mul)

{
    arf_t arf;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    //Initialize to mul
    arf_init  (arf);
    arf_set_d (arf, mul);

    // Multiply
    arb_mul_arf (dst, src, arf, prec);

    // Clean up
    arf_clear (arf);
} // End arb_mul_d


//***************************************************************************
//  $arb_mod
//***************************************************************************

void arb_mod
    (arb_t Dst,
     arb_t Val,
     arb_t Mod)

{
    APLARB aplDst = {0};

    aplDst = ModBA1F (*Mod, *Val);
    arb_set (Dst, &aplDst);

    arb_clear (&aplDst);
} // End arb_mod


//***************************************************************************
//  $arb_mod2
//***************************************************************************

void arb_mod2
    (arb_t Dst,
     arb_t Val,
     arb_t Mod)

{
    APLARB aplDiv,
           aplFlr,
           aplMul,
           aplSub;

    // If the modulus is Zero, ...
    if (arb_zero_p (Mod))
        arb_set (Dst, Val);
    else
    {
        // Calculate Val - Mod * Floor (Val / Mod)
        aplDiv = DivBA1F_RE   (*Val, *Mod);     //                    Val / Mod
        aplFlr = FloorBA1F    (aplDiv);         //             Floor (Val / Mod)
        aplMul = MulBA1F_RE   (*Mod, aplFlr);   //       Mod * Floor (Val / Mod)
        aplSub = SubBA1F_RE   (*Val, aplMul);   // Val - Mod * Floor (Val / Mod)

        // Save in the result
        arb_set (Dst, &aplSub);

#ifdef DEBUG
        if (guTest > 0)
        {
            // (|Mod) > |Dst
            ArbOut (L"Mod = ", Mod);
            ArbOut (L"Dst = ", Dst);
        } // End IF
#endif

        // Free the temps
        Myarb_clear (&aplSub);
        Myarb_clear (&aplMul);
        Myarb_clear (&aplFlr);
        Myarb_clear (&aplDiv);
    } // End IF/ELSE
} // End arb_mod2


//***************************************************************************
//  $mag_set_d2
//***************************************************************************

void mag_set_d2
    (mag_t  mag,
     double d)

{
    fmpr_t fr;

    // Initialize
    fmpr_init (fr);

    // Convert FLT to FMPR
    fmpr_set_d (fr, d);

    // Set the radius
    mag_set_fmpr (mag, fr);

    // We no longer need this storage
    fmpr_clear (fr);
} // End mag_set_d2


//***************************************************************************
//  $arb_end_pow
//***************************************************************************

void arb_end_pow
    (arb_t       y,
     const arb_t x,
     ULONG       u,
     slong       prec)

{
    mpfr_t lo    = {0},
           hi    = {0},
           powLo = {0},
           powHi = {0};

    // Initialize
    mpfr_inits (lo, hi, powLo, powHi, NULL);

    // Get the endpoints
    arb_get_interval_mpfr (lo, hi, x);

#ifdef DEBUG
    if (guTest > 100)
    {
        VfpOut (L"lo   = ", lo);
        VfpOut (L"hi   = ", hi);
    } // End IF
#endif

    // Calculate the powers of the endpoints
    mpfr_pow_ui (powLo, lo, u, MPFR_RNDD);
    mpfr_pow_ui (powHi, hi, u, MPFR_RNDU);

#ifdef DEBUG
    if (guTest > 100)
    {
        VfpOut (L"powLo = ", powLo);
        VfpOut (L"powHi = ", powHi);
    } // End IF
#endif

    // Ensure proper order and rounding
    if (mpfr_cmp (powLo, powHi) > 0)
    {
        mpfr_pow_ui (powHi, lo, u, MPFR_RNDU);
        mpfr_pow_ui (powLo, hi, u, MPFR_RNDD);
    } // End IF

    // If the argument interval contains a zero,
    //   and the low power is positive, ...
    if (arb_contains_zero (x)
     && mpfr_sgn (powLo) > 0)
        // Set to zero
        mpfr_set_zero (powLo, +1);  // powLo = +0

#ifdef DEBUG
    if (guTest > 100)
    {
        VfpOut (L"powLo = ", powLo);
        VfpOut (L"powHi = ", powHi);
    } // End IF
#endif

    // Construct the interval of the powers of the endpoints

    // Note that because <arb_set_interval_mpfr> may create a bigger
    //   Ball than the  endpoints indicate (including negative numbers),
    //   we must use a cover function for even powers
    // Create the interval excluding negative numbers
    arb_set_interval_mpfr2 (y, powLo, powHi, prec);

#ifdef DEBUG
    if (guTest > 100)
    {
        ArbOut (L"Ival = ", y);

        // Get the endpoints
        arb_get_interval_mpfr (lo, hi, y);

        VfpOut (L"y.lo = ", lo);
        VfpOut (L"y.hi = ", hi);
    } // End IF
#endif

    // We no longer need this storage
    mpfr_clears (powHi, powLo, hi, lo, NULL);
} // End arb_end_pow


//***************************************************************************
//  arb_abs2
//
//  Ensure that the result does not contian negative numbers
//***************************************************************************

void arb_abs2
    (      arb_t y,
     const arb_t x)

{
    // Call the API
    arb_abs (y, x);

    if (arb_contains_negative (y))
        // Delete the negative values
        arb_nonnegative_part (y, y);
} // End arb_abs2


//***************************************************************************
//  arb_set_interval_mpfr2
//
//  If the low endpoint is zero, ensure that the result
//    does not contain any negative numbers
//***************************************************************************

void arb_set_interval_mpfr2
    (arb_t  x,
     mpfr_t a,
     mpfr_t b,
     slong  prec)

{
#ifdef DEBUG
    if (guTest > 100)
    {
        VfpOut (L"a = ", a);
        VfpOut (L"b = ", b);
    } // End IF
#endif
    // Call the original function
    arb_set_interval_mpfr (x, a, b, prec);

    // If the low endpoint is zero, ...
    if (mpfr_zero_p (a))
        // Delete the negative values
        arb_nonnegative_part (x, x);

    // If the high endpoint is zero, ...
    if (mpfr_zero_p (b))
        // Delete the negative values
        arb_nonpositive_part (x, x);
} // End arb_set_interval_mpfr2


//***************************************************************************
//  $arf_get_mpfr2
//
//  Cover function for <arf_get_mpfr> which handles neither
//    overflow nor underflow
//***************************************************************************

int arf_get_mpfr2
    (mpfr_t     res,
     arf_t      x,
     mpfr_rnd_t rnd)

{
    int    iRet;                    // The result
    fmpz_t fmpzMan,                 // Mantissa
           fmpzExp;                 // Exponent
////char *pMan, *pExp;

    fmpz_init (fmpzMan);
    fmpz_init (fmpzExp);

    // Convert x to Man * s^Exp
    arf_get_fmpz_2exp (fmpzMan, fmpzExp, x);

////pMan = fmpz_get_str (NULL, 10, fmpzMan);
////pExp = fmpz_get_str (NULL, 10, fmpzExp);

    // If the exponent is too large, ...
    if (fmpz_cmp_si (fmpzExp, COEFF_MAX) >= 0)
    {
        mpfr_set_inf  (res, +1);

        iRet = 0;
    } else
    // If the exponent is too small, ...
    if (fmpz_cmp_si (fmpzExp, COEFF_MIN) <= 0)
    {
        mpfr_set_zero (res, +1);

        iRet = 0;
    } else
        // Let ARRB handle it
        iRet = arf_get_mpfr (res, x, rnd);

    // We no longer need this storage
////free (pMan);
////free (pExp);
    fmpz_clear (fmpzMan);
    fmpz_clear (fmpzExp);

    return iRet;
} // End arf_get_mpfr2


//***************************************************************************
//  $arb_sqr2
//***************************************************************************

void arb_sqr2
    (arb_t       y,
     const arb_t x,
     slong       prec)

{
#ifdef DEBUG
    if (guTest > 100)
        arb_sqr (y, x, prec);
    else
#endif
        arb_end_pow (y, x, 2, prec);

//  mpfr_t mpfrLo = {0},
//         mpfrHi = {0};
//
//  // Get the endpoints
//  arb_get_interval_mpfr (mpfrLo, mpfrHi, x);
//
//  // Square the endpoints
//  mpfr_sqr (mpfrLo, mpfrLo, MPFR_RNDD);
//  mpfr_sqr (mpfrHi, mpfrHi, MPFR_RNDU);
//
//  // Create the new Ball from the interval
//  arb_set_interval_mpfr2 (y, mpfrLo, mpfrHi, prec);

//  // Call API
//  arb_sqr (y, x, prec);
///
///     arb_t  w;
///
///     // Calculate the interval of the power of the endpoints
///     arb_end_pow (w, x, 2, prec);
///
///     // Call API
///     arb_sqr (y, x, prec);
///
/// #ifdef DEBUG
///     if (guTest > 0)
///         ArbOut (L"Sqrt = ", w);
/// #endif
///
///     // Intersect the two balls
///     arb_intersection (y, y, w, prec);
///
/// #ifdef DEBUG
///     if (guTest > 0)
///         ArbOut (L"Res  = ", w);
/// #endif
///
///     // We no longer need this storage
///     Myarb_clear  (w);
} // End arb_sqr2


//***************************************************************************
//  $arb_cmp_d
//***************************************************************************

int arb_cmp_d
    (arb_t  x,
     double d)

{
    return arf_cmp_d (arb_midref (x), d);
} // End arb_cmp_d


//***************************************************************************
//  $arb_si_cmp
//
//  Compare a signed long int and a ARB
//***************************************************************************

int arb_si_cmp
    (signed long int op1,       // Left arg
     LPAPLARB        op2)       // Right arg

{
    return -arb_cmp_si (op2, op1);
} // End arb_si_cmp


//***************************************************************************
//  $arb_ui_cmp
//
//  Compare an unsigned long int and a ARB
//***************************************************************************

int arb_ui_cmp
    (unsigned long int op1,     // Left arg
     LPAPLARB          op2)     // Right arg

{
    return -arb_cmp_ui (op2, op1);
} // End arb_ui_cmp


//***************************************************************************
//  $arb_set_str2
//***************************************************************************

void arb_set_str2
    (LPAPLARB         lpArb,
     LPCHAR           lpSrcStr,
     mp_limb_signed_t prec)

{
    // NOT ARB_CHARS as ± is handled separately
    arb_set_spn_str (lpArb, lpSrcStr, prec, DEC_CHARS DEF_POSINFINITY_STR);
} // End arb_set_str2


//***************************************************************************
//  $arb_init_set_str2
//***************************************************************************

void arb_init_set_str2
    (LPAPLARB         lpArb,
     LPCHAR           lpSrcStr,
     mp_limb_signed_t prec)

{
    Myarb_init   (lpArb);
    arb_set_str2 (lpArb, lpSrcStr, prec);
} // End arb_init_set_str2


//***************************************************************************
//  $arb_set_spn_str
//***************************************************************************

void arb_set_spn_str
    (LPAPLARB         lpArb,
     LPCHAR           lpSrcStr,
     mp_limb_signed_t prec,
     LPCHAR           spnChars)

{
    size_t numLen;
    char   cZap;

    // Zap the trailing character as arb_set_str crashes with it present
    numLen = strspn (lpSrcStr, spnChars);
    cZap   = lpSrcStr[numLen];
             lpSrcStr[numLen] = AC_EOS;

    // If the Midpoint is PosInfinity or NegInfinity, ...
    if (lpSrcStr[0] EQ DEF_POSINFINITY_CHAR)
        // Set the Midpoint to +Infinity
        arf_pos_inf (arb_midref (lpArb));
    else
    if (lpSrcStr[0] EQ OVERBAR1
     && lpSrcStr[1] EQ DEF_POSINFINITY_CHAR)
        // Set the Midpoint to -Infinity
        arf_neg_inf (arb_midref (lpArb));
    else
        // In order to preserve the precision of the floating point number,
        //   we rescan it as a ARB number
        // Convert the string to a ARB number
        arb_set_str (lpArb, lpSrcStr, prec);

    // Restore the zapped char
    lpSrcStr[numLen] = cZap;

    // If there's a Radius, ...
    if (cZap EQ DEF_ARBSEP)
    {
        LPCHAR lpRadStr = &lpSrcStr[numLen + 1];
        APLARB arbRad = {0};

        // Zap the trailing character as arb_set_str crashes with it present
        numLen = strspn (lpRadStr, spnChars);
        cZap   = lpRadStr[numLen];
                 lpRadStr[numLen] = AC_EOS;
        // If there's an explicit Radius, ...
        if (numLen NE 0)
        {
            APLVFP aplVfpRad = {0};

            // Initialize
            mpfr_init0 (&aplVfpRad);

            // In order to preserve the precision of the floating point number,
            //   we rescan it as an MPFR number
            // Convert the string to MPFR
            mpfr_set_str (&aplVfpRad, lpRadStr, 10, MPFR_RNDN);

            // Set the Radius of lpArb
            mag_set_mpfr (arb_radref (lpArb), &aplVfpRad);

            // We no longer need this storage
            mpfr_clear (&aplVfpRad);

            // Restore the zapped char
            lpRadStr[numLen] = cZap;
        } // End IF/ELSE
    } // End IF
} // End arb_set_spn_str


//***************************************************************************
//  $arb_fits_uint_p
//***************************************************************************

UBOOL arb_fits_uint_p
    (LPAPLARB lpaplArb)

{
    return (arb_cmp_si (lpaplArb, UINT_MAX) < 0)
        && (arb_cmp_si (lpaplArb, 0) >= 0);
} // End arb_fits_uint_p


//***************************************************************************
//  $arb_fits_slong_p
//***************************************************************************

UBOOL arb_fits_slong_p
    (LPAPLARB lpaplArb)

{
    return (arb_cmp_si (lpaplArb, INT_MAX) < 0)
        && (arb_cmp_si (lpaplArb, INT_MIN) > 0);
} // End arb_fits_slong_p


//***************************************************************************
//  $arb_urandomb
//
// Generate a uniformly-distributed random number in [0, 1)
//***************************************************************************

void arb_urandomb
    (LPAPLARB     lpDst,            // Output save area
     flint_rand_t randStateFLINT)   // FLINT random number state for Query

{
    LPPERTABDATA lpMemPTD = GetMemPTD ();   // Ptr to PerTabData global memory
    APLVFP       aplVfp = {0};

    // Split cases based upon the Distribution Type
    switch (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DT]->stData.stChar)
    {
        case L'r':
            // Initialize to 0/0
            Myarb_init (lpDst);

////        // Generate a random number -- DON'T KNOW IN WHAT RANGE
////        arb_randtest_precise (lpDst, lpMemPTD->randStateFLINT, 10, 10);  // prec);

            // Initialize the result to 0
            mpfr_init0 (&aplVfp);

            // Generate a uniformly-distributed random number in [0, 1)
            mpfr_urandomb (&aplVfp, lpMemPTD->randStateGMP);

            // Copy to the result
            arb_set_mpfr (lpDst, &aplVfp);

            // We no longer need this storage
            mpfr_clear (&aplVfp);

            break;

        case L'g':
        case L'p':
            NONCE_RE    // ***FINISHME*** -- []DT EQ 'g' or 'p'

            break;

        defstop
            break;
    } // End SWITCH
} // End arb_urandomb


//***************************************************************************
//  $arb_urandomm
//
// Generate a uniformly-distributed random number in [0, Src)
//***************************************************************************

void arb_urandomm
    (LPAPLARB     lpDst,            // Output save area
     LPAPLARB     lpSrc,            // Range upper limit
     flint_rand_t randStateFLINT)   // FLINT random number state for Query

{
    LPPERTABDATA lpMemPTD = GetMemPTD ();   // Ptr to PerTabData global memory
    APLRAT       aplRat = {0};

    // Split cases based upon the Distribution Type
    switch (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DT]->stData.stChar)
    {
        case L'r':
            // Initialize to 0/0
            Myarb_init (lpDst);

////        // Generate a random number -- DON'T KNOW IN WHAT RANGE
////        arb_randtest (lpDst, randStateFLINT, ARB_PREC_FPC_PTD, 10); // ***FIXME***

            // Initialize the result to R
            aplRat = arb_get_q (lpSrc);

            // Generate a uniformly-distributed random number in [0, Rht)
            mpz_urandomm (mpq_numref (&aplRat),
                          lpMemPTD->randStateGMP,
                          mpq_numref (&aplRat));
            // Copy to the ARB result
            arb_set_q (lpDst, &aplRat);

            break;

        case L'g':
        case L'p':
            NONCE_RE    // ***FINISHME*** -- []DT EQ 'g' or 'p'

            break;

        defstop
            break;
    } // End SWITCH
} // End arb_urandomm


//***************************************************************************
//  $mag_get_mpfr
//***************************************************************************

void mag_get_mpfr
    (LPAPLVFP   lpVfp,
     mag_t      lpMag,
     mpfr_rnd_t rnd)

{
    // Test for special cases because <mag_get_fmpq> doesn't handle them
    if (!mag_is_finite (lpMag))
        mpfr_set_inf (lpVfp, +1);   // ARB handles +Inf only
    else
    {
        double dblMag;

        // Get the approximate log2 of the Radius
        dblMag = mag_get_d_log2_approx (lpMag);

        // Test for special cases because <mag_get_fmpq> doesn't handle them
        if (dblMag EQ COEFF_MIN)
            mpfr_set_zero (lpVfp, +1);
        else
        if (dblMag EQ COEFF_MAX)
            mpfr_set_inf  (lpVfp, +1);
        else
        {
            fmpq_t fmpq;

            // Initialize
            fmpq_init (fmpq);

            // Convert the MAG to FMPQ
            mag_get_fmpq (fmpq, lpMag);

            // Convert FMPQ to MPFR
            fmpq_get_mpfr (lpVfp, fmpq, rnd);

            // We no longer need this storage
            fmpq_clear (fmpq);
        } // End IF/ELSE/...
    } // End IF/ELSE/...
} // End mag_get_mpfr


//***************************************************************************
//  $mag_set_mpfr
//***************************************************************************

void mag_set_mpfr
    (mag_t      lpMag,
     LPAPLVFP   lpVfp)

{
    fmpr_t fmpr;

    // Initialize
    fmpr_init (fmpr);

    // Convert from MPFR to FMPR
    fmpr_set_mpfr (fmpr, lpVfp);

    // Convert from FMPR to MAG
    mag_set_fmpr (lpMag, fmpr);

    // We no longer need this storage
    fmpr_clear (fmpr);
} // End mag_get_mpfr


//***************************************************************************
//  $arb_gamma2
//***************************************************************************

void arb_gamma2
    (      arb_t z,
     const arb_t x,
           slong prec)

{
    mpfr_t lo = {0},
           hi = {0};
    static mpfr_t LocalMin = {0};

    if (LocalMin[0]._mpfr_d EQ NULL)
    {
        mpfr_init    (LocalMin);
        mpfr_set_str (LocalMin,
                      "1.461632144968362341262659542325721328468196204006446351295988409",
                      10,
                      MPFR_RNDU);
    } // End IF

    // Because the Gamma function is monotonically increasing
    //   above LocalMin, we can calculate Gamma on the endpoints
    //   and convert that interval into a Ball.
    //   This tends to be a tighter Ball than given by <arb_gamma>.

    // Initialize
    mpfr_inits (lo, hi, NULL);

    // Get the endpoints
    arb_get_interval_mpfr (lo, hi, x);

    // If the low point is above the Local Minimum, ...
    if (mpfr_cmp (lo, LocalMin) > 0)
    {
        // Use MPFR to calculate the Gamma function on the endpoints
        mpfr_gamma (lo, lo, MPFR_RNDD);
        mpfr_gamma (hi, hi, MPFR_RNDU);

        // Convert from Endpoints to Midpoint-Radius
        arb_set_interval_mpfr2 (z, lo, hi, prec);
    } else
        // Let ARB handle it
        arb_gamma (z, x, prec);

    // We no longer need this storage
    mpfr_clears (lo, hi, NULL);
} // End arb_gamma2


//***************************************************************************
//  $arb_nonpositive_part
//
//  Missing coounterpart to <arb_nonnegative_part>.
//***************************************************************************

void arb_nonpositive_part
    (LPAPLARB aplDst,               // Destination
     LPAPLARB aplSrc)               // Source

{
    // Negate so we can use <arb_nonnegative_part>
    arb_neg (aplDst, aplSrc);

    // Remove the negative parts
    arb_nonnegative_part (aplDst, aplDst);

    // Negate to undo the above <arb_neg>
    arb_neg (aplDst, aplDst);
} // End arb_nonpositive_part


//***************************************************************************
//  $arb_eq2
//
//  Return TRUE iff the Balls are identical
//***************************************************************************

UBOOL arb_eq2
    (arb_t arbLft,                  // Left arg
     arb_t arbRht)                  // Right ...

{
    return (arb_equal (arbLft, arbRht) NE 0);
} // End arb_eq2


//***************************************************************************
//  $arb_ne2
//
//  Return TRUE iff the Balls have no overlap
//***************************************************************************

UBOOL arb_ne2
    (arb_t arbLft,                  // Left arg
     arb_t arbRht)                  // Right ...

{
    mpfr_t Lftlo = {0},
           Lfthi = {0},
           Rhtlo = {0},
           Rhthi = {0};
    UBOOL  bRet;                    // The result

    // Initialize
    mpfr_inits (Lftlo, Lfthi, Rhtlo, Rhthi, NULL);

    // Get the endpoints
    arb_get_interval_mpfr (Lftlo, Lfthi, arbLft);
    arb_get_interval_mpfr (Rhtlo, Rhthi, arbRht);

    bRet = mpfr_less_p    (Lfthi, Rhtlo)
        || mpfr_greater_p (Lftlo, Rhthi);

    // We no longer need this storage
    mpfr_clears (Lftlo, Lfthi, Rhtlo, Rhthi, NULL);

    return bRet;
} // End arb_ne2


#ifdef DEBUG
//***************************************************************************
//  $ArbOut
//***************************************************************************

void ArbOut
    (LPWCHAR  lpwTxt,
     LPAPLARB lpArb)

{
    WCHAR wszTemp[512];

    strcpyW (wszTemp, lpwTxt);
    if (lpArb NE NULL)
       *FormatAplArb (&wszTemp[lstrlenW (wszTemp)], lpArb, 0) = WC_EOS;
    strcatW (wszTemp, WS_CRLF);
    DbgMsgW (wszTemp);
} // End ArbOut
#endif


#ifdef DEBUG
//***************************************************************************
//  $VfpOut
//***************************************************************************

void VfpOut
    (LPWCHAR  lpwTxt,
     LPAPLVFP lpVfp)

{
    WCHAR wszTemp[512];

    strcpyW (wszTemp, lpwTxt);
    if (lpVfp NE NULL)
       *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], lpVfp, 0) = WC_EOS;
    strcatW (wszTemp, WS_CRLF);
    DbgMsgW (wszTemp);
} // End VfpOut
#endif


#ifdef DEBUG
//***************************************************************************
//  $RatOut
//***************************************************************************

void RatOut
    (LPWCHAR  lpwTxt,
     LPAPLRAT lpRat)

{
    WCHAR wszTemp[512];

    strcpyW (wszTemp, lpwTxt);
    if (lpRat NE NULL)
       *FormatAplRat (&wszTemp[lstrlenW (wszTemp)], lpRat) = WC_EOS;
    strcatW (wszTemp, WS_CRLF);
    DbgMsgW (wszTemp);
} // End RatOut
#endif


#ifdef DEBUG
//***************************************************************************
//  $FltOut
//***************************************************************************

void FltOut
    (LPWCHAR    lpwTxt,
     LPAPLFLOAT lpFlt)

{
    WCHAR wszTemp[512];

    strcpyW (wszTemp, lpwTxt);
    if (lpFlt NE NULL)
       *FormatAplFlt (&wszTemp[lstrlenW (wszTemp)], lpFlt, 18) = WC_EOS;
    strcatW (wszTemp, WS_CRLF);
    DbgMsgW (wszTemp);
} // End FltOut
#endif


//***************************************************************************
//  End of File: mpfns.c
//***************************************************************************
