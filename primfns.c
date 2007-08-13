//***************************************************************************
//  NARS2000 -- Primitive Functions
//***************************************************************************

#define STRICT
#include <windows.h>
#include <float.h>
#include <math.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "sysvars.h"
#include "externs.h"
#include "pertab.h"
#include "dfnhdr.h"
#include "sis.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

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


// Symbols for which there is no definition            Monadic      Dyadic
#define PrimFnAlpha_EM_YY           PrimFn_EM       // ERROR        ERROR
#define PrimFnIotaUnderbar_EM_YY    PrimFn_EM       // ERROR        ERROR
#define PrimFnOmega_EM_YY           PrimFn_EM       // ERROR        ERROR
#define PrimFnUpShoe_EM_YY          PrimFn_EM       // ERROR        ERROR


// Primitives TO DO                                    Monadic      Dyadic
#define PrimFnDelStile_EM_YY        PrimFn_EM       // Mixed (*)    Mixed (*)
#define PrimFnDeltaStile_EM_YY      PrimFn_EM       // Mixed (*)    Mixed (*)
#define PrimFnDownArrow_EM_YY       PrimFn_EM       // ERROR        Mixed (*)
#define PrimFnDownShoe_EM_YY        PrimFn_EM       // Mixed (*)    ERROR
#define PrimFnDownTack_EM_YY        PrimFn_EM       // ERROR        Mixed (*)
#define PrimFnEpsilonUnderbar_EM_YY PrimFn_EM       // ERROR        Mixed (*)
#define PrimFnRightShoe_EM_YY       PrimFn_EM       // Mixed (*)    Mixed (*)
#define PrimFnUpTack_EM_YY          PrimFn_EM       // ERROR        Mixed (*)


// Operators TO DO                                     Monadic      Dyadic
/////// PrimOpDot_EM_YY                             // ERROR        Inner product (*)


// Primitives DONE                                     Monadic      Dyadic
/////// PrimFnBar_EM_YY                             // Scalar       Scalar
/////// PrimFnCircle_EM_YY                          // Scalar       Scalar
/////// PrimFnCircleBar_EM_YY                       // Mixed        Mixed
/////// PrimFnCircleSlope_EM_YY                     // Mixed        Mixed
/////// PrimFnCircleStar_EM_YY                      // Scalar       Scalar
/////// PrimFnCircleStile_EM_YY                     // Mixed        Mixed
/////// PrimFnComma_EM_YY                           // Mixed        Mixed
/////// PrimFnCommaBar_EM_YY                        // Mixed        Mixed
/////// PrimFnColonBar_EM_YY                        // Scalar       Scalar
/////// PrimFnDomino_EM_YY                          // Mixed        Mixed
/////// PrimFnDownCaret_EM_YY                       // ERROR        Scalar
/////// PrimFnDownCaretTilde_EM_YY                  // ERROR        Scalar
/////// PrimFnDownStile_EM_YY                       // Scalar       Scalar
/////// PrimFnEpsilon_EM_YY                         // Mixed        Mixed (*)
/////// PrimFnEqual_EM_YY                           // ERROR        Scalar
/////// PrimFnEqualUnderbar_EM_YY                   // Mixed        Mixed
/////// PrimFnIota_EM_YY                            // Mixed        Mixed (*)
/////// PrimFnLeftCaret_EM_YY                       // ERROR        Scalar
/////// PrimFnLeftCaretUnderbar_EM_YY               // ERROR        Scalar
/////// PrimFnLeftShoe_EM_YY                        // Mixed        Mixed (*)
/////// PrimFnLeftTack_EM_YY                        // ERROR        Mixed
/////// PrimFnNotEqual_EM_YY                        // ERROR        Scalar
/////// PrimFnPlus_EM_YY                            // Scalar       Scalar
/////// PrimFnQuoteDot_EM_YY                        // Scalar       Scalar
/////// PrimFnQuery_EM_YY                           // Scalar       Mixed
/////// PrimFnRightCaret_EM_YY                      // ERROR        Scalar
/////// PrimFnRightCaretUnderbar_EM_YY              // ERROR        Scalar
/////// PrimFnRightTack_EM_YY                       // ERROR        Mixed
/////// PrimFnRho_EM_YY                             // Mixed        Mixed
/////// PrimFnSlash_EM_YY                           // ERROR        Mixed
/////// PrimFnSlashBar_EM_YY                        // ERROR        Mixed
/////// PrimFnSlope_EM_YY                           // ERROR        Mixed
/////// PrimFnSlopeBar_EM_YY                        // ERROR        Mixed
/////// PrimFnSquad_EM_YY                           // ERROR        Mixed (*)
/////// PrimFnStar_EM_YY                            // Scalar       Scalar
/////// PrimFnStile_EM_YY                           // Scalar       Scalar
/////// PrimFnDownTackJot_EM_YY                     // Mixed        Mixed (*)
/////// PrimFnTilde_EM_YY                           // Scalar       Mixed (*)
/////// PrimFnTimes_EM_YY                           // Scalar       Scalar
/////// PrimFnUpArrow_EM_YY                         // Mixed        Mixed (*)
/////// PrimFnUpCaret_EM_YY                         // ERROR        Scalar
/////// PrimFnUpCaretTilde_EM_YY                    // ERROR        Scalar
/////// PrimFnUpStile_EM_YY                         // Scalar       Scalar
/////// PrimFnUpTackJot_EM_YY                       // Mixed        ERROR

// (*) = Unfinished


// Operators DONE                                      Monadic      Dyadic
/////// PrimOpDieresis_EM_YY                        // Each         Each
/////// PrimOpDieresisTilde_EM_YY                   // Duplicate    Commute
/////// PrimOpJot_EM_YY                             // Composition  Composition
/////// PrimOpJotDot_EM_YY                          // ERROR        Outer Product
/////// PrimOpSlash_EM_YY                           // Reduction    N-wise reduction
/////// PrimOpSlope_EM_YY                           // Scan         ERROR


// First coordinate functions handled by common function
#define PrimFnCircleBar_EM_YY   PrimFnCircleStile_EM_YY
#define PrimFnCommaBar_EM_YY    PrimFnComma_EM_YY
#define PrimFnSlashBar_EM_YY    PrimFnSlash_EM_YY
#define PrimFnSlopeBar_EM_YY    PrimFnSlope_EM_YY
#define PrimOpSlashBar_EM_YY    PrimOpSlash_EM_YY
#define PrimOpSlopeBar_EM_YY    PrimOpSlope_EM_YY


//***************************************************************************
//  $InitPrimTabs
//
//  Initialize various primitive function, operator,
//    prototype tables, and flags
//***************************************************************************

void InitPrimTabs
    (void)

{
    APLINT aplInteger;

    InitPrimFns ();
    InitPrimProtoFns ();
    InitPrimSpecs ();
    InitPrimFlags ();

    // Initialize identity elements
    InitIdentityElements ();

#define POS_INFINITY            (0x7FF0000000000000)
#define NEG_INFINITY            (0xFFF0000000000000)
#define QUIET_NAN               (0xFFF8000000000000)
#define FLOAT2POW53             (0x4340000000000000)

    // Create various floating point constants
    aplInteger = POS_INFINITY; PosInfinity = *(double *) &aplInteger;
    aplInteger = NEG_INFINITY; NegInfinity = *(double *) &aplInteger;
    aplInteger = FLOAT2POW53;  Float2Pow53 = *(double *) &aplInteger;
} // End InitPrimTabs


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
    InitPrimFn (INDEX_EPSILON           , &PrimFnEpsilon_EM_YY           ); // Alt-'e' - epsilon
////                                                                        // Alt-'f' - underbar
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
    InitPrimFn (INDEX_DOWNSHOE          , &PrimFnDownShoe_EM_YY          ); // Alt-'v' - down shoe
    InitPrimFn (UTF16_OMEGA             , &PrimFnOmega_EM_YY             ); // Alt-'w' - omega
    InitPrimFn (UTF16_RIGHTSHOE         , &PrimFnRightShoe_EM_YY         ); // Alt-'x' - right shoe
    InitPrimFn (UTF16_UPARROW           , &PrimFnUpArrow_EM_YY           ); // Alt-'y' - up arrow
    InitPrimFn (UTF16_LEFTSHOE          , &PrimFnLeftShoe_EM_YY          ); // Alt-'z' - left shoe
    InitPrimFn (UTF16_EQUALUNDERBAR     , &PrimFnEqualUnderbar_EM_YY     ); // Alt-'!' - match
////                                                                        // Alt-'"' - (none)
    InitPrimFn (UTF16_DELSTILE          , &PrimFnDelStile_EM_YY          ); // Alt-'#' - grade-down
    InitPrimFn (UTF16_DELTASTILE        , &PrimFnDeltaStile_EM_YY        ); // Alt-'$' - grade-up
    InitPrimFn (INDEX_CIRCLESTILE       , &PrimFnCircleStile_EM_YY       ); // Alt-'%' - rotate
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
    InitPrimFn (INDEX_SLASHBAR          , &PrimFnSlashBar_EM_YY          ); // Alt-'/' - slash-bar
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
////                                                                        // Alt-'?' - (none)
////                                                                        // Alt-'@' - del-tilde
////                                                                        // Alt-'A' - (none)
////                                                                        // Alt-'B' - (none)
////                                                                        // Alt-'C' - (none)
////                                                                        // Alt-'D' - (none)
    InitPrimFn (UTF16_EPSILONUNDERBAR   , &PrimFnEpsilonUnderbar_EM_YY   ); // Alt-'E' - epsilon-underbar
////                                                                        // Alt-'F' - (none)
    InitPrimFn (UTF16_DIERESISDEL       , (LPPRIMFNS) -1                 ); // Alt-'G' - dieresis-del (dual)
////                                                                        // Alt-'H' - delta-underbar
    InitPrimFn (UTF16_IOTAUNDERBAR      , &PrimFnIotaUnderbar_EM_YY      ); // Alt-'I' - iota-underbar
    InitPrimFn (INDEX_DIERESISJOT       , (LPPRIMFNS) -1                 ); // Alt-'J' - dieresis-jot (rank)
////                                                                        // Alt-'K' - (none)
    InitPrimFn (UTF16_SQUAD             , &PrimFnSquad_EM_YY             ); // Alt-'L' - squad
    InitPrimFn (UTF16_STILETILDE        , (LPPRIMFNS) -1                 ); // Alt-'M' - stile-tilde (partition)
    InitPrimFn (INDEX_DIERESISDOWNTACK  , (LPPRIMFNS) -1                 ); // Alt-'N' - dieresis-downtack (convolution)
    InitPrimFn (INDEX_DIERESISCIRCLE    , (LPPRIMFNS) -1                 ); // Alt-'O' - dieresis-circle (holler)
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
    InitPrimFn (UTF16_LEFTTACK          , &PrimFnLeftTack_EM_YY          ); // Alt-'\\'- left tack
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
    if (PrimFnsTab[(UCHAR) wchFn])
        DbgStop ();
    else
        PrimFnsTab[(UCHAR) wchFn] = lpPrimFn;
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

// Symbols for which there is no definition
#define PrimProtoFnAlpha_EM_YY                  PrimProtoFn_EM
#define PrimProtoFnIotaUnderbar_EM_YY           PrimProtoFn_EM
#define PrimProtoFnOmega_EM_YY                  PrimProtoFn_EM
#define PrimProtoFnUpShoe_EM_YY                 PrimProtoFn_EM


// Primitive functions TO DO
#define PrimProtoFnDelStile_EM_YY               PrimProtoFn_EM
#define PrimProtoFnDeltaStile_EM_YY             PrimProtoFn_EM
#define PrimProtoFnDownArrow_EM_YY              PrimProtoFn_EM
#define PrimProtoFnDownShoe_EM_YY               PrimProtoFn_EM
#define PrimProtoFnDownTack_EM_YY               PrimProtoFn_EM
#define PrimProtoFnEpsilonUnderbar_EM_YY        PrimProtoFn_EM
#define PrimProtoFnRightShoe_EM_YY              PrimProtoFn_EM
#define PrimProtoFnUpTack_EM_YY                 PrimProtoFn_EM
#define PrimProtoFnUpTackJot_EM_YY              PrimProtoFn_EM


// Primitive operators TO DO
#define PrimProtoOpDieresisCircle_EM_YY         PrimProtoOp_EM
#define PrimProtoOpDieresisDel_EM_YY            PrimProtoOp_EM
#define PrimProtoOpDieresisDownTack_EM_YY       PrimProtoOp_EM
#define PrimProtoOpDieresisJot_EM_YY            PrimProtoOp_EM
#define PrimProtoOpDieresisStar_EM_YY           PrimProtoOp_EM
#define PrimProtoOpStileTilde_EM_YY             PrimProtoOp_EM


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
#define PrimProtoFnQuery_EM_YY                  PrimProtoFnQuery_EM_YY
#define PrimProtoFnRightCaret_EM_YY             PrimProtoFnScalar_EM_YY
#define PrimProtoFnRightCaretUnderbar_EM_YY     PrimProtoFnScalar_EM_YY
#define PrimProtoFnStar_EM_YY                   PrimProtoFnScalar_EM_YY
#define PrimProtoFnStile_EM_YY                  PrimProtoFnScalar_EM_YY
#define PrimProtoFnTilde_EM_YY                  PrimProtoFnTilde_EM_YY
#define PrimProtoFnTimes_EM_YY                  PrimProtoFnScalar_EM_YY
#define PrimProtoFnUpCaret_EM_YY                PrimProtoFnScalar_EM_YY
#define PrimProtoFnUpCaretTilde_EM_YY           PrimProtoFnScalar_EM_YY
#define PrimProtoFnUpStile_EM_YY                PrimProtoFnScalar_EM_YY


// Primitive mixed functions DONE
/////// PrimProtoFnComma_EM_YY
/////// PrimProtoFnDomino_EM_YY
/////// PrimProtoFnCircleSlope_EM_YY
/////// PrimProtoFnCircleStile_EM_YY
/////// PrimProtoFnDomino_EM_YY
/////// PrimProtoFnDownTackJot_EM_YY
/////// PrimProtoFnEpsilon_EM_YY
/////// PrimProtoFnEqualUnderbar_EM_YY
/////// PrimProtoFnIota_EM_YY
/////// PrimProtoFnLeftShoe_EM_YY
/////// PrimProtoFnLeftTack_EM_YY
/////// PrimProtoFnQuery_EM_YY
/////// PrimProtoFnRho_EM_YY
/////// PrimProtoFnRightTack_EM_YY
/////// PrimProtoFnSlash_EM_YY
/////// PrimProtoFnSlope_EM_YY
/////// PrimProtoFnSquad_EM_YY
/////// PrimProtoFnTilde_EM_YY
/////// PrimProtoFnUpArrow_EM_YY


// Primitive operators DONE
/////// PrimProtoOpDieresis_EM_YY
/////// PrimProtoOpDieresisTilde_EM_YY
/////// PrimProtoOpDot_EM_YY
/////// PrimProtoOpJot_EM_YY
/////// PrimProtoOpJotDot_EM_YY
/////// PrimProtoOpSlash_EM_YY
/////// PrimProtoOpSlope_EM_YY


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
    InitPrimProtoFn (INDEX_EPSILON           , &PrimProtoFnEpsilon_EM_YY           );   // Alt-'e' - epsilon
////                                                                                    // Alt-'f' - underbar
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
    InitPrimProtoFn (INDEX_DOWNSHOE          , &PrimProtoFnDownShoe_EM_YY          );   // Alt-'v' - down shoe
    InitPrimProtoFn (UTF16_OMEGA             , &PrimProtoFnOmega_EM_YY             );   // Alt-'w' - omega
    InitPrimProtoFn (UTF16_RIGHTSHOE         , &PrimProtoFnRightShoe_EM_YY         );   // Alt-'x' - right shoe
    InitPrimProtoFn (UTF16_UPARROW           , &PrimProtoFnUpArrow_EM_YY           );   // Alt-'y' - up arrow
    InitPrimProtoFn (UTF16_LEFTSHOE          , &PrimProtoFnLeftShoe_EM_YY          );   // Alt-'z' - left shoe
    InitPrimProtoFn (UTF16_EQUALUNDERBAR     , &PrimProtoFnEqualUnderbar_EM_YY     );   // Alt-'!' - match
////                                                                                    // Alt-'"' - (none)
    InitPrimProtoFn (UTF16_DELSTILE          , &PrimProtoFnDelStile_EM_YY          );   // Alt-'#' - grade-down
    InitPrimProtoFn (UTF16_DELTASTILE        , &PrimProtoFnDeltaStile_EM_YY        );   // Alt-'$' - grade-up
    InitPrimProtoFn (INDEX_CIRCLESTILE       , &PrimProtoFnCircleStile_EM_YY       );   // Alt-'%' - rotate
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
    InitPrimProtoFn (INDEX_SLASHBAR          , &PrimProtoFnSlashBar_EM_YY          );   // Alt-'/' - slash-bar as Function
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
////                                                                                    // Alt-'?' - (none)
////                                                                                    // Alt-'@' - del-tilde
////                                                                                    // Alt-'A' - (none)
////                                                                                    // Alt-'B' - (none)
////                                                                                    // Alt-'C' - (none)
////                                                                                    // Alt-'D' - (none)
    InitPrimProtoFn (UTF16_EPSILONUNDERBAR   , &PrimProtoFnEpsilonUnderbar_EM_YY   );   // Alt-'E' - epsilon-underbar
////                                                                                    // Alt-'F' - (none)
    InitPrimProtoOp (UTF16_DIERESISDEL       , &PrimProtoOpDieresisDel_EM_YY       );   // Alt-'G' - dieresis-del (dual)
////                                                                                    // Alt-'H' - delta-underbar
    InitPrimProtoFn (UTF16_IOTAUNDERBAR      , &PrimProtoFnIotaUnderbar_EM_YY      );   // Alt-'I' - iota-underbar
    InitPrimProtoOp (INDEX_DIERESISJOT       , &PrimProtoOpDieresisJot_EM_YY       );   // Alt-'J' - dieresis-jot (rank)
////                                                                                    // Alt-'K' - (none)
    InitPrimProtoFn (UTF16_SQUAD             , &PrimProtoFnSquad_EM_YY             );   // Alt-'L' - squad
    InitPrimProtoOp (UTF16_STILETILDE        , &PrimProtoOpStileTilde_EM_YY        );   // Alt-'M' - stile-tilde (partition)
    InitPrimProtoOp (INDEX_DIERESISDOWNTACK  , &PrimProtoOpDieresisDownTack_EM_YY  );   // Alt-'N' - dieresis-downtack (convolution)
    InitPrimProtoOp (INDEX_DIERESISCIRCLE    , &PrimProtoOpDieresisCircle_EM_YY    );   // Alt-'O' - dieresis-circle (holler)
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
    InitPrimProtoFn (UTF16_LEFTTACK          , &PrimProtoFnLeftTack_EM_YY          );   // Alt-'\\'- left tack
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
    if (PrimProtoFnsTab[(UCHAR) wchFn])
        DbgStop ();
    else
        PrimProtoFnsTab[(UCHAR) wchFn] = lpPrimFn;
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
    if (PrimProtoFnsTab[(UCHAR) wchFn])
        DbgStop ();
    else
        // Note the white lie we tell here (LPPRIMFNS) instead of
        //   (LPPRIMOPS) so we can use one table for both primitive
        //   functions and operators.  When the prototype function
        //   for a primitive function is called, lptkFunc is a ptr
        //   to the primitive function token.  When the prototype
        //   function for a primitive operator is called, the same
        //   parameter is a ptr to the function strand.
        PrimProtoFnsTab[(UCHAR) wchFn] = (LPPRIMFNS) lpPrimOp;
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
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
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
    InitPrimSpec (UTF16_BAR               , &PrimSpecBar               );
    InitPrimSpec (UTF16_CIRCLE            , &PrimSpecCircle            );
    InitPrimSpec (UTF16_CIRCLESTAR        , &PrimSpecCircleStar        );
    InitPrimSpec (UTF16_CIRCUMFLEX        , &PrimSpecUpCaret           );
    InitPrimSpec (UTF16_COLONBAR          , &PrimSpecColonBar          );
    InitPrimSpec (UTF16_DOWNCARET         , &PrimSpecDownCaret         );
    InitPrimSpec (UTF16_DOWNCARETTILDE    , &PrimSpecDownCaretTilde    );
    InitPrimSpec (UTF16_DOWNSTILE         , &PrimSpecDownStile         );
    InitPrimSpec (UTF16_EQUAL             , &PrimSpecEqual             );
    InitPrimSpec (UTF16_LEFTCARET         , &PrimSpecLeftCaret         );
    InitPrimSpec (UTF16_LEFTCARETUNDERBAR , &PrimSpecLeftCaretUnderbar );
    InitPrimSpec (UTF16_NOTEQUAL          , &PrimSpecNotEqual          );
    InitPrimSpec (UTF16_PLUS              , &PrimSpecPlus              );
    InitPrimSpec (UTF16_QUOTEDOT          , &PrimSpecQuoteDot          );
    InitPrimSpec (UTF16_QUERY             , &PrimSpecQuery             );
    InitPrimSpec (UTF16_RIGHTCARET        , &PrimSpecRightCaret        );
    InitPrimSpec (UTF16_RIGHTCARETUNDERBAR, &PrimSpecRightCaretUnderbar);
    InitPrimSpec (UTF16_STAR              , &PrimSpecStar              );
    InitPrimSpec (UTF16_STILE             , &PrimSpecStile             );
    InitPrimSpec (UTF16_STILE2            , &PrimSpecStile             );
    InitPrimSpec (UTF16_TILDE             , &PrimSpecTilde             );
    InitPrimSpec (UTF16_TIMES             , &PrimSpecTimes             );
    InitPrimSpec (UTF16_UPCARET           , &PrimSpecUpCaret           );
    InitPrimSpec (UTF16_UPCARETTILDE      , &PrimSpecUpCaretTilde      );
    InitPrimSpec (UTF16_UPSTILE           , &PrimSpecUpStile           );
} // End InitPrimSpecs


