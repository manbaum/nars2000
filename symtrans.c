//***************************************************************************
//  NARS2000 -- Symbol Translation Routine & Tables
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2008 Sudley Place Software

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

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $FcnTrans
//
//  Return the index corresponding to the UTF16_xxx function
//***************************************************************************

UINT FcnTrans
    (WCHAR wchFcn)          // Function symbol

{
    // Split cases based upon the function symbol
    switch (wchFcn)
    {
////////case UTF16_DOWNSTILE:               // 0x230A
////////    return (UCHAR) wchFcn;          // Use the low-order byte

        case UTF16_EPSILON:                 // 0x220A
            return INDEX_EPSILON;           // Use special index

////////case UTF16_STAR:                    // 0x002A
////////    return (UCHAR) wchFcn;          // Use the low-order byte

        case UTF16_DOWNSHOE:                // 0x222A
            return INDEX_DOWNSHOE;          // Use special index

////////case UTF16_EQUAL:                   // 0x003D
////////    return (UCHAR) wchFcn;          // Use the low-order byte

        case UTF16_CIRCLESTILE:             // 0x233D
            return INDEX_CIRCLESTILE;       // Use special index

////////case UTF16_QUERY:                   // 0x003F
////////    return (UCHAR) wchFcn;          // Use the low-order byte

        case UTF16_SLASHBAR:                // 0x233F
            return INDEX_SLASHBAR;          // Use special index

////////case UTF16_EQUALUNDERBAR:           // 0x2261
////////    return (UCHAR) wchFcn;          // Use the low-order byte

////////case UTF16_LEFTCARETUNDERBAR:       // 0x2264
////////    return (UCHAR) wchFcn;          // Use the low-order byte

////////case UTF16_RIGHTCARETUNDERBAR:      // 0x2265
////////    return (UCHAR) wchFcn;          // Use the low-order byte

        default:                            // All others
            return (UCHAR) wchFcn;          // Use the low-order byte
    } // End SWITCH
} // End FcnTrans


//***************************************************************************
//  $OprTrans
//
//  Return the index corresponding to the UTF16_xxx operator
//***************************************************************************

UINT OprTrans
    (WCHAR wchOpr)          // Operator symbol

{
    // Split cases based upon the operator symbol
    switch (wchOpr)
    {
        case UTF16_SLASH:                   // 0x002F
            return INDEX_OPSLASH;           // Use special index

        case UTF16_SLASHBAR:                // 0x233F
            return INDEX_OPSLASHBAR;        // Use special index

        case UTF16_SLOPE:                   // 0x005C
            return INDEX_OPSLOPE;           // Use special index

        case UTF16_SLOPEBAR:                // 0x2340
            return INDEX_OPSLOPEBAR;        // Use special index

        case UTF16_DIERESISDOWNTACK:        // 0x2361
            return INDEX_DIERESISDOWNTACK;  // Use special index

        case UTF16_DIERESISJOT:             // 0x2364
            return INDEX_DIERESISJOT;       // Use special index

        case UTF16_JOTDOT:                  // 0x0001
            return INDEX_JOTDOT;            // Use special index

        default:                            // All others
            return (UCHAR) wchOpr;          // Use the low-order byte
    } // End SWITCH
} // End OprTrans


//***************************************************************************
//  $SymTrans
//
//  Return the index corresponding to the UTF16_xxx symbol
//***************************************************************************

UINT SymTrans
    (LPTOKEN lptkFunc)              // Ptr to function token

{
    WCHAR        wch;               // The symbol
    HGLOBAL      hGlbFcn = NULL;    // Function array global memory handle
    LPPL_YYSTYPE lpMemFcn;          // Ptr to function array global memory
    UINT         uRet;              // Result

    // Split cases based upon the function token type
    switch (lptkFunc->tkFlags.TknType)
    {
        case TKT_FCNIMMED:
        case TKT_OP1IMMED:
        case TKT_OP2IMMED:
        case TKT_OP3IMMED:
        case TKT_OPJOTDOT:
            // This case works because TOKEN is the first element of PL_YYSTYPE
            lpMemFcn = (LPPL_YYSTYPE) lptkFunc;

            break;

        case TKT_FCNARRAY:
            // Get the function array global memory handle
            hGlbFcn = ClrPtrTypeDirAsGlb (lptkFunc->tkData.tkGlbData);

            // Lock the memory to get a ptr to it
            lpMemFcn = MyGlobalLock (hGlbFcn);

            // Skip over the header to the data (PL_YYSTYPEs)
            lpMemFcn = FcnArrayBaseToData (lpMemFcn);

            break;

        defstop
            break;
    } // End SWITCH

    // Get the symbol of the primary function/operator
    wch = lpMemFcn->tkToken.tkData.tkChar;

    // Split cases based upon the token type
    switch (lpMemFcn->tkToken.tkFlags.TknType)
    {
        case TKT_FCNIMMED:
            uRet = FcnTrans (wch);

            break;

        case TKT_OP1IMMED:
        case TKT_OP2IMMED:
        case TKT_OP3IMMED:
        case TKT_OPJOTDOT:
            uRet = OprTrans (wch);

            break;

        defstop
            uRet = 0;

            break;
    } // End SWITCH

    // If we locked a function array, unlock it now
    if (hGlbFcn)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbFcn); lpMemFcn = NULL;
    } // End IF

    return uRet;
} // End SymTrans


//***************************************************************************
//  End of File: symtrans.c
//***************************************************************************
