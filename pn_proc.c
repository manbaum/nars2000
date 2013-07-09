//***************************************************************************
//  NARS2000 -- Parser Grammar Functions XXX Point Notation
//***************************************************************************

//***************************************************************************
//  Parse a line of XXX Point Notation characters.
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2013 Sudley Place Software

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
#define _USE_MATH_DEFINES
#include <math.h>
#include "headers.h"


//***************************************************************************
//  $PN_NumAcc
//
//  Accumulate another digit into the number
//***************************************************************************

void PN_NumAcc
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     char          chCur)               // The next digit

{
    // Save the digit
    lppnLocalVars->lpszNumAccum[lppnLocalVars->uNumAcc++] = chCur;

    // Ensure properly terminated
    lppnLocalVars->lpszNumAccum[lppnLocalVars->uNumAcc  ] = '\0';
} // End PN_NumAcc


//***************************************************************************
//  $PN_NumCalc
//
//  Calculate the value of a number
//***************************************************************************

void PN_NumCalc
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     LPPN_YYSTYPE  lpYYArg,             // Ptr to incoming YYSTYPE
     UBOOL         bUseRat)             // TRUE iff this routine may blowup to Rational

{
    UBOOL  bRet = TRUE,                 // TRUE iff the result is valid
           bSigned;                     // TRUE iff the number is negative
    APLINT aplInteger;                  // Temporary integer
    UINT   uLen,                        // Loop length
           uOff,                        // Starting offset
           uAcc;                        // Loop counter

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return;

    // Initialize the starting offset
    uOff = lpYYArg->uNumOff;

    // Split cases based upon the current numeric type
    switch (lpYYArg->chType)
    {
        case PN_NUMTYPE_INT:
            // If we're forming the numerator of a Rational number, ...
            if (bUseRat)
            {
                // Convert the numerator to a GMP integer
                mpz_init_set_str (mpq_numref (&lpYYArg->at.aplRat), &lppnLocalVars->lpszNumAccum[uOff], 10);

                break;
            } else
            {
                // Initialize the accumulator
                aplInteger = 0;

                // Initialize the loop length
                uLen = lstrlen (&lppnLocalVars->lpszNumAccum[uOff]);

                // Handle negative sign
                uAcc = bSigned = (lppnLocalVars->lpszNumAccum[uOff] EQ OVERBAR1);

                // If the number is negative, ...
                if (bSigned)
                    // Loop through the digits
                    for (; bRet && uAcc < uLen; uAcc++)
                    {
                        aplInteger = _imul64 (aplInteger, 10, &bRet);
                        if (bRet)
                            aplInteger = _isub64 (aplInteger, lppnLocalVars->lpszNumAccum[uOff + uAcc] - '0', &bRet);
                    } // End FOR
                else
                    // Loop through the digits
                    for (; bRet && uAcc < uLen; uAcc++)
                    {
                        aplInteger = _imul64 (aplInteger, 10, &bRet);
                        if (bRet)
                            aplInteger = _iadd64 (aplInteger, lppnLocalVars->lpszNumAccum[uOff + uAcc] - '0', &bRet);
                    } // End FOR
                if (bRet)
                {
                    // Save in the result
                    lpYYArg->at.aplInteger = aplInteger;

                    if (IsBooleanValue (aplInteger))
                        lpYYArg->chType = PN_NUMTYPE_BOOL;
                    break;
                } // End IF
            } // End IF

            // Change the type to float
            lpYYArg->chType = PN_NUMTYPE_FLT;

            // Fall through to the float case

        case PN_NUMTYPE_FLT:
            // Use David Gay's routines
            lpYYArg->at.aplFloat = strtod (&lppnLocalVars->lpszNumAccum[uOff], NULL);

            break;

        case PN_NUMTYPE_RAT:
            // This type is used after the numerator has been calculated
            //   and is in mpq_numref (&lpYYArg->at.aplRat).  The denominator is
            //   in lppnLocalVars->NumAccum[uOff] and must be converted.

            // Convert the denominator to a GMP integer
            mpz_init_set_str (mpq_denref (&lpYYArg->at.aplRat), &lppnLocalVars->lpszNumAccum[uOff], 10);

            // If the denominator is zero, ...
            if (IsMpz0 (mpq_denref (&lpYYArg->at.aplRat)))
            {
                int iSgn;

                // Save the sign of the numerator
                iSgn = mpz_sgn (mpq_numref (&lpYYArg->at.aplRat));

                // We no longer need this storage
                Myq_clear (&lpYYArg->at.aplRat);

                // Set to the appropriate-signed infinity
                mpq_set_infsub (&lpYYArg->at.aplRat, iSgn);
            } else
                // Canonicalize the arg
                mpq_canonicalize (&lpYYArg->at.aplRat);
            break;

        case PN_NUMTYPE_HC2:
        case PN_NUMTYPE_HC4:
        case PN_NUMTYPE_HC8:
            break;

        defstop
            break;
    } // End SWITCH
} // End PN_NumCalc


