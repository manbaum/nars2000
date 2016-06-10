//***************************************************************************
//  NARS2000 -- Multiple Precision Quaternion Floating Point Header
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

#ifndef __MPHF_H
#define __MPHF_H

typedef union tagMPHF
{
    __mpfr_struct parts[4];
    struct tagMPHFPARTS
    {
        __mpcf_struct partsLo,
                      partsHi;
    };
} __mphf_struct, mphf_str;


typedef __mphf_struct   mphf_t[1];
typedef __mphf_struct  *mphf_ptr;

// Used to return a struct as a result
typedef __mphf_struct   mphf_res;

#ifndef mpfr_res
typedef __mpfr_struct   mpfr_res;
#endif

#ifndef mpfr_init0
#define mpfr_init0(a)   mpfr_init_set_ui ((a), 0, MPFR_RNDN)
#endif


//***************************************************************************
//  Define prototypes
//***************************************************************************

void mphf_init0     (mphf_t rop);
void mphf_clear     (mphf_t rop);
void mphf_set_sj    (mphf_t rop, __int64 op,  mpfr_rnd_t rnd);


//***************************************************************************
//  Define names for the MPHF_ functions
//***************************************************************************

#define mphf_sin(rop,op,rnd)    mpXf_sin   (rop, op, rnd, 4)
#define mphf_cos(rop,op,rnd)    mpXf_cos   (rop, op, rnd, 4)
#define mphf_tan(rop,op,rnd)    mpXf_tan   (rop, op, rnd, 4)

#define mphf_asin(rop,op,rnd)   mpXf_asin  (rop, op, rnd, 4)
#define mphf_acos(rop,op,rnd)   mpXf_acos  (rop, op, rnd, 4)
#define mphf_atan(rop,op,rnd)   mpXf_atan  (rop, op, rnd, 4)

#define mphf_sinh(rop,op,rnd)   mpXf_sinh  (rop, op, rnd, 4)
#define mphf_cosh(rop,op,rnd)   mpXf_cosh  (rop, op, rnd, 4)
#define mphf_tanh(rop,op,rnd)   mpXf_tanh  (rop, op, rnd, 4)

#define mphf_asinh(rop,op,rnd)  mpXf_asinh (rop, op, rnd, 4)
#define mphf_acosh(rop,op,rnd)  mpXf_acosh (rop, op, rnd, 4)
#define mphf_atanh(rop,op,rnd)  mpXf_atanh (rop, op, rnd, 4)

#endif /* __MPHF_H */


//***************************************************************************
//  End of File: mphf.h
//***************************************************************************
