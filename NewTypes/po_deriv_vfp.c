//***************************************************************************
//  NARS2000 -- Primitive Operator -- Deriv:  MPFR Routines
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

typedef int (*MPFR_DERIV)
    (const vfp_function *f,
     LPAPLVFP            x,
     LPAPLVFP            h,
     LPAPLVFP            result,
     LPAPLVFP            abserr);

typedef int (*MPFR_DERIV_CALC)
    (const vfp_function *f,
     LPAPLVFP            x,
     LPAPLVFP            h,
     LPAPLVFP            result,
     LPAPLVFP            abserr_round,
     LPAPLVFP            abserr_trunc);

typedef struct tagLCLPARAMS
{
    LPTOKEN         lptkLftArg;
    LPPL_YYSTYPE    lpYYFcnStrLft;
    LPUBOOL         lpbCtrlBreak;
    BOOL            bInitPTD;
    EXCEPTION_CODES exCode;        /* Exception code from lclFuncXXX */
} LCL_PARAMS, *LPLCL_PARAMS;


#define MAX_DEGREE      9
#define MIN_ORDER       2
#define MAX_ORDER       7
#define DEF_ORDER       5


//***************************************************************************
//  Local VFP Function
//***************************************************************************

APLVFP lclFuncVfp
    (LPAPLVFP X,
     LPVOID   params)

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result
    TOKEN        tkRhtArg = {0},        // Save area for right arg token
                 tkRes = {0};           // ...           result    ...
    APLSTYPE     aplTypeRes;            // Storage type of the result
    APLNELM      aplNELMRes;            // NELM         ...
    APLVFP       aplVfpRht = {0};       // Immediate value ...
    UBOOL        bRet = FALSE;          // TRUE iff the result is valid

    // Check for Ctrl-Break
    if (CheckCtrlBreak (((LPLCL_PARAMS) params)->lpbCtrlBreak))
    {
        // Set the exception code
        ((LPLCL_PARAMS) params)->exCode = EXCEPTION_CTRL_BREAK;

        // Initialize the result to a NaN
        mpfr_init (&aplVfpRht);
    } else
    {
        // If we have not as yet initialized the PTD vars, ...
        if (!((LPLCL_PARAMS) params)->bInitPTD)
        {
            // Initialize VFP constants as the default precision may have changed
            InitVfpPrecision (mpfr_get_default_prec ());

            // Initialize PerTabData vars
            InitPTDVars (GetMemPTD ());

            // Set for the next time
            ((LPLCL_PARAMS) params)->bInitPTD = TRUE;
        } // End IF

        // Save X in global memory
        lpYYRes = SaveGlbNumeric_EM (ARRAY_VFP, X, -1);

        // Fill in the right arg token
        CopyAll (&tkRhtArg, &lpYYRes->tkToken);

        // Free it
        YYFree (lpYYRes); lpYYRes = NULL;

        // Execute the function monadically/dyadically on/between the left arg and the input parameter X
        lpYYRes =
          ExecFuncStr_EM_YY (
             ((LPLCL_PARAMS) params)->lptkLftArg,       // Ptr to left arg token
             ((LPLCL_PARAMS) params)->lpYYFcnStrLft,    // Ptr to left operand function strand
                            &tkRhtArg,                  // Ptr to right arg token (may be NULL if niladic)
                             NULL);                     // Ptr to operator axis token
        Assert (lpYYRes NE NULL);

        // Free the right arg token
        FreeResultTkn (&tkRhtArg);

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

            // Initialize the result to a NaN
            mpfr_init (&aplVfpRht);
        } else
        {
            // Get the singleton value
            aplVfpRht = GetNextVfpToken (&tkRes, 0, aplTypeRes, &bRet);

            // Check for error
            if (!bRet)
            {
                // Set the exception code
                ((LPLCL_PARAMS) params)->exCode = EXCEPTION_DOMAIN_ERROR;

                // Set the result to a NaN
                mpfr_set_nan (&aplVfpRht);
            } // End IF
        } // End IF/ELSE

        // Free the token
        FreeResultTkn (&tkRes);
    } // End IF/ELSE

    return aplVfpRht;
} // End lclFuncVfp


