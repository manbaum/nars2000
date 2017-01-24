//***************************************************************************
//  NARS2000 -- Fixed Precision CHO Common Routines
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
#include <math.h>
#include "headers.h"
#include "fpcho_com.mac"
#undef  gsl_complex
#include <gsl\gsl_sys.h>
#include <gsl\gsl_complex.h>
#include <gsl\gsl_complex_math.h>


//***************************************************************************
//  $fpXf_sin
//
//  The code below was translated to C from the original Javascript
//    http://tamivox.org/redbear/qtrn_calc/index.html
//  written by Dave Barber, Copyright 2015, Dave Barber.
//***************************************************************************

void fpXf_sin
    (fpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     fpcho_t    op,             // The argument (as oneof mpcf_/mphf_/mpof_)
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    fprf_t      fpfIMag,        // Magnitude of imaginary parts in the arg
                fpfTmp1;        // Temp var
    fpcho_t     fpfAbs;         // ...
    gsl_complex Z;              // ...

    // Calculate the magnitude of the imaginary parts in the arg
////fpXf_mag_imag (fpfIMag, op        );                        // g
    fpXf_mag_imag8(fpfIMag, op, fpfAbs);                        // g

    // Construct a complex number from h and g
    fprf_set (Z.dat[0], op->parts[0]);
    fprf_set (Z.dat[1], fpfIMag);

    // Calculate the corresponding complex number
    Z = gsl_complex_sin (Z);

    // Save as the real part of the result
    fprf_set (rop->parts[0], Z.dat[0]);

    // If the arg has any imaginary parts, ...
    if (!fprf_zero_p (fpfIMag))                                 // g == 0 ?
    {
        // Calculate:  Z.dat[1] / g
        fprf_div (fpfTmp1, Z.dat[1], fpfIMag);

        // Multiply the imaginary parts of the arg by Z / g
        fpXf_scale (rop, op, fpfTmp1, iHCDim);
    } else
    // The imaginary part of the arg is zero
        // Save in first imaginary part in the result
        fprf_set (rop->parts[1], Z.dat[1]);
} // End fpXf_sin


//***************************************************************************
//  $fpXf_cos
//***************************************************************************

void fpXf_cos
    (fpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     fpcho_t    op,             // The argument (as oneof mpcf_/mphf_/mpof_)
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    fprf_t      fpfIMag,        // Magnitude of imaginary parts in the arg
                fpfTmp1;        // Temp var
    fpcho_t     fpfAbs;         // ...
    gsl_complex Z;              // ...

    // Calculate the magnitude of the imaginary parts in the arg
////fpXf_mag_imag (fpfIMag, op        );                        // g
    fpXf_mag_imag8(fpfIMag, op, fpfAbs);                        // g

    // Construct a complex number from h and g
    fprf_set (Z.dat[0], op->parts[0]);
    fprf_set (Z.dat[1], fpfIMag);

    // Calculate the corresponding complex number
    Z = gsl_complex_cos (Z);

    // Save as the real part of the result
    fprf_set (rop->parts[0], Z.dat[0]);

    // If the arg has any imaginary parts, ...
    if (!fprf_zero_p (fpfIMag))                                 // g == 0 ?
    {
        // Calculate:  Z.dat[1] / g
        fprf_div (fpfTmp1, Z.dat[1], fpfIMag);

        // Multiply the imaginary parts of the arg by Z / g
        fpXf_scale (rop, op, fpfTmp1, iHCDim);
    } else
    // The imaginary part of the arg is zero
        // Save in first imaginary part in the result
        fprf_set (rop->parts[1], Z.dat[1]);
} // End fpXf_cos


//***************************************************************************
//  $fpXf_tan
//***************************************************************************

