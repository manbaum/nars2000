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

LPPL_YYSTYPE PrimOpDieresisTilde_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_DIERESISTILDE);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (&lpYYFcnStrOpr->tkToken);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonDieresisTilde_EM_YY (lpYYFcnStrOpr, // Ptr to operator function strand
                                             lptkRhtArg);   // Ptr to right arg
    else
        return PrimOpDydDieresisTilde_EM_YY (lptkLftArg,    // Ptr to left arg token
                                             lpYYFcnStrOpr, // Ptr to operator function strand
                                             lptkRhtArg);   // Ptr to right arg token
} // End PrimOpDieresisTilde_EM_YY


//***************************************************************************
//  $PrimProtoOpDieresisTilde_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator DieresisTilde ("duplicate" and "commute")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpDieresisTilde_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis)             // Ptr to axis token always NULL)

{
    Assert (lptkAxis EQ NULL);

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
    {
        //***************************************************************
        // Called monadically
        //***************************************************************

        return PrimOpMonDieresisTildeCommon_EM_YY (lpYYFcnStrOpr,   // Ptr to operator function strand
                                                   lptkRhtArg,      // Ptr to right arg token
                                                   TRUE);           // TRUE iff prototyping
    } else
    {
        //***************************************************************
        // Called dyadically
        //***************************************************************

        return PrimOpDydDieresisTildeCommon_EM_YY (lptkLftArg,      // Ptr to left arg token
                                                   lpYYFcnStrOpr,   // Ptr to operator function strand
                                                   lptkRhtArg,      // Ptr to right arg token
                                                   TRUE);           // TRUE iff prototyping
    } // End IF/ELSE
} // End PrimProtoOpDieresisTilde_EM_YY


//***************************************************************************
//  $PrimOpMonDieresisTilde_EM_YY
//
//  Primitive operator for monadic derived function from DieresisTilde ("duplicate")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonDieresisTilde_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpMonDieresisTildeCommon_EM_YY (lpYYFcnStrOpr,   // Ptr to operator function strand
                                               lptkRhtArg,      // Ptr to right arg token
                                               FALSE);          // TRUE iff prototyping
} // End PrimOpMonDieresisTilde_EM_YY


//***************************************************************************
//  $PrimOpMonDieresisTilde_EM_YY
//
//  Primitive operator for monadic derived function from DieresisTilde ("duplicate")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonDieresisTildeCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpMonDieresisTildeCommon_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     BOOL         bPrototyping)         // TRUE if prototyping

{
    LPPL_YYSTYPE lpYYFcnStrLft;         // Ptr to function strand
    LPPRIMFNS    lpPrimProtoLft;        // Ptr to left operand prototype function
    LPTOKEN      lptkAxis;              // Ptr to axis token (may be NULL)

    // Check for axis operator
    if (lpYYFcnStrOpr->FcnCount > 1
     && (lpYYFcnStrOpr[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
      || lpYYFcnStrOpr[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
        lptkAxis = &lpYYFcnStrOpr[1].tkToken;
     else
        lptkAxis = NULL;

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

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];

    // Split cases depending on whether or not we're prototyping
    if (bPrototyping)
    {
        // Get a ptr to the prototype function for the first symbol (a function or operator)
        lpPrimProtoLft = PrimProtoFnsTab[SymTrans (&lpYYFcnStrOpr->tkToken)];
        if (!lpPrimProtoLft)
        {
            ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                      &lpYYFcnStrOpr->tkToken);
            return NULL;
        } // End IF

        // Set ptr to left operand,
        //   skipping over the operator and axis token (if present)
        lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];

        // Execute the function between the right arg and itself
        // Note that we cast the function strand to LPTOKEN
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        return (*lpPrimProtoLft) (lptkRhtArg,       // Ptr to left arg token
                        (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand fnuction strand
                                  lptkRhtArg,       // Ptr to right arg token
                                  lptkAxis);        // Ptr to axis token
    } else
        // Execute the function between the right arg and itself
        return ExecFuncStr_EM_YY (lptkRhtArg,       // Ptr to left arg token
                                  lpYYFcnStrLft,    // Ptr to left operand function strand
                                  lptkRhtArg,       // Ptr to right arg token
                                  lptkAxis);        // Ptr to axis token (may be NULL)
} // End PrimOpMonDieresisTildeCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydDieresisTilde_EM_YY
//
//  Primitive operator for dyadic derived function from DieresisTilde ("commute")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydDieresisTilde_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpDydDieresisTildeCommon_EM_YY (lptkLftArg,      // Ptr to left arg token
                                               lpYYFcnStrOpr,   // Ptr to operator function strand
                                               lptkRhtArg,      // Ptr to right arg token
                                               FALSE);          // TRUE iff prototyping
} // End PrimOpDydDieresisTilde_EM_YY


//***************************************************************************
//  $PrimOpDydDieresisTildeCommon_EM_YY
//
//  Primitive operator for dyadic derived function from DieresisTilde ("commute")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydDieresisTildeCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDydDieresisTildeCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     BOOL         bPrototyping)         // TRUE iff prototyping

{
    LPPL_YYSTYPE lpYYFcnStrLft;         // Ptr to left operand function strand
    LPPRIMFNS    lpPrimProtoLft;        // Ptr to left operand prototype function
    LPTOKEN      lptkAxis;              // Ptr to axis token (may be NULL)

    // Check for axis operator
    if (lpYYFcnStrOpr->FcnCount > 1
     && (lpYYFcnStrOpr[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
      || lpYYFcnStrOpr[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
        lptkAxis = &lpYYFcnStrOpr[1].tkToken;
     else
        lptkAxis = NULL;

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

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];

    // Split cases depending on whether or not we're prototyping
    if (bPrototyping)
    {
        // Get a ptr to the prototype function for the first symbol (a function or operator)
        lpPrimProtoLft = PrimProtoFnsTab[SymTrans (&lpYYFcnStrOpr->tkToken)];
        if (!lpPrimProtoLft)
        {
            ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                      &lpYYFcnStrOpr->tkToken);
            return NULL;
        } // End IF

        // Execute the function between the two args switched
        // Note that we cast the function strand to LPTOKEN
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        return (*lpPrimProtoLft) (lptkRhtArg,       // Ptr to left arg token
                        (LPTOKEN) lpYYFcnStrOpr,    // Ptr to left operand function strand
                                  lptkLftArg,       // Ptr to right arg token
                                  lptkAxis);        // Ptr to axis token
    } else
        // Execute the function between the two args switched
        return ExecFuncStr_EM_YY (lptkRhtArg,   // Ptr to left arg token
                                  lpYYFcnStrOpr,// Ptr to left operand function strand
                                  lptkLftArg,   // Ptr to right arg token
                                  lptkAxis);    // Ptr to axis token (may be NULL)
} // End PrimOpDydDieresisTildeCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_ditilde.c
//***************************************************************************