//***************************************************************************
//  $PN_ChrAcc
//
//  Append a new char to the internal buffer
//***************************************************************************

void PN_ChrAcc
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     char          chCur)               // The next char

{
    // Accumulate this char
    lppnLocalVars->lpszAlphaInt[lppnLocalVars->uAlpAcc++] = chCur;
} // End PN_ChrAcc


//***************************************************************************
//  $PN_MakeBasePoint
//
//  Merge the base and alphaint part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeBasePoint
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     LPPN_YYSTYPE  lpYYBase,            // The base part
     LPPN_YYSTYPE  lpYYAlphaInt)        // The AlphaInt part

{
    UINT     uLen,                      // Length of lpszAlphaInt
             uAcc;                      // Loop counter
    LPCHAR   lpszAlphaInt;              // Ptr to next char in lpszAlphaInt
    APLINT   aplIntBase,                // Base as an integer
             aplIntPowBase,             // Base as successive powers
             aplIntTmp,                 // Temporary integer
             aplIntAcc;                 // Temporary accumulator
    APLFLOAT aplFltBase,                // Base as a  float
             aplFltPowBase,             // Base as successive powers
             aplFltAcc,                 // Temporary accumulator
             aplAccHC2Real,             // Accumulator real part
             aplAccHC2Imag,             // ...         imag ...
             aplBaseHC2Real,            // Base real part
             aplBaseHC2Imag,            // ...  imag ...
             aplPowBaseHC2Real,         // Base as successive powers real part
             aplPowBaseHC2Imag;         // ...                       imag ...
    UBOOL    bRet = TRUE;               // TRUE iff the result is valid
    UCHAR    chCur;                     // Temporary character

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // Get the string length
    uLen = lppnLocalVars->uAlpAcc;

    // Get the ptr to the end of the AlphaInt string
    lpszAlphaInt = &lppnLocalVars->lpszAlphaInt[uLen - 1];

    // Split cases based upon the numeric type of the base
    switch (lpYYBase->chType)
    {
        case PN_NUMTYPE_BOOL:
        case PN_NUMTYPE_INT:
            // Get the base value
            aplIntBase = lpYYBase->at.aplInteger;

            // Initialize the accumulator and base power
            aplIntAcc = 0;
            aplIntPowBase = 1;

            // Loop through the AlphaInt arg
            for (uAcc = 0; bRet && uAcc < uLen; uAcc++)
            {
                // Get the next char as lowercase
                chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaInt--));

                // Convert to a number
                if ('0' <= chCur
                 &&        chCur <= '9')
                    chCur -= '0';
                else
                if ('a' <= chCur
                 &&        chCur <= 'z')
                    chCur -= 'a' - 10;

                // Times the power base
                aplIntTmp = _imul64 (chCur, aplIntPowBase, &bRet);
                if (!bRet)
                    break;

                // Accumulate
                aplIntAcc = _iadd64 (aplIntAcc, aplIntTmp, &bRet);
                if (!bRet)
                    break;

                // Shift over the power base
                aplIntPowBase = _imul64 (aplIntPowBase, aplIntBase, &bRet);

                // Ignore spurious overflow on last digit
                bRet |= (uAcc EQ (uLen - 1));

////////////////if (!bRet)          // Not needed as we're at the end of the FOR loop
////////////////    break;
            } // End FOR

            if (bRet)
            {
                // Save the result
                lpYYBase->at.aplInteger = aplIntAcc;

                break;
            } // End IF

            // Convert the base value to Rational
            mpq_init_set_sx (&lpYYBase->at.aplRat, lpYYBase->at.aplInteger, 1);

            // Get the ptr to the end of the AlphaInt string
            lpszAlphaInt = &lppnLocalVars->lpszAlphaInt[uLen - 1];

            // Change the type to RAT
            lpYYBase->chType = PN_NUMTYPE_RAT;

            // Fall through to the rational case

        case PN_NUMTYPE_RAT:
        {
            APLRAT aplRatBase    = {0},
                   aplRatPowBase = {0},
                   aplRatTmp     = {0};

            // Initialize and set the base value
            mpq_init_set (&aplRatBase, &lpYYBase->at.aplRat);

            // Initialize the accumulator and base power
            mpq_set_ui (&lpYYBase->at.aplRat, 0, 1);
            mpq_init_set_ui (&aplRatPowBase, 1, 1);
            mpq_init (&aplRatTmp);

            // Loop through the AlphaInt arg
            for (uAcc = 0; uAcc < uLen; uAcc++)
            {
                // Get the next char as lowercase
                chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaInt--));

                // Convert to a number
                if ('0' <= chCur
                 &&        chCur <= '9')
                    chCur -= '0';
                else
                if ('a' <= chCur
                 &&        chCur <= 'z')
                    chCur -= 'a' - 10;

                // Times the power base and accumulate
                mpq_set_ui (&aplRatTmp, chCur, 1);
                mpq_mul (&aplRatTmp, &aplRatPowBase, &aplRatTmp);
                mpq_add (&lpYYBase->at.aplRat, &lpYYBase->at.aplRat, &aplRatTmp);
                mpq_canonicalize (&lpYYBase->at.aplRat);

                // Shift over the power base
                mpq_mul (&aplRatPowBase, &aplRatPowBase, &aplRatBase);
            } // End FOR

            // We no longer need this storage
            Myq_clear (&aplRatTmp);
            Myq_clear (&aplRatPowBase);
            Myq_clear (&aplRatBase);

            break;
        } // End PN_NUMTYPE_RAT

        case PN_NUMTYPE_VFP:
        {
            APLVFP aplVfpBase    = {0},
                   aplVfpPowBase = {0},
                   aplVfpTmp     = {0};

            // Initialize and set the base value
            mpfr_init_copy (&aplVfpBase, &lpYYBase->at.aplVfp);

            // Initialize the accumulator and base power
            mpfr_set_ui (&lpYYBase->at.aplVfp, 0, MPFR_RNDN);
            mpfr_init_set_ui (&aplVfpPowBase, 1, MPFR_RNDN);
            mpfr_init0 (&aplVfpTmp);

            // Loop through the AlphaInt arg
            for (uAcc = 0; uAcc < uLen; uAcc++)
            {
                // Get the next char as lowercase
                chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaInt--));

                // Convert to a number
                if ('0' <= chCur
                 &&        chCur <= '9')
                    chCur -= '0';
                else
                if ('a' <= chCur
                 &&        chCur <= 'z')
                    chCur -= 'a' - 10;

                // Times the power base and accumulate
                mpfr_set_ui (&aplVfpTmp, chCur, MPFR_RNDN);
                mpfr_mul (&aplVfpTmp, &aplVfpPowBase, &aplVfpTmp, MPFR_RNDN);
                mpfr_add (&lpYYBase->at.aplVfp, &lpYYBase->at.aplVfp, &aplVfpTmp, MPFR_RNDN);

                // Shift over the power base
                mpfr_mul (&aplVfpPowBase, &aplVfpPowBase, &aplVfpBase, MPFR_RNDN);
            } // End FOR

            // We no longer need this storage
            Myf_clear (&aplVfpTmp);
            Myf_clear (&aplVfpPowBase);
            Myf_clear (&aplVfpBase);

            break;
        } // End PN_NUMTYPE_VFP

        case PN_NUMTYPE_FLT:
            // Get the base value
            aplFltBase = lpYYBase->at.aplFloat;

            // Initialize the accumulator and base power
            aplFltAcc = 0;
            aplFltPowBase = 1;

            // Loop through the AlphaInt arg
            for (uAcc = 0; uAcc < uLen; uAcc++)
            {
                // Get the next char as lowercase
                chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaInt--));

                // Convert to a number
                if ('0' <= chCur
                 &&        chCur <= '9')
                    chCur -= '0';
                else
                if ('a' <= chCur
                 &&        chCur <= 'z')
                    chCur -= 'a' - 10;

                // Times the power base and accumulate
                aplFltAcc += chCur * aplFltPowBase;

                // Shift over the power base
                aplFltPowBase *= aplFltBase;
            } // End FOR

            // Save the result
            lpYYBase->at.aplFloat = aplFltAcc;

            break;

        case PN_NUMTYPE_HC2:
            // Get the base value
            aplBaseHC2Real = lpYYBase->at.aplHC2.Real;
            aplBaseHC2Imag = lpYYBase->at.aplHC2.Imag;

            // Initialize the accumulator and base power
            aplAccHC2Real =
            aplAccHC2Imag = 0;
            aplPowBaseHC2Real = 1;
            aplPowBaseHC2Imag = 0;

            // Loop through the AlphaInt arg
            for (uAcc = 0; uAcc < uLen; uAcc++)
            {
                // Get the next char as lowercase
                chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaInt--));

                // Convert to a number
                if ('0' <= chCur
                 &&        chCur <= '9')
                    chCur -= '0';
                else
                if ('a' <= chCur
                 &&        chCur <= 'z')
                    chCur -= 'a' - 10;

                // Times the power base and accumulate
                aplAccHC2Real += chCur * aplPowBaseHC2Real;
                aplAccHC2Imag += chCur * aplPowBaseHC2Imag;

                // Shift over the power base
                aplPowBaseHC2Real = aplBaseHC2Real * aplPowBaseHC2Real - aplBaseHC2Imag * aplPowBaseHC2Imag;
                aplPowBaseHC2Imag = aplBaseHC2Imag * aplPowBaseHC2Real + aplBaseHC2Real * aplPowBaseHC2Imag;
            } // End FOR

            // Save the result
            lpYYBase->at.aplHC2.Real = aplAccHC2Real;
            lpYYBase->at.aplHC2.Imag = aplAccHC2Imag;

            break;

        case PN_NUMTYPE_HC4:
        case PN_NUMTYPE_HC8:
            // ***FINISHME***

            break;

        defstop
            break;
    } // End SWITCH

    return lpYYBase;
} // End PN_MakeBasePoint


