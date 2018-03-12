//***************************************************************************
//  NARS2000 -- System Function -- Quad A
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2018 Sudley Place Software

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
//  $SysFnACom_EM_YY
//
//  System functions:  []a, []A, []�, []�
//***************************************************************************

LPPL_YYSTYPE SysFnACom_EM_YY
    (HGLOBAL hGlbCom,               // Common handle for the appropriate alphabet
     LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbCom);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnACom_EM_YY


//***************************************************************************
//  $SysFnA_EM_YY
//
//  System function:  []a -- Alphabet Lowercase
//***************************************************************************

LPPL_YYSTYPE SysFna_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuada,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFna_EM_YY


//***************************************************************************
//  $SysFnA_EM_YY
//
//  System function:  []A -- Alphabet Uppercase
//***************************************************************************

LPPL_YYSTYPE SysFnA_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuadA,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFnA_EM_YY


//***************************************************************************
//  $SysFn�_EM_YY
//
//  System function:  []� -- Alphabet Lowercase With Circumflex
//***************************************************************************

LPPL_YYSTYPE SysFn�_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuad�,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFn�_EM_YY


//***************************************************************************
//  $SysFn�_EM_YY
//
//  System function:  []� -- Alphabet Uppercase With Circumflex
//***************************************************************************

LPPL_YYSTYPE SysFn�_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuad�,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFn�_EM_YY


//***************************************************************************
//  $SysFn�_EM_YY
//
//  System function:  []� -- Alphabet Lowercase With Dieresis
//***************************************************************************

LPPL_YYSTYPE SysFn�_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuad�,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFn�_EM_YY


//***************************************************************************
//  $SysFn�_EM_YY
//
//  System function:  []� -- Alphabet Uppercase With Dieresis
//***************************************************************************

LPPL_YYSTYPE SysFn�_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuad�,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFn�_EM_YY


//***************************************************************************
//  $SysFn�_EM_YY
//
//  System function:  []� -- Alphabet Lowercase With Acute
//***************************************************************************

LPPL_YYSTYPE SysFn�_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuad�,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFn�_EM_YY


//***************************************************************************
//  $SysFn�_EM_YY
//
//  System function:  []� -- Alphabet Uppercase With Acute
//***************************************************************************

LPPL_YYSTYPE SysFn�_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuad�,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFn�_EM_YY


//***************************************************************************
//  $SysFn�_EM_YY
//
//  System function:  []� -- Alphabet Lowercase With Grave
//***************************************************************************

LPPL_YYSTYPE SysFn�_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuad�,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFn�_EM_YY


//***************************************************************************
//  $SysFn�_EM_YY
//
//  System function:  []� -- Alphabet Uppercase With Grave
//***************************************************************************

LPPL_YYSTYPE SysFn�_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuad�,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFn�_EM_YY


//***************************************************************************
//  $SysFn�_EM_YY
//
//  System function:  []� -- Alphabet Lowercase With Tilde
//***************************************************************************

LPPL_YYSTYPE SysFn�_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuad�,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFn�_EM_YY


//***************************************************************************
//  $SysFn�_EM_YY
//
//  System function:  []� -- Alphabet Uppercase With Tilde
//***************************************************************************

LPPL_YYSTYPE SysFn�_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuad�,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFn�_EM_YY


//***************************************************************************
//  End of File: qf_a.c
//***************************************************************************
