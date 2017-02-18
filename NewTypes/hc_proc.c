//***************************************************************************
//  NARS2000 -- Hypercomplex Helper Functions
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2017 Sudley Place Software

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
//  $Myhcxr_clear
//***************************************************************************

void Myhcxr_clear
    (LPVOID mphcrVal,           // Ptr to item to clear
     int    iHCDimVal)          // HC Dimension of item (1, 2, 4, 8)

{
    // Split cases based upon the HC Dimension
    switch (iHCDimVal)
    {
        case 1:
            Myhc1r_clear (mphcrVal);

            break;

        case 2:
            Myhc2r_clear (mphcrVal);

            break;

        case 4:
            Myhc4r_clear (mphcrVal);

            break;

        case 8:
            Myhc8r_clear (mphcrVal);

            break;

        defstop
            break;
    } // End SWITCH
} // End Myhcxr_clear


//***************************************************************************
//  $Myhc2r_clear
//***************************************************************************

void Myhc2r_clear
    (LPAPLHC2R mphc2rVal)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    // If this part is valid, ...
    if (mphc2rVal->parts[i]._mp_num._mp_d NE NULL)
    {
        mpq_clear (&mphc2rVal->parts[i]);
        mphc2rVal->parts[i]._mp_num._mp_d =
        mphc2rVal->parts[i]._mp_den._mp_d = NULL;
    } // End FOR/IF
} // End Myhc2r_clear


//***************************************************************************
//  $Myhc4r_clear
//***************************************************************************

void Myhc4r_clear
    (LPAPLHC4R mphc4rVal)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    // If this part is valid, ...
    if (mphc4rVal->parts[i]._mp_num._mp_d NE NULL)
    {
        mpq_clear (&mphc4rVal->parts[i]);
        mphc4rVal->parts[i]._mp_num._mp_d =
        mphc4rVal->parts[i]._mp_den._mp_d = NULL;
    } // End FOR/IF
} // End Myhc4f_clear


//***************************************************************************
//  $Myhc8r_clear
//***************************************************************************

void Myhc8r_clear
    (LPAPLHC8R mphc8rVal)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    // If this part is valid, ...
    if (mphc8rVal->parts[i]._mp_num._mp_d NE NULL)
    {
        mpq_clear (&mphc8rVal->parts[i]);
        mphc8rVal->parts[i]._mp_num._mp_d =
        mphc8rVal->parts[i]._mp_den._mp_d = NULL;
    } // End FOR/IF
} // End Myhc8r_clear


//***************************************************************************
//  $Myhcxv_clear
//***************************************************************************

void Myhcxv_clear
    (LPVOID mphcvVal,           // Ptr to item to clear
     int    iHCDimVal)          // HC Dimension of item (1, 2, 4, 8)

{
    // Split cases based upon the HC Dimension
    switch (iHCDimVal)
    {
        case 1:
            Myhc1v_clear (mphcvVal);

            break;

        case 2:
            Myhc2v_clear (mphcvVal);

            break;

        case 4:
            Myhc4v_clear (mphcvVal);

            break;

        case 8:
            Myhc8v_clear (mphcvVal);

            break;

        defstop
            break;
    } // End SWITCH
} // End Myhcxv_clear


//***************************************************************************
//  $Myhc2v_clear
//***************************************************************************

void Myhc2v_clear
    (LPAPLHC2V mphc2vVal)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    // If this part is valid, ...
    if (mphc2vVal->parts[i]._mpfr_d NE NULL)
    {
        mpfr_clear (&mphc2vVal->parts[i]);
        mphc2vVal->parts[i]._mpfr_d = NULL;
    } // End FOR/IF
} // End Myhc2v_clear


//***************************************************************************
//  $Myhc4v_clear
//***************************************************************************

void Myhc4v_clear
    (LPAPLHC4V mphc4vVal)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    // If this part is valid, ...
    if (mphc4vVal->parts[i]._mpfr_d NE NULL)
    {
        mpfr_clear (&mphc4vVal->parts[i]);
        mphc4vVal->parts[i]._mpfr_d = NULL;
    } // End FOR/IF
} // End Myhc4v_clear


//***************************************************************************
//  $Myhc8v_clear
//***************************************************************************

void Myhc8v_clear
    (LPAPLHC8V mphc8vVal)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    // If this part is valid, ...
    if (mphc8vVal->parts[i]._mpfr_d NE NULL)
    {
        mpfr_clear (&mphc8vVal->parts[i]);
        mphc8vVal->parts[i]._mpfr_d = NULL;
    } // End FOR/IF
} // End Myhc8v_clear


