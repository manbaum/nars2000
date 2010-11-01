//***************************************************************************
//  NARS2000 -- Initialization of Data
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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
#include "headers.h"
#include <float.h>
#include <math.h>


extern PRIMSPEC PrimSpecBar;
extern PRIMSPEC PrimSpecCircle;
extern PRIMSPEC PrimSpecCircleStar;
extern PRIMSPEC PrimSpecColonBar;
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
extern PRIMSPEC PrimSpecStar;
extern PRIMSPEC PrimSpecStile;
extern PRIMSPEC PrimSpecTilde;
extern PRIMSPEC PrimSpecTimes;
extern PRIMSPEC PrimSpecUpCaret;
extern PRIMSPEC PrimSpecUpCaretTilde;
extern PRIMSPEC PrimSpecUpStile;


// Symbols for which there is no definition            Monadic          Dyadic
#define PrimFnAlpha_EM_YY           PrimFn_EM       // ERROR            ERROR
#define PrimFnOmega_EM_YY           PrimFn_EM       // ERROR            ERROR
#define PrimFnUpShoe_EM_YY          PrimFn_EM       // ERROR            ERROR


// Monadic Operators TO DO                             Monadic          Dyadic
/////// PrimOpStileTilde_EM_YY                      // ERROR            Partition (*)

// Dyadic  Operators TO DO                             Monadic          Dyadic
/////// PrimOpDieresisDel_EM_YY                     // ERROR            Dual (*)
/////// PrimOpDieresisDowntack_EM_YY                // ERROR            Convolution (*)
/////// PrimOpDieresisStar_EM_YY                    // ERROR            Power (*)

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
    InitIdentityFunctions ();   // ...        identity functions
} // End InitPrimTabs


//***************************************************************************
//  $InitConstants
//
//  Initialize various constants
//***************************************************************************

void InitConstants
    (void)

{
    APLINT aplInteger;          // Temporary value

#define POS_INFINITY            (0x7FF0000000000000)
#define NEG_INFINITY            (0xFFF0000000000000)
#define QUIET_NAN               (0xFFF8000000000000)
#define FLOAT2POW53             (0x4340000000000000)
#define FLOATPI                 (0x400921FB54442D18)
#define FLOATE                  (0x4005BF0A8B145769)

    // Create various floating point constants
    aplInteger = POS_INFINITY; PosInfinity = *(LPAPLFLOAT) &aplInteger;
                               __infinity  = PosInfinity;
    aplInteger = NEG_INFINITY; NegInfinity = *(LPAPLFLOAT) &aplInteger;
    aplInteger = FLOAT2POW53;  Float2Pow53 = *(LPAPLFLOAT) &aplInteger;
    aplInteger = FLOATPI;      FloatPi     = *(LPAPLFLOAT) &aplInteger;
    aplInteger = FLOATE;       FloatE      = *(LPAPLFLOAT) &aplInteger;

#undef  FLOATE
#undef  FLOATPI
#undef  FLOAT2POW53
#undef  QUIET_NAN
#undef  NEG_INFINITY
#undef  POS_INFINITY
} // End InitConstants


//***************************************************************************
//  $InitPrimFns
//
//  Initialize the primitive function and operator jump table
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- InitPrimFns"
#else
#define APPEND_NAME
#endif

void InitPrimFns
    (void)

