//***************************************************************************
//  NARS2000 -- System Function -- Quad DM
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2009 Sudley Place Software

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
//  $SysFnDM_EM_YY
//
//  System function:  []DM -- Diagnostic Message
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDM_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDM_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token  (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result
    HGLOBAL      hGlbRes;       // Result global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get the global memory handle for quad-DM
    hGlbRes = CopySymGlbDirAsGlb (lpMemPTD->hGlbQuadDM);

    // Allocate a YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = hGlbRes;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnDM_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_dm.c
//***************************************************************************
