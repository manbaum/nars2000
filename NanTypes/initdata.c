//***************************************************************************
//  NARS2000 -- Initialization of Data
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


extern PRIMSPEC PrimSpecBar;
extern PRIMSPEC PrimSpecCircle;
extern PRIMSPEC PrimSpecCircleStar;
extern PRIMSPEC PrimSpecColonBar;
extern PRIMSPEC PrimSpecDoubleTildeInter;
extern PRIMSPEC PrimSpecDoubleTildeUnion;
extern PRIMSPEC PrimSpecDoubleTildeLt;
extern PRIMSPEC PrimSpecDoubleTildeLe;
extern PRIMSPEC PrimSpecDoubleTildeEq;
extern PRIMSPEC PrimSpecDoubleTildeGe;
extern PRIMSPEC PrimSpecDoubleTildeGt;
extern PRIMSPEC PrimSpecDoubleTildeNe;
extern PRIMSPEC PrimSpecDoubleTildeLs;
extern PRIMSPEC PrimSpecDoubleTildeRs;
extern PRIMSPEC PrimSpecDoubleTildeJot;
extern PRIMSPEC PrimSpecDownCaret;
extern PRIMSPEC PrimSpecDownCaretTilde;
extern PRIMSPEC PrimSpecDownStile;
extern PRIMSPEC PrimSpecEqual;
extern PRIMSPEC PrimSpecLeftCaret;
extern PRIMSPEC PrimSpecLeftCaretUnderbar;
extern PRIMSPEC PrimSpecNotEqual;
extern PRIMSPEC PrimSpecPlus;
extern PRIMSPEC PrimSpecQuoteDot;
extern PRIMSPEC PrimSpecQuery;
extern PRIMSPEC PrimSpecRightCaret;
extern PRIMSPEC PrimSpecRightCaretUnderbar;
extern PRIMSPEC PrimSpecRoot;
extern PRIMSPEC PrimSpecStar;
extern PRIMSPEC PrimSpecStile;
extern PRIMSPEC PrimSpecTilde;
extern PRIMSPEC PrimSpecTimes;
extern PRIMSPEC PrimSpecUpCaret;
extern PRIMSPEC PrimSpecUpCaretTilde;
extern PRIMSPEC PrimSpecUpStile;


// Monadic Operators TO DO                             Monadic          Dyadic
/////// PrimOpStileTilde_EM_YY                      // ERROR            Partition (*)

// Dyadic  Operators TO DO                             Monadic          Dyadic
/////// PrimOpDieresisDel_EM_YY                     // ERROR            Dual (*)

// (*) = Unfinished

// First coordinate functions handled by common function
#define PrimFnCircleBar_EM_YY   PrimFnCircleStile_EM_YY
#define PrimFnCommaBar_EM_YY    PrimFnComma_EM_YY
#define PrimFnSlashBar_EM_YY    PrimFnSlash_EM_YY
#define PrimFnSlopeBar_EM_YY    PrimFnSlope_EM_YY
#define PrimOpSlashBar_EM_YY    PrimOpSlash_EM_YY
#define PrimOpSlopeBar_EM_YY    PrimOpSlope_EM_YY

// This var is needed by the GSL routines.
double __infinity;


//***************************************************************************
//  $InitPrimTabs
//
//  Initialize various primitive function, operator,
//    prototype tables, and flags
//***************************************************************************

void InitPrimTabs
    (void)

{
    InitConstants ();           // Initialize various constants
    InitPrimFns ();             // ...        prim fcn/opr jump tables
    InitPrimProtoFns ();        // ...        prim fcn/opr prototype jump table
    InitPrimSpecs ();           // ...        table of PRIMSPECs
    InitPrimFlags ();           // ...        table of primitive fcn/opr flags
    InitIdentityElements ();    // ...        identity elements
    InitPrimeTabs ();           // ...        prime # tables
    InitVarOprTab ();             // ...        Variant operator tables
} // End InitPrimTabs


//***************************************************************************
//  $InitConstants
//
//  Initialize various constants none of which allocate global or heap memory
//***************************************************************************

void InitConstants
    (void)

{
    APLINT aplInteger;          // Temporary value

    // Create various floating point constants
    aplInteger = POS_INFINITY; fltPosInfinity = *(LPAPLFLOAT) &aplInteger;
                                  __infinity  = fltPosInfinity;
    aplInteger = NEG_INFINITY; fltNegInfinity = *(LPAPLFLOAT) &aplInteger;
    aplInteger = QUIET_NAN;    fltNaN         = *(LPAPLFLOAT) &aplInteger;
    aplInteger = FLOAT2POW53;  Float2Pow53    = *(LPAPLFLOAT) &aplInteger;
    aplInteger = FLOATPI;      FloatPi        = *(LPAPLFLOAT) &aplInteger;
    aplInteger = FLOATGAMMA;   FloatGamma     = *(LPAPLFLOAT) &aplInteger;
    aplInteger = FLOATE;       FloatE         = *(LPAPLFLOAT) &aplInteger;

    // Create various Hypercomplex constants

    // Construct Pi and Gamma as HC8F
    aplHC8F_Pi   .parts[0] = FloatPi;
    aplHC8F_E    .parts[0] = FloatE;
    aplHC8F_Gamma.parts[0] = FloatGamma;

    // Get # ticks per second to be used as a conversion
    //   factor for QueryPerformanceCounter into seconds
    QueryPerformanceFrequency (&liTicksPerSec);
} // End InitConstants


//***************************************************************************
//  $InitGlbNumConstants
//
//  Initialize global numeric constants
//***************************************************************************

void InitGlbNumConstants
    (void)

{
    // Clear the MPFR cache constants
    mpfr_free_cache ();

    // Use our own memory management functions for MPIR/MPFR
    mp_set_memory_functions (mp_alloc, mp_realloc, mp_free);

    // Set the default precision for the following 64-bit VFP constants
    mpfr_set_default_prec (128);

    // Initialize the MPI, RAT, and VFP constants
    mpz_init_set_str  (&mpzMinInt  , "-9223372036854775808", 10);
    mpz_init_set_str  (&mpzMaxInt  ,  "9223372036854775807", 10);
    mpz_init_set_str  (&mpzMaxUInt , "18446744073709551615", 10);

    mpq_init_set_str  (&mpqMinInt  , "-9223372036854775808", 10);
    mpq_init_set_str  (&mpqMaxInt  ,  "9223372036854775807", 10);
    mpq_init_set_str  (&mpqMaxUInt , "18446744073709551615", 10);
    mpq_init_set_ui   (&mpqHalf    , 1, 2);

    mpfr_init_set_str (&mpfMinInt  , "-9223372036854775808", 10, MPFR_RNDN);
    mpfr_init_set_str (&mpfMaxInt  ,  "9223372036854775807", 10, MPFR_RNDN);
    mpfr_init_set_str (&mpfMaxUInt , "18446744073709551615", 10, MPFR_RNDN);
    mpfr_set_inf      (&mpfPosInfinity                     ,  1);
    mpfr_set_inf      (&mpfNegInfinity                     , -1);
    mpfr_init_set_d   (&mpfHalf    , 0.5                   , MPFR_RNDN);

    // Constants 0, 1, -1, i, -i in both RAT and VFP types
    mphc8r_init  (&conmpoi_0 ); mpq_set_si  (&conmpoi_0 .parts[0],  0, 1);
    mphc8r_init  (&conmpoi_1 ); mpq_set_si  (&conmpoi_1 .parts[0],  1, 1);
    mphc8r_init  (&conmpoi_N1); mpq_set_si  (&conmpoi_N1.parts[0], -1, 1);
    mphc8r_init  (&conmpoi_I ); mpq_set_si  (&conmpoi_I .parts[1],  1, 1);
    mphc8r_init  (&conmpoi_NI); mpq_set_si  (&conmpoi_NI.parts[1], -1, 1);

    mphc8v_init0 (&conmpof_0 ); mpfr_set_si (&conmpof_0 .parts[0],  0, MPFR_RNDN);
    mphc8v_init0 (&conmpof_1 ); mpfr_set_si (&conmpof_1 .parts[0],  1, MPFR_RNDN);
    mphc8v_init0 (&conmpof_N1); mpfr_set_si (&conmpof_N1.parts[0], -1, MPFR_RNDN);
    mphc8v_init0 (&conmpof_I ); mpfr_set_si (&conmpof_I .parts[1],  1, MPFR_RNDN);
    mphc8v_init0 (&conmpof_NI); mpfr_set_si (&conmpof_NI.parts[1], -1, MPFR_RNDN);

    // Use our own invalid operation functions for MPIR/MPFR
    mp_set_invalid_functions (mpz_invalid, mpq_invalid, mpfr_invalid);

    // Create various Ball Arithmetic constants
    arb_pos_inf (&arbPosInfinity);
    arb_neg_inf (&arbNegInfinity);
    Myarb_init  (&arbHalf); arb_set_d   (&arbHalf    , 0.5);

    arb8f_init (&conbaof_0 ); arb_set_si (&conbaof_0 .parts[0],  0);
    arb8f_init (&conbaof_1 ); arb_set_si (&conbaof_1 .parts[0],  1);
    arb8f_init (&conbaof_N1); arb_set_si (&conbaof_N1.parts[0], -1);
    arb8f_init (&conbaof_I ); arb_set_si (&conbaof_I .parts[1],  1);
    arb8f_init (&conbaof_NI); arb_set_si (&conbaof_NI.parts[1], -1);
} // End InitGlbNumConstants


//***************************************************************************
//  $UninitGlbNumConstants
//
//  Uninitialize global numeric constants
//***************************************************************************

void UninitGlbNumConstants
    (void)

