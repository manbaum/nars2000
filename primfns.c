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
#include "primspec.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

// The jump table for all primitive functions
LPPRIMFNS PrimFnsTab[256];

// As these functions are implemented, delete the
//   appropriate line.

// Primitives To Be Done                               Monadic      Dyadic
#define PrimFnAlpha_EM              PrimFn_EM       // ERROR        ERROR
#define PrimFnDelStile_EM           PrimFn_EM       // Mixed        Mixed
#define PrimFnDeltaStile_EM         PrimFn_EM       // Mixed        Mixed
#define PrimFnDownArrow_EM          PrimFn_EM       // ERROR        Mixed
#define PrimFnDownShoe_EM           PrimFn_EM       // Mixed        ERROR
#define PrimFnDownTack_EM           PrimFn_EM       // ERROR        Mixed
#define PrimFnEpsilonUnderbar_EM    PrimFn_EM       // ERROR        Mixed
#define PrimFnHydrant_EM            PrimFn_EM       // Mixed        ERROR
#define PrimFnIotaUnderbar_EM       PrimFn_EM       // ERROR        ERROR
#define PrimFnOmega_EM              PrimFn_EM       // ERROR        ERROR
#define PrimFnRightShoe_EM          PrimFn_EM       // Mixed        Mixed
#define PrimFnSquad_EM              PrimFn_EM       // ERROR        Mixed
#define PrimFnUpArrow_EM            PrimFn_EM       // Mixed        Mixed
#define PrimFnUpShoe_EM             PrimFn_EM       // ERROR        ERROR
#define PrimFnUpTack_EM             PrimFn_EM       // ERROR        Mixed


// Primitives Done                                     Monadic      Dyadic
/////// PrimFnBar_EM                                // Scalar       Scalar
/////// PrimFnCircle_EM                             // Scalar       Scalar
/////// PrimFnCircleBar_EM                          // Mixed        Mixed
/////// PrimFnCircleSlope_EM                        // Mixed        Mixed
/////// PrimFnCircleStar_EM                         // Scalar       Scalar
/////// PrimFnCircleStile_EM                        // Mixed        Mixed
/////// PrimFnComma_EM                              // Mixed        Mixed
/////// PrimFnCommaBar_EM                           // Mixed        Mixed
/////// PrimFnDivide_EM                             // Scalar       Scalar
/////// PrimFnDomino_EM                             // Mixed        Mixed
/////// PrimFnDownCaret_EM                          // ERROR        Scalar
/////// PrimFnDownCaretTilde_EM                     // ERROR        Scalar
/////// PrimFnDownStile_EM                          // Scalar       Scalar
/////// PrimFnEpsilon_EM                            // Mixed        Mixed (*)
/////// PrimFnEqual_EM                              // ERROR        Scalar
/////// PrimFnEqualUnderbar_EM                      // Mixed        Mixed
/////// PrimFnIota_EM                               // Mixed        Mixed (*)
/////// PrimFnLeftCaret_EM                          // ERROR        Scalar
/////// PrimFnLeftShoe_EM                           // Mixed        Mixed (*)
/////// PrimFnLeftTack_EM                           // ERROR        Mixed
/////// PrimFnNotEqual_EM                           // ERROR        Scalar
/////// PrimFnNotLess_EM                            // ERROR        Scalar
/////// PrimFnNotMore_EM                            // ERROR        Scalar
/////// PrimFnPlus_EM                               // Scalar       Scalar
/////// PrimFnQuoteDot_EM                           // Scalar       Scalar
/////// PrimFnQuery_EM                              // Scalar       Mixed
/////// PrimFnRightCaret_EM                         // ERROR        Scalar
/////// PrimFnRightTack_EM                          // ERROR        Mixed
/////// PrimFnRho_EM                                // Mixed        Mixed
/////// PrimFnSlash_EM                              // ERROR        Mixed
/////// PrimFnSlashBar_EM                           // ERROR        Mixed
/////// PrimFnSlope_EM                              // ERROR        Mixed
/////// PrimFnSlopeBar_EM                           // ERROR        Mixed
/////// PrimFnStar_EM                               // Scalar       Scalar
/////// PrimFnStile_EM                              // Scalar       Scalar
/////// PrimFnThorn_EM                              // Mixed        Mixed (*)
/////// PrimFnTilde_EM                              // Scalar       Mixed (*)
/////// PrimFnTimes_EM                              // Scalar       Scalar
/////// PrimFnUpCaret_EM                            // ERROR        Scalar
/////// PrimFnUpCaretTilde_EM                       // ERROR        Scalar
/////// PrimFnUpStile_EM                            // Scalar       Scalar

// (*) = Unfinished

// First coordinate functions handled by common function
#define PrimFnSlashBar_EM       PrimFnSlash_EM
#define PrimFnSlopeBar_EM       PrimFnSlope_EM
#define PrimFnCircleBar_EM      PrimFnCircleStile_EM
#define PrimFnCommaBar_EM       PrimFnComma_EM


//***************************************************************************
//  InitPrimFns
//
//  Initialize the primitive function and operator jump tables
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- InitPrimFns"
#else
#define APPEND_NAME
#endif

void InitPrimFns
    (void)

