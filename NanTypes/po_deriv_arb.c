//***************************************************************************
//  NARS2000 -- Primitive Operator -- Deriv:  ARB Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2020 Sudley Place Software

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


//***************************************************************************
//  Local ARB Function
//***************************************************************************

APLARB lclFuncArb
    (LPAPLARB X,
     LPVOID   params)

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result
    TOKEN        tkRhtArg = {0},        // Save area for right arg token
                 tkRes = {0};           // ...           result    ...
    APLSTYPE     aplTypeRes;            // Storage type of the result
    APLNELM      aplNELMRes;            // NELM         ...
    APLARB       aplArbRht = {0};       // Immediate value ...
    UBOOL        bRet = FALSE;          // TRUE iff the result is valid

    // Check for Ctrl-Break
    if (CheckCtrlBreak (((LPLCL_PARAMS) params)->lpbCtrlBreak))
    {
        // Set the exception code
        ((LPLCL_PARAMS) params)->exCode = EXCEPTION_CTRL_BREAK;

        // Initialize the result to a NaN
        arb_init (&aplArbRht);
    } else
    {
        // If we have not as yet initialized the PTD vars, ...
        if (!((LPLCL_PARAMS) params)->bInitPTD)
        {
            // Initialize ARB constants as the default precision may have changed
            InitVfpPrecision (mpfr_get_default_prec ());

            // Initialize PerTabData vars
            InitPTDVars (GetMemPTD ());

            // Set for the next time
            ((LPLCL_PARAMS) params)->bInitPTD = TRUE;
        } // End IF

        // Save X in global memory
        lpYYRes = SaveGlbNumeric_EM (ARRAY_ARB, X, -1);

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
            arb_init    (&aplArbRht);
            arb_set_nan (&aplArbRht);
        } else
        {
            // Get the singleton value
            aplArbRht = GetNextArbToken (&tkRes, 0, aplTypeRes, &bRet);

            // Check for error
            if (!bRet)
            {
                // Set the exception code
                ((LPLCL_PARAMS) params)->exCode = EXCEPTION_DOMAIN_ERROR;

                // Set the result to a NaN
                arb_set_nan (&aplArbRht);
            } // End IF
        } // End IF/ELSE

        // Free the token
        FreeResultTkn (&tkRes);
    } // End IF/ELSE

    return aplArbRht;
} // End lclFuncArb


//***************************************************************************
//  Local ARB Function
//***************************************************************************

void lclFuncArb2
    (arb_ptr R,
     arb_ptr X,
     LPVOID params)

{
    APLARB p;

    p = lclFuncArb (X, params);
    arb_set (R, &p);
    arb_clear (&p);
} // End lclFuncArb2


//***************************************************************************
//  $arb_deriv_forward_calc
//
//  Calculate the finite differences
//
//  This routine was translated into ARB from the original GSL routine
//    <forward_deriv> in <gsl\deriv\deriv.c>.
//***************************************************************************

void arb_deriv_forward_calc
    (const gsl_function_arb *f,
     LPAPLARB                x,
     LPAPLARB                h,
     LPAPLARB                result,
     LPAPLARB                abserr_round,
     LPAPLARB                abserr_trunc)

