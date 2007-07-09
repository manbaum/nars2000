//***************************************************************************
//  NARS2000 -- Primitive Operator -- DieresisTilde
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
//  $PrimOpDieresisTilde_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    monadic operator DieresisTilde ("duplicate" and "commute")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDieresisTilde_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpDieresisTilde_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    Assert (lpYYFcnStr->tkToken.tkData.tkChar EQ UTF16_DIERESISTILDE);

    //***************************************************************
    // The derived functions from this operator are not sensitive
    //   to the axis operator, so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis);
        return NULL;
    } // End IF

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonDieresisTilde_EM_YY (lpYYFcnStr,    // Ptr to operator function strand
                                             lptkRhtArg,    // Ptr to right arg
                                             lptkAxis);     // Ptr to axis token (may be NULL)
    else
        return PrimOpDydDieresisTilde_EM_YY (lptkLftArg,    // Ptr to left arg token
                                             lpYYFcnStr,    // Ptr to operator function strand
                                             lptkRhtArg,    // Ptr to right arg token
                                             lptkAxis);     // Ptr to axis token (may be NULL)
} // End PrimOpDieresisTilde_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOpDieresisTilde_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator DieresisTilde ("duplicate" and "commute")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoOpDieresisTilde_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimProtoOpDieresisTilde_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    LPYYSTYPE lpYYRes = NULL,       // Ptr to the result
              lpYYFcnStrLft;        // Ptr to left operand function strand
    LPPRIMFNS lpPrimProtoLft;       // Ptr to left operand prototype function

    //***************************************************************
    // The derived functions from this operator are not sensitive
    //   to the axis operator, so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis);
        return NULL;
    } // End IF

    // Get a ptr to the prototype function for the first symbol (a function or operator)
    lpPrimProtoLft = PrimProtoFnsTab[SymTrans (&lpYYFcnStr->tkToken)];
    if (!lpPrimProtoLft)
    {
        ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                  &lpYYFcnStr->tkToken);
        return NULL;
    } // End IF

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStr[1 + (lptkAxis NE NULL)];

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
    {
        //***************************************************************
        // Called monadically
        //***************************************************************

        // Execute the function between the right arg and itself
        // Note that we cast the function strand to LPTOKEN
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        return (*lpPrimProtoLft) (lptkRhtArg,       // Ptr to left arg token
                        (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand fnuction strand
                                  lptkRhtArg,       // Ptr to right arg token
                                  NULL);            // Ptr to axis token
    } else
    {
        //***************************************************************
        // Called dyadically
        //***************************************************************

        // Execute the function between the two args switched
        // Note that we cast the function strand to LPTOKEN
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        return (*lpPrimProtoLft) (lptkRhtArg,       // Ptr to left arg token
                        (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand function strand
                                  lptkLftArg,       // Ptr to right arg token
                                  NULL);            // Ptr to axis token
    } // End IF/ELSE
} // End PrimProtoOpDieresisTilde_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpMonDieresisTilde_EM_YY
//
//  Primitive operator for monadic derived function from DieresisTilde ("duplicate")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonDieresisTilde_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpMonDieresisTilde_EM_YY
    (LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    LPYYSTYPE lpYYFcnStrLft;

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStr[1 + (lptkAxis NE NULL)];

    // Execute the function between the right arg and itself
    return ExecFuncStr_EM_YY (lptkRhtArg,       // Ptr to left arg token
                              lpYYFcnStrLft,    // Ptr to left operand function strand
                              lptkRhtArg);      // Ptr to right arg token
} // End PrimOpMonDieresisTilde_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydDieresisTilde_EM_YY
//
//  Primitive operator for dyadic derived function from DieresisTilde ("commute")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydDieresisTilde_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpDydDieresisTilde_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    LPYYSTYPE lpYYFcnStrLft;

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStr[1 + (lptkAxis NE NULL)];

    // Execute the function between the two args switched
    return ExecFuncStr_EM_YY (lptkRhtArg,   // Ptr to left arg token
                              lpYYFcnStr,   // Ptr to left operand function strand
                              lptkLftArg);  // Ptr to right arg token
} // End PrimOpDydDieresisTilde_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_ditilde.c
//***************************************************************************