{
    APLINT aplInteger;

    //****************************************************
    //  Primitive Functions
    //  Primitive operators are handled case-by-case in
    //    ExecFunc_EM
    //****************************************************

////InitPrimOp1(UCS2_JOTDOT         , &PrimOp1Jotdot_EM        );   // Pseudo-symbol

    InitPrimFn (UCS2_ALPHA          , &PrimFnAlpha_EM          );   // Alt-'a' - alpha
    InitPrimFn (UCS2_UPTACK         , &PrimFnUpTack_EM         );   // Alt-'b' - up tack
    InitPrimFn (UCS2_UPSHOE         , &PrimFnUpShoe_EM         );   // Alt-'c' - up shoe
    InitPrimFn (UCS2_DOWNSTILE      , &PrimFnDownStile_EM      );   // Alt-'d' - down stile
    InitPrimFn (UCS2_EPSILON        , &PrimFnEpsilon_EM        );   // Alt-'e' - epsilon
////                                                                // Alt-'f' - underbar
////                                                                // Alt-'g' - del
////                                                                // Alt-'h' - delta
    InitPrimFn (UCS2_IOTA           , &PrimFnIota_EM           );   // Alt-'i' - iota
////InitPrimOp2(UCS2_JOT            , &PrimOp2Jot_EM           );   // Alt-'j' - jot
////                                                                // Alt-'k' - single quote
////                                                                // Alt-'l' - quad
    InitPrimFn (UCS2_STILE          , &PrimFnStile_EM          );   // Alt-'m' - stile
    InitPrimFn (UCS2_DOWNTACK       , &PrimFnDownTack_EM       );   // Alt-'n' - down tack
    InitPrimFn (UCS2_CIRCLE         , &PrimFnCircle_EM         );   // Alt-'o' - circle
    InitPrimFn (UCS2_STAR           , &PrimFnStar_EM           );   // Alt-'p' - star
    InitPrimFn (UCS2_QUERY          , &PrimFnQuery_EM          );   // Alt-'q' - question mark
    InitPrimFn (UCS2_RHO            , &PrimFnRho_EM            );   // Alt-'r' - rho
    InitPrimFn (UCS2_UPSTILE        , &PrimFnUpStile_EM        );   // Alt-'s' - up stile
    InitPrimFn (UCS2_TILDE          , &PrimFnTilde_EM          );   // Alt-'t' - tilde
    InitPrimFn (UCS2_DNARROW        , &PrimFnDownArrow_EM      );   // Alt-'u' - down arrow
    InitPrimFn (UCS2_DOWNSHOE       , &PrimFnDownShoe_EM       );   // Alt-'v' - down shoe
    InitPrimFn (UCS2_OMEGA          , &PrimFnOmega_EM          );   // Alt-'w' - omega
    InitPrimFn (UCS2_RIGHTSHOE      , &PrimFnRightShoe_EM      );   // Alt-'x' - right shoe
    InitPrimFn (UCS2_UPARROW        , &PrimFnUpArrow_EM        );   // Alt-'y' - up arrow
    InitPrimFn (UCS2_LEFTSHOE       , &PrimFnLeftShoe_EM       );   // Alt-'z' - left shoe
    InitPrimFn (UCS2_EQUALUNDERBAR  , &PrimFnEqualUnderbar_EM  );   // Alt-'!' - match
////                                                                // Alt-'"' - (none)
    InitPrimFn (UCS2_DELSTILE       , &PrimFnDelStile_EM       );   // Alt-'#' - grade-down
    InitPrimFn (UCS2_DELTASTILE     , &PrimFnDeltaStile_EM     );   // Alt-'$' - grade-up
    InitPrimFn (UCS2_CIRCLESTILE    , &PrimFnCircleStile_EM    );   // Alt-'%' - rotate
    InitPrimFn (UCS2_CIRCLESLOPE    , &PrimFnCircleSlope_EM    );   // Alt-'^' - transpose
    InitPrimFn (UCS2_CIRCLEBAR      , &PrimFnCircleBar_EM      );   // Alt-'&' - circle-bar
    InitPrimFn (UCS2_HYDRANT        , &PrimFnHydrant_EM        );   // Alt-'\''- execute
    InitPrimFn (UCS2_DOWNCARETTILDE , &PrimFnDownCaretTilde_EM );   // Alt-'(' - nor
    InitPrimFn (UCS2_UPCARETTILDE   , &PrimFnUpCaretTilde_EM   );   // Alt-')' - nand
    InitPrimFn (UCS2_CIRCLESTAR     , &PrimFnCircleStar_EM     );   // Alt-'*' - log
    InitPrimFn (UCS2_DOMINO         , &PrimFnDomino_EM         );   // Alt-'+' - domino
////                                                                // Alt-',' - lamp
    InitPrimFn (UCS2_TIMES          , &PrimFnTimes_EM          );   // Alt-'-' - times
    InitPrimFn (UCS2_SLOPEBAR       , &PrimFnSlopeBar_EM       );   // Alt-'.' - slope-bar
    InitPrimFn (UCS2_SLASHBAR       , &PrimFnSlashBar_EM       );   // Alt-'/' - slash-bar
    InitPrimFn (UCS2_UPCARET        , &PrimFnUpCaret_EM        );   // Alt-'0' - and (94??)
////InitPrimOp1(UCS2_DIERESIS       , &PrimOp1Dieresis_EM      );   // Alt-'1' - dieresis
////                                                                // Alt-'2' - overbar
    InitPrimFn (UCS2_LEFTCARET      , &PrimFnLeftCaret_EM      );   // Alt-'3' - less
    InitPrimFn (UCS2_NOTMORE        , &PrimFnNotMore_EM        );   // Alt-'4' - not more
    InitPrimFn (UCS2_EQUAL          , &PrimFnEqual_EM          );   // Alt-'5' - equal
    InitPrimFn (UCS2_NOTLESS        , &PrimFnNotLess_EM        );   // Alt-'6' - not less
    InitPrimFn (UCS2_RIGHTCARET     , &PrimFnRightCaret_EM     );   // Alt-'7' - more
    InitPrimFn (UCS2_NOTEQUAL       , &PrimFnNotEqual_EM       );   // Alt-'8' - not equal
    InitPrimFn (UCS2_DOWNCARET      , &PrimFnDownCaret_EM      );   // Alt-'9' - or
////                                                                // Alt-':' - (none)
    InitPrimFn (UCS2_THORN          , &PrimFnThorn_EM          );   // Alt-';' - format
////                                                                // Alt-'<' - (none)
    InitPrimFn (UCS2_DIVIDE         , &PrimFnDivide_EM         );   // Alt-'=' - divide
////                                                                // Alt-'>' - (none)
////                                                                // Alt-'?' - (none)
////                                                                // Alt-'@' - del-tilde
////                                                                // Alt-'A' - (none)
////                                                                // Alt-'B' - (none)
////                                                                // Alt-'C' - (none)
////                                                                // Alt-'D' - (none)
    InitPrimFn (UCS2_EPSILONUNDERBAR, &PrimFnEpsilonUnderbar_EM);   // Alt-'E' - epsilon-underbar
////                                                                // Alt-'F' - (none)
////                                                                // Alt-'G' - (none)
////                                                                // Alt-'H' - delta-underbar
    InitPrimFn (UCS2_IOTAUNDERBAR   , &PrimFnIotaUnderbar_EM   );   // Alt-'I' - iota-underbar
////                                                                // Alt-'J' - (none)
////                                                                // Alt-'K' - (none)
    InitPrimFn (UCS2_SQUAD          , &PrimFnSquad_EM          );   // Alt-'L' - squad
////                                                                // Alt-'M' - (none)
////                                                                // Alt-'N' - (none)
////                                                                // Alt-'O' - (none)
////                                                                // Alt-'P' - (none)
////                                                                // Alt-'Q' - (none)
////                                                                // Alt-'R' - (none)
////                                                                // Alt-'S' - (none)
////                                                                // Alt-'T' - (none)
////                                                                // Alt-'U' - (none)
////                                                                // Alt-'V' - (none)
////                                                                // Alt-'W' - (none)
////                                                                // Alt-'X' - (none)
////                                                                // Alt-'Y' - (none)
////                                                                // Alt-'Z' - (none)
////                                                                // Alt-'[' - left arrow
    InitPrimFn (UCS2_LEFTTACK       , &PrimFnLeftTack_EM       );   // Alt-'\\'- left tack
////                                                                // Alt-']' - right arrow
    InitPrimFn (UCS2_QUOTEDOT       , &PrimFnQuoteDot_EM       );   // Alt-'_' - quote-dot
////                                                                // Alt-'`' - diamond
////                                                                // Alt-'{' - quote-quad
    InitPrimFn (UCS2_RIGHTTACK      , &PrimFnRightTack_EM      );   // Alt-'|' - right tack
////                                                                // Alt-'}' - zilde
    InitPrimFn (UCS2_COMMABAR       , &PrimFnCommaBar_EM       );   // Alt-'~' - comma-bar
    InitPrimFn ('^'                 , &PrimFnUpCaret_EM        );
    InitPrimFn ('-'                 , &PrimFnBar_EM            );
    InitPrimFn ('+'                 , &PrimFnPlus_EM           );
    InitPrimFn ('|'                 , &PrimFnStile_EM          );
    InitPrimFn (','                 , &PrimFnComma_EM          );
    InitPrimFn ('/'                 , &PrimFnSlash_EM          );
    InitPrimFn ('\\'                , &PrimFnSlope_EM          );
////InitPrimOp2('.'                 , &PrimOp2Dot_EM           );

#define POS_INFINITY            (0x7FF0000000000000)
#define NEG_INFINITY            (0xFFF0000000000000)
#define QUIET_NAN               (0xFFF8000000000000)
#define FLOAT2POW53             (0x4340000000000000)

    // Create various floating point constants
    aplInteger = POS_INFINITY; PosInfinity = *(double *) &aplInteger;
    aplInteger = NEG_INFINITY; NegInfinity = *(double *) &aplInteger;
    aplInteger = FLOAT2POW53;  Float2Pow53 = *(double *) &aplInteger;

} // End InitPrimFns
#undef  APPEND_NAME


//***************************************************************************
//  MakePermVars
//
//  Make various permanent variables
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakePermVars"
#else
#define APPEND_NAME
#endif

void MakePermVars
    (void)

{
    LPVARARRAY_HEADER lpHeader;

    // Create zilde
////hGlbZilde = DbgGlobalAlloc (GHND, sizeof (VARARRAY_HEADER)
////                                + sizeof (APLDIM) * 1);
    hGlbZilde = DbgGlobalAlloc (GHND, (UINT) CalcArraySize (ARRAY_BOOL, 0, 1));
    if (!hGlbZilde)
    {
        DbgStop ();         // We should never get here
    } // End IF

    // Lock the memory to get a ptr to it
    lpHeader = MyGlobalLock (hGlbZilde);

    // Fill in the header values
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_BOOL;
    lpHeader->Perm       = 1;       // So we don't free it
////lpHeader->SysVar     = 0;       // Already zero from GHND
////lpHeader->RefCnt     = 0;       // Ignore as this is perm
////lpHeader->NELM       = 0;       // Already zero from GHND
    lpHeader->Rank       = 1;

////// Mark as zero length
////*VarArrayBaseToDim (lpHeader) = 0;  // Already zero from GHND

    // We no longer need this ptr
    MyGlobalUnlock (hGlbZilde); lpHeader = NULL;

    // Create various permanent char vectors
    hGlbQuadDM  = MakePermCharVector (WS_QUADDM);
    hGlbMTChar  = MakePermCharVector (MTChar);
    hGlbSAEmpty = hGlbMTChar;
    hGlbSAClear = MakePermCharVector (SAClear);
    hGlbSAError = MakePermCharVector (SAError);
    hGlbSAExit  = MakePermCharVector (SAExit);
    hGlbSAOff   = MakePermCharVector (SAOff);
    hGlbQuadWSID_CWS = hGlbMTChar;
} // End MakePermVars
#undef  APPEND_NAME


//***************************************************************************
//  MakePermCharVector
//
//  Make a permanent character vector
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakePermCharVector"
#else
#define APPEND_NAME
#endif

HGLOBAL MakePermCharVector
    (LPWCHAR lpwc)