//***************************************************************************
// $mphc1r_set
//***************************************************************************

void mphc1r_set
    (LPAPLHC1R lpaplHC1RRes,
     LPAPLHC1R lpaplHC1RRht)

{
    // Initialize to 0/1
    mpq_set (lpaplHC1RRes, lpaplHC1RRht);
} // End mphc1r_set


//***************************************************************************
// $mphc2r_set
//***************************************************************************

void mphc2r_set
    (LPAPLHC2R lpaplHC2RRes,
     LPAPLHC2R lpaplHC2RRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Initialize to 0/1
        mpq_set (&lpaplHC2RRes->parts[i], &lpaplHC2RRht->parts[i]);
} // End mphc2r_set


//***************************************************************************
// $mphc4r_set
//***************************************************************************

void mphc4r_set
    (LPAPLHC4R lpaplHC4RRes,
     LPAPLHC4R lpaplHC4RRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Initialize to 0/1
        mpq_set (&lpaplHC4RRes->parts[i], &lpaplHC4RRht->parts[i]);
} // End mphc4r_set


//***************************************************************************
// $mphc8r_set
//***************************************************************************

void mphc8r_set
    (LPAPLHC8R lpaplHC8RRes,
     LPAPLHC8R lpaplHC8RRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Initialize to 0/1
        mpq_set (&lpaplHC8RRes->parts[i], &lpaplHC8RRht->parts[i]);
} // End mphc8r_set


//***************************************************************************
//  $mphc1v_set
//***************************************************************************

void mphc1v_set
    (LPAPLHC1V lpaplHC1VRes,
     LPAPLHC1V lpaplHC1VRht)

{
    // Initialize to 0
    mpfr_set (lpaplHC1VRes, lpaplHC1VRht, MPFR_RNDN);
} // End mphc1v_set


//***************************************************************************
//  $mphc2v_set
//***************************************************************************

void mphc2v_set
    (LPAPLHC2V lpaplHC2VRes,
     LPAPLHC2V lpaplHC2VRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Initialize to 0
        mpfr_set (&lpaplHC2VRes->parts[i], &lpaplHC2VRht->parts[i], MPFR_RNDN);
} // End mphc2v_set


//***************************************************************************
//  $mphc4v_set
//***************************************************************************

void mphc4v_set
    (LPAPLHC4V lpaplHC4VRes,
     LPAPLHC4V lpaplHC4VRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Initialize to 0
        mpfr_set (&lpaplHC4VRes->parts[i], &lpaplHC4VRht->parts[i], MPFR_RNDN);
} // End mphc4v_set


//***************************************************************************
//  $mphc8v_set
//***************************************************************************

void mphc8v_set
    (LPAPLHC8V lpaplHC8VRes,
     LPAPLHC8V lpaplHC8VRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Initialize to 0
        mpfr_set (&lpaplHC8VRes->parts[i], &lpaplHC8VRht->parts[i], MPFR_RNDN);
} // End mphc8v_set


//***************************************************************************
//  $mphcxr_init_set
//***************************************************************************

void mphcxr_init_set
    (LPVOID mphcxrVal,          // Ptr to item to set
     LPVOID mphcxrSrc,          // Ptr to source item
     int    iHCDimVal)          // HC Dimension of item (1, 2, 4, 8)

{
    // Split cases based upon the HC Dimension
    switch (iHCDimVal)
    {
        case 1:
            mphc1r_init_set (mphcxrVal, mphcxrSrc);

            break;

        case 2:
            mphc2r_init_set (mphcxrVal, mphcxrSrc);

            break;

        case 4:
            mphc4r_init_set (mphcxrVal, mphcxrSrc);

            break;

        case 8:
            mphc8r_init_set (mphcxrVal, mphcxrSrc);

            break;

        defstop
            break;
    } // End SWITCH
} // End mphcxr_init_set


//***************************************************************************
// $mphc1r_init_set
//***************************************************************************

void mphc1r_init_set
    (LPAPLHC1R lpaplHC1RRes,
     LPAPLHC1R lpaplHC1RRht)

{
    // Initialize to 0/1
    mpq_init_set (lpaplHC1RRes, lpaplHC1RRht);
} // End mphc1r_init_set


//***************************************************************************
// $mphc2r_init_set
//***************************************************************************

void mphc2r_init_set
    (LPAPLHC2R lpaplHC2RRes,
     LPAPLHC2R lpaplHC2RRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Initialize to 0/1
        mpq_init_set (&lpaplHC2RRes->parts[i], &lpaplHC2RRht->parts[i]);
} // End mphc2r_init_set


