//***************************************************************************
//  NARS2000 -- Multi Precision Functions
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
      dlmalloc (size);
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

            wsprintfW (wszTemp,
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
      dlrealloc (lpMem, new_size);
#endif
    if (lpMemRes EQ NULL)
    {
#ifdef DEBUG
        APLINT aplOldSize = old_size,
               aplUseSize = dlmalloc_usable_size (lpMem),
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

            wsprintfW (wszTemp,
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
    dlfree (lpMem);
#endif
#ifdef DEBUG
    if (gDbgLvl >= gVfpLvl)
    {
        WCHAR wszTemp[256];

        wsprintfW (wszTemp,
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

unsigned long int mpz_invalid
    (enum MP_ENUM mp_enum,
     mpz_t             rop,
     mpz_t             op1,
     mpz_t             op2,
     mpz_t             op3,
     unsigned long int a,
     unsigned long int b)

{
    DbgBrk ();





    return 0;       // To keep the compiler happy
} // End mpz_invalid


//***************************************************************************
//  $mpq_invalid
//***************************************************************************

unsigned long int mpq_invalid
    (enum MP_ENUM mp_enum,
     mpq_t             rop,
     mpq_t             op1,
     mpq_t             op2,
     mpq_t             op3,
     unsigned long int d)

{
    DbgBrk ();





    return 0;       // To keep the compiler happy
} // End mpq_invalid


//***************************************************************************
//  $mpf_invalid
//***************************************************************************

unsigned long int mpf_invalid
    (enum MP_ENUM mp_enum,
     mpf_t             rop,
     mpf_t             op1,
     mpf_t             op2,
     mpf_t             op3,
     unsigned long int d)

{
    DbgBrk ();





    return 0;       // To keep the compiler happy
} // End mpf_invalid


//***************************************************************************
//  MPZ Functions
//***************************************************************************

//***************************************************************************
//  $mpz_QuadICValue
//
//  Return the appropriate []IC value
//***************************************************************************

APLMPI mpz_QuadICValue
    (IC_INDICES icNdx,
     APLMPI     mpzRes)

{
    switch (GetQuadICValue (icNdx))
    {
        case ICVAL_NEG1:
            // Initialize the result to -1
            mpz_init_set_si (&mpzRes, -1);

            return mpzRes;

        case ICVAL_ZERO:
            // Initialize the result to 0
            mpz_init (&mpzRes);

            return mpzRes;

        case ICVAL_ONE:
            // Initialize the result to 1
            mpz_init_set_si (&mpzRes,  1);

            return mpzRes;

        case ICVAL_DOMAIN_ERROR:
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        case ICVAL_POS_INFINITY:
            return mpzPosInfinity;

        case ICVAL_NEG_INFINITY:
            return mpzNegInfinity;

        defstop
            return mpzRes;
    } // End SWITCH
} // End mpz_QuadICValue


//***************************************************************************
//  $mpz_init_set_f
//
//  Save an APLVFP value as an MP Integer
//***************************************************************************

void mpz_init_set_f
    (mpz_ptr  dest,
     LPAPLVFP val)

{
    mpz_init (dest);
    mpz_set_f (dest, val);
} // End mpz_init_set_f


#if FALSE
//***************************************************************************
//  $mpz_get_sa
//
//  Convert an APLMPI to an APLINT
//***************************************************************************

APLINT mpz_get_sa
    (mpz_ptr src,               // Ptr to source
     LPUBOOL lpbRet)            // Ptr to TRUE iff result is valid (may be NULL)

{
    UBOOL        bRet;
    APLUINTUNION dwSplit;

    // Check the range
    bRet = (0 <= mpz_cmp (src, &mpzMinInt)
         &&      mpz_cmp (src, &mpzMaxInt) <= 0);
    if (lpbRet)
        *lpbRet = bRet;
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

    while (!mpz_likely_prime_p (mpzRes, lpMemPTD->randState, 0))
    {
        if (CheckCtrlBreak (*lpbCtrlBreak))
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

    while (!mpz_likely_prime_p (mpzRes, lpMemPTD->randState, 0))
    {
        if (CheckCtrlBreak (*lpbCtrlBreak))
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
    if (mpzVal->_mp_d EQ NULL)
        mpz_init (mpzVal);
} // End Myz_init


//***************************************************************************
//  $Myz_clear
//***************************************************************************

void Myz_clear
    (MP_INT *mpzVal)

{
    if (mpzVal->_mp_d)
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

APLRAT mpq_QuadICValue
    (APLRAT     aplRatLft,
     IC_INDICES icNdx,
     APLRAT     aplRatRht,
     APLRAT     mpqRes)

{
    switch (GetQuadICValue (icNdx))
    {
        case ICVAL_NEG1:
            // Initialize the result to -1
            mpq_init_set_si (&mpqRes, -1, 1);

            return mpqRes;

        case ICVAL_ZERO:
            // Initialize the result to 0
            mpq_init (&mpqRes);

            return mpqRes;

        case ICVAL_ONE:
            // Initialize the result to 1
            mpq_init_set_si (&mpqRes,  1, 1);

            return mpqRes;

        case ICVAL_DOMAIN_ERROR:
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        case ICVAL_POS_INFINITY:
            return mpqPosInfinity;

        case ICVAL_NEG_INFINITY:
            return mpqNegInfinity;

        case ICVAL_LEFT:
            mpq_init_set (&mpqRes, &aplRatLft);

            return mpqRes;

        case ICVAL_RIGHT:
            mpq_init_set (&mpqRes, &aplRatRht);

            return mpqRes;

        defstop
            return mpqRes;
    } // End SWITCH
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
     UINT    num,
     UINT    den)

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
//  $mpq_init_set_f
//
//  Save a VFP value as a Rational
//***************************************************************************

void mpq_init_set_f
    (mpq_ptr dest,
     mpf_ptr src)

{
    mpq_init (dest);
    mpq_set_f (dest, src);
} // End mpq_init_set_f


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
    (mpq_ptr src,
     LPUBOOL lpbRet)

{
    APLMPI mpzDiv = {0};
    APLINT aplInteger;
    UBOOL  bRet;

    // Check the range
    bRet = (0 <= mpz_cmp (mpq_numref (src), &mpzMinInt)
         &&      mpz_cmp (mpq_numref (src), &mpzMaxInt) <= 0);
    if (lpbRet)
        *lpbRet = bRet;
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
//  $mpq_get_ctsa
//
//  Convert an APLRAT to an APLINT within system CT
//***************************************************************************

APLINT mpq_get_ctsa
    (mpq_ptr src,
     LPUBOOL lpbRet)

{
    APLVFP mpfSrc  = {0},
           mpfTmp1 = {0},
           mpfTmp2 = {0};
    APLINT aplInt;

    // Initialize the temps
    mpf_init (&mpfSrc );
    mpf_init (&mpfTmp1);
    mpf_init (&mpfTmp2);

    // Convert the RAT to a VFP
    mpf_set_q (&mpfSrc , src);

    // Get the floor
    mpf_floor (&mpfTmp1, &mpfSrc);

    // Get the ceil
    mpf_ceil  (&mpfTmp2, &mpfSrc);

    // Calculate the relative difference between the source and its floor
    mpf_reldiff (&mpfTmp1, &mpfTmp1, &mpfSrc);

    // Calculate the relative difference between the source and its ceil
    mpf_reldiff (&mpfTmp2, &mpfTmp2, &mpfSrc);

    // Compare the relative diff with SYS_CT
    if (fabs (mpf_get_d (&mpfTmp1)) < SYS_CT)
    {
        mpf_floor (&mpfTmp1, &mpfSrc);
        aplInt = mpf_get_sx (&mpfTmp1, lpbRet);
    } else
    if (fabs (mpf_get_d (&mpfTmp2)) < SYS_CT)
    {
        mpf_ceil  (&mpfTmp2, &mpfSrc);
        aplInt = mpf_get_sx (&mpfTmp2, lpbRet);
    } else
        aplInt = mpf_get_sx (&mpfSrc , lpbRet);

    // We no longer need this storage
    Myf_clear (&mpfTmp2);
    Myf_clear (&mpfTmp1);
    Myf_clear (&mpfSrc );

    return aplInt;
} // End mpq_get_ctsa


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
//  &mpq_sub_ui
//***************************************************************************

void mpq_sub_ui
    (mpq_ptr dest,
     mpq_ptr src,
     UINT    num,
     UINT    den)

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
//  &mpq_floor
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
//  &mpq_ceil
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
//  &mpq_mod
//
//  Calculate aplLft % aplRht
//***************************************************************************

void mpq_mod
    (mpq_ptr dest,
     mpq_ptr aplLft,
     mpq_ptr aplRht)

{
    APLRAT aplTmp = {0};

    // Initialize the temp
    mpq_init (&aplTmp);

    mpq_div   (&aplTmp,  aplLft,  aplRht);      // T = L / R
    mpq_floor (&aplTmp, &aplTmp);               // T = floor (T)
    mpq_mul   (&aplTmp,  aplRht, &aplTmp);      // T = R * T
    mpq_sub   (dest   ,  aplLft, &aplTmp);      // Z = L - T

    // We no longer need this storage
    Myq_clear (&aplTmp);
} // End mpq_mod


//***************************************************************************
//  $mpq_cmp_ct
//
//  Compare two RATs relative to a given comparison tolerance
//
//  Return +1 if Lft >  Rht
//          0 if Lft EQ Rht
//         -1 if Lft <  Rht
//***************************************************************************

int mpq_cmp_ct
    (APLRAT   aplRatLft,
     APLRAT   aplRatRht,
     APLFLOAT fQuadCT)

{
    APLVFP mpfLft = {0},
           mpfRht = {0};
    UBOOL  bRet;

    // So as to avoid dividing by zero, if either arg is zero, ...
    if (!IsMpq0 (&aplRatLft)
     && !IsMpq0 (&aplRatRht))
    {
        // Initialize the temps
        mpf_init (&mpfLft);
        mpf_init (&mpfRht);

        // Copy the RAT as a VFP
        mpf_set_q (&mpfLft, &aplRatLft);
        mpf_set_q (&mpfRht, &aplRatRht);

        // Compare the two VFPs relative to []CT
        bRet = mpf_cmp_ct (mpfLft, mpfRht, fQuadCT) EQ 0;

        // We no longer need this storage
        Myf_clear (&mpfRht);
        Myf_clear (&mpfLft);

        if (bRet)
            return 0;
    } // End IF

    return mpq_cmp (&aplRatLft, &aplRatRht);
} // End mpq_cmp_ct


//***************************************************************************
//  $Myq_init
//***************************************************************************

void Myq_init
    (LPAPLRAT mpqVal)

{
    if (mpqVal->_mp_num._mp_d EQ NULL)
        mpq_init (mpqVal);
} // End Myq_init


//***************************************************************************
//  $Myq_clear
//***************************************************************************

void Myq_clear
    (LPAPLRAT mpqVal)

{
    if (mpqVal->_mp_num._mp_d)
    {
        mpq_clear (mpqVal);
        mpqVal->_mp_num._mp_d =
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
    return mpz_divisible_p (mpq_numref (op), mpq_denref (op));
} // mpq_integer_p


//***************************************************************************
//  MPF Functions
//***************************************************************************

//***************************************************************************
//  $mpf_QuadICValue
//
//  Return the appropriate []IC value
//***************************************************************************

APLVFP mpf_QuadICValue
    (APLVFP     aplVfpLft,
     IC_INDICES icNdx,
     APLVFP     aplVfpRht,
     APLVFP     mpfRes)

{
    switch (GetQuadICValue (icNdx))
    {
        case ICVAL_NEG1:
            // Initialize the result to -1
            mpf_init_set_si (&mpfRes, -1);

            return mpfRes;

        case ICVAL_ZERO:
            // Initialize the result to 0
            mpf_init (&mpfRes);

            return mpfRes;

        case ICVAL_ONE:
            // Initialize the result to 1
            mpf_init_set_si (&mpfRes,  1);

            return mpfRes;

        case ICVAL_DOMAIN_ERROR:
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        case ICVAL_POS_INFINITY:
            return mpfPosInfinity;

        case ICVAL_NEG_INFINITY:
            return mpfNegInfinity;

        case ICVAL_LEFT:
            mpf_init_copy (&mpfRes, &aplVfpLft);

            return mpfRes;

        case ICVAL_RIGHT:
            mpf_init_copy (&mpfRes, &aplVfpRht);

            return mpfRes;

        defstop
            return mpfRes;
    } // End SWITCH
} // End mpf_QuadICValue


//***************************************************************************
//  $mpf_copy
//
//  Save an MPF value as a Variable FP with the same precision
//***************************************************************************

void mpf_copy
    (mpf_ptr dest,
     mpf_ptr val)

{
    mpf_set_prec (dest, mpf_get_prec (val));
    mpf_set      (dest, val);
} // End mpf_copy


//***************************************************************************
//  $mpf_set_sx
//
//  Save an APLINT value as a Variable FP
//***************************************************************************

void mpf_set_sx
    (mpf_ptr dest,
     APLINT  val)

{
    // Get the high-order long
    mpf_set_si (dest, HIAPLINT (val));

    // Shift to upper long
    mpf_mul_2exp (dest, dest, BITS_IN_APLINT / 2);

    // Add in the low long
    mpf_add_ui (dest, dest, LOAPLINT (val));
} // End mpf_set_sx


//***************************************************************************
//  $mpf_init_copy
//
//  Save an MPF value as a Variable FP with the same precision
//***************************************************************************

void mpf_init_copy
    (mpf_ptr dest,
     mpf_ptr val)

{
    mpf_init2 (dest, mpf_get_prec (val));
    mpf_set   (dest, val);
} // End mpf_init_copy


//***************************************************************************
//  $mpf_init_set_sx
//
//  Save an APLINT value as a Variable FP
//***************************************************************************

void mpf_init_set_sx
    (mpf_ptr dest,
     APLINT  val)

{
    mpf_init (dest);
    mpf_set_sx (dest, val);
} // End mpf_init_set_sx


//***************************************************************************
//  $mpf_init_set_q
//
//  Save an APLRAT value as a Variable FP
//***************************************************************************

void mpf_init_set_q
    (mpf_ptr   dest,
     LPAPLRAT  lpVal)

{
    mpf_init (dest);
    mpf_set_q (dest, lpVal);
} // End mpf_init_set_q


//***************************************************************************
//  $mpf_init_set_z
//
//  Save an APLMPI value as a Variable FP
//***************************************************************************

void mpf_init_set_z
    (mpf_ptr   dest,
     LPAPLMPI  lpVal)

{
    mpf_init (dest);
    mpf_set_z (dest, lpVal);
} // End mpf_init_set_z


//***************************************************************************
//  $mpf_get_sx
//
//  Convert a VFP to an APLINT
//***************************************************************************

APLINT mpf_get_sx
    (mpf_ptr src,
     LPUBOOL lpbRet)

{
    APLRAT mpqTmp = {0};
    APLINT aplInteger;

    // Initialize the temp
    mpq_init (&mpqTmp);

    // Convert the number to RAT
    mpq_set_f (&mpqTmp, src);

    // Get the integer within
    aplInteger = mpq_get_sx (&mpqTmp, lpbRet);

    // We no longer need this storage
    Myq_clear (&mpqTmp);

    return aplInteger;
} // End mpf_get_sx


//***************************************************************************
//  $mpf_get_ctsa
//
//  Convert an APLVFP to an APLINT within system CT
//***************************************************************************

APLINT mpf_get_ctsa
    (mpf_ptr src,
     LPUBOOL lpbRet)

{
    APLVFP mpfTmp1 = {0},
           mpfTmp2 = {0};
    APLINT aplInt;

    // Initialize the temps
    mpf_init (&mpfTmp1);
    mpf_init (&mpfTmp2);

    // Get the floor
    mpf_floor (&mpfTmp1, src);

    // Get the ceil
    mpf_ceil  (&mpfTmp2, src);

    // Calculate the relative difference between the source and its floor
    mpf_reldiff (&mpfTmp1, &mpfTmp1, src);

    // Calculate the relative difference between the source and its ceil
    mpf_reldiff (&mpfTmp2, &mpfTmp2, src);

    // Compare the relative diff with []CT
    if (fabs (mpf_get_d (&mpfTmp1)) < SYS_CT)
    {
        mpf_floor (&mpfTmp1, src);
        aplInt = mpf_get_sx (&mpfTmp1, lpbRet);
    } else
    if (fabs (mpf_get_d (&mpfTmp2)) < SYS_CT)
    {
        mpf_ceil  (&mpfTmp2, src);
        aplInt = mpf_get_sx (&mpfTmp2, lpbRet);
    } else
        aplInt = mpf_get_sx (src     , lpbRet);

    // We no longer need this storage
    Myf_clear (&mpfTmp2);
    Myf_clear (&mpfTmp1);

    return aplInt;
} // End mpf_get_ctsa


//***************************************************************************
//  $mpf_get_ua
//
//  Convert a VFP to an APLUINT
//***************************************************************************

APLUINT mpf_get_ua
    (mpf_ptr src,
     LPUBOOL lpbRet)

{
    APLUINTUNION dwSplit;

    // Check the fractional part and range
    *lpbRet = (mpf_integer_p (src) NE 0
            && 0 <= mpf_cmp (src, &mpfMinInt)
            &&      mpf_cmp (src, &mpfMaxInt) <= 0);
    // If the fractional part is zero and the integer part is in range, ...
    if (*lpbRet)
    {
        APLVFP aplTmp;

        mpf_init     (&aplTmp);
        mpf_div_2exp (&aplTmp, src    , BITS_IN_APLINT / 2);
        dwSplit.dwords[1] = mpf_get_ui (&aplTmp);
        mpf_mul_2exp (&aplTmp, &aplTmp, BITS_IN_APLINT / 2);
        mpf_sub      (&aplTmp, src    , &aplTmp);
        dwSplit.dwords[0] = mpf_get_ui (&aplTmp);

        // We no longer need this storage
        Myf_clear (&aplTmp);

        return dwSplit.aplUInt;
    } else
        // Return a known value
        return 0;
} // End mpf_get_ua


//***************************************************************************
//  $mpf_mod
//
//  Calculate aplLft % aplRht
//***************************************************************************

void mpf_mod
    (mpf_ptr dest,          // Destination
     mpf_ptr aplOpr,        // Operand
     mpf_ptr aplMod)        // Modulus

{
    // Due to precision limitations, the result of <mpf_mod_sub>
    //   might not be less than the modulus, so we iterate until it is
    mpf_mod_sub (dest, aplOpr, aplMod);
    while (mpf_cmp (dest, aplMod) > 0)
        mpf_mod_sub (dest, dest  , aplMod);
} // End mpf_mod


//***************************************************************************
//  $mpf_mod_sub
//
//  Calculate aplLft % aplRht
//***************************************************************************

void mpf_mod_sub
    (mpf_ptr dest,          // Destination
     mpf_ptr aplOpr,        // Operand
     mpf_ptr aplMod)        // Modulus

{
    APLVFP aplTmp = {0};
#ifdef DEBUG
    char szTemp1[1024],
         szTemp2[1024];
    int  expptr1,
         expptr2;
#endif

    // Initialize the temp
    mpf_init (&aplTmp);

#ifdef DEBUG
    mpf_get_str (szTemp1, &expptr1, 10, 200, aplOpr);
    mpf_get_str (szTemp2, &expptr2, 10, 200, aplMod);
#endif
    mpf_div   (&aplTmp,  aplOpr,  aplMod);
#ifdef DEBUG
    mpf_get_str (szTemp1, &expptr1, 10, 200, &aplTmp);
#endif
    mpf_floor (&aplTmp, &aplTmp);
#ifdef DEBUG
    mpf_get_str (szTemp1, &expptr1, 10, 200, &aplTmp);
#endif
    mpf_mul   (&aplTmp,  aplMod, &aplTmp);
#ifdef DEBUG
    mpf_get_str (szTemp1, &expptr1, 10, 200, &aplTmp);
#endif
    mpf_sub   (dest   ,  aplOpr, &aplTmp);
#ifdef DEBUG
    mpf_get_str (szTemp1, &expptr1, 10, 200, dest);
#endif

    // We no longer need this storage
    Myf_clear (&aplTmp);
} // End mpf_mod_sub


//***************************************************************************
//  $mpf_cmp_ct
//
//  Compare two VFPs relative to a given comparison tolerance
//
//  Return +1 if Lft >  Rht
//          0 if Lft EQ Rht
//         -1 if Lft <  Rht
//***************************************************************************

int mpf_cmp_ct
    (APLVFP   aplVfpLft,
     APLVFP   aplVfpRht,
     APLFLOAT fQuadCT)

{
#if defined (DEBUG) && FALSE
    WCHAR  wszTemp[1024];
    APLVFP mpfFmt = {0};

    *FormatAplVfp (wszTemp, aplVfpLft, 0) = WC_EOS; DbgMsgW (wszTemp);
    *FormatAplVfp (wszTemp, aplVfpRht, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
    // So as to avoid dividing by zero, if neither arg is zero, ...
    if (!IsMpf0 (&aplVfpLft)                    // Lft NE 0
     && !IsMpf0 (&aplVfpRht)                    // Rht NE 0
     && mpf_cmp (&aplVfpLft, &aplVfpRht) NE 0   // Lft NE Rht
     && fQuadCT NE 0)                           // []CT NE 0
    {
        APLVFP mpfLftAbs = {0},
               mpfRhtAbs = {0};
        UBOOL  bRet = TRUE;         // TRUE iff the result in iRet is valid
        int    iRet;                // The result

        // Use an algorithm similar to the one in _CompareCT

        // Initialize the temps
        mpf_init (&mpfLftAbs);
        mpf_init (&mpfRhtAbs);

        // Get the absolute values
        mpf_abs (&mpfLftAbs, &aplVfpLft);
        mpf_abs (&mpfRhtAbs, &aplVfpRht);

        // If the signs differ, ...
        if (mpf_sgn (&mpfLftAbs) NE mpf_sgn (&mpfRhtAbs))
            bRet = FALSE;
        else
        {
            APLVFP mpfCT     = {0},
                   mpfHoodLo = {0};

            // Initialize the temps
            mpf_init (&mpfCT);
            mpf_init (&mpfHoodLo);

            // Convert the comparison tolerance
            mpf_set_d (&mpfCT, fQuadCT);

            // Calculate the low end of the left neighborhood of (|Rht)
////////////aplHoodLo = aplRhtAbs - aplRhtAbs * fQuadCT;
            mpf_mul (&mpfHoodLo, &mpfRhtAbs, &mpfCT);
            mpf_sub (&mpfHoodLo, &mpfRhtAbs, &mpfHoodLo);

            // If (|Rht) is greater than (|Lft),
            // and (|Lft) is in the
            //    left-neighborhood of (|Rht) with CT, return 1
////////////if (aplHoodLo <= aplLftAbs
//////////// &&              aplLftAbs < aplRhtAbs)
////////////    return TRUE;
            if (mpf_cmp (&mpfHoodLo, &mpfLftAbs)             <= 0
             && mpf_cmp (            &mpfLftAbs, &mpfRhtAbs) <  0)
                iRet = 0;
            else
            {
                // Calculate the low end of the left neighborhood of (|Lft)
////////////////aplHoodLo = aplLftAbs - aplLftAbs * fQuadCT;
                mpf_mul (&mpfHoodLo, &mpfLftAbs, &mpfCT);
                mpf_sub (&mpfHoodLo, &mpfLftAbs, &mpfHoodLo);

                // If (|Lft) is greater than (|Rht),
                // and (|Rht) is in the
                //    left-neighborhood of (|Lft) with CT, return 1
////////////////if (aplHoodLo <= aplRhtAbs
//////////////// &&              aplRhtAbs < aplLftAbs)
////////////////    return TRUE;
                if (mpf_cmp (&mpfHoodLo, &mpfRhtAbs)             <= 0
                 && mpf_cmp (            &mpfRhtAbs, &mpfLftAbs) <  0)
                    iRet = 0;
                else
                    bRet = FALSE;
            } // End IF/ELSE

#if defined (DEBUG) && FALSE
            *FormatAplVfp (wszTemp, mpfTmp, 0) = WC_EOS;
            DbgMsgW (wszTemp);
            mpf_init_set_d (&mpfFmt, fQuadCT);
            *FormatAplVfp (wszTemp, mpfFmt, 0) = WC_EOS;
            DbgMsgW (wszTemp);
            Myf_clear (&mpfFmt);
#endif
            // We no longer need this storage
            Myf_clear (&mpfHoodLo);
            Myf_clear (&mpfCT    );
        } // End IF/ELSE

        // We no longer need this storage
        Myf_clear (&mpfRhtAbs);
        Myf_clear (&mpfLftAbs);

        if (bRet)
            return iRet;
    } // End IF

    return mpf_cmp (&aplVfpLft, &aplVfpRht);
} // End mpf_cmp_ct


//***************************************************************************
//  $mpf_pow
//***************************************************************************

void mpf_pow
    (mpf_ptr dest,          // Ptr to (initialized) destination
     mpf_ptr base,          // Ptr to base
     mpf_ptr exp)           // Ptr to exponent

{
    APLVFP mpfTmp;

    // Call common code for exponentiation
    mpfTmp = PrimFnDydStarVisVvV (*base, *exp, NULL);

    // Copy to the destination
    mpf_copy (dest, &mpfTmp);

    // We no longer need this storage
    Myf_clear (&mpfTmp);
} // End mpf_pow


//***************************************************************************
//  $mpf_ui_cmp
//
//  Compare an unsigned long int and a VFP
//***************************************************************************

int mpf_ui_cmp
    (unsigned long int op1,     // Left arg
     mpf_t             op2)     // Right arg

{
    return -mpf_cmp_ui (op2, op1);
} // End mpf_ui_cmp


//***************************************************************************
//  $mpf_si_cmp
//
//  Compare a signed long int and a VFP
//***************************************************************************

int mpf_si_cmp
    (signed long int op1,       // Left arg
     mpf_t           op2)       // Right arg

{
    return -mpf_cmp_si (op2, op1);
} // End mpf_si_cmp


//***************************************************************************
//  $Myf_init
//***************************************************************************

void Myf_init
    (LPAPLVFP mpfVal)

{
    if (mpfVal->_mp_d EQ NULL)
        mpf_init (mpfVal);
    else
        mpf_set_prec (mpfVal, mpf_get_default_prec ());
} // End Myf_init


//***************************************************************************
//  $Myf_clear
//***************************************************************************

void Myf_clear
    (LPAPLVFP mpfVal)

{
    if (mpfVal->_mp_d)
    {
        mpf_clear (mpfVal);
        mpfVal->_mp_d = NULL;
    } // End IF
} // End Myf_clear


//***************************************************************************
//  End of File: primfns.c
//***************************************************************************