{
    HGLOBAL hGlbRes;
    UINT    uLen;
    LPVARARRAY_HEADER lpHeader;

    // Get the string length
    uLen = lstrlenW (lpwc);

////hGlbRes = DbgGlobalAlloc (GHND, sizeof (VARARRAY_HEADER)
////                              + sizeof (APLDIM) * 1
////                              + sizeof (APLCHAR) * uLen);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) CalcArraySize (ARRAY_CHAR, uLen, 1));
    if (!hGlbRes)
    {
        DbgStop ();         // We should never get here
    } // End IF

    // Lock the memory to get a ptr to it
    lpHeader = MyGlobalLock (hGlbRes);

    // Fill in the header values
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
    lpHeader->Perm       = 1;       // So we don't free it
////lpHeader->SysVar     = 0;       // Already zero from GHND
////lpHeader->RefCnt     = 0;       // Ignore as this is perm
    lpHeader->NELM       = uLen;
    lpHeader->Rank       = 1;

    // Save the dimension
    *VarArrayBaseToDim (lpHeader) = uLen;

    // Skip over the header and dimensions to the data
    lpHeader = VarArrayBaseToData (lpHeader, 1);

    // Copy the data to memory
    CopyMemory (lpHeader, lpwc, uLen * sizeof (APLCHAR));

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpHeader = NULL;

    return hGlbRes;
} // End MakePermCharVector
#undef  APPEND_NAME


//***************************************************************************
//  InitPrimFn
//
//  Initialize a single primitive function
//***************************************************************************

void InitPrimFn
    (WCHAR     wchFn,
     LPPRIMFNS PrimFn)

{
    if (PrimFnsTab[(unsigned char) wchFn])
        OverflowPrimFn (wchFn, PrimFn);
    else
        PrimFnsTab[(unsigned char) wchFn] = PrimFn;
} // End InitPrimFn


//***************************************************************************
//  PrimFn_EM
//
//  Default function for overflow
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFn_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFn_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFn_EM
#undef  APPEND_NAME


//***************************************************************************
//  OverflowPrimFn0A_EM
//
//  Overflow function for Epsilon (220A) and DownStile (230A)
//***************************************************************************

LPYYSTYPE OverflowPrimFn0A_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Split cases based upon the token char
    switch (lptkFunc->tkData.tkChar)
    {
        case UCS2_EPSILON:
            return PrimFnEpsilon_EM     (lptkLftArg,
                                         lptkFunc,
                                         lptkRhtArg,
                                         lptkAxis);
        case UCS2_DOWNSTILE:
            return PrimFnDownStile_EM   (lptkLftArg,
                                         lptkFunc,
                                         lptkRhtArg,
                                         lptkAxis);
        defstop
            return NULL;
    } // End SWITCH
} // End OverflowPrimFn0A_EM


//***************************************************************************
//  OverflowPrimFn2A_EM
//
//  Overflow function for Star (002A) and DownShoe (222A)
//***************************************************************************

LPYYSTYPE OverflowPrimFn2A_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Split cases based upon the token char
    switch (lptkFunc->tkData.tkChar)
    {
        case UCS2_STAR:
            return PrimFnStar_EM        (lptkLftArg,
                                         lptkFunc,
                                         lptkRhtArg,
                                         lptkAxis);
        case UCS2_DOWNSHOE:
            return PrimFnDownShoe_EM    (lptkLftArg,
                                         lptkFunc,
                                         lptkRhtArg,
                                         lptkAxis);
        defstop
            return NULL;
    } // End SWITCH
} // End OverflowPrimFn2A_EM


//***************************************************************************
//  OverflowPrimFn3D_EM
//
//  Overflow function for Equal (003D) and CircleStile (233D)
//***************************************************************************

LPYYSTYPE OverflowPrimFn3D_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Split cases based upon the token char
    switch (lptkFunc->tkData.tkChar)
    {
        case UCS2_EQUAL:
            return PrimFnEqual_EM       (lptkLftArg,
                                         lptkFunc,
                                         lptkRhtArg,
                                         lptkAxis);
        case UCS2_CIRCLESTILE:
            return PrimFnCircleStile_EM (lptkLftArg,
                                         lptkFunc,
                                         lptkRhtArg,
                                         lptkAxis);
        defstop
            return NULL;
    } // End SWITCH
} // End OverflowPrimFn3D_EM


//***************************************************************************
//  OverflowPrimFn3F_EM
//
//  Overflow function for Query (003F) and SlashBar (233F)
//***************************************************************************

LPYYSTYPE OverflowPrimFn3F_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Split cases based upon the token char
    switch (lptkFunc->tkData.tkChar)
    {
        case UCS2_QUERY:
            return PrimFnQuery_EM       (lptkLftArg,
                                         lptkFunc,
                                         lptkRhtArg,
                                         lptkAxis);
        case UCS2_SLASHBAR:
            return PrimFnSlashBar_EM    (lptkLftArg,
                                         lptkFunc,
                                         lptkRhtArg,
                                         lptkAxis);
        defstop
            return NULL;
    } // End SWITCH
} // End OverflowPrimFn3F_EM


//***************************************************************************
//  OverflowPrimFn
//
//  Handle overflow in table of primitive functions
//***************************************************************************

void OverflowPrimFn
    (WCHAR     wchFn,
     LPPRIMFNS PrimFn)

{
    // Here are the overflow cases we know about

    // Split cases based upon the low-order byte
    switch ((unsigned char) wchFn)
    {
        case 0x0A:
            PrimFnsTab[(unsigned char) wchFn] = &OverflowPrimFn0A_EM;

            break;

        case 0x2A:
            PrimFnsTab[(unsigned char) wchFn] = &OverflowPrimFn2A_EM;

            break;

        case 0x3D:
            PrimFnsTab[(unsigned char) wchFn] = &OverflowPrimFn3D_EM;

            break;

        case 0x3F:
            PrimFnsTab[(unsigned char) wchFn] = &OverflowPrimFn3F_EM;

            break;

        defstop
            // If we get here, we have another overflow conflict to resolve
            break;
    } // End SWITCH
} // End OverflowPrimFn


//***************************************************************************
//  ExecFunc_EM
//
//  Execute a user-defined, system, or primitive function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecFunc_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE ExecFunc_EM
    (LPTOKEN   lptkLftArg,      // The left argument (may be NULL)
     LPTOKEN   lptkFunc,        // The function token
     LPTOKEN   lptkRhtArg,      // The right argument
     LPTOKEN   lptkAxis)        // The axis (may be NULL)   // ***FIXME*** -- Is it ever non-NULL?

{
    LPPRIMFNS PrimFn;

    DBGENTER;

    // Split cases based upon the function token type
    switch (lptkFunc->tkFlags.TknType)
    {
        case TKT_FCNIMMED:
            PrimFn = PrimFnsTab[(unsigned char) (lptkFunc->tkData.tkChar)];
            if (!PrimFn)
            {
                ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                           lptkFunc);
                return NULL;
            } // End IF

            return (*PrimFn) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);

        case TKT_FCNNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkFunc->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If the LPSYMENTRY is not immediate, it must be an HGLOBAL
            if (!lptkFunc->tkData.lpSym->stFlags.Imm)
            {
                STFLAGS stFlags;
                HGLOBAL hGlbFcn;

                // Skip assertion if it's some kind of function
                stFlags = lptkFunc->tkData.lpSym->stFlags;
                if (stFlags.SysFn0
                 || stFlags.SysFn12
                 || stFlags.UsrFn0
                 || stFlags.UsrFn12
                 || stFlags.UsrOp1
                 || stFlags.UsrOp2)
                {
                    LPPRIMFNS lpNameFcn;

                    // Get the address of the execution routine
                    lpNameFcn = lptkFunc->tkData.lpSym->stData.stNameFcn;

                    // If it's a direct function, go there
                    if (lptkFunc->tkFlags.FcnDir)
                        return (*lpNameFcn) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);

                    // Save the HGLOBAL
                    hGlbFcn = lpNameFcn;
                } else
                    // Save the HGLOBAL
                    hGlbFcn = lptkFunc->tkData.lpSym->stData.stGlbData;

                // stData is a valid HGLOBAL function array
                Assert (IsGlbTypeFcnDir (hGlbFcn));

                return ExecFuncGlb_EM (lptkLftArg,
                                       ClrPtrTypeDirGlb (hGlbFcn),
                                       lptkRhtArg,
                                       lptkAxis);
            } // End IF

            // Handle the immediate case

            // Split cases based upon the immediate type
            switch (lptkFunc->tkData.lpSym->stFlags.ImmType)
            {
                case IMMTYPE_PRIMFCN:
                {
                    TOKEN tkFn = {0};

                    PrimFn = PrimFnsTab[(unsigned char) (lptkFunc->tkData.lpSym->stData.stChar)];
                    if (!PrimFn)
                    {
                        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                                   lptkFunc);
                        return NULL;
                    } // End IF

                    // Fill in for PrimFn*** test
                    tkFn.tkFlags.TknType   = TKT_FCNIMMED;
                    tkFn.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////////////////////tkFn.tkFlags.NoDisplay = 0;     // Already zero from {0}
                    tkFn.tkData.tkChar     = lptkFunc->tkData.lpSym->stData.stChar;
                    tkFn.tkCharIndex       = lptkFunc->tkCharIndex;

                    return (*PrimFn) (lptkLftArg, &tkFn, lptkRhtArg, lptkAxis);
                } // End IMMTYPE_PRIMFCN

                defstop
                    break;
            } // End SWITCH

        case TKT_FCNARRAY:
            // tkData is a valid HGLOBAL function array
            Assert (IsGlbTypeFcnDir (lptkFunc->tkData.tkGlbData));

            return ExecFuncGlb_EM (lptkLftArg,
                                   ClrPtrTypeDirGlb (lptkFunc->tkData.tkGlbData),
                                   lptkRhtArg,
                                   lptkAxis);
        defstop
            break;
    } // End SWITCH

    return NULL;
} // End ExecFunc_EM
#undef  APPEND_NAME