void fpXf_tan
    (fpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     fpcho_t    op,             // The argument (as oneof mpcf_/mphf_/mpof_)
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    fprf_t      fpfIMag,        // Magnitude of imaginary parts in the arg
                fpfTmp1;        // Temp var
    fpcho_t     fpfAbs;         // ...
    gsl_complex Z;              // ...

    // Calculate the magnitude of the imaginary parts in the arg
////fpXf_mag_imag (fpfIMag, op        );                        // g
    fpXf_mag_imag8(fpfIMag, op, fpfAbs);                        // g

    // Construct a complex number from h and g
    fprf_set (Z.dat[0], op->parts[0]);
    fprf_set (Z.dat[1], fpfIMag);

    // Calculate the corresponding complex number
    Z = gsl_complex_tan (Z);

    // Save as the real part of the result
    fprf_set (rop->parts[0], Z.dat[0]);

    // If the arg has any imaginary parts, ...
    if (!fprf_zero_p (fpfIMag))                                 // g == 0 ?
    {
        // Calculate:  Z.dat[1] / g
        fprf_div (fpfTmp1, Z.dat[1], fpfIMag);

        // Multiply the imaginary parts of the arg by Z / g
        fpXf_scale (rop, op, fpfTmp1, iHCDim);
    } else
    // The imaginary part of the arg is zero
        // Save in first imaginary part in the result
        fprf_set (rop->parts[1], Z.dat[1]);
} // End fpXf_tan


//***************************************************************************
//  $fpXf_asin
//***************************************************************************

void fpXf_asin
    (fpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     fpcho_t    op,             // The argument (as oneof mpcf_/mphf_/mpof_)
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    fprf_t      fpfIMag,        // Magnitude of imaginary parts in the arg
                fpfTmp1;        // Temp var
    fpcho_t     fpfAbs;         // ...
    gsl_complex Z;              // ...

    // Calculate the magnitude of the imaginary parts in the arg
////fpXf_mag_imag (fpfIMag, op        );                        // g
    fpXf_mag_imag8(fpfIMag, op, fpfAbs);                        // g

    // Construct a complex number from h and g
    fprf_set (Z.dat[0], op->parts[0]);
    fprf_set (Z.dat[1], fpfIMag);

    // Calculate the corresponding complex number
    Z = gsl_complex_arcsin (Z);

    // Save as the real part of the result
    fprf_set (rop->parts[0], Z.dat[0]);

    // If the arg has any imaginary parts, ...
    if (!fprf_zero_p (fpfIMag))                                 // g == 0 ?
    {
        // Calculate:  Z.dat[1] / g
        fprf_div (fpfTmp1, Z.dat[1], fpfIMag);

        // Multiply the imaginary parts of the arg by Z / g
        fpXf_scale (rop, op, fpfTmp1, iHCDim);
    } else
    // The imaginary part of the arg is zero
        // Save in first imaginary part in the result
        fprf_set (rop->parts[1], Z.dat[1]);
} // End fpXf_asin


//***************************************************************************
//  $fpXf_acos
//***************************************************************************

void fpXf_acos
    (fpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     fpcho_t    op,             // The argument (as oneof mpcf_/mphf_/mpof_)
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    fprf_t      fpfIMag,        // Magnitude of imaginary parts in the arg
                fpfTmp1;        // Temp var
    fpcho_t     fpfAbs;         // ...
    gsl_complex Z;              // ...

    // Calculate the magnitude of the imaginary parts in the arg
////fpXf_mag_imag (fpfIMag, op        );                        // g
    fpXf_mag_imag8(fpfIMag, op, fpfAbs);                        // g

    // Construct a complex number from h and g
    fprf_set (Z.dat[0], op->parts[0]);
    fprf_set (Z.dat[1], fpfIMag);

    // Calculate the corresponding complex number
    Z = gsl_complex_arccos (Z);

    // Save as the real part of the result
    fprf_set (rop->parts[0], Z.dat[0]);

    // If the arg has any imaginary parts, ...
    if (!fprf_zero_p (fpfIMag))                                 // g == 0 ?
    {
        // Calculate:  Z.dat[1] / g
        fprf_div (fpfTmp1, Z.dat[1], fpfIMag);

        // Multiply the imaginary parts of the arg by Z / g
        fpXf_scale (rop, op, fpfTmp1, iHCDim);
    } else
        // Save in first imaginary part in the result
        fprf_set (rop->parts[1], Z.dat[1]);
} // End fpXf_acos


//***************************************************************************
//  $fpXf_atan
//***************************************************************************

