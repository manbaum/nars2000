//***************************************************************************
//  NARS2000 -- Primitive Operator -- Slash
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
//  PrimOpSlash_EM
//
//  Primitive operator for monadic and dyadic derived functions from Slash
//    ("reduction" and "N-wise reduction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpSlash_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpSlash_EM
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    Assert (lpYYFcnStr->tkToken.tkData.tkChar EQ INDEX_OPSLASH
         || lpYYFcnStr->tkToken.tkData.tkChar EQ INDEX_OPSLASHBAR);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonSlash_EM (lpYYFcnStr,   // Ptr to operator function strand
                                  lptkRhtArg,   // Ptr to right arg
                                  lptkAxis);    // Ptr to axis token (may be NULL)
    else
        return PrimOpDydSlash_EM (lptkLftArg,   // Ptr to left arg token
                                  lpYYFcnStr,   // Ptr to operator function strand
                                  lptkRhtArg,   // Ptr to right arg token
                                  lptkAxis);    // Ptr to axis token (may be NULL)
} // End PrimOpSlash_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimProtoOpSlash_EM
//
//  Generate a prototype for the derived functions from
//    monadic operator Slash ("reduction" and "N-wise reduction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoOpSlash_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimProtoOpSlash_EM
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
} // End PrimProtoOpSlash_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimOpMonSlash_EM
//
//  Primitive operator for monadic derived function from Slash ("reduction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonSlash_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpMonSlash_EM
    (LPYYSTYPE lpYYFcnStrOpr,       // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    APLSTYPE   aplTypeRht,
               aplTypeRes;
    APLNELM    aplNELMRht;
    APLRANK    aplRankRht;
    APLUINT    aplAxis;             // The (one and only) axis value
    HGLOBAL    hGlbRes = NULL;
////LPPRIMSPEC lpPrimSpec;
    LPYYSTYPE  lpYYRes = NULL;
    LPYYSTYPE  lpYYFcnStrLft;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Reduction allows a single integer axis value only
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
        // If Slash, use last axis
        if (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLASH)
            aplAxis = aplRankRht - 1;
        else
            // Otherwise, it's SlashBar on the first axis
            aplAxis = 0;
    } // End IF/ELSE

    DbgBrk ();          // ***FINISHME***

////// If the operand is a primitive scalar dyadic function,
//////   calculate the storage type of the result,
//////   otherwise, assume it's ARRAY_NESTED
////if (lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
//// && PF_DYDSCALAR & PrimFlags[(UCHAR) lpYYFcnStrLft->tkToken.tkData.tkChar])
////{
////    // Get the corresponding lpPrimSpec
////    lpPrimSpec = PrimSpecTab[SymTrans (&lpYYFcnStrLft->tkToken)];
////
////    // Calculate the storage type of the result
////    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (1,
////                                               &aplTypeRht,
////                                               &lpYYFcnStrLft->tkToken,
////                                                1,
////                                               &aplTypeRht);
////    if (aplTypeRes EQ ARRAY_ERROR)
////    {
////        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
////                                  &lpYYFcnStrOpr->tkToken);
////        goto ERROR_EXIT;
////    } // End IF
////} else
        aplTypeRes = ARRAY_NESTED;

    //









    // If this is the last coordinate, and
    //   this is a primitive function, and
    //   the right arg is Boolean, then
    //   check for the possibliity of doing a
    //   Fast Boolean Reduction
    if (aplAxis EQ aplRankRht - 1
     && lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
     && aplTypeRht EQ ARRAY_BOOL
     && PF_FASTBOOL & PrimFlags[(UCHAR) lpYYFcnStrLft->tkToken.tkData.tkChar])
    {
        DbgBrk ();










    } // End IF
















    goto NORMAL_EXIT;

//ERROR_EXIT:




NORMAL_EXIT:


    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (TypeDemote (hGlbRes));
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    return lpYYRes;
} // End PrimOpMonSlash_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimOpDydSlash_EM
//
//  Primitive operator for dyadic derived function from Slash ("N-wise reduction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydSlash_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpDydSlash_EM
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
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
        // If Slash, use last axis
        if (lpYYFcnStr->tkToken.tkData.tkChar EQ INDEX_OPSLASH)
            aplAxis = aplRankRht - 1;
        else
            // Otherwise, it's SlashBar on the first axis
            aplAxis = 0;
    } // End IF/ELSE

    DbgBrk ();      // ***FINISHME***












    return lpYYRes;
} // End PrimOpDydSlash_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_slash.c
//***************************************************************************