//***************************************************************************
//  ExecFuncGlb_EM
//
//  Execute a function in global memory
//***************************************************************************

LPYYSTYPE ExecFuncGlb_EM
    (LPTOKEN lptkLftArg,
     HGLOBAL hGlbFcn,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    LPYYSTYPE lpYYFcnStr, lpYYRes;

    // Lock the global memory to get a ptr to it
    lpYYFcnStr = MyGlobalLock (hGlbFcn);

    // Skip over the header to the data
    lpYYFcnStr = FcnArrayBaseToData (lpYYFcnStr);

    // The contents of the global memory object consist of
    //   a series of YYSTYPEs in RPN order.
    lpYYRes = ExecFuncStr_EM (lptkLftArg,
                              lpYYFcnStr,
                              lptkRhtArg);
    // We no longer need this ptr
    MyGlobalUnlock (hGlbFcn); lpYYFcnStr = NULL;

    return lpYYRes;
} // End ExecFuncGlb_EM


//***************************************************************************
//  ExecFuncStr_EM
//
//  Execute a function strand
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecFuncStr_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE ExecFuncStr_EM
    (LPTOKEN   lptkLftArg,
     LPYYSTYPE lpYYFcnStr,
     LPTOKEN   lptkRhtArg)

{
    LPTOKEN   lptkAxis;
    LPPRIMFNS PrimFn;

    // Split cases based upon the type of the first token
    switch (lpYYFcnStr->tkToken.tkFlags.TknType)
    {
        case TKT_OP1IMMED:
            return ExecOp1_EM (lptkLftArg,
                               lpYYFcnStr,
                               lptkRhtArg);
            break;

        case TKT_OP2IMMED:
            return ExecOp2_EM (lptkLftArg,
                               lpYYFcnStr,
                               lptkRhtArg);
            break;

        case TKT_OP1NAMED:
            DbgBrk ();      // ***FINISHME***


            break;

        case TKT_OP2NAMED:
            DbgBrk ();      // ***FINISHME***


            break;

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

            PrimFn = PrimFnsTab[(unsigned char) (lpYYFcnStr->tkToken.tkData.tkChar)];
            if (!PrimFn)
            {
                ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                           &lpYYFcnStr->tkToken);
                return NULL;
            } // End IF

            return (*PrimFn) (lptkLftArg, &lpYYFcnStr->tkToken, lptkRhtArg, lptkAxis);

        defstop
            break;
    } // End SWITCH

    return NULL;
} // End ExecFuncStr_EM
#undef  APPEND_NAME


//***************************************************************************
//  ExecOp1_EM
//
//  Execute a monadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecOp1_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE ExecOp1_EM
    (LPTOKEN   lpYYLftArg,
     LPYYSTYPE lpYYFcnStr,
     LPTOKEN   lpYYRhtArg)

{
    LPTOKEN lptkAxis;
    UINT    YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = NewYYResIndex ();

    // Check for axis operator
    if (lpYYFcnStr->FcnCount > 1
     && (lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
      || lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
        lptkAxis = &lpYYFcnStr[1].tkToken;
     else
        lptkAxis = NULL;

    // Split cases based upon the type of the monadic operator
    switch (lpYYFcnStr->tkToken.tkData.tkChar)
    {
        case UCS2_SLASH:        // Reduction along the last coordinate
            DbgBrk ();          // ***FINISHME***


            break;

        case UCS2_SLASHBAR:     // Reduction along the first coordinate
            DbgBrk ();          // ***FINISHME***


            break;

        case UCS2_SLOPE:        // Scan along the last coordinate
            DbgBrk ();          // ***FINISHME***


            break;

        case UCS2_SLOPEBAR:     // Scan along the first coordinate
            DbgBrk ();          // ***FINISHME***


            break;

        case UCS2_DIERESIS:     // Each
            DbgBrk ();          // ***FINISHME***


            break;

        case UCS2_DIERESISTILDE:// Commute
            DbgBrk ();          // ***FINISHME***


            break;

        defstop
            break;
    } // End SWITCH

    return &YYRes[YYLclIndex];
} // End ExecOp1_EM
#undef  APPEND_NAME


//***************************************************************************
//  TokenTypeFV
//
//  Return the type of a token
//
//  'F' = Function
//  'V' = Variable
//  '1' = Monadic operator
//  '2' = Dyadic operator
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
            return '1';

        case TKT_OP2IMMED:
        case TKT_OP2NAMED:
            return '2';

        case TKT_AXISIMMED:
        case TKT_AXISARRAY:
        case TKT_JOTDOT:
        case TKT_LPAREN:
        case TKT_RPAREN:
        case TKT_LBRACKET:
        case TKT_RBRACKET:
        case TKT_COMMENT:
        case TKT_ASSIGN:
        case TKT_LISTSEP:
        case TKT_EOS:
        case TKT_EOL:
        case TKT_LINECONT:
        case TKT_STRAND:
        case TKT_LIST:
        case TKT_STRNAMED:
        defstop
            return '?';
    } // End SWITCH
} // End TokenTypeFV


//***************************************************************************
//  ExecOp2_EM
//
//  Execute a dyadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecOp2_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE ExecOp2_EM
    (LPTOKEN   lptkLftArg,
     LPYYSTYPE lpYYFcnStr,
     LPTOKEN   lptkRhtArg)

{
    UINT YYLclIndex;

    // Split cases based upon the type of the dyadic operator
    switch (lpYYFcnStr->tkToken.tkData.tkChar)
    {
        case UCS2_JOTDOT:       // Outer product
            return ExecOuterProd_EM (lptkLftArg,
                                    &lpYYFcnStr[1],
                                     lptkRhtArg);
        case '.':               // Inner product
            return ExecInnerProd_EM (lptkLftArg,
                                    &lpYYFcnStr[1],
                                    &lpYYFcnStr[lpYYFcnStr[1].FcnCount + 1],
                                     lptkRhtArg);
        case UCS2_JOT:          // Compose
        {
            LPYYSTYPE lpYYRes;
            UINT      uLftArg,
                      uRhtArg;

            // Split cases based upon the left operand's token type
            switch (TokenTypeFV (&lpYYFcnStr[1].tkToken))
            {
                case 'F':
                    uLftArg = 0;

                    break;

                case 'V':
                    uLftArg = 1;

                    break;

                defstop
                    break;
            } // End SWITCH

            // Split cases based upon the right operand's token type
            switch (TokenTypeFV (&lpYYFcnStr[lpYYFcnStr[1].FcnCount + 1].tkToken))
            {
                case 'F':
                    uRhtArg = 0;

                    break;

                case 'V':
                    uRhtArg = 1;

                    break;

                defstop
                    break;
            } // End SWITCH

            // Split cases based upon the type (V or F) of
            //   the left and right operands
            switch (uLftArg * 2 + uRhtArg * 1)
            {
                case 0 * 2 + 0 * 1:     // F1 op2 F2 -> F1 F2 R or L F1 F2 R
                    // Execute the right operand monadically
                    //   on the right arg
                    lpYYRes = ExecFuncStr_EM (NULL,
                                             &lpYYFcnStr[lpYYFcnStr[1].FcnCount + 1],
                                              lptkRhtArg);
                    if (lpYYRes)
                    {
                        // Get new index into YYRes
                        YYLclIndex = NewYYResIndex ();

                        // Save the contents of the previous token
                        //   so we can free it when we're done
                        YYRes[YYLclIndex] = *lpYYRes;   // Save it

                        // Execute the left operand dyadically
                        //   between the (optional) left arg and the
                        //   above result from the right operand.
                        lpYYRes = ExecFuncStr_EM (lptkLftArg,
                                                 &lpYYFcnStr[1],
                                                 &YYRes[YYLclIndex].tkToken);
                        FreeResult (&YYRes[YYLclIndex].tkToken);
                    } // End IF

                    break;

                case 1 * 2 + 0 * 1:     // V op2 F -> V F R
                    // If there's a left arg, signal a SYNTAX ERROR
                    if (lptkLftArg)
                    {
                        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                                  &lpYYFcnStr->tkToken);
                        return NULL;
                    } // End IF

                    // Execute the right operand dyadically
                    //   between the left operand and the right arg.
                    lpYYRes = ExecFuncStr_EM (&lpYYFcnStr[1].tkToken,
                                              &lpYYFcnStr[lpYYFcnStr[1].FcnCount + 1],
                                               lptkRhtArg);
                    break;

                case 0 * 2 + 1 * 1:     // F op2 V
                    // If there's a left arg, signal a SYNTAX ERROR
                    if (lptkLftArg)
                    {
                        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                                  &lpYYFcnStr->tkToken);
                        return NULL;
                    } // End IF

                    // Execute the left operand dyadically
                    //   between the right arg and the right operand.
                    lpYYRes = ExecFuncStr_EM (lptkRhtArg,
                                             &lpYYFcnStr[1],
                                             &lpYYFcnStr[lpYYFcnStr[1].FcnCount + 1].tkToken);
                    break;

                case 1 * 2 + 1 * 1:     // V op2 V
                    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                              &lpYYFcnStr->tkToken);
                    break;

                defstop
                    break;
            } // End SWITCH

            return lpYYRes;
        } // End UCS2_JOT

        case UCS2_DIERESISDEL:  // Dual
            DbgBrk ();          // ***FINISHME***


            return NULL;        // break;

        case UCS2_DIERESISJOT:  // Rank
            DbgBrk ();          // ***FINISHME***


            return NULL;        // break;

        case UCS2_DIERESISDOWNTACK:// Convolution
            DbgBrk ();          // ***FINISHME***


            return NULL;        // break;

        case UCS2_DIERESISSTAR: // Power
            DbgBrk ();          // ***FINISHME***


            return NULL;        // break;

        defstop
            return NULL;
    } // End SWITCH
} // End ExecOp2_EM
#undef  APPEND_NAME