//***************************************************************************
// $mphc1r_init_set_sx
//***************************************************************************

void mphc1r_init_set_sx
    (LPAPLHC1R lpaplHC1RRht,
     APLINT    aplIntNum,
     APLINT    aplIntDen)

{
    // Initialize the real part to Num/Den
    mpq_init_set_sx (lpaplHC1RRht, aplIntNum, aplIntDen);
} // End mphc1r_init_set_sx


//***************************************************************************
// $mphc2r_init_set_sx
//***************************************************************************

void mphc2r_init_set_sx
    (LPAPLHC2R lpaplHC2RRht,
     APLINT    aplIntNum,
     APLINT    aplIntDen)

{
    // Initialize the real part to Num/Den
    mpq_init_set_sx (&lpaplHC2RRht->parts[0], aplIntNum, aplIntDen);

    // Initialize the imaginary part to 0/1
    Myq_init        (&lpaplHC2RRht->parts[1]);
} // End mphc2r_init_set_sx


//***************************************************************************
// $mphc2r_init_set_d
//***************************************************************************

void mphc2r_init_set_d
    (LPAPLHC2R lpaplHC2RRht,
     APLFLOAT  aplFloat)

{
    int i;

    // Initialize the real part to FLT
    mpq_init_set_d (&lpaplHC2RRht->parts[0], aplFloat);

    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
        // Initialize to 0/1
        Myq_init (&lpaplHC2RRht->parts[i]);
} // End mphc2r_init_set_d


//***************************************************************************
//  $mphc4r_init_set
//***************************************************************************

void mphc4r_init_set
    (LPAPLHC4R lpaplHC4RRes,
     LPAPLHC4R lpaplHC4RRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Initialize to 0/1
        mpq_init_set (&lpaplHC4RRes->parts[i], &lpaplHC4RRht->parts[i]);
} // End mphc4r_init_set


//***************************************************************************
//  $mphc4r_init_set_sx
//***************************************************************************

void mphc4r_init_set_sx
    (LPAPLHC4R lpaplHC4RRht,
     APLINT    aplIntNum,
     APLINT    aplIntDen)

{
    int i;

    // Initialize the real part to Num/Den
    mpq_init_set_sx (&lpaplHC4RRht->parts[0], aplIntNum, aplIntDen);

    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
        // Initialize to 0/1
        Myq_init (&lpaplHC4RRht->parts[i]);
} // End mphc4r_init_set_sx


//***************************************************************************
//  $mphc8r_init_set
//***************************************************************************

void mphc8r_init_set
    (LPAPLHC8R lpaplHC8RRes,
     LPAPLHC8R lpaplHC8RRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Initialize to 0/1
        mpq_init_set (&lpaplHC8RRes->parts[i], &lpaplHC8RRht->parts[i]);
} // End mphc8r_init_set


//***************************************************************************
//  $mphc8r_init_set_sx
//***************************************************************************

void mphc8r_init_set_sx
    (LPAPLHC8R lpaplHC8RRht,
     APLINT    aplIntNum,
     APLINT    aplIntDen)

{
    int i;

    // Initialize the real part to Num/Den
    mpq_init_set_sx (&lpaplHC8RRht->parts[0], aplIntNum, aplIntDen);

    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
        // Initialize to 0/1
        Myq_init (&lpaplHC8RRht->parts[i]);
} // End mphc8r_init_set_sx


//***************************************************************************
//  $mphc1v_init_set
//***************************************************************************

void mphc1v_init_set
    (LPAPLHC1V lpaplHC1VRes,
     LPAPLHC1V lpaplHC1VRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 1; i++)
        // Initialize to 0
        mpfr_init_set (lpaplHC1VRes, lpaplHC1VRht, MPFR_RNDN);
} // End mphc1v_init_set


//***************************************************************************
//  $mphc2v_init_set
//***************************************************************************

void mphc2v_init_set
    (LPAPLHC2V lpaplHC2VRes,
     LPAPLHC2V lpaplHC2VRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Initialize to 0
        mpfr_init_set (&lpaplHC2VRes->parts[i], &lpaplHC2VRht->parts[i], MPFR_RNDN);
} // End mphc2v_init_set


//***************************************************************************
//  $mphc2v_init_set_sx
//***************************************************************************

void mphc2v_init_set_sx
    (LPAPLHC2V lpaplHC2VRht,
     APLINT    aplInteger)