//***************************************************************************
//  $PN_MakeEulerPoint
//
//  Merge the multiplier and exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeEulerPoint
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     LPPN_YYSTYPE  lpYYMultiplier,      // The multiplier part
     LPPN_YYSTYPE  lpYYExponent)        // The exponent part

{
    PNNUMTYPE pnTypeRes;

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // Promote the multiplier and exponent to a common format
    pnTypeRes = aNumTypePromote[lpYYMultiplier->chType][lpYYExponent->chType];

    // If the result is BOOL/INT, ...
    if (pnTypeRes EQ PN_NUMTYPE_BOOL
     || pnTypeRes EQ PN_NUMTYPE_INT)
        // Make it FLT
        pnTypeRes = PN_NUMTYPE_FLT;

    // If the result is RAT, ...
    if (pnTypeRes EQ PN_NUMTYPE_RAT)
        // Make it VFP
        pnTypeRes = PN_NUMTYPE_VFP;

    // If the multiplier must be promoted, ...
    if (pnTypeRes NE lpYYMultiplier->chType
     && aNumTypeAction[lpYYMultiplier->chType][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYMultiplier->chType][pnTypeRes]) (lpYYMultiplier);

    // If the exponent must be promoted, ...
    if (pnTypeRes NE lpYYExponent->chType
     && aNumTypeAction[lpYYExponent->chType  ][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYExponent->chType  ][pnTypeRes]) (lpYYExponent);

    // Set the result type
    lpYYMultiplier->chType =
    lpYYExponent->chType   = pnTypeRes;

    // Split cases based upon the numeric type of the result
    switch (pnTypeRes)
    {
        case PN_NUMTYPE_BOOL:
        case PN_NUMTYPE_INT:
            // The result is Multiplier x (*1) * Exponent
            lpYYMultiplier->at.aplFloat *= pow (FloatE, (APLFLOAT) lpYYExponent->at.aplInteger);

            break;

        case PN_NUMTYPE_FLT:
            // The result is Multiplier x (*1) * Exponent
            lpYYMultiplier->at.aplFloat *= pow (FloatE, lpYYExponent->at.aplFloat);

            break;

        case PN_NUMTYPE_VFP:
        {
            APLVFP aplVfpTmp = {0};

            // Initialize the temp array
            mpfr_init0 (&aplVfpTmp);

            // The result is Multiplier x (*1) * Exponent
            mpfr_pow (&aplVfpTmp, &GetMemPTD ()->mpfrE, &lpYYExponent->at.aplVfp, MPFR_RNDN);

            // Accumulate in the multiplier
            mpfr_mul (&lpYYMultiplier->at.aplVfp, &lpYYMultiplier->at.aplVfp, &aplVfpTmp, MPFR_RNDN);

            // We no longer need this storage
            Myf_clear (&aplVfpTmp);
            Myf_clear (&lpYYExponent->at.aplVfp);

            break;
        } // End PN_NUMTYPE_VFP

        case PN_NUMTYPE_HC2:
        case PN_NUMTYPE_HC4:
        case PN_NUMTYPE_HC8:
            // ***FINISHME***

            break;

        defstop
            break;
    } // End SWITCH

    return lpYYMultiplier;
} // End PN_MakeEulerPoint


