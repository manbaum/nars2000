//***************************************************************************
//  NARS2000 -- Fixed Precision Complex Floating Point Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2016 Sudley Place Software

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

#ifndef __FPCF_H
#define __FPCF_H

typedef union tagFPCF
{
    fprf_t parts[2];
    struct tagFPCFPARTS
    {
        fprf_t partsLo,
               partsHi;
    };
} __fpcf_struct, fpcf_str;


typedef __fpcf_struct   fpcf_t[1];
typedef __fpcf_struct  *fpcf_ptr;

// Used to return a struct as a result
typedef __fpcf_struct   fpcf_res;


//***************************************************************************
//  Define prototypes
//***************************************************************************



//***************************************************************************
//  Define names for the FPCF_ functions
//***************************************************************************

#define fpcf_sin(rop,op)        fpXf_sin   (rop, op, 2)
#define fpcf_cos(rop,op)        fpXf_cos   (rop, op, 2)
#define fpcf_tan(rop,op)        fpXf_tan   (rop, op, 2)

#define fpcf_asin(rop,op)       fpXf_asin  (rop, op, 2)
#define fpcf_acos(rop,op)       fpXf_acos  (rop, op, 2)
#define fpcf_atan(rop,op)       fpXf_atan  (rop, op, 2)

#define fpcf_sinh(rop,op)       fpXf_sinh  (rop, op, 2)
#define fpcf_cosh(rop,op)       fpXf_cosh  (rop, op, 2)
#define fpcf_tanh(rop,op)       fpXf_tanh  (rop, op, 2)

#define fpcf_asinh(rop,op)      fpXf_asinh (rop, op, 2)
#define fpcf_acosh(rop,op)      fpXf_acosh (rop, op, 2)
#define fpcf_atanh(rop,op)      fpXf_atanh (rop, op, 2)

#endif /* __FPCF_H */


//***************************************************************************
//  End of File: fpcf.h
//***************************************************************************
