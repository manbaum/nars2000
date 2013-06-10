//***************************************************************************
//  NARS2000 -- System Function -- Quad VR
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2013 Sudley Place Software

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
//  $SysFnVR_EM_YY
//
//  System function:  []VR -- Visual Representation
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnVR_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnVR_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonVR_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydVR_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnVR_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonVR_EM_YY
//
//  Monadic []VR -- Visual Representation
//***************************************************************************

LPPL_YYSTYPE SysFnMonVR_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    HGLOBAL      hGlbMFO;           // Magic function/operator global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    Assert (lptkAxis EQ NULL);

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the magic function/operator global memory handle
    hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MonVR];

    //  Use an internal magic function/operator.
    return
      ExecuteMagicFunction_EM_YY (NULL,         // Ptr to left arg token
                                  lptkFunc,     // Ptr to function token
                                  NULL,         // Ptr to function strand
                                  lptkRhtArg,   // Ptr to right arg token
                                  lptkAxis,     // Ptr to axis token
                                  hGlbMFO,      // Magic function/operator global memory handle
                                 &lpMemPTD->htsMFO_MonVR, // Ptr to HSHTAB struc (may be NULL)
                                  LINENUM_ONE); // Starting line # type (see LINE_NUMS)
} // End SysFnMonVR_EM_YY


//***************************************************************************
//  Magic function/operator for Monadic []VR
//
//  Monadic []VR -- Visual Represenation
//
//  Return a visual representation of a user-defined function/operator
//***************************************************************************

static APLCHAR MonHeaderVR[] =
  L"Z" $IS MFON_MonVR L" R";

static APLCHAR MonLineVR1[] =
  L"Z" $IS L"1 ##." $QUAD L"CR R";

static APLCHAR MonLineVR2[] =
  $GOTO L"(0" $NOTEQUAL $RHO L"Z)/L1 " $DIAMOND L" Z" $IS L"'' " $DIAMOND L" " $GOTO L"0";

static APLCHAR MonLineVR3[] =
  L"L1:" $GOTO L"(1=" $MATCH L"Z)/0";

static APLCHAR MonLineVR4[] =
  L"Z" $IS L"(" $EPSILON L"((" $ENCLOSE L"[1]'LO<    " $DEL L">P<[>Q<]>I6' " $QUAD L"FMT " $IOTA $RHO L"Z),"
  $EACH L"Z)," $EACH $QUAD L"TCNL" L"),'    " $DEL L" ',(" $FORMAT L"2 ##." $QUAD L"AT R), ' (UTC)'";

static LPAPLCHAR MonBodyVR[] =
{MonLineVR1,
 MonLineVR2,
 MonLineVR3,
 MonLineVR4,
};

MAGIC_FCNOPR MFO_MonVR =
{MonHeaderVR,
 MonBodyVR,
 countof (MonBodyVR),
};


//***************************************************************************
//  $SysFnDydVR_EM_YY
//
//  Dyadic []VR -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydVR_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydVR_EM_YY
    (LPTOKEN lptkLftArg,                    // Ptr to left arg token
     LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis)                      // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnDydVR_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_vr.c
//***************************************************************************
