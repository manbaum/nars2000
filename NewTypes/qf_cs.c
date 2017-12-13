//***************************************************************************
//  NARS2000 -- System Function -- Quad CS
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2017 Sudley Place Software

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
//  $SysFnCS_EM_YY
//
//  System function:  []CS -- Collating Sequence
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnCS_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnCS_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
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
    lpYYRes->tkToken.tkData.tkGlbData  = hGlbQuadCS;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnCS_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $MakeQuadCS
//
//  Create the global memory value for []CS
//***************************************************************************

#define D1      10      // Length of dimension #1
#define D2       6      // ...                  2
#define D3       2      // ...                  3
#define D4      27      // ...                  4

void MakeQuadCS
    (void)

{
    LPVARARRAY_HEADER lpHeader;                     // Ptr to array header
    APLCHAR           (*lpMemRes)[D1][D2][D3][D4];  // Ptr to result global memory
    APLUINT           ByteRes;                      // # bytes in the result
    UINT              uRes;                         // Loop counter

#define QUADCS_LEN      D1 * D2 * D3 * D4

    // Calculate # bytes in the result
    ByteRes = CalcArraySize (ARRAY_CHAR, QUADCS_LEN, 4);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
    {
        DbgStop ();         // We should never get here
    } // End IF

    // Create []CS
    hGlbQuadCS = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbQuadCS EQ NULL)
    {
        DbgStop ();         // We should never get here
    } // End IF

    // Lock the memory to get a ptr to it
    lpHeader = MyGlobalLock000 (hGlbQuadCS);

    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
    lpHeader->PermNdx    = PERMNDX_QUADCS;  // So we don't free it
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
////lpHeader->RefCnt     = 0;               // Ignore as this is perm
    lpHeader->NELM       = QUADCS_LEN;
    lpHeader->Rank       = 4;

    // Fill in the dimensions
    (VarArrayBaseToDim (lpHeader))[0] = D1;
    (VarArrayBaseToDim (lpHeader))[1] = D2;
    (VarArrayBaseToDim (lpHeader))[2] = D3;
    (VarArrayBaseToDim (lpHeader))[3] = D4;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpHeader);

    // Fill in the background
    FillMemoryW (&(*lpMemRes)[0][0][0][0], QUADCS_LEN, L' ');

    // Fill in the alphabets in proper order
    MakeQuadCSRow (&(*lpMemRes)[0][0][0][1], hGlbQuada);
    MakeQuadCSRow (&(*lpMemRes)[0][0][1][1], hGlbQuadA);
    MakeQuadCSRow (&(*lpMemRes)[0][1][0][1], hGlbQuad·);
    MakeQuadCSRow (&(*lpMemRes)[0][1][1][1], hGlbQuad¡);
    MakeQuadCSRow (&(*lpMemRes)[0][2][0][1], hGlbQuad‡);
    MakeQuadCSRow (&(*lpMemRes)[0][2][1][1], hGlbQuad¿);
    MakeQuadCSRow (&(*lpMemRes)[0][3][0][1], hGlbQuad‚);
    MakeQuadCSRow (&(*lpMemRes)[0][3][1][1], hGlbQuad¬);
    MakeQuadCSRow (&(*lpMemRes)[0][4][0][1], hGlbQuad‰);
    MakeQuadCSRow (&(*lpMemRes)[0][4][1][1], hGlbQuadƒ);
    MakeQuadCSRow (&(*lpMemRes)[0][5][0][1], hGlbQuad„);
    MakeQuadCSRow (&(*lpMemRes)[0][5][1][1], hGlbQuad√);

    // Fill in the digits
    for (uRes = 0; uRes < D1; uRes++)
      (*lpMemRes)[uRes][0][1][0] = L"0123456789"[uRes];

    // We no longer need this ptr
    MyGlobalUnlock (hGlbQuadCS); lpMemRes = NULL;

    // Set the PTB
    hGlbQuadCS = MakePtrTypeGlb (hGlbQuadCS);
} // End MakeQuadCS


//***************************************************************************
// $MakeQuadCSRow
//
//  Fill in a row of []CS
//***************************************************************************

void MakeQuadCSRow
    (LPAPLCHAR lpMemRes,
     HGLOBAL   hGlbArg)

{
    LPVARARRAY_HEADER lpMemHdrArg;      // Ptr to arg header
    LPAPLCHAR         lpMemArg;         // Ptr to arg global memory

    // Lock the memory to get a ptr to it
    lpMemHdrArg = MyGlobalLockVar (hGlbArg);

    // Skip over the header and dimensions to the data
    lpMemArg = VarArrayDataFmBase (lpMemHdrArg);

    // Copy the data
    CopyMemoryW (lpMemRes, lpMemArg, 26);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;
} // End MakeQuadCSRow


//***************************************************************************
//  End of File: qf_cs.c
//***************************************************************************