//***************************************************************************
//  Local VFP Function
//***************************************************************************

void lclFuncVfp2
    (mpfr_ptr R,
     mpfr_ptr X,
     LPVOID params)

{
    APLVFP p;

    p = lclFuncVfp (X, params);
    mpfr_set (R, &p, MPFR_RNDN);
    Myf_clear (&p);
} // End lclFuncVfp2


//***************************************************************************
//  $mpfr_deriv_forward_calc
//
//  Calculate the finite differences
//
//  This routine was translated into MPFR from the original GSL routine
//    <forward_deriv> in <gsl\deriv\deriv.c>.
//***************************************************************************

void mpfr_deriv_forward_calc
    (const vfp_function *f,
     LPAPLVFP            x,
     LPAPLVFP            h,
     LPAPLVFP            result,
     LPAPLVFP            abserr_round,
     LPAPLVFP            abserr_trunc)

{
    APLVFP T = {0},
           r2 = {0},
           r4 = {0},
           e4 = {0},
           dy = {0};
    APLVFP dbleps = {0};    // Set to 2*(1 - []FPC)
    mpfr_init_set_si (&dbleps, (long int) (1 - GetQuadFPC ()), MPFR_RNDN);  // 1 - []FPC
    mpfr_exp2        (&dbleps, &dbleps, MPFR_RNDN);                         // pow (2, 1 - []FPC)

  /* Compute the derivative using the 4-point rule (x+h/4, x+h/2,
     x+3h/4, x+h).

     Compute the error using the difference between the 4-point and
     the 2-point rule (x+h/2,x+h).  */

    mpfr_init   (&T);
    mpfr_init   (&r2);
    mpfr_init   (&r4);
    mpfr_init   (&e4);
    mpfr_init   (&dy);

    // double f1 = GSL_FN_EVAL (f, x + h / 4.0);
    mpfr_set    (&T, h, MPFR_RNDN);             //         h
    mpfr_div_si (&T, &T, 4, MPFR_RNDN);         //         h / 4
    mpfr_add    (&T, x, &T, MPFR_RNDN);         //     x + h / 4
    APLVFP f1 = VFP_FN_EVAL (f, &T);            // (f, x + h / 4);

    // double f2 = GSL_FN_EVAL (f, x + h / 2.0);
    mpfr_set    (&T, h, MPFR_RNDN);             //         h
    mpfr_div_si (&T, &T, 2, MPFR_RNDN);         //         h / 2
    mpfr_add    (&T, x, &T, MPFR_RNDN);         //     x + h / 2
    APLVFP f2 = VFP_FN_EVAL (f, &T);            // (f, x + h / 2);

    // double f3 = GSL_FN_EVAL (f, x + (3.0 / 4.0) * h);
    mpfr_set    (&T, h, MPFR_RNDN);             //         h
    mpfr_mul_si (&T, &T, 3, MPFR_RNDN);         //         h * 3
    mpfr_div_si (&T, &T, 4, MPFR_RNDN);         //         h * 3 / 4
    mpfr_add    (&T, x, &T, MPFR_RNDN);         //     x + h * 3 / 4
    APLVFP f3 = VFP_FN_EVAL (f, &T);            // (f, x + h * 3 / 4);

    // double f4 = GSL_FN_EVAL (f, x + h);
    mpfr_set    (&T, h, MPFR_RNDN);             //         h
    mpfr_add    (&T, x, &T, MPFR_RNDN);         //     x + h
    APLVFP f4 = VFP_FN_EVAL (f, &T);            // (f, x + h);

    // double r2 = 2.0 * (f4 - f2);
    mpfr_set    (&r2, &f4, MPFR_RNDN);          //          f4
    mpfr_sub    (&r2, &r2, &f2, MPFR_RNDN);     //          f4 - f2
    mpfr_mul_si (&r2, &r2,   2, MPFR_RNDN);     //   2.0 * (f4 - f2)

    // double r4 = (22.0 / 3.0) * (f4 - f3) - (62.0 / 3.0) * (f3 - f2) + (52.0 / 3.0) * (f2 - f1);
    mpfr_set    (&r4, &f4, MPFR_RNDN);          //           f4
    mpfr_sub    (&r4, &r4, &f3, MPFR_RNDN);     //           f4 - f3
    mpfr_mul_si (&r4, &r4, 22, MPFR_RNDN);      //     22 * (f4 - f3)
    mpfr_div_si (&r4, &r4,  3, MPFR_RNDN);      //     22 * (f4 - f3) / 3

    mpfr_set    (&T,  &f3, MPFR_RNDN);          //           f3
    mpfr_sub    (&T,  &T, &f2, MPFR_RNDN);      //           f3 - f2
    mpfr_mul_si (&T,  &T,  62, MPFR_RNDN);      //     62 * (f3 - f2)
    mpfr_div_si (&T,  &T,   3, MPFR_RNDN);      //     62 * (f3 - f2) / 3

    mpfr_sub    (&r4, &r4, &T, MPFR_RNDN);      //   (22 * (f4 - f3) / 3)
                                                // - (62 * (f3 - f2) / 3)
    mpfr_set    (&T,  &f2, MPFR_RNDN);          //             f2
    mpfr_sub    (&T,  &T,  &f1, MPFR_RNDN);     //             f2 - f1
    mpfr_mul_si (&T,  &T,  52, MPFR_RNDN);      //       52 * (f2 - f1)
    mpfr_div_si (&T,  &T,  3, MPFR_RNDN);       //       52 * (f2 - f1) / 3
    mpfr_add    (&r4, &r4, &T, MPFR_RNDN);      // r4 += 52 * (f2 - f1) / 3

    /* Estimate the rounding error for r4 */
    // double e4 = 2 * 20.67 * (fabs (f4) + fabs (f3) + fabs (f2) + fabs (f1)) * GSL_DBL_EPSILON;
    mpfr_set    (&e4, &f4, MPFR_RNDN);          //                   f4
    mpfr_abs    (&e4, &e4, MPFR_RNDN);          //              abs (f4)
    mpfr_set    (&T,  &f3, MPFR_RNDN);          //                              f3
    mpfr_abs    (&T,  &T,  MPFR_RNDN);          //                         abs (f3)
    mpfr_add    (&e4, &e4, &T, MPFR_RNDN);      //              abs (f4) + abs (f3)
    mpfr_set    (&T,  &f2, MPFR_RNDN);          //                                         f2
    mpfr_abs    (&T,  &T,  MPFR_RNDN);          //                                    abs (f2)
    mpfr_add    (&e4, &e4, &T, MPFR_RNDN);      //              abs (f4) + abs (f3) + abs (f2)
    mpfr_set    (&T,  &f1, MPFR_RNDN);          //                                                    f1
    mpfr_abs    (&T,  &T,  MPFR_RNDN);          //                                               abs (f1)
    mpfr_add    (&e4, &e4, &T, MPFR_RNDN);      //              abs (f4) + abs (f3) + abs (f2) + abs (f1)
    mpfr_mul_si (&e4, &e4, 2, MPFR_RNDN);       // 2 *         (abs (f4) + abs (f3) + abs (f2) + abs (f1))
    mpfr_set_d  (&T,  20.67, MPFR_RNDN);        //     20.67
    mpfr_mul    (&e4, &e4, &T, MPFR_RNDN);      // 2 * 20.67 * (abs (f4) + abs (f3) + abs (f2) + abs (f1))
    mpfr_mul    (&e4, &e4, &dbleps, MPFR_RNDN); // 2 * 20.67 * (abs (f4) + abs (f3) + abs (f2) + abs (f1)) * GSL_DBL_EPSILON

    /* The next term is due to finite precision in x+h = O (eps * x) */
    // double dy = GSL_MAX (fabs (r2 / h), fabs (r4 / h)) * fabs (x / h) * GSL_DBL_EPSILON;
    mpfr_set   (&dy, &r2, MPFR_RNDN);           //           r2
    mpfr_div   (&dy, &dy, h, MPFR_RNDN);        //           r2 / h
    mpfr_abs   (&dy, &dy, MPFR_RNDN);           //      abs (r2 / h)
    mpfr_set   (&T,  &r4, MPFR_RNDN);           //                         r4
    mpfr_div   (&T,  &T,  h, MPFR_RNDN);        //                         r4 / h
    mpfr_abs   (&T,  &T,  MPFR_RNDN);           //                    abs (r4 / h)
    mpfr_max   (&dy, &dy, &T, MPFR_RNDN);       // max (abs (r2 / h), abs (r4 / h))
    mpfr_set   (&T,   x, MPFR_RNDN);            //                                          x
    mpfr_abs   (&T, &T, MPFR_RNDN);             //                                     abs (x)
    mpfr_div   (&T, &T, h, MPFR_RNDN);          //                                     abs (x) / h
    mpfr_mul   (&dy, &dy, &T, MPFR_RNDN);       // max (abs (r2 / h), abs (r4 / h)) * (abs (x) / h)
    mpfr_mul   (&dy, &dy, &dbleps, MPFR_RNDN);  // max (abs (r3 / h), abs (r4 / h)) * (abs (x) / h) * GSL_DBL_EPSILON

    /* The truncation error in the r4 approximation itself is O(h^3).
       However, for safety, we estimate the error from r4-r2, which is
       O(h).  By scaling h we will minimise this estimated error, not
       the actual truncation error in r4. */

    // *result = r4 / h;
    mpfr_set   (&T, &r4, MPFR_RNDN);                //           r4
    mpfr_div   (result, &T, h, MPFR_RNDN);          // *result = r4

    // *abserr_trunc = fabs ((r4 - r2) / h); /* Estimated truncation error O(h) */
    mpfr_set   (&T, &r4, MPFR_RNDN);                //       r4
    mpfr_sub   (&T, &T, &r2, MPFR_RNDN);            //       r4 - r2
    mpfr_div   (&T, &T, h, MPFR_RNDN);              //      (r4 - r2) / h
    mpfr_abs   (abserr_trunc, &T, MPFR_RNDN);       // abs ((r4 - r2) / h)

    // *abserr_round = fabs (e4 / h) + dy;
    mpfr_set   (&T, &e4, MPFR_RNDN);                //      e4
    mpfr_div   (&T, &T, h, MPFR_RNDN);              //      e4 / h
    mpfr_abs   (&T, &T, MPFR_RNDN);                 // abs (e4 / h)
    mpfr_add   (abserr_round, &T, &dy, MPFR_RNDN);  // abs (e4 / h) + dy

    Myf_clear (&dy);
    Myf_clear (&e4);
    Myf_clear (&r4);
    Myf_clear (&r2);
    Myf_clear (&f4);
    Myf_clear (&f3);
    Myf_clear (&f2);
    Myf_clear (&f1);
    Myf_clear (&T);
    Myf_clear (&dbleps);
} // End mpfr_deriv_forward_calc


