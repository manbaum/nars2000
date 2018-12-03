//***************************************************************************
//  NARS2000 -- Multiple Precision CHO Common Routines
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
#include <math.h>
#include "headers.h"
#include "mpcho_com.mac"


//***************************************************************************
//  $mpXf_sin       (1)
//
//  The code below was translated to C from the original Javascript
//    http://tamivox.org/redbear/qtrn_calc/index.html
//  written by Dave Barber, Copyright 2015, Dave Barber.
//***************************************************************************

void mpXf_sin
    (mpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     mpcho_t    op,             // The argument (as one of mpcf_/mphf_/mpof_)
     mpfr_rnd_t rnd,            // Rounding mode
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    mpfr_t mpfIMag,             // Magnitude of imag parts in the arg
           mpfTmp1;             // Temp var
    mpc_t  Z;

    // Initialize to NaN
    mpfr_init (mpfIMag);
    mpfr_init (mpfTmp1);
    mpc_init2 (Z, mpfr_get_default_prec ());

    // Calculate the magnitude of the imag parts in the arg
    mpXf_mag_imag (mpfIMag, op, rnd, iHCDim, 1);                // g

    // Construct a complex number from h and g
    mpfr_set (Z->re, &op->parts[0], rnd);
    mpfr_set (Z->im, mpfIMag, rnd);

    // If the imaginary part is zero, ...
    if (IsMpf0 (mpfIMag))
    {
        // Call the Real number version so as to avoid
        //   inconsistencies in the two cases:
        //   Real vs. Complex (with imaginary part EQ 0)
        mpfr_sin    (Z->re, Z->re, MPFR_RNDN);
////////mpfr_set_ui (Z->im, 0, MPFR_RNDN);  // Already zero from <mpfr_set> above
    } else
        // Calculate the corresponding complex number
        mpc_sin (Z, Z, MPC_RNDNN);

    // Save as the real part of the result
    mpfr_set (&rop->parts[0], Z->re, rnd);

    // If the arg has any imag parts, ...
    if (!mpfr_zero_p (mpfIMag))                                         // g == 0 ?
    {
        // Calculate:  Z->im / g
        mpfr_div (mpfTmp1, Z->im, mpfIMag, rnd);

        // Multiply the imag parts of the arg by Z.im / g
        mpXf_scale (rop, op, mpfTmp1, iHCDim, rnd);
    } else
    // The imag part of the arg is zero
        // Save in first imag part of the result
        mpfr_set (&rop->parts[1], Z->im, rnd);

    // We no longer need this storage
    mpc_clear  (Z      );
    mpfr_clear (mpfTmp1);
    mpfr_clear (mpfIMag);
} // End mpXf_sin


//***************************************************************************
//  $mpXf_cos       (2)
//***************************************************************************

void mpXf_cos
    (mpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     mpcho_t    op,             // The argument (as one of mpcf_/mphf_/mpof_)
     mpfr_rnd_t rnd,            // Rounding mode
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    mpfr_t mpfIMag,             // Magnitude of imag parts in the arg
           mpfTmp1;             // Temp var
    mpc_t  Z;

    // Initialize to NaN
    mpfr_init (mpfIMag);
    mpfr_init (mpfTmp1);
    mpc_init2 (Z, mpfr_get_default_prec ());

    // Calculate the magnitude of the imag parts in the arg
    mpXf_mag_imag (mpfIMag, op, rnd, iHCDim, 1);                // g

    // Construct a complex number from h and g
    mpfr_set (Z->re, &op->parts[0], rnd);
    mpfr_set (Z->im, mpfIMag, rnd);

    // If the imaginary part is zero, ...
    if (IsMpf0 (mpfIMag))
    {
        // Call the Real number version so as to avoid
        //   inconsistencies in the two cases:
        //   Real vs. Complex (with imaginary part EQ 0)
        mpfr_cos    (Z->re, Z->re, MPFR_RNDN);
////////mpfr_set_ui (Z->im, 0, MPFR_RNDN);  // Already zero from <mpfr_set> above
    } else
        // Calculate the corresponding complex number
        mpc_cos (Z, Z, MPC_RNDNN);

    // Save as the real part of the result
    mpfr_set (&rop->parts[0], Z->re, rnd);

    // If the arg has any imag parts, ...
    if (!mpfr_zero_p (mpfIMag))                                         // g == 0 ?
    {
        // Calculate:  Z->im / g
        mpfr_div (mpfTmp1, Z->im, mpfIMag, rnd);

        // Multiply the imag parts of the arg by Z.im / g
        mpXf_scale (rop, op, mpfTmp1, iHCDim, rnd);
    } else
    // The imag part of the arg is zero
        // Save in first imag part of the result
        mpfr_set (&rop->parts[1], Z->im, rnd);

    // We no longer need this storage
    mpc_clear  (Z      );
    mpfr_clear (mpfTmp1);
    mpfr_clear (mpfIMag);
} // End mpXf_cos


//***************************************************************************
//  $mpXf_tan       (3)
//***************************************************************************