{
    // Uninitialize the ARB constants
    arb8f_clear  (&conbaof_NI);
    arb8f_clear  (&conbaof_I );
    arb8f_clear  (&conbaof_N1);
    arb8f_clear  (&conbaof_1 );
    arb8f_clear  (&conbaof_0 );

    Myarb_clear  (&arbHalf);

    // Uninitialize the MPI, RAT, and VFP constants
    Myhc8v_clear (&conmpof_NI     );
    Myhc8v_clear (&conmpof_I      );
    Myhc8v_clear (&conmpof_N1     );
    Myhc8v_clear (&conmpof_1      );
    Myhc8v_clear (&conmpof_0      );

    Myhc8r_clear (&conmpoi_NI     );
    Myhc8r_clear (&conmpoi_I      );
    Myhc8r_clear (&conmpoi_N1     );
    Myhc8r_clear (&conmpoi_1      );
    Myhc8r_clear (&conmpoi_0      );

    Myf_clear    (&mpfHalf        );
    Myf_clear    (&mpfNegInfinity );
    Myf_clear    (&mpfPosInfinity );
    Myf_clear    (&mpfMaxUInt     );
    Myf_clear    (&mpfMaxInt      );
    Myf_clear    (&mpfMinInt      );
    Myq_clear    (&mpqHalf        );
    Myq_clear    (&mpqMaxUInt     );
    Myq_clear    (&mpqMaxInt      );
    Myq_clear    (&mpqMinInt      );
    Myz_clear    (&mpzMaxUInt     );
    Myz_clear    (&mpzMaxInt      );
    Myz_clear    (&mpzMinInt      );
} // End UninitGlbNumConstants


//***************************************************************************
//  $InitPTDVars
//
//  Initialize PerTabData vars
//***************************************************************************

void InitPTDVars
    (LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    // If these vars have been initialized, ...
    if (lpMemPTD->bInit_E)
        // Free them
        FreePTD_E (lpMemPTD);

    // If these vars have been initialized, ...
    if (lpMemPTD->bInit_Gamma)
        // Free them
        FreePTD_Gamma (lpMemPTD);

    // If these vars have been initialized, ...
    if (lpMemPTD->bInit_Pi)
        // Free them
        FreePTD_Pi (lpMemPTD);

    // Initialize the PTD vars
    InitPTD_E     (lpMemPTD);
    InitPTD_Gamma (lpMemPTD);
    InitPTD_Pi    (lpMemPTD);
} // InitPTDVars


//***************************************************************************
//  $FreePTD_E
//***************************************************************************

void FreePTD_E
    (LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    Myhc8v_clear  (&lpMemPTD->mpfrHC8V_E);
    arb8f_clear   (&lpMemPTD->arbBA8F_E);

    // Clear the flag
    lpMemPTD->bInit_E = FALSE;
} // End FreePTD_E


//****************************************************************************
//  $FreePTD_Gamma
//***************************************************************************

void FreePTD_Gamma
    (LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    Myhc8v_clear  (&lpMemPTD->mpfrHC8V_Gamma);
    arb8f_clear   (&lpMemPTD->arbBA8F_Gamma);

    // Clear the flag
    lpMemPTD->bInit_Gamma = FALSE;
} // End FreePTD_Gamma


//***************************************************************************
//  $FreePTD_Pi
//***************************************************************************

void FreePTD_Pi
    (LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    Myhc8v_clear  (&lpMemPTD->mpfrHC8V_Pi);
    arb8f_clear   (&lpMemPTD->arbBA8F_Pi);

    // Clear the flag
    lpMemPTD->bInit_Pi = FALSE;
} // End FreePTD_Pi


//***************************************************************************
//  $InitPTD_E
//
//  Initialize VFP/ARB E
//***************************************************************************

void InitPTD_E
    (LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // If these vars have been initialized already, ...
    if (lpMemPTD->bInit_E)
        // Free them
        FreePTD_E (lpMemPTD);

    // Create a local value for e
    mphc8v_init0 (&lpMemPTD->mpfrHC8V_E);
    mpfr_set_ui  (&lpMemPTD->mpfrHC8V_E.parts[0],                              1, MPFR_RNDN);
    mpfr_exp     (&lpMemPTD->mpfrHC8V_E.parts[0], &lpMemPTD->mpfrHC8V_E.parts[0], MPFR_RNDN);

    // Create a local value for e
    arb8f_init  (&lpMemPTD->arbBA8F_E);
    arb_const_e (&lpMemPTD->arbBA8F_E.parts[0], prec);

    // Set the flag
    lpMemPTD->bInit_E = TRUE;
} // End InitPTD_E


//***************************************************************************
//  $InitPTD_Gamma
//
//  Initialize VFP/ARB Gamma
//***************************************************************************

void InitPTD_Gamma
    (LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // If these vars have been initialized already, ...
    if (lpMemPTD->bInit_Gamma)
        // Free them
        FreePTD_Gamma (lpMemPTD);

    // Create a local value for Gamma
    mphc8v_init0     (&lpMemPTD->mpfrHC8V_Gamma);
    mpfr_const_euler (&lpMemPTD->mpfrHC8V_Gamma.parts[0], MPFR_RNDN);

    // Create a local value for Gamma
    arb8f_init      (&lpMemPTD->arbBA8F_Gamma);
    arb_const_euler (&lpMemPTD->arbBA8F_Gamma.parts[0], prec);

    // Set the flag
    lpMemPTD->bInit_Gamma = TRUE;
} // End InitPTD_Gamma


//***************************************************************************
//  $InitPTD_Pi
//
//  Initialize VFP/ARB Pi
//***************************************************************************

void InitPTD_Pi
    (LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // If these vars have been initialized already, ...
    if (lpMemPTD->bInit_Pi)
        // Free them
        FreePTD_Pi (lpMemPTD);

    // Create a local value for Pi
    mphc8v_init0     (&lpMemPTD->mpfrHC8V_Pi);
    mpfr_const_pi    (&lpMemPTD->mpfrHC8V_Pi.parts[0], MPFR_RNDN);

    // Create a local value for Pi
    arb8f_init      (&lpMemPTD->arbBA8F_Pi);
    arb_const_pi    (&lpMemPTD->arbBA8F_Pi.parts[0], prec);

    // Set the flag
    lpMemPTD->bInit_Pi = TRUE;
} // End InitPTD_Pi


//***************************************************************************
//  $InitVfpPrecision
//
//  Initalize the VFP default precision
//***************************************************************************

void InitVfpPrecision
    (APLUINT uDefPrec)          // Default VFP precision

{
    // Set the new default precision
    mpfr_set_default_prec ((mpfr_prec_t) uDefPrec);

    // Calculate the # decimal digits in the new precision
    nDigitsFPC = 1 + (UINT) floor ((APLFLOAT) (uDefPrec) * M_LN2 / M_LN10);
} // End InitVfpPrecision


//***************************************************************************
//  $InitPrimFns
//
//  Initialize the primitive function jump table
//***************************************************************************

void InitPrimFns
    (void)