//**************************************************************************
//  $mpfr_deriv_central_calc
//
//  Calculate the finite differences
//
//  This routine was translated into MPFR from the original GSL routine
//    <central_deriv> in <gsl\deriv\deriv.c>.
//***************************************************************************

void mpfr_deriv_central_calc
    (const vfp_function *f,
     LPAPLVFP            x,
     LPAPLVFP            h,
     LPAPLVFP            result,
     LPAPLVFP            abserr_round,
     LPAPLVFP            abserr_trunc)

{
    APLVFP dbleps = {0};    // Set to 2*(1 - []FPC)
    mpfr_init_set_si (&dbleps, (long int) (1 - GetQuadFPC ()), MPFR_RNDN);  // 1 - []FPC
    mpfr_exp2        (&dbleps, &dbleps, MPFR_RNDN);                         // pow (2, 1 - []FPC)

  /* Compute the derivative using the 5-point rule (x-h, x-h/2, x,
     x+h/2, x+h). Note that the central point is not used.

     Compute the error using the difference between the 5-point and
     the 3-point rule (x-h,x,x+h). Again the central point is not
     used. */

    APLVFP T = {0};
    mpfr_init   (&T);

    mpfr_set    (&T, h, MPFR_RNDN);             //         h
    mpfr_sub    (&T, x, &T, MPFR_RNDN);         //     x - h
    APLVFP fm1 = VFP_FN_EVAL (f, &T);           // (f, x - h);

    mpfr_set    (&T, h, MPFR_RNDN);             //         h
    mpfr_add    (&T, x, &T, MPFR_RNDN);         //     x + h
    APLVFP fp1 = VFP_FN_EVAL (f, &T);           // (f, x + h);

    mpfr_set    (&T, h, MPFR_RNDN);             //         h
    mpfr_div_si (&T, &T, 2, MPFR_RNDN);         //         h / 2
    mpfr_sub    (&T, x, &T, MPFR_RNDN);         //     x - h / 2
    APLVFP fmh = VFP_FN_EVAL (f, &T);           // (f, x - h / 2);

    mpfr_set    (&T, h, MPFR_RNDN);             //         h
    mpfr_div_si (&T, &T, 2, MPFR_RNDN);         //         h / 2
    mpfr_add    (&T, x, &T, MPFR_RNDN);         //     x + h / 2
    APLVFP fph = VFP_FN_EVAL (f, &T);           // (f, x + h / 2);

    APLVFP r3 = {0};        // r3 = 0.5 * (fp1 - fm1);
    mpfr_init   (&r3);
    mpfr_set    (&r3, &fp1, MPFR_RNDN);         //  fp1
    mpfr_sub    (&r3, &r3, &fm1, MPFR_RNDN);    //  fp1 - fm1
    mpfr_div_si (&r3, &r3, 2, MPFR_RNDN);       // (fp1 - fm1) / 2

    APLVFP r5 = {0};        // r5 = (4.0 / 3.0) * (fph - fmh) - (1.0 / 3.0) * r3;
    mpfr_init   (&r5);
    mpfr_set    (&r5, &fph, MPFR_RNDN);         //      fph
    mpfr_sub    (&r5, &r5, &fmh, MPFR_RNDN);    //      fph - fmh
    mpfr_mul_si (&r5, &r5, 4, MPFR_RNDN);       // 4 * (fph -fmh)
    mpfr_div_si (&r5, &r5, 3, MPFR_RNDN);       // 4 * (fph - fmh) / 3
    mpfr_set    (&T, &r3, MPFR_RNDN);           //                              r3
////mpfr_mul_si (&T, &T, 1, MPFR_RNDN);         //                          1 * r3
    mpfr_div_si (&T, &T, 3, MPFR_RNDN);         //                          1 * r3 / 3
    mpfr_sub    (&r5, &r5, &T, MPFR_RNDN);      // (4 * (fph - fmh) / 3) - (1 * r3 / 3)

    APLVFP e3 = {0};        // e3 = (fabs (fp1) + fabs (fm1)) * GSL_DBL_EPSILON;
    mpfr_init  (&e3);
    mpfr_set   (&e3, &fp1, MPFR_RNDN);          //      fp1
    mpfr_abs   (&e3, &e3, MPFR_RNDN);           // abs (fp1)
    mpfr_set   (&T, &fm1, MPFR_RNDN);           //                  fm1
    mpfr_abs   (&T, &T, MPFR_RNDN);             //             abs (fm1)
    mpfr_add   (&e3, &e3, &T, MPFR_RNDN);       // abs (fp1) + abs (fm1)
#ifdef DEBUG
////VfpOut     (L"abs (fp1) + abs (fm1) = ", &e3);
#endif
    mpfr_mul   (&e3, &e3, &dbleps, MPFR_RNDN);  // abs (fp1) + abs (fm1) * GSL_DBL_EPSILON
#ifdef DEBUG
////VfpOut     (L"abs (fp1) + abs (fm1) * dbleps =", &e3);
#endif

    APLVFP e5 = {0};        // e5 = 2.0 * (fabs (fph) + fabs (fmh)) * GSL_DBL_EPSILON + e3;
    mpfr_init  (&e5);
    mpfr_set   (&e5, &fph, MPFR_RNDN);          //            fph
    mpfr_abs   (&e5, &e5, MPFR_RNDN);           //       abs (fph)
    mpfr_set   (&T, &fmh, MPFR_RNDN);           //                        fmh
    mpfr_abs   (&T, &T, MPFR_RNDN);             //                   abs (fmh)
    mpfr_add   (&e5, &e5, &T, MPFR_RNDN);       //       abs (fph) + abs (fmh)
#ifdef DEBUG
////VfpOut     (L"abs (fph) + abs (fmh) = ", &e5);
#endif
    mpfr_mul   (&e5, &e5, &dbleps, MPFR_RNDN);  //       abs (fph) + abs (fmh) * GSL_DBL_EPSILON
#ifdef DEBUG
////VfpOut     (L"abs (fph) + abs (fmh) * dbleps = ", &e5);
#endif
    mpfr_mul_si(&e5, &e5, 2, MPFR_RNDN);        // 2.0 * abs (fph) + abs (fmh) * GSL_DBL_EPSILON
#ifdef DEBUG
////VfpOut     (L"2 * abs (fph) + abs (fmh) * dbleps = ", &e5);
#endif
    mpfr_add   (&e5, &e5, &e3, MPFR_RNDN);      // 2.0 * abs (fph) + abs (fmh) * GSL_DBL_EPSILON + e3
#ifdef DEBUG
////VfpOut     (L"2 * abs (fph) + abs (fmh) * dbleps + e3 = ", &e5);
#endif

    // The next term is due to finite precision in x+h = O (eps * x)
    APLVFP dy = {0};        // dy = GSL_MAX (fabs (r3 / h), fabs (r5 / h)) *(fabs (x) / h) * GSL_DBL_EPSILON;
    mpfr_init  (&dy);
    mpfr_set   (&dy, &r3, MPFR_RNDN);           //           r3
    mpfr_div   (&dy, &dy, h, MPFR_RNDN);        //           r3 / h
    mpfr_abs   (&dy, &dy, MPFR_RNDN);           //      abs (r3 / h)
    mpfr_set   (&T, &r5, MPFR_RNDN);            //                         r5
    mpfr_div   (&T, &T, h, MPFR_RNDN);          //                         r5 / h
    mpfr_abs   (&T, &T, MPFR_RNDN);             //                    abs (r5 / h)
    mpfr_max   (&dy, &dy, &T, MPFR_RNDN);       // max (abs (r3 / h), abs (r5 / h))
    mpfr_set   (&T, x, MPFR_RNDN);              //                                           x
    mpfr_abs   (&T, &T, MPFR_RNDN);             //                                     fabs (x)
    mpfr_div   (&T, &T, h, MPFR_RNDN);          //                                     fabs (x) / h
    mpfr_mul   (&dy, &dy, &T, MPFR_RNDN);       // max (abs (r3 / h), abs (r5 / h)) * (fabs (x) / h)
    mpfr_mul   (&dy, &dy, &dbleps, MPFR_RNDN);  // max (abs (r3 / h), abs (r5 / h)) * (fabs (x) / h) * GSL_DBL_EPSILON

    /* The truncation error in the r5 approximation itself is O(h^4).
       However, for safety, we estimate the error from r5-r3, which is
       O(h^2).  By scaling h we will minimise this estimated error, not
       the actual truncation error in r5. */

    // *result = r5 / h;
    mpfr_set   (&T, &r5, MPFR_RNDN);                //           r5
    mpfr_div   (result, &T, h, MPFR_RNDN);          // *result = r5

    // *abserr_trunc = fabs ((r5 - r3) / h); /* Estimated truncation error O(h^2) */
    mpfr_set   (&T, &r5, MPFR_RNDN);                //       r5
    mpfr_sub   (&T, &T, &r3, MPFR_RNDN);            //       r5 - r3
    mpfr_div   (&T, &T, h, MPFR_RNDN);              //      (r5 - r3) / h
    mpfr_abs   (abserr_trunc, &T, MPFR_RNDN);       // abs ((r5 - r3) / h)

    // *abserr_round = fabs (e5 / h) + dy;   /* Rounding error (cancellations) */
    mpfr_set   (&T, &e5, MPFR_RNDN);                //      e5
    mpfr_div   (&T, &T, h, MPFR_RNDN);              //      e5 / h
    mpfr_abs   (&T, &T, MPFR_RNDN);                 // abs (e5 / h)
    mpfr_add   (abserr_round, &T, &dy, MPFR_RNDN);  // abs (e5 / h) + dy

    Myf_clear (&dy);
    Myf_clear (&e5);
    Myf_clear (&e3);
    Myf_clear (&r5);
    Myf_clear (&r3);
    Myf_clear (&fph);
    Myf_clear (&fmh);
    Myf_clear (&fp1);
    Myf_clear (&fm1);
    Myf_clear (&T);
    Myf_clear (&dbleps);
} // End mpfr_deriv_central_calc