{
    mp_limb_signed_t prec = ARB_PREC_FPC;   // ARB precision
    APLARB T = {0},
           r2 = {0},
           r4 = {0},
           e4 = {0},
           dy = {0},
           con2 = {0};
    APLARB dbleps = {0};    // Set to 2*(1 - []FPC)

    arb_init_set_sx (&con2,   2);
    arb_init_set_sx (&dbleps, (long int) (1 - GetQuadFPC ()));        // 1 - []FPC
    arb_pow         (&dbleps, &con2, &dbleps, prec);                  // pow (2, 1 - []FPC)

  /* Compute the derivative using the 4-point rule (x+h/4, x+h/2,
     x+3h/4, x+h).

     Compute the error using the difference between the 4-point and
     the 2-point rule (x+h/2,x+h).  */

    arb_init   (&T);
    arb_init   (&r2);
    arb_init   (&r4);
    arb_init   (&e4);
    arb_init   (&dy);

    // double f1 = GSL_FN_EVAL (f, x + h / 4.0);
    arb_set    (&T, h);                   //         h
    arb_div_si (&T, &T, 4, prec);         //         h / 4
    arb_add    (&T, x, &T, prec);         //     x + h / 4
    APLARB f1 = GSL_FN_ARB_EVAL (f, &T);  // (f, x + h / 4);

    // double f2 = GSL_FN_EVAL (f, x + h / 2.0);
    arb_set    (&T, h);                   //         h
    arb_div_si (&T, &T, 2, prec);         //         h / 2
    arb_add    (&T, x, &T, prec);         //     x + h / 2
    APLARB f2 = GSL_FN_ARB_EVAL (f, &T);  // (f, x + h / 2);

    // double f3 = GSL_FN_EVAL (f, x + (3.0 / 4.0) * h);
    arb_set    (&T, h);                   //         h
    arb_mul_si (&T, &T, 3, prec);         //         h * 3
    arb_div_si (&T, &T, 4, prec);         //         h * 3 / 4
    arb_add    (&T, x, &T, prec);         //     x + h * 3 / 4
    APLARB f3 = GSL_FN_ARB_EVAL (f, &T);  // (f, x + h * 3 / 4);

    // double f4 = GSL_FN_EVAL (f, x + h);
    arb_set    (&T, h);                   //         h
    arb_add    (&T, x, &T, prec);         //     x + h
    APLARB f4 = GSL_FN_ARB_EVAL (f, &T);  // (f, x + h);

    // double r2 = 2.0 * (f4 - f2);
    arb_set    (&r2, &f4);                //          f4
    arb_sub    (&r2, &r2, &f2, prec);     //          f4 - f2
    arb_mul_si (&r2, &r2,   2, prec);     //   2.0 * (f4 - f2)

    // double r4 = (22.0 / 3.0) * (f4 - f3) - (62.0 / 3.0) * (f3 - f2) + (52.0 / 3.0) * (f2 - f1);
    arb_set    (&r4, &f4);                //           f4
    arb_sub    (&r4, &r4, &f3, prec);     //           f4 - f3
    arb_mul_si (&r4, &r4, 22, prec);      //     22 * (f4 - f3)
    arb_div_si (&r4, &r4,  3, prec);      //     22 * (f4 - f3) / 3

    arb_set    (&T,  &f3);                //           f3
    arb_sub    (&T,  &T, &f2, prec);      //           f3 - f2
    arb_mul_si (&T,  &T,  62, prec);      //     62 * (f3 - f2)
    arb_div_si (&T,  &T,   3, prec);      //     62 * (f3 - f2) / 3

    arb_sub    (&r4, &r4, &T, prec);      //   (22 * (f4 - f3) / 3)
                                                // - (62 * (f3 - f2) / 3)
    arb_set    (&T,  &f2);                //             f2
    arb_sub    (&T,  &T,  &f1, prec);     //             f2 - f1
    arb_mul_si (&T,  &T,  52, prec);      //       52 * (f2 - f1)
    arb_div_si (&T,  &T,  3, prec);       //       52 * (f2 - f1) / 3
    arb_add    (&r4, &r4, &T, prec);      // r4 += 52 * (f2 - f1) / 3

    /* Estimate the rounding error for r4 */
    // double e4 = 2 * 20.67 * (fabs (f4) + fabs (f3) + fabs (f2) + fabs (f1)) * GSL_DBL_EPSILON;
    arb_set    (&e4, &f4);                //                   f4
    arb_abs    (&e4, &e4);                //              abs (f4)
    arb_set    (&T,  &f3);                //                              f3
    arb_abs    (&T,  &T);                 //                         abs (f3)
    arb_add    (&e4, &e4, &T, prec);      //              abs (f4) + abs (f3)
    arb_set    (&T,  &f2);                //                                         f2
    arb_abs    (&T,  &T);                 //                                    abs (f2)
    arb_add    (&e4, &e4, &T, prec);      //              abs (f4) + abs (f3) + abs (f2)
    arb_set    (&T,  &f1);                //                                                    f1
    arb_abs    (&T,  &T);                 //                                               abs (f1)
    arb_add    (&e4, &e4, &T, prec);      //              abs (f4) + abs (f3) + abs (f2) + abs (f1)
    arb_mul_si (&e4, &e4, 2, prec);       // 2 *         (abs (f4) + abs (f3) + abs (f2) + abs (f1))
    arb_set_d  (&T,  20.67);              //     20.67
    arb_mul    (&e4, &e4, &T, prec);      // 2 * 20.67 * (abs (f4) + abs (f3) + abs (f2) + abs (f1))
    arb_mul    (&e4, &e4, &dbleps, prec); // 2 * 20.67 * (abs (f4) + abs (f3) + abs (f2) + abs (f1)) * GSL_DBL_EPSILON

    /* The next term is due to finite precision in x+h = O (eps * x) */
    // double dy = GSL_MAX (fabs (r2 / h), fabs (r4 / h)) * fabs (x / h) * GSL_DBL_EPSILON;
    arb_set   (&dy, &r2);                 //           r2
    arb_div   (&dy, &dy, h, prec);        //           r2 / h
    arb_abs   (&dy, &dy);                 //      abs (r2 / h)
    arb_set   (&T,  &r4);                 //                         r4
    arb_div   (&T,  &T,  h, prec);        //                         r4 / h
    arb_abs   (&T,  &T);                  //                    abs (r4 / h)
    arb_max   (&dy, &dy, &T, prec);       // max (abs (r2 / h), abs (r4 / h))
    arb_set   (&T,   x);                  //                                          x
    arb_abs   (&T, &T);                   //                                     abs (x)
    arb_div   (&T, &T, h, prec);          //                                     abs (x) / h
    arb_mul   (&dy, &dy, &T, prec);       // max (abs (r2 / h), abs (r4 / h)) * (abs (x) / h)
    arb_mul   (&dy, &dy, &dbleps, prec);  // max (abs (r3 / h), abs (r4 / h)) * (abs (x) / h) * GSL_DBL_EPSILON

    /* The truncation error in the r4 approximation itself is O(h^3).
       However, for safety, we estimate the error from r4-r2, which is
       O(h).  By scaling h we will minimise this estimated error, not
       the actual truncation error in r4. */

    // *result = r4 / h;
    arb_set   (&T, &r4);                      //           r4
    arb_div   (result, &T, h, prec);          // *result = r4

    // *abserr_trunc = fabs ((r4 - r2) / h); /* Estimated truncation error O(h) */
    arb_set   (&T, &r4);                      //       r4
    arb_sub   (&T, &T, &r2, prec);            //       r4 - r2
    arb_div   (&T, &T, h, prec);              //      (r4 - r2) / h
    arb_abs   (abserr_trunc, &T);             // abs ((r4 - r2) / h)

    // *abserr_round = fabs (e4 / h) + dy;
    arb_set   (&T, &e4);                      //      e4
    arb_div   (&T, &T, h, prec);              //      e4 / h
    arb_abs   (&T, &T);                       // abs (e4 / h)
    arb_add   (abserr_round, &T, &dy, prec);  // abs (e4 / h) + dy

    Myarb_clear (&dy);
    Myarb_clear (&e4);
    Myarb_clear (&r4);
    Myarb_clear (&r2);
    Myarb_clear (&f4);
    Myarb_clear (&f3);
    Myarb_clear (&f2);
    Myarb_clear (&f1);
    Myarb_clear (&T);
    Myarb_clear (&dbleps);
} // End arb_deriv_forward_calc