void mpXf_tan
    (mpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     mpcho_t    op,             // The argument (as one of mpcf_/mphf_/mpof_)
     mpfr_rnd_t rnd,            // Rounding mode
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    mpfr_t mpfIMag,             // Magnitude of imag parts in the arg
           mpfTmp1;             // Temp var
    mpc_t  Z;

    // Initialize to NaN
    mpfr_init (mpfIMag);
    mpfr_init (mpfTmp1);
    mpc_init2 (Z, mpfr_get_default_prec ());

    // Calculate the magnitude of the imag parts in the arg
    mpXf_mag_imag (mpfIMag, op, rnd, iHCDim, 1);                // g

    // Construct a complex number from h and g
    mpfr_set (Z->re, &op->parts[0], rnd);
    mpfr_set (Z->im, mpfIMag, rnd);

    // If the imaginary part is zero, ...
    if (IsMpf0 (mpfIMag))
    {
        // Call the Real number version so as to avoid
        //   inconsistencies in the two cases:
        //   Real vs. Complex (with imaginary part EQ 0)
        mpfr_tan    (Z->re, Z->re, MPFR_RNDN);
////////mpfr_set_ui (Z->im, 0, MPFR_RNDN);  // Already zero from <mpfr_set> above
    } else
        // Calculate the corresponding complex number
        mpc_tan (Z, Z, MPC_RNDNN);

    // Save as the real part of the result
    mpfr_set (&rop->parts[0], Z->re, rnd);

    // If the arg has any imag parts, ...
    if (!mpfr_zero_p (mpfIMag))                                         // g == 0 ?
    {
        // Calculate:  Z->im / g
        mpfr_div (mpfTmp1, Z->im, mpfIMag, rnd);

        // Multiply the imag parts of the arg by Z.im / g
        mpXf_scale (rop, op, mpfTmp1, iHCDim, rnd);
    } else
    // The imag part of the arg is zero
        // Save in first imag part of the result
        mpfr_set (&rop->parts[1], Z->im, rnd);

    // We no longer need this storage
    mpc_clear  (Z      );
    mpfr_clear (mpfTmp1);
    mpfr_clear (mpfIMag);
} // End mpXf_tan


//***************************************************************************
//  $mpXf_asin      (-1)
//***************************************************************************

void mpXf_asin
    (mpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     mpcho_t    op,             // The argument (as one of mpcf_/mphf_/mpof_)
     mpfr_rnd_t rnd,            // Rounding mode
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    mpfr_t mpfIMag,             // Magnitude of imag parts in the arg
           mpfTmp1;             // Temp var
    mpc_t  Z;

    // Initialize to NaN
    mpfr_init (mpfIMag);
    mpfr_init (mpfTmp1);
    mpc_init2 (Z, mpfr_get_default_prec ());

    // Calculate the magnitude of the imag parts in the arg
    mpXf_mag_imag (mpfIMag, op, rnd, iHCDim, 1);                // g

    // Construct a complex number from h and g
    mpfr_set (Z->re, &op->parts[0], rnd);
    mpfr_set (Z->im, mpfIMag, rnd);

    // In order to get correct CCC results from mpc_asin,
    //   we must ensure that if the imag part of the
    //   arg is zero, then its sign is opposite that of
    //   the real part of the arg
    if (mpfr_zero_p (mpfIMag))
    {
        // Ensure no sign on zero
        mpfr_abs (Z->im, Z->im, MPFR_RNDN);

        // If the real part is >= 1, ...
        if (mpfr_cmp_si (&op->parts[0], 1) >= 0)
            // Pass negative zero to mpc_asin
            mpfr_neg (Z->im, Z->im, MPFR_RNDN);
    } // End IF

#if (defined DEBUG) && FALSE
    VfpOut (L"Z.re:  ", &Z->re);
    VfpOut (L"Z.im:  ", &Z->im);
#endif
    // If the Imaginary part is zero, and
    //    the Real part is a NaN or within [-1,1], ...
    if (IsMpf0 (mpfIMag)
     && (mpfr_nan_p (&op->parts[0])
      || (mpfr_cmp_si (&op->parts[0],  1) <= 0
       && mpfr_cmp_si (&op->parts[0], -1) >= 0)))
    {
        // Call the Real number version so as to avoid
        //   inconsistencies in the two cases:
        //   Real vs. Complex (with imaginary part EQ 0)
        mpfr_asin   (Z->re, Z->re, MPFR_RNDN);
    } else
        // Calculate the corresponding complex number
        mpc_asin (Z, Z, MPC_RNDNN);

#if (defined DEBUG) && FALSE
    VfpOut (L"Z.re:  ", &Z->re);
    VfpOut (L"Z.im:  ", &Z->im);
#endif
    // Save as the real part of the result
    mpfr_set (&rop->parts[0], Z->re, rnd);

    // If the arg has any imag parts, ...
    if (!mpfr_zero_p (mpfIMag))                                         // g == 0 ?
    {
        // Calculate:  Z->im / g
        mpfr_div (mpfTmp1, Z->im, mpfIMag, rnd);

        // Multiply the imag parts of the arg by Z.im / g
        mpXf_scale (rop, op, mpfTmp1, iHCDim, rnd);
    } else
    // The imag part of the arg is zero
        // Save in first imag part of the result
        mpfr_set (&rop->parts[1], Z->im, rnd);

    // We no longer need this storage
    mpc_clear  (Z      );
    mpfr_clear (mpfTmp1);
    mpfr_clear (mpfIMag);
} // End mpXf_asin


