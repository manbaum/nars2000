//***************************************************************************
//  NARS2000 -- System Function -- Quad NL
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $SysFnNL_EM
//
//  System function:  []NL -- Name List
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnNL_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnNL_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis);
        return NULL;
    } // End IF

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonNL_EM (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydNL_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnNL_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonNL_EM
//
//  Monadic []NL -- Name List (by Class)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonNL_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonNL_EM
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return SysFnDydNL_EM (NULL,         // Ptr to left arg token
                          lptkFunc,     // Ptr to function token
                          lptkRhtArg,   // Ptr to right arg token
                          lptkAxis);    // Ptr to axis token (may be NULL)
} // End SysFnMonNL_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydNL_EM
//
//  Dyadic []NL -- Name List (by Alphabet and Class)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydNC_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydNL_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes = NULL;




    return lpYYRes;
} // End SysFnDydNL_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_nl.c
//***************************************************************************