//**************************************************************************
//  $arb_deriv_central_calc
//
//  Calculate the finite differences
//
//  This routine was translated into ARB from the original GSL routine
//    <central_deriv> in <gsl\deriv\deriv.c>.
//***************************************************************************

void arb_deriv_central_calc
    (const gsl_function_arb *f,
     LPAPLARB                x,
     LPAPLARB                h,
     LPAPLARB                result,
     LPAPLARB                abserr_round,
     LPAPLARB                abserr_trunc)

{
    mp_limb_signed_t prec = ARB_PREC_FPC;   // ARB precision

    APLARB dbleps = {0},    // Set to 2*(1 - []FPC)
           con2 = {0};

    arb_init_set_sx (&con2,   2);
    arb_init_set_sx (&dbleps, (long int) (1 - GetQuadFPC ()));        // 1 - []FPC
    arb_pow         (&dbleps, &con2, &dbleps, prec);                  // pow (2, 1 - []FPC)

  /* Compute the derivative using the 5-point rule (x-h, x-h/2, x,
     x+h/2, x+h). Note that the central point is not used.

     Compute the error using the difference between the 5-point and
     the 3-point rule (x-h,x,x+h). Again the central point is not
     used. */

    APLARB T = {0};
    arb_init   (&T);

    arb_set    (&T, h);                   //         h
    arb_sub    (&T, x, &T, prec);         //     x - h
    APLARB fm1 = GSL_FN_ARB_EVAL (f, &T);       // (f, x - h);

    arb_set    (&T, h);                   //         h
    arb_add    (&T, x, &T, prec);         //     x + h
    APLARB fp1 = GSL_FN_ARB_EVAL (f, &T);       // (f, x + h);

    arb_set    (&T, h);                   //         h
    arb_div_si (&T, &T, 2, prec);         //         h / 2
    arb_sub    (&T, x, &T, prec);         //     x - h / 2
    APLARB fmh = GSL_FN_ARB_EVAL (f, &T);       // (f, x - h / 2);

    arb_set    (&T, h);                   //         h
    arb_div_si (&T, &T, 2, prec);         //         h / 2
    arb_add    (&T, x, &T, prec);         //     x + h / 2
    APLARB fph = GSL_FN_ARB_EVAL (f, &T);       // (f, x + h / 2);

    APLARB r3 = {0};        // r3 = 0.5 * (fp1 - fm1);
    arb_init   (&r3);
    arb_set    (&r3, &fp1);               //  fp1
    arb_sub    (&r3, &r3, &fm1, prec);    //  fp1 - fm1
    arb_div_si (&r3, &r3, 2, prec);       // (fp1 - fm1) / 2

    APLARB r5 = {0};        // r5 = (4.0 / 3.0) * (fph - fmh) - (1.0 / 3.0) * r3;
    arb_init   (&r5);
    arb_set    (&r5, &fph);               //      fph
    arb_sub    (&r5, &r5, &fmh, prec);    //      fph - fmh
    arb_mul_si (&r5, &r5, 4, prec);       // 4 * (fph -fmh)
    arb_div_si (&r5, &r5, 3, prec);       // 4 * (fph - fmh) / 3
    arb_set    (&T, &r3);                 //                              r3
////arb_mul_si (&T, &T, 1, prec);         //                          1 * r3
    arb_div_si (&T, &T, 3, prec);         //                          1 * r3 / 3
    arb_sub    (&r5, &r5, &T, prec);      // (4 * (fph - fmh) / 3) - (1 * r3 / 3)

    APLARB e3 = {0};        // e3 = (fabs (fp1) + fabs (fm1)) * GSL_DBL_EPSILON;
    arb_init  (&e3);
    arb_set   (&e3, &fp1);                //      fp1
    arb_abs   (&e3, &e3);                 // abs (fp1)
    arb_set   (&T, &fm1);                 //                  fm1
    arb_abs   (&T, &T);                   //             abs (fm1)
    arb_add   (&e3, &e3, &T, prec);       // abs (fp1) + abs (fm1)
#ifdef DEBUG
////VfpOut     (L"abs (fp1) + abs (fm1) = ", &e3);
#endif
    arb_mul   (&e3, &e3, &dbleps, prec);  // abs (fp1) + abs (fm1) * GSL_DBL_EPSILON
#ifdef DEBUG
////VfpOut     (L"abs (fp1) + abs (fm1) * dbleps =", &e3);
#endif

    APLARB e5 = {0};        // e5 = 2.0 * (fabs (fph) + fabs (fmh)) * GSL_DBL_EPSILON + e3;
    arb_init  (&e5);
    arb_set   (&e5, &fph);                //            fph
    arb_abs   (&e5, &e5);                 //       abs (fph)
    arb_set   (&T, &fmh);                 //                        fmh
    arb_abs   (&T, &T);                   //                   abs (fmh)
    arb_add   (&e5, &e5, &T, prec);       //       abs (fph) + abs (fmh)
#ifdef DEBUG
////VfpOut     (L"abs (fph) + abs (fmh) = ", &e5);
#endif
    arb_mul   (&e5, &e5, &dbleps, prec);  //       abs (fph) + abs (fmh) * GSL_DBL_EPSILON
#ifdef DEBUG
////VfpOut     (L"abs (fph) + abs (fmh) * dbleps = ", &e5);
#endif
    arb_mul_si(&e5, &e5, 2, prec);        // 2.0 * abs (fph) + abs (fmh) * GSL_DBL_EPSILON
#ifdef DEBUG
////VfpOut     (L"2 * abs (fph) + abs (fmh) * dbleps = ", &e5);
#endif
    arb_add   (&e5, &e5, &e3, prec);      // 2.0 * abs (fph) + abs (fmh) * GSL_DBL_EPSILON + e3
#ifdef DEBUG
////VfpOut     (L"2 * abs (fph) + abs (fmh) * dbleps + e3 = ", &e5);
#endif

    // The next term is due to finite precision in x+h = O (eps * x)
    APLARB dy = {0};        // dy = GSL_MAX (fabs (r3 / h), fabs (r5 / h)) *(fabs (x) / h) * GSL_DBL_EPSILON;
    arb_init  (&dy);
    arb_set   (&dy, &r3);                 //           r3
    arb_div   (&dy, &dy, h, prec);        //           r3 / h
    arb_abs   (&dy, &dy);                 //      abs (r3 / h)
    arb_set   (&T, &r5);                  //                         r5
    arb_div   (&T, &T, h, prec);          //                         r5 / h
    arb_abs   (&T, &T);                   //                    abs (r5 / h)
    arb_max   (&dy, &dy, &T, prec);       // max (abs (r3 / h), abs (r5 / h))
    arb_set   (&T, x);                    //                                           x
    arb_abs   (&T, &T);                   //                                     fabs (x)
    arb_div   (&T, &T, h, prec);          //                                     fabs (x) / h
    arb_mul   (&dy, &dy, &T, prec);       // max (abs (r3 / h), abs (r5 / h)) * (fabs (x) / h)
    arb_mul   (&dy, &dy, &dbleps, prec);  // max (abs (r3 / h), abs (r5 / h)) * (fabs (x) / h) * GSL_DBL_EPSILON

    /* The truncation error in the r5 approximation itself is O(h^4).
       However, for safety, we estimate the error from r5-r3, which is
       O(h^2).  By scaling h we will minimise this estimated error, not
       the actual truncation error in r5. */

    // *result = r5 / h;
    arb_set   (&T, &r5);                      //           r5
    arb_div   (result, &T, h, prec);          // *result = r5

    // *abserr_trunc = fabs ((r5 - r3) / h); /* Estimated truncation error O(h^2) */
    arb_set   (&T, &r5);                      //       r5
    arb_sub   (&T, &T, &r3, prec);            //       r5 - r3
    arb_div   (&T, &T, h, prec);              //      (r5 - r3) / h
    arb_abs   (abserr_trunc, &T);             // abs ((r5 - r3) / h)

    // *abserr_round = fabs (e5 / h) + dy;   /* Rounding error (cancellations) */
    arb_set   (&T, &e5);                      //      e5
    arb_div   (&T, &T, h, prec);              //      e5 / h
    arb_abs   (&T, &T);                       // abs (e5 / h)
    arb_add   (abserr_round, &T, &dy, prec);  // abs (e5 / h) + dy

    Myarb_clear (&dy);
    Myarb_clear (&e5);
    Myarb_clear (&e3);
    Myarb_clear (&r5);
    Myarb_clear (&r3);
    Myarb_clear (&fph);
    Myarb_clear (&fmh);
    Myarb_clear (&fp1);
    Myarb_clear (&fm1);
    Myarb_clear (&T);
    Myarb_clear (&dbleps);
} // End arb_deriv_central_calc