//***************************************************************************
//  $mpXf_acos      (-2)
//***************************************************************************

void mpXf_acos
    (mpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     mpcho_t    op,             // The argument (as one of mpcf_/mphf_/mpof_)
     mpfr_rnd_t rnd,            // Rounding mode
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    mpfr_t mpfIMag,             // Magnitude of imag parts in the arg
           mpfTmp1;             // Temp var
    mpc_t  Z;

    // Initialize to NaN
    mpfr_init (mpfIMag);
    mpfr_init (mpfTmp1);
    mpc_init2 (Z, mpfr_get_default_prec ());

    // Calculate the magnitude of the imag parts in the arg
    mpXf_mag_imag (mpfIMag, op, rnd, iHCDim, 1);                // g

    // Construct a complex number from h and g
    mpfr_set (Z->re, &op->parts[0], rnd);
    mpfr_set (Z->im, mpfIMag, rnd);

    // In order to get correct CCC results from mpc_acos,
    //   we must ensure that if the imag part of the
    //   arg is zero, then its sign is opposite that of
    //   the real part of the arg
    if (mpfr_zero_p (mpfIMag))
    {
        // Ensure no sign on zero
        mpfr_abs (Z->im, Z->im, MPFR_RNDN);

        // If the real part is >= 1, ...
        if (mpfr_cmp_si (&op->parts[0], 1) >= 0)
            // Pass negative zero to mpc_acos
            mpfr_neg (Z->im, Z->im, MPFR_RNDN);
    } // End IF

#if (defined DEBUG) && FALSE
    VfpOut (L"Z.re:  ", &Z->re);
    VfpOut (L"Z.im:  ", &Z->im);
#endif
    // If the Imaginary part is zero, and
    //    the Real part is a NaN or within [-1,1], ...
    if (IsMpf0 (mpfIMag)
     && (mpfr_nan_p (&op->parts[0])
      || (mpfr_cmp_si (&op->parts[0],  1) <= 0
       && mpfr_cmp_si (&op->parts[0], -1) >= 0)))
    {
        // Call the Real number version so as to avoid
        //   inconsistencies in the two cases:
        //   Real vs. Complex (with imaginary part EQ 0)
        mpfr_acos   (Z->re, Z->re, MPFR_RNDN);
    } else
        // Calculate the corresponding complex number
        mpc_acos (Z, Z, MPC_RNDNN);

#if (defined DEBUG) && FALSE
    VfpOut (L"Z.re:  ", &Z->re);
    VfpOut (L"Z.im:  ", &Z->im);
#endif
    // Save as the real part of the result
    mpfr_set (&rop->parts[0], Z->re, rnd);

    // If the arg has any imag parts, ...
    if (!mpfr_zero_p (mpfIMag))                                         // g == 0 ?
    {
        // Calculate:  Z->im / g
        mpfr_div (mpfTmp1, Z->im, mpfIMag, rnd);

        // Multiply the imag parts of the arg by Z.im / g
        mpXf_scale (rop, op, mpfTmp1, iHCDim, rnd);
    } else
    // The imag part of the arg is zero
        // Save in first imag part of the result
        mpfr_set (&rop->parts[1], Z->im, rnd);

    // We no longer need this storage
    mpc_clear  (Z      );
    mpfr_clear (mpfTmp1);
    mpfr_clear (mpfIMag);
} // End mpXf_acos


//***************************************************************************
//  $mpXf_atan      (-3)
//***************************************************************************

