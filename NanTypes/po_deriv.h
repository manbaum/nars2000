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
//  Deriv enums, strucs, etc.
//***************************************************************************

#define MAX_DEGREE      9
#define MIN_ORDER       2
#define MAX_ORDER       7
#define DEF_ORDER       5


// Definition of an arbitrary function with parameters
struct gsl_function_vfp_struct
{
    APLVFP (*function) (LPAPLVFP x, void *params);
    void *params;
};

typedef struct gsl_function_vfp_struct gsl_function_vfp ;

#define GSL_FN_VFP_EVAL(F,x) (*((F)->function))(x,(F)->params)

struct gsl_function_arb_struct
{
    APLARB (*function) (LPAPLARB x, void *params);
    void *params;
};

typedef struct gsl_function_arb_struct gsl_function_arb ;

#define GSL_FN_ARB_EVAL(F,x) (*((F)->function))(x,(F)->params)


//***************************************************************************
//  End of File: po_deriv.h
//***************************************************************************
