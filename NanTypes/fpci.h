//***************************************************************************
//  NARS2000 -- Fixed Precision Complex Integer Header
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

#ifndef __FPCI_H
#define __FPCI_H

typedef union tagFPCI
{
    fpri_t parts[2];
    struct tagFPCIPARTS
    {
        fpri_t partsLo,
               partsHi;
    };
} __fpci_struct, fpci_str;


typedef __fpci_struct   fpci_t[1];
typedef __fpci_struct  *fpci_ptr;

// Used to return a struct as a result
typedef __fpci_struct   fpci_res;


//***************************************************************************
//  Define prototypes
//***************************************************************************



//***************************************************************************
//  Define names for the FPCI_ functions
//***************************************************************************



#endif /* __FPCI_H */


//***************************************************************************
//  End of File: fpci.h
//***************************************************************************