{
    //***************************************************************
    //  Primitive Functions
    //  Primitive operators are handled case-by-case in
    //    ExecOp1_EM_YY and ExecOp2_EM_YY.
    //***************************************************************
    InitPrimFn (UTF16_ALPHA             , &PrimFnAlpha_EM_YY             ); // Alt-'a' - alpha
    InitPrimFn (UTF16_UPTACK            , &PrimFnUpTack_EM_YY            ); // Alt-'b' - up tack
    InitPrimFn (UTF16_UPSHOE            , &PrimFnUpShoe_EM_YY            ); // Alt-'c' - up shoe
    InitPrimFn (UTF16_DOWNSTILE         , &PrimFnDownStile_EM_YY         ); // Alt-'d' - down stile
    InitPrimFn (UTF16_EPSILON           , &PrimFnEpsilon_EM_YY           ); // Alt-'e' - epsilon
////                                                                        // Alt-'f' - infinity
////                                                                        // Alt-'g' - del
////                                                                        // Alt-'h' - delta
    InitPrimFn (UTF16_IOTA              , &PrimFnIota_EM_YY              ); // Alt-'i' - iota
    InitPrimFn (UTF16_JOT               , (LPPRIMFNS) -1                 ); // Alt-'j' - jot
////                                                                        // Alt-'k' - single quote
////                                                                        // Alt-'l' - quad
    InitPrimFn (UTF16_STILE             , &PrimFnStile_EM_YY             ); // Alt-'m' - stile
    InitPrimFn (UTF16_DOWNTACK          , &PrimFnDownTack_EM_YY          ); // Alt-'n' - down tack
    InitPrimFn (UTF16_CIRCLE            , &PrimFnCircle_EM_YY            ); // Alt-'o' - circle
    InitPrimFn (UTF16_STAR              , &PrimFnStar_EM_YY              ); // Alt-'p' - star
    InitPrimFn (UTF16_QUERY             , &PrimFnQuery_EM_YY             ); // Alt-'q' - question mark
    InitPrimFn (UTF16_RHO               , &PrimFnRho_EM_YY               ); // Alt-'r' - rho
    InitPrimFn (UTF16_UPSTILE           , &PrimFnUpStile_EM_YY           ); // Alt-'s' - up stile
    InitPrimFn (UTF16_TILDE             , &PrimFnTilde_EM_YY             ); // Alt-'t' - tilde
    InitPrimFn (UTF16_DOWNARROW         , &PrimFnDownArrow_EM_YY         ); // Alt-'u' - down arrow
    InitPrimFn (UTF16_DOWNSHOE          , &PrimFnDownShoe_EM_YY          ); // Alt-'v' - down shoe
    InitPrimFn (UTF16_OMEGA             , &PrimFnOmega_EM_YY             ); // Alt-'w' - omega
    InitPrimFn (UTF16_RIGHTSHOE         , &PrimFnRightShoe_EM_YY         ); // Alt-'x' - right shoe
    InitPrimFn (UTF16_UPARROW           , &PrimFnUpArrow_EM_YY           ); // Alt-'y' - up arrow
    InitPrimFn (UTF16_LEFTSHOE          , &PrimFnLeftShoe_EM_YY          ); // Alt-'z' - left shoe
    InitPrimFn (UTF16_EQUALUNDERBAR     , &PrimFnEqualUnderbar_EM_YY     ); // Alt-'!' - match
////                                                                        // Alt-'"' - (none)
    InitPrimFn (UTF16_DELSTILE          , &PrimFnDelStile_EM_YY          ); // Alt-'#' - grade-down
    InitPrimFn (UTF16_DELTASTILE        , &PrimFnDeltaStile_EM_YY        ); // Alt-'$' - grade-up
    InitPrimFn (UTF16_CIRCLESTILE       , &PrimFnCircleStile_EM_YY       ); // Alt-'%' - rotate
    InitPrimFn (UTF16_CIRCLESLOPE       , &PrimFnCircleSlope_EM_YY       ); // Alt-'^' - transpose
    InitPrimFn (UTF16_CIRCLEBAR         , &PrimFnCircleBar_EM_YY         ); // Alt-'&' - circle-bar
    InitPrimFn (UTF16_UPTACKJOT         , &PrimFnUpTackJot_EM_YY         ); // Alt-'\''- execute
    InitPrimFn (UTF16_DOWNCARETTILDE    , &PrimFnDownCaretTilde_EM_YY    ); // Alt-'(' - nor
    InitPrimFn (UTF16_UPCARETTILDE      , &PrimFnUpCaretTilde_EM_YY      ); // Alt-')' - nand
    InitPrimFn (UTF16_CIRCLESTAR        , &PrimFnCircleStar_EM_YY        ); // Alt-'*' - log
    InitPrimFn (UTF16_DOMINO            , &PrimFnDomino_EM_YY            ); // Alt-'+' - domino
////                                                                        // Alt-',' - lamp
    InitPrimFn (UTF16_TIMES             , &PrimFnTimes_EM_YY             ); // Alt-'-' - times
    InitPrimFn (UTF16_SLOPEBAR          , &PrimFnSlopeBar_EM_YY          ); // Alt-'.' - slope-bar
    InitPrimFn (UTF16_SLASHBAR          , &PrimFnSlashBar_EM_YY          ); // Alt-'/' - slash-bar
    InitPrimFn (UTF16_UPCARET           , &PrimFnUpCaret_EM_YY           ); // Alt-'0' - and (94??)
    InitPrimFn (UTF16_DIERESIS          , (LPPRIMFNS) -1                 ); // Alt-'1' - dieresis
////                                                                        // Alt-'2' - overbar
    InitPrimFn (UTF16_LEFTCARET         , &PrimFnLeftCaret_EM_YY         ); // Alt-'3' - less
    InitPrimFn (UTF16_LEFTCARETUNDERBAR , &PrimFnLeftCaretUnderbar_EM_YY ); // Alt-'4' - not more
    InitPrimFn (UTF16_EQUAL             , &PrimFnEqual_EM_YY             ); // Alt-'5' - equal
    InitPrimFn (UTF16_RIGHTCARETUNDERBAR, &PrimFnRightCaretUnderbar_EM_YY); // Alt-'6' - not less
    InitPrimFn (UTF16_RIGHTCARET        , &PrimFnRightCaret_EM_YY        ); // Alt-'7' - more
    InitPrimFn (UTF16_NOTEQUAL          , &PrimFnNotEqual_EM_YY          ); // Alt-'8' - not equal
    InitPrimFn (UTF16_DOWNCARET         , &PrimFnDownCaret_EM_YY         ); // Alt-'9' - or
////                                                                        // Alt-':' - (none)
    InitPrimFn (UTF16_DOWNTACKJOT       , &PrimFnDownTackJot_EM_YY       ); // Alt-';' - format
////                                                                        // Alt-'<' - (none)
    InitPrimFn (UTF16_COLONBAR          , &PrimFnColonBar_EM_YY          ); // Alt-'=' - divide
////                                                                        // Alt-'>' - (none)
    InitPrimFn (UTF16_CIRCLEMIDDLEDOT   , (LPPRIMFNS) -1                 ); // Alt-'?' - circle-middle-dot
    InitPrimFn (UTF16_NOTEQUALUNDERBAR  , &PrimFnNotEqualUnderbar_EM_YY  ); // Alt-'@' - mismatch
////                                                                        // Alt-'A' - (none)
////                                                                        // Alt-'B' - (none)
////                                                                        // Alt-'C' - (none)
////                                                                        // Alt-'D' - (none)
    InitPrimFn (UTF16_EPSILONUNDERBAR   , &PrimFnEpsilonUnderbar_EM_YY   ); // Alt-'E' - epsilon-underbar
////                                                                        // Alt-'F' - (none)
    InitPrimFn (UTF16_DIERESISDEL       , (LPPRIMFNS) -1                 ); // Alt-'G' - dieresis-del (dual)
////                                                                        // Alt-'H' - delta-underbar
    InitPrimFn (UTF16_IOTAUNDERBAR      , &PrimFnIotaUnderbar_EM_YY      ); // Alt-'I' - iota-underbar
    InitPrimFn (UTF16_DIERESISJOT       , (LPPRIMFNS) -1                 ); // Alt-'J' - dieresis-jot (rank)
////                                                                        // Alt-'K' - (none)
    InitPrimFn (UTF16_SQUAD             , &PrimFnSquad_EM_YY             ); // Alt-'L' - squad
    InitPrimFn (UTF16_STILETILDE        , (LPPRIMFNS) -1                 ); // Alt-'M' - stile-tilde (partition)
    InitPrimFn (UTF16_DIERESISDOWNTACK  , (LPPRIMFNS) -1                 ); // Alt-'N' - dieresis-downtack (convolution)
    InitPrimFn (UTF16_DIERESISCIRCLE    , (LPPRIMFNS) -1                 ); // Alt-'O' - dieresis-circle (compose)
    InitPrimFn (UTF16_DIERESISSTAR      , (LPPRIMFNS) -1                 ); // Alt-'P' - dieresis-star (power)
////                                                                        // Alt-'Q' - (none)
////                                                                        // Alt-'R' - (none)
////                                                                        // Alt-'S' - (none)
    InitPrimFn (UTF16_DIERESISTILDE     , (LPPRIMFNS) -1                 ); // Alt-'T' - dieresis-tilde (commute/duplicate)
////                                                                        // Alt-'U' - (none)
////                                                                        // Alt-'V' - (none)
////                                                                        // Alt-'W' - (none)
////                                                                        // Alt-'X' - (none)
////                                                                        // Alt-'Y' - (none)
////                                                                        // Alt-'Z' - (none)
////                                                                        // Alt-'[' - left arrow
    InitPrimFn (UTF16_LEFTTACK          , &PrimFnLeftTack_EM_YY          ); // Alt-'\' - left tack
////                                                                        // Alt-']' - right arrow
    InitPrimFn (UTF16_QUOTEDOT          , &PrimFnQuoteDot_EM_YY          ); // Alt-'_' - quote-dot
////                                                                        // Alt-'`' - diamond
////                                                                        // Alt-'{' - quote-quad
    InitPrimFn (UTF16_RIGHTTACK         , &PrimFnRightTack_EM_YY         ); // Alt-'|' - right tack
////                                                                        // Alt-'}' - zilde
    InitPrimFn (UTF16_COMMABAR          , &PrimFnCommaBar_EM_YY          ); // Alt-'~' - comma-bar
    InitPrimFn (UTF16_CIRCUMFLEX        , &PrimFnUpCaret_EM_YY           );
    InitPrimFn (UTF16_BAR               , &PrimFnBar_EM_YY               );
    InitPrimFn (UTF16_PLUS              , &PrimFnPlus_EM_YY              );
    InitPrimFn (UTF16_STILE2            , &PrimFnStile_EM_YY             );
    InitPrimFn (UTF16_TILDE2            , &PrimFnTilde_EM_YY             );
    InitPrimFn (UTF16_COMMA             , &PrimFnComma_EM_YY             );
    InitPrimFn (UTF16_SLASH             , &PrimFnSlash_EM_YY             );
    InitPrimFn (UTF16_SLOPE             , &PrimFnSlope_EM_YY             );
    InitPrimFn (UTF16_DOT               , (LPPRIMFNS) -1                 ); //         - dot (inner product)
    InitPrimFn (INDEX_JOTDOT            , (LPPRIMFNS) -1                 ); //         - jotdot (outer product)
} // End InitPrimFns
#undef  APPEND_NAME