void fpXf_atan
    (fpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     fpcho_t    op,             // The argument (as oneof mpcf_/mphf_/mpof_)
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    fprf_t      fpfIMag,        // Magnitude of imaginary parts in the arg
                fpfTmp1;        // Temp var
    fpcho_t     fpfAbs;         // ...
    gsl_complex Z;              // ...

    // Calculate the magnitude of the imaginary parts in the arg
////fpXf_mag_imag (fpfIMag, op        );                        // g
    fpXf_mag_imag8(fpfIMag, op, fpfAbs);                        // g

    // Construct a complex number from h and g
    fprf_set (Z.dat[0], op->parts[0]);
    fprf_set (Z.dat[1], fpfIMag);

    // Calculate the corresponding complex number
    Z = gsl_complex_arctan (Z);

    // Save as the real part of the result
    fprf_set (rop->parts[0], Z.dat[0]);

    // If the arg has any imaginary parts, ...
    if (!fprf_zero_p (fpfIMag))                                 // g == 0 ?
    {
        // Calculate:  Z.dat[1] / g
        fprf_div (fpfTmp1, Z.dat[1], fpfIMag);

        // Multiply the imaginary parts of the arg by of Z / g
        fpXf_scale (rop, op, fpfTmp1, iHCDim);

        // Because GSL implements CCW continuity for arctan,
        //   we need to change that to CW continuity (as per
        //   PP's tables, which, as per Kahan, I think are a mistake)

        // If the real part of the arg is zero, ...
        if (fprf_zero_p (op->parts[0]))
        {
            // If the (first) imaginary part of the arg is 0, ...
            if (fprf_zero_p (op->parts[1]))
                // Set the real part of the result to zero
                fprf_set_zero (rop->parts[0]);
            else
            {
                // Set the real part of the result to ±Pi/2
                // Calculate:  Pi
                fprf_const_pi (rop->parts[0]);

                // Calculate:  Pi/2
                fprf_div_si (rop->parts[0], rop->parts[0], 2);

                // Set the sign of the real part in the result
                //   to the opposite sign of the (first) imaginary
                //   part of the result
                if (fprf_sgn (rop->parts[1]) EQ 1)
                    fprf_neg (rop->parts[0], rop->parts[0]);
            } // End IF/ELSE
        } // End IF
    } else
    // The imaginary part of the arg is zero
        // Save in first imaginary part in the result
        fprf_set (rop->parts[1], Z.dat[1]);
} // End fpXf_atan


//***************************************************************************
//  $fpXf_sinh
//***************************************************************************

void fpXf_sinh
    (fpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     fpcho_t    op,             // The argument (as oneof mpcf_/mphf_/mpof_)
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    fprf_t      fpfIMag,        // Magnitude of imaginary parts in the arg
                fpfTmp1;        // Temp var
    fpcho_t     fpfAbs;         // ...
    gsl_complex Z;              // ...

    // Calculate the magnitude of the imaginary parts in the arg
////fpXf_mag_imag (fpfIMag, op        );                        // g
    fpXf_mag_imag8(fpfIMag, op, fpfAbs);                        // g

    // Construct a complex number from h and g
    fprf_set (Z.dat[0], op->parts[0]);
    fprf_set (Z.dat[1], fpfIMag);

    // Calculate the corresponding complex number
    Z = gsl_complex_sinh (Z);

    // Save as the real part of the result
    fprf_set (rop->parts[0], Z.dat[0]);

    // If the arg has any imaginary parts, ...
    if (!fprf_zero_p (fpfIMag))                                 // g == 0 ?
    {
        // Calculate:  Z.dat[1] / g
        fprf_div (fpfTmp1, Z.dat[1], fpfIMag);

        // Multiply the imaginary parts of the arg by Z / g
        fpXf_scale (rop, op, fpfTmp1, iHCDim);
    } else
    // The imaginary part of the arg is zero
        // Save in first imaginary part in the result
        fprf_set (rop->parts[1], Z.dat[1]);
} // End fpXf_sinh


//***************************************************************************
//  $fpXf_cosh
//***************************************************************************

void fpXf_cosh
    (fpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     fpcho_t    op,             // The argument (as oneof mpcf_/mphf_/mpof_)
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    fprf_t      fpfIMag,        // Magnitude of imaginary parts in the arg
                fpfTmp1;        // Temp var
    fpcho_t     fpfAbs;         // ...
    gsl_complex Z;              // ...

    // Calculate the magnitude of the imaginary parts in the arg
////fpXf_mag_imag (fpfIMag, op        );                        // g
    fpXf_mag_imag8(fpfIMag, op, fpfAbs);                        // g

    // Construct a complex number from h and g
    fprf_set (Z.dat[0], op->parts[0]);
    fprf_set (Z.dat[1], fpfIMag);

    // Calculate the corresponding complex number
    Z = gsl_complex_cosh (Z);

    // Save as the real part of the result
    fprf_set (rop->parts[0], Z.dat[0]);

    // If the arg has any imaginary parts, ...
    if (!fprf_zero_p (fpfIMag))                                 // g == 0 ?
    {
        // Calculate:  Z.dat[1] / g
        fprf_div (fpfTmp1, Z.dat[1], fpfIMag);

        // Multiply the imaginary parts of the arg by Z / g
        fpXf_scale (rop, op, fpfTmp1, iHCDim);
    } else
    // The imaginary part of the arg is zero
        // Save in first imaginary part in the result
        fprf_set (rop->parts[1], Z.dat[1]);
} // End fpXf_cosh


