//***************************************************************************
//  NARS2000 -- System Function -- Quad EM
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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
//  $SysFnEM_EM_YY
//
//  System function:  []EM -- Event Message
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnEM_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnEM_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token  (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    LPSIS_HEADER lpSISCur;          // Ptr to current SIS header
    HGLOBAL      hGlbRes = NULL;    // Result ...
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    // Niladic functions cannot have axis operator
    Assert (lptkAxis EQ NULL);

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to current SIS header
    lpSISCur = lpMemPTD->lpSISCur;
        while (lpSISCur
        && lpSISCur->DfnType NE DFNTYPE_FCN
        && lpSISCur->DfnType NE DFNTYPE_OP1
        && lpSISCur->DfnType NE DFNTYPE_OP2)
        lpSISCur = lpSISCur->lpSISPrv;

    if (lpSISCur)
        hGlbRes = CopySymGlbDirAsGlb (MakePtrTypeGlb (lpSISCur->hGlbQuadEM));
    else
        hGlbRes = hGlbQuadEM;

    // Allocate a YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnEM_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_em.c
//***************************************************************************
