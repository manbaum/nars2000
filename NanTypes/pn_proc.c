//***************************************************************************
//  NARS2000 -- Parser Grammar Functions XXX Point Notation
//***************************************************************************

//***************************************************************************
//  Parse a line of XXX Point Notation characters.
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
#define _USE_MATH_DEFINES
#include <math.h>
#include "headers.h"
#include <gsl/gsl_sf_zeta.h>

//#define DEBUG_FMT


//***************************************************************************
//  $PN_actIDENT
//***************************************************************************

void PN_actIDENT
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
} // End PN_actIDENT


//***************************************************************************
//  $PN_actUNK
//***************************************************************************

void PN_actUNK
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    DbgStop ();
} // End PN_actUNK


//***************************************************************************
//  $PN_actINT_FLT
//***************************************************************************

void PN_actINT_FLT
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to FLT
    lpSrc->at.aplFloat = (APLFLOAT) lpSrc->at.aplInteger;

    lpSrc->chType = PN_NUMTYPE_FLT;
} // End PN_actINT_FLT


//***************************************************************************
//  $PN_actINT_RAT
//***************************************************************************

void PN_actINT_RAT
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to RAT
    mpq_init_set_sx (&lpSrc->at.aplRat, lpSrc->at.aplInteger, 1);

    lpSrc->chType = PN_NUMTYPE_RAT;
} // End PN_actINT_RAT


//***************************************************************************
//  $PN_actINT_VFP
//***************************************************************************

void PN_actINT_VFP
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to VFP
    mpfr_init_set_sx (&lpSrc->at.aplVfp, lpSrc->at.aplInteger, MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_VFP;
} // End PN_actINT_VFP


//***************************************************************************
//  $PN_actINT_HC2I
//***************************************************************************

void PN_actINT_HC2I
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to HC2I
    // The following stmt is not needed as the union overlays the two elements
////lpSrc->at.aplHC2I.parts[0] = lpSrc->at.aplInteger;
    lpSrc->at.aplHC2I.parts[1] = 0;

    lpSrc->chType = PN_NUMTYPE_HC2I;
} // End PN_actINT_HC2I


//***************************************************************************
//  $PN_actINT_HC4I
//***************************************************************************

void PN_actINT_HC4I
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to HC4I
    // The following stmt is not needed as the union overlays the two elements
////lpSrc->at.aplHC4I.parts[0] = lpSrc->at.aplInteger;
    lpSrc->at.aplHC4I.parts[1] = 0;
    lpSrc->at.aplHC4I.parts[2] = 0;
    lpSrc->at.aplHC4I.parts[3] = 0;

    lpSrc->chType = PN_NUMTYPE_HC4I;
} // End PN_actINT_HC4I


//***************************************************************************
//  $PN_actINT_HC8I
//***************************************************************************

void PN_actINT_HC8I
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to HC8I
    // The following stmt is not needed as the union overlays the two elements
////lpSrc->at.aplHC8I.parts[0] = lpSrc->at.aplInteger;
    lpSrc->at.aplHC8I.parts[1] = 0;
    lpSrc->at.aplHC8I.parts[2] = 0;
    lpSrc->at.aplHC8I.parts[3] = 0;
    lpSrc->at.aplHC8I.parts[4] = 0;
    lpSrc->at.aplHC8I.parts[5] = 0;
    lpSrc->at.aplHC8I.parts[6] = 0;
    lpSrc->at.aplHC8I.parts[7] = 0;

    lpSrc->chType = PN_NUMTYPE_HC8I;
} // End PN_actINT_HC8I


//***************************************************************************
//  $PN_actINT_HC2F
//***************************************************************************

void PN_actINT_HC2F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to HC2F
    lpSrc->at.aplHC2F.parts[0] = (APLFLOAT) lpSrc->at.aplInteger;
    lpSrc->at.aplHC2F.parts[1] = 0;

    lpSrc->chType = PN_NUMTYPE_HC2F;
} // End PN_actINT_HC2F


//***************************************************************************
//  $PN_actINT_HC4F
//***************************************************************************

void PN_actINT_HC4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to HC4F
    lpSrc->at.aplHC4F.parts[0] = (APLFLOAT) lpSrc->at.aplInteger;
    lpSrc->at.aplHC4F.parts[1] = 0;
    lpSrc->at.aplHC4F.parts[2] = 0;
    lpSrc->at.aplHC4F.parts[3] = 0;

    lpSrc->chType = PN_NUMTYPE_HC4F;
} // End PN_actINT_HC4F


//***************************************************************************
//  $PN_actINT_HC8F
//***************************************************************************

void PN_actINT_HC8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to HC8F
    lpSrc->at.aplHC8F.parts[0] = (APLFLOAT) lpSrc->at.aplInteger;
    lpSrc->at.aplHC8F.parts[1] = 0;
    lpSrc->at.aplHC8F.parts[2] = 0;
    lpSrc->at.aplHC8F.parts[3] = 0;
    lpSrc->at.aplHC8F.parts[4] = 0;
    lpSrc->at.aplHC8F.parts[5] = 0;
    lpSrc->at.aplHC8F.parts[6] = 0;
    lpSrc->at.aplHC8F.parts[7] = 0;

    lpSrc->chType = PN_NUMTYPE_HC8F;
} // End PN_actINT_HC8F


//***************************************************************************
//  $PN_actINT_HC2R
//***************************************************************************

void PN_actINT_HC2R
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to RAT
    mpq_init_set_sx (&lpSrc->at.aplHC2R.parts[0], lpSrc->at.aplInteger, 1);
    mpq_init        (&lpSrc->at.aplHC2R.parts[1]                         );

    lpSrc->chType = PN_NUMTYPE_HC2R;
} // End PN_actINT_HC2R


//***************************************************************************
//  $PN_actINT_HC4R
//***************************************************************************

void PN_actINT_HC4R
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to RAT
    mpq_init_set_sx (&lpSrc->at.aplHC4R.parts[0], lpSrc->at.aplInteger, 1);
    mpq_init        (&lpSrc->at.aplHC4R.parts[1]                         );
    mpq_init        (&lpSrc->at.aplHC4R.parts[2]                         );
    mpq_init        (&lpSrc->at.aplHC4R.parts[3]                         );

    lpSrc->chType = PN_NUMTYPE_HC4R;
} // End PN_actINT_HC4R


//***************************************************************************
//  $PN_actINT_HC8R
//***************************************************************************

void PN_actINT_HC8R
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to RAT
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[0], lpSrc->at.aplInteger, 1);
    mpq_init        (&lpSrc->at.aplHC8R.parts[1]                         );
    mpq_init        (&lpSrc->at.aplHC8R.parts[2]                         );
    mpq_init        (&lpSrc->at.aplHC8R.parts[3]                         );
    mpq_init        (&lpSrc->at.aplHC8R.parts[4]                         );
    mpq_init        (&lpSrc->at.aplHC8R.parts[5]                         );
    mpq_init        (&lpSrc->at.aplHC8R.parts[6]                         );
    mpq_init        (&lpSrc->at.aplHC8R.parts[7]                         );

    lpSrc->chType = PN_NUMTYPE_HC8R;
} // End PN_actINT_HC8R


//***************************************************************************
//  $PN_actINT_HC2V
//***************************************************************************

void PN_actINT_HC2V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to VFP
    mpfr_init_set_sx (&lpSrc->at.aplHC2V.parts[0], lpSrc->at.aplInteger, MPFR_RNDN);
    mpfr_init0       (&lpSrc->at.aplHC2V.parts[1]);

    lpSrc->chType = PN_NUMTYPE_HC2V;
} // End PN_actINT_HC2V


//***************************************************************************
//  $PN_actINT_HC4V
//***************************************************************************

void PN_actINT_HC4V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to VFP
    mpfr_init_set_sx (&lpSrc->at.aplHC4V.parts[0], lpSrc->at.aplInteger, MPFR_RNDN);
    mpfr_init0       (&lpSrc->at.aplHC4V.parts[1]);
    mpfr_init0       (&lpSrc->at.aplHC4V.parts[2]);
    mpfr_init0       (&lpSrc->at.aplHC4V.parts[3]);

    lpSrc->chType = PN_NUMTYPE_HC4V;
} // End PN_actINT_HC4V


//***************************************************************************
//  $PN_actINT_HC8V
//***************************************************************************

void PN_actINT_HC8V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to VFP
    mpfr_init_set_sx (&lpSrc->at.aplHC8V.parts[0], lpSrc->at.aplInteger, MPFR_RNDN);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[1]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[2]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[3]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[4]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[5]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[6]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[7]);

    lpSrc->chType = PN_NUMTYPE_HC8V;
} // End PN_actINT_HC8V


//***************************************************************************
//  $PN_actFLT_VFP
//***************************************************************************

void PN_actFLT_VFP
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Initialize to 0
    mphcXv_init0 (&lpSrc->at.aplVfp, 1);

    // In order to preserve the precision of the floating point number,
    //   we rescan it as a VFP number
    // Convert the string to a VFP number
    mpfr_strtofr (&lpSrc->at.aplVfp, &lppnLocalVars->lpszStart[lpSrc->uNumStart], NULL, 10, MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_VFP;
} // End PN_actFLT_VFP


//***************************************************************************
//  $PN_actFLT_HC2F
//***************************************************************************

void PN_actFLT_HC2F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from FLT to HC2F
    // The following stmt is not needed as the union overlays the two elements
////lpSrc->at.aplHC2F.parts[0] = lpSrc->at.aplFloat;
    lpSrc->at.aplHC2F.parts[1] = 0;

    lpSrc->chType = PN_NUMTYPE_HC2F;
} // End PN_actFLT_HC2F


//***************************************************************************
//  $PN_actFLT_HC4F
//***************************************************************************

void PN_actFLT_HC4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from FLT to HC4F
    // The following stmt is not needed as the union overlays the two elements
////lpSrc->at.aplHC4F.parts[0] = lpSrc->at.aplFloat;
    lpSrc->at.aplHC4F.parts[1] = 0;
    lpSrc->at.aplHC4F.parts[2] = 0;
    lpSrc->at.aplHC4F.parts[3] = 0;

    lpSrc->chType = PN_NUMTYPE_HC4F;
} // End PN_actFLT_HC4F


//***************************************************************************
//  $PN_actFLT_HC8F
//***************************************************************************

void PN_actFLT_HC8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from FLT to HC8F
    // The following stmt is not needed as the union overlays the two elements
////lpSrc->at.aplHC8F.parts[0] = lpSrc->at.aplFloat;
    lpSrc->at.aplHC8F.parts[1] = 0;
    lpSrc->at.aplHC8F.parts[2] = 0;
    lpSrc->at.aplHC8F.parts[3] = 0;

    lpSrc->chType = PN_NUMTYPE_HC8F;
} // End PN_actFLT_HC8F


//***************************************************************************
//  $PN_actFLT_HC2V
//***************************************************************************

void PN_actFLT_HC2V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Initialize to 0
    mphcXv_init0 (&lpSrc->at.aplVfp, 2);

    // In order to preserve the precision of the floating point number,
    //   we rescan it as a VFP number
    // Convert the string to a VFP number
    mpfr_set_str (&lpSrc->at.aplVfp, &lppnLocalVars->lpszStart[lpSrc->uNumStart], 10, MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_HC2V;
} // End PN_actFLT_HC2V


//***************************************************************************
//  $PN_actFLT_HC4V
//***************************************************************************

void PN_actFLT_HC4V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Initialize to 0
    mphcXv_init0 (&lpSrc->at.aplVfp, 4);

    // In order to preserve the precision of the floating point number,
    //   we rescan it as a VFP number
    // Convert the string to a VFP number
    mpfr_set_str (&lpSrc->at.aplVfp, &lppnLocalVars->lpszStart[lpSrc->uNumStart], 10, MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_HC4V;
} // End PN_actFLT_HC4V


//***************************************************************************
//  $PN_actFLT_HC8V
//***************************************************************************

void PN_actFLT_HC8V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Initialize to 0
    mphcXv_init0 (&lpSrc->at.aplVfp, 8);

    // In order to preserve the precision of the floating point number,
    //   we rescan it as a VFP number
    // Convert the string to a VFP number
    mpfr_set_str (&lpSrc->at.aplVfp, &lppnLocalVars->lpszStart[lpSrc->uNumStart], 10, MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_HC8V;
} // End PN_actFLT_HC8V


//***************************************************************************
//  $PN_actRAT_VFP
//***************************************************************************

void PN_actRAT_VFP
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLVFP aplVfp = {0};

    // Convert the value from RAT to VFP
    mpfr_init_set_q  (&aplVfp, &lpSrc->at.aplRat, MPFR_RNDN);

    Myq_clear (&lpSrc->at.aplRat);

    lpSrc->at.aplVfp = aplVfp;

    lpSrc->chType = PN_NUMTYPE_VFP;
} // End PN_actRAT_VFP


//***************************************************************************
//  $PN_actRAT_HC2R
//***************************************************************************

void PN_actRAT_HC2R
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Copy the RAT to the real part
    // The following stmt is not needed as the union overlays the two elements
////lpSrc->at.aplHC2R.parts[0] = lpSrc->at.aplRat;

    // Set the imaginary part to 0/1
    mpq_init (&lpSrc->at.aplHC2R.parts[1]);

    lpSrc->chType = PN_NUMTYPE_HC2R;
} // End PN_actRAT_HC2R


//***************************************************************************
//  $PN_actRAT_HC4R
//***************************************************************************

void PN_actRAT_HC4R
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Copy the RAT to the real part
    // The following stmt is not needed as the union overlays the two elements
////lpSrc->at.aplHC4R.parts[0] = lpSrc->at.aplRat;

    // Set the imaginary parts to 0/1
    mpq_init (&lpSrc->at.aplHC4R.parts[1]);
    mpq_init (&lpSrc->at.aplHC4R.parts[2]);
    mpq_init (&lpSrc->at.aplHC4R.parts[3]);

    lpSrc->chType = PN_NUMTYPE_HC4R;
} // End PN_actRAT_HC4R


//***************************************************************************
//  $PN_actRAT_HC8R
//***************************************************************************

void PN_actRAT_HC8R
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Copy the RAT to the real part
    // The following stmt is not needed as the union overlays the two elements
////lpSrc->at.aplHC8R.parts[0] = lpSrc->at.aplRat;

    // Set the imaginary parts to 0/1
    mpq_init (&lpSrc->at.aplHC8R.parts[1]);
    mpq_init (&lpSrc->at.aplHC8R.parts[2]);
    mpq_init (&lpSrc->at.aplHC8R.parts[3]);
    mpq_init (&lpSrc->at.aplHC8R.parts[4]);
    mpq_init (&lpSrc->at.aplHC8R.parts[5]);
    mpq_init (&lpSrc->at.aplHC8R.parts[6]);
    mpq_init (&lpSrc->at.aplHC8R.parts[7]);

    lpSrc->chType = PN_NUMTYPE_HC8R;
} // End PN_actRAT_HC8R


//***************************************************************************
//  $PN_actRAT_HC2V
//***************************************************************************

void PN_actRAT_HC2V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLVFP aplVfp = {0};

    // Convert the value from RAT to VFP
    mpfr_init_set_q  (&aplVfp, &lpSrc->at.aplRat, MPFR_RNDN);

    Myq_clear (&lpSrc->at.aplRat);

    lpSrc->at.aplHC2V.parts[0] = aplVfp;
    mpfr_init0 (&lpSrc->at.aplHC2V.parts[1]);

    lpSrc->chType = PN_NUMTYPE_HC2V;
} // End PN_actRAT_HC2V


//***************************************************************************
//  $PN_actRAT_HC4V
//***************************************************************************

void PN_actRAT_HC4V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLVFP aplVfp = {0};

    // Convert the value from RAT to VFP
    mpfr_init_set_q  (&aplVfp, &lpSrc->at.aplRat, MPFR_RNDN);

    Myq_clear (&lpSrc->at.aplRat);

    lpSrc->at.aplHC4V.parts[0] = aplVfp;
    mpfr_init0 (&lpSrc->at.aplHC4V.parts[1]);
    mpfr_init0 (&lpSrc->at.aplHC4V.parts[2]);
    mpfr_init0 (&lpSrc->at.aplHC4V.parts[3]);

    lpSrc->chType = PN_NUMTYPE_HC4V;
} // End PN_actRAT_HC4V


//***************************************************************************
//  $PN_actRAT_HC8V
//***************************************************************************

void PN_actRAT_HC8V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLVFP aplVfp = {0};

    // Convert the value from RAT to VFP
    mpfr_init_set_q  (&aplVfp, &lpSrc->at.aplRat, MPFR_RNDN);

    Myq_clear (&lpSrc->at.aplRat);

    lpSrc->at.aplHC4V.parts[0] = aplVfp;
    mpfr_init0 (&lpSrc->at.aplHC4V.parts[1]);
    mpfr_init0 (&lpSrc->at.aplHC4V.parts[2]);
    mpfr_init0 (&lpSrc->at.aplHC4V.parts[3]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[4]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[5]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[6]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[7]);

    lpSrc->chType = PN_NUMTYPE_HC8V;
} // End PN_actRAT_HC8V


//***************************************************************************
//  $PN_actVFP_HC2V
//***************************************************************************

void PN_actVFP_HC2V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Copy the VFP to the real part
    // The following stmt is not needed as the union overlays the two elements
////lpSrc->at.aplHC2V.parts[0] = lpSrc->at.aplVfp;

    // Set the imaginary part to 0/1
    mpfr_init0 (&lpSrc->at.aplHC2V.parts[1]);

    lpSrc->chType = PN_NUMTYPE_HC2V;
} // End PN_actVFP_HC2V


//***************************************************************************
//  $PN_actVFP_HC4V
//***************************************************************************

void PN_actVFP_HC4V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Copy the VFP to the real part
    // The following stmt is not needed as the union overlays the two elements
////lpSrc->at.aplHC4V.parts[0] = lpSrc->at.aplVfp;

    // Set the imaginary part to 0/1
    mpfr_init0 (&lpSrc->at.aplHC4V.parts[1]);
    mpfr_init0 (&lpSrc->at.aplHC4V.parts[2]);
    mpfr_init0 (&lpSrc->at.aplHC4V.parts[3]);

    lpSrc->chType = PN_NUMTYPE_HC4V;
} // End PN_actVFP_HC4V


//***************************************************************************
//  $PN_actVFP_HC8V
//***************************************************************************

void PN_actVFP_HC8V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Copy the VFP to the real part
    // The following stmt is not needed as the union overlays the two elements
////lpSrc->at.aplHC8V.parts[0] = lpSrc->at.aplVfp;

    // Set the imaginary part to 0/1
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[1]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[2]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[3]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[4]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[5]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[6]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[7]);

    lpSrc->chType = PN_NUMTYPE_HC8V;
} // End PN_actVFP_HC8V


//***************************************************************************
//  $PN_actHC2I_HC4I
//***************************************************************************

void PN_actHC2I_HC4I
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from HC2I to HC4I
    // The following stmts are not needed as the union overlays the two elements
////lpSrc->at.aplHC4I.parts[0] = lpSrc->at.aplHC2I.parts[0];
////lpSrc->at.aplHC4I.parts[1] = lpSrc->at.aplHC2I.parts[1];
    lpSrc->at.aplHC4I.parts[2] = 0;
    lpSrc->at.aplHC4I.parts[3] = 0;

    lpSrc->chType = PN_NUMTYPE_HC4I;
} // End PN_actHC2I_HC4I


//***************************************************************************
//  $PN_actHC2I_HC8I
//***************************************************************************

void PN_actHC2I_HC8I
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from HC2I to HC8I
    // The following stmts are not needed as the union overlays the two elements
////lpSrc->at.aplHC8I.parts[0] = lpSrc->at.aplHC2I.parts[0];
////lpSrc->at.aplHC8I.parts[1] = lpSrc->at.aplHC2I.parts[1];
    lpSrc->at.aplHC8I.parts[2] = 0;
    lpSrc->at.aplHC8I.parts[3] = 0;
    lpSrc->at.aplHC8I.parts[4] = 0;
    lpSrc->at.aplHC8I.parts[5] = 0;
    lpSrc->at.aplHC8I.parts[6] = 0;
    lpSrc->at.aplHC8I.parts[7] = 0;

    lpSrc->chType = PN_NUMTYPE_HC8I;
} // End PN_actHC2I_HC8I


//***************************************************************************
//  $PN_actHC2I_HC2F
//***************************************************************************

void PN_actHC2I_HC2F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to FLT
    lpSrc->at.aplHC2F.parts[0] = (APLFLOAT) lpSrc->at.aplHC2I.parts[0];
    lpSrc->at.aplHC2F.parts[1] = (APLFLOAT) lpSrc->at.aplHC2I.parts[1];

    lpSrc->chType = PN_NUMTYPE_HC2F;
} // End PN_actHC2I_HC2F


//***************************************************************************
//  $PN_actHC2I_HC4F
//***************************************************************************

void PN_actHC2I_HC4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to FLT
    lpSrc->at.aplHC4F.parts[0] = (APLFLOAT) lpSrc->at.aplHC2I.parts[0];
    lpSrc->at.aplHC4F.parts[1] = (APLFLOAT) lpSrc->at.aplHC2I.parts[1];
    lpSrc->at.aplHC4F.parts[2] = 0;
    lpSrc->at.aplHC4F.parts[3] = 0;

    lpSrc->chType = PN_NUMTYPE_HC4F;
} // End PN_actHC2I_HC4F


//***************************************************************************
//  $PN_actHC2I_HC8F
//***************************************************************************

void PN_actHC2I_HC8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to FLT
    lpSrc->at.aplHC8F.parts[0] = (APLFLOAT) lpSrc->at.aplHC2I.parts[0];
    lpSrc->at.aplHC8F.parts[1] = (APLFLOAT) lpSrc->at.aplHC2I.parts[1];
    lpSrc->at.aplHC8F.parts[2] = 0;
    lpSrc->at.aplHC8F.parts[3] = 0;
    lpSrc->at.aplHC8F.parts[4] = 0;
    lpSrc->at.aplHC8F.parts[5] = 0;
    lpSrc->at.aplHC8F.parts[6] = 0;
    lpSrc->at.aplHC8F.parts[7] = 0;

    lpSrc->chType = PN_NUMTYPE_HC8F;
} // End PN_actINT_HC8F


//***************************************************************************
//  $PN_actHC2I_HC2R
//***************************************************************************

void PN_actHC2I_HC2R
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2I aplHC2I;

    // Make a temp copy of the HC2I as we overwrite it
    aplHC2I = lpSrc->at.aplHC2I;

    // Convert the real & imaginary parts from INT to RAT
    mpq_init_set_sx (&lpSrc->at.aplHC2R.parts[0], aplHC2I.parts[0], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC2R.parts[1], aplHC2I.parts[1], 1);

    lpSrc->chType = PN_NUMTYPE_HC2R;
} // End PN_actHC2I_HC2R


//***************************************************************************
//  $PN_actHC2I_HC4R
//***************************************************************************

void PN_actHC2I_HC4R
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2I aplHC2I;

    // Make a temp copy of the HC2I as we overwrite it
    aplHC2I = lpSrc->at.aplHC2I;

    // Convert the real & imaginary parts from INT to RAT
    mpq_init_set_sx (&lpSrc->at.aplHC4R.parts[0], aplHC2I.parts[0], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC4R.parts[1], aplHC2I.parts[1], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC4R.parts[2], 0               , 1);
    mpq_init_set_sx (&lpSrc->at.aplHC4R.parts[3], 0               , 1);

    lpSrc->chType = PN_NUMTYPE_HC4R;
} // End PN_actHC2I_HC4R


//***************************************************************************
//  $PN_actHC2I_HC8R
//***************************************************************************

void PN_actHC2I_HC8R
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2I aplHC2I;

    // Make a temp copy of the HC2I as we overwrite it
    aplHC2I = lpSrc->at.aplHC2I;

    // Convert the real & imaginary parts from INT to RAT
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[0], aplHC2I.parts[0], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[1], aplHC2I.parts[1], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[2], 0               , 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[3], 0               , 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[4], 0               , 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[5], 0               , 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[6], 0               , 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[7], 0               , 1);

    lpSrc->chType = PN_NUMTYPE_HC8R;
} // End PN_actHC2I_HC8R


//***************************************************************************
//  $PN_actHC2I_HC2V
//***************************************************************************

void PN_actHC2I_HC2V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2I aplHC2I;

    // Make a temp copy of the HC2I as we overwrite it
    aplHC2I = lpSrc->at.aplHC2I;

    // Convert the value from INT to VFP
    mpfr_init_set_sx (&lpSrc->at.aplHC2V.parts[0], aplHC2I.parts[0], MPFR_RNDN);
    mpfr_init_set_sx (&lpSrc->at.aplHC2V.parts[1], aplHC2I.parts[1], MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_HC2V;
} // End PN_actHC2I_HC2V


//***************************************************************************
//  $PN_actHC2I_HC4V
//***************************************************************************

void PN_actHC2I_HC4V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2I aplHC2I;

    // Make a temp copy of the HC2I as we overwrite it
    aplHC2I = lpSrc->at.aplHC2I;

    // Convert the value from INT to VFP
    mpfr_init_set_sx (&lpSrc->at.aplHC4V.parts[0], aplHC2I.parts[0], MPFR_RNDN);
    mpfr_init_set_sx (&lpSrc->at.aplHC4V.parts[1], aplHC2I.parts[1], MPFR_RNDN);
    mpfr_init0       (&lpSrc->at.aplHC4V.parts[2]);
    mpfr_init0       (&lpSrc->at.aplHC4V.parts[3]);

    lpSrc->chType = PN_NUMTYPE_HC4V;
} // End PN_actHC2I_HC4V


//***************************************************************************
//  $PN_actHC2I_HC8V
//***************************************************************************

void PN_actHC2I_HC8V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2I aplHC2I;

    // Make a temp copy of the HC2I as we overwrite it
    aplHC2I = lpSrc->at.aplHC2I;

    // Convert the value from INT to VFP
    mpfr_init_set_sx (&lpSrc->at.aplHC8V.parts[0], aplHC2I.parts[0], MPFR_RNDN);
    mpfr_init_set_sx (&lpSrc->at.aplHC8V.parts[1], aplHC2I.parts[1], MPFR_RNDN);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[2]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[3]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[4]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[5]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[6]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[7]);

    lpSrc->chType = PN_NUMTYPE_HC8V;
} // End PN_actHC2I_HC8V


//***************************************************************************
//  $PN_actHC2F_HC4F
//***************************************************************************

void PN_actHC2F_HC4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from FLT to HC4F
    // The following stmts are not needed as the union overlays the two elements
////lpSrc->at.aplHC4F.parts[0] = lpSrc->at.aplHC2F.parts[0];
////lpSrc->at.aplHC4F.parts[1] = lpSrc->at.aplHC2F.parts[1];
    lpSrc->at.aplHC4F.parts[2] = 0;
    lpSrc->at.aplHC4F.parts[3] = 0;

    lpSrc->chType = PN_NUMTYPE_HC4F;
} // End PN_actHC2F_HC4F


//***************************************************************************
//  $PN_actHC2F_HC8F
//***************************************************************************

void PN_actHC2F_HC8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from FLT to HC8F
    // The following stmts are not needed as the union overlays the two elements
////lpSrc->at.aplHC8F.parts[0] = lpSrc->at.aplHC2F.parts[0];
////lpSrc->at.aplHC8F.parts[1] = lpSrc->at.aplHC2F.parts[1];
    lpSrc->at.aplHC8F.parts[2] = 0;
    lpSrc->at.aplHC8F.parts[3] = 0;
    lpSrc->at.aplHC8F.parts[4] = 0;
    lpSrc->at.aplHC8F.parts[5] = 0;
    lpSrc->at.aplHC8F.parts[6] = 0;
    lpSrc->at.aplHC8F.parts[7] = 0;

    lpSrc->chType = PN_NUMTYPE_HC8F;
} // End PN_actHC2F_HC8F


//***************************************************************************
//  $PN_actHC2F_HC2V
//***************************************************************************

void PN_actHC2F_HC2V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2F aplHC2F;

    // ***TESTME*** -- When is this function ever called?

    // Make a temp copy of the HC2F as we overwrite it
    aplHC2F = lpSrc->at.aplHC2F;

    // Convert the value from FLT to VFP
    mpfr_init_set_d (&lpSrc->at.aplHC2V.parts[0], aplHC2F.parts[0], MPFR_RNDN);
    mpfr_init_set_d (&lpSrc->at.aplHC2V.parts[1], aplHC2F.parts[1], MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_HC2V;
} // End PN_actHC2F_HC2V


//***************************************************************************
//  $PN_actHC2F_HC4V
//***************************************************************************

void PN_actHC2F_HC4V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2F aplHC2F;

    // ***TESTME*** -- When is this function ever called?

    // Make a temp copy of the HC2F as we overwrite it
    aplHC2F = lpSrc->at.aplHC2F;

    // Convert the value from FLT to VFP
    mpfr_init_set_d (&lpSrc->at.aplHC4V.parts[0], aplHC2F.parts[0], MPFR_RNDN);
    mpfr_init_set_d (&lpSrc->at.aplHC4V.parts[1], aplHC2F.parts[1], MPFR_RNDN);
    mpfr_init0      (&lpSrc->at.aplHC4V.parts[2]);
    mpfr_init0      (&lpSrc->at.aplHC4V.parts[3]);

    lpSrc->chType = PN_NUMTYPE_HC4V;
} // End PN_actHC2F_HC4V


//***************************************************************************
//  $PN_actHC2F_HC8V
//***************************************************************************

void PN_actHC2F_HC8V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2F aplHC2F;

    // ***TESTME*** -- When is this function ever called?

    // Make a temp copy of the HC2F as we overwrite it
    aplHC2F = lpSrc->at.aplHC2F;

    // Convert the value from FLT to VFP
    mpfr_init_set_d (&lpSrc->at.aplHC8V.parts[0], aplHC2F.parts[0], MPFR_RNDN);
    mpfr_init_set_d (&lpSrc->at.aplHC8V.parts[1], aplHC2F.parts[1], MPFR_RNDN);
    mpfr_init0      (&lpSrc->at.aplHC8V.parts[2]);
    mpfr_init0      (&lpSrc->at.aplHC8V.parts[3]);

    lpSrc->chType = PN_NUMTYPE_HC8V;
} // End PN_actHC2F_HC8V


//***************************************************************************
//  $PN_actHC2R_HC4R
//***************************************************************************

void PN_actHC2R_HC4R
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Copy the RAT to the real part
    // The following stmts are not needed as the union overlays the two elements
////lpSrc->at.aplHC4R.parts[0] = lpSrc->at.aplHC2R.parts[0];
////lpSrc->at.aplHC4R.parts[1] = lpSrc->at.aplHC2R.parts[1];

    // Set the imaginary parts to 0/1
    mpq_init (&lpSrc->at.aplHC4R.parts[2]);
    mpq_init (&lpSrc->at.aplHC4R.parts[3]);

    lpSrc->chType = PN_NUMTYPE_HC4R;
} // End PN_actHC2R_HC4R


//***************************************************************************
//  $PN_actHC2R_HC8R
//***************************************************************************

void PN_actHC2R_HC8R
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Copy the RAT to the real part
    // The following stmts are not needed as the union overlays the two elements
////lpSrc->at.aplHC8R.parts[0] = lpSrc->at.aplHC2R.parts[0];
////lpSrc->at.aplHC8R.parts[1] = lpSrc->at.aplHC2R.parts[1];

    // Set the imaginary parts to 0/1
    mpq_init (&lpSrc->at.aplHC8R.parts[2]);
    mpq_init (&lpSrc->at.aplHC8R.parts[3]);
    mpq_init (&lpSrc->at.aplHC8R.parts[4]);
    mpq_init (&lpSrc->at.aplHC8R.parts[5]);
    mpq_init (&lpSrc->at.aplHC8R.parts[6]);
    mpq_init (&lpSrc->at.aplHC8R.parts[7]);

    lpSrc->chType = PN_NUMTYPE_HC8R;
} // End PN_actHC2R_HC8R


//**************************************************************************
//  $PN_actHC2R_HC2V
//***************************************************************************

void PN_actHC2R_HC2V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2R aplHC2R;

    // Make a temp copy of the HC2R as we overwrite it
    aplHC2R = lpSrc->at.aplHC2R;

    // Convert the value from RAT to VFP
    mpfr_init_set_q  (&lpSrc->at.aplHC2V.parts[0], &aplHC2R.parts[0], MPFR_RNDN);
    mpfr_init_set_q  (&lpSrc->at.aplHC2V.parts[1], &aplHC2R.parts[1], MPFR_RNDN);

    Myq_clear (&aplHC2R.parts[0]);
    Myq_clear (&aplHC2R.parts[1]);

    lpSrc->chType = PN_NUMTYPE_HC2V;
} // End PN_actHC2R_HC2V


//**************************************************************************
//  $PN_actHC2R_HC4V
//***************************************************************************

void PN_actHC2R_HC4V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2R aplHC2R;

    // Make a temp copy of the HC2R as we overwrite it
    aplHC2R = lpSrc->at.aplHC2R;

    // Convert the value from RAT to VFP
    mpfr_init_set_q  (&lpSrc->at.aplHC4V.parts[0], &aplHC2R.parts[0], MPFR_RNDN);
    mpfr_init_set_q  (&lpSrc->at.aplHC4V.parts[1], &aplHC2R.parts[1], MPFR_RNDN);
    mpfr_init0       (&lpSrc->at.aplHC4V.parts[2]);
    mpfr_init0       (&lpSrc->at.aplHC4V.parts[3]);

    Myq_clear (&aplHC2R.parts[0]);
    Myq_clear (&aplHC2R.parts[1]);

    lpSrc->chType = PN_NUMTYPE_HC4V;
} // End PN_actHC2R_HC4V


//**************************************************************************
//  $PN_actHC2R_HC8V
//***************************************************************************

void PN_actHC2R_HC8V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2R aplHC2R;

    // Make a temp copy of the HC2R as we overwrite it
    aplHC2R = lpSrc->at.aplHC2R;

    // Convert the value from RAT to VFP
    mpfr_init_set_q  (&lpSrc->at.aplHC8V.parts[0], &aplHC2R.parts[0], MPFR_RNDN);
    mpfr_init_set_q  (&lpSrc->at.aplHC8V.parts[1], &aplHC2R.parts[1], MPFR_RNDN);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[2]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[3]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[4]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[5]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[6]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[7]);

    Myq_clear (&aplHC2R.parts[0]);
    Myq_clear (&aplHC2R.parts[1]);

    lpSrc->chType = PN_NUMTYPE_HC8V;
} // End PN_actHC2R_HC8V


//***************************************************************************
//  $PN_actHC2V_HC4V
//***************************************************************************

void PN_actHC2V_HC4V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Copy the VFP to the real part
    // The following stmts are not needed as the union overlays the two elements
////lpSrc->at.aplHC4V.parts[0] = lpSrc->at.aplHC2V.parts[0];
////lpSrc->at.aplHC4V.parts[1] = lpSrc->at.aplHC2V.parts[1];

    // Set the imaginary part to 0/1
    mpfr_init0 (&lpSrc->at.aplHC4V.parts[2]);
    mpfr_init0 (&lpSrc->at.aplHC4V.parts[3]);

    lpSrc->chType = PN_NUMTYPE_HC4V;
} // End PN_actHC2V_HC4V


//***************************************************************************
//  $PN_actHC2V_HC8V
//***************************************************************************

void PN_actHC2V_HC8V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Copy the VFP to the real part
    // The following stmts are not needed as the union overlays the two elements
////lpSrc->at.aplHC8V.parts[0] = lpSrc->at.aplHC2V.parts[0];
////lpSrc->at.aplHC8V.parts[1] = lpSrc->at.aplHC2V.parts[1];

    // Set the imaginary part to 0/1
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[2]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[3]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[4]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[5]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[6]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[7]);

    lpSrc->chType = PN_NUMTYPE_HC8V;
} // End PN_actHC2V_HC8V


//***************************************************************************
//  $PN_actHC4I_HC8I
//***************************************************************************

void PN_actHC4I_HC8I
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from HC4I to HC8I
    // The following stmts are not needed as the union overlays the two elements
////lpSrc->at.aplHC8I.parts[0] = lpSrc->at.aplHC4I.parts[0];
////lpSrc->at.aplHC8I.parts[1] = lpSrc->at.aplHC4I.parts[1];
////lpSrc->at.aplHC8I.parts[2] = lpSrc->at.aplHC4I.parts[2];
////lpSrc->at.aplHC8I.parts[3] = lpSrc->at.aplHC4I.parts[3];
    lpSrc->at.aplHC8I.parts[4] = 0;
    lpSrc->at.aplHC8I.parts[5] = 0;
    lpSrc->at.aplHC8I.parts[6] = 0;
    lpSrc->at.aplHC8I.parts[7] = 0;

    lpSrc->chType = PN_NUMTYPE_HC8I;
} // End PN_actHC4I_HC8I


//***************************************************************************
//  $PN_actHC4I_HC4F
//***************************************************************************

void PN_actHC4I_HC4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to FLT
    lpSrc->at.aplHC4F.parts[0] = (APLFLOAT) lpSrc->at.aplHC4I.parts[0];
    lpSrc->at.aplHC4F.parts[1] = (APLFLOAT) lpSrc->at.aplHC4I.parts[1];
    lpSrc->at.aplHC4F.parts[2] = (APLFLOAT) lpSrc->at.aplHC4I.parts[2];
    lpSrc->at.aplHC4F.parts[3] = (APLFLOAT) lpSrc->at.aplHC4I.parts[3];

    lpSrc->chType = PN_NUMTYPE_HC4F;
} // End PN_actHC4I_HC4F


//***************************************************************************
//  $PN_actHC4I_HC8F
//***************************************************************************

void PN_actHC4I_HC8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to FLT
    lpSrc->at.aplHC8F.parts[0] = (APLFLOAT) lpSrc->at.aplHC4I.parts[0];
    lpSrc->at.aplHC8F.parts[1] = (APLFLOAT) lpSrc->at.aplHC4I.parts[1];
    lpSrc->at.aplHC8F.parts[2] = (APLFLOAT) lpSrc->at.aplHC4I.parts[2];
    lpSrc->at.aplHC8F.parts[3] = (APLFLOAT) lpSrc->at.aplHC4I.parts[3];
    lpSrc->at.aplHC8F.parts[4] = 0;
    lpSrc->at.aplHC8F.parts[5] = 0;
    lpSrc->at.aplHC8F.parts[6] = 0;
    lpSrc->at.aplHC8F.parts[7] = 0;

    lpSrc->chType = PN_NUMTYPE_HC8F;
} // End PN_actHC4I_HC8F


//***************************************************************************
//  $PN_actHC4I_HC4R
//***************************************************************************

void PN_actHC4I_HC4R
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC4I aplHC4I;

    // Make a temp copy of the HC4I as we overwrite it
    aplHC4I = lpSrc->at.aplHC4I;

    // Convert the real & imaginary parts from INT to RAT
    mpq_init_set_sx (&lpSrc->at.aplHC4R.parts[0], aplHC4I.parts[0], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC4R.parts[1], aplHC4I.parts[1], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC4R.parts[2], aplHC4I.parts[2], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC4R.parts[3], aplHC4I.parts[3], 1);

    lpSrc->chType = PN_NUMTYPE_HC4R;
} // End PN_actHC4I_HC4R


//***************************************************************************
//  $PN_actHC4I_HC8R
//***************************************************************************

void PN_actHC4I_HC8R
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC4I aplHC4I;

    // Make a temp copy of the HC4I as we overwrite it
    aplHC4I = lpSrc->at.aplHC4I;

    // Convert the real & imaginary parts from INT to RAT
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[0], aplHC4I.parts[0], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[1], aplHC4I.parts[1], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[2], aplHC4I.parts[2], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[3], aplHC4I.parts[3], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[4], 0               , 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[5], 0               , 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[6], 0               , 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[7], 0               , 1);

    lpSrc->chType = PN_NUMTYPE_HC8R;
} // End PN_actHC4I_HC8R


//***************************************************************************
//  $PN_actHC4I_HC4V
//***************************************************************************

void PN_actHC4I_HC4V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC4I aplHC4I;

    // Make a temp copy of the HC4I as we overwrite it
    aplHC4I = lpSrc->at.aplHC4I;

    // Convert the value from INT to VFP
    mpfr_init_set_sx (&lpSrc->at.aplHC4V.parts[0], aplHC4I.parts[0], MPFR_RNDN);
    mpfr_init_set_sx (&lpSrc->at.aplHC4V.parts[1], aplHC4I.parts[1], MPFR_RNDN);
    mpfr_init_set_sx (&lpSrc->at.aplHC4V.parts[2], aplHC4I.parts[2], MPFR_RNDN);
    mpfr_init_set_sx (&lpSrc->at.aplHC4V.parts[3], aplHC4I.parts[3], MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_HC4V;
} // End PN_actHC4I_HC4V


//***************************************************************************
//  $PN_actHC4I_HC8V
//***************************************************************************

void PN_actHC4I_HC8V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC4I aplHC4I;

    // Make a temp copy of the HC4I as we overwrite it
    aplHC4I = lpSrc->at.aplHC4I;

    // Convert the value from INT to VFP
    mpfr_init_set_sx (&lpSrc->at.aplHC8V.parts[0], aplHC4I.parts[0], MPFR_RNDN);
    mpfr_init_set_sx (&lpSrc->at.aplHC8V.parts[1], aplHC4I.parts[1], MPFR_RNDN);
    mpfr_init_set_sx (&lpSrc->at.aplHC8V.parts[2], aplHC4I.parts[2], MPFR_RNDN);
    mpfr_init_set_sx (&lpSrc->at.aplHC8V.parts[3], aplHC4I.parts[3], MPFR_RNDN);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[4]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[5]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[6]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[7]);

    lpSrc->chType = PN_NUMTYPE_HC8V;
} // End PN_actHC4I_HC8V


//***************************************************************************
//  $PN_actHC4F_HC8F
//***************************************************************************

void PN_actHC4F_HC8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from FLT to HC8F
    // The following stmts are not needed as the union overlays the two elements
////lpSrc->at.aplHC8F.parts[0] = lpSrc->at.aplHC4F.parts[0];
////lpSrc->at.aplHC8F.parts[1] = lpSrc->at.aplHC4F.parts[1];
////lpSrc->at.aplHC8F.parts[2] = lpSrc->at.aplHC4F.parts[2];
////lpSrc->at.aplHC8F.parts[3] = lpSrc->at.aplHC4F.parts[3];
    lpSrc->at.aplHC8F.parts[4] = 0;
    lpSrc->at.aplHC8F.parts[5] = 0;
    lpSrc->at.aplHC8F.parts[6] = 0;
    lpSrc->at.aplHC8F.parts[7] = 0;

    lpSrc->chType = PN_NUMTYPE_HC8F;
} // End PN_actHC4F_HC8F


//***************************************************************************
//  $PN_actHC4F_HC4V
//***************************************************************************

void PN_actHC4F_HC4V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC4F aplHC4F;

    // ***TESTME*** -- When is this function ever called?

    // Make a temp copy of the HC4F as we overwrite it
    aplHC4F = lpSrc->at.aplHC4F;

    // Convert the value from FLT to VFP
    mpfr_init_set_d (&lpSrc->at.aplHC4V.parts[0], aplHC4F.parts[0], MPFR_RNDN);
    mpfr_init_set_d (&lpSrc->at.aplHC4V.parts[1], aplHC4F.parts[1], MPFR_RNDN);
    mpfr_init_set_d (&lpSrc->at.aplHC4V.parts[2], aplHC4F.parts[2], MPFR_RNDN);
    mpfr_init_set_d (&lpSrc->at.aplHC4V.parts[3], aplHC4F.parts[3], MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_HC4V;
} // End PN_actHC4F_HC4V


//***************************************************************************
//  $PN_actHC4F_HC8V
//***************************************************************************

void PN_actHC4F_HC8V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC4F aplHC4F;

    // ***TESTME*** -- When is this function ever called?

    // Make a temp copy of the HC4F as we overwrite it
    aplHC4F = lpSrc->at.aplHC4F;

    // Convert the value from FLT to VFP
    mpfr_init_set_d (&lpSrc->at.aplHC8V.parts[0], aplHC4F.parts[0], MPFR_RNDN);
    mpfr_init_set_d (&lpSrc->at.aplHC8V.parts[1], aplHC4F.parts[1], MPFR_RNDN);
    mpfr_init_set_d (&lpSrc->at.aplHC8V.parts[2], aplHC4F.parts[2], MPFR_RNDN);
    mpfr_init_set_d (&lpSrc->at.aplHC8V.parts[3], aplHC4F.parts[3], MPFR_RNDN);
    mpfr_init0      (&lpSrc->at.aplHC8V.parts[4]);
    mpfr_init0      (&lpSrc->at.aplHC8V.parts[5]);
    mpfr_init0      (&lpSrc->at.aplHC8V.parts[6]);
    mpfr_init0      (&lpSrc->at.aplHC8V.parts[7]);

    lpSrc->chType = PN_NUMTYPE_HC8V;
} // End PN_actHC4F_HC8V


//**************************************************************************
//  $PN_actHC4R_HC4V
//***************************************************************************

void PN_actHC4R_HC4V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC4R aplHC4R;

    // Make a temp copy of the HC4R as we overwrite it
    aplHC4R = lpSrc->at.aplHC4R;

    // Convert the value from RAT to VFP
    mpfr_init_set_q  (&lpSrc->at.aplHC4V.parts[0], &aplHC4R.parts[0], MPFR_RNDN);
    mpfr_init_set_q  (&lpSrc->at.aplHC4V.parts[1], &aplHC4R.parts[1], MPFR_RNDN);
    mpfr_init_set_q  (&lpSrc->at.aplHC4V.parts[2], &aplHC4R.parts[2], MPFR_RNDN);
    mpfr_init_set_q  (&lpSrc->at.aplHC4V.parts[3], &aplHC4R.parts[3], MPFR_RNDN);

    Myq_clear (&aplHC4R.parts[0]);
    Myq_clear (&aplHC4R.parts[1]);
    Myq_clear (&aplHC4R.parts[2]);
    Myq_clear (&aplHC4R.parts[3]);

    lpSrc->chType = PN_NUMTYPE_HC4V;
} // End PN_actHC4R_HC4V


//***************************************************************************
//  $PN_actHC4R_HC8R
//***************************************************************************

void PN_actHC4R_HC8R
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Copy the RAT to the real part
    // The following stmts are not needed as the union overlays the two elements
////lpSrc->at.aplHC8R.parts[0] = lpSrc->at.aplHC4R.parts[0];
////lpSrc->at.aplHC8R.parts[1] = lpSrc->at.aplHC4R.parts[1];
////lpSrc->at.aplHC8R.parts[2] = lpSrc->at.aplHC4R.parts[2];
////lpSrc->at.aplHC8R.parts[3] = lpSrc->at.aplHC4R.parts[3];

    // Set the imaginary parts to 0/1
    mpq_init (&lpSrc->at.aplHC8R.parts[4]);
    mpq_init (&lpSrc->at.aplHC8R.parts[5]);
    mpq_init (&lpSrc->at.aplHC8R.parts[6]);
    mpq_init (&lpSrc->at.aplHC8R.parts[7]);

    lpSrc->chType = PN_NUMTYPE_HC8R;
} // End PN_actHC4R_HC8R


//**************************************************************************
//  $PN_actHC4R_HC8V
//***************************************************************************

void PN_actHC4R_HC8V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC4R aplHC4R;

    // Make a temp copy of the HC4R as we overwrite it
    aplHC4R = lpSrc->at.aplHC4R;

    // Convert the value from RAT to VFP
    mpfr_init_set_q  (&lpSrc->at.aplHC8V.parts[0], &aplHC4R.parts[0], MPFR_RNDN);
    mpfr_init_set_q  (&lpSrc->at.aplHC8V.parts[1], &aplHC4R.parts[1], MPFR_RNDN);
    mpfr_init_set_q  (&lpSrc->at.aplHC8V.parts[2], &aplHC4R.parts[2], MPFR_RNDN);
    mpfr_init_set_q  (&lpSrc->at.aplHC8V.parts[3], &aplHC4R.parts[3], MPFR_RNDN);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[4]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[5]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[6]);
    mpfr_init0       (&lpSrc->at.aplHC8V.parts[7]);

    Myq_clear (&aplHC4R.parts[0]);
    Myq_clear (&aplHC4R.parts[1]);
    Myq_clear (&aplHC4R.parts[2]);
    Myq_clear (&aplHC4R.parts[3]);

    lpSrc->chType = PN_NUMTYPE_HC8V;
} // End PN_actHC4R_HC8V


//***************************************************************************
//  $PN_actHC4V_HC8V
//***************************************************************************

void PN_actHC4V_HC8V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Copy the VFP to the real part
    // The following stmts are not needed as the union overlays the two elements
////lpSrc->at.aplHC8V.parts[0] = lpSrc->at.aplHC4V.parts[0];
////lpSrc->at.aplHC8V.parts[1] = lpSrc->at.aplHC4V.parts[1];
////lpSrc->at.aplHC8V.parts[2] = lpSrc->at.aplHC4V.parts[2];
////lpSrc->at.aplHC8V.parts[3] = lpSrc->at.aplHC4V.parts[3];

    // Set the imaginary part to 0/1
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[2]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[3]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[4]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[5]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[6]);
    mpfr_init0 (&lpSrc->at.aplHC8V.parts[7]);

    lpSrc->chType = PN_NUMTYPE_HC8V;
} // End PN_actHC4V_HC8V


//***************************************************************************
//  $PN_actHC8I_HC8F
//***************************************************************************

void PN_actHC8I_HC8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    // Convert the value from INT to FLT
    lpSrc->at.aplHC8F.parts[0] = (APLFLOAT) lpSrc->at.aplHC8I.parts[0];
    lpSrc->at.aplHC8F.parts[1] = (APLFLOAT) lpSrc->at.aplHC8I.parts[1];
    lpSrc->at.aplHC8F.parts[2] = (APLFLOAT) lpSrc->at.aplHC8I.parts[2];
    lpSrc->at.aplHC8F.parts[3] = (APLFLOAT) lpSrc->at.aplHC8I.parts[3];
    lpSrc->at.aplHC8F.parts[4] = (APLFLOAT) lpSrc->at.aplHC8I.parts[4];
    lpSrc->at.aplHC8F.parts[5] = (APLFLOAT) lpSrc->at.aplHC8I.parts[5];
    lpSrc->at.aplHC8F.parts[6] = (APLFLOAT) lpSrc->at.aplHC8I.parts[6];
    lpSrc->at.aplHC8F.parts[7] = (APLFLOAT) lpSrc->at.aplHC8I.parts[7];

    lpSrc->chType = PN_NUMTYPE_HC8F;
} // End PN_actHC8I_HC8F


//***************************************************************************
//  $PN_actHC8I_HC8R
//***************************************************************************

void PN_actHC8I_HC8R
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC8I aplHC8I;

    // Make a temp copy of the HC8I as we overwrite it
    aplHC8I = lpSrc->at.aplHC8I;

    // Convert the real & imaginary parts from INT to RAT
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[0], aplHC8I.parts[0], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[1], aplHC8I.parts[1], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[2], aplHC8I.parts[2], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[3], aplHC8I.parts[3], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[4], aplHC8I.parts[4], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[5], aplHC8I.parts[5], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[6], aplHC8I.parts[6], 1);
    mpq_init_set_sx (&lpSrc->at.aplHC8R.parts[7], aplHC8I.parts[7], 1);

    lpSrc->chType = PN_NUMTYPE_HC8R;
} // End PN_actHC8I_HC8R


//***************************************************************************
//  $PN_actHC8I_HC8V
//***************************************************************************

void PN_actHC8I_HC8V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC8I aplHC8I;

    // Make a temp copy of the HC8I as we overwrite it
    aplHC8I = lpSrc->at.aplHC8I;

    // Convert the value from INT to VFP
    mpfr_init_set_sx (&lpSrc->at.aplHC8V.parts[0], aplHC8I.parts[0], MPFR_RNDN);
    mpfr_init_set_sx (&lpSrc->at.aplHC8V.parts[1], aplHC8I.parts[1], MPFR_RNDN);
    mpfr_init_set_sx (&lpSrc->at.aplHC8V.parts[2], aplHC8I.parts[2], MPFR_RNDN);
    mpfr_init_set_sx (&lpSrc->at.aplHC8V.parts[3], aplHC8I.parts[3], MPFR_RNDN);
    mpfr_init_set_sx (&lpSrc->at.aplHC8V.parts[4], aplHC8I.parts[4], MPFR_RNDN);
    mpfr_init_set_sx (&lpSrc->at.aplHC8V.parts[5], aplHC8I.parts[5], MPFR_RNDN);
    mpfr_init_set_sx (&lpSrc->at.aplHC8V.parts[6], aplHC8I.parts[6], MPFR_RNDN);
    mpfr_init_set_sx (&lpSrc->at.aplHC8V.parts[7], aplHC8I.parts[7], MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_HC8V;
} // End PN_actHC8I_HC8V


//***************************************************************************
//  $PN_actHC8F_HC8V
//***************************************************************************

void PN_actHC8F_HC8V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC8F aplHC8F;

    // ***TESTME*** -- When is this function ever called?

    // Make a temp copy of the HC8F as we overwrite it
    aplHC8F = lpSrc->at.aplHC8F;

    // Convert the value from FLT to VFP
    mpfr_init_set_d (&lpSrc->at.aplHC8V.parts[0], aplHC8F.parts[0], MPFR_RNDN);
    mpfr_init_set_d (&lpSrc->at.aplHC8V.parts[1], aplHC8F.parts[1], MPFR_RNDN);
    mpfr_init_set_d (&lpSrc->at.aplHC8V.parts[2], aplHC8F.parts[2], MPFR_RNDN);
    mpfr_init_set_d (&lpSrc->at.aplHC8V.parts[3], aplHC8F.parts[3], MPFR_RNDN);
    mpfr_init_set_d (&lpSrc->at.aplHC8V.parts[4], aplHC8F.parts[4], MPFR_RNDN);
    mpfr_init_set_d (&lpSrc->at.aplHC8V.parts[5], aplHC8F.parts[5], MPFR_RNDN);
    mpfr_init_set_d (&lpSrc->at.aplHC8V.parts[6], aplHC8F.parts[6], MPFR_RNDN);
    mpfr_init_set_d (&lpSrc->at.aplHC8V.parts[7], aplHC8F.parts[7], MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_HC8V;
} // End PN_actHC8F_HC8V


//**************************************************************************
//  $PN_actHC8R_HC8V
//***************************************************************************

void PN_actHC8R_HC8V
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC8R aplHC8R;

    // Make a temp copy of the HC8R as we overwrite it
    aplHC8R = lpSrc->at.aplHC8R;

    // Convert the value from RAT to VFP
    mpfr_init_set_q  (&lpSrc->at.aplHC8V.parts[0], &aplHC8R.parts[0], MPFR_RNDN);
    mpfr_init_set_q  (&lpSrc->at.aplHC8V.parts[1], &aplHC8R.parts[1], MPFR_RNDN);
    mpfr_init_set_q  (&lpSrc->at.aplHC8V.parts[2], &aplHC8R.parts[2], MPFR_RNDN);
    mpfr_init_set_q  (&lpSrc->at.aplHC8V.parts[3], &aplHC8R.parts[3], MPFR_RNDN);
    mpfr_init_set_q  (&lpSrc->at.aplHC8V.parts[4], &aplHC8R.parts[4], MPFR_RNDN);
    mpfr_init_set_q  (&lpSrc->at.aplHC8V.parts[5], &aplHC8R.parts[5], MPFR_RNDN);
    mpfr_init_set_q  (&lpSrc->at.aplHC8V.parts[6], &aplHC8R.parts[6], MPFR_RNDN);
    mpfr_init_set_q  (&lpSrc->at.aplHC8V.parts[7], &aplHC8R.parts[7], MPFR_RNDN);

    Myq_clear (&aplHC8R.parts[0]);
    Myq_clear (&aplHC8R.parts[1]);
    Myq_clear (&aplHC8R.parts[2]);
    Myq_clear (&aplHC8R.parts[3]);
    Myq_clear (&aplHC8R.parts[4]);
    Myq_clear (&aplHC8R.parts[5]);
    Myq_clear (&aplHC8R.parts[6]);
    Myq_clear (&aplHC8R.parts[7]);

    lpSrc->chType = PN_NUMTYPE_HC8V;
} // End PN_actHC8R_HC8V


//***************************************************************************
//  $PN_actINT_BA1F
//***************************************************************************

void PN_actINT_BA1F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLINT aplInt;

    // Make a temp copy of the INT as we overwrite it
    aplInt = lpSrc->at.aplInteger;

    // Initialize the temp
    Myarb_init (&lpSrc->at.aplBA1F);

    // Set the INT into the BA1F
    arb_set_sx (&lpSrc->at.aplBA1F,
                 aplInt);
    lpSrc->chType = PN_NUMTYPE_BA1F;
} // End PN_actINT_BA1F


//***************************************************************************
//  $PN_actINT_BA2F
//***************************************************************************

void PN_actINT_BA2F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLINT aplInt;

    // Make a temp copy of the INT as we overwrite it
    aplInt = lpSrc->at.aplInteger;

    // Initialize the result
    arb2f_init (&lpSrc->at.aplBA2F);

    // Set the INT into the BA1F
    arb_set_sx (&lpSrc->at.aplBA1F,
                 aplInt);
    lpSrc->chType = PN_NUMTYPE_BA2F;
} // End PN_actINT_BA2F


//***************************************************************************
//  $PN_actINT_BA4F
//***************************************************************************

void PN_actINT_BA4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLINT aplInt;

    // Make a temp copy of the INT as we overwrite it
    aplInt = lpSrc->at.aplInteger;

    // Initialize the result
    arb4f_init (&lpSrc->at.aplBA4F);

    // Set the INT into the BA1F
    arb_set_sx (&lpSrc->at.aplBA1F,
                 aplInt);
    lpSrc->chType = PN_NUMTYPE_BA4F;
} // End PN_actINT_BA4F


//***************************************************************************
//  $PN_actINT_BA8F
//***************************************************************************

void PN_actINT_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLINT aplInt;

    // Make a temp copy of the INT as we overwrite it
    aplInt = lpSrc->at.aplInteger;

    // Initialize the result
    arb8f_init (&lpSrc->at.aplBA8F);

    // Set the INT into the BA1F
    arb_set_sx (&lpSrc->at.aplBA1F,
                 aplInt);
    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actINT_BA8F


//***************************************************************************
//  $PN_actFLT_BA1F
//***************************************************************************

void PN_actFLT_BA1F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Initialize to 0/0
    Myarb_init (&lpSrc->at.aplArb);

    // Convert the FLT string to an ARB
    arb_set_str2 (&lpSrc->at.aplArb,                            // Ptr to destin ARB
                  &lppnLocalVars->lpszStart[lpSrc->uNumStart],  // Ptr to source FLT string
                   prec);
    lpSrc->chType = PN_NUMTYPE_BA1F;
} // End PN_actFLT_BA1F


//***************************************************************************
//  $PN_actFLT_BA2F
//***************************************************************************

void PN_actFLT_BA2F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Initialize to 0/0
    arb2f_init (&lpSrc->at.aplBA2F);

    // Convert the FLT string to an ARB
    arb_set_str2 (&lpSrc->at.aplArb,                            // Ptr to destin ARB
                  &lppnLocalVars->lpszStart[lpSrc->uNumStart],  // Ptr to source FLT string
                   prec);
    lpSrc->chType = PN_NUMTYPE_BA2F;
} // End PN_actFLT_BA2F


//***************************************************************************
//  $PN_actFLT_BA4F
//***************************************************************************

void PN_actFLT_BA4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Initialize to 0/0
    arb4f_init (&lpSrc->at.aplBA4F);

    // Convert the FLT string to an ARB
    arb_set_str2 (&lpSrc->at.aplArb,                            // Ptr to destin ARB
                  &lppnLocalVars->lpszStart[lpSrc->uNumStart],  // Ptr to source FLT string
                   prec);
    lpSrc->chType = PN_NUMTYPE_BA4F;
} // End PN_actFLT_BA4F


//***************************************************************************
//  $PN_actFLT_BA8F
//***************************************************************************

void PN_actFLT_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Initialize to 0/0
    arb8f_init (&lpSrc->at.aplBA8F);

    // Convert the FLT string to an ARB
    arb_set_str2 (&lpSrc->at.aplArb,                            // Ptr to destin ARB
                  &lppnLocalVars->lpszStart[lpSrc->uNumStart],  // Ptr to source FLT string
                   prec);
    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actFLT_BA8F


//***************************************************************************
//  $PN_actRAT_BA1F
//***************************************************************************

void PN_actRAT_BA1F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLRAT aplRat;
    fmpq_t fmpq;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Make a temp copy of the RAT as we overwrite it
    aplRat = lpSrc->at.aplRat;

    // Initialize the temps
    Myarb_init (&lpSrc->at.aplBA1F);
    fmpq_init (fmpq);

    // Convert the value from RAT to FMPQ
    fmpq_set_mpq (fmpq, &aplRat);

    // Convert the value from FMPQ to BA1F
    arb_set_fmpq (&lpSrc->at.aplBA1F, fmpq, prec);

    // Free the temps
    fmpq_clear (fmpq);
    mpq_clear (&aplRat);

    lpSrc->chType = PN_NUMTYPE_BA1F;
} // End PN_actRAT_BA1F


//***************************************************************************
//  $PN_actRAT_BA2F
//***************************************************************************

void PN_actRAT_BA2F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLRAT aplRat;
    fmpq_t fmpq;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Make a temp copy of the RAT as we overwrite it
    aplRat = lpSrc->at.aplRat;

    // Initialize the temps
    arb2f_init (&lpSrc->at.aplBA2F);
    fmpq_init (fmpq);

    // Convert the value from RAT to FMPQ
    fmpq_set_mpq (fmpq, &aplRat);

    // Convert the value from FMPQ to BA2F
    arb_set_fmpq (&lpSrc->at.aplBA1F, fmpq, prec);

    // Free the temps
    fmpq_clear (fmpq);
    mpq_clear (&aplRat);

    lpSrc->chType = PN_NUMTYPE_BA2F;
} // End PN_actRAT_BA2F


//***************************************************************************
//  $PN_actRAT_BA4F
//***************************************************************************

void PN_actRAT_BA4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLRAT aplRat;
    fmpq_t fmpq;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Make a temp copy of the RAT as we overwrite it
    aplRat = lpSrc->at.aplRat;

    // Initialize the temps
    arb4f_init (&lpSrc->at.aplBA4F);
    fmpq_init (fmpq);

    // Convert the value from RAT to FMPQ
    fmpq_set_mpq (fmpq, &aplRat);

    // Convert the value from FMPQ to BA4F
    arb_set_fmpq (&lpSrc->at.aplBA1F, fmpq, prec);

    // Free the temps
    fmpq_clear (fmpq);
    mpq_clear (&aplRat);

    lpSrc->chType = PN_NUMTYPE_BA4F;
} // End PN_actRAT_BA4F


//***************************************************************************
//  $PN_actRAT_BA8F
//***************************************************************************

void PN_actRAT_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLRAT aplRat;
    fmpq_t fmpq;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Make a temp copy of the RAT as we overwrite it
    aplRat = lpSrc->at.aplRat;

    // Initialize the temps
    arb8f_init (&lpSrc->at.aplBA8F);
    fmpq_init (fmpq);

    // Convert the value from RAT to FMPQ
    fmpq_set_mpq (fmpq, &aplRat);

    // Convert the value from FMPQ to BA8F
    arb_set_fmpq (&lpSrc->at.aplBA1F, fmpq, prec);

    // Free the temps
    fmpq_clear (fmpq);
    mpq_clear (&aplRat);

    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actRAT_BA8F


//***************************************************************************
//  $PN_actVFP_BA1F
//***************************************************************************

void PN_actVFP_BA1F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLVFP aplVfp;

    // Make a temp copy of the VFP as we overwrite it
    aplVfp = lpSrc->at.aplVfp;

    // Initialize the temps
    Myarb_init (&lpSrc->at.aplBA1F);

    // Convert the value from VFP to BA1F
    arb_set_mpfr (&lpSrc->at.aplBA1F, &aplVfp);

    // Free the temps
    mpfr_clear (&aplVfp);

    lpSrc->chType = PN_NUMTYPE_BA1F;
} // End PN_actVFP_BA1F


//***************************************************************************
//  $PN_actVFP_BA2F
//***************************************************************************

void PN_actVFP_BA2F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLVFP aplVfp;

    // Make a temp copy of the VFP as we overwrite it
    aplVfp = lpSrc->at.aplVfp;

    // Initialize the temps
    arb2f_init (&lpSrc->at.aplBA2F);

    // Convert the value from VFP to BA2F
    arb_set_mpfr (&lpSrc->at.aplBA1F, &aplVfp);

    // Free the temps
    mpfr_clear (&aplVfp);

    lpSrc->chType = PN_NUMTYPE_BA2F;
} // End PN_actVFP_BA2F


//***************************************************************************
//  $PN_actVFP_BA4F
//***************************************************************************

void PN_actVFP_BA4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLVFP aplVfp;

    // Make a temp copy of the VFP as we overwrite it
    aplVfp = lpSrc->at.aplVfp;

    // Initialize the temps
    arb4f_init (&lpSrc->at.aplBA4F);

    // Convert the value from VFP to BA4F
    arb_set_mpfr (&lpSrc->at.aplBA1F, &aplVfp);

    // Free the temps
    mpfr_clear (&aplVfp);

    lpSrc->chType = PN_NUMTYPE_BA4F;
} // End PN_actVFP_BA4F


//***************************************************************************
//  $PN_actVFP_BA8F
//***************************************************************************

void PN_actVFP_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLVFP aplVfp;

    // Make a temp copy of the VFP as we overwrite it
    aplVfp = lpSrc->at.aplVfp;

    // Initialize the temps
    arb8f_init (&lpSrc->at.aplBA8F);

    // Convert the value from VFP to BA8F
    arb_set_mpfr (&lpSrc->at.aplBA1F, &aplVfp);

    // Free the temps
    mpfr_clear (&aplVfp);

    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actVFP_BA8F


//***************************************************************************
//  $PN_actHC2I_BA2F
//***************************************************************************

void PN_actHC2I_BA2F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2I          aplInt;
    APLRAT           aplRat = {0};
    fmpq_t           fmpq;
    int              i;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Make a temp copy of the HC2I as we overwrite it
    aplInt = lpSrc->at.aplHC2I;

    // Initialize the temps
    arbX_init (&lpSrc->at.aplBA1F, 2);
    mpq_init (&aplRat);
    fmpq_init (fmpq);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Convert the value from INT to RAT
        mpq_set_sx (&aplRat, aplInt.parts[i], 1);

        // Convert the value from RAT to FMPQ
        fmpq_set_mpq (fmpq, &aplRat);

        // Convert the value from FMPQ to BA2F
        arb_set_fmpq (&lpSrc->at.aplBA8F.parts[i], fmpq, prec);
    } // End FOR

    // Free the temps
    fmpq_clear (fmpq);
    mpq_clear (&aplRat);

    lpSrc->chType = PN_NUMTYPE_BA2F;
} // End PN_actHC2I_BA2F


//***************************************************************************
//  $PN_actHC2I_BA4F
//***************************************************************************

void PN_actHC2I_BA4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2I          aplInt;
    APLRAT           aplRat = {0};
    fmpq_t           fmpq;
    int              i;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Make a temp copy of the HC2I as we overwrite it
    aplInt = lpSrc->at.aplHC2I;

    // Initialize the temps
    arbX_init (&lpSrc->at.aplBA1F, 4);
    mpq_init (&aplRat);
    fmpq_init (fmpq);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Convert the value from INT to RAT
        mpq_set_sx (&aplRat, aplInt.parts[i], 1);

        // Convert the value from RAT to FMPQ
        fmpq_set_mpq (fmpq, &aplRat);

        // Convert the value from FMPQ to BA2F
        arb_set_fmpq (&lpSrc->at.aplBA8F.parts[i], fmpq, prec);
    } // End FOR

    // Free the temps
    fmpq_clear (fmpq);
    mpq_clear (&aplRat);

    lpSrc->chType = PN_NUMTYPE_BA4F;
} // End PN_actHC2I_BA4F


//***************************************************************************
//  $PN_actHC2I_BA8F
//***************************************************************************

void PN_actHC2I_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2I          aplInt;
    APLRAT           aplRat = {0};
    fmpq_t           fmpq;
    int              i;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Make a temp copy of the HC2I as we overwrite it
    aplInt = lpSrc->at.aplHC2I;

    // Initialize the temps
    arbX_init (&lpSrc->at.aplBA1F, 8);
    mpq_init (&aplRat);
    fmpq_init (fmpq);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Convert the value from INT to RAT
        mpq_set_sx (&aplRat, aplInt.parts[i], 1);

        // Convert the value from RAT to FMPQ
        fmpq_set_mpq (fmpq, &aplRat);

        // Convert the value from FMPQ to BA2F
        arb_set_fmpq (&lpSrc->at.aplBA8F.parts[i], fmpq, prec);
    } // End FOR

    // Free the temps
    fmpq_clear (fmpq);
    mpq_clear (&aplRat);

    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actHC2I_BA8F


//***************************************************************************
//  $PN_actHC2F_BA2F
//***************************************************************************

void PN_actHC2F_BA2F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2F aplFlt;
    int     i;

    // Make a temp copy of the HC2F as we overwrite it
    aplFlt = lpSrc->at.aplHC2F;

    // Initialize the temps
    arbX_init (&lpSrc->at.aplBA1F, 2);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Convert the value from FLT to BA2F
        arb_set_d (&lpSrc->at.aplBA8F.parts[i], aplFlt.parts[i]);

    lpSrc->chType = PN_NUMTYPE_BA2F;
} // End PN_actHC2F_BA2F


//***************************************************************************
//  $PN_actHC2R_BA2F
//***************************************************************************

void PN_actHC2R_BA2F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2R          aplRat;
    fmpq_t           fmpq;
    int              i;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Make a temp copy of the HC2R as we overwrite it
    aplRat = lpSrc->at.aplHC2R;

    // Initialize the temps
    ZeroMemory (&lpSrc->at.aplBA2F, sizeof (lpSrc->at.aplBA2F));
    arbX_init (&lpSrc->at.aplBA1F, 2);
    fmpq_init (fmpq);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Convert the value from RAT to FMPQ
        fmpq_set_mpq (fmpq, &aplRat.parts[i]);

        // Convert the value from FMPQ to BA2F
        arb_set_fmpq (&lpSrc->at.aplBA8F.parts[i], fmpq, prec);
    } // End FOR

    // Free the temps
    fmpq_clear (fmpq);
    MyhcXr_clear (&aplRat, 2);

    lpSrc->chType = PN_NUMTYPE_BA2F;
} // End PN_actHC2R_BA2F


//***************************************************************************
//  $PN_actHC2V_BA2F
//***************************************************************************

void PN_actHC2V_BA2F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2V aplVfp;
    int     i;

    // Make a temp copy of the HC2V as we overwrite it
    aplVfp = lpSrc->at.aplHC2V;

    // Initialize the temps
    ZeroMemory (&lpSrc->at.aplBA2F, sizeof (lpSrc->at.aplBA2F));
    arbX_init (&lpSrc->at.aplBA1F, 2);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Convert the value from VFP to BA2F
        arb_set_mpfr (&lpSrc->at.aplBA8F.parts[i], &aplVfp.parts[i]);

    // Free the temps
    MyhcXv_clear (&aplVfp, 2);

    lpSrc->chType = PN_NUMTYPE_BA2F;
} // End PN_actHC2V_BA2F


//***************************************************************************
//  $PN_actHC2F_BA4F
//***************************************************************************

void PN_actHC2F_BA4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2F aplFlt;
    int     i;

    // Make a temp copy of the HC2F as we overwrite it
    aplFlt = lpSrc->at.aplHC2F;

    // Initialize the temps
    arbX_init (&lpSrc->at.aplBA1F, 4);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Convert the value from FLT to BA4F
        arb_set_d (&lpSrc->at.aplBA8F.parts[i], aplFlt.parts[i]);

    lpSrc->chType = PN_NUMTYPE_BA4F;
} // End PN_actHC2F_BA4F


//***************************************************************************
//  $PN_actHC2R_BA4F
//***************************************************************************

void PN_actHC2R_BA4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2R          aplRat;
    fmpq_t           fmpq;
    int              i;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Make a temp copy of the HC2R as we overwrite it
    aplRat = lpSrc->at.aplHC2R;

    // Initialize the temps
    ZeroMemory (&lpSrc->at.aplBA2F, sizeof (lpSrc->at.aplBA2F));
    arbX_init (&lpSrc->at.aplBA1F, 4);
    fmpq_init (fmpq);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Convert the value from RAT to FMPQ
        fmpq_set_mpq (fmpq, &aplRat.parts[i]);

        // Convert the value from FMPQ to BA2F
        arb_set_fmpq (&lpSrc->at.aplBA8F.parts[i], fmpq, prec);
    } // End FOR

    // Free the temps
    fmpq_clear (fmpq);
    MyhcXr_clear (&aplRat, 2);

    lpSrc->chType = PN_NUMTYPE_BA4F;
} // End PN_actHC2R_BA4F


//***************************************************************************
//  $PN_actHC2V_BA4F
//***************************************************************************

void PN_actHC2V_BA4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2V aplVfp;
    int     i;

    // Make a temp copy of the HC2V as we overwrite it
    aplVfp = lpSrc->at.aplHC2V;

    // Initialize the temps
    ZeroMemory (&lpSrc->at.aplBA2F, sizeof (lpSrc->at.aplBA2F));
    arbX_init (&lpSrc->at.aplBA1F, 4);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Convert the value from VFP to BA4F
        arb_set_mpfr (&lpSrc->at.aplBA8F.parts[i], &aplVfp.parts[i]);

    // Free the temps
    MyhcXv_clear (&aplVfp, 2);

    lpSrc->chType = PN_NUMTYPE_BA4F;
} // End PN_actHC2V_BA4F


//***************************************************************************
//  $PN_actHC2F_BA8F
//***************************************************************************

void PN_actHC2F_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2F aplFlt;
    int     i;

    // Make a temp copy of the HC2F as we overwrite it
    aplFlt = lpSrc->at.aplHC2F;

    // Initialize the temps
    arbX_init (&lpSrc->at.aplBA1F, 8);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Convert the value from FLT to BA8F
        arb_set_d (&lpSrc->at.aplBA8F.parts[i], aplFlt.parts[i]);

    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actHC2F_BA8F


//***************************************************************************
//  $PN_actHC2R_BA8F
//***************************************************************************

void PN_actHC2R_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2R          aplRat;
    fmpq_t           fmpq;
    int              i;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Make a temp copy of the HC2R as we overwrite it
    aplRat = lpSrc->at.aplHC2R;

    // Initialize the temps
    ZeroMemory (&lpSrc->at.aplBA2F, sizeof (lpSrc->at.aplBA2F));
    arbX_init (&lpSrc->at.aplBA1F, 8);
    fmpq_init (fmpq);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Convert the value from RAT to FMPQ
        fmpq_set_mpq (fmpq, &aplRat.parts[i]);

        // Convert the value from FMPQ to BA2F
        arb_set_fmpq (&lpSrc->at.aplBA8F.parts[i], fmpq, prec);
    } // End FOR

    // Free the temps
    fmpq_clear (fmpq);
    MyhcXr_clear (&aplRat, 2);

    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actHC2R_BA8F


//***************************************************************************
//  $PN_actHC2V_BA8F
//***************************************************************************

void PN_actHC2V_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC2V aplVfp;
    int     i;

    // Make a temp copy of the HC2V as we overwrite it
    aplVfp = lpSrc->at.aplHC2V;

    // Initialize the temps
    ZeroMemory (&lpSrc->at.aplBA2F, sizeof (lpSrc->at.aplBA2F));
    arbX_init (&lpSrc->at.aplBA1F, 8);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Convert the value from VFP to BA8F
        arb_set_mpfr (&lpSrc->at.aplBA8F.parts[i], &aplVfp.parts[i]);

    // Free the temps
    MyhcXv_clear (&aplVfp, 2);

    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actHC2V_BA8F


//***************************************************************************
//  $PN_actHC4R_BA4F
//***************************************************************************

void PN_actHC4R_BA4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC4R          aplRat;
    fmpq_t           fmpq;
    int              i;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Make a temp copy of the HC4R as we overwrite it
    aplRat = lpSrc->at.aplHC4R;

    // Initialize the temps
    ZeroMemory (&lpSrc->at.aplBA4F, sizeof (lpSrc->at.aplBA4F));
    arbX_init (&lpSrc->at.aplBA1F, 4);
    fmpq_init (fmpq);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Convert the value from RAT to FMPQ
        fmpq_set_mpq (fmpq, &aplRat.parts[i]);

        // Convert the value from FMPQ to BA4F
        arb_set_fmpq (&lpSrc->at.aplBA8F.parts[i], fmpq, prec);
    } // End FOR

    // Free the temps
    fmpq_clear (fmpq);
    MyhcXr_clear (&aplRat, 4);

    lpSrc->chType = PN_NUMTYPE_BA4F;
} // End PN_actHC4R_BA4F


//***************************************************************************
//  $PN_actHC4V_BA4F
//***************************************************************************

void PN_actHC4V_BA4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC4V aplVfp;
    int     i;

    // Make a temp copy of the HC4V as we overwrite it
    aplVfp = lpSrc->at.aplHC4V;

    // Initialize the temps
    ZeroMemory (&lpSrc->at.aplBA4F, sizeof (lpSrc->at.aplBA4F));
    arbX_init (&lpSrc->at.aplBA1F, 4);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Convert the value from VFP to BA4F
        arb_set_mpfr (&lpSrc->at.aplBA8F.parts[i], &aplVfp.parts[i]);

    // Free the temps
    MyhcXv_clear (&aplVfp, 4);

    lpSrc->chType = PN_NUMTYPE_BA4F;
} // End PN_actHC4V_BA4F


//***************************************************************************
//  $PN_actHC4R_BA8F
//***************************************************************************

void PN_actHC4R_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC4R          aplRat;
    fmpq_t           fmpq;
    int              i;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Make a temp copy of the HC4R as we overwrite it
    aplRat = lpSrc->at.aplHC4R;

    // Initialize the temps
    ZeroMemory (&lpSrc->at.aplBA4F, sizeof (lpSrc->at.aplBA4F));
    arbX_init (&lpSrc->at.aplBA1F, 8);
    fmpq_init (fmpq);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Convert the value from RAT to FMPQ
        fmpq_set_mpq (fmpq, &aplRat.parts[i]);

        // Convert the value from FMPQ to BA8F
        arb_set_fmpq (&lpSrc->at.aplBA8F.parts[i], fmpq, prec);
    } // End FOR

    // Free the temps
    fmpq_clear (fmpq);
    MyhcXr_clear (&aplRat, 4);

    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actHC4R_BA8F


//***************************************************************************
//  $PN_actHC4V_BA8F
//***************************************************************************

void PN_actHC4V_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC4V aplVfp;
    int     i;

    // Make a temp copy of the HC4V as we overwrite it
    aplVfp = lpSrc->at.aplHC4V;

    // Initialize the temps
    ZeroMemory (&lpSrc->at.aplBA4F, sizeof (lpSrc->at.aplBA4F));
    arbX_init (&lpSrc->at.aplBA1F, 8);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Convert the value from VFP to BA8F
        arb_set_mpfr (&lpSrc->at.aplBA8F.parts[i], &aplVfp.parts[i]);

    // Free the temps
    MyhcXv_clear (&aplVfp, 4);

    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actHC4V_BA8F


//***************************************************************************
//  $PN_actHC8R_BA8F
//***************************************************************************

void PN_actHC8R_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC8R          aplRat;
    fmpq_t           fmpq;
    int              i;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Make a temp copy of the HC8R as we overwrite it
    aplRat = lpSrc->at.aplHC8R;

    // Initialize the temps
    ZeroMemory (&lpSrc->at.aplBA8F, sizeof (lpSrc->at.aplBA8F));
    arbX_init (&lpSrc->at.aplBA1F, 8);
    fmpq_init (fmpq);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    {
        // Convert the value from RAT to FMPQ
        fmpq_set_mpq (fmpq, &aplRat.parts[i]);

        // Convert the value from FMPQ to BA8F
        arb_set_fmpq (&lpSrc->at.aplBA8F.parts[i], fmpq, prec);
    } // End FOR

    // Free the temps
    fmpq_clear (fmpq);
    MyhcXr_clear (&aplRat, 8);

    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actHC8R_BA8F


//***************************************************************************
//  $PN_actHC8V_BA8F
//***************************************************************************

void PN_actHC8V_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC8V aplVfp;
    int     i;

    // Make a temp copy of the HC8V as we overwrite it
    aplVfp = lpSrc->at.aplHC8V;

    // Initialize the temps
    ZeroMemory (&lpSrc->at.aplBA8F, sizeof (lpSrc->at.aplBA8F));
    arbX_init (&lpSrc->at.aplBA1F, 8);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Convert the value from VFP to BA8F
        arb_set_mpfr (&lpSrc->at.aplBA8F.parts[i], &aplVfp.parts[i]);

    // Free the temps
    MyhcXv_clear (&aplVfp, 8);

    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actHC8V_BA8F


//***************************************************************************
//  $PN_actHC4I_BA4F
//***************************************************************************

void PN_actHC4I_BA4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC4I          aplInt;
    APLRAT           aplRat = {0};
    fmpq_t           fmpq;
    int              i;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Make a temp copy of the HC4I as we overwrite it
    aplInt = lpSrc->at.aplHC4I;

    // Initialize the temps
    arbX_init (&lpSrc->at.aplBA1F, 4);
    mpq_init (&aplRat);
    fmpq_init (fmpq);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Convert the value from INT to RAT
        mpq_set_sx (&aplRat, aplInt.parts[i], 1);

        // Convert the value from RAT to FMPQ
        fmpq_set_mpq (fmpq, &aplRat);

        // Convert the value from FMPQ to BA4F
        arb_set_fmpq (&lpSrc->at.aplBA8F.parts[i], fmpq, prec);
    } // End FOR

    // Free the temps
    fmpq_clear (fmpq);
    mpq_clear (&aplRat);

    lpSrc->chType = PN_NUMTYPE_BA4F;
} // End PN_actHC4I_BA4F


//***************************************************************************
//  $PN_actHC4F_BA4F
//***************************************************************************

void PN_actHC4F_BA4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC4F aplFlt;
    int     i;

    // Make a temp copy of the HC4F as we overwrite it
    aplFlt = lpSrc->at.aplHC4F;

    // Initialize the temps
    arbX_init (&lpSrc->at.aplBA1F, 4);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Convert the value from FLT to BA4F
        arb_set_d (&lpSrc->at.aplBA8F.parts[i], aplFlt.parts[i]);

    lpSrc->chType = PN_NUMTYPE_BA4F;
} // End PN_actHC4F_BA4F


//***************************************************************************
//  $PN_actHC4I_BA8F
//***************************************************************************

void PN_actHC4I_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC4I          aplInt;
    APLRAT           aplRat = {0};
    fmpq_t           fmpq;
    int              i;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Make a temp copy of the HC4I as we overwrite it
    aplInt = lpSrc->at.aplHC4I;

    // Initialize the temps
    arbX_init (&lpSrc->at.aplBA1F, 8);
    mpq_init (&aplRat);
    fmpq_init (fmpq);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Convert the value from INT to RAT
        mpq_set_sx (&aplRat, aplInt.parts[i], 1);

        // Convert the value from RAT to FMPQ
        fmpq_set_mpq (fmpq, &aplRat);

        // Convert the value from FMPQ to BA8F
        arb_set_fmpq (&lpSrc->at.aplBA8F.parts[i], fmpq, prec);
    } // End FOR

    // Free the temps
    fmpq_clear (fmpq);
    mpq_clear (&aplRat);

    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actHC4I_BA8F


//***************************************************************************
//  $PN_actHC4F_BA8F
//***************************************************************************

void PN_actHC4F_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC4F aplFlt;
    int     i;

    // Make a temp copy of the HC4F as we overwrite it
    aplFlt = lpSrc->at.aplHC4F;

    // Initialize the temps
    arbX_init (&lpSrc->at.aplBA1F, 8);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Convert the value from FLT to BA8F
        arb_set_d (&lpSrc->at.aplBA8F.parts[i], aplFlt.parts[i]);

    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actHC4F_BA8F


//***************************************************************************
//  $PN_actHC8I_BA8F
//***************************************************************************

void PN_actHC8I_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC8I          aplInt;
    APLRAT           aplRat = {0};
    fmpq_t           fmpq;
    int              i;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Make a temp copy of the HC8I as we overwrite it
    aplInt = lpSrc->at.aplHC8I;

    // Initialize the temps
    arbX_init (&lpSrc->at.aplBA1F, 8);
    mpq_init (&aplRat);
    fmpq_init (fmpq);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    {
        // Convert the value from INT to RAT
        mpq_set_sx (&aplRat, aplInt.parts[i], 1);

        // Convert the value from RAT to FMPQ
        fmpq_set_mpq (fmpq, &aplRat);

        // Convert the value from FMPQ to BA8F
        arb_set_fmpq (&lpSrc->at.aplBA8F.parts[i], fmpq, prec);
    } // End FOR

    // Free the temps
    fmpq_clear (fmpq);
    mpq_clear (&aplRat);

    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actHC8I_BA8F


//***************************************************************************
//  $PN_actHC8F_BA8F
//***************************************************************************

void PN_actHC8F_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    APLHC8F aplFlt;
    int     i;

    // Make a temp copy of the HC8F as we overwrite it
    aplFlt = lpSrc->at.aplHC8F;

    // Initialize the temps
    arbX_init (&lpSrc->at.aplBA1F, 8);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Convert the value from FLT to BA8F
        arb_set_d (&lpSrc->at.aplBA8F.parts[i], aplFlt.parts[i]);

    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actHC8F_BA8F


//***************************************************************************
//  $PN_actBA1F_BA2F
//***************************************************************************

void PN_actBA1F_BA2F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    int i;

    // Convert the value from BA1F to BA2F
    for (i = 1; i < 2; i++)
        Myarb_init (&lpSrc->at.aplBA8F.parts[i]);

    lpSrc->chType = PN_NUMTYPE_BA2F;
} // End PN_actBA1F_BA2F


//***************************************************************************
//  $PN_actBA1F_BA4F
//***************************************************************************

void PN_actBA1F_BA4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    int i;

    // Convert the value from BA1F to BA4F
    for (i = 1; i < 4; i++)
        Myarb_init (&lpSrc->at.aplBA8F.parts[i]);

    lpSrc->chType = PN_NUMTYPE_BA4F;
} // End PN_actBA1F_BA4F


//***************************************************************************
//  $PN_actBA1F_BA8F
//***************************************************************************

void PN_actBA1F_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    int i;

    // Convert the value from BA1F to BA8F
    for (i = 1; i < 8; i++)
        Myarb_init (&lpSrc->at.aplBA8F.parts[i]);

    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actBA1F_BA8F


//***************************************************************************
//  $PN_actBA2F_BA4F
//***************************************************************************

void PN_actBA2F_BA4F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    int i;

    // Convert the value from BA2F to BA4F
    for (i = 2; i < 4; i++)
        Myarb_init (&lpSrc->at.aplBA8F.parts[i]);

    lpSrc->chType = PN_NUMTYPE_BA4F;
} // End PN_actBA2F_BA4F


//***************************************************************************
//  $PN_actBA2F_BA8F
//***************************************************************************

void PN_actBA2F_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    int i;

    // Convert the value from BA2F to BA8F
    for (i = 2; i < 8; i++)
        Myarb_init (&lpSrc->at.aplBA8F.parts[i]);

    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actBA2F_BA8F


//***************************************************************************
//  $PN_actBA4F_BA8F
//***************************************************************************

void PN_actBA4F_BA8F
    (LPPN_YYSTYPE  lpSrc,
     LPPNLOCALVARS lppnLocalVars)

{
    int i;

    // Convert the value from BA4F to BA8F
    for (i = 4; i < 8; i++)
        Myarb_init (&lpSrc->at.aplBA8F.parts[i]);

    lpSrc->chType = PN_NUMTYPE_BA8F;
} // End PN_actBA4F_BA8F


// The first and second columns of M(a,b) are the action routine <PN_act##a##_##b> used to promote to a common type <b>.
// The second column of M(a,b) is the numeric type <PN_NUMTYPE_##b> to which the row and col are promoted when joined.

#define PN_MAT                                                                                                                                                                                                                                                                                                  \
{/*     BOOL          INT          FLT          RAT          VFP          HC2I         HC2F         HC2R         HC2V         HC4I         HC4F         HC4R         HC4V         HC8I         HC8F         HC8R         HC8V        BA1F         BA2F         BA4F         BA8F         INIT     */            \
   {M(BOOL,BOOL),M(BOOL,INT ),M(BOOL,FLT ),M(BOOL,RAT ),M(BOOL,VFP ),M(BOOL,HC2I),M(BOOL,HC2F),M(BOOL,HC2R),M(BOOL,HC2V),M(BOOL,HC4I),M(BOOL,HC4F),M(BOOL,HC4R),M(BOOL,HC4V),M(BOOL,HC8I),M(BOOL,HC8F),M(BOOL,HC8R),M(BOOL,HC8V),M(BOOL,BA1F),M(BOOL,BA2F),M(BOOL,BA4F),M(BOOL,BA8F),M(BOOL,INIT)}, /* BOOL */  \
   {M(INT ,INT ),M(INT ,INT ),M(INT ,FLT ),M(INT ,RAT ),M(INT ,VFP ),M(INT ,HC2I),M(INT ,HC2F),M(INT ,HC2R),M(INT ,HC2V),M(INT ,HC4I),M(INT ,HC4F),M(INT ,HC4R),M(INT ,HC4V),M(INT ,HC8I),M(INT ,HC8F),M(INT ,HC8R),M(INT ,HC8V),M(INT ,BA1F),M(INT ,BA2F),M(INT ,BA4F),M(INT ,BA8F),M(INT ,INIT)}, /* INT  */  \
   {M(FLT ,FLT ),M(FLT ,FLT ),M(FLT ,FLT ),M(FLT ,VFP ),M(FLT ,VFP ),M(FLT ,HC2F),M(FLT ,HC2F),M(FLT ,HC2V),M(FLT ,HC2V),M(FLT ,HC4F),M(FLT ,HC4F),M(FLT ,HC4V),M(FLT ,HC4V),M(FLT ,HC8F),M(FLT ,HC8F),M(FLT ,HC8V),M(FLT ,HC8V),M(FLT ,BA1F),M(FLT ,BA2F),M(FLT ,BA4F),M(FLT ,BA8F),M(FLT ,INIT)}, /* FLT  */  \
   {M(RAT ,RAT ),M(RAT ,RAT ),M(RAT ,VFP ),M(RAT ,RAT ),M(RAT ,VFP ),M(RAT ,HC2R),M(RAT ,HC2V),M(RAT ,HC2R),M(RAT ,HC2V),M(RAT ,HC4R),M(RAT ,HC4V),M(RAT ,HC4R),M(RAT ,HC4V),M(RAT ,HC8R),M(RAT ,HC8V),M(RAT ,HC8R),M(RAT ,HC8V),M(RAT ,BA1F),M(RAT ,BA2F),M(RAT ,BA4F),M(RAT ,BA8F),M(RAT ,INIT)}, /* RAT  */  \
   {M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,HC2V),M(VFP ,HC2V),M(VFP ,HC2V),M(VFP ,HC2V),M(VFP ,HC4V),M(VFP ,HC4V),M(VFP ,HC4V),M(VFP ,HC4V),M(VFP ,HC8V),M(VFP ,HC8V),M(VFP ,HC8V),M(VFP ,HC8V),M(VFP ,BA1F),M(VFP ,BA2F),M(VFP ,BA4F),M(VFP ,BA8F),M(VFP ,INIT)}, /* VFP  */  \
   {M(HC2I,HC2I),M(HC2I,HC2I),M(HC2I,HC2F),M(HC2I,HC2R),M(HC2I,HC2V),M(HC2I,HC2I),M(HC2I,HC2F),M(HC2I,HC2R),M(HC2I,HC2V),M(HC2I,HC4I),M(HC2I,HC4F),M(HC2I,HC4R),M(HC2I,HC4V),M(HC2I,HC8I),M(HC2I,HC8F),M(HC2I,HC8R),M(HC2I,HC8V),M(HC2I,BA2F),M(HC2I,BA2F),M(HC2I,BA4F),M(HC2I,BA8F),M(HC2I,INIT)}, /* HC2I */  \
   {M(HC2F,HC2F),M(HC2F,HC2F),M(HC2F,HC2F),M(HC2F,HC2V),M(HC2F,HC2V),M(HC2F,HC2F),M(HC2F,HC2F),M(HC2F,HC2V),M(HC2F,HC2V),M(HC2F,HC4F),M(HC2F,HC4F),M(HC2F,HC4V),M(HC2F,HC4V),M(HC2F,HC8F),M(HC2F,HC8F),M(HC2F,HC8V),M(HC2F,HC8V),M(HC2F,BA2F),M(HC2F,BA2F),M(HC2F,BA4F),M(HC2F,BA8F),M(HC2F,INIT)}, /* HC2F */  \
   {M(HC2R,HC2R),M(HC2R,HC2R),M(HC2R,HC2V),M(HC2R,HC2R),M(HC2R,HC2V),M(HC2R,HC2R),M(HC2R,HC2V),M(HC2R,HC2R),M(HC2R,HC2V),M(HC2R,HC4R),M(HC2R,HC4V),M(HC2R,HC4R),M(HC2R,HC4V),M(HC2R,HC8R),M(HC2R,HC8V),M(HC2R,HC8R),M(HC2R,HC8V),M(HC2R,BA2F),M(HC2R,BA2F),M(HC2R,BA4F),M(HC2R,BA8F),M(HC2R,INIT)}, /* HC2R */  \
   {M(HC2V,HC2V),M(HC2V,HC2V),M(HC2V,HC2V),M(HC2V,HC2V),M(HC2V,HC2V),M(HC2V,HC2V),M(HC2V,HC2V),M(HC2V,HC2V),M(HC2V,HC2V),M(HC2V,HC4V),M(HC2V,HC4V),M(HC2V,HC4V),M(HC2V,HC4V),M(HC2V,HC8V),M(HC2V,HC8V),M(HC2V,HC8V),M(HC2V,HC8V),M(HC2V,BA2F),M(HC2V,BA2F),M(HC2V,BA4F),M(HC2V,BA8F),M(HC2V,INIT)}, /* HC2V */  \
   {M(HC4I,HC4I),M(HC4I,HC4I),M(HC4I,HC4F),M(HC4I,HC4R),M(HC4I,HC4V),M(HC4I,HC4I),M(HC4I,HC4F),M(HC4I,HC4R),M(HC4I,HC4V),M(HC4I,HC4I),M(HC4I,HC4F),M(HC4I,HC4R),M(HC4I,HC4V),M(HC4I,HC8I),M(HC4I,HC8F),M(HC4I,HC8R),M(HC4I,HC8V),M(HC4I,BA4F),M(HC4I,BA4F),M(HC4I,BA4F),M(HC4I,BA8F),M(HC4I,INIT)}, /* HC4I */  \
   {M(HC4F,HC4F),M(HC4F,HC4F),M(HC4F,HC4F),M(HC4F,HC4V),M(HC4F,HC4V),M(HC4F,HC4F),M(HC4F,HC4F),M(HC4F,HC4V),M(HC4F,HC4V),M(HC4F,HC4F),M(HC4F,HC4F),M(HC4F,HC4V),M(HC4F,HC4V),M(HC4F,HC8F),M(HC4F,HC8F),M(HC4F,HC8V),M(HC4F,HC8V),M(HC4F,BA4F),M(HC4F,BA4F),M(HC4F,BA4F),M(HC4F,BA8F),M(HC4F,INIT)}, /* HC4F */  \
   {M(HC4R,HC4R),M(HC4R,HC4R),M(HC4R,HC4V),M(HC4R,HC4R),M(HC4R,HC4V),M(HC4R,HC4R),M(HC4R,HC4V),M(HC4R,HC4R),M(HC4R,HC4V),M(HC4R,HC4R),M(HC4R,HC4V),M(HC4R,HC4R),M(HC4R,HC4V),M(HC4R,HC8R),M(HC4R,HC8V),M(HC4R,HC8R),M(HC4R,HC8V),M(HC4R,BA4F),M(HC4R,BA4F),M(HC4R,BA4F),M(HC4R,BA8F),M(HC4R,INIT)}, /* HC4R */  \
   {M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC4V),M(HC4V,HC8V),M(HC4V,HC8V),M(HC4V,HC8V),M(HC4V,HC8V),M(HC4V,BA4F),M(HC4V,BA4F),M(HC4V,BA4F),M(HC4V,BA8F),M(HC4V,INIT)}, /* HC4V */  \
   {M(HC8I,HC8I),M(HC8I,HC8I),M(HC8I,HC8F),M(HC8I,HC8R),M(HC8I,HC8V),M(HC8I,HC8I),M(HC8I,HC8F),M(HC8I,HC8R),M(HC8I,HC8V),M(HC8I,HC8I),M(HC8I,HC8F),M(HC8I,HC8R),M(HC8I,HC8V),M(HC8I,HC8I),M(HC8I,HC8F),M(HC8I,HC8R),M(HC8I,HC8V),M(HC8I,BA8F),M(HC8I,BA8F),M(HC8I,BA8F),M(HC8I,BA8F),M(HC8I,INIT)}, /* HC8I */  \
   {M(HC8F,HC8F),M(HC8F,HC8F),M(HC8F,HC8F),M(HC8F,HC8V),M(HC8F,HC8V),M(HC8F,HC8F),M(HC8F,HC8F),M(HC8F,HC8V),M(HC8F,HC8V),M(HC8F,HC8F),M(HC8F,HC8F),M(HC8F,HC8V),M(HC8F,HC8V),M(HC8F,HC8F),M(HC8F,HC8F),M(HC8F,HC8V),M(HC8F,HC8V),M(HC8F,BA8F),M(HC8F,BA8F),M(HC8F,BA8F),M(HC8F,BA8F),M(HC8F,INIT)}, /* HC8F */  \
   {M(HC8R,HC8R),M(HC8R,HC8R),M(HC8R,HC8V),M(HC8R,HC8R),M(HC8R,HC8V),M(HC8R,HC8R),M(HC8R,HC8V),M(HC8R,HC8R),M(HC8R,HC8V),M(HC8R,HC8R),M(HC8R,HC8V),M(HC8R,HC8R),M(HC8R,HC8V),M(HC8R,HC8R),M(HC8R,HC8V),M(HC8R,HC8R),M(HC8R,HC8V),M(HC8R,BA8F),M(HC8R,BA8F),M(HC8R,BA8F),M(HC8R,BA8F),M(HC8R,INIT)}, /* HC8R */  \
   {M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,HC8V),M(HC8V,BA8F),M(HC8V,BA8F),M(HC8V,BA8F),M(HC8V,BA8F),M(HC8V,INIT)}, /* HC8V */  \
   {M(BA1F,BA1F),M(BA1F,BA1F),M(BA1F,BA1F),M(BA1F,BA1F),M(BA1F,BA1F),M(BA1F,BA2F),M(BA1F,BA2F),M(BA1F,BA2F),M(BA1F,BA2F),M(BA1F,BA4F),M(BA1F,BA4F),M(BA1F,BA4F),M(BA1F,BA4F),M(BA1F,BA8F),M(BA1F,BA8F),M(BA1F,BA8F),M(BA1F,BA8F),M(BA1F,BA1F),M(BA1F,BA2F),M(BA1F,BA4F),M(BA1F,BA8F),M(BA1F,INIT)}, /* BA1F */  \
   {M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,BA4F),M(BA2F,BA4F),M(BA2F,BA4F),M(BA2F,BA4F),M(BA2F,BA8F),M(BA2F,BA8F),M(BA2F,BA8F),M(BA2F,BA8F),M(BA2F,BA2F),M(BA2F,BA2F),M(BA2F,BA4F),M(BA2F,BA8F),M(BA2F,INIT)}, /* BA2F */  \
   {M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA8F),M(BA4F,BA8F),M(BA4F,BA8F),M(BA4F,BA8F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA4F),M(BA4F,BA8F),M(BA4F,INIT)}, /* BA4F */  \
   {M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,BA8F),M(BA8F,INIT)}, /* BA8F */  \
   {M(INIT,BOOL),M(INIT,INT ),M(INIT,FLT ),M(INIT,RAT ),M(INIT,VFP ),M(INIT,HC2I),M(INIT,HC2F),M(INIT,HC2R),M(INIT,HC2V),M(INIT,HC4I),M(INIT,HC4F),M(INIT,HC4R),M(INIT,HC4V),M(INIT,HC8I),M(INIT,HC8F),M(INIT,HC8R),M(INIT,HC8V),M(INIT,BA1F),M(INIT,BA2F),M(INIT,BA4F),M(INIT,BA8F),M(INIT,INIT)}, /* INIT */  \
}

// N.B.:  The rows and columns in the above #define *MUST* be in the same order as those of <PN_NUMTYPE>.

// PN_NUMTYPE_xxx promotion result matrix
PNNUMTYPE aNumTypePromote[PN_NUMTYPE_LENGTH][PN_NUMTYPE_LENGTH] =
#define M(a,b)  PN_NUMTYPE_##b
PN_MAT;
#undef  M

#define PN_actBOOL_INIT     PN_actIDENT
#define PN_actINT_INIT      PN_actIDENT
#define PN_actFLT_INIT      PN_actIDENT
#define PN_actRAT_INIT      PN_actIDENT
#define PN_actVFP_INIT      PN_actIDENT
#define PN_actHC2I_INIT     PN_actIDENT
#define PN_actHC2F_INIT     PN_actIDENT
#define PN_actHC2R_INIT     PN_actIDENT
#define PN_actHC2V_INIT     PN_actIDENT
#define PN_actHC4I_INIT     PN_actIDENT
#define PN_actHC4F_INIT     PN_actIDENT
#define PN_actHC4R_INIT     PN_actIDENT
#define PN_actHC4V_INIT     PN_actIDENT
#define PN_actHC8I_INIT     PN_actIDENT
#define PN_actHC8F_INIT     PN_actIDENT
#define PN_actHC8R_INIT     PN_actIDENT
#define PN_actHC8V_INIT     PN_actIDENT
#define PN_actBA1F_INIT     PN_actIDENT
#define PN_actBA2F_INIT     PN_actIDENT
#define PN_actBA4F_INIT     PN_actIDENT
#define PN_actBA8F_INIT     PN_actIDENT

#define PN_actBOOL_BOOL     PN_actIDENT
#define PN_actBOOL_INT      PN_actIDENT
#define PN_actBOOL_FLT      PN_actINT_FLT
#define PN_actBOOL_RAT      PN_actINT_RAT
#define PN_actBOOL_VFP      PN_actINT_VFP
#define PN_actBOOL_HC2I     PN_actINT_HC2I
#define PN_actBOOL_HC2F     PN_actINT_HC2F
#define PN_actBOOL_HC2R     PN_actINT_HC2R
#define PN_actBOOL_HC2V     PN_actINT_HC2V
#define PN_actBOOL_HC4I     PN_actINT_HC4I
#define PN_actBOOL_HC4F     PN_actINT_HC4F
#define PN_actBOOL_HC4R     PN_actINT_HC4R
#define PN_actBOOL_HC4V     PN_actINT_HC4V
#define PN_actBOOL_HC8I     PN_actINT_HC8I
#define PN_actBOOL_HC8F     PN_actINT_HC8F
#define PN_actBOOL_HC8R     PN_actINT_HC8R
#define PN_actBOOL_HC8V     PN_actINT_HC8V
#define PN_actBOOL_BA1F     PN_actINT_BA1F
#define PN_actBOOL_BA2F     PN_actINT_BA2F
#define PN_actBOOL_BA4F     PN_actINT_BA4F
#define PN_actBOOL_BA8F     PN_actINT_BA8F

#define PN_actINT_INT       PN_actIDENT
////ine PN_actINT_HC2I      PN_actUNK
////ine PN_actINT_HC2F      PN_actUNK
////ine PN_actINT_HC2R      PN_actUNK
////ine PN_actINT_HC2V      PN_actUNK
////ine PN_actINT_HC4I      PN_actUNK
////ine PN_actINT_HC4F      PN_actUNK
////ine PN_actINT_HC4R      PN_actUNK
////ine PN_actINT_HC4V      PN_actUNK
////ine PN_actINT_HC8I      PN_actUNK
////ine PN_actINT_HC8F      PN_actUNK
////ine PN_actINT_HC8R      PN_actUNK
////ine PN_actINT_HC8V      PN_actUNK

#define PN_actFLT_FLT       PN_actIDENT
////ine PN_actFLT_HC2F      PN_actUNK
////ine PN_actFLT_HC2V      PN_actUNK
////ine PN_actFLT_HC4F      PN_actUNK
////ine PN_actFLT_HC4V      PN_actUNK
////ine PN_actFLT_HC8F      PN_actUNK
////ine PN_actFLT_HC8V      PN_actUNK

#define PN_actRAT_RAT       PN_actIDENT
////ine PN_actRAT_HC2R      PN_actUNK
////ine PN_actRAT_HC4R      PN_actUNK
////ine PN_actRAT_HC8R      PN_actUNK
////ine PN_actRAT_HC2V      PN_actUNK
////ine PN_actRAT_HC4V      PN_actUNK
////ine PN_actRAT_HC8V      PN_actUNK

#define PN_actVFP_VFP       PN_actIDENT
////ine PN_actVFP_HC2V      PN_actUNK
////ine PN_actVFP_HC4V      PN_actUNK
////ine PN_actVFP_HC8V      PN_actUNK

#define PN_actHC2I_HC2I     PN_actIDENT
////ine PN_actHC2I_HC2F     PN_actUNK
////ine PN_actHC2I_HC2R     PN_actUNK
////ine PN_actHC2I_HC2V     PN_actUNK
////ine PN_actHC2I_HC4I     PN_actUNK
////ine PN_actHC2I_HC4F     PN_actUNK
////ine PN_actHC2I_HC4R     PN_actUNK
////ine PN_actHC2I_HC4V     PN_actUNK
////ine PN_actHC2I_HC8I     PN_actUNK
////ine PN_actHC2I_HC8F     PN_actUNK
////ine PN_actHC2I_HC8R     PN_actUNK
////ine PN_actHC2I_HC8V     PN_actUNK

#define PN_actHC2F_HC2F     PN_actIDENT
////ine PN_actHC2F_HC2V     PN_actUNK
////ine PN_actHC2F_HC4F     PN_actUNK
////ine PN_actHC2F_HC4V     PN_actUNK
////ine PN_actHC2F_HC8F     PN_actUNK
////ine PN_actHC2F_HC8V     PN_actUNK

#define PN_actHC2R_HC2R     PN_actIDENT
////ine PN_actHC2R_HC2V     PN_actUNK
////ine PN_actHC2R_HC4R     PN_actUNK
////ine PN_actHC2R_HC4V     PN_actUNK
////ine PN_actHC2R_HC8R     PN_actUNK
////ine PN_actHC2R_HC8V     PN_actUNK

#define PN_actHC2V_HC2V     PN_actIDENT
////ine PN_actHC2V_HC4V     PN_actUNK
////ine PN_actHC2V_HC8V     PN_actUNK

#define PN_actHC4I_HC4I     PN_actIDENT
////ine PN_actHC4I_HC4F     PN_actUNK
////ine PN_actHC4I_HC4R     PN_actUNK
////ine PN_actHC4I_HC4V     PN_actUNK
////ine PN_actHC4I_HC8I     PN_actUNK
////ine PN_actHC4I_HC8F     PN_actUNK
////ine PN_actHC4I_HC8R     PN_actUNK
////ine PN_actHC4I_HC8V     PN_actUNK

#define PN_actHC4F_HC4F     PN_actIDENT
////ine PN_actHC4F_HC4V     PN_actUNK
////ine PN_actHC4F_HC8F     PN_actUNK
////ine PN_actHC4F_HC8V     PN_actUNK

#define PN_actHC4R_HC4R     PN_actIDENT
////ine PN_actHC4R_HC4V     PN_actUNK
////ine PN_actHC4R_HC8R     PN_actUNK
////ine PN_actHC4R_HC8V     PN_actUNK

#define PN_actHC4V_HC4V     PN_actIDENT
////ine PN_actHC4V_HC8V     PN_actUNK

#define PN_actHC8I_HC8I     PN_actIDENT
////ine PN_actHC8I_HC8F     PN_actUNK
////ine PN_actHC8I_HC8R     PN_actUNK
////ine PN_actHC8I_HC8V     PN_actUNK

#define PN_actHC8F_HC8F     PN_actIDENT
////ine PN_actHC8F_HC8V     PN_actUNK

#define PN_actHC8R_HC8R     PN_actIDENT
////ine PN_actHC8R_HC8V     PN_actUNK

#define PN_actHC8V_HC8V     PN_actIDENT

#define PN_actBA1F_BA1F     PN_actIDENT
#define PN_actBA2F_BA2F     PN_actIDENT
#define PN_actBA4F_BA4F     PN_actIDENT
#define PN_actBA8F_BA8F     PN_actIDENT

#define PN_actINIT_BOOL     PN_actUNK
#define PN_actINIT_INT      PN_actUNK
#define PN_actINIT_FLT      PN_actUNK
#define PN_actINIT_RAT      PN_actUNK
#define PN_actINIT_VFP      PN_actUNK
#define PN_actINIT_HC2I     PN_actUNK
#define PN_actINIT_HC2F     PN_actUNK
#define PN_actINIT_HC2R     PN_actUNK
#define PN_actINIT_HC2V     PN_actUNK
#define PN_actINIT_HC4I     PN_actUNK
#define PN_actINIT_HC4F     PN_actUNK
#define PN_actINIT_HC4R     PN_actUNK
#define PN_actINIT_HC4V     PN_actUNK
#define PN_actINIT_HC8I     PN_actUNK
#define PN_actINIT_HC8F     PN_actUNK
#define PN_actINIT_HC8R     PN_actUNK
#define PN_actINIT_HC8V     PN_actUNK
#define PN_actINIT_BA1F     PN_actUNK
#define PN_actINIT_BA2F     PN_actUNK
#define PN_actINIT_BA4F     PN_actUNK
#define PN_actINIT_BA8F     PN_actUNK
#define PN_actINIT_INIT     PN_actUNK

// PN_NUMTYPE_xxx promotion action matrix
PN_ACTION aNumTypeAction [PN_NUMTYPE_LENGTH][PN_NUMTYPE_LENGTH] =
#define M(a,b)  PN_act##a##_##b
PN_MAT;
#undef  M


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
    UBOOL     bRet = TRUE,              // TRUE iff the result is valid
              bSigned;                  // TRUE iff the number is negative
    APLINT    aplInteger;               // Temporary integer
    size_t    uLen,                     // Loop length
              uNumAcc,                  // Starting offset
              uAcc;                     // Loop counter
    PNNUMTYPE chType;                   // The numeric type (see PNNUMTYPE)
    LPCHAR    lpszNumAccum;             // Ptr to next byte
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return;

    // Initialize the starting offset
    uNumAcc = lpYYArg->uNumAcc;
    lpszNumAccum = &lppnLocalVars->lpszNumAccum[0];

    // Get the numeric type
    chType = (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT) ? lpYYArg->chType : lppnLocalVars->chComType;

    // Handle negative sign
    bSigned = (lpszNumAccum[uNumAcc] EQ OVERBAR1);

    // Split cases based upon the current numeric type
    switch (chType)
    {
        case PN_NUMTYPE_BOOL:
        case PN_NUMTYPE_INT:
            // If we're forming the numerator of a Rational number, ...
            if (bUseRat)
            {
                // Convert the numerator to a GMP integer
                mpz_init_set_str (mpq_numref (&lpYYArg->at.aplRat), &lpszNumAccum[uNumAcc], 10);

                // Change the type to RAT
                lpYYArg->chType = PN_NUMTYPE_RAT;

                break;
            } else
            {
                // Initialize the accumulator
                aplInteger = 0;

                // Initialize the loop length
                uLen = lstrlen (&lpszNumAccum[uNumAcc]);

                // If the number is negative, ...
                if (bSigned)
                    // Loop through the digits
                    for (uAcc = 1; bRet && uAcc < uLen; uAcc++)
                    {
                        aplInteger = imul64 (aplInteger, 10, &bRet, EXCEPTION_RESULT_FLOAT);
                        if (bRet)
                            aplInteger = isub64 (aplInteger, lpszNumAccum[uNumAcc + uAcc] - '0', &bRet, EXCEPTION_RESULT_FLOAT);
                    } // End FOR
                else
                    // Loop through the digits
                    for (uAcc = 0; bRet && uAcc < uLen; uAcc++)
                    {
                        aplInteger = imul64 (aplInteger, 10, &bRet, EXCEPTION_RESULT_FLOAT);
                        if (bRet)
                            aplInteger = iadd64 (aplInteger, lpszNumAccum[uNumAcc + uAcc] - '0', &bRet, EXCEPTION_RESULT_FLOAT);
                    } // End FOR
                if (bRet)
                {
                    // If it's -0, ...
                    if (gAllowNeg0
                     && bSigned
                     && aplInteger EQ 0)
                    {
                        // Save -0 as a float
                        lpYYArg->at.aplFloat = -0.0;

                        // Change the type to float
                        lpYYArg->chType = PN_NUMTYPE_FLT;

                        break;
                    } // End IF

                    // Save in the result
                    lpYYArg->at.aplInteger = aplInteger;

                    if (IsBooleanValue (aplInteger))
                        lpYYArg->chType = PN_NUMTYPE_BOOL;
                    break;
                } // End IF
            } // End IF

            // Fall through to the float case

        case PN_NUMTYPE_FLT:
            Assert (!bUseRat);

            // Use David Gay's routines
            lpYYArg->at.aplFloat = MyStrtod (&lpszNumAccum[uNumAcc], NULL);

            // Change the type to FLT
            lpYYArg->chType = PN_NUMTYPE_FLT;

            break;

        case PN_NUMTYPE_HC2R:               // Convert the number to a RAT
        case PN_NUMTYPE_HC8R:               // ...
        case PN_NUMTYPE_HC4R:               // ...
            // Initialize to 0/1
            mphcXr_init (&lpYYArg->at.aplRat, TranslateNumTypeToHCDim (chType));

            // Fall through to common code

        case PN_NUMTYPE_RAT:                // Convert the number to a RAT
            // If we're in the first pass, ...
            if (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT)
            {
                // This type is used after the numerator has been calculated
                //   and is in mpq_numref (&lpYYArg->at.aplRat).  The denominator is
                //   in lppnLocalVars->NumAccum[uNumAcc] and must be converted.

                // Convert the denominator to a GMP integer
                mpz_init_set_str (mpq_denref (&lpYYArg->at.aplRat), &lpszNumAccum[uNumAcc], 10);

                // Save the sign of the result
                bSigned = (lpYYArg->bSigned NE bSigned);

                // If the denominator is zero, ...
                if (IsMpz0 (mpq_denref (&lpYYArg->at.aplRat)))
                {
                    // If the numerator is zero, ...
                    if (IsMpz0 (mpq_numref (&lpYYArg->at.aplRat)))
                    {
                        APLRAT mpqRes = {0};

                        // See what the []IC oracle has to say
                        mpq_QuadICValue (&lpYYArg->at.aplRat,
                                          ICNDX_0DIV0,
                                         &lpYYArg->at.aplRat,
                                         &mpqRes,
                                          (mpz_sgn (mpq_numref (&lpYYArg->at.aplRat)) EQ -1) NE
                                          (mpz_sgn (mpq_denref (&lpYYArg->at.aplRat)) EQ -1));
                        // We no longer need this storage
                        Myq_clear (&lpYYArg->at.aplRat);

                        // If the result is -0, ...
                        if (gAllowNeg0
                         && bSigned
                         && IsMpq0 (&mpqRes))
                        {
                            // Change the type to VFP
                            lpYYArg->chType = PN_NUMTYPE_VFP;

                            // Initialize the result and set to -0
                            mpfr_init_set_str (&lpYYArg->at.aplVfp, "-0", 10, MPFR_RNDN);
                        } else
                        {
                            // If the result is negative, ...
                            if (bSigned)
                                // Negate it
                                mpq_neg (&mpqRes, &mpqRes);

                            // Copy result to ALLTYPES
                            lpYYArg->at.aplRat = mpqRes;
                        } // End IF/ELSE
                    } else
                    {
                        // We no longer need this storage
                        Myq_clear (&lpYYArg->at.aplRat);

                        // Set to the appropriate-signed infinity
                        mpq_set_infsub (&lpYYArg->at.aplRat, bSigned ? -1 : 1);
                    } // End IF/ELSE
                } else
                // If the result is -0, ...
                if (gAllowNeg0
                 && bSigned
                 && IsMpz0 (mpq_numref (&lpYYArg->at.aplRat)))
                {
                    // We no longer need this storage
                    Myq_clear (&lpYYArg->at.aplRat);

                    // Change the type to VFP
                    lpYYArg->chType = PN_NUMTYPE_VFP;

                    // Initialize the result and set to -0
                    mpfr_init_set_str (&lpYYArg->at.aplVfp, "-0", 10, MPFR_RNDN);
                } else
                    // Canonicalize the arg
                    mpq_canonicalize (&lpYYArg->at.aplRat);
            } else
            {
                // Not formatting from a Rational number
                // Must be one that is expressible as an integer
                Assert (!bUseRat);

                // If it's a FLT, ...
                if (IsPnNumTypeFlt (lpYYArg->chType))
                {
                    CR_RETCODES crRetCode;              // Return code

                    // Convert to a RAT
                    crRetCode =
                      mpq_init_set_str2 (&lpYYArg->at.aplRat,       // Ptr to result
                                         &lpszNumAccum[uNumAcc],    // Ptr to incoming line
                                          10);                      // Base of number system
                    // Split cases based upon the return code
                    switch (crRetCode)
                    {
                        case CR_SUCCESS:
                            // Change the type to Rational
                            lpYYArg->chType = PN_NUMTYPE_RAT;

                            break;

                        case CR_SYNTAX_ERROR:
                            // Save the error message
                            ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);

                            // Mark as in error
                            lpYYArg = NULL;

                            break;

                        case CR_DOMAIN_ERROR:
                            // Save the error message
                            ErrorMessageIndirect (ERRMSG_DOMAIN_ERROR APPEND_NAME);

                            // Mark as in error
                            lpYYArg = NULL;

                            break;

                        case CR_RESULT_NEG0:
                            // Clear the previous memory
                            Myq_clear (&lpYYArg->at.aplRat);

                            // Zap it
                            ZeroMemory (&lpYYArg->at.aplRat, sizeof (lpYYArg->at.aplRat));

                            // Set the result to -0
                            mpfr_init_set_str (&lpYYArg->at.aplVfp, "-0", 10, MPFR_RNDN);

                            // Set the result type
                            lpYYArg->chType = PN_NUMTYPE_VFP;

                            break;

                        defstop
                            break;
                    } // End SWITCH

                } else
                {
                    // Convert the string to a rational number
                    mpz_init_set_str (mpq_numref (&lpYYArg->at.aplRat), &lppnLocalVars->lpszNumAccum[uNumAcc], 10);
                    mpz_init_set_str (mpq_denref (&lpYYArg->at.aplRat), "1"                                  , 10);

                    // Change the type to Rational
                    lpYYArg->chType = PN_NUMTYPE_RAT;
                } // End IF/ELSE
            } // End IF/ELSE

            break;


#define PN_NumCalcHCxV(N)                                                                                   \
            /* Not formatting from a Rational number */                                                     \
            Assert (!bUseRat);                                                                              \
                                                                                                            \
            /* Initialize to 0 */                                                                           \
            mphcXv_init0 (&lpYYArg->at.aplVfp, N);                                                          \
                                                                                                            \
            /* Convert the string to a VFP number */                                                        \
            mpfr_set_str (&lpYYArg->at.aplVfp, &lpszNumAccum[uNumAcc], 10, MPFR_RNDN);                      \
                                                                                                            \
            /* Change the type to HCxV */                                                                   \
            lpYYArg->chType = PN_NUMTYPE_HC##N##V;


        case PN_NUMTYPE_VFP:
            PN_NumCalcHCxV (1)
            break;

        case PN_NUMTYPE_HC2V:
            PN_NumCalcHCxV (2)
            break;

        case PN_NUMTYPE_HC4V:
            PN_NumCalcHCxV (4)
            break;

        case PN_NUMTYPE_HC8V:
            PN_NumCalcHCxV (8)
            break;

#define PN_NumCalcBAxF(N)                                                                                   \
            /* Not formatting from a Rational number */                                                     \
            Assert (!bUseRat);                                                                              \
                                                                                                            \
            arbX_init (&lpYYArg->at.aplBA1F, N);                                                            \
                                                                                                            \
            /* Convert the string to a BAxF number */                                                       \
            arb_set_str (&lpYYArg->at.aplBA1F, &lpszNumAccum[uNumAcc], prec);                               \
                                                                                                            \
            /* Change the type to BAxF */                                                                   \
            lpYYArg->chType = PN_NUMTYPE_BA##N##F;

        case PN_NUMTYPE_BA1F:
            PN_NumCalcBAxF (1)
            break;

        case PN_NUMTYPE_BA2F:
            PN_NumCalcBAxF (2)
            break;

        case PN_NUMTYPE_BA4F:
            PN_NumCalcBAxF (4)
            break;

        case PN_NUMTYPE_BA8F:
            PN_NumCalcBAxF (8)
            break;

        case PN_NUMTYPE_HC2I:               // Can't happen with PN_NumCalc as it's used to produce simple results only
        case PN_NUMTYPE_HC2F:               // ...
        case PN_NUMTYPE_HC4F:               // ...
        case PN_NUMTYPE_HC8F:               // ...
        defstop
            break;
    } // End SWITCH

    // If it's valid, ...
    if (lpYYArg NE NULL)
        // Save the sign
        lpYYArg->bSigned = bSigned;
} // End PN_NumCalc


//***************************************************************************
//  $PN_ChrAccInt
//
//  Append a new char to the internal buffer
//***************************************************************************

void PN_ChrAccInt
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     char          chCur)               // The next char

{
    // Accumulate this char
    lppnLocalVars->lpszAlphaInt[lppnLocalVars->uAlpAccInt++] = chCur;
} // End PN_ChrAccInt


//***************************************************************************
//  $PN_ChrAccDec
//
//  Append a new char to the internal buffer
//***************************************************************************

void PN_ChrAccDec
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     char          chCur)               // The next char

{
    // Accumulate this char
    lppnLocalVars->lpszAlphaDec[lppnLocalVars->uAlpAccDec++] = chCur;
} // End PN_ChrAccDec


//***************************************************************************
//  $PN_MakeRatPoint
//
//  Merge the numerator and denominator to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeRatPoint
    (LPPN_YYSTYPE  lpYYNum,             // Ptr to The numerator part
     LPPN_YYSTYPE  lpYYDen,             // Ptr to The denominator part (may be NULL) (N.B. This parameter is unused)
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    size_t      numLen;                 // # chars in number
    PNNUMTYPE   chType;                 // The numeric type (see PNNUMTYPE)
    int         iHCDim;                 // HC Dimension (1, 2, 4, 8)
    char        chZap;                  // Zapped char
    CR_RETCODES crRetCode;              // Return code

    // Get the numeric type
    chType = (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT) ? PN_NUMTYPE_RAT : lppnLocalVars->chComType;

    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDim = TranslateNumTypeToHCDim (chType);

    // Get the length of the number
    numLen  = strspn (&lppnLocalVars->lpszStart[lpYYNum->uNumStart         ], OVERBAR1_STR INFINITY1_STR               );
    numLen += strspn (&lppnLocalVars->lpszStart[lpYYNum->uNumStart + numLen],                            DEC_CHARS "r/");

    // If it's already a RAT, ...
    if (IsPnNumTypeRat (lpYYNum->chType))
        // Clear the previous memory
        Myq_clear (&lpYYNum->at.aplRat);

    // Initialize to 0/1
    ZeroMemory  (&lpYYNum->at.aplHC8R, sizeof (lpYYNum->at.aplHC8R));
    mphcXr_init (&lpYYNum->at.aplHC8R, iHCDim);

    // Save and zap the next char
    chZap = lppnLocalVars->lpszStart[numLen + lpYYNum->uNumStart];
            lppnLocalVars->lpszStart[numLen + lpYYNum->uNumStart] = AC_EOS;

    crRetCode =
      mpq_set_str2 (&lpYYNum->at.aplRat,
                    &lppnLocalVars->lpszStart[lpYYNum->uNumStart],
                     10);
    // Restore the zapped char
    lppnLocalVars->lpszStart[numLen + lpYYNum->uNumStart] = chZap;

    // Change the type to RAT
    lpYYNum->chType = PN_NUMTYPE_RAT;

    // Split cases based upon the return code
    switch (crRetCode)
    {
        case CR_SUCCESS:
            break;

        case CR_SYNTAX_ERROR:
            // Save the error message
            ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);

            // Mark as in error
            lpYYNum = NULL;

            break;

        case CR_DOMAIN_ERROR:
            // Save the error message
            ErrorMessageIndirect (ERRMSG_DOMAIN_ERROR APPEND_NAME);

            // Mark as in error
            lpYYNum = NULL;

            break;

        case CR_RESULT_NEG0:
            // Clear the previous memory
            Myq_clear (&lpYYNum->at.aplRat);

            // Zap it
            ZeroMemory (&lpYYNum->at.aplRat, sizeof (lpYYNum->at.aplRat));

            // Set the result to -0
            mpfr_init_set_str (&lpYYNum->at.aplVfp, "-0", 10, MPFR_RNDN);

            // Set the result type
            lpYYNum->chType = PN_NUMTYPE_VFP;

            // Clear the RatSep flag
            lppnLocalVars->bRatSep = FALSE;

            break;

        defstop
            break;
    } // End SWITCH

    // If the result type is not INIT, ...
    if (lppnLocalVars->chComType NE PN_NUMTYPE_INIT)
        // Promote the result
        (*aNumTypeAction[lpYYNum->chType][lppnLocalVars->chComType]) (lpYYNum, lppnLocalVars);

    return lpYYNum;
} // End PN_MakeRatPoint


//***************************************************************************
//  $PN_MakeBasePoint
//
//  Merge the base and alphaint part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeBasePoint
    (LPPN_YYSTYPE  lpYYBase,            // Ptr to the base part
     LPPN_YYSTYPE  lpYYAlphaInt,        // Ptr to the Integer AlphaInt part (may be NULL)
     LPPN_YYSTYPE  lpYYAlphaDec,        // Ptr to the Decimal AlphaInt part (may be NULL)
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    PN_YYSTYPE   pnBaseInt,             // Copy of lpYYBase for the Integer part and the Integer result
                 pnBaseDec;             // ...                      Decimal ...          Decimal ...
    UBOOL        bContinue = TRUE;      // TRUE iff we are to continue the WHILE loop
    PNNUMTYPE    chType;                // The numeric type (see PNNUMTYPE)
    APLSTYPE     aplTypeNum;            // ... as an APLSTYPE
    ALLTYPES     atBaseInt = {0},       // Copy of lpYYBase->at for the Integer part
                 atBaseDec = {0};       // ...                          Decimal ...
    int          iHCDim;                // HC Dimension (1, 2, 4, 8)

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // Get the numeric type
    chType = (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT) ? lpYYBase->chType : lppnLocalVars->chComType;
    aplTypeNum = TranslateNumTypeToArrayType (chType);

    // Make a copy of the Base as it is overwritten by PN_MakeBasePointSub
    (*aTypeActPromote[aplTypeNum][aplTypeNum])(&lpYYBase->at, 0, &atBaseInt);
    CopyAll (&pnBaseInt   ,  lpYYBase);
    CopyAll (&pnBaseInt.at, &atBaseInt);

    // Make a copy of the Base as it is overwritten by PN_MakeBasePointSub
    (*aTypeActPromote[aplTypeNum][aplTypeNum])(&lpYYBase->at, 0, &atBaseDec);
    CopyAll (&pnBaseDec   ,  lpYYBase);
    CopyAll (&pnBaseDec.at, &atBaseDec);

    // Call subroutine on the integer part
    if (!PN_MakeBasePointSub (&pnBaseInt,
                               lpYYAlphaInt,
                               lppnLocalVars->lpszAlphaInt,
                               lppnLocalVars->uAlpAccInt,
                               lppnLocalVars))
            goto ERROR_EXIT;
    // If we promoted, ...
    if (lpYYBase->chType NE pnBaseInt.chType)
    {
        Assert (IsPnNumTypeInt (lpYYBase->chType));

        // Copy to result var
        CopyAll (lpYYBase, &pnBaseInt);

        // Clear the ALLTYPES
        ZeroMemory (&lpYYBase->at, sizeof (lpYYBase->at));

        // Calculate the HC Dimension (1, 2, 4, 8)
        iHCDim = TranslateNumTypeToHCDim (lpYYBase->chType);

        // Initialize the ALLTYPES
        switch (lpYYBase->chType)
        {
            case PN_NUMTYPE_BOOL:   // Skip the non-global numerics
            case PN_NUMTYPE_INT:    // ...
            case PN_NUMTYPE_HC2I:   // ...
            case PN_NUMTYPE_HC4I:   // ...
            case PN_NUMTYPE_HC8I:   // ...
            case PN_NUMTYPE_FLT:    // ...
            case PN_NUMTYPE_HC2F:   // ...
            case PN_NUMTYPE_HC4F:   // ...
            case PN_NUMTYPE_HC8F:   // ...
                break;

            case PN_NUMTYPE_RAT:
            case PN_NUMTYPE_HC2R:
            case PN_NUMTYPE_HC4R:
            case PN_NUMTYPE_HC8R:
                // Initialize to 0/1
                mphcXr_init  (&lpYYBase->at.aplRat, iHCDim);

                break;

            case PN_NUMTYPE_VFP:
            case PN_NUMTYPE_HC2V:
            case PN_NUMTYPE_HC4V:
            case PN_NUMTYPE_HC8V:
                // Initialize to 0
                mphcXv_init0 (&lpYYBase->at.aplVfp, iHCDim);

                break;

            case PN_NUMTYPE_ARB:
            case PN_NUMTYPE_BA2F:
            case PN_NUMTYPE_BA4F:
            case PN_NUMTYPE_BA8F:
                // Initialize to 0/0
                arbX_init    (&lpYYBase->at.aplArb, iHCDim);

                break;

            defstop
                break;
        } // End SWITCH
    } // End IF

    // If there's a decimal part, ...
    if (lppnLocalVars->uAlpAccDec NE 0)
    {
        if (!PN_MakeBasePointSub (&pnBaseDec,
                                   lpYYAlphaDec,
                                   lppnLocalVars->lpszAlphaDec,
                                   lppnLocalVars->uAlpAccDec,
                                   lppnLocalVars))
            goto ERROR_EXIT;
        // Calculate the common data type
        chType = aNumTypePromote[pnBaseInt.chType][pnBaseDec.chType];

        // Promote the Integer, Decimal, and Base parts to the common datatype
        (*aNumTypeAction[pnBaseInt.chType][chType]) (&pnBaseInt, lppnLocalVars);
        (*aNumTypeAction[pnBaseDec.chType][chType]) (&pnBaseDec, lppnLocalVars);
        (*aNumTypeAction[lpYYBase->chType][chType]) (lpYYBase  , lppnLocalVars);

        while (bContinue)
        // Split cases based upon the numeric type of the base
        switch (chType)
        {
            case PN_NUMTYPE_BOOL:
            case PN_NUMTYPE_INT:
                // Promote the Integer, Decimal, and Base parts to the common datatype
                (*aNumTypeAction[chType][PN_NUMTYPE_FLT]) (&pnBaseInt, lppnLocalVars);
                (*aNumTypeAction[chType][PN_NUMTYPE_FLT]) (&pnBaseDec, lppnLocalVars);
                (*aNumTypeAction[chType][PN_NUMTYPE_FLT]) (lpYYBase  , lppnLocalVars);

                // Set the new common type
                chType = PN_NUMTYPE_FLT;

                // Go around again
                break;

            case PN_NUMTYPE_HC2I:
            case PN_NUMTYPE_HC4I:
            case PN_NUMTYPE_HC8I:
                // Promote the Integer, Decimal, and Base parts to the common datatype
                (*aNumTypeAction[chType][chType + 1]) (&pnBaseInt, lppnLocalVars);
                (*aNumTypeAction[chType][chType + 1]) (&pnBaseDec, lppnLocalVars);
                (*aNumTypeAction[chType][chType + 1]) (lpYYBase  , lppnLocalVars);

                // Set the new common type
                chType++;

                // Go around again
                break;


#define PN_BaseHCxF_MAC(N)                                                              \
            {                                                                           \
                APLHC##N##F aplPow,                                                     \
                            aplDiv,                                                     \
                            aplExp = {(UINT) lppnLocalVars->uAlpAccDec};                \
                                                                                        \
                /* Calculate lpYYBase to the power uAlpAccDec                         */\
                aplPow = PowHC##N##F_RE (lpYYBase->at.aplHC##N##F, aplExp);             \
                                                                                        \
                /* Divide pnBaseDec by the power                                      */\
                aplDiv = DivHC##N##F_RE (pnBaseDec.at.aplHC##N##F, aplPow);             \
                                                                                        \
                /* Add to pnBaseInt for the result                                    */\
                pnBaseInt.at.aplHC##N##F =                                              \
                  AddHC##N##F_RE (pnBaseInt.at.aplHC##N##F, aplDiv);                    \
                                                                                        \
                /* We're done                                                         */\
                bContinue = FALSE;                                                      \
                                                                                        \
                break;                                                                  \
            } // End PN_NUMTYPE_HCxF

            case PN_NUMTYPE_FLT:
                PN_BaseHCxF_MAC (1)

            case PN_NUMTYPE_HC2F:
                PN_BaseHCxF_MAC (2)

            case PN_NUMTYPE_HC4F:
                PN_BaseHCxF_MAC (4)

            case PN_NUMTYPE_HC8F:
                PN_BaseHCxF_MAC (8)


#define PN_BaseHCxR_MAC(N)                                                              \
            {                                                                           \
                APLHC##N##R aplPow,                                                     \
                            aplDiv,                                                     \
                            aplExp = {0},                                               \
                            aplAdd;                                                     \
                                                                                        \
                /* Initialize the exponent to 0/1                                     */\
                mphc##N##r_init (&aplExp);                                              \
                                                                                        \
                /* Save the exponent as HCxR                                          */\
                mpq_set_sx ((LPAPLRAT) &aplExp, lppnLocalVars->uAlpAccDec, 1);          \
                                                                                        \
                /* Calculate lpYYBase to the power uAlpAccDec                         */\
                aplPow = PowHC##N##R_RE (lpYYBase->at.aplHC##N##R, aplExp);             \
                                                                                        \
                /* Divide pnBaseDec by the power                                      */\
                aplDiv = DivHC##N##R_RE (pnBaseDec.at.aplHC##N##R, aplPow);             \
                                                                                        \
                /* Add to pnBaseInt for the result                                    */\
                aplAdd =                                                                \
                  AddHC##N##R_RE (pnBaseInt.at.aplHC##N##R, aplDiv);                    \
                mphc##N##r_set (&pnBaseInt.at.aplHC##N##R, &aplAdd);                    \
                                                                                        \
                /* We no longer need this storage                                     */\
                Myhc##N##r_clear (&aplAdd);                                             \
                Myhc##N##r_clear (&aplDiv);                                             \
                Myhc##N##r_clear (&aplPow);                                             \
                Myhc##N##r_clear (&aplExp);                                             \
                                                                                        \
                /* We're done                                                         */\
                bContinue = FALSE;                                                      \
                                                                                        \
                break;                                                                  \
            } // End PN_NUMTYPE_HCxR

            case PN_NUMTYPE_RAT:
                PN_BaseHCxR_MAC (1)

            case PN_NUMTYPE_HC2R:
                PN_BaseHCxR_MAC (2)

            case PN_NUMTYPE_HC4R:
                PN_BaseHCxR_MAC (4)

            case PN_NUMTYPE_HC8R:
                PN_BaseHCxR_MAC (8)


#define PN_BaseHCxV_MAC(N)                                                              \
            {                                                                           \
                APLHC##N##V aplPow,                                                     \
                            aplDiv,                                                     \
                            aplExp = {0},                                               \
                            aplAdd;                                                     \
                                                                                        \
                /* Initialize the exponent to 0                                       */\
                mphc##N##v_init0 (&aplExp);                                             \
                                                                                        \
                /* Save the exponent as HCxV                                          */\
                mpfr_set_sx ((LPAPLVFP) &aplExp, lppnLocalVars->uAlpAccDec, MPFR_RNDN); \
                                                                                        \
                /* Calculate lpYYBase to the power uAlpAccDec                         */\
                aplPow = PowHC##N##V_RE (lpYYBase->at.aplHC##N##V, aplExp);             \
                                                                                        \
                /* Divide pnBaseDec by the power                                      */\
                aplDiv = DivHC##N##V_RE (pnBaseDec.at.aplHC##N##V, aplPow);             \
                                                                                        \
                /* Add to pnBaseInt for the result                                    */\
                aplAdd =                                                                \
                  AddHC##N##V_RE (pnBaseInt.at.aplHC##N##V, aplDiv);                    \
                mphc##N##v_set (&pnBaseInt.at.aplHC##N##V, &aplAdd);                    \
                                                                                        \
                /* We no longer need this storage                                     */\
                Myhc##N##v_clear (&aplAdd);                                             \
                Myhc##N##v_clear (&aplDiv);                                             \
                Myhc##N##v_clear (&aplPow);                                             \
                Myhc##N##v_clear (&aplExp);                                             \
                                                                                        \
                /* We're done                                                         */\
                bContinue = FALSE;                                                      \
                                                                                        \
                break;                                                                  \
            } // End PN_NUMTYPE_HCxV

            case PN_NUMTYPE_VFP:
                PN_BaseHCxV_MAC (1)

            case PN_NUMTYPE_HC2V:
                PN_BaseHCxV_MAC (2)

            case PN_NUMTYPE_HC4V:
                PN_BaseHCxV_MAC (4)

            case PN_NUMTYPE_HC8V:
                PN_BaseHCxV_MAC (8)

#define PN_BaseBAxF_MAC(N)                                                              \
            {                                                                           \
                APLBA##N##F aplPow,                                                     \
                            aplDiv,                                                     \
                            aplExp = {0},                                               \
                            aplAdd;                                                     \
                                                                                        \
                /* Initialize the exponent to 0                                       */\
                arb##N##f_init (&aplExp);                                               \
                                                                                        \
                /* Save the exponent as BAxF                                          */\
                arb_set_sx ((LPAPLBA1F) &aplExp, lppnLocalVars->uAlpAccDec);            \
                                                                                        \
                /* Calculate lpYYBase to the power uAlpAccDec                         */\
                aplPow = PowBA##N##F_RE (lpYYBase->at.aplBA##N##F, aplExp);             \
                                                                                        \
                /* Divide pnBaseDec by the power                                      */\
                aplDiv = DivBA##N##F_RE (pnBaseDec.at.aplBA##N##F, aplPow);             \
                                                                                        \
                /* Add to pnBaseInt for the result                                    */\
                aplAdd =                                                                \
                  AddBA##N##F_RE (pnBaseInt.at.aplBA##N##F, aplDiv);                    \
                arb##N##f_set (&pnBaseInt.at.aplBA##N##F, &aplAdd);                     \
                                                                                        \
                /* We no longer need this storage                                     */\
                arb##N##f_clear (&aplAdd);                                              \
                arb##N##f_clear (&aplDiv);                                              \
                arb##N##f_clear (&aplPow);                                              \
                arb##N##f_clear (&aplExp);                                              \
                                                                                        \
                /* We're done                                                         */\
                bContinue = FALSE;                                                      \
                                                                                        \
                break;                                                                  \
            } // End PN_NUMTYPE_HCxV

            case PN_NUMTYPE_BA1F:
                PN_BaseBAxF_MAC (1)

            case PN_NUMTYPE_BA2F:
                PN_BaseBAxF_MAC (2)

            case PN_NUMTYPE_BA4F:
                PN_BaseBAxF_MAC (4)

            case PN_NUMTYPE_BA8F:
                PN_BaseBAxF_MAC (8)

            defstop
                break;
        } // End WHILE/SWITCH
    } // End IF

    // The result is in <pnBaseInt>

    // Copy <pnBaseInt> to <lpYYBase>
    PN_Copy (lpYYBase, &pnBaseInt);

    goto NORMAL_EXIT;

ERROR_EXIT:
    // We no longer need this storage
    PN_Free (lpYYBase  ); lpYYBase = NULL;
NORMAL_EXIT:
    // Clear the accumulators
    lppnLocalVars->uAlpAccInt =
    lppnLocalVars->uAlpAccDec = 0;

    // We no longer need this storage
    PN_Free (&pnBaseInt);
    PN_Free (&pnBaseDec);

    return lpYYBase;
} // End PN_MakeBasePoint


//***************************************************************************
//  $PN_MakeBasePointSub
//
//  Merge the base and AlphaInt part to form a number
//***************************************************************************

UBOOL PN_MakeBasePointSub
    (LPPN_YYSTYPE  lpYYBase,            // Ptr to the base part
     LPPN_YYSTYPE  lpYYAlphaInt,        // Ptr to the Integer AlphaInt part
     LPCHAR        lpszAlphaInt,        // Ptr to the beginning of szAlphaInt buffer
     size_t        uLen,                // Length of lpAlphaInt
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    UINT       uAcc;                    // Loop counter
    APLINT     aplIntBase,              // Base as an integer
               aplIntPowBase,           // Base as successive powers
               aplIntTmp,               // Temporary integer
               aplIntAcc;               // Temporary accumulator
    APLFLOAT   aplFltBase,              // Base as a  float
               aplFltPowBase,           // Base as successive powers
               aplFltAcc;               // Temporary accumulator
    APLRAT     aplRatCur = {0};         // Temporary RAT for chCur
    APLVFP     aplVfpCur = {0};         // Temporary VFP for chCur
    APLARB     aplArbCur = {0};         // Temporary ARB for chCur
    ALLTYPES   atAcc     = {0},         // Accumulator as ALLTYPES
               atBase    = {0},         // Base        ...
               atPowBase = {0};         // Power of base ...
    UBOOL      bRet = TRUE,             // TRUE iff the result is valid
               bNeg = FALSE;            // TRUE iff the result is negative
    UCHAR      chCur;                   // Temporary character
    LPCHAR     lpszAlphaEnd;            // Ptr to end of szAlphaInt buffer
    PNNUMTYPE  chType;                  // The numeric type (see PNNUMTYPE)
    PN_YYSTYPE pnBase;                  // Local copy of <lpYYBase>
    int        i,                       // Loop counter
               iHCDim;                  // HC Dimension (1, 2, 4, 8)
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Copy the arg in case we promote
    pnBase = *lpYYBase;
RESTART_BASEPOINT:
    // Is the first char a negative sign?
    bNeg = uAcc = (lpszAlphaInt[0] EQ OVERBAR1);

    // Get the ptr to the end of the AlphaInt string
    lpszAlphaEnd = &lpszAlphaInt[uLen - 1];

    // Get the numeric type
    chType = (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT) ? pnBase.chType : lppnLocalVars->chComType;

    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDim = TranslateNumTypeToHCDim (chType);

    __try
    {
        // Split cases based upon the numeric type of the base
        switch (chType)
        {
            case PN_NUMTYPE_BOOL:
            case PN_NUMTYPE_INT:
                // Get the base value
                aplIntBase = pnBase.at.aplInteger;

                // Initialize the accumulator and base power
                aplIntAcc = 0;
                aplIntPowBase = 1;

                // Loop through the AlphaInt arg
                for (; bRet && uAcc < uLen; uAcc++)
                {
                    // Get the next char as lowercase
                    chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaEnd--));

                    // Convert to a number
                    if ('0' <= chCur
                     &&        chCur <= '9')
                        chCur -= '0';
                    else
                    if ('a' <= chCur
                     &&        chCur <= 'z')
                        chCur -= 'a' - 10;

                    // Times the power base
                    aplIntTmp = imul64 (chCur, aplIntPowBase, &bRet, EXCEPTION_RESULT_FLOAT);
                    if (!bRet)
                        break;

                    // Accumulate
                    aplIntAcc = iadd64 (aplIntAcc, aplIntTmp, &bRet, EXCEPTION_RESULT_FLOAT);
                    if (!bRet)
                        break;

                    // Shift over the power base
                    aplIntPowBase = imul64 (aplIntPowBase, aplIntBase, &bRet, EXCEPTION_RESULT_FLOAT);

                    // Ignore spurious overflow on last digit
                    bRet |= (uAcc EQ (uLen - 1));

////////////////////if (!bRet)          // Not needed as we're at the end of the FOR loop
////////////////////    break;
                } // End FOR

                if (bRet)
                {
                    // If the result is negative, ...
                    if (bNeg)
                        aplIntAcc = -aplIntAcc;

                    // Save the result
                    lpYYBase->at.aplInteger = aplIntAcc;
                    lpYYBase->chType        = PN_NUMTYPE_INT;

                    break;
                } // End IF

                // Convert the base value to Rational
                mpq_init_set_sx (&lpYYBase->at.aplRat, pnBase.at.aplInteger, 1);

                // Change the type to RAT
                lpYYBase->chType = pnBase.chType = PN_NUMTYPE_RAT;

                goto RESTART_BASEPOINT;

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
                for (; uAcc < uLen; uAcc++)
                {
                    // Get the next char as lowercase
                    chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaEnd--));

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

                // If the result is negative, ...
                if (bNeg)
                    // Negate the result
                    mpq_neg (&lpYYBase->at.aplRat, &lpYYBase->at.aplRat);

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
                for (; uAcc < uLen; uAcc++)
                {
                    // Get the next char as lowercase
                    chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaEnd--));

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

                // If the result is negative, ...
                if (bNeg)
                    // Negate the result
                    mpfr_neg (&lpYYBase->at.aplVfp, &lpYYBase->at.aplVfp, MPFR_RNDN);

                // We no longer need this storage
                Myf_clear (&aplVfpTmp);
                Myf_clear (&aplVfpPowBase);
                Myf_clear (&aplVfpBase);

                break;
            } // End PN_NUMTYPE_VFP

            case PN_NUMTYPE_ARB:
            {
                APLARB aplArbBase    = {0},
                       aplArbPowBase = {0},
                       aplArbTmp     = {0};

                // Initialize and set the base value
                arb_init_set (&aplArbBase, &lpYYBase->at.aplArb);

                // Initialize the accumulator and base power
                arb_set_sx (&lpYYBase->at.aplArb, 0);
                arb_init_set_sx (&aplArbPowBase, 1);
                Myarb_init (&aplArbTmp);

                // Loop through the AlphaInt arg
                for (; uAcc < uLen; uAcc++)
                {
                    // Get the next char as lowercase
                    chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaEnd--));

                    // Convert to a number
                    if ('0' <= chCur
                     &&        chCur <= '9')
                        chCur -= '0';
                    else
                    if ('a' <= chCur
                     &&        chCur <= 'z')
                        chCur -= 'a' - 10;

                    // Times the power base and accumulate
                    arb_set_sx (&aplArbTmp, chCur);
                    arb_mul (&aplArbTmp, &aplArbPowBase, &aplArbTmp, prec);
                    arb_add (&lpYYBase->at.aplArb, &lpYYBase->at.aplArb, &aplArbTmp, prec);

                    // Shift over the power base
                    arb_mul (&aplArbPowBase, &aplArbPowBase, &aplArbBase, prec);
                } // End FOR

                // If the result is negative, ...
                if (bNeg)
                    // Negate the result
                    arb_neg (&lpYYBase->at.aplArb, &lpYYBase->at.aplArb);

                // We no longer need this storage
                Myarb_clear (&aplArbTmp);
                Myarb_clear (&aplArbPowBase);
                Myarb_clear (&aplArbBase);

                break;
            } // End PN_NUMTYPE_ARB

            case PN_NUMTYPE_FLT:
                // Get the base value
                aplFltBase = pnBase.at.aplFloat;

                // Initialize the accumulator and base power
                aplFltAcc = 0;
                aplFltPowBase = 1;

                // Loop through the AlphaInt arg
                for (; uAcc < uLen; uAcc++)
                {
                    // Get the next char as lowercase
                    chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaEnd--));

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

                // If the result is negative, ...
                if (bNeg)
                    // Negate the result
                    aplFltAcc = -aplFltAcc;

                // Save the result
                lpYYBase->at.aplFloat = aplFltAcc;

                break;

            case PN_NUMTYPE_HC2I:
            case PN_NUMTYPE_HC4I:
            case PN_NUMTYPE_HC8I:
                // Get the base value
                atBase = pnBase.at;

                // Initialize the accumulator to 0 and base power to 1
                ZeroMemory (&atAcc,     sizeof (atAcc    ));
                ZeroMemory (&atPowBase, sizeof (atPowBase));
                atPowBase.aplHC8I.parts[0] = 1;

                // Loop through the AlphaInt arg
                for (; uAcc < uLen; uAcc++)
                {
                    // Get the next char as lowercase
                    chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaEnd--));

                    // Convert to a number
                    if ('0' <= chCur
                     &&        chCur <= '9')
                        chCur -= '0';
                    else
                    if ('a' <= chCur
                     &&        chCur <= 'z')
                        chCur -= 'a' - 10;

                    // Loop through all of the parts
                    for (i = 0; i < iHCDim; i++)
                        // Times the power base and accumulate
                        atAcc.aplHC8I.parts[i] += chCur * atPowBase.aplHC8I.parts[i];

                    // Split cases based upon the dimension
                    switch (iHCDim)
                    {
                        case 2:
                            // Shift over the power base
                            atPowBase.aplHC2I = MulHC2I_RE (atBase.aplHC2I, atPowBase.aplHC2I);

                            break;

                        case 4:
                            // Shift over the power base
                            atPowBase.aplHC4I = MulHC4I_RE (atBase.aplHC4I, atPowBase.aplHC4I);

                            break;

                        case 8:
                            // Shift over the power base
                            atPowBase.aplHC8I = MulHC8I_RE (atBase.aplHC8I, atPowBase.aplHC8I);

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End FOR

                // If the result is negative, ...
                if (bNeg)
                // Loop through all of the parts
                for (i = 0; i < iHCDim; i++)
                    // Negate the result
                    atAcc.aplHC8I.parts[i] = -atAcc.aplHC8I.parts[i];

                // Save the result
                lpYYBase->chType = chType;
                lpYYBase->at     = atAcc;

                break;

            case PN_NUMTYPE_HC2F:
            case PN_NUMTYPE_HC4F:
            case PN_NUMTYPE_HC8F:
                // Get the base value
                atBase = pnBase.at;

                // Initialize the accumulator to 0 and base power to 1
                ZeroMemory (&atAcc,     sizeof (atAcc    ));
                ZeroMemory (&atPowBase, sizeof (atPowBase));
                atPowBase.aplHC8F.parts[0] = 1;

                // Loop through the AlphaInt arg
                for (; uAcc < uLen; uAcc++)
                {
                    // Get the next char as lowercase
                    chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaEnd--));

                    // Convert to a number
                    if ('0' <= chCur
                     &&        chCur <= '9')
                        chCur -= '0';
                    else
                    if ('a' <= chCur
                     &&        chCur <= 'z')
                        chCur -= 'a' - 10;

                    // Loop through all of the parts
                    for (i = 0; i < iHCDim; i++)
                        // Times the power base and accumulate
                        atAcc.aplHC8F.parts[i] += chCur * atPowBase.aplHC8F.parts[i];

                    // Split cases based upon the dimension
                    switch (iHCDim)
                    {
                        case 2:
                            // Shift over the power base
                            atPowBase.aplHC2F = MulHC2F_RE (atBase.aplHC2F, atPowBase.aplHC2F);

                            break;

                        case 4:
                            // Shift over the power base
                            atPowBase.aplHC4F = MulHC4F_RE (atBase.aplHC4F, atPowBase.aplHC4F);

                            break;

                        case 8:
                            // Shift over the power base
                            atPowBase.aplHC8F = MulHC8F_RE (atBase.aplHC8F, atPowBase.aplHC8F);

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End FOR

                // If the result is negative, ...
                if (bNeg)
                // Loop through all of the parts
                for (i = 0; i < iHCDim; i++)
                    // Negate the result
                    atAcc.aplHC8F.parts[i] = -atAcc.aplHC8F.parts[i];

                // Save the result
                lpYYBase->chType = chType;
                lpYYBase->at     = atAcc;

                break;

            case PN_NUMTYPE_HC2R:
            case PN_NUMTYPE_HC4R:
            case PN_NUMTYPE_HC8R:
                // Get the base value
                atBase = pnBase.at;

                // Initialize the accumulator to 0 and base power to 1
                // Loop through all of the parts
                for (i = 0; i < iHCDim; i++)
                {
                    // Initialize to 0/1
                    mpq_init (&atAcc    .aplHC8R.parts[i]);
                    mpq_init (&atPowBase.aplHC8R.parts[i]);
                } // End FOR

                // Initialize the real part to 1/1
                mpq_set_sx (&atPowBase.aplHC8R.parts[0], 1, 1);

                // Initialize to 0/1
                mpq_init (&aplRatCur);

                // Loop through the AlphaInt arg
                for (; uAcc < uLen; uAcc++)
                {
                    // Get the next char as lowercase
                    chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaEnd--));

                    // Convert to a number
                    if ('0' <= chCur
                     &&        chCur <= '9')
                        chCur -= '0';
                    else
                    if ('a' <= chCur
                     &&        chCur <= 'z')
                        chCur -= 'a' - 10;

                    // Loop through all of the parts
                    for (i = 0; i < iHCDim; i++)
                    {
                        // Convert <chCur> to a RAT
                        mpq_set_sx (&aplRatCur, chCur, 1);

                        // Times the power base
                        // chCur * atPowBase
                        mpq_mul (&aplRatCur, &aplRatCur, &atPowBase.aplHC8R.parts[i]);

                        // Accumulate
                        mpq_add (&atAcc.aplHC8R.parts[i], &atAcc.aplHC8R.parts[i], &aplRatCur);
                    } // End FOR

                    // Split cases based upon the dimension
                    switch (iHCDim)
                    {
                        ALLTYPES atTmp;

                        case 2:
                            // Shift over the power base
                            atTmp.aplHC2R = MulHC2R_RE (atBase.aplHC2R, atPowBase.aplHC2R);

                            // We no longer need this storage
                            Myhc2r_clear (&atPowBase.aplHC2R);

                            // Copy back
                            atPowBase.aplHC2R = atTmp.aplHC2R;

                            break;

                        case 4:
                            // Shift over the power base
                            atTmp.aplHC4R = MulHC4R_RE (atBase.aplHC4R, atPowBase.aplHC4R);

                            // We no longer need this storage
                            Myhc4r_clear (&atPowBase.aplHC4R);

                            // Copy back
                            atPowBase.aplHC4R = atTmp.aplHC4R;

                            break;

                        case 8:
                            // Shift over the power base
                            atTmp.aplHC8R = MulHC8R_RE (atBase.aplHC8R, atPowBase.aplHC8R);

                            // We no longer need this storage
                            Myhc8r_clear (&atPowBase.aplHC8R);

                            // Copy back
                            atPowBase.aplHC8R = atTmp.aplHC8R;

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End FOR

                // If the result is negative, ...
                if (bNeg)
                // Loop through all of the parts
                for (i = 0; i < iHCDim; i++)
                    // Negate the result
                    mpq_neg (&atAcc.aplHC8R.parts[i], &atAcc.aplHC8R.parts[i]);

                // Save the result
                lpYYBase->chType = chType;

                // Loop through all of the parts
                for (i = 0; i < iHCDim; i++)
                    mpq_set (&lpYYBase->at.aplHC8R.parts[i], &atAcc.aplHC8R.parts[i]);
                break;

            case PN_NUMTYPE_HC2V:
            case PN_NUMTYPE_HC4V:
            case PN_NUMTYPE_HC8V:
                // Get the base value
                atBase = pnBase.at;

                // Initialize the accumulator to 0 and base power to 1
                // Loop through all of the parts
                for (i = 0; i < iHCDim; i++)
                {
                    // Initialize to 0
                    mpfr_init0 (&atAcc    .aplHC8V.parts[i]);
                    mpfr_init0 (&atPowBase.aplHC8V.parts[i]);
                } // End FOR

                // Initialize the real part to 1/1
                mpfr_set_sx (&atPowBase.aplHC8V.parts[0], 1, MPFR_RNDN);

                // Initialize to 0
                mpfr_init0 (&aplVfpCur);

                // Loop through the AlphaInt arg
                for (; uAcc < uLen; uAcc++)
                {
                    // Get the next char as lowercase
                    chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaEnd--));

                    // Convert to a number
                    if ('0' <= chCur
                     &&        chCur <= '9')
                        chCur -= '0';
                    else
                    if ('a' <= chCur
                     &&        chCur <= 'z')
                        chCur -= 'a' - 10;

                    // Loop through all of the parts
                    for (i = 0; i < iHCDim; i++)
                    {
                        // Convert <chCur> to a VFP
                        mpfr_set_sx (&aplVfpCur, chCur, MPFR_RNDN);

                        // Times the power base
                        // chCur * atPowBase
                        mpfr_mul (&aplVfpCur, &aplVfpCur, &atPowBase.aplHC8V.parts[i], MPFR_RNDN);

                        // Accumulate
                        mpfr_add (&atAcc.aplHC8V.parts[i], &atAcc.aplHC8V.parts[i], &aplVfpCur, MPFR_RNDN);
                    } // End FOR

                    // Split cases based upon the dimension
                    switch (iHCDim)
                    {
                        ALLTYPES atTmp;

                        case 2:
                            // Shift over the power base
                            atTmp.aplHC2V = MulHC2V_RE (atBase.aplHC2V, atPowBase.aplHC2V);

                            // We no longer need this storage
                            Myhc2v_clear (&atPowBase.aplHC2V);

                            // Copy back
                            atPowBase.aplHC2V = atTmp.aplHC2V;

                            break;

                        case 4:
                            // Shift over the power base
                            atTmp.aplHC4V = MulHC4V_RE (atBase.aplHC4V, atPowBase.aplHC4V);

                            // We no longer need this storage
                            Myhc4v_clear (&atPowBase.aplHC4V);

                            // Copy back
                            atPowBase.aplHC4V = atTmp.aplHC4V;

                            break;

                        case 8:
                            // Shift over the power base
                            atTmp.aplHC8V = MulHC8V_RE (atBase.aplHC8V, atPowBase.aplHC8V);

                            // We no longer need this storage
                            Myhc8v_clear (&atPowBase.aplHC8V);

                            // Copy back
                            atPowBase.aplHC8V = atTmp.aplHC8V;

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End FOR

                // If the result is negative, ...
                if (bNeg)
                // Loop through all of the parts
                for (i = 0; i < iHCDim; i++)
                    // Negate the result
                    mpfr_neg (&atAcc.aplHC8V.parts[i], &atAcc.aplHC8V.parts[i], MPFR_RNDN);

                // Save the result
                lpYYBase->chType = chType;

                // Loop through all of the parts
                for (i = 0; i < iHCDim; i++)
                    mpfr_set (&lpYYBase->at.aplHC8V.parts[i], &atAcc.aplHC8V.parts[i], MPFR_RNDN);
                break;

            case PN_NUMTYPE_BA2F:
            case PN_NUMTYPE_BA4F:
            case PN_NUMTYPE_BA8F:
                // Get the base value
                atBase = pnBase.at;

                // Initialize the accumulator to 0 and base power to 1
                // Loop through all of the parts
                for (i = 0; i < iHCDim; i++)
                {
                    // Initialize to 0/0
                    Myarb_init (&atAcc    .aplBA8F.parts[i]);
                    Myarb_init (&atPowBase.aplBA8F.parts[i]);
                } // End FOR

                // Initialize the real part to 1/1
                arb_set_sx (&atPowBase.aplBA8F.parts[0], 1);

                // Initialize to 0/0
                Myarb_init (&aplArbCur);

                // Loop through the AlphaInt arg
                for (; uAcc < uLen; uAcc++)
                {
                    // Get the next char as lowercase
                    chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaEnd--));

                    // Convert to a number
                    if ('0' <= chCur
                     &&        chCur <= '9')
                        chCur -= '0';
                    else
                    if ('a' <= chCur
                     &&        chCur <= 'z')
                        chCur -= 'a' - 10;

                    // Loop through all of the parts
                    for (i = 0; i < iHCDim; i++)
                    {
                        // Convert <chCur> to a ARB
                        arb_set_sx (&aplArbCur, chCur);

                        // Times the power base
                        // chCur * atPowBase
                        arb_mul (&aplArbCur, &aplArbCur, &atPowBase.aplBA8F.parts[i], prec);

                        // Accumulate
                        arb_add (&atAcc.aplBA8F.parts[i], &atAcc.aplBA8F.parts[i], &aplArbCur, prec);
                    } // End FOR

                    // Split cases based upon the dimension
                    switch (iHCDim)
                    {
                        ALLTYPES atTmp;

                        case 2:
                            // Shift over the power base
                            atTmp.aplBA2F = MulBA2F_RE (atBase.aplBA2F, atPowBase.aplBA2F);

                            // We no longer need this storage
                            arb2f_clear (&atPowBase.aplBA2F);

                            // Copy back
                            atPowBase.aplBA2F = atTmp.aplBA2F;

                            break;

                        case 4:
                            // Shift over the power base
                            atTmp.aplBA4F = MulBA4F_RE (atBase.aplBA4F, atPowBase.aplBA4F);

                            // We no longer need this storage
                            arb4f_clear (&atPowBase.aplBA4F);

                            // Copy back
                            atPowBase.aplBA4F = atTmp.aplBA4F;

                            break;

                        case 8:
                            // Shift over the power base
                            atTmp.aplBA8F = MulBA8F_RE (atBase.aplBA8F, atPowBase.aplBA8F);

                            // We no longer need this storage
                            arb8f_clear (&atPowBase.aplBA8F);

                            // Copy back
                            atPowBase.aplBA8F = atTmp.aplBA8F;

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End FOR

                // If the result is negative, ...
                if (bNeg)
                // Loop through all of the parts
                for (i = 0; i < iHCDim; i++)
                    // Negate the result
                    arb_neg (&atAcc.aplBA8F.parts[i], &atAcc.aplBA8F.parts[i]);

                // Save the result
                lpYYBase->chType = chType;

                // Loop through all of the parts
                for (i = 0; i < iHCDim; i++)
                    arb_set (&lpYYBase->at.aplBA8F.parts[i], &atAcc.aplBA8F.parts[i]);
                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        EXCEPTION_CODES ExceptionCode = MyGetExceptionCode ();  // The exception code
        PNNUMTYPE       pnType;             // Temp PNNUMTYPE
        UINT            uHCDimIndex;        // HC Dimension Index (0, 1, 2, 3)

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L" #2: %s (%S#%d)", MyGetExceptionStr (ExceptionCode), FNLN);

        // Get the new result type
        pnType = TranslateExceptionCodeToNumType (ExceptionCode);

        // Convert iHCDim to uHCDimIndex
        uHCDimIndex = aHCDimToIndex[iHCDim];

        // Reset THE EXCEPTION CODE
        MySetExceptionCode (EXCEPTION_SUCCESS);

        // Split cases based upon the ExceptionCode
        switch (ExceptionCode)
        {
            case EXCEPTION_DOMAIN_ERROR:
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                goto DOMAIN_EXIT;

            case EXCEPTION_WS_FULL:
                goto WSFULL_EXIT;

            case EXCEPTION_NONCE_ERROR:
                goto NONCE_EXIT;

            case EXCEPTION_RESULT_FLOAT:
            case EXCEPTION_RESULT_HC2F:
            case EXCEPTION_RESULT_HC4F:
            case EXCEPTION_RESULT_HC8F:
                // Calculate the result type
                pnType = TranslateArrayTypeToNumType (uHCxF[uHCDimIndex]);

                // Promote the arg to HCxF
                (*aNumTypeAction[lpYYBase->chType][pnType]) (&pnBase, lppnLocalVars);

                goto RESTART_BASEPOINT;

            case EXCEPTION_RESULT_VFP:
            case EXCEPTION_RESULT_HC2V:
            case EXCEPTION_RESULT_HC4V:
            case EXCEPTION_RESULT_HC8V:
                // Calculate the result type
                pnType = TranslateArrayTypeToNumType (uHCxV[uHCDimIndex]);

                // Promote the arg to HCxV
                (*aNumTypeAction[lpYYBase->chType][pnType]) (&pnBase, lppnLocalVars);

                goto RESTART_BASEPOINT;

            case EXCEPTION_RESULT_ARB:
            case EXCEPTION_RESULT_BA2F:
            case EXCEPTION_RESULT_BA4F:
            case EXCEPTION_RESULT_BA8F:
                // Calculate the result type
                pnType = TranslateArrayTypeToNumType (uBAxF[uHCDimIndex]);

                // Promote the arg to BAxF
                (*aNumTypeAction[lpYYBase->chType][pnType]) (&pnBase, lppnLocalVars);

                goto RESTART_BASEPOINT;

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    // Signal the error
    pn_yyerror (lppnLocalVars, PN_DOMAIN);

    goto ERROR_EXIT;

NONCE_EXIT:
    // Signal the error
    pn_yyerror (lppnLocalVars, PN_NONCE);

    goto ERROR_EXIT;

WSFULL_EXIT:
    // Signal the error
    pn_yyerror (lppnLocalVars, PN_WSFULL);

    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as in error
    lpYYBase = NULL;

    goto ERROR_EXIT2;

NORMAL_EXIT:
ERROR_EXIT2:
    Myarb_clear (&aplArbCur);
    Myq_clear (&aplRatCur);
    Myf_clear (&aplVfpCur);

    return (lpYYBase NE NULL);
} // End PN_MakeBasePointSub


//***************************************************************************
//  $PN_MakeEulerPoint
//
//  Merge the Multiplier and Exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeEulerPoint
    (LPPN_YYSTYPE  lpYYMultiplier,      // Ptr to the multiplier part
     LPPN_YYSTYPE  lpYYExponent,        // Ptr to the exponent part
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    PNNUMTYPE pnTypeRes;                // Result PNNUMTYPE
    ALLTYPES  atTmp = {0},              // Temp as ALLTYPES
              atExp = {0},              // ...
              atMul = {0},              // ...
              atRes = {0};              // Result ...
    int       i,                        // Loop counter
              iHCDimRes;                // HC Dimension (1, 2, 4, 8)

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // If this is the first time through, ...
    if (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT)
    {
        // Promote the Multiplier and Exponent to a common format
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
    } else
        // Use common type
        pnTypeRes = lppnLocalVars->chComType;

    // If the Multiplier must be promoted, ...
    if (pnTypeRes NE lpYYMultiplier->chType
     && aNumTypeAction[lpYYMultiplier->chType][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYMultiplier->chType][pnTypeRes]) (lpYYMultiplier, lppnLocalVars);

    // If the exponent must be promoted, ...
    if (pnTypeRes NE lpYYExponent->chType
     && aNumTypeAction[lpYYExponent->chType  ][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYExponent->chType  ][pnTypeRes]) (lpYYExponent, lppnLocalVars);

    // Set the result type
    lpYYMultiplier->chType =
    lpYYExponent->chType   = pnTypeRes;

    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDimRes = TranslateArrayTypeToHCDim (TranslateNumTypeToArrayType (pnTypeRes));

    // Split cases based upon the numeric type of the result
    switch (pnTypeRes)
    {
////////case PN_NUMTYPE_BOOL:
////////case PN_NUMTYPE_INT:
////////    // The result is Multiplier x (*1) * Exponent
////////    lpYYMultiplier->at.aplFloat *= pow (FloatE, (APLFLOAT) lpYYExponent->at.aplInteger);
////////
////////    break;
////////
        case PN_NUMTYPE_FLT:
            // The result is Multiplier x (*1) * Exponent
            lpYYMultiplier->at.aplFloat *= pow (FloatE, lpYYExponent->at.aplFloat);

            break;

        case PN_NUMTYPE_VFP:
        {
            LPPERTABDATA lpMemPTD = GetMemPTD ();   // Ptr to PerTabData global memory handle
#if (defined DEBUG) && (defined DEBUG_FMT)
            WCHAR wszTemp[512];
#endif
            // Initialize the temp array
            mpfr_init0 (&atRes.aplVfp);
#if (defined DEBUG) && (defined DEBUG_FMT)
            strcpyW (wszTemp, L"Mul:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], lpYYMultiplier->at.aplVfp, 0) = WC_EOS; DbgMsgW (wszTemp);
            strcpyW (wszTemp, L"Exp:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], lpYYExponent->at.aplVfp, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
            // Initialize the VFP E if not already done
            InitPTD_E (lpMemPTD);

            // The result is Multiplier x (*1) * Exponent
            mpfr_pow (&atRes.aplVfp, &lpMemPTD->mpfrHC8V_E.parts[0], &lpYYExponent->at.aplVfp, MPFR_RNDN);
#if (defined DEBUG) && (defined DEBUG_FMT)
            strcpyW (wszTemp, L"e *:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], atRes.aplVfp, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
            // Accumulate in the Multiplier
            mpfr_mul (&lpYYMultiplier->at.aplVfp, &lpYYMultiplier->at.aplVfp, &atRes.aplVfp, MPFR_RNDN);
#if (defined DEBUG) && (defined DEBUG_FMT)
            strcpyW (wszTemp, L"Res:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], lpYYMultiplier->at.aplVfp, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
            // We no longer need this storage
            Myf_clear (&atRes.aplVfp);
            Myf_clear (&lpYYExponent->at.aplVfp);

            break;
        } // End PN_NUMTYPE_VFP

////////case PN_NUMTYPE_HC2I:
////////    // Convert the Exponent to the result type
////////    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (&lpYYExponent->at.aplHC2I, 0, &atExp);
////////
////////    // Calculate (*1) * Exponent
////////    atExp.aplHC2F = ExpHC2F_RE (atExp.aplHC2F);
////////
////////    // Convert the Multiplier to the result type
////////    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (&lpYYMultiplier->at.aplHC2I, 0, &atMul);
////////
////////    // The result is Multiplier x (*1) * Exponent
////////    lpYYMultiplier->at.aplHC2F = MulHC2F_RE (atMul.aplHC2F, atExp.aplHC2F);
////////
////////    // Change the result type
////////    lpYYMultiplier->chType = PN_NUMTYPE_HC2F;
////////
////////    break;
////////
////////case PN_NUMTYPE_HC4I:
////////    // Convert the Exponent to the result type
////////    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (&lpYYExponent->at.aplHC4I, 0, &atExp);
////////
////////    // Calculate (*1) * Exponent
////////    atExp.aplHC4F = ExpHC4F_RE (atExp.aplHC4F);
////////
////////    // Convert the Multiplier to the result type
////////    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (&lpYYMultiplier->at.aplHC4I, 0, &atMul);
////////
////////    // The result is Multiplier x (*1) * Exponent
////////    lpYYMultiplier->at.aplHC4F = MulHC4F_RE (atMul.aplHC4F, atExp.aplHC4F);
////////
////////    // Change the result type
////////    lpYYMultiplier->chType = PN_NUMTYPE_HC4F;
////////
////////    break;
////////
////////case PN_NUMTYPE_HC8I:
////////    // Convert the Exponent to the result type
////////    (*aTypeActPromote[ARRAY_HC8I][ARRAY_HC8F]) (&lpYYExponent->at.aplHC8I, 0, &atExp);
////////
////////    // Calculate (*1) * Exponent
////////    atExp.aplHC8F = ExpHC8F_RE (atExp.aplHC8F);
////////
////////    // Convert the Multiplier to the result type
////////    (*aTypeActPromote[ARRAY_HC8I][ARRAY_HC8F]) (&lpYYMultiplier->at.aplHC8I, 0, &atMul);
////////
////////    // The result is Multiplier x (*1) * Exponent
////////    lpYYMultiplier->at.aplHC8F = MulHC8F_RE (atMul.aplHC8F, atExp.aplHC8F);
////////
////////    // Change the result type
////////    lpYYMultiplier->chType = PN_NUMTYPE_HC8F;
////////
////////    break;
////////
        case PN_NUMTYPE_HC2F:
            // The result is Multiplier x (*1) * Exponent
            lpYYMultiplier->at.aplHC2F = MulHC2F_RE (lpYYMultiplier->at.aplHC2F,
                                                     ExpHC2F_RE (lpYYExponent->at.aplHC2F));
            break;

        case PN_NUMTYPE_HC4F:
            // The result is Multiplier x (*1) * Exponent
            lpYYMultiplier->at.aplHC4F = MulHC4F_RE (lpYYMultiplier->at.aplHC4F,
                                                     ExpHC4F_RE (lpYYExponent->at.aplHC4F));
            break;

        case PN_NUMTYPE_HC8F:
            // The result is Multiplier x (*1) * Exponent
            lpYYMultiplier->at.aplHC8F = MulHC8F_RE (lpYYMultiplier->at.aplHC8F,
                                                     ExpHC8F_RE (lpYYExponent->at.aplHC8F));
            break;

        case PN_NUMTYPE_HC2R:
            // Convert the Exponent to the result type
            (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (&lpYYExponent->at.aplHC2R, 0, &atExp);

            // Calculate (*1) * Exponent
            atTmp.aplHC2V = ExpHC2V_RE (atExp.aplHC2V);

            // Convert the Multiplier to the result type
            (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (&lpYYMultiplier->at.aplHC2R, 0, &atMul);

            // The result is Multiplier x (*1) * Exponent
            atRes.aplHC2V = MulHC2V_RE (atMul.aplHC2V, atTmp.aplHC2V);

            // Loop through all of the parts
            for (i = 0; i < iHCDimRes; i++)
                // Save in the result
                mpfr_set (&lpYYMultiplier->at.aplHC2V.parts[i], &atRes.aplHC2V.parts[i], MPFR_RNDN);

            // Change the result type
            lpYYMultiplier->chType = PN_NUMTYPE_HC2V;

            // We no longer need this storage
            Myhc2v_clear (&atRes.aplHC2V);
            Myhc2v_clear (&atMul.aplHC2V);
            Myhc2v_clear (&atTmp.aplHC2V);
            Myhc2v_clear (&atExp.aplHC2V);

            break;

        case PN_NUMTYPE_HC4R:
            // Convert the Exponent to the result type
            (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (&lpYYExponent->at.aplHC4R, 0, &atExp);

            // Calculate (*1) * Exponent
            atTmp.aplHC4V = ExpHC4V_RE (atExp.aplHC4V);

            // Convert the Multiplier to the result type
            (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (&lpYYMultiplier->at.aplHC4R, 0, &atMul);

            // The result is Multiplier x (*1) * Exponent
            atRes.aplHC4V = MulHC4V_RE (atMul.aplHC4V, atTmp.aplHC4V);

            // Loop through all of the parts
            for (i = 0; i < iHCDimRes; i++)
                // Save in the result
                mpfr_set (&lpYYMultiplier->at.aplHC4V.parts[i], &atRes.aplHC4V.parts[i], MPFR_RNDN);

            // Change the result type
            lpYYMultiplier->chType = PN_NUMTYPE_HC4V;

            // We no longer need this storage
            Myhc4v_clear (&atRes.aplHC4V);
            Myhc4v_clear (&atMul.aplHC4V);
            Myhc4v_clear (&atTmp.aplHC4V);
            Myhc4v_clear (&atExp.aplHC4V);

            break;

        case PN_NUMTYPE_HC8R:
            // Convert the Exponent to the result type
            (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (&lpYYExponent->at.aplHC8R, 0, &atExp);

            // Calculate (*1) * Exponent
            atTmp.aplHC8V = ExpHC8V_RE (atExp.aplHC8V);

            // Convert the Multiplier to the result type
            (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (&lpYYMultiplier->at.aplHC8R, 0, &atMul);

            // The result is Multiplier x (*1) * Exponent
            atRes.aplHC8V = MulHC8V_RE (atMul.aplHC8V, atTmp.aplHC8V);

            // Loop through all of the parts
            for (i = 0; i < iHCDimRes; i++)
                // Save in the result
                mpfr_set (&lpYYMultiplier->at.aplHC8V.parts[i], &atRes.aplHC8V.parts[i], MPFR_RNDN);

            // Change the result type
            lpYYMultiplier->chType = PN_NUMTYPE_HC8V;

            // We no longer need this storage
            Myhc8v_clear (&atRes.aplHC8V);
            Myhc8v_clear (&atMul.aplHC8V);
            Myhc8v_clear (&atTmp.aplHC8V);
            Myhc8v_clear (&atExp.aplHC8V);

            break;

        case PN_NUMTYPE_HC2V:
            // Calculate (*1) * Exponent
            atTmp.aplHC2V = ExpHC2V_RE (lpYYExponent->at.aplHC2V);

            // The result is Multiplier x (*1) * Exponent
            atRes.aplHC2V = MulHC2V_RE (lpYYMultiplier->at.aplHC2V, atTmp.aplHC2V);

            // Loop through all of the parts
            for (i = 0; i < iHCDimRes; i++)
                // Save in the result
                mpfr_set (&lpYYMultiplier->at.aplHC2V.parts[i], &atRes.aplHC2V.parts[i], MPFR_RNDN);

            // We no longer need this storage
            Myhc2v_clear (&atRes.aplHC2V);
            Myhc2v_clear (&atTmp.aplHC2V);

            break;

        case PN_NUMTYPE_HC4V:
            // Calculate (*1) * Exponent
            atTmp.aplHC4V = ExpHC4V_RE (lpYYExponent->at.aplHC4V);

            // The result is Multiplier x (*1) * Exponent
            atRes.aplHC4V = MulHC4V_RE (lpYYMultiplier->at.aplHC4V, atTmp.aplHC4V);

            // Loop through all of the parts
            for (i = 0; i < iHCDimRes; i++)
                // Save in the result
                mpfr_set (&lpYYMultiplier->at.aplHC4V.parts[i], &atRes.aplHC4V.parts[i], MPFR_RNDN);

            // We no longer need this storage
            Myhc4v_clear (&atRes.aplHC4V);
            Myhc4v_clear (&atTmp.aplHC4V);

            break;

        case PN_NUMTYPE_HC8V:
            // Calculate (*1) * Exponent
            atTmp.aplHC8V = ExpHC8V_RE (lpYYExponent->at.aplHC8V);

            // The result is Multiplier x (*1) * Exponent
            atRes.aplHC8V = MulHC8V_RE (lpYYMultiplier->at.aplHC8V, atTmp.aplHC8V);

            // Loop through all of the parts
            for (i = 0; i < iHCDimRes; i++)
                // Save in the result
                mpfr_set (&lpYYMultiplier->at.aplHC8V.parts[i], &atRes.aplHC8V.parts[i], MPFR_RNDN);

            // We no longer need this storage
            Myhc8v_clear (&atRes.aplHC8V);
            Myhc8v_clear (&atTmp.aplHC8V);

            break;

        case PN_NUMTYPE_BA1F:
            // Calculate (*1) * Exponent
            atTmp.aplBA1F = ExpArb_RE (lpYYExponent->at.aplBA1F);

            // The result is Multiplier x (*1) * Exponent
            atRes.aplBA1F = MulBA1F_RE (lpYYMultiplier->at.aplBA1F, atTmp.aplBA1F);

            Assert (iHCDimRes EQ 1);

            // Save in the result
            arb_set (&lpYYMultiplier->at.aplBA1F, &atRes.aplBA1F);

            // We no longer need this storage
            arb1f_clear (&atRes.aplBA1F);
            arb1f_clear (&atTmp.aplBA1F);

            break;

        case PN_NUMTYPE_BA2F:
            // Calculate (*1) * Exponent
            atTmp.aplBA2F = ExpBA2F_RE (lpYYExponent->at.aplBA2F);

            // The result is Multiplier x (*1) * Exponent
            atRes.aplBA2F = MulBA2F_RE (lpYYMultiplier->at.aplBA2F, atTmp.aplBA2F);

            // Loop through all of the parts
            for (i = 0; i < iHCDimRes; i++)
                // Save in the result
                arb_set (&lpYYMultiplier->at.aplBA2F.parts[i], &atRes.aplBA2F.parts[i]);

            // We no longer need this storage
            arb2f_clear (&atRes.aplBA2F);
            arb2f_clear (&atTmp.aplBA2F);

            break;

        case PN_NUMTYPE_BA4F:
            // Calculate (*1) * Exponent
            atTmp.aplBA4F = ExpBA4F_RE (lpYYExponent->at.aplBA4F);

            // The result is Multiplier x (*1) * Exponent
            atRes.aplBA4F = MulBA4F_RE (lpYYMultiplier->at.aplBA4F, atTmp.aplBA4F);

            // Loop through all of the parts
            for (i = 0; i < iHCDimRes; i++)
                // Save in the result
                arb_set (&lpYYMultiplier->at.aplBA4F.parts[i], &atRes.aplBA4F.parts[i]);

            // We no longer need this storage
            arb4f_clear (&atRes.aplBA4F);
            arb4f_clear (&atTmp.aplBA4F);

            break;

        case PN_NUMTYPE_BA8F:
            // Calculate (*1) * Exponent
            atTmp.aplBA8F = ExpBA8F_RE (lpYYExponent->at.aplBA8F);

            // The result is Multiplier x (*1) * Exponent
            atRes.aplBA8F = MulBA8F_RE (lpYYMultiplier->at.aplBA8F, atTmp.aplBA8F);

            // Loop through all of the parts
            for (i = 0; i < iHCDimRes; i++)
                // Save in the result
                arb_set (&lpYYMultiplier->at.aplBA8F.parts[i], &atRes.aplBA8F.parts[i]);

            // We no longer need this storage
            arb8f_clear (&atRes.aplBA8F);
            arb8f_clear (&atTmp.aplBA8F);

            break;

        defstop
            break;
    } // End SWITCH

    // Return as the result
    return lpYYMultiplier;
} // End PN_MakeEulerPoint


//***************************************************************************
//  $PN_MakeExpPoint
//
//  Merge the Multiplier and Exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeExpPoint
    (LPPN_YYSTYPE  lpYYArg,             // Ptr to the Mantissa part
     LPPN_YYSTYPE  lpYYExponent,        // Ptr to the Exponent part
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    size_t         uNumAcc;             // Starting offset
    APLMPI         aplMpiMult = {0},
                   aplMpiExp  = {0};
#if (defined DEBUG) && (defined DEBUG_FMT)
    WCHAR          wszTemp[512];
#endif
    LPCHAR         lpszNumAccum;        // Ptr to next byte
    int            iHCDim,              // HC Dimension (1, 2, 4, 8)
                   i;                   // Loop counter
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Initialize the starting offset
    uNumAcc = lpYYArg->uNumAcc;
    lpszNumAccum = &lppnLocalVars->lpszNumAccum[0];

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // Terminate the (Exponent) argument
    PN_NumAcc (lppnLocalVars, '\0');

    // If this is not a RAT, ...
    if (lppnLocalVars->chComType NE PN_NUMTYPE_RAT)
        // Insert the exponent separator
        lppnLocalVars->lpszNumAccum[lpYYExponent->uNumAcc - 1] = 'e';

    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDim = TranslateNumTypeToHCDim (lppnLocalVars->chComType);

    // Split cases based upon the common type
    switch (lppnLocalVars->chComType)
    {
        int iExpSgn;                    // The signum of the exponent

        case PN_NUMTYPE_RAT:
        case PN_NUMTYPE_HC2R:
        case PN_NUMTYPE_HC4R:
        case PN_NUMTYPE_HC8R:
            // Convert the exponent to a rational number
            mpz_init_set_str (&aplMpiExp , &lpszNumAccum[lpYYExponent->uNumAcc], 10);

            // Check the exponent for sign
            iExpSgn = mpz_sgn (&aplMpiExp);

            // Set exponent to absolute value
            mpz_abs (&aplMpiExp, &aplMpiExp);

            // If it doesn't fit in a UINT, ...
            if (mpz_fits_uint_p (&aplMpiExp) EQ 0)
            {
                // We no longer need this storage
                Myz_clear (&aplMpiExp);

                // Set to +/- infinity
                mpq_set_inf (&lpYYArg->at.aplRat, (lpszNumAccum[uNumAcc] EQ '-') ? -1 : 1);
            } else
            {
                // Create 10 * abs (exp)
                mpz_ui_pow_ui (&aplMpiExp, 10, mpz_get_ui (&aplMpiExp));

                // If we inserted an Exponent separator, ...
                if (lppnLocalVars->lpszNumAccum[lpYYExponent->uNumAcc - 1] EQ 'e')
                    // Delete it so we can convert the Multiplier
                    lppnLocalVars->lpszNumAccum[lpYYExponent->uNumAcc - 1] = AC_EOS;

                // Convert the Multiplier to a rational integer
                mpz_init_set_str (&aplMpiMult, &lpszNumAccum[uNumAcc], 10);

                // If the exponent sign is negative, ...
                if (iExpSgn < 0)
                {
                    mpz_set_ui (mpq_numref (&lpYYArg->at.aplRat), 1);
                               *mpq_denref (&lpYYArg->at.aplRat) = aplMpiExp;
                } else
                {
                    mpz_set_ui (mpq_denref (&lpYYArg->at.aplRat), 1);
                               *mpq_numref (&lpYYArg->at.aplRat) = aplMpiExp;
                } // End IF/ELSE

#if (defined DEBUG) && (defined DEBUG_FMT)
                strcpyW (wszTemp, L"Res1: "); *FormatAplRat (&wszTemp[lstrlenW (wszTemp)], lpYYArg->at.aplRat) = WC_EOS; DbgMsgW (wszTemp);
#endif
                // Multiply the Multiplier by the 10 * exp
                mpz_mul (mpq_numref (&lpYYArg->at.aplRat),
                         mpq_numref (&lpYYArg->at.aplRat),
                        &aplMpiMult);
#if (defined DEBUG) && (defined DEBUG_FMT)
                strcpyW (wszTemp, L"Res2: "); *FormatAplRat (&wszTemp[lstrlenW (wszTemp)], lpYYArg->at.aplRat) = WC_EOS; DbgMsgW (wszTemp);
#endif
                mpq_canonicalize (&lpYYArg->at.aplRat);

                // We no longer need this storage
                Myz_clear (&aplMpiMult);

#if (defined DEBUG) && (defined DEBUG_FMT)
                strcpyW (wszTemp, L"Res3: "); *FormatAplRat (&wszTemp[lstrlenW (wszTemp)], lpYYArg->at.aplRat) = WC_EOS; DbgMsgW (wszTemp);
#endif
            } // End IF/ELSE

            // Change the type to RAT
            lpYYArg->chType = PN_NUMTYPE_RAT;

            break;

        case PN_NUMTYPE_VFP:
        case PN_NUMTYPE_HC2V:
        case PN_NUMTYPE_HC4V:
        case PN_NUMTYPE_HC8V:
            // Convert the string to a VFP number
            mpfr_set_str (&lpYYArg->at.aplVfp, &lppnLocalVars->lpszStart[lpYYArg->uNumAcc], 10, MPFR_RNDN);

            // Change the type to HCxV
            lpYYArg->chType = lppnLocalVars->chComType;

            break;

        case PN_NUMTYPE_ARB:
        case PN_NUMTYPE_BA2F:
        case PN_NUMTYPE_BA4F:
        case PN_NUMTYPE_BA8F:
            // Convert the string to a ARB number
            arb_set_str2 (&lpYYArg->at.aplArb, &lppnLocalVars->lpszStart[lpYYArg->uNumAcc], prec);

            // Change the type to BAxF
            lpYYArg->chType = lppnLocalVars->chComType;

            break;

        case PN_NUMTYPE_HC2F:
        case PN_NUMTYPE_HC4F:
        case PN_NUMTYPE_HC8F:
            // Loop through the imaginary parts
            for (i = 1; i < iHCDim; i++)
                // Initialize to 0
                lpYYArg->at.aplHC8F.parts[i] = 0.0;

            // Fall through to common code

        case PN_NUMTYPE_FLT:
        case PN_NUMTYPE_INIT:
            // Use David Gay's routines
            lpYYArg->at.aplFloat = MyStrtod (&lpszNumAccum[uNumAcc], NULL);

            // If this is the initial entry, ...
            if (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT)
                lpYYArg->chType =  PN_NUMTYPE_FLT;
            else
                // Change the type to HCxF
                lpYYArg->chType = lppnLocalVars->chComType;

            break;

        case PN_NUMTYPE_BOOL:       // Can't occur as the result of MakeExpPoint
        case PN_NUMTYPE_INT:        // ...
        case PN_NUMTYPE_HC2I:       // ...
        case PN_NUMTYPE_HC4I:       // ...
        case PN_NUMTYPE_HC8I:       // ...
        defstop
            break;
    } // End SWITCH

    // Return as the result
    return lpYYArg;
} // End PN_MakeExpPoint


//***************************************************************************
//  $PN_MakeGammaPoint
//
//  Merge the Multiplier and Exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeGammaPoint
    (LPPN_YYSTYPE  lpYYMultiplier,      // The Multiplier part
     LPPN_YYSTYPE  lpYYExponent,        // The Exponent part
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    PNNUMTYPE pnTypeRes;                // Result type
    ALLTYPES  atExp = {0},              // Exponent as ALLTYPES
              atMul = {0};              // Multiplier ...
    LPPERTABDATA lpMemPTD = GetMemPTD ();   // Ptr to PerTabData global memory handle

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // If this is the first time through, ...
    if (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT)
    {
        // Promote the Multiplier and Exponent to a common format
        pnTypeRes = aNumTypePromote[lpYYMultiplier->chType][lpYYExponent->chType];

        // If the result is BOOL, ...
        if (IsPnNumTypeBool (pnTypeRes))
            // Make it FLT
            pnTypeRes = PN_NUMTYPE_FLT;
        else
        // If the result is INT, ...
        if (IsPnNumTypeInt (pnTypeRes))
            // Make it FLT
            pnTypeRes++;
        else
        // If the result is RAT, ...
        if (IsPnNumTypeRat (pnTypeRes))
            // Make it VFP
            pnTypeRes++;
    } else
        // Use common type
        pnTypeRes = lppnLocalVars->chComType;

    // If the Multiplier must be promoted, ...
    if (pnTypeRes NE lpYYMultiplier->chType
     && aNumTypeAction[lpYYMultiplier->chType][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYMultiplier->chType][pnTypeRes]) (lpYYMultiplier, lppnLocalVars);

    // If the Exponent must be promoted, ...
    if (pnTypeRes NE lpYYExponent->chType
     && aNumTypeAction[lpYYExponent->chType  ][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYExponent->chType  ][pnTypeRes]) (lpYYExponent, lppnLocalVars);

    // Set the result type
    lpYYMultiplier->chType =
    lpYYExponent->chType   = pnTypeRes;

    // Initialize the VFP/ARB Gamma if not already done
    InitPTD_Gamma (lpMemPTD);

    // Split cases based upon the numeric type
    switch (pnTypeRes)
    {
        case PN_NUMTYPE_FLT:
            // The result is Multiplier x Gamma * Exponent
            lpYYMultiplier->at.aplFloat *= pow (FloatGamma, lpYYExponent->at.aplFloat);

            break;

        case PN_NUMTYPE_VFP:
        {
            APLVFP aplVfpTmp = {0};

            // Initialize the temp array
            mpfr_init0 (&aplVfpTmp);

            // The result is Multiplier x Gamma * Exponent
            mpfr_pow (&aplVfpTmp, &lpMemPTD->mpfrHC8V_Gamma.parts[0], &lpYYExponent->at.aplVfp, MPFR_RNDN);

            // Accumulate in the Multiplier
            mpfr_mul (&lpYYMultiplier->at.aplVfp, &lpYYMultiplier->at.aplVfp, &aplVfpTmp, MPFR_RNDN);

            // We no longer need this storage
            Myf_clear (&aplVfpTmp);
            Myf_clear (&lpYYExponent->at.aplVfp);

            break;
        } // End PN_NUMTYPE_VFP

        case PN_NUMTYPE_ARB:
        {
            APLARB aplArbTmp = {0};
            mp_limb_signed_t prec = ARB_PREC_FPC;

            // Initialize the temp array to 0/0
            Myarb_init (&aplArbTmp);

            // The result is Multiplier x Gamma * Exponent
            arb_pow (&aplArbTmp, &lpMemPTD->arbBA8F_Gamma.parts[0], &lpYYExponent->at.aplArb, prec);

            // Accumulate in the Multiplier
            arb_mul (&lpYYMultiplier->at.aplArb, &lpYYMultiplier->at.aplArb, &aplArbTmp, prec);

            // We no longer need this storage
            Myarb_clear (&aplArbTmp);
            Myarb_clear (&lpYYExponent->at.aplArb);

            break;
        } // End PN_NUMTYPE_ARB

        case PN_NUMTYPE_HC2F:
            // The result is Multiplier x Gamma * Exponent
            lpYYMultiplier->at.aplHC2F = MulHC2F_RE (lpYYMultiplier->at.aplHC2F,
                                                     PowHC2F_RE (aplHC8F_Gamma.partsLo[0].partsLo[0],
                                                                 lpYYExponent->at.aplHC2F));
            break;

        case PN_NUMTYPE_HC4F:
            // The result is Multiplier x Gamma * Exponent
            lpYYMultiplier->at.aplHC4F = MulHC4F_RE (lpYYMultiplier->at.aplHC4F,
                                                     PowHC4F_RE (aplHC8F_Gamma.partsLo[0],
                                                                 lpYYExponent->at.aplHC4F));
            break;

        case PN_NUMTYPE_HC8F:
            // The result is Multiplier x Gamma * Exponent
            lpYYMultiplier->at.aplHC8F = MulHC8F_RE (lpYYMultiplier->at.aplHC8F,
                                                     PowHC8F_RE (aplHC8F_Gamma,
                                                                 lpYYExponent->at.aplHC8F));
            break;

        case PN_NUMTYPE_HC2V:
            // Calculate the power
            atExp.aplHC2V = PowHC2V_RE (lpMemPTD->mpfrHC8V_Gamma.partsLo.partsLo, lpYYExponent->at.aplHC2V);

            // The result is Multiplier x Gamma * Exponent
            atMul.aplHC2V = MulHC2V_RE (lpYYMultiplier->at.aplHC2V, atExp.aplHC2V);

            // Save in the result
            mphc2v_set (&lpYYMultiplier->at.aplHC2V, &atMul.aplHC2V);

            // We no longer need this storage
            Myhc2v_clear (&atMul.aplHC2V);
            Myhc2v_clear (&atExp.aplHC2V);

            break;

        case PN_NUMTYPE_HC4V:
            // Calculate the power
            atExp.aplHC4V = PowHC4V_RE (lpMemPTD->mpfrHC8V_Gamma.partsLo        , lpYYExponent->at.aplHC4V);

            // The result is Multiplier x Gamma * Exponent
            atMul.aplHC4V = MulHC4V_RE (lpYYMultiplier->at.aplHC4V, atExp.aplHC4V);

            // Save in the result
            mphc4v_set (&lpYYMultiplier->at.aplHC4V, &atMul.aplHC4V);

            // We no longer need this storage
            Myhc4v_clear (&atMul.aplHC4V);
            Myhc4v_clear (&atExp.aplHC4V);

            break;

        case PN_NUMTYPE_HC8V:
            // Calculate the power
            atExp.aplHC8V = PowHC8V_RE (lpMemPTD->mpfrHC8V_Gamma                , lpYYExponent->at.aplHC8V);

            // The result is Multiplier x Gamma * Exponent
            atMul.aplHC8V = MulHC8V_RE (lpYYMultiplier->at.aplHC8V, atExp.aplHC8V);

            // Save in the result
            mphc8v_set (&lpYYMultiplier->at.aplHC8V, &atMul.aplHC8V);

            // We no longer need this storage
            Myhc8v_clear (&atMul.aplHC8V);
            Myhc8v_clear (&atExp.aplHC8V);

            break;

        case PN_NUMTYPE_BA2F:
            // Calculate the power
            atExp.aplBA2F = PowBA2F_RE (lpMemPTD->arbBA8F_Gamma.partsLo.partsLo, lpYYExponent->at.aplBA2F);

            // The result is Multiplier x Gamma * Exponent
            atMul.aplBA2F = MulBA2F_RE (lpYYMultiplier->at.aplBA2F, atExp.aplBA2F);

            // Save in the result
            arb2f_set (&lpYYMultiplier->at.aplBA2F, &atMul.aplBA2F);

            // We no longer need this storage
            arb2f_clear (&atMul.aplBA2F);
            arb2f_clear (&atExp.aplBA2F);

            break;

        case PN_NUMTYPE_BA4F:
            // Calculate the power
            atExp.aplBA4F = PowBA4F_RE (lpMemPTD->arbBA8F_Gamma.partsLo        , lpYYExponent->at.aplBA4F);

            // The result is Multiplier x Gamma * Exponent
            atMul.aplBA4F = MulBA4F_RE (lpYYMultiplier->at.aplBA4F, atExp.aplBA4F);

            // Save in the result
            arb4f_set (&lpYYMultiplier->at.aplBA4F, &atMul.aplBA4F);

            // We no longer need this storage
            arb4f_clear (&atMul.aplBA4F);
            arb4f_clear (&atExp.aplBA4F);

            break;

        case PN_NUMTYPE_BA8F:
            // Calculate the power
            atExp.aplBA8F = PowBA8F_RE (lpMemPTD->arbBA8F_Gamma, lpYYExponent->at.aplBA8F);

            // The result is Multiplier x Gamma * Exponent
            atMul.aplBA8F = MulBA8F_RE (lpYYMultiplier->at.aplBA8F, atExp.aplBA8F);

            // Save in the result
            arb8f_set (&lpYYMultiplier->at.aplBA8F, &atMul.aplBA8F);

            // We no longer need this storage
            arb8f_clear (&atMul.aplBA8F);
            arb8f_clear (&atExp.aplBA8F);

            break;

////////case PN_NUMTYPE_BOOL:               // Can't happen w/GammaPoint
////////case PN_NUMTYPE_INT:                // ...
////////case PN_NUMTYPE_RAT:                // ...
////////case PN_NUMTYPE_HC2I:               // ...
////////case PN_NUMTYPE_HC2R:               // ...
////////case PN_NUMTYPE_HC4I:               // ...
////////case PN_NUMTYPE_HC4R:               // ...
////////case PN_NUMTYPE_HC8I:               // ...
////////case PN_NUMTYPE_HC8R:               // ...
        defstop
            break;
    } // End SWITCH

    return lpYYMultiplier;
} // End PN_MakeGammaPoint


//***************************************************************************
//  $PN_MakeZetaPoint
//
//  Merge the Multiplier and Argument part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeZetaPoint
    (LPPN_YYSTYPE  lpYYMultiplier,      // The Multiplier part
     LPPN_YYSTYPE  lpYYArgument,        // The Argument part
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    PNNUMTYPE pnTypeRes;                // Result type
    ALLTYPES  atArg = {0},              // Argument as ALLTYPES
              atMul = {0};              // Multiplier ...

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // If this is the first time through, ...
    if (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT)
    {
        // Promote the Multiplier and Argument to a common format
        pnTypeRes = aNumTypePromote[lpYYMultiplier->chType][lpYYArgument->chType];

        // If the result is BOOL, ...
        if (IsPnNumTypeBool (pnTypeRes))
            // Make it FLT
            pnTypeRes = PN_NUMTYPE_FLT;
        else
        // If the result is INT, ...
        if (IsPnNumTypeInt (pnTypeRes))
            // Make it FLT
            pnTypeRes++;
        else
        // If the result is RAT, ...
        if (IsPnNumTypeRat (pnTypeRes))
            // Make it VFP
            pnTypeRes++;
    } else
        // Use common type
        pnTypeRes = lppnLocalVars->chComType;

    // If the Multiplier must be promoted, ...
    if (pnTypeRes NE lpYYMultiplier->chType
     && aNumTypeAction[lpYYMultiplier->chType][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYMultiplier->chType][pnTypeRes]) (lpYYMultiplier, lppnLocalVars);

    // If the Argument must be promoted, ...
    if (pnTypeRes NE lpYYArgument->chType
     && aNumTypeAction[lpYYArgument->chType  ][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYArgument->chType  ][pnTypeRes]) (lpYYArgument, lppnLocalVars);

    // Set the result type
    lpYYMultiplier->chType =
    lpYYArgument->chType   = pnTypeRes;

    // Split cases based upon the numeric type
    switch (pnTypeRes)
    {
        case PN_NUMTYPE_FLT:
            // If the argument is 1, ...
            if (lpYYArgument->at.aplHC1F EQ 1.0)
                // Set the Zeta (1) value to {infinity}
                lpYYMultiplier->at.aplHC1F  = fltPosInfinity;
            else
                // The result is Multiplier x Zeta (Argument)
                lpYYMultiplier->at.aplHC1F *= gsl_sf_zeta (lpYYArgument->at.aplHC1F);

            break;

        case PN_NUMTYPE_HC2F:
            // If any of the imaginary parts are non-zero, ...
            if (IzitImaginary (TranslateNumTypeToArrayType (pnTypeRes), &lpYYArgument->at.aplHC8F))
                goto NONCE_EXIT;

            // If the argument is 1, ...
            if (lpYYArgument->at.aplHC1F EQ 1.0)
                // Set the Zeta (1) value to {infinity}
                atArg.aplHC1F = fltPosInfinity;
            else
                // Calculate Zeta (Argument)
                atArg.aplHC1F = gsl_sf_zeta(lpYYArgument->at.aplHC1F);

            // The result is Multiplier x Zeta (Argument)
            lpYYMultiplier->at.aplHC2F = MulHC2F_RE (lpYYMultiplier->at.aplHC2F, atArg.aplHC2F);

            break;

        case PN_NUMTYPE_HC4F:
            // If any of the imaginary parts are non-zero, ...
            if (IzitImaginary (TranslateNumTypeToArrayType (pnTypeRes), &lpYYArgument->at.aplHC8F))
                goto NONCE_EXIT;

            // If the argument is 1, ...
            if (lpYYArgument->at.aplHC1F EQ 1.0)
                // Set the Zeta (1) value to {infinity}
                atArg.aplHC1F = fltPosInfinity;
            else
                // Calculate Zeta (Argument)
                atArg.aplHC1F = gsl_sf_zeta(lpYYArgument->at.aplHC1F);

            // The result is Multiplier x Zeta (Argument)
            lpYYMultiplier->at.aplHC4F = MulHC4F_RE (lpYYMultiplier->at.aplHC4F, atArg.aplHC4F);

            break;

        case PN_NUMTYPE_HC8F:
            // If any of the imaginary parts are non-zero, ...
            if (IzitImaginary (TranslateNumTypeToArrayType (pnTypeRes), &lpYYArgument->at.aplHC8F))
                goto NONCE_EXIT;

            // If the argument is 1, ...
            if (lpYYArgument->at.aplHC1F EQ 1.0)
                // Set the Zeta (1) value to {infinity}
                atArg.aplHC1F = fltPosInfinity;
            else
                // Calculate Zeta (Argument)
                atArg.aplHC1F = gsl_sf_zeta(lpYYArgument->at.aplHC1F);

            // The result is Multiplier x Zeta (Argument)
            lpYYMultiplier->at.aplHC8F = MulHC8F_RE (lpYYMultiplier->at.aplHC8F, atArg.aplHC8F);

            break;

        case PN_NUMTYPE_VFP:
            mphc1v_init0 (&atArg.aplHC1V);

            // If the argument is 1, ...
            if (IsMpf1 (&lpYYArgument->at.aplHC1V))
                // Set the Zeta (1) value to {infinity}
                mpfr_set_inf (&atArg.aplHC1V, 1);
            else
                // Calculate Zeta (Argument)
                mpfr_zeta (&atArg.aplHC1V, &lpYYArgument->at.aplHC1V, MPFR_RNDN);

            // Accumulate in the Multiplier
            mpfr_mul  (&lpYYMultiplier->at.aplVfp, &lpYYMultiplier->at.aplVfp, &atArg.aplHC1V, MPFR_RNDN);

            // We no longer need this storage
            Myf_clear (&atArg.aplVfp);
            Myf_clear (&lpYYArgument->at.aplVfp);

            break;

        case PN_NUMTYPE_HC2V:
            // If any of the imaginary parts are non-zero, ...
            if (IzitImaginary (TranslateNumTypeToArrayType (pnTypeRes), &lpYYArgument->at.aplHC8V))
                goto NONCE_EXIT;

            mphc2v_init0 (&atArg.aplHC2V);

            // If the argument is 1, ...
            if (IsMpf1 (&lpYYArgument->at.aplHC1V))
                // Set the Zeta (1) value to {infinity}
                mpfr_set_inf (&atArg.aplHC1V, 1);
            else
                // Calculate Zeta (Argument)
                mpfr_zeta (&atArg.aplHC1V, &lpYYArgument->at.aplHC1V, MPFR_RNDN);

            // The result is Multiplier x Zeta (Argument)
            atMul.aplHC2V = MulHC2V_RE (lpYYMultiplier->at.aplHC2V, atArg.aplHC2V);

            // Save in the result
            mphc2v_set (&lpYYMultiplier->at.aplHC2V, &atMul.aplHC2V);

            // We no longer need this storage
            Myhc2v_clear (&atMul.aplHC2V);
            Myhc2v_clear (&atArg.aplHC2V);

            break;

        case PN_NUMTYPE_HC4V:
            // If any of the imaginary parts are non-zero, ...
            if (IzitImaginary (TranslateNumTypeToArrayType (pnTypeRes), &lpYYArgument->at.aplHC8V))
                goto NONCE_EXIT;

            mphc4v_init0 (&atArg.aplHC4V);

            // If the argument is 1, ...
            if (IsMpf1 (&lpYYArgument->at.aplHC1V))
                // Set the Zeta (1) value to {infinity}
                mpfr_set_inf (&atArg.aplHC1V, 1);
            else
                // Calculate Zeta (Argument)
                mpfr_zeta (&atArg.aplHC1V, &lpYYArgument->at.aplHC1V, MPFR_RNDN);

            // The result is Multiplier x Zeta (Argument)
            atMul.aplHC4V = MulHC4V_RE (lpYYMultiplier->at.aplHC4V, atArg.aplHC4V);

            // Save in the result
            mphc4v_set (&lpYYMultiplier->at.aplHC4V, &atMul.aplHC4V);

            // We no longer need this storage
            Myhc4v_clear (&atMul.aplHC4V);
            Myhc4v_clear (&atArg.aplHC4V);

            break;

        case PN_NUMTYPE_HC8V:
            // If any of the imaginary parts are non-zero, ...
            if (IzitImaginary (TranslateNumTypeToArrayType (pnTypeRes), &lpYYArgument->at.aplHC8V))
                goto NONCE_EXIT;

            mphc8v_init0 (&atArg.aplHC8V);

            // If the argument is 1, ...
            if (IsMpf1 (&lpYYArgument->at.aplHC1V))
                // Set the Zeta (1) value to {infinity}
                mpfr_set_inf (&atArg.aplHC1V, 1);
            else
                // Calculate Zeta (Argument)
                mpfr_zeta (&atArg.aplHC1V, &lpYYArgument->at.aplHC1V, MPFR_RNDN);

            // The result is Multiplier x Zeta (Argument)
            atMul.aplHC8V = MulHC8V_RE (lpYYMultiplier->at.aplHC8V, atArg.aplHC8V);

            // Save in the result
            mphc8v_set (&lpYYMultiplier->at.aplHC8V, &atMul.aplHC8V);

            // We no longer need this storage
            Myhc8v_clear (&atMul.aplHC8V);
            Myhc8v_clear (&atArg.aplHC8V);

            break;

        case PN_NUMTYPE_ARB:
        {
            mp_limb_signed_t prec = ARB_PREC_FPC;

            arb2f_init (&atArg.aplBA2F);

            // If the argument is 1, ...
            if (IsArb1 (&lpYYArgument->at.aplBA1F))
                // Set the Zeta (1) value to {infinity}
                arb_pos_inf (&atArg.aplBA1F);
            else
                // Calculate Zeta (Argument)
                arb_zeta (&atArg.aplBA1F, &lpYYArgument->at.aplBA1F, prec);

            // The result is Multiplier x Zeta (Argument)
            atMul.aplBA2F = MulBA2F_RE (lpYYMultiplier->at.aplBA2F, atArg.aplBA2F);

            // Save in the result
            arb2f_set (&lpYYMultiplier->at.aplBA2F, &atMul.aplBA2F);

            // We no longer need this storage
            arb2f_clear (&atMul.aplBA2F);
            arb2f_clear (&atArg.aplBA2F);

            break;
        } // End PN_NUMTYPE_ARB

        case PN_NUMTYPE_BA2F:
        {
////        // If any of the imaginary parts are non-zero, ...
////        if (IzitImaginary (TranslateNumTypeToArrayType (pnTypeRes), &lpYYArgument->at.aplBA8F))
////            goto NONCE_EXIT;
////
            mp_limb_signed_t prec = ARB_PREC_FPC;

            arb2f_init (&atArg.aplBA2F);

            // If the argument is 1, ...
            if (IsArb1 (&lpYYArgument->at.aplBA2F.parts[2])
             && IsArb0 (&lpYYArgument->at.aplBA2F.parts[0]))
                // Set the Zeta (1) value to {infinity}
                arb_pos_inf (acb_realref ((acb_ptr) &atArg.aplBA2F));
            else
                // Calculate Zeta (Argument)
                acb_zeta ((acb_ptr) &atArg.aplBA2F, (acb_ptr) &lpYYArgument->at.aplBA2F, prec);

            // The result is Multiplier x Zeta (Argument)
            atMul.aplBA2F = MulBA2F_RE (lpYYMultiplier->at.aplBA2F, atArg.aplBA2F);

            // Save in the result
            arb2f_set (&lpYYMultiplier->at.aplBA2F, &atMul.aplBA2F);

            // We no longer need this storage
            arb2f_clear (&atMul.aplBA2F);
            arb2f_clear (&atArg.aplBA2F);

            break;
        } // End PN_NUMTYPE_BA2F

        case PN_NUMTYPE_BA4F:
        {
            // If any of the high-order imaginary parts are non-zero, ...
            if (IzitImaginary (ARRAY_BA2F, &lpYYArgument->at.aplBA8F.partsHi))
                goto NONCE_EXIT;

            mp_limb_signed_t prec = ARB_PREC_FPC;

            arb4f_init (&atArg.aplBA4F);

            // If the argument is 1, ...
            if (IsArb1 (&lpYYArgument->at.aplBA2F.parts[2])
             && IsArb0 (&lpYYArgument->at.aplBA2F.parts[0]))
                // Set the Zeta (1) value to {infinity}
                arb_pos_inf (&atArg.aplBA1F);
            else
                // Calculate Zeta (Argument)
                acb_zeta ((acb_ptr) &atArg.aplBA2F, (acb_ptr) &lpYYArgument->at.aplBA2F, prec);

            // The result is Multiplier x Zeta (Argument)
            atMul.aplBA4F = MulBA4F_RE (lpYYMultiplier->at.aplBA4F, atArg.aplBA4F);

            // Save in the result
            arb4f_set (&lpYYMultiplier->at.aplBA4F, &atMul.aplBA4F);

            // We no longer need this storage
            arb4f_clear (&atMul.aplBA4F);
            arb4f_clear (&atArg.aplBA4F);

            break;
        } // End PN_NUMTYPE_BA4F

        case PN_NUMTYPE_BA8F:
        {
            // If any of the high-order imaginary parts are non-zero, ...
            if (IzitImaginary (ARRAY_BA2F, &lpYYArgument->at.aplBA8F.partsLo.partsHi)
             || IzitImaginary (ARRAY_BA4F, &lpYYArgument->at.aplBA8F.partsHi))
                goto NONCE_EXIT;

            mp_limb_signed_t prec = ARB_PREC_FPC;

            arb8f_init (&atArg.aplBA8F);

            // If the argument is 1, ...
            if (IsArb1 (&lpYYArgument->at.aplBA2F.parts[2])
             && IsArb0 (&lpYYArgument->at.aplBA2F.parts[0]))
                // Set the Zeta (1) value to {infinity}
                arb_pos_inf (&atArg.aplBA1F);
            else
                // Calculate Zeta (Argument)
                acb_zeta ((acb_ptr) &atArg.aplBA2F, (acb_ptr) &lpYYArgument->at.aplBA2F, prec);

            // The result is Multiplier x Zeta (Argument)
            atMul.aplBA8F = MulBA8F_RE (lpYYMultiplier->at.aplBA8F, atArg.aplBA8F);

            // Save in the result
            arb8f_set (&lpYYMultiplier->at.aplBA8F, &atMul.aplBA8F);

            // We no longer need this storage
            arb8f_clear (&atMul.aplBA8F);
            arb8f_clear (&atArg.aplBA8F);

            break;
        } // End PN_NUMTYPE_BA8F

////////case PN_NUMTYPE_BOOL:               // Can't happen w/ZetaPoint
////////case PN_NUMTYPE_INT:                // ...
////////case PN_NUMTYPE_RAT:                // ...
////////case PN_NUMTYPE_HC2I:               // ...
////////case PN_NUMTYPE_HC2R:               // ...
////////case PN_NUMTYPE_HC4I:               // ...
////////case PN_NUMTYPE_HC4R:               // ...
////////case PN_NUMTYPE_HC8I:               // ...
////////case PN_NUMTYPE_HC8R:               // ...
        defstop
            break;
    } // End SWITCH

    return lpYYMultiplier;

NONCE_EXIT:
    // Mark as a NONCE ERROR
    lppnLocalVars->lptkLocalVars->State[0] = TKROW_NONCE;

    return NULL;        // Keep the compiler happy
} // End PN_MakeZetaPoint


//***************************************************************************
//  $PN_MakePiPoint
//
//  Merge the Multiplier and Exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakePiPoint
    (LPPN_YYSTYPE  lpYYMultiplier,      // Ptr to the Multiplier part
     LPPN_YYSTYPE  lpYYExponent,        // Ptr to the Exponent part
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    PNNUMTYPE pnTypeRes;                // Result type
    ALLTYPES  atExp = {0},              // Exponent as ALLTYPES
              atMul = {0},              // Multiplier ...
              atRes = {0};              // Result ...
    int       i = 0,                    // Loop counter
              iHCDimRes;                // HC Dimension (1, 2, 4, 8)
    LPPERTABDATA lpMemPTD = GetMemPTD ();   // Ptr to PerTabData global memory handle

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // If this is the first time through, ...
    if (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT)
    {
        // Promote the Multiplier and Exponent to a common format
        pnTypeRes = aNumTypePromote[lpYYMultiplier->chType][lpYYExponent->chType];

        // If the result is BOOL, ...
        if (IsPnNumTypeBool (pnTypeRes))
            // Make it FLT
            pnTypeRes = PN_NUMTYPE_FLT;
        else
        // If the result is INT, ...
        if (IsPnNumTypeInt (pnTypeRes))
            // Make it FLT
            pnTypeRes++;
        else
        // If the result is RAT, ...
        if (IsPnNumTypeRat (pnTypeRes))
            // Make it VFP
            pnTypeRes++;
    } else
        // Use common type
        pnTypeRes = lppnLocalVars->chComType;

    // If the Multiplier must be promoted, ...
    if (pnTypeRes NE lpYYMultiplier->chType
     && aNumTypeAction[lpYYMultiplier->chType][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYMultiplier->chType][pnTypeRes]) (lpYYMultiplier, lppnLocalVars);

    // If the Exponent must be promoted, ...
    if (pnTypeRes NE lpYYExponent->chType
     && aNumTypeAction[lpYYExponent->chType  ][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYExponent->chType  ][pnTypeRes]) (lpYYExponent, lppnLocalVars);

    // Set the result type
    lpYYMultiplier->chType =
    lpYYExponent->chType   = pnTypeRes;

    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDimRes = TranslateArrayTypeToHCDim (TranslateNumTypeToArrayType (pnTypeRes));

    // Initialize the VFP Pi if not already done
    InitPTD_Pi (lpMemPTD);

    // Split cases based upon the numeric type
    switch (pnTypeRes)
    {
////////case PN_NUMTYPE_BOOL:
////////case PN_NUMTYPE_INT:
////////    // The result is Multiplier x (o1) * Exponent
////////    lpYYMultiplier->at.aplFloat *= pow (FloatPi, (APLFLOAT) lpYYExponent->at.aplInteger);
////////
////////    break;
////////
        case PN_NUMTYPE_FLT:
            // The result is Multiplier x (o1) * Exponent
            lpYYMultiplier->at.aplFloat *= pow (FloatPi, lpYYExponent->at.aplFloat);

            break;

        case PN_NUMTYPE_VFP:
        {
            // Initialize the temp array
            mpfr_init0 (&atRes.aplVfp);

            // Get the PerTabData ptr
            lpMemPTD = GetMemPTD ();

            // The result is Multiplier x (o1) * Exponent
            mpfr_pow (&atRes.aplVfp, &lpMemPTD->mpfrHC8V_Pi.parts[0], &lpYYExponent->at.aplVfp, MPFR_RNDN);

            // Accumulate in the Multiplier
            mpfr_mul (&lpYYMultiplier->at.aplVfp, &lpYYMultiplier->at.aplVfp, &atRes.aplVfp, MPFR_RNDN);

            // We no longer need this storage
            Myf_clear (&atRes.aplVfp);
            Myf_clear (&lpYYExponent->at.aplVfp);

            break;
        } // End PN_NUMTYPE_VFP

        case PN_NUMTYPE_ARB:
            // Calculate the power
            atExp.aplBA1F = PowBA1F_RE (lpMemPTD->arbBA8F_Pi.partsLo.partsLo.partsLo, lpYYExponent->at.aplBA1F);

            // The result is Multiplier x (o1) * Exponent
            atMul.aplBA1F = MulBA1F_RE (lpYYMultiplier->at.aplBA1F, atExp.aplBA1F);

            // Save in the result
            arb1f_set (&lpYYMultiplier->at.aplBA1F, &atMul.aplBA1F);

            // We no longer need this storage
            arb1f_clear (&atMul.aplBA1F);
            arb1f_clear (&atExp.aplBA1F);

            break;

        case PN_NUMTYPE_HC2F:
            // The result is Multiplier x (o1) * Exponent
            lpYYMultiplier->at.aplHC2F = MulHC2F_RE (lpYYMultiplier->at.aplHC2F,
                                                     PowHC2F_RE (aplHC8F_Pi.partsLo[0].partsLo[0],
                                                                 lpYYExponent->at.aplHC2F));
            break;

        case PN_NUMTYPE_HC4F:
            // The result is Multiplier x (o1) * Exponent
            lpYYMultiplier->at.aplHC4F = MulHC4F_RE (lpYYMultiplier->at.aplHC4F,
                                                     PowHC4F_RE (aplHC8F_Pi.partsLo[0],
                                                                 lpYYExponent->at.aplHC4F));
            break;

        case PN_NUMTYPE_HC8F:
            // The result is Multiplier x (o1) * Exponent
            lpYYMultiplier->at.aplHC8F = MulHC8F_RE (lpYYMultiplier->at.aplHC8F,
                                                     PowHC8F_RE (aplHC8F_Pi,
                                                                 lpYYExponent->at.aplHC8F));
            break;

        case PN_NUMTYPE_HC2V:
            // Calculate the power
            atExp.aplHC2V = PowHC2V_RE (lpMemPTD->mpfrHC8V_Pi.partsLo.partsLo, lpYYExponent->at.aplHC2V);

            // The result is Multiplier x (o1) * Exponent
            atMul.aplHC2V = MulHC2V_RE (lpYYMultiplier->at.aplHC2V, atExp.aplHC2V);

            // Save in the result
            mphc2v_set (&lpYYMultiplier->at.aplHC2V, &atMul.aplHC2V);

            // We no longer need this storage
            Myhc2v_clear (&atMul.aplHC2V);
            Myhc2v_clear (&atExp.aplHC2V);

            break;

        case PN_NUMTYPE_HC4V:
            // Calculate the power
            atExp.aplHC4V = PowHC4V_RE (lpMemPTD->mpfrHC8V_Pi.partsLo        , lpYYExponent->at.aplHC4V);

            // The result is Multiplier x (o1) * Exponent
            atMul.aplHC4V = MulHC4V_RE (lpYYMultiplier->at.aplHC4V, atExp.aplHC4V);

            // Save in the result
            mphc4v_set (&lpYYMultiplier->at.aplHC4V, &atMul.aplHC4V);

            // We no longer need this storage
            Myhc4v_clear (&atMul.aplHC4V);
            Myhc4v_clear (&atExp.aplHC4V);

            break;

        case PN_NUMTYPE_HC8V:
            // Calculate the power
            atExp.aplHC8V = PowHC8V_RE (lpMemPTD->mpfrHC8V_Pi             , lpYYExponent->at.aplHC8V);

            // The result is Multiplier x (o1) * Exponent
            atMul.aplHC8V = MulHC8V_RE (lpYYMultiplier->at.aplHC8V, atExp.aplHC8V);

            // Save in the result
            mphc8v_set (&lpYYMultiplier->at.aplHC8V, &atMul.aplHC8V);

            // We no longer need this storage
            Myhc8v_clear (&atMul.aplHC8V);
            Myhc8v_clear (&atExp.aplHC8V);

            break;

        case PN_NUMTYPE_BA2F:
            // Calculate the power
            atExp.aplBA2F = PowBA2F_RE (lpMemPTD->arbBA8F_Pi.partsLo.partsLo, lpYYExponent->at.aplBA2F);

            // The result is Multiplier x (o1) * Exponent
            atMul.aplBA2F = MulBA2F_RE (lpYYMultiplier->at.aplBA2F, atExp.aplBA2F);

            // Save in the result
            arb2f_set (&lpYYMultiplier->at.aplBA2F, &atMul.aplBA2F);

            // We no longer need this storage
            arb2f_clear (&atMul.aplBA2F);
            arb2f_clear (&atExp.aplBA2F);

            break;

        case PN_NUMTYPE_BA4F:
            // Calculate the power
            atExp.aplBA4F = PowBA4F_RE (lpMemPTD->arbBA8F_Pi.partsLo        , lpYYExponent->at.aplBA4F);

            // The result is Multiplier x (o1) * Exponent
            atMul.aplBA4F = MulBA4F_RE (lpYYMultiplier->at.aplBA4F, atExp.aplBA4F);

            // Save in the result
            arb4f_set (&lpYYMultiplier->at.aplBA4F, &atMul.aplBA4F);

            // We no longer need this storage
            arb4f_clear (&atMul.aplBA4F);
            arb4f_clear (&atExp.aplBA4F);

            break;

        case PN_NUMTYPE_BA8F:
            // Calculate the power
            atExp.aplBA8F = PowBA8F_RE (lpMemPTD->arbBA8F_Pi                , lpYYExponent->at.aplBA8F);

            // The result is Multiplier x (o1) * Exponent
            atMul.aplBA8F = MulBA8F_RE (lpYYMultiplier->at.aplBA8F, atExp.aplBA8F);

            // Save in the result
            arb8f_set (&lpYYMultiplier->at.aplBA8F, &atMul.aplBA8F);

            // We no longer need this storage
            arb8f_clear (&atMul.aplBA8F);
            arb8f_clear (&atExp.aplBA8F);

            break;

        case PN_NUMTYPE_HC2I:               // Can't happen w/PiPoint
        case PN_NUMTYPE_HC2R:               // ...
        case PN_NUMTYPE_HC4I:               // ...
        case PN_NUMTYPE_HC4R:               // ...
        case PN_NUMTYPE_HC8I:               // ...
        case PN_NUMTYPE_HC8R:               // ...
        defstop
            break;
    } // End SWITCH

    // Return as the result
    return lpYYMultiplier;
} // End PN_MakePiPoint


//***************************************************************************
//  $PN_MakeVfpPoint
//
//  Merge the multiplier and exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeVfpPoint
    (LPPN_YYSTYPE  lpYYArg,             // Ptr to the mantissa part
     LPPN_YYSTYPE  lpYYExponent,        // Ptr to the exponent part (may be NULL)
     LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     LPAPLINT      lpiVfpPrec)          // Ptr to VFP precision (may be NULL)

{
    size_t    uNumAcc,                  // Starting offset
              uDig,                     // # significant digits
              uArg,                     // Loop counter
              uLen;                     // # accumulated chars
    mp_prec_t uOldPrec,                 // Old precision
              uNewPrec;                 // # significant bits
    LPCHAR    lpszNumAccum;             // Ptr to next byte

    // Initialize the starting offset
    uNumAcc = lpYYArg->uNumAcc;
    lpszNumAccum = &lppnLocalVars->lpszNumAccum[0];

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // Get # accumulated chars
    uLen = lstrlen (&lpszNumAccum[uNumAcc]);

    // Calculate the # significant digits in the number
    for (uDig = uArg = 0; uArg < uLen; uArg++)
    if (isdigit (lpszNumAccum[uNumAcc + uArg]))
        uDig++;

    // Convert the # significant digits to # significant bits
    //   via the formula 1 + floor (log2 (10^N))
    //                 = 1 + floor (N x log2 (10))
    //   where log2 (10) = (ln (10)) / (ln (2))
    //                   = M_LN10 / M_LN2
    uNewPrec = 1 + (mp_prec_t) floor (uDig * M_LN10 / M_LN2);

    // If present, ...
    if (lpYYExponent NE NULL)
        // Insert the exponent separator
        lpszNumAccum[lpYYExponent->uNumAcc - 1] = 'e';

    // Get and save the current precision
    uOldPrec = mpfr_get_default_prec ();

    if (lpiVfpPrec NE NULL && *lpiVfpPrec NE 0)
    {
        // Validate the desired precision
        if (!ValidateIntegerTest (lpiVfpPrec,           // Ptr to the integer to test
                                  DEF_MIN_QUADFPC,      // Low range value (inclusive)
                                  DEF_MAX_QUADFPC,      // High ...
                                  TRUE))                // TRUE iff we're range limiting
        {
            // Mark as invalid result
            lppnLocalVars->bYYERROR = TRUE;

            return NULL;
        } else
            uNewPrec = (UINT) *lpiVfpPrec;
    } else
        // Set the precision to the default
        uNewPrec = uOldPrec;

    // Set the default precision
    mpfr_set_default_prec (uNewPrec);

    // Use MPFR routine
    mpfr_init_set_str (&lpYYArg->at.aplVfp, &lpszNumAccum[uNumAcc], 10, MPFR_RNDN);

    // Restore the default precision
    mpfr_set_default_prec (uOldPrec);

    // Change the type to VFP
    lpYYArg->chType = PN_NUMTYPE_VFP;

    // If the result type is not INIT, ...
    if (lppnLocalVars->chComType NE PN_NUMTYPE_INIT)
        // Promote the result
        (*aNumTypeAction[lpYYArg->chType][lppnLocalVars->chComType]) (lpYYArg, lppnLocalVars);

    // Return as the result
    return lpYYArg;
} // End PN_MakeVfpPoint


//***************************************************************************
//  $PN_MakeArbPoint
//
//  Merge the Midpoint and Radius parts to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeArbPoint
    (LPPN_YYSTYPE  lpYYMidpoint,        // Ptr to the Midpoint part
     LPPN_YYSTYPE  lpYYRadius,          // Ptr to the Radius part (may be NULL)
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    PNNUMTYPE        pnTypeRes;         // Result type
    APLVFP           aplVfpMid = {0},   // Midpoint as VFP
                     aplVfpRad = {0};   // Radius   ...
    LPCHAR           lpFltStr;          // Ptr to FLT string
    int              iHCDimRes,         // HC Dimension (1, 2, 4, 8)
                     i;                 // Loop counter
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // If this is the first time through, ...
    if (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT)
    {
        // Use the type of the Midpoint
        pnTypeRes = lpYYMidpoint->chType;

        // If the result is BOOL, ...
        if (IsPnNumTypeBool (pnTypeRes))
            // Make it FLT
            pnTypeRes = PN_NUMTYPE_FLT;
        else
        // If the result is INT, ...
        if (IsPnNumTypeInt (pnTypeRes))
            // Make it FLT
            pnTypeRes++;
        else
        // If the result is RAT, ...
        if (IsPnNumTypeRat (pnTypeRes))
            // Make it VFP
            pnTypeRes++;
    } else
        // Use common type
        pnTypeRes = lppnLocalVars->chComType;

    // If the Midpoint must be promoted, ...
    if (pnTypeRes NE lpYYMidpoint->chType
     && aNumTypeAction[lpYYMidpoint->chType][pnTypeRes] NE NULL)
    {
        (*aNumTypeAction[lpYYMidpoint->chType][pnTypeRes]) (lpYYMidpoint, lppnLocalVars);

        // Set the result type
        lpYYMidpoint->chType = pnTypeRes;
    } // End IF

    // Split cases based upon the result type
    switch (pnTypeRes)
    {
        case PN_NUMTYPE_FLT:
            // Copy a ptr to the FLT string
            lpFltStr = &lppnLocalVars->lpszStart[lpYYMidpoint->uNumStart];

            // Set the midpoint
            arb_init_set_str2 (&lpYYMidpoint->at.aplBA1F, lpFltStr, prec);

            break;

        case PN_NUMTYPE_VFP:
            // Copy the Midpoint VFP as we're about to overwrite it
            aplVfpMid = lpYYMidpoint->at.aplVfp;

            // Initialize the result to 0/0
            ZeroMemory (&lpYYMidpoint->at.aplBA1F, sizeof (lpYYMidpoint->at.aplBA1F));
            Myarb_init (&lpYYMidpoint->at.aplBA1F);

            // Set the midpoint
            arb_set_mpfr (&lpYYMidpoint->at.aplBA1F, &aplVfpMid);

            // We no longer need this storage
            mpfr_clear (&aplVfpMid);

            break;

        case PN_NUMTYPE_BA2F:       // lpYYMidpoint already in Arb format
        case PN_NUMTYPE_BA4F:       // ...
        case PN_NUMTYPE_BA8F:       // ...
            break;

        defstop
            break;
    } // End SWITCH

    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDimRes = TranslateNumTypeToHCDim (pnTypeRes);

    Assert (iHCDimRes EQ 1);

    // If defined, ...
    if (lpYYRadius NE NULL)
    {
////    APLFLOATUNION aplFloatUnion;    // Temporary union
////    UBOOL         bExact;           // TRUE iff the Radius is "exact"
////     mpq_t        q;
////    fmpq_t        fq;
//      fmpr_t        fr;

        // Initialize to 0/1
////     mpq_init (q);
////    fmpq_init (fq);
//      fmpr_init (fr);
        mpfr_init0 (&aplVfpRad);

////    // Convert FLT to RAT
////    mpq_set_d (q, aplFltRad);
////
////    // Convert RAT to FMPQ
////    fmpq_set_mpq (fq, q);
////
        // In order to preserve the precision of the floating point number,
        //   we rescan it as a VFP number
        // Convert the string to a VFP number
        mpfr_set_str (&aplVfpRad, &lppnLocalVars->lpszStart[lpYYRadius->uNumStart], 10, MPFR_RNDN);

//      // Convert VFP to FMPR
//      fmpr_set_mpfr (fr, &aplVfpRad);

#define FLT_MASK        0xFFFF          // Low-order 16 bits

////    // Save the FP value in the union so we can pick it apart
////    aplFloatUnion.aplFloat = aplFltRad;
////
////    // Calculate whether or not the FLT is "exact"
////    bExact = (aplFloatUnion.aplFloatStr.uMantissa & FLT_MASK) EQ 0;

        // Loop through all the parts
        for (i = 0; i < iHCDimRes; i++)
        {
            // Set the radius
////        mag_set_d2   (arb_radref (&lpYYMidpoint->at.aplBA8F.parts[i]), aplFltRad);
            mag_set_mpfr (arb_radref (&lpYYMidpoint->at.aplBA8F.parts[i]), &aplVfpRad);
//          mag_set_fmpr (arb_radref (&lpYYMidpoint->at.aplBA8F.parts[i]), fr);
#ifdef DEBUG
            // Check for the erroneous addition of one ulp
            if (1 EQ (arb_radref (&lpYYMidpoint->at.aplBA8F.parts[i])->man &  FLT_MASK))
                DbgStop ();
#endif

////        // If the Radius is "exact", ...
////        if (bExact)
////        // Handle the erroneous addition of one ulp
////        if (1 EQ (arb_radref (&lpYYMidpoint->at.aplBA8F.parts[i])->man &  FLT_MASK))
////                  arb_radref (&lpYYMidpoint->at.aplBA8F.parts[i])->man ^= BIT0;
        } // End FOR

        // We no longer need this storage
        mpfr_clear (&aplVfpRad);
//      fmpr_clear (fr);
////    fmpq_clear (fq);
////     mpq_clear (q);
#undef  FLT_MASK
    } // End IF

    // Set the result type
    lpYYMidpoint->chType = PN_NUMTYPE_BA1F + iHCDimLog2[iHCDimRes];

    return lpYYMidpoint;
} // End PN_MakeArbPoint


//***************************************************************************
//  $PN_MakeHc1Point
//
//  Make a Real Number
//***************************************************************************

PN_YYSTYPE PN_MakeHc1Point
    (LPPNLOCALVARS lppnLocalVars)       // Ptr to PN local vars
{
    PNNUMTYPE chType;
    LPPN_YYSTYPE lpC0;                  // HC1 coefficient #0

    // Get the coefficient
    lpC0 = lppnLocalVars->lpCoeff[0];

    // Promote to the highest type
    chType = lpC0->chType;

    // Split cases based upon the PN_NUMTYPE_xxx
    switch (chType)
    {
        case PN_NUMTYPE_BOOL:
        case PN_NUMTYPE_INT:
            // Mark the result as INT
            chType = PN_NUMTYPE_INT;

////        // Save the real & imaginary parts
////        lpC0->at.aplInteger = lpC0->at.aplInteger;

            break;

        case PN_NUMTYPE_FLT:
            // Mark the result as Real FLT
            chType = PN_NUMTYPE_FLT;

            // Promote the coefficients to the common type
            (*aNumTypeAction[lpC0->chType][chType]) (lpC0, lppnLocalVars);

            // Presumably, the coefficients are now all FLT
            Assert (IsPnNumTypeFlt (lpC0->chType));

////        // Save the real & imaginary parts
////        lpC0->at.aplFloat = lpC0->at.aplFloat;

            break;

        case PN_NUMTYPE_RAT:
            // Mark the result as RAT
            chType = PN_NUMTYPE_HC4R;

            // Promote the coefficients to the common type
            (*aNumTypeAction[lpC0->chType][chType]) (lpC0, lppnLocalVars);

            // Presumably, the coefficients are now all RAT
            Assert (IsPnNumTypeRat (lpC0->chType));

////        // Save the real & imaginary parts
////        lpC0->at.aplRat = lpC0->at.aplRat;

            break;

        case PN_NUMTYPE_VFP:
            // Mark the result as VFP
            chType = PN_NUMTYPE_HC4V;

            // Promote the coefficients to the common type
            (*aNumTypeAction[lpC0->chType][chType]) (lpC0, lppnLocalVars);

            // Presumably, the coefficients are now all VFP
            Assert (IsPnNumTypeVfp (lpC0->chType));

////        // Save the real & imaginary parts
////        lpC0->at.aplVfp = lpC0->at.aplVfp;

            break;

        defstop
            break;
    } // End SWITCH

    // Mark the result as Real
    lpC0->chType = chType;

    return *lpC0;
} // End PN_MakeHc1Point


//***************************************************************************
//  $PN_MakeHc2Point
//
//  Make a Complex Number
//***************************************************************************

PN_YYSTYPE PN_MakeHc2Point
    (LPPN_YYSTYPE lpC0,             // HC2 coefficient #0
     LPPN_YYSTYPE lpC1,             // ...              1
     char          cConvType,       // Conversion Type:  'i' = none
                                    //                   'J' = none
                                    //                   'd' = degrees
                                    //                   'h' = signed unit normalized radians
                                    //                   'r' = radians
                                    //                   'u' = unit normalized radians
     LPPNLOCALVARS lppnLocalVars)   // Ptr to PN local vars

{
    PNNUMTYPE        chType,            // Type
                     chCom;             // Temp
    APLHC2V          aplHC2V;           // Temp value
    LPPERTABDATA     lpMemPTD;          // Ptr to PerTabData global memory
    APLFLOAT         aplFloat0;         // Temp var
    APLINT           aplInteger0,       // ...
                     aplInteger1;       // ...
    APLRAT           aplRat0 = {0};     // ...
    APLVFP           aplVfp0 = {0};     // ...
    APLARB           aplArb0 = {0};     // ...
    APLBA2F          aplBA2F = {0};     // ...
    UBOOL            bRet;              // TRUE iff the result is valid
    APLSTYPE         aplTypeCom;        // Common storage types
    mp_limb_signed_t prec;              // Precision

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Initialize the VFP Pi if not already done
    InitPTD_Pi (lpMemPTD);

    // Promote to a common type
    chType = aNumTypePromote[lpC0->chType][lpC1->chType];

    // Convert to array type
    aplTypeCom = TranslateNumTypeToArrayType (chType);

    // If the result is to be some form of RAT, and
    //    we're not any form of VFP, ...
    if (lppnLocalVars->bRatSep
     && !IsAnyVfp (aplTypeCom))
    {
        int         iHCDimIndex;            // HC Dimension index (0, 1, 2, 3)
        CR_RETCODES crRetCode;              // Return code

        // Get the HC Dimension Index (0, 1, 2, 3)
        iHCDimIndex = aArrayTypeToHCDimIndex[aplTypeCom];

        // Set the result type
        chType = TranslateNumTypeToHCNumType (PN_NUMTYPE_RAT, iHCDimIndex);

        // If this one is not already a RAT, ...
        if (lpC0->chType NE PN_NUMTYPE_RAT)
        {
            // Zero the previous value
            ZeroMemory (&lpC0->at, sizeof (lpC0->at));

            // If the arg is pnBool, ...
            if (lpC0->bFiller)
            {
                // Initialize to 0/1
                mpq_init (&lpC0->at.aplRat);

                // Set the return code
                crRetCode = CR_SUCCESS;
            } else
                // Convert to a RAT
                crRetCode =
                  mpq_init_set_str2 (&lpC0->at.aplRat,                              // Ptr to result
                                     &lppnLocalVars->lpszStart[lpC0->uNumStart],    // Ptr to incoming line
                                      10);                                          // Base of number system
            Assert (crRetCode EQ CR_SUCCESS
                 || crRetCode EQ CR_RESULT_NEG0);

            lpC0->chType = PN_NUMTYPE_RAT;
        } // End IF

        // If this one is not already a RAT, ...
        if (lpC1->chType NE PN_NUMTYPE_RAT)
        {
            // Zero the previous value
            ZeroMemory (&lpC1->at, sizeof (lpC1->at));

            // If the arg is pnBool, ...
            if (lpC1->bFiller)
            {
                // Initialize to 0/1
                mpq_init (&lpC1->at.aplRat);

                // Set the return code
                crRetCode = CR_SUCCESS;
            } else
                // Convert to a RAT
                crRetCode =
                  mpq_init_set_str2 (&lpC1->at.aplRat,                              // Ptr to result
                                     &lppnLocalVars->lpszStart[lpC1->uNumStart],    // Ptr to incoming line
                                      10);                                          // Base of number system
            Assert (crRetCode EQ CR_SUCCESS
                 || crRetCode EQ CR_RESULT_NEG0);

            lpC1->chType = PN_NUMTYPE_RAT;
        } // End IF
    } // End IF

    // Split cases based upon the PN_NUMTYPE_xxx
    switch (chType)
    {
        case PN_NUMTYPE_BOOL:
        case PN_NUMTYPE_INT:
            // Mark the result as Complex INT
            chType = PN_NUMTYPE_HC2I;

            // Fall through to common code

        case PN_NUMTYPE_HC2I:
            // Make a temp copy of lpC1->at.aplInteger as we overwrite it
            aplInteger1 = lpC1->at.aplInteger;

            // If we're using Signed Unit Normalized Radians [-0.5, 0.5], ...
            if (cConvType EQ 'h')
            {
                // Check for error
                if (0 NE aplInteger1)
                    goto DOMAIN_EXIT;

                // By the fact that the Signed Unit Normalized value is an integer
                //   it is a multiple of 360 degrees

                // Convert from Signed Unit Normalized Radians [-0.5, 0.5] to Signed Degrees [-180, 180]
                aplInteger1 *= 360;

                // Convert from Signed Degrees [-180, 180] to Degrees [0, 360]
                aplInteger1 += 180;

                // Change the conversion type to Degrees
                cConvType ='d';
            } else
            // If we're using Unit Normalized Radians [0, 1], ...
            if (cConvType EQ 'u')
            {
                // Check for error
                if (0 > aplInteger1
                 ||     aplInteger1 > 1)
                    goto DOMAIN_EXIT;

                // By the fact that the Unit Normalized value is an integer
                //   it is a multiple of 360 degrees

                // Convert from Unit Normalized Radians [0, 1] to Degrees [0, 360]
                aplInteger1 *= 360;

                // Change the conversion type to Degrees
                cConvType ='d';
            } else
            // If we're using Degrees [-360, 360], ...
            if (cConvType EQ 'd')
                // Check for error
                if (-360 > aplInteger1
                 ||        aplInteger1 > 360)
                    goto DOMAIN_EXIT;
            else
            // If we're using Radians [-2p1, 2p1], ...
            if (cConvType EQ 'r')
                // Check for error
                if (abs64 (aplInteger1) > 6)
                    goto DOMAIN_EXIT;

            // If we're converting from degrees or radians, ...
            if (cConvType EQ 'r'
             || cConvType EQ 'd')
            {
                // Check for error
                if (lpC0->at.aplInteger < 0)
                    goto DOMAIN_EXIT;

                // Convert lpC1 from INT to FLT
                lpC1->at.aplFloat = (APLFLOAT) aplInteger1;
            } // End IF

            // Split cases based upon the Conversion Type
            switch (cConvType)
            {
                case 'i':       // None
                case 'J':       // None
                    // Save the real & imaginary parts
                    lpC0->at.aplHC2I.parts[0] = lpC0->at.aplInteger;
                    lpC0->at.aplHC2I.parts[1] = lpC1->at.aplInteger;

                    break;

                case 'd':       // Degrees
                    // Special case multiples of 90 degrees
                    if (0 EQ (aplInteger1 % 90))
                    {
                        // Make a temp copy of lpC0->at.aplInteger as we overwrite it
                        aplInteger0 = lpC0->at.aplInteger;

                        // Split cases based upon the 360 degrees residue
                        switch (aplInteger1 % 360)
                        {
                            case 0:
                                lpC0->at.aplHC2I.parts[0] =  aplInteger0;
                                lpC0->at.aplHC2I.parts[1] =  0;

                                break;

                            case 90:
                                lpC0->at.aplHC2I.parts[0] =  0;
                                lpC0->at.aplHC2I.parts[1] =  aplInteger0;

                                break;

                            case 180:
                                lpC0->at.aplHC2I.parts[0] = -aplInteger0;
                                lpC0->at.aplHC2I.parts[1] =  0;

                                break;

                            case 270:
                                lpC0->at.aplHC2I.parts[0] =  0;
                                lpC0->at.aplHC2I.parts[1] = -aplInteger0;

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        break;
                    } // End IF

                    // Divide by Pi radians
                    lpC1->at.aplFloat /= 180.0;

                    // Convert lpC1 from degrees to radians
                    iAsmMulPiFlt (&lpC1->at.aplFloat, &lpC1->at.aplFloat);

                    // Save the new type
                    lpC1->chType = PN_NUMTYPE_FLT;

                    // Fall through to common code

                case 'r':       // Radians
                    // Make a temp copy of lpC0->at.aplInteger as a FLT as we overwrite it
                    aplFloat0 = (APLFLOAT) lpC0->at.aplInteger;

                    // Multiply lpC0->at.aplFloat by cos/sin (lpC1->at.aplFloat) to get the result
                    // ***FIXME*** -- Do we need to reduce the radians modulo 2*Pi ??
                    lpC0->at.aplHC2F.parts[0] = aplFloat0 * cos (lpC1->at.aplFloat);
                    lpC0->at.aplHC2F.parts[1] = aplFloat0 * sin (lpC1->at.aplFloat);

                    // Mark the result as Complex FLT
                    chType = PN_NUMTYPE_HC2F;

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case PN_NUMTYPE_FLT:
            // Mark the result as Complex FLT
            chType = PN_NUMTYPE_HC2F;

            // Fall through to common code

        case PN_NUMTYPE_HC2F:
////////case PN_NUMTYPE_HC4F:
////////case PN_NUMTYPE_HC8F:
            // Promote the coefficients to the common type
            (*aNumTypeAction[lpC0->chType][chType]) (lpC0, lppnLocalVars);
            (*aNumTypeAction[lpC1->chType][chType]) (lpC1, lppnLocalVars);

            // Presumably, the coefficients are now both FLT
            Assert (IsPnNumTypeFlt (lpC0->chType));
            Assert (IsPnNumTypeFlt (lpC1->chType));

            // If we're using Signed Unit Normalized Radians [-0.5, 0.5], ...
            if (cConvType EQ 'h')
            {
                // Check for error
                if (-0.5 > lpC1->at.aplFloat
                 ||        lpC1->at.aplFloat > 0.5)
                    goto DOMAIN_EXIT;

                // Convert from Signed Unit Normalized Radians [-0.5, 0.5] to Signed Degrees [-180, 180]
                lpC1->at.aplFloat *= 360;

                // Convert from Signed Degrees [-180, 180] to Degrees [0, 360]
                lpC1->at.aplFloat += 180;

                // Change the conversion type to Degrees
                cConvType ='d';
            } else
            // If we're using Unit Normalized Radians [0, 1], ...
            if (cConvType EQ 'u')
            {
                // Check for error
                if (0 > lpC1->at.aplFloat
                 ||     lpC1->at.aplFloat > 1)
                    goto DOMAIN_EXIT;

                // Convert from Unit Normalized Radians [0, 1] to Degrees [0, 360]
                lpC1->at.aplFloat *= 360;

                // Change the conversion type to Degrees
                cConvType ='d';
            } else
            // If we're using Degrees [-360, 360], ...
            if (cConvType EQ 'd')
                // Check for error
                if (-360.0 > lpC1->at.aplFloat
                 ||          lpC1->at.aplFloat > 360.0)
                    goto DOMAIN_EXIT;
            else
            // If we're using Radians [-2p1, 2p1], ...
            if (cConvType EQ 'r')
                // Check for error
                if (fabs (lpC1->at.aplFloat) > (2 * FloatPi))
                    goto DOMAIN_EXIT;

            // If we're converting from degrees or radians, ...
            if (cConvType EQ 'r'
             || cConvType EQ 'd')
            {
                // Check for error
                if (lpC0->at.aplFloat < 0)
                    goto DOMAIN_EXIT;
            } // End IF

            // Split cases based upon the Conversion Type
            switch (cConvType)
            {
                case 'i':       // None
                case 'J':       // None
                    // Save the real & imaginary parts
                    lpC0->at.aplHC2F.parts[0] = lpC0->at.aplFloat;
                    lpC0->at.aplHC2F.parts[1] = lpC1->at.aplFloat;

                    break;

                case 'd':       // Degrees
                    // Special case multiples of 90 degrees
                    if (0 EQ fmod (lpC1->at.aplFloat, 90))
                    {
                        // Make a temp copy of lpC0->at.aplFloat as we overwrite it
                        aplFloat0 = lpC0->at.aplFloat;

                        // Split cases based upon the 360 degrees residue
                        switch ((int) fmod (lpC1->at.aplFloat, 360))
                        {
                            case    0:
                                lpC0->at.aplHC2F.parts[0] =  aplFloat0;
                                lpC0->at.aplHC2F.parts[1] =  0;

                                break;

                            case   90:
                            case -270:
                                lpC0->at.aplHC2F.parts[0] =  0;
                                lpC0->at.aplHC2F.parts[1] =  aplFloat0;

                                break;

                            case  180:
                            case -180:
                                lpC0->at.aplHC2F.parts[0] = -aplFloat0;
                                lpC0->at.aplHC2F.parts[1] =  0;

                                break;

                            case 270:
                            case -90:
                                lpC0->at.aplHC2F.parts[0] =  0;
                                lpC0->at.aplHC2F.parts[1] = -aplFloat0;

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        break;
                    } // End IF

                    // Divide by Pi radians
                    lpC1->at.aplFloat /= 180.0;

                    // Convert lpC1 from degrees to radians
                    iAsmMulPiFlt (&lpC1->at.aplFloat, &lpC1->at.aplFloat);

                    // Fall through to common code

                case 'r':       // Radians
                    // Make a temp copy of lpC0->at.aplFloat as we overwrite it
                    aplFloat0 = lpC0->at.aplFloat;

                    // Save the real & imaginary parts
                    // ***FIXME*** -- Do we need to reduce the radians modulo 2*Pi ??
                    lpC0->at.aplHC2F.parts[0] = aplFloat0 * cos (lpC1->at.aplFloat);
                    lpC0->at.aplHC2F.parts[1] = aplFloat0 * sin (lpC1->at.aplFloat);

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case PN_NUMTYPE_RAT:
            // Mark the result as Complex RAT
            chType = PN_NUMTYPE_HC2R;

            // Fall through to common code

        case PN_NUMTYPE_HC2R:
////////case PN_NUMTYPE_HC4R:
////////case PN_NUMTYPE_HC8R:
            // If we're using Signed Unit Normalized Radians [-0.5, 0.5], ...
            if (cConvType EQ 'h')
            {
                // Check for error
                if (mpq_cmp_si (&lpC1->at.aplRat, -1, 2) < 0
                 || mpq_cmp_si (&lpC1->at.aplRat,  1, 2) > 0)
                    goto DOMAIN_EXIT;

                // Convert from Signed Unit Normalized Radians [-0.5, 0.5] to Signed Degrees [-180, 180]
                mpq_mul_si (&lpC1->at.aplRat, &lpC1->at.aplRat, 180, 1);

                // Convert from Signed Degrees [-180, 180] to Degrees [0, 360]
                mpq_add_si (&lpC1->at.aplRat, &lpC1->at.aplRat, 180, 1);

                // Change the conversion type to Degrees
                cConvType ='d';
            } else
            // If we're using Unit Normalized Radians [0, 1], ...
            if (cConvType EQ 'u')
            {
                // Check for error
                if (mpq_cmp_si (&lpC1->at.aplRat,  0, 1) < 0
                 || mpq_cmp_si (&lpC1->at.aplRat,  1, 1) > 0)
                    goto DOMAIN_EXIT;

                // Convert from Unit Normalized Radians [0, 1] to Degrees [0, 360]
                mpq_mul_si (&lpC1->at.aplRat, &lpC1->at.aplRat, 360, 1);

                // Change the conversion type to Degrees
                cConvType ='d';
            } else
            // If we're using Degrees [-360, 360], ...
            if (cConvType EQ 'd')
                // Check for error
                if (mpq_cmp_si (&lpC1->at.aplRat, -360, 1) < 0
                 || mpq_cmp_si (&lpC1->at.aplRat,  360, 1) > 0)
                    goto DOMAIN_EXIT;
            else
            // If we're using Radians [-2p1, 2p1], ...
            if (cConvType EQ 'r')
            {
                APLVFP apl2p1 = {0},
                       aplVfp = {0};
                UBOOL  bRet;

                // Create 2p1
                mpfr_init0    (&apl2p1);
                mpfr_set      (&apl2p1, &lpMemPTD->mpfrHC8V_Pi.parts[0], MPFR_RNDN);
                mpfr_mul_si   (&apl2p1, &apl2p1, 2, MPFR_RNDN);

                // Convert the argument to VFP
                mpfr_init_set_q (&aplVfp, &lpC1->at.aplRat, MPFR_RNDN);

                // Check for error
                bRet = mpfr_cmpabs (&aplVfp, &apl2p1) > 0;

                // We no longer need this storage
                mpfr_clear (&aplVfp);
                mpfr_clear (&apl2p1);

                if (bRet)
                    goto DOMAIN_EXIT;
            }  // End IF

            // If we're converting from degrees or radians, ...
            if (cConvType EQ 'r'
             || cConvType EQ 'd')
            {
                // Check for error
                if (mpfr_sgn (&lpC0->at.aplVfp) < 0)
                    goto DOMAIN_EXIT;
            } // End IF

            // Split cases based upon the Conversion Type
            switch (cConvType)
            {
                case 'd':
                    // Get the angle in degrees as an integer
                    aplInteger1 = ConvertToInteger_SCT (TranslateNumTypeToArrayType (lpC1->chType), &lpC1->at.aplRat, 0, &bRet);

                    // If the integer conversion is valid,
                    //   and the angle is a multiple of 90 degrees, ...
                    if (bRet
                     && 0 EQ (aplInteger1 % 90))
                        // It's a RAT
                        chCom  = PN_NUMTYPE_RAT;
                    else
                    {
                        // It's a VFP
                        chCom  = PN_NUMTYPE_VFP;
                        lppnLocalVars->bRatSep = FALSE;
                    } // End IF/ELSE

                    break;

                case 'r':
                    // It's a VFP
                    chCom  = PN_NUMTYPE_VFP;

                    break;

                case 'i':
                case 'J':
                    // It's a RAT
                    chCom  = PN_NUMTYPE_RAT;

                    break;

                defstop
                    break;
            } // End SWITCH

            // Promote the coefficients to the common type
            (*aNumTypeAction[lpC0->chType][chCom]) (lpC0, lppnLocalVars);
            (*aNumTypeAction[lpC1->chType][chCom]) (lpC1, lppnLocalVars);

            // Split cases based upon the Conversion Type
            switch (cConvType)
            {
                case 'i':       // None
                case 'J':       // None
                    // Save the real & imaginary parts
                    lpC0->at.aplHC2R.parts[0] = lpC0->at.aplRat;
                    lpC0->at.aplHC2R.parts[1] = lpC1->at.aplRat;

                    break;

                case 'd':       // Degrees
                    // If the result is a RAT (special case multiples of 90 degrees), ...
                    if (chCom EQ PN_NUMTYPE_RAT)
                    {
                        // Make a temp copy of lpC0->at.aplRat as we overwrite it
                        mpq_init_set (&aplRat0, &lpC0->at.aplRat);

                        // Split cases based upon the 360 degrees residue
                        // Note that in lpC0->at.aplHC2R, .parts[0] has been initialized
                        //   and .parts[1] has not.
                        switch (aplInteger1 % 360)
                        {
                            case 0:
                                mpq_set      (&lpC0->at.aplHC2R.parts[0], &aplRat0);
                                mpq_init     (&lpC0->at.aplHC2R.parts[1]);

                                break;

                            case 90:
                                mpq_set_ui   (&lpC0->at.aplHC2R.parts[0], 0, 1);
                                mpq_init_set (&lpC0->at.aplHC2R.parts[1], &aplRat0);

                                break;

                            case 180:
                                mpq_neg      (&aplRat0, &aplRat0);
                                mpq_set      (&lpC0->at.aplHC2R.parts[0], &aplRat0);
                                mpq_init     (&lpC0->at.aplHC2R.parts[1]);

                                break;

                            case 270:
                                mpq_neg      (&aplRat0, &aplRat0);
                                mpq_set_ui   (&lpC0->at.aplHC2R.parts[0], 0, 1);
                                mpq_init_set (&lpC0->at.aplHC2R.parts[1], &aplRat0);

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        // We no longer need this resource
                        Myq_clear (&aplRat0);

                        break;
                    } // End IF

                    // Convert lpC1 from degrees to radians
////////////////////lpC1->at.aplVfp = FloatPi * lpC1->at.aplVfp / 180;
                    mpfr_div_ui (&lpC1->at.aplVfp, &lpC1->at.aplVfp, 180, MPFR_RNDN);
                    mpfr_mul    (&lpC1->at.aplVfp, &lpC1->at.aplVfp, &lpMemPTD->mpfrHC8V_Pi.parts[0], MPFR_RNDN);

                    // Fall through to common code

                case 'r':       // Radians
                    // Calculate the sin & cos of lpC1->at.aplVfp
                    // ***FIXME*** -- Do we need to reduce the radians modulo 2*Pi ??
                    aplHC2V.parts[0] = cosVfp (lpC1->at.aplVfp);
                    aplHC2V.parts[1] = sinVfp (lpC1->at.aplVfp);

                    // Make a temp copy of lpC0->at.aplVfp as we overwrite it
                    mpfr_init_set (&aplVfp0, &lpC0->at.aplVfp, MPFR_RNDN);

                    // Initialize to 0
                    mpfr_init0 (&lpC0->at.aplHC2V.parts[1]);

                    // Multiply lpC0->at.aplVfp by cos/sin (lpC1->at.aplVfp) to get the result
                    mpfr_mul (&lpC0->at.aplHC2V.parts[0], &aplVfp0, &aplHC2V.parts[0], MPFR_RNDN);
                    mpfr_mul (&lpC0->at.aplHC2V.parts[1], &aplVfp0, &aplHC2V.parts[1], MPFR_RNDN);

                    Myhc2v_clear (&aplHC2V);
                    Myf_clear    (&aplVfp0);

                    // Mark the result as Complex VFP
                    chType = PN_NUMTYPE_HC2V;

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case PN_NUMTYPE_VFP:
            // Mark the result as Complex VFP
            chType = PN_NUMTYPE_HC2V;

            // Fall through to common code

        case PN_NUMTYPE_HC2V:
////////case PN_NUMTYPE_HC4V:
////////case PN_NUMTYPE_HC8V:
            // Set common type
            chCom  = PN_NUMTYPE_VFP;

            // Promote the coefficients to the common type
            (*aNumTypeAction[lpC0->chType][chCom]) (lpC0, lppnLocalVars);
            (*aNumTypeAction[lpC1->chType][chCom]) (lpC1, lppnLocalVars);

            // If we're using Signed Unit Normalized Radians [-0.5, 0.5], ...
            if (cConvType EQ 'h')
            {
                // Check for error
                if (mpfr_cmp_d (&lpC1->at.aplVfp, -0.5) < 0
                 || mpfr_cmp_d (&lpC1->at.aplVfp,  0.5) > 0)
                    goto DOMAIN_EXIT;

                // Convert from Signed Unit Normalized Radians [-0.5, 0.5] to Signed Degrees [-180, 180]
                mpfr_mul_si (&lpC1->at.aplVfp, &lpC1->at.aplVfp, 180, MPFR_RNDN);

                // Convert from Signed Degrees [-180, 180] to Degrees [0, 360]
                mpfr_add_si (&lpC1->at.aplVfp, &lpC1->at.aplVfp, 180, MPFR_RNDN);

                // Change the conversion type to Degrees
                cConvType ='d';
            } else
            // If we're using Unit Normalized Radians [0, 1], ...
            if (cConvType EQ 'u')
            {
                // Check for error
                if (mpfr_cmp_d (&lpC1->at.aplVfp, 0) < 0
                 || mpfr_cmp_d (&lpC1->at.aplVfp, 1) > 0)
                    goto DOMAIN_EXIT;

                // Convert from Unit Normalized Radians [0, 1] to Degrees [0, 360]
                mpfr_mul_si (&lpC1->at.aplVfp, &lpC1->at.aplVfp, 360, MPFR_RNDN);

                // Change the conversion type to Degrees
                cConvType ='d';
            } else
            // If we're using Degrees [-360, 360], ...
            if (cConvType EQ 'd')
                // Check for error
                if (mpfr_cmp_d (&lpC1->at.aplVfp, -360.0) < 0
                 || mpfr_cmp_d (&lpC1->at.aplVfp,  360.0) > 0)
                    goto DOMAIN_EXIT;
            else
            // If we're using Radians [-2p1, 2p1], ...
            if (cConvType EQ 'r')
            {
                APLVFP apl2p1 = {0};
                UBOOL  bRet;

                // Create 2p1
                mpfr_init0    (&apl2p1);
                mpfr_set      (&apl2p1, &lpMemPTD->mpfrHC8V_Pi.parts[0], MPFR_RNDN);
                mpfr_mul_si   (&apl2p1, &apl2p1, 2, MPFR_RNDN);

                // Check for error
                bRet = mpfr_cmpabs (&lpC1->at.aplVfp, &apl2p1) > 0;

                // We no longer need this storage
                mpfr_clear (&apl2p1);

                if (bRet)
                    goto DOMAIN_EXIT;
            }  // End IF

            // If we're converting from degrees or radians, ...
            if (cConvType EQ 'r'
             || cConvType EQ 'd')
            {
                // Check for error
                if (mpfr_sgn (&lpC0->at.aplVfp) < 0)
                    goto DOMAIN_EXIT;
            } // End IF

            // Split cases based upon the Conversion Type
            switch (cConvType)
            {
                case 'i':       // None
                case 'J':       // None
                    // Save the real & imaginary parts
                    lpC0->at.aplHC2V.parts[0] = lpC0->at.aplVfp;
                    lpC0->at.aplHC2V.parts[1] = lpC1->at.aplVfp;

                    break;

                case 'd':       // Degrees
                    // Get the angle in degrees as an integer
                    aplInteger1 = ConvertToInteger_SCT (ARRAY_VFP, &lpC1->at.aplVfp, 0, &bRet);

                    // Special case multiples of 90 degrees
                    if (bRet && 0 EQ (aplInteger1 % 90))
                    {
                        // Make a temp copy of lpC0->at.aplVfp as we overwrite it
                        mpfr_init_set (&aplVfp0, &lpC0->at.aplVfp, MPFR_RNDN);

                        // Split cases based upon the 360 degrees residue
                        // Note that in lpC0->at.aplHC2V, .parts[0] has been initialized
                        //   and .parts[1] has not.
                        switch (aplInteger1 % 360)
                        {
                            case 0:
                                mpfr_set      (&lpC0->at.aplHC2V.parts[0], &aplVfp0, MPFR_RNDN);
                                mpfr_init0    (&lpC0->at.aplHC2V.parts[1]);

                                break;

                            case 90:
                                mpfr_set_ui   (&lpC0->at.aplHC2V.parts[0], 0, MPFR_RNDN);
                                mpfr_init_set (&lpC0->at.aplHC2V.parts[1], &aplVfp0, MPFR_RNDN);

                                break;

                            case 180:
                                mpfr_neg      (&aplVfp0, &aplVfp0, MPFR_RNDN);
                                mpfr_set      (&lpC0->at.aplHC2V.parts[0], &aplVfp0, MPFR_RNDN);
                                mpfr_init0    (&lpC0->at.aplHC2V.parts[1]);

                                break;

                            case 270:
                                mpfr_neg      (&aplVfp0, &aplVfp0, MPFR_RNDN);
                                mpfr_init0    (&lpC0->at.aplHC2V.parts[0]);
                                mpfr_init_set (&lpC0->at.aplHC2V.parts[1], &aplVfp0, MPFR_RNDN);

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        // We no longer need this resource
                        Myf_clear (&aplVfp0);

                        break;
                    } // End IF

                    // Convert lpC1 from degrees to radians
////////////////////lpC1->at.aplVfp = FloatPi * lpC1->at.aplVfp / 180;
                    mpfr_div_ui (&lpC1->at.aplVfp, &lpC1->at.aplVfp, 180, MPFR_RNDN);
                    mpfr_mul    (&lpC1->at.aplVfp, &lpC1->at.aplVfp, &lpMemPTD->mpfrHC8V_Pi.parts[0], MPFR_RNDN);

                    // Fall through to common code

                case 'r':       // Radians
                    // Calculate the sin & cos of lpC1->at.aplVfp
                    // ***FIXME*** -- Do we need to reduce the radians modulo 2*Pi ??
                    aplHC2V.parts[0] = cosVfp (lpC1->at.aplVfp);
                    aplHC2V.parts[1] = sinVfp (lpC1->at.aplVfp);

                    // Make a temp copy of lpC0->at.aplVfp as we overwrite it
                    mpfr_init_set (&aplVfp0, &lpC0->at.aplVfp, MPFR_RNDN);

                    // Initialize to 0
                    mpfr_init0 (&lpC0->at.aplHC2V.parts[1]);

                    // Multiply lpC0->at.aplVfp by cos/sin (lpC1->at.aplVfp) to get the result
                    mpfr_mul (&lpC0->at.aplHC2V.parts[0], &aplVfp0, &aplHC2V.parts[0], MPFR_RNDN);
                    mpfr_mul (&lpC0->at.aplHC2V.parts[1], &aplVfp0, &aplHC2V.parts[1], MPFR_RNDN);

                    Myhc2v_clear (&aplHC2V);
                    Myf_clear    (&aplVfp0);

                    // Mark the result as Complex VFP
                    chType = PN_NUMTYPE_HC2V;

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case PN_NUMTYPE_ARB:
            // Mark the result as Complex Ball
            chType = PN_NUMTYPE_BA2F;

            // Fall through to common code

        case PN_NUMTYPE_BA2F:
////////case PN_NUMTYPE_BA4F:
////////case PN_NUMTYPE_BA8F:
            // Set common type
            chCom  = PN_NUMTYPE_BA1F;

            // Get the precision
            prec = ARB_PREC_FPC;

            // Promote the coefficients to the common type
            (*aNumTypeAction[lpC0->chType][chCom]) (lpC0, lppnLocalVars);
            (*aNumTypeAction[lpC1->chType][chCom]) (lpC1, lppnLocalVars);

            // If we're using Signed Unit Normalized Radians [-0.5, 0.5], ...
            if (cConvType EQ 'h')
            {
                // Check for error
                if (arb_cmp_d (&lpC1->at.aplArb, -0.5) < 0
                 || arb_cmp_d (&lpC1->at.aplArb,  0.5) > 0)
                    goto DOMAIN_EXIT;

                // Convert from Signed Unit Normalized Radians [-0.5, 0.5] to Signed Degrees [-180, 180]
                arb_mul_si (&lpC1->at.aplArb, &lpC1->at.aplArb, 180, prec);

                // Convert from Signed Degrees [-180, 180] to Degrees [0, 360]
                arb_add_si (&lpC1->at.aplArb, &lpC1->at.aplArb, 180, prec);

                // Change the conversion type to Degrees
                cConvType ='d';
            } else
            // If we're using Unit Normalized Radians [0, 1], ...
            if (cConvType EQ 'u')
            {
                // Check for error
                if (arb_cmp_d (&lpC1->at.aplArb, 0) < 0
                 || arb_cmp_d (&lpC1->at.aplArb, 1) > 0)
                    goto DOMAIN_EXIT;

                // Convert from Unit Normalized Radians to Degrees
                arb_mul_si (&lpC1->at.aplBA1F, &lpC1->at.aplBA1F, 360, prec);

                // Change the conversion type to Degrees
                cConvType ='d';
            } else
            // If we're using Degrees [-360, 360], ...
            if (cConvType EQ 'd')
                // Check for error
                if (arb_cmp_d (&lpC1->at.aplArb, -360.0) < 0
                 || arb_cmp_d (&lpC1->at.aplArb,  360.0) > 0)
                    goto DOMAIN_EXIT;
            else
            // If we're using Radians [-2p1, 2p1], ...
            if (cConvType EQ 'r')
            {
                APLARB apl2p1 = {0};
                UBOOL  bRet;

                // Create 2p1
                Myarb_init   (&apl2p1);
                arb_set      (&apl2p1, &lpMemPTD->arbBA8F_Pi.parts[0]);
                arb_mul_si   (&apl2p1, &apl2p1, 2, prec);

                // Check for error
                bRet = arf_cmpabs (arb_midref (&lpC1->at.aplArb), arb_midref (&apl2p1)) > 0;

                // We no longer need this storage
                arb_clear (&apl2p1);

                if (bRet)
                    goto DOMAIN_EXIT;
            }  // End IF

            // If we're converting from degrees or radians, ...
            if (cConvType EQ 'r'
             || cConvType EQ 'd')
            {
                // Check for error
                if (arb_sign (&lpC0->at.aplArb) < 0)
                    goto DOMAIN_EXIT;
            } // End IF

            // Split cases based upon the Conversion Type
            switch (cConvType)
            {
                case 'i':       // None
                case 'J':       // None
                    // Save the real & imaginary parts
                    lpC0->at.aplBA2F.parts[0] = lpC0->at.aplBA1F;
                    lpC0->at.aplBA2F.parts[1] = lpC1->at.aplBA1F;

                    break;

                case 'd':       // Degrees
                    // Get the angle in degrees as an integer
                    aplInteger1 = ConvertToInteger_SCT (ARRAY_BA1F, &lpC1->at.aplBA1F, 0, &bRet);

                    // Special case multiples of 90 degrees
                    if (bRet && 0 EQ (aplInteger1 % 90))
                    {
                        // Make a temp copy of lpC0->at.aplBA1F as we overwrite it
                        arb_init_set (&aplArb0, &lpC0->at.aplBA1F);

                        // Split cases based upon the 360 degrees residue
                        // Note that in lpC0->at.aplBA2F, .parts[0] has been initialized
                        //   and .parts[1] has not.
                        switch (aplInteger1 % 360)
                        {
                            case 0:
                                arb_init_set  (&lpC0->at.aplBA2F.parts[0], &aplArb0);

                                break;

                            case 90:
                                arb_set_ui    (&lpC0->at.aplBA2F.parts[0], 0);
                                arb_init_set  (&lpC0->at.aplBA2F.parts[1], &aplArb0);

                                break;

                            case 180:
                                arb_neg       (&aplArb0, &aplArb0);
                                arb_set       (&lpC0->at.aplBA2F.parts[0], &aplArb0);
                                Myarb_init    (&lpC0->at.aplBA2F.parts[1]);

                                break;

                            case 270:
                                arb_neg       (&aplArb0, &aplArb0);
                                Myarb_init    (&lpC0->at.aplBA2F.parts[0]);
                                arb_init_set  (&lpC0->at.aplBA2F.parts[1], &aplArb0);

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        // We no longer need this resource
                        Myarb_clear (&aplArb0);

                        break;
                    } // End IF

                    // Convert lpC1 from degrees to radians
////////////////////lpC1->at.aplBA1F = FloatPi * lpC1->at.aplBA1F / 180;
                    arb_div_ui (&lpC1->at.aplBA1F, &lpC1->at.aplBA1F, 180, prec);
                    arb_mul    (&lpC1->at.aplBA1F, &lpC1->at.aplBA1F, &lpMemPTD->arbBA8F_Pi.parts[0], prec);

                    // Fall through to common code

                case 'r':       // Radians
                    // Calculate the sin & cos of lpC1->at.aplBA1F
                    // ***FIXME*** -- Do we need to reduce the radians modulo 2*Pi ??
                    arb2f_init (&aplBA2F);
                    arb_cos (&aplBA2F.parts[0], &lpC1->at.aplBA1F, prec);
                    arb_sin (&aplBA2F.parts[1], &lpC1->at.aplBA1F, prec);

                    // Make a temp copy of lpC0->at.aplBA1F as we overwrite it
                    arb_init_set (&aplArb0, &lpC0->at.aplBA1F);

                    // Initialize to 0
                    Myarb_init (&lpC0->at.aplBA2F.parts[1]);

                    // Multiply lpC0->at.aplBA1F by cos/sin (lpC1->at.aplBA1F) to get the result
                    arb_mul (&lpC0->at.aplBA2F.parts[0], &aplArb0, &aplBA2F.parts[0], prec);
                    arb_mul (&lpC0->at.aplBA2F.parts[1], &aplArb0, &aplBA2F.parts[1], prec);

                    arb2f_clear (&aplBA2F);
                    Myarb_clear (&aplArb0);

                    // Mark the result as Complex Ball
                    chType = PN_NUMTYPE_BA2F;

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH

    // Mark the result as Complex
    lpC0->chType = chType;

    return *lpC0;

DOMAIN_EXIT:
    // Signal the error
    pn_yyerror (lppnLocalVars, PN_DOMAIN);

    return *lpC0;       // Arbitrary value as the caller first checks ->bYYERROR
} // End PN_MakeHc2Point


//***************************************************************************
//  $PN_MakeHc4Point
//
//  Make a Quaternion Number
//***************************************************************************

PN_YYSTYPE PN_MakeHc4Point
    (LPPNLOCALVARS lppnLocalVars)   // Ptr to PN local vars
{
    PNNUMTYPE    chType;
    LPPN_YYSTYPE lpC0,          // HC4 coefficient #0
                 lpC1,          // ...              1
                 lpC2,          // ...              2
                 lpC3;          // ...              3

    // Get the coefficients
    lpC0 = lppnLocalVars->lpCoeff[0];
    lpC1 = lppnLocalVars->lpCoeff[1];
    lpC2 = lppnLocalVars->lpCoeff[2];
    lpC3 = lppnLocalVars->lpCoeff[3];

    // Promote to the highest type
    chType = lpC0->chType;
    chType = aNumTypePromote[chType][lpC1->chType];
    chType = aNumTypePromote[chType][lpC2->chType];
    chType = aNumTypePromote[chType][lpC3->chType];

    // Split cases based upon the PN_NUMTYPE_xxx
    switch (chType)
    {
        case PN_NUMTYPE_BOOL:
        case PN_NUMTYPE_INT:
            // Mark the result as Quaternion INT
            chType = PN_NUMTYPE_HC4I;

            // Fall through to common code

        case PN_NUMTYPE_HC4I:
            // Save the real & imaginary parts
            lpC0->at.aplHC4I.parts[0] = lpC0->at.aplInteger;
            lpC0->at.aplHC4I.parts[1] = lpC1->at.aplInteger;
            lpC0->at.aplHC4I.parts[2] = lpC2->at.aplInteger;
            lpC0->at.aplHC4I.parts[3] = lpC3->at.aplInteger;

            break;

        case PN_NUMTYPE_FLT:
            // Mark the result as Quaternion FLT
            chType = PN_NUMTYPE_HC4F;

            // Fall through to common code

        case PN_NUMTYPE_HC4F:
            // Promote the coefficients to the common type
            (*aNumTypeAction[lpC0->chType][chType]) (lpC0, lppnLocalVars);
            (*aNumTypeAction[lpC1->chType][chType]) (lpC1, lppnLocalVars);
            (*aNumTypeAction[lpC2->chType][chType]) (lpC2, lppnLocalVars);
            (*aNumTypeAction[lpC3->chType][chType]) (lpC3, lppnLocalVars);

            // Presumably, the coefficients are now all FLT
            Assert (IsPnNumTypeFlt (lpC0->chType));
            Assert (IsPnNumTypeFlt (lpC1->chType));
            Assert (IsPnNumTypeFlt (lpC2->chType));
            Assert (IsPnNumTypeFlt (lpC3->chType));

            // Save the real & imaginary parts
            lpC0->at.aplHC4F.parts[0] = lpC0->at.aplFloat;
            lpC0->at.aplHC4F.parts[1] = lpC1->at.aplFloat;
            lpC0->at.aplHC4F.parts[2] = lpC2->at.aplFloat;
            lpC0->at.aplHC4F.parts[3] = lpC3->at.aplFloat;

            break;

        case PN_NUMTYPE_RAT:
            // Mark the result as Quaternion RAT
            chType = PN_NUMTYPE_HC4R;

            // Fall through to common code

        case PN_NUMTYPE_HC4R:
            // Promote the coefficients to the common type
            (*aNumTypeAction[lpC0->chType][chType]) (lpC0, lppnLocalVars);
            (*aNumTypeAction[lpC1->chType][chType]) (lpC1, lppnLocalVars);
            (*aNumTypeAction[lpC2->chType][chType]) (lpC2, lppnLocalVars);
            (*aNumTypeAction[lpC3->chType][chType]) (lpC3, lppnLocalVars);

            // Presumably, the coefficients are now all RAT
            Assert (IsPnNumTypeRat (lpC0->chType));
            Assert (IsPnNumTypeRat (lpC1->chType));
            Assert (IsPnNumTypeRat (lpC2->chType));
            Assert (IsPnNumTypeRat (lpC3->chType));

            // Save the real & imaginary parts
            lpC0->at.aplHC4R.parts[0] = lpC0->at.aplRat;
            lpC0->at.aplHC4R.parts[1] = lpC1->at.aplRat;
            lpC0->at.aplHC4R.parts[2] = lpC2->at.aplRat;
            lpC0->at.aplHC4R.parts[3] = lpC3->at.aplRat;

            break;

        case PN_NUMTYPE_VFP:
            // Mark the result as Quaternion VFP
            chType = PN_NUMTYPE_HC4V;

            // Fall through to common code

        case PN_NUMTYPE_HC4V:
            // Promote the coefficients to the common type
            (*aNumTypeAction[lpC0->chType][chType]) (lpC0, lppnLocalVars);
            (*aNumTypeAction[lpC1->chType][chType]) (lpC1, lppnLocalVars);
            (*aNumTypeAction[lpC2->chType][chType]) (lpC2, lppnLocalVars);
            (*aNumTypeAction[lpC3->chType][chType]) (lpC3, lppnLocalVars);

            // Presumably, the coefficients are now all VFP
            Assert (IsPnNumTypeVfp (lpC0->chType));
            Assert (IsPnNumTypeVfp (lpC1->chType));
            Assert (IsPnNumTypeVfp (lpC2->chType));
            Assert (IsPnNumTypeVfp (lpC3->chType));

            // Save the real & imaginary parts
            lpC0->at.aplHC4V.parts[0] = lpC0->at.aplVfp;
            lpC0->at.aplHC4V.parts[1] = lpC1->at.aplVfp;
            lpC0->at.aplHC4V.parts[2] = lpC2->at.aplVfp;
            lpC0->at.aplHC4V.parts[3] = lpC3->at.aplVfp;

            break;

        case PN_NUMTYPE_ARB:
            // Mark the result as Quaternion Ball
            chType = PN_NUMTYPE_BA4F;

            // Fall through to common code

        case PN_NUMTYPE_BA4F:
            // Promote the coefficients to the common type
            (*aNumTypeAction[lpC0->chType][chType]) (lpC0, lppnLocalVars);
            (*aNumTypeAction[lpC1->chType][chType]) (lpC1, lppnLocalVars);
            (*aNumTypeAction[lpC2->chType][chType]) (lpC2, lppnLocalVars);
            (*aNumTypeAction[lpC3->chType][chType]) (lpC3, lppnLocalVars);

            // Presumably, the coefficients are now all Balls
            Assert (IsPnNumTypeBA (lpC0->chType));
            Assert (IsPnNumTypeBA (lpC1->chType));
            Assert (IsPnNumTypeBA (lpC2->chType));
            Assert (IsPnNumTypeBA (lpC3->chType));

            // Save the real & imaginary parts
            lpC0->at.aplBA4F.parts[0] = lpC0->at.aplBA1F;
            lpC0->at.aplBA4F.parts[1] = lpC1->at.aplBA1F;
            lpC0->at.aplBA4F.parts[2] = lpC2->at.aplBA1F;
            lpC0->at.aplBA4F.parts[3] = lpC3->at.aplBA1F;

            break;

        defstop
            break;
    } // End SWITCH

    // Mark the result as Quaternion
    lpC0->chType = chType;

    return *lpC0;
} // End PN_MakeHc4Point


//***************************************************************************
//  $SaveCoeff
//
//  Copy a coefficient to global memory so as to avoid it being overwritten
//***************************************************************************

LPPN_YYSTYPE SaveCoeff
    (LPPN_YYSTYPE lpCoeff)

{
    HGLOBAL      hGlbTmp;
    LPPN_YYSTYPE lpMemTmp;

    // Allocate space for lpCoeff
    hGlbTmp = DbgGlobalAlloc (GPTR, sizeof (lpCoeff[0]));
    if (hGlbTmp NE NULL)
    {
        // Lock the memory to get a ptr to it
        lpMemTmp = MyGlobalLock000 (hGlbTmp);

        // Copy the memory to the global
        CopyMemory (lpMemTmp, lpCoeff, sizeof (lpCoeff[0]));

        // We no longer need this ptr
        MyGlobalUnlock (hGlbTmp); lpMemTmp = NULL;
    } // End IF

    return hGlbTmp;
} // End SaveCoeff


//***************************************************************************
//  $FreeCoeff
//
//  Free a coefficient if it's been allocated
//***************************************************************************

void FreeCoeff
    (LPPN_YYSTYPE *lplpCoeff)

{
    // If the coeff has been allocated, ...
    if (*lplpCoeff NE NULL)
    {
        // Free it
        MyGlobalFree (*lplpCoeff); *lplpCoeff = NULL;
    } // End IF
} // End FreeCoeff


//***************************************************************************
//  $PM_MakeHcxPoint
//
//  Make a Hypercomplex number
//***************************************************************************

PN_YYSTYPE PN_MakeHcxPoint
    (LPPN_YYSTYPE  lpC0,            // HCx real coefficient (may be NULL)
     int           iCoeff,          // Coefficient #
     LPPN_YYSTYPE  lpC1,            // HCx imaginary <iCoeff> coefficient
     char          cConvType,       // Conversion Type:  'i' = none
                                    //                   'J' = none
                                    //                   'd' = degrees
                                    //                   'h' = signed unit normalized radians
                                    //                   'r' = radians
                                    //                   'u' = unit normalized radians
     LPPNLOCALVARS lppnLocalVars)   // Ptr to local pnLocalVars

{
           int        i,                // Loop counter
                      iHCDimRes;        // HC Dimension (1, 2, 4, 8)
           size_t     uNumStart = -1;   // Starting point initialize to a large unsigned #
    static PN_YYSTYPE pnBool = {{0},                // at
                                0,                  // uNumAcc
                                FALSE,              // bSigned:1,
                                TRUE,               // bFiller:1
                                PN_NUMTYPE_BOOL,    // chType
                                0,                  // uNumStart
                                '\0',               // chCur
                               };       // Filler for empty slots
           PN_YYSTYPE pnRes = {0};      // The result

    // If the lpCoeff[0] is allocated, free it
    FreeCoeff (&lppnLocalVars->lpCoeff[0]);

    // Save the real coefficient in global memory
    //   because the arg is on the stack and will be overwritten
    if (lpC0 NE NULL)
    {
        lppnLocalVars->lpCoeff[0] = SaveCoeff (lpC0);

        // Check for errors
        if (lppnLocalVars->lpCoeff[0] EQ NULL)
            goto WSFULL_EXIT;

        // Save the smaller value
        uNumStart = min (uNumStart, lpC0->uNumStart);
    } else
        lppnLocalVars->lpCoeff[0] = NULL;

    // If the arg is non-empty, ...
    if (lpC1 NE NULL)
    {
        // If the lpCoeff[iCoeff] is allocated, free it
        FreeCoeff (&lppnLocalVars->lpCoeff[iCoeff]);

        // Save the new coefficient in global memory
        //   because the arg is on the stack and will be overwritten
        lppnLocalVars->lpCoeff[iCoeff] = SaveCoeff (lpC1);

        // Check for errors
        if (lppnLocalVars->lpCoeff[iCoeff] EQ NULL)
            goto WSFULL_EXIT;

        // Save the smaller value
        uNumStart = min (uNumStart, lpC1->uNumStart);
    } // End IF

    // Search for the highest HC Dimension filled in
    for (iHCDimRes = 8 - 1; iHCDimRes >= 1; iHCDimRes--)
    if (lppnLocalVars->lpCoeff[iHCDimRes] NE NULL)
        break;

    // Round up to a power of two
    iHCDimRes = iHCDimPof2[iHCDimRes];

    // If we're not initiating, ...
    if (lppnLocalVars->chComType NE PN_NUMTYPE_INIT)
        // Use the larger of this number and the
        //   HC Dimension of lppnLocalVars->chComType
        iHCDimRes = max (iHCDimRes,
                         TranslateNumTypeToHCDim (lppnLocalVars->chComType));

    // Looop through all of the parts
    for (i = 0; i < iHCDimRes; i++)
    // If the ptr is empty, ...
    if (lppnLocalVars->lpCoeff[i] EQ NULL)
    {
        // Fill in empty slots
        // Save the coefficient in global memory
        //   because the arg is on the stack and will be overwritten
        lppnLocalVars->lpCoeff[i] = SaveCoeff (&pnBool);

        // Check for errors
        if (lppnLocalVars->lpCoeff[i] EQ NULL)
            goto WSFULL_EXIT;

        // Save the starting value
        lppnLocalVars->lpCoeff[i]->uNumStart = uNumStart;
    } // End FOR/IF

    // Split cases based upon the highest dimension
    switch (iHCDimRes)
    {
        case 1:
            pnRes = PN_MakeHc1Point (lppnLocalVars);

            break;

        case 2:
            pnRes = PN_MakeHc2Point (lppnLocalVars->lpCoeff[0],
                                     lppnLocalVars->lpCoeff[1],
                                     cConvType,
                                     lppnLocalVars);
            // Check for error
            if (lppnLocalVars->bYYERROR)
                goto SYNTAX_EXIT;
            break;

        case 4:
            pnRes = PN_MakeHc4Point (lppnLocalVars);

            break;

        case 8:
            pnRes = PN_MakeHc8Point (lppnLocalVars);

            break;

        defstop
            goto SYNTAX_EXIT;
    } // End SWITCH

    goto NORMAL_EXIT;

WSFULL_EXIT:
    // Signal an error
    pn_yyerror (lppnLocalVars, PN_WSFULL);

    goto ERROR_EXIT;

SYNTAX_EXIT:
    // Signal an error
    pn_yyerror (lppnLocalVars, PN_SYNTAX);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // Clear all lppnLocalVars->lpCoeff

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    if (lppnLocalVars->lpCoeff[i] NE NULL)
    {
        MyGlobalFree (lppnLocalVars->lpCoeff[i]); lppnLocalVars->lpCoeff[i] = NULL;
    } // End FOR/IF

    return pnRes;
} // End PN_MakeHcxPoint


//***************************************************************************
//  $PN_MakeHcxCo
//
//  Make a Hypercomplex number coefficient
//***************************************************************************

PN_YYSTYPE PN_MakeHcxCo
    (int           iCoeff,          // Coefficient #
     LPPN_YYSTYPE  lpCn,            // HCx coefficient for <iCoeff>
     LPPNLOCALVARS lppnLocalVars)   // Ptr to local pnLocalVars

{
    PN_YYSTYPE aplRes = {0};

    // If the lpCoeff[iCoeff] is allocated, free it
    FreeCoeff (&lppnLocalVars->lpCoeff[iCoeff]);

    // Save the new coefficient in global memory
    //   because the arg is on the stack and will be overwritten
    lppnLocalVars->lpCoeff[iCoeff] = SaveCoeff (lpCn);

    // Check for errors
    if (lppnLocalVars->lpCoeff[iCoeff] EQ NULL)
    {
        // Signal an error
        pn_yyerror (lppnLocalVars, PN_WSFULL);

        return aplRes;
    } // End IF

    return *lpCn;
} // End PN_MakeHcxCo


//***************************************************************************
//  $PN_MakeHc8Point
//
//  Make an Octonion Number
//***************************************************************************

PN_YYSTYPE PN_MakeHc8Point
    (LPPNLOCALVARS lppnLocalVars)   // Ptr to PN local vars

{
    PNNUMTYPE     chType;
    LPPN_YYSTYPE  lpC0,        // HC8 coefficient #0
                  lpC1,        // ...              1
                  lpC2,        // ...              2
                  lpC3,        // ...              3
                  lpC4,        // ...              4
                  lpC5,        // ...              5
                  lpC6,        // ...              6
                  lpC7;        // ...              7

    // Get the coefficients
    lpC0 = lppnLocalVars->lpCoeff[0];
    lpC1 = lppnLocalVars->lpCoeff[1];
    lpC2 = lppnLocalVars->lpCoeff[2];
    lpC3 = lppnLocalVars->lpCoeff[3];
    lpC4 = lppnLocalVars->lpCoeff[4];
    lpC5 = lppnLocalVars->lpCoeff[5];
    lpC6 = lppnLocalVars->lpCoeff[6];
    lpC7 = lppnLocalVars->lpCoeff[7];

    // Promote to the highest type
    chType = lpC0->chType;
    chType = aNumTypePromote[chType][lpC1->chType];
    chType = aNumTypePromote[chType][lpC2->chType];
    chType = aNumTypePromote[chType][lpC3->chType];
    chType = aNumTypePromote[chType][lpC4->chType];
    chType = aNumTypePromote[chType][lpC5->chType];
    chType = aNumTypePromote[chType][lpC6->chType];
    chType = aNumTypePromote[chType][lpC7->chType];

    // Split cases based upon the PN_NUMTYPE_xxx
    switch (chType)
    {
        case PN_NUMTYPE_BOOL:
        case PN_NUMTYPE_INT:
            // Mark the result as Octonion INT
            chType = PN_NUMTYPE_HC8I;

            // Fall through to common code

        case PN_NUMTYPE_HC8I:
            // Save the real & imaginary parts
            lpC0->at.aplHC8I.parts[0] = lpC0->at.aplInteger;
            lpC0->at.aplHC8I.parts[1] = lpC1->at.aplInteger;
            lpC0->at.aplHC8I.parts[2] = lpC2->at.aplInteger;
            lpC0->at.aplHC8I.parts[3] = lpC3->at.aplInteger;
            lpC0->at.aplHC8I.parts[4] = lpC4->at.aplInteger;
            lpC0->at.aplHC8I.parts[5] = lpC5->at.aplInteger;
            lpC0->at.aplHC8I.parts[6] = lpC6->at.aplInteger;
            lpC0->at.aplHC8I.parts[7] = lpC7->at.aplInteger;

            break;

        case PN_NUMTYPE_FLT:
            // Mark the result as Octonion FLT
            chType = PN_NUMTYPE_HC8F;

            // Fall through to common code

        case PN_NUMTYPE_HC8F:
            // Promote the coefficients to the common type
            (*aNumTypeAction[lpC0->chType][chType]) (lpC0, lppnLocalVars);
            (*aNumTypeAction[lpC1->chType][chType]) (lpC1, lppnLocalVars);
            (*aNumTypeAction[lpC2->chType][chType]) (lpC2, lppnLocalVars);
            (*aNumTypeAction[lpC3->chType][chType]) (lpC3, lppnLocalVars);
            (*aNumTypeAction[lpC4->chType][chType]) (lpC4, lppnLocalVars);
            (*aNumTypeAction[lpC5->chType][chType]) (lpC5, lppnLocalVars);
            (*aNumTypeAction[lpC6->chType][chType]) (lpC6, lppnLocalVars);
            (*aNumTypeAction[lpC7->chType][chType]) (lpC7, lppnLocalVars);

            // Presumably, the coefficients are now all FLT
            Assert (IsPnNumTypeFlt (lpC0->chType));
            Assert (IsPnNumTypeFlt (lpC1->chType));
            Assert (IsPnNumTypeFlt (lpC2->chType));
            Assert (IsPnNumTypeFlt (lpC3->chType));
            Assert (IsPnNumTypeFlt (lpC4->chType));
            Assert (IsPnNumTypeFlt (lpC5->chType));
            Assert (IsPnNumTypeFlt (lpC6->chType));
            Assert (IsPnNumTypeFlt (lpC7->chType));

            // Save the real & imaginary parts
            lpC0->at.aplHC8F.parts[0] = lpC0->at.aplFloat;
            lpC0->at.aplHC8F.parts[1] = lpC1->at.aplFloat;
            lpC0->at.aplHC8F.parts[2] = lpC2->at.aplFloat;
            lpC0->at.aplHC8F.parts[3] = lpC3->at.aplFloat;
            lpC0->at.aplHC8F.parts[4] = lpC4->at.aplFloat;
            lpC0->at.aplHC8F.parts[5] = lpC5->at.aplFloat;
            lpC0->at.aplHC8F.parts[6] = lpC6->at.aplFloat;
            lpC0->at.aplHC8F.parts[7] = lpC7->at.aplFloat;

            break;

        case PN_NUMTYPE_RAT:
            // Mark the result as Octonion RAT
            chType = PN_NUMTYPE_HC8R;

            // Fall through to common code

        case PN_NUMTYPE_HC8R:
            // Promote the coefficients to the common type
            (*aNumTypeAction[lpC0->chType][chType]) (lpC0, lppnLocalVars);
            (*aNumTypeAction[lpC1->chType][chType]) (lpC1, lppnLocalVars);
            (*aNumTypeAction[lpC2->chType][chType]) (lpC2, lppnLocalVars);
            (*aNumTypeAction[lpC3->chType][chType]) (lpC3, lppnLocalVars);
            (*aNumTypeAction[lpC4->chType][chType]) (lpC4, lppnLocalVars);
            (*aNumTypeAction[lpC5->chType][chType]) (lpC5, lppnLocalVars);
            (*aNumTypeAction[lpC6->chType][chType]) (lpC6, lppnLocalVars);
            (*aNumTypeAction[lpC7->chType][chType]) (lpC7, lppnLocalVars);

            // Presumably, the coefficients are now all RAT
            Assert (IsPnNumTypeRat (lpC0->chType));
            Assert (IsPnNumTypeRat (lpC1->chType));
            Assert (IsPnNumTypeRat (lpC2->chType));
            Assert (IsPnNumTypeRat (lpC3->chType));
            Assert (IsPnNumTypeRat (lpC4->chType));
            Assert (IsPnNumTypeRat (lpC5->chType));
            Assert (IsPnNumTypeRat (lpC6->chType));
            Assert (IsPnNumTypeRat (lpC7->chType));

            // Save the real & imaginary parts
            lpC0->at.aplHC8R.parts[0] = lpC0->at.aplRat;
            lpC0->at.aplHC8R.parts[1] = lpC1->at.aplRat;
            lpC0->at.aplHC8R.parts[2] = lpC2->at.aplRat;
            lpC0->at.aplHC8R.parts[3] = lpC3->at.aplRat;
            lpC0->at.aplHC8R.parts[4] = lpC4->at.aplRat;
            lpC0->at.aplHC8R.parts[5] = lpC5->at.aplRat;
            lpC0->at.aplHC8R.parts[6] = lpC6->at.aplRat;
            lpC0->at.aplHC8R.parts[7] = lpC7->at.aplRat;

            break;

        case PN_NUMTYPE_VFP:
            // Mark the result as Octonion VFP
            chType = PN_NUMTYPE_HC8V;

            // Fall through to common code

        case PN_NUMTYPE_HC8V:
            // Promote the coefficients to the common type
            (*aNumTypeAction[lpC0->chType][chType]) (lpC0, lppnLocalVars);
            (*aNumTypeAction[lpC1->chType][chType]) (lpC1, lppnLocalVars);
            (*aNumTypeAction[lpC2->chType][chType]) (lpC2, lppnLocalVars);
            (*aNumTypeAction[lpC3->chType][chType]) (lpC3, lppnLocalVars);
            (*aNumTypeAction[lpC4->chType][chType]) (lpC4, lppnLocalVars);
            (*aNumTypeAction[lpC5->chType][chType]) (lpC5, lppnLocalVars);
            (*aNumTypeAction[lpC6->chType][chType]) (lpC6, lppnLocalVars);
            (*aNumTypeAction[lpC7->chType][chType]) (lpC7, lppnLocalVars);

            // Presumably, the coefficients are now all VFP
            Assert (IsPnNumTypeVfp (lpC0->chType));
            Assert (IsPnNumTypeVfp (lpC1->chType));
            Assert (IsPnNumTypeVfp (lpC2->chType));
            Assert (IsPnNumTypeVfp (lpC3->chType));
            Assert (IsPnNumTypeVfp (lpC4->chType));
            Assert (IsPnNumTypeVfp (lpC5->chType));
            Assert (IsPnNumTypeVfp (lpC6->chType));
            Assert (IsPnNumTypeVfp (lpC7->chType));

            // Save the real & imaginary parts
            lpC0->at.aplHC8V.parts[0] = lpC0->at.aplVfp;
            lpC0->at.aplHC8V.parts[1] = lpC1->at.aplVfp;
            lpC0->at.aplHC8V.parts[2] = lpC2->at.aplVfp;
            lpC0->at.aplHC8V.parts[3] = lpC3->at.aplVfp;
            lpC0->at.aplHC8V.parts[4] = lpC4->at.aplVfp;
            lpC0->at.aplHC8V.parts[5] = lpC5->at.aplVfp;
            lpC0->at.aplHC8V.parts[6] = lpC6->at.aplVfp;
            lpC0->at.aplHC8V.parts[7] = lpC7->at.aplVfp;

            break;

        case PN_NUMTYPE_ARB:
            // Mark the result as Octonion Ball
            chType = PN_NUMTYPE_BA8F;

            // Fall through to common code

        case PN_NUMTYPE_BA8F:
            // Promote the coefficients to the common type
            (*aNumTypeAction[lpC0->chType][chType]) (lpC0, lppnLocalVars);
            (*aNumTypeAction[lpC1->chType][chType]) (lpC1, lppnLocalVars);
            (*aNumTypeAction[lpC2->chType][chType]) (lpC2, lppnLocalVars);
            (*aNumTypeAction[lpC3->chType][chType]) (lpC3, lppnLocalVars);
            (*aNumTypeAction[lpC4->chType][chType]) (lpC4, lppnLocalVars);
            (*aNumTypeAction[lpC5->chType][chType]) (lpC5, lppnLocalVars);
            (*aNumTypeAction[lpC6->chType][chType]) (lpC6, lppnLocalVars);
            (*aNumTypeAction[lpC7->chType][chType]) (lpC7, lppnLocalVars);

            // Presumably, the coefficients are now all Balls
            Assert (IsPnNumTypeBA (lpC0->chType));
            Assert (IsPnNumTypeBA (lpC1->chType));
            Assert (IsPnNumTypeBA (lpC2->chType));
            Assert (IsPnNumTypeBA (lpC3->chType));
            Assert (IsPnNumTypeBA (lpC4->chType));
            Assert (IsPnNumTypeBA (lpC5->chType));
            Assert (IsPnNumTypeBA (lpC6->chType));
            Assert (IsPnNumTypeBA (lpC7->chType));

            // Save the real & imaginary parts
            lpC0->at.aplBA8F.parts[0] = lpC0->at.aplBA1F;
            lpC0->at.aplBA8F.parts[1] = lpC1->at.aplBA1F;
            lpC0->at.aplBA8F.parts[2] = lpC2->at.aplBA1F;
            lpC0->at.aplBA8F.parts[3] = lpC3->at.aplBA1F;
            lpC0->at.aplBA8F.parts[4] = lpC4->at.aplBA1F;
            lpC0->at.aplBA8F.parts[5] = lpC5->at.aplBA1F;
            lpC0->at.aplBA8F.parts[6] = lpC6->at.aplBA1F;
            lpC0->at.aplBA8F.parts[7] = lpC7->at.aplBA1F;

            break;

        defstop
            break;
    } // End SWITCH

    // Mark the result as Octonion
    lpC0->chType = chType;

    return *lpC0;
} // End PN_MakeHc8Point


//***************************************************************************
//  $PN_VectorAcc
//
//  Accumulate the current value into the vector
//***************************************************************************

UBOOL PN_VectorAcc
    (LPPN_YYSTYPE  lpYYArg,             // Number to accumulate
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    LPPN_VECTOR lppnVector;
    LPCHAR      lpStart;
    size_t      uNumLen;
    UBOOL       bFltN0,                 // TRUE iff the arg is FLT and -0
                bVfpN0,                 // ...                 VFP ...
                bNeg0;                  // ...      bFltN0 || bVfpN0

    // Accumulate the number

    // Turn off the BasePoint Notation flag
    lppnLocalVars->bBasePoint = FALSE;

    // If the global handle has not been allocated as yet, ...
    if (lppnLocalVars->hGlbVector EQ NULL)
    {
        lppnLocalVars->hGlbVector =
          DbgGlobalAlloc (GHND, PNVECTOR_INIT * sizeof (PN_VECTOR));
        if (lppnLocalVars->hGlbVector EQ NULL)
            goto WSFULL_EXIT;
        lppnLocalVars->uGlbVectorMaxLen = PNVECTOR_INIT;
        lppnLocalVars->uGlbVectorCurLen = 0;
    } // End IF

    // Check to see if we need to reallocate
    if (lppnLocalVars->uGlbVectorCurLen EQ lppnLocalVars->uGlbVectorMaxLen)
    {
        UINT uMaxLen;

        // Calculate the new maximum length
        uMaxLen = lppnLocalVars->uGlbVectorMaxLen + PNVECTOR_INCR;

        // Attempt to reallocate the storage
        //   moving the old data to the new location, and
        //   freeing the old global memory
        lppnLocalVars->hGlbVector =
          MyGlobalReAlloc (lppnLocalVars->hGlbVector,
                           uMaxLen * sizeof (PN_VECTOR),
                           GMEM_MOVEABLE);
        // If that failed, ...
        if (lppnLocalVars->hGlbVector EQ NULL)
        {
            HGLOBAL hGlbVector;
            LPVOID  lpMemVectorOld,
                    lpMemVectorNew;

            // Allocate a new global memory object
            hGlbVector =
               DbgGlobalAlloc (GHND,
                              uMaxLen * sizeof (PN_VECTOR));
            if (hGlbVector EQ NULL)    // ***FIXME*** do we need to free the hGlbVector storage???
                goto WSFULL_EXIT;
            // Lock the two global memory areas and copy old to new
            lpMemVectorOld = MyGlobalLock    (lppnLocalVars->hGlbVector);
            lpMemVectorNew = MyGlobalLock000 (               hGlbVector);
            CopyMemory (lpMemVectorNew,
                        lpMemVectorOld,
                        lppnLocalVars->uGlbVectorMaxLen * sizeof (PN_VECTOR));
            // We no longer need these ptrs
            MyGlobalUnlock (lppnLocalVars->hGlbVector); lpMemVectorNew = NULL;
            MyGlobalUnlock (               hGlbVector); lpMemVectorOld = NULL;

            // Free the old storage
            DbgGlobalFree (lppnLocalVars->hGlbVector); lppnLocalVars->hGlbVector = NULL;

            // Copy to save area
            lppnLocalVars->hGlbVector = hGlbVector; hGlbVector = NULL;
        } // End IF

        // Save as new maximum length
        lppnLocalVars->uGlbVectorMaxLen = uMaxLen;
    } // End IF

    // Lock the memory to get a ptr to it
    lppnVector = MyGlobalLock (lppnLocalVars->hGlbVector);

    lppnVector[lppnLocalVars->uGlbVectorCurLen].at       = lpYYArg->at;
    ZeroMemory (&lppnLocalVars->at, sizeof (lppnLocalVars->at));

    lppnVector[lppnLocalVars->uGlbVectorCurLen].chType   = lpYYArg->chType;
    lppnLocalVars->chType = PN_NUMTYPE_INIT;

    lpStart = &lppnLocalVars->lpszStart[lpYYArg->uNumStart];
    uNumLen =  lppnLocalVars->uNumCur - lpYYArg->uNumStart;     // ***CHECKME***

    // Delete trailing blanks
    // While the last char is white, ...
    while (uNumLen && IsWhite (lpStart[uNumLen - 1]))
        // Skip to the previous char
        uNumLen--;

    // Delete leading blanks
    // While the first char is white, ...
    while (uNumLen && IsWhite (lpStart[0]))
    {
        // Skip over it
        lpStart++; uNumLen--;
    } // End WHILE

    lppnVector[lppnLocalVars->uGlbVectorCurLen].lpStart = lpStart;
    lppnVector[lppnLocalVars->uGlbVectorCurLen].uNumLen = uNumLen;

    bFltN0 = (IsPnNumTypeFlt (lppnVector[lppnLocalVars->uGlbVectorCurLen].chType)           // Is -0.0
           && IsFltN0        (lppnVector[lppnLocalVars->uGlbVectorCurLen].at.aplFloat));    // ...
    bVfpN0 = (IsPnNumTypeVfp (lppnVector[lppnLocalVars->uGlbVectorCurLen].chType)           // Is -0.0v
           && IsVfpN0        (lppnVector[lppnLocalVars->uGlbVectorCurLen].at.aplVfp  ));    // ...
    bNeg0  = bFltN0 || bVfpN0;

    // Izit expressible as a rational?
    lppnVector[lppnLocalVars->uGlbVectorCurLen].bRatExp =
        lppnLocalVars->bRatSep
     && !(gAllowNeg0 ? bNeg0
                     : FALSE
         );
    // Set the new initial point
    lppnLocalVars->uNumIni += uNumLen;

    // Count in another item
    lppnLocalVars->uGlbVectorCurLen++;

    // We no longer need this resource
    MyGlobalUnlock (lppnLocalVars->hGlbVector); lppnVector = NULL;

    // Mark as successful
    return TRUE;

WSFULL_EXIT:
    // Save the error message
    ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);

    return FALSE;
} // End PN_VectorAcc


//***************************************************************************
//  $PN_VectorRes
//
//  Create the scalar or vector result
//***************************************************************************

UBOOL PN_VectorRes
    (LPPNLOCALVARS lppnLocalVars)           // Ptr to local pnLocalVars

{
    UINT              uCnt,                 // Loop counter
                      uRes;                 // ...
    UBOOL             bRet = FALSE;         // TRUE iff the result is valid
    LPPN_VECTOR       lppnVector;           // Ptr to accumulated vector
    APLSTYPE          aplTypeRes;           // Result storage type
    APLNELM           aplNELMRes;           // ...    NELM
    APLRANK           aplRankRes;           // ...    rank
    APLUINT           ByteRes;              // # bytes in the string vector
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPVOID            lpMemRes;             // Ptr to result global memory
    PNLOCALVARS       pnLocalVars = {0};    // PN Local vars
    PNNUMTYPE         chComType;            // Common numeric type (see PNNUMTYPE)
    PN_YYSTYPE        pnType = {0};         // PN_YYSTYPE stack element
    UBOOL             bPass1;               // TRUE iff pass 1
    int               i,                    // Loop counter
                      iHCDimRes;            // HC Dimension (1, 2, 4, 8)
    WCHAR             wchType;              // Temp WCHAR

    // Lock the memory to get a ptr to it
    lppnVector = MyGlobalLock (lppnLocalVars->hGlbVector);

    // Get the # items
    aplNELMRes = lppnLocalVars->uGlbVectorCurLen;

    Assert (aplNELMRes NE 0);

    // Mark as pass 1 or not
    bPass1 = (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT);

    // If this is the first pass, ...
    if (bPass1)
    {
        // Initialize the starting datatype
        chComType                =
        lppnLocalVars->chComType = PN_NUMTYPE_INIT;

        // Scan all items to determine a common datatype
        for (uCnt = 0; uCnt < aplNELMRes; uCnt++)
        {
            UBOOL bFltN0 = (IsPnNumTypeFlt (lppnVector[uCnt].chType)        // Is -0.0
                         && IsFltN0        (lppnVector[uCnt].at.aplFloat)), // ...
                  bVfpN0 = (IsPnNumTypeVfp (lppnVector[uCnt].chType)        // Is -0.0v
                         && IsVfpN0        (lppnVector[uCnt].at.aplVfp  )), // ...
                  bNeg0  = bFltN0 || bVfpN0,
                  bFlt   =  IsPnNumTypeFlt (lppnVector[uCnt].chType)        // Is FLT
                         || IsPnNumTypeVfp (lppnVector[uCnt].chType);       // Is VFP

            // If there's a Rat but no Vfp separator, and
            //    the item is not -0, ...
            if (lppnLocalVars->bRatSep
             && !(gAllowNeg0 ? bNeg0
                             : FALSE
                 )
                )
                // ***ASSUME***:  chType - 1 is INT- or RAT-like
                lppnLocalVars->chComType = aNumTypePromote[lppnLocalVars->chComType][lppnVector[uCnt].chType - bFlt];
            else
                lppnLocalVars->chComType = aNumTypePromote[lppnLocalVars->chComType][lppnVector[uCnt].chType       ];
        } // End FOR

        // Translate the datatype to an array storage type
        aplTypeRes = TranslateNumTypeToArrayType (lppnLocalVars->chComType);

        // Calculate the HC Dimension (1, 2, 4, 8)
        iHCDimRes = TranslateArrayTypeToHCDim (aplTypeRes);

        // Calculate the result rank
        aplRankRes = aplNELMRes > 1;        // 0 if singleton, 1 if not

        // Calculate space needed for the numeric scalar/vector
        ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Allocate global memory for the array
        lppnLocalVars->hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (lppnLocalVars->hGlbRes EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lppnLocalVars->lpMemHdrRes = MyGlobalLock000 (lppnLocalVars->hGlbRes);

#define lpHeader        lppnLocalVars->lpMemHdrRes
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = aplTypeRes;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
#ifdef DEBUG
        lpHeader->bMFOvar    = lppnLocalVars->lptkLocalVars->bMFO;
#endif
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMRes;
        lpHeader->Rank       = aplRankRes;
#undef  lpHeader

        // If it's not a scalar, ...
        if (!IsScalar (aplRankRes))
            // Save the dimension
            *VarArrayBaseToDim (lppnLocalVars->lpMemHdrRes) = aplNELMRes;

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lppnLocalVars->lpMemHdrRes);

        // Save common values
        pnLocalVars.lpszAlphaInt  = lppnLocalVars->lpszAlphaInt;
        pnLocalVars.lpszNumAccum  = lppnLocalVars->lpszNumAccum;
        pnLocalVars.uCharIndex    = lppnLocalVars->uCharIndex;
        pnLocalVars.lptkLocalVars = lppnLocalVars->lptkLocalVars;
        pnLocalVars.bRatSep       = lppnLocalVars->bRatSep;

        // Calculate the coefficient type (IFRV)
        wchType = PN_CoeffType (lppnLocalVars->chComType);

        // Convert all items to a common datatype (lppnLocalVars->chComType)
        for (uCnt = uRes = 0; uCnt < aplNELMRes; uCnt++)
        {
            // We're trying to avioid anomalies such as having (say)
            // 1:   1111111111111111111111111111111111111 1r2
            //   return a VFP because the string of 1s is converted to FLT
            //   and FLT RAT -> VFP where we would prefer RAT RAT -> RAT
            // 2:   1111111111111111111111111111111111111 0i1
            //   return a HC2F because the string of 1s is converted to FLT
            //   and FLT HC2I -> HC2F where we would prefer RAT HC2I -> HC2R.
            // 3:   0.1 5v
            //   convert 0.1 as FLT and then to VFP
            //   instead of convert 0.1 to VFP directly and not introduce spurious trailing digits.

            // If the item needs promoting, and
            //   the result is not type 'I' or 'F', ...
            if (wchType NE PN_CoeffType (lppnVector[uCnt].chType)
             && wchType NE L'I'
             && wchType NE L'F')
            {
                UBOOL bRet;                 // TRUE iff result is valid
                char  cZap;                 // Temporary char

                // Save the starting point and length of the character stream
                pnLocalVars.lpszStart    = lppnVector[uCnt].lpStart;
                pnLocalVars.uNumLen      = lppnVector[uCnt].uNumLen;
                pnLocalVars.uNumIni      = 0;
                pnLocalVars.chComType    = lppnLocalVars->chComType;

                // Ensure properly terminated
                cZap = pnLocalVars.lpszStart[pnLocalVars.uNumLen];
                       pnLocalVars.lpszStart[pnLocalVars.uNumLen] = AC_EOS;

                // Call the parser to convert the PN to a number
                bRet = ParsePointNotation (&pnLocalVars);

                // Restore zapped char
                pnLocalVars.lpszStart[pnLocalVars.uNumLen] = cZap;

                if (bRet)
                {
                    LPPN_VECTOR lpMemVector;            // Ptr to hGlbVector global memory

                    // Lock the memory to get a ptr to it
                    lpMemVector = MyGlobalLock (pnLocalVars.hGlbVector);

                    // Copy the new value to temp storage
                    pnType.at = lpMemVector->at;

                    // Promote to the common type and free old storage
                    (*aNumTypeAction[lpMemVector->chType][lppnLocalVars->chComType]) (&pnType, lppnLocalVars);

                    // We no longer need this ptr
                    MyGlobalUnlock (pnLocalVars.hGlbVector); lpMemVector = NULL;

                    // We no longer need this storage
                    DbgGlobalFree (pnLocalVars.hGlbVector); pnLocalVars.hGlbVector = NULL;
                } else
                    goto ERROR_EXIT;
            } else
            {
                // Copy the new value to temp storage
                pnType.at = lppnVector[uCnt].at;

                // Promote to the common type and free old storage
                (*aNumTypeAction[lppnVector[uCnt].chType][lppnLocalVars->chComType]) (&pnType, lppnLocalVars);
            } // End IF/ELSE

            // Split cases based upon the common type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:
                    // Save the promoted type in the result
                    *((LPAPLBOOL ) lpMemRes)  |= (pnType.at.aplInteger) << uRes;

                    // Check for end-of-byte
                    if (++uRes EQ NBIB)
                    {
                        uRes = 0;                   // Start over
                        ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                    } // End IF

                    break;

                case ARRAY_INT:
                case ARRAY_HC2I:
                case ARRAY_HC4I:
                case ARRAY_HC8I:
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRes; i++)
                        // Save the promoted type in the result
                        *((LPAPLINT  ) lpMemRes)++ = pnType.at.aplHC8I.parts[i];

                    break;

                case ARRAY_FLOAT:
                case ARRAY_HC2F:
                case ARRAY_HC4F:
                case ARRAY_HC8F:
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRes; i++)
                        // Save the promoted type in the result
                        *((LPAPLFLOAT) lpMemRes)++ = pnType.at.aplHC8F.parts[i];

                    break;

                case ARRAY_RAT:
                case ARRAY_HC2R:
                case ARRAY_HC4R:
                case ARRAY_HC8R:
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRes; i++)
                        // Save the promoted type in the result
                        *((LPAPLRAT  ) lpMemRes)++ = pnType.at.aplHC8R.parts[i];

                    break;

                case ARRAY_VFP:
                case ARRAY_HC2V:
                case ARRAY_HC4V:
                case ARRAY_HC8V:
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRes; i++)
                        // Save the promoted type in the result
                        *((LPAPLVFP  ) lpMemRes)++ = pnType.at.aplHC8V.parts[i];

                    break;

                case ARRAY_ARB:
                case ARRAY_BA2F:
                case ARRAY_BA4F:
                case ARRAY_BA8F:
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRes; i++)
                        // Save the promoted type in the result
                        *((LPAPLBA1F ) lpMemRes)++ = pnType.at.aplBA8F.parts[i];

                    break;

                case ARRAY_CHAR:            // Can't happen as this code is for numbers only
                case ARRAY_HETERO:          // ...
                case ARRAY_NESTED:          // ...
                defstop
                    break;
            } // End SWITCH
        } // End FOR
    } else
    {
        Assert (IsSingleton (aplNELMRes));

        // Save the global memory handle
        lppnLocalVars->hGlbRes = lppnLocalVars->hGlbVector;
    } // End IF/ELSE

    // Copy the last result back to <lppnLocalVars->at> in case it's a scalar
    //   and we need its value for :CONSTANT on )LOAD or )COPY
    lppnLocalVars->at = pnType.at;

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    // Save the error message
    ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (lppnLocalVars->hGlbRes NE NULL && lppnLocalVars->lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lppnLocalVars->hGlbRes); lppnLocalVars->lpMemHdrRes = NULL;
    } // End IF

    if (lppnVector NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lppnLocalVars->hGlbVector); lppnVector = NULL;
    } // End IF

    if (bPass1)
    {
        // We no longer need this storage
        DbgGlobalFree (lppnLocalVars->hGlbVector); lppnLocalVars->hGlbVector = NULL;
    } // End IF

    return bRet;
} // End PN_VectorRes


//***************************************************************************
//  $PN_CoeffType
//
//  Return the coefficient type (I, F, R, V, ±)
//***************************************************************************

WCHAR PN_CoeffType
    (PNNUMTYPE chType)                  // The numeric type (see PNNUMTYPE)

{
    // Split cases based upon the PNNUMTYPE
    switch (chType)
    {
        case PN_NUMTYPE_BOOL:
        case PN_NUMTYPE_INT:
        case PN_NUMTYPE_HC2I:
        case PN_NUMTYPE_HC4I:
        case PN_NUMTYPE_HC8I:
            return L'I';

        case PN_NUMTYPE_FLT:
        case PN_NUMTYPE_HC2F:
        case PN_NUMTYPE_HC4F:
        case PN_NUMTYPE_HC8F:
            return L'F';

        case PN_NUMTYPE_RAT:
        case PN_NUMTYPE_HC2R:
        case PN_NUMTYPE_HC4R:
        case PN_NUMTYPE_HC8R:
            return L'R';

        case PN_NUMTYPE_VFP:
        case PN_NUMTYPE_HC2V:
        case PN_NUMTYPE_HC4V:
        case PN_NUMTYPE_HC8V:
            return L'V';

        case PN_NUMTYPE_ARB:
        case PN_NUMTYPE_BA2F:
        case PN_NUMTYPE_BA4F:
        case PN_NUMTYPE_BA8F:
            return DEF_ARBSEP_W;

        defstop
            return L'?';
    } // End SWITCH
} // End PN_CoeffType


//***************************************************************************
//  $PN_SetInfinity
//
//  Set a constant infinity
//***************************************************************************

PN_YYSTYPE PN_SetInfinity
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     PNNUMTYPE     pnNumType,           // The suggested PN_NUMTYPE_xx
     size_t        uNumStart,           // The starting offset in lpszStart
     int           iInfSgn,             // The sign of infinity (1 for positive, -1 for negative)
     LPAPLINT      lpiVfpPrec)          // Ptr to VFP Precision (NULL = none, ptr to 0 = default)

{
    PN_YYSTYPE  pnYYRes = {0};          // The result
    mpfr_prec_t uDefPrec;               // Default VFP precision
    int         i,                      // Loop counter
                iHCDim;                 // HC Dimension (1, 2, 4, 8)

    Assert (iInfSgn EQ 1 || iInfSgn EQ -1);

    // Save the starting offset
    pnYYRes.uNumStart = uNumStart;

    // Get the numeric type
    pnYYRes.chType = (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT) ? pnNumType : lppnLocalVars->chComType;

    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDim = TranslateNumTypeToHCDim (pnYYRes.chType);

    // Split cases based upon the current numeric type
    switch (pnYYRes.chType)
    {
        case PN_NUMTYPE_FLT:
        case PN_NUMTYPE_HC2F:
        case PN_NUMTYPE_HC4F:
        case PN_NUMTYPE_HC8F:
            // Mark as +/- infinity
            pnYYRes.at.aplFloat = (iInfSgn EQ 1) ? fltPosInfinity : fltNegInfinity;

            // Loop through the imaginary parts
            for (i = 1; i < iHCDim; i++)
                // Initialize to 0
                pnYYRes.at.aplHC8F.parts[i] = 0.0;
            break;

        case PN_NUMTYPE_RAT:
        case PN_NUMTYPE_HC2R:
        case PN_NUMTYPE_HC4R:
        case PN_NUMTYPE_HC8R:
            // Mark as _/- infinity
            mpq_set_infsub (&pnYYRes.at.aplRat, iInfSgn);

            // Loop through the imaginary parts
            for (i = 1; i < iHCDim; i++)
                // Initialize to 0/1
                mpq_init (&pnYYRes.at.aplHC8R.parts[i]);
            break;

        case PN_NUMTYPE_VFP:
        case PN_NUMTYPE_HC2V:
        case PN_NUMTYPE_HC4V:
        case PN_NUMTYPE_HC8V:
            if (lpiVfpPrec NE NULL && *lpiVfpPrec NE 0)
            {
                // Validate the desired precision
                if (!ValidateIntegerTest (lpiVfpPrec,           // Ptr to the integer to test
                                          DEF_MIN_QUADFPC,      // Low range value (inclusive)
                                          DEF_MIN_QUADFPC,      // High ...
                                          TRUE))                // TRUE iff we're range limiting
                {
                    // Mark as invalid result
                    lppnLocalVars->bYYERROR = TRUE;

                    break;
                } else
                    uDefPrec = (UINT) *lpiVfpPrec;
            } else
                // Use the default precision
                uDefPrec = mpfr_get_default_prec ();

            // Initialize the VFP
            mpfr_init2 (&pnYYRes.at.aplVfp, uDefPrec);

            // Mark as +/- infinity
            mpfr_set_inf (&pnYYRes.at.aplVfp, iInfSgn);

            // Loop through the imaginary parts
            for (i = 1; i < iHCDim; i++)
                // Initialize to 0
                mpfr_init0 (&pnYYRes.at.aplHC8V.parts[i]);
            break;

        case PN_NUMTYPE_ARB:
        case PN_NUMTYPE_BA2F:
        case PN_NUMTYPE_BA4F:
        case PN_NUMTYPE_BA8F:
            if (lpiVfpPrec NE NULL && *lpiVfpPrec NE 0)
            {
                // Validate the desired precision
                if (!ValidateIntegerTest (lpiVfpPrec,           // Ptr to the integer to test
                                          DEF_MIN_QUADFPC,      // Low range value (inclusive)
                                          DEF_MIN_QUADFPC,      // High ...
                                          TRUE))                // TRUE iff we're range limiting
                {
                    // Mark as invalid result
                    lppnLocalVars->bYYERROR = TRUE;

                    break;
                } else
                    uDefPrec = (UINT) *lpiVfpPrec;
            } else
                // Use the default precision
                uDefPrec = mpfr_get_default_prec ();    // Use MPFR here

            // Initialize the ARB
            Myarb_init (&pnYYRes.at.aplArb);

            // Mark as +/- infinity
            arb_set_inf (&pnYYRes.at.aplArb, iInfSgn);

            // Loop through the imaginary parts
            for (i = 1; i < iHCDim; i++)
                // Initialize to 0/0
                Myarb_init (&pnYYRes.at.aplBA8F.parts[i]);
            break;

        case PN_NUMTYPE_BOOL:           // Can't happen -- no such type of infinity
        case PN_NUMTYPE_INT:            // ...
        case PN_NUMTYPE_HC2I:           // ...
        case PN_NUMTYPE_HC4I:           // ...
        case PN_NUMTYPE_HC8I:           // ...
        defstop
            break;
    } // End SWITCH

    return pnYYRes;
} // End PN_SetInfinity


//***************************************************************************
//  $PN_SetNaN
//
//  Set a constant NaN
//***************************************************************************

PN_YYSTYPE PN_SetNaN
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     PNNUMTYPE     pnNumType,           // The suggested PN_NUMTYPE_xx
     size_t        uNumStart,           // The starting offset in lpszStart
     LPAPLINT      lpiVfpPrec)          // Ptr to VFP Precision (NULL = none, ptr to 0 = default)

{
    PN_YYSTYPE  pnYYRes = {0};          // The result
    mpfr_prec_t uDefPrec;               // Default VFP precision
    int         i,                      // Loop counter
                iHCDim;                 // HC Dimension (1, 2, 4, 8)

    // Save the starting offset
    pnYYRes.uNumStart = uNumStart;

    // Get the numeric type
    pnYYRes.chType = (lppnLocalVars->chComType EQ PN_NUMTYPE_INIT) ? pnNumType : lppnLocalVars->chComType;

    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDim = TranslateNumTypeToHCDim (pnYYRes.chType);

    // Split cases based upon the current numeric type
    switch (pnYYRes.chType)
    {
        case PN_NUMTYPE_FLT:
        case PN_NUMTYPE_HC2F:
        case PN_NUMTYPE_HC4F:
        case PN_NUMTYPE_HC8F:
            // Save as a NaN
            pnYYRes.at.aplFloat = fltNaN;

            // Loop through the imaginary parts
            for (i = 1; i < iHCDim; i++)
                // Initialize to 0
                pnYYRes.at.aplHC8F.parts[i] = 0.0;
            break;

        case PN_NUMTYPE_RAT:
        case PN_NUMTYPE_HC2R:
        case PN_NUMTYPE_HC4R:
        case PN_NUMTYPE_HC8R:
            // Save as a NaN
            mpq_set_nan (&pnYYRes.at.aplRat);

            // Loop through the imaginary parts
            for (i = 1; i < iHCDim; i++)
                // Initialize to 0/1
                mpq_init (&pnYYRes.at.aplHC8R.parts[i]);
            break;

        case PN_NUMTYPE_VFP:
        case PN_NUMTYPE_HC2V:
        case PN_NUMTYPE_HC4V:
        case PN_NUMTYPE_HC8V:
            if (lpiVfpPrec NE NULL && *lpiVfpPrec NE 0)
            {
                // Validate the desired precision
                if (!ValidateIntegerTest (lpiVfpPrec,           // Ptr to the integer to test
                                          DEF_MIN_QUADFPC,      // Low range value (inclusive)
                                          DEF_MIN_QUADFPC,      // High ...
                                          TRUE))                // TRUE iff we're range limiting
                {
                    // Mark as invalid result
                    lppnLocalVars->bYYERROR = TRUE;

                    break;
                } else
                    uDefPrec = (UINT) *lpiVfpPrec;
            } else
                // Use the default precision
                uDefPrec = mpfr_get_default_prec ();

            // Initialize the VFP
            mpfr_init2 (&pnYYRes.at.aplVfp, uDefPrec);

            // Save as a NaN
            mpfr_set_nan (&pnYYRes.at.aplVfp);

            // Loop through the imaginary parts
            for (i = 1; i < iHCDim; i++)
                // Initialize to 0
                mpfr_init0 (&pnYYRes.at.aplHC8V.parts[i]);
            break;

        case PN_NUMTYPE_ARB:
        case PN_NUMTYPE_BA2F:
        case PN_NUMTYPE_BA4F:
        case PN_NUMTYPE_BA8F:
            if (lpiVfpPrec NE NULL && *lpiVfpPrec NE 0)
            {
                // Validate the desired precision
                if (!ValidateIntegerTest (lpiVfpPrec,           // Ptr to the integer to test
                                          DEF_MIN_QUADFPC,      // Low range value (inclusive)
                                          DEF_MIN_QUADFPC,      // High ...
                                          TRUE))                // TRUE iff we're range limiting
                {
                    // Mark as invalid result
                    lppnLocalVars->bYYERROR = TRUE;

                    break;
                } else
                    uDefPrec = (UINT) *lpiVfpPrec;
            } else
                // Use the default precision
                uDefPrec = mpfr_get_default_prec ();    // Use MPFR here

            // Initialize the ARB
            Myarb_init (&pnYYRes.at.aplArb);

            // Save as a NaN
            arb_set_nan (&pnYYRes.at.aplArb);

            // Loop through the imaginary parts
            for (i = 1; i < iHCDim; i++)
                // Initialize to 0/0
                Myarb_init (&pnYYRes.at.aplBA8F.parts[i]);
            break;

        case PN_NUMTYPE_BOOL:           // Can't happen -- no such type of infinity
        case PN_NUMTYPE_INT:            // ...
        case PN_NUMTYPE_HC2I:           // ...
        case PN_NUMTYPE_HC4I:           // ...
        case PN_NUMTYPE_HC8I:           // ...
        defstop
            break;
    } // End SWITCH

    return pnYYRes;
} // End PN_SetNaN


//***************************************************************************
//  $PN_Copy
//***************************************************************************

void PN_Copy
    (LPPN_YYSTYPE lpYYDest,
     LPPN_YYSTYPE lpYYSrc)

{
    Assert (lpYYDest->chType EQ lpYYSrc->chType);

    // Split cases based upon the current numeric type
    switch (lpYYSrc->chType)
    {
        case PN_NUMTYPE_BOOL:           // Just move the data

        case PN_NUMTYPE_INT :           // ...
        case PN_NUMTYPE_HC2I:           // ...
        case PN_NUMTYPE_HC4I:           // ...
        case PN_NUMTYPE_HC8I:           // ...

        case PN_NUMTYPE_FLT :           // ...
        case PN_NUMTYPE_HC2F:           // ...
        case PN_NUMTYPE_HC4F:           // ...
        case PN_NUMTYPE_HC8F:           // ...
            CopyAll (lpYYDest, lpYYSrc);

            break;

        case PN_NUMTYPE_RAT :           // Copy the RAT
            mphc1r_set (&lpYYDest->at.aplHC1R,
                        &lpYYSrc ->at.aplHC1R);
            break;

        case PN_NUMTYPE_HC2R:           // Copy the RAT
            mphc2r_set (&lpYYDest->at.aplHC2R,
                        &lpYYSrc ->at.aplHC2R);
            break;

        case PN_NUMTYPE_HC4R:           // Copy the RAT
            mphc4r_set (&lpYYDest->at.aplHC4R,
                        &lpYYSrc ->at.aplHC4R);
            break;

        case PN_NUMTYPE_HC8R:           // Copy the RAT
            mphc8r_set (&lpYYDest->at.aplHC8R,
                        &lpYYSrc ->at.aplHC8R);
            break;

        case PN_NUMTYPE_VFP :           // Copy the VFP
            mphc1v_set (&lpYYDest->at.aplHC1V,
                        &lpYYSrc ->at.aplHC1V);
            break;

        case PN_NUMTYPE_ARB :           // Copy the ARB
            arb1f_set (&lpYYDest->at.aplBA1F,
                       &lpYYSrc ->at.aplBA1F);
            break;

        case PN_NUMTYPE_HC2V:           // Copy the VFP
            mphc2v_set (&lpYYDest->at.aplHC2V,
                        &lpYYSrc ->at.aplHC2V);
            break;

        case PN_NUMTYPE_HC4V:           // Copy the VFP
            mphc4v_set (&lpYYDest->at.aplHC4V,
                        &lpYYSrc ->at.aplHC4V);
            break;

        case PN_NUMTYPE_HC8V:           // Copy the VFP
            mphc8v_set (&lpYYDest->at.aplHC8V,
                        &lpYYSrc ->at.aplHC8V);
            break;

        case PN_NUMTYPE_BA2F:           // Copy the ARB
            arb2f_set (&lpYYDest->at.aplBA2F,
                       &lpYYSrc ->at.aplBA2F);
            break;

        case PN_NUMTYPE_BA4F:           // Copy the ARB
            arb4f_set (&lpYYDest->at.aplBA4F,
                       &lpYYSrc ->at.aplBA4F);
            break;

        case PN_NUMTYPE_BA8F:           // Copy the ARB
            arb8f_set (&lpYYDest->at.aplBA8F,
                       &lpYYSrc ->at.aplBA8F);
            break;

        defstop
            break;
    } // End SWITCH
} // End PN_Copy


//***************************************************************************
//  $PN_Free
//***************************************************************************

void PN_Free
    (LPPN_YYSTYPE lpYYRes)

{
    // Split cases based upon the current numeric type
    switch (lpYYRes->chType)
    {
        case PN_NUMTYPE_BOOL:           // Nothing to do when freeing

        case PN_NUMTYPE_INT :           // Nothing to do when freeing
        case PN_NUMTYPE_HC2I:           // ...
        case PN_NUMTYPE_HC4I:           // ...
        case PN_NUMTYPE_HC8I:           // ...

        case PN_NUMTYPE_FLT :           // Nothing to do when freeing
        case PN_NUMTYPE_HC2F:           // ...
        case PN_NUMTYPE_HC4F:           // ...
        case PN_NUMTYPE_HC8F:           // ...
            break;

        case PN_NUMTYPE_RAT :           // Free the RAT
            Myhc1r_clear (&lpYYRes->at.aplHC1R);

            break;

        case PN_NUMTYPE_HC2R:           // Free the RAT
            Myhc2r_clear (&lpYYRes->at.aplHC2R);

            break;

        case PN_NUMTYPE_HC4R:           // Free the RAT
            Myhc4r_clear (&lpYYRes->at.aplHC4R);

            break;

        case PN_NUMTYPE_HC8R:           // Free the RAT
            Myhc8r_clear (&lpYYRes->at.aplHC8R);

            break;

        case PN_NUMTYPE_VFP :           // Free the VFP
            Myhc1v_clear (&lpYYRes->at.aplHC1V);

            break;

        case PN_NUMTYPE_ARB :           // Free the ARB
            arb1f_clear (&lpYYRes->at.aplBA1F);

            break;

        case PN_NUMTYPE_HC2V:           // Free the VFP
            Myhc2v_clear (&lpYYRes->at.aplHC2V);

            break;

        case PN_NUMTYPE_HC4V:           // Free the VFP
            Myhc4v_clear (&lpYYRes->at.aplHC4V);

            break;

        case PN_NUMTYPE_HC8V:           // Free the VFP
            Myhc8v_clear (&lpYYRes->at.aplHC8V);

            break;

        case PN_NUMTYPE_BA2F:           // Free the ARB
            arb2f_clear (&lpYYRes->at.aplBA2F);

            break;

        case PN_NUMTYPE_BA4F:           // Free the ARB
            arb4f_clear (&lpYYRes->at.aplBA4F);

            break;

        case PN_NUMTYPE_BA8F:           // Free the ARB
            arb8f_clear (&lpYYRes->at.aplBA8F);

            break;

        defstop
            break;
    } // End SWITCH
} // End PN_Free


//***************************************************************************
//  End of File: pn_proc.c
//***************************************************************************