void mpXf_atan
    (mpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     mpcho_t    op,             // The argument (as one of mpcf_/mphf_/mpof_)
     mpfr_rnd_t rnd,            // Rounding mode
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    mpfr_t mpfIMag,             // Magnitude of imag parts in the arg
           mpfTmp1;             // Temp var
    mpc_t  Z;

    // Initialize to NaN
    mpfr_init (mpfIMag);
    mpfr_init (mpfTmp1);
    mpc_init2 (Z, mpfr_get_default_prec ());

    // Calculate the magnitude of the imag parts in the arg
    mpXf_mag_imag (mpfIMag, op, rnd, iHCDim, 1);                // g

    // Construct a complex number from h and g
    mpfr_set (Z->re, &op->parts[0], rnd);
    mpfr_set (Z->im, mpfIMag, rnd);

    // In order to get correct CW continuity results from mpc_atan,
    //   we must ensure that if the real part of the
    //   arg is zero, then its sign is opposite that of
    //   the imag part of the arg
    if (mpfr_zero_p (&op->parts[0]))
    {
        // Ensure no sign on zero
        mpfr_abs (Z->re, Z->re, MPFR_RNDN);

        // If the imag part of the arg is >= 1, ... (CW)
        if (mpfr_cmp_si (&op->parts[1], 1) >= 0)
////    // If the imag part is <= -1, ... (CCW)
////    if (mpfr_cmp_si (&op->parts[1], -1) <=  0)
            // Pass negative zero to mpc_atan
            mpfr_neg (Z->re, Z->re, MPFR_RNDN);
    } // End IF

#if (defined DEBUG) && FALSE
    VfpOut (L"Z.re:  ", &Z->re);
    VfpOut (L"Z.im:  ", &Z->im);
#endif
    // Calculate the corresponding complex number
    mpc_atan (Z, Z, MPC_RNDNN);

#if (defined DEBUG) && FALSE
    VfpOut (L"Z.re:  ", &Z->re);
    VfpOut (L"Z.im:  ", &Z->im);
#endif
    // Save as the real part of the result
    mpfr_set (&rop->parts[0], Z->re, rnd);

    // If the arg has any imag parts, ...
    if (!mpfr_zero_p (mpfIMag))                                         // g == 0 ?
    {
        // Calculate:  Z->im / g
        mpfr_div (mpfTmp1, Z->im, mpfIMag, rnd);

        // Multiply the imag parts of the arg by Z.im / g
        mpXf_scale (rop, op, mpfTmp1, iHCDim, rnd);
    } else
    // The imag part of the arg is zero
        // Save in first imag part of the result
        mpfr_set (&rop->parts[1], Z->im, rnd);

////// If the real part of the arg is zero, ...
////if (mpfr_zero_p (&op->parts[0]))
////{
////    LPPERTABDATA lpMemPTD = GetMemPTD ();
////
////    // Initialize the VFP Pi if not already done
////    InitPTD_Pi (lpMemPTD);
////
////    // Set the real part of the result to ±Pi/2
////    mpfr_set    (&rop->parts[0], &lpMemPTD->mpfrHC8V_Pi.parts[0], rnd);
////    mpfr_div_ui (&rop->parts[0], &rop->parts[0],                  2, rnd);
////
////    // Set the sign of the real part of the result
////    //   to the opposite sign of the (first) imag part
////    //   of the result
////    if (mpfr_sgn (&op->parts[1]) EQ 1)
////        mpfr_neg (&rop->parts[0], &rop->parts[0], rnd);
////} // End IF

    // We no longer need this storage
    mpc_clear  (Z      );
    mpfr_clear (mpfTmp1);
    mpfr_clear (mpfIMag);
} // End mpXf_atan


//***************************************************************************
//  $mpXf_sinh      (5)
//***************************************************************************

void mpXf_sinh
    (mpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     mpcho_t    op,             // The argument (as one of mpcf_/mphf_/mpof_)
     mpfr_rnd_t rnd,            // Rounding mode
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    mpfr_t mpfIMag,             // Magnitude of imag parts in the arg
           mpfTmp1;             // Temp var
    mpc_t  Z;

    // Initialize to NaN
    mpfr_init (mpfIMag);
    mpfr_init (mpfTmp1);
    mpc_init2 (Z, mpfr_get_default_prec ());

    // Calculate the magnitude of the imag parts in the arg
    mpXf_mag_imag (mpfIMag, op, rnd, iHCDim, 1);                // g

    // Construct a complex number from h and g
    mpfr_set (Z->re, &op->parts[0], rnd);
    mpfr_set (Z->im, mpfIMag, rnd);

    // If the imaginary part is zero, ...
    if (IsMpf0 (mpfIMag))
    {
        // Call the Real number version so as to avoid
        //   inconsistencies in the two cases:
        //   Real vs. Complex (with imaginary part EQ 0)
        mpfr_sinh   (Z->re, Z->re, MPFR_RNDN);
////////mpfr_set_ui (Z->im, 0, MPFR_RNDN);  // Already zero from <mpfr_set> above
    } else
        // Calculate the corresponding complex number
        mpc_sinh (Z, Z, MPC_RNDNN);

    // Save as the real part of the result
    mpfr_set (&rop->parts[0], Z->re, rnd);

    // If the arg has any imag parts, ...
    if (!mpfr_zero_p (mpfIMag))                                         // g == 0 ?
    {
        // Calculate:  Z->im / g
        mpfr_div (mpfTmp1, Z->im, mpfIMag, rnd);

        // Multiply the imag parts of the arg by Z.im / g
        mpXf_scale (rop, op, mpfTmp1, iHCDim, rnd);
    } else
    // The imag part of the arg is zero
        // Save in first imag part of the result
        mpfr_set (&rop->parts[1], Z->im, rnd);

    // We no longer need this storage
    mpc_clear  (Z      );
    mpfr_clear (mpfTmp1);
    mpfr_clear (mpfIMag);
} // End mpXf_sinh