//***************************************************************************
//  $InitPrimSpec
//
//  Initialize a single lpPrimSpec
//***************************************************************************

void InitPrimSpec
    (WCHAR      wchFn,
     LPPRIMSPEC lpPrimSpec)

{
    if (PrimSpecTab[(UCHAR) wchFn])
        DbgStop ();
    else
        PrimSpecTab[(UCHAR) wchFn] = lpPrimSpec;
} // End InitPrimSpec


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

    InitPrimFlag (UTF16_BAR               , 0                     | PF_AL | PF_MS | PF_DS | PF_ID | PF_INDEX_MINUS   );
    InitPrimFlag (UTF16_CIRCLE            , 0                             | PF_MS | PF_DS                            );
    InitPrimFlag (UTF16_CIRCLESTAR        , 0                             | PF_MS | PF_DS                            );
    InitPrimFlag (UTF16_CIRCUMFLEX        , PF_FB | PF_AB                 | PF_MS | PF_DS | PF_ID | PF_INDEX_AND     );
    InitPrimFlag (UTF16_COLONBAR          , 0                     | PF_AL | PF_MS | PF_DS | PF_ID | PF_INDEX_DIVIDE  );
    InitPrimFlag (UTF16_DOWNCARET         , PF_FB | PF_AB                 | PF_MS | PF_DS | PF_ID | PF_INDEX_OR      );
    InitPrimFlag (UTF16_DOWNCARETTILDE    , PF_FB                         | PF_MS | PF_DS         | PF_INDEX_NOR     );
    InitPrimFlag (UTF16_DOWNSTILE         , PF_FB | PF_AB | PF_AN         | PF_MS | PF_DS | PF_ID | PF_INDEX_MIN     );
    InitPrimFlag (UTF16_EQUAL             , PF_FB | PF_AB                 | PF_MS | PF_DS | PF_ID | PF_INDEX_EQUAL   );
    InitPrimFlag (UTF16_LEFTCARET         , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_INDEX_LESS    );
    InitPrimFlag (UTF16_LEFTCARETUNDERBAR , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_INDEX_LESSEQ  );
    InitPrimFlag (UTF16_NOTEQUAL          , PF_FB | PF_AB                 | PF_MS | PF_DS | PF_ID | PF_INDEX_NOTEQUAL);
    InitPrimFlag (UTF16_PLUS              , 0     | PF_AB | PF_AN         | PF_MS | PF_DS | PF_ID | PF_INDEX_PLUS    );
    InitPrimFlag (UTF16_QUOTEDOT          , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_INDEX_LESSEQ  );
    InitPrimFlag (UTF16_QUERY             , 0                             | PF_MS                                    );
    InitPrimFlag (UTF16_RIGHTCARET        , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_INDEX_MORE    );
    InitPrimFlag (UTF16_RIGHTCARETUNDERBAR, PF_FB                         | PF_MS | PF_DS | PF_ID | PF_INDEX_MOREEQ  );
    InitPrimFlag (UTF16_STAR              , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_INDEX_MOREEQ  );
    InitPrimFlag (UTF16_STILE             , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_INDEX_LESS    );
    InitPrimFlag (UTF16_STILE2            , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_INDEX_LESS    );
    InitPrimFlag (UTF16_TILDE             , 0                             | PF_MS                                    ); // Identity element:  {enclose}{zilde} ??
    InitPrimFlag (UTF16_TIMES             , PF_FB | PF_AB | PF_AN         | PF_MS | PF_DS | PF_ID | PF_INDEX_AND     );
    InitPrimFlag (UTF16_UPCARET           , PF_FB | PF_AB                 | PF_MS | PF_DS | PF_ID | PF_INDEX_AND     );
    InitPrimFlag (UTF16_UPCARETTILDE      , PF_FB                         | PF_MS | PF_DS         | PF_INDEX_NAND    );
    InitPrimFlag (UTF16_UPSTILE           , PF_FB | PF_AB | PF_AN         | PF_MS | PF_DS | PF_ID | PF_INDEX_MAX     );
} // End InitPrimFlags


//***************************************************************************
//  $InitPrimFlag
//
//  Initialize a single primitive flag
//***************************************************************************

void InitPrimFlag
    (WCHAR wchFn,
     UINT  uFlag)

{
    *((WORD *) &PrimFlags[(UCHAR) wchFn]) |= uFlag;
} // End InitPrimFlag


//***************************************************************************
//  $InitIdentityElements
//
//  Initialize the table of primitive scalar function identity elements
//***************************************************************************

void InitIdentityElements
    (void)

{
    APLFLOAT aplFloatPosInf,
             aplFloatNegInf;
    APLUINT  aplInteger;

    aplInteger = POS_INFINITY; aplFloatPosInf = *(LPAPLFLOAT) &aplInteger;
    aplInteger = NEG_INFINITY; aplFloatNegInf = *(LPAPLFLOAT) &aplInteger;

    InitIdentityElement (PF_INDEX_PLUS    , 0.0);
    InitIdentityElement (PF_INDEX_MINUS   , 0.0);
    InitIdentityElement (PF_INDEX_DIVIDE  , 1.0);

    InitIdentityElement (PF_INDEX_MIN     , aplFloatPosInf);
    InitIdentityElement (PF_INDEX_MAX     , aplFloatNegInf);

    InitIdentityElement (PF_INDEX_AND     , 1.0);
    InitIdentityElement (PF_INDEX_OR      , 0.0);

    InitIdentityElement (PF_INDEX_LESS    , 0.0);
    InitIdentityElement (PF_INDEX_LESSEQ  , 1.0);
    InitIdentityElement (PF_INDEX_EQUAL   , 1.0);
    InitIdentityElement (PF_INDEX_MOREEQ  , 1.0);
    InitIdentityElement (PF_INDEX_MORE    , 0.0);

    InitIdentityElement (PF_INDEX_NOTEQUAL, 0.0);
} // End InitIdentityElements


//***************************************************************************
//  $InitIdentityElement
//
//  Initialize an identity element
//***************************************************************************

void InitIdentityElement
    (UINT     uIndex,
     APLFLOAT aplFloat)

{
    PrimIdent[uIndex] = aplFloat;
} // End InitIdentityElement


//***************************************************************************
//  $ExecFunc_EM_YY
//
//  Execute a user-defined, system, or primitive function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecFunc_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecFunc_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStr,           // Ptr to function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    LPPRIMFNS lpPrimFn;             // Ptr to direct primitive or system function
    LPTOKEN   lptkAxis;             // Ptr to axis token (may be NULL)

    DBGENTER;

    // Check for NoValue
    if (IsTokenNoValue (lptkLftArg)
     || IsTokenNoValue (lptkRhtArg))
    {
        ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                                   (lptkLftArg NE NULL) ? lptkLftArg
                                                        : lptkRhtArg);
        return NULL;
    } // End IF

    // Split cases based upon the function token type
    switch (lpYYFcnStr->tkToken.tkFlags.TknType)
    {
        case TKT_FCNIMMED:
            lpPrimFn = PrimFnsTab[SymTrans (&lpYYFcnStr->tkToken)];
            if (!lpPrimFn)
            {
                ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                          &lpYYFcnStr->tkToken);
                return NULL;
            } // End IF

            return (*lpPrimFn) (lptkLftArg, &lpYYFcnStr->tkToken, lptkRhtArg, NULL);

        case TKT_FCNNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYFcnStr->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If the LPSYMENTRY is not immediate, it must be an HGLOBAL
            if (!lpYYFcnStr->tkToken.tkData.tkSym->stFlags.Imm)
            {
                STFLAGS stFlags;
                HGLOBAL hGlbFcn;

                // Get the STE flags of the first token
                stFlags = lpYYFcnStr->tkToken.tkData.tkSym->stFlags;

                // Skip assertion if it's some kind of function/operator
                if (IsNameTypeFnOp (stFlags.ObjType))
                {
                    LPPRIMFNS lpNameFcn;

                    // Get the address of the execution routine
                    lpNameFcn = lpYYFcnStr->tkToken.tkData.tkSym->stData.stNameFcn;

                    // If it's an internal function, go there
                    if (stFlags.FcnDir)
                        return (*lpNameFcn) (lptkLftArg, &lpYYFcnStr->tkToken, lptkRhtArg, NULL);

                    // Use the HGLOBAL
                    hGlbFcn = lpNameFcn;
                } else
                    // Use the HGLOBAL
                    hGlbFcn = lpYYFcnStr->tkToken.tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL function array or defined function
                Assert (IsGlbTypeFcnDir (hGlbFcn)
                     || IsGlbTypeDfnDir (hGlbFcn));

                // If it's a defined function, ...
                if (stFlags.UsrDfn)
                    return ExecDfnGlb_EM_YY (ClrPtrTypeDirGlb (hGlbFcn),    // Defined function global memory handle
                                             lptkLftArg,                    // Ptr to left arg token (may be NULL if monadic)
                                             lpYYFcnStr,                    // Ptr to function strand
                                             lptkRhtArg);                   // Ptr to right arg token
                else
                    // Execute a function array global memory handle
                    return ExecFuncGlb_EM_YY (lptkLftArg,
                                              ClrPtrTypeDirGlb (hGlbFcn),
                                              lptkRhtArg,
                                              NULL);
            } // End IF

            // Handle the immediate case

            // Split cases based upon the immediate type
            switch (lpYYFcnStr->tkToken.tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_PRIMFCN:
                {
                    TOKEN tkFn = {0};

                    lpPrimFn = PrimFnsTab[FcnTrans (lpYYFcnStr->tkToken.tkData.tkSym->stData.stChar)];
                    if (!lpPrimFn)
                    {
                        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                                  &lpYYFcnStr->tkToken);
                        return NULL;
                    } // End IF

                    // Fill in for PrimFn*** test
                    tkFn.tkFlags.TknType   = TKT_FCNIMMED;
                    tkFn.tkFlags.ImmType   = GetImmTypeFcn (lpYYFcnStr->tkToken.tkData.tkSym->stData.stChar);
////////////////////tkFn.tkFlags.NoDisplay = 0;     // Already zero from {0}
                    tkFn.tkData.tkChar     = lpYYFcnStr->tkToken.tkData.tkSym->stData.stChar;
                    tkFn.tkCharIndex       = lpYYFcnStr->tkToken.tkCharIndex;

                    return (*lpPrimFn) (lptkLftArg, &tkFn, lptkRhtArg, NULL);
                } // End IMMTYPE_PRIMFCN

                defstop
                    break;
            } // End SWITCH

        case TKT_FCNARRAY:
            // tkData is a valid HGLOBAL function array
            Assert (IsGlbTypeFcnDir (lpYYFcnStr->tkToken.tkData.tkVoid));

            // Check for axis operator
            if (lpYYFcnStr->FcnCount > 1
             && (lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
                lptkAxis = &lpYYFcnStr[1].tkToken;
            else
                lptkAxis = NULL;

            return ExecFuncGlb_EM_YY (lptkLftArg,
                                      ClrPtrTypeDirGlb (lpYYFcnStr->tkToken.tkData.tkGlbData),
                                      lptkRhtArg,
                                      lptkAxis);
        defstop
            break;
    } // End SWITCH

    return NULL;
} // End ExecFunc_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecFuncGlb_EM_YY
//
//  Execute a function in global memory
//***************************************************************************

LPPL_YYSTYPE ExecFuncGlb_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     HGLOBAL hGlbFcn,               // Handle to function strand
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYFcnStr,        // Ptr to function strand
                 lpYYRes;           // Ptr to the result

    // Lock the memory to get a ptr to it
    lpYYFcnStr = MyGlobalLock (hGlbFcn);

    // Split cases based upon the signature
    switch (((LPHEADER_SIGNATURE) lpYYFcnStr)->nature)
    {
        case FCNARRAY_HEADER_SIGNATURE:
            // Skip over the header to the data
            lpYYFcnStr = FcnArrayBaseToData (lpYYFcnStr);

            // The contents of the global memory object consist of
            //   a series of PL_YYSTYPEs in RPN order.
            lpYYRes = ExecFuncStr_EM_YY (lptkLftArg,    // Ptr to left arg token
                                         lpYYFcnStr,    // Ptr to function strand
                                         lptkRhtArg,    // Ptr to right arg token
                                         lptkAxis);     // Ptr to axis token
            break;

        case DFN_HEADER_SIGNATURE:
            DbgBrk ();


            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbFcn); lpYYFcnStr = NULL;

    return lpYYRes;
} // End ExecFuncGlb_EM_YY


//***************************************************************************
//  $ExecFuncStr_EM_YY
//
//  Execute a function strand
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecFuncStr_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecFuncStr_EM_YY
    (LPTOKEN      lptkLftArg,       // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStr,       // Ptr to function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     LPTOKEN      lptkAxis)         // Ptr to axis token (may be NULL)

{
    LPPRIMFNS lpPrimFn;             // Ptr to function address
    HGLOBAL   hGlbFcn;              // Function strand or defined function global memory handle

    Assert (lptkAxis EQ NULL);

    // Split cases based upon the type of the first token
    switch (lpYYFcnStr->tkToken.tkFlags.TknType)
    {
        case TKT_OP1IMMED:
        case TKT_OP1NAMED:
            return ExecOp1_EM_YY
                   (lptkLftArg,     // Ptr to left arg token
                    lpYYFcnStr,     // Ptr to operator function strand
                    lptkRhtArg);    // Ptr to right arg token

        case TKT_OP2IMMED:
        case TKT_OP2NAMED:
        case TKT_OPJOTDOT:
            return ExecOp2_EM_YY
                   (lptkLftArg,     // Ptr to left arg token
                    lpYYFcnStr,     // Ptr to operator function strand
                    lptkRhtArg);    // Ptr to right arg token

        case TKT_FCNIMMED:  // Either F or F[X]
            Assert (lpYYFcnStr->FcnCount EQ 1
                 || lpYYFcnStr->FcnCount EQ 2);

            // Check for axis operator
            if (lpYYFcnStr->FcnCount > 1
             && (lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
                lptkAxis = &lpYYFcnStr[1].tkToken;
            else
                lptkAxis = NULL;

            lpPrimFn = PrimFnsTab[SymTrans (&lpYYFcnStr->tkToken)];
            if (!lpPrimFn)
            {
                ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                          &lpYYFcnStr->tkToken);
                return NULL;
            } // End IF

            return (*lpPrimFn) (lptkLftArg, &lpYYFcnStr->tkToken, lptkRhtArg, lptkAxis);

        case TKT_FCNARRAY:
                                // 1.  User-defined operator
                                //   e.g., Z{is}L (F FOO G) R
                                //         +foo- 1 2
                                // 2.  User-defined function
                                //   e.g., Z{is}FOO R
                                //         3{jot}foo 1 2
                                // 3.  User-defined function w/axis operator
                                //   e.g., Z{is}FOO[X] R
                                //         foo[1] 1 2
                                // 4.  Monadic operator derived function
                                //   e.g., f{is}/[1]
                                //         +f 2 3{rho}{iota}6
            // Get the global memory handle
            hGlbFcn = lpYYFcnStr->tkToken.tkData.tkGlbData;

            // tkData is a valid HGLOBAL function array or defined function
            Assert (IsGlbTypeFcnDir (hGlbFcn)
                 || IsGlbTypeDfnDir (hGlbFcn));

            // Split cases based upon the array signature
            switch (GetSignatureGlb (ClrPtrTypeDirGlb (hGlbFcn)))
            {
                case FCNARRAY_HEADER_SIGNATURE:
                    // Check for axis operator
                    if (lpYYFcnStr->FcnCount > 1
                     && (lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
                      || lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
                        lptkAxis = &lpYYFcnStr[1].tkToken;
                    else
                        lptkAxis = NULL;

                    return ExecFuncGlb_EM_YY (lptkLftArg,                   // Ptr to left arg token (may be NULL if monadic)
                                              ClrPtrTypeDirGlb (hGlbFcn),   // Function array global memory handle
                                              lptkRhtArg,                   // Ptr to right arg token
                                              lptkAxis);                    // Ptr to axis token (may be NULL)
                case DFN_HEADER_SIGNATURE:
                    return ExecDfnGlb_EM_YY (ClrPtrTypeDirGlb (hGlbFcn),    // Defined function global memory handle
                                             lptkLftArg,                    // Ptr to left arg token (may be NULL if monadic)
                                             lpYYFcnStr,                    // Ptr to function strand
                                             lptkRhtArg);                   // Ptr to right arg token
                defstop
                    break;
            } // SWITCH

        case TKT_FCNNAMED:
            Assert (lpYYFcnStr->FcnCount EQ 1
                 || lpYYFcnStr->FcnCount EQ 2);

            // Check for axis operator
            if (lpYYFcnStr->FcnCount > 1
             && (lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
                lptkAxis = &lpYYFcnStr[1].tkToken;
            else
                lptkAxis = NULL;

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYFcnStr->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Get the global memory handle or function address if direct
            lpPrimFn = lpYYFcnStr->tkToken.tkData.tkSym->stData.stNameFcn;

            // Check for internal functions
            if (lpYYFcnStr->tkToken.tkData.tkSym->stFlags.FcnDir)
                return (*lpPrimFn) (lptkLftArg, &lpYYFcnStr->tkToken, lptkRhtArg, lptkAxis);

            // tkData is a valid HGLOBAL function array
            Assert (IsGlbTypeFcnDir (lpPrimFn));

            return ExecFuncGlb_EM_YY (lptkLftArg,
                                      ClrPtrTypeDirGlb (lpPrimFn),
                                      lptkRhtArg,
                                      lptkAxis);
        defstop
            return NULL;
    } // End SWITCH
} // End ExecFuncStr_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecOp1_EM_YY
//
//  Execute a monadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecOp1_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecOp1_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    // Split cases based upon the type of the monadic operator
    switch (lpYYFcnStrOpr->tkToken.tkData.tkChar)
    {
        case INDEX_OPSLASH:     // Reduction along the last coordinate
        case INDEX_OPSLASHBAR:  // Reduction along the first coordinate
            return PrimOpSlash_EM_YY (lptkLftArg,   // Ptr to left arg token (may be NULL if monadic)
                                      lpYYFcnStrOpr,// Ptr to operator function strand
                                      lptkRhtArg);  // Ptr to right arg token
        case INDEX_OPSLOPE:     // Scan along the last coordinate
        case INDEX_OPSLOPEBAR:  // Scan along the first coordinate
            return PrimOpSlope_EM_YY (lptkLftArg,   // Ptr to left arg token (may be NULL if monadic)
                                      lpYYFcnStrOpr,// Ptr to operator function strand
                                      lptkRhtArg);  // Ptr to right arg token
        case UTF16_STILETILDE:  // Partition
            DbgBrk ();          // ***FINISHME*** -- UTF16_STILETILDE






            return NULL;

        case UTF16_DIERESIS:    // Each
            return PrimOpDieresis_EM_YY (lptkLftArg,        // Ptr to left arg token (may be NULL if monadic)
                                         lpYYFcnStrOpr,     // Ptr to operator function strand
                                         lptkRhtArg);       // Ptr to right arg token
        case UTF16_DIERESISTILDE:   // Commute/Duplicate
            return PrimOpDieresisTilde_EM_YY (lptkLftArg,   // Ptr to left arg token (may be NULL if monadic)
                                              lpYYFcnStrOpr,// Ptr to operator function strand
                                              lptkRhtArg);  // Ptr to right arg token
        defstop
            return NULL;
    } // End SWITCH
} // End ExecOp1_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecOp2_EM_YY
//
//  Execute a dyadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecOp2_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecOp2_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStr,           // Ptr to function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    LPTOKEN lptkAxis;           // Ptr to axis token (may be NULL)

    // Check for axis operator
    if (lpYYFcnStr->FcnCount > 1
     && (lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
      || lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
        lptkAxis = &lpYYFcnStr[1].tkToken;
     else
        lptkAxis = NULL;

    // Split cases based upon the type of the dyadic operator
    switch (lpYYFcnStr->tkToken.tkData.tkChar)
    {
        case UTF16_JOTDOT:      // Outer product
            return PrimOpJotDot_EM_YY (lptkLftArg,  // Ptr to left arg token (may be NULL if monadic)
                                       lpYYFcnStr,  // Ptr to operator function strand
                                       lptkRhtArg); // Ptr to right arg token
        case UTF16_DOT:         // Inner product
            return PrimOpDot_EM_YY    (lptkLftArg,  // Ptr to left arg token (may be NULL if monadic)
                                       lpYYFcnStr,  // Ptr to operator function strand
                                       lptkRhtArg); // Ptr to right arg token
        case UTF16_JOT:         // Compose
            return PrimOpJot_EM_YY    (lptkLftArg,  // Ptr to left arg token (may be NULL if monadic)
                                       lpYYFcnStr,  // Ptr to operator function strand
                                       lptkRhtArg); // Ptr to right arg token
        case UTF16_DIERESISDEL: // Dual
            DbgBrk ();          // ***FINISHME*** -- UTF16_DIERESISDEL






            return NULL;        // break;

        case UTF16_DIERESISJOT: // Rank
            DbgBrk ();          // ***FINISHME*** -- UTF16_DIERESISJOT






            return NULL;        // break;

        case UTF16_DIERESISDOWNTACK:// Convolution
            DbgBrk ();          // ***FINISHME*** -- UTF16_DIERESISDOWNTACK






            return NULL;        // break;

        case UTF16_DIERESISSTAR:// Power
            DbgBrk ();          // ***FINISHME*** -- UTF16_DIERESISSTAR






            return NULL;        // break;

        defstop
            return NULL;
    } // End SWITCH
} // End ExecOp2_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $IsFcnOpr
//
//  Determine if a given token is a function or operator
//***************************************************************************

BOOL IsFcnOpr
    (LPTOKEN lptk)

{
    switch (TokenTypeFV (lptk))
    {
        case 'F':
        case '1':
        case '2':
            return TRUE;

        case 'V':
            return FALSE;

        defstop
            return 0;
    } // End SWITCH
} // End IsFcnOpr


//***************************************************************************
//  $TokenTypeFV
//
//  Return the type of a token
//
//  'F' = Function
//  'V' = Variable
//  '1' = Monadic operator
//  '2' = Dyadic operator
//  '?' = None of the above
//
//***************************************************************************

char TokenTypeFV
    (LPTOKEN lptk)

{
    // Split cases based upon the token type
    switch (lptk->tkFlags.TknType)
    {
        case TKT_VARNAMED:
        case TKT_STRING:
        case TKT_VARIMMED:
        case TKT_VARARRAY:
            return 'V';

        case TKT_FCNIMMED:
        case TKT_FCNARRAY:
        case TKT_FCNNAMED:
            return 'F';

        case TKT_OP1IMMED:
        case TKT_OP1NAMED:
        case TKT_OPJOTDOT:
            return '1';

        case TKT_OP2IMMED:
        case TKT_OP2NAMED:
            return '2';

        case TKT_COMMENT:
        case TKT_ASSIGN:
        case TKT_LISTSEP:
        case TKT_COLON:
        case TKT_LPAREN:
        case TKT_RPAREN:
        case TKT_LBRACKET:
        case TKT_RBRACKET:
        case TKT_EOS:
        case TKT_EOL:
        case TKT_LINECONT:
        case TKT_INPOUT:
        case TKT_STRAND:
        case TKT_LISTINT:
        case TKT_LISTPAR:
        case TKT_LISTBR:
        case TKT_AXISIMMED:
        case TKT_AXISARRAY:
        case TKT_STRNAMED:
        defstop
            return '?';
    } // End SWITCH
} // End TokenTypeFV


//***************************************************************************
//  $RankOfGlb
//
//  Get the rank of a global memory object
//***************************************************************************

APLRANK RankOfGlb
    (HGLOBAL hGlb)

{
    LPVOID  lpMem;
    APLRANK aplRank;

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlb);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    aplRank = lpHeader->Rank;

#undef  lpHeader

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMem = NULL;

    return aplRank;
} // End RankOfGlb


//***************************************************************************
//  $AttrsOfToken
//
//  Return the attributes (Type, NELM, and Rank) of a token.
//***************************************************************************

void AttrsOfToken
    (LPTOKEN   lpToken,         // Ptr to token
     APLSTYPE *lpaplType,       // Ptr to token storage type (may be NULL)
     LPAPLNELM lpaplNELM,       // ...          NELM (may be NULL)
     LPAPLRANK lpaplRank)       // ...          rank (may be NULL)

{
    HGLOBAL hGlbData;

    // Split case based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                // Get the global handle
                hGlbData = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbData));

                break;      // Continue with HGLOBAL case
            } // End IF

            if (lpaplType)
                *lpaplType = TranslateImmTypeToArrayType (lpToken->tkData.tkSym->stFlags.ImmType);
            if (lpaplNELM)
                *lpaplNELM = 1;
            if (lpaplRank)
                *lpaplRank = 0;
            return;

        case TKT_VARIMMED:
            if (lpaplType)
                *lpaplType = TranslateImmTypeToArrayType (lpToken->tkFlags.ImmType);
            if (lpaplNELM)
                *lpaplNELM = 1;
            if (lpaplRank)
                *lpaplRank = 0;
            return;

        case TKT_VARARRAY:
            // Get the global handle
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbData));

            break;      // Continue with HGLOBAL case

        case TKT_LISTPAR:
        case TKT_LISTBR:
            hGlbData = lpToken->tkData.tkGlbData;

            break;      // Continue with HGLOBAL case

        defstop
            return;
    } // End SWITCH

    // Get the attributes (Type, NELM, and Rank) of the global
    AttrsOfGlb (ClrPtrTypeDirGlb (hGlbData), lpaplType, lpaplNELM, lpaplRank, NULL);
} // End AttrsOfToken