{
    int i;

    // Initialize the real part to INT
    mpfr_init_set_sx (&lpaplHC2VRht->parts[0], aplInteger, MPFR_RNDN);

    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
        // Initialize to 0
        mpfr_init (&lpaplHC2VRht->parts[i]);
} // End mphc2v_init_set_sx


//***************************************************************************
//  $mphc4v_init_set
//***************************************************************************

void mphc4v_init_set
    (LPAPLHC4V lpaplHC4VRes,
     LPAPLHC4V lpaplHC4VRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Initialize to 0
        mpfr_init_set (&lpaplHC4VRes->parts[i], &lpaplHC4VRht->parts[i], MPFR_RNDN);
} // End mphc4v_init_set


//***************************************************************************
//  $mphc4v_init_set_si
//***************************************************************************

void mphc4v_init_set_si
    (LPAPLHC4V lpaplHC4VRht,
     int       aplInt32)

{
    int i;

    // Initialize the real part to INT
    mpfr_init_set_si (&lpaplHC4VRht->parts[0], aplInt32, MPFR_RNDN);

    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
        // Initialize to 0
        mpfr_init (&lpaplHC4VRht->parts[i]);
} // End mphc4v_init_set_si


//***************************************************************************
//  $mphc8v_init_set
//***************************************************************************

void mphc8v_init_set
    (LPAPLHC8V lpaplHC8VRes,
     LPAPLHC8V lpaplHC8VRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Initialize to 0/1
        mpfr_init_set (&lpaplHC8VRes->parts[i], &lpaplHC8VRht->parts[i], MPFR_RNDN);
} // End mphc8v_init_set


//***************************************************************************
//  $mphc8v_init_set_si
//***************************************************************************

void mphc8v_init_set_si
    (LPAPLHC8V lpaplHC8VRht,
     int       aplInt32)

{
    int i;

    // Initialize the real part to INT
    mpfr_init_set_si (&lpaplHC8VRht->parts[0], aplInt32, MPFR_RNDN);

    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
        // Initialize to 0
        mpfr_init (&lpaplHC8VRht->parts[i]);
} // End mphc8v_init_set_si


//***************************************************************************
//  $mphcxr_init
//***************************************************************************

void mphcxr_init
    (LPVOID mphcxrVal,
     int    iHCDimVal)

{
    // Split cases based upon the HC Dimension
    switch (iHCDimVal)
    {
        case 1:
            // Initialize to 0/1
            mphc1r_init (mphcxrVal);

            break;

        case 2:
            // Initialize to 0/1
            mphc2r_init (mphcxrVal);

            break;

        case 4:
            // Initialize to 0/1
            mphc4r_init (mphcxrVal);

            break;

        case 8:
            // Initialize to 0/1
            mphc8r_init (mphcxrVal);

            break;

        defstop
            break;
    } // End SWITCH
} // mphcxr_init


//***************************************************************************
//  $mphc2r_init
//***************************************************************************

void mphc2r_init
    (LPAPLHC2R lpaplHC2RRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Initialize to 0/1
        Myq_init (&lpaplHC2RRht->parts[i]);
} // End mphc2r_init


//***************************************************************************
//  $mphc4r_init
//***************************************************************************

void mphc4r_init
    (LPAPLHC4R lpaplHC4RRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Initialize to 0/1
        Myq_init (&lpaplHC4RRht->parts[i]);
} // End mphc4r_init


//***************************************************************************
//  $mphc8r_init
//***************************************************************************

void mphc8r_init
    (LPAPLHC8R lpaplHC8RRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Initialize to 0/1
        Myq_init (&lpaplHC8RRht->parts[i]);
} // End mphc8r_init


//***************************************************************************
//  $mphcxv_init0
//***************************************************************************

void mphcxv_init0
    (LPVOID mphcxvVal,
     int    iHCDimVal)

{
    // Split cases based upon the HC Dimension
    switch (iHCDimVal)
    {
        case 1:
            // Initialize to 0
            mphc1v_init0 (mphcxvVal);

            break;

        case 2:
            // Initialize to 0
            mphc2v_init0 (mphcxvVal);

            break;

        case 4:
            // Initialize to 0
            mphc4v_init0 (mphcxvVal);

            break;

        case 8:
            // Initialize to 0
            mphc8v_init0 (mphcxvVal);

            break;

        defstop
            break;
    } // End SWITCH
} // mphcxv_init0


//***************************************************************************
//  $mphc2v_init0
//***************************************************************************

void mphc2v_init0
    (LPAPLHC2V lpaplHC2VRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Initialize to 0
        mpfr_init0 (&lpaplHC2VRht->parts[i]);
} // End mphc2v_init0