//***************************************************************************
//  $mpXf_cosh      (6)
//***************************************************************************

void mpXf_cosh
    (mpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     mpcho_t    op,             // The argument (as one of mpcf_/mphf_/mpof_)
     mpfr_rnd_t rnd,            // Rounding mode
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    mpfr_t mpfIMag,             // Magnitude of imag parts in the arg
           mpfTmp1;             // Temp var
    mpc_t  Z;

    // Initialize to NaN
    mpfr_init (mpfIMag);
    mpfr_init (mpfTmp1);
    mpc_init2 (Z, mpfr_get_default_prec ());

    // Calculate the magnitude of the imag parts in the arg
    mpXf_mag_imag (mpfIMag, op, rnd, iHCDim, 1);                // g

    // Construct a complex number from h and g
    mpfr_set (Z->re, &op->parts[0], rnd);
    mpfr_set (Z->im, mpfIMag, rnd);

    // If the imaginary part is zero, ...
    if (IsMpf0 (mpfIMag))
    {
        // Call the Real number version so as to avoid
        //   inconsistencies in the two cases:
        //   Real vs. Complex (with imaginary part EQ 0)
        mpfr_cosh   (Z->re, Z->re, MPFR_RNDN);
////////mpfr_set_ui (Z->im, 0, MPFR_RNDN);  // Already zero from <mpfr_set> above
    } else
        // Calculate the corresponding complex number
        mpc_cosh (Z, Z, MPC_RNDNN);

    // Save as the real part of the result
    mpfr_set (&rop->parts[0], Z->re, rnd);

    // If the arg has any imag parts, ...
    if (!mpfr_zero_p (mpfIMag))                                         // g == 0 ?
    {
        // Calculate:  Z->im / g
        mpfr_div (mpfTmp1, Z->im, mpfIMag, rnd);

        // Multiply the imag parts of the arg by Z.im / g
        mpXf_scale (rop, op, mpfTmp1, iHCDim, rnd);
    } else
    // The imag part of the arg is zero
        // Save in first imag part of the result
        mpfr_set (&rop->parts[1], Z->im, rnd);

    // We no longer need this storage
    mpc_clear  (Z      );
    mpfr_clear (mpfTmp1);
    mpfr_clear (mpfIMag);
} // End mpXf_cosh


//***************************************************************************
//  $mpXf_tanh      (7)
//***************************************************************************

void mpXf_tanh
    (mpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     mpcho_t    op,             // The argument (as one of mpcf_/mphf_/mpof_)
     mpfr_rnd_t rnd,            // Rounding mode
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    mpfr_t mpfIMag,             // Magnitude of imag parts in the arg
           mpfTmp1;             // Temp var
    mpc_t  Z;

    // Initialize to NaN
    mpfr_init (mpfIMag);
    mpfr_init (mpfTmp1);
    mpc_init2 (Z, mpfr_get_default_prec ());

    // Calculate the magnitude of the imag parts in the arg
    mpXf_mag_imag (mpfIMag, op, rnd, iHCDim, 1);                // g

    // Construct a complex number from h and g
    mpfr_set (Z->re, &op->parts[0], rnd);
    mpfr_set (Z->im, mpfIMag, rnd);

    // If the imaginary part is zero, ...
    if (IsMpf0 (mpfIMag))
    {
        // Call the Real number version so as to avoid
        //   inconsistencies in the two cases:
        //   Real vs. Complex (with imaginary part EQ 0)
        mpfr_tanh   (Z->re, Z->re, MPFR_RNDN);
////////mpfr_set_ui (Z->im, 0, MPFR_RNDN);  // Already zero from <mpfr_set> above
    } else
        // Calculate the corresponding complex number
        mpc_tanh (Z, Z, MPC_RNDNN);

    // Save as the real part of the result
    mpfr_set (&rop->parts[0], Z->re, rnd);

    // If the arg has any imag parts, ...
    if (!mpfr_zero_p (mpfIMag))                                         // g == 0 ?
    {
        // Calculate:  Z->im / g
        mpfr_div (mpfTmp1, Z->im, mpfIMag, rnd);

        // Multiply the imag parts of the arg by Z.im / g
        mpXf_scale (rop, op, mpfTmp1, iHCDim, rnd);
    } else
    // The imag part of the arg is zero
        // Save in first imag part of the result
        mpfr_set (&rop->parts[1], Z->im, rnd);

    // We no longer need this storage
    mpc_clear  (Z      );
    mpfr_clear (mpfTmp1);
    mpfr_clear (mpfIMag);
} // End mpXf_tanh


//***************************************************************************
//  $mpXf_asinh     (-5)
//***************************************************************************