{
    //***************************************************************
    //  Primitive Functions
    //  Primitive operators are handled case-by-case in
    //    ExecOp1_EM_YY and ExecOp2_EM_YY.
    //***************************************************************
                                                                                // Alt-'a' - alpha
    Init1PrimFn (UTF16_UPTACK              , &PrimFnUpTack_EM_YY             );  // Alt-'b' - up tack
    Init1PrimFn (UTF16_UPSHOE              , &PrimFnUpShoe_EM_YY             );  // Alt-'c' - up shoe
    Init1PrimFn (UTF16_DOWNSTILE           , &PrimFnDownStile_EM_YY          );  // Alt-'d' - down stile
    Init1PrimFn (UTF16_EPSILON             , &PrimFnEpsilon_EM_YY            );  // Alt-'e' - epsilon
    Init1PrimFn (UTF16_EPSILON2            , &PrimFnEpsilon_EM_YY            );  //         - epsilon2
////                                                                             // Alt-'f' - infinity
////                                                                             // Alt-'g' - del
////                                                                             // Alt-'h' - delta
    Init1PrimFn (UTF16_IOTA                , &PrimFnIota_EM_YY               );  // Alt-'i' - iota
////                                                                             // Alt-'j' - jot (compose)
////                                                                             // Alt-'k' - single quote
////                                                                             // Alt-'l' - quad
////                                                                             // Alt-'m' - down-shoe-stile
    Init1PrimFn (UTF16_DOWNTACK            , &PrimFnDownTack_EM_YY           );  // Alt-'n' - down tack
    Init1PrimFn (UTF16_CIRCLE              , &PrimFnCircle_EM_YY             );  // Alt-'o' - circle
    Init1PrimFn (UTF16_CIRCLE2             , &PrimFnCircle_EM_YY             );  // Circle2
    Init1PrimFn (UTF16_PI                  , &PrimFnPi_EM_YY                 );  // Alt-'p' - pi
    Init1PrimFn (UTF16_QUERY               , &PrimFnQuery_EM_YY              );  //     '?'   question mark
    Init1PrimFn (UTF16_RHO                 , &PrimFnRho_EM_YY                );  // Alt-'r' - rho
    Init1PrimFn (UTF16_UPSTILE             , &PrimFnUpStile_EM_YY            );  // Alt-'s' - up stile
    Init1PrimFn (UTF16_TILDE               , &PrimFnTilde_EM_YY              );  // Alt-'t' - tilde
    Init1PrimFn (UTF16_TILDE2              , &PrimFnTilde_EM_YY              );  // Tilde2'
    Init1PrimFn (UTF16_DOWNARROW           , &PrimFnDownArrow_EM_YY          );  // Alt-'u' - down arrow
    Init1PrimFn (UTF16_DOWNSHOE            , &PrimFnDownShoe_EM_YY           );  // Alt-'v' - down shoe
                                                                                 // Alt-'w' - omega
    Init1PrimFn (UTF16_RIGHTSHOE           , &PrimFnRightShoe_EM_YY          );  // Alt-'x' - right shoe
    Init1PrimFn (UTF16_UPARROW             , &PrimFnUpArrow_EM_YY            );  // Alt-'y' - up arrow
    Init1PrimFn (UTF16_LEFTSHOE            , &PrimFnLeftShoe_EM_YY           );  // Alt-'z' - left shoe
    Init1PrimFn (UTF16_EQUALUNDERBAR       , &PrimFnEqualUnderbar_EM_YY      );  // Alt-'!' - match
////                                                                             // Alt-'"' - (none)
    Init1PrimFn (UTF16_DELSTILE            , &PrimFnDelStile_EM_YY           );  // Alt-'#' - grade-down
    Init1PrimFn (UTF16_DELTASTILE          , &PrimFnDeltaStile_EM_YY         );  // Alt-'$' - grade-up
    Init1PrimFn (UTF16_CIRCLESTILE         , &PrimFnCircleStile_EM_YY        );  // Alt-'%' - rotate
    Init1PrimFn (UTF16_CIRCLESLOPE         , &PrimFnCircleSlope_EM_YY        );  // Alt-'^' - transpose
    Init1PrimFn (UTF16_CIRCLEBAR           , &PrimFnCircleBar_EM_YY          );  // Alt-'&' - circle-bar
    Init1PrimFn (UTF16_UPTACKJOT           , &PrimFnUpTackJot_EM_YY          );  // Alt-'\''- execute
    Init1PrimFn (UTF16_DOWNCARETTILDE      , &PrimFnDownCaretTilde_EM_YY     );  // Alt-'(' - nor
    Init1PrimFn (UTF16_NOR                 , &PrimFnDownCaretTilde_EM_YY     );  // Alt-'(' - nor
    Init1PrimFn (UTF16_UPCARETTILDE        , &PrimFnUpCaretTilde_EM_YY       );  // Alt-')' - nand
    Init1PrimFn (UTF16_NAND                , &PrimFnUpCaretTilde_EM_YY       );  // Alt-')' - nand
    Init1PrimFn (UTF16_CIRCLESTAR          , &PrimFnCircleStar_EM_YY         );  // Alt-'*' - log
    Init1PrimFn (UTF16_DOMINO              , &PrimFnDomino_EM_YY             );  // Alt-'+' - domino
////                                                                             // Alt-',' - lamp
    Init1PrimFn (UTF16_TIMES               , &PrimFnTimes_EM_YY              );  // Alt-'-' - times
    Init1PrimFn (UTF16_SLOPEBAR            , &PrimFnSlopeBar_EM_YY           );  // Alt-'.' - slope-bar
    Init1PrimFn (UTF16_SLASHBAR            , &PrimFnSlashBar_EM_YY           );  // Alt-'/' - slash-bar
    Init1PrimFn (UTF16_UPCARET             , &PrimFnUpCaret_EM_YY            );  // Alt-'0' - and (94??)
    Init1PrimFn (UTF16_CIRCUMFLEX          , &PrimFnUpCaret_EM_YY            );  // '^'
////                                                                             // Alt-'1' - dieresis
////                                                                             // Alt-'2' - overbar
    Init1PrimFn (UTF16_LEFTCARET           , &PrimFnLeftCaret_EM_YY          );  // Alt-'3' - less
    Init1PrimFn (UTF16_LEFTCARETUNDERBAR   , &PrimFnLeftCaretUnderbar_EM_YY  );  // Alt-'4' - not more
    Init1PrimFn (UTF16_LEFTCARETUNDERBAR2  , &PrimFnLeftCaretUnderbar_EM_YY  );  // Not more2
    Init1PrimFn (UTF16_EQUAL               , &PrimFnEqual_EM_YY              );  // Alt-'5' - equal
    Init1PrimFn (UTF16_RIGHTCARETUNDERBAR  , &PrimFnRightCaretUnderbar_EM_YY );  // Alt-'6' - not less
    Init1PrimFn (UTF16_RIGHTCARETUNDERBAR2 , &PrimFnRightCaretUnderbar_EM_YY );  // Not less2
    Init1PrimFn (UTF16_RIGHTCARET          , &PrimFnRightCaret_EM_YY         );  // Alt-'7' - more
    Init1PrimFn (UTF16_NOTEQUAL            , &PrimFnNotEqual_EM_YY           );  // Alt-'8' - not equal
    Init1PrimFn (UTF16_DOWNCARET           , &PrimFnDownCaret_EM_YY          );  // Alt-'9' - or
////                                                                             // Alt-':' - (none)
    Init1PrimFn (UTF16_DOWNTACKJOT         , &PrimFnDownTackJot_EM_YY        );  // Alt-';' - format
////                                                                             // Alt-'<' - (none)
    Init1PrimFn (UTF16_COLONBAR            , &PrimFnColonBar_EM_YY           );  // Alt-'=' - divide
////                                                                             // Alt-'>' - (none)
////                                                                             // Alt-'?' - circle-middle-dot
    Init1PrimFn (UTF16_NOTEQUALUNDERBAR    , &PrimFnNotEqualUnderbar_EM_YY   );  // Alt-'@' - mismatch
////                                                                             // Alt-'A' - (none)
////                                                                             // Alt-'B' - (none)
////                                                                             // Alt-'C' - (none)
////                                                                             // Alt-'D' - (none)
    Init1PrimFn (UTF16_EPSILONUNDERBAR     , &PrimFnEpsilonUnderbar_EM_YY    );  // Alt-'E' - epsilon-underbar
////                                                                             // Alt-'F' - quad-jot
////                                                                             // Alt-'G' - dieresis-del (dual)
////                                                                             // Alt-'H' - delta-underbar
    Init1PrimFn (UTF16_IOTAUNDERBAR        , &PrimFnIotaUnderbar_EM_YY       );  // Alt-'I' - iota-underbar
////                                                                             // Alt-'J' - dieresis-jot (rank)
////                                                                             // Alt-'K' - (none)
    Init1PrimFn (UTF16_SQUAD               , &PrimFnSquad_EM_YY              );  // Alt-'L' - squad
////                                                                             // Alt-'M' - stile-tilde (partition)
////                                                                             // Alt-'N' - dieresis-downtack (convolution)
////                                                                             // Alt-'O' - dieresis-circle (composition)
////                                                                             // Alt-'P' - dieresis-star (power)
    Init1PrimFn (INDEX_DOUBLETILDE_INTER   , &PrimFnDoubleTildeInter_EM_YY   );  // Alt-'Q' - double-tilde-intersection (arbitrary precision)
    Init1PrimFn (INDEX_DOUBLETILDE_UNION   , &PrimFnDoubleTildeUnion_EM_YY   );  // Alt-'Q' - double-tilde-union        ...
    Init1PrimFn (INDEX_DOUBLETILDE_LT      , &PrimFnDoubleTildeLt_EM_YY      );  // Alt-'Q' - double-tilde-lt           ...
    Init1PrimFn (INDEX_DOUBLETILDE_LE      , &PrimFnDoubleTildeLe_EM_YY      );  // Alt-'Q' - double-tilde-le           ...
    Init1PrimFn (INDEX_DOUBLETILDE_EQ      , &PrimFnDoubleTildeEq_EM_YY      );  // Alt-'Q' - double-tilde-eq           ...
    Init1PrimFn (INDEX_DOUBLETILDE_GE      , &PrimFnDoubleTildeGe_EM_YY      );  // Alt-'Q' - double-tilde-ge           ...
    Init1PrimFn (INDEX_DOUBLETILDE_GT      , &PrimFnDoubleTildeGt_EM_YY      );  // Alt-'Q' - double-tilde-gt           ...
    Init1PrimFn (INDEX_DOUBLETILDE_NE      , &PrimFnDoubleTildeNe_EM_YY      );  // Alt-'Q' - double-tilde-ne           ...
    Init1PrimFn (INDEX_DOUBLETILDE_LS      , &PrimFnDoubleTildeNe_EM_YY      );  // Alt-'Q' - double-tilde-ls           ...
    Init1PrimFn (INDEX_DOUBLETILDE_RS      , &PrimFnDoubleTildeNe_EM_YY      );  // Alt-'Q' - double-tilde-rs           ...
    Init1PrimFn (INDEX_DOUBLETILDE_JOT     , &PrimFnDoubleTildeJot_EM_YY     );  // Alt-'Q' - double-tilde-jot          ...
    Init1PrimFn (UTF16_ROOT                , &PrimFnRoot_EM_YY               );  // Alt-'R' - root
    Init1PrimFn (UTF16_SECTION             , &PrimFnSection_EM_YY            );  // Alt-'S' - section (symmetric difference)
////                                                                             // Alt-'T' - dieresis-tilde (commute/duplicate)
////                                                                             // Alt-'U' - (none)
////                                                                             // Alt-'V' - (none)
////                                                                             // Alt-'W' - (none)
    Init1PrimFn (UTF16_RIGHTSHOEUNDERBAR   , &PrimFnRightShoeUnderbar_EM_YY  );  // Alt-'X' - right shoe
////                                                                             // Alt-'Y' - (none)
    Init1PrimFn (UTF16_LEFTSHOEUNDERBAR    , &PrimFnLeftShoeUnderbar_EM_YY   );  // Alt-'Z' - left shoe underbar
////                                                                             // Alt-'[' - left arrow
    Init1PrimFn (UTF16_LEFTTACK            , &PrimFnLeftTack_EM_YY           );  // Alt-'\' - left tack
////                                                                             // Alt-']' - right arrow
////                                                                             // Alt-'_' - variant
////                                                                             // Alt-'`' - diamond
////                                                                             // Alt-'{' - quote-quad
    Init1PrimFn (UTF16_RIGHTTACK           , &PrimFnRightTack_EM_YY          );  // Alt-'|' - right tack
////                                                                             // Alt-'}' - zilde
    Init1PrimFn (UTF16_COMMABAR            , &PrimFnCommaBar_EM_YY           );  // Alt-'~' - comma-bar
    Init1PrimFn (UTF16_BAR                 , &PrimFnBar_EM_YY                );  //     '-'
    Init1PrimFn (UTF16_BAR2                , &PrimFnBar_EM_YY                );  //     '-'
    Init1PrimFn (UTF16_DOT                 , &PrimFnDotDot_EM_YY             );  //     '.'
    Init1PrimFn (UTF16_PLUS                , &PrimFnPlus_EM_YY               );  //     '+'
    Init1PrimFn (UTF16_STILE               , &PrimFnStile_EM_YY              );  //     '|'
    Init1PrimFn (UTF16_STILE2              , &PrimFnStile_EM_YY              );  //     '|'
    Init1PrimFn (UTF16_COMMA               , &PrimFnComma_EM_YY              );  //     ','
    Init1PrimFn (UTF16_QUOTEDOT            , &PrimFnQuoteDot_EM_YY           );  //     '!'
    Init1PrimFn (UTF16_SLASH               , &PrimFnSlash_EM_YY              );  //     '/'
    Init1PrimFn (UTF16_SLOPE               , &PrimFnSlope_EM_YY              );  //     '\'
    Init1PrimFn (UTF16_STAR                , &PrimFnStar_EM_YY               );  //     '*'
    Init1PrimFn (UTF16_STAR2               , &PrimFnStar_EM_YY               );  //     '*'
////                                                                             //         - dot (inner product)
////                                                                             //         - jotdot (outer product)
} // End InitPrimFns