//***************************************************************************
//  $InitPrimFn
//
//  Initialize a single primitive function/operator
//***************************************************************************

void InitPrimFn
    (WCHAR     wchFn,
     LPPRIMFNS lpPrimFn)

{
    if (PrimFnsTab[PRIMTAB_MASK & wchFn])
        DbgStop ();
    else
        PrimFnsTab[PRIMTAB_MASK & wchFn] = lpPrimFn;
} // End InitPrimFn


//***************************************************************************
//  $PrimFn_EM
//
//  Default function for symbols for which there is no corresponding
//    primitive function/operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFn_EM"
#else
#define APPEND_NAME
#endif

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
#undef  APPEND_NAME

// Symbols for which there is no definition as a primitive
#define PrimProtoFnAlpha_EM_YY                  PrimProtoFn_EM
#define PrimProtoFnOmega_EM_YY                  PrimProtoFn_EM
#define PrimProtoFnUpShoe_EM_YY                 PrimProtoFn_EM


// Monadic operators TO DO
#define PrimProtoOpStileTilde_EM_YY             PrimProtoOp_EM

// Dyadic operators TO DO
#define PrimProtoOpDieresisDel_EM_YY            PrimProtoOp_EM
#define PrimProtoOpDieresisDownTack_EM_YY       PrimProtoOp_EM
#define PrimProtoOpDieresisStar_EM_YY           PrimProtoOp_EM


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

#ifdef DEBUG
#define APPEND_NAME     L" -- InitPrimProtoFns"
#else
#define APPEND_NAME
#endif

void InitPrimProtoFns
    (void)

