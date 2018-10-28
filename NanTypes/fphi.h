//***************************************************************************
//  NARS2000 -- Fixed Precision Quaternion Integer Header
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

#ifndef __FPHI_H
#define __FPHI_H

typedef union tagFPHI
{
    fpri_t parts[4];
    struct tagFPHIPARTS
    {
        fpci_t partsLo,
               partsHi;
    };
} __fphi_struct, fphi_str;


typedef __fphi_struct   fphi_t[1];
typedef __fphi_struct  *fphi_ptr;

// Used to return a struct as a result
typedef __fphi_struct   fphi_res;


//***************************************************************************
//  Define prototypes
//***************************************************************************



//***************************************************************************
//  Define names for the FPHI_ functions
//***************************************************************************



#endif /* __FPHI_H */


//***************************************************************************
//  End of File: fphi.h
//***************************************************************************