//***************************************************************************
//  $fpXf_tanh
//***************************************************************************

void fpXf_tanh
    (fpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     fpcho_t    op,             // The argument (as oneof mpcf_/mphf_/mpof_)
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    fprf_t      fpfIMag,        // Magnitude of imaginary parts in the arg
                fpfTmp1;        // Temp var
    fpcho_t     fpfAbs;         // ...
    gsl_complex Z;              // ...

    // Calculate the magnitude of the imaginary parts in the arg
////fpXf_mag_imag (fpfIMag, op        );                        // g
    fpXf_mag_imag8(fpfIMag, op, fpfAbs);                        // g

    // Construct a complex number from h and g
    fprf_set (Z.dat[0], op->parts[0]);
    fprf_set (Z.dat[1], fpfIMag);

    // Calculate the corresponding complex number
    Z = gsl_complex_tanh (Z);

    // Save as the real part of the result
    fprf_set (rop->parts[0], Z.dat[0]);

    // If the arg has any imaginary parts, ...
    if (!fprf_zero_p (fpfIMag))                                 // g == 0 ?
    {
        // Calculate:  Z.dat[1] / g
        fprf_div (fpfTmp1, Z.dat[1], fpfIMag);

        // Multiply the imaginary parts of the arg by Z / g
        fpXf_scale (rop, op, fpfTmp1, iHCDim);
    } else
    // The imaginary part of the arg is zero
        // Save in first imaginary part in the result
        fprf_set (rop->parts[1], Z.dat[1]);
} // End fpXf_tanh


//***************************************************************************
//  $fpXf_asinh
//***************************************************************************

void fpXf_asinh
    (fpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     fpcho_t    op,             // The argument (as oneof mpcf_/mphf_/mpof_)
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    fprf_t      fpfIMag,        // Magnitude of imaginary parts in the arg
                fpfTmp1;        // Temp var
    fpcho_t     fpfAbs;         // ...
    gsl_complex Z;              // ...

    // Calculate the magnitude of the imaginary parts in the arg
////fpXf_mag_imag (fpfIMag, op        );                        // g
    fpXf_mag_imag8(fpfIMag, op, fpfAbs);                        // g

    // Construct a complex number from h and g
    fprf_set (Z.dat[0], op->parts[0]);
    fprf_set (Z.dat[1], fpfIMag);

    // Calculate the corresponding complex number
    Z = gsl_complex_arcsinh (Z);

    // Save as the real part of the result
    fprf_set (rop->parts[0], Z.dat[0]);

    // Because we may lose the sign of the imaginary part(s) of the arg
    //   when we compute fpfIMag, we need to check it now
    // This is a problem for both gsl_complex_arcsinh and mpc_asinh

    // If the real part of the arg is zero, ...
    if (fprf_zero_p (op->parts[0]))
    {
        // The sign of the real part of the result is the same sign
        //   as the imaginary part of the arg
        if ((fprf_sgn ( op->parts[1]) EQ -1
          && fprf_sgn (rop->parts[0]) EQ  1)
         || (fprf_sgn ( op->parts[1]) EQ  1
          && fprf_sgn (rop->parts[0]) EQ -1))
            fprf_neg (rop->parts[0], rop->parts[0]);
    } // End IF

    // If the arg has any imaginary parts, ...
    if (!fprf_zero_p (fpfIMag))                                 // g == 0 ?
    {
        // Calculate:  Z.dat[1] / g
        fprf_div (fpfTmp1, Z.dat[1], fpfIMag);

        // Multiply the imaginary parts of the arg by Z / g
        fpXf_scale (rop, op, fpfTmp1, iHCDim);
    } else
    // The imaginary part of the arg is zero
        // Save in first imaginary part in the result
        fprf_set (rop->parts[1], Z.dat[1]);
} // End fpXf_asinh


//***************************************************************************
//  $fpXf_acosh
//***************************************************************************

