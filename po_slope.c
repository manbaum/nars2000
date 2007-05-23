//***************************************************************************
//  NARS2000 -- Primitive Operator -- Slope
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
//  PrimOpSlope_EM
//
//  Primitive operator for monadic and dyadic derived functions from Slope
//    ("scan" and "ERROR")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpSlope_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpSlope_EM
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    Assert (lpYYFcnStr->tkToken.tkData.tkChar EQ INDEX_OPSLOPE
         || lpYYFcnStr->tkToken.tkData.tkChar EQ INDEX_OPSLOPEBAR);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonSlope_EM (lpYYFcnStr,   // Ptr to operator function strand
                                  lptkRhtArg,   // Ptr to right arg
                                  lptkAxis);    // Ptr to axis token (may be NULL)
    else
        return PrimOpDydSlope_EM (lptkLftArg,   // Ptr to left arg token
                                  lpYYFcnStr,   // Ptr to operator function strand
                                  lptkRhtArg,   // Ptr to right arg token
                                  lptkAxis);    // Ptr to axis token (may be NULL)
} // End PrimOpSlope_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimProtoOpSlope_EM
//
//  Generate a prototype for the derived functions from
//    monadic operator Slope ("scan" and "ERROR")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoOpSlope_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimProtoOpSlope_EM
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










        return lpYYRes;
    } else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return PrimFnSyntaxError_EM (&lpYYFcnStr->tkToken);
} // End PrimProtoOpSlope_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimOpMonSlope_EM
//
//  Primitive operator for monadic derived function from Slope ("scan")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonSlope_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpMonSlope_EM
    (LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    APLSTYPE  aplTypeRht;
    APLNELM   aplNELMRht;
    APLRANK   aplRankRht;
    APLUINT   aplAxis;          // The (one and only) axis value
    LPYYSTYPE lpYYRes = NULL;
    LPYYSTYPE lpYYFcnStrLft;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStr[1 + (lptkAxis NE NULL)];

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Reverse allows a single integer axis value only
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRht,      // All values less than this
                           TRUE,            // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // Return TRUE iff fractional values present
                          &aplAxis,         // Return last axis value
                           NULL,            // Return # elements in axis vector
                           NULL))           // Return HGLOBAL with APLINT axis values
            return NULL;
    } else
    {
        // If Slope, use last axis
        if (lpYYFcnStr->tkToken.tkData.tkChar EQ INDEX_OPSLOPE)
            aplAxis = aplRankRht - 1;
        else
            // Otherwise, it's SlopeBar on the first axis
            aplAxis = 0;
    } // End IF/ELSE

    DbgBrk ();          // ***FINISHME***













    return lpYYRes;
} // End PrimOpMonSlope_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimOpDydSlope_EM
//
//  Primitive operator for dyadic derived function from Slope ("ERROR")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydSlope_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpDydSlope_EM
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    return PrimFnSyntaxError_EM (&lpYYFcnStr->tkToken);
} // End PrimOpDydSlope_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_slope.c
//***************************************************************************