//***************************************************************************
//  $Init1PrimFn
//
//  Initialize a single primitive function/operator
//***************************************************************************

void Init1PrimFn
    (WCHAR     wchFn,
     LPPRIMFNS lpPrimFn)

{
    if (PrimFnsTab[FcnTrans (wchFn)] NE NULL)
        DbgStop ();         // We should never get here
    else
        PrimFnsTab[FcnTrans (wchFn)] = lpPrimFn;
} // End Init1PrimFn


//***************************************************************************
//  $PrimFn_EM
//
//  Default function for symbols for which there is no corresponding
//    primitive function/operator
//***************************************************************************

LPPL_YYSTYPE PrimFn_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFn_EM


// Monadic operators TO DO
#define PrimProtoOpStileTilde_EM_YY             PrimProtoOp_EM

// Dyadic operators TO DO
#define PrimProtoOpDieresisDel_EM_YY            PrimProtoOp_EM


// Primitive scalar functions DONE
#define PrimProtoFnBar_EM_YY                    PrimProtoFnScalar_EM_YY
#define PrimProtoFnCircle_EM_YY                 PrimProtoFnScalar_EM_YY
#define PrimProtoFnCircleStar_EM_YY             PrimProtoFnScalar_EM_YY
#define PrimProtoFnColonBar_EM_YY               PrimProtoFnScalar_EM_YY
#define PrimProtoFnDownCaret_EM_YY              PrimProtoFnScalar_EM_YY
#define PrimProtoFnDownCaretTilde_EM_YY         PrimProtoFnScalar_EM_YY
#define PrimProtoFnDownStile_EM_YY              PrimProtoFnScalar_EM_YY
#define PrimProtoFnEqual_EM_YY                  PrimProtoFnScalar_EM_YY
#define PrimProtoFnLeftCaret_EM_YY              PrimProtoFnScalar_EM_YY
#define PrimProtoFnLeftCaretUnderbar_EM_YY      PrimProtoFnScalar_EM_YY
#define PrimProtoFnNotEqual_EM_YY               PrimProtoFnScalar_EM_YY
#define PrimProtoFnPlus_EM_YY                   PrimProtoFnScalar_EM_YY
#define PrimProtoFnQuoteDot_EM_YY               PrimProtoFnScalar_EM_YY
#define PrimProtoFnRightCaret_EM_YY             PrimProtoFnScalar_EM_YY
#define PrimProtoFnRightCaretUnderbar_EM_YY     PrimProtoFnScalar_EM_YY
#define PrimProtoFnRoot_EM_YY                   PrimProtoFnScalar_EM_YY
#define PrimProtoFnStar_EM_YY                   PrimProtoFnScalar_EM_YY
#define PrimProtoFnStile_EM_YY                  PrimProtoFnScalar_EM_YY
#define PrimProtoFnTimes_EM_YY                  PrimProtoFnScalar_EM_YY
#define PrimProtoFnUpCaret_EM_YY                PrimProtoFnScalar_EM_YY
#define PrimProtoFnUpCaretTilde_EM_YY           PrimProtoFnScalar_EM_YY
#define PrimProtoFnUpStile_EM_YY                PrimProtoFnScalar_EM_YY


// First coordinate functions handled by common function
#define PrimProtoFnCircleBar_EM_YY   PrimProtoFnCircleStile_EM_YY
#define PrimProtoFnCommaBar_EM_YY    PrimProtoFnComma_EM_YY
#define PrimProtoFnSlashBar_EM_YY    PrimProtoFnSlash_EM_YY
#define PrimProtoFnSlopeBar_EM_YY    PrimProtoFnSlope_EM_YY
#define PrimProtoOpSlashBar_EM_YY    PrimProtoOpSlash_EM_YY
#define PrimProtoOpSlopeBar_EM_YY    PrimProtoOpSlope_EM_YY


//***************************************************************************
//  $InitPrimProtoFns
//
//  Initialize the primitive function and operator prototype jump table
//***************************************************************************

void InitPrimProtoFns
    (void)

