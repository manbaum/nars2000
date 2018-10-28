//***************************************************************************
//  NARS2000 -- Fixed Precision Octonion Floating Point Header
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

#ifndef __FPOF_H
#define __FPOF_H

typedef union tagFPOF
{
    fprf_t parts[8];
    struct tagFPOFPARTS
    {
        fphf_t partsLo,
               partsHi;
    };
} __fpof_struct, fpof_str;


typedef __fpof_struct   fpof_t[1];
typedef __fpof_struct  *fpof_ptr;

// Used to return a struct as a result
typedef __fpof_struct   fpof_res;


//***************************************************************************
//  Define prototypes
//***************************************************************************



//***************************************************************************
//  Define names for the FPOF_ functions
//***************************************************************************

#define fpof_sin(rop,op)        fpXf_sin   (rop, op, 8)
#define fpof_cos(rop,op)        fpXf_cos   (rop, op, 8)
#define fpof_tan(rop,op)        fpXf_tan   (rop, op, 8)

#define fpof_asin(rop,op)       fpXf_asin  (rop, op, 8)
#define fpof_acos(rop,op)       fpXf_acos  (rop, op, 8)
#define fpof_atan(rop,op)       fpXf_atan  (rop, op, 8)

#define fpof_sinh(rop,op)       fpXf_sinh  (rop, op, 8)
#define fpof_cosh(rop,op)       fpXf_cosh  (rop, op, 8)
#define fpof_tanh(rop,op)       fpXf_tanh  (rop, op, 8)

#define fpof_asinh(rop,op)      fpXf_asinh (rop, op, 8)
#define fpof_acosh(rop,op)      fpXf_acosh (rop, op, 8)
#define fpof_atanh(rop,op)      fpXf_atanh (rop, op, 8)

#endif /* __FPOF_H */


//***************************************************************************
//  End of File: fpof.h
//***************************************************************************