//***************************************************************************
// $mphc2v_init_copy
//***************************************************************************

void mphc2v_init_copy
    (LPAPLHC2V lpaplHC2VRes,
     LPAPLHC2V lpaplHC2VRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Initialize to 0
        mpfr_init_copy (&lpaplHC2VRes->parts[i], &lpaplHC2VRht->parts[i]);
} // End mphc2v_init_copy


//***************************************************************************
// $mphc4v_init0
//***************************************************************************

void mphc4v_init0
    (LPAPLHC4V lpaplHC4VRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Initialize to 0
        mpfr_init0 (&lpaplHC4VRht->parts[i]);
} // End mphc4v_init0


//***************************************************************************
// $mphc4v_init_copy
//***************************************************************************

void mphc4v_init_copy
    (LPAPLHC4V lpaplHC4VRes,
     LPAPLHC4V lpaplHC4VRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Initialize to 0
        mpfr_init_copy (&lpaplHC4VRes->parts[i], &lpaplHC4VRht->parts[i]);
} // End mphc4v_init_copy


//***************************************************************************
// $mphc8v_init0
//***************************************************************************

void mphc8v_init0
    (LPAPLHC8V lpaplHC8VRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Initialize to 0/1
        mpfr_init0 (&lpaplHC8VRht->parts[i]);
} // End mphc8v_init0


//***************************************************************************
// $mphc8v_init_copy
//***************************************************************************

void mphc8v_init_copy
    (LPAPLHC8V lpaplHC8VRes,
     LPAPLHC8V lpaplHC8VRht)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Initialize to 0
        mpfr_init_copy (&lpaplHC8VRes->parts[i], &lpaplHC8VRht->parts[i]);
} // End mphc8v_init_copy


//***************************************************************************
//  $DistHC2I
//***************************************************************************

APLFLOAT DistHC2I
    (APLHC2I aplLft,                    // Left arg
     APLHC2I aplRht)                    // Right ...

{
    return MagHC2I (SubHC2I_RE (aplLft, aplRht));
} // End DistHC2I


//***************************************************************************
//  $DistHC2F
//***************************************************************************

APLFLOAT DistHC2F
    (APLHC2F aplLft,                    // Left arg
     APLHC2F aplRht)                    // Right ...

{
    return MagHC2F (SubHC2F_RE (aplLft, aplRht));
} // End DistHC2F


//***************************************************************************
//  $DistHC4I
//***************************************************************************

APLFLOAT DistHC4I
    (APLHC4I aplLft,                    // Left arg
     APLHC4I aplRht)                    // Right ...

{
    return MagHC4I (SubHC4I_RE (aplLft, aplRht));
} // End DistHC4I


//***************************************************************************
//  $DistHC4F
//***************************************************************************

APLFLOAT DistHC4F
    (APLHC4F aplLft,                    // Left arg
     APLHC4F aplRht)                    // Right ...

{
    return MagHC4F (SubHC4F_RE (aplLft, aplRht));
} // End DistHC4F


//***************************************************************************
//  $DistHC8I
//***************************************************************************

APLFLOAT DistHC8I
    (APLHC8I aplLft,                    // Left arg
     APLHC8I aplRht)                    // Right ...

{
    return MagHC8I (SubHC8I_RE (aplLft, aplRht));
} // End DistHC8I


//***************************************************************************
//  $DistHC8F
//***************************************************************************

APLFLOAT DistHC8F
    (APLHC8F aplLft,                    // Left arg
     APLHC8F aplRht)                    // Right ...

{
    return MagHC8F (SubHC8F_RE (aplLft, aplRht));
} // End DistHC8F


//***************************************************************************
//  $DistHC2R
//***************************************************************************

APLVFP DistHC2R
    (APLHC2R aplLft,                    // Left arg
     APLHC2R aplRht)                    // Right ...

{
    return MagHC2R (SubHC2R_RE (aplLft, aplRht));
} // End DistHC2R


//***************************************************************************
//  $DistHC4R
//***************************************************************************

APLVFP DistHC4R
    (APLHC4R aplLft,                    // Left arg
     APLHC4R aplRht)                    // Right ...

{
    return MagHC4R (SubHC4R_RE (aplLft, aplRht));
} // End DistHC4R


//***************************************************************************
//  $DistHC8R
//***************************************************************************

APLVFP DistHC8R
    (APLHC8R aplLft,                    // Left arg
     APLHC8R aplRht)                    // Right ...