{
    //***************************************************************
    //  Primitive Functions & operators
    //***************************************************************

                                                                                            // Alt-'a' - alpha
    InitPrimProtoFn (UTF16_UPTACK              , &PrimProtoFnUpTack_EM_YY             );    // Alt-'b' - up tack
    InitPrimProtoFn (UTF16_UPSHOE              , &PrimProtoFnUpShoe_EM_YY             );    // Alt-'c' - up shoe
    InitPrimProtoFn (UTF16_DOWNSTILE           , &PrimProtoFnDownStile_EM_YY          );    // Alt-'d' - down stile
    InitPrimProtoFn (UTF16_EPSILON             , &PrimProtoFnEpsilon_EM_YY            );    // Alt-'e' - epsilon
    InitPrimProtoFn (UTF16_EPSILON2            , &PrimProtoFnEpsilon_EM_YY            );    //         - epsilon2
////                                                                                        // Alt-'f' - infinity
////                                                                                        // Alt-'g' - del
////                                                                                        // Alt-'h' - delta
    InitPrimProtoFn (UTF16_IOTA                , &PrimProtoFnIota_EM_YY               );    // Alt-'i' - iota
    InitPrimProtoOp (UTF16_JOT                 , &PrimProtoOpJot_EM_YY                );    // Alt-'j' - jot (compose)
    InitPrimProtoOp (UTF16_JOT2                , &PrimProtoOpJot_EM_YY                );    // Jot2
    InitPrimProtoOp (UTF16_DOUBLESHRIEK        , &PrimProtoOpCombinatorial_EM_YY      );    // Alt-'k' - combinatorial
////                                                                                        // Alt-'l' - quad
    InitPrimProtoOp (UTF16_DOWNSHOESTILE       , &PrimProtoOpDownShoeStile_EM_YY      );    // Alt-'m' - down-shoe-stile
    InitPrimProtoFn (UTF16_DOWNTACK            , &PrimProtoFnDownTack_EM_YY           );    // Alt-'n' - down tack
    InitPrimProtoFn (UTF16_CIRCLE              , &PrimProtoFnCircle_EM_YY             );    // Alt-'o' - circle
    InitPrimProtoFn (UTF16_CIRCLE2             , &PrimProtoFnCircle_EM_YY             );    // Circle2
    InitPrimProtoFn (UTF16_PI                  , &PrimProtoFnPi_EM_YY                 );    // Alt-'p' - pi
////                                                                                        // Alt-'q' - ± (plus-or-minus)
    InitPrimProtoFn (UTF16_RHO                 , &PrimProtoFnRho_EM_YY                );    // Alt-'r' - rho
    InitPrimProtoFn (UTF16_UPSTILE             , &PrimProtoFnUpStile_EM_YY            );    // Alt-'s' - up stile
    InitPrimProtoFn (UTF16_TILDE               , &PrimProtoFnTilde_EM_YY              );    // Alt-'t' - tilde
    InitPrimProtoFn (UTF16_TILDE2              , &PrimProtoFnTilde_EM_YY              );    // Tilde2
    InitPrimProtoFn (UTF16_DOWNARROW           , &PrimProtoFnDownArrow_EM_YY          );    // Alt-'u' - down arrow
    InitPrimProtoFn (UTF16_DOWNSHOE            , &PrimProtoFnDownShoe_EM_YY           );    // Alt-'v' - down shoe
                                                                                            // Alt-'w' - omega
    InitPrimProtoFn (UTF16_RIGHTSHOE           , &PrimProtoFnRightShoe_EM_YY          );    // Alt-'x' - right shoe
    InitPrimProtoFn (UTF16_UPARROW             , &PrimProtoFnUpArrow_EM_YY            );    // Alt-'y' - up arrow
    InitPrimProtoFn (UTF16_LEFTSHOE            , &PrimProtoFnLeftShoe_EM_YY           );    // Alt-'z' - left shoe
    InitPrimProtoFn (UTF16_EQUALUNDERBAR       , &PrimProtoFnEqualUnderbar_EM_YY      );    // Alt-'!' - match
////                                                                                        // Alt-'"' - (none)
    InitPrimProtoFn (UTF16_DELSTILE            , &PrimProtoFnDelStile_EM_YY           );    // Alt-'#' - grade-down
    InitPrimProtoFn (UTF16_DELTASTILE          , &PrimProtoFnDeltaStile_EM_YY         );    // Alt-'$' - grade-up
    InitPrimProtoFn (UTF16_CIRCLESTILE         , &PrimProtoFnCircleStile_EM_YY        );    // Alt-'%' - rotate
    InitPrimProtoFn (UTF16_CIRCLESLOPE         , &PrimProtoFnCircleSlope_EM_YY        );    // Alt-'^' - transpose
    InitPrimProtoFn (UTF16_CIRCLEBAR           , &PrimProtoFnCircleBar_EM_YY          );    // Alt-'&' - circle-bar
    InitPrimProtoFn (UTF16_UPTACKJOT           , &PrimProtoFnUpTackJot_EM_YY          );    // Alt-'\''- execute
    InitPrimProtoFn (UTF16_DOWNCARETTILDE      , &PrimProtoFnDownCaretTilde_EM_YY     );    // Alt-'(' - nor
    InitPrimProtoFn (UTF16_NOR                 , &PrimProtoFnDownCaretTilde_EM_YY     );    // Alt-'(' - nor
    InitPrimProtoFn (UTF16_UPCARETTILDE        , &PrimProtoFnUpCaretTilde_EM_YY       );    // Alt-')' - nand
    InitPrimProtoFn (UTF16_NAND                , &PrimProtoFnUpCaretTilde_EM_YY       );    // Alt-')' - nand
    InitPrimProtoFn (UTF16_CIRCLESTAR          , &PrimProtoFnCircleStar_EM_YY         );    // Alt-'*' - log
    InitPrimProtoFn (UTF16_DOMINO              , &PrimProtoFnDomino_EM_YY             );    // Alt-'+' - domino
////                                                                                        // Alt-',' - lamp
    InitPrimProtoFn (UTF16_TIMES               , &PrimProtoFnTimes_EM_YY              );    // Alt-'-' - times
    InitPrimProtoFn (UTF16_SLOPEBAR            , &PrimProtoFnSlopeBar_EM_YY           );    // Alt-'.' - slope-bar as Function
    InitPrimProtoOp (UTF16_OPSLOPEBAR          , &PrimProtoOpSlopeBar_EM_YY           );    // Alt-'.' - ...          Operator
    InitPrimProtoFn (UTF16_SLASHBAR            , &PrimProtoFnSlashBar_EM_YY           );    // Alt-'/' - slash-bar as Function
    InitPrimProtoOp (UTF16_OPSLASHBAR          , &PrimProtoOpSlashBar_EM_YY           );    // Alt-'/' - ...          Operator
    InitPrimProtoFn (UTF16_UPCARET             , &PrimProtoFnUpCaret_EM_YY            );    // Alt-'0' - and (94??)
    InitPrimProtoFn (UTF16_CIRCUMFLEX          , &PrimProtoFnUpCaret_EM_YY            );    //         -
    InitPrimProtoOp (UTF16_DIERESIS            , &PrimProtoOpDieresis_EM_YY           );    // Alt-'1' - dieresis
////                                                                                        // Alt-'2' - overbar
    InitPrimProtoFn (UTF16_LEFTCARET           , &PrimProtoFnLeftCaret_EM_YY          );    // Alt-'3' - less
    InitPrimProtoFn (UTF16_LEFTCARETUNDERBAR   , &PrimProtoFnLeftCaretUnderbar_EM_YY  );    // Alt-'4' - not more
    InitPrimProtoFn (UTF16_LEFTCARETUNDERBAR2  , &PrimProtoFnLeftCaretUnderbar_EM_YY  );    // Not more2
    InitPrimProtoFn (UTF16_EQUAL               , &PrimProtoFnEqual_EM_YY              );    // Alt-'5' - equal
    InitPrimProtoFn (UTF16_RIGHTCARETUNDERBAR  , &PrimProtoFnRightCaretUnderbar_EM_YY );    // Alt-'6' - not less
    InitPrimProtoFn (UTF16_RIGHTCARETUNDERBAR2 , &PrimProtoFnRightCaretUnderbar_EM_YY );    // Not less2
    InitPrimProtoFn (UTF16_RIGHTCARET          , &PrimProtoFnRightCaret_EM_YY         );    // Alt-'7' - more
    InitPrimProtoFn (UTF16_NOTEQUAL            , &PrimProtoFnNotEqual_EM_YY           );    // Alt-'8' - not equal
    InitPrimProtoFn (UTF16_DOWNCARET           , &PrimProtoFnDownCaret_EM_YY          );    // Alt-'9' - or
////                                                                                        // Alt-':' - (none)
    InitPrimProtoFn (UTF16_DOWNTACKJOT         , &PrimProtoFnDownTackJot_EM_YY        );    // Alt-';' - format
////                                                                                        // Alt-'<' - (none)
    InitPrimProtoFn (UTF16_COLONBAR            , &PrimProtoFnColonBar_EM_YY           );    // Alt-'=' - divide
////                                                                                        // Alt-'>' - (none)
    InitPrimProtoOp (UTF16_CIRCLEMIDDLEDOT     , &PrimProtoOpCircleMiddleDot_EM_YY    );    // Alt-'?' - circle-middle-dot
    InitPrimProtoFn (UTF16_NOTEQUALUNDERBAR    , &PrimProtoFnNotEqualUnderbar_EM_YY   );    // Alt-'@' - mismatch
////                                                                                        // Alt-'A' - (none)
////                                                                                        // Alt-'B' - (none)
////                                                                                        // Alt-'C' - (none)
////                                                                                        // Alt-'D' - (none)
    InitPrimProtoFn (UTF16_EPSILONUNDERBAR     , &PrimProtoFnEpsilonUnderbar_EM_YY    );    // Alt-'E' - epsilon-underbar
    InitPrimProtoOp (UTF16_QUADJOT             , &PrimProtoOpQuadJot_EM_YY            );    // Alt-'F' - quad-jot
    InitPrimProtoOp (UTF16_DIERESISDEL         , &PrimProtoOpDieresisDel_EM_YY        );    // Alt-'G' - dieresis-del (dual)
////                                                                                        // Alt-'H' - delta-underbar
    InitPrimProtoFn (UTF16_IOTAUNDERBAR        , &PrimProtoFnIotaUnderbar_EM_YY       );    // Alt-'I' - iota-underbar
    InitPrimProtoOp (UTF16_DIERESISJOT         , &PrimProtoOpDieresisJot_EM_YY        );    // Alt-'J' - dieresis-jot (rank)
////                                                                                        // Alt-'K' - (none)
    InitPrimProtoFn (UTF16_SQUAD               , &PrimProtoFnSquad_EM_YY              );    // Alt-'L' - squad
    InitPrimProtoOp (UTF16_STILETILDE          , &PrimProtoOpStileTilde_EM_YY         );    // Alt-'M' - stile-tilde (partition)
    InitPrimProtoOp (UTF16_DIERESISDOWNTACK    , &PrimProtoOpDieresisDownTack_EM_YY   );    // Alt-'N' - dieresis-downtack (convolution)
    InitPrimProtoOp (UTF16_DIERESISCIRCLE      , &PrimProtoOpDieresisCircle_EM_YY     );    // Alt-'O' - dieresis-circle (composition)
    InitPrimProtoOp (UTF16_DIERESISSTAR        , &PrimProtoOpDieresisStar_EM_YY       );    // Alt-'P' - dieresis-star (power)
////                                                                                        // Alt-'Q' - double-tilde (arbitrary precision)
    InitPrimProtoFn (UTF16_ROOT                , &PrimProtoFnRoot_EM_YY               );    // Alt-'R' - root
    InitPrimProtoFn (UTF16_SECTION             , &PrimProtoFnSection_EM_YY            );    // Alt-'S' - section (multiset symmetric difference)
    InitPrimProtoOp (UTF16_DIERESISTILDE       , &PrimProtoOpDieresisTilde_EM_YY      );    // Alt-'T' - dieresis-tilde (commute/duplicate)
////                                                                                        // Alt-'U' - (none)
////                                                                                        // Alt-'V' - (none)
////                                                                                        // Alt-'W' - (none)
    InitPrimProtoFn (UTF16_RIGHTSHOEUNDERBAR   , &PrimProtoFnRightShoeUnderbar_EM_YY  );    // Alt-'X' - right shoe underbar
////                                                                                        // Alt-'Y' - (none)
    InitPrimProtoFn (UTF16_LEFTSHOEUNDERBAR    , &PrimProtoFnLeftShoeUnderbar_EM_YY   );    // Alt-'Z' - left shoe underbar
////                                                                                        // Alt-'[' - left arrow
    InitPrimProtoFn (UTF16_LEFTTACK            , &PrimProtoFnLeftTack_EM_YY           );    // Alt-'\' - left tack
////                                                                                        // Alt-']' - right arrow
    InitPrimProtoOp (UTF16_VARIANT             , &PrimProtoOpVariant_EM_YY            );    // Alt-'_' - variant
////                                                                                        // Alt-'`' - diamond
////                                                                                        // Alt-'{' - quote-quad
    InitPrimProtoFn (UTF16_RIGHTTACK           , &PrimProtoFnRightTack_EM_YY          );    // Alt-'|' - right tack
////                                                                                        // Alt-'}' - zilde
    InitPrimProtoFn (UTF16_COMMABAR            , &PrimProtoFnCommaBar_EM_YY           );    // Alt-'~' - comma-bar
    InitPrimProtoFn (UTF16_BAR                 , &PrimProtoFnBar_EM_YY                );    //         -
    InitPrimProtoFn (UTF16_BAR2                , &PrimProtoFnBar_EM_YY                );    //         -
    InitPrimProtoFn (UTF16_PLUS                , &PrimProtoFnPlus_EM_YY               );    //         -
    InitPrimProtoFn (UTF16_DOT                 , &PrimProtoFnDotDot_EM_YY             );    //         -
    InitPrimProtoFn (UTF16_QUERY               , &PrimProtoFnQuery_EM_YY              );    //         - question mark
    InitPrimProtoFn (UTF16_STILE               , &PrimProtoFnStile_EM_YY              );    //         -
    InitPrimProtoFn (UTF16_STILE2              , &PrimProtoFnStile_EM_YY              );    //         -
    InitPrimProtoFn (UTF16_COMMA               , &PrimProtoFnComma_EM_YY              );    //         -
    InitPrimProtoFn (UTF16_QUOTEDOT            , &PrimProtoFnQuoteDot_EM_YY           );    //         -
    InitPrimProtoFn (UTF16_STAR                , &PrimProtoFnStar_EM_YY               );    //         -
    InitPrimProtoFn (UTF16_STAR2               , &PrimProtoFnStar_EM_YY               );    //         -
    InitPrimProtoFn (UTF16_SLASH               , &PrimProtoFnSlash_EM_YY              );    //         - slash as Function
    InitPrimProtoOp (UTF16_OPSLASH             , &PrimProtoOpSlash_EM_YY              );    //         - ...      Operator
    InitPrimProtoFn (UTF16_SLOPE               , &PrimProtoFnSlope_EM_YY              );    //         - slope as Function
    InitPrimProtoOp (UTF16_OPSLOPE             , &PrimProtoOpSlope_EM_YY              );    //         - ...      Operator
    InitPrimProtoOp (UTF16_DOT                 , &PrimProtoOpDot_EM_YY                );    //         - dot (inner product)
    InitPrimProtoOp (UTF16_OPJOTDOT            , &PrimProtoOpJotDot_EM_YY             );    //         - jotdot (outer product)

    InitPrimProtoFn (INDEX_DOUBLETILDE_INTER   , &PrimProtoFnDoubleTilde_EM_YY        );    //         - (doubletilde-inter)
    InitPrimProtoFn (INDEX_DOUBLETILDE_UNION   , &PrimProtoFnDoubleTilde_EM_YY        );    //         - (doubletilde-union)
    InitPrimProtoFn (INDEX_DOUBLETILDE_LT      , &PrimProtoFnDoubleTilde_EM_YY        );    //         - (doubletilde-lt)
    InitPrimProtoFn (INDEX_DOUBLETILDE_LE      , &PrimProtoFnDoubleTilde_EM_YY        );    //         - (doubletilde-le)
    InitPrimProtoFn (INDEX_DOUBLETILDE_EQ      , &PrimProtoFnDoubleTilde_EM_YY        );    //         - (doubletilde-eq)
    InitPrimProtoFn (INDEX_DOUBLETILDE_GE      , &PrimProtoFnDoubleTilde_EM_YY        );    //         - (doubletilde-ge)
    InitPrimProtoFn (INDEX_DOUBLETILDE_GT      , &PrimProtoFnDoubleTilde_EM_YY        );    //         - (doubletilde-gt)
    InitPrimProtoFn (INDEX_DOUBLETILDE_NE      , &PrimProtoFnDoubleTilde_EM_YY        );    //         - (doubletilde-ne)
    InitPrimProtoFn (INDEX_DOUBLETILDE_LS      , &PrimProtoFnDoubleTilde_EM_YY        );    //         - (doubletilde-ls)
    InitPrimProtoFn (INDEX_DOUBLETILDE_RS      , &PrimProtoFnDoubleTilde_EM_YY        );    //         - (doubletilde-rs)
    InitPrimProtoFn (INDEX_DOUBLETILDE_JOT     , &PrimProtoFnDoubleTilde_EM_YY        );    //         - (doubletilde-jot)

    InitPrimProtoOp (INDEX_DOUBLETILDE_INTER   , &PrimProtoOpDoubleTilde_EM_YY        );    //         - (doubletilde-inter)
    InitPrimProtoOp (INDEX_DOUBLETILDE_UNION   , &PrimProtoOpDoubleTilde_EM_YY        );    //         - (doubletilde-union)
    InitPrimProtoOp (INDEX_DOUBLETILDE_LT      , &PrimProtoOpDoubleTilde_EM_YY        );    //         - (doubletilde-lt)
    InitPrimProtoOp (INDEX_DOUBLETILDE_LE      , &PrimProtoOpDoubleTilde_EM_YY        );    //         - (doubletilde-le)
    InitPrimProtoOp (INDEX_DOUBLETILDE_EQ      , &PrimProtoOpDoubleTilde_EM_YY        );    //         - (doubletilde-eq)
    InitPrimProtoOp (INDEX_DOUBLETILDE_GE      , &PrimProtoOpDoubleTilde_EM_YY        );    //         - (doubletilde-ge)
    InitPrimProtoOp (INDEX_DOUBLETILDE_GT      , &PrimProtoOpDoubleTilde_EM_YY        );    //         - (doubletilde-gt)
    InitPrimProtoOp (INDEX_DOUBLETILDE_NE      , &PrimProtoOpDoubleTilde_EM_YY        );    //         - (doubletilde-ne)
    InitPrimProtoOp (INDEX_DOUBLETILDE_LS      , &PrimProtoOpDoubleTilde_EM_YY        );    //         - (doubletilde-ls)
    InitPrimProtoOp (INDEX_DOUBLETILDE_RS      , &PrimProtoOpDoubleTilde_EM_YY        );    //         - (doubletilde-rs)
    InitPrimProtoOp (INDEX_DOUBLETILDE_JOT     , &PrimProtoOpDoubleTilde_EM_YY        );    //         - (doubletilde-jot)
} // End InitPrimProtoFns