//***************************************************************************
//  $arb_deriv_central2
//
//  Compute the first derivative of the function f
//
//  This routine was translated into ARB from the original GSL routine
//    <gsl_deriv_central> in <gsl\deriv\deriv.c>.
//***************************************************************************

int arb_deriv_central2
    (const gsl_function_arb *f,
     LPAPLARB                x,
     LPAPLARB                h,
     LPAPLARB                result,
     LPAPLARB                abserr)

{
    return
      arb_deriv_common (f,
                        x,
                        h,
                        result,
                        abserr,
                       &arb_deriv_central_calc);
} // End arb_deriv_central2


//***************************************************************************
//  $arb_deriv_forward
//
//  Compute the first derivative of the function f
//
//  This routine was translated into ARB from the original GSL routine
//    <gsl_deriv_forward> in <gsl\deriv\deriv.c>.
//***************************************************************************

int arb_deriv_forward
    (const gsl_function_arb *f,
     LPAPLARB                x,
     LPAPLARB                h,
     LPAPLARB                result,
     LPAPLARB                abserr)

{
    return
      arb_deriv_common (f,
                        x,
                        h,
                        result,
                        abserr,
                       &arb_deriv_forward_calc);
} // End arb_deriv_forward


//***************************************************************************
//  $arb_deriv_backward
//
//  Compute the first derivative of the function f
//
//  This routine was translated into ARB from the original GSL routine
//    <gsl_deriv_backward> in <gsl\deriv\deriv.c>.
//***************************************************************************