{
    //***************************************************************
    //  Primitive Functions & operators
    //***************************************************************

    InitPrimProtoFn (UTF16_ALPHA             , &PrimProtoFnAlpha_EM_YY             );   // Alt-'a' - alpha
    InitPrimProtoFn (UTF16_UPTACK            , &PrimProtoFnUpTack_EM_YY            );   // Alt-'b' - up tack
    InitPrimProtoFn (UTF16_UPSHOE            , &PrimProtoFnUpShoe_EM_YY            );   // Alt-'c' - up shoe
    InitPrimProtoFn (UTF16_DOWNSTILE         , &PrimProtoFnDownStile_EM_YY         );   // Alt-'d' - down stile
    InitPrimProtoFn (UTF16_EPSILON           , &PrimProtoFnEpsilon_EM_YY           );   // Alt-'e' - epsilon
////                                                                                    // Alt-'f' - infinity
////                                                                                    // Alt-'g' - del
////                                                                                    // Alt-'h' - delta
    InitPrimProtoFn (UTF16_IOTA              , &PrimProtoFnIota_EM_YY              );   // Alt-'i' - iota
    InitPrimProtoOp (UTF16_JOT               , &PrimProtoOpJot_EM_YY               );   // Alt-'j' - jot
////                                                                                    // Alt-'k' - single quote
////                                                                                    // Alt-'l' - quad
    InitPrimProtoFn (UTF16_STILE             , &PrimProtoFnStile_EM_YY             );   // Alt-'m' - stile
    InitPrimProtoFn (UTF16_DOWNTACK          , &PrimProtoFnDownTack_EM_YY          );   // Alt-'n' - down tack
    InitPrimProtoFn (UTF16_CIRCLE            , &PrimProtoFnCircle_EM_YY            );   // Alt-'o' - circle
    InitPrimProtoFn (UTF16_STAR              , &PrimProtoFnStar_EM_YY              );   // Alt-'p' - star
    InitPrimProtoFn (UTF16_QUERY             , &PrimProtoFnQuery_EM_YY             );   // Alt-'q' - question mark
    InitPrimProtoFn (UTF16_RHO               , &PrimProtoFnRho_EM_YY               );   // Alt-'r' - rho
    InitPrimProtoFn (UTF16_UPSTILE           , &PrimProtoFnUpStile_EM_YY           );   // Alt-'s' - up stile
    InitPrimProtoFn (UTF16_TILDE             , &PrimProtoFnTilde_EM_YY             );   // Alt-'t' - tilde
    InitPrimProtoFn (UTF16_DOWNARROW         , &PrimProtoFnDownArrow_EM_YY         );   // Alt-'u' - down arrow
    InitPrimProtoFn (UTF16_DOWNSHOE          , &PrimProtoFnDownShoe_EM_YY          );   // Alt-'v' - down shoe
    InitPrimProtoFn (UTF16_OMEGA             , &PrimProtoFnOmega_EM_YY             );   // Alt-'w' - omega
    InitPrimProtoFn (UTF16_RIGHTSHOE         , &PrimProtoFnRightShoe_EM_YY         );   // Alt-'x' - right shoe
    InitPrimProtoFn (UTF16_UPARROW           , &PrimProtoFnUpArrow_EM_YY           );   // Alt-'y' - up arrow
    InitPrimProtoFn (UTF16_LEFTSHOE          , &PrimProtoFnLeftShoe_EM_YY          );   // Alt-'z' - left shoe
    InitPrimProtoFn (UTF16_EQUALUNDERBAR     , &PrimProtoFnEqualUnderbar_EM_YY     );   // Alt-'!' - match
////                                                                                    // Alt-'"' - (none)
    InitPrimProtoFn (UTF16_DELSTILE          , &PrimProtoFnDelStile_EM_YY          );   // Alt-'#' - grade-down
    InitPrimProtoFn (UTF16_DELTASTILE        , &PrimProtoFnDeltaStile_EM_YY        );   // Alt-'$' - grade-up
    InitPrimProtoFn (UTF16_CIRCLESTILE       , &PrimProtoFnCircleStile_EM_YY       );   // Alt-'%' - rotate
    InitPrimProtoFn (UTF16_CIRCLESLOPE       , &PrimProtoFnCircleSlope_EM_YY       );   // Alt-'^' - transpose
    InitPrimProtoFn (UTF16_CIRCLEBAR         , &PrimProtoFnCircleBar_EM_YY         );   // Alt-'&' - circle-bar
    InitPrimProtoFn (UTF16_UPTACKJOT         , &PrimProtoFnUpTackJot_EM_YY         );   // Alt-'\''- execute
    InitPrimProtoFn (UTF16_DOWNCARETTILDE    , &PrimProtoFnDownCaretTilde_EM_YY    );   // Alt-'(' - nor
    InitPrimProtoFn (UTF16_UPCARETTILDE      , &PrimProtoFnUpCaretTilde_EM_YY      );   // Alt-')' - nand
    InitPrimProtoFn (UTF16_CIRCLESTAR        , &PrimProtoFnCircleStar_EM_YY        );   // Alt-'*' - log
    InitPrimProtoFn (UTF16_DOMINO            , &PrimProtoFnDomino_EM_YY            );   // Alt-'+' - domino
////                                                                                    // Alt-',' - lamp
    InitPrimProtoFn (UTF16_TIMES             , &PrimProtoFnTimes_EM_YY             );   // Alt-'-' - times
    InitPrimProtoFn (UTF16_SLOPEBAR          , &PrimProtoFnSlopeBar_EM_YY          );   // Alt-'.' - slope-bar as Function
    InitPrimProtoOp (INDEX_OPSLOPEBAR        , &PrimProtoOpSlopeBar_EM_YY          );   // Alt-'.' - ...          Operator
    InitPrimProtoFn (UTF16_SLASHBAR          , &PrimProtoFnSlashBar_EM_YY          );   // Alt-'/' - slash-bar as Function
    InitPrimProtoOp (INDEX_OPSLASHBAR        , &PrimProtoOpSlashBar_EM_YY          );   // Alt-'/' - ...          Operator
    InitPrimProtoFn (UTF16_UPCARET           , &PrimProtoFnUpCaret_EM_YY           );   // Alt-'0' - and (94??)
    InitPrimProtoOp (UTF16_DIERESIS          , &PrimProtoOpDieresis_EM_YY          );   // Alt-'1' - dieresis
////                                                                                    // Alt-'2' - overbar
    InitPrimProtoFn (UTF16_LEFTCARET         , &PrimProtoFnLeftCaret_EM_YY         );   // Alt-'3' - less
    InitPrimProtoFn (UTF16_LEFTCARETUNDERBAR , &PrimProtoFnLeftCaretUnderbar_EM_YY );   // Alt-'4' - not more
    InitPrimProtoFn (UTF16_EQUAL             , &PrimProtoFnEqual_EM_YY             );   // Alt-'5' - equal
    InitPrimProtoFn (UTF16_RIGHTCARETUNDERBAR, &PrimProtoFnRightCaretUnderbar_EM_YY);   // Alt-'6' - not less
    InitPrimProtoFn (UTF16_RIGHTCARET        , &PrimProtoFnRightCaret_EM_YY        );   // Alt-'7' - more
    InitPrimProtoFn (UTF16_NOTEQUAL          , &PrimProtoFnNotEqual_EM_YY          );   // Alt-'8' - not equal
    InitPrimProtoFn (UTF16_DOWNCARET         , &PrimProtoFnDownCaret_EM_YY         );   // Alt-'9' - or
////                                                                                    // Alt-':' - (none)
    InitPrimProtoFn (UTF16_DOWNTACKJOT       , &PrimProtoFnDownTackJot_EM_YY       );   // Alt-';' - format
////                                                                                    // Alt-'<' - (none)
    InitPrimProtoFn (UTF16_COLONBAR          , &PrimProtoFnColonBar_EM_YY          );   // Alt-'=' - divide
////                                                                                    // Alt-'>' - (none)
    InitPrimProtoOp (UTF16_CIRCLEMIDDLEDOT   , &PrimProtoOpCircleMiddleDot_EM_YY   );   // Alt-'?' - circle-middle-dot
    InitPrimProtoFn (UTF16_NOTEQUALUNDERBAR  , &PrimProtoFnNotEqualUnderbar_EM_YY  );   // Alt-'@' - mismatch
////                                                                                    // Alt-'A' - (none)
////                                                                                    // Alt-'B' - (none)
////                                                                                    // Alt-'C' - (none)
////                                                                                    // Alt-'D' - (none)
    InitPrimProtoFn (UTF16_EPSILONUNDERBAR   , &PrimProtoFnEpsilonUnderbar_EM_YY   );   // Alt-'E' - epsilon-underbar
////                                                                                    // Alt-'F' - (none)
    InitPrimProtoOp (UTF16_DIERESISDEL       , &PrimProtoOpDieresisDel_EM_YY       );   // Alt-'G' - dieresis-del (dual)
////                                                                                    // Alt-'H' - delta-underbar
    InitPrimProtoFn (UTF16_IOTAUNDERBAR      , &PrimProtoFnIotaUnderbar_EM_YY      );   // Alt-'I' - iota-underbar
    InitPrimProtoOp (UTF16_DIERESISJOT       , &PrimProtoOpDieresisJot_EM_YY       );   // Alt-'J' - dieresis-jot (rank)
////                                                                                    // Alt-'K' - (none)
    InitPrimProtoFn (UTF16_SQUAD             , &PrimProtoFnSquad_EM_YY             );   // Alt-'L' - squad
    InitPrimProtoOp (UTF16_STILETILDE        , &PrimProtoOpStileTilde_EM_YY        );   // Alt-'M' - stile-tilde (partition)
    InitPrimProtoOp (UTF16_DIERESISDOWNTACK  , &PrimProtoOpDieresisDownTack_EM_YY  );   // Alt-'N' - dieresis-downtack (convolution)
    InitPrimProtoOp (UTF16_DIERESISCIRCLE    , &PrimProtoOpDieresisCircle_EM_YY    );   // Alt-'O' - dieresis-circle (compose)
    InitPrimProtoOp (UTF16_DIERESISSTAR      , &PrimProtoOpDieresisStar_EM_YY      );   // Alt-'P' - dieresis-star (power)
////                                                                                    // Alt-'Q' - (none)
////                                                                                    // Alt-'R' - (none)
////                                                                                    // Alt-'S' - (none)
    InitPrimProtoOp (UTF16_DIERESISTILDE     , &PrimProtoOpDieresisTilde_EM_YY     );   // Alt-'T' - dieresis-tilde (commute/duplicate)
////                                                                                    // Alt-'U' - (none)
////                                                                                    // Alt-'V' - (none)
////                                                                                    // Alt-'W' - (none)
////                                                                                    // Alt-'X' - (none)
////                                                                                    // Alt-'Y' - (none)
////                                                                                    // Alt-'Z' - (none)
////                                                                                    // Alt-'[' - left arrow
    InitPrimProtoFn (UTF16_LEFTTACK          , &PrimProtoFnLeftTack_EM_YY          );   // Alt-'\' - left tack
////                                                                                    // Alt-']' - right arrow
    InitPrimProtoFn (UTF16_QUOTEDOT          , &PrimProtoFnQuoteDot_EM_YY          );   // Alt-'_' - quote-dot
////                                                                                    // Alt-'`' - diamond
////                                                                                    // Alt-'{' - quote-quad
    InitPrimProtoFn (UTF16_RIGHTTACK         , &PrimProtoFnRightTack_EM_YY         );   // Alt-'|' - right tack
////                                                                                    // Alt-'}' - zilde
    InitPrimProtoFn (UTF16_COMMABAR          , &PrimProtoFnCommaBar_EM_YY          );   // Alt-'~' - comma-bar
    InitPrimProtoFn (UTF16_CIRCUMFLEX        , &PrimProtoFnUpCaret_EM_YY           );   //         -
    InitPrimProtoFn (UTF16_BAR               , &PrimProtoFnBar_EM_YY               );   //         -
    InitPrimProtoFn (UTF16_PLUS              , &PrimProtoFnPlus_EM_YY              );   //         -
    InitPrimProtoFn (UTF16_STILE2            , &PrimProtoFnStile_EM_YY             );   //         -
    InitPrimProtoFn (UTF16_TILDE2            , &PrimProtoFnTilde_EM_YY             );   //         -
    InitPrimProtoFn (UTF16_COMMA             , &PrimProtoFnComma_EM_YY             );   //         -
    InitPrimProtoFn (UTF16_SLASH             , &PrimProtoFnSlash_EM_YY             );   //         - slash as Function
    InitPrimProtoOp (INDEX_OPSLASH           , &PrimProtoOpSlash_EM_YY             );   //         - ...      Operator
    InitPrimProtoFn (UTF16_SLOPE             , &PrimProtoFnSlope_EM_YY             );   //         - slope as Function
    InitPrimProtoOp (INDEX_OPSLOPE           , &PrimProtoOpSlope_EM_YY             );   //         - ...      Operator
    InitPrimProtoOp (UTF16_DOT               , &PrimProtoOpDot_EM_YY               );   //         - dot (inner product)
    InitPrimProtoOp (INDEX_JOTDOT            , &PrimProtoOpJotDot_EM_YY            );   //         - jotdot (outer product)
} // End InitPrimProtoFns
#undef  APPEND_NAME