//***************************************************************************
//  $AttrsOfGlb
//
//  Return the attributes (Type, NELM, and Rank) of a global memory handle.
//***************************************************************************

void AttrsOfGlb
    (HGLOBAL   hGlbData,        // Memory handle
     APLSTYPE *lpaplType,       // Ptr to token storage type (may be NULL)
     LPAPLNELM lpaplNELM,       // ...          NELM (may be NULL)
     LPAPLRANK lpaplRank,       // ...          rank (may be NULL)
     LPAPLUINT lpaplCols)       // ...          # cols in the array (scalar = 1) (may be NULL)

{
    LPVOID  lpMem;

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    if (lpaplType)
        *lpaplType = lpHeader->ArrType;
    if (lpaplNELM)
        *lpaplNELM = lpHeader->NELM;
    if (lpaplRank)
        *lpaplRank = lpHeader->Rank;

    // Skip over the header to the dimensions
    if (lpaplCols)
    {
        if (lpHeader->Rank NE 0)
        {
            lpMem = VarArrayBaseToDim (lpMem);
            *lpaplCols = ((LPAPLDIM) lpMem)[lpHeader->Rank - 1];
        } else
            *lpaplCols = 1;
    } // End IF

#undef  lpHeader

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;
} // End AttrsOfGlb


//***************************************************************************
//  $CheckAxisImm
//
//  Subroutine to CheckAxis_EM for immediate values
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckAxisImm"
#else
#define APPEND_NAME
#endif

BOOL CheckAxisImm
    (UINT       immType,        // Type of the immediate value
     APLLONGEST aplLongest,     // The immediate value
     LPTOKEN    lptkAxis,       // The Axis values
     APLRANK    aplRankCmp,     // Comparison rank
     LPBOOL     lpbFract,       // Return TRUE iff fractional values are present,
                                //   (may be NULL if fractional values not allowed)
     LPAPLINT   lpaplLastAxis,  // Return last axis value or ceiling if fractional
                                //   (may be NULL if caller is not interested)
     LPAPLNELM  lpaplNELMAxis,  // Return # elements in axis
                                //   (may be NULL if caller is not interested)
     HGLOBAL    *lphGlbAxis,    // Ptr to HGLOBAL where the cleaned up axis
                                //   is to be stored.  If the return is FALSE,
                                //   this ptr must be set to NULL.
                                //   (may be NULL if caller is not interested)
     LPAPLNELM  lpaplNELM,      // Local var w/NELM
     LPAPLINT  *lplpAxisStart,  // Ptr to ptr to start of Axis values in *lphGlbAxis
     LPAPLINT  *lplpAxisHead,   // ...                    user axis values in *lphGlbAxis
     LPAPLUINT  lpaplAxisContLo)// Contiguous low axis (not NULL)

{
    APLUINT      ByteAxis;      // # bytes for the axis global
    APLRANK      aplRank;       // Maximum rank for comparison
    BOOL         bRet = TRUE;   // TRUE iff result is valid
    UINT         u;             // Loop counter
    LPAPLINT     lpAxisTail;    // Ptr to grade up of AxisHead
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    APLBOOL      bQuadIO;       // []IO

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    bQuadIO = lpMemPTD->lpSymQuadIO->stData.stBoolean;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // The immediate value minus []IO
    //   must be an integer in the range
    //   [0, aplRankCmp - 1], inclusive.

    // Set # elements
    *lpaplNELM = 1;

    // Return the # elements
    if (lpaplNELMAxis NE NULL)
        *lpaplNELMAxis = *lpaplNELM;

    if (lphGlbAxis)
    {
        // Calculate space needed for axis
        // If the comparison rank is zero, the allocation
        //   size is zero, and the lock ptr is zero
        //   which GlobalLock will treat as an error,
        //   returning a zero ptr, so we max aplRankCmp with 1.
        ByteAxis = sizeof (APLINT) * 2 * max (aplRankCmp, 1);

        // Allocate storage for the axis vector
        // N.B. Conversion from APLUINT to UINT.
        Assert (ByteAxis EQ (UINT) ByteAxis);
        *lphGlbAxis = DbgGlobalAlloc (GHND, (UINT) ByteAxis);
        if (!*lphGlbAxis)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkAxis);
            return FALSE;
        } // End IF

        // Lock the memory to get a ptr it
        *lplpAxisStart = *lplpAxisHead = MyGlobalLock (*lphGlbAxis);

        // Point to the start of the trailing axes
        lpAxisTail = &(*lplpAxisHead)[aplRankCmp - *lpaplNELM];
    } // End IF

    // Split cases based upon the immediate axis type
    switch (lptkAxis->tkFlags.ImmType)
    {
        case IMMTYPE_BOOL:
        case IMMTYPE_INT:
            // Get the Boolean or Integer value,
            //   less the current index origin
            aplRank = aplLongest - bQuadIO;

            // Ensure it's within range
            // Note that because aplRank and aplRankCmp
            //   are unsigned, we don't need to check
            //   against zero
            bRet = (aplRank < aplRankCmp);

            break;

        case IMMTYPE_FLOAT:     // Ensure it's close enough
            // Convert the value to an integer using System CT
            aplRank = FloatToAplint_SCT (lptkAxis->tkData.tkFloat, &bRet);
            aplRank -= bQuadIO; // Less the index origin

            // If fractional values are allowed,
            //   return whether or not they are present
            if (lpbFract)
                *lpbFract = !bRet;

            // If fractional values allowed and are present, ...
            if (lpbFract && !bRet)
                bRet = TRUE;

            // Ensure it's within range
            // Note that because aplRank and aplRankCmp
            //   are unsigned, we don't need to check
            //   against zero
            bRet = bRet && (aplRank < aplRankCmp);

            break;

        case IMMTYPE_CHAR:      // No chars allowed
            return FALSE;

        defstop
            return FALSE;
    } // End SWITCH

    if (bRet && lphGlbAxis)
    {
        // Save the remaining values
        for (u = 0; u < aplRankCmp; u++)
        if (u NE aplRank)
            *(*lplpAxisHead)++ = u;

        // Save the trailing value
        *lpAxisTail++ = aplRank;
    } // End IF

    // Save the contiguous low axis
    *lpaplAxisContLo = aplRank;

    // Save the last (and only) value
    if (lpaplLastAxis)
        *lpaplLastAxis = aplRank;
    return bRet;
} // End CheckAxisImm
#undef  APPEND_NAME


//***************************************************************************
//  $CheckAxisGlb
//
//  Subroutine to CheckAxis_EM for global values
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckAxisGlb"
#else
#define APPEND_NAME
#endif

BOOL CheckAxisGlb
    (HGLOBAL    hGlbData,       // The global handle to check
     LPTOKEN    lptkAxis,       // The Axis values
     APLRANK    aplRankCmp,     // Comparison rank
     BOOL       bSingleton,     // TRUE iff scalar or one-element vector only
                                //   is allowed
     BOOL       bSortAxes,      // TRUE iff the axes should be sorted
                                //   (i.e., the order of the axes is unimportant)
     BOOL       bContiguous,    // TRUE iff the axes must be contiguous
     BOOL       bAllowDups,     // TRUE iff duplicate axes are allowed
     LPBOOL     lpbFract,       // Return TRUE iff fractional values are present,
                                //   (may be NULL if fractional values not allowed)
     LPAPLINT   lpaplLastAxis,  // Return last axis value or ceiling if fractional
                                //   (may be NULL if caller is not interested)
     LPAPLNELM  lpaplNELMAxis,  // Return # elements in axis
                                //   (may be NULL if caller is not interested)
     HGLOBAL   *lphGlbAxis,     // Ptr to HGLOBAL where the cleaned up axis
                                //   is to be stored.  If the return is FALSE,
                                //   this ptr must be set to NULL.
                                //   (may be NULL if caller is not interested)
     LPAPLNELM  lpaplNELM,      // Local var for NELM
     LPAPLINT  *lplpAxisStart,  // Ptr to ptr to start of Axis values in *lphGlbAxis
     LPAPLINT  *lplpAxisHead,   // ...                    user axis values in *lphGlbAxis
     LPAPLUINT  lpaplAxisContLo)// Contiguous low axis (not NULL)

{
    BOOL         bRet = TRUE;
    LPVOID       lpMem,
                 lpDup = NULL;
    HGLOBAL      hGlbDup = NULL;
    UINT         uBitMask;
    APLUINT      ByteDup,
                 ByteAxis,
                 u;
    APLSTYPE     aplTypeLcl;
    APLRANK      aplRankLcl;
    LPAPLINT     lpAxisTail;    // Ptr to grade up of AxisHead
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    APLBOOL      bQuadIO;       // []IO

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    bQuadIO = lpMemPTD->lpSymQuadIO->stData.stBoolean;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // st/tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (hGlbData));

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (ClrPtrTypeDirGlb (hGlbData));

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    aplTypeLcl = lpHeader->ArrType;
   *lpaplNELM  = lpHeader->NELM;
    aplRankLcl = lpHeader->Rank;

#undef  lpHeader

    // Check the axis rank and the NELM (if singletons only)
    if ((aplRankLcl > 1)
     || (bSingleton && *lpaplNELM NE 1))
        goto ERROR_EXIT;

    // Return the # elements
    if (lpaplNELMAxis NE NULL)
        *lpaplNELMAxis = *lpaplNELM;

    if (lphGlbAxis)
    {
        // Calculate space needed for axis
        // If the comparison rank is zero, the allocation
        //   size is zero, and the lock ptr is zero
        //   which GlobalLock will treat as an error,
        //   returning a zero ptr, so we max aplRankCmp with 1.
        ByteAxis = sizeof (APLINT) * 2 * max (aplRankCmp, 1);

        // Allocate storage for the axis vector
        // N.B. Conversion from APLUINT to UINT.
        Assert (ByteAxis EQ (UINT) ByteAxis);
        *lphGlbAxis = DbgGlobalAlloc (GHND, (UINT) ByteAxis);
        if (!*lphGlbAxis)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkAxis);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        *lplpAxisStart = *lplpAxisHead = MyGlobalLock (*lphGlbAxis);

        // Point to the start of the trailing axes
        lpAxisTail = &(*lplpAxisHead)[aplRankCmp - *lpaplNELM];
    } // End IF

    // If the comparison rank is zero, the allocation
    //   size is zero, and the lock ptr is zero
    //   which GlobalLock will treat as an error,
    //   returning a zero ptr, so we max aplRankCmp with 1.
    ByteDup = RoundUpBits8 (max (aplRankCmp, 1));

    // Allocate global memory bit vector to test for duplicates
    // N.B.  Conversion from APLUINT to UINT.
    Assert (ByteDup EQ (UINT) ByteDup);
    hGlbDup = DbgGlobalAlloc (GHND, (UINT) ByteDup);
    if (!hGlbDup)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkAxis);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to the
    //   duplicate indices testing area
    lpDup = MyGlobalLock (hGlbDup);

    // Skip over the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, aplRankLcl);

    // Split cases based upon the array type
    switch (aplTypeLcl)
    {
        case ARRAY_BOOL:
            uBitMask = 0x01;

            // Loop through the elements
            for (u = 0; bRet && u < *lpaplNELM; u++)
            {
                // Get the next bit value
                aplRankLcl = (uBitMask & *(LPAPLBOOL) lpMem) ? 1 : 0;
                aplRankLcl -= bQuadIO; // Less the index origin

                // Save the next trailing value
                //   if asked to and not sorting
                //   the axes.
                if (lphGlbAxis && !bSortAxes)
                    *lpAxisTail++ = aplRankLcl;

                // Test against the comparison rank
                bRet = (aplRankLcl < aplRankCmp);

                // Test for duplicates
                if (bRet)
                    bRet = TestDupAxis (lpDup, aplRankLcl, bAllowDups);

                if (bRet)
                {
                    // Shift over the bit mask
                    uBitMask <<= 1;

                    // Check for end-of-byte
                    if (uBitMask EQ END_OF_BYTE)
                    {
                        uBitMask = 0x01;        // Start over
                        ((LPAPLBOOL) lpMem)++;  // Skip to next byte
                    } // End IF
                } // End IF
            } // End FOR

            break;

        case ARRAY_INT:

#define lpaplInteger    ((LPAPLINT) lpMem)

            // Loop through the elements
            for (u = 0; bRet && u < *lpaplNELM; u++)
            {
                aplRankLcl = *lpaplInteger++ - bQuadIO;

                // Save the next trailing value
                //   if asked to and not sorting
                //   the axes.
                if (lphGlbAxis && !bSortAxes)
                    *lpAxisTail++ = aplRankLcl;

                // Ensure it's within range
                // Note that because aplRank and aplRankCmp
                //   are unsigned, we don't need to check
                //   against zero
                bRet = (aplRankLcl < aplRankCmp);

                // Test for duplicates
                if (bRet)
                    bRet = TestDupAxis (lpDup, aplRankLcl, bAllowDups);
            } // End FOR

#undef  lpaplInteger

            break;

        case ARRAY_FLOAT:

#define lpaplFloat      ((LPAPLFLOAT) lpMem)

            // Loop through the elements
            for (u = 0; bRet && u < *lpaplNELM; u++)
            {
                // Convert the value to an integer using System CT
                aplRankLcl = FloatToAplint_SCT (*lpaplFloat++, &bRet);
                aplRankLcl -= bQuadIO; // Less the index origin

                // If fractional values are allowed,
                //   return whether or not they are present
                if (lpbFract)
                    *lpbFract |= !bRet;

                // Save the next trailing value
                //   if asked to and not sorting
                //   the axes.
                if (lphGlbAxis && !bSortAxes)
                    *lpAxisTail++ = aplRankLcl;

                // If fractional values allowed and are present, ...
                if (lpbFract && !bRet)
                    bRet = TRUE;

                // Ensure it's within range
                // Note that because aplRank and aplRankCmp
                //   are unsigned, we don't need to check
                //   against zero
                bRet = bRet && (aplRankLcl < aplRankCmp);

                // Test for duplicates
                if (bRet)
                    bRet = TestDupAxis (lpDup, aplRankLcl, bAllowDups);
            } // End FOR

#undef  lpaplFloat

            break;

        case ARRAY_APA:
        {
            APLINT apaOff,
                   apaMul,
                   apaLen;

#define lpAPA       ((LPAPLAPA) lpMem)

            // Get the APA data
            apaOff = lpAPA->Off;
            apaMul = lpAPA->Mul;
            apaLen = *lpaplNELM;

#undef  lpAPA

            // Convert to origin-0
            apaOff -= bQuadIO;

            // It's sufficient to check the length and
            //   the first and last values for validity.
            bRet = (apaLen EQ 0)
                || (((apaOff + apaMul *           0 ) < (APLRANKSIGN) aplRankCmp)
                 && ((apaOff + apaMul * (apaLen - 1)) < (APLRANKSIGN) aplRankCmp));

            // Save the trailing axis values
            if (bRet)
            for (u = 0; bRet && u < (APLUINT) apaLen; u++)
            {
                // Get the next value
                aplRankLcl = apaOff + apaMul * u;

                // Save the next trailing value
                //   if asked to and not sorting
                //   the axes.
                if (lphGlbAxis && !bSortAxes)
                    *lpAxisTail++ = aplRankLcl;

                // Note there's no possibility of duplicates,
                //   however, we still need to fill in the
                //   bit array of values so we can fill in
                //   the leading axis values from the zeroes
                //   lpDup.
                // Test for duplicates
                if (bRet)
                    bRet = TestDupAxis (lpDup, aplRankLcl, bAllowDups);
            } // End FOR

            break;
        } // End ARRAY_APA

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
        case ARRAY_LIST:
            goto ERROR_EXIT;

            break;

        defstop
            break;
    } // End SWITCH

    // Save the last value
    if (lpaplLastAxis)
        *lpaplLastAxis = aplRankLcl;

    // Fill in the leading axis values
    if (bRet && lphGlbAxis && !bAllowDups)
    {
        uBitMask = 0x01;

        // Loop through the lpDup values looking for zeros
        for (u = 0; u < aplRankCmp; u++)
        {
            // If the bit is a zero, ...
            if (!(uBitMask & *(LPAPLBOOL) lpDup))
                // Save the next value
                *(*lplpAxisHead)++ = u;
            else            // the bit is a one
            if (bSortAxes)  // and we're sorting axes
                // Fill in the trailing axis values in order
                *lpAxisTail++ = u;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = 0x01;        // Start over
                ((LPAPLBOOL) lpDup)++;  // Skip to next byte
            } // End IF
        } // End FOR
    } // End IF

    // If the axes must be contiguous, check that
    if (bRet && bContiguous)
    {
        // Unlock and lock the memory to reset the
        //   ptr to the start
        MyGlobalUnlock (hGlbDup); lpDup = NULL;
        lpDup = MyGlobalLock (hGlbDup);

        uBitMask = 0x01;

        // Look for the first 1
        for (u = 0; u < aplRankCmp; u++)
        {
            // If it's a 1, break
            if (uBitMask & *(LPAPLBOOL) lpDup)
                break;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = 0x01;        // Start over
                ((LPAPLBOOL) lpDup)++;  // Skip to next byte
            } // End IF
        } // End FOR

        // Save as contiguous low axis
        *lpaplAxisContLo = u;

        // Look for consecutive 1s
        for (; u < aplRankCmp; u++)
        {
            // If it's a 0, break
            if (!(uBitMask & *(LPAPLBOOL) lpDup))
                break;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = 0x01;        // Start over
                ((LPAPLBOOL) lpDup)++;  // Skip to next byte
            } // End IF
        } // End FOR

        // Save as highest contiguous axis
        if (lpaplLastAxis)
            *lpaplLastAxis = u - 1;

        // Look for consecutive 0s
        for (; u < aplRankCmp; u++)
        {
            // If it's a 1, break
            if (uBitMask & *(LPAPLBOOL) lpDup)
                break;

            // Shift over the bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = 0x01;        // Start over
                ((LPAPLBOOL) lpDup)++;  // Skip to next byte
            } // End IF
        } // End FOR

        // If we're not at the end, the axes
        //   are not contiguous
        bRet = (u EQ aplRankCmp);
    } // End IF

    goto NORMAL_EXIT;

ERROR_EXIT:
    bRet = FALSE;