//***************************************************************************
//  $InitPrimProtoFn
//
//  Initialize a single primitive function prototype
//***************************************************************************

void InitPrimProtoFn
    (WCHAR     wchFn,
     LPPRIMFNS lpPrimFn)

{
    if (PrimProtoFnsTab[FcnTrans (wchFn)] NE NULL)
        DbgStop ();         // We should never get here
    else
        PrimProtoFnsTab[FcnTrans (wchFn)] = lpPrimFn;
} // End InitPrimProtoFn


//***************************************************************************
//  $InitPrimProtoOp
//
//  Initialize a single primitive operator prototype
//***************************************************************************

void InitPrimProtoOp
    (WCHAR     wchFn,
     LPPRIMOPS lpPrimOp)

{
    if (PrimProtoOpsTab[OprTrans (wchFn)] NE NULL)
        DbgStop ();         // We should never get here
    else
        PrimProtoOpsTab[OprTrans (wchFn)] = lpPrimOp;
} // End InitPrimProtoOp


//***************************************************************************
//  $PrimProtoFn_EM
//
//  Default function for symbols for which there is no corresponding
//    primitive function/operator prototype
//***************************************************************************

LPPL_YYSTYPE PrimProtoFn_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnNonceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimProtoFn_EM


//***************************************************************************
//  $PrimProtoOp_EM
//
//  Default function for symbols for which there is no corresponding
//    primitive operator prototype
//***************************************************************************

LPPL_YYSTYPE PrimProtoOp_EM
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to function token
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis)             // Ptr to axis token (always NULL)

{
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    return NULL;
} // End PrimProtoOp_EM


//***************************************************************************
//  $InitPrimSpecs
//
//  Initialize the table of PRIMSPECs
//***************************************************************************

void InitPrimSpecs
    (void)

{
    // Initialize the table of PRIMSPECs
    Init1PrimSpec (UTF16_BAR                 , &PrimSpecBar                );
    Init1PrimSpec (UTF16_BAR2                , &PrimSpecBar                );
    Init1PrimSpec (UTF16_CIRCLE              , &PrimSpecCircle             );
    Init1PrimSpec (UTF16_CIRCLE2             , &PrimSpecCircle             );
    Init1PrimSpec (UTF16_CIRCLESTAR          , &PrimSpecCircleStar         );
    Init1PrimSpec (UTF16_COLONBAR            , &PrimSpecColonBar           );
    Init1PrimSpec (UTF16_DOWNCARET           , &PrimSpecDownCaret          );
    Init1PrimSpec (UTF16_DOWNCARETTILDE      , &PrimSpecDownCaretTilde     );
    Init1PrimSpec (UTF16_DOWNSTILE           , &PrimSpecDownStile          );
    Init1PrimSpec (UTF16_EQUAL               , &PrimSpecEqual              );
    Init1PrimSpec (UTF16_LEFTCARET           , &PrimSpecLeftCaret          );
    Init1PrimSpec (UTF16_LEFTCARETUNDERBAR   , &PrimSpecLeftCaretUnderbar  );
    Init1PrimSpec (UTF16_LEFTCARETUNDERBAR2  , &PrimSpecLeftCaretUnderbar  );
    Init1PrimSpec (UTF16_NAND                , &PrimSpecUpCaretTilde       );
    Init1PrimSpec (INDEX_DOUBLETILDE_INTER   , &PrimSpecDoubleTildeInter   );
    Init1PrimSpec (INDEX_DOUBLETILDE_UNION   , &PrimSpecDoubleTildeUnion   );
    Init1PrimSpec (INDEX_DOUBLETILDE_LT      , &PrimSpecDoubleTildeLt      );
    Init1PrimSpec (INDEX_DOUBLETILDE_LE      , &PrimSpecDoubleTildeLe      );
    Init1PrimSpec (INDEX_DOUBLETILDE_EQ      , &PrimSpecDoubleTildeEq      );
    Init1PrimSpec (INDEX_DOUBLETILDE_GE      , &PrimSpecDoubleTildeGe      );
    Init1PrimSpec (INDEX_DOUBLETILDE_GT      , &PrimSpecDoubleTildeGt      );
    Init1PrimSpec (INDEX_DOUBLETILDE_NE      , &PrimSpecDoubleTildeNe      );
    Init1PrimSpec (INDEX_DOUBLETILDE_LS      , &PrimSpecDoubleTildeLs      );
    Init1PrimSpec (INDEX_DOUBLETILDE_RS      , &PrimSpecDoubleTildeRs      );
    Init1PrimSpec (INDEX_DOUBLETILDE_JOT     , &PrimSpecDoubleTildeJot     );
    Init1PrimSpec (UTF16_NOR                 , &PrimSpecDownCaretTilde     );
    Init1PrimSpec (UTF16_NOTEQUAL            , &PrimSpecNotEqual           );
    Init1PrimSpec (UTF16_PLUS                , &PrimSpecPlus               );
    Init1PrimSpec (UTF16_QUOTEDOT            , &PrimSpecQuoteDot           );
    Init1PrimSpec (UTF16_QUERY               , &PrimSpecQuery              );
    Init1PrimSpec (UTF16_RIGHTCARET          , &PrimSpecRightCaret         );
    Init1PrimSpec (UTF16_RIGHTCARETUNDERBAR  , &PrimSpecRightCaretUnderbar );
    Init1PrimSpec (UTF16_RIGHTCARETUNDERBAR2 , &PrimSpecRightCaretUnderbar );
    Init1PrimSpec (UTF16_ROOT                , &PrimSpecRoot               );
    Init1PrimSpec (UTF16_STAR                , &PrimSpecStar               );
    Init1PrimSpec (UTF16_STAR2               , &PrimSpecStar               );
    Init1PrimSpec (UTF16_STILE               , &PrimSpecStile              );
    Init1PrimSpec (UTF16_STILE2              , &PrimSpecStile              );
    Init1PrimSpec (UTF16_TILDE               , &PrimSpecTilde              );
    Init1PrimSpec (UTF16_TILDE2              , &PrimSpecTilde              );
    Init1PrimSpec (UTF16_TIMES               , &PrimSpecTimes              );
    Init1PrimSpec (UTF16_UPCARET             , &PrimSpecUpCaret            );
    Init1PrimSpec (UTF16_CIRCUMFLEX          , &PrimSpecUpCaret            );
    Init1PrimSpec (UTF16_UPCARETTILDE        , &PrimSpecUpCaretTilde       );
    Init1PrimSpec (UTF16_UPSTILE             , &PrimSpecUpStile            );
} // End InitPrimSpecs


