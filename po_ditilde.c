//***************************************************************************
//  NARS2000 -- Primitive Operator -- DieresisTilde
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2008 Sudley Place Software

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
     UBOOL        bPrototyping)         // TRUE if prototyping

{
    LPPL_YYSTYPE lpYYFcnStrLft;         // Ptr to function strand
    LPPRIMFNS    lpPrimProtoLft;        // Ptr to left operand prototype function
    LPTOKEN      lptkAxis;              // Ptr to axis token (may be NULL)

    // Check for axis operator
    lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive
    //   to the axis operator, so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken))
        goto LEFT_SYNTAX_EXIT;

    // Split cases depending on whether or not we're prototyping
    if (bPrototyping)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoLft = GetPrototypeFcnPtr (lpYYFcnStrLft);
        if (!lpPrimProtoLft)
            goto NONCE_EXIT;

        // Execute the function dyadically between the right arg and itself
        // Note that we cast the function strand to LPTOKEN
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        return (*lpPrimProtoLft) (lptkRhtArg,       // Ptr to left arg token
                        (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand fnuction strand
                                  lptkRhtArg,       // Ptr to right arg token
                                  lptkAxis);        // Ptr to axis token
    } else
        // Execute the function dyadically between the right arg and itself
        return ExecFuncStr_EM_YY (lptkRhtArg,       // Ptr to left arg token
                                  lpYYFcnStrLft,    // Ptr to left operand function strand
                                  lptkRhtArg,       // Ptr to right arg token
                                  lptkAxis);        // Ptr to axis token (may be NULL)
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;

LEFT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    return NULL;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    return NULL;
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
     UBOOL        bPrototyping)         // TRUE iff prototyping

{
    LPPL_YYSTYPE lpYYFcnStrLft;         // Ptr to left operand function strand
    LPPRIMFNS    lpPrimProtoLft;        // Ptr to left operand prototype function
    LPTOKEN      lptkAxis;              // Ptr to axis token (may be NULL)

    // Check for axis operator
    lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive
    //   to the axis operator, so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken))
        goto LEFT_SYNTAX_EXIT;

    // Split cases depending on whether or not we're prototyping
    if (bPrototyping)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoLft = GetPrototypeFcnPtr (lpYYFcnStrLft);
        if (!lpPrimProtoLft)
            goto NONCE_EXIT;

        // Execute the function dyadically between the two args switched
        // Note that we cast the function strand to LPTOKEN
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        return (*lpPrimProtoLft) (lptkRhtArg,       // Ptr to left arg token
                        (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand function strand
                                  lptkLftArg,       // Ptr to right arg token
                                  lptkAxis);        // Ptr to axis token
    } else
        // Execute the function dyadically between the two args switched
        return ExecFuncStr_EM_YY (lptkRhtArg,   // Ptr to left arg token
                                  lpYYFcnStrLft,// Ptr to left operand function strand
                                  lptkLftArg,   // Ptr to right arg token
                                  lptkAxis);    // Ptr to axis token (may be NULL)
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;

LEFT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    return NULL;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    return NULL;
} // End PrimOpDydDieresisTildeCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_ditilde.c
//***************************************************************************