NORMAL_EXIT:
    if (hGlbDup)
    {
        if (lpDup)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbDup); lpDup = NULL;
        } // End IF

        // We no longer need this HGLOBAL
        DbgGlobalFree (hGlbDup); hGlbDup = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (ClrPtrTypeDirGlb (hGlbData)); lpMem = NULL;

    return bRet;
} // End CheckAxisGlb
#undef  APPEND_NAME


//***************************************************************************
//  $CheckAxis_EM
//
//  Check an axis value against a given rank
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckAxis_EM"
#else
#define APPEND_NAME
#endif

BOOL CheckAxis_EM
    (LPTOKEN    lptkAxis,       // The Axis values
     APLRANK    aplRankCmp,     // Comparison rank
     BOOL       bSingleton,     // TRUE iff scalar or one-element vector only
                                //   is allowed
     BOOL       bSortAxes,      // TRUE iff the axes should be sorted
                                //   (i.e., the order of the axes is unimportant)
     BOOL       bContiguous,    // TRUE iff the axes must be contiguous
     BOOL       bAllowDups,     // TRUE iff duplicate axes are allowed
     LPBOOL     lpbFract,       // Return TRUE iff fractional values are present,
                                //   (may be NULL if fractional values not allowed)
     LPAPLINT   lpaplLastAxis,  // Return last axis value or ceiling if fractional
                                //   (may be NULL if caller is not interested)
     LPAPLNELM  lpaplNELMAxis,  // Return # elements in axis
                                //   (may be NULL if caller is not interested)
     HGLOBAL    *lphGlbAxis)    // Ptr to HGLOBAL where the cleaned up axis
                                //   is to be stored.  If the return is FALSE,
                                //   this ptr must be set to NULL.
                                //   (may be NULL if caller is not interested)
{
    BOOL       bRet = TRUE;
    APLNELM    aplNELM;
    LPAPLINT   lpAxisStart,     // Ptr to ptr to start of Axis values in *lphGlbAxis
               lpAxisHead;      // ...                    user axis values in *lphGlbAxis
    UINT       u;
    APLUINT    aplAxisContLo;   // Contiguous low axis
    HGLOBAL    hGlbData = NULL;
    UINT       immType;
    APLLONGEST aplLongest;

    // Ensure the object is a subset of {iota}aplRankCmp
    //   with no duplicates, and not necessarily in order.

    // If the caller requests, an axis vector is allocated
    //   of length <2 * aplRankCmp>.  In the first <aplRankCmp>
    //   values, the trailing <*lpaplNELMAxis> values are
    //   the actual values in the axis (converted to origin-0
    //   APLINTs), and the leading values are all the other
    //   values in {iota}aplRankCmp.
    // The second <aplRankCmp> values are the grade-up of
    //   the first <aplRankCmp> values.

    // Mark as no fractional values
    if (lpbFract)
        *lpbFract = FALSE;
    // Split cases based upon the axis token type
    switch (lptkAxis->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkAxis->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, it's an HGLOBAL
            if (!lptkAxis->tkData.tkSym->stFlags.Imm)
            {
                // Get the global handle
                hGlbData = lptkAxis->tkData.tkSym->stData.stGlbData;
            } else
            {
                // Get the immediate type and value
                immType    = lptkAxis->tkData.tkSym->stFlags.ImmType;
                aplLongest = lptkAxis->tkData.tkSym->stData.stLongest;
            } // End IF/ELSE

            break;

        case TKT_VARIMMED:
        case TKT_AXISIMMED:
            // Get the immediate type and value
            immType    = lptkAxis->tkFlags.ImmType;
            aplLongest = lptkAxis->tkData.tkLongest;

            break;

        case TKT_VARARRAY:
        case TKT_AXISARRAY:
            // Get the global handle
            hGlbData = lptkAxis->tkData.tkGlbData;

            break;

        defstop
            break;
    } // End SWITCH

    if (hGlbData)
        // Handle the global case
        bRet = CheckAxisGlb (hGlbData,      // The global handl;e to check
                             lptkAxis,      // The axis values
                             aplRankCmp,    // Comparison rank
                             bSingleton,    // TRUE iff scalar or one-element vector only
                                            //   is allowed
                             bSortAxes,     // TRUE iff the axes should be sorted
                                            //   (i.e., the order of the axes is unimportant)
                             bContiguous,   // TRUE iff the axes must be contiguous
                             bAllowDups,    // TRUE iff duplicate axes are allowed
                             lpbFract,      // Return TRUE iff fractional values are present,
                                            //   (may be NULL if fractional values not allowed)
                             lpaplLastAxis, // Return last axis value or ceiling if fractional
                                            //   (may be NULL if caller is not interested)
                             lpaplNELMAxis, // Return # elements in axis
                                            //   (may be NULL if caller is not interested)
                             lphGlbAxis,    // Ptr to HGLOBAL where the cleaned up axis
                                            //   is to be stored.  If the return is FALSE,
                                            //   this ptr must be set to NULL.
                                            //   (may be NULL if caller is not interested)
                            &aplNELM,       // Local var for NELM
                            &lpAxisStart,   // Ptr to ptr to start of Axis values in *lphGlbAxis
                            &lpAxisHead,    // ...                    user axis values in *lphGlbAxis
                            &aplAxisContLo);// Contiguous low axis
    else
        // Handle the immediate case
        bRet = CheckAxisImm (immType,
                             aplLongest,
                             lptkAxis,
                             aplRankCmp,    // Comparison rank
                             lpbFract,      // Return TRUE iff fractional values are present,
                                            //   (may be NULL if fractional values not allowed)
                             lpaplLastAxis, // Return last axis value or ceiling if fractional
                                            //   (may be NULL if caller is not interested)
                             lpaplNELMAxis, // Return # elements in axis
                                            //   (may be NULL if caller is not interested)
                             lphGlbAxis,    // Ptr to HGLOBAL where the cleaned up axis
                                            //   is to be stored.  If the return is FALSE,
                                            //   this ptr must be set to NULL.
                                            //   (may be NULL if caller is not interested)
                            &aplNELM,       // Local var for NELM
                            &lpAxisStart,   // Ptr to ptr to start of Axis values in *lphGlbAxis
                            &lpAxisHead,    // ...                    user axis values in *lphGlbAxis
                            &aplAxisContLo);// Contiguous low axis
    // If bad values, it's an AXIS ERROR
    if (!bRet)
    {
        ErrorMessageIndirectToken (ERRMSG_AXIS_ERROR APPEND_NAME,
                                   lptkAxis);
        if (lphGlbAxis && *lphGlbAxis)
        {
            // We no longer need this ptr
            MyGlobalUnlock (*lphGlbAxis); lpAxisStart = lpAxisHead = NULL;

            // We no longer need this storage
            DbgGlobalFree (*lphGlbAxis); *lphGlbAxis = NULL;
        } // End IF
    } else
    if (lphGlbAxis && *lphGlbAxis)
    {
        if (bContiguous && !bAllowDups)
        {
            // If the axes are to be contiguous and no duplicates,
            //   this must be ravel (contiguous and duplicates is transpose)
            //   in which case the axes must be represented as
            //   A{is}{iota}aplRankCmp {diamond} A[X[{gradeup}X]]{is}X
            //   not as they are above for the dyadic scalar functions.

            // We accomplish this by forming the above result in the
            //   second half of lpAxisStart, and copying it to the
            //   first half.

            // A{is}{iota}aplRankCmp
            for (u = 0; u < aplRankCmp; u++)
                lpAxisStart[aplRankCmp + u] = u;

            // A[X[{gradeup}X]]{is}X
            for (u = 0; u < aplNELM; u++)
                lpAxisStart[aplRankCmp + aplAxisContLo + u] = lpAxisHead[u];

            // Copy back to the first half
            CopyMemory (lpAxisStart, &lpAxisStart[aplRankCmp], (UINT) (aplRankCmp * sizeof (lpAxisStart[0])));
        } // End IF

        // Place the grade-up of lpAxisStart into &lpAxisStart[aplRankCmp]
        if (bAllowDups)
            GradeUp (lpAxisStart, &lpAxisStart[aplRankCmp], aplRankCmp);
        else
            // Because these values are guaranteed to be a permutation
            //   vector, we can use an address sort.
            for (u = 0; u < aplRankCmp; u++)
                lpAxisStart[aplRankCmp + lpAxisStart[u]] = u;

        if (lphGlbAxis)
        {
            // We no longer need this ptr
            MyGlobalUnlock (*lphGlbAxis); lpAxisStart = lpAxisHead = NULL;
        } // End IF
    } // End IF/ELSE

    return bRet;
} // End CheckAxis_EM
#undef  APPEND_NAME


//***************************************************************************
//  $GradeUp
//
//  Grade up on a small number of APLUINTs
//***************************************************************************

void GradeUp
    (LPAPLUINT lpSrc,       // Source
     LPAPLUINT lpDst,       // Destination
     APLUINT   uCount)      // # APLUINTs in the source

{
    APLUINT u1, u2, u3;

    // Start with {iota}aplRankCmp in lpDst
    for (u1 = 0; u1 < uCount; u1++)
        lpDst[u1] = u1;

    // Use Gnome sort on lpDst while comparing lpSrc
    u2 = 1; u3 = 2;
    while (u2 < uCount)
    {
        if (lpSrc[lpDst[u2 - 1]] <= lpSrc[lpDst[u2]])
        {
            u2 = u3;
            u3++;
        } else
        {
            u1            = lpDst[u2 - 1];
            lpDst[u2 - 1] = lpDst[u2];
            lpDst[u2]     = u1;

            if (u2 NE 1)
                u2--;
        } // End IF/ELSE
    } // End WHILE
} // End GradeUp


//***************************************************************************
//  $TestDupAxis
//
//  Test an axis value for duplicates
//***************************************************************************

BOOL TestDupAxis
    (LPVOID  lpDup,
     APLRANK aplRank,
     BOOL    bAllowDups)

{
    BOOL bRet = TRUE;
    UINT uBitMask;

    // Calculate the bit mask
    uBitMask = (1 << (UINT) (aplRank % NBIB));

    // See if this value has already been seen
    if (!bAllowDups)
        bRet = !(uBitMask & ((LPAPLBOOL) lpDup)[aplRank >> LOG2NBIB]);

    // Set this value for the next time if necessary
    if (bRet || bAllowDups)
        ((LPAPLBOOL) lpDup)[aplRank >> LOG2NBIB] |= uBitMask;

    return bRet;
} // End TestDupAxis


//***************************************************************************
//  $StorageType
//
//  Return the storage type for a dyadic scalar function
//    given the left and right storage types and the function.
//***************************************************************************

APLSTYPE StorageType
    (APLSTYPE aplTypeLft,   // Storage type of left arg
     LPTOKEN  lptkFunc,     // Immediate function
     APLSTYPE aplTypeRht)   // Storage type of right arg

{
    static APLSTYPE aplTypeArr[ARRAY_LENGTH][ARRAY_LENGTH] =
    //      BOOL          INT           FLOAT         CHAR        HETERO        NESTED        LIST         APA
    {{ARRAY_BOOL  , ARRAY_INT   , ARRAY_FLOAT , ARRAY_MIXED , ARRAY_MIXED , ARRAY_NESTED, ARRAY_ERROR , ARRAY_INT   },  // BOOL
     {ARRAY_INT   , ARRAY_INT   , ARRAY_FLOAT , ARRAY_MIXED , ARRAY_MIXED , ARRAY_NESTED, ARRAY_ERROR , ARRAY_INT   },  // INT
     {ARRAY_FLOAT , ARRAY_FLOAT , ARRAY_FLOAT , ARRAY_MIXED , ARRAY_MIXED , ARRAY_NESTED, ARRAY_ERROR , ARRAY_FLOAT },  // FLOAT
     {ARRAY_MIXED , ARRAY_MIXED , ARRAY_MIXED , ARRAY_MIXED , ARRAY_MIXED , ARRAY_MIXED , ARRAY_ERROR , ARRAY_MIXED },  // CHAR
     {ARRAY_MIXED , ARRAY_MIXED , ARRAY_MIXED , ARRAY_MIXED , ARRAY_MIXED , ARRAY_MIXED , ARRAY_ERROR , ARRAY_MIXED },  // HETERO
     {ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_MIXED , ARRAY_MIXED , ARRAY_NESTED, ARRAY_ERROR , ARRAY_NESTED},  // NESTED
     {ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR },  // LIST
     {ARRAY_INT   , ARRAY_INT   , ARRAY_FLOAT , ARRAY_MIXED , ARRAY_MIXED , ARRAY_NESTED, ARRAY_ERROR , ARRAY_INT   },  // APA
    };

    APLSTYPE aplTypeRes;

    // Both arguments are within bounds
    Assert (aplTypeLft < ARRAY_LENGTH);
    Assert (aplTypeRht < ARRAY_LENGTH);

    // tkData is an immediate function
    Assert (lptkFunc->tkFlags.TknType EQ TKT_FCNIMMED);

    // Get the common storage type
    aplTypeRes = aplTypeArr[aplTypeLft][aplTypeRht];

    // Split cases based upon the result type
    switch (aplTypeRes)
    {
        case ARRAY_ERROR:
            return aplTypeRes;

        case ARRAY_MIXED:
            // A mixed result is from CHAR or HETERO vs. numeric (BOOL, INT, APA, FLOAT) or NESTED
            // The result is BOOL or NESTED if the function is equal or not-equal, otherwise
            //   the result is ERROR.
            if (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
             || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL)
            {
                if (aplTypeLft EQ ARRAY_NESTED
                 || aplTypeRht EQ ARRAY_NESTED)
                    return ARRAY_NESTED;
                else
                    return ARRAY_BOOL;
            } else
                return ARRAY_ERROR;

        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
            if (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
             || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL)
                return ARRAY_BOOL;
            else
                return aplTypeRes;

        case ARRAY_NESTED:
            return aplTypeRes;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_APA:
        defstop
            return (APLSTYPE) -1;
    } // End SWITCH
} // StorageType


//***************************************************************************
//  $FirstValue
//
//  Return the first value from a token as either
//    both an integer and a float, or as a character,
//    or as an LPSYMENTRY/HGLOBAL.  The token may be
//    an empty array in which case the value of the
//    prototype is returned.
//***************************************************************************

void FirstValue
    (LPTOKEN      lpToken,      // The token
     LPAPLINT     lpaplInteger, // Return the integer (or Boolean) (may be NULL)
     LPAPLFLOAT   lpaplFloat,   // ...        float (may be NULL)
     LPAPLCHAR    lpaplChar,    // ...        char (may be NULL)
     LPAPLLONGEST lpaplLongest, // ...        longest (may be NULL)
     LPVOID      *lpSymGlb,     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
     LPUCHAR      lpImmType,    // ...        immediate type:  IMMTYPES (may be NULL)
     LPAPLSTYPE   lpArrType)    // ...        array type:  ARRAY_TYPES (may be NULL)

{
    HGLOBAL hGlbData;

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbData));

                break;      // Continue below with global case
            } // End IF

            // Handle the immediate case
            FirstValueImm (lpToken->tkData.tkSym->stFlags.ImmType,
                           lpToken->tkData.tkSym->stData.stLongest,
                           lpaplInteger,
                           lpaplFloat,
                           lpaplChar,
                           lpaplLongest,
                           lpSymGlb,
                           lpImmType,
                           lpArrType);
            return;

        case TKT_VARIMMED:
            // Handle the immediate case
            FirstValueImm (lpToken->tkFlags.ImmType,
                           lpToken->tkData.tkLongest,
                           lpaplInteger,
                           lpaplFloat,
                           lpaplChar,
                           lpaplLongest,
                           lpSymGlb,
                           lpImmType,
                           lpArrType);
            return;

        case TKT_VARARRAY:
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbData));

            break;      // Continue below with global case

        defstop
            return;
    } // End SWITCH

    // Handle the LPSYMENTRY/HGLOBAL case
    FirstValueSymGlb (hGlbData,
                      lpaplInteger,
                      lpaplFloat,
                      lpaplChar,
                      lpaplLongest,
                      lpSymGlb,
                      lpImmType,
                      lpArrType);
} // End FirstValue


//***************************************************************************
//  $FirstValueImm
//
//  Return the first value of an immediate
//***************************************************************************

void FirstValueImm
    (UINT         immType,      // The token's immediate type
     APLLONGEST   aplLongest,   // The longest immediate value
     LPAPLINT     lpaplInteger, // Return the integer (or Boolean) (may be NULL)
     LPAPLFLOAT   lpaplFloat,   // ...        float (may be NULL)
     LPAPLCHAR    lpaplChar,    // ...        char (may be NULL)
     LPAPLLONGEST lpaplLongest, // ...        longest (may be NULL)
     LPVOID      *lpSymGlb,     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
     LPUCHAR      lpImmType,    // ...        immediate type:  IMMTYPES (may be NULL)
     LPAPLSTYPE   lpArrType)    // ...        array type:  ARRAY_TYPES (may be NULL)
{
    if (lpImmType)
        *lpImmType    = immType;
    if (lpArrType)
        *lpArrType    = TranslateImmTypeToArrayType (immType);
    if (lpSymGlb)
        *lpSymGlb     = NULL;
    if (lpaplLongest)
        *lpaplLongest = aplLongest;

    // Split cases based upon the immediate type
    switch (immType)
    {
        case IMMTYPE_BOOL:
            if (lpaplInteger)
                *lpaplInteger = (APLBOOL) aplLongest;
            if (lpaplFloat)
                *lpaplFloat   = (APLFLOAT) (APLBOOL) aplLongest;
            if (lpaplChar)
                *lpaplChar    = L'\0';
            break;

        case IMMTYPE_INT:
            if (lpaplInteger)
                *lpaplInteger = (APLINT) aplLongest;
            if (lpaplFloat)
                // ***FIXME*** -- Possible loss of precision
                *lpaplFloat   = (APLFLOAT) (APLINT) aplLongest;
            if (lpaplChar)
                *lpaplChar    = L'\0';
            break;

        case IMMTYPE_FLOAT:
            if (lpaplFloat)
                *lpaplFloat   = *(LPAPLFLOAT) &aplLongest;
            if (lpaplInteger)
                *lpaplInteger = (APLINT) *(LPAPLFLOAT) &aplLongest;
            if (lpaplChar)
                *lpaplChar    = L'\0';
            break;

        case IMMTYPE_CHAR:
            if (lpaplInteger)
                *lpaplInteger = 0;
            if (lpaplFloat)
                *lpaplFloat   = 0;
            if (lpaplChar)
                *lpaplChar    = (APLCHAR) aplLongest;
            break;

        defstop
            break;
    } // End SWITCH
} // End FirstValueImm


//***************************************************************************
//  $FirstValueSymGlb
//
//  Return the first value from an LPSYMENTRY/HGLOBAL as either
//    both an integer and a float, or as a character,
//    or as an LPSYMENTRY/HGLOBAL.  The HGLOBAL may be
//    an empty array in which case the value of the
//    prototype is returned.
//***************************************************************************

void FirstValueSymGlb
    (HGLOBAL      hGlbData,     // The global memory handle
     LPAPLINT     lpaplInteger, // Return the integer (or Boolean) (may be NULL)
     LPAPLFLOAT   lpaplFloat,   // ...        float (may be NULL)
     LPAPLCHAR    lpaplChar,    // ...        char (may be NULL)
     LPAPLLONGEST lpaplLongest, // ...        longest (may be NULL)
     LPVOID      *lpSymGlb,     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
     LPUCHAR      lpImmType,    // ...        immediate type IMMTYPES (may be NULL)
     LPAPLSTYPE   lpArrType)    // ...        array type -- ARRAY_TYPES (may be NULL)

{
    LPVOID     lpMem;
    APLSTYPE   aplType;
    APLNELM    aplNELM;
    APLINT     aplInteger;
    APLFLOAT   aplFloat;
    APLCHAR    aplChar;
    LPSYMENTRY lpSym;

    // Split cases based upon the ptr type
    switch (GetPtrTypeDir (hGlbData))
    {
        case PTRTYPE_STCONST:
            FirstValueImm (((LPSYMENTRY) hGlbData)->stFlags.ImmType,
                           ((LPSYMENTRY) hGlbData)->stData.stLongest,
                           lpaplInteger,
                           lpaplFloat,
                           lpaplChar,
                           lpaplLongest,
                           lpSymGlb,
                           lpImmType,
                           lpArrType);
            return;

        case PTRTYPE_HGLOBAL:
            hGlbData = ClrPtrTypeDirGlb (hGlbData);

            break;

        defstop
            break;
    } // End SWITCH

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    // Get the array's storage type & NELM
    aplType = lpHeader->ArrType;
    aplNELM = lpHeader->NELM;

    // Skip over the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, lpHeader->Rank);

#undef  lpHeader

    if (lpImmType)
        *lpImmType = TranslateArrayTypeToImmType (aplType);
    if (lpArrType)
        *lpArrType = aplType;

    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            // If the array is empty and numeric, the value must be Boolean or APA
            aplInteger = aplNELM ? (BIT0 & *(LPAPLBOOL) lpMem) : 0;

            if (lpaplInteger)
                *lpaplInteger = aplInteger;
            if (lpaplFloat)
                *lpaplFloat   = (APLFLOAT) aplInteger;
            if (lpaplChar)
                *lpaplChar    = L'\0';
            if (lpaplLongest)
                *lpaplLongest = aplInteger;
            if (lpSymGlb)
                *lpSymGlb = NULL;
            break;

        case ARRAY_INT:
            Assert (aplNELM > 0);

            aplInteger = *(LPAPLINT) lpMem;

            if (lpaplInteger)
                *lpaplInteger = aplInteger;
            if (lpaplFloat)
                *lpaplFloat   = (APLFLOAT) aplInteger;  // ***FIXME*** -- Possible loss of precision
            if (lpaplChar)
                *lpaplChar    = L'\0';
            if (lpaplLongest)
                *lpaplLongest = aplInteger;
            if (lpSymGlb)
                *lpSymGlb = NULL;
            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMem)
            // If the array is empty and numeric, the value must be Boolean or APA
            aplInteger = aplNELM ? lpAPA->Off : 0;