void fpXf_acosh
    (fpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     fpcho_t    op,             // The argument (as oneof mpcf_/mphf_/mpof_)
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    fprf_t      fpfIMag,        // Magnitude of imaginary parts in the arg
                fpfTmp1;        // Temp var
    fpcho_t     fpfAbs;         // ...
    gsl_complex Z;              // ...

    // Calculate the magnitude of the imaginary parts in the arg
////fpXf_mag_imag (fpfIMag, op        );                        // g
    fpXf_mag_imag8(fpfIMag, op, fpfAbs);                        // g

    // Construct a complex number from h and g
    fprf_set (Z.dat[0], op->parts[0]);
    fprf_set (Z.dat[1], fpfIMag);

    // Calculate the corresponding complex number
    Z = gsl_complex_arccosh (Z);

    // Save as the real part of the result
    fprf_set (rop->parts[0], Z.dat[0]);

    // If the arg has any imaginary parts, ...
    if (!fprf_zero_p (fpfIMag))                                 // g == 0 ?
    {
        // Calculate:  Z.dat[1] / g
        fprf_div (fpfTmp1, Z.dat[1], fpfIMag);

        // Multiply the imaginary parts of the arg by Z / g
        fpXf_scale (rop, op, fpfTmp1, iHCDim);
    } else
    // The imaginary part of the arg is zero
        // Save in first imaginary part of the result
        fprf_set (rop->parts[1], Z.dat[1]);
} // End fpXf_acosh


//***************************************************************************
//  $Mygsl_atanh
//***************************************************************************

void Mygsl_atanh
    (fpcf_res *rop,
     fpcf_res  op)

{
    gsl_complex Zp1, Zm1;

    // Use NIST (A&S v2) of 0.5 * log ((1+z) / (1-z))
    Zp1 = gsl_complex_add_real (*(gsl_complex *) &op, 1.0);
    Zm1 = gsl_complex_sub_real (*(gsl_complex *) &op, 1.0);
    Zm1 = gsl_complex_negative (Zm1);

    *(gsl_complex *) rop = gsl_complex_div      (Zp1, Zm1);
    *(gsl_complex *) rop = gsl_complex_log      (*(gsl_complex *) rop);
    *(gsl_complex *) rop = gsl_complex_div_real (*(gsl_complex *) rop, 2);
} // End Mygsl_atanh


//***************************************************************************
//  $fpXf_atanh
//***************************************************************************

void fpXf_atanh
    (fpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     fpcho_t    op,             // The argument (as oneof mpcf_/mphf_/mpof_)
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    fprf_t      fpfIMag,        // Magnitude of imaginary parts in the arg
                fpfTmp1;        // Temp var
    fpcho_t     fpfAbs;         // ...
    gsl_complex Z, Z1;          // ...
    fpcf_res    Z2;             // ...

    // Calculate the magnitude of the imaginary parts in the arg
////fpXf_mag_imag (fpfIMag, op        );                        // g
    fpXf_mag_imag8(fpfIMag, op, fpfAbs);                        // g

    // Construct a complex number from h and g
    fprf_set (Z.dat[0], op->parts[0]);
    fprf_set (Z.dat[1], fpfIMag);

    // Because the GSL version of atanh isn't very accurate in the imaginary part,
    //   we use a different algorithm to calculate that, using the sign of the
    //   GSL imaginary (just not its value).
    Z1 = gsl_complex_arctanh (Z);
    Mygsl_atanh (&Z2, *(fpcf_res *) &Z);

    // Because my algorithm calls GSL code which doesn't handle infinities well,
    //   if the result is a Real Indefinite in either coordinate, we go with
    //   the original <gsl_complex_arctanh> result
    if (!_isnan (Z2.parts[0]) && !_isnan (Z2.parts[1]))
        // Use my result
        Z1.dat[1] = signumflt (Z1.dat[1]) * fabs (Z2.parts[1]);

    // Save as the real part of the result
    fprf_set (rop->parts[0], Z1.dat[0]);

    // If the arg has any imaginary parts, ...
    if (!fprf_zero_p (fpfIMag))                                 // g == 0 ?
    {
        // Calculate:  Z1.dat[1] / g
        fprf_div (fpfTmp1, Z1.dat[1], fpfIMag);

        // Multiply the imaginary parts of the arg by Z1.dat[1] / g
        fpXf_scale (rop, op, fpfTmp1, iHCDim);
    } else
    // The imaginary part of the arg is zero
        // Save in first imaginary part of the result
        fprf_set (rop->parts[1], Z1.dat[1]);
} // End fpXf_atanh