//***************************************************************************
//  $InitPrimProtoFn
//
//  Initialize a single primitive function prototype
//***************************************************************************

void InitPrimProtoFn
    (WCHAR     wchFn,
     LPPRIMFNS lpPrimFn)

{
    if (PrimProtoFnsTab[PRIMTAB_MASK & wchFn])
        DbgStop ();
    else
        PrimProtoFnsTab[PRIMTAB_MASK & wchFn] = lpPrimFn;
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
    if (PrimProtoFnsTab[PRIMTAB_MASK & wchFn])
        DbgStop ();
    else
        // Note the white lie we tell here (LPPRIMFNS) instead of
        //   (LPPRIMOPS) so we can use one table for both primitive
        //   functions and operators.  When the prototype function
        //   for a primitive function is called, lptkFunc is a ptr
        //   to the primitive function token.  When the prototype
        //   function for a primitive operator is called, the same
        //   parameter is a ptr to the function strand.
        PrimProtoFnsTab[PRIMTAB_MASK & wchFn] = (LPPRIMFNS) lpPrimOp;
} // End InitPrimProtoOp


//***************************************************************************
//  $PrimProtoFn_EM
//
//  Default function for symbols for which there is no corresponding
//    primitive function/operator prototype
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFn_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFn_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnNonceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimProtoFn_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOp_EM
//
//  Default function for symbols for which there is no corresponding
//    primitive operator prototype
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoOp_EM"
#else
#define APPEND_NAME
#endif

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
#undef  APPEND_NAME


