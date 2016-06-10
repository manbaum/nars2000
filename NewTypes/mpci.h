//***************************************************************************
//  NARS2000 -- Multiple Precision Complex Integer/Rational Header
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

#ifndef __MPCI_H
#define __MPCI_H

typedef union tagMPCI
{
    __mpq_struct parts[2];
    struct tagMPCIPARTS
    {
        __mpq_struct partsLo,
                     partsHi;
    };
} __mpci_struct, mpci_str;


typedef __mpci_struct   mpci_t[1];
typedef __mpci_struct  *mpci_ptr;

// Used to return a struct as a result
typedef __mpci_struct   mpci_res;


//***************************************************************************
//  Define prototypes
//***************************************************************************

void mpci_init0     (mpci_t rop);
void mpci_clear     (mpci_t rop);
void mpci_set_sx    (mpci_t rop, __int64 num, __int64 den);


//***************************************************************************
//  Define names for the MPCI_ functions
//***************************************************************************


#endif /* __MPCI_H */


//***************************************************************************
//  End of File: mpci.h
//***************************************************************************