#undef  lpAPA
            if (lpaplInteger)
                *lpaplInteger = aplInteger;
            if (lpaplFloat)
                *lpaplFloat   = (APLFLOAT) aplInteger;  // ***FIXME*** -- Possible loss of precision
            if (lpaplChar)
                *lpaplChar    = L'\0';
            if (lpaplLongest)
                *lpaplLongest = aplInteger;
            if (lpSymGlb)
                *lpSymGlb = NULL;
            break;

        case ARRAY_FLOAT:
            Assert (aplNELM > 0);

            aplFloat = *(LPAPLFLOAT) lpMem;

            if (lpaplFloat)
                *lpaplFloat   = aplFloat;
            if (lpaplInteger)
                *lpaplInteger = (APLINT) aplFloat;
            if (lpaplChar)
                *lpaplChar    = L'\0';
            if (lpaplLongest)
                *lpaplLongest = *(LPAPLLONGEST) &aplFloat;
            if (lpSymGlb)
                *lpSymGlb = NULL;
            break;

        case ARRAY_CHAR:
            // If the array is empty and char, the value must be blank
            aplChar = aplNELM ? (*(LPAPLCHAR) lpMem) : L' ';

            if (lpaplInteger)
                *lpaplInteger = 0;
            if (lpaplFloat)
                *lpaplFloat   = 0;
            if (lpaplChar)
                *lpaplChar    = aplChar;
            if (lpaplLongest)
                *lpaplLongest = aplChar;
            if (lpSymGlb)
                *lpSymGlb = NULL;
            break;

        case ARRAY_NESTED:
            // If it's an HGLOBAL, ...
            if (GetPtrTypeInd (lpMem) EQ PTRTYPE_HGLOBAL)
            {
                if (lpaplInteger)
                    *lpaplInteger = 0;
                if (lpaplFloat)
                    *lpaplFloat   = 0;
                if (lpaplChar)
                    *lpaplChar    = L'\0';
                if (lpaplLongest)
                    *lpaplLongest = 0;
                if (lpSymGlb)
                    *lpSymGlb = *(LPAPLNESTED) lpMem;
                break;
            } // End IF

            // Fall through to ARRAY_HETERO case

        case ARRAY_HETERO:
            Assert ((aplType EQ ARRAY_NESTED) || aplNELM > 0);

            lpSym = *(LPAPLHETERO) lpMem;

            // stData is immediate
            Assert (lpSym->stFlags.Imm);

            // Handle the immediate case
            FirstValueImm (lpSym->stFlags.ImmType,
                           lpSym->stData.stLongest,
                           lpaplInteger,
                           lpaplFloat,
                           lpaplChar,
                           lpaplLongest,
                           lpSymGlb,
                           lpImmType,
                           lpArrType);
            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;
} // End FirstValueSymGlb


//***************************************************************************
//  $GetValueIntoToken
//
//  Get the next value from a variable into a token
//***************************************************************************

void GetValueIntoToken
    (APLUINT  uArg,         // Index to use
     LPVOID   lpMemArg,     // Ptr to global memory object to index
     APLSTYPE aplTypeArg,   // Storage type of the arg
     APLINT   apaOff,       // APA offset (if needed)
     APLINT   apaMul,       // APA multiplier (if needed)
     LPTOKEN  lptkArg)      // Ptr to token in which to place the value

{
    // Clear the NoDisplay flag
    lptkArg->tkFlags.NoDisplay = 0;

    // Split cases based upon the arg storage type
    switch (aplTypeArg)
    {
        case ARRAY_BOOL:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_BOOL;
            lptkArg->tkData.tkInteger = BIT0 & (UCHAR) (((LPAPLBOOL) lpMemArg)[uArg >> LOG2NBIB] >> (uArg & MASKLOG2NBIB));

            break;

        case ARRAY_INT:
            lptkArg->tkFlags.TknType      = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_INT;
            lptkArg->tkData.tkInteger = ((LPAPLINT) lpMemArg)[uArg];

            break;

        case ARRAY_FLOAT:
            lptkArg->tkFlags.TknType      = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_FLOAT;
            lptkArg->tkData.tkFloat   = ((LPAPLFLOAT) lpMemArg)[uArg];

            break;

        case ARRAY_CHAR:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_CHAR;
            lptkArg->tkData.tkChar    = ((LPAPLCHAR) lpMemArg)[uArg];

            break;

        case ARRAY_APA:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_INT;
            lptkArg->tkData.tkInteger = (apaOff + apaMul * uArg);

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Split cases based upon the ptr type
            switch (GetPtrTypeDir (((LPAPLNESTED) lpMemArg)[uArg]))
            {
                case PTRTYPE_STCONST:
                    lptkArg->tkFlags.TknType  = TKT_VARIMMED;
                    lptkArg->tkFlags.ImmType  = ((LPSYMENTRY *) lpMemArg)[uArg]->stFlags.ImmType;
                    lptkArg->tkData.tkLongest = ((LPSYMENTRY *) lpMemArg)[uArg]->stData.stLongest;

                    break;

                case PTRTYPE_HGLOBAL:
                    lptkArg->tkFlags.TknType  = TKT_VARARRAY;
                    lptkArg->tkFlags.ImmType  = 0;
                    lptkArg->tkData.tkGlbData = CopySymGlbDir (((LPAPLNESTED) lpMemArg)[uArg]);

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH
} // End GetValueIntoToken


//***************************************************************************
//  $GetGlbHandle
//
//  Return the HGLOBAL from a token if it's an HGLOBAL
//    NULL otherwise.
//***************************************************************************

HGLOBAL GetGlbHandle
    (LPTOKEN lpToken)           // Ptr to token

{
    HGLOBAL hGlb;

    // Get the global handle w/o locking it
    GetGlbPtrs_LOCK (lpToken, &hGlb, NULL);

    return hGlb;
} // End GetGlbHandle


//***************************************************************************
//  $GetGlbPtrs_LOCK
//
//  Return the HGLOBAL and LPVOID from a token if it's an HGLOBAL
//    NULL otherwise.
//  If the arg is immediate, return the APLLONGEST value.
//
//  The "_LOCK" suffix is a reminder that this function locks
//    a global memory handle and the caller must unlock it.
//***************************************************************************

APLLONGEST GetGlbPtrs_LOCK
    (LPTOKEN  lpToken,          // Ptr to token
     HGLOBAL *lphGlb,           // Ptr to ptr to HGLOBAL
     LPVOID  *lplpMem)          // Ptr to ptr to memory (may be NULL)

{
    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                *lphGlb = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (*lphGlb));

                break;      // Continue with common HGLOBAL code
            } // End IF

            // Handle the immediate case
            *lphGlb  = NULL;

            if (lplpMem)
                *lplpMem = NULL;

            return lpToken->tkData.tkSym->stData.stLongest;

        case TKT_VARIMMED:
            *lphGlb  = NULL;

            if (lplpMem)
                *lplpMem = NULL;

            return lpToken->tkData.tkLongest;

        case TKT_VARARRAY:
        case TKT_LISTBR:
            *lphGlb = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (*lphGlb));

            break;      // Continue with common HGLOBAL code

        defstop
            break;
    } // End SWITCH

    // Handle the HGLOBAL case
    if (*lphGlb)
    {
        *lphGlb = ClrPtrTypeDirGlb (*lphGlb);

        // Lock the memory to get a ptr to it
        if (lplpMem)
            *lplpMem = MyGlobalLock (*lphGlb);
    } // End IF

    return (APLLONGEST) 0;
} // End GetGlbPtrs_LOCK


//***************************************************************************
//  $GetNextInteger
//
//  Return the next value as an integer
//***************************************************************************

APLINT GetNextInteger
    (LPVOID   lpMem,
     APLSTYPE aplType,
     APLINT   uRes,
     APLINT   apaOff,
     APLINT   apaMul)

{
    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            return BIT0 & (((LPAPLBOOL) lpMem)[uRes >> LOG2NBIB] >> (uRes & MASKLOG2NBIB));

        case ARRAY_INT:
            return ((LPAPLINT) lpMem)[uRes];

        case ARRAY_FLOAT:
            return (APLINT) ((LPAPLFLOAT) lpMem)[uRes];

        case ARRAY_APA:
            return apaOff + apaMul * uRes;

        defstop
            return 0;
    } // End SWITCH
} // End GetNextInteger


//***************************************************************************
//  $GetNextFloat
//
//  Return the next value as a float
//***************************************************************************

APLFLOAT GetNextFloat
    (LPVOID   lpMem,
     APLSTYPE aplType,
     APLINT   uRes,
     APLINT   apaOff,
     APLINT   apaMul)

{
    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            return (APLFLOAT) (BIT0 & (((LPAPLBOOL) lpMem)[uRes >> LOG2NBIB] >> (uRes & MASKLOG2NBIB)));

        case ARRAY_INT:
            return (APLFLOAT) ((LPAPLINT) lpMem)[uRes];

        case ARRAY_FLOAT:
            return ((LPAPLFLOAT) lpMem)[uRes];

        case ARRAY_APA:
            return (APLFLOAT) (apaOff + apaMul * uRes); // ***FIXME*** -- Possible loss of precision

        defstop
            return 0;
    } // End SWITCH
} // End GetNextFloat


//***************************************************************************
//  $GetNextHetero
//
//  Return the next value as Boolean/Integer, Float, or Char
//***************************************************************************

APLSTYPE GetNextHetero
    (LPAPLHETERO lpMem,
     APLINT      u,
     LPAPLINT    lpaplInteger,
     LPAPLFLOAT  lpaplFloat,
     LPAPLCHAR   lpaplChar)

{
    LPSYMENTRY lpSymEntry;

    // Get the next LPSYMENTRY
    lpSymEntry = lpMem[u];

    // stData is immediate
    Assert (lpSymEntry->stFlags.Imm);

    // Split cases based upon the immediate storage type
    switch (lpSymEntry->stFlags.ImmType)
    {
        case IMMTYPE_BOOL:
            *lpaplInteger = lpSymEntry->stData.stBoolean;
            *lpaplFloat   = (APLFLOAT) *lpaplInteger;  // ***FIXME*** -- Possible loss of precision

            return ARRAY_BOOL;

        case IMMTYPE_INT:
            *lpaplInteger = lpSymEntry->stData.stInteger;
            *lpaplFloat   = (APLFLOAT) *lpaplInteger;  // ***FIXME*** -- Possible loss of precision

            return ARRAY_INT;

        case IMMTYPE_FLOAT:
            *lpaplFloat   = lpSymEntry->stData.stFloat;

            return ARRAY_FLOAT;

        case IMMTYPE_CHAR:
            *lpaplChar    = lpSymEntry->stData.stChar;

            return ARRAY_CHAR;

        defstop
            return ARRAY_ERROR;
    } // End SWITCH
} // End GetNextHetero


//***************************************************************************
//  $IncrOdometer
//
//  Increment the odometer in lpMemOdo subject to the
//    limits in lpMemDim[lpMemAxis] or lpMemDim, all
//    of length <aplRank>.
//***************************************************************************

void IncrOdometer
    (LPAPLUINT lpMemOdo,    // Ptr to the values to increment
     LPAPLDIM  lpMemDim,    // Ptr to the limits on each dimension
     LPAPLINT  lpMemAxis,   // Ptr to optional index vector (may be NULL)
     APLRANK   aplRank)     // # elements to which each of the
                            //   ptrs point

{
    APLINT iOdo;

    // Loop through the odometer values from right to left
    // Note we use a signed index variable because we're
    //   walking backwards and the test against zero must be
    //   made as a signed variable.
    if (lpMemAxis)
    {
        for (iOdo = aplRank - 1; iOdo >= 0; iOdo--)
        if (++lpMemOdo[iOdo] EQ lpMemDim[lpMemAxis[iOdo]])
            lpMemOdo[iOdo] = 0;
        else
            break;
    } else
    {
        for (iOdo = aplRank - 1; iOdo >= 0; iOdo--)
        if (++lpMemOdo[iOdo] EQ lpMemDim[iOdo])
            lpMemOdo[iOdo] = 0;
        else
            break;
    } // End IF/ELSE
} // End IncrOdometer


//***************************************************************************
//  $PrimScalarFnDydAllocate_EM
//
//  Allocate space for result of a primitive scalar dyadic function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimScalarFnDydAllocate_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimScalarFnDydAllocate_EM
    (LPTOKEN    lptkFunc,       // Ptr to function token

     HGLOBAL   *lphGlbRes,      // Return HGLOBAL of the result

     LPVOID     lpMemLft,       // Ptr to left arg memory (may be NULL if monadic)
     LPVOID     lpMemRht,       // ...    right ...

     APLRANK    aplRankLft,     // Left arg rank
     APLRANK    aplRankRht,     // Right ...
     LPAPLRANK  lpaplRankRes,   // Ptr to result rank

     APLSTYPE   aplTypeRes,     // Result type

     APLNELM    aplNELMLft,     // Left arg NELM
     APLNELM    aplNELMRht,     // Right ...
     APLNELM    aplNELMRes)     // Result ...

{
    APLUINT  ByteRes;           // # bytes needed for the result
    LPVOID   lpMemRes;          // Ptr to locked memory in result
    LPAPLDIM lpMemDimArg;
    APLUINT  uRes;
    LPVOID   lpMem;

    // Split cases based upon the singleton status
    //   of the left & right args
    if (aplNELMLft EQ 1
     && aplNELMRht EQ 1)
    {
        //***************************************************************
        // Both args are singletons
        //***************************************************************

        // The rank of the result is the larger of the two args
        *lpaplRankRes = max (aplRankLft, aplRankRht);

        // If it's a simple numeric scalar, it'll
        //   be stored as an immediate token,
        //   not global memory.
        if (*lpaplRankRes EQ 0
         && IsSimpleNum (aplTypeRes))
            return TRUE;
    } else
    if (aplNELMLft EQ 1
     || aplNELMRht EQ 1)
    {
        //***************************************************************
        // One of the args is a singleton, the other not
        //***************************************************************

        // The rank of the result is the rank of the non-singleton
        if (aplNELMLft NE 1)
            *lpaplRankRes = aplRankLft;
        else
            *lpaplRankRes = aplRankRht;
    } else
    {
        //***************************************************************
        // Neither arg is a singleton
        //***************************************************************

        // The rank of the result is the larger of the two args
        *lpaplRankRes = max (aplRankLft, aplRankRht);
    } // End IF/ELSE/...

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, *lpaplRankRes);

    // Allocate space for the result.
    // N.B. Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    *lphGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!*lphGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return FALSE;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (*lphGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;               // Already zero from GHND
////lpHeader->SysVar     = 0;               // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = *lpaplRankRes;

#undef  lpHeader

    // Fill in the dimensions
    if (aplNELMLft EQ 1
     && aplNELMRht EQ 1)
    {
        // Fill in the dimensions (all 1)
        for (uRes = 0; uRes < *lpaplRankRes; uRes++)
            (VarArrayBaseToDim (lpMemRes))[uRes] = 1;
    } else
    if (aplNELMLft EQ 1
     || aplNELMRht EQ 1)
    {
        // Copy the ptr to the non-singleton argument
        if (aplNELMLft NE 1)
            lpMemDimArg = lpMemLft;
        else
            lpMemDimArg = lpMemRht;

        // Copy the dimensions from the non-singleton arg
        //   to the result
        for (uRes = 0; uRes < *lpaplRankRes; uRes++)
            (VarArrayBaseToDim (lpMemRes))[uRes] = (VarArrayBaseToDim (lpMemDimArg))[uRes];
    } else
    {
        // Get the higher rank's pointer
        //   to copy its dimensions to the result
        if (aplRankLft > aplRankRht)
            lpMemDimArg = lpMemLft;
        else
            lpMemDimArg = lpMemRht;

        // Copy the dimensions to the result
        for (uRes = 0; uRes < *lpaplRankRes; uRes++)
            (VarArrayBaseToDim (lpMemRes))[uRes] = (VarArrayBaseToDim (lpMemDimArg))[uRes];
    } // End IF/ELSE/...

    // Fill nested result with PTR_REUSED
    //   in case we fail part way through
    if (aplTypeRes EQ ARRAY_NESTED)
    {
        // Skip over the header and dimensions to the data
        lpMem = VarArrayBaseToData (lpMemRes, *lpaplRankRes);

        // Fill in the prototype
        *((LPAPLNESTED) lpMem) = PTR_REUSED;
        for (uRes = 1; uRes < aplNELMRes; uRes++)
            ((LPAPLNESTED) lpMem)[uRes] = PTR_REUSED;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;

    return TRUE;
} // End PrimScalarFnDydAllocate_EM
#undef  APPEND_NAME


//***************************************************************************
//  $MakeMonPrototype_EM
//
//  Make a prototype copy of a single global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeMonPrototype_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL MakeMonPrototype_EM
    (HGLOBAL   hGlbArr,             // Incoming array handle
     LPTOKEN   lptkFunc,            // Ptr to function token
     MAKEPROTO mpEnum)              // See MAKEPROTO enum

{
    LPVOID      lpMemArr,
                lpMemRes;
    LPVARARRAY_HEADER lpHeader;
    APLSTYPE    aplType;
    APLNELM     aplNELM;
    APLRANK     aplRank;
    UINT        u;
    APLNELM     uLen;
    HGLOBAL     hGlbTmp;
    BOOL        bRet = TRUE;        // TRUE iff result is valid
    APLUINT     ByteRes;            // # bytes needed for the result
    LPSYMENTRY  lpSymArr,
                lpSymRes;

    DBGENTER;

    // Make a copy of the array as we're changing it
    hGlbArr = CopyArray_EM (hGlbArr,
                            TRUE,
                            lptkFunc);
    if (!hGlbArr)
        return NULL;

    // Lock the memory to get a ptr to it
    lpMemArr = lpHeader = MyGlobalLock (hGlbArr);

    aplType = lpHeader->ArrType;
    aplNELM = lpHeader->NELM;
    aplRank = lpHeader->Rank;

    // Point to the data
    lpMemArr = VarArrayBaseToData (lpMemArr, aplRank);

    // Split cases based upon the array type
    switch (aplType)
    {
        case ARRAY_BOOL:
            // Calculate # bytes of Boolean data
            uLen = RoundUpBits8 (aplNELM);

            for (u = 0; u < uLen; u++)
                *((LPAPLBOOL)  lpMemArr)++ = 0x00;
            break;

        case ARRAY_INT:
            for (u = 0; u < aplNELM; u++)
                *((LPAPLINT)   lpMemArr)++ = 0;
            break;

        case ARRAY_FLOAT:
            for (u = 0; u < aplNELM; u++)
                *((LPAPLFLOAT) lpMemArr)++ = 0;
            break;

        case ARRAY_CHAR:
            // Split cases based upon mpEnum
            switch (mpEnum)
            {
                case MP_CHARS:
                    break;

                case MP_NUMONLY:
                    // If the arg is non-empty, that's an error
                    if (aplNELM NE 0)
                        goto DOMAIN_ERROR_EXIT;

                    // Change the storage type to Boolean
                    lpHeader->ArrType = ARRAY_BOOL;

                    break;

                case MP_NUMCONV:
                    // Convert the chars to numerics by allocating
                    //   new (Boolean) storage and copying over
                    //   the dimensions

                    // Calculate space needed for the result
                    ByteRes = CalcArraySize (ARRAY_BOOL, aplNELM, aplRank);

                    // Allocate space for the result.
                    // N.B. Conversion from APLUINT to UINT.
                    Assert (ByteRes EQ (UINT) ByteRes);
                    hGlbTmp = DbgGlobalAlloc (GHND, (UINT) ByteRes);
                    if (!hGlbTmp)
                    {
                        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                                   lptkFunc);
                        goto ERROR_EXIT;
                    } // End IF

                    // Lock the memory to get a ptr to it
                    lpMemRes = MyGlobalLock (hGlbTmp);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

                    // Fill in the header
                    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                    lpHeader->ArrType    = ARRAY_BOOL;
////////////////////lpHeader->Perm       = 0;   // Already zero from GHND
////////////////////lpHeader->SysVar     = 0;   // Already zero from GHND
                    lpHeader->RefCnt     = 1;
                    lpHeader->NELM       = aplNELM;
                    lpHeader->Rank       = aplRank;

#undef  lpHeader

                    // Copy the dimensions to the result
                    CopyMemory (VarArrayBaseToDim (lpMemRes),
                                VarArrayBaseToDim (lpHeader),
                                (UINT) aplRank * sizeof (APLDIM));

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbArr); lpMemArr = NULL;

                    // We no longer need this storage
                    DbgGlobalFree (hGlbArr); hGlbArr = NULL;

                    // Copy the global handle
                    hGlbArr = hGlbTmp;

                    goto NORMAL_EXIT;

                defstop
                    break;
            } // End SWITCH

            // Convert the chars to blanks
            for (u = 0; u < aplNELM; u++)
                *((LPAPLCHAR)  lpMemArr)++ = L' ';
            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemArr)

            // To make a prototype APA, set its
            //   offset and multiplier to zero
            lpAPA->Off =
            lpAPA->Mul = 0;