//***************************************************************************
//  $InitPrimSpecs
//
//  Initialize the table of PRIMSPECs
//***************************************************************************

void InitPrimSpecs
    (void)

{
    // Initialize the table of PRIMSPECs
    Init1PrimSpec (UTF16_BAR               , &PrimSpecBar               );
    Init1PrimSpec (UTF16_CIRCLE            , &PrimSpecCircle            );
    Init1PrimSpec (UTF16_CIRCLESTAR        , &PrimSpecCircleStar        );
    Init1PrimSpec (UTF16_CIRCUMFLEX        , &PrimSpecUpCaret           );
    Init1PrimSpec (UTF16_COLONBAR          , &PrimSpecColonBar          );
    Init1PrimSpec (UTF16_DOWNCARET         , &PrimSpecDownCaret         );
    Init1PrimSpec (UTF16_DOWNCARETTILDE    , &PrimSpecDownCaretTilde    );
    Init1PrimSpec (UTF16_DOWNSTILE         , &PrimSpecDownStile         );
    Init1PrimSpec (UTF16_EQUAL             , &PrimSpecEqual             );
    Init1PrimSpec (UTF16_LEFTCARET         , &PrimSpecLeftCaret         );
    Init1PrimSpec (UTF16_LEFTCARETUNDERBAR , &PrimSpecLeftCaretUnderbar );
    Init1PrimSpec (UTF16_NOTEQUAL          , &PrimSpecNotEqual          );
    Init1PrimSpec (UTF16_PLUS              , &PrimSpecPlus              );
    Init1PrimSpec (UTF16_QUOTEDOT          , &PrimSpecQuoteDot          );
    Init1PrimSpec (UTF16_QUERY             , &PrimSpecQuery             );
    Init1PrimSpec (UTF16_RIGHTCARET        , &PrimSpecRightCaret        );
    Init1PrimSpec (UTF16_RIGHTCARETUNDERBAR, &PrimSpecRightCaretUnderbar);
    Init1PrimSpec (UTF16_STAR              , &PrimSpecStar              );
    Init1PrimSpec (UTF16_STILE             , &PrimSpecStile             );
    Init1PrimSpec (UTF16_STILE2            , &PrimSpecStile             );
    Init1PrimSpec (UTF16_TILDE             , &PrimSpecTilde             );
    Init1PrimSpec (UTF16_TILDE2            , &PrimSpecTilde             );
    Init1PrimSpec (UTF16_TIMES             , &PrimSpecTimes             );
    Init1PrimSpec (UTF16_UPCARET           , &PrimSpecUpCaret           );
    Init1PrimSpec (UTF16_UPCARETTILDE      , &PrimSpecUpCaretTilde      );
    Init1PrimSpec (UTF16_UPSTILE           , &PrimSpecUpStile           );
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
    if (PrimSpecTab[PRIMTAB_MASK & wchFn])
        DbgStop ();
    else
        PrimSpecTab[PRIMTAB_MASK & wchFn] = lpPrimSpec;
} // End Init1PrimSpec


