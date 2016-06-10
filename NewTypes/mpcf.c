//***************************************************************************
//  NARS2000 -- Multiple Precision Complex Floating Point
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
#include <stdint.h>
#include <mpfr.h>
#include "mpcf.h"
#include "mphf.h"
#include "mpof.h"


//***************************************************************************
//  $mpcf_init0
//***************************************************************************

void mpcf_init0
    (mpcf_t rop)

{
    mpfr_init0 (&rop->parts[0]);
    mpfr_init0 (&rop->parts[1]);
} // End mpcf_init0


//***************************************************************************
//  $mpcf_clear
//***************************************************************************

void mpcf_clear
    (mpcf_t rop)

{
    mpfr_clear (&rop->parts[0]);
    mpfr_clear (&rop->parts[1]);
} // End mpcf_clear


//***************************************************************************
//  $mpcf_set_sj
//***************************************************************************

void mpcf_set_sj
    (mpcf_t     rop,
     __int64    op,
     mpfr_rnd_t rnd)

{
    mpfr_set_sj (&rop->parts[0], op, rnd);
    mpfr_set_sj (&rop->parts[1], 0 , rnd);
} // End mpcf_set_sj


//***************************************************************************
//  End of File: mpcf.c
//***************************************************************************