int arb_deriv_backward
    (const gsl_function_arb *f,
     LPAPLARB                x,
     LPAPLARB                h,
     LPAPLARB                result,
     LPAPLARB                abserr)

{
    APLARB T = {0};
    int    iRet;

    arb_init (&T);
    arb_set  (&T,  h);
    arb_neg  (&T, &T);

    iRet =
      arb_deriv_common (f,
                        x,
                       &T,
                        result,
                        abserr,
                       &arb_deriv_forward_calc);
    arb_clear (&T);

    return iRet;
} // End arb_deriv_backward


//***************************************************************************
//  $arb_deriv_common
//
//  Compute the first derivative of the function f
//
//  This routine was translated into ARB from the original GSL routine
//    <gsl_deriv_XXXXXXX> in <gsl\deriv\deriv.c>.
//***************************************************************************

int arb_deriv_common
    (const gsl_function_arb *f,
     LPAPLARB                x,
     LPAPLARB                h,
     LPAPLARB                result,
     LPAPLARB                abserr,
     void                   *arb_deriv_calc)

{
    mp_limb_signed_t prec = ARB_PREC_FPC;   // ARB precision
    APLARB r_0 = {0},
           round = {0},
           trunc = {0},
           error = {0};

    arb_init  (&r_0);
    arb_init  (&round);
    arb_init  (&trunc);
    arb_init  (&error);

    // Calculate the finite differences
    (*(ARB_DERIV_CALC) arb_deriv_calc) (f, x, h, &r_0, &round, &trunc);

    // error = round + trunc;
    arb_set (&error, &round);
    arb_add (&error, &error, &trunc, prec);

    // (round < trunc && (round > 0 && trunc > 0))
    if (arb_lt (&round, &trunc) && arf_sgn (arb_midref (&round)) > 0 && arf_sgn (arb_midref (&trunc)) > 0)
    {
        APLARB r_opt     = {0},
               round_opt = {0},
               trunc_opt = {0},
               error_opt = {0},
               T1        = {0},
               T2        = {0};
        arb_init  (&r_opt);
        arb_init  (&round_opt);
        arb_init  (&trunc_opt);
        arb_init  (&error_opt);
        arb_init  (&T1);
        arb_init  (&T2);

        /* Compute an optimised stepsize to minimize the total error,
           using the scaling of the truncation error (O(h^2)) and
           rounding error (O(1/h)). */

        APLARB h_opt = {0};     // h_opt = h * pow (round / (2.0 * trunc), 1.0 / 3.0);
        arb_init   (&h_opt);
        arb_set    (&h_opt, &trunc);                  //                                   trunc
        arb_mul_si (&h_opt, &h_opt, 2, prec);         //                             2.0 * trunc
        arb_div    (&h_opt, &round, &h_opt, prec);    //                    round / (2.0 * trunc)
        arb_root_ui(&h_opt, &h_opt, 3, prec);         //             cbrt ( round / (2.0 * trunc))
        arb_mul    (&h_opt, &h_opt, h, prec);         // h_opt = h * cbrt ( round / (2.0 * trunc))

        // Calculate the finite differences
        (*(ARB_DERIV_CALC) arb_deriv_calc) (f, x, &h_opt, &r_opt, &round_opt, &trunc_opt);

        // error_opt = round_opt + trunc_opt;
        arb_set (&error_opt, &round_opt);
        arb_add (&error_opt, &error_opt, &trunc_opt, prec);

        /* Check that the new error is smaller, and that the new derivative
           is consistent with the error bounds of the original estimate. */
        // (error_opt < error && fabs (r_opt - r_0) < 4.0 * error)
        arb_set    (&T1, &r_opt);                     //           r_opt
        arb_sub    (&T1, &T1, &r_0, prec);            //           r_opt - r_0
        arb_abs    (&T1, &T1);                        // T1 = abs (r_opt - r_0)
        arb_set    (&T2, &error);                     //            error
        arb_mul_si (&T2, &T2, 4, prec);               // T2 = 4.0 * error

        if (arb_lt (&error_opt, &error) && arb_lt (&T1, &T2))
        {
            arb_set (&r_0, &r_opt);                   // r_0   = r_opt
            arb_set (&error, &error_opt);             // error = error_opt
        } // End IF

        Myarb_clear (&T2);
        Myarb_clear (&T1);
        Myarb_clear (&h_opt);
        Myarb_clear (&error_opt);
        Myarb_clear (&trunc_opt);
        Myarb_clear (&round_opt);
        Myarb_clear (&r_opt);
    } // End IF

    arb_set (result, &r_0);
    arb_set (abserr, &error);

    Myarb_clear (&error);
    Myarb_clear (&trunc);
    Myarb_clear (&round);
    Myarb_clear (&r_0);

    return GSL_SUCCESS;
} // End arb_deriv_common


//***************************************************************************
//  End of File: po_deriv_arb.c
//***************************************************************************