#undef  lpAPA

            break;

        case ARRAY_NESTED:
            // Empty nested arrays have one element (the prototype)
            aplNELM = max (aplNELM, 1);

            // Fall through to common HETERO/NESTED code

        case ARRAY_HETERO:
            for (u = 0; bRet && u < aplNELM; u++)
            // Split cases based upon the element's ptr type
            switch (GetPtrTypeInd (lpMemArr))
            {
                case PTRTYPE_STCONST:
                    lpSymArr = *(LPSYMENTRY *) lpMemArr;

                    // It's an immediate
                    Assert (lpSymArr->stFlags.Imm);

                    // Split cases based upon the immediate type
                    switch (lpSymArr->stFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:
                        case IMMTYPE_INT:
                        case IMMTYPE_FLOAT:
                            lpSymRes = GetSteZero ();

                            break;

                        case IMMTYPE_CHAR:
                            // Split cases based upon mpEnum
                            switch (mpEnum)
                            {
                                case MP_CHARS:
                                    lpSymRes = GetSteBlank ();

                                    break;

                                case MP_NUMONLY:
                                    goto DOMAIN_ERROR_EXIT;

                                    break;

                                case MP_NUMCONV:
                                    lpSymRes = GetSteZero ();

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        defstop
                            goto ERROR_EXIT;
                    } // End SWITCH

                    if (lpSymRes)
                        // Save back into array
                        *((LPSYMENTRY *) lpMemArr)++ = lpSymRes;
                    else
                        goto SYMTAB_ERROR_EXIT;
                    break;

                case PTRTYPE_HGLOBAL:
                    Assert (aplType EQ ARRAY_NESTED);

                    // It's a valid HGLOBAL array
                    Assert (IsGlbTypeVarInd (lpMemArr));

                    hGlbTmp =
                    MakeMonPrototype_EM (ClrPtrTypeIndGlb (lpMemArr),   // Proto arg handle
                                         lptkFunc,                      // Ptr to function token
                                         mpEnum);                       // Pass flag through
                    if (hGlbTmp)
                    {
                        // We no longer need this storage
                        FreeResultGlobalVar (ClrPtrTypeIndGlb (lpMemArr)); *((LPAPLNESTED) lpMemArr) = NULL;

                        *((LPAPLNESTED) lpMemArr)++ = MakeGlbTypeGlb (hGlbTmp);
                    } else
                        bRet = FALSE;
                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

DOMAIN_ERROR_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);

    goto ERROR_EXIT;

SYMTAB_ERROR_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYMBOL_TABLE_FULL APPEND_NAME,
                               lptkFunc);
ERROR_EXIT:
    bRet = FALSE;
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbArr); lpMemArr = NULL;

    if (!bRet)
    {
        // We no longer need this storage
        FreeResultGlobalVar (hGlbArr); hGlbArr = NULL;
    } // End IF

    DBGLEAVE;

    // ***FIXME*** -- Do we still need to TypeDemote the result??
////if (hGlbArr)
////    return TypeDemote (hGlbArr);
////else
////    return hGlbArr;
    return hGlbArr;
} // End MakeMonPrototype_EM
#undef  APPEND_NAME


//***************************************************************************
//  MakeDydPrototype_EM
//
//  Make a prototype from a dyadic scalar function between
//    two global memory objects
//
//  Instead of returning one of the args as the result,
//    we create a result by trundling through the args
//    using scalar extension as well as checking for
//    RANK, LENGTH, and DOMAIN ERRORs between the args.
//
//  For example, adding empty vectors whose prototypes are
//    0 (0 0) and (0 0) 0 should produce an empty vector
//    whose prototype is (0 0) (0 0) by scalar extension.
//  Also, adding empty vectors whose prototypes are
//    (0 0) and (0 0 0) should produce a LENGTH ERROR.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeDydPrototype_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL MakeDydPrototype_EM
    (HGLOBAL  hGlbLft,      // Left arg global memory handle (may be NULL if immediate)
     IMMTYPES immTypeLft,   // Left arg storage type if immediate
     LPTOKEN  lptkFunc,     // Ptr to function token
     HGLOBAL  hGlbRht,      // Right arg global memory handle (may be NULL if immediate)
     IMMTYPES immTypeRht,   // Right arg storage type if immediate
     LPTOKEN  lptkAxis)     // Ptr to axis token (may be NULL)

{
    APLSTYPE    aplTypeLft,             // Left arg storage type
                aplTypeRht,             // Right ...
                aplTypeRes;             // Result ...
    APLNELM     aplNELMLft,             // Left arg NELM
                aplNELMRht,             // Right ...
                aplNELMRes,             // Result   ...
                aplNELMAxis;            // Axis value ...
    APLRANK     aplRankLft,             // Left arg rank
                aplRankRht,             // Right ...
                aplRankRes;             // Result ...
    HGLOBAL     hGlbRes = NULL,         // Result global memory handle
                hGlbAxis = NULL,        // Axis value ...
                hGlbWVec = NULL,        // Weighting vector ...
                hGlbOdo = NULL,         // Odometer ...
                hGlbSub;                // Subarray ...
    LPAPLNESTED lpMemLft = NULL,        // Ptr to left arg global memory
                lpMemRht = NULL,        // Ptr to right ...
                lpMemRes = NULL;        // Ptr to result   ...
    LPAPLDIM    lpMemDimRes;            // Ptr to result dimensions
    APLUINT     ByteRes,                // # bytes in the result
                uLft,                   // Loop counter
                uRht,                   // Loop counter
                uRes;                   // Loop counter
    APLINT      iDim;                   // Dimension loop counter
    BOOL        bBoolFn;                // TRUE iff the function is equal or not-equal
    LPPRIMSPEC  lpPrimSpec;             // Ptr to local PRIMSPEC
    LPAPLUINT   lpMemAxisHead = NULL,   // Ptr to axis values, fleshed out
                lpMemAxisTail = NULL,   // Ptr to grade up of AxisHead
                lpMemOdo = NULL,        // Ptr to odometer global memory
                lpMemWVec = NULL;       // Ptr to weighting vector ...

    // Get the attributes (Type, NELM, and Rank)
    //   of the left arg
    if (hGlbLft)        // If not immediate, ...
        AttrsOfGlb (hGlbLft, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    else
    {
        aplTypeLft = TranslateImmTypeToArrayType (immTypeLft);
        aplNELMLft = 1;
        aplRankLft = 0;
    } // End IF

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    if (hGlbRht)        // If not immediate, ...
        AttrsOfGlb (hGlbRht, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);
    else
    {
        aplTypeRht = TranslateImmTypeToArrayType (immTypeRht);
        aplNELMRht = 1;
        aplRankRht = 0;
    } // End IF

    // Handle scalar extension with empty other arg
    if (aplNELMLft EQ 0
     && hGlbRht EQ NULL)
        aplTypeRht = ARRAY_BOOL;
    if (aplNELMRht EQ 0
     && hGlbLft EQ NULL)
        aplTypeLft = ARRAY_BOOL;

    // The rank of the result is the larger of the two args
    //   unless one is a singleton.
    if (aplNELMLft EQ 1 && aplNELMRht NE 1 && aplRankLft > aplRankRht)
        aplRankRes = aplRankRht;
    else
    if (aplNELMLft NE 1 && aplNELMRht EQ 1 && aplRankLft < aplRankRht)
        aplRankRes = aplRankLft;
    else
        aplRankRes = max (aplRankLft, aplRankRht);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Check the axis values, fill in # elements in axis
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRes,      // All values less than this
                           FALSE,           // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // TRUE iff fractional values allowed
                           NULL,            // Return last axis value
                          &aplNELMAxis,     // Return # elements in axis vector
                          &hGlbAxis))       // Return HGLOBAL with APLINT axis values
            return NULL;

        // Lock the memory to get a ptr to it
        lpMemAxisHead = MyGlobalLock (hGlbAxis);

        // Get pointer to the axis tail (where the [X] values are)
        lpMemAxisTail = &lpMemAxisHead[aplRankRes - aplNELMAxis];
    } else
        // No axis is the same as all axes
        aplNELMAxis = aplRankRes;

    // Get the corresponding lpPrimSpec
    lpPrimSpec = PrimSpecTab[SymTrans (lptkFunc)];

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                               &aplTypeLft,
                                                lptkFunc,
                                                aplNELMRht,
                                               &aplTypeRht);
    if (aplTypeRes EQ ARRAY_ERROR)
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    Assert (IsSimpleNum (aplTypeRes)
         || aplTypeRes EQ ARRAY_NESTED);

    // Lock the memory to get a ptr to it
    if (hGlbLft)
        lpMemLft = MyGlobalLock (hGlbLft);

    // Lock the memory to get a ptr to it
    if (hGlbRht)
        lpMemRht = MyGlobalLock (hGlbRht);

    // Check for RANK and LENGTH ERRORs
    if (!CheckRankLengthError_EM (aplRankRes,
                                  aplRankLft,
                                  aplNELMLft,
                                  lpMemLft,
                                  aplRankRht,
                                  aplNELMRht,
                                  lpMemRht,
                                  aplNELMAxis,
                                  lpMemAxisTail,
                                  lptkFunc))
        goto ERROR_EXIT;

    // The NELM of the result is the larger of the two args
    //   unless one is empty
    if (aplNELMLft EQ 0 || aplNELMRht EQ 0)
        aplNELMRes = 0;
    else
        aplNELMRes = max (aplNELMLft, aplNELMRht);

    // Handle APA result separately
    if (aplTypeRes EQ ARRAY_APA)
    {
        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

        // Allocate space for the result.
        // N.B. Conversion from APLUINT to UINT.
        Assert (ByteRes EQ (UINT) ByteRes);
        hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (!hGlbRes)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = aplTypeRes;
////////lpHeader->Perm       = 0;               // Already zero from GHND
////////lpHeader->SysVar     = 0;               // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMRes;
        lpHeader->Rank       = aplRankRes;

#undef  lpHeader

        // Copy the dimensions to the result
        if (aplRankRes EQ aplRankLft)
            CopyMemory (VarArrayBaseToDim (lpMemRes),
                        VarArrayBaseToDim (lpMemLft),
                        (UINT) aplNELMRes * sizeof (APLDIM));
        else
            CopyMemory (VarArrayBaseToDim (lpMemRes),
                        VarArrayBaseToDim (lpMemRht),
                        (UINT) aplNELMRes * sizeof (APLDIM));
        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

#define lpAPA       ((LPAPLAPA) lpMemRes)

        // To make a prototype APA, set its
        //   offset and multiplier to zero
        lpAPA->Off =
        lpAPA->Mul = 0;

#undef  lpAPA

        goto NORMAL_EXIT;
    } // End IF

    // If the result is simple numeric,
    //   convert it to Boolean
    if (IsSimpleNum (aplTypeRes))
        aplTypeRes = ARRAY_BOOL;

    // Get the function type
    bBoolFn = lptkFunc->tkData.tkChar EQ UTF16_EQUAL
           || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL;

    // If the left arg is immediate, the result is the prototype of the other arg
    if (hGlbLft EQ NULL)
        hGlbRes = MakeMonPrototype_EM (hGlbRht,     // Proto arg handle
                                       lptkFunc,    // Ptr to function token
                                       bBoolFn ? MP_NUMCONV : MP_NUMONLY);
    else
    if (hGlbRht EQ NULL)
        hGlbRes = MakeMonPrototype_EM (hGlbLft,     // Proto arg handle
                                       lptkFunc,    // Ptr to function token
                                       bBoolFn ? MP_NUMCONV : MP_NUMONLY);
    else
    {
        // Neither arg is immediate

        // Allocate space for result
        if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                        &hGlbRes,
                                         lpMemLft,
                                         lpMemRht,
                                         aplRankLft,
                                         aplRankRht,
                                        &aplRankRes,
                                         aplTypeRes,
                                         aplNELMLft,
                                         aplNELMRht,
                                         aplNELMRes))
            goto ERROR_EXIT;

        // Take into account the nested prototype
        if (aplTypeLft EQ ARRAY_NESTED)
            aplNELMLft = max (aplNELMLft, 1);
        if (aplTypeRht EQ ARRAY_NESTED)
            aplNELMRht = max (aplNELMRht, 1);
        if (aplTypeRes EQ ARRAY_NESTED)
            aplNELMRes = max (aplNELMRes, 1);

        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbRes);      // ***FIXME***

        // Skip over the header to the dimensions
        lpMemDimRes = VarArrayBaseToDim (lpMemRes);

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

        // If either arg is not simple, loop through the result
        // Otherwise, the result is all zero (already filled in by GHND).
        if (!IsSimple (aplTypeLft)
         || !IsSimple (aplTypeRht))
        {
            // If the left arg is simple, the result is
            //   the prototype of the right arg
            if (IsSimple (aplTypeLft))
            {
                hGlbSub = MakeMonPrototype_EM (ClrPtrTypeIndGlb (lpMemRht), // Proto arg handle
                                               lptkFunc,                    // Ptr to function token
                                               bBoolFn ? MP_NUMCONV : MP_NUMONLY);
                if (!hGlbSub)
                    goto ERROR_EXIT;
                *lpMemRes++ = MakeGlbTypeGlb (hGlbSub);
            } else
            // If the right arg is simple, the result is
            //   the prototype of the left arg
            if (IsSimple (aplTypeRht))
            {
                hGlbSub = MakeMonPrototype_EM (ClrPtrTypeIndGlb (lpMemLft), // Proto arg handle
                                               lptkFunc,                    // Ptr to function token
                                               bBoolFn ? MP_NUMCONV : MP_NUMONLY);
                if (!hGlbSub)
                    goto ERROR_EXIT;
                *lpMemRes++ = MakeGlbTypeGlb (hGlbSub);
            } else
            {
                // Both args are nested

                // Handle axis if present
                if (aplNELMAxis NE aplRankRes)
                {
                    //***************************************************************
                    // Allocate space for the weighting vector which is
                    //   {times}{backscan}1{drop}({rho}Z),1
                    // N.B.  Conversion from APLUINT to UINT.
                    //***************************************************************
                    ByteRes = aplRankRes * sizeof (APLINT);
                    Assert (ByteRes EQ (UINT) ByteRes);
                    hGlbWVec = DbgGlobalAlloc (GHND, (UINT) ByteRes);
                    if (!hGlbWVec)
                    {
                        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                                   lptkFunc);
                        goto ERROR_EXIT;
                    } // End IF

                    // Lock the memory to get a ptr to it
                    lpMemWVec = MyGlobalLock (hGlbWVec);

                    // Loop through the dimensions of the result in reverse
                    //   order {backscan} and compute the cumulative product
                    //   (weighting vector).
                    // Note we use a signed index variable because we're
                    //   walking backwards and the test against zero must be
                    //   made as a signed variable.
                    for (uRes = 1, iDim = aplRankRes - 1; iDim >= 0; iDim--)
                    {
                        lpMemWVec[iDim] = uRes;
                        uRes *= lpMemDimRes[iDim];
                    } // End FOR

                    //***************************************************************
                    // Allocate space for the odometer array, one value per dimension
                    //   in the right arg, with values initially all zero (thanks to GHND).
                    // N.B.  Conversion from APLUINT to UINT.
                    //***************************************************************
                    ByteRes = aplRankRes * sizeof (APLINT);
                    Assert (ByteRes EQ (UINT) ByteRes);
                    hGlbOdo = DbgGlobalAlloc (GHND, (UINT) ByteRes);
                    if (!hGlbOdo)
                    {
                        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                                   lptkFunc);
                        goto ERROR_EXIT;
                    } // End IF

                    // Lock the memory to get a ptr to it
                    lpMemOdo = MyGlobalLock (hGlbOdo);
                } // End IF

                // Loop through the elements of the result
                for (uRes = 0; uRes < aplNELMRes; uRes++)
                {
                    // If there's an axis, ...
                    if (lptkAxis
                     && aplNELMAxis NE aplRankRes)
                    {
                        APLUINT uArg;       // Loop counter

                        // Loop through the odometer values accumulating
                        //   the weighted sum
                        for (uArg = 0, uRht = aplRankRes - aplNELMAxis; uRht < aplRankRes; uRht++)
                            uArg += lpMemOdo[lpMemAxisHead[uRht]] * lpMemWVec[uRht];

                        // Increment the odometer in lpMemOdo subject to
                        //   the values in lpMemDimRes
                        IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

                        // Use the just computed index for the argument
                        //   with the smaller rank
                        if (aplRankLft < aplRankRht)
                        {
                            uLft = uArg;
                            uRht = uRes;
                        } else
                        {
                            uRht = uArg;
                            uLft = uRes;
                        } // End IF/ELSE
                    } else
                    {
                        uLft = uRes % aplNELMLft;
                        uRht = uRes % aplNELMRht;
                    } // End IF/ELSE

                    // If the left arg element is an STE,
                    //   the result element is the prototype
                    //   of the right arg element
                    if (GetPtrTypeDir (lpMemLft[uLft]) EQ PTRTYPE_STCONST)
                    {
                        hGlbSub = MakeMonPrototype_EM (ClrPtrTypeDirGlb (lpMemRht[uRht]),   // Proto arg handle
                                                       lptkFunc,                            // Ptr to function token
                                                       bBoolFn ? MP_NUMCONV : MP_NUMONLY);
                        if (!hGlbSub)
                            goto ERROR_EXIT;
                        *lpMemRes++ = MakeGlbTypeGlb (hGlbSub);
                    } else
                    // If the right arg element is an STE,
                    //   the result element is the prototype
                    //   of the left arg element
                    if (GetPtrTypeDir (lpMemRht[uRht]) EQ PTRTYPE_STCONST)
                    {
                        hGlbSub = MakeMonPrototype_EM (ClrPtrTypeDirGlb (lpMemLft[uLft]),   // Proto arg handle
                                                       lptkFunc,                            // Ptr to function token
                                                       bBoolFn ? MP_NUMCONV : MP_NUMONLY);
                        if (!hGlbSub)
                            goto ERROR_EXIT;
                        *lpMemRes++ = MakeGlbTypeGlb (hGlbSub);
                    } else
                    {
                        // Both args are nested HGLOBALs
                        hGlbSub = MakeDydPrototype_EM (ClrPtrTypeDirGlb (lpMemLft[uLft]),   // Left arg proto handle
                                                       0,                                   // Left arg immediate type (irrelevant as it's an HGLOBAL)
                                                       lptkFunc,                            // Ptr to function token
                                                       ClrPtrTypeDirGlb (lpMemRht[uRht]),   // Right arg proto handle
                                                       0,                                   // Right arg immediate type (irrelevant as it's an HGLOBAL)
                                                       NULL);
                        if (!hGlbSub)
                            goto ERROR_EXIT;
                        *lpMemRes++ = MakeGlbTypeGlb (hGlbSub);
                    } // End IF/ELSE/...
                } // End FOR
            } // End IF/ELSE/...
        } // End IF
    } // End IF/ELSE/...

    goto NORMAL_EXIT;

ERROR_EXIT:
    if (lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbRes)
    {
        // We no longer need this storage
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    if (hGlbWVec)
    {
        if (lpMemWVec)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbWVec); lpMemWVec = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbWVec); hGlbWVec = NULL;
    } // End IF

    if (hGlbOdo)
    {
        if (lpMemOdo)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbOdo); lpMemOdo = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbOdo); hGlbOdo = NULL;
    } // End IF

    if (hGlbAxis)
    {
        if (lpMemAxisHead)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbAxis); lpMemAxisHead = lpMemAxisTail = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbAxis); hGlbAxis = NULL;
    } // End IF

    return hGlbRes;
} // End MakeDydPrototype_EM
#undef  APPEND_NAME


//***************************************************************************
//  $IsFirstSimpleGlb
//
//  Is the first element of a global simple?
//***************************************************************************

BOOL IsFirstSimpleGlb
    (HGLOBAL *lphGlbRht,
     LPCHAR   lpaplTypeRes)

{
    LPVOID     lpMemRht;
    APLSTYPE   aplTypeRht;
    APLRANK    aplRankRht;
    BOOL       bRet = TRUE;
    LPSYMENTRY lpSym;
    HGLOBAL    hGlbFirst;

    // It's a valid HGLOBAL array
    Assert (IsGlbTypeVarDir (*lphGlbRht));

    *lphGlbRht = ClrPtrTypeDirGlb (*lphGlbRht);

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (*lphGlbRht);

#define lpHeaderRht     ((LPVARARRAY_HEADER) lpMemRht)

    aplTypeRht = lpHeaderRht->ArrType;
    aplRankRht = lpHeaderRht->Rank;

#undef  lpHeaderRht

    // It's a nested element
    Assert (aplTypeRht EQ ARRAY_NESTED);

    // Point to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Split cases based upon the element's ptr type
    switch (GetPtrTypeInd (lpMemRht))
    {
        case PTRTYPE_STCONST:       // a {is} 1 {diamond} 0{rho}a (2 3)
            lpSym = *(LPSYMENTRY *) lpMemRht;

            // It's an immediate
            Assert (lpSym->stFlags.Imm);
            *lpaplTypeRes = TranslateImmTypeToArrayType (lpSym->stFlags.ImmType);

            break;

        case PTRTYPE_HGLOBAL:
            // Get the HGLOBAL
            hGlbFirst = *(LPAPLNESTED) lpMemRht;

            bRet = FALSE;

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRht); lpMemRht = NULL;

    // If not simple, return the HGLOBAL of the first element
    if (!bRet)
        *lphGlbRht = hGlbFirst;

    return bRet;
} // End IsFirstSimpleGlb


//***************************************************************************
//  $CopySymGlbDir
//
//  Copy a direct LPSYMENTRY or HGLOBAL incrementing the reference count
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopySymGlbDir"
#else
#define APPEND_NAME
#endif

HGLOBAL CopySymGlbDir
    (LPVOID lpSymGlb)

{
    // Split cases based upon the ptr type
    switch (GetPtrTypeDir (lpSymGlb))
    {
        case PTRTYPE_STCONST:
            return lpSymGlb;

        case PTRTYPE_HGLOBAL:
            // Increment the reference count in global memory
            DbgIncrRefCntDir ((HGLOBAL) lpSymGlb);

            return lpSymGlb;

        defstop
            return NULL;
    } // End SWITCH
} // End CopySymGlbDir
#undef  APPEND_NAME


//***************************************************************************
//  $CopySymGlbInd
//
//  Copy an indirect LPSYMENTRY or HGLOBAL incrementing the reference count
//***************************************************************************

HGLOBAL CopySymGlbInd
    (LPVOID lpSymGlb)

{
    return CopySymGlbDir (*(LPAPLNESTED) lpSymGlb);
} // End CopySymGlbInd


//***************************************************************************
//  $CopySymGlbDirGlb
//
//  Copy a direct HGLOBAL incrementing the reference count
//***************************************************************************

