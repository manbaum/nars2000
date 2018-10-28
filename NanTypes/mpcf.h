//***************************************************************************
//  NARS2000 -- Multiple Precision Complex Floating Point Header
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

#ifndef __MPCF_H
#define __MPCF_H

typedef union tagMPCF
{
    __mpfr_struct parts[2];
    struct tagMPCFPARTS
    {
        __mpfr_struct partsLo,
                      partsHi;
    };
} __mpcf_struct, mpcf_str;


typedef __mpcf_struct   mpcf_t[1];
typedef __mpcf_struct  *mpcf_ptr;

// Used to return a struct as a result
typedef __mpcf_struct   mpcf_res;

#ifndef mpfr_res
typedef __mpfr_struct   mpfr_res;
#endif

#ifndef mpfr_init0
#define mpfr_init0(a)   mpfr_init_set_ui ((a), 0, MPFR_RNDN)
#endif


//***************************************************************************
//  Define prototypes
//***************************************************************************

void mpcf_init0     (mpcf_t rop);
void mpcf_clear     (mpcf_t rop);
void mpcf_set_sj    (mpcf_t rop, __int64 op,  mpfr_rnd_t rnd);


//***************************************************************************
//  Define names for the MPCF_ functions
//***************************************************************************

#define mpcf_sin(rop,op,rnd)    mpXf_sin   (rop, op, rnd, 2)
#define mpcf_cos(rop,op,rnd)    mpXf_cos   (rop, op, rnd, 2)
#define mpcf_tan(rop,op,rnd)    mpXf_tan   (rop, op, rnd, 2)

#define mpcf_asin(rop,op,rnd)   mpXf_asin  (rop, op, rnd, 2)
#define mpcf_acos(rop,op,rnd)   mpXf_acos  (rop, op, rnd, 2)
#define mpcf_atan(rop,op,rnd)   mpXf_atan  (rop, op, rnd, 2)

#define mpcf_sinh(rop,op,rnd)   mpXf_sinh  (rop, op, rnd, 2)
#define mpcf_cosh(rop,op,rnd)   mpXf_cosh  (rop, op, rnd, 2)
#define mpcf_tanh(rop,op,rnd)   mpXf_tanh  (rop, op, rnd, 2)

#define mpcf_asinh(rop,op,rnd)  mpXf_asinh (rop, op, rnd, 2)
#define mpcf_acosh(rop,op,rnd)  mpXf_acosh (rop, op, rnd, 2)
#define mpcf_atanh(rop,op,rnd)  mpXf_atanh (rop, op, rnd, 2)

#endif /* __MPCF_H */


//***************************************************************************
//  End of File: mpcf.h
//***************************************************************************
