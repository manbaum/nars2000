//***************************************************************************
//  NARS2000 -- Multiple Precision CHO Common Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2015 Sudley Place Software

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
//  $mpXf_sin
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
//  $mpXf_cos
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
//  $mpXf_tan
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
//  $mpXf_asin
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
#ifdef DEBUG
////WCHAR wszTemp[1024] = {'\0'};
#endif

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

#ifdef DEBUG
////strcpyW (wszTemp, L"Z.re:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], &Z->re, 0) = WC_EOS; DbgMsgW (wszTemp);
////strcpyW (wszTemp, L"Z.im:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], &Z->im, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
    // Calculate the corresponding complex number
    mpc_asin (Z, Z, MPC_RNDNN);

#ifdef DEBUG
////strcpyW (wszTemp, L"Z.re:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], &Z->re, 0) = WC_EOS; DbgMsgW (wszTemp);
////strcpyW (wszTemp, L"Z.im:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], &Z->im, 0) = WC_EOS; DbgMsgW (wszTemp);
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
//  $mpXf_acos
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
#ifdef DEBUG
////WCHAR wszTemp[1024] = {'\0'};
#endif

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

#ifdef DEBUG
////strcpyW (wszTemp, L"Z.re:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], &Z->re, 0) = WC_EOS; DbgMsgW (wszTemp);
////strcpyW (wszTemp, L"Z.im:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], &Z->im, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
    // Calculate the corresponding complex number
    mpc_acos (Z, Z, MPC_RNDNN);

#ifdef DEBUG
////strcpyW (wszTemp, L"Z.re:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], &Z->re, 0) = WC_EOS; DbgMsgW (wszTemp);
////strcpyW (wszTemp, L"Z.im:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], &Z->im, 0) = WC_EOS; DbgMsgW (wszTemp);
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
//  $mpXf_atan
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
#ifdef DEBUG
////WCHAR wszTemp[1024] = {'\0'};
#endif

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

#ifdef DEBUG
////strcpyW (wszTemp, L"Z.re:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], &Z->re, 0) = WC_EOS; DbgMsgW (wszTemp);
////strcpyW (wszTemp, L"Z.im:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], &Z->im, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
    // Calculate the corresponding complex number
    mpc_atan (Z, Z, MPC_RNDNN);

#ifdef DEBUG
////strcpyW (wszTemp, L"Z.re:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], &Z->re, 0) = WC_EOS; DbgMsgW (wszTemp);
////strcpyW (wszTemp, L"Z.im:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], &Z->im, 0) = WC_EOS; DbgMsgW (wszTemp);
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
////    // Set the real part of the result to Pi/2
////    // Calculate:  Pi
////    mpfr_const_pi (&rop->parts[0], rnd);
////
////    // Calculate:  Pi / 2
////    mpfr_div_si (&rop->parts[0], &rop->parts[0], 2, rnd);
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
//  $mpXf_sinh
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
//  $mpXf_cosh
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
//  $mpXf_tanh
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
//  $mpXf_asinh
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
#ifdef DEBUG
////WCHAR wszTemp[1024] = {L'\0'};
#endif

    // Initialize to NaN
    mpfr_init (mpfIMag);
    mpfr_init (mpfTmp1);
    mpc_init2 (Z, mpfr_get_default_prec ());

    // Calculate the magnitude of the imag parts in the arg
    mpXf_mag_imag (mpfIMag, op, rnd, iHCDim, 1);                // g

    // Construct a complex number from h and g
    mpfr_set (Z->re, &op->parts[0], rnd);
    mpfr_set (Z->im, mpfIMag, rnd);

#ifdef DEBUG
////strcpyW (wszTemp, L"Z.re:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], &Z->re, 0) = WC_EOS; DbgMsgW (wszTemp);
////strcpyW (wszTemp, L"Z.im:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], &Z->im, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
    // Calculate the corresponding complex number
    mpc_asinh (Z, Z, MPC_RNDNN);
#ifdef DEBUG
////strcpyW (wszTemp, L"Z.re:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], &Z->re, 0) = WC_EOS; DbgMsgW (wszTemp);
////strcpyW (wszTemp, L"Z.im:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], &Z->im, 0) = WC_EOS; DbgMsgW (wszTemp);
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
//  $mpXf_acosh
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
//  $Mympc_atanh
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
//  $mpXf_atanh
//***************************************************************************

void mpXf_atanh
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

    // In order to get correct CW continuity results from mpc_atanh,
    //   we must ensure that if the imag part of the
    //   arg is zero, then its sign is opposite that of
    //   the real part of the arg
    if (mpfr_zero_p (&op->parts[1]))
    {
        // Ensure no sign on zero
        mpfr_abs (Z->im, Z->im, MPFR_RNDN);

        // If the real part of the arg is >= 1, ... (CW)
        if (mpfr_cmp_si (&op->parts[0], 1) >= 0)
////    // If the real part is <= -1, ... (CCW)
////    if (mpfr_cmp_si (&op->parts[0], -1) <=  0)
            // Pass negative zero to mpc_atanh
            mpfr_neg (Z->im, Z->im, MPFR_RNDN);
    } // End IF
    // Calculate the corresponding complex number
    mpc_atanh (Z, Z, MPC_RNDNN);

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
        // Multiply the imag parts of the arg by the scale factor
        mpfr_mul (&rop->parts[i], &op->parts[i], mpfTmp1, rnd);
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
