//***************************************************************************
//  NARS2000 -- Multiple Precision Quaternion Integer/Rational Header
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

#ifndef __MPHI_H
#define __MPHI_H

typedef union tagMPHI
{
    __mpq_struct parts[4];
    struct tagMPHIPARTS
    {
        __mpci_struct partsLo,
                      partsHi;
    };
} __mphi_struct, mphi_str;


typedef __mphi_struct   mphi_t[1];
typedef __mphi_struct  *mphi_ptr;

// Used to return a struct as a result
typedef __mphi_struct   mphi_res;


//***************************************************************************
//  Define prototypes
//***************************************************************************

void mphi_init0     (mphi_t rop);
void mphi_clear     (mphi_t rop);
void mphi_set_sx    (mphi_t rop, __int64 num, __int64 den);


//***************************************************************************
//  Define names for the MPHI_ functions
//***************************************************************************


#endif /* __MPHI_H */


//***************************************************************************
//  End of File: mphi.h
//***************************************************************************