//***************************************************************************
//  ExecOuterProd_EM
//
//  Execute an outer product
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecOuterProd_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE ExecOuterProd_EM
    (LPTOKEN   lptkLftArg,
     LPYYSTYPE lpYYFcnStr,
     LPTOKEN   lptkRhtArg)

{
    UINT YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = NewYYResIndex ();

    DbgBrk ();                  // ***FINISHME***







    return &YYRes[YYLclIndex];
} // End ExecOuterProd_EM
#undef  APPEND_NAME


//***************************************************************************
//  ExecInnerProd_EM
//
//  Execute an inner product
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecInnerProd_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE ExecInnerProd_EM
    (LPTOKEN   lptkLftArg,
     LPYYSTYPE lpYYLftFcnStr,
     LPYYSTYPE lpYYRhtFcnStr,
     LPTOKEN   lptkRhtArg)

{
    UINT YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = NewYYResIndex ();

    DbgBrk ();                  // ***FINISHME***







    return &YYRes[YYLclIndex];
} // End ExecInnerProd_EM
#undef  APPEND_NAME


//***************************************************************************
//  RankOfGlb
//
//  Get the rank of a global memory object
//***************************************************************************

APLRANK RankOfGlb
    (HGLOBAL hGlb)

{
    LPVOID  lpMem;
    APLRANK aplRank;

    // Lock the global memory to get a ptr to it
    lpMem = MyGlobalLock (hGlb);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    aplRank = lpHeader->Rank;

#undef  lpHeader

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMem = NULL;

    return aplRank;
} // End RankOfGlb


//***************************************************************************
//  AttrsOfToken
//
//  Return the attributes (Type, NELM, and Rank) of a token.
//***************************************************************************

void AttrsOfToken
    (LPTOKEN   lpToken,
     APLSTYPE *lpaplType,
     LPAPLNELM lpaplNELM,
     LPAPLRANK lpaplRank)

{
    HGLOBAL hGlbData;

    // Split case based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lpToken->tkData.lpSym->stFlags.Imm)
            {
                // Get the global handle
                hGlbData = lpToken->tkData.lpSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbData));

                break;      // Continue with HGLOBAL case
            } // End IF

            *lpaplType = TranslateImmTypeToArrayType (lpToken->tkData.lpSym->stFlags.ImmType);
            *lpaplNELM = 1;
            *lpaplRank = 0;

            return;

        case TKT_VARIMMED:
            *lpaplType = TranslateImmTypeToArrayType (lpToken->tkFlags.ImmType);
            *lpaplNELM = 1;
            *lpaplRank = 0;

            return;

        case TKT_VARARRAY:
            // Get the global handle
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbData));

            break;      // Continue with HGLOBAL case

        case TKT_LIST:
            hGlbData = lpToken->tkData.tkGlbData;

            break;      // Continue with HGLOBAL case

        defstop
            return;
    } // End SWITCH

    // Handle the global case
    AttrsOfGlb (ClrPtrTypeDirGlb (hGlbData), lpaplType, lpaplNELM, lpaplRank, NULL);
} // End AttrsOfToken


//***************************************************************************
//  AttrsOfGlb
//
//  Return the attributes (Type, NELM, and Rank) of a global memory handle.
//***************************************************************************

void AttrsOfGlb
    (HGLOBAL   hGlbData,
     APLSTYPE *lpaplType,
     LPAPLNELM lpaplNELM,
     LPAPLRANK lpaplRank,
     LPAPLUINT lpaplCols)   // # cols in the array (scalar = 1) (may be NULL)

{
    LPVOID  lpMem;

    // Lock the global memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    *lpaplType = lpHeader->ArrType;
    *lpaplNELM = lpHeader->NELM;
    *lpaplRank = lpHeader->Rank;

#undef  lpHeader

    // Skip over the header to the dimensions
    if (lpaplCols)
    {
        if (*lpaplRank NE 0)
        {
            lpMem = VarArrayBaseToDim (lpMem);
            *lpaplCols = ((LPAPLDIM) lpMem)[*lpaplRank - 1];
        } else
            *lpaplCols = 1;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;
} // End AttrsOfGlb


//***************************************************************************
//  CheckAxisImm
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
     LPAPLNELM  lpaplNELM,      // Local var w/NELM
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
     LPAPLINT  *lplpAxisStart,  // Ptr to ptr to start of Axis values in *lphGlbAxis
     LPAPLINT  *lplpAxisHead,   // ...                    user axis values in *lphGlbAxis
     LPAPLUINT  lpaplAxisContLo)// Contiguous low axis (not NULL)