HGLOBAL CopySymGlbDirGlb
    (HGLOBAL hGlb)

{
    return CopySymGlbDir (MakeGlbTypeGlb (hGlb));
} // End CopySymGlbDirGlb


//***************************************************************************
//  $CopyArray_EM
//
//  Make a copy of a global memory ptr array
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopyArray_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL CopyArray_EM
    (HGLOBAL hGlbSrc,       // Source handle
     BOOL    bChanging,     // TRUE iff we're changing the copy
     LPTOKEN lptkFunc)      // Ptr to function token

{
    DWORD       dwSize;
    LPVOID      lpMemSrc, lpMemSrcBase,
                lpMemDst, lpMemDstBase;
    HGLOBAL     hGlbDst,
                hGlbTmp;
    APLSTYPE    aplType;
    APLNELM     aplNELM;
    APLRANK     aplRank;
    LPSYMENTRY  lpSymSrc,
                lpSymDst;
    APLNELM     u;
    BOOL        bRet = TRUE;

    // If we're not changing the copy, instead just increment
    //   the reference count and return the input HGLOBAL.
    if (!bChanging)
    {
        // Increment the reference count in global memory
        DbgIncrRefCntDir (MakeGlbTypeGlb (hGlbSrc));

        return hGlbSrc;
    } // End IF

    // Get the size of the global memory object
    dwSize = MyGlobalSize (hGlbSrc);
    Assert (dwSize NE 0);

    hGlbDst = DbgGlobalAlloc (GHND, dwSize);
    if (hGlbDst)
    {
        // Lock both memory blocks
        lpMemDst = MyGlobalLock (hGlbDst);
        lpMemSrc = MyGlobalLock (hGlbSrc);

        // Copy source to destin
        CopyMemory (lpMemDst, lpMemSrc, dwSize);

        // Set the reference count in case it was a TKT_VARNAMED
#ifdef DEBUG
        dprintfW (L"##RefCnt=1 in " APPEND_NAME L": %08X (%S#%d)", lpMemDst, FNLN);
#endif
#define lpHeaderDst ((LPVARARRAY_HEADER) lpMemDst)

        lpHeaderDst->RefCnt = 1;

        // Recurse through the array, copying all the global ptrs
        aplType = lpHeaderDst->ArrType;
        aplNELM = lpHeaderDst->NELM;
        aplRank = lpHeaderDst->Rank;

#undef  lpHeaderDst

        lpMemDstBase = lpMemDst = VarArrayBaseToData (lpMemDst, aplRank);
        lpMemSrcBase = lpMemSrc = VarArrayBaseToData (lpMemSrc, aplRank);

        // Split cases based upon the array type
        switch (aplType)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_CHAR:
            case ARRAY_APA:
                break;

            case ARRAY_HETERO:
            case ARRAY_NESTED:
                // Handle the empty case
                aplNELM = max (aplNELM, 1);

                // Fill nested result with PTR_REUSED
                //   in case we fail part way through
                for (u = 0; u < aplNELM; u++)
                    *((LPAPLNESTED) lpMemDst)++ = PTR_REUSED;

                // Start the destin ptr over again
                lpMemDst = lpMemDstBase;

                // Loop through the source and destin arrays
                for (u = 0;
                     u < aplNELM;
                     u++, ((LPAPLNESTED) lpMemDst)++, ((LPAPLNESTED) lpMemSrc)++)
                // Split cases based upon the ptr type
                switch (GetPtrTypeInd (lpMemSrc))
                {
                    case PTRTYPE_STCONST:
                        lpSymSrc = *(LPSYMENTRY *) lpMemSrc;

                        // It's an immediate
                        Assert (lpSymSrc->stFlags.Imm);

                        // Copy it
                        lpSymDst = CopyImmSymEntry_EM (lpSymSrc,
                                                       -1,
                                                       lptkFunc);
                        if (lpSymDst)
                            // Save into the destin
                            *((LPSYMENTRY *) lpMemDst) = lpSymDst;
                        else
                            bRet = FALSE;
                        break;

                    case PTRTYPE_HGLOBAL:
                        Assert (aplType EQ ARRAY_NESTED);

                        // It's a valid HGLOBAL array
                        Assert (IsGlbTypeVarInd (lpMemSrc));

                        // Copy the array
                        hGlbTmp = CopyArray_EM (ClrPtrTypeIndGlb (lpMemSrc),
                                                TRUE,
                                                lptkFunc);
                        if (hGlbTmp)
                            // Save into the destin
                            *((LPAPLNESTED) lpMemDst) = MakeGlbTypeGlb (hGlbTmp);
                        else
                            bRet = FALSE;
                        break;

                    defstop
                        break;
                } // End FOR/SWITCH

                break;

            defstop
                break;
        } // End SWITCH

        // We no longer need these ptrs
        MyGlobalUnlock (hGlbDst); lpMemDst = NULL;
        MyGlobalUnlock (hGlbSrc); lpMemSrc = NULL;

        if (!bRet)
        {
            FreeResultGlobalVar (hGlbDst); hGlbDst = NULL;
        } // End IF
    } else
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
    return hGlbDst;
} // End CopyArray_EM
#undef  APPEND_NAME


#ifdef DEBUG
//***************************************************************************
//  $IsGlobalTypeArray
//
//  Confirm that an HGLOBAL is a valid variable or function array, or a
//    defined function.
//***************************************************************************

BOOL IsGlobalTypeArray
    (HGLOBAL hGlb,
     UINT    Signature)

{
    LPVOID lpMem;
    BOOL   bRet = TRUE;

    // It's an HGLOBAL
    switch (GetPtrTypeDir (hGlb))
    {
        case PTRTYPE_STCONST:
            return FALSE;

        case PTRTYPE_HGLOBAL:
            break;

        defstop
            break;
    } // End SWITCH

    // Clear the ptr type bits
    hGlb = ClrPtrTypeDirGlb (hGlb);

    // It's a valid ptr
    bRet = bRet && IsGlbPtr (hGlb);

    if (bRet)
    {
        // Lock the memory to get a ptr to it
        lpMem = GlobalLock (hGlb); Assert (lpMem NE NULL);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

        // Ensure it has the correct signature
        bRet = (lpHeader->Sig.nature EQ Signature)
            && (lpHeader->Perm || lpHeader->RefCnt > 0);

#undef  lpHeader

        // We no longer need this ptr
        GlobalUnlock (hGlb); lpMem = NULL;
    } // End IF

    return bRet;
} // End IsGlobalTypeArray
#endif


//***************************************************************************
//  $CheckRankLengthError_EM
//
//  Check a dyadic scalar function for RANK and LENGTH ERRORs
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckRankLengthError_EM"
#else
#define APPEND_NAME
#endif

BOOL CheckRankLengthError_EM
    (APLRANK  aplRankRes,           // Result rank
     APLRANK  aplRankLft,           // Left arg ...
     APLNELM  aplNELMLft,           // ...      NELM
     LPVOID   lpMemLft,             // Ptr to left arg memory
     APLRANK  aplRankRht,           // Right arg rank
     APLNELM  aplNELMRht,           // ...       NELM
     LPVOID   lpMemRht,             // Ptr to right arg memory
     APLNELM  aplNELMAxis,          // Axis NELM
     LPAPLINT lpMemAxisTail,        // Ptr to grade up of AxisHead
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    APLINT uRes;

    //***************************************************************
    // If the ranks don't match, then it gets complicated:
    //
    //   If both arguments are singletons, the result rank
    //     (and shape) is the same rank (and shape) as the
    //     argument with the larger rank.
    //   If only one argument is a singleton, the result rank
    //     (and shape) is the same as the rank (and shape) of the
    //     non-singleton argument.
    //   If either argument is a singleton, any valid axis values
    //     are allowed.  That is, any scalar or vector X such that
    //     {and}/X{epsilon}{iota}{rho}{rho}Z.
    //   If no axis is specified, it is equivalent to the axis being
    //     {iota}{rho}{rho}Z.
    //   If neither argument is a singleton and axis is specified,
    //     then it must be true that
    //       ({rho}L)[X]{match}{rho}R
    //     assuming that L is the argument of lower rank.  If not,
    //     switch L and R in the above expression.
    //***************************************************************

    // OUTER RANK and LENGTH ERRORs are possible only if
    //   neither argument is a singleton
    if (aplNELMLft NE 1 && aplNELMRht NE 1)
    {
        // Check for OUTER RANK ERROR
        if ((aplRankLft <  aplRankRht && (aplRankLft NE aplNELMAxis))
         || (aplRankLft >  aplRankRht && (aplRankRht NE aplNELMAxis)))
        {
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkFunc);
            return FALSE;
        } // End IF

        // If axis not full, ...
        if (aplNELMAxis NE aplRankRes)
        {
            if (aplRankLft < aplRankRht)
            {
                // Check for OUTER LENGTH ERROR
                for (uRes = 0; uRes < (APLRANKSIGN) aplRankLft; uRes++)
                if ((VarArrayBaseToDim (lpMemLft))[uRes] !=
                    (VarArrayBaseToDim (lpMemRht))[lpMemAxisTail[uRes]])
                {
                    uRes = (APLINT) -1; // Mark as in error

                    break;
                } // End FOR/IF
            } else
            if (aplRankLft > aplRankRht)
            {
                // Check for OUTER LENGTH ERROR
                for (uRes = 0; uRes < (APLRANKSIGN) aplRankRht; uRes++)
                if ((VarArrayBaseToDim (lpMemLft))[lpMemAxisTail[uRes]] !=
                    (VarArrayBaseToDim (lpMemRht))[uRes])
                {
                    uRes = (APLINT) -1; // Mark as in error

                    break;
                } // End FOR/IF
            } else
                uRes = (APLINT) -1; // Mark as in error (equal ranks with axis)
        } else
        // If axis full (or no axis) and ranks the same, ...
        if (aplRankLft EQ aplRankRht)
        {
            // Check for OUTER LENGTH ERROR
            for (uRes = 0; uRes < (APLRANKSIGN) aplRankRes; uRes++)
            if ((VarArrayBaseToDim (lpMemLft))[uRes] !=
                (VarArrayBaseToDim (lpMemRht))[uRes])
            {
                uRes = (APLINT) -1; // Mark as in error

                break;
            } // End FOR/IF
        } else
            uRes = (APLINT) -1; // Mark as in error (unequal ranks w/o axis)

        // Check for error
        if (uRes EQ (APLINT) -1)
        {
            ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                       lptkFunc);
            return FALSE;
        } // End IF
    } // End IF

    return TRUE;
} // End CheckRankLengthError_EM
#undef  APPEND_NAME


//***************************************************************************
//  $RoundUpBits8
//
//  Round up bits to a byte (8 bits/byte)
//***************************************************************************

APLINT RoundUpBits8
    (APLNELM aplNELM)

{
    return (aplNELM + (NBIB - 1)) & ~MASKLOG2NBIB;
} // End RoundUpBits8


//***************************************************************************
//  $RoundUpBitsInArray
//
//  Round up bits to a dword (8 bits/byte)
//***************************************************************************

APLINT RoundUpBitsInArray
    (APLNELM aplNELM)

{
    return (aplNELM + (NBID - 1)) & ~MASKLOG2NBID;
} // End RoundUpBitsInArray


//***************************************************************************
//  $CheckException
//
//  Check on a structured exception
//***************************************************************************

long CheckException
    (LPEXCEPTION_POINTERS lpExcept)

{
    // Save the exception code for later use
    MySetExceptionCode (lpExcept->ExceptionRecord->ExceptionCode);

    // Split cases based upon the exception code
    switch (MyGetExceptionCode ())
    {
////////case EXCEPTION_RESULT_BOOL:
////////case EXCEPTION_RESULT_INT:
        case EXCEPTION_RESULT_FLOAT:
        case EXCEPTION_BREAKPOINT:
        case EXCEPTION_DOMAIN_ERROR:
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
        case EXCEPTION_ACCESS_VIOLATION:
            return EXCEPTION_EXECUTE_HANDLER;

        case EXCEPTION_CTRL_BREAK:
            return EXCEPTION_CONTINUE_EXECUTION;

        default:
            return EXCEPTION_CONTINUE_SEARCH;
    } // End SWITCH
} // End CheckException


//***************************************************************************
//  $imul64
//
//  Multiply two 64-bit integers retaining maximum precision
//***************************************************************************

APLINT imul64
    (APLINT aplLft,
     APLINT aplRht,
     LPBOOL lpbRet)     // Is the result valid?? (may be NULL)

{
    APLINT aplRes;

    _clear87 ();

    _asm
    {
        fild    aplLft;
        fild    aplRht;
        fmulp   st(1),st(0);
        fistp   aplRes;
    }

    // Check for overflow
    if (_SW_INVALID & _status87 ())
    {
        if (lpbRet)
            *lpbRet = FALSE;
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    return aplRes;
} // End imul64


//***************************************************************************
//  $CalcArraySize
//
//  Calculate the size in bytes of an array
//***************************************************************************

APLUINT CalcArraySize
    (ARRAY_TYPES aplType,       // Result storage type
     APLNELM     aplNELM,       // ...    NELM
     APLRANK     aplRank)       // ...    rank

{
    APLUINT ByteRes;            // # bytes needed for the result

    Assert (0 <= aplType
         &&      aplType < ARRAY_LENGTH);

    // Split cases based upon the result storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            ByteRes = sizeof (APLBOOL)   * RoundUpBitsInArray (aplNELM);

            break;

        case ARRAY_INT:
            ByteRes = sizeof (APLINT)    * aplNELM;

            break;

        case ARRAY_FLOAT:
            ByteRes = sizeof (APLFLOAT)  * aplNELM;

            break;

        case ARRAY_CHAR:
            ByteRes = sizeof (APLCHAR)   * aplNELM;

            break;

        case ARRAY_APA:
            ByteRes = sizeof (APLAPA);

            break;

        case ARRAY_HETERO:
            ByteRes = sizeof (APLHETERO) * aplNELM;

            break;

        case ARRAY_NESTED:
            // Make room for the prototype
            ByteRes = sizeof (APLNESTED) * max (aplNELM, 1);

            break;

        defstop
            break;
    } // End SWITCH

    // Add in the size of the header and dimension
    ByteRes += CalcHeaderSize (aplRank);

    return ByteRes;
} // End CalcArraySize


//***************************************************************************
//  $CalcHeaderSize
//
//  Calculate the size in bytes of the header of an array
//***************************************************************************

APLUINT CalcHeaderSize
    (APLRANK aplRank)

{
    return sizeof (VARARRAY_HEADER)
         + sizeof (APLDIM) * aplRank;
} // End CalcHeaderSize


//***************************************************************************
//  $TypeDemote
//
//  Attempt to demote the type of a global memory handle
//
//  Various primitives (e.g. Match) rely upon type demotion to
//  return a correct result.
//
//  Consequently, it must not be possible to store an all simple
//  homogeneous array in nested array format.  In other words,
//  structural functions such as
//
//  take
//  drop
//  indexed assignment
//  pick
//  unique
//  partition
//  without
//  etc.
//
//  The following functions have been changed to use TypeDemote:
//    enclose with axis
//    reshape
//    collapsing dyadic transpose
//    enlist
//    compression/replicate
//    each
//    first
//    outer product
//    reduction w/ or w/o axis
//    each
//    indexing/squad
//    reduction
//    N-wise reduction
//    scan
//
//  must call this function to check their result to see if it
//  can be stored more simply.  Note that more simply does not
//  mean in fewer bytes as ptrs are four bytes while integers are eight.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- TypeDemote"
#else
#define APPEND_NAME
#endif

LPTOKEN TypeDemote
    (LPTOKEN lptkRhtArg)

