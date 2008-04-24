//***************************************************************************
//  NARS2000 -- System Function -- Quad EA
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
//  $SysFnEA_EM_YY
//
//  System function:  []EA -- Execute Alternate
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnEA_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnEA_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token  (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
        goto SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonEA_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydEA_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnEA_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonEA_EM_YY
//
//  Monadic []EA -- error
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonEA_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonEA_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc);
} // End SysMonEA_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydEA_EM_YY
//
//  Dyadic []EA -- Execute Alternate
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydEA_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydEA_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnNonceError_EM (lptkFunc);













} // End SysDydEA_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_ea.c
//***************************************************************************