//***************************************************************************
//  $mpfr_deriv_central2
//
//  Compute the first derivative of the function f
//
//  This routine was translated into MPFR from the original GSL routine
//    <gsl_deriv_central> in <gsl\deriv\deriv.c>.
//***************************************************************************

int mpfr_deriv_central2
    (const vfp_function *f,
     LPAPLVFP            x,
     LPAPLVFP            h,
     LPAPLVFP            result,
     LPAPLVFP            abserr)

{
    return
      mpfr_deriv_common (f,
                         x,
                         h,
                         result,
                         abserr,
                        &mpfr_deriv_central_calc);
} // End mpfr_deriv_central2


//***************************************************************************
//  $mpfr_deriv_forward
//
//  Compute the first derivative of the function f
//
//  This routine was translated into MPFR from the original GSL routine
//    <gsl_deriv_forward> in <gsl\deriv\deriv.c>.
//***************************************************************************

int mpfr_deriv_forward
    (const vfp_function *f,
     LPAPLVFP            x,
     LPAPLVFP            h,
     LPAPLVFP            result,
     LPAPLVFP            abserr)

{
    return
      mpfr_deriv_common (f,
                         x,
                         h,
                         result,
                         abserr,
                        &mpfr_deriv_forward_calc);
} // End mpfr_deriv_forward