void mpXf_asinh
    (mpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     mpcho_t    op,             // The argument (as one of mpcf_/mphf_/mpof_)
     mpfr_rnd_t rnd,            // Rounding mode
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    mpfr_t mpfIMag,             // Magnitude of imag parts in the arg
           mpfTmp1;             // Temp var
    mpc_t  Z;

    // Initialize to NaN
    mpfr_init (mpfIMag);
    mpfr_init (mpfTmp1);
    mpc_init2 (Z, mpfr_get_default_prec ());

    // Calculate the magnitude of the imag parts in the arg
    mpXf_mag_imag (mpfIMag, op, rnd, iHCDim, 1);                // g

    // Construct a complex number from h and g
    mpfr_set (Z->re, &op->parts[0], rnd);
    mpfr_set (Z->im, mpfIMag, rnd);

#if (defined DEBUG) && FALSE
    VfpOut (L"Z.re:  ", &Z->re);
    VfpOut (L"Z.im:  ", &Z->im);
#endif
    // Calculate the corresponding complex number
    mpc_asinh (Z, Z, MPC_RNDNN);
#if (defined DEBUG) && FALSE
    VfpOut (L"Z.re:  ", &Z->re);
    VfpOut (L"Z.im:  ", &Z->im);
#endif

    // Save as the real part of the result
    mpfr_set (&rop->parts[0], Z->re, rnd);

    // Because we may lose the sign of the imag part(s) of the arg
    //   when we compute mpfIMag, we need to check it now
    // This is a problem for both gsl_complex_arcsinh and mpc_asinh

    // If the real part of the arg is zero, ...
    if (mpfr_zero_p (&op->parts[0]))
    {
        // The sign of the real part of the result is the same sign
        //   as the imag part of the arg
        if ((mpfr_sgn (& op->parts[1]) EQ -1
          && mpfr_sgn (&rop->parts[0]) EQ  1)
         || (mpfr_sgn (& op->parts[1]) EQ  1
          && mpfr_sgn (&rop->parts[0]) EQ -1))
            mpfr_neg (&rop->parts[0], &rop->parts[0], rnd);
    } // End IF

    // If the arg has any imag parts, ...
    if (!mpfr_zero_p (mpfIMag))                                         // g == 0 ?
    {
        // Calculate:  Z->im / g
        mpfr_div (mpfTmp1, Z->im, mpfIMag, rnd);

        // Multiply the imag parts of the arg by Z.im / g
        mpXf_scale (rop, op, mpfTmp1, iHCDim, rnd);
    } else
    // The imag part of the arg is zero
        // Save in first imag part of the result
        mpfr_set (&rop->parts[1], Z->im, rnd);

    // We no longer need this storage
    mpc_clear  (Z      );
    mpfr_clear (mpfTmp1);
    mpfr_clear (mpfIMag);
} // End mpXf_asinh


//***************************************************************************
//  $mpXf_acosh     (-6)
//***************************************************************************

void mpXf_acosh
    (mpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     mpcho_t    op,             // The argument (as one of mpcf_/mphf_/mpof_)
     mpfr_rnd_t rnd,            // Rounding mode
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    mpfr_t mpfIMag,             // Magnitude of imag parts in the arg
           mpfTmp1;             // Temp var
    mpc_t  Z;

    // Initialize to NaN
    mpfr_init (mpfIMag);
    mpfr_init (mpfTmp1);
    mpc_init2 (Z, mpfr_get_default_prec ());

    // Calculate the magnitude of the imag parts in the arg
    mpXf_mag_imag (mpfIMag, op, rnd, iHCDim, 1);                // g

    // Construct a complex number from h and g
    mpfr_set (Z->re, &op->parts[0], rnd);
    mpfr_set (Z->im, mpfIMag, rnd);

    // Calculate the corresponding complex number
    mpc_acosh (Z, Z, MPC_RNDNN);

    // Save as the real part of the result
    mpfr_set (&rop->parts[0], Z->re, rnd);

    // If the arg has any imag parts, ...
    if (!mpfr_zero_p (mpfIMag))                                         // g == 0 ?
    {
        // Calculate:  Z->im / g
        mpfr_div (mpfTmp1, Z->im, mpfIMag, rnd);

        // Multiply the imag parts of the arg by Z.im / g
        mpXf_scale (rop, op, mpfTmp1, iHCDim, rnd);
    } else
    // The imag part of the arg is zero
        // Save in first imag part of the result
        mpfr_set (&rop->parts[1], Z->im, rnd);

    // We no longer need this storage
    mpc_clear  (Z      );
    mpfr_clear (mpfTmp1);
    mpfr_clear (mpfIMag);
} // End mpXf_acosh


//***************************************************************************
//  $Mympc_atanh    (-7)
//***************************************************************************

void Mympc_atanh
    (mpc_ptr    rop,
     mpc_srcptr op,
     mpc_rnd_t  rnd)

{
    mpc_t Zp1, Zm1;

    // Use NIST (A&S v2) of 0.5 * log ((1+z) / (1-z))
    mpc_init2 (Zp1, mpfr_get_default_prec ());
    mpc_init2 (Zm1, mpfr_get_default_prec ());

    mpc_add_si (Zp1,  op,   1, rnd);
    mpc_sub_ui (Zm1,  op,   1, rnd);
    mpc_neg    (Zm1, Zm1,      rnd);
    mpc_div    (rop, Zp1, Zm1, rnd);
    mpc_log    (rop, rop,      rnd);
    mpc_div_ui (rop, rop,   2, rnd);

    mpc_clear (Zm1);
    mpc_clear (Zp1);
} // End Mympc_atanh