//***************************************************************************
//  $PN_MakeExpPoint
//
//  Merge the multiplier and exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeExpPoint
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     LPPN_YYSTYPE  lpYYArg,             // The mantissa part
     LPPN_YYSTYPE  lpYYExponent)        // The exponent part

{
    UINT uOff;                          // Starting offset

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // Terminate the (Exponent) argument
    PN_NumAcc (lppnLocalVars, '\0');

    // Initialize the starting offset
    uOff = lpYYArg->uNumOff;

    // Insert the exponent separator
    lppnLocalVars->lpszNumAccum[lpYYExponent->uNumOff - 1] = 'e';

    // Use David Gay's routines
    lpYYArg->at.aplFloat = strtod (&lppnLocalVars->lpszNumAccum[uOff], NULL);

    // Change the type to float
    lpYYArg->chType = PN_NUMTYPE_FLT;

    return lpYYArg;
} // End PN_MakeExpPoint


//***************************************************************************
//  $PN_MakePiPoint
//
//  Merge the multiplier and exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakePiPoint
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     LPPN_YYSTYPE  lpYYMultiplier,      // The multiplier part
     LPPN_YYSTYPE  lpYYExponent)        // The exponent part

{
    PNNUMTYPE pnTypeRes;

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // Promote the multiplier and exponent to a common format
    pnTypeRes = aNumTypePromote[lpYYMultiplier->chType][lpYYExponent->chType];

    // If the result is BOOL/INT, ...
    if (pnTypeRes EQ PN_NUMTYPE_BOOL
     || pnTypeRes EQ PN_NUMTYPE_INT)
        // Make it FLT
        pnTypeRes = PN_NUMTYPE_FLT;

    // If the result is RAT, ...
    if (pnTypeRes EQ PN_NUMTYPE_RAT)
        // Make it VFP
        pnTypeRes = PN_NUMTYPE_VFP;

    // If the multiplier must be promoted, ...
    if (pnTypeRes NE lpYYMultiplier->chType
     && aNumTypeAction[lpYYMultiplier->chType][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYMultiplier->chType][pnTypeRes]) (lpYYMultiplier);

    // If the exponent must be promoted, ...
    if (pnTypeRes NE lpYYExponent->chType
     && aNumTypeAction[lpYYExponent->chType  ][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYExponent->chType  ][pnTypeRes]) (lpYYExponent);

    // Set the result type
    lpYYMultiplier->chType =
    lpYYExponent->chType   = pnTypeRes;

    // Split cases based upon the numeric type
    switch (pnTypeRes)
    {
        case PN_NUMTYPE_BOOL:
        case PN_NUMTYPE_INT:
            // The result is Multiplier x (o1) * Exponent
            lpYYMultiplier->at.aplFloat *= pow (FloatPi, (APLFLOAT) lpYYExponent->at.aplInteger);

            break;

        case PN_NUMTYPE_FLT:
            // The result is Multiplier x (o1) * Exponent
            lpYYMultiplier->at.aplFloat *= pow (FloatPi, lpYYExponent->at.aplFloat);

            break;

        case PN_NUMTYPE_VFP:
        {
            APLVFP aplVfpTmp = {0};

            // Initialize the temp array
            mpfr_init0 (&aplVfpTmp);

            // The result is Multiplier x (o1) * Exponent
            mpfr_pow (&aplVfpTmp, &GetMemPTD ()->mpfrPi, &lpYYExponent->at.aplVfp, MPFR_RNDN);

            // Accumulate in the multiplier
            mpfr_mul (&lpYYMultiplier->at.aplVfp, &lpYYMultiplier->at.aplVfp, &aplVfpTmp, MPFR_RNDN);

            // We no longer need this storage
            Myf_clear (&aplVfpTmp);
            Myf_clear (&lpYYExponent->at.aplVfp);

            break;
        } // End PN_NUMTYPE_VFP

        case PN_NUMTYPE_HC2:
        case PN_NUMTYPE_HC4:
        case PN_NUMTYPE_HC8:
            // ***FINISHME***

            break;

        defstop
            break;
    } // End SWITCH

    return lpYYMultiplier;
} // End PN_MakePiPoint