//***************************************************************************
//  $fpXf_scale
//
//  Multiply the imaginary parts of the arg by a scale factor
//***************************************************************************

void fpXf_scale
    (fpcho_t    rop,            // The result (as one of mpcf_/mphf_/mpof_)
     fpcho_t    op,             // The argument (as oneof mpcf_/mphf_/mpof_)
     fprf_t     fpfTmp1,        // Scale factor
     int        iHCDim)         // HC Dimension (1, 2, 4, 8)

{
    int i;                      // Loop counter

    // Loop through the imaginary parts
    for (i = 1; i < iHCDim; i++)
    {
        // Multiply the imaginary parts of the arg by the scale factor
        fprf_mul (rop->parts[i], op->parts[i], fpfTmp1);

        // If the result is a NaN (really a Real Indefinite), ...
        if (_isnan (rop->parts[i]))
            // Set to zero
            // We are deciding for this function that {inf} x 0 is 0
            rop->parts[i] = 0.0;
    } // End FOR
} // End fpXf_scale


#ifdef USE_HYPOT
//***************************************************************************
//  fpcf_hypot
//
//  Calculate sqrt (xSign * x * x + ySign * y * y)
//    for complex fixed-precision floating point numbers
//***************************************************************************

APLHC2F fpcf_hypot
    (int     xSign,
     APLHC2F x,
     int     ySign,
     APLHC2F y)

{
    fpXf_hypot (xSign, x, ySign, y, 2, .partsLo[0].partsLo[0])

    // If the real part of the result is zero, ...
    if (res.parts[0] EQ 0)
        // Ensure that the sign of the imag part of the result is non-negative
        res.parts[1] = fabs (res.parts[1]);

    return res;
////APLFLOAT xabs = MagHC2F (x),
////         yabs = MagHC2F (y);
////APLHC2F  min, max, u, v;
////
////Assert (abs (xSign) EQ 1);
////Assert (abs (ySign) EQ 1);
////
////if (xabs < yabs)
////{
////    int tSign;
////
////    if (xabs EQ 0)
////        return y;
////
////    min = x;
////    max = y;
////
////    tSign = xSign;
////    xSign = ySign;
////    ySign = tSign;
////} else
////{
////    if (yabs EQ 0)
////        return x;
////
////    min = y;
////    max = x;
////} /* End IF/ELSE                                                      */
////
/////* Calculate the quotient                                             */
////u = DivHC2F_RE (min , max);
////
/////* Calculate the square                                               */
////u = MulHC2F_RE (u, u);
////
/////* Include the sign for y                                             */
////if (ySign EQ -1)
////    u = NegHC2F_RE (u);
////
/////* Get the constant 1                                                 */
////v = confpof_1.partsLo[0].partsLo[0];
////
/////* Include the sign for x                                             */
////if (xSign EQ -1)
////    v = NegHC2F_RE (v);
////
/////* Plus or minus one                                                  */
////u = AddHC2F_RE (u, v);
////
////return MulHC2F_RE (max,  SqrtHCxF_RE (*(LPAPLHC8F) &u, 2).partsLo[0].partsLo[0]);
} // End fpcf_hypot
#endif


#ifdef USE_HYPOT
//***************************************************************************
//  fphf_hypot
//
//  Calculate sqrt (xSign * x * x + ySign * y * y)
//    for quaternion fixed-precision floating point numbers
//***************************************************************************

APLHC4F fphf_hypot
    (int     xSign,
     APLHC4F x,
     int     ySign,
     APLHC4F y)

{
    fpXf_hypot(xSign, x, ySign, y, 4, .partsLo[0]            )

    return res;
} // End fphf_hypot
#endif


#ifdef USE_HYPOT
//***************************************************************************
//  fpof_hypot
//
//  Calculate sqrt (xSign * x * x + ySign * y * y)
//    for octonion fixed-precision floating point numbers
//***************************************************************************

APLHC8F fpof_hypot
    (int     xSign,
     APLHC8F x,
     int     ySign,
     APLHC8F y)

{
    fpXf_hypot(xSign, x, ySign, y, 8,                        )

    return res;
} // End fpof_hypot
#endif


//***************************************************************************
//  End of File: fpcho_com.c
//***************************************************************************
