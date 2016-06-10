//***************************************************************************
//  NARS2000 -- Fixed Precision Octonion Integer Header
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

#ifndef __FPOI_H
#define __FPOI_H

typedef union tagFPOI
{
    fpri_t parts[8];
    struct tagFPOIPARTS
    {
        fphi_t partsLo,
               partsHi;
    };
} __fpoi_struct, fpoi_str;


typedef __fpoi_struct   fpoi_t[1];
typedef __fpoi_struct  *fpoi_ptr;

// Used to return a struct as a result
typedef __fpoi_struct   fpoi_res;


//***************************************************************************
//  Define prototypes
//***************************************************************************



//***************************************************************************
//  Define names for the FPOI_ functions
//***************************************************************************



#endif /* __FPOI_H */


//***************************************************************************
//  End of File: fpoi.h
//***************************************************************************
