//***************************************************************************
//  NARS2000 -- Multiple Precision Quaternion Floating Point
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
#include <stdint.h>
#include <mpfr.h>
#include "mpcf.h"
#include "mphf.h"
#include "mpof.h"


//***************************************************************************
//  $mphf_init0
//***************************************************************************

void mphf_init0
    (mphf_t rop)

{
    mpfr_init0 (&rop->parts[0]);
    mpfr_init0 (&rop->parts[1]);
    mpfr_init0 (&rop->parts[2]);
    mpfr_init0 (&rop->parts[3]);
} // End mphf_init0


//***************************************************************************
//  $mphf_init_set
//***************************************************************************

void mphf_init_set
    (mphf_t rop,
     mphf_t op)

{
    mpfr_init_set (&rop->parts[0], &op->parts[0], MPFR_RNDN);
    mpfr_init_set (&rop->parts[1], &op->parts[1], MPFR_RNDN);
    mpfr_init_set (&rop->parts[2], &op->parts[2], MPFR_RNDN);
    mpfr_init_set (&rop->parts[3], &op->parts[3], MPFR_RNDN);
} // End mphf_init_set


//***************************************************************************
//  $mphf_clear
//***************************************************************************

void mphf_clear
    (mphf_t rop)

{
    mpfr_clear (&rop->parts[0]);
    mpfr_clear (&rop->parts[1]);
    mpfr_clear (&rop->parts[2]);
    mpfr_clear (&rop->parts[3]);
} // End mphf_clear


//***************************************************************************
//  $mphf_set_sj
//***************************************************************************

void mphf_set_sj
    (mphf_t     rop,
     __int64    op,
     mpfr_rnd_t rnd)

{
    mpfr_set_sj (&rop->parts[0], op, rnd);
    mpfr_set_sj (&rop->parts[1], 0 , rnd);
    mpfr_set_sj (&rop->parts[2], 0 , rnd);
    mpfr_set_sj (&rop->parts[3], 0 , rnd);
} // End mphf_set_sj


//***************************************************************************
//  End of File: mphf.c
//***************************************************************************
