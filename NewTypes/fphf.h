//***************************************************************************
//  NARS2000 -- Fixed Precision Quaternion Floating Point Header
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

#ifndef __FPHF_H
#define __FPHF_H

typedef union tagFPHF
{
    fprf_t parts[4];
    struct tagFPHFPARTS
    {
        fpcf_t partsLo,
               partsHi;
    };
} __fphf_struct, fphf_str;


typedef __fphf_struct   fphf_t[1];
typedef __fphf_struct  *fphf_ptr;

// Used to return a struct as a result
typedef __fphf_struct   fphf_res;


//***************************************************************************
//  Define prototypes
//***************************************************************************



//***************************************************************************
//  Define names for the FPHF_ functions
//***************************************************************************

#define fphf_sin(rop,op)        fpXf_sin   (rop, op, 4)
#define fphf_cos(rop,op)        fpXf_cos   (rop, op, 4)
#define fphf_tan(rop,op)        fpXf_tan   (rop, op, 4)

#define fphf_asin(rop,op)       fpXf_asin  (rop, op, 4)
#define fphf_acos(rop,op)       fpXf_acos  (rop, op, 4)
#define fphf_atan(rop,op)       fpXf_atan  (rop, op, 4)

#define fphf_sinh(rop,op)       fpXf_sinh  (rop, op, 4)
#define fphf_cosh(rop,op)       fpXf_cosh  (rop, op, 4)
#define fphf_tanh(rop,op)       fpXf_tanh  (rop, op, 4)

#define fphf_asinh(rop,op)      fpXf_asinh (rop, op, 4)
#define fphf_acosh(rop,op)      fpXf_acosh (rop, op, 4)
#define fphf_atanh(rop,op)      fpXf_atanh (rop, op, 4)

#endif /* __FPHF_H */


//***************************************************************************
//  End of File: fphf.h
//***************************************************************************
