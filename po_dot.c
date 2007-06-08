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
//  $PrimOpDot_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    dyadic operator Dot ("ERROR" and "inner product")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpDot_EM_YY
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
        return PrimOpMonDot_EM_YY (lpYYFcnStr,  // Ptr to operator function strand
                                   lptkRhtArg,  // Ptr to right arg
                                   lptkAxis);   // Ptr to axis token (may be NULL)
    else
        return PrimOpDydDot_EM_YY (lptkLftArg,  // Ptr to left arg token
                                   lpYYFcnStr,  // Ptr to operator function strand
                                   lptkRhtArg,  // Ptr to right arg token
                                   lptkAxis);   // Ptr to axis token (may be NULL)
} // End PrimOpDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOpDot_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator Dot ("ERROR" and "inner product")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoOpDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimProtoOpDot_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    LPYYSTYPE lpYYFcnStrLft,        // Ptr to left operand function strand
              lpYYFcnStrRht;        // Ptr to right ...
    LPPRIMFNS lpPrimProtoLft = NULL,// Ptr to left operand prototype function
              lpPrimProtoRht = NULL;// Ptr to right ...

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStr[1 + (lptkAxis NE NULL)];
    lpYYFcnStrRht = &lpYYFcnStrLft[lpYYFcnStrLft->FcnCount];

    lpPrimProtoLft = PrimProtoFnsTab[SymTrans (&lpYYFcnStrLft->tkToken)];
    if (!lpPrimProtoLft)
    {
        ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                  &lpYYFcnStrLft->tkToken);
        return NULL;
    } // End IF

    lpPrimProtoRht = PrimProtoFnsTab[SymTrans (&lpYYFcnStrRht->tkToken)];
    if (!lpPrimProtoRht)
    {
        ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                  &lpYYFcnStrRht->tkToken);
        return NULL;
    } // End IF

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return PrimFnSyntaxError_EM (&lpYYFcnStr->tkToken);
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return PrimOpDydDotCommon_EM_YY (lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
                                         lpYYFcnStr,            // Ptr to operator function strand
                                         lptkRhtArg,            // Ptr to right arg token
                                         lptkAxis,              // Ptr to axis token (may be NULL)
                                         lpPrimProtoLft,        // Ptr to left operand prototype function
                                         lpPrimProtoRht);       // Ptr to right ...
} // End PrimProtoOpDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpMonDot_EM_YY
//
//  Primitive operator for monadic derived function from Dot ("ERROR")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonDot_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpMonDot_EM_YY
    (LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    return PrimFnSyntaxError_EM (&lpYYFcnStr->tkToken);
} // End PrimOpMonDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydDot_EM_YY
//
//  Primitive operator for dyadic derived function from Dot ("inner product")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpDydDot_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    return PrimOpDydDotCommon_EM_YY (lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
                                     lpYYFcnStr,            // Ptr to operator function strand
                                     lptkRhtArg,            // Ptr to right arg token
                                     lptkAxis,              // Ptr to axis token (may be NULL)
                                     NULL,                  // Ptr to left operand prototype function
                                     NULL);                 // Ptr to right ...
} // End PrimOpDydDot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydDotCommon_EM_YY
//
//  Primitive operator for dyadic derived function from Dot ("inner product")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydDot_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpDydDotCommon_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis,            // Ptr to axis token (may be NULL)
     LPPRIMFNS lpPrimProtoLft,      // Ptr to left operand prototype function
     LPPRIMFNS lpPrimProtoRht)      // Ptr to right ...

{
    LPYYSTYPE lpYYRes = NULL;       // Ptr to the result
    LPYYSTYPE lpYYFcnStrLft,        // Ptr to left operand function strand
              lpYYFcnStrRht;        // Ptr to right ...

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStr[1 + (lptkAxis NE NULL)];
    lpYYFcnStrRht = &lpYYFcnStrLft[lpYYFcnStrLft->FcnCount];

    DbgBrk ();      // ***FINISHME***












    return lpYYRes;
} // End PrimOpDydDotCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_dot.c
//***************************************************************************
