//***************************************************************************
//  NARS2000 -- Primitive Operator -- Jot
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
//  $PrimOpJot_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    dyadic operator Jot ("composition" and "composition")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpJot_EM_YY
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

    return PrimOpJotCommon_EM_YY (lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
                                  lpYYFcnStr,           // Ptr to operator function strand
                                  lptkRhtArg,           // Ptr to right arg token
                                  lptkAxis,             // Ptr to axis token (may be NULL)
                                  FALSE,                // TRUE iff computing a prototype result
                                  NULL,                 // Ptr to left operand prototype function
                                  NULL);                // Ptr to right ...
} // End PrimOpJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpJotCommon_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    dyadic operator Jot ("composition" and "composition")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpJotCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpJotCommon_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis,            // Ptr to axis token (may be NULL)
     BOOL      bPrototype,          // TRUE iff computing a prototype result
     LPPRIMFNS lpPrimProtoLft,      // Ptr to left operand prototype function
     LPPRIMFNS lpPrimProtoRht)      // Ptr to right ...

{
    LPYYSTYPE lpYYFcnStrLft,
              lpYYFcnStrRht,
              lpYYRes,
              lpYYRes2;
    UINT      uLftArg,
              uRhtArg;

    Assert (lpYYFcnStr->tkToken.tkData.tkChar EQ UTF16_JOT);

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStr[1 + (lptkAxis NE NULL)];
    lpYYFcnStrRht = &lpYYFcnStrLft[lpYYFcnStrLft->FcnCount];

    // Handle both monadic & dyadic derived functions together

    // Determine if the laft & right arg tokens are functions/operators
    uLftArg = IsFcnOpr (&lpYYFcnStrLft->tkToken);
    uRhtArg = IsFcnOpr (&lpYYFcnStrRht->tkToken);

    // Split cases based upon the type (V or F) of
    //   the left and right operands
    switch (uLftArg * 2 + uRhtArg * 1)
    {
        case 1 * 2 + 1 * 1:     // F1 op2 F2 -> F1 F2 R or L F1 F2 R
            // Execute the right operand monadically
            //   on the right arg
            if (bPrototype)
                // Note that we cast the function strand to LPTOKEN
                //   to bridge the two types of calls -- one to a primitive
                //   function which takes a function token, and one to a
                //   primitive operator which takes a function strand
                lpYYRes2 = (*lpPrimProtoRht) (NULL,                     // Ptr to left arg token
                                    (LPTOKEN) lpYYFcnStrRht,            // Ptr to right operand function strand
                                              lptkRhtArg,               // Ptr to right arg token
                                              lptkAxis);                // Ptr to axis token
            else
                lpYYRes2 = ExecFuncStr_EM_YY (NULL,                     // Ptr to left arg token
                                              lpYYFcnStrRht,            // Ptr to right operand function strand
                                              lptkRhtArg);              // Ptr to right arg token
            if (lpYYRes2)
            {
                // Execute the left operand dyadically
                //   between the (optional) left arg and the
                //   above result from the right operand.
                if (bPrototype)
                    // Note that we cast the function strand to LPTOKEN
                    //   to bridge the two types of calls -- one to a primitive
                    //   function which takes a function token, and one to a
                    //   primitive operator which takes a function strand
                    lpYYRes = (*lpPrimProtoLft) (lptkLftArg,            // Ptr to left arg token
                                       (LPTOKEN) lpYYFcnStrLft,         // Ptr to left operand function strand
                                                &lpYYRes2->tkToken,     // Ptr to right arg token
                                                 lptkAxis);             // Ptr to axis token
                else
                    lpYYRes = ExecFuncStr_EM_YY (lptkLftArg,            // Ptr to left arg token
                                                 lpYYFcnStrLft,         // Ptr to left operand function strand
                                                &lpYYRes2->tkToken);    // Ptr to right arg token
                FreeResult (&lpYYRes2->tkToken); YYFree (lpYYRes2); lpYYRes2 = NULL;
            } else
                lpYYRes = NULL;

            break;

        case 0 * 2 + 1 * 1:     // V op2 F -> V F R
            // If there's a left arg, signal a SYNTAX ERROR
            if (lptkLftArg)
            {
                ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                          &lpYYFcnStrLft->tkToken);
                return NULL;
            } // End IF

            // Execute the right operand dyadically
            //   between the left operand and the right arg.
            if (bPrototype)
                // Note that we cast the function strand to LPTOKEN
                //   to bridge the two types of calls -- one to a primitive
                //   function which takes a function token, and one to a
                //   primitive operator which takes a function strand
                lpYYRes = (*lpPrimProtoRht) (&lpYYFcnStrLft->tkToken,   // Ptr to left arg token
                                    (LPTOKEN) lpYYFcnStrRht,            // Ptr to right operand function strand
                                              lptkRhtArg,               // Ptr to right arg token
                                              lptkAxis);                // Ptr to axis token
            else
                lpYYRes = ExecFuncStr_EM_YY (&lpYYFcnStrLft->tkToken,   // Ptr to left arg token
                                              lpYYFcnStrRht,            // Ptr to right operand function strand
                                              lptkRhtArg);              // Ptr to right arg token
            break;

        case 1 * 2 + 0 * 1:     // F op2 V
            // If there's a left arg, signal a SYNTAX ERROR
            if (lptkLftArg)
            {
                ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                          &lpYYFcnStrLft->tkToken);
                return NULL;
            } // End IF

            // Execute the left operand dyadically
            //   between the right arg and the right operand.
            if (bPrototype)
                // Note that we cast the function strand to LPTOKEN
                //   to bridge the two types of calls -- one to a primitive
                //   function which takes a function token, and one to a
                //   primitive operator which takes a function strand
                lpYYRes = (*lpPrimProtoLft) (lptkRhtArg,                // Ptr to left arg token
                                   (LPTOKEN) lpYYFcnStrLft,             // Ptr to left operand function strand
                                            &lpYYFcnStrRht->tkToken,    // Ptr to right arg token
                                             lptkAxis);                 // Ptr to axis token
            else
                lpYYRes = ExecFuncStr_EM_YY (lptkRhtArg,                // Ptr to left arg token
                                             lpYYFcnStrLft,             // Ptr to left operand function strand
                                            &lpYYFcnStrRht->tkToken);   // Ptr to right arg token
            break;

        case 0 * 2 + 0 * 1:     // V op2 V
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                      &lpYYFcnStrLft->tkToken);
            break;

        defstop
            break;
    } // End SWITCH

    return lpYYRes;
} // End PrimOpJotCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOpJot_EM_YY
//
//  Generate a prototype for the derived functions from Jot ("composition")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoOpJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimProtoOpJot_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    LPYYSTYPE lpYYFcnStrLft,        // Ptr to left operand function strand
              lpYYFcnStrRht;        // Ptr to right ...
    LPPRIMFNS lpPrimProtoLft = NULL,// Ptr to left operand prototype function
              lpPrimProtoRht = NULL;// Ptr to right ...

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

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStr[1 + (lptkAxis NE NULL)];
    lpYYFcnStrRht = &lpYYFcnStrLft[lpYYFcnStrLft->FcnCount];

    // Get a ptr to the prototype function for the left and right operands
    if (IsFcnOpr (&lpYYFcnStrLft->tkToken))
    {
        lpPrimProtoLft = PrimProtoFnsTab[SymTrans (&lpYYFcnStrLft->tkToken)];
        if (!lpPrimProtoLft)
        {
            ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                      &lpYYFcnStrLft->tkToken);
            return NULL;
        } // End IF
    } // End IF

    if (IsFcnOpr (&lpYYFcnStrRht->tkToken))
    {
        lpPrimProtoRht = PrimProtoFnsTab[SymTrans (&lpYYFcnStrRht->tkToken)];
        if (!lpPrimProtoRht)
        {
            ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                      &lpYYFcnStrRht->tkToken);
            return NULL;
        } // End IF
    } // End IF

    return PrimOpJotCommon_EM_YY (lptkLftArg,           // Ptr to left arg token
                                  lpYYFcnStr,           // Ptr to operator function strand
                                  lptkRhtArg,           // Ptr to right arg token
                                  lptkAxis,             // Ptr to axis token (may be NULL)
                                  TRUE,                 // TRUE iff computing a prototype result
                                  lpPrimProtoLft,       // Ptr to left operand prototype function
                                  lpPrimProtoRht);      // Ptr to right ...
} // End PrimProtoOpJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_jot.c
//***************************************************************************