{
    APLUINT  ByteAxis;
    APLRANK  aplRank;
    BOOL     bRet = TRUE;
    UINT     u;
    LPAPLINT lpAxisTail;        // Ptr to second set of Axis values in *lphGlbAxis

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

        // Lock the memory to get a ptr to the axis storage area
        *lplpAxisStart = *lplpAxisHead = MyGlobalLock (*lphGlbAxis);

        // Point to the start of the trailing axes
        lpAxisTail = lplpAxisHead[aplRankCmp - *lpaplNELM];
    } // End IF

    // Split cases based upon the immediate axis type
    switch (lptkAxis->tkFlags.ImmType)
    {
        case IMMTYPE_BOOL:
        case IMMTYPE_INT:
            // Get the Boolean or Integer value,
            //   less the current index origin
            aplRank = lptkAxis->tkData.tkInteger - bQuadIO;

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
            **lplpAxisHead++ = u;

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
//  CheckAxisGlb
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
    BOOL     bRet = TRUE;
    LPVOID   lpMem,
             lpDup = NULL;
    HGLOBAL  hGlbDup = NULL;
    UINT     uBitMask;
    APLUINT  ByteDup,
             ByteAxis,
             u;
    APLSTYPE aplTypeLcl;
    APLRANK  aplRankLcl;
    LPAPLINT lpAxisTail;        // Ptr to second set of Axis values in *lphGlbAxis

    // st/tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (hGlbData));

    // Lock the global memory object to get a ptr to it
    lpMem = MyGlobalLock (ClrPtrTypeDirGlb (hGlbData));

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    aplTypeLcl = lpHeader->ArrType;
    *lpaplNELM = lpHeader->NELM;
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

        // Lock the memory to get a ptr to the axis storage area
        *lplpAxisStart = *lplpAxisHead = MyGlobalLock (*lphGlbAxis);

        // Point to the start of the trailing axes
        lpAxisTail = lplpAxisHead[aplRankCmp - *lpaplNELM];
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

#define lpaplAPA    ((LPAPLAPA) lpMem)

            // Get the APA data
            apaOff = lpaplAPA->Off;
            apaMul = lpaplAPA->Mul;
            apaLen = lpaplAPA->Len;

#undef  lpaplAPA
            // Convert to origin-0
            apaOff -= bQuadIO;

            // It's sufficient to check the length and
            //   the first and last values for validity.
            bRet = (apaLen EQ 0)
                || (((apaOff + apaMul *           0 ) < (APLRANKSIGN) aplRankCmp)
                 && ((apaOff + apaMul * (apaLen - 1)) < (APLRANKSIGN) aplRankCmp));

            // Save the trailing axis values
            if (bRet)
            for (u = 0; bRet && u < *lpaplNELM; u++)
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
//  CheckAxis_EM
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
            Assert (GetPtrTypeDir (lptkAxis->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, it's an HGLOBAL
            if (!lptkAxis->tkData.lpSym->stFlags.Imm)
            {
                // Get the global handle
                hGlbData = lptkAxis->tkData.lpSym->stData.stGlbData;
            } else
            {
                // Get the immediate type and value
                immType    = lptkAxis->tkData.lpSym->stFlags.ImmType;
                aplLongest = lptkAxis->tkData.lpSym->stData.stLongest;
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
                             aplRankCmp,
                            &aplNELM,
                             lpbFract,
                             lpaplLastAxis,
                             lpaplNELMAxis,
                             lphGlbAxis,
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
//  GradeUp
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
//  TestDupAxis
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
//  StorageType
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
            if (lptkFunc->tkData.tkChar EQ UCS2_EQUAL
             || lptkFunc->tkData.tkChar EQ UCS2_NOTEQUAL)
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
            if (lptkFunc->tkData.tkChar EQ UCS2_EQUAL
             || lptkFunc->tkData.tkChar EQ UCS2_NOTEQUAL)
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
//  FirstValue
//
//  Return the first value from a token as either
//    both an integer and a float, or as a character,
//    or as an LPSYMENTRY/HGLOBAL.  The token may be
//    an empty nested array.
//***************************************************************************

void FirstValue
    (LPTOKEN      lpToken,      // The token
     LPAPLINT     lpaplInteger, // Return the integer (or Boolean) (may be NULL)
     LPAPLFLOAT   lpaplFloat,   // ...        float (may be NULL)
     LPAPLCHAR    lpaplChar,    // ...        char (may be NULL)
     LPAPLLONGEST lpaplLongest, // ...        longest (may be NULL)
     LPVOID      *lpSymGlb,     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
     LPUCHAR      lpImmType,    // ...        immediate type IMM_TYPES (may be NULL)
     LPAPLSTYPE   lpArrType)    // ...        array type -- ARRAY_TYPES (may be NULL)

{
    HGLOBAL hGlbData;

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lpToken->tkData.lpSym->stFlags.Imm)
            {
                hGlbData = lpToken->tkData.lpSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbData));

                break;      // Continue below with global case
            } // End IF

            // Handle the immediate case

            if (lpImmType)
                *lpImmType = lpToken->tkData.lpSym->stFlags.ImmType;

            if (lpArrType)
                *lpArrType = TranslateImmTypeToArrayType (lpToken->tkData.lpSym->stFlags.ImmType);

            if (lpSymGlb)
                *lpSymGlb = NULL;

            if (lpaplLongest)
                *lpaplLongest = lpToken->tkData.lpSym->stData.stInteger;

            // Split cases based upon the immediate type
            switch (lpToken->tkData.lpSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    if (lpaplInteger)
                        *lpaplInteger = lpToken->tkData.lpSym->stData.stBoolean;
                    if (lpaplFloat)
                        // ***FIXME*** -- Possible loss of precision
                        *lpaplFloat   = (APLFLOAT) (lpToken->tkData.lpSym->stData.stBoolean);
                    if (lpaplChar)
                        *lpaplChar    = L'\0';
                    break;

                case IMMTYPE_INT:
                    if (lpaplInteger)
                        *lpaplInteger = lpToken->tkData.lpSym->stData.stInteger;
                    if (lpaplFloat)
                        // ***FIXME*** -- Possible loss of precision
                        *lpaplFloat   = (APLFLOAT) (lpToken->tkData.lpSym->stData.stInteger);
                    if (lpaplChar)
                        *lpaplChar    = L'\0';
                    break;

                case IMMTYPE_FLOAT:
                    if (lpaplFloat)
                        *lpaplFloat   = lpToken->tkData.lpSym->stData.stFloat;
                    if (lpaplInteger)
                        *lpaplInteger = (APLINT) (lpToken->tkData.lpSym->stData.stFloat);
                    if (lpaplChar)
                        *lpaplChar    = L'\0';

                    break;

                case IMMTYPE_CHAR:
                    if (lpaplInteger)
                        *lpaplInteger = 0;
                    if (lpaplFloat)
                        *lpaplFloat   = 0;
                    if (lpaplChar)
                        *lpaplChar    = lpToken->tkData.lpSym->stData.stChar;

                    break;

                defstop
                    break;
            } // End SWITCH

            return;

        case TKT_VARIMMED:
            if (lpImmType)
                *lpImmType = lpToken->tkFlags.ImmType;

            if (lpArrType)
                *lpArrType = TranslateImmTypeToArrayType (lpToken->tkFlags.ImmType);

            if (lpSymGlb)
                *lpSymGlb = NULL;

            if (lpaplLongest)
                *lpaplLongest = lpToken->tkData.tkInteger;

            // Split cases based upon the immediate type
            switch (lpToken->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    if (lpaplInteger)
                        *lpaplInteger = lpToken->tkData.tkBoolean;
                    if (lpaplFloat)
                        // ***FIXME*** -- Possible loss of precision
                        *lpaplFloat   = (APLFLOAT) (lpToken->tkData.tkBoolean);
                    if (lpaplChar)
                        *lpaplChar    = L'\0';

                    break;

                case IMMTYPE_INT:
                    if (lpaplInteger)
                        *lpaplInteger = lpToken->tkData.tkInteger;
                    if (lpaplFloat)
                        // ***FIXME*** -- Possible loss of precision
                        *lpaplFloat   = (APLFLOAT) (lpToken->tkData.tkInteger);
                    if (lpaplChar)
                        *lpaplChar    = L'\0';

                    break;

                case IMMTYPE_FLOAT:
                    if (lpaplFloat)
                        *lpaplFloat   = lpToken->tkData.tkFloat;
                    if (lpaplInteger)
                        *lpaplInteger = (APLINT) (lpToken->tkData.tkFloat);
                    if (lpaplChar)
                        *lpaplChar    = L'\0';

                    break;

                case IMMTYPE_CHAR:
                    if (lpaplInteger)
                        *lpaplInteger = 0;
                    if (lpaplFloat)
                        *lpaplFloat   = 0;
                    if (lpaplChar)
                        *lpaplChar    = lpToken->tkData.tkChar;

                    break;

                defstop
                    break;
            } // End SWITCH

            return;

        case TKT_VARARRAY:
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbData));

            break;      // Continue below with global case

        defstop
            return;
    } // End SWITCH

    // Handle the global case
    FirstValueGlb (ClrPtrTypeDirGlb (hGlbData),
                   lpaplInteger,
                   lpaplFloat,
                   lpaplChar,
                   lpaplLongest,
                   lpSymGlb,
                   lpImmType,
                   lpArrType);
} // End FirstValue


//***************************************************************************
//  FirstValueGlb
//
//  Return the first value from an HGLOBAL as either
//    both an integer and a float, or as a character,
//    or as an LPSYMENTRY/HGLOBAL.  The HGLOBAL may be
//    an empty nested array.
//***************************************************************************

void FirstValueGlb
    (HGLOBAL      hGlbData,     // The global memory handle
     LPAPLINT     lpaplInteger, // Return the integer (or Boolean) (may be NULL)
     LPAPLFLOAT   lpaplFloat,   // ...        float (may be NULL)
     LPAPLCHAR    lpaplChar,    // ...        char (may be NULL)
     LPAPLLONGEST lpaplLongest, // ...        longest (may be NULL)
     LPVOID      *lpSymGlb,     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
     LPUCHAR      lpImmType,    // ...        immediate type IMM_TYPES (may be NULL)
     LPAPLSTYPE   lpArrType)    // ...        array type -- ARRAY_TYPES (may be NULL)

