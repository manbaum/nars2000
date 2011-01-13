//***************************************************************************
//  NARS2000 -- System Function -- Quad WA
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

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
//  $SysFnWA_EM_YY
//
//  System function:  []WA -- Workspace Available
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnWA_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnWA_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE   lpYYRes;         // Ptr to the result
    APLU3264       ByteTmpHi,       // # bytes to allocate, high value
                   ByteTmpMid,      // ...                  mid  ...
                   ByteTmpLo;       // ...                  low  ...
    HGLOBAL        hGlbTmp;         // Temporary global memory handle

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

#ifdef _WIN64
    // Start with 256GB-1
    ByteTmpHi  = 0x3FFFFFFFFF;

    // The amount of storage we don't care about
  #define FUDGE_FACTOR    (128*1024)

#elif defined (_WIN32)
    // Start with 4GB-1
    ByteTmpHi  = 0xFFFFFFFF;

    // The amount of storage we don't care about
  #define FUDGE_FACTOR    (16*1024)

#else
  #error Need code for this architecture.
#endif
    ByteTmpMid = ByteTmpHi / 2;
    ByteTmpLo  = 1;

    while (ByteTmpHi > (ByteTmpLo + FUDGE_FACTOR))
    {
        // Coalesce memory by allocating a large contiguous block
        // We use GlobalAlloc/Free instead of MyGlobalAlloc/Free to
        //   avoid debug messages upon failure.
        hGlbTmp = GlobalAlloc (GMEM_MOVEABLE, (APLU3264) ByteTmpMid);
        if (hGlbTmp)
        {
            // We no longer need this storage
            GlobalFree (hGlbTmp); hGlbTmp = NULL;

            // Save as low water mark
            ByteTmpLo = ByteTmpMid;

            // Split the difference between Hi and Mid
            ByteTmpMid = (APLU3264) ((APLUINT) ByteTmpHi + (APLUINT) ByteTmpMid) / 2;
        } else
        {
            // Save as high water mark
            ByteTmpHi = ByteTmpMid;

            // Split the difference between Lo and Mid
            ByteTmpMid = (APLU3264) ((APLUINT) ByteTmpLo + (APLUINT) ByteTmpMid) / 2;
        } // End IF/ELSE
    } // End WHILE

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkInteger  = ByteTmpLo;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnWA_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_wa.c
//***************************************************************************
