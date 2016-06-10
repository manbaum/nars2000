//***************************************************************************
//  NARS2000 -- Multiple Precision Quaternion Integer/Rational
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
//  $mphi_init0
//***************************************************************************

void mphi_init0
    (mphi_t rop)

{
    mpq_init (&rop->parts[0]);
    mpq_init (&rop->parts[1]);
    mpq_init (&rop->parts[2]);
    mpq_init (&rop->parts[3]);
} // End mphi_init0


//***************************************************************************
//  $mphi_clear
//***************************************************************************

void mphi_clear
    (mphi_t rop)

{
    mpq_clear (&rop->parts[0]);
    mpq_clear (&rop->parts[1]);
    mpq_clear (&rop->parts[2]);
    mpq_clear (&rop->parts[3]);
} // End mphi_clear


//***************************************************************************
//  $mphi_set_sj
//***************************************************************************

void mphi_set_sx
    (mphi_t     rop,
     __int64    num,
     __int64    den)

{
    // Handle via macro
    mpcho_set_sx (rop, num, den, 4);
} // End mphi_set_sx


//***************************************************************************
//  End of File: mphi.c
//***************************************************************************
