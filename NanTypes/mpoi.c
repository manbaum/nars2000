//***************************************************************************
//  NARS2000 -- Multiple Precision Octonion Integer/Rational
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
#include "mpci.h"
#include "mphi.h"
#include "mpoi.h"
#include "mpcho_com.mac"


//***************************************************************************
//  $mpoi_init0
//***************************************************************************

void mpoi_init0
    (mpoi_t rop)

{
    mpq_init (&rop->parts[0]);
    mpq_init (&rop->parts[1]);
    mpq_init (&rop->parts[2]);
    mpq_init (&rop->parts[3]);
    mpq_init (&rop->parts[4]);
    mpq_init (&rop->parts[5]);
    mpq_init (&rop->parts[6]);
    mpq_init (&rop->parts[7]);
} // End mpoi_init0


//***************************************************************************
//  $mpoi_clear
//***************************************************************************

void mpoi_clear
    (mpoi_t rop)

{
    mpq_clear (&rop->parts[0]);
    mpq_clear (&rop->parts[1]);
    mpq_clear (&rop->parts[2]);
    mpq_clear (&rop->parts[3]);
    mpq_clear (&rop->parts[4]);
    mpq_clear (&rop->parts[5]);
    mpq_clear (&rop->parts[6]);
    mpq_clear (&rop->parts[7]);
} // End mpoi_clear


//***************************************************************************
//  $mpoi_set_sx
//***************************************************************************

void mpoi_set_sx
    (mpoi_t     rop,
     __int64    num,
     __int64    den)

{
    // Handle via macro
    mpcho_set_sx (rop, num, den, 8);
} // End mpoi_set_sx


//***************************************************************************
//  End of File: mpoi.c
//***************************************************************************