//***************************************************************************
//  $mpfr_deriv_backward
//
//  Compute the first derivative of the function f
//
//  This routine was translated into MPFR from the original GSL routine
//    <gsl_deriv_backward> in <gsl\deriv\deriv.c>.
//***************************************************************************

int mpfr_deriv_backward
    (const vfp_function *f,
     LPAPLVFP            x,
     LPAPLVFP            h,
     LPAPLVFP            result,
     LPAPLVFP            abserr)

{
    APLVFP T = {0};
    int    iRet;

    mpfr_init (&T);
    mpfr_set  (&T,  h, MPFR_RNDN);
    mpfr_neg  (&T, &T, MPFR_RNDN);

    iRet =
      mpfr_deriv_common (f,
                         x,
                        &T,
                         result,
                         abserr,
                        &mpfr_deriv_forward_calc);
    mpfr_clear (&T);

    return iRet;
} // End mpfr_deriv_backward


//***************************************************************************
//  $mpfr_deriv_common
//
//  Compute the first derivative of the function f
//
//  This routine was translated into MPFR from the original GSL routine
//    <gsl_deriv_XXXXXXX> in <gsl\deriv\deriv.c>.
//***************************************************************************

int mpfr_deriv_common
    (const vfp_function *f,
     LPAPLVFP            x,
     LPAPLVFP            h,
     LPAPLVFP            result,
     LPAPLVFP            abserr,
     void               *mpfr_deriv_calc)