//***************************************************************************
//  $mpXf_atanh     (-7)
//***************************************************************************

void mpXf_atanh
    (mpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     mpcho_t    op,             // The argument (as one of mpcf_/mphf_/mpof_)
     mpfr_rnd_t rnd,            // Rounding mode
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    mpfr_t mpfIMag,             // Magnitude of imag parts in the arg
           mpfTmp1;             // Temp var
    mpc_t  Z1, Z2;
    LPPERTABDATA lpMemPTD = GetMemPTD ();

    // Initialize the VFP Pi if not already done
    InitPTD_Pi (lpMemPTD);

    // Initialize to NaN
    mpfr_init (mpfIMag);
    mpfr_init (mpfTmp1);
    mpc_init2 (Z1, mpfr_get_default_prec ());
    mpc_init2 (Z2, mpfr_get_default_prec ());

    // Calculate the magnitude of the imag parts in the arg
    mpXf_mag_imag (mpfIMag, op, rnd, iHCDim, 1);                // g

    // Construct a complex number from h and g
    mpfr_set (Z1->re, &op->parts[0], rnd);
    mpfr_set (Z1->im, mpfIMag, rnd);

    //***************************************************************************
    // In order to get correct CW continuity results from mpc_atanh,
    //   we must ensure that if the imag part of the
    //   arg is zero, then its sign is opposite that of
    //   the real part of the arg
    //***************************************************************************

    // If the arg has no imag parts, ...
    if (mpfr_zero_p (mpfIMag))
    {
        // Ensure no sign on zero
        mpfr_abs (Z1->im, Z1->im, MPFR_RNDN);

        // If the real part of the arg is >= 1, ... (CW)
        if (mpfr_cmp_si (&op->parts[0], 1) >= 0)
////    // If the real part is <= -1, ... (CCW)
////    if (mpfr_cmp_si (&op->parts[0], -1) <=  0)
            // Pass negative zero to mpc_atanh
            mpfr_neg (Z1->im, Z1->im, MPFR_RNDN);
    } // End IF

    //***************************************************************************
    // If the real part of Z1 is ±1, and the imaginary part is 0,
    //   then <mpc_atanh> and <gsl_complex_arctanh>
    //   disagree on the result.
    // MPC says the result is (±inf,0)
    //   and GSL says it is   (+inf,-Pi/2)  or  (-inf, Pi/2)
    // For the moment, I'm going with GSL because it is more like
    //   "NIST Handbook of Mathematical Functions" (a.k.a. A&S v2),
    //   section 4.37.25.
    //***************************************************************************

    // If the real part of Z1 is ±1, and
    //    the imag part of Z1 is  0, ...
    if ((mpfr_cmp_si (Z1->re,  1) EQ 0
      || mpfr_cmp_si (Z1->re, -1) EQ 0)
     && mpfr_zero_p (Z1->im))
    {
        // Set the real part of Z2 to ±Inf
        mpfr_set_inf (Z2->re, mpfr_sgn (Z1->re));

        // Set the imag part of Z2 to ±Pi/2
        mpfr_set (Z2->im, &lpMemPTD->mpfrHC8V_Pi.parts[0], MPFR_RNDN);
        mpfr_div_ui (Z2->im, Z2->im, 2, MPFR_RNDN);

        // Set the sign of the imag part in Z2
        //   to the opposite sign of the real
        //   part of Z2
        if (SIGN_APLVFP (Z2->re) EQ 0)
            mpfr_neg  (Z2->im, Z2->im, MPFR_RNDN);
    } else
    {
        // If either part is an infinity or NaN, ...
        if (mpfr_inf_p (Z1->re) || mpfr_inf_p (Z1->im)
         || mpfr_nan_p (Z1->re) || mpfr_nan_p (Z1->im))
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        // Calculate the corresponding complex number
        mpc_atanh (Z2, Z1, MPC_RNDNN);
     } // End IF/ELSE

    // Save as the real part of the result
    mpfr_set (&rop->parts[0], Z2->re, rnd);

    // If the arg has any imag parts, ...
    if (!mpfr_zero_p (mpfIMag))                                         // g == 0 ?
    {
        // Calculate:  Z2->im / g
        mpfr_div (mpfTmp1, Z2->im, mpfIMag, rnd);

        // Multiply the imag parts of the arg by Z2.im / g
        mpXf_scale (rop, op, mpfTmp1, iHCDim, rnd);
    } else
    // The imag part of the arg is zero
        // Save in first imag part of the result
        mpfr_set (&rop->parts[1], Z2->im, rnd);

    // We no longer need this storage
    mpc_clear  (Z2     );
    mpc_clear  (Z1     );
    mpfr_clear (mpfTmp1);
    mpfr_clear (mpfIMag);
} // End mpXf_atanh