//***************************************************************************
//  $InitPrimFlags
//
//  Initialize the table of primitive function/operator flags
//***************************************************************************

void InitPrimFlags
    (void)

{
    PRIMFLAGS PrimFlag;
    WORD      PF_FB,                // Fast Boolean reduction and/or scan
              PF_AN,                // Function is associative on all numbers
              PF_AB,                // ...         associative on Booleans only
              PF_AL,                // ...         alternating
              PF_MS,                // ...         monadic scalar
              PF_DS,                // ...         dyadic  ...
              PF_ID;                // ...      has an identity element

    // Get the bits as masks
    *((WORD *) &PrimFlag) = 0; PrimFlag.FastBool  = TRUE; PF_FB = *(WORD *) &PrimFlag;
    *((WORD *) &PrimFlag) = 0; PrimFlag.AssocBool = TRUE; PF_AB = *(WORD *) &PrimFlag;
    *((WORD *) &PrimFlag) = 0; PrimFlag.AssocNumb = TRUE; PF_AN = *(WORD *) &PrimFlag;
    *((WORD *) &PrimFlag) = 0; PrimFlag.Alter     = TRUE; PF_AL = *(WORD *) &PrimFlag;
    *((WORD *) &PrimFlag) = 0; PrimFlag.MonScalar = TRUE; PF_MS = *(WORD *) &PrimFlag;
    *((WORD *) &PrimFlag) = 0; PrimFlag.DydScalar = TRUE; PF_DS = *(WORD *) &PrimFlag;
    *((WORD *) &PrimFlag) = 0; PrimFlag.IdentElem = TRUE; PF_ID = *(WORD *) &PrimFlag;

    Init1PrimFlag (UTF16_BAR               , 0                     | PF_AL | PF_MS | PF_DS | PF_ID | PF_INDEX_MINUS   );
    Init1PrimFlag (UTF16_CIRCLE            , 0                             | PF_MS | PF_DS                            );
    Init1PrimFlag (UTF16_CIRCLESTAR        , 0                             | PF_MS | PF_DS                            );
    Init1PrimFlag (UTF16_CIRCUMFLEX        , PF_FB | PF_AB                 | PF_MS | PF_DS | PF_ID | PF_INDEX_AND     );
    Init1PrimFlag (UTF16_COLONBAR          , 0                     | PF_AL | PF_MS | PF_DS | PF_ID | PF_INDEX_DIVIDE  );
    Init1PrimFlag (UTF16_DOWNCARET         , PF_FB | PF_AB                 | PF_MS | PF_DS | PF_ID | PF_INDEX_OR      );
    Init1PrimFlag (UTF16_DOWNCARETTILDE    , PF_FB                         | PF_MS | PF_DS         | PF_INDEX_NOR     );
    Init1PrimFlag (UTF16_DOWNSTILE         , PF_FB | PF_AB | PF_AN         | PF_MS | PF_DS | PF_ID | PF_INDEX_MIN     );
    Init1PrimFlag (UTF16_EQUAL             , PF_FB | PF_AB                 | PF_MS | PF_DS | PF_ID | PF_INDEX_EQUAL   );
    Init1PrimFlag (UTF16_LEFTCARET         , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_INDEX_LESS    );
    Init1PrimFlag (UTF16_LEFTCARETUNDERBAR , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_INDEX_LESSEQ  );
    Init1PrimFlag (UTF16_NOTEQUAL          , PF_FB | PF_AB                 | PF_MS | PF_DS | PF_ID | PF_INDEX_NOTEQUAL);
    Init1PrimFlag (UTF16_PLUS              , 0     | PF_AB | PF_AN         | PF_MS | PF_DS | PF_ID | PF_INDEX_PLUS    );
    Init1PrimFlag (UTF16_QUOTEDOT          , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_INDEX_LESSEQ  );
    Init1PrimFlag (UTF16_QUERY             , 0                             | PF_MS                                    );
    Init1PrimFlag (UTF16_RIGHTCARET        , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_INDEX_MORE    );
    Init1PrimFlag (UTF16_RIGHTCARETUNDERBAR, PF_FB                         | PF_MS | PF_DS | PF_ID | PF_INDEX_MOREEQ  );
    Init1PrimFlag (UTF16_STAR              , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_INDEX_MOREEQ  );
    Init1PrimFlag (UTF16_STILE             , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_INDEX_LESS    );
    Init1PrimFlag (UTF16_STILE2            , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_INDEX_LESS    );
    Init1PrimFlag (UTF16_TILDE             , 0                             | PF_MS                                    ); // Identity element:  {enclose}{zilde} ??
    Init1PrimFlag (UTF16_TILDE2            , 0                             | PF_MS                                    ); // Identity element:  {enclose}{zilde} ??
    Init1PrimFlag (UTF16_TIMES             , PF_FB | PF_AB | PF_AN         | PF_MS | PF_DS | PF_ID | PF_INDEX_AND     );
    Init1PrimFlag (UTF16_UPCARET           , PF_FB | PF_AB                 | PF_MS | PF_DS | PF_ID | PF_INDEX_AND     );
    Init1PrimFlag (UTF16_UPCARETTILDE      , PF_FB                         | PF_MS | PF_DS         | PF_INDEX_NAND    );
    Init1PrimFlag (UTF16_UPSTILE           , PF_FB | PF_AB | PF_AN         | PF_MS | PF_DS | PF_ID | PF_INDEX_MAX     );
} // End InitPrimFlags


//***************************************************************************
//  $Init1PrimFlag
//
//  Initialize a single primitive flag
//***************************************************************************

void Init1PrimFlag
    (WCHAR wchFn,
     UINT  uFlag)

