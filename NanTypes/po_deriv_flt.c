//***************************************************************************
//  NARS2000 -- Primitive Operator -- Deriv:  GSL Routines
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

#define STRICT
#include <windows.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_deriv.h>

#include "headers.h"

typedef int (*GSL_DERIV)
    (const gsl_function *f,
     double x, double h,
     double *result, double *abserr);

typedef struct tagLCLPARAMS
{
    LPTOKEN         lptkLftArg;
    LPPL_YYSTYPE    lpYYFcnStrLft;
    LPUBOOL         lpbCtrlBreak;
    BOOL            bInitPTD;
    EXCEPTION_CODES exCode;        /* Exception code from lclFuncXXX */
} LCL_PARAMS, *LPLCL_PARAMS;


//***************************************************************************
//  Local GSL Function
//***************************************************************************

double lclFuncFlt
    (double X,
     LPVOID params)

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result
    TOKEN        tkRhtArg = {0},        // Save area for right arg token
                 tkRes = {0};           // ...           result    ...
    APLSTYPE     aplTypeRes;            // Storage type of the result
    APLNELM      aplNELMRes;            // NELM         ...
    APLFLOAT     aplFltRht;             // Immediate value ...
    UBOOL        bRet = FALSE;          // TRUE iff the result is valid

    // Check for Ctrl-Break
    if (CheckCtrlBreak (((LPLCL_PARAMS) params)->lpbCtrlBreak))
    {
        // Set the exception code
        ((LPLCL_PARAMS) params)->exCode = EXCEPTION_CTRL_BREAK;

        // Set the result to a NaN
        aplFltRht = fltNaN;
    } else
    {
        // Fill in the right arg token
        tkRhtArg.tkFlags.TknType   = TKT_VARIMMED;
        tkRhtArg.tkFlags.ImmType   = IMMTYPE_FLOAT;
////////tkRhtArg.tkFlags.NoDisplay = FALSE;     // Already zero from = {0};
        tkRhtArg.tkData.tkFloat    = X;
        tkRhtArg.tkCharIndex       = -1;

        // Execute the function monadically/dyadically on/between the left arg and the input parameter X
        lpYYRes =
          ExecFuncStr_EM_YY (
             ((LPLCL_PARAMS) params)->lptkLftArg,       // Ptr to left arg token
             ((LPLCL_PARAMS) params)->lpYYFcnStrLft,    // Ptr to left operand function strand
                            &tkRhtArg,                  // Ptr to right arg token (may be NULL if niladic)
                             NULL);                     // Ptr to operator axis token
        if (lpYYRes EQ NULL)
        {
            // Set the exception code
            ((LPLCL_PARAMS) params)->exCode = EXCEPTION_ERRMSG_ALREADY_SET;

            // Set the result to a NaN
            aplFltRht = fltNaN;
        } else
        {
            // Get the attributes (Type, NELM, and Rank) of the result
            AttrsOfToken (&lpYYRes->tkToken, &aplTypeRes, &aplNELMRes, NULL, NULL);

            // Save the token locally so we can free the YYSTYPE
            CopyAll (&tkRes, &lpYYRes->tkToken);

            // Free the YYSTYPE
            YYFree (lpYYRes); lpYYRes = NULL;

            // Ensure the result is a simple numeric singleton
            if (!IsSingleton (aplNELMRes))
            {
                // Set the exception code
                ((LPLCL_PARAMS) params)->exCode = EXCEPTION_LENGTH_ERROR;

                // Set the result to a NaN
                aplFltRht = fltNaN;
            } else
            {
                // Get the singleton value
                aplFltRht = GetNextFloatToken (&tkRes, 0, aplTypeRes, &bRet);

                // Check for error
                if (!bRet)
                {
                    // Set the exception code
                    ((LPLCL_PARAMS) params)->exCode = EXCEPTION_DOMAIN_ERROR;

                    // Set the result to a NaN
                    aplFltRht = fltNaN;
                } // End IF
            } // End IF/ELSE
        } // End IF/ELSE
    } // End IF/ELSE

    return aplFltRht;
} // End lclFuncFlt


//***************************************************************************
//  End of File: po_deriv_flt.c
//***************************************************************************