{
    APLVFP r_0 = {0},
           round = {0},
           trunc = {0},
           error = {0};

    mpfr_init  (&r_0);
    mpfr_init  (&round);
    mpfr_init  (&trunc);
    mpfr_init  (&error);

    // Calculate the finite differences
    (*(MPFR_DERIV_CALC) mpfr_deriv_calc) (f, x, h, &r_0, &round, &trunc);

    // error = round + trunc;
    mpfr_set (&error, &round, MPFR_RNDN);
    mpfr_add (&error, &error, &trunc, MPFR_RNDN);

    // (round < trunc && (round > 0 && trunc > 0))
    if (mpfr_less_p (&round, &trunc) && mpfr_sgn (&round) > 0 && mpfr_sgn (&trunc) > 0)
    {
        APLVFP r_opt     = {0},
               round_opt = {0},
               trunc_opt = {0},
               error_opt = {0},
               T1        = {0},
               T2        = {0};
        mpfr_init  (&r_opt);
        mpfr_init  (&round_opt);
        mpfr_init  (&trunc_opt);
        mpfr_init  (&error_opt);
        mpfr_init  (&T1);
        mpfr_init  (&T2);

        /* Compute an optimised stepsize to minimize the total error,
           using the scaling of the truncation error (O(h^2)) and
           rounding error (O(1/h)). */

        APLVFP h_opt = {0};     // h_opt = h * pow (round / (2.0 * trunc), 1.0 / 3.0);
        mpfr_init   (&h_opt);
        mpfr_set    (&h_opt, &trunc, MPFR_RNDN);            //                                    trunc
        mpfr_mul_si (&h_opt, &h_opt, 2, MPFR_RNDN);         //                              2.0 * trunc
        mpfr_div    (&h_opt, &round, &h_opt, MPFR_RNDN);    //                     round / (2.0 * trunc)
        mpfr_cbrt   (&h_opt, &h_opt, MPFR_RNDN);            //             cubrt ( round / (2.0 * trunc))
        mpfr_mul    (&h_opt, &h_opt, h, MPFR_RNDN);         // h_opt = h * cubrt ( round / (2.0 * trunc))

        // Calculate the finite differences
        (*(MPFR_DERIV_CALC) mpfr_deriv_calc) (f, x, &h_opt, &r_opt, &round_opt, &trunc_opt);

        // error_opt = round_opt + trunc_opt;
        mpfr_set (&error_opt, &round_opt, MPFR_RNDN);
        mpfr_add (&error_opt, &error_opt, &trunc_opt, MPFR_RNDN);

        /* Check that the new error is smaller, and that the new derivative
           is consistent with the error bounds of the original estimate. */
        // (error_opt < error && fabs (r_opt - r_0) < 4.0 * error)
        mpfr_set    (&T1, &r_opt, MPFR_RNDN);               //           r_opt
        mpfr_sub    (&T1, &T1, &r_0, MPFR_RNDN);            //           r_opt - r_0
        mpfr_abs    (&T1, &T1, MPFR_RNDN);                  // T1 = abs (r_opt - r_0)
        mpfr_set    (&T2, &error, MPFR_RNDN);               //            error
        mpfr_mul_si (&T2, &T2, 4, MPFR_RNDN);               // T2 = 4.0 * error

        if (mpfr_less_p (&error_opt, &error) && mpfr_less_p (&T1, &T2))
        {
            mpfr_set (&r_0, &r_opt, MPFR_RNDN);             // r_0   = r_opt
            mpfr_set (&error, &error_opt, MPFR_RNDN);       // error = error_opt
        } // End IF

        Myf_clear (&T2);
        Myf_clear (&T1);
        Myf_clear (&h_opt);
        Myf_clear (&error_opt);
        Myf_clear (&trunc_opt);
        Myf_clear (&round_opt);
        Myf_clear (&r_opt);
    } // End IF

    mpfr_set (result, &r_0, MPFR_RNDN);
    mpfr_set (abserr, &error, MPFR_RNDN);

    Myf_clear (&error);
    Myf_clear (&trunc);
    Myf_clear (&round);
    Myf_clear (&r_0);

    return GSL_SUCCESS;
} // End mpfr_deriv_common


//***************************************************************************
//  End of File: po_deriv_vfp.c
//***************************************************************************
