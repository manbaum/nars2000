//***************************************************************************
//  NARS2000 -- Primitive Operator -- Dot
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  PrimOpDot_EM
//
//  Primitive operator for monadic and dyadic derived functions from Dot ("ERROR" and "inner product")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDot_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpDot_EM
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // The derived functions from this operator are not sensitive to
    //   the axis operator, so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis);
        return NULL;
    } // End IF

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonDot_EM (lpYYFcnStr,     // Ptr to operator function strand
                                lptkRhtArg,     // Ptr to right arg
                                lptkAxis);      // Ptr to axis token (may be NULL)
    else
        return PrimOpDydDot_EM (lptkLftArg,     // Ptr to left arg token
                                lpYYFcnStr,     // Ptr to operator function strand
                                lptkRhtArg,     // Ptr to right arg token
                                lptkAxis);      // Ptr to axis token (may be NULL)
} // End PrimOpDot_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimProtoOpDot_EM
//
//  Generate a prototype for the derived functions from
//    monadic operator Dot ("ERROR" and "inner product")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoOpDot_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimProtoOpDot_EM
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    LPYYSTYPE lpYYRes = NULL;

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
    {
        //***************************************************************
        // Called monadically
        //***************************************************************

        DbgBrk ();          // ***FINISHME***










    } else
    {
        //***************************************************************
        // Called dyadically
        //***************************************************************

        DbgBrk ();          // ***FINISHME***










    } // End IF/ELSE

    return lpYYRes;
} // End PrimProtoOpDot_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimOpMonDot_EM
//
//  Primitive operator for monadic derived function from Dot ("ERROR")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonDot_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpMonDot_EM
    (LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    return PrimFnSyntaxError_EM (&lpYYFcnStr->tkToken);
} // End PrimOpMonDot_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimOpDydDot_EM
//
//  Primitive operator for dyadic derived function from Dot ("inner product")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydDot_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpDydDot_EM
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    LPYYSTYPE lpYYRes = NULL;
    LPYYSTYPE lpYYFcnStrLft,
              lpYYFcnStrRht;

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStr[1 + (lptkAxis NE NULL)];
    lpYYFcnStrRht = &lpYYFcnStrLft[lpYYFcnStrLft->FcnCount];

    DbgBrk ();      // ***FINISHME***












    return lpYYRes;
} // End PrimOpDydDot_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_dot.c
//***************************************************************************