{
    HGLOBAL           hGlbRht = NULL,   // Right arg global memory handle
                      hGlbRes = NULL;   // Result    ...
    LPVOID            lpMemRht = NULL,  // Ptr to right arg global memory
                      lpMemRes = NULL;  // Ptr to result    ...
    APLNELM           aplNELMRht;       // Right arg NELM
    APLRANK           aplRankRht;       // Right arg rank
    APLUINT           uRht,             // Right arg loop counter
                      ByteRes;          // # bytes needed for the result
    APLINT            aplInteger;       // Temporary integer
    APLFLOAT          aplFloat;         // ...       float
    APLSTYPE          aplTypeRes,
                      aplTypeRht,
                      aplTypeSub;
    LPSYMENTRY        lpSymEntry;
    LPVARARRAY_HEADER lpMemRhtHdr;
    DWORD             dwSize;

    // Note that neither ARRAY_APA not ARRAY_LIST can occur
    //   as an argument to this function.
    static APLSTYPE aplTypeArr[ARRAY_LENGTH][ARRAY_LENGTH] =
    //      BOOL          INT           FLOAT         CHAR        HETERO        NESTED        INIT
    {{ARRAY_BOOL  , ARRAY_INT   , ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_BOOL  },  // BOOL
     {ARRAY_INT   , ARRAY_INT   , ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_INT   },  // INT
     {ARRAY_FLOAT , ARRAY_FLOAT , ARRAY_FLOAT , ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_FLOAT },  // FLOAT
     {ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_CHAR  , ARRAY_HETERO, ARRAY_NESTED, ARRAY_CHAR  },  // CHAR
     {ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_HETERO, ARRAY_NESTED, ARRAY_HETERO},  // HETERO
     {ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED},  // NESTED
     {ARRAY_BOOL  , ARRAY_INT   , ARRAY_FLOAT , ARRAY_CHAR  , ARRAY_HETERO, ARRAY_NESTED, ARRAY_LIST  },  // INIT
    };

    // Split cases based upon the arg token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARIMMED:
            return lptkRhtArg;

        case TKT_VARARRAY:
            hGlbRht = ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkGlbData);

            break;

        defstop
            break;
    } // End SWITCH

    // Copy to the result in case we don't demote
    hGlbRes = hGlbRht;

    // Lock the memory to get a ptr to it
    lpMemRht = lpMemRhtHdr = MyGlobalLock (hGlbRht);

    // Get the Type, NELM, and Rank
    aplTypeRht = lpMemRhtHdr->ArrType;
    aplNELMRht = lpMemRhtHdr->NELM;
    aplRankRht = lpMemRhtHdr->Rank;

    // Handle empties up front
    if (aplNELMRht EQ 0)
        goto NORMAL_EXIT;

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRhtHdr, aplRankRht);

    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:        // No place to go
        case ARRAY_APA:         // No place to go
        case ARRAY_CHAR:        // No place to go
            goto IMMED_EXIT;

        case ARRAY_INT:         // Demote to Boolean
            // Start with lowest type
            aplTypeRes = ARRAY_BOOL;

            // Loop through the elements
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                aplInteger = *((LPAPLINT) lpMemRht)++;

                // Check for Boolean value
                if (!IsBooleanValue (aplInteger))
                {
                    aplTypeRes = ARRAY_INT;

                    break;
                } // End IF
            } // End FOR

            break;

        case ARRAY_FLOAT:       // Demote to Boolean/Integer
            // Start with lowest type
            aplTypeRes = ARRAY_BOOL;

            // Loop through the elements
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                aplFloat = *((LPAPLFLOAT) lpMemRht)++;

                // Check for Boolean value
                if (IsBooleanValue (aplFloat))
                    aplTypeSub = ARRAY_BOOL;
                else
                // Check for Integer value
                if (aplFloat EQ floor (aplFloat)
                 && fabs (aplFloat) < Float2Pow53)
                    aplTypeSub = ARRAY_INT;
                else
                    aplTypeSub = ARRAY_FLOAT;

                // Check storage type
                aplTypeRes = aplTypeArr[aplTypeRes][aplTypeSub];

                // If we're back to the same type, quit
                if (aplTypeRes EQ aplTypeRht)
                    break;
            } // End FOR

            break;

        case ARRAY_HETERO:      // Demote to simple homogeneous
        case ARRAY_NESTED:      // Demote to simple homogeneous/heterogeneous
            // Initialize the type (as ARRAY_LIST never occurs
            //   in this context, we co-opt it).
            aplTypeRes = ARRAY_LIST;

            // Loop through the elements
            for (uRht = 0; uRht < aplNELMRht; uRht++, ((LPAPLHETERO) lpMemRht)++)
            {
                // Split cases based upon the ptr type of the element
                switch (GetPtrTypeInd (lpMemRht))
                {
                    case PTRTYPE_STCONST:
                        lpSymEntry = *(LPAPLHETERO) lpMemRht;

                        // stData is an immediate
                        Assert (lpSymEntry->stFlags.Imm);

                        // Get the immediate type (translated to an ARRAY_TYPE)
                        aplTypeSub = TranslateImmTypeToArrayType (lpSymEntry->stFlags.ImmType);

                        // Check for various possible type demotions
                        switch (aplTypeSub)
                        {
                            case ARRAY_FLOAT:   // Check for demotion from FLOAT to INT/BOOL
                                aplFloat = lpSymEntry->stData.stFloat;

                                if (IsBooleanValue (aplFloat))
                                    aplTypeSub = ARRAY_BOOL;
                                else
                                if (aplFloat EQ floor (aplFloat)
                                 && fabs (aplFloat) < Float2Pow53)
                                    aplTypeSub = ARRAY_INT;
                                break;

                            case ARRAY_INT:     // Check for demotion from INT to BOOL
                                aplInteger = lpSymEntry->stData.stInteger;

                                if (IsBooleanValue (aplInteger))
                                    aplTypeSub = ARRAY_BOOL;
                                break;

                            case ARRAY_BOOL:    // No type demotion
                            case ARRAY_CHAR:    // No type demotion
                            default:
                                break;
                        } // End SWITCH

                        // Check storage type
                        aplTypeRes = aplTypeArr[aplTypeRes][aplTypeSub];

                        // Check for no demotion
                        if (aplTypeRes EQ ARRAY_HETERO
                         && aplTypeRht EQ ARRAY_HETERO)
                            goto NORMAL_EXIT;
                        break;

                    case PTRTYPE_HGLOBAL:
                        // It's nested, so there's no demotion
                        goto NORMAL_EXIT;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    // If the result type is the same as the right arg type,
    //   there's no demotion
    if (aplTypeRes EQ aplTypeRht)
        goto NORMAL_EXIT;

    // Check for demotion from Nested to Hetero
    if (aplTypeRes EQ ARRAY_HETERO
     && aplTypeRht EQ ARRAY_NESTED)
    {
        // If the reference count of this array is one, just
        //   change the array type from ARRAY_NESTED to ARRAY_HETERO.
        if (lpMemRhtHdr->RefCnt EQ 1)
            lpMemRhtHdr->ArrType = ARRAY_HETERO;
        else
        {
            // Copy this array and change the type from ARRAY_NESTED to ARRAY_HETERO
            dwSize = MyGlobalSize (hGlbRht);
            hGlbRes = DbgGlobalAlloc (GHND, dwSize);
            if (hGlbRes)
            {
                // Lock the memory to get a ptr to it
                lpMemRes = MyGlobalLock (hGlbRes);

                // Copy source to destin
                CopyMemory (lpMemRes, lpMemRht, dwSize);

#ifdef DEBUG
                dprintfW (L"##RefCnt=1 in " APPEND_NAME L": %08X (%S#%d)", lpMemRes, FNLN);
#endif
#define lpHeaderRes ((LPVARARRAY_HEADER) lpMemRes)

                // Set the reference count and array type
                lpHeaderRes->RefCnt  = 1;
                lpHeaderRes->ArrType = ARRAY_HETERO;

#undef  lpHeaderRes
                // We no longer need these ptrs
                MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
                MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

                // Free the old array
                FreeResultGlobalVar (hGlbRht); hGlbRht = NULL;
            } else
                // WS FULL, so no demotion
                hGlbRes = hGlbRht;
        } // End IF/ELSE
    } else
    // Check for demotion to simple homogeneous
    if (uTypeMap[aplTypeRes] < min (uTypeMap[ARRAY_HETERO], uTypeMap[aplTypeRht]))
    {
        // Handle simple scalars as immediates
        if (aplRankRht EQ 0
         && IsSimple (aplTypeRes))
        {
            lptkRhtArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkRhtArg->tkFlags.ImmType  = TranslateArrayTypeToImmType (aplTypeRes);
            lptkRhtArg->tkData.tkLongest = 0;

            // Skip over header and dimensions to the data
            lpMemRht = VarArrayBaseToData (lpMemRhtHdr, aplRankRht);

            // Demote the data in the right arg, copying it to the result
            DemoteData (aplTypeRes,
                       &lptkRhtArg->tkData.tkLongest,
                        aplTypeRht,
                        aplNELMRht,
                        lpMemRht);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

            // Free the old array
            FreeResultGlobalVar (hGlbRht); hGlbRht = NULL;

            goto IMMED_EXIT;
        } // End IF

        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

        // Allocate space for a new array
        Assert (ByteRes EQ (UINT) ByteRes);
        hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (hGlbRes)
        {
            // Lock the memory to get a ptr to it
            lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

            // Fill in the header
            lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = aplTypeRes;
////////////lpHeader->Perm       = 0;               // Already zero from GHND
////////////lpHeader->SysVar     = 0;               // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELMRht;
            lpHeader->Rank       = aplRankRht;

#undef  lpHeader

            // Skip over the header to the dimensions
            lpMemRht = VarArrayBaseToDim (lpMemRhtHdr);
            lpMemRes = VarArrayBaseToDim (lpMemRes);

            // Copy the dimensions to the result
            for (uRht = 0; uRht < aplRankRht; uRht++)
                *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;

            // Now, lpMemRes and lpMemRht both point to their
            //   respective data

            // Demote the data in the right arg, copying it to the result
            DemoteData (aplTypeRes,
                        lpMemRes,
                        aplTypeRht,
                        aplNELMRht,
                        lpMemRht);
            // We no longer need these ptrs
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
            MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

            // Free the old array
            FreeResultGlobalVar (hGlbRht); hGlbRht = NULL;
        } else
            // WS FULL, so no demotion
            hGlbRes = hGlbRht;
    } // End IF/ELSE
NORMAL_EXIT:
    // Save the result in the right arg token
    lptkRhtArg->tkData.tkGlbData = MakeGlbTypeGlb (hGlbRes);
IMMED_EXIT:
    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lptkRhtArg;
} // End TypeDemote
#undef  APPEND_NAME


//***************************************************************************
//  $DemoteData
//
//  Demote the data in the right arg, copying it to the result
//***************************************************************************

void DemoteData
    (APLSTYPE aplTypeRes,           // Result storage type
     LPVOID   lpMemRes,             // Ptr to result global memory
     APLSTYPE aplTypeRht,           // Right arg storage type
     APLNELM  aplNELMRht,           // Right arg NELM
     LPVOID   lpMemRht)             // Ptr to right arg global memory

{
    APLUINT uRht;
    UINT    uBitIndex;

    // Split cases based upon the result's storage type
    // Note that the result is always of lower type than
    //   the right arg.
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:            // Res = BOOL, Rht = INT/FLOAT/HETERO/NESTED
            uBitIndex = 0;

            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_INT:     // Res = BOOL, Rht = INT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        *((LPAPLBOOL) lpMemRes) |= (*((LPAPLINT) lpMemRht)++) << uBitIndex;

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_FLOAT:   // Res = BOOL, Rht = FLOAT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        *((LPAPLBOOL) lpMemRes) |= ((APLBOOL) *((LPAPLFLOAT) lpMemRht)++) << uBitIndex;

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_HETERO:  // Res = BOOL, Rht = HETERO
                case ARRAY_NESTED:  // Res = BOOL, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                    {
                        *((LPAPLBOOL) lpMemRes) |= ((*((LPSYMENTRY *) lpMemRht)++)->stData.stInteger) << uBitIndex;

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_INT:         // Res = INT, Rht = FLOAT/HETERO/NESTED
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_FLOAT:   // Res = INT , Rht = FLOAT
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                        *((LPAPLINT) lpMemRes)++ = (APLINT) *((LPAPLFLOAT) lpMemRht)++;
                    break;

                case ARRAY_HETERO:  // Res = INT , Rht = HETERO
                case ARRAY_NESTED:  // Res = INT , Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                        *((LPAPLINT) lpMemRes)++ = (*((LPSYMENTRY *) lpMemRht)++)->stData.stInteger;
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_FLOAT:       // Res = FLOAT, Rht = HETERO/NESTED
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_HETERO:  // Res = FLOAT, Rht = HETERO
                case ARRAY_NESTED:  // Res = FLOAT, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                        *((LPAPLFLOAT) lpMemRes)++ = (*((LPSYMENTRY *) lpMemRht)++)->stData.stFloat;
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_CHAR:        // Res = CHAR, Rht = HETERO/NESTED
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_HETERO:  // Res = CHAR, Rht = HETERO
                case ARRAY_NESTED:  // Res = CHAR, Rht = NESTED
                    // Loop through the elements
                    for (uRht = 0; uRht < aplNELMRht; uRht++)
                        *((LPAPLCHAR) lpMemRes)++ = (*((LPSYMENTRY *) lpMemRht)++)->stData.stChar;
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH
} // End DemoteData


//***************************************************************************
//  $YYAlloc
//
//  Allocate a new YYRes entry
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- YYAlloc"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE YYAlloc
    (void)

{
    UINT         u;             // Loop counter
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes = NULL;// Ptr to the result

#ifdef DEBUG
    static UINT YYIndex = 0;
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Search for an empty YYRes slot,
    //   zero it,
    //   mark it as inuse,
    //   and return a ptr to it
    for (u = 0; u < lpMemPTD->numYYRes; u++)
    if (!lpMemPTD->lpYYRes[u].YYInuse)
    {
        // Set the return value
        lpYYRes = &lpMemPTD->lpYYRes[u];

        // Zero the memory
        ZeroMemory (lpYYRes, sizeof (lpYYRes[0]));

        // Mark as inuse
        lpYYRes->YYInuse = 1;

#ifdef DEBUG
        lpYYRes->SILevel = lpMemPTD->SILevel;   // Save the SI Level
        lpYYRes->YYFlag = 0;  // Mark as a YYAlloc Index

        // Save unique number for debugging/tracking purposes
        lpYYRes->YYIndex = ++YYIndex;
#endif
        goto NORMAL_EXIT;
    } // End FOR/IF

    // If we get here, we ran out of indices
    lpYYRes = &lpMemPTD->lpYYRes[u++];
    lpMemPTD->numYYRes = u;
RESTART_EXCEPTION_YYALLOC:
    __try
    {
        // Zero the memory
        ZeroMemory (lpYYRes, sizeof (lpYYRes[0]));

        // Mark as inuse
        lpYYRes->YYInuse = 1;
#ifdef DEBUG
        lpYYRes->YYFlag = 0;  // Mark as a YYAlloc Index

        // Save unique number for debugging/tracking purposes
        lpYYRes->YYIndex = ++YYIndex;
#endif
    } __except (CheckException (GetExceptionInformation ()))
    {
#ifdef DEBUG
        dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %08X (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
        // Split cases based upon the ExceptionCode
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_ACCESS_VIOLATION:
            {
                MEMORY_BASIC_INFORMATION mbi;

                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                // See how many pages are already allocated
                VirtualQuery (lpMemPTD->lpYYRes,
                             &mbi,
                              sizeof (mbi));

                // Check for no allocation as yet
                if (mbi.State EQ MEM_RESERVE)
                    mbi.RegionSize = 0;

                // Allocate more memory to the YYRes buffer
                if (VirtualAlloc (lpMemPTD->lpYYRes,
                                  mbi.RegionSize + DEF_YYRES_INCRSIZE * sizeof (PL_YYSTYPE),
                                  MEM_COMMIT,
                                  PAGE_READWRITE) NE NULL)
                    goto RESTART_EXCEPTION_YYALLOC;

                // Fall through to never-never-land

            } // End EXCEPTION_ACCESS_VIOLATION

            defstop
                break;
        } // End SWITCH
    } // End __try/__except
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpYYRes;
} // End YYAlloc
#undef  APPEND_NAME


//***************************************************************************
//  $YYCopy
//
//  Copy one PL_YYSTYPE to another
//    retaining the destination Inuse, Flag, and Index
//***************************************************************************

void YYCopy
    (LPPL_YYSTYPE lpYYDst,
     LPPL_YYSTYPE lpYYSrc)

{
#ifdef DEBUG
    UINT YYIndex,
         YYFlag;
#endif

    Assert (lpYYDst->YYInuse);

#ifdef DEBUG
    // Save the old index & flag
    YYIndex = lpYYDst->YYIndex;
    YYFlag  = lpYYDst->YYFlag;
#endif
    *lpYYDst = *lpYYSrc;            // Copy the PL_YYSTYPE
    lpYYDst->YYInuse = 1;           // Retain YYInuse flag
#ifdef DEBUG
    lpYYDst->YYIndex = YYIndex;     // Retain YYIndex
    lpYYDst->YYFlag  = YYFlag;      // ...    YYFlag
#endif
} // End YYCopy


//***************************************************************************
//  $YYCopyFreeDst
//
//  Copy one PL_YYSTYPE to another
//    retaining the destination Inuse, Flag, and Index
//    and free the destination copy if it's not inuse.
//***************************************************************************

void YYCopyFreeDst
    (LPPL_YYSTYPE lpYYDst,
     LPPL_YYSTYPE lpYYSrc)

{
    if (lpYYDst->YYInuse)
        YYCopy (lpYYDst, lpYYSrc);
    else
    {
        lpYYDst->YYInuse = 1;       // Mark as in use for YYCopy
        YYCopy (lpYYDst, lpYYSrc);
        lpYYDst->YYInuse = 0;       // Mark as no longer in use
    } // End IF/ELSE
} // End YYCopyFreeDst


//***************************************************************************
//  $YYFree
//
//  Free a YYRes entry
//***************************************************************************

void YYFree
    (LPPL_YYSTYPE lpYYRes)      // Ptr to the YYRes entry

{
#ifdef DEBUG                    // ***DEBUG***
    UINT         u;             // Index into lpMemPTD->YYRes
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    u = lpYYRes - lpMemPTD->lpYYRes;
    Assert (u < lpMemPTD->numYYRes);
    Assert (lpYYRes->YYInuse EQ 1);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
#endif
////lpYYRes->YYInuse = 0;       // Free it
    ZeroMemory (lpYYRes, sizeof (PL_YYSTYPE));
} // End YYFree


#ifdef DEBUG
//***************************************************************************
//  $YYResIsEmpty
//
//  Ensure that YYRes has no Inuse entries
//***************************************************************************

BOOL YYResIsEmpty
    (void)

{
    UINT         u;             // Loop counter
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    BOOL         bRet = TRUE;   // TRUE iff result is valid

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Loop through the YYRes entries looking for
    //   entries in use at this SI Level
    for (u = 0; u < lpMemPTD->numYYRes; u++)
    if (lpMemPTD->lpYYRes[u].YYInuse
     && lpMemPTD->SILevel EQ lpMemPTD->lpYYRes[u].SILevel)
    {
        bRet = FALSE;

        break;
    } // End FOR/IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End YYResIsEmpty
#endif


//***************************************************************************
//  $MakeNoValue_YY
//
//  Return a NoValue PL_YYSTYPE entry
//***************************************************************************

LPPL_YYSTYPE MakeNoValue_YY
    (LPTOKEN lptkFunc)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARNAMED;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;             // Already zero from YYAlloc
    lpYYRes->tkToken.tkFlags.NoDisplay = 1;
    lpYYRes->tkToken.tkData.tkSym      = lpMemPTD->steNoValue;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpYYRes;
} // End MakeNoValue_YY


//***************************************************************************
//  $GetSISLayer
//
//  Peel back to the first non-Imm/Exec layer
//    starting with the previous SIS header
//***************************************************************************

LPSIS_HEADER GetSISLayer
    (LPSIS_HEADER lpSISCur)

{
    while (lpSISCur
        && (lpSISCur->DfnType EQ DFNTYPE_IMM
         || lpSISCur->DfnType EQ DFNTYPE_EXEC))
        lpSISCur = lpSISCur->lpSISPrv;

    return lpSISCur;
} // End GetSISLayer


//***************************************************************************
//  $FillSISNxt
//
//  Fill in the SIS Header for lpSISNxt
//***************************************************************************

void FillSISNxt
    (LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     HANDLE       hSemaphore,           // Semaphore handle
     DFNTYPES     DfnType,              // DFNTYPE
     FCNVALENCES  FcnValence,           // FCNVALENCE
     BOOL         Suspended)            // TRUE iff starts Suspended

{
RESTART_EXCEPTION_FILLSISNXT:
    __try
    {
        // Create another level on the SI stack
        lpMemPTD->lpSISNxt->Sig.nature    = SIS_HEADER_SIGNATURE;
        lpMemPTD->lpSISNxt->hSemaphore    = hSemaphore;
        lpMemPTD->lpSISNxt->hSigaphore    = NULL;
        lpMemPTD->lpSISNxt->hGlbDfnHdr    = NULL;
        lpMemPTD->lpSISNxt->hGlbFcnName   = NULL;
        lpMemPTD->lpSISNxt->DfnType       = DfnType;
        lpMemPTD->lpSISNxt->FcnValence    = FcnValence;
        lpMemPTD->lpSISNxt->DfnAxis       = FALSE;
        lpMemPTD->lpSISNxt->Suspended     = Suspended;
        lpMemPTD->lpSISNxt->Avail         = 0;
        lpMemPTD->lpSISNxt->CurLineNum    = 0;
        lpMemPTD->lpSISNxt->NxtLineNum    = 0;
        lpMemPTD->lpSISNxt->numLabels     = 0;
        lpMemPTD->lpSISNxt->numResultSTE  = 0;
        lpMemPTD->lpSISNxt->offResultSTE  = 0;
        lpMemPTD->lpSISNxt->numLftArgSTE  = 0;
        lpMemPTD->lpSISNxt->offLftArgSTE  = 0;
        lpMemPTD->lpSISNxt->numRhtArgSTE  = 0;
        lpMemPTD->lpSISNxt->offRhtArgSTE  = 0;
        lpMemPTD->lpSISNxt->numLocalsSTE  = 0;
        lpMemPTD->lpSISNxt->offLocalsSTE  = 0;
        lpMemPTD->lpSISNxt->numFcnLines   = 0;
        lpMemPTD->lpSISNxt->SILevel       = ++lpMemPTD->SILevel;
        lpMemPTD->lpSISNxt->lpSISPrv      = lpMemPTD->lpSISCur;
        lpMemPTD->lpSISNxt->lpSISNxt      = (LPSIS_HEADER) ByteAddr (lpMemPTD->lpSISNxt, sizeof (SIS_HEADER));

        // Link this SIS into the chain
        if (lpMemPTD->lpSISCur)
            lpMemPTD->lpSISCur->lpSISNxt = lpMemPTD->lpSISNxt;
        lpMemPTD->lpSISCur = lpMemPTD->lpSISNxt;
        lpMemPTD->lpSISNxt = lpMemPTD->lpSISNxt->lpSISNxt;
    } __except (CheckException (GetExceptionInformation ()))
    {
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_ACCESS_VIOLATION:
            {
                MEMORY_BASIC_INFORMATION mbi;

                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                // See how many pages are already allocated
                VirtualQuery (lpMemPTD->lpSISNxt,
                             &mbi,
                              sizeof (mbi));

                // Check for no allocation as yet
                if (mbi.State EQ MEM_RESERVE)
                    mbi.RegionSize = 0;

                // Allocate more memory to the YYRes buffer
                if (VirtualAlloc (lpMemPTD->lpSISNxt,
                                  mbi.RegionSize + DEF_SIS_INCRSIZE * sizeof (SYMENTRY),
                                  MEM_COMMIT,
                                  PAGE_READWRITE) NE NULL)
                    goto RESTART_EXCEPTION_FILLSISNXT;

                // Fall through to never-never-land

            } // End EXCEPTION_ACCESS_VIOLATION

            defstop
                break;
        } // End SWITCH
    } // End __try/__except
} // End FillSISNxt


//***************************************************************************
//  $PassSigaphore
//
// If this hSigaphore is not for this level, pass it on up the line
//***************************************************************************

LPSIS_HEADER PassSigaphore
    (LPPERTABDATA lpMemPTD,
     HANDLE       hSigaphore)
{
    if (hSigaphore
     && lpMemPTD->lpSISCur
     && hSigaphore NE lpMemPTD->lpSISCur->hSemaphore)
    {
        Assert (lpMemPTD->lpSISCur->hSigaphore EQ NULL);

        // Pass it on up the line
        lpMemPTD->lpSISCur->hSigaphore = hSigaphore;
        hSigaphore = NULL;
    } // End IF

    return hSigaphore;
} // End PassSigaphore


//***************************************************************************
//  $GetImmTypeFcn
//
//  Get the IMMTYPE_PRIMFCN value corresponding to a primitive function
//***************************************************************************

UINT GetImmTypeFcn
    (WCHAR wch)                 // Immediate function symbol (UTF16_xxx)

{
    return IMMTYPE_PRIMFCN;

////// Split cases based upon the immediate function symbol
////switch (wch)
////{
////    case UTF16_BAR:
////    case UTF16_CIRCLE:
////    case UTF16_CIRCLESTAR:
////    case UTF16_COLONBAR:
////    case UTF16_DOWNSTILE:
////    case UTF16_PLUS:
////    case UTF16_QUOTEDOT:
////    case UTF16_STAR:
////    case UTF16_STILE:
////    case UTF16_TIMES:
////    case UTF16_UPSTILE:
////        return IMMTYPE_PRIMFCN_SS;      // Monadic scalar     , dyadic scalar
////
////    case UTF16_QUERY:
////    case UTF16_TILDE:
////        return IMMTYPE_PRIMFCN_SM;      // Monadic scalar     , dyadic mixed/error
////
////    case UTF16_DOWNCARET:
////    case UTF16_DOWNCARETTILDE:
////    case UTF16_EQUAL:
////    case UTF16_LEFTCARET:
////    case UTF16_LEFTCARETUNDERBAR:
////    case UTF16_NOTEQUAL:
////    case UTF16_RIGHTCARET:
////    case UTF16_RIGHTCARETUNDERBAR:
////    case UTF16_UPCARET:
////    case UTF16_UPCARETTILDE:
////        return IMMTYPE_PRIMFCN_MS;      // Monadic mixed/error, dyadic scalar
////
////    case UTF16_ALPHA:
////    case UTF16_CIRCLEBAR:
////    case UTF16_CIRCLESLOPE:
////    case UTF16_CIRCLESTILE:
////    case UTF16_COMMA:
////    case UTF16_COMMABAR:
////    case UTF16_DELSTILE:
////    case UTF16_DELTASTILE:
////    case UTF16_DOMINO:
////    case UTF16_DOWNARROW:
////    case UTF16_DOWNTACK:
////    case UTF16_DOWNTACKJOT:
////    case UTF16_EPSILON:
////    case UTF16_EPSILONUNDERBAR:
////    case UTF16_EQUALUNDERBAR:
////    case UTF16_IOTA:
////    case UTF16_IOTAUNDERBAR:
////    case UTF16_LEFTSHOE:
////    case UTF16_LEFTTACK:
////    case UTF16_OMEGA:
////    case UTF16_RIGHTSHOE:
////    case UTF16_RIGHTTACK:
////    case UTF16_RHO:
////    case UTF16_SLASH:
////    case UTF16_SLASHBAR:
////    case UTF16_SLOPE:
////    case UTF16_SLOPEBAR:
////    case UTF16_SQUAD:
////    case UTF16_UPARROW:
////    case UTF16_UPSHOE:
////    case UTF16_UPTACK:
////    case UTF16_UPTACKJOT:
////        return IMMTYPE_PRIMFCN_MM;      // Monadic mixed/error, dyadic mixed/error
////
////    defstop
////        return 0;
////} // End SWITCH
} // End GetImmTypeFcn


//***************************************************************************
//  End of File: primfns.c
//***************************************************************************
