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
//  System functions:  []a, []A, []â, []Â
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
//  $SysFnâ_EM_YY
//
//  System function:  []â -- Alphabet Lowercase With Circumflex
//***************************************************************************

LPPL_YYSTYPE SysFnâ_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuadâ,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFnâ_EM_YY


//***************************************************************************
//  $SysFnÂ_EM_YY
//
//  System function:  []Â -- Alphabet Uppercase With Circumflex
//***************************************************************************

LPPL_YYSTYPE SysFnÂ_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuadÂ,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFnÂ_EM_YY


//***************************************************************************
//  $SysFnä_EM_YY
//
//  System function:  []ä -- Alphabet Lowercase With Dieresis
//***************************************************************************

LPPL_YYSTYPE SysFnä_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuadä,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFnä_EM_YY


//***************************************************************************
//  $SysFnÄ_EM_YY
//
//  System function:  []Ä -- Alphabet Uppercase With Dieresis
//***************************************************************************

LPPL_YYSTYPE SysFnÄ_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuadÄ,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFnÄ_EM_YY


//***************************************************************************
//  $SysFná_EM_YY
//
//  System function:  []á -- Alphabet Lowercase With Acute
//***************************************************************************

LPPL_YYSTYPE SysFná_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuadá,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFná_EM_YY


//***************************************************************************
//  $SysFnÁ_EM_YY
//
//  System function:  []Á -- Alphabet Uppercase With Acute
//***************************************************************************

LPPL_YYSTYPE SysFnÁ_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuadÁ,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFnÁ_EM_YY


//***************************************************************************
//  $SysFnà_EM_YY
//
//  System function:  []à -- Alphabet Lowercase With Grave
//***************************************************************************

LPPL_YYSTYPE SysFnà_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuadà,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFnà_EM_YY


//***************************************************************************
//  $SysFnÀ_EM_YY
//
//  System function:  []À -- Alphabet Uppercase With Grave
//***************************************************************************

LPPL_YYSTYPE SysFnÀ_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuadÀ,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFnÀ_EM_YY


//***************************************************************************
//  $SysFnã_EM_YY
//
//  System function:  []ã -- Alphabet Lowercase With Tilde
//***************************************************************************

LPPL_YYSTYPE SysFnã_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuadã,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFnã_EM_YY


//***************************************************************************
//  $SysFnÃ_EM_YY
//
//  System function:  []Ã -- Alphabet Uppercase With Tilde
//***************************************************************************

LPPL_YYSTYPE SysFnÃ_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Call common routine
    return
      SysFnACom_EM_YY (hGlbQuadÃ,   // Common handle for the appropriate alphabet
                       lptkLftArg,  // Ptr to left arg token (should be NULL)
                       lptkFunc,    // Ptr to function token
                       lptkRhtArg,  // Ptr to right arg token (should be NULL)
                       lptkAxis);   // Ptr to axis token (may be NULL)
} // End SysFnÃ_EM_YY


//***************************************************************************
//  End of File: qf_a.c
//***************************************************************************
