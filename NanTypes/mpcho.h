//***************************************************************************
//  NARS2000 -- Multiple Precision CHO Common Header
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

#ifndef __MPCHO_H
#define __MPCHO_H


// The following name is needed to be able to index any of its parts
//   regardless of whether it's C, H, or O.  As such it could be
//   defined equally as well as any one of <mpcf_t>, <mphf_t>, or <mpof_t>
// Same goes for <fpcho_t>.
#define mpcho_t     mpof_t
#define fpcho_t     fpof_t
#define fpri_t      __int64
#define fprf_t      double
#define fprf_ptr    double
#define fprf_res    double

#define fprf_sin(a,b)       a = sin (b)
#define fprf_cos(a,b)       a = cos (b)
#define fprf_tan(a,b)       a = tan (b)

#define fprf_asin(a,b)      a = asin (b)
#define fprf_acos(a,b)      a = acos (b)
#define fprf_atan(a,b)      a = atan (b)
#define fprf_atan2(a,b,c)   a = atan2 (b, c)

#define fprf_sinh(a,b)      a = sinh (b)
#define fprf_cosh(a,b)      a = cosh (b)
#define fprf_tanh(a,b)      a = tanh (b)

#define fprf_asinh(a,b)     a = asinh (b)
#define fprf_acosh(a,b)     a = acosh (b)
#define fprf_atanh(a,b)     a = atanh (b)

#define fprf_abs(a,b)       a = fabs (b)
#define fprf_add(a,b,c)     a = b + c
#define fprf_add_si(a,b,c)  a = b + c
#define fprf_si_add(a,b,c)  a = b + c
#define fprf_sub(a,b,c)     a = b - c
#define fprf_sub_si(a,b,c)  a = b - c
#define fprf_si_sub(a,b,c)  a = b - c
#define fprf_mul(a,b,c)     a = b * c
#define fprf_div(a,b,c)     a = b / c
#define fprf_div_si(a,b,c)  a = b / c
#define fprf_log(a,b)       a = log  (b)
#define fprf_sgn(a)         signumflt (a)
#define fprf_sqrt(a,b)      a= sqrt (b)
#define fprf_set(a,b)       a = b
#define fprf_neg(a,b)       a = -b
#define fprf_zero_p(a)      ((a) EQ 0)
#define fprf_set_zero(a)    (a) =  0
#define fprf_const_pi(a)    (a) =  FloatPi
#define fprf_cmp_d(a,b)     signumflt ((a) - (b))
#define fprf_cmp_si(a,b)    signumflt ((a) - (b))

#endif /* __MPCHO_H */


//***************************************************************************
//  End of File: mpcho.h
//***************************************************************************