{
    LPVOID   lpMem,
             lpData;
    APLINT   aplInteger;
    APLFLOAT aplFloat;
    APLCHAR  aplChar;

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

    Assert (lpHeader->ArrType EQ ARRAY_NESTED || lpHeader->NELM > 0);

    lpData = VarArrayBaseToData (lpMem, lpHeader->Rank);

    if (lpImmType)
        *lpImmType = TranslateArrayTypeToImmType (lpHeader->ArrType);

    if (lpArrType)
        *lpArrType = lpHeader->ArrType;

    // Split cases based upon the storage type
    switch (lpHeader->ArrType)
    {
        case ARRAY_BOOL:
            aplInteger = BIT0 & *(LPAPLBOOL) lpData;

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

        case ARRAY_INT:
            aplInteger = *(LPAPLINT) lpData;

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
            aplInteger = ((LPAPLAPA) lpData)->Off;

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
            aplFloat = *(LPAPLFLOAT) lpData;

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
            aplChar = *(LPAPLCHAR) lpData;

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

        case ARRAY_HETERO:
            DbgBrk ();
            DbgBrk ();



            break;

        case ARRAY_NESTED:
            if (lpaplInteger)
                *lpaplInteger = 0;
            if (lpaplFloat)
                *lpaplFloat   = 0;
            if (lpaplChar)
                *lpaplChar    = L'\0';
            if (lpaplLongest)
                *lpaplLongest = 0;
            if (lpSymGlb)
                *lpSymGlb = *(LPAPLNESTED) lpData;
            break;

        defstop
            break;
    } // End SWITCH

#undef  lpHeader

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;
} // End FirstValueGlb


//***************************************************************************
//  GetGlbPtrs_LOCK
//
//  Return the HGLOBAL and LPVOID from a token if it's an HGLOBAL
//    NULL otherwise.
//  The "_LOCK" suffix is a reminder that this function locks
//    a global memory handle and the caller must unlock it.
//***************************************************************************

void GetGlbPtrs_LOCK
    (LPTOKEN  lpToken,
     HGLOBAL *lphGlb,
     LPVOID  *lplpMem)

{
    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lpToken->tkData.lpSym->stFlags.Imm)
            {
                *lphGlb = lpToken->tkData.lpSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (*lphGlb));

                break;      // Continue with common HGLOBAL code
            } // End IF

            // Handle the immediate case

            // Fall through to handle immediate case

        case TKT_VARIMMED:
            *lphGlb  = NULL;
            *lplpMem = NULL;

            return;

        case TKT_VARARRAY:
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
        *lplpMem = MyGlobalLock (*lphGlb);
    } // End IF
} // End GetGlbPtrs_LOCK


//***************************************************************************
//  GetNextInteger
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
//  GetNextFloat
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
//  GetNextHetero
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
//  IncrOdometer
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
//  PrimScalarFnDydAllocate_EM
//
//  Allocate space for result of a primitive scalar dyadic function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimScalarFnDydAllocate_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimScalarFnDydAllocate_EM
    (LPTOKEN    lptkFunc,

     HGLOBAL   *lphGlbRes,      // Return HGLOBAL of the result

     LPVOID     lpMemLft,       // Ptr to left arg memory (may be NULL)
     LPVOID     lpMemRht,
     LPVOID    *lplpMemRes,     // Return ptr to locked memory in result

     APLRANK    aplRankLft,
     APLRANK    aplRankRht,
     LPAPLRANK  lpaplRankRes,

     APLSTYPE   aplTypeRes,

     APLNELM    aplNELMLft,
     APLNELM    aplNELMRht,
     APLNELM    aplNELMRes)

{
    APLUINT  ByteRes;
    APLNELM  aplNELMNested;
    LPAPLDIM lpMemDimArg;
    APLINT   uRes;
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

    // Calculate space for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, *lpaplRankRes);

    // Take into account space for a prototype
    aplNELMNested = max (aplNELMRes, 1);

////ByteRes = sizeof (VARARRAY_HEADER)
////        + sizeof (APLDIM) * *lpaplRankRes;
////
////// Split cases based upon the result storage type
////switch (aplTypeRes)
////{
////    case ARRAY_BOOL:        // One value per bit, rounded up to the byte
////        ByteRes += sizeof (APLBOOL)   * RoundUpBits8 (aplNELMRes);
////
////        break;
////
////    case ARRAY_INT:
////        ByteRes += sizeof (APLINT)    * aplNELMRes;
////
////        break;
////
////    case ARRAY_FLOAT:
////        ByteRes += sizeof (APLFLOAT)  * aplNELMRes;
////
////        break;
////
////    case ARRAY_NESTED:
////        // Take into account space for a prototype
////        aplNELMNested = max (aplNELMRes, 1);
////
////        ByteRes += sizeof (APLNESTED) * aplNELMNested;
////
////        break;
////
////    defstop
////        break;
////} // End IF/ELSE

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
    *lplpMemRes = MyGlobalLock (*lphGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) *lplpMemRes)

    // Fill in the header
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
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
        for (uRes = 0; uRes < (APLRANKSIGN) *lpaplRankRes; uRes++)
            (VarArrayBaseToDim (*lplpMemRes))[uRes] = 1;
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
        for (uRes = 0; uRes < (APLRANKSIGN) *lpaplRankRes; uRes++)
            (VarArrayBaseToDim (*lplpMemRes))[uRes] = (VarArrayBaseToDim (lpMemDimArg))[uRes];
    } else
    {
        // Get the higher rank's pointer
        //   to copy its dimensions to the result
        if (aplRankLft > aplRankRht)
            lpMemDimArg = lpMemLft;
        else
            lpMemDimArg = lpMemRht;

        // Copy the dimensions to the result
        for (uRes = 0; uRes < (APLRANKSIGN) *lpaplRankRes; uRes++)
            (VarArrayBaseToDim (*lplpMemRes))[uRes] = (VarArrayBaseToDim (lpMemDimArg))[uRes];
    } // End IF/ELSE/...

    // If the result is nested, fill in the
    //   elements with PTR_REUSED in case of failure
    //   later so as not to fail in <FreeResultGlobal>.
    if (aplTypeRes EQ ARRAY_NESTED)
    {
        // Skip over the header and dimensions to the data
        lpMem = VarArrayBaseToData (*lplpMemRes, *lpaplRankRes);

        // Mark the values as reused in case we fail later on
        for (uRes = 0; uRes < (APLNELMSIGN) aplNELMNested; uRes++)
            *((LPAPLNESTED) lpMem)++ = PTR_REUSED;
    } // End IF

    return TRUE;
} // End PrimScalarFnDydAllocate_EM
#undef  APPEND_NAME


//***************************************************************************
//  MakePrototype_EM
//
//  Make a prototype copy of a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakePrototype_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL MakePrototype_EM
    (HGLOBAL hGlbArr,
     LPTOKEN lptkFunc,
     BOOL    bNumericOnly)

{
    LPVOID      lpMemArr;
    LPVARARRAY_HEADER lpHeader;
    APLSTYPE    aplType;
    APLNELM     aplNELM;
    APLRANK     aplRank;
    UINT        u;
    APLNELM     uLen;
    HGLOBAL     hGlbTmp;
    BOOL        bRet = TRUE;
    LPSYMENTRY  lpSymArr, lpSymRes;

    DBGENTER;

    // Make a copy of the array as we're changing it
    hGlbArr = CopyArray_EM (hGlbArr, TRUE, lptkFunc);

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
            if (bNumericOnly)
            {
                if (aplNELM NE 0)
                    goto DOMAIN_ERROR_EXIT;

                // Change the storage type to Boolean
                lpHeader->ArrType = ARRAY_BOOL;
            } // End IF

            for (u = 0; u < aplNELM; u++)
                *((LPAPLCHAR)  lpMemArr)++ = L' ';
            break;

        case ARRAY_APA:
            // To convert an APA to a prototype, set its
            //   offset and multiplier to zero
            ((LPAPLAPA) lpMemArr)->Off = 0;
            ((LPAPLAPA) lpMemArr)->Mul = 0;

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Handle prototype case
            aplNELM = max (aplNELM, 1);

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
                            lpSymRes = SymTabAppendInteger_EM (0);

                            break;

                        case IMMTYPE_CHAR:
                            lpSymRes = SymTabAppendChar_EM (L' ');

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
                    MakePrototype_EM (ClrPtrTypeIndGlb (lpMemArr),
                                      lptkFunc,
                                      bNumericOnly);    // Pass flag through
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

    return hGlbArr;
} // End MakePrototype_EM
#undef  APPEND_NAME


//***************************************************************************
//  IsFirstSimpleGlb
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
//  CopySymGlbDir
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
//  CopySymGlbInd
//
//  Copy an indirect LPSYMENTRY or HGLOBAL incrementing the reference count
//***************************************************************************

HGLOBAL CopySymGlbInd
    (LPVOID lpSymGlb)

{
    return CopySymGlbDir (*(LPAPLNESTED) lpSymGlb);
} // End CopySymGlbInd


//***************************************************************************
//  CopySymGlbDirGlb
//
//  Copy a direct HGLOBAL incrementing the reference count
//***************************************************************************