{
    *((WORD *) &PrimFlags[PRIMTAB_MASK & wchFn]) |= uFlag;
} // End Init1PrimFlag


//***************************************************************************
//  $InitIdentityElements
//
//  Initialize the table of primitive scalar function identity elements
//***************************************************************************

void InitIdentityElements
    (void)

{
    Init1IdentityElement (PF_INDEX_PLUS    , 0.0);
    Init1IdentityElement (PF_INDEX_MINUS   , 0.0);
    Init1IdentityElement (PF_INDEX_DIVIDE  , 1.0);

    Init1IdentityElement (PF_INDEX_MIN     , PosInfinity);
    Init1IdentityElement (PF_INDEX_MAX     , NegInfinity);

    Init1IdentityElement (PF_INDEX_AND     , 1.0);
    Init1IdentityElement (PF_INDEX_OR      , 0.0);

    Init1IdentityElement (PF_INDEX_LESS    , 0.0);
    Init1IdentityElement (PF_INDEX_LESSEQ  , 1.0);
    Init1IdentityElement (PF_INDEX_EQUAL   , 1.0);
    Init1IdentityElement (PF_INDEX_MOREEQ  , 1.0);
    Init1IdentityElement (PF_INDEX_MORE    , 0.0);

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
//  $InitIdentityFunctions
//
//  Initialize the table of primitive scalar function identity functions
//***************************************************************************

void InitIdentityFunctions
    (void)

{
    //                     Unicode symbol      Identity Function                              LftIE   RhtIE      Axis?
    Init1IdentityFunction (UTF16_CIRCLESLOPE   , (LPPRIMOPS) PrimIdentFnCircleSlope_EM_YY   , TRUE  , FALSE );  // N
    Init1IdentityFunction (UTF16_CIRCLEBAR     , (LPPRIMOPS) PrimIdentFnCircleStile_EM_YY   , TRUE  , FALSE );  // Y
    Init1IdentityFunction (UTF16_CIRCLESTILE   , (LPPRIMOPS) PrimIdentFnCircleStile_EM_YY   , TRUE  , FALSE );  // Y
    Init1IdentityFunction (UTF16_COMMA         , (LPPRIMOPS) PrimIdentFnComma_EM_YY         , TRUE  , TRUE  );  // Y
    Init1IdentityFunction (UTF16_COMMABAR      , (LPPRIMOPS) PrimIdentFnComma_EM_YY         , TRUE  , TRUE  );  // Y
    Init1IdentityFunction (UTF16_DIERESIS      ,             PrimIdentOpDieresis_EM_YY      , TRUE  , TRUE  );  // Y
    Init1IdentityFunction (UTF16_DIERESISTILDE ,             PrimIdentOpDieresisTilde_EM_YY , TRUE  , TRUE  );  // N
    Init1IdentityFunction (UTF16_DOMINO        , (LPPRIMOPS) PrimIdentFnDomino_EM_YY        , FALSE , TRUE  );  // N
    Init1IdentityFunction (UTF16_DOT           ,             PrimIdentOpDot_EM_YY           , FALSE , TRUE  );  // N
    Init1IdentityFunction (UTF16_DOWNARROW     , (LPPRIMOPS) PrimIdentFnDownArrow_EM_YY     , TRUE  , FALSE );  // Y
    Init1IdentityFunction (UTF16_DOWNTACK      , (LPPRIMOPS) PrimIdentFnDownTack_EM_YY      , TRUE  , FALSE );  // N
    Init1IdentityFunction (INDEX_JOTDOT        ,             PrimIdentOpJotDot_EM_YY        , TRUE  , TRUE  );  // N
    Init1IdentityFunction (UTF16_RHO           , (LPPRIMOPS) PrimIdentFnRho_EM_YY           , TRUE  , FALSE );  // N
    Init1IdentityFunction (UTF16_RIGHTSHOE     , (LPPRIMOPS) PrimIdentFnRightShoe_EM_YY     , TRUE  , FALSE );  // N
    Init1IdentityFunction (UTF16_SQUAD         , (LPPRIMOPS) PrimIdentFnSquad_EM_YY         , TRUE  , FALSE );  // Y
    Init1IdentityFunction (UTF16_TILDE         , (LPPRIMOPS) PrimIdentFnTilde_EM_YY         , FALSE , TRUE  );  // N
    Init1IdentityFunction (UTF16_TILDE2        , (LPPRIMOPS) PrimIdentFnTilde_EM_YY         , FALSE , TRUE  );  // N
    Init1IdentityFunction (UTF16_UPARROW       , (LPPRIMOPS) PrimIdentFnUpArrow_EM_YY       , TRUE  , FALSE );  // Y
} // End InitIdentityFunctions


//***************************************************************************
//  $Init1IdentityFunction
//
//  Initialize an identity function
//***************************************************************************

void Init1IdentityFunction
    (WCHAR     wchFn,               // The masked UTF16_xxx value
     LPPRIMOPS lpPrimOps,           // Ptr to identity function
     UBOOL     bLftIdent,           // TRUE iff the identity function is a left identity
     UBOOL     bRhtIdent)           // ...                                 right ...

{
    if (PrimIdentFnsTab[PRIMTAB_MASK & wchFn].lpPrimOps)
        DbgStop ();
    else
    {
        PrimIdentFnsTab[PRIMTAB_MASK & wchFn].lpPrimOps = lpPrimOps;
        PrimIdentFnsTab[PRIMTAB_MASK & wchFn].bLftIdent = bLftIdent;
        PrimIdentFnsTab[PRIMTAB_MASK & wchFn].bRhtIdent = bRhtIdent;
    } // End IF/ELSE
} // End Init1IdentityFunction


//***************************************************************************
//  End of File: initdata.c
//***************************************************************************
