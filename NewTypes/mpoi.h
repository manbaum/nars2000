//***************************************************************************
//  NARS2000 -- Multiple Precision Octonion Integer/Rational Header
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

#ifndef __MPOI_H
#define __MPOI_H

typedef union tagMPOI
{
    __mpq_struct parts[8];
    struct tagMPOIPARTS
    {
        __mphi_struct partsLo,
                      partsHi;
    };
} __mpoi_struct, mpoi_str;


typedef __mpoi_struct   mpoi_t[1];
typedef __mpoi_struct  *mpoi_ptr;

// Used to return a struct as a result
typedef __mpoi_struct   mpoi_res;


//***************************************************************************
//  Define prototypes
//***************************************************************************

void mpoi_init0     (mpoi_t rop);
void mpoi_clear     (mpoi_t rop);
void mpoi_set_sx    (mpoi_t rop, __int64 num, __int64 den);


//***************************************************************************
//  Define names for the MPOI_ functions
//***************************************************************************


#endif /* __MPOI_H */


//***************************************************************************
//  End of File: mpoi.h
//***************************************************************************