//***************************************************************************
//  $Init1PrimSpec
//
//  Initialize a single lpPrimSpec
//***************************************************************************

void Init1PrimSpec
    (WCHAR      wchFn,
     LPPRIMSPEC lpPrimSpec)

{
    if (PrimSpecTab[FcnTrans (wchFn)] NE NULL)
        DbgStop ();         // We should never get here
    else
        PrimSpecTab[FcnTrans (wchFn)] = lpPrimSpec;
} // End Init1PrimSpec


//***************************************************************************
//  $InitPrimFlags
//
//  Initialize the table of primitive function/operator flags
//***************************************************************************

void InitPrimFlags
    (void)

{
    UINT      uCnt;                 // Loop counter
    PRIMFLAGS PrimFlag,             // Temporary save area
              PrimFlags0 = {0};     // All zero PrimFlags for global R/O use
    UINT      PF_FB,                // Fast Boolean reduction and/or scan
              PF_AN,                // Function is associative on all numbers
              PF_AB,                // ...         associative on Booleans only
              PF_AL,                // ...         alternating
              PF_MS,                // ...         monadic scalar
              PF_DS,                // ...         dyadic  ...
              PF_ID,                // ...      has an identity element
              PF_LI,                // ...          a left identity element
              PF_RI;                // ...            right ...

    //***************************************************************
    // N.B.:  Do NOT mark a function as PF_DS unless it has a simple
    //    Boolean or Floating Point Identity Element, OR it does not
    //    have an Identity Element and a DOMAIN ERROR is the
    //    appropriate response when asked to produce one.
    //***************************************************************

    // Initialize the array
    for (uCnt = 0; uCnt < countof (PrimFlags); uCnt++)
        PrimFlags[uCnt] = PrimFlags0;

    // Get the bits as masks
    PrimFlag.Flags = 0; PrimFlag.bFastBool  = TRUE; PF_FB = PrimFlag.Flags;
    PrimFlag.Flags = 0; PrimFlag.bAssocBool = TRUE; PF_AB = PrimFlag.Flags;
    PrimFlag.Flags = 0; PrimFlag.bAssocNumb = TRUE; PF_AN = PrimFlag.Flags;
    PrimFlag.Flags = 0; PrimFlag.bAlter     = TRUE; PF_AL = PrimFlag.Flags;
    PrimFlag.Flags = 0; PrimFlag.bMonScalar = TRUE; PF_MS = PrimFlag.Flags;
    PrimFlag.Flags = 0; PrimFlag.bDydScalar = TRUE; PF_DS = PrimFlag.Flags;
    PrimFlag.Flags = 0; PrimFlag.bIdentElem = TRUE; PF_ID = PrimFlag.Flags;
    PrimFlag.Flags = 0; PrimFlag.bLftIdent  = TRUE; PF_LI = PrimFlag.Flags;
    PrimFlag.Flags = 0; PrimFlag.bRhtIdent  = TRUE; PF_RI = PrimFlag.Flags;

    // Operators
    Init1PrimFlag (UTF16_CIRCLEMIDDLEDOT     , 0                                                                                          ,             NULL ); // PrimIdentOpCircleMiddleDot_EM_YY  );
    Init1PrimFlag (UTF16_DIERESIS            , 0                                                                                          ,             PrimIdentOpDieresis_EM_YY         );
    Init1PrimFlag (UTF16_DIERESISCIRCLE      , 0                                                                                          ,             NULL ); // PrimIdentOpDieresisCircle_EM_YY   );
    Init1PrimFlag (UTF16_DIERESISDEL         , 0                                                                                          ,             NULL ); // PrimIdentOpDieresisDel_EM_YY      );
    Init1PrimFlag (UTF16_DIERESISDOWNTACK    , 0                                             | PF_ID | PF_LI | PF_RI                      ,             PrimIdentOpDieresisDownTack_EM_YY );
    Init1PrimFlag (UTF16_DIERESISJOT         , 0                                                                                          ,             NULL ); // PrimIdentOpDieresisJot_EM_YY      );
    Init1PrimFlag (UTF16_DIERESISSTAR        , 0                                                                                          ,             NULL ); // PrimIdentOpDieresisStar_EM_YY     );
    Init1PrimFlag (UTF16_DIERESISTILDE       , 0                                                                                          ,             PrimIdentOpDieresisTilde_EM_YY    );
    Init1PrimFlag (UTF16_DOT                 , 0                                                                                          ,             PrimIdentOpDot_EM_YY              );
    Init1PrimFlag (INDEX_DOUBLETILDE_INTER   , 0             | PF_AN                         | PF_ID | PF_LI | PF_RI                      ,             PrimIdentOpDoubleTildeInter_EM_YY );
    Init1PrimFlag (INDEX_DOUBLETILDE_UNION   , 0             | PF_AN                                                                      ,             NULL ); // PrimIdentOpDoubleTildeUnion_EM_YY );
    Init1PrimFlag (INDEX_DOUBLETILDE_LT      , 0                                                                                          ,             NULL ); // PrimIdentOpDoubleTildeLt_EM_YY    );
    Init1PrimFlag (INDEX_DOUBLETILDE_LE      , 0                                                                                          ,             NULL ); // PrimIdentOpDoubleTildeLe_EM_YY    );
    Init1PrimFlag (INDEX_DOUBLETILDE_EQ      , 0                                                                                          ,             NULL ); // PrimIdentOpDoubleTildeEq_EM_YY    );
    Init1PrimFlag (INDEX_DOUBLETILDE_GE      , 0                                                                                          ,             NULL ); // PrimIdentOpDoubleTildeGe_EM_YY    );
    Init1PrimFlag (INDEX_DOUBLETILDE_GT      , 0                                                                                          ,             NULL ); // PrimIdentOpDoubleTildeGt_EM_YY    );
    Init1PrimFlag (INDEX_DOUBLETILDE_NE      , 0                                                                                          ,             NULL ); // PrimIdentOpDoubleTildeNe_EM_YY    );
    Init1PrimFlag (INDEX_DOUBLETILDE_LS      , 0                                             | PF_ID         | PF_RI                      ,             PrimIdentOpDoubleTildeLsRs_EM_YY  );
    Init1PrimFlag (INDEX_DOUBLETILDE_RS      , 0                                             | PF_ID | PF_LI                              ,             PrimIdentOpDoubleTildeLsRs_EM_YY  );
    Init1PrimFlag (INDEX_DOUBLETILDE_JOT     , 0                                                                                          ,             NULL ); // PrimIdentOpDoubleTildeJot_EM_YY   );
    Init1PrimFlag (UTF16_DOWNSHOESTILE       , 0                                                                                          ,             NULL ); // PrimIdentOpDownShoeStile_EM_YY    );
    Init1PrimFlag (UTF16_JOT                 , 0                                                                                          ,             PrimIdentOpJot_EM_YY              );
    Init1PrimFlag (UTF16_OPJOTDOT            , 0                                                                                          ,             PrimIdentOpJotDot_EM_YY           );
    Init1PrimFlag (UTF16_OPSLASH             , 0                                             | PF_ID | PF_LI                              ,             PrimIdentOpSlash_EM_YY            );
    Init1PrimFlag (UTF16_QUADJOT             , 0                                                                                          ,             NULL ); // PrimIdentOpQuadJot_EM_YY  );
    Init1PrimFlag (UTF16_STILETILDE          , 0                                                                                          ,             NULL ); // PrimIdentOpStileTilde_EM_YY       );
    Init1PrimFlag (UTF16_VARIANT             , 0                                                                                          ,             PrimIdentOpVariant_EM_YY          );


    // Functions
    Init1PrimFlag (UTF16_BAR                 , 0                     | PF_AL | PF_MS | PF_DS | PF_ID |         PF_RI | PF_INDEX_MINUS     , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_BAR2                , 0                     | PF_AL | PF_MS | PF_DS | PF_ID |         PF_RI | PF_INDEX_MINUS     , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_CIRCLE              , 0                             | PF_MS | PF_DS                                              , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_CIRCLE2             , 0                             | PF_MS | PF_DS                                              , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_CIRCLEBAR           , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnCircleStile_EM_YY      );
    Init1PrimFlag (UTF16_CIRCLESLOPE         , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnCircleSlope_EM_YY      );
    Init1PrimFlag (UTF16_CIRCLESTILE         , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnCircleStile_EM_YY      );
    Init1PrimFlag (UTF16_CIRCLESTAR          , 0                             | PF_MS | PF_DS                                              , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_COLONBAR            , 0                     | PF_AL | PF_MS | PF_DS | PF_ID         | PF_RI | PF_INDEX_DIVIDE    , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_COMMA               , 0                                             | PF_ID | PF_LI | PF_RI                      , (LPPRIMOPS) PrimIdentFnComma_EM_YY            );
    Init1PrimFlag (UTF16_COMMABAR            , 0                                             | PF_ID | PF_LI | PF_RI                      , (LPPRIMOPS) PrimIdentFnComma_EM_YY            );
    Init1PrimFlag (UTF16_DOMINO              , 0                                             | PF_ID         | PF_RI                      , (LPPRIMOPS) PrimIdentFnDomino_EM_YY           );
    Init1PrimFlag (UTF16_DOWNARROW           , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnDownArrow_EM_YY        );
    Init1PrimFlag (UTF16_DOWNCARET           , PF_FB | PF_AB                         | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_OR        , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_DOWNCARETTILDE      , PF_FB                                 | PF_DS                         | PF_INDEX_NOR       , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_DOWNSHOE            , 0                                             | PF_ID | PF_LI | PF_RI                      , (LPPRIMOPS) PrimIdentFnDownShoe_EM_YY         );
    Init1PrimFlag (UTF16_DOWNSTILE           , PF_FB | PF_AB | PF_AN         | PF_MS | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_MIN       , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_DOWNTACK            , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnDownTack_EM_YY         );
    Init1PrimFlag (UTF16_EQUAL               , PF_FB | PF_AB                         | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_EQUAL     , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_LEFTCARET           , PF_FB                                 | PF_DS | PF_ID | PF_LI         | PF_INDEX_LESS      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_LEFTCARETUNDERBAR   , PF_FB                                 | PF_DS | PF_ID | PF_LI         | PF_INDEX_LESSEQ    , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_LEFTCARETUNDERBAR2  , PF_FB                                 | PF_DS | PF_ID | PF_LI         | PF_INDEX_LESSEQ    , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_LEFTTACK            , 0                                             | PF_ID |         PF_RI                      , (LPPRIMOPS) PrimIdentFnLeftTack_EM_YY         );
    Init1PrimFlag (UTF16_NAND                , PF_FB                                 | PF_DS                         | PF_INDEX_NAND      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_NOR                 , PF_FB                                 | PF_DS                         | PF_INDEX_NOR       , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_NOTEQUAL            , PF_FB | PF_AB                         | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_NOTEQUAL  , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_PLUS                , 0     | PF_AB | PF_AN         | PF_MS | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_PLUS      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_QUOTEDOT            , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_LI         | PF_INDEX_LESSEQ    , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_QUERY               , 0                             | PF_MS                                                      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_RHO                 , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnRho_EM_YY              );
    Init1PrimFlag (UTF16_RIGHTCARET          , PF_FB                                 | PF_DS | PF_ID         | PF_RI | PF_INDEX_MORE      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_RIGHTCARETUNDERBAR  , PF_FB                                 | PF_DS | PF_ID         | PF_RI | PF_INDEX_MOREEQ    , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_RIGHTCARETUNDERBAR2 , PF_FB                                 | PF_DS | PF_ID         | PF_RI | PF_INDEX_MOREEQ    , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_RIGHTSHOE           , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnRightShoe_EM_YY        );
    Init1PrimFlag (UTF16_RIGHTTACK           , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnRightTack_EM_YY        );
    Init1PrimFlag (UTF16_ROOT                , 0                             | PF_MS | PF_DS | PF_ID | PF_LI         | PF_INDEX_ROOT      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_SECTION             , 0                                             | PF_ID         | PF_RI                      , (LPPRIMOPS) PrimIdentFnSection_EM_YY          );
    Init1PrimFlag (UTF16_SLASH               , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnSlash_EM_YY            );
    Init1PrimFlag (UTF16_SLASHBAR            , 0                                                                                          , (LPPRIMOPS) PrimIdentFnSlash_EM_YY            );
    Init1PrimFlag (UTF16_SLOPE               , 0                                                                                          ,             NULL ); // PrimIdentFnSlope_EM_YY            );
    Init1PrimFlag (UTF16_SLOPEBAR            , 0                                                                                          ,             NULL ); // PrimIdentFnSlope_EM_YY            );
    Init1PrimFlag (UTF16_SQUAD               , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnSquad_EM_YY            );
    Init1PrimFlag (UTF16_STAR                , PF_FB                         | PF_MS | PF_DS | PF_ID         | PF_RI | PF_INDEX_MOREEQ    , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_STAR2               , PF_FB                         | PF_MS | PF_DS | PF_ID         | PF_RI | PF_INDEX_MOREEQ    , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_STILE               , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_LI         | PF_INDEX_LESS      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_STILE2              , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_LI         | PF_INDEX_LESS      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_TILDE               , 0                             | PF_MS         | PF_ID         | PF_RI                      , (LPPRIMOPS) PrimIdentFnTilde_EM_YY            );
    Init1PrimFlag (UTF16_TILDE2              , 0                             | PF_MS         | PF_ID         | PF_RI                      , (LPPRIMOPS) PrimIdentFnTilde_EM_YY            );
    Init1PrimFlag (UTF16_TIMES               , PF_FB | PF_AB | PF_AN         | PF_MS | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_AND       , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_UPARROW             , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnUpArrow_EM_YY          );
    Init1PrimFlag (UTF16_UPCARET             , PF_FB | PF_AB                         | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_AND       , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_CIRCUMFLEX          , PF_FB | PF_AB                         | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_AND       , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_UPCARETTILDE        , PF_FB                                 | PF_DS                         | PF_INDEX_NAND      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_UPSTILE             , PF_FB | PF_AB | PF_AN         | PF_MS | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_MAX       , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
} // End InitPrimFlags


//***************************************************************************
//  $Init1PrimFlag
//
//  Initialize a single primitive flag
//***************************************************************************

void Init1PrimFlag
    (WCHAR     wchFn,
     UINT      uFlag,
     LPPRIMOPS lpPrimOps)

{
    PrimFlags[FcnTrans (wchFn)].Flags |= uFlag;
    if (PrimFlags[FcnTrans (wchFn)].lpPrimOps NE NULL)
        DbgStop ();         // We should never get here
    else
        PrimFlags[FcnTrans (wchFn)].lpPrimOps = lpPrimOps;
} // End Init1PrimFlag


//***************************************************************************
//  $InitIdentityElements
//
//  Initialize the table of primitive scalar functions with simple
//    scalar identity elements
//***************************************************************************

void InitIdentityElements
    (void)

{
    Init1IdentityElement (PF_INDEX_PLUS    , 0.0);
    Init1IdentityElement (PF_INDEX_MINUS   , 0.0);
    Init1IdentityElement (PF_INDEX_DIVIDE  , 1.0);

    Init1IdentityElement (PF_INDEX_MIN     , fltPosInfinity);
    Init1IdentityElement (PF_INDEX_MAX     , fltNegInfinity);

    Init1IdentityElement (PF_INDEX_AND     , 1.0);
    Init1IdentityElement (PF_INDEX_OR      , 0.0);

    Init1IdentityElement (PF_INDEX_LESS    , 0.0);
    Init1IdentityElement (PF_INDEX_LESSEQ  , 1.0);
    Init1IdentityElement (PF_INDEX_EQUAL   , 1.0);
    Init1IdentityElement (PF_INDEX_MOREEQ  , 1.0);
    Init1IdentityElement (PF_INDEX_MORE    , 0.0);

    Init1IdentityElement (PF_INDEX_ROOT    , 1.0);

    Init1IdentityElement (PF_INDEX_NOTEQUAL, 0.0);
} // End InitIdentityElements


//***************************************************************************
//  $Init1IdentityElement
//
//  Initialize an identity element
//***************************************************************************

void Init1IdentityElement
    (UINT     uIndex,
     APLFLOAT aplFloat)

{
    // If the identity element is Boolean, ...
    PrimIdent[uIndex].IsBool     = (aplFloat EQ 0.0) || (aplFloat EQ 1.0);
    PrimIdent[uIndex].bIdentElem = (aplFloat EQ 1.0);

    // Save if it's float
    PrimIdent[uIndex].fIdentElem = aplFloat;
} // End Init1IdentityElement


//***************************************************************************
//  $InitPrimeTabs
//
//  Initialize prime # tables
//***************************************************************************

void InitPrimeTabs
    (void)

{
    UINT uCnt;

    // Convert the chars in NthPowerCharTab to
    //   APLMPIs in NthPowerTab
    for (uCnt = 0; uCnt < NthPowerCnt; uCnt++)
        mpz_init_set_str (&NthPowerTab[uCnt].aplMPI, NthPowerTab[uCnt].lpChar, 10);

    // Initialize the Nth1e0PrimeStr values to the last entry in the Nth1e0PrimeTab
    Nth1e0PrimeStr.aplNthVal = Nth1e0PrimeTab[Nth1e0PrimeCnt - 1];
    Nth1e0PrimeStr.aplNthCnt = (Nth1e0PrimeCnt - 1) * Nth1e0PrimeInc;

    // Initialize the Nth1e2PrimeStr values to the last entry in the Nth1e2PrimeTab
    Nth1e2PrimeStr.aplNthVal = Nth1e2PrimeTab[Nth1e2PrimeCnt - 1];
    Nth1e2PrimeStr.aplNthCnt = (Nth1e2PrimeCnt - 1) * Nth1e2PrimeInc;

    // Initialize the Nth1e5PrimeStr values to the last entry in the Nth1e5PrimeTab
    Nth1e5PrimeStr.aplNthVal = Nth1e5PrimeTab[Nth1e5PrimeCnt - 1];
    Nth1e5PrimeStr.aplNthCnt = (Nth1e5PrimeCnt - 1) * Nth1e5PrimeInc;
} // End InitPrimeTabs


//***************************************************************************
//  End of File: initdata.c
//***************************************************************************