{
    return MagHC8R (SubHC8R_RE (aplLft, aplRht));
} // End DistHC8R


//***************************************************************************
//  $DistHC2V
//***************************************************************************

APLVFP DistHC2V
    (APLHC2V aplLft,                    // Left arg
     APLHC2V aplRht)                    // Right ...

{
    return MagHC2V (SubHC2V_RE (aplLft, aplRht));
} // End DistHC2V


//***************************************************************************
//  $DistHC4V
//***************************************************************************

APLVFP DistHC4V
    (APLHC4V aplLft,                    // Left arg
     APLHC4V aplRht)                    // Right ...

{
    return MagHC4V (SubHC4V_RE (aplLft, aplRht));
} // End DistHC4V


//***************************************************************************
//  $DistHC8V
//***************************************************************************

APLVFP DistHC8V
    (APLHC8V aplLft,                    // Left arg
     APLHC8V aplRht)                    // Right ...

{
    return MagHC8V (SubHC8V_RE (aplLft, aplRht));
} // End DistHC8V


//***************************************************************************
//  $InitSet
//
//  Initialize a memory location and copy data to it
//***************************************************************************

void InitSet
    (APLSTYPE   aplTypeArg,         // Arg storage type
     LPVOID     lpMemRes,           // Ptr to save area
     APLUINT    uRes,               // Index into <lpMemRes>
     LPALLTYPES lpatArg)            // Ptr to Arg as ALLTYPES

{
    int i,                          // Loop counter
        iHCDimArg;                  // HC Dimension (1, 2, 4, 8)

    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDimArg = TranslateArrayTypeToHCDim (aplTypeArg);

    // Split cases based upon the result storage type
    switch (aplTypeArg)
    {
        case ARRAY_INT:
            ((LPAPLINT  ) lpMemRes)[uRes] = lpatArg->aplInteger;

            break;

        case ARRAY_FLOAT:
            ((LPAPLFLOAT) lpMemRes)[uRes] = lpatArg->aplFloat;

            break;

        case ARRAY_RAT:
            mpq_init_set (&((LPAPLRAT ) lpMemRes)[uRes], &lpatArg->aplRat);

            break;

        case ARRAY_VFP:
            mpfr_init_copy (&((LPAPLVFP ) lpMemRes)[uRes], &lpatArg->aplVfp);

            break;

        case ARRAY_CHAR:
            ((LPAPLCHAR) lpMemRes)[uRes] = lpatArg->aplChar;

            break;

        case ARRAY_HC2I:
            ((LPAPLHC2I) lpMemRes)[uRes] = lpatArg->aplHC2I;

            break;

        case ARRAY_HC2F:
            ((LPAPLHC2F) lpMemRes)[uRes] = lpatArg->aplHC2F;

            break;

        case ARRAY_HC4I:
            ((LPAPLHC4I) lpMemRes)[uRes] = lpatArg->aplHC4I;

            break;

        case ARRAY_HC4F:
            ((LPAPLHC4F) lpMemRes)[uRes] = lpatArg->aplHC4F;

            break;

        case ARRAY_HC8I:
            ((LPAPLHC8I) lpMemRes)[uRes] = lpatArg->aplHC8I;

            break;

        case ARRAY_HC8F:
            ((LPAPLHC8F) lpMemRes)[uRes] = lpatArg->aplHC8F;

            break;

        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            // Multiply uRes by iHCDimArg as uRes is in those units, but
            //   is used in units of one below
            uRes *= iHCDimArg;

            for (i = 0; i < iHCDimArg; i++)
                mpq_init_set (&((LPAPLRAT) lpMemRes)[uRes++], &lpatArg->aplHC2R.parts[i]);

            break;

        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            // Multiply uRes by iHCDimArg as uRes is in those units, but
            //   is used in units of one below
            uRes *= iHCDimArg;

            for (i = 0; i < iHCDimArg; i++)
                mpfr_init_copy (&((LPAPLVFP) lpMemRes)[uRes++], &lpatArg->aplHC2V.parts[i]);

            break;

        defstop
            break;
    } // End SWITCH
} // End InitSet


//***************************************************************************
//  $IzitImaginary
//
//  Return TRUE iff the given HC number has non-zero imaginary parts
//***************************************************************************

UBOOL IzitImaginary
    (APLSTYPE aplTypeArg,       // Arg storage type
     LPVOID   lpMemArg)         // Ptr to global memory

