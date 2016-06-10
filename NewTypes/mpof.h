//***************************************************************************
//  NARS2000 -- Multiple Precision Octonion Floating Point Header
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

#include <mpcho.h>

#ifndef __MPOF_H
#define __MPOF_H

typedef union tagMPOF
{
    __mpfr_struct parts[8];
    struct tagMPOFPARTS
    {
        __mphf_struct partsLo,
                      partsHi;
    };
} __mpof_struct, mpof_str;


typedef __mpof_struct   mpof_t[1];
typedef __mpof_struct  *mpof_ptr;

// Used to return a struct as a result
typedef __mpof_struct   mpof_res;

#ifndef mpfr_res
typedef __mpfr_struct   mpfr_res;
#endif

#ifndef mpfr_init0
#define mpfr_init0(a)   mpfr_init_set_ui ((a), 0, MPFR_RNDN)
#endif


//***************************************************************************
//  Define prototypes
//***************************************************************************

void mpof_init0     (mpof_t rop);
void mpof_clear     (mpof_t rop);
void mpof_set_sj    (mpof_t rop, __int64 op,  mpfr_rnd_t rnd);


//***************************************************************************
//  Define names for the MPOF_ functions
//***************************************************************************

#define mpof_sin(rop,op,rnd)    mpXf_sin   (rop, op, rnd, 8)
#define mpof_cos(rop,op,rnd)    mpXf_cos   (rop, op, rnd, 8)
#define mpof_tan(rop,op,rnd)    mpXf_tan   (rop, op, rnd, 8)

#define mpof_asin(rop,op,rnd)   mpXf_asin  (rop, op, rnd, 8)
#define mpof_acos(rop,op,rnd)   mpXf_acos  (rop, op, rnd, 8)
#define mpof_atan(rop,op,rnd)   mpXf_atan  (rop, op, rnd, 8)

#define mpof_sinh(rop,op,rnd)   mpXf_sinh  (rop, op, rnd, 8)
#define mpof_cosh(rop,op,rnd)   mpXf_cosh  (rop, op, rnd, 8)
#define mpof_tanh(rop,op,rnd)   mpXf_tanh  (rop, op, rnd, 8)

#define mpof_asinh(rop,op,rnd)  mpXf_asinh (rop, op, rnd, 8)
#define mpof_acosh(rop,op,rnd)  mpXf_acosh (rop, op, rnd, 8)
#define mpof_atanh(rop,op,rnd)  mpXf_atanh (rop, op, rnd, 8)

#endif /* __MPOF_H */


//***************************************************************************
//  End of File: mpof.h
//***************************************************************************