//***************************************************************************
//  $PN_MakeVfpPoint
//
//  Merge the multiplier and exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeVfpPoint
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     LPPN_YYSTYPE  lpYYArg,             // The mantissa part
     LPPN_YYSTYPE  lpYYExponent)        // The exponent part (may be NULL)

{
    UINT      uOff,                     // Starting offset
              uDig,                     // # significant digits
              uArg,                     // Loop counter
              uLen;                     // # accumulated chars
    mp_prec_t uOldPrec,                 // Old precision
              uNewPrec;                 // # significant bits

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // Initialize the starting offset
    uOff = lpYYArg->uNumOff;

    // Get # accumulated chars
    uLen = lstrlen (&lppnLocalVars->lpszNumAccum[uOff]);

    // Calculate the # significant digits in the number
    for (uDig = uArg = 0; uArg < uLen; uArg++)
    if (isdigit (lppnLocalVars->lpszNumAccum[uOff + uArg]))
        uDig++;

    // Convert the # significant digits to # significant bits
    //   via the formula 1 + floor (log2 (10^N))
    //                 = 1 + floor (N x log2 (10))
    //   where log2 (10) = (ln (10)) / (ln (2))
    //                   = M_LN10 / M_LN2
    uNewPrec = 1 + (mp_prec_t) floor (uDig * M_LN10 / M_LN2);

    // If present, ...
    if (lpYYExponent)
        // Insert the exponent separator
        lppnLocalVars->lpszNumAccum[lpYYExponent->uNumOff - 1] = 'e';

    // Get and save the current precision
    uOldPrec = mpfr_get_default_prec ();

    // Set the default precision to the larger ...
    mpfr_set_default_prec (max (uNewPrec, uOldPrec));

    // Use MPFR routine
    mpfr_init_set_str (&lpYYArg->at.aplVfp, &lppnLocalVars->lpszNumAccum[uOff], 10, MPFR_RNDN);

    // Restore the default precision
    mpfr_set_default_prec (uOldPrec);

    // Change the type to VFP
    lpYYArg->chType = PN_NUMTYPE_VFP;

    return lpYYArg;
} // End PN_MakeVfpPoint


//***************************************************************************
//  End of File: pn_proc.c
//***************************************************************************