{
    UBOOL bImag = FALSE;        // TRUE iff the number is not imaginary
    int   i,                    // Loop counter
          iHCDimArg;            // Arg HC Dimension (1, 2, 4, 8)

    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDimArg = TranslateArrayTypeToHCDim (aplTypeArg);

    // Loop through the imaginary HC Dimensions
    // Note that if there are no imaginary parts (iHCDimArg EQ 1),
    //   this routine returns FALSE.
    for (i = 1; !bImag && i < iHCDimArg; i++)
    // Split cases based upon the axis storage type
    switch (aplTypeArg)
    {
        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
            bImag = ((LPAPLHC8I) lpMemArg)->parts[i] NE 0;

            break;

        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
            bImag = ((LPAPLHC8F) lpMemArg)->parts[i] NE 0;

            break;

        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            bImag = !IsMpq0 (&((LPAPLHC8R) lpMemArg)->parts[i]);

            break;

        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            bImag = !IsMpf0 (&((LPAPLHC8V) lpMemArg)->parts[i]);

            break;

        defstop
            break;
    } // End IF/FOR/SWITCH

    return bImag;
} // End IzitImaginary


//***************************************************************************
//  $IsZeroHCxI
//***************************************************************************

UBOOL IsZeroHCxI
    (LPVOID lpaplRht,               // Ptr to right arg
     int    iHCDimRht)              // HC Dimension (1, 2, 4, 8)

{
    int i;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < iHCDimRht; i++)
    if (((LPAPLHC8I) lpaplRht)->parts[i] NE 0)
        return FALSE;

    return TRUE;
} // End IsZeroHCxI


//***************************************************************************
//  $IsOneHCxI
//***************************************************************************

UBOOL IsOneHCxI
    (LPVOID lpaplRht,               // Ptr to right arg
     int    iHCDimRht)              // HC Dimension (1, 2, 4, 8)

{
    int i,
        j = 1;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < iHCDimRht; i++, j = 0)
    if (((LPAPLHC8I) lpaplRht)->parts[i] NE j)
        return FALSE;

    return TRUE;
} // End IsOneHCxI


//***************************************************************************
//  $IsZeroHCxF
//***************************************************************************

UBOOL IsZeroHCxF
    (LPVOID lpaplRht,               // Ptr to right arg
     int    iHCDimRht)              // HC Dimension (1, 2, 4, 8)

{
    int i;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < iHCDimRht; i++)
    if (((LPAPLHC8F) lpaplRht)->parts[i] NE 0.0)
        return FALSE;

    return TRUE;
} // End IsZeroHCxF


//***************************************************************************
//  $IsOneHCxF
//***************************************************************************

UBOOL IsOneHCxF
    (LPVOID lpaplRht,               // Ptr to right arg
     int    iHCDimRht)              // HC Dimension (1, 2, 4, 8)

{
    int      i;
    APLFLOAT j = 1.0;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < iHCDimRht; i++, j = 0.0)
    if (((LPAPLHC8F) lpaplRht)->parts[i] NE j)
        return FALSE;

    return TRUE;
} // End IsOneHCxF


//***************************************************************************
//  $IsZeroHCxR
//***************************************************************************

UBOOL IsZeroHCxR
    (LPVOID lpaplRht,               // Ptr to right arg
     int    iHCDimRht)              // HC Dimension (1, 2, 4, 8)

{
    int i;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < iHCDimRht; i++)
    if (!IsMpq0 (&((LPAPLHC8R) lpaplRht)->parts[i]))
        return FALSE;

    return TRUE;
} // End IsZeroHCxR


//***************************************************************************
//  $IsOneHCxR
//***************************************************************************

UBOOL IsOneHCxR
    (LPVOID lpaplRht,               // Ptr to right arg
     int    iHCDimRht)              // HC Dimension (1, 2, 4, 8)

{
    int    i;
    APLRAT j = mpqOne;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < iHCDimRht; i++, j = mpqZero)
    if (mpq_cmp (&((LPAPLHC8R) lpaplRht)->parts[i], &j) NE 0)
        return FALSE;

    return TRUE;
} // End IsOneHCxR


//***************************************************************************
//  $IsZeroHCxV
//***************************************************************************

UBOOL IsZeroHCxV
    (LPVOID lpaplRht,               // Ptr to right arg
     int    iHCDimRht)              // HC Dimension (1, 2, 4, 8)

{
    int i;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < iHCDimRht; i++)
    if (!IsMpf0 (&((LPAPLHC8V) lpaplRht)->parts[i]))
        return FALSE;

    return TRUE;
} // End IsZeroHCxV


//***************************************************************************
//  $IsOneHCxV
//***************************************************************************

