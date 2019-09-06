//***************************************************************************
//  NARS2000 -- Primitive Operator -- Deriv Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2019 Sudley Place Software

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


//***************************************************************************
//  Variant enums, strucs, etc.
//***************************************************************************

// Definition of an arbitrary function with parameters
struct vfp_function_struct
{
    APLVFP (*function) (LPAPLVFP x, void *params);
    void *params;
};

typedef struct vfp_function_struct vfp_function ;

#define VFP_FN_EVAL(F,x) (*((F)->function))(x,(F)->params)


//***************************************************************************
//  End of File: po_deriv.h
//***************************************************************************
