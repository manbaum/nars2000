//***************************************************************************
//  NARS2000 -- Primitive Operator -- DieresisCircle
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
#include "pertab.h"
#include "execmfn.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $PrimOpDieresisCircle_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    monadic operator DieresisCircle ("rank" and "rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpDieresisCircle_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_DIERESISCIRCLE
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_DIERESISCIRCLE);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (&lpYYFcnStrOpr->tkToken);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonDieresisCircle_EM_YY (lpYYFcnStrOpr,// Ptr to operator function strand
                                              lptkRhtArg);  // Ptr to right arg token
    else
        return PrimOpDydDieresisCircle_EM_YY (lptkLftArg,   // Ptr to left arg token
                                              lpYYFcnStrOpr,// Ptr to operator function strand
                                              lptkRhtArg);  // Ptr to right arg token
} // End PrimOpDieresisCircle_EM_YY


//***************************************************************************
//  $PrimProtoOpDieresisCircle_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator DieresisCircle ("rank" and "rank")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpDieresisCircle_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis)             // Ptr to axis token always NULL)

{
    Assert (lptkAxis EQ NULL);

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return PrimOpMonDieresisCircleCommon_EM_YY (lpYYFcnStrOpr,  // Ptr to operator function strand
                                                    lptkRhtArg,     // Ptr to right arg token
                                                    TRUE);          // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return PrimOpDydDieresisCircleCommon_EM_YY (lptkLftArg,     // Ptr to left arg token
                                                    lpYYFcnStrOpr,  // Ptr to operator function strand
                                                    lptkRhtArg,     // Ptr to right arg token
                                                    TRUE);          // TRUE iff prototyping
} // End PrimProtoOpDieresisCircle_EM_YY


//***************************************************************************
//  $PrimOpMonDieresisCircle_EM_YY
//
//  Primitive operator for monadic derived function from DieresisCircle ("rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonDieresisCircle_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpMonDieresisCircleCommon_EM_YY (lpYYFcnStrOpr,  // Ptr to operator function strand
                                                lptkRhtArg,     // Ptr to right arg token
                                                FALSE);         // TRUE iff prototyping
} // End PrimOpDieresisCircle_EM_YY


//***************************************************************************
//  $PrimOpMonDieresisCircleCommon_EM_YY
//
//  Primitive operator for monadic derived function from DieresisCircle ("rank")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonDieresisCircleCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpMonDieresisCircleCommon_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     BOOL         bPrototyping)         // TRUE iff protoyping

{
    HGLOBAL      hGlbPTD,               // PerTabData global memory handle
                 hGlbMF;                // Magic function global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory

    // Check for missing axis
    if (NULL EQ CheckAxisOper (lpYYFcnStrOpr))
        goto AXIS_EXIT;

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the magic function global memory handle
    hGlbMF = lpMemPTD->hGlbMF_MonRankAxis;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return
      ExecuteMagicFunction_EM_YY (NULL,                     // Ptr to left arg token
                                 &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                  lpYYFcnStrOpr,            // Ptr to function strand
                                  lptkRhtArg,               // Ptr to right arg token
                                  NULL,                     // Ptr to axis token
                                  hGlbMF,                   // Magic function global memory handle
                                  bPrototyping
                                ? LINENUM_PROTOTYPE
                                : LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
AXIS_EXIT:
    ErrorMessageIndirectToken (ERRMSG_AXIS_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    return NULL;
} // End PrimOpMonDieresisCircleCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  Magic function for monadic derived function from the rank monadic operator
//***************************************************************************

static APLCHAR MonHeader[] =
  $Z $IS L"(" $LO L" " $F L"[" $X L"]) " $R;

static APLCHAR MonLine1[] =
  $Z $IS L"(" $LO $RANK $X L") " $R;

static LPAPLCHAR MonBody[] =
{MonLine1,
};

MAGIC_FUNCTION MF_MonRankAxis =
{MonHeader,
 MonBody,
 sizeof (MonBody) / sizeof (MonBody[0]),
};


//***************************************************************************
//  $PrimOpDydDieresisCircle_EM_YY
//
//  Primitive operator for dyadic derived function from DieresisCircle ("rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydDieresisCircle_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpDydDieresisCircleCommon_EM_YY (lptkLftArg,     // Ptr to left arg token
                                                lpYYFcnStrOpr,  // Ptr to operator function strand
                                                lptkRhtArg,     // Ptr to right arg token
                                                FALSE);         // TRUE iff prototyping
} // End PrimOpDydDieresisCircle_EM_YY


//***************************************************************************
//  $PrimOpDydDieresisCircleCommon_EM_YY
//
//  Primitive operator for dyadic derived function from DieresisCircle ("rank")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydDieresisCircleCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDydDieresisCircleCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     BOOL         bPrototyping)         // TRUE iff prototyping

{
    HGLOBAL      hGlbPTD,               // PerTabData global memory handle
                 hGlbMF;                // Magic function global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory

    // Check for missing axis
    if (NULL EQ CheckAxisOper (lpYYFcnStrOpr))
        goto AXIS_EXIT;

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the magic function global memory handle
    hGlbMF = lpMemPTD->hGlbMF_DydRankAxis;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return
      ExecuteMagicFunction_EM_YY (lptkLftArg,               // Ptr to left arg token
                                 &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                  lpYYFcnStrOpr,            // Ptr to function strand
                                  lptkRhtArg,               // Ptr to right arg token
                                  NULL,                     // Ptr to axis token
                                  hGlbMF,                   // Magic function global memory handle
                                  bPrototyping
                                ? LINENUM_PROTOTYPE
                                : LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
AXIS_EXIT:
    ErrorMessageIndirectToken (ERRMSG_AXIS_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    return NULL;
} // End PrimOpDydDieresisCircle_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  Magic function for dyadic derived function from the rank monadic operator
//***************************************************************************

static APLCHAR DydHeader[] =
  $Z $IS $L L" (" $LO L" " $F L"[" $X L"]) " $R;

static APLCHAR DydLine1[] =
  $Z $IS $L L" (" $LO $RANK $X L") " $R;

static LPAPLCHAR DydBody[] =
{DydLine1,
};

MAGIC_FUNCTION MF_DydRankAxis =
{DydHeader,
 DydBody,
 sizeof (DydBody) / sizeof (DydBody[0]),
};


//***************************************************************************
//  End of File: po_dicircle.c
//***************************************************************************