UBOOL IsOneHCxV
    (LPVOID lpaplRht,               // Ptr to right arg
     int    iHCDimRht)              // HC Dimension (1, 2, 4, 8)

{
    int    i;
    APLVFP j = mpfOne;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < iHCDimRht; i++, j = mpfZero)
    if (mpfr_cmp (&((LPAPLHC8V) lpaplRht)->parts[i], &j) NE 0)
        return FALSE;

    return TRUE;
} // End IsOneHCxV


//***************************************************************************
//  $MakeUnit
//
//  Make a HC unit
//***************************************************************************

void MakeUnit
    (APLSTYPE   aplType,    // Result storage type
     UINT       uIndex,     // Index into appropriate table
     LPALLTYPES lpatRes)    // Result as ALLTYPES

{
    int iHCDimRes,          // HC Dimension (1, 2, 4, 8)
        i;                  // Loop counter

    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDimRes = TranslateArrayTypeToHCDim (aplType);

    // Split cases based upon the result storage type
    switch (aplType)
    {
        case ARRAY_HC2I:
        case ARRAY_HC4I:
////////case ARRAY_HC8I:
            // Loop through all of the parts
            for (i = 0; i < iHCDimRes; i++)
                lpatRes->aplHC8I.parts[i] = gUnitCoeffs4[uIndex][i];
            break;

        case ARRAY_HC2F:
        case ARRAY_HC4F:
////////case ARRAY_HC8F:
            // Loop through all of the parts
            for (i = 0; i < iHCDimRes; i++)
                lpatRes->aplHC8F.parts[i] = (APLFLOAT) gUnitCoeffs4[uIndex][i];
            break;

        case ARRAY_HC2R:
        case ARRAY_HC4R:
////////case ARRAY_HC8R:
            // Loop through all of the parts
            for (i = 0; i < iHCDimRes; i++)
            {
                // Initialize to 0/1
                Myq_init   (&lpatRes->aplHC8R.parts[i]);
                mpq_set_sx (&lpatRes->aplHC8R.parts[i], gUnitCoeffs4[uIndex][i], 1);
            } // End FOR

            break;

        case ARRAY_HC2V:
        case ARRAY_HC4V:
////////case ARRAY_HC8V:
            // Loop through all of the parts
            for (i = 0; i < iHCDimRes; i++)
            {
                // Initialize to 0
                mpfr_init0  (&lpatRes->aplHC8V.parts[i]);
                mpfr_set_sx (&lpatRes->aplHC8V.parts[i], gUnitCoeffs4[uIndex][i], MPFR_RNDN);
            } // End FOR

            break;

////////case ARRAY_BOOL:
////////case ARRAY_INT:
////////case ARRAY_FLOAT:
////////case ARRAY_APA:
////////case ARRAY_CHAR:
////////case ARRAY_HETERO:
////////case ARRAY_NESTED:
        defstop
            break;
    } // End SWITCH
} // End MakeUnit


//***************************************************************************
//  $GetHCImagSepWS
//
//  Get a ptr to the list of imaginary HC separators
//***************************************************************************

LPWCHAR GetHCImagSepWS
    (APLSTYPE aplType)              // Array storage type

{
    int iHCDim;             // HC Dimension (1, 2, 4, 8)

    // Get the HC Dimension (1, 2, 4, 8)
    iHCDim = TranslateArrayTypeToHCDim (aplType);

    // Split cases based upon the HC Dimension
    switch (iHCDim)
    {
        case 1:
            return L".";

        case 2:
        case 4:
        case 8:
            return L"ijJkl";

        defstop
            return L"\0";
    } // End SWITCH
} // GetHcImagSep


//***************************************************************************
//  $HasFractionality
//
//  Is a storage type allowed for Floor/Ceiling/GCD/LCM/Residue/Encode/Base Value?
//***************************************************************************

UBOOL HasFractionality
    (APLSTYPE aplType)              // Array storage type

{
    int iHCDim;                     // HC Dimension (1, 2, 4, 8)

    // Get the HC Dimension (1, 2, 4, 8)
    iHCDim = TranslateArrayTypeToHCDim (aplType);

    // Split cases based upon the HC Dimension
    switch (iHCDim)
    {
        case 1:
            return IsNumeric (aplType);

        case 2:
            return TRUE;

        case 4:
            return gUseHurwitz ? TRUE : FALSE;

        case 8:
            return FALSE;

        defstop
            return FALSE;
    } // End SWITCH
} // End HasFractionality


//***************************************************************************
//  End of File: hc_proc.c
//***************************************************************************