HGLOBAL CopySymGlbDirGlb
    (HGLOBAL hGlb)

{
    return CopySymGlbDir (MakeGlbTypeGlb (hGlb));
} // End CopySymGlbDirGlb


//***************************************************************************
//  CopyArray_EM
//
//  Make a copy of a global memory ptr array
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopyArray_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL CopyArray_EM
    (HGLOBAL hGlbSrc,
     BOOL    bChanging,     // TRUE iff we're changing the copy
     LPTOKEN lpToken)

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

                // Loop through the destin array, setting all
                //   HGLOBAL ptrs to PTR_REUSED so we can recover
                //   if a CopyArray_EM fails
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
                        lpSymDst = CopyImmSymEntry_EM (lpSymSrc, -1, lpToken);
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
                        hGlbTmp = CopyArray_EM (ClrPtrTypeIndGlb (lpMemSrc), TRUE, lpToken);
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
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME, lpToken);
    return hGlbDst;
} // End CopyArray_EM
#undef  APPEND_NAME


#ifdef DEBUG
//***************************************************************************
//  IsGlobalTypeArray
//
//  Confirm that an HGLOBAL is a valid variable or function array
//***************************************************************************

BOOL IsGlobalTypeArray
    (HGLOBAL hGlb,
     UINT    Signature)

{
    LPVOID lpMem;
    BOOL   bRet = TRUE;

    // It's an HGLOBAL
    if (GetPtrTypeDir (hGlb) NE PTRTYPE_HGLOBAL)
        return FALSE;

    // Clear the PTRTYPE_*** flags
    hGlb = ClrPtrTypeDirGlb (hGlb);

    // It's a valid ptr
    bRet = bRet && IsGlbPtr (hGlb);

    if (bRet)
    {
        // Lock the memory to get a ptr to it
        lpMem = GlobalLock (hGlb); Assert (lpMem NE NULL);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

        // It's an array
        bRet = bRet && (lpHeader->Sign.ature EQ Signature);

#undef  lpHeader

        // We no longer need this ptr
        GlobalUnlock (hGlb); lpMem = NULL;
    } // End IF

    return bRet;
} // End IsGlobalTypeArray
#endif


//***************************************************************************
//  CheckRankLengthError_EM
//
//  Check a dyadic scalar function for RANK and LENGTH ERRORs
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckRankLengthError_EM"
#else
#define APPEND_NAME
#endif

BOOL CheckRankLengthError_EM
    (APLRANK  aplRankRes,
     APLRANK  aplRankLft,
     APLNELM  aplNELMLft,
     LPVOID   lpMemLft,
     APLRANK  aplRankRht,
     APLNELM  aplNELMRht,
     LPVOID   lpMemRht,
     APLNELM  aplNELMAxis,
     LPAPLINT lpMemAxisTail,
     LPTOKEN  lptkFunc)

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
//  RoundUpBits8
//
//  Round up bits to a byte (8 bits/byte)
//***************************************************************************

APLINT RoundUpBits8
    (APLNELM aplNELM)

{
    return ((aplNELM + (NBIB - 1)) >> LOG2NBIB);
} // End RoundUpBits8


//***************************************************************************
//  CheckException
//
//  Check on a structured exception
//***************************************************************************

UINT CheckException
    (LPEXCEPTION_POINTERS lpExcept,
     LPPRIMSPEC           lpPrimSpec)

{
    *lpPrimSpec->lpExecCode = lpExcept->ExceptionRecord->ExceptionCode;

    // Split cases based upon the exception code
    switch (*lpPrimSpec->lpExecCode)
    {
////////case EXEC_RESULT_BOOL:
////////case EXEC_RESULT_INT:
        case EXEC_RESULT_FLOAT:
        case EXEC_DOMAIN_ERROR:
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            return EXCEPTION_EXECUTE_HANDLER;

        default:
            return EXCEPTION_CONTINUE_SEARCH;
    } // End SWITCH
} // End CheckException


//***************************************************************************
//  imul64
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
            RaiseException (EXEC_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    return aplRes;
} // End imul64


//***************************************************************************
//  CalcArraySize
//
//  Calculate the size in bytes of an array
//***************************************************************************

APLUINT CalcArraySize
    (APLSTYPE aplType,
     APLNELM  aplNELM,
     APLRANK  aplRank)

{
    APLUINT ByteRes;

    Assert (0 <= aplType
         &&      aplType < ARRAY_LENGTH);

    switch (aplType)
    {
        case ARRAY_BOOL:
            ByteRes = sizeof (APLBOOL)   * RoundUpBits8 (aplNELM);

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
    ByteRes += sizeof (VARARRAY_HEADER)
             + sizeof (APLDIM) * aplRank;
    return ByteRes;
} // End CalcArraySize


//***************************************************************************
//  TypeDemote
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
//  indexing/squad
//  indexed assignment
//  pick
//  compression/replicate
//  unique
//  partition
//  first
//  enlist
//  without
//  each
//  etc.
//
//  The following functions have been changed to use TypeDemote:
//    enclose with axis
//    reshape
//    dyadic transpose
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

HGLOBAL TypeDemote
    (HGLOBAL hGlbRht)

{
    HGLOBAL           hGlbRes;
    LPVOID            lpMemRht = NULL,
                      lpMemRes;
    APLNELM           aplNELMRht;
    APLRANK           aplRankRht;
    APLINT            uRht,
                      aplInteger;
    APLFLOAT          aplFloat;
    APLUINT           ByteRes;
    APLSTYPE          aplTypeRes,
                      aplTypeRht,
                      aplTypeSub;
    LPSYMENTRY        lpSymEntry;
    LPVARARRAY_HEADER lpMemRhtHdr;
    DWORD             dwSize;
    UINT              uBitIndex;

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
            goto NORMAL_EXIT;

        case ARRAY_INT:         // Demote to Boolean
            // Start with lowest type
            aplTypeRes = ARRAY_BOOL;

            // Loop through the elements
            for (uRht = 0; uRht < (APLINT) aplNELMRht; uRht++)
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
            for (uRht = 0; uRht < (APLINT) aplNELMRht; uRht++)
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
            for (uRht = 0; uRht < (APLINT) aplNELMRht; uRht++, ((LPAPLHETERO) lpMemRht)++)
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
        // Calculate space needed for the new array
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
            lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = aplTypeRes;
        ////lpHeader->Perm       = 0;               // Already zero from GHND
        ////lpHeader->SysVar     = 0;               // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELMRht;
            lpHeader->Rank       = aplRankRht;

#undef  lpHeader

            // Skip over the header to the dimensions
            lpMemRht = VarArrayBaseToDim (lpMemRhtHdr);
            lpMemRes = VarArrayBaseToDim (lpMemRes);

            // Copy the dimensions to the result
            for (uRht = 0; uRht < (APLINT) aplRankRht; uRht++)
                *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;

            // Now, lpMemRes and lpMemRht both point to their
            //   respective data

            // Loop through the old values converting
            //   to the new storage type
            for (uRht = 0; uRht < (APLINT) aplNELMRht; uRht++)
            {
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
                                for (uRht = 0; uRht < (APLINT) aplNELMRht; uRht++)
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
                                for (uRht = 0; uRht < (APLINT) aplNELMRht; uRht++)
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
                                for (uRht = 0; uRht < (APLINT) aplNELMRht; uRht++)
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
                                for (uRht = 0; uRht < (APLINT) aplNELMRht; uRht++)
                                    *((LPAPLINT) lpMemRes)++ = (APLINT) *((LPAPLFLOAT) lpMemRht)++;
                                break;

                            case ARRAY_HETERO:  // Res = INT , Rht = HETERO
                            case ARRAY_NESTED:  // Res = INT , Rht = NESTED
                                // Loop through the elements
                                for (uRht = 0; uRht < (APLINT) aplNELMRht; uRht++)
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
                                for (uRht = 0; uRht < (APLINT) aplNELMRht; uRht++)
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
                                for (uRht = 0; uRht < (APLINT) aplNELMRht; uRht++)
                                    *((LPAPLCHAR) lpMemRes)++ = (*((LPSYMENTRY *) lpMemRht)++)->stData.stChar;
                                break;

                            defstop
                                break;
                        } // End SWITCH

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

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
    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return hGlbRes;
} // End TypeDemote
#undef  APPEND_NAME


//***************************************************************************
//  NewYYResIndex
//
//  Get new index into YYRes
//***************************************************************************

UINT NewYYResIndex
    (void)

{
    UINT YYLclIndex;

    // Increment and wrap the global index (YYResIndex)
    //   zero the memory, and return the current value.
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;
    ZeroMemory (&YYRes[YYLclIndex], sizeof (YYRes[0]));

    return YYLclIndex;
} // End NewYYResIndex


//***************************************************************************
//  End of File: primfns.c
//***************************************************************************