//***************************************************************************
//  $mpXf_scale
//
//  Multiply the imag parts of the arg by a scale factor
//***************************************************************************

void mpXf_scale
    (mpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     mpcho_t    op,             // The argument (as oneof mpcf_/mphf_/mpof_)
     mpfr_t     mpfTmp1,        // Scale factor
     int        iHCDim,         // HC Dimension (1, 2, 4, 8)
     mpfr_rnd_t rnd)            // Rounding mode

{
    int i;                      // Loop counter

    // Loop through the imag parts
    for (i = 1; i < iHCDim; i++)
    {
        // Multiply the imag parts of the arg by the scale factor
        mpfr_mul (&rop->parts[i], &op->parts[i], mpfTmp1, rnd);

        // If the result is a NaN (really a Real Indefinite), ...
        if (mpfr_nan_p (&rop->parts[i]))
            // Set to zero
            // We are deciding for this function that {inf} x 0 is 0
            mpfr_set_d (&rop->parts[i], 0.0, MPFR_RNDN);
    } // End FOR
} // End mpXf_scale


#if FALSE
//***************************************************************************
//  mpfr_albe_l
//
//  function albe_l (x, y) { var xp = x + 1; return r_sqrt (xp * xp + y * y); }
//***************************************************************************

mpfr_res mpfr_albe_l
    (mpfr_ptr x,
     mpfr_ptr y)

{
    mpfr_res   xp, yp;
    mpfr_rnd_t rnd = MPFR_RNDN;

    // Initialize to NaN
    mpfr_init (&xp);

    // Increment
    mpfr_add_si (&xp, x, 1, rnd);

    // Square
    mpfr_mul (&xp, &xp, &xp, rnd);

    // Initialize to NaN
    mpfr_init (&yp);

    // Square
    mpfr_mul (&yp, y, y, rnd);

    // Add the squares
    mpfr_add (&xp, &xp, &yp, rnd);

    // Sqrt
    mpfr_sqrt (&xp, &xp, rnd);

    // We no longer need this storage
    mpfr_clear (&yp);

    return xp;
} // End mpfr_albe_l
#endif


#if FALSE
//***************************************************************************
//  mpfr_albe_r
//
//  function albe_r (x, y) { var xp = x - 1; return r_sqrt (xp * xp + y * y); }
//***************************************************************************

mpfr_res mpfr_albe_r
    (mpfr_ptr x,
     mpfr_ptr y)

{
    mpfr_res   xp, yp;
    mpfr_rnd_t rnd = MPFR_RNDN;

    // Initialize to NaN
    mpfr_init (&xp);

    // Decrement
    mpfr_sub_si (&xp, x, 1, rnd);

    // Square
    mpfr_mul (&xp, &xp, &xp, rnd);

    // Initialize to NaN
    mpfr_init (&yp);

    // Square
    mpfr_mul (&yp, y, y, rnd);

    // Add the squares
    mpfr_add (&xp, &xp, &yp, rnd);

    // Sqrt
    mpfr_sqrt (&xp, &xp, rnd);

    // We no longer need this storage
    mpfr_clear (&yp);

    return xp;
} // End mpfr_albe_r
#endif


#if FALSE
//***************************************************************************
//  mpfr_alpha
//
//  function alpha  (x, y) { return 0.5 * (albe_l (x, y) + albe_r (x, y)); }
//***************************************************************************

mpfr_res mpfr_alpha
    (mpfr_ptr x,
     mpfr_ptr y)

{
    mpfr_res   xp, yp;
    mpfr_rnd_t rnd = MPFR_RNDN;

    // albe_l
    xp = mpfr_albe_l (x, y);
    yp = mpfr_albe_r (x, y);

    // Add
    mpfr_add (&xp, &xp, &yp, rnd);

    // Divide by 2 to get the average
    mpfr_div_si (&xp, &xp, 2, rnd);

    // We no longer need this storage
    mpfr_clear (&yp);

    return xp;
} // End mpfr_alpha
#endif


#if FALSE
//***************************************************************************
//  mpfr_beta
//
//  function beta   (x, y) { return 0.5 * (albe_l (x, y) - albe_r (x, y)); }
//***************************************************************************

mpfr_res mpfr_beta
    (mpfr_ptr x,
     mpfr_ptr y)

{
    mpfr_res   xp, yp;
    mpfr_rnd_t rnd = MPFR_RNDN;

    // albe_l
    xp = mpfr_albe_l (x, y);
    yp = mpfr_albe_r (x, y);

    // Subtract
    mpfr_sub (&xp, &xp, &yp, rnd);

    // Divide by 2 to get the average
    mpfr_div_si (&xp, &xp, 2, rnd);

    // We no longer need this storage
    mpfr_clear (&yp);

    return xp;
} // End mpfr_beta
#endif


//***************************************************************************
//  End of File: mpcho_com.c
//***************************************************************************
